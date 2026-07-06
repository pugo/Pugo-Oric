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


#include <chrono>
#include <format>
#include <csignal>
#include <print>
#include <stdexcept>
#include <string>
#include <thread>

#include <boost/log/trivial.hpp>
#include <boost/algorithm/string.hpp>

#include "oric.hpp"
#include "memory.hpp"
#include "frontends/sdl/frontend.hpp"

volatile std::sig_atomic_t sigint_received = 0;


Oric::Oric(Config& config) :
    config(config),
    state(STATE_RUN),
    frontend(nullptr),
    machine(nullptr)
{
    if (config.start_in_monitor()) {
        state = STATE_HALTED;
    }
}

Oric::~Oric()
{
    if (frontend) {
        frontend->close_sound();
        frontend.reset();
    }
    machine.reset();
}

void check_rom_exists(std::filesystem::path path)
{
    if (! std::filesystem::exists(path)) {
        throw std::runtime_error(std::format("'{}' does not exist", path.string()));
    }

    if (! std::filesystem::is_regular_file(path)) {
        throw std::runtime_error(std::format("'{}' is not a file", path.string()));
    }
}

void Oric::init()
{
    machine = std::make_unique<Machine>(*this);
    debugger_controller = std::make_unique<DebuggerController>(*machine);
    frontend = std::make_unique<Frontend>(*this);

    frontend->get_status_bar().show_text_for("Starting Auric!", std::chrono::seconds(3));

    machine->init(frontend.get());

    try {
        if (config.use_oric1_rom()) {
            auto path = config.roms_path() / config.rom_name(RomType::Oric1);
            check_rom_exists(path);
            machine->oric_rom.load(path, 0x0000);
        }
        else {
            auto path = config.roms_path() / config.rom_name(RomType::OricAtmos);
            check_rom_exists(path);
            machine->oric_rom.load(path, 0x0000);
        }
    }
    catch (const std::runtime_error& err) {
        throw(std::runtime_error(std::format("Failed loading ROM: {}", err.what())));
    }

    try {
        auto path = config.roms_path() / config.rom_name(RomType::Microdisk);
        check_rom_exists(path);
        machine->disk_rom.load(path, 0x0000);
    }
    catch (const std::runtime_error& err) {
        throw(std::runtime_error(std::format("Failed loading disk drive ROM: {}", err.what())));
    }

    machine->init_tape();

    frontend->init_graphics();
    frontend->init_sound();

    machine->set_disassemble_execution(false);

    if (state == STATE_HALTED) {
        break_execution();
    }
}

void Oric::init_machine()
{
    machine = std::make_unique<Machine>(*this);
    debugger_controller = std::make_unique<DebuggerController>(*machine);
}

void Oric::run()
{
    bool do_run{true};

    while (do_run) {
        if (handle_sigint()) {
            continue;
        }

        switch (state) {
            case STATE_RUN:
                machine->run_until_frame_or_break(this);
                break;
            case STATE_HALTED:
                run_halted_frame();
                break;
            case STATE_QUIT:
                do_run = false;
                break;
        }
    }
    frontend->close_sound();
}

bool Oric::handle_sigint()
{
    if (!sigint_received) {
        return false;
    }

    sigint_received = 0;

    if (state == STATE_HALTED) {
        state = STATE_QUIT;
        return true;
    }

    break_execution();
    return true;
}


void Oric::do_break()
{
    break_execution();
}

void Oric::break_execution()
{
    if (debugger_controller) {
        debugger_controller->reset();
    }

    if (frontend) {
        frontend->show_debugger();
    }

    state = STATE_HALTED;
}

void Oric::continue_execution()
{
    state = STATE_RUN;
}


void Oric::do_quit()
{
    state = STATE_QUIT;
}

void Oric::run_halted_frame()
{
    if (!frontend->handle_frame()) {
        state = STATE_QUIT;
        return;
    }

    machine->render_current_frame();
    std::this_thread::sleep_for(std::chrono::milliseconds(20));
}

DebuggerController::Result Oric::submit_debugger_command(const std::string& command_line)
{
    std::string trimmed_command = command_line;
    boost::trim(trimmed_command);
    if (state == STATE_RUN && (trimmed_command == "s" || trimmed_command.starts_with("s "))) {
        break_execution();
    }

    auto result = debugger_controller->execute(command_line);

    if (result.action == DebuggerController::Action::Break) {
        break_execution();
    }
    else if (result.action == DebuggerController::Action::Continue) {
        continue_execution();
    }
    else if (result.action == DebuggerController::Action::Quit) {
        state = STATE_QUIT;
    }

    return result;
}
