
#include "stringtokenizer.h"

StringTokenizer::StringTokenizer(String string)
  : st(string), pos(0)
{ }

bool
StringTokenizer::hasMoreTokens()
{
  if (pos == st.length())
    return false;

  for (int i = pos; i < st.length(); i++)
    if (st[i] != ' ' && st[i] != '\t')
      return true;

  return false;
}

bool
StringTokenizer::hasMoreTokens(char c)
{
  if (pos == st.length())
    return false;

  for (int i = pos; i < st.length(); i++)
    if (st[i] != c)
      return true;

  return false;
}

int
StringTokenizer::countTokens()
{
  int i = 0;
  StringTokenizer s(st);
  
  while (s.hasMoreTokens()) {
    s.nextToken();
    i++;
  }

  return i;
}

int
StringTokenizer::countTokens(char c)
{
  int i = 0;
  StringTokenizer s(st);
  
  while (s.hasMoreTokens(c)) {
    s.nextToken(c);
    i++;
  }

  return i;
}

String
StringTokenizer::nextToken()
{
  int i = pos;
  
  while (i < st.length() && (st[i] == ' ' || st[i] == '\t'))
    i++;
  
  for (int j = i; j < st.length(); j++)
    if (st[j] == ' ' || st[j] == '\t') {
      pos = j + 1;
      return st.subString(i, j - 1);
    }

  pos = st.length();
  return st.subString(i, st.length() - 1);
}

String
StringTokenizer::nextToken(char c, bool empty)
{
  int i = pos;

  while (i < st.length() && (st[i] == c))
    i++;
  for (int j = i; j < st.length(); j++)
    if (st[j] == c) {
      pos = j + 1;
      return st.subString(i, j - 1);
    }

  if (empty)
    return "";

  pos = st.length();
  return st.subString(i, st.length() - 1);
}

String
StringTokenizer::rest()
{
  if (pos == st.length())
    return "";

  while (pos < st.length() && (st[pos] == ' ' || st[pos] == '\t'))
    pos++;

  return st.subString(pos, st.length() - 1);
}
