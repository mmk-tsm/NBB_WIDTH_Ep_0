
//////////////////////////////////////////////////////
// Filename = ChkFill.c
//
// Checking fill accuracy
//
//
// M.McKiernan                          04-08-2004
// First Pass
// M.McKiernan                          14-09-2004
// Changed how a regrind is detected, use g_CurrentRecipe.m_nRegrindComponent
//
// M.McKiernan                          13-12-2004
// Activated the RemoveAlarmTable()'s and PutAlarmTable.
//
// P.Smith                          29/09/05
// check g_bAllowLargeAlarmBand if set allow large alarm band.
// this flag is set when the order is changed.
//
// P.Smith                          20/10/05
// remove todo set of g_bAllowLargeAlarmBand

// P.Smith                          20/10/05
// rename  g_bAllowLargeAlarmBand -> g_nAllowLargeAlarmBand
//
// P.Smith                      23/2/06
// first pass at netburner hardware conversion.
// Bool -> BOOL
//#include <basictypes.h>
//
// P.Smith                      28/2/06
// bPointer = GetHasRetriedPointer( (int)g_cCompNo );
// nRetryPointer = GetRetryCounter( (int)g_cCompNo );
// CalculateNewTargetTime();       //ASM = CALCNEWTAR
// lMinTime = GetComponentMinTime( (int)g_cCompNo );
//  nStage = GetFillingStage( (int)cCompNo );
// #include "CPIRRobin.h"
//
// P.Smith                      4/12/06
// Remove --todoh-- comments.
//
// P.Smith                      17/5/07
// set up m_nResetCPIRRCtr to ensure that flow rate round robin is reset
// for 3 fills after the fill by weight is initiated
//
// P.Smith                      22/6/07
// name change to proper blender names.
//
// M.McKiernan                      17/9/07
// Removed g_arrnMBTable[] & g_arrnMBTableFlags[], now in BatVars2
//
// P.Smith                      23/1/08
// remove fill by weight set on five consecutive batches of retries
// Calculate new flow rate and use this from now on.
// set g_bNoFlowRateCalculation to false to ensure that the new flow rate is calculated
//
// P.Smith                      28/1/08
// tighten alarm bands for the retry on the component weights
// 0-5 ->  40%
// 5-20  ->  20%
// 20 - 500 -> 10%
// +500  -> 5%
// remove check for 50/150
//
// P.Smith                      4/6/08
// in CheckFillAccuracy, set g_bNoFlowRateCalculation to true to stop the flow rate
// from being calculated when the blender is skipping past the regrind component.
// allow for regrind in the retries, if the component is regrind and regrind retry is
// off, then  if the weight is less than 20 grams, it is assumed that the regrind is not
// there and it is not taken as an actual retry looking for material.
//
// P.Smith                      6/6/08
// Modify software to allow fill by weight to allow recovery of the correct flow rate.
// if the reties are max-3, then calculate the flow rate, if it is max retries, then
// fill by weight to recover.
// A problem has been seen where the flow rate has gone to zero.
// this only happened when the fill retries was not set to 00.
// it is due to the fact that the moveon flag is set when the required no of retries have been completed.
// and this calls the flow rate calculation again. To prevent this, g_bNoFlowRateCalculation
// is set to true to stop the flow rate from being calculated on a retry.
// This stops the flow rate from going to zero..
//
// P.Smith                      1/7/08
// Remove check for Micrabatch
//
// P.Smith                      18/7/08
// remove ; from end of if statement  if(!g_CalibrationData.m_bRegrindRetry && (cData & REGRIND_BIT !=0));
// this was causing the if statement to fail the test and carry on regardless.
//
// P.Smith                      14/8/08
// modify recalculation of flow rate to use the current component and not
// to use the component no in the table.
// this is no different except for the top up mode.
// in this case the top up component is not in the sequence table.
// by using the component no that is retrying, the top up component flow rate
// can be recalculated when the component has retried for 4 consecutive batches.
// nbb--todo-- there is still a problem with the decision to make the blender do
// a fill by weight and the rate calibration does not work properly.
// the main reason for this is that the component no is not in the sequence table.
// this will eventuallly have to be modified to make compatable with the sequence table set up
// the retry counter for the particular component is reset to 0 so that if the component
// retries again,the counter will be at zero and can continue to count up.
//
// P.Smith                      25/8/08
// a problem has been seen with the retries, the recalculation of the flow rate is not
// correct.  this is modified to use the component no
//
// P.Smith                      11/9/08
// if the retries have reached the max and top up is enabled and comp no is 8, then
// set g_bVenturiFirstTime to make sure that the flow rate will be copied from component
// 1 to component 8 on the next cycle.
//
// P.Smith                      24/8/09
// set g_bAcceptFlowRate to TRUE to ensure that the flow rate is accepted and not rejected
//
// P.Smith                      1/2/10
// set g_bComponentHasRetried if component is retrying, remove if not retrying
//
// P.Smith                      4/2/10
// only set g_bComponentHasRetried if a fill alarm occurs, do not clear it.
// this means that any occurance of a any fill alarm is detected.
//
// P.Smith                      6/4/10
// if less than min opening time, then when retrying set incrementing flag
// and increment on time by FILL_TIME_INCREMENT
// in CalculateNewTargetTime if incrementing, then set g_bNoFlowRateCalculation
// so that the flow rate is not updated
// if there is no fill error, then is incrementing is cleared
//////////////////////////////////////////////////////

//*****************************************************************************
// INCLUDES
//*****************************************************************************
#include <Stdio.h>
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
#include "CycleMonitor.h"
#include "ChkFill.h"
#include "MBProgs.h"
#include "SetpointFormat.h"
#include "MonBat.h"
#include "RRFormat.h"
#include "CPIRRobin.h"
#include "Alarms.h"
#include "Batvars2.h"
#include <basictypes.h>

// Locally declared global variables
// Externally declared global variables
// Data
extern CalDataStruct    g_CalibrationData;
extern  structSetpointData  g_CurrentRecipe;

//extern float  g_fWeightAtoDCounts;

extern  int g_nKeyCode;
extern  int g_nLanguage;
extern  int g_nGeneralSecondTimer;
extern  structMeasurementUnits  g_MeasurementUnits;

BOOL g_bIsIncrementing = FALSE;

//////////////////////////////////////////////////////
// CheckFillAccuracy( void )            from ASM = CHKFILLACC
// Note: This is based on assembler version of 5th Aug, 2004.
//
// Check fill accuracy for comp x (compno).
// COMPWTERROR = (CMPXTARWGT - CMPXACTWGT)
// COMPWTLIMIT = 30 % OF COMPWTERROR.
// CHECK THIS AGAINST THE ACTUAL WEIGHT AND ALARM IF NECESSARY.
//
// M.McKiernan                          04-08-2004
// First pass.
// M.McKiernan                          03-09-2004
// Located in "( nRetryCtr  > MAX_RETRY_CYCLES )" section:
// Cause the reset CPI flags to be set for each of the CPI round robin structures
// (g_MainCPIRRData.m_ComponentCPIRRData[nCompIndex].m_bResetCPIRR = TRUE_)
// Similarly set the ignore CPI RR flag for the relevant CPI structure (stage).
//
// P.Smith                          24/6/08
// remove g_arrnMBTable
//////////////////////////////////////////////////////
BOOL CheckFillAccuracy( void )
{
int nCompIndex,nIndex;
float fComponentWtLimit;
float fComponentWeightError;
unsigned char cData,cCompNo;
BOOL bResult;
BOOL *bPointer;
//int *nPointer;
int *nRetryPointer;
long    lCompTime,lMinTime;
int nPercentageLimit;
int nRetryCtr;
int     nStage;

    g_bNoFlowRateCalculation = FALSE;
    // If AnyFillByWeight clr and Retry active.
    if(!g_bAnyFillByWeight && g_cRetryActive)
    {
        g_bNoFlowRateCalculation = TRUE;  // No flow rate calculation.
    }
    g_bUnderFill = FALSE;
    nCompIndex = (int)g_cCompNo - 1;    //index is 0 based.
    // error = target wt - actual wt.
    fComponentWeightError = g_fComponentTargetWeight[nCompIndex] - g_fComponentActualWeight[nCompIndex];


    if(fComponentWeightError <= 0)  // error negative or 0??
    {   // ASM = FILLOKAY1:
    	g_bIsIncrementing = FALSE;
        g_bRegrindFillProblem = FALSE;
        RemoveAlarmTable( NOFILLALARM,  (int)g_cCompNo );       // clear alarm, Component no
        g_cRetryActive = 0;
        bResult = FALSE;    // value to return.
    }
    else
    {   //weight error is positive.
        if(g_nAllowLargeAlarmBand)    //.
        {
            nPercentageLimit = LOWER_TARGET_LIMIT;
        }
        else
        if(g_fComponentTargetWeight[nCompIndex] > 0.5) // target wt > 1kg??
            nPercentageLimit = HIGHER_COMPONENT_PERCENTAGE_LIMIT;
        else
        // IS Target for this component > venturi band limit??
        if( g_fComponentTargetWeight[nCompIndex] > 0.020 )
            nPercentageLimit = LOWER_COMPONENT_PERCENTAGE_LIMIT;
        else
        if( g_fComponentTargetWeight[nCompIndex] > VENTURI_BAND_LIMIT )
            nPercentageLimit = 20;
        else
        nPercentageLimit = LOWER_TARGET_LIMIT;

        //ASM = CONP1
        // Comp wt limit = target wt * selected percentage
        fComponentWtLimit = (g_fComponentTargetWeight[nCompIndex] * nPercentageLimit) / 100.0;
        // Check error vs limit just calculated.
        if(fComponentWeightError > fComponentWtLimit)
        {
            g_bUnderFill = TRUE;
            // ASM = FILLALR
            bPointer = GetHasRetriedPointer( (int)g_cCompNo );
            g_bComponentHasRetried[g_cCompNo-1] = TRUE;
            *(bPointer + g_cCompNo - 1 ) = TRUE;    // set retried flag.

                BOOL bValidRetry = TRUE;
                cData = g_cSeqTable[g_nSeqTableIndex];
                if(!g_CalibrationData.m_bRegrindRetry && ((cData & REGRIND_BIT) !=0))
                {
                    if(g_fComponentActualWeight[nCompIndex] < 0.020f)  // if less than 20 grams assume no material
                    {
                        bValidRetry = FALSE; // component is empty
                    }
                }

            if((int)g_cCompNo != g_nRetryCompNoHis) // has component already retried??
            {
                g_nRetryCompNoHis = (int)g_cCompNo;
                if(bValidRetry)
                {
                // increment retry counter.
                nRetryPointer = GetRetryCounter( (int)g_cCompNo );
                nRetryCtr = *(nRetryPointer + g_cCompNo - 1);
                nRetryCtr++;

                *(nRetryPointer + g_cCompNo - 1) = nRetryCtr;
                 if( nRetryCtr  == (MAX_RETRY_CYCLES-1) )
                 {
                      g_nCorrectRetries = 1;

//--done--      ResetRoundRobinCPI( cCompNo );  //
//--REVIEW--
                    // cause the CPI round robins to be reset.
                    g_MainCPIRRData.m_ComponentCPIRRData[nCompIndex].m_nResetCPIRRCtr = RESETRRNO;
                    g_Stage2CPIRRData.m_ComponentCPIRRData[nCompIndex].m_nResetCPIRRCtr = RESETRRNO;    //
                    g_Stage3CPIRRData.m_ComponentCPIRRData[nCompIndex].m_nResetCPIRRCtr = RESETRRNO;    //

                    g_bNoFlowRateCalculation = FALSE;

//--REVIEW-- - This is new, i.e. setting igoreCPI for stage being used.
// clear relevant ignoreCPI's.
                    if(g_CalibrationData.m_nStageFillEn)
                    {
                        nStage = GetFillingStage( (int)nCompIndex );
                        if(nStage == STAGE_3)
                            g_Stage3CPIRRData.m_ComponentCPIRRData[nCompIndex].m_bIgnoreCPIRR = FALSE;
                        else if(nStage == STAGE_2)
                            g_Stage2CPIRRData.m_ComponentCPIRRData[nCompIndex].m_bIgnoreCPIRR = FALSE;
                        else
                            g_MainCPIRRData.m_ComponentCPIRRData[nCompIndex].m_bIgnoreCPIRR = FALSE;
                    }
                    else // i.e. not multistage fill
                    {
                        g_MainCPIRRData.m_ComponentCPIRRData[nCompIndex].m_bIgnoreCPIRR = FALSE;
                    }
//                  g_bIgnoreCPI[g_cCompNo -1] = FALSE; // ignore this error.
                    g_bAcceptFlowRate[g_cCompNo-1] = TRUE;
                    CalculateCPI( g_cCompNo );      // CALCNTPI calculate count per interrupt for comp n
               }
               else

               if( nRetryCtr  > MAX_RETRY_CYCLES )
               {
                    nRetryCtr = 0;
                    *(nRetryPointer + g_cCompNo - 1) = nRetryCtr;
                    if(g_bTopUpFlag && (g_cCompNo == EIGHT ))
                    {
                        g_bVenturiFirstTime = TRUE;
                    }
                    else
                    {
                        g_nCorrectRetries = 1;
                        if(g_bTopUpFlag && g_nTopUpSequencing == FILLCOLOUR)    //Top up enabled & fill colour
                        {
                            nIndex = g_nLastCompTableIndex;  //LASTCOMPTABLEP
                        }
                        else
                        {
                            nIndex = g_nSeqTableIndex;  //SEQPOINTER
                        }

                        cData = g_cSeqTable[nIndex];  //load byte from selected index
                        cData |= FILLBYWEIGHT;          // Fill by wt.
                        cData |= LEAVEFILLBYWEIGHT; // indicate f.b.w. should not be cleared.

                        g_cSeqTable[nIndex] = cData;    //
                        cCompNo = cData & 0x0F;         // isolate comp no.
                        nCompIndex = (int)cCompNo - 1;
//--done--      ResetRoundRobinCPI( cCompNo );  //
//--REVIEW--
                    // cause the CPI round robins to be reset.
                        g_MainCPIRRData.m_ComponentCPIRRData[nCompIndex].m_nResetCPIRRCtr = RESETRRNO;
                        g_Stage2CPIRRData.m_ComponentCPIRRData[nCompIndex].m_nResetCPIRRCtr = RESETRRNO;    //
                        g_Stage3CPIRRData.m_ComponentCPIRRData[nCompIndex].m_nResetCPIRRCtr = RESETRRNO;    //

                        g_bNoFlowRateCalculation = FALSE;

//--REVIEW-- - This is new, i.e. setting igoreCPI for stage being used.
// clear relevant ignoreCPI's.
                        if(g_CalibrationData.m_nStageFillEn)
                        {
                            nStage = GetFillingStage( (int)g_cCompNo );
                            if(nStage == STAGE_3)
                                g_Stage3CPIRRData.m_ComponentCPIRRData[nCompIndex].m_bIgnoreCPIRR = FALSE;
                            else if(nStage == STAGE_2)
                                g_Stage2CPIRRData.m_ComponentCPIRRData[nCompIndex].m_bIgnoreCPIRR = FALSE;
                            else
                                g_MainCPIRRData.m_ComponentCPIRRData[nCompIndex].m_bIgnoreCPIRR = FALSE;

                        }
                        else // i.e. not multistage fill
                        {
                            g_MainCPIRRData.m_ComponentCPIRRData[nCompIndex].m_bIgnoreCPIRR = FALSE;
                        }
                   }
               }

              }
            }

// ASM = HASRET
                // regrind and retry on regrind = false, and is regrind component ??
                //--REVIEW  is the logic with regrind retry correct??
                if(g_CurrentRecipe.m_nRegrindComponent && !g_CalibrationData.m_bRegrindRetry && g_nPITCom == g_CurrentRecipe.m_nRegrindComponent)
//              if(g_bRegrindPresent && !g_CalibrationData.m_bRegrindRetry && g_cCompNo == g_CalibrationData.m_nRegrindComponentNumber )
                {
                    // SKIPREGFILL  - no refill if regrind and regrind retry disabled.
                    g_bIsIncrementing = FALSE;
                     g_bRegrindFillProblem = TRUE;
                    g_bNoFlowRateCalculation = TRUE;
                    RemoveAlarmTable( NOFILLALARM,  (int)g_cCompNo );       // clear alarm, Component no
                    g_cRetryActive = 0;
                    bResult = FALSE;    // value to return.
                }
                else
                {
                    // NOREGTO
                    CalculateNewTargetTime();       //ASM = CALCNEWTAR
                    if(g_cRetryActive)
                    {
                        if(g_fComponentTargetWeight[nCompIndex] > 0.020  ||  g_nRetryCounter >= MAX_RETRY_NO)   //target > 20g ??
                        {
                            PutAlarmTable( NOFILLALARM,  (int)g_cCompNo );      // indicate alarm, Component no
                        }
                    }
                    // asm = NOALRM
                    g_cRetryActive = g_cCompNo;
                    CalculateNewTargetTime();       //ASM = CALCNEWTAR --REVIEW-- has been called already.
                    g_bFillAlarm = TRUE;        // indicate fill alarm
                    g_nAnyFillAlarm++;
                    //
                    cData = g_cSeqTable[g_nSeqTableIndex];  //load byte from SEQPOINTER
                    if(cData & FILLBYWEIGHT)  // fill by wt bit set??
                    {
                        //DOFILL
                  //DOFILL
                  if(g_cCalcRouter == CALC1STCMPTAR || g_cCalcRouter == CALCMPNTAR)
                  {
                     g_nAnyFillAlarm++;
                     g_bPctFlag = TRUE;
                  }
                  //asm = NOT1STC
                  g_bFillAlarm = TRUE;    // Set fill alarm flag
                  // continuous retries??
                  if(g_CalibrationData.m_nFillRetries == 0)
                  {
                     bResult = TRUE;   // indicate an error.
                  }
                  else
                  {
                     g_nFillCtr++;  // fill counter
                     if(g_nFillCtr >= g_CalibrationData.m_nFillRetries)  // reached entered no. of retries??
                     {
                        g_nFillCtr = 0;   // reset counter.
                        g_cRetryActive = 0; // reset flag, as moving to next component.
                        g_bMoveOn = TRUE;
                        g_bNoFlowRateCalculation = TRUE; // nbb--todo-- check this out.

                     }
                     bResult = TRUE;   // indicate an error.
                  }
                  // end of DOFILL section
                    }
                    else
                    {   // fill by wt bit not set.
                        lCompTime = g_lCmpTime[g_cCompNo -1];
                        lMinTime = GetComponentMinTime( (int)g_cCompNo );
                        if(lCompTime < lMinTime)
                        //      g_lCmpTime[g_cCompNo - 1] = lMinTime; //set component time to the minimum.
                        {
                          	g_bIsIncrementing  = TRUE;
                        	if(g_lCmpTime[g_cCompNo - 1] < MAX_FILL_TIME_TO_INCREMENT_TO)
                        	{
                        		g_lCmpTime[g_cCompNo - 1] += FILL_TIME_INCREMENT;
                        	}
                        }
                        if(!g_cRetryActive && !g_bUnderFill)
                        {
                            // ASM = FILLOKAY
                        	g_bIsIncrementing = FALSE;
                            g_bRegrindFillProblem = FALSE;
                            RemoveAlarmTable( NOFILLALARM,  (int)g_cCompNo );       // clear alarm, Component no
                            g_cRetryActive = 0;
                            bResult = FALSE;    // value to return.
                        }
                        else
                        {
                            //DOFILL ( - This is an exact replication of the other DOFILL section)
                            if(g_cCalcRouter == CALC1STCMPTAR || g_cCalcRouter == CALCMPNTAR)
                            {
                                g_nAnyFillAlarm++;
                                g_bPctFlag = TRUE;
                            }
                            //asm = NOT1STC
                            g_bFillAlarm = TRUE;        // Set fill alarm flag
                            // continuous retries??
                            if(g_CalibrationData.m_nFillRetries == 0)
                            {
                                bResult = TRUE; // indicate an error.
                            }
                            else
                            {
                                g_nFillCtr++;   // fill counter
                                if(g_nFillCtr >= g_CalibrationData.m_nFillRetries)  // reached entered no. of retries??
                                {
                                    g_nFillCtr = 0; // reset counter.
                                    g_cRetryActive = 0; // reset flag, as moving to next component.
                                    g_bMoveOn = TRUE;
                                    g_bNoFlowRateCalculation = TRUE;  //nbb--todo-- check that this works.

                                }
                                bResult = TRUE; // indicate an error.

                            }
                            // end of DOFILL section
                        }
                    }


                }
            // WAS BRACKET
        }
        else
        {   // ASM = FILLOKAY
        	g_bIsIncrementing = FALSE;
        	g_bRegrindFillProblem = FALSE;
            RemoveAlarmTable( NOFILLALARM,  (int)g_cCompNo );       // clear alarm, Component no
            g_cRetryActive = 0;
            bResult = FALSE;    // value to return.

        }
    }





    return( bResult );
}
//////////////////////////////////////////////////////
// GetHasRetriedPointer(nComp )         from ASM = GETHASRETRIEDPTR
//
// Decide where to get "Has Retried" value
//
//
// Entry:   component no.
//
//
// Exit:      Return pointer to correct array.
//
// M.McKiernan                          05-08-2004
// First pass.
//////////////////////////////////////////////////////
BOOL *GetHasRetriedPointer( int nComponent )
{
BOOL *bPtr;
int nCompIndex = nComponent - 1;
int nStage;
// --REVIEW-- Note the storage areas and stages (names) are confusing, ...Store 3 for Stage1.
      if(g_CalibrationData.m_nStageFillEn)         // Stage filling enabled?? can be single/two/three)
      {
         // Stage filling enabled
         nStage = g_nStageSeq[nCompIndex];

         if(nStage == STAGE_2)
            bPtr = g_bHasRetriedStage2;  // use Store 2 for Stage 2
         else if(nStage == STAGE_1)
            bPtr = g_bHasRetriedStage3;  // use Store 3 for Stage 1  --review--
         else
            bPtr = g_bHasRetried;  // use Store 1 for all others

      }
      else
      { // no stage filling.
         bPtr = g_bHasRetried;
      }

      return(bPtr);  // return address of array chosen.
}

//////////////////////////////////////////////////////
// GetRetryCounter(nComp )          from ASM = GETRETRYCTR
//
// Decide where to get "Retry Counter" value
//
//
// Entry:   component no.
//
//
// Exit:       return pointer to correct counter depending on stage.
//
// M.McKiernan                          05-08-2004
// First pass.
//////////////////////////////////////////////////////
int *GetRetryCounter( int nComponent )
{
int *nPtr;
int nCompIndex = nComponent - 1;
int nStage;
// --REVIEW-- Note the storage areas and stages (names) are confusing, ...Store 3 for Stage1.
      if(g_CalibrationData.m_nStageFillEn)         // Stage filling enabled?? can be single/two/three)
      {
         // Stage filling enabled
         nStage = g_nStageSeq[nCompIndex];

         if(nStage == STAGE_2)
            nPtr = g_nRetryCounterStage2;  // use Stage 2 for Stage 2
         else if(nStage == STAGE_1)
            nPtr = g_nRetryCounterStage3;  // use Stage 3 for Stage 1  --review--
         else
            nPtr = g_nRetryCounterStage1;  // use Stage 1 for all others

      }
      else
      { // no stage filling.
         nPtr = g_nRetryCounterStage1;
      }

      return(nPtr);  // return address of array chosen.
}


//////////////////////////////////////////////////////
// GetComponentMinTime(nComp )  from ASM = GETCOMPONENTMINIMUM
//
// Entry:   component no.
//
//
// Exit:       return minimum time based on component and blender type.
//
// M.McKiernan                          06-08-2004
// First pass.
//////////////////////////////////////////////////////
long GetComponentMinTime( int nComponent)
{
long lMinTime;
unsigned int nCompConfig;

    if(g_nRetryCounter == MAX_RETRY_NO)
    {   // asm = DONORC
        switch( g_CalibrationData.m_nBlenderType )
        {
            case TSM150:
            case TSM50:
                // asm = ISMIDI
                if(g_CalibrationData.m_bStandardCompConfig)
                {   //standard config
                    if( nComponent == 1 || nComponent == 2)  // components 1 or 2.
                        lMinTime = MAX_MIN_VALVE_OPEN_TIME;
                    else
                        lMinTime = MIN_VENTURI_OPEN_TIME;
                }
                else
                {   // i.e. non-std component config.
                    nCompConfig = g_CalibrationData.m_nCompConfig[nComponent - 1];  // pick up this components configuration.
                    if( nCompConfig & 0x0003)   // using outputs 1 or 2??
                        lMinTime = MAX_MIN_VALVE_OPEN_TIME;
                    else
                        lMinTime = MIN_VENTURI_OPEN_TIME;
                }
            break;
            case TSM3000:
                if(g_CalibrationData.m_bStandardCompConfig)
                {   //standard config
                    if( nComponent > EIGHT )
                        lMinTime = MIN_VENTURI_OPEN_TIME;
                    else  // i.e. <= 8
                        lMinTime = MAX_MIN_VALVE_OPEN_TIME;
                }
                else
                {   // i.e. non-std component config.
                    nCompConfig = g_CalibrationData.m_nCompConfig[nComponent - 1];  // pick up this components configuration.
                    if( nCompConfig & 0xFF00)   // using outputs > 8??
                        lMinTime = MIN_VENTURI_OPEN_TIME;
                    else // i.e. <= 8
                        lMinTime = MAX_MIN_VALVE_OPEN_TIME;
                }
            break;
            default:
                lMinTime = MAX_MIN_VALVE_OPEN_TIME;
            break;
        }
    }
    else
    {
        g_nRetryCounter++;  // incr. retry ctr
        lMinTime = 1;       // set min to 1 (will disable check later on)
    }

    return( lMinTime );
}

//////////////////////////////////////////////////////
// CalculateNewTargetTime();        //ASM = CALCNEWTAR
//
// Entry:   component no.
//
//
// Exit:    Newly calculated time in component time location.
//
// M.McKiernan                          10-08-2004
// First pass.
//////////////////////////////////////////////////////
void CalculateNewTargetTime( void )
{
int nCompIndex = g_cCompNo - 1; // index for component arrays.
long lDiff;
long lTempS;

	if(g_bIsIncrementing)  // calculate new target if not incrementing
	{
        g_bNoFlowRateCalculation = TRUE;  // No flow rate calculation.
	}
	else
	{
    // save current target a/d counts.
    lTempS = g_lComponentTargetCounts[nCompIndex];
    // underfill (target - actual) in a/d counts.
    lDiff = g_lComponentTargetCounts[nCompIndex] - g_lComponentCountsActual[nCompIndex];

    //store as new target
    g_lComponentTargetCounts[nCompIndex] = lDiff;

    CalcCompNTime( (int)g_cCompNo );        // calculate & store new time in PIT periods.

    // restore old target a/d counts.
    g_lComponentTargetCounts[nCompIndex] = lTempS;
	}
}

/*
This is version from Aug 5th 2004 - Had been changed from original set of ASM's

;       CHECK FILL ACCURACY FOR COMP X (COMPNO).
;       COMPWTERROR = (CMPXTARWGT - CMPXACTWGT)
;       COMPWTLIMIT = 30 % OF COMPWTERROR.
;       CHECK THIS AGAINST THE ACTUAL WEIGHT AND ALARM IF NECESSARY.
;
CHKFILLACC:
        CLR     NOFLOWRATECALCULATION
        TST     ANYFILLBYWEIGHT
        BNE     NOFLRC             ; ALLOW CALCULATION OF FLOW RATE
        TST     RETRYACTIVE        ; IS THE RETRY ACTIVE.
        BEQ     NOFLRC             ; NO FLOW RATE CALCULATION.
        LDAA    #1
        STAA    NOFLOWRATECALCULATION
NOFLRC:
        CLR     UNDERFILL       ; RESET FLAG.
        JSR     CLRAC           ; A = C = 0
        LDX     #CMP1TARWGT     ; COMPONENT #1 TARGET WEIGHT
        LDAB    COMPNO          ; READ COMPONENT COUNTER
        DECB                    ;
        ABX
        ABX
        ABX
        PSHM    X               ; SAVE TARGET ADDRESS
        JSR     AMOVE           ; A = CMPTARWGT

        LDX     #CMP1ACTWGT     ; COMPONENT WEIGHT
        LDAB    COMPNO
        DECB
        ABX
        ABX
        ABX
        JSR     CMOVE           ; C = CMPXACTWGT
        JSR     SUB             ; A-C
        LDX     #COMPWTERROR    ; XX XX X.Y GRAM ERROR
        JSR     EMOVE
        PULM    X               ; CMPXTARWGT
        LDAA    MATHDREG
        LBMI     FILLOKAY1

;
;       CALCULATE ACCEPTABLE % IN GRAMS
;
        JSR     CLRAC

        LDX     #CMP1TARWGT     ; COMPONENT #1 TARGET WEIGHT
        LDAB    COMPNO          ; READ COMPONENT COUNTER
        DECB                    ;
        ABX
        ABX
        ABX
        TST     0,X             ; CHECK TARGET < 1000
        BNE     GREATER
        LDAA    BLENDERTYPE
        CMPA    #BABYTYPE       ; M50
        BEQ     CHKLM           ; CHECK LOWER LIMIT FOR VENTURI
        CMPA    #MIDITYPE       ; MIDI
        BNE     INSEBND         ;

CHKLM   LDAB    COMPNO
        CMPB    #THREE
        BEQ     CHKLM1          ; VENTURI COMP
        CMPB    #FOUR           ;
        BNE     INSEBND         ;

CHKLM1  LDD     1,X
        CPD     #VENTURIBANDLIMIT         ; LIMIT FOR VENTURIS GRAMS
        BHI     INSEBND
        LDAB    #LOWTARGETLIMIT
        BRA     CONP1

INSEBND LDAB    #LOWERCOMPPERLIMIT       ; ACCEPTABLE %
        BRA     CONP1
GREATER:

        LDAB    #HIGHERCOMPPERLIMIT ; LIMIT % FOR LOWER COMP.
CONP1   STAB    AREG+4          ;

        JSR     CMOVE           ; C= CMPXTARWGT
        JSR     MUL             ; % OF TARGET
        LDX     EREG+1
        STX     COMPWTLIMIT     ; XX XX X.Y GRAMS LIMIT
        LDAA    EREG+3          ;
        STAA    COMPWTLIMIT+2   ;

;
;       CHECK ERROR
;
        LDAA    COMPWTERROR     ;
        CMPA    COMPWTLIMIT     ; COMPARE TO LIMIT
        BHI     UNDERFILLING    ; WTERROR > LIMIT
        BNE     NOTUNDERFILLING
        LDD     COMPWTERROR+1
        CPD     COMPWTLIMIT+1
        BHI     UNDERFILLING     ; FILL ALARM


NOTUNDERFILLING:
        BRA     CHECKRETRY


UNDERFILLING:
        LDAA    #1
        STAA    UNDERFILL       ; SET UNDER FILLING FLAG.


CHECKRETRY:
;        TST     RETRYACTIVE
;        BNE     FILLALR         ; FILL ALARM
        TST     UNDERFILL       ; HAS IT UNDERFILLED ?
        BNE     FILLALR         ; IS THERE A FILL ALARM


FILLOKAY:

        CLR     REGRINDFILLPROBLEM
FILLOKAYSRC:
        LDAA    COMPNO          ; READ COMP NO
        ORAA    #NOFILLALARM    ;
        JSR     RMALTB          ; INDICATE FILL ALARM
        CLR     RETRYACTIVE
        CLRA                    ; A = 0 INDICATES ALARM OKAY
        RTS

SKIPREGFILL:
        LDAA    #1
        STAA    REGRINDFILLPROBLEM
        BRA     FILLOKAYSRC




FILLOKAY1:
        CLR     REGRINDFILLPROBLEM
        LDAA    COMPNO          ; READ COMP NO
        ORAA    #NOFILLALARM    ;
        JSR     RMALTB          ; INDICATE FILL ALARM
        CLR     RETRYACTIVE
        CLRA
        RTS

FILLALR:
        NOP
FILLAL:
;        LDX     #HASRETRIED1    ; HAS RETRIED FLAG IS SET.
        JSR     GETHASRETRIEDPTR
        LDAB    COMPNO
        DECB
        ABX
        LDAA    #1
        STAA    0,X             ; SET FLAG.
        LDAA    COMPNO          ; HAS THIS COMP ALREADY RETRIED.
        CMPA    RETRYCOMPNOHIS  ; RETRY COMP NO.
        BEQ     HASRET          ; YES
        STAA    RETRYCOMPNOHIS  ; UPDATE HISTORY.
;        LDX     #RETRYCTR1
        JSR     GETRETRYCTR
        LDAB    COMPNO
        DECB
        ABX
        INC     0,X             ; INCREMENT COUNTER.
        LDAA    0,X             ; CHECK THIS COUNTER.
        CMPA    #MAXRETRYCYCLES ; MAX NO OF RETRY CYCLES.
        BLS     HASRET          ; EXIT.

        LDAA    #1
        STAA    CORRECTRETRIES

        TST     TOPUPF           ; TEST TOP UP MODE.
        BEQ     RDNRTB1         ; READ NORMAL TABLE.
        LDAA    TOPUPSEQUENCING ;
        CMPA    #FILLCOLOUR
        BNE     RDNRTB1         ; NO.
        LDX     LASTCOMPTABLEP  ;
        BRA     RDASN1          ; READ
RDNRTB1 LDX     SEQPOINTER      ; READ POINTER
RDASN1  LDAA    0,X             ;
        ORAA    #FILLBYWEIGHT   ; FILL BY WEIGHT
        ORAA    #LEAVEFILLBYWEIGHT ; INDICATE THAT FILL BY WEIGHT SHOULD NOT BE CLEARED
        STAA    0,X
        LDAB    0,X             ;
        ANDB    #$0F
        JSR     RESETRROBINCPI  ; ACCEPT CPI.
        LDAB    COMPNO
        LDX     #IGNORECPI1      ; FLAG TO IGNORE CPI ERROR.
        DECB                    ; COMPONENT NO.
        ABX
        CLR     0,X             ; IGNORE THIS ERROR.

HASRET  TST     REG_PRESENT     ; ANY REGRIND.
        BEQ     NOREGTO         ; NOT THIS ORDER.
        TST     REGRINDRETRY    ; CHECK FOR RETRY ON REGRIND COMPONENT
        BNE     NOREGTO         ; RETRY ON REGRIND COMPONENT
        LDAA    COMPNO
        CMPA    REGCOMP
;        LBEQ     FILLOKAY        ; NO REFILL IF REGRIND COMPONENT.
        LBEQ     SKIPREGFILL      ; NO REFILL IF REGRIND COMPONENT.

NOREGTO JSR     CALCNEWTAR      ; CALCULATE NEW TARGET.
NOTARCL TST     RETRYACTIVE     ;
        BEQ     NOALRM          ; NO ALARM THE 1ST TIME.

        LDX     #CMP1TARWGT     ; COMPONENT #1 TARGET WEIGHT
        LDAB    COMPNO          ; READ COMPONENT COUNTER
        DECB                    ;
        ABX
        ABX
        ABX
        TST     0,X             ; CHECK TARGET < 50
        BNE     GENAL
        LDD     1,X
        CPD     #$0200          ; >20 G
        BHI     GENAL

        LDAA    RETRYCOUNTER     ; READ RETRY COUNTER
        CMPA    #MAXRETRYNO       ; MAX RETRY NO
        BNE     NOALRM           ; CHECK TARGET           ;
GENAL   LDAA    COMPNO          ; READ PIT COMP NO
        ORAA    #NOFILLALARM    ;
        JSR     PTALTB          ; INDICATE FILL ALARM


NOALRM  LDAA    COMPNO
        STAA    RETRYACTIVE
        JSR     CALCNEWTAR      ; CALCULATE NEW TARGET.
        LDAA    #$0F
        STAA    FILLALARM
        INC     ANYFILLALARM
        LDX     SEQPOINTER      ; READ POINTER
        LDAA    0,X             ;
        ANDA    #FILLBYWEIGHT   ; FILL BY WEIGHT SET ?
        BNE     DOFILL          ; NO FILL
        LDX     #CMP1TIMS       ; LOOK AT TARGET.
        LDAB    COMPNO          ;
        DECB
        ABX
        ABX
        ABX
        STX     TEMPX
        TST     0,X              ; CHECK IF COUNTER >0
        BNE     DOFILL           ; FILL
        LDE     1,X              ; READ COUNTER.
        JSR     GETCOMPONENTMINIMUM
        LDX     TEMPX
        CPE     1,X               ; COMPARE TO ACTUAL
        BLS     DOFILL            ; BRANCH IF LIMIT IS LOWER
        CLR     0,X
        STE     1,X

        TST     RETRYACTIVE        ; IS THE RETRY ACTIVE.
        BNE     DOFILL             ; FILL AGAIN.
        TST     UNDERFILL          ; UNDERFILLING.
        LBEQ     FILLOKAY           ; THE FILL WAS OKAY.

DOFILL  LDAA    CALCROUTER
        CMPA    #CALC1STCMPTAR
        BEQ     IS1ST           ; IS THE 1ST REG COMP
        CMPA    #CALCMPNTAR
        BNE     NOT1STC         ; NO FIRST COMPONENT.
IS1ST   INC     ANYFILLALARM    ; INCREMENT NO OF FILL ALARMS.
        LDAA    #$AA
        STAA    PCTFLG
NOT1STC:        ;
        LDAA    #$0F
        STAA    FILLALARM       ; A NON ZERO INDICATES FILL ALARM.
        TST     FILLRETRY       ; CONTINUOUS RETRY
        BEQ     RETRYCM1
        INC     FILLCTR         ; FILL COUNTER
        LDAA    FILLCTR
        CMPA    FILLRETRY      ; AT COUNT
        BLO     RETRYCM1         ; RETRY
        CLR     FILLCTR         ; RESET COUNTER.
        CLR     RETRYACTIVE     ; RESET THIS FLAG AS WE ARE NOW MOVING TO THE NEXT COMPONENT.
        LDAA    #$0F
        STAA    MOVEON          ; MOVE ON TO NEXT
RETRYCM1 LDAA    #$0F            ; INDICATE ERROR
        RTS


ISMEGMIN:
        LDAA    NOBLNDS
        CMPA    #TWELVE         ; TWELVE COMPONENTS
        LBNE    CHKSLIDE        ; ASSUME ALL SLIDE VALVES
        LDAB    COMPNO          ; READ COMPONENT NO
        CMPB    #EIGHT          ; SLIDE
        LBHI     CHKVENTURI
        LBRA     CHKSLIDE


GETCOMPONENTMINIMUM:

        LDAA    RETRYCOUNTER     ; READ RETRY COUNTER
        CMPA    #MAXRETRYNO       ; MAX RETRY NO
        BEQ     DONORC           ; CHECK TARGET           ;
        INC     RETRYCOUNTER     ; INCREMENT COUNTER
        BRA     NOMINVL

;CHKTAR  LDX     #CMP1TARWGT     ; COMPONENT #1 TARGET WEIGHT
;        LDAB    COMPNO          ; READ COMPONENT COUNTER
;        DECB                    ;
;        ABX
;        ABX
;        ABX
;        TST     0,X             ;
;        BNE     DONORC
;        LDD     1,X
;        CPD     #$200                   ; 20 GRAMS
;        BHI     DONORC                  ; DO NORMAL CHECK
;        LDE     #MINSLIDEOPENTIME ; MIN VALVE OPENTIME
;;        LDE     #MINVENTURIOPENTIME ; MIN VALVE OPENTIME
;        BRA     XITMIN

DONORC  LDAA    BLENDERTYPE      ; CHECK BLENDER TYPE.
        CMPA    #MIDITYPE        ; IS THIS A MICRA BATCH.
        BEQ     ISMIDI           ; YES.
        CMPA    #BABYTYPE        ; M50 MACHINE
        BEQ     ISMIDI           ; YES.
        CMPA    #MEGATYPE        ; MEGA BATCH.
        BEQ     ISMEGAB          ; IS MEGABATCH
        CMPA    #TINYTYPE        ; IS THIS A MICRA BATCH.
        BNE     CHKSLIDE         ; NO.
ISMIDI  TST     STDCCFG          ; NON STANDARD COMP CONFIG.
        BEQ     NORMCFG          ; NORMAL CONFIGURATION.
        LDAB    COMPNO           ; READ COMP NO.
        DECB
        LDX     #COMP1CFG
        ABX                      ; GET COMPONENT CONFIGURATION.
        ABX                      ; GET COMPONENT CONFIGURATION.
        LDD     0,X              ; READ CONFIG.
        ANDA    #%00000011       ; CHECK FOR SLIDE VALVES
        BNE     CHKSLIDE         ;
        BRA     CHKVENTURI       ; CHECK FOR MICRA MIN.

NORMCFG:
        LDAA    COMPNO           ; READ COMPONENT NO.
        CMPA    #1
        BEQ     CHKSLIDE         ; SLIDE VALVE BEING ACTIVATED.
        CMPA    #2               ;
        BEQ     CHKSLIDE         ;

CHKVENTURI:
        LDX     TEMPX
        LDE     #MINVENTURIOPENTIME ; MIN VALVE OPENTIME
        BRA     XITMIN

CHKSLIDE:
        LDX     TEMPX
        LDE     #MAXMINVALVEOPENTIME
        BRA     XITMIN
USEMIN  LDE     #MINVALVEOPENTIME   ; MIN VALVE OPENTIME
XITMIN  RTS

NOMINVL LDE     #1
        BRA     XITMIN


; CHECK COMPONENT USED IN MEGA


ISMEGAB TST     STDCCFG             ; STANDARD CONFIGURATION
        BEQ     ISSTDCFG            ;
        LDAB    COMPNO           ; READ COMP NO.
        DECB
        LDX     #COMP1CFG
        ABX                      ; GET COMPONENT CONFIGURATION.
        ABX                      ; GET COMPONENT CONFIGURATION.
        LDD     0,X              ; READ CONFIG.
        ANDD    #$00FF           ; CHECK FOR SLIDE VALVES
        BNE     CHKSLIDE         ;
        BRA     CHKVENTURI       ; CHECK VENTURI
ISSTDCFG LDAA   COMPNO
        CMPA    #EIGHT
        BHI     CHKVENTURI       ; ASSUME VENTURI
        BRA     CHKSLIDE




CALCNEWTAR:
        LDX     #CMP1CNTG       ;
        LDAB    COMPNO          ; CURRENT COMPONENT
        DECB                    ;
        ABX
        ABX
        ABX
        JSR     AMOVE
        LDAA    AREG+2
        STAA    TEMPS
        LDE     AREG+3
        STE     TEMPS+1


        LDX     #CMP1CNAC
        LDAB    COMPNO          ; CURRENT COMPONENT
        DECB                    ;
        ABX
        ABX
        ABX
        JSR     CMOVE
        JSR     SUB             ; A-C
        LDX     #CMP1CNTG       ; LOAD COMP #1 TARGET COUNT STORAGE.
        LDAB    COMPNO          ; READ COMPONENT COUNTER
        DECB                    ;
        ABX
        ABX
        ABX
        JSR     EMOVE
        LDAB    COMPNO
        STAB    TARGNO
        JSR     CALCTGWT        ; CONVERT TO HEX
        JSR     CALCMNTIM       ; CONVERT TARGET TO TIME.

        LDX     #CMP1CNTG       ;
        LDAB    COMPNO          ; CURRENT COMPONENT
        DECB                    ;
        ABX
        ABX
        ABX
        LDAA    TEMPS
        STAA    0,X
        LDE     TEMPS+1
        STE     1,X
        LDAA    #1
        STAA    NOFLOWRATECALCULATION

        RTS

 ;*****************************************************************************;
; FUNCTION NAME : GETHASRETRIEDPTR                                            ;
; FUNCTION      : SETS HAS RETRIED PTR DEPENDING ON STAGE OF FILLING          ;                                  ;
;*****************************************************************************;



GETHASRETRIEDPTR:

        PSHB
        LDX     #HASRETRIED1      ; HAS RETRIED POINTER SET UP
        TST     STAGEFILLEN
        BEQ     XITHRC
        LDX     #STAGESEQ1
        LDAB    COMPNO
        DECB
        ABX
        TST     0,X
        BEQ     SETNMH
        LDAB    0,X
        CMPB    #THREE
        BEQ     SETNMH
        CMPB    #TWO
        BNE     CHFRH
        LDX     #HASRETRIED1ST2      ;
        BRA     XITHRC
CHFRH   CMPB    #ONE
        BNE     SETNMH
        LDX     #HASRETRIED1ST3      ;
        BRA     XITHRC
SETNMH  LDX     #HASRETRIED1      ;
XITHRC  PULB
        RTS


LSPCHG  LDAA    LS5CFG          ; CHECK FOR CHANGE IN LINE SPEED

;*****************************************************************************;
; FUNCTION NAME : GETRETRYCTR                                            ;
; FUNCTION      : SETS HAS RETRIED PTR DEPENDING ON STAGE OF FILLING          ;                                  ;
;*****************************************************************************;

GETRETRYCTR:
        PSHB
        LDX     #RETRYCTR1       ; HAS RETRIED POINTER SET UP
        TST     STAGEFILLEN
        BEQ     XITRC
        LDX     #STAGESEQ1
        LDAB    COMPNO
        DECB
        ABX
        TST     0,X
        BEQ     SETNMR
        LDAB    0,X
        CMPB    #THREE
        BEQ     SETNMR
        CMPB    #TWO
        BNE     CHFRT
        LDX     #RETRYCTR1ST2
        BRA     XITRC
CHFRT   CMPB    #ONE
        BNE     SETNMR
        LDX     #RETRYCTR1ST3
        BRA     XITRC
SETNMR  LDX     #RETRYCTR1       ; HAS RETRIED POINTER SET UP
XITRC   PULB
        RTS


 */
