
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
CXXFLAGS = -g -O3

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

COMMON_OBJECTS  = oric.o \
				  cpu.o \
				  mos6502.o \
				  memory.o


## Implicit rules #########################################

.SUFFIXES: .cpp .c

.cpp.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATHS) $(DEFINES) -o $@ $<

.c.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATHS) $(DEFINES) -o $@ $<


## Build rules ############################################


all: oric


oric: $(COMMON_OBJECTS) $(CONFIG)
	$(CXX) $(COMMON_OBJECTS) -o oric $(LFLAGS)


clean:
	rm -f *.o *.bak *BAK *~ *% *pyc 
	rm -f oric
	rm -f core*
	rm -f config.cache config.log
	rm -f -r doxygen/html doxygen/latex


install:
	install -m 755 -s $(TARGET) $(BUILDROOT)/$(bindir)

doc:
	doxygen doxygen/doxygen.cfg


count:
	@echo -n "        Header files (lines): "; cat *.h*  | wc -l;
	@echo -n "Implementation files (lines): "; cat *.c*  | wc -l

.depend:
	touch .depend

depend:
	makedepend -f- -- $(CFLAGS) -- *.cpp  > .depend

include .depend
