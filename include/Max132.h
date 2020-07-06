/*-------------------------------------------------------------------
 File: MAX132.h
 Description: Definitions and prototypes for MAX132 a/d converter.
 -------------------------------------------------------------------*/
 
#ifndef __MAX132_H__
#define __MAX132_H__


/******************************************************************
 *  MAX132 commands
 ******************************************************************/
#define MAX132_START  ( 0xC2 )  			// Start Conv/50Hz/ Read Reg 1
#define MAX132_READ_REG0   ( 0x40 )  	// 50Hz/ Read Reg 0
#define MAX132_READ_REG1   ( 0x42 )  	// 50Hz/ Read Reg 1
#define MAX132_START_READ_REG2 ( 0xC4 )  // Start Conv/50Hz/ Read Reg 2, i.e. status.
#define MAX132_MAXIMUM_VALUE (279040)	// soft over-range value for MAX132.

/******************************************************************
 * This function will read the MAX132 A/D, also sets Mux bits.
 ******************************************************************/
void  ReadMAX132(BYTE MuxBits, WORD chipSelectUsed, BYTE *RxData); 

/******************************************************************
 * This function will start the MAX132 A/D, conversion.
 ******************************************************************/
void StartMAX132(WORD chipSelectUsed); 

void SetMAX132MuxOutputs(BYTE MuxBits, WORD chipSelectUsed);

DWORD ExtractMAX132Data( BYTE *MAX132DATA, WORD *ptrError );

#endif	//__MAX132_H__
