/* src/games/parser.c
 * Built in game channel data parsing routines
 */

#include "games.h"
#include "gamequiz.h"
#include "flags.h"

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


/* parseJoin - Parse someone joining the channel/game
 * Original 27/07/01, Simon Butcher <simonb@alien.net.au>
 */
void Games::parseJoin(Channel *channel, Person *from)
{
   // Check if this is for the quiz channel
   if (channel->cnx->bot->wantedChannels[channel->channelName]->gameflags &
       GAMEFLAG_QUIZ) {
      quiz->parseJoin(channel, from);
   }
}


/* parseLeave - Parse someone leaving the channel/game (eg. part or kick)
 * Original 27/07/01, Simon Butcher <simonb@alien.net.au>
 */
void Games::parseLeave(Channel *channel, Person *from)
{
   // Check if this is for the quiz channel
   if (channel->cnx->bot->wantedChannels[channel->channelName]->gameflags &
       GAMEFLAG_QUIZ) {
      quiz->parseLeave(channel, from);
   }
}
