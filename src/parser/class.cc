
#include "parser.h"
#include "telnetspy.h"

struct {
   char *name;
   void (*function)(ServerConnection *, Person *, String);
} functions[] = {
     { "001",	Parser::parse001     }, /* RPL_WELCOME - Welcome message */
   /* 002 - RPL_YOURHOST - Server/Version info */
   /* 003 - RPL_CREATED - Date server built */
   /* 004 - RPL_MYINFO - Detailed server info */
   /* 005 - RPL_BOUNCE - Use given server */
   /* 200 - RPL_TRACELINK - Trace link info */
   /* 201 - RPL_TRACECONNECTING - Try trace on... */
   /* 202 - RPL_TRACEHANDSHAKE - Trace handshake */
   /* 203 - RPL_TRACEUNKNOWN - Unknown trace */
   /* 204 - RPL_TRACEOPERATOR - Trace +O entry */
   /* 205 - RPL_TRACEUSER - Trace user entry */
   /* 206 - RPL_TRACESERVER - Trace server entry */
   /* 207 - RPL_TRACESERVICE - Trace service entry */
   /* 208 - RPL_TRACENEWTYPE - Trace other entry */
   /* 209 - RPL_TRACECLASS - Trace class entry */
   /* 210 - RPL_TRACERECONNECT - Reconnect trace */
   /* 211 - RPL_STATSLINKINFO - Link info stats */
   /* 212 - RPL_STATSCOMMANDS - Command stats */
   /* 213 - RPL_STATSCLINE - ? */
   /* 214 - RPL_STATSNLINE - ? */
   /* 215 - RPL_STATSILINE - ? */
   /* 216 - RPL_STATSKLINE - ? */
   /* 217 - RPL_STATSQLINE - ? */
   /* 218 - RPL_STATSYLINE - ? */
   /* 219 - RPL_ENDOFSTATS - End of stats */
   /* 221 - RPL_UMODEIS - User mode is... */
   /* 231 - RPL_SERVICEINFO - ? */
   /* 232 - RPL_ENDOFSERVICES - ? */
   /* 233 - RPL_SERVICE - ? */
   /* 234 - RPL_SERVLIST - Server list entry */
   /* 235 - RPL_SERVLISTEND - End of server list */
   /* 240 - RPL_STATSVLINE - ? */
   /* 241 - RPL_STATSLLINE - ? */
   /* 242 - RPL_STATSUPTIME - Uptime stats */
   /* 243 - RPL_STATSONLINE - Online stats */
   /* 244 - RPL_STATSHLINE - ? */
   /* 245 - RPL_STATSSLINE - ??? */
   /* 246 - RPL_STATSPING - ? */
   /* 247 - RPL_STATSBLINE - ? */
   /* 250 - RPL_STATSDLINE - ? */
   /* 251 - RPL_LUSERCLIENT - User connections info */
   /* 252 - RPL_LUSEROP - +O connections info */
   /* 253 - RPL_LUSERUNKNOWN - Unknown connections info */
   /* 254 - RPL_LUSERCHANNELS - Number of channels info */
   /* 255 - RPL_LUSERME - Local connections info */
   /* 256 - RPL_ADMINME - Local admin info */
   /* 257 - RPL_ADMINLOC1 - Local admin info (1) */
   /* 258 - RPL_ADMINLOC2 - Local admin info (2) */
   /* 259 - RPL_ADMINEMAIL - Local admin e-mail info */
   /* 261 - RPL_TRACELOG - Trace log file */
   /* 262 - RPL_TRACEEND - End of trace */
   /* 263 - RPL_TRYAGAIN - Try again soon */
   /* 300 - RPL_NONE - ? */
   /* 301 - RPL_AWAY - User is away */
     { "302",	Parser::parse302     }, /* RPL_USERHOST - ? */
     { "303",   Parser::parse303     }, /* RPL_ISON  - ? */
   /* 305 - RPL_UNAWAY - Away turned off msg */
   /* 306 - RPL_NOWAWAY - Marked as AWAY */
//     { "311",	Parser::parse311     }, /* RPL_WHOISUSER - User details */
   /* 312 - RPL_WHOISSERVER - User is using this */
   /* 313 - RPL_WHOISOPERATOR - User is IRC OP */
   /* 314 - RPL_WHOWASUSER - WHOWAS details */
     { "315",	Parser::parse315     }, /* RPL_ENDOFWHO - End of WHO */
   /* 316 - RPL_WHOISCHANOP - ? */
   /* 317 - RPL_WHOISIDLE - User idle stats */
   /* 318 - RPL_ENDOFWHOIS - End of WHOIS list */
   /* 319 - RPL_WHOISCHANNELS - User is on these */
   /* 321 - RPL_LISTSTART - Start of channel LIST */
   /* 322 - RPL_LIST - Channel LIST entry */
   /* 323 - RPL_LISTEND - End of channel LIST */
     { "324",   Parser::parse324     }, /* RPL_CHANNELMODEIS - Channel mode is...*/
   /* 325 - RPL_UNIQOPIS - ?? */
   /* 331 - RPL_NOTOPIC - No topic is set */
     { "332",   Parser::parse332     }, /* RPL_TOPIC - Topic is now... */
   /* 341 - RPL_INVITING - INVITEd someone OK */
   /* 342 - RPL_SUMMONING - SUMMONing someone */
   /* 346 - RPL_INVITELIST - INVITE list entry */
   /* 347 - RPL_ENDOFINVITELIST - End of INVITE list */
   /* 348 - RPL_EXCEPTLIST - EXCEPTION list entry */
   /* 349 - RPL_ENDOFEXCEPTLIST - End of EXCEPTION list */
   /* 351 - RPL_VERSION - Server version details */
     { "352",   Parser::parse352     }, /* RPL_WHOREPLY - WHO entry */
     { "353",   Parser::parse353     }, /* RPL_NAMESREPLY - NAMES entry */
   /* 361 - RPL_KILLDONE - ? */
   /* 362 - RPL_CLOSING - ? */
   /* 363 - RPL_CLOSEEND - ? */
   /* 364 - RPL_LINKS - LINKS entry */
   /* 365 - RPL_ENDOFLINKS - End of LINKS list */
     { "366",   Parser::parse366     }, /* RPL_ENDOFNAMES - End of NAMES list */
     { "367",   Parser::parse367     }, /* RPL_BANLIST - BANLIST entry */
   /* 368 - RPL_ENDOFBANDLIST - End of BANLIST */
   /* 369 - RPL_ENDOFWHOWAS - End of WHOWAS */
   /* 371 - RPL_INFO - INFO entry */
   /* 372 - RPL_MOTD - MOTD entry */
   /* 373 - RPL_INFOSTART - ? */
   /* 374 - RPL_ENDOFINFO - End of INFO list */
   /* 375 - RPL_MOTDSTART - Start of MOTD list */
   /* 376 - RPL_ENDOFMOTD - End of MOTD list */
   /* 381 - RPL_YOUREOPER - You got +O */
   /* 382 - RPL_REHASHING - Rehashing */
   /* 383 - RPL_YOURESERVICE - You are service.. */
   /* 384 - RPL_MYPORTIS - ? */
   /* 391 - RPL_TIME - Server time */
   /* 392 - RPL_USERSSTART - Users list */
   /* 393 - RPL_USERS - Users list entry */
   /* 394 - RPL_ENDOFUSERS - End of user list */
   /* 395 - RPL_NOUSERS - Nobody logged in */
     { "401",   Parser::parse401     }, /* ERR_NOSUCHNICK - No such nick */
   /* 402 - ERR_NOSUCHSERVER - No such server */
   /* 403 - ERR_NOSUCHCHANNEL - No such channel */
   /* 404 - ERR_CANNOTSENDTOCHAN - Cannot send to channel */
   /* 405 - ERR_TOOMANYCHANNELS - On too many channels */
   /* 406 - ERR_WASNOSUCHNICK - No nick in WHOWAS history */
   /* 407 - ERR_TOOMANYTARGETS - Target list overloaded */
   /* 408 - ERR_NOSUCHSERVICE - No such service */
   /* 409 - ERR_NOORIGIN - No origin given */
   /* 411 - ERR_NORECIPIENT - No recipient given */
   /* 412 - ERR_NOTEXTTOSEND - No text given */
   /* 413 - ERR_NOTOPLEVEL - No top level domain give */
   /* 414 - ERR_WILDTOPLEVEL - Top level was wildcard */
   /* 415 - ERR_BADMASK - Invalid mask given */
   /* 421 - ERR_UNKNOWNCOMMAND - Unknown command */
   /* 422 - ERR_NOMOTD - No MOTD on server */
   /* 423 - ERR_NOADMININFO - No admin info */
   /* 424 - ERR_FILEERROR - File error */
   /* 431 - ERR_NONICKNAMEGIVEN - No nick given */
   /* 432 - ERR_ERRONEUSNICKNAME - Bad nick given */
     { "433",   Parser::parse433     }, /* ERR_NICKNAMEINUSE - Nick in use */
   /* 436 - ERR_NICKCOLLISION - Nick collision KILL */
     { "437",   Parser::parse433     }, /* ERR_UNAVAILRESOURCE - Delay block */
   /* 441 - ERR_USERNOTINCHANNEL - User not in channel */
   /* 442 - ERR_NOTONCHANNEL - Not in channel */
   /* 443 - ERR_USERONCHANNEL - Already on channel */
   /* 444 - ERR_NOLOGIN - Not logged on */
   /* 445 - ERR_SUMMONDISABLED - No SUMMON command */
   /* 446 - ERR_USERSDISABED - No USERS command */
   /* 451 - ERR_NOTREGISTERS - Not registered */
   /* 461 - ERR_NEEDMOREPARAMS - Not enough params */
   /* 462 - ERR_ALREADYREGISTERD - Already registered */
   /* 463 - ERR_NOPERMFORHOST - Host not privileged */
   /* 464 - ERR_PASSWDMISMATCH - Bad password */
   /* 465 - ERR_YOUREBANNEDCREEP - Banned from server */
   /* 466 - ERR_YOUWILLBEBANNED - Access to be denied soon */
   /* 467 - ERR_KEYSET - Channel key already set */
     { "471",   Parser::parse473     }, /* ERR_CHANNELISFULL - +l overflow */
   /* 472 - ERR_UNKNOWNMODE - Unknown mode char */
     { "473",   Parser::parse473     }, /* ERR_INVITEONLYCHAN - +i channel */
     { "474",   Parser::parse473     }, /* ERR_BANNEDFROMCHAN - +b channel */
     { "475",   Parser::parse473     }, /* ERR_BADCHANNELKEY - +k channel */
   /* 476 - ERR_BADCHANMASK - Channel mask bad */
   /* 477 - ERR_NOCHANMODES - Channel has no modes */
   /* 478 - ERR_BANLISTFULL - Too many banned masks */
   /* 481 - ERR_NOPRIVILEGES - Not a +O */
   /* 482 - ERR_CHANOPRIVSNEEDED - Not a +o */
   /* 483 - ERR_CANTKILLSERVER - Cannot kill a server */
   /* 484 - ERR_RESTRICTED - Connection restricted */
   /* 485 - ERR_UNIQOPPRIVSNEEDED - Not original +o */
   /* 491 - ERR_NOOPERHOST - No O-line for host */
   /* 492 - ERR_NOSERVICEHOST - ? */
   /* 501 - ERR_UMODEUNKNOWNFLAG - Unknown user mode char */
   /* 502 - ERR_USERSDONTMATCH - No mode change for other users */
     { "ERROR", 	Parser::parseError	},
     { "INVITE",	Parser::parseInvite	},
     { "JOIN",  	Parser::parseJoin	},
     { "KICK",  	Parser::parseKick	},
     { "MODE",  	Parser::parseMode	},
     { "NICK",		Parser::parseNick	},
     { "NOTICE",	Parser::parseNotice	},
     { "PART",  	Parser::parsePart	},
     { "PING",  	Parser::parsePing	},
     { "PONG",		Parser::parsePong	},
     { "PRIVMSG",	Parser::parsePrivmsg	},
     { "QUIT",		Parser::parseQuit	},
     { "TOPIC", 	Parser::parseTopic	},
//     { "WALLOPS",	Parser::parseWallops	},
     { "",	    	Parser::parseError	},
     { 0,         0                    }
};

/* parseLine - Parse a line from the server then act upon it
 * Original 12/12/00, Pickle <pickle@alien.net.au>
 */
void Parser::parseLine(ServerConnection * cnx, String line)
{
   StringTokens st(line);
   Person * from = 0;
   String fromMask = "";
   
   cnx->receivedLen += line.length();

   if (line[0] == ':') {
      fromMask = st.nextToken().subString(1);
      if (fromMask.find('!') != -1)
	from = new Person(cnx->bot, fromMask);
   }
   
   String command = st.nextToken();
   String rest = st.rest();
      
   for (int i = 0; functions[i].name != 0; i++) {
      if (command == functions[i].name) {
	 functions[i].function(cnx, from, Utils::dwindleSpaces(rest));
	 break;
      }
   }

   TelnetSpy::spyLine(cnx->bot, fromMask, command, rest);
   
   delete from;
}
