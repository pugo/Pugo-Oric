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

#include <vector>
#include <stack>
#include <utility>
#include <bitset>

#include <boost/assign.hpp>
#include <boost/tuple/tuple.hpp>

#include <machine.hpp>
#include "mos6522.hpp"


using namespace std;

// VIA Lines        Oric usage
// ----------       ---------------------------------
// PA0..PA7         PSG data bus, printer data lines
// CA1              printer acknowledge line
// CA2              PSG BC1 line
// PB0..PB2         keyboard lines-demultiplexer
// PB3              keyboard sense line
// PB4              printer strobe line
// PB5              (not connected)
// PB6              tape connector motor control
// PB7              tape connector output
// CB1              tape connector input
// CB2              PSG BDIR line

// Oric port B:
// | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
//                 | kp| Keyb. row |


MOS6522::MOS6522(Machine& a_Machine) :
    machine(a_Machine),
    orb_changed_handler(nullptr),
    ca2_changed_handler(nullptr),
    cb2_changed_handler(nullptr),
    irq_handler(nullptr),
    irq_clear_handler(nullptr)
{
    boost::assign::insert(register_names)
        (ORB, "ORB")(ORA, "ORA")(DDRB, "DDRB")(DDRA, "DDRA")
        (T1C_L, "T1C_L")(T1C_H, "T1C_H")(T1L_L, "T1L_L")(T1L_H, "T1L_H")
        (T2C_L, "T2C_L")(T2C_H, "T2C_H")(SR, "SR")(ACR, "ACR")
        (PCR, "PCR")(IFR, "IFR")(IER, "IER")(IORA2, "IORA2");

    reset();
}

MOS6522::~MOS6522()
{
}

void MOS6522::reset()
{
    ca1 = false;
    ca2 = false;
    ca2_do_pulse = false;

    cb1 = false;
    cb2 = false;
    cb2_do_pulse = false;

    ira = 0x00;   // input register A
    ora = 0;      // output register A
    ddra = 0;     // data direction register A

    irb = 0x00;   // input register B
    orb = 0;      // output register B
    ddrb = 0;     // data direction register B

    t1_latch_low = 0;
    t1_latch_high = 0;
    t1_counter = 0;
    t1_run = false;
    t1_reload = 0;

    t2_latch_low = 0;
    t2_latch_high = 0;
    t2_counter = 0;
    t2_run = false;
    t2_reload = false;

    sr = 0;    // Shift Register
    acr = 0;   // Auxilliary Control Register
    pcr = 0;   // Peripheral Control Register
    ifr = 0;   // Interrupt Flag Register
    ier = 0;   // Interrupt Enable Register
}

void MOS6522::exec()
{
    if (ca2_do_pulse) {
        ca2 = true;
        ca2_do_pulse = false;
        if (ca2_changed_handler) { ca2_changed_handler(machine, ca2); }
    }

    if (cb2_do_pulse) {
        cb2 = true;
        cb2_do_pulse = false;
        if (cb2_changed_handler) { cb2_changed_handler(machine, cb2); }
    }

    switch (acr & 0xc0)
    {
        case 0x00:
        case 0x80:
            // T1 - One shot
            if (t1_reload) {
                --t1_reload;
                if (t1_reload == 0) {
                    t1_counter = (t1_latch_high << 8) | t1_latch_low;
                }
            }
            else {
                if (t1_run && t1_counter == 0) {
                    irq_set(IRQ_T1);
                    if (acr & 0x80) {
                        orb |= 0x80;    // Output 1 on PB7 if ACR7 is set.
                    }
                    t1_run = false;
                }
                --t1_counter;
            }
            break;
        case 0x40:
        case 0xC0:
            // T1 - Continuous
            if (t1_reload) {
                --t1_reload;
                if (t1_reload == 0) {
                    t1_counter = (t1_latch_high << 8) | t1_latch_low;
                }
            }
            else {
                if (t1_counter == 0) {
                    irq_set(IRQ_T1);

                    if (acr & 0x80) {
                        orb ^= 0x80;    // Output squarewave on PB7 if ACR7 is set.
                    }

                    t1_reload = 1;
                }

                --t1_counter;
            }

            break;
    }

    if (!(acr & 0x20)) {
        // T2 - One shot
        if (t2_reload) {
            t2_reload = false;
        }
        else {
            if (t2_run && (t2_counter == 0)) {
                irq_set(IRQ_T2);
                t2_run = false;
            }

            --t2_counter;
        }
    }

    switch (acr & 0x1c)
    {
        case 0x00:  // off
            break;
        case 0x04:  // Shift in under T2 control (not implemented)
            std::cout << "shift: 0x04" << std::endl;
            break;
        case 0x08:  // Shift in under O2 control (not implemented)
            std::cout << "shift: 0x08" << std::endl;
            break;
        case 0x0c:  // Shift in under control of external clock (not implemented)
            std::cout << "shift: 0x0c" << std::endl;
            break;
        case 0x1c:  // Shift out under control of external clock (not implemented)
            std::cout << "shift: 0x1c" << std::endl;
            break;
        case 0x10:  // Shift out free-running at T2 rate
            std::cout << "shift: 0x10" << std::endl;
            break;
        case 0x14:  // Shift out under T2 control
            std::cout << "shift: 0x14" << std::endl;
            break;
        case 0x18:  // Shift out under O2 control
            std::cout << "shift: 0x18" << std::endl;
            break;
    }
}

uint8_t MOS6522::read_byte(uint16_t a_Offset)
{
    switch(a_Offset & 0x000f)
    {
        case ORB:
            irq_clear(IRQ_CB1);
            switch (pcr & PCR_MASK_CB2) {
                case 0x00:
                case 0x40:
                    irq_clear(IRQ_CB2);
                    break;
                case 0x80:
                    // set CB2 to low on read/write of ORB if CB2-ctrl is 100.
                    cb2 = false;
                    if (cb2_changed_handler) { cb2_changed_handler(machine, cb2); }
                    break;
                case 0xa0:
                    // pulse low for one cycle if CB2-ctrl is 101.
                    cb2 = false;
                    cb2_do_pulse = true;
                    if (cb2_changed_handler) { cb2_changed_handler(machine, cb2); }
                    break;
            }
            return (orb & ddrb) | (irb & ~ddrb);
        case ORA:
            irq_clear(IRQ_CA1);
            switch (pcr & PCR_MASK_CA2) {
                case 0x00:
                case 0x04:
                    irq_clear(IRQ_CA2);
                    break;
                case 0x08:
                    // set CA2 to low on read/write of ORA if CA2-ctrl is 100.
                    ca2 = false;
                    if (ca2_changed_handler) { ca2_changed_handler(machine, ca2); }
                    break;
                case 0x0a:
                    // pulse low for one cycle if CA2-ctrl is 101.
                    ca2 = false;
                    ca2_do_pulse = true;
                    if (ca2_changed_handler) { ca2_changed_handler(machine, ca2); }
                    break;
            }
            return (ora & ddra) | (ira & ~ddra);
        case DDRB:
            return ddrb;
        case DDRA:
            return ddra;
        case T1C_L:
            irq_clear(IRQ_T1);
            return t1_counter & 0x00ff;
        case T1C_H:
            return t1_counter >> 8;
        case T1L_L:
            return t1_latch_low;
        case T1L_H:
            return t1_latch_high;
        case T2C_L:
            irq_clear(IRQ_T2);
            return t2_counter & 0x00ff;
        case T2C_H:
            return t2_counter >> 8;
        case SR:
            irq_clear(IRQ_SR);
            std::cout << "  ===== READ SR TRIGGER =====" << std::endl;
            return sr;
        case ACR:
            return acr;
        case PCR:
            return pcr;
        case IFR:
            return ifr;
        case IER:
            return ier | 0x80;
        case IORA2:
            return (ora & ddra) | (ira & ~ddra);
    }
    return 0;
}

void MOS6522::write_byte(uint16_t a_Offset, uint8_t a_Value)
{
    switch(a_Offset & 0x000f)
    {
        case ORB:
// 	cout << "Write " << m_RegisterNames[static_cast<Register>(a_Offset & 0x000f)] << ": " << static_cast<unsigned int>(a_Value) << endl;
            orb = a_Value;
            irq_clear(IRQ_CB1);
            switch (pcr & PCR_MASK_CB2) {
                case 0x00:
                case 0x40:
                    irq_clear(IRQ_CB2);
                    break;
                case 0x80:
                    // set CB2 to low on read/write of ORB if CB2-ctrl is 100.
                    cb2 = false;
                    if (cb2_changed_handler) { cb2_changed_handler(machine, cb2); }
                    break;
                case 0xa0:
                    // pulse low for one cycle if CB2-ctrl is 101.
                    cb2 = false;
                    cb2_do_pulse = true;
                    if (cb2_changed_handler) { cb2_changed_handler(machine, cb2); }
                    break;
            }
//            machine.update_key_output();
            if (orb_changed_handler) { orb_changed_handler(machine, orb); }
            break;
        case ORA:
            ora = a_Value;
            irq_clear(IRQ_CA1);
            switch (pcr & PCR_MASK_CA2) {
                case 0x00:
                case 0x04:
                    irq_clear(IRQ_CA2);
                    break;
                case 0x08:
                    // set CA2 to low on read/write of ORA if CA2-ctrl is 100.
                    ca2 = false;
                    if (ca2_changed_handler) { ca2_changed_handler(machine, ca2); }
                    break;
                case 0x0a:
                    // pulse low for one cycle if CA2-ctrl is 101.
                    ca2 = false;
                    ca2_do_pulse = true;
                    if (ca2_changed_handler) { ca2_changed_handler(machine, ca2); }
                    break;
            }
            break;
        case DDRB:
            ddrb = a_Value;
            break;
        case DDRA:
            ddra = a_Value;
            break;
        case T1C_L:
            t1_latch_low = a_Value;
            break;
        case T1C_H:
            t1_latch_high = a_Value;
            t1_counter = (t1_latch_high << 8) | t1_latch_low;
            t1_reload = true;
            t1_run = true;
            irq_clear(IRQ_T1);
            // If ORB7 pulse mode is set, prepare by setting ORB7 low.
            if ((acr & 0xc0) == 0x80) {
                orb &= 0x7f;
            }
            break;
        case T1L_L:
            t1_latch_low = a_Value;
            break;
        case T1L_H:
            t1_latch_high = a_Value;
            irq_clear(IRQ_T1);
            break;
        case T2C_L:
            t2_latch_low = a_Value;
            break;
        case T2C_H:
            t2_latch_high = a_Value;
            t2_counter = (t2_latch_high << 8) | t2_latch_low;
            t2_run = true;
            t2_reload = true;
            irq_clear(IRQ_T2);
            break;
        case SR:
            sr = a_Value;
            irq_clear(IRQ_SR);
            break;
        case ACR:
            acr = a_Value;
            if( ( ( a_Value & 0xc0 ) != 0x40 ) &&
                ( ( a_Value & 0xc0 ) != 0xc0 ) )
                t1_reload = false;
            break;
        case PCR:
            pcr = a_Value;
            // Manual output modes
            if ((pcr & 0x0c) == 0x0c) {
                ca2 = !!(pcr & 0x02);
                if (!ca2) {
                    ca2_do_pulse = false;
                }
                if (ca2_changed_handler) { ca2_changed_handler(machine, ca2); }
            }

            if ((pcr & 0xc0) == 0xc0) {
                cb2 = !!(pcr & 0x20);
                if (!cb2) {
                    cb2_do_pulse = false;
                }
                if (cb2_changed_handler) { cb2_changed_handler(machine, cb2); }
            }
            break;
        case IFR:
            // Interrupt flag bits are cleared by writing 1:s for corresponding bit.
            ifr &= (~a_Value) & 0x7f;
            if ((ifr & ier) & 0x7f) {
                ifr |= 0x80;	// bit 7=1 if any IRQ is set.
            }
            else {
                if (irq_clear_handler) { irq_clear_handler(machine); }
            }
            break;
        case IER:
            if (a_Value & 0x80) {
                ier |= (a_Value & 0x7f);	// if bit 7=1: set given bits.
            }
            else {
                ier &= ~(a_Value & 0x7f);	// if bit 7=0: clear given bits.
            }

            irq_check();
            break;
        case IORA2:
            ora = a_Value;
            break;
    }
}

void MOS6522::set_irb_bit(const uint8_t a_Bit, const bool a_Value)
{
    uint8_t b = 1 << a_Bit;
    irb = (irb & ~b) | (a_Value ? b : 0);

    if (acr & 0x20) {
        if (a_Bit == 5 && (irb & 0x40) && !a_Value) {
            t2_counter--;

            if (t2_run && (t2_counter == 0)) {
                irq_set(IRQ_T2);
                t2_run = false;
            }
        }
    }
}

void MOS6522::irq_check()
{
    if ((ier & ifr) & 0x7f) {
        if ((ifr & 0x80) == 0) {
            if (irq_handler) { irq_handler(machine); }
            ifr |= 0x80;
        }
    }
    else {
        if (irq_clear_handler) { irq_clear_handler(machine); }
        ifr &= 0x7f;
    }
}

void MOS6522::irq_set(uint8_t a_Bits)
{
    ifr |= a_Bits;
    if ((ifr & ier) & 0x7f) {
        ifr |= 0x80;
    }
    if (a_Bits & ier) {
        if (irq_handler) { irq_handler(machine); }
    }
}

void MOS6522::irq_clear(uint8_t a_Bits)
{
    ifr &= ~a_Bits;

    // Clear bit 7 if no (enabled) interrupts exist.
    if (!((ifr & ier) & 0x7f)) {
        ifr &= 0x7f;
    }
}

void MOS6522::write_ca1(bool a_Value)
{
    if (ca1 != a_Value) {
        ca1 = a_Value;
        // Transitions only if enabled in PCR.
        if ((ca1 && (pcr & PCR_MASK_CA1)) || (!ca1 && !(pcr & PCR_MASK_CA1))) {
            irq_set(IRQ_CA1);

            // Handshake mode, set ca2 on pos transition of ca1.
            if (ca1 && !ca2 && (pcr & PCR_MASK_CA2) == 0x08) {
                ca2 = true;
                if (ca2_changed_handler) { ca2_changed_handler(machine, ca2); }
            }
        }
    }
}

void MOS6522::write_ca2(bool a_Value)
{
    if (ca2 != a_Value) {
        ca2 = a_Value;
        // Set interrupt on pos/neg transition if 0 or 4 in pcr.
        if ((ca2 && ((pcr & 0x0C) == 0x04 || (pcr & 0x0C) == 0x06)) || (!ca2 && ((pcr & 0x0C) == 0x00) || (pcr & 0x0C) == 0x02)) {
            irq_set(IRQ_CA2);
        }

        if (ca2_changed_handler) { ca2_changed_handler(machine, ca2); }
    }
}

void MOS6522::write_cb1(bool a_Value)
{
    if (cb1 != a_Value) {
        // Transitions only if enabled in PCR.
        cb1 = a_Value;
        if ((cb1 && (pcr & PCR_MASK_CB1)) || (!cb1 && !(pcr & PCR_MASK_CB1))) {
            irq_set(IRQ_CB1);

            // Handshake mode, set cb2 on pos transition of cb1.
            if (cb1 && !cb2 && (pcr & PCR_MASK_CB2) == 0x80) {
                cb2 = true;
                if (cb2_changed_handler) { cb2_changed_handler(machine, cb2); }
            }
        }
    }
}

void MOS6522::write_cb2(bool a_Value)
{
    if (cb2 != a_Value) {
        cb2 = a_Value;
        // Set interrupt on pos/neg transition if 0 or 40 in pcr.
        if ((cb2 && ((pcr & 0xC0) == 0x40)) || (!cb2 && ((pcr & 0xC0) == 0x00))) {
            irq_set(IRQ_CB2);
        }

        if (cb2_changed_handler) { cb2_changed_handler(machine, cb2); }
    }
}

void MOS6522::print_stat()
{
    std::cout << "VIA stats:" << std::endl;
    std::cout << "  -   ORA: " << (int)ora << std::endl;
    std::cout << "  -  DDRA: " << (int)ddra << std::endl;
    std::cout << "  -   ORB: " << (int)orb << std::endl;
    std::cout << "  -  DDRB: " << (int)ddrb << std::endl;
    std::cout << "  - T1C_L: " << (int)(t1_counter & 0x00ff) << std::endl;
    std::cout << "  - T1C_H: " << (int)(t1_counter >> 8) << std::endl;
    std::cout << "  - T1L_L: " << (int)t1_latch_low << std::endl;
    std::cout << "  - T1L_H: " << (int)t1_latch_high << std::endl;
    std::cout << "  - T2C_L: " << (int)(t2_counter & 0x00ff) << std::endl;
    std::cout << "  - T2C_H: " << (int)(t2_counter >> 8) << std::endl;
    std::cout << "  -    SR: " << (int)sr << std::endl;
    std::cout << "  -   ACR: " << (int)acr << std::endl;
    std::cout << "  -   PCR: " << (int)pcr << std::endl;
    std::cout << "  -   IFR: " << (int)ifr << std::endl;
    std::cout << "  -   IER: " << (int)(ier | 0x80) << std::endl;
    std::cout << "  - IORA2: " << (int)ora << std::endl;
}
