// Copyright (C) 2009-2016 by Anders Piniesjö <pugo@pugo.org>

#include <iostream>

#include "mos6522.hpp"
#include "machine.hpp"
#include "memory.hpp"

#include <boost/assign.hpp>
#include <boost/tuple/tuple.hpp>
#include <vector>
#include <stack>
#include <map>
#include <string>
#include <utility>

using namespace std;


MOS6522::MOS6522(std::shared_ptr<Machine> a_Machine) :
	m_Machine(a_Machine)
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

short MOS6522::Exec(uint8_t a_Cycles)
{
	t1_counter -= a_Cycles;
	
	if (t1_counter < 0) {
		switch (acr & 0xc0)
		{
			case 0x00:
			case 0x80:
				// One shot
				if (t1_run) {
					std::cout << "T1 interrupt one shot" << std::endl;
					IRQSet(IRQ_T1);
					t1_run = false;
				}
				t1_counter &= 0xffff;
				break;
			case 0x40:
			case 0xC0:
				// Continuous
				if (t1_run) {
					std::cout << "T1 interrupt cont" << std::endl;
					IRQSet(IRQ_T1);
				}
				t1_counter += ((t1_latch_high << 8) | t1_latch_low) + 2; // +2: compensate for boundary time and load time.
				break;
		}
	}

	t2_counter -= a_Cycles;
	if (t2_counter < 0) {
		if (t2_run) {
			std::cout << "T2 interrupt" << std::endl;
			IRQSet(IRQ_T2);
		}
		t2_counter &= 0xffff;
	}

}

uint8_t MOS6522::ReadByte(uint16_t a_Offset)
{
	switch(a_Offset & 0x000f)
	{
	case ORB:
// 	cout << "Read " << m_RegisterNames[static_cast<Register>(a_Offset & 0x000f)] << endl;
		IRQClear(IRQ_CB1);
		if ((pcr & PCR_CONTROL_CB2) == 0x00 || (pcr & PCR_CONTROL_CB2) == 0x40) {
			IRQClear(IRQ_CB2);
		}
		return (orb & ddrb) | (irb & ~ddrb);
	case ORA:
// 	cout << "Read " << m_RegisterNames[static_cast<Register>(a_Offset & 0x000f)] << endl;
		IRQClear(IRQ_CA1);
		if ((pcr & PCR_CONTROL_CA2) == 0x00 || (pcr & PCR_CONTROL_CA2) == 0x04) {
			IRQClear(IRQ_CA2);
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
		if ((pcr & PCR_CONTROL_CB2) == 0x00 || (pcr & PCR_CONTROL_CB2) == 0x40) {
			IRQClear(IRQ_CB2);
		}
		break;
	case ORA:
// 	cout << "Read " << m_RegisterNames[static_cast<Register>(a_Offset & 0x000f)] << endl;
		ora = a_Value;
		IRQClear(IRQ_CA1);
		if ((pcr & PCR_CONTROL_CA2) == 0x00 || (pcr & PCR_CONTROL_CA2) == 0x04) {
			IRQClear(IRQ_CA2);
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
	if (!ca1 && a_Value) {
		// Positive transition only of enabled in PCR.
		if (pcr & PCR_CONTROL_CA1) {
			IRQSet(IRQ_CA1);
		}
	}
	else if (ca1 && ! a_Value) {
		// Negative transition only of enabled in PCR.
		if (!(pcr & PCR_CONTROL_CA1)) {
			IRQSet(IRQ_CA1);
		}
	}

	ca1 = a_Value;
}

void MOS6522::WriteCA2(bool a_Value)
{
	if (!ca2 && a_Value) {
		// Positive transition only of enabled in PCR.
		if ((pcr & PCR_CONTROL_CA2) == 0x04 || (pcr & PCR_CONTROL_CA2) == 0x06) {
			IRQSet(IRQ_CA2);
		}
		ca2 = a_Value;
	}
	else if (ca2 && ! a_Value) {
		// Negative transition only of enabled in PCR.
		if ((pcr & PCR_CONTROL_CA2) == 0x00 || (pcr & PCR_CONTROL_CA2) == 0x02) {
			IRQSet(IRQ_CA2);
		}
		ca2 = a_Value;
	}
}


void MOS6522::WriteCB1(bool a_Value)
{
	if (!cb1 && a_Value) {
		// Positive transition only of enabled in PCR.
		if (pcr & PCR_CONTROL_CB1) {
			IRQSet(IRQ_CB1);
		}
	}
	else if (cb1 && ! a_Value) {
		// Negative transition only of enabled in PCR.
		if (!(pcr & PCR_CONTROL_CB1)) {
			IRQSet(IRQ_CB1);
		}
	}

	cb1 = a_Value;
}

void MOS6522::WriteCB2(bool a_Value)
{
	if (!cb2 && a_Value) {
		// Positive transition only of enabled in PCR.
		if ((pcr & PCR_CONTROL_CB2) == 0x40 || (pcr & PCR_CONTROL_CB2) == 0x60) {
			IRQSet(IRQ_CB2);
		}
		cb2 = a_Value;
	}
	else if (cb2 && ! a_Value) {
		// Negative transition only of enabled in PCR.
		if ((pcr & PCR_CONTROL_CB2) == 0x00 || (pcr & PCR_CONTROL_CB2) == 0x20) {
			IRQSet(IRQ_CB2);
		}
		cb2 = a_Value;
	}
}

void MOS6522::PrintStat()
{
	std::cout << "VIA stats:" << std::endl;
	std::cout << "  -   ORB: " << (int)orb << std::endl;
	std::cout << "  -   ORA: " << (int)ora << std::endl;
	std::cout << "  -  DDRB: " << (int)ddrb << std::endl;
	std::cout << "  -  DDRA: " << (int)ddra << std::endl;
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
