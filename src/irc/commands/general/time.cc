/* src/irc/commands/general/time.cc
 * TIME command
 * Copyright (c) 1994-2001 Alien Internet Services
 */

#include "config.h"

#include "commands.h"
#include "flags.h"
#include "utils.h"


/* Time - Tell someone the time (duh)
 * Original 6/7/01, Simon Butcher <simonb@alien.net.au>
 * Note: This comes from the original DumbOP, which had a time command
 *       simply because back then ircII did not tell the time natively...
 */
void Commands::Time(Person *from, String channel, String rest)
{
   struct tm *timeNow = localtime(&from->cnx->bot->currentTime.time);
   
   from->sendLine(String("The time is \002") +
		    String(timeNow->tm_hour).prepad(2, '0') + String(":") +
		    String(timeNow->tm_min).prepad(2, '0') + String(":") +
		    String(timeNow->tm_sec).prepad(2, '0') + String(".") +
		    String(from->cnx->bot->currentTime.millitm).prepad(3, '0') + 
		    String("\002, \002") + 
		    Utils::intToDayOfWeek(timeNow->tm_wday) + String(" ") + 
		    String(timeNow->tm_mday) + String(" ") +
		    Utils::intToMonth(timeNow->tm_mon) + String(" ") +
		    String(timeNow->tm_year + 1900) + String("\002."));
}
