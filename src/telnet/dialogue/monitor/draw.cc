/* draw.cc
 * Monitor dialogue box, page drawing routines
 */

#include "config.h"

#include "telnetdialogue.h"
#include "ansi.h"
#include "utils.h"
#include "version.h"


/* drawPage - Draw the Login dialogue
 * Original 01/08/01, Simon Butcher <simonb@alien.net.au>
 */
void TelnetDialogueMonitor::drawPage(void)
{
   // Wipe our portion of the screen clean..
   clearWindow();

   // Setup the 'bar' and lock the scroll region to the bottom line
   desc->write(ANSI::gotoXY(1, (desc->rows - 1)) + String(ANSI_FINVERSE) +
	       String(ANSI_CLR_LINE) + // Stuff?
	       String(ANSI_NORMAL) + ANSI::scrollRegion(2, (desc->rows - 2)) +
	       ANSI::gotoXY(1, desc->rows));
}

