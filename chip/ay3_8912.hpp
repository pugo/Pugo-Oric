//   Copyright (C) 2009-2018 by Anders Piniesjö <pugo@pugo.org>

#ifndef AY3_8912_H
#define AY3_8912_H

#include <memory>
#include <machine.hpp>

typedef uint8_t (*f_read_data_handler)(Machine &oric);


class AY3_8912
{
public:
	enum Register
	{
		CH_A_PITCH_LOW = 0,
		CH_A_PITCH_HIGH,
		CH_B_PITCH_LOW,
		CH_B_PITCH_HIGH,
		CH_C_PITCH_LOW,
		CH_C_PITCH_HIGH,
		NOICE_PITCH,
		MIXER,
		CH_A_VOLUME,
		CH_B_VOLUME,
		CH_C_VOLUME,
		ENV_DURATION_LOW,
		ENV_DURATION_HIGH,
		ENV_SHAPE,
		IO_PORT_A,
		NUM_REGS
	};

	AY3_8912(Machine& machine);
	~AY3_8912();

	void reset();
	short exec(uint8_t cycles);
	
	void set_bdir(bool value);
	void set_bc1(bool value);
	void set_bc2(bool value);

	uint8_t get_register(Register a_Register) { return registers[a_Register]; }

	static void set_bdir(Machine& machine, bool a_Value);
	static void set_bc1(Machine& machine, bool a_Value);
	static void set_bc2(Machine& machine, bool a_Value);

	f_read_data_handler m_read_data_handler;

private:
	Machine& machine;

	bool bdir;
	bool bc1;
	bool bc2;

	uint8_t current_register;
	uint8_t registers[NUM_REGS];
};


#endif // AY3_8912_H
