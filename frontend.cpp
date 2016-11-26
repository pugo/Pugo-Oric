// Copyright (C) 2009-2016 by Anders Piniesjö <pugo@pugo.org>

#include "frontend.hpp"

#include <SDL_image.h>

Frontend::Frontend(std::shared_ptr<Oric> a_Oric) :
	m_Oric(a_Oric),
	m_SdlWindow(NULL),
	m_SdlSurface(NULL),
	m_SdlRenderer(NULL)
{
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

void Frontend::UpdateGraphics(uint8_t a_RasterLine, uint8_t* a_Mem)
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

void Frontend::RenderGraphics()
{
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
