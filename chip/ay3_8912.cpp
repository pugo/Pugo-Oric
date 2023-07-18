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

#include <SDL_image.h>  // Temporary
#include <iostream>
#include <bitset>

#include <machine.hpp>
#include <memory.hpp>
#include "mos6522.hpp"

#include "ay3_8912.hpp"


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
//
//                14 | I/O port A

//                                     |   7  |   6  |   5  |   4  |   3  |   2  |   1  |   0  |
//     0     Channel A Tone Period     |                  8 bit fine tune A                    |
//     1                               |                           |        coarse tune A      |
//     2     Channel B Tone Period     |                  8 bit fine tune B                    |
//     3                               |                           |        coarse tune B      |
//     4     Channel C Tone Period     |                  8 bit fine tune C                    |
//     5                               |                           |        coarse tune C      |
//     6     Noise Period              |                    |       5 bit period control       |
//     7     Enable (inv)              |    IN~/OUT  |       Noise~       |        Tone~       |
//                                     |  IOB |  IOA |   C  |   B  |   A  |   C  |   B  |   A  |
//     8     Channel A Amplitude       |                    |   M  |  L3  |  L2  |  L1  |  L0  |
//     9     Channel B Amplitude       |                    |   M  |  L3  |  L2  |  L1  |  L0  |
//     A     Channel C Amplitude       |                    |   M  |  L3  |  L2  |  L1  |  L0  |
//     B     Envelope Period           |                  8 bit fine tune E                    |
//     C                               |                  8 bit Coarse tune E                  |
//     D     Envelope shape/cycle      |                           | CONT |  ATT |  ALT | HOLD |
//     E     IO Port A Data Store      |                8 bit parallel IO on port A            |
//     F     IO Port B Data Store      |                8 bit parallel IO on port B            |


// BDIR   BC2   BC1
//   0     1     0     Inactive
//   0     1     1     Read from PSG
//   1     1     0     Write to PSG
//   1     1     1     Latch address


using namespace std;

AY3_8912::AY3_8912(Machine& machine) :
	machine(machine),
	m_read_data_handler(nullptr),
    ay_writes(32768),
    sound_frequency(30),
    sound_high(0),
    sound_samples_played(0)
{
	reset();
    std::cout << "AY3_8912 writes buffer size: " << ay_writes.capacity() << std::endl;
}

AY3_8912::~AY3_8912()
{}

void AY3_8912::reset()
{
	bdir = false;
	bc1 = false;
	bc2 = false;
	current_register = 0;

    // Reset all registers.
	for (auto& i : registers) { i = 0; }

    // Reset all tone and noise periods.
    for (auto& tp : tone_period) { tp = 0; }
    noise_period = 0;

    for (auto& v : volumes) { v = 0; }
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
			if (bc1) {  // 1 ? 1
                // Latch address: read address from data bus.
				if (uint8_t new_curr = m_read_data_handler(machine); new_curr < NUM_REGS) {
					current_register = new_curr;
				}
			}
			else {  // 1 ? 0
                // Write to PSG: read value from data bus to current register.
                write_to_psg(m_read_data_handler(machine));
			}
		}
        else {
            if (bc1) {
                // Read from PSG.
                std::cout << "-- not yet supported read from PSG (AY)." << std::endl;
            }
        }
	}

//	std::cout << "AY3_8912 regs: " << std::hex <<
//  	  (int)registers[0] << " " << (int)registers[1] << " " << (int)registers[2] << " " << (int)registers[3] << " " <<
// 	  (int)registers[4] << " " << (int)registers[5] << " " << (int)registers[6] << " " << (int)registers[7] << " " <<
//	  (int)registers[8] << " " << (int)registers[9] << " " << (int)registers[10] << " " << (int)registers[11] << " " <<
//      (int)registers[12] << " " << (int)registers[13] << " " << (int)registers[14] << " " << (int)registers[15] << " " << std::endl;
}

inline void AY3_8912::write_to_psg(uint8_t value)
{
    registers[current_register] = value;

    switch (current_register) {
        case CH_A_PERIOD_LOW:
        case CH_A_PERIOD_HIGH:
        case CH_B_PERIOD_LOW:
        case CH_B_PERIOD_HIGH:
        case CH_C_PERIOD_LOW:
        case CH_C_PERIOD_HIGH:
        case NOICE_PERIOD:
        case ENABLE:
        case CH_A_AMPLITUDE:
        case CH_B_AMPLITUDE:
        case CH_C_AMPLITUDE:
            struct Write write = {1, static_cast<Register>(current_register), value};
            ay_writes.push_back(write);
            std::cout << "-- ay writes now: " << ay_writes.size() << std::endl;
            std::cout << "  -- end value: " << (int)ay_writes.back().cycle << ", " << (int)ay_writes.back().reg << ", " << (int)ay_writes.back().value << std::endl;
    };

//    switch (current_register) {
//        case CH_A_PERIOD_LOW:
//        case CH_A_PERIOD_HIGH:
//            tone_period[0] = ((registers[CH_A_PERIOD_HIGH] << 8) | registers[CH_A_PERIOD_LOW]) * 8;
//            break;
//        case CH_B_PERIOD_LOW:
//        case CH_B_PERIOD_HIGH:
//            tone_period[1] = ((registers[CH_B_PERIOD_HIGH] << 8) | registers[CH_B_PERIOD_LOW]) * 8;
//            break;
//        case CH_C_PERIOD_LOW:
//        case CH_C_PERIOD_HIGH:
//            tone_period[2] = ((registers[CH_C_PERIOD_HIGH] << 8) | registers[CH_C_PERIOD_LOW]) * 8;
//            break;
//
//        case NOICE_PERIOD:
//            noise_period = value;
//            break;
//
//        case ENABLE:
//            break;
//
//        case CH_A_AMPLITUDE:
//            volumes[0] = (value & 0x10) ? 0xffff : (((uint16_t)value & 0x0f) << 12);
//            break;
//        case CH_B_AMPLITUDE:
//            volumes[1] = (value & 0x10) ? 0xffff : (((uint16_t)value & 0x0f) << 12);
//            break;
//        case CH_C_AMPLITUDE:
//            volumes[2] = (value & 0x10) ? 0xffff : (((uint16_t)value & 0x0f) << 12);
//            break;
//    };

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
	machine.ay3->set_bdir(a_Value);
}

void AY3_8912::set_bc1(Machine& machine, bool a_Value)
{
	machine.ay3->set_bc1(a_Value);
}

void AY3_8912::set_bc2(Machine& machine, bool a_Value)
{
	machine.ay3->set_bc2(a_Value);
}




void AY3_8912::audio_callback(void* user_data, uint8_t* raw_buffer, int len)
{
    AY3_8912* f = (AY3_8912*)user_data;
    Sint16* buffer = (Sint16*)raw_buffer;

    std::cout << "audio_callback: "  << len << ", " << std::dec <<  (int) f->sound_frequency << ", " << (int) f->sound_high << std::endl;

    for (int i=0; i < (len / 2); ++i, ++f->sound_samples_played)
    {
        if ((f->sound_samples_played % f->sound_frequency) == 0) {
            f->sound_high = !f->sound_high;
        }

//        double time = (double)frontend->sound_samples_played / 44100.0;
        buffer[i] = (Sint16)28000 * f->sound_high;
    }

    if (f->sound_frequency++ >= 100) f->sound_frequency = 30;
}
