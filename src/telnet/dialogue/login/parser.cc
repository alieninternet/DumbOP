/* parser.cc
 * Login dialogue box, parsing routines
 */

#include "config.h"
#include "login.h"

#include "telnetdialogue.h"
#include "ansi.h"
#include "utils.h"
#include "version.h"
#include "userlist.h"
#include "user.h"


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
	    // Check if the password is good
	    if (desc->telnet->bot->userList->identify(nickname, buff, false)) {
	       // Grab that user list entry
	       UserListItem *uli =
		 desc->telnet->bot->userList->getUserListItemNick(nickname);
	       
	       /* Double check (yes double, since identify did it) we got
		* that userlist entry, and make sure this user has access
		*/
	       if ((uli) && (uli->level >= User::MANAGER)) {
		  // Pass onto next dialogue
		  desc->user = uli;
		  desc->nextDialogue = new TelnetDialogueMonitor(desc);
		  
		  // Break the loop
		  return;
	       }
	    } 
	    
	    // The password failed. Increase the counter and reset variables
	    attempts++;
	       
	    // Check if this was the last attempt allowed
	    if (attempts >= TELNET_LOGIN_MAX_ATTEMPTS) {
	       // Say bye bye to the connection
	       desc->goodbyeSocket();
	       return;
	    }
	    
	    // Reset variables
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
