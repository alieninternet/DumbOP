
#include "usercommands.h"
#include "games.h"
#include "gamequiz.h"
#include "channellist.h"
#include "flags.h"

/* Category - List or change quiz channel category setting for next round
 * Original 9/7/01, Simon Butcher <simonb@alien.net.au>
 */
void UserCommands::Category(ServerConnection *cnx, Person *from,
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

   // Are we are being requested to look for a category and set it.
   if (rest.length() && (rest != '*') && (rest != '-')) {
      // Check if the next category has been already set
      if (gqc->nextCategory.length()) {
	 from->sendNotice(String("The category for the next round in \002") +
			  c->channelName +
			  String("\002 has already been set to \002") +
			  gqc->nextCategory + String("\002, sorry."));
	 return;
      }
      
      // Otherwise, check that the category exists
      if (cnx->bot->games->quiz->categories[rest.toUpper()]) {
	 // Set the next category
	 gqc->nextCategory = rest.toUpper();
	 
	 // Tell them we are setting the category
	 from->sendNotice(String("The category for the next round in \002") +
			  c->channelName + String("\002 will be \002") +
			  rest.toUpper() + String("\002"));
	 return;
      } else {
	 // Could not find the category
	 from->sendNotice(String("I could not find the category \"\002") +
			  rest.toUpper() + String("\002\", sorry."));
	 return; // get rid of this?
      }
   }
   
   // If we get HERE, we know the category was not specified or was invalid.
   from->sendNotice(String("Current Quiz Category in \002") +
		    c->channelName + String("\002 is \002") +
		    gqc->category + String("\002. ") +
		    ((gqc->nextCategory.length() > 0) ?
		     (String("Next category selected is \002") +
		      gqc->nextCategory + String("\002. ")) : 
		     (String("Changing the category for the next round will cost \002") +
		      String(DEFAULT_QUIZ_CATEGORY_CHANGE_COST) + 
		      String("\002 points."))));
   
   // Have we been requested to list the categories too?
   if ((rest[0] == '-') || (rest[0] == '*')) {
      String result = "\002Available Quiz Categories:\002";
      unsigned long totalQuestions = 0;
      
      for (map<String, gameQuizCategory *, less<String> >::iterator it =
	   cnx->bot->games->quiz->categories.begin();
	   it != cnx->bot->games->quiz->categories.end(); ++it) {
	 // Double check the pointer is valid
	 if ((*it).second) {
	    // Add to our tallies
	    totalQuestions += (*it).second->numQuestions;
	    
	    // Start the category string with a space
	    if (result.length()) {
	       result = result + String(" ");
	    }
	    
	    // Check the age of this category
	    if ((cnx->bot->currentTime.time - (*it).second->lastPlayed) <
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
	       from->sendNotice(result.subString(0,(result.length() - 2)));
	       result = "";
	    }   
	 }
      }
      
      // Check if we have a line left to send
      if (result.length() > 0) {
	 from->sendNotice(result.subString(0,(result.length() - 2)));
      }
      
      from->sendNotice(String("\002End of Quiz Category listing\002 (") +
		       String(totalQuestions) + String(" questions from ") +
		       String(cnx->bot->games->quiz->numCategories) +
		       String(" categories)"));
   }
}

/* Repeat - Repeat the last quiz question
 * Original 11/7/01, Simon Butcher <simonb@alien.net.au>
 */
void UserCommands::Repeat(ServerConnection *cnx, Person *from,
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

   // Do we actually HAVE a question to repeat?
   if ((!gqc->questionStr.length()) || (gqc->answered)) {
      from->sendNotice(String("There is no question to repeat in \002") +
		       c->channelName + String("\002."));
      return;
   }
   
   // Tell them the question
   from->sendNotice(String("The question from \002") + gqc->category +
		    String("\002 is: ") + gqc->questionStr);
}


/* Hint - Send out the next hint for the quiz
 * Original 11/7/01, Simon Butcher <simonb@alien.net.au>
 */
void UserCommands::Hint(ServerConnection *cnx, Person *from,
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
      from->sendNotice(String("There is no question to hint for in \002") +
		       c->channelName + String("\002."));
      return;
   }

   // Are we being asked to pull up the expensive hint thingy?
   if (true) {
      // Make sure a hint is available for that question
      if (gqc->question->hint.length()) {
	 c->sendNotice(String("Extra Hint: ") +
		       gqc->question->hint);
      } else {
	 from->sendNotice(String("There is no extra hint for the current question in \002") +
			  c->channelName + String("\002, sorry."));
      }
   }
}
