
DEFINES=

RM=rm -f
CPP=c++
CC=gcc
STRIP=strip
CFLAGS=$(DEFINES) -Wall
LDFLAGS=-lnsl -lcrypt  

# This really should be fused with the make debug tag
DEBUG_FLAGS=-g -DDEBUG

SOURCES=ansi.cc banentry.cc bot.cc channel.cc channellist.cc commands.cc \
	       config.cc connection.cc ctcp.cc dccconnection.cc dccperson.cc \
	       flags.cc games.cc log.cc main.cc mask.cc note.cc notelist.cc \
	       parser.cc person.cc queue.cc server.cc serverconnection.cc \
	       serverlist.cc serverqueue.cc serverqueueitem.cc signal.cc \
	       socket.cc string.cc stringtokenizer.cc telnet.cc telnetspy.cc \
	       todolist.cc user.cc usercommands.cc userlist.cc utils.cc

INCLUDES=ansi.h banentry.h bot.h channel.h channellist.h config.h connection.h \
		ctcp.h dccconnection.h dccperson.h flags.h games.h log.h \
		macros.h mask.h message.h note.h notelist.h parser.h person.h \
		queue.h server.h serverconnection.h serverlist.h serverqueue.h \
		serverqueueitem.h signal.h socket.h string.h stringtokenizer.h \
		telnet.h telnetspy.h todolist.h user.h usercommands.h \
		userlist.h userlistitem.h utils.h

VER_SRC=version.cc
VER_OBJ=version.o
VER_INC=version.h
VER_FLG=-DCOMPILE_STRING="\"`uname -a`\"" -DBUILD_STRING="\"`date`\""

OBJECTS=$(SOURCES:.cc=.o)

TARGET=dumbop

all: $(TARGET)
	$(STRIP) $(TARGET)

debug: $(TARGET)

$(TARGET): $(OBJECTS)
	   $(CPP) $(CFLAGS) $(DEBUG_FLAGS) $(VER_FLG) -c $(VER_SRC)
	   $(CPP) -o $@ $(OBJECTS) $(VER_OBJ) $(LIBS) $(LDFLAGS) $(DEBUG_FLAGS)

%.o: %.cc
	$(CPP) $(CFLAGS) $(DEBUG_FLAGS) -c $<

%.o: %.c
	$(CC) $(CFLAGS) $(DEBUG_FLAGS) -c $<

dep depend:
	makedepend $(SOURCES) 2> /dev/null

clean:
	$(RM) *~ *.o $(TARGET) core Makefile.bak gmon.out

