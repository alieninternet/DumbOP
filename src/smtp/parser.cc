/* src/smtp/parser.cc
 * SMTP (email) parser routines
 */

#include "config.h"
#include "smtp.h"


// Table of numerics we may received
struct {
   unsigned short numeric;
   void (*function)(unsigned short);
} functions[] = {
     { 211,	0 },			// System status/Help reply
     { 214,	0 },			// Help message reply
     { 220,	0 },			// <domain> service ready
     { 221,	0 },			// <domain> service closing
     { 250,	0 },			// Mail action OK
     { 251,	0 },			// User not local, but mail action ok
     { 252,	0 },			// Cannot VRFY user, but will accept
     { 354,	0 },			// Start mail input
     { 421,	0 },			// <domain> service not available
     { 450,	0 },			// Mail action not taken (eg. busy)
     { 451,	0 },			// Requested action aborted (error)
     { 452,	0 },			// Requested action aborted (resources)
     { 500,	0 },			// Syntax error (command unknown)
     { 501,	0 },			// Syntax error (wrong parameters)
     { 502,	0 },			// Command not implemented
     { 503,	0 },			// Bad sequence of commands
     { 504,	0 },			// Command parameter not implemented
     { 550,	0 },			// Mail action not taken (unavailable)
     { 551, 	0 },			// Mail action not taken (not local)
     { 552,	0 },			// Mail action not taken (resources)
     { 553,	0 },			// Mail action not taken (not allowed)
     { 554,	0 },			// Transaction failed
     { 0, 0 }		
};

