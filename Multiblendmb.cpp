////////////////////////////////////////////////////////
// 
// P.Smith                      25/4/07
// SaveMultiblendExtruderSetUp to save Multiblend setup to eeprom
// CopyMultiblendSummaryDataToMB copies production summary data to memory.
// 
// P.Smith                      30/4/07
// Ensure that g_nMBWriteOrFlag is set to g_nMBWriteOrFlagTemp to initiate the order change
//
// P.Smith                      30/5/07
// Extruder recipe no is no longer needed, this is removed.
//
// P.Smith                      2/6/07
// added CopyMultiblendRecipesToMB 
//
// P.Smith                      8/6/07
// added CopyMultiblendSetupToMB so that the multi blend set up is copied to modbus on 
// power up, all printfs removed.
//
// P.Smith                      26/7/07
// added copy of recipe no for multiblend, this is used to store the recipe no in the panel
//
// P.Smith                      2/8/07
// only inititiate SaveMultiblendExtruderSetUp if single recipe mode
// removed unused print functions
//// M.McKiernan                      17/9/07 
// Removed g_arrnMBTable[] & g_arrnMBTableFlags[], now in BatVars2
//       g_nMBWriteOrFlag, g_nMBWriteOrErrorFlag, g_nMBWriteOrFlagTemp
//
// P.Smith                  23/10/07
// CopyMultiBlendRecipesToModbus & CopyCurrentRecipeToMBlend called to copy 
//
// P.Smith                          30/1/08
// correct compiler warnings
//
// P.Smith                          1/5/08
// in StartRecipe, set g_bPanelWriteHasHappened to TRUE to ensure that the comparision
// check does not stop the download from happening.
//
// P.Smith                          17/5/08
// copy g_bMultiBlendisPaused to modbus table
//
// P.Smith                          23/7/08
// remove g_arrnMBTable extern 
//
// P.Smith                          16/10/08
// make todo lp
////////////////////////////////////////////////////////

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
#include "Storecommsrecipe.h"


extern  CalDataStruct   g_CalibrationData;
extern  structSetpointData  g_TempRecipe; 
extern  structSetpointData g_CommsRecipe;
extern  structSetpointData  g_CommsDownloadedRecipe[];


    
//////////////////////////////////////////////////////
// SaveMultiblendExtruderSetUp( void )           
// 
//
//////////////////////////////////////////////////////
void SaveMultiblendExtruderSetUp( void )
{
    unsigned int i,j;
    union CharsAndWord uTempCI;
    BOOL    bSwap = TRUE;
  if(g_CalibrationData.m_nBlenderMode == MODE_SINGLERECIPEMODE)  // single recipe mode ?
  {
    
       for(j=0; j<MULTIBLEND_EXTRUDER_NO; j++)
       {                  //
       for(i=0; i<(MULTIBLEND_EXTRUDER_NAME_SIZE/2); i++)                  //
        {
            uTempCI.nValue = g_arrnMBTable[(j * MULTIBLEND_EXTRUDER_NAME_SIZE) + MULTIBLEND_EXTRUDERSETUP_EXTRUDER_NAME_1 + i];        // Modbus has two ascii chars in each register.
            if(bSwap)
            {
                g_CalibrationData.m_stMultiblendExtruders[j].m_cExtruderName[i*2] = uTempCI.cValue[1];
                g_CalibrationData.m_stMultiblendExtruders[j].m_cExtruderName[i*2 + 1] = uTempCI.cValue[0];
            }
            else
            {
                g_CalibrationData.m_stMultiblendExtruders[j].m_cExtruderName[i*2] = uTempCI.cValue[0];
                g_CalibrationData.m_stMultiblendExtruders[j].m_cExtruderName[i*2 + 1] = uTempCI.cValue[1];
            }
        }
      }  

     // Extruder Enabled / disabled option
       for(j=0; j<MULTIBLEND_EXTRUDER_NO; j++)
       {
              g_CalibrationData.m_stMultiblendExtruders[j].m_bEnabled =  g_arrnMBTable[MULTIBLEND_EXTRUDERSETUP_ENABLE_EX1 + j];        
       }      
      
       g_bSaveAllCalibrationToEEprom = TRUE;
  }
}


//////////////////////////////////////////////////////
// CopyMultiblendSummaryDataToMB( void )           
// 
//
//////////////////////////////////////////////////////
void CopyMultiblendSummaryDataToMB( void )
{
    int i;
    // queue data
    for(i=0; i<MULTIBLEND_EXTRUDER_NO; i++)                  
    {
        g_arrnMBTable[i+ MULTIBLEND_SUMMARY_QUEUE_POS_1] =  g_unaCurrentQueue[i];
    }
        g_arrnMBTable[i+ MULTIBLEND_SUMMARY_QUEUE_LENGTH] =  0;
    
    // flap status
    for(i=0; i<MULTIBLEND_EXTRUDER_NO; i++)                  
    {
        g_arrnMBTable[i+ MULTIBLEND_SUMMARY_FLAP_STATUS_EX1] =  g_baCurrentFlapStatus[i];
    }
    // in progress status
    for(i=0; i<MULTIBLEND_EXTRUDER_NO; i++)                  
    {
        g_arrnMBTable[i+ MULTIBLEND_SUMMARY_IN_PROGREES_STATUS_EX1] =  g_baInProgress[i];
    }
    
    // MULTIBLEND STAGE
    
    g_arrnMBTable[MULTIBLEND_SUMMARY_MULTIBLEND_STAGE] =  g_nVacuumSequence;
    // vacuum on
    g_arrnMBTable[MULTIBLEND_SUMMARY_VACUUM_ON_OFF_STATUS] = g_arrnMBTable[BATCH_SINGLE_RECIPE_VACUUM_INFO];
    
    // download history
    for(i=0; i<DOWNLOADHISTORYNO; i++)                  
    {
        g_arrnMBTable[i+ MULTIBLEND_SUMMARY_DOWNLOAD_HISTORY_1] =  g_unaDownLoadHistory[i];
    }

    // paused status
    if(g_bMultiBlendisPaused)  //kwh
    {
        g_arrnMBTable[MULTIBLEND_SUMMARY_MULTIBLEND_PAUSED_STATUS] =  1;
    }
    else
    {
        g_arrnMBTable[MULTIBLEND_SUMMARY_MULTIBLEND_PAUSED_STATUS] =  0;
    }
}


//////////////////////////////////////////////////////
// StartRecipe( void )           
// 
//
//////////////////////////////////////////////////////
BOOL StartRecipe( int nRecipe )
{
   char   cTemp;
   LoadRecipe(nRecipe);
    g_TempRecipe.m_nExtruder_No	= g_unCurrentExtrNo+1;  // set extruder no 
    CopySetpointsToMB();                // COPYSPMB Copy setpoint data into MB table.
 
   cTemp =  CheckMBValidData();
   if(cTemp != ILLEGAL_MB_DATA_VALUE)
   {
      g_nMBWriteOrFlagTemp = MB_WRITE_PERCENTAGES_BIT;
      g_nMBWriteOrFlag = g_nMBWriteOrFlagTemp;            // sTORE TEMP 'OR' FLAG IN WRITE FLAG
      g_bPanelWriteHasHappened = TRUE;
   }
   else
   {
//      iprintf("\n component per do not total 100%");
   }

    return(FALSE);
}

//////////////////////////////////////////////////////
// CopyMultiblendRecipesToMB( void )           
// 
//
//////////////////////////////////////////////////////
void CopyMultiblendRecipesToMB( void )
{
    unsigned int i,j,k,l;
    float fPercentage;
    union CharsAndWord uTempRev;

    if(g_CalibrationData.m_nBlenderMode == MODE_SINGLERECIPEMODE)
    {
        for( i=0; i < MULTIBLEND_EXTRUDER_NO; i++)
        {
            LoadRecipe(i+1);
            for(k = RECIPEDOWNLOAD_BATCH_SETPOINT_PERCENT_1_OFFSET, j=0; k <= RECIPEDOWNLOAD_BATCH_SETPOINT_PERCENT_12_OFFSET && j < g_CalibrationData.m_nComponents; k++, j++ )
            {
                fPercentage =   g_TempRecipe.m_fPercentage[j];
                fPercentage += 0.0005;       // round
                g_arrnMBTable[MULTIBLEND_RECIPEDOWNLOAD_BLOCK1 +(i * MULTIBLEND_BLOCK_SIZE) + k] = (int)(fPercentage * 100);        // NB: Percentages are to 2DP in MB table. (9999 = 99.99%)
            }   
            for(l=0; l<(RECIPE_NAME_STRING_SIZE/2); l++) // copy into MB table.
            {
                uTempRev.cValue[0] = g_TempRecipe.m_arrDescription[l*2];           
                uTempRev.cValue[1] = g_TempRecipe.m_arrDescription[(l*2)+1]; 
                g_arrnMBTable[MULTIBLEND_RECIPEDOWNLOAD_BLOCK1 +(i * MULTIBLEND_BLOCK_SIZE)+RECIPEDOWNLOAD_RECIPE_NAME_OFFSET + l] = uTempRev.nValue;
            }   
            g_arrnMBTable[MULTIBLEND_RECIPEDOWNLOAD_BLOCK1 +(i * MULTIBLEND_BLOCK_SIZE)+RECIPEDOWNLOAD_RECIPE_NO_OFFSET ] = g_TempRecipe.m_nRecipe_No; //nbb--todolp-- check that this works

       }
     }
     CopyMultiBlendRecipesToModbus();
     CopyCurrentRecipeToMBlend();
}


//////////////////////////////////////////////////////
// CopyMultiblendSetupToMB( void )           
// 
//
//////////////////////////////////////////////////////
void CopyMultiblendSetupToMB( void )
{
    int i;
    if(g_CalibrationData.m_nBlenderMode == MODE_SINGLERECIPEMODE)
    {
        for( i=0; i < MULTIBLEND_EXTRUDER_NO; i++)
        {
                g_arrnMBTable[MULTIBLEND_EXTRUDERSETUP_ENABLE_EX1+i] = g_CalibrationData.m_stMultiblendExtruders[i].m_bEnabled;
        }    
    }
    
}




//////////////////////////////////////////////////////
// CopyCurrentRecipeToMBlend( void )           
// 
//
//////////////////////////////////////////////////////
void CopyCurrentRecipeToMBlend( void )
 {   
    unsigned int i,j,k,l;
    float    fPercentage;
    union CharsAndWord uTempCI;

    for( i=0; i < MULTIBLEND_EXTRUDER_NO; i++)
    {
        g_CommsDownloadedRecipe[i].m_nRecipe_No = g_arrnMBTable[MULTIBLEND_RECIPEDOWNLOAD_BLOCK1 +(i * MULTIBLEND_BLOCK_SIZE) + RECIPEDOWNLOAD_RECIPE_NO_OFFSET];       // recipe no

        for(k = RECIPEDOWNLOAD_BATCH_SETPOINT_PERCENT_1_OFFSET, j=0; k <= RECIPEDOWNLOAD_BATCH_SETPOINT_PERCENT_12_OFFSET && j < g_CalibrationData.m_nComponents; k++, j++ )
        {
            fPercentage = (float)g_arrnMBTable[MULTIBLEND_RECIPEDOWNLOAD_BLOCK1 +(i * MULTIBLEND_BLOCK_SIZE) + k];              //nb MB table has %'s to 2dp.
            fPercentage /= 100.0f;                                  // div by 100.
            g_CommsDownloadedRecipe[i].m_fPercentage[j] = fPercentage;       
        }

        g_CommsDownloadedRecipe[i].m_nRegrindComponent = g_arrnMBTable[MULTIBLEND_RECIPEDOWNLOAD_BLOCK1 +(i * MULTIBLEND_BLOCK_SIZE) + RECIPEDOWNLOAD_BATCH_SETPOINT_REGRIND_COMPONENT_OFFSET ] & 0x00FF;  // the regrind component no. is in the L.S.byte.;
         for(l=0; l<(RECIPE_NAME_STRING_SIZE/2); l++)                  //
        {
            uTempCI.nValue = g_arrnMBTable[MULTIBLEND_RECIPEDOWNLOAD_BLOCK1 +(i * MULTIBLEND_BLOCK_SIZE)+RECIPEDOWNLOAD_RECIPE_NAME_OFFSET + l];        // Modbus has two ascii chars in each register.
            {
                g_CommsDownloadedRecipe[i].m_arrDescription[l*2] = uTempCI.cValue[0];
                g_CommsDownloadedRecipe[i].m_arrDescription[l*2 + 1] = uTempCI.cValue[1];
            }
        }
     }
}     




