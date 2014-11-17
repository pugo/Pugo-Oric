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


#include <unistd.h>

#include <iostream>
#include <sstream>
#include <cstdlib>
#include <vector>
#include <string>

#include "machine.h"


using namespace std;


Machine::Machine() : 
	running(false),
	brk(false)
{
	memory = new Memory(65536);
	cpu = new MOS6502(this, memory);
	mos_6522 = new MOS6522(this, memory);
	
	cpu->memory_read_byte_handler = read_byte;
	cpu->memory_read_byte_zp_handler = read_byte_zp;
	cpu->memory_read_word_handler = read_word;
	cpu->memory_read_word_zp_handler = read_word_zp;
	cpu->memory_write_byte_handler = write_byte;
	cpu->memory_write_byte_zp_handler = write_byte_zp;
}


Machine::~Machine()
{
	delete cpu;
	delete memory;
}


void Machine::reset()
{
	cpu->reset();
}


/**
 * Run machine.
 * \param steps number of steps to run. If 0: run infinite (or to BRK).
 */
void Machine::run(long steps)
{
	long cycles = 0;
	long steps_count = 0;
	brk = false;

	while(!brk)
	{
		cycles += cpu->execInstruction(brk);
		mos_6522->exec();

		++steps_count;
		if (steps > 0 && steps_count == steps)
			return;
	}
}


// --- Memory functions


byte inline Machine::read_byte(Machine &machine, word address)
{
	if (address >= 0x300 && address < 0x400)
	{
		cout << "read: " << hex << address << endl;
		return machine.getVIA().readByte(address);
	}

	return machine.getMemory().mem[address];
}

byte inline Machine::read_byte_zp(Machine &machine, byte address)
{
	return machine.getMemory().mem[address];
}

word inline Machine::read_word(Machine &machine, word address)
{
	if (address >= 0x300 && address < 0x400)
	{
		cout << "read word: " << hex << address << endl;
	}

	return machine.getMemory().mem[address] | machine.getMemory().mem[address+1]<<8;
}

word inline Machine::read_word_zp(Machine &machine, byte address)
{
	return machine.getMemory().mem[address] | machine.getMemory().mem[address+1 & 0xff]<<8;
}

void inline Machine::write_byte(Machine &machine, word address, byte val)
{
	if (address >= 0xc000)
		return;
	if (address >= 0x300 && address < 0x400)
	{
		machine.getVIA().writeByte(address, val);
	}

	machine.getMemory().mem[address] = val;
}

void inline Machine::write_byte_zp(Machine &machine, byte address, byte val)
{
	if (address >= 0x00ff)
		return;

	machine.getMemory().mem[address] = val;
}

