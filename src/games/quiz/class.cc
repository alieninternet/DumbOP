/* src/games/quiz/class.cc
 * 'Quiz' game, modelled on numerous quizbots that have been around for ages
 * Copyright (c) 2001 Alien Internet Services
 */

#include "config.h"

#include "gamequiz.h"
#include "utils.h"


/* GameQuiz - Initialise the quiz game
 * Original 3/6/01, Simon Butcher <simonb@alien.net.au>
 */
GameQuiz::GameQuiz(Games *parent)
: games(parent),
  available(false)
{
   categories.clear();
   channels.clear();
   
   load();
}


/* ~GameQuiz - Shutdown the quiz game
 * Original 3/6/01, Simon Butcher <simonb@alien.net.au>
 */
GameQuiz::~GameQuiz()
{
   // Wipe our the data
   clear();
}


/* clear - Totally wipe out the quiz data in memory
 * Original 3/6/01, Simon Butcher <simonb@alien.net.au>
 */
void GameQuiz::clear(void)
{
   gameQuizCategory *cat;
   gameQuizChannel *chan;
   map<String, gameQuizCategory *, less<String> >::iterator itCat;
   map<String, gameQuizChannel *, less<String> >::iterator itChan;
   
   // Run through the categories in memory and wipe them out
   while (categories.size() != 0) {
      itCat = categories.begin();
      cat = (*itCat).second;
      categories.erase(itCat);
      delete cat;
   }
   
   while (channels.size() != 0) {
      itChan = channels.begin();
      chan = (*itChan).second;
      channels.erase(itChan);
      delete chan;
   }
}


