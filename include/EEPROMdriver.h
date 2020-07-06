/*-------------------------------------------------------------------
 File: EEPROMdriver.h
 Description: Read/Write EEPROM function definitions and descriptions.
 -------------------------------------------------------------------*/

#ifndef __EEPROM_DRIVER_H__
#define __EEPROM_DRIVER_H__


/******************************************************************
 * This function will enable writing to the EEPROM given a chip
 * select.
 P.Smith                          8/9/09
 return bool from EEPROMWrite1Byte
 ******************************************************************/
void EEPROM_WREN(WORD chipSelectUsed);



/******************************************************************
 * This function will write up to 16 datums to the QSPI register
 * and send them to the EEPROM.  The case will probably be:
 * 1) write instruction (1 byte)
 * 2) address (2 byte)
 * 3) data (13 bytes)
 ******************************************************************/
void EEPROM_WRITE16(WORD chipSelectUsed, WORD TxLen, WORD * QSPI_BufferWRITE);

/******************************************************************
 * This function will write up to 13 datums to the EEPROM
 * This function is a special case of EEPROM_WREN() and EEPROM_WRITE16()
 ******************************************************************/
void EEPROM_WRITE13(WORD chipSelectUsed, WORD TxLength, WORD * TxData, WORD startAddress);

/******************************************************************
 * This function will write on multiple pages of the EEPROM.
 ******************************************************************/
int  EEPROM_WriteBuffer( WORD * TxData, WORD chipSelectUsed, WORD startAddress, WORD TxLength );



/******************************************************************
 * This function will read 13 datums from the EEPROM given a
 * start address and a chip select used .
 * The data will be left in the QRR (Use QSPI_READ16() to read values).
 * (NOTE: First 3 datums returned can be ignored)
 ******************************************************************/
void EEPROM_READ(WORD chipSelectUsed, WORD startAddress);  // (reading enabled)

/******************************************************************
 * This function will read up to 13 datums to the EEPROM
 * This function is a special case of EEPROM_READ() and QSPI_READ16()
 ******************************************************************/
int EEPROM_Read13(WORD chipSelectUsed, WORD RxLength, WORD * RxData, WORD startAddress);

/******************************************************************
 * This function will read multiple pages of the EEPROM.
 ******************************************************************/
int EEPROM_ReadBuffer( WORD * RxData, WORD chipSelectUsed, WORD startAddress, WORD RxLength );

BYTE EEPROMRead1Byte(WORD chipSelectUsed, WORD startAddress); 

BOOL EEPROMWrite1Byte(WORD chipSelectUsed,  BYTE TxData, WORD startAddress);

void EEPROMWriteDisable(WORD chipSelectUsed); 

void EEPROMWriteStatusRegister(WORD chipSelectUsed,  BYTE NewStatus);
 
BYTE  ReadEEPROMStatusRegister(WORD chipSelectUsed ); 

BOOL IsEEPROMWriteFinished( WORD chipSelectUsed );

BYTE  ReadEEPROMStatusRegister(WORD chipSelectUsed ); 
 
void WriteProtectEEPROM( WORD chipSelectUsed );

void UnProtectEEPROM( WORD chipSelectUsed );

#endif // __EEPROM_DRIVER_H__


