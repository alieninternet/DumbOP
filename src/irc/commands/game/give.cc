/* src/irc/commands/game/give.cc
 * GIVE command
 * Copyright (c) 2001 Alien Internet Services
 */

#include "config.h"

#include "commands.h"
#include "user.h"
#include "flags.h"
#include "utils.h"


/* Give - Give credits to another user.. Good for 'friendly' users :)
 * Original 31/07/01, Simon Butcher <simonb@alien.net.au>
 * Note: It would be funny if I left an exploit in here.. anyone feel like
 *       giving someone a negative amount? AXS BBS (who stole the name AXS)
 *       had a '/give' command in 'TeleConf', and in the early days of that
 *       command you could /give someone a negative amount of on-line time
 *       and get them to return the favour.. Hours of fun and abuse right
 *       there! Sorry, I wasn't stupid enough the first time to let something
 *       like that slip through, but it would have been fun!!
 */
void Commands::Give(Person *from, String channel, String rest)
{
   // We need the user's details for this function, make sure we have them
   if (!from->uli) {
      return;
   }
   
   // Split apart the command line...
   StringTokens st(rest);
   String creditor = st.nextToken();
   String item = st.nextToken();
   String reason = st.rest();
   
   if (!creditor.length()) {
      from->sendLine("I cannot give credits to a blackhole, sorry.");
      return;
   } else if (creditor.toLower() == from->cnx->bot->nickName.toLower()) {
      from->sendLine("I'm flattered, really I am, but no thanks!");
      return;
   } else if (!item.length()) {
      from->sendLine("I need to know what you're giving to give it!");
      return;
   }
   
   // Check the creditor is valid
   UserListItem *creditorUli =
     from->cnx->bot->userList->getUserListItemNick(creditor);
   
   if (!creditorUli) {
      from->sendLine(String("Sorry, I don't know who \002") + creditor +
		       String("\002 is."));
      return;
   }
   
   // Check the amount is valid, and the transaction is worth it
   long amount = atol(item);
   
   if (amount < 0) {
      from->sendLine(String("Are you trying to steal money from \002") +
		       creditorUli->nick + String("\002? That's not nice!"));
      // send a notice to the person who this guy attempted to rob?
   } else if (amount == 0) {
      from->sendLine("You cannot give away nothing!");
   } else if (!from->uli->canAfford(amount)) {
      from->sendLine("You cannot afford to give that much away!");
   } else if ((creditorUli->flags & USERFLAG_HAS_MONEY_TREE) || 
	      (creditorUli->level >= User::MANAGER)) {
      from->sendLine("That person already has enough credits!");
   } else {
      // Finally, do the transaction!
      from->uli->charge(amount);
      creditorUli->pay(amount);
      
      // Tell the debtor what we did
      from->sendLine(String("Giving \002") + creditorUli->nick + 
		       String(" ") + String(amount) + String("\002 credit") +
		       ((amount > 1) ? String("s") : String("")) +
		       ((reason.length()) ?
			(String(" (Reason: \002") + reason + String("\002)")) :
			String("")) +
		       (((from->uli->flags & USERFLAG_HAS_MONEY_TREE) ||
			 (from->uli->level >= User::MANAGER)) ?
			String("") :
			(String(", leaving you with \002") + 
			 String(from->uli->credits) + 
			 String("\002 credits."))));
      
      // Tell the creditor they got some credits here??
      // Send a note if they are off-line
      // Send a notice if they are on-line
   }
}

