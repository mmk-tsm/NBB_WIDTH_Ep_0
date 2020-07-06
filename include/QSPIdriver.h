/*-------------------------------------------------------------------
 File: QSPIdriver.h
 Description: Read/Write QSPI functions, as well as a structure used
              to setup QSPI to work with your hardware. 
 -------------------------------------------------------------------*/
#ifndef __QSPI_DRIVER_H__
#define __QSPI_DRIVER_H__

/******************************************************************
 * QSPI Configuration type description.
 ******************************************************************/
typedef struct  {

 WORD QMR; // QSPI Mode Register
 WORD QDLYR; // QSPI Delay Register
 WORD QWR; // QSPI Wrap Register
 WORD QIR; // QSPI Interrupt Register 
 WORD QCR[16]; // QSPI Comand RAM Registers
 
} QSPI_CFG;

/******************************************************************
 * This function will write datums into the Transmit ram
 * and initiate transfer.
 ******************************************************************/
int QSPI_WriteQTR( WORD * TxBuffer, WORD TxLen, QSPI_CFG * pcfg );

/******************************************************************
 * This function will read datums from the Recieve ram
 ******************************************************************/
int QSPI_ReadQRR( WORD * RxBuffer, WORD RxLen );

/******************************************************************
 * This function will read 16 datums from the Recieve ram
 * This is just a special case of QSPI_ReadQRR()
 ******************************************************************/
void QSPI_READ16( WORD * RxBuffer );

#endif // __QSPI_DRIVER_H__

