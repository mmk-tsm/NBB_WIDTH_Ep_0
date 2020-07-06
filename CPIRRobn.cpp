//////////////////////////////////////////////////////
// Filename = CPIRRobn.c
//
// CPI Round robin routines
//
//
// M.McKiernan                          12-08-2004
// First Pass
//
// P.Smith                              10/1/06
// remove Bool bResult;
//
// P.Smith                      23/2/06
// first pass at netburner hardware conversion.
// Bool -> BOOL
//#include <basictypes.h>
//#include <string.h>
//
// P.Smith                      4/12/06
// Removed unused comments
//
// P.Smith                      11/6/07
// added ResetRoundRobinFlowRateForAllComponents to reset round robin.
//
// M.McKiernan                      17/9/07
// Removed g_arrnMBTable[] & g_arrnMBTableFlags[], now in BatVars2
//
// P.Smith                          24/6/08
// remove g_arrnMBTable
//
// P.Smith                          24/8/09
// added GetRRobinData to get round robin data
// added CheckForValidFlowRate
// do not run check if vacuum transport delay is 99
// if the flow rate exceeds the max deviation allowed then ignore it
// allow flow rate debug via Telnet by checking a debug flag.
// if the blender has filled by weight or the flow rate is supposed to
// to be accepted then the flow rate check is not run.
//
// P.Smith                          26/8/09
// modify software so that if the component is being ignored then do not run the
// flow rate check.
// this normally happens when the component is filling.
//
// P.Smith                          17/10/09
// added GetFillingStage when copying back multistage data, this was not being called.
// before the data was copied back.
//
// P.Smith                          20/10/09
// check m_bFlowRateCheck to determine if flow rate check should be done.
// this has not been done on the panel yet.
// do not run if transport delay is 99.
//
// P.Smith                          27/11/09
// check g_CalibrationData.m_bFlowRateCheck to determine if the flow rate check should be done.
//
// P.Smith                          12/2/10
// only do the flow rate check if stage is 1st stage,ie if the stage no is 0 or 3

//////////////////////////////////////////////////////

//*****************************************************************************
// INCLUDES
//*****************************************************************************
//nbb #include <hc16.h>
#include <Stdio.h>
//nbb #include <StdLib.h>

//nbb #include "Hardware.h"
#include "InitialiseHardware.h"
#include "SerialStuff.h"
#include "General.h"
#include "ConversionFactors.h"

#include "BatchCalibrationFunctions.h"
#include "BatchMBIndices.h"
#include "ConfigurationFunctions.h"
#include "Foreground.h"
#include "16R40C.h"
#include    "PITWeigh.h"
#include    "BatVars.h"
#include    "BatVars2.h"
#include "CycleMonitor.h"
#include "ChkFill.h"
#include "MBProgs.h"
#include "SetpointFormat.h"
#include "RRFormat.h"
#include "CPIRRobin.h"
#include <basictypes.h>
#include <string.h>

// Locally declared global variables
// Externally declared global variables
// Data
structCPIRRData g_WorkCPIRRData;    // Temporary or working structure (all actions carried out on it)

//structCPIRRData   g_MainCPIRRData;
//structCPIRRData   g_Stage2CPIRRData;
//structCPIRRData   g_Stage3CPIRRData;


//extern float  g_fWeightAtoDCounts;

extern  int g_nKeyCode;
extern  int g_nLanguage;
extern  int g_nGeneralSecondTimer;
extern  structMeasurementUnits  g_MeasurementUnits;
extern  CalDataStruct   g_CalibrationData;



//////////////////////////////////////////////////////
// CalculateCPIRoundRobin( int nComponent )         from ASM = CALCCPIRROBIN (loosely based)
//
// Calculates the CPI round robin for specified component, basically adds current CPI value into RR structure.
// Note there are 3 RR structures for each component, one used depends on fill stage.
// The routines copy the relevant RR structure into the Working RR structure and perform operations on it,
// and at end copy result back to relevant RR structure.
//
// EXIT: return a TRUE if the result of the round robin (r.r. average) should be used.
//       If true, the round robin result is stored in g_ffCPI_RR.
//
// M.McKiernan                          11-08-2004
// First pass.
//
// P.Smith                              11/05/07
// check g_WorkCPIRRData.m_ComponentCPIRRData[nCompIndex].m_nResetCPIRRCtr to allow the flow rate
// round robin to be reset a number of times, decrement after it is detected.
//////////////////////////////////////////////////////
BOOL CalculateCPIRoundRobin( int nComponent )
{
    BOOL bUseRRCPI = FALSE;     // flag indicates if result of RR should be used.
    float fCompWtError,fCompWtLimit;
    double ffCPI;
    int nStage;
    int nCompIndex = nComponent - 1;    // index for component arrays is 0 based.

    GetRRobinData(nComponent);

    // is ignore flag set??
    if(g_WorkCPIRRData.m_ComponentCPIRRData[nCompIndex].m_bIgnoreCPIRR)
    {
        g_WorkCPIRRData.m_ComponentCPIRRData[nCompIndex].m_bIgnoreCPIRR = FALSE;    // clear ignore flag.
        // if ignore, use previous RR average value.
        ffCPI = g_WorkCPIRRData.m_ComponentCPIRRData[nCompIndex].m_ffCPIAverage;
    }
    else    // ignore not set.
    {
        // is flag set to reset the round robin.
        if(g_WorkCPIRRData.m_ComponentCPIRRData[nCompIndex].m_nResetCPIRRCtr !=0 )
        {
            g_WorkCPIRRData.m_ComponentCPIRRData[nCompIndex].m_nResetCPIRRCtr --;
            ResetWorkCPIRoundRobin( nComponent );       // reset CPI round robin
            CalculateWorkCPIRoundRobin( nComponent );   // calculate round robin (adds in current CPI value)
        }
        else // i.e. reset r.r. not set
        {
            if(g_WorkCPIRRData.m_ComponentCPIRRData[nCompIndex].m_nSummationCounter)
            {
// //asm = CALCPERERROR - now expanded inline - next 2 lines.
            fCompWtError = g_fComponentTargetWeight[nCompIndex] - g_fComponentActualWeight[nCompIndex];
            fCompWtLimit = g_fComponentTargetWeight[nCompIndex] * 0.20;  // Weight limit = 20% of target.

            if(fCompWtError < fCompWtLimit)
                {
                    //asm = USECPI
                    // clear error counter
                    g_WorkCPIRRData.m_ComponentCPIRRData[nCompIndex].m_nErrorCounter = 0;
                    CalculateWorkCPIRoundRobin( nComponent );   // calculate round robin (adds in current CPI value)
                }
                else  // Weight error >= limit
                {
                    // increment error counter
                    g_WorkCPIRRData.m_ComponentCPIRRData[nCompIndex].m_nErrorCounter++;
                    if(g_WorkCPIRRData.m_ComponentCPIRRData[nCompIndex].m_nErrorCounter >= CPI_THRESHOLD)
                    {
                        ResetWorkCPIRoundRobin( nComponent );       // reset CPI round robin
                        CalculateWorkCPIRoundRobin( nComponent );   // calculate round robin (adds in current CPI value)
                    }
                }
            }
            else // summation counter is currently zero (R.R. empty)
            {   //asm = USECPI
                // clear error counter
                g_WorkCPIRRData.m_ComponentCPIRRData[nCompIndex].m_nErrorCounter = 0;
                CalculateWorkCPIRoundRobin( nComponent );   // calculate round robin (adds in current CPI value)
           }
        }
    }

    ffCPI = g_WorkCPIRRData.m_ComponentCPIRRData[nCompIndex].m_ffCPIAverage;

    nStage =  GetFillingStage( nComponent );    // get stage of filling.

    // copy work CPI round robin structure into the relevant RR structure. (depends on filling stage)
    if(nStage == STAGE_3)
    {
        memcpy( &g_Stage3CPIRRData, &g_WorkCPIRRData, sizeof( g_WorkCPIRRData ) );
    }
    else if(nStage == STAGE_2)
    {
        memcpy( &g_Stage2CPIRRData, &g_WorkCPIRRData, sizeof( g_WorkCPIRRData ) );
    }
    else // assume first or no stage filling.
    {
        memcpy(&g_MainCPIRRData, &g_WorkCPIRRData, sizeof( g_WorkCPIRRData ) );
    }

    if(ffCPI > MIN_CPI_VALUE)
    {
        g_ffCPI_RR = ffCPI;
        bUseRRCPI = TRUE;
    }
    return( bUseRRCPI );
}



//////////////////////////////////////////////////////
// CalculateWorkCPIRoundRobin( nComponent )         from ASM = CALCRROBIN
//
// Adds a sample or element (g_ffCPI) to the Work template of the CPI round robin
// for component nComponent.  Sample added into RR at CPIRRIndex.  Index is then incremented,
// until max and then rolls over to 0.  Samples in RR is incremented up to max value.
// The sum of the samples in the RR is calculated.
// The average of the RR is calculated by dividing the sum by the summation counter (no. of samples in sum)
// CPI (counts per interrupt) are double floats.
//
// M.McKiernan                          11-08-2004
// First pass.
//////////////////////////////////////////////////////
void CalculateWorkCPIRoundRobin( int nComponent )
{
    int nCompIndex = nComponent - 1;    // index is 0 based.
    int nCPIIndex;
    int i;
    int nSampleCount;
   double ffSum = 0.0;
   double ffCPIRR;

    nCPIIndex = g_WorkCPIRRData.m_ComponentCPIRRData[nCompIndex].m_nCPIRRIndex; // index or ptr into CPI buffer.

//from ASM = RRINSERTDATA
    // move g_ffCPI value into CPI round robin for this component.
    g_WorkCPIRRData.m_ComponentCPIRRData[nCompIndex].m_ffCPI[nCPIIndex] = g_ffCPI;

//from aSM = UPDATESAMPLECOUNTERS

    g_WorkCPIRRData.m_ComponentCPIRRData[nCompIndex].m_nCPIRRIndex++;
    if(g_WorkCPIRRData.m_ComponentCPIRRData[nCompIndex].m_nCPIRRIndex >= MAX_CPI_RR_SAMPLES)
        g_WorkCPIRRData.m_ComponentCPIRRData[nCompIndex].m_nCPIRRIndex = 0; // Roll over to 0.

    // increment summation counter up to max rr size.
    if(g_WorkCPIRRData.m_ComponentCPIRRData[nCompIndex].m_nSummationCounter < MAX_CPI_RR_SAMPLES)
        g_WorkCPIRRData.m_ComponentCPIRRData[nCompIndex].m_nSummationCounter++;

//from aSM = SUMMATERR
// assumes summation counter has no. of elements in RR.

    for(i = 0; i < g_WorkCPIRRData.m_ComponentCPIRRData[nCompIndex].m_nSummationCounter; i++)
    {
        ffSum += g_WorkCPIRRData.m_ComponentCPIRRData[nCompIndex].m_ffCPI[i]; // add each sample to sum.
    }

    //store sum into structure.
    g_WorkCPIRRData.m_ComponentCPIRRData[nCompIndex].m_ffCPISummation = ffSum;

//from aSM = RRDIVDATA
    nSampleCount = g_WorkCPIRRData.m_ComponentCPIRRData[nCompIndex].m_nSummationCounter;

    if( nSampleCount > 0)   // no divide by 0.
    {   // calculate average for RR CPI (sum / no of samples)
        ffCPIRR = g_WorkCPIRRData.m_ComponentCPIRRData[nCompIndex].m_ffCPISummation / nSampleCount;
        // store new average into structure.
        g_WorkCPIRRData.m_ComponentCPIRRData[nCompIndex].m_ffCPIAverage = ffCPIRR;
    }
    else    // if sample count = 0, set avg to 0.
        g_WorkCPIRRData.m_ComponentCPIRRData[nCompIndex].m_ffCPIAverage = 0.0;
}


//////////////////////////////////////////////////////
// ResetWorkCPIRoundRobin( int nComponent )         from ASM = RESETRROBIN (loosely based)
//
// Resets the CPI round robin
//
// M.McKiernan                          11-08-2004
// First pass.
//////////////////////////////////////////////////////
void ResetWorkCPIRoundRobin( int nComponent )
{
    int nCompIndex = nComponent - 1;    // index is 0 based.
    int i;
// clear each sample in RR.
    for(i = 0; i < MAX_CPI_RR_SAMPLES; i++)
    {
        g_WorkCPIRRData.m_ComponentCPIRRData[nCompIndex].m_ffCPI[i] = 0.0; // clear each rr sample.
    }
// clear sum
    g_WorkCPIRRData.m_ComponentCPIRRData[nCompIndex].m_ffCPISummation = 0.0;

// clear summation counter
        g_WorkCPIRRData.m_ComponentCPIRRData[nCompIndex].m_nSummationCounter = 0;

// clear avg.
        g_WorkCPIRRData.m_ComponentCPIRRData[nCompIndex].m_ffCPIAverage = 0.0;

// clear index or sample counter.
    g_WorkCPIRRData.m_ComponentCPIRRData[nCompIndex].m_nCPIRRIndex = 0; // Roll over to 0.

}


//////////////////////////////////////////////////////
// GetFillingStage(nComp )          from ASM = no direct correspondance
//
// Decide which stage of filling we are at.
//
//
// Entry:   component no.
//
//
// Exit:      Return stage
//
// M.McKiernan                          12-08-2004
// First pass.
//////////////////////////////////////////////////////
int GetFillingStage( int nComponent )
{
int nCompIndex = nComponent - 1;
int nStage;
// --REVIEW-- Note the storage areas and stages (names) are confusing, ...Store 3 for Stage1.
      if(g_CalibrationData.m_nStageFillEn)     // Stage filling enabled?? can be single/two/three)
      {
         // Stage filling enabled
         nStage = g_nStageSeq[nCompIndex];

         if( nStage > STAGE_3 )     // currently only 3 stages.
            nStage = STAGE_0;       // use Stage 0 as default.

      }
      else
      { // no stage filling.
         nStage = STAGE_0;
      }

      return(nStage);  // return stage of filling.
}


//////////////////////////////////////////////////////
// ResetRoundRobinFlowRateForAllComponents
//
////////////////////////////////////////////////////
void ResetRoundRobinFlowRateForAllComponents( void )
{
    int k;
    for(k= 0; k < MAX_COMPONENTS; k++)       //asm = RESETRROBINFORALLCOMPS
    {
        g_MainCPIRRData.m_ComponentCPIRRData[k].m_nResetCPIRRCtr = RESETRRNO;	// cause the CPI round robins to be reset.
        g_Stage2CPIRRData.m_ComponentCPIRRData[k].m_nResetCPIRRCtr = RESETRRNO;	//
        g_Stage3CPIRRData.m_ComponentCPIRRData[k].m_nResetCPIRRCtr = RESETRRNO;	//
    }
}


//////////////////////////////////////////////////////
// GetRRobinData
// gets rr data and store in working array,g_WorkCPIRRData
//
////////////////////////////////////////////////////

void GetRRobinData( int nComponent )
{
    int nStage;
    nStage =  GetFillingStage( nComponent );    // get stage of filling.

    // copy relevant CPI round robin structure into the working RR structure.
    if(nStage == STAGE_3)
    {
        memcpy(&g_WorkCPIRRData, &g_Stage3CPIRRData, sizeof( g_WorkCPIRRData ) );
    }
    else if(nStage == STAGE_2)
    {
        memcpy(&g_WorkCPIRRData, &g_Stage2CPIRRData, sizeof( g_WorkCPIRRData ) );
    }
    else // assume first or no stage filling.
    {
        memcpy(&g_WorkCPIRRData, &g_MainCPIRRData, sizeof( g_WorkCPIRRData ) );
    }
}


//////////////////////////////////////////////////////
// CheckForValidFlowRate
// checks if flow rate is valid.
//
////////////////////////////////////////////////////

void CheckForValidFlowRate( float ffCPIInst,int nComponent )
{
    float  fCPIAverage,fDiff,fPercentage;
    BOOL   bIgnore = FALSE;
    int nStage;

    nStage =  GetFillingStage( g_cCompNo );    // get stage of filling.
    if((g_CalibrationData.m_bFlowRateCheck)&&((nStage == 0)||(nStage == 3)))

    {
        GetRRobinData(nComponent);  // get rr data
        fCPIAverage = g_WorkCPIRRData.m_ComponentCPIRRData[nComponent-1].m_ffCPIAverage;
        bIgnore = g_WorkCPIRRData.m_ComponentCPIRRData[nComponent-1].m_bIgnoreCPIRR;
         if((fCPIAverage > 0.001)&&(!g_bComponentHasFilledByWeight[nComponent-1])&&!bIgnore)
        {
            if(g_bAcceptFlowRate[nComponent-1])    // should the flow rate be accepted ?
            {
                g_bAcceptFlowRate[nComponent-1] = FALSE;
            }
            else
            {
                if(ffCPIInst > fCPIAverage)
                {
                    fDiff = ffCPIInst - fCPIAverage;
                }
                else
                {
                    fDiff = fCPIAverage - ffCPIInst;
                }
                fPercentage = 100.0 * fDiff / fCPIAverage;  // calculate %
                if((fdTelnet > 0 && g_nTSMDebug.m_bFlowRateDebug)&& g_nTSMDebug.m_bFlowRateDebug)
                printf("\n flow rate for comp %d is inst %3.4f average %3.4f percentage diff %3.4f",nComponent,ffCPIInst,fCPIAverage,fPercentage);

                if(fPercentage > MAX_FLOW_RATE_PERCENTAGE_DEVIATION_ALLOWED)
                {
                    g_nValidFlowRateExceededCtr[nComponent-1]++;
                    if(g_nValidFlowRateExceededCtr[nComponent-1] > MAX_FLOW_RATE_EXCEEDED_CTR_NO)
                    {
                        if((fdTelnet > 0) && g_nTSMDebug.m_bFlowRateDebug)
                        iprintf("\n comp %d exceeded counter",nComponent);

                        g_nValidFlowRateExceededCtr[nComponent-1] = 0;
                        g_MainCPIRRData.m_ComponentCPIRRData[nComponent-1].m_nResetCPIRRCtr = 1;
                    }
                    else
                    {
                        g_MainCPIRRData.m_ComponentCPIRRData[nComponent-1].m_bIgnoreCPIRR = TRUE;
                    }
                    if((fdTelnet > 0)&& g_nTSMDebug.m_bFlowRateDebug)
                    printf("\n flow rate exceeded for comp %d",nComponent);
                }
                else
                {
                    g_nValidFlowRateExceededCtr[nComponent-1] = 0;
                }
            }
       }
       else
       {
           if((fdTelnet > 0)&& g_nTSMDebug.m_bFlowRateDebug)
           printf("\n average is 0 for component  %d cpi %3.4f",nComponent,fCPIAverage);
       }
    }
}

