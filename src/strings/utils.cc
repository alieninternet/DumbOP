/* ./src/strings/utils.cc
 * String utility functions
 * Copyright (c) 2000-2001 Alien Internet Services
 */

#include "strings.h"

#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>

/* length - Return length of a string
 * Original 15/12/00, Simon Butcher <simonb@alien.net.au>
 */
int String::length() const
{
   return len;
}

/* find - Find a character in a string
 * Original 15/12/00, Simon Butcher <simonb@alien.net.au>
 */
int String::find(char c)
{
   if (!strchr(p->s, c))
     return -1;
   
   return (long)(strchr(p->s, c) - p->s);
}

/* fill - Fill a string with a character
 * Original 15/12/00, Simon Butcher <simonb@alien.net.au>
 */
void String::fill(char c)
{
   for (char * s = p->s; *s; s++)
     *s = c;
}

/* pad - Make string exactly n by cropping or adding spaces to the end
 * Original 15/12/00, Simon Butcher <simonb@alien.net.au>
 */
String String::pad(long n)
{
   long l = len;
   
   if (n <= l)
     return subString(0, n-1);
   
   char *temp = new char[n+1];
   strcpy(temp, p->s);
   
   for (long i = l; i < n; i++)
     temp[i] = ' ';
   temp[n] = '\0';
   
   String res(temp);
   delete temp;
   
   return res;
}

/* prepad - Make string exactly n by cropping or adding spaces to the start
 * Original 15/12/00, Simon Butcher <simonb@alien.net.au>
 */
String String::prepad(long n, char c = ' ')
{
   char *temp = new char[n+1];
   strcpy(temp, p->s);
   String out(temp);
   delete temp;

   while (out.length() < n)
     out = String((char)c) + out;

   return out;
}

/* subString - [Various Forms] Return a cut section of a string
 * Original 15/12/00, Simon Butcher <simonb@alien.net.au>
 */
String String::subString(long start, long end)
{
   if (end < start) return "";
   
   char * temp = new char[end-start+2];
   
   strncpy(temp, p->s + start, end - start + 1);
   
   temp[end - start + 1] = '\0';
   String res(temp);
   delete temp;
   
   return res;
}

String String::subString(long start)
{
   return subString(start, len - 1);
}

/* getFirst - Mini token grabber - Grab one section
 * Original 16/1/01, Simon Butcher <simonb@alien.net.au>
 */
String String::getFirst(void)
{
   bool in_quote = false;
   bool in_inverted_comma = false;
   
   for (long i = 0; i < len; i++)
      switch (p->s[i]) {
       case '"':
	 in_quote = !in_quote;
	 break;
       case '\'':
	 in_inverted_comma = !in_inverted_comma;
	 break;
       case ' ':
	 if (!in_quote && !in_inverted_comma)
	   return subString(0, --i);
	 break;
      }
   
   return *this;
}

/* toLower - Convert an entire string to lower case
 * Original 15/12/00, Simon Butcher <simonb@alien.net.au>
 */
String String::toLower()
{
   char *temp = new char[len + 1];
   
   for (long i = 0; i < len + 1; i++) {
      if (isupper(p->s[i]))
	temp[i] = tolower(p->s[i]);
      else
	temp[i] = p->s[i];
   }
   
   String res(temp);
   delete temp;
   return res;
}

/* toUpper - Convert an entire string to upper case
 * Original 15/12/00, Simon Butcher <simonb@alien.net.au>
 */
String String::toUpper()
{
   char *temp = new char[len + 1];
   
   for (long i = 0; i < len + 1; i++) {
      if (islower(p->s[i]))
	temp[i] = toupper(p->s[i]);
      else
	temp[i] = p->s[i];
   }
   
   String res(temp);
   delete temp;
   return res;
}

/* toInt - Convert a string to an integer
 * Original 10/1/01, Simon Butcher <simonb@alien.net.au>
 */
int String::toInt(void)
{
   return atoi(p->s);
}

/* toLong - Convert a string to a long integer
 * Original 10/1/01, Simon Butcher <simonb@alien.net.au>
 */
long String::toLong(void)
{
   return atol(p->s);
}

/* toDouble - Convert a string to a double floating point number
 * Original 10/1/01, Simon Butcher <simonb@alien.net.au>
 */
double String::toDouble(void)
{
   return atof(p->s);
}

/* trim - Crop a string
 * Original 15/12/00, Simon Butcher <simonb@alien.net.au>
 */
String String::trim()
{
   long i = 0;
   long j = len - 1;
   
   while (i < j && (p->s[i] == ' ' || p->s[i] == '\t' || p->s[i] == '\r'))
     i++;
   
   while (j > 0 && (p->s[j] == ' ' || p->s[j] == '\t' || p->s[i] == '\r'))
     j--;
   
   return subString(i, j);
}

/* indexOf - Find the index of a character
 * Original 15/12/00, Simon Butcher <simonb@alien.net.au>
 */
int String::indexOf(char c)
{
   char *s = strchr(p->s, c);
   if (s)
     return p->s - s;
   
   return -1;
}

/* safetyEscape - Escape a char checking for things that may become an issue
 * Original 22/1/01, Simon Butcher <simonb@alien.net.au>
 */
String String::safetyEscape()
{
   String out = "";
   
   for (long i = 0; i < len; i++)
     switch (p->s[i]) {
      case '\\':
	out = out + String("\\\\");
	break;
      case '\0': // NULL
	printf(" ** NULL ** ");
	out = out + String("\\0");
	break;
      case '\r': // Carrage return
	out = out + String("\\r");
	break;
      case '\n': // Line feed
	out = out + String("\\n");
	break;
      default:
	out = out + String(p->s[i]);
     }
   
   return out;
}

/* safetyUnescape - Convert a safety-escaped string back to its original form
 * Original 22/1/01, Simon Butcher <simonb@alien.net.au>
 */
String String::safetyUnescape()
{
   String out = "";
   
   for (long i = 0; i < len; i++)
     if (p->s[i] == '\\') { // Escape found
	i++;
	switch (p->s[i]) {
	 case '\\':
	   out = out + String('\\');
	   break;
	 case '0':
	   out = out + String('\0');
	 case 'r':
	   out = out + String('\r');
	   break;
	 case 'n':
	   out = out + String('\n');
	   break;
	 default: // If this is reached it is probably not even escaped properly
	   out = out + String('\\') +
	     String(p->s[i]); // Do whatever it was in the string
	}
     } else {
	out = out + String(p->s[i]);
     }
       

   return out;
}

