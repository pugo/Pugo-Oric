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
    cycle_count(0),
    sound_buffer_index(0)
{
	reset();
}

AY3_8912::~AY3_8912()
{}

void AY3_8912::reset()
{
	bdir = false;
	bc1 = false;
	bc2 = false;
	current_register = 0;
    cycle_count = 0;
    sound_buffer_index = 0;

    // Reset all registers.
	for (auto& i : registers) { i = 0; }

    // Reset all tone and noise periods.
    for (auto& tp : channel_tone_period) { tp = 0; }
    for (auto& cc : channel_counter) { cc = 0; }
    for (auto& cv : channel_value) { cv = 0; }
    for (auto& ce : channel_enabled) { ce = 0; }

    noise_period = 0;

    for (auto& v : volumes) { v = 0; }
}

#define CYCLES_PER_SAMPLE (1000000 / 44100)

short AY3_8912::exec(uint8_t cycles)
{
    while (cycles) {
        for (uint8_t channel = 0; channel < 3; channel++) {
            channel_counter[channel]++;
            if (channel_counter[channel] >= channel_tone_period[channel]) {
                channel_counter[channel] = 0;
                channel_value[channel] ^= 1;
            }
        }

        if (++cycle_count >= CYCLES_PER_SAMPLE) {
            int32_t out = 0;

            for (uint8_t channel = 0; channel < 3; channel++) {
                out += channel_enabled[channel] * channel_value[channel] * volumes[channel];
            }

            out /= 3;
            out -= 8192;

            if ((sound_buffer_index + 2) < 2048) {
                sound_buffer[sound_buffer_index++] = out;
                sound_buffer[sound_buffer_index++] = out;
            }
            cycle_count = 0;
        }

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
//      (int)registers[12] << " " << (int)registers[13] << " " << (int)registers[14] << std::endl;
}

inline void AY3_8912::write_to_psg(uint8_t value)
{

    switch (current_register) {
        case CH_A_PERIOD_LOW:
        case CH_A_PERIOD_HIGH:
            registers[current_register] = value;
            channel_tone_period[0] = (((registers[CH_A_PERIOD_HIGH] & 0x0f) << 8) + registers[CH_A_PERIOD_LOW]) * 8;
            if (channel_tone_period[0] == 0) { channel_tone_period[0] = 1; }
            break;
        case CH_B_PERIOD_LOW:
        case CH_B_PERIOD_HIGH:
            registers[current_register] = value;
            channel_tone_period[1] = (((registers[CH_B_PERIOD_HIGH] & 0x0f) << 8) + registers[CH_B_PERIOD_LOW]) * 8;
            if (channel_tone_period[1] == 0) { channel_tone_period[1] = 1; }
            break;
        case CH_C_PERIOD_LOW:
        case CH_C_PERIOD_HIGH:
            registers[current_register] = value;
            channel_tone_period[2] = (((registers[CH_C_PERIOD_HIGH] & 0x0f) << 8) + registers[CH_C_PERIOD_LOW]) * 8;
            if (channel_tone_period[2] == 0) { channel_tone_period[2] = 1; }
            break;

        case NOICE_PERIOD:
            registers[current_register] = value;
            noise_period = value;
            break;

        case ENABLE:
            registers[current_register] = value;
            channel_enabled[0] = (value & 0x01) ? 0 : 1;
            channel_enabled[1] = (value & 0x02) ? 0 : 1;
            channel_enabled[2] = (value & 0x04) ? 0 : 1;
            break;

        case CH_A_AMPLITUDE:
            registers[current_register] = value;
//            volumes[0] = (value & 0x10) ? 0xffff : (((uint16_t)value & 0x0f) << 12);
            volumes[0] = ((uint16_t)value & 0x0f) << 12;
            break;
        case CH_B_AMPLITUDE:
            registers[current_register] = value;
//            volumes[1] = (value & 0x10) ? 0xffff : (((uint16_t)value & 0x0f) << 12);
            volumes[1] = ((uint16_t)value & 0x0f) << 12;
            break;
        case CH_C_AMPLITUDE:
            registers[current_register] = value;
//            volumes[2] = (value & 0x10) ? 0xffff : (((uint16_t)value & 0x0f) << 12);
            volumes[2] = ((uint16_t)value & 0x0f) << 12;
            break;
        case IO_PORT_A:
            registers[current_register] = value;
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
    AY3_8912* ay = (AY3_8912*)user_data;
    uint16_t* buffer = (uint16_t*)raw_buffer;

    uint16_t samples = len/2;

//    std::cout << "-- underrun? Want: " << std::dec << (int)samples << ", got: " <<  (int)ay->sound_buffer_index*2 << std::endl;
//    if (samples > ay->sound_buffer_index*2) {
//        std::cout << "-- underrun" << std::endl;
//    }

//    if (len > 2048*2) { len = 2048*2; }
    memcpy(buffer, ay->sound_buffer, len);

//    if (ay->sound_buffer_index * 2 > len) {
    int16_t rest_bytes = ay->sound_buffer_index * 2 - len;
    if (rest_bytes > 0) {
        memcpy(ay->sound_buffer, ay->sound_buffer + len, rest_bytes);
        ay->sound_buffer_index -= samples;
    }
    else {
        ay->sound_buffer_index = 0;
    }
//    }
}
