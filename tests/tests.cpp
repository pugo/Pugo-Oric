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
CxxTest::StaticSuiteDescription suiteDescription_MOS6502TestSuite( "tests/test1.h", 68, "MOS6502TestSuite", suite_MOS6502TestSuite, Tests_MOS6502TestSuite );

static class TestDescription_MOS6502TestSuite_testCPUCreate : public CxxTest::RealTestDescription {
public:
 TestDescription_MOS6502TestSuite_testCPUCreate() : CxxTest::RealTestDescription( Tests_MOS6502TestSuite, suiteDescription_MOS6502TestSuite, 71, "testCPUCreate" ) {}
 void runTest() { suite_MOS6502TestSuite.testCPUCreate(); }
} testDescription_MOS6502TestSuite_testCPUCreate;

static class TestDescription_MOS6502TestSuite_testOpLDA_IMM : public CxxTest::RealTestDescription {
public:
 TestDescription_MOS6502TestSuite_testOpLDA_IMM() : CxxTest::RealTestDescription( Tests_MOS6502TestSuite, suiteDescription_MOS6502TestSuite, 88, "testOpLDA_IMM" ) {}
 void runTest() { suite_MOS6502TestSuite.testOpLDA_IMM(); }
} testDescription_MOS6502TestSuite_testOpLDA_IMM;

static class TestDescription_MOS6502TestSuite_testOpLDA_ZP : public CxxTest::RealTestDescription {
public:
 TestDescription_MOS6502TestSuite_testOpLDA_ZP() : CxxTest::RealTestDescription( Tests_MOS6502TestSuite, suiteDescription_MOS6502TestSuite, 101, "testOpLDA_ZP" ) {}
 void runTest() { suite_MOS6502TestSuite.testOpLDA_ZP(); }
} testDescription_MOS6502TestSuite_testOpLDA_ZP;

static class TestDescription_MOS6502TestSuite_testOpLDA_ZP_X : public CxxTest::RealTestDescription {
public:
 TestDescription_MOS6502TestSuite_testOpLDA_ZP_X() : CxxTest::RealTestDescription( Tests_MOS6502TestSuite, suiteDescription_MOS6502TestSuite, 115, "testOpLDA_ZP_X" ) {}
 void runTest() { suite_MOS6502TestSuite.testOpLDA_ZP_X(); }
} testDescription_MOS6502TestSuite_testOpLDA_ZP_X;

static class TestDescription_MOS6502TestSuite_testOpLDA_ABS : public CxxTest::RealTestDescription {
public:
 TestDescription_MOS6502TestSuite_testOpLDA_ABS() : CxxTest::RealTestDescription( Tests_MOS6502TestSuite, suiteDescription_MOS6502TestSuite, 130, "testOpLDA_ABS" ) {}
 void runTest() { suite_MOS6502TestSuite.testOpLDA_ABS(); }
} testDescription_MOS6502TestSuite_testOpLDA_ABS;

static class TestDescription_MOS6502TestSuite_testOpLDA_ABS_X : public CxxTest::RealTestDescription {
public:
 TestDescription_MOS6502TestSuite_testOpLDA_ABS_X() : CxxTest::RealTestDescription( Tests_MOS6502TestSuite, suiteDescription_MOS6502TestSuite, 144, "testOpLDA_ABS_X" ) {}
 void runTest() { suite_MOS6502TestSuite.testOpLDA_ABS_X(); }
} testDescription_MOS6502TestSuite_testOpLDA_ABS_X;

static class TestDescription_MOS6502TestSuite_testOpLDA_ABS_Y : public CxxTest::RealTestDescription {
public:
 TestDescription_MOS6502TestSuite_testOpLDA_ABS_Y() : CxxTest::RealTestDescription( Tests_MOS6502TestSuite, suiteDescription_MOS6502TestSuite, 159, "testOpLDA_ABS_Y" ) {}
 void runTest() { suite_MOS6502TestSuite.testOpLDA_ABS_Y(); }
} testDescription_MOS6502TestSuite_testOpLDA_ABS_Y;

static class TestDescription_MOS6502TestSuite_testOpLDA_IND_X : public CxxTest::RealTestDescription {
public:
 TestDescription_MOS6502TestSuite_testOpLDA_IND_X() : CxxTest::RealTestDescription( Tests_MOS6502TestSuite, suiteDescription_MOS6502TestSuite, 175, "testOpLDA_IND_X" ) {}
 void runTest() { suite_MOS6502TestSuite.testOpLDA_IND_X(); }
} testDescription_MOS6502TestSuite_testOpLDA_IND_X;

static class TestDescription_MOS6502TestSuite_testOpLDA_IND_Y : public CxxTest::RealTestDescription {
public:
 TestDescription_MOS6502TestSuite_testOpLDA_IND_Y() : CxxTest::RealTestDescription( Tests_MOS6502TestSuite, suiteDescription_MOS6502TestSuite, 193, "testOpLDA_IND_Y" ) {}
 void runTest() { suite_MOS6502TestSuite.testOpLDA_IND_Y(); }
} testDescription_MOS6502TestSuite_testOpLDA_IND_Y;

static class TestDescription_MOS6502TestSuite_testOpLDX_IMM : public CxxTest::RealTestDescription {
public:
 TestDescription_MOS6502TestSuite_testOpLDX_IMM() : CxxTest::RealTestDescription( Tests_MOS6502TestSuite, suiteDescription_MOS6502TestSuite, 213, "testOpLDX_IMM" ) {}
 void runTest() { suite_MOS6502TestSuite.testOpLDX_IMM(); }
} testDescription_MOS6502TestSuite_testOpLDX_IMM;

static class TestDescription_MOS6502TestSuite_testOpLDX_ZP : public CxxTest::RealTestDescription {
public:
 TestDescription_MOS6502TestSuite_testOpLDX_ZP() : CxxTest::RealTestDescription( Tests_MOS6502TestSuite, suiteDescription_MOS6502TestSuite, 226, "testOpLDX_ZP" ) {}
 void runTest() { suite_MOS6502TestSuite.testOpLDX_ZP(); }
} testDescription_MOS6502TestSuite_testOpLDX_ZP;

static class TestDescription_MOS6502TestSuite_testOpLDX_ZP_Y : public CxxTest::RealTestDescription {
public:
 TestDescription_MOS6502TestSuite_testOpLDX_ZP_Y() : CxxTest::RealTestDescription( Tests_MOS6502TestSuite, suiteDescription_MOS6502TestSuite, 240, "testOpLDX_ZP_Y" ) {}
 void runTest() { suite_MOS6502TestSuite.testOpLDX_ZP_Y(); }
} testDescription_MOS6502TestSuite_testOpLDX_ZP_Y;

static class TestDescription_MOS6502TestSuite_testOpLDX_ABS : public CxxTest::RealTestDescription {
public:
 TestDescription_MOS6502TestSuite_testOpLDX_ABS() : CxxTest::RealTestDescription( Tests_MOS6502TestSuite, suiteDescription_MOS6502TestSuite, 255, "testOpLDX_ABS" ) {}
 void runTest() { suite_MOS6502TestSuite.testOpLDX_ABS(); }
} testDescription_MOS6502TestSuite_testOpLDX_ABS;

static class TestDescription_MOS6502TestSuite_testOpLDX_ABS_Y : public CxxTest::RealTestDescription {
public:
 TestDescription_MOS6502TestSuite_testOpLDX_ABS_Y() : CxxTest::RealTestDescription( Tests_MOS6502TestSuite, suiteDescription_MOS6502TestSuite, 269, "testOpLDX_ABS_Y" ) {}
 void runTest() { suite_MOS6502TestSuite.testOpLDX_ABS_Y(); }
} testDescription_MOS6502TestSuite_testOpLDX_ABS_Y;

static class TestDescription_MOS6502TestSuite_testOpLDY_IMM : public CxxTest::RealTestDescription {
public:
 TestDescription_MOS6502TestSuite_testOpLDY_IMM() : CxxTest::RealTestDescription( Tests_MOS6502TestSuite, suiteDescription_MOS6502TestSuite, 286, "testOpLDY_IMM" ) {}
 void runTest() { suite_MOS6502TestSuite.testOpLDY_IMM(); }
} testDescription_MOS6502TestSuite_testOpLDY_IMM;

static class TestDescription_MOS6502TestSuite_testOpLDY_ZP : public CxxTest::RealTestDescription {
public:
 TestDescription_MOS6502TestSuite_testOpLDY_ZP() : CxxTest::RealTestDescription( Tests_MOS6502TestSuite, suiteDescription_MOS6502TestSuite, 299, "testOpLDY_ZP" ) {}
 void runTest() { suite_MOS6502TestSuite.testOpLDY_ZP(); }
} testDescription_MOS6502TestSuite_testOpLDY_ZP;

static class TestDescription_MOS6502TestSuite_testOpLDY_ZP_X : public CxxTest::RealTestDescription {
public:
 TestDescription_MOS6502TestSuite_testOpLDY_ZP_X() : CxxTest::RealTestDescription( Tests_MOS6502TestSuite, suiteDescription_MOS6502TestSuite, 313, "testOpLDY_ZP_X" ) {}
 void runTest() { suite_MOS6502TestSuite.testOpLDY_ZP_X(); }
} testDescription_MOS6502TestSuite_testOpLDY_ZP_X;

static class TestDescription_MOS6502TestSuite_testOpLDY_ABS : public CxxTest::RealTestDescription {
public:
 TestDescription_MOS6502TestSuite_testOpLDY_ABS() : CxxTest::RealTestDescription( Tests_MOS6502TestSuite, suiteDescription_MOS6502TestSuite, 328, "testOpLDY_ABS" ) {}
 void runTest() { suite_MOS6502TestSuite.testOpLDY_ABS(); }
} testDescription_MOS6502TestSuite_testOpLDY_ABS;

static class TestDescription_MOS6502TestSuite_testOpLDY_ABS_X : public CxxTest::RealTestDescription {
public:
 TestDescription_MOS6502TestSuite_testOpLDY_ABS_X() : CxxTest::RealTestDescription( Tests_MOS6502TestSuite, suiteDescription_MOS6502TestSuite, 342, "testOpLDY_ABS_X" ) {}
 void runTest() { suite_MOS6502TestSuite.testOpLDY_ABS_X(); }
} testDescription_MOS6502TestSuite_testOpLDY_ABS_X;

static class TestDescription_MOS6502TestSuite_testOpSTA_ZP : public CxxTest::RealTestDescription {
public:
 TestDescription_MOS6502TestSuite_testOpSTA_ZP() : CxxTest::RealTestDescription( Tests_MOS6502TestSuite, suiteDescription_MOS6502TestSuite, 361, "testOpSTA_ZP" ) {}
 void runTest() { suite_MOS6502TestSuite.testOpSTA_ZP(); }
} testDescription_MOS6502TestSuite_testOpSTA_ZP;

static class TestDescription_MOS6502TestSuite_testOpSTA_ZP_X : public CxxTest::RealTestDescription {
public:
 TestDescription_MOS6502TestSuite_testOpSTA_ZP_X() : CxxTest::RealTestDescription( Tests_MOS6502TestSuite, suiteDescription_MOS6502TestSuite, 375, "testOpSTA_ZP_X" ) {}
 void runTest() { suite_MOS6502TestSuite.testOpSTA_ZP_X(); }
} testDescription_MOS6502TestSuite_testOpSTA_ZP_X;

static class TestDescription_MOS6502TestSuite_testOpSTA_ABS : public CxxTest::RealTestDescription {
public:
 TestDescription_MOS6502TestSuite_testOpSTA_ABS() : CxxTest::RealTestDescription( Tests_MOS6502TestSuite, suiteDescription_MOS6502TestSuite, 390, "testOpSTA_ABS" ) {}
 void runTest() { suite_MOS6502TestSuite.testOpSTA_ABS(); }
} testDescription_MOS6502TestSuite_testOpSTA_ABS;

static class TestDescription_MOS6502TestSuite_testOpSTA_ABS_X : public CxxTest::RealTestDescription {
public:
 TestDescription_MOS6502TestSuite_testOpSTA_ABS_X() : CxxTest::RealTestDescription( Tests_MOS6502TestSuite, suiteDescription_MOS6502TestSuite, 404, "testOpSTA_ABS_X" ) {}
 void runTest() { suite_MOS6502TestSuite.testOpSTA_ABS_X(); }
} testDescription_MOS6502TestSuite_testOpSTA_ABS_X;

static class TestDescription_MOS6502TestSuite_testOpSTA_ABS_Y : public CxxTest::RealTestDescription {
public:
 TestDescription_MOS6502TestSuite_testOpSTA_ABS_Y() : CxxTest::RealTestDescription( Tests_MOS6502TestSuite, suiteDescription_MOS6502TestSuite, 419, "testOpSTA_ABS_Y" ) {}
 void runTest() { suite_MOS6502TestSuite.testOpSTA_ABS_Y(); }
} testDescription_MOS6502TestSuite_testOpSTA_ABS_Y;

static class TestDescription_MOS6502TestSuite_testOpSTA_IND_X : public CxxTest::RealTestDescription {
public:
 TestDescription_MOS6502TestSuite_testOpSTA_IND_X() : CxxTest::RealTestDescription( Tests_MOS6502TestSuite, suiteDescription_MOS6502TestSuite, 435, "testOpSTA_IND_X" ) {}
 void runTest() { suite_MOS6502TestSuite.testOpSTA_IND_X(); }
} testDescription_MOS6502TestSuite_testOpSTA_IND_X;

static class TestDescription_MOS6502TestSuite_testOpSTA_IND_Y : public CxxTest::RealTestDescription {
public:
 TestDescription_MOS6502TestSuite_testOpSTA_IND_Y() : CxxTest::RealTestDescription( Tests_MOS6502TestSuite, suiteDescription_MOS6502TestSuite, 453, "testOpSTA_IND_Y" ) {}
 void runTest() { suite_MOS6502TestSuite.testOpSTA_IND_Y(); }
} testDescription_MOS6502TestSuite_testOpSTA_IND_Y;

static class TestDescription_MOS6502TestSuite_testOpSTX_ZP : public CxxTest::RealTestDescription {
public:
 TestDescription_MOS6502TestSuite_testOpSTX_ZP() : CxxTest::RealTestDescription( Tests_MOS6502TestSuite, suiteDescription_MOS6502TestSuite, 474, "testOpSTX_ZP" ) {}
 void runTest() { suite_MOS6502TestSuite.testOpSTX_ZP(); }
} testDescription_MOS6502TestSuite_testOpSTX_ZP;

static class TestDescription_MOS6502TestSuite_testOpSTX_ZP_Y : public CxxTest::RealTestDescription {
public:
 TestDescription_MOS6502TestSuite_testOpSTX_ZP_Y() : CxxTest::RealTestDescription( Tests_MOS6502TestSuite, suiteDescription_MOS6502TestSuite, 488, "testOpSTX_ZP_Y" ) {}
 void runTest() { suite_MOS6502TestSuite.testOpSTX_ZP_Y(); }
} testDescription_MOS6502TestSuite_testOpSTX_ZP_Y;

static class TestDescription_MOS6502TestSuite_testOpSTX_ABS : public CxxTest::RealTestDescription {
public:
 TestDescription_MOS6502TestSuite_testOpSTX_ABS() : CxxTest::RealTestDescription( Tests_MOS6502TestSuite, suiteDescription_MOS6502TestSuite, 503, "testOpSTX_ABS" ) {}
 void runTest() { suite_MOS6502TestSuite.testOpSTX_ABS(); }
} testDescription_MOS6502TestSuite_testOpSTX_ABS;

static class TestDescription_MOS6502TestSuite_testOpSTY_ZP : public CxxTest::RealTestDescription {
public:
 TestDescription_MOS6502TestSuite_testOpSTY_ZP() : CxxTest::RealTestDescription( Tests_MOS6502TestSuite, suiteDescription_MOS6502TestSuite, 520, "testOpSTY_ZP" ) {}
 void runTest() { suite_MOS6502TestSuite.testOpSTY_ZP(); }
} testDescription_MOS6502TestSuite_testOpSTY_ZP;

static class TestDescription_MOS6502TestSuite_testOpSTY_ZP_X : public CxxTest::RealTestDescription {
public:
 TestDescription_MOS6502TestSuite_testOpSTY_ZP_X() : CxxTest::RealTestDescription( Tests_MOS6502TestSuite, suiteDescription_MOS6502TestSuite, 534, "testOpSTY_ZP_X" ) {}
 void runTest() { suite_MOS6502TestSuite.testOpSTY_ZP_X(); }
} testDescription_MOS6502TestSuite_testOpSTY_ZP_X;

static class TestDescription_MOS6502TestSuite_testOpSTY_ABS : public CxxTest::RealTestDescription {
public:
 TestDescription_MOS6502TestSuite_testOpSTY_ABS() : CxxTest::RealTestDescription( Tests_MOS6502TestSuite, suiteDescription_MOS6502TestSuite, 549, "testOpSTY_ABS" ) {}
 void runTest() { suite_MOS6502TestSuite.testOpSTY_ABS(); }
} testDescription_MOS6502TestSuite_testOpSTY_ABS;

static class TestDescription_MOS6502TestSuite_testOpADC_IMM : public CxxTest::RealTestDescription {
public:
 TestDescription_MOS6502TestSuite_testOpADC_IMM() : CxxTest::RealTestDescription( Tests_MOS6502TestSuite, suiteDescription_MOS6502TestSuite, 566, "testOpADC_IMM" ) {}
 void runTest() { suite_MOS6502TestSuite.testOpADC_IMM(); }
} testDescription_MOS6502TestSuite_testOpADC_IMM;

static class TestDescription_MOS6502TestSuite_testOpADC_IMM_DEC : public CxxTest::RealTestDescription {
public:
 TestDescription_MOS6502TestSuite_testOpADC_IMM_DEC() : CxxTest::RealTestDescription( Tests_MOS6502TestSuite, suiteDescription_MOS6502TestSuite, 586, "testOpADC_IMM_DEC" ) {}
 void runTest() { suite_MOS6502TestSuite.testOpADC_IMM_DEC(); }
} testDescription_MOS6502TestSuite_testOpADC_IMM_DEC;

static class TestDescription_MOS6502TestSuite_testOpADC_IMM_DEC2 : public CxxTest::RealTestDescription {
public:
 TestDescription_MOS6502TestSuite_testOpADC_IMM_DEC2() : CxxTest::RealTestDescription( Tests_MOS6502TestSuite, suiteDescription_MOS6502TestSuite, 611, "testOpADC_IMM_DEC2" ) {}
 void runTest() { suite_MOS6502TestSuite.testOpADC_IMM_DEC2(); }
} testDescription_MOS6502TestSuite_testOpADC_IMM_DEC2;

static class TestDescription_MOS6502TestSuite_testOpADC_ZP : public CxxTest::RealTestDescription {
public:
 TestDescription_MOS6502TestSuite_testOpADC_ZP() : CxxTest::RealTestDescription( Tests_MOS6502TestSuite, suiteDescription_MOS6502TestSuite, 634, "testOpADC_ZP" ) {}
 void runTest() { suite_MOS6502TestSuite.testOpADC_ZP(); }
} testDescription_MOS6502TestSuite_testOpADC_ZP;

static class TestDescription_MOS6502TestSuite_testOpADC_ZP_DEC : public CxxTest::RealTestDescription {
public:
 TestDescription_MOS6502TestSuite_testOpADC_ZP_DEC() : CxxTest::RealTestDescription( Tests_MOS6502TestSuite, suiteDescription_MOS6502TestSuite, 655, "testOpADC_ZP_DEC" ) {}
 void runTest() { suite_MOS6502TestSuite.testOpADC_ZP_DEC(); }
} testDescription_MOS6502TestSuite_testOpADC_ZP_DEC;

static class TestDescription_MOS6502TestSuite_testOpADC_ZP_X : public CxxTest::RealTestDescription {
public:
 TestDescription_MOS6502TestSuite_testOpADC_ZP_X() : CxxTest::RealTestDescription( Tests_MOS6502TestSuite, suiteDescription_MOS6502TestSuite, 678, "testOpADC_ZP_X" ) {}
 void runTest() { suite_MOS6502TestSuite.testOpADC_ZP_X(); }
} testDescription_MOS6502TestSuite_testOpADC_ZP_X;

static class TestDescription_MOS6502TestSuite_testOpADC_ZP_X_DEC : public CxxTest::RealTestDescription {
public:
 TestDescription_MOS6502TestSuite_testOpADC_ZP_X_DEC() : CxxTest::RealTestDescription( Tests_MOS6502TestSuite, suiteDescription_MOS6502TestSuite, 700, "testOpADC_ZP_X_DEC" ) {}
 void runTest() { suite_MOS6502TestSuite.testOpADC_ZP_X_DEC(); }
} testDescription_MOS6502TestSuite_testOpADC_ZP_X_DEC;

static class TestDescription_MOS6502TestSuite_testOpADC_ABS : public CxxTest::RealTestDescription {
public:
 TestDescription_MOS6502TestSuite_testOpADC_ABS() : CxxTest::RealTestDescription( Tests_MOS6502TestSuite, suiteDescription_MOS6502TestSuite, 724, "testOpADC_ABS" ) {}
 void runTest() { suite_MOS6502TestSuite.testOpADC_ABS(); }
} testDescription_MOS6502TestSuite_testOpADC_ABS;

static class TestDescription_MOS6502TestSuite_testOpADC_ABS_DEC : public CxxTest::RealTestDescription {
public:
 TestDescription_MOS6502TestSuite_testOpADC_ABS_DEC() : CxxTest::RealTestDescription( Tests_MOS6502TestSuite, suiteDescription_MOS6502TestSuite, 745, "testOpADC_ABS_DEC" ) {}
 void runTest() { suite_MOS6502TestSuite.testOpADC_ABS_DEC(); }
} testDescription_MOS6502TestSuite_testOpADC_ABS_DEC;

static class TestDescription_MOS6502TestSuite_testOpADC_ABS_X : public CxxTest::RealTestDescription {
public:
 TestDescription_MOS6502TestSuite_testOpADC_ABS_X() : CxxTest::RealTestDescription( Tests_MOS6502TestSuite, suiteDescription_MOS6502TestSuite, 768, "testOpADC_ABS_X" ) {}
 void runTest() { suite_MOS6502TestSuite.testOpADC_ABS_X(); }
} testDescription_MOS6502TestSuite_testOpADC_ABS_X;

static class TestDescription_MOS6502TestSuite_testOpADC_ABS_X_DEC : public CxxTest::RealTestDescription {
public:
 TestDescription_MOS6502TestSuite_testOpADC_ABS_X_DEC() : CxxTest::RealTestDescription( Tests_MOS6502TestSuite, suiteDescription_MOS6502TestSuite, 790, "testOpADC_ABS_X_DEC" ) {}
 void runTest() { suite_MOS6502TestSuite.testOpADC_ABS_X_DEC(); }
} testDescription_MOS6502TestSuite_testOpADC_ABS_X_DEC;

static class TestDescription_MOS6502TestSuite_testOpADC_ABS_Y : public CxxTest::RealTestDescription {
public:
 TestDescription_MOS6502TestSuite_testOpADC_ABS_Y() : CxxTest::RealTestDescription( Tests_MOS6502TestSuite, suiteDescription_MOS6502TestSuite, 814, "testOpADC_ABS_Y" ) {}
 void runTest() { suite_MOS6502TestSuite.testOpADC_ABS_Y(); }
} testDescription_MOS6502TestSuite_testOpADC_ABS_Y;

static class TestDescription_MOS6502TestSuite_testOpADC_ABS_Y_DEC : public CxxTest::RealTestDescription {
public:
 TestDescription_MOS6502TestSuite_testOpADC_ABS_Y_DEC() : CxxTest::RealTestDescription( Tests_MOS6502TestSuite, suiteDescription_MOS6502TestSuite, 836, "testOpADC_ABS_Y_DEC" ) {}
 void runTest() { suite_MOS6502TestSuite.testOpADC_ABS_Y_DEC(); }
} testDescription_MOS6502TestSuite_testOpADC_ABS_Y_DEC;

static class TestDescription_MOS6502TestSuite_testOpADC_IND_X : public CxxTest::RealTestDescription {
public:
 TestDescription_MOS6502TestSuite_testOpADC_IND_X() : CxxTest::RealTestDescription( Tests_MOS6502TestSuite, suiteDescription_MOS6502TestSuite, 860, "testOpADC_IND_X" ) {}
 void runTest() { suite_MOS6502TestSuite.testOpADC_IND_X(); }
} testDescription_MOS6502TestSuite_testOpADC_IND_X;

static class TestDescription_MOS6502TestSuite_testOpADC_IND_X_DEC : public CxxTest::RealTestDescription {
public:
 TestDescription_MOS6502TestSuite_testOpADC_IND_X_DEC() : CxxTest::RealTestDescription( Tests_MOS6502TestSuite, suiteDescription_MOS6502TestSuite, 884, "testOpADC_IND_X_DEC" ) {}
 void runTest() { suite_MOS6502TestSuite.testOpADC_IND_X_DEC(); }
} testDescription_MOS6502TestSuite_testOpADC_IND_X_DEC;

static class TestDescription_MOS6502TestSuite_testOpADC_IND_Y : public CxxTest::RealTestDescription {
public:
 TestDescription_MOS6502TestSuite_testOpADC_IND_Y() : CxxTest::RealTestDescription( Tests_MOS6502TestSuite, suiteDescription_MOS6502TestSuite, 910, "testOpADC_IND_Y" ) {}
 void runTest() { suite_MOS6502TestSuite.testOpADC_IND_Y(); }
} testDescription_MOS6502TestSuite_testOpADC_IND_Y;

static class TestDescription_MOS6502TestSuite_testOpADC_IND_Y_DEC : public CxxTest::RealTestDescription {
public:
 TestDescription_MOS6502TestSuite_testOpADC_IND_Y_DEC() : CxxTest::RealTestDescription( Tests_MOS6502TestSuite, suiteDescription_MOS6502TestSuite, 934, "testOpADC_IND_Y_DEC" ) {}
 void runTest() { suite_MOS6502TestSuite.testOpADC_IND_Y_DEC(); }
} testDescription_MOS6502TestSuite_testOpADC_IND_Y_DEC;

static class TestDescription_MOS6502TestSuite_testOpSBC_IMM : public CxxTest::RealTestDescription {
public:
 TestDescription_MOS6502TestSuite_testOpSBC_IMM() : CxxTest::RealTestDescription( Tests_MOS6502TestSuite, suiteDescription_MOS6502TestSuite, 963, "testOpSBC_IMM" ) {}
 void runTest() { suite_MOS6502TestSuite.testOpSBC_IMM(); }
} testDescription_MOS6502TestSuite_testOpSBC_IMM;

static class TestDescription_MOS6502TestSuite_testOpSBC_IMM_DEC : public CxxTest::RealTestDescription {
public:
 TestDescription_MOS6502TestSuite_testOpSBC_IMM_DEC() : CxxTest::RealTestDescription( Tests_MOS6502TestSuite, suiteDescription_MOS6502TestSuite, 986, "testOpSBC_IMM_DEC" ) {}
 void runTest() { suite_MOS6502TestSuite.testOpSBC_IMM_DEC(); }
} testDescription_MOS6502TestSuite_testOpSBC_IMM_DEC;

static class TestDescription_MOS6502TestSuite_testOpSBC_ZP : public CxxTest::RealTestDescription {
public:
 TestDescription_MOS6502TestSuite_testOpSBC_ZP() : CxxTest::RealTestDescription( Tests_MOS6502TestSuite, suiteDescription_MOS6502TestSuite, 1011, "testOpSBC_ZP" ) {}
 void runTest() { suite_MOS6502TestSuite.testOpSBC_ZP(); }
} testDescription_MOS6502TestSuite_testOpSBC_ZP;

static class TestDescription_MOS6502TestSuite_testOpSBC_ZP_DEC : public CxxTest::RealTestDescription {
public:
 TestDescription_MOS6502TestSuite_testOpSBC_ZP_DEC() : CxxTest::RealTestDescription( Tests_MOS6502TestSuite, suiteDescription_MOS6502TestSuite, 1034, "testOpSBC_ZP_DEC" ) {}
 void runTest() { suite_MOS6502TestSuite.testOpSBC_ZP_DEC(); }
} testDescription_MOS6502TestSuite_testOpSBC_ZP_DEC;

static class TestDescription_MOS6502TestSuite_testOpSBC_ZP_X : public CxxTest::RealTestDescription {
public:
 TestDescription_MOS6502TestSuite_testOpSBC_ZP_X() : CxxTest::RealTestDescription( Tests_MOS6502TestSuite, suiteDescription_MOS6502TestSuite, 1059, "testOpSBC_ZP_X" ) {}
 void runTest() { suite_MOS6502TestSuite.testOpSBC_ZP_X(); }
} testDescription_MOS6502TestSuite_testOpSBC_ZP_X;

static class TestDescription_MOS6502TestSuite_testOpSBC_ZP_X_DEC : public CxxTest::RealTestDescription {
public:
 TestDescription_MOS6502TestSuite_testOpSBC_ZP_X_DEC() : CxxTest::RealTestDescription( Tests_MOS6502TestSuite, suiteDescription_MOS6502TestSuite, 1083, "testOpSBC_ZP_X_DEC" ) {}
 void runTest() { suite_MOS6502TestSuite.testOpSBC_ZP_X_DEC(); }
} testDescription_MOS6502TestSuite_testOpSBC_ZP_X_DEC;

static class TestDescription_MOS6502TestSuite_testOpSBC_ABS : public CxxTest::RealTestDescription {
public:
 TestDescription_MOS6502TestSuite_testOpSBC_ABS() : CxxTest::RealTestDescription( Tests_MOS6502TestSuite, suiteDescription_MOS6502TestSuite, 1109, "testOpSBC_ABS" ) {}
 void runTest() { suite_MOS6502TestSuite.testOpSBC_ABS(); }
} testDescription_MOS6502TestSuite_testOpSBC_ABS;

static class TestDescription_MOS6502TestSuite_testOpSBC_ABS_DEC : public CxxTest::RealTestDescription {
public:
 TestDescription_MOS6502TestSuite_testOpSBC_ABS_DEC() : CxxTest::RealTestDescription( Tests_MOS6502TestSuite, suiteDescription_MOS6502TestSuite, 1132, "testOpSBC_ABS_DEC" ) {}
 void runTest() { suite_MOS6502TestSuite.testOpSBC_ABS_DEC(); }
} testDescription_MOS6502TestSuite_testOpSBC_ABS_DEC;

static class TestDescription_MOS6502TestSuite_testOpSBC_ABS_X : public CxxTest::RealTestDescription {
public:
 TestDescription_MOS6502TestSuite_testOpSBC_ABS_X() : CxxTest::RealTestDescription( Tests_MOS6502TestSuite, suiteDescription_MOS6502TestSuite, 1157, "testOpSBC_ABS_X" ) {}
 void runTest() { suite_MOS6502TestSuite.testOpSBC_ABS_X(); }
} testDescription_MOS6502TestSuite_testOpSBC_ABS_X;

static class TestDescription_MOS6502TestSuite_testOpSBC_ABS_X_DEC : public CxxTest::RealTestDescription {
public:
 TestDescription_MOS6502TestSuite_testOpSBC_ABS_X_DEC() : CxxTest::RealTestDescription( Tests_MOS6502TestSuite, suiteDescription_MOS6502TestSuite, 1181, "testOpSBC_ABS_X_DEC" ) {}
 void runTest() { suite_MOS6502TestSuite.testOpSBC_ABS_X_DEC(); }
} testDescription_MOS6502TestSuite_testOpSBC_ABS_X_DEC;

static class TestDescription_MOS6502TestSuite_testOpSBC_ABS_Y : public CxxTest::RealTestDescription {
public:
 TestDescription_MOS6502TestSuite_testOpSBC_ABS_Y() : CxxTest::RealTestDescription( Tests_MOS6502TestSuite, suiteDescription_MOS6502TestSuite, 1207, "testOpSBC_ABS_Y" ) {}
 void runTest() { suite_MOS6502TestSuite.testOpSBC_ABS_Y(); }
} testDescription_MOS6502TestSuite_testOpSBC_ABS_Y;

static class TestDescription_MOS6502TestSuite_testOpSBC_ABS_Y_DEC : public CxxTest::RealTestDescription {
public:
 TestDescription_MOS6502TestSuite_testOpSBC_ABS_Y_DEC() : CxxTest::RealTestDescription( Tests_MOS6502TestSuite, suiteDescription_MOS6502TestSuite, 1231, "testOpSBC_ABS_Y_DEC" ) {}
 void runTest() { suite_MOS6502TestSuite.testOpSBC_ABS_Y_DEC(); }
} testDescription_MOS6502TestSuite_testOpSBC_ABS_Y_DEC;

static class TestDescription_MOS6502TestSuite_testOpSBC_IND_X : public CxxTest::RealTestDescription {
public:
 TestDescription_MOS6502TestSuite_testOpSBC_IND_X() : CxxTest::RealTestDescription( Tests_MOS6502TestSuite, suiteDescription_MOS6502TestSuite, 1257, "testOpSBC_IND_X" ) {}
 void runTest() { suite_MOS6502TestSuite.testOpSBC_IND_X(); }
} testDescription_MOS6502TestSuite_testOpSBC_IND_X;

static class TestDescription_MOS6502TestSuite_testOpSBC_IND_X_DEC : public CxxTest::RealTestDescription {
public:
 TestDescription_MOS6502TestSuite_testOpSBC_IND_X_DEC() : CxxTest::RealTestDescription( Tests_MOS6502TestSuite, suiteDescription_MOS6502TestSuite, 1283, "testOpSBC_IND_X_DEC" ) {}
 void runTest() { suite_MOS6502TestSuite.testOpSBC_IND_X_DEC(); }
} testDescription_MOS6502TestSuite_testOpSBC_IND_X_DEC;

static class TestDescription_MOS6502TestSuite_testOpSBC_IND_Y : public CxxTest::RealTestDescription {
public:
 TestDescription_MOS6502TestSuite_testOpSBC_IND_Y() : CxxTest::RealTestDescription( Tests_MOS6502TestSuite, suiteDescription_MOS6502TestSuite, 1311, "testOpSBC_IND_Y" ) {}
 void runTest() { suite_MOS6502TestSuite.testOpSBC_IND_Y(); }
} testDescription_MOS6502TestSuite_testOpSBC_IND_Y;

static class TestDescription_MOS6502TestSuite_testOpSBC_IND_Y_DEC : public CxxTest::RealTestDescription {
public:
 TestDescription_MOS6502TestSuite_testOpSBC_IND_Y_DEC() : CxxTest::RealTestDescription( Tests_MOS6502TestSuite, suiteDescription_MOS6502TestSuite, 1337, "testOpSBC_IND_Y_DEC" ) {}
 void runTest() { suite_MOS6502TestSuite.testOpSBC_IND_Y_DEC(); }
} testDescription_MOS6502TestSuite_testOpSBC_IND_Y_DEC;

#include <cxxtest/Root.cpp>
