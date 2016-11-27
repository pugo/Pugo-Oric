// Copyright (C) 2009-2016 by Anders Piniesjö <pugo@pugo.org>

#ifndef FRONTEND_H
#define FRONTEND_H

#include <memory>
#include <iostream>
#include <ostream>
#include <vector>
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

	const uint8_t m_TextureWidth = 240;
	const uint8_t m_TextureHeight = 224;
	const uint8_t m_TextureBpp = 4;
	
	SDL_Window* m_SdlWindow;
	SDL_Surface* m_SdlSurface;
	SDL_Renderer* m_SdlRenderer;
	SDL_Texture* m_SdlTexture;

	std::vector<uint8_t> m_Pixels;
	
	uint32_t m_Colors[8] {0xff000000, 0xffff0000, 0xff00ff00, 0xffffff00, 0xff0000ff, 0xffff00ff, 0xff00ffff, 0xffffffff};
};



#endif // FRONTEND_H
