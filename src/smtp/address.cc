/* src/smtp/address.cc
 * SMTP (email) address class
 */

#include "config.h"
#include "smtp.h"


/* SMTPAddress - Create a new address
 * Original 05/08/01, Simon Butcher <simonb@alien.net.au>
 */
SMTPAddress::SMTPAddress(String n, String a)
: name(n),
  address(a)
{
}


/* getAddress - Compile a little address for SMTP use..
 * Original 05/08/01, Simon Butcher <simonb@alien.net.au>
 * Note: Format is "Name" <user@host>
 */
String SMTPAddress::getAddress(void) const
{
   return (String("\"") + name + String("\" <") + address + String(">"));
}

