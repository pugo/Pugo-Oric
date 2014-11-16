#include <cxxtest/TestSuite.h>
#include <iostream>
#include <cstdlib>

#include "../machine.h"
#include "../mos6502.h"
#include "../memory.h"
#include "../datatypes.h"




int decToBCD(int dec)
{
	int major = dec / 10;
	int minor = dec - major*10;
	//std::cout << "D2BCD - major: " << major << ", minor: " << minor << ", result: " 
	//          << std::hex << major * 16 + minor << std::endl;
	return major * 16 + minor;
}

int bcdToDec(int bcd)
{
	int major = bcd / 16;
	int minor = bcd - major*16;
	//std::cout << "BCD2D - major: " << major << ", minor: " << minor << ", result: " 
	//          << std::hex << major * 16 + minor << std::endl;
	return major * 10 + minor;
}


	//   7                           0
	// +---+---+---+---+---+---+---+---+
	// | N | V |   | B | D | I | Z | C |  <-- flag, 0/1 = reset/set
	// +---+---+---+---+---+---+---+---+


class MOS6502TestSuite : public CxxTest::TestSuite 
{
public:
	Machine* machine;

	void setUp()
	{
		machine = new Machine();
		machine->getCPU().setPC(0);
		machine->getCPU().setQuiet(true);
	}
	
	void tearDown()
	{
		delete machine;
	}

	void testCPUCreate(void)
	{
		TS_ASSERT_EQUALS(machine->getCPU().getSP(), 0);
		byte registers = machine->getCPU().getP();
		TS_ASSERT_EQUALS(registers & 0x80, 0);	// N
		TS_ASSERT_EQUALS(registers & 0x40, 0);	// V
		TS_ASSERT_EQUALS(registers & 0x10, 0);	// B
		TS_ASSERT_EQUALS(registers & 0x08, 0);	// D
		TS_ASSERT_EQUALS(registers & 0x04, 0);	// I
		TS_ASSERT_DIFFERS(registers & 0x02, 0);	// Z
		TS_ASSERT_EQUALS(registers & 0x01, 0);	// C
		TS_ASSERT_EQUALS(machine->getCPU().getPC(), 0);
	}

	// LDA
	void testOpLDA_IMM(void)
	{
		Memory& memory = machine->getMemory();
		memory << LDA_IMM;
		memory << 0x1f;
		memory << BRK;

		machine->run(0);
		TS_ASSERT_EQUALS(machine->getCPU().A, 0x1f);
	}

	void testOpLDA_ZP(void)
	{
		Memory& memory = machine->getMemory();
		memory.mem[0x10] = 0x2f;

		memory << LDA_ZP;
		memory << 0x10;
		memory << BRK;

		machine->run(0);
		TS_ASSERT_EQUALS(machine->getCPU().A, 0x2f);
	}

	void testOpLDA_ZP_X(void)
	{
		Memory& memory = machine->getMemory();
		memory.mem[0x15] = 0x3f;
		machine->getCPU().X = 0x05;

		memory << LDA_ZP_X;
		memory << 0x10;
		memory << BRK;

		machine->run(0);
		TS_ASSERT_EQUALS(machine->getCPU().A, 0x3f);
	}

	void testOpLDA_ABS(void)
	{
		Memory& memory = machine->getMemory();
		memory.mem[0x1234] = 0x4f;

		memory << LDA_ABS;
		memory << 0x34 << 0x12;
		memory << BRK;

		machine->run(0);
		TS_ASSERT_EQUALS(machine->getCPU().A, 0x4f);
	}

	void testOpLDA_ABS_X(void)
	{
		Memory& memory = machine->getMemory();
		memory.mem[0x1122] = 0x5f;
		machine->getCPU().X = 0x11;

		memory << LDA_ABS_X;
		memory << 0x11 << 0x11;
		memory << BRK;

		machine->run(0);
		TS_ASSERT_EQUALS(machine->getCPU().A, 0x5f);
	}

	void testOpLDA_ABS_Y(void)
	{
		Memory& memory = machine->getMemory();
		memory.mem[0x2233] = 0x6f;
		machine->getCPU().X = 0x11;

		memory << LDA_ABS_X;
		memory << 0x22 << 0x22;
		memory << BRK;

		machine->run(0);
		TS_ASSERT_EQUALS(machine->getCPU().A, 0x6f);
	}


	void testOpLDA_IND_X(void)
	{
		Memory& memory = machine->getMemory();
		memory.mem[0x4711] = 0x7f;
		memory.mem[0x14] = 0x11;
		memory.mem[0x15] = 0x47;
		machine->getCPU().X = 0x04;

		memory << LDA_IND_X;
		memory << 0x10;
		memory << BRK;

		machine->run(0);
		TS_ASSERT_EQUALS(machine->getCPU().X, 0x04);
		TS_ASSERT_EQUALS(machine->getCPU().A, 0x7f);
	}

void testOpLDA_IND_Y(void)
	{
		Memory& memory = machine->getMemory();
		memory.mem[0x4711] = 0x8f;
		memory.mem[0x10] = 0x00;
		memory.mem[0x11] = 0x47;
		machine->getCPU().Y = 0x11;

		memory << LDA_IND_Y;
		memory << 0x10;
		memory << BRK;

		machine->run(0);
		TS_ASSERT_EQUALS(machine->getCPU().Y, 0x11);
		TS_ASSERT_EQUALS(machine->getCPU().A, 0x8f);
	}


	// LDX
	void testOpLDX_IMM(void)
	{
		Memory& memory = machine->getMemory();

		memory << LDX_IMM;
		memory << 0x1f;
		memory << BRK;

		machine->run(0);
		TS_ASSERT_EQUALS(machine->getCPU().X, 0x1f);
	}

	void testOpLDX_ZP(void)
	{
		Memory& memory = machine->getMemory();
		memory.mem[0x10] = 0x2f;

		memory << LDX_ZP;
		memory << 0x10;
		memory << BRK;

		machine->run(0);
		TS_ASSERT_EQUALS(machine->getCPU().X, 0x2f);
	}

	void testOpLDX_ZP_Y(void)
	{
		Memory& memory = machine->getMemory();
		memory.mem[0x15] = 0x3f;
		machine->getCPU().Y = 0x05;

		memory << LDX_ZP_Y;
		memory << 0x10;
		memory << BRK;

		machine->run(0);
		TS_ASSERT_EQUALS(machine->getCPU().X, 0x3f);
	}

	void testOpLDX_ABS(void)
	{
		Memory& memory = machine->getMemory();
		memory.mem[0x1234] = 0x4f;

		memory << LDX_ABS;
		memory << 0x34 << 0x12;
		memory << BRK;

		machine->run(0);
		TS_ASSERT_EQUALS(machine->getCPU().X, 0x4f);
	}

	void testOpLDX_ABS_Y(void)
	{
		Memory& memory = machine->getMemory();
		memory.mem[0x1122] = 0x5f;
		machine->getCPU().Y = 0x11;

		memory << LDX_ABS_Y;
		memory << 0x11 << 0x11;
		memory << BRK;

		machine->run(0);
		TS_ASSERT_EQUALS(machine->getCPU().X, 0x5f);
	}


	// LDY
	void testOpLDY_IMM(void)
	{
		Memory& memory = machine->getMemory();

		memory << LDY_IMM;
		memory << 0x1f;
		memory << BRK;

		machine->run(0);
		TS_ASSERT_EQUALS(machine->getCPU().Y, 0x1f);
	}

	void testOpLDY_ZP(void)
	{
		Memory& memory = machine->getMemory();
		memory.mem[0x10] = 0x2f;

		memory << LDY_ZP;
		memory << 0x10;
		memory << BRK;

		machine->run(0);
		TS_ASSERT_EQUALS(machine->getCPU().Y, 0x2f);
	}

	void testOpLDY_ZP_X(void)
	{
		Memory& memory = machine->getMemory();
		memory.mem[0x15] = 0x3f;
		machine->getCPU().X = 0x05;

		memory << LDY_ZP_X;
		memory << 0x10;
		memory << BRK;

		machine->run(0);
		TS_ASSERT_EQUALS(machine->getCPU().Y, 0x3f);
	}

	void testOpLDY_ABS(void)
	{
		Memory& memory = machine->getMemory();
		memory.mem[0x1234] = 0x4f;

		memory << LDY_ABS;
		memory << 0x34 << 0x12;
		memory << BRK;

		machine->run(0);
		TS_ASSERT_EQUALS(machine->getCPU().Y, 0x4f);
	}

	void testOpLDY_ABS_X(void)
	{
		Memory& memory = machine->getMemory();
		memory.mem[0x1122] = 0x5f;
		machine->getCPU().X = 0x11;

		memory << LDY_ABS_X;
		memory << 0x11 << 0x11;
		memory << BRK;

		machine->run(0);
		TS_ASSERT_EQUALS(machine->getCPU().Y, 0x5f);
	}



	// STA
	void testOpSTA_ZP(void)
	{
		Memory& memory = machine->getMemory();
		machine->getCPU().A = 0x1f;

		memory << STA_ZP;
		memory << 0x10;
		memory << BRK;

		machine->run(0);
		TS_ASSERT_EQUALS(memory.mem[0x10], 0x1f);
	}

	void testOpSTA_ZP_X(void)
	{
		Memory& memory = machine->getMemory();
		machine->getCPU().A = 0x2f;
		machine->getCPU().X = 0x05;

		memory << STA_ZP_X;
		memory << 0x10;
		memory << BRK;

		machine->run(0);
		TS_ASSERT_EQUALS(memory.mem[0x15], 0x2f);
	}

	void testOpSTA_ABS(void)
	{
		Memory& memory = machine->getMemory();
		machine->getCPU().A = 0x3f;

		memory << STA_ABS;
		memory << 0x34 << 0x12;
		memory << BRK;

		machine->run(0);
		TS_ASSERT_EQUALS(memory.mem[0x1234], 0x3f);
	}

	void testOpSTA_ABS_X(void)
	{
		Memory& memory = machine->getMemory();
		machine->getCPU().A = 0x4f;
		machine->getCPU().X = 0x11;

		memory << STA_ABS_X;
		memory << 0x11 << 0x11;
		memory << BRK;

		machine->run(0);
		TS_ASSERT_EQUALS(memory.mem[0x1122], 0x4f);
	}

	void testOpSTA_ABS_Y(void)
	{
		Memory& memory = machine->getMemory();
		machine->getCPU().A = 0x5f;
		machine->getCPU().X = 0x11;

		memory << STA_ABS_X;
		memory << 0x22 << 0x22;
		memory << BRK;

		machine->run(0);
		TS_ASSERT_EQUALS(memory.mem[0x2233], 0x5f);
	}


	void testOpSTA_IND_X(void)
	{
		Memory& memory = machine->getMemory();
		memory.mem[0x14] = 0x11;
		memory.mem[0x15] = 0x47;
		machine->getCPU().A = 0x6f;
		machine->getCPU().X = 0x04;

		memory << STA_IND_X;
		memory << 0x10;
		memory << BRK;

		machine->run(0);
		TS_ASSERT_EQUALS(machine->getCPU().X, 0x04);
		TS_ASSERT_EQUALS(memory.mem[0x4711], 0x6f);
	}

	void testOpSTA_IND_Y(void)
	{
		Memory& memory = machine->getMemory();
		memory.mem[0x10] = 0x00;
		memory.mem[0x11] = 0x47;
		machine->getCPU().A = 0x7f;
		machine->getCPU().Y = 0x11;

		memory << STA_IND_Y;
		memory << 0x10;
		memory << BRK;

		machine->run(0);
		TS_ASSERT_EQUALS(machine->getCPU().Y, 0x11);
		TS_ASSERT_EQUALS(memory.mem[0x4711], 0x7f);
	}


	// STX
	void testOpSTX_ZP(void)
	{
		Memory& memory = machine->getMemory();
		machine->getCPU().X = 0x1f;

		memory << STX_ZP;
		memory << 0x10;
		memory << BRK;

		machine->run(0);
		TS_ASSERT_EQUALS(memory.mem[0x10], 0x1f);
	}

	void testOpSTX_ZP_Y(void)
	{
		Memory& memory = machine->getMemory();
		machine->getCPU().X = 0x2f;
		machine->getCPU().Y = 0x05;

		memory << STX_ZP_Y;
		memory << 0x10;
		memory << BRK;

		machine->run(0);
		TS_ASSERT_EQUALS(memory.mem[0x15], 0x2f);
	}

	void testOpSTX_ABS(void)
	{
		Memory& memory = machine->getMemory();
		machine->getCPU().X = 0x3f;

		memory << STX_ABS;
		memory << 0x34 << 0x12;
		memory << BRK;

		machine->run(0);
		TS_ASSERT_EQUALS(memory.mem[0x1234], 0x3f);
	}


	// STY
	void testOpSTY_ZP(void)
	{
		Memory& memory = machine->getMemory();
		machine->getCPU().Y = 0x1f;

		memory << STY_ZP;
		memory << 0x10;
		memory << BRK;

		machine->run(0);
		TS_ASSERT_EQUALS(memory.mem[0x10], 0x1f);
	}

	void testOpSTY_ZP_X(void)
	{
		Memory& memory = machine->getMemory();
		machine->getCPU().Y = 0x2f;
		machine->getCPU().X = 0x05;

		memory << STY_ZP_X;
		memory << 0x10;
		memory << BRK;

		machine->run(0);
		TS_ASSERT_EQUALS(memory.mem[0x15], 0x2f);
	}

	void testOpSTY_ABS(void)
	{
		Memory& memory = machine->getMemory();
		machine->getCPU().Y = 0x3f;

		memory << STY_ABS;
		memory << 0x34 << 0x12;
		memory << BRK;

		machine->run(0);
		TS_ASSERT_EQUALS(memory.mem[0x1234], 0x3f);
	}


	// ADC
	void testOpADC_IMM(void)
	{
		// Normal mode
		for (int i=0; i<12; i++)
		{
			Memory& memory = machine->getMemory();
			machine->getCPU().D = false;
			machine->getCPU().A = 0x11;

			memory.setMemPos(0);
			memory << CLC;
			memory << ADC_IMM;
			memory << (0x13 * i);
			memory << BRK;

			machine->run(0);
			TS_ASSERT_EQUALS(machine->getCPU().A, 0x11 + 0x13 * i);
		}
	}

	void testOpADC_IMM_DEC(void)
	{
		// Decimal mode
		short a[] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88};
		short b[] = {0x88, 0x77, 0x66, 0x55, 0x44, 0x33, 0x22, 0x11};
		short v[] = {0,    1,    1,    1,    1,    1,    1,    0 };

		for (int i=0; i < 8; i++)
		{ 
			Memory& memory = machine->getMemory();
			machine->getCPU().D = true;
			machine->getCPU().A = a[i];
			
			memory.setMemPos(0);
			memory << CLC;
			memory << ADC_IMM;
			memory << b[i];
			memory << BRK;
			
			machine->run(0);
			TS_ASSERT_EQUALS(machine->getCPU().A, 0x99);
			TS_ASSERT_EQUALS(machine->getCPU().V, v[i]);
		}
	}

	void testOpADC_IMM_DEC2(void)
	{
		for (int a=0; a <= 99; a++)
		{
			for (int b=0; b <= 99; b++)
			{ 
				Memory& memory = machine->getMemory();
				machine->getCPU().D = true;
				machine->getCPU().A = decToBCD(a);
				
				memory.setMemPos(0);
				memory << CLC;
				memory << ADC_IMM;
				memory << decToBCD(b);
				memory << BRK;
				
				machine->run(0);
				TS_ASSERT_EQUALS(bcdToDec(machine->getCPU().A), (a+b) % 100);
			}
		}
	}

	void testOpADC_ZP(void)
	{
		// Normal mode
		for (int i=0; i<12; i++)
		{
			Memory& memory = machine->getMemory();
			machine->getCPU().D = false;
			machine->getCPU().A = 0x11;
			memory.mem[0x15] = 0x13 * i;
			
			memory.setMemPos(0);
			memory << CLC;
			memory << ADC_ZP;
			memory << 0x15;
			memory << BRK;
			
			machine->run(0);
			TS_ASSERT_EQUALS(machine->getCPU().A, 0x11 + 0x13 * i);
		}
	}

	void testOpADC_ZP_DEC(void)
	{
		for (int a=0; a <= 99; a++)
		{
			for (int b=0; b <= 99; b++)
			{
				Memory& memory = machine->getMemory();
				machine->getCPU().D = true;
				machine->getCPU().A = decToBCD(a);
				memory.mem[0x15] = decToBCD(b);
				
				memory.setMemPos(0);
				memory << CLC;
				memory << ADC_ZP;
				memory << 0x15;
				memory << BRK;
				
				machine->run(0);
				TS_ASSERT_EQUALS(bcdToDec(machine->getCPU().A), (a+b) % 100);
			}
		}
	}

	void testOpADC_ZP_X(void)
	{
		// Normal mode
		for (int i=0; i<12; i++)
		{
			Memory& memory = machine->getMemory();
			machine->getCPU().D = false;
			machine->getCPU().A = 0x11;
			machine->getCPU().X = 0x05;
			memory.mem[0x15] = 0x13 * i;
			
			memory.setMemPos(0);
			memory << CLC;
			memory << ADC_ZP_X;
			memory << 0x10;
			memory << BRK;
			
			machine->run(0);
			TS_ASSERT_EQUALS(machine->getCPU().A, 0x11 + 0x13 * i);
		}
	}

	void testOpADC_ZP_X_DEC(void)
	{
		for (int a=0; a <= 99; a++)
		{
			for (int b=0; b <= 99; b++)
			{
				Memory& memory = machine->getMemory();
				machine->getCPU().D = true;
				machine->getCPU().A = decToBCD(a);
				machine->getCPU().X = 0x05;
				memory.mem[0x15] = decToBCD(b);
				
				memory.setMemPos(0);
				memory << CLC;
				memory << ADC_ZP_X;
				memory << 0x10;
				memory << BRK;
				
				machine->run(0);
				TS_ASSERT_EQUALS(bcdToDec(machine->getCPU().A), (a+b) % 100);
			}
		}
	}

	void testOpADC_ABS(void)
	{
		// Normal mode
		for (int i=0; i<12; i++)
		{
			Memory& memory = machine->getMemory();
			machine->getCPU().D = false;
			machine->getCPU().A = 0x12;
			memory.mem[0x1234] = 0x13 * i;

			memory.setMemPos(0);
			memory << CLC;
			memory << ADC_ABS;
			memory << 0x34 << 0x12;
			memory << BRK;

			machine->run(0);
			TS_ASSERT_EQUALS(machine->getCPU().A, 0x12 + 0x13 * i);
		}
	}

	void testOpADC_ABS_DEC(void)
	{
		for (int a=0; a <= 99; a++)
		{
			for (int b=0; b <= 99; b++)
			{
				Memory& memory = machine->getMemory();
				machine->getCPU().D = true;
				machine->getCPU().A = decToBCD(a);
				memory.mem[0x1234] = decToBCD(b);

				memory.setMemPos(0);
				memory << CLC;
				memory << ADC_ABS;
				memory << 0x34 << 0x12;
				memory << BRK;
				
				machine->run(0);
				TS_ASSERT_EQUALS(bcdToDec(machine->getCPU().A), (a+b) % 100);
			}
		}
	}

	void testOpADC_ABS_X(void)
	{
		// Normal mode
		for (int i=0; i<12; i++)
		{
			Memory& memory = machine->getMemory();
			machine->getCPU().D = false;
			machine->getCPU().A = 0x12;
			machine->getCPU().X = 0x11;
			memory.mem[0x1245] = 0x13 * i;

			memory.setMemPos(0);
			memory << CLC;
			memory << ADC_ABS_X;
			memory << 0x34 << 0x12;
			memory << BRK;

			machine->run(0);
			TS_ASSERT_EQUALS(machine->getCPU().A, 0x12 + 0x13 * i);
		}
	}

	void testOpADC_ABS_X_DEC(void)
	{
		for (int a=0; a <= 99; a++)
		{
			for (int b=0; b <= 99; b++)
			{
				Memory& memory = machine->getMemory();
				machine->getCPU().D = true;
				machine->getCPU().A = decToBCD(a);
				machine->getCPU().X = 0x11;
				memory.mem[0x1245] = decToBCD(b);

				memory.setMemPos(0);
				memory << CLC;
				memory << ADC_ABS_X;
				memory << 0x34 << 0x12;
				memory << BRK;
				
				machine->run(0);
				TS_ASSERT_EQUALS(bcdToDec(machine->getCPU().A), (a+b) % 100);
			}
		}
	}

void testOpADC_ABS_Y(void)
	{
		// Normal mode
		for (int i=0; i<12; i++)
		{
			Memory& memory = machine->getMemory();
			machine->getCPU().D = false;
			machine->getCPU().A = 0x12;
			machine->getCPU().Y = 0x12;
			memory.mem[0x1246] = 0x13 * i;
			
			memory.setMemPos(0);
			memory << CLC;
			memory << ADC_ABS_Y;
			memory << 0x34 << 0x12;
			memory << BRK;
			
			machine->run(0);
			TS_ASSERT_EQUALS(machine->getCPU().A, 0x12 + 0x13 * i);
		}
	}

	void testOpADC_ABS_Y_DEC(void)
	{
		for (int a=0; a <= 99; a++)
		{
			for (int b=0; b <= 99; b++)
			{
				Memory& memory = machine->getMemory();
				machine->getCPU().D = true;
				machine->getCPU().A = decToBCD(a);
				machine->getCPU().Y = 0x12;
				memory.mem[0x1246] = decToBCD(b);
				
				memory.setMemPos(0);
				memory << CLC;
				memory << ADC_ABS_Y;
				memory << 0x34 << 0x12;
				memory << BRK;
				
				machine->run(0);
				TS_ASSERT_EQUALS(bcdToDec(machine->getCPU().A), (a+b) % 100);
			}
		}
	}

	void testOpADC_IND_X(void)
	{
		// Normal mode
		for (int i=0; i<12; i++)
		{
			Memory& memory = machine->getMemory();
			memory.mem[0x14] = 0x11;
			memory.mem[0x15] = 0x47;
			memory.mem[0x4711] = 0x13 * i;
			machine->getCPU().D = false;
			machine->getCPU().A = 0x12;                
			machine->getCPU().X = 0x04;
			
			memory.setMemPos(0);
			memory << CLC;
			memory << ADC_IND_X;
			memory << 0x10;
			memory << BRK;
			
			machine->run(0);
			TS_ASSERT_EQUALS(machine->getCPU().A, 0x12 + 0x13 * i);
		}
	}

	void testOpADC_IND_X_DEC(void)
	{
		for (int a=0; a <= 99; a++)
		{
			for (int b=0; b <= 99; b++)
			{
				Memory& memory = machine->getMemory();
				memory.mem[0x14] = 0x11;
				memory.mem[0x15] = 0x47;
				memory.mem[0x4711] = decToBCD(b);;
				machine->getCPU().D = true;
				machine->getCPU().A = decToBCD(a);
				machine->getCPU().X = 0x04;
				
				memory.setMemPos(0);
				memory << CLC;
				memory << ADC_IND_X;
				memory << 0x10;
				memory << BRK;
				
				machine->run(0);
				TS_ASSERT_EQUALS(bcdToDec(machine->getCPU().A), (a+b) % 100);
			}
		}
	}

	void testOpADC_IND_Y(void)
	{
		// Normal mode
		for (int i=0; i<12; i++)
		{
			Memory& memory = machine->getMemory();
			memory.mem[0x10] = 0x00;
			memory.mem[0x11] = 0x47;
			memory.mem[0x4711] = 0x13 * i;
			machine->getCPU().D = false;
			machine->getCPU().A = 0x12;              
			machine->getCPU().Y = 0x11;
			
			memory.setMemPos(0);
			memory << CLC;
			memory << ADC_IND_Y;
			memory << 0x10;
			memory << BRK;
			
			machine->run(0);
			TS_ASSERT_EQUALS(machine->getCPU().A, 0x12 + 0x13 * i);
		}
	}

	void testOpADC_IND_Y_DEC(void)
	{
		for (int a=0; a <= 99; a++)
		{
			for (int b=0; b <= 99; b++)
			{
				Memory& memory = machine->getMemory();
				memory.mem[0x10] = 0x00;
				memory.mem[0x11] = 0x47;
				memory.mem[0x4711] = decToBCD(b);;
				machine->getCPU().D = true;
				machine->getCPU().A = decToBCD(a);
				machine->getCPU().Y = 0x11;
				
				memory.setMemPos(0);
				memory << CLC;
				memory << ADC_IND_Y;
				memory << 0x10;
				memory << BRK;
				
				machine->run(0);
				TS_ASSERT_EQUALS(bcdToDec(machine->getCPU().A), (a+b) % 100);
			}
		}
	}


// SBC
void testOpSBC_IMM(void)
	{
		// Normal mode
		for (int i=0; i<12; i++)
		{
			Memory& memory = machine->getMemory();
			machine->getCPU().D = false;
			machine->getCPU().A = 0x11;
			
			memory.setMemPos(0);
			memory << SEC;
			memory << SBC_IMM;
			memory << 0x13 * i;
			memory << BRK;
			
			machine->run(0);
			int ok = 0x11 - 0x13*i;
			if (ok < 0)  ok = 256 - abs(ok);
			TS_ASSERT_EQUALS(machine->getCPU().A, ok);
		}
	}


	void testOpSBC_IMM_DEC(void)
	{
		for (int a=0; a <= 99; a++)
		{
			for (int b=0; b <= 99; b++)
			{ 
				Memory& memory = machine->getMemory();
				machine->getCPU().D = true;
				machine->getCPU().A = decToBCD(a);
				
				memory.setMemPos(0);
				memory << SEC;
				memory << SBC_IMM;
				memory << decToBCD(b);
				memory << BRK;
				
				machine->run(0);
				int ok = a - b; 
				if (ok < 0) ok = 100 - abs(ok);
				TS_ASSERT_EQUALS(bcdToDec(machine->getCPU().A), ok);
			}
		}
	}

	void testOpSBC_ZP(void)
	{
		// Normal mode
		for (int i=0; i<12; i++)
		{
			Memory& memory = machine->getMemory();
			machine->getCPU().D = false;
			machine->getCPU().A = 0x11;
			memory.mem[0x15] = 0x13 * i;
			
			memory.setMemPos(0);
			memory << SEC;
			memory << SBC_ZP;
			memory << 0x15;
			memory << BRK;
			
			machine->run(0);
			int ok = 0x11 - 0x13*i;
			if (ok < 0)  ok = 256 - abs(ok);
			TS_ASSERT_EQUALS(machine->getCPU().A, ok);
		}
	}

	void testOpSBC_ZP_DEC(void)
	{
		for (int a=0; a <= 99; a++)
		{
			for (int b=0; b <= 99; b++)
			{
				Memory& memory = machine->getMemory();
				machine->getCPU().D = true;
				machine->getCPU().A = decToBCD(a);
				memory.mem[0x15] = decToBCD(b);
				
				memory.setMemPos(0);
				memory << SEC;
				memory << SBC_ZP;
				memory << 0x15;
				memory << BRK;
				
				machine->run(0);
				int ok = a - b; 
				if (ok < 0) ok = 100 - abs(ok);
				TS_ASSERT_EQUALS(bcdToDec(machine->getCPU().A), ok);
			}
		}
	}

	void testOpSBC_ZP_X(void)
	{
		// Normal mode
		for (int i=0; i<12; i++)
		{
			Memory& memory = machine->getMemory();
			machine->getCPU().D = false;
			machine->getCPU().A = 0x11;
			machine->getCPU().X = 0x05;
			memory.mem[0x15] = 0x13 * i;
			
			memory.setMemPos(0);
			memory << SEC;
			memory << SBC_ZP_X;
			memory << 0x10;
			memory << BRK;
			
			machine->run(0);
			int ok = 0x11 - 0x13*i;
			if (ok < 0)  ok = 256 - abs(ok);
			TS_ASSERT_EQUALS(machine->getCPU().A, ok);
		}
	}

	void testOpSBC_ZP_X_DEC(void)
	{
		for (int a=0; a <= 99; a++)
		{
			for (int b=0; b <= 99; b++)
			{
				Memory& memory = machine->getMemory();
				machine->getCPU().D = true;
				machine->getCPU().A = decToBCD(a);
				machine->getCPU().X = 0x05;
				memory.mem[0x15] = decToBCD(b);
				
				memory.setMemPos(0);
				memory << SEC;
				memory << SBC_ZP_X;
				memory << 0x10;
				memory << BRK;
				
				machine->run(0);
				int ok = a - b; 
				if (ok < 0) ok = 100 - abs(ok);
				TS_ASSERT_EQUALS(bcdToDec(machine->getCPU().A), ok);
			}
		}
	}

	void testOpSBC_ABS(void)
	{
		// Normal mode
		for (int i=0; i<12; i++)
		{
			Memory& memory = machine->getMemory();
			machine->getCPU().D = false;
			machine->getCPU().A = 0x12;
			memory.mem[0x1234] = 0x13 * i;
			
			memory.setMemPos(0);
			memory << SEC;
			memory << SBC_ABS;
			memory << 0x34 << 0x12;
			memory << BRK;
			
			machine->run(0);
			int ok = 0x12 - 0x13*i;
			if (ok < 0)  ok = 256 - abs(ok);
			TS_ASSERT_EQUALS(machine->getCPU().A, ok);
		}
	}

	void testOpSBC_ABS_DEC(void)
	{
		for (int a=0; a <= 99; a++)
		{
			for (int b=0; b <= 99; b++)
			{
				Memory& memory = machine->getMemory();
				machine->getCPU().D = true;
				machine->getCPU().A = decToBCD(a);
				memory.mem[0x1234] = decToBCD(b);
				
				memory.setMemPos(0);
				memory << SEC;
				memory << SBC_ABS;
				memory << 0x34 << 0x12;
				memory << BRK;
				
				machine->run(0);
				int ok = a - b; 
				if (ok < 0) ok = 100 - abs(ok);
				TS_ASSERT_EQUALS(bcdToDec(machine->getCPU().A), ok);
			}
		}
	}

	void testOpSBC_ABS_X(void)
	{
		// Normal mode
		for (int i=0; i<12; i++)
		{
			Memory& memory = machine->getMemory();
			machine->getCPU().D = false;
			machine->getCPU().A = 0x12;
			machine->getCPU().X = 0x11;
			memory.mem[0x1245] = 0x13 * i;
			
			memory.setMemPos(0);
			memory << SEC;
			memory << SBC_ABS_X;
			memory << 0x34 << 0x12;
			memory << BRK;
			
			machine->run(0);
			int ok = 0x12 - 0x13*i;
			if (ok < 0)  ok = 256 - abs(ok);
			TS_ASSERT_EQUALS(machine->getCPU().A, ok);
		}
	}

	void testOpSBC_ABS_X_DEC(void)
	{
		for (int a=0; a <= 99; a++)
		{
			for (int b=0; b <= 99; b++)
			{
				Memory& memory = machine->getMemory();
				machine->getCPU().D = true;
				machine->getCPU().A = decToBCD(a);
				machine->getCPU().X = 0x11;
				memory.mem[0x1245] = decToBCD(b);
				
				memory.setMemPos(0);
				memory << SEC;
				memory << SBC_ABS_X;
				memory << 0x34 << 0x12;
				memory << BRK;
				
				machine->run(0);
				int ok = a - b; 
				if (ok < 0) ok = 100 - abs(ok);
				TS_ASSERT_EQUALS(bcdToDec(machine->getCPU().A), ok);
			}
		}
	}

	void testOpSBC_ABS_Y(void)
	{
		// Normal mode
		for (int i=0; i<12; i++)
		{
			Memory& memory = machine->getMemory();
			machine->getCPU().D = false;
			machine->getCPU().A = 0x12;
			machine->getCPU().Y = 0x12;
			memory.mem[0x1246] = 0x13 * i;
			
			memory.setMemPos(0);
			memory << SEC;
			memory << SBC_ABS_Y;
			memory << 0x34 << 0x12;
			memory << BRK;
			
			machine->run(0);
			int ok = 0x12 - 0x13*i;
			if (ok < 0)  ok = 256 - abs(ok);
			TS_ASSERT_EQUALS(machine->getCPU().A, ok);
		}
	}

	void testOpSBC_ABS_Y_DEC(void)
	{
		for (int a=0; a <= 99; a++)
		{
			for (int b=0; b <= 99; b++)
			{
				Memory& memory = machine->getMemory();
				machine->getCPU().D = true;
				machine->getCPU().A = decToBCD(a);
				machine->getCPU().Y = 0x12;
				memory.mem[0x1246] = decToBCD(b);
				
				memory.setMemPos(0);
				memory << SEC;
				memory << SBC_ABS_Y;
				memory << 0x34 << 0x12;
				memory << BRK;
				
				machine->run(0);
				int ok = a - b; 
				if (ok < 0) ok = 100 - abs(ok);
				TS_ASSERT_EQUALS(bcdToDec(machine->getCPU().A), ok);
			}
		}
	}

	void testOpSBC_IND_X(void)
	{
		// Normal mode
		for (int i=0; i<12; i++)
		{
			Memory& memory = machine->getMemory();
			memory.mem[0x14] = 0x11;
			memory.mem[0x15] = 0x47;
			memory.mem[0x4711] = 0x13 * i;
			machine->getCPU().D = false;
			machine->getCPU().A = 0x12;                
			machine->getCPU().X = 0x04;
			
			memory.setMemPos(0);
			memory << SEC;
			memory << SBC_IND_X;
			memory << 0x10;
			memory << BRK;
			
			machine->run(0);
			int ok = 0x12 - 0x13*i;
			if (ok < 0)  ok = 256 - abs(ok);
			TS_ASSERT_EQUALS(machine->getCPU().A, ok);
		}
	}

	void testOpSBC_IND_X_DEC(void)
	{
		for (int a=0; a <= 99; a++)
		{
			for (int b=0; b <= 99; b++)
			{
				Memory& memory = machine->getMemory();
				memory.mem[0x14] = 0x11;
				memory.mem[0x15] = 0x47;
				memory.mem[0x4711] = decToBCD(b);;
				machine->getCPU().D = true;
				machine->getCPU().A = decToBCD(a);
				machine->getCPU().X = 0x04;
				
				memory.setMemPos(0);
				memory << SEC;
				memory << SBC_IND_X;
				memory << 0x10;
				memory << BRK;
				
				machine->run(0);
				int ok = a - b; 
				if (ok < 0) ok = 100 - abs(ok);
				TS_ASSERT_EQUALS(bcdToDec(machine->getCPU().A), ok);
			}
		}
	}

	void testOpSBC_IND_Y(void)
	{
		// Normal mode
		for (int i=0; i<12; i++)
		{
			Memory& memory = machine->getMemory();
			memory.mem[0x10] = 0x00;
			memory.mem[0x11] = 0x47;
			memory.mem[0x4711] = 0x13 * i;
			machine->getCPU().D = false;
			machine->getCPU().A = 0x12;              
			machine->getCPU().Y = 0x11;
			
			memory.setMemPos(0);
			memory << SEC;
			memory << SBC_IND_Y;
			memory << 0x10;
			memory << BRK;
			
			machine->run(0);
			int ok = 0x12 - 0x13*i;
			if (ok < 0)  ok = 256 - abs(ok);
			TS_ASSERT_EQUALS(machine->getCPU().A, ok);
		}
	}

	void testOpSBC_IND_Y_DEC(void)
	{
		for (int a=0; a <= 99; a++)
		{
			for (int b=0; b <= 99; b++)
			{
				Memory& memory = machine->getMemory();
				memory.mem[0x10] = 0x00;
				memory.mem[0x11] = 0x47;
				memory.mem[0x4711] = decToBCD(b);;
				machine->getCPU().D = true;
				machine->getCPU().A = decToBCD(a);
				machine->getCPU().Y = 0x11;
				
				memory.setMemPos(0);
				memory << SEC;
				memory << SBC_IND_Y;
				memory << 0x10;
				memory << BRK;
				
				machine->run(0);
				int ok = a - b; 
				if (ok < 0) ok = 100 - abs(ok);
				TS_ASSERT_EQUALS(bcdToDec(machine->getCPU().A), ok);
			}
		}
	}
};
