#ifndef __BOT_H_
# define __BOT_H_

/* WARNING! __ANY__ changed to this file will require a COMPLETE RECOMPILE */

# include <time.h>
# include <set.h>
# include <fstream.h>
# include <sys/timeb.h>

# include "strings.h"
# include "person.h"
# include "serverlist.h"
# include "channellist.h"
# include "userlist.h"
# include "notelist.h"
# include "todolist.h"
# include "parser.h"
# include "telnet.h"
# include "smtp.h"
# include "snmp.h"
# include "signals.h"
# include "version.h"
# include "games.h"

/* THIS SHOULD BE TEMPORARY
 * shhh scott :)
 */
# define AUSTNET_PASSWORD "k11mgc72pb"

class Channel;
class DCCConnection;
class Parser;
class DCCParser;
class ServerConnection;
class Commands;
class Utils;
class Telnet;

class Bot {
 public:
   String nickName;
   String wantedNickName;
   String userName;
   String ircName;
   String userHost;
   String localIP;
   char commandChar;
   
   String configFileName;
   String userListFileName;
   String channelListFileName;
   String noteListFileName;
   String logFileName;
   String helpFileName;
   String initFileName;
   String quizDirectory;
   
   ofstream logFile;
   
   bool connected;
# ifdef DEBUG
   bool debug;
# endif
   bool stop;
   bool sentPing;
   
   Signals *sigs;
   
   ChannelList * channelList;
   UserList * userList;
   NoteList * noteList;
   ServerList * serverList;
   TodoList * todoList;
   
   list<class userFunction *> userFunctions;
   list<class CTCPFunction *> CTCPFunctions;
   
   map<String, wantedChannel *, less<String> > wantedChannels;
   
   map<String, unsigned int, less<String> > ignoredUserhosts;
   
   time_t startTime;				// Time the bot was started
   time_t lastNickNameChange;			// Last time of nick change
   time_t lastChannelJoin;			// Last channel join time
   time_t lastCheckpoint;			// Last database checkpoint
 
   
   ServerConnection *serverConnection;
   list<DCCConnection *> dccConnections;
   
   static const long TELNET_PORT = 4000; // Default port is 4000
   Telnet *telnetDaemon;		// Telnet Management Daemon

   SMTP *SMTPagent;			// E-mail agent

   SNMP *SNMPagent;			// SNMP (Network Management) agent
   
   Games *games;
   
   map<unsigned long, String, less<unsigned long> > userhostMap;
   unsigned long sentUserhostID;
   unsigned long receivedUserhostID;
   
   static const unsigned int MAX_MESSAGES = 20;
   static const time_t NICK_CHANGE  = 300; // 5 mins
   static const time_t CHANNEL_JOIN = 600; // 10 mins
   static const time_t IGNORE_DELAY = 120; // 2 mins
   static const time_t DCC_DELAY = 300; // 5 mins
   static const time_t PING_TIME = 120; // 2 mins *MUST BE SHORTED THAN TIMEOUT*
   static const time_t TIMEOUT = 300; // 5 mins
   static const time_t CHECKPOINT_TIME = 1800; // 30 mins
   
 public:
   
   struct timeb currentTime;
   
# ifdef DEBUG
   Bot(String, bool=false);
# else
   Bot(String);
# endif
   ~Bot();
   
   void run();
   
   void waitForInput();
   
   void logLine(String);
   void readConfig();

   void checkpoint(void);			// Checkpoint the databases
   
   void reconnect();
   
   friend class Parser;
   friend class DCCParser;
   friend class Person;
   friend class Channel;
   friend class Utils;
   friend class ServerConnection;
   friend class Commands;
   friend class Telnet;
   
 private:
   bool canChangeServer();
   void nextServer();
   void connect(int);
   
   void addDCC(Person *, unsigned long, int);
   
   void rehash();
   String getUserhost(String, String);
   bool iAmOp(String);
};

#endif
