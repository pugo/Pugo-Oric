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

AY3_8912::AY3_8912(Machine& a_Machine) :
	m_Machine(a_Machine),
	m_read_data_handler(nullptr)
{
	Reset();
}

AY3_8912::~AY3_8912()
{
}

void AY3_8912::Reset()
{
	bdir = false;
	bc1 = false;
	bc2 = false;

	m_CurrentRegister = 0;

	for (uint8_t i=0; i < 15; i++) {
		m_Registers[i] = 0;
	}
}

short AY3_8912::Exec(uint8_t a_Cycles)
{
	return 0;
}

void AY3_8912::SetBdir(bool a_Value)
{
	if (bdir != a_Value) {
		bdir = a_Value;
		if (bdir) {
			if (bc1) {
				uint8_t new_curr = m_read_data_handler(m_Machine);
				if (new_curr < NUM_REGS) {
					m_CurrentRegister = new_curr;
				}
			}
			else {
				m_Registers[m_CurrentRegister] = m_read_data_handler(m_Machine);
			}
		}
	}
}

void AY3_8912::SetBc1(bool a_Value)
{
	bc1 = a_Value;
}

void AY3_8912::SetBc2(bool a_Value)
{
	bc2 = a_Value;
}


void AY3_8912::set_bdir(Machine& a_Machine, bool a_Value) {
	a_Machine.GetAY3().SetBdir(a_Value);
}

void AY3_8912::set_bc1(Machine& a_Machine, bool a_Value)
{
	a_Machine.GetAY3().SetBc1(a_Value);
}

void AY3_8912::set_bc2(Machine& a_Machine, bool a_Value)
{ 
	a_Machine.GetAY3().SetBc2(a_Value);
}
