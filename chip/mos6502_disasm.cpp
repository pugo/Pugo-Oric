
#include <stdio.h>
#include <string.h>
#include <iostream>

#include "oric.hpp"
#include "memory.hpp"

#define NUMBER_OPCODES 151

// The 6502's 13 addressing modes
#define IMMED 0	// Immediate
#define ABSOL 1	// Absolute
#define ZEROP 2	// Zero Page
#define IMPLI 3	// Implied
#define INDIA 4	// Indirect Absolute
#define ABSIX 5	// Absolute indexed with X
#define ABSIY 6	// Absolute indexed with Y
#define ZEPIX 7	// Zero page indexed with X
#define ZEPIY 8	// Zero page indexed with Y
#define INDIN 9	// Indexed indirect (with x)
#define ININD 10	// Indirect indexed (with y)
#define RELAT 11	// Relative
#define ACCUM 12	// Accumulator

#define LSB_FIRST

typedef struct OPcode {
	uint8_t number;		// Number of the opcode
	uint8_t name;			// Index in the name table
	uint8_t addressing;	// Addressing mode
	uint8_t cycles;		// Number of cycles
	uint8_t cross_page;	// 1 if cross-page boundaries affect cycles
} OPcode;

typedef union
{
#ifdef LSB_FIRST
	struct { uint8_t l,h; } B;
#else
	struct { byte h,l; } B;
#endif
	unsigned short W;
} d_word;

char name_table[56][4]={
"ADC", "AND", "ASL", "BCC", "BCS", "BEQ", "BIT", "BMI", "BNE", "BPL",
"BRK", "BVC", "BVS", "CLC", "CLD", "CLI", "CLV", "CMP", "CPX", "CPY",
"DEC", "DEX", "DEY", "EOR", "INC", "INX", "INY", "JMP", "JSR", "LDA",
"LDX", "LDY", "LSR", "NOP", "ORA", "PHA", "PHP", "PLA", "PLP", "ROL",
"ROR", "RTI", "RTS", "SBC", "SEC", "SED", "SEI", "STA", "STX", "STY",
"TAX", "TAY", "TSX", "TXA", "TXS", "TYA"};

/* Opcode table */
OPcode opcode_table[NUMBER_OPCODES] = {
	{0x69,0,IMMED,2,1}, /* ADC */
	{0x65,0,ZEROP,3,1},
	{0x75,0,ZEPIX,4,1},
	{0x6D,0,ABSOL,4,1},
	{0x7D,0,ABSIX,4,1},
	{0x79,0,ABSIY,4,1},
	{0x61,0,INDIN,6,1},
	{0x71,0,ININD,5,1},

	{0x29,1,IMMED,2,1}, /* AND */
	{0x25,1,ZEROP,3,1},
	{0x35,1,ZEPIX,4,1},
	{0x2D,1,ABSOL,4,1},
	{0x3D,1,ABSIX,4,1},
	{0x39,1,ABSIY,4,1},
	{0x21,1,INDIN,6,1},
	{0x31,1,ININD,5,1},

	{0x0A,2,ACCUM,2,0}, /* ASL */
	{0x06,2,ZEROP,5,0}, 
	{0x16,2,ZEPIX,6,0},
	{0x0E,2,ABSOL,6,0},
	{0x1E,2,ABSIX,6,0},

	{0x90,3,RELAT,4,1}, /* BCC */

	{0xB0,4,RELAT,4,1}, /* BCS */

	{0xF0,5,RELAT,4,1}, /* BEQ */

	{0x24,6,ZEROP,3,0}, /* BIT */
	{0x2C,6,ABSOL,4,0}, 

	{0x30,7,RELAT,4,1}, /* BMI */

	{0xD0,8,RELAT,4,1}, /* BNE */

	{0x10,9,RELAT,4,1}, /* BPL */

	{0x00,10,IMPLI,7,0}, /* BRK */

	{0x50,11,RELAT,4,1}, /* BVC */ 

	{0x70,12,RELAT,4,1}, /* BVS */ 

	{0x18,13,IMPLI,2,0}, /* CLC */ 

	{0xD8,14,IMPLI,2,0}, /* CLD */ 

	{0x58,15,IMPLI,2,0}, /* CLI */ 

	{0xB8,16,IMPLI,2,0}, /* CLV */ 

	{0xC9,17,IMMED,2,0}, /* CMP */
	{0xC5,17,ZEROP,3,0},
	{0xD5,17,ZEPIX,4,0},
	{0xCD,17,ABSOL,4,0},
	{0xDD,17,ABSIX,4,0},
	{0xD9,17,ABSIY,4,0},
	{0xC1,17,INDIN,6,0},
	{0xD1,17,ININD,5,0},

	{0xE0,18,IMMED,2,0}, /* CPX */
	{0xE4,18,ZEROP,3,0},
	{0xEC,18,ABSOL,4,0},

	{0xC0,19,IMMED,2,0}, /* CPY */
	{0xC4,19,ZEROP,3,0},
	{0xCC,19,ABSOL,4,0},

	{0xC6,20,ZEROP,5,0}, /* DEC */
	{0xD6,20,ZEPIX,6,0},
	{0xCE,20,ABSOL,6,0},
	{0xDE,20,ABSIX,6,0},

	{0xCA,21,IMPLI,2,0}, /* DEX */

	{0x88,22,IMPLI,2,0}, /* DEY */

	{0x49,23,IMMED,2,1}, /* EOR */
	{0x45,23,ZEROP,3,1},
	{0x55,23,ZEPIX,4,1},
	{0x4D,23,ABSOL,4,1},
	{0x5D,23,ABSIX,4,1},
	{0x59,23,ABSIY,4,1},
	{0x41,23,INDIN,6,1},
	{0x51,23,ININD,5,1},

	{0xE6,24,ZEROP,5,0}, /* INC */
	{0xF6,24,ZEPIX,6,0},
	{0xEE,24,ABSOL,6,0},
	{0xFE,24,ABSIX,6,0},

	{0xE8,25,IMPLI,2,0}, /* INX */

	{0xC8,26,IMPLI,2,0}, /* INY */

	{0x4C,27,ABSOL,3,0}, /* JMP */
	{0x6C,27,INDIA,5,0},

	{0x20,28,ABSOL,6,0}, /* JSR */

	{0xA9,29,IMMED,2,1}, /* LDA */
	{0xA5,29,ZEROP,3,1},
	{0xB5,29,ZEPIX,4,1},
	{0xAD,29,ABSOL,4,1},
	{0xBD,29,ABSIX,4,1},
	{0xB9,29,ABSIY,4,1},
	{0xA1,29,INDIN,6,1},
	{0xB1,29,ININD,5,1},

	{0xA2,30,IMMED,2,1}, /* LDX */
	{0xA6,30,ZEROP,3,1},
	{0xB6,30,ZEPIY,4,1},
	{0xAE,30,ABSOL,4,1},
	{0xBE,30,ABSIY,4,1},

	{0xA0,31,IMMED,2,1}, /* LDY */
	{0xA4,31,ZEROP,3,1},
	{0xB4,31,ZEPIX,4,1},
	{0xAC,31,ABSOL,4,1},
	{0xBC,31,ABSIX,4,1},

	{0x4A,32,ACCUM,2,0}, /* LSR */
	{0x46,32,ZEROP,5,0}, 
	{0x56,32,ZEPIX,6,0},
	{0x4E,32,ABSOL,6,0},
	{0x5E,32,ABSIX,6,0},

	{0xEA,33,IMPLI,2,0}, /* NOP */

	{0x09,34,IMMED,2,0}, /* ORA */
	{0x05,34,ZEROP,3,0},
	{0x15,34,ZEPIX,4,0},
	{0x0D,34,ABSOL,4,0},
	{0x1D,34,ABSIX,4,0},
	{0x19,34,ABSIY,4,0},
	{0x01,34,INDIN,6,0},
	{0x11,34,ININD,5,0},

	{0x48,35,IMPLI,3,0}, /* PHA */

	{0x08,36,IMPLI,3,0}, /* PHP */

	{0x68,37,IMPLI,4,0}, /* PLA */

	{0x28,38,IMPLI,4,0}, /* PLP */

	{0x2A,39,ACCUM,2,0}, /* ROL */
	{0x26,39,ZEROP,5,0}, 
	{0x36,39,ZEPIX,6,0},
	{0x2E,39,ABSOL,6,0},
	{0x3E,39,ABSIX,6,0},

	{0x6A,40,ACCUM,2,0}, /* ROR */
	{0x66,40,ZEROP,5,0}, 
	{0x76,40,ZEPIX,6,0},
	{0x6E,40,ABSOL,6,0},
	{0x7E,40,ABSIX,6,0},

	{0x40,41,IMPLI,6,0}, /* RTI */

	{0x60,42,IMPLI,6,0}, /* RTS */

	{0xE9,43,IMMED,2,1}, /* SBC */
	{0xE5,43,ZEROP,3,1},
	{0xF5,43,ZEPIX,4,1},
	{0xED,43,ABSOL,4,1},
	{0xFD,43,ABSIX,4,1},
	{0xF9,43,ABSIY,4,1},
	{0xE1,43,INDIN,6,1},
	{0xF1,43,ININD,5,1},

	{0x38,44,IMPLI,2,0}, /* SEC */

	{0xF8,45,IMPLI,2,0}, /* SED */

	{0x78,46,IMPLI,2,0}, /* SEI */

	{0x85,47,ZEROP,3,0}, /* STA */
	{0x95,47,ZEPIX,4,0},
	{0x8D,47,ABSOL,4,0},
	{0x9D,47,ABSIX,4,0},
	{0x99,47,ABSIY,4,0},
	{0x81,47,INDIN,6,0},
	{0x91,47,ININD,5,0},

	{0x86,48,ZEROP,3,0}, /* STX */
	{0x96,48,ZEPIY,4,0},
	{0x8E,48,ABSOL,4,0},

	{0x84,49,ZEROP,3,0}, /* STY */
	{0x94,49,ZEPIX,4,0},
	{0x8C,49,ABSOL,4,0},

	{0xAA,50,IMPLI,2,0}, /* TAX */

	{0xA8,51,IMPLI,2,0}, /* TAY */

	{0xBA,52,IMPLI,2,0}, /* TSX */

	{0x8A,53,IMPLI,2,0}, /* TXA */

	{0x9A,54,IMPLI,2,0}, /* TXS */

	{0x98,55,IMPLI,2,0} /* TYA */
};

unsigned short org; /* Origin of addresses */
char hex_output = 0; /* 1 if hex output is desired at beginning of line */
char cycle_counting = 0; /* 1 if we want cycle counting */
char nes_mode = 0; /* 1 if NES commenting and warnings are enabled */
FILE *f; /* Input file */
unsigned short max=0xffff; /* Maximum number of bytes to disassemble */
char line[512];


/* This function appends cycle counting to the comment block */
void append_cycle(char *input, uint8_t entry, unsigned short arg, unsigned short cur_PC)
{
	char tmpstr[256];
	int cycles = 0; 

	cycles = opcode_table[entry].cycles;

	sprintf(tmpstr," Cycles: %d ",cycles);
	if (opcode_table[entry].cross_page) strcat(tmpstr,"*!* ");
	strcat(input,tmpstr);
}

void add_nes_str(char *instr, char *instr2)
{
	strcat(instr," [NES] ");
	strcat(instr,instr2);
}


/* This function disassembles the opcode at the PC and outputs it in *output */
std::string MOS6502::disassemble(uint16_t a_Address)
{
	uint8_t tmp_byte1, tmp_byte2, opcode; 
	char argument_signed;
	d_word tmp_word;
	char tmpstr[256], tmpstr2[256], tmpstr3[256];
	int i,j,entry,found = 0;

	opcode = memory.mem[a_Address];
	for (i = 0; i < NUMBER_OPCODES; i++) {
		if (opcode == opcode_table[i].number) {
			found = 1; /* Found the opcode */
			entry = i; /* Note the entry number in the table */
		}
	}

	if (!found) {
		if (hex_output) {
			sprintf(tmpstr, "$%04X> %02X:\t.byte $%02x\t\t; INVALID OPCODE !!!\n", org + a_Address, opcode, opcode); 
		}
		else {
			sprintf(tmpstr, "$%04X\t.byte $%02x\t; INVALID OPCODE !!!\n", org + a_Address, opcode);
		}
	}
	else {
		switch (opcode_table[entry].addressing)
		{
		case IMMED:
			a_Address++;
			tmp_byte1 = memory.mem[a_Address]; /* Get immediate value */
			if (hex_output)
				sprintf(tmpstr,"$%04X> %02X %02X:\t%s #$%02x\t;",org+a_Address-1,opcode,tmp_byte1,name_table[opcode_table[entry].name],tmp_byte1);
			else
				sprintf(tmpstr,"$%04X\t%s #$%02x\t;",org+a_Address-1,name_table[opcode_table[entry].name],tmp_byte1);

			/* Add cycle count if necessary */
			if (cycle_counting) append_cycle(tmpstr,entry,org+a_Address-1,org+a_Address-1);
			break;

		case ABSOL:
			a_Address++;
			tmp_word.B.l = memory.mem[a_Address]; /* Get low byte of a_Address */
			a_Address++;
			tmp_word.B.h = memory.mem[a_Address]; /* Get high byte of a_Address */

			if (hex_output)
				sprintf(tmpstr,"$%04X> %02X %02X%02X:\t%s $%02X%02X\t;",org+a_Address-2,opcode,tmp_word.B.l,tmp_word.B.h,name_table[opcode_table[entry].name],tmp_word.B.h,tmp_word.B.l);
			else
				sprintf(tmpstr,"$%04X\t%s $%02X%02X\t;",org+a_Address-2,name_table[opcode_table[entry].name],tmp_word.B.h,tmp_word.B.l);

			/* Add cycle count if necessary */
			if (cycle_counting) append_cycle(tmpstr,entry,tmp_word.W,org+a_Address-2);
			break;

		case ZEROP:
			a_Address++;
			tmp_byte1 = memory.mem[a_Address]; /* Get low byte of a_Address */

			if (hex_output)
				sprintf(tmpstr,"$%04X> %02X %02X:\t%s $%02X\t\t;",org+a_Address-1,opcode,tmp_byte1,name_table[opcode_table[entry].name],tmp_byte1);
			else
				sprintf(tmpstr,"$%04X\t%s $%02X\t\t;",org+a_Address-1,name_table[opcode_table[entry].name],tmp_byte1);

			/* Add cycle count if necessary */
			if (cycle_counting) append_cycle(tmpstr,entry,org+a_Address-1,org+a_Address-1);
			break;

		case IMPLI:
			if (hex_output)
				sprintf(tmpstr,"$%04X> %02X:\t%s\t\t;",org+a_Address,opcode,name_table[opcode_table[entry].name]);
			else
				sprintf(tmpstr,"$%04X\t%s\t\t;",org+a_Address,name_table[opcode_table[entry].name]);

			/* Add cycle count if necessary */
			if (cycle_counting) append_cycle(tmpstr,entry,org+a_Address,org+a_Address);
			break;

		case INDIA:
			a_Address++;
			tmp_word.B.l = memory.mem[a_Address]; /* Get low byte of a_Address */
			a_Address++;
			tmp_word.B.h = memory.mem[a_Address]; /* Get high byte of a_Address */

			if (hex_output)
				sprintf(tmpstr,"$%04X> %02X %02X%02X:\t%s ($%02X%02X)\t;",org+a_Address-2,opcode,tmp_word.B.l,tmp_word.B.h,name_table[opcode_table[entry].name],tmp_word.B.h,tmp_word.B.l);
			else
				sprintf(tmpstr,"$%04X\t%s ($%02X%02X)\t;",org+a_Address-2,name_table[opcode_table[entry].name],tmp_word.B.h,tmp_word.B.l);

			/* Add cycle count if necessary */
			if (cycle_counting) append_cycle(tmpstr,entry,tmp_word.W,org+a_Address-2);
			break;

		case ABSIX:
			a_Address++;
			tmp_word.B.l = memory.mem[a_Address]; /* Get low byte of a_Address */
			a_Address++;
			tmp_word.B.h = memory.mem[a_Address]; /* Get high byte of a_Address */

			if (hex_output)
				sprintf(tmpstr,"$%04X> %02X %02X%02X:\t%s $%02X%02X,X\t;",org+a_Address-2,opcode,tmp_word.B.l,tmp_word.B.h,name_table[opcode_table[entry].name],tmp_word.B.h,tmp_word.B.l);
			else
				sprintf(tmpstr,"$%04X\t%s $%02X%02X,X\t;",org+a_Address-2,name_table[opcode_table[entry].name],tmp_word.B.h,tmp_word.B.l);

			/* Add cycle count if necessary */
			if (cycle_counting) append_cycle(tmpstr,entry,tmp_word.W,org+a_Address-2);
			break;

		case ABSIY:
			a_Address++;
			tmp_word.B.l = memory.mem[a_Address]; /* Get low byte of a_Address */
			a_Address++;
			tmp_word.B.h = memory.mem[a_Address]; /* Get high byte of a_Address */

			if (hex_output)
				sprintf(tmpstr,"$%04X> %02X %02X%02X:\t%s $%02X%02X,Y\t;",org+a_Address-2,opcode,tmp_word.B.l,tmp_word.B.h,name_table[opcode_table[entry].name],tmp_word.B.h,tmp_word.B.l);
			else
				sprintf(tmpstr,"$%04X\t%s $%02X%02X,Y\t;",org+a_Address-2,name_table[opcode_table[entry].name],tmp_word.B.h,tmp_word.B.l);

			/* Add cycle count if necessary */
			if (cycle_counting) append_cycle(tmpstr,entry,tmp_word.W,org+a_Address-2);
			break;

		case ZEPIX:
			a_Address++;
			tmp_byte1 = memory.mem[a_Address]; /* Get low byte of a_Address */

			if (hex_output)
				sprintf(tmpstr,"$%04X> %02X %02X:\t%s $%02X,X\t\t;",org+a_Address-1,opcode,tmp_byte1,name_table[opcode_table[entry].name],tmp_byte1);
			else
				sprintf(tmpstr,"$%04X\t%s $%02X,X\t;",org+a_Address-1,name_table[opcode_table[entry].name],tmp_byte1);

			/* Add cycle count if necessary */
			if (cycle_counting) append_cycle(tmpstr,entry,org+a_Address-1,org+a_Address-1);
			break;

		case ZEPIY:
			a_Address++;
			tmp_byte1 = memory.mem[a_Address]; /* Get low byte of a_Address */

			if (hex_output)
				sprintf(tmpstr,"$%04X> %02X %02X:\t%s $%02X,Y\t\t;",org+a_Address-1,opcode,tmp_byte1,name_table[opcode_table[entry].name],tmp_byte1);
			else
				sprintf(tmpstr,"$%04X\t%s $%02X,Y\t;",org+a_Address-1,name_table[opcode_table[entry].name],tmp_byte1);

			/* Add cycle count if necessary */
			if (cycle_counting) append_cycle(tmpstr,entry,org+a_Address-1,org+a_Address-1);
			break;

		case INDIN:
			a_Address++;
			tmp_byte1 = memory.mem[a_Address]; /* Get low byte of a_Address */

			if (hex_output)
				sprintf(tmpstr,"$%04X> %02X %02X:\t%s ($%02X,X)\t\t;",org+a_Address-1,opcode,tmp_byte1,name_table[opcode_table[entry].name],tmp_byte1);
			else
				sprintf(tmpstr,"$%04X\t%s ($%02X,X)\t;",org+a_Address-1,name_table[opcode_table[entry].name],tmp_byte1);

			/* Add cycle count if necessary */
			if (cycle_counting) append_cycle(tmpstr,entry,org+a_Address-1,org+a_Address-1);
			break;

		case ININD:
			a_Address++;
			tmp_byte1 = memory.mem[a_Address]; /* Get low byte of a_Address */

			if (hex_output)
				sprintf(tmpstr,"$%04X> %02X %02X:\t%s ($%02X),Y\t\t;",org+a_Address-1,opcode,tmp_byte1,name_table[opcode_table[entry].name],tmp_byte1);
			else
				sprintf(tmpstr,"$%04X\t%s ($%02X),Y\t;",org+a_Address-1,name_table[opcode_table[entry].name],tmp_byte1);

			/* Add cycle count if necessary */
			if (cycle_counting) append_cycle(tmpstr,entry,org+a_Address-1,org+a_Address-1);
			break;

		case RELAT:
			a_Address++;
			tmp_byte1 = memory.mem[a_Address]; /* Get relative modifier */

			if (hex_output) 
				sprintf(tmpstr,"$%04X> %02X %02X:\t%s $%04X\t\t;",org+a_Address-1,opcode,tmp_byte1,name_table[opcode_table[entry].name],(org+a_Address)+(signed char)(tmp_byte1)+1);
			else
				sprintf(tmpstr,"$%04X\t%s $%04X\t;",org+a_Address-1,name_table[opcode_table[entry].name],(org+a_Address)+(signed char)(tmp_byte1)+1);

			/* Add cycle count if necessary */
			if (cycle_counting) append_cycle(tmpstr,entry,org+a_Address,org+a_Address);
			break;

		case ACCUM:
			if (hex_output)
				sprintf(tmpstr,"$%04X> %02X:\t%s A\t\t;",org+a_Address,opcode,name_table[opcode_table[entry].name]);
			else
				sprintf(tmpstr,"$%04X\t%s A\t\t;",org+a_Address,name_table[opcode_table[entry].name]);

			/* Add cycle count if necessary */
			if (cycle_counting) append_cycle(tmpstr,entry,org+a_Address,org+a_Address);
			break;

		default:
			break;
		}
	}
	return tmpstr;
}
