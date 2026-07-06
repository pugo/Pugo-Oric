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

#ifndef TAPE_TAP_TURBO_H
#define TAPE_TAP_TURBO_H

#include <filesystem>
#include <optional>

#include "tape_tap_normal.hpp"

class Memory;
class MOS6502;


class TapeTapTurbo : public TapeTapNormal
{
public:
    struct TapeRomPatch {
        const char* name;
        const char* sha1;
        uint16_t get_sync_pc;
        uint16_t get_sync_end_pc;
        uint16_t get_sync_loop_pc;
        uint16_t read_byte_pc;
        uint16_t read_byte_end_pc;
        bool read_byte_set_carry;
        std::optional<uint16_t> read_byte_store_byte_addr;
        std::optional<uint16_t> read_byte_store_zero_addr;
    };

    TapeTapTurbo(MOS6522& via, const std::filesystem::path& path, const TapeRomPatch& patch);

    virtual ~TapeTapTurbo() = default;

    /**
     * Find turbo tape patch information for a ROM.
     * @return patch if the ROM is supported.
     */
    static const TapeRomPatch* find_patch(const Memory& rom);

    /**
     * Reset tape postion.
     */
    void reset() override;

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
     * Intercept a ROM tape routine before CPU opcode fetch.
     * @return true if the tape handled this CPU step.
     */
    bool intercept(MOS6502& cpu, Memory& ram, bool oric_rom_enabled);

protected:
    void reset_normal_timing_state();
    bool intercept_sync(MOS6502& cpu);
    bool intercept_read_byte(MOS6502& cpu, Memory& ram);

    const TapeRomPatch& patch;
    bool turbo_loading;
};

#endif // TAPE_TAP_TURBO_H
