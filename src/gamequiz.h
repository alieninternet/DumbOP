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

#define DEFAULT_QUIZ_CATEGORY_LOCKOUT	1800	// 30 mins
#define DEFAULT_QUIZ_QUESTION_LOCKOUT	3600	// 1 hour

// Class for quiz questions
class gameQuizQuestion {
 public:
   String question;			// The question
   String hint;				// A written hint

   list<String> answers;		// The answers
   
   time_t lastPlayed;			// Time question was last played
   
   gameQuizQuestion(void);		// Class constructors

   gameQuizQuestion(String, String, list<String>);
   ~gameQuizQuestion(void);		// Class destructor
};


// Class for quiz categories
class gameQuizCategory {
 public:
   list<gameQuizQuestion *> questions;	// Category questions
   unsigned long numQuestions;		// Number of questions here.
   time_t lastPlayed;			// Time category last played
   
   gameQuizCategory(void);		// Class constructor
   ~gameQuizCategory(void);		// Class destructor
};


// Class for quiz channel in-game run-time information
class gameQuizChannel {
 public:
   String category;			// Current category
   String nextCategory;			// Next category
   
   gameQuizQuestion *question;		// Current question pointer
   bool asked;				// Are we awaiting an answer?

   int hintLevel;			// -1 = Make new hint, 0 = no hint...
   
   
};


// Main quiz game class
class GameQuiz {
 private:
   Games *games;			// Pointer 'up'

   map<String, gameQuizCategory *, less<String> > gameQuizCategories;

   map<String, gameQuizChannel *, less<String> > gameQuizChannels;
   
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
