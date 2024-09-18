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

#include <iostream>
#include <cstdlib>
#include <thread>
#include <sys/time.h>
#include <unistd.h>

#include <boost/assign.hpp>
#include <SDL.h>

#include "machine.hpp"
#include "oric.hpp"
#include "frontend.hpp"

// VIA Lines        Oric usage
// ----------       ---------------------------------
// PA0..PA7         PSG data bus, printer data lines
// CA1              printer acknowledge line
// CA2              PSG BC1 line
// PB0..PB2         keyboard lines-demultiplexer
// PB3              keyboard sense line
// PB4              printer strobe line
// PB5              (not connected)
// PB6              tape connector motor control
// PB7              tape connector output
// CB1              tape connector input
// CB2              PSG BDIR line

// 19968 cycles per frame / 312 lines = 64 cycles per raster
constexpr uint8_t cycles_per_raster = 64;
constexpr uint32_t sound_pause_target = 1000;


Machine::Machine(Oric* oric) :
    ula(this, &memory, Frontend::texture_width, Frontend::texture_height, Frontend::texture_bpp),
    oric(oric),
    memory(65535),
    tape(nullptr),
    cycle_count(0),
    next_frame(0),
    warpmode_on(false),
    break_exec(false),
    sound_paused(true),
    sound_pause_counter(0),
    current_key_row(0)
{
    for (uint8_t i=0; i < 8; i++) {
        key_rows[i] = 0;
    }
}

Machine::~Machine()
{}

void Machine::init(Frontend* frontend)
{
    this->frontend = frontend;
    init_cpu();
    init_mos6522();
    init_ay3();
    init_tape();
}

void Machine::init_cpu()
{
    cpu = new MOS6502(*this);
    cpu->memory_read_byte_handler = read_byte;
    cpu->memory_read_byte_zp_handler = read_byte_zp;
    cpu->memory_read_word_handler = read_word;
    cpu->memory_read_word_zp_handler = read_word_zp;
    cpu->memory_write_byte_handler = write_byte;
    cpu->memory_write_byte_zp_handler = write_byte_zp;
}

void Machine::init_mos6522()
{
    mos_6522 = new MOS6522(*this);

    // CA1 is connected to printer ACK line.
    // -- printer not supported.

    mos_6522->orb_changed_handler = via_orb_changed_callback;

    // CA2 is connected to AY BC1 line.
    mos_6522->ca2_changed_handler = AY3_8912::set_bc1_callback;

    // CB1 is connected to tape connector input, tape_tap.cpp writes directly to CB1.

    // CB2 is connected to AY BDIR line.
    mos_6522->cb2_changed_handler = AY3_8912::set_bdir_callback;

    mos_6522->psg_changed_handler = AY3_8912::update_state_callback;

    mos_6522->irq_handler = irq_callback;
    mos_6522->irq_clear_handler = irq_clear_callback;
}

void Machine::init_ay3()
{
    ay3 = new AY3_8912(*this);

    // AY data bus reads from VIA ORA (Output Register A).
    ay3->m_read_data_handler = read_via_ora;
    //	ay3->m_write_data_handler = write_vi
}

void Machine::init_tape()
{
    if (! oric->get_config().tape_path().empty()) {
        tape = new TapeTap(*mos_6522, oric->get_config().tape_path());
        if (!tape->init()) {
            exit(1);
        }
    }
    else {
        std::cout << "No tape specified." << std::endl;
        tape = new TapeBlank();
    }
}

void Machine::reset()
{
    cpu->Reset();
}


void Machine::run(Oric* oric)
{
    uint32_t instructions = 0;

    struct timeval tv;
    gettimeofday(&tv, NULL);
    next_frame = tv.tv_sec * 1000000 + tv.tv_usec;

    break_exec = false;
    uint8_t ran = 0;

    cycle_count += cycles_per_raster;

    while (! break_exec) {
        if (sound_paused) {
            sound_pause_counter += 1;

            if (sound_pause_counter > sound_pause_target) {
                sound_paused = false;
                frontend->pause_sound(false);
            }
        }

        while (cycle_count > 0) {
            tape->exec();
            mos_6522->exec();
            ay3->exec();

            if (cpu->exec(break_exec)) {
                update_key_output();
//                frontend->unlock_audio();
            }

            if (break_exec) {
                oric->do_break();
                return;
            }

            --cycle_count;
        }

        if (ula.paint_raster()) {
            next_frame += 20000;

            if (! frontend->handle_frame()) {
                break_exec = true;
            }

            gettimeofday(&tv, NULL);
            uint64_t now = tv.tv_sec * 1000000 + tv.tv_usec + 20000;
            if (now > next_frame) {
                next_frame = now;
            }
            else {
                if (! warpmode_on) {
                    usleep(next_frame - now);
                }
            }
        }

        cycle_count += cycles_per_raster;
    }
}

void Machine::key_press(uint8_t key_bits, bool down)
{
    if (down) {
        key_rows[key_bits >> 3] |= (1 << (key_bits & 0x07));
    }
    else {
        key_rows[key_bits >> 3] &= ~(1 << (key_bits & 0x07));
    }
}

void Machine::update_key_output()
{
    current_key_row = mos_6522->read_orb() & 0x07;

    if (! (ay3->get_register(AY3_8912::ENABLE) & 0x40))
    {
        mos_6522->set_irb_bit(3, false);
        return;
    }

    if (key_rows[current_key_row] & (ay3->get_register(AY3_8912::IO_PORT_A) ^ 0xff)) {
        mos_6522->set_irb_bit(3, true);
    }
    else {
        mos_6522->set_irb_bit(3, false);
    }
}


void Machine::via_orb_changed(uint8_t orb)
{
    bool motor_on = orb & 0x40;
    if (motor_on != tape->is_motor_running()) {
        tape->set_motor(motor_on);
    }
}

void Machine::save_snapshot()
{
    cpu->save_to_snapshot(snapshot);
    mos_6522->save_to_snapshot(snapshot);
    memory.save_to_snapshot(snapshot);
    ay3->save_to_snapshot(snapshot);

    std::cout << "Saved snapshot." << std::endl;
}

void Machine::load_snapshot()
{
    cpu->load_from_snapshot(snapshot);
    mos_6522->load_from_snapshot(snapshot);
    memory.load_from_snapshot(snapshot);
    ay3->load_from_snapshot(snapshot);

    std::cout << "Loaded snapshot." << std::endl;
}



bool Machine::toggle_warp_mode()
{
    warpmode_on = !warpmode_on;
    if (! warpmode_on) {
        struct timeval tv;
        gettimeofday(&tv, NULL);
        next_frame = tv.tv_sec * 1000000 + tv.tv_usec;
        frontend->unlock_audio();
    }
    else {
        frontend->lock_audio();
    }

    std::cout << "Warp mode: " << (warpmode_on ? "on" : "off") << std::endl;
    return warpmode_on;
}