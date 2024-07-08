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

#include <iostream>
#include <cstdlib>
#include <vector>

#include "tape_blank.hpp"


TapeBlank::TapeBlank()
{
}


TapeBlank::~TapeBlank()
{
}


void TapeBlank::reset()
{
}


bool TapeBlank::init()
{
    return true;
}


void TapeBlank::print_stat()
{
    std::cout << "Blank Tape." << std::endl;
}


void TapeBlank::set_motor(bool motor_on)
{
    do_run_motor = motor_on;
}


short TapeBlank::exec(uint8_t cycles)
{
    return 0;
}

