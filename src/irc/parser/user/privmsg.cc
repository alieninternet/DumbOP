
#include "config.h"

#include "parser.h"
#include "bot.h"
#include "utils.h"


void Parser::parseUserPrivmsg(Person *from, String rest)
{
   String nick = from->getNick();
   
   StringTokens st(rest);
   String to = st.nextToken();
   String fromUserhost = Utils::getUserhost(from->getAddress());
   
   rest = st.rest().subString(1);
   
   if (++(from->cnx->bot->ignoredUserhosts[fromUserhost]) > Bot::MAX_MESSAGES) {
      if (from->cnx->bot->ignoredUserhosts[fromUserhost] == Bot::MAX_MESSAGES+1) {
	 from->cnx->bot->ignoredUserhosts[fromUserhost] += Bot::IGNORE_DELAY;
	 from->cnx->bot->logLine(from->getAddress() +
			   " is flooding me. We will ignore him/her/it.");
	 if (!Utils::isChannel(to)) {
	    from->sendLine(String("Stop flooding me -- You are now being ignored for ") +
			   String((long)Bot::IGNORE_DELAY) + " seconds.\002");
	 }
      }
      // The following lines reset the counter if you use the
      // command "!sorry" (if '!' is your command char).
      if (rest.toUpper() == String(from->cnx->bot->commandChar) + "SORRY") {
	 from->cnx->bot->ignoredUserhosts[fromUserhost] = 0;
	 from->sendLine("\002Don't do it again!\002");
      }
      return;
   }
   
   if (rest[0] == '\001') {
      rest = rest.subString(1, rest.length() - 2);
#ifdef NOT_VERY_HAPPY_ABOUT_CHANNEL_CTCP_BROADCASTS_WORKING_NO_SIREE_BOB
      if (!Utils::isChannel(to)) 
#endif
	Parser::parseCTCP(from, to, rest);
   } else {
      Parser::parseMessage(from, to, rest);
   }
}


