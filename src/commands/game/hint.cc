
#include "commands.h"
#include "games.h"
#include "gamequiz.h"
#include "channellist.h"
#include "flags.h"
#include "utils.h"

/* Hint - Send out a hint for the quiz
 * Original 24/7/01, Simon Butcher <simonb@alien.net.au>
 */
void Commands::Hint(ServerConnection *cnx, Person *from,
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
  
   // Do we actually HAVE a question to hint for?
   if ((!gqc->questionStr.length()) || (gqc->answered)) {
      from->sendNotice(String("There is no question currently asked in \002") +
		       c->channelName + String("\002."));
      return;
   }

   // Do we have a hint?
   if (gqc->hintLevel <= 0) {
      from->sendNotice(String("There are no hints available for the question in \002") +
		       c->channelName + String("\002") +
		       ((gqc->question->clue.length()) ?
			String(", but there is a clue!") :
			String(".")));
      return;
   }
   
   // Make sure they can pay for this!
   UserListItem *uli = 
     cnx->bot->userList->getUserListItem(from->getAddress());
   
   if ((!uli) || 
       !uli->canAfford(DEFAULT_QUIZ_QUESTION_HINT_COST)) {
      c->sendNotice(String("Sorry, you need at least ") +
		    String(DEFAULT_QUIZ_QUESTION_HINT_COST) +
		    String(" credits to get a hint."));
      return;
   }
   
   // Charge them!
   uli->charge(DEFAULT_QUIZ_QUESTION_HINT_COST);
   
   // We hinted, turn that autohint thing on now
   gqc->autoHint = true;
   
   // Ok well we must have a hint waiting for us if we got here...
   c->sendNotice(String("Hint: ") + gqc->nextHint());
}


