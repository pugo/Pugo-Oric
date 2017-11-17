// Copyright (C) 2009-2016 by Anders Piniesjö <pugo@pugo.org>

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

AY3_8912::AY3_8912(std::shared_ptr<Machine> a_Machine) :
	m_Machine(a_Machine)
{
	m_Memory = m_Machine->GetMemory();
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

	m_CurrentKeyRow = 0;
	m_CurrentRegister = 0;

	for (uint8_t i=0; i < 8; i++) {
		m_KeyRows[i] = 0;
	}
	
	for (uint8_t i=0; i < 15; i++) {
		m_Registers[i] = 0;
	}
}

short AY3_8912::Exec(uint8_t a_Cycles)
{
	UpdateKeyOutput();
}

void AY3_8912::KeyPress(uint8_t a_KeyBits, bool a_Down)
{
	std::cout << "key: " << (int)a_KeyBits << ", " << (a_Down ? "down" : "up") << std::endl;
	if (a_Down) {
		m_KeyRows[a_KeyBits >> 3] |= (1 << (a_KeyBits & 0x07));
	}
	else {
		m_KeyRows[a_KeyBits >> 3] &= ~(1 << (a_KeyBits & 0x07));
	}
	
	if (m_CurrentKeyRow == (a_KeyBits >> 3)) {
		UpdateKeyOutput();
	}
}

void AY3_8912::UpdateKeyOutput()
{
	m_CurrentKeyRow = m_Machine->GetVIA()->ReadORB() & 0x07;

	if (m_KeyRows[m_CurrentKeyRow] & (m_Registers[IO_PORT_A] ^ 0xff)) {
		m_Machine->GetVIA()->SetIRBBit(3, true);
	}
	else {
		m_Machine->GetVIA()->SetIRBBit(3, false);
	}
}



void AY3_8912::SetBdir(bool a_Value)
{
	if (bdir != a_Value) {
		bdir = a_Value;
		if (bdir) {
			if (bc1) {
				// TODO: read this from machine instead, to decouple chips.
				uint8_t new_curr = m_Machine->GetVIA()->ReadORA();
				if (new_curr < NUM_REGS) {
					m_CurrentRegister = new_curr;
// 					std::cout << "!!!! --- new current reg: " << static_cast<unsigned int>(m_CurrentRegister) << std::endl;
				}
			}
			else {
				m_Registers[m_CurrentRegister] = m_Machine->GetVIA()->ReadORA();
// 				std::cout << "!!!! +++ register[" << static_cast<unsigned int>(m_CurrentRegister) << "] = "  << static_cast<unsigned 	int>(m_Registers[m_CurrentRegister]) << std::endl;
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
	a_Machine.GetAY3()->SetBdir(a_Value);
}

void AY3_8912::set_bc1(Machine& a_Machine, bool a_Value)
{
	a_Machine.GetAY3()->SetBc1(a_Value);
}

void AY3_8912::set_bc2(Machine& a_Machine, bool a_Value)
{ 
	a_Machine.GetAY3()->SetBc2(a_Value);
}
