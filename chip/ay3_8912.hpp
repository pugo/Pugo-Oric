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

#ifndef AY3_8912_H
#define AY3_8912_H

#include <memory>
#include <mutex>
#include <boost/circular_buffer.hpp>

class Snapshot;
class Machine;

typedef uint8_t (*f_read_data_handler)(Machine &oric);
typedef uint8_t (*f_write_data_handler)(Machine &oric);


class Channel
{
public:
    Channel();

    void reset();

    uint16_t volume;
    uint32_t tone_period;
    uint32_t counter;
    uint16_t value;
    uint8_t disabled;
    uint16_t noise_diabled;
    bool use_envelope;
};


class Noise
{
public:
    Noise();

    void reset();

    uint16_t period;
    uint16_t counter;
    uint8_t bit;
    uint32_t rng;
};


class Envelope
{
public:
    Envelope();

    void reset();

    uint32_t period;
    uint32_t counter;

    uint8_t shape;
    uint8_t shape_counter;
    uint8_t out_level;

    bool cont;
    bool hold;
    bool holding;
};


class AY3_8912
{
public:
    enum Register
    {
        CH_A_PERIOD_LOW = 0,
        CH_A_PERIOD_HIGH,
        CH_B_PERIOD_LOW,
        CH_B_PERIOD_HIGH,
        CH_C_PERIOD_LOW,
        CH_C_PERIOD_HIGH,
        NOICE_PERIOD,
        ENABLE,
        CH_A_AMPLITUDE,
        CH_B_AMPLITUDE,
        CH_C_AMPLITUDE,
        ENV_DURATION_LOW,
        ENV_DURATION_HIGH,
        ENV_SHAPE,
        IO_PORT_A,
        NUM_REGS
    };

    struct State
    {
        void reset();

        bool bdir;
        bool bc1;
        bool bc2;

        uint8_t current_register;
        uint8_t registers[NUM_REGS];

        Channel channels[3];
        Noise noise;
        Envelope envelope;
    };

    AY3_8912(Machine& machine);
    ~AY3_8912();

    /**
     * Reset the AY-3-8912.
     */
    void reset();

    /**
     * Save AY-3-8912 state to snapshot.
     * @param snapshot reference to snapshot
     */
    void save_to_snapshot(Snapshot& snapshot);

    /**
     * Load AY-3-8912 state from snapshot.
     * @param snapshot reference to snapshot
     */
    void load_from_snapshot(Snapshot& snapshot);

    /**
     * Execute one clock cycle.
     */
    short exec();

    /**
     * Set bus direction pin value.
     * @param value bus direction pin value
     */
    void set_bdir(bool value);

    /**
     * Set BC1 pin value.
     * @param value BC1 pin value
     */
    void set_bc1(bool value);

    /**
     * Set BC2 pin value.
     * @param value BC1 pin value
     */
    void set_bc2(bool value);

    /**
     * Get value of specified register
     * @param reg register to get
     * @return register value
     */
    uint8_t get_register(Register reg) { return state.registers[reg]; }

    /**
     * Set bus direction pin value - callback function.
     * @param machine Machine object for current machine
     * @param value new value
     */
    static void set_bdir_callback(Machine& machine, bool value);

    /**
     * Set BC1 pin value - callback function.
     * @param machine Machine object for current machine
     * @param value new value
     */
    static void set_bc1_callback(Machine& machine, bool value);

    /**
     * Set BC2 pin value - callback function.
     * @param machine Machine object for current machine
     * @param value new value
     */
    static void set_bc2_callback(Machine& machine, bool value);

    /**
     * Called by frontend to get audio.
     * @param user_data pointer to opaque AY3_8912 pointer
     * @param raw_buffer buffer to fill with data
     * @param len buffer length
     */
    static void audio_callback(void* user_data, uint8_t* raw_buffer, int len);

    f_read_data_handler m_read_data_handler;
    f_write_data_handler m_write_data_handler;

    uint32_t sound_buffer_index;
    uint32_t sound_buffer_next_play_index;
    int16_t sound_buffer[32768];
    std::mutex buffer_mutex;
    bool start_play;

private:
    inline void write_to_psg(uint8_t value);

    Machine& machine;
    AY3_8912::State state;

    uint32_t cycles_per_sample;
    uint32_t cycle_count;
    bool move_sound_data;

    int16_t cycle_diff;
    boost::circular_buffer<uint32_t> cycle_diffs_buffer;
    uint32_t old_length;
    uint8_t old_length_counter;
};

#endif // AY3_8912_H
