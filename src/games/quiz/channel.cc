/* src/games/quiz/channel.cc
 * Quiz channel class
 */

#include <ctype.h>
#include <stdlib.h>

#include "gamequiz.h"


/* gameQuizChannel - Initialise the quiz game run-time class
 * Original 12/7/01, SImon Butcher <simonb@alien.net.au>
 */
gameQuizChannel::gameQuizChannel(Channel *c, GameQuiz *gq)
: channel(c),
  gameQuiz(gq),
  category(""),
  nextCategory(""),
  question(0),
  questionStr(""),
  questionNum(0),
  questionLevel(NORMAL),
  timeAsked(gameQuiz->games->bot->currentTime),
  answered(false),
  hintLevel(-1),
  hint("")
{
   // Fire up a category
   bumpCategory();
   
   // Tell the channel we we just did...!
   channel->sendNotice(String("Starting category will be \002") + category + 
		       String("\002."));
}


/* bumpCategory - Fire up the next category
 * Original 13/7/01, Simon Butcher <simonb@alien.net.au>
 */
void gameQuizChannel::bumpCategory(void) {
   // Have we been requested to change to a specific category?
   if (nextCategory.length()) {
      /* Copy the next category across. The category command checks this
       * so that we don't have to. Phew!
       */
      category = nextCategory;
   } else { // Otherwise we run through the list and find the next
      String potentialCategory = "";
      unsigned char randomAttempts = 0;
      
      while ((!gameQuiz->categories[potentialCategory]) ||
	     ((gameQuiz->games->bot->currentTime.time -
	       gameQuiz->categories[potentialCategory]->lastPlayed) <
	      DEFAULT_QUIZ_CATEGORY_LOCKOUT_TIME)) {
	 // Are we forcing a run-through to find the oldest category?
	 if (randomAttempts < DEFAULT_QUIZ_CATEGORY_RANDOM_ATTEMPTS) {
	    // Randomly select a category
	    
	    // This was a random attempt, increase the attempt counter
	    randomAttempts++;
	 } else {
	    /* No more random, to save time we will just run through the
	     * damned categories and pick whatever we find that has not been
	     * used in a long time.
	     */
	    for (map<String, gameQuizCategory *, less<String> >::iterator it =
		 gameQuiz->categories.begin();
		 it != gameQuiz->categories.end(); ++it) {
	       // Is this older than the one we already have marked down?
	       if ((!potentialCategory.length()) ||
		   ((*it).second->lastPlayed <
		    gameQuiz->categories[potentialCategory]->lastPlayed)) {
		  potentialCategory = (*it).first;
	       }
	    }
	    
	    /* End the while, just in case this category has not expired.
	     * For the quiz to keep going, we MUST break even if we might
	     * be breaking the expiration rules.
	     */
	    break;
	 }
      }
      
      category = potentialCategory;
   }
   
   // Reset the next category selection
   nextCategory = "";

   // Fix the last played variable for this category
   gameQuiz->categories[category]->lastPlayed = 
     gameQuiz->games->bot->currentTime.time;
   
   // Reset the question number counter too
   questionNum = 0;
}


/* bumpQuestion - Fire up the next question
 * Original 14/7/01, Simon Butcher <simonb@alien.net.au>
 * Note: Should the 'find the oldest' method be replaced with a 'find the
 *       first' style loop?
 */
void gameQuizChannel::bumpQuestion(void) {
   gameQuizQuestion *potentialQuestion = 0;
   unsigned char randomAttempts = 0;
   
   while ((!potentialQuestion) ||
	  ((gameQuiz->games->bot->currentTime.time -
	    potentialQuestion->lastPlayed) <
	   DEFAULT_QUIZ_QUESTION_LOCKOUT_TIME)) {
      potentialQuestion = 0;
      
      if (randomAttempts < DEFAULT_QUIZ_QUESTION_RANDOM_ATTEMPTS) {
	 // Try and pull the question out of the vector sequence
	 potentialQuestion =
	   gameQuiz->categories[category]->questions[Utils::random(gameQuiz->categories[category]->questions.size() - 2) + 1];
	 
	 // This was a random attempt, increase the attempt counter
	 randomAttempts++;
      } else {
	 /* No more random attempts, we gotta stop mucking around and just
	  * grab the first acceptable one we can find
	  */
	 for (vector<gameQuizQuestion *>::iterator it = 
	      gameQuiz->categories[category]->questions.begin();
	      it != gameQuiz->categories[category]->questions.end(); ++it) {
	    // Check if this is older than the one we have marked down
	    if ((!potentialQuestion) ||
		((*it)->lastPlayed < potentialQuestion->lastPlayed)) {
	       potentialQuestion = (*it);
	    }
	 }
	 
	 // Break the while loop to avoid any failure to comply to rules issues
	 break;
      }
   }
   
   // Copy the question pointer across.
   question = potentialQuestion;
   
   // Fix the last played variable for this category
   question->lastPlayed = gameQuiz->games->bot->currentTime.time;

   // Reset the answered boolean
   answered = false;

   // Reset the hint
   hintLevel = -1;
   hint = "";
   autoHint = false;
   nextHint();
   
   // Increase the question number counter
   questionNum++;
}


/* nextHint - Fire up the next hint, or create a new one if we have to..
 * Original 24/7/01, Simon Butcher <simonb@alien.net.au>
 * Note: Don't get it confused with CLUE which is a written line, a HINT is
 * 	 merely a hint to the answer, by using the answer.
 */
String gameQuizChannel::nextHint(char chr = '\0') {
   // If there is no point to this, we may as well bail out now..
   if ((hintLevel == 0) || (!question)) {
      return hint;
   }

   // Some handy dandy variables..
   String primaryAnswer = *question->answers.begin();
   String output = "";
   String newHint = "";
   
   // This is only useful if we are looking for a char..
   bool charFound = ((chr == '\0') ? true : false);
   
   // Are we creating a new hint?
   if (hintLevel == -1) {
      // Zero out the hint level..
      hintLevel++;
      
      /* First of all, make sure the primary answer is long enough for a
       * hint, otherwise the hint becomes too easy. We may as well bail out
       * now rather than piss fart around later.
       */
      if (primaryAnswer.length() < DEFAULT_QUIZ_HINT_MIN_LENGTH) {
	 hint = "";
	 return "";
      }
      
      /* Run through the primary answer. We will create both the internal
       * hint string and the user friendly hint string output, while also
       * seeking for a char if it was provided.
       */
      for (int i = 0; i < primaryAnswer.length(); i++) {
	 switch (primaryAnswer[i]) {
	  case '-':
	  case '\'':
	  case ',':
	  case '&':
	  case ' ': // A char we are not going to hide?
	    output = output + String(primaryAnswer[i]);
	    newHint = newHint + String(primaryAnswer[i]);
	    break;
	  default: // Everything else we hide.
	    output = output + String(DEFAULT_QUIZ_HINT_SPECIAL_CHAR_OUTPUT);
	    newHint = newHint + String(DEFAULT_QUIZ_HINT_SPECIAL_CHAR);
	    hintLevel++; // Another level of hintness.. or something..
	 }
      }
   } else { // Otherwise we are modifying an original hint..
      bool gotChar = false;
      unsigned int normCharCount = 0;
      
      // Do not be fooled, this works differently to the creation loop!
      for (int i = 0; i < hint.length(); i++) {
	 switch (hint[i]) {
	  case DEFAULT_QUIZ_HINT_SPECIAL_CHAR: // Super special marker char?
	    // If we have gotten the char, we keep these hidden..
	    if (gotChar) {
	       output = output + String(DEFAULT_QUIZ_HINT_SPECIAL_CHAR_OUTPUT);
	       newHint = newHint + String(hint[i]);
	    } else {
	       // We are definately bumping in a char here.
	       hintLevel--;
	       gotChar = true;
	       normCharCount++;
	       
	       // Check how exactly we are bumping a char tho..
	       if ((!charFound) && 
		   (toupper(primaryAnswer[i]) == toupper(chr))) {
		  // If we are looking for a specific char, this is it!
		  output = output + String("\002") + String(primaryAnswer[i]) +
		    String("\002");
		  newHint = newHint + String(primaryAnswer[i]);
		  charFound = true;
	       } else { // This is the char, and that is that.
		  output = output + String(primaryAnswer[i]);
		  newHint = newHint + String(primaryAnswer[i]);
	       }
	    }
	    break;
	  default: // Everything else we let fall through
	    output = output + String(hint[i]);
	    newHint = newHint + String(hint[i]);
	    normCharCount++;
	 }
      }
      
      /* Check if we have now broken the limit and in the future no more
       * hints are available.. We check here the number of hints we have
       * given (by looking roughly at the number of 'normal' characters)
       * and seeing if that is greater-than or equal-to the percentage
       * allowed. If the percentage of hints has been breached, then we
       * will be wanting to enforce a blockage here.
       * 
       * FLOATING POINT WORK AROUND AHOY! notice the 'odd' location of the
       * 100 multiplier.
       */
      if (((normCharCount * 100) / hint.length()) >=
	  DEFAULT_QUIZ_HINT_BLOCK_PERCENTAGE) {
	 hintLevel = 0;
      }
   }
   
   // Fix up the 'new hint'
   hint = newHint;
   
   // Return the user friendly hint
   return output;
}
