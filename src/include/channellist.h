#ifndef __CHANNELLIST_H_
#define __CHANNELLIST_H_

#include <map.h>

#include "strings.h"
#include "serverconnection.h"
#include "channel.h"

class ChannelList;

#include "bot.h"

class Utils;
class UserCommands;

class ChannelList {
   String listFilename;
   map<String, Channel *, less<String> > list;

   Bot *bot;
   
public:
  ChannelList(String, Bot *);
  ~ChannelList();

  void addChannel(ServerConnection *, String, String = "");
  void delChannel(String);
  
  Channel * getChannel(String);

   void read();
   void save();
   void clear();

  map<String, Channel *, less<String> >::iterator begin()
    { return list.begin(); }
  map<String, Channel *, less<String> >::iterator end()
    { return list.end(); }

  friend class Utils;
  friend class UserCommands;
};

#endif
