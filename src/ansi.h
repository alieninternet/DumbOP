/* ansi.h
 * Basic ANSI/VT-102 implementation for telnet sessions
 */

#define BEEP_CHAR     		'\007'		// Beep ^G
#define ESC_CHAR      		'\033'		// ESC (Char 27)

// Basic cursor functions
#define ANSI_SAVE       	"\033[s"	// Save cursor location
#define ANSI_RESTORE    	"\033[U"	// Restore cursor location
#define ANSI_CURLEFT    	"\033[1D"	// Move cursor left
#define ANSI_CURRIGHT   	"\033[1C"	// Move cursor right
#define ANSI_CURUP      	"\033[1A"	// Move cursor up
#define ANSI_CURDOWN    	"\033[1B"	// Move cursor down
#define ANSI_CLRSCR     	"\033[2J"	// Clear entire screen
#define ANSI_CLRLINE    	"\033[K"	// Clear line

// Text mode manipulation
#define ANSI_NORMAL		"\033[0m"	// Normal text
#define ANSI_HILITE		"\033[1m"	// Highlighted (Bold) text
#define ANSI_INVERSE		"\033[7m"	// Inverted text
#define ANSI_BLINK    		"\033[5m"	// Blinking text
#define ANSI_INV_BLINK         	"\033[7;5m"	// Inverted blinking text
#define ANSI_INV_HILITE        	"\033[1;7m"	// Inverted highlighted text
#define ANSI_BLINK_HILITE      	"\033[1;5m"	// Blinking highlighted text
#define ANSI_INV_BLINK_HILITE  	"\033[1;5;7m"	// Inverted blinking highlighted text

// Foreground colours
#define ANSI_BLACK      	"\033[30m"	// Black
#define ANSI_RED        	"\033[31m"	// Red
#define ANSI_GREEN      	"\033[32m"	// Green
#define ANSI_YELLOW     	"\033[33m"	// Yellow
#define ANSI_BLUE       	"\033[34m"	// Blue
#define ANSI_MAGENTA    	"\033[35m"	// Magenta
#define ANSI_CYAN       	"\033[36m"	// Cyan
#define ANSI_WHITE      	"\033[37m"	// White

// Background colours
#define ANSI_BBLACK     	"\033[40m"	// Black
#define ANSI_BRED       	"\033[41m"	// Red
#define ANSI_BGREEN     	"\033[42m"	// Green
#define ANSI_BYELLOW    	"\033[43m"	// Yellow
#define ANSI_BBLUE      	"\033[44m"	// Blue
#define ANSI_BMAGENTA   	"\033[45m"	// Magenta
#define ANSI_BCYAN      	"\033[46m"	// Cyan
#define ANSI_BWHITE     	"\033[47m"	// White

// Line headers (for CLI mode)
#define ANSI_HEAD_GEN  	"\033[0;32m-\033[1m=\033[36m> \033[0m"    // White-Cyan
#define ANSI_HEAD_WARN 	"\033[1;30m-\033[0;31m=\033[1m> \033[0m"  // Red
#define ANSI_HEAD_MAIL 	"\033[1;30m-\033[0;32m=\033[1m> \033[0m"  // Green
#define ANSI_HEAD_MSG  	"\033[1;30m-\033[0;34m=\033[1m> \033[0m"  // Blue
#define ANSI_HEAD_NEWS	"\033[1;30m-\033[0;35m=\033[1m> \033[0m"  // Magenta
#define ANSI_HEAD_BW   	"\033[1;30m-\033[0;37m=\033[1m> \033[0m"  // Bland B&W
#define ANSI_HEAD_CLI	"\033[0;34m> \033[37m"	// Command line header
