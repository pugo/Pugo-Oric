// =========================================================================
//   Copyright (C) 2009-2024 by Anders Piniesjö <pugo@pugo.org>
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

#include <memory>
#include <gtest/gtest.h>

#include "6522_test.hpp"

namespace Unittest {

using namespace testing;

class MOS6522TestSR : public MOS6522Test
{};


// ------ T1 counter ----------

TEST_F(MOS6522TestSR, T1_shift_out_by_t2)
{
    mos6522->write_byte(MOS6522::ACR, 0x14);    // Shift out under control of T2
    mos6522->write_byte(MOS6522::IER, 0xff);    // Enable interrupts for bit 0-6.
    mos6522->write_byte(MOS6522::T2C_L, 0x03);
    mos6522->write_byte(MOS6522::SR, 0xaa);

    ASSERT_EQ(mos6522->get_state().cb2, false);

    // Count down to 0. But first time it is positive transition. No shift.
    for (uint8_t i = 0; i < 4; i++) {
        mos6522->exec();
        ASSERT_EQ(mos6522->get_state().cb2, false);
    }

    // Count down again, but now it is negative transition. Shift!
    for (uint8_t i = 0; i < 3; i++) {
        mos6522->exec();
        ASSERT_EQ(mos6522->get_state().cb2, false);
    }

    mos6522->exec();  // Initial load
    ASSERT_EQ(mos6522->get_state().cb2, true);
}

TEST_F(MOS6522TestSR, T1_shift_out_by_o2)
{
    mos6522->write_byte(MOS6522::ACR, 0x18);    // Shift out under control of T2
    mos6522->write_byte(MOS6522::IER, 0xff);    // Enable interrupts for bit 0-6.
    mos6522->write_byte(MOS6522::SR, 0xaa);
    mos6522->get_state().cb1 = false;


    mos6522->exec();
    ASSERT_EQ(mos6522->get_state().cb2, false); // Positive transition, no shift

    mos6522->exec();
    ASSERT_EQ(mos6522->get_state().cb2, true);  // Negative transition, shift
}

} // Unittest