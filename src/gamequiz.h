/* gamequiz.h
 * 'Quiz' game, modelled on numerous quizbots that have been around for ages
 */

#ifndef __GAMEQUIZ_H_
# define __GAMEQUIZ_H_

#include <time.h>
#include <map.h>
#include <list.h>

#include "string.h"

class Games;

#include "games.h"

class gameQuizQuestion {
 public:
   String question;			// The question
   String hint;				// A written hint

   list<String> answers;		// The answers
   
   time_t lastPlayed;			// Time question was last played
   
   gameQuizQuestion(void);		// Class constructors
//   gameQuizQuestion(String, String);
   gameQuizQuestion(String, String, list<String>);
   ~gameQuizQuestion(void);		// Class destructor
};

class gameQuizCategory {
 public:
   list<gameQuizQuestion *> questions;	// Category questions
   unsigned long numQuestions;		// Number of questions here.
   time_t lastPlayed;			// Time category last played
   
   gameQuizCategory(void);		// Class constructor
   ~gameQuizCategory(void);		// Class destructor
};

class GameQuiz {
 private:
   Games *games;			// Pointer 'up'

   map<String, gameQuizCategory *, less<String> > gameQuizCategories;
   
 public:
   bool available;			// Is the quiz online?
   
   GameQuiz(Games *);			// Start up
   ~GameQuiz(void);			// Shut down
   
   void clear(void);			// Clear memory
   bool load(void);			// Load quiz data

   friend class Games;
   friend class UserCommands;
};

#endif
