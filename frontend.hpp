// =========================================================================
//   Copyright (C) 2009-2023 by Anders Piniesjö <pugo@pugo.org>
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

#ifndef FRONTEND_H
#define FRONTEND_H

#include <memory>
#include <iostream>
#include <ostream>
#include <vector>
#include <SDL.h>
#include <SDL_audio.h>

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

	Frontend(Oric* oric);
	~Frontend();

    bool init_graphics();
	void close_graphics();

    bool init_sound();
    void close_sound();

    void close_sdl();

    void update_graphics(uint8_t raster_line, uint8_t* mem);
	void render_graphics();

protected:
	uint32_t colors[8] {0xff000000, 0xffff0000, 0xff00ff00, 0xffffff00, 0xff0000ff, 0xffff00ff, 0xff00ffff, 0xffffffff};
	const uint8_t texture_width = 240;
	const uint8_t texture_height = 224;
	const uint8_t texture_bpp = 4;

	Oric* oric;

	SDL_Window* sdl_window;
	SDL_Surface* sdl_surface;
	SDL_Renderer* sdl_renderer;
	SDL_Texture* sdl_texture;
	SDL_AudioDeviceID audio_device;

	std::vector<uint8_t> pixels;
	uint8_t video_attrib;
	uint8_t text_attrib;

    SDL_AudioDeviceID sound_audio_device_id;
};



#endif // FRONTEND_H
