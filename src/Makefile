# Generated automatically from Makefile.in by configure.
#
#
# ./src/Makefile
# Main make-file for DumbOP sources
# Author: Simon Butcher
# Copyright (c) 2001 Alien Internet Services

########################################################################
##                                                                    ##
##     TO REDUCE THE RISK OF ELECTRIC SHOCK, DO NOT REMOVE COVER.     ##
##                NO USER - SERVICEABLE PARTS INSIDE.                 ##
##           REFER SERVICING TO QUALIFIED SERVICE PERSONNEL.          ##
##                                                                    ##
##       WARNING: TO REDUCE THE RISK OF FIRE OR ELECTRIC SHOCK,       ##
##          DO NOT EXPOSE THIS APPLIANCE TO RAIN OR MOISTURE          ##
##                                                                    ##
########################################################################

CXX=c++
RM=rm -vf

CXXFLAGS=-g -DDEBUG -Wall

srcdir=.
prefix=/usr/local
libdir=${exec_prefix}/lib

INCDIRS=-I$(srcdir) -I$(srcdir)/include
DEFS=-DHAVE_CONFIG_H
LIBS=

TARGET=dumbop

SOURCES=banentry.cc bot.cc commands.cc config.cc connection.cc ctcp.cc \
	flags.cc log.cc main.cc mask.cc server.cc \
	serverconnection.cc serverlist.cc todolist.cc

SUB_DIRS=channels commands dcc games notes parser queue strings system telnet \
	users utils
	
VER_SRC=version.cc
VER_OBJ=version.o
VER_FLG=-DCOMPILE_STRING="\"`uname -n -m`\"" -DBUILD_STRING="\"`date`\""

# this needs to be fixed...
OBJECTS=$(SOURCES:.cc=.o) \
	channels/channels.o commands/commands.o dcc/dcc.o games/games.o \
	notes/notes.o parser/parser.o queue/queue.o strings/strings.o \
	system/system.o telnet/telnet.o users/users.o utils/utils.o

########################################################################

.PHONY: all subdirs clean dclean

all: $(TARGET)

$(TARGET): subdirs $(OBJECTS) $(VER_OBJ)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $(OBJECTS) $(VER_OBJ) $(LIBS)

subdirs:
	for d in $(SUB_DIRS); do $(MAKE) -C $$d; done

%.o: %.cc
	$(CXX) $(CXXFLAGS) $(DEFS) $(INCDIRS) -c $<
	
$(VER_OBJ): $(VER_SRC)
	$(CXX) $(CXXFLAGS) $(VER_FLG) $(DEFS) $(INCDIRS) -c $(VER_SRC)

clean:
	for d in $(SUB_DIRS); do $(MAKE) -C $$d clean; done
	$(RM) *~ *.o $(TARGET)

dclean:
	for d in $(SUB_DIRS); do $(MAKE) -C $$d dclean; done
	$(RM) *~ *.o $(TARGET) Makefile
