/* src/include/config.h.  Generated automatically by configure.  */
/* src/include/config.h.in.  Generated automatically from configure.in by autoheader.  */
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
/* # undef NO_ANSWER_NAZI */

/* [CNX] Define if you are connecting servers with no multi-WHO suppport */
/* # undef ANAL_WHO_COMMAND */

/* [CNX] Define if you REALLY do not like channel CTCP broadcast replies */
/* # undef NOT_VERY_HAPPY_ABOUT_CHANNEL_CTCP_BROADCASTS_WORKING_NO_SIREE_BOB */

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


/* BOT                                                                       */
#define DEFAULT_NICKNAME "DumbOP"
#define DEFAULT_USERNAME "dumbop"
#define DEFAULT_IRCNAME "DumbOP"
#define DEFAULT_COMMANDCHAR '!'
#define DEFAULT_USERLISTFILENAME "bot.users"
#define DEFAULT_CHANNELLISTFILENAME "bot.channels"
#define DEFAULT_NOTELISTFILENAME "bot.notes"
#define DEFAULT_HELPFILENAME "bot.help"
#define DEFAULT_SCRIPTLOGFILENAME "script.log"
#define DEFAULT_LOGFILENAME "bot.log"
#define DEFAULT_INITFILENAME "bot.init"
#define DEFAULT_QUIZDIRECTORY "quiz"


/* CHANNEL                                                                   */
#define DEFAULT_KEEPMODES "iklmnpst"


/* GAMES -> QUIZ                                                             */
// Timing and sequencing values
#define DEFAULT_QUIZ_QUESTION_LOCKOUT_TIME	7200	// 2 hours
#define DEFAULT_QUIZ_CATEGORY_LOCKOUT_TIME	1800	// 30 mins
#define DEFAULT_QUIZ_QUESTION_ASK_TIME		80	// 1 min 20 secs
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

/* Define if type char is unsigned and you are not using gcc.  */
#ifndef __CHAR_UNSIGNED__
/* #undef __CHAR_UNSIGNED__ */
#endif

/* Define to empty if the keyword does not work.  */
/* #undef const */

/* Define if the `long double' type works.  */
#define HAVE_LONG_DOUBLE 1

/* Define if you have <sys/wait.h> that is POSIX.1 compatible.  */
#define HAVE_SYS_WAIT_H 1

/* Define if your struct tm has tm_zone.  */
#define HAVE_TM_ZONE 1

/* Define if you don't have tm_zone but do have the external array
   tzname.  */
/* #undef HAVE_TZNAME */

/* Define as __inline if that's what the C compiler calls it.  */
/* #undef inline */

/* Define to `int' if <sys/types.h> doesn't define.  */
/* #undef mode_t */

/* Define to `int' if <sys/types.h> doesn't define.  */
/* #undef pid_t */

/* Define as the return type of signal handlers (int or void).  */
#define RETSIGTYPE void

/* Define to `unsigned' if <sys/types.h> doesn't define.  */
/* #undef size_t */

/* Define if the `S_IS*' macros in <sys/stat.h> do not work properly.  */
/* #undef STAT_MACROS_BROKEN */

/* Define if you have the ANSI C header files.  */
#define STDC_HEADERS 1

/* Define if `sys_siglist' is declared by <signal.h>.  */
#define SYS_SIGLIST_DECLARED 1

/* Define if you can safely include both <sys/time.h> and <time.h>.  */
#define TIME_WITH_SYS_TIME 1

/* Define if your <sys/time.h> declares struct tm.  */
/* #undef TM_IN_SYS_TIME */

/* Define if your processor stores words with the most significant
   byte first (like Motorola and SPARC, unlike Intel and VAX).  */
/* #undef WORDS_BIGENDIAN */

/* Define it the C++ compiler has STL (Standard Template Library) support */
#define HAVE_CXX_STL 1

/* Define if signal() has a third variable (struct sigcontext *) */
#define HAVE_STRUCT_SIGCONTEXT 1

/* Define if the signal SIGCHLD is broken */
/* #undef SIGCHLD_BRAIN_DAMAGE */

/* Define if you have the ftime function.  */
#define HAVE_FTIME 1

/* Define if you have the select function.  */
#define HAVE_SELECT 1

/* Define if you have the socket function.  */
#define HAVE_SOCKET 1

/* Define if you have the <dirent.h> header file.  */
#define HAVE_DIRENT_H 1

/* Define if you have the <fcntl.h> header file.  */
#define HAVE_FCNTL_H 1

/* Define if you have the <limits.h> header file.  */
#define HAVE_LIMITS_H 1

/* Define if you have the <ndir.h> header file.  */
/* #undef HAVE_NDIR_H */

/* Define if you have the <sys/dir.h> header file.  */
/* #undef HAVE_SYS_DIR_H */

/* Define if you have the <sys/ndir.h> header file.  */
/* #undef HAVE_SYS_NDIR_H */

/* Define if you have the <sys/time.h> header file.  */
#define HAVE_SYS_TIME_H 1

/* Define if you have the <unistd.h> header file.  */
#define HAVE_UNISTD_H 1


#endif
