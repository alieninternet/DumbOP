/* src/games/quiz/class.cc
 * 'Quiz' game, modelled on numerous quizbots that have been around for ages
 */

#include <string.h>
#include <dirent.h>
#include <fstream.h>
#include <errno.h>
#include <sys/types.h>
#include <ctype.h>

#include "gamequiz.h"

#define QUIZ_FILE_SUFFIX ".quiz"


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
	 StringTokens st(fileName);
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
	    
	    // Fix the line up before we go ahead and do stuff with it
	    line = Utils::dwindleSpaces(Utils::stripCRLF(line));
	    
	    // Make sure the line is not an empty line
	    if (line.length() < 1) {
	       continue; // Do nothing if it is..
	    }
	    
	    // Check what type of line we are at
	    switch (line[0]) {
	     case '#': // Comment. Ignore
	     case '!': // Reserved chars for future use are ignored
	     case '`':
	     case '%':
	     case '^':
	     case '&':
	     case '*':
	     case '/':
	     case '|':
	     case '\\':
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
									readQuestion->clue,
									readQuestion->source,
									readQuestion->answers));
		  }
		  
		  // Ok.. start it up again
		  justStartedQuestion = true;
	       }
	       
	       // Prep the variables
	       readQuestion->question = line.subString(1);
	       readQuestion->clue = readQuestion->source = "";
	       readQuestion->answers.clear();
	       
	       // Make sure length was OK
	       if (readQuestion->question.length() < 1) {
		  continue;
	       }
	       break;
	     case '$': // Clue
	       readQuestion->clue = line.subString(1);
	       break;
	     case '@': // Source
	       readQuestion->source = line.subString(1);
	       break;
	     default: // Must be a possible answer
	       if (justStartedQuestion) { // make sure Q&A pair are ok
		  // This question must be ok.
		  justStartedQuestion = false;
		  
		  // Increase the question number counter
		  numQuestions++;
	       }

	       // Copy this question into the list
	       readQuestion->answers.push_back(line);
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
								  readQuestion->clue,
								  readQuestion->source,
								  readQuestion->answers));
	    }
	 
	    // Finish up the category data
	    readCategory->numQuestions = numQuestions;

	    /* This is to create a 'random offset' when the quiz starts up
	     * to counteract repartition
	     */
	    readCategory->lastPlayed = Utils::random(10000);
	    
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

