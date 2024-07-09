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

#ifndef MONITOR_H
#define MONITOR_H

#include <stdio.h>
#include <string.h>
#include <iostream>

#include <map>
#include <vector>

#include "memory.hpp"


enum class Addressing
{
    immediate,
    absolute,
    zero_page,
    implied,
    indirect_absolute,
    absolute_indexed_x,
    absolute_indexed_y,
    zero_page_indexed_x,
    zero_page_indexed_y,
    indexed_indirect_x,
    indirect_indexed_y,
    relative,
    accumulator
};


struct Opcode
{
    uint8_t opcode;
    std::string name;
    Addressing addressing;
};


class Monitor
{
public:
    Monitor(Memory& memory);

    uint16_t disassemble(uint16_t address);
    uint16_t disassemble(uint16_t address, size_t bytes);

private:
    Memory& memory;

    std::map<uint8_t, Opcode> opcodes;
};


#endif // MONITOR_H