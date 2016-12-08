// Copyright (C) 2009-2016 by Anders Piniesjö <pugo@pugo.org>

#include "frontend.hpp"

#include <SDL_image.h>

Frontend::Frontend(std::shared_ptr<Oric> a_Oric) :
	m_Oric(a_Oric),
	m_SdlWindow(NULL),
	m_SdlSurface(NULL),
	m_SdlRenderer(NULL)
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
		printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
		success = false;
	}
	else {
		//Set texture filtering to linear
		if(!SDL_SetHint( SDL_HINT_RENDER_SCALE_QUALITY, "1" ))
		{
			printf( "Warning: Linear texture filtering not enabled!" );
		}

		//Create window (240x224)
		m_SdlWindow = SDL_CreateWindow("SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 600, 600, SDL_WINDOW_SHOWN);
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

void Frontend::UpdateGraphics(uint8_t a_RasterLine, uint8_t* a_Mem)
{
	uint8_t* char_mem = a_Mem + 0xb400;
	
	uint8_t dx = 0;
	uint32_t bg_col = m_Colors[0];
	uint32_t fg_col = m_Colors[7];
	uint32_t* texture_line = (uint32_t*)&m_Pixels[a_RasterLine * m_TextureWidth * m_TextureBpp];
	
	for (uint16_t x = 0; x < 40; x++) {
		uint8_t ch = a_Mem[0xbb80 + (a_RasterLine >> 3)*40 + x];
		uint8_t chr_dat = char_mem[((ch & 0x7f) << 3) + (a_RasterLine & 0x07)];

		if (!(ch & 0x60)) {
			chr_dat = 0;
			switch(ch & 0x18)
			{
				case 0x00:
					fg_col = m_Colors[ch & 7];
					break;
				case 0x08:
// 					lattr = ch & 7;
// 					std::cout << " ------- LATTR" << std::endl;
					break;
				case 0x10:
					bg_col = m_Colors[ch & 7];
					break;
				case 0x18:
// 					pattr = ch & 7;
// 					std::cout << " ------- PATTR" << std::endl;
					break;
			}
		}
		
		uint32_t _fg_col = fg_col;
		uint32_t _bg_col = bg_col;
		
		if (ch & 0x80) {
			// Inverse colors
			_fg_col = _fg_col ^ 0x00ffffff;
			_bg_col = _bg_col ^ 0x00ffffff;
		}

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
