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

#ifndef ROM_PATCHER_H
#define ROM_PATCHER_H

#include "memory.hpp"

struct RomPatch {
    const char* name;
    const char* sha1;
    uint16_t get_sync_pc;
    uint16_t get_sync_end_pc;
    uint16_t get_sync_loop_pc;
    uint16_t read_byte_pc;
    uint16_t read_byte_end_pc;
    std::optional<uint16_t> basic_line_input_pc;
    bool read_byte_set_carry;
    std::optional<uint16_t> read_byte_store_byte_addr;
    std::optional<uint16_t> read_byte_store_zero_addr;
};

class RomPatcher
{
public:
    RomPatcher();

    virtual ~RomPatcher() = default;

    /**
     * Find patch information for a ROM.
     * @return patch if the ROM is supported.
     */
    static const RomPatch* find_patch(const Memory& rom);
};



#endif // ROM_PATCHER_H
