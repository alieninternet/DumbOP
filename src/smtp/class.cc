/* src/smtp/class.cc
 * SMTP (email) main class
 * Copyright (c) 2001 Alien Internet Services
 */

#include "config.h"
#include "smtp.h"


/* SMTP - Initialise the SMTP mailer engine
 * Original 01/08/01, Simon Butcher <simonb@alien.net.au>
 */
SMTP::SMTP(Bot *b)
: bot(b),
  address(new SMTPAddress(bot->wantedNickName, 
			  (bot->userName + "@" + bot->userHost)))
{
}


/* ~SMTP - Class destructor
 * Original 05/08/01, Simon Butcher <simonb@alien.net.au>
 */
SMTP::~SMTP(void)
{
   // Kill off stuff we made
   delete address;
}
