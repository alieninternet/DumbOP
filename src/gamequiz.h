/* gamequiz.h
 * 'Quiz' game, modelled on numerous quizbots that have been around for ages
 */

#ifndef __GAMEQUIZ_H_
# define __GAMEQUIZ_H_

#include <time.h>
#include <map.h>
#include <vector.h>
#include <list.h>

#include "string.h"
#include "channel.h"

class Games;

#include "games.h"

// Timing and sequencing values
#define DEFAULT_QUIZ_CATEGORY_LOCKOUT_TIME	3600	// 1 hour
#define DEFAULT_QUIZ_QUESTION_LOCKOUT_TIME	3600	// 1 hour
#define DEFAULT_QUIZ_QUESTION_ASK_TIME		60	// 1 min
#define DEFAULT_QUIZ_QUESTION_BETWEEN_DELAY	15	// 15 secs
#define DEFAULT_QUIZ_CATEGORY_BETWEEN_DELAY	45	// 45 secs
#define DEFAULT_QUIZ_QUESTION_NEXTHINT_DELAY	20	// 20 secs
#define DEFAULT_QUIZ_ROUND_QUESTIONS		10	// 10 per round

// Scoring and cost values
#define DEFAULT_QUIZ_QUESTION_NORMAL_POINTS	1
#define DEFAULT_QUIZ_QUESTION_BONUS_POINTS	3
#define DEFAULT_QUIZ_CATEGORY_CHANGE_COST	5

// These effect the randomness of "stuff"
#define DEFAULT_QUIZ_CATEGORY_RANDOM_ATTEMPTS	1 // no random cats yet
#define DEFAULT_QUIZ_QUESTION_RANDOM_ATTEMPTS	5
#define DEFAULT_QUIZ_BONUS_QUESTION_PERCENTILE	5 // 5%


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
   vector<gameQuizQuestion *> questions; // Category questions
   unsigned long numQuestions;		// Number of questions here.
   time_t lastPlayed;			// Time category last played
   
   gameQuizCategory(void);		// Class constructor
   ~gameQuizCategory(void);		// Class destructor
};


// Class for quiz channel in-game run-time information
class gameQuizChannel {
 public:
   Channel *channel;			// Recursive back to original channel
   GameQuiz *gameQuiz;			// Recursive back to quiz game class
   
   String category;			// Current category
   String nextCategory;			// Next category
   
   gameQuizQuestion *question;		// Current question pointer
   String questionStr;			// Current question (may be warped)
   time_t timeAsked;			// Time question was asked
   
   short hintLevel;			// -1 = Make new hint, 0 = no hint...
   unsigned short questionNum;		// Question number (this round)
   bool answered;			// Question is answered?
   
   gameQuizChannel(Channel *, 
		   GameQuiz *);		// Class constructor
   
   void bumpCategory(void);		// Fire up the next category
   void bumpQuestion(void);		// Fire up the next question
   
};


// Main quiz game class
class GameQuiz {
 private:
   Games *games;			// Pointer 'up'

   map<String, gameQuizCategory *, less<String> > categories;
   unsigned int numCategories;		// Number of categories
   
   map<String, gameQuizChannel *, less<String> > channels;

   void attend(void);			// Run the quiz!
   
   void parseLine(Channel *, Person *, 
		  String);		// Parse a line of input
   
 public:
   bool available;			// Is the quiz online?
   
   GameQuiz(Games *);			// Start up
   ~GameQuiz(void);			// Shut down
   
   void clear(void);			// Clear memory
   bool load(void);			// Load quiz data

   friend class gameQuizChannel;

   friend class Games;
   friend class UserCommands;
   friend class Channel;
};

#endif
