
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

#define DEFAULT_NICKNAME "DumbOP"
#define DEFAULT_USERNAME "dumbop"
#define DEFAULT_IRCNAME "DumbOP"
#define DEFAULT_COMMANDCHAR '!'
#define DEFAULT_USERLISTFILENAME "bot.users"
#define DEFAULT_HELPFILENAME "bot.help"
#define DEFAULT_SCRIPTLOGFILENAME "script.log"
#define DEFAULT_LOGFILENAME "bot.log"
#define DEFAULT_INITFILENAME "bot.init"

Bot::Bot(String filename, bool debug_on)
  : nickName(DEFAULT_NICKNAME),
    wantedNickName(DEFAULT_NICKNAME),
    userName(DEFAULT_USERNAME),
    ircName(DEFAULT_IRCNAME),
    userHost(""),
    localIP(""),
    commandChar(DEFAULT_COMMANDCHAR),
    configFileName(filename),
    userListFileName(DEFAULT_USERLISTFILENAME),
    logFileName(DEFAULT_LOGFILENAME),
    helpFileName(DEFAULT_HELPFILENAME),
    initFileName(DEFAULT_INITFILENAME),
    receivedLen(0), sentLen(0), connected(false),
    debug(debug_on), stop(false), sentPing(false),
    startTime(time(NULL)), currentTime(startTime),
    lastNickNameChange(startTime), lastChannelJoin(startTime),
    serverConnection(0), sentUserhostID(0), receivedUserhostID(0)
{
  extern userFunctionsStruct userFunctionsInit[];

  wantedChannels.clear();
  ignoredUserhosts.clear();
  spyList.clear();
  userhostMap.clear();

  for (int i = 0; userFunctionsInit[i].name[0] != '\0'; i++) {
    userFunctions.push_back(new
                            userFunction(String(userFunctionsInit[i].name),
                                         userFunctionsInit[i].function,
                                         userFunctionsInit[i].minLevel,
                                         userFunctionsInit[i].needsChannelName));
  }

  logFile.open(logFileName, ios::out | ios::ate | ios::app);
  logLine("Starting log.");
  channelList = new ChannelList();
  serverList = new ServerList();
  readConfig();
  userList = new UserList(userListFileName);
  todoList = new TodoList();

  // Let's read the alias file
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

      userFunctions.push_back(new
                              userFunction((char *)(const char *)alias,
                                           u->function,
                                           u->minLevel,
                                           u->needsChannelName));
    }
  }

}

Bot::~Bot()
{
  Person *p;
  while (spyList.size() != 0) {
    p = (*spyList.begin()).second;
    spyList.erase(spyList.begin());
    delete p;
  }
  DCCConnection *d;
  while (dccConnections.size() != 0) {
    d = *dccConnections.begin();
    dccConnections.erase(dccConnections.begin());
    delete d;
  }
  userFunction *u;
  while (userFunctions.size() != 0) {
    u = *userFunctions.begin();
    userFunctions.erase(userFunctions.begin());
    delete u;
  }
  wantedChannel *w;
  while (wantedChannels.size() != 0) {
    w = (*wantedChannels.begin()).second;
    wantedChannels.erase(wantedChannels.begin());
    delete w;
  }
  userList->save();
  delete channelList;
  delete userList;
  delete todoList;
  delete serverList;
  delete serverConnection;
  logLine("Stopping log.");
  logFile.close();
}

void
Bot::logLine(String line)
{
  struct tm *d;
  time_t current_time = time(0);

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

void
Bot::readConfig()
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
    else if (command == "CHANNEL") {
      if (parameters.indexOf(':') == -1) {
        cout << "Warning. The 'channel' syntax has changed."
             << " Please see the README file for more information."
             << " I will use compatibility mode, but you're really"
             << " missing something.\n";
        StringTokenizer st2(parameters);
        String name = st2.nextToken().toLower();
        String key = st2.nextToken();
        wantedChannels[name] = new wantedChannel("", "", key);
      } else {
        StringTokenizer st2(parameters);
        String name = st2.nextToken(':').toLower();
        String mode = st2.nextToken(':');
        String keep = st2.nextToken(':');
        String key = st2.nextToken(':');
        wantedChannels[name] = new wantedChannel(mode, keep, key);
      }
    }
    else if (command == "LOGFILE")
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
    }
    else {
      logLine(String("Syntax error in file ") + configFileName +
              ", line " + String((long)line));
      file.close();
      exit(1);
    }

    line++;
  }

  file.close();
}

void
Bot::run()
{
  nextServer();

  while (!stop) {
    waitForInput();  // This is the main event loop
    if (!serverConnection->queue->flush())
      nextServer();
  }
}

void
Bot::waitForInput()
{
#ifdef _HPUX_SOURCE
  int rd;
#else
  fd_set rd;
#endif
  struct timeval timer;

  int sock = serverConnection->getFileDescriptor();
  int maxSocketNumber = sock;

#ifdef _HPUX_SOURCE
  rd = sock;
#else
  FD_ZERO(&rd);
  FD_SET(sock, &rd);
#endif

  for (list<DCCConnection *>::iterator it = dccConnections.begin();
       it != dccConnections.end(); ++it) {
    int s = (*it)->getFileDescriptor();
#ifdef _HPUX_SOURCE
    rd |= s;
#else
    FD_SET(s, &rd);
#endif
    if (s > maxSocketNumber)
      maxSocketNumber = s;
  }

  timer.tv_sec = 1; /*                           MAIN TIMER??? */
  timer.tv_usec = 0;

  switch (select(maxSocketNumber + 1, &rd, NULL, NULL, &timer)) {
  case 0: /* timeout */
    break;
  case -1: /* error */
    break;
  default: /* normal */
#ifdef _HPUX_SOURCE
    if (rd & sock)
#else
    if (FD_ISSET(sock, &rd))
#endif
      if (serverConnection->handleInput())
        nextServer();

    list<DCCConnection *>::iterator it = dccConnections.begin();
    list<DCCConnection *>::iterator it2;

    while (it != dccConnections.end()) {
      it2 = it;
      ++it;
#ifdef _HPUX_SOURCE
      if (rd & (*it2)->getFileDescriptor()) {
#else
      if (FD_ISSET((*it2)->getFileDescriptor(), &rd)) {
#endif
        if ((*it2)->handleInput()) {
          delete *it2;
          dccConnections.erase(it2);
        }
      }
    }
  }

  if (currentTime < time(NULL)) { // Actions that we do each second
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
#ifdef USESCRIPTS
    botInterp->RunTimers(currentTime);
#endif

#ifdef USESCRIPTS
    struct tm *thisTime = localtime(&currentTime);
    if (thisTime->tm_sec == 0) {
      char s[6];
      sprintf(s, "%2d:%2d", thisTime->tm_hour, thisTime->tm_min);
      botInterp->RunHooks(Hook::TIMER, String(s),
                          gh_list(Utils::string2SCM(String(s)), SCM_UNDEFINED));
    }
#endif

  }

  if (currentTime >= (time_t)(lastNickNameChange + Bot::NICK_CHANGE) &&
      nickName != wantedNickName) {
    lastNickNameChange = currentTime;
    serverConnection->queue->sendNick(wantedNickName);
  }

  if (currentTime >= (time_t)(lastChannelJoin + Bot::CHANNEL_JOIN)) {
    lastChannelJoin = currentTime;
    for (map<String, wantedChannel *, less<String> >::iterator it =
           wantedChannels.begin(); it != wantedChannels.end();
         ++it)
      if (channelList->getChannel((*it).first) == 0)
         serverConnection->queue->sendJoin((*it).first, (*it).second->key);
  }

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

  if (currentTime >= (time_t)(serverConnection->serverLastSpoken + Bot::PING_TIME) && !sentPing) {
    serverConnection->queue->sendPing("Hello?");
    serverConnection->pingTime = time(NULL);
    sentPing = true;
  }

  if (currentTime >= (time_t)(serverConnection->serverLastSpoken + Bot::TIMEOUT)) {
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
