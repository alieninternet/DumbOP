
#include <ctype.h>
#include <stdlib.h>

#include "utils.h"
#include "stringtokenizer.h"
#include "flags.h"
#include "ansi.h"


/* getNick - Grab a nickname from a mask
 * Original 14/12/00, Pickle <pickle@alien.net.au>
 */
String Utils::getNick(String in)
{
   StringTokenizer st(in);
   return st.nextToken('!');
}


/* getUserhost - Grab the userhost from a mask
 * Original 14/12/00, Pickle <pickle@alien.net.au>
 */
String Utils::getUserhost(String in)
{
   StringTokenizer st(in);
   st.nextToken('@');
   return st.rest();
}


/* getFirstNick - Grab the first nickname from a nick list
 * Original 18/12/00, Pickle <pickle@alien.net.au>
 * No longer needed due to new database code??
 */
String Utils::getFirstNick(String in)
{
   /*   StringTokenizer st(in);
    *  st.nextToken(',');
    *  return st.rest(); */
   return in;
}


/* getKey - Generate a random number key
 * Original 14/12/00, Pickle <pickle@alien.net.au>
 */
String Utils::getKey()
{
   return String((long)rand());
}


/* isIP - Check if a string is an IP address
 * Original 14/12/00, Pickle <pickle@alien.net.au>
 */
bool Utils::isIP(String host)
{
   for (int i = 0; i < host.length(); i++)
     if (!isdigit(host[i]) && host[i]!='.')
       return false;
   return true;
}


/* makeWildcard - Generate a wildcard from a complete user variable
 * Original 12/12/00, Pickle <pickle@alien.net.au>
 * Needs: work :)
 */
String Utils::makeWildcard(String mask)
{
   StringTokenizer st(mask);
   
   st.nextToken('!', true);
   String nick = "*";
   
   String user = st.nextToken('@', true);
   if (user[0] == '~' || user[0] == '^' ||
       user[0] == '+' || user[0] == '-' ||
       user[0] == '*')
     user = user.subString(1);
   if (user.length() < 10)
     user = String("*") + user;
   
   String host = st.rest();
   StringTokenizer st2(host);
   if (!isWildcard(host)) {
      if (isIP(host)) {
	 host = st2.nextToken('.') + ".";
	 host = host + st2.nextToken('.') + ".";
	 host = host + st2.nextToken('.') + ".*";
      } else {
	 st2.nextToken('.', true);
	 if (st2.countTokens('.') > 1)
	   host = String("*.") + st2.rest();
      }
   } else {
      if (host == "") host = "*";
   }
   
   cout << nick + "!" + user + "@" + host << endl;
   
   return nick + "!" + user + "@" + host;
}


/* isChannel - Check if a string is a channel name
 * Original 14/12/00, Pickle <pickle@alien.net.au>
 */
bool Utils::isChannel(String c)
{
   return (c[0] == '#' || c[0] == '&');
}


/* isWildcard - Check if string is a wildcarded string
 * Original 14/12/00, Pickle <pickle@alien.net.au>
 */
bool Utils::isWildcard(String c)
{
   return (c.find('*') != -1);
}


/* isValidChannelName - Check if a channel name is allowed
 * Original 14/12/00, Pickle <pickle@alien.net.au>
 */
bool Utils::isValidChannelName(String c)
{
   return isChannel(c) && c.find(',') == -1;
}


/* isValidNickName - Check if a nickname is allowed
 * Original 14/12/00, Pickle <pickle@alien.net.au>
 */
#define isvalid(c) (((c) >= 'A' && (c) <= '~') || isdigit(c) || (c) == '-')
bool Utils::isValidNickName(String n)
{
   if (n[0] == '-' || isdigit(n[0]) || n.length() > 9)
     return false;
   
   for (int i = 0; i < n.length(); i++)
     if (!isvalid(n[i]) || isspace(n[i]))
       return false;
   
   return true;
}

int Utils::getLevel(Bot * b, String nuh)
{
   return b->userList->getMaxLevel(nuh);
}

int Utils::getLevel(Bot * b, String nuh, String channel)
{
   if (!isChannel(channel))
     return getLevel(b, nuh);
   
   if (Channel * c =  b->channelList->getChannel(channel)) {
      User * u = c->getUser(getNick(nuh));
      if (u)
	return u->getLevel();
   } else {
      return -1;
   }
   
   return b->userList->getLevel(nuh, channel);
}


String Utils::levelToStr(int l)
{
   switch (l) {
    case User::USER: return "User";
    case User::TRUSTED_USER: return "Trusted User";
    case User::MANAGER: return "Manager";
    case User::MASTER: return "Master";
   }
   return "None";
}


String Utils::protToStr(int p)
{
   switch (p) {
    case User::NO_BAN: return "No ban";
    case User::NO_KICK: return "No kick";
    case User::NO_DEOP: return "No deop";
   }
   return "None";
}


String Utils::boolToStr(bool b)
{
   return b ? "Yes" : "No";
}


String Utils::flagsToStr(long flags)
{
   return "----------";
}


/* timelenToStr - Convert a length of time_t into a nice string
 * Original 19/12/00, Pickle <pickle@alien.net.au>
 */
String Utils::timelenToStr(time_t len)
{
   return 
     ((((long)(len / 86400) == 0) ? String("") :
       (String((long)(len / 86400)) + " day" +
	(((long)(len / 86400) >= 2) ? String("s") : String("")) + 
	", ")) +
      ((((long)(len % 86400) / 3600) == 0) ? String("") :
       (String((long)(len % 86400) / 3600) + " hour" + 
	((((long)(len % 86400) / 3600) >= 2) ? String("s") : String("")) + 
	", ")) +
      (((long)((len % 3600) / 60) == 0) ? String("") :
       (String((long)((len % 3600) / 60)) + " min" + 
	(((long)((len % 3600) / 60) >= 2) ? String("s") : String("")) + 
	", ")) +
      (String((long)(len % 60)) + " sec" + 
       (((long)(len % 60) == 1) ? String("") : String("s"))));
}

/* timeBigToStr - Convert time in millisecs (time_t * 1000 + millitm) to string
 * Original 27/06/01, Pickle <pickle@alien.net.au>
 */
String Utils::timeBigToStr(long long usecs)
{
   return 
     ((((usecs / 1000) == 0) ? String("") :
       (timelenToStr(usecs / 1000) +
	(((usecs % 1000) == 0) ? String("") :
	 (String(", "))))) +
      ((((usecs % 1000) > 0) || ((usecs / 1000) == 0)) ?
       (String((long)usecs % 1000) + String(" millisec") +
	((usecs % 1000 == 1) ? String("") : String("s"))) :
       String("")));
}


time_t Utils::strToTime(String str)
{
   char num[512];
   int len = 0;
   time_t ans = 0;
   
   num[0]='\0';
   
   if (str.toLower() == "Inf")
     return -1;
   
   if (!isdigit(str[0]))
     return 0;
   
   for (int i = 0; i < str.length(); i++) {
      switch (str[i]) {
       case 'y':
       case 'Y':
	 num[len] = '\0';
	 len = 0;
	 ans += (atoi(num) * 31557600);
	 break;
       case 'M':
	 num[len] = '\0';
	 len = 0;
	 ans += (atoi(num) * 2629800);
	 break;
       case 'd':
       case 'D':
	 num[len] = '\0';
	 len = 0;
	 ans += (atoi(num) * 86400);
	 break;
       case 'h':
       case 'H':
	 num[len] = '\0';
	 len = 0;
	 ans += (atoi(num) * 3600);
	 break;
       case 'm':
	 num[len] = '\0';
	 len = 0;
	 ans += (atoi(num) * 60);
	 break;
       case 's':
       case 'S':
	 num[len] = '\0';
	 len = 0;
	 ans += atoi(num);
       default:
	 if (isdigit(str[i])) {
	    num[len++] = str[i];
	    num[len] = '\0';
	 } else
	   return 0;
      }
   }
   
   if (len)
     ans += atoi(num);
   
   return time(NULL) + ans;
}

/* replaceVowels - Replace all vowels with given character
 * Original 29/06/01, Pickle <pickle@alien.net.au>
 */
String Utils::replaceVowels(String line, char ch)
{
   String temp = "";
   
   for (int i = 0; i < line.length(); i++) {
      switch (line[i]) {
       case 'A':
       case 'a':
       case 'E':
       case 'e':
       case 'I':
       case 'i':
       case 'O':
       case 'o':
       case 'U':
       case 'u': /* A vowel? I'm a lazy bastard programmer hey :) */
	 temp = temp + String(ch);
	 break;
       default:
	 temp = temp + String(line[i]);
	 break;
      }
   }
   
   return temp;
}

/* reverseStr - Reverse a string
 * Original 29/06/01, Pickle <pickle@alien.net.au>
 */
String Utils::reverseStr(String line)
{
   String temp = "";

   for (int i = (line.length() - 1); i >= 0; i--) {
      temp = temp + String(line[i]);
   }
   
   return temp;
}

/* intToDayOfWeek - Return the name of the day from an integer
 * Original 11/7/01, SImon Butcher <simonb@alien.net.au>
 * Note: This conforms to ctime(), eg 0 = Sunday...
 */
String Utils::intToDayOfWeek(int dow)
{
   switch (dow) {
    case 0:
      return "Sunday";
    case 1:
      return "Monday";
    case 2:
      return "Tuesday";
    case 3:
      return "Wednesday";
    case 4:
      return "Thursday";
    case 5:
      return "Friday";
    case 6:
      return "Saturday";
   }
   
   return "?";
}
/* intToMonth - Return the name of the month from an integer
 * Original 11/7/01, SImon Butcher <simonb@alien.net.au>
 * Note: This conforms to ctime(), eg 0 = January...
 */
String Utils::intToMonth(int month)
{
   switch (month) {
    case 0:
      return "January";
    case 1:
      return "February";
    case 2:
      return "March";
    case 3:
      return "April";
    case 4:
      return "May";
    case 5:
      return "June";
    case 6:
      return "July";
    case 7:
      return "August";
    case 8:
      return "September";
    case 9:
      return "October";
    case 10:
      return "November";
    case 11:
      return "December";
   }
   
   return "?";
}
