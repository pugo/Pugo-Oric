// =========================================================================
//   Copyright (C) 2009-2026 by Anders Piniesjö <pugo@pugo.org>
//
//   This program is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
// =========================================================================

#include "debugger_controller.hpp"

#include <boost/algorithm/string.hpp>
#include <format>
#include <sstream>
#include <string>
#include <vector>

#include "machine.hpp"

DebuggerController::DebuggerController(Machine& machine) :
    machine(machine)
{
}

void DebuggerController::reset()
{
    last_command.clear();
    last_address = 0;
}

uint16_t DebuggerController::string_to_word(const std::string& addr) const
{
    uint16_t x = 0;
    std::stringstream ss;
    ss << std::hex << addr;
    ss >> x;
    return x;
}

std::string DebuggerController::help_text() const
{
    return
        "Available monitor commands:\n\n"
        "ay              : print AY-3-8912 sound chip info\n"
        "b               : break\n"
        "bs <address>    : set breakpoint for address\n"
        "d               : disassemble from last address or PC\n"
        "d <address> <n> : disassemble from address and len and n bytes ahead\n"
        "                  (example: d c000 10)\n"
        "debug           : show debug output at run time\n"
        "g               : go (continue)\n"
        "g <address>     : go to address and run (example: g 1f00)\n"
        "h               : help (showing this text)\n"
        "i               : print machine info\n"
        "m <address> <n> : dump memory from address and n bytes ahead\n"
        "                  (example: m 1f00 20)\n"
        "pc <address>    : set program counter to address\n"
        "quiet           : prevent debug output at run time\n"
        "q               : quit\n"
        "s [n]           : step one or possible n steps\n"
        "sr, softreset   : soft reset oric\n"
        "v               : print VIA (6522) info\n";
}

std::string DebuggerController::step(size_t count)
{
    std::ostringstream out;

    for (size_t i = 0; i < count; ++i) {
        bool brk = false;
        while (!machine.cpu->exec(false, brk)) {}
        if (brk) {
            out << "Instruction BRK executed.\n";
            break;
        }
    }

    out << machine.format_stat();
    return out.str();
}

DebuggerController::Result DebuggerController::execute(std::string command_line)
{
    boost::trim(command_line);

    if (command_line.empty()) {
        if (last_command.empty()) {
            return {Action::Stay, ""};
        }
        command_line = last_command;
    }
    else {
        last_command = command_line;
    }

    std::vector<std::string> parts;
    boost::split(parts, command_line, boost::is_any_of("\t "), boost::token_compress_on);
    const std::string& cmd = parts[0];

    if (cmd == "h") {
        return {Action::Stay, help_text()};
    }
    if (cmd == "ay") {
        return {Action::Stay, machine.ay3->status_string()};
    }
    if (cmd == "b") {
        return {Action::Break, "Exec break\n"};
    }
    if (cmd == "bs") {
        if (parts.size() < 2) {
            return {Action::Stay, "Error: missing address\n"};
        }
        const uint16_t addr = string_to_word(parts[1]);
        machine.cpu->set_breakpoint(addr);
        return {Action::Stay, std::format("Set breakpoint at ${:04X}\n", addr)};
    }
    if (cmd == "d") {
        if (parts.size() == 1) {
            const uint16_t addr = (last_address == 0) ? machine.cpu->get_pc() : last_address;
            auto result = machine.get_monitor().disassemble_to_string(addr, 30);
            last_address = result.next_address;
            return {Action::Stay, result.output};
        }
        if (parts.size() < 3) {
            return {Action::Stay, "Use: d <start address> <length>\n"};
        }
        auto result = machine.get_monitor().disassemble_to_string(string_to_word(parts[1]), string_to_word(parts[2]));
        last_address = result.next_address;
        return {Action::Stay, result.output};
    }
    if (cmd == "debug") {
        machine.set_disassemble_execution(true);
        return {Action::Stay, "Debug mode enabled\n"};
    }
    if (cmd == "g") {
        if (parts.size() >= 2) {
            machine.cpu->set_pc(string_to_word(parts[1]));
        }
        return {Action::Continue, ""};
    }
    if (cmd == "i") {
        return {Action::Stay, std::format("PC: ${:04X}\n{}", machine.cpu->get_pc(), machine.format_stat())};
    }
    if (cmd == "m") {
        if (parts.size() < 3) {
            return {Action::Stay, "Use: m <start address> <length>\n"};
        }
        return {Action::Stay, machine.memory.dump_string(string_to_word(parts[1]), string_to_word(parts[2]))};
    }
    if (cmd == "pc") {
        if (parts.size() < 2) {
            return {Action::Stay, "Error: missing address\n"};
        }
        machine.cpu->set_pc(string_to_word(parts[1]));
        return {Action::Stay, machine.format_stat()};
    }
    if (cmd == "q") {
        return {Action::Quit, "quit\n"};
    }
    if (cmd == "quiet") {
        machine.set_disassemble_execution(false);
        return {Action::Stay, "Quiet mode enabled\n"};
    }
    if (cmd == "s") {
        const size_t count = (parts.size() == 2) ? std::stoul(parts[1]) : 1;
        return {Action::Stay, step(count)};
    }
    if (cmd == "sr" || cmd == "softreset") {
        machine.cpu->NMI();
        return {Action::Stay, "NMI triggered\n"};
    }
    if (cmd == "v") {
        return {Action::Stay, machine.mos_6522->get_state().to_string()};
    }

    return {Action::Stay, std::format("Unknown command \"{}\". Use command \"h\" to get help.\n", cmd)};
}
