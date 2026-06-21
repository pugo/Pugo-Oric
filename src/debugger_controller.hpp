// =========================================================================
//   Copyright (C) 2009-2026 by Anders Piniesjö <pugo@pugo.org>
//
//   This program is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
// =========================================================================

#ifndef DEBUGGER_CONTROLLER_H
#define DEBUGGER_CONTROLLER_H

#include <cstdint>
#include <string>

class Machine;

class DebuggerController
{
public:
    enum class Action
    {
        Stay,
        Break,
        Continue,
        Quit
    };

    struct Result
    {
        Action action{Action::Stay};
        std::string output;
    };

    explicit DebuggerController(Machine& machine);

    void reset();
    Result execute(std::string command_line);

private:
    uint16_t string_to_word(const std::string& addr) const;
    std::string help_text() const;
    std::string step(size_t count);

    Machine& machine;
    std::string last_command;
    uint16_t last_address{0};
};

#endif // DEBUGGER_CONTROLLER_H
