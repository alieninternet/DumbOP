# Generated automatically from Makefile.in by configure.
#
#
# ./src/queue/Makefile
# Make for IRC command queue routines
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

TARGET=queue.o

SOURCES=class.cc item.cc joinitem.cc kickitem.cc modeitem.cc noticeitem.cc \
	otheritem.cc send.cc serverqueue.cc whoitem.cc

INCLUDES=

# fix this too.. :(
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

