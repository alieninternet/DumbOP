/* src/games/quiz/parser.cc
 * Quiz run-time parsing routines
 */

#include "gamequiz.h"


/* parseLine - Parse a potential correct answer to a question
 * Original 16/07/01, Simon Butcher <simonb@alien.net.au>
 */
void GameQuiz::parseLine(Channel *channel, Person *from, String line)
{
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

      // If we are in bonus mode, there are a few more things to check for..
      if (gqc->questionLevel >= 0) {
	 // Except I am too lazy to program them just yet..
      }
      
      // Was it answered in this run?
      if (gqc->answered) {
	 // Firstly, calculate the time it took to answer in milliseconds
	 long answerTime = (((games->bot->currentTime.time * 1000) +
			     games->bot->currentTime.millitm) -
			    ((gqc->timeAsked.time * 1000) +
			     gqc->timeAsked.millitm));
	 
	 channel->sendNotice(String("In: ") + Utils::timeBigToStr(answerTime));
	 
	 // Tell the channel this user was correct!
	 channel->sendNotice(String("\002") + from->getNick() + 
			     String("\002 is correct! The answer was \"\002") +
			     *gqc->question->answers.begin() +
			     String("\002\". \003"));
	 
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
