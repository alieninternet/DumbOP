/* gamequiz.cc
 * 'Quiz' game, modelled on numerous quizbots that have been around for ages
 */

#include <string.h>
#include <dirent.h>
#include <fstream.h>
#include <errno.h>
#include <sys/types.h>

#include "gamequiz.h"
#include "stringtokenizer.h"

#define QUIZ_FILE_SUFFIX ".quiz"


/* gameQuizQuestion - Initialise the question class
 * Original 4/6/01, Simon Butcher <simonb@alien.net.au>
 */
gameQuizQuestion::gameQuizQuestion(void)
: question(""), hint("")
{
   answers.clear();
}

gameQuizQuestion::gameQuizQuestion(String q, String h,
				   list<String> a)
: question(q), hint(h), lastPlayed(0)
{
   // Wipe out answers list
   answers.clear();
   
   // Copy the answer pointers across
   //
   // THIS SHOULD BE STL LIST COPY()???
   //
   for (list<String>::iterator it = a.begin();
	it != a.end(); ++it) {
      answers.push_back(*it);
   }
}


/* ~gameQuizQuestion - Wipe out the question class
 * Original 4/6/01, Simon Butcher <simonb@alien.net.au>
 */
gameQuizQuestion::~gameQuizQuestion(void)
{
   answers.clear();
}


/* gameQuizCategory - Initialise the category class
 * Original 4/6/01, Simon Butcher <simonb@alien.net.au>
 */
gameQuizCategory::gameQuizCategory(void)
: lastPlayed(0)
{
   questions.clear();
}


/* ~gameQuizCategory - Kill the category class :)
 * Original 4/6/01, Simon Butcher <simonb@alien.net.au>
 */
gameQuizCategory::~gameQuizCategory(void)
{
   gameQuizQuestion *q;

   // Run through the questions and wipe them out
   while (questions.size() != 0) {
      q = (*questions.begin());
      questions.erase(questions.begin());
      delete q;
   }
}


/* gameQuizChannel - Initialise the quiz game run-time class
 * Original 12/7/01, SImon Butcher <simonb@alien.net.au>
 */
gameQuizChannel::gameQuizChannel(Channel *c, GameQuiz *gq)
: channel(c),
  gameQuiz(gq),
  category(""),
  nextCategory(""),
  nextCategoryCost(0),
  question(0),
  questionStr(""),
  hintLevel(-1),
  questionNum(0)
{
   // Fire up a category
   bumpCategory();
}


/* bumpCategory - Fire up the next category
 * Original 13/7/01, Simon Butcher <simonb@alien.net.au>
 */
void gameQuizChannel::bumpCategory(void) {
   // Have we been requested to change to a specific category?
   if (nextCategory.length()) {
      /* Copy the next category across. The category command checks this
       * so that we don't have to. Phew!
       */
      category = nextCategory;
   } else { // Otherwise we run through the list and find the next
      String potentialCategory = "";
      unsigned char randomAttempts = 0;
      
      while ((!gameQuiz->categories[potentialCategory]) ||
	     ((gameQuiz->games->bot->currentTime.time -
	       gameQuiz->categories[potentialCategory]->lastPlayed) <
	      DEFAULT_QUIZ_CATEGORY_LOCKOUT_TIME)) {
	 // Are we forcing a run-through to find the oldest category?
	 if (randomAttempts < DEFAULT_QUIZ_CATEGORY_RANDOM_ATTEMPTS) {
	    // Randomly select a category
	    
	    // This was a random attempt, increase the attempt counter
	    randomAttempts++;
	 } else {
	    /* No more random, to save time we will just run through the
	     * damned categories and pick whatever we find that has not been
	     * used in a long time.
	     */
	    for (map<String, gameQuizCategory *, less<String> >::iterator it =
		 gameQuiz->categories.begin();
		 it != gameQuiz->categories.end(); ++it) {
	       // Is this older than the one we already have marked down?
	       if ((!potentialCategory.length()) ||
		   ((*it).second->lastPlayed <
		    gameQuiz->categories[potentialCategory]->lastPlayed)) {
		  potentialCategory = (*it).first;
	       }
	    }
	    
	    /* End the while, just in case this category has not expired.
	     * For the quiz to keep going, we MUST break even if we might
	     * be breaking the expiration rules.
	     */
	    break;
	 }
      }
      
      category = potentialCategory;
   }
   
   // Reset the next category selection
   nextCategory = "";

   // Fix the last played variable for this category
   gameQuiz->categories[category]->lastPlayed = 
     gameQuiz->games->bot->currentTime.time;
   
   // Reset the cost for the next category
   nextCategoryCost = DEFAULT_QUIZ_CATEGORY_BASE_CHANGE_COST;
   
   // Reset the question number counter too
   questionNum = 0;
}


/* bumpQuestion - Fire up the next question
 * Original 14/7/01, Simon Butcher <simonb@alien.net.au>
 * Note: Should the 'find the oldest' method be replaced with a 'find the
 *       first' style loop?
 */
void gameQuizChannel::bumpQuestion(void) {
   gameQuizQuestion *potentialQuestion = 0;
   unsigned char randomAttempts = 0;
   
   while ((!potentialQuestion) ||
	  ((gameQuiz->games->bot->currentTime.time - 
	    potentialQuestion->lastPlayed) < 
	   DEFAULT_QUIZ_QUESTION_LOCKOUT_TIME)) {
      if (randomAttempts < DEFAULT_QUIZ_QUESTION_RANDOM_ATTEMPTS) {
	 // Randomly select a question
	 potentialQuestion = 
	   gameQuiz->categories[category]->questions[Utils::random(gameQuiz->categories[category]->numQuestions - 1)];
	 
	 // This was a random attempt, increase the attempt counter
	 randomAttempts++;
      } else {
	 /* No more random attempts, we gotta stop mucking around and just
	  * grab the first acceptable one we can find
	  */
	 for (vector<gameQuizQuestion *>::iterator it = 
	      gameQuiz->categories[category]->questions.begin();
	      it != gameQuiz->categories[category]->questions.end(); ++it) {
	    // Check if this is older than the one we have marked down
	    if ((!potentialQuestion) ||
		((*it)->lastPlayed < potentialQuestion->lastPlayed)) {
	       potentialQuestion = (*it);
	    }
	 }
	 
	 // Break the while loop to avoid any failure to comply to rules issues
	 break;
      }
   }
   
   // Copy the question pointer across.
   question = potentialQuestion;
   
   // Fix the last played variable for this category
   question->lastPlayed = gameQuiz->games->bot->currentTime.time;
   
   // Increase the question number counter
   questionNum++;
}


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
   map<String, gameQuizCategory *, less<String> > categories;

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


/* load - Seek and load the quiz question files
 * Original 3/6/01, Simon Butcher <simonb@alien.net.au>
 */
bool GameQuiz::load(void)
{
   struct dirent *dirEntry;
   DIR *directory = opendir(games->bot->quizDirectory);
   gameQuizQuestion *readQuestion = new gameQuizQuestion;
   unsigned int numCats = 0;
   
   // Clear a blank slate
   clear();
   
   // Make sure the directory was opened
   if (!directory) {
      String reason;
      
      switch (errno) {
       case EACCES: // Access denied
	 reason = "Access denied to language directory";
	 break;
       case EMFILE: // Too many files open by process
       case ENFILE: // Too many files open by system
	 reason = "Ran out of file descriptors while reading";
	 break;
       case ENOENT: // Directory does not exist
	 reason = "Could not find";
	 break;
       case ENOMEM:
	 reason = "Ran out of memory while reading";
	 break;
       case ENOTDIR:
	 reason = "File found instead of";
	 break;
       default:
	 reason = "Unknown error reading";
      };
      
      games->bot->logLine(String("QUIZ Game Failed: ") + reason);
      available = false; // Cannot load.
      return false;
   }
   
   // Ok, run through the directory list and look for what we want
   while ((dirEntry = readdir(directory)) != NULL) {
      /* Is this lazy programming doing a reverse to compare the last
       * bit? Is there a better way? Hrrm, probably both :)
       */
      if (!strncasecmp(Utils::reverseStr(dirEntry->d_name),
		       Utils::reverseStr(QUIZ_FILE_SUFFIX),
		       strlen(QUIZ_FILE_SUFFIX))) {
	 String fileName = String(dirEntry->d_name);
	 ifstream file(games->bot->quizDirectory +
		       String("/") + fileName);
		       
	 // Make sure we actually opened the file
	 if (!file) {
	    games->bot->logLine(String("Failed to open QUIZ file: ") +
				fileName);
	    continue;
	 }
	 
	 // Grab the category name (the name of the file before the .)
	 StringTokenizer st(fileName);
	 String category = st.nextToken('.').toUpper();
	 
	 // Make sure that worked..
	 if (category.length() < 1) {
	    games->bot->logLine(String("Could not get QUIZ category from: ") +
				fileName);
	    continue;
	 }
	 
	 // Get ready for reading
	 String line = "";
	 unsigned long numQuestions = 0, numLines = 0;
	 bool justStartedQuestion = false;
	 gameQuizCategory *readCategory = new gameQuizCategory;
	 readQuestion->question = "";
	 
	 // Run through the file
	 while (!file.eof()) {
	    // Read a line
	    file >> line;
	    numLines++;
	    
	    // Make sure the line is not an empty line
	    if (line.length() < 1) {
	       continue; // Do nothing if it is..
	    }
	    
	    // Check what type of line we are at
	    switch (line[0]) {
	     case '#': // Comment. Ignore
	       continue;
	       break; // this needed? not really..
	     case ':': // Question
	       /* Make sure prior question was valid so we don't make a mess
	        * of things
		*/
	       if (!justStartedQuestion) {
		  /* We are starting a new question.. but we MIGHT be JUST
		   * starting. Make sure we have a prior question loaded.
		   */
		  if (readQuestion->question.length() > 0) {
		     // Copy this question into the list for this category
		     readCategory->questions.push_back(new 
						       gameQuizQuestion(readQuestion->question,
									readQuestion->hint,
									readQuestion->answers));
		  }
		  
		  // Ok.. start it up again
		  justStartedQuestion = true;
	       }
	       
	       // Prep the variables
	       readQuestion->question = line.subString(1);
	       readQuestion->hint = "";
	       readQuestion->answers.clear();
	       
	       // Make sure length was OK
	       if (readQuestion->question.length() < 1) {
		  continue;
	       }
	       break;
	     case '$': // Hint
	       readQuestion->hint = line.subString(1);
	       break;
	     default: // Must be a possible answer
	       if (justStartedQuestion) { // make sure Q&A pair are ok
		  // This question must be ok.
		  justStartedQuestion = false;
		  numQuestions++;
		  
		  // Copy this question into the list
		  readQuestion->answers.push_back(line);
	       }
	       break;
	    }
	 }
	 
	 // Make sure we got at least 1 question there..
	 if (numQuestions > 0) {
	    // Make sure we are not forgetting the last question
	    if (!justStartedQuestion) {
	       // Copy this question into the list for this category
	       readCategory->questions.push_back(new 
						 gameQuizQuestion(readQuestion->question,
								  readQuestion->hint,
								  readQuestion->answers));
	    }
	 
	    // Finish up the category data
	    readCategory->numQuestions = numQuestions;
	    
	    // Copy this category into the main map
	    categories[category] = readCategory;
	    numCats++;
	    
	    // Tell the world what we did
	    games->bot->logLine(String("Loaded QUIZ category '") +
				category + String("' (") +
				String(numLines) + String(" lines, ") +
				String(numQuestions) + String(" questions)"));
	 } else {
	    // Bugger off that category then.. OH WELL
	    delete readCategory;
	    
	    // Tell the world we are not happy chappies
	    games->bot->logLine(String("Broken QUIZ file '") +
				fileName);
	 }
	 
	 // Close the file
	 file.close();
      }
    
   }

   // Close the directory file
   closedir(directory);

   // Copy the number of categories we loaded over
   numCategories = numCats;
   
   // Bitch and moan if we are not happy
   if (numCats < 1) {
      games->bot->logLine(String("Could not load any QUIZ files from directory ") +
			  games->bot->quizDirectory);
   }
   
   // The end.
   delete readQuestion;
   return (numCats < 1);
}

/* attend - Run the quiz channel, or channels
 * Original 13/7/01, Simon Butcher <simonb@alien.net.au>
 */
void GameQuiz::attend(void)
{
   for (map<String, gameQuizChannel *, less<String> >::iterator it =
	channels.begin(); it != channels.end(); ++it) {
      // Make sure it is not a broken pointer..
      if ((*it).second) {
	 time_t period = games->bot->currentTime.time - (*it).second->timeAsked;
	 
	 // Has the question expired without someone answering it?
	 if (((*it).second->questionStr.length()) &&
	     (period >= DEFAULT_QUIZ_QUESTION_ASK_TIME) &&
	     (!(*it).second->answered)) {
	    // Reset the question
	    (*it).second->questionStr = "";
	    
	    // Tell them nobody answered the question
	    (*it).second->channel->sendNotice("Nobody got that question.");
	 }
	 
	 // Are we in a question, and it is time for another hint?
	 if (((*it).second->questionStr.length()) &&
	     ((period % DEFAULT_QUIZ_QUESTION_NEXTHINT_DELAY) == 0) &&
	     ((*it).second->hintLevel > 0)) {
	    // Send another hint
	    (*it).second->channel->sendNotice("This is a hint placeholder.");
	 }
	 
	 // Have we finished with a question?
	 if (!(*it).second->questionStr.length()) {
	    // Next question time? or next category time?
	    if ((period >= (DEFAULT_QUIZ_QUESTION_ASK_TIME +
			    DEFAULT_QUIZ_QUESTION_BETWEEN_DELAY)) &&
		((*it).second->questionNum < DEFAULT_QUIZ_ROUND_QUESTIONS)) {
	       // Bump in the next question
	       (*it).second->bumpQuestion();
	       
	       // Normal question?
	       if (true) {
		  // Copy the question string across normally
		  (*it).second->questionStr = (*it).second->question->question;
		  
		  // Set the points correctly
	       } else {
		  // Bonus question.
		  
		  // Set the points correctly
		  
	       }
	       
	       // Send the question to the channel
	       (*it).second->channel->sendNotice(String("Question ") +
						 String((*it).second->questionNum) +
						 String("/") +
						 String(DEFAULT_QUIZ_ROUND_QUESTIONS) +
						 String(": ") +
						 (*it).second->questionStr);
	       
	       // Reset the time we asked the question variable
	       (*it).second->timeAsked = games->bot->currentTime.time;
	    } else if ((period >= (DEFAULT_QUIZ_QUESTION_ASK_TIME +
				   DEFAULT_QUIZ_CATEGORY_BETWEEN_DELAY)) &&
		       ((*it).second->questionNum >=
			DEFAULT_QUIZ_ROUND_QUESTIONS)) {
	       // Bump in the next category
	       (*it).second->bumpCategory();
	       
	       // Send the next category to the channel
	       (*it).second->channel->sendNotice(String("Category is now \002") +
						 (*it).second->category +
						 String("\002"));
	    }
	    
	    
	 }
      }
   }
}
