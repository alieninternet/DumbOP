/* flags.cc
 * Resource flagging system
 */

#include "flags.h"
#include "user.h"

// flagLookup - Lookup table of all flags for the database
struct flagLookupStruct flagLookup[] = {
     { "AOP",		USERFLAG_JOIN_AOP,		false,	TYPE_USER,
	  '@',	"Auto-Op on join",			User::USER,
	  USERFLAG_IS_CHAN_MAINT,
	  FLAG_NONE
     },
     { "AOV",		USERFLAG_JOIN_AOV,		false,	TYPE_USER,
	  '+',	"Auto-Voice on join",			User::USER,
	  USERFLAG_IS_CHAN_MAINT,	
	  FLAG_NONE
     },
     { "CANBAN",	USERFLAG_CAN_BAN,		false,	TYPE_USER,
	  'b',	"Allowed to ban people on channel",	User::USER,
	  USERFLAG_IS_CHAN_MAINT,
	  FLAG_NONE
     },
     { "CANKICK",	USERFLAG_CAN_KICK,		false,	TYPE_USER,
	  'k',	"Allowed to kick people on channel",	User::USER,
	  USERFLAG_IS_CHAN_MAINT,
	  FLAG_NONE
     },
     { "CANOP",		USERFLAG_CAN_OP,		false,	TYPE_USER,
	  'o',	"Allowed to get ops on channel",	User::USER,
	  USERFLAG_IS_CHAN_MAINT,
	  FLAG_NONE
     },
     { "CANVOICE",	USERFLAG_CAN_VOICE,		false,	TYPE_USER,
	  'v',	"Allowed to get voiced on channel",	User::USER,
	  USERFLAG_IS_CHAN_MAINT,
	  FLAG_NONE
     },
     { "CHANMAINT",	USERFLAG_IS_CHAN_MAINT,		false,	TYPE_USER,
	  '$',	"User can maintain channel",		User::USER,
	  USERFLAG_IS_CHAN_OWNER,
	  FLAG_NONE
     },
     { "CHANOWNER",	USERFLAG_IS_CHAN_OWNER,		false,	TYPE_USER,
	  '%',	"User is a channel operator",		User::MANAGER,
	  FLAG_NONE,			
	  FLAG_NONE
     },
     { "GAME",		CHANFLAG_ALLOW_GAMES,		false,	TYPE_CHANNEL,
	  'G',	"Allow games to be played in channel",	User::USER,
	  USERFLAG_IS_CHAN_MAINT,	
	  FLAG_NONE
     },
     { "ISBOT",		PERSONFLAG_IS_BOT,		false,	TYPE_PERSON,
	  '~',	"User is actually a bot",		User::MANAGER,
	  FLAG_NONE,			
	  FLAG_NONE
     },
     { "LOCKMODES",	CHANFLAG_LOCK_MODES,		false,	TYPE_CHANNEL,
	  '^',	"Channel modes are locked",		User::USER,
	  USERFLAG_IS_CHAN_MAINT,
	  FLAG_NONE
     },
     { "NOBAN",		USERFLAG_PROTECT_NOBAN,		false,	TYPE_USER,
	  'B',	"Protected from being banned",		User::USER,
	  USERFLAG_IS_CHAN_MAINT,	
	  FLAG_NONE
     },
     { "NOCODES",	CHANFLAG_PROTECT_CODES,		false,	TYPE_CHANNEL,
	  'C',	"Protected from excessive formatting",	User::USER,
	  USERFLAG_IS_CHAN_MAINT,	
	  FLAG_NONE
     },
     { "NODEOP",	USERFLAG_PROTECT_NODEOP,	false,	TYPE_USER,
	  'O',	"Protected from being deopped",		User::USER,
	  USERFLAG_IS_CHAN_MAINT,	
	  FLAG_NONE
     },
     { "NODEVOICE",	USERFLAG_PROTECT_NODEVOICE,	false,	TYPE_USER,
	  'V',	"Protected from DeVoice",		User::USER,
	  USERFLAG_IS_CHAN_MAINT,	
	  FLAG_NONE
     },
     { "NOFLOOD",	CHANFLAG_PROTECT_NOFLOOD,	false,	TYPE_CHANNEL,
	  'F',	"Protect channel from flooders",	User::USER,
	  USERFLAG_IS_CHAN_MAINT,	
	  FLAG_NONE
     },
     { "NOKICK",	USERFLAG_PROTECT_NOKICK,	false,	TYPE_USER,
	  'K',	"Protected from being kicked",		User::USER,
	  USERFLAG_IS_CHAN_MAINT,	
	  FLAG_NONE
     },
     { "NOSWEAR",	CHANFLAG_PROTECT_PROFANITY,	false,	TYPE_CHANNEL,
	  'P',	"Protect channel from profanity",	User::USER,
	  USERFLAG_IS_CHAN_MAINT,	
	  FLAG_NONE
     },
     { "SUSPENDED",	PERSONFLAG_SUSPENDED,		false,	TYPE_PERSON,
	  '!',	"This person is suspended",		User::MANAGER,
	  FLAG_NONE,			
	  FLAG_NONE
     },
     { "SUSPENDED",	USERFLAG_SUSPENDED,		false,	TYPE_USER,
	  '!',	"This user is suspended on channel",	User::USER,
	  USERFLAG_IS_CHAN_MAINT,	
	  FLAG_NONE
     },
     { "SUSPENDED",	CHANFLAG_SUSPENDED,		false,	TYPE_CHANNEL,
	  '!',	"This channel is suspended",		User::MANAGER,
	  FLAG_NONE,
	  FLAG_NONE
     },
     { "TALKER",	CHANFLAG_ALLOW_TALKER,		false,	TYPE_CHANNEL,
	  'T',	"Allow me to talk on channel",		User::USER,
	  USERFLAG_IS_CHAN_MAINT,	
	  FLAG_NONE
     },
     { "USEMSG",	PERSONFLAG_USE_PRIVMSG,		true,	TYPE_PERSON,
	  'M',	"Use MSG interface instead of NOTICE",	User::USER,
	  FLAG_NONE,
	  FLAG_NONE
     },
     { NULL,	0,	false,	0,	0,	NULL,	0,	0,	0 }
};
 
