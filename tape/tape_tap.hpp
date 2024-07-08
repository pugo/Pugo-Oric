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

#ifndef TAPE_TAP_H
#define TAPE_TAP_H

#include <iostream>
#include <memory>
#include <map>

#include "chip/mos6522.hpp"
#include "tape.hpp"


class TapeTap : public Tape
{
public:
    TapeTap(MOS6522& via, const std::string& path);
    virtual ~TapeTap();

    bool init() override;
    void reset() override;

    void print_stat() override;

    void set_motor(bool motor_on) override;
    short exec(uint8_t cycles) override;


protected:
    bool read_header();
    uint8_t get_current_bit();

    std::string path;
    MOS6522& via;
    size_t size;
    size_t body_start;

    int32_t delay;
    int32_t duplicate_bytes;

    uint32_t tape_pos;
    uint8_t bit_count;
    uint8_t current_bit;
    uint8_t parity;

    int16_t tape_cycles_counter;
    uint8_t tape_pulse;

    uint8_t* data;

    static const int Pulse_1 = 208;
    static const int Pulse_0 = 416;
};

#endif // TAPE_TAP_H
