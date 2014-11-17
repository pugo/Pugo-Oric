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


#ifndef MEMORY_H
#define MEMORY_H

#include "datatypes.h"

#include <iostream>
#include <ostream>


class Memory
{
public:
	Memory(unsigned int size);
	~Memory();

	void load(std::string path, word address);
	unsigned int getSize() { return size; }
	
	void setMemPos(word address) { mempos = address; }

// 	friend Memory& operator>>(Memory& is, Memory& obj)                            
// 	{                                                                   
// 		obj.mem[obj.mempos++] = is;
// 		return is;
// 	}

	friend Memory& operator<<(Memory& os, unsigned int i)
	{
		os.mem[os.mempos++] = static_cast<byte>(i & 0xff);
		return os;
	}

	void show(unsigned int pos, unsigned int length);

	byte* mem;

	
	
	
protected:
	unsigned int size;
	unsigned int mempos;
};



#endif // MEMORY_H
