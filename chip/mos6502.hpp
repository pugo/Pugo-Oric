// =========================================================================
//   Copyright (C) 2009-2023 by Anders Piniesjö <pugo@pugo.org>
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

// The 6522 has provision for two eight-bits IO ports (A and B) plus control
// lines (CA1, CA2, CB1, CB2), interfacing with the other components.
//
// Port A is used as a secondary bus : PA0 - PA7 lines connect both to the AY (sound)
// data bus and to the printer port. AY selection is done thanks to CA2 and CB2 lines.
//
// Port B provides many connections to the keyboard, tape, and printer port.


#ifndef MOS6502_H
#define MOS6502_H

#include "mos6502_opcodes.hpp"

#include <memory>


#define STACK_BOTTOM 0x0100

#define NMI_VECTOR_L 0xFFFA
#define NMI_VECTOR_H 0xFFFB

#define RESET_VECTOR_L 0xFFFC
#define RESET_VECTOR_H 0xFFFD

#define IRQ_VECTOR_L 0xFFFE
#define IRQ_VECTOR_H 0xFFFF

class Machine;
class MOS6502;
class Memory;

typedef uint8_t (*f_memory_read_byte_handler)(Machine &oric, uint16_t address);
typedef uint8_t (*f_memory_read_byte_zp_handler)(Machine &oric, uint8_t address);

typedef uint16_t (*f_memory_read_word_handler)(Machine &oric, uint16_t address);
typedef uint16_t (*f_memory_read_word_zp_handler)(Machine &oric, uint8_t address);

typedef void (*f_memory_write_byte_handler)(Machine &oric, uint16_t address, uint8_t val);
typedef void (*f_memory_write_byte_zp_handler)(Machine &oric, uint8_t address, uint8_t val);


class MOS6502
{
public:
	MOS6502(Machine& a_Machine);
	~MOS6502();

	void set_pc(uint16_t pc) { PC = pc; }
	uint16_t get_pc() { return PC; }
	uint8_t get_sp() { return SP; }
	uint8_t get_p();
	void set_p(uint8_t p);

	void set_quiet(bool val) { quiet = val; }

	void Reset();
	void PrintStat();
	void PrintStat(uint16_t address);

	bool exec_instruction_cycles(int16_t cycles);
    uint8_t exec_instruction(bool& a_Brk);

	// Registers
	uint8_t A;
	uint8_t X;
	uint8_t Y;

	// Flags
	//   7                           0
	// +---+---+---+---+---+---+---+---+
	// | N | V |   | B | D | I | Z | C |  <-- flag, 0/1 = reset/set
	// +---+---+---+---+---+---+---+---+
	uint8_t N_INTERN; // negative
	uint8_t Z_INTERN; // zero
	bool V; // overflow
	bool B; // break
	bool D; // decimal
	bool I; // interrupt
	bool C; // carry

	void NMI();
	void irq();
    void irq_clear();

	int inline signed_byte_to_int(uint8_t b);

	// Add and sub are complex
	void ADC(uint8_t a_Val);
	void SBC(uint8_t a_Val);

	f_memory_read_byte_handler memory_read_byte_handler;
	f_memory_read_byte_zp_handler memory_read_byte_zp_handler;

	f_memory_read_word_handler memory_read_word_handler;
	f_memory_read_word_zp_handler memory_read_word_zp_handler;

	f_memory_write_byte_handler memory_write_byte_handler;
	f_memory_write_byte_zp_handler memory_write_byte_zp_handler;

	std::string disassemble(uint16_t a_Address);

protected:
	void handle_irq();
	
	Machine& machine;
	Memory& memory;

	uint16_t PC;
	uint8_t SP;
	bool quiet;
	bool irq_flag;
};

#endif // MOS6502_H
