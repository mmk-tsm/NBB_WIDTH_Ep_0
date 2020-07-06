/*-------------------------------------------------------------------
 File: EEPROMconstants.h
 Description: EEPROM constants listed
 -------------------------------------------------------------------*/

#ifndef __EEPROM_CONSTS_H__
#define __EEPROM_CONSTS_H__

/******************************************************************
 *  INST: Instructions for EEPROM
 ******************************************************************/
#define EEPROM_INST_READ  (0x0003)  
#define EEPROM_INST_WRITE (0x0002) 
#define EEPROM_INST_WREN  (0x0006) 
#define EEPROM_INST_WRDI  (0x0004) 
#define EEPROM_INST_RDSR  (0x0005) 
#define EEPROM_INST_WRSR  (0x0001)


//status reg bits

#define EEPROM_WIP  (0x01)
#define EEPROM_BP1  (x08)   //
#define EEPROM_BP0  (x04)   //
#define EEPROM_WEL  (x02)   //
#define EEPROM_WPEN (x80)   //

#define EEPROM_SIZE_BYTES (0x2000)  // 8 kbytes.

#endif // __EEPROM_CONSTS_H__
