/* src/games/quiz/hints.cc
 * Quiz channel class - Hint generation/posting routines
 * Copyright (c) 2001 Alien Internet Services
 */

#include "config.h"

#include <ctype.h>

#include "gamequiz.h"


/* nextHint - Fire up the next hint, or create a new one if we have to..
 * Original 24/7/01, Simon Butcher <simonb@alien.net.au>
 * Note: Don't get it confused with CLUE which is a written line, a HINT is
 * 	 merely a hint to the answer, by using the answer.
 */
String gameQuizChannel::nextHint(char chr = '\0') {
   // If there is no point to this, we may as well bail out now..
   if ((hintLevel == 0) || (!question)) {
      return hint;
   }

   // Some handy dandy variables..
   String primaryAnswer = *question->answers.begin();
   String output = "";
   String newHint = "";
   
   // This is only useful if we are looking for a char..
   bool charFound = ((chr == '\0') ? true : false);
   
   // Are we creating a new hint?
   if (hintLevel == -1) {
      // Zero out the hint level..
      hintLevel++;
      
      /* First of all, make sure the primary answer is long enough for a
       * hint, otherwise the hint becomes too easy. We may as well bail out
       * now rather than piss fart around later.
       */
      if (primaryAnswer.length() < confHintMinLength) {
	 hint = "";
	 return "";
      }
      
      /* Run through the primary answer. We will create both the internal
       * hint string and the user friendly hint string output, while also
       * seeking for a char if it was provided.
       */
      for (int i = 0; i < primaryAnswer.length(); i++) {
	 // Copy through things that are not alphanumeric...
	 if (!isalnum(primaryAnswer[i])) {
	    output = output + String(primaryAnswer[i]);
	    newHint = newHint + String(primaryAnswer[i]);
	 } else { // Everything else we hide.
	    output = output + String(DEFAULT_QUIZ_HINT_SPECIAL_CHAR_OUTPUT);
	    newHint = newHint + String(DEFAULT_QUIZ_HINT_SPECIAL_CHAR);
	    hintLevel++; // Another level of hintness.. or something..
	 }
      }
   } else { // Otherwise we are modifying an original hint..
      bool gotChar = false;
      unsigned int normCharCount = 0;
      
      // Do not be fooled, this works differently to the creation loop!
      for (int i = 0; i < hint.length(); i++) {
	 switch (hint[i]) {
	  case DEFAULT_QUIZ_HINT_SPECIAL_CHAR: // Super special marker char?
	    // If we have gotten the char, we keep these hidden..
	    if (gotChar) {
	       output = output + String(DEFAULT_QUIZ_HINT_SPECIAL_CHAR_OUTPUT);
	       newHint = newHint + String(hint[i]);
	    } else {
	       // We are definately bumping in a char here.
	       hintLevel--;
	       gotChar = true;
	       normCharCount++;
	       
	       // Check how exactly we are bumping a char tho..
	       if ((!charFound) && 
		   (toupper(primaryAnswer[i]) == toupper(chr))) {
		  // If we are looking for a specific char, this is it!
		  output = output + String("\002") + String(primaryAnswer[i]) +
		    String("\002");
		  newHint = newHint + String(primaryAnswer[i]);
		  charFound = true;
	       } else { // This is the char, and that is that.
		  output = output + String(primaryAnswer[i]);
		  newHint = newHint + String(primaryAnswer[i]);
	       }
	    }
	    break;
	  default: // Everything else we let fall through
	    output = output + String(hint[i]);
	    newHint = newHint + String(hint[i]);
	    normCharCount++;
	 }
      }
      
      /* Check if we have now broken the limit and in the future no more
       * hints are available.. We check here the number of hints we have
       * given (by looking roughly at the number of 'normal' characters)
       * and seeing if that is greater-than or equal-to the percentage
       * allowed. If the percentage of hints has been breached, then we
       * will be wanting to enforce a blockage here.
       * 
       * FLOATING POINT WORK AROUND AHOY! notice the 'odd' location of the
       * 100 multiplier.
       */
      if (((normCharCount * 100) / hint.length()) >=
	  confHintBlockPercentage) {
	 hintLevel = 0;
      }
   }
   
   // Fix up the 'new hint'
   hint = newHint;
   
   // Return the user friendly hint
   return output;
}


/* sendHint - Send a nicely formatted nextHint() string to the channel
 * Original 06/08/01, Simon Butcher <simonb@alien.net.au>
 * Note: This is only here because it is needed in two places (!hint and here)
 */
void gameQuizChannel::sendHint()
{
   channel->sendNotice(confColourNormal +
		       String(" Question ") +
		       confColourHilight +
		       String("\002\002") +
		       String(questionNum) +
		       confColourNormal +
		       String("/") +
		       confColourHilight +
		       String("\002\002") +
		       String(confRoundQuestions) +
		       confColourNormal +
		       String(" Hint:") +
		       confColourText +
		       String(" ") +
		       nextHint() +
		       String(" \003"));
}
