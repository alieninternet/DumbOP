#ifndef __FLAGS_H_
# define __FLAGS_H_

/* 
 * The Flags
 */

# define USERFLAG_BLANK			0x00000000 // No flags set
# define USERFLAG_JOIN_AOP		0x00000001 // Auto +o on join
# define USERFLAG_JOIN_AOV		0x00000002 // Auto +v on join
# define USERFLAG_JOIN_GREET		0x00000004 // Notify user when discovered :)
# define USERFLAG_PROTECT_NOBAN		0x00000010 // Protect from baning
# define USERFLAG_PROTECT_NOKICK	0x00000020 // Protect from kicking
# define USERFLAG_PROTECT_NODEOP	0x00000040 // Protect from MODE -o
# define USERFLAG_PROTECT_NODEVOICE	0x00000080 // Protect from MODE -v
# define USERFLAG_IS_BOT		0x00000010 // User is actually a bot
# define USERFLAG_IS_CHAN_OWNER		0x00000020 // Consider user as channel owner
# define USERFLAG_USE_PRIVMSG		0x00000100 // Use PRIVMSG instead of NOTICE
# define USERFLAG_LASTSEEN_AUTH		0x01000000 // Lastseen was password identified
# define USERFLAG_SUSPENDED		0x02000000 // User is suspended
# define USERFLAG_IDENTIFIED		0x10000000 // User has identified themselves

# define CHANFLAG_PROTECT_NOFLOOD	0x00000001 // Protect from flooding
# define CHANFLAG_PROTECT_PROFANITY	0x00000002 // Protect from swearing
# define CHANFLAG_PROTECT_CODES		0x00000004 // Protect from format codes
# define CHANFLAG_ALLOW_GAMES		0x00000010 // Allow built in games
# define CHANFLAG_ALLOW_TALKER		0x00000020 // Allow built in talker
# define CHANFLAG_ALLOW_GUESTS		0x00000040 // Allow users not in userbase
# define CHANFLAG_JOINED		0x10000000 // Channel Joined
# define CHANFLAG_DOMODE		0x20000000 // Do mode change
# define CHANFLAG_GOTWHO		0x40000000 // Got initial WHO list

/*
 * Flag Names / Codes
 */

#endif
