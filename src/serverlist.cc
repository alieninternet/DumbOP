
#include "config.h"
#include "serverlist.h"

ServerList::ServerList()
  : it(v.end()), current(0), currentNumber(0)
{ }

ServerList::~ServerList()
{
  Server *s;
  for (it = v.begin(); it != v.end(); ++it) {
    s = *it;
    delete s;
  }
}

void
ServerList::addServer(Server *s)
{
  v.push_back(s);
  it = v.end();
}

void
ServerList::delServer(int n)
{
  v.erase(&v[n]);
}

Server *
ServerList::nextServer()
{
  if (v.empty())
    return 0;
  
  if (it == v.end()) {
    it = v.begin();
    currentNumber = 0;
  } else
    currentNumber++;

  return current = *(it++);
}

Server *
ServerList::currentServer()
{
  return current;
}
