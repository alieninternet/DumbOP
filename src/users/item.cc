
#include "userlistitem.h"
#include "user.h"
#include "flags.h"

UserListItem::UserListItem(String m, time_t r, int l, unsigned long c, 
			   String n = "", long f = 0, time_t ls = -1, 
			   String pass = "")
: mask(m), 
  registered(r),
  level(l),
  credits(c),
  nick(n),
  flags(f),
  lastseen(ls),
  passwd(pass),
  identified(0) 
{ 
}


// Returns true if it matches <m>
bool UserListItem::matches(String m)
{ 
   return mask.matches(m); 
}


/* canAfford - Can this user afford to pay for something?
 * Original 30/07/01, Simon Butcher <simonb@alien.net.au>
 */
bool UserListItem::canAfford(unsigned long amount)
{
   // If the user is a manager or better, they have endless credit
   if ((identified) && 
       ((level >= User::MANAGER) ||
	(flags & USERFLAG_HAS_MONEY_TREE) ||
	((credits - amount) >= 0))) {
      return true;
   }
   
   return false;
}


/* charge - Charge the user an amount of credits (blind)
 * Original 30/07/01, Simon Butcher <simonb@alien.net.au>
 */
void UserListItem::charge(unsigned long amount)
{
   // If the user is not a manager, we can do this)
   if ((level < User::MANAGER) && 
       !(flags & USERFLAG_HAS_MONEY_TREE)) {
      credits -= amount;
   }
}


/* pay - Give the user an amount of credits
 * Original 30/07/01, Simon Butcher <simonb@alien.net.au>
 */
void UserListItem::pay(unsigned long amount)
{
   // If the user is not a manager, we can do this)
   if ((level < User::MANAGER) && 
       !(flags & USERFLAG_HAS_MONEY_TREE)) {
      credits += amount;
   }
}
