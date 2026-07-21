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

#ifndef TAPE_BLANK_H
#define TAPE_BLANK_H

#include "tape.hpp"

class Memory;
class MOS6502;

class TapeBlank : public Tape
{
public:
    TapeBlank();
    virtual ~TapeBlank();

    /**
     * Initialize tape.
     * @return true on success
     */
    bool init() override;

    /**
     * Reset tape postion.
     */
    void reset() override;

    /**
     * Print tape status to console.
     */
    void print_stat() override;

    /**
     * Set motor state.
     * @param motor_on true if motor is on
     */
    void motor_on(bool motor_on) override;

    /**
     * Execute one cycle.
     */
    void exec(uint8_t cycles) override;

    /**
     * Intercept a ROM tape routine before CPU opcode fetch. Allows turbo loading and saving.
     * @param cpu reference to CPU
     * @param ram reference to RAM
     * @param oric_rom_enabled true if Oric ROM is enabled
     * @return true if the tape handled this CPU step.
     */
    bool intercept(MOS6502& cpu, Memory& ram, bool oric_rom_enabled) override { return false; };

protected:
};

#endif // TAPE_BLANK_H
