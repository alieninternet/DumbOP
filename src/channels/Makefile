# Generated automatically from Makefile.in by configure.
#
#
# ./src/channels/Makefile
# Make for channel and channel user handling functions
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

TARGET=channels.o

SOURCES=channel.cc list.cc person.cc

INCLUDES=

OBJECTS=$(SOURCES:.cc=.o)

########################################################################

.PHONY: all clean dclean

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(LD) $(LDFLAGS) -o $@ $(OBJECTS) $(LIBS)

%.o: %.cc
	$(CXX) $(CXXFLAGS) $(DEFS) $(INCDIRS) -c $<
	
clean:
	$(RM) *~ *.o $(TARGET)

dclean:
	$(RM) *~ *.o $(TARGET) Makefile

