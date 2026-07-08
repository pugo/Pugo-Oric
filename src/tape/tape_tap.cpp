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

#include "logging.hpp"
#include <fstream>
#include <cstdlib>
#include <format>
#include <optional>
#include <print>
#include <vector>
#include <string>

#include "tape_tap.hpp"


TapeTap::TapeTap(MOS6522& via, const std::filesystem::path& path) :
    via(via),
    path(path),
    tape_size(0),
    tape_state(TapeState::Idle),
    sync_end(0),
    body_start(0),
    body_remaining(0),
    leader_count(0),
    tape_pos(0),
    bit_index(0),
    stopped_mid_byte(false),
    data(nullptr)
{
}

void TapeTap::reset()
{
    motor_running = false;
    tape_state = TapeState::Idle;
    sync_end = 0;
    body_start = 0;
    body_remaining = 0;
    leader_count = 0;
    tape_pos = 0;
    bit_index = 0;
    stopped_mid_byte = false;
}


bool TapeTap::init()
{
    reset();
    AURIC_LOG(info) << "Tape: Reading TAP file '" << path << "'";

    if (!std::filesystem::exists(path)) {
        AURIC_LOG(warning) << "Tape: TAP file does not exist";
        return false;
    }

    tape_size = std::filesystem::file_size(path);

    std::ifstream file(path, std::ios::binary);
    if (!file) {
        AURIC_LOG(warning) << "Tape: unable to open TAP file";
        return false;
    }

    memory_vector.assign(std::istreambuf_iterator<char>(file), {});
    data = memory_vector.data();

    return true;
}


void TapeTap::print_stat()
{
    std::println("Current Tape pos: {}", tape_pos);
}


void TapeTap::motor_on(bool motor_on)
{
    if (motor_on == motor_running) {
        return;
    }
    AURIC_LOG(debug) << "Tape: motor " << (motor_on ? "on" : "off");

    motor_running = motor_on;

    if (motor_on) {
        if (stopped_mid_byte) {
            ++tape_pos;
            stopped_mid_byte = false;
        }
        tape_state = TapeState::ParseHeader;
    }
    else {
        if (bit_index > 0) {
            // stopped mid-byte: drop the partial byte on resume
            AURIC_LOG(debug) << "Skipped one byte at resume (pos now " << tape_pos << ")";
            stopped_mid_byte = true;
            bit_index = 0;
        }
    }
}

bool TapeTap::has_tap_data() const
{
    return data != nullptr && tape_size > 0;
}


bool TapeTap::seek_next_sync()
{
    if (!has_tap_data()) {
        return false;
    }

    const uint32_t original_tape_pos = tape_pos;

    while (tape_pos < tape_size) {
        if (data[tape_pos] != 0x16) {
            ++tape_pos;
            continue;
        }

        const size_t sync_start = tape_pos;
        size_t sync_len = 0;

        while (sync_start + sync_len < tape_size && data[sync_start + sync_len] == 0x16) {
            ++sync_len;
        }

        if (sync_len >= 3 && sync_start + sync_len < tape_size && data[sync_start + sync_len] == 0x24) {
            tape_pos = sync_start + sync_len;
            AURIC_LOG(debug) << std::format("Tape: turbo sync at ${:x}", tape_pos);
            return true;
        }

        tape_pos = sync_start + 1;
    }

    tape_pos = original_tape_pos;
    return false;
}


std::optional<uint8_t> TapeTap::read_next_tap_byte()
{
    if (!has_tap_data() || tape_pos >= tape_size) {
        return std::nullopt;
    }

    return data[tape_pos++];
}


bool TapeTap::parse_header()
{
    size_t i = count_sync_bytes();
    uint16_t sync_len = i;
    sync_end = tape_pos + i;

    if (!validate_header_start(i)) {
        return false;
    }

    // Skip end of sync and reserved bytes.
    i += 1 + 2;

    auto file_type = data[tape_pos + i++];
    uint8_t auto_flag = data[tape_pos + i++];
    log_file_type(file_type, auto_flag);

    const bool basic_mode = (file_type == 0x00) || (auto_flag == 0x80);
    size_t desired_sync = basic_mode ? 192 : 112;

    const uint16_t end_address = data[tape_pos + i] << 8 | data[tape_pos + i + 1];
    i += 2;
    const uint16_t start_address = data[tape_pos + i] << 8 | data[tape_pos + i + 1];
    i += 2;

    AURIC_LOG(debug) << std::format("Tape: start address: ${:04x}", start_address);
    AURIC_LOG(debug) << std::format("Tape:   end address: ${:04x}", end_address);

    // Skip one reserved byte.
    i++;

    // Read variable-length name
    const std::string name = read_null_terminated_string(i);
    AURIC_LOG(info) << "Tape: file name: " << name;

    // Store where body starts, to allow delay after header.
    body_start = tape_pos + i + 1;
    body_remaining = uint32_t(end_address) - size_t(start_address) + 1;
    leader_count = (sync_len < desired_sync) ? (desired_sync - sync_len) : 0;

    return true;
}


size_t TapeTap::count_sync_bytes()
{
    size_t i{0};

    while (true) {
        if (tape_pos + i >= tape_size) {
            return false;
        }

        if (data[tape_pos + i] != 0x16) {
            break;
        }
        ++i;
    }

    return i;
}


bool TapeTap::validate_header_start(size_t pos)
{
    AURIC_LOG(debug) << "Tape: found " << pos << " sync bytes (0x16)";

    if (pos < 3) {
        AURIC_LOG(warning) << "Tape: too few sync bytes, failing.";
        return false;
    }

    if (data[tape_pos + pos] != 0x24) {
        AURIC_LOG(warning) << "Tape: missing end of sync bytes (0x24), failing.";
        return false;
    }

    ++pos;

    if (pos + 9 >= tape_size) {
        AURIC_LOG(warning) << "Tape: too short (no specs and addresses).";
        return false;
    }

    return true;
}


void TapeTap::log_file_type(uint8_t file_type, uint8_t auto_flag)
{
    switch(file_type)
    {
        case 0x00:
            AURIC_LOG(debug) << "Tape: file is BASIC.";
            break;
        case 0x80:
            AURIC_LOG(debug) << "Tape: file is machine code.";
            break;
        default:
            AURIC_LOG(debug) << "Tape: file is unknown.";
            break;
    }

    switch(auto_flag)
    {
        case 0x80:
            AURIC_LOG(debug) << "Tape: run automatically as BASIC.";
            break;
        case 0xc7:
            AURIC_LOG(debug) << "Tape: run automatically as machine code.";
            break;
        default:
            AURIC_LOG(debug) << "Tape: Don't run automatically.";
            break;
    }
}


std::string TapeTap::read_null_terminated_string(size_t& offset) const
{
    std::string result;

    while (true) {
        if (tape_pos + offset >= tape_size) {
            return result;
        }

        if (data[tape_pos + offset] == 0x00) {
            break;
        }

        result += data[tape_pos + offset];
        ++offset;
    }

    return result;
}
