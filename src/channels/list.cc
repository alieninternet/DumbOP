
#include "channellist.h"

ChannelList::ChannelList(String filename, Bot *b)
  : listFilename(filename), bot(b)
{
   list.clear();
   read();
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

void ChannelList::read()
{
   ifstream file(listFilename);
   String temp, empty = "";
   int line = 1;
   
   clear();
   
   if (!file) {
      cerr << "I cannot find the file " << listFilename << endl;
      return;
   }
   
   while (file >> temp, temp.length() != 0) {
      StringTokens st(temp);
      if (st.countTokens(':') != 8) {
	 cerr << "Error when reading channel list (" << listFilename <<
	   ") line " << line << "...\n";
	 return;
      }
      String name = st.nextToken(':').toLower();
      String mode = st.nextToken(':');
      String keep = st.nextToken(':');
      String key = st.nextToken(':');
      String flags = st.nextToken(':');
      String gameflags = st.nextToken(':');
      String greeting = st.nextToken(':');
      String rules = st.rest().trim();

      if (mode == "-")
	mode = "";
      if (keep == "-")
	keep = "";
      if (key == "-")
	key = "";
      if (greeting == "-")
	greeting = "";
      if (rules == "-")
	rules = "";

      bot->wantedChannels[name] = new wantedChannel(mode, keep, key, 
						    atoi(flags), 
						    atoi(gameflags),
						    greeting, rules);
      
      line++;
   }
   file.close();
}

void ChannelList::save()
{
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

