/* ansi.cc
 * Basic ANSI/VT-102 implementation for telnet sessions
 */

#include "ansi.h"
#include "string.h"
#include "utils.h"
#include "version.h"

/* moveLeft - Move left a number of characters
 * Original 30/12/00, Pickle <pickle@alien.net.au>
 */
String ANSI::moveLeft(int num)
{
   return String(ANSI_BEGIN) + String(num) + String("D");
}

/* moveRight - Move cursor left a number of characters
 * Original 30/12/00, Pickle <pickle@alien.net.au>
 */
String ANSI::moveRight(int num)
{
   return String(ANSI_BEGIN) + String(num) + String("C");
}

/* moveUp - Move cursor up a number of characters
 * Original 30/12/00, Pickle <pickle@alien.net.au>
 */
String ANSI::moveUp(int num)
{
   return String(ANSI_BEGIN) + String(num) + String("A");
}

/* moveDown - Move cursor down a number of characters
 * Original 30/12/00, Pickle <pickle@alien.net.au>
 */
String ANSI::moveDown(int num)
{
   return String(ANSI_BEGIN) + String(num) + String("B");
}

/* gotoXY - Move cursor to a specific location
 * Original 30/12/00, Pickle <pickle@alien.net.au>
 */
String ANSI::gotoXY(int x, int y)
{
   return String(ANSI_BEGIN) + String(y) + String(";") + String(x) + 
     String("H");
}

/* scrollRegion - Set the scrolling region on the screen
 * Original 30/12/00, Pickle <pickle@alien.net.au>
 */
String ANSI::scrollRegion(int top, int bottom)
{
   return String(ANSI_BEGIN) + String(top) + String(";") + String(bottom) +
     String("r");
}

/* telnetHeaderInit - Initialise header for telnet sessions
 * Original 30/12/00, Pickle <pickle@alien.net.au>
 */
String ANSI::telnetHeaderInit()
{
   return String(ANSI_CUR_HOME) + String(ANSI_FINVERSE) + String(ANSI_BOLD) +
     String(ANSI_CLR_LINE) + String(" ") + String(VERSION_STRING) +
     ANSI::scrollRegion(2, 25) + String(ANSI_NORMAL) + ANSI::gotoXY(1, 25) +
     ANSI::telnetHeaderUpdate();
}

/* telnetHeaderUpdate - Update header for telnet sessions
 * Original 30/12/00, Pickle <pickle@alien.net.au>
 */
String ANSI::telnetHeaderUpdate()
{
   struct tm *d;
   time_t currentTime = time(NULL);
   
   d = localtime(&currentTime);
   
   return String(ANSI_CUR_SAVE) + String(ANSI_FINVERSE) + String(ANSI_BOLD) +
     ANSI::gotoXY(26, 1) + String(ANSI_CLR_EOL) +
     ANSI::gotoXY(62, 1) /*+ String("[%.2d/%.2d/%.4d %.2d:%.2d]",
				  d->tm_mday, d->tm_mon, d->tm_year + 1900,
				  d->tm_hour, d->tm_min)*/ + String((long)time(NULL)) +
     String(ANSI_CUR_RESTORE) + String(ANSI_NORMAL);
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

