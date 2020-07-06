///////////////////////////////////////////////////////////
// LTC2415.cpp
//
// A library of functions for accessing the LTC2415 a/d
//
//
// M.McKiernan								23-11-2005
//
// 
// P.Smith                      11/4/07
// call SPI_Select_AD on a/d read
//////////////////////////////////////////////////////////


//*****************************************************************************
// INCLUDES
//*****************************************************************************
#include <startnet.h>

#include <basictypes.h>
#include <stdio.h> 

#include "QSPIconstants.h"
#include "QSPIdriver.h"

#include "LTC2415.h"
#include "NBBGpio.h"




/*
  * Read LTC2415 a/d.
  *
*/
void  ReadLTC2415(WORD chipSelectUsed, WORD *RxData) 
{ 
 SPI_Select_AD();       // select a/d spi
                 
          
 // Create QSPI Configuration Structure to read LTC2415
 WORD nRxBuffer[2];
 WORD nRdLen = 2;
 
 WORD QSPI_Buffer2415[2];
 WORD * ptrQSPI_Buffer2415 = QSPI_Buffer2415;
 QSPI_CFG Config2415;
 QSPI_CFG * ptrConfig2415 = &Config2415;
 
 // QSPI Mode Register
 // A) Setup QMR register (16-bit transfer) 
 //ConfigRDSR.QMR = ( QSPI_QMR_MSTR|QSPI_QMR_CPOL|QSPI_QMR_CPHA|QSPI_QMR_BITS_16|QSPI_QMR_DOHIE|QSPI_QMR_BAUD(19) );
 Config2415.QMR = ( QSPI_QMR_MSTR|QSPI_QMR_BITS_16|QSPI_QMR_DOHIE|QSPI_QMR_BAUD(20) ); //cpol=0, cpha=0
                                                                                       //baud = just under 2MHz (same as for eeprom)

 // B) QSPI Delay Register
 Config2415.QDLYR = QSPI_QDLYR_SPE|QSPI_QDLYR_QCD(0)|QSPI_QDLYR_DTL(1);
 
 // QSPI Wrap Register 
 // C) Setup QWR register to send 2 words(16-bits), and set CS to active low
 Config2415.QWR = ( QSPI_QWR_ENDQP(1)|QSPI_QWR_NEWQP(0)|QSPI_QWR_CSIV );
 
 // D) QSPI Comand RAM Registers[0]
 Config2415.QCR[0] = (QSPI_CRX_BITSE|QSPI_CRX_DSCK|QSPI_CRX_DT|QSPI_CRX_CONT|chipSelectUsed); // note CONT bit set.   
 Config2415.QCR[1] =(QSPI_CRX_BITSE|QSPI_CRX_DSCK|QSPI_CRX_DT|chipSelectUsed);  

 QSPI_Buffer2415[0] = 0;                    // dummy word.
 QSPI_Buffer2415[1] = 0;	 				// dummy word.
 
 QSPI_WriteQTR(ptrQSPI_Buffer2415, nRdLen, ptrConfig2415);

// Copy ALL QRR to RXBuff
    nRxBuffer[0] = QSPI_READ_QRR(0);// Read Transfer information - word will be in QRR(0) & QRR(1).
    nRxBuffer[1] = QSPI_READ_QRR(1);// 
    
    *RxData = nRxBuffer[0];
    RxData++;
    *RxData = nRxBuffer[1];
    
    return; 
               
}

