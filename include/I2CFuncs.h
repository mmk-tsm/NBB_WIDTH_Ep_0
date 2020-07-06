//////////////////////////////////////////////////////
// I2CFuncs.h
//
// Contains i2c functiion prototypes.
// First created.
// M.McKiernan								01-12-2005
//
// P.Smith                              	29/11/07
// WriteMAX5812DAC added
//
// M.McKiernan								18/9/2008
// Temperature sensor functions
//
//
// P.Smith								    1/10/10
// added definitions for new rtc mmk
// ******************* For NBB ***************************************

// M.McKiernan								21-05-20043

#ifndef _I2C_FUNCS_H
#define _I2C_FUNCS_H

// #include <time.h>

#define I2C_OK (0)
#define I2C_FAIL (-1)
#define I2CTIMEOUT_MS (20)


#define I2C_FREQ_DIV (0x31)   // GIVES FREQ. DIVIDER OF 192.

// For Temperature sensor TMP100.
#define TMP100_CONFIG_REGISTER   (0x01)
#define TMP100_TEMP_REGISTER     (0x00)
#define TMP100_I2C_ADDR          (0x9C)

extern volatile WORD g_nI2CTimeoutInPits;   // Timeout time in PITs, calculated from PIT freq and ms timeout.(I2CInit)
extern volatile WORD g_nI2CWaitPits;			// Value - decremented each PIT.

// /*Structures:
struct tm
{
  int tm_sec;
  int tm_min;
  int tm_hour;
  int tm_mday;
  int tm_mon;
  int tm_year;
  int tm_wday;
  int tm_yday;
  int tm_isdst;
};
// */


/* Get the current clock time */
/*
   Paramaters:
      bts the basic time struct to fill in with values from the clock.
   Returns -1 on failure, 0 on success
*/
int IOBoardRTCGetTime( struct tm &bts );


/* Set the clock time */
/* Set the current time in the clock.
   Paramaters:
      bts the basic time struct to use to set the clock.
   Returns -1 on failure, 0 on success
*/
int IOBoardRTCSetTime( struct tm &bts );


/* Set the system time using the RTC */
/* returns 0- on success */
//mmk int IOBoardRTCSetSystemFromRTCTime();


/* Set the RTC time using the current system time */
/* returns 0- on success */
//mmk int IOBoardRTCSetRTCfromSystemTime();

void I2CInit( int slave_Addr, int freqdiv );

int EnableRTCInterrupt( void );

int * ReadPCA9555( BYTE *PCA9555Data );

void InitialisePCA9555( void);

// Write to LTC2626 D/A chip.
void WriteLTC2626DAC( WORD nDACValue );

// Write to MAX5812 D/A chip.
void WriteMAX5812DAC( WORD nDACValue );

// Temperature sensor functions
void InitTempSensor( void );
WORD ReadTempSensor(void);

// PCF8563 RTC
int PCF8563RTCSetTime( struct tm &bts );
int InitialisePCF8563( void );
int PCF8563RTCGetTime( struct tm &bts );
bool DetectRTCPCF8563( void );
bool DetectX1226( void );

#endif
