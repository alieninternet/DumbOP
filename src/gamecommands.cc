
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
   gameQuizChannel *gqc = cnx->bot->games->quiz->gameQuizChannels[c->channelName];
   
   // Check if the quiz is running in this channel or not
   if (!gqc) {
      from->sendNotice(String("The Quiz in \002") + c->channelName + 
		       String("\002 has not started yet, sorry."));
      return;
   }
   
   // If we get HERE, we know the category was not specified or was invalid.
   from->sendNotice(String("Current Quiz Category in \002") +
		    c->channelName + String("\002 is \002") +
		    String("?") +
		    String("\002. ") +
		    ((false) ?
		     (String("Next category selected is \002") +
		      String("?") +
		      String("\002. ")) : String("")) +
		    String("Changing the category for the next round will cost \002") +
		    String("?") +
		    String("\002 points."));
   
   // Have we been requested to list the categories too?
   if ((rest[0] == '-') || (rest[0] == '*')) {
      String result = "\002Available Quiz Categories:\002";
      unsigned long totalQuestions = 0;
      int numCategories = 0;
      
      for (map<String, gameQuizCategory *, less<String> >::iterator it =
	   cnx->bot->games->quiz->gameQuizCategories.begin();
	   it != cnx->bot->games->quiz->gameQuizCategories.end(); ++it) {
	 // Add to our tallies
	 totalQuestions += ((*it).second)->numQuestions;
	 numCategories++;
	 
	 // Start the category string with a space
	 result = result + String(" ");
	 
	 // Check the age of this category
	 if ((cnx->bot->currentTime.time - ((*it).second)->lastPlayed) <
	     DEFAULT_QUIZ_CATEGORY_LOCKOUT) {
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
	   String(((*it).second)->numQuestions) + String("),");
	 
	 // Check if the category list is too long yet
	 if (result.length() >= 350) {
	    // Notice how we chop the last char (a comma) off
	    from->sendNotice(result.subString(0,(result.length() - 2)));
	    result = "";
	 }   
      }
      
      // Check if we have a line left to send
      if (result.length() > 0) {
	 from->sendNotice(result.subString(0,(result.length() - 2)));
      }
      
      from->sendNotice(String("\002End of Quiz Category listing\002 (") +
		       String(totalQuestions) + 
		       String(" questions from ") +
		       String(numCategories) + String(" categories)"));
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
   gameQuizChannel *gqc = cnx->bot->games->quiz->gameQuizChannels[c->channelName];
   
   // Check if the quiz is running in this channel or not
   if (!gqc) {
      from->sendNotice(String("The Quiz in \002") + c->channelName + 
		       String("\002 has not started yet, sorry."));
      return;
   }
   
   from->sendNotice("The repeat command does not currently do anything.");
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
   gameQuizChannel *gqc = cnx->bot->games->quiz->gameQuizChannels[c->channelName];
   
   // Check if the quiz is running in this channel or not
   if (!gqc) {
      from->sendNotice(String("The Quiz in \002") + c->channelName + 
		       String("\002 has not started yet, sorry."));
      return;
   }
   
   from->sendNotice("The hint command does not do anything.. yet..");
}
