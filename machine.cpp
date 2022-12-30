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


static int32_t keytab[] = {
	'7'        , 'n'        , '5'        , 'v'        , 0 ,          '1'        , 'x'        , '3'        ,     // 7
	'j'        , 't'        , 'r'        , 'f'        , 0          , SDLK_ESCAPE, 'q'        , 'd'        ,     // 15
	'm'        , '6'        , 'b'        , '4'        , SDLK_LCTRL , 'z'        , '2'        , 'c'        ,     // 23
	'k'        , '9'        , ';'        , '-'        , 0          , 0          , '\\'       , '\''       ,     // 31
	SDLK_SPACE , ','        , '.'        , SDLK_UP    , SDLK_LSHIFT, SDLK_LEFT  , SDLK_DOWN  , SDLK_RIGHT ,     //
	'u'        , 'i'        , 'o'        , 'p'        , SDLK_LALT  , SDLK_BACKSPACE, ']'     , '['        ,
	'y'        , 'h'        , 'g'        , 'e'        , 0          , 'a'        , 's'        , 'w'        ,
	'8'        , 'l'        , '0'        , '/'        , SDLK_RSHIFT, SDLK_RETURN, 0          , SDLK_EQUALS };

	
Machine::Machine(const Oric* oric) :
	oric(oric),
	memory(65535),
	tape(nullptr),
	is_running(false),
	warpmode_on(false),
	break_exec(false),
	raster_current(0),
	current_key_row(0)
{
	for (uint8_t i=0; i < 8; i++) {
		key_rows[i] = 0;
	}

	boost::assign::insert(key_translations)
		(std::make_pair(0xe4, false), std::make_pair('/', false))
		(std::make_pair(0xe4, true), std::make_pair('/', false))
		(std::make_pair(0xf6, false), std::make_pair(';', false))
		(std::make_pair(0xf6, true), std::make_pair(';', false))
		(std::make_pair(0x2b, false), std::make_pair('=', false))
		(std::make_pair(0x2b, true), std::make_pair('=', false));
}

Machine::~Machine()
{}

void Machine::init(Frontend* frontend)
{
	this->frontend = frontend;
	cpu = new MOS6502(*this);
	mos_6522 = new MOS6522(*this);
	ay3 = new AY3_8912(*this);

//    tape = new TapeTap(*mos_6522, "taps/Xenon1.tap");
//    tape = new TapeTap(*mos_6522, "taps/WIMPY.TAP");
    tape = new TapeTap(*mos_6522, "taps/HUNCHBACK");
//   tape = new TapeTap(*mos_6522, "taps/Oricium12.tap");
//    tape = new TapeTap(*mos_6522, "taps/SCUBA");
    if (!tape->init()) {
        exit(1);
    }

	cpu->memory_read_byte_handler = read_byte;
	cpu->memory_read_byte_zp_handler = read_byte_zp;
	cpu->memory_read_word_handler = read_word;
	cpu->memory_read_word_zp_handler = read_word_zp;
	cpu->memory_write_byte_handler = write_byte;
	cpu->memory_write_byte_zp_handler = write_byte_zp;

	ay3->m_read_data_handler = read_via_ora;

	mos_6522->ca2_changed_handler = AY3_8912::set_bc1;
	mos_6522->cb2_changed_handler = AY3_8912::set_bdir;
	
	for (uint8_t i=0; i < 64; ++i) {
      if (keytab[i] != 0) {
			key_map[keytab[i]] = i;
      }
	}
}

void Machine::reset()
{
	cpu->Reset();
}

/**
 * run machine.
 * \param steps number of instructions to run. If 0: run infinite (or to BRK).
 */
void Machine::run(uint32_t steps, Oric* oric)
{
	uint32_t instructions = 0;

	uint32_t now = SDL_GetTicks();
	uint64_t next_frame = static_cast<uint64_t>(now) * 1000;
	SDL_Event event;

	break_exec = false;
	while (! break_exec) {
		int32_t cycles = cycles_per_raster;

		while (cycles > 0) {
			const uint8_t ran = cpu->exec_instruction(break_exec);
			cycles -= ran;

			tape->exec(ran);
			mos_6522->exec(ran);
			ay3->exec(ran);

			update_key_output();

			if (steps > 0 && ++instructions == steps) {
				return;
			}
		}

		while (SDL_PollEvent(&event)) {
			// We are only worried about SDL_KEYDOWN and SDL_KEYUP events.
			switch (event.type)
			{
				case SDL_KEYDOWN:
				case SDL_KEYUP:
				{
//					std::cout << "sym: " << event.key.keysym.sym << ", shifted: " << event.key.keysym.mod  << std::endl;
					auto sym = event.key.keysym.sym;

					if (event.key.keysym.sym == SDLK_F12 && event.type == SDL_KEYDOWN) {
						warpmode_on = !warpmode_on;
						std::cout << "Warp mode: " << (warpmode_on ? "on" : "off") << std::endl;
						if (! warpmode_on) {
							now = SDL_GetTicks();
							next_frame = static_cast<uint64_t>(now) * 1000;
						}
					}

				 	auto trans = key_translations.find(std::make_pair(sym, event.key.keysym.mod));
					if (trans != key_translations.end()) {
//						std::cout << "translated '" << trans->first.first << "' to '" << trans->second.first << "'" << std::endl;
						sym = trans->second.first;
					}

					auto key = key_map.find(sym);
					if (key != key_map.end()) {
//						printf( "Key event detected: %d (%s): %d\n", event.key.keysym.sym, ((event.type == SDL_KEYDOWN) ? "down" : "up"), key->second);
						key_press(key->second, event.type == SDL_KEYDOWN);
					}
					break;
				}
	 			default:
			 		break;
			}
		}

		if (paint_raster(oric)) {
			next_frame += 20000;
			const uint32_t future = static_cast<uint32_t>(next_frame/1000);
			now = SDL_GetTicks();
			
			if (now > future) {
				next_frame = static_cast<uint64_t>(now) * 1000;
			}
			else {
		 		if (! warpmode_on) {
					SDL_Delay(future - now);
			 	}
			}
		}
	}
}

bool Machine::paint_raster(Oric* oric)
{
	if (raster_current >= raster_visible_first && raster_current < raster_visible_last) {
		frontend->update_graphics(raster_current - raster_visible_first, memory.mem);
	}
	
	if (++raster_current == raster_max) {
		raster_current = 0;
		frontend->render_graphics();
		return true;
	}

	return false;
}

void Machine::key_press(uint8_t a_KeyBits, bool a_Down)
{
	std::cout << "key: " << (int)a_KeyBits << ", " << (a_Down ? "down" : "up") << std::endl;
	if (a_Down) {
		key_rows[a_KeyBits >> 3] |= (1 << (a_KeyBits & 0x07));
	}
	else {
		key_rows[a_KeyBits >> 3] &= ~(1 << (a_KeyBits & 0x07));
	}
	
	if (current_key_row == (a_KeyBits >> 3)) {
		update_key_output();
	}
}

void Machine::update_key_output()
{
	current_key_row = mos_6522->read_orb() & 0x07;

	if (key_rows[current_key_row] & (ay3->get_register(AY3_8912::IO_PORT_A) ^ 0xff)) {
		mos_6522->set_irb_bit(3, true);
	}
	else {
		mos_6522->set_irb_bit(3, false);
	}
}


void Machine::via_orb_changed(uint8_t a_Orb)
{
	tape->set_motor(a_Orb & 0x40);
}


// --- Memory functions -------------------

uint8_t inline Machine::read_byte(Machine& a_Machine, uint16_t a_Address)
{
	if (a_Address >= 0x300 && a_Address < 0x400) {
		return a_Machine.get_via().read_byte(a_Address);
	}
	return a_Machine.get_memory().mem[a_Address];
}

uint8_t inline Machine::read_byte_zp(Machine &a_Machine, uint8_t a_Address)
{
	return a_Machine.get_memory().mem[a_Address];
}

uint16_t inline Machine::read_word(Machine &a_Machine, uint16_t a_Address)
{
// 	if (a_Address >= 0x300 && a_Address < 0x400) {
// 		std::cout << "read word: " << std::hex << a_Address << std::endl;
// 	}
	return a_Machine.get_memory().mem[a_Address] | a_Machine.get_memory().mem[a_Address + 1] << 8;
}

uint16_t inline Machine::read_word_zp(Machine &a_Machine, uint8_t a_Address)
{
	return a_Machine.get_memory().mem[a_Address] | a_Machine.get_memory().mem[a_Address + 1 & 0xff] << 8;
}

void inline Machine::write_byte(Machine &a_Machine, uint16_t a_Address, uint8_t a_Val)
{
	if (a_Address >= 0xc000) {
		return;
	}

	if (a_Address >= 0x300 && a_Address < 0x400) {
		a_Machine.get_via().write_byte(a_Address, a_Val);
	}

	a_Machine.get_memory().mem[a_Address] = a_Val;
}

void inline Machine::write_byte_zp(Machine &a_Machine, uint8_t a_Address, uint8_t a_Val)
{
	if (a_Address >= 0x00ff) {
		return;
	}
	a_Machine.get_memory().mem[a_Address] = a_Val;
}


uint8_t inline Machine::read_via_ora(Machine& a_Machine)
{
	return a_Machine.get_via().read_ora();
}

uint8_t inline Machine::read_via_orb(Machine& a_Machine)
{
	return a_Machine.get_via().read_orb();
}
