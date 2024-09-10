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

#include <map>
#include <memory>
#include <iostream>
#include <ostream>
#include <vector>
#include <SDL.h>
#include <SDL_audio.h>

class Oric;
class Memory;

typedef std::map<int32_t, uint8_t> KeyMap_t;
typedef std::pair<int32_t, bool> KeyPress_t;
typedef std::map<KeyPress_t, KeyPress_t> KeyTranslation_t;


class Frontend
{
public:
    static const uint8_t texture_width = 240;
    static const uint8_t texture_height = 224;
    static const uint8_t texture_bpp = 4;

    Frontend(Oric* oric);
    ~Frontend();

    /**
     * Initialize graphics output.
     * @return true on success
     */
    bool init_graphics();

    /**
     * Close graphics output.
     */
    void close_graphics();

    /**
     * Initialize sound
     * @return true on success
     */
    bool init_sound();

    /**
     * Pause sound.
     * @param pause_on true if sound should be paused, false otherwise
     */
    void pause_sound(bool pause_on);

    /**
     * Lock the audio device.
     */
    void lock_audio_device() { SDL_LockAudioDevice(sound_audio_device_id); }

    /**
     * Unlock the audio device.
     */
    void unlock_audio_device() { SDL_UnlockAudioDevice(sound_audio_device_id); }

    /**
     * Lock audio playback.
     */
    void lock_audio() { SDL_LockAudio(); }

    /**
     * Unlock audio playback.
     */
    void unlock_audio() { SDL_UnlockAudio(); }

    /**
     * Close sound.
     */
    void close_sound();

    /**
     * Close SDL.
     */
    void close_sdl();

    /**
     * Perform all tasks happening each frame.
     * @return true if machine should continue.
     */
    bool handle_frame();

    /**
     * Render graphics.
     * @param pixels refernce to pixels to render
     */
    void render_graphics(std::vector<uint8_t>& pixels);

protected:
    Oric* oric;

    SDL_Window* sdl_window;
    SDL_Surface* sdl_surface;
    SDL_Renderer* sdl_renderer;
    SDL_Texture* sdl_texture;
    SDL_AudioDeviceID audio_device;
    SDL_AudioDeviceID sound_audio_device_id;

    KeyMap_t key_map;
    KeyTranslation_t key_translations;
};


#endif // FRONTEND_H
