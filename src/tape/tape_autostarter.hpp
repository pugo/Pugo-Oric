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

#ifndef TAPE_AUTOSTARTER_H
#define TAPE_AUTOSTARTER_H

#include <array>

#include "tape.hpp"

class Memory;
class MOS6502;
class Machine;
class Tape;

// ORIC char codes for: CLOAD"
constexpr std::array<uint8_t, 8> cload_input = {
    23, 57, 42, 53, 15, 128 + 31, 128 + 31, 61
};

constexpr uint8_t cload_input_shift = 0x80;
constexpr uint8_t cload_input_length = 8;


class TapeAutostarter
{
    enum class AutostartState {
        WaitingForBasicInput,
        PressKey,
        ReleaseKey,
        Done,
    };

public:
    TapeAutostarter();
    virtual ~TapeAutostarter() = default;

    bool exec(Machine& machine, Tape& tape);


protected:
    AutostartState autostart_state;
    uint8_t cload_input_index;
    uint8_t delay_counter;
};

#endif // TAPE_AUTOSTARTER_H
