/* src/irc/serverconnection.cc
 * IRC Server connection routines
 * Copyright (c) 1995-2001 Alien Internet Services
 */

#include "config.h"

#include <time.h>

#include "serverconnection.h"
#include "parser.h"
#include "bot.h"

ServerConnection::ServerConnection(Bot *b, Server *s, String localIP)
: socket(new Socket()),
  bot(b),
  server(s),
  type(USER),
  receivedLen(0), 
  sentLen(0), 
  queue(new ServerQueue(this, socket)),
  pingTime(bot->currentTime),
  lag(0),
  serverLastSpoken(bot->currentTime.time)
{
   // We are not connected.
   bot->connected = false;
   
   // Set up the socket nicely
   socket->setRemoteHostname(s->getHostName());
   socket->setRemotePort(s->getPort());
   if (localIP.length()) {
      socket->setLocalHostname(localIP);
   }
}

ServerConnection::~ServerConnection()
{
   // Announce we are going bye bye
//   bot->logLine(String("Closing connection to ") + server->hostName +
//		String(":") + String(server->port) + 
//		String(" (Rx: ") + String(receivedLen) +
//		String(" Tx: ") + String(sentLen) + String(")"));
   
   // Kill stuff off
   delete socket;
   delete queue;
}

int ServerConnection::getFileDescriptor() const
{
   return socket->getFileDescriptor();
}

bool ServerConnection::connect()
{
   if (!socket->connect()) {
      return false;
   }
   
   socket->setNonBlocking();
   
   if (server->getPassword().length() != 0) {
      queue->sendPass(server->getPassword());
   }
   
   queue->sendNick(bot->wantedNickName);
   queue->sendUser(bot->userName, bot->ircName,
		   IRC_MODE_WALLOPS | IRC_MODE_INVISIBLE);
   
   queue->flush();
   
   return true;
}

/* handleInput - Grab a line, quick check it, then send it off for parsing
 * Original 11/12/00, Pickle <pickle@alien.net.au>
 */
bool ServerConnection::handleInput()
{
   String line = socket->readLine();
   
   if (line.length() == 0)
     return true;
   
#ifdef DEBUG
   if (bot->debug)
     cout << line << "\n";
#endif
   
   serverLastSpoken = bot->currentTime.time;
   
   Parser::parseLine(this, line);
   
   return false;
}
