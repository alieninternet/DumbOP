
#ifndef __SERVERLIST_H_
#define __SERVERLIST_H_

#include <vector.h>

#include "server.h"

class Commands;

class ServerList {
  vector<Server *> v;
  vector<Server *>::iterator it;
  Server * current;
  int currentNumber;

  vector<Server *>::iterator begin()
    { return v.begin(); }
  vector<Server *>::iterator end()
    { return v.end(); }

public:
  ServerList();
  ~ServerList();
  
  void addServer(Server *);
  void delServer(int);

  Server * nextServer();
  Server * currentServer();

  Server * get(int n) const { return v[n]; }
  int size() { return v.size(); }

  friend class Commands;
};

#endif
