
#include <fstream.h>

#include "userlist.h"
#include "stringtokenizer.h"

UserList::UserList(String filename)
  : listFilename(filename)
{
  read();
}

UserList::~UserList()
{
  clear();
}

void UserList::read()
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
      StringTokenizer st(temp);
      if (st.countTokens(':') != 8) {
	 cerr << "Error when reading userlist (" << listFilename <<
	   ") line " << line << "...\n";
	 return;
      }
      String mask = st.nextToken(':');
      String maskChannel = st.nextToken(':');
      String level = st.nextToken(':');
      String prot = st.nextToken(':');
      String nicks = st.nextToken(':');
      String flags = st.nextToken(':');
      String lastseen = st.nextToken(':');
      String password = st.rest().trim();
      if (password == "*NONE*")
	password = "";
      l.push_back(new UserListItem(mask, maskChannel, atoi(level),
				   atoi(prot), nicks,
				   atol(flags),
				   ((atol(lastseen) == 0) ? 
				    -1 : atol(lastseen)),
				   password));
      line++;
   }
   file.close();
}

void UserList::save()
{
   list<UserListItem *>::iterator it = l.begin();
   ofstream file(listFilename);
   
   if (!file)
     return;
   
   ++it; // We skip the bot's entry
   for ( ; it != l.end(); ++it) {
      file << (*it)->mask.getMask().toLower() << ":"
	<< (*it)->channelMask.getMask().toLower() << ":"
	<< (*it)->level << ":"
	<< (*it)->prot << ":"
	<< (*it)->nicks << ":"
	<< (*it)->flags << ":"
	<< (*it)->lastseen << ":";
      if ((*it)->passwd == "")
	file << "*NONE*";
      else
	file << (*it)->passwd;
      file << endl;
   }
}

void UserList::clear()
  {
     UserListItem *uli;
     
     while (!l.empty()) {
	uli = (*l.begin());
	l.erase(l.begin());
	delete uli;
     }
  }

void UserList::addUser(String m, String mc, int lev, int p, String n,
		       long f, time_t ls, String pa)
  {
     l.push_back(new UserListItem(m, mc, lev, p, n, f, ls, pa));
  }

void UserList::addUserFirst(String m, String mc, int lev, int p,
			    String n, long f, time_t ls, String pa)
  {
     l.push_front(new UserListItem(m, mc, lev, p, n, f, ls, pa, true));
  }

UserListItem *UserList::getUserListItem(String nuh, String channel)
  {
     for (list<UserListItem *>::iterator it = l.begin();
	  it != l.end(); ++it)
       if ((*it)->matches(nuh, channel)) {
	  return (*it);
       }
     
     return 0;
  }

int UserList::getMaxLevel(String nuh)
  {
     int level = -1;
     
     for (list<UserListItem *>::iterator it = l.begin();
	  it != l.end(); it++)
       if ((*it)->matches(nuh) && level < (*it)->level &&
	   ((*it)->passwd == "" || (*it)->identified > 0))
	 level = (*it)->level;
     
     return level;
  }

int UserList::getLevel(String nuh, String channel)
  {
     if (UserListItem *uli = getUserListItem(nuh, channel))
       return uli->level;
     
     return -1;
  }

int UserList::getMaxProt(String nuh, String channel)
  {
     int prot = -1;
     
     for (list<UserListItem *>::iterator it = l.begin();
	  it != l.end(); it++) {
	Mask m(nuh), mc(channel), msc((*it)->channelMask.getMask());
	if (m.matches((*it)->mask) &&
	    (mc.matches((*it)->channelMask) || msc.matches(channel)) &&
	    prot < (*it)->prot) {
	   prot = (*it)->prot;
	}
     }
     return prot;
  }

bool UserList::isInUserList(String nuh, String maskChannel)
  {
     for (list<UserListItem *>::iterator it = l.begin();
	  it != l.end();
	  ++it)
       if ((*it)->matches(nuh, maskChannel))
	 return true;
     
     return false;
  }

void UserList::removeFirst()
  {
     UserListItem * uli = *(l.begin());
     if (uli->autoEntry) {
	l.erase(l.begin());
	delete uli;
     };
  }

void UserList::removeUser(String mask, String maskChannel)
  {
     for (list<UserListItem *>::iterator it = l.begin();
	  it != l.end();
	  ++it)
       if ((*it)->mask.getMask() == mask &&
	   (*it)->channelMask.getMask() == maskChannel) {
	  delete (*it);
	  l.erase(it);
	  return;
       }
  }
