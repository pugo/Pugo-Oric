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
#include "snapshot.hpp"


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

void MOS6522::State::reset()
{
    ca1 = false;
    ca2 = false;
    ca2_do_pulse = false;

    cb1 = false;
    cb2 = false;
    cb2_do_pulse = false;

    ira = 0x00;         // input register A
    ira_latch = 0x00;   // input register A - input latch
    ora = 0x00;         // output register A
    ddra = 0x00;        // data direction register A

    irb = 0x00;         // input register B
    irb_latch = 0x00;   // input register B - input latch
    orb = 0x00;         // output register B
    ddrb = 0x00;        // data direction register B

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

    sr = 0;             // Shift Register
    sr_counter = 0;     // Modulo 8 counter for current bit
    sr_timer = 0;       // Time countdown
    sr_run = false;     // Whether SR shifting should be done, triggered by SR read/write

    acr = 0;            // Auxilliary Control Register
    pcr = 0;            // Peripheral Control Register
    ifr = 0;            // Interrupt Flag Register
    ier = 0;            // Interrupt Enable Register
}


void MOS6522::State::print()
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

void MOS6522::State::sr_shift_in()
{
    sr = (sr << 1) + (cb2 ? 1 : 0);
}

void MOS6522::State::sr_shift_out()
{
    cb2 = sr & 0x80;
    sr = (sr << 1) + (cb2 ? 1 : 0);
}


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

    state.reset();
}

MOS6522::~MOS6522()
{
}


void MOS6522::save_to_snapshot(Snapshot& snapshot)
{
    snapshot.mos6522 = state;
}

void MOS6522::load_from_snapshot(Snapshot& snapshot)
{
    state = snapshot.mos6522;
}


void MOS6522::exec()
{
    // In pulse output mode, CA2 goes low for one cycle after read/write of ORA. Return it to high here.
    if (state.ca2_do_pulse) {
        state.ca2 = true;
        state.ca2_do_pulse = false;
        if (ca2_changed_handler) { ca2_changed_handler(machine, state.ca2); }
    }

    // In pulse output mode, CB2 goes low for one cycle after read/write of ORA. Return it to high here.
    if (state.cb2_do_pulse) {
        state.cb2 = true;
        state.cb2_do_pulse = false;
        if (cb2_changed_handler) { cb2_changed_handler(machine, state.cb2); }
    }

    switch (state.acr & 0xc0)
    {
        case 0x00:
        case 0x80:
            // T1 - One shot
            if (state.t1_reload) {
                --state.t1_reload;
                if (state.t1_reload == 0) {
                    state.t1_counter = (state.t1_latch_high << 8) | state.t1_latch_low;
                }
            }
            else {
                if (state.t1_run && state.t1_counter == 0) {
                    irq_set(IRQ_T1);
                    if (state.acr & 0x80) {
                        state.orb |= 0x80;    // Output 1 on PB7 if ACR7 is set.
                    }
                    state.t1_run = false;
                }
                --state.t1_counter;
            }
            break;
        case 0x40:
        case 0xC0:
            // T1 - Continuous
            if (state.t1_reload) {
                --state.t1_reload;
                if (state.t1_reload == 0) {
                    state.t1_counter = (state.t1_latch_high << 8) | state.t1_latch_low;
                }
            }
            else {
                if (state.t1_counter == 0) {
                    irq_set(IRQ_T1);

                    if (state.acr & 0x80) {
                        state.orb ^= 0x80;    // Output squarewave on PB7 if ACR7 is set.
                    }

                    state.t1_reload = 1;
                }

                --state.t1_counter;
            }

            break;
    }

    if (!(state.acr & 0x20)) {
        // T2 - One shot (pulse counting mode handled in set_irb_bit)
        if (state.t2_reload) {
            state.t2_reload = false;
        }
        else {
            if (state.t2_run && (state.t2_counter == 0)) {
                irq_set(IRQ_T2);
                state.t2_run = false;
            }

            --state.t2_counter;
        }
    }

    switch (state.acr & 0x1c)
    {
        case 0x00:  // off
            break;
        case 0x04:  // Shift in under T2 control
            if (! state.sr_run) { break; }

            if (state.sr_timer == 0) {
                state.sr_timer = state.t2_latch_low;
                break;
            }

            if (--state.sr_timer == 0) {
                state.cb1 = ! state.cb1;
                if (state.cb1) {
                    state.sr_shift_in();
                    sr_handle_counter();
                }
            }
            break;
        case 0x08:  // Shift in under O2 control
            if (! state.sr_run) { break; }

            state.cb1 = ! state.cb1;
            if (state.cb1) {
                state.sr_shift_in();
                sr_handle_counter();
            }
            break;
        case 0x0c:  // Shift in under control of external clock (not implemented)
            if (state.ifr & IRQ_SR) { irq_clear(IRQ_SR); }
            state.sr_run = false;
            state.sr_counter = 0;
            break;
        case 0x1c:  // Shift out under control of external clock (not implemented)
            if (state.ifr & IRQ_SR) { irq_clear(IRQ_SR); }
            state.sr_run = false;
            state.sr_counter = 0;
            break;
        case 0x10:  // Shift out free-running at T2 rate
            if (! state.sr_run) { break; }

            if (state.sr_timer == 0) {
                state.sr_timer = state.t2_latch_low;
                break;
            }

            if (--state.sr_timer == 0) {
                state.cb1 = ! state.cb1;
                if (! state.cb1) {
                    state.sr_shift_out();
                    state.sr_counter = (state.sr_counter + 1) % 8;
                }
            }
            break;
        case 0x14:  // Shift out under T2 control
            if (! state.sr_run) { break; }

            if (state.sr_timer == 0) {
                state.sr_timer = state.t2_latch_low;
                break;
            }

            if (--state.sr_timer == 0) {
                state.cb1 = ! state.cb1;
                if (! state.cb1) {
                    state.sr_shift_out();
                    sr_handle_counter();
                }
            }
            break;
        case 0x18:  // Shift out under O2 control
            if (! state.sr_run) { break; }

            state.cb1 = ! state.cb1;
            if (! state.cb1) {
                state.sr_shift_out();
                sr_handle_counter();
            }
            break;
    }
}

uint8_t MOS6522::read_byte(uint16_t offset)
{
    switch(offset & 0x000f)
    {
        case ORB:
            irq_clear(IRQ_CB1);
            switch (state.pcr & PCR_MASK_CB2) {
                case 0x00:
                case 0x40:
                    irq_clear(IRQ_CB2);
                    break;
                case 0x80:
                    // set CB2 to low on read/write of ORB if CB2-ctrl is 100.
                    state.cb2 = false;
                    if (cb2_changed_handler) { cb2_changed_handler(machine, state.cb2); }
                    break;
                case 0xa0:
                    // pulse low for one cycle if CB2-ctrl is 101.
                    state.cb2 = false;
                    state.cb2_do_pulse = true;
                    if (cb2_changed_handler) { cb2_changed_handler(machine, state.cb2); }
                    break;
            }

            if (state.acr & ACR_PB_LATCH_ENABLE) {
                return (state.orb & state.ddrb) | (state.irb_latch & ~state.ddrb);
            }
            return (state.orb & state.ddrb) | (state.irb & ~state.ddrb);
        case ORA:
            irq_clear(IRQ_CA1);
            switch (state.pcr & PCR_MASK_CA2) {
                case 0x00:
                case 0x04:
                    irq_clear(IRQ_CA2);
                    break;
                case 0x08:
                    // set CA2 to low on read/write of ORA if CA2-ctrl is 100.
                    state.ca2 = false;
                    if (ca2_changed_handler) { ca2_changed_handler(machine, state.ca2); }
                    break;
                case 0x0a:
                    // pulse low for one cycle if CA2-ctrl is 101.
                    state.ca2 = false;
                    state.ca2_do_pulse = true;
                    if (ca2_changed_handler) { ca2_changed_handler(machine, state.ca2); }
                    break;
            }

            if (state.acr & ACR_PA_LATCH_ENABLE) {
                return (state.ora & state.ddra) | (state.ira_latch & ~state.ddra);
            }
            return (state.ora & state.ddra) | (state.ira & ~state.ddra);
        case DDRB:
            return state.ddrb;
        case DDRA:
            return state.ddra;
        case T1C_L:
            irq_clear(IRQ_T1);
            return state.t1_counter & 0x00ff;
        case T1C_H:
            return state.t1_counter >> 8;
        case T1L_L:
            return state.t1_latch_low;
        case T1L_H:
            return state.t1_latch_high;
        case T2C_L:
            irq_clear(IRQ_T2);
            return state.t2_counter & 0x00ff;
        case T2C_H:
            return state.t2_counter >> 8;
        case SR:
            state.sr_timer = 0;
            state.sr_counter = 0;
            state.sr_run = true;
            irq_clear(IRQ_SR);
            return state.sr;
        case ACR:
            return state.acr;
        case PCR:
            return state.pcr;
        case IFR:
            return state.ifr;
        case IER:
            return state.ier | 0x80;
        case IORA2:
            return (state.ora & state.ddra) | (state.ira & ~state.ddra);
    }
    return 0;
}

void MOS6522::write_byte(uint16_t offset, uint8_t value)
{
    switch(offset & 0x000f)
    {
        case ORB:
// 	cout << "Write " << m_RegisterNames[static_cast<Register>(offset & 0x000f)] << ": " << static_cast<unsigned int>(value) << endl;
            state.orb = value;
            irq_clear(IRQ_CB1);
            switch (state.pcr & PCR_MASK_CB2) {
                case 0x00:
                case 0x40:
                    irq_clear(IRQ_CB2);
                    break;
                case 0x80:
                    // set CB2 to low on read/write of ORB if CB2-ctrl is 100.
                    state.cb2 = false;
                    if (cb2_changed_handler) { cb2_changed_handler(machine, state.cb2); }
                    break;
                case 0xa0:
                    // pulse low for one cycle if CB2-ctrl is 101.
                    state.cb2 = false;
                    state.cb2_do_pulse = true;
                    if (cb2_changed_handler) { cb2_changed_handler(machine, state.cb2); }
                    break;
            }
//            machine.update_key_output();
            if (orb_changed_handler) { orb_changed_handler(machine, state.orb); }
            break;
        case ORA:
            state.ora = value;
            irq_clear(IRQ_CA1);
            switch (state.pcr & PCR_MASK_CA2) {
                case 0x00:
                case 0x04:
                    irq_clear(IRQ_CA2);
                    break;
                case 0x08:
                    // set CA2 to low on read/write of ORA if CA2-ctrl is 100.
                    state.ca2 = false;
                    if (ca2_changed_handler) { ca2_changed_handler(machine, state.ca2); }
                    break;
                case 0x0a:
                    // pulse low for one cycle if CA2-ctrl is 101.
                    state.ca2 = false;
                    state.ca2_do_pulse = true;
                    if (ca2_changed_handler) { ca2_changed_handler(machine, state.ca2); }
                    break;
            }
            break;
        case DDRB:
            state.ddrb = value;
            break;
        case DDRA:
            state.ddra = value;
            break;
        case T1C_L:
            state.t1_latch_low = value;
            break;
        case T1C_H:
            state.t1_latch_high = value;
            state.t1_counter = (state.t1_latch_high << 8) | state.t1_latch_low;
            state.t1_reload = true;
            state.t1_run = true;
            irq_clear(IRQ_T1);
            // If ORB7 pulse mode is set, prepare by setting ORB7 low.
            if ((state.acr & 0xc0) == 0x80) {
                state.orb &= 0x7f;
            }
            break;
        case T1L_L:
            state.t1_latch_low = value;
            break;
        case T1L_H:
            state.t1_latch_high = value;
            irq_clear(IRQ_T1);
            break;
        case T2C_L:
            state.t2_latch_low = value;
            break;
        case T2C_H:
            state.t2_latch_high = value;
            state.t2_counter = (state.t2_latch_high << 8) | state.t2_latch_low;
            state.t2_run = true;
            state.t2_reload = true;
            irq_clear(IRQ_T2);
            break;
        case SR:
            state.sr = value;
            state.sr_timer = 0;
            state.sr_counter = 0;
            state.sr_run = true;
            irq_clear(IRQ_SR);
            break;
        case ACR:
            state.acr = value;
            if( ( ( value & 0xc0 ) != 0x40 ) &&
                ( ( value & 0xc0 ) != 0xc0 ) )
                state.t1_reload = false;
            break;
        case PCR:
            state.pcr = value;
            // Manual output modes
            if ((state.pcr & 0x0c) == 0x0c) {
                state.ca2 = !!(state.pcr & 0x02);
                if (!state.ca2) {
                    state.ca2_do_pulse = false;
                }
                if (ca2_changed_handler) { ca2_changed_handler(machine, state.ca2); }
            }

            if ((state.pcr & 0xc0) == 0xc0) {
                state.cb2 = !!(state.pcr & 0x20);
                if (!state.cb2) {
                    state.cb2_do_pulse = false;
                }
                if (cb2_changed_handler) { cb2_changed_handler(machine, state.cb2); }
            }
            break;
        case IFR:
            // Interrupt flag bits are cleared by writing 1:s for corresponding bit.
            state.ifr &= (~value) & 0x7f;
            if ((state.ifr & state.ier) & 0x7f) {
                state.ifr |= 0x80;	// bit 7=1 if any IRQ is set.
            }
            else {
                if (irq_clear_handler) { irq_clear_handler(machine); }
            }
            break;
        case IER:
            if (value & 0x80) {
                state.ier |= (value & 0x7f);	// if bit 7=1: set given bits.
            }
            else {
                state.ier &= ~(value & 0x7f);	// if bit 7=0: clear given bits.
            }

            irq_check();
            break;
        case IORA2:
            state.ora = value;
            break;
    }
}

void MOS6522::set_ira_bit(const uint8_t bit, const bool value)
{
    uint8_t b = 1 << bit;

    state.ira = (state.ira & ~b) | (value ? b : 0);
}

void MOS6522::set_irb_bit(const uint8_t bit, const bool value)
{
    uint8_t original_bit_6 = state.irb & 0x40;

    uint8_t b = 1 << bit;

    state.irb = (state.irb & ~b) | (value ? b : 0);

    if (state.acr & 0x20) {
        if (bit == 6 && (original_bit_6 & 0x40) && !value) {
            state.t2_counter--;
            if (state.t2_run && (state.t2_counter == 0)) {
                irq_set(IRQ_T2);
                state.t2_run = false;
            }
        }
    }
}

void MOS6522::irq_check()
{
    if ((state.ier & state.ifr) & 0x7f) {
        if ((state.ifr & 0x80) == 0) {
            if (irq_handler) { irq_handler(machine); }
            state.ifr |= 0x80;
        }
    }
    else {
        if (irq_clear_handler) { irq_clear_handler(machine); }
        state.ifr &= 0x7f;
    }
}

void MOS6522::irq_set(uint8_t bits)
{
    state.ifr |= bits;

    if ((state.acr & ACR_PA_LATCH_ENABLE) && (bits & IRQ_CA1)) {
        state.ira_latch = state.ira;
    }

    if ((state.acr & ACR_PB_LATCH_ENABLE) && (bits & IRQ_CB1)) {
        state.irb_latch = state.irb;
    }

    if ((state.ifr & state.ier) & 0x7f) {
        state.ifr |= 0x80;
    }
    if (bits & state.ier) {
        if (irq_handler) { irq_handler(machine); }
    }
}

void MOS6522::irq_clear(uint8_t bits)
{
    state.ifr &= ~bits;

    // Clear bit 7 if no (enabled) interrupts exist.
    if (!((state.ifr & state.ier) & 0x7f)) {
        state.ifr &= 0x7f;
    }
}

void MOS6522::sr_handle_counter()
{
    if (++state.sr_counter == 8) {
        irq_set(IRQ_SR);
        state.sr_timer = 0;
        state.sr_run = false;
    }
}

void MOS6522::write_ca1(bool value)
{
    if (state.ca1 != value) {
        state.ca1 = value;

        // Transitions only if enabled in PCR.
        if ((state.ca1 && (state.pcr & PCR_MASK_CA1)) || (!state.ca1 && !(state.pcr & PCR_MASK_CA1))) {
            irq_set(IRQ_CA1);
        }

        // Handshake mode, set ca2 on pos transition of ca1.
        if (state.ca1 && !state.ca2 && (state.pcr & PCR_MASK_CA2) == 0x08) {
            state.ca2 = true;
            if (ca2_changed_handler) { ca2_changed_handler(machine, state.ca2); }
        }
    }
}

void MOS6522::write_ca2(bool value)
{
    if (state.ca2 != value) {
        state.ca2 = value;
        // Set interrupt on pos/neg transition if 0 or 4 in pcr.
        if ((state.ca2 && ((state.pcr & 0x0C) == 0x04 || (state.pcr & 0x0C) == 0x06)) ||
            (!state.ca2 && ((state.pcr & 0x0C) == 0x00) || (state.pcr & 0x0C) == 0x02)) {
            irq_set(IRQ_CA2);
        }

        if (ca2_changed_handler) { ca2_changed_handler(machine, state.ca2); }
    }
}

void MOS6522::write_cb1(bool value)
{
    if (state.cb1 != value) {
        // Transitions only if enabled in PCR.
        state.cb1 = value;
        if ((state.cb1 && (state.pcr & PCR_MASK_CB1)) || (!state.cb1 && !(state.pcr & PCR_MASK_CB1))) {
            irq_set(IRQ_CB1);
        }

        // Handshake mode, set cb2 on pos transition of cb1.
        if (state.cb1 && !state.cb2 && (state.pcr & PCR_MASK_CB2) == 0x80) {
            state.cb2 = true;
            if (cb2_changed_handler) { cb2_changed_handler(machine, state.cb2); }
        }
    }
}

void MOS6522::write_cb2(bool value)
{
    if (state.cb2 != value) {
        state.cb2 = value;
        // Set interrupt on pos/neg transition if 0 or 40 in pcr.
        if ((state.cb2 && ((state.pcr & 0xC0) == 0x40)) || (!state.cb2 && ((state.pcr & 0xC0) == 0x00))) {
            irq_set(IRQ_CB2);
        }

        if (cb2_changed_handler) { cb2_changed_handler(machine, state.cb2); }
    }
}

