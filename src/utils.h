#ifndef __UTILS_H_
# define __UTILS_H_

# include <time.h>
# include "string.h"
# include "bot.h"

# define CHR_BOLD	'\002'
# define CHR_INVERSE	'\026'
# define CHR_UNDERLINE	'\037'
# define CHR_COLOUR	'\003'

class Utils {
 public:
   static String getNick(String);
   static String getUserhost(String);
   static String getFirstNick(String);
   static String getKey();
   static String makeWildcard(String);
   
   static bool isChannel(String);
   static bool isWildcard(String);
   static bool isValidChannelName(String);
   static bool isValidNickName(String);
   static bool isIP(String);
   
   static int getLevel(Bot *, String);
   static int getLevel(Bot *, String, String);
   
   static String levelToStr(int);
   static String protToStr(int);
   static String boolToStr(bool);
   static String flagsToStr(long);
   
   static String timelenToStr(time_t);
   static String timeBigToStr(long long);
   
   static time_t strToTime(String);
};

#endif
