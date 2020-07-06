//////////////////////////////////////////////////////
// GPMCalc.c
//
// Calculate grams per meter calculation.
//
// P.Smith                              05-01-2005
// BatchGPMCalc(),CopyGPMToModbus,CalculateGPMRoundRobin
// ResetGPMRoundRobin
// If start up ResetGPMRoundRobin
//
// P.Smith                            12/10/05  
// ensure that round robin is reset in manual & auto startup
// & if control average has changed.
//
// P.Smith                            7/11/05  
// Call CopyGPMToModbus() after round robin gram per meter has been calculated.
// not beforehand.
//
//
// P.Smith                            10/1/06  
// remove nTemp,nTemp1,nTemp2
// 
// P.Smith                      23/2/06
// first pass at netburner hardware conversion.
// included  <basictypes.h>
// 
// P.Smith                      24/2/06
// changed unions IntsAndLong to WordAndDWord
//
// P.Smith                            19/5/06  
// only run round robin calculation if (g_nEstCtrUse == 0) && (!g_bRunEst))  .
//
// P.Smith                            19/5/06 
// avoid a divide by zero in the g/m calc                          
//
//
// P.Smith                            25/5/06 
// multiply modbus data by conversion factor.                        
//
// P.Smith                            28/6/06 
// check for MINIMUM_LENGTH when doing g/m calc
//
// P.Smith                            17/11/06 
// added calculation of g/m from line speed over comms
//
// P.Smith                            11/1/07 
// name change g_CalibrationData.m_nLineSpeedSignalType
//
// P.Smith                            6/2/07 
// Remove printfs
//
// P.Smith                            3/5/07 
// added CalculateSetGramsPerMeter
//
// P.Smith                            9/7/07 
// correct CalculateSetGramsPerMeter, ensure that calculation only takes place
// if the blender is in micron mode of operation.
//
// M.McKiernan                      17/9/07 
// Removed g_arrnMBTable[] & g_arrnMBTableFlags[], now in BatVars2
//
// P.Smith                      14/11/07 
// added grams per square meter functionality to CalculateGramsPerSquareMeter
// CalculateSetGramsPerMeterFromGramsPerSqMeter (GRAMPERQTOGPM) added 
//
// P.Smith                      19/11/07 
// Remove ResetGPMRoundRobin in manual mode
//
//                                                                           ;
// P.Smith                      5/12/07             
// No gpm calculation if control is suspended.                
//
//        
// P.Smith                          30/1/08
// correct compiler warnings
//
//        
// P.Smith                          30/1/08
// call CopyGPMToModbus so that the set gpm can be seen on the screen
// name change to BATCH_GPMMICRONENTRY to MODE_GPMMICRONENTRY
// in CalculateSetGramsPerMeterFromGramsPerSqMeter check for MODE_GPSQMENTRY or MODE_GP12SQFEET
// This was causing a problem in that the blender was calculating the g/m value on reset
// and getting a value of 0 even when it was not in gp12sq feet mode.
//
//        
// P.Smith                          12/6/08
// Correct reset of reset of grams per meter round robin on a change in the control average
// This was calling the reset of the kg/h round robin instead
// in BatchGPMCalc, no entry if g_nDisableDTAPerKgCtr is non zero
// Remove g_CalibrationData.m_nBatchesInKghAvg check for greater than max value
//
// P.Smith                          24/6/08
// remove g_arrnMBTable 
//
// P.Smith                          10/9/08
// BatchGPMCalc if liquid additive is enabled calculate g/m using line speed and kg/hr figure.
//
//////////////////////////////////////////////////////

//*****************************************************************************
// INCLUDES
//*****************************************************************************
#include "General.h"
#include "BatVars.h"
#include "GPMCalc.h"
#include "ConversionFactors.h"
#include "BatchCalibrationFunctions.h"
#include "SetpointFormat.h"
#include "BatchMBIndices.h"
#include "BatVars2.h"
#include <basictypes.h>
#include "Lspeed.h"
#include <stdio.h>


extern CalDataStruct    g_CalibrationData;
extern structSetpointData   g_CurrentRecipe;
extern  structSetpointData      g_TempRecipe;





//////////////////////////////////////////////////////
// BatchGPMCalc()               ASM = BATCHGPMCALC
// calculates grams per meter from batch weight and length
//
// If line speed is over comms, then calculation of g/m is 
// (kg/h  * 1000) / (line speed * 60)
//////////////////////////////////////////////////////


void BatchGPMCalc()   // Check for round robin reset.
{
    if((g_nSuspendControlCounter == 0) && (g_nDisableDTAPerKgCtr ==0))
    {
        if((g_CalibrationData.m_nLineSpeedSignalType == COMMUNICATION_LINE_SPEED)|| ((g_CalibrationData.m_bLiquidEnabled)))
        {
            g_fWeightPerMeter = (g_fThroughputKgPerHour * 1000.0f) / (g_fLineSpeed5sec * 60.0f);
            g_fLbsPer1000Feet = g_fWeightPerMeter * WEIGHT_PER_LENGTH_CONVERSION_FACTOR;  // ASM = CONVGPM
        }
        else
        {
            if(g_fBatchLength  > MINIMUM_LENGTH)            // 
            g_fWeightPerMeter = 1000 * g_fPreviousBatchWeight / g_fBatchLength; // calculate g/m                   
            g_fLbsPer1000Feet = g_fWeightPerMeter * WEIGHT_PER_LENGTH_CONVERSION_FACTOR;  // ASM = CONVGPM
        }
    
        if(g_bManualAutoFlag)  // round robin only reset if Auto.
        {    
            if(g_bExtruderControlStartUpFlag)       // if start up check if inside the alarm band.
            {
                ResetGPMRoundRobin();
            }
        }
        else
        {
//        ResetGPMRoundRobin();    //nbb--todo-- decide if this should go back in
        }
        if(g_CalibrationData.m_nControlAvgFactor != g_nControlAvgFactorGpmHistory)
        {
            g_nControlAvgFactorGpmHistory = g_CalibrationData.m_nControlAvgFactor;  
            ResetGPMRoundRobin();
        }
        if( (g_nEstCtrUse == 0) && (!g_bRunEst))  // sbb--todo-- review this.
        {
            CalculateGPMRoundRobin();               // GENRRKGH ( generate r.robin g/m )
        }    
        CalculateGramsPerSquareMeter();         // ASM = GPMTOGRAMPERSM 
        CopyGPMToModbus();                      // ASM = CPYGPMMB 
    }
}


//////////////////////////////////////////////////////
// CopyGPMToMB( void )          from ASM = CPYKGHMB
// Copies g/m value into MB table.
//  
//////////////////////////////////////////////////////
void CopyGPMToModbus( void )
{
    float fValue;
    union WordAndDWord uValue;
    fValue = g_fWeightPerMeterRR;            
    fValue *=    g_fWeightPerLengthConversionFactor;          
    fValue += 0.005;        // for rounding to nearest 2nd decimal digit.
    uValue.lValue = (long)(fValue * 100.0f);
    g_arrnMBTable[BATCH_SUMMARY_TOTAL_WEIGHT_LENGTH] = uValue.nValue[0];       // Ms..
    g_arrnMBTable[BATCH_SUMMARY_TOTAL_WEIGHT_LENGTH + 1] = uValue.nValue[1];   // ls.
}


//////////////////////////////////////////////////////
// CalculateGPMRoundRobin( void )           
//  
//
// P.Smiht                          02-03-2005
// First pass.
//////////////////////////////////////////////////////
void CalculateGPMRoundRobin( void )
{
   unsigned int i;
   float fSum = 0.0f;
    float   fAvg = 0.0f;

    // Check initially that there avg. factor is not greater than the R.R. buffer size.

    if(!g_bFirstBatch)          // FIRSTBATCH - only do calculation if not first batch.
    {
        // move batch g/m value into round robin
        g_fGPMRRBuffer[g_nGPMRRIndex] = g_fWeightPerMeter;

        // increment index or pointer.
        g_nGPMRRIndex++;
        if(g_nGPMRRIndex >= g_CalibrationData.m_nControlAvgFactor)
            g_nGPMRRIndex = 0;

        // increment summation counter up to max rr size.
        if(g_nGPMRRSummationCounter < g_CalibrationData.m_nControlAvgFactor)
            g_nGPMRRSummationCounter++;     

        // assumes summation counter has no. of elements in RR.
        for(i = 0; i < g_nGPMRRSummationCounter; i++)
        {
            fSum += g_fGPMRRBuffer[i]; // add each sample to sum.
        }

        if(g_nGPMRRSummationCounter > 0)
            fAvg = fSum / g_nGPMRRSummationCounter;     // R.R. avg.

        g_fWeightPerMeterRR = fAvg;    //RRGMPM
        
        // convert to English units.
        g_fLbsPer1000Feet = fAvg * WEIGHT_PER_LENGTH_CONVERSION_FACTOR; //  RRGMPMI
    }
}


//////////////////////////////////////////////////////
// ResetGPMRoundRobin( void )           from ASM = RESRROB (loosely based)
// Routine to reset the round robin for g/m.
//  
//
// Exit: 
// 
//
// M.McKiernan                          05-01-2005
// First pass.
//////////////////////////////////////////////////////
void ResetGPMRoundRobin( void )
{
int i;
    g_nGPMRRIndex = 0;
    g_nGPMRRSummationCounter = 0;
        // clear each element in RR.
        for(i = 0; i < MAX_GPM_RR_SAMPLES; i++)
        {
            g_fGPMRRBuffer[i] = 0.0; // clear each sample
        }
}


//////////////////////////////////////////////////////
// void CalculateGramsPerSquareMeter()          ASM = GPMTOGRAMPERSM
//
// grams per square meter = grams per meter  / width set point
//  
//
// P.Smith                              31-3-2005
//////////////////////////////////////////////////////

// --review-- this could be put inline

void CalculateGramsPerSquareMeter( void )  
{
    if(g_CalibrationData.m_nBlenderMode == MODE_GP12SQFEET)
    {
        g_fActualGrammesPerM2 = GP12SQFTTOGPM2CONVERSIONFACTOR * (g_fWeightPerMeterRR / (g_CurrentRecipe.m_fDesiredWidth / 1000.0f));  // g/m / width set
    }
    else
    {
        g_fActualGrammesPerM2 = g_fWeightPerMeterRR / (g_CurrentRecipe.m_fDesiredWidth / 1000.0f);  // g/m / width set
    }
 }

//////////////////////////////////////////////////////
// void CalculateSetGramsPerMeter()          ASM = CALCSETGPM
//
//  
//
// P.Smith                              3-5-2005
//////////////////////////////////////////////////////


void CalculateSetGramsPerMeter( void )  
{
    int i=0,nCompIndex;
    float  fDensityTotal = 0.0f;
    if(g_CalibrationData.m_nBlenderMode == MODE_GPMMICRONENTRY)
    {
        while(g_cSeqTable[i] != SEQDELIMITER && i < MAX_COMPONENTS) // SEQDELIMITER indicates table end
        {
            nCompIndex = (int)(g_cSeqTable[i] & 0x0F) - 1;  // get component no. from table, mask off the m.s. nibble, 0 based.

            if((g_cSeqTable[i] & REGRIND_BIT) == 0 )
            {
                fDensityTotal +=  (g_CurrentRecipe.m_fPercentage[nCompIndex]/100) * g_CurrentRecipe.m_fDensity[nCompIndex]; 
            }
            i++;
        }
    g_CurrentRecipe.m_fTotalWeightLength = (2.0f * (g_CurrentRecipe.m_fDesiredWidth/1000.0f) * fDensityTotal * g_CurrentRecipe.m_fFilmThickness);
    }
}





//////////////////////////////////////////////////////
// void CalculateSetGramsPerMeterFromGramsPerSqMeter()          ASM = GRAMPERQTOGPM
// Calculates grams per meter from grams per square meter
//  
//////////////////////////////////////////////////////


void CalculateSetGramsPerMeterFromGramsPerSqMeter( void )  
{
    union WordAndDWord;


   if( (g_CalibrationData.m_nBlenderMode == MODE_GPSQMENTRY) || (g_CalibrationData.m_nBlenderMode == MODE_GP12SQFEET))
   {

        if(g_CalibrationData.m_nBlenderMode == MODE_GP12SQFEET)
        {
            g_CurrentRecipe.m_fTotalWeightLength = (g_CurrentRecipe.m_fTotalWeightArea * (g_CurrentRecipe.m_fDesiredWidth / 1000.0f))/GP12SQFTTOGPM2CONVERSIONFACTOR;
            g_TempRecipe.m_fTotalWeightLength = g_CurrentRecipe.m_fTotalWeightLength;
//        SetupMttty();
//        printf("\n calculating g/12sq feet width is %3.2f g/12sq sp %3.2f gpm calc %3.2f ",g_CurrentRecipe.m_fDesiredWidth,g_CurrentRecipe.m_fTotalWeightArea,g_CurrentRecipe.m_fTotalWeightLength);
        }
        else
        {
            g_CurrentRecipe.m_fTotalWeightLength = g_CurrentRecipe.m_fTotalWeightArea * (g_CurrentRecipe.m_fDesiredWidth / 1000.0f);
        }
    }
}



/*

MCE12-86
;*****************************************************************************;
; FUNCTION NAME : GPMTOGRAMPERSM                                              ;  
; FUNCTION      : CONVERT GRAMS PER METER TO GRAMS PER SQUARE METER           ;
; INPUTS        : GRAMS PER METER.                                            ; 
;               ; GPM2SPT GRAMS PER SQUARE METER.                             ;
;*****************************************************************************;


GPMTOGRAMPERSM:      
        JSR     CLCREG7
        JSR     CLAREG7
        LDD     #ONE
        STD     AREG
        LDX     WIDTHSP    
        STX     AREG+2
        
        LDD      #SIX
        STD      MATHCREG         ; XXXXXX.YY
        LDX      RRGMPM5+1        ; g/m 
        STX      MATHCREG+2        
        LDX      RRGMPM5+3
        STX      MATHCREG+4
        JSR      FDIV              ; CALCULATE G/M


        LDAA    BLENDERMODE
        CMPA    #GPSQMENTRYIMP
        BNE     GETGPM2           ; GET RESULTING G/M
        
        LDX         #MATHCREG
        JSR         FFDREG

        LDD         #DECIMALPOSITION
        STD         AREG
        LDD         #GP12SQFTTOGPM2CONVERSION1
        STD         AREG+2
        LDD         #GP12SQFTTOGPM2CONVERSION2
        STD         AREG+4
        LDD         #GP12SQFTTOGPM2CONVERSION3
        STD         AREG+6       
        JSR         FMUL  ; CORRECT THIS WAS FDIV
        
        ; ROUND OFF THE RESULT.
        
GETGPM2 LDX         #MATHCREG
        JSR         FFDREG
        JSR         CLAREG7   ;~A = 0
        LDD         #$0055
        STD         AREG+2
        JSR         FADD
        JSR         FPTINC    
        LDD         MATHDREG+3
        STD         GPM2VAR
        LDAA        MATHDREG+5
        STAA        GPM2VAR+2

        RTS

; FUNCTION NAME : GRAMPERQTOGPM                                               ;  
; FUNCTION      : CONVERT GRAMS PER SQUARE METER TO GRAMS PER METER           ;
; INPUTS        : WIDTHSP WIDTH SET POINT.                                    ; 
;               ; GPM2SPT GRAMS PER SQUARE METER.                             ;
;*****************************************************************************;


GRAMPERQTOGPM:      
        JSR     CLRAC7
        LDD     #3
        STD     AREG
        LDX     WIDTHSP     ; WIDTH IN METERS   
        STX     AREG+3


        LDD     #4
        STD     MATHCREG
        LDX    GPM2SPT          ; g/m2 SETPOINT
        STX    MATHCREG+2        
        LDAA   GPM2SPT+2
        STAA   MATHCREG+4
        JSR    FMUL              ; CALCULATE G/M

        LDAA    BLENDERMODE
        CMPA    #GPSQMENTRYIMP
        BNE     GETGPM            ; GET RESULTING G/M
        
        LDX         #MATHCREG
        JSR         FFDREG

        LDD         #DECIMALPOSITION
        STD         AREG
        LDD         #GP12SQFTTOGPM2CONVERSION1
        STD         AREG+2
        LDD         #GP12SQFTTOGPM2CONVERSION2
        STD         AREG+4
        LDD         #GP12SQFTTOGPM2CONVERSION3
        STD         AREG+6       
        JSR         FDIV     ; CORRECT THIS WAS FMUL
        
        ; ROUND OFF THE RESULT.
        
GETGPM  LDX         #MATHCREG
        JSR         FFDREG
        JSR         CLAREG7   ;~A = 0
        LDD         #$0055
        STD         AREG+2
        JSR         FADD
        JSR         FPTINC 
        LDD         MATHDREG+2
        STD         WTPMSP
        LDE         MATHDREG+4
        STE         WTPMSP+2
        RTS


*/

