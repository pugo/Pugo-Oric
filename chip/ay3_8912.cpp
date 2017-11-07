// Copyright (C) 2009-2016 by Anders Piniesjö <pugo@pugo.org>

#include <machine.hpp>
#include <memory.hpp>
#include "mos6522.hpp"

#include "ay3_8912.hpp"
#include <iostream>

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
}

short AY3_8912::Exec(uint8_t a_Cycles)
{
}

void AY3_8912::KeyPress(uint8_t a_KeyBits, bool a_Down)
{
	std::cout << "key: " << (int)a_KeyBits << ", " << (a_Down ? "down" : "up") << std::endl;
}
