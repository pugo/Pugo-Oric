//   Copyright (C) 2009-2014 by Anders Piniesjö <pugo@pugo.org>

#ifndef AY3_8912_H
#define AY3_8912_H

#include <memory>

class Machine;

class AY3_8912
{
public:
	AY3_8912(std::shared_ptr<Machine> a_Machine);
	~AY3_8912();

	void Reset();
	short Exec(uint8_t a_Cycles);
	
	void KeyPress(uint8_t a_KeyBits, bool a_Down);

private:
	std::shared_ptr<Machine> m_Machine;
	std::shared_ptr<Memory> m_Memory;
};


#endif // AY3_8912_H
