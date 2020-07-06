
//////////////////////////////////////////////////////
// P.Smith                      15/1/08
// implement CheckForValidHardware & ShouldSHABeChecked functions
// the serial no is checked for one of the nbbs in the list of nbbs shipped
// prior to putting in the updated sha chip.
//
// P.Smith                      16/1/08
// print result of SHA test
//
// P.Smith                      3/6/09
// repeat the sha function test 10 times if it has not passed the 1st time.
//
// P.Smith                      24/6/09
// if the SHA check fails, then set g_nSHASecretFailCtr to 1.
// this counter will eventually crash the software after a period
// of time. the blender will not cycle either.
//
// P.Smith                          25/3/10
// shorten printfs to hide what is being initialised

// M.McKiernan						29.6.2020
// In void CheckForValidHardware( void ) force it to skip the SHA test.
//{
//    int i;
//    BOOL bSHATestOkay = FALSE;
//    //if(ShouldSHABeChecked())
 //   if( FALSE )						// force it to ske the check.
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

#include <basictypes.h>
#include <string.h>
#include "NBBgpio.h"
#include "ConversionFactors.h"
#include "MultiblendMB.h"
#include "Mtttymux.h"
#include "Secret.h"
#include "OneWire.h"

extern CalDataStruct    g_CalibrationData;
extern  BYTE DS2432Buffer[];
extern WORD g_nSHASecretFailCtr;



//////////////////////////////////////////////////////
// CheckForValidHardware( void )
// If the nbb is one of the previous batches of nbbs, then the check on
// the sha is not run, if the SHA test is unsucessful, then the a hardware
// license alarm is inititated.
//
//////////////////////////////////////////////////////
int g_nSHATestMMK = 0;

void CheckForValidHardware( void )
{
    int i;
    BOOL bSHATestOkay = FALSE;
#if( 0 )
    //if(ShouldSHABeChecked())
    if( 0 )						// force it to ske the check.
    {
        for(i=0; (i < 10) && (!bSHATestOkay); i++)
        {
            if( TestSHAFunction() )
            {
                bSHATestOkay = TRUE;
            }
        }

        if(bSHATestOkay)
        {
            //SetupMttty();
            //iprintf("\n Memory written correctly - SHA working");
            g_bSHAHardwareCheckFailed = FALSE;
            if(g_arrnMBTable[BATCH_SUMMARY_SYSTEM_ALARMS] & HARDWARELICENSEALARMBITPOS) // bit set?
            {
                RemoveAlarmTable( HARDWARELICENSEALARM,  0 );     // RMALTB
            }

        }
        else
        {
            //SetupMttty();
            //iprintf("\n Memory write incorrect - SHA NOT working");
            g_bSHAHardwareCheckFailed = TRUE;
            if( (g_arrnMBTable[BATCH_SUMMARY_SYSTEM_ALARMS] & HARDWARELICENSEALARMBITPOS) == 0 ) // license bit not set???
            {
                PutAlarmTable( HARDWARELICENSEALARM, 0 );     // indicate alarm,  //PTALTB
            }
            g_nSHASecretFailCtr = 1; //security--tsm--
        }
        g_nSHATestMMK = 33;
    }
    else
#endif

    {
//            SetupMttty();
//            iprintf("\n sha should not be checked");
        g_bSHAHardwareCheckFailed = FALSE;
        if(g_arrnMBTable[BATCH_SUMMARY_SYSTEM_ALARMS] & HARDWARELICENSEALARMBITPOS) // bit set?
        {
            RemoveAlarmTable( HARDWARELICENSEALARM,  0 );     // RMALTB
        }

        g_nSHATestMMK = 7;
    }
    g_nSHATestMMK++;
}
//////////////////////////////////////////////////////
// ShouldSHABeChecked( void )
// The updated sha chip on the 1st 2 batches of nbbs were not
// the correct revision.
// the software now checkks for any of those initial nbbs
// if it is in the list, then the sha check if not initiated
//
//////////////////////////////////////////////////////
BOOL ShouldSHABeChecked( void )
{
    int i,j;
    BOOL bCheckSha = TRUE;
    BOOL bFound;
    for(i=0; i<NOSHACHECKNO; i++)
    {
        bFound = TRUE;
        for(j=0; j<HARDWARESERIALNO; j++)
        {
            if(DS2432Buffer[j] != NoShaCheckList[i][j])
            {
                bFound = FALSE;
            }
        }
        if(bFound)
        {
//            SetupMttty();
//            iprintf("\n found");
            bCheckSha = FALSE;
        }
    }
    return(bCheckSha);
}








