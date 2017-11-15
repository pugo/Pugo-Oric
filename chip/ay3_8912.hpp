//   Copyright (C) 2009-2014 by Anders Piniesjö <pugo@pugo.org>

#ifndef AY3_8912_H
#define AY3_8912_H

#include <memory>

class Machine;

class AY3_8912
{
public:
	static const uint8_t c_NumberOfRegisters = 15;
	
	AY3_8912(std::shared_ptr<Machine> a_Machine);
	~AY3_8912();

	void Reset();
	short Exec(uint8_t a_Cycles);
	
	void KeyPress(uint8_t a_KeyBits, bool a_Down);

	void SetBdir(bool a_Value);
	void SetBc1(bool a_Value);
	void SetBc2(bool a_Value);

	static void set_bdir(Machine& a_Machine, bool a_Value);
	static void set_bc1(Machine& a_Machine, bool a_Value);
	static void set_bc2(Machine& a_Machine, bool a_Value);

private:
	std::shared_ptr<Machine> m_Machine;
	std::shared_ptr<Memory> m_Memory;

	bool bdir;
	bool bc1;
	bool bc2;

	uint8_t m_CurrentRegister;
	uint8_t m_Registers[c_NumberOfRegisters];
	uint8_t m_KeyRows[8];
};


#endif // AY3_8912_H
