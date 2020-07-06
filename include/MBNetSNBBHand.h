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
// P.Smith                      22/1/07
// name change U2SerialHandler to SlaveSerialHandler
//
// P.Smith                      5/2/07
// name change NetworkSlaveRxHandler, NetworkSlaveTxHandler,NetworkSlaveTxOff,ResetSlavembrx
//
////////////////////////////////////////////////////////////////

#ifndef __MBNETSNBBHAND_H__
#define __MBNETSNBBHAND_H__

#include <basictypes.h>

void NetworkSlaveRxHandler( void );
void NetworkSlaveTxHandler( void );
void NetworkSlaveTxOff( void );
void SlaveSerialHandler( void );
void ResetSlavembrx(void);



#endif   // __MBNETSNBBHAND_H__

