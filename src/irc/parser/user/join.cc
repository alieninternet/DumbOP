
#include "config.h"

#include "parser.h"
#include "bot.h"
#include "channel.h"
#include "flags.h"
#include "utils.h"


void Parser::parseUserJoin(Person *from, String rest)
{
   StringTokens st(from->getAddress());
   String n = st.nextToken('!');
   String uh = st.nextToken();
   StringTokens st2(rest);
   String c = st2.nextToken(':');
   String mode;
   bool joinAndMode = false;
   
   // This part of code is for the combined JOIN & MODE of ircd 2.9
   if (c.find('\007') >= 0) {
      joinAndMode = true;
      StringTokens st3(c);
      c = st3.nextToken('\007');
      String m = st3.rest();
      mode = c + " +" + m;
      
      for (int i = 0; i < m.length(); i++) {
	mode = mode + " " + n;    
      }
   }
   
   if (n == from->cnx->bot->nickName) {
      from->cnx->bot->logLine(String("Joined channel ") + c + ".");
      
      if (from->cnx->bot->wantedChannels.find(c) != from->cnx->bot->wantedChannels.end()) {
	from->cnx->bot->channelList->addChannel(from->cnx, c, from->cnx->bot->wantedChannels[c]->keep);
      } else {
	 from->cnx->bot->channelList->addChannel(from->cnx, c);
      }
      
      from->cnx->queue->sendWho(c);
      from->cnx->queue->sendChannelMode(String("MODE ") + c + " b");
      from->cnx->queue->sendChannelMode(String("MODE ") + c);
   } else {
      Channel * ch = from->cnx->bot->channelList->getChannel(c);
      
      // Make sure we got the channel, just in case
      if (!ch) {
	 return;
      }

      // Add this nickname to the channel
      ch->addNick(n, uh, 0, from->cnx->bot->userList);
      
      // Check if we have a greeting message we should send this user
      if (from->cnx->bot->wantedChannels[ch->channelName]->greeting.length()) {
	 from->sendNotice(from->cnx->bot->wantedChannels[ch->channelName]->greeting);
      }
      
      // If it is a game channel, the game engine may need this data too.
      if (from->cnx->bot->wantedChannels[ch->channelName]->flags & 
	  CHANFLAG_ALLOW_GAMES) {
	 from->cnx->bot->games->parseJoin(ch, from);
	 
	 // Check if this channel is also allowed to do 'credit finding'
	 if (from->cnx->bot->wantedChannels[ch->channelName]->flags &
	     CHANFLAG_ALLOW_FIND_CREDITS) {
	    /* Check if we got the user, if they are identified, and if the
	     * user has 'won' some credits
	     */
	    if ((from->uli) && (from->uli->identified) &&
		!((from->uli->flags & USERFLAG_HAS_MONEY_TREE) ||
		  (from->uli->level >= User::MANAGER)) &&
		(Utils::random(100) <= CHANNEL_JOIN_FIND_PERCENTAGE)) {
	       // Get the amount they found/won
	       short amount = (Utils::random(CHANNEL_JOIN_MAX_CREDIT_FIND) + 
			       1);
	       
	       // Tell the user what they found
	       from->sendLine(String("I felt nice, and have given you \002") +
			      String(amount) + String("\002 credits!"));
	       
	       // Give the credits to the user
	       from->uli->pay(amount);
	    }
	 }
      }
   }
   
   if (joinAndMode) {
      parseUserMode(from, mode);
   }
}


