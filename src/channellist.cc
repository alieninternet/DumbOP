
#include "channellist.h"

ChannelList::ChannelList()
{
  list.clear();
}

ChannelList::~ChannelList()
{
  Channel *c;
  map<String, Channel *, less<String> >::iterator it;

  while (list.size()!=0) {
    it = list.begin();
    c = (*it).second;
    list.erase(it);
    delete c;
  }
}

void
ChannelList::addChannel(ServerConnection *cnx, String name, String wantedModes = "")
{
  name = name.toLower();
  list[name] = new Channel(cnx, name, wantedModes);
}

void
ChannelList::delChannel(String name)
{
  name = name.toLower();
  Channel *c = list[name];
  if (c != 0) {
    list.erase(name);
    delete c;
  }
}

Channel *
ChannelList::getChannel(String name)
{
  name = name.toLower();
  Channel *c = list[name];

  if (c)
    return c;

  list.erase(name);
  return 0;
}

void
ChannelList::clear()
{
  Channel *c;
  map<String, Channel *, less<String> >::iterator it;

  while (list.size() != 0) {
    it = list.begin();
    c = (*it).second;
    list.erase(it);
    delete c;
  }
}
