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
#include <signal.h>

#include <sstream>
#include <cstdlib>
#include <vector>
#include <string>
#include <boost/algorithm/string.hpp>

#include "oric.h"


Oric::Oric() : 
	m_LastCommand("")
{
	m_Machine = new Machine();
}

Oric::~Oric()
{
	delete m_Machine;
}

void Oric::Monitor()
{
	std::string cmd;

	while (true) {
		std::cout << ">> " << std::flush;
		std::cin.clear();
		getline(std::cin, cmd);

		if (! HandleCommand(cmd)) {
			break;
		}
	}
}

uint16_t Oric::StringToWord(std::string& a_Addr)
{
	uint16_t x;
	std::stringstream ss;
	ss << std::hex << a_Addr;
	ss >> x;
	return x;
}

bool Oric::HandleCommand(std::string& a_Line)
{
	if (a_Line.length() == 0) {
		if (m_LastCommand.length() == 0) {
			return true;
		}
		a_Line = m_LastCommand;
	}
	else {
		m_LastCommand = a_Line;
	}

	std::vector<std::string> parts;
	boost::split(parts, a_Line, boost::is_any_of("\t "));
	std::string cmd = parts[0];

	if (cmd == "h") {
		std::cout << "Available monitor commands:" << std::endl << std::endl;
		std::cout << "h              : help (showing this text)" << std::endl;
		std::cout << "g <address>    : go to address and run" << std::endl;
		std::cout << "pc <address>   : set program counter to address" << std::endl;
		std::cout << "s [n]          : step one or possible n steps" << std::endl;
		std::cout << "i              : print machine info" << std::endl;
		std::cout << "m <address> <n>: dump memory from address and n bytes ahead" << std::endl;
		std::cout << "" << std::endl;
		return true;
	}
	if (cmd == "g") { // go <address>
		long steps = 0;
		if (parts.size() == 2) {
			steps = std::stol(parts[1]);
		}
		m_Machine->Run(steps);
	}
	else if (cmd == "pc") { // set pc
		if (parts.size() < 2) {
			std::cout << "Error: missing address" << std::endl;
			return true;
		}
		uint16_t addr = StringToWord(parts[1]);
		m_Machine->GetCPU().SetPC(addr);
		m_Machine->GetCPU().PrintStat();
	}
	else if (cmd == "s") { // step
		if (parts.size() == 2) {
			m_Machine->Run(std::stol(parts[1]));
		}
		else {
			bool brk = false;
			m_Machine->GetCPU().ExecInstruction(brk);
			if (brk) {
				std::cout << "Instruction BRK executed." << std::endl;
			}
		}
	}
	else if (cmd == "i") { // info
		std::cout << "PC: " << m_Machine->GetCPU().GetPC() << std::endl;
		m_Machine->GetCPU().PrintStat();
	}
	else if (cmd == "m") { // info
		if (parts.size() < 3) {
			std::cout << "Use: m <start address> <length>" << std::endl;
			return true;
		}
		m_Machine->GetMemory().Show(StringToWord(parts[1]), StringToWord(parts[2]));
	}
	else if (cmd == "quiet") {
		m_Machine->GetCPU().SetQuiet(true);
	}

	else if (cmd == "q") { // quit
		std::cout << "quit" << std::endl;
		return false;
	}

	return true;
}

static void signal_handler(int);
void init_signals(void);

struct sigaction sigact;
Oric g_Oric;

static void signal_handler(int a_Sig)
{
	std::cout << "Signal: " << a_Sig << std::endl;
	if (a_Sig == SIGINT)
		g_Oric.GetMachine().Stop();
}

void init_signals()
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
	std::cout << "pwd: " << pwd << std::endl;

	init_signals();
	
	//g_Oric.GetMemory()->Load("basic10.rom", 0xc000);
	//g_Oric.GetMemory()->Load("ROMS/dayofweek.rom", 0xc000);
	//g_Oric.GetMemory()->Load("ROMS/AllSuiteA.rom", 0x4000);

	g_Oric.GetMachine().GetMemory().Load("ROMS/basic11b.rom", 0xc000);
	g_Oric.GetMachine().GetMemory().Load("ROMS/font.rom", 0xb400);

	g_Oric.GetMachine().Reset();
	std::cout << std::endl;
	g_Oric.Monitor();

	return 0;
}
