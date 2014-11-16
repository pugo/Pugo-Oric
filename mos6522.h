
#ifndef MOS6522_H
#define MOS6522_H

#include "stdio.h"
#include "datatypes.h"

class Machine;
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
#define VIA_SR    0x0a		// Shift Register
#define VIA_ACR   0x0b		// Auxilliary Control Register
#define VIA_PCR   0x0c		// Peripheral Control Register
#define VIA_IFR   0x0d		// Interrupt Flag Register
#define VIA_IER   0x0e		// Interrupt Enable Register
#define VIA_IORA2 0x0f


#define VIA_IER_WRITE 0x80	// Bit 7=1: enable IER bits according to data, bit 7=0 disables.


/**
	@author Anders Piniesjö <pugo@pugo.org>
*/
class MOS6522
{
public:
	MOS6522(Machine* machine, Memory* memory);
	~MOS6522();

	void reset();
	short exec();
	
	byte readByte(word offset);
	void writeByte(word offset, byte value);


private:
	byte orb;		// Output Register B
	byte ora;		// Output Register A

	byte ddrb;		// Data Direction Register B (input = 0, output = 1)
	byte ddra;		// Data Direction Register A (input = 0, output = 1)

	byte t1_latch_low;
	byte t1_latch_high;
	word t1_counter;
	bool t1_run;

	byte t2_latch_low;
	byte t2_latch_high;
	word t2_counter;
	bool t2_run;

	byte sr;		// Shift register

	byte acr;		// Auxilliary control register (shift mode, etc)
					// |  7  |  6  |  5  |     4    |  3  |  2  |  1  |     0    | 
					// |    CB2 ctrl     | CB1 ctrl |     CA2 ctrl    | CA1 ctrl | 

	byte pcr;		// Peripheral control register
					// |  7  |  6  |    5    |  4  |  3  |  2  |      1       |      0       | 
					// |  T1 ctrl  | T2 ctrl |     SR  ctrl    | PB latch en. | PA latch en. |


	byte ifr;		// Interrupt Flag Register:   | IRQ  | T1 | T2 | CB1 | CB2 | SR | CA1 | CA2 |
	byte ier;		// Interrupt Enable Register: | ctrl | T1 | T2 | CB1 | CB2 | SR | CA1 | CA2 |

	byte ira;		// Input Register A
	byte iral;

	byte irb;		// Input Register B
	byte irbl;
	
	
	Machine* machine;
	Memory* memory;
};

#endif // MOS6502_H
