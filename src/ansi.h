/* ansi.h
 * Basic ANSI/VT-102 implementation for telnet sessions
 */

#ifndef __ANSI_H_
# define __ANSI_H_

# include "string.h"

// Basic stuff
# define BEEP_CHAR     		'\007'		// Beep ^G
# define ESC_CHAR      		'\033'		// ESC (Char 27)
# define ANSI_BEGIN		"\033["		// ANSI Escape sequence header

// Basic cursor codes
# define ANSI_CUR_SAVE       	"\033[s"	// Save cursor location
# define ANSI_CUR_RESTORE    	"\033[u"	// Restore cursor location
# define ANSI_CUR_LEFT    	"\033[1D"	// Move cursor left
# define ANSI_CUR_RIGHT   	"\033[1C"	// Move cursor right
# define ANSI_CUR_UP      	"\033[1A"	// Move cursor up
# define ANSI_CUR_DOWN    	"\033[1B"	// Move cursor down
# define ANSI_CUR_HOME		"\033[H"	// Home cursor to 0,0

// Text erasure codes
# define ANSI_CLR_EOL		"\033[K"	// Clear to end of line
# define ANSI_CLR_SOL		"\033[1K"	// Clear to start of line
# define ANSI_CLR_LINE		"\033[2K"	// Clear entire line
# define ANSI_CLR_UP		"\033[1J"	// Clear to top of screen
# define ANSI_CLR_DOWN		"\033[J"	// Clear to bottom of screen
# define ANSI_CLR_SCREEN	"\033[2J"	// Clear entire screen

// Text mode manipulation codes
# define ANSI_NORMAL		"\033[0m"	// Normal text
# define ANSI_BOLD		"\033[1m"	// Bold text
# define ANSI_DIM		"\033[2m"	// Dim text
# define ANSI_UNDERLINE		"\033[4m"	// Underlined text
# define ANSI_BLINK    		"\033[5m"	// Blinking text
# define ANSI_INVERSE		"\033[7m"	// Inverted text
# define ANSI_HIDDEN		"\003[8m"	// Hidden text
# define ANSI_FBOLD		"\033[0;1m"	// Bold text (Fresh)
# define ANSI_FDIM		"\033[0;2m"	// Dim text (Fresh)
# define ANSI_FUNDERLINE	"\033[0;4m"	// Underlined text (Fresh)
# define ANSI_FBLINK   		"\033[0;5m"	// Blinking text (Fresh)
# define ANSI_FINVERSE		"\033[0;7m"	// Inverted text (Fresh)
# define ANSI_FHIDDEN		"\003[0;8m"	// Hidden text (Fresh)

// Foreground colours
# define ANSI_BLACK      	"\033[30m"	// Black
# define ANSI_RED        	"\033[31m"	// Red
# define ANSI_GREEN      	"\033[32m"	// Green
# define ANSI_YELLOW     	"\033[33m"	// Yellow
# define ANSI_BLUE       	"\033[34m"	// Blue
# define ANSI_MAGENTA    	"\033[35m"	// Magenta
# define ANSI_CYAN       	"\033[36m"	// Cyan
# define ANSI_WHITE      	"\033[37m"	// White

// Normal/Foreground Colours
# define ANSI_NBLACK      	"\033[0;30m"	// Black
# define ANSI_NRED        	"\033[0;31m"	// Red
# define ANSI_NGREEN      	"\033[0;32m"	// Green
# define ANSI_NYELLOW     	"\033[0;33m"	// Yellow
# define ANSI_NBLUE       	"\033[0;34m"	// Blue
# define ANSI_NMAGENTA    	"\033[0;35m"	// Magenta
# define ANSI_NCYAN       	"\033[0;36m"	// Cyan
# define ANSI_NWHITE      	"\033[0;37m"	// White

// Bold/Foreground Colours
# define ANSI_HBLACK      	"\033[1;30m"	// Black
# define ANSI_HRED        	"\033[1;31m"	// Red
# define ANSI_HGREEN      	"\033[1;32m"	// Green
# define ANSI_HYELLOW     	"\033[1;33m"	// Yellow
# define ANSI_HBLUE       	"\033[1;34m"	// Blue
# define ANSI_HMAGENTA    	"\033[1;35m"	// Magenta
# define ANSI_HCYAN       	"\033[1;36m"	// Cyan
# define ANSI_HWHITE      	"\033[1;37m"	// White

// Background colours
# define ANSI_BBLACK     	"\033[40m"	// Black
# define ANSI_BRED       	"\033[41m"	// Red
# define ANSI_BGREEN     	"\033[42m"	// Green
# define ANSI_BYELLOW    	"\033[43m"	// Yellow
# define ANSI_BBLUE      	"\033[44m"	// Blue
# define ANSI_BMAGENTA   	"\033[45m"	// Magenta
# define ANSI_BCYAN      	"\033[46m"	// Cyan
# define ANSI_BWHITE     	"\033[47m"	// White

// Request codes
# define ANSI_REQ_DEV_CODE	"\033[c"	// Request Device Code (^[[#;0c)
# define ANSI_REQ_DEV_STATUS	"\033[5n"	// Request Device Status (^[[0n ok, 3n failure)
# define ANSI_REQ_CURPOS	"\033[6n"	// Request Cursor Position (^[[y;xR)

// Terminal setup codes
# define ANSI_TERM_RESET	"\033c"		// Reset terminal
# define ANSI_TERM_LINEWRAP_ON	"\033[7h"	// Enable line-wrapping
# define ANSI_TERM_LINEWRAP_OFF	"\033[7l"	// Disable line-wrapping

// Font codes
# define ANSI_FONT_G0		"\033("		// Set font to BANK 0 (default)
# define ANSI_FONT_G1		"\033)"		// Set font to BANK 1 (alternative)

// Scrolling codes
# define ANSI_SCROLL_SCREEN	"\033[r"	// Enable scrolling for entire screen
# define ANSI_SCROLL_UP		"\033M"		// Scroll up one line
# define ANSI_SCROLL_DOWN	"\033D"		// Scroll down one line

// Tabulation codes
# define ANSI_TAB_SET		"\033H"		// Set a tab at current curloc
# define ANSI_TAB_CLEAR		"\033[g"	// Clear tab at current curloc
# define ANSI_TAB_CLEAR_ALL	"\033[3g"	// Clear all tab marks

// Printing codes
# define ANSI_PRINT_SCREEN	"\033[i"	// Print screen
# define ANSI_PRINT_LINE	"\033[1i"	// Print line
# define ANSI_PRINT_START_LOG	"\033[5i"	// Start logging to printer
# define ANSI_PRINT_STOP_LOG	"\033[4i"	// Stop logging to printer

// Line headers (for CLI mode)
# define ANSI_HEAD_GEN 	"\033[0;32m-\033[1m=\033[36m> \033[0m"    // White-Cyan
# define ANSI_HEAD_WARN	"\033[1;30m-\033[0;31m=\033[1m> \033[0m"  // Red
# define ANSI_HEAD_IRC	"\033[1;30m-\033[0;32m=\033[1m> \033[0m"  // Green
# define ANSI_HEAD_MAIL	"\033[1;30m-\033[0;34m=\033[1m> \033[0m"  // Blue
# define ANSI_HEAD_NEWS	"\033[1;30m-\033[0;35m=\033[1m> \033[0m"  // Magenta
# define ANSI_HEAD_BW  	"\033[1;30m-\033[0;37m=\033[1m> \033[0m"  // Bland B&W
# define ANSI_HEAD_CLI	"\033[0;34m> \033[37m"	// Command line header

class ANSI {
 public:
   static String moveLeft(int);
   static String moveRight(int);
   static String moveUp(int);
   static String moveDown(int);
   static String gotoXY(int, int);

   static String scrollRegion(int, int);
   
   static String telnetHeaderInit();
   static String telnetHeaderUpdate();
};

#endif
