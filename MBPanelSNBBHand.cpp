////////////////////////////////////////////////////////////////
// ModbusSlaveHandler.c - adapted for Batch blender.
//
//
// How it works:
// The CP2000 acts as a slave from a higher order system.
// Channel B is used as the slave port.
//
//	P.Smith								6/5/3
//	Readnwrd and writenwrd data is distributed and the appropriate
//  flags are set to show that the writenwrd command has been
//	initiated.
//	Write1wrd is inserted and verified as operational
// M.McKiernan						09-06-2003
// Brought g_cVAC350MBUpdateFlag setting inside of return statement - in Write1Wrd() and WritenWrd()
// Transmit & Receive buffer pointers declared as ints g_nTx1Ptr g_nRx1Ptr
// g_cBlenderNetworkAddress is the blender network comms address, switch selectable on SBB.
// 12-06-2003	In TXBHandler disable TxMtr only, not receiver.
//			Watchdog timer on slave comms  (MBSLAVECOMSWDOGPERIOD)
//			Diagnostic for write 1 word CopyRx1Buff[j] = Rx1Buff[j] and CopyTx1Buff[j] = Tx1NBBBuff[j]


// M.McKiernan						09-06-2004
// Adapting for batch blender from VAC350 version.
// Removed g_cVAC350MBUpdateFlag, g_arrVAC350MBUpdates[k]
// M.McKiernan						29-10-2004
// Added function CalculateModbusCRC() but it has been commented out, and is not called.
// M.McKiernan						08-11-2004
// Replaced CalculateModbusChecksum() function by CalculateModbusCRC() - uses tabular approach, tested at 5 times faster.
//
//
// P.Smith                              11/1/06
// Corrected warnings
// char    Rx1Buff[RX1BUFSIZE];  to unsigned char
// char    Tx1Buff[TX1BUFSIZE];
// char    CopyRx1Buff[8];
// char    CopyTx1Buff[8];
// remove     char    cRxByte;
// int     CRCChecksum; to unsigned
// remove    int     j from SaveRxdCh
// removed   unsigned char MBByteIndex,BlockStartAddr, AddressTemp2,ny;
// removed kb/x from writenwrd & write1wrd
//
// P.Smith                          16/1/06
// testing modbus comms, serial commns is operational through modbus.
//
// P.Smith                          19/1/06
// removed    BOOL g_bSBBL2Fitted;, already defined in Batvars.c
//  correct definitions for WORD g_arrnMBTable[MB_TABLE_SIZE];
// WORD g_arrnMBTableFlags[MB_TABLE_WRITE_SIZE]; added
//
// P.Smith                          7/2/06
// remove  int g_nFlashRate from this file
//
// P.Smith                          15/2/06
// name change CharsAndInts -> CharsAndWords
// put back in g_arrnMBTable variables and other back in as ints.
// These were originally defined as words, to be review later on.
//
// P.Smith                          17/2/06
// implement CheckMBValidData in writenwrd and write1wrd
//
// P.Smith                          22/2/06
// WORD -> int g_nMBSlaveCommsWatchDogTimer;
//
//
// P.Smith                          19-05-2006
// Ensure that g_nMBWriteOrFlagTemp is cleared in writenwrd
// This caused a problem in that the order was downloaded a second time
// when the time was downloaded.
//
// P.Smith                              19/5/06
// remove fifo reset on rxbon / off, enable rxb when last char is transmitted
// not after 4 chars of break.
//
// P.Smith                              19/5/06
// first pass for NBB serial handler.
// Tx1Buff -> Tx1NBBBuff
// Rx1Buff -> Rx1NBBBuff
// g_nTx1Ptr -> g_nTx1NBBPtr
// g_nMBSlaveCommsWatchDogTimer -> g_nMBSlaveNBBCommsWatchDogTimer
// g_cReceiveBEnabled -> g_cNBBReceiveBEnabled
// g_cTransmitBEnabled -> g_cNBBTransmitBEnabled
// g_nRx1Ptr -> g_nNBBRx1Ptr
// g_cChannelAddr -> g_cNBBChannelAddr
// g_nRx1ByteNo -> g_nNBBRx1ByteNo
// g_cMB1txdlytime -> g_cNBBMB1txdlytime
// g_nMBRx1Bytes -> g_nNBBMBRx1Bytes
// g_nMBTx1Bytes -> g_nNBBMBTx1Bytes
// g_cMBRx1Func -> g_cNBBMBRx1Func
// g_bBlenderSelected -> g_bNBBBlenderSelected
// g_nMB1BreakCounter -> g_nNBBMB1BreakCounter
// g_cMB1BreakFlag -> g_cNBBMB1BreakFlag
// g_nCRCErrorCounter -> g_nNBBCRCErrorCounter
// g_nSlaveGoodRxCounter -> g_nNBBSlaveGoodRxCounter
// g_nSlaveGoodTxCounter -> g_nNBBSlaveGoodTxCounter
// g_cRx1Data -> g_cNBBRx1Data
// g_cErrorCode -> g_cNBBErrorCode
// g_cTx1OffFlag -> g_cNBBTx1OffFlag
// g_cTx1ModeFlag -> g_cNBBTx1ModeFlag
// g_cLastChTimer -> g_cNBBLastChTimer
// TxBOn -> TxBNBBOn
// RxBOn -> RxBNBBOn
// TxRxBOff -> TxRxBNBBOff
// TxBOff -> TxBNBBOff
// SerialBNBBHandler -> SerialBHandler
// TXBHandler -> TXBNBBHandler
// RXBHandler -> RXBNBBHandler
// ResetMBSlaveComms -> ResetNBBMBSlaveComms
// Resetmbrx1 -> ResetNBBmbrx1
// GetMBBytes -> GetNBBMBBytes
// SaveRxdCh -> SaveNBBRxdCh
// readnwrd -> readNBBnwrd
// writenwrd -> writeNBBnwrd
// RX1BUFSIZE -> RX1NBBBUFSIZE
//
// P.Smith                              11/7/06
// name change to U1 instead of NBB for the above.
//
// P.Smith                              19/9/06
// Ensure that the transmitter is switched off correctly, this was
// not done correctly and had the effect of the transmitter staying on
// Rename functions to reflect the functionality and not the port that
// it is assigned to.
//
//
// P.Smith                              19/9/06
// name change to reflect the functionality of the port rather than the
// port name, in this case "panel"
//
// P.Smith                              2/11/06
// name change PANEL_DELAYAFTERLASTCHTX,PANEL_MBBREAK
//
// P.Smith                              5/2/06
// check for g_bPrintfRunning in ResetPanelMBSlaveComms, should not be initialised if
// printf is running.
// Remove printfs
//
// P.Smith                              7/3/07
// Remove check for address off the bcd switch for the operator panel.
// address set at OPERATORPANELADDRESS for the operator panel
//
//
// P.Smith                              6/6/07
// if BATCH_SETPOINT_DOWNLOAD_MANAGER is not INHIBIT_DOWNLOAD,
// set g_nMBWriteOrFlagTemp to 0
// call CheckMBValidData if BATCH_SETPOINT_DOWNLOAD_MANAGER is not INHIBIT_DOWNLOAD
// else set the error code to 0
//
// P.Smith                              11/6/07
// check g_arrnMBTable[BATCH_SETPOINT_DOWNLOAD_MANAGER] != ALLOW_DOWNLOAD before
// running CheckMBValidData
// in PrintPanelModbusMessage print 100 characters
//
// P.Smith                              26/7/07
// modified write commands to check for different data being written.
// this is done in write 1 and write n words commands.
//
// M.McKiernan                      17/9/07
// Removed g_arrnMBTable[] & g_arrnMBTableFlags[], now in BatVars2
//       g_nMBWriteOrFlag, g_nMBWriteOrErrorFlag, g_nMBWriteOrFlagTemp
//
//
// P.Smith                              23/11/07
// added BATCH_CONTROL_SCREW_SPEED check in writePanelnwrd & writePanel1wrd, g_bModbusScrewSpeedWritten flag
// is set if this word is written.
//
// P.Smith                              31/1/08
// no TakeACopyOfModbusSetpointData from write set g_bPanelWriteHasHappened
// to indicate that write has taken place from the panel
// g_nWritenwrdCounter incremented to indicate that the write n word command has happened.
//
// P.Smith                          23/7/08
// remove g_arrnMBTable & comms flags externs
//
// P.Smith                          26/9/08
// added d/a written commands check on writen and write1 word.
//
// P.Smith                          14/11/08
// call PrintModbusMessage to print to telnet
//
// P.Smith                          16/6/09
// allow for download of data for vacuum loading set point data
// check for vacuum loader block and set g_bVacuumLoaderCalibrationWritten
// this will then initiate the download of the data to eeprom.
//
// P.Smith                          21/7/09
// if download manager is set to ALLOW_DOWNLOAD, then do not run writenwrd
// command.
// set g_bICSRecipeDataWritten if ics data has been written.
//
// P.Smith                          24/8/09
// set g_bLockIsEnabled if the panel version number is being downloaded
//
// P.Smith                          1/9/09
// added RecordChangedModbusData to record changed data in modbus write n
// word and write 1 word.
//
// P.Smith                          5/10/09
// do not record changes to modbus if the panel version no is being written down
//
// P.Smith                          15/4/10
// added check for BATCH_SET_LICENSE_OPTIONS_1-4 written and set flag indicating write
//
// P.Smith                          8/6/10
// added commands for width control writes.
//
// P.Smith                          7/7/10
// added calibrate of reference and diameter commands
//
// P.Smith                          19/7/10
// added offset sign,offset,diameter from lay flat entry
//
// P.Smith                          16/9/10
// added width set point command
////////////////////////////////////////////////////////////////


#include <Stdio.h>

#include "General.h"

#include <basictypes.h>

#include "MBSHand.h"
#include "BatchMBIndices.h"

#include "InitComms.h"
#include "NBBGpio.h"

#include "MBProgs.h"
#include "CalculateModbusCRC.h"

#include "predef.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "includes.h"
#include "constants.h"
#include "ucos.h"
#include "cfinter.h"
#include "utils.h"
#include "buffers.h"
#include "serial.h"
#include "bsp.h"
#include "iointernal.h"
#include "iosys.h"
#include "serinternal.h"
#include "randseed.h"
#include <sim5270.h>
#include "InitNBBComms.h"
#include "PrinterFunctions.h"
#include "MBPanelSNBBhand.h"
#include "Batvars.h"
#include "Batvars2.h"
#include "PrntDiag.h"
#include "Vacvars.h"
#include "WidthMB.h"
#include "WidthVars.h"



extern  int NBBBoardAddress;
extern int g_nFlashRate;		// LED flash rate period in PIT ticks.


    int g_nMBSlaveNBBCommsWatchDogTimer;
	BYTE g_cNBBReceiveBEnabled = 0;
	BYTE g_cNBBTransmitBEnabled = 0;



extern int g_nFlashRate;		// LED flash rate period in PIT ticks.
extern  char    *strAutomatic[];
extern  char    *strNewLine;
extern  char    g_cPrinterBuffer[];

extern BOOL	g_bLicenseOptions1Written;
extern BOOL	g_bLicenseOptions2Written;
extern BOOL	g_bLicenseOptions3Written;
extern BOOL	g_bLicenseOptions4Written;


unsigned char	Rx1NBBBuff[RX1PANELBUFSIZE];
unsigned char	Tx1NBBBuff[TX1PANELBUFSIZE];

WORD g_nTx1NBBPtr = 0;
WORD g_nNBBRx1Ptr = 0;
unsigned char g_cNBBChannelAddr;
unsigned char g_cNBBMB1txdlytime=0;


//unsigned char g_nRx1ByteNo;
WORD g_nNBBRx1ByteNo;
//unsigned char g_nMBRx1Bytes;
WORD g_nNBBMBRx1Bytes;
//unsigned char g_nMBTx1Bytes;
WORD g_nNBBMBTx1Bytes;
char g_cNBBMBRx1Func;
BOOL  g_bNBBBlenderSelected = FALSE;
WORD  g_nNBBMB1BreakCounter=0;
char g_cNBBMB1BreakFlag = 1;                  // set break flag on power-up
WORD  g_nNBBCRCErrorCounter = 0;
WORD	g_nNBBSlaveGoodRxCounter = 0;
WORD	g_nNBBSlaveGoodTxCounter = 0;
unsigned char g_cNBBRx1Data,g_cNBBErrorCode;
char g_cNBBTx1OffFlag = 0;            // flag to have Tx1 switched off when TSR empty.
char g_cNBBTx1ModeFlag = 0;
char g_cNBBLastChTimer = 0;


void PanelTxOn( void )
{
   volatile uartstruct * ThisUart = &sim.uarts[NBB_PANEL_COMMS_PORT];
   ThisUart->ucr = 0x04;   // 0000 0100 - Enable  tx
   ThisUart->uisr = 0x01; // 00000001 - enable tx interrupt
	g_cNBBTransmitBEnabled = 1;
	g_cNBBReceiveBEnabled = 0;
}




void PanelRxOn( void )
{
   volatile uartstruct * ThisUart = &sim.uarts[NBB_PANEL_COMMS_PORT];
   ThisUart->ucr = RX_FIFO_ENABLE;   //
   ThisUart->uisr = 0x02;
	g_cNBBReceiveBEnabled = 1;
	g_cNBBTransmitBEnabled = 0;
}


void PanelTxRxOff( void )
{
   volatile uartstruct * ThisUart = &sim.uarts[NBB_PANEL_COMMS_PORT];
   ThisUart->uisr = NO_INTERRUPTS;
//    *(SERIAL_PORT_B + INTERRUPT_ENABLE) = NO_INTERRUPTS;

	g_cNBBTransmitBEnabled = 0;
	g_cNBBReceiveBEnabled  = 0;

}


void PanelTxOff( void )
{
   volatile uartstruct * ThisUart = &sim.uarts[NBB_PANEL_COMMS_PORT];
   ThisUart->uisr = NO_INTERRUPTS;
//    *(SERIAL_PORT_B + INTERRUPT_ENABLE) = NO_INTERRUPTS;
   ThisUart->ucr = 0x08;
   g_cNBBTransmitBEnabled = 0;
}


//////////////////////////////////////////////////////
// PanelSerialHandler( void )
// NBB Panel serial handler.
//
// Function : decides if the interrupt is transmit or receive and
// handles the message accordingly.
//
//////////////////////////////////////////////////////



void PanelSerialHandler( void )
{
volatile uartstruct * ThisUart= & (sim.uarts[NBB_PANEL_COMMS_PORT]);
	char    cStatus;
    BOOL    bError = TRUE;
    cStatus = ThisUart->uisr;

    if((cStatus & NBB_UART_ISR_RX_READY) && (g_cNBBReceiveBEnabled))
    {
		// Indicate that this is a valid interrupt
		bError = FALSE;

		// Process the rx handler

		PanelRxHandler();
    }
    else if((cStatus & NBB_UART_ISR_TX_READY) && g_cNBBTransmitBEnabled )
	{
		// Indicate that this is a valid interrupt

		// Process the tx handler
		PanelTxHandler();
	}
}

//////////////////////////////////////////////////////
// PanelTxHandler( void )
// NBB Panel transmit serial handler.
//
// Function : transmits characters in buffer
//
//////////////////////////////////////////////////////


void PanelTxHandler( void )
{
volatile uartstruct * ThisUart= & (sim.uarts[NBB_PANEL_COMMS_PORT]);
        ThisUart->utb = Tx1NBBBuff[g_nTx1NBBPtr];

        g_nTx1NBBPtr++;
            if((g_nTx1NBBPtr >= g_nNBBMBTx1Bytes) || (g_nTx1NBBPtr >= TX1PANELBUFSIZE))          // LASTBYTE, OR END OF BUFFER
                {
//                    TxRxBOff();
                    g_cNBBTx1ModeFlag = 0; // switch out of transmit mode
                    g_cNBBLastChTimer = PANEL_DELAYAFTERLASTCHTX;
                        g_cNBBTx1OffFlag = 1;  // set flag for turning off xmtr
                    PanelTxOff();
                    g_nNBBSlaveGoodTxCounter++;
                    PanelRxOn();
                }
}

//////////////////////////////////////////////////////
// PanelRxHandler( void )
// NBB Panel transmit serial handler.
//
// Function : Receives characters in buffer
//
//////////////////////////////////////////////////////

void PanelRxHandler( void )
{
volatile uartstruct * ThisUart= & (sim.uarts[NBB_PANEL_COMMS_PORT]);
    char    cLineStatus;


	 if(g_nNBBRx1Ptr >= RX1PANELBUFSIZE)       // Ensure no write beyond buffer
         g_nNBBRx1Ptr = RX1PANELBUFSIZE-1;


	g_nNBBMB1BreakCounter = PANEL_MBBREAK;        // X mS (decremented on 4 kHz to zero)(9600/19200)
	g_nMBSlaveNBBCommsWatchDogTimer = MBSLAVECOMSWDOGPERIOD;		// decremented to 0 on 1sec (in PIT), and if -> 0, call reset mb slave comms.
	// Read the status

	// read received data while the receive ready bit is set
		while(ThisUart->usr & NBB_UART_SR_RX_READY)

		{
			cLineStatus = ThisUart->usr;

			// If there is a pending byte, process it
			if( cLineStatus & NBB_UART_SR_RX_READY )
			{
			// Reading the byte clears the rxrdy bit
			g_cNBBRx1Data = ThisUart->utb;


			// check for transmission error
			if( cLineStatus & 0x70 )
				{
				    ThisUart->ucr |= NBB_UART_RESET_ERROR_STATUS;   // reset error status

 //                  iprintf("\n panel comms errors %x",cLineStatus);  //nbb--testonly--
				// Bad byte, reset receive handler
				ResetPanelmbrx1();
				return;

				}
				else
				{

			// Save the byte
 				    SavePanelRxdCh();
				}

			}
		}  // end of while
}

//////////////////////////////////////////////////////
// ResetPanelMBSlaveComms( void )
//
// Function : reinitialises slave comms
//
//////////////////////////////////////////////////////


void ResetPanelMBSlaveComms(void)
{
    if(!g_bPrintfRunning)
	{
        InitialisePanelSerialPort();
	    ResetPanelmbrx1();
	}

}


//////////////////////////////////////////////////////
// ResetPanelmbrx1( void )
//
// Function : reinitialises pointers for comms
//
//////////////////////////////////////////////////////

void ResetPanelmbrx1(void)
{
        g_nNBBRx1Ptr = 0;
        g_nNBBRx1ByteNo = 0;
        g_nNBBMBRx1Bytes = 0; // no of bytes expected
        g_cNBBMBRx1Func = 0;
        g_bNBBBlenderSelected = FALSE;
}


//////GetMBBytes /////////////////////////////////////////////////
//////GetMBBytes gets no. of bytes in modbus message//////////////
void GetPanelMBBytes(void)
{
	g_nNBBMBRx1Bytes = 8;                        // leave at 8 for unused messages
	if(g_cNBBMBRx1Func == 7 )                    // status read, 4 bytes
			g_nNBBMBRx1Bytes = 4;
	if(g_cNBBMBRx1Func == 3 || g_cNBBMBRx1Func == 4 || g_cNBBMBRx1Func == 6 ) //3/4read n words, 6 write 1 word, 8 bytes
        g_nNBBMBRx1Bytes = 8;
	if(g_cNBBMBRx1Func == 16 )                   // write n words
		{
        		if (g_nNBBRx1ByteNo <= 7)
                	g_nNBBMBRx1Bytes = 10;        // temp value, min of 10 bytes in write n words
        		else
                	g_nNBBMBRx1Bytes = 9 + 2*Rx1NBBBuff[5];  //Rxbuff[5] has no. of words.
		}
}


//////////////////////////////////////////////////////
// SavePanelRxdCh( void )
//
// Function : takes receive data, verifies checksum and fills
//            the transmit buffer to reply to the master unit
//
//////////////////////////////////////////////////////


void SavePanelRxdCh(void)

{
	union   CharsAndWord     Rxed1CRC;
	union   CharsAndWord     uTxChecksum ;
	WORD		CRCChecksum;

 if(g_cNBBMB1BreakFlag)
  {
        g_cNBBMB1BreakFlag = 0;
        ResetPanelmbrx1();
        if(g_cNBBRx1Data == OPERATORPANELADDRESS)   // addr on SBB pcb

        {

             g_bNBBBlenderSelected = TRUE;                //this blender being addressed g_cChannelAddr = g_cRx1Data
                Rx1NBBBuff[g_nNBBRx1Ptr] = g_cNBBRx1Data;      // store byte in buffer
                g_cNBBChannelAddr = g_cNBBRx1Data;
//                CommsChannelNum = 0;  //loader channel no. (0-7)
                g_nNBBRx1ByteNo++;
                g_nNBBRx1Ptr++;
                return;
        }
        else
        {
                g_bNBBBlenderSelected = FALSE;                //this IVR not addressed
                return;         // do nothing if not address
        }
  }  // end of break detect

 		if(!g_bNBBBlenderSelected)          		// finished if unit not addressed
			return;
 		Rx1NBBBuff[g_nNBBRx1Ptr] = g_cNBBRx1Data;      // (SAVRXBYT)store byte in buffer
 		g_nNBBRx1ByteNo++;
 		g_nNBBRx1Ptr++;
 		if(g_nNBBRx1ByteNo == 2)
 		{
         g_cNBBMBRx1Func = g_cNBBRx1Data;
         return;
 		}
 		if(g_nNBBRx1ByteNo < 4)
           return;
  		if(g_nNBBRx1ByteNo == 9 || g_nNBBRx1ByteNo == 4)
  		{
          GetPanelMBBytes();           // get no of bytes expected in message
  		}

  		if(g_nNBBRx1ByteNo != g_nNBBMBRx1Bytes)     // check if last byte
                  return;


//// last byte
		CRCChecksum = CalculateModbusCRC( Rx1NBBBuff, g_nNBBRx1Ptr-2);
//		CRCChecksum = CalculateModbusCRC( Rx1Buff, g_nRx1Ptr-2);



		Rxed1CRC.cValue[0] = Rx1NBBBuff[g_nNBBRx1Ptr-1];         // l.s. byte
		Rxed1CRC.cValue[1] = Rx1NBBBuff[g_nNBBRx1Ptr-2];
		if(Rxed1CRC.nValue != CRCChecksum)             // CRC in error ?
		{
		        g_nNBBCRCErrorCounter++;
                return;
		}
		else
		{
			g_nNBBSlaveGoodRxCounter++;				// increment good message counter.
		}


		g_nFlashRate = FLASH_RATE_10HZ;		// set to flash Heart beat LED at 10 Hz.

   	if(g_cNBBMBRx1Func == 0x03 || g_cNBBMBRx1Func == 0x04)     // check for read n words command
   	{
        g_cNBBErrorCode = readPanelnwrd();
        if(g_cNBBErrorCode)
        {
                Tx1NBBBuff[0] = g_cNBBChannelAddr;
                Tx1NBBBuff[1] = g_cNBBMBRx1Func | 0x80;          // 8-03-02 set msb for error.
                Tx1NBBBuff[2] = g_cNBBErrorCode;
		        uTxChecksum.nValue = CalculateModbusCRC( Tx1NBBBuff,3);
                Tx1NBBBuff[3] = uTxChecksum.cValue[1];
                Tx1NBBBuff[4] = uTxChecksum.cValue[0];
                g_nNBBMBTx1Bytes = 5;                 // transmit back 5 bytes
        }
        else                                    // must be ok
        {

			 uTxChecksum.nValue = CalculateModbusCRC( Tx1NBBBuff, g_nTx1NBBPtr);
                Tx1NBBBuff[g_nTx1NBBPtr] = uTxChecksum.cValue[1];
                g_nTx1NBBPtr++;
                Tx1NBBBuff[g_nTx1NBBPtr] = uTxChecksum.cValue[0];  // l.s. byte
                g_nNBBMBTx1Bytes = g_nTx1NBBPtr+1;                 // no. of bytes to transmit back


        }
   	}


   	if(g_cNBBMBRx1Func == 0x06)     // check for write 1 word
   	{
         g_cNBBErrorCode = writePanel1wrd();
        if(g_cNBBErrorCode)          // error condition  if non zero
        {
                Tx1NBBBuff[0] = g_cNBBChannelAddr;
                Tx1NBBBuff[1] = g_cNBBMBRx1Func | 0x80;          // 8-03-02 set msb for error.
                Tx1NBBBuff[2] = g_cNBBErrorCode;
			 		uTxChecksum.nValue = CalculateModbusCRC( Tx1NBBBuff,3);
                Tx1NBBBuff[3] = uTxChecksum.cValue[1];
                Tx1NBBBuff[4] = uTxChecksum.cValue[0];
                g_nNBBMBTx1Bytes = 5;                 // transmit back 5 bytes
        }
        else                                    // must be ok
        {
	 		    uTxChecksum.nValue = CalculateModbusCRC( Tx1NBBBuff,6);
                Tx1NBBBuff[6] = uTxChecksum.cValue[1];
                Tx1NBBBuff[7] = uTxChecksum.cValue[0];  // l.s. byte
                g_nNBBMBTx1Bytes = 8;                 // no. of bytes to transmit back
        }
   	}


   if(g_cNBBMBRx1Func == 0x10)     // check for write n words
   {
        g_cNBBErrorCode = writePanelnwrd();
        if(g_cNBBErrorCode)          // error condition  if non zero
        {
                Tx1NBBBuff[0] = g_cNBBChannelAddr;
                Tx1NBBBuff[1] = g_cNBBMBRx1Func | 0x80;          // 8-03-02 set msb for error.
                Tx1NBBBuff[2] = g_cNBBErrorCode;
			 uTxChecksum.nValue = CalculateModbusCRC( Tx1NBBBuff,3);
                Tx1NBBBuff[3] = uTxChecksum.cValue[1];
                Tx1NBBBuff[4] = uTxChecksum.cValue[0];
                g_nNBBMBTx1Bytes = 5;                 // transmit back 5 bytes
        }
        else                                    // must be ok
        {
 			 uTxChecksum.nValue = CalculateModbusCRC( Tx1NBBBuff,6);
			 Tx1NBBBuff[6] = uTxChecksum.cValue[1];
             Tx1NBBBuff[7] = uTxChecksum.cValue[0];  // l.s. byte
             g_nNBBMBTx1Bytes = 8;                   // no. of bytes to transmit back
        }

   } ////end of write n words check


	 	g_nNBBRx1Ptr = 0;              // reset rx buff pointer.
     	g_nTx1NBBPtr = 0;              // reset tx buff pointer.

     if(g_nNBBMBTx1Bytes)             // i.e. if bytes to transmit is non-zero
     {
        g_cNBBMB1txdlytime = MBTX_DELAY;      //mbtx_delay depends on baud rate
     }

}

//////////////////////////////////////////////////////
// readPanelnwrd( void )
//
// Function : read n word command.
//
//////////////////////////////////////////////////////



///// ROUTINE TO READ N WORDS
char readPanelnwrd(void)
{
	unsigned char numbytes,nNumwords;
	unsigned char j;		//,y;
	WORD i = 3;			// point to 4th byte in buffer
	WORD	MBRegisterIndex,k;
	WORD AddressTemp;    // ,AddressTemp2,ny;
	union   CharsAndWord     MBRegisterAddress;
	union   CharsAndWord     uData;




    Tx1NBBBuff[0] = g_cNBBChannelAddr;
	Tx1NBBBuff[1] = g_cNBBMBRx1Func;
	numbytes = 2 * Rx1NBBBuff[5];    // bytes to return
	nNumwords = Rx1NBBBuff[5];
	// if(numbytes > MAX_TX_BYTES)
//        return(DEVICE_FAILURE); // max bytes that can be sent is 80 (PIC memory issue)

	Tx1NBBBuff[2] = numbytes;    // bytes to return
	MBRegisterAddress.cValue[0] = Rx1NBBBuff[2];
	MBRegisterAddress.cValue[1] = Rx1NBBBuff[3];

	AddressTemp = MBRegisterAddress.nValue ;

  MBRegisterIndex = AddressTemp;  //

  	if((MBRegisterIndex + nNumwords) > MB_TABLE_SIZE)		// check that highest address is still in MB table.
        		return(ILLEGAL_MB_ADDRESS);



   for(j=0; j<nNumwords; j++)
   {
           k = MBRegisterIndex + j;
           uData.nValue = g_arrnMBTable[k];       // move data from modbus table
           Tx1NBBBuff[i] = uData.cValue[0];       // move data from modbus table
           i++;
           Tx1NBBBuff[i] = uData.cValue[1];       // move data from modbus table
           i++;
 	}

   g_nTx1NBBPtr = i;                                     // tx1ptr point to next vacant byte
   return(0);

}

/////end or readPanelnwrd/////////////////////////

//////////////////////////////////////////////////////
// writePanelnwrd( void )
//
// Function : write n word command.
//
//////////////////////////////////////////////////////


char writePanelnwrd(void)
{
    unsigned char nNumbytes,nNumwords;                   //,BlockStartAddr, MaxWriteAddr
	unsigned char j,cErrorCode;     // kb,x,
	WORD	k;
	WORD i=7;                       // point to 7th byte  in Rx1buff
	WORD MBByteIndex;
	union   CharsAndWord     MBRegisterAddress;
	union   CharsAndWord     uData;
    PrintModbusMessage(Rx1NBBBuff);
    g_bPanelWriteHasHappened = TRUE;
    g_nWritenwrdCounter++;

    cErrorCode = 0;

	Tx1NBBBuff[0] = g_cNBBChannelAddr;
	Tx1NBBBuff[1] = g_cNBBMBRx1Func;
	Tx1NBBBuff[2] = Rx1NBBBuff[2];
	Tx1NBBBuff[3] = Rx1NBBBuff[3];               // address of 1st word
	Tx1NBBBuff[4] = Rx1NBBBuff[4];
	Tx1NBBBuff[5] = Rx1NBBBuff[5];               // no. of words to  write

	nNumbytes = Rx1NBBBuff[6];    // bytes to write
	nNumwords = Rx1NBBBuff[5];    // words to write

	MBRegisterAddress.cValue[0] = Rx1NBBBuff[2];
	MBRegisterAddress.cValue[1] = Rx1NBBBuff[3];


	MBByteIndex = (MBRegisterAddress.nValue);    // word address in MB Table

//	MaxWriteAddr = 0 + MBBLOCKWSIZ*2;

    if((MBByteIndex + nNumwords) > BATCH_LAST_WRITE_REGISTER)		// last writeable addres.
	{
		return(ILLEGAL_MB_DATA_ACCESS);    // error, illegal address
	}
	else
	if(g_arrnMBTable[BATCH_SETPOINT_DOWNLOAD_MANAGER] == ALLOW_DOWNLOAD)
    {
        cErrorCode = 0;
		return(cErrorCode);
    }
    else
	{

//	Distribute the required data

	if(g_arrnMBTable[BATCH_SETPOINT_DOWNLOAD_MANAGER] != INHIBIT_DOWNLOAD)
    {
        g_nMBWriteOrFlagTemp = 0;
    }

	for(j = 0; j < nNumwords; j++)
		{
         k = MBByteIndex + j;
		  	uData.cValue[0]= Rx1NBBBuff[i];          				// read msb
//        checkcheck  i++;
		  	uData.cValue[1] = Rx1NBBBuff[i+1];         			// read lsb
            if((k < BATCH_PANEL_VERSION_NUMBER)||(k > BATCH_PANEL_VERSION_NUMBER+3)) // exclude panel version write
            {
                RecordChangedModbusData(k,g_arrnMBTable[k],uData.nValue,TRUE);
		  	}
            g_arrnMBTable[k] = uData.nValue; 						// move data to modbus table
		  	g_nMBWriteOrFlagTemp |= g_arrnMBTableFlags[k];	// set the approp. bit by oring with the corresponding MB flag.
        	i += 2;
        	if(k == BATCH_CONTROL_LINE_SPEED)
        	{
              	g_bModbusLineSpeedWritten = TRUE;
        	}
            if(k == BATCH_CONTROL_WRITE_DAC)
            {
                g_bModbusDToAWritten = TRUE;
            }
             if(k == BATCH_DTOA_PERCENTAGE)
            {
                g_bModbusDToAPercentageWritten = TRUE;
            }
            if(k == TOGGLE_STATUS_COMMAND2_LIQUID_ADDITIVE_COMMAND_SET_DTOA)
            {
                g_bModbusLiquidDToAWritten = TRUE;
            }
            if(k == TOGGLE_STATUS_COMMAND2_LIQUID_ADDITIVE_COMMAND_SET_DTOA_PERCENTAGE)
            {
                g_bModbusLiquidDToAPercentageWritten = TRUE;
            }

            if(k == TOGGLE_STATUS_COMMAND2_LENGTH)
            {
                g_bModbusLengthWritten = TRUE;
            }
            if(k == BATCH_CONTROL_SCREW_SPEED)
            {
                g_bModbusScrewSpeedWritten = TRUE;
            }
            if((k >= VACUUM_LOADER_START_CALIBRATION_BLOCK) && (k <= VACUUM_LOADER_END_CALIBRATION_BLOCK))
            {
                g_bVacuumLoaderCalibrationWritten = TRUE;
            }
            if((k >= BATCH_ICS_RECIPE_START) && (k <= BATCH_ICS_RECIPE_END))
            {
                g_bICSRecipeDataWritten = TRUE;
            }
            if(k == BATCH_PANEL_VERSION_NUMBER)
            {
                g_bLockIsEnabled = TRUE;
            }
            if(k == BATCH_SET_LICENSE_OPTIONS_1)
            {
                g_bLicenseOptions1Written = TRUE;
            }
            if(k == BATCH_SET_LICENSE_OPTIONS_2)
            {
                g_bLicenseOptions2Written = TRUE;
            }
            if(k == BATCH_SET_LICENSE_OPTIONS_3)
            {
            	g_bLicenseOptions3Written = TRUE;
            }
            if(k == BATCH_SET_LICENSE_OPTIONS_4)
            {
            	g_bLicenseOptions4Written = TRUE;
            }
            if((k >= MB_WIDTH_CONFIG_START) && (k <= MB_WIDTH_CONFIG_END))
            {
                g_bWidthConfigWritten = TRUE;
            }
            if(k == WIDTH_CONFIG_CALIBRATE_WIDTH)
            {
            	g_bWidthCalibrateInConfigWritten = TRUE;
            }
            if(k == WIDTH_CONFIG_MAXIMUM_WIDTH)
            {
            	g_bMaxWidthCalibrateInConfigWritten = TRUE;
            }
            if(k == WIDTH_COMMAND_AUTO_TOGGLE_COMMAND)
            {
            	g_bWidthAutoManualToggleCommandWritten = TRUE;
            }
            if(k == WIDTH_COMMAND_INCREASE)
            {
            	g_bWidthIncreaseCommandWritten = TRUE;
            }
            if(k == WIDTH_COMMAND_FAST_CORRECTION_INCREASE)
            {
            	g_bWidthIncreaseFastCorrectionCommandWritten = TRUE;
            }
            if(k == WIDTH_COMMAND_DECREASE)
            {
            	g_bWidthDecreaseCommandWritten = TRUE;
            }
            if(k == WIDTH_COMMAND_CALIBRATE_ULTRASONIC_REF)
            {
            	g_bWidthUltrasonicRefCommandWritten = TRUE;
            }
            if(k == WIDTH_COMMAND_CALIBRATE_DIAMETER)
            {
            	g_bWidthUltrasonicDiameterCommandWritten = TRUE;
            }
            if(k == WIDTH_COMMAND_US_LAYFLAT_OFFSET_SIGN)
            {
            	g_bWidthUltrasonicLayFlatOffsetSignWritten = TRUE;
            }
            if(k == WIDTH_COMMAND_US_LAYFLAT_OFFSET)
            {
           	    g_bWidthUltrasonicLayFlatOffsetWritten = TRUE;
            }
            if(k == WIDTH_COMMAND_US_DIAMETER_FROM_LAYFLAT_ENTRY)
            {
            	g_bWidthUltrasonicDiameterFromLayFlatEntry = TRUE;
            }
            if(k == WIDTH_COMMAND_WIDTH_SETPOINT)
            {
             	g_bWidthSetPointWritten = TRUE;
            }
		}


		g_nMBWriteOrFlagTemp &= 0x7FFF;						// clear the m.s. bit.

        if((g_arrnMBTable[BATCH_SETPOINT_DOWNLOAD_MANAGER] != INHIBIT_DOWNLOAD) && (g_arrnMBTable[BATCH_SETPOINT_DOWNLOAD_MANAGER] != ALLOW_DOWNLOAD) )
        {
              cErrorCode = CheckMBValidData();						// some checks to see if data written is valid. (asm = CHMBVDATA)
        }
        else
        {
            cErrorCode = 0;
        }
		g_nTx1NBBPtr = 6;                              // tx1ptr point to next vacant byte
		return(cErrorCode);
	}
}



///// ROUTINE TO write 1 WORD////////////////////////////////
char writePanel1wrd(void)
{


	unsigned char nNumbytes,nNumwords;                   //,BlockStartAddr,MaxWriteAddr
	unsigned char j,cErrorCode;  //kb,x,
	WORD	k;
	WORD i=4;                       // point to 7th byte  in Rx1buff
	WORD MBByteIndex;
	union   CharsAndWord     MBRegisterAddress;
	union   CharsAndWord     uData;

    PrintModbusMessage(Rx1NBBBuff);

    g_bPanelWriteHasHappened = TRUE;
    cErrorCode = 0;

	Tx1NBBBuff[0] = g_cNBBChannelAddr;
	Tx1NBBBuff[1] = g_cNBBMBRx1Func;
	Tx1NBBBuff[2] = Rx1NBBBuff[2];
	Tx1NBBBuff[3] = Rx1NBBBuff[3];               // address of 1st word
	Tx1NBBBuff[4] = Rx1NBBBuff[4];
	Tx1NBBBuff[5] = Rx1NBBBuff[5];               // no. of words to  write

	nNumbytes = 2;              // bytes to write
	nNumwords = 1;             // words to write

	MBRegisterAddress.cValue[0] = Rx1NBBBuff[2];
	MBRegisterAddress.cValue[1] = Rx1NBBBuff[3];


	MBByteIndex = (MBRegisterAddress.nValue);    // word address in MB Table

    if((MBByteIndex + nNumwords) > BATCH_LAST_WRITE_REGISTER)
	{
		return(ILLEGAL_MB_DATA_ACCESS);    // error, illegal address
	}
	else

	{

//	Distribute the required data
	if(g_arrnMBTable[BATCH_SETPOINT_DOWNLOAD_MANAGER] != INHIBIT_DOWNLOAD)
	{
 	    g_nMBWriteOrFlagTemp = 0;
    }
	for(j=0; j < nNumwords; j++)
		{
        k = MBByteIndex + j;

		  uData.cValue[0]= Rx1NBBBuff[i];          // read msb
//          i++;
		  uData.cValue[1] = Rx1NBBBuff[i+1];          // read lsb
          if((k < BATCH_PANEL_VERSION_NUMBER)||(k > BATCH_PANEL_VERSION_NUMBER+3)) // exclude panel version write
          {
              RecordChangedModbusData(k,g_arrnMBTable[k],uData.nValue,TRUE);
		  }
          g_arrnMBTable[k] = uData.nValue; // move data to modbus table
		  g_nMBWriteOrFlagTemp |= g_arrnMBTableFlags[k];	// set the approp. bit by oring with the corresponding MB flag.
        i += 2;
		  if(k == BATCH_CONTROL_LINE_SPEED)
           {
            g_bModbusLineSpeedWritten = TRUE;
            }
            if(k == BATCH_CONTROL_WRITE_DAC)
            {
                g_bModbusDToAWritten = TRUE;
            }
             if(k == BATCH_DTOA_PERCENTAGE)
            {
                g_bModbusDToAPercentageWritten = TRUE;
            }
            if(k == TOGGLE_STATUS_COMMAND2_LIQUID_ADDITIVE_COMMAND_SET_DTOA)
            {
                g_bModbusLiquidDToAWritten = TRUE;
            }
            if(k == TOGGLE_STATUS_COMMAND2_LIQUID_ADDITIVE_COMMAND_SET_DTOA_PERCENTAGE)
            {
                g_bModbusLiquidDToAPercentageWritten = TRUE;
            }

            if(k == TOGGLE_STATUS_COMMAND2_LENGTH)
            {
                g_bModbusLengthWritten = TRUE;
            }
            if(k == BATCH_CONTROL_SCREW_SPEED)
            {
                g_bModbusScrewSpeedWritten = TRUE;
            }
            if((k >= VACUUM_LOADER_START_CALIBRATION_BLOCK) && (k <= VACUUM_LOADER_END_CALIBRATION_BLOCK))
            {
                g_bVacuumLoaderCalibrationWritten = TRUE;
            }
            if((k >= BATCH_ICS_RECIPE_START) && (k <= BATCH_ICS_RECIPE_END))
            {
                g_bICSRecipeDataWritten = TRUE;
            }

            if(k == BATCH_SET_LICENSE_OPTIONS_1)
            {
                g_bLicenseOptions1Written = TRUE;
            }
            if(k == BATCH_SET_LICENSE_OPTIONS_2)
            {
                g_bLicenseOptions2Written = TRUE;
            }
            if(k == BATCH_SET_LICENSE_OPTIONS_3)
            {
            	g_bLicenseOptions3Written = TRUE;
            }
            if(k == BATCH_SET_LICENSE_OPTIONS_4)
            {
            	g_bLicenseOptions4Written = TRUE;
            }
            if((k >= MB_WIDTH_CONFIG_START) && (k <= MB_WIDTH_CONFIG_END))
            {
                 g_bWidthConfigWritten = TRUE;
            }
            if(k == WIDTH_CONFIG_CALIBRATE_WIDTH)
            {
             	g_bWidthCalibrateInConfigWritten = TRUE;
            }
            if(k == WIDTH_COMMAND_AUTO_TOGGLE_COMMAND)
            {
            	g_bWidthAutoManualToggleCommandWritten = TRUE;
            }
            if(k == WIDTH_COMMAND_INCREASE)
            {
            	g_bWidthIncreaseCommandWritten = TRUE;
            }
            if(k == WIDTH_COMMAND_FAST_CORRECTION_INCREASE)
            {
            	g_bWidthIncreaseFastCorrectionCommandWritten = TRUE;
            }
            if(k == WIDTH_COMMAND_DECREASE)
            {
            	g_bWidthDecreaseCommandWritten = TRUE;
            }
            if(k == WIDTH_COMMAND_CALIBRATE_ULTRASONIC_REF)
            {
             	g_bWidthUltrasonicRefCommandWritten = TRUE;
            }
            if(k == WIDTH_COMMAND_CALIBRATE_DIAMETER)
            {
            	g_bWidthUltrasonicDiameterCommandWritten = TRUE;
            }
            if(k == WIDTH_COMMAND_US_LAYFLAT_OFFSET_SIGN)
            {
            	g_bWidthUltrasonicLayFlatOffsetSignWritten = TRUE;
            }
            if(k == WIDTH_COMMAND_US_LAYFLAT_OFFSET)
            {
            	g_bWidthUltrasonicLayFlatOffsetWritten = TRUE;
            }
            if(k == WIDTH_COMMAND_US_DIAMETER_FROM_LAYFLAT_ENTRY)
            {
            	g_bWidthUltrasonicDiameterFromLayFlatEntry = TRUE;
            }
            if(k == WIDTH_COMMAND_WIDTH_SETPOINT)
            {
            	g_bWidthSetPointWritten = TRUE;
            }


		}


		g_nMBWriteOrFlagTemp &= 0x7FFF;						// clear the m.s. bit.

        if(g_arrnMBTable[BATCH_SETPOINT_DOWNLOAD_MANAGER] != INHIBIT_DOWNLOAD)  //nbb--todo-- change this
        {
		    cErrorCode = CheckMBValidData();						// some checks to see if data written is valid. (asm = CHMBVDATA)
        }
        else
        {
            cErrorCode = 0;
        }

		g_nTx1NBBPtr = 6;                            // tx1ptr point to next vacant byte

		return(cErrorCode);

	}
}
	//

/////end of writePanel1wrd//////////////////////


void PrintPanelModbusMessage( void )
{

        char    cBuffer[MAX_PRINT_WIDTH+1];
        unsigned int i;

        g_cPrinterBuffer[0] = 0x0;
        sprintf( cBuffer, "\n ");
        strcat( cBuffer, strAutomatic[0] );
        strcat( g_cPrinterBuffer, cBuffer);
       for(i=0; i<100; i++)
        {
            sprintf( cBuffer, " %02x ", Rx1NBBBuff[i]);
            strcat( g_cPrinterBuffer, cBuffer);

        }
        strcat( g_cPrinterBuffer, strNewLine );
        printf("%s",g_cPrinterBuffer);  //nbb--testonly--
}













