// Copyright (C) 2009-2016 by Anders Piniesjö <pugo@pugo.org>

#include "frontend.hpp"

#include <SDL_image.h>

Frontend::Frontend(const Oric* a_Oric) :
	m_Oric(a_Oric),
	m_SdlWindow(NULL),
	m_SdlSurface(NULL),
	m_SdlRenderer(NULL),
	m_VideoAttrib(0),
	m_TextAttrib(0)
{
	m_Pixels = std::vector<uint8_t>(m_TextureWidth * m_TextureHeight * m_TextureBpp, 0);
}

Frontend::~Frontend()
{
}

void Frontend::InitGraphics()
{
	//Initialization flag
	bool success = true;

	//Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		std::cout << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
		success = false;
	}
	else {
		//Set texture filtering to linear
		if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1")) {
			std::cout <<  "Warning: Linear texture filtering not enabled!" << std::endl;
		}

		//Create window (240x224)
		m_SdlWindow = SDL_CreateWindow("Pugo-Oric", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 600, 600, SDL_WINDOW_SHOWN);
		if (m_SdlWindow == NULL) {
			std::cout << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
			success = false;
		}
		else {
			//Create renderer for window
			m_SdlRenderer = SDL_CreateRenderer(m_SdlWindow, -1, SDL_RENDERER_ACCELERATED);
			if (m_SdlRenderer == NULL) {
				std::cout <<  "Renderer could not be created! SDL Error: " << SDL_GetError() << std::endl;
				success = false;
			}
			else {
				SDL_RendererInfo info;
				SDL_GetRendererInfo(m_SdlRenderer, &info);
				for (uint32_t i = 0; i < info.num_texture_formats; i++) {
					std::cout << SDL_GetPixelFormatName( info.texture_formats[i] ) << std::endl;
				}

				m_SdlTexture = SDL_CreateTexture(m_SdlRenderer, SDL_PIXELFORMAT_ARGB8888,
															SDL_TEXTUREACCESS_STREAMING, m_TextureWidth, m_TextureHeight);

				//Initialize renderer color
				SDL_SetRenderDrawColor(m_SdlRenderer, 0xff, 0xff, 0xff, 0xff);
				SDL_RenderClear(m_SdlRenderer);
			}
		}
	}
}

// Return memory address corresponding to a raster line, for current video mode.
inline uint16_t calcRowAddr(uint8_t a_RasterLine, uint8_t a_VideoAttrib)
{
	if (a_VideoAttrib & Frontend::VideoAttribs::HIRES && a_RasterLine < 200) {		
		return 0xa000 + a_RasterLine * 40;			// Hires: return address for hires data for line.
	}
	return 0xbb80 + (a_RasterLine >> 3) * 40;		// Text (lores or > 200): return address to char data for line (>>3).
}

void Frontend::UpdateGraphics(uint8_t a_RasterLine, uint8_t* a_Mem)
{
	uint8_t dx = 0;
	uint32_t bg_col = m_Colors[0];
	uint32_t fg_col = m_Colors[7];
	m_TextAttrib = 0;

	uint32_t* texture_line = (uint32_t*)&m_Pixels[a_RasterLine * m_TextureWidth * m_TextureBpp];
	uint16_t row = calcRowAddr(a_RasterLine, m_VideoAttrib);

	// 40 characters wide, regardless of lores or hires.
	for (uint16_t x = 0; x < 40; x++) {
		// get char code.
		uint8_t ch = a_Mem[row + x];
		bool ctrl_char = false;
		
		if (!(ch & 0x60)) {
			ctrl_char = true;
			switch(ch & 0x18)
			{
			case 0x00:
				// Ink color.
				fg_col = m_Colors[ch & 7];
				break;
			case 0x08:
				// Text attributes.
				m_TextAttrib = ch & 7;
				break;
			case 0x10:
				// Paper color.
				bg_col = m_Colors[ch & 7];
				break;
			case 0x18:
				// Video control attrs.
				m_VideoAttrib = ch & 0x07;
				row = calcRowAddr(a_RasterLine, m_VideoAttrib);
				break;
			}
		}

		uint32_t _fg_col = fg_col;
		uint32_t _bg_col = bg_col;

		// Inverse colors if upper bit is set in char code.
		if (ch & 0x80) {
			_fg_col = _fg_col ^ 0x00ffffff;
			_bg_col = _bg_col ^ 0x00ffffff;
		}

		uint8_t chr_dat = 0;
		if (!ctrl_char) {
			if ((m_VideoAttrib & VideoAttribs::HIRES) && a_RasterLine < 200) {
				chr_dat = ch;	// Hires, read byte directly.
			}
			else {
				// get char pixel data for read char code. If hires > 200, charmem is at 0x9800.
				uint8_t* char_mem = a_Mem + ((m_VideoAttrib & VideoAttribs::HIRES) ? 0x9800 : 0xb400) +
											((m_TextAttrib & TextAttribs::ALTERNATE_CHARSET) ? 128*8 : 0);
				chr_dat = char_mem[((ch & 0x7f) << 3) + (a_RasterLine & 0x07)] & 0x3f;
			}
		}

		// Copy 6 pixels.
		for (uint8_t i = 0x20; i > 0; i >>= 1, dx++) {
			*texture_line++ = (chr_dat & i) ? _fg_col : _bg_col;
		}
	}
}

void Frontend::RenderGraphics()
{
	SDL_UpdateTexture(m_SdlTexture, NULL, &m_Pixels[0], m_TextureWidth * m_TextureBpp);
	SDL_RenderCopy(m_SdlRenderer, m_SdlTexture, NULL, NULL );
	SDL_RenderPresent(m_SdlRenderer);
}

void Frontend::CloseGraphics()
{
	//Destroy window
	SDL_DestroyWindow(m_SdlWindow);
	m_SdlWindow = NULL;

	//Quit SDL subsystems
	SDL_Quit();
}

