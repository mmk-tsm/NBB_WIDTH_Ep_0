////////////////////////////////////////////////////////////////
// ModbusSlaveHandler.h
//
//
// 
//
// P.Smith             				3-05-2003
// M.McKiernan						09-06-2003
// Changed buffer sizes and break definitions to comply with modbus.
// M.McKiernan						12.06.2003
// Added TxBoff,ResetMBSlaveComms
// M.McKiernan						08.08.2003
//		MBBREAK 	changed 30 -> 8
//		MBTX_DELAY 	changed 5 -> 4
//
// P.Smith             				22/02/06
// define DELAYAFTERLASTCHTX, MBBREAK, MBTX_DELAY in terms of baud rate and pit frequency.
// WORD -> int extern   int g_nMBSlaveCommsWatchDogTimer;
//
// P.Smith             				20/09/06
// name change to reflect the functionality rather than the serial port used.
//
// P.Smith             				4/7/7
// use NETWORK_BAUDRATE when defining comms timeeouts etc.

//
////////////////////////////////////////////////////////////////

#ifndef __MBSHAND_H__
#define __MBSHAND_H__

#include <basictypes.h>
#include "General.h"
extern   BOOL g_bL2Fitted;
extern   int g_nMBSlaveCommsWatchDogTimer;
extern	BYTE g_cReceiveBEnabled;
extern	BYTE g_cTransmitBEnabled;

void	NetworkTransmitterOn( void );
void	NetworkReceiverOn( void );
void	Resetmbrx1(void);
void	SaveRxdCh(void);
char	readnwrd(void);
char	writenwrd(void);
char	write1wrd(void);
void NetworkTransmitterOff( void );
void ResetMBSlaveComms(void);
void GetMBBytes(void);
//nbb #define MBTX_DELAY		4		// set to 4 character times
#define MBTX_DELAY_CHAR_NO		4		// set to 4 character times


#define MBTX_DELAY		(MBTX_DELAY_CHAR_NO * 11 * PIT_FREQUENCY) / NETWORK_BAUDRATE		// increased due to increased speed of pit

#define MBSLAVECOMSWDOGPERIOD   3		// 3 seconds.

//#define	RX1BUFSIZE				200
//#define	TX1BUFSIZE				200

//#define	MBBREAK 				30

#define	RX1BUFSIZE				270
#define	TX1BUFSIZE				270

//#define	MBBREAK 				30
//#define	MBBREAK 				5
//#define	MBBREAK 				8		// nb FIFO in operation (4 chars), try to not get false breaks.
//nbb #define	MBBREAK 				24		// increased due to increased speed of pit
#define BREAK_CHAR_NO               7
#define	MBBREAK 				(BREAK_CHAR_NO * 11 * PIT_FREQUENCY) / NETWORK_BAUDRATE		// increased due to increased speed of pit

//#define	CP2000ADDRESS 			1
//nbb define	DELAYAFTERLASTCHTX		13      // was 5 increase to 13 to allow for faster pit.

#define	DELAYAFTERLASTCHTX_CHAR_NO	4               // 

#define	DELAYAFTERLASTCHTX		    (DELAYAFTERLASTCHTX_CHAR_NO * 11 * PIT_FREQUENCY) / NETWORK_BAUDRATE      // was 5 increase to 13 to allow for faster pit.

#define ILLEGAL_MB_FUNCTION         1     // ILLEGAL FUNCTION.       
#define ILLEGAL_MB_ADDRESS				2	   // ILLEGAL DATA ADDRESS    
#define ILLEGAL_MB_DATA_VALUE			3	   // ILLEGAL DATA VALUE      
#define ILLEGAL_MB_DATA_ACCESS		4	   // ILLEGAL DATA ACCESS     

#endif   // __MBSHAND_H__

