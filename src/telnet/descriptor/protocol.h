/* src/telnet/descriptor/protocol.h
 * Telnet descriptor (connection) telnet protocol handlers
 * Copyright (c) 2001 Alien Internet Services
 */


/* Telnet Control Codes
 * 
 * Comments in the format of:
 * <decimal char number> <layer> <name> [<description>] (<rfc ref>)
 * Where layer is the number of the set of commands, 0 as an usher, and
 * > as a shift to the next layer.
 * 
 * Most of these commands are not implemented, but they are here because
 * the telnet commands are listed in the most annoying way, and this list
 * represents a concise list of codes at the time of writing (3/2/01), 
 * however be aware that some (or all) of the older (pre-1980) command options
 * are not listed.
 */
#define TC_SUBLIMINAL_MESSAGE	'\379'	// 257 1 Subliminal Message [!] (rfc1097)
#define TC_RANDOMLY_LOSE	'\378'	// 256 1 Randomly-Lose [!] (rfc748)
#define TC_IAC			'\377'	// 255 0 Interpret as Command (rfc854)
#define TC_EXOPL		'\377'	// 255 > Extended Options List (rfc861)
#define TC_DONT			'\376'	// 254 1 Do not (rfc854)
#define TC_DO			'\375'	// 253 1 Do (rfc854)
#define TC_WONT			'\374'	// 252 1 Will not (rfc854)
#define TC_WILL			'\373'	// 251 1 Will (rfc854)
#define TC_SB			'\372'	// 250 1 Start subnegotiation (rfc855)
#define TC_GA			'\371'	// 249 1 Go ahead (rfc854)
#define TC_EL			'\370'	// 248 1 Erase line (rfc854)
#define TC_EC			'\367'	// 247 1 Erase character (rfc854)
#define TC_AYT			'\366'	// 246 1 Are you there (rfc854)
#define TC_AO			'\365'	// 245 1 Abort output (rfc854)
#define TC_IP			'\364'	// 244 1 Interrupt Process (rfc854)
#define TC_BRK			'\363'	// 243 1 Break (rfc854)
#define TC_DM			'\362'	// 242 1 Data mark [Synch] (rfc854)
#define TC_NOP			'\361'	// 241 1 No-operation (rfc854)
#define TC_SE			'\360'	// 240 1 End of subnegotiation (rfc855)
#define TC_ABORT		'\356'	// 238 1 Abort (rfc1116)
#define TC_SUSP			'\355'	// 237 1 Suspend (rfc1116)
#define TC_EOF			'\354'	// 236 1 EOF (rfc1116)
#define TC_FORWARD_X		'\061'	// 049 1 Forward X (rfc2840)
#define TC_SEND_URL		'\060'	// 048 1 Send URL (Independant: David Croft)
#define TC_KERMIT		'\057'	// 047 1 Kermit server (rfc2840)
#define TC_START_TLS		'\056'	// 046 1 Start TLS (Independant: Michael Boe)
#define TC_SUPRESS_LOCAL_ECHO	'\055'	// 045 1 Suppress Local Echo (Independant: Jeffrey Altman)
#define TC_COM_PORT_OPTION	'\054'	// 044 1 Com-Port Functionality (rfc2217)
#define TC_RSP			'\053'	// 043 1 Remote Serial Port (Independant: Robert Barnes)
#define TC_CHARSET		'\052'	// 042 1 Character Set (rfc2066)
#define TC_XAUTH		'\051'	// 041 1 X Authentication (Independant: Rob Earhart)
#define TC_TN3270E		'\050'	// 040 1 TN3270E (rfc1647)
#define TC_NEW_ENVIRON		'\047'	// 039 1 (New) Environment (rfc1572)
#define TC_ENCRYPT		'\046'	// 038 1 Encryption (rfc2946)
#define TC_AUTHENTICATION	'\045'	// 037 1 Authentication (rfc1409)
#define TC_ENVIRON		'\044'	// 036 1 Environment (rfc1571,1408)
#define TC_XDISPLOC		'\043'	// 035 1 X Display Location (rfc1096)
#define TC_LINEMODE		'\042'	// 034 1 Line mode (rfc1116)
#define TC_TOGGLE_FLOW_CONTROL	'\041'	// 033 1 Toggle Flow Control (rfc1372,1080)
#define TC_TERMINAL_SPEED	'\040'	// 032 1 Terminal Speed (rfc1079)
#define TC_NAWS			'\037'	// 031 1 Negotiate about window size (rfc1073)
#define TC_X3_PAD		'\036'	// 030 1 X.3 Padding (rfc1053)
#define TC_3270_REGIME		'\035'	// 029 1 TN3270 Support (rfc1041)
#define TC_TTYLOC		'\034'	// 028 1 Terminal Location [address] (rfc946)
#define TC_OUTMRK		'\033'	// 027 1 Output Marking [Indepandant Banner] (rfc933)
#define TC_TUID			'\032'	// 026 1 TACACS User ID (rfc927)
#define TC_EOR			'\031'	// 025 1 End of Record (rfc885)
#define TC_TERMINAL_TYPE	'\030'	// 024 1 Terminal type (rfc1091,930,884)
#define TC_SEND_LOCATION	'\027'	// 023 1 Send Location (rfc779)
#define TC_SUPDUP_OUTPUT	'\026'	// 022 1 SUPDUP Output (rfc749)
#define TC_SUPDUP		'\025'	// 021 1 SUPDUP [display protocol] (rfc736)
#define TC_DET			'\024'	// 020 1 Data-Entry Terminal (rfc1043,732,731)
#define TC_BM			'\023'	// 019 1 Byte Macro (rfc735,729)
#define TC_LOGOUT		'\022'	// 018 1 Logout (rfc727)
#define TC_EXTEND_ASCII		'\021'	// 017 1 Extended ASCII (rfc698)
#define TC_NAOLFD		'\020'	// 016 1 Output line-feed disposition (rfc658)
#define TC_NAOVTD		'\017'	// 015 1 Output vertical tab disposition (rfc657)
#define TC_NAOVTS		'\016'	// 014 1 Output vertical tab-stops (rfc656)
#define TC_NAOFFD		'\015'	// 013 1 Output form-feed disposition (rfc655)
#define TC_NAOHTD		'\014'	// 012 1 Output horizontal tab disposition (rfc654)
#define TC_NAOHTS		'\013'	// 011 1 Output horizontal tab-stops (rfc653)
#define TC_NAOCRD		'\012'	// 010 1 Output carriage-return disposition (rfc652)
#define TC_OUTPUT_PAGE_SIZE	'\009'	// 009 1 Output page size
#define TC_OUTPUT_LINE_WIDTH	'\008'	// 008 1 Output line width
#define TC_RCTE			'\007'	// 007 1 Remote Controlled Transmission and Echoing (rfc726)
#define TC_TIMING_MARK		'\006'	// 006 1 Timing mark (rfc860,560)
#define TC_STATUS		'\005'	// 005 1 Status (rfc859,651)
#define TC_MSG_SIZE		'\004'	// 004 1 Approx. Message size for negotiation (DEC AA-K759B-TK and XEROX X3T51/80-50)
#define TC_SUPPRESS_GO_AHEAD	'\003'	// 003 1 Suppress Go-ahead command (rfc858)
#define TC_RECONNECTION		'\002'	// 002 1 Reconnection (nic50005)
#define TC_ECHO			'\001'	// 001 1 Remote Echo (rfc857)
#define TC_TRANSMIT_BINARY	'\000'	// 000 1 Transit 8-bit data (rfc856)

// TC_COM_PORT_OPTION [044] options (rfc2217)
#define TC_COM_PORT_OPTION__C_SET_BAUDRATE	1	// Cli>Srv Set baud rate
#define TC_COM_PORT_OPTION__C_SET_DATASIZE	2	// Cli>Srv Set data size
#define TC_COM_PORT_OPTION__C_SET_PARITY	3	// Cli>Srv Set parity
#define TC_COM_PORT_OPTION__C_SET_STOPSIZE	4	// Cli>Srv Set stop size
#define TC_COM_PORT_OPTION__C_SET_CONTROL	5	// Cli>Srv Set control
#define TC_COM_PORT_OPTION__C_NOTIFY_LINESTATE	6	// Cli>Srv Notify line state
#define TC_COM_PORT_OPTION__C_NOTIFY_MODEMSTATE 7	// Cli>Srv Notify modem state
#define TC_COM_PORT_OPTION__C_FLOWCTRL_SUSPEND	8	// Cli>Srv Flow control suspend
#define TC_COM_PORT_OPTION__C_FLOWCTRL_RESUME	9	// Cli>Srv Flow control resume
#define TC_COM_PORT_OPTION__C_SETMSK_LINESTATE	10	// Cli>Srv Set line state mask
#define TC_COM_PORT_OPTION__C_SETMSK_MODEMSTATE	11	// Cli>Srv Set modem state mask
#define TC_COM_PORT_OPTION__C_PURGE_DATA	12	// Cli>Srv Purge data
#define TC_COM_PORT_OPTION__S_SET_BAUDRATE	101	// Srv>Cli Set baud rate
#define TC_COM_PORT_OPTION__S_SET_DATASIZE	102	// Srv>Cli Set data size
#define TC_COM_PORT_OPTION__S_SET_PARITY	103	// Srv>Cli Set parity
#define TC_COM_PORT_OPTION__S_SET_STOPSIZE	104	// Srv>Cli Set stop size
#define TC_COM_PORT_OPTION__S_SET_CONTROL	105	// Srv>Cli Set control
#define TC_COM_PORT_OPTION__S_NOTIFY_LINESTATE	106	// Srv>Cli Notify line state
#define TC_COM_PORT_OPTION__S_NOTIFY_MODEMSTATE 107	// Srv>Cli Notify modem state
#define TC_COM_PORT_OPTION__S_FLOWCTRL_SUSPEND	108	// Srv>Cli Flow control suspend
#define TC_COM_PORT_OPTION__S_FLOWCTRL_RESUME	109	// Srv>Cli Flow control resume
#define TC_COM_PORT_OPTION__S_SETMSK_LINESTATE	110	// Srv>Cli Set line state mask
#define TC_COM_PORT_OPTION__S_SETMSK_MODEMSTATE	111	// Srv>Cli Set modem state mask
#define TC_COM_PORT_OPTION__S_PURGE_DATA	112	// Srv>Cli Purge data

// TC_KERMIT [047] options (rfc2840)
#define TC_KERMIT__START_SERVER			0	// Start server
#define TC_KERMIT__STOP_SERVER			1	// Stop server
#define TC_KERMIT__REQ_START_SERVER		2	// Request start server
#define TC_KERMIT__REQ_STOP_SERVER		3	// Request stop server
#define TC_KERMIT__SOP				4	// Start of packet
#define TC_KERMIT__RESP_START_SERVER		8	// Responce start server
#define TV_KERMIT__RESP_STOP_SERVER		9	// Responce stop server

// TC_CHARSET [042] options (rfc2066)
#define TC_CHARSET__REQUEST			1	// Request
#define TC_CHARSET__ACCEPTED			2	// Accepted
#define TC_CHARSET__REJECTED			3	// Rejected
#define TC_CHARSET__TTABLE_IS			4	// Translation table is 
#define TC_CHARSET__TTABLE_REJECTED		5	// Translation table rejected
#define TC_CHARSET__TTABLE_ACK			6	// Translation table acknowledged
#define TC_CHARSET__TTABLE_NAK			7	// Translation table not acknowledged

// TC_ENCRYPT [038] options (rfc2946 with 2952,2950,2949,2948,2947)
#define TC_ENCRYPT__IS				0	// Is
#define TC_ENCRYPT__SUPPORT			1	// Supports
#define TC_ENCRYPT__REPLY			2	// Reply
#define TC_ENCRYPT__START			3	// Start
#define TC_ENCRYPT__END				4	// End
#define TC_ENCRYPT__REQUEST_START		5	// Request start
#define TC_ENCRYPT__REQUEST_END			6	// Request end
#define TC_ENCRYPT__ENC_KEYID			7	// Encryption key-id
#define TC_ENCRYPT__DEC_KEYID			8	// Decryption key-id
#define TC_ENCRYPT__TYPE_NULL			0	// Null (none)
#define TC_ENCRYPT__TYPE_DES_CFB64		1	// Data Encryption Standard Cipher feedback 64bit
#define TC_ENCRYPT__TYPE_DES_OFB64		2	// Data Encryption Standard Output feedback 64bit
#define TC_ENCRYPT__TYPE_DES3_CFB64		3	// Triple-Data Encryption Standard Cipher feedback 64bit
#define TC_ENCRYPT__TYPE_DES3_OFB64		4	// Triple-Data Encryption Standard Output feedback 64bit
#define TC_ENCRYPT__TYPE_CAST5_40_CFB64		8	// CAST5 40bit key Cipher feedback 64bit
#define TC_ENCRYPT__TYPE_CAST5_40_OFB64		9	// CAST5 40bit key Output feedback 64bit
#define TC_ENCRYPT__TYPE_CAST128_CFB64		10	// CAST-128bit key Cipher feedback 64bit
#define TC_ENCRYPT__TYPE_CAST128_OFB64		11	// CAST-128bit key Output feedback 64bit
#define TC_ENCRYPT__DES_CFB64_IV		1	// DES CFB64 Initial vector
#define TC_ENCRYPT__DES_CFB64_IV_OK		2	// DES CFB64 Initial vector ok
#define TC_ENCRYPT__DES_CFB64_IV_BAD		3	// DES CFB64 Initial vector bad
#define TC_ENCRYPT__DES_OFB64_IV		1	// DES OFB64 Initial vector
#define TC_ENCRYPT__DES_OFB64_IV_OK		2	// DES OFB64 Initial vector ok
#define TC_ENCRYPT__DES_OFB64_IV_BAD		3	// DES OFB64 Initial vector bad
#define TC_ENCRYPT__DES3_CFB64_IV		1	// DES3 CFB64 Initial vector
#define TC_ENCRYPT__DES3_CFB64_IV_OK		2	// DES3 CFB64 Initial vector ok
#define TC_ENCRYPT__DES3_CFB64_IV_BAD		3	// DES3 CFB64 Initial vector bad
#define TC_ENCRYPT__DES3_OFB64_IV		1	// DES3 OFB64 Initial vector
#define TC_ENCRYPT__DES3_OFB64_IV_OK		2	// DES3 OFB64 Initial vector ok
#define TC_ENCRYPT__DES3_OFB64_IV_BAD		3	// DES3 OFB64 Initial vector bad
#define TC_ENCRYPT__CAST128_CFB64_IV		1	// CAST5-40/128 CFB64 Initial vector
#define TC_ENCRYPT__CAST128_CFB64_IV_OK		2	// CAST5-40/128 CFB64 Initial vector ok
#define TC_ENCRYPT__CAST128_CFB64_IV_BAD	3	// CAST5-40/128 CFB64 Initial vector bad
#define TC_ENCRYPT__CAST128_OFB64_IV		1	// CAST5-40/128 OFB64 Initial vector
#define TC_ENCRYPT__CAST128_OFB64_IV_OK		2	// CAST5-40/128 OFB64 Initial vector ok
#define TC_ENCRYPT__CAST128_OFB64_IV_BAD	3	// CAST5-40/128 OFB64 Initial vector bad

// TC_AUTHENTICATION [037] options (rfc2941,1416,1409 with 2951,2944,2943,
// 2942,1412,1411)
#define TC_AUTHENTICATION__A_IS			0	// Is
#define TC_AUTHENTICATION__A_SEND		1	// Send
#define TC_AUTHENTICATION__A_REPLY		2	// Reply
#define TC_AUTHENTICATION__A_NAME		3	// Name
#define TC_AUTHENTICATION__TYPE_NULL		0	// Null (no type)
#define TC_AUTHENTICATION__TYPE_KERBEROS4	1	// Kerberos V4
#define TC_AUTHENTICATION__TYPE_KERBEROS5	2	// Kerberos V5
#define TC_AUTHENTICATION__TYPE_SPX		3	// SPX
#define TC_AUTHENTICATION__TYPE_MINK		4	// MINK
#define TC_AUTHENTICATION__TYPE_SRP		5	// SRP
#define TC_AUTHENTICATION__TYPE_RSA		6	// RSA
#define TC_AUTHENTICATION__TYPE_SSL		7	// Secure Socket Layer
#define TC_AUTHENTICATION__TYPE_LOKI		10	// LOKI
#define TC_AUTHENTICATION__TYPE_SSA		11	// SSA (uses RSA)
#define TC_AUTHENTICATION__TYPE_KEA_SJ		12	// KEA/SKIPJACK
#define TC_AUTHENTICATION__TYPE_KEA_SJ_INTEG	13	// KEA/SKIPJACK (integrated)
#define TC_AUTHENTICATION__TYPE_DSS		14	// DSS
#define TC_AUTHENTICATION__TYPE_NTLM		15	// NT/LanManager
#define TC_AUTHENTICATION__AUTH_WHO_MASK	1	// Who mask
#define TC_AUTHENTICATION__AUTH_WHO_CLI_TO_SRV	0	// Who - client to server
#define TC_AUTHENTICATION__AUTH_WHO_SRV_TO_CLI	1	// Who - server to client
#define TC_AUTHENTICATION__AUTH_HOW_MASK	2	// How mask
#define TC_AUTHENTICATION__AUTH_HOW_ONE_WAY	0	// How - one-way
#define TC_AUTHENTICATION__AUTH_HOW_MUTUAL	2	// How - mutual
#define TC_AUTHENTICATION__ENCRYPT_MASK		20	// Encryption mask
#define TC_AUTHENTICATION__ENCRYPT_OFF		0	// Encryption - off
#define TC_AUTHENTICATION__ENCRYPT_USING_TELOPT	4	// Encryption - Telopt
#define TC_AUTHENTICATION__ENCRYPT_AFTER_EXCHNG 16	// Encryption - After exchange
#define TC_AUTHENTICATION__ENCRYPT_RESERVED	20	// Encryption - RESERVED
#define TC_AUTHENTICATION__INI_CRED_FWD_MASK	8	// INI CRED FWD mask
#define TC_AUTHENTICATION__INI_CRED_FWD_OFF	0	// INI CRED FWD - off
#define TC_AUTHENTICATION__INI_CRED_FWD_ON	8	// INI CRED FWD - on
#define TC_AUTHENTICATION__KERBEROS4_AUTH	0	// Kerberos V4 Authorisation Request
#define TC_AUTHENTICATION__KERBEROS4_REJECT	1	// Kerberos V4 Reject
#define TC_AUTHENTICATION__KERBEROS4_ACCEPT	2	// Kerberos V4 Accept
#define TC_AUTHENTICATION__KERBEROS4_CHALLENGE	3	// Kerberos V4 Challenge
#define TC_AUTHENTICATION__KERBEROS4_RESPONSE	4	// Kerberos V4 Response
#define TC_AUTHENTICATION__KERBEROS5_AUTH	0	// Kerberos V5 Authorisation Request
#define TC_AUTHENTICATION__KERBEROS5_REJECT	1	// Kerberos V5 Reject
#define TC_AUTHENTICATION__KERBEROS5_ACCEPT	2	// Kerberos V5 Accept
#define TC_AUTHENTICATION__KERBEROS5_RESPONSE	3	// Kerberos V5 Response
#define TC_AUTHENTICATION__KERBEROS5_FWD	4	// Kerberos V5 Forward
#define TC_AUTHENTICATION__KERBEROS5_FWD_ACCEPT	5	// Kerberos V5 Forward accept
#define TC_AUTHENTICATION__SPX_AUTH		0	// SPX Authorisation Request
#define TC_AUTHENTICATION__SPX_REJECT		1	// SPX Reject
#define TC_AUTHENTICATION__SPX_ACCEPT		2	// SPX Accept
#define TC_AUTHENTICATION__SRP_AUTH		0	// SRP Authorisation Request
#define TC_AUTHENTICATION__SRP_REJECT		1	// SRP Reject
#define TC_AUTHENTICATION__SRP_ACCEPT		2	// SRP Accept
#define TC_AUTHENTICATION__SRP_CHALLENGE	3	// SRP Challenge
#define TC_AUTHENTICATION__SRP_RESPONSE		4	// SRP Response
#define TC_AUTHENTICATION__SRP_EXP		8	// SRP Exponential residue
#define TC_AUTHENTICATION__SRP_PARAMS		9	// SRP Parameters
#define TC_AUTHENTICATION__KEA_CA_RA		1	// KEA/SKIPJACK Cert-A RA
#define TC_AUTHENTICATION__KEA_CB_RB_IVB_NONCEB	2	// KEA/SKIPJACK Cert-B RB IVB Non-CEB
#define TC_AUTHENTICATION__KEA_IVA_RESPB_NONCEA 3	// KEA/SKIPJACK IVA Response-B Non-CEA
#define TC_AUTHENTICATION__KEA_RESPA		4	// KEA/SKIPJACK Response A
#define TC_AUTHENTICATION__DSS_INITIALIZE	1	// DSS Initialize
#define TC_AUTHENTICATION__DSS_TOKENBA		2	// DSS Token-BA
#define TC_AUTHENTICATION__DSS_CA_TOKENAB	3	// DSS Cert-A Token-AB
#define TC_AUTHENTICATION__DSS_CB_TOKENBA2	4	// DSS Cert-B Token-BA2

// TC_ENVIRON/TC_NEW_ENVIRON [036/039] options (rfc1572,1571,1408)
#define TC_ENVIRON__A_IS			0	// Is
#define TC_ENVIRON__A_SEND			1	// Send
#define TC_ENVIRON__A_INFO			2	// Information
#define TC_ENVIRON__B_VAR			0	// Variable
#define TC_ENVIRON__B_VALUE			1	// Value
#define TC_ENVIRON__B_ESC			2	// Escape char
#define TC_ENVIRON__B_USERVAR			3	// User variable

// TC_LINEMODE (rfc1184,1116)
#define TC_LINEMODE__MODE			1	// Mode
#define TC_LINEMODE__MODE_EDIT			1	// Mode - Edit
#define TC_LINEMODE__MODE_TRAPSIG		2	// Mode - Trap signal
#define TC_LINEMODE__MODE_MODE_ACK		4	// Mode - Mode acknowledge
#define TC_LINEMODE__MODE_SOFT_TAB		8	// Mode - Software tab
#define TC_LINEMODE__MODE_LIT_ECHO		16	// Mode - Literal echo
#define TC_LINEMODE__FORWARDMASK		2	// Forward Mask
#define TC_LINEMODE__SLC			3	// SLC
#define TC_LINEMODE__SLC_SLC_SYNCH		1	// SLC - Synch
#define TC_LINEMODE__SLC_SLC_BRK		2	// SLC - Break
#define TC_LINEMODE__SLC_SLC_IP			3	// SLC - Interrupt process
#define TC_LINEMODE__SLC_SLC_AO			4	// SLC - Abort output
#define TC_LINEMODE__SLC_SLC_AYT		5	// SLC - Are you there
#define TC_LINEMODE__SLC_SLC_EOR		6	// SLC - End of record
#define TC_LINEMODE__SLC_SLC_ABORT		7	// SLC - Abort
#define TC_LINEMODE__SLC_SLC_EOF		8	// SLC - End of file
#define TC_LINEMODE__SLC_SLC_SUSP		9	// SLC - Suspend
#define TC_LINEMODE__SLC_SLC_EC			10	// SLC - Erase character
#define TC_LINEMODE__SLC_SLC_EL			11	// SLC - Erase line
#define TC_LINEMODE__SLC_SLC_EW			12	// SLC - Erase word
#define TC_LINEMODE__SLC_SLC_RP			13	// SLC - RP
#define TC_LINEMODE__SLC_SLC_LNEXT		14	// SLC - LNEXP
#define TC_LINEMODE__SLC_SLC_XON		15	// SLC - Flow Control - XON
#define TC_LINEMODE__SLC_SLC_XOFF		16	// SLC - Flow Control - XOFF
#define TC_LINEMODE__SLC_SLC_FORW1		17	// SLC - Forward 1
#define TC_LINEMODE__SLC_SLC_FORW2		18	// SLC - Forward 2
#define TC_LINEMODE__SLC_SLC_MCL		19	// SLC - Move cursor left
#define TC_LINEMODE__SLC_SLC_MCR		20	// SLC - Move cursor right
#define TC_LINEMODE__SLC_SLC_MCWL		21	// SLC - Move cursor word left
#define TC_LINEMODE__SLC_SLC_MCWR		22	// SLC - Move cursor word right
#define TC_LINEMODE__SLC_SLC_MCBOL		23	// SLC - Move cursor beginning of line
#define TC_LINEMODE__SLC_SLC_MCEOL		24	// SLC - Move cursor end of line
#define TC_LINEMODE__SLC_SLC_INSRT		25	// SLC - Insert
#define TC_LINEMODE__SLC_SLC_OVER		26	// SLC - Overwrite
#define TC_LINEMODE__SLC_SLC_ECR		27	// SLC - Erase char right
#define TC_LINEMODE__SLC_SLC_EWR		28	// SLC - Erase word right
#define TC_LINEMODE__SLC_SLC_EBOL		29	// SLC - Erase to beginning of line
#define TC_LINEMODE__SLC_SLC_EEOL		30	// SLC - Erase to end of line
#define TC_LINEMODE__SLC_DEFAULT		3	// SLC Default
#define TC_LINEMODE__SLC_VALUE			2	// SLC Value
#define TC_LINEMODE__SLC_CANTCHANGE		1	// SLC Can not change
#define TC_LINEMODE__SLC_NOSUPPORT		0	// SLC Not supported
#define TC_LINEMODE__SLC_LEVELBITS		3	// SLC Level bits
#define TC_LINEMODE__SLC_ACK			128	// SLC Acknowledge
#define TC_LINEMODE__SLC_FLUSHIN		64	// SLC Flush input
#define TC_LINEMODE__SLC_FLUSHOUT		32	// SLC Flush output

// TC_TOGGLE_FLOW_CONTROL [033] options (rfc1372)
#define TC_TOGGLE_FLOW_CONTROL__OFF		0	// Flow control off
#define TC_TOGGLE_FLOW_CONTROL__ON		1	// Flow control on
#define TC_TOGGLE_FLOW_CONTROL__RESTART_ANY	2	// Any char but XOFF restarts output
#define TC_TOGGLE_FLOW_CONTROL__RESTART_XON	3	// Only XON will restart output


