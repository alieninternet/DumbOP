
#include <time.h>

#include "serverconnection.h"
#include "parser.h"
#include "bot.h"

ServerConnection::ServerConnection(Bot *b, Server *s, String localIP)
  : Connection(s->getHostName(),
               s->getPort(), localIP),
    server(s),
    bot(b),
    queue(new ServerQueue(&socket, b->debug)),
    pingTime(0), lag(0), serverLastSpoken(time(0)), 
    debug(b->debug)
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

bool
ServerConnection::handleInput()
{
  String line = socket.readLine();

  if (line.length() == 0)
    return true;
  
  if (bot->debug)
    cout << line << "\n";
  
  serverLastSpoken = time(0);
  
  Parser::parseLine(this, line);
  
  return false;
}
