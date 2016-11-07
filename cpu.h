//   Copyright (C) 2009-2016 by Anders Piniesjö <pugo@pugo.org>

#ifndef CPU_H
#define CPU_H

#include "memory.h"


class CPU
{
public:
	CPU();
	~CPU();


	virtual bool execInstructionCycles(int cycles) = 0;
	virtual short execInstruction(bool& brk) = 0;

	Memory* memory;

protected:
};

#endif
