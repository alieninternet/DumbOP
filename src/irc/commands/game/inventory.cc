/* src/irc/commands/game/inventory.cc
 * INVENTORY command
 * Copyright (c) 2001 Alien Internet Services
 */

#include "config.h"

#include "commands.h"
#include "user.h"
#include "flags.h"
#include "utils.h"


/* Inventory - Show a user's inventory
 * Original 04/08/01, Simon Butcher <simonb@alien.net.au>
 * Note: Does nothing much apart from show a user how many credits they have.
 */
void Commands::Inventory(Person *from, String channel, String rest)
{
   // Make sure this came from a valid user
   if (!from->uli) {
      return;
   }
   
   // Tell the user how many credits they have, if any
   from->sendLine(String("You have ") + 
		  (((from->uli->flags & USERFLAG_HAS_MONEY_TREE) ||
		    (from->uli->level >= User::MANAGER)) ?
		   String("unlimited") :
		   ((from->uli->credits > 0) ?
		    String(from->uli->credits) : String("no"))) +
		  String(" credits."));
}

