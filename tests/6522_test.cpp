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

#include "../config.hpp"
#include "../oric.hpp"


namespace Unittest {

using namespace testing;


class MOS6522Test : public ::testing::Test
{
protected:
    virtual void SetUp()
    {
        Config config;

        oric = new Oric(config);
        oric->init_machine();
        oric->get_machine().init_mos6522();

        mos6522 = oric->get_machine().mos_6522;
        mos6522->orb_changed_handler = test_via_orb_changed_callback;
        mos6522->ca2_changed_handler = test_ca2_changed_callback;
        mos6522->cb2_changed_handler = test_cb2_changed_callback;
        mos6522->irq_handler = test_irq_callback;
        mos6522->irq_clear_handler = test_irq_clear_callback;
    }

    virtual void TearDown()
    {
        delete oric;
    }

    void run(Machine& machine) {
        bool brk = false;
        while (! brk) {
            machine.cpu->exec_instruction(brk);
        }
    }

    static void inline test_via_orb_changed_callback(Machine& a_Machine, uint8_t orb)
    {}

    static void inline test_ca2_changed_callback(Machine& a_Machine, bool ca2)
    {}

    static void inline test_cb2_changed_callback(Machine& a_Machine, bool cb2)
    {}

    static void inline test_irq_callback(Machine& a_Machine)
    {}

    static void inline test_irq_clear_callback(Machine& a_Machine)
    {}

    Oric* oric;
    MOS6522* mos6522;
};


// Set and read data direction register A. Expect equal value.
TEST_F(MOS6522Test, WriteReadDDRA)
{
    mos6522->write_byte(MOS6522::DDRA, 0xff);
    ASSERT_EQ(mos6522->read_byte(MOS6522::DDRA), 0xff);
}

// Set data direction to input for all bits. Expect no result from ORA.
TEST_F(MOS6522Test, ReadORAAllInputs)
{
    mos6522->write_byte(MOS6522::DDRA, 0x00);
    mos6522->write_byte(MOS6522::ORA, 0xff);
    ASSERT_EQ(mos6522->read_byte(MOS6522::ORA), 0x00);
}

// Set data direction to output for all bits. Expect result from ORA.
TEST_F(MOS6522Test, ReadORAAllOutputs)
{
    mos6522->write_byte(MOS6522::DDRA, 0xff);
    mos6522->write_byte(MOS6522::ORA, 0xff);
    ASSERT_EQ(mos6522->read_byte(MOS6522::ORA), 0xff);
}

// Set and read data direction register B. Expect equal value.
TEST_F(MOS6522Test, WriteReadDDRB)
{
    mos6522->write_byte(MOS6522::DDRB, 0xff);
    ASSERT_EQ(mos6522->read_byte(MOS6522::DDRB), 0xff);
}

// Set data direction to input for all bits. Expect no result from ORA.
TEST_F(MOS6522Test, ReadORBAllInputs)
{
    mos6522->write_byte(MOS6522::DDRB, 0x00);
    mos6522->write_byte(MOS6522::ORB, 0xff);
    ASSERT_EQ(mos6522->read_byte(MOS6522::ORB), 0x00);
}

// Set data direction to output for all bits. Expect result from ORA.
TEST_F(MOS6522Test, ReadORBAllOutputs)
{
    mos6522->write_byte(MOS6522::DDRB, 0xff);
    mos6522->write_byte(MOS6522::ORB, 0xff);
    ASSERT_EQ(mos6522->read_byte(MOS6522::ORB), 0xff);
}

// Writing T1L_L should not transfer to any other T1 registers.
TEST_F(MOS6522Test, WriteT1L_L)
{
    mos6522->write_byte(MOS6522::T1L_L, 0xee);
    ASSERT_EQ(mos6522->read_byte(MOS6522::T1L_L), 0xee);
    ASSERT_EQ(mos6522->read_byte(MOS6522::T1L_H), 0x00);
    ASSERT_EQ(mos6522->read_byte(MOS6522::T1C_L), 0x00);
    ASSERT_EQ(mos6522->read_byte(MOS6522::T1C_H), 0x00);
}

// Writing T1L_H should not transfer to any other T1 registers.
TEST_F(MOS6522Test, WriteT1L_H)
{
    mos6522->write_byte(MOS6522::T1L_H, 0x44);
    ASSERT_EQ(mos6522->read_byte(MOS6522::T1L_L), 0x00);
    ASSERT_EQ(mos6522->read_byte(MOS6522::T1L_H), 0x44);
    ASSERT_EQ(mos6522->read_byte(MOS6522::T1C_L), 0x00);
    ASSERT_EQ(mos6522->read_byte(MOS6522::T1C_H), 0x00);
}

// Writing T1L_H should clear interrupt.
TEST_F(MOS6522Test, WriteT1L_H_interrupt_clear)
{
    mos6522->write_byte(MOS6522::IER, 0xff);                        // Enable interrupts for bit 0-6.
    mos6522->set_ifr(0x80 | MOS6522::IRQ_T1);

    ASSERT_EQ(mos6522->read_byte(MOS6522::IFR), MOS6522::IRQ_T1 | 0x80);

    mos6522->write_byte(MOS6522::T1L_H, 0x88);
    ASSERT_EQ(mos6522->read_byte(MOS6522::IFR), 0);
}

// Writing both T1L_L and T1L_H should not transfer to any other T1 registers.
TEST_F(MOS6522Test, WriteT1L_LH)
{
    mos6522->write_byte(MOS6522::T1L_L, 0x12);
    mos6522->write_byte(MOS6522::T1L_H, 0x34);
    ASSERT_EQ(mos6522->read_byte(MOS6522::T1L_L), 0x12);
    ASSERT_EQ(mos6522->read_byte(MOS6522::T1L_H), 0x34);
    ASSERT_EQ(mos6522->read_byte(MOS6522::T1C_L), 0x00);
    ASSERT_EQ(mos6522->read_byte(MOS6522::T1C_H), 0x00);
}

// Writing T1C_L should only write to T1L_L.
TEST_F(MOS6522Test, WriteT1C_L)
{
    mos6522->write_byte(MOS6522::T1C_L, 0x77);
    ASSERT_EQ(mos6522->read_byte(MOS6522::T1L_L), 0x77);
    ASSERT_EQ(mos6522->read_byte(MOS6522::T1L_H), 0x00);
    ASSERT_EQ(mos6522->read_byte(MOS6522::T1C_L), 0x00);
    ASSERT_EQ(mos6522->read_byte(MOS6522::T1C_H), 0x00);
}

// Reading T1C_L should clear interrupt.
TEST_F(MOS6522Test, ReadT1C_L_interrupt_clear)
{
    mos6522->write_byte(MOS6522::IER, 0xff);                        // Enable interrupts for bit 0-6.
    mos6522->set_ifr(0x80 | MOS6522::IRQ_T1);

    ASSERT_EQ(mos6522->read_byte(MOS6522::IFR), MOS6522::IRQ_T1 | 0x80);

    mos6522->read_byte(MOS6522::T1C_L);
    ASSERT_EQ(mos6522->read_byte(MOS6522::IFR), 0);
}

// Writing T1C_H should write to T1L_H and T1C_H and transfer T1L_L to T1C_L.
TEST_F(MOS6522Test, WriteT1C_H)
{
    mos6522->write_byte(MOS6522::IER, 0xff);                        // Enable interrupts for bit 0-6.
    mos6522->write_byte(MOS6522::IFR, 0x80 + MOS6522::IRQ_T1);      // Start with T1 interrupt set.

    mos6522->write_byte(MOS6522::T1C_L, 0x11);
    mos6522->write_byte(MOS6522::T1C_H, 0x88);

    ASSERT_EQ(mos6522->read_byte(MOS6522::T1L_L), 0x11);
    ASSERT_EQ(mos6522->read_byte(MOS6522::T1L_H), 0x88);
    ASSERT_EQ(mos6522->read_byte(MOS6522::T1C_L), 0x11);
    ASSERT_EQ(mos6522->read_byte(MOS6522::T1C_H), 0x88);
}

// Writing T1C_H should clear interrupt.
TEST_F(MOS6522Test, WriteT1C_H_interrupt_clear)
{
    mos6522->write_byte(MOS6522::IER, 0xff);                        // Enable interrupts for bit 0-6.
    mos6522->set_ifr(0x80 | MOS6522::IRQ_T1);

    ASSERT_EQ(mos6522->read_byte(MOS6522::IFR), MOS6522::IRQ_T1 | 0x80);

    mos6522->write_byte(MOS6522::T1C_H, 0x88);
    ASSERT_EQ(mos6522->read_byte(MOS6522::IFR), 0);
}

// Writing T2C_L only writes to T2L_L, not T2C_L until transfer.
TEST_F(MOS6522Test, WriteT2C_L)
{
    mos6522->write_byte(MOS6522::T2C_L, 0x99);
    ASSERT_EQ(mos6522->read_byte(MOS6522::T2C_L), 0x00);
    ASSERT_EQ(mos6522->read_byte(MOS6522::T2C_H), 0x00);
}

// Reading T2C_L should clear interrupt.
TEST_F(MOS6522Test, ReadT2C_L_interrupt_clear)
{
    mos6522->write_byte(MOS6522::IER, 0xff);                        // Enable interrupts for bit 0-6.
    mos6522->set_ifr(0x80 | MOS6522::IRQ_T2);

    ASSERT_EQ(mos6522->read_byte(MOS6522::IFR), MOS6522::IRQ_T2 | 0x80);

    mos6522->read_byte(MOS6522::T2C_L);
    ASSERT_EQ(mos6522->read_byte(MOS6522::IFR), 0);
}

// Writing T2C_H transfers T2L_L to T2C_L.
TEST_F(MOS6522Test, WriteT2C_H)
{
    mos6522->write_byte(MOS6522::T2C_L, 0xaa);
    mos6522->write_byte(MOS6522::T2C_H, 0xbb);
    ASSERT_EQ(mos6522->read_byte(MOS6522::T2C_L), 0xaa);
    ASSERT_EQ(mos6522->read_byte(MOS6522::T2C_H), 0xbb);
}

// Writing T2C_H should clear interrupt.
TEST_F(MOS6522Test, WriteT2C_H_interrupt_clear)
{
    mos6522->write_byte(MOS6522::IER, 0xff);                        // Enable interrupts for bit 0-6.
    mos6522->set_ifr(0x80 | MOS6522::IRQ_T2);

    ASSERT_EQ(mos6522->read_byte(MOS6522::IFR), MOS6522::IRQ_T2 | 0x80);

    mos6522->write_byte(MOS6522::T2C_H, 0x88);
    ASSERT_EQ(mos6522->read_byte(MOS6522::IFR), 0);
}

TEST_F(MOS6522Test, WriteSR)
{
    mos6522->write_byte(MOS6522::SR, 0xaa);
    ASSERT_EQ(mos6522->read_byte(MOS6522::SR), 0xaa);
}

TEST_F(MOS6522Test, WriteACR)
{
    mos6522->write_byte(MOS6522::ACR, 0xbb);
    ASSERT_EQ(mos6522->read_byte(MOS6522::ACR), 0xbb);
}

TEST_F(MOS6522Test, WritePCR)
{
    mos6522->write_byte(MOS6522::PCR, 0xcc);
    ASSERT_EQ(mos6522->read_byte(MOS6522::PCR), 0xcc);
}

} // Unittest