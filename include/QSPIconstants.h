/*-------------------------------------------------------------------
 File: QSPIconstants.h
 Description: Many of the functions and variables that you will use are already 
              been defined in this file.
 P.Smith                                          16/01/07
 EEPROM_SPI_CHIP_SELECT_REVB1 & LTC2415_SPI_CHIP_SELECT_REVB1
  
   P.Smith                      2/2/07
   Removed unused EEPROM select
  
   P.Smith                      22/6/07
   added SPI_BAUD_EXPAN
 -------------------------------------------------------------------*/

#ifndef __QSPI_CONSTS_H__
#define __QSPI_CONSTS_H__



//#include <..\MOD5272\system\sim5272.h> 
#include <C:\Nburn\MOD5270\system\sim5270.h>

/******************************************************************
 * BASE: Base register
 *  These are the locations of the registers
 *  outlined in the QSPI RAM model.
 ******************************************************************/
#define  QSPI_BASE_QTR ( 0 )    /* the Transmit RAM #0 memory location */
#define  QSPI_BASE_QRR (0x10) /* the Receive  RAM #0 memory location */
#define  QSPI_BASE_QCR (0x20) /* the Command  RAM #0 memory location */




/****************mmk*************************************************/

#define EEPROM_CHIP_SELECT  (0x0e00)   // EEPROM chip select CS0  (AS ON DEV KIT - USING CS0)

//FOR NBB BOARD.
#define EEPROM_SPI_CHIP_SELECT  (0x0d00)    // EEPROM (U7) chip select CS1=0, CS0=1  
#define EEPROM_SPI_CHIP_SELECT_REVB1 (0x0e00) // EEPROM (U7) chip select CS1=1, CS0=0 
                                    // Chip selects CS3:0 are in bits 11-8.
#define EEPROM_SPI_CHIP_SELECT_REVB2 (0x0C00) // EEPROM (U7) chip select CS0=1, CS0=0 
                                    
#define FLASH_SPI_CHIP_SELECT  (0x0c00)    // FLASH (U8) chip select CS1=0, CS0=0. 
#define LTC2415_SPI_CHIP_SELECT (0x0e00)   // ltc2415 (U52) chip select CS1=1, CS0=0. 
#define LTC2415_SPI_CHIP_SELECT_REVB1 (0x0e00)   // ltc2415 (U52) chip select CS1=1, CS0=0. 

#define LTC2415_SPI_CHIP_SELECT_REVB2 (0x0C00)   // ltc2415 (U52) chip select CS1=0, CS0=0. 

#define CAN_SPI_CHIP_SELECT     (0x0c00)    // CAN CONTROLLER (U47) chip select CS1=0, CS0=0. 
#define CAN_SPI_CHIP_SELECT_REVB2  (0x0E00)    // CAN CONTROLLER (U47) chip select CS1=1, CS0=0. 
#define MAX132_SPI_CHIP_SELECT  (0x0d00)    // MAX132 (A/D U103) chip select CS1=0, CS0=1. 

#define SPIEXCS1_CHIP_SELECT  (0x0cC00)    // SPI EXPANSION chip select 1 CS1=0, CS0=0. 
#define SPIEXCS1_CHIP_SELECT_REVB2  (0x0E00)    // SPI EXPANSION chip select 1 CS1=1, CS0=0. 

#define SPIEXCS2_CHIP_SELECT  (0x0d00)    // SPI EXPANSION chip select 2 CS1=0, CS0=1. 
#define SPIEXCS3_CHIP_SELECT  (0x0e00)    // SPI EXPANSION chip select 3 CS1=1, CS0=0. 

#define VNC1L_SPI_CHIP_SELECT (0x0d00)    // SPI TO USB - VNC1L. CS1=0, CS0=1. bits 11-8 == cs3:0.

#define VNC1L_SPI_CHIP_SELECT_REVB2  (0x0E00)    // SPI TO USB - VNC1L. CS1=1, CS0=0. bits 11-8 == cs3:0.

#define SPI_EX1_CHIP_SELECT  (0x0d00)    // SPI EXPANSION chip select 1 CS1=0, CS0=1. 

#define SPI_EX1_CHIP_SELECT_REVB2  (0x0E00)    // SPI EXPANSION chip select 1 CS1=1, CS0=0. 
#define SPI_AD2_CHIP_SELECT_REVB2  (0x0E00)     // 2nd A/D chip.

#define SPI_BAUD_EXPAN                 (74)


/******************************************************************
 * QMR: QSPI Mode Register
 *  The mode register determines the basic operating modes of the 
 *  QSPI module.
 ******************************************************************/ 
#define QSPI_QMR_MSTR     (0x8000)                               
#define QSPI_QMR_DOHIE    (0x4000)
#define QSPI_QMR_BITS_16  (0)
#define QSPI_QMR_BITS_8   (0x2000)
#define QSPI_QMR_BITS_9   (0x2400)
#define QSPI_QMR_BITS_10  (0x2800)
#define QSPI_QMR_BITS_11  (0x2C00)
#define QSPI_QMR_BITS_12  (0x3000)
#define QSPI_QMR_BITS_13  (0x3400)
#define QSPI_QMR_BITS_14  (0x3800)
#define QSPI_QMR_BITS_15  (0x3C00)
#define QSPI_QMR_CPOL     (0x0200)
#define QSPI_QMR_CPHA     (0x0100)
#define QSPI_QMR_BAUD(x)  ((x & 0xff))// must be a value between 2-255
                                      // here we use a mask 0xFF to guarantee
                                      // a number (0>x>255)
                                      
/******************************************************************
 * QDLYR: QSPI Delay Register
 ******************************************************************/
#define QSPI_QDLYR_SPE    (0x8000)
#define QSPI_QDLYR_QCD(x) ((x & 0x7F)<<8)  // in this example we shift
                                           // 8 bits to the left because
                                           // we want this data to end up in 
                                           // bits 8-14
#define QSPI_QDLYR_DTL(x) (x & 0xFF)

/******************************************************************
 * QWR: QSPI Wrap Register
 ******************************************************************/
#define QSPI_QWR_HALT (0x8000)
#define QSPI_QWR_WREN (0x4000)
#define QSPI_QWR_WRTO (0x2000)
#define QSPI_QWR_CSIV (0x1000)
#define QSPI_QWR_ENDQP(x) ((x & 0x0f)<<8)
#define QSPI_QWR_NEWQP(x) ((x & 0x0f))

/******************************************************************
 * QIR: QSPI Interrupt Register
 ******************************************************************/
#define QSPI_QIR_WCEFB (0x8000)
#define QSPI_QIR_ABRTB (0x4000)
#define QSPI_QIR_ABRTL (0x1000)
#define QSPI_QIR_WCEFE (0x0800)
#define QSPI_QIR_ABRTE (0x0400)
#define QSPI_QIR_SPIFE (0x0100)
#define QSPI_QIR_WCEF  (0x0008)
#define QSPI_QIR_ABRT  (0x0004)
#define QSPI_QIR_SPIF  (0x0001)

/******************************************************************
 *  CRX: Command Ram X
 *  Command Register Ram
 ******************************************************************/
#define QSPI_CRX_CONT  (0x8000)
#define QSPI_CRX_BITSE (0x4000)
#define QSPI_CRX_DT    (0x2000)
#define QSPI_CRX_DSCK  (0x1000)
#define QSPI_CRX_CS0   (0x0100)
#define QSPI_CRX_CS1   (0x0200)
#define QSPI_CRX_CS2   (0x0400)
#define QSPI_CRX_CS3   (0x0800) 
                          
/******************************************************************
 *  QTR: QSPI Transmit Ram
 *  Write to Transmit ram register(registerOfTR), data to transmit
 ******************************************************************/
inline void QSPI_WRITE_QTR(int registerOfTR, int dataToTransmit) { 
       sim.qspi.qar = (registerOfTR + QSPI_BASE_QTR); 
       sim.qspi.qdr = dataToTransmit; 
};

/******************************************************************
 *  QCR: QSPI Command RAM
 *  Write to command register(registerOfCR), control information data
 ******************************************************************/
inline void QSPI_WRITE_QCR(int registerOfCR, int controlInformationData) { 
       sim.qspi.qar=(registerOfCR + QSPI_BASE_QCR);              
       sim.qspi.qdr=controlInformationData; 
};

/******************************************************************
 *  QRR: QSPI Receive RAM
 *  Read from recieve register(registerOfRR), return a word of data
 ******************************************************************/
inline WORD QSPI_READ_QRR(int registerOfRR)   {
       sim.qspi.qar=(registerOfRR + QSPI_BASE_QRR); return sim.qspi.qdr; 
};


#endif // __QSPI_CONSTS_H__
