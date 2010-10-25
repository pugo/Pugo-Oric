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

#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <fstream>


#include "memory.h"

#include "mos6502_opcodes.h"
#include "mos6502.h"

using namespace std;

Memory::Memory(unsigned int size) :
    mem(NULL), size(size), mempos(START_VECTOR)
{
	//cout << "Memory::Memory( " << size << " )" << endl;

    mem = new byte[size];
    
    
//     mem[0x0010] = 0x00;
//     mem[0x0011] = 0x10;
//     mem[0x0012] = 0x20;
//     mem[0x0020] = 0x42;

//     mem[0x1001] = 0x42;



//     mem[0x0200] = SED;

//     mem[0x0201] = LDA_IMM;
//     mem[0x0202] = 0x5;
//     mem[0x0203] = ADC_IMM;
//     mem[0x0204] = 0x6;

//     mem[0x0205] = ADC_ABS;
//     mem[0x0206] = 0x00;
//     mem[0x0207] = 0x03;

//     mem[0x0208] = SEC;

//     mem[0x0209] = SBC_IMM;
//     mem[0x020A] = 0x01;
//     mem[0x020B] = SBC_IMM;
//     mem[0x020C] = 0x01;
//     mem[0x020D] = NOP;
    

//     mem[0x020E] = BRK;


//     mem[0x0300] = 0x01;

//     mem[0x0200] = LDA_IMM;
//     mem[0x0201] = 0x10;
//     mem[0x0202] = STA_ZP;
//     mem[0x0203] = 0x10;
    
//     mem[0x0204] = DEC_ZP;
//     mem[0x0205] = 0x10;
//     mem[0x0206] = BPL;
//     mem[0x0207] = -4;
    
//     mem[0x0208] = JMP_ABS;
//     mem[0x0209] = 0x08;
//     mem[0x020A] = 0x02;

//     mem[0x020B] = BRK;
    

//      mem[0x0200] = LDY_IMM;
//      mem[0x0201] = 0xff;
//      mem[0x0202] = LDX_IMM;
//      mem[0x0203] = 0xff;

//      mem[0x0204] = DEX;
//      mem[0x0205] = BNE;
//      mem[0x0206] = -3;

//      mem[0x0207] = BRK;

//      mem[0x0208] = NOP;

//      mem[0x0209] = DEY;
//      mem[0x020A] = BNE;
//      mem[0x020B] = -10;


//      // Set interrupt vector to handler 2
//     mem[0x020C] = LDA_IMM;
//     mem[0x020D] = 0x00;
//     mem[0x020E] = STA_ABS;
//     mem[0x020F] = 0xFE;
//     mem[0x0210] = 0xFF;

//     mem[0x0211] = LDA_IMM;
//     mem[0x0212] = 0x05;
//     mem[0x0213] = STA_ABS;
//     mem[0x0214] = 0xFF;
//     mem[0x0215] = 0xFF;
    

//     mem[0x0216] = BRK;

//     // interrupt handler 1
//     mem[0x0400] = NOP;
//     mem[0x0401] = NOP;
//     mem[0x0402] = NOP;
//     mem[0x0403] = NOP;
//     mem[0x0404] = RTI;

//     // interrupt handler 2
//     mem[0x0500] = JMP_ABS;
//     mem[0x0501] = 0x00;
//     mem[0x0502] = 0x05;

//     mem[IRQ_VECTOR_L] = 0x00;
//     mem[IRQ_VECTOR_H] = 0x04;
}


Memory::~Memory()
{
    if (mem)
        delete[] mem;
}


void error_exit( string description )
{
    cout << endl << "!!! Error: " << description << endl << endl;
    exit( 1 );
}



void Memory::load(std::string path, word address)
{
    // stat to check existance and size
    struct stat file_info;
    if ( stat( path.c_str(), &file_info ) )
        error_exit( "no such file: " + path );

    int file_size = file_info.st_size;

    int fd = open( path.c_str(), O_RDONLY );
    if ( fd < 0 )
        error_exit( "could not open file: " + path );

    ssize_t count = 0;
    int pos = address;
    byte * buff = (byte*)malloc(1);
    while ( file_size-count > 0 )
    {
        ssize_t result = read( fd, buff, 1 );

        if ( result == 0 )
            break;
        if ( result == -1 )
            error_exit( "error reading file: " + path );
        
        mem[pos] = *buff;
        pos += result;
        count += result;
    }

	free( buff );
    return;
}


void Memory::show(unsigned int pos, unsigned int length)
{
    cout << "Showing " << length << " bytes from " << pos << endl;
    for (unsigned int i=0; i < length; i++)
        cout << "[" << pos + i << "] " << (unsigned int)mem[pos + i] << " (" << hex << (unsigned int)mem[pos + i] << ")" << endl;
}
