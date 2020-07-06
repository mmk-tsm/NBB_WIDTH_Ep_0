//////////////////////////////////////////////////////
// MBVacLdr.cpp
// P.Smith                          12/6/09
// first pass copy and load cf vacuum loader data
// CopyVacuumLoaderCalibrationParametersToMB, LoadVacuumLoaderCalibrationParametersFromMB
// CopyVacuumLoaderSummaryDataToMB
//
// M.McKiernan                   7/09/2009.
// Edited LoadVacuumLoaderCalibrationParametersFromMB() See all - new 7.Sept.2009
// manual loader command added for each loader.
// Renamed m_nDumpDelayTime -> m_nDischargeTime.
// Added sanity check on Discharge time & suction time(s) coming down on Modbus.
// Cater for unused loaders.
//
// P.Smith                      17/9/09
// added changes for vacuum loading also do a check for licensing option.
// if not set, set the no of loaders to 0
//
// P.Smith                      29/9/09
// check that loader cal data is valid.
//
// P.Smith                      5/3/10
// include license.h
//////////////////////////////////////////////////////



#include <stdio.h>
#include <basictypes.h>
#include "batvars.h"
#include "batvars2.h"

#include "VacVars.h"
#include "MBVacLdr.h"
#include "Hash.h"
#include "VacSeq.h"
#include "Vacldee.h"
#include "License.h"

//////////////////////////////////////////////////////
// CopyVacuumLoaderCalibrationParametersToMB
//
//
//////////////////////////////////////////////////////


extern   structVacCalib   g_VacCalibrationData;
extern   structVacLoaderData g_sLoader[];
extern  OnBoardCalDataStruct   g_OnBoardCalData;


void CopyVacuumLoaderCalibrationParametersToMB(void)
{
int i;
WORD nCompBlock;

// copy common data first to mb table
//
    g_arrnMBTable[VACUUM_LOADER_NUMBER_OF_LOADERS_RW] = (WORD)g_VacCalibrationData.m_nLoaders;
    g_arrnMBTable[VACUUM_LOADER_PUMP_IDLE_TIME_SECONDS_RW] = (WORD)g_VacCalibrationData.m_nPumpIdleTime;
    g_arrnMBTable[VACUUM_LOADER_PUMP_START_TIME_SECONDS_RW] = (WORD)g_VacCalibrationData.m_nPumpStartTime;
    g_arrnMBTable[VACUUM_LOADER_ATMOSPHERE_VALVE_OVERLAPTIME_RW] = (WORD)g_VacCalibrationData.m_nAVOverLapTime;
    g_arrnMBTable[VACUUM_LOADER_DUST_FILTER_REPETITIONS_RW] = (WORD)g_VacCalibrationData.m_nFilterRepetitions;
    g_arrnMBTable[VACUUM_LOADER_DUST_FILTER_TYPE_RW] = (WORD)g_VacCalibrationData.m_nFilterType;
    g_arrnMBTable[VACUUM_LOADER_DUMP_DELAY_SECONDS_RW] = (WORD)g_VacCalibrationData.m_nDischargeTime;

// copy individual loader data to modbus
   for(i=0;  i<g_VacCalibrationData.m_nLoaders; i++)
//   for(i=0;  i<MAX_LOADERS; i++)

   {
      nCompBlock = VACUUM_LOADER_BLOCK1 + (i*VACUUM_LOADER_BLOCK_SIZE);  //calculate data block
       // on /off status
      g_arrnMBTable[nCompBlock + VACUUM_LOADER_ON_OFF_OFFSET_RW] = g_VacCalibrationData.m_bOnFlag[i];
      // sunction time
      g_arrnMBTable[nCompBlock + VACUUM_LOADER_SUCTION_TIME_SECONDS_OFFSET_RW] = g_VacCalibrationData.m_nSuctionTime[i];

      // post fill
      g_arrnMBTable[nCompBlock + VACUUM_LOADER_POST_FILL_OFFSET_RW] = g_VacCalibrationData.m_nPostFillTime[i];
      // priority
      g_arrnMBTable[nCompBlock + VACUUM_LOADER_PRIORITY_OFFSET_RW] = g_VacCalibrationData.m_nPriority[i];

   }

}

//////////////////////////////////////////////////////
// LoadVacuumLoaderCalibrationParametersFromMB
//
//
//////////////////////////////////////////////////////
void LoadVacuumLoaderCalibrationParametersFromMB( void )
{
int i;
WORD nCompBlock;
BYTE nLoaders;
BYTE nStartTime;
BOOL bVacCalDataValid;
    if((g_OnBoardCalData.m_cPermanentOptions & LICENSE_VACUUMLOADING) == 0)
    {
        nLoaders = 0;
        g_arrnMBTable[VACUUM_LOADER_NUMBER_OF_LOADERS_RW] = 0;
    }
    else
    {
        nLoaders = g_arrnMBTable[VACUUM_LOADER_NUMBER_OF_LOADERS_RW];
    }
    if( nLoaders > MAX_LOADERS)
    {
      nLoaders = MAX_LOADERS;
      g_arrnMBTable[VACUUM_LOADER_NUMBER_OF_LOADERS_RW] = nLoaders;
    }
    g_VacCalibrationData.m_nLoaders = nLoaders;
    // hash
    g_VacCalibrationData.m_nLoaderHash = TSMHashOfByte( g_VacCalibrationData.m_nLoaders );

    g_VacCalibrationData.m_nPumpIdleTime = g_arrnMBTable[VACUUM_LOADER_PUMP_IDLE_TIME_SECONDS_RW];

    nStartTime = g_arrnMBTable[VACUUM_LOADER_PUMP_START_TIME_SECONDS_RW];
    if( nStartTime < PUMP_START_TIME_MIN )
      nStartTime = PUMP_START_TIME_MIN;
    else if( nStartTime > PUMP_START_TIME_MAX )
      nStartTime = PUMP_START_TIME_MAX;
    g_arrnMBTable[VACUUM_LOADER_PUMP_START_TIME_SECONDS_RW] = nStartTime;

    g_VacCalibrationData.m_nPumpStartTime = nStartTime;
    g_VacCalibrationData.m_nAVOverLapTime = g_arrnMBTable[VACUUM_LOADER_ATMOSPHERE_VALVE_OVERLAPTIME_RW];
    g_VacCalibrationData.m_nFilterRepetitions = g_arrnMBTable[VACUUM_LOADER_DUST_FILTER_REPETITIONS_RW];
    g_VacCalibrationData.m_nFilterType = g_arrnMBTable[VACUUM_LOADER_DUST_FILTER_TYPE_RW];
    g_VacCalibrationData.m_nDischargeTime = g_arrnMBTable[VACUUM_LOADER_DUMP_DELAY_SECONDS_RW];
    if(g_VacCalibrationData.m_nDischargeTime > MAX_DISCHARGE_TIME  || g_VacCalibrationData.m_nDischargeTime < MIN_DISCHARGE_TIME)
    {
        g_VacCalibrationData.m_nDischargeTime = DEFAULT_DISCHARGE_TIME;
        g_arrnMBTable[VACUUM_LOADER_DUMP_DELAY_SECONDS_RW] = g_VacCalibrationData.m_nDischargeTime;
   }

// new 7.Sept.2009 -
    if(g_arrnMBTable[VACUUM_LOADER_MODE_MANUAL] > 0)
    {
      if(g_bVacLoaderSystemInManual == FALSE)
      {
         g_bVacLoaderSystemInManual = TRUE;
         g_bVacLoaderSystemEnterManual = TRUE;     // flag enter to Manual.
      }
    }
    else  // i.e. must be a 0.
    {
      if(g_bVacLoaderSystemInManual == TRUE)
      {
         g_bVacLoaderSystemInManual = FALSE;
         g_bVacLoaderSystemExitManual = TRUE;      // flag exit from manual.
      }
    }
      if(g_arrnMBTable[VACUUM_LOADER_PUMP_COMMAND] == 0)
         g_bPumpManualCommand = OFF;      // use 0 to turn off.
      else
         g_bPumpManualCommand = ON;

      if(g_arrnMBTable[VACUUM_LOADER_ATMOSPHERE_VALVE_COMMAND] == 0)
         g_bAtmValveManualCommand = CLOSED;      // use 0 to turn off.
      else
         g_bAtmValveManualCommand = OPEN;

      if(g_arrnMBTable[VACUUM_LOADER_DUST_FILTER_COMMAND] == 0)
         g_bFilterManualCommand = OFF;      // use 0 to turn off.
      else
         g_bFilterManualCommand = ON;

// clear alarms command.
      if(g_arrnMBTable[VACUUM_LOADER_CLEAR_ALARMS_COMMAND] != 0 )
      {
         ClearAllLoaderAlarms();
         g_arrnMBTable[VACUUM_LOADER_CLEAR_ALARMS_COMMAND] = 0;      // clear for next pass.
      }

// endnew 7.Sept.2009 -

// copy individual loader data to modbus
   for(i=0;  i<g_VacCalibrationData.m_nLoaders; i++)
   {
      nCompBlock = VACUUM_LOADER_BLOCK1 + (i*VACUUM_LOADER_BLOCK_SIZE);  //calculate data block
       // on /off status

      if(g_arrnMBTable[nCompBlock + VACUUM_LOADER_ON_OFF_OFFSET_RW] !=0)
      {
          g_VacCalibrationData.m_bOnFlag[i] = ON;
      }
      else
      {
          g_VacCalibrationData.m_bOnFlag[i] = OFF;
      }

      // new 7.Sept.2009 - manual loader command - only relevant when system in MANUAL mode
      if(g_arrnMBTable[nCompBlock + VACUUM_LOADER_MANUAL_COMMAND_OFFSET] == 0 )
      {
         g_sLoader[i].m_bManualCommand = OFF;
      }
      else
      {
         g_sLoader[i].m_bManualCommand = ON;
      }
      // endnew 7.Sept.2009

      // suction time
      g_VacCalibrationData.m_nSuctionTime[i] = g_arrnMBTable[nCompBlock + VACUUM_LOADER_SUCTION_TIME_SECONDS_OFFSET_RW];
      if(g_VacCalibrationData.m_nSuctionTime[i] < MIN_SUCTION_TIME || g_VacCalibrationData.m_nSuctionTime[i] > MAX_SUCTION_TIME )
      {
         g_VacCalibrationData.m_nSuctionTime[i] = DEFAULT_SUCTION_TIME;
         g_arrnMBTable[nCompBlock + VACUUM_LOADER_SUCTION_TIME_SECONDS_OFFSET_RW] = DEFAULT_SUCTION_TIME;
      }
      // post fill
 //     g_sLoader[i].m_nFillTimer = g_VacCalibrationData.m_nSuctionTime[i];//nbb--todo--remove vac
      g_VacCalibrationData.m_nPostFillTime[i] = g_arrnMBTable[nCompBlock + VACUUM_LOADER_POST_FILL_OFFSET_RW];
      // priority
      BYTE nPriority = g_arrnMBTable[nCompBlock + VACUUM_LOADER_PRIORITY_OFFSET_RW];
      if(nPriority < 1 )
         nPriority = 1;
      else if(nPriority > MAX_LOADER_PRIORITY)
         nPriority = MAX_LOADER_PRIORITY;
      g_arrnMBTable[nCompBlock + VACUUM_LOADER_PRIORITY_OFFSET_RW] = nPriority;
      g_VacCalibrationData.m_nPriority[i] = nPriority;
   }
      // cater for unused(not physically present) loaders
   if( g_VacCalibrationData.m_nLoaders < MAX_LOADERS )
   {
      for(i=g_VacCalibrationData.m_nLoaders;  i<MAX_LOADERS; i++)
      {
            // on /off status -
          g_VacCalibrationData.m_bOnFlag[i] = OFF;       // must be off.
          g_VacCalibrationData.m_nSuctionTime[i] = DEFAULT_SUCTION_TIME;
          g_VacCalibrationData.m_nPriority[i] = 7;       // lowest priority.
          g_sLoader[i].m_nFillTimer = 0;
          g_sLoader[i].m_bFillValve = OFF;       // TURN off fill relay
          g_sLoader[i].m_nStatusByte = LOADER_STATUS_UNUSED;
          g_sLoader[i].m_bLevelFlag = EMPTY;
      }
   }

   bVacCalDataValid = CheckForValidLoaderConfigData();
   g_bSaveLoaderCalibrationData = TRUE;
}

//////////////////////////////////////////////////////
// CheckForVacuumLoaderCalibrationWrite
// called once per second from the foreground
//
//////////////////////////////////////////////////////
void CheckForCalibrationWrite( void )
{
    if(g_bVacuumLoaderCalibrationWritten)
    {
        g_bVacuumLoaderCalibrationWritten = FALSE;
        LoadVacuumLoaderCalibrationParametersFromMB();
    }
/*    if(fdTelnet>0)
    {
        iprintf("\n number of loaders %d",g_VacCalibrationData.m_nLoaders);
        iprintf("\n pump idle time %d",g_VacCalibrationData.m_nPumpIdleTime);
        iprintf("\n pump start time %d",g_VacCalibrationData.m_nPumpStartTime);
        iprintf("\n atm overlap %d",g_VacCalibrationData.m_nAVOverLapTime);
        iprintf("\n filter repetitions %d",g_VacCalibrationData.m_nFilterRepetitions);
        iprintf("\n filter type %d",g_VacCalibrationData.m_nFilterType);
        iprintf("\n dump delay %d",g_VacCalibrationData.m_nDischargeTime);
    }

       for(i=0;  i<g_VacCalibrationData.m_nLoaders; i++)
        {
            if(fdTelnet>0)
            {
                iprintf("\n loader %d on / off status %d",i+1,g_VacCalibrationData.m_bOnFlag[i]);
                iprintf("\n loader %d m_nSuctionTime %d",i+1,g_VacCalibrationData.m_nSuctionTime[i]);
                iprintf("\n loader %d m_nPostFillTime %d",i+1,g_VacCalibrationData.m_nPostFillTime[i]);
                iprintf("\n loader %d m_nPriority %d",i+1,g_VacCalibrationData.m_nPriority[i]);

                iprintf("\n loader %d m_nStatusByte %d",i+1,g_sLoader[i].m_nStatusByte);
                iprintf("\n loader %d m_nFillTimer %d",i+1,g_sLoader[i].m_nFillTimer);
                iprintf("\n loader %d m_bLevelFlag %d",i+1,g_sLoader[i].m_bLevelFlag);

            }
        }
*/

}

//////////////////////////////////////////////////////
// CopyVacuumLoaderSummaryDataToMB
// copy vacuum loader summary data to the modbus table
//
//////////////////////////////////////////////////////
void CopyVacuumLoaderSummaryDataToMB(void)
{
int i;
WORD nCompBlock;
   if(g_VacCalibrationData.m_nLoaders > 0)
   {
       // alarms
       g_arrnMBTable[VACUUM_LOADER_ALARMS_TOTAL] = g_nVacLoaderAlarmsTotal;
       // pump on /off status
       g_arrnMBTable[VACUUM_LOADER_PUMP_ON_OFF_RO] = g_bPumpStatus;
       // atm valve open / closed
       g_arrnMBTable[VACUUM_LOADER_ATMOSPHERE_VALVE_STATUS_RO] = g_bAtmValveStatus;
       // dust filter on / off
       g_arrnMBTable[VACUUM_LOADER_DUST_FILTER_STATUS_RO] = g_bFilterStatus;
       // now copy individual loader data
       for(i=0;  (i<g_VacCalibrationData.m_nLoaders) && (i<MAX_LOADERS); i++)
       {
          nCompBlock = VACUUM_LOADER_BLOCK1 + (i*VACUUM_LOADER_BLOCK_SIZE);  //calculate data block
          // on /off status
         g_arrnMBTable[nCompBlock + VACUUM_LOADER_ON_OFF_OFFSET_RW] = g_VacCalibrationData.m_bOnFlag[i] ;
         // status word
         g_arrnMBTable[nCompBlock + VACUUM_LOADER_STATUS_OFFSET_RO] = g_sLoader[i].m_nStatusByte;
         // fill timer
         g_arrnMBTable[nCompBlock + VACUUM_LOADER_FILL_TIMER_OFFSET_RO] = g_sLoader[i].m_nFillTimer;
         //full status
         g_arrnMBTable[nCompBlock + VACUUM_LOADER_FULL_STATUS_OFFSET_RO] = g_sLoader[i].m_bLevelFlag ;
       }
       //UNUSED LOADERS
       if( g_VacCalibrationData.m_nLoaders < MAX_LOADERS )
       {
          for(i=g_VacCalibrationData.m_nLoaders; i<MAX_LOADERS; i++)
          {
             nCompBlock = VACUUM_LOADER_BLOCK1 + (i*VACUUM_LOADER_BLOCK_SIZE);  //calculate data block
             // on /off status
            g_arrnMBTable[nCompBlock + VACUUM_LOADER_ON_OFF_OFFSET_RW] = OFF;
            // status word
            g_arrnMBTable[nCompBlock + VACUUM_LOADER_STATUS_OFFSET_RO] = LOADER_STATUS_UNUSED;
            // fill timer
            g_arrnMBTable[nCompBlock + VACUUM_LOADER_FILL_TIMER_OFFSET_RO] = 0;
            //full status
            g_arrnMBTable[nCompBlock + VACUUM_LOADER_FULL_STATUS_OFFSET_RO] = EMPTY;
          }
       }
   }
}

//////////////////////////////////////////////////////
// VacuumLoaderOneHertz
//
//
//////////////////////////////////////////////////////
void VacuumLoaderOneHertz(void)
{
//    CheckForCalibrationWrite();
    CopyVacuumLoaderSummaryDataToMB();
}



