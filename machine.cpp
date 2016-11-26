// Copyright (C) 2009-2016 by Anders Piniesjö <pugo@pugo.org>

#include <unistd.h>
#include <iostream>
#include <sstream>
#include <cstdlib>
#include <vector>
#include <string>

#include "machine.hpp"
#include "oric.hpp"
#include "memory.hpp"
#include "frontend.hpp"


Machine::Machine(std::shared_ptr<Oric> a_Oric) : 
	m_Oric(a_Oric),
	m_Running(false),
	m_Brk(false),
	m_RasterCurrent(0)
{
}

Machine::~Machine()
{}

void Machine::Init(std::shared_ptr<Frontend> a_Frontend)
{
	m_Frontend = a_Frontend;
	m_Memory = std::make_shared<Memory>(65536);
	m_Cpu = std::make_shared<MOS6502>(shared_from_this());
	m_Mos_6522 = std::make_shared<MOS6522>(shared_from_this());

	m_Cpu->memory_read_byte_handler = read_byte;
	m_Cpu->memory_read_byte_zp_handler = read_byte_zp;
	m_Cpu->memory_read_word_handler = read_word;
	m_Cpu->memory_read_word_zp_handler = read_word_zp;
	m_Cpu->memory_write_byte_handler = write_byte;
	m_Cpu->memory_write_byte_zp_handler = write_byte_zp;
}

void Machine::Reset()
{
	m_Cpu->Reset();
}

/**
 * Run machine.
 * \param a_Instructions number of instructions to run. If 0: run infinite (or to BRK).
 */
void Machine::Run(uint32_t a_Instructions, Oric* a_Oric)
{
	uint32_t instructions = 0;
	m_Brk = false;

	while (! m_Brk) {
		int32_t cycles = cycles_per_raster;

		while (cycles > 0) {
			cycles -= m_Cpu->ExecInstruction(m_Brk);
			m_Mos_6522->Exec();

			if (a_Instructions > 0 && ++instructions == a_Instructions) {
				return;
			}
		}

		PaintRaster(a_Oric);
	}
}

bool Machine::PaintRaster(Oric* a_Oric)
{
	if (m_RasterCurrent >= raster_visible_first && m_RasterCurrent < raster_visible_last) {
		m_Frontend->UpdateGraphics(m_RasterCurrent - raster_visible_first, m_Memory->m_Mem);
	}
	
	if (++m_RasterCurrent == raster_max) {
		m_RasterCurrent = 0;
		m_Frontend->RenderGraphics();
		return true;
	}

	return false;
}

// --- Memory functions

uint8_t inline Machine::read_byte(Machine& a_Machine, uint16_t a_Address)
{
	if (a_Address >= 0x300 && a_Address < 0x400) {
// 		std::cout << "read: " << std::hex << a_Address << std::endl;
		return a_Machine.GetVIA()->ReadByte(a_Address);
	}

	return a_Machine.GetMemory()->m_Mem[a_Address];
}

uint8_t inline Machine::read_byte_zp(Machine &a_Machine, uint8_t a_Address)
{
	return a_Machine.GetMemory()->m_Mem[a_Address];
}

uint16_t inline Machine::read_word(Machine &a_Machine, uint16_t a_Address)
{
	if (a_Address >= 0x300 && a_Address < 0x400) {
		std::cout << "read word: " << std::hex << a_Address << std::endl;
	}

	return a_Machine.GetMemory()->m_Mem[a_Address] | a_Machine.GetMemory()->m_Mem[a_Address + 1] << 8;
}

uint16_t inline Machine::read_word_zp(Machine &a_Machine, uint8_t a_Address)
{
	return a_Machine.GetMemory()->m_Mem[a_Address] | a_Machine.GetMemory()->m_Mem[a_Address+1 & 0xff] << 8;
}

void inline Machine::write_byte(Machine &a_Machine, uint16_t a_Address, uint8_t a_Val)
{
	if (a_Address >= 0xc000) {
		return;
	}
	
	if (a_Address >= 0x300 && a_Address < 0x400) {
		a_Machine.GetVIA()->WriteByte(a_Address, a_Val);
	}

	a_Machine.GetMemory()->m_Mem[a_Address] = a_Val;
}

void inline Machine::write_byte_zp(Machine &a_Machine, uint8_t a_Address, uint8_t a_Val)
{
	if (a_Address >= 0x00ff) {
		return;
	}

	a_Machine.GetMemory()->m_Mem[a_Address] = a_Val;
}

