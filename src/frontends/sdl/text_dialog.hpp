// =========================================================================
//   Copyright (C) 2009-2026 by Anders Piniesjö <pugo@pugo.org>
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

#ifndef FRONTENDS_SDL_TEXTDIALOG_H
#define FRONTENDS_SDL_TEXTDIALOG_H

#include <string>
#include <SDL3/SDL.h>

/**
 * Show a simple error dialog using SDL message box.
 * @param title title text
 * @param text body text
 */
void simple_error_dialog(std::string_view title, std::string_view text)
{
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, title.data(), text.data(), nullptr);
}

#endif // FRONTENDS_SDL_TEXTDIALOG_H
