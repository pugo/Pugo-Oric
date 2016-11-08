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

#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <string.h>

#include "memory.h"

#include "mos6502_opcodes.h"
#include "mos6502.h"


Memory::Memory(uint32_t a_Size) :
	m_Mem(NULL),
	m_Size(a_Size),
	m_Mempos(0)
{
	m_Mem = new uint8_t[a_Size];
	memset(m_Mem, 0, a_Size);
}

Memory::~Memory()
{
	delete[] m_Mem;
}

void error_exit(std::string a_Description)
{
	std::cout << std::endl << "!!! Error: " << a_Description << std::endl << std::endl;
	exit(1);
}

void Memory::Load(const std::string& a_Path, uint32_t a_Address)
{
	std::cout << "Memory: loading " << a_Path << " -> $" << std::hex << a_Address << std::endl;
	// stat to check existance and size
	struct stat file_info;
	if (stat(a_Path.c_str(), &file_info)) {
		error_exit("no such file: " + a_Path);
	}

	int file_size = file_info.st_size;

	int fd = open(a_Path.c_str(), O_RDONLY);
	if (fd < 0) {
		error_exit("could not open file: " + a_Path);
	}

	ssize_t count = 0;
	int pos = a_Address;
	uint8_t* buff = (uint8_t*)malloc(1);
	while (file_size - count > 0) {
		ssize_t result = read(fd, buff, 1);

		if (result == 0) {
			break;
		}
		if (result == -1) {
			error_exit("error reading file: " + a_Path);
		}

		m_Mem[pos] = *buff;
		pos += result;
		count += result;
	}

	free(buff);
	return;
}

void Memory::Show(uint32_t a_Pos, uint32_t a_Length)
{
	std::cout << "Showing 0x" << a_Length << " bytes from " << std::hex << a_Pos << std::endl;
	std::ostringstream chars;

	for (uint32_t i=0; i < a_Length; i++) {
		if ((i % 16) == 0) {
			std::cout << "    " << chars.str() << std::endl << "[" << a_Pos + i << "] " << std::hex;
			chars.str("");
		}

		std::cout << std::setw(2) << std::setfill('0') << (unsigned int)m_Mem[a_Pos + i] << " ";
		if ((m_Mem[a_Pos + i] & 0x7f) >= 32) {
			chars << (char)(m_Mem[a_Pos + i] & 0x7f) << " ";
		}
		else {
			chars << "  ";
		}
	}
	std::cout << std::endl;
}


// " (" << (char)(mem[pos + i] & 0x7f) << ")" << endl;

