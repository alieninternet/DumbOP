/* src/games/quiz/player.cc
 * Quiz Channel Member (player) class
 * Copyright (c) 2001 Alien Internet Services
 */

#include "config.h"
#include "gamequiz.h"


/* gameQuizPlayer - Initialise the player class
 * Original 05/08/01, Simon Butcher <simonb@alien.net.au>
 */
gameQuizPlayer::gameQuizPlayer(void)
: 
  questionsRight(0),
  totalQuestionsRight(0),
  creditsWon(0),
  totalCreditsWon(0)
{
}


