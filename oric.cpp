// Copyright (C) 2009-2016 by Anders Piniesjö <pugo@pugo.org>

#include <unistd.h>
#include <signal.h>

#include <sstream>
#include <cstdlib>
#include <vector>
#include <string>
#include <boost/algorithm/string.hpp>

#include "oric.hpp"
#include "memory.hpp"
#include "frontend.hpp"


Oric::Oric() : 
	m_State(STATE_RUN),
	m_LastCommand("")
{
}

void Oric::Init()
{
	m_Machine = new Machine(this);
	m_Frontend = new Frontend(this);
	m_Machine->Init(m_Frontend);
	m_Frontend->InitGraphics();

	m_Machine->GetCPU().SetQuiet(true);
}

Oric::~Oric()
{}


void Oric::Run()
{
	while (true) {
		switch (m_State) {
			case STATE_RUN:
				m_Machine->Run(0, this);
				break;
			case STATE_MON:
			{
				std::string cmd;
				std::cout << ">> " << std::flush;
				getline(std::cin, cmd);

				m_State = HandleCommand(cmd);
				break;
			}
			case STATE_QUIT:
				return;
		}
	}
}

void Oric::Break()
{
	m_State = STATE_MON;
}

uint16_t Oric::StringToWord(std::string& a_Addr)
{
	uint16_t x;
	std::stringstream ss;
	ss << std::hex << a_Addr;
	ss >> x;
	return x;
}

Oric::State Oric::HandleCommand(std::string& a_Line)
{
	if (a_Line.length() == 0) {
		if (m_LastCommand.length() == 0) {
			return STATE_MON;
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
		std::cout << "quiet          : prevent debug output at run time" << std::endl;
		std::cout << "debug          : show debug output at run time" << std::endl;
		std::cout << "" << std::endl;
		return STATE_MON;
	}
	else if (cmd == "g") { // go <address>
		return STATE_RUN;
	}
	else if (cmd == "pc") { // set pc
		if (parts.size() < 2) {
			std::cout << "Error: missing address" << std::endl;
			return STATE_MON;
		}
		uint16_t addr = StringToWord(parts[1]);
		m_Machine->GetCPU().SetPC(addr);
		m_Machine->GetCPU().PrintStat();
	}
	else if (cmd == "s") { // step
		if (parts.size() == 2) {
			m_Machine->Run(std::stol(parts[1]), this);
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
	else if (cmd == "v") { // info
		m_Machine->GetVIA().PrintStat();
	}
	else if (cmd == "m") { // info
		if (parts.size() < 3) {
			std::cout << "Use: m <start address> <length>" << std::endl;
			return STATE_MON;
		}
		m_Machine->GetMemory().Show(StringToWord(parts[1]), StringToWord(parts[2]));
	}
	else if (cmd == "quiet") {
		m_Machine->GetCPU().SetQuiet(true);
		std::cout << "Quiet mode enabled" << std::endl;
	}
	else if (cmd == "debug") {
		m_Machine->GetCPU().SetQuiet(false);
		std::cout << "Debug mode enabled" << std::endl;
	}

	else if (cmd == "q") { // quit
		std::cout << "quit" << std::endl;
		return STATE_QUIT;
	}

	return STATE_MON;
}

static void signal_handler(int);
void init_signals(void);

struct sigaction sigact;

static void signal_handler(int a_Sig)
{
	std::cout << "Signal: " << a_Sig << std::endl;
	if (a_Sig == SIGINT) {
		Oric::GetInstance().GetMachine().Stop();
		Oric::GetInstance().Break();
	}
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

	Oric& oric = Oric::GetInstance();
	oric.Init();
	oric.GetMachine().GetMemory().Load("ROMS/basic11b.rom", 0xc000);
	oric.GetMachine().GetMemory().Load("ROMS/font.rom", 0xb400);
	oric.GetMachine().Reset();

	std::cout << std::endl;
	oric.Run();

	return 0;
}
