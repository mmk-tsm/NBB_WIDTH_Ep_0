/*-------------------------------------------------------------------
 File: EEPROMdriver.cpp
 Description: Read/Write EEPROM functions.
 
 MMk - Replaced QSPI_QMR_BAUD(32) with QSPI_QMR_BAUD(19)...i.e. divisor = 19.
 MMK - Replaced QSPI_QDLYR_DTL(0) with QSPI_QDLYR_DTL(1) i.e. delay of 32 x DTL x fsys/2.
 --
  P.Smith                                        1/06/06
  read eeprom to determine if it is necessary to write to it, if not exit

  P.Smith                                        10/11/06
  Timeout on all functions that have the potential to stay in loops.
   EEPROMRead1Byte,EEPROMWrite1Byte, WriteProtectEEPROM,
  
   P.Smith                                               5/2/07
    Remove unused iprintf 
  
   P.Smith                                               7/3/07
    Remove unused iprintf 

   P.Smith                                               31/7/07
   Ensure that in EEPROMWrite1Byte, data is not reprogrammed if 
   already at the correct value.  
   The set of g_nEEPROMWriteTimeoutCounter was done in the wrong place

   P.Smith                                             2/11/07
   Remove read of status register, there is a problem with the fram
   devices if the status register is used in conjuction with a write 
   command.
   Ensure that the write1byte checks the eeprom data after it is written
   to the fram and waits until it is the same.
   
   P.Smith                                          8/11/07
   SPI_Select_EEPROM called at beginning of fram write.
   
   M.McKiernan                                     7/4/2008
   This version was done to overcome problems with reverting to EEPROMs (from FRAMs).
   Took file from NBB_LIW...loses debug test that was in latest batch version
   Main thing that cures problem was in EEPROMRead1Byte reading status reg. & wait till any previous 
   Write in Progress finished (WIP bit cleared).
   See new 7.4.2008
   All diagnostics have been commented out.  Note this version has stuff that works for Telnet debugging if set up.
   
   P.Smith                                          7/5/07
   removed cSReg   
   
   P.Smith                                          3/9/09
   EEPROMWrite1Byte return Bool to show that the data has been written correctly.
   
 -----------------------------------------------------------------*/
#include <startnet.h>

#include <basictypes.h>
#include <stdio.h> 

#include "QSPIconstants.h"
#include "QSPIdriver.h"

#include "EEPROMconstants.h"
#include "EEPROMdriver.h"
#include "Batvars.h"
#include "Batvars2.h"
#include "General.h"

 /*
  *
  * Part 1: Write enable
  *
  */
  
void EEPROM_WREN(WORD chipSelectUsed) {

 // Create QSPI Configuration Structure to send write ENABLE instruction
 WORD QSPI_BufferWREN[1];
 WORD * ptrQSPI_BufferWREN = QSPI_BufferWREN;
 WORD TxLen = 1;
 QSPI_CFG ConfigWREN;
 QSPI_CFG * ptrConfigWREN = &ConfigWREN;
 
 // QSPI Mode Register
 // A) Setup QMR register (8-bit transfer) 
 ConfigWREN.QMR = ( QSPI_QMR_MSTR|QSPI_QMR_CPOL|QSPI_QMR_CPHA|QSPI_QMR_BITS_8|QSPI_QMR_DOHIE|QSPI_QMR_BAUD(19) );

 // B) QSPI Delay Register
 ConfigWREN.QDLYR =QSPI_QDLYR_SPE|QSPI_QDLYR_QCD(0)|QSPI_QDLYR_DTL(1);
 
 // QSPI Wrap Register 
 // C) Setup QWR register to send one byte(8-bits), and set CS to active low
 ConfigWREN.QWR = ( QSPI_QWR_ENDQP(0)|QSPI_QWR_NEWQP(0)|QSPI_QWR_CSIV );
 
 // D) QSPI Comand RAM Registers[0]
 ConfigWREN.QCR[0] =(QSPI_CRX_BITSE|QSPI_CRX_DSCK|QSPI_CRX_DT|chipSelectUsed);  
 
 QSPI_BufferWREN[0] = EEPROM_INST_WREN;
 
 QSPI_WriteQTR(ptrQSPI_BufferWREN, TxLen, ptrConfigWREN);
 
} // close function


  /*
  *
  * Part 2: Write data
  *
  */
void EEPROM_WRITE16(WORD chipSelectUsed, WORD TxLen, WORD * ptrTxBuffer) {

 // Create QSPI Configuration Structure 1 to send write instruction
 QSPI_CFG ConfigWRITE;
 QSPI_CFG * ptrConfigWRITE = &ConfigWRITE;
 
 // QSPI Mode Register
 // A) Setup QMR register (8-bit transfer) 
 ConfigWRITE.QMR = ( QSPI_QMR_MSTR|QSPI_QMR_CPOL|QSPI_QMR_CPHA|QSPI_QMR_BITS_8|QSPI_QMR_DOHIE|QSPI_QMR_BAUD(19) );

 // B) QSPI Delay Register
 ConfigWRITE.QDLYR = QSPI_QDLYR_SPE|QSPI_QDLYR_QCD(0)|QSPI_QDLYR_DTL(1);
 
 // QSPI Wrap Register 
 // C) Set up QWR register to send write instruction, address, and data
 // EndQP is TxLen-1 so that 16 = 15...
 ConfigWRITE.QWR = ( QSPI_QWR_ENDQP(TxLen-1)|QSPI_QWR_NEWQP(0)|QSPI_QWR_CSIV );  
 
 // D) QSPI Comand RAM Registers
 ConfigWRITE.QCR[0] = (QSPI_CRX_BITSE|QSPI_CRX_DSCK|QSPI_CRX_DT|QSPI_CRX_CONT|chipSelectUsed);  
 ConfigWRITE.QCR[1] = (QSPI_CRX_BITSE|QSPI_CRX_DSCK|QSPI_CRX_DT|QSPI_CRX_CONT|chipSelectUsed);
 ConfigWRITE.QCR[2] = (QSPI_CRX_BITSE|QSPI_CRX_DSCK|QSPI_CRX_DT|QSPI_CRX_CONT|chipSelectUsed); 
 ConfigWRITE.QCR[3] = (QSPI_CRX_BITSE|QSPI_CRX_DSCK|QSPI_CRX_DT|QSPI_CRX_CONT|chipSelectUsed);
 ConfigWRITE.QCR[4] = (QSPI_CRX_BITSE|QSPI_CRX_DSCK|QSPI_CRX_DT|QSPI_CRX_CONT|chipSelectUsed);
 ConfigWRITE.QCR[5] = (QSPI_CRX_BITSE|QSPI_CRX_DSCK|QSPI_CRX_DT|QSPI_CRX_CONT|chipSelectUsed);
 ConfigWRITE.QCR[6] = (QSPI_CRX_BITSE|QSPI_CRX_DSCK|QSPI_CRX_DT|QSPI_CRX_CONT|chipSelectUsed);
 ConfigWRITE.QCR[7] = (QSPI_CRX_BITSE|QSPI_CRX_DSCK|QSPI_CRX_DT|QSPI_CRX_CONT|chipSelectUsed);
 ConfigWRITE.QCR[8] = (QSPI_CRX_BITSE|QSPI_CRX_DSCK|QSPI_CRX_DT|QSPI_CRX_CONT|chipSelectUsed);
 ConfigWRITE.QCR[9]  = (QSPI_CRX_BITSE|QSPI_CRX_DSCK|QSPI_CRX_DT|QSPI_CRX_CONT|chipSelectUsed); 
 ConfigWRITE.QCR[10] = (QSPI_CRX_BITSE|QSPI_CRX_DSCK|QSPI_CRX_DT|QSPI_CRX_CONT|chipSelectUsed);
 ConfigWRITE.QCR[11] = (QSPI_CRX_BITSE|QSPI_CRX_DSCK|QSPI_CRX_DT|QSPI_CRX_CONT|chipSelectUsed);
 ConfigWRITE.QCR[12] = (QSPI_CRX_BITSE|QSPI_CRX_DSCK|QSPI_CRX_DT|QSPI_CRX_CONT|chipSelectUsed);
 ConfigWRITE.QCR[13] = (QSPI_CRX_BITSE|QSPI_CRX_DSCK|QSPI_CRX_DT|QSPI_CRX_CONT|chipSelectUsed);
 ConfigWRITE.QCR[14] = (QSPI_CRX_BITSE|QSPI_CRX_DSCK|QSPI_CRX_DT|QSPI_CRX_CONT|chipSelectUsed); 
 ConfigWRITE.QCR[15] = (QSPI_CRX_BITSE|QSPI_CRX_DSCK|QSPI_CRX_DT|QSPI_CRX_CONT|chipSelectUsed);
 
 QSPI_WriteQTR(ptrTxBuffer, TxLen, ptrConfigWRITE);  
 
} // close function write16 


  /*
  *
  * Part 3: Read data
  *
  */
void EEPROM_READ(WORD chipSelectUsed, WORD startAddress) {

 // Create QSPI Configuration Structure 1 to send write ENABLE instruction
 #define TxLen 16
 WORD QSPI_BufferREAD[TxLen];
 QSPI_CFG ConfigREAD;
 QSPI_CFG * ptrConfigREAD = &ConfigREAD;
 
 // QSPI Mode Register
 // A) Setup QMR register (8-bit transfer) 
 ConfigREAD.QMR = ( QSPI_QMR_MSTR|QSPI_QMR_CPOL|QSPI_QMR_CPHA|QSPI_QMR_BITS_8|QSPI_QMR_DOHIE|QSPI_QMR_BAUD(19) );

 // B) QSPI Delay Register
 ConfigREAD.QDLYR = QSPI_QDLYR_SPE|QSPI_QDLYR_QCD(0)|QSPI_QDLYR_DTL(1);
 
 // QSPI Wrap Register 
 // C) Setup QWR register to send one byte(8-bits), and set CS to active low
 ConfigREAD.QWR = ( QSPI_QWR_ENDQP(TxLen-1)|QSPI_QWR_NEWQP(0)|QSPI_QWR_CSIV ); // TxLen-1 = register 2
 
 // D) QSPI Comand RAM Registers[0]
 ConfigREAD.QCR[0] = (QSPI_CRX_BITSE|QSPI_CRX_DSCK|QSPI_CRX_DT|QSPI_CRX_CONT|chipSelectUsed);  
 ConfigREAD.QCR[1] = (QSPI_CRX_BITSE|QSPI_CRX_DSCK|QSPI_CRX_DT|QSPI_CRX_CONT|chipSelectUsed);
 ConfigREAD.QCR[2] = (QSPI_CRX_BITSE|QSPI_CRX_DSCK|QSPI_CRX_DT|QSPI_CRX_CONT|chipSelectUsed);
 ConfigREAD.QCR[3] = (QSPI_CRX_BITSE|QSPI_CRX_DSCK|QSPI_CRX_DT|QSPI_CRX_CONT|chipSelectUsed);  
 ConfigREAD.QCR[4] = (QSPI_CRX_BITSE|QSPI_CRX_DSCK|QSPI_CRX_DT|QSPI_CRX_CONT|chipSelectUsed);
 ConfigREAD.QCR[5] = (QSPI_CRX_BITSE|QSPI_CRX_DSCK|QSPI_CRX_DT|QSPI_CRX_CONT|chipSelectUsed);
 ConfigREAD.QCR[6] = (QSPI_CRX_BITSE|QSPI_CRX_DSCK|QSPI_CRX_DT|QSPI_CRX_CONT|chipSelectUsed);  
 ConfigREAD.QCR[7] = (QSPI_CRX_BITSE|QSPI_CRX_DSCK|QSPI_CRX_DT|QSPI_CRX_CONT|chipSelectUsed);
 ConfigREAD.QCR[8] = (QSPI_CRX_BITSE|QSPI_CRX_DSCK|QSPI_CRX_DT|QSPI_CRX_CONT|chipSelectUsed);
 ConfigREAD.QCR[9] = (QSPI_CRX_BITSE|QSPI_CRX_DSCK|QSPI_CRX_DT|QSPI_CRX_CONT|chipSelectUsed);  
 ConfigREAD.QCR[10] = (QSPI_CRX_BITSE|QSPI_CRX_DSCK|QSPI_CRX_DT|QSPI_CRX_CONT|chipSelectUsed);
 ConfigREAD.QCR[11] = (QSPI_CRX_BITSE|QSPI_CRX_DSCK|QSPI_CRX_DT|QSPI_CRX_CONT|chipSelectUsed);
 ConfigREAD.QCR[12] = (QSPI_CRX_BITSE|QSPI_CRX_DSCK|QSPI_CRX_DT|QSPI_CRX_CONT|chipSelectUsed);
 ConfigREAD.QCR[13] = (QSPI_CRX_BITSE|QSPI_CRX_DSCK|QSPI_CRX_DT|QSPI_CRX_CONT|chipSelectUsed);  
 ConfigREAD.QCR[14] = (QSPI_CRX_BITSE|QSPI_CRX_DSCK|QSPI_CRX_DT|QSPI_CRX_CONT|chipSelectUsed);
 ConfigREAD.QCR[15] = (QSPI_CRX_BITSE|QSPI_CRX_DSCK|QSPI_CRX_DT|QSPI_CRX_CONT|chipSelectUsed);
 
 QSPI_BufferREAD[0] = EEPROM_INST_READ;
 QSPI_BufferREAD[1] = (BYTE)(startAddress >> 8); // throw away bottom 8-bits
 QSPI_BufferREAD[2] = (BYTE)(startAddress);  // thow away top 8-bits
 QSPI_BufferREAD[3] = 0; // used to clock out data
 QSPI_BufferREAD[4] = 0;
 QSPI_BufferREAD[5] = 0;
 QSPI_BufferREAD[6] = 0;
 QSPI_BufferREAD[7] = 0;
 QSPI_BufferREAD[8] = 0;
 QSPI_BufferREAD[9] = 0;
 QSPI_BufferREAD[10] = 0;
 QSPI_BufferREAD[11] = 0;
 QSPI_BufferREAD[12] = 0;
 QSPI_BufferREAD[13] = 0;
 QSPI_BufferREAD[14] = 0;
 QSPI_BufferREAD[15] = 0;
   
 QSPI_WriteQTR(QSPI_BufferREAD, TxLen, ptrConfigREAD);
 
}// close function read

//   MMK start new
  /*
  *
  * Part 3a: Read byte
  *
  */

BYTE EEPROMRead1Byte(WORD chipSelectUsed, WORD startAddress) 
{
    BYTE EEPROMDataByte;
    BYTE cSReg;
  BOOL bSaveSPIEEActive = g_bSPIEEActive;
    g_bSPIEEActive = TRUE;    // prevent access to SPI on PIT.
  
    
//new 7.4.2008
// must wait till any previous Write in Progress finished (WIP bit cleared).
    SPI_Select_EEPROM();      //hardware SPI chip selects set up

    cSReg = ReadEEPROMStatusRegister( chipSelectUsed );
    if(cSReg & EEPROM_WIP)      // Is there a write in progress?
    {
//        OSTimeDly(1);            // OS blocking function - will allow other tasks to run.
 	    do{
    		cSReg = ReadEEPROMStatusRegister( chipSelectUsed );
//               SetupMttty();  //nbb--testonly--
//            if(g_bLiWDebug && fdnet) //Telnet print 
//            {
//               iprintf("x"); //nbb--testonly--
//            }
    		
  		  }while( cSReg & EEPROM_WIP );        //bit is cleared when finished
  		  
//            if(g_bLiWDebug && fdnet) //Telnet print 
//            {
//               iprintf("y"); //nbb--testonly--
//            }
   		  
    }
 
//endnew 7.4.2008    

 // Create QSPI Configuration Structure to send read instruction
 #define TxLen1 4     // 3 instruction bytes (READ ADD_Hi_Byte, ADD_Lo_Byte + data read 0,1,2,3
 WORD QSPI_BufferREADByte[TxLen1];
 QSPI_CFG ConfigREADByte;
 QSPI_CFG * ptrConfigREADByte = &ConfigREADByte;
 
 // QSPI Mode Register
 // A) Setup QMR register (8-bit transfer) 
 ConfigREADByte.QMR = ( QSPI_QMR_MSTR|QSPI_QMR_CPOL|QSPI_QMR_CPHA|QSPI_QMR_BITS_8|QSPI_QMR_DOHIE|QSPI_QMR_BAUD(19) );

 // B) QSPI Delay Register
 ConfigREADByte.QDLYR = QSPI_QDLYR_SPE|QSPI_QDLYR_QCD(0)|QSPI_QDLYR_DTL(1);
 
 // QSPI Wrap Register 
 // C) Setup QWR register to send one byte(8-bits), and set CS to active low
 ConfigREADByte.QWR = ( QSPI_QWR_ENDQP(TxLen1-1)|QSPI_QWR_NEWQP(0)|QSPI_QWR_CSIV ); // 
 
 // D) QSPI Comand RAM Registers[0]
 ConfigREADByte.QCR[0] = (QSPI_CRX_BITSE|QSPI_CRX_DSCK|QSPI_CRX_DT|QSPI_CRX_CONT|chipSelectUsed);  
 ConfigREADByte.QCR[1] = (QSPI_CRX_BITSE|QSPI_CRX_DSCK|QSPI_CRX_DT|QSPI_CRX_CONT|chipSelectUsed);
 ConfigREADByte.QCR[2] = (QSPI_CRX_BITSE|QSPI_CRX_DSCK|QSPI_CRX_DT|QSPI_CRX_CONT|chipSelectUsed);
 ConfigREADByte.QCR[3] = (QSPI_CRX_BITSE|QSPI_CRX_DSCK|QSPI_CRX_DT|chipSelectUsed);  
 QSPI_BufferREADByte[0] = EEPROM_INST_READ;
 QSPI_BufferREADByte[1] = (BYTE)(startAddress >> 8); // throw away bottom 8-bits
 QSPI_BufferREADByte[2] = (BYTE)(startAddress);  // thow away top 8-bits
 QSPI_BufferREADByte[3] = 0; // used to clock out data

  QSPI_WriteQTR(QSPI_BufferREADByte, TxLen1, ptrConfigREADByte);

// Copy ALL QRR to RXBuff
    EEPROMDataByte = (BYTE)QSPI_READ_QRR(3);// Read Tansfer information - data byte will be in QRR(3).
//  #define RxLen 16
//  QSPI_ReadQRR( ptrRxBuffer, RxLen );         

   g_bSPIEEActive = bSaveSPIEEActive;     //restore   

    return ( EEPROMDataByte ); 
}// close function read

 /*
  * Part 4a: Write a single byte to EEPROM)
  *         A combination of EEPROM_WREN() and Write16()
  *
  */
BOOL EEPROMWrite1Byte(WORD chipSelectUsed,  BYTE TxData, WORD startAddress) 
{
  BOOL bSaveSPIEEActive = g_bSPIEEActive;
  BOOL bOkay = TRUE;
    g_bSPIEEActive = TRUE;    // prevent access to SPI on PIT.
  
    SPI_Select_EEPROM();
    

    unsigned char	cReadValue;
//    SPI_Select_EEPROM();
    cReadValue = EEPROMRead1Byte(chipSelectUsed, startAddress);
	if( cReadValue != TxData )
    {
    EEPROM_WREN(chipSelectUsed);  // write enable the EEPROM
          
    WORD tempTxLen = 1 + 3;  // Write Inst, Addr Hi, Addr Lo, data byte.
    WORD TxBuffer[tempTxLen];
    
    TxBuffer[0] = EEPROM_INST_WRITE;
    TxBuffer[1] = (BYTE)(startAddress >> 8); // throw away bottom 8-bits
    TxBuffer[2] = (BYTE)(startAddress);  // thow away top 8-bits
    TxBuffer[3] = TxData;  // byte to be written. (ignore top 8-bits)
        
    EEPROM_WRITE16(chipSelectUsed, tempTxLen, TxBuffer);
    
	g_nEEPROMWriteTimeoutCounter = EEPROMCOMMANDTIMEOUT;
	do{
		cReadValue = EEPROMRead1Byte(chipSelectUsed, startAddress);
       
	  }while((cReadValue != TxData) && (g_nEEPROMWriteTimeoutCounter != 0));
    
        if(g_nEEPROMWriteTimeoutCounter == 0)
        {
              //if(fdTelnet > 0)
              //iprintf("\n  timeout %d TX %d act %d address %d",g_nEEPROMWriteTimeoutCounter,TxData,cReadValue,startAddress); //nbb--testonly--
              bOkay = FALSE;
        }
    }
    g_bSPIEEActive = bSaveSPIEEActive;     //restore   
    
    return(bOkay);
}


 /*
  *
  * Part 1b: Write disable
  *
  */
  
void EEPROMWriteDisable(WORD chipSelectUsed) 
{
 // Create QSPI Configuration Structure to send write DISABLE instruction
 WORD QSPI_BufferWRDI[1];
 WORD * ptrQSPI_BufferWRDI = QSPI_BufferWRDI;
 
 WORD Len = 1;
 
 QSPI_CFG ConfigWRDI;
 QSPI_CFG * ptrConfigWRDI = &ConfigWRDI;
 
 // QSPI Mode Register
 // A) Setup QMR register (8-bit transfer) 
 ConfigWRDI.QMR = ( QSPI_QMR_MSTR|QSPI_QMR_CPOL|QSPI_QMR_CPHA|QSPI_QMR_BITS_8|QSPI_QMR_DOHIE|QSPI_QMR_BAUD(19) );

 // B) QSPI Delay Register
 ConfigWRDI.QDLYR = QSPI_QDLYR_SPE|QSPI_QDLYR_QCD(0)|QSPI_QDLYR_DTL(1);
 
 // QSPI Wrap Register 
 // C) Setup QWR register to send one byte(8-bits), and set CS to active low
 ConfigWRDI.QWR = ( QSPI_QWR_ENDQP(0)|QSPI_QWR_NEWQP(0)|QSPI_QWR_CSIV );
 
 // D) QSPI Comand RAM Registers[0]
 ConfigWRDI.QCR[0] = (QSPI_CRX_BITSE|QSPI_CRX_DSCK|QSPI_CRX_DT|chipSelectUsed);  
 
 QSPI_BufferWRDI[0] = EEPROM_INST_WRDI;
 
 QSPI_WriteQTR(ptrQSPI_BufferWRDI, Len, ptrConfigWRDI);
 
} // close function


 /*
  * Part 1C: Write a single byte to EEPROM STATUS register
  *
  */
void EEPROMWriteStatusRegister(WORD chipSelectUsed,  BYTE NewStatus) 
{

  EEPROM_WREN(chipSelectUsed);  // write enable the EEPROM (Status Register protected otherwise)
          
 // Create QSPI Configuration Structure to send write TO STATUS
 WORD Len = 2;
 WORD QSPI_BufferWRSR[2];
 WORD * ptrQSPI_BufferWRSR = QSPI_BufferWRSR;
 QSPI_CFG ConfigWRSR;
 QSPI_CFG * ptrConfigWRSR = &ConfigWRSR;
 
 // QSPI Mode Register
 // A) Setup QMR register (8-bit transfer) 
 ConfigWRSR.QMR = ( QSPI_QMR_MSTR|QSPI_QMR_CPOL|QSPI_QMR_CPHA|QSPI_QMR_BITS_8|QSPI_QMR_DOHIE|QSPI_QMR_BAUD(19) );

 // B) QSPI Delay Register
 ConfigWRSR.QDLYR =QSPI_QDLYR_SPE|QSPI_QDLYR_QCD(0)|QSPI_QDLYR_DTL(1);
 
 // QSPI Wrap Register 
 // C) Setup QWR register to send one byte(8-bits), and set CS to active low
 ConfigWRSR.QWR = ( QSPI_QWR_ENDQP(1)|QSPI_QWR_NEWQP(0)|QSPI_QWR_CSIV );
 
 // D) QSPI Comand RAM Registers[0]
 ConfigWRSR.QCR[0] = (QSPI_CRX_BITSE|QSPI_CRX_DSCK|QSPI_CRX_DT|QSPI_CRX_CONT|chipSelectUsed); // note CONT bit set.   
 ConfigWRSR.QCR[1] =(QSPI_CRX_BITSE|QSPI_CRX_DSCK|QSPI_CRX_DT|chipSelectUsed);  

 QSPI_BufferWRSR[0] = EEPROM_INST_WRSR;
 QSPI_BufferWRSR[1] = NewStatus;
 
 QSPI_WriteQTR(ptrQSPI_BufferWRSR, Len, ptrConfigWRSR);
               
}
 /*
  * Part 1d: Read EEPROM STATUS register
  *
  */
BYTE  ReadEEPROMStatusRegister(WORD chipSelectUsed ) 
{ 
          
 // Create QSPI Configuration Structure to read STATUS
 BYTE StatusByte;
 WORD RSLen = 2;
 WORD QSPI_BufferRDSR[2];
 WORD * ptrQSPI_BufferRDSR = QSPI_BufferRDSR;
 QSPI_CFG ConfigRDSR;
 QSPI_CFG * ptrConfigRDSR = &ConfigRDSR;
 
 // QSPI Mode Register
 // A) Setup QMR register (8-bit transfer) 
 ConfigRDSR.QMR = ( QSPI_QMR_MSTR|QSPI_QMR_CPOL|QSPI_QMR_CPHA|QSPI_QMR_BITS_8|QSPI_QMR_DOHIE|QSPI_QMR_BAUD(19) );

 // B) QSPI Delay Register
 ConfigRDSR.QDLYR = QSPI_QDLYR_SPE|QSPI_QDLYR_QCD(0)|QSPI_QDLYR_DTL(1);
 
 // QSPI Wrap Register 
 // C) Setup QWR register to send 2 bytes(8-bits), and set CS to active low
 ConfigRDSR.QWR = ( QSPI_QWR_ENDQP(1)|QSPI_QWR_NEWQP(0)|QSPI_QWR_CSIV );
 
 // D) QSPI Comand RAM Registers[0]
 ConfigRDSR.QCR[0] = (QSPI_CRX_BITSE|QSPI_CRX_DSCK|QSPI_CRX_DT|QSPI_CRX_CONT|chipSelectUsed); // note CONT bit set.   
 ConfigRDSR.QCR[1] =(QSPI_CRX_BITSE|QSPI_CRX_DSCK|QSPI_CRX_DT|chipSelectUsed);  

 QSPI_BufferRDSR[0] = EEPROM_INST_RDSR;
 QSPI_BufferRDSR[1] = 0;	 				// dummy byte.
 
 QSPI_WriteQTR(ptrQSPI_BufferRDSR, RSLen, ptrConfigRDSR);

// Copy ALL QRR to RXBuff
    StatusByte = (BYTE)QSPI_READ_QRR(1);// Read Tansfer information - data byte will be in QRR(1).

    return ( StatusByte ); 
               
}



////////////////////////////////////////////////////////////////
// It's important to verify that the EEPROM has completed any
// previous write cycle since interrupting a write cycle with
// a new write causes the first one to abort resulting in a 
// probable loss of data.
//
// Entry:	Void
//
// Exit:	TRUE if the EEPROM write is finished.
//			FALSE if still in progress.
////////////////////////////////////////////////////////////////
BOOL IsEEPROMWriteFinished( WORD chipSelectUsed )
{
	BOOL bFinished = FALSE;
	unsigned char	cStatus;

    cStatus = ReadEEPROMStatusRegister( chipSelectUsed );

	if( !(cStatus & EEPROM_WIP) )
		bFinished = TRUE;

	return bFinished;
}

////////////////////////////////////////////////////////////////
// WriteProtectEEPROM()
//
// Entry:	ChipSelect for EEPROM
// Function:
//			1. Sets WP\ pin high (needs to be to write to Status Reg.)
//			2. Write WPEN, BP0 and BP1 bits of Status reg.
//			3. Issue WRDI instruction.
//			4. Bring WP\ pin low.
//
// Exit:	
//
////////////////////////////////////////////////////////////////
void WriteProtectEEPROM( WORD chipSelectUsed )
{
	unsigned char	cSReg;
	
	g_nEEPROMWriteTimeoutCounter = EEPROMCOMMANDTIMEOUT;
//todo	EEPROMWPPinHigh();					// set WP\ pin high (hardware)

// must wait till any previous Write in Progress finished (WIP bit cleared).
    cSReg = ReadEEPROMStatusRegister( chipSelectUsed );
    if(cSReg & EEPROM_WIP)      // Is there a write in progress?
    {
        OSTimeDly(1);            // OS blocking function - will allow other tasks to run.
 	    do{
    		cSReg = ReadEEPROMStatusRegister( chipSelectUsed );
  		  }while( (cSReg & EEPROM_WIP) && (g_nEEPROMWriteTimeoutCounter != 0 ));        //bit is cleared when finished
    }

   if(g_nEEPROMReadTimeoutCounter == 0)
    {
//        iprintf("\n  WriteProtectEEPROM has timed out");  //nbb--testonly--
    }

    cSReg = 0x8C;
	EEPROMWriteStatusRegister(chipSelectUsed, cSReg  );	// set bits for write protection
	
	EEPROMWriteDisable( chipSelectUsed);    // issues WRDI instruction.

//todo	EEPROMWPPinLow();						// set WP\ pin low (hardware)

}

////////////////////////////////////////////////////////////////
// UnProtectEEPROM()
//
// Entry:	ChipSelect for EEPROM
// Function:
//			1. Sets WP\ pin high (needs to be to write to Status Reg.)
//			2. Clear WPEN, BP0 and BP1 bits of Status reg. (WREN instruction built int WriteStatusReg.)
//
// Exit:	
//
////////////////////////////////////////////////////////////////
void UnProtectEEPROM( WORD chipSelectUsed )
{
	unsigned char	cStatus;
	

//todo	EEPROMWPPinHigh();					// set WP\ pin high (hardware)


    cStatus = 0x00; //     EEPROM_BP1 | EEPROM_BP0 | EEPROM_WPEN 
	EEPROMWriteStatusRegister(chipSelectUsed, cStatus  );	// clear bits for write protection
	
}


//   MMK end new

  /*
  *
  * Part 4: Write 13 (Write 13 datums to EEPROM)
  *         A combination of EEPROM_WREN() and Write16()
  *
  */
void EEPROM_WRITE13(WORD chipSelectUsed, WORD TxLength, WORD * TxData, WORD startAddress) {

          EEPROM_WREN(chipSelectUsed);  // write enable the EEPROM
          
          WORD tempTxLen = TxLength + 3;  // where TxLen!<=0 (no error checking)
          WORD TxBuffer[tempTxLen];

          TxBuffer[0] = EEPROM_INST_WRITE;
          TxBuffer[1] = (BYTE)(startAddress >> 8); // throw away bottom 8-bits
          TxBuffer[2] = (BYTE)(startAddress);  // thow away top 8-bits
               
          for (int setupData = 3; setupData<tempTxLen; setupData++) {
              TxBuffer[setupData] = *TxData; 
              TxData++;
          }

          EEPROM_WRITE16(chipSelectUsed, tempTxLen, TxBuffer);
}


  /*
  *
  * Part 5: Write Buffer (Write on mupltiple pages of data)
  *
  */
int EEPROM_WriteBuffer( WORD * TxData, WORD chipSelectUsed, WORD startAddress, WORD TxLength) {

    // if there is nothing to be transferred exit.
    if(TxLength<=0) {
        return(-1);  // error flag returned
    }
 
    // This variable will store the number of bytes that still need to be 
    // transfered. 
    int currentOffsetBytes = TxLength;  

    // This variable will store the address of begining of current page.  
    int beginCurrentPage = (startAddress & 0x1FE0);  
    
    // This variable will store the address of end of current page.  
    int endOfCurrentPage = (beginCurrentPage + 0x001F);  
    
    // This variable will store the address to start writing at for the current
    // cycle.  
    int currentStartAddress = startAddress;

    // This variable will store the total number of bytes we can transfer in 
    // current page if needed.
    // EX: endOfCurrentPage=31, currentStartAddress=29,
    //     currentTxBytes= 3
    // In this psudo-case, 29, 20, and 31 will have data written to. 
    int currentTxBytes = (endOfCurrentPage - currentStartAddress + 1); 

    // This variable will store the number of bytes we should send to be written  
    // now, keeping in mind that we can only write 13 bytes of data at a time
    // with the EEPROM_WRITE16() function. 
    int current13Load;   
    
    // This variable is a pointer to data that needs to be transferred. 
    WORD * pData = TxData;
    WORD * pDataTemp = TxData;

    // This loop will continue so long are there are bytes to be written.
    while(currentOffsetBytes > 0) {  
    
        // Update the amount of bytes we can transfer in this register cycle.
        // We only have 13 registers to work with.
        if (currentTxBytes <= 13) {  
             current13Load = currentTxBytes; // load the leftovers
             currentTxBytes = 0;
        }
        else {
             current13Load = 13; // load as much as possible (13) in this cycle.
             currentTxBytes -= 13;
        }                      

        // Initiate current tranfer
          pDataTemp = pData;
          EEPROM_WRITE13(chipSelectUsed, current13Load, pDataTemp, currentStartAddress);

        // Subtract the amount of bytes that were transferred from 
        // currentOffsetBytes.
        currentOffsetBytes -= current13Load;  
        
        // Move data pointer over to new datam location depending on how much 
        // was written.
        pData += current13Load; 

        // Update the address to start writing to next cycle only if we can
        // do so within this page.  
        if (currentTxBytes > 0) {
             currentStartAddress += current13Load;   
        }

        // Since we have no more bytes to transfer in this page, setup variables
        // for the next page.
        if (currentTxBytes==0) {  
             // If there is more than a page of data to write, write the max 
             // possible (1 page (32 bytes) at a time).
             if (currentOffsetBytes>32)
                  currentTxBytes = 32;  // use entire page
             else
                  currentTxBytes = currentOffsetBytes; // write what is leftover

             // Update the address of begining of next page.       
             beginCurrentPage = (endOfCurrentPage+0x1);  
             // Update the address of end of next page.
             endOfCurrentPage = (beginCurrentPage + 0x001F);     
             // Update address to start writing at on the next page if we need
             // to "write" there.
             currentStartAddress = beginCurrentPage;  
        }   
        
    }  // close while loop
    
    return (1);  // success! 
  
} // close function Write Buffer


  /*
  *
  * Part 6: Read 13 datums
  *
  */
/******************************************************************
 * This function will read up to 13 datums to the EEPROM
 * This function is a special case of EEPROM_READ() and QSPI_READ16()
 ******************************************************************/
int EEPROM_Read13(WORD chipSelectUsed, WORD RxLength, WORD * RxData, WORD startAddress) {
 WORD RxBuffer[16];
 WORD * ptrRxBuffer = RxBuffer;
 
 // if there is nothing to be transferred exit.
 if((RxLength>13) || (RxLength<=0)) {
      return(-1);  // error flag returned
 }
 
 EEPROM_READ( chipSelectUsed, startAddress );

 QSPI_READ16( ptrRxBuffer );
 
 for ( int RxDatums = 3; RxDatums < (RxLength + 3); RxDatums++) {
      *RxData = RxBuffer[RxDatums];
      RxData++;
 }
 
 return(1);
 
}  // close function Read 13

  /*
  *
  * Part 7: Read Buffer (Read multiple pages of data)
  *
  */

int EEPROM_ReadBuffer( WORD * RxData, WORD chipSelectUsed, WORD startAddress, WORD RxLength ) {

    // if there is nothing to be transferred exit.
    if(RxLength<=0) {
        return(-1);  // error flag returned
    }
 
    // This variable will store the number of bytes that still need to be 
    // transfered.
    int currentOffsetBytes = RxLength;  

    // This variable will store the address of begining of current page. 
    WORD beginCurrentPage = (startAddress & 0x1FE0);  
    
    // This variable will store the address of end of current page.  
    WORD endOfCurrentPage = (beginCurrentPage + 0x001F);  
    
    // This variable will store the address to start writing at for the current
    // cycle.
    WORD currentStartAddress = startAddress;

    // This variable will store the total number of bytes we can transfer in 
    // current page if needed.
    // EX: endOfCurrentPage=31, currentStartAddress=29,
    //     currentRxBytes= 3
    // In this psudo-case, 29, 20, and 31 will have data written to.
    int currentRxBytes = (endOfCurrentPage - currentStartAddress + 1); 

    // This variable will store the number of bytes we should send to be written  
    // now, keeping in mind that we can only write 13 bytes of data at a time
    // with the EEPROM_WRITE16() function.
    WORD current13Load;   
    
    // This variable is a pointer to data that needs to be transferred. 
    WORD * pData = RxData;

    // This loop will continue so long are there are bytes to be written.
    while(currentOffsetBytes > 0) {  
    
        // Update the amount of bytes we can transfer in this register cycle.
        // We only have 13 registers to work with.
        if (currentRxBytes <= 13) {  
             current13Load = currentRxBytes; // load the leftovers
             currentRxBytes = 0;
        }
        else {
             current13Load = 13; // load as much as possible (13) in this cycle.
             currentRxBytes -= current13Load;
        }                  

        // Initiate current tranfer
          EEPROM_Read13(chipSelectUsed, current13Load, pData, currentStartAddress);

        // Subtract the amount of bytes that were transferred from 
        // currentOffsetBytes.
        currentOffsetBytes -= current13Load;  
        
        // Move data pointer over to new datam location depending on how much 
        // was written.
        pData = (pData + current13Load); 

        // Update the address to start writing to next cycle only if we can
        // do so within this page.  
        if (currentRxBytes > 0) {
             currentStartAddress += current13Load;   
        }

        // Since we have no more bytes to transfer in this page, setup variables
        // for the next page.
        if (currentRxBytes==0) {  
             // If there is more than a page of data to write, write the max 
             // possible (1 page (32 bytes) at a time).
             if (currentOffsetBytes>32)
                  currentRxBytes = 32;  // use entire page
             else
                  currentRxBytes = currentOffsetBytes; // write what is leftover

             // Update the address of begining of next page.       
             beginCurrentPage = (endOfCurrentPage+0x1);  
             // Update the address of end of next page.
             endOfCurrentPage = (beginCurrentPage + 0x001F);     
             // Update address to start writing at on the next page if we need
             // to "write" there.
             currentStartAddress = beginCurrentPage;  
        }   
        
    }  // close while loop
    
    return (1);  // success! 
}


 
