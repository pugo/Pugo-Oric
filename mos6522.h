
#ifndef MOS6522_H
#define MOS6522_H

#include "stdio.h"
#include "datatypes.h"

class Oric;
class Memory;


// RS3, RS2, RS1, RS0 values (from address bus), addressing VIA registers.
#define VIA_ORB   0x00		// Output register B
#define VIA_ORA   0x01		// Output register A
#define VIA_DDRB  0x02		// Data direction port B
#define VIA_DDRA  0x03		// Data direction port A
#define VIA_T1C_L 0x04		// Write: into T1 low latch.   Read: T1 low counter, reset interrupt flag.
#define VIA_T1C_H 0x05		// Write: into T1 high latch+counter, transfer low.  Read: T1 high counter.
#define VIA_T1L_L 0x06		// Write: into T1 low latch.   Read: T1 low latch.
#define VIA_T1L_H 0x07		// Write: into T1 high latch.  Read: T2 high latch.
#define VIA_T2C_L 0x08		// Write: into T2 low latch.   Read: T2 low counter, reset interrupt flag.
#define VIA_T2C_H 0x09		// Write: into T2 high latch+counter, transfer low.  Read: T2 high counter.
#define VIA_SR    0x0a
#define VIA_ACR   0x0b
#define VIA_PCR   0x0c
#define VIA_IFR   0x0d
#define VIA_IER   0x0e
#define VIA_IORA2 0x0f



/**
	@author Anders Piniesjö <pugo@pugo.org>
*/
class MOS6522
{
public:
	MOS6522(Oric* oric, Memory* memory);
	~MOS6522();

	void reset();
	short exec();
	
	byte readByte(word offset);
	void writeByte(word offset, byte value);


private:
	byte orb;
	byte ora;

	// Data direction for port A and B. Input = 0, output = 1.
	byte ddrb;
	byte ddra;

	byte t1_latch_low;
	byte t1_latch_high;
	word t1_counter;
	bool t1_run;

	byte t2_latch_low;
	byte t2_latch_high;
	word t2_counter;
	bool t2_run;

	byte sr;
	byte acr;
	byte pcr;
	byte ifr;
	byte ier;

	byte ira;
	byte iral;

	byte irb;
	byte irbl;
	
	
	Oric* oric;
	Memory* memory;
};

#endif // MOS6502_H
