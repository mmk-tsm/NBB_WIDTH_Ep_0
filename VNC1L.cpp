/*-------------------------------------------------------------------
 File: VNC1L.cpp
 Description: Read/Write VNC1L functions.
 Taken from EEPROMdriver.cpp
   
   P.Smith                          30/1/08
   correct compiler warnings
   
   P.Smith                          16/10/08
   correct compiler warnings
   
   P.Smith                          25/2/09
   modify usb read / write to select the usb every time just before the write
   takes place.
   for some reason, the expansion access is causing a problem where 
   the expansion select activated instead of the usb select.
   this makes the usb time out.
-------------------------------------------------------------------*/
#include <startnet.h>

#include <basictypes.h>
#include <stdio.h> 

#include "QSPIconstants.h"
#include "QSPIdriver.h"

#include "VNC1L.h"

#include "NBBGpio.h"
#include <serial.h>

#include <string.h>
#include "strlib.h"



//#include "EEPROMconstants.h"
//#include "EEPROMdriver.h"
extern WORD g_nEX1SPIChipSelect;
int g_nFdVNC1L;
int g_nFdMttty;
int g_nU0Mode = U0_MODE_MTTTY;

int   g_nVRBufIndex = 0;
int   g_nVNC1LErrorCode = 0;
bool   g_bVNC1LReadFinished;
bool   g_bVNC1LReadInProgress = FALSE;
int   g_nVNC1LTimer;    // time allowed.
DWORD g_nVNC1LPITCounter;
DWORD g_nVReadCount = 0;
DWORD g_nVWriteCount = 0;
structVNC1LData   g_sVNC1L;
  /*
  *
  * Part 2: Write data...MAX 16 BYTES.
  *
  */
void VNC1L_WRITE16(WORD chipSelectUsed, WORD TxLen, WORD * ptrTxBuffer) {

 // Create QSPI Configuration Structure 1 to send write instruction
 QSPI_CFG ConfigWRITE;
 QSPI_CFG * ptrConfigWRITE = &ConfigWRITE;
 
 // QSPI Mode Register
 // A) Setup QMR register (11-bit transfer) 
 ConfigWRITE.QMR = ( QSPI_QMR_MSTR|QSPI_QMR_CPOL|QSPI_QMR_CPHA|QSPI_QMR_BITS_11|QSPI_QMR_DOHIE|QSPI_QMR_BAUD(19) );

 // B) QSPI Delay Register
 ConfigWRITE.QDLYR = QSPI_QDLYR_SPE|QSPI_QDLYR_QCD(0)|QSPI_QDLYR_DTL(1);
 
 // QSPI Wrap Register 
 // C) Set up QWR register to send write instruction, address, and data
 // EndQP is TxLen-1 so that 16 = 15...
 ConfigWRITE.QWR = ( QSPI_QWR_ENDQP(TxLen-1)|QSPI_QWR_NEWQP(0)|QSPI_QWR_CSIV );  
 
 // D) QSPI Comand RAM Registers
 ConfigWRITE.QCR[0] = (QSPI_CRX_BITSE|QSPI_CRX_DSCK|QSPI_CRX_DT|chipSelectUsed);       //QSPI_CRX_CONT = 0.
 ConfigWRITE.QCR[1] = (QSPI_CRX_BITSE|QSPI_CRX_DSCK|QSPI_CRX_DT|chipSelectUsed);
 ConfigWRITE.QCR[2] = (QSPI_CRX_BITSE|QSPI_CRX_DSCK|QSPI_CRX_DT|chipSelectUsed); 
 ConfigWRITE.QCR[3] = (QSPI_CRX_BITSE|QSPI_CRX_DSCK|QSPI_CRX_DT|chipSelectUsed);
 ConfigWRITE.QCR[4] = (QSPI_CRX_BITSE|QSPI_CRX_DSCK|QSPI_CRX_DT|chipSelectUsed);
 ConfigWRITE.QCR[5] = (QSPI_CRX_BITSE|QSPI_CRX_DSCK|QSPI_CRX_DT|chipSelectUsed);
 ConfigWRITE.QCR[6] = (QSPI_CRX_BITSE|QSPI_CRX_DSCK|QSPI_CRX_DT|chipSelectUsed);
 ConfigWRITE.QCR[7] = (QSPI_CRX_BITSE|QSPI_CRX_DSCK|QSPI_CRX_DT|chipSelectUsed);
 ConfigWRITE.QCR[8] = (QSPI_CRX_BITSE|QSPI_CRX_DSCK|QSPI_CRX_DT|chipSelectUsed);
 ConfigWRITE.QCR[9]  = (QSPI_CRX_BITSE|QSPI_CRX_DSCK|QSPI_CRX_DT|chipSelectUsed); 
 ConfigWRITE.QCR[10] = (QSPI_CRX_BITSE|QSPI_CRX_DSCK|QSPI_CRX_DT|chipSelectUsed);
 ConfigWRITE.QCR[11] = (QSPI_CRX_BITSE|QSPI_CRX_DSCK|QSPI_CRX_DT|chipSelectUsed);
 ConfigWRITE.QCR[12] = (QSPI_CRX_BITSE|QSPI_CRX_DSCK|QSPI_CRX_DT|chipSelectUsed);
 ConfigWRITE.QCR[13] = (QSPI_CRX_BITSE|QSPI_CRX_DSCK|QSPI_CRX_DT|chipSelectUsed);
 ConfigWRITE.QCR[14] = (QSPI_CRX_BITSE|QSPI_CRX_DSCK|QSPI_CRX_DT|chipSelectUsed); 
 ConfigWRITE.QCR[15] = (QSPI_CRX_BITSE|QSPI_CRX_DSCK|QSPI_CRX_DT|chipSelectUsed);
 
 QSPI_WriteQTR(ptrTxBuffer, TxLen, ptrConfigWRITE);  
 
} // close function write16 


  /*
  *
  * Part 3: Read data   TxLen BYTES.
  *
  */
void VNC1L_READ(WORD chipSelectUsed, BYTE TxLen) 
{

 // Create QSPI Configuration Structure 1 to send write ENABLE instruction
// #define TxLen 16
 WORD QSPI_BufferREAD[16];    // MAX 16 WORDS.
 QSPI_CFG ConfigREAD;
 QSPI_CFG * ptrConfigREAD = &ConfigREAD;
 
 // QSPI Mode Register
 // A) Setup QMR register (11-bit transfer) 
 ConfigREAD.QMR = ( QSPI_QMR_MSTR|QSPI_QMR_CPOL|QSPI_QMR_CPHA|QSPI_QMR_BITS_11|QSPI_QMR_DOHIE|QSPI_QMR_BAUD(19) );

 // B) QSPI Delay Register
 ConfigREAD.QDLYR = QSPI_QDLYR_SPE|QSPI_QDLYR_QCD(0)|QSPI_QDLYR_DTL(1);
 
 // QSPI Wrap Register 
 // C) Setup QWR register to send one byte(8-bits), and set CS to active low
 ConfigREAD.QWR = ( QSPI_QWR_ENDQP(TxLen-1)|QSPI_QWR_NEWQP(0)|QSPI_QWR_CSIV ); // TxLen-1 = register 2
 
 // D) QSPI Comand RAM Registers[0]
 ConfigREAD.QCR[0] = (QSPI_CRX_BITSE|QSPI_CRX_DSCK|QSPI_CRX_DT|chipSelectUsed);  // QSPI_CRX_CONT|=0
 ConfigREAD.QCR[1] = (QSPI_CRX_BITSE|QSPI_CRX_DSCK|QSPI_CRX_DT|chipSelectUsed);
 ConfigREAD.QCR[2] = (QSPI_CRX_BITSE|QSPI_CRX_DSCK|QSPI_CRX_DT|chipSelectUsed);
 ConfigREAD.QCR[3] = (QSPI_CRX_BITSE|QSPI_CRX_DSCK|QSPI_CRX_DT|chipSelectUsed);  
 ConfigREAD.QCR[4] = (QSPI_CRX_BITSE|QSPI_CRX_DSCK|QSPI_CRX_DT|chipSelectUsed);
 ConfigREAD.QCR[5] = (QSPI_CRX_BITSE|QSPI_CRX_DSCK|QSPI_CRX_DT|chipSelectUsed);
 ConfigREAD.QCR[6] = (QSPI_CRX_BITSE|QSPI_CRX_DSCK|QSPI_CRX_DT|chipSelectUsed);  
 ConfigREAD.QCR[7] = (QSPI_CRX_BITSE|QSPI_CRX_DSCK|QSPI_CRX_DT|chipSelectUsed);
 ConfigREAD.QCR[8] = (QSPI_CRX_BITSE|QSPI_CRX_DSCK|QSPI_CRX_DT|chipSelectUsed);
 ConfigREAD.QCR[9] = (QSPI_CRX_BITSE|QSPI_CRX_DSCK|QSPI_CRX_DT|chipSelectUsed);  
 ConfigREAD.QCR[10] = (QSPI_CRX_BITSE|QSPI_CRX_DSCK|QSPI_CRX_DT|chipSelectUsed);
 ConfigREAD.QCR[11] = (QSPI_CRX_BITSE|QSPI_CRX_DSCK|QSPI_CRX_DT|chipSelectUsed);
 ConfigREAD.QCR[12] = (QSPI_CRX_BITSE|QSPI_CRX_DSCK|QSPI_CRX_DT|chipSelectUsed);
 ConfigREAD.QCR[13] = (QSPI_CRX_BITSE|QSPI_CRX_DSCK|QSPI_CRX_DT|chipSelectUsed);  
 ConfigREAD.QCR[14] = (QSPI_CRX_BITSE|QSPI_CRX_DSCK|QSPI_CRX_DT|chipSelectUsed);
 ConfigREAD.QCR[15] = (QSPI_CRX_BITSE|QSPI_CRX_DSCK|QSPI_CRX_DT|chipSelectUsed);
 
 QSPI_BufferREAD[0] = 0; //VNC1L_INST_READ;
 QSPI_BufferREAD[1] = 0; //(BYTE)(startAddress >> 8); // throw away bottom 8-bits
 QSPI_BufferREAD[2] = 0; // (BYTE)(startAddress);  // thow away top 8-bits
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
WORD VNC1LRead1Byte(WORD chipSelectUsed, BYTE RegisterType) 
{
      SPI_Select_USB ( );  // set SPI chip selects for SPI.  //nbb--todo--put this back
//    BYTE VNC1LDataByte;
    WORD VNC1LDataWord;
/*    
    BYTE cSReg;
// must wait till any previous Write in Progress finished (WIP bit cleared).
    cSReg = ReadEEPROMStatusRegister( chipSelectUsed );
    if(cSReg & EEPROM_WIP)      // Is there a write in progress?
    {
        OSTimeDly(1);            // OS blocking function - will allow other tasks to run.
 	    do{
    		cSReg = ReadEEPROMStatusRegister( chipSelectUsed );
  		  }while( cSReg & EEPROM_WIP );        //bit is cleared when finished
    }
*/
   
    
 // Create QSPI Configuration Structure to send read instruction
 //#define TxLen1 4     // 3 instruction bytes (READ ADD_Hi_Byte, ADD_Lo_Byte + data read 0,1,2,3
 #define TXLEN001 (1)
 WORD QSPI_BufferREADByte[4];
 QSPI_CFG ConfigREADByte;
 QSPI_CFG * ptrConfigREADByte = &ConfigREADByte;
 WORD nTemp;
 
 // QSPI Mode Register
 // A) Setup QMR register (12-bit transfer) 
 ConfigREADByte.QMR = ( QSPI_QMR_MSTR|QSPI_QMR_CPOL|QSPI_QMR_CPHA|QSPI_QMR_BITS_12|QSPI_QMR_DOHIE|QSPI_QMR_BAUD(19) );

 // B) QSPI Delay Register
 ConfigREADByte.QDLYR = QSPI_QDLYR_SPE|QSPI_QDLYR_QCD(0)|QSPI_QDLYR_DTL(1);
 
 // QSPI Wrap Register 
 // C) Setup QWR register to send one byte(11-bits), and set CS to active low
 ConfigREADByte.QWR = ( QSPI_QWR_ENDQP(TXLEN001-1)|QSPI_QWR_NEWQP(0)|QSPI_QWR_CSIV ); // 
 
 // D) QSPI Comand RAM Registers[0]
 ConfigREADByte.QCR[0] = (QSPI_CRX_BITSE|QSPI_CRX_DSCK|QSPI_CRX_DT|chipSelectUsed);  // QSPI_CRX_CONT| = 0
 /*
 ConfigREADByte.QCR[1] = (QSPI_CRX_BITSE|QSPI_CRX_DSCK|QSPI_CRX_DT|QSPI_CRX_CONT|chipSelectUsed);
 ConfigREADByte.QCR[2] = (QSPI_CRX_BITSE|QSPI_CRX_DSCK|QSPI_CRX_DT|QSPI_CRX_CONT|chipSelectUsed);
 ConfigREADByte.QCR[3] = (QSPI_CRX_BITSE|QSPI_CRX_DSCK|QSPI_CRX_DT|chipSelectUsed); 
 */ 
 /*ConfigREAD.QCR[4] = (QSPI_CRX_BITSE|QSPI_CRX_DSCK|QSPI_CRX_DT|QSPI_CRX_CONT|chipSelectUsed);
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
 */
   if(RegisterType == VNC1L_DATA_REGISTER)
      nTemp = 0xFC00;         //4 unused 1's at beg, start bit = 1, R/W = 1, ADD=0, 8 0's, one 0 corresponding to status bit.
   else
      nTemp = 0xFE00;
      
 QSPI_BufferREADByte[0] =  nTemp ; //
// QSPI_BufferREADByte[1] = 0; //(BYTE)(startAddress >> 8); // throw away bottom 8-bits
// QSPI_BufferREADByte[2] = 0; //(BYTE)(startAddress);  // thow away top 8-bits
// QSPI_BufferREADByte[3] = 0; // used to clock out data
/* QSPI_BufferREAD[4] = 0;
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
*/  

  QSPI_WriteQTR(QSPI_BufferREADByte, TXLEN001, ptrConfigREADByte);

// Copy ALL QRR to RXBuff
//    VNC1LDataByte = (BYTE)QSPI_READ_QRR(0);// Read Tansfer information - data byte will be in QRR(3).
    VNC1LDataWord = QSPI_READ_QRR(0);
//  #define RxLen 16
//  QSPI_ReadQRR( ptrRxBuffer, RxLen );         

    return ( VNC1LDataWord ); 
}// close function read

 /*
  * Part 4a: Write a single byte 
  *         
  *
  */
WORD VNC1LWrite1Byte(WORD chipSelectUsed,  BYTE TxData, BYTE RegisterType ) 
{
      SPI_Select_USB ( );  // set SPI chip selects for SPI.  //nbb--todo--put this back

//	unsigned char	cSReg;
// must wait till any previous Write in Progress finished (WIP bit cleared).
/*    cSReg = ReadEEPROMStatusRegister( chipSelectUsed );
    if(cSReg & EEPROM_WIP)      // Is there a write in progress?
    {
        OSTimeDly(1);            // OS blocking function - will allow other tasks to run.
 	    do{
    		cSReg = ReadEEPROMStatusRegister( chipSelectUsed );
  		  }while( cSReg & EEPROM_WIP );        //bit is cleared when finished
    }

          EEPROM_WREN(chipSelectUsed);  // write enable the EEPROM
*/
         
//          WORD tempTxLen = 1; // + 3;  // Write Inst, Addr Hi, Addr Lo, data byte.
//          WORD TxBuffer[tempTxLen];

/////////////////          TxBuffer[0] = (TxData <<1) | 0x0001;
//          TxBuffer[1] = (BYTE)(startAddress >> 8); // throw away bottom 8-bits
//          TxBuffer[2] = (BYTE)(startAddress);  // thow away top 8-bits
//          TxBuffer[3] = TxData;  // byte to be written. (ignore top 8-bits)
              

///////////////          VNC1L_WRITE16(chipSelectUsed, tempTxLen, TxBuffer);

//22.01.2007
//    BYTE VNC1LDataByte;
    WORD VNC1LDataWord;
/*    
    BYTE cSReg;
// must wait till any previous Write in Progress finished (WIP bit cleared).
    cSReg = ReadEEPROMStatusRegister( chipSelectUsed );
    if(cSReg & EEPROM_WIP)      // Is there a write in progress?
    {
        OSTimeDly(1);            // OS blocking function - will allow other tasks to run.
 	    do{
    		cSReg = ReadEEPROMStatusRegister( chipSelectUsed );
  		  }while( cSReg & EEPROM_WIP );        //bit is cleared when finished
    }
*/
   
    
 // Create QSPI Configuration Structure to send read instruction
 //#define TxLen1 4     // 3 instruction bytes (READ ADD_Hi_Byte, ADD_Lo_Byte + data read 0,1,2,3
 #define TXLEN_1 (1)
 WORD QSPI_BufferWriteByte[4];
 QSPI_CFG ConfigWriteByte;
 QSPI_CFG * ptrConfigWriteByte = &ConfigWriteByte;
 WORD nTemp;
 
 // QSPI Mode Register
 // A) Setup QMR register (12-bit transfer) 
 ConfigWriteByte.QMR = ( QSPI_QMR_MSTR|QSPI_QMR_CPOL|QSPI_QMR_CPHA|QSPI_QMR_BITS_12|QSPI_QMR_DOHIE|QSPI_QMR_BAUD(19) );

 // B) QSPI Delay Register
 ConfigWriteByte.QDLYR = QSPI_QDLYR_SPE|QSPI_QDLYR_QCD(0)|QSPI_QDLYR_DTL(1);
 
 // QSPI Wrap Register 
 // C) Setup QWR register to send one byte(12-bits), and set CS to active low
 ConfigWriteByte.QWR = ( QSPI_QWR_ENDQP(TXLEN001-1)|QSPI_QWR_NEWQP(0)|QSPI_QWR_CSIV ); // 
 
 // D) QSPI Comand RAM Registers[0]
 ConfigWriteByte.QCR[0] = (QSPI_CRX_BITSE|QSPI_CRX_DSCK|QSPI_CRX_DT|chipSelectUsed);  // QSPI_CRX_CONT| = 0
 /*
 ConfigWriteByte.QCR[1] = (QSPI_CRX_BITSE|QSPI_CRX_DSCK|QSPI_CRX_DT|QSPI_CRX_CONT|chipSelectUsed);
 ConfigWriteByte.QCR[2] = (QSPI_CRX_BITSE|QSPI_CRX_DSCK|QSPI_CRX_DT|QSPI_CRX_CONT|chipSelectUsed);
 ConfigWriteByte.QCR[3] = (QSPI_CRX_BITSE|QSPI_CRX_DSCK|QSPI_CRX_DT|chipSelectUsed); 
 */ 
 /*ConfigREAD.QCR[4] = (QSPI_CRX_BITSE|QSPI_CRX_DSCK|QSPI_CRX_DT|QSPI_CRX_CONT|chipSelectUsed);
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
 */
  nTemp = (TxData <<1);       // move byte to be written 1 bit left    
  if(RegisterType == VNC1L_DATA_REGISTER)
      nTemp |= 0xF800;         //4 unused 1's at beg, start bit = 1, R/W = 0, ADD=0, 8 0's, one 0 corresponding to status bit.
   else
      nTemp |= 0xFA00;
      
 QSPI_BufferWriteByte[0] =  nTemp ; //
// QSPI_BufferWriteByte[1] = 0; //(BYTE)(startAddress >> 8); // throw away bottom 8-bits
// QSPI_BufferWriteByte[2] = 0; //(BYTE)(startAddress);  // thow away top 8-bits
// QSPI_BufferWriteByte[3] = 0; // used to clock out data
/* QSPI_BufferREAD[4] = 0;
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
*/  

  QSPI_WriteQTR(QSPI_BufferWriteByte, TXLEN_1, ptrConfigWriteByte);

// Copy ALL QRR to RXBuff
//    VNC1LDataByte = (BYTE)QSPI_READ_QRR(0);// Read Tansfer information - data byte will be in QRR(3).
    VNC1LDataWord = QSPI_READ_QRR(0);
//  #define RxLen 16
//  QSPI_ReadQRR( ptrRxBuffer, RxLen );         

    return ( VNC1LDataWord ); 
}


void VNC1LWriteByte( BYTE DataByte )
{
WORD nTemp;
int i=0;
              iprintf("%c",DataByte);     // display char to be send 
              do{

                 CheckForVNC1LData( );
                 nTemp = VNC1LWrite1Byte(VNC1L_SPI_CHIP_SELECT_REVB2,  DataByte, VNC1L_DATA_REGISTER ); 
                 i++;
              } while( (nTemp & 0x0001) && i<3000 );
//dummy read.              
//         nTemp = VNC1LRead1Byte( VNC1L_SPI_CHIP_SELECT, VNC1L_DATA_REGISTER ); 
}

void CheckForVNC1LData( void )
{
BYTE cTemp;
int i;
      for(i=0; i<20; i++)
      {
         WORD nTemp = VNC1LRead1Byte( VNC1L_SPI_CHIP_SELECT_REVB2, VNC1L_DATA_REGISTER );
         if( (nTemp & 0x0001) == 0) // is data new (b0 = 0)
         {
            cTemp = BYTE(nTemp >> 1);
            iprintf("%c", cTemp);     // display char received. 
            if( cTemp == 0x0D )
               iprintf("\n");          // add a line feed.
         }
      }
}      


///***************New 14.11.2007**********************///
BYTE g_cVNC1LWriteBuffer[1024];
BYTE g_cVNC1LReadBuffer[1024];
BYTE g_cVNC1LStatusBuffer[32];
int g_nVNC1LWriteLen;

// "Setting mode to Shortened command set
void SetVNC1LMode_SCS( void )
{
//SCS<CR>
   BYTE CmdSet[4] = {0x53,0x43,0x53,0x0D};
   BYTE *pVNC1LWriteData = g_cVNC1LWriteBuffer;
   int nWriteLen;
   int i;
   
   nWriteLen = 4;
   for(i=0; i<4; i++)
   {
      g_cVNC1LWriteBuffer[i] = CmdSet[i];     
   }
   
   TransmitVNC1L(g_nFdVNC1L, pVNC1LWriteData, nWriteLen);
   
}
// read disk directory command via VNC1L.
void VNC1LReadDir( void )
{
//SCS<CR>
//   Command = 01 0D
   BYTE *pVNC1LWriteData = g_cVNC1LWriteBuffer;
   int nWriteLen;
   
      nWriteLen = 2;
      g_cVNC1LWriteBuffer[0] = 0x01;
      g_cVNC1LWriteBuffer[1] = 0x0d;
  
   TransmitVNC1L(g_nFdVNC1L, pVNC1LWriteData, nWriteLen);
   
}

void VNC1L_OpenFileForWrite( char * Filename )
{
// need to transmit 09 20 filename 0D
   BYTE *pVNC1LWriteData;
   int i=0;
   int nWriteLen;


    char    cBuffer[20];
    sprintf( cBuffer, Filename  );     // 
//    strcat( cBuffer, ".txt");        // assume the ending is in Filename.

      g_cVNC1LWriteBuffer[0] = 0x09;     
      g_cVNC1LWriteBuffer[1] = 0x20;   
      while( cBuffer[i] != 0x00)
      {
         g_cVNC1LWriteBuffer[i+2] = cBuffer[i];
         i++;
      }  

      g_cVNC1LWriteBuffer[i+2] = 0x0D; 
      nWriteLen = i+3;                 // no. of bytes in message.
      pVNC1LWriteData = g_cVNC1LWriteBuffer;
      
   TransmitVNC1L(g_nFdVNC1L, pVNC1LWriteData, nWriteLen);

}
// Open file for read
void VNC1L_OpenFileForRead( char * Filename )
{
// need to transmit 0E 20 filename 0D
   BYTE *pVNC1LWriteData;
   int i=0;
   int nWriteLen;


    char    cBuffer[20];
    sprintf( cBuffer, Filename  );     // 
//    strcat( cBuffer, ".txt");        // assume the ending is in Filename.

      g_cVNC1LWriteBuffer[0] = 0x0E;     
      g_cVNC1LWriteBuffer[1] = 0x20;   
      while( cBuffer[i] != 0x00)
      {
         g_cVNC1LWriteBuffer[i+2] = cBuffer[i];
         i++;
      }  

      g_cVNC1LWriteBuffer[i+2] = 0x0D; 
      nWriteLen = i+3;                 // no. of bytes in message.
      pVNC1LWriteData = g_cVNC1LWriteBuffer;
      
   TransmitVNC1L(g_nFdVNC1L, pVNC1LWriteData, nWriteLen);

}

//Close file
void VNC1L_CloseFile( char * Filename )
{
// need to transmit 0A 20 filename 0D
   BYTE *pVNC1LWriteData;
   int i=0;
   int nWriteLen;


    char    cBuffer[20];
    sprintf( cBuffer, Filename  );     // 
      g_cVNC1LWriteBuffer[0] = 0x0A;     
      g_cVNC1LWriteBuffer[1] = 0x20;   
      while( cBuffer[i] != 0x00)
      {
         g_cVNC1LWriteBuffer[i+2] = cBuffer[i];
         i++;
      }  

      g_cVNC1LWriteBuffer[i+2] = 0x0D; 
      nWriteLen = i+3;                 // no. of bytes in message.
      pVNC1LWriteData = g_cVNC1LWriteBuffer;
      
   TransmitVNC1L(g_nFdVNC1L, pVNC1LWriteData, nWriteLen);

}
void VNC1L_WriteDataToFile( BYTE * pData,  int nLen )
{

// need to transmit 08 20 dWordLen 0D data
   BYTE *pVNC1LWriteData;
   int i=0;
   int nWriteLen;
   DWORD nWriteBytes;
//   char    cBuffer[20];
    
   nWriteBytes = nLen; 
   
   g_cVNC1LWriteBuffer[0] = 0x08;     
   g_cVNC1LWriteBuffer[1] = 0x20;
   g_cVNC1LWriteBuffer[2] = (BYTE)(nWriteBytes>>24); 
   g_cVNC1LWriteBuffer[3] = (BYTE)(nWriteBytes>>16); 
   g_cVNC1LWriteBuffer[4] = (BYTE)(nWriteBytes>>8); 
   g_cVNC1LWriteBuffer[5] = (BYTE)(nWriteBytes); 
   
   g_cVNC1LWriteBuffer[6] = 0x0D; 
   

   for(i=0; i<nLen; i++)
   {
      g_cVNC1LWriteBuffer[7+i] =*(pData+i);
   }         

   nWriteLen = nLen+7;                 // no. of bytes in message.
   pVNC1LWriteData = g_cVNC1LWriteBuffer;
   
      
   TransmitVNC1L(g_nFdVNC1L, pVNC1LWriteData, nWriteLen);

}

// Read data from file (file already open)
void VNC1L_ReadDataFromFile( int nLen )
{
      SPI_Select_USB ( );  // set SPI chip selects for SPI.  //nbb--todo--put this back
// need to transmit 0B 20 dWordLen 0D 
   BYTE *pVNC1LWriteData;
//   int i=0;
   int nWriteLen;
   DWORD nReadBytes;
//   char    cBuffer[20];
    
   nReadBytes = nLen; 
   
   g_cVNC1LWriteBuffer[0] = 0x0B;     
   g_cVNC1LWriteBuffer[1] = 0x20;
   g_cVNC1LWriteBuffer[2] = (BYTE)(nReadBytes>>24); 
   g_cVNC1LWriteBuffer[3] = (BYTE)(nReadBytes>>16); 
   g_cVNC1LWriteBuffer[4] = (BYTE)(nReadBytes>>8); 
   g_cVNC1LWriteBuffer[5] = (BYTE)(nReadBytes); 
   
   g_cVNC1LWriteBuffer[6] = 0x0D; 
   


   nWriteLen = 7;                 // no. of bytes in message.
   pVNC1LWriteData = g_cVNC1LWriteBuffer;
   
      
   TransmitVNC1L(g_nFdVNC1L, pVNC1LWriteData, nWriteLen);

}


//Test version only to check if I have the correct data.
void TransmitVNC1L(int fd0, BYTE* pVNC1LWriteData, int nWriteLen)
{
int i;
char cTemp[2];
char * pChar = cTemp;
/*
   for(i=0; i<nWriteLen; i++)
   {
      iprintf("%2x ", g_cVNC1LWriteBuffer[i]);
   }
   iprintf("\n" );
   for(i=0; i<nWriteLen; i++)
   {
      putchar( *(pVNC1LWriteData+i) );
   
   }
   iprintf( "\n");
*/ 
   for(i=0; i<nWriteLen; i++)
   {
      while( ReadVNC1L_RTS() ) ;    //wait until VNC1L's RTS goes low.
      cTemp[0] = *(pVNC1LWriteData+i);
      
      write( fd0, pChar, 1 );
      
//      putchar( *(pVNC1LWriteData+i) );
   
   }
  
}
//
int ReadDataFromVNC1L( int nSecs )
{
int i;
int j = 0;
char cTemp[10];
//char * pCTemp = cTemp;
      if(g_nU0Mode != U0_MODE_USB )     
         RestoreU0VNC1LSettings( );      
/*
      U0_Select_USB();                 // direct U0 to VNC1l chip.     
//      U0_Select_Mtty();                 // direct U0 to VNC1l chip.     
      SerialClose( 0 );               
      int fd = OpenSerial( 0, 9600, 1, 8, eParityNone );      
//      OpenSerial( 0, 115200, 1, 8, eParityNone );      
//      SimpleOpenSerial( 0, 9600 );     // set baud rate to 9600
 */     
      SetVNC1L_CTS_Lo();      // i.e. allow VNC1L to txmit.
      
      for(i=0; i<(nSecs*100); i++)      //read for n seconds.
      {
/*  This works but uses timeout.      
         if( ReadWithTimeout( fd, cTemp, 1, 1 ) )     //the timeout value = 1, i.e. 1 tick.
         {
            g_cVNC1LReadBuffer[j] = cTemp[0] ;
            j++;
         }
*/
         OSTimeDly(1);      // wait 1 tick - 100th of sec.
          if(dataavail(g_nFdVNC1L) )
         {
            read( g_nFdVNC1L, cTemp, 1 );
            g_cVNC1LReadBuffer[j] = cTemp[0];
            j++;
         }
         
      }
      SetVNC1L_CTS_Hi();      // i.e. stop VNC1L txmit.
      
      return(j);     // return no. of bytes read.
}

// Attempt to write to USB flash disk
void WriteToUSBFlashDisk( void )
{
BYTE vPrintBuffer[1024];
   int nBytesRead,i;
   
      USB_5V_ENABLE(); 
      VNC1L_PROG_HIGH();
      VNC1L_RESET_LOW();
      OSTimeDly(TICKS_PER_SECOND/10);      // wait .1 second.
      VNC1L_RESET_HIGH();

      nBytesRead = ReadDataFromVNC1L(10);    // read for 10 seconds.

      if(g_nU0Mode != U0_MODE_MTTTY)     
         RestoreU0MttySettings( );      
/*      
      U0_Select_Mtty();                 // direct U0 to Mttty     
      SerialClose( 0 );               
      OpenSerial( 0, 115200, 1, 8, eParityNone );      
*/
      if(nBytesRead)
      {
         VPrintf( g_cVNC1LReadBuffer, nBytesRead );
      }     
// Go to short command mode
      if( g_nU0Mode != U0_MODE_USB )     
         RestoreU0VNC1LSettings();
      SetVNC1LMode_SCS();     

      nBytesRead = ReadDataFromVNC1L(4);    // read for 4 seconds.
      if(g_nU0Mode != U0_MODE_MTTTY)     
         RestoreU0MttySettings( );      
      if(nBytesRead)
      {
         VPrintf( g_cVNC1LReadBuffer, nBytesRead );
      }     
// Read directory
      if( g_nU0Mode != U0_MODE_USB )     
         RestoreU0VNC1LSettings();
      VNC1LReadDir();     //read directory.

      nBytesRead = ReadDataFromVNC1L(10);    // read for 10 seconds.
      if(g_nU0Mode != U0_MODE_MTTTY)     
         RestoreU0MttySettings( );      
      if(nBytesRead)
      {
         VPrintf( g_cVNC1LReadBuffer, nBytesRead );
      }    
       
// Open a file for writing.
      if( g_nU0Mode != U0_MODE_USB )     
         RestoreU0VNC1LSettings();
         
      VNC1L_OpenFileForWrite( "MMKF001.Txt" );     //open a file for writing.

      nBytesRead = ReadDataFromVNC1L(5);    // read for 5 seconds.
      if(g_nU0Mode != U0_MODE_MTTTY)     
         RestoreU0MttySettings( );      
      if(nBytesRead)
      {
         VPrintf( g_cVNC1LReadBuffer, nBytesRead );
      }    

// Write data to a file.
   char cTemp = '1';
      for(i=0; i<1024; i++)
      {
         vPrintBuffer[i] = cTemp++;
         if(cTemp == '9')
            cTemp = '0';
         
      }
         
      if( g_nU0Mode != U0_MODE_USB )     
         RestoreU0VNC1LSettings();
         
      VNC1L_WriteDataToFile( vPrintBuffer,  1000 );
      nBytesRead = ReadDataFromVNC1L(4);    // read for 4 seconds.
      if(g_nU0Mode != U0_MODE_MTTTY)     
         RestoreU0MttySettings( );      
      if(nBytesRead)
      {
         VPrintf( g_cVNC1LReadBuffer, nBytesRead );
      } 
         
// Close a file.
      if( g_nU0Mode != U0_MODE_USB )     
         RestoreU0VNC1LSettings();
         
      VNC1L_CloseFile( "MMKF001.Txt" );     //close file.

      nBytesRead = ReadDataFromVNC1L(4);    // read for 4 seconds.
      if(g_nU0Mode != U0_MODE_MTTTY)     
         RestoreU0MttySettings( );      
      if(nBytesRead)
      {
         VPrintf( g_cVNC1LReadBuffer, nBytesRead );
      }    
 
// Open a file for reading.
      if( g_nU0Mode != U0_MODE_USB )     
         RestoreU0VNC1LSettings();
         
      VNC1L_OpenFileForRead( "Test110.Txt" );     //open a file for writing.

      nBytesRead = ReadDataFromVNC1L(4);    // read for 4 seconds.
      if(g_nU0Mode != U0_MODE_MTTTY)     
         RestoreU0MttySettings( );      
      if(nBytesRead)
      {
         VPrintf( g_cVNC1LReadBuffer, nBytesRead );
      }    
// read data from file
// Open a file for reading.
      if( g_nU0Mode != U0_MODE_USB )     
         RestoreU0VNC1LSettings();
         
      VNC1L_ReadDataFromFile( 100 );     //read 100 bytes from currently open file.

      nBytesRead = ReadDataFromVNC1L(10);    // read for 10 seconds.
      if(g_nU0Mode != U0_MODE_MTTTY)     
         RestoreU0MttySettings( );      
      if(nBytesRead)
      {
         VPrintf( g_cVNC1LReadBuffer, nBytesRead );
      }    
    
// Close the file.
      if( g_nU0Mode != U0_MODE_USB )     
         RestoreU0VNC1LSettings();
         
      VNC1L_CloseFile( "Test110.Txt" );     //close file.

      nBytesRead = ReadDataFromVNC1L(4);    // read for 4 seconds.
      if(g_nU0Mode != U0_MODE_MTTTY)     
         RestoreU0MttySettings( );      
      if(nBytesRead)
      {
         VPrintf( g_cVNC1LReadBuffer, nBytesRead );
      }    
       
}

// restore U0 to Mttty settings.
void RestoreU0MttySettings( void )
{
      U0_Select_Mtty();                 // direct U0 to Mttty     
      SerialClose( 0 );               
      g_nFdMttty = OpenSerial( 0, 115200, 1, 8, eParityNone ); 
      g_nU0Mode = U0_MODE_MTTTY;     
}


void RestoreU0VNC1LSettings( void )
{
      U0_Select_USB();                 // direct U0 to VNC1L - USB chip.    
      SerialClose( 0 );               
      g_nFdVNC1L = OpenSerial( 0, 9600, 1, 8, eParityNone );      
      g_nU0Mode = U0_MODE_USB;     
}


//
void VPrintf( BYTE * pData, int nBytes )
{  
int i;       
      if(g_nU0Mode != U0_MODE_MTTTY)     
         RestoreU0MttySettings( );      
      
         for(i=0; i<nBytes; i++)
         {
            iprintf("%2x ", *(pData+i));
         }
        
         iprintf("\n");
         for(i=0; i<nBytes; i++)
         {
            iprintf("%c", *(pData+i));
            if(*(pData+i) == 0x0d)   // cr
               iprintf("\n");
         }
//      iprintf("\nFinished Read.\n");
         
}

//Read data from VNC1L in SPI mode for nSeconds.
int ReadDataFromVNC1LSPIMode( int nSecs )
{
int i,k;
int j = 0;
WORD nTemp;
char cTemp;
   
//      SetVNC1L_CTS_Lo();      // i.e. allow VNC1L to txmit.
      
      for(i=0; i<(nSecs*100); i++)      //read for n seconds.
      {

         OSTimeDly(1);      // wait 1 tick - 100th of sec.
         for(k=0; k<10; k++)
         {
            nTemp = VNC1LRead1Byte( VNC1L_SPI_CHIP_SELECT_REVB2, VNC1L_DATA_REGISTER );
            if( (nTemp & 0x0001) == 0) // is data new (b0 = 0)
            {
               cTemp = BYTE(nTemp >> 1);
               g_cVNC1LReadBuffer[j] = cTemp;
               j++;
            } 
         }       
/*         
          if(dataavail(g_nFdVNC1L) )
         {
            read( g_nFdVNC1L, cTemp, 1 );
            g_cVNC1LReadBuffer[j] = cTemp[0];
            j++;
         }
*/         
      }
//      SetVNC1L_CTS_Hi();      // i.e. stop VNC1L txmit.
      
      return(j);     // return no. of bytes read.
}

//Poll data from VNC1L in SPI mode until prompt (> cr [3E 0d] received for nSeconds max.
int PollVNC1LUntilPrompt_SPI( int nSecs )
{
int i=0;
int k;
int j = 0;
WORD nTemp;
char cTemp;
bool bDone = FALSE;   
//      SetVNC1L_CTS_Lo();      // i.e. allow VNC1L to txmit.
      while( i<(nSecs*100) && !bDone)
//      for(i=0; i<(nSecs*100); i++)      //read for n seconds.
      {

         OSTimeDly(1);      // wait 1 tick - 100th of sec.
         for(k=0; k<5; k++)
         {
            nTemp = VNC1LRead1Byte( VNC1L_SPI_CHIP_SELECT_REVB2, VNC1L_DATA_REGISTER );
            if( (nTemp & 0x0001) == 0) // is data new (b0 = 0)
            {
               cTemp = BYTE(nTemp >> 1);
               g_cVNC1LReadBuffer[j] = cTemp;
               if( j>1 && g_cVNC1LReadBuffer[j-1] == 0x3E  && g_cVNC1LReadBuffer[j] == 0x0D ) // 3E oD i.e prompt.
                  bDone = TRUE;
               j++;
               i++;
            } 
         }       
        
      }
//      SetVNC1L_CTS_Hi();      // i.e. stop VNC1L txmit.
      
      return(j);     // return no. of bytes read.
}

void TransmitVNC1LSPIMode(BYTE* pVNC1LWriteData, int nWriteLen)
{
int i;
//char * pChar = cTemp;
/*
   for(i=0; i<nWriteLen; i++)
   {
      iprintf("%2x ", g_cVNC1LWriteBuffer[i]);
   }
   iprintf("\n" );
   for(i=0; i<nWriteLen; i++)
   {
      putchar( *(pVNC1LWriteData+i) );
   
   }
   iprintf( "\n");
*/ 
   for(i=0; i<nWriteLen; i++)
   {
      VNC1LWriteByte( *(pVNC1LWriteData+i) );
         
//      while( ReadVNC1L_RTS() ) ;    //wait until VNC1L's RTS goes low.
//      cTemp[0] = *(pVNC1LWriteData+i);
      
//      write( fd0, pChar, 1 );
      
//      putchar( *(pVNC1LWriteData+i) );
   
   }
}
//
/*
      for(i=0; i<20; i++)
      {
         WORD nTemp = VNC1LRead1Byte( VNC1L_SPI_CHIP_SELECT_REVB2, VNC1L_DATA_REGISTER );
         if( (nTemp & 0x0001) == 0) // is data new (b0 = 0)
         {
            cTemp = BYTE(nTemp >> 1);
            iprintf("%c", cTemp);     // display char received. 
            if( cTemp == 0x0D )
               iprintf("\n");          // add a line feed.
         }
      }
*/

// "Setting mode to Shortened command set - using SPI
void SetVNC1LMode_SCS_SPI( void )
{
//SCS<CR>
   BYTE CmdSet[4] = {0x53,0x43,0x53,0x0D};
   BYTE *pVNC1LWriteData = g_cVNC1LWriteBuffer;
   int nWriteLen;
   int i;
   
   nWriteLen = 4;
   for(i=0; i<4; i++)
   {
      g_cVNC1LWriteBuffer[i] = CmdSet[i];     
   }
   TransmitVNC1LSPIMode( pVNC1LWriteData, nWriteLen);   
//   TransmitVNC1L(g_nFdVNC1L, pVNC1LWriteData, nWriteLen);
   
}

// read disk directory command via VNC1L using SPI
void VNC1LReadDir_SPI( void )
{
//SCS<CR>
//   Command = 01 0D
   BYTE *pVNC1LWriteData = g_cVNC1LWriteBuffer;
   int nWriteLen;
   
      nWriteLen = 2;
      g_cVNC1LWriteBuffer[0] = 0x01;
      g_cVNC1LWriteBuffer[1] = 0x0d;
  
//   TransmitVNC1L(g_nFdVNC1L, pVNC1LWriteData, nWriteLen);
   TransmitVNC1LSPIMode( pVNC1LWriteData, nWriteLen);    
}

//Open file for write in SPI mode
void VNC1L_OpenFileForWrite_SPI( char * Filename )
{
// need to transmit 09 20 filename 0D
   BYTE *pVNC1LWriteData;
   int i=0;
   int nWriteLen;


    char    cBuffer[20];
    sprintf( cBuffer, Filename  );     // 
//    strcat( cBuffer, ".txt");        // assume the ending is in Filename.

      g_cVNC1LWriteBuffer[0] = 0x09;     
      g_cVNC1LWriteBuffer[1] = 0x20;   
      while( cBuffer[i] != 0x00)
      {
         g_cVNC1LWriteBuffer[i+2] = cBuffer[i];
         i++;
      }  

      g_cVNC1LWriteBuffer[i+2] = 0x0D; 
      nWriteLen = i+3;                 // no. of bytes in message.
      pVNC1LWriteData = g_cVNC1LWriteBuffer;
      
//   TransmitVNC1L(g_nFdVNC1L, pVNC1LWriteData, nWriteLen);
   TransmitVNC1LSPIMode( pVNC1LWriteData, nWriteLen);    

}
// Write data to file on USB stick - SPI mode.
void VNC1L_WriteDataToFile_SPI( BYTE * pData,  int nLen )
{
// need to transmit 08 20 dWordLen 0D data
   BYTE *pVNC1LWriteData;
   int i=0;
   int nWriteLen;
   DWORD nWriteBytes;
//   char    cBuffer[20];
    
   nWriteBytes = nLen; 
   
   g_cVNC1LWriteBuffer[0] = 0x08;     
   g_cVNC1LWriteBuffer[1] = 0x20;
   g_cVNC1LWriteBuffer[2] = (BYTE)(nWriteBytes>>24); 
   g_cVNC1LWriteBuffer[3] = (BYTE)(nWriteBytes>>16); 
   g_cVNC1LWriteBuffer[4] = (BYTE)(nWriteBytes>>8); 
   g_cVNC1LWriteBuffer[5] = (BYTE)(nWriteBytes); 
   
   g_cVNC1LWriteBuffer[6] = 0x0D; 
   

   for(i=0; i<nLen; i++)
   {
      g_cVNC1LWriteBuffer[7+i] =*(pData+i);
   }         

   nWriteLen = nLen+7;                 // no. of bytes in message.
   pVNC1LWriteData = g_cVNC1LWriteBuffer;
   
   TransmitVNC1LSPIMode( pVNC1LWriteData, nWriteLen);          
//   TransmitVNC1L(g_nFdVNC1L, pVNC1LWriteData, nWriteLen);

}

//Close file - SPI mode.
void VNC1L_CloseFile_SPI( char * Filename )
{
// need to transmit 0A 20 filename 0D
   BYTE *pVNC1LWriteData;
   int i=0;
   int nWriteLen;


    char    cBuffer[20];
    sprintf( cBuffer, Filename  );     // 
      g_cVNC1LWriteBuffer[0] = 0x0A;     
      g_cVNC1LWriteBuffer[1] = 0x20;   
      while( cBuffer[i] != 0x00)
      {
         g_cVNC1LWriteBuffer[i+2] = cBuffer[i];
         i++;
      }  

      g_cVNC1LWriteBuffer[i+2] = 0x0D; 
      nWriteLen = i+3;                 // no. of bytes in message.
      pVNC1LWriteData = g_cVNC1LWriteBuffer;
      
//   TransmitVNC1L(g_nFdVNC1L, pVNC1LWriteData, nWriteLen);
   TransmitVNC1LSPIMode( pVNC1LWriteData, nWriteLen);          

}

// Open file for read - SPI mode.
void VNC1L_OpenFileForRead_SPI( char * Filename )
{
// need to transmit 0E 20 filename 0D
   BYTE *pVNC1LWriteData;
   int i=0;
   int nWriteLen;


    char    cBuffer[20];
    sprintf( cBuffer, Filename  );     // 
//    strcat( cBuffer, ".txt");        // assume the ending is in Filename.

      g_cVNC1LWriteBuffer[0] = 0x0E;     
      g_cVNC1LWriteBuffer[1] = 0x20;   
      while( cBuffer[i] != 0x00)
      {
         g_cVNC1LWriteBuffer[i+2] = cBuffer[i];
         i++;
      }  

      g_cVNC1LWriteBuffer[i+2] = 0x0D; 
      nWriteLen = i+3;                 // no. of bytes in message.
      pVNC1LWriteData = g_cVNC1LWriteBuffer;
      
//   TransmitVNC1L(g_nFdVNC1L, pVNC1LWriteData, nWriteLen);
   TransmitVNC1LSPIMode( pVNC1LWriteData, nWriteLen);          

}

// Read data from file  - SPI mode. (file already open)
void VNC1L_ReadDataFromFile_SPI( int nLen )
{
      SPI_Select_USB ( );  // set SPI chip selects for SPI.  //nbb--todo--put this back
// need to transmit 0B 20 dWordLen 0D 
   BYTE *pVNC1LWriteData;
//   int i=0;
   int nWriteLen;
   DWORD nReadBytes;
//   char    cBuffer[20];
    
   nReadBytes = nLen; 
   
   g_cVNC1LWriteBuffer[0] = 0x0B;     
   g_cVNC1LWriteBuffer[1] = 0x20;
   g_cVNC1LWriteBuffer[2] = (BYTE)(nReadBytes>>24); 
   g_cVNC1LWriteBuffer[3] = (BYTE)(nReadBytes>>16); 
   g_cVNC1LWriteBuffer[4] = (BYTE)(nReadBytes>>8); 
   g_cVNC1LWriteBuffer[5] = (BYTE)(nReadBytes); 
   
   g_cVNC1LWriteBuffer[6] = 0x0D; 
   


   nWriteLen = 7;                 // no. of bytes in message.
   pVNC1LWriteData = g_cVNC1LWriteBuffer;
   
      
//   TransmitVNC1L(g_nFdVNC1L, pVNC1LWriteData, nWriteLen);
   TransmitVNC1LSPIMode( pVNC1LWriteData, nWriteLen);          

}


void WriteToUSBFlashDiskSPIMode( void )
{
BYTE vPrintBuffer[1024];
   int nBytesRead,i;
   int nSecs;
   
      MakePD4AnInput();    // PD4 must be set as i/p so as not to interfere with USB cs\.
   
      USB_5V_ENABLE(); 
      VNC1L_PROG_HIGH();
      VNC1L_RESET_LOW();
      OSTimeDly(TICKS_PER_SECOND/10);      // wait .1 second.
      VNC1L_RESET_HIGH();
      SPI_Select_USB ( );

//      nBytesRead = ReadDataFromVNC1LSPIMode(10);    // read for 10 seconds.
      OSTimeDly(TICKS_PER_SECOND*10);      // wait 10 second .
      
      iprintf("\nStarting polling...\n"); 
//to call isr

   g_nVRBufIndex = 0;
   g_nVNC1LErrorCode = 0;
   g_bVNC1LReadFinished = FALSE;
   g_bVNC1LReadInProgress = TRUE;
   nSecs = 10;
   g_nVNC1LTimer = 3000 * nSecs;    // time allowed. - nSecs at 3kHz irq.
           
//      nBytesRead = PollVNC1LUntilPrompt_SPI(20);      // poll until prompt or 20 sec's expire.
      while( !g_bVNC1LReadFinished )
         OSTimeDly(1);

      nBytesRead = g_nVRBufIndex;
      
      if(nBytesRead)
      {
         VPrintf( g_cVNC1LReadBuffer, nBytesRead );
      } 
          
// Go to short command mode
      SetVNC1LMode_SCS_SPI();     

      nBytesRead = ReadDataFromVNC1LSPIMode(4);    // read for 4 seconds.
      if(nBytesRead)
      {
         VPrintf( g_cVNC1LReadBuffer, nBytesRead );
      }     
// Read directory
      VNC1LReadDir_SPI();     //read directory.

      nBytesRead = ReadDataFromVNC1LSPIMode(10);    // read for 10 seconds.
      if(nBytesRead)
      {
         VPrintf( g_cVNC1LReadBuffer, nBytesRead );
      }    
//////       
// Open a file for writing.        
      VNC1L_OpenFileForWrite_SPI( "MMKSPI02.Txt" );     //open a file for writing.

      nBytesRead = ReadDataFromVNC1LSPIMode(5);    // read for 5 seconds.
      if(nBytesRead)
      {
         VPrintf( g_cVNC1LReadBuffer, nBytesRead );
      }  
        
      iprintf("\nMMKSPI02.txt opened"); 

// Write data to a file.
// Fill buffer with test data to write.   
   char cTemp = '1';
      for(i=0; i<1024; i++)
      {
         vPrintBuffer[i] = cTemp++;
         if(cTemp == '8')
            cTemp = '7';
         
      }
                  
      VNC1L_WriteDataToFile_SPI( vPrintBuffer,  1000 );
      nBytesRead = ReadDataFromVNC1LSPIMode(4);    // read for 4 seconds.
      if(nBytesRead)
      {
         VPrintf( g_cVNC1LReadBuffer, nBytesRead );
      } 
      iprintf("\nMMKSPI02.txt data written"); 
         
// Close the file.
         
      VNC1L_CloseFile_SPI( "MMKSPI02.Txt" );     //close file.
      nBytesRead = ReadDataFromVNC1LSPIMode(4);    // read for 4 seconds.
      if(nBytesRead)
      {
         VPrintf( g_cVNC1LReadBuffer, nBytesRead );
      }    
      iprintf("\nMMKSPI02.txt closed"); 
 
// Open a file for reading.
      VNC1L_OpenFileForRead_SPI( "Test110.Txt" );     //open a file for writing.

      nBytesRead = ReadDataFromVNC1LSPIMode(4);    // read for 4 seconds.
       if(nBytesRead)
      {
         VPrintf( g_cVNC1LReadBuffer, nBytesRead );
      } 
        
      iprintf("\n Test110.txt opened for read"); 
// read data from file
        
      VNC1L_ReadDataFromFile_SPI( 100 );     //read 100 bytes from currently open file.

      nBytesRead = ReadDataFromVNC1LSPIMode(10);    // read for 10 seconds.
      if(nBytesRead)
      {
         VPrintf( g_cVNC1LReadBuffer, nBytesRead );
      }    
    
// Close the file.        
      VNC1L_CloseFile_SPI( "Test110.Txt" );     //close file.

      nBytesRead = ReadDataFromVNC1LSPIMode(4);    // read for 4 seconds.
      if(nBytesRead)
      {
         VPrintf( g_cVNC1LReadBuffer, nBytesRead );
      }    
       
}



//3kHz isr
void VNC1LISRHandler( void )
{
int k;
WORD nTemp;

   if(g_nVNC1LTimer && g_bVNC1LReadInProgress && !g_bVNC1LReadFinished)
   {
      for(k=0; k<2; k++)      // try 2 reads each pass thru isr.
      {
         nTemp = VNC1LRead1Byte( VNC1L_SPI_CHIP_SELECT_REVB2, VNC1L_DATA_REGISTER );
         if( (nTemp & 0x0001) == 0) // is data new (b0 = 0)
         {
//            BYTE cTemp = BYTE(nTemp >> 1);
            g_cVNC1LReadBuffer[g_nVRBufIndex] = BYTE(nTemp >> 1);
            if( g_nVRBufIndex>1 && g_cVNC1LReadBuffer[g_nVRBufIndex-1] == 0x3E  && g_cVNC1LReadBuffer[g_nVRBufIndex] == 0x0D ) // 3E oD i.e prompt.
               g_bVNC1LReadFinished = TRUE;
               
            g_nVRBufIndex++;
            
         }
      }
      
      if( g_nVNC1LTimer )
      {
         g_nVNC1LTimer--;
         if(g_nVNC1LTimer == 0)
            g_nVNC1LErrorCode = 1;
         
      }

   }  
} 

void ResetVNC1LAndWaitForPrompt( void )
{
      MakePD4AnInput();    // PD4 must be set as i/p so as not to interfere with USB cs\.
   
      USB_5V_ENABLE(); 
      VNC1L_PROG_HIGH();
      VNC1L_RESET_LOW();
      OSTimeDly(2);      // wait 2 ticks
      VNC1L_RESET_HIGH();
      SPI_Select_USB ( );
//WORD  chipSelectUsed = VNC1L_SPI_CHIP_SELECT_REVB2;

 //set up the command Q.  
   g_sVNC1L.m_cCommandQ[0] = VNC1L_PTP;   // poll till prompt.
   
//   g_sVNC1L.m_cCommandQ[1] = VNC1L_SCS;   // Short Command Set.
   
//   g_sVNC1L.m_cCommandQ[2] = VNC1L_PTP;   // poll till prompt.
      
   g_sVNC1L.m_cCommandQ[1] = VNC1L_TERMINATOR;   // finished.
   
   g_sVNC1L.m_nCommandQIndex = 0;    

   g_sVNC1L.m_nBytesToWrite = 0;      // no of bytes to be written.
   g_sVNC1L.m_nErrorCode = 0;             // clear error flag at start.
   g_sVNC1L.m_bWriteInProgress = FALSE;   //

// trigger the start.   
   g_sVNC1L.m_bStart = TRUE;
   g_sVNC1L.m_bTransactionInProgress = TRUE;    // this indicates to the isr that it has work to do. 
 
}   
// setting up to write a file under interrupt control.

void StartVNC1LFileWrite( int nWriteLen, BYTE * pDataToWrite, char * FileName)
{
int i;
int n=0;
    char    cBuffer[20];
    
// copy filename into struct.    
    sprintf( cBuffer, FileName  );     // 
    for(i=0; i<12; i++)
      g_sVNC1L.m_cFileName[i] = 0x00;
   i=0;      
   while( cBuffer[i] != 0x00 && i<12)
   {
      g_sVNC1L.m_cFileName[i] = cBuffer[i];     
      i++;
   }  

   g_sVNC1L.m_nBytesToWrite = nWriteLen;      // no of bytes to be written.
   g_sVNC1L.m_pWriteData = pDataToWrite;  // pointer to data that will be written.
   g_sVNC1L.m_nErrorCode = 0;             // clear error flag at start.
   g_sVNC1L.m_bWriteInProgress = FALSE;   //
 //set up the command Q.  
//   g_sVNC1L.m_cCommandQ[0] = VNC1L_PTP;   // poll till prompt.
   n=0;  
   g_sVNC1L.m_cCommandQ[n] = VNC1L_SCS;   // Short Command Set.
   
   g_sVNC1L.m_cCommandQ[n+1] = VNC1L_PTP;   // poll till prompt.
   
   g_sVNC1L.m_cCommandQ[n+2] = VNC1L_OPW;   // open file for write.
   
   g_sVNC1L.m_cCommandQ[n+3] = VNC1L_PTP;   // poll till prompt.
   
   g_sVNC1L.m_cCommandQ[n+4] = VNC1L_WRF;   // write data to file
   g_sVNC1L.m_cCommandQ[n+5] = VNC1L_PTP;   // poll till prompt.
   g_sVNC1L.m_cCommandQ[n+6] = VNC1L_CLF;   // close file
   g_sVNC1L.m_cCommandQ[n+7] = VNC1L_PTP;   // poll till prompt.
   
   g_sVNC1L.m_cCommandQ[n+8] = VNC1L_TERMINATOR;   // finished.
   
   g_sVNC1L.m_nCommandQIndex = 0;    
   
// trigger the start.   
   g_sVNC1L.m_bStart = TRUE;
   g_sVNC1L.m_bTransactionInProgress = TRUE;    // this indicates to the isr that it has work to do. 

}

void StartVNC1LFileWrite1( int nWriteLen, BYTE * pDataToWrite, char * FileName)
{
int i;
int n=0;
    char    cBuffer[20];
    
// copy filename into struct.    
    sprintf( cBuffer, FileName  );     // 
    for(i=0; i<12; i++)
      g_sVNC1L.m_cFileName[i] = 0x00;
   i=0;      
   while( cBuffer[i] != 0x00 && i<12)
   {
      g_sVNC1L.m_cFileName[i] = cBuffer[i];     
      i++;
   }  

   g_sVNC1L.m_nBytesToWrite = nWriteLen;      // no of bytes to be written.
   g_sVNC1L.m_pWriteData = pDataToWrite;  // pointer to data that will be written.
   g_sVNC1L.m_nErrorCode = 0;             // clear error flag at start.
   g_sVNC1L.m_bWriteInProgress = FALSE;   //
 //set up the command Q.  
//   g_sVNC1L.m_cCommandQ[0] = VNC1L_PTP;   // poll till prompt.
   n=0;  
   g_sVNC1L.m_cCommandQ[n] = VNC1L_SCS;   // Short Command Set.
   
   g_sVNC1L.m_cCommandQ[n+1] = VNC1L_PTP;   // poll till prompt.
   
   g_sVNC1L.m_cCommandQ[n+2] = VNC1L_OPW;   // open file for write.
   
   g_sVNC1L.m_cCommandQ[n+3] = VNC1L_PTP;   // poll till prompt.
   
   g_sVNC1L.m_cCommandQ[n+4] = VNC1L_WRF;   // write data to file
   g_sVNC1L.m_cCommandQ[n+5] = VNC1L_PTP;   // poll till prompt.
   g_sVNC1L.m_cCommandQ[n+6] = VNC1L_CLF;   // close file
   g_sVNC1L.m_cCommandQ[n+7] = VNC1L_PTP;   // poll till prompt.
   
   g_sVNC1L.m_cCommandQ[n+8] = VNC1L_TERMINATOR;   // finished.
   
   g_sVNC1L.m_nCommandQIndex = 0;    
   
// trigger the start.   
   g_sVNC1L.m_bStart = TRUE;
   g_sVNC1L.m_bTransactionInProgress = TRUE;    // this indicates to the isr that it has work to do. 
}

// setting up to write a file under interrupt control.
void StartVNC1LDirRead(  BYTE * pDirData )
{
//int i;
//    char    cBuffer[20];
    
   g_sVNC1L.m_nBytesToWrite = 0;      // no of bytes to be written.
   g_sVNC1L.m_pReadData = pDirData;  // pointer to data that will be read
   g_sVNC1L.m_nErrorCode = 0;             // clear error flag at start.
   g_sVNC1L.m_bWriteInProgress = FALSE;   //
 //set up the command Q.    
   g_sVNC1L.m_cCommandQ[0] = VNC1L_SCS;   // Short Command Set.
   
   g_sVNC1L.m_cCommandQ[1] = VNC1L_PTP;   // poll till prompt.
   
   g_sVNC1L.m_cCommandQ[2] = VNC1L_DIR;   // dir
   
   g_sVNC1L.m_cCommandQ[3] = VNC1L_DIR_REPLY;   // reply to dir command.
   
//   g_sVNC1L.m_cCommandQ[4] = VNC1L_PTP;   // poll till prompt.
   
   g_sVNC1L.m_cCommandQ[4] = VNC1L_TERMINATOR;   // finished.
   
   g_sVNC1L.m_nCommandQIndex = 0;    
   
// trigger the start.   
   g_sVNC1L.m_bStart = TRUE;
   g_sVNC1L.m_bTransactionInProgress = TRUE;    // this indicates to the isr that it has work to do. 
}
// Send data to USB printer
void PrintDataUsingVNC1L( int nWriteLen, BYTE * pDataToWrite, bool bPageFeed )
{
int n=0;

   g_sVNC1L.m_nBytesToWrite = nWriteLen;      // no of bytes to be written.
   g_sVNC1L.m_pWriteData = pDataToWrite;  // pointer to data that will be written.
   g_sVNC1L.m_nErrorCode = 0;             // clear error flag at start.
   g_sVNC1L.m_bWriteInProgress = FALSE;   //
   g_sVNC1L.m_bPageFeed = bPageFeed;
 //set up the command Q.  
//   g_sVNC1L.m_cCommandQ[0] = VNC1L_PTP;   // poll till prompt.
   n=0;  
   g_sVNC1L.m_cCommandQ[n] = VNC1L_IPA;   // Ensure Ascii mode
   
   g_sVNC1L.m_cCommandQ[n+1] = VNC1L_PTP;   // poll till prompt.
   
   g_sVNC1L.m_cCommandQ[n+2] = VNC1L_SC0;   // set current device to 1.
   
   g_sVNC1L.m_cCommandQ[n+3] = VNC1L_PTP;   // poll till prompt.
   
   g_sVNC1L.m_cCommandQ[n+4] = VNC1L_DSD;   // Device Send Data command.
   
   g_sVNC1L.m_cCommandQ[n+5] = VNC1L_PTP;   // poll till prompt.
   
   g_sVNC1L.m_cCommandQ[n+6] = VNC1L_TERMINATOR;   // finished.
   
   g_sVNC1L.m_nCommandQIndex = 0;    
   
// trigger the start.   
   g_sVNC1L.m_bStart = TRUE;
   g_sVNC1L.m_bTransactionInProgress = TRUE;    // this indicates to the isr that it has work to do. 
}



void AssembleVNC1LCommand( BYTE nCommand )
{
char    cBuffer[20];
unsigned int i;
unsigned int nLen;
int nLastByte;
int nBytes;
DWORD nWriteBytes;

	g_sVNC1L.m_bCommandInProgress = FALSE;    //havent started processing message yet.
	switch( nCommand )
	{
//Poll until prompt.	
	case VNC1L_PTP:
	  g_sVNC1L.m_bReading = TRUE;    //this is a write operation.
	       
      g_sVNC1L.m_nRxBytes = 0;                        //no. of bytes received.
      g_sVNC1L.m_nRxIndex = 0;                        // point to first byte to txmit.
      g_sVNC1L.m_nBytesToRead = 0;     // no read data 
//      g_sVNC1L.m_nBytesToRead = VNC1L_BUFFER_SIZE-10;     // make large 
      
      g_sVNC1L.m_nTimeoutCounter = 10*VNC1L_PIT_FREQUENCY; // allow 10 seconds max to complete.
      
		break;
// Set to Short Command Set.		
	case VNC1L_SCS:
	  g_sVNC1L.m_bReading = FALSE;    //this is a write operation.
	  
//   BYTE CmdSet[4] = {0x53,0x43,0x53,0x0D};
      g_sVNC1L.m_cTxBuffer[0] = 0x53;	                 // 
      g_sVNC1L.m_cTxBuffer[1] = 0x43;	                 // 
      g_sVNC1L.m_cTxBuffer[2] = 0x53;	                 // 
      g_sVNC1L.m_cTxBuffer[3] = 0x0d;	                 // 
      g_sVNC1L.m_nTxBytes = 4;                        //no. of bytes to transmit.
      g_sVNC1L.m_nTxIndex = 0;                        // point to first byte to txmit.
      
      g_sVNC1L.m_nRxBytes = 0;     
      g_sVNC1L.m_nRxIndex = 0;
      
      g_sVNC1L.m_nTimeoutCounter = 4*VNC1L_PIT_FREQUENCY; // allow 4 seconds max to complete.
		break;
/*
   BYTE CmdSet[4] = {0x53,0x43,0x53,0x0D};
   BYTE *pVNC1LWriteData = g_cVNC1LWriteBuffer;
   int nWriteLen;
   int i;
   
   nWriteLen = 4;
   for(i=0; i<4; i++)
   {
      g_cVNC1LWriteBuffer[i] = CmdSet[i];     
   }
   
   TransmitVNC1L(g_nFdVNC1L, pVNC1LWriteData, nWriteLen);

*/		
// Open file for write.
	case VNC1L_OPW:
	  g_sVNC1L.m_bReading = FALSE;    //this is a write operation.
/*

    char    cBuffer[20];
    sprintf( cBuffer, Filename  );     // 

      g_cVNC1LWriteBuffer[0] = 0x09;     
      g_cVNC1LWriteBuffer[1] = 0x20;   
      while( cBuffer[i] != 0x00)
      {
         g_cVNC1LWriteBuffer[i+2] = cBuffer[i];
         i++;
      }  

      g_cVNC1LWriteBuffer[i+2] = 0x0D; 
      nWriteLen = i+3;                 // no. of bytes in message.
      pVNC1LWriteData = g_cVNC1LWriteBuffer;
 
*/
      g_sVNC1L.m_cTxBuffer[0] = 0x09;	                 // 
      g_sVNC1L.m_cTxBuffer[1] = 0x20;	                 // 
      sprintf( cBuffer, g_sVNC1L.m_cFileName  );     // 
      i=0;
      while( cBuffer[i] != 0x00)
      {
         g_sVNC1L.m_cTxBuffer[i+2] = cBuffer[i];    //copy file name into Tx buffer.
         i++;
      }  
      g_sVNC1L.m_cTxBuffer[i+2] = 0x0d;	          //       
      
      g_sVNC1L.m_nTxBytes = i+3;                    //no. of bytes to transmit.      
      g_sVNC1L.m_nTxIndex = 0;                      // point to first byte to txmit.
      
      g_sVNC1L.m_nRxBytes = 0;     
      g_sVNC1L.m_nRxIndex = 0;
	  
      
      g_sVNC1L.m_nTimeoutCounter = 5*VNC1L_PIT_FREQUENCY; // allow 5 seconds max to complete.
		break;
//Write data to file.		
	case VNC1L_WRF:
// need to transmit 08 20 dWordLen 0D data
	   g_sVNC1L.m_bReading = FALSE;    //this is a write operation.
	  
      g_sVNC1L.m_cTxBuffer[0] = 0x08;	                 // 
      g_sVNC1L.m_cTxBuffer[1] = 0x20;	                 // 
      
      nWriteBytes = g_sVNC1L.m_nBytesToWrite;
       g_sVNC1L.m_cTxBuffer[2] = (BYTE)(nWriteBytes>>24); 
       g_sVNC1L.m_cTxBuffer[3] = (BYTE)(nWriteBytes>>16); 
       g_sVNC1L.m_cTxBuffer[4] = (BYTE)(nWriteBytes>>8); 
       g_sVNC1L.m_cTxBuffer[5] = (BYTE)(nWriteBytes); 
      
       g_sVNC1L.m_cTxBuffer[6] = 0x0D; 
// now for data to be written to file
      for(i=0; i<nWriteBytes && i<VNC1L_BUFFER_SIZE-8; i++)      // dont overwrite buffer size
      {
         g_sVNC1L.m_cTxBuffer[7+i] = *(g_sVNC1L.m_pWriteData+i);  //copy file data into Tx buffer
      }         
      
      g_sVNC1L.m_nTxBytes = nWriteBytes +7;         //no. of bytes to transmit.      
      g_sVNC1L.m_nTxIndex = 0;                      // point to first byte to txmit.
	  
      g_sVNC1L.m_nRxBytes = 0;     
      g_sVNC1L.m_nRxIndex = 0;
      
      g_sVNC1L.m_nTimeoutCounter = 10*VNC1L_PIT_FREQUENCY; // allow 10 seconds max to complete.
	  
/*
// need to transmit 08 20 dWordLen 0D data
   BYTE *pVNC1LWriteData;
   int i=0;
   int nWriteLen;
   DWORD nWriteBytes;
//   char    cBuffer[20];
    
   nWriteBytes = nLen; 
   
   g_cVNC1LWriteBuffer[0] = 0x08;     
   g_cVNC1LWriteBuffer[1] = 0x20;
   g_cVNC1LWriteBuffer[2] = (BYTE)(nWriteBytes>>24); 
   g_cVNC1LWriteBuffer[3] = (BYTE)(nWriteBytes>>16); 
   g_cVNC1LWriteBuffer[4] = (BYTE)(nWriteBytes>>8); 
   g_cVNC1LWriteBuffer[5] = (BYTE)(nWriteBytes); 
   
   g_cVNC1LWriteBuffer[6] = 0x0D; 
   

   for(i=0; i<nLen; i++)
   {
      g_cVNC1LWriteBuffer[7+i] =*(pData+i);
   }         

   nWriteLen = nLen+7;                 // no. of bytes in message.
   pVNC1LWriteData = g_cVNC1LWriteBuffer;
   
      
   TransmitVNC1L(g_nFdVNC1L, pVNC1LWriteData, nWriteLen);
*/	
//		ToDoHerE();			// 
		break;
//Close file		
	case VNC1L_CLF:
	  g_sVNC1L.m_bReading = FALSE;    //this is a write operation.
      g_sVNC1L.m_cTxBuffer[0] = 0x0A;	                 // 
      g_sVNC1L.m_cTxBuffer[1] = 0x20;	                 // 
      sprintf( cBuffer, g_sVNC1L.m_cFileName  );     // 
      i=0;
      while( cBuffer[i] != 0x00)
      {
         g_sVNC1L.m_cTxBuffer[i+2] = cBuffer[i];    //copy file name into Tx buffer.
         i++;
      }  
      g_sVNC1L.m_cTxBuffer[i+2] = 0x0d;	          //       
      
      g_sVNC1L.m_nTxBytes = i+3;                    //no. of bytes to transmit.      
      g_sVNC1L.m_nTxIndex = 0;                      // point to first byte to txmit.
      
      g_sVNC1L.m_nRxBytes = 0;     
      g_sVNC1L.m_nRxIndex = 0;
	  
      
      g_sVNC1L.m_nTimeoutCounter = 5*VNC1L_PIT_FREQUENCY; // allow 5 seconds max to complete.
		break;
	
/*

    sprintf( cBuffer, Filename  );     // 
      g_cVNC1LWriteBuffer[0] = 0x0A;     
      g_cVNC1LWriteBuffer[1] = 0x20;   
      while( cBuffer[i] != 0x00)
      {
         g_cVNC1LWriteBuffer[i+2] = cBuffer[i];
         i++;
      }  

      g_cVNC1LWriteBuffer[i+2] = 0x0D; 
      nWriteLen = i+3;                 // no. of bytes in message.
      pVNC1LWriteData = g_cVNC1LWriteBuffer;

*/	
//Read directory command.		
	case VNC1L_DIR:
	  g_sVNC1L.m_bReading = FALSE;    //this is a write operation.
      g_sVNC1L.m_cTxBuffer[0] = 0x01;	                 // 
      g_sVNC1L.m_cTxBuffer[1] = 0x0d;	                 // 
      
      g_sVNC1L.m_nTxBytes = 2;                    //no. of bytes to transmit.      
      g_sVNC1L.m_nTxIndex = 0;                      // point to first byte to txmit.
      
      g_sVNC1L.m_nRxBytes = 0;                  // bytes received.   
      g_sVNC1L.m_nBytesToRead = 0;     // make large 
      g_sVNC1L.m_nRxIndex = 0;
	  
      
      g_sVNC1L.m_nTimeoutCounter = 2*VNC1L_PIT_FREQUENCY; // allow 2 seconds max to complete.

		break;
//Read directory reply, i.e. reading data.		
	case VNC1L_DIR_REPLY:
	  g_sVNC1L.m_bReading = TRUE;    //this is a read operation.
      g_sVNC1L.m_cTxBuffer[0] = 0x01;	                 // 
      g_sVNC1L.m_cTxBuffer[1] = 0x0d;	                 // 
      
      g_sVNC1L.m_nTxBytes = 0;                    //no. of bytes to transmit.      
      g_sVNC1L.m_nTxIndex = 0;                      // point to first byte to txmit.
      
      g_sVNC1L.m_nRxBytes = 0;                  // bytes received.   
      g_sVNC1L.m_nBytesToRead = VNC1L_BUFFER_SIZE-10;     // not sure of reply length - allow up to maximum     
      g_sVNC1L.m_nRxIndex = 0;
      	  
      
      g_sVNC1L.m_nTimeoutCounter = 5*VNC1L_PIT_FREQUENCY; // allow 5 seconds max to complete.

		break;
		
// Set to Ascii mode		
	case VNC1L_IPA:
	  g_sVNC1L.m_bReading = FALSE;    //this is a write operation.
	  
//   BYTE CmdSet[4] = {ipa,0x0D};
      g_sVNC1L.m_cTxBuffer[0] = 'i';	                 // 
      g_sVNC1L.m_cTxBuffer[1] = 'p';	                 // 
      g_sVNC1L.m_cTxBuffer[2] = 'a';	                 // 
      g_sVNC1L.m_cTxBuffer[3] = 0x0d;	                 // 
      g_sVNC1L.m_nTxBytes = 4;                        //no. of bytes to transmit.
      g_sVNC1L.m_nTxIndex = 0;                        // point to first byte to txmit.
      
      g_sVNC1L.m_nRxBytes = 0;     
      g_sVNC1L.m_nRxIndex = 0;
      
      g_sVNC1L.m_nTimeoutCounter = 2*VNC1L_PIT_FREQUENCY; // allow 2 seconds max to complete.
		break;
		
// Set current device to 1	
	case VNC1L_SC0:
	  g_sVNC1L.m_bReading = FALSE;    //this is a write operation.
	  
//   BYTE CmdSet[4] = {sc 1,0x0D};
      g_sVNC1L.m_cTxBuffer[0] = 's';	                 // 
      g_sVNC1L.m_cTxBuffer[1] = 'c';	                 // 
      g_sVNC1L.m_cTxBuffer[2] = ' ';	                 // note the space char.
      g_sVNC1L.m_cTxBuffer[3] = '0';	                 // 
      g_sVNC1L.m_cTxBuffer[4] = 0x0d;	                 // 
      g_sVNC1L.m_nTxBytes = 5;                        //no. of bytes to transmit.
      g_sVNC1L.m_nTxIndex = 0;                        // point to first byte to txmit.
      
      g_sVNC1L.m_nRxBytes = 0;     
      g_sVNC1L.m_nRxIndex = 0;
      
      g_sVNC1L.m_nTimeoutCounter = 2*VNC1L_PIT_FREQUENCY; // allow 2 seconds max to complete.
		break;
// Set current device to 1	
	case VNC1L_DSD:
	  g_sVNC1L.m_bReading = FALSE;    //this is a write operation.
	  
//   BYTE CmdSet[4] = {sc 1,0x0D};
      if(g_sVNC1L.m_bPageFeed)
         nBytes = g_sVNC1L.m_nBytesToWrite+2; //add on 0A 0C
      else
         nBytes = g_sVNC1L.m_nBytesToWrite;
      sprintf(cBuffer,"dsd %d", nBytes);
      
      nLen = strlen(cBuffer);    //nLen = length of "dsd xxxx" where xxx is the no. of data bytes.
      for(i=0; i<nLen; i++)
         g_sVNC1L.m_cTxBuffer[i] = cBuffer[i];
         
      g_sVNC1L.m_cTxBuffer[nLen] = 0x0d;
         
      for(i=0; i<g_sVNC1L.m_nBytesToWrite; i++)
         g_sVNC1L.m_cTxBuffer[nLen+1+i] = *(g_sVNC1L.m_pWriteData + i);
         
      if(g_sVNC1L.m_bPageFeed)            // should we append a page feed??
      {
         g_sVNC1L.m_cTxBuffer[nLen+1+g_sVNC1L.m_nBytesToWrite] = 0x0a;    //line feed character.
         g_sVNC1L.m_cTxBuffer[nLen+2+g_sVNC1L.m_nBytesToWrite] = 0x0c;    //form feed character.
         nLastByte = nLen+g_sVNC1L.m_nBytesToWrite+3;
      }
      else
         nLastByte = nLen+g_sVNC1L.m_nBytesToWrite+1;

      g_sVNC1L.m_cTxBuffer[nLastByte] = 0x0d;

//      g_sVNC1L.m_cTxBuffer[0] = 'd';	                 // 
//      g_sVNC1L.m_cTxBuffer[1] = 's';	                 // 
//      g_sVNC1L.m_cTxBuffer[2] = 'd';	                 // 
//      g_sVNC1L.m_cTxBuffer[3] = ' ';	                 // note the space char.
//      g_sVNC1L.m_cTxBuffer[4] = 0x0d;	                 // 
      g_sVNC1L.m_nTxBytes = nLastByte+1;                        //no. of bytes to transmit.
      g_sVNC1L.m_nTxIndex = 0;                        // point to first byte to txmit.
      
      g_sVNC1L.m_nRxBytes = 0;     
      g_sVNC1L.m_nRxIndex = 0;
      
      g_sVNC1L.m_nTimeoutCounter = 10*VNC1L_PIT_FREQUENCY; // allow 10 seconds max to complete.
		break;
		
	case VNC1L_TERMINATOR:
//		ToDoHerE();			// 
		break;
		
	default:
		break;

	}  // end of switch statement.
	if(g_sVNC1L.m_bReading)      // if it is a read command clear prompt rx'ed flag.
	{
	  g_sVNC1L.m_nBytesRead = 0;
	  g_sVNC1L.m_bPromptReceived = FALSE;
   }
	  
   g_sVNC1L.m_nRxIndex = 0;	  
//testonly	
//   iprintf("\n\n\nAssembleCommand: ");
//   PrintVNC1LDiagnostics();
   
   for(i=0; i<20; i++)
   {
      g_sVNC1L.m_cRxBuffer[i] = 0;
   }   
} 
//3kHz isr
void VNC1LPITISRHandler( void )
{
int i;
WORD nTemp;
BYTE nCommand;
bool bVNC1LCommandFinished = FALSE;
BYTE DataByte;
 if( g_sVNC1L.m_bTransactionInProgress )
 {
    g_nVNC1LPITCounter++;        // count passes thru active part of program.
   if(g_sVNC1L.m_bStart)
   {
      g_sVNC1L.m_bBusy = TRUE;            // set busy to prevent other attempts at use of VNC1L.
      g_sVNC1L.m_bStart = FALSE;
      g_sVNC1L.m_nCommandQIndex = 0;      // point to first command.
      
      nCommand = g_sVNC1L.m_cCommandQ[g_sVNC1L.m_nCommandQIndex]; //pick up command.
      if( nCommand == VNC1L_TERMINATOR)  //empty Q ???
      {
         bVNC1LCommandFinished = TRUE;
      }
      else  // format up first message/command.
      { 
         AssembleVNC1LCommand( nCommand );
         g_sVNC1L.m_bCommandInProgress = TRUE; //next pass will be processing a command.        
      }
   }
   else if(g_sVNC1L.m_bBusy && g_sVNC1L.m_bCommandInProgress )
   {

      if( g_sVNC1L.m_bReading )  //read command.
      {
         g_nVReadCount++;
         nTemp = VNC1LRead1Byte( VNC1L_SPI_CHIP_SELECT_REVB2, VNC1L_DATA_REGISTER );
         if( (nTemp & 0x0001) == 0) // is data new (b0 = 0)
         {
//            BYTE cTemp = BYTE(nTemp >> 1);
            g_sVNC1L.m_cRxBuffer[g_sVNC1L.m_nRxIndex] = BYTE(nTemp >> 1);
            g_sVNC1L.m_nRxBytes++;     // inc no. of bytes received.
            //check if expected data length received.  
            if(g_sVNC1L.m_nBytesToRead && (g_sVNC1L.m_nRxBytes == g_sVNC1L.m_nBytesToRead) )
            {
               bVNC1LCommandFinished = TRUE; 
               g_sVNC1L.m_nBytesRead  = g_sVNC1L.m_nRxBytes;      // save no. of bytes read
//               iprintf("ooops");          
            }
                  //check if prompt received.            
            else if( g_sVNC1L.m_nRxIndex>0 && g_sVNC1L.m_cRxBuffer[g_sVNC1L.m_nRxIndex-1] == 0x3E  && g_sVNC1L.m_cRxBuffer[g_sVNC1L.m_nRxIndex] == 0x0D ) // 3E oD i.e prompt.
            {
               g_sVNC1L.m_nBytesRead  = g_sVNC1L.m_nRxBytes;      // save no. of bytes read            
               bVNC1LCommandFinished = TRUE;
               g_sVNC1L.m_bPromptReceived = TRUE;
            }
               
            g_sVNC1L.m_nRxIndex++;          
         }      
      }
      else  // must be a write.
      {
         g_nVWriteCount++;
//         do{
/*
//           CheckForVNC1LData( );
         WORD nTemp = VNC1LRead1Byte( VNC1L_SPI_CHIP_SELECT_REVB2, VNC1L_DATA_REGISTER );
         if( (nTemp & 0x0001) == 0) // is data new (b0 = 0)
         {
            g_sVNC1L.m_cRxBuffer[g_sVNC1L.m_nRxIndex] = BYTE(nTemp >> 1);
            g_sVNC1L.m_nRxBytes++;     // inc no. of bytes received.
            
            g_sVNC1L.m_nRxIndex++;
         }
*/         
// now write data out.         
           DataByte = g_sVNC1L.m_cTxBuffer[g_sVNC1L.m_nTxIndex];
           nTemp = VNC1LWrite1Byte(VNC1L_SPI_CHIP_SELECT_REVB2,  DataByte, VNC1L_DATA_REGISTER ); 
//           iprintf(",%2x", DataByte);
           if((nTemp & 0x0001) == 0)   // l.s. bit = 0 => write accepted.
           {
            g_sVNC1L.m_nTxIndex++;
            
            if(g_sVNC1L.m_nTxIndex == g_sVNC1L.m_nTxBytes)  // have we reached the last byte.
            {
               bVNC1LCommandFinished = TRUE;
/*               
               // testonly...see what was received.
               if(g_sVNC1L.m_nRxBytes)
               {
                  iprintf("\nRx_W ");
                  for(i=0; i<g_sVNC1L.m_nRxBytes; i++)
                  {
                     iprintf("%2x", g_sVNC1L.m_cRxBuffer[i]);
                  }

               }
*/               
            }
           }
 //          i++;
//         } while( (nTemp & 0x0001) && i<3000 );
     
      }  // end of write brackets.
   
   } // end of processing command brackets.
      
// checking for timeout...   
   if( g_sVNC1L.m_nTimeoutCounter )
   {
      g_sVNC1L.m_nTimeoutCounter--;
      if(g_sVNC1L.m_nTimeoutCounter == 0)
      {
         g_sVNC1L.m_nErrorCode = VNC1L_TIMEOUT_ERROR;
         bVNC1LCommandFinished = TRUE;
         g_sVNC1L.m_bTransactionInProgress = FALSE;     // release the VNC1L for next job.
      }
   }
   
   
   if( bVNC1LCommandFinished  && !g_sVNC1L.m_nErrorCode )   // finished processing current command
   {
      g_sVNC1L.m_nCommandQIndex++;      // advance to next command.     
      nCommand = g_sVNC1L.m_cCommandQ[g_sVNC1L.m_nCommandQIndex]; //pick up command.
//read data received.      
      if( g_sVNC1L.m_bReading && g_sVNC1L.m_nRxBytes && g_sVNC1L.m_nBytesToRead )   
      {
         for(i=0; i<g_sVNC1L.m_nRxBytes; i++)
         {
           *(g_sVNC1L.m_pReadData+i) = g_sVNC1L.m_cRxBuffer[i];      //store received data.
         }
      }
      
      if( nCommand == VNC1L_TERMINATOR)
      {
         //done, can exit.
         g_sVNC1L.m_bTransactionInProgress = FALSE;     // release the VNC1L for next job.
         g_sVNC1L.m_bCommandInProgress = FALSE;
      }
      else
      {
         AssembleVNC1LCommand( nCommand );   //format up command data.
         g_sVNC1L.m_bCommandInProgress = TRUE; //next pass will be processing a command.
      }   
   }
 }   
}    

//  
void InitialiseVNC1lDataStructure( void )
{
   g_nVReadCount = 0;
   g_nVWriteCount = 0;
   g_sVNC1L.m_bTransactionInProgress = FALSE;
   g_sVNC1L.m_bCommandInProgress = FALSE;
   g_sVNC1L.m_bBusy = FALSE;
   g_sVNC1L.m_bStart = FALSE;
   g_sVNC1L.m_nTimeoutCounter = 0;
   // Q of messages in transaction
   g_sVNC1L.m_cCommandQ[0] = VNC1L_TERMINATOR;
   g_sVNC1L.m_nCommandQIndex = 0;
   g_sVNC1L.m_nErrorCode = 0;
   g_sVNC1L.m_bPromptReceived = FALSE;
 
} 
//
void PrintVNC1LDiagnostics( void )
{
int i,nBytes;
   iprintf("\nVNC1L PITs: %ld Reads = %ld Writes = %ld  Error Code = %4x ", g_nVNC1LPITCounter, g_nVReadCount, g_nVWriteCount, g_sVNC1L.m_nErrorCode);
   
   iprintf("\nCommand Q: ");
   for(i=0; i<10; i++)
   {
      iprintf("%2x ", g_sVNC1L.m_cCommandQ[i]);
   }
   iprintf("\nQ Index: %d", g_sVNC1L.m_nCommandQIndex );
   
   iprintf("\nFileName: ");
   for(i=0; i<12; i++)
   {
      iprintf("%c", g_sVNC1L.m_cFileName[i]);
   }
   
      
   iprintf("\nRxBuff: ");
   if(g_sVNC1L.m_bPromptReceived)
      nBytes = g_sVNC1L.m_nRxBytes;
   else
      nBytes = 20;
      
   for(i=0; i<45; i++)
   {
//      iprintf("%c ", g_sVNC1L.m_cRxBuffer[i]);
      iprintf("%2x ", g_sVNC1L.m_cRxBuffer[i]);
      
   }
   
   iprintf("\nRxBuff: ");
   for(i=0; i<45; i++)
   {
      iprintf("%c", g_sVNC1L.m_cRxBuffer[i]);
      if(g_sVNC1L.m_cRxBuffer[i] == 0x0d)
         iprintf("\n");
   }
   
   iprintf("\nTxBuff: ");
   for(i=0; i<20; i++)
   {
      iprintf("%2x ", g_sVNC1L.m_cTxBuffer[i]);
   }
   iprintf("\nTxBuff: ");
   for(i=0; i<12; i++)
   {
      iprintf("%c ", g_sVNC1L.m_cTxBuffer[i]);
   }

}
