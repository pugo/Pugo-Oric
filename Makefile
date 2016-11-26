
.EXPORT_ALL_VARIABLES:

#####################################################################
# Variables
#####################################################################
SHELL = /bin/sh

top_srcdir = .
srcdir = .
prefix = /usr/local
exec_prefix = ${prefix}
sysconfdir = ${prefix}/etc
bindir = ${exec_prefix}/bin
sbindir = ${exec_prefix}/sbin
infodir = ${prefix}/info
libdir = ${exec_prefix}/lib
mandir = ${prefix}/man

CXX = c++
CXXFLAGS = -g -O2 -std=c++11 -MD -MP

CXXTESTDIR = /usr/share/cxxtest
CXXTESTGEN = /usr/bin/cxxtestgen

LDFLAGS = 
LIBS = -lSDL2 -lSDL2_image
INSTALL = /usr/bin/install

DEBUG        = -ggdb 
DEFINES      = ${DEBUG}

INCPATHS    := -I/usr/include/SDL2
LIBPATHS     = #-L/lib -L/usr/local/lib -L/usr/lib

CFLAGS       = $(O) $(DEFINES) $(INCPATHS) $(INCLUDES)
LFLAGS       = $(LIBPATHS) $(LIBS)

## Objects ################################################

COMMON_OBJECTS  = mos6502.o \
		  mos6502_disasm.o \
		  mos6522.o \
		  memory.o \
		  machine.o \
		  frontend.o

EMU_OBJECTS = $(COMMON_OBJECTS) \
		  oric.o

TEST_FILES = tests/6502_tests.h \
	     tests/6522_tests.h \
	     tests/memory_tests.h

## Implicit rules #########################################

.SUFFIXES: .cpp .c

.cpp.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATHS) $(DEFINES) -o $@ $<
.c.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATHS) $(DEFINES) -o $@ $<

## Build rules ############################################

all: oric

oric: $(EMU_OBJECTS)
	$(CXX) $(EMU_OBJECTS) -o oric $(LFLAGS)


clean:
	rm -f *.o *.bak *BAK *~ *% *pyc 
	rm -f oric
	rm -f core*
	rm -f -r doxygen/html doxygen/latex
	rm -f tests/run_tests.cpp run_tests

install:
	install -m 755 -s $(TARGET) $(BUILDROOT)/$(bindir)

doc:
	doxygen doxygen/doxygen.cfg

tests/run_tests.cpp: $(TEST_FILES)
	$(CXXTESTGEN) --error-printer -o tests/run_tests.cpp $(TEST_FILES)

test: tests/run_tests.cpp $(COMMON_OBJECTS) 
	$(CXX) $(CXXFLAGS) -I$(CXXTESTDIR) -I. tests/run_tests.cpp $(COMMON_OBJECTS) -o run_tests

count:
	@echo -n "        Header files (lines): "; cat *.h*  | wc -l;
	@echo -n "Implementation files (lines): "; cat *.c*  | wc -l

-include $(OBJECTS:%.o=%.d)
