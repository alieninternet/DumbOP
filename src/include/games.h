/* games.h
 * Built in games for the heck of it
 */

#ifndef __GAMES_H_
# define __GAMES_H_

class Games;
class GameQuiz;

#include "gamequiz.h"
#include "bot.h"

// Game flags
# define GAME_EIGHT_BALL	0x00000001 // Allow 8ball game
# define GAME_FORTUNE		0x00000002 // Allow fortune game
# define GAME_MISFORTUNE	0x00000004 // Allow mis-fortune game

class Games {
 private:
   Bot *bot;				// Recursive up...
   
   GameQuiz *quiz;			// The Quiz game
   
 public:
   Games(Bot *);			// Start up
   ~Games();				// Shutdown

   void attend(void);			// Main game loop called from main loop

   bool parseLine(Channel *, Person *, 
		  String);		// Parse a line of input
   void parseJoin(Channel *, Person *);	// Parse a join
   void parseLeave(Channel *, Person *);// Parse a leave (eg. part or kick)
   
   friend class GameQuiz;
   friend class gameQuizChannel;
   
   friend class Commands;
   friend class Channel;
};

#endif
