// Copyright (C) 2009-2016 by Anders Piniesjö <pugo@pugo.org>

#include <iostream>

#include <machine.hpp>
#include <memory.hpp>
#include "mos6522.hpp"

#include <boost/assign.hpp>
#include <boost/tuple/tuple.hpp>
#include <vector>
#include <stack>
#include <map>
#include <string>
#include <utility>
#include <bitset>

using namespace std;

// Oric port B:
// | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
//                 | kp| Keyb. row |


MOS6522::MOS6522(std::shared_ptr<Machine> a_Machine) :
	m_Machine(a_Machine),
	ca2_changed_handler(nullptr),
	cb2_changed_handler(nullptr)
{
	m_Memory = m_Machine->GetMemory();
	boost::assign::insert(m_RegisterNames)
		(ORB, "ORB")(ORA, "ORA")(DDRB, "DDRB")(DDRA, "DDRA")
		(T1C_L, "T1C_L")(T1C_H, "T1C_H")(T1L_L, "T1L_L")(T1L_H, "T1L_H")
		(T2C_L, "T2C_L")(T2C_H, "T2C_H")(SR, "SR")(ACR, "ACR")
		(PCR, "PCR")(IFR, "IFR")(IER, "IER")(IORA2, "IORA2");

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
	ca2_do_pulse = false;

	cb1 = false;
	cb2 = false;
	cb2_do_pulse = false;

	ira = 0;   // input register A
	ora = 0;   // output register A
	ddra = 0;  // data direction register A

	irb = 0;   // input register B
	orb = 0;   // output register B
	ddrb = 0;  // data direction register B

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
}

short MOS6522::Exec(uint8_t a_Cycles)
{
	if (ca2_do_pulse) {
		ca2 = true;
		ca2_do_pulse = false;
		if (ca2_changed_handler) { ca2_changed_handler(*m_Machine, ca2); }
	}

	if (cb2_do_pulse) {
		cb2 = true;
		cb2_do_pulse = false;
		if (cb2_changed_handler) { cb2_changed_handler(*m_Machine, cb2); }
	}

	t1_counter -= a_Cycles;
	
	if (t1_counter < 0) {
		switch (acr & 0xc0)
		{
			case 0x00:
			case 0x80:
				// One shot
				if (t1_run) {
					IRQSet(IRQ_T1);
					t1_run = false;
				}
				t1_counter &= 0xffff;
				break;
			case 0x40:
			case 0xC0:
				// Continuous
				if (t1_run) {
					IRQSet(IRQ_T1);
				}
				t1_counter += ((t1_latch_high << 8) | t1_latch_low) + 2; // +2: compensate for boundary time and load time.
				break;
		}
	}

	t2_counter -= a_Cycles;
	if (t2_counter < 0) {
		if (t2_run) {
			IRQSet(IRQ_T2);
		}
		t2_counter &= 0xffff;
	}

	if ((orb & ddrb)) {
//		std::cout << "---> orb: " << (orb & ddrb) << std::endl;
	}
}

uint8_t MOS6522::ReadByte(uint16_t a_Offset)
{
	switch(a_Offset & 0x000f)
	{
	case ORB:
// 	cout << "Read " << m_RegisterNames[static_cast<Register>(a_Offset & 0x000f)] << endl;
		IRQClear(IRQ_CB1);
		switch (pcr & PCR_MASK_CA2) {
			case 0x00:
			case 0x40:
				IRQClear(IRQ_CB2);
				break;
			case 0x80:
				// set CB2 to low on read/write of ORB if CB2-ctrl is 100.
				cb2 = false;
				if (cb2_changed_handler) { cb2_changed_handler(*m_Machine, cb2); }
				break;
			case 0xa0:
				// pulse low for one cycle if CB2-ctrl is 101.
				cb2 = false;
				cb2_do_pulse = true;
				if (cb2_changed_handler) { cb2_changed_handler(*m_Machine, cb2); }
				break;
		}
		return (orb & ddrb) | (irb & ~ddrb);
	case ORA:
// 	cout << "Read " << m_RegisterNames[static_cast<Register>(a_Offset & 0x000f)] << endl;
		IRQClear(IRQ_CA1);
		switch (pcr & PCR_MASK_CA2) {
			case 0x00:
			case 0x04:
				IRQClear(IRQ_CA2);
				break;
			case 0x08:
				// set CA2 to low on read/write of ORA if CA2-ctrl is 100.
				ca2 = false;
				if (ca2_changed_handler) { ca2_changed_handler(*m_Machine, ca2); }
				break;
			case 0x0a:
				// pulse low for one cycle if CA2-ctrl is 101.
				ca2 = false;
				ca2_do_pulse = true;
				if (ca2_changed_handler) { ca2_changed_handler(*m_Machine, ca2); }
				break;
		}
		return (ora & ddra) | (ira & ~ddra);
	case DDRB:
		return ddrb;
	case DDRA:
		return ddra;
	case T1C_L:
		IRQClear(IRQ_T1);
		return t1_counter & 0x00ff;
	case T1C_H:
		return t1_counter >> 8;
	case T1L_L:
		return t1_latch_low;
	case T1L_H:
		return t1_latch_high;
	case T2C_L:
		IRQClear(IRQ_T2);
		return t2_counter & 0x00ff;
	case T2C_H:
		return t2_counter >> 8;
	case SR:
		IRQClear(IRQ_SR);
		return sr;
	case ACR:
		return acr;
	case PCR:
		return pcr;
	case IFR:
		return ifr;
	case IER:
		return ier | 0x80;
	case IORA2:
		return (ora & ddra) | (ira & ~ddra);
	}
	return 0;
}

void MOS6522::WriteByte(uint16_t a_Offset, uint8_t a_Value)
{
	switch(a_Offset & 0x000f)
	{
	case ORB:
// 	cout << "Write " << m_RegisterNames[static_cast<Register>(a_Offset & 0x000f)] << ": " << static_cast<unsigned int>(a_Value) << endl;
		orb = a_Value;
		IRQClear(IRQ_CB1);
		switch (pcr & PCR_MASK_CA2) {
			case 0x00:
			case 0x40:
				IRQClear(IRQ_CB2);
				break;
			case 0x80:
				// set CB2 to low on read/write of ORB if CB2-ctrl is 100.
				cb2 = false;
				if (cb2_changed_handler) { cb2_changed_handler(*m_Machine, cb2); }
				break;
			case 0xa0:
				// pulse low for one cycle if CB2-ctrl is 101.
				cb2 = false;
				cb2_do_pulse = true;
				if (cb2_changed_handler) { cb2_changed_handler(*m_Machine, cb2); }
				break;
		}
		break;
	case ORA:
// 	cout << "Read " << m_RegisterNames[static_cast<Register>(a_Offset & 0x000f)] << endl;
		ora = a_Value;
		IRQClear(IRQ_CA1);
		switch (pcr & PCR_MASK_CA2) {
			case 0x00:
			case 0x04:
				IRQClear(IRQ_CA2);
				break;
			case 0x08:
				// set CA2 to low on read/write of ORA if CA2-ctrl is 100.
				ca2 = false;
				if (ca2_changed_handler) { ca2_changed_handler(*m_Machine, ca2); }
				break;
			case 0x0a:
				// pulse low for one cycle if CA2-ctrl is 101.
				ca2 = false;
				ca2_do_pulse = true;
				if (ca2_changed_handler) { ca2_changed_handler(*m_Machine, ca2); }
				break;
		}
		break;
	case DDRB:
		ddrb = a_Value;
		break;
	case DDRA:
		ddra = a_Value;
		break;
	case T1C_L:
		t1_latch_low = a_Value;
		break;
	case T1C_H:
		t1_latch_high = a_Value;
		t1_counter = (t1_latch_high << 8) | t1_latch_low;
		t1_run = true;
		IRQClear(IRQ_T1);
		// reload? Interrupt stuff
		// om översta biten i acr: släck översta i orb.
		break;
	case T1L_L:
		t1_latch_low = a_Value;
		break;
	case T1L_H:
		t1_latch_high = a_Value;
		IRQClear(IRQ_T1);
		break;
	case T2C_L:
		t2_latch_low = a_Value;
		break;
	case T2C_H:
		t2_latch_high = a_Value;
		t2_counter = (t2_latch_high << 8) | t2_latch_low;
		t2_run = true;
		IRQClear(IRQ_T2);
		// reload? 
		break;
	case SR:
		sr = a_Value;
		IRQClear(IRQ_SR);
		break;
	case ACR:
		acr = a_Value;
		break;
	case PCR:
		pcr = a_Value;
		// Manual output modes
		if ((pcr & 0x0c) == 0x0c) {
			ca2 = !!(pcr & 0x02);
			if (ca2_changed_handler) { ca2_changed_handler(*m_Machine, ca2); }
		}
		if ((pcr & 0xc0) == 0xc0) {
			cb2 = !!(pcr & 0x20);
			if (cb2_changed_handler) { cb2_changed_handler(*m_Machine, cb2); }
		}

		// TODO: ca and cb pulsing stuff.
		
		break;
	case IFR:
		// Interrupt flag bits are cleared by writing 1:s for corresponding bit.
		ifr &= (~a_Value) & 0x7f;
		if (ifr & ier) {
			ifr |= 0x80;	// bit 7=1 if any IRQ is set.
		}
		break;
	case IER:
		if (a_Value & 0x80) {
			ier |= (a_Value & 0x7f);	// if bit 7=1: set given bits.
		}
		else {
			ier &= ~(a_Value & 0x7f);	// if bit 7=0: clear given bits.
		}
		// TODO: check interrupt? 
		break;
	case IORA2:
		ora = a_Value;
		cout << "----- Write ORA: " << std::bitset<8>(ora) << endl;
		break;
	}
}


void MOS6522::IRQSet(uint8_t bits)
{
	ifr |= bits;
	if ((ifr & ier) & 0x7f) {
		ifr |= 0x80;
	}
	if (bits & ier) {
		m_Machine->GetCPU()->IRQ();
	}
}

void MOS6522::IRQClear(uint8_t bits)
{
	ifr &= ~bits;

	// Clear bit 7 if no (enabled) interrupts exist.
	if (!((ifr & ier) & 0x7f)) {
		ifr &= 0x7f;
	}
}


void MOS6522::WriteCA1(bool a_Value)
{
	if (ca1 != a_Value) {
		ca1 = a_Value;
		// Transitions only if enabled in PCR.
		if ((ca1 && (pcr & PCR_MASK_CA1)) || (!ca1 && !(pcr & PCR_MASK_CA1))) {
			IRQSet(IRQ_CA1);

			// Handshake mode, set ca2 on pos transition of ca1.
			if (!ca2 && (pcr & PCR_MASK_CA2) == 0x08) {
				ca2 = true;
				if (ca2_changed_handler) { ca2_changed_handler(*m_Machine, ca2); }
			}
		}
	}
}

void MOS6522::WriteCA2(bool a_Value)
{
	if (ca2 != a_Value) {
		ca2 = a_Value;

		// Set interrupt on pos/neg transition if 0 or 4 in pcr.
		if ((ca2 && ((pcr & 0x0C) == 0x04)) || (!ca2 && ((pcr & 0x0C) == 0x00))) {
			IRQSet(IRQ_CA2);
		}

		if (ca2_changed_handler) { ca2_changed_handler(*m_Machine, ca2); }
	}
}


void MOS6522::WriteCB1(bool a_Value)
{
	if (cb1 != a_Value) {
		cb1 = a_Value;
		// Transitions only if enabled in PCR.
		if ((cb1 && (pcr & PCR_MASK_CB1)) || (!cb1 && !(pcr & PCR_MASK_CB1))) {
			IRQSet(IRQ_CB1);

			// Handshake mode, set cb2 on pos transition of cb1.
			if (!cb2 && (pcr & PCR_MASK_CB2) == 0x80) {
				cb2 = true;
				if (cb2_changed_handler) { cb2_changed_handler(*m_Machine, cb2); }
			}
		}
	}
}

void MOS6522::WriteCB2(bool a_Value)
{
	if (cb2 != a_Value) {
		cb2 = a_Value;

		// Set interrupt on pos/neg transition if 0 or 40 in pcr.
		if ((cb2 && ((pcr & 0xC0) == 0x40)) || (!ca2 && ((pcr & 0xC0) == 0x00))) {
			IRQSet(IRQ_CB2);				if (cb2_changed_handler) { cb2_changed_handler(*m_Machine, cb2); }

		}

		if (cb2_changed_handler) { cb2_changed_handler(*m_Machine, cb2); }
	}
}

void MOS6522::PrintStat()
{
	std::cout << "VIA stats:" << std::endl;
	std::cout << "  -   ORA: " << (int)ora << std::endl;
	std::cout << "  -  DDRA: " << (int)ddra << std::endl;
	std::cout << "  -   ORB: " << (int)orb << std::endl;
	std::cout << "  -  DDRB: " << (int)ddrb << std::endl;
	std::cout << "  - T1C_L: " << (int)(t1_counter & 0x00ff) << std::endl;
	std::cout << "  - T1C_H: " << (int)(t1_counter >> 8) << std::endl;
	std::cout << "  - T1L_L: " << (int)t1_latch_low << std::endl;
	std::cout << "  - T1L_H: " << (int)t1_latch_high << std::endl;
	std::cout << "  - T2C_L: " << (int)(t2_counter & 0x00ff) << std::endl;
	std::cout << "  - T2C_H: " << (int)(t2_counter >> 8) << std::endl;
	std::cout << "  -    SR: " << (int)sr << std::endl;
	std::cout << "  -   ACR: " << (int)acr << std::endl;
	std::cout << "  -   PCR: " << (int)pcr << std::endl;
	std::cout << "  -   IFR: " << (int)ifr << std::endl;
	std::cout << "  -   IER: " << (int)(ier | 0x80) << std::endl;
	std::cout << "  - IORA2: " << (int)ora << std::endl;
}
