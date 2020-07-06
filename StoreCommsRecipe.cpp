//****************************************************************************
//
// P.Smith                              24/4/07
// initial pass to store recipes from modbus to a specified recipe in the blender
//
// P.Smith                              30/5/07
// added CheckForMultiBlendRecipeSave
//
// P.Smith                              6/6/07
// added copy of recipe name to modbus table.
// there is a strange problem  in the recipe download in that if a printf is used,  
// the save of the recipe data does not always function properly.
//
// P.Smith                              26/7/07
// added CopyMultiBlendRecipesToModbus to copy the currently used recipes to modbus table
// to allow them to be viewed by the panel
//
// P.Smith                              2/8/07
// check for single recipe mode in CheckForMultiBlendRecipeSave
//
// P.Smith                            27/8/07
// CheckForZeroMultiBlendRecipe added to check for Multiblend recipe equal to 0
//
// M.McKiernan                      17/9/07 
// Removed g_arrnMBTable[] & g_arrnMBTableFlags[], now in BatVars2
//
// P.Smith                          30/1/08
// correct compiler warnings
//
// P.Smith                          23/7/08
// remove g_arrnMBTable extern 

//****************************************************************************


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
//nbb-todoh-- #include "Conalg.h"
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
#include "StoreCommsRecipe.h"

extern  CalDataStruct   g_CalibrationData;
extern  structSetpointData  g_TempRecipe; 

structSetpointData  g_CommsDownloadedRecipe[16];

    
//////////////////////////////////////////////////////
// StoreCommsRecipe( void )           
// Routine to transfer the comms recipe from the modbus table to the specified recipe no
// 
//
//////////////////////////////////////////////////////

void StoreCommsRecipe( void )
{
    // store initial recipe data in a structure
    //
    float fPercentage;
    unsigned int i,j,k,l;
    union CharsAndWord uTempCI;
 

    for( i=0; i < MULTIBLEND_EXTRUDER_NO; i++)
    {
        memset( &g_CommsDownloadedRecipe[i], 0, sizeof( structSetpointData ) );
    }
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
/*        iprintf("\n name is");
        for(m=0; m<(RECIPE_NAME_STRING_SIZE); m++)  
        {                //
            iprintf("%c",g_CommsDownloadedRecipe[i].m_arrDescription[m]); 
        }
       
*/ 
    }
    // recipe name

  CheckForZeroMultiBlendRecipe();
  g_nMultiBlendRecipeSaveCtr = 1;                // start multi blend recipe save
}


//////////////////////////////////////////////////////
// CheckForMultiBlendRecipeSave( void )           
// 
//
//////////////////////////////////////////////////////

void CheckForMultiBlendRecipeSave( void )
{
    if(g_CalibrationData.m_nBlenderMode == MODE_SINGLERECIPEMODE)
    {
        if((g_nMultiBlendRecipeSaveCtr !=0) && (g_nSaveRecipeSequence == 0) && (g_CalibrationData.m_nBlenderMode == MODE_SINGLERECIPEMODE))
        {
            g_CommsDownloadedRecipe[g_nMultiBlendRecipeSaveCtr-1].m_nFileNumber = g_nMultiBlendRecipeSaveCtr;
            memcpy(&g_TempRecipe, &g_CommsDownloadedRecipe[g_nMultiBlendRecipeSaveCtr-1], sizeof( structSetpointData ) );
    
//        SetupMttty();
//        iprintf("\n saving recipe no %d",g_nMultiBlendRecipeSaveCtr);
//            for( i = 0; i < g_CalibrationData.m_nComponents; i++ )
//        {
//                 printf("\n percentage for comp %d is    %2.1f",i+1,g_TempRecipe.m_fPercentage[i]); //nbb--testonly--
//        }

            g_nSaveRecipeSequence = SAVE_RECIPE_START;
            ForegroundSaveRecipe( );    // save recipe while running in foreground (1 byte per pass).
            if(g_nMultiBlendRecipeSaveCtr >= MULTIBLEND_EXTRUDER_NO)
            {
                g_nMultiBlendRecipeSaveCtr = 0;
            }
            else
            {
                g_nMultiBlendRecipeSaveCtr++;
            }
           CopyMultiBlendRecipesToModbus();
        } 
    }
}



//////////////////////////////////////////////////////
// CopyMultiBlendRecipesToModbus( void )           
// copies currently used multi blend recipe to modbus 
//
//////////////////////////////////////////////////////

void CopyMultiBlendRecipesToModbus( void )
{
    unsigned int i,j,k;
    for( i=0; i < MULTIBLEND_EXTRUDER_NO; i++)
    {
        for(k = RECIPEDOWNLOAD_BATCH_SETPOINT_PERCENT_1_OFFSET, j=0; k <= RECIPEDOWNLOAD_BATCH_SETPOINT_PERCENT_12_OFFSET && j < g_CalibrationData.m_nComponents; k++, j++ )
        {
            g_arrnMBTable[MULTIBLEND_RECIPEDOWNLOAD_SUMMARY_BLOCK1 +(i * MULTIBLEND_BLOCK_SIZE) + k] = (int) ((g_CommsDownloadedRecipe[i].m_fPercentage[j] + 0.005f) * 100.0f)  ;              //nb MB table has %'s to 2dp.
        }
    }
}



//////////////////////////////////////////////////////
// CheckForZeroMultiBlendRecipe( void )           
// checks for zero multiblend recipe
//
//////////////////////////////////////////////////////

void CheckForZeroMultiBlendRecipe( void )
{
    BOOL bRecipeIsZero;
    unsigned int i,j;
    for( i=0; i < MULTIBLEND_EXTRUDER_NO; i++)
    {
        bRecipeIsZero = TRUE;
        for( j=0; j < g_CalibrationData.m_nComponents; j++)
        {
             if(g_CommsDownloadedRecipe[i].m_fPercentage[j] > 0.01)
            {
                bRecipeIsZero = FALSE; 
            }  
        }
        if(bRecipeIsZero)
        {
            g_bMultiBlendRecipeIsZero[i] = TRUE;
        }
        else
        {
            g_bMultiBlendRecipeIsZero[i] = FALSE;
        }
    }
}









