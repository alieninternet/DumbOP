/* src/games/quiz/parser.cc
 * Quiz run-time parsing routines
 * Copyright (c) 2001 Alien Internet Services
 */

#include "config.h"
#include "gamequiz.h"
#include "userlist.h"
#include "utils.h"


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
	 notice = notice + (gqc->confColourHilight +
			    String(" ") + 
			    from->getNick() + 
			    gqc->confColourNormal);
	 
	 // A bonus answer to a bonus question?
	 if (bonusAnswer) {
	    notice = notice + String(" who gets double the bonus points,");
	 }
	 
	 // Check if this was a fast answer ppl should know about
	 if (answerTime <= gqc->confAnswerMaxFastTime) {
	    notice = notice + String(" answered in \002") +
	      Utils::timeBigToStr(answerTime) + String("\002!");
	 } else {
	    // Plain old 'correct'
	    notice = notice + String(" is correct!");
	 }
	 
	 // Append what the answer was..
	 notice = notice + (String(" The answer was \"") +
			    gqc->confColourHilight +
			    String("\002\002") +
			    *(gqc->question->answers.begin()) +
			    gqc->confColourNormal +
			    String("\" \003"));
	 
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
	       points = gqc->confQuestionNormalPoints;
	    } else {
	       // Bonus points
	       points = gqc->confQuestionBonusPoints;
	       
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
 * Note: We do something tricky here. We store their info in a map
 * 	 where the key is their identity (username@hostname) since during
 * 	 their IRC connection that can never change, while their nickname
 * 	 may change. This mechanism is only for statistics so it runs
 * 	 separately to the main client connection map.
 */
void GameQuiz::parseJoin(Channel *channel, Person *from)
{
   // Create a few fresh variables to save time..
   gameQuizChannel *gqc = channels[channel->channelName];
   
   // Make sure we have the quiz channel safely.
   if (!gqc) {
      return;
   }
   
   // Add this person to the appropriate quiz channel
   gqc->players[from->getIdent()] = new gameQuizPlayer();
}


/* parseLeave - Parse a quiz channel player leaving (eg. part or kick etc)
 * Original 27/07/01, Simon Butcher <simonb@alien.net.au>
 * Note: See note on parseJoin..
 */
void GameQuiz::parseLeave(Channel *channel, Person *from)
{
   // Create a few fresh variables to save time..
   gameQuizChannel *gqc = channels[channel->channelName];
   
   // Make sure we have the quiz channel safely.
   if (!gqc) {
      return;
   }
   
   // Grab the player record from the players list in this channel...
   gameQuizPlayer *gqp = gqc->players[from->getIdent()];
   
   // Make sure we got a record (sanity check)
   if (!gqp) {
      return;
   }
   
   // Remove that record from the players list, and delete the player
   gqc->players.erase(from->getIdent());
   delete gqp;
}
