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
//			Diagnostic for write 1 word CopyRx1Buff[j] = Rx1Buff[j] and CopyTx1Buff[j] = Tx1Buff[j]


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
// P.Smith                              14/9/06
// name change to make the functions to make them more generic.
//#include "InitNBBComms.h"
//
// P.Smith                              14/11/06
// In write word commands set g_bModbusLineSpeedWritten, g_bModbusDToAWritten
// g_bModbusDToAPercentageWritten

//
// P.Smith                              15/1/07
// Remove all references to Duart port.
//
// P.Smith                              2/2/07
// Remove unused functions
// Name changes to make function more generic
//
// P.Smith                              6/2/07
// remove InitialiseSerialPortB
//
// P.Smith                              6/6/07
// if BATCH_SETPOINT_DOWNLOAD_MANAGER is not INHIBIT_DOWNLOAD,
// set g_nMBWriteOrFlagTemp to 0
// call CheckMBValidData if BATCH_SETPOINT_DOWNLOAD_MANAGER is not INHIBIT_DOWNLOAD
// else set the error code to 0
//
// P.Smith                              22/6/07
// added m_lChecksumErrorCounter for network.
//
// P.Smith                              6/7/07
// g_bModbusLengthWritten set if length is written to modbus table
// M.McKiernan                      17/9/07
// Removed g_arrnMBTable[] & g_arrnMBTableFlags[], now in BatVars2
//       g_nMBWriteOrFlag, g_nMBWriteOrErrorFlag, g_nMBWriteOrFlagTemp
//
// P.Smith                              23/11/07
// added BATCH_CONTROL_SCREW_SPEED check in writenwrd & write1wrd, g_bModbusScrewSpeedWritten flag
// is set if this word is written.
//
// P.Smith                              31/1/08
// call TakeACopyOfModbusSetpointData from write n/1 word function to take a copy
// of the modbus table on write to blender.
//
// P.Smith                              23/4/08
// Remove TakeACopyOfModbusSetpointData, check if the data written is within the sp area
// if it is only copy that particular data.
//
// P.Smith                          23/7/08
// remove g_arrnMBTable extern
//
// P.Smith                          26/9/08
// added d/a written commands check on writen and write1 word.
//
// P.Smith                          14/11/08
// call PrintModbusMessage to print modbus to telnet
//
// P.Smith                          8/12/08
// added g_sOngoingHistoryNetworkComms.m_lChecksumErrorCounter, m_lGoodRx
//
// P.Smith                          16/6/09
// allow for download of data for vacuum loading set point data
// check for vacuum loader block and set g_bVacuumLoaderCalibrationWritten
// this will then initiate the download of the data to eeprom.
//
// P.Smith                          22/7/09
// added g_bICSRecipeDataWritten set
//
// P.Smith                          1/9/09
// call RecordChangedModbusData to record change in modbus table in writen 1wrd
//
// P.Smith                          15/4/10
// added check for BATCH_SET_LICENSE_OPTIONS_1-4 written and set flag indicating write
//
// P.Smith                          13/5/10
// added check for Width Config data written and set flags to indicate that the write has
// happened
//
// P.Smith                          7/7/10
// added width ultrasonic ref and diameter commands
//
// P.Smith                          19/7/10
// added offset sign,offset,diameter from lay flat entry
//
// P.Smith                          16/9/10
// added width set point command
////////////////////////////////////////////////////////////////


/*

The main modbus table is an arrary of integers.  data is writeable (from the external MB master).

To indicate that a write has happened, a flag should be set for the corresponding MB table of flags, one flag per writeable register.

The background program will have to check if data has been written to the MB table (ie. g_nMBWriteOrFlag set??), and then check which data has been written and react accordingly.
*/


#include <Stdio.h>

// #include "Hardware.h"
 #include "General.h"
#include "SerialStuff.h"

#include <basictypes.h>

#include "MBSHand.h"
#include "BatchMBIndices.h"

#include "InitComms.h"
#include "NBBGpio.h"

//#include "CalculateModbusChecksum.h"
#include "MBProgs.h"
#include "CalculateModbusCRC.h"
#include "Batvars2.h"
#include "InitNBBComms.h"
#include "PrntDiag.h"
#include "HistoryLog.h"
#include "Vacvars.h"
#include "WidthMB.h"
#include "WidthVars.h"


extern  int NBBBoardAddress;
extern int g_nFlashRate;		// LED flash rate period in PIT ticks.
extern BOOL	g_bLicenseOptions1Written;
extern BOOL	g_bLicenseOptions2Written;
extern BOOL	g_bLicenseOptions3Written;
extern BOOL	g_bLicenseOptions4Written;


    int g_nMBSlaveCommsWatchDogTimer;
	BYTE g_cReceiveBEnabled = 0;
	BYTE g_cTransmitBEnabled = 0;



extern int g_nFlashRate;		// LED flash rate period in PIT ticks.
       structCommsData         g_sModbusNetworkSlave;



unsigned char	Rx1Buff[RX1BUFSIZE];
unsigned char	Tx1Buff[TX1BUFSIZE];
//char	CopyRx1Buff[8];
//char	CopyTx1Buff[8];

//unsigned char g_cTx1Ptr = 0;
//unsigned char g_cRx1Ptr = 0;
WORD g_nTx1Ptr = 0;
WORD g_nRx1Ptr = 0;
unsigned char g_cChannelAddr;
unsigned char g_cMB1txdlytime=0;


//unsigned char g_nRx1ByteNo;
WORD g_nRx1ByteNo;
//unsigned char g_nMBRx1Bytes;
WORD g_nMBRx1Bytes;
//unsigned char g_nMBTx1Bytes;
WORD g_nMBTx1Bytes;
char g_cMBRx1Func;
BOOL  g_bBlenderSelected = FALSE;
WORD  g_nMB1BreakCounter=0;
char g_cMB1BreakFlag = 1;                  // set break flag on power-up
WORD  g_nCRCErrorCounter = 0;
WORD	g_nSlaveGoodRxCounter = 0;
WORD	g_nSlaveGoodTxCounter = 0;
unsigned char g_cRx1Data,g_cErrorCode;
char g_cTx1OffFlag = 0;            // flag to have Tx1 switched off when TSR empty.
char g_cTx1ModeFlag = 0;
char g_cLastChTimer = 0;
//char g_cTransmitBEnabled = 0;
//char g_cReceiveBEnabled = 0;
//char g_cVAC350MBUpdateFlag = 0;
//char g_arrVAC350MBUpdates[10 + VAC350_MAX_LOADERS];


//int g_arrnMBTable[VAC350MBTABLESIZE];


void NetworkTransmitterOn( void )
{
	 	NetworkTxOn();
	 	g_cTransmitBEnabled = 1;
	 	g_cReceiveBEnabled = 0;
}




void NetworkReceiverOn( void )  //nbb--todo-- review this
{
    NetworkRxOn();
	g_cReceiveBEnabled = 1;
	g_cTransmitBEnabled = 0;

}




void ResetMBSlaveComms(void)
{
    InitialiseNetworkPort();
	Resetmbrx1();
}

void Resetmbrx1(void)
{
        g_nRx1Ptr = 0;
        g_nRx1ByteNo = 0;
        g_nMBRx1Bytes = 0; // no of bytes expected
        g_cMBRx1Func = 0;
        g_bBlenderSelected = FALSE;
}


//////GetMBBytes /////////////////////////////////////////////////
//////GetMBBytes gets no. of bytes in modbus message//////////////
void GetMBBytes(void)
{
	g_nMBRx1Bytes = 8;                        // leave at 8 for unused messages
	if(g_cMBRx1Func == 7 )                    // status read, 4 bytes
			g_nMBRx1Bytes = 4;
	if(g_cMBRx1Func == 3 || g_cMBRx1Func == 4 || g_cMBRx1Func == 6 ) //3/4read n words, 6 write 1 word, 8 bytes
        g_nMBRx1Bytes = 8;
	if(g_cMBRx1Func == 16 )                   // write n words
		{
        		if (g_nRx1ByteNo <= 7)
                	g_nMBRx1Bytes = 10;        // temp value, min of 10 bytes in write n words
        		else
                	g_nMBRx1Bytes = 9 + 2*Rx1Buff[5];  //Rxbuff[5] has no. of words.
		}
}




void SaveRxdCh(void)

{
	union   CharsAndWord     Rxed1CRC;
	union   CharsAndWord     uTxChecksum ;
	WORD		CRCChecksum;
//	int		j;



 if(g_cMB1BreakFlag)
  {
        g_cMB1BreakFlag = 0;
        Resetmbrx1();
        if(g_cRx1Data == g_cBlenderNetworkAddress)   // addr on SBB pcb

        {
                g_bBlenderSelected = TRUE;                //this blender being addressed g_cChannelAddr = g_cRx1Data
                Rx1Buff[g_nRx1Ptr] = g_cRx1Data;      // store byte in buffer
                g_cChannelAddr = g_cRx1Data;
//                CommsChannelNum = 0;  //loader channel no. (0-7)
                g_nRx1ByteNo++;
                g_nRx1Ptr++;
                return;
        }
        else
        {
                g_bBlenderSelected = FALSE;                //this IVR not addressed
                return;         // do nothing if not address
        }
  }  // end of break detect

 		if(!g_bBlenderSelected)          		// finished if unit not addressed
			return;
 		Rx1Buff[g_nRx1Ptr] = g_cRx1Data;      // (SAVRXBYT)store byte in buffer
 		g_nRx1ByteNo++;
 		g_nRx1Ptr++;
 		if(g_nRx1ByteNo == 2)
 		{
         g_cMBRx1Func = g_cRx1Data;
         return;
 		}
 		if(g_nRx1ByteNo < 4)
           return;
  		if(g_nRx1ByteNo == 9 || g_nRx1ByteNo == 4)
  		{
          GetMBBytes();           // get no of bytes expected in message
  		}

  		if(g_nRx1ByteNo != g_nMBRx1Bytes)     // check if last byte
                  return;


//// last byte
		CRCChecksum = CalculateModbusCRC( Rx1Buff, g_nRx1Ptr-2);
//		CRCChecksum = CalculateModbusCRC( Rx1Buff, g_nRx1Ptr-2);



		Rxed1CRC.cValue[0] = Rx1Buff[g_nRx1Ptr-1];         // l.s. byte
		Rxed1CRC.cValue[1] = Rx1Buff[g_nRx1Ptr-2];
		if(Rxed1CRC.nValue != CRCChecksum)             // CRC in error ?
		{
		        g_nCRCErrorCounter++;
                g_sModbusNetworkSlave.m_lChecksumErrorCounter++;
                g_sOngoingHistoryNetworkComms.m_lChecksumErrorCounter++;

                return;
		}
		else
		{
			g_nSlaveGoodRxCounter++;				// increment good message counter.
 			g_sModbusNetworkSlave.m_lGoodRx++;
 			g_sOngoingHistoryNetworkComms.m_lGoodRx++;



		}


		g_nFlashRate = FLASH_RATE_10HZ;		// set to flash Heart beat LED at 10 Hz.

   	if(g_cMBRx1Func == 0x03 || g_cMBRx1Func == 0x04)     // check for read n words command
   	{
        g_cErrorCode = readnwrd();
        if(g_cErrorCode)
        {
                Tx1Buff[0] = g_cChannelAddr;
                Tx1Buff[1] = g_cMBRx1Func | 0x80;          // 8-03-02 set msb for error.
                Tx1Buff[2] = g_cErrorCode;
//                gencrc(3, &Tx1Buff);
			 uTxChecksum.nValue = CalculateModbusCRC( Tx1Buff,3);
                Tx1Buff[3] = uTxChecksum.cValue[1];
                Tx1Buff[4] = uTxChecksum.cValue[0];
                g_nMBTx1Bytes = 5;                 // transmit back 5 bytes
        }
        else                                    // must be ok
        {
//                gencrc( g_nTx1Ptr, &Tx1Buff);  // generate crc on Tx1buff

			 uTxChecksum.nValue = CalculateModbusCRC( Tx1Buff, g_nTx1Ptr);
                Tx1Buff[g_nTx1Ptr] = uTxChecksum.cValue[1];
                g_nTx1Ptr++;
                Tx1Buff[g_nTx1Ptr] = uTxChecksum.cValue[0];  // l.s. byte
                g_nMBTx1Bytes = g_nTx1Ptr+1;                 // no. of bytes to transmit back


        }
   	}


   	if(g_cMBRx1Func == 0x06)     // check for write 1 word
   	{
         g_cErrorCode = write1wrd();
        if(g_cErrorCode)          // error condition  if non zero
        {
                Tx1Buff[0] = g_cChannelAddr;
                Tx1Buff[1] = g_cMBRx1Func | 0x80;          // 8-03-02 set msb for error.
                Tx1Buff[2] = g_cErrorCode;
			 		uTxChecksum.nValue = CalculateModbusCRC( Tx1Buff,3);
                Tx1Buff[3] = uTxChecksum.cValue[1];
                Tx1Buff[4] = uTxChecksum.cValue[0];
                g_nMBTx1Bytes = 5;                 // transmit back 5 bytes
        }
        else                                    // must be ok
        {
//                gencrc( 6, &Tx1Buff);  // generate crc on Tx1buff
			 		uTxChecksum.nValue = CalculateModbusCRC( Tx1Buff,6);
                Tx1Buff[6] = uTxChecksum.cValue[1];
                Tx1Buff[7] = uTxChecksum.cValue[0];  // l.s. byte
                g_nMBTx1Bytes = 8;                 // no. of bytes to transmit back
        }
   	}


   if(g_cMBRx1Func == 0x10)     // check for write n words
   {
        g_cErrorCode = writenwrd();
        if(g_cErrorCode)          // error condition  if non zero
        {
                Tx1Buff[0] = g_cChannelAddr;
                Tx1Buff[1] = g_cMBRx1Func | 0x80;          // 8-03-02 set msb for error.
                Tx1Buff[2] = g_cErrorCode;
			 uTxChecksum.nValue = CalculateModbusCRC( Tx1Buff,3);
                Tx1Buff[3] = uTxChecksum.cValue[1];
                Tx1Buff[4] = uTxChecksum.cValue[0];
                g_nMBTx1Bytes = 5;                 // transmit back 5 bytes
        }
        else                                    // must be ok
        {
 //               gencrc( 6, &Tx1Buff);           // generate crc on Tx1buff
			 uTxChecksum.nValue = CalculateModbusCRC( Tx1Buff,6);
			 Tx1Buff[6] = uTxChecksum.cValue[1];
                Tx1Buff[7] = uTxChecksum.cValue[0];  // l.s. byte
                g_nMBTx1Bytes = 8;                   // no. of bytes to transmit back
        }

   } ////end of write n words check


	 	g_nRx1Ptr = 0;              // reset rx buff pointer.
     	g_nTx1Ptr = 0;              // reset tx buff pointer.

     if(g_nMBTx1Bytes)             // i.e. if bytes to transmit is non-zero
     {
        g_cMB1txdlytime = MBTX_DELAY;      //mbtx_delay depends on baud rate

/*   //diagnostic
    	   if(g_cMBRx1Func == 0x06)
	   	{
	   		for(j=0; j<8; j++)
				{
					CopyRx1Buff[j] = Rx1Buff[j];
					CopyTx1Buff[j] = Tx1Buff[j];
				}
	   	}
*/
     }

}




///// ROUTINE TO READ N WORDS
char readnwrd(void)
{
	unsigned char numbytes,nNumwords;
	unsigned char j;		//,y;
//	unsigned char i=3;                       // point to 4th byte
	WORD i = 3;			// point to 4th byte in buffer
//	unsigned char MBByteIndex,BlockStartAddr;
	WORD	MBRegisterIndex,k;
	WORD AddressTemp;    // ,AddressTemp2,ny;
	union   CharsAndWord     MBRegisterAddress;
	union   CharsAndWord     uData;


	Tx1Buff[0] = g_cChannelAddr;
	Tx1Buff[1] = g_cMBRx1Func;
	numbytes = 2 * Rx1Buff[5];    // bytes to return
	nNumwords = Rx1Buff[5];
	// if(numbytes > MAX_TX_BYTES)
//        return(DEVICE_FAILURE); // max bytes that can be sent is 80 (PIC memory issue)

	Tx1Buff[2] = numbytes;    // bytes to return
	MBRegisterAddress.cValue[0] = Rx1Buff[2];
	MBRegisterAddress.cValue[1] = Rx1Buff[3];

	AddressTemp = MBRegisterAddress.nValue ;


  MBRegisterIndex = AddressTemp;  //

  	if((MBRegisterIndex + nNumwords) > MB_TABLE_SIZE)		// check that highest address is still in MB table.
  		return(ILLEGAL_MB_ADDRESS);




   for(j=0; j<nNumwords; j++)
   {
           k = MBRegisterIndex + j;
           uData.nValue = g_arrnMBTable[k];       // move data from modbus table
           Tx1Buff[i] = uData.cValue[0];       // move data from modbus table
           i++;
           Tx1Buff[i] = uData.cValue[1];       // move data from modbus table
           i++;
 	}

   g_nTx1Ptr = i;                                     // tx1ptr point to next vacant byte
   return(0);

}

/////end or readnwrd/////////////////////////



char writenwrd(void)
{

    unsigned char nNumbytes,nNumwords;                   //,BlockStartAddr, MaxWriteAddr
	unsigned char j,cErrorCode;     // kb,x,
	WORD	k;
	WORD i=7;                       // point to 7th byte  in Rx1buff
	WORD MBByteIndex;
	union   CharsAndWord     MBRegisterAddress;
	union   CharsAndWord     uData;
    cErrorCode = 0;
   PrintModbusMessage(Rx1Buff);

	Tx1Buff[0] = g_cChannelAddr;
	Tx1Buff[1] = g_cMBRx1Func;
	Tx1Buff[2] = Rx1Buff[2];
	Tx1Buff[3] = Rx1Buff[3];               // address of 1st word
	Tx1Buff[4] = Rx1Buff[4];
	Tx1Buff[5] = Rx1Buff[5];               // no. of words to  write

	nNumbytes = Rx1Buff[6];    // bytes to write
	nNumwords = Rx1Buff[5];    // words to write

	MBRegisterAddress.cValue[0] = Rx1Buff[2];
	MBRegisterAddress.cValue[1] = Rx1Buff[3];


	MBByteIndex = (MBRegisterAddress.nValue);    // word address in MB Table
//	MaxWriteAddr = 0 + MBBLOCKWSIZ*2;

    if((MBByteIndex + nNumwords) > BATCH_LAST_WRITE_REGISTER)		// last writeable addres.
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



	for(j = 0; j < nNumwords; j++)
		{
         k = MBByteIndex + j;

		  	uData.cValue[0]= Rx1Buff[i];          				// read msb
		  	uData.cValue[1] = Rx1Buff[i+1];         			// read lsb
		  	RecordChangedModbusData(k,g_arrnMBTable[k],uData.nValue,FALSE);
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

       if(g_arrnMBTable[BATCH_SETPOINT_DOWNLOAD_MANAGER] != INHIBIT_DOWNLOAD)
        {
		    cErrorCode = CheckMBValidData();						// some checks to see if data written is valid. (asm = CHMBVDATA)
        }
        else
        {
            cErrorCode = 0;
        }


		g_nTx1Ptr = 6;                              // tx1ptr point to next vacant byte
//		g_cVAC350MBUpdateFlag = 1;		// indicate the write modbus command is initiated
		return(cErrorCode);
	}

}



///// ROUTINE TO write 1 WORD////////////////////////////////
char write1wrd(void)
{
	unsigned char nNumbytes,nNumwords;                   //,BlockStartAddr,MaxWriteAddr
	unsigned char j,cErrorCode;  //kb,x,
	WORD	k;
	WORD i=4;                       // point to 7th byte  in Rx1buff
	WORD MBByteIndex;
	union   CharsAndWord     MBRegisterAddress;
	union   CharsAndWord     uData;
    cErrorCode = 0;
    PrintModbusMessage(Rx1Buff);


	Tx1Buff[0] = g_cChannelAddr;
	Tx1Buff[1] = g_cMBRx1Func;
	Tx1Buff[2] = Rx1Buff[2];
	Tx1Buff[3] = Rx1Buff[3];               // address of 1st word
	Tx1Buff[4] = Rx1Buff[4];
	Tx1Buff[5] = Rx1Buff[5];               // no. of words to  write

	nNumbytes = 2;              // bytes to write
	nNumwords = 1;             // words to write

	MBRegisterAddress.cValue[0] = Rx1Buff[2];
	MBRegisterAddress.cValue[1] = Rx1Buff[3];


	MBByteIndex = (MBRegisterAddress.nValue);    // word address in MB Table
//	MaxWriteAddr = 0 + MBBLOCKWSIZ*2;
    k = MBByteIndex;
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
		  uData.cValue[0]= Rx1Buff[i];          // read msb
//          i++;
		  uData.cValue[1] = Rx1Buff[i+1];          // read lsb
		  RecordChangedModbusData(k,g_arrnMBTable[k],uData.nValue,FALSE);
          g_arrnMBTable[k] = uData.nValue; // move data to modbus table
		  g_nMBWriteOrFlagTemp |= g_arrnMBTableFlags[k];	// set the approp. bit by oring with the corresponding MB flag.
          i += 2;
    }
		g_nMBWriteOrFlagTemp &= 0x7FFF;						// clear the m.s. bit.
       if(g_arrnMBTable[BATCH_SETPOINT_DOWNLOAD_MANAGER] != INHIBIT_DOWNLOAD)
        {
		    cErrorCode = CheckMBValidData();						// some checks to see if data written is valid. (asm = CHMBVDATA)
        }
        else
        {
            cErrorCode = 0;
        }

		g_nTx1Ptr = 6;                            // tx1ptr point to next vacant byte
//		g_cVAC350MBUpdateFlag = 1;						// indicate the write modbus command is initiated

		return(cErrorCode);

	}
}
	//

/////end of write1wrd//////////////////////
/*
///////////////////////////////////////////////////////////
// Calculates the polynomial checksum used by the Modbus
// protocol.
//
// Entry:
//		'buffer'	is a pointer to the tx or rx buffer
//					holding	the message data.
//		'count'		is the number of bytes in the buffer.
//					the checksum calculation should include.
//	Exit:
//		returns		the calculated checksum.
///////////////////////////////////////////////////////////
WORD	CalculateModbusCRC( unsigned char *buffer, WORD count )
{
	WORD		nChecksum = 0xffff;
	char	*ptr;
	WORD		i;

	// calculate the checksum

	ptr = (char*)&nChecksum;

	// calculate the checksum

	for( i = 0; i < count; i++ )
	{
		WORD	j;

		// EXOR the data with the msb of the checksum
		*(ptr + 1) ^= *(buffer + i);

		for( j = 0; j < 8; j++ )
			if( nChecksum & 0x0001 )
			{
				nChecksum >>= 1;
				nChecksum &= 0x7fff;
				nChecksum ^= 0xa001;
			}
			else
			{
				nChecksum >>= 1;
				nChecksum &= 0x7fff;
			}
	}

	return nChecksum;
}


*/
















