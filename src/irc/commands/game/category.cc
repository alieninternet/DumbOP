/* src/irc/commands/game/category.cc
 * CATEGORY command
 * Copyright (c) 2001 Alien Internet Services
 */

#include "config.h"

#include "commands.h"
#include "games.h"
#include "gamequiz.h"
#include "channellist.h"
#include "flags.h"
#include "utils.h"

/* Category - List or change quiz channel category setting for next round
 * Original 9/7/01, Simon Butcher <simonb@alien.net.au>
 */
void Commands::Category(Person *from, String channel, String rest)
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
   
   // Are we are being requested to look for a category and set it.
   if (rest.length() && (rest != '*') && (rest != '-')) {
      // Check if the next category has been already set
      if (gqc->nextCategory.length()) {
	 from->sendLine(String("The category for the next round in \002") +
			  c->channelName +
			  String("\002 has already been set to \002") +
			  gqc->nextCategory + String("\002, sorry."));
	 return;
      }
      
      // Fix the potential category to save time..
      String potentialCategory = rest.toUpper();
      
      // Otherwise, check that the category exists
      if (from->cnx->bot->games->quiz->categories[potentialCategory]) {
	 // Calculate how long ago this selected category was played...
	 time_t playedTimeAgo = from->cnx->bot->currentTime.time -
	   from->cnx->bot->games->quiz->categories[potentialCategory]->lastPlayed;
	 
	 // Make sure the category is not the same, or too old...
	 if (potentialCategory == gqc->category) {
	    // Tell the user the category is being played NOW!!
	    from->sendLine(String("But the current category is \002") +
			     potentialCategory + String("\002!!"));
	 } else if (playedTimeAgo < DEFAULT_QUIZ_CATEGORY_LOCKOUT_TIME) {
	    // Tell the user the category is too old, and when to try next.
	    from->sendLine(String("The category \002") + potentialCategory +
			     String("\002 was played too recently to select. Try again in \002") +
			     Utils::timelenToStr(DEFAULT_QUIZ_CATEGORY_LOCKOUT_TIME -
						 playedTimeAgo) +
			     String("\002."));
	 } else {
	    // Make sure they can pay for this!
	    UserListItem *uli = 
	      from->cnx->bot->userList->getUserListItem(from->getAddress());

	    if ((!uli) || 
		!uli->canAfford(DEFAULT_QUIZ_CATEGORY_CHANGE_COST)) {
	       c->sendNotice(String("Sorry, you need at least ") +
			     String(DEFAULT_QUIZ_CATEGORY_CHANGE_COST) +
			     String(" credits to change the category."));
	       return;
	    }

	    // We are OK! Set the next category, and take some credits away
	    gqc->nextCategory = potentialCategory;
	    uli->charge(DEFAULT_QUIZ_CATEGORY_CHANGE_COST);

	    // Tell them we are setting the category
	    c->sendNotice(String("The category for the next round in \002") +
			  c->channelName + String("\002 has been changed by \002") +
			  from->getNick() + String("\002 to \002") +
			  rest.toUpper() + String("\002"));
	 }
	 return;
      } else {
	 // Could not find the category
	 from->sendLine(String("I could not find the category \"\002") +
			  rest.toUpper() + String("\002\", sorry."));
	 return; // get rid of this?
      }
   }
   
   // If we get HERE, we know the category was not specified or was invalid.
   from->sendLine(((gqc->questionNum <= DEFAULT_QUIZ_ROUND_QUESTIONS) ?
		     (String("Current Quiz Category in \002") +
		      c->channelName + String("\002 is \002") +
		      gqc->category + String("\002. ")) : String("")) +
		    ((gqc->nextCategory.length() > 0) ?
		     (String("Next category selected is \002") +
		      gqc->nextCategory + String("\002. ")) : 
		     (String("Changing the category for the next round will cost \002") +
		      String(DEFAULT_QUIZ_CATEGORY_CHANGE_COST) + 
		      String("\002 credits."))));
   
   // Have we been requested to list the categories too?
   if ((rest[0] == '-') || (rest[0] == '*')) {
      String result = "\002Available Quiz Categories:\002";
      unsigned long totalQuestions = 0;
      
      for (map<String, gameQuizCategory *, less<String> >::iterator it =
	   from->cnx->bot->games->quiz->categories.begin();
	   it != from->cnx->bot->games->quiz->categories.end(); ++it) {
	 // Double check the pointer is valid
	 if ((*it).second) {
	    // Add to our tallies
	    totalQuestions += (*it).second->numQuestions;
	    
	    // Start the category string with a space
	    if (result.length()) {
	       result = result + String(" ");
	    }
	    
	    // Check the age of this category
	    if ((from->cnx->bot->currentTime.time - (*it).second->lastPlayed) <
		DEFAULT_QUIZ_CATEGORY_LOCKOUT_TIME) {
	       // Too old to select
	       result = result + (*it).first;
	    } else {
	       // OK to select
	       result = result + String("\037") + (*it).first + String("\037");
	    }
	    
	    /* End the category name including the number of questions.
	     * Notice how we do not put a space here, this is to avoid any
	     * line wrappers splitting apart small category names and the
	     * number of questions and making it harder to read.
	     */
	    result = result + String("(") + 
	      String((*it).second->numQuestions) + String("),");
	    
	    // Check if the category list is too long yet
	    if (result.length() >= 350) {
	       // Notice how we chop the last char (a comma) off
	       from->sendLine(result.subString(0,(result.length() - 2)));
	       result = "";
	    }   
	 }
      }
      
      // Check if we have a line left to send
      if (result.length() > 0) {
	 from->sendLine(result.subString(0,(result.length() - 2)));
      }
      
      from->sendLine(String("\002End of Quiz Category listing\002 (") +
		       String(totalQuestions) + String(" questions from ") +
		       String(from->cnx->bot->games->quiz->numCategories) +
		       String(" categories)"));
   }
}

