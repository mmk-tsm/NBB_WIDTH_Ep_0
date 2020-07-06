// P.Smith                          17/9/08
// liquid additive LiquidAdditiveCommand implemented, check for start / stop command.
// if start set d/a to STARTLIQUIDCALIBRATIONPERCENTAGE of total output (now 0.25)
// if stop set liquid d/a to 0.
//
// P.Smith                          1/10/08
// a problem has been seen on the liquid additive calculation of kg/hr when in g/m
// mode of operation.
// In auto, calculation kg/hr from g/m set, line speed, also make sure to divide result
// by 1000.
// in manual use rr g/m value.
// in kg/hr mode, in auto, use kg/hr set point, in manual, use kg/hr actual value.
// Correct copy of kg/hr value to modbus
//
//
// P.Smith                          3/10/08
// in SetLiquidAdditiveControlOutput allow for regrind component
// if regrind component is used, recalculate the liquid additive, la / total % * 100
// also use kg/hr value in manual and g/m
//
// P.Smith                          10/10/08
// liquid additive kg/hr calculation is calculated and stored in a global g_fKgperHour so
// that it can be accessed.
//
// P.Smith                          14/10/08
// remove fKgperHour, add scaling factor for liquid kg/hr setpoint, normally 1.0
// add one minute to g_nLiquidOnTimeCtr every time command is activated
// copy BATCH_SUMMARY_LIQUID_ADDITIVE_ON_TIMER to modbus table.
// set fAddition to 0.0
//
// P.Smith                          22/10/08
// set liquid addtive ctr g_nLiquidOnTimeCtr to 0 when liquid additive output is 
// switched off. 
//////////////////////////////////////////////////////

//*****************************************************************************
// INCLUDES
//*****************************************************************************
#include "General.h"
#include "BatVars.h"
#include "BatVars2.h"
#include "ConversionFactors.h"       
#include "BatchCalibrationFunctions.h"
#include "SetpointFormat.h"
#include "BatchMBIndices.h"   
#include "Conalg.h"
#include "Alarms.h"
#include "TSMPeripheralsMBIndices.h"
#include "Lspeed.h"
#include "BlRep.h"
#include <Stdio.h>
#include "Eventlog.h"


extern  CalDataStruct   g_CalibrationData;                                                                
extern  structSetpointData  g_CurrentRecipe;




//////////////////////////////////////////////////////
// SetLiquidAdditiveControlOutput( void )         
// sets liquid additive d/a output depending on the kg/hr set point
//
//////////////////////////////////////////////////////

void SetLiquidAdditiveControlOutput( void )
{
    float fLiquidAdditivePercentage;
    if(g_CalibrationData.m_bLiquidEnabled && g_bLiquidControlAllowed)
    {
       if((g_CurrentRecipe.m_fPercentage[g_nLiquidAdditiveComponent-1] > 0.0001f) && (g_nExtruderDAValue != 0)) //not 0
       {
           g_fLiquidKgPerHourPerPercentOutput = g_CalibrationData.m_fLiquidAdditiveKgPerHour / 25.0f;
           if(g_CalibrationData.m_nBlenderMode == MODE_KGHCONTROLMODE)
           {
               if(g_bManualAutoFlag == AUTO)       // 
               {
                   g_fKgperHour = g_CurrentRecipe.m_fTotalThroughput; 
               }
               else
               {
                   g_fKgperHour = g_fRRThroughputKgPerHour; 
               }
           }
           else //g/m
           if(g_bManualAutoFlag == AUTO)       // 
           {
               g_fKgperHour = (g_CurrentRecipe.m_fTotalWeightLength * g_fLineSpeed5sec * 60.0f)/1000.0f; 
           }
           else
           {
               g_fKgperHour = g_fRRThroughputKgPerHour; 
           }           

           if(g_CurrentRecipe.m_nRegrindComponent == 0)   // Is there regrind??
           {
               fLiquidAdditivePercentage = g_CurrentRecipe.m_fPercentage[g_nLiquidAdditiveComponent-1];
           }
           else
           {
               fLiquidAdditivePercentage = (100.0 * g_CurrentRecipe.m_fPercentage[g_nLiquidAdditiveComponent-1])/(100.0 + g_CurrentRecipe.m_fPercentage[g_CurrentRecipe.m_nRegrindComponent-1]);
           }
         
            g_fKgperHour = g_fKgperHour * fLiquidAdditivePercentage/100.0f;
            g_fKgperHour = g_fKgperHour * g_CalibrationData.m_fLiquidAdditiveScalingFactor;  
            g_nLiquidAdditiveOutputDAValue =(int) (((float)MAXEXTRUDERDTOA) * (g_fKgperHour / g_fLiquidKgPerHourPerPercentOutput)/100.0f);      
        }
        else  // (set liquid additive % is 0)
        {
            g_nLiquidAdditiveOutputDAValue = 0;  // set liquid additive output to 0
        }
    }  
}

//////////////////////////////////////////////////////
// LiquidAdditiveCommand( void )         
// one hz program 
//
//////////////////////////////////////////////////////

void LiquidAdditiveOnehz( void )
{
    if(g_bLiquidEnabledHistory != g_CalibrationData.m_bLiquidEnabled)
    {
        g_bLiquidEnabledHistory = g_CalibrationData.m_bLiquidEnabled;
        g_bPctFlag = TRUE;   // force sequence table to be reformed.
    }

    if(g_CalibrationData.m_bLiquidEnabled)
    {
        g_nLiquidAdditiveComponent = g_CalibrationData.m_nComponents;
        g_CurrentRecipe.m_fPercentageLiquid = g_CurrentRecipe.m_fPercentage[g_CalibrationData.m_nComponents-1];
        SetLiquidAdditiveControlOutput();
        g_fLiquidAdditiveOutputPercentage = 100.0 * ((float)g_nLiquidAdditiveOutputDAValue / (float)MAXEXTRUDERDTOA);
    }

}


//////////////////////////////////////////////////////
// LiquidAdditiveCommand( void )         
// Liquid additive command starts and stops liquid pump
//
//////////////////////////////////////////////////////

void LiquidAdditiveCommand( int nTemp )
{
    if(g_CalibrationData.m_bLiquidEnabled)
    {
        if(nTemp == STARTLIQUIDCALIBRATION)        
        {
            g_nLiquidAdditiveOutputDAValue = (int) (MAXEXTRUDERDTOA * STARTLIQUIDCALIBRATIONPERCENTAGE);
            g_bLiquidControlAllowed = FALSE;         // stop normal control
        }
        else
        if(nTemp == ENDLIQUIDCALIBRATION)        
        {
            g_nLiquidAdditiveOutputDAValue = 0;
            g_bLiquidControlAllowed = TRUE;         // stop normal control
            g_nLiquidOnTimeCtr = 0;
        }
        else if(nTemp == RUNLIQUIDCALIBRATIONFORONEMINUTE)        
        {
            g_nLiquidAdditiveOutputDAValue = (int) (MAXEXTRUDERDTOA * STARTLIQUIDCALIBRATIONPERCENTAGE);
            g_bLiquidControlAllowed = FALSE;         // stop normal control
            g_nLiquidOnTimeCtr += LIQUIDADDITIVEONTIME;
        }
    }
}



//////////////////////////////////////////////////////
// LiquidAdditiveOnTimeExpired( void )         
// C
//
//////////////////////////////////////////////////////

void LiquidAdditiveOnTimeExpired( void )
{
    g_bLiquidControlAllowed = TRUE;         // allow normal control
    g_nLiquidAdditiveOutputDAValue = 0;  // set liquid additive output to 0
}

//////////////////////////////////////////////////////
// CopyLiquidAdditiveToMB( void )         
// Copy liquid additive details to modbus table.
//
//////////////////////////////////////////////////////

void CopyLiquidAdditiveToMB( void )
{
    g_arrnMBTable[BATCH_SUMMARY_LIQUID_ADDITIVE_OUTPUT_DTOA] = g_nLiquidAdditiveOutputDAValue;  
    g_arrnMBTable[BATCH_SUMMARY_LIQUID_ADDITIVE_OUTPUT_PERCENTAGE] = (int)(g_fLiquidAdditiveOutputPercentage * 100);  //
    g_arrnMBTable[BATCH_SUMMARY_LIQUID_ADDITIVE_SET_KGHR] = (int)((g_fKgperHour + 0.005f) * 100.0f);
    g_arrnMBTable[BATCH_SUMMARY_LIQUID_ADDITIVE_ON_TIMER] = g_nLiquidOnTimeCtr /PIT_FREQUENCY;
}


//////////////////////////////////////////////////////
// Estimate Liquid additive weight.        
// As the liquid additive control is open loop, it is necessary to 
// estimate the weight for this component.
// it is assumed that the liquid component is the last component.
// liquid weight  = Liquid % * (total component weights - regrind)/100-liquid %
//
//////////////////////////////////////////////////////

void EstimateLiquidComponentWeight( void )
{
    float fLiquidAdditivePercentage,fTemp,fAddition,fWeight;
    unsigned int i;
    int nRegCompIndex = g_CurrentRecipe.m_nRegrindComponent - 1;    //Regrind component index.
 
    fAddition = 0.0f;
    if(g_CalibrationData.m_bLiquidEnabled)
    {
        for(i = 0; i < g_nLiquidAdditiveComponent-1 && i < MAX_COMPONENTS; i++)  // .
        {
            fAddition += g_fComponentActualWeight[i]; 
        }
        if(g_CurrentRecipe.m_nRegrindComponent != 0)   // Is there regrind??
        {
        // subtract wt of regrind component.
           fWeight = fAddition - g_fComponentActualWeight[nRegCompIndex];
        }
        else
        {
            fWeight = fAddition;
        }

        fLiquidAdditivePercentage = g_CurrentRecipe.m_fPercentage[g_nLiquidAdditiveComponent-1]; // read liquid %
        fTemp = fLiquidAdditivePercentage * fWeight / (100.0 - g_CurrentRecipe.m_fPercentage[g_CalibrationData.m_nComponents-1]);
        g_fComponentActualWeight[g_nLiquidAdditiveComponent-1] = fTemp;
        g_fComponentTargetWeight[g_nLiquidAdditiveComponent-1] = fTemp;

    }
}





