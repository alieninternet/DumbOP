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


/* parseLine - Parse an incoming line of text on a channel that we might want
 * Original 16/7/01, Simon Butcher <simonb@alien.net.au>
 */
bool Games::parseLine(Channel *channel, Person *from, String line)
{
   // Check if this is for the quiz channel
   if (channel->cnx->bot->wantedChannels[channel->channelName]->gameflags &
       GAMEFLAG_QUIZ) {
      quiz->parseLine(channel, from, line);
      return true;
   }
       
   
   
   // If we got here, we did not use the line
   return false;
}
