/* src/telnet/descriptor/header.cc
 * Telnet descriptor (connection) header bar routines
 */

#include "config.h"

#include <time.h>

#include "telnet.h"
#include "ansi.h"
#include "utils.h"


/* headerInit - Initialise header for telnet sessions
 * Original 30/12/00, Simon Butcher <simonb@alien.net.au>
 * Note: Clears the screen, draws the basic header, and flows through to
 *       further header routines to draw the message and time.
 */
void TelnetDescriptor::headerInit(void)
{
   write(String(ANSI_CLR_SCREEN) + String(ANSI_CUR_HOME) + 
	 String(ANSI_FINVERSE) + String(ANSI_CLR_LINE) + String(" ") + 
	 Version::getVersion().pad(columns - 1) + 
	 ANSI::scrollRegion(2, rows));
   
   /* Fire up the status bar with a new message. This will also call the
    * first header bar update to occur (eg. put the time etc on there)
    */
   headerMessage();
}


/* headerUpdate - Update header for telnet sessions
 * Original 30/12/00, Simon Butcher <simonb@alien.net.au>
 * Note: We save the cursor location before updating the bar. If the client
 * 	 does not support this ANSI string, TO HELL WITH THEM! Pretty bad
 * 	 client if it doesn't even support THAT.
 */
void TelnetDescriptor::headerUpdate(void)
{
   struct tm *timeNow = localtime(&telnet->bot->currentTime.time);
   String timeStr = (Utils::intToMonth(timeNow->tm_mon) + String(" ") +
		     String(timeNow->tm_mday) + String(" ") +
		     String(timeNow->tm_year + 1900) + String(", ") +
		     String(timeNow->tm_hour).prepad(2, '0') + String(":") +
		     String(timeNow->tm_min).prepad(2, '0'));
   
   
   write(String(ANSI_CUR_SAVE) + String(ANSI_FINVERSE) + 
	 ANSI::gotoXY((columns - timeStr.length()), 1) + timeStr +
	 String(ANSI_CUR_RESTORE) + String(ANSI_NORMAL));
}


/* headerMessage - Update header message
 * Original 30/12/00, Simon Butcher <simonb@alien.net.au>
 * Note: We also prevoke a header update, since we clear the line.
 */
void TelnetDescriptor::headerMessage(String message = "")
{
   // Are we changing the message?
   if (message.length()) {
      barMessage = message;
   }

   // Pump out the message.
   write(String(ANSI_CUR_SAVE) + String(ANSI_FINVERSE) + 
	 ANSI::gotoXY(15, 1) + String(ANSI_CLR_EOL) +
	 ANSI::gotoXY((int)((columns / 2) - (barMessage.length() / 2)), 1) +
	 barMessage + String(ANSI_CUR_RESTORE) + String(ANSI_NORMAL));
   
   // Fire up the data we killed
   headerUpdate();
}


