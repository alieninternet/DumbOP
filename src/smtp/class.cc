/* src/smtp/class.cc
 * SMTP (email) main class
 */

#include "config.h"
#include "smtp.h"


/* SMTP - Initialise the SMTP mailer engine
 * Original 01/08/01, Simon Butcher <simonb@alien.net.au>
 */
SMTP::SMTP(Bot *b)
: bot(b)
{
}
