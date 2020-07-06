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
// P.Smith                          21/09/06               
// added PANEL_MBBREAK,PANEL_DELAYAFTERLASTCHTX, PANEL_MBTX_DELAY		
////////////////////////////////////////////////////////////////

#ifndef __MBPANELSNBBHAND_H__
#define __MBPANELSNBBHAND_H__

#include <basictypes.h>
extern   BOOL g_bL2Fitted;
extern   int g_nMBSlaveCommsWatchDogTimer;
extern	BYTE g_cReceiveBEnabled;
extern	BYTE g_cTransmitBEnabled;

void	PanelTxOn( void );
void	PanelRxOn( void );
void	PanelTxRxOff( void );
void	PanelTxHandler( void );
void	PanelRxHandler( void );
void	ResetPanelmbrx1(void);
void	SavePanelRxdCh(void);
char	readPanelnwrd(void);
char    writePanelnwrd(void);
char	writePanel1wrd(void);
void PanelTxOff( void );
void ResetPanelMBSlaveComms(void);
void GetPanelMBBytes(void);
void SerialBHandler( void );
void PanelSerialHandler( void );
void PrintPanelModbusMessage( void );

#define	NBB_UART_RX_BYTE_AVAILABLE	0x01	// Bit 0

#define	PANEL_MBBREAK 				(BREAK_CHAR_NO * 11 * PIT_FREQUENCY) / PANEL_BAUDRATE		// increased due to increased speed of pit
#define	PANEL_DELAYAFTERLASTCHTX    (DELAYAFTERLASTCHTX_CHAR_NO * 11 * PIT_FREQUENCY) / PANEL_BAUDRATE         // was 5 increase to 13 to allow for faster pit.
#define PANEL_MBTX_DELAY		(MBTX_DELAY_CHAR_NO * 11 * PIT_FREQUENCY) / PANEL_BAUDRATE		// increased due to increased speed of pit  //



#define	NBB_UART_SR_RX_READY			0x01
#define	NBB_UART_SR_TX_READY			0x04

// Interrupt status bits

#define	NBB_UART_ISR_RX_READY	   0x02
#define	NBB_UART_ISR_TX_READY		0x01







#define NBB_UART_RESET_ERROR_STATUS          0x40
#define	RX1PANELBUFSIZE				270
#define	TX1PANELBUFSIZE				270


#endif   // __MBPANELSNBBHAND_H__

