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


#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <iostream>
#include <cstdlib>

#include "mos6502.h"
#include "memory.h"
#include "datatypes.h"

using namespace std;

int main(int argc, char *argv[])
{
	cout << "Hello, world!" << endl;
	char pwd[1024];
	getcwd( pwd, 1024 );
	cout << "pwd: " << pwd << endl;

	Memory * memory = new Memory( 65536 );
	memory->load("/home/pugo/projekt/oric/ROMS/basic10.rom", 0xc000);
	
	MOS6502 * cpu = new MOS6502( memory );

	cpu->run();

	return EXIT_SUCCESS;
}
