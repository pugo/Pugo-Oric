// Copyright (C) 2009-2016 by Anders Piniesjö <pugo@pugo.org>

#include <unistd.h>
#include <signal.h>

#include <sstream>
#include <cstdlib>
#include <vector>
#include <string>
#include <boost/algorithm/string.hpp>

#include "oric.h"
#include <SDL_image.h>

Oric::Oric() : 
	m_LastCommand(""),
	m_SdlWindow(NULL),
	m_SdlSurface(NULL),
	m_SdlRenderer(NULL)
{
}

void Oric::Init()
{
	InitGraphics();

	m_Machine = std::make_shared<Machine>();
	m_Machine->Init();
}

Oric::~Oric()
{}

void Oric::InitGraphics()
{
	//Initialization flag
	bool success = true;

	//Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
		success = false;
	}
	else {
		//Set texture filtering to linear
		if(!SDL_SetHint( SDL_HINT_RENDER_SCALE_QUALITY, "1" ))
		{
			printf( "Warning: Linear texture filtering not enabled!" );
		}

		//Create window
		m_SdlWindow = SDL_CreateWindow("SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 240, 224, SDL_WINDOW_SHOWN);
		if (m_SdlWindow == NULL) {
			printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
			success = false;
		}
		else {

			//Create renderer for window
			m_SdlRenderer = SDL_CreateRenderer(m_SdlWindow, -1, SDL_RENDERER_ACCELERATED);
			if(m_SdlRenderer == NULL) {
				printf( "Renderer could not be created! SDL Error: %s\n", SDL_GetError() );
				success = false;
			}
			else {
				//Initialize renderer color
				SDL_SetRenderDrawColor(m_SdlRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
				SDL_RenderClear(m_SdlRenderer);
			}
		}
	}
}

void Oric::UpdateGraphics(uint8_t a_RasterLine, uint8_t* a_Mem)
{
	uint8_t* char_mem = a_Mem + 0xb400;
	
	uint8_t dx = 0;
	uint32_t bg_col = m_Colors[0];
	uint32_t fg_col = m_Colors[7];

	for (uint16_t x = 0; x < 40; x++) {
		uint8_t ch = a_Mem[0xbb80 + (a_RasterLine >> 3)*40 + x];
		uint8_t chr_dat = char_mem[((ch & 0x7f) << 3) + (a_RasterLine & 0x07)];

		if (!(ch & 0x60)) {
			chr_dat = 0;
			switch(ch & 0x18)
			{
				case 0x00: fg_col = m_Colors[ch & 7]; break;
// 					case 0x08: lattr = ch & 7; break;
				case 0x10: bg_col = m_Colors[ch & 7]; break;
// 					case 0x18: pattr = ch & 7; break;
			}
		}
		
		if (ch & 0x80) {
			// Inverse colors
			fg_col = fg_col ^ 0xffffff;
			bg_col = bg_col ^ 0xffffff;
		}

		for (uint8_t i = 0x20; i > 0; i >>= 1, dx++) {
			if (chr_dat & i) {
				SDL_SetRenderDrawColor(m_SdlRenderer, fg_col >> 16, (fg_col & 0x0000ff00) >> 8, fg_col & 0x000000ff, 0xff); 
			}
			else {
				SDL_SetRenderDrawColor(m_SdlRenderer, bg_col >> 16, (bg_col & 0x0000ff00) >> 8, bg_col & 0x000000ff, 0xff); 
			}

			SDL_RenderDrawPoint(m_SdlRenderer, dx, a_RasterLine);
		}
	}
}

void Oric::RenderGraphics()
{
	SDL_RenderPresent(m_SdlRenderer);
}

void Oric::CloseGraphics()
{
	//Destroy window
	SDL_DestroyWindow(m_SdlWindow);
	m_SdlWindow = NULL;

	//Quit SDL subsystems
	SDL_Quit();
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
		std::cout << "quiet          : prevent debug output at run time" << std::endl;
		std::cout << "" << std::endl;
		return true;
	}
	else if (cmd == "g") { // go <address>
		long steps = 0; // 0 = infinite
		if (parts.size() == 2) {
			steps = std::stol(parts[1]);
		}
		m_Machine->Run(steps, this);
	}
	else if (cmd == "pc") { // set pc
		if (parts.size() < 2) {
			std::cout << "Error: missing address" << std::endl;
			return true;
		}
		uint16_t addr = StringToWord(parts[1]);
		m_Machine->GetCPU()->SetPC(addr);
		m_Machine->GetCPU()->PrintStat();
	}
	else if (cmd == "s") { // step
		if (parts.size() == 2) {
			m_Machine->Run(std::stol(parts[1]), this);
		}
		else {
			bool brk = false;
			m_Machine->GetCPU()->ExecInstruction(brk);
			if (brk) {
				std::cout << "Instruction BRK executed." << std::endl;
			}
		}
	}
	else if (cmd == "i") { // info
		std::cout << "PC: " << m_Machine->GetCPU()->GetPC() << std::endl;
		m_Machine->GetCPU()->PrintStat();
	}
	else if (cmd == "m") { // info
		if (parts.size() < 3) {
			std::cout << "Use: m <start address> <length>" << std::endl;
			return true;
		}
		m_Machine->GetMemory()->Show(StringToWord(parts[1]), StringToWord(parts[2]));
	}
	else if (cmd == "quiet") {
		m_Machine->GetCPU()->SetQuiet(true);
		std::cout << "Quiet mode enabled" << std::endl;
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

static void signal_handler(int a_Sig)
{
	std::cout << "Signal: " << a_Sig << std::endl;
	if (a_Sig == SIGINT) {
		Oric::GetInstance()->GetMachine()->Stop();
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

	std::shared_ptr<Oric> oric = Oric::GetInstance();
	oric->Init();
	oric->GetMachine()->GetMemory()->Load("ROMS/basic11b.rom", 0xc000);
	oric->GetMachine()->GetMemory()->Load("ROMS/font.rom", 0xb400);
	oric->GetMachine()->Reset();

	std::cout << std::endl;
	oric->Monitor();

	return 0;
}
