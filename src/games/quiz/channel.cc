/* src/games/quiz/channel.cc
 * Quiz channel class
 * Copyright (c) 2001 Alien Internet Services
 */

#include "config.h"

#include <ctype.h>
#include <stdlib.h>

#include "gamequiz.h"
#include "utils.h"


/* gameQuizChannel - Initialise the quiz game run-time class
 * Original 12/7/01, SImon Butcher <simonb@alien.net.au>
 */
gameQuizChannel::gameQuizChannel(Channel *c, GameQuiz *gq)
: channel(c),
  gameQuiz(gq),
  category(""),
  nextCategory(""),
  question(0),
  questionStr(""),
  questionNum(0),
  questionLevel(NORMAL),
  timeAsked(gameQuiz->games->bot->currentTime),
  answered(false),
  hintLevel(-1),
  hint(""),
  autoHint(false),
  confQuestionLockoutTime(DEFAULT_QUIZ_QUESTION_LOCKOUT_TIME),
  confCategoryLockoutTime(DEFAULT_QUIZ_CATEGORY_LOCKOUT_TIME),
  confQuestionAskTime(DEFAULT_QUIZ_QUESTION_ASK_TIME),
  confQuestionBetweenDelay(DEFAULT_QUIZ_QUESTION_BETWEEN_DELAY),
  confCategoryBetweenDelay(DEFAULT_QUIZ_CATEGORY_BETWEEN_DELAY),
  confQuestionNexthintDelay(DEFAULT_QUIZ_QUESTION_NEXTHINT_DELAY),
  confRoundQuestions(DEFAULT_QUIZ_ROUND_QUESTIONS),
  confQuestionNormalPoints(DEFAULT_QUIZ_QUESTION_NORMAL_POINTS),
  confQuestionBonusPoints(DEFAULT_QUIZ_QUESTION_BONUS_POINTS),
  confCategoryChangeCost(DEFAULT_QUIZ_CATEGORY_CHANGE_COST),
  confQuestionHintCost(DEFAULT_QUIZ_QUESTION_HINT_COST),
  confQuestionClueCost(DEFAULT_QUIZ_QUESTION_CLUE_COST),
  confBonusQuestionPercentile(DEFAULT_QUIZ_BONUS_QUESTION_PERCENTILE),
  confHintMinLength(DEFAULT_QUIZ_HINT_MIN_LENGTH),
  confHintBlockPercentage(DEFAULT_QUIZ_HINT_BLOCK_PERCENTAGE),
  confAnswerMaxFastTime(DEFAULT_QUIZ_ANSWER_MAX_FAST_TIME),
  confColourNormal("\0038,7"),
  confColourHilight("\0030,7"),
  confColourText("\0031,7")
{
   // Initialise the players list...
   players.clear();
   // oops.. should check the current players in this channel here!!
   
   // Fire up a category
   bumpCategory();
   
   // Tell the channel we we just did...!
   channel->sendNotice(String("Starting category will be \002") + category + 
		       String("\002."));
}


/* ~gameQuizChannel - Channel destructor
 * Original 05/08/01, Simon Butcher <simonb@alien.net.au>
 */
gameQuizChannel::~gameQuizChannel(void) 
{
   // Wipe out the players list
   gameQuizPlayer *player;
   map<String, gameQuizPlayer *, less<String> >::iterator itPlayer;
   while (players.size() != 0) {
      itPlayer = players.begin();
      player = (*itPlayer).second;
      players.erase(itPlayer);
      delete player;
   }
}


