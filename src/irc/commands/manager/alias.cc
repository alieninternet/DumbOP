/* src/irc/commands/manager/alias.cc
 * ALIAS command
 * Copyright (c) 2000 Alien Internet Services
 */

#include "config.h"

#include "commands.h"
#include "flags.h"
#include "utils.h"


/* Alias
 * Original 18/12/00, Pickle <pickle@alien.net.au>
 */
void Commands::Alias(Person *from, String channel, String rest)
{
  StringTokens st(rest);
  String newF = st.nextToken().toUpper();
  String oldF = st.nextToken().toUpper();
  list<userFunction *>::iterator it;

  if (newF == "" || oldF == "") {
    from->sendLine("\002Invalid syntax for this command.\002");
    return;
  }

  // First, we check that the "new" function does not exist
  for (it = from->cnx->bot->userFunctions.begin(); it !=
         from->cnx->bot->userFunctions.end(); ++it)
    if (newF == (*it)->name) {
      from->sendLine(newF + " \002is already an alias.\002");
      return;
    }

  // Next, we check that the "old" function exist
  bool found = false;
  userFunction *u = 0;
  for (it = from->cnx->bot->userFunctions.begin(); it !=
         from->cnx->bot->userFunctions.end(); ++it)
    if (oldF == (*it)->name) {
      found = true;
      u = *it;
      break;
    }
  if (!found) {
    from->sendLine(String("\002I don't know the\002 ") + oldF +
                     " \002command.");
    return;
  }

  // Fine, we do the binding
  from->cnx->bot->userFunctions.push_back(new
                               userFunction((char *)(const char 
                                                            *)newF,
                                                   u->function,
                                                   u->minLevel,
                                                   u->needsChannelName, true));

  from->sendLine("\002Alias added.\002");
}
