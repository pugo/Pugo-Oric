// Copyright (C) 2009-2018 by Anders Piniesjö <pugo@pugo.org>

#include <machine.hpp>
#include <memory.hpp>
#include "mos6522.hpp"

#include "ay3_8912.hpp"
#include <iostream>
#include <bitset>

// Registers
//              0, 1 | Lowest 12 bits = pitch channel A 
//              2, 3 | Lowest 12 bits = pitch channel B
//              4, 5 | Lowest 12 bits = pitch channel C
//                 6 | Lowest 5 bits = pitch of noise channel
//                 7 | Enablers, bit 6 = port A output/input
//                 8 | Bit 3, 4, 5 = mix noise with ch A, B or C
//                   | Bit 0, 1, 2 = enable ch A, B, C
// 9, 10, 11, 12, 13 | Channel amplitudes
//                14 | I/O port A

using namespace std;

AY3_8912::AY3_8912(Machine& machine) :
	machine(machine),
	m_read_data_handler(nullptr)
{
	reset();
}

AY3_8912::~AY3_8912()
{
}

void AY3_8912::reset()
{
	bdir = false;
	bc1 = false;
	bc2 = false;

	current_register = 0;

	for (uint8_t i=0; i < 15; i++) {
		registers[i] = 0;
	}
}

short AY3_8912::exec(uint8_t cycles)
{
	return 0;
}

void AY3_8912::set_bdir(bool value)
{
	if (bdir != value) {
		bdir = value;
		if (bdir) {
			if (bc1) {
				uint8_t new_curr = m_read_data_handler(machine);
				if (new_curr < NUM_REGS) {
					current_register = new_curr;
				}
			}
			else {
				registers[current_register] = m_read_data_handler(machine);
			}
		}
	}
}

void AY3_8912::set_bc1(bool value)
{
	bc1 = value;
}

void AY3_8912::set_bc2(bool value)
{
	bc2 = value;
}


void AY3_8912::set_bdir(Machine& machine, bool a_Value) {
	machine.get_ay3().set_bdir(a_Value);
}

void AY3_8912::set_bc1(Machine& machine, bool a_Value)
{
	machine.get_ay3().set_bc1(a_Value);
}

void AY3_8912::set_bc2(Machine& machine, bool a_Value)
{
	machine.get_ay3().set_bc2(a_Value);
}
