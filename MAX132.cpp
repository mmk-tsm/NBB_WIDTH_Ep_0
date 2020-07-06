///////////////////////////////////////////////////////////
// MAX132.cpp
//
// A library of functions for accessing the MAX132 a/d
//
//
// M.McKiernan								05-12-2005
//
//////////////////////////////////////////////////////////


//*****************************************************************************
// INCLUDES
//*****************************************************************************
#include <startnet.h>

#include <basictypes.h>
#include <stdio.h> 

#include "QSPIconstants.h"
#include "QSPIdriver.h"

#include "MAX132.h"




/*
  * Read MAX132 a/d.
  *
*/
void  ReadMAX132(BYTE MuxBits, WORD chipSelectUsed, BYTE *RxData) 
{ 
          
 // Create QSPI Configuration Structure to read MAX132
// WORD nRxBuffer[3];
 WORD XfrLen = 4;
 
 WORD QSPI_BufferMAX132[4];
 WORD * ptrQSPI_BufferMAX132 = QSPI_BufferMAX132;
 QSPI_CFG ConfigMAX132;
 QSPI_CFG * ptrConfigMAX132 = &ConfigMAX132;
 
 // QSPI Mode Register
 // A) Setup QMR register (8-bit transfer) 
 //ConfigRDSR.QMR = ( QSPI_QMR_MSTR|QSPI_QMR_CPOL|QSPI_QMR_CPHA|QSPI_QMR_BITS_16|QSPI_QMR_DOHIE|QSPI_QMR_BAUD(19) );
 ConfigMAX132.QMR = ( QSPI_QMR_MSTR|QSPI_QMR_BITS_8|QSPI_QMR_DOHIE|QSPI_QMR_BAUD(40) ); //cpol=0, cpha=0
                                                                                       //baud = just under 2MHz (same as for eeprom)

 // B) QSPI Delay Register
 ConfigMAX132.QDLYR = QSPI_QDLYR_SPE|QSPI_QDLYR_QCD(0)|QSPI_QDLYR_DTL(3);
 
 // QSPI Wrap Register 
 // C) Setup QWR register to send 4 Bytes(8-bits), and set CS to active low
 ConfigMAX132.QWR = ( QSPI_QWR_ENDQP(3)|QSPI_QWR_NEWQP(0)|QSPI_QWR_CSIV );
 
 // D) QSPI Comand RAM Registers[0] - 4 discrete 8 bit transfers.
 ConfigMAX132.QCR[0] = (QSPI_CRX_BITSE|QSPI_CRX_DT|chipSelectUsed); // note CONT bit not set.|QSPI_CRX_CONT  
 ConfigMAX132.QCR[1] = (QSPI_CRX_BITSE|QSPI_CRX_DT|chipSelectUsed);  
 ConfigMAX132.QCR[2] = (QSPI_CRX_BITSE|QSPI_CRX_DT|chipSelectUsed);  
 ConfigMAX132.QCR[3] = (QSPI_CRX_BITSE|QSPI_CRX_DT|chipSelectUsed);  //QSPI_CRX_DSCK|

 QSPI_BufferMAX132[0] = MAX132_READ_REG1;     // Read higher bits
 QSPI_BufferMAX132[1] = MAX132_READ_REG0;     // Read lower bits
 QSPI_BufferMAX132[2] = MAX132_START_READ_REG2;     // Start command, and read status reg (reg2) 
 QSPI_BufferMAX132[3] = (MuxBits << 4) | 0x01;     //   Set mux bits (will correspond to reg2 read).
 
 QSPI_WriteQTR(ptrQSPI_BufferMAX132, XfrLen, ptrConfigMAX132);

// Copy ALL QRR 
//    nRxBuffer[0] = QSPI_READ_QRR(0);// Read Transfer information - word will be in QRR(0) & QRR(1).
//    nRxBuffer[1] = QSPI_READ_QRR(1);// 
    
    *RxData = QSPI_READ_QRR(1);// Read Transfer information - word will be in QRR(1) & QRR(2) & QRR(3).
    RxData++;
    *RxData = QSPI_READ_QRR(2);//
    RxData++;
    *RxData = QSPI_READ_QRR(3);//

    
    return; 
               
}


 /*
  *
  * Part 1: Start A/D conversion
  *
  */
  
void StartMAX132(WORD chipSelectUsed) 
{

 // Create QSPI Configuration Structure to send write ENABLE instruction
 WORD QSPI_BufferSTARTCNV[1];
 WORD * ptrQSPI_BufferSTARTCNV = QSPI_BufferSTARTCNV;
 WORD sTxLen = 1;
 QSPI_CFG ConfigSTARTCNV;
 QSPI_CFG * ptrConfigSTARTCNV = &ConfigSTARTCNV;
 
 // QSPI Mode Register
 // A) Setup QMR register (8-bit transfer) 
 //								< 1MHz SPI baud rate to MAX132.
 //								CPOL=0, CPHA=0.
 ConfigSTARTCNV.QMR = ( QSPI_QMR_MSTR|QSPI_QMR_BITS_8|QSPI_QMR_DOHIE|QSPI_QMR_BAUD(40) ); 		//|QSPI_QMR_CPOL|QSPI_QMR_CPHA

 // B) QSPI Delay Register
 ConfigSTARTCNV.QDLYR =QSPI_QDLYR_SPE|QSPI_QDLYR_QCD(0)|QSPI_QDLYR_DTL(3);
 
 // QSPI Wrap Register 
 // C) Setup QWR register to send one byte(8-bits), and set CS to active low
 ConfigSTARTCNV.QWR = ( QSPI_QWR_ENDQP(0)|QSPI_QWR_NEWQP(0)|QSPI_QWR_CSIV );
 
 // D) QSPI Comand RAM Registers[0]
 ConfigSTARTCNV.QCR[0] =(QSPI_CRX_BITSE|QSPI_CRX_DSCK|QSPI_CRX_DT|chipSelectUsed);  
 
 QSPI_BufferSTARTCNV[0] = MAX132_START;
 
 QSPI_WriteQTR(ptrQSPI_BufferSTARTCNV, sTxLen, ptrConfigSTARTCNV);
 
} // close function

void SetMAX132MuxOutputs(BYTE MuxBits, WORD chipSelectUsed) 
{

 // Create QSPI Configuration Structure to send Mux instruction
 WORD QSPI_BufferMAX132MUX[1];
 WORD * ptrQSPI_BufferMAX132MUX = QSPI_BufferMAX132MUX;
 WORD MuxTxLen = 1;
 QSPI_CFG ConfigMAX132MUX;
 QSPI_CFG * ptrConfigMAX132MUX = &ConfigMAX132MUX;
 
 // QSPI Mode Register
 // A) Setup QMR register (8-bit transfer) 
 //								< 1MHz SPI baud rate to MAX132.
 //								CPOL=0, CPHA=0.
 ConfigMAX132MUX.QMR = ( QSPI_QMR_MSTR|QSPI_QMR_BITS_8|QSPI_QMR_DOHIE|QSPI_QMR_BAUD(40) ); 		//|QSPI_QMR_CPOL|QSPI_QMR_CPHA

 // B) QSPI Delay Register
 ConfigMAX132MUX.QDLYR =QSPI_QDLYR_SPE|QSPI_QDLYR_QCD(0)|QSPI_QDLYR_DTL(3);
 
 // QSPI Wrap Register 
 // C) Setup QWR register to send one byte(8-bits), and set CS to active low
 ConfigMAX132MUX.QWR = ( QSPI_QWR_ENDQP(0)|QSPI_QWR_NEWQP(0)|QSPI_QWR_CSIV );
 
 // D) QSPI Comand RAM Registers[0]
 ConfigMAX132MUX.QCR[0] =(QSPI_CRX_BITSE|QSPI_CRX_DSCK|QSPI_CRX_DT|chipSelectUsed);  
 
 QSPI_BufferMAX132MUX[0] = (MuxBits << 4) | 0x01; //   Set mux bits
 
 QSPI_WriteQTR(ptrQSPI_BufferMAX132MUX, MuxTxLen, ptrConfigMAX132MUX);
 
} // close function


//  ********Extracting Max132 data************************  //
// Note, this should probably be built into the ReadMAX132() function.
// Left this way to debug the design.

DWORD ExtractMAX132Data( BYTE *MAX132Data, WORD *ptrError )
{
	DWORD lTemp = 0;

	lTemp = MAX132Data[0] << 11;	 // contains Reg1 data - B11-B18.
	lTemp |= (MAX132Data[1] << 3);  //contains Reg0 data - B3-B10.
	lTemp |= (MAX132Data[2] & 0x07);		// Reg2, add in least sign. 3 bits.
//	g_lRawAtDCounts = lTemp;			// raw count data.
		if(!(MAX132Data[2] & 0x40))
		{
			//error - EOC not set
				*ptrError |= 0x0004;							// Set b2 for EOC not set

//				GenerateBeep( );									// sound beep or buzzer.
		}
		else  // conversion had finished.
		{
//			g_bWeightAvailableFlag = TRUE;
			if( MAX132Data[2] & 0x08 )						// check polarity
			{
				*ptrError |= 0x0002;					// set b1 for negative polarity.
				lTemp = 0;						// if negative, set to 0.
			}
			if(lTemp > MAX132_MAXIMUM_VALUE)	// Soft overange limit (279040)
			{
				lTemp = MAX132_MAXIMUM_VALUE;	// limit to maximum
				*ptrError |= 0x0001;							// Set b0 for overrange
//				GenerateBeep( );									// sound beep or buzzer.
			}
		}

	return( lTemp );
 }




