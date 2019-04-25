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

class Oric;
class Frontend;

typedef std::map<int32_t, uint8_t> KeyMap_t;

typedef std::pair<int32_t, bool> KeyPress_t;
typedef std::map<KeyPress_t, KeyPress_t> KeyTranslation_t;

class Machine
{
public:
	Machine(const Oric* a_Oric);
	virtual ~Machine();

	void Init(Frontend* a_Frontend);
	
	Memory& GetMemory() { return m_Memory; }
	MOS6502& GetCPU() { return *m_Cpu; }
	MOS6522& GetVIA() { return *m_Mos_6522; }
	AY3_8912& GetAY3() { return *m_Ay3; }

	void Reset();
	void Run(uint32_t a_Steps, Oric* a_Oric);
	void Run(uint16_t a_Address, long a_Steps, Oric* a_Oric) { m_Cpu->SetPC(a_Address); Run(a_Steps, a_Oric); }
	void Stop() { m_Brk = true; }

	void IRQ() { m_Cpu->IRQ(); }
	
	void KeyPress(uint8_t a_KeyBits, bool a_Down);
	void UpdateKeyOutput();

	static uint8_t read_byte(Machine& a_machine, uint16_t a_Address);
	static uint8_t read_byte_zp(Machine& a_Machine, uint8_t a_Address);

	static uint16_t read_word(Machine& a_Machine, uint16_t a_Address);
	static uint16_t read_word_zp(Machine& a_Machine, uint8_t a_Address);

	static void write_byte(Machine& a_Machine, uint16_t a_Address, uint8_t a_Val);
	static void write_byte_zp(Machine& a_Machine, uint8_t a_Address, uint8_t a_Val);

	static uint8_t read_via_ora(Machine& a_Machine);
	static uint8_t read_via_orb(Machine& a_Machine);
	
	bool m_Brk;

	static const uint8_t cycles_per_raster = 64;
	static const uint16_t raster_max = 312;
	static const uint16_t raster_visible_lines = 224;
	static const uint16_t raster_visible_first = 65;
	static const uint16_t raster_visible_last = raster_visible_first + raster_visible_lines;
	
protected:
	bool PaintRaster(Oric* a_Oric);

	const Oric* m_Oric;
	Frontend* m_Frontend;
	
	MOS6502* m_Cpu;
	MOS6522* m_Mos_6522;
	AY3_8912* m_Ay3;

	Memory m_Memory;

	bool m_Running;

	uint16_t m_RasterCurrent;

	KeyMap_t m_KeyMap;
	KeyTranslation_t m_KeyTranslations;

	uint8_t m_CurrentKeyRow;
	uint8_t m_KeyRows[8];
};

#endif // MACHINE_H
