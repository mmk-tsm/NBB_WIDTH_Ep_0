//////////////////////////////////////////////////////
// BBCalc.c
//
// Group of functions relating to Batch Blender calculations - from BBCalc74.asm
//
// M.McKiernan                          13-09-2004
// First Pass
//
// M.McKiernan                          20-09-2004
// Added CheckForStagedFill()
// Added SetCopyFlowRatesCommand()
// M.McKiernan                          29-09-2004
// Added CalculateColourCompensation()
// Added NewTargetWeightEntry()
// m_fMaxGramsPerPercent renamed to m_fMaxBatchSizeInKg
// Must activate the right fill output for Component being used for top-up, i.e. top-up component.
// Also must set for non-standard component configuration.
//
// M.McKiernan                          07-10-2004
// Added CheckProductionSetpointsTotal100()
// Added TareWeighHopper
// M.McKiernan                          13-12-2004
// Activated PutAlarmTable() and RemoveAlarmTable().
//
// P.Smith                              4/10/05
// modify CalculateWeightAfterRegrind as per the assembler.
// decide based on "use regrind as reference" set up whether to use the
// regrind as the reference.
// removed double call to  CalculateComponentTargetWeight
//
// P.Smith                              21/10/05
// nCompIndex set to g_cCompNo - 1 for top up component calculation
// asm CALCULATETOPUPTARGET
//
// P.Smith                              24/10/05
// Correct calculation of top up target, ensure that the g_fUsedTargetWeight
// is used when the top up component weight is being calculated and not the
// batch weight.
//
// P.Smith                              24/10/05
// Removed "testonly" code
//
// P.Smith                              15/11/05
// in CalculateWeightAfterRegrind, use variable rather than setpoint weight as
// the reference. If a fill problem exists, then use the set point as the ref
//
// P.Smith                              10/1/06
// removed ntemp that is unused.
//
// P.Smith                      23/2/06
// first pass at netburner hardware conversion.
// Bool -> BOOL
// #include <basictypes.h>
//
// P.Smith                      23/2/06
// implement todos
// CalculateComponentTargetWeight();   // CMPNTAR calculate targets for this component.
// CalcCompNTime( (int)g_cCompNo );                    // CALCMNTIM - COMP #N TIMES FOR #2...N
// CalculateComponentTargetWeight();   // CMPNTAR calculate targets for this component.
// CalcCompNTime( (int)g_cCompNo );                    // CALCMNTIM - COMP #N TIMES FOR #2...N
// ResetFillCounters();                    // reset fill counters
// CalcCompNTime( (int)g_cCompNo );                    // CALCMNTIM
// ffPointer = SetCPIPointer( g_cCompNo );
// Comp1stxTotWgt();   // CALCULATE TARGET COUNTS FOR COMP #1
// CalcCompNTime( (int)cCompNo );                  // CALCMNTIM
// #include "MonBat.h"
//
// P.Smith                      31/5/06
// make i and unsigned int.
//
// P.Smith                      14/6/06
// added void CalculateValveLatency( void )
//
// P.Smith                      16/11/06
// remove unused commented out code.
//
// M.McKiernan                      17/9/07
// Removed g_arrnMBTable[] & g_arrnMBTableFlags[], now in BatVars2
//
// P.Smith                      5/12/07
// Added CheckComponentPercentages to allow the component % to be checked.
// The component percentage deviation alarm is generated to show what component
// is generating the alarm.
// also takes care of the show instantaneous % flag and uses the round robin value
// if required
//
// P.Smith                      28/1/08
// correct deviation alarm settings, note that the system alarm word is not used to
// determine when the alarm should occurr or clear, there is an individual bit for
// each component like the fill alarms and this determines if the alarm should occur
// or clear.
//
// P.Smith                          30/1/08
// correct compiler warnings
// correct latency calculation casting to int
// change CheckComponentPercentages to void return
//
// P.Smith                          10/6/08
// call CloseDump on batseq set to fill
//
// P.Smith                          16/6/08
// when in top up mode, there is a problem with the set up of the configuration
// the config is set up but it is not copied into eeprom. This causes a checksum error
// every time the blender cycles. this is corrected by setting g_bSaveAllCalibrationToEEprom
// to true to save the calibration data. A check on the data for the correct value is done
// before g_bSaveAllCalibrationToEEprom flag is set.
//
// P.Smith                          10/7/08
// add hopper tare event to log
//
// P.Smith                          24/6/08
// remove g_arrnMBTable ,g_CurrentTime
//
// P.Smith                          14/8/08
// a problem has been seen with the top up software, where the the correct time
// is not being calulated for the topped up component.
// this is due to the fact that the calculation of the component on time is being
// done on the wrong component, it is done on the 1st component in the sequece
// call CopyComponentConfigurationToMB to copy component configuration to the modbus table
// this ensures  that the configuration on the panel is up to date.
//  when calling CalcCompNTime, use nTopUpComponent to calculate the topped up component.
//
// P.Smith                          31/3/09
// in TareWeighHopper if diagnostics password, then save
//
// P.Smith                          8/6/09
// added CheckForCorrectComponentTargets, only run if option is enabled
// on time is calculated, if less than 0.1 seconds, then an error is flagged
// and an alarm is created.
//
// P.Smith                          10/6/09
// modify check to allow for regrind when doing min checks.
// the component targets are adjusted accordingly.
// provide way to stop this test from running, material transport delay set to 99
//
// P.Smith                          1/10/09
// only check for min opening time if g_CalibrationData.m_bMinimumOpeningTimeCheck is
// enabled.
//
// P.Smith                          14/10/09
// in CheckForCorrectComponentTargets check for m_nMaterialTransportDelay != 99
//
// P.Smith                          20/10/09
// check m_bMinimumOpeningTimeCheck to determine if min opening time check should be done
//
// P.Smith                          27/11/09
// added TOPUPCOMPONENT instead of specific component no.
// remove set top up component, this is no longer needed as this is set up dynamically.
//
// P.Smith                          7/12/09
// calculate the max batch size allowing for other components, add the other components
// to calculate the max weight for the top up mode.
// note that the 1st component and the top up component are not added in.
//
// P.Smith                          9/12/09
// if top up mode and regrind and only 2 component in table, then do not calculate the
// top up target, the components should be allowed to fill as normal.
//
// P.Smith                          14/12/09
// there is a problem in the way that the top up is checked.
// the counts per percent is changed incorrectly when the target batch cannot
// be achieved.
// now the check is done using the weight directly and the top up weight is modified
// the top up weight is calculated by using the kg/percent by the current recipe
// correct some compiler warnings.
//
// P.Smith                          6/1/10
// allow for regrind in calculations of max kgs for top up.
// in CalculateWeightAfterRegrind, store g_fRegrindWeight and g_fWeightLeftAfterRegrind
// in CalculateColourCompensation, if regrind component used, then calculate the target
// weights using the remaining weight after regrind.
// also subtract off the regrind weight to know how much of the batch
// is left.

// M.McKiernan                          06-07-2020
// testing Git no real change.
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
#include    "FiftyHz.h"
#include    "BatVars.h"
#include "SetpointFormat.h"
#include    "BBCalc.h"
#include "MonBat.h"
#include "Alarms.h"
#include    "BatVars2.h"
#include    "Pause.h"
#include "Eventlog.h"
#include "MBProgs.h"

// Locally declared global variables
// Externally declared global variables
// Data
extern CalDataStruct    g_CalibrationData;
extern  structSetpointData  g_CurrentRecipe;

//////////////////////////////////////////////////////
// CalculateWeightAfterRegrind( void )          from ASM = CALCWGTAFTREG
//
//
// M.McKiernan                          13-09-2004
// First pass.
//////////////////////////////////////////////////////
void CalculateWeightAfterRegrind( void )
{
int nRegCompNo,nCompIndex;
int i;
float fRefWeight;

        nRegCompNo = (int)(g_cSeqTable[0] & 0x0F);  // get component no. from table, first position
        nCompIndex = nRegCompNo - 1;                     // component no.s in table start at 1, index is 0 based.
        if(g_bRegrindFillProblem)
            fRefWeight = g_fComponentTargetWeight[nCompIndex];   /* use set weight */
            else
            fRefWeight = g_fComponentActualWeight[nCompIndex];   /* use variable weight */

        g_fRegrindWeight = g_fComponentActualWeight[nCompIndex];
        g_fWeightLeftAfterRegrind = g_CalibrationData.m_fBatchSize - g_fComponentActualWeight[nCompIndex];

        if(g_CalibrationData.m_nUseRegrindAsReference && !g_bAnyFillByWeight)
        {
            // overall target = (Regrind wt / Reg. %) * (100 - Reg %).
            g_fUsedTargetWeight = (fRefWeight / g_CurrentRecipe.m_fPercentage[nCompIndex]) * (100.0 - g_CurrentRecipe.m_fPercentage[nCompIndex]);


        }
        else //OLDTRCG
        {
            g_fUsedTargetWeight = g_CalibrationData.m_fBatchSize - g_fComponentActualWeight[nCompIndex];
        }


        g_cCompNoDetails = g_cSeqTable[g_nSeqTableIndex + 1];   // get next component details from table
        g_cCompNo = g_cCompNoDetails & 0x0F;                        // component no.
        nCompIndex = g_cCompNo - 1;                     // component no.s in table start at 1, index is 0 based.

        CalculateComponentTargetWeight();   // CMPNTAR calculate targets for this component.
        CalcCompNTime( (int)g_cCompNo );                    // CALCMNTIM - COMP #N TIMES FOR #2...N

    if((g_bTopUpFlag) && (g_cSeqTable[g_nSeqTableIndex + 2]!= SEQDELIMITER))
    {  //asm CALCULATETOPUPTARGET
        g_nTopUpSequencing = FILLFIRST;
        g_lComponentTargetCountsTotal[nCompIndex] = g_lComponentTargetCounts[nCompIndex];

        //CMP1SETTOPUPWT // COMPONENT #1 TARGET WEIGHT
        g_fComponentTargetTopUpWeight[nCompIndex] = (float)g_lComponentTargetCountsTotal[nCompIndex] / g_CalibrationData.m_fWeightConstant;
        // Comp. top up wt in Imperial
        g_fComponentTargetTopUpWeightI[nCompIndex] = g_fComponentTargetTopUpWeight[nCompIndex] * WEIGHT_CONVERSION_FACTOR;
        // Wt used in CalculateComponentTarget() = (Topup% * batch size) / 100.
        g_fUsedTargetWeight = (g_CalibrationData.m_nPercentageTopUp * g_fUsedTargetWeight) / 100.0f;

        // Calculate target wt. for this component.
// --review--        CalculateComponentTargetWeight( );
    }
        CalculateComponentTargetWeight();   // CMPNTAR calculate targets for this component.
        CalcCompNTime( (int)g_cCompNo );                    // CALCMNTIM - COMP #N TIMES FOR #2...N
}

//////////////////////////////////////////////////////
// CheckForStagedFill( void )           from ASM = CHECKFORSTAGEDFILL
//
//
// M.McKiernan                          20-09-2004
// First pass.
//////////////////////////////////////////////////////
void CheckForStagedFill( void )
{
unsigned char cData;
int i;
int nTableIndex,nCompIndex;
BOOL bFound = FALSE;
BOOL bFinished = FALSE;

    if( g_CalibrationData.m_nStageFillEn )
    {
        nTableIndex = g_nSeqTableIndex;
        while(!bFound && !bFinished)
        {
            cData = g_cSeqTable[nTableIndex];
            nCompIndex = (int)(cData & 0x0F) - 1;   // comp index is 0 based.

            if(cData == SEQDELIMITER)       // are we at end??
            {
                // asm = XITATE

                bFinished = TRUE;   // indicate finished.
                g_bMultipleFilling = FALSE; // Show multiple filling is finisthed
                for( i=0; i < MAX_COMPONENTS; i++)
                {
                    g_bMulFillStarted[i] = FALSE;   // cleare multiple fill started flags
                }

            }
            else if(g_nStageSeq[nCompIndex] != 0)
            {
                    //asm = NOINC
                    bFound = TRUE;      // have found a component with staged filling.

                    g_nSeqTableIndex = nTableIndex; // store new table index.
                    g_cBatSeq = BATSEQ_FILL;            // advance sequence to filling
                    CloseDump();
                    g_bCycleIndicate = TRUE; //
                    g_bActivateLatency = TRUE;          // set the activatelatency
                    g_bMultipleFilling = TRUE;          // Show multiple filling
                    g_bFillAlarm = FALSE;               // reset comp. fill alarm
                    ResetFillCounters();                    // reset fill counters
                    g_nFillCtr = 0;
                    g_nFillCounter = 0;
            }

            nTableIndex++;          // advance to next byte in seq. table.

        }  //end of while loop.
    }
}

//////////////////////////////////////////////////////
// SetCopyFlowRatesCommand( void )          from ASM = SETCOPYCOMMAND
//
// Enable copy of flow rate for each component in the sequence table.
//
// M.McKiernan                          22-09-2004
// First pass.
//////////////////////////////////////////////////////
void SetCopyFlowRatesCommand( void )
{
    int i;
    int nCompIndex;
    i = 0;
    while(g_cSeqTable[i] != SEQDELIMITER && i < MAX_COMPONENTS) // table end or delimiter
    {
        nCompIndex = (int)(g_cSeqTable[i] & 0x0F) - 1;  // component no. in l.s. nibble
        g_bCopyCompFlowRate[nCompIndex] = TRUE;             // enable copy of flow rate
        i++;
    }
}

//////////////////////////////////////////////////////
// CalculateColourCompensation( void )          from ASM = CALCULATECOLOURCOMPENSATION
// Gist of how Top-up works:
// 1. First component in table fills 70% of its target. - This is the topped up component.
// 2. Colour component (always last element in Sequence table) fills to its target.
// 3. The remainder for component 1 is calculated based on the amount of the Colour (top-up ref component)
//      This amount (approx 30% of comp. 1) becomes the top-up  target and is placed into component #8.
//      Component #8 - This is the Top-Up component (a pseudo component).
//    I.e. component #8 is used to top-up component #1, it actually causes component #1 valve to open.
// 4. The top-up component is then filled. (i.e. remainder for 1st component).
// 5. The remaining components are filled as normal.
//
// The equation for calculating the target for the top-up component is as follows:
// (Note counts and weight are essentially the same)
// Top-up counts = (Actual counts for colour * 100% / Colour set %) * (Component #1 set % / 100%)
//
// Simplifying by taking out the 2 100%'s yields:
//  Top-up counts = (Actual counts for colour / Colour Set %) * Component # 1 Set %.
// The first term of the equation (Actual counts for colour / Colour %) is called "fCountsPerPercent" in
// the routine below.
//
// P.Smith                          23-09-2004
// First pass.
//////////////////////////////////////////////////////
void CalculateColourCompensation( void )
{
unsigned char cTemp;
float   fTopUpTargetCounts,fCountsPerPercent,fKgPerPercent;
float fAddition = 0.0f;
float fTargetWeight = 0.0f;
float fMaxWeight = 0.0f;
float fTopUpWeight = 0.0f;
float fBatchWeightLeft = 0.0f;

int nIndex,nCompIndex,nTopUpCompIndex,nTopUpComponent;
int nLastCompIndex;
unsigned int i;
double *ffPointer;
double ffCPI;
long    lCPI,lTopUpTargetCounts; // ---test only---
unsigned char cData;
    nCompIndex = 0;

    if(g_bVenturiFillByWeight)
    {
        g_bVenturiFillByWeight = FALSE;     //asm VENTURIFILLBYWEIGHTH
        //NOTOPUP
        nTopUpComponent = TOPUPCOMPONENT;
        nTopUpCompIndex = nTopUpComponent - 1;
        g_bNoTopUpRequired = TRUE;  // No Top up required
        g_lComponentCountsActual[nTopUpCompIndex] = 0;
        g_fComponentTargetWeight[nTopUpCompIndex] = 0;
        g_nTopUpSequencing = TOPUPCOLOUR;   // next stage is to top up colour
    }
    else
    {
        //CHKCOL
        nIndex = g_nLastCompTableIndex;  //LASTCOMPTABLEP
         cTemp = g_cSeqTable[nIndex];  //load byte from selected index
         g_cCompNo = cTemp & 0x0F;
         nLastCompIndex = (int)(cTemp & 0x0F)-1;    //mask off, comp. number.
         fCountsPerPercent = (float)g_lComponentCountsActual[nLastCompIndex] / g_CurrentRecipe.m_fPercentage[nLastCompIndex];

         fKgPerPercent = fCountsPerPercent / g_CalibrationData.m_fWeightConstant;
         fTopUpWeight = fKgPerPercent * g_CurrentRecipe.m_fPercentage[nCompIndex];
         if((g_CurrentRecipe.m_nRegrindComponent != 0)&&(g_CurrentRecipe.m_fPercentage[g_CurrentRecipe.m_nRegrindComponent-1]>0.0001f))   // Is there regrind??
         {
        	 fBatchWeightLeft = g_fWeightLeftAfterRegrind;
         }
         else
         {
           	 fBatchWeightLeft = g_CalibrationData.m_fBatchSize;
         }

         i = g_nSeqTableIndex+1;
         while(g_cSeqTable[i] != SEQDELIMITER && i < MAX_COMPONENTS) // table end or delimiter
         {
             nCompIndex = (int)(g_cSeqTable[i] & 0x0F) - 1;   //mask off, comp. number
             //if(i != g_nLastCompTableIndex)
             if(1)
             {
                 fTargetWeight = fBatchWeightLeft * (g_CurrentRecipe.m_fPercentage[nCompIndex]/100);
            //     if(fdTelnet>0)
            //     {
            //    	 printf("\n comp %d target is %3.4f",nCompIndex+1,fTargetWeight);
            //     }
                 fAddition += fTargetWeight;
             }
             i++;
         }
         if((g_CurrentRecipe.m_nRegrindComponent != 0)&&(g_CurrentRecipe.m_fPercentage[g_CurrentRecipe.m_nRegrindComponent-1]>0.0001f))   // Is there regrind??
         {
             fAddition += g_fRegrindWeight;
         }
         fMaxWeight = g_CalibrationData.m_fMaxBatchSizeInKg - fAddition;

         //if(fdTelnet >0)
         //printf("\n fTopUpWeight %f fMaxWeight %f",fTopUpWeight,fMaxWeight);

         //DOCALC1
            nCompIndex = (int)(g_cSeqTable[g_nSeqTableIndex] & 0x0F) - 1;   //mask off, comp. number.

         //   if(fdTelnet > 0)
         //   printf("\n counts per percent %f",fCountsPerPercent);
            if(fTopUpWeight < fMaxWeight)
            {
             fTopUpTargetCounts = fCountsPerPercent * g_CurrentRecipe.m_fPercentage[nCompIndex];
             //if(fdTelnet > 0)
             //printf("\n limit not exceeded fTopUpWeight %f",fTopUpWeight);
            }
            else
            {
             fTopUpTargetCounts = fMaxWeight * g_CalibrationData.m_fWeightConstant;
            // if(fdTelnet > 0)
            // printf("\n exceeded limit fTopUpWeight %f",fTopUpWeight);
            }
            lTopUpTargetCounts = (long)fTopUpTargetCounts;
       //  if(fdTelnet >0)
       //  printf("\n target top up %f\n",fTopUpTargetCounts);

        if(lTopUpTargetCounts > g_lComponentCountsActual[nCompIndex])
        {
                nTopUpComponent = TOPUPCOMPONENT;                // set top up component
                nTopUpCompIndex = nTopUpComponent - 1;
                // calculate remainder of topped up component, i.e. which remains to be filled.
                g_lComponentTargetCounts[nTopUpCompIndex] = lTopUpTargetCounts - g_lComponentCountsActual[nCompIndex];

                if(g_bVenturiFirstTime)
                {
                    g_bVenturiFirstTime = FALSE;
                    nIndex = g_nSeqTableIndex;
                    cTemp = g_cSeqTable[nIndex];  //load byte from selected index
                    nCompIndex = (int)(cTemp & 0x0F)-1; //mask off, comp. number, points to current component
                    g_ffComponentCPI[nTopUpCompIndex] = g_ffComponentCPI[nCompIndex]; // copy flow rate if 1st time
                }
                //NOT1ST
                CalcCompNTime( nTopUpComponent );                    // CALCMNTIM

                g_fComponentTargetWeight[nTopUpCompIndex] = (float)g_lComponentTargetCounts[nTopUpCompIndex] / g_CalibrationData.m_fWeightConstant;

                    // CONVTARTOLBS  (expanded inline)
                    // comp wt Imperial units version
                g_fComponentTargetWeightI[nTopUpCompIndex] = g_fComponentTargetWeight[nTopUpCompIndex] * WEIGHT_CONVERSION_FACTOR;

                if (g_lCmpTime[nCompIndex] > MIN_VALVE_OPEN_TIME) // YESTOPUP in assembler.
                {
                        g_bNoTopUpRequired = FALSE;  // top up is required
                        g_nTopUpSequencing = TOPUPCOLOUR;   // next stage is to top up colour
                }
                else
                {
                        ffPointer = SetCPIPointer( g_cCompNo );
                        ffCPI = *(ffPointer + nTopUpCompIndex); //get the CPI value
                        lCPI = (long)ffCPI;
                        // ensure flow rate is not  0
                        if(ffCPI == 0)
                        {
                            cData = g_cSeqTable[g_nSeqTableIndex];  //load byte from selected index
                            cData |= FILLBYWEIGHT;          // Fill by wt.
                            g_cSeqTable[g_nSeqTableIndex] = cData;  //
                            g_bNoTopUpRequired = FALSE;  // top up is required
                            g_nTopUpSequencing = TOPUPCOLOUR;   // next stage is to top up colour

                        }
                        else
                        {
//                          NoTopUp();
                        //NOTOPUP
                            nTopUpComponent = TOPUPCOMPONENT;
                            nTopUpCompIndex = nTopUpComponent - 1;
                            g_bNoTopUpRequired = TRUE;  // No Top up required
                            g_lComponentCountsActual[nTopUpCompIndex] = 0;
                            g_fComponentTargetWeight[nTopUpCompIndex] = 0;
                            g_nTopUpSequencing = TOPUPCOLOUR;   // next stage is to top up colour

                        }

                    }
            }
            else
            {
                //NOTOPUP
                nTopUpComponent = TOPUPCOMPONENT;
                nTopUpCompIndex = nTopUpComponent - 1;
                g_bNoTopUpRequired = TRUE;  // No Top up required
                g_lComponentCountsActual[nTopUpCompIndex] = 0;
                g_fComponentTargetWeight[nTopUpCompIndex] = 0;
                g_nTopUpSequencing = TOPUPCOLOUR;   // next stage is to top up colour
            }

    }
}




//////////////////////////////////////////////////////
// IndicateNoTopUpIsRequired( void )            ASM = NOTOPUP
//
// Sets NoTopup required and clears set and actuals for top up component
//
// M.McKiernan                          22-09-2004
// First pass.
//////////////////////////////////////////////////////
void IndicateNoTopUpIsRequired( int nTopUpCompIndex )
{

    g_bNoTopUpRequired = TRUE;  // No Top up required
    g_nTopUpSequencing = TOPUPCOLOUR;   // next stage is to top up colour
    g_lComponentCountsActual[nTopUpCompIndex] = 0;
    g_fComponentTargetWeight[nTopUpCompIndex] = 0;
}

//////////////////////////////////////////////////////
// CheckProductionSetpointsTotal100( void )         ASM = CHECKPRODSETE100P
//
// Checks current production setpoints total up to 100%
//
// Exit - returns a TRUE if good data, i.e. = 100%.
//
// M.McKiernan                          06-10-2004
// First pass.
//////////////////////////////////////////////////////
BOOL CheckProductionSetpointsTotal100( void )
{
unsigned int i;
float fTotal;
BOOL bGoodData = FALSE;

    fTotal = 0.0f;
   // total percentages for current no. of components (not including regrind component) add up to 100%
   for( i = 0; i < g_CalibrationData.m_nComponents; i++ )
   {
         if((i+1) != g_CurrentRecipe.m_nRegrindComponent )      // sum if not regrind
            fTotal += g_CurrentRecipe.m_fPercentage[i];
   }

    // total between 99.99% and 100.01% is ok.
    if( fTotal > 99.99f && fTotal < 100.01f )
    {
        bGoodData = TRUE;
    }
    if(!bGoodData)
    {
        if( (g_arrnMBTable[BATCH_SUMMARY_PROCESS_ALARMS] & PERALBIT) == 0 ) // percentage alarm bit not set???
        {  //
            PutAlarmTable( PERALARM,  0 );      // indicate alarm, Component no. is zero.
        }
    }
    else  //  i.e good data, %'s total 100%
    {
        if( g_arrnMBTable[BATCH_SUMMARY_PROCESS_ALARMS] & PERALBIT ) // percentage alarm bit set???
        {  //
            RemoveAlarmTable( PERALARM,  0 );       // clear % alarm., Component no. is zero.
        }
    }
/*
*/
    return(bGoodData);
}


//////////////////////////////////////////////////////
// NewTargetWeightEntry( void )         ASM = NEWTARWEIGHTENTRY
//
// Called after changes to batch target weight.
//
// M.McKiernan                          29-09-2004
// First pass.
//////////////////////////////////////////////////////
void NewTargetWeightEntry( void )
{
    unsigned char cCompNo;

    Comp1stxTotWgt();   // CALCULATE TARGET COUNTS FOR COMP #1
    cCompNo = g_cSeqTable[0] & 0x0F;    // first element in table.
    CalcCompNTime( (int)cCompNo );                  // CALCMNTIM
}



//////////////////////////////////////////////////////
// TareWeighHopper( void )          ASM = TareWeighHopper
//
// Called to tare weigh hopper, e.g  from Modbus write tare command.
//
// M.McKiernan                          12-10-2004
// First pass.
//////////////////////////////////////////////////////
void TareWeighHopper( void )
{

    if(g_bAutoCycleFlag || g_bStopped)  // must be stopped or in auto-cycle mode
    {
        g_lHopperTareCounts = g_lOneSecondWeightAverage;
        g_CalibrationData.m_lTareCounts = g_lOneSecondWeightAverage;
        if(g_bDiagnosticsPasswordInUse)
        {
            g_CalibrationData.m_lFactoryTareCounts = g_CalibrationData.m_lTareCounts;
        }

        g_lCurrentLCAtDValue = g_lOneSecondWeightAverage;
        g_bCalibrationDataChange = TRUE;    //CALFLG
        g_bWeighHopperTaredEvent = TRUE;
        g_nSDCardDelayTime = SDCARDCOMMANDDELAYTIME; // delay before write to sd

//        JSR    CALBT1   ;~UPDATE DATA IN VRAM -----Doesn't make sense in C
//        JSR    COPYCDMB ; COPY CALIB. DATA INTO MODBUS TABLE  --catered for from where function called.
    }

}
//////////////////////////////////////////////////////
// CalculateValveLatency( void )          ASM = CALCULATEVALVELATENCY
//
// Called to tare weigh hopper, e.g  from Modbus write tare command.
//
//////////////////////////////////////////////////////
void CalculateValveLatency( void )
{
    if(g_CalibrationData.m_fLatency != 0.0f)
    {
        g_nVavleCalculatedLatencyTime = (int)(BLENDER_PIT_FREQUENCY * g_CalibrationData.m_fLatency); // asm = CALCULATELATENCY  nbb--todo-- check this calc again.
    }
    else
    {
        g_nVavleCalculatedLatencyTime = (int)(BLENDER_PIT_FREQUENCY * NOMINAL_LATENCY); //nbb--todo-- check this calc again
    }
}


//////////////////////////////////////////////////////
// CheckComponentPercentages( void )         ASM = CHECKCOMPONENTPERCENTAGES
//
// Checks component percentages and if difference in set and actual
// is greater than a set deviation, a component % alarm is generated.
//
//////////////////////////////////////////////////////
void CheckComponentPercentages( void )
{
    int i;
    int nCompIndex;
    BOOL bDeviationExceeded = FALSE;
    BOOL bPauseWhenFinished = FALSE;
    float fPerDifference;
    unsigned char cData;
    i = 0;
    if(!g_bAnyFillByWeight && (g_CalibrationData.m_nDeviationAlarmEnable !=0))
    {
        while(g_cSeqTable[i] != SEQDELIMITER && i < MAX_COMPONENTS) // table end or delimiter
        {
            cData = g_cSeqTable[i];
            nCompIndex = (int)(cData & 0x0F) - 1;   // comp index is 0 based.

            if(g_bShowInstantaneousWeight)
            {
                if(g_CurrentRecipe.m_fPercentage[nCompIndex] > g_fComponentActualPercentage[nCompIndex])
                {
                     fPerDifference = g_CurrentRecipe.m_fPercentage[nCompIndex] - g_fComponentActualPercentage[nCompIndex];
                }
                else
                {
                    fPerDifference = g_fComponentActualPercentage[nCompIndex] - g_CurrentRecipe.m_fPercentage[nCompIndex];
                }
                if(fPerDifference > g_CalibrationData.m_fMaxPercentageDeviation[nCompIndex])
                {
                    bDeviationExceeded = TRUE;
                }

            }
            else
            {
                if(g_CurrentRecipe.m_fPercentage[nCompIndex] > g_fComponentActualPercentageRR[nCompIndex])
                {
                    fPerDifference = g_CurrentRecipe.m_fPercentage[nCompIndex] - g_fComponentActualPercentageRR[nCompIndex];
                }
                else
                {
                    fPerDifference = g_fComponentActualPercentageRR[nCompIndex] - g_CurrentRecipe.m_fPercentage[nCompIndex];
                }
                if(fPerDifference > g_CalibrationData.m_fMaxPercentageDeviation[nCompIndex])
                {
                    bDeviationExceeded = TRUE;
                }

            }

            if(bDeviationExceeded)
            {

                g_nComponentPerDeviationErrorCounter[nCompIndex]++;
                if(g_nComponentPerDeviationErrorCounter[nCompIndex] >= g_CalibrationData.m_nPauseOnErrorNo)
                {
                    g_nComponentPerDeviationErrorCounter[nCompIndex] = 0;
                    if(g_CalibrationData.m_nDeviationAlarmEnable == COMPPERENABLEDWITHPAUSE)
                    {
                        bPauseWhenFinished = TRUE;
                    }

                   PutAlarmTable( PERCENTAGEDEVIATIONALARM,  nCompIndex+1 );
                }
           }
           else
           {
              RemoveAlarmTable( PERCENTAGEDEVIATIONALARM,  nCompIndex+1 );       /* clear % alarm., Component no. is zero. */
           }
            i++;
        }
        if(bPauseWhenFinished)
        {
            ImmediatePauseOn();
        }
    }
}


//////////////////////////////////////////////////////
// CheckForCorrectComponentTargets( void )
//
// checks for valid component targets
//
//////////////////////////////////////////////////////
void CheckForCorrectComponentTargets( void )
{
    float    fTargetWt,fFlowRate,fTime,fMinTimeAllowed,fRegTargetWt,fRemainingWtAfterReg;
    BOOL     bRegrindPresent;
    unsigned int i,nRegCompIndex;
    fRemainingWtAfterReg = 0.0f;
    nRegCompIndex = 0;

    if(g_CalibrationData.m_bMinimumOpeningTimeCheck) // option is enabled
    {
        if(g_CurrentRecipe.m_nRegrindComponent == 0)   // is there a regrind component??
        {
            bRegrindPresent = FALSE;
        }
        else
        {
            bRegrindPresent = TRUE;
            nRegCompIndex = g_CurrentRecipe.m_nRegrindComponent - 1;
            fRegTargetWt = g_CalibrationData.m_fBatchSize * (g_CurrentRecipe.m_fPercentage[nRegCompIndex] / 100.0);
            fRemainingWtAfterReg = g_CalibrationData.m_fBatchSize - fRegTargetWt;
        }


        for( i = 0; i < g_CalibrationData.m_nComponents; i++ )
        {
            if((g_CurrentRecipe.m_fPercentage[i] > 0.001f)&&(g_ffComponentCPI[i] > 0.001f))
            {
                // calculate target component %
                if(bRegrindPresent)
                {
                    if(i == nRegCompIndex)
                    {
                        fTargetWt = g_CalibrationData.m_fBatchSize * (g_CurrentRecipe.m_fPercentage[i] / 100.0);
                    }
                    else
                    {
                        fTargetWt = fRemainingWtAfterReg * (g_CurrentRecipe.m_fPercentage[i] / 100.0);
                    }
                }
                else
                {
                    fTargetWt = g_CalibrationData.m_fBatchSize * (g_CurrentRecipe.m_fPercentage[i] / 100.0);
                }
                         // calculate flow rate from cpi
                fFlowRate = (BLENDER_PIT_FREQUENCY * 1000.0 * g_ffComponentCPI[i])/g_CalibrationData.m_fWeightConstant;
                fTime = (1000.0 * fTargetWt) / fFlowRate; // estimate on time in seconds
                //if(fdTelnet > 0)
                //printf("\ncmp %d target %1.4f flowrate %1.4f open time %1.4f",i+1,fTargetWt,fFlowRate,fTime);
                if(fFlowRate < 500) // less than 500 grams per second
                {
                    fMinTimeAllowed = 0.05;
                }
                else
                {
                    fMinTimeAllowed = 0.1;
                }
                if(fTime < fMinTimeAllowed)
                {
                    PutAlarmTable( COMPOPENTIMETOOSHORTALARM,  i+1 );      /* indicate alarm, Component no. is zero.  */

                //if(fdTelnet > 0)
                //printf("\n component %d flow rate of %1.4f is too low open time is %1.4f",i+1,fFlowRate,fTime);
                }
                else
                {
                    RemoveAlarmTable( COMPOPENTIMETOOSHORTALARM,i+1 );      /* indicate alarm, Component no. is zero.  */
                }

            }
        }
    }
}

/*
;*****************************************************************************;
; FUNCTION NAME : CALCULATELATENCY                                           ;
; FUNCTION      : CALCULATES LATENCY                             ;                                          ;                                          ;
; INPUTS        : NONE                                                        ;                       ;
;*****************************************************************************;


CALCULATELATENCY:
        LDD     LATENCY         ; READ LINE NO DATA.
        BNE     LATOK           ; CHECK FOR ZERO.
        LDD     #NORMALATENCY
LATOK   JSR     BCDHEX2
        LDE     #PITINDEX
        EMUL
        LDX     #TENTHOUSAND
        EDIV                    ; DIVIDE
        STX     VALVELATENCYTIMECALC ; VALVE LATENCY TIME SET
        RTS


*/


/*
TAREWEIGHHOPPER:
       TST      AUTOCYCLEFLAG   ; AUTO CYCLE.
       BNE      TARE          ; AUTOCYCLE
       TST      STOPPED
       BEQ      TAREX ; NOT STOPPED

TARE    LDE     AN1BUF  ;       READ A/D COUNTS g_lOneSecondWeightAverage
        LDX     #CALBLK
        LDAB    #OFFSOF         ; POSITION FOR TARE READING.
        ABX
        STE     0,X             ; STORE TARE READING
        STE     HOPPERTARECOUNTS
        LDAA    AN1BUF+2
        STAA    2,X             ; STORE TARED READING.
        STAA    HOPPERTARECOUNTS+2
        LDAA   #$AA
        STAA   CALFLG   ;~SET 'CAL DATA CHANGE' FLAG
        LDX     #AN1BUF
        JSR     BCDHEX3X        ; TO HEX
        STAB    CURRLCADH       ; "CURRENT LOAD CELL A/D" HEX
        STE     CURRLCADH+1     ;
        JSR    CALBT1   ;~UPDATE DATA IN VRAM
        JSR    COPYCDMB ; COPY CALIB. DATA INTO MODBUS TABLE
TAREX   RTS


*/
/*
CHECKPRODSETE100P:
        PSHM    D
        JSR     CHK100   ; CHECK IF THE % STILL ADD UP TO 100%
        LBCS    NOT100P   ;~C SET, INDICATES NOT
        LDAA    MBPROALRM               ;
        ANDA    #PERALBIT
        BEQ     XITPCK
        LDAA    #PERALARM
        JSR     RMALTB                     ;~
        BRA     XITPCK
NOT100P LDAA    MBPROALRM       ;
        ANDA    #PERALBIT       ; IS THE ALARM ALREADY ACTIVE ?
        BNE     XITPCK
        LDAA    #PERALARM
        JSR     PTALTB           ; ACTIVATE ALARM.
XITPCK  PULM    D
        RTS

*/



/*
NewTarWeightEntry:


        JSR     COMP1STXTOTWGT        ; CALCULATE TARGET COUNTS FOR COMP #1
        LDX     SEQPOINTER
        LDAA    0,X                   ; READ 1ST COMP NO
        ANDA    #$0F                  ; 1ST COMP
        STAA    COMPNO
        JSR     CALCMNTIM
        RTS


*/

/*
; CALCULATE COLOUR COMPONENT %.
;
CALCULATECOLOURCOMPENSATION:

        TST     VENTURIFILLBYWEIGHTH
        BEQ     CHKCOL
        CLR     VENTURIFILLBYWEIGHTH ; RESET.
        LBRA    NOTOPUP
CHKCOL  JSR     CLCREG7
        JSR     CLAREG7
        LDX     LASTCOMPTABLEP     ; COLOUR COMPONENT.
        LDAB    0,X
        ANDB    #$0F
        STAB    COMPNO             ; STORE COMPONENT NO.
        PSHB
        LDX     #CMP1CNAC
        DECB                    ;
        ABX
        ABX
        ABX
        LDD     #6
        STD     MATHCREG
        LDD     0,X
        STD     MATHCREG+2
        LDAA    2,X
        STAA    MATHCREG+4          ;

        LDD     #2
        STD     AREG
        PULB

        LDX     #PCNT1
        DECB
        ASLB
        ABX
        LDE     0,X
        STE     AREG+2
        JSR     FDIV          ; NO OF GRAMS PER %.

        LDX     #TEMPGRAMPERPER
        JSR     FFDREG          ; FROM D REGISTER.

        JSR     FPTINC          ; .
        CLRW    EREG
        LDD     MATHDREG+2
        STD     EREG+2
        LDAA    MATHDREG+4
        STAA    EREG+4          ; NEXT CALCULATION EXPECTING COUNTS IN EREG.
        JSR     CMPWCAL          ; CACULATE WEIGHT FOR THIS COMPONENT

        TST     EREG
        BNE     SUBMIN          ; SUBSTITUTE MIN VALUE.
        LDD     EREG+1          ; READ GRAMS PER %.
        CPD     MAXGRAMSPERPER  ; GRAMS PER PERCNT
        BLS     CALCTUW         ; CALCULATE TOP UP WEIGHT.

; CALCUALTE COUNTS.
;
SUBMIN  LDX     #TEMPGRAMPERPER
        JSR     FTDREG          ; FROM D REGISTER.
        LDX     #EREG
        JSR     CLRNM1          ; E = 0
        LDD     MAXGRAMSPERPER
        STD     EREG+2
        JSR     DIV1            ; CALCULATE COUNTS FOR COMP #1
        LDX     #TEMPGRAMPERPER
        JSR     CLRNM2          ; CLEAR REGISTER.
        LDD     #SIX            ; COUNTS.
        STD     TEMPGRAMPERPER  ; GRAMS PER PERCENT
        LDD     EREG+2
        STD     TEMPGRAMPERPER+2
        LDAA    EREG+4
        STAA    TEMPGRAMPERPER+4
        BRA     DOCALC1

CALCTUW

; * % FOR 1ST COMP.

DOCALC1 LDX     #TEMPGRAMPERPER
;        LDX     #AREG
        JSR     FTAREG
        LDX     SEQPOINTER      ; READ POINTER.
        LDAB    0,X
        ANDB    #$0F
        LDX     #PCNT1
        DECB
        ASLB
        ABX
        LDD     #2
        STD     MATHCREG
        LDD     0,X
        STD     MATHCREG+2
        JSR     FMUL          ; COUNTS FOR 1ST COMPONENT.

; DIFFERENCE
; CALCULATED VALUE - ACTUAL  -> DISPENSED TO COMPONENT 5.
;

        LDX     #AREG
        JSR     FFDREG
        LDX     SEQPOINTER     ; MAIN COMPONENT.
        LDAB    0,X
        ANDB    #$0F
        STAB    COMPNO
        LDX     #CMP1CNAC       ; ACTUAL WEIGHT FOR 1ST COMPONENT.
        DECB                    ;
        ABX
        ABX
        ABX
        LDD     #6
        STD     MATHCREG
        LDD     0,X
        STD     MATHCREG+2
        LDAA    2,X
        STAA    MATHCREG+4
        JSR     FSUB
        LDAA    MATHDREG
        LBMI     NOTOPUP         ; NO TOP UP.
        JSR     FPTINC          ;

        LDX     SEQPOINTER      ; READ POINTER.
        LDAB    0,X
        ANDB    #$0F
;        ADDB    #4
        LDAB    #EIGHT

        STAB    COMPNO
        LDX     #CMP1CNTG        ; TARGET FOR COMPONENT #1
        LDAB    COMPNO
        DECB
        ABX
        ABX
        ABX
        LDAB    MATHDREG+2
        STAB    0,X             ; TARGET FOR COMPONENT #N
        LDE     MATHDREG+3      ;
        STE     1,X             ; LSB

        JSR     BCDHEX3X        ; CONVERT BACK TO HEX
        PSHB
        PSHM    E               ;SAVE RESULT
        LDX     #CMP1CNTGH      ; COUNTS FOR COMPONENTS.
        LDAB    COMPNO
        DECB
        ABX
        ABX
        ABX
        PULM    E
        PULB
        STAB    0,X             ; STORE HEX TARGET
        STE     1,X             ; LSB

; IF THIS IS THE FIRST TIME THEN MOVE THE FLOW RATE TO THE TO THE MAIN COMPONENT.

        TST     VENTURIFIRSTTIME
        BEQ     NOT1ST          ; NO
        CLR     VENTURIFIRSTTIME ; RESET.

        LDX     SEQPOINTER     ; MAIN COMPONENT.
        LDAB    0,X
        ANDB    #$0F
        LDX     #CMP1CPI
        DECB
        ABX
        ABX
        ABX
        LDE     0,X             ; FLOW RATE FOR MAIN COMPONENT
        LDAA    2,X             ;
        LDX     #CMP1CPI
        LDAB    #EIGHT
        DECB
        ABX
        ABX
        ABX
        STE     0,X
        STAA    2,X             ; STORE FLOW RATE
NOT1ST:

        JSR     CALCMNTIM       ; COMP #N TIMES FOR #2...N


        LDX     #CMP1CNTG       ; LOAD COMP #1 TARGET COUNT STORAGE.
        LDAB    TARGNO          ; READ COMPONENT COUNTER
        DECB                    ;
        ABX
        ABX
        ABX
        CLRW    EREG
        LDD     0,X
        STD     EREG+2
        LDAA    2,X
        STAA    EREG+4          ; NEXT CALCULATION EXPECTING COUNTS IN EREG.
        JSR     CMPWCAL          ; CACULATE WEIGHT FOR THIS COMPONENT
        LDX     #CMP1TARWGT     ; COMPONENT #1 TARGET WEIGHT
        LDAB    TARGNO          ; READ COMPONENT COUNTER
        DECB                    ;
        ABX
        ABX
        ABX
        LDD     EREG
        STD     0,X             ; STORE WEIGHT
        LDAA    EREG+2          ;
        STAA    2,X             ;
        LDAB    COMPNO
        PSHB
        LDAB    TARGNO
        STAB    COMPNO
        JSR     CONVTARTOLBS
        PULB
        STAB    COMPNO

        LDX     #CMP1TIMS       ; LOOK AT TARGET.
        LDAB    COMPNO          ;
        DECB
        ABX
        ABX
        ABX
        TST     0,X              ; CHECK IF COUNTER >0
        BNE     YESTOPUP         ; FILL
        LDD     1,X              ; READ COUNTER.
        CPD     #MINVALVEOPENTIME ; MIN VALVE OPENTIME
        BLS     NOTOPUPMIN       ; NO TOP UP DUE TO MINIMUM.
YESTOPUP:

        CLR     NOTOPUPREQUIRED ; TOP UP REQUIRED.
XITCMC  LDAA    #TOPUPCOLOUR
        STAA    TOPUPSEQUENCING ;
        RTS

NOTOPUPMIN:
;        LDX     #CMP1CPI
        JSR     SETCPIPTR
        LDAB    #EIGHT
        DECB
        ABX
        ABX
        ABX
        TST     0,X
        BNE     NOTOPUP
        TST     2,X
        BNE     NOTOPUP

        LDX     SEQPOINTER      ;
        LDAA    0,X             ;
        ORAA    #FILLBYWEIGHT   ; FILL BY WEIGHT
        STAA    0,X
        BRA     YESTOPUP        ; PROCEED ON 1ST PASS


NOTOPUP:
        LDAA    #1
        STAA    NOTOPUPREQUIRED ; NO TOP UP REQUIRED.

; RESET THE TOP UP COMPONENT DATA.

        LDX     SEQPOINTER      ; READ POINTER.
        LDAB    0,X
        ANDB    #$0F
        LDAB    #EIGHT
        PSHB
        LDX     #CMP1CNAC       ; ACTUAL WEIGHT FOR 1ST COMPONENT.
        DECB                    ;
        ABX
        ABX
        ABX
        CLRW    0,X
        CLR     2,X
        PULB
        LDX     #CMP1TARWGT     ; ACTUAL WEIGHT FOR 1ST COMPONENT.
        DECB                    ;
        ABX
        ABX
        ABX
        CLRW    0,X
        CLR     2,X
        BRA     XITCMC          ; COMPENSATION CALCULATION.


;*****************************************************************************;
; FUNCTION NAME : NEEDFORTOUP                                                 ;
; FUNCTION      : CHECKS IS TOP IS REQUIRED.                                  ;
; IF NOT THEN RESET TOP UP FLAG.                                              ;                                   ;
;*****************************************************************************;

NEEDFORTOUP:
        TST     TOPUP                   ; IS TOP UP ENABLED
        BEQ     XITTUC                  ; EXIT TOP UP CHECK.
        LDX     #SEQTABLE               ; LOAD TABLE START ADDRESS.
        LDAA    1,X                     ;
        CMPA    #SEQDELIMITER           ; IS THERE ONLY 1 COMPONENT.
        BEQ     ON1CM                   ; ONLY 1 COMPONENT.
        TST     TOPUPF
        BNE     XITTUC                  ; ALREADY SET
        LDAA    #1
        STAA    TOPUPF                  ; SET TOP UP FLAG.
        STAA    FILLTARGETENTRYF        ; SET FILL TARGET
        BRA     XITTUC                  ; EXIT NEED FOR TOP UP.

ON1CM   TST     TOPUPF                  ; CHECK TOP UP FLAG FOR NOW.
        BEQ     XITTUC                  ; EXIT
        CLR     TOPUPF                  ; NO TOP UP
        LDAA    #1
        STAA    FILLTARGETENTRYF        ; SET FILL TARGET

XITTUC  RTS



*/
/*
SETCOPYCOMMAND:
        LDAB    #RAMBANK
        TBZK
        LDZ     #SEQTABLE
REPSET  LDAB    0,Z
        ANDB    #$0F
        LDX     #COPYFLOWRATECMP1
        DECB
        ABX                             ; PICK UP CORRECT BUFFER
        LDAA    #1
        STAA    0,X                     ; SET
        AIZ     #1
        LDAA    0,Z
        CMPA    #SEQDELIMITER
        BNE     REPSET                  ; REPEAT SET OF COPY COMMAND
        RTS


*/
/*

; CHECK FOR STAGED FILLING

CHECKFORSTAGEDFILL:
        CLRA
        TST     STAGEFILLEN             ; STAGE FILL ENABLE CLEAR ?
        BEQ     XITSTC                  ; EXIT STAGE SET
        LDAB    #RAMBANK
        TBZK
        LDZ     #STAGESEQ1
        LDX     SEQPOINTER             ; LOAD START
CHKNX   LDAB    0,X
        CMPB    #SEQDELIMITER           ; SEQUENCE DELIMITER
        BEQ     XITATE                  ; AT END, EXIT
        ANDB    #$0F
        LDZ     #STAGESEQ1
        DECB
        ABZ
        TST     0,Z
        BNE     NOINC
        AIX     #1                      ; TO NEXT COMPONENT
        AIZ     #1                      ; TO NEXT STAGESEQ
        BRA     CHKNX
NOINC   STX     SEQPOINTER
        LDAA    #FILL
        STAA    BATSEQ
        STAA    CYCLEINDICATE           ; CONTINUE THE CYCLE
        LDAA    #1
        STAA    ACTIVATELATENCY
        STAA    MULTIPLEFILLING
        CLR     FILLALARM       ; RESET COMP FILL ALARM
        JSR     RESETFILLCOUNTERS ; RESET FILL COUNTER
        CLR     FILLCTR
        CLR     FILLCOUNTER     ; RESET FILL COUNTER
XITSTC  RTS
XITATE  CLR     MULTIPLEFILLING         ; SHOW THAT MULTIPLE FILL IS FINISHED
        LDAA    #1                      ; STAGED FILLING IS FINISHED
        LDX     #MULFILL1STARTED
        LDAA    #MAXCOMPONENTNO
        JSR     CLRMEM

        BRA     XITSTC



*/
/*
;
;       CALCULATE WEIGHT AFTER REGRIND.
;       (ACTUAL WEIGHT / REG %)   * REMAINDER 100 -

;
CALCWGTAFTREG:
        JSR     CLRAC7
        LDX     SEQPOINTER
        LDAB    0,X             ;
        ANDB    #$0F            ; COMP NO
        PSHB                    ; SAVE
        TST     REGRINDFILLPROBLEM
        BEQ     USEACT
        LDX     #CMP1TARWGT
        BRA     CNCL            ; CONTINUE ON     ;
USEACT  LDX     #CMP1ACTWGT     ;
CNCL    DECB
        ABX
        ABX
        ABX                     ; ACCESS WEIGHT
        LDD     #FIVE
        STD     MATHCREG
        LDD     0,X
        STD     MATHCREG+2
        LDAA    2,X
        STAA    MATHCREG+4
        PULB
        PSHB
        LDX     #PCNT1          ; LOAD SET POINT
        DECB
        ASLB
        ABX
        LDD     #TWO
        STD     AREG
        LDD     0,X
        STD     AREG+2
        JSR     FDIV            ; CALCULATE COMPONENT REGRIND WEIGHT / REGRIND %
        LDX     #TEMP1S
        JSR     FFDREG          ; STORE THIS


        JSR     CLRAC7
        LDX     #FOUR
        STX     AREG
        LDX     #$0100
        STX     AREG+2

        PULB

        LDX     #PCNT1          ; LOAD SET POINT
        DECB
        ASLB
        ABX
        LDD     #TWO
        STD     MATHCREG
        LDD     0,X
        STD     MATHCREG+2
        JSR     FSUB            ; 100%- REG %
        LDX     #AREG
        JSR     FFDREG
        LDX     #TEMP1S
        JSR     FTCREG
        JSR     FMUL
        JSR     FPTINC
        LDE     MATHDREG+2
        LDAA    MATHDREG+4

        LDX     SEQPOINTER
        AIX     #1
        LDAB    0,X             ;
        STAB    COMPNODETAILS   ; COMP NO DETAILS STORED.
        ANDB    #$0F            ; COMP NO
        STAB    COMPNO
        JSR     CMPNTAR         ; COMPONENT TARGET CALCULATION
        JSR     CALCMNTIM       ; CALCULATE TIME ANYWAY.
        RTS
  */
 /*

CHECKCOMPONENTPERCENTAGES:
        LDAA     COMPONENTPERENABLE
        LBEQ     XITCMC1          ; NO CHECK ON ALARM CONDITIONS
        TST     ANYFILLBYWEIGHT ; ANY FILL BY WEIGHTS.
        BEQ     NOFBW           ; NO.
        CLR     ANYFILLBYWEIGHT ;
        LBRA     XITCMC1
NOFBW   CLR     COMPONENTPERALARM         ; COMPONENT ALARMS.
        LDAB    #RAMBANK
        TBZK
        LDZ     #SEQTABLE               ; TABLE START ADDRESS.
REPPERLC:
        PSHM    Z               ; SAVE TABLE POINTER
        JSR     CLRAC
        LDAB    #RAMBANK
        TBZK
        LDAB    0,Z             ; READ COMPONENT NO.
        TBA
        ANDB    #$0F
        STAB    PERALARMCOMPNO  ; STORE COMPONENT NO.

        PSHB
        LDZ     #PCNT1          ; COMPONENT 1 SETPT % (ABC.D%)
        TST     USEREGWITHIN100PER           ; USE REGRIND WITHIN 100%
        BEQ     USENPF                       ; EXIT.
        LDZ     #PCNT1COPY        ; USE COPIED SET POINT
USENPF  DECB
        ASLB
        ABZ
        LDD     0,Z
        STD     AREG+3
        PULB
        PSHB

        LDX     #BPCNT1         ;  % #1
        TST     SHOWINWEIGHT    ; ARE WE SHOW THE INSTANTANEOUS WEIGHT
        BNE     CONWP1
        LDX     #BPRRCNT1       ;  % #1
CONWP1  DECB
        ASLB
        ABX
        LDD     0,X
        STD     MATHCREG+3
        JSR     SUB             ; SUBTRACT TO CHECK DIFFERENCE.
        LDE     EREG+3          ; READ DIFFERENCE IN %
        PULB
        LDX     #MAXPCNT1DEV
        DECB
        ASLB
        ABX

        LDD     0,X
        CPD     EREG+3
        BHI     COMPPEROK       ; COMPONENT % IS OKAY.

PERAL:
        TST     REG_PRESENT
        BEQ     GENCAL
        LDAA    PERALARMCOMPNO
        CMPA    REGCOMP        ; IS THIS THE REGRIND COMPONENT
        BNE     GENCAL
        TST     REGRINDRETRY
        BEQ     COMPPEROK

GENCAL  LDX     #COMP1PERDEVERRORCOUNTER
        LDAB    PERALARMCOMPNO
        DECB
        ABX
        INC     0,X
        LDAA    0,X
        CMPA    PAUSEONERRNO
        BLT     NOPYET                          ; NO PAUSE YET
        CLR     0,X
        STAA    COMPONENTPERALARM
        LDAA    COMPONENTPERENABLE
        CMPA    #COMPPERENANDPAUSE
        BLT     NOPYET
        JSR     IMDPAUSEON
NOPYET  BRA     NXTCOMP         ; GO TO NEXT COMPONENT.
COMPPEROK:
        LDX     #COMP1PERDEVERRORCOUNTER
        LDAB    PERALARMCOMPNO
        DECB
        ABX
        CLR     0,X
NXTCOMP:
        PULM    Z               ;
        AIZ     #1
        LDAA    0,Z
        CMPA    #SEQDELIMITER
        LBNE     REPPERLC
        TST     COMPONENTPERALARM ; ANY COMPONENT ALARMS.
        BEQ     NOFLALARM
        LDAA    #PERFILLALARM
        JSR     PTALTB          ; ACTIVATE ALARM.
        LDAA    #PERALARMBIT
        ORAA    MBSYSALRM+1
        STAA    MBSYSALRM+1
        LDAA    #PERALARMBIT
        ORAA    AUXALARM
        STAA    AUXALARM
        LDAA    #1
        STAA    ALRMCFG           ;~ALARM CHANGE FLAG, FOR DISPLAY PURPOSES.

XITCMC1  RTS
*/

