///////////////////////////////////////////////////////////
// TimeDate.c
//
// Holds all of the functions for initialising, setting
// and reading the time and date from the Real Time Clock.
//
//
// M.Parks                                  29-03-2000
// Modified the assembler code from the Batch blender.
//
// P.Smith                                  16-08-2005
// added StorePowerDownTime,
// name change g_PowerDownTime to g_cPowerDownTime
//
// P.Smith                      17/2/06
// first pass at netburner hardware conversion.
// Bool -> BOOL
// #include <basictypes.h>
// removed all unnecessary code.
//
// P.Smith                      30/6/06
// modified functions GetTime, GetDate
//
// P.Smith                          30/1/08
// correct compiler warnings
// remove ConfirmTime
//
// P.Smith                          23/7/08
// remove g_arrnMBTable extern
// correct year set and get
//
// P.Smith                          6/3/09
// check for valid time, CheckForValidTime
// if the time is not valid, it is set to 12:00:0
//
// P.Smith                          1/10/10
// added new rtc mmk
///////////////////////////////////////////////////////////


//*****************************************************************************
// INCLUDES
//*****************************************************************************
#include <stdlib.h>
#include <stdio.h>
#include <basictypes.h>

#include "TimeDate.h"
#include "I2CFuncs.h"
#include "Mtttymux.h"
#include "Batvars2.h"


// Locally defined global variables
extern  struct tm bts;
extern bool g_bPCF8563Fitted;
extern bool g_bX1226Fitted;
extern CalDataStruct    g_CalibrationData;


//////////////////////////////////////////////////////////////////////
// Reads the time from the RTC and stores a converted value in
// CurrentTime
//
// Entry:   Void.
//
// Exit:    Void
//////////////////////////////////////////////////////////////////////
void GetTime( void )
{
    g_CurrentTime[TIME_SECOND] = bts.tm_sec;
    g_CurrentTime[TIME_MINUTE] = bts.tm_min;
    g_CurrentTime[TIME_HOUR] = bts.tm_hour;

}

//////////////////////////////////////////////////////////////////////
// Reads the date from the RTC and stores a converted value in
// CurrentTime
//
// Entry:   Void.
//
// Exit:    Void
//////////////////////////////////////////////////////////////////////
void GetDate( void )
{
      g_CurrentTime[TIME_DATE] = bts.tm_mday;
      g_CurrentTime[TIME_MONTH] = bts.tm_mon;
      g_CurrentTime[TIME_YEAR] = bts.tm_year + TIME_CENTURY;
      g_CurrentTime[TIME_DAY_OF_WEEK] = bts.tm_wday;
}

//////////////////////////////////////////////////////////////////////
// Parses the current time in CurrentTime and if it is valid, writes
// it to the RTC.
//
// Entry:   Void.
//
// Exit:    Void.
//////////////////////////////////////////////////////////////////////
void SetTime( void )
{
      bts.tm_sec = g_CurrentTime[TIME_SECOND];
      bts.tm_min = g_CurrentTime[TIME_MINUTE];
      bts.tm_hour = g_CurrentTime[TIME_HOUR];
	  if(!g_CalibrationData.m_bPCF8563RTCFitted) // normal rtc
	  {
		  IOBoardRTCSetTime( bts );
	  }
	  else
	  {
		  PCF8563RTCSetTime( bts );
	  }


/*nbb
    Bool    bMatched = FALSE;

    // Convert from the integer buffer to
    // individual BCD digits
    ConvertTimeToBuffer();

    do {

        WriteBufferToTime();
        bMatched = ConfirmTime();

    } while( !bMatched );
nbb*/
}

//////////////////////////////////////////////////////////////////////
// Parses the current date in CurrentTime and if it is valid, writes
// it to the RTC.
//
// Entry:   Void.
//
// Exit:    Void.
//////////////////////////////////////////////////////////////////////
void SetDate( void )
{
      bts.tm_mday = g_CurrentTime[TIME_DATE];
      bts.tm_mon = g_CurrentTime[TIME_MONTH];
      bts.tm_year = (g_CurrentTime[TIME_YEAR]) - TIME_CENTURY;
      bts.tm_wday = g_CurrentTime[TIME_DAY_OF_WEEK];

	  if(!g_CalibrationData.m_bPCF8563RTCFitted)
	  {
		  IOBoardRTCSetTime( bts );
	  }
	  else
	  {
		  PCF8563RTCSetTime( bts );
	  }
}




//////////////////////////////////////////////////////
// StorePowerDownTime( void )           from ASM = STRPDNT
// Routine to store the time continuously. This time is then used
// to determine if the blender has been off for a long period of
// time.
//
// P.Smith                          16-08-2005
// First pass.
//////////////////////////////////////////////////////

void StorePowerDownTime( void )
{
    GetTime();  // ASM READTIME
    GetDate(); // ASM READDATE

    g_cPowerDownTime[TIME_MONTH] = g_CurrentTime[TIME_MONTH];
    g_cPowerDownTime[TIME_DATE] = g_CurrentTime[TIME_DATE];
    g_cPowerDownTime[TIME_HOUR] = g_CurrentTime[TIME_HOUR];
    g_cPowerDownTime[TIME_MINUTE] = g_CurrentTime[TIME_MINUTE];
    g_cPowerDownTime[TIME_SECOND] = g_CurrentTime[TIME_SECOND];
}


//////////////////////////////////////////////////////
// CheckForValidTime( void )
// check for valid time, if not set to 12:00
//////////////////////////////////////////////////////

void CheckForValidTime( void )
{
    BOOL bSuccess = TRUE;
    if(!((g_CurrentTime[TIME_HOUR] >=0)&&(g_CurrentTime[TIME_HOUR]<= 23)))
    {
        bSuccess = FALSE;
    }
    else
    if(!((g_CurrentTime[TIME_MINUTE] >=0)&&(g_CurrentTime[TIME_MINUTE]<= 59)))
    {
        bSuccess = FALSE;
    }
    else
    if(!((g_CurrentTime[TIME_SECOND] >=0)&&(g_CurrentTime[TIME_SECOND]<= 59)))
    {
        bSuccess = FALSE;
    }
    if(!bSuccess)
    {
        g_CurrentTime[TIME_HOUR] = 12;   // set some time
        g_CurrentTime[TIME_MINUTE] = 00;
        g_CurrentTime[TIME_SECOND] = 0;
        SetTime();
    }
}





/*

MCE12-15

;  STRPUPT     IS A ROUTINE TO STORE THE POWER UP TIME
STRPDNT JSR    READTIME
        JSR    READDATE
        LDX    #TIMEBUF
        LDAA   DATE,X
        STAA   PDNDOM     ;~DATE
        LDAA   MON,X
        STAA   PDNMON     ;~MONTH
        LDAA   YEAR,X
        STAA   PDNYR      ;~YEAR
        LDAA   HOUR,X
        STAA   PDNHRS     ;~HOUR
        LDAA   MIN,X
        STAA   PDNMIN     ;~MINUTES
        LDAA   SEC,X
        STAA   PDNSEC     ;~SECONDS
        LDAA   DAY,X
        STAA   PDNDOW
        RTS
*/





