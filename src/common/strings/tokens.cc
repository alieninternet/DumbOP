/* ./src/strings/tokens.cc
 * String tokenisation routines
 * Copyright (c) 2000-2001 Alien Internet Services
 */

#include "config.h"
#include "strings.h"

/* StringTokens - Initialise new token array
 * Original 15/12/00, Simon Butcher <simonb@alien.net.au>
 */
StringTokens::StringTokens(String string)
: st(string), pos(0)
{
}

/* hasMoreTokens - [Various Forms] Check if the string has more tokens
 * Original 15/12/00, Simon Butcher <simonb@alien.net.au>
 */
bool StringTokens::hasMoreTokens()
{
   if (pos == st.length())
     return false;
   
   for (long i = pos; i < st.length(); i++)
     if (st[i] != ' ' && st[i] != '\t')
       return true;
   
   return false;
}

bool StringTokens::hasMoreTokens(char c)
{
   if (pos == st.length())
     return false;
   
   for (long i = pos; i < st.length(); i++)
     if (st[i] != c)
       return true;
   
   return false;
}

/* countTokens - [Various Forms] Count the number of tokens in the token array
 * Original 15/12/00, Simon Butcher <simonb@alien.net.au>
 */
long StringTokens::countTokens()
{
   long i = 0;
   StringTokens s(st);
   
   while (s.hasMoreTokens()) {
      s.nextToken();
      i++;
   }
   
   return i;
}

long StringTokens::countTokens(char c)
{
   long i = 0;
   StringTokens s(st);
   
   while (s.hasMoreTokens(c)) {
      s.nextToken(c);
      i++;
   }
   
   return i;
}

/* nextToken - [Various Forms] Grab the next token
 * Original 15/12/00, Simon Butcher <simonb@alien.net.au>
 * Notes: WARNING! Do not use multiple times with printf as it seems to
 *        construct strings BACKWARDS, and your order will be in reverse
 */
String StringTokens::nextToken()
{
   long i = pos;
   
   while (i < st.length() && ((st[i] == ' ' || st[i] == '\t') &&
			      (st[i-1] != '\\')))
     i++;
   
   for (long j = i; j < st.length(); j++)
     if ((st[j] == ' ' || st[j] == '\t') &&
	 (st[j-1] != '\\')) {
	pos = j + 1;
	return st.subString(i, j - 1);
     }
   
   pos = st.length();
   return st.subString(i, st.length() - 1);
}

String StringTokens::nextToken(char c, bool empty)
{
   long i = pos;
   
   while (i < st.length() && ((st[i] == c) &&
			      (st[i-1] != '\\')))
     i++;
   for (long j = i; j < st.length(); j++)
     if ((st[j] == c) && (st[j-1] != '\\')) {
	pos = j + 1;
	return st.subString(i, j - 1);
     }
   
   if (empty)
     return "";
   
   pos = st.length();
   return st.subString(i, st.length() - 1);
}

/* rest - Grab the rest of the string ignoring the remaining tokens
 * Original 15/12/00, Simon Butcher <simonb@alien.net.au>
 */
String StringTokens::rest()
{
   if (pos == st.length())
     return "";
   
   while (pos < st.length() && (st[pos] == ' ' || st[pos] == '\t'))
     pos++;
   
   return st.subString(pos, st.length() - 1);
}
