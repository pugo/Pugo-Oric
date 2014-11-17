#include <cxxtest/TestSuite.h>
#include <iostream>
#include <cstdlib>

#include "../mos6522.h"
#include "../datatypes.h"

class MOS6522TestSuite : public CxxTest::TestSuite 
{
public:
	Machine* machine;
	MOS6522* mos6522;

	void setUp()
	{
		machine = new Machine();
		machine->getCPU().setPC(0);
		machine->getCPU().setQuiet(true);
		mos6522 = &machine->getVIA();
	}
	
	void tearDown()
	{
		delete machine;
	}

// #define VIA_ORB   0x00		// Output register B
// #define VIA_ORA   0x01		// Output register A
// #define VIA_DDRB  0x02		// Data direction port B
// #define VIA_DDRA  0x03		// Data direction port A
// #define VIA_T1C_L 0x04		// Write: into T1 low latch.   Read: T1 low counter, reset interrupt flag.
// #define VIA_T1C_H 0x05		// Write: into T1 high latch+counter, transfer low.  Read: T1 high counter.
// #define VIA_T1L_L 0x06		// Write: into T1 low latch.   Read: T1 low latch.
// #define VIA_T1L_H 0x07		// Write: into T1 high latch.  Read: T2 high latch.
// #define VIA_T2C_L 0x08		// Write: into T2 low latch.   Read: T2 low counter, reset interrupt flag.
// #define VIA_T2C_H 0x09		// Write: into T2 high latch+counter, transfer low.  Read: T2 high counter.
// #define VIA_SR    0x0a		// Shift Register
// #define VIA_ACR   0x0b		// Auxilliary Control Register
// #define VIA_PCR   0x0c		// Peripheral Control Register
// #define VIA_IFR   0x0d		// Interrupt Flag Register
// #define VIA_IER   0x0e		// Interrupt Enable Register
// #define VIA_IORA2 0x0f

	void testWriteORB( void )
	{
		mos6522->writeByte(VIA_ORB, 0xff);
		TS_ASSERT_EQUALS(mos6522->readByte(VIA_ORB), 0xff);
	}

	void testWriteORA( void )
	{
		mos6522->writeByte(VIA_ORA, 0xff);
		TS_ASSERT_EQUALS(mos6522->readByte(VIA_ORA), 0xff);
	}

	void testWriteDDRB( void )
	{
		mos6522->writeByte(VIA_DDRB, 0xff);
		TS_ASSERT_EQUALS(mos6522->readByte(VIA_DDRB), 0xff);
	}

	void testWriteDDRA( void )
	{
		mos6522->writeByte(VIA_DDRA, 0xff);
		TS_ASSERT_EQUALS(mos6522->readByte(VIA_DDRA), 0xff);
	}

	void testWriteT1C( void )
	{
		mos6522->writeByte(VIA_T1C_L, 0xee);
		TS_ASSERT_EQUALS(mos6522->readByte(VIA_T1C_L), 0x00);
		TS_ASSERT_EQUALS(mos6522->readByte(VIA_T1C_H), 0x00);
		TS_ASSERT_EQUALS(mos6522->readByte(VIA_T1L_L), 0xee);
		TS_ASSERT_EQUALS(mos6522->readByte(VIA_T1L_H), 0x00);

		mos6522->writeByte(VIA_T1C_H, 0xff);
		TS_ASSERT_EQUALS(mos6522->readByte(VIA_T1C_L), 0xee);
		TS_ASSERT_EQUALS(mos6522->readByte(VIA_T1C_H), 0xff);
		TS_ASSERT_EQUALS(mos6522->readByte(VIA_T1L_L), 0xee);
		TS_ASSERT_EQUALS(mos6522->readByte(VIA_T1L_H), 0xff);
	}

	void testWriteT1L( void )
	{
		mos6522->writeByte(VIA_T1L_H, 0xdd);
		mos6522->writeByte(VIA_T1L_L, 0xcc);
		TS_ASSERT_EQUALS(mos6522->readByte(VIA_T1L_H), 0xdd);
		TS_ASSERT_EQUALS(mos6522->readByte(VIA_T1L_L), 0xcc);
	}

	void testWriteT2C( void )
	{
		mos6522->writeByte(VIA_T2C_L, 0xee);
		TS_ASSERT_EQUALS(mos6522->readByte(VIA_T2C_L), 0x00);
		TS_ASSERT_EQUALS(mos6522->readByte(VIA_T2C_H), 0x00);

		mos6522->writeByte(VIA_T2C_H, 0xff);
		TS_ASSERT_EQUALS(mos6522->readByte(VIA_T2C_L), 0xee);
		TS_ASSERT_EQUALS(mos6522->readByte(VIA_T2C_H), 0xff);
	}

	void testWriteSR( void )
	{
		mos6522->writeByte(VIA_SR, 0xff);
		TS_ASSERT_EQUALS(mos6522->readByte(VIA_SR), 0xff);
	}

	void testWriteACR( void )
	{
		mos6522->writeByte(VIA_ACR, 0xff);
		TS_ASSERT_EQUALS(mos6522->readByte(VIA_ACR), 0xff);
	}

	void testWritePCR( void )
	{
		mos6522->writeByte(VIA_PCR, 0xff);
		TS_ASSERT_EQUALS(mos6522->readByte(VIA_PCR), 0xff);
	}

	void testWriteIFR( void )
	{
		TS_ASSERT_EQUALS(mos6522->readByte(VIA_IFR), 0x00);		// Initial IFR empty.
		mos6522->writeByte(VIA_IFR, 0xff);
		TS_ASSERT_EQUALS(mos6522->readByte(VIA_IFR), 0xff);
	}

	void testWriteIER( void )
	{
		TS_ASSERT_EQUALS(mos6522->readByte(VIA_IER), 0x80);		// Initial IER em
		for (int i = 0; i < 7; i++)
		{
			mos6522->writeByte(VIA_IER, 0x7f);
			TS_ASSERT_EQUALS(mos6522->readByte(VIA_IER), 0x80);

			byte b = 1 << i;
			mos6522->writeByte(VIA_IER, 0x80 | b);
			TS_ASSERT_EQUALS(mos6522->readByte(VIA_IER), 0x80 | b);
			mos6522->writeByte(VIA_IER, b);
			TS_ASSERT_EQUALS(mos6522->readByte(VIA_IER), 0x80);
			
		}
	}

	void testWriteIORA2( void )
	{
		mos6522->writeByte(VIA_IORA2, 0xff);
		TS_ASSERT_EQUALS(mos6522->readByte(VIA_IORA2), 0xff);
	}


};
