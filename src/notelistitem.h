/* notelistitem.h
 * Single note item
 */

#ifndef __NOTE_H_
# define __NOTE_H_

#include <time.h>

#include "str.h"

// Tricky flagging system which also includes folder allocation! :)
# define NOTE_FUNREAD		0x0001	// Is note new
# define NOTE_FDELETED		0x0002	// Is note considered deleted
# define NOTE_FURGENT		0x0004	// Is note urgent
# define NOTE_FMASS		0x0008	// Was note sent to multiple people
# define NOTE_FFORWARD		0x0010	// Was the note forwarded
# define NOTE_FREPLY		0x0020	// Was the note a reply
# define NOTE_FGLOBAL		0x0040	// Was the note global
# define NOTE_FGRIPE		0x0080	// Note is of a cranky nature :)
# define NOTE_FGEN_SYSTEM	0x0100	// Generated note from the bot
# define NOTE_FGEN_CHANNEL	0x0200	// Generated note about a channel
# define NOTE_FGEN_GAME		0x0400	// Generated note from a game

# define NOTE_FOLDERMASK	0xF000	// 0x1000 to 0xF000 are folder numbers
# define NOTE_MAX_FOLDERS	15	// 0x1..0xF (15) folders allowed

class Note;
class NoteList;

class NoteListItem {
   String to;			// Database link to who the note is to
   String from;			// Database link to who the note was from
   time_t sent;			// Time message was sent
   int flags;			// Message flags
   String message;		// The actual message

 public:
   NoteListItem(String t, String f, time_t s, int fl, String m)
     : to(t) = "", from(f) = "", sent(s) = -1, flags(fl) = 0, message(m) = ""
     {
     }

   friend class Note;
   friend class NoteListItem;
};

#endif
