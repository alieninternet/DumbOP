/* src/bot/run.cc
 * Bot - Main loop routines
 * Copyright (c) 1995-2001 Alien Internet Services
 */

#include "config.h"

#include <sys/time.h>

#include "bot.h"
#include "dccconnection.h"


/* run - The command that loops
 * Original 12/12/00, Pickle <pickle@alien.net.au>
 */
void Bot::run()
{
#ifdef DEBUG
   if (debug) {
      cout << "Connecting..." << endl;
   }
#endif
   
   nextServer();
   
   // This one little while holds the key to my extraordinary multitasking! :)
   while (!stop) {
      waitForInput();
      if (!serverConnection->queue->flush()) {
#ifdef DEBUG
	 if (debug) {
	    cout << "Reconnecting... (Queue flushing error)" << endl;
	 }
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
   
   // Grab the current time
   ftime(&currentTime);
   
   // Clear the file descriptor records
   FD_ZERO(&rd);
   
   // Add the Telnet Daemon socket
   FD_SET(telnetDaemon->sock->getFileDescriptor(), &rd);
   if (telnetDaemon->sock->getFileDescriptor() > maxSocketNumber) {
      maxSocketNumber = telnetDaemon->sock->getFileDescriptor();
   }
   
   // Add live telnet connections
   for (list<TelnetDescriptor *>::iterator it = telnetDaemon->descList.begin();
	it != telnetDaemon->descList.end(); it++) {
      if (((*it)->flags & TELNETFLAG_CONNECTED) &&
	  ((*it)->sock->isConnected())) {
	 FD_SET((*it)->sock->getFileDescriptor(), &rd);
	 if ((*it)->sock->getFileDescriptor() > maxSocketNumber) {
	    maxSocketNumber = (*it)->sock->getFileDescriptor();
	 }
      }
   }
   
   // Add the IRC server socket
   FD_SET(sock, &rd);
   
   // Make a list of DCC connections and their sockets for select
   for (list<DCCConnection *>::iterator it = dccConnections.begin();
	it != dccConnections.end(); ++it) {
      int s = (*it)->getFileDescriptor();
      FD_SET(s, &rd);
      if (s > maxSocketNumber) {
	 maxSocketNumber = s;
      }
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
    case -1: // Select broke
#ifdef DEBUG
      if (debug) {
	 cout << "Select prematurely returned!" << endl;
      }
#endif
      break;
    default: // Select says there is something to process
      // New connection from the telnet socket
      if (FD_ISSET(telnetDaemon->sock->getFileDescriptor(), &rd)) {
	 if (!telnetDaemon->newConnection()) {
#ifdef DEBUG
	    if (debug) {
	       cout << "Could not establish incoming telnet connection" << endl;
	    }
#endif
	 }
      }
      
      // Check for activity on live telnet sockets
      for (list<TelnetDescriptor *>::iterator it = telnetDaemon->descList.begin();
	   it != telnetDaemon->descList.end(); it++) {
	 if (((*it)->flags & TELNETFLAG_CONNECTED) &&
	     ((*it)->sock->isConnected()) &&
	     FD_ISSET((*it)->sock->getFileDescriptor(), &rd)) {
	    // Stuff should really happen here.
	    (*it)->handleInput();
	 }
      }
      
      // Something from the IRC socket
      if (FD_ISSET(sock, &rd)) {
	 if (serverConnection->handleInput()) {
#ifdef DEBUG
	    if (debug) {
	       cout << "Reconnection... (Input handling error)" << endl;
	    }
#endif	   
	   
	    nextServer();
	 }
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
   if (currentTime.time < time(NULL)) {
      currentTime.time = time(NULL);
      for (map<String, unsigned int, less<String> >::iterator
           it = ignoredUserhosts.begin();
	   it != ignoredUserhosts.end(); ++it) {
	 if ((*it).second > 0) {
	    (*it).second--;
	 }
      }
      
      String line;
      while ((line = todoList->getNext()) != "") {
	 serverConnection->queue->sendChannelMode(line);
      }
   }
   
   // If we are not called what we want to be called, try changing nick
   if ((currentTime.time >= (time_t)(lastNickNameChange + Bot::NICK_CHANGE)) &&
       (nickName != wantedNickName)) {
      lastNickNameChange = currentTime.time;
      serverConnection->queue->sendNick(wantedNickName);
      // Authentication should be a little "smarter" I think, using ISON?
      serverConnection->queue->sendNickopIdent(AUSTNET_PASSWORD);
   }
   
   // If we need to join a channel we are supposed to be on, give it a shot
   if (currentTime.time >= (time_t)(lastChannelJoin + Bot::CHANNEL_JOIN)) {
      lastChannelJoin = currentTime.time;
      for (map<String, wantedChannel *, less<String> >::iterator it =
           wantedChannels.begin(); it != wantedChannels.end();
	   ++it) {
	 if (channelList->getChannel((*it).first) == 0) {
	    serverConnection->queue->sendJoin((*it).first, (*it).second->key);
	 }
      }
   }
   
   // Do our business with current DCC connections
//   list<DCCConnection *>::iterator it2;
//   for (list<DCCConnection *>::iterator it = dccConnections.begin();
//	it != dccConnections.end(); ) {
//      it2 = it;
//      ++it;
//      if ((*it2)->autoRemove && 
//	  currentTime.time >= (time_t)((*it2)->lastSpoken + Bot::DCC_DELAY)) {
//	 delete *it2;
//	 dccConnections.erase(it2);
//      }
//   }
   
   // Ping the server and calculate our current client <-> server lag
   if (((currentTime.time >= (time_t)(serverConnection->pingTime.time + 
				      Bot::PING_TIME)) ||
	(currentTime.time >= (time_t)(serverConnection->serverLastSpoken + Bot::PING_TIME))) &&
       !sentPing) {
      serverConnection->queue->sendPing(nickName);
      serverConnection->pingTime = currentTime;
      sentPing = true;
   }
   
   // If the server is ignoring us, it is probably dead. Time to reconnect.
   if ((currentTime.time >= (time_t)(serverConnection->serverLastSpoken + Bot::TIMEOUT)) /*||
       ((currentTime.time >= (time_t)(serverConnection->pingTime.time +
				      Bot::PING_TIME)) && 
	sentPing)*/) {
#ifdef DEBUG
      if (debug) {
	 cout << "Reconnection... (Server timed out)" << endl;
      }
#endif
      
      sentPing = false;
      nextServer();
   }

   // Is it time for the almighty database checkpoint sequence to run?
   if (currentTime.time >= (time_t)(lastCheckpoint + Bot::CHECKPOINT_TIME)) {
      checkpoint();
   }
   
   // The telnet console may need our attention - give it a slice
   telnetDaemon->attend();
   
   // The game engine may have some need for some attention too
   games->attend();
}

