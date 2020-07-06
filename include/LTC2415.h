/*-------------------------------------------------------------------
 File: LTC2415.h
 Description: Definitions and prototypes for LTC2415 a/d converter.
 -------------------------------------------------------------------*/
 
#ifndef __LTC2415_H__
#define __LTC2415_H__

/******************************************************************
 *  INST: Instructions for EEPROM
 ******************************************************************/
#define LTC2415_EOC_BIT   (0x8000)  //eoc is m.s. bit of m.s. word of result.
#define LTC2415_SIG_BIT   (0x2000)  //Sign bit is b29 of 32 bits.
#define LTC2415_MAX_VALUE  (1000000)  // 1 Million.  use 20 bits, 2 to power 20 = 1048576

/******************************************************************
 * This function will read the LTC2415 A/D 
 ******************************************************************/
void  ReadLTC2415(WORD chipSelectUsed, WORD *RxData); 

#endif // LTC2415_H
