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

	static Oric& get_instance()
	{
		static Oric instance;
		return instance;
	}
	
	Oric();
	~Oric();

	void init();

	Machine& get_machine() { return *machine; }
	Frontend& get_frontend() { return *frontend; }

	void run();
	void do_break();

protected:
	State handle_command(std::string& a_Cmd);
	uint16_t string_to_word(std::string& a_Addr);

	State state;
	Frontend* frontend;
	Machine* machine;
	std::string last_command;
};

#endif // ORIC_H
