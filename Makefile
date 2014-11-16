
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
CXXFLAGS = -g -O3 -std=c++11 -MD -MP

CXXTESTDIR = /usr/share/cxxtest
CXXTESTGEN = /usr/bin/cxxtestgen

LDFLAGS = 
LIBS = 
INSTALL = /usr/bin/install

LIBS        := $(LIBS) 

DEBUG        = -ggdb 
DEFINES      = ${DEBUG}


INCPATHS    := 
LIBPATHS     = #-L/lib -L/usr/local/lib -L/usr/lib

CFLAGS       = $(O) $(DEFINES) $(INCPATHS) $(INCLUDES)
LFLAGS       = $(LIBPATHS) $(LIBS)


## Objects ################################################

COMMON_OBJECTS  = mos6502.o \
		  mos6502_disasm.o \
		  mos6522.o \
		  memory.o \
		  machine.o

EMU_OBJECTS = $(COMMON_OBJECTS) \
		  oric.o


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
	rm -f tests/tests.cpp

install:
	install -m 755 -s $(TARGET) $(BUILDROOT)/$(bindir)

doc:
	doxygen doxygen/doxygen.cfg


tests/tests.cpp: tests/test1.h
	$(CXXTESTGEN) --error-printer -o tests/tests.cpp tests/test1.h

test: tests/tests.cpp $(COMMON_OBJECTS) 
	$(CXX) -I$(CXXTESTDIR) -I. tests/tests.cpp $(COMMON_OBJECTS) -o selftest


count:
	@echo -n "        Header files (lines): "; cat *.h*  | wc -l;
	@echo -n "Implementation files (lines): "; cat *.c*  | wc -l



-include $(OBJECTS:%.o=%.d)