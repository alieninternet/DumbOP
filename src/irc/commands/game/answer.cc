/* src/irc/commands/game/answer.cc
 * ANSWER command
 * Copyright (c) 2001 Alien Internet Services
 */

#include "config.h"

#include "commands.h"
#include "games.h"
#include "gamequiz.h"
#include "channellist.h"
#include "flags.h"
#include "utils.h"


/* Answer - Return the answer of the question
 * Original 01/08/01, Simon Butcher <simonb@alien.net.au>
 * Note: This does not mark the question being asked as 'answered'.
 */
void Commands::Answer(Person *from, String channel, String rest)
{
   // Grab the channel info
   Channel *c = from->cnx->bot->channelList->getChannel(channel);

   // Check if this is a quiz channel
   if (!((from->cnx->bot->wantedChannels[c->channelName]->flags & 
	  CHANFLAG_ALLOW_GAMES) &&
	 (from->cnx->bot->wantedChannels[c->channelName]->gameflags & 
	  GAMEFLAG_QUIZ))) {
      // No comment!
      return;
   }
   
   // Try to get the quiz channel information
   gameQuizChannel *gqc = 
     from->cnx->bot->games->quiz->channels[c->channelName];
   
   // Check if the quiz is running in this channel or not
   if (!gqc) {
      from->sendLine(String("The Quiz in \002") + c->channelName + 
		       String("\002 has not started yet, sorry."));
      return;
   }
  
   // Do we actually HAVE a question to return the answer for?
   if ((!gqc->questionStr.length()) || (gqc->answered)) {
      from->sendLine(String("There is no question currently asked in \002") +
		       c->channelName + String("\002."));
      return;
   }

   from->sendLine(String("The answer for the question in \002") +
		    c->channelName + String("\002 is \"\002") +
		    *gqc->question->answers.begin() + String("\002\"."));
}

