/* draw.cc
 * Login dialogue box, page drawing routines
 */

#include "config.h"
#include "login.h"

#include "telnetdialogue.h"
#include "ansi.h"
#include "utils.h"
#include "version.h"


/* drawPage - Draw the Login dialogue
 * Original 28/07/01, Simon Butcher <simonb@alien.net.au>
 */
void TelnetDialogueLogin::drawPage(void)
{
   /* Calculate the box location.
    * We have to do this here just in case the terminal is resized.
    */
   calcBoxTLHC(DIALOGUE_LOGIN_WIDTH, DIALOGUE_LOGIN_HEIGHT);

   // Make up the version string. We banner this on the bottom line.
   String version = (Version::getVersion() + String (" - ") +
		     Version::getCopyright());
   
   // Wipe our portion of the screen clean..
   clearWindow();
   
   /* Write the stuff. TAKE THIS MR. COMPILER!! Notice how we do not
    * check for a password output. It is the last field, if there is anything
    * there it will be 'current' text.
    */
   desc->write(ANSI::gotoXY((desc->columns - version.length()), desc->rows) +
	       String(ANSI_FDIM) + version + String(ANSI_FBOLD) +
	       ANSI::drawBox(boxX, boxY, (boxX + DIALOGUE_LOGIN_WIDTH),
			     (boxY + DIALOGUE_LOGIN_HEIGHT)) +
	       String(ANSI_NCYAN) + 
	       ANSI::gotoXY(boxX + 3, // Note the +1 char offset.
			    (boxY + DIALOGUE_LOGIN_FIRST_FIELD_LOC)) +
	       String(fields[0]) + (network.length() ?
				    (String(ANSI_NWHITE) + String(" ") +
				     network + String(ANSI_NCYAN)) :
				    String("")) +
	       ANSI::gotoXY(boxX + 2,
			    (boxY + DIALOGUE_LOGIN_FIRST_FIELD_LOC + 1)) +
	       String(fields[1]) + (nickname.length() ?
				    (String(ANSI_NWHITE) + String(" ") +
				     nickname + String(ANSI_NCYAN)) :
				    String("")) +
   	       ANSI::gotoXY(boxX + 2, 
			    (boxY + DIALOGUE_LOGIN_FIRST_FIELD_LOC + 2)) + 
	       String(fields[2]));
   
   /* Highlight the appropriate field and sit the cursor next to it.
    * It is surprising how more 'optomised' is it to do it this way,
    * otherwise we would have to waste CPU on this one issue, or send
    * an ANSI text attribute code per field. This logically to me seems
    * the easiest and 'cheapest' method, dispite it looking a little
    * crappy on slow terms. Hell, who has a slow terminal these days? :)
    */
   desc->write(String(ANSI_HCYAN));
   
   // Find the one we are highlighting.
   switch (field) {
    case NETWORK:
    default:
      desc->write(ANSI::gotoXY(boxX + 3, // Note the +1 char offset.
			       (boxY + DIALOGUE_LOGIN_FIRST_FIELD_LOC)) +
		  String(fields[0]) + String(ANSI_HWHITE) + String(" ") +
		  buff);
      break;
    case NICKNAME:
      desc->write(ANSI::gotoXY(boxX + 2,
			       (boxY + DIALOGUE_LOGIN_FIRST_FIELD_LOC + 1)) +
		  String(fields[1]) + String(ANSI_HWHITE) + String(" ") +
		  buff);
      break;
    case PASSWORD:
      desc->write(ANSI::gotoXY(boxX + 2, 
			       (boxY + DIALOGUE_LOGIN_FIRST_FIELD_LOC + 2)) + 
		  String(fields[2]) + String(ANSI_HWHITE) + String(" ") +
		  Utils::repeatChar((buff.length() - 1), 
				    TELNET_TELETYPE_MASK_CHAR));
      break;
   }
}

