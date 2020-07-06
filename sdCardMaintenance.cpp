//////////////////////////////////////////////////////
//
// P.Smith                      1/7/08 
// sd card maintenance
//
// P.Smith                          23/7/08
// call MaintainSDCard, check if writable before maintenance
// correct year, must subtract century
//
// P.Smith                          1/4/9
// added g_bSPIBusBusy set for sd card maintenance
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
#include "Sdcard.h"
#include "SdCardMaintenance.h"

//////////////////////////////////////////////////////
// CheckForSDCardMaintainenance( void )           
// checks for sd card maintenance, if the month changes,
// then maintain the sd card files.
// This function is called every minute
//////////////////////////////////////////////////////
void CheckForSDCardMaintainenance( void )
{
    if(g_CurrentTime[TIME_MONTH] != g_cCurrentMonthHistory)   // change in the month  ?
    {
       g_cCurrentMonthHistory = g_CurrentTime[TIME_MONTH];  // update historic value
       if(SDCardWritable())
       {
           MaintainSDCard(g_CurrentTime[TIME_MONTH]);
       }
    }
}



//////////////////////////////////////////////////////
// MaintainSDCard( void )           
// maintains files on sd card
// This function deletes files that are not needed
// two full months plus the current month of logs and events are kept
// while the others are deleted.
// example if the current months is July (07), months 1 - 4 and 8 - 12 are deleted
// if the month is January (01), then months 2 - 10 are deleted.
// a directory of the files concerned are first obtained and they are then deleted
// the maintenance is done at the end of every day when the date changes
//
//////////////////////////////////////////////////////

void MaintainSDCard( int nCurrentMonth )
{
    g_bSPIBusBusy = TRUE;
    InitialiseSDCard();         // initialise sd card
    if(nCurrentMonth -NOOFFILESTOKEEP <= 0)
    {
        MaintainEventLog(nCurrentMonth+1,nCurrentMonth - NOOFFILESTOKEEP + 12);
        MaintainBatchLog(nCurrentMonth+1,nCurrentMonth - NOOFFILESTOKEEP + 12);
    }
    else
    {
        MaintainEventLog(nCurrentMonth+1,12);
        MaintainEventLog(1,nCurrentMonth-NOOFFILESTOKEEP);

        MaintainBatchLog(nCurrentMonth+1,12);
        MaintainBatchLog(1,nCurrentMonth-NOOFFILESTOKEEP);
    }
    f_delvolume(MMC_DRV_NUM);
    ReInitialisePinsUsedBySDCard();       
    
    g_bSPIBusBusy = FALSE;
}

        

//////////////////////////////////////////////////////
// MaintainEventLog( void )           
// maintains the event log from months nStartMonth to nEndMonth
// firstly the directory for the files for the month is generated
// and the files in that directory are then deleted.
//////////////////////////////////////////////////////
void MaintainEventLog( int nStartMonth,int nEndMonth )
{
        char    cName[20];
        int i;
// get directory first of all
    for(i=nStartMonth; i<= nEndMonth; i++)
    {
        sprintf( cName, "Evnt%02d%02d", g_CurrentTime[TIME_YEAR]-TIME_CENTURY,i);     // 
        strcat( cName, ".*");
        GetDirectoryandDeleteFiles(cName);
    }
}

//////////////////////////////////////////////////////
// MaintainBatchLog( void )           
// maintains the batch logs from months nStartMonth to nEndMonth
// firstly the directory for the files for the month is generated
// and the files in that directory are then deleted.
//////////////////////////////////////////////////////
void MaintainBatchLog( int nStartMonth,int nEndMonth )
{
        char    cName[20];
        int i;

    for(i=nStartMonth; i<= nEndMonth; i++)
    {
        sprintf( cName, "lg%02d%02d", g_CurrentTime[TIME_YEAR]-TIME_CENTURY,i);     // 
        strcat( cName, "*.*");
        GetDirectoryandDeleteFiles(cName);
    }
}

//////////////////////////////////////////////////////
// GetDirectoryandDeleteFiles( void )           
// gets the directory and deletes the files in that directory.
//
//////////////////////////////////////////////////////

void GetDirectoryandDeleteFiles( char *cName )
{

    F_FIND finder;        
    unsigned int i,j,rc;
    g_bSPIBusBusy = TRUE;
    rc=f_findfirst(cName,&finder);  // find 1st file in directory
    if ( rc == F_NO_ERROR )  // found a file or directory
    {
       i = 0;
       g_nSdCardFileDetails[i] = finder;
       i++;
       while (!f_findnext(&finder) && i < MAX_SD_CARD_FILES)   
        {
            g_nSdCardFileDetails[i] = finder;                // copy file details
            i++;
        } 
        // now delete the files found that are no longer required.
        j = i;
        for(i=0; i< j; i++)
        {
            f_delete(g_nSdCardFileDetails[i].filename);  // delete the file
        }

    }
    else
    {
//        iprintf("\n error is %d",rc);
    }
}




