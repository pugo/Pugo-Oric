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
	enum VideoAttribs
	{
		HZ_50 = 0x02,
		HIRES = 0x04
	};

	enum TextAttribs
	{
		ALTERNATE_CHARSET = 0x01,
		DOUBLE_SIZE = 0x02,
		BLINKING = 0x04,
	};

	Frontend(const Oric* a_Oric);
	~Frontend();

	void InitGraphics();
	void CloseGraphics();
	void UpdateGraphics(uint8_t a_RasterLine, uint8_t* a_Mem);
	void RenderGraphics();

protected:
	uint32_t m_Colors[8] {0xff000000, 0xffff0000, 0xff00ff00, 0xffffff00, 0xff0000ff, 0xffff00ff, 0xff00ffff, 0xffffffff};
	const uint8_t m_TextureWidth = 240;
	const uint8_t m_TextureHeight = 224;
	const uint8_t m_TextureBpp = 4;

	const Oric* m_Oric;

	SDL_Window* m_SdlWindow;
	SDL_Surface* m_SdlSurface;
	SDL_Renderer* m_SdlRenderer;
	SDL_Texture* m_SdlTexture;

	std::vector<uint8_t> m_Pixels;
	uint8_t m_VideoAttrib;
	uint8_t m_TextAttrib;
};



#endif // FRONTEND_H
