/* src/irc/commands/manager/checkpoint.cc
 * CHECKPOINT command
 * Copyright (c) 2000-2001 Alien Internet Services
 */

#include "config.h"

#include "commands.h"
#include "flags.h"
#include "utils.h"


/* Checkpoint - Force a database checkpoint
 * Original 15/12/00, Simon Butcher <simonb@alien.net.au>
 * 02/08/01 simonb - Checkpoint routine is now in Bot class.
 * 02/08/01 simonb - Added more output (in case of future *LONG* checkpoints)
 */
void Commands::Checkpoint(Person *from, String channel, String rest)
{
   // Warn the user about potential delays
   from->sendLine("Performing database checkpoint...");
   
   // Do the checkpoint *gulp*
   from->cnx->bot->checkpoint();
   
   // Tell the user the pain is over!
   from->sendNotice("Database checkpoint complete!");
}
