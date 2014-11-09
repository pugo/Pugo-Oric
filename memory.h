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
#ifndef MEMORY_H
#define MEMORY_H

#include "datatypes.h"

#include <iostream>
#include <ostream>


/**
	@author Anders Karlsson <pugo@pugo.org>
*/
class Memory
{
public:
	Memory(unsigned int size);
	~Memory();

	void load(std::string path, word address);
	unsigned int getSize() { return size; }

	void show(unsigned int pos, unsigned int length);

// 	friend Memory& operator>>(Memory& is, Memory& obj)
// 	{
// 		std::cout << ">>" << std::endl;
// 		//obj.mem[obj.mempos++] = is;
// 		return is;
// 	}
// 
// 
// 	friend Memory& operator<<(Memory& os, unsigned int i)
// 	{
// 		os.mem[os.mempos++] = i & 0xff;
// 		return os;
// 	}

	byte* mem;

protected:
	unsigned int size;
};



#endif // MEMORY_H
