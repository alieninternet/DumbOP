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
 
/* GameQuiz - Initialise the quiz game
 * Original 3/6/01, Simon Butcher <simonb@alien.net.au>
 */
GameQuiz::GameQuiz(Games *parent)
: games(parent),
  available(false)
{
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
   map<String, gameQuizCategory *, less<String> > gameQuizCategories;

   gameQuizCategory *c;
   map<String, gameQuizCategory *, less<String> >::iterator it;
   
   // Run through the categories in memory and wipe them out
   while (gameQuizCategories.size() != 0) {
      it = gameQuizCategories.begin();
      c = (*it).second;
      gameQuizCategories.erase(it);
      delete c;
   }
}

/* load - Seek and load the quiz question files
 * Original 3/6/01, Simon Butcher <simonb@alien.net.au>
 */
bool GameQuiz::load(void)
{
   struct dirent *dirEntry;
   DIR *directory = opendir(games->bot->quizDirectory);
   bool happy = false; // Did we work happily? eg. At least 1 question
   gameQuizQuestion *readQuestion = new gameQuizQuestion;
   
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
		  happy = false;
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
	       happy = true; // We are happy, we have at least 1 Q&A
	       break;
	    }
	    
	    
	    // uhh.. dirty.
	    happy = true;
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
	    gameQuizCategories[category] = readCategory;
	    
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

   // Bitch and moan if we are not happy
   if (!happy) {
      games->bot->logLine(String("Could not load any QUIZ files from directory ") +
			  games->bot->quizDirectory);
   }
   
   // The end.
   delete readQuestion;
   return happy;
}
