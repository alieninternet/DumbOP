/* src/telnet/descriptor/protocol.cc
 * Telnet descriptor (connection) telnet protocol handlers
 * Copyright (c) 2001 Alien Internet Services
 */

#include "config.h"

#include "telnet.h"
#include "protocol.h"


/* parseTelnet - Parse a telnet process code thing
 * Original 3/2/01, Simon Butcher <simonb@alien.net.au>
 * 02/08/01 simonb - Adapted for DumbOP management console use
 */
void TelnetDescriptor::parseTelnet(String line)
{
   // This code seems broken.
   return;
   
   // Check to see if there is a point to run through this line
   if ((line.length() > 1) && // Make sure the line is long enough, and...
       (line.find(TC_IAC) >= 0)) { // Make sure there is an Telnet IAC in there
      String tempStr = "";
      bool inCode = false;
      bool inSub = false;
      
      for (long i = 0; i <= line.length(); i++)
	switch (line[i]) {
	 case TC_IAC:
	   inCode = true;
	   inSub = false;
	   while (inCode) {
	      switch (line[++i]) {
	       case TC_SB:
		 inSub = true; // We are now in a subnegotiation bit
		 break;
	       case TC_IAC:
		 if (inSub) { // Are we in a subnegotiation?
		 } else { // We are not in a subnegotiation, it is invalid...
		    inCode = false; // Leave the loop
		 }
		 break;
	       case TC_SE:
		 inSub = false; // We are now out of a subnegotiation bit, so...
		 inCode = false; // Leave the loop
		 break;
	       case TC_WILL:
		 break;
	       case TC_WONT:
		 break;
	       case TC_DO:
		 switch (line[++i]) {
		  default: // We dont know it, so we dont do it
		    inCode = false; // Leave the loop
		 }
		 break;
	       case TC_DONT:
		 break;
	       default: // Invalid or unsupported command
		 inCode = false; // Leave the loop
	      }
	   }
	   break;
	 default:
	   tempStr = tempStr + String(line[i]);
	}
   }
}


/* telnetWill - Send a telnet "WILL" command
 * Original 04/08/01, Simon Butcher <simonb@alien.net.au>
 */
void TelnetDescriptor::telnetWill(char option)
{
   write(String(TC_IAC) + String(TC_WILL) + String(option));
}


/* telnetWont - Send a telnet "WON'T" command
 * Original 04/08/01, Simon Butcher <simonb@alien.net.au>
 */
void TelnetDescriptor::telnetWont(char option)
{
   write(String(TC_IAC) + String(TC_WONT) + String(option));
}


/* telnetDo - Send a telnet "DO" command
 * Original 04/08/01, Simon Butcher <simonb@alien.net.au>
 */
void TelnetDescriptor::telnetDo(char option)
{
   write(String(TC_IAC) + String(TC_DO) + String(option));
}


/* telnetDont - Send a telnet "DON'T" command
 * Original 04/08/01, Simon Butcher <simonb@alien.net.au>
 */
void TelnetDescriptor::telnetDont(char option)
{
   write(String(TC_IAC) + String(TC_DONT) + String(option));
}

