
DEFINES=

RM=rm -f
CXX=c++ -g
CXXFLAGS=$(DEFINES) -Wall
LDFLAGS=-lnsl -lcrypt  

SOURCES=ansi.c banentry.c bot.c channel.c channellist.c commands.c \
	       connection.c ctcp.c dccconnection.c dccperson.c flags.c \
	       games.c main.c mask.c note.c parser.c person.c queue.c \
	       server.c serverconnection.c serverlist.c serverqueue.c \
	       serverqueueitem.c socket.c string.c stringtokenizer.c \
	       telnet.c telnetspy.c todolist.c user.c usercommands.c \
	       userlist.c utils.c version.c

INCLUDES=ansi.h banentry.h bot.h channel.h channellist.h connection.h \
		ctcp.h dccconnection.h dccperson.h flags.h games.h macros.h \
		mask.h message.h note.h parser.h person.h queue.h server.h \
		serverconnection.h serverlist.h serverqueue.h \
		serverqueueitem.h socket.h string.h stringtokenizer.h \
		telnet.h telnetspy.h todolist.h user.h usercommands.h \
		userlist.h userlistitem.h utils.h version.h

OBJECTS=$(SOURCES:.c=.o)

TARGET=dumbop

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) -o $@ $(OBJECTS) $(LIBS) $(LDFLAGS)

%.o: %.c
	$(CXX) $(CXXFLAGS) -c $<

dep depend:
	makedepend $(SOURCES) 2> /dev/null

clean:
	$(RM) *~ *.o $(TARGET) core Makefile.bak gmon.out

distclean: clean
	$(RM) .pure log TAGS Makefile config.* configure

