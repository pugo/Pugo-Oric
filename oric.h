// Copyright (C) 2009-2016 by Anders Piniesjö <pugo@pugo.org>

#ifndef ORIC_H
#define ORIC_H

#include <iostream>

#include "machine.h"

class Oric
{
public:
	Oric();
	~Oric();

	Machine& GetMachine() { return *m_Machine; }
	void Monitor();

protected:
	bool HandleCommand(std::string& a_Cmd);
	uint16_t StringToWord(std::string& a_Addr);

	Machine* m_Machine;
	std::string m_LastCommand;
};

#endif // ORIC_H
