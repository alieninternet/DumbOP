/* src/snmp/class.cc
 * SNMP (network management) main class
 */

#include "config.h"
#include "snmp.h"


/* SNMP - Initialise the SNMP node
 * Original 01/08/01, Simon Butcher <simonb@alien.net.au>
 */
SNMP::SNMP(Bot *b)
: bot(b)
{
}
