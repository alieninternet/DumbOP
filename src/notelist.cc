/* notelist.cc
 * Note list for note server
 */

#include <fstream.h>
#include <stdlib.h>

#include "notelist.h"
#include "stringtokenizer.h" 

/* NoteList - Init the Note List
 * Original 30/12/00, Pickle <pickle@alien.net.au>
 */
NoteList::NoteList(String filename)
  : listFilename(filename)
{
   read();
}

/* ~NoteList - De-init the Note List
 * Original 30/12/00, Pickle <pickle@alien.net.au>
 */
NoteList::~NoteList()
{
   clear();
}

/* read - Read in the Note List file
 * Original 30/12/00, Pickle <pickle@alien.net.au>
 */
void NoteList::read()
{
   ifstream file(listFilename);
   String temp, empty = "";
   int linenum = 1;
   
   clear();
   
   if (!file) {
      cerr << "I cannot file the file " << listFilename << endl;
      return;
   }

   while (file >> temp, temp.length() != 0) {
      StringTokenizer st(temp);
      if (st.countTokens(':') < 5) {
	 cerr << "Error when reading note list file (" << listFilename <<
	   ") line " << linenum << "...\n";
	 return;
      }
      String to = st.nextToken(':');
      String from = st.nextToken(':');
      String sent = st.nextToken(':');
      String flags = st.nextToken(':');
      String message = st.rest().trim();
      l.push_back(new NoteListItem(to, from, atol(sent), atoi(flags),
				   message));
      linenum++;
   }
   file.close();
}

/* save - Save the Note List file
 * Original 30/12/00, Pickle <pickle@alien.net.au>
 */
void NoteList::save()
{
   ofstream file(listFilename);
   
   if (!file)
     return;
   
   for (list<NoteListItem *>::iterator it = l.begin(); it != l.end(); it++)
     file << (*it)->to << ":"
     << (*it)->from << ":"
     << (*it)->sent << ":"
     << (*it)->flags << ":"
     << (*it)->message
     << endl;
}

/* clear - Clear the Note List table
 * Original 30/12/00, Pickle <pickle@alien.net.au>
 */
void NoteList::clear()
{
   NoteListItem *nli;
   
   while (!l.empty()) {
      nli = (*l.begin());
      l.erase(l.begin());
      delete nli;
   }
}

/* addNote - Add a note to the table
 * Original 30/12/00, Pickle <pickle@alien.net.au>
 */
void NoteList::addNote(String t, String f, time_t s, int fl, String m)
{
   l.push_back(new NoteListItem(t, f, s, fl, m));
}
