/* ansi.cc
 * Basic ANSI/VT-102 implementation for telnet sessions
 */

#include <ctype.h>

#include "ansi.h"
#include "str.h"
#include "utils.h"
#include "version.h"

/* moveLeft - Move left a number of characters
 * Original 30/12/00, Pickle <pickle@alien.net.au>
 */
String ANSI::moveLeft(int num)
{
   return (String(ANSI_BEGIN) + String(num) + String("D"));
}

/* moveRight - Move cursor left a number of characters
 * Original 30/12/00, Pickle <pickle@alien.net.au>
 */
String ANSI::moveRight(int num)
{
   return (String(ANSI_BEGIN) + String(num) + String("C"));
}

/* moveUp - Move cursor up a number of characters
 * Original 30/12/00, Pickle <pickle@alien.net.au>
 */
String ANSI::moveUp(int num)
{
   return (String(ANSI_BEGIN) + String(num) + String("A"));
}

/* moveDown - Move cursor down a number of characters
 * Original 30/12/00, Pickle <pickle@alien.net.au>
 */
String ANSI::moveDown(int num)
{
   return (String(ANSI_BEGIN) + String(num) + String("B"));
}

/* gotoXY - Move cursor to a specific location
 * Original 30/12/00, Pickle <pickle@alien.net.au>
 */
String ANSI::gotoXY(int x, int y)
{
   return (String(ANSI_BEGIN) + String(y) + String(";") + String(x) + 
	   String("H"));
}

/* scrollRegion - Set the scrolling region on the screen
 * Original 30/12/00, Pickle <pickle@alien.net.au>
 */
String ANSI::scrollRegion(int top, int bottom)
{
   return (String(ANSI_BEGIN) + String(top) + String(";") + String(bottom) +
	   String("r"));
}

/* telnetHeaderInit - Initialise header for telnet sessions
 * Original 30/12/00, Pickle <pickle@alien.net.au>
 */
String ANSI::telnetHeaderInit()
{
   return (String(ANSI_CUR_HOME) + String(ANSI_FINVERSE) + String(ANSI_BOLD) +
	   String(ANSI_CLR_LINE) + String(" ") + String(VERSION_STRING) +
	   String(" - Management Console") + scrollRegion(2, 25) + 
	   String(ANSI_NORMAL) + gotoXY(1, 25) + telnetHeaderUpdate());
}

/* telnetHeaderUpdate - Update header for telnet sessions
 * Original 30/12/00, Pickle <pickle@alien.net.au>
 */
String ANSI::telnetHeaderUpdate()
{
   time_t currentTime = time(NULL);
   struct tm *timeNow = localtime(&currentTime);
   
   return (String(ANSI_CUR_SAVE) + String(ANSI_FINVERSE) + String(ANSI_BOLD) +
	   ANSI::gotoXY(40, 1) + String(ANSI_CLR_EOL) + ANSI::gotoXY(54, 1) + 
	   (Utils::intToMonth(timeNow->tm_mon) + String(" ") +
	    String(timeNow->tm_mday) + String(" ") +
	    String(timeNow->tm_year + 1900) + String(", ") +
	    String(timeNow->tm_hour).prepad(2, '0') + String(":") +
	    String(timeNow->tm_min).prepad(2, '0')).prepad(26) +
	   String(ANSI_CUR_RESTORE) + String(ANSI_NORMAL));
}

// Table roughly converting "Khaled Colour" to base ANSI
struct {
   char *fgColour;
   char *bgColour;
} colourTable[16] = { // 0..15 colours
     { ANSI_HWHITE,		ANSI_BWHITE },		// 00 - White
     { ANSI_BLACK,		ANSI_BBLACK },		// 01 - Black
     { ANSI_BLUE,		ANSI_BBLUE },		// 02 - Dark Blue
     { ANSI_GREEN,		ANSI_BGREEN },		// 03 - Dark Green
     { ANSI_HRED,		ANSI_BRED },		// 04 - Light Red
     { ANSI_RED,		ANSI_BRED },		// 05 - Dark Red
     { ANSI_MAGENTA,		ANSI_BMAGENTA },	// 06 - Dark Magenta
     { ANSI_YELLOW,		ANSI_BRED },		// 07 - Orange
     { ANSI_HYELLOW,		ANSI_BYELLOW },		// 08 - Yellow
     { ANSI_HGREEN,		ANSI_BGREEN },		// 09 - Light Green
     { ANSI_CYAN,		ANSI_BCYAN },		// 10 - Dark Cyan
     { ANSI_HCYAN,		ANSI_BCYAN },		// 11 - Light Cyan
     { ANSI_BLUE,		ANSI_BBLUE },		// 12 - Dark Blue
     { ANSI_HMAGENTA,		ANSI_BMAGENTA },	// 13 - Light Magenta
     { ANSI_WHITE,		ANSI_BWHITE },		// 14 - Dark Grey
     { ANSI_WHITE,		ANSI_BWHITE }		// 15 - Light Grey
};

/* toANSI - Convert IRC formatting to ANSI
 * Original 29/06/01, Pickle <pickle@alien.net.au>
 * Note: This doesn't only convert IRC formatting to ANSI, it also strips any
 *       nasties that it might find, such as CTCP chars etc.
 */
String ANSI::toANSI(String line)
{
   if (line.length() < 1) {
      return line;
   }
   
   String temp = "";
   bool bold_on = false, underline_on = false, inverse_on = false;
   bool attr_change = false, fgColour_change = false, bgColour_change = false;
   bool coloured = false;
   char fgColour = 15, bgColour = 1;
   
   for (int i = 0; i < line.length(); i++) {
      switch (line[i]) {
       case '\002': /* bold (^B) - toggle the bold setting */
	 bold_on = !bold_on;
	 attr_change = true;
	 break;
       case '\003': /* colour (^C) - Convert mIRC's "Khaled Colour" *gulp* */
	 fgColour_change = true;
	 attr_change = true;
	 
	 /* Make sure we are not being played with */
	 if (isdigit(line[i+1])) {
	    i++;
	    coloured = true;
	    
	    /* OK... BIG EXPLANATION OF HOW THIS WORKS IS NEEDED HERE!!
	     * Hehe, shortcut ppl? Yes. atoi() grabs char by char down the
	     * pointer to establish the number, no error checking, until
	     * it hits a non-digit then it stops. Eg "15," would return 15
	     * and then it'll stop because it's hit the comma. We can use
	     * this to our advantage to read mIRC's horrible colour format
	     * which has very poor formatting rules. All we have to do is
	     * work out if the number is two digits as only values 0..15 are
	     * acceptable, then check if there's a comma to proceed to
	     * the background colour. Easy hey! :) It IS a shortcut and may
	     * produce some awkward bugs which will need some awkward work-
	     * arounds..
	     */
	    fgColour = atoi(&line[i]);
	    
	    // Are we dealing with a two digit number?
	    if (isdigit(line[i+1])) {
	       i++;
	    }
		
	    /* Are we looking at a colour change WITH a background?
	     * We also check here if we are being screwed around...
	     */
	    if ((line[i+1] == ',') && (isdigit(line[i+2]))) {
	       i += 2;
	       bgColour_change = true;
	       
	       bgColour = atoi(&line[i]);

	       // Are we dealing with another two digit number?
	       if (isdigit(line[i+1])) {
		  i++;
	       }
	    }
	 } else { /* If it's broken, we need to act like mIRC and reset */
	    coloured = false; /* YES! We go back to attribute only style */
	    fgColour = 15;
	    bgColour = 1;
	 }
	 break;
       case '\006': /* ^G - a beep char.. We don't need that anymore */
	 break;
       case '\026': /* inverse (^W) - toggle the inverse setting */
	 inverse_on = !inverse_on;
	 attr_change = true;
	 break;
       case '\037': /* underline - act the same as bold a la ircII */
	 underline_on = !underline_on;
	 attr_change = true;
	 break;
       default: /* Everything else falls through the trap */
	 temp = temp + String(line[i]);
      }
      
      /* If we changed a basic attribute (bold, underline, inverse) then
       * we have to append the appropriate ANSI codes. To compensate for
       * our stupidity, we append a full ANSI attribute string.
       */
      if (attr_change) {
	 // Start of ANSI + normal
	 temp = temp + String(ANSI_BEGIN) + String("0");
	 
	 // Bold?
	 if (bold_on) {
	    temp = temp + String(";1");
	 }
	 
	 // Underline?
	 if (underline_on) {
	    temp = temp + String(";4");
	 }
	 
	 // Inverse?
	 if (inverse_on) {
	    temp = temp + String(";7");
	 }

	 // End of ANSI codes
	 temp = temp + String("m");
	 
	 /* If we have changed some colour information, it's time we appended
	  * the new it here too... As with above, we are stupid about how we
	  * do this, and do it the LONG way rather than the simple way.
	  * BTW, why don't we put the colours properly into the attribute
	  * setting string as above? It's easier to change the colour mapping
	  * table above if it's this way.... OK OK, shush, I'm lazy.. Fix it
	  * one day later but it's not important! :)
	  * 
	  * We cannot change do any colour unless we are NOT in inverse mode.
	  * mIRC has this strange way or working, so I suppose we are sort of
	  * forced to follow suite.
	  */
	 if (fgColour_change && !inverse_on) {
	    // Foreground colour
	    temp = temp + String(colourTable[fgColour].fgColour);
	    
	    /* Following the way mIRC works, background colours cannot
	     * change unless the forground colour is reset or change itself.
	     * This is more proof that mIRC's colour scheme is pretty crap.
	     */
	    if (bgColour_change) {
	       temp = temp + String(colourTable[bgColour].bgColour);
	    }
	 }
      }
   }

   /* This magical mystical line fixed everything up so that any chars after
    * this converted string are back to normal. This stops any horrible
    * bleeding effects that might be created after IRC idiots do not terminate
    * their attributes.
    */
   temp = temp + String(ANSI_NORMAL);
   
   return temp;
}
