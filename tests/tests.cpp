/* Generated file, do not edit */

#ifndef CXXTEST_RUNNING
#define CXXTEST_RUNNING
#endif

#define _CXXTEST_HAVE_STD
#include <cxxtest/TestListener.h>
#include <cxxtest/TestTracker.h>
#include <cxxtest/TestRunner.h>
#include <cxxtest/RealDescriptions.h>
#include <cxxtest/ErrorPrinter.h>

int main() {
 return CxxTest::ErrorPrinter().run();
}
#include "tests/test1.h"

static MemoryTestSuite suite_MemoryTestSuite;

static CxxTest::List Tests_MemoryTestSuite = { 0, 0 };
CxxTest::StaticSuiteDescription suiteDescription_MemoryTestSuite( "tests/test1.h", 9, "MemoryTestSuite", suite_MemoryTestSuite, Tests_MemoryTestSuite );

static class TestDescription_MemoryTestSuite_testMemoryCreateSize : public CxxTest::RealTestDescription {
public:
 TestDescription_MemoryTestSuite_testMemoryCreateSize() : CxxTest::RealTestDescription( Tests_MemoryTestSuite, suiteDescription_MemoryTestSuite, 15, "testMemoryCreateSize" ) {}
 void runTest() { suite_MemoryTestSuite.testMemoryCreateSize(); }
} testDescription_MemoryTestSuite_testMemoryCreateSize;

static class TestDescription_MemoryTestSuite_testMemoryCreateEmpty : public CxxTest::RealTestDescription {
public:
 TestDescription_MemoryTestSuite_testMemoryCreateEmpty() : CxxTest::RealTestDescription( Tests_MemoryTestSuite, suiteDescription_MemoryTestSuite, 22, "testMemoryCreateEmpty" ) {}
 void runTest() { suite_MemoryTestSuite.testMemoryCreateEmpty(); }
} testDescription_MemoryTestSuite_testMemoryCreateEmpty;

static class TestDescription_MemoryTestSuite_testMemoryLoad : public CxxTest::RealTestDescription {
public:
 TestDescription_MemoryTestSuite_testMemoryLoad() : CxxTest::RealTestDescription( Tests_MemoryTestSuite, suiteDescription_MemoryTestSuite, 33, "testMemoryLoad" ) {}
 void runTest() { suite_MemoryTestSuite.testMemoryLoad(); }
} testDescription_MemoryTestSuite_testMemoryLoad;

static MOS6502TestSuite suite_MOS6502TestSuite;

static CxxTest::List Tests_MOS6502TestSuite = { 0, 0 };
CxxTest::StaticSuiteDescription suiteDescription_MOS6502TestSuite( "tests/test1.h", 47, "MOS6502TestSuite", suite_MOS6502TestSuite, Tests_MOS6502TestSuite );

static class TestDescription_MOS6502TestSuite_testCPUCreate : public CxxTest::RealTestDescription {
public:
 TestDescription_MOS6502TestSuite_testCPUCreate() : CxxTest::RealTestDescription( Tests_MOS6502TestSuite, suiteDescription_MOS6502TestSuite, 50, "testCPUCreate" ) {}
 void runTest() { suite_MOS6502TestSuite.testCPUCreate(); }
} testDescription_MOS6502TestSuite_testCPUCreate;

static class TestDescription_MOS6502TestSuite_testOpLDA_IMM : public CxxTest::RealTestDescription {
public:
 TestDescription_MOS6502TestSuite_testOpLDA_IMM() : CxxTest::RealTestDescription( Tests_MOS6502TestSuite, suiteDescription_MOS6502TestSuite, 67, "testOpLDA_IMM" ) {}
 void runTest() { suite_MOS6502TestSuite.testOpLDA_IMM(); }
} testDescription_MOS6502TestSuite_testOpLDA_IMM;

static class TestDescription_MOS6502TestSuite_testOpLDA_ZP : public CxxTest::RealTestDescription {
public:
 TestDescription_MOS6502TestSuite_testOpLDA_ZP() : CxxTest::RealTestDescription( Tests_MOS6502TestSuite, suiteDescription_MOS6502TestSuite, 80, "testOpLDA_ZP" ) {}
 void runTest() { suite_MOS6502TestSuite.testOpLDA_ZP(); }
} testDescription_MOS6502TestSuite_testOpLDA_ZP;

static class TestDescription_MOS6502TestSuite_testOpLDA_ZP_X : public CxxTest::RealTestDescription {
public:
 TestDescription_MOS6502TestSuite_testOpLDA_ZP_X() : CxxTest::RealTestDescription( Tests_MOS6502TestSuite, suiteDescription_MOS6502TestSuite, 94, "testOpLDA_ZP_X" ) {}
 void runTest() { suite_MOS6502TestSuite.testOpLDA_ZP_X(); }
} testDescription_MOS6502TestSuite_testOpLDA_ZP_X;

static class TestDescription_MOS6502TestSuite_testOpLDA_ABS : public CxxTest::RealTestDescription {
public:
 TestDescription_MOS6502TestSuite_testOpLDA_ABS() : CxxTest::RealTestDescription( Tests_MOS6502TestSuite, suiteDescription_MOS6502TestSuite, 109, "testOpLDA_ABS" ) {}
 void runTest() { suite_MOS6502TestSuite.testOpLDA_ABS(); }
} testDescription_MOS6502TestSuite_testOpLDA_ABS;

static class TestDescription_MOS6502TestSuite_testOpLDA_ABS_X : public CxxTest::RealTestDescription {
public:
 TestDescription_MOS6502TestSuite_testOpLDA_ABS_X() : CxxTest::RealTestDescription( Tests_MOS6502TestSuite, suiteDescription_MOS6502TestSuite, 123, "testOpLDA_ABS_X" ) {}
 void runTest() { suite_MOS6502TestSuite.testOpLDA_ABS_X(); }
} testDescription_MOS6502TestSuite_testOpLDA_ABS_X;

static class TestDescription_MOS6502TestSuite_testOpLDA_ABS_Y : public CxxTest::RealTestDescription {
public:
 TestDescription_MOS6502TestSuite_testOpLDA_ABS_Y() : CxxTest::RealTestDescription( Tests_MOS6502TestSuite, suiteDescription_MOS6502TestSuite, 138, "testOpLDA_ABS_Y" ) {}
 void runTest() { suite_MOS6502TestSuite.testOpLDA_ABS_Y(); }
} testDescription_MOS6502TestSuite_testOpLDA_ABS_Y;

static class TestDescription_MOS6502TestSuite_testOpLDA_IND_X : public CxxTest::RealTestDescription {
public:
 TestDescription_MOS6502TestSuite_testOpLDA_IND_X() : CxxTest::RealTestDescription( Tests_MOS6502TestSuite, suiteDescription_MOS6502TestSuite, 154, "testOpLDA_IND_X" ) {}
 void runTest() { suite_MOS6502TestSuite.testOpLDA_IND_X(); }
} testDescription_MOS6502TestSuite_testOpLDA_IND_X;

static class TestDescription_MOS6502TestSuite_testOpLDA_IND_Y : public CxxTest::RealTestDescription {
public:
 TestDescription_MOS6502TestSuite_testOpLDA_IND_Y() : CxxTest::RealTestDescription( Tests_MOS6502TestSuite, suiteDescription_MOS6502TestSuite, 172, "testOpLDA_IND_Y" ) {}
 void runTest() { suite_MOS6502TestSuite.testOpLDA_IND_Y(); }
} testDescription_MOS6502TestSuite_testOpLDA_IND_Y;

static class TestDescription_MOS6502TestSuite_testOpLDX_IMM : public CxxTest::RealTestDescription {
public:
 TestDescription_MOS6502TestSuite_testOpLDX_IMM() : CxxTest::RealTestDescription( Tests_MOS6502TestSuite, suiteDescription_MOS6502TestSuite, 192, "testOpLDX_IMM" ) {}
 void runTest() { suite_MOS6502TestSuite.testOpLDX_IMM(); }
} testDescription_MOS6502TestSuite_testOpLDX_IMM;

static class TestDescription_MOS6502TestSuite_testOpLDX_ZP : public CxxTest::RealTestDescription {
public:
 TestDescription_MOS6502TestSuite_testOpLDX_ZP() : CxxTest::RealTestDescription( Tests_MOS6502TestSuite, suiteDescription_MOS6502TestSuite, 205, "testOpLDX_ZP" ) {}
 void runTest() { suite_MOS6502TestSuite.testOpLDX_ZP(); }
} testDescription_MOS6502TestSuite_testOpLDX_ZP;

static class TestDescription_MOS6502TestSuite_testOpLDX_ZP_Y : public CxxTest::RealTestDescription {
public:
 TestDescription_MOS6502TestSuite_testOpLDX_ZP_Y() : CxxTest::RealTestDescription( Tests_MOS6502TestSuite, suiteDescription_MOS6502TestSuite, 219, "testOpLDX_ZP_Y" ) {}
 void runTest() { suite_MOS6502TestSuite.testOpLDX_ZP_Y(); }
} testDescription_MOS6502TestSuite_testOpLDX_ZP_Y;

static class TestDescription_MOS6502TestSuite_testOpLDX_ABS : public CxxTest::RealTestDescription {
public:
 TestDescription_MOS6502TestSuite_testOpLDX_ABS() : CxxTest::RealTestDescription( Tests_MOS6502TestSuite, suiteDescription_MOS6502TestSuite, 234, "testOpLDX_ABS" ) {}
 void runTest() { suite_MOS6502TestSuite.testOpLDX_ABS(); }
} testDescription_MOS6502TestSuite_testOpLDX_ABS;

static class TestDescription_MOS6502TestSuite_testOpLDX_ABS_Y : public CxxTest::RealTestDescription {
public:
 TestDescription_MOS6502TestSuite_testOpLDX_ABS_Y() : CxxTest::RealTestDescription( Tests_MOS6502TestSuite, suiteDescription_MOS6502TestSuite, 248, "testOpLDX_ABS_Y" ) {}
 void runTest() { suite_MOS6502TestSuite.testOpLDX_ABS_Y(); }
} testDescription_MOS6502TestSuite_testOpLDX_ABS_Y;

static class TestDescription_MOS6502TestSuite_testOpLDY_IMM : public CxxTest::RealTestDescription {
public:
 TestDescription_MOS6502TestSuite_testOpLDY_IMM() : CxxTest::RealTestDescription( Tests_MOS6502TestSuite, suiteDescription_MOS6502TestSuite, 265, "testOpLDY_IMM" ) {}
 void runTest() { suite_MOS6502TestSuite.testOpLDY_IMM(); }
} testDescription_MOS6502TestSuite_testOpLDY_IMM;

static class TestDescription_MOS6502TestSuite_testOpLDY_ZP : public CxxTest::RealTestDescription {
public:
 TestDescription_MOS6502TestSuite_testOpLDY_ZP() : CxxTest::RealTestDescription( Tests_MOS6502TestSuite, suiteDescription_MOS6502TestSuite, 278, "testOpLDY_ZP" ) {}
 void runTest() { suite_MOS6502TestSuite.testOpLDY_ZP(); }
} testDescription_MOS6502TestSuite_testOpLDY_ZP;

static class TestDescription_MOS6502TestSuite_testOpLDY_ZP_X : public CxxTest::RealTestDescription {
public:
 TestDescription_MOS6502TestSuite_testOpLDY_ZP_X() : CxxTest::RealTestDescription( Tests_MOS6502TestSuite, suiteDescription_MOS6502TestSuite, 292, "testOpLDY_ZP_X" ) {}
 void runTest() { suite_MOS6502TestSuite.testOpLDY_ZP_X(); }
} testDescription_MOS6502TestSuite_testOpLDY_ZP_X;

static class TestDescription_MOS6502TestSuite_testOpLDY_ABS : public CxxTest::RealTestDescription {
public:
 TestDescription_MOS6502TestSuite_testOpLDY_ABS() : CxxTest::RealTestDescription( Tests_MOS6502TestSuite, suiteDescription_MOS6502TestSuite, 307, "testOpLDY_ABS" ) {}
 void runTest() { suite_MOS6502TestSuite.testOpLDY_ABS(); }
} testDescription_MOS6502TestSuite_testOpLDY_ABS;

static class TestDescription_MOS6502TestSuite_testOpLDY_ABS_X : public CxxTest::RealTestDescription {
public:
 TestDescription_MOS6502TestSuite_testOpLDY_ABS_X() : CxxTest::RealTestDescription( Tests_MOS6502TestSuite, suiteDescription_MOS6502TestSuite, 321, "testOpLDY_ABS_X" ) {}
 void runTest() { suite_MOS6502TestSuite.testOpLDY_ABS_X(); }
} testDescription_MOS6502TestSuite_testOpLDY_ABS_X;

static class TestDescription_MOS6502TestSuite_testOpSTA_ZP : public CxxTest::RealTestDescription {
public:
 TestDescription_MOS6502TestSuite_testOpSTA_ZP() : CxxTest::RealTestDescription( Tests_MOS6502TestSuite, suiteDescription_MOS6502TestSuite, 340, "testOpSTA_ZP" ) {}
 void runTest() { suite_MOS6502TestSuite.testOpSTA_ZP(); }
} testDescription_MOS6502TestSuite_testOpSTA_ZP;

static class TestDescription_MOS6502TestSuite_testOpSTA_ZP_X : public CxxTest::RealTestDescription {
public:
 TestDescription_MOS6502TestSuite_testOpSTA_ZP_X() : CxxTest::RealTestDescription( Tests_MOS6502TestSuite, suiteDescription_MOS6502TestSuite, 354, "testOpSTA_ZP_X" ) {}
 void runTest() { suite_MOS6502TestSuite.testOpSTA_ZP_X(); }
} testDescription_MOS6502TestSuite_testOpSTA_ZP_X;

static class TestDescription_MOS6502TestSuite_testOpSTA_ABS : public CxxTest::RealTestDescription {
public:
 TestDescription_MOS6502TestSuite_testOpSTA_ABS() : CxxTest::RealTestDescription( Tests_MOS6502TestSuite, suiteDescription_MOS6502TestSuite, 369, "testOpSTA_ABS" ) {}
 void runTest() { suite_MOS6502TestSuite.testOpSTA_ABS(); }
} testDescription_MOS6502TestSuite_testOpSTA_ABS;

static class TestDescription_MOS6502TestSuite_testOpSTA_ABS_X : public CxxTest::RealTestDescription {
public:
 TestDescription_MOS6502TestSuite_testOpSTA_ABS_X() : CxxTest::RealTestDescription( Tests_MOS6502TestSuite, suiteDescription_MOS6502TestSuite, 383, "testOpSTA_ABS_X" ) {}
 void runTest() { suite_MOS6502TestSuite.testOpSTA_ABS_X(); }
} testDescription_MOS6502TestSuite_testOpSTA_ABS_X;

static class TestDescription_MOS6502TestSuite_testOpSTA_ABS_Y : public CxxTest::RealTestDescription {
public:
 TestDescription_MOS6502TestSuite_testOpSTA_ABS_Y() : CxxTest::RealTestDescription( Tests_MOS6502TestSuite, suiteDescription_MOS6502TestSuite, 398, "testOpSTA_ABS_Y" ) {}
 void runTest() { suite_MOS6502TestSuite.testOpSTA_ABS_Y(); }
} testDescription_MOS6502TestSuite_testOpSTA_ABS_Y;

static class TestDescription_MOS6502TestSuite_testOpSTA_IND_X : public CxxTest::RealTestDescription {
public:
 TestDescription_MOS6502TestSuite_testOpSTA_IND_X() : CxxTest::RealTestDescription( Tests_MOS6502TestSuite, suiteDescription_MOS6502TestSuite, 414, "testOpSTA_IND_X" ) {}
 void runTest() { suite_MOS6502TestSuite.testOpSTA_IND_X(); }
} testDescription_MOS6502TestSuite_testOpSTA_IND_X;

static class TestDescription_MOS6502TestSuite_testOpSTA_IND_Y : public CxxTest::RealTestDescription {
public:
 TestDescription_MOS6502TestSuite_testOpSTA_IND_Y() : CxxTest::RealTestDescription( Tests_MOS6502TestSuite, suiteDescription_MOS6502TestSuite, 432, "testOpSTA_IND_Y" ) {}
 void runTest() { suite_MOS6502TestSuite.testOpSTA_IND_Y(); }
} testDescription_MOS6502TestSuite_testOpSTA_IND_Y;

static class TestDescription_MOS6502TestSuite_testOpSTX_ZP : public CxxTest::RealTestDescription {
public:
 TestDescription_MOS6502TestSuite_testOpSTX_ZP() : CxxTest::RealTestDescription( Tests_MOS6502TestSuite, suiteDescription_MOS6502TestSuite, 453, "testOpSTX_ZP" ) {}
 void runTest() { suite_MOS6502TestSuite.testOpSTX_ZP(); }
} testDescription_MOS6502TestSuite_testOpSTX_ZP;

static class TestDescription_MOS6502TestSuite_testOpSTX_ZP_Y : public CxxTest::RealTestDescription {
public:
 TestDescription_MOS6502TestSuite_testOpSTX_ZP_Y() : CxxTest::RealTestDescription( Tests_MOS6502TestSuite, suiteDescription_MOS6502TestSuite, 467, "testOpSTX_ZP_Y" ) {}
 void runTest() { suite_MOS6502TestSuite.testOpSTX_ZP_Y(); }
} testDescription_MOS6502TestSuite_testOpSTX_ZP_Y;

static class TestDescription_MOS6502TestSuite_testOpSTX_ABS : public CxxTest::RealTestDescription {
public:
 TestDescription_MOS6502TestSuite_testOpSTX_ABS() : CxxTest::RealTestDescription( Tests_MOS6502TestSuite, suiteDescription_MOS6502TestSuite, 482, "testOpSTX_ABS" ) {}
 void runTest() { suite_MOS6502TestSuite.testOpSTX_ABS(); }
} testDescription_MOS6502TestSuite_testOpSTX_ABS;

static class TestDescription_MOS6502TestSuite_testOpSTY_ZP : public CxxTest::RealTestDescription {
public:
 TestDescription_MOS6502TestSuite_testOpSTY_ZP() : CxxTest::RealTestDescription( Tests_MOS6502TestSuite, suiteDescription_MOS6502TestSuite, 499, "testOpSTY_ZP" ) {}
 void runTest() { suite_MOS6502TestSuite.testOpSTY_ZP(); }
} testDescription_MOS6502TestSuite_testOpSTY_ZP;

static class TestDescription_MOS6502TestSuite_testOpSTY_ZP_X : public CxxTest::RealTestDescription {
public:
 TestDescription_MOS6502TestSuite_testOpSTY_ZP_X() : CxxTest::RealTestDescription( Tests_MOS6502TestSuite, suiteDescription_MOS6502TestSuite, 513, "testOpSTY_ZP_X" ) {}
 void runTest() { suite_MOS6502TestSuite.testOpSTY_ZP_X(); }
} testDescription_MOS6502TestSuite_testOpSTY_ZP_X;

static class TestDescription_MOS6502TestSuite_testOpSTY_ABS : public CxxTest::RealTestDescription {
public:
 TestDescription_MOS6502TestSuite_testOpSTY_ABS() : CxxTest::RealTestDescription( Tests_MOS6502TestSuite, suiteDescription_MOS6502TestSuite, 528, "testOpSTY_ABS" ) {}
 void runTest() { suite_MOS6502TestSuite.testOpSTY_ABS(); }
} testDescription_MOS6502TestSuite_testOpSTY_ABS;

#include <cxxtest/Root.cpp>
