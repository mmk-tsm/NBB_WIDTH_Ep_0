/*-------------------------------------------------------------------
 File: MCP23Sexp.cpp
 Description: MCP23S17 & MCP23S08 SPI expander chip functions.
 Taken from EEPROMdriver.cpp

 P.Smith                                      22/6/07
 SPI_BAUD_EXPAN used to increase speed of spi read

 P.Smith                                      08/01/09
 modify access of expansion to specify address information
 this allows an expansion card at a different address to be
 initialised and accessed easily.

 P.Smith                                      13/02/09
 changed the speed of the expansion test.
 allow the relay output to function.

 P.Smith                                      13/02/09
 In InitialiseOnNBBMCP23S08 set up all inputs, b7 was originally set up as an o/p

-------------------------------------------------------------------*/
#include <startnet.h>

#include <basictypes.h>
#include <stdio.h>

#include "QSPIconstants.h"
#include "QSPIdriver.h"

#include "MCP23Sexp.h"


//#include "EEPROMconstants.h"
//#include "EEPROMdriver.h"
extern WORD g_nEX1SPIChipSelect;

// Function to read a register in the MCP23S08 chip on the NBB
BYTE MCP23S08Read1Byte(WORD chipSelectUsed, BYTE S08Address, BYTE RegisterNo)
{

    BYTE Result;


 // Create QSPI Configuration Structure to send read instruction
#define TXLENS08 3     // 3  bytes (
// #define TXLEN001 (1)
 WORD QSPI_BufferREADByte[4];
 QSPI_CFG ConfigREADByte;
 QSPI_CFG * ptrConfigREADByte = &ConfigREADByte;
 WORD nTemp;

 // QSPI Mode Register
 // A) Setup QMR register (8-bit transfer)
 // 148 should give 250kbaud.
 // 250 will be below 200kbaud.
 ConfigREADByte.QMR = ( QSPI_QMR_MSTR|QSPI_QMR_CPOL|QSPI_QMR_CPHA|QSPI_QMR_BITS_8|QSPI_QMR_DOHIE|QSPI_QMR_BAUD(SPI_BAUD_EXPAN ) );   //QSPI_QMR_BAUD(19

 // B) QSPI Delay Register
 ConfigREADByte.QDLYR = QSPI_QDLYR_SPE|QSPI_QDLYR_QCD(0)|QSPI_QDLYR_DTL(1);

 // QSPI Wrap Register
 // C) Setup QWR register to send one byte(8-bits), and set CS to active low
 ConfigREADByte.QWR = ( QSPI_QWR_ENDQP(TXLENS08-1)|QSPI_QWR_NEWQP(0)|QSPI_QWR_CSIV ); //

 // D) QSPI Comand RAM Registers[0]
 ConfigREADByte.QCR[0] = (QSPI_CRX_BITSE|QSPI_CRX_DSCK|QSPI_CRX_DT|QSPI_CRX_CONT|chipSelectUsed);  // QSPI_CRX_CONT| = 0

 ConfigREADByte.QCR[1] = (QSPI_CRX_BITSE|QSPI_CRX_DSCK|QSPI_CRX_DT|QSPI_CRX_CONT|chipSelectUsed);
 ConfigREADByte.QCR[2] = (QSPI_CRX_BITSE|QSPI_CRX_DSCK|QSPI_CRX_DT|chipSelectUsed);
// ConfigREADByte.QCR[3] = (QSPI_CRX_BITSE|QSPI_CRX_DSCK|QSPI_CRX_DT|chipSelectUsed);

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
//   if(RegisterType == VNC1L_DATA_REGISTER)
//      nTemp = 0xFC00;         //4 unused 1's at beg, start bit = 1, R/W = 1, ADD=0, 8 0's, one 0 corresponding to status bit.
//   else
//      nTemp = 0xFE00;
 nTemp = 0x41 + (S08Address << 1);     //ADDRESS BITS SHIFTED 1 BIT LEFT.

 QSPI_BufferREADByte[0] =  nTemp ; //
 QSPI_BufferREADByte[1] = RegisterNo; //(BYTE)(startAddress >> 8); // throw away bottom 8-bits
 QSPI_BufferREADByte[2] = 0; // used to clock out data
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

  QSPI_WriteQTR(QSPI_BufferREADByte, TXLENS08, ptrConfigREADByte);

// Copy ALL QRR to RXBuff
//    VNC1LDataByte = (BYTE)QSPI_READ_QRR(0);// Read Tansfer information - data byte will be in QRR(3).
    Result = QSPI_READ_QRR(2);
//  #define RxLen 16
//  QSPI_ReadQRR( ptrRxBuffer, RxLen );

    return ( Result );
}// close function read


BYTE ReadMCP23S08GPIO( void )
{
BYTE nResult;
   nResult = MCP23S08Read1Byte(g_nEX1SPIChipSelect, MCP23S08_NBB_ADDRESS, MCP23S08_GPIO);
   return ( nResult );
}

// Function to WRITE a register in the MCP23S08 chip on the NBB
void MCP23S08Write1Byte(WORD chipSelectUsed, BYTE S08Address, BYTE RegisterNo, BYTE WriteData )
{

 // Create QSPI Configuration Structure to send read instruction
// #define TXLENS08 3     // 3  bytes (
// #define TXLEN001 (1)
 WORD QSPI_BufferWriteByte[4];
 QSPI_CFG ConfigWriteByte;
 QSPI_CFG * ptrConfigWriteByte = &ConfigWriteByte;
 WORD nTemp;

 // QSPI Mode Register
 // A) Setup QMR register (8-bit transfer)
 // 148 should give 250kbaud.
 // 250 will be below 200kbaud.
 ConfigWriteByte.QMR = ( QSPI_QMR_MSTR|QSPI_QMR_CPOL|QSPI_QMR_CPHA|QSPI_QMR_BITS_8|QSPI_QMR_DOHIE|QSPI_QMR_BAUD(SPI_BAUD_EXPAN) );   //QSPI_QMR_BAUD(19

 // B) QSPI Delay Register
 ConfigWriteByte.QDLYR = QSPI_QDLYR_SPE|QSPI_QDLYR_QCD(0)|QSPI_QDLYR_DTL(1);

 // QSPI Wrap Register
 // C) Setup QWR register to send one byte(8-bits), and set CS to active low
 ConfigWriteByte.QWR = ( QSPI_QWR_ENDQP(TXLENS08-1)|QSPI_QWR_NEWQP(0)|QSPI_QWR_CSIV ); //

 // D) QSPI Comand RAM Registers[0]
 ConfigWriteByte.QCR[0] = (QSPI_CRX_BITSE|QSPI_CRX_DSCK|QSPI_CRX_DT|QSPI_CRX_CONT|chipSelectUsed);  // QSPI_CRX_CONT| = 0

 ConfigWriteByte.QCR[1] = (QSPI_CRX_BITSE|QSPI_CRX_DSCK|QSPI_CRX_DT|QSPI_CRX_CONT|chipSelectUsed);
 ConfigWriteByte.QCR[2] = (QSPI_CRX_BITSE|QSPI_CRX_DSCK|QSPI_CRX_DT|chipSelectUsed);
// ConfigWriteByte.QCR[3] = (QSPI_CRX_BITSE|QSPI_CRX_DSCK|QSPI_CRX_DT|chipSelectUsed);


//   if(RegisterType == VNC1L_DATA_REGISTER)
//      nTemp = 0xFC00;         //4 unused 1's at beg, start bit = 1, R/W = 1, ADD=0, 8 0's, one 0 corresponding to status bit.
//   else
//      nTemp = 0xFE00;
 nTemp = 0x40 + (S08Address << 1);     //ADDRESS BITS SHIFTED 1 BIT LEFT.
 nTemp &= 0xFE;                        // clear the r/W bit for write.

 QSPI_BufferWriteByte[0] =  nTemp ; //
 QSPI_BufferWriteByte[1] = RegisterNo; //(BYTE)(startAddress >> 8); // throw away bottom 8-bits
 QSPI_BufferWriteByte[2] = WriteData; //  data to write.
// QSPI_BufferWriteByte[3] = 0; // used to clock out data

  QSPI_WriteQTR(QSPI_BufferWriteByte, TXLENS08, ptrConfigWriteByte);

// Copy ALL QRR to RXBuff
//    VNC1LDataByte = (BYTE)QSPI_READ_QRR(0);// Read Tansfer information - data byte will be in QRR(3).
//    Result = QSPI_READ_QRR(2);
//  #define RxLen 16
//  QSPI_ReadQRR( ptrRxBuffer, RxLen );

    return;
}// close function write

void WriteOnNBBMCP23S08( BYTE RegisterNo, BYTE WriteData )
{
   MCP23S08Write1Byte(g_nEX1SPIChipSelect, MCP23S08_NBB_ADDRESS, RegisterNo, WriteData );  //SPI_EX1_CHIP_SELECT_REVB2
   return;
}
void InitialiseOnNBBMCP23S08( void )
{
   WriteOnNBBMCP23S08( MCP23S08_IOCON, 0x08 );     // set the HAEN bit.
   WriteOnNBBMCP23S08( MCP23S08_IOCON, 0x08 );     // set the HAEN bit.
//  WriteOnNBBMCP23S08( MCP23S08_IODIR, 0x7F );     // gp7 set to be o/p(b7=0), rest inputs.
//   WriteOnNBBMCP23S08( MCP23S08_GPIO, 0x00 );     // gp7 set TO 0.
   WriteOnNBBMCP23S08( MCP23S08_IODIR, 0xFF ); // all (gp0-gp7) inputs.

}
void SetOnNBBMCP23S08_GP7( void )
{
   WriteOnNBBMCP23S08( MCP23S08_GPIO, 0x80 );     // gp7 set TO 1.
}
void ResetOnNBBMCP23S08_GP7( void )
{
   WriteOnNBBMCP23S08( MCP23S08_GPIO, 0x00 );     // gp7 set TO 0.
}


BYTE ReadExpansionMCP23S08GPIO( void )
{
BYTE nResult;
   nResult = MCP23S08Read1Byte(g_nEX1SPIChipSelect, MCP23S08_EXP_ADDRESS_3, MCP23S08_GPIO);
   return ( nResult );
}
void InitialiseOffNBBMCP23S08( void )
{
   WriteOffNBBMCP23S08( MCP23S08_IOCON, 0x08 );     // set the HAEN bit.
   WriteOffNBBMCP23S08( MCP23S08_IOCON, 0x08 );     // set the HAEN bit.
   WriteOffNBBMCP23S08( MCP23S08_IODIR, 0x7F );     // gp7 set to be o/p(b7=0), rest inputs.
   WriteOffNBBMCP23S08( MCP23S08_GPIO, 0x00 );     // gp7 set TO 0.
}

void WriteOffNBBMCP23S08( BYTE RegisterNo, BYTE WriteData )
{
   MCP23S08Write1Byte(g_nEX1SPIChipSelect, MCP23S08_EXP_ADDRESS_3, RegisterNo, WriteData );
   return;
}

void SetOffNBBMCP23S08_GP7( void )
{
   WriteOffNBBMCP23S08( MCP23S08_GPIO, 0x80 );     // gp7 set TO 1.
}
void ResetOffNBBMCP23S08_GP7( void )
{
   WriteOffNBBMCP23S08( MCP23S08_GPIO, 0x00 );     // gp7 set TO 0.
}
// Initialise for the McP23S17 on th NBBEX pcb.

void WriteNBBEXMCP23S17( BYTE RegisterNo,BYTE S08Address,BYTE WriteData )
{
   MCP23S08Write1Byte(g_nEX1SPIChipSelect,S08Address, RegisterNo, WriteData );
   return;
}

void InitialiseNBBEXMCP23S17( BYTE S08Address )
{
   WriteNBBEXMCP23S17( MCP23S17_IOCON,S08Address,0x8C );     // set the bank, HAEN & ODR bits.
   WriteNBBEXMCP23S17( MCP23S17_IODIRA,S08Address,0x00 );     // set gpioA's to be ouptuts
   WriteNBBEXMCP23S17( MCP23S17_GPIOA,S08Address,0x00 );      // gpa0-gpa7 set TO 0.
}

void NBBEXMCP23S17AllOutputsOn( BYTE S08Address )
{
   WriteNBBEXMCP23S17( MCP23S17_GPIOA,S08Address,0xFF );     // gpa0-gpa7 set TO 1's.
}
void NBBEXMCP23S17AllOutputsOff(BYTE S08Address )
{
   WriteNBBEXMCP23S17( MCP23S17_GPIOA,S08Address,0x00 );     // gpa0-gpa7 set TO 0's.
}

void NBBEXMCP23S17ScrollOutputs( BYTE S08Address )
{
BYTE nMask = 0x01;
int i;

   for(i=0; i<8; i++)
   {
      //if(nMask != 0x08)    // do not toggle the mechanical relay (SRC20).
      WriteNBBEXMCP23S17( MCP23S17_GPIOA,S08Address, nMask );     // gpa0-gpa7 set TO 1's.
      OSTimeDly( TICKS_PER_SECOND/3 );
      WriteNBBEXMCP23S17( MCP23S17_GPIOA,S08Address, 0x00 );     // gpa0-gpa7 set TO 0's.
      nMask <<= 1;
   }
}



