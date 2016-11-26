// Copyright (C) 2009-2016 by Anders Piniesjö <pugo@pugo.org>

#ifndef FRONTEND_H
#define FRONTEND_H

#include <memory>
#include <iostream>
#include <ostream>
#include <SDL.h>

class Oric;
class Memory;

class Frontend
{
public:
	Frontend(std::shared_ptr<Oric> a_Oric);
	~Frontend();

	void InitGraphics();
	void CloseGraphics();
	void UpdateGraphics(uint8_t a_RasterLine, uint8_t* a_Mem);
	void RenderGraphics();

protected:
	std::shared_ptr<Oric> m_Oric;
	std::shared_ptr<Memory> m_Memory;
	
	SDL_Window* m_SdlWindow;
	SDL_Surface* m_SdlSurface;
	SDL_Renderer* m_SdlRenderer;
	uint32_t m_Colors[8] {0x00000000, 0x00ff0000, 0x0000ff00, 0x00ffff00, 0x000000ff, 0x00ff00ff, 0x0000ffff, 0x00ffffff};
};



#endif // FRONTEND_H
