//
// P.Smith                      19/7/07
// purge blender
//
// M.McKiernan                      17/9/07 
// Removed g_arrnMBTable[] & g_arrnMBTableFlags[], now in BatVars2
//       g_nMBWriteOrFlag, g_nMBWriteOrErrorFlag, g_nMBWriteOrFlagTemp
//
// P.Smith                          22/11/07
// initiate cleaning on purge of blender.
//
// P.Smith                          8/1/08
// only initiate cleaning if enabled.
//
// P.Smith                          30/1/08
// correct compiler warnings
//
// P.Smith                          10/6/08
// do not reset totals on purge command implementation.
//
// P.Smith                          23/7/08
// remove g_arrnMBTable and array comms extern 
//
// P.Smith                          16/10/08
// remove todo
/////////////////////////////////////////////////////////

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
#include "Optimisation.h"


extern  CalDataStruct   g_CalibrationData;
extern  structSetpointData      g_CurrentRecipe;
extern  structRollData      g_Order;
extern  structSetpointData  g_TempRecipe; 


//////////////////////////////////////////////////////
// purges blender sets purge component to 100%
//
//////////////////////////////////////////////////////

void PurgeBlender( void )
{
    unsigned int i;
    char   cTemp;

    LoadRecipe(RUNNINGRECIPEFILENO);                                                                  //load recipe 5  to TempRecipe.
    g_bBlenderPurged = TRUE;
// set purge component no to 100%
    for( i = 0; i < g_CalibrationData.m_nComponents; i++ )
    {
        if((i+1) == g_CalibrationData.m_nPurgeComponentNumber)
        {
            g_TempRecipe.m_fPercentage[i] = 100.0f;
        } 
        else
        {
           g_TempRecipe.m_fPercentage[i] = 0.0f;
        }
          
    }                                                                                                         

   CopySetpointsToMB();                // COPYSPMB Copy setpoint data into MB table.
   
   g_arrnMBTable[BATCH_SETPOINT_RESET_TOTALS] |= MB_RESET_TOTALS_BIT;  

   cTemp =  CheckMBValidData();
   if(cTemp != ILLEGAL_MB_DATA_VALUE)
   {
      g_nMBWriteOrFlagTemp = MB_WRITE_PERCENTAGES_BIT;
      g_nMBWriteOrFlag = g_nMBWriteOrFlagTemp;            // sTORE TEMP 'OR' FLAG IN WRITE FLAG
   }
   if(g_CalibrationData.m_nCleaning != 0)           // asm = CLEANING
   {
       InitiateCleanCycle();   // ASM = INITIATECLEANCYCLE                            
   }
}



