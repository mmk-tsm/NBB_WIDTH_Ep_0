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
////////////////////////////////////////////////////////////////

#ifndef __MBNBSBHAND_H__
#define __MBNBBSHAND_H__

#include <basictypes.h>
extern   BOOL g_bSBBL2Fitted;
extern   int g_nMBSlaveCommsWatchDogTimer;
extern	BYTE g_cReceiveBEnabled;
extern	BYTE g_cTransmitBEnabled;

void	U1TxOn( void );
void	U1RxOn( void );
void	U1TxRxOff( void );
void	U1TxHandler( void );
void	U1RxHandler( void );
void	ResetU1mbrx1(void);
void	SaveU1RxdCh(void);
char	readU1nwrd(void);
char    writeU1nwrd(void);
char	writeU11wrd(void);
void U1TxOff( void );
void ResetU1MBSlaveComms(void);
void GetU1MBBytes(void);
void SerialBHandler( void );
void U1SerialHandler( void );
void PrintU1ModbusMessage( void );

#define	RX1U1BUFSIZE				270
#define	TX1U1BUFSIZE				270


#endif   // __MBNBBSHAND_H__

