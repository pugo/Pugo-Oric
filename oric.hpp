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

#ifndef ORIC_H
#define ORIC_H

#include <iostream>
#include <memory>

#include "machine.hpp"

class Frontend;

class Oric
{
public:
    enum State
    {
        STATE_RUN,
        STATE_MON,
        STATE_QUIT
    };

    static Oric& get_instance()
    {
        static Oric instance;
        return instance;
    }

    Oric();
    ~Oric();

    void init();

    Machine& get_machine() { return *machine; }
    Frontend& get_frontend() { return *frontend; }

    void run();
    void do_break();

protected:
    State handle_command(std::string& a_Cmd);
    uint16_t string_to_word(std::string& a_Addr);

    State state;
    Frontend* frontend;
    Machine* machine;
    std::string last_command;
};

#endif // ORIC_H
