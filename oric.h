/***************************************************************************
*   Copyright (C) 2009 by Anders Karlsson   *
*   pugo@pugo.org   *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
*                                                                         *
*   This program is distributed in the hope that it will be useful,       *
*   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
*   GNU General Public License for more details.                          *
*                                                                         *
*   You should have received a copy of the GNU General Public License     *
*   along with this program; if not, write to the                         *
*   Free Software Foundation, Inc.,                                       *
*   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
***************************************************************************/

#ifndef ORIC_H
#define ORIC_H

#include <iostream>

#include "datatypes.h"
#include "mos6502.h"
#include "memory.h"

class Oric
{
public:
	Oric();
	~Oric();

	Memory* getMemory() { return memory; }
	MOS6502* getCPU() { return cpu; }

	void reset();
	void run(long steps);
	void run(word address, long steps) { cpu->setPC(address); run(steps); }
	void stop() { brk = true; }

	void monitor();

	static byte memoryReadHandler(word address);
	static void memoryWriteHandler(word address, byte data);

	bool brk;

protected:
	bool handleCommand(std::string cmd);
	word stringToWord(std::string addr);

	Memory* memory;
	MOS6502* cpu;

	bool running;
	std::string last_command;
};

#endif // ORIC_H
