// Copyright (C) 2009-2016 by Anders Piniesjö <pugo@pugo.org>

#ifndef ORIC_H
#define ORIC_H

#include <iostream>
#include <memory>

#include "machine.h"

class Oric
{
public:
	static std::shared_ptr<Oric> GetInstance() {
		static std::shared_ptr<Oric> instance = std::make_shared<Oric>();
		return instance;
	}
	
	Oric();
	~Oric();
	void Init();

	Machine& GetMachine() { return *m_Machine; }
	void Monitor();

protected:
	bool HandleCommand(std::string& a_Cmd);
	uint16_t StringToWord(std::string& a_Addr);

	std::shared_ptr<Machine> m_Machine;
	std::string m_LastCommand;
};

#endif // ORIC_H
