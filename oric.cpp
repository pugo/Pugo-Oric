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


#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <unistd.h>
#include <signal.h>
// #include <stdio.h>
// #include <stdlib.h>
// #include <stdarg.h>


#include <iostream>
#include <sstream>
#include <cstdlib>
#include <vector>
#include <string>
#include <boost/algorithm/string.hpp>

#include "oric.h"

using namespace std;


Oric::Oric() : 
	running(false),
	last_command("")
{
	memory = new Memory(65536);
	cpu = new MOS6502(memory);
	cpu->memory_read_handler = memoryReadHandler;
	cpu->memory_write_handler = memoryWriteHandler;
}


Oric::~Oric()
{
	delete cpu;
	delete memory;
}


void Oric::run(long steps)
{
	running = true;

	if (steps > 0)
	{
		for (long i = 0; i < steps; i++)
		  {
			cpu->execInstruction();
		  }
	}
	else
	{
		while(running)
		  {
			running = cpu->execInstruction();
		  }
	}
}


void Oric::monitor()
{
	string cmd;

	while (true)
	{
		cout << ">> " << flush;
		cin.clear();
		getline(cin, cmd);

		if (! handleCommand(cmd))
			break;
	}
}


word Oric::stringToWord(string addr)
{
	word x;
	stringstream ss;
	ss << std::hex << addr;
	ss >> x;
	return x;
}



bool Oric::handleCommand(string line)
{
	if (line.length() == 0)
	{
		if (last_command.length() == 0)
			return true;

		line = last_command;
	}
	else
		last_command = line;

	std::vector<std::string> parts;
	boost::split(parts, line, boost::is_any_of("\t "));
	string cmd = parts[0];

	if (cmd == "g")			// go <address>
	{
		long steps = 0;
		if (parts.size() < 2)
		{
			cout << "Error: missing address" << endl;
			return true;
		}

		if (parts.size() == 3)
			steps = std::stol(parts[2]);

		word addr = stringToWord(parts[1]);
		run(addr, steps);
	}

	else if (cmd == "pc")	// step
	{
		if (parts.size() < 2)
		{
			cout << "Error: missing address" << endl;
			return true;
		}
		word addr = stringToWord(parts[1]);
		cpu->setPC(addr);
	}

	else if (cmd == "s")	// step
	{
		cpu->execInstruction();
	}

	else if (cmd == "i")	// info
	{
		cpu->printStat();
	}

	else if (cmd == "m")	// info
	{
		if (parts.size() < 3)
		{
			cout << "Use: m <start address> <length>" << endl;
			return true;
		}
		memory->show(stringToWord(parts[1]), stringToWord(parts[2]));
	}

	else if (cmd == "q")	// quit
	{
		cout << "quit" << endl;
		return false;
	}

	return true;
}


byte Oric::memoryReadHandler(word address)
{
	cout << "memoryReadHandler( " << hex << address << " )" << endl;
	return 0;
}

void Oric::memoryWriteHandler(word address, byte data)
{
}


// -------------------------------------------------------

static void signal_handler(int);
void init_signals(void);

struct sigaction sigact;

Oric oric;


static void signal_handler(int sig)
{
	cout << "Signal: " << sig << endl;
// 	if (sig == SIGINT)
// 	{
// 		oric.stop();
// 	}
}


void init_signals(void)
{
	sigact.sa_handler = signal_handler;
	sigemptyset(&sigact.sa_mask);
	sigact.sa_flags = 0;
	sigaction(SIGINT, &sigact, (struct sigaction *)NULL);
}


int main(int argc, char *argv[])
{
	char pwd[1024];
	getcwd(pwd, 1024);
	cout << "pwd: " << pwd << endl;

	init_signals();
	
	oric.getMemory()->load("/home/pugo/projekt/oric/ROMS/basic10.rom", 0xc000);
	oric.getMemory()->load("/home/pugo/projekt/oric/ROMS/font.rom", 0xb400);
	// 	oric.getMemory()->load("/home/pugo/projekt/oric/ROMS/dayofweek.rom", 0xc000);
// 	oric.getCPU()->C = true;

	oric.monitor();
	oric.run(0x200, 0);

	return 0;
}
