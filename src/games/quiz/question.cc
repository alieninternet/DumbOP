/* src/games/quiz/class.cc
 * Quiz question class
 */

#include "gamequiz.h"

/* gameQuizQuestion - Initialise the question class
 * Original 4/6/01, Simon Butcher <simonb@alien.net.au>
 */
gameQuizQuestion::gameQuizQuestion(void)
: question(""), 
  clue(""), 
  source(""), 
  lastPlayed(0)
{
   answers.clear();
}

gameQuizQuestion::gameQuizQuestion(String q, String c, String s,
				   list<String> a)
: question(q), 
  clue(c),
  source(s),
  lastPlayed(0)
{
   // Wipe out answers list
   answers.clear();
   
   // Copy the answer pointers across
   //
   // THIS SHOULD BE STL LIST COPY()???
   //
   for (list<String>::iterator it = a.begin();
	it != a.end(); ++it) {
      answers.push_back(*it);
   }
}


/* ~gameQuizQuestion - Wipe out the question class
 * Original 4/6/01, Simon Butcher <simonb@alien.net.au>
 */
gameQuizQuestion::~gameQuizQuestion(void)
{
   answers.clear();
}


