/* src/irc/server.cc
 * IRC Server list entry routines
 * Copyright (c) 1995-2001 Alien Internet Services
 */

#include "config.h"

#include "server.h"

Server::Server(String h, int p, String pass)
: hostName(h),
  port(p),
  password(pass)
{ 
}

String Server::getHostName() const
{
   return hostName;
}

int Server::getPort() const
{
   return port;
}

String Server::getPassword() const
{
   return password;
}
