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

#include <array>
#include <boost/log/trivial.hpp>
#include <boost/uuid/detail/sha1.hpp>
#include <format>
#include <iomanip>
#include <sstream>

#include "chip/mos6502.hpp"
#include "memory.hpp"
#include "tape_tap_turbo.hpp"


namespace {
constexpr std::array<TapeTapTurbo::TapeRomPatch, 2> tape_rom_patches{{
    {
        .name = "Oric Atmos BASIC 1.1b",
        .sha1 = "9451a1a09d8f75944dbd6f91193fc360f1de80ac",
        .get_sync_pc = 0xe735,
        .get_sync_end_pc = 0xe759,
        .get_sync_loop_pc = 0xe720,
        .read_byte_pc = 0xe6c9,
        .read_byte_end_pc = 0xe6fb,
        .read_byte_set_carry = true,
        .read_byte_store_byte_addr = 0x002f,
        .read_byte_store_zero_addr = 0x02b1,
    },
    {
        .name = "Oric-1 BASIC 1.0",
        .sha1 = "333116e6884d85aaa4dfc7578a91cceeea66d016",
        .get_sync_pc = 0xe696,
        .get_sync_end_pc = 0xe6b9,
        .get_sync_loop_pc = 0xe681,
        .read_byte_pc = 0xe630,
        .read_byte_end_pc = 0xe65b,
        .read_byte_set_carry = false,
        .read_byte_store_byte_addr = 0x002f,
        .read_byte_store_zero_addr = std::nullopt,
    },
}};

std::string sha1_hex(const uint8_t* data, size_t size)
{
    boost::uuids::detail::sha1 sha1;
    sha1.process_bytes(data, size);

    boost::uuids::detail::sha1::digest_type digest;
    sha1.get_digest(digest);

    std::ostringstream result;
    result << std::hex << std::setfill('0');

    for (const uint8_t byte : digest) {
        result << std::setw(2) << static_cast<unsigned int>(byte);
    }

    return result.str();
}
}


TapeTapTurbo::TapeTapTurbo(MOS6522& via, const std::filesystem::path& path, const TapeRomPatch& patch) :
    TapeTapNormal(via, path),
    patch(patch),
    turbo_loading(false)
{
    BOOST_LOG_TRIVIAL(info) << "Tape: turbo supported for " << patch.name;
}


void TapeTapTurbo::reset()
{
    turbo_loading = false;

    TapeTapNormal::reset();
}


void TapeTapTurbo::motor_on(bool motor_on)
{
    if (!motor_on) {
        if (turbo_loading) {
            reset_normal_timing_state();
        }
        turbo_loading = false;
    }

    TapeTapNormal::motor_on(motor_on);
}


void TapeTapTurbo::exec(uint8_t cycles)
{
    if (turbo_loading && motor_running) {
        via.write_cb1(true);
        return;
    }

    TapeTapNormal::exec(cycles);
}


bool TapeTapTurbo::intercept(MOS6502& cpu, Memory& ram, bool oric_rom_enabled)
{
    if (!motor_running || !oric_rom_enabled || !has_tap_data()) {
        return false;
    }

    if (intercept_sync(cpu)) {
        return true;
    }

    return intercept_read_byte(cpu, ram);
}


const TapeTapTurbo::TapeRomPatch* TapeTapTurbo::find_patch(const Memory& rom)
{
    const std::string rom_sha1 = sha1_hex(rom.mem, rom.get_size());

    for (const auto& candidate : tape_rom_patches) {
        if (rom_sha1 == candidate.sha1) {
            return &candidate;
        }
    }

    BOOST_LOG_TRIVIAL(info) << "Tape: turbo not supported for ROM SHA-1 " << rom_sha1;
    return nullptr;
}

void TapeTapTurbo::reset_normal_timing_state()
{
    current_byte = 0;
    current_bit = 0;
    parity = 0;
    tape_cycle_counter = 0;
    gap_bits_remaining = 0;
    line_out = 0;
    bit_index = 0;
    stopped_mid_byte = false;
}


bool TapeTapTurbo::intercept_sync(MOS6502& cpu)
{
    const uint16_t pc = cpu.get_pc();
    if (pc != patch.get_sync_pc && pc != patch.get_sync_loop_pc) {
        return false;
    }

    if (!seek_next_sync()) {
        turbo_loading = false;
        return false;
    }

    reset_normal_timing_state();
    turbo_loading = true;
    cpu.set_pc(patch.get_sync_end_pc);
    return true;
}


bool TapeTapTurbo::intercept_read_byte(MOS6502& cpu, Memory& ram)
{
    if (cpu.get_pc() != patch.read_byte_pc) {
        return false;
    }

    auto byte = read_next_tap_byte();
    if (!byte) {
        turbo_loading = false;
        return false;
    }

    cpu.A = *byte;
    cpu.N_INTERN = *byte;
    cpu.Z_INTERN = *byte;
    cpu.C = patch.read_byte_set_carry;

    if (patch.read_byte_store_byte_addr) {
        ram.mem[*patch.read_byte_store_byte_addr] = *byte;
    }

    if (patch.read_byte_store_zero_addr) {
        ram.mem[*patch.read_byte_store_zero_addr] = 0x00;
    }

    cpu.set_pc(patch.read_byte_end_pc);
    return true;
}
