
#include <iostream>

#include "mos6522.h"


using namespace std;


MOS6522::MOS6522(Machine* machine, Memory* memory) : machine(machine), memory(memory)
{
	reset();
}

MOS6522::~MOS6522()
{
}

void MOS6522::reset()
{
	t1_run = false;
	t2_run = false;
}

short MOS6522::exec()
{
}


byte MOS6522::readByte(word offset)
{
	switch(offset & 0x000f)
	{
	case VIA_ORB:
		cout << "Read VIA_ORB" << endl;
		// TODO: clear interrupt
		return orb;
	case VIA_ORA:
		cout << "Read VIA_ORA" << endl;
		// TODO: clear interrupt
		return ora;
	case VIA_DDRB:
		cout << "Read VIA_DDRB" << endl;
		return ddrb;
	case VIA_DDRA:
		cout << "Read VIA_DDRA" << endl;
		return ddra;
	case VIA_T1C_L:
		cout << "Read VIA_T1C_L" << endl;
		// TODO: reset interrupt;
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
		// TODO: reset interrupt;
		return t2_counter & 0x00ff;
	case VIA_T2C_H:
		cout << "Read VIA_T2C_H" << endl;
		return t2_counter >> 8;
	case VIA_SR:
		cout << "Read VIA_SR" << endl;
		// TODO: clear interrupt
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

void MOS6522::writeByte(word offset, byte value)
{
	switch(offset & 0x000f)
	{
	case VIA_ORB:
		cout << "Write VIA_ORB";
		orb = value;
		// TODO: clear IRQ things.
		break;
	case VIA_ORA:
		cout << "Write VIA_ORA";
		ora = value;
		// TODO: clear IRQ things.
		break;
	case VIA_DDRB:
		cout << "Write VIA_DDRB";
		ddrb = value;
		break;
	case VIA_DDRA:
		cout << "Write VIA_DDRA";
		ddra = value;
		break;
	case VIA_T1C_L:
		cout << "Write VIA_T1C_L";
		t1_latch_low = value;
		break;
	case VIA_T1C_H:
		cout << "Write VIA_T1C_H";
		t1_latch_high = value;
		t1_counter = (t1_latch_high << 8)  < t1_latch_low;
		t1_run = true;
		// reload? Interrupt stuff
		// om översta biten i acr: släck översta i orb.
		break;
	case VIA_T1L_L:
		cout << "Write VIA_T1L_L";
		t1_latch_low = value;
		break;
	case VIA_T1L_H:
		// TODO: reset interrupt flag;
		cout << "Write VIA_T1L_H";
		t1_latch_high = value;
		break;
	case VIA_T2C_L:
		cout << "Write VIA_T2C_L";
		t2_latch_low = value;
		break;
	case VIA_T2C_H:
		cout << "Write VIA_T2C_H";
		t2_latch_high = value;
		t2_counter = (t2_latch_high << 8) | t2_latch_low;
		t2_run = true;
		// reload? 
		break;
	case VIA_SR:
		cout << "Write VIA_SR";
		sr = value;
		break;
	case VIA_ACR:
		cout << "Write VIA_ACR";
		acr = value;
		break;
	case VIA_PCR:
		cout << "Write VIA_PCR";
		pcr = value;
		// TODO: ca and cb pulsing stuff.
		break;
	case VIA_IFR:
		cout << "Write VIA_IFR";
		ifr = value;
		break;
	case VIA_IER:
		cout << "Write VIA_IER";
		if( value & VIA_IER_WRITE )
			ier |= (value & 0x7f);	// if bit 7: turn on given bits.
		else
			ier &= ~(value & 0x7f);	// if !bit 7: turn off given bits.
		// TODO: check interrupt? 
		break;
	case VIA_IORA2:
		cout << "Write VIA_IORA2";
		ora = value;
		break;
	}
	
	cout << ": " << hex << (int) value << endl;
}