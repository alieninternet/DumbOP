
#include "config.h"

#include <fstream.h>

#include "userlist.h"
#include "user.h"
#include "utils.h"

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
      StringTokens st(temp);
      if (st.countTokens(':') != 8) {
	 cerr << "Error when reading userlist (" << listFilename <<
	   ") line " << line << "...\n";
	 return;
      }
      String mask = st.nextToken(':');
      String regTime = st.nextToken(':');
      String level = st.nextToken(':');
      String credits = st.nextToken(':');
      String nick = st.nextToken(':');
      String flags = st.nextToken(':');
      String lastseen = st.nextToken(':');
      String password = st.rest().trim();

      if (password == "*NONE*") {
	 password = "";
      }
      
      l.push_back(new UserListItem(mask, atol(regTime), atoi(level),
				   ((atol(credits) < 0) ?
				    0 : atol(credits)),
				   nick, atol(flags),
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
   
   for ( ; it != l.end(); ++it) {
      file << (*it)->mask.getMask().toLower() << ":"
	<< (*it)->registered << ":"
	<< (*it)->level << ":"
	<< (*it)->credits << ":"
	<< (*it)->nick << ":"
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

void UserList::addUser(String m, time_t r, int lev, unsigned long c, String n,
		       long f, time_t ls, String pa)
{
   l.push_back(new UserListItem(m, r, lev, c, n, f, ls, pa));
}

UserListItem *UserList::getUserListItem(String nuh)
  {
     for (list<UserListItem *>::iterator it = l.begin();
	  it != l.end(); ++it)
       if ((*it)->matches(nuh)) {
	  return (*it);
       }
     
     return 0;
  }


/* getUserListItemNick - Grab a userlist item by a nickname
 * Original 31/07/01, Simon Butcher
 * Note: This is a little rough...
 */
UserListItem *UserList::getUserListItemNick(String nick)
{
   for (list<UserListItem *>::iterator it = l.begin();
	it != l.end(); ++it) {
      if ((*it)->nick.toLower() == nick.toLower()) {
	 return (*it);
      }
   }
   
   return 0;
}

int UserList::getMaxLevel(String nuh)
  {
     int level = -1;
     
     for (list<UserListItem *>::iterator it = l.begin();
	  it != l.end(); it++)
       if ((*it)->matches(nuh) && level < (*it)->level &&
	   ((*it)->passwd == "" || (*it)->identified))
	 level = (*it)->level;
     
     // Check we actually got it
     if (level == -1) {
	// Return 'NONE' level
	return User::NONE;
     }
     
     // Return the level we got
     return level;
  }

int UserList::getLevel(String nuh)
  {
     if (UserListItem *uli = getUserListItem(nuh))
       return uli->level;
     
     return -1;
  }


bool UserList::isInUserList(String nuh)
  {
     for (list<UserListItem *>::iterator it = l.begin();
	  it != l.end(); ++it) {
	if ((*it)->matches(nuh)) {
	   return true;
	}
     }
     
     return false;
  }

/* isNickInUserlist - Scan the userlist and check if a nickname in the list
 * Original 19/7/01, Simon Butcher <simonb@alien.net.au>
 */
bool UserList::isNickInUserList(String nick)
{
   for (list<UserListItem *>::iterator it = l.begin();
	it != l.end(); ++it) {
      if ((*it)->nick.toLower() == nick.toLower()) {
	 return true;
      }
   }
   
   return false;
}


/* identify - Identify a user via a given mask and password
 * Original 19/7/01, Simon Butcher <simonb@alien.net.au>
 */
bool UserList::identify(String nick, String password, bool online = true)
{
   bool identified = false;
   UserListItem *uli = 0;

   // Run through the userlist and find this nickname
   for (list<UserListItem *>::iterator it = l.begin(); it != l.end(); ++it) {
      if ((*it)->nick.toLower() == nick.toLower()) {
	 uli = *it;
      }
   }
   
   // Make sure we got a record out of that
   if (!uli) {
      return false;
   }
   
   // If the password is blank, they are automatically identified...
   if (uli->passwd.length()) {
      if (Utils::generateSHA1(password.toLower()) == uli->passwd) {
	 identified = true;
      }
   } else {
      // no password = automatically identified
      identified = true;
   }
   
   // If we are on-line, mark this user with the identification result
   if (online) {
     uli->identified = identified;
   }
   
   // Return the identified status
   return identified;
}


void UserList::removeUser(String mask)
  {
     for (list<UserListItem *>::iterator it = l.begin();
	  it != l.end();
	  ++it)
       if ((*it)->mask.getMask() == mask) {
	  delete (*it);
	  l.erase(it);
	  return;
       }
  }
