/* flags.cc
 * Resource flagging system
 */

#include "config.h"
#include "flags.h"
#include "user.h"

// User flag table
flagLookupStruct userFlagTable[] = {
     {
	"BOT",		USERFLAG_IS_BOT,		'B',
	"User is a BOT",			User::MASTER 
     },
     {
	"SUSPENDED",	USERFLAG_IS_SUSPENDED,		'S',
	"User is SUSPENDED",			User::MANAGER
     },
     {
	"HOLIDAY",	USERFLAG_IS_ON_HOLIDAY,		'H',
	"User is on holiday",			User::USER
     },
     {
	"MSGONPRIV",	USERFLAG_USE_PRIVMSG_ON_PRIV,	'M',
	"Use /msg on private commands",		User::USER
     },
     {
	"MSGONCHAN",	USERFLAG_USE_PRIVMSG_ON_CHAN,	'm',
	"Use /msg on channel commands",		User::USER
     },
     {
	"EMAILNOTES",	USERFLAG_EMAIL_NOTES,		'N',
	"Forward notes to e-mail",		User::USER
     },
     {
	"MONEYTREE",	USERFLAG_HAS_MONEY_TREE,	'$',
	"User has unlimited credits",		User::MASTER
     },
     { 0, 0, 0, 0, 0 }
};
