
#include <ctype.h>
#include <stdlib.h>

#include "utils.h"
#include "stringtokenizer.h"
#include "flags.h"

String
Utils::getNick(String nuh)
{
  StringTokenizer st(nuh);
  return st.nextToken('!');
}

String
Utils::getUserhost(String nuh)
{
  StringTokenizer st(nuh);
  st.nextToken('@');
  return st.rest();
}

String Utils::getFirstNick(String in)
{
/*   StringTokenizer st(in);
 *  st.nextToken(',');
 *  return st.rest(); */
   return in;
}

String
Utils::getKey()
{
  return String((long)rand());
}

bool
Utils::isIP(String host)
{
  for (int i = 0; i < host.length(); i++)
    if (!isdigit(host[i]) && host[i]!='.')
      return false;
  return true;
}

String
Utils::makeWildcard(String mask)
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

bool
Utils::isChannel(String c)
{
  return (c[0] == '#' || c[0] == '&');
}

bool
Utils::isWildcard(String c)
{
  return (c.find('*') != -1);
}

bool
Utils::isValidChannelName(String c)
{
  return isChannel(c) && c.find(',') == -1;
}

#define isvalid(c) (((c) >= 'A' && (c) <= '~') || isdigit(c) || (c) == '-')

bool
Utils::isValidNickName(String n)
{
  if (n[0] == '-' || isdigit(n[0]) || n.length() > 9)
    return false;

  for (int i = 0; i < n.length(); i++)
    if (!isvalid(n[i]) || isspace(n[i]))
      return false;

  return true;
}

int
Utils::getLevel(Bot * b, String nuh)
{
  return b->userList->getMaxLevel(nuh);
}

int
Utils::getLevel(Bot * b, String nuh, String channel)
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

String
Utils::levelToStr(int l)
{
  switch (l) {
  case User::USER: return "User";
  case User::TRUSTED_USER: return "Trusted User";
  case User::FRIEND: return "Friend";
  case User::MASTER: return "Master";
  }
  return "None";
}

String
Utils::protToStr(int p)
{
  switch (p) {
  case User::NO_BAN: return "No ban";
  case User::NO_KICK: return "No kick";
  case User::NO_DEOP: return "No deop";
  }
  return "None";
}

String
Utils::boolToStr(bool b)
{
  return b ? "Yes" : "No";
}

String Utils::flagsToStr(long flags)
{
   return "----------";
/*     (String(((flags & USERFLAG_JOIN_AOP) ?
	      USERFLAG_CHR_JOIN_AOP :  
	      ((flags & USERFLAG_JOIN_AOV) ?
	       USERFLAG_CHR_JOIN_AOV :
	       "-"))) +
      String(((flags & USERFLAG_JOIN_GREET) ?
	      USERFLAG_CHR_JOIN_GREET : 
	      "-")) +
      String(((flags & USERFLAG_PROTECT_NOBAN) ?
	      USERFLAG_CHR_PROTECT_NOBAN :
	      ((flags & USERFLAG_PROTECT_NOKICK) ?
	       USERFLAG_CHR_PROTECT_NOKICK :
	       ((flags & USERFLAG_PROTECT_NODEOP) ?
		USERFLAG_CHR_PROTECT_NODEOP :
		((flags & USERFLAG_PROTECT_NODEVOICE) ?
		 USERFLAG_CHR_PROTECT_NODEVOICE : 
		 "-"))))) +
      String(((flags & USERFLAG_IS_BOT) ?
	      USERFLAG_CHR_IS_BOT :
	      ((flags & USERFLAG_IS_CHAN_OWNER) ?
	       USERFLAG_CHR_IS_CHAN_OWNER :
	       "-"))) +
      String(((flags & USERFLAG_SUSPENDED) ?
	      USERFLAG_CHR_SUSPENDED :
	      ((flags & USERFLAG_IDENTIFIED) ?
	       USERFLAG_CHR_IDENTIFIED :
	       "-"))) +
      "-----"); */
}

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
      (((long)(len % 3600) == 0) ? String("") :
       (String((long)(len % 3600) / 60) + " min" + 
	(((long)(len % 3600) >= 2) ? String("s") : String("")) + 
	", ")) +
      (String((long)(len % 60)) + " sec" + 
       (((long)(len % 60) == 1) ? String("") : String("s"))));
}

time_t
Utils::strToTime(String str)
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

  return time(0) + ans;
}

