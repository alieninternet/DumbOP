
#include "banentry.h"

BanEntry::BanEntry(String m, time_t e)
: banMask(m), expirationDate(e)
{ }

time_t
BanEntry::getExpirationDate() const
{
   return expirationDate;
}

void
BanEntry::setExpirationDate(time_t exp)
{
   expirationDate = exp;
}

String
BanEntry::getMask()
{
   return banMask.getMask();
}
