// =========================================================================
//   Copyright (C) 2009-2026 by Anders Piniesjö <pugo@pugo.org>
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

#ifndef TAPE_TAP_H
#define TAPE_TAP_H

#include <memory>
#include <filesystem>

#include "chip/mos6522.hpp"
#include "tape.hpp"


class TapeTap : public Tape
{
    enum class TapeState {
        Idle,
        ParseHeader,
        Leader,
        Header,
        Gap,
        Body,
        EndOfBlock,
        Fail
    };

    struct TapeHeader {
        uint16_t sync_len;
        uint8_t file_type;
        uint8_t auto_flag;
        uint16_t start_addr, end_addr;
        std::string name;
    };

public:
    TapeTap(MOS6522& via, const std::filesystem::path& path);

    virtual ~TapeTap() = default;

    /**
     * Initialize tape.
     * @return true on success
     */
    bool init() override;

    /**
     * Reset tape postion.
     */
    void reset() override;

    /**
     * Print tape status to console.
     */
    void print_stat() override;

    /**
     * Set motor state.
     * @param motor_on true if motor is on
     */
    void motor_on(bool motor_on) override;

    /**
     * Execute one cycle.
     */
    void exec(uint8_t cycles) override;

protected:
    /**
     * Read tape header.
     * @return true if header is valid
     */
    bool parse_header();

    /**
     * Count sync bytes.
     * @return number of sync bytes
     */
    size_t count_sync_bytes();

    /**
     * Validate header start.
     * @param pos header start position
     * @return true if header start is valid
     */
    bool validate_header_start(size_t pos);

    /**
     * Log file type info.
     * @param file_type file type byte
     * @param auto_flag auto flag byte
     */
    void log_file_type(uint8_t file_type, uint8_t auto_flag);

    /**
     * Read a null-terminated string from tape.
     * @param offset offset to read from. Updated to end of string.
     * @return read string
     */
    std::string read_null_terminated_string(size_t& offset) const;

    /**
     * Get current bit value.
     * @return current bit value
     */
    uint8_t next_bit();

    std::filesystem::path path;
    MOS6522& via;
    size_t tape_size;

    TapeState tape_state;

    uint32_t sync_end;
    uint32_t body_start;
    uint32_t body_remaining;
    bool stopped_mid_byte;

    uint16_t leader_count;
    uint8_t  gap_bits_remaining;

    uint32_t tape_pos;
    uint8_t bit_index;
    uint8_t current_byte;
    uint8_t current_bit;
    uint8_t parity;

    int16_t tape_cycle_counter;
    uint8_t line_out;

    std::vector<uint8_t> memory_vector;
    uint8_t* data;

    static const int Pulse_1 = 208;
    static const int Pulse_0 = 416;
};

#endif // TAPE_TAP_H
