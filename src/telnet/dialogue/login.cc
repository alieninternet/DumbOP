/* login.cc
 * Login dialogue box
 */

#include "telnetdialogue.h"
#include "ansi.h"
#include "utils.h"
#include "version.h"

/* Dialogue box width and height, and other box controls.
 * Do not change unless you know what you are doing! :)
 */
#define DIALOGUE_LOGIN_FIRST_FIELD_LOC	1
#define DIALOGUE_LOGIN_FIELD_LENGTH	25 // should be max password length
#define DIALOGUE_LOGIN_WIDTH		(DIALOGUE_LOGIN_FIELD_LENGTH + 14)
#define DIALOGUE_LOGIN_HEIGHT		5

// This is a table of the three fields we have. Cheap way of saving memory.
const char *fields[] = {
   "Network:", // Note the +1 char offset.
   "Nickname:",
   "Password:"
};


/* TelnetDialogueLogin - Login sub-class constructor
 * Original 28/07/01, Simon Butcher <simonb@alien.net.au>
 */
TelnetDialogueLogin::TelnetDialogueLogin(TelnetDescriptor *d)
: TelnetDialogue(d)
{
   // Reset some variables
   field = NETWORK;
   network = nickname = buff = "";
   attempts = 0;

   // We should be in non-masking teletype mode.
   desc->flags |= TELNETFLAG_TELETYPE | TELNETFLAG_TELETYPE_NO_BS;
   desc->flags &= ~TELNETFLAG_TELETYPE_MASK;
}


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


/* parseInput - Parse an incoming char
 * Original 28/07/01, Simon Butcher <simonb@alien.net.au>
 */
void TelnetDialogueLogin::parseInput(char chr)
{
   switch (chr) {
    case '\n': // New line, well, to us it means next field.
      // Make sure we HAVE something.
      if (buff.length()) {
	 // What are we switching?
	 switch (field) {
	  case NETWORK:
	    // Dim the entry and jump to the next line
	    desc->write(ANSI::gotoXY(boxX + 3, // Note the +1 char offset.
				     (boxY + DIALOGUE_LOGIN_FIRST_FIELD_LOC)) +
			String(ANSI_NCYAN) + fields[0] + 
			String(ANSI_NWHITE) + String(" ") + buff +
			ANSI::gotoXY(boxX + 2,
				     (boxY + 
				      DIALOGUE_LOGIN_FIRST_FIELD_LOC + 1)) +
			String(ANSI_HCYAN) + fields[1] + 
			String(ANSI_HWHITE) + String(" "));

	    // Do the swap
	    network = buff;
	    field = NICKNAME;
	    break;
	  case NICKNAME:
	    // Dim the entry and jump to the next line
	    desc->write(ANSI::gotoXY(boxX + 2,
				     (boxY + 
				      DIALOGUE_LOGIN_FIRST_FIELD_LOC + 1)) +
			String(ANSI_NCYAN) + fields[1] +
			String(ANSI_NWHITE) + String(" ") + buff +
			ANSI::gotoXY(boxX + 2,
				     (boxY + 
				      DIALOGUE_LOGIN_FIRST_FIELD_LOC + 2)) +
			String(ANSI_HCYAN) + fields[2] +
			String(ANSI_HWHITE) + String(" "));
	    
	    // Do the swap
	    nickname = buff;
	    field = PASSWORD;
	    
	    // The current field is the password. We should mask the input
	    desc->flags |= TELNETFLAG_TELETYPE_MASK;
	    break;
	  case PASSWORD:
	    if (true) {
	       // The password failed. Increase the counter and reset variables
	       attempts++;
	       field = NETWORK;
	       network = nickname = "";
	       
	       /* Dim this field, clear the entries, and return to the top.
		* This seems quite ugly, unfortunately
		*/
	       desc->write(ANSI::gotoXY(boxX + 2,
					(boxY + 
					 DIALOGUE_LOGIN_FIRST_FIELD_LOC + 2)) +
			   String(ANSI_NCYAN) + fields[2] + String(" ") +
			   Utils::repeatChar(DIALOGUE_LOGIN_FIELD_LENGTH) +
			   ANSI::gotoXY(boxX + 12, // = 2 + 8 + 2
					(boxY + 
					 DIALOGUE_LOGIN_FIRST_FIELD_LOC)) +
			   Utils::repeatChar(DIALOGUE_LOGIN_FIELD_LENGTH) +
			   ANSI::gotoXY(boxX + 12, // = 2 + 8 + 2
					(boxY +
					 DIALOGUE_LOGIN_FIRST_FIELD_LOC + 1)) +
			   Utils::repeatChar(DIALOGUE_LOGIN_FIELD_LENGTH) +
			   ANSI::gotoXY(boxX + 3, // Note the +1 char offset.
					(boxY + 
					 DIALOGUE_LOGIN_FIRST_FIELD_LOC)) +
			   String(ANSI_HCYAN) + fields[0] +
			   String(ANSI_HWHITE) + String(" "));
	       
	       // Turn off masking
	       desc->flags &= ~TELNETFLAG_TELETYPE_MASK;
	    }
	    break;
	 }
	 
	 // Clean up buff for the next field, assert teletype w/ no backspaces
	 buff = "";
	 desc->flags |= TELNETFLAG_TELETYPE | TELNETFLAG_TELETYPE_NO_BS;
      } else {
	 // Beep. This user is dumb.
	 desc->write('\a');
      }

      break;
    case '\b': // A backspace - the main parser will have delt with it.
      // Make sure we actually have a char to remove, else this is dumb.
      if (buff.length()) {
	 // Trim that last char
	 buff = buff.subString(0, (buff.length() - 2));
	 
	 // Have we shortened the string enough?
	 if (buff.length() == (DIALOGUE_LOGIN_FIELD_LENGTH - 1)) {
	    // Turn teletype mode back on
	    desc->flags |= TELNETFLAG_TELETYPE;
	    
	    /* We need to compensate for the backspace that would not have
	     * been sent since TELETYPE mode was off
	     */
	    desc->write(String(TELNET_BACKSPACE));
	 }
	 
	 // Is the length so small, now, that the string is nothing?
	 if (!buff.length()) {
	    // Stop backspaces from working.
	    desc->flags |= TELNETFLAG_TELETYPE_NO_BS;
	 }
      } else {
	 // Beep, you cannot backspace here, duh.
	 desc->write('\a');
      }
      break;
    default: // Must be text. The main parser blocks any nasties.
      // Add this char to the line if we have not broken any limits
      if ((field == PASSWORD) || 
	  (buff.length() < DIALOGUE_LOGIN_FIELD_LENGTH)) {
	 // Are we starting from the start of this line?
	 if (!buff.length()) {
	    // Turn on backspaces
	    desc->flags &= ~TELNETFLAG_TELETYPE_NO_BS;
	 }
	 
	 // Copy that char in..
	 buff = buff + String(chr);
      } else {
	 // Send a lil beep!
	 desc->write('\a');
      }
      
      // Have we bumped into the end of a realistic field length?
      if (buff.length() == DIALOGUE_LOGIN_FIELD_LENGTH) {
	 // Turn teletype mode off
	 desc->flags &= ~TELNETFLAG_TELETYPE;
      }
   }
}
