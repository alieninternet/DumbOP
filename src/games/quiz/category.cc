/* src/games/quiz/category.cc
 * 'Quiz' game, modelled on numerous quizbots that have been around for ages
 */

#include "gamequiz.h"


/* gameQuizCategory - Initialise the category class
 * Original 4/6/01, Simon Butcher <simonb@alien.net.au>
 */
gameQuizCategory::gameQuizCategory(void)
: lastPlayed(0)
{
   questions.clear();
}


/* ~gameQuizCategory - Kill the category class :)
 * Original 4/6/01, Simon Butcher <simonb@alien.net.au>
 */
gameQuizCategory::~gameQuizCategory(void)
{
   gameQuizQuestion *q;

   // Run through the questions and wipe them out
   while (questions.size() != 0) {
      q = (*questions.begin());
      questions.erase(questions.begin());
      delete q;
   }
}


