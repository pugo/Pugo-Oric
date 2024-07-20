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

#include <iostream>
#include <bitset>
#include <numeric>

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

// Volume table from Oricutron.
uint32_t voltab[] = {0, 513/4, 828/4, 1239/4, 1923/4, 3238/4, 4926/4, 9110/4, 10344/4, 17876/4, 24682/4, 30442/4, 38844/4, 47270/4, 56402/4, 65535/4};

static const uint8_t _ay38910_shapes[16][32] = {
    // CONTINUE ATTACK ALTERNATE HOLD
    // 0 0 X X
    { 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    // 0 1 X X
    { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    // 1 0 0 0
    { 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 },
    // 1 0 0 1
    { 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    // 1 0 1 0
    { 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 },
    // 1 0 1 1
    { 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15 },
    // 1 1 0 0
    { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 },
    // 1 1 0 1
    { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15 },
    // 1 1 1 0
    { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 },
    // 1 1 1 1
    { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
};

constexpr uint8_t cycle_multiplier = 128;


Channel::Channel() :
    volume(0), tone_period(0), counter(0), value(0), disabled(1), noise_diabled(1), use_envelope(false)
{}

void Channel::reset()
{
    volume = 0;
    tone_period = 0;
    counter = 0;
    value = 0;
    disabled = 1;
    noise_diabled = 1;
    use_envelope = false;
}


Noise::Noise() :
    period(0), counter(0), bit(0), rng(1)
{}

void Noise::reset()
{
    period = 0;
    counter = 0;
}


Envelope::Envelope() :
    period(0), counter(0), shape(0), shape_counter(0), out_level(0), hold(false), holding(false)
{}


void Envelope::reset()
{
    period = 0;
    counter = 0;
    shape = 0;
    shape_counter = 0;
    out_level = 0;
    hold = false;
    holding = false;
}


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
    sound_buffer_next_play_index = 0;
    sound_buffer_index = 0;
    move_sound_data = false;
    start_play = false;
    cycle_diff = -25;
    cycle_diffs_buffer.set_capacity(40);
    old_length_counter = 0;
    old_length = 0;

    cycles_per_sample = ((1000000 * cycle_multiplier) / 44100);

    for (uint32_t i = 0; i < 32768; i++) {
        sound_buffer[i] = 0;
    }

    // Reset all registers.
    for (auto& i : registers) { i = 0; }

    // Reset all tone and noise periods.
    for (auto& c : channels) { c.reset(); }
    noise.reset();
}


short AY3_8912::exec(uint8_t cycles)
{
    uint32_t c;
//    std::cout << "-- cycles: " << std::dec << (int)cycles <<  std::endl;

    int32_t cycle_diff_change = 0;
    while (cycles) {
        // Tones
        for (uint8_t channel = 0; channel < 3; channel++) {
            if (++channels[channel].counter >= channels[channel].tone_period) {
                channels[channel].counter = 0;
                channels[channel].value ^= 1;
            }
        }

        // Noise
        if (++noise.counter >= noise.period) {
            noise.counter = 0;
            noise.bit ^= 1;
            if (noise.bit) {
                noise.rng ^= (((noise.rng & 1) ^ ((noise.rng >> 3) & 1)) << 17);
                noise.rng >>= 1;
            }
        }

        // Envelope
        if (++envelope.counter >= envelope.period) {
            envelope.counter = 0;

            if (! envelope.holding) {
                envelope.shape_counter = (envelope.shape_counter + 1) & 0x1f;
                if (envelope.hold && (envelope.shape_counter == 0x1f)) {
                    envelope.holding = true;
                }
            }

            for (uint8_t channel = 0; channel < 3; channel++) {
                if (channels[channel].use_envelope) {
                    channels[channel].volume = voltab[_ay38910_shapes[envelope.shape][envelope.shape_counter]];
                }
            }
        }

        if (cycle_count >= (cycles_per_sample + cycle_diff)) {
            if (move_sound_data) {
//                buffer_mutex.lock();
                uint32_t len = sound_buffer_index - sound_buffer_next_play_index;
//    std::cout << "-- sound_buffer_index: " << std::dec << (int)sound_buffer_index << ", sound_buffer_next_play_index: " <<  (int)sound_buffer_next_play_index << ", len: " << (int)len << std::endl;

                memmove(sound_buffer, &sound_buffer[sound_buffer_next_play_index], len * 2);
                sound_buffer_index = len;
                sound_buffer_next_play_index = 0;
                move_sound_data = false;

                cycle_diffs_buffer.push_back(len);

                if (old_length_counter++ == 100) {
                    old_length_counter = 0;

                    if (cycle_diffs_buffer.size() == 40) {
                        int16_t avg = std::accumulate(cycle_diffs_buffer.begin(), cycle_diffs_buffer.end(), 0) / cycle_diffs_buffer.size();
                        if (old_length == 0) {
                            old_length = avg;
                        }

                        if (avg > old_length) {
                            cycle_diff_change = 1;
                        }
                        else if (avg < old_length) {
                            cycle_diff_change = -1;
                        }

                        old_length = avg;
                    }
                }

//                buffer_mutex.unlock();
            }

            uint32_t out = 0;

            for (uint8_t channel = 0; channel < 3; channel++) {
                out += ((channels[channel].value | channels[channel].disabled) & ((noise.rng & 1) |
                                  channels[channel].noise_diabled)) * channels[channel].volume;
            }

            if (out > 32767) { out = 32767; }

            if ((sound_buffer_index + 2) < 32768) {
                sound_buffer[sound_buffer_index++] = out;
                sound_buffer[sound_buffer_index++] = out;
            }
            cycle_count -= (cycles_per_sample + cycle_diff);
        }
        else {
            cycle_count += cycle_multiplier;
        }

        --cycles;
    }

    cycle_diff += cycle_diff_change;

    if (sound_buffer_index > 5000) {
        start_play = true;
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
}


inline void AY3_8912::write_to_psg(uint8_t value)
{
    switch (current_register) {
        case CH_A_PERIOD_LOW:
        case CH_A_PERIOD_HIGH:
            registers[current_register] = value;
            channels[0].tone_period = (((registers[CH_A_PERIOD_HIGH] & 0x0f) << 8) + registers[CH_A_PERIOD_LOW]) * 8;
            if (channels[0].tone_period == 0) { channels[0].tone_period = 1; }
            break;
        case CH_B_PERIOD_LOW:
        case CH_B_PERIOD_HIGH:
            registers[current_register] = value;
            channels[1].tone_period = (((registers[CH_B_PERIOD_HIGH] & 0x0f) << 8) + registers[CH_B_PERIOD_LOW]) * 8;
            if (channels[1].tone_period == 0) { channels[1].tone_period = 1; }
            break;
        case CH_C_PERIOD_LOW:
        case CH_C_PERIOD_HIGH:
            registers[current_register] = value;
            channels[2].tone_period = (((registers[CH_C_PERIOD_HIGH] & 0x0f) << 8) + registers[CH_C_PERIOD_LOW]) * 8;
            if (channels[2].tone_period == 0) { channels[2].tone_period = 1; }
            break;

        case NOICE_PERIOD:
            registers[current_register] = value;
            noise.period = (value & 0x1f) * 8;
            break;

        case ENABLE:
            registers[current_register] = value;
            channels[0].disabled = (value & 0x01) ? 1 : 0;
            channels[1].disabled = (value & 0x02) ? 1 : 0;
            channels[2].disabled = (value & 0x04) ? 1 : 0;
            channels[0].noise_diabled = (value & 0x08) ? 1 : 0;
            channels[1].noise_diabled = (value & 0x10) ? 1 : 0;
            channels[2].noise_diabled = (value & 0x20) ? 1 : 0;
            break;

        case CH_A_AMPLITUDE:
            registers[current_register] = value;
            channels[0].use_envelope = (value & 0x10) != 0;
            if (channels[0].use_envelope) {
                channels[0].volume = voltab[_ay38910_shapes[envelope.shape][envelope.shape_counter]];
            }
            else {
                channels[0].volume = voltab[value & 0x0f];
            }
            break;
        case CH_B_AMPLITUDE:
            registers[current_register] = value;
            channels[1].use_envelope = (value & 0x10) != 0;
            if (channels[1].use_envelope) {
                channels[1].volume = voltab[_ay38910_shapes[envelope.shape][envelope.shape_counter]];
            }
            else {
                channels[1].volume = voltab[value & 0x0f];
            }
            break;
        case CH_C_AMPLITUDE:
            registers[current_register] = value;
            channels[2].use_envelope = (value & 0x10) != 0;
            if (channels[2].use_envelope) {
                channels[2].volume = voltab[_ay38910_shapes[envelope.shape][envelope.shape_counter]];
            }
            else {
                channels[2].volume = voltab[value & 0x0f];
            }
            break;
        case ENV_DURATION_LOW:
        case ENV_DURATION_HIGH:
            registers[current_register] = value;
            envelope.period = ((registers[ENV_DURATION_HIGH] << 8) + registers[CH_A_PERIOD_LOW]) * 16;
            if (envelope.period == 0) { envelope.period = 1; }
            break;
        case ENV_SHAPE:
            registers[current_register] = value;
            envelope.shape = value & 0x0f;
            envelope.holding = false;
            envelope.counter = 0;
            envelope.shape_counter = 0;

            // 0x08 = continue, 0x01 = hold
            if (!(value & 0x08) || (value & 0x01)) {
                envelope.hold = true;
            }

            for (uint8_t channel = 0; channel < 3; channel++) {
                if (channels[channel].use_envelope) {
                    channels[channel].volume = voltab[_ay38910_shapes[envelope.shape][envelope.shape_counter]];
                }
            }
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
//    std::cout << "-- underrun? Want: " << std::dec << (int)samples << " samples, got: " <<  (int)ay->sound_buffer_index << std::endl;

    if (! ay->start_play) {
        return;
    }

    bool underrun = false;

//    ay->buffer_mutex.lock();
    uint32_t first = ay->sound_buffer_next_play_index;
    if (ay->sound_buffer_index - first < samples) {
        underrun = true;
        samples = ay->sound_buffer_index - first;
//        std::cout << "-- underrun!" << std::endl;
    }

    memcpy(buffer, &ay->sound_buffer[first], samples * 2);

    if (! underrun) {
        ay->sound_buffer_next_play_index += samples;
        ay->move_sound_data = true;
    }

//    ay->buffer_mutex.unlock();
}
