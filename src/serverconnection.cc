
#include <time.h>

#include "serverconnection.h"
#include "parser.h"
#include "bot.h"

ServerConnection::ServerConnection(Bot *b, Server *s, String localIP)
  : Connection(s->getHostName(),
               s->getPort(), localIP),
    server(s),
    bot(b),
#ifdef DEBUG
    queue(new ServerQueue(&socket, b->debug)),
#else
    queue(new ServerQueue(&socket)),
#endif
    pingTime(time(NULL)), lag(0), 
#ifdef DEBUG
    debug(b->debug),
#endif
    serverLastSpoken(time(NULL))
{
  b->connected = false;
}

ServerConnection::~ServerConnection()
{
  delete queue;
}

bool
ServerConnection::connect()
{
  if (!socket.connect())
    return false;

  socket.setNonBlocking();

  if (server->getPassword().length() != 0)
    queue->sendPass(server->getPassword());
  queue->sendNick(bot->wantedNickName);
  queue->sendUser(bot->userName, bot->ircName);

  queue->flush();
 
  return true;
}

/* handleInput - Grab a line, quick check it, then send it off for parsing
 * Original 11/12/00, Pickle <pickle@alien.net.au>
 */
bool ServerConnection::handleInput()
{
   String line = socket.readLine();
   
   if (line.length() == 0)
     return true;
   
#ifdef DEBUG
   if (bot->debug)
     cout << line << "\n";
#endif
   
   serverLastSpoken = time(NULL);
   
   Parser::parseLine(this, line);
   
   return false;
}
