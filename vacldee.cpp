///////////////////////////////////////////////////////
// VacLdee.cpp
//
// Vacuum loader EEPROM functions.
// the necessary data structures
//
// P.Smith                          16-06-2009

// P.Smith                          25/6/09
// added, not called CheckForValidLoaderConfigData
// 
// P.Smith                          30/6/09
// name change DEFAULT_OVERLAP_TIME, default filter type set to TSM
// check for min dump delay, name change DEFAULT_POST_FILL_TIME
// DEFAULT_PRIORITY
// 
// P.Smith                          2/7/09
// name change dump delay to discharge time
// 
// P.Smith                          29/9/09
// add min check for priority
// check for filter repetitions added.
//
// P.Smith                          29/9/09
// if vacuum loader calibration data is not okay, then copy to the modbus table.
// check for idle time = 0, set default if invalid.
// correct filter repetitions
// if data invalid, call CopyVacuumLoaderCalibrationParametersToMB  
//******************* For Batch Blender ***************************************



#include <stdio.h>
#include <basictypes.h>
#include "vacvars.h"
#include "EEPROMdriver.h"
#include "BatVars.h"
#include "BatVars2.h"
#include "CsumCalc.h"
#include "Vacldee.h"
#include <string.h>
#include "Hash.h"
#include "Mbvacldr.h"

extern   structVacCalib   g_VacCalibrationData;


//////////////////////////////////////////////////////////
// LoadLoaderCalibrationDataFromEEprom
// load loader calibration data from eeprom
///////////////////////////////////////////////////////////
int LoadLoaderCalibrationDataFromEEprom( void )
{
    int nResult = 0;
    int nChipSelectUsed,nSize;
    unsigned char    g_arrcTemp[sizeof(g_VacCalibrationData)];

    int     i,nBytes;
    char    *pPtr;
    pPtr = (char*)&g_VacCalibrationData;
    g_bSPIEEActive = TRUE;                // indicate eeprom is active.
    nChipSelectUsed = g_nEEPROMSPIChipSelect;    // for NBB.
    SPI_Select_EEPROM();
    UnProtectEEPROM( nChipSelectUsed );    // enable writes to eeprom (all).
 
    // Write out the data
    nBytes = sizeof( g_VacCalibrationData );

    for( i = 0; i < nBytes; i++ )
    {
        *(pPtr + i) =  EEPROMRead1Byte( nChipSelectUsed, VACUUM_LOADER_CALIBRATION_DATA_START + i ); 
    }                                                                          

    memcpy(&g_arrcTemp, &g_VacCalibrationData, sizeof(g_VacCalibrationData));    // copy structure to array.
    nSize = sizeof(g_VacCalibrationData)- sizeof(g_VacCalibrationData.m_nChecksum);
    g_unCalibrationCheckSum = CalculateCheckSum(g_arrcTemp, nSize);

    if(g_unCalibrationCheckSum != g_VacCalibrationData.m_nChecksum)
    {
    //nbb--todo--vac add to ram    g_bLoadCalibrationDataInvalid = TRUE;
    }
    else
    {
    //nbb--todo--vac add to ram     g_bLoadCalibrationDataInvalid = FALSE;
    }

    g_bSPIEEActive = FALSE;                // indicate eeprom is inactive.
    return( nResult );
}


///////////////////////////////////////////////////////////
// SaveLoaderDataToEEprom
///////////////////////////////////////////////////////////
void SaveLoaderDataToEEprom( void )
{
    int     i,nBytes,nSize;
    WORD  nChipSelectUsed;
    char    *pPtr;
    unsigned char    g_arrcTemp[sizeof(g_VacCalibrationData)];
    unsigned char   cData;
    
    g_bSPIEEActive = TRUE;                // indicate eeprom is active.
    
    if( (g_arrnMBTable[BATCH_SUMMARY_PROCESS_ALARMS] & CALINVALARMBIT) ) /* tare alarm bit set???  */
    {  
        RemoveAlarmTable( CALINVALARM,  0 );      /* indicate alarm cleared, RMALTB */
    }

    pPtr = (char*)&g_VacCalibrationData;
    nSize = sizeof(g_VacCalibrationData)- sizeof(g_VacCalibrationData.m_nChecksum);
    // merge equivalent of CALCULATEEEPROMCHECKSUM inline 
    memcpy(&g_arrcTemp, &g_VacCalibrationData,nSize );    // copy structure to array.
 
    g_VacCalibrationData.m_nChecksum = CalculateCheckSum(g_arrcTemp, nSize);
    // Write out the data
    nBytes = sizeof( g_VacCalibrationData );

    nChipSelectUsed = g_nEEPROMSPIChipSelect;    // for NBB.
    SPI_Select_EEPROM();
    Remove_EEPROM_Write_Protect();
    UnProtectEEPROM( nChipSelectUsed );    // 
   
    for( i = 0; i < nBytes ; i++ )
      {
          cData = *(pPtr + i);
          EEPROMWrite1Byte(nChipSelectUsed,  cData, VACUUM_LOADER_CALIBRATION_DATA_START + i);      //
      }
     Set_EEPROM_Write_Protect();
     g_bSPIEEActive = FALSE;   
}

///////////////////////////////////////////////////////////
// CheckForValidLoaderConfigData
///////////////////////////////////////////////////////////
BOOL CheckForValidLoaderConfigData( void )
{
    unsigned int i;
    BOOL  bDataOkay = TRUE;
    if(g_VacCalibrationData.m_nLoaders > MAX_LOADERS)
    {
        g_VacCalibrationData.m_nLoaders = MAX_LOADERS;
        bDataOkay = FALSE;
    }
    if(g_VacCalibrationData.m_nAVOverLapTime > MAX_OVERLAP_TIME)
    {
            g_VacCalibrationData.m_nAVOverLapTime = DEFAULT_OVERLAP_TIME;
            bDataOkay = FALSE;
    }

   if((g_VacCalibrationData.m_nPumpIdleTime == 0)||(g_VacCalibrationData.m_nPumpIdleTime > MAX_PUMP_IDLE_TIME))
    {
        g_VacCalibrationData.m_nPumpIdleTime = DEFAULT_PUMP_IDLE_TIME;
        bDataOkay = FALSE;
    }
   if(g_VacCalibrationData.m_nPumpStartTime < PUMP_START_TIME_MIN)
    {
        g_VacCalibrationData.m_nPumpStartTime = PUMP_START_TIME_MIN;
        bDataOkay = FALSE;
    }
    if(g_VacCalibrationData.m_nPumpStartTime > PUMP_START_TIME_MAX)
    {
        g_VacCalibrationData.m_nPumpStartTime = PUMP_START_TIME_MAX;
        bDataOkay = FALSE;
    }
   
   if(g_VacCalibrationData.m_nFilterType > 3)
    {
        g_VacCalibrationData.m_nFilterType = FILTER_TYPE_TSM;  // default to tsm filter
        bDataOkay = FALSE;
    }
    if(g_VacCalibrationData.m_nDischargeTime > MAX_DISCHARGE_TIME)
    {
        g_VacCalibrationData.m_nDischargeTime = DEFAULT_DISCHARGE_TIME;
        bDataOkay = FALSE;
   }
   else
   if(g_VacCalibrationData.m_nDischargeTime < MIN_DISCHARGE_TIME)
   {
        g_VacCalibrationData.m_nDischargeTime = MIN_DISCHARGE_TIME;
        bDataOkay = FALSE;
   }   
   //Filter Repetitions 
   if((g_VacCalibrationData.m_nFilterRepetitions == 0)||(g_VacCalibrationData.m_nFilterRepetitions > MAX_FILTER_REPETITIONS))
   {
       g_VacCalibrationData.m_nFilterRepetitions = DEFAULT_FILTER_REPETITIONS;
       bDataOkay = FALSE;
   }

   for(i=0;  i<g_VacCalibrationData.m_nLoaders; i++)
   {
       if(g_VacCalibrationData.m_nSuctionTime[i] > MAX_SUCTION_TIME)
       {
           g_VacCalibrationData.m_nSuctionTime[i] = DEFAULT_SUCTION_TIME;
           bDataOkay = FALSE;
       }
   
       if(g_VacCalibrationData.m_nPostFillTime[i] > MAX_POST_FILL_TIME)
       {
            g_VacCalibrationData.m_nPostFillTime[i] = DEFAULT_POST_FILL_TIME;
            bDataOkay = FALSE;
       }
       if(g_VacCalibrationData.m_nPriority[i] > MAX_LOADER_PRIORITY ||  g_VacCalibrationData.m_nPriority[i] < 1 )
       {
           g_VacCalibrationData.m_nPriority[i] = DEFAULT_PRIORITY;
           bDataOkay = FALSE;
       }
   }
   if(!bDataOkay)   // data not valid - reload again.
   {
       CopyVacuumLoaderCalibrationParametersToMB();
   }

   
    return(bDataOkay);
}

