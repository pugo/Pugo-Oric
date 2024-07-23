// =========================================================================
//   Copyright (C) 2009-2024 by Anders Piniesjö <pugo@pugo.org>
//
//   This program is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
//   This program is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.
//
//   You should have received a copy of the GNU General Public License
//   along with this program.  If not, see <http://www.gnu.org/licenses/>
// =========================================================================

#include "mos6502.hpp"
#include "mos6502_opcodes.hpp"
#include "mos6502_cycles.hpp"
#include <machine.hpp>

#include <iostream>
#include <cstdio>

// Macros for addressing modes
#define READ_BYTE_IMM()     memory_read_byte_handler(machine, PC++)

// Read addresses
#define READ_ADDR_ZP()      (READ_BYTE_IMM())
#define READ_ADDR_ZP_X()    ((READ_BYTE_IMM() + X) & 0xff)
#define READ_ADDR_ZP_Y()    ((READ_BYTE_IMM() + Y) & 0xff)

#define READ_ADDR_ABS()    addr = READ_BYTE_IMM(); addr += READ_BYTE_IMM() << 8
#define READ_ADDR_ABS_X()   READ_ADDR_ABS(); addr += X
#define READ_ADDR_ABS_Y()   READ_ADDR_ABS(); addr += Y

// Direct assigning versions with page check
#define PAGECHECK(n) (((addr + n) & 0xff00) != (addr & 0xff00))
#define PAGECHECK2(a, b) ((a & 0xff00) != (b & 0xff00))

#define READ_ADDR_IND_X()   (memory_read_word_zp_handler(machine, READ_BYTE_IMM() + X))
#define READ_ADDR_IND_Y()   (memory_read_word_zp_handler(machine, READ_BYTE_IMM()) + Y)

#define READ_JUMP_ADDR()    (b1 = READ_BYTE_IMM(), b1 & 0x80 ? (PC - ((b1 ^ 0xff)+1)) : (PC + b1))
#define PEEK_JUMP_ADDR(pc)    (b1 = memory_read_byte_handler(machine, pc+1), b1 & 0x80 ? (pc + 2 - ((b1 ^ 0xff)+1)) : (pc + 2 + b1))

// Read data
#define READ_BYTE_ZP()      memory_read_byte_zp_handler(machine, READ_ADDR_ZP())
#define READ_BYTE_ZP_X()    memory_read_byte_zp_handler(machine, READ_ADDR_ZP_X())
#define READ_BYTE_ZP_Y()    memory_read_byte_zp_handler(machine, READ_ADDR_ZP_Y())

#define READ_BYTE_ABS(b)   READ_ADDR_ABS(); b = memory_read_byte_handler(machine, addr)

#define READ_BYTE_IND_X()   memory_read_byte_handler(machine, READ_ADDR_IND_X())
#define READ_BYTE_IND_Y()   memory_read_byte_handler(machine, READ_ADDR_IND_Y())

#define PUSH_BYTE_STACK(b)  (memory.mem[STACK_BOTTOM | (SP--)] = (b))
#define POP_BYTE_STACK()    (memory.mem[STACK_BOTTOM | (++SP)])

// Macros for flag handling
#define SET_FLAG_NZ(B)     (N_INTERN = Z_INTERN = B)

#define Z	(!Z_INTERN)
#define N	(!!(N_INTERN & 0x80))


MOS6502::MOS6502(Machine& a_Machine) :
    A(0),
    X(0),
    Y(0),
    N_INTERN(0),
    Z_INTERN(0),
    V(false),
    B(false),
    D(false),
    I(false),
    C(false),
    PC(0),
    SP(0),
    quiet(false),
    machine(a_Machine),
    irq_flag(false),
    do_interrupt(false),
    memory_read_byte_handler(nullptr),
    memory_read_byte_zp_handler(nullptr),
    memory_read_word_handler(nullptr),
    memory_read_word_zp_handler(nullptr),
    memory_write_byte_handler(nullptr),
    memory_write_byte_zp_handler(nullptr),
    memory(a_Machine.memory),
    monitor(memory)
{
}


void MOS6502::Reset()
{
    A = 0;
    X = 0;
    Y = 0;
    N_INTERN = 0;
    Z_INTERN = 0;
    V = false;
    B = false;
    D = false;
    I = true;	// Block interrupts after reset.
    C = false;

    PC = memory_read_byte_handler(machine, RESET_VECTOR_L) + (memory_read_byte_handler(machine, RESET_VECTOR_H) << 8);
    SP = 0xff;
    irq_flag = false;
    do_interrupt = false;
}

void MOS6502::PrintStat()
{
    PrintStat(PC);
}

void MOS6502::PrintStat(uint16_t address)
{
    std::cout << monitor.disassemble(address) << " ";
    printf("A: %02X, X: %02X, Y: %02X  |  N: %d, Z: %d, C: %d, V: %d  |  SP: %02X\n", A, X, Y, N, Z, C, V, SP);
}

//   7                           0
// +---+---+---+---+---+---+---+---+
// | N | V |   | B | D | I | Z | C |
// +---+---+---+---+---+---+---+---+
uint8_t MOS6502::get_p()
{
    uint8_t result = 0;
    result |= N ? FLAG_N : 0;
    result |= V ? FLAG_V : 0;
    result |= B ? FLAG_B : 0;
    result |= D ? FLAG_D : 0;
    result |= I ? FLAG_I : 0;
    result |= Z ? FLAG_Z : 0;
    result |= C ? FLAG_C : 0;
    return result;
}

void MOS6502::set_p(uint8_t p)
{
    N_INTERN = (p & FLAG_N) ? FLAG_N : 0;
    V = !! (p & FLAG_V);
    B = !! (p & FLAG_B);
    D = !! (p & FLAG_D);
    I = !! (p & FLAG_I);
    Z_INTERN = (p & FLAG_Z) ? 0 : 1;
    C = !! (p & FLAG_C);
}

void MOS6502::NMI()
{
    std::cout << "NMI interrupt" << std::endl;
    PUSH_BYTE_STACK(PC >> 8);
    PUSH_BYTE_STACK(PC);
    PUSH_BYTE_STACK(get_p());
    PC = memory_read_word_handler(machine, NMI_VECTOR_L);
    I = true;
}

void MOS6502::irq()
{
    irq_flag = true;
}

void MOS6502::irq_clear()
{
    irq_flag = false;
}

bool MOS6502::handle_irq()
{
    irq_flag = false;
    if (I) { // Interrupt disabled ?
        return false;
    }

    PUSH_BYTE_STACK(PC >> 8);
    PUSH_BYTE_STACK(PC & 0xff);
    PUSH_BYTE_STACK(get_p());
    I = true;
    PC = memory_read_word_handler(machine, IRQ_VECTOR_L);

    return true;
}

int inline MOS6502::signed_byte_to_int(uint8_t b)
{
    if (b < 0x80) {
        return b;
    }
    b ^= 0xff;
    b += 1;
    return -b;
}

void MOS6502::ADC(uint8_t a_Val)
{
    if (D) { // Decimal mode
        uint16_t low = (A & 0x0f) + (a_Val & 0x0f) + (C ? 1 : 0);
        if (low > 9) low += 6;    // 11 + 6 = (0xb + 6) = 0x11
        uint16_t high = (A >> 4) + (a_Val >> 4) + (low > 0x0f);	// remainder from low figure -> high

        Z_INTERN = (A + a_Val + (C ? 1 : 0)) & 0xff;
        N_INTERN = (high & 0x08) ? FLAG_N : 0;
        V = ~(A ^ a_Val) & (A ^ (high << 4)) & 0x80;

        if (high > 9) high += 6;  // 11 + 6 = (0xb + 6) = 0x11
        C = high > 0x0f;
        A = (high << 4) | (low & 0x0f);
    }
    else { // Normal mode
        uint16_t w = A + a_Val + (C ? 1 : 0);
        C = w > 0xff;
        V = ~(A ^ a_Val) & (A ^ w) & 0x80;
        SET_FLAG_NZ(A = w);
    }
}

void MOS6502::SBC(uint8_t a_Val)
{
    if (D) {
        uint16_t low = (A & 0x0f) - (a_Val & 0x0f) - (C ? 0 : 1);
        uint16_t high = (A >> 4) - (a_Val >> 4);

        if (low & 0x10) { // Fix negative
            low -= 6;
            --high;
        }
        if (high & 0x10) { // Fix negative
            high -= 6;
        }

        uint16_t w = SET_FLAG_NZ(A - a_Val - (C ? 0 : 1));
        C = w < 0x100;
        V = (A ^ a_Val) & (A ^ w) & 0x80;
        A = (high << 4) | (low & 0x0f);
    }
    else {
        uint16_t w = A - a_Val - (C ? 0 : 1);
        C = w < 0x100;
        V = (A ^ a_Val) & (A ^ w) & 0x80;
        SET_FLAG_NZ(A = w);
    }

    //std::cout << "A now: " << hex << A << std::std::endl;
}


#define PEEK_ADDR_ABS()     (memory_read_byte_handler(machine, _pc + 1) | memory_read_byte_handler(machine, _pc + 2) << 8)


uint8_t MOS6502::time_instruction()
{
    uint16_t _pc = PC;
    uint8_t extra = 0;

    if (irq_flag && !I) {
        _pc = memory_read_word_handler(machine, IRQ_VECTOR_L);
        extra += 7;
        do_interrupt = true;
    }

    uint8_t b1;
    uint16_t addr;
    uint8_t instruction = memory_read_byte_handler(machine, _pc);

    switch(instruction)
    {
        case LDA_ABS_X:
        case LDY_ABS_X:
        case ADC_ABS_X:
        case SBC_ABS_X:
        case AND_ABS_X:
        case ORA_ABS_X:
        case EOR_ABS_X:
        case CMP_ABS_X:
            addr = PEEK_ADDR_ABS();
            extra += PAGECHECK(X) ? 1 : 0;
            break;

        case LDA_ABS_Y:
        case LDX_ABS_Y:
        case ADC_ABS_Y:
        case SBC_ABS_Y:
        case AND_ABS_Y:
        case ORA_ABS_Y:
        case EOR_ABS_Y:
        case CMP_ABS_Y:
            addr = PEEK_ADDR_ABS();
            extra += PAGECHECK(Y) ? 1 : 0;
            break;

        case LDA_IND_Y:
        case ADC_IND_Y:
        case SBC_IND_Y:
        case AND_IND_Y:
        case ORA_IND_Y:
        case EOR_IND_Y:
        case CMP_IND_Y:
            addr = memory_read_word_zp_handler(machine, memory_read_byte_handler(machine, _pc + 1));
            extra += PAGECHECK(Y) ? 1 : 0;
            break;


        case BCC:
            if (!C) {
                addr = PEEK_JUMP_ADDR(_pc);
                extra += PAGECHECK2(addr, (_pc + 2)) ? 2 : 1;
            }
            break;
        case BCS:
            if (C) {
                addr = PEEK_JUMP_ADDR(_pc);
                extra += PAGECHECK2(addr, (_pc + 2)) ? 2 : 1;
            }
            break;
        case BEQ:
            if (Z) {
                addr = PEEK_JUMP_ADDR(_pc);
                extra += PAGECHECK2(addr, (_pc + 2)) ? 2 : 1;
            }
            break;
        case BNE:
            if (!Z) {
                addr = PEEK_JUMP_ADDR(_pc);
                extra += PAGECHECK2(addr, (_pc + 2)) ? 2 : 1;
            }
            break;

        case BMI:
            if (N) {
                addr = PEEK_JUMP_ADDR(_pc);
                extra += PAGECHECK2(addr, (_pc + 2)) ? 2 : 1;
            }
            break;

        case BPL:
            if (!N) {
                addr = PEEK_JUMP_ADDR(_pc);
                extra += PAGECHECK2(addr, (_pc + 2)) ? 2 : 1;
            }
            break;

        case BVC:
            if (!V) {
                addr = PEEK_JUMP_ADDR(_pc);
                extra += PAGECHECK2(addr, (_pc + 2)) ? 2 : 1;
            }
            break;

        case BVS:
            if (V) {
                addr = PEEK_JUMP_ADDR(_pc);
                extra += PAGECHECK2(addr, (_pc + 2)) ? 2 : 1;
            }
            break;
        default:
            break;
    }

    return opcode_cycles[instruction] + extra;
}


void MOS6502::exec_instruction(bool& a_Brk)
{
    if (do_interrupt) {
        do_interrupt = false;
        irq_flag = false;

        PUSH_BYTE_STACK(PC >> 8);
        PUSH_BYTE_STACK(PC & 0xff);
        PUSH_BYTE_STACK(get_p());
        I = true;
        PC = memory_read_word_handler(machine, IRQ_VECTOR_L);
    }

    uint8_t b1, b2;
    uint16_t addr;
    int i;

    uint16_t pc_initial = PC;
    uint8_t instruction = READ_BYTE_IMM();

    switch(instruction)
    {
        case LDA_IMM:
            SET_FLAG_NZ(A = READ_BYTE_IMM());
            break;
        case LDA_ZP:
            SET_FLAG_NZ(A = READ_BYTE_ZP());
            break;
        case LDA_ZP_X:
            SET_FLAG_NZ(A = READ_BYTE_ZP_X());
            break;
        case LDA_ABS:
            READ_BYTE_ABS(A);
            SET_FLAG_NZ(A);
            break;
        case LDA_ABS_X:
            READ_ADDR_ABS_X();
            SET_FLAG_NZ(A = memory_read_byte_handler(machine, addr));
            break;
        case LDA_ABS_Y:
            READ_ADDR_ABS_Y();
            SET_FLAG_NZ(A = memory_read_byte_handler(machine, addr));
            break;
        case LDA_IND_X:
            SET_FLAG_NZ(A = READ_BYTE_IND_X());
            break;
        case LDA_IND_Y:
            addr = READ_ADDR_IND_Y();
            SET_FLAG_NZ(A = memory_read_byte_handler(machine, addr));
            break;

        case LDX_IMM:
            SET_FLAG_NZ(X = READ_BYTE_IMM());
            break;
        case LDX_ZP:
            SET_FLAG_NZ(X = READ_BYTE_ZP());
            break;
        case LDX_ZP_Y:
            SET_FLAG_NZ(X = READ_BYTE_ZP_Y());
            break;
        case LDX_ABS:
            READ_BYTE_ABS(b1);
            SET_FLAG_NZ(X = b1);
            break;
        case LDX_ABS_Y:
            READ_ADDR_ABS_Y();
            SET_FLAG_NZ(X = memory_read_byte_handler(machine, addr));
            break;

            
        case LDY_IMM:
            SET_FLAG_NZ(Y = READ_BYTE_IMM());
            break;
        case LDY_ZP:
            SET_FLAG_NZ(Y = READ_BYTE_ZP());
            break;
        case LDY_ZP_X:
            SET_FLAG_NZ(Y = READ_BYTE_ZP_X());
            break;
        case LDY_ABS:
            READ_BYTE_ABS(b1);
            SET_FLAG_NZ(Y = b1);
            break;
        case LDY_ABS_X:
            READ_ADDR_ABS_X();
            SET_FLAG_NZ(Y = memory_read_byte_handler(machine, addr));
            break;

        case STA_ZP:
            memory_write_byte_handler(machine, READ_ADDR_ZP(), A);
            break;
        case STA_ZP_X:
            memory_write_byte_handler(machine, READ_ADDR_ZP_X(), A);
            break;
        case STA_ABS:
            READ_ADDR_ABS();
            memory_write_byte_handler(machine, addr, A);
            break;
        case STA_ABS_X:
            READ_ADDR_ABS_X();
            memory_write_byte_handler(machine, addr, A);
            break;
        case STA_ABS_Y:
            READ_ADDR_ABS_Y();
            memory_write_byte_handler(machine, addr, A);
            break;
        case STA_IND_X:
            memory_write_byte_handler(machine, READ_ADDR_IND_X(), A);
            break;
        case STA_IND_Y:
            memory_write_byte_handler(machine, READ_ADDR_IND_Y(), A);
            break;

        case STX_ZP:
            memory_write_byte_handler(machine, READ_ADDR_ZP(), X);
            break;
        case STX_ZP_Y:
            memory_write_byte_handler(machine, READ_ADDR_ZP_Y(), X);
            break;
        case STX_ABS:
            READ_ADDR_ABS();
            memory_write_byte_handler(machine, addr, X);
            break;

        case STY_ZP:
            memory_write_byte_handler(machine, READ_ADDR_ZP(), Y);
            break;
        case STY_ZP_X:
            memory_write_byte_handler(machine, READ_ADDR_ZP_X(), Y);
            break;
        case STY_ABS:
            READ_ADDR_ABS();
            memory_write_byte_handler(machine, addr, Y);
            break;

            // ADD to accumulator with carry
        case ADC_IMM:
            ADC(READ_BYTE_IMM());
            break;
        case ADC_ZP:
            ADC(READ_BYTE_ZP());
            break;
        case ADC_ZP_X:
            ADC(READ_BYTE_ZP_X());
            break;
        case ADC_ABS:
            READ_BYTE_ABS(b1);
            ADC(b1);
            break;
        case ADC_ABS_X:
            READ_ADDR_ABS_X();
            ADC(memory_read_byte_handler(machine, addr));
            break;
        case ADC_ABS_Y:
            READ_ADDR_ABS_Y();
            ADC(memory_read_byte_handler(machine, addr));
            break;
        case ADC_IND_X:
            ADC(READ_BYTE_IND_X());
            break;
        case ADC_IND_Y:
            addr = READ_ADDR_IND_Y();
            ADC(memory_read_byte_handler(machine, addr));
            break;

            // Subtract from accumulator with borrow
        case SBC_IMM:
            SBC(READ_BYTE_IMM());
            break;
        case SBC_ZP:
            SBC(READ_BYTE_ZP());
            break;
        case SBC_ZP_X:
            SBC(READ_BYTE_ZP_X());
            break;
        case SBC_ABS:
            READ_BYTE_ABS(b1);
            SBC(b1);
            break;
        case SBC_ABS_X:
            READ_ADDR_ABS_X();
            SBC(memory_read_byte_handler(machine, addr));
            break;
        case SBC_ABS_Y:
            READ_ADDR_ABS_Y();
            SBC(memory_read_byte_handler(machine, addr));
            break;
        case SBC_IND_X:
            SBC(READ_BYTE_IND_X());
            break;
        case SBC_IND_Y:
            addr = READ_ADDR_IND_Y();
            SBC(memory_read_byte_handler(machine, addr));
            break;

            // Increment memory by one
        case INC_ZP:
            addr = READ_ADDR_ZP();
            memory_write_byte_handler(machine, addr, SET_FLAG_NZ(memory_read_byte_handler(machine, addr) + 1));
            break;
        case INC_ZP_X:
            addr = READ_ADDR_ZP_X();
            memory_write_byte_handler(machine, addr, SET_FLAG_NZ(memory_read_byte_handler(machine, addr) + 1));
            break;
        case INC_ABS:
            READ_ADDR_ABS();
            memory_write_byte_handler(machine, addr, SET_FLAG_NZ(memory_read_byte_handler(machine, addr) + 1));
            break;
        case INC_ABS_X:
            READ_ADDR_ABS_X();
            memory_write_byte_handler(machine, addr, SET_FLAG_NZ(memory_read_byte_handler(machine, addr) + 1));
            break;

            // Decrease memory by one
        case DEC_ZP:
            addr = READ_ADDR_ZP();
            memory_write_byte_handler(machine, addr, SET_FLAG_NZ(memory_read_byte_handler(machine, addr) - 1));
            break;
        case DEC_ZP_X:
            addr = READ_ADDR_ZP_X();
            memory_write_byte_handler(machine, addr, SET_FLAG_NZ(memory_read_byte_handler(machine, addr) - 1));
            break;
        case DEC_ABS:
            READ_ADDR_ABS();
            memory_write_byte_handler(machine, addr, SET_FLAG_NZ(memory_read_byte_handler(machine, addr) - 1));
            break;
        case DEC_ABS_X:
            READ_ADDR_ABS_X();
            memory_write_byte_handler(machine, addr, SET_FLAG_NZ(memory_read_byte_handler(machine, addr) - 1));
            break;

            // Increase X by one
        case INX:
            SET_FLAG_NZ(++X);
            break;
            // Decrease X by one
        case DEX:
            SET_FLAG_NZ(--X);
            break;
            // Increase Y by one
        case INY:
            SET_FLAG_NZ(++Y);
            break;
            // Decrease Y by one
        case DEY:
            SET_FLAG_NZ(--Y);
            break;

            // And accumulator with memory
        case AND_IMM:
            SET_FLAG_NZ(A &= READ_BYTE_IMM());
            break;
        case AND_ZP:
            SET_FLAG_NZ(A &= READ_BYTE_ZP());
            break;
        case AND_ZP_X:
            SET_FLAG_NZ(A &= READ_BYTE_ZP_X());
            break;
        case AND_ABS:
            READ_BYTE_ABS(b1);
            SET_FLAG_NZ(A &= b1);
            break;
        case AND_ABS_X:
            READ_ADDR_ABS_X();
            SET_FLAG_NZ(A &= memory_read_byte_handler(machine, addr));
            break;
        case AND_ABS_Y:
            READ_ADDR_ABS_Y();
            SET_FLAG_NZ(A &= memory_read_byte_handler(machine, addr));
            break;
        case AND_IND_X:
            SET_FLAG_NZ(A &= READ_BYTE_IND_X());
            break;
        case AND_IND_Y :
            addr = READ_ADDR_IND_Y();
            SET_FLAG_NZ(A &= memory_read_byte_handler(machine, addr));
            break;

            // Or accumulator with memory
        case ORA_IMM:
            SET_FLAG_NZ(A |= READ_BYTE_IMM());
            break;
        case ORA_ZP:
            SET_FLAG_NZ(A |= READ_BYTE_ZP());
            break;
        case ORA_ZP_X:
            SET_FLAG_NZ(A |= READ_BYTE_ZP_X());
            break;
        case ORA_ABS:
            READ_BYTE_ABS(b1);
            SET_FLAG_NZ(A |= b1);
            break;
        case ORA_ABS_X:
            READ_ADDR_ABS_X();
            SET_FLAG_NZ(A |= memory_read_byte_handler(machine, addr));
            break;
        case ORA_ABS_Y:
            READ_ADDR_ABS_Y();
            SET_FLAG_NZ(A |= memory_read_byte_handler(machine, addr));
            break;
        case ORA_IND_X:
            SET_FLAG_NZ(A |= READ_BYTE_IND_X());
            break;
        case ORA_IND_Y:
            addr = READ_ADDR_IND_Y();
            SET_FLAG_NZ(A |= memory_read_byte_handler(machine, addr));
            break;

            // Exclusive or accumulator with memory
        case EOR_IMM:
            SET_FLAG_NZ(A ^= READ_BYTE_IMM());
            break;
        case EOR_ZP:
            SET_FLAG_NZ(A ^= READ_BYTE_ZP());
            break;
        case EOR_ZP_X:
            SET_FLAG_NZ(A ^= READ_BYTE_ZP_X());
            break;
        case EOR_ABS:
            READ_BYTE_ABS(b1);
            SET_FLAG_NZ(A ^= b1);
            break;
        case EOR_ABS_X:
            READ_ADDR_ABS_X();
            SET_FLAG_NZ(A ^= memory_read_byte_handler(machine, addr));
            break;
        case EOR_ABS_Y:
            READ_ADDR_ABS_Y();
            SET_FLAG_NZ(A ^= memory_read_byte_handler(machine, addr));
            break;
        case EOR_IND_X:
            SET_FLAG_NZ(A ^= READ_BYTE_IND_X());
            break;
        case EOR_IND_Y:
            addr = READ_ADDR_IND_Y();
            SET_FLAG_NZ(A ^= memory_read_byte_handler(machine, addr));
            break;

            //      +-+-+-+-+-+-+-+-+
            // C <- |7|6|5|4|3|2|1|0| <- 0              N Z C I D V
            //      +-+-+-+-+-+-+-+-+                   / / / _ _ _
        case ASL_ACC:
            C = A & 0x80;
            SET_FLAG_NZ(A <<= 1);
            break;
        case ASL_ZP:
            b1 = memory_read_byte_zp_handler(machine, addr = READ_ADDR_ZP());
            C = b1 & 0x80;
            memory_write_byte_zp_handler(machine, addr, SET_FLAG_NZ(b1 <<= 1));
            break;
        case ASL_ZP_X:
            b1 = memory_read_byte_zp_handler(machine, addr = READ_ADDR_ZP_X());
            C = b1 & 0x80;
            memory_write_byte_zp_handler(machine, addr, SET_FLAG_NZ(b1 <<= 1));
            break;
        case ASL_ABS:
            READ_ADDR_ABS();
            b1 = memory_read_byte_handler(machine, addr);
            C = b1 & 0x80;
            memory_write_byte_handler(machine, addr, SET_FLAG_NZ(b1 <<= 1));
            break;
        case ASL_ABS_X:
            READ_ADDR_ABS_X();
            b1 = memory_read_byte_handler(machine, addr);
            C = b1 & 0x80;
            memory_write_byte_handler(machine, addr, SET_FLAG_NZ(b1 <<= 1));
            break;

            //      +-+-+-+-+-+-+-+-+
            // 0 -> |7|6|5|4|3|2|1|0| -> C              N Z C I D V
            //      +-+-+-+-+-+-+-+-+                   0 / / _ _ _
        case LSR_ACC:
            C = A & 0x01;
            SET_FLAG_NZ(A >>= 1);
            break;
        case LSR_ZP:
            b1 = memory_read_byte_zp_handler(machine, addr = READ_ADDR_ZP());
            C = b1 & 0x01;
            memory_write_byte_zp_handler(machine, addr, SET_FLAG_NZ(b1 >>= 1));
            break;
        case LSR_ZP_X:
            b1 = memory_read_byte_zp_handler(machine, addr = READ_ADDR_ZP_X());
            C = b1 & 0x01;
            memory_write_byte_zp_handler(machine, addr, SET_FLAG_NZ(b1 >>= 1));
            break;
        case LSR_ABS:
            READ_ADDR_ABS();
            b1 = memory_read_byte_handler(machine, addr);
            C = b1 & 0x01;
            memory_write_byte_handler(machine, addr, SET_FLAG_NZ(b1 >>= 1));
            break;
        case LSR_ABS_X:
            READ_ADDR_ABS_X();
            b1 = memory_read_byte_handler(machine, addr);
            C = b1 & 0x01;
            memory_write_byte_handler(machine, addr, SET_FLAG_NZ(b1 >>= 1));
            break;

            // +------------------------------+
            // |         M or A               |
            // |   +-+-+-+-+-+-+-+-+    +-+   |
            // +-< |7|6|5|4|3|2|1|0| <- |C| <-+         N Z C I D V
            //     +-+-+-+-+-+-+-+-+    +-+             / / / _ _ _
        case ROL_ACC:
            b2 = A & 0x80;
            SET_FLAG_NZ(A = C ? (A<<=1) + 1 : A<<=1);
            C = b2;
            break;
        case ROL_ZP:
            b1 = memory_read_byte_zp_handler(machine, addr = READ_ADDR_ZP());
            b2 = b1 & 0x80;
            memory_write_byte_zp_handler(machine, addr, SET_FLAG_NZ(C ? (b1<<=1) + 1 : b1<<=1));
            C = b2;
            break;
        case ROL_ZP_X:
            b1 = memory_read_byte_zp_handler(machine, addr = READ_ADDR_ZP_X());
            b2 = b1 & 0x80;
            memory_write_byte_zp_handler(machine, addr, SET_FLAG_NZ(C ? (b1<<=1) + 1 : b1<<=1));
            C = b2;
            break;
        case ROL_ABS:
            READ_ADDR_ABS();
            b1 = memory_read_byte_handler(machine, addr);
            b2 = b1 & 0x80;
            memory_write_byte_handler(machine, addr, SET_FLAG_NZ(C ? (b1<<=1) + 1 : b1<<=1));
            C = b2;
            break;
        case ROL_ABS_X:
            READ_ADDR_ABS_X();
            b1 = memory_read_byte_handler(machine, addr);
            b2 = b1 & 0x80;
            memory_write_byte_handler(machine, addr, SET_FLAG_NZ(C ? (b1<<=1) + 1 : b1<<=1));
            C = b2;
            break;

            // +------------------------------+
            // |                              |
            // |   +-+    +-+-+-+-+-+-+-+-+   |
            // +-> |C| -> |7|6|5|4|3|2|1|0| >-+         N Z C I D V
            //     +-+    +-+-+-+-+-+-+-+-+             / / / _ _ _
        case ROR_ACC:
            b2 = A & 0x01;
            SET_FLAG_NZ(A = C ? (A>>=1)|0x80 : A>>=1);
            C = b2;
            break;
        case ROR_ZP:
            b1 = memory_read_byte_zp_handler(machine, addr = READ_ADDR_ZP());
            b2 = b1 & 0x01;
            memory_write_byte_zp_handler(machine, addr, SET_FLAG_NZ(C ? (b1>>=1) | 0x80 : b1>>=1));
            C = b2;
            break;
        case ROR_ZP_X:
            b1 = memory_read_byte_zp_handler(machine, addr = READ_ADDR_ZP_X());
            b2 = b1 & 0x01;
            memory_write_byte_zp_handler(machine, addr, SET_FLAG_NZ(C ? (b1>>=1) | 0x80 : b1>>=1));
            C = b2;
            break;
        case ROR_ABS:
            READ_ADDR_ABS();
            b1 = memory_read_byte_handler(machine, addr);
            b2 = b1 & 0x01;
            memory_write_byte_handler(machine, addr, SET_FLAG_NZ(C ? (b1>>=1) | 0x80 : b1>>=1));
            C = b2;
            break;
        case ROR_ABS_X:
            READ_ADDR_ABS_X();
            b1 = memory_read_byte_handler(machine, addr);
            b2 = b1 & 0x01;
            memory_write_byte_handler(machine, addr, SET_FLAG_NZ(C ? (b1>>=1) | 0x80 : b1>>=1));
            C = b2;
            break;

            // Branches
        case BCC:
            if (!C) {
                PC = READ_JUMP_ADDR();
            }
            else
                ++PC;
            break;
        case BCS:
            if (C) {
                PC = READ_JUMP_ADDR();
            }
            else
                ++PC;
            break;
        case BEQ:
            if (Z) {
                PC = READ_JUMP_ADDR();
            }
            else
                ++PC;
            break;
        case BNE:
            if (!Z) {
                PC = READ_JUMP_ADDR();
            }
            else
                ++PC;
            break;

        case BMI:
            if (N) {
                PC = READ_JUMP_ADDR();
            }
            else
                ++PC;
            break;

        case BPL:
            if (!N) {
                PC = READ_JUMP_ADDR();
            }
            else
                ++PC;
            break;

        case BVC:
            if (!V) {
                PC = READ_JUMP_ADDR();
            }
            else
                ++PC;
            break;

        case BVS:
            if (V) {
                PC = READ_JUMP_ADDR();
            }
            else
                ++PC;
            break;

        case BIT_ZP:
            b1 = READ_BYTE_ZP();
            N_INTERN = b1;
            Z_INTERN = A & b1;
            V = b1 & FLAG_V;  // bit 6 -> V
            break;

        case BIT_ABS:
            READ_BYTE_ABS(b1);
            N_INTERN = b1;
            Z_INTERN = A & b1;
            V = b1 & FLAG_V;  // bit 6 -> V
            break;

        case SEC: // Set carry flag
            C = true;
            break;
        case SED: // Set decimal flag
            D = true;
            break;
        case SEI: // Set interrupt flag
            I = true;
            break;

        case CLC: // Clear carry flag
            C = false;
            break;
        case CLD: // Clear decimal flag
            D = false;
            break;
        case CLI: // Clear interrupt flag
            I = false;
            break;
        case CLV: // Clear overflow flag
            V = false;
            break;

        case CMP_IMM:
            i = A - READ_BYTE_IMM();
            C = i >= 0;
            SET_FLAG_NZ((uint8_t)i);
            break;
        case CMP_ZP:
            i = A - READ_BYTE_ZP();
            C = i >= 0;
            SET_FLAG_NZ((uint8_t)i);
            break;
        case CMP_ZP_X:
            i = A - READ_BYTE_ZP_X();
            C = i >= 0;
            SET_FLAG_NZ((uint8_t)i);
            break;
        case CMP_ABS:
            READ_BYTE_ABS(b1);
            i = A - b1;
            C = i >= 0;
            SET_FLAG_NZ((uint8_t)i);
            break;
        case CMP_ABS_X:
            READ_ADDR_ABS_X();
            i = A - memory_read_byte_handler(machine, addr);
            C = i >= 0;
            SET_FLAG_NZ((uint8_t)i);
            break;
        case CMP_ABS_Y:
            READ_ADDR_ABS_Y();
            i = A - memory_read_byte_handler(machine, addr);
            C = i >= 0;
            SET_FLAG_NZ((uint8_t)i);
            break;
        case CMP_IND_X:
            i = A - READ_BYTE_IND_X();
            C = i >= 0;
            SET_FLAG_NZ((uint8_t)i);
            break;
        case CMP_IND_Y:
            addr = READ_ADDR_IND_Y();
            i = A - memory_read_byte_handler(machine, addr);
            C = i >= 0;
            SET_FLAG_NZ((uint8_t)i);
            break;

        case CPX_IMM:
            i = X - READ_BYTE_IMM();
            C = i >= 0;
            SET_FLAG_NZ((uint8_t)i);
            break;
        case CPX_ZP:
            i = X - READ_BYTE_ZP();
            C = i >= 0;
            SET_FLAG_NZ((uint8_t)i);
            break;
        case CPX_ABS:
            READ_BYTE_ABS(b1);
            i = X - b1;
            C = i >= 0;
            SET_FLAG_NZ((uint8_t)i);
            break;

        case CPY_IMM:
            i = Y - READ_BYTE_IMM();
            C = i >= 0;
            SET_FLAG_NZ((uint8_t)i);
            break;
        case CPY_ZP:
            i = Y - READ_BYTE_ZP();
            C = i >= 0;
            SET_FLAG_NZ((uint8_t)i);
            break;
        case CPY_ABS:
            READ_BYTE_ABS(b1);
            i = Y - b1;
            C = i >= 0;
            SET_FLAG_NZ((uint8_t)i);
            break;

        case JMP_ABS:
            READ_ADDR_ABS();
            PC = addr;
            break;
        case JMP_IND:
            READ_ADDR_ABS();
            PC = memory_read_word_handler(machine, addr);
            break;

        case JSR:
            PUSH_BYTE_STACK((PC+1) >> 8); // Store 1 before next instruction
            PUSH_BYTE_STACK((PC+1) & 0xff);
            READ_ADDR_ABS();
            PC = addr;
            break;

        case RTS:
            PC = POP_BYTE_STACK();
            PC += (POP_BYTE_STACK() << 8) + 1;
            break;

        case BRK:
            PUSH_BYTE_STACK((PC+1) >> 8); // Byte after BRK will not be executed on return!
            PUSH_BYTE_STACK(PC+1);
            PUSH_BYTE_STACK(get_p() | FLAG_B);
            I = true;
            D = false;
            PC = memory_read_word_handler(machine, IRQ_VECTOR_L);
            a_Brk = true;
            break;

        case RTI:  // Return from interrupt
            set_p(POP_BYTE_STACK()); //  & 0xdb);
            PC = POP_BYTE_STACK();
            PC += (POP_BYTE_STACK() << 8);
            break;

        case NOP:
            break;

        case PHA:  // Push accumulator to stack
            PUSH_BYTE_STACK(A);
            break;
        case PLA:  // Pull accumulator from stack
            A = POP_BYTE_STACK();
            SET_FLAG_NZ(A);
            break;
        case PHP:  // Push status to stack
            PUSH_BYTE_STACK(get_p());
            break;
        case PLP:  // Pull status from stack
            set_p(POP_BYTE_STACK());
            break;

        case TAX:  // Transfer A to X
            SET_FLAG_NZ(X = A);
            break;
        case TXA:  // Transfer X to A
            SET_FLAG_NZ(A = X);
            break;
        case TAY:  // Transfer A to Y
            SET_FLAG_NZ(Y = A);
            break;
        case TYA:  // Transfer Y to A
            SET_FLAG_NZ(A = Y);
            break;
        case TXS:  // Transfer X to SP
            SET_FLAG_NZ(SP = X);
            break;
        case TSX:  // Transfer SP to X
            SET_FLAG_NZ(X = SP);
            break;

        case ILL_SLO_IND_X:
            b1 = memory_read_byte_handler(machine, addr = READ_ADDR_IND_X());
            C = (b1 & 0x80) != 0;
            b1 <<= 1;
            memory_write_byte_handler(machine, addr, b1);
            SET_FLAG_NZ(A |= b1);
            break;

        case ILL_RLA_IND_Y:
            b1 = memory_read_byte_handler(machine, addr = READ_ADDR_IND_Y());
            b2 = b1 & 0x80;
            b1 <<= 1;
            if (C) { b1 |= 0x01; }
            memory_write_byte_handler(machine, addr, b1);
            C = b2 != 0;
            SET_FLAG_NZ(A &= b1);
            break;

        default:
            PrintStat(pc_initial);
            a_Brk = true;
            break;
    };

    if (! quiet) {
        PrintStat(pc_initial);
    }
}
