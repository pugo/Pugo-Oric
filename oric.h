// Copyright (C) 2009-2016 by Anders Piniesjö <pugo@pugo.org>

#ifndef ORIC_H
#define ORIC_H

#include <iostream>
#include <memory>
#include <SDL.h>

#include "machine.h"

class Oric
{
public:
	static std::shared_ptr<Oric> GetInstance() {
		static std::shared_ptr<Oric> instance = std::make_shared<Oric>();
		return instance;
	}
	
	Oric();
	~Oric();

	void Init();
	void InitGraphics();
	void CloseGraphics();
	void UpdateGraphics(uint8_t a_RasterLine, uint8_t* a_Mem);
	void RenderGraphics();

	std::shared_ptr<Machine> GetMachine() { return m_Machine; }
	void Monitor();

protected:
	bool HandleCommand(std::string& a_Cmd);
	uint16_t StringToWord(std::string& a_Addr);

	std::shared_ptr<Machine> m_Machine;
	std::string m_LastCommand;
	
	SDL_Window* m_SdlWindow;
	SDL_Surface* m_SdlSurface;
	SDL_Renderer* m_SdlRenderer;
	uint32_t m_Colors[8] {0x00000000, 0x00ff0000, 0x0000ff00, 0x00ffff00, 0x000000ff, 0x00ff00ff, 0x0000ffff, 0x00ffffff};
};

#endif // ORIC_H
