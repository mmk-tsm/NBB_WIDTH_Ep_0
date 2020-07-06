////////////////////////////////////////////////////////////////////////////////
// P.Smith                                               15/9/09
// added CheckForChangeInComponentFillingMethod, this function checks for a change
// in the state of the filling enable and enables the pusling calibrate
// moved PulsingTargetReached into this file
// modified pulsing so that when it gets within the band it reverts to 
// a shorter pulse,
// This allows honing in on the target
// added RecalibratePulsingForComponents, needs some more work.
//
// P.Smith                                               15/9/09
// make variable component related so that they will be available at all times
// name change CheckIfPulsingCalibrateIsRequired, check for set point % change
// 
// P.Smith                                               15/9/09
// if optimix, then make pulse weight required 0.6 grams
//
// 
////////////////////////////////////////////////////////////////////////////////
#include <startnet.h>
#include <basictypes.h>
#include <stdio.h> 
#include "MCP23Sexp.h"
#include "Batvars.h"
#include "Batvars2.h"
#include "BatchCalibrationFunctions.h"
#include "Expansion.h"
#include "SetpointFormat.h"

extern CalDataStruct    g_CalibrationData;
extern  structSetpointData      g_CurrentRecipe;


//////////////////////////////////////////////////////


void CheckIfPulsingCalibrateIsRequired( void )
{
    unsigned int i;
    for(i= 0; i < g_CalibrationData.m_nComponents; i++)
    {
       if(g_nFillingMethodHistory[i] != g_CalibrationData.m_wFillingMethod[i])
        {
            if(g_CalibrationData.m_wFillingMethod[i] == FILLING_METHOD_PULSING_MODE)
            {            
                if((fdTelnet >0)&& (g_nTSMDebug.m_bPulsingDebug)) 
                iprintf("\n change in filling method for comp %d",i+1);
                g_bCalibratePulsing[i] = TRUE; 
            }
            g_nFillingMethodHistory[i] = g_CalibrationData.m_wFillingMethod[i];
        }
    }

    for(i= 0; i < g_CalibrationData.m_nComponents; i++)
    {
       if(g_CurrentRecipe.m_fPercentage[i] != g_fCurrentRecipePercentageHistory[i])
        {
            if(g_CalibrationData.m_wFillingMethod[i] == FILLING_METHOD_PULSING_MODE)
            {            
                if((fdTelnet >0)&& (g_nTSMDebug.m_bPulsingDebug)) 
                iprintf("\n percentage change in filling method for comp %d",i+1);
                g_bCalibratePulsing[i] = TRUE; 
            }
            g_fCurrentRecipePercentageHistory[i] = g_CurrentRecipe.m_fPercentage[i]; 
        }
    }
 }



//////////////////////////////////////////////////////
// PulsingTargetReached( void )          
// checks if the pulsing target has been reached.
//////////////////////////////////////////////////////
BOOL PulsingTargetReached(unsigned int CompNo )
{
    BOOL    bAtTarget = FALSE;
    int nCompIndex;
    float fComponentWeightError,fDiffgrams;
    nCompIndex = CompNo - 1;    //index is 0 based.
    fComponentWeightError = g_fComponentTargetWeight[nCompIndex] - g_fComponentActualWeight[nCompIndex];
    if(g_bFirstPulse)
    {
        if(g_bCalibratePulsing[g_cCompNo-1])
        {
            g_bGotHalfGramTime = FALSE;
            if(g_bAutoCycleFlag)
            g_nPulseTime = INITIAL_PULSE_TIME_FOR_AUTOCYCLE;
            else
            g_nPulseTime = INITIAL_PULSE_TIME;
            g_nPulseTimeForMinWeight[nCompIndex] = g_nPulseTime;
            if(fdTelnet > 0 && g_nTSMDebug.m_bPulsingDebug)
            {
                printf("\n component %d initial pulse time is %d",CompNo,g_nPulseTime);  
            }
        }
        else
        {
             g_nPulseTime = g_nPulseTimeForMinWeight[nCompIndex]; //nbb--todo--check this
        }
        
        g_bFirstPulse = FALSE;
        g_fComponentActualWeightHistory[nCompIndex] = g_fComponentActualWeight[nCompIndex];
    }
    else
    {
        fDiffgrams = 1000.0*(g_fComponentActualWeight[nCompIndex] - g_fComponentActualWeightHistory[nCompIndex]);
        if(fdTelnet > 0 && g_nTSMDebug.m_bPulsingDebug)
        {
            printf("\n component %d difference is %4.3f target %3.1f  act wt %3.1f time is %d",CompNo,fDiffgrams,1000*g_fComponentTargetWeight[nCompIndex],1000.0f*g_fComponentActualWeight[nCompIndex],g_nPulseTime);  
        }
        if(g_bCalibratePulsing[g_cCompNo-1])
        {
            g_fRequiredPulseWeightInGrams[nCompIndex] = (1000 * 0.1 * g_fComponentTargetWeight[nCompIndex])/5;
            if(g_fRequiredPulseWeightInGrams[nCompIndex] < 0.5)
            g_fRequiredPulseWeightInGrams[nCompIndex] = 0.6;
            if(g_CalibrationData.m_nBlenderType == OPTIMIX150)
            g_fRequiredPulseWeightInGrams[nCompIndex] = 0.6;
            if(fdTelnet > 0 && g_nTSMDebug.m_bPulsingDebug)
            printf("\n component %d required weight per pulse is %3.4f",g_cCompNo,g_fRequiredPulseWeightInGrams[nCompIndex]);
            if((fDiffgrams > 0.6)&&(!g_bGotHalfGramTime))
            {
                g_nHalfGramTime[nCompIndex] = g_nPulseTime;
                g_bGotHalfGramTime = TRUE;  
                if(fdTelnet > 0 && g_nTSMDebug.m_bPulsingDebug)
                printf("\n g_nHalfGramTime is %d",g_nHalfGramTime[nCompIndex]);
            }
            if(fDiffgrams > g_fRequiredPulseWeightInGrams[nCompIndex])
            {
                g_bCalibratePulsing[g_cCompNo-1] = FALSE;
                g_nPulseTimeForMinWeight[nCompIndex] = g_nPulseTime;
                if(fdTelnet > 0 && g_nTSMDebug.m_bPulsingDebug)
                {
                    iprintf("\n component %d min grams achieved",CompNo);
                }
            }
            else
            {
                g_nPulseTime = g_nPulseTime + PULSE_INCREMENT_TIME;  // increase time
            }
        }
        else
        {
         g_nPulseTime = g_nPulseTimeForMinWeight[nCompIndex];
        }
        
    fDiffgrams = 1000.0*(g_fComponentActualWeight[nCompIndex] - g_fComponentActualWeightHistory[nCompIndex]);
    g_fComponentActualWeightHistory[nCompIndex] = g_fComponentActualWeight[nCompIndex];
    }
    if((fComponentWeightError * 1000) < g_fRequiredPulseWeightInGrams[nCompIndex])
    {
        g_nPulseTime = g_nHalfGramTime[nCompIndex];
    }
    if((fComponentWeightError <= 0)||(fComponentWeightError < 0.00025))  // error negative or 0??
    {
       bAtTarget = TRUE; 
       g_nPulseTime = g_nPulseTimeForMinWeight[nCompIndex]; // revert back for next time
    }
    return(bAtTarget);
}


//////////////////////////////////////////////////////
// RecalibratePulsingForComponents( void )          
// initiates pulsing recalibration
//////////////////////////////////////////////////////

void RecalibratePulsingForComponents( void )
{
    unsigned int i;
    for(i= 0; i < g_CalibrationData.m_nComponents; i++)
    {
        if(g_CalibrationData.m_wFillingMethod[i] == FILLING_METHOD_PULSING_MODE)
        {            
            g_bCalibratePulsing[i] = TRUE; 
        }
    }
}




