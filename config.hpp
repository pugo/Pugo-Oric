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

#ifndef CONFIG_H
#define CONFIG_H

#include <iostream>
#include <memory>
#include <filesystem>

#include "machine.hpp"


class Config
{
public:
    Config();

    bool parse(int argc, char **argv);

    std::filesystem::path& tape_path() { return _tape_path; }
    bool start_in_monitor() { return _start_in_monitor; }
    bool use_atmos_rom() { return _use_atmos_rom; }

protected:
    bool _start_in_monitor;
    bool _use_atmos_rom;
    std::filesystem::path _tape_path;
};

#endif // CONFIG_H
