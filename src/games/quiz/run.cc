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
	 time_t period = games->bot->currentTime.time - (*it).second->timeAsked;
	 
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
		  (*it).second->timeAsked = games->bot->currentTime.time;
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


/* parseLine - Parse a potential correct answer to a question
 * Original 16/7/01, Simon Butcher <simonb@alien.net.au>
 */
void GameQuiz::parseLine(Channel *channel, Person *from, String line)
{
   // Create a few fresh variables to save time..
   String potentialAnswer = line.toLower();
   gameQuizChannel *gqc = channels[channel->channelName];
   
   // If this channel does not exist, we will have problems. Bail out now
   if (!gqc) {
      return;
   }
   
   /* First, make sure the channel does exist, and the pointer is OK, and
    * make sure a question has been asked. We need to make sure both the
    * pointer and the question string are available. Also, if the question
    * has already been answered, there's not much point looking up the answer
    * really...
    */
   if ((!gqc) || (!gqc->questionStr.length()) || (gqc->answered)) {
      return;
   }

   // Well we got this far, run through the answer list and look for a match
   for (list<String>::iterator it = gqc->question->answers.begin();
	it != gqc->question->answers.end(); it++) {
      String anAnswer = (*it).toLower();
      
      // Does it match outright? Otherwise we check it another way...
      if (potentialAnswer == anAnswer) {
	 // Mark this question as answered
	 gqc->answered = true;
      }
#ifdef NO_ANSWER_NAZI // Eg, you have more freedom with your answers      
      else if ((potentialAnswer.length() > anAnswer.length()) &&
		 (!isalnum(potentialAnswer[anAnswer.length()])) &&
		 (potentialAnswer.subString(0, anAnswer.length() - 1) == 
		  anAnswer)) {
	 /* This string IS longer than the answer, and the char after the
	  * answer in the potentialAnswer string is not a number or a digit,
	  * therefore we can consider this to be *safe* (eg. a question mark)
	  * and not someone just throwing garble at the parser. We're smarter
	  * than that :)
	  */
	 gqc->answered = true;
      } 
#endif

      // If we are in bonus mode, there are a few more things to check for..
      if (gqc->questionLevel >= 0) {
	 // Except I am too lazy to program them just yet..
      }
      
      // Was it answered in this run?
      if (gqc->answered) {
	 // Tell the channel this user was correct!
	 channel->sendNotice(String("\002") + from->getNick() + 
			     String("\002 is correct! The answer was \"\002") +
			     *gqc->question->answers.begin() +
			     String("\002\". \003"));
	 
	 // Reset the question string and the timeer
	 gqc->questionStr = "";
	 gqc->timeAsked = games->bot->currentTime.time;

	 // Bail out, no more checks needed
	 break;
      }
   }
}


