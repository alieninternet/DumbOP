/* ./src/include/config.h
 * Defines created automatically for configuration script
 * Copyright (c) 2001 Alien Internet Services
 *
 * Note: 
 *   acconfig.h becomes config.h.in, which in turn becomes config.h
 * 
 * This means:
 *   Modify config.h for compile-time changes
 *   Modify acconfig.h for hacking changes
 *
 */

#ifndef __CONFIG_H_
# define __CONFIG_H_

/* WARNING!
 * 
 * These settings should NOT be changed unless you really know what you are
 * doing. They may conform to standards, or have limits you may or may not be
 * unaware of.
 * 
 * When changing settings in here, just to be safe make sure you do a 
 * 'make clean' then remake the package to avoid code discrepancies.
 */

/* Define if you want to compile an on-line debug ready version */
# define DEBUG

/* [GAMES] Define if you want a more free answering system */
# undef NO_ANSWER_NAZI

/* [CNX] Define if you are connecting servers with no multi-WHO suppport */
# undef ANAL_WHO_COMMAND

/* [CNX] Define if you REALLY do not like channel CTCP broadcast replies */
# undef NOT_VERY_HAPPY_ABOUT_CHANNEL_CTCP_BROADCASTS_WORKING_NO_SIREE_BOB

/* ANSI                                                                      */
// This is for word-wrap functionality
# define WORDWRAP_MIN_LEFT	20		// 20 chars left on the line

// Drawing chars
# define ANSI_DRAW_TOP_LH_CORNER	'.'	// Top left-hand corner
# define ANSI_DRAW_TOP_RH_CORNER	'.'	// Top right-hand corner
# define ANSI_DRAW_BOTTOM_LH_CORNER	'`'	// Bottom left-hand corner
# define ANSI_DRAW_BOTTOM_RH_CORNER	'\''	// Bottom right-hand corner
# define ANSI_DRAW_HORIZONTAL_LINE	'-'	// Horizontal line
# define ANSI_DRAW_VERTICLE_LINE	'|'	// Verticle line


/* CHANNEL                                                                   */
#define DEFAULT_KEEPMODES "iklmnpst"


/* GAMES -> QUIZ                                                             */
// Timing and sequencing values
#define DEFAULT_QUIZ_QUESTION_LOCKOUT_TIME	7200	// 2 hours
#define DEFAULT_QUIZ_CATEGORY_LOCKOUT_TIME	1800	// 30 mins
#define DEFAULT_QUIZ_QUESTION_ASK_TIME		120	// 2 mins
#define DEFAULT_QUIZ_QUESTION_BETWEEN_DELAY	12	// 12 secs
#define DEFAULT_QUIZ_CATEGORY_BETWEEN_DELAY	50	// 50 secs
#define DEFAULT_QUIZ_QUESTION_NEXTHINT_DELAY	25	// 25 secs
#define DEFAULT_QUIZ_ROUND_QUESTIONS		10	// 10 per round

// Scoring and cost values
#define DEFAULT_QUIZ_QUESTION_NORMAL_POINTS	1
#define DEFAULT_QUIZ_QUESTION_BONUS_POINTS	5
#define DEFAULT_QUIZ_CATEGORY_CHANGE_COST	5
#define DEFAULT_QUIZ_QUESTION_HINT_COST		3
#define DEFAULT_QUIZ_QUESTION_CLUE_COST		1

// These effect the randomness of "stuff"
#define DEFAULT_QUIZ_CATEGORY_RANDOM_ATTEMPTS	1 // no random cats yet
#define DEFAULT_QUIZ_QUESTION_RANDOM_ATTEMPTS	5
#define DEFAULT_QUIZ_BONUS_QUESTION_PERCENTILE	5 // 5%

// Auto-Hint controls
#define DEFAULT_QUIZ_HINT_MIN_LENGTH		0	// Length to auto-hint
#define DEFAULT_QUIZ_HINT_BLOCK_PERCENTAGE	40	// % hints before block

// These thingies effect the answering mechanism
#define DEFAULT_QUIZ_ANSWER_MAX_FAST_TIME	4000	// 4 sec


/* TELNET                                                                    */
# define TELNET_TELETYPE_MASK_CHAR	'*'	// Char to mask input with
# define TELNET_DEFAULT_BAR_MESSAGE	"Management Console" // Bar message
# define TELNET_ASSUMED_TTY_ROWS	24
# define TELNET_ASSUMED_TTY_COLUMNS	80
# define TELNET_LOGIN_MAX_ATTEMPTS	3	// Three attempts allowed


/* JOINING CHANNELS	                  	         	             */
# define CHANNEL_JOIN_MAX_CREDIT_FIND	5	// Maximum 'found' credits
# define CHANNEL_JOIN_FIND_PERCENTAGE	1	// 1 in 100 chance


/******************************************************************************
**                                                                           **
**                  WARNING!   WARNING!   WARNING!   WARNING!                **
**                                                                           **
**   Do not change anything below this line - These settings are generated   **
**   _AUTOMATICALLY_ by the configuration script and reflect the settings    **
**   required to allow this software to compile on your system. You should   **
**     only edit the below lines if you know what you are doing, and this    **
**        package is not compiling for you due to an incorrect define.       **
**                                                                           **
******************************************************************************/

# ifndef HAVE_CONFIG_H
#  error "You must run configure before compiling this software"  
# endif
@TOP@

/* Define it the C++ compiler has STL (Standard Template Library) support */
#undef HAVE_CXX_STL

/* Define if signal() has a third variable (struct sigcontext *) */
#undef HAVE_STRUCT_SIGCONTEXT

/* Define if the signal SIGCHLD is broken */
#undef SIGCHLD_BRAIN_DAMAGE

@BOTTOM@


#endif
