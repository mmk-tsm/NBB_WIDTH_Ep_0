/*-------------------------------------------------------------------
 File: QSPIdriver.cpp
 Description: Read/Write QSPI functions.


 // M.McKiernan	07-11-2005	 time delay & iprintf removed from QSPI_WriteQTR
 										//mmk0711  OSTimeDly(1);  
										//mmk0711  iprintf(".");  

    P.Smith                               6/2/07
    Remove printfs
 -------------------------------------------------------------------*/

#include <basictypes.h>
#include <stdio.h>  
#include <ctype.h>
#include <ucos.h>

#include "QSPIconstants.h"
#include "QSPIdriver.h"


int QSPI_WriteQTR( WORD * TxBuffer, WORD TxLen, QSPI_CFG * pcfg ) {

  // GOALS FOR THIS FUNCTION
  // 1 Setup transfer (QMR, QWR, QCR0-TxLen,
  //   copy * TxBuffer into QTR)  
  // 2 Initiate transfer
  // 3 Wait for completion
    
//mmk0711  OSTimeDly(1);  
//mmk0711  iprintf(".");  // This is only a visual marker to track progress  
      
  // NOTE: no error condition if nothing is to be transferred.
  if (TxLen>16){
//    iprintf("ERROR:  Loaded Register w/ more than 16 datums!\n"); //nbb--testonly--
    return(-1);
  }
           
  // Setup QMR register
  sim.qspi.qmr = pcfg->QMR;
	
  // Setup QWR register
  sim.qspi.qwr = pcfg->QWR;	 
    
  for(WORD setUpReg = 0; setUpReg < TxLen; setUpReg++) {   
    // Setup Command Register
    QSPI_WRITE_QCR(setUpReg,pcfg->QCR[setUpReg]);     

    // Write transfer information
    QSPI_WRITE_QTR(setUpReg,*TxBuffer);
    TxBuffer++; // move pointer over to next value
  }
    
  // Initiate the send to EEPROM by setting delay register
  sim.qspi.qdlyr=pcfg->QDLYR;
    
  // Wait for transfer to complete
  while (sim.qspi.qdlyr & QSPI_QDLYR_SPE);  // polling      
  return(1);  
}  // close function



// Copy QRR to RxBuff
int QSPI_ReadQRR( WORD * ptrRxBuffer, WORD RxLen ) {

  OSTimeDly(1);
//  iprintf("-");  // This is only a visual marker to track progress  //nbb--testonly--
 
  if ( (RxLen >16) || (RxLen==0) ) {
//    iprintf("ERROR: RX has too many (>16) or too little (<=0) values"); //nbb--testonly--
    return(-1);
  }  
  for(WORD readReg = 0; readReg < RxLen; readReg++) {        
    *ptrRxBuffer = QSPI_READ_QRR(readReg);// Read Tansfer information
    // iprintf("RxBuffer %d: %d\n",readReg,*ptrRxBuffer);
    ptrRxBuffer++; // move pointer over to next value
  }
  return ( 1 );
} // close function



// Copy ALL QRR to RXBuff
void QSPI_READ16( WORD * ptrRxBuffer ) {
  #define RxLen 16
  QSPI_ReadQRR( ptrRxBuffer, RxLen );         
}  // close function








