/* src/bot/bot.cc
 * Bot class - the left overs that need fixing and sorting
 * Copyright (c) 1995-2001 Alien Internet Services
 */

#include "config.h"

#include <fstream.h>
#include <iomanip.h>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <time.h>
#include <stdlib.h>

#include "bot.h"
#include "dccconnection.h"
#include "serverconnection.h"
#include "utils.h"
#include "telnet.h"

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
      
      StringTokens st(temp);
      String command = st.nextToken('=').trim().toUpper();
      String parameters = st.nextToken('=').trim();
      
      if (command == "NICK" || command == "NICKNAME") {
	 nickName = wantedNickName = parameters;
      } else if (command == "USERNAME") {
	 userName = parameters;
      } else if (command == "IRCNAME" || command == "REALNAME") {
	 ircName = parameters;
      } else if (command == "CMDCHAR" || command == "COMMAND") {
	 commandChar = parameters[0];
      } else if (command == "USERLIST") {
	 userListFileName = parameters;
      } else if (command == "CHANNELLIST") {
	 channelListFileName == parameters;
      } else if (command == "NOTELIST") {
	 noteListFileName == parameters;
      } else if (command == "LOGFILE") {
	 logFileName = parameters;
      } else if (command == "INITFILE") {
	 initFileName = parameters;
      } else if (command == "QUIZDIRECTORY") {
	 quizDirectory = parameters;
      } else if (command == "LOCALIP") {
	 localIP = parameters;
      } else if (command == "SERVER") {
	 if (parameters.indexOf(' ') == -1) {
	    serverList->addServer(new Server(parameters));
	 } else {
	    StringTokens st2(parameters);
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


// We can change server if we will not lose op on a channel
bool Bot::canChangeServer()
{
   String channel;
   Channel *c;
   
   for (map<String, Channel *, less<String> >::iterator it =
	channelList->begin();
	it != channelList->end(); ++it) {
      channel = (*it).first;
      c = channelList->getChannel(channel);
      if (c->countOp == 1 &&
	  c->count > 1 && this->iAmOp(channel)) {
	 return false;
      }
   }
   return true;
}


void Bot::nextServer()
{
   bool cont = false;
   
   if (channelList) {
      channelList->clear();
   }
   
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


/* checkpoint - Database checkpoint (check/save) routine
 * Original 03/08/01, Simon Butcher <simonb@alien.net.au>
 */
void Bot::checkpoint(void) {
#ifdef DEBUG
   if (debug)
     cout << "********> Checkpoint." << endl;
#endif

   // Save the userlist
   userList->save();
   
   // Save the notelist
   noteList->save();
   
   // Mark down the new checkpoint time
   lastCheckpoint = currentTime.time;
}


void Bot::reconnect()
{
   if (channelList)
     channelList->clear();
   
   delete serverConnection;
   
   serverConnection =
     new ServerConnection(this, serverList->currentServer(), localIP);
   
   serverConnection->connect();
}

void Bot::connect(int serverNumber)
{
   if (channelList)
     channelList->clear();
   
   delete serverConnection;
   
   serverConnection =
     new ServerConnection(this, serverList->get(serverNumber), localIP);
   
   serverConnection->connect();
}

void Bot::addDCC(Person * from, unsigned long address, int port)
{
   DCCConnection * d = new DCCConnection(this, from->getAddress(),
					 address, port);
   
   if (!d->connect())
     return;
   
   dccConnections.push_back(d);
}

void Bot::rehash()
{
   for (map<String, Channel *, less<String> >::iterator it = 
	channelList->begin();
	it != channelList->end(); ++it) {
      serverConnection->queue->sendWho((*it).first);
   }
}

String Bot::getUserhost(String channel, String nick)
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

bool Bot::iAmOp(String channel)
{
   User * me = channelList->getChannel(channel)->getUser(nickName);
   return (me->mode & User::OP_MODE);
}
