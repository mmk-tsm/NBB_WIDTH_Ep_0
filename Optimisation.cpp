////////////////////////////////////////////////////////
// P.Smith                      26/6/07
// Optimisation file created, added DecideIfLoaderShouldFill
//
// P.Smith                      28/6/07
// modify SetOptimisationLoaderStatus to check g_bOptimisationUpdate
//
// P.Smith                      2/7/07
// extended DecideIfLoaderShouldBeEnabled to check if EnoughMaterial
// if reached then g_bOptimisationLoaderEnable set to FALSE, this will keep the 
// extruder output full.
//
// P.Smith                      4/7/07
// added CheckForFlapFullToEmptyTransition to check for full to empty transition
// The calculation to calculate the full weight for the component concerned is implemetned.
// The rate for the component is calculated and verified as operational.
// loader weight is verified by activating the empty input every minute.
// Batch size is 5kgs and component % is 80,20, the weigh decrements from 4kgs to 0 over a 1 minute period.
// The estimated weight must not be allowed to go negative, if it goes negative, it is set 
// to 0.
// EstimateLoaderWeight is called on a one second basis.
//
// P.Smith                      10/7/07
// call EnoughMaterial function anyway so that the relevant data is calculated even though
// optimisation is disabled. 
//
// P.Smith                      17/7/07
// if the loader is disabled and the flap is showing empty, then the loader must be empty.
// loader weight and loader accumulating weight is set to 0
// 
// P.Smith                      20/7/07
// added ComponentInRecipe to determine if component no is in the recipe. If it is not in the recipe the 
// loader for this component is switched off.
// 
// P.Smith                      25/7/07
// To avoid multiple triggering of the purge commmand check g_bBlenderPurged and g_bBlenderPurgeingInProgress
// before triggering the purge command.
// No optimisation on purge component if purge is auto or manual
// implement order length download from modbus table
// check for automatic purging of the machine, this is based on the order length, not when optimisation is complete
// remove check for optimisation in auto purging check
// ensure that g_bBlenderPurged & g_bBlenderPurgeingInProgress
// No optimisation on purge component
// if g_nLoaderEmptyingCounter is 0, set emptying rate to 0
// CheckForAutomaticPurging added
// 
// P.Smith                      25/7/07
// remove set of denisty in PresetOptimsationvariables
//
// M.McKiernan                      17/9/07 
// Removed g_arrnMBTable[] & g_arrnMBTableFlags[] - now in BatVars2
//
// P.Smith                      27/8/07
// Ensure that if a component is not in the recipe that it is left on.
// This will allow components that are not being used to be set up
// before they are actually needed.
// this is done by setting bEnoughMaterial to FALSE if ComponentInRecipe (n) is FALSE
//
// P.Smith                      9/1/08
// removed optimisation length diagnostic.
//
// P.Smith                          30/1/08
// correct compiler warnings
//
// P.Smith                          23/7/08
// remove g_arrnMBTable and array comms extern 
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
#include "Optimisation.h"

extern  CalDataStruct   g_CalibrationData;
extern  structSetpointData      g_CurrentRecipe;
extern  structRollData      g_Order;



//////////////////////////////////////////////////////
// SetOptimisationLoaderStatus ( void )           
// Decide if the flap status should be passed directly to the output.
// If it should be, then set fill output status to that of the flap status.
// else set the load fill output to false.
//
//////////////////////////////////////////////////////

void SetOptimisationLoaderStatus( void )
{
    int i;
    for(i=0; i < MAX_COMPONENTS; i++)
    {
        if(g_bOptimisationUpdate[i])
        {
            g_bOptimisationUpdate[i] = FALSE;
            if(g_bOptimisationLoaderEnable[i])   // loader work as normal ?
            {
                g_bOptimisationLoaderFill[i] = g_bOptimisationFlapStatus[i];
            }
            else
            {
                g_bOptimisationLoaderFill[i] = FULL;
            }
        }
    }
}


//////////////////////////////////////////////////////
// DecideIfLoaderShouldBeEnabled ( void )           
// Decide if the loader status should be enabled or disabled, if disabled it will be set to full
//
//////////////////////////////////////////////////////

void DecideIfLoaderShouldBeEnabled( void )
{
    unsigned int i;
 
    for(i=0; i < MAX_COMPONENTS; i++)
    {
        if(!g_CalibrationData.m_bOptimisation)
        {
            g_bOptimisationLoaderEnable[i] = TRUE;  // pass status through as normal.    
            EnoughMaterial(i);                      // run the calculation anyway
        }
        else
        {
            if(((g_CalibrationData.m_nPurgeEnable == AUTO_PURGE) || (g_CalibrationData.m_nPurgeEnable == MANUAL_PURGE)) && (g_CalibrationData.m_nPurgeComponentNumber == (i+1)))
            {
                g_bOptimisationLoaderEnable[i] = TRUE;  //     
            }

            else
            if(EnoughMaterial(i))
            {
                g_bOptimisationLoaderEnable[i] = FALSE;  //     
            }
            else
            {
                g_bOptimisationLoaderEnable[i] = TRUE;  //      
            }
        }
        g_bOptimisationUpdate[i] = TRUE;
    }
    CheckForAutomaticPurging();

}
            

//////////////////////////////////////////////////////
// EnoughMaterial ( void )           
// determine if the order length has been reached.
// The bin weight is estimated from the bin volume and the bulk density.
// if there is enough material to fill the order return true.
//////////////////////////////////////////////////////

BOOL EnoughMaterial( int n )
{
    BOOL bEnoughMaterial;
//    g_fOptimisationOrderLength = g_Order.m_fLength; //  
    g_fComponentBinWeight[n] =  g_CurrentRecipe.m_fDensity[n] * ((float)g_CalibrationData.m_nComponentBinVolume[n]);
    g_fComponentBinAndLoaderWeight[n] = g_fComponentLoaderWeight[n] + g_fComponentBinWeight[n];

    g_fRemainingOrderLength = g_CurrentRecipe.m_fOrderLength - g_fOptimisationOrderLength; 

    if(g_fRemainingOrderLength < 0)
    {
        g_fRemainingOrderLength = 0;
    }

    g_fWeightPerMeterFromLSpeed = (g_fRRThroughputKgPerHour * 1000.0f) / (g_fLineSpeed5sec * 60);
    g_fRemainingOrderWeight = (g_fRemainingOrderLength * g_fWeightPerMeterFromLSpeed)/1000.0f;
//    g_fRemainingOrderWeight = (g_fRemainingOrderLength * g_fWeightPerMeterRR)/1000.0f;
    
    g_fComponentRemainingOrderWeight[n] = (g_CurrentRecipe.m_fPercentage[n]/100) * g_fRemainingOrderWeight;
 
    if(!ComponentInRecipe(n))
    {
        bEnoughMaterial = FALSE;
    }
    else
    if(g_fComponentRemainingOrderWeight[n] < g_fComponentBinAndLoaderWeight[n])
    {
        bEnoughMaterial = TRUE;
    }
    else
    {
        bEnoughMaterial = FALSE;
    }
  
   return(bEnoughMaterial);
}




//////////////////////////////////////////////////////
// SetOptimisationLoaderFillStatus ( void )           
// Sets the load fill output depending on g_bOptimisationLoaderFill
//////////////////////////////////////////////////////

void SetOptimisationLoaderFillOutputStatus( void )
{
    int i;
    unsigned int unTemp,unFillOutput;
    unTemp = 0x01;
    unFillOutput = 0;
    for(i=0; i < MAX_COMPONENTS; i++)
    {
 
         if(g_bOptimisationLoaderFill[i])
        {
            unFillOutput |= unTemp;
        }
        else
        {
             unFillOutput &= ~unTemp;
        }
       unTemp <<= 1;  
    }
    g_unOptimisationLoaderOuputs = unFillOutput;   // set the resulting output status.
}


//////////////////////////////////////////////////////
// CheckForFlapFullToEmptyTransition
// called 10 times a second.
// checks for full to empty transition on flap input
// debounce counter set to stop the flap being read when it initially opens.
//
//////////////////////////////////////////////////////

void CheckForFlapFullToEmptyTransition( void )
{
    unsigned int i;
    for(i=0; i < g_CalibrationData.m_nComponents; i++)
    {
        if((g_bOptimisationFlapStatus[i] == EMPTY) && (g_nOptimisationFlapDebounceCounter[i] == 0))
        {
            if(g_bOptimisationFlapHistoryStatus[i] == FULL)  // transition detected
            {
                g_nOptimisationFlapDebounceCounter[i] = FLAPDEBOUNCETIME;
                CalculateLoaderEmptyingRate(i);
            }
        }
        g_bOptimisationFlapHistoryStatus[i] = g_bOptimisationFlapStatus[i]; 
    }
}

//////////////////////////////////////////////////////
// CalculateLoaderEmptyingRate
// called 10 times a second.
// checks for full to empty transition on flap input
// debounce counter set to stop the flap being read when it initially opens.
//
//////////////////////////////////////////////////////

void CalculateLoaderEmptyingRate( int n )
{
    BatchWeightCompensation();
    g_fComponentLoaderWeightAccumulator[n] = g_fComponentLoaderWeightAccumulator[n] - g_fComponentActualUnusedWeight[n]; 
// calculate emptying rate and take full weight, reset weight counter

    g_CalibrationData.m_fComponentLoaderFullWeight[n] = g_fComponentLoaderWeightAccumulator[n];
    if(g_nLoaderEmptyingCounter[n] != 0)
    {
        g_CalibrationData.m_fComponentLoaderEmptyingRate[n] = g_CalibrationData.m_fComponentLoaderFullWeight[n] / (float) g_nLoaderEmptyingCounter[n];
    }
    else
    {
        g_CalibrationData.m_fComponentLoaderEmptyingRate[n] = 0.0f;  // avoid divide by 0
    }
    g_fComponentLoaderWeightAccumulator[n] = 0.0f;
    g_nLoaderEmptyingCounter[n] = 0;
    BatchWeightCompensation();
    g_fComponentLoaderWeightAccumulator[n] = g_fComponentLoaderWeightAccumulator[n] + g_fComponentActualUnusedWeight[n]; 
    g_bSaveAllCalibrationToEEpromOutsideCycle = TRUE;
}

//////////////////////////////////////////////////////
// EstimateLoaderWeight
// called once a second
// Estimates the loader weight for all components.
//////////////////////////////////////////////////////

void EstimateLoaderWeight( void )
{
    unsigned int i;
    for(i=0; i < g_CalibrationData.m_nComponents; i++) 
    {
        if((!g_bOptimisationLoaderEnable[i]) && (g_bOptimisationFlapStatus[i] == EMPTY))
        { // if loader is disabled and flap status is empty, loader must be empty
            g_fComponentLoaderWeight[i] = 0.0f;
            g_fComponentLoaderWeightAccumulator[i] = 0.0f;
        }
        else
        {
            g_fComponentLoaderWeight[i] = g_CalibrationData.m_fComponentLoaderFullWeight[i] - (g_nLoaderEmptyingCounter[i] * g_CalibrationData.m_fComponentLoaderEmptyingRate[i]);
        }
        if(g_fComponentLoaderWeight[i] < 0)
        {
            g_fComponentLoaderWeight[i] = 0.0f;
        }
        
/*  nbb--testonly--
        if((i==0) || (i==1) || (i==2) )  
        {
          if(g_bRedirectStdioToPort2)
          {
              iprintf("\n\n flap %d en %d output %d",g_bOptimisationFlapStatus[i],g_bOptimisationLoaderEnable[i],g_bOptimisationLoaderFill[i]);
              printf("\n loader %d bin volume %d bin wt %3.2f full wt %3.4f, empty rate %3.4f, actual weight %3.4f",i+1,g_CalibrationData.m_nComponentBinVolume[i],g_fComponentBinWeight[i],g_CalibrationData.m_fComponentLoaderFullWeight[i],g_CalibrationData.m_fComponentLoaderEmptyingRate[i],g_fComponentLoaderWeight[i]);
              printf("\n set length  %4.1f act %4.1f  remaining order is %3.2f kg comp wt is %3.2f",g_CurrentRecipe.m_fOrderLength,g_fOptimisationOrderLength,g_fRemainingOrderWeight,g_fComponentRemainingOrderWeight[i]);
            printf("\n weight per meter %3.2f round robin value is %3.2f",g_fWeightPerMeterFromLSpeed,g_fWeightPerMeterRR);

          }
        }          
*/    }
}



//////////////////////////////////////////////////////
// ComponentInRecipe
// checks if the component is in the recipe, components passed in as 0 ---
// checks against component nos in seq table 1---
//
//////////////////////////////////////////////////////

BOOL ComponentInRecipe( int n )
{
    int i,nCompIndex;
    BOOL bFoundComponent;

    bFoundComponent = FALSE;
    i = 0;
    while((g_cSeqTable[i] != SEQDELIMITER) && (i < MAX_COMPONENTS)) // SEQDELIMITER indicates table end
    {
        nCompIndex = (int)(g_cSeqTable[i] & 0x0F) - 1;  
        if(n == nCompIndex)
        {
            bFoundComponent = TRUE;
        }
        i++;
    }
    return(bFoundComponent);
}

//////////////////////////////////////////////////////
// CheckForAutomaticPurging
// checks if the component is in the recipe, components passed in as 0 ---
// checks against component nos in seq table 1---
//
//////////////////////////////////////////////////////

void CheckForAutomaticPurging( void )
{
    BOOL bOptimisationComplete;

    if(g_CalibrationData.m_nPurgeEnable == AUTO_PURGE)
    {    
 
        if(g_fOptimisationOrderLength > g_CurrentRecipe.m_fOrderLength )
        {
          bOptimisationComplete = TRUE;      
        }
        else
        { 
          bOptimisationComplete = FALSE;
        }  
               
        if(bOptimisationComplete && !g_bBlenderPurged && !g_bBlenderPurgeingInProgress)
        {
            g_bInitiatePurge = TRUE;  // if optimisation complete, initate purge
        }
    } 
}






