
#include "server.h"

Server::Server(String h, int p, String pass)
  : hostName(h), port(p), password(pass)
{ }

String
Server::getHostName() const
{
  return hostName;
}

int
Server::getPort() const
{
  return port;
}

String
Server::getPassword() const
{
  return password;
}
