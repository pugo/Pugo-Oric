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
    void disassemble(uint16_t address, size_t bytes);

private:
    Memory& memory;

    std::map<uint8_t, Opcode> opcodes;
};


#endif // MONITOR_H