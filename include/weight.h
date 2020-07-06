///////////////////////////////////////////////////////////
// SPIFunctions.h
//
// A library of functions for accessing the SPI EEPROM.
//
//
// M.Parks								12-04-2000
// Converted from Batch Blender assembler.
// 
// P.Smith                      2/2/06
// first pass at netburner hardware conversion.
// comment out #include "StdTypes.h"
// Bool -> BOOL
// included  <basictypes.h>
// added void ReadWeightFromLoadCell (void ) instead of void  HandleMAX132( void );
//
// 
// P.Smith                      14/2/06
// name change to weight.h
//////////////////////////////////////////////////////////


//*****************************************************************************
// INCLUDES
//*****************************************************************************
#ifndef __SPIFUNCTIONS_H__
#define __SPIFUNCTIONS_H__

#include <basictypes.h>

// Status word bit definitions
#define	SPI_STATUS_BUSY_SET		0x80	
#define	SPI_STATUS_BUSY_CLEAR	0x7f
#define	SPI_COMMAND_QUEUE_PTR	0x0f

// Control Word 1 
#define	SPI_CONTROL_SPI_ENABLE	0x8000

// Control Word 1 

// EEPROM
#define	EEPROM_WRITE_STATUS_REG		0x0001	// WRITE TO STATUS REG COMMAND.
#define	EEPROM_WRITE_DATA_CMD		0x0002	// WRITE EEPROM
#define	EEPROM_READ_DATA_CMD		0x0003	// READ EEPROM.
#define	EEPROM_RESET_WRITE_LATCH	0x0004	// WRITE RESET LATCH COMMAND.
#define	EEPROM_READ_STATUS_REG		0x0005	// READ STATUS REG COMMAND.
#define	EEPROM_ENABLE_WRITE_LATCH	0x0006	// WRITE ENABLE LATCH COMMAND

#define	EEPROM_WRITE_IN_PROGRESS	0x01	// WRITE IN PROGRESS.


void ProcessLoadCellWeight (void );
void  HandleMAX132( void );
void CalculateAverage( void );
void SettledAverage( void );
void Tarend( void );
void NormTare( void );
void TareForAutoCycler( void );

void GenerateHalfSecondWeightAverage( void );
void GenerateOneSecondWeightAverage( void );
void GenerateTwoSecondWeightAverage( void );
void Generate8SecondWeightAverage( void );
void InitialiseWeightVariables( void );

#endif // __SPIFUNCTIONS_H__

