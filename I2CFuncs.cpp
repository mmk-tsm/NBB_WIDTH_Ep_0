//////////////////////////////////////////////////////
// I2CFuncs.cpp
//
// Contains I2C funtions, and RTC (X1226) functions
//
// First created.
// M.McKiernan								01-12-2005
//
// PIT_FREQUENCY.
//  P.Smith                                 28/11/07
// added WriteMAX5812DAC
// M.McKiernan								18/9/2008
// Temperature sensor functions
//
//  P.Smith                                 6/1/09
// a problem has been seen on the month increment, it did not increment from
// dec to january, instead it moved onto the 13th month
// this is corrected by removing the offset of 1 from the month
//
// P.Smith                                 25/5/09
// check for time mins between 0 and 59, if not then reset ic2 and generate alarm
// if diagnostics password generate alarm.
// set g_bI2CError so that event log is recorded.
//
// P.Smith                                 12/1/10
// modify read of rtc to check the status of the i2c functions.
// if they have failed, then do not copy the rtc data and generate an alarm / event.
// only transfer the time if there are no errors.
//
// P.Smith                                 12/1/10
// removed temporary structure for the time
// only read the time if i2c okay
//
// P.Smith                                 19/3/10
// set up g_nI2CTimeoutInPits each time in IOBoardRTCGetTime.
// this avoids any possible corruption in ram.
//
// P.Smith                                 30/9/10
// added new rtc PCF8563RTCGetTime & PCF8563RTCSetTime
//////////////////////////////////////////////////////

#include "predef.h"
#include <stdio.h>
#include <ctype.h>
//#include <time.h>
#include <string.h>
#include <basictypes.h>
#include <utils.h>
#include <ucos.h>
#include "I2CFuncs.h"
#include "NBBGpio.h"
#include "..\MOD5270\system\sim5270.h"
#include "batvars.h"
#include "batvars2.h"
#include "BatchCalibrationFunctions.h"
#include "Alarms.h"

extern CalDataStruct    g_CalibrationData;

volatile WORD g_nI2CTimeoutInPits;   // Timeout time in PITs, calculated from PIT freq and ms timeout.(I2CInit)
volatile WORD g_nI2CWaitPits;			// Value - decremented each PIT.
/* Functions to implement Time */
/* The following two functions are all you need to implment time if you have some method of setting time */
static time_t TimeOffset;
extern BYTE g_nPCF8563Reg0B;
extern bool g_bPCF8563Fitted;

BYTE g_nReg36Initial;
BYTE g_nReg36Final;

extern  struct tm bts;

time_t time( time_t *pt )
{
   time_t tv = Secs + TimeOffset;
   if ( pt )
   {
      *pt = tv;
   }
   return tv;
}


time_t set_time( time_t time_to_set )
{
   TimeOffset = time_to_set - Secs;
   time_t tv = Secs + TimeOffset;
   return tv;
}


BYTE i2cbuf[256];


void I2CInit( int slave_Addr, int freqdiv )
{

	g_nI2CTimeoutInPits = ((I2CTIMEOUT_MS * PIT_FREQUENCY) / 1000);  //Timeout time in PITs, calculated from PIT freq and ms timeout.

   volatile BYTE bv;
//mmk   sim.gpio.paspar |= 0x0f;
   sim.gpio.par_feci2c |=0x0f;                 // set PAR_CSL & PAR_SDA up for I2C
   sim.i2c.adr = slave_Addr;
   sim.i2c.fdr = freqdiv;
   sim.i2c.cr = 0x0;
//mmk   sim.i2c.cr = 0xA;
   sim.i2c.cr = 0xA0;
   bv = sim.i2c.dr;
   sim.i2c.sr = 0x0;
   sim.i2c.cr = 0x0;
   OSTimeDly( 1 );
   sim.i2c.cr = 0xB0; /* 10110100 */
   OSTimeDly( 1 );
   sim.i2c.cr = 0x90; /* 10110100 */
   OSTimeDly( 1 );
   sim.i2c.sr = 0x12;
   OSTimeDly( 1 );
   sim.i2c.sr = 0x0;
   OSTimeDly( 1 );
}

// from i2cdemo, files section of Netburner yahoo group
/*
// *********************************************************************
// ************************  INIT I2C REGISTERS  ***********************
// *********************************************************************
void I2CInit( BYTE Slave_Addr, BYTE freqdiv )
{

    sim.gpio.par_feci2c |=0x0f;                 // set PAR_CSL & PAR_SDA up for I2C
                                                //
												//
    sim.i2c.adr = Slave_Addr;					// Set our address when we are a slave
    sim.i2c.fdr = freqdiv;						// Set the FREQ Divider register 0x00- 0x3f
                                                //
	I2CResetFault();							// Initalize the I2C signals and clear and Fault
												// conditions that may exsist
}
// *********************************************************************
// ***************  CLEAR ANY FAULTS AND RESET THE I2C   ***************
// *********************************************************************
void I2CResetFault(void)
{

	BYTE temp;

    sim.i2c.cr = 0x00;							// This is per Motorola spec these next few lines
    sim.i2c.cr = 0xA0;   						// They are to RE-initalize all the I2C devices to IDLE state
    temp = sim.i2c.dr;							// dummy read to clear
    sim.i2c.sr = 0x00;							//
    sim.i2c.cr = 0x00;							//
	OSTimeDly( 1 );								//
}

*/
int I2CSend( BYTE val, DWORD nPITs_to_wait )
{
   sim.i2c.dr = val;
   g_nI2CWaitPits = nPITs_to_wait; 		// value decremented by PIT.
   while ( ( sim.i2c.sr & 0x2 ) == 0 )
   {
//mmk      OSTimeDly( 1 );
//mmk       nPITs_to_wait--;
//mmk       if ( nPITs_to_wait == 0 )
      if( g_nI2CWaitPits == 0 )     // this value is decremented by PIT
      {
         return I2C_FAIL;
      }
   }
   sim.i2c.sr = 0x0;
   return I2C_OK;
}



int I2CStart( BYTE addr, DWORD nPITs_to_wait )
{
   g_nI2CWaitPits = nPITs_to_wait;
   while ( sim.i2c.sr & 0x20 )
   {
//mmk      OSTimeDly( 1 );
//mmk       nPITs_to_wait--;
//mmk       if ( nPITs_to_wait == 0 )
      if( g_nI2CWaitPits == 0 )     // this value is decremented by PIT
      {
         return I2C_FAIL;
      }
   }

   sim.i2c.cr = 0xB0;
   sim.i2c.sr = 0x0;
   return I2CSend( addr, nPITs_to_wait );
}



int I2CStop()
{
   sim.i2c.cr &= 0xDF;
   return I2C_OK;
}




int I2CRestart( BYTE addr, DWORD nPITs_to_wait )
{
   sim.i2c.cr |= 0x04;
   g_nI2CWaitPits = nPITs_to_wait;

   while ( ( sim.i2c.sr & 0x20 ) == 0 )
   {
//mmk      OSTimeDly( 1 );
//mmk       nPITs_to_wait--;
//mmk       if ( nPITs_to_wait == 0 )
      if( g_nI2CWaitPits == 0 )     // this value is decremented by PIT
      {
         return I2C_FAIL;
      }
   }

   sim.i2c.sr = 0x0;

   return I2CSend( addr, nPITs_to_wait );
}

int I2CRead( PBYTE rx_buffer, int count, DWORD nPITs_to_wait )
{
   int n = 0;
   volatile BYTE bv;

   sim.i2c.cr = 0xA0;  /*10100000 */

   bv = sim.i2c.dr;

   while ( count )
   {
      if ( count == 1 )
      {
         sim.i2c.cr |= 0x08;
      }

      g_nI2CWaitPits = nPITs_to_wait;

      while ( ( sim.i2c.sr & 0x2 ) == 0 )
      {

//mmk         OSTimeDly( 1 );
//mmk         nPITs_to_wait--;
//mmk         if ( nPITs_to_wait == 0 )
         if( g_nI2CWaitPits == 0 )     // this value is decremented by PIT
         {
            return -n;
        }
      }

      sim.i2c.sr = 0x0;
      bv = sim.i2c.dr;
      rx_buffer[n++] = bv;
//      OSTimeDly( 1 );
      g_nI2CWaitPits = 10;
      while( g_nI2CWaitPits )         // Wait till  PIT decrements it to 0 (1-2mS)
       ;
           count--;
   }
   sim.i2c.cr &= ~0x08;
   return n;
}





BYTE MakeBCD( int i )
{
   BYTE bv = i % 10;
   bv |= ( ( i / 10 ) % 10 ) * 16;
   return bv;
}

int GetBCD( BYTE bv )
{
   int i = bv & 0x0F;
   i += 10 * ( ( bv >> 4 ) & 0x0f );
   return i;
}

int IOBoardRTCGetTime( struct tm &bts )
{
   int nStatus1=0,nStatus2=0,nStatus3=0,nStatus4=0,nStatus5=0,nStatus6=0;
   BOOL   bOkay = TRUE;
   BYTE Read_buffer[10];
   g_nI2CTimeoutInPits = ((I2CTIMEOUT_MS * PIT_FREQUENCY) / 1000);  //Timeout time in PITs, calculated from PIT freq and ms timeout.

   nStatus1 = I2CStart( 0xDE, g_nI2CTimeoutInPits );
   if(nStatus1 == I2C_FAIL)
   bOkay = FALSE;
   if(bOkay)
   {
   nStatus2 = I2CSend( 0x00, g_nI2CTimeoutInPits );
   if(nStatus2 == I2C_FAIL)
   bOkay = FALSE;
   }
   if(bOkay)
   {
   nStatus3 = I2CSend( 0x30, g_nI2CTimeoutInPits );
   if(nStatus3 == I2C_FAIL)
   bOkay = FALSE;
   }
   if(bOkay)
   {
   nStatus4 = I2CRestart( 0xDF, g_nI2CTimeoutInPits );
   if(nStatus4 == I2C_FAIL)
   bOkay = FALSE;
   }
   if(bOkay)
   {
   nStatus5 = I2CRead( Read_buffer, 8, g_nI2CTimeoutInPits );
   if(nStatus5 < 0 )  // returns - n on return.
   bOkay = FALSE;
   }
   nStatus6 = I2CStop();

   if(bOkay)
   {
       bts.tm_sec = GetBCD( Read_buffer[0] );
       bts.tm_min = GetBCD( Read_buffer[1] );
       bts.tm_hour = GetBCD( Read_buffer[2] & 0x7F );
       bts.tm_mday = GetBCD( Read_buffer[3] );
       bts.tm_mon = GetBCD( Read_buffer[4] );
       bts.tm_year = GetBCD( Read_buffer[5] );      //mmk  + 100;
       bts.tm_wday = GetBCD( Read_buffer[6] );
   }
   else  // not okay
   {
       if(fdTelnet > 0)
       iprintf("\ntimeout %d s1 %d s2 %d s3 %d s4 %d s5 %d s6 %d",g_nI2CTimeoutInPits,nStatus1,nStatus2,nStatus3,nStatus4,nStatus5,nStatus6);
   }


   if(!((bts.tm_min >= 00) && (bts.tm_min <= 59))||((sim.i2c.sr & 0x20) != 0)||!bOkay)
   {
       if(fdTelnet > 0)
       iprintf("\nseq%d %d sr %x cr %x fdr %x okay %x",g_cBatSeq,bts.tm_min,sim.i2c.sr,sim.i2c.cr,sim.i2c.fdr,bOkay);
       g_bI2CError = TRUE;

       if(g_bDiagnosticsPasswordInUse)        // only allow alarm if diagnostics password
       {
           PutAlarmTable( I2CSTOPPEDALARM,  0 );      //
       }
       I2CInit( 0, I2C_FREQ_DIV );	 //  from 147456000 clock.
   }
   else
   {
      if(g_bDiagnosticsPasswordInUse)        // only allow alarm if diagnostics password
      {
          RemoveAlarmTable( I2CSTOPPEDALARM,  0 );      //
      }
   }

   return 0;
}


int PCF8563RTCGetTime( struct tm &bts )
{
   int nStatus1=0,nStatus2=0,nStatus3=0,nStatus4=0,nStatus5=0,nStatus6=0;
   BOOL   bOK = TRUE;
   BYTE Read_buffer[10];
   g_nI2CTimeoutInPits = ((I2CTIMEOUT_MS * PIT_FREQUENCY) / 1000);  //Timeout time in PITs, calculated from PIT freq and ms timeout.

   nStatus1 = I2CStart( 0xA2, g_nI2CTimeoutInPits );

   if( !g_CalibrationData.m_bPCF8563RTCFitted )
		   return(-1);		// return invalid value if 8563 not fitted.

   if(nStatus1 == I2C_FAIL)
	   bOK = FALSE;
//   if(bOK)
//   {
//	   nStatus2 = I2CSend( 0x00, g_nI2CTimeoutInPits );
//	   if(nStatus2 == I2C_FAIL)
//		   bOK = FALSE;
//   }
   if(bOK)
   {
	   nStatus3 = I2CSend( 0x02, g_nI2CTimeoutInPits );		// address for seconds register
	   if(nStatus3 == I2C_FAIL)
		   bOK = FALSE;
   }
   if(bOK)
   {
	   nStatus4 = I2CRestart( 0xA3, g_nI2CTimeoutInPits );	//
	   if(nStatus4 == I2C_FAIL)
		   bOK = FALSE;
   }
   if(bOK)
   {
	   nStatus5 = I2CRead( Read_buffer, 8, g_nI2CTimeoutInPits );
	   if(nStatus5 < 0 )  // returns - n on return.
		   bOK = FALSE;
   }
   nStatus6 = I2CStop();

   if(bOK)
   {
       bts.tm_sec =  GetBCD( Read_buffer[0]  & 0x7F );		// clear VL bit (todo, need to check for valid)
       bts.tm_min =  GetBCD( Read_buffer[1]  & 0x7F  );
       bts.tm_hour = GetBCD( Read_buffer[2]  & 0x3F  );
       bts.tm_mday = GetBCD( Read_buffer[3]  & 0x3F  );
       bts.tm_wday = GetBCD( Read_buffer[4]  & 0x07  );

       bts.tm_mon = GetBCD( Read_buffer[5] & 0x1F );
       bts.tm_year = GetBCD( Read_buffer[6] );      //mmk  + 100;
   }
   else  // not okay
   {
       if(fdTelnet > 0)

       iprintf("\ntimeout %d s1 %d s2 %d s3 %d s4 %d s5 %d s6 %d",g_nI2CTimeoutInPits,nStatus1,nStatus2,nStatus3,nStatus4,nStatus5,nStatus6);
   }


   if(!((bts.tm_min >= 00) && (bts.tm_min <= 59))||((sim.i2c.sr & 0x20) != 0)||!bOK)
   {
       if(fdTelnet > 0)
       iprintf("\nseq%d %d sr %x cr %x fdr %x okay %x",g_cBatSeq,bts.tm_min,sim.i2c.sr,sim.i2c.cr,sim.i2c.fdr,bOK);
       iprintf("\nBuf0= %02x Buf1= %02x Buf2= %02x Buf3= %02x",Read_buffer[0],Read_buffer[1],Read_buffer[2],Read_buffer[3] );
       g_bI2CError = TRUE;

       if(g_bDiagnosticsPasswordInUse)        // only allow alarm if diagnostics password
       {
           PutAlarmTable( I2CSTOPPEDALARM,  0 );      //
       }
       I2CInit( 0, I2C_FREQ_DIV );	 //  from 147456000 clock.
   }
   else
   {
      if(g_bDiagnosticsPasswordInUse)        // only allow alarm if diagnostics password
      {
          RemoveAlarmTable( I2CSTOPPEDALARM,  0 );      //
      }
   }

   return 0;
}


int IOBoardRTCSetTime( struct tm &bts )
{
//mmk   I2CInit( 0, 0x12 );
   I2CInit( 0, I2C_FREQ_DIV );	 //  from 147456000 clock.
   I2CStop();

   OSTimeDly( 1 );
   I2CStart( 0xDE, g_nI2CTimeoutInPits );
   I2CSend( 0x00, g_nI2CTimeoutInPits );
   I2CSend( 0x3F, g_nI2CTimeoutInPits );
   I2CSend( 0x02, g_nI2CTimeoutInPits ); /* Set SR reg to 02 */
   I2CStop();
   OSTimeDly( 1 );

   I2CStart( 0xDE, g_nI2CTimeoutInPits );
   I2CSend( 0x00, g_nI2CTimeoutInPits );
   I2CSend( 0x3F, g_nI2CTimeoutInPits );
   I2CSend( 0x06, g_nI2CTimeoutInPits ); /* Set SR Reg to 06 */
   I2CStop();
   OSTimeDly( 1 );

   I2CStart( 0xDE, g_nI2CTimeoutInPits );
   I2CSend( 0x00, g_nI2CTimeoutInPits );
   I2CSend( 0x30, g_nI2CTimeoutInPits );
   I2CSend( MakeBCD( bts.tm_sec ), g_nI2CTimeoutInPits ); /* Seconds */
   I2CSend( MakeBCD( bts.tm_min ), g_nI2CTimeoutInPits ); /* Minutes */
   I2CSend( MakeBCD( bts.tm_hour ) + 0x80, g_nI2CTimeoutInPits ); /* Hour */
   I2CSend( MakeBCD( bts.tm_mday ), g_nI2CTimeoutInPits ); /* Day  */
   I2CSend( MakeBCD( bts.tm_mon  ), g_nI2CTimeoutInPits ); /* Month */
   I2CSend( MakeBCD( bts.tm_year % 100 ), g_nI2CTimeoutInPits ); /* Year */
   I2CSend( MakeBCD( bts.tm_wday ), g_nI2CTimeoutInPits ); /* Day of week */
   I2CSend( 0x20, g_nI2CTimeoutInPits ); /* Y2k */
   I2CStop();

   return 0;
}


int PCF8563RTCSetTime( struct tm &bts )
{
//mmk   I2CInit( 0, 0x12 );
   I2CInit( 0, I2C_FREQ_DIV );	 //  from 147456000 clock.
   I2CStop();

   OSTimeDly( 1 );
//   I2CStart( 0xDE, g_nI2CTimeoutInPits );
//   I2CSend( 0x00, g_nI2CTimeoutInPits );
//   I2CSend( 0x3F, g_nI2CTimeoutInPits );
//   I2CSend( 0x02, g_nI2CTimeoutInPits ); /* Set SR reg to 02 */

// Control/Status 1 (00H) -> 00 (Test1=0, Stop=0, TestC=0).
/*   I2CStart( 0xA2, g_nI2CTimeoutInPits );
   I2CSend( 0x00, g_nI2CTimeoutInPits );	//Address 00 i.e. Control-Status 1
   I2CSend( 0x00, g_nI2CTimeoutInPits ); // Set Control-Status 1 to 00
   I2CStop();
   OSTimeDly( 1 );
   */

//   I2CStart( 0xDE, g_nI2CTimeoutInPits );
//   I2CSend( 0x00, g_nI2CTimeoutInPits );
 //  I2CSend( 0x3F, g_nI2CTimeoutInPits );
//   I2CSend( 0x06, g_nI2CTimeoutInPits ); /* Set SR Reg to 06 */

 // Control/Status 2(01H) -> 00 (T1/TP=1, ints disabled, clear flags).
/*   I2CStart( 0xA2, g_nI2CTimeoutInPits );
   I2CSend( 0x01, g_nI2CTimeoutInPits );	//Address 01 i.e. Control-Status 2
   I2CSend( 0x10, g_nI2CTimeoutInPits ); // Set Control-Status 2 to 10
   I2CStop();
   OSTimeDly( 1 );
*/

// Disable all time alarms - register 09 - 0C
/*     I2CStart( 0xA2, g_nI2CTimeoutInPits );
     I2CSend( 0x09, g_nI2CTimeoutInPits );	//Address 09 i.e. Minute alarm address
     I2CSend( 0x80, g_nI2CTimeoutInPits ); // setting b7 disables alarm. - Minute
     I2CSend( 0x80, g_nI2CTimeoutInPits ); // setting b7 disables alarm. - Hour
     I2CSend( 0x80, g_nI2CTimeoutInPits ); // setting b7 disables alarm. - Day
     I2CSend( 0x80, g_nI2CTimeoutInPits ); // setting b7 disables alarm. - Weekday

     I2CStop();
     OSTimeDly( 1 );
*/
// Disable CLKOUT - register 0D
/*	  I2CStart( 0xA2, g_nI2CTimeoutInPits );
	  I2CSend( 0x0D, g_nI2CTimeoutInPits );	//Address 0D i.e.
	  I2CSend( 0x00, g_nI2CTimeoutInPits ); //  b7 = 0, inhibit clkout.

	  I2CStop();
	  OSTimeDly( 1 );
*/
// Setup timer for 1Hz int - register 0E - 0F
/*
   I2CStart( 0xA2, g_nI2CTimeoutInPits );
   I2CSend( 0x0E, g_nI2CTimeoutInPits );	//Address 0e i.e. Timer control address
   I2CSend( 0x81, g_nI2CTimeoutInPits ); 	// TE=1, TD1-TD0 = 01. (64Hz)
   I2CSend( 0x40, g_nI2CTimeoutInPits ); 	// Timer countdown value(n) = 64 (40H)

   I2CStop();
   OSTimeDly( 1 );
*/
// Now set time.
//   I2CStart( 0xDE, g_nI2CTimeoutInPits );
//   I2CSend( 0x00, g_nI2CTimeoutInPits );
//   I2CSend( 0x30, g_nI2CTimeoutInPits );
//   I2CSend( MakeBCD( bts.tm_sec ), g_nI2CTimeoutInPits ); /* Seconds */
//   I2CSend( MakeBCD( bts.tm_min ), g_nI2CTimeoutInPits ); /* Minutes */
//   I2CSend( MakeBCD( bts.tm_hour ) + 0x80, g_nI2CTimeoutInPits ); /* Hour */
//   I2CSend( MakeBCD( bts.tm_mday ), g_nI2CTimeoutInPits ); /* Day  */
//   I2CSend( MakeBCD( bts.tm_mon  ), g_nI2CTimeoutInPits ); /* Month */
//   I2CSend( MakeBCD( bts.tm_year % 100 ), g_nI2CTimeoutInPits ); /* Year */
//   I2CSend( MakeBCD( bts.tm_wday ), g_nI2CTimeoutInPits ); /* Day of week */
//   I2CSend( 0x20, g_nI2CTimeoutInPits ); /* Y2k */
 //  I2CStop();
//
   I2CStart( 0xA2, g_nI2CTimeoutInPits );
   I2CSend( 0x02, g_nI2CTimeoutInPits );	// 02H = seconds address
   I2CSend( MakeBCD( bts.tm_sec ), g_nI2CTimeoutInPits ); // 02H Seconds
   I2CSend( MakeBCD( bts.tm_min ), g_nI2CTimeoutInPits ); // 03H Minutes
   I2CSend( MakeBCD( bts.tm_hour ), g_nI2CTimeoutInPits ); // 04H Hour 0-23
   I2CSend( MakeBCD( bts.tm_mday ), g_nI2CTimeoutInPits ); // 05H Day
   //nb - register order different to x1226 here.
   I2CSend( MakeBCD( bts.tm_wday ), g_nI2CTimeoutInPits ); // 06H Day of week
   I2CSend( MakeBCD( bts.tm_mon  ), g_nI2CTimeoutInPits ); // 07H Month
   I2CSend( MakeBCD( bts.tm_year % 100 ), g_nI2CTimeoutInPits ); // 08H Year
   I2CSend( 0x20, g_nI2CTimeoutInPits ); /* Y2k */
   I2CStop();

   return 0;
}


int EnableRTCInterrupt( void )
{
   OSTimeDly( 1 );
   I2CStart( 0xDE, g_nI2CTimeoutInPits );
   I2CSend( 0x00, g_nI2CTimeoutInPits );
   I2CSend( 0x3F, g_nI2CTimeoutInPits );
   I2CSend( 0x02, g_nI2CTimeoutInPits ); /* Set SR reg to 02 */
   I2CStop();
   OSTimeDly( 1 );

   I2CStart( 0xDE, g_nI2CTimeoutInPits );
   I2CSend( 0x00, g_nI2CTimeoutInPits );
   I2CSend( 0x3F, g_nI2CTimeoutInPits );
   I2CSend( 0x06, g_nI2CTimeoutInPits ); /* Set SR Reg to 06 */
   I2CStop();
   OSTimeDly( 1 );
//
   I2CStart( 0xDE, g_nI2CTimeoutInPits );
   I2CSend( 0x00, g_nI2CTimeoutInPits );
   I2CSend( 0x11, g_nI2CTimeoutInPits );
   I2CSend( 0x18, g_nI2CTimeoutInPits ); /* Set INT Reg to 0x18 - 1Hz o-p */
   I2CStop();
   OSTimeDly( 1 );

   I2CStart( 0xDE, g_nI2CTimeoutInPits );
   I2CSend( 0x00, g_nI2CTimeoutInPits );
   I2CSend( 0x3F, g_nI2CTimeoutInPits );
   I2CSend( 0x00, g_nI2CTimeoutInPits ); /* Set SR Reg to 00 */
   I2CStop();
   OSTimeDly( 1 );


   return 0;
}

int InitialisePCF8563( void )
{
	   I2CInit( 0, I2C_FREQ_DIV );	 //  from 147456000 clock.
	   I2CStop();

	   OSTimeDly( 1 );
	// Control/Status 1 (00H) -> 00 (Test1=0, Stop=0, TestC=0).
	   I2CStart( 0xA2, g_nI2CTimeoutInPits );
	   I2CSend( 0x00, g_nI2CTimeoutInPits );	//Address 00 i.e. Control-Status 1
	   I2CSend( 0x00, g_nI2CTimeoutInPits ); // Set Control-Status 1 to 00
	   I2CStop();

   OSTimeDly( 1 );

   // Control/Status 2(01H) -> 00 (T1/TP=1, AE disabled, TIE enabled clear flags).
	 I2CStart( 0xA2, g_nI2CTimeoutInPits );
	 I2CSend( 0x01, g_nI2CTimeoutInPits );	//Address 01 i.e. Control-Status 2
	 I2CSend( 0x11, g_nI2CTimeoutInPits ); // Set Control-Status 2 to 10
	 I2CStop();
	 OSTimeDly( 1 );

	// Disable all time alarms - register 09 - 0C
	   I2CStart( 0xA2, g_nI2CTimeoutInPits );
	   I2CSend( 0x09, g_nI2CTimeoutInPits );	//Address 09 i.e. Minute alarm address
	   I2CSend( 0x80, g_nI2CTimeoutInPits ); // setting b7 disables alarm. - Minute
	   I2CSend( 0x80, g_nI2CTimeoutInPits ); // setting b7 disables alarm. - Hour
	   I2CSend( 0x80, g_nI2CTimeoutInPits ); // setting b7 disables alarm. - Day
	   I2CSend( 0x80, g_nI2CTimeoutInPits ); // setting b7 disables alarm. - Weekday

	   I2CStop();
	   OSTimeDly( 1 );

	// Setup timer for 1Hz int - register 0E - 0F
	  I2CStart( 0xA2, g_nI2CTimeoutInPits );
	  I2CSend( 0x0E, g_nI2CTimeoutInPits );	//Address 0e i.e. Timer control address
	  I2CSend( 0x81, g_nI2CTimeoutInPits ); 	// TE=1, TD1-TD0 = 01. (64Hz)
	  I2CSend( 0x40, g_nI2CTimeoutInPits ); 	// Timer countdown value(n) = 64 (40H)
	  //test to see if get 32Hz Test succeeded got expected waveform.
	  //I2CSend( 0x02, g_nI2CTimeoutInPits ); 	// Timer countdown value(n) = 2 (

	  I2CStop();
	  OSTimeDly( 1 );


   return 0;
}

bool DetectRTCPCF8563( void )
{
	bool bPCF8563Fitted = FALSE;

	   BYTE Read_buffer[4];
	   g_nI2CTimeoutInPits = ((I2CTIMEOUT_MS * PIT_FREQUENCY) / 1000);  //Timeout time in PITs, calculated from PIT freq and ms timeout.

	   // initialise the time data to some valid values, in case RTC isnt running.
       bts.tm_sec =  55;		// clear VL bit (todo, need to check for valid)
       bts.tm_min =  44;
       bts.tm_hour = 11;
       bts.tm_mday = 22;
       bts.tm_wday = 1;

       bts.tm_mon = 11;
       bts.tm_year = 11;      //mmk  + 100;


	   I2CInit( 0, I2C_FREQ_DIV );	 //  from 147456000 clock.
	   I2CStop();

	   OSTimeDly( 1 );

	// Day Alarm (0BH) -> 19
	   I2CStart( 0xA2, g_nI2CTimeoutInPits );
	   I2CSend( 0x0B, g_nI2CTimeoutInPits );	//Address 0B
	   I2CSend( (0x19 | 0x80), g_nI2CTimeoutInPits ); // Set day alarm to 19(bcd) ored with 0x80(alarm disabled)
	   I2CStop();

	   OSTimeDly( 1 );

	   I2CStart( 0xA2, g_nI2CTimeoutInPits );

	   I2CSend( 0x0B, g_nI2CTimeoutInPits );		// Command byte - input register 0

	   I2CRestart( 0xA3, g_nI2CTimeoutInPits );

	   I2CRead( Read_buffer, 1, g_nI2CTimeoutInPits );
	   I2CStop();
	   OSTimeDly( 1 );

	   g_nPCF8563Reg0B = Read_buffer[0];

	   if( (Read_buffer[0] & 0x7F) == 0x19 )
		   bPCF8563Fitted = TRUE;

	return( bPCF8563Fitted);
}

bool DetectX1226( void )
{
	bool bX1226Fitted = FALSE;

	   BYTE Read_buffer[4] = {7,8,9,5};
	   BYTE nReg36Initial = 2;
	   BYTE nReg36Final = 1;		// initialise to 2 different values.

	   g_nI2CTimeoutInPits = ((I2CTIMEOUT_MS * PIT_FREQUENCY) / 1000);  //Timeout time in PITs, calculated from PIT freq and ms timeout.

	   // initialise the time data to some valid values, in case RTC isnt running.
       bts.tm_sec =  55;		// clear VL bit (todo, need to check for valid)
       bts.tm_min =  44;
       bts.tm_hour = 11;
       bts.tm_mday = 22;
       bts.tm_wday = 1;

       bts.tm_mon = 11;
       bts.tm_year = 11;      //mmk  + 100;


	   I2CInit( 0, I2C_FREQ_DIV );	 //  from 147456000 clock.
	   I2CStop();

	   OSTimeDly( 1 );

// First read the day of week - Register 36
	// Day of Week (0036h) - read first.
	   I2CStart( 0xDE, g_nI2CTimeoutInPits );
	   I2CSend( 0x00, g_nI2CTimeoutInPits );	//Address 0036
	   I2CSend( 0x36, g_nI2CTimeoutInPits );		// Command byte - input register 0

	   I2CRestart( 0xDF, g_nI2CTimeoutInPits );
	   I2CRead( Read_buffer, 1, g_nI2CTimeoutInPits );
	   I2CStop();
	   OSTimeDly( 1 );

	   nReg36Initial = Read_buffer[0];
	   if(nReg36Initial > 6)				//day of week, valid values 0-6
		   nReg36Initial = 5;

// write back day of week value to address 0036.
	   OSTimeDly( 1 );
	   I2CStart( 0xDE, g_nI2CTimeoutInPits );
	   I2CSend( 0x00, g_nI2CTimeoutInPits );		//
	   I2CSend( 0x36, g_nI2CTimeoutInPits );		// send address 0036
	   I2CSend( nReg36Initial, g_nI2CTimeoutInPits );		// send data, i.e. initial value
	   I2CStop();
	   OSTimeDly( 1 );


// read back the day of week - Register 36
	   I2CStart( 0xDE, g_nI2CTimeoutInPits );
	   I2CSend( 0x00, g_nI2CTimeoutInPits );
	   I2CSend( 0x36, g_nI2CTimeoutInPits );
	   I2CRestart( 0xDF, g_nI2CTimeoutInPits );
	   I2CRead( Read_buffer, 1, g_nI2CTimeoutInPits );
	   I2CStop();
	   OSTimeDly( 1 );

	   nReg36Final = Read_buffer[0];

	   if( nReg36Final == nReg36Initial )
		   bX1226Fitted = TRUE;
	   //for testing
	   g_nReg36Initial = nReg36Initial;
	   g_nReg36Final = nReg36Final;

	return( bX1226Fitted );
}
/* to call
 BYTE PCA9555InputReg[2];
 BYTE *ptrPCA9555InputReg = PCA9555InputReg;
 ReadPCA9555( ptrPCA9555InputReg );
*/

void InitialisePCA9555( void)
{
   I2CStart( 0x4E, g_nI2CTimeoutInPits );
   I2CSend( 0x04, g_nI2CTimeoutInPits );		// Command byte - inversion register 0
   I2CSend( 0x00, g_nI2CTimeoutInPits );		// write 00's to inversion reg 0
   I2CSend( 0x00, g_nI2CTimeoutInPits );		// and inv. reg 1 (0=> no inversion)
   I2CStop();
   OSTimeDly( 1 );

   I2CStart( 0x4E, g_nI2CTimeoutInPits );
   I2CSend( 0x06, g_nI2CTimeoutInPits );		// Command byte - direction register 0
   I2CSend( 0xFF, g_nI2CTimeoutInPits );		// write 1's to config reg 0 - all i/p's
   I2CSend( 0xFF, g_nI2CTimeoutInPits );		// write 1's to config reg 1 - all i/p's
   I2CStop();
   OSTimeDly( 1 );

}


int * ReadPCA9555( BYTE *PCA9555Data )
{
   BYTE Read_buffer[2];
   I2CStart( 0x4E, g_nI2CTimeoutInPits );
   I2CSend( 0x00, g_nI2CTimeoutInPits );		// Command byte - input register 0

   I2CRestart( 0x4F, g_nI2CTimeoutInPits );
   I2CRead( Read_buffer, 2, g_nI2CTimeoutInPits );  // read 2 bytes.
   I2CStop();

   *PCA9555Data = Read_buffer[0];
	PCA9555Data++;
   *PCA9555Data = Read_buffer[1];

   return 0;
}
// WriteLTC2626DAC - writes to LTC2626 i2c D/A chip.
// LTC2626 is 12 bit DAC. - therefore nDACValue must be <= 4095.
void WriteLTC2626DAC( WORD nDACValue )
{
 	BYTE nTemp;

	if(nDACValue > 4095)
		nDACValue = 4095;			// 0x0FFF.

   I2CStart( 0x30, g_nI2CTimeoutInPits );  	// slave address (CA2=0, CA1=1, CA0=1)

   I2CSend( 0x30, g_nI2CTimeoutInPits );		// Command byte - write & update DAC.

	nTemp = nDACValue >> 4;
   I2CSend( nTemp, g_nI2CTimeoutInPits );		// 8 m.s.bits of DAC value

  	nTemp = ((nDACValue << 4) & 0xF0);	 		// 4 l.s. bits of DAC value, 4 l.s. bits =0000.
   I2CSend( nTemp, g_nI2CTimeoutInPits );		//

   I2CStop();

}

// WriteMAX5812DAC - writes to MAX5812 i2c D/A chip.
// MAX5812 is 12 bit DAC. - therefore nDACValue must be <= 4095.
void WriteMAX5812DAC( WORD nDACValue )
{
  BYTE nTemp;

 if(nDACValue > 4095)
  nDACValue = 4095;   // 0x0FFF.
// power up device.
   I2CStart( 0x20, g_nI2CTimeoutInPits );   // slave address (ADD = 0)
   I2CSend( 0x40, g_nI2CTimeoutInPits );  // Command byte - power up DAC.

   I2CStop();

// load D/A value
   I2CStart( 0x20, g_nI2CTimeoutInPits );   // slave address (ADD = 0, RW = 0)
   I2CSend( 0x40, g_nI2CTimeoutInPits );  // Command byte - power up DAC.

 nTemp = (nDACValue >> 8);      // get 4 m.s.bits of DAC value
 nTemp &= 0x0F;
 nTemp |= 0xC0;               // or in 0xc0 - load and update command.
   I2CSend( nTemp, g_nI2CTimeoutInPits );
// testonly
//   iprintf("M.S. byte = %2x ", nTemp&0x0F);
   nTemp = (nDACValue & 0x00FF);    // 8 l.s. bits of DAC value
   I2CSend( nTemp, g_nI2CTimeoutInPits );  //

//   iprintf("L.S. byte = %2x ", nTemp);

   I2CStop();

}



//////////////////////////////////////////////////////
// InitTempSensor( void )
// Function to initialise the temperature sensor TMP100.
// Configured for 9 bit resolution, i.e. 0.5C
// First Pass - 17.9.2008
//////////////////////////////////////////////////////
void InitTempSensor( void )
{
   BYTE nAddr = TMP100_I2C_ADDR;
   BYTE nPtr;
   BYTE nConfigData,nData;

   I2CInit( 0, I2C_FREQ_DIV );	 //  from 147456000 clock.
   I2CStop();

   OSTimeDly( 1 );
   I2CStart( nAddr, g_nI2CTimeoutInPits );

   nPtr = TMP100_CONFIG_REGISTER;
   I2CSend( nPtr, g_nI2CTimeoutInPits );

   nConfigData = 0x00;     // SD=0, TM=0, POL=0, Faults = 1, Resolution = 9 bits, OS= 0.
   I2CSend( nConfigData, g_nI2CTimeoutInPits );

// ToDo not sure what to send for 2nd data byte...
   nData = 0x00;
   I2CSend( nData, g_nI2CTimeoutInPits );

   I2CStop();
   OSTimeDly( 1 );

}

//////////////////////////////////////////////////////
// ReadTempSensor( void )
// Function to read the temperature sensor.
// Temperature is read at 9 bit resolution, i.e. 0.5C, but only return integer result.
// First Pass - 17.9.2008
//////////////////////////////////////////////////////
WORD ReadTempSensor(void)
{
   BYTE nAddr = TMP100_I2C_ADDR;
   BYTE nPtr;
   BYTE Read_Buf[2];
   WORD nTemperature;

   I2CStart( nAddr, g_nI2CTimeoutInPits );

   nPtr = TMP100_TEMP_REGISTER;
   I2CSend( nPtr, g_nI2CTimeoutInPits );

   I2CRestart( (nAddr | 0x01), g_nI2CTimeoutInPits ); // R/W = 1 for read.


   I2CRead( Read_Buf, 2, g_nI2CTimeoutInPits );    // read 2 bytes.

   I2CStop();
//testonly.
//   g_nTemperature0 = Read_Buf[0];
//   g_nTemperature1 = Read_Buf[1];

//   nTemperature = (( Read_Buf[0] << 1) | Read_Buf[1] >>7 );   (for 0.5 degree resolution)
   nTemperature = Read_Buf[0];   // integer resolution only (degrees C)
   return(nTemperature);
}


/*
int IOBoardRTCSetSystemFromRTCTime()
{
   struct tm bts;
   int rv = IOBoardRTCGetTime( bts );
   if ( rv )
   {
      return rv;
   }
   time_t t = mktime( &bts );
   set_time( t );
   return 0;
}


int IOBoardRTCSetRTCfromSystemTime()
{
   time_t t = time( NULL );
   struct tm *ptm = gmtime( &t );
   return IOBoardRTCSetTime( *ptm );
}

*/

