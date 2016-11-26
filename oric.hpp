// Copyright (C) 2009-2016 by Anders Piniesjö <pugo@pugo.org>

#ifndef ORIC_H
#define ORIC_H

#include <iostream>
#include <memory>

#include "machine.hpp"

class Frontend;

class Oric : public std::enable_shared_from_this<Oric>
{
public:
	static std::shared_ptr<Oric> GetInstance() {
		static std::shared_ptr<Oric> instance = std::make_shared<Oric>();
		return instance;
	}
	
	Oric();
	~Oric();

	void Init();

	std::shared_ptr<Machine> GetMachine() { return m_Machine; }
	std::shared_ptr<Frontend> GetFrontend() { return m_Frontend; }
	void Monitor();

protected:
	bool HandleCommand(std::string& a_Cmd);
	uint16_t StringToWord(std::string& a_Addr);

	std::shared_ptr<Frontend> m_Frontend;
	std::shared_ptr<Machine> m_Machine;
	std::string m_LastCommand;
};

#endif // ORIC_H
