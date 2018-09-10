// Copyright (C) 2009-2016 by Anders Piniesjö <pugo@pugo.org>

#ifndef ORIC_H
#define ORIC_H

#include <iostream>
#include <memory>

#include "machine.hpp"

class Frontend;

class Oric
{
public:
	enum State
	{
		STATE_RUN,
		STATE_MON,
		STATE_QUIT
	};

	static Oric& GetInstance()
	{
		static Oric instance;
		return instance;
	}
	
	Oric();
	~Oric();

	void Init();

	Machine& GetMachine() { return *m_Machine; }
	Frontend& GetFrontend() { return *m_Frontend; }

	void Run();
	void Break();

protected:
	State HandleCommand(std::string& a_Cmd);
	uint16_t StringToWord(std::string& a_Addr);

	State m_State;
	Frontend* m_Frontend;
	Machine* m_Machine;
	std::string m_LastCommand;
};

#endif // ORIC_H
