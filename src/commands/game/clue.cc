/* src/commands/game/clue.cc
 * CLUE command
 */

#include "config.h"
#include "commands.h"
#include "games.h"
#include "gamequiz.h"
#include "channellist.h"
#include "flags.h"
#include "utils.h"

/* Clue - Send out the clue for the quiz, if one exists
 * Original 11/7/01, Simon Butcher <simonb@alien.net.au>
 */
void Commands::Clue(ServerConnection *cnx, Person *from,
			String channel, String rest)
{
   // Grab the channel info
   Channel *c = cnx->bot->channelList->getChannel(channel);

   // Check if this is a quiz channel
   if (!((cnx->bot->wantedChannels[c->channelName]->flags & CHANFLAG_ALLOW_GAMES) &&
	 (cnx->bot->wantedChannels[c->channelName]->gameflags & GAMEFLAG_QUIZ))) {
      // No comment!
      return;
   }
   
   // Try to get the quiz channel information
   gameQuizChannel *gqc = cnx->bot->games->quiz->channels[c->channelName];
   
   // Check if the quiz is running in this channel or not
   if (!gqc) {
      from->sendNotice(String("The Quiz in \002") + c->channelName + 
		       String("\002 has not started yet, sorry."));
      return;
   }
  
   // Do we actually HAVE a question to clue for?
   if ((!gqc->questionStr.length()) || (gqc->answered)) {
      from->sendNotice(String("There is no question currently asked in \002") +
		       c->channelName + String("\002."));
      return;
   }

   // Make sure a clue is available for that question
   if (gqc->question->clue.length()) {
      // Make sure they can pay for this!
      UserListItem *uli = 
	cnx->bot->userList->getUserListItem(from->getAddress());
      
      if ((!uli) || 
	  !uli->canAfford(DEFAULT_QUIZ_QUESTION_CLUE_COST)) {
	 c->sendNotice(String("Sorry, you need at least ") +
		       String(DEFAULT_QUIZ_QUESTION_HINT_COST) +
		       String(" credits to get this clue."));
	 return;
      }
      
      // Charge them!
      uli->charge(DEFAULT_QUIZ_QUESTION_CLUE_COST);
   
      c->sendNotice(String("Clue: ") +
		    gqc->question->clue + String(" \003"));
   } else {
      from->sendNotice(String("There is no clue for the current question in \002") +
		       c->channelName + String("\002, sorry."));
   }
}

