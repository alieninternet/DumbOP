
#include <fstream.h>
#include <iomanip.h>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <time.h>

#include "bot.h"
#include "dccconnection.h"
#include "stringtokenizer.h"
#include "serverconnection.h"
#include "utils.h"
#include "telnet.h"

#define DEFAULT_NICKNAME "DumbOP"
#define DEFAULT_USERNAME "dumbop"
#define DEFAULT_IRCNAME "DumbOP"
#define DEFAULT_COMMANDCHAR '!'
#define DEFAULT_USERLISTFILENAME "bot.users"
#define DEFAULT_NOTELISTFILENAME "bot.notes"
#define DEFAULT_HELPFILENAME "bot.help"
#define DEFAULT_SCRIPTLOGFILENAME "script.log"
#define DEFAULT_LOGFILENAME "bot.log"
#define DEFAULT_INITFILENAME "bot.init"

/* Bot - Initialise the incredible hulk
 * Original 11/12/00, Pickle <pickle@alien.net.au>
 */
#ifdef DEBUG
Bot::Bot(String filename, bool debug_on)
#else
Bot::Bot(String filename)
#endif
  : nickName(DEFAULT_NICKNAME),
    wantedNickName(DEFAULT_NICKNAME),
    userName(DEFAULT_USERNAME),
    ircName(DEFAULT_IRCNAME),
    userHost(""), localIP(""),
    commandChar(DEFAULT_COMMANDCHAR),
    configFileName(filename),
    userListFileName(DEFAULT_USERLISTFILENAME),
    noteListFileName(DEFAULT_NOTELISTFILENAME),
    logFileName(DEFAULT_LOGFILENAME),
    helpFileName(DEFAULT_HELPFILENAME),
    initFileName(DEFAULT_INITFILENAME),
    receivedLen(0), sentLen(0), connected(false),
#ifdef DEBUG
    debug(debug_on),
#endif
    stop(false), sentPing(false),
    startTime(time(NULL)), currentTime(startTime),
    lastNickNameChange(startTime), lastChannelJoin(startTime),
    serverConnection(0), telnetDaemon(0), sentUserhostID(0), 
    receivedUserhostID(0)
{
   extern userFunctionsStruct userFunctionsInit[];
   extern CTCPFunctionsStruct CTCPFunctionsInit[];
   
#ifdef DEBUG
   if (debug)
     cout << "Clearing lists..." << endl;
#endif
   
   wantedChannels.clear();
   ignoredUserhosts.clear();
   userhostMap.clear();
   
#ifdef DEBUG
   if (debug)
     cout << "Setting up base UserFunctions..." << endl;
#endif
   
   for (int i = 0; userFunctionsInit[i].name[0] != '\0'; i++) {
      userFunctions.push_back(new
			      userFunction(String(userFunctionsInit[i].name),
					   userFunctionsInit[i].function,
					   userFunctionsInit[i].minLevel,
					   userFunctionsInit[i].needsChannelName));
   }

#ifdef DEBUG
   if (debug)
     cout << "Setting up base CTCPFunctions..." << endl;
#endif
   
   for (int i = 0; CTCPFunctionsInit[i].name[0] != '\0'; i++) {
      CTCPFunctions.push_back(new 
			      CTCPFunction(String(CTCPFunctionsInit[i].name),
					   CTCPFunctionsInit[i].function,
					   CTCPFunctionsInit[i].needsIdent,
					   CTCPFunctionsInit[i].hidden,
					   String(CTCPFunctionsInit[i].usage)));
   }
   
#ifdef DEBUG
   if (debug)
     cout << "Opening logfile" << endl;
#endif
   
   logFile.open(logFileName, ios::out | ios::ate | ios::app);
   logLine("Starting log.");
   
#ifdef DEBUG
   if (debug)
     cout << "Setting up lists..." << endl;
#endif
   
   channelList = new ChannelList();
   serverList = new ServerList();
   readConfig();
   userList = new UserList(userListFileName);
   noteList = new NoteList(noteListFileName);
   todoList = new TodoList();
   
#ifdef DEBUG
   if (debug)
     cout << "Adding aliases..." << endl;
#endif
   
   ifstream initFile(initFileName);
   
   if (initFile) {
      String temp, alias, command;
      list<userFunction *>::iterator it;
      bool found = false;
      userFunction *u;
      int line = 0;
      while (initFile >> temp, temp.length() != 0) {
	 line++;
	 StringTokenizer st(temp);
	 temp = temp.trim();
	 if (temp[0]=='#') continue;
	 if (st.countTokens(' ') != 2) {
	    cerr << "Error when reading alias file (" << initFileName <<
	      ") line " << line << "...\n";
	    continue;
	 }
	 alias = st.nextToken().toUpper();
	 command = st.nextToken().toUpper();
	 
	 // Does the function already exist ?
	 found = false;
	 for (it = userFunctions.begin(); it != userFunctions.end(); ++it)
	   if (alias == (*it)->name) {
	      found = true;
	      break;
	   }
	 if (found) continue;
	 
	 // Check that the command exists
	 found = false;
	 for (it = userFunctions.begin(); it != userFunctions.end(); ++it)
	   if (command == (*it)->name) {
	      found = true;
	      u = *it;
	      break;
	   }
	 if (!found) continue;

	 userFunctions.push_back(new userFunction((char *)(const char *)alias,
						  u->function,
						  u->minLevel,
						  u->needsChannelName));
      }
   }

#ifdef DEBUG
   if (debug)
     cout << "Starting telnet Daemon on port " << TELNET_PORT << endl;
#endif
   
   telnetDaemon = new Telnet(this, TELNET_PORT);
   
#ifdef DEBUG
   if (debug)
     cout << "Initialised!" << endl;
#endif
}

/* ~Bot - Shutdown procedure for Bot (clear our memory hogging crap)
 * Original 13/12/00, Pickle <pickle@alien.net.au>
 */
Bot::~Bot()
{
#ifdef DEBUG
   if (debug)
     cout << "Killing DCC connections..." << endl;
#endif
   
   DCCConnection *d;
   while (dccConnections.size() != 0) {
      d = *dccConnections.begin();
      dccConnections.erase(dccConnections.begin());
      delete d;
   }
   
#ifdef DEBUG
   if (debug)
     cout << "Killing UserFunctions..." << endl;
#endif
   
   userFunction *u;
   while (userFunctions.size() != 0) {
      u = *userFunctions.begin();
      userFunctions.erase(userFunctions.begin());
      delete u;
   }
   
#ifdef DEBUG
   if (debug)
     cout << "Killing lists..." << endl;
#endif

   wantedChannel *w;
   while (wantedChannels.size() != 0) {
      w = (*wantedChannels.begin()).second;
      wantedChannels.erase(wantedChannels.begin());
      delete w;
   }
   
#ifdef DEBUG
   if (debug)
     cout << "Dumping userlist and note list..." << endl;
#endif
   
   userList->save();
   noteList->save();
   
#ifdef DEBUG
   if (debug)
     cout << "Killing old pointers..." << endl;
#endif
   
   delete channelList;
   delete userList;
   delete noteList;
   delete todoList;
   delete serverList;
   delete serverConnection;
   delete telnetDaemon;
   
#ifdef DEBUG
   if (debug)
     cout << "Ending logfile..." << endl;
#endif
   
   logLine("Stopping log.");
   logFile.close();
}

/* logLine - Dump a line into the log file
 * Original 13/12/00, Pickle <pickle@alien.net.au>
 */
void Bot::logLine(String line)
{
   struct tm *d;
   time_t current_time = time(NULL);
   
   d = localtime(&current_time);
   logFile << "[" <<  setfill('0') << setw(2)
     << d->tm_mday << "/" <<  setfill('0') << setw(2)
       << d->tm_mon + 1 << "/"
     << d->tm_year + 1900 << " - " <<  setfill('0') << setw(2)
       << d->tm_hour << ":" <<  setfill('0') << setw(2)
	 << d->tm_min << ":" <<  setfill('0') << setw(2)
	   << d->tm_sec << "] "
     << line
     << endl;
}

/* readConfig - Read the primary configuration file for the bot
 * Original 12/12/00, Pickle <pickle@alien.net.au>
 */
void Bot::readConfig()
{
   ifstream file(configFileName);
   String temp;
   int line = 1;
   
   if (!file) {
      logLine(String("I cannot find the file ") + configFileName);
      return;
   }
   
   while (!file.eof()) {
      
      file >> temp;
      
      if (temp.length() == 0 || temp[0] == '#') {
	 line++;
	 continue;
      }
      
      StringTokenizer st(temp);
      String command = st.nextToken('=').trim().toUpper();
      String parameters = st.nextToken('=').trim();
      
      if (command == "NICK" || command == "NICKNAME")
	nickName = wantedNickName = parameters;
      else if (command == "USERNAME")
	userName = parameters;
      else if (command == "IRCNAME" || command == "REALNAME")
	ircName = parameters;
      else if (command == "CMDCHAR" || command == "COMMAND")
	commandChar = parameters[0];
      else if (command == "USERLIST")
	userListFileName = parameters;
      else if (command == "NOTELIST")
	noteListFileName == parameters;
      else if (command == "CHANNEL") {
	 StringTokenizer st2(parameters);
	 String name = st2.nextToken(':').toLower();
	 String mode = st2.nextToken(':');
	 String keep = st2.nextToken(':');
	 String key = st2.nextToken(':');
	 wantedChannels[name] = new wantedChannel(mode, keep, key);
      } else if (command == "LOGFILE")
	   logFileName = parameters;
      else if (command == "INITFILE")
	initFileName = parameters;
      else if (command == "LOCALIP")
	localIP = parameters;
      else if (command == "SERVER") {
	 if (parameters.indexOf(' ') == -1)
	   serverList->addServer(new Server(parameters));
	 else {
	    StringTokenizer st2(parameters);
	    String name = st2.nextToken();
	    int port = atoi(st2.nextToken());
	    serverList->addServer(new Server(name,
					     port,
					     st2.nextToken()));
	 }
      } else {
	 logLine(String("Syntax error in file ") + configFileName +
		 ", line " + String((long)line));
	 file.close();
	 exit(1);
      }
      
      line++;
   }
   
   file.close();
}

/* run - The command that loops
 * Original 12/12/00, Pickle <pickle@alien.net.au>
 */
void Bot::run()
{
#ifdef DEBUG
   if (debug)
     cout << "Connecting..." << endl;
#endif
   
   nextServer();
   
   // This one little while holds the key to my extraordinary multitasking! :)
   while (!stop) {
      waitForInput();
      if (!serverConnection->queue->flush()) {
#ifdef DEBUG
	 if (debug)
	   cout << "Reconnecting... (Queue flushing error)" << endl;
#endif
	 nextServer();
      }
   }
}

/* waitForInput - The main loop procedure for this whole damned contraption
 * Original 12/12/00, Pickle <pickle@alien.net.au>
 * 30/12/00 Pickle - Will ping every PING_TIME secs, regardless of inactivity
 */
void Bot::waitForInput()
{
   fd_set rd;
   struct timeval timer;
   int sock = serverConnection->getFileDescriptor();
   int maxSocketNumber = sock;
   
   // Clear the file descriptor records
   FD_ZERO(&rd);
   
   // Add the Telnet Daemon socket
   FD_SET(telnetDaemon->sock->getFileDescriptor(), &rd);
   if (telnetDaemon->sock->getFileDescriptor() > maxSocketNumber)
     maxSocketNumber = telnetDaemon->sock->getFileDescriptor();
   
   // Add live telnet connections
   for (list<telnetDescriptor *>::iterator it = telnetDaemon->descList.begin();
	it != telnetDaemon->descList.end(); it++)
     if (((*it)->flags & TELNETFLAG_CONNECTED) &&
	 ((*it)->sock->isConnected())) {
	FD_SET((*it)->sock->getFileDescriptor(), &rd);
	if ((*it)->sock->getFileDescriptor() > maxSocketNumber)
	  maxSocketNumber = (*it)->sock->getFileDescriptor();
     }
   
   // Add the IRC server socket
   FD_SET(sock, &rd);
   
   // Make a list of DCC connections and their sockets for select
   for (list<DCCConnection *>::iterator it = dccConnections.begin();
	it != dccConnections.end(); ++it) {
      int s = (*it)->getFileDescriptor();
      FD_SET(s, &rd);
      if (s > maxSocketNumber)
	maxSocketNumber = s;
   }
  
   /* This timer is pretty important, it controls how quickly the queue is
    * flushed during non-input wait states
    */
   timer.tv_sec = 1;
   timer.tv_usec = 0;
   
   // Check for input/output readiness
   switch (select(maxSocketNumber + 1, &rd, NULL, NULL, &timer)) {
    case 0: // Select timed out
      break;
    case -1: // Select broke :(
#ifdef DEBUG
      if (debug) {
	 cout << "Select broke!" << endl;
	 logLine("Select broke on us :(");
      }
#endif
      break;
    default: // Select says there is something to process
      // New connection from the telnet socket
      if (FD_ISSET(telnetDaemon->sock->getFileDescriptor(), &rd))
	if (!telnetDaemon->newConnection()) {
#ifdef DEBUG
	   if (debug)
	     cout << "Could not establish incoming telnet connection" << endl;
#endif
	}
      
      // Check for activity on live telnet sockets
      for (list<telnetDescriptor *>::iterator it = telnetDaemon->descList.begin();
	   it != telnetDaemon->descList.end(); it++)
	if (((*it)->flags & TELNETFLAG_CONNECTED) &&
	    ((*it)->sock->isConnected()) &&
	    FD_ISSET((*it)->sock->getFileDescriptor(), &rd)) {
	   // Stuff should really happen here.
	}
      
      // Something from the IRC socket
      if (FD_ISSET(sock, &rd))
	if (serverConnection->handleInput()) {
#ifdef DEBUG
	   if (debug)
	     cout << "Reconnection... (Input handling error)" << endl;
#endif	   
	   
	   nextServer();
	}
      list<DCCConnection *>::iterator it = dccConnections.begin();
      list<DCCConnection *>::iterator it2;
      
      while (it != dccConnections.end()) {
	 it2 = it;
	 ++it;
	 if (FD_ISSET((*it2)->getFileDescriptor(), &rd)) {
	    if ((*it2)->handleInput()) {
	       delete *it2;
	       dccConnections.erase(it2);
	    }
	 }
      }
   }
   
   // Check ignore list for expired ignores, and run current TODO items
   if (currentTime < time(NULL)) {
      currentTime = time(NULL);
      for (map<String, unsigned int, less<String> >::iterator
           it = ignoredUserhosts.begin();
	   it != ignoredUserhosts.end(); ++it)
	if ((*it).second > 0)
	  (*it).second--;
      
      String line;
      while ((line = todoList->getNext()) != "") {
	 serverConnection->queue->sendChannelMode(line);
      }
   }
   
   // If we are not called what we want to be called, try changing nick
   if ((currentTime >= (time_t)(lastNickNameChange + Bot::NICK_CHANGE)) &&
       (nickName != wantedNickName)) {
      lastNickNameChange = currentTime;
      serverConnection->queue->sendNick(wantedNickName);
      // Authentication should be a little "smarter" I think, using ISON?
      serverConnection->queue->sendNickopIdent(AUSTNET_PASSWORD);
   }
   
   // If we need to join a channel we're supposed to be on, give it a shot
   if (currentTime >= (time_t)(lastChannelJoin + Bot::CHANNEL_JOIN)) {
      lastChannelJoin = currentTime;
      for (map<String, wantedChannel *, less<String> >::iterator it =
           wantedChannels.begin(); it != wantedChannels.end();
	   ++it)
	if (channelList->getChannel((*it).first) == 0)
	  serverConnection->queue->sendJoin((*it).first, (*it).second->key);
   }
   
   // Do our businessed with current DCC connections
   list<DCCConnection *>::iterator it2;
   for (list<DCCConnection *>::iterator it = dccConnections.begin();
	it != dccConnections.end(); ) {
      it2 = it;
      ++it;
      if ((*it2)->autoRemove && currentTime >= (time_t)((*it2)->lastSpoken + Bot::DCC_DELAY)) {
	 delete *it2;
	 dccConnections.erase(it2);
      }
   }

   // Ping the server and calculate our current client <-> server lag
   if (((currentTime >= (time_t)(serverConnection->pingTime + Bot::PING_TIME)) ||
	(currentTime >= (time_t)(serverConnection->serverLastSpoken + Bot::PING_TIME))) &&
       !sentPing) {
#ifdef DEBUG
      serverConnection->queue->sendPing(String((long)currentTime) + ":" +
					String((long)serverConnection->pingTime) + 
					":Check");
#else
      serverConnection->queue->sendPing("Check");
#endif

      serverConnection->pingTime = currentTime;
      sentPing = true;
   }
   
   // If the server is ignoring us, it is probably dead. Time to reconnect.
   if ((currentTime >= (time_t)(serverConnection->serverLastSpoken + Bot::TIMEOUT)) ||
       ((currentTime >= (time_t)(serverConnection->pingTime + Bot::PING_TIME)) && 
	sentPing)) {
#ifdef DEBUG
      if (debug)
	cout << "Reconnection... (Server timed out)" << endl;
#endif
      
      sentPing = false;
      nextServer();
   }
}

// We can change server if we will not lose op on a channel
bool
  Bot::canChangeServer()
{
   String channel;
   Channel *c;
   
   for (map<String, Channel *, less<String> >::iterator it =
	channelList->begin();
	it != channelList->end(); ++it) {
      channel = (*it).first;
      c = channelList->getChannel(channel);
      if (c->countOp == 1 &&
	  c->count > 1 && this->iAmOp(channel))
	return false;
   }
   return true;
}

void
  Bot::nextServer()
{
   bool cont = false;
   
   if (channelList)
     channelList->clear();
   
   if (serverConnection)
     userList->removeFirst();
   
   delete serverConnection;
   
   do {
      Server * s = serverList->nextServer();
      if (!s) {
	 cout << "No server found. Exiting..." << endl;
	 exit(1);
      }
      serverConnection = new ServerConnection(this, s, localIP);
      if (!serverConnection->connect()) {
	 cont = true;
	 // We sleep 10 seconds, to avoid connection flood
	 sleep(10);
	 delete serverConnection;
      } else {
	 cont = false;
      }
   } while (cont);
}

void
  Bot::reconnect()
{
   if (channelList)
     channelList->clear();
   
   userList->removeFirst();
   
   delete serverConnection;
   
   serverConnection =
     new ServerConnection(this, serverList->currentServer(), localIP);
   
   serverConnection->connect();
}

void
  Bot::connect(int serverNumber)
{
   if (channelList)
     channelList->clear();
   
   userList->removeFirst();
   
   delete serverConnection;
   
   serverConnection =
     new ServerConnection(this, serverList->get(serverNumber), localIP);
   
   serverConnection->connect();
}

void
  Bot::addDCC(Person * from, unsigned long address, int port)
{
   DCCConnection * d = new DCCConnection(this, from->getAddress(),
					 address, port);
   
   if (!d->connect())
     return;
   
   dccConnections.push_back(d);
}

void
  Bot::rehash()
{
   for (map<String, Channel *, less<String> >::iterator it = channelList->begin();
	it != channelList->end(); ++it)
     serverConnection->queue->sendWho((*it).first);
}

String
  Bot::getUserhost(String channel, String nick)
{
   Channel *c;
   
   if (channel == "")
     c = 0;
   else
     c = channelList->getChannel(channel);
   
   nick = nick.toLower();
   
   
   if (c && c->hasNick(nick))
     return c->getUser(nick)->userhost;
   
   unsigned long num = sentUserhostID++;
   
   serverConnection->queue->sendUserhost(nick);
   userhostMap[num] = "+";
   
   while (userhostMap[num] == "+") {
      waitForInput();
      serverConnection->queue->flush();
   }
   
   // We have got our answer
   String res = userhostMap[num];
   userhostMap.erase(num);
   
   return res;
}

bool
  Bot::iAmOp(String channel)
{
   User * me = channelList->getChannel(channel)->getUser(nickName);
   return (me->mode & User::OP_MODE);
}
