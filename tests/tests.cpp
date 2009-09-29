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
#include "test1.h"

static MemoryTestSuite suite_MemoryTestSuite;

static CxxTest::List Tests_MemoryTestSuite = { 0, 0 };
CxxTest::StaticSuiteDescription suiteDescription_MemoryTestSuite( "test1.h", 9, "MemoryTestSuite", suite_MemoryTestSuite, Tests_MemoryTestSuite );

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
CxxTest::StaticSuiteDescription suiteDescription_MOS6502TestSuite( "test1.h", 48, "MOS6502TestSuite", suite_MOS6502TestSuite, Tests_MOS6502TestSuite );

static class TestDescription_MOS6502TestSuite_testCPUCreate : public CxxTest::RealTestDescription {
public:
 TestDescription_MOS6502TestSuite_testCPUCreate() : CxxTest::RealTestDescription( Tests_MOS6502TestSuite, suiteDescription_MOS6502TestSuite, 51, "testCPUCreate" ) {}
 void runTest() { suite_MOS6502TestSuite.testCPUCreate(); }
} testDescription_MOS6502TestSuite_testCPUCreate;

static class TestDescription_MOS6502TestSuite_testOpLDA_IMM : public CxxTest::RealTestDescription {
public:
 TestDescription_MOS6502TestSuite_testOpLDA_IMM() : CxxTest::RealTestDescription( Tests_MOS6502TestSuite, suiteDescription_MOS6502TestSuite, 67, "testOpLDA_IMM" ) {}
 void runTest() { suite_MOS6502TestSuite.testOpLDA_IMM(); }
} testDescription_MOS6502TestSuite_testOpLDA_IMM;

#include <cxxtest/Root.cpp>
