/* src/games/quiz/run.cc
 * Quiz running routines
 * Copyright (c) 2001 Alien Internet Services
 */

#include "config.h"
#include "gamequiz.h"
#include "utils.h"


/* attend - Run the quiz channel, or channels
 * Original 13/7/01, Simon Butcher <simonb@alien.net.au>
 */
void GameQuiz::attend(void)
{
   for (map<String, gameQuizChannel *, less<String> >::iterator it =
	channels.begin(); it != channels.end(); ++it) {
      // Rip out the pointer, for our sanity..
      gameQuizChannel *gqc = (*it).second;
      
      // Make sure it is not a broken pointer..
      if (!gqc) {
	 continue; // Next channel
      }

      // Grab the period, again for our sanity
      time_t period = (games->bot->currentTime.time - 
		       gqc->timeAsked.time);
      
      // Check if we are running a question or not...
      if (gqc->questionStr.length()) {
	 // Has the question expired without someone answering it?
	 if ((period >= gqc->confQuestionAskTime) && (!gqc->answered)) {
	    // Reset the question
	    gqc->questionStr = "";
	    
	    // Tell them nobody answered the question
	    gqc->channel->
	      sendNotice(gqc->confColourHilight +
			 String(" TIME IS UP!") +
			 gqc->confColourNormal +
			 String(" Nobody answered that question correctly \003"));
	 } else {
	    // Are we in a question, and it is time for another hint?
	    if (((period % gqc->confQuestionNexthintDelay) == 0) &&
		(gqc->hintLevel > 0) && (gqc->autoHint)) {
	       // Send another hint
	       gqc->sendHint();
	    }
	 }
      } else {
	 // Next question time? or next category time?
	 if (((period >= (gqc->confQuestionAskTime +
			  gqc->confQuestionBetweenDelay)) ||
	      (period >= gqc->confQuestionBetweenDelay) &&
	      gqc->answered) &&
	     (gqc->questionNum <= gqc->confRoundQuestions)) {
	    // Bump in the next question?
	    if (gqc->questionNum < gqc->confRoundQuestions) {
	       // Bump in the next question
	       gqc->bumpQuestion();
	       
	       // Normal question?
	       if (Utils::random(100) > gqc->confBonusQuestionPercentile) {
		  // Copy the question string across normally
		  gqc->questionStr = gqc->question->question;
		  
		  // Tell the rest of the software this question is normal
		  gqc->questionLevel = gameQuizChannel::NORMAL;
		  
		  // Set the points correctly
	       } else {
		  // Bonus question, grab the string for mangling
		  String questionStr = gqc->question->question;
		  
		  // Pick the type of mangling we want...
		  gqc->questionLevel = Utils::random(1);
		  
		  // Select the type of mangling we will do...
		  switch (gqc->questionLevel) {
		   default:
		   case gameQuizChannel::REVERSE: // Reverse the string
		     gqc->questionStr =
		       Utils::reverseStr(questionStr);
		     break;
		   case gameQuizChannel::NO_VOWELS: // Hide the vowels
		     gqc->questionStr = 
		       Utils::replaceVowels(questionStr, '-');
		     break;
		  }
		  
		  // Set the points correctly
		  
		  // Tell the channel it is a bonus question
		  gqc->channel->
		    sendNotice(gqc->confColourHilight +
			       String(" Bonus points for getting this question! \003"));
	       }
	       
	       // Send the question to the channel
	       gqc->channel->
		 sendNotice(gqc->confColourHilight +
			    String(" ") +
			    String(gqc->category) +
			    gqc->confColourNormal +
			    String(" - Question ") +
			    gqc->confColourHilight +
			    String("\002\002") +
			    String(gqc->questionNum) +
			    gqc->confColourNormal +
			    String("/") +
			    gqc->confColourHilight +
			    String("\002\002") +
			    String(gqc->confRoundQuestions) +
			    gqc->confColourNormal +
			    String(":") +
			    gqc->confColourText +
			    String(" ") +
			    gqc->questionStr +
			    String(" \003"));
	       
	       // Reset the time we asked the question variable
	       gqc->timeAsked = games->bot->currentTime;
	    } else if (gqc->questionNum == gqc->confRoundQuestions) {
	       // We are telling the players the round is over..
	       gqc->channel->
		 sendNotice(gqc->confColourNormal +
			    String(" That's the end of the ") +
			    gqc->confColourHilight +
			    gqc->category +
			    gqc->confColourNormal +
			    String(" round... \003")); 
	       
	       // Mark down that we have already ended the round
	       gqc->questionNum++;
	       
	       // We should be doing stats here.
	    }
	 } else if ((period >= (gqc->confQuestionAskTime +
				gqc->confCategoryBetweenDelay)) &&
		    (gqc->questionNum > gqc->confRoundQuestions)) {
	    // Bump in the next category
	    gqc->bumpCategory();
	    
	    // Send the next category to the channel
	    gqc->channel->
	      sendNotice(gqc->confColourNormal +
			 String(" The category for this round will be") +
			 gqc->confColourHilight +
			 String(" ") +
			 gqc->category +
			 gqc->confColourNormal +
			 String(" - ") +
			 gqc->confColourHilight +
			 String("Good luck! \003"));
	 }
      }
   }
}


