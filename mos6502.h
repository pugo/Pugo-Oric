/***************************************************************************
*   Copyright (C) 2009 by Anders Karlsson   *
*   pugo@pugo.org   *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
*                                                                         *
*   This program is distributed in the hope that it will be useful,       *
*   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
*   GNU General Public License for more details.                          *
*                                                                         *
*   You should have received a copy of the GNU General Public License     *
*   along with this program; if not, write to the                         *
*   Free Software Foundation, Inc.,                                       *
*   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
***************************************************************************/
#ifndef MOS6502_H
#define MOS6502_H

#include "datatypes.h"
#include "cpu.h"

#include "mos6502_opcodes.h"

#define STACK_BOTTOM 0x0100

#define NMI_VECTOR_L 0xFFFA
#define NMI_VECTOR_H 0xFFFB

#define RESET_VECTOR_L 0xFFFC
#define RESET_VECTOR_H 0xFFFD

#define IRQ_VECTOR_L 0xFFFE
#define IRQ_VECTOR_H 0xFFFF


typedef byte (*f_memory_read_handler)(word address);
typedef void (*f_memory_write_handler)(word address, byte data);


/**
	@author Anders Karlsson <pugo@pugo.org>
*/
class MOS6502 : public CPU
{
public:
	MOS6502(Memory* memory);
	~MOS6502();

	void reset();
	void printStat(word address);
	bool execInstructions(unsigned int cycles);

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

	byte getP();
	void setP(byte new_p);

	void NMI();
	void IRQ();

	int inline signedByteToInt(byte b);

	byte inline read_byte(word address);
	byte inline read_byte_zp(byte address);

	word inline read_word(word address);
	word inline read_word_zp(byte address);

	void inline write_byte(word address, byte val);
	void inline write_byte_zp(byte address, byte val);

	// Add and sub are complex
	void adc(byte val);
	void sbc(byte val);

	f_memory_read_handler memory_read_handler;
	f_memory_write_handler memory_write_handler;

	std::string disassemble(word address);

protected:

};

#endif // MOS6502_H
