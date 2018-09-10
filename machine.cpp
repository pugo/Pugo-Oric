// Copyright (C) 2009-2016 by Anders Piniesjö <pugo@pugo.org>

#include <unistd.h>
#include <iostream>
#include <sstream>
#include <cstdlib>
#include <vector>
#include <string>
#include <boost/assign.hpp>

#include "machine.hpp"
#include "oric.hpp"
#include "memory.hpp"
#include "frontend.hpp"


static int32_t keytab[] = {'7'       , 'n'        , '5'        , 'v'        , 0 ,          '1'        , 'x'        , '3'        ,
									'j'        , 't'        , 'r'        , 'f'        , 0          , SDLK_ESCAPE, 'q'        , 'd'        ,
									'm'        , '6'        , 'b'        , '4'        , SDLK_LCTRL , 'z'        , '2'        , 'c'        ,
									'k'        , '9'        , ';'        , '-'        , 0          , 0          , '\\'       , '\''       ,
									SDLK_SPACE , ','        , '.'        , SDLK_UP    , SDLK_LSHIFT, SDLK_LEFT  , SDLK_DOWN  , SDLK_RIGHT ,
									'u'        , 'i'        , 'o'        , 'p'        , SDLK_LALT  , SDLK_BACKSPACE, ']'     , '['        ,
									'y'        , 'h'        , 'g'        , 'e'        , 0          , 'a'        , 's'        , 'w'        ,
									'8'        , 'l'        , '0'        , '/'        , SDLK_RSHIFT, SDLK_RETURN, 0          , SDLK_EQUALS };


Machine::Machine(const Oric* a_Oric) : 
	m_Oric(a_Oric),
	m_Memory(65535),
	m_Running(false),
	m_Brk(false),
	m_RasterCurrent(0),
	m_CurrentKeyRow(0)
{
	for (uint8_t i=0; i < 8; i++) {
		m_KeyRows[i] = 0;
	}

	boost::assign::insert(m_KeyTranslations)
		(std::make_pair(0xe4, false), std::make_pair('/', false))
		(std::make_pair(0xe4, true), std::make_pair('/', false))
		(std::make_pair(0xf6, false), std::make_pair(';', false))
		(std::make_pair(0xf6, true), std::make_pair(';', false))
		(std::make_pair(0x2b, false), std::make_pair('=', false))
		(std::make_pair(0x2b, true), std::make_pair('=', false));
}

Machine::~Machine()
{}

void Machine::Init(Frontend* a_Frontend)
{
	m_Frontend = a_Frontend;
	m_Cpu = new MOS6502(*this);
	m_Mos_6522 = new MOS6522(*this);
	m_Ay3 = new AY3_8912(*this);

	m_Cpu->memory_read_byte_handler = read_byte;
	m_Cpu->memory_read_byte_zp_handler = read_byte_zp;
	m_Cpu->memory_read_word_handler = read_word;
	m_Cpu->memory_read_word_zp_handler = read_word_zp;
	m_Cpu->memory_write_byte_handler = write_byte;
	m_Cpu->memory_write_byte_zp_handler = write_byte_zp;
	
	m_Ay3->m_read_data_handler = read_via_ora;
	
	m_Mos_6522->ca2_changed_handler = AY3_8912::set_bc1;
	m_Mos_6522->cb2_changed_handler = AY3_8912::set_bdir;
	
	for (uint8_t i=0; i < 64; ++i) {
		m_KeyMap[keytab[i]] = i;
	}
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

	uint32_t now = SDL_GetTicks();
	uint64_t next_frame = static_cast<uint64_t>(now) * 1000;
	SDL_Event event;
 
	m_Brk = false;
	while (! m_Brk) {
		int32_t cycles = cycles_per_raster;

		while (cycles > 0) {
			uint8_t ran = m_Cpu->ExecInstruction(m_Brk);
			cycles -= ran;
			m_Mos_6522->Exec(ran);
			m_Ay3->Exec(ran);

			UpdateKeyOutput();

			if (a_Instructions > 0 && ++instructions == a_Instructions) {
				return;
			}
		}

		if (PaintRaster(a_Oric)) {
			next_frame += 20000;
			uint32_t future = static_cast<uint32_t>(next_frame/1000);
			now = SDL_GetTicks();
			
			if (now > future) {
				next_frame = static_cast<uint64_t>(now) * 1000;
			}
			else {
				SDL_Delay(future - now);
			}

			while (SDL_PollEvent(&event)) {
				/* We are only worried about SDL_KEYDOWN and SDL_KEYUP events */
				switch (event.type) {
				case SDL_KEYDOWN:
				case SDL_KEYUP:
				{
					std::cout << "sym: " << event.key.keysym.sym << ", shifted: " << event.key.keysym.mod  << std::endl;
					auto sym = event.key.keysym.sym;
					auto trans = m_KeyTranslations.find(std::make_pair(sym, event.key.keysym.mod));
					if (trans != m_KeyTranslations.end()) {
						std::cout << "translated '" << trans->first.first << "' to '" << trans->second.first << "'" << std::endl;
						sym = trans->second.first;
					}
					
					auto key = m_KeyMap.find(sym);
					if (key != m_KeyMap.end()) {
						printf( "Key event detected: %d (%s)\n", event.key.keysym.sym, (event.type == SDL_KEYDOWN) ? "down" : "up");
						KeyPress(key->second, event.type == SDL_KEYDOWN);
					}
					break;
				}

				default:
					break;
				}
			}
		}
	}
}

bool Machine::PaintRaster(Oric* a_Oric)
{
	if (m_RasterCurrent >= raster_visible_first && m_RasterCurrent < raster_visible_last) {
		m_Frontend->UpdateGraphics(m_RasterCurrent - raster_visible_first, m_Memory.m_Mem);
	}
	
	if (++m_RasterCurrent == raster_max) {
		m_RasterCurrent = 0;
		m_Frontend->RenderGraphics();
		return true;
	}

	return false;
}

void Machine::KeyPress(uint8_t a_KeyBits, bool a_Down)
{
	std::cout << "key: " << (int)a_KeyBits << ", " << (a_Down ? "down" : "up") << std::endl;
	if (a_Down) {
		m_KeyRows[a_KeyBits >> 3] |= (1 << (a_KeyBits & 0x07));
	}
	else {
		m_KeyRows[a_KeyBits >> 3] &= ~(1 << (a_KeyBits & 0x07));
	}
	
	if (m_CurrentKeyRow == (a_KeyBits >> 3)) {
		UpdateKeyOutput();
	}
}

void Machine::UpdateKeyOutput()
{
	m_CurrentKeyRow = m_Mos_6522->ReadORB() & 0x07;

	if (m_KeyRows[m_CurrentKeyRow] & (m_Ay3->GetRegister(AY3_8912::IO_PORT_A) ^ 0xff)) {
		m_Mos_6522->SetIRBBit(3, true);
	}
	else {
		m_Mos_6522->SetIRBBit(3, false);
	}
}



// --- Memory functions

uint8_t inline Machine::read_byte(Machine& a_Machine, uint16_t a_Address)
{
	if (a_Address >= 0x300 && a_Address < 0x400) {
// 		std::cout << "read: " << std::hex << a_Address << std::endl;
		return a_Machine.GetVIA().ReadByte(a_Address);
	}

	return a_Machine.GetMemory().m_Mem[a_Address];
}

uint8_t inline Machine::read_byte_zp(Machine &a_Machine, uint8_t a_Address)
{
	return a_Machine.GetMemory().m_Mem[a_Address];
}

uint16_t inline Machine::read_word(Machine &a_Machine, uint16_t a_Address)
{
	if (a_Address >= 0x300 && a_Address < 0x400) {
		std::cout << "read word: " << std::hex << a_Address << std::endl;
	}

	return a_Machine.GetMemory().m_Mem[a_Address] | a_Machine.GetMemory().m_Mem[a_Address + 1] << 8;
}

uint16_t inline Machine::read_word_zp(Machine &a_Machine, uint8_t a_Address)
{
	return a_Machine.GetMemory().m_Mem[a_Address] | a_Machine.GetMemory().m_Mem[a_Address+1 & 0xff] << 8;
}

void inline Machine::write_byte(Machine &a_Machine, uint16_t a_Address, uint8_t a_Val)
{
	if (a_Address >= 0xc000) {
		return;
	}
	
	if (a_Address >= 0x300 && a_Address < 0x400) {
		a_Machine.GetVIA().WriteByte(a_Address, a_Val);
	}

	a_Machine.GetMemory().m_Mem[a_Address] = a_Val;
}

void inline Machine::write_byte_zp(Machine &a_Machine, uint8_t a_Address, uint8_t a_Val)
{
	if (a_Address >= 0x00ff) {
		return;
	}

	a_Machine.GetMemory().m_Mem[a_Address] = a_Val;
}


uint8_t inline Machine::read_via_ora(Machine& a_Machine)
{
	return a_Machine.GetVIA().ReadORA();
}

uint8_t inline Machine::read_via_orb(Machine& a_Machine)
{
	return a_Machine.GetVIA().ReadORB();
}
