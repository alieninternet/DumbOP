
#ifndef __SERVER_H_
#define __SERVER_H_

#include "strings.h"

class Server {

  String hostName;
  int port;
  String password;

public:
  Server(String, int = 6667, String = "");

  String getHostName() const;
  int getPort() const;
  String getPassword() const;

   friend class ServerConnection;
};

#endif
