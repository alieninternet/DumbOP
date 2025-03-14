/* src/irc/commands/game/clue.cc
 * CLUE command
 * Copyright (c) 2001 Alien Internet Services
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
void Commands::Clue(Person *from, String channel, String rest)
{
   // Grab the channel info
   Channel *c = from->cnx->bot->channelList->getChannel(channel);

   // Check if this is a quiz channel
   if (!((from->cnx->bot->wantedChannels[c->channelName]->flags & CHANFLAG_ALLOW_GAMES) &&
	 (from->cnx->bot->wantedChannels[c->channelName]->gameflags & GAMEFLAG_QUIZ))) {
      // No comment!
      return;
   }
   
   // Try to get the quiz channel information
   gameQuizChannel *gqc = from->cnx->bot->games->quiz->channels[c->channelName];
   
   // Check if the quiz is running in this channel or not
   if (!gqc) {
      from->sendLine(String("The Quiz in \002") + c->channelName + 
		       String("\002 has not started yet, sorry."));
      return;
   }
  
   // Do we actually HAVE a question to clue for?
   if ((!gqc->questionStr.length()) || (gqc->answered)) {
      from->sendLine(String("There is no question currently asked in \002") +
		       c->channelName + String("\002."));
      return;
   }

   // Make sure a clue is available for that question
   if (gqc->question->clue.length()) {
      // Make sure they can pay for this!
      UserListItem *uli = 
	from->cnx->bot->userList->getUserListItem(from->getAddress());
      
      if ((!uli) || 
	  !uli->canAfford(gqc->confQuestionClueCost)) {
	 c->sendNotice(String("Sorry, you need at least ") +
		       String(gqc->confQuestionClueCost) +
		       String(" credits to get this clue."));
	 return;
      }
      
      // Charge them!
      uli->charge(gqc->confQuestionClueCost);
   
      // Send the clue to the channel
      c->sendNotice(gqc->confColourNormal +
		    String(" Question ") +
		    gqc->confColourHilight +
		    String("\002\002") +
		    String(gqc->questionNum) +
		    gqc->confColourNormal +
		    String("/") +
		    gqc->confColourHilight +
		    String("\002\002") +
		    String(gqc->confRoundQuestions) +
		    gqc->confColourNormal +
		    String(" Clue:") +
		    gqc->confColourText +
		    String(" ") +
		    gqc->question->clue +
		    String(" \003"));
   } else {
      from->sendLine(String("There is no clue for the current question in \002") +
		       c->channelName + String("\002, sorry."));
   }
}

