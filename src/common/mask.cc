
#include "config.h"

#include <string.h>

#include "mask.h"

Mask::Mask(String s)
  : mask(s)
{ }

String
Mask::getMask()
{
  return mask;
}

bool
Mask::matches(String s)
{
  return match((const char *)mask.toLower(), (const char *)s.toLower());
}

bool
Mask::matches(Mask & m)
{
  return match((const char *)mask.toLower(), (const char *)m.mask.toLower());
}

bool
Mask::match(const char * m, const char * n)
{
  if (!*m) {
    if (!*n)
      return true;
    else
      return false;
  } else
    if (!*n)
      return false;

  if (m[0] == '*') {
    while (*m && (m[0] == '*' || m[0] == '?'))
      m++;
    
    if (*m == '\0')
      return true;
    
    const char *c = n;
    while ((c = strchr(c, *m)) != 0) {
      if (match(m, c))
        return 1;
      c++;
    }
  }

  if ((m[0] == n[0]) ||
      (m[0] == '?') ||
      ((m[0] == '\\') && ((m[1] == '?' && n[0] == '?') ||
                          (m[1] == '*' && n[0] == '*'))))
    return match(++m, ++n);

  return false;
}
