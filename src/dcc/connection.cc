
#include "config.h"

#include <time.h>

#include "socket.h"
#include "dccconnection.h"

DCCConnection::DCCConnection(Bot *b, String n, unsigned long address, int port)
 : Connection(address, port), bot(b), nuh(n),
   lastSpoken(time(NULL)), autoRemove(true)
{ }

bool
DCCConnection::connect()
{
  if (!socket.connect())
    return false;

  return true;
}

bool
DCCConnection::handleInput()
{
  String line = socket.readLine();

  lastSpoken = time(NULL);

  if (line.length() == 0)
    return true;

#ifdef DEBUG
   if (bot->debug)
    cout << "DCC: <" << line << ">" << endl;
#endif
   
  return false;
}

void
DCCConnection::sendNotice(String message)
{
  socket.writeln(message, true);
}
