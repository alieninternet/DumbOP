/* src/telnet/ansi/drawing.cc
 * Basic ANSI/VT-102 drawing routines
 * Copyright (c) 1994-2001 Alien Internet Services
 */

#include "config.h"

#include "ansi.h"
#include "utils.h"


/* drawHLine - Draw a horizontal line at x,y for l
 * Original 15/08/94, Simon Butcher <simonb@alien.net.au>
 * 29/07/01 simonb - Ported from AXS BBS/internet connection menu (dos pascal)
 * Note: The x,y default to -1 which means 'draw from here'.
 */
String ANSI::drawHLine(short l, short x = -1, short y = -1)
{
   String temp = "";

   // Set our location if we need to.
   if ((x >= 0) || (y >= 0)) {
      temp = gotoXY(x, y);
   }

   // Add the chars
   temp = temp + Utils::repeatChar(l, ANSI_DRAW_HORIZONTAL_LINE);
   
   return temp;
}


/* drawVLine - Draw a verticle line at x,y for l
 * Original 15/08/94, Simon Butcher <simonb@alien.net.au>
 * 29/07/01 simonb - Ported from AXS BBS/internet connection menu (dos pascal)
 * Note: It is faster to do a cursor down, then a backspace, rather than
 * 	 either a cursor down then left, or a gotoxy. NOTE, if we are told
 * 	 not to move the char (with X,Y both being -1) then the char must be
 *       originally offset by being one char to the right, one char above of
 *       the actual point of drawing.
 */
String ANSI::drawVLine(short l, short x = -1, short y = -1)
{
   String temp = "";
   
   // Set our location if we need to.
   if ((x >= 0) || (y >= 0)) {
      temp = gotoXY((x + 1), (y - 1));
   }
   
   // Run through the line chars
   for (short i = 0; i < l; i++) {
      temp = temp + String(ANSI_CUR_DOWN) + String("\b") + 
	String(ANSI_DRAW_VERTICLE_LINE);
   }
   
   return temp;
}


/* drawBox - Draw a box with the two line functions from xA,yA to xB,yB
 * Original 15/08/94, Simon Butcher <simonb@alien.net.au>
 * 29/07/01 simonb - Ported from AXS BBS/internet connection menu (dos pascal)
 * Note: We save chars by starting from the beginning, doing the top line
 * 	 and the right-hand line, then jumping back to the top corner again,
 *       doing the left line, then the bottom line. Whew! Hopefully it wont
 *       look that crap - admittantly it would look better to draw top to
 *       bottom in one sweep, but this is technically 'faster'.. hrrm :-/
 */
String ANSI::drawBox(short xA, short yA, short xB, short yB)
{
   return (gotoXY(xA, yA) + String(ANSI_DRAW_TOP_LH_CORNER) + 
	   drawHLine(xB - xA - 2) + String(ANSI_DRAW_TOP_RH_CORNER) +
	   drawVLine(yB - yA - 2) + gotoXY(xA + 1, yA) + 
	   drawVLine(yB - yA - 2) + String(ANSI_CUR_DOWN) + String("\b") +
	   String(ANSI_DRAW_BOTTOM_LH_CORNER) + drawHLine(xB - xA - 2) +
	   String(ANSI_DRAW_BOTTOM_RH_CORNER));
}


