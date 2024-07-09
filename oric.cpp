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

#include <unistd.h>
#include <signal.h>

#include <sstream>
#include <cstdlib>
#include <vector>
#include <string>

#include <boost/program_options.hpp>
#include <boost/algorithm/string.hpp>

#include "oric.hpp"
#include "memory.hpp"
#include "frontend.hpp"

namespace po = boost::program_options;


Oric::Oric() :
    state(STATE_RUN),
    last_command(""),
    last_address(0)
{
}


bool Oric::parse_config(int argc, char **argv)
{
    try {
        po::options_description desc("Allowed options");
        desc.add_options()
            ("help,?", "produce help message")
            ("tape,t", po::value<std::filesystem::path>(&tape_path), "Tape file to use");

        po::variables_map vm;
        po::store(po::command_line_parser(argc, argv).options(desc).run(), vm);

        if (vm.count("help")) {
            std::cout << "Usage: oric [options]" << std::endl << desc;
            return false;
        }

        po::notify(vm);
    }
    catch(std::exception& e)
    {
        std::cout << e.what() << std::endl;
        return false;
    }
    return true;
}



void Oric::init()
{
    machine = new Machine(this);
    frontend = new Frontend(this);

    machine->init(frontend);
    frontend->init_graphics();
    frontend->init_sound();

    machine->cpu->set_quiet(true);
}


Oric::~Oric()
{
    delete machine;
    delete frontend;
}


void Oric::run()
{
    while (true) {
        switch (state) {
            case STATE_RUN:
                machine->run(0, this);
                break;
            case STATE_MON:
            {
                std::string cmd;
                std::cout << ">> " << std::flush;

                if (! getline(std::cin, cmd)) {
                    state = STATE_QUIT;
                    break;
                }

                state = handle_command(cmd);
                break;
            }
            case STATE_QUIT:
                return;
        }
    }
}


void Oric::do_break()
{
    last_command = "";
    last_address = 0;
    state = STATE_MON;
}


uint16_t Oric::string_to_word(std::string& a_Addr)
{
    uint16_t x;
    std::stringstream ss;
    ss << std::hex << a_Addr;
    ss >> x;
    return x;
}


Oric::State Oric::handle_command(std::string& a_Cmd)
{
    if (a_Cmd.length() == 0) {
        if (last_command.length() == 0) {
            return STATE_MON;
        }
        a_Cmd = last_command;
    }
    else {
        last_command = a_Cmd;
    }

    std::vector<std::string> parts;
    boost::split(parts, a_Cmd, boost::is_any_of("\t "));
    std::string cmd = parts[0];

    if (cmd == "h") {
        std::cout << "Available monitor commands:" << std::endl << std::endl;
        std::cout << "h              : help (showing this text)" << std::endl;
        std::cout << "g <address>    : go to address and run" << std::endl;
        std::cout << "pc <address>   : set program counter to address" << std::endl;
        std::cout << "s [n]          : step one or possible n steps" << std::endl;
        std::cout << "i              : print machine info" << std::endl;
        std::cout << "v              : print VIA (6522) info" << std::endl;
        std::cout << "d              : disassemble from PC" << std::endl;
        std::cout << "d <address> <n>: disassemble from address and n bytes ahead" << std::endl;
        std::cout << "m <address> <n>: dump memory from address and n bytes ahead" << std::endl;
        std::cout << "quiet          : prevent debug output at run time" << std::endl;
        std::cout << "debug          : show debug output at run time" << std::endl;
        std::cout << "" << std::endl;
        return STATE_MON;
    }
    else if (cmd == "g") { // go <address>
        return STATE_RUN;
    }
    else if (cmd == "pc") { // set pc
        if (parts.size() < 2) {
            std::cout << "Error: missing address" << std::endl;
            return STATE_MON;
        }
        uint16_t addr = string_to_word(parts[1]);
        machine->cpu->set_pc(addr);
        machine->cpu->PrintStat();
    }
    else if (cmd == "s") { // step
        if (parts.size() == 2) {
            machine->run(std::stol(parts[1]), this);
        }
        else {
            bool brk = false;
            machine->cpu->exec_instruction(brk);
            if (brk) {
                std::cout << "Instruction BRK executed." << std::endl;
            }
        }
        machine->cpu->PrintStat();
    }
    else if (cmd == "i") { // info
        std::cout << "PC: " << machine->cpu->get_pc() << std::endl;
        machine->cpu->PrintStat();
    }
    else if (cmd == "d") { // info
        if (parts.size() == 1) {
            uint16_t addr = (last_address == 0) ? machine->cpu->get_pc() : last_address;
            last_address = machine->cpu->get_monitor().disassemble(addr, 30);
            return STATE_MON;
        }
        if (parts.size() < 3) {
            std::cout << "Use: d <start address> <length>" << std::endl;
            return STATE_MON;
        }
        last_address = machine->cpu->get_monitor().disassemble(string_to_word(parts[1]), string_to_word(parts[2]));
    }
    else if (cmd == "v") { // info
        machine->mos_6522->print_stat();
    }
    else if (cmd == "m") { // info
        if (parts.size() < 3) {
            std::cout << "Use: m <start address> <length>" << std::endl;
            return STATE_MON;
        }
        machine->memory.show(string_to_word(parts[1]), string_to_word(parts[2]));
    }
    else if (cmd == "quiet") {
        machine->cpu->set_quiet(true);
        std::cout << "Quiet mode enabled" << std::endl;
    }
    else if (cmd == "debug") {
        machine->cpu->set_quiet(false);
        std::cout << "Debug mode enabled" << std::endl;
    }

    else if (cmd == "q") { // quit
        std::cout << "quit" << std::endl;
        return STATE_QUIT;
    }

    return STATE_MON;
}


static void signal_handler(int);
void init_signals(void);

struct sigaction sigact;

static void signal_handler(int a_Sig)
{
    std::cout << "Signal: " << a_Sig << std::endl;
    if (a_Sig == SIGINT) {
        Oric::get_instance().get_machine().stop();
        Oric::get_instance().do_break();
    }
}


void init_signals()
{
    sigact.sa_handler = signal_handler;
    sigemptyset(&sigact.sa_mask);
    sigact.sa_flags = 0;
    sigaction(SIGINT, &sigact, (struct sigaction *)NULL);
}


int main(int argc, char *argv[])
{
    char pwd[1024];
    getcwd(pwd, 1024);
    std::cout << "pwd: " << pwd << std::endl;

    init_signals();

    Oric& oric = Oric::get_instance();

    if (! oric.parse_config(argc, argv)) {
        return 0;
    }

    oric.init();
	oric.get_machine().memory.load("ROMS/basic10.rom", 0xc000);
//    oric.get_machine().memory.load("ROMS/basic11b.rom", 0xc000);
    oric.get_machine().reset();

    std::cout << std::endl;
    oric.run();

    return 0;
}
