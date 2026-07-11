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

#include <spdlog/spdlog.h>
#include <format>
#include <fstream>

#include "rom_patcher.hpp"
#include "utils/sha1.hpp"

namespace {
constexpr std::array<RomPatch, 2> rom_patches{{
    {
        .name = "Oric Atmos BASIC 1.1b",
        .sha1 = "9451a1a09d8f75944dbd6f91193fc360f1de80ac",
        .get_sync_pc = 0xe735,
        .get_sync_end_pc = 0xe759,
        .get_sync_loop_pc = 0xe720,
        .read_byte_pc = 0xe6c9,
        .read_byte_end_pc = 0xe6fb,
        .basic_line_input_pc = 0xc5e8,
        .read_byte_set_carry = false,
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
        .basic_line_input_pc = 0xc5f7,
        .read_byte_set_carry = false,
        .read_byte_store_byte_addr = 0x002f,
        .read_byte_store_zero_addr = std::nullopt,
    },
}};
} // namespace


RomPatcher::RomPatcher()
{
}


const RomPatch* RomPatcher::find_patch(const Memory& rom)
{
    const std::string rom_sha1 = utils::sha1_hex(rom.mem, rom.get_size());

    for (const auto& candidate : rom_patches) {
        if (rom_sha1 == candidate.sha1) {
            return &candidate;
        }
    }

    spdlog::info("Tape: turbo not supported for ROM SHA-1 {}", rom_sha1);
    return nullptr;
}

