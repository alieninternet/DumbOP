/* gamequiz.h
 * 'Quiz' game, modelled on numerous quizbots that have been around for ages
 */

#ifndef __GAMEQUIZ_H_
# define __GAMEQUIZ_H_

#include <time.h>
#include <map.h>
#include <vector.h>
#include <list.h>

#include "strings.h"
#include "channel.h"

class Games;

#include "games.h"

// Auto-Hint things. Do not change unless you want to make a mess!!
#define DEFAULT_QUIZ_HINT_SPECIAL_CHAR		'\001'	// Internal mask char
#define DEFAULT_QUIZ_HINT_SPECIAL_CHAR_OUTPUT	'-'	// As seen by user

// Class for quiz questions
class gameQuizQuestion {
 public:
   String question;			// The question
   String clue;				// A written clue

   String source;			// Where the question came from
   
   list<String> answers;		// The answers
   
   time_t lastPlayed;			// Time question was last played
   
   gameQuizQuestion(void);		// Class constructors

   gameQuizQuestion(String, String, String, list<String>);
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
   enum {				// Question types
      NORMAL = -1,
      REVERSE = 0,
      NO_VOWELS
   };
   
   Channel *channel;			// Recursive back to original channel
   GameQuiz *gameQuiz;			// Recursive back to quiz game class

   // Category variables
   String category;			// Current category
   String nextCategory;			// Next category
   
   // Question variables
   gameQuizQuestion *question;		// Current question pointer
   String questionStr;			// Current question (may be warped)
   unsigned short questionNum;		// Question number (this round)
   char questionLevel;			// -1 = Normal, >= 0 are bonus-Q types
   struct timeb timeAsked;		// Time question was asked
   bool answered;			// Question is answered?
   
   // Hint system variables
   short hintLevel;			// -1 = Make new hint, 0 = no hint...
   String hint;				// Current hint string
   bool autoHint;			// Auto-hinting?
   
   gameQuizChannel(Channel *, 
		   GameQuiz *);		// Class constructor
   
   void bumpCategory(void);		// Fire up the next category
   void bumpQuestion(void);		// Fire up the next question
   
   String nextHint(char = '\0');	// Fire up the next hint
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
   void parseJoin(Channel *, Person *);	// Parse a new quiz player joining
   void parseLeave(Channel *, Person *);// Parse a quiz player leaving
   
 public:
   bool available;			// Is the quiz online?
   
   GameQuiz(Games *);			// Start up
   ~GameQuiz(void);			// Shut down
   
   void clear(void);			// Clear memory
   bool load(void);			// Load quiz data

   friend class gameQuizChannel;

   friend class Games;
   friend class Commands;
   friend class Channel;
};

#endif
