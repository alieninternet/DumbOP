/* src/games/quiz/parser.cc
 * Quiz run-time parsing routines
 */

#include "gamequiz.h"
#include "userlist.h"


/* parseLine - Parse a potential correct answer to a question
 * Original 16/07/01, Simon Butcher <simonb@alien.net.au>
 */
void GameQuiz::parseLine(Channel *channel, Person *from, String line)
{
   bool bonusAnswer = false;
   
   // Create a few fresh variables to save time..
   String potentialAnswer = line.toLower();
   gameQuizChannel *gqc = channels[channel->channelName];
   
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
      else if (gqc->questionLevel > gameQuizChannel::NORMAL) {
	 // If we are in bonus mode, there are a few more things to check for..
	 switch (gqc->questionLevel) {
	  case gameQuizChannel::REVERSE: 
	    if (potentialAnswer == Utils::reverseStr(anAnswer)) {
	       gqc->answered = bonusAnswer = true;
	    }
	    break;
	  case gameQuizChannel::NO_VOWELS:
	    if (potentialAnswer == Utils::replaceVowels(anAnswer, '-')) {
	       gqc->answered = bonusAnswer = true;
	    }
	    break;
	  default: // Everything else we do not handle
	    break;
	 }
      }
      
      // Was it answered in this run?
      if (gqc->answered) {
	 String notice = "";
	 
	 // Firstly, calculate the time it took to answer in milliseconds
	 long answerTime = (((games->bot->currentTime.time * 1000) +
			     games->bot->currentTime.millitm) -
			    ((gqc->timeAsked.time * 1000) +
			     gqc->timeAsked.millitm));

	 // Throw the nick on the string, first off.
	 notice = notice + String("\002") + from->getNick() + String("\002");
	 
	 // A bonus answer to a bonus question?
	 if (bonusAnswer) {
	    notice = notice + String(" who gets double the bonus points,");
	 }
	 
	 // Check if this was a fast answer ppl should know about
	 if (answerTime <= DEFAULT_QUIZ_ANSWER_MAX_FAST_TIME) {
	    notice = notice + String(" answered in \002") +
	      Utils::timeBigToStr(answerTime) + String("\002!");
	 } else {
	    // Plain old 'correct'
	    notice = notice + String(" is correct!");
	 }
	 
	 // Append what the answer was..
	 notice = notice + String(" The answer was \"\002") +
	   *gqc->question->answers.begin() + String("\002\". \003");
	 
	 // Send the notice
	 channel->sendNotice(notice);
	 
	 // Award the points to this user, if we can.
	 UserListItem *uli = 
	   games->bot->userList->getUserListItem(from->getAddress());

	 // They have to be identified to be eligable for the points!
	 if ((uli) && (uli->identified)) {
	    unsigned char points;
	    
	    // Grab the points they are entitled to
	    if (gqc->questionLevel == gameQuizChannel::NORMAL) {
	       // Normal points
	       points = DEFAULT_QUIZ_QUESTION_NORMAL_POINTS;
	    } else {
	       // Bonus points
	       points = DEFAULT_QUIZ_QUESTION_BONUS_POINTS;
	       
	       /* If this user answered specially, they get double the 
		* bonus score
		*/
	       if (bonusAnswer) {
		  points *= 2;
	       }
	    }
	    
	    // Add the points to their credit talley
	    uli->pay(points);
	 }
	 
	 // Reset the question string and the timeer
	 gqc->questionStr = "";
	 gqc->timeAsked = games->bot->currentTime;

	 // Bail out, no more checks needed
	 break;
      }
   }
}


/* parseJoin - Parse a new player joining the quiz channel
 * Original 27/07/01, Simon Butcher <simonb@alien.net.au>
 */
void GameQuiz::parseJoin(Channel *channel, Person *from)
{
   // Create a few fresh variables to save time..
   gameQuizChannel *gqc = channels[channel->channelName];
   
   // Make sure we have the quiz channel safely.
   if (!gqc) {
      return;
   }
}


/* parseLeave - Parse a quiz channel player leaving (eg. part or kick etc)
 * Original 27/07/01, Simon Butcher <simonb@alien.net.au>
 */
void GameQuiz::parseLeave(Channel *channel, Person *from)
{
   // Create a few fresh variables to save time..
   gameQuizChannel *gqc = channels[channel->channelName];
   
   // Make sure we have the quiz channel safely.
   if (!gqc) {
      return;
   }
}
