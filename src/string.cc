
#include "string.h"

#include <stdio.h>
#include <string.h>
#include <ctype.h>

String::String()
{
  p = new srep;
  len = 0;
  p->s = 0;
}

String::String(const char *s)
{
  p = new srep;
  len = strlen(s);
  p->s = new char[len + 1];
  strcpy(p->s, s);
}

String::String(const String & s)
{
  s.p->n++;
  p = s.p;
  len = s.len;
}

String::String(int i)
{
   char temp[16];
   sprintf(temp, "%d", i);
   
   p = new srep;
   len = strlen(temp);
   p->s = new char[len + 1];
   strcpy(p->s, temp);
}

String::String(unsigned int i)
{
   char temp[16];
   sprintf(temp, "%d", i);
   
   p = new srep;
   len = strlen(temp);
   p->s = new char[len + 1];
   strcpy(p->s, temp);
}

String::String(long i)
{
  char temp[32];
  sprintf(temp, "%ld", i);

  p = new srep;
  len = strlen(temp);
  p->s = new char[len + 1];
  strcpy(p->s, temp);
}

String::String(unsigned long i)
{
  char temp[32];
  sprintf(temp, "%ld", i);

  p = new srep;
  len = strlen(temp);
  p->s = new char[len + 1];
  strcpy(p->s, temp);
}

String::String(char c)
{
  p = new srep;
  p->s = new char[2];
  p->s[0] = c;
  p->s[1] = '\0';
  len = 1;
}

String::~String()
{
  if (--p->n == 0) {
    delete[] p->s;
    delete p;
  }
}

String &
String::operator=(const char *s)
{
  if (p->n > 1) {
    p->n--;
    p = new srep;
  }
  else
    delete[] p->s;
  
  len = strlen(s);
  p->s = new char[len + 1];
  strcpy(p->s, s);
  
  return *this;
}

String &
String::operator=(const String & s)
{
  s.p->n++; // protection against st = st
  if (--p->n == 0) {
    delete[] p->s;
    delete p;
  }
  p = s.p;
  len = s.len;
  return *this;
}

int
String::length() const
{
  return len;
}

int
String::find(char c)
{
  if (!strchr(p->s, c))
    return -1;
  
  return (int)(strchr(p->s, c) - p->s);
}

void
String::fill(char c)
{
  for (char * s = p->s; *s; s++)
    *s = c;
}

String
String::pad(int n)
{
  int l = len;

  if (n <= l)
    return subString(0, n-1);

  char *temp = new char[n+1];
  strcpy(temp, p->s);

  for (int i = l; i < n; i++)
    temp[i] = ' ';
  temp[n] = '\0';

  String res(temp);
  delete temp;

  return res;
}

/* prepad - Make string exactly n by cropping or adding spaces to the start
 *  * Original 15/12/00, Simon Butcher <simonb@alien.net.au>
 *  */
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

String
String::subString(int debut, int fin)
{
  if (fin < debut) return "";

  char * temp = new char[fin-debut+2];
  strncpy(temp, p->s + debut, fin - debut + 1);
  temp[fin - debut + 1] = '\0';
  String res(temp);
  delete temp;
  return res;
}

String
String::subString(int debut)
{
  return subString(debut, len - 1);
}

String
String::toLower()
{
  char *temp = new char[len + 1];

  for (int i = 0; i < len + 1; i++)
    if (isupper(p->s[i]))
      temp[i] = tolower(p->s[i]);
    else
      temp[i] = p->s[i];

  String res(temp);
  delete temp;
  return res;
}

String
String::toUpper()
{
  char *temp = new char[len + 1];

  for (int i = 0; i < len + 1; i++)
    if (islower(p->s[i]))
      temp[i] = toupper(p->s[i]);
    else
      temp[i] = p->s[i];

  String res(temp);
  delete temp;
  return res;
}

String
String::trim()
{
  int i = 0, j = len - 1;

  while (i < j && (p->s[i] == ' ' || p->s[i] == '\t' || p->s[i] == '\r'))
    i++;
  
  while (j > 0 && (p->s[j] == ' ' || p->s[j] == '\t' || p->s[i] == '\r'))
    j--;
  
  return subString(i, j);
}

int
String::indexOf(char c)
{
  char *s = strchr(p->s, c);
  if (s)
    return p->s - s;
  
  return -1;
}

char &
String::operator[](int i)
{
  if (i < 0 || len < i) {
    cerr << "String index out of range\n";
    exit(1);
  }
  return p->s[i];
}

const char &
String::operator[](int i) const
{
  if (i < 0 || len < i) {
    cerr << "String index out of range\n";
    exit(1);
  }
  return p->s[i];
}

bool
String::operator==(const char *s) const
{
  return strcmp(p->s, s) == 0;
}

bool
String::operator==(const String & s) const
{
  return (p == s.p) || (strcmp(p->s, s.p->s) == 0);
}

bool
String::operator!=(const char *s) const
{
  return strcmp(p->s, s) != 0;
}

bool
String::operator!=(const String & s) const
{
  return strcmp(p->s, s.p->s) != 0;
}

bool
String::operator<(const String & s) const
{
  return strcmp(p->s, s.p->s) < 0;
}

bool
String::operator>(const String & s) const
{
  return strcmp(p->s, s.p->s) > 0;
}

bool
String::operator<=(const String & s) const
{
  return strcmp(p->s, s.p->s) <= 0;
}

bool
String::operator>=(const String & s) const
{
  return strcmp(p->s, s.p->s) >= 0;
}

String
String::operator+(const char *s)
{
  char *temp = new char[len + strlen(s) + 1];

  strcpy(temp, p->s);
  strcat(temp, s);

  String res(temp);
  delete temp;

  return res;
}

String
String::operator+(const String & s)
{
  char * temp = new char[len + s.len + 1];

  strcpy(temp, p->s);
  strcat(temp, s.p->s);

  String res(temp);
  delete temp;

  return res;
}

String::operator const char *() const
{
  return p->s;
}

ostream &
operator<<(ostream & s, const String & st)
{
  return s << st.p->s;
}

istream &
operator>>(istream & s, String & st)
{
  if (st.p->n > 1) {
    st.p->n--;
    st.p = new String::srep;
  }
  else
    delete[] st.p->s;
  
  char buf[2048];
  char c;

  s.get(buf, 2048, '\n');
  s.get(c);

  st.len = strlen(buf);
  st.p->s = new char[st.len + 1];
  strcpy(st.p->s, buf);

  return s;
}

