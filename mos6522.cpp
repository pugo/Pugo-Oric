// =========================================================================
//   Copyright (C) 2009-2014 by Anders Piniesjö <pugo@pugo.org>
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

#include "mos6522.h"


using namespace std;


MOS6522::MOS6522(Machine* a_Machine, Memory* a_Memory) :
	m_Machine(a_Machine),
	m_Memory(a_Memory)
{
	Reset();
}

MOS6522::~MOS6522()
{
}

void MOS6522::Reset()
{
	t1_run = false;
	t2_run = false;

	ca1 = false;
	ca2 = false;
	cb1 = false;
	cb2 = false;

	orb = 0;
	ora = 0;

	ddrb = 0;
	ddra = 0;

	t1_latch_low = 0;
	t1_latch_high = 0;
	t1_counter = 0;
	t1_run = false;

	t2_latch_low = 0;
	t2_latch_high = 0;
	t2_counter = 0;
	t2_run = false;

	sr = 0;
	acr = 0;
	pcr = 0;
	ifr = 0;
	ier = 0;
	ira = 0;
	iral = 0;
	irb = 0;
	irbl = 0;
}

short MOS6522::Exec()
{
}

uint8_t MOS6522::ReadByte(uint16_t a_Offset)
{
	switch(a_Offset & 0x000f)
	{
	case VIA_ORB:
		cout << "Read VIA_ORB" << endl;
		IRQClear(VIA_IRQ_CB1);
		if ((pcr & 0xe0) == 0x00 || (pcr & 0xe0) == 0x40)
			IRQClear(VIA_IRQ_CB2);
		return orb;
	case VIA_ORA:
		cout << "Read VIA_ORA" << endl;
		IRQClear(VIA_IRQ_CA1);
		if ((pcr & 0x0e) == 0x00 || (pcr & 0x0e) == 0x04)
			IRQClear(VIA_IRQ_CA2);
		return ora;
	case VIA_DDRB:
		cout << "Read VIA_DDRB" << endl;
		return ddrb;
	case VIA_DDRA:
		cout << "Read VIA_DDRA" << endl;
		return ddra;
	case VIA_T1C_L:
		cout << "Read VIA_T1C_L" << endl;
		IRQClear(VIA_IRQ_T1);
		return t1_counter & 0x00ff;
	case VIA_T1C_H:
		cout << "Read VIA_T1C_H" << endl;
		return t1_counter >> 8;
	case VIA_T1L_L:
		cout << "Read VIA_T1L_L" << endl;
		return t1_latch_low;
	case VIA_T1L_H:
		cout << "Read VIA_T1L_H" << endl;
		return t1_latch_high;
	case VIA_T2C_L:
		cout << "Read VIA_T2C_L" << endl;
		IRQClear(VIA_IRQ_T2);
		return t2_counter & 0x00ff;
	case VIA_T2C_H:
		cout << "Read VIA_T2C_H" << endl;
		return t2_counter >> 8;
	case VIA_SR:
		cout << "Read VIA_SR" << endl;
		IRQClear(VIA_IRQ_SR);
		return sr;
	case VIA_ACR:
		cout << "Read VIA_ACR" << endl;
		return acr;
	case VIA_PCR:
		cout << "Read VIA_PCR" << endl;
		return pcr;
	case VIA_IFR:
		cout << "Read VIA_IFR" << endl;
		return ifr;
	case VIA_IER:
		cout << "Read VIA_IER" << endl;
		return ier | 0x80;
	case VIA_IORA2:
		cout << "Read VIA_IORA2" << endl;
		return ora;  // some bit manipulation..
	}
	return 0;
}

void MOS6522::WriteByte(uint16_t a_Offset, uint8_t a_Value)
{
	switch(a_Offset & 0x000f)
	{
	case VIA_ORB:
		cout << "Write VIA_ORB";
		orb = a_Value;
		IRQClear(VIA_IRQ_CB1);
		if ((pcr & 0xe0) == 0x00 || (pcr & 0xe0) == 0x40)
			IRQClear(VIA_IRQ_CB2);
		break;
	case VIA_ORA:
		cout << "Write VIA_ORA";
		ora = a_Value;
		IRQClear(VIA_IRQ_CA1);
		if ((pcr & 0x0e) == 0x00 || (pcr & 0x0e) == 0x04)
			IRQClear(VIA_IRQ_CA2);
		break;
	case VIA_DDRB:
		cout << "Write VIA_DDRB";
		ddrb = a_Value;
		break;
	case VIA_DDRA:
		cout << "Write VIA_DDRA";
		ddra = a_Value;
		break;
	case VIA_T1C_L:
		cout << "Write VIA_T1C_L";
		t1_latch_low = a_Value;
		break;
	case VIA_T1C_H:
		cout << "Write VIA_T1C_H";
		t1_latch_high = a_Value;
		t1_counter = (t1_latch_high << 8) | t1_latch_low;
		t1_run = true;
		IRQClear(VIA_IRQ_T1);
		// reload? Interrupt stuff
		// om översta biten i acr: släck översta i orb.
		break;
	case VIA_T1L_L:
		cout << "Write VIA_T1L_L";
		t1_latch_low = a_Value;
		break;
	case VIA_T1L_H:
		cout << "Write VIA_T1L_H";
		t1_latch_high = a_Value;
		IRQClear(VIA_IRQ_T1);
		break;
	case VIA_T2C_L:
		cout << "Write VIA_T2C_L";
		t2_latch_low = a_Value;
		break;
	case VIA_T2C_H:
		cout << "Write VIA_T2C_H";
		t2_latch_high = a_Value;
		t2_counter = (t2_latch_high << 8) | t2_latch_low;
		t2_run = true;
		IRQClear(VIA_IRQ_T2);
		// reload? 
		break;
	case VIA_SR:
		cout << "Write VIA_SR";
		sr = a_Value;
		IRQClear(VIA_IRQ_SR);
		break;
	case VIA_ACR:
		cout << "Write VIA_ACR";
		acr = a_Value;
		break;
	case VIA_PCR:
		cout << "Write VIA_PCR, ";
		pcr = a_Value;
		// TODO: ca and cb pulsing stuff.
		break;
	case VIA_IFR:
		cout << "Write VIA_IFR";
		ifr &= (~a_Value) & 0x7f;
		if (ifr & ier)
			ifr |= 0x80;
		break;
	case VIA_IER:
		cout << "Write VIA_IER";
		if( a_Value & VIA_IER_WRITE )
			ier |= (a_Value & 0x7f);	// if bit 7: turn on given bits.
		else
			ier &= ~(a_Value & 0x7f);	// if !bit 7: turn off given bits.
		// TODO: check interrupt? 
		break;
	case VIA_IORA2:
		cout << "Write VIA_IORA2";
		ora = a_Value;
		break;
	}
	
	cout << ": " << hex << (int) a_Value << endl;
}


void MOS6522::IRQSet(uint8_t bits)
{
	ifr |= bits;
	if ((ifr & ier) & 0x7f)
		ifr |= 0x80;

	if (bits & ier)
		cout << "TODO: send IRQ to CPU here!" << endl;
}

void MOS6522::IRQClear(uint8_t bits)
{
	ifr &= ~bits;

	// Clear bit 7 if no (enabled) interrupts exist.
	if (!((ifr & ier) & 0x7f))
		ifr &= 0x7f;
}


void MOS6522::WriteCA1(bool a_Value)
{
	if (!ca1 && a_Value) {
		// Positive transition only of enabled in PCR.
		if (pcr & VIA_PCR_CONTROL_CA1)
			IRQSet(VIA_IRQ_CA1);
	}
	else if (ca1 && ! a_Value) {
		// Negative transition only of enabled in PCR.
		if (!(pcr & VIA_PCR_CONTROL_CA1))
			IRQSet(VIA_IRQ_CA1);
	}

	ca1 = a_Value;
}

void MOS6522::WriteCA2(bool a_Value)
{
	if (!ca2 && a_Value) {
		// Positive transition only of enabled in PCR.
		if ((pcr & VIA_PCR_CONTROL_CA2) == 0x04 || (pcr & VIA_PCR_CONTROL_CA2) == 0x06) {
			IRQSet(VIA_IRQ_CA2);
		}
		ca2 = a_Value;
	}
	else if (ca2 && ! a_Value) {
		// Negative transition only of enabled in PCR.
		if ((pcr & VIA_PCR_CONTROL_CA2) == 0x00 || (pcr & VIA_PCR_CONTROL_CA2) == 0x02)
			IRQSet(VIA_IRQ_CA2);
		ca2 = a_Value;
	}
}


void MOS6522::WriteCB1(bool a_Value)
{
	if (!cb1 && a_Value) {
		// Positive transition only of enabled in PCR.
		if (pcr & VIA_PCR_CONTROL_CB1) {
			IRQSet(VIA_IRQ_CB1);
		}
	}
	else if (cb1 && ! a_Value) {
		// Negative transition only of enabled in PCR.
		if (!(pcr & VIA_PCR_CONTROL_CB1)) {
			IRQSet(VIA_IRQ_CB1);
		}
	}

	cb1 = a_Value;
}

void MOS6522::WriteCB2(bool a_Value)
{
	if (!cb2 && a_Value) {
		// Positive transition only of enabled in PCR.
		if ((pcr & VIA_PCR_CONTROL_CB2) == 0x40 || (pcr & VIA_PCR_CONTROL_CB2) == 0x60) {
			IRQSet(VIA_IRQ_CB2);
		}
		cb2 = a_Value;
	}
	else if (cb2 && ! a_Value) {
		// Negative transition only of enabled in PCR.
		if ((pcr & VIA_PCR_CONTROL_CB2) == 0x00 || (pcr & VIA_PCR_CONTROL_CB2) == 0x20) {
			IRQSet(VIA_IRQ_CB2);
		}
		cb2 = a_Value;
	}
}
