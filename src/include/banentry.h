#ifndef __BANENTRY_H_
# define __BANENTRY_H_

# include <time.h>
# include "mask.h"

class Commands;

class BanEntry {
   Mask banMask;
   time_t expirationDate;
   
 public:
   
   BanEntry(String, time_t);
   
   time_t getExpirationDate() const;
   void setExpirationDate(time_t);
   
   String getMask();
   
   friend class Commands;
};

#endif
