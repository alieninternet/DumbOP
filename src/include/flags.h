/* flags.h
 * Resource flagging system
 */

#ifndef __FLAGS_H_
# define __FLAGS_H_

/* DO NOT CHANGE THESE FLAGS ONCE THEY ARE SET OR I WILL HAVE TO KILL YOU 
 * or something.
 */

// User flags
# define USERFLAG_IS_BOT		0x00000001 // User is actually a bot
# define USERFLAG_IS_SUSPENDED		0x00000002 // User is suspended completely
# define USERFLAG_IS_ON_HOLIDAY		0x00000004 // User is on holiday
# define USERFLAG_USE_PRIVMSG_ON_PRIV	0x00000010 // Use PRIVMSG for priv cmds
# define USERFLAG_USE_PRIVMSG_ON_CHAN	0x00000020 // Use PRIVMSG for chan cmds
# define USERFLAG_EMAIL_NOTES		0x00000040 // Forward NOTES to EMAIL
# define USERFLAG_HAS_MONEY_TREE	0x00100000 // User has endless credits
# define USERFLAG_LASTSEEN_AUTH		0x10000000 // Lastseen was identified

// Channel flags
# define CHANFLAG_PROTECT_NOFLOOD	0x00000001 // Protect from flooding
# define CHANFLAG_PROTECT_PROFANITY	0x00000002 // Protect from swearing
# define CHANFLAG_PROTECT_CODES		0x00000004 // Protect from format codes
# define CHANFLAG_ALLOW_GAMES		0x00000010 // Allow built in games
# define CHANFLAG_ALLOW_TALKER		0x00000020 // Allow built in talker
# define CHANFLAG_JOIN_WELCOME		0x00000040 // Welcome users when joining
# define CHANFLAG_LOCK_MODES		0x00000080 // Lock channel modes
# define CHANFLAG_ALLOW_FIND_CREDITS	0x00000100 // Can find credits on join
# define CHANFLAG_TALK_TO_NONREGISTERED	0x00000200 // Tell others to REGISTER
# define CHANFLAG_DISABLE_COMMANDS	0x00000400 // Disable commands on channel
# define CHANFLAG_SUSPENDED		0x00100000 // Channel is suspended
# define CHANFLAG_JOINED		0x10000000 // Channel Joined
# define CHANFLAG_DOMODE		0x20000000 // Do mode change
# define CHANFLAG_GOTWHO		0x40000000 // Got initial WHO list

// Game flags
# define GAMEFLAG_EIGHT_BALL		0x00000001 // Allow 8ball game
# define GAMEFLAG_FORTUNE		0x00000002 // Allow fortune game
# define GAMEFLAG_MISFORTUNE		0x00000004 // Allow mis-fortune game
# define GAMEFLAG_QUIZ			0x00000008 // Allow quiz game

// Flag list structure
struct flagLookupStruct {
   const char *flagCommand;	// Flag command for setting (flags.cc)
   unsigned long flagValue;	// The bit the flag refers to
   const char flagChar;		// Flag character for quick viewing
   const char *flagName;	// Flag name for verbosity
   short minLevel;		// Minimum level required to set the flag
};

#endif
