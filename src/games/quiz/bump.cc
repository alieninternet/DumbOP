/* src/games/quiz/bump.cc
 * Quiz channel class - bump in/out routines for questions and categories
 * Copyright (c) 2001 Alien Internet Services
 */

#include "config.h"

#include "gamequiz.h"
#include "utils.h"


/* bumpCategory - Fire up the next category
 * Original 13/07/01, Simon Butcher <simonb@alien.net.au>
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
	      confCategoryLockoutTime)) {
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
	    potentialQuestion->lastPlayed) < confQuestionLockoutTime)) {
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


