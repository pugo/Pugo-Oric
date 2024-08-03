// =========================================================================
//   Copyright ( C) 2009-2024 by Anders Piniesjö <pugo@pugo.org>
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

#ifndef MOS6522_H
#define MOS6522_H

#include <map>
#include <memory>
#include <string>
#include <iostream>

class Machine;
class Memory;


/**
    @author Anders Piniesjö <pugo@pugo.org>
*/
class MOS6522
{
public:
    enum Register {
        ORB   = 0x00,	// Output register B
        ORA   = 0x01,	// Output register A
        DDRB  = 0x02,	// Data direction port B
        DDRA  = 0x03,	// Data direction port A
        T1C_L = 0x04,	// Write: into T1 low latch.   Read: T1 low counter, reset interrupt flag.
        T1C_H = 0x05,	// Write: into T1 high latch+counter, transfer low.  Read: T1 high counter.
        T1L_L = 0x06,	// Write: into T1 low latch.   Read: T1 low latch.
        T1L_H = 0x07,	// Write: into T1 high latch.  Read: T2 high latch.
        T2C_L = 0x08,	// Write: into T2 low latch.   Read: T2 low counter, reset interrupt flag.
        T2C_H = 0x09,	// Write: into T2 high latch+counter, transfer low.  Read: T2 high counter.
        SR    = 0x0A,	// Shift Register
        ACR   = 0x0B,	// Auxilliary Control Register
        PCR   = 0x0C,	// Peripheral Control Register
        IFR   = 0x0D,	// Interrupt Flag Register
        IER   = 0x0E,	// Interrupt Enable Register
        IORA2 = 0x0F,
    };

    enum IRQ {
        IRQ_CA2  = 0x01,
        IRQ_CA1  = 0x02,
        IRQ_SR   = 0x04,
        IRQ_CB2  = 0x08,
        IRQ_CB1  = 0x10,
        IRQ_T2   = 0x20,
        IRQ_T1   = 0x40,
        IRQ_CTRL = 0x80
    };

    // PCR controlling bits.
    enum PCR {
        PCR_MASK_CA1 = 0x01,
        PCR_MASK_CA2 = 0x0E,
        PCR_MASK_CB1 = 0x10,
        PCR_MASK_CB2 = 0xE0
    };

    typedef void (*f_ca2_changed_handler)(Machine &machine, bool value);
    typedef void (*f_cb2_changed_handler)(Machine &machine, bool aValue);

    MOS6522(Machine& a_Machine);
    ~MOS6522();

    void reset();
    void exec();

    uint8_t read_byte(uint16_t a_Offset);
    void write_byte(uint16_t a_Offset, uint8_t a_Value);

    uint8_t read_ora() { return (ora & ddra); }
    uint8_t read_orb() { return (orb & ddrb); }

    void set_irb_bit(const uint8_t a_Bit, const bool a_Value);

    void write_irb(uint8_t a_Value) { irb = a_Value; }

    void write_ca1(bool a_Value);
    void write_ca2(bool a_Value);

    void write_cb1(bool a_Value);
    void write_cb2(bool a_Value);

    void print_stat();

    f_ca2_changed_handler ca2_changed_handler;
    f_cb2_changed_handler cb2_changed_handler;

private:
    void irq_check();
    void irq_set(uint8_t a_Bits);
    void irq_clear(uint8_t a_Bits);

    bool ca1;
    bool ca2;
    bool ca2_do_pulse;

    bool cb1;
    bool cb2;
    bool cb2_do_pulse;

    uint8_t ira;		// Input Register A
    uint8_t ora;		// Output Register A
    uint8_t ddra;		// Data Direction Register A (input = 0, output = 1)

    uint8_t irb;		// Input Register B
    uint8_t orb;		// Output Register B
    uint8_t ddrb;		// Data Direction Register B (input = 0, output = 1)

    uint8_t t1_latch_low;
    uint8_t t1_latch_high;
    uint16_t t1_counter;
    bool t1_run;
    uint8_t t1_reload;

    uint8_t t2_latch_low;
    uint8_t t2_latch_high;
    uint16_t t2_counter;
    bool t2_run;
    bool t2_reload;

    uint8_t sr;         // Shift register

    uint8_t acr;		// Auxilliary control register (shift mode, etc)
    // |  7  |  6  |    5    |  4  |  3  |  2  |      1       |      0       |
    // |  T1 ctrl  | T2 ctrl |     SR  ctrl    | PB latch en. | PA latch en. |

    uint8_t pcr;		// Peripheral control register
    // |  7  |  6  |  5  |     4    |  3  |  2  |  1  |     0    |
    // |    CB2 ctrl     | CB1 ctrl |     CA2 ctrl    | CA1 ctrl |

    uint8_t ifr;		// Interrupt Flag Register:   | IRQ  | T1 | T2 | CB1 | CB2 | SR | CA1 | CA2 |
    uint8_t ier;		// Interrupt Enable Register: | ctrl | T1 | T2 | CB1 | CB2 | SR | CA1 | CA2 |

    Machine& machine;

    std::map<Register, std::string> register_names;
};


#endif // MOS6502_H
