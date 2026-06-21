// =========================================================================
//   Copyright (C) 2009-2026 by Anders Piniesjö <pugo@pugo.org>
// =========================================================================

#include <gtest/gtest.h>

#include "config.hpp"
#include "debugger_controller.hpp"
#include "machine.hpp"
#include "oric.hpp"

class DebuggerControllerTest : public ::testing::Test
{
protected:
    DebuggerControllerTest() :
        oric(config)
    {
        oric.init_machine();
        machine = &oric.get_machine();
        machine->init_ram();
        machine->init_cpu();
        machine->init_mos6522();
        machine->init_ay3();
        controller = std::make_unique<DebuggerController>(*machine);
    }

    Config config;
    Oric oric;
    Machine* machine{nullptr};
    std::unique_ptr<DebuggerController> controller;
};

TEST_F(DebuggerControllerTest, HelpReturnsCommandList)
{
    auto result = controller->execute("h");

    EXPECT_EQ(result.action, DebuggerController::Action::Stay);
    EXPECT_NE(result.output.find("Available monitor commands"), std::string::npos);
    EXPECT_NE(result.output.find("bs <address>"), std::string::npos);
}

TEST_F(DebuggerControllerTest, EmptyCommandRepeatsLastCommand)
{
    auto first = controller->execute("h");
    auto second = controller->execute("");

    EXPECT_EQ(second.action, DebuggerController::Action::Stay);
    EXPECT_EQ(second.output, first.output);
}

TEST_F(DebuggerControllerTest, SetBreakpointReturnsConfirmation)
{
    auto result = controller->execute("bs c000");

    EXPECT_EQ(result.action, DebuggerController::Action::Stay);
    EXPECT_NE(result.output.find("Set breakpoint at $C000"), std::string::npos);
}

TEST_F(DebuggerControllerTest, BreakReturnsBreakAction)
{
    auto result = controller->execute("b");

    EXPECT_EQ(result.action, DebuggerController::Action::Break);
    EXPECT_NE(result.output.find("Exec break"), std::string::npos);
}

TEST_F(DebuggerControllerTest, SetProgramCounterReturnsStatus)
{
    auto result = controller->execute("pc c000");

    EXPECT_EQ(result.action, DebuggerController::Action::Stay);
    EXPECT_EQ(machine->cpu->get_pc(), 0xc000);
    EXPECT_NE(result.output.find("[A:"), std::string::npos);
}

TEST_F(DebuggerControllerTest, GoContinues)
{
    auto result = controller->execute("g");

    EXPECT_EQ(result.action, DebuggerController::Action::Continue);
}

TEST_F(DebuggerControllerTest, GoWithAddressSetsProgramCounter)
{
    auto result = controller->execute("g 1f00");

    EXPECT_EQ(result.action, DebuggerController::Action::Continue);
    EXPECT_EQ(machine->cpu->get_pc(), 0x1f00);
}

TEST_F(DebuggerControllerTest, MissingArgumentsStayInDebugger)
{
    EXPECT_NE(controller->execute("bs").output.find("missing address"), std::string::npos);
    EXPECT_NE(controller->execute("pc").output.find("missing address"), std::string::npos);
    EXPECT_NE(controller->execute("m c000").output.find("Use: m"), std::string::npos);
    EXPECT_NE(controller->execute("d c000").output.find("Use: d"), std::string::npos);
}

TEST_F(DebuggerControllerTest, UnknownCommandReturnsError)
{
    auto result = controller->execute("wat");

    EXPECT_EQ(result.action, DebuggerController::Action::Stay);
    EXPECT_NE(result.output.find("Unknown command \"wat\""), std::string::npos);
}

TEST_F(DebuggerControllerTest, OricBreakAndContinueCommandsChangeState)
{
    auto break_result = oric.submit_debugger_command("b");

    EXPECT_EQ(break_result.action, DebuggerController::Action::Break);
    EXPECT_TRUE(oric.is_halted());

    auto continue_result = oric.submit_debugger_command("g");

    EXPECT_EQ(continue_result.action, DebuggerController::Action::Continue);
    EXPECT_FALSE(oric.is_halted());
}
