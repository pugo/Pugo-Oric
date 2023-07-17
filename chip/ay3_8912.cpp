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

#include <machine.hpp>
#include <memory.hpp>
#include "mos6522.hpp"

#include "ay3_8912.hpp"
#include <iostream>
#include <bitset>

// Registers
//              0, 1 | Lowest 12 bits = pitch channel A 
//              2, 3 | Lowest 12 bits = pitch channel B
//              4, 5 | Lowest 12 bits = pitch channel C
//                 6 | Lowest 5 bits = pitch of noise channel
//
//                 7 | Enablers: bit 6 = port A output/input
//                   |           bits 5, 4, 3 = ^enable noise channels C B A
//                   |           bits 2, 1, 0 = ^enable tone channels C B A
//
//                 8 | Amplitude channel A: bit 4 = amplitude envelope mode, bits 3-0 = fixed level
//                 9 | Amplitude channel B: bit 4 = amplitude envelope mode, bits 3-0 = fixed level
//                10 | Amplitude channel C: bit 4 = amplitude envelope mode, bits 3-0 = fixed level
//
//                11 | Envelope period fine tune
//                12 | Envelope period coarse tune
//                13 | Envelope shape/cycle control

//                14 | I/O port A

using namespace std;

AY3_8912::AY3_8912(Machine& machine) :
	machine(machine),
	m_read_data_handler(nullptr)
{
	reset();
}

AY3_8912::~AY3_8912()
{
}

void AY3_8912::reset()
{
	bdir = false;
	bc1 = false;
	bc2 = false;

	current_register = 0;

    // Reset all registers.
	for (uint8_t i=0; i < 15; i++) {
		registers[i] = 0;
	}

    // Reset all tone and noise periods.
    tone_period[0] = 0;
    tone_period[1] = 0;
    tone_period[2] = 0;
    noise_period = 0;

    volumes[0] = 0;
    volumes[1] = 0;
    volumes[2] = 0;
}

short AY3_8912::exec(uint8_t cycles)
{
    while (cycles) {


        --cycles;
    }

	return 0;
}

void AY3_8912::set_bdir(bool value)
{
	if (bdir != value) {
		bdir = value;
		if (bdir) {
			if (bc1) {
                // Latch address: read address from data bus.
				uint8_t new_curr = m_read_data_handler(machine);
				if (new_curr < NUM_REGS) {
					current_register = new_curr;
				}
			}
			else {
                // Write to PSG: read value from data bus to current register.
                uint8_t value = m_read_data_handler(machine);
                write_to_psg(value);
			}
		}
        else {
            if (bc1) {
                // Read from PSG.
                std::cout << "-- not yet supported read from PSG (AY)." << std::endl;
            }
        }
	}
}

inline void AY3_8912::write_to_psg(uint8_t value)
{
    registers[current_register] = value;

    switch (current_register) {
        case CH_A_PERIOD_LOW:
        case CH_A_PERIOD_HIGH:
            tone_period[0] = ((registers[CH_A_PERIOD_HIGH] << 8) | registers[CH_A_PERIOD_LOW]) * 8;
            break;
        case CH_B_PERIOD_LOW:
        case CH_B_PERIOD_HIGH:
            tone_period[1] = ((registers[CH_B_PERIOD_HIGH] << 8) | registers[CH_B_PERIOD_LOW]) * 8;
            break;
        case CH_C_PERIOD_LOW:
        case CH_C_PERIOD_HIGH:
            tone_period[2] = ((registers[CH_C_PERIOD_HIGH] << 8) | registers[CH_C_PERIOD_LOW]) * 8;
            break;

        case NOICE_PERIOD:
            noise_period = value;
            break;

        case ENABLE:
            break;

        case CH_A_AMPLITUDE:
            volumes[0] = (value & 0x10) ? 0xffff : (((uint16_t)value & 0x0f) << 12);
            break;
        case CH_B_AMPLITUDE:
            volumes[1] = (value & 0x10) ? 0xffff : (((uint16_t)value & 0x0f) << 12);
            break;
        case CH_C_AMPLITUDE:
            volumes[2] = (value & 0x10) ? 0xffff : (((uint16_t)value & 0x0f) << 12);
            break;
    };

}


void AY3_8912::set_bc1(bool value)
{
	bc1 = value;
}

void AY3_8912::set_bc2(bool value)
{
	bc2 = value;
}


void AY3_8912::set_bdir(Machine& machine, bool a_Value) {
	machine.get_ay3().set_bdir(a_Value);
}

void AY3_8912::set_bc1(Machine& machine, bool a_Value)
{
	machine.get_ay3().set_bc1(a_Value);
}

void AY3_8912::set_bc2(Machine& machine, bool a_Value)
{
	machine.get_ay3().set_bc2(a_Value);
}
