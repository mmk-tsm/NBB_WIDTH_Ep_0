///////////////////////////////////////////////////////////
// TimeDate.h
//
// Holds all of the functions declarations related to the
// Real Time Clock.
//
//
// M.Parks                                  29-03-2000
// Modified the assembler code from the Batch blender.
// 
// P.Smith                      16/1/06
// first pass at netburner hardware conversion.
// Bool -> BOOL
// 
// P.Smith                      2/2/06
//#include <basictypes.h>
// 
// P.Smith                      23/7/08
// remove TIME_ARRAY_SIZE def from here
// 
// P.Smith                      1/4/09
// added CheckForValidTime
///////////////////////////////////////////////////////////

//*****************************************************************************
// MACROS
//*****************************************************************************
#ifndef __TIMEDATE_H__
#define __TIMEDATE_H__

//*****************************************************************************
// INCLUDES
//*****************************************************************************
#include <stdlib.h>
#include <basictypes.h>


// Data offsets in TimeBuffer
#define TIME_SECOND         0
#define TIME_MINUTE         1
#define TIME_HOUR           2
#define TIME_DATE           3
#define TIME_MONTH          4
#define TIME_YEAR           5
#define TIME_DAY_OF_WEEK    6


#define TIME_CENTURY        2000

// Reg E masks
#define RTC_DISABLE_INTERRUPT       0x01    // Bit 0
#define RTC_INTERRUPT_OUTPUT        0x02    // Bit 1
#define RTC_INTERRUPT_PERIOD_64TH   0x00    // Bits 2, 3
#define RTC_INTERRUPT_PERIOD_1SEC   0x04
#define RTC_INTERRUPT_PERIOD_1MIN   0x08
#define RTC_INTERRUPT_PERIOD_1HOUR  0x0c

// This is used in some of the sequencer timing so it must be
// modified to reflect the actual rate.
#define RTC_INTERRUPTS_PER_MINUTE   60

// Functions available to the programmer
void    GetTime( void );
void    GetDate( void );
void    SetTime( void );
void    SetDate( void );
void StorePowerDownTime( void );
void CheckForValidTime( void );


// DATA OFFSETS IN RTCRAM BUFFER
// These should not be used directly
#define BUFFER_RAM_SECOND_1     0x0     // 1-SECOND DIGIT REG.
#define BUFFER_RAM_SECOND_10    0x1     // 10-SECOND DIGIT REG.
#define BUFFER_RAM_MINUTE_1     0x2     // 1-MINUTE DIGIT REG.
#define BUFFER_RAM_MINUTE_10    0x3     // 10-MINUTE DIGIT REG.
#define BUFFER_RAM_HOUR_1       0x4     // 1-HOUR DIGIT REG.
#define BUFFER_RAM_HOUR_10      0x5     // 10-HOUR DIGIT REG.
#define BUFFER_RAM_DATE_1       0x6     // 1-DAY (DATE) DIGIT REG.
#define BUFFER_RAM_DATE_10      0x7     // 10-DAY (DATE) DIGIT REG.
#define BUFFER_RAM_MONTH_1      0x8     // 1-MONTH DIGIT REG.
#define BUFFER_RAM_MONTH_10     0x9     // 10-MONTH DIGIT REG.
#define BUFFER_RAM_YEAR_1       0xA     // 1-YEAR DIGIT REG.
#define BUFFER_RAM_YEAR_10      0xB     // 10-YEAR DIG. REG
#define BUFFER_RAM_DAY_OF_WEEK  0xC     // WEEKDAY REG.


// Functions called by low level routines.
// These should not be called directly


#endif  //__TIMEDATE_H__
