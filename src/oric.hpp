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

#ifndef ORIC_H
#define ORIC_H

#include <memory>
#include <filesystem>

#include "debugger_controller.hpp"
#include "machine.hpp"
#include "config.hpp"

class Frontend;

class Oric
{
public:
    enum State
    {
        STATE_RUN,
        STATE_HALTED,
        STATE_QUIT
    };

    explicit Oric(Config& config);
    ~Oric();

    /**
     * Initialize Oric.
     */
    void init();

    /**
     * Initialize Machine.
     */
    void init_machine();

    /**
     * Get current config.
     * @return reference to config
     */
    Config& get_config() { return config; }

    /**
     * Get current machine.
     * @return reference to machine
     */
    Machine& get_machine() { return *machine; }

    /**
     * Get frontend.
     * @return reference to frontend
     */
    Frontend& get_frontend() { return *frontend; }

    /**
     * Run Oric.
     */
    void run();

    /**
     * Break Oric.
     */
    void do_break();
    void break_execution();
    void continue_execution();
    bool is_halted() const { return state == STATE_HALTED; }

    /**
     * Quit Oric.
     */
    void do_quit();

    /**
     * Handle pending SIGINT from system.
     */
    void handle_sigint();

    DebuggerController::Result submit_debugger_command(const std::string& command_line);

protected:
    void run_halted_frame();

    Config& config;
    State state;
    std::unique_ptr<Frontend> frontend;
    std::unique_ptr<Machine> machine;
    std::unique_ptr<DebuggerController> debugger_controller;
};

#endif // ORIC_H
