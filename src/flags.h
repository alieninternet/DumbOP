/* flags.h
 * Resource flagging system
 */

#ifndef __FLAGS_H_
# define __FLAGS_H_

// Flag types
# define TYPE_PERSON	0x00	// Flag is for a PERSON
# define TYPE_USER	0x01	// Flag is for a CHANNEL USER
# define TYPE_CHANNEL	0x02	// Flag is for a CHANNEL

// General flag stuff
# define FLAG_NONE			0x00000000 // Use flag =
# define FLAG_CLEANUP			0xFF000000 // Use flag &=

// Personal flags
# define PERSONFLAG_IS_BOT		0x00000001 // Person is actually a bot
# define PERSONFLAG_USE_PRIVMSG		0x00000010 // Use PRIVMSG, not NOTICE
# define PERSONFLAG_SUSPENDED		0x00100000 // Person is suspended completely
# define PERSONFLAG_IDENTIFIED		0x10000000 // Person is identified

// User flags
# define USERFLAG_JOIN_AOP		0x00000001 // Auto +o on join
# define USERFLAG_JOIN_AOV		0x00000002 // Auto +v on join
# define USERFLAG_PROTECT_NOBAN		0x00000010 // Protect from baning
# define USERFLAG_PROTECT_NOKICK	0x00000020 // Protect from kicking
# define USERFLAG_PROTECT_NODEOP	0x00000040 // Protect from MODE -o
# define USERFLAG_PROTECT_NODEVOICE	0x00000080 // Protect from MODE -v
# define USERFLAG_IS_CHAN_OWNER		0x00000100 // Consider user as channel owner
# define USERFLAG_IS_CHAN_MAINT		0x00000200 // Can maintain channel
# define USERFLAG_CAN_OP		0x00001000 // Can +o self
# define USERFLAG_CAN_VOICE		0x00002000 // Can +v self
# define USERFLAG_CAN_KICK		0x00004000 // Allowed to kick
# define USERFLAG_CAN_BAN		0x00008000 // Allowed to ban
# define USERFLAG_LASTSEEN_AUTH		0x00100000 // Lastseen was identified
# define USERFLAG_SUSPENDED		0x00200000 // User is suspended on channel

// Channel flags
# define CHANFLAG_PROTECT_NOFLOOD	0x00000001 // Protect from flooding
# define CHANFLAG_PROTECT_PROFANITY	0x00000002 // Protect from swearing
# define CHANFLAG_PROTECT_CODES		0x00000004 // Protect from format codes
# define CHANFLAG_ALLOW_GAMES		0x00000010 // Allow built in games
# define CHANFLAG_ALLOW_TALKER		0x00000020 // Allow built in talker
# define CHANFLAG_JOIN_WELCOME		0x00000040 // Welcome users when joining
# define CHANFLAG_LOCK_MODES		0x00000080 // Lock channel modes
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
   const char *flagCommand;	// Flag command for setting (flags.c)
   long flagValue;		// The bit the flag refers to
   bool bySelf;			// Is the flag a self infliction only?
   short flagType;		// What type of flag this flag is
   const char flagChar;		// Flag character for quick viewing
   const char *flagName;	// Flag name for verbosity
   short minLevel;		// Minimum level required to set the flag
   long reqUserFlags;		// User Flags required for this operation
   long reqPersonFlags;		// Personal Flags required for this operation
};

#endif
