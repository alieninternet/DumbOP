/* src/games/interface.c
 * Built in games for the heck of it
 */

#include "games.h"
#include "gamequiz.h"
#include "flags.h"

/* Games - Initialise the internal games
 * Original 3/6/01, Simon Butcher <simonb@alien.net.au>
 */
Games::Games(Bot *b)
: bot(b),
  quiz(new GameQuiz(this))
{
}

/* ~Games - Shutdown internal games
 * Original 3/6/01, Simon Butcher <simonb@alien.net.au>
 */
Games::~Games()
{
   delete quiz;
}

/* attend - Main loop for games that require constant attention
 * Original 8/6/01, Simon Butcher <simonb@alien.net.au>
 * Notes: We do not loop here, we get called by the bot's main select() loop.
 */
void Games::attend(void)
{
   // If we are able to run a quiz, do so
   if (quiz) {
      quiz->attend();
   }
}


