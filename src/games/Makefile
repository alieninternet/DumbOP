# Generated automatically from Makefile.in by configure.
#
#
# ./src/games/Makefile
# Make for IRC game routines
# Author: Simon Butcher
# Copyright (c) 2001 Alien Internet Services

########################################################################
##                                                                    ##
########################################################################

CXX=c++
LD=ld
RM=rm -vf

CXXFLAGS=-g -DDEBUG -Wall
LDFLAGS=-r

srcdir=.
prefix=/usr/local
libdir=${exec_prefix}/lib

INCDIRS=-I$(srcdir) -I$(srcdir)/../include
DEFS=-DHAVE_CONFIG_H
LIBS=

TARGET=games.o

SOURCES=class.cc

SUB_DIRS=quiz

INCLUDES=

# fix this too.. :(
OBJECTS=$(SOURCES:.cc=.o) quiz/quiz.o

########################################################################

.PHONY: all subdirs clean dclean

all: $(TARGET)

$(TARGET): subdirs $(OBJECTS)
	$(LD) $(LDFLAGS) -o $@ $(OBJECTS) $(LIBS)

subdirs:
	for d in $(SUB_DIRS); do $(MAKE) -C $$d; done

%.o: %.cc
	$(CXX) $(CXXFLAGS) $(DEFS) $(INCDIRS) -c $<
	
clean:
	for d in $(SUB_DIRS); do $(MAKE) -C $$d clean; done
	$(RM) *~ *.o $(TARGET)

dclean:
	for d in $(SUB_DIRS); do $(MAKE) -C $$d dclean; done
	$(RM) *~ *.o $(TARGET) Makefile

