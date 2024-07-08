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

#ifndef TAPE_H
#define TAPE_H

#include <iostream>
#include <memory>
#include <map>

class Tape
{
public:
    Tape() :
        do_run_motor(false)
    {}

    virtual bool init() = 0;
    virtual void reset() = 0;

    virtual void print_stat() = 0;

    virtual void set_motor(bool motor_on) = 0;
    virtual short exec(uint8_t cycles) = 0;

    bool do_run_motor;
};

#endif // TAPE_TAP_H
