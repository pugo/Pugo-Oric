// Copyright (C) 2009-2018 by Anders Piniesjö <pugo@pugo.org>

#include <machine.hpp>
#include <memory.hpp>
#include "mos6522.hpp"

#include "ay3_8912.hpp"
#include <iostream>
#include <bitset>

//                                     |   7  |   6  |   5  |   4  |   3  |   2  |   1  |   0  |
//     0     Channel A Tone Period     |                  8 bit fine tune A                    |
//     1                               |                           |        coarse tune A      |
//     2     Channel B Tone Period     |                  8 bit fine tune B                    |
//     3                               |                           |        coarse tune B      |
//     4     Channel C Tone Period     |                  8 bit fine tune C                    |
//     5                               |                           |        coarse tune C      |
//     6     Noise Period              |                    |       5 bit period control       |
//     7     Enable (inv)              |    IN~/OUT  |       Noise~       |        Tone~       |
//                                     |  IOB |  IOA |   C  |   B  |   A  |   C  |   B  |   A  |
//     8     Channel A Amplitude       |                    |   M  |  L3  |  L2  |  L1  |  L0  |
//     9     Channel B Amplitude       |                    |   M  |  L3  |  L2  |  L1  |  L0  |
//     A     Channel C Amplitude       |                    |   M  |  L3  |  L2  |  L1  |  L0  |
//     B     Envelope Period           |                  8 bit fine tune E                    |
//     C                               |                  8 bit Coarse tune E                  |
//     D     Envelope shape/cycle      |                           | CONT |  ATT |  ALT | HOLD |
//     E     IO Port A Data Store      |                8 bit parallel IO on port A            |
//     F     IO Port B Data Store      |                8 bit parallel IO on port B            |


// BDIR   BC2   BC1
//   0     1     0     Inactive
//   0     1     1     Read from PSG
//   1     1     0     Write to PSG
//   1     1     1     Latch address

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
			if (bc1) {  // 1 1 0
				uint8_t new_curr = m_read_data_handler(machine);
				if (new_curr < NUM_REGS) {
					current_register = new_curr;
				}
			}
			else {      // 1 1 1
				registers[current_register] = m_read_data_handler(machine);
			}
		}
		else {
			if (bc1) {  // 0 1 1
				std::cout << "---- read AY" << std::endl;
			}
		}
	}

	std::cout << "AY3_8912 regs: " << std::hex <<
  	  (int)registers[0] << " " << (int)registers[1] << " " << (int)registers[2] << " " << (int)registers[3] << " " <<
 	  (int)registers[4] << " " << (int)registers[5] << " " << (int)registers[6] << " " << (int)registers[7] << " " <<
	  (int)registers[8] << " " << (int)registers[9] << " " << (int)registers[10] << " " << (int)registers[11] << " " <<
     (int)registers[12] << " " << (int)registers[13] << " " << (int)registers[14] << " " << (int)registers[15] << " " << std::endl;
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
	machine.ay3->set_bdir(a_Value);
}

void AY3_8912::set_bc1(Machine& machine, bool a_Value)
{
	machine.ay3->set_bc1(a_Value);
}

void AY3_8912::set_bc2(Machine& machine, bool a_Value)
{
	machine.ay3->set_bc2(a_Value);
}
