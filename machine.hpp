// =========================================================================
//   Copyright (C) 2009-2014 by Anders Piniesjö <pugo@pugo.org>
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

#ifndef MACHINE_H
#define MACHINE_H

#include <iostream>
#include <memory>
#include <map>

#include "chip/mos6502.hpp"
#include "chip/mos6522.hpp"
#include "chip/ay3_8912.hpp"
#include "memory.hpp"
#include "tape_tap.hpp"

class Oric;
class Frontend;

typedef std::map<int32_t, uint8_t> KeyMap_t;

typedef std::pair<int32_t, bool> KeyPress_t;
typedef std::map<KeyPress_t, KeyPress_t> KeyTranslation_t;

class Machine
{
public:
	Machine(const Oric* oric);
	virtual ~Machine();

	void init(Frontend* frontend);
	
	Memory& get_memory() { return memory; }
	MOS6502& get_cpu() { return *cpu; }
	MOS6522& get_via() { return *mos_6522; }
	AY3_8912& get_ay3() { return *ay3; }

	void reset();
	void run(uint32_t steps, Oric* oric);
	void run(uint16_t address, long steps, Oric* oric) { cpu->set_pc(address); run(steps, oric); }
	void stop() { break_exec = true; }

	void irq() { cpu->IRQ(); }
	
	void key_press(uint8_t a_KeyBits, bool a_Down);
	void update_key_output();
	void via_orb_changed(uint8_t a_Orb);

	static uint8_t read_byte(Machine& a_machine, uint16_t a_Address);
	static uint8_t read_byte_zp(Machine& a_Machine, uint8_t a_Address);

	static uint16_t read_word(Machine& a_Machine, uint16_t a_Address);
	static uint16_t read_word_zp(Machine& a_Machine, uint8_t a_Address);

	static void write_byte(Machine& a_Machine, uint16_t a_Address, uint8_t a_Val);
	static void write_byte_zp(Machine& a_Machine, uint8_t a_Address, uint8_t a_Val);

	static uint8_t read_via_ora(Machine& a_Machine);
	static uint8_t read_via_orb(Machine& a_Machine);
	
	static const uint8_t cycles_per_raster = 64;
	static const uint16_t raster_max = 312;
	static const uint16_t raster_visible_lines = 224;
	static const uint16_t raster_visible_first = 65;
	static const uint16_t raster_visible_last = raster_visible_first + raster_visible_lines;
	
protected:
	bool paint_raster(Oric* oric);

	const Oric* oric;
	Frontend* frontend;
	
	MOS6502* cpu;
	MOS6522* mos_6522;
	AY3_8912* ay3;
	bool break_exec;

	Memory memory;
	TapeTap* tape;

	bool is_running;
	bool warpmode_on;

	uint16_t raster_current;

	KeyMap_t key_map;
	KeyTranslation_t key_translations;

	uint8_t current_key_row;
	uint8_t key_rows[8];
};

#endif // MACHINE_H
