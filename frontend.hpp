// =========================================================================
//   Copyright (C) 2009-2024 by Anders Piniesjö <pugo@pugo.org>
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
    static const uint8_t texture_width = 240;
    static const uint8_t texture_height = 224;
    static const uint8_t texture_bpp = 4;

    Frontend(Oric* oric);
    ~Frontend();

    bool init_graphics();
    void close_graphics();

    bool init_sound();
    void pause_sound(bool pause_on);
    void close_sound();

    void close_sdl();

    void render_graphics(std::vector<uint8_t>& pixels);

protected:
    Oric* oric;

    SDL_Window* sdl_window;
    SDL_Surface* sdl_surface;
    SDL_Renderer* sdl_renderer;
    SDL_Texture* sdl_texture;
    SDL_AudioDeviceID audio_device;

    SDL_AudioDeviceID sound_audio_device_id;
};


#endif // FRONTEND_H
