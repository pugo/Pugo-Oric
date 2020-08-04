// =========================================================================
//   Copyright (C) 2009-2014 by Anders Piniesjö <pugo@pugo.org>
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

#ifndef TAPE_TAP_H
#define TAPE_TAP_H

#include <iostream>
#include <memory>
#include <map>
#include "chip/mos6522.hpp"

class TapeTap
{
public:
    TapeTap(MOS6522& a_Via, const std::string& a_Path);
	virtual ~TapeTap();

    bool Init();
	void Reset();

    void PrintStat();

    void SetMotor(bool a_MotorOn);
    short Exec(uint8_t a_Cycles);

protected:
    bool ReadHeader();
    uint8_t CurrentBit();

    std::string m_Path;
    MOS6522& m_Via;
    size_t m_Size;
    size_t m_BodyStart;

    bool m_RunMotor;
    int32_t m_Delay;
    int32_t m_DuplicateBytes;

    uint32_t m_TapePos;
    uint8_t m_BitCount;
    uint8_t m_CurrentBit;
    uint8_t m_Parity;

    int16_t m_TapeCyclesCounter;
    uint8_t m_TapePulse;

    uint8_t* m_Data;

    static const int Pulse_1 = 208;
    static const int Pulse_0 = 416;
};

#endif // TAPE_TAP_H
