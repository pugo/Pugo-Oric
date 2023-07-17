// =========================================================================
//   Copyright (C) 2009-2023 by Anders Piniesjö <pugo@pugo.org>
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
		CH_A_PERIOD_LOW = 0,
		CH_A_PERIOD_HIGH,
		CH_B_PERIOD_LOW,
		CH_B_PERIOD_HIGH,
		CH_C_PERIOD_LOW,
		CH_C_PERIOD_HIGH,
		NOICE_PERIOD,
		ENABLE,
		CH_A_AMPLITUDE,
		CH_B_AMPLITUDE,
		CH_C_AMPLITUDE,
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
    inline void write_to_psg(uint8_t value);

	Machine& machine;

	bool bdir;
	bool bc1;
	bool bc2;

	uint8_t current_register;
	uint8_t registers[NUM_REGS];

    uint16_t tone_period[3];
    uint8_t noise_period;

    uint16_t volumes[3];
};


#endif // AY3_8912_H
