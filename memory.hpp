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

#ifndef MEMORY_H
#define MEMORY_H

#include <iostream>
#include <ostream>

class Memory
{
public:
    Memory(uint32_t a_Size);
    ~Memory();

    void load(const std::string& path, uint32_t address);
    uint32_t get_size() { return size; }

    void set_mem_pos(uint16_t a_Address) { mempos = a_Address; }

    friend Memory& operator<<(Memory& a_Os, unsigned int a_In) {
        a_Os.mem[a_Os.mempos++] = static_cast<uint8_t>(a_In & 0xff);
        return a_Os;
    }

    void show(uint32_t a_Pos, uint32_t a_Length);

    uint8_t* mem;

protected:
    uint32_t size;
    uint32_t mempos;
};



#endif // MEMORY_H
