/* src/telnet/ansi/cursor.cc
 * Basic ANSI/VT-102 cursor control routines
 * Copyright (c) 2000 Alien Internet Services
 */

#include "config.h"

#include "ansi.h"


/* moveLeft - Move left a number of characters
 * Original 30/12/00, Simon Butcher <simonb@alien.net.au>
 */
String ANSI::moveLeft(unsigned char num = 1)
{
   return (String(ANSI_BEGIN) + String(num) + String("D"));
}


/* moveRight - Move cursor left a number of characters
 * Original 30/12/00, Simon Butcher <simonb@alien.net.au>
 */
String ANSI::moveRight(unsigned char num = 1)
{
   return (String(ANSI_BEGIN) + String(num) + String("C"));
}


/* moveUp - Move cursor up a number of characters
 * Original 30/12/00, Simon Butcher <simonb@alien.net.au>
 */
String ANSI::moveUp(unsigned char num = 1)
{
   return (String(ANSI_BEGIN) + String(num) + String("A"));
}


/* moveDown - Move cursor down a number of characters
 * Original 30/12/00, Simon Butcher <simonb@alien.net.au>
 */
String ANSI::moveDown(unsigned char num = 1)
{
   return (String(ANSI_BEGIN) + String(num) + String("B"));
}


/* gotoXY - Move cursor to a specific location
 * Original 30/12/00, Simon Butcher <simonb@alien.net.au>
 */
String ANSI::gotoXY(unsigned char x, unsigned char y)
{
   return (String(ANSI_BEGIN) + String(y) + String(";") + String(x) + 
	   String("H"));
}

