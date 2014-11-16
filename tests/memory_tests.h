#include <cxxtest/TestSuite.h>
#include <iostream>
#include <cstdlib>

#include "../memory.h"
#include "../datatypes.h"

class MemoryTestSuite : public CxxTest::TestSuite 
{
public:
	void setUp()
	{
	}
	
	void tearDown()
	{
		
	}

	void testMemoryCreateSize( void )
	{
		int size = 65536;
		Memory memory( size );
		TS_ASSERT_EQUALS( memory.getSize(), size );
	}

	void testMemoryCreateEmpty( void )
	{
		int size = 65536;
		int max = 0;
		Memory memory( size );
		for( int i = 0; i < size; i++ )
			if ( memory.mem[i] > max )
				max = memory.mem[i];
		TS_ASSERT_EQUALS( max, 0 );
	}

	void testMemoryLoad( void )
	{
		// Test ROM file contains 256 integers from 0-255 with total sum 32640
		Memory memory( 65536 );
		memory.load("ROMS/test_0_to_255.rom", 0xc000);
		int sum = 0;
		for( int i=0; i < 256; i++ )
			sum += memory.mem[0xc000+i];
		TS_ASSERT_EQUALS( sum, 32640 );
	}
};
