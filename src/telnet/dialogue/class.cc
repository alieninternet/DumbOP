/* class.cc
 * Telnet Dialogue handler class
 */

#include "telnetdialogue.h"
#include "ansi.h"

/* TelnetDialogue - Dialogue base class constructor
 * Original 28/07/01, Simon Butcher <simonb@alien.net.au>
 */
TelnetDialogue::TelnetDialogue(TelnetDescriptor *d)
: desc(d)
{
}


/* ~TelnetDialogue - Virtual class destructor
 * Original 29/07/01, Simon Butcher <simonb@alien.net.au>
 */
TelnetDialogue::~TelnetDialogue(void)
{
   // This is virtual. Nothing should go here.
}


/* drawPage - Virtual page drawing routine
 * Original 29/07/01, Simon Butcher <simonb@alien.net.au>
 */
void TelnetDialogue::drawPage(void)
{
   // This is virtual. Nothing should go here.
}


/* parseInput - Virtual input char handling routine
 * Original 29/07/01, Simon Butcher <simonb@alien.net.au>
 */
void TelnetDialogue::parseInput(char chr)
{
   // This is virtual. Nothing should go here.
}


/* calcBoxTLHC - Calculate a dialogue box top left-hand corner (centered box)
 * Original 29/07/01, Simon Butcher <simonb@ailen.net.au>
 */
void TelnetDialogue::calcBoxTLHC(unsigned char w, unsigned char h)
{
   boxX = (unsigned char)((desc->columns / 2) - (w / 2));
   boxY = (unsigned char)((desc->rows / 2) - (h / 2));
}


/* clearWindow - Clear the 'window' (everywhere BUT the status bar thing)
 * Original 29/07/01, Simon Butcher <simonb@alien.net.au>
 * Note: According to the ANSI specs by IBM in the early 80's, a simple
 * 	 clear-down command should clear the current line, and line below it.
 *       Standards being standards, most of my terminals don't follow this
 *       and clear from the line BELOW the current one and down. This is why
 *       we have to do a clear-line, then clear down :(
 */
void TelnetDialogue::clearWindow(void)
{
   desc->write(String(ANSI_CUR_SAVE) + ANSI::gotoXY(1,2) +
	       String(ANSI_NORMAL) + String(ANSI_CLR_LINE) + 
	       String(ANSI_CLR_DOWN) + String(ANSI_CUR_RESTORE));
}
   
				    
