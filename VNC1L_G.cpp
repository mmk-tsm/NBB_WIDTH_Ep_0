/*----------------------------_---------------------------------
 File: VNC1L_G.cpp
 Description: Read/Write VNC1L functions - building on Gurundur Singh's assembler routines.
 
//
// P.Smith                      23/12/08
// added timeouts to usb functionality.
// correct some compiler warnings
//
// P.Smith                      12/2/09
// added timeout counters to show when a particular function has timed out.
// add telnet debug to check g_nTSMDebug.m_bUSBPrinterDebug to allow telnet 
// diagnostics.
//
// P.Smith                      13/2/09
// allow diagnostics data to be printed out to telnet for usb printer.         
// it was necessary to increase USB_Write timeout to 4 x long timeout to 
// allow it to work consistently. 
//
// P.Smith                      26/2/09
// added some test software 
//
// P.Smith                      3/12/09
// problem found by coverity,in StartUSBPrint, use USB_TX_BUF_SIZE-2
////////////////////////////////////////////////////

-------------------------------------------------------------------*/
#include <startnet.h>

#include <basictypes.h>
#include <stdio.h> 

#include "QSPIconstants.h"
#include "QSPIdriver.h"

#include "VNC1L.h"

//#include <C:\Nburn\examples\NBBRevB1\NBBGpio.h>
#include "NBBGpio.h"
#include <serial.h>

#include <string.h>
#include "strlib.h"

#include "VNC1L_G.h"
#include "OneWire.h"
#include "BatVars2.h"
#include "BatVars.h"
#include "General.h"


//#include "EEPROMconstants.h"
//#include "EEPROMdriver.h"
extern WORD g_nEX1SPIChipSelect;

/*
USB_COMMANDS:
;         FCB        $85,$20,$00,$0D,0  ; QUERY DEVICE1
;         FCB        $85,$20,$01,$0D,0  ; QUERY DEVICE 2
         
         FCB        2,$2B,$0D       ; query port 1
         FCB        2,$2C,$0D       ; query port 2
         
DEVICE_READ:
         FCB        2,$84,$0D       ; DEVICE READ DATA

DEVICE_SET1:
         FCB        4,$86,$20,$00,$0D  ; SET CURRENT PORT1
DEVICE_SET2:
         FCB        4,$86,$20,$01,$0D  ; SET CURRENT PORT2 
         
DEVICE_SETINT:
         FCB        1,$30

USB_CMD_SHORT:
         FCB       4,'SCS',$0D   

HP_SETUP:
   FCB  8,$83,$20,4,$0D,$1B,'(0U'

HP_SETUP1:
   FCB  6,$83,$20,$02,$0D,$1B,'E'

*/

char cShortCmdSet[] = {'S','C','S',0x0D};
char cQueryPort1[] = {0x2B,0x0D};
char cQueryPort2[] = {0x2C,0x0D};
char cQueryDevice[] = {0x85,0x20,0x00,0x0D};    // the 3rd byte is the device no.

char cQueryDevice1[] = {0x85,0x20,0x00,0x0D};    // the 3rd byte is the device no. (think is 0 for device #1)

char cDeviceSet1[] = {0x86,0x20,0x00,0x0D};
char cDeviceSet2[] = {0x86,0x20,0x01,0x0D};
char cPrinterGetStatus[] = {0x81,0x0D};         //printer get status

char cPCL_SelfTest[] = {0x1B,0x7A};             //PCL = Self Test.

char cHPSetup1[] = {0x83,0x20,0x02,0x0D,0x1B,'E'};    //PCL = Reset
char cHPSetup[] = {0x83,0x20,0x04,0x0D,0x1B,'(','O','U'};   //PCL = AnSII ascii (ISO 6)
char cHPSetup12[] = {0x83,0x20,0x06,0x0D,0x1B,0x28,0x73,'1','2',0x48};   //PCL = pitch = 12 chars/inch
char cHPSetup14[] = {0x83,0x20,0x06,0x0D,0x1B,0x28,0x73,'1','4',0x48};   //PCL = pitch = 14 chars/inch

char cHPSetupBold[] = {0x83,0x20,0x05,0x0D,0x1B,0x28,0x73,0x33,0x42};   //PCL = Bold

char cDeviceSendData[] = {0x83,0x20,0x04,0x0D};

//char cDPrompt[] = {'D', ':', '\', '>', 0x0D};
char cDPrompt[] = {0x44, 0x3a, 0x5c, 0x3e, 0x0D};

BYTE g_cUSBReadBuffer[USB_READ_BUF_SIZE];
//bool g_bSPIBusBusy = FALSE; 
structUSB   g_sUSB;
extern CalDataStruct    g_CalibrationData;

extern  bool g_bXfrFinished;
extern  bool g_bFileXfrInProgress;
extern  bool g_bXfrFileReadFinished;
extern  bool g_bXfrBufBusy;

/***********************************************
*         USB CONTROLLER INIT
*
*  THIS ROUTINE INITIALIZES THE USB IC
*  AND SETS THE COMMAND MODE TO SHORT
*
**********************************************/

void USB_Init( void )
{
	int nUSBTimeOutCounter;
    
      // clear out VNC1L's transmit buffer before looking for status.
 /*     do{
            Delay_uS(1000);        // 1 mS delay.
            ReadAndParseUSBResponse();
         }
         while( g_sUSB.m_nCharsRead > 0 ); */
         
   FlushVNC1LTransmitter();     // clear out VNC1L's transmit buffer 
    
   if( g_sUSB.m_bOnlineDetected )
   {

//			nLen = strlen( strShortCmdSet  );

      // clear out VNC1L's transmit buffer before looking for status.
/*      do{
            Delay_uS(1000);        // 1 mS delay.
            ReadAndParseUSBResponse();
         }
         while( g_sUSB.m_nCharsRead > 0 );   */
      FlushVNC1LTransmitter();     // clear out VNC1L's transmit buffer 
 		USB_Write(  cShortCmdSet, 4 );	  //SET SHORT MESSAGE MODE
      nUSBTimeOutCounter = 0;
      
      do{
            Delay_uS(1000);        // 1 mS delay.
            ReadAndParseUSBResponse();
            nUSBTimeOutCounter++;
         }
         while(g_sUSB.m_nCharsRead == 0 && nUSBTimeOutCounter < 1000 ); // poll the VNC1L until it responds.
      
            //testonly.
      if( nUSBTimeOutCounter>1 )
      {
//            iprintf("\nUSBTO= %d ", nUSBTimeOutCounter); 		
//            for(i=0; i<g_sUSB.m_nCharsRead; i++)
//            {
//            iprintf( " i%02x", g_sUSB.m_ucReadBuf[i] ); 
//            }
         
 		}
 		
 		g_sUSB.m_bUSBInitialised = TRUE;
   }
}

/***********************************************
*         USB CONTROLLER INIT
*
*  THIS ROUTINE INITIALIZES THE USB IC
*  AND SETS THE COMMAND MODE TO SHORT
*
***********************************************
USB_INIT:
         JSR       USB_QUERY           ; CHECK FOR STARTUP MESSAGE
;         RTS
         
         LDY       #USB_CMD_SHORT      ; SET SHORT MESSAGE MODE
         JSR       USB_WRITE           ; SEND COMMAND TO USB IC
         RTS
*/

// Read the status of the VNC1L - return true if new data available.
bool ReadVNC1LStatus( void )
{

 WORD nTemp; 
 nTemp = VNC1LRead1Byte( VNC1L_SPI_CHIP_SELECT_REVB2, VNC1L_STATUS_REGISTER );

// if( nTemp & 0x0001 )	// if status bit is 1, implies data is old.
 if( nTemp & 0x0002 )	// Checking RXF bit in status register data
 	return( TRUE );
 else
 	return( FALSE );			// i.e new data available.
}

/***********************************************
*         USB CONTROLLER QUERY
*
*  THIS ROUTINE POLLS THE USB IC FOR ANY NEW
*  DATA
*
***********************************************/

void USB_Query( void )
{

//	nTemp = ReadUSBStatus();
	while( ReadVNC1LStatus() )  
	{
// ???		DelayuS(12);	 		// is a delay necessary?????
//		USB_Read();
//todo		USB_Parse();

	}

}

/***********************************************
*         USB CONTROLLER QUERY
*
*  THIS ROUTINE POLLS THE USB IC FOR ANY NEW
*  DATA
*
***********************************************
USB_QUERY:
         BSET       USBPORT,USBCS      ; ENABLE USB IC
         LDAA       #$E0               ; READ USB STATUS
         JSR        RUNSPI
         TAB                           ; SAVE FOR LATER
         
         JSR        RUNSPI
         BCLR       USBPORT,USBCS      ; DISABLE USB IC

         PSHD
         LDAB       #12
         DBNE       B,*
         PULD
         
         EXG        A,B                ; REVERSE DATA ORDER
         LSLD                          ; ALIGN BYTES
         LSLD
         LSLD
         BITA       #$02               ; NEW DATA AVAILABLE?
         BNE        USB_QX             ; NOT IF JUMP
         JSR        USB_READ           ; READ NEW MESSAGE
         JSR        USB_PARSE          ; PARSE NEW MESSAGE FOR COMMANDS
         BRA        USB_QUERY          ; LOOK FOR MORE COMMANDS
USB_QX:  RTS
*/



// USB controller read
int USB_Read(void)
{
int i;
BYTE nReadByte;
WORD nReadWord;
bool bDone = FALSE;
bool bNewData;

    nReadWord = VNC1LRead1Byte( VNC1L_SPI_CHIP_SELECT_REVB2, VNC1L_DATA_REGISTER );
	if(nReadWord & 0x0001) 
		bNewData = FALSE;		// status bit = 1, => data is old data
	else
	{
		bNewData = TRUE;
      // clear the read (receive) buffer
	   for(i=0; i<USB_READ_BUF_SIZE; i++)
		    g_sUSB.m_ucReadBuf[i] = 0;

   }
	nReadByte = BYTE(nReadWord >> 1);
    i=0;
	if(bNewData)
	{
   	g_sUSB.m_ucReadBuf[i] = nReadByte;
      i++;
      if(nReadByte == 0x0D)
		  	return(1);
      
   }
   else
   {
      return(0);
   }

   if( bNewData )    // is/was there new data available.
   {
          g_nUSBReadTimeoutInPits = USBTIMEOUT;
		  while(!bDone && (g_nUSBReadTimeoutInPits != 0))
		  {
               Delay_uS(5);      // 5 microsecond delay between SPI reads
		       
		  		nReadWord = VNC1LRead1Byte( VNC1L_SPI_CHIP_SELECT_REVB2, VNC1L_DATA_REGISTER );

				if(nReadWord & 0x0001) 
					bNewData = FALSE;		// status bit = 1, => data is old data
				else
					bNewData = TRUE;

				nReadByte = BYTE(nReadWord >> 1);


		  		if( nReadByte != g_sUSB.m_ucReadBuf[i-1] || bNewData )	  	//differnt from last data???
				{
					g_sUSB.m_ucReadBuf[i] = nReadByte;
					i++;
				   if( nReadByte == 0x0D )
						bDone = TRUE;

				}
//				else if( bNewData )
//				{
//					g_sUSB.m_ucReadBuf[i] = nReadByte;
//				}
				else 	// must be old data (repeat)
				{
					if( nReadByte == 0x0D )
						bDone = TRUE;
				}
		  }
		  if(g_nUSBReadTimeoutInPits == 0)
		  {
		      g_nUSBReadTimeoutCtr++;
		  }
   }
   return(i);     // return no. of chars read.
}

/***********************************************
*         USB CONTROLLER READ
*
*  THIS ROUTINE READS A MESSAGE FROM THE USB IC 
*
***********************************************
USB_READ:
         LDAA       #LEN_USBBUFF
         LDX        #USBBUFF
USB_R0:  MOVW       #0,2,X+
         DBNE       A,USB_R0
         LDX        #USBBUFF
USB_R1:
         BSET       USBPORT,USBCS      ; ENABLE USB IC
         LDAA       #$C0               ; READ USB DATA
         JSR        RUNSPI
         TAB                           ; SAVE FOR LATER
         
         JSR        RUNSPI
         BCLR       USBPORT,USBCS      ; DISABLE USB IC

         PSHD
         LDAB       #12
         DBNE       B,*
         PULD
         
         EXG        A,B                ; REVERSE DATA ORDER
         LSLD                          ; ALIGN BYTES
         LSLD
         LSLD
         CMPA       -1,X               ; SAME AS LAST DATA?			///THIS is a problem!!!!!!!!
         BNE        USB_R2             ; NOT IF JUMP
         BITB       #$80               ; OLD OR NEW DATA?
;         BNE        USB_RX             ; OLD DATA IF JUMP
         BEQ        USB_R2             ; NEW DATA IF JUMP
         CMPA       #$0D               ; END OF XMISSION?
         BEQ        USB_RX             ; YES IF JUMP
         BRA        USB_R1

USB_R2:  STAA       1,X+
         
         BRA        USB_R1             ; GET NEXT BYTE
         
USB_RX:  RTS
*/

//  THIS ROUTINE Writes A MESSAGE TO USB IC 
//  Command string pointer passed 
//	 SENDS NULL TERMINATED DATA TO VINCULUM

void USB_WriteCommand(  BYTE * pData )
{
 int i=0;
 WORD nTemp;

   while( *(pData+i) != 0x00)		// command string terminated by 0.
   {
		nTemp = VNC1LWrite1Byte(VNC1L_SPI_CHIP_SELECT_REVB2,  *(pData+i), VNC1L_STATUS_REGISTER ); 
		i++;
   }         

}


/***********************************************
*         USB CONTROLLER WRITE
*
*  THIS ROUTINE READS A MESSAGE FROM TO USB IC 
*
*  ENTERS-> Y= ADDRESS OF MESSAGE TO SEND
***********************************************
USB_WRITECMD:
         
USB_WC1:
         LDAA       1,Y+               ; GET NEXT BYTE TO WRITE
         BEQ        USB_WCX            ; DONE IF JUMP
         
         BSET       USBPORT,USBCS      ; ENABLE USB IC
         CLRB                          ; REFORMAT DATA TO SEND
         LSRD
         LSRD
         LSRD
         ORAA       #$90               ; WRITE STATUS REGISTER
         JSR        RUNSPI
         TBA                           ; GET LSB
         
         JSR        RUNSPI
         BCLR       USBPORT,USBCS      ; DISABLE USB IC

         PSHD
         LDAB       #55
         DBNE       B,*
         PULD
         
         BRA        USB_WC1            ; GET NEXT BYTE
         
USB_WCX: RTS
*/

//The SPI master must read the status bit at the end of each write transaction to determine if the data was
//written successfully to VNC1L Receive Buffer. The Data Write status bit is defined in Table 5.7. The status
//bit is only valid until the next rising edge of SCLK after the last data bit.
// Status Bit 	Meaning
// 0 				Accept   Data from the current transaction was accepted and added to the Receive Buffer
// 1 				Reject   Write data was not accepted.
//								Retry the same write cycle.
//  USB_WRITE:        SENDS KNOWN LENGTH COMMAND/DATA TO VINCULUM CHIP
void USB_Write(  char * pData, int nLen )
{
 int i;
 WORD nTemp;
 

   for(i=0; i<nLen; i++)
   {
//         ReadAndParseUSBResponse();
		nTemp = VNC1LWrite1Byte(VNC1L_SPI_CHIP_SELECT_REVB2,  *(pData+i), VNC1L_DATA_REGISTER ); 
//		iprintf("%2x ",*(pData+i) ); 
        g_nUSBWriteTimeoutInPits = 4 * USBLONGTIMEOUT;
        while((nTemp & 0x0001) && (g_nUSBWriteTimeoutInPits != 0))    // was data accepted.
		{
//			USB_Query();  			// check if VNC1L has some message.
//         USB_Read();          // Read data if any available.
         ReadAndParseUSBResponse();
			nTemp = VNC1LWrite1Byte(VNC1L_SPI_CHIP_SELECT_REVB2,  *(pData+i), VNC1L_DATA_REGISTER ); //retransmit.
            
		}
		if(g_nUSBWriteTimeoutInPits == 0)
        {
            //if(fdTelnet > 0)
            //iprintf("usb has timed out %d g_bSPIBusBusy is %d g_nSPISelect is %d ",g_nUSBWriteTimeoutInPits,g_bSPIBusBusy,g_nSPISelect);
            g_nUSBWriteTimeoutCtr++;
        }
   }         

}


/***********************************************
*         USB CONTROLLER WRITE
*
*  THIS ROUTINE READS A MESSAGE FROM TO USB IC 
*
*  ENTERS-> Y= ADDRESS OF MESSAGE TO SEND
***********************************************
USB_WRITE:
         MOVB       1,Y+,1,-SP         ; SAVE CHAR COUNT
USB_W1:
         LDAA       1,Y+               ; GET NEXT BYTE TO WRITE
         STAA       TTEMP1
         
USB_W1A: BSET       USBPORT,USBCS      ; ENABLE USB IC
         CLRB                          ; REFORMAT DATA TO SEND
         LSRD
         LSRD
         LSRD
         ORAA       #$80               ; WRITE DATA REGISTER
         JSR        RUNSPI
         TBA                           ; GET LSB
         
         JSR        RUNSPI
         BCLR       USBPORT,USBCS      ; DISABLE USB IC

         PSHD
         LDAB       #55
         DBNE       B,*
         PULD
         
         BITA       #$10
         BEQ        USB_W2             ; BUFFER NOT FULL IF JUMP
         PSHY
         JSR        USB_QUERY
         PULY
         LDAA       TTEMP1
         BRA        USB_W1A            ; RE-TRANSMIT
         
USB_W2:  DEC        0,SP               ; DONE WITH COMMAND?
         BNE        USB_W1             ; NO-GET NEXT BYTE
         
         LEAS       1,SP               ; DEALLOCATE LOOP COUNTER
USB_WX:  RTS
*/         


void USB_Print(  char * pData  )
{

 int i;
 char pBuf[PRINT_BUF_SIZE];

// sprintf(pBuf, strDeviceSendData);		// dsd  string	   // 83 20 byte 0D data  (byte = no. of chars)
// strcat(pBuf, pData );		//	
   for(i=0; i<4; i++)
   {
      pBuf[i] = cDeviceSendData[i];
   }
   sprintf(pBuf+4, pData);	
       
   pBuf[2] = strlen(pData);      //NB it is the no. of data bytes or chars, not total message length


   USB_Write( pBuf, strlen(pData)+4 );
//   iprintf("\nUSB_Print chars=%d :", strlen(pData)+4); 
//   for(i=0; i<strlen(pData)+4; i++)
//   {
//      iprintf(" %2x", pBuf[i]);
//   }

}

/*************************************************************
*          USB PRINT
*
*  THIS ROUTINE SENDS AN ASCII STRING TO THE PRINTER
*
*  THE STRING MUST BE NULL TERMINATED
*
*  THE USB COMMAND DSD IS ADDED TO THE BEGINNING OF THE STRING
*************************************************************         
USB_PRINT:
         MOVW      #$8320,PRINT_BUFF+1  ; SETUP DEVICE SEND DATA COMMAND
         MOVB      #$0D,PRINT_BUFF+4    ; SETUP CARRIAGE RETURN
         LDAA      #$FF                 ; INIT CHAR COUNTER
         LDX       #PRINT_BUFF+5        ; START OF PRINT BUFFER DATA
F03S1:   INCA                           ; COUNT CHARS IN STRING
         LDAB      1,Y+
         STAB      1,X+
         BNE       F03S1                ; LOOP TILL END OF STRING
         
         STAA      PRINT_BUFF+3         ; SAVE # CHARS TO SEND
         ADDA      #4                   ; ADD # CONTROL BYTES
         STAA      PRINT_BUFF
         PSHY                           ; SAVE SENT STRING ADDRESS
         LDY       #PRINT_BUFF          ; GET START OF COMMAND
         JSR       USB_WRITE            ; SEND TO PRINTER
         PULY
         
F03X:    RTS
*/

// sends a command string to the printer.
void USB_Send_Print(  char * pCommand, int nChars  )
{

 int i;
 char pBuf[PRINT_BUF_SIZE];

// sprintf(pBuf, strDeviceSendData);		// dsd  string	   // 83 20 byte 0D data  (byte = no. of chars)
// strcat(pBuf, pData );		//	
   for(i=0; i<4; i++)
   {
      pBuf[i] = cDeviceSendData[i];
   }
   for(i=0; i<nChars; i++)
   {
      pBuf[i+4] = *(pCommand+i);
   }
          
   pBuf[2] = nChars;    // NB it is the no. of data bytes or chars, not total message length

//   for(i=0; i<nLen; i++)
//      iprintf("-%2x", pBuf[i]);
      
   USB_Write( pBuf, nChars + 4 );

}

/*********************************************************
*           INITIALIZE PRINTER
*
*  THIS ROUTINE RESETS THE PRINTER AND SETS IT TO ASCII
*  PRINT (PCL3 LANGUAGE, HP PRINTERS ONLY)
*
*   RETURNS WITH Z SET IF PRINTER OK
*                Z CLR IF NOT READY OR NOT PRESENT
*
*********************************************************/
void Init_Printer( void )
{
int nUSBTimeOutCounter = 0;
   if((fdTelnet > 0)&& g_nTSMDebug.m_bUSBPrinterDebug)
   iprintf("\n initialise printer");
   if( g_sUSB.m_bUSBInitialised )      //note, the USB chip, VNC1L must be initialised first.
   {
      g_sUSB.m_bDevicePresentP1Transition = FALSE;
      // clear out VNC1L's transmit buffer before looking for status.
/*      do{
            Delay_uS(1000);        // 1 mS delay.
            ReadAndParseUSBResponse();
         }
         while( g_sUSB.m_nCharsRead > 0 );   */
      FlushVNC1LTransmitter();     // clear out VNC1L's transmit buffer 
      USB_Write(  cDeviceSet1, 4 );	  //SET CURRENT DEVICE #1   
      
      do{
            Delay_uS(30000);        // 30 mS delay.
            ReadAndParseUSBResponse();
            nUSBTimeOutCounter++;
         }
         while(g_sUSB.m_nCharsRead == 0 && nUSBTimeOutCounter < 10 );
         
      // clear out VNC1L's transmit buffer before looking for status.
/*      do{
            Delay_uS(1000);        // 1 mS delay.
            ReadAndParseUSBResponse();
         }
         while( g_sUSB.m_nCharsRead > 0 );   */
      FlushVNC1LTransmitter();     // clear out VNC1L's transmit buffer 
      USB_Write( cHPSetup1, 6 );	    //PCL = Reset
      
      nUSBTimeOutCounter = 0;
      do{
            Delay_uS(30000);        // 30 mS delay.
            ReadAndParseUSBResponse();
            nUSBTimeOutCounter++;
         }
         while(g_sUSB.m_nCharsRead == 0 && nUSBTimeOutCounter < 10 );
      
//      USB_Read();          // read any response.
//      ReadAndParseUSBResponse();

      // clear out VNC1L's transmit buffer before looking for status.
/*      do{
            Delay_uS(1000);        // 1 mS delay.
            ReadAndParseUSBResponse();
         }
         while( g_sUSB.m_nCharsRead > 0 );   */
      FlushVNC1LTransmitter();     // clear out VNC1L's transmit buffer 
      USB_Write( cHPSetup, 8 );	  //PCL = AnSII ascii (ISO 6)
      nUSBTimeOutCounter = 0;
      do{
            Delay_uS(30000);        // 30 mS delay.
            ReadAndParseUSBResponse();
            nUSBTimeOutCounter++;
         }
         while(g_sUSB.m_nCharsRead == 0 && nUSBTimeOutCounter < 10 );
/*
      USB_Write( cHPSetup12, 10 );	    //PCL = 12 chars/incch
      nUSBTimeOutCounter = 0;
      do{
            Delay_uS(30000);        // 30 mS delay.
            ReadAndParseUSBResponse();
            nUSBTimeOutCounter++;
         }
         while(g_sUSB.m_nCharsRead == 0 && nUSBTimeOutCounter < 10 ); // wait for reply.
*/         
      // clear out VNC1L's transmit buffer before looking for status.
/*      do{
            Delay_uS(1000);        // 1 mS delay.
            ReadAndParseUSBResponse();
         }
         while( g_sUSB.m_nCharsRead > 0 );   */
         
//      iprintf("about to query device 1:\n");     
      FlushVNC1LTransmitter();     // clear out VNC1L's transmit buffer 
      USB_Write( cQueryDevice1, 4 );	    //Query device #1
      nUSBTimeOutCounter = 0;
      do{
            Delay_uS(30000);        // 30 mS delay.
            ReadAndParseUSBResponse();
            nUSBTimeOutCounter++;
         }
         while(g_sUSB.m_nCharsRead == 0 && nUSBTimeOutCounter < 10 ); // wait for reply.
         
//         for(i=0; i<9; i++)
//         {
//            iprintf( "#%02x", g_sUSB.m_ucReadBuf[i] ); 
//         }
      if(g_sUSB.m_ucReadBuf[7] == 0x04 )
      {
          if((fdTelnet > 0)&& g_nTSMDebug.m_bUSBPrinterDebug)
          iprintf("\n Printer Found\n");
          g_sUSB.m_bPrinterFound = TRUE;
      }
      
 
//      iprintf("about to query p1\n");     
      // clear out VNC1L's transmit buffer before looking for status.
/*      do{
            Delay_uS(1000);        // 1 mS delay.
            ReadAndParseUSBResponse();
         }
         while( g_sUSB.m_nCharsRead > 0 );
//      iprintf("about to query p1\n");  */          
      FlushVNC1LTransmitter();     // clear out VNC1L's transmit buffer 
      USB_Write( cQueryPort1, 2 );	    //Query Port 1
      nUSBTimeOutCounter = 0;
      do{
            Delay_uS(30000);        // 30 mS delay.
            ReadAndParseUSBResponse();
            nUSBTimeOutCounter++;
         }
         while(g_sUSB.m_nCharsRead == 0 && nUSBTimeOutCounter < 10 );   //wait for a response

// get printer status
      if( g_sUSB.m_bPrinterFound )
      {
            // clear out VNC1L's transmit buffer before looking for status.
/*            do{
                  Delay_uS(1000);        // 1 mS delay.
                  ReadAndParseUSBResponse();
               }
               while( g_sUSB.m_nCharsRead > 0 );   */
               
//            iprintf("\nPrinter Get Status.\n"); 
            FlushVNC1LTransmitter();     // clear out VNC1L's transmit buffer                
            USB_Write( cPrinterGetStatus, 2 );	    //get printer status
            nUSBTimeOutCounter = 0;
            do{
                  Delay_uS(30000);        // 30 mS delay.
                  ReadAndParseUSBResponse();
                  nUSBTimeOutCounter++;
               }
               while(g_sUSB.m_nCharsRead == 0 && nUSBTimeOutCounter < 10 );
      }         
      
/*      iprintf("about to query p2\n");     
      USB_Write( cQueryPort2, 2 );	    //Query Port 2
      for(i=0; i<5; i++)
      {
         Delay_uS(30000);        // 30 mS delay.
         ReadAndParseUSBResponse();
      }
*/      
      Delay_uS(30000);        // 30 mS delay.

      
//      USB_Write( cHPSetupBold, 9 );	  //PCL = bold
//      Delay_uS(30000);        // 30 mS delay.
//      ReadAndParseUSBResponse();
      if( g_sUSB.m_bPrinterFound )
      {
         g_sUSB.m_bPrinterInitialised = TRUE;
         g_sUSB.m_bPrinterReady = TRUE;
      }         
         
   }   
   
}

/*********************************************************
*           INITIALIZE PRINTER
*
*  THIS ROUTINE RESETS THE PRINTER AND SETS IT TO ASCII
*  PRINT (PCL3 LANGUAGE, HP PRINTERS ONLY)
*
*   RETURNS WITH Z SET IF PRINTER OK
*                Z CLR IF NOT READY OR NOT PRESENT
*
*********************************************************
INIT_PRINTER:
         LDAA      USB1_TYPE            ; IS PORT 1 PRINTER?
         CMPA      #$01              
         LBNE      PINITY               ; NOT IF JUMP
         LDY       #DEVICE_SET1         ; SET CURRENT DEVICE
         BRA       PINIT2
PINIT1:  LDAA      USB2_TYPE            ; IS PORT 2 PRINTER?
         CMPA      #$01 
         LBNE      PINITY               ; NO-ERROR
         LDY       #DEVICE_SET2         ; SET PORT 2 AS ACTIVE DEVICE
PINIT2:  JSR       USB_WRITE                                                                            

;         LDY       #CANON_SETUP         ; SEND PRINT ENABLE COMMAND
;         JSR       USB_WRITE
         LDY       #HP_SETUP1           ; SEND PRINT ENABLE COMMAND
         JSR       USB_WRITE
         JSR       VAR_30MS
         JSR       USB_QUERY
         LDY       #HP_SETUP            ; SEND PRINT ENABLE COMMAND
         JSR       USB_WRITE
         JSR       VAR_30MS
         JSR       USB_QUERY
         CLRA                           ; RETURN ZERO FOR PRINTER READY
         BRA       PINITX
         
PINITY:  LDAA      #$FF                 ; RETURN NON-ZERO FOR ERROR
PINITX:  RTS 
*/

//*********************************************************************
//*******************  Parse Commands Received  ***********************
//*********************************************************************
/*
int Search_String(char cBuf, int nBufLen, char *search_string)
{
	int  i,x,result,command_length;
 	char mini_string[80];
	char temp_string[80];
   int len = strlen(search_string);
 	
//	command_length = sizeof(Commands) / sizeof(char *);

	x=0;
   strcpy(temp_string, search_string);	
	do
	{
		result = strcmp(cBuf+i, temp_string);

  	}while( (result !=0x00) && (++i+len < nBufLen) );

	if(result == 0x00)  //i.e. found
		return(0);
   else
      return(1);
}
*/
/*		
 char szKey[] = "apple";
  char szInput[80];
  do {
     printf ("Guess my favourite fruit? ");
     gets (szInput);
  } while (strcmp (szKey,szInput) != 0);
  puts ("Correct answer!");
  return 0;		
*/  


/*
   got this from http://www.dreamincode.net/code/snippet2227.htm
takes two strings, the pattern to search for and the string to search
returns the starting position of the pattern or -1 if not found
note, all C "strings" are terminated with a zero value
*/
int inStr(char *pattern, char *str) 
{
	int i, foundPos = 0;
	
	/* stop looping at end of string */
	while(str[foundPos]!=0) 
   {
		if (str[foundPos]==pattern[0]) 
      {
			/* our current search position matches 
			the first character in the pattern, 
			we'll look for more
			*/
			
			i = 1;
			/* we know i=0 is a match, so we start at 1 
			we loop until one of one of three conditions is met
			1. we hit the end the pattern
			2. we hit the end of the string
			3. the characters are not equal
			*/
			while(pattern[i]!=0 && str[foundPos + i]!=0 && str[foundPos + i]==pattern[i])
          { 
            i++; 
          }
			/* if we got the end of the pattern, we have a match */
			if (pattern[i]==0) 
         {
				return foundPos;
				
			/* if we got to the end of the search string, all other searches will fail too */
			} 
         else if (str[foundPos + i]==0) 
         {
				return -1;
			}
			/* if it just didn't match, we move on */
		}
		/* move our search one position forward and repeat */
		foundPos++;
	}
	/* if we got here, we found nothing */
	
	return -1;
}

void InitialiseUSBDataStructure( void )
{
int i;
   g_sUSB.m_bUSBMode = USB_MODE_FLASH_DISK;      //let flash disk be default mode.
   g_sUSB.m_bSCSMode = FALSE;   
   g_sUSB.m_bDiskWokeUp = FALSE;   
   
   g_sUSB.m_bOnlineDetected = FALSE;
   g_sUSB.m_bDevicePresentP1 = FALSE;    // set if device detected on port P1 (cleared by device removed P1)
   g_sUSB.m_bUSBInitialised = FALSE;
   g_sUSB.m_bPrinterFound = FALSE;
   g_sUSB.m_bPrinterReady = FALSE;
   g_sUSB.m_nPrinterStatus = 0x00;
   g_sUSB.m_bPaperEmpty = FALSE;
      
//   g_sUSB.m_pDataToPrint = pData;
   g_sUSB.m_bPrintRequest = FALSE;
   
   
   g_sUSB.m_bPrinterInitialised = FALSE;
   
   g_sUSB.m_bDevicePresentP1Transition = FALSE;
   g_sUSB.m_bPromptReceived = FALSE;
   g_sUSB.m_nCharsRead = 0;
   
   for(i=0; i<USB_READ_BUF_SIZE; i++)
      g_sUSB.m_ucReadBuf[i] = 0;
      
   g_sUSB.m_nTxIndex = 0;
   g_sUSB.m_bPrintFinished = TRUE;
   g_sUSB.m_bPrintInProgress = FALSE;
   g_sUSB.m_nCharsInLine = 0;
   
   g_sUSB.m_bDevicePresentP2 = FALSE;    
   g_sUSB.m_bDevicePresentP2Transition = FALSE; 
   g_sUSB.m_bUSBDriveReady = FALSE;

   g_sUSB.m_nBytesToWrite = 0;  // this is the number of data bytes to write to a file.
//	BYTE	m_cTxBuffer[USB_BUFFER_SIZE];
   g_sUSB.m_nTxBytes = 0;
   g_sUSB.m_bFlashWriteFinished = TRUE;
   g_sUSB.m_bFlashWriteInProgress = FALSE;
   g_sUSB.m_nTransmittedBytes = 0;
   g_sUSB.m_nBytesReceived = 0;
      
   g_sUSB.m_nReadIndex=0;
   g_sUSB.m_nBytesToRead = 0;  // this is the number of data bytes to read from a file.
   g_sUSB.m_bFlashReadFinished = TRUE;
   g_sUSB.m_bFlashReadInProgress = FALSE;
   
   g_sUSB.m_bEchoReceived = FALSE;
   g_sUSB.m_bGTCharReceived = FALSE;
   
//   g_sUSB.m_pWrData;
   sprintf( g_sUSB.m_cFileName, "Test1234.txt");        // currently limited to 8.3 size.

// for diagnostics   
   g_sUSB.m_bStartDiagWrite = FALSE;         // Start Diagnostics write (i.e when set Write to USB needs to be started)
   sprintf(g_sUSB.m_cDiagFileName, "Diag001.txt");       // Diagnostics file name currently limited to 8.3 size.
// for Event Log   

   g_sUSB.m_bStartEvLogWrite = FALSE;         // Start Event log write (i.e when set Write to USB needs to be started)
   sprintf( g_sUSB.m_cEvLogFileName, "EvLog001.txt");        // Event log file name currently limited to 8.3 size.
   
   g_sUSB.m_bStartXfrWrite = FALSE;         // Start Transfer write (i.e when set Write to USB needs to be started)
   sprintf( g_sUSB.m_cXfrFileName, "XfrFile1.txt");        // Transfer file name currently limited to 8.3 size.
   g_sUSB.m_nXfrBytes = 0;
}
//
void ReadAndParseUSBResponse( void )
{
   int nChars;
   int i;
   g_sUSB.m_bPromptReceived = FALSE;  
   
   nChars = USB_Read();
   
   g_sUSB.m_nCharsRead = nChars;
   
//      iprintf("\nN=%d", nChars);
      if( nChars > 0 )
      {
         for(i=0; i<nChars; i++)
         {
//             if(fdTelnet > 0)
//            iprintf( "_%02x", g_sUSB.m_ucReadBuf[i] ); 
            g_sUSB.m_cReadBuf[i] = g_sUSB.m_ucReadBuf[i];      //copy unsigned char into char buffer.
//            if(g_sUSB.m_cReadBuf[i] == '>')
//            {
//               g_sUSB.m_bPromptReceived = TRUE;             // got a prompt.
////               iprintf("yes");   //testing.
//            }
         }
//2411         iprintf("\n");
//2511 - iprintf's added
/*         for(i=0; i<nChars; i++)
         {
            iprintf( "_%02x", g_sUSB.m_ucReadBuf[i] );
            g_sUSB.m_cReadBuf[i] = (char)g_sUSB.m_ucReadBuf[i]; 
            if(g_sUSB.m_cReadBuf[i] == 0x0d)
               iprintf("\r\n");
            else
              iprintf("%c", g_sUSB.m_cReadBuf[i]); 
         }
*/         
         g_sUSB.m_cReadBuf[nChars] = '\0';   //terminate
//         cUSBCharBuffer[nChars] = '\0';   //terminate
//         iprintf("\n");
         if( g_sUSB.m_cReadBuf[0] == 'E' && g_sUSB.m_cReadBuf[1] == 0x0d )
         {
//2411            iprintf("\necho received");
            g_sUSB.m_bEchoReceived = TRUE;
            
         }
         else if( g_sUSB.m_cReadBuf[0] == 0x3e && g_sUSB.m_cReadBuf[1] == 0x0d )
         {
//2411            iprintf("\necho received");
            g_sUSB.m_bGTCharReceived = TRUE;
            
         }
         else if(inStr("On-Line", g_sUSB.m_cReadBuf) != -1)
         {
//            iprintf("\n found    On-Line\n");
            g_sUSB.m_bOnlineDetected = TRUE;
         }
         else if( inStr("Device Detected P1", g_sUSB.m_cReadBuf) != -1)
         {
         //            iprintf("\n found    Device Detected P1\n");
            if( !g_sUSB.m_bDevicePresentP1 )
            {
               g_sUSB.m_bDevicePresentP1Transition = TRUE;
            }
            g_sUSB.m_bDevicePresentP1 = TRUE;
            
         }
         else if(inStr("DD1", g_sUSB.m_cReadBuf) != -1)
         {
//            iprintf("\n found    DD1\n");
            if( !g_sUSB.m_bDevicePresentP1 )
            {
               g_sUSB.m_bDevicePresentP1Transition = TRUE;
            }
            g_sUSB.m_bDevicePresentP1 = TRUE;
            
         }
         else if(inStr("Device Removed P1", g_sUSB.m_cReadBuf) != -1)
         {
//            iprintf("\n found    Device Removed P1\n");
            g_sUSB.m_bDevicePresentP1 = FALSE;
            g_sUSB.m_bPrinterReady = FALSE;
         }
         else if(inStr("DR1", g_sUSB.m_cReadBuf) != -1)     //DR1 = Device Removed #1
         {
//            iprintf("\n found DR1\n");
            g_sUSB.m_bDevicePresentP1 = FALSE;
            g_sUSB.m_bPrinterReady = FALSE;
         }
         else if( inStr("Device Detected P2", g_sUSB.m_cReadBuf) != -1)
         {
//            iprintf("\n found    Device Detected P2\n");
            if( !g_sUSB.m_bDevicePresentP2 )
            {
               g_sUSB.m_bDevicePresentP2Transition = TRUE;
            }
            g_sUSB.m_bDevicePresentP2 = TRUE;
            
         }
         else if(inStr("DD2", g_sUSB.m_cReadBuf) != -1)
         {
//            iprintf("\n found    DD1\n");
            if( !g_sUSB.m_bDevicePresentP2 )
            {
               g_sUSB.m_bDevicePresentP2Transition = TRUE;
            }
            g_sUSB.m_bDevicePresentP2 = TRUE;
            
         }
         else if(inStr("Device Removed P2", g_sUSB.m_cReadBuf) != -1)
         {
//            iprintf("\n found    Device Removed P2\n");
            g_sUSB.m_bDevicePresentP2 = FALSE;
            g_sUSB.m_bUSBDriveReady = FALSE;
            g_sUSB.m_bDiskWokeUp = FALSE; 
         }
         else if(inStr("DR2", g_sUSB.m_cReadBuf) != -1)     //DR1 = Device Removed #2
         {
//            iprintf("\n found    DR2\n");
            g_sUSB.m_bDevicePresentP2 = FALSE;
            g_sUSB.m_bUSBDriveReady = FALSE;
            g_sUSB.m_bDiskWokeUp = FALSE; 
         }
         // prompt message???
         else if(inStr("D:\\>", g_sUSB.m_cReadBuf) != -1)     // "D:\>0d" D prompt (need double \ for compiler to pick up one\)
         {
//2411            iprintf("\n Prompt Received\n");
            g_sUSB.m_bPromptReceived = TRUE;
            g_sUSB.m_bUSBDriveReady = TRUE;
         }
/*         else if(inStr("E", g_sUSB.m_cReadBuf) != -1)
         {
            iprintf("\nEcho Received");
            g_sUSB.m_bEchoReceived = TRUE;
            
         }  */

//         for(i=0; i<nChars; i++)
//         {
//               iprintf(".%2x", g_sUSB.m_cReadBuf[i]); 
//         }
         
//         iprintf("\n\n");  int remainder = 7 % 3
      }

}


void TestPrinting( void )
{
 int i;
 
   char cBuf[1000];
   
   //Fill up a buffer with data
 for(i=0; i<USB_TX_BUF_SIZE; i++)
 {
   g_sUSB.m_cTxBuffer[i] = 'A'+ i%22;
 }
 
 sprintf(g_sUSB.m_cTxBuffer, "\r\n < PrriiiNT TesT > \r\n");
 for(i=0; i< 5; i++)
 {
   sprintf(cBuf, "\nThis is a UsB Printer Test - 00-11-2008 back-slash n at start %d.", i+1);
   strcat(g_sUSB.m_cTxBuffer, cBuf);
  }
 for(i=5; i<10; i++)
 {
   sprintf(cBuf, "UsB Printer Test - 00-11-2008 back-slash n at end %d.\n", i+1);
   strcat(g_sUSB.m_cTxBuffer, cBuf);
  }
 for(i=10; i<15; i++)
 {
   sprintf(cBuf, "Us__B Printer Test on 10-11-2008 no line terminations at start or at end %d.", i+1);
   strcat(g_sUSB.m_cTxBuffer, cBuf);
  }
   
   g_sUSB.m_cTxBuffer[USB_TX_BUF_SIZE-1] = '\0';  //terminate

// new test...
   sprintf(cBuf, "\nPrint Test 2\n");
   for(i=0; i<10; i++)
      strcat(cBuf, "I am trying to print something....\n");
      
   if( g_sUSB.m_bPrinterReady && !g_sUSB.m_bPrintInProgress)
      StartUSBPrint( cBuf, TRUE );     // start print and specify a page feed.
   while( g_sUSB.m_bPrintInProgress )
   {
      HandleUSBPrintJob();
      OSTimeDly(2);
   }     
/*
      nCharsInLine = 0;
 //10.11.08
    g_sUSB.m_nTxIndex = 0;
   g_sUSB.m_nCharsInLine = 0;
   g_sUSB.m_bPrintFinished = FALSE;
   do{
      HandleUSBPrintJob();
      OSTimeDly(2);
   } while(!g_sUSB.m_bPrintFinished);
*/
 //end 10.11.08
          
/*                            
   do{
      i=0;
      while( i < MAX_USB_BLOCK_SIZE && !g_bUSBPrintFinished )
      {
         cLineBuffer[i] = g_sUSB.m_cTxBuffer[nIndex];
         nCharsInLine++;
         i++;
         if(g_sUSB.m_cTxBuffer[nIndex] == 0 || nIndex >= USB_TX_BUF_SIZE ) 
         {
            g_bUSBPrintFinished = TRUE;
            if(nIndex >= USB_TX_BUF_SIZE)
            {
               cLineBuffer[i] = '\0';        // terminate cLineBuffer
               i++;
            }
         }
         else if( g_sUSB.m_cTxBuffer[nIndex] == '\n')    //line feed??
         {
            if(g_sUSB.m_cTxBuffer[nIndex] != '\r')    // next char a CR???
            {
                cLineBuffer[i] = '\r';              // if not add one.
                i++;
                nCharsInLine = 0;
            }
         }
         else if(nCharsInLine >= MAX_LINE_SIZE)   //havent had a line feed, then force a wrap around.
         {
               cLineBuffer[i] = '\r';
               i++;
               cLineBuffer[i] = '\n';
               i++;
               nCharsInLine = 0;
         }
         
         nIndex++;   // advance to next char in buffer.

           
      }
      
      
      cLineBuffer[i] = '\0';      // terminate.     
      i++;

      nCharsInBuf = i;
      
//         sprintf(cLineBuffer, "One day, at 7'oclock in the evening.. %d \r\n", 10*i );
      
      USB_Print( cLineBuffer );
//         iprintf("\n %s  \n", cLineBuffer);
//         for(i=0; i<nCharsInBuf; i++)
//            iprintf("+%02x",cLineBuffer[i]);
//         iprintf("yyy\n");
      
      OSTimeDly(2);
      
   }while( !g_bUSBPrintFinished );
 
*/ 
}
void ScheduleUSBPrint( char * pData )
{
   g_sUSB.m_pDataToPrint = pData;
   g_sUSB.m_bPrintRequest = TRUE;
} 

void HandleUSBPrintJob( void )
{
   int i;
   int nCharsInBuf;
   char cLineBuffer[100];

//   if( g_sUSB.m_bPrintInProgress )     
   // is there a print job in progress?? Printer ready?? Paper not empty??  
   if( g_sUSB.m_bPrinterReady && g_sUSB.m_bPrintInProgress && !g_sUSB.m_bPaperEmpty  ) 
   {
     if(!g_bSPIBusBusy)    //SPI bus available??? 
     { 
      g_bSPIBusBusy = TRUE;
      SPI_Select_USB ( );  // set SPI chip selects for SPI.
         
      if( !g_sUSB.m_bPrintFinished )
      {   
         i=0;
         
         while( i < MAX_USB_BLOCK_SIZE && !g_sUSB.m_bPrintFinished )
         {
            cLineBuffer[i] = g_sUSB.m_cTxBuffer[g_sUSB.m_nTxIndex];
            g_sUSB.m_nCharsInLine++;
            i++;
            if(g_sUSB.m_cTxBuffer[g_sUSB.m_nTxIndex] == 0 || g_sUSB.m_nTxIndex >= USB_TX_BUF_SIZE ) 
            {
               g_sUSB.m_bPrintFinished = TRUE;
               if(g_sUSB.m_nTxIndex >= USB_TX_BUF_SIZE)
               {
                  cLineBuffer[i] = '\0';        // terminate cLineBuffer
                  i++;
               }
            }
            else if( g_sUSB.m_cTxBuffer[g_sUSB.m_nTxIndex] == '\n')    //line feed??
            {
               if(g_sUSB.m_cTxBuffer[g_sUSB.m_nTxIndex] != '\r')    // next char a CR???
               {
                   cLineBuffer[i] = '\r';              // if not add one.
                   i++;
                   g_sUSB.m_nCharsInLine = 0;
               }
            }
            else if( g_sUSB.m_cTxBuffer[g_sUSB.m_nTxIndex] == '%')    //% char? - causes problems
            {
                   cLineBuffer[i] = '%';              // if not add one.
                   i++;
                   cLineBuffer[i] = '%';              // add a second % char.  i.e. %%
            }
            
            else if(g_sUSB.m_nCharsInLine >= MAX_LINE_SIZE)   //havent had a line feed, then force a wrap around.
            {
                  cLineBuffer[i] = '\r';
                  i++;
                  cLineBuffer[i] = '\n';
                  i++;
                  g_sUSB.m_nCharsInLine = 0;
            }
            
            g_sUSB.m_nTxIndex++;   // advance to next char in buffer.
   
              
         }
         
         
         cLineBuffer[i] = '\0';      // terminate.     
         i++;
   
         nCharsInBuf = i;
         
   //         sprintf(cLineBuffer, "One day, at 7'oclock in the evening.. %d \r\n", 10*i );        
         USB_Print( cLineBuffer );  //4.12.2008
      }  
      
      g_bSPIBusBusy = FALSE;     // release the SPI bus. 
                  
          
      if( g_sUSB.m_bPrintFinished )
      {
         if((fdTelnet > 0)&& g_nTSMDebug.m_bUSBPrinterDebug)
         {
             iprintf("\njob  finished ");
         }
         g_sUSB.m_bPrintInProgress = FALSE;     // release for next job. 
      }
     }  
      
   } 
}

void StartUSBPrint( char * pData, bool AppendPageFeed )
{
 int i=0;
 bool bCopied = FALSE;
   // no other job in progress and paper not empty?

  if(!g_sUSB.m_bPrintInProgress && !g_sUSB.m_bPaperEmpty )
  {
  if((fdTelnet > 0)&& g_nTSMDebug.m_bUSBPrinterDebug)
  iprintf("\n usb printing started");        
   // copy data to be printed into TxBuffer in USB structure.
      while( i < (USB_TX_BUF_SIZE-2) && !bCopied )
      {
         g_sUSB.m_cTxBuffer[i] = *(pData+i);
         if(*(pData+i) == '\0')
         {
            bCopied = TRUE;
            if( AppendPageFeed )      // call for a page feed??
            {
               if(g_sUSB.m_cTxBuffer[i-1] != '\f')    // char before terminator = form feed??
               {
                  g_sUSB.m_cTxBuffer[i] = '\f';       // if not, add in one.
                  i++;
                  g_sUSB.m_cTxBuffer[i] = '\0';
                  
               }
            }
         }
         i++;
      }
      g_sUSB.m_cTxBuffer[USB_TX_BUF_SIZE-1] = '\0';  //terminate
   
   
      g_sUSB.m_nTxIndex = 0;
      g_sUSB.m_nCharsInLine = 0;
      g_sUSB.m_bPrintFinished = FALSE;
      
      g_sUSB.m_bPrintInProgress = TRUE;  // flag a print job in progress.
      
      g_sUSB.m_bPageFeed = AppendPageFeed;      // if called with page feed.
//                 if(fdTelnet > 0)
//                  iprintf("\nSt.UP\n");
//      if( fdTelnet > 0)
//            iprintf( g_sUSB.m_cTxBuffer ); 

   }
   else
   {
      if((fdTelnet > 0)&& g_nTSMDebug.m_bUSBPrinterDebug)
      iprintf("\n usb printing not started");        
   }
}

// function continuously monitors printer port on VNCiL, initialises it if necessary.
void MonitorPrinterPort10Hz( void )
{
   if(!g_bSPIBusBusy)    //SPI bus available??? 
   {
      g_bSPIBusBusy = TRUE;
      SPI_Select_USB( );  // set SPI chip selects for SPI.      

     ReadAndParseUSBResponse();    // poll the VNC1L to see if any message present.
      
      if(!g_sUSB.m_bUSBInitialised)
      {
    
//         if(g_sUSB.m_bOnlineDetected)
//         {
            USB_Init();
//         }
//            if( g_sUSB.m_bUSBInitialised )      //has initialisation succeeded?
//               iprintf("\n USB_Init finished\n");
      }
      
    
      if( g_sUSB.m_bDevicePresentP1Transition )
      {
         g_sUSB.m_bDevicePresentP1Transition = FALSE;
         Init_Printer();
      }
      
      if( g_sUSB.m_bDevicePresentP2Transition )
      {
         g_sUSB.m_bDevicePresentP2Transition = FALSE;
         g_sUSB.m_bGTCharReceived = FALSE;
         USBCmdSend( CMD_CR );   // expecting to get > prompt.
          
      }
      
      if( !g_sUSB.m_bSCSMode )      // if not in Short Command Mode.
      {
      
         USBCmdSend( CMD_SCS );     // send command to put in SCS mode. (mode set by function)

      }

      if( !g_sUSB.m_bDiskWokeUp && g_sUSB.m_bDevicePresentP2 )      // if not woke up
      {
         USBCmdSend( CMD_WKD );     // send command to put wake disk
      }
      if( g_sUSB.m_bGTCharReceived  && g_sUSB.m_bDevicePresentP2 )
         g_sUSB.m_bUSBDriveReady = TRUE;
         
/*  This section will be needed to get the printing working
      // is there a print request?
      if( g_sUSB.m_bPrintRequest )     
      {
         // is the USB chip in PRINT mode?
        if(fdTelnet > 0)
         iprintf("\nPRq\n");
        if( g_sUSB.m_bUSBMode != USB_MODE_PRINTER)
        {
            if(!g_sUSB.m_bFlashWriteInProgress )
            {
               SetUSBMode( USB_MODE_PRINTER );
            }
        }   
        else if( !g_sUSB.m_bPrintInProgress  && g_sUSB.m_bUSBMode == USB_MODE_PRINTER )
        {
                 if(fdTelnet > 0)
                  iprintf("\nPR St.\n");

            StartUSBPrint( g_sUSB.m_pDataToPrint, FALSE );
            g_sUSB.m_bPrintRequest = FALSE;              // clear the request.
        }
      }
*/

//Testonly:

      if( g_CalibrationData.m_nDumpDeviceType == DUMP_DEVICE_PRINTER )  // use 0 for USB printer.
         GetPrinterStatus();   
      g_bSPIBusBusy = FALSE;     // release the SPI.
   }       
}

void ResetVNC1L( void )
{
      // initialise the data struct for the USB chip - VNC1L.
      InitialiseUSBDataStructure();
      
      USB_5V_ENABLE(); 
      VNC1L_PROG_HIGH();
      VNC1L_RESET_LOW();
      OSTimeDly(TICKS_PER_SECOND/20);      // wait .05 second.
      VNC1L_RESET_HIGH();
//      SPI_Select_USB ( );
      MakePD4AnInput();
}

// reads VNC1L until no more characters present in Transmitter(spi)
void FlushVNC1LTransmitter( void )
{
      g_nUSBFlushTimeoutInPits = USBTIMEOUT;
      do{
            Delay_uS(1000);        // 1 mS delay.
            ReadAndParseUSBResponse();
         }
         while( (g_sUSB.m_nCharsRead > 0) && (g_nUSBFlushTimeoutInPits != 0) );
         if(g_nUSBFlushTimeoutInPits == 0)
         {
             g_nUSBFlushTimeoutCtr++;
         }

}

//***************************************************************************
// Name: USBCmdSend
//
// Description: Send a single byte command to the Vinculum Monitor.
//
// Parameters: Cmd - Byte code of command.
//
// Returns: None.
//
// Comments: Supports only SCS commands without parameters.
//
//***************************************************************************
void USBCmdSend(char Cmd)
{
WORD nTemp;
	if (Cmd != CMD_CR)
	{
			nTemp = VNC1LWrite1Byte(VNC1L_SPI_CHIP_SELECT_REVB2,  Cmd, VNC1L_DATA_REGISTER ); //retransmit.
		    g_nUSBCommandSendTimeoutInPits = USBTIMEOUT;
			while((nTemp & 0x0001) && (g_nUSBCommandSendTimeoutInPits !=0 ))    // was data accepted.
   		{
   //			USB_Query();  			// check if VNC1L has some message.
   //         USB_Read();          // Read data if any available.
            
            ReadAndParseUSBResponse();
   			
            nTemp = VNC1LWrite1Byte(VNC1L_SPI_CHIP_SELECT_REVB2,  Cmd, VNC1L_DATA_REGISTER ); //retransmit.
   
   		}
        if(g_nUSBCommandSendTimeoutInPits == 0)
        {
            g_nUSBCommandSendTimeoutCtr++;
        }


	}
	// now send CR (0x0d).
	nTemp = VNC1LWrite1Byte(VNC1L_SPI_CHIP_SELECT_REVB2,  0x0d, VNC1L_DATA_REGISTER );
    g_nUSBCommandSendTimeoutInPits = USBTIMEOUT;
    while(nTemp & 0x0001 && (g_nUSBCommandSendTimeoutInPits !=0))    // was data accepted.
	{
//			USB_Query();  			// check if VNC1L has some message.
 //        USB_Read();          // Read data if any available.
      ReadAndParseUSBResponse();
		nTemp = VNC1LWrite1Byte(VNC1L_SPI_CHIP_SELECT_REVB2,  0x0d, VNC1L_DATA_REGISTER );

	}
    if(g_nUSBCommandSendTimeoutInPits == 0)
    {
        g_nUSBCommandSendTimeoutCtr++;
    }

	
   if(Cmd == CMD_SCS)
		 g_sUSB.m_bSCSMode = TRUE;
   if(Cmd == CMD_WKD)               //wake disk command.
		 g_sUSB.m_bDiskWokeUp = TRUE;
		 
	
//	USBWrite(0x0d);
}

//***************************************************************************
// Name: USBSendFileCommands
//
// Description: Sends file commands to VNC1L
//
// Parameters: Cmd - Byte code of command.
//             FileName - name of file to write, read etc.
//             pData - data to read or write
//             nLen - length of data to read or write.
//
// Returns: None.
//
// Comments: Supports only SCS commands  
//
//***************************************************************************
void USBSendFileCommands(char Cmd, char * cFileName, char * pData, int nLen)
{
#define TX_BUFFER_SIZE  (USB_FLASH_BLOCK_SIZE+20)
   bool bDone;
  char cBuf[20];
  char TxBuf[ TX_BUFFER_SIZE ];
  int nTxBytes,i;
   int nBytesReceived;  
   switch( Cmd )
   {
      case CMD_OPW :      // open file for write
         TxBuf[0] = 0x09;	                 // 
         TxBuf[1] = 0x20;	                 // 
         sprintf( cBuf, cFileName  );     // 
         i=0;
         while( cBuf[i] != 0x00)
         {
            TxBuf[i+2] = cBuf[i];    //copy file name into Tx buffer.
            i++;
         }  
         TxBuf[i+2] = 0x0d;	          //       
         
         nTxBytes = i+3;                  // no. of bytes to transmit.
         USB_Write( TxBuf, nTxBytes );
      
         break;
         
      case CMD_OPR :      // open file for read
         TxBuf[0] = 0x0E;	                 // 
         TxBuf[1] = 0x20;	                 // 
         sprintf( cBuf, cFileName  );     // 
         i=0;
         while( cBuf[i] != 0x00)
         {
            TxBuf[i+2] = cBuf[i];    //copy file name into Tx buffer.
            i++;
         }  
         TxBuf[i+2] = 0x0d;	          //       
         
         nTxBytes = i+3;                  // no. of bytes to transmit.
         USB_Write( TxBuf, nTxBytes );
         
//         for(j=0; j<nTxBytes; j++)
//            iprintf(" %x ", TxBuf[j]);
//         for(j=0; j<nTxBytes; j++)
//            iprintf("%c ", TxBuf[j]);
//         iprintf("\n");
      
         break;
         
      case CMD_WRF :      // write to file
         TxBuf[0] = 0x08;	                 // 
         TxBuf[1] = 0x20;	                 // 
          TxBuf[2] = (BYTE)(nLen>>24); 
          TxBuf[3] = (BYTE)(nLen>>16); 
          TxBuf[4] = (BYTE)(nLen>>8); 
          TxBuf[5] = (BYTE)(nLen); 
              
          TxBuf[6] = 0x0d;	          //       
            // now for data to be written to file
            for(i=0; i<nLen && i<TX_BUFFER_SIZE-8; i++)      // dont overwrite buffer size
            {
               TxBuf[7+i] = *(pData+i);  //copy file data into Tx buffer
            }         
          
         
         nTxBytes= nLen +7;                  // no. of bytes to transmit.
         USB_Write( TxBuf, nTxBytes );
      
         break;
//seek
      case CMD_SEK :      // seek to specified offset in  file (use nLen for offset value)
         TxBuf[0] = 0x28;	                 // 
         TxBuf[1] = 0x20;	                 // 
          TxBuf[2] = (BYTE)(nLen>>24); 
          TxBuf[3] = (BYTE)(nLen>>16); 
          TxBuf[4] = (BYTE)(nLen>>8); 
          TxBuf[5] = (BYTE)(nLen); 
              
          TxBuf[6] = 0x0d;	          //       
                   
         nTxBytes= 7;                  // no. of bytes to transmit.
         USB_Write( TxBuf, nTxBytes );
      
         break;
         
      case CMD_CLF :      // close file
         TxBuf[0] = 0x0A;	                 // 
         TxBuf[1] = 0x20;	                 // 
         sprintf( cBuf, cFileName  );     // 
         i=0;
         while( cBuf[i] != 0x00)
         {
            TxBuf[i+2] = cBuf[i];    //copy file name into Tx buffer.
            i++;
         }  
         TxBuf[i+2] = 0x0d;	          //       
         
         nTxBytes = i+3;                  // no. of bytes to transmit.
         USB_Write( TxBuf, nTxBytes );
//         for(j=0; j<nTxBytes; j++)
//            iprintf("c%x ", TxBuf[j]);
//         for(j=0; j<nTxBytes; j++)
//            iprintf("C%c ", TxBuf[j]);
//         iprintf("\n");
      
         break;
         
      case CMD_DLF :      // delete file
         TxBuf[0] = 0x07;	                 // 
         TxBuf[1] = 0x20;	                 // 
         sprintf( cBuf, cFileName  );     // 
         i=0;
         while( cBuf[i] != 0x00)
         {
            TxBuf[i+2] = cBuf[i];    //copy file name into Tx buffer.
            i++;
         }  
         TxBuf[i+2] = 0x0d;	          //       
         
         nTxBytes = i+3;                  // no. of bytes to transmit.
         USB_Write( TxBuf, nTxBytes );
//         for(j=0; j<nTxBytes; j++)
//            iprintf("c%x ", TxBuf[j]);
//         for(j=0; j<nTxBytes; j++)
//            iprintf("C%c ", TxBuf[j]);
//         iprintf("\n");
      
         break;
         
      case CMD_RDF :      // read nLen bytes from file
         TxBuf[0] = 0x0B;	                 // 
         TxBuf[1] = 0x20;	                 // 
          TxBuf[2] = (BYTE)(nLen>>24); 
          TxBuf[3] = (BYTE)(nLen>>16); 
          TxBuf[4] = (BYTE)(nLen>>8); 
          TxBuf[5] = (BYTE)(nLen); 
              
          TxBuf[6] = 0x0d;	          //       
         
         nTxBytes= 7;                  // no. of bytes to txmit.
         USB_Write( TxBuf, nTxBytes );
/*         iprintf("\nRDF...");
         for(i=0; i<nTxBytes; i++)
            iprintf(" %02x ", TxBuf[i]);
*/            
//         OSTimeDly(1);
         bDone = FALSE;
         g_nUSBUSBSendFileCommandsTimeoutInPits = USBTIMEOUT;
         while(!bDone && (g_nUSBUSBSendFileCommandsTimeoutInPits != 0)) 
         {   
            nBytesReceived = USB_ReadData( pData, nLen);
            if(nBytesReceived > 0)
               bDone = TRUE;
         }
         if(g_nUSBUSBSendFileCommandsTimeoutInPits == 0)
         {
             g_nUSBUSBSendFileCommandsTimeoutCtr++;
         }

//         iprintf("\nByytes Received= %d", nBytesReceived);
            
         break;
      case CMD_DIRF :      // dir file (returns size in bytes)
         TxBuf[0] = 0x01;	                 // 
         TxBuf[1] = 0x20;	                 // 
         sprintf( cBuf, cFileName  );     // 
         i=0;
         while( cBuf[i] != 0x00)
         {
            TxBuf[i+2] = cBuf[i];    //copy file name into Tx buffer.
            i++;
         }  
         TxBuf[i+2] = 0x0d;	          //       
         
         nTxBytes = i+3;                  // no. of bytes to transmit.
         USB_Write( TxBuf, nTxBytes );
         
//         iprintf("\nDirF...");
//         for(i=0; i<nTxBytes; i++)
//            iprintf(" %02x ", TxBuf[i]);
//         iprintf("\nDirF...");
//         for(i=0; i<nTxBytes; i++)
//            iprintf(" %c", TxBuf[i]);
//         iprintf("\n");
            
//         OSTimeDly(1);
         bDone = FALSE;
         nBytesReceived = 0;
         g_nUSBUSBSendFileCommandsTimeoutInPits = USBTIMEOUT;
         while((nBytesReceived < 2) && (g_nUSBUSBSendFileCommandsTimeoutInPits != 0))  
         {   
            nBytesReceived = USB_Read();
         }
         if(g_nUSBUSBSendFileCommandsTimeoutInPits == 0)
         {
             g_nUSBUSBSendFileCommandsTimeoutCtr++;
         }

         g_sUSB.m_nBytesReceived = nBytesReceived;
         
/*         nSize = g_sUSB.m_ucReadBuf[nBytesReceived-5];
         if( g_sUSB.m_ucReadBuf[nBytesReceived-4])
            nSize += g_sUSB.m_ucReadBuf[nBytesReceived-4]<<8;
         if( g_sUSB.m_ucReadBuf[nBytesReceived-3])
            nSize += g_sUSB.m_ucReadBuf[nBytesReceived-4]<<16;
         if( g_sUSB.m_ucReadBuf[nBytesReceived-2])
            nSize += g_sUSB.m_ucReadBuf[nBytesReceived-4]<<24;
 */           
         

         
//         iprintf("\nDIRF - Bytes = %d Size= %ld %02x\n", nBytesReceived, nSize, g_sUSB.m_ucReadBuf[nBytesReceived-5]);
//	     for(i=0; i<nBytesReceived && i<25; i++)
//	     {
//		    iprintf(" %02x",  g_sUSB.m_ucReadBuf[i]);
//         }   
//         iprintf("\n");      
//	     for(i=0; i<nBytesReceived; i++)
//	     {
//		    iprintf(" %c ",  (char)g_sUSB.m_ucReadBuf[i]);
//         }         
            
         break;
      case CMD_SCD1 :      // Sets current device to 1.
         TxBuf[0] = 0x86;	                 // 
         TxBuf[1] = 0x20;	                 // 
         TxBuf[2] = 0x00;              
         TxBuf[3] = 0x0d;	          //       
         break;
      case CMD_SCD2 :      // Sets current device to 2.
         TxBuf[0] = 0x86;	                 // 
         TxBuf[1] = 0x20;	                 // 
         TxBuf[2] = 0x01;              
         TxBuf[3] = 0x0d;	          //       
         break;
         
      default:
         break;

    }  


}
 
// Program to handle the writing of data to a USB flash memory stick.
// Writes out one block maximum - max size = USB_FLASH_BLOCK_SIZE
// If end of data, then clears the write in progress flag (g_sUSB.m_bFlashWriteInProgress) 
// Intended to be called in the foreground repetitively.
void HandleUSBFlashWriteJob( void )
{
   int i;
   int nBytesInBuf;
   char fBuffer[USB_FLASH_BLOCK_SIZE];

   if( g_sUSB.m_bFlashWriteInProgress && g_sUSB.m_bUSBDriveReady )     // is there a flash write job in progress.  
   {
     if(!g_bSPIBusBusy)    //SPI bus available??? 
     { 
      g_bSPIBusBusy = TRUE;
      SPI_Select_USB ( );  // set SPI chip selects for SPI.
      g_nSPISelect1 = 13;   //nbb--todo--determine if this should stay
         
      if( !g_sUSB.m_bFlashWriteFinished )
      {   
         i=0;
           
         while( i < USB_FLASH_BLOCK_SIZE && !g_sUSB.m_bFlashWriteFinished )
         {
            fBuffer[i] = *(g_sUSB.m_pWrData + g_sUSB.m_nTxIndex);
//            g_sUSB.m_nCharsInLine++;
            i++;
            g_sUSB.m_nTransmittedBytes++;
            if(g_sUSB.m_nTransmittedBytes >= g_sUSB.m_nBytesToWrite ) 
            {
               g_sUSB.m_bFlashWriteFinished = TRUE;
            }
            
            
            g_sUSB.m_nTxIndex++;   // advance to next char in buffer.
               
         }
            
         nBytesInBuf = i;
         
   //         sprintf(fBuffer, "One day, at 7'oclock in the evening.. %d \r\n", 10*i );
         if(nBytesInBuf)
         { 
//            iprintf("\n To file %d at %d", nBytesInBuf, g_sUSB.m_nTxIndex );
            USBSendFileCommands(CMD_WRF, g_sUSB.m_cFileName, fBuffer, nBytesInBuf);        
//            USB_Print( fBuffer );
         }
         
         if( g_sUSB.m_bFlashWriteFinished )
         {
            // close the file. 
//            if(fdTelnet > 0)
//            iprintf("\n Closing file %s", g_sUSB.m_cFileName);
            OSTimeDly(1);
            USBSendFileCommands(CMD_CLF, g_sUSB.m_cFileName, " ",0);     
         
         }
      }  
      
      g_bSPIBusBusy = FALSE;     // release the SPI bus.   
                
              
      if( g_sUSB.m_bFlashWriteFinished )
      {
         g_sUSB.m_bFlashWriteInProgress = FALSE;     // release for next job. 
         if( g_bFileXfrInProgress )    // was the UsB write part of a file transfer?
         { 
            g_bXfrBufBusy = FALSE;  // release the transfer buffer.
            if( g_bXfrFileReadFinished )  // finished reading transfer file??
               g_bFileXfrInProgress = FALSE;
         }
      }
      
     }  
      
   } 
}
 
 
bool StartUSBFlashWrite( char * pData, char * cFileName, int nLen )
{
 bool bWriteStarted = FALSE;
  
  if(!g_sUSB.m_bFlashWriteInProgress && g_sUSB.m_bUSBDriveReady)
  {
       if(!g_bSPIBusBusy)    //SPI bus available??? 
       { 
         g_bSPIBusBusy = TRUE;
         SPI_Select_USB ( );  // set SPI chip selects for SPI.
      	 g_nSPISelect1= 14;   //nbb--todo--determine if this should stay

   
         sprintf(g_sUSB.m_cFileName, cFileName);      // set up the filename
         g_sUSB.m_pWrData = pData;
   
         USBCmdSend( CMD_SCS );
         // wake disk
         USBCmdSend( CMD_WKD );     
   
         // open file
//        iprintf("\n OpeNinG  %s", g_sUSB.m_cFileName);
        
         USBSendFileCommands(CMD_OPW, g_sUSB.m_cFileName, " ",0 );
   // synchronise      
         g_sUSB.m_bEchoReceived = FALSE;
         USBCmdSend( CMD_E );      // send echo or 'e' command 
         g_nUSBEchoedTimeoutInPits = (2 * USBLONGTIMEOUT);
         while( !g_sUSB.m_bEchoReceived && (g_nUSBEchoedTimeoutInPits != 0))          
            ReadAndParseUSBResponse();
         if(g_nUSBEchoedTimeoutInPits == 0)
         {
             g_nUSBEchoedTimeoutCtr++;             
         }

           
         g_sUSB.m_nTransmittedBytes = 0;
         g_sUSB.m_nTxIndex = 0;
         g_sUSB.m_nBytesToWrite = nLen;
               
         g_sUSB.m_bFlashWriteFinished = FALSE;     
         g_sUSB.m_bFlashWriteInProgress = TRUE;  // flag a print job in progress.
         
         g_bSPIBusBusy = FALSE;
         bWriteStarted = TRUE;
        }     
   }
   return( bWriteStarted );
}
 
// Program to handle the reading file data from a USB flash memory stick.
// reads nLen bytes into buffer pointed to by pData.
// If no data available, will return a 0, else the no. of bytes read.
int USB_ReadData(char * pData, int nLen )
{
   int i;
   BYTE nReadByte;
   WORD nReadWord;
   bool bNewData;
   
   int nBytesReceived = 0;

	nReadWord = VNC1LRead1Byte( VNC1L_SPI_CHIP_SELECT_REVB2, VNC1L_DATA_REGISTER );
	
	if(nReadWord & 0x0001) 
	{
		bNewData = FALSE;		// status bit = 1, => data is old data
		return(nBytesReceived);  //i.e. return showing no new data available.
   }
	else
	{
		bNewData = TRUE;

   	nReadByte = BYTE(nReadWord >> 1);
   	i=0;
   	*(pData+i) = (char)nReadByte;
   	i++;
   }	
	
   while(i<nLen)
   {
   	nReadWord = VNC1LRead1Byte( VNC1L_SPI_CHIP_SELECT_REVB2, VNC1L_DATA_REGISTER );   	
   	if(nReadWord & 0x0001) 
   	{
   		bNewData = FALSE;		// status bit = 1, => data is old data
      }
   	else
   	{
   		bNewData = TRUE;
   	   nReadByte = BYTE(nReadWord >> 1);
   	  *(pData+i) = (char)nReadByte;
   	  i++;
   	  nBytesReceived = i;
      }
   
   }	

   return( nBytesReceived );     // return no. of chars read.
}


// Program to handle the reading of data from a USB flash memory stick.
// Reads one block maximum - max size = READ_BLOCK_SIZE
// If end of data, then clears the read in progress flag (g_sUSB.m_bFlashReadInProgress) 
// Intended to be called in the foreground repetitively.
void HandleUSBFlashReadJob( void )
{
   int nReadLen=0;

   if( g_sUSB.m_bFlashReadInProgress && g_sUSB.m_bUSBDriveReady )     // is there a flash write job in progress.  
   {
     if(!g_bSPIBusBusy)    //SPI bus available??? 
     { 
      g_bSPIBusBusy = TRUE;
      SPI_Select_USB ( );  // set SPI chip selects for SPI.
  	  g_nSPISelect1 = 15;

         
      if( !g_sUSB.m_bFlashReadFinished )
      {   
         
               if( (g_sUSB.m_nBytesToRead - USB_READ_BLOCK_SIZE*g_sUSB.m_nReadIndex) > USB_READ_BLOCK_SIZE )
                  nReadLen = USB_READ_BLOCK_SIZE;
               else
               {
                  nReadLen = g_sUSB.m_nBytesToRead - (USB_READ_BLOCK_SIZE*g_sUSB.m_nReadIndex);
                  g_sUSB.m_bFlashReadFinished = TRUE;
               }
               USBSendFileCommands(CMD_RDF, g_sUSB.m_cFileName, g_sUSB.m_pRdData + g_sUSB.m_nReadIndex*USB_READ_BLOCK_SIZE, nReadLen );
               g_sUSB.m_nReadIndex++;
//               iprintf("\nReading block %d", g_sUSB.m_nReadIndex);


               
         
         if( g_sUSB.m_bFlashReadFinished )
         {
            // close the file. 
//            iprintf("\n Closing file %s", g_sUSB.m_cFileName);
            USBSendFileCommands(CMD_CLF, g_sUSB.m_cFileName, " ",0);     
         
         }
      }  
      
      g_bSPIBusBusy = FALSE;     // release the SPI bus.   
                    
      if( g_sUSB.m_bFlashReadFinished )
         g_sUSB.m_bFlashReadInProgress = FALSE;     // release for next job. 
      
     }  
      
   } 
}

//start reading nLen bytes from a file cFileName.
// Data returned to pData.
// Opens file for reading and sets up all pointers etc.
void StartUSBFlashRead( char * pData, char * cFileName, int nLen, int nOffset )
{

  if(!g_sUSB.m_bFlashReadInProgress && g_sUSB.m_bUSBDriveReady)
  {
      sprintf(g_sUSB.m_cFileName, cFileName);      // set up the filename
      g_sUSB.m_pRdData = pData;

      USBCmdSend( CMD_SCS );
      // wake disk
      USBCmdSend( CMD_WKD );  
         
//      FlushVNC1LTransmitter();      //ensure the SPI tx pipeline is empty.      

      // open file
//      iprintf("\n StartUSBFlashRead....file= %s", g_sUSB.m_cFileName);
      USBSendFileCommands(CMD_OPR, g_sUSB.m_cFileName, " ",0 );
   
      g_sUSB.m_nBytesRead = 0;
      g_sUSB.m_nReadIndex = 0;
      g_sUSB.m_nBytesToRead = nLen;
            
      g_sUSB.m_bFlashReadFinished = FALSE;
      g_sUSB.m_bFlashReadInProgress = TRUE;
      
// synchronise      
      g_sUSB.m_bEchoReceived = FALSE;
      USBCmdSend( CMD_E );      // send echo or 'e' command 
      g_nUSBEchoedTimeoutInPits = USBTIMEOUT;
      while( !g_sUSB.m_bEchoReceived && (g_nUSBEchoedTimeoutInPits != 0))  
         ReadAndParseUSBResponse();
//new 28.11.2008         
      if(g_nUSBEchoedTimeoutInPits == 0)
      {
          //if(fdTelnet > 0)
          //iprintf("\n echo timeout 2");
          g_nUSBEchoedTimeoutCtr++;
      }

      if( nOffset > 0)
      {
            //seek to specified offset in open file.
           USBSendFileCommands(CMD_SEK, g_sUSB.m_cFileName, " ",nOffset );     
           SynchroniseUSB();
      }
         
      
   }
}
 
 //Functionn to deleta a file
void DeleteUSBFile( char * cFileName)
{
   USBSendFileCommands(CMD_DLF, cFileName, " ",0);     
// synchronise      
      g_sUSB.m_bEchoReceived = FALSE;
      USBCmdSend( CMD_E );      // send echo or 'e' command 
      g_nUSBEchoedTimeoutInPits = USBTIMEOUT;
      while( !g_sUSB.m_bEchoReceived && (g_nUSBEchoedTimeoutInPits != 0))             
         ReadAndParseUSBResponse();
      if(g_nUSBEchoedTimeoutInPits == 0)
      {
          g_nUSBEchoedTimeoutCtr++;
      }

}
 //Functionn to read size of a file
long GetUSBFileSize( char * cFileName )
{
char cDataBuf[20];
char cUCFileName[20];
int i,nLen;
int nIndex;
long lSize;
   USBSendFileCommands(CMD_DIRF, cFileName, cDataBuf,0);  

   nLen = strlen(cFileName);
      
   for(i=0;i<nLen; i++)
   {
      cUCFileName[i] = toupper(cFileName[i]);   // (char)g_sUSB.m_cReadBuf[i]);   
      cDataBuf[i] = (char)g_sUSB.m_ucReadBuf[i];
   }      
   cUCFileName[nLen] = '\0';                    //terminate the string.
   cDataBuf[nLen] = '\0';                       //terminate the string.

   if(strcmp( cUCFileName, cDataBuf) == 0 && g_sUSB.m_nBytesReceived >= nLen+4 )  // i.e. filenames match.    
   {
         nIndex = g_sUSB.m_nBytesReceived-5;    // points to least sign. byte of size.
         
         lSize = g_sUSB.m_ucReadBuf[nIndex];
//         iprintf("\nLSB= %02x", g_sUSB.m_ucReadBuf[nIndex]);
         if( g_sUSB.m_ucReadBuf[nIndex+1])
            lSize += g_sUSB.m_ucReadBuf[nIndex+1]<<8;
//         iprintf("\nB1= %02x %ld", g_sUSB.m_ucReadBuf[nIndex+1], lSize);
            
         if( g_sUSB.m_ucReadBuf[nIndex+2])
            lSize += g_sUSB.m_ucReadBuf[nIndex+2]<<16;
//         iprintf("\nb2= %02x %ld", g_sUSB.m_ucReadBuf[nIndex+2], lSize);
            
         if( g_sUSB.m_ucReadBuf[nIndex+3])
            lSize += g_sUSB.m_ucReadBuf[nIndex+3]<<24;
//         iprintf("\nB3= %02x %ld", g_sUSB.m_ucReadBuf[nIndex+3], lSize);
      
   }
   else
      lSize = 0;     // use 0 as default or error condition.
// synchronise
      SynchroniseUSB( );
/*
      g_sUSB.m_bEchoReceived = FALSE;
      USBCmdSend( CMD_E );      // send echo or 'e' command 
      while( !g_sUSB.m_bEchoReceived )            
         ReadAndParseUSBResponse();
*/
   return(lSize);
}
//  Sends E or echo char and waits to get it back.
void SynchroniseUSB(void)
{
     ReadAndParseUSBResponse();      
      g_sUSB.m_bEchoReceived = FALSE;
      USBCmdSend( CMD_E );      // send echo or 'e' command 
      g_nUSBEchoedTimeoutInPits = (2 * USBLONGTIMEOUT);
      while( !g_sUSB.m_bEchoReceived && (g_nUSBEchoedTimeoutInPits != 0))  
         ReadAndParseUSBResponse();
      if(g_nUSBEchoedTimeoutInPits == 0)
      {
          if((fdTelnet > 0)&& g_nTSMDebug.m_bUSBPrinterDebug)
          iprintf("\n timing out g_nUSBEchoedTimeoutInPits is %d,",g_nUSBEchoedTimeoutInPits);
          g_nUSBEchoedTimeoutCtr++;
      }

} 

void SetUSBMode( bool bMode )
{
//   if(fdTelnet > 0)
//      iprintf("\nSUMode= %d", bMode);
      
   if( bMode == USB_MODE_PRINTER )
   {
      USBSendFileCommands(CMD_SCD1, g_sUSB.m_cFileName, " ",0 );     //set current device to 1 (lowest) 
      g_sUSB.m_bUSBMode = USB_MODE_PRINTER;
   }
   else
   {
      USBSendFileCommands(CMD_SCD2, g_sUSB.m_cFileName, " ",0 );   
      g_sUSB.m_bUSBMode = USB_MODE_FLASH_DISK;
   }      
// synchronise      
   SynchroniseUSB( );

}

// Get status of printer
void GetPrinterStatus( void )
{
WORD nReadWord;
BYTE nStatus;

// synchronise      
   SynchroniseUSB( );

   USBCmdSend(CMD_PGS);       // send "printer get status" command.
// response will be 1 byte.
    g_nUSBGetPrinterStatusTimeoutInPits = (2 * USBLONGTIMEOUT);
   do{
      Delay_uS(10);        // 10uS delay.
	     nReadWord = VNC1LRead1Byte( VNC1L_SPI_CHIP_SELECT_REVB2, VNC1L_DATA_REGISTER );
	  } while ((nReadWord & 0x0001) && (g_nUSBGetPrinterStatusTimeoutInPits != 0)); //status bit = 1, => data is old data 
      if(g_nUSBGetPrinterStatusTimeoutInPits == 0)
      {
          g_nUSBGetPrinterStatusTimeoutCtr++;
      }

/*	  
	if(nReadWord & 0x0001) 
		bNewData = FALSE;		// status bit = 1, => data is old data
	else
	{
		bNewData = TRUE;
      // clear the read (receive) buffer
	   for(i=0; i<USB_READ_BUF_SIZE; i++)
		    g_sUSB.m_ucReadBuf[i] = 0;

   }
*/   
	nStatus = BYTE(nReadWord >> 1);
	g_sUSB.m_nPrinterStatus = nStatus;
	if(nStatus & 0x20)
	  g_sUSB.m_bPaperEmpty = TRUE;
   else
      g_sUSB.m_bPaperEmpty = FALSE;
	
}
