/* src/bot/class.cc
 * Bot class (Main/Top-Level class)
 * Copyright (c) 1995-2001 Alien Internet Services
 */

#include "config.h"

#include "bot.h"
#include "dccconnection.h"


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
  userHost(""),
  localIP(""),
  commandChar(DEFAULT_COMMANDCHAR),
  configFileName(filename),
  userListFileName(DEFAULT_USERLISTFILENAME),
  channelListFileName(DEFAULT_CHANNELLISTFILENAME),
  noteListFileName(DEFAULT_NOTELISTFILENAME),
  logFileName(DEFAULT_LOGFILENAME),
  helpFileName(DEFAULT_HELPFILENAME),
  initFileName(DEFAULT_INITFILENAME),
  quizDirectory(DEFAULT_QUIZDIRECTORY),
  connected(false),
#ifdef DEBUG
  debug(debug_on),
#endif
  stop(false), 
  sentPing(false),
  sigs(new Signals(this)),
  startTime(time(NULL)), 
  lastNickNameChange(startTime), 
  lastChannelJoin(startTime),
  lastCheckpoint(startTime),
  serverConnection(0), 
  telnetDaemon(0),
  SMTPagent(new SMTP(this)),
  SNMPagent(0),
  games(0),
  sentUserhostID(0), 
  receivedUserhostID(0)
{
   extern userFunctionsStruct userFunctionsInit[];
   extern CTCPFunctionsStruct CTCPFunctionsInit[];

   // Set up first clock run
   ftime(&currentTime);

   // Spawn the random seed for this run
   srand((unsigned int)currentTime.time);
   
#ifdef DEBUG
   if (debug) {
      cout << "Clearing lists..." << endl;
   }
#endif
   
   wantedChannels.clear();
   ignoredUserhosts.clear();
   userhostMap.clear();
   
#ifdef DEBUG
   if (debug) {
      cout << "Setting up base UserFunctions..." << endl;
   }
#endif
   
   for (int i = 0; userFunctionsInit[i].name[0] != '\0'; i++) {
      userFunctions.push_back(new
			      userFunction(String(userFunctionsInit[i].name),
					   userFunctionsInit[i].function,
					   userFunctionsInit[i].minLevel,
					   userFunctionsInit[i].needsChannelName,
					   false));
   }
   
#ifdef DEBUG
   if (debug) {
      cout << "Setting up base CTCPFunctions..." << endl;
   }
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
   if (debug) {
      cout << "Opening logfile" << endl;
   }
#endif
   
   logFile.open(logFileName, ios::out | ios::ate | ios::app);
   logLine("Starting log.");
   logLine(Version::getVersion());
   
#ifdef DEBUG
   if (debug) {
      cout << "Setting up lists..." << endl;
   }
#endif
   
   serverList = new ServerList();
   readConfig();
   userList = new UserList(userListFileName);
   channelList = new ChannelList(channelListFileName, this);
   noteList = new NoteList(noteListFileName);
   todoList = new TodoList();
   
#ifdef DEBUG
   if (debug) {
      cout << "Adding aliases..." << endl;
   }
#endif
   
   ifstream initFile(initFileName);
   
   if (initFile) {
      String temp, alias, command;
      list<userFunction *>::iterator it;
      bool found = false;
      userFunction *u = 0;
      int line = 0;
      while (initFile >> temp, temp.length() != 0) {
	 line++;
	 StringTokens st(temp);
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
	 for (it = userFunctions.begin(); it != userFunctions.end(); ++it) {
	    if (alias == (*it)->name) {
	       found = true;
	       break;
	    }
	 }
	 
	 if (found) {
	    continue;
	 }
	 
	 // Check that the command exists
	 found = false;
	 
	 for (it = userFunctions.begin(); it != userFunctions.end(); ++it) {
	    if (command == (*it)->name) { 
	       found = true;
	       u = *it;
	       break;
	    }
	 }
	 
	 if (!found) {
	    continue;
	 }

	 userFunctions.push_back(new userFunction((char *)(const char *)alias,
						  u->function,
						  u->minLevel,
						  u->needsChannelName,
						  true));
      }
   }
   
#ifdef DEBUG
   if (debug) {
      cout << "Starting telnet Daemon on port " << TELNET_PORT << endl;
   }
#endif
   
   telnetDaemon = new Telnet(this, TELNET_PORT);
   
#ifdef DEBUG
   if (debug) {
      cout << "Starting SNMP agent" << endl;
   }
#endif
   
   SNMPagent = new SNMP(this);
   
#ifdef DEBUG
   if (debug) {
      cout << "Setting up internal games..." << endl;
   }
#endif

   games = new Games(this);
   
#ifdef DEBUG
   if (debug) {
      cout << "Initialised!" << endl;
   }
#endif
}


/* ~Bot - Shutdown procedure for Bot (clear our memory hogging crap)
 * Original 13/12/00, Pickle <pickle@alien.net.au>
 */
Bot::~Bot()
{
#ifdef DEBUG
   if (debug) {
      cout << "Killing DCC connections..." << endl;
   }
#endif
   
   DCCConnection *d;
   while (dccConnections.size() != 0) {
      d = *dccConnections.begin();
      dccConnections.erase(dccConnections.begin());
      delete d;
   }
   
#ifdef DEBUG
   if (debug) {
      cout << "Killing UserFunctions..." << endl;
   }
#endif
   
   userFunction *u;
   while (userFunctions.size() != 0) {
      u = *userFunctions.begin();
      userFunctions.erase(userFunctions.begin());
      delete u;
   }
   
#ifdef DEBUG
   if (debug) {
      cout << "Killing lists..." << endl;
   }
#endif
   
   wantedChannel *w;
   while (wantedChannels.size() != 0) {
      w = (*wantedChannels.begin()).second;
      wantedChannels.erase(wantedChannels.begin());
      delete w;
   }
   
#ifdef DEBUG
   if (debug) {
      cout << "Dumping userlist and note list..." << endl;
   }
#endif
   
   userList->save();
   noteList->save();
   
#ifdef DEBUG
   if (debug) {
      cout << "Killing old pointers..." << endl;
   }
#endif
   
   delete sigs;
   delete channelList;
   delete userList;
   delete noteList;
   delete todoList;
   delete serverList;
   delete serverConnection;
   delete telnetDaemon;
   
#ifdef DEBUG
   if (debug) {
      cout << "Ending logfile..." << endl;
   }
#endif
   
   logLine("Stopping log.");
   logFile.close();
}


