
#include "config.h"

#include "parser.h"
#include "bot.h"
#include "utils.h"


void Parser::parseUserNotice(Person *from, String rest)
{
   String nick = "";
   
   if (from) {
      nick = from->getNick();
   }
   
   StringTokens st(rest);
   String to = st.nextToken();
   
   rest = st.rest().subString(1);
   
   if (rest[0] != '\001') {
    return;
   }
   
   rest = rest.subString(1, rest.length() - 2);
   StringTokens st2(rest);
   String command = st2.nextToken();
   rest = st2.rest();

   if (command == "PING") {
      StringTokens st3(rest);
      time_t time_secs = atol(st3.nextToken(':'));
      unsigned short time_usecs = atoi(st3.rest());
      
      /* The internals of this process can be big -- eg. long long */
      long long pingpong = (((from->cnx->bot->currentTime.time * 1000) + 
			     from->cnx->bot->currentTime.millitm) -
			    ((time_secs * 1000) + time_usecs));
      
      from->sendLine(String("Ping shows a lag between us of \002") +
		     Utils::timeBigToStr((long)pingpong) +
		     String("\002") +
		     ((from->cnx->lag > 0) ? 
		      (String(" (My connection is lagged by \002") +
		       Utils::timeBigToStr(from->cnx->lag) + String("\002)")) :
		      (String(""))));
   }
}


