/* src/telnet/ansi/class.cc
 * Basic ANSI/VT-102 implementation for telnet sessions
 * Copyright (c) 1994-2001 Alien Internet Services
 */

#include "config.h"

#include <ctype.h>

#include "ansi.h"
#include "utils.h"
#include "version.h"


/* scrollRegion - Set the scrolling region on the screen
 * Original 30/12/00, Simon Butcher <simonb@alien.net.au>
 */
String ANSI::scrollRegion(unsigned char top, unsigned char bottom)
{
   return (String(ANSI_BEGIN) + String(top) + String(";") + String(bottom) +
	   String("r"));
}

