
DEFINES=

RM=rm -fv
CPP=c++
CC=gcc
STRIP=strip
CFLAGS=$(DEFINES) -Wall -O2
LDFLAGS=-lnsl -lcrypt  
INCDIRS=

# This really should be fused with the make debug tag
#DEBUG_FLAGS=-g -DDEBUG
DEBUG_FLAGS=-DDEBUG

SOURCES=ansi.cc banentry.cc bot.cc channel.cc channellist.cc commands.cc \
	config.cc connection.cc ctcp.cc dccconnection.cc dccperson.cc \
	flags.cc games.cc log.cc main.cc mask.cc note.cc notelist.cc \
	parser.cc person.cc queue.cc server.cc serverconnection.cc \
	serverlist.cc serverqueue.cc serverqueueitem.cc signal.cc socket.cc \
	string.cc stringtokenizer.cc telnet.cc telnetspy.cc todolist.cc \
	user.cc usercommands.cc userlist.cc utils.cc

VER_SRC=version.cc
VER_OBJ=version.o
VER_INC=version.h
VER_FLG=-DCOMPILE_STRING="\"`uname -n -m`\"" -DBUILD_STRING="\"`date`\""

OBJECTS=$(SOURCES:.cc=.o)

TARGET=dumbop

all: $(TARGET)
#	$(STRIP) $(TARGET)

debug: $(TARGET)

$(TARGET): $(OBJECTS)
	   $(CPP) $(CFLAGS) $(INCDIRS) $(DEBUG_FLAGS) $(VER_FLG) -c $(VER_SRC)
	   $(CPP) $(CFLAGS) $(INCDIRS) $(LDFLAGS) $(DEBUG_FLAGS) -o $@ $(OBJECTS) $(VER_OBJ) $(LIBS)

%.o: %.cc
	$(CPP) $(CFLAGS) $(INCDIRS) $(DEBUG_FLAGS) -c $<

clean:
	$(RM) *~ *.o $(TARGET) core Makefile.bak gmon.out

