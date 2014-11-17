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

#ifndef MOS6502_H
#define MOS6502_H

#include "memory.h"
#include "datatypes.h"
#include "mos6502_opcodes.h"

#define STACK_BOTTOM 0x0100

#define NMI_VECTOR_L 0xFFFA
#define NMI_VECTOR_H 0xFFFB

#define RESET_VECTOR_L 0xFFFC
#define RESET_VECTOR_H 0xFFFD

#define IRQ_VECTOR_L 0xFFFE
#define IRQ_VECTOR_H 0xFFFF


class Machine;
class MOS6502;

typedef byte (*f_memory_read_byte_handler)(Machine &oric, word address);
typedef byte (*f_memory_read_byte_zp_handler)(Machine &oric, byte address);

typedef word (*f_memory_read_word_handler)(Machine &oric, word address);
typedef word (*f_memory_read_word_zp_handler)(Machine &oric, byte address);

typedef void (*f_memory_write_byte_handler)(Machine &oric, word address, byte val);
typedef void (*f_memory_write_byte_zp_handler)(Machine &oric, byte address, byte val);


class MOS6502
{
public:
	MOS6502(Machine* machine, Memory* memory);
	~MOS6502();

	void setPC(word pc) { PC = pc; }
	word getPC() { return PC; }
	byte getSP() { return SP; }
	byte getP();
	void setP(byte new_p);

	void setQuiet(bool val) { quiet = val; }

	void reset();
	void printStat();
	void printStat(word address);

	bool execInstructionCycles(int cycles);
	short execInstruction(bool& brk);

	Memory& getMemory() { return *memory; }

	// Registers
	byte A;
	byte X;
	byte Y;

	// Flags
	//   7                           0
	// +---+---+---+---+---+---+---+---+
	// | N | V |   | B | D | I | Z | C |  <-- flag, 0/1 = reset/set
	// +---+---+---+---+---+---+---+---+
	byte N_INTERN; // negative
	byte Z_INTERN; // zero
	bool V; // overflow
	bool B; // break
	bool D; // decimal
	bool I; // interrupt
	bool C; // carry

	void NMI();
	void IRQ();

	int inline signedByteToInt(byte b);

	// Add and sub are complex
	void adc(byte val);
	void sbc(byte val);

	f_memory_read_byte_handler memory_read_byte_handler;
	f_memory_read_byte_zp_handler memory_read_byte_zp_handler;

	f_memory_read_word_handler memory_read_word_handler;
	f_memory_read_word_zp_handler memory_read_word_zp_handler;

	f_memory_write_byte_handler memory_write_byte_handler;
	f_memory_write_byte_zp_handler memory_write_byte_zp_handler;

	std::string disassemble(word address);

protected:
	Machine* machine;
	Memory* memory;

	word PC;
	byte SP;
	bool quiet;
};

#endif // MOS6502_H
