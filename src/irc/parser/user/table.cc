/* src/irc/parser/user/table.cc
 * Parser-Functions for User class connections
 * Copyright (c) 1998-2001 Alien Internet Services
 */

#include "config.h"

#include "parser.h"
#include "tables.h"

parserFunctionStruct userParserFunctions[] = {
     { "001",		Parser::parseUser001 },
     { "302",		Parser::parseUser302 },
//     { "303",		Parser::parseUser303 },
//     { "311",		Parser::parseUser311 },
     { "315",		Parser::parseUser315 },
     { "324",		Parser::parseUser324 },
     { "332",		Parser::parseUser332 },
     { "352",		Parser::parseUser352 },
     { "353",		Parser::parseUser353 },
     { "366",		Parser::parseUser366 },
     { "367",		Parser::parseUser367 },
//     { "401",		Parser::parseUser401 },
     { "433",		Parser::parseUser433 },
     { "437",		Parser::parseUser433 },
     { "471",		Parser::parseUser473 },
     { "473",		Parser::parseUser473 },
     { "474",		Parser::parseUser473 },
     { "475",		Parser::parseUser473 },
     { "ERROR", 	Parser::parseUserError },
     { "INVITE",	Parser::parseUserInvite },
     { "JOIN",  	Parser::parseUserJoin },
     { "KICK",  	Parser::parseUserKick },
     { "MODE",  	Parser::parseUserMode },
     { "NICK",		Parser::parseUserNick },
     { "NOTICE",	Parser::parseUserNotice },
     { "PART",  	Parser::parseUserPart },
     { "PING",  	Parser::parseUserPing },
     { "PONG",		Parser::parseUserPong },
     { "PRIVMSG",	Parser::parseUserPrivmsg },
     { "QUIT",		Parser::parseUserQuit },
     { "TOPIC", 	Parser::parseUserTopic },
//     { "WALLOPS",	Parser::parseUserWallops },
     { "",	    	Parser::parseUserError }, // well, it has happened!
     { 0, 0 }
};

