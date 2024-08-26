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

#ifndef MEMORY_H
#define MEMORY_H

#include <iostream>
#include <ostream>
#include <cstdint>
#include <vector>


class Snapshot;


class Memory
{
public:
    Memory(uint32_t size);
    ~Memory();

    void load(const std::string& path, uint32_t address);
    uint32_t get_size() { return size; }

    void save_to_snapshot(Snapshot& snapshot);
    void load_from_snapshot(Snapshot& snapshot);

    void set_mem_pos(uint16_t address) { mempos = address; }

    friend Memory& operator<<(Memory& os, unsigned int in) {
        os.mem[os.mempos++] = static_cast<uint8_t>(in & 0xff);
        return os;
    }

    void show(uint32_t pos, uint32_t length);

    std::vector<uint8_t> memory;
    uint8_t* mem;

protected:
    uint32_t size;
    uint32_t mempos;
};



#endif // MEMORY_H
