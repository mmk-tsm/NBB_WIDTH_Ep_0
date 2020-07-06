//////////////////////////////////////////////////////
// P.Smith                      6/12/07             
// round robin calculation for component weights
//
// P.Smith                      7/12/07             
// Use batches in kg/hr average as the no of samples that are requirewd
//
// P.Smith                      9/1/08             
// added ResetAllWeightRoundRobin, remove printfs
//
// P.Smith                      26/9/08             
// name change g_fComponentActualWeightBufferRR
//
// P.Smith                      16/10/08             
// correct compiler warnings
//////////////////////////////////////////////////////


#include "General.h"
#include "BatVars.h"
#include "KghCalc.h"
#include "ConversionFactors.h"
#include "BatchCalibrationFunctions.h"
#include "SetpointFormat.h"
#include "BatchMBIndices.h"
#include "BatVars2.h"
#include <basictypes.h>
#include <stdio.h>

extern CalDataStruct    g_CalibrationData;
extern structSetpointData   g_CurrentRecipe;


//////////////////////////////////////////////////////
// CalculateComponentRoundRobinWeight( void )           from ASM = CALCRROBINFORWEIGHT(loosely based)
// Routine to calculate the round robin component weight for component n
//
//////////////////////////////////////////////////////
void CalculateComponentRoundRobinWeight( int nComp )
{
    int nIndex;
    unsigned int i;
   float fSum = 0.0f;
    float   fAvg = 0.0f;
    nIndex = nComp-1;    // work off index of 0
        // Check initially that there avg. factor is not greater than the R.R. buffer size.


            // move batch kg/h value into round robin
            g_fComponentActualWeightBufferRR[nIndex][g_nComponentActualWeightRRIndex[nIndex]] = g_fComponentActualWeight[nIndex];

            // increment index or pointer.
            g_nComponentActualWeightRRIndex[nIndex]++;
            if(g_nComponentActualWeightRRIndex[nIndex] >= g_CalibrationData.m_nBatchesInKghAvg)
                g_nComponentActualWeightRRIndex[nIndex] = 0;

            // increment summation counter up to max rr size.
            if(g_nComponentActualWeightRRSummationCounter[nIndex] < g_CalibrationData.m_nBatchesInKghAvg)
                g_nComponentActualWeightRRSummationCounter[nIndex]++;     

            // assumes summation counter has no. of elements in RR.
            for(i = 0; i < g_nComponentActualWeightRRSummationCounter[nIndex]; i++)
            {
                fSum += g_fComponentActualWeightBufferRR[nIndex][i]; // add each sample to sum.
            }

            if(g_nComponentActualWeightRRSummationCounter[nIndex] > 0)
                fAvg = fSum / g_nComponentActualWeightRRSummationCounter[nIndex];     // R.R. avg.

            g_fComponentActualWeightRR[nIndex] = fAvg;    //

            // convert to English units.
            g_fComponentActualWeightRRI[nIndex] = fAvg * THROUGHPUT_CONVERSION_FACTOR; //RRTPTLBH
}



//////////////////////////////////////////////////////
// ResetKgHRoundRobin( void )           from ASM = RESRROB (loosely based)
// Routine to reset the round robin for kg/h.
//  
//
//////////////////////////////////////////////////////
void ResetComponentWeightRoundRobin( int nComp, int NoOfResets )
{
int i,nIndex;

    nIndex = nComp-1;
    g_nComponentActualWeightRRIndex[nIndex] = 0;
    g_nComponentActualWeightRRSummationCounter[nIndex] = 0;
        // clear each element in RR.
        for(i = 0; i < MAX_KGH_RR_SAMPLES; i++)
        {
            g_fComponentActualWeightBufferRR[nIndex][i] = 0.0; // clear each sample
        }
}

//////////////////////////////////////////////////////
// ResetAllWeightRoundRobin( void )           asm RESETALLWGTRROBIN
// Routine to reset the weight round robin for all components
//  
//
//////////////////////////////////////////////////////

void ResetAllWeightRoundRobin( void )
{
int i,j;
    for(j = 0; j < MAX_COMPONENTS; j++)
    { 
        g_nComponentActualWeightRRIndex[j] = 0;
        g_nComponentActualWeightRRSummationCounter[j] = 0;
        // clear each element in RR.
        for(i = 0; i < MAX_KGH_RR_SAMPLES; i++)
        {
            g_fComponentActualWeightBufferRR[j][i] = 0.0; // clear each sample
        }
    }
}


/*

RESETALLWGTRROBIN:
        LDAB    #MAXCOMPONENTNO         ; NO OF COMPONENTS
        LDAA    #2                      ; RESET OF RR NO
        LDX     #RESETWEIGHTRROBINCMP1  ; FLAG TO IGNORE CPI ERROR.
REPRES  STAA    0,X
        AIX     #1
        DECB
        BNE     REPRES                  ; REPEAT RESET.
        RTS
        
*/



