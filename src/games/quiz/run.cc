/* src/games/quiz/run.cc
 * Quiz running routines
 */

#include "gamequiz.h"


/* attend - Run the quiz channel, or channels
 * Original 13/7/01, Simon Butcher <simonb@alien.net.au>
 */
void GameQuiz::attend(void)
{
   for (map<String, gameQuizChannel *, less<String> >::iterator it =
	channels.begin(); it != channels.end(); ++it) {
      // Make sure it is not a broken pointer..
      if ((*it).second) {
	 time_t period = (games->bot->currentTime.time - 
			  (*it).second->timeAsked.time);
	 
	 // Check if we are running a question or not...
	 if ((*it).second->questionStr.length()) {
	    // Has the question expired without someone answering it?
	    if ((period >= DEFAULT_QUIZ_QUESTION_ASK_TIME) &&
		(!(*it).second->answered)) {
	       // Reset the question
	       (*it).second->questionStr = "";
	       
	       // Tell them nobody answered the question
	       (*it).second->channel->sendNotice("Nobody got that question. \003");
	    } else {
#ifdef QUIZ_AUTOHINT_THINGY // waste of time and it was not wanted
	       // Are we in a question, and it is time for another hint?
	       if (((period % DEFAULT_QUIZ_QUESTION_NEXTHINT_DELAY) == 0) &&
		   ((*it).second->hintLevel > 0)) {
		  // Send another hint
		  (*it).second->channel->sendNotice(String("Hint: ") +
						    (*it).second->nextHint() +
						    String(" \003"));
	       }
#endif
	    }
	 } else {
	    // Next question time? or next category time?
	    if (((period >= (DEFAULT_QUIZ_QUESTION_ASK_TIME +
			     DEFAULT_QUIZ_QUESTION_BETWEEN_DELAY)) ||
		 (period >= DEFAULT_QUIZ_QUESTION_BETWEEN_DELAY) &&
		 (*it).second->answered) &&
		((*it).second->questionNum <= DEFAULT_QUIZ_ROUND_QUESTIONS)) {
	       // Bump in the next question?
	       if ((*it).second->questionNum < DEFAULT_QUIZ_ROUND_QUESTIONS) {
		  // Bump in the next question
		  (*it).second->bumpQuestion();
		  
		  // Normal question?
		  if (Utils::random(100) > DEFAULT_QUIZ_BONUS_QUESTION_PERCENTILE) {
		     // Copy the question string across normally
		     (*it).second->questionStr = (*it).second->question->question;
		     
		     // Tell the rest of the software this question is normal
		     (*it).second->questionLevel = Q_NORMAL;
		     
		     // Set the points correctly
		  } else {
		     // Bonus question, grab the string for mangling
		     String questionStr = (*it).second->question->question;
		     
		     // Pick the type of mangling we want...
		     (*it).second->questionLevel = Utils::random(1);
		     
		     // Select the type of mangling we will do...
		     switch ((*it).second->questionLevel) {
		      default:
		      case Q_BONUS_REVERSE: // Reverse the string
			(*it).second->questionStr =
			  Utils::reverseStr(questionStr);
			break;
		      case Q_BONUS_NO_VOWELS: // Hide the vowels
			(*it).second->questionStr = 
			  Utils::replaceVowels(questionStr, '-');
			break;
		     }
		     
		     // Set the points correctly

		     // Tell the channel it is a bonus question
		     (*it).second->channel->sendNotice(String("Bonus points for getting this question! \003"));
		  }

		  // Send the question to the channel
		  (*it).second->channel->sendNotice(String("Question ") +
						    String((*it).second->questionNum) +
						    String("/") +
						    String(DEFAULT_QUIZ_ROUND_QUESTIONS) +
						    String(": ") +
						    (*it).second->questionStr +
						    String(" \003"));
		  
		  // Reset the time we asked the question variable
		  (*it).second->timeAsked = games->bot->currentTime;
	       } else if ((*it).second->questionNum == DEFAULT_QUIZ_ROUND_QUESTIONS) {
		  // We are telling the players the round is over..
		  (*it).second->channel->sendNotice(String("That's the end of that round... \003"));
		  
		  // Mark down that we have already ended the round
		  (*it).second->questionNum++;
		  
		  // We should be doing stats here.
	       }
	    } else if ((period >= (DEFAULT_QUIZ_QUESTION_ASK_TIME +
				   DEFAULT_QUIZ_CATEGORY_BETWEEN_DELAY)) &&
		       ((*it).second->questionNum >
			DEFAULT_QUIZ_ROUND_QUESTIONS)) {
	       // Bump in the next category
	       (*it).second->bumpCategory();
	       
	       // Send the next category to the channel
	       (*it).second->channel->sendNotice(String("The category for this round will be \002") +
						 (*it).second->category +
						 String("\002 - Get ready! \003"));
	    }
	    
	    
	 }
      }
   }
}


