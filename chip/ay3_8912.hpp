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
#include <array>

class Snapshot;
class Machine;

typedef uint8_t (*f_read_data_handler)(Machine &oric);
typedef uint8_t (*f_write_data_handler)(Machine &oric);

constexpr size_t register_changes_size = 32768;


class Channel
{
public:
    Channel();

    void reset();

    void print_status(uint8_t channel) {
        std::cout << " ------- Channel " << (int)channel << " -------------------------" << std::endl;
        std::cout << "    -         Volume: " << (int)volume << std::endl;
        std::cout << "    -    Tome period: " << (int)tone_period << std::endl;
        std::cout << "    -        Counter: " << (int)counter << std::endl;
        std::cout << "    -          Value: " << (int)value << std::endl;
        std::cout << "    -       Disabled: " << (disabled ? "true" : "false") << std::endl;
        std::cout << "    - Noise disabled: " << (noise_diabled ? "true" : "false") << std::endl;
        std::cout << "    -   Use envelope: " << (use_envelope ? "true" : "false") << std::endl;
        std::cout << std::endl;
    }

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

    void print_status() {
        std::cout << " ------- Noise -------------------------" << std::endl;
        std::cout << "    -  Period: " << (int)period << std::endl;
        std::cout << "    - Counter: " << (int)counter << std::endl;
        std::cout << "    -     Bit: " << (int)bit << std::endl;
        std::cout << "    -     Rng: " << (int)rng << std::endl;
        std::cout << std::endl;
    }

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


struct RegisterChange
{
    uint32_t cycle;
    uint8_t register_index;
    uint8_t value;
};

class State;

class RegisterChanges
{
public:
    RegisterChanges();

    void reset();
    void exec();

    std::array<RegisterChange, register_changes_size> changes;
    uint32_t changes_count;

    uint32_t new_log_cycle;
    uint32_t log_cycle;
    bool update_log_cycle;
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

    struct SoundState
    {
        void reset();
        void print_status();

        void write_register_change(uint8_t value);

        void exec_register_changes(uint32_t& changes_written, uint32_t cycle) {
            while ((changes_written < changes.changes_count) &&
                   (cycle >= changes.changes[changes_written].cycle))
            {
                exec_register_change(changes.changes[changes_written++]);
            }
        }

        void exec_register_change(RegisterChange& register_change);

        void trim_register_changes(uint32_t& changes_written);

        /**
         * Execute audio a number of clock cycles.
          */
        void exec_audio(uint32_t cycle);

        bool bdir;
        bool bc1;
        bool bc2;

        uint8_t current_register;
        uint8_t registers[NUM_REGS];
        uint8_t audio_registers[NUM_REGS];
        uint32_t audio_out;

        RegisterChanges changes;
        Channel channels[3];
        Noise noise;
        Envelope envelope;

        uint32_t cycles_per_sample;
        uint32_t cycle_count;
        uint32_t last_cycle;
    };


    AY3_8912(Machine& machine);
    ~AY3_8912();

    /**
     * Reset the AY-3-8912.
     */
    void reset();

    /**
     * Print AY-3-8912 state to console.
     */
    void print_status();

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
     * Execute a number of clock cycles.
     */
    short exec();

    /**
     * Update AY state based on BC1 and BDIR.
     */
    void update_state();

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
     * Update state - callback function.
     * @param machine Machine object for current machine
     */
    static void update_state_callback(Machine& machine);

    /**
     * Called by frontend to get audio.
     * @param user_data pointer to opaque AY3_8912 pointer
     * @param raw_buffer buffer to fill with data
     * @param len buffer length
     */
    static void audio_callback(void* user_data, uint8_t* raw_buffer, int len);

//    void write_register_change(RegisterChange& register_change);

    f_read_data_handler m_read_data_handler;
    f_write_data_handler m_write_data_handler;

private:
    Machine& machine;
    SoundState state;
};

#endif // AY3_8912_H
