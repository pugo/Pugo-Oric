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

#include "chip/mos6502.hpp"
#include "chip/mos6522.hpp"
#include "chip/ay3_8912.hpp"

class Oric;
class Memory;
class Frontend;

typedef std::map<uint32_t, uint8_t> KeyMap_t;

class Machine : public std::enable_shared_from_this<Machine>
{
public:
	Machine(std::shared_ptr<Oric> a_Oric);
	virtual ~Machine();

	void Init(std::shared_ptr<Frontend> a_Frontend);
	
	std::shared_ptr<Memory> GetMemory() { return m_Memory; }
	std::shared_ptr<MOS6502> GetCPU() { return m_Cpu; }
	std::shared_ptr<MOS6522> GetVIA() { return m_Mos_6522; }

	void Reset();
	void Run(uint32_t a_Steps, Oric* a_Oric);
	void Run(uint16_t a_Address, long a_Steps, Oric* a_Oric) { m_Cpu->SetPC(a_Address); Run(a_Steps, a_Oric); }
	void Stop() { m_Brk = true; }

	static inline uint8_t read_byte(Machine& a_machine, uint16_t a_Address);
	static inline uint8_t read_byte_zp(Machine& a_Machine, uint8_t a_Address);

	static inline uint16_t read_word(Machine& a_Machine, uint16_t a_Address);
	static inline uint16_t read_word_zp(Machine& a_Machine, uint8_t a_Address);

	static inline void write_byte(Machine& a_Machine, uint16_t a_Address, uint8_t a_Val);
	static inline void write_byte_zp(Machine& a_Machine, uint8_t a_Address, uint8_t a_Val);

	bool m_Brk;

	static const uint8_t cycles_per_raster = 64;
	static const uint16_t raster_max = 312;
	static const uint16_t raster_visible_lines = 224;
	static const uint16_t raster_visible_first = 65;
	static const uint16_t raster_visible_last = raster_visible_first + raster_visible_lines;
	
protected:
	bool PaintRaster(Oric* a_Oric);

	uint16_t m_RasterCurrent;
	
	std::shared_ptr<Oric> m_Oric;
	std::shared_ptr<Frontend> m_Frontend;
	
	std::shared_ptr<MOS6502> m_Cpu;
	std::shared_ptr<MOS6522> m_Mos_6522;
	std::shared_ptr<AY3_8912> m_Ay3;

	std::shared_ptr<Memory> m_Memory;

	KeyMap_t m_KeyMap;

	bool m_Running;
};

#endif // MACHINE_H
