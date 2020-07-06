
//////////////////////////////////////////////////////
// P.Smith                      29/10/08
// watchdog attempt pass 1
// there is some issue in the netburner software not allowing the watch dog timer
// to be switched on, the only place that it can be switched on from now is the
// monitor program
// functions to enable,disable and service watch dog timer.
//
// P.Smith                      26/11/08
// or in the watch dog enable bit.
// bring time up to 65535 - 5 seconds

// M.McKiernan					3.3.2010
// Added functions EnableWatchdogNoReboot, EnableWatchdogAndReboot, DisableWatchdogNoReboot, DisableWatchdogAndReboot
// GetWatchdogStatus, InitialiseWatchdog
//
// P.Smith                      9/3/10
// disable watch dog for now.
//
// P.Smith                      16/3/10
// re enable watch dog for now.
// a problem has been seen where the processor is crashing.
//
// P.Smith                      16/3/10
// if address switch is 0, disable watch dog.
// it will now be switch on /off from the panel
//
// P.Smith                      19/3/10
// put back in watch dog as it was before, enabled if non zero and disabled if
// address switch is zero.
//
// P.Smith                      25/3/10
// revert to enable / disable of watchdog from the address switch
//
// P.Smith                          25/3/10
// shorten printfs to hide what is being initialised
//
// P.Smith                      6/4/10
// remove enable / disable of watchdog from the address switch
//////////////////////////////////////////////////////




#include <stdio.h>

#include "SerialStuff.h"
#include "BatchMBIndices.h"
#include "BatchCalibrationFunctions.h"
#include "General.h"
#include "ConversionFactors.h"

#include "16R40C.h"
#include "ConfigurationFunctions.h"
#include "MBMHand.h"
#include "MBSHand.h"

#include "MBProgs.h"
#include "SetpointFormat.h"
#include "TimeDate.h"
#include "BatVars.h"
#include "BBCalc.h"
#include "MBTogSta.h"
#include "Alarms.h"
#include "KghCalc.h"
#include "BatVars2.h"
#include "Monbat.h"
#include "Blrep.h"
#include "MBProsum.h"
#include "Pause.h"
#include "CycleMonitor.h"
#include "TSMPeripheralsMBIndices.h"
#include "BSP.h"
#include <System.h>


bool g_bWatchdogStatus;		//ENABLED or DISABLED


/******************************************************************
 *  Enable the Watchdog timer.
 ******************************************************************/
void Enable_watchdog( void )
{
//sim.wtm.wmr = 18000;  // approx 2S Timeout.
    sim.wtm.wmr = 65535;  // approx 4S Timeout.
    sim.wtm.wcr = 0x000F; //

    ConfigRecord* newCR=(ConfigRecord*)malloc(sizeof(ConfigRecord));
    memcpy(newCR,&gConfigRec,sizeof(ConfigRecord));
//    SetupMttty();
//    iprintf("value is %d",newCR->m_bUnused[2]);
    newCR->m_bUnused[2] |= 0x01;
    UpdateConfigRecord (newCR);
    free (newCR);

}

/******************************************************************
 *  Disable the Watchdog timer.
 ******************************************************************/
void Disable_watchdog( void )
{
   sim.wtm.wcr = 0x0000;
}

/******************************************************************
 *  Kick the Watchdog timer.
 ******************************************************************/
void Service_Watchdog( void )
{
   sim.wtm.wsr = 0x5555;
   sim.wtm.wsr = 0xAAAA;
}


void EnableWatchdogNoReboot( void )
{
 ConfigRecord new_rec;
	memcpy( &new_rec, &gConfigRec, sizeof( new_rec ) );
	if ( (new_rec.m_Flags & 0x01) == 0) // if watchdog disabled
	{
		new_rec.m_Flags |= 0x01; // WD enable
		UpdateConfigRecord( &new_rec );
	}

}

void EnableWatchdogAndReboot( void )
{
 ConfigRecord new_rec;
	memcpy( &new_rec, &gConfigRec, sizeof( new_rec ) );
	if ( (new_rec.m_Flags & 0x01) == 0) // if watchdog disabled
	{
		new_rec.m_Flags |= 0x01; // WD enable
		UpdateConfigRecord( &new_rec );
		ForceReboot();
	}

}
void DisableWatchdogNoReboot( void )
{
 ConfigRecord new_rec;
	memcpy( &new_rec, &gConfigRec, sizeof( new_rec ) );
	if (new_rec.m_Flags & 0x01) // if watchdog enabled
	{
		new_rec.m_Flags &= 0xFE; // WD disable
		UpdateConfigRecord( &new_rec );
	}

}
void DisableWatchdogAndReboot( void )
{
 ConfigRecord new_rec;
	memcpy( &new_rec, &gConfigRec, sizeof( new_rec ) );
	if (new_rec.m_Flags & 0x01) // if watchdog enabled
	{
		new_rec.m_Flags &= 0xFE; // WD disable
		UpdateConfigRecord( &new_rec );
		ForceReboot();
	}

}
bool GetWatchdogStatus( void )
{
	 ConfigRecord new_rec;
		memcpy( &new_rec, &gConfigRec, sizeof( new_rec ) );
		if ( new_rec.m_Flags & 0x01 ) // if watchdog enabled
			return( ENABLED );
		else
			return( DISABLED );

}

// InitialiseWatchdog:
// 		reads current status of watchdog timer.
//		If NBB address (hex switch) is 0 always disable watchdog.
//		If NBB address non-zero, always enable watchdog.
//	Note: if going from enable to disable or vice versa, must reboot.
void InitialiseWatchdog( void )
{
 BYTE nNBBAddress;

 	Service_Watchdog();

    g_bWatchdogStatus = GetWatchdogStatus();

 	if(g_bWatchdogStatus == ENABLED )
 		iprintf("wE\n");
 	else
 		iprintf("wD\n");
 	Service_Watchdog();
 	nNBBAddress = ReadHexSwitch();
/*
 	if(nNBBAddress != 0)
 	 {
 	 		//iprintf("\nLink L3 is fit ted");
 	     //iprintf("\nNBB address is not 0, => enable Watchdog");
 	     if(g_bWatchdogStatus != ENABLED )
 	     EnableWatchdogAndReboot();	//L3 fitted, if not enabled,  enable wdog
 	 }
 	 else
 	 {
 	     //iprintf("\nLink L3 is not fittted");
 	 	 iprintf("\nNBB address = 0, => disable Watchdog");
 	     if(g_bWatchdogStatus == ENABLED )
 	 	 DisableWatchdogAndReboot();		//L3 not fitted, disable wdog
 	 }
*/


 	if(nNBBAddress == 0)
  	{
 		//iprintf("\nLink L3 is not fittted");
 		iprintf("\nNBB address = 0, => disable Watchdog");
        if(g_bWatchdogStatus == ENABLED )
 		DisableWatchdogAndReboot();		//L3 not fitted, disable wdog
 	}


}
