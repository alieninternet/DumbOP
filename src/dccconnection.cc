
#include <time.h>

#include "socket.h"
#include "dccconnection.h"

DCCConnection::DCCConnection(Bot *b, String n, unsigned long address, int port)
 : Connection(address, port), bot(b), nuh(n),
   lastSpoken(time(0)), autoRemove(true)
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

  lastSpoken = time(0);

  if (line.length() == 0)
    return true;

  if (bot->debug)
    cout << "DCC: <" << line << ">" << endl;

  return false;
}

void
DCCConnection::sendNotice(String message)
{
  socket.write(message, true);
}
