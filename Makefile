
DEFINES=

RM=rm -f
CXX=c++ -g
CXXFLAGS=$(DEFINES) -Wall
LDFLAGS=-lnsl -lcrypt  

SOURCES=banentry.c bot.c channel.c channellist.c commands.c connection.c dccconnection.c dccparser.c dccperson.c main.c mask.c parser.c person.c queue.c server.c serverconnection.c serverlist.c serverqueue.c serverqueueitem.c socket.c string.c stringtokenizer.c todolist.c user.c usercommands.c userlist.c utils.c

INCLUDES=banentry.h bot.h channel.h channellist.h connection.h dccconnection.h dccparser.h dccperson.h macros.h mask.h parser.h person.h queue.h select.h server.h serverconnection.h serverlist.h serverqueue.h serverqueueitem.h socket.h string.h stringtokenizer.h todolist.h user.h usercommands.h userlist.h userlistitem.h utils.h

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

