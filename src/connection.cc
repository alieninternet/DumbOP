
#include "connection.h"

Connection::Connection(String h, int p, String lip)
{
  socket.setRemoteHostname(h);
  socket.setRemotePort(p);
  if (lip != "")
    socket.setLocalHostname(lip);
}

Connection::Connection(unsigned long a, int p)
{
  socket.setRemoteIP(a);
  socket.setRemotePort(p);
}

Connection::~Connection()
{ }

int
Connection::getFileDescriptor() const
{
  return socket.getFileDescriptor();
}
