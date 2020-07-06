//////////////////////////////////////////////////////
// CycleMon.c
//
// Foreground program used in cycling.  Advances the batch sequence as needed.
//
// M.McKiernan                          24-06-2004
// First Pass
// M.McKiernan                          10-09-2004
// Completed translating the program, now all sections from original assembler are included.
// Rearranged start of program, some if statements before main switch, to cater for some conditions that
// dont come under main switch.
// Following functiions added:
//  DoPhase2Clean( void );
// InitiateCleanCycle( void );
// StartCleanCycle( void );
// SetMixerTime( void );
// ResetCleanFinishMBBit( void );
// SetStartCleanMBBit( void );
// CheckForCleanPhase2( void );
// CheckHopperWeight( void );
// P.Smith                          09-11-2004
// Added CloseFeed, LeftCornerCleanOff,RightCornerCleanOff
// M.McKiernan                          17-11-2004
// Replaced g_bPauseFlag by g_nPauseFlag, there is more than one type of Pause.
// Removed unused variables nComp, bState.  Removed an if(1).
// Added PauseOn.
// M.McKiernan                          13-12-2004
// Activated the PutAlarmTable's and RemoveAlarmTable().
//
// M.McKiernan                          06-01-2005
// Activated BatchSecondsCalculation() & CalculateKgPerHour().
//
// P.Smith                          04-02-2005
// Call control batch algotithm.
// Call d/a per kg calculation.
//
// P.Smith                          10-02-2005
// Check if the round robin needs to be reset before calculating the rr value.
//
//
// P.Smith                          2-3-2005
// Batch length calculation call comments removed
// BatchGPMCalc is called.
//
// P.Smith                          4-3-2005
// Remove comments from ToManual
// Remove comments from StartUpOn.
//
// P.Smith                          9-3-2005
// Remove text from CalcCMR and change name to CalcCMRVoltage
//
// P.Smith                          10-3-2005
// Remove text from AcumulateComponentWeights
// Remove comments from CopyWeightsToMB
//
// P.Smith                          15-3-2005
// uncomment ClearThroughputTotals
//
//
// P.Smith                          31-3-2005
// Added SetEndCleanMBBit
//
// P.Smith                          9-8-2005
// Remove check for min value of 35000 on load cell a/d
// If tare alarm, if level sensor uncovered proceed with tare and
// generate alarm.
//
//
// P.Smith                          10-8-2005
// Copy in mce12-15 ASM listing
// use m_nIntermittentMixTime instead of m_nMixingTime for Micrabatch.
// No stop / run transition in single recipe mode
//
// P.Smith                          11-8-2005
// removed
// g_fPreviousBatchWeight = g_fCurrentBatchWeight; // current wt to prev. batch wt.
// AccumulateWeights(); //ACUMWT    - Accumulate roll and order wt.
// CopyWeightsToMB();      // copy to Modbus table.
// removed
//      if( (g_arrnMBTable[BATCH_SUMMARY_PROCESS_ALARMS] & TAREALBIT) ) // tare alarm bit set???
//      {  //
//            RemoveAlarmTable( TAREALARM,  0 );      // indicate tare alarm cleared, Component no = 0 i.e. not component alarm.
//      }
//
// If moulding is enabled, skip top cleaning and move to bottom cleaning.
// added checkofftime,CheckforsingleRecipe
//
//Stop / Run transition not effective if single recipe mode.
//Run / Stop transition not effective if single recipe mode.
//No stop / to run unless blender run option bit is set.
//No cycling if warmupctr is non zero.
//No cycling if blender run option bits clear.
//Corner cleans activated if blow every cycle is activated.
//Update batch variables added for offline mode.
//Remove duplication of acumwt in offline mode of operation.
//Remove mixoff set.
//No tare alarm clear.
//If tare alarm & level sensor uncovered, move on.
//Use Intmixtime for intermittent mixing.
//If moulding, move to stage 2 clean.
//Check for recipe waiting added & load per targets from comms.
//Check for single recipe mode at the end of the cycle & vacinhibit on call.
//On phase 2 clean, check for offline valve open
//Check for single recipe & cleaning activation.
//CheckOffTime added to determine if warmup period is required for a/d
//
// P.Smith                              16-08-2005
// name change g_OnBoardCalDataStruct to g_OnBoardCalData
// name change g_PowerDownTime to g_cPowerDownTime
//
// P.Smith                              17-08-2005
// else is missing in StartPhase1Clean
//
// P.Smith                              2/09-2005
// Correct g_CalibrationData.m_bCycleFastFlag, 0 indicates fast not slow
// allow intermittent mixing for all models
//
//
// P.Smith                              7/9/05
// Correct CheckHopperWeight check, allow to check for +ive and -ive difference.
// Correct DumpingWeightCheck ensure that it will move on.
//
//
// P.Smith                              12/9/2005
// CheckForVacuumSequence() call added
//
//
// P.Smith                              16/09/05
// ensure that g_bBatchHasFilled is set to FALSE
//
// P.Smith                              29/09/05
// call UpdateBatchVariables from the correct place
// correct the if /else for UpdateBatchVariables
// LICENSE_BLENDERRUNOPTIONBIT used in license check
// correct UpdateBatchVariables function and call.
//
//
// P.Smith                              4/10/05
// Rename low priority todos
//
// P.Smith                              5/10/05
// added void ResetCleanFinishMBBit( void )
//
// P.Smith                              20/10/05
// uncoment LoadPercentageTargetsFromComms
//
// P.Smith                              15/11/05
// check for fast cycle flag corrected. when g_CalibrationData.m_bCycleFastFlag
// is set, option is fast.
// check for micra blender type in SetMixerTime
//
// P.Smith                              1/12/05
// in InitiateCleanCycle check for clean license bit being set.
// before starting the cleaning process.
//
// P.Smith                              2/12/05
// OpenFeed removed before CheckForVacuumSequence
//
// P.Smith                              10/1/06
// Corrected warnings
//
//
// P.Smith                      23/2/06
// first pass at netburner hardware conversion.
// Bool -> BOOL
//#include <basictypes.h>
//
// P.Smith                      24/2/06
// implement Tarend();
// #include "weight.h"
//
// P.Smith                      28/2/06
// AccumulateWeights(); //ACUMWT    - Accumulate roll and order wt.
//#include "Acumwt.h"
//
// P.Smith                      19/4/06
// call CheckForVacuumSequence(); // ASM CHECKFORVACSEQ
//#include "Vaccyc.h"
//
// P.Smith                              31/5/06
// removed check on f(g_nDmpSec > nAfterTareTime)   //Finished after-tare time??
// (g_nTransferSetpointsFlag != TRANSFERSPFROMCOMMSID) removed.
// this stopped the blender from cycling if the recipe was downloaded while in the cycle.
//
// P.Smith                              19/7/06
// added clearing of target counts, actual counts, pit opening time in clearunused()
//
// P.Smith                              16/11/06
// remove level sensor watch dog software.
// remove SetLevelSensorWatchDog call
//
//
// P.Smith                               17/11/06
// modify grams per meter calculation to be called after kg/h calc ;
// this will give a more accurate reading of the g/m value.        ;
// use g_bCycleIndicate instead of g_nCycleIndicate
// Remove level sensor watch dog software.
//
// P.Smith                                               5/2/07
// Remove unused iprintf
//
// P.Smith                                              28/2/02
// Modify grams per meter calculation to be called after kg/hr calculation
// This will ensure the line speed over comms will function properly and
// give a more accurate reading of grams / meter.
// Blow every cycle cyclecleanblowingtime used for the on time.    ;
// if offline mode no delay after closing feed valve and dumping   ;
// ensure that grams per meter is calculated every time even though;
// the estimated kg/h value is being used. this allows the counters;
// to be reset, if they are not reset then the length on the next  ;
// cycle is double and cuased the g/m to reduce and the screw speed;
// to increase.
// set  g_nAirJet2OnCounter to CYCLECLEANBLOWINGTIME;
//
// P.Smith                                              12/3/02
// called ultrasonic calculation on batch drop
//
// P.Smith                                              14/3/02
// only call UltrasonicCalculation if g_bUltrasonicLIWInstalled is set.
//
// P.Smith                                              1/5/07
// Some diagnostics for multiblend added
//
// P.Smith                                              7/6/07
// Remove printf on multiblend.
//
// P.Smith                      22/6/07
// name change to proper blender names.
//
// M.McKiernan                      17/9/07
// Removed g_arrnMBTable[] & g_arrnMBTableFlags[], now in BatVars2
//
// P.Smith                          3/10/07
// only cycle blender on stop / run transition if level sensor is uncovered.
//
// P.Smith                          7/11/07
// Remove double calculation of kg/hr in offline mode
// Removed CalculateKgPerHour & BatchSecondsCalculation in offline mode.
// this meant that the calculation was being done twice.
//
// P.Smith                          5/12/07
// in StartCleanCycle suspend control for n cycles by setting g_nSuspendControlCounter
// Set holdofflsensoralctr to holdofflsensoralinseconds on finish  ;
// of cleaning                                                     ;;
// check cleaning to determine if full or mixer clean, do not use  ;
// the moulding option to check this anymore.
// set suspendcontrolcounter on clean activation                    ;
//
// P.Smith                          10/1/08
// remove printf
//
// P.Smith                          11/1/08
// use SUPERFASTSETTLINGTIME if set to super fast speed.
// Also use SUPER_FAST_AFTER_TARE_TIME,SUPER_FAST_MATERIAL_FALL_TIME
// use SUPER_FAST_TIME_BEFORE_TARE
//
// P.Smith                          15/1/08
// added g_bSHAHardwareCheckFailed check, no cycle if this is set to TRUE.
//
// P.Smith                          30/1/08
// correct compiler warnings
//
// P.Smith                          28/4/08
// if blender is set to mixer clean only, remove the corner cleans as this is taking too long
// for the smaller machines.(DoPhase2Clean)
//
// P.Smith                          28/4/08
// in case check for BATSEQ_SETTLEAFTERBANGID, make the blender cycle as it should after
// the initial top clean, it is important that the next batch is filled after the cleaning
// cycle has been initiated
// check for the single mode recipe, if not make the blender cycle. this ensures that
// the blender can fill the next batch before the cleaning is finished.
//
// P.Smith                          17/5/08
// in CheckForSingleRecipeClean set g_bAllowMultiBlendCycle to true to allow multi
// blend vacuum cycle to start
//
// P.Smith                          3/6/08
// when g_cBatSeq is set to BATSEQ_FILL, call CloseDump to ensure that the dump is closed
// when the filling starts
//
// P.Smith                          12/6/08
// set g_nDisableDTAPerKgCtr to NOOFCYCLES_TODISABLEDAPERKG_DURINGCLEAN
//
// P.Smith                          16/6/08
// added WeighHopperHasOpened to check if the weigh hopper has opened.
// a check is done to check the difference in the current weight and the
// weight filled at the end of the batch.
// if the dump flap has not opened, then the blender is not tared to try and
// keep the line going.
// remove ResetFillCounters();
//
//
// P.Smith                          1/7/08
// remove Micrabatch related stuff
//
// P.Smith                          16/7/08
// Set g_bCleaningInitiatedEvent to TRUE to log cleaning event or cleaning start
// add blender leak alarm to event log
// add sd card actions to event log, ie when the log has been completed.
// add leak alarm to event log.
//
// P.Smith                          21/7/08
// remove licensing check.
//
// P.Smith                          24/6/08
// remove g_arrnMBTable ,g_CurrentTime,g_cPowerDownTime
//
// P.Smith                          11/9/08
// if top up mode do not clear the data for this eight component.
// check for vacuum loading and increase the settling time
//
// P.Smith                          11/9/08
// put in else statement for checking vacuum loading.
//
// P.Smith                          22/9/08
// if liquid additive is enabled, then add liquid additive weight to g_fPreviousBatchWeight
// for control.
// also do not clear the liquid additive component variables if pib is enabled.
//
// P.Smith                          23/9/08
// modify leak alarm to subtract off g_fOneSecondHopperWeight rather than the added weight.
//
// P.Smith                          26/9/08
// added DumpMaterialFromWeighHopper, this function is called when the dump flap is opened.
// added CheckIfMaterialHasDumped checks if material has been dumped from the weigh
// hopper
// if (g_CalibrationData.m_bLiquidEnabled then previous batch weight g_fCurrentBatchWeight +
//  g_fComponentActualWeight[g_nLiquidAdditiveComponent-1];
// use g_fOneSecondHopperWeight instead of g_fWeightInHopper in WeighHopperHasOpened
// if liquid additive is enabled, do not add it to the sequence table.
// in CheckHopperWeight, use g_fOneSecondHopperWeight rather than g_fWeightInHopper
//
// P.Smith                          30/9/08
// if tare alarm goes away, then clear the alarm.
//
// P.Smith                          3/10/08
// in CheckHopperWeight use g_fOneSecondHopperWeightAtEndOfCycle & g_fOneSecondHopperWeight
//
// P.Smith                          8/10/08
// in stop to run transition, copy g_fOneSecondHopperWeight to g_fOneSecondHopperWeightAtEndOfCycle
// this ensures that if the blender is stopped that it will not produce a leak alarm.
// when the dump flap is opened.
//
// P.Smith                          9/10/08
// store g_fPreviousBatchWeight to g_fThroughputCalculationBatchWeight
// only add in liquid weight if comp % is non zero.
//
// P.Smith                          9/10/08
// read batch weight as before g_fPreviousBatchWeight
//
// P.Smith                          16/10/08
// todo -> lp
//
// P.Smith                          17/11/08
// remove ultrasonic stuff from here
// increment g_nCycleCounter in UpdateBatchVariables
// remove commented out code
//
// P.Smith                          4/12/08
// increment history tare alarm counter and tare alarms
//
// P.Smith                          15/1/09
// reinitialise expansion card when the blender is switched from stop to run.
// the expansion card was getting a glitch on the reset line
//
// P.Smith                          22/1/09
// when the expansion is being reinitialised, stop any external spi access from coming
// in by setting g_bSPIBusBusy = TRUE; and setting g_bSPIBusBusy = FALSE; when finished.
//
// P.Smith                          28/1/09
// put back in check for license, do not allow cycle or stop to run to function if no license
//
// P.Smith                          2/3/09
// increment g_s24HrOngoingHistoryEventsLogData tare, leak, dump flap not opened alarm ctr
//
// P.Smith                          12/3/09
// in CheckIfMaterialHasDumped, only do the check on the weight if in the dump cycle
// otherwise move on.
// only add to manual leak alarm to event log if the blender was originally in auto.
// in CheckHopperWeight, if g_bNoLeakAlarmCheck is true, do not run leak alarm check.
// in DumpingWeightCheck if tare alarm has been detected, only cycle if the dump valve
// has been detected as open.g_bDumpFlapHasOpened indicates if the dump valve has opened
//
//
// P.Smith                          22/7/09
// added check for fast settling.
//
// P.Smith                          7/9/09
// add back in refill of weigh hopper when stop to run transition is initiated.
// do not refill the weigh hopper if the weight is greater than 50 grams.
// this allows for material getting stuck in the weigh hopper if the blender
// is not fully empty.
// added possibility of different min weights for different machines
// the mega min is set at 500 grams, there is a possibility that granules
// could get stuck to the side of the weigh hopper in cases of increased
// static.
// set g_lLoadTime to 0 to ensure that this counter is set to 0.
//
//
// P.Smith                          10/9/09
// change orientation of fast flag m_bCycleFastFlag
// if fast cycle, no double dump.
//
// P.Smith                          17/9/09
// put back in cycle speed check as it was before.
// if set to 1, this is the normal speed, 0 is the fast speed.
// this needs to be renamed nbb--todo--
//
// P.Smith                          17/10/09
// do not run leak alarm check if g_nAfterEndOfCycleCounter is non zero.
// a problem has been seen where when in fast mode, the blender shows a leak alarm
// if the blender is cycling very fast one batch after another, the one second average is not updated
// fast enough to have the current batch weight. it is lagging behind.
// therefore if the batch has dropped the batch within 5 seconds after end of cycle,
// the leak alarm check is not run.
//
// P.Smith                          27/11/09
// added TOPUPCOMPONENT instead of specific component no.
//
// P.Smith                          4/3/10
// only initiate cleaning if cleaning flag is non zero.
// remove check for license, if the license is not enabled, the function
// is now disabled.
//
// P.Smith                          25/3/10
// add check for security fail ctr exceeding max value.
// if it does crash the system.
// this check only takes place if not in the cycle
//
// P.Smith                          25/3/10
// remove check for security failure code, set code and crash
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
#include "CycleMonitor.h"
#include    "MonBat.h"
#include    "Cleanio.h"
#include    "Pause.h"
#include "Alarms.h"
#include "KghCalc.h"
#include "Conalg.h"
#include "LSpeed.h"
#include "GPMCalc.h"
#include "Acumwt.h"
#include "Mbprogs.h"
#include "Exstall.h"
#include "Batvars2.h"
#include "TimeDate.h"
#include "Vaccyc.h"
#include "CycleMonitor.h"
#include "ProgLoop.h"
#include <basictypes.h>
#include "weight.h"
#include "UltrasonicCalc.h"
#include "Eventlog.h"
#include "SetpointFormat.h"
#include "HistoryLog.h"
#include "MCP23Sexp.h"
#include "License.h"
#include "Security.h"
#include <Bsp.h>



// Locally declared global variables
// Externally declared global variables
// Data
extern CalDataStruct    g_CalibrationData;
//extern float  g_fWeightAtoDCounts;

extern  int g_nKeyCode;
extern  int g_nLanguage;
extern  int g_nGeneralSecondTimer;
extern  structMeasurementUnits  g_MeasurementUnits;
extern  OnBoardCalDataStruct   g_OnBoardCalData;
extern  structSetpointData  g_CurrentRecipe;
extern  WORD g_nTimeHashFail;
extern WORD g_nSHAHashOfIdFail;
extern WORD g_nSHASecretFailCtr;


extern  int g_nDisplayScreen;
BOOL g_bLSStateT;       // testonly.

//////////////////////////////////////////////////////
// CycleMonitor( void )         from ASM = CYCMON
// Monitors the batch cycling for blender.
//
//
// M.McKiernan                          24-06-2004
// First pass.
//////////////////////////////////////////////////////
void CycleMonitor( void )
{
unsigned int nTime;
BOOL bFinished = FALSE;
unsigned int nTimeInTenths;
unsigned int nAfterTareTime,nStatus,nMask;
    // takes care of first batch case, which is common to all of switch cases
    // See NOTSTL in assembler.
//  if(g_bFirstBatch && g_cBatSeq != BATSEQ_BYPASSID)
//  {
    if( g_bFirstBatch &&  g_cBatSeq == BATSEQ_AUTOCYCLETARE )
    {
                    // Autocycle delay reached??
                    if(g_nDmpSec > AUTOCYCLEDELAY)
                    {
                        // must put into Autocycle mode.
                        g_bAutoCycleFlag = TRUE;
                        //NORMTARE
                        //NormTare();
                        Tarend();
                        // --REVIEW--
                        // TareForAutoCycler();

                        g_cBatSeq = BATSEQ_LEVPOLL; // advance sequence.
                        g_nDmpSec = 0;  // clear cycle timer.
                    }
            bFinished = TRUE;   // Dont enter main switch statement.
    }
    //  else  //NOAUTOCYCLE
    //  {
         // Stop Transition flag set
    if(!g_bCycleIndicate)
    {
    	if(g_nTimeHashFail >= MAX_TIME_FAIL_IN_SECONDS) //security--tsm--
    	{
    	    g_ProcessData.m_bySecurityFailErrorCode = TIME_HASH_FAIL_ERROR_CODE;
    		SaveAllProcessDataToEEprom();
    		Crash2();
    			// crash system
    	}
    	else if(g_nSHAHashOfIdFail >= MAX_HASH_OF_ID_FAIL_IN_SECONDS) //security--tsm--
    	{
    	    g_ProcessData.m_bySecurityFailErrorCode = ID_HASH_FAIL_ERROR_CODE;
    		SaveAllProcessDataToEEprom();
    		Crash2();
        			// crash system
	   }
       else if(g_nSHASecretFailCtr >= MAX_SHA_SECRET_FAIL_IN_SECONDS)
       {
           g_ProcessData.m_bySecurityFailErrorCode = SHA_SECRET_FAIL_ERROR_CODE;
    	   SaveAllProcessDataToEEprom();
    	   Crash2();
           // crash system
       }
       else if(g_ProcessData.m_bySecurityFailErrorCode != SECURITY_PASS_CODE)
       {
    	   g_ProcessData.m_bySecurityFailErrorCode = SECURITY_PASS_CODE;
    	   SaveAllProcessDataToEEprom();
       }
    }



   if(g_bStopTransition)
   {
        g_bStopTransition = FALSE;
        if(g_CalibrationData.m_nBlenderMode != MODE_SINGLERECIPEMODE)
        {
            // CYCLEINDICATE = 0
            g_bCycleIndicate = FALSE;
            // JSR CLEARTHROUGHPUTTOTALS
            ClearThroughputTotals( );
            // JSR MAKECYC
            MakeCycle();
            // done.
            bFinished = TRUE;   // Dont enter main switch statement.
        }
   }
   else if(g_bRunTransition)//NOSTTR no stop transition
   {
                g_bRunTransition = FALSE;
                g_bSPIBusBusy = TRUE;
                SPI_Select_EXPANSION1();
                InitialiseExpansion23S17();
                g_bSPIBusBusy = FALSE;


            //Run transition?
               if(!g_nPauseFlag) //Not Paused?
               {
                    if(g_CalibrationData.m_nBlenderMode != MODE_SINGLERECIPEMODE)
                    {
                        if(((g_OnBoardCalData.m_cPermanentOptions & LICENSE_BLENDERRUNOPTIONBIT) || (g_OnBoardCalData.m_cTemporaryOptions & LICENSE_BLENDERRUNOPTIONBIT)) && !g_bSHAHardwareCheckFailed)   // if blender run option enabled
                        // run if blender license is enabled
                        {
                            g_fOneSecondHopperWeightAtEndOfCycle = g_fOneSecondHopperWeight;

                            if(g_fOneSecondHopperWeight < MINIMUM_WEIGHT_TO_ALLOW_HOPPER_REFILL)
                            {
                            //MAKEDUMP:
                            //DMPSEC = 0
                            g_nDmpSec = 0;
                            // set Sequence to Dump.
                            g_cBatSeq = BATSEQ_DUMP;
                            }
                            bFinished = TRUE;   // Dont enter main switch statement.
                        }
                    }
                }
  }  // end for if run transition.

// section to cater for NOSENSOR in assembler
//NOSENSOR
   if(g_CalibrationData.m_nBlenderMode == MODE_OFFLINEMODE && g_bOffLSF)
   {
      g_bOffLSF = FALSE;
      g_sOngoingHistoryEventsLogData.m_nLevelSensorSwitchesAcknowledged++;
        if(g_nPauseFlag || g_bBatchReady)
        {
            bFinished = TRUE; // exit if either set.(XITOF1)
        }
        else
      {
            CloseFeed();   // Close pinch valve in throat.
            g_nOffline_Tracking = BATSEQ_OFFDELADMPID;      // "Delay after dump ID";   // OFFLINE_TRK = OFFDELADMPID
            g_nOffTime1 = 0;
            if(!g_nPauseFlag)
            {
              // ASM = ONLINE
                NoSensorOnline();
            }
        }

   }
    else    // i.e. online
    {
      // ASM = ONLINE
        NoSensorOnline();
    }

 if(!bFinished) // dont run large switch statement if bFinished set
 {
   switch(g_cBatSeq)          // BATSEQ
    {
        case BATSEQ_AUTOCYCLETARE: //Autocycletare only activated once on start-up.
                //first batch & AutoCycleTare??
                if(g_bFirstBatch && g_cBatSeq == BATSEQ_AUTOCYCLETARE)
                {
                    // Autocycle delay reached??
                    if(g_nDmpSec > AUTOCYCLEDELAY)
                    {
                        // must put into Autocycle mode.
                        g_bAutoCycleFlag = TRUE;
                        //NORMTARE
                        //NormTare();
                        Tarend();
                        // --REVIEW--
                        // TareForAutoCycler();

                        g_cBatSeq = BATSEQ_LEVPOLL; // advance sequence.
                    }
                    else
                    {
                        //settling period not elapsed - done
                        bFinished = TRUE;
                    }
                }

                break;
      case BATSEQ_BYPASSID:   //
//          if( BubBrk )                            // Paraic - not being used!! - BUBBRK not set cleared in code.
//              OpenByPass();
//          else
//              CloseByPass();
           break;
      case BATSEQ_SETTLING:   //

            if(g_CalibrationData.m_bVacLoading && (g_cRetryActive != 0))
            {
                nTime = 150;
            }
            else if(g_bFastSettling)
                nTime = SUPERFASTSETTLINGTIME;
            else if(g_CalibrationData.m_bCycleFastFlag)                  // batch cycling fast or slow - settling time)
                nTime = NORMALSETTLINGTIME;
            else
               nTime = SUPERFASTSETTLINGTIME;
              if(g_nDmpSec > nTime)
            {
                //JSR RESETAVG -
                g_nAvgSumCounter = 0;   //reset sum entry counter
                g_lAvgSum = 0;              // reset sum
                g_bWeightAvgAvail = FALSE;  // CLEAR Weight avg AVAILABLE FLAG
                // BATSEQ = CALC
            g_cBatSeq = BATSEQ_CALC;
            }

            //NOTSTL i.e. not settled
            {
                //first batch & AutoCycleTare??
                if(g_bFirstBatch && g_cBatSeq == BATSEQ_AUTOCYCLETARE)
                {
                    // Autocycle delay reached??
                    if(g_nDmpSec > AUTOCYCLEDELAY)
                    {
                        // must put into Autocycle mode.
                        g_bAutoCycleFlag = TRUE;
                        // was NORMTARE
                        // NormTare();
                        Tarend();
                        // --REVIEW--
                        //TareForAutoCycler();

                        g_cBatSeq = BATSEQ_LEVPOLL; // advance sequence.
                    }
                    else
                    {
                        //settling period not elapsed - done
                        bFinished = TRUE;
                    }
                }
                else
                { // NOAUTOCYCLE
                    //  Stop Transition flag set
                    if(g_bStopTransition)
                    {
                        g_bStopTransition = FALSE;
                        // CYCLEINDICATE = 0
                        g_bCycleIndicate = FALSE;
                        // JSR CLEARTHROUGHPUTTOTALS
                        ClearThroughputTotals( );
                        // JSR MAKECYC
                        MakeCycle();

                        // done.
                        bFinished = TRUE;
                    }
                    else //NOSTTR no stop transition
                    {
                        //Run transition?
                        if(g_bRunTransition)
                        {
                            if(!g_nPauseFlag) //Not Paused?
                            {
                                g_bRunTransition = FALSE;
                                if(g_bLevSen1Stat == UNCOVERED)
                                {
                                   //MAKEDUMP:
                                   //DMPSEC = 0
                                    g_nDmpSec = 0;
                                    // set Sequence to Dump.
                                    g_cBatSeq = BATSEQ_DUMP;
                                }
                            }
                        }

                    }

                }

            }
           break;
      case BATSEQ_CALC:   // -----todolp------  this is testonly to get cycle started.
/*  Section be catered for by MonitorBatch(). */
           break;
      case BATSEQ_LEVPOLL:                                  // polling level sensor.
                if(g_nCleanCycleSeq == CLEANPHASE2ID)
                {
                    DoPhase2Clean();    //DOPHASE2CLEAN
                }
                else if(g_CalibrationData.m_nBlenderMode == MODE_OFFLINEMODE && g_nPauseFlag )
                {
                    ;   // exit if offline and paused - catered for by if before switch statement.
                }
                else  //ONLI
                {
                    if(g_bDmpLevSen == TRUE)
                    {
                        // ISACTIVE
                        if(!g_bStopped && (g_nWarmUpCounter == 0))
                        {
                        if(((g_OnBoardCalData.m_cPermanentOptions & LICENSE_BLENDERRUNOPTIONBIT) || (g_OnBoardCalData.m_cTemporaryOptions & LICENSE_BLENDERRUNOPTIONBIT)) && !g_bSHAHardwareCheckFailed)   // if blender run option enabled
                        {
                            g_sOngoingHistoryEventsLogData.m_nLevelSensorSwitchesAcknowledged++;
                            g_bDmpLevSen = FALSE; //clear Dump level sensor flag
                            if(g_CalibrationData.m_bBlowEveryCycle) // asm = BLOWEVERYCYCLE
                            {

                                if(g_nCleanCycleSeq == 0) // Do not do during clean cycle.
                                {
                                    g_nLeftCornerCleanOnCounter = CYCLECLEANBLOWINGTIME;
                                    g_nRightCornerCleanOnCounter = CYCLECLEANBLOWINGTIME;
                                    g_nAirJet2OnCounter = CYCLECLEANBLOWINGSHORTTIME;
                                }
                            }

                            SetMixerTime();  //.
                            CloseAll( );    // switch off all the valves/(fill outputs).
                            CalculateHopperWeightAvg( );    //CALHWTAV
                            g_fCurrentBatchWeight = g_fAvgHopperWeight; // Store to current batch wt.
                              // Not first weigh cycle??
                            if(!g_bFirstWCycle)
                            {
                                CheckHopperWeight(); //CHECKHOPWT
                            }
                            if(g_CalibrationData.m_nBlenderMode == MODE_OFFLINEMODE)
                            {
//OFFLINE:
                                if(g_nPauseFlag || g_bBatchReady)  //paused or ready for pause??
                                {
                                    ; //XITOFF  exit, nowt to do
                                }
                                else
                                {
                                    CloseFeed();    // Close pinch valve in throat.
                                    if(g_CalibrationData.m_nOfflineType == OFFLINETYPE_HILO) //Offline type = hi/lo??
                                    {
                                     CalculateCMRVoltage();  // drive CMR relay, etc.
                                    }
                                    g_nDmpSec = 0;
                                    g_nOffline_Tracking = 0;    // OFFLINE_TRK = 0
                                    g_cBatSeq = BATSEQ_OFFDELBDMPID;        // "Delay before dump ID"
                                }
                            }
                            else  // i.e. not offline
                            {
                                //OpenDump(); //dump material.
                                g_lLoadTime = 0;
                                DumpMaterialFromWeighHopper();
//                                UpdateBatchVariables();  // ASM = UPDATEBATCHVARIABLES
                                CloseFeed();    // Close pinch valve in throat.
                                g_nDmpSec = 0;
                                g_nOffline_Tracking = 0;    // OFFLINE_TRK = 0
                                g_cBatSeq = BATSEQ_OFFDELBDMPID;        // "Delay before dump ID"
                            }
                        }
                        }
                    }
                }

           break;

      case BATSEQ_OFFDELBDMPID:
            if( g_nDmpSec > OFFDELAY1 )       // OFFLINE DELAY #1
            {
//                OpenDump();
                g_lLoadTime = 0;
                DumpMaterialFromWeighHopper();
                g_cBatSeq = BATSEQ_DUMP;                        // move on to DUMP.
                //JSR STARTOFFLINEDELAY (expanded inline)
                CloseFeed();    // Close pinch valve in throat.
                g_nOffline_Tracking = BATSEQ_OFFDELADMPID;  // OFFLINE_TRK = OFFDELADMPID
                g_nOffTime1 = 0;
                g_nDmpSec = 0;          // 1/10 sec counter.
                UpdateBatchVariables(); // update batch weights etc.  // ASM = UPDATEBATCHVARIABLES
            }
           break;


      case BATSEQ_DUMP:
                if(g_bAutoCycleFlag)
                {
                    DumpingWeightCheck();   //Wt check while dumping.
                }
                else // not autocycling
                {
                    if(g_bStopped)
                    {
                        OpenDump();
                    }
                    else
                    {  // CHECKWT
                        DumpingWeightCheck();
                    }
                }

           break;
      case BATSEQ_TAREID:
            unsigned int nTareTime;
            if(g_CalibrationData.m_bCycleFastFlag)
            {
                 nTareTime = TIME_BEFORE_TARE;
            }
            else
            {
                nTareTime = SUPER_FAST_TIME_BEFORE_TARE;
            }

            if(g_nDmpSec > nTareTime)  //TIME BEFORE TARE.
            {
                g_nDmpSec = 0;
                Tarend();       //Tare hopper.
                g_bTaringStatus = TRUE; // Indicate taring status.
                g_cBatSeq = BATSEQ_AFTERTAREID; // after tare period

            }
           break;
      case BATSEQ_MIXINGID:
                nTimeInTenths = g_CalibrationData.m_nIntermittentMixTime * 10;   // mixing time in 10ths of second
            if(g_nDmpSec > nTimeInTenths)  //at mixing time??
            {
                    g_bMixOff = TRUE;   // mixer off
               g_cBatSeq = BATSEQ_AFTERTAREID;  // after tare period
               g_nDmpSec = 0;
            }
           break;

      case BATSEQ_AFTERTAREID:
            if(g_CalibrationData.m_bCycleFastFlag)
            {
                nAfterTareTime = AFTER_TARE_TIME;
            }
            else
            {
                nAfterTareTime = SUPER_FAST_AFTER_TARE_TIME;
            }


            if(g_nDmpSec > nAfterTareTime)   //Finished after-tare time??
//            if((g_nDmpSec > nAfterTareTime) && (g_nTransferSetpointsFlag != TRANSFERSPFROMCOMMSID))  //Finished after-tare time?? nbb--todo-- review this
            {
               g_bTaringStatus = FALSE;
               g_nFillCounter = 0;
               g_bFillAlarm = FALSE;   //reset component fill alarm.
               ClearUnused();
               g_nFillCtr = 0;
               ResetFillCounters();
               StartFillSequence(); // JSR CHECKLLS (exit to fill mode)
               g_nAnyFillAlarm = 0;
               g_bLevelSensorAlarmOccurred = FALSE;
               g_nRetryCompNoHis = 0;     //Update history.(RETRYCOMPNOHIS )

//removingif(1) if( 1 )  // BRA NOVOL (bypasses condition: if(g_bFirstWCycle) )
//removingif(1) {
                  //NOVOL
                  if(!g_nPauseFlag) //not paused??
                  {
                     if(g_bTopUpFlag)
                        g_nTopUpSequencing = FILLFIRST;
                     g_cBatSeq = BATSEQ_FILL;   // Advance to fill mode
                     CloseDump();
                     g_nDmpSec = 0;

                     g_bCycleIndicate = TRUE;  //indicate cycle status.
                     ResetFillCounters();
                     CheckForCleanPhase2();
                  }
//removingif(1) }

            }
           break;


      case BATSEQ_NOFILL:
           break;
      case BATSEQ_MATERIALFALL:
            unsigned int nFallTime;
            if(g_CalibrationData.m_bCycleFastFlag)
            {
                nFallTime = MATERIAL_FALL_TIME;
            }
            else
            {
                nFallTime = SUPER_FAST_MATERIAL_FALL_TIME;
            }

            if(g_nDmpSec > nFallTime)  // Time for material to fall elapsed??
                {
                    if(g_nCleanCycleSeq == CLEANPHASE1ID)
                    {
                        StartPhase1Clean(); //ISPHASE1CLEAN Phase 1 of clean cycle.
                    }
                    else
                    {
                        CloseDump();
                        // Double dump req'd??
                        if((g_CalibrationData.m_bDoubleDump)&&(g_CalibrationData.m_bCycleFastFlag))

                            g_cBatSeq = BATSEQ_DOUBLEDUMPCLOSEID;
                        else
                            g_cBatSeq = BATSEQ_TAREID;      // no, Normal tare

                       g_nDmpSec = 0;
                    }
                }
         break;
        case BATSEQ_DOUBLEDUMPCLOSEID:
            if(g_nDmpSec > DDCLOSETIME)
            {
                OpenDump();
                g_cBatSeq = BATSEQ_DOUBLEDUMPOPENID;    // set to open phase of D.D.
                 g_nDmpSec = 0;
            }
            break;

        case BATSEQ_DOUBLEDUMPOPENID:
            if(g_nDmpSec > DDOPENTIME)
            {
                CloseDump();
                g_cBatSeq = BATSEQ_TAREID;  // set to Tare
                g_nDmpSec = 0;
            }
            break;

        case BATSEQ_VACLOADING:
            if(g_nDmpSec > VACDELAY)
            {
                g_nFillCounter = 0;
                if(!g_nPauseFlag)   //not paused??
                {
                    g_cBatSeq = BATSEQ_FILL;    // set to Fill
                    g_nDmpSec = 0;
                    CheckForCleanPhase2();

                }
            }
            break;

        case BATSEQ_WAITFORTRAN:        // Not being used in assembler.
        case BATSEQ_DELAYBFILL:     // Not being used
            g_nDmpSec = 0;
            g_cBatSeq = BATSEQ_FILL;    // set to Fill      (in case accidently arrive here)
            CloseDump();

            break;

        case BATSEQ_ACTIVATINGJET1ID:   //ASM = CHKCLC
            if(g_nAirJet1OnCounter == 0)
            {
                g_cBatSeq = BATSEQ_AIRJET1OFF1ID;   // set to AIRJET1OFF
                g_nDmpSec = 0;
            }
            break;

        case BATSEQ_AIRJET1OFF1ID:  //ASM = CHKFORJ1OFF
            if( g_nDmpSec > g_CalibrationData.m_nPhase1OnTime )
            {
                g_nBlenderCleanRepetionsCounter++;  // Incr. repetitions counter
                if(g_nBlenderCleanRepetionsCounter >= g_CalibrationData.m_nCleanRepetitions)
                {   //ASM = BCLFIN  (Cleaning finished)
                    g_nBlenderCleanRepetionsCounter = 0;
                    g_cBatSeq = BATSEQ_CLOSEDUMP1ID;    // set to CLOSEDUMP1
                    g_nDmpSec = 0;
                }
                else
                {
                    g_nAirJet1OnCounter = g_CalibrationData.m_nPhase1OnTime;    //set air jet #1 time
                    g_cBatSeq = BATSEQ_ACTIVATINGJET1ID;
                }
            }
            break;

      case BATSEQ_CLOSEDUMP1ID:  //ASM = CHKFRDM
            if(g_nDmpSec > CLOSEDUMP1TIMETENTHS)
            {
            CloseDump();
            g_cBatSeq = BATSEQ_OPENDUMP1ID; // set to OPENDUMP1
                g_nDmpSec = 0;
            }
            break;

      case BATSEQ_OPENDUMP1ID:  //ASM = CHKOD1
            if(g_nDmpSec > OPENDUMP1TIMETENTHS)
            {
            OpenDump();
            g_cBatSeq = BATSEQ_CLOSEDUMP2ID; // set to CLOSE DUMP #2
                g_nDmpSec = 0;
            }
            break;

      case BATSEQ_CLOSEDUMP2ID:  //ASM = CHKCD2
            if(g_nDmpSec > CLOSEDUMP1TIMETENTHS)
            {
            CloseDump();
                g_nDmpSec = 0;
                if(g_nDumpRepetitions)
                {
                    g_nDumpRepetitions--;
                g_cBatSeq = BATSEQ_OPENDUMP1ID; // set to OPENDUMP1
                }
                else
                {
                g_cBatSeq = BATSEQ_SETTLEAFTERBANGID; // set to SETTLINGAFTERBANG
                }
            }
            break;

      case BATSEQ_SETTLEAFTERBANGID:  //ASM = CHKOD2
            if(g_nDmpSec > SETTLEAFTERBANGTIMETENTHS)
            {
                g_bPhaseOneFinished = TRUE;
                g_nDmpSec = 0;
                if(g_CalibrationData.m_nBlenderMode == MODE_SINGLERECIPEMODE)
                {
                    if(g_bRecipeWaiting)
                    {
                      g_bRecipeWaiting = FALSE;
                      g_bBatchHasFilled = TRUE;
                      LoadPercentageTargetsFromComms();
                      TransferSetPercentagesFromComms();
                      g_cBatSeq = BATSEQ_TAREID; // set to TARE
                    }
                    else
                    {
                        g_bBatchHasFilled = FALSE;
                        g_bPhaseOneFinished = FALSE;
                        g_nCleanCycleSeq = CLEANPHASE2ID;
                        g_cBatSeq = BATSEQ_LEVPOLL; // Advance to level sensor polling.
                    }
                }
                else
                {
                    g_cBatSeq = BATSEQ_TAREID; // make the blender fill the next batch
                }
            }
            break;

      case BATSEQ_CCLACTIVEID:  //ASM = CHKCC
            if(g_nLeftCornerCleanOnCounter == 0)    //CCLONCTR
            {
            g_cBatSeq = BATSEQ_CCLOFFID; // set to Corner Clean off id
                g_nDmpSec = 0;
            }
            break;

      case BATSEQ_CCLOFFID:  //ASM = CHCCLOFF
            LeftCornerCleanOff();   // turn off left corner clean
            if(g_nDmpSec > g_CalibrationData. m_nPhase2OffTime )    //
            {
            g_cBatSeq = BATSEQ_CCRACTIVEID; // set to Right Corner Clean id
                g_nRightCornerCleanOnCounter = g_CalibrationData. m_nPhase2OnTime;
                g_nDmpSec = 0;
            }
            break;

      case BATSEQ_CCRACTIVEID:  //ASM = CHCRON
            if(g_nRightCornerCleanOnCounter == 0)   //CCRONCTR
            {
            g_cBatSeq = BATSEQ_CCROFFID; // set to Right Corner Clean OFF id
                g_nDmpSec = 0;
            }
            break;

      case BATSEQ_CCROFFID:  //ASM = CHCCROFF
            RightCornerCleanOff();  // turn off right corner clean
            if(g_nDmpSec > g_CalibrationData. m_nPhase2OffTime )    //
            {
                g_nMixerCleanRepetionsCounter++;
                if(g_nMixerCleanRepetionsCounter >= g_CalibrationData. m_nMixerCleanRepetitions)
                {   //asm = FINCC
                    g_nMixerCleanRepetionsCounter = 0;
                    g_nAirJet2OnCounter = g_CalibrationData.m_nPhase2OnTime;
                    g_cBatSeq = BATSEQ_ACTIVATINGJET2ID; // set to JET 2 id
                    g_nDmpSec = 0;
                }
            else
                {
                    g_cBatSeq = BATSEQ_CCLACTIVEID;     // repeat left corner clean.
                    g_nLeftCornerCleanOnCounter = g_CalibrationData.m_nPhase2OnTime;
                    g_nDmpSec = 0;
                }
//--review--
//              g_nRightCornerCleanOnCounter = g_CalibrationData. m_nPhase2OnTime;
            }
            break;

      case BATSEQ_ACTIVATINGJET2ID:  //ASM = CHCLP2   Activating jet 2??
            if(g_nAirJet2OnCounter == 0)    //AIRJET2ONCTR - expired??
            {
            g_cBatSeq = BATSEQ_AIRJET2OFFID; // set to Air jet 2 OFF id
                g_nDmpSec = 0;
            }
            break;

      case BATSEQ_AIRJET2OFFID:  //ASM = AJ2OFF
            if(g_nDmpSec > g_CalibrationData. m_nPhase2OffTime )    //
            {
                g_nMixerCleanRepetionsCounter++;

                if(g_nMixerCleanRepetionsCounter >= g_CalibrationData. m_nMixerCleanRepetitions)
                {   //asm = FINCLC
                    g_nMixerCleanRepetionsCounter = 0;
                    RightCornerCleanOff();  // turn off right corner clean
                    LeftCornerCleanOff();   // turn off left corner clean
                    g_nCleanCycleSeq = 0;       // Reset cleaning cycle.
                    SetEndCleanMBBit();         //

/* Clean bits taken care of by CopyBBAuxToMB()
*/
                    //i.e. clear clean stage 2 bit in MB
// --review-- should this be copied in CopyBBAuxToMb

                    nStatus = g_arrnMBTable[BATCH_SUMMARY_OPERATIONAL_STATUS];  //MBOPSTS
                    nMask = BATCH_STATUS_IN_CLEAN_2NDSTAGE ^ 0xFFFF;    // COM of BATCH_STATUS_IN_CLEAN_2NDSTAGE(invert bits by excl. or)
                    g_arrnMBTable[BATCH_SUMMARY_OPERATIONAL_STATUS] = nStatus & nMask;

                    if(g_CalibrationData.m_nBlenderMode == MODE_SINGLERECIPEMODE)
                    {
                        g_bOfflineValveIsOpen = FALSE;
                        if(g_bBatchHasFilled)
                        {
                            g_bWaitForVacuumToFinish = TRUE;    // wait for vacuum to finish.
                        }
                             VacInhibitOn();      // ASM VACINHIBITON  sets vacuum inhibit in pause flag
                    }

                    if(g_bNoFillOnClean)    //No Fill on clean??
                    {
                        g_bNoFillOnClean = FALSE;
                        PauseOn();
                    }
                    g_cBatSeq = BATSEQ_LEVPOLL; // set to poll level sensor.
                    g_nDmpSec = 0;
                }
            else
                {
                    g_nAirJet2OnCounter = g_CalibrationData.m_nPhase2OnTime;
                    RightCornerCleanOff();  // turn off right corner clean
                    LeftCornerCleanOff();   // turn off left corner clean
                    g_cBatSeq = BATSEQ_ACTIVATINGJET2ID;        // repeat left corner clean.
                    g_nDmpSec = 0;
                }
            }
            break;


      default:   //
//          g_nDmpSec = 0;      //testonly
            bFinished = TRUE;           //NB This seems to be necessary - not sure why, suspect compiler problem.(08.09.2004)
         break;
    } // end of switch statement.

 } // end of if for bFinished.

}

/*
    g_nBlenderCleanRepetionsCounter = 0;
    g_nMixerCleanRepetionsCounter = 0;

NOTDMPS CMPA    #MATERIALFALL   ; IS MATERIAL STILL FALLING ?
        BNE     NOTMATF         ;  NO
        LDD     DMPSEC          ; READ SECONDS COUNTER
        CPD    #MATFALLTIME    ; TIME BEFORE TARE.
        BLS     NOTALLFALL      ; YES
        LDAA    CLEANCYCLESEQ   ; IS THIS A CLEAN CYCLE
        CMPA    #CLEANPHASE1ID  ; IS THIS PHASE 1
        LBEQ     ISPHASE1CLEAN   ; IS PHASE 1 CLEAN
        JSR     CLOSEDUMP       ; CLOSE FLAP
        TST     DOUBLEDUMP      ; IS DOUBLE DUMP REQUIRED
        BEQ     TARASN          ; TARE AS NORMAL
        LDAA    #DOUBLEDUMPCLOSEID
        BRA     STRACD          ; STORE AND CLEAR COUNTER

TARASN  LDAA    #TAREID         ;
STRACD  STAA    BATSEQ          ; TAREING IS NEXT IN THE SEQUENCE
        CLRW    DMPSEC          ; RESET DUMP SECONDS COUNTER.
NOTALLFALL:
        RTS

*/

//////////////////////////////////////////////////////
// NoSensorOnline( void )           from ASM = ONLINE (in CYCMON)
// Routine to caters for checking Offline tracking flag, and ONLINE part of OFFLINEMODE.
//
//
// M.McKiernan                          07-09-2004
// First pass.
//////////////////////////////////////////////////////
void NoSensorOnline( void )
{
  // ASM = ONLINE
  if(g_cBatSeq == BATSEQ_OFFDELBDMPID)    // "Delay before dump ID"
  {
      if( (g_nDmpSec > OFFDELAY1) || (g_CalibrationData.m_nBlenderMode == MODE_OFFLINEMODE)  )       // OFFLINE DELAY #1
      {
         //OpenDump();
         DumpMaterialFromWeighHopper();
         g_cBatSeq = BATSEQ_DUMP;                        // move on to DUMP.
      //JSR STARTOFFLINEDELAY (expanded inline)
            CloseFeed();    // Close pinch valve in throat.
         g_nOffline_Tracking = BATSEQ_OFFDELADMPID;   // OFFLINE_TRK = OFFDELADMPID
         g_nOffTime1 = 0;
         g_nDmpSec = 0;       // 1/10 sec counter.
         UpdateBatchVariables();  // ASM = UPDATEBATCHVARIABLES
      }
  }
  else //NEXTIDCHK
  {
      if(g_nOffline_Tracking == BATSEQ_OFFDELADMPID)
      {
         if(g_nOffTime1 >= g_CalibrationData.m_nMixingTime)
         {
            CheckForVacuumSequence(); // ASM CHECKFORVACSEQ
            g_nCheckLSensorInNSeconds = 10;
            if(g_bBatchReady)
            {
               g_bBatchReady = FALSE;
               StartUpOn();
            }
            g_nOffline_Tracking = 0;   // reset tracking flag.
         }
      }
  }

}

//////////////////////////////////////////////////////
// MakeCycle( void )            from ASM = MAKECYC
// Routine to start a batch cycle.
//
//
// M.McKiernan                          07-07-2004
// First pass.
//////////////////////////////////////////////////////
void MakeCycle( void )
{
    g_cBatSeq = BATSEQ_LEVPOLL;
    CloseAll();
    g_lComponentOnTimer = 0;
    ToManual();
    g_nSeqTableIndex = 0;   // reset to start of table

}
//////////////////////////////////////////////////////
// StartPhase1Clean( void )         from ASM = ISPHASE1CLEAN (in CYCMON)
// Routine to start phase 1 cleaning
//
//
// M.McKiernan                          28-07-2004
// First pass.
//////////////////////////////////////////////////////
void StartPhase1Clean( void )
{


    if(g_CalibrationData.m_nCleaning == MIXERCLEAN)
    {
        g_cBatSeq = BATSEQ_CLOSEDUMP1ID;    // set to CLOSEDUMP1
        g_nDmpSec = 0;
    }

// NORCLN
    else
    {
        g_nBlenderCleanRepetionsCounter = 0;
        g_nMixerCleanRepetionsCounter = 0;
        g_nDumpRepetitions = 1;
        g_bPhaseOneFinished = FALSE;
        g_nAirJet1OnCounter = g_CalibrationData.m_nPhase1OnTime;    //set air jet #1 time
        g_cBatSeq = BATSEQ_ACTIVATINGJET1ID;
}   }

/*

mce12-15

;  CLEANING PHASE 1
;
ISPHASE1CLEAN:
        TST     MOULDING                ; MOULDING MODE
        BEQ     NORCLN                  ; NO NORMAL CLEAN

        LDAA    #CLOSEDUMP1ID           ;
        STAA    BATSEQ                  ; TAREING IS NEXT IN THE SEQUENCE
        CLRW    DMPSEC                  ; READ 1/10 SECONDS COUNTER.
        BRA     STANXI                  ; EXIT.

NORCLN  CLR    BCLEANREPETITIONSCTR    ; COUNTER INITIATED
        CLR     MCLEANREPETITIONSCTR    ; COUNTER INITIATED
        LDAA   #1
        STAA    DUMPREPETITIONS
        CLRA
        LDAB    PH1ONTIME
        JSR     BCDHEX1
        STD     AIRJET1ONCTR            ; AIR JET 1 COUNTER ONE TIME
        LDAA    #ACTIVATINGJET1ID
        STAA    BATSEQ                  ; STORE SEQUENCING.
STANXI  RTS

mce09-5

ISPHASE1CLEAN:
        CLR    BCLEANREPETITIONSCTR    ; COUNTER INITIATED
        CLR     MCLEANREPETITIONSCTR    ; COUNTER INITIATED
        LDAA   #1
        STAA    DUMPREPETITIONS
        CLRA
        LDAB    PH1ONTIME
        JSR     BCDHEX1
        STD     AIRJET1ONCTR            ; AIR JET 1 COUNTER ONE TIME
        LDAA    #ACTIVATINGJET1ID
        STAA    BATSEQ                  ; STORE SEQUENCING.
STANXI  RTS

*/
//////////////////////////////////////////////////////
// DumpingWeightCheck( void )           from ASM = CHECKWT in dump check section of CYCMON.
// Wt check during dumping.
//
//
// M.McKiernan                          27-07-2004
// First pass.
//////////////////////////////////////////////////////
void DumpingWeightCheck( void )
{
long lOneSecHopWt;
float fMinWeight;

   CalculateHopperWeight();   // cal hopper wt from 1 sec average
    lOneSecHopWt = (long)(g_fOneSecondHopperWeight * 1000); //wt. in g - testonly
    fMinWeight = GetMinimumHopperWeight(g_CalibrationData.m_nBlenderType);
   if(g_fOneSecondHopperWeight > (fMinWeight / 1000.0))  // MINWEIGHT is in grammes. ( hopper wt in kg)
   {    //CONDMP (Continue dumping)
      OpenDump();
      if(g_nDmpSec > MAX_TARE_TIME )
      {
         g_nDmpSec = 0;
         if( (g_arrnMBTable[BATCH_SUMMARY_PROCESS_ALARMS] & TAREALBIT) == 0 ) // tare alarm bit not set???
         {  //
            g_sOngoingHistoryEventsLogData.m_nTareAlarms++;
            g_s24HrOngoingHistoryEventsLogData.m_nTareAlarms++;
            PutAlarmTable( TAREALARM,  0 );     // indicate alarm, Component no. is zero. //PTALTB
         }

        if(g_bDumpFlapHasOpened)
        {
            if(g_bLevSen1Stat == UNCOVERED)    // if level sensor uncovered proceed.
            {
                g_cBatSeq = BATSEQ_MATERIALFALL;
//                iprintf("\nmaterial fall"); //nbb--testonly--
                g_nDmpSec = 0;
                NeedForTopUp();   // check if top up req'd.
            }
         }
      }
   }
   else  // i.e. <= minweight
   {
       if( g_arrnMBTable[BATCH_SUMMARY_PROCESS_ALARMS] & TAREALBIT ) /* percentage alarm bit set???  */
       {
           RemoveAlarmTable( TAREALARM,  0 );       /* clear % alarm., Component no. is zero. */
       }
       g_cBatSeq = BATSEQ_MATERIALFALL;
       g_nDmpSec = 0;
       NeedForTopUp();   // check if top up req'd.
   }
}


//////////////////////////////////////////////////////
// WeighHopperHasOpened( void )
// Determine if the weigh hopper has opened
// if the weight change is greater than 100 grams it is assumed that
// the weigh hopper dump flap has opened
//
//////////////////////////////////////////////////////
BOOL WeighHopperHasOpened( void )  // nbb--todolp--is this necessary ?
{
    BOOL bFlapHasOpened = TRUE;
    if(g_fOneSecondHopperWeight > g_fCurrentBatchWeight)
    {
        if((g_fOneSecondHopperWeight - g_fCurrentBatchWeight) > 0.100)  // greater - flap has opened
        {
            bFlapHasOpened = TRUE;
        }
        else
        {
            bFlapHasOpened = FALSE;
        }
    }
    else
    {
        if((g_fCurrentBatchWeight - g_fOneSecondHopperWeight) > 0.100) // greater - flap has opened
        {
            bFlapHasOpened = TRUE;
        }
        else
        {
            bFlapHasOpened = FALSE;
        }
    }
    return(bFlapHasOpened);
}

//////////////////////////////////////////////////////
// ClearUnused( void )          from ASM = CLEARUNUSED
// Clear data areas for unused components (components not currently in Sequence table (g_cSeqTable[])
//
//
// M.McKiernan                          29-07-2004
// First pass.
//////////////////////////////////////////////////////
void ClearUnused( void )
{
int i, nCompIndex;


    for(i=0; i < MAX_COMPONENTS; i++ )
        g_bComponentInSeqTable[i] = FALSE;  //initialise to false.

    // check if component in SeqTable.
    i=0;
    while(g_cSeqTable[i] != SEQDELIMITER && i < MAX_COMPONENTS) // SEQDELIMITER indicates table end
    {
        nCompIndex = (int)(g_cSeqTable[i] & 0x0F) - 1;  // get component no. from table, mask off the m.s. nibble
        g_bComponentInSeqTable[nCompIndex] = TRUE;
        i++;
    }

    // TOPUP MODE & first comp in table = 8??
    if(g_bTopUpFlag)
    {
        g_bComponentInSeqTable[TOPUPCOMPONENT - 1] = TRUE;  // NB not strictly necessary as while loop should handle (if MAX_COMPONENTS >= 8)
    }
    if(g_CalibrationData.m_bLiquidEnabled)
    {
        g_bComponentInSeqTable[g_nLiquidAdditiveComponent-1] = TRUE;  // NB not strictly necessary as while loop should handle (if MAX_COMPONENTS >= 8)
    }
    // now clear data areas for components not in table.
    for(i=0; i < MAX_COMPONENTS; i++ )
    {
        if(g_bComponentInSeqTable[i] == FALSE) // i.e. not in table
        {
            g_fComponentActualWeight[i] = 0;    //actual weight.
            g_fComponentActualWeightI[i] = 0;   //actual weight. (imperial units)
            g_fComponentActualWeightRR[i] = 0;  //actual weight. (round robin
            g_fComponentActualWeightRRI[i] = 0; //actual weight. (RR & imperial units)

            g_fComponentTargetWeight[i] = 0;        //target wt
            g_fComponentTargetWeightI[i] = 0;   //target wt, imperial units.
            g_lComponentTargetCounts[i] = 0;      // CMP1CNTG sbb--todo--
            g_lComponentCountsActual[i] = 0;      // CMP1CNAC sbb--todo--
            g_lCmpTime[i] = 0;                    // CMP1TIMS sbb--todo--

        }
    }

}


//////////////////////////////////////////////////////
// DoPhase2Clean( void )            from ASM = DOPHASE2CLEAN
// Phase 2 clean, ignore level sensor.
// Check for bypass sensor off high level sensor
// Activate air jets 2
// Set batseq to LEVPOLL

//
// M.McKiernan                          09-09-2004
// First pass.
//////////////////////////////////////////////////////
void DoPhase2Clean( void )
{

        if(((g_CalibrationData.m_nBlenderMode == MODE_SINGLERECIPEMODE)&&(g_bOfflineValveIsOpen))||(g_CalibrationData.m_nBlenderMode != MODE_SINGLERECIPEMODE))
        {
            OpenFeed();
            if(g_bHighLevelSensor == UNCOVERED)     // HIGHLEVELSENSOR.
            {
        //i.e. set clean stage 2 bit in MB
//--REVIEW-- should this be handled by CopyBBAuxToMB

                g_arrnMBTable[BATCH_SUMMARY_OPERATIONAL_STATUS_2] |= BATCH_STATUS_2_CLEAN_PHASE_2_MASK; //MBOPSTS

                if((g_CalibrationData.m_nBlenderType == TSM50) || (g_CalibrationData.m_nCleaning == MIXERCLEAN))
                { //asm = FINCC   (skip corner cleaning and go to mixer clean)
                    g_nMixerCleanRepetionsCounter = 0;
                    g_nAirJet2OnCounter = g_CalibrationData.m_nPhase2OnTime;
                    g_cBatSeq = BATSEQ_ACTIVATINGJET2ID; // set to JET 2 id
                    g_nDmpSec = 0;
                }
                else  // i.e. not baby
                {
                    g_cBatSeq = BATSEQ_CCLACTIVEID;     // repeat left corner clean.
                    g_nLeftCornerCleanOnCounter = g_CalibrationData.m_nPhase2OnTime;
                    g_nDmpSec = 0;
                }
            }
        }
}



//////////////////////////////////////////////////////
// StartCleanCycle( void )          from ASM = STARTCLEANCYCLE
// Starts clean cycle - sets clean cycle seq for phase 1.
//
//
// M.McKiernan                          09-09-2004
// First pass.
//////////////////////////////////////////////////////
void StartCleanCycle( void )
{
    g_nCleanCycleSeq = CLEANPHASE1ID;
    g_nSuspendControlCounter = SUSPENDCONTROLNO; // suspend control for n cycles.
    g_bCleaningInitiatedEvent = TRUE;
    g_nSDCardDelayTime = SDCARDCOMMANDDELAYTIME; // delay event to allow comms to finish
}

//////////////////////////////////////////////////////
// InitiateCleanCycle( void )           from ASM = INITIATECLEANCYCLE
// Initiates clean cycle -
//
//
// M.McKiernan                          09-09-2004
// First pass.
//////////////////////////////////////////////////////
void InitiateCleanCycle( void )
{
    if(g_CalibrationData.m_nCleaning != 0)
    {
        g_bOverrideClean = FALSE;
        ResetCleanFinishMBBit();
        SetStartCleanMBBit();
        if( g_bCycleIndicate )
        {
            g_bSetCleanWhenCycleEnd = TRUE;
        }
        else
        {
            StartCleanCycle();
        }
    }
}

/*
//////////////////////////////////////////////////////
// ResetCleanFinishMBBit( void )            from ASM = RESETCLEANFINISHBIT
// Resets clean finish bit in operational status word in Modbus.
//
//
// P.Smith                          5/10/05
//////////////////////////////////////////////////////
*/

void ResetCleanFinishMBBit( void )
{
unsigned int nStatus,nMask;
      //i.e. Clear clean finished bit in MB
      nStatus = g_arrnMBTable[BATCH_SUMMARY_OPERATIONAL_STATUS];  //MBOPSTS
      nMask = BATCH_STATUS_FINISHED_CLEAN_MODE ^ 0xFFFF; // COM of BATCH_STATUS_FINISHED_CLEAN_MODE(invert bits by excl. or)
      g_arrnMBTable[BATCH_SUMMARY_OPERATIONAL_STATUS] = nStatus & nMask;
}

/*
RESETCLEANFINISHBIT:
        LDD     #MBSTATCLEANENDEDBPOS
        COMD
        ANDD    MBOPSTS
        STD     MBOPSTS
        RTS
  */


/*
//////////////////////////////////////////////////////
// ResetCleanStartMBBit( void )            from ASM = RESETCLEANBITS
// Resets clean start bit in operational status word in Modbus.
//
//
// P.Smith                          5/10/05
// First pass.
//////////////////////////////////////////////////////
*/

void ResetCleanStartMBBit( void )
{
unsigned int nStatus,nMask;
      //i.e. Clear clean finished bit in MB
      nStatus = g_arrnMBTable[BATCH_SUMMARY_OPERATIONAL_STATUS];  /*MBOPSTS */
      nMask = BATCH_STATUS_FINISHED_CLEAN_MODE ^ 0xFFFF; /*   complement */
      g_arrnMBTable[BATCH_SUMMARY_OPERATIONAL_STATUS] = nStatus & nMask;
}

/*

RESETCLEANBITS:
        LDD     #MBSTATCLEANSTARTEDBPOS
        COMD
        ANDD    MBOPSTS
        STD     MBOPSTS

*/




//////////////////////////////////////////////////////
// SetStartCleanMBBit( void )           from ASM = SETSTARTCLEANMBBIT
// Resets clean finish bit in operational status word in Modbus.  //MBOPSTS
//
//
// M.McKiernan                          09-09-2004
// First pass.
//////////////////////////////////////////////////////
void SetStartCleanMBBit( void )
{
      //i.e. Set start clean bit in MB
      g_arrnMBTable[BATCH_SUMMARY_OPERATIONAL_STATUS] |= BATCH_STATUS_IN_CLEAN_MODE;
}
/*
SETSTARTCLEANMBBIT:
        LDD     #MBSTATCLEANSTARTEDBPOS
        ORD     MBOPSTS
        STD     MBOPSTS
        RTS
*/


//////////////////////////////////////////////////////
// SetEndCleanMBBit( void )           from ASM = SETENDCLEANMBBIT
// Sets clean finish bit in operational status word in Modbus.  //MBOPSTS
//
//
// P.McKiernan                          31-03-2005
// First pass.
//////////////////////////////////////////////////////
void SetEndCleanMBBit ( void )
{
      //i.e. Set end clean bit in MB
      g_arrnMBTable[BATCH_SUMMARY_OPERATIONAL_STATUS] |= BATCH_STATUS_FINISHED_CLEAN_MODE;
      g_nHoldOffLevelSensorAlarmSecondCounter = HOLDOFFLSENSORALINSECONDS;
      g_nDisableDTAPerKgCtr = NOOFCYCLES_TODISABLEDAPERKG_DURINGCLEAN;
}

/*
*****************************************************************************;
; FUNCTION NAME : SETSTARTCLEANMBBIT                                          ;
; FUNCTION      : RESET CLEAN START                                           ;
; INPUTS        : NONE                                                        ;                       ;
;*****************************************************************************;


SETENDCLEANMBBIT:
        LDD     #MBSTATCLEANENDEDBPOS
        ORD     MBOPSTS
        STD     MBOPSTS
        RTS

*/

//////////////////////////////////////////////////////
// SetMixerTime( void )         from ASM = SETMIXERTIME
// Sets mixer time depending on blender type.
//
//
// M.McKiernan                          09-09-2004
// First pass.
//////////////////////////////////////////////////////
void SetMixerTime( void )
{
        g_nMixerOnCounter10Hz = g_CalibrationData.m_nIntermittentMixTime * 10; // mixing time entered in seconds
}


//////////////////////////////////////////////////////
// CheckForCleanPhase2( void )          from ASM = CHKFORCLEANPHASE2
// check for clean phase 2 requirement
//
//
// M.McKiernan                          09-09-2004
// First pass.
//////////////////////////////////////////////////////
void CheckForCleanPhase2( void )
{
    if(g_bPhaseOneFinished)
    {
        g_bPhaseOneFinished = FALSE;
        g_nCleanCycleSeq = CLEANPHASE2ID;
    }
}


//////////////////////////////////////////////////////
// CheckHopperWeight( void )            from ASM = CHECKHOPWT
// Check for change in hopper weight when level sensor uncovered.
// I.e. check for material leaking from weigh hopper.
//
//
// M.McKiernan                          10-09-2004
// First pass.
//////////////////////////////////////////////////////
void CheckHopperWeight( void )
{
  if((g_bNoLeakAlarmCheck)||(g_nAfterEndOfCycleCounter != 0))
    {
        g_bNoLeakAlarmCheck = FALSE;
    }
    else
    {
        //if(fdTelnet >0)
        //printf("\n g_fOneSecondHopperWeight %3.4f g_fOneSecondHopperWeightAtEndOfCycle %3.4f",g_fOneSecondHopperWeight,g_fOneSecondHopperWeightAtEndOfCycle);
        if(((g_fOneSecondHopperWeight - g_fOneSecondHopperWeightAtEndOfCycle) > MINIMUM_WEIGHT_CHANGE) || ((g_fOneSecondHopperWeightAtEndOfCycle - g_fOneSecondHopperWeight) > MINIMUM_WEIGHT_CHANGE))
        {  //asm = WTCHANGE
            g_bDoorOpen = TRUE; // Set error flag
            if(g_bStopped)
            {
            ToManual();     // switch to manual
            }
            else
            {
                // Put Leak alarm if not already there.
                if((g_arrnMBTable[BATCH_SUMMARY_PROCESS_ALARMS] & WTCHANGEALBIT) == 0)
                {
                    PutAlarmTable( LEAKALARM,  0 );     // indicate alarm, Component no. is zero. //PTALTB
                    g_s24HrOngoingHistoryEventsLogData.m_nLeakAlarmsCtr++;
                    if(g_bManualAutoFlag == AUTO)
                    {
                        AddBlenderToManualLeakAlarmEventToLog();
                    }
                }
                ToManual();     // switch to manual
            }

        }
        else
        {   // i.e. no wt. change
            // Remove leak alarm if already set.
            if(g_arrnMBTable[BATCH_SUMMARY_PROCESS_ALARMS] & WTCHANGEALBIT)
                RemoveAlarmTable( LEAKALARM,  0 );      // indicate leakage alarm cleared, Component no = 0 i.e. not component alarm.
        }
    }
}






//////////////////////////////////////////////////////
// UpdateBatchVariables( void )          from ASM = UPDATEBATCHVARIABLES
// updates batch variables
//
//////////////////////////////////////////////////////
void UpdateBatchVariables( void )
{
    BatchSecondsCalculation();      // BATSECAL
    if( g_nEstCtrUse > 0 )  // use estimate flag set??
    {
        g_nEstCtrUse--;
        g_bRunEst = TRUE;
        if(g_bFirstBatch)
        {
            BatchLengthCalculation();   // ASM = BATCHLENGTHCAL
        }
        else
        {
            BatchLengthCalculation();   // Calculate length ASM = BATCHLENGTHCAL
            BatchGPMCalc(); //Calculate g/m  ASM = BATCHGPMCALC
        }

    }
    else //NORCALC (g_nEstCtrUse = 0)
    {
// --review-- no kg/h reset required here        CheckForRoundRobinReset();  // Check for round robin reset.
        CalculateKgPerHour();  // CALKGPHR
        g_nCycleTimePrn = g_lLoadTime;
        g_lLoadTime = 0;
        if(g_bFirstBatch)
        {
            BatchLengthCalculation();   // ASM = BATCHLENGTHCAL
        }
        else
        {
            BatchLengthCalculation();   // Calculate length ASM = BATCHLENGTHCAL
            BatchGPMCalc(); //Calculate g/m  ASM = BATCHGPMCALC
        }

        if(!g_bDisableControlAction)  //disable control action flag not set??
        {
            CalculateDAPerKg(); //CALCDACPKG
        }
    }
    g_fThroughputCalculationBatchWeight = g_fPreviousBatchWeight; // take a copy of weight reading used in calculation
    //NOCALC
        g_fPreviousBatchWeight = g_fCurrentBatchWeight; // current wt to prev. batch wt.

    AccumulateWeights(); //ACUMWT    - Accumulate roll and order wt.
    CopyWeightsToMB();      // copy to Modbus table.
    if(g_bFirstBatch)
    {   //NOALGCALL
        g_bFirstBatch = FALSE;
        CloseFeed();    // Close pinch valve in throat.
        g_nDmpSec = 0;
        g_nOffline_Tracking = 0;    // OFFLINE_TRK = 0
        g_cBatSeq = BATSEQ_OFFDELBDMPID;        // "Delay before dump ID"
    }
    else  // not 1st batch.
    {
        if(!g_bDisableControlAction )   // control action not disabled??
        {
            ExtruderControlAlgotithm();  //asm ALGOR1 control algorithm
//--todolp--                          CalculateActualMicrons();   //CALC_ACTMIC - Actual microns calculated.
        }
        else    // Disable control action flag set
        {
            g_bDisableControlAction  = FALSE;   // clear the disable.
        }
    }
//CONCYCLE:
    g_nCycleCounter++;
}



//////////////////////////////////////////////////////
// CheckForSingleRecipe( void )          from ASM = CHECKFORSINGLERECIPE:
// checks if cleaning should be initiated, if single recipe it should be in progress.
// if it is not start the cleaning process.
//
// P.Smith                          11-08-2005
//////////////////////////////////////////////////////
void CheckForSingleRecipeClean( void )
{

    if(g_CalibrationData.m_nBlenderMode == MODE_SINGLERECIPEMODE)  // single recipe mode ?
    {
        g_bAllowMultiBlendCycle = TRUE;   //nbb--todolp--kwh
        if(g_nCleanCycleSeq == 0) // no cleaning in progress.
        {
//            SetupMttty();
//            iprintf("\n starting clean");
            InitiateCleanCycle();
        }
        else
        {       // ISCLN
            if(g_bCleanOnNextCycle)
            {
                g_bCleanOnNextCycle = FALSE;
                InitiateCleanCycle();
            }
        }

    }
}



//////////////////////////////////////////////////////
// void DumpMaterialFromWeighHopper( void )
//
//////////////////////////////////////////////////////
void DumpMaterialFromWeighHopper( void )
{
    if(g_nCheckIfWeighHopperOpenedSecCtr == 0)
    {
    g_nCheckIfWeighHopperOpenedSecCtr = TIMETOWEIGHHOPPEROPENCHECK;
    g_fWeightAtOpenDumpFlap = g_fOneSecondHopperWeight;
    }
    OpenDump();    // open dump flap and release material

}


//////////////////////////////////////////////////////
// void CheckIfMaterialHasDumped( void )
//
//////////////////////////////////////////////////////
void CheckIfMaterialHasDumped( void )
{
    float fWeightChange,fWeightChangeExpected;
    if(g_nCheckIfWeighHopperOpenedSecCtr != 0)
    {
        g_nCheckIfWeighHopperOpenedSecCtr--;
        if((g_nCheckIfWeighHopperOpenedSecCtr == 0) && (g_fWeightAtOpenDumpFlap > 0.050) && !g_bCycleIndicate)
        {
            if(g_fWeightAtOpenDumpFlap > g_fOneSecondHopperWeight)
            {
                fWeightChange = g_fWeightAtOpenDumpFlap - g_fOneSecondHopperWeight;
            }
            else
            {
                fWeightChange = g_fOneSecondHopperWeight - g_fWeightAtOpenDumpFlap;
            }
            fWeightChangeExpected = (EMPTYBATCHPERCENTAGE/100.0f) * g_fWeightAtOpenDumpFlap;

            if(fWeightChange < fWeightChangeExpected)
            {
                // generate dump alarm
                PutAlarmTable( DUMPFLAPHASNOTOPENED,  0 );
                g_s24HrOngoingHistoryEventsLogData.m_nDumpFlapNotOpenAlarmsCtr++;
                g_bDumpFlapHasOpened = FALSE;
            }
            else
            {
                RemoveAlarmTable( DUMPFLAPHASNOTOPENED,  0 );
                g_bDumpFlapHasOpened = TRUE;
            }
        }
    }
}



/////////////////////////////////////////////////////
// GetMinimumHopperWeight()
//
//////////////////////////////////////////////////////
float GetMinimumHopperWeight(unsigned int nBlenderType)

{
    float   fMinweight;
        switch( g_CalibrationData.m_nBlenderType )
        {
        case TSM3000:
            fMinweight = MINWEIGHTTSM3000;
            break;

        default:
            fMinweight = MINWEIGHT;
            break;

        }  // end of switch statement.

        return(fMinweight);
}



/*

#define TIME_SECOND         0
#define TIME_MINUTE         1
#define TIME_HOUR           2
#define TIME_DATE           3
#define TIME_MONTH          4
#define TIME_YEAR           5
#define TIME_DAY_OF_WEEK    6

;*****************************************************************************;
; FUNCTION NAME : SETLEVELSENSORWDOG                                          ;
; FUNCTION      : SETS WATCHDOG COUNTER (BATCHSECS * MULTIPLIER)/10                                      ;
; INPUTS        : NONE                                                        ;                       ;
;*****************************************************************************;



SETLEVELSENSORWDOG:
        TST     LEVELMONITOR
        BEQ     XITLSW                       ; EXIT LEVEL SENSOR WATCH DOG
        TST     IGNORELSTIME
        BEQ     CHTIME                  ; CHECK TIME
        DEC     IGNORELSTIME
        BRA     XITLSW
CHTIME  LDAA     MBPROALRM               ; CHECK EXISTING ALARM.
        ANDA     #LSMONITORALARMBIT           ;
        BEQ      LSALRDC                 ; ALARM ALREADY SET.
        LDAA    #LSMONITORALARM
        JSR     RMALTB
LSALRDC LDD     BATCHSECS
        JSR     BCDHEX2
        PSHM    D
        CLRA
        LDAB    MONITORFACTOR
        JSR     BCDHEX2
        TDE
        PULM    D
        EMUL
        LDX     #TEN
        EDIV
        STX     BATCHMONCTR                  ; SET LEVEL SENSOR W DOG .
XITLSW  RTS

*/




/*

CHECKHOPWT:
        LDX     #WEIGHTINHOP    ; WEIGHT AFTER WEIGHING
        JSR     AMOVE           ;

        LDX     #CURBATWGH      ; WEIGHT WHEN LEVEL SENSOE UNCOVERED.
        JSR     CMOVE
        JSR     SUB             ; A-C
        TST     MATHDREG            ; IS ANSWER NEGATIVE ?
        BEQ     NOBOB           ; NO

        LDX     #WEIGHTINHOP    ; WEIGHT AFTER WEIGHING
        JSR     CMOVE           ;
        LDX     #CURBATWGH      ; WEIGHT WHEN LEVEL SENSOE UNCOVERED.
        JSR     AMOVE
NOBOB   TST     EREG            ; CHECK RESULT.
        BNE     WTCHANGE
        TSTW    EREG+1
        BNE     WTCHANGE
        LDD     EREG+3          ; CHECK XXX.Y   GRAM DIFFERENCE.
        CPD     #MINWTCHG        ; GRAMS OF ALLOWED WEIGHT CHANGE
        BHI     WTCHANGE
        LDAA    MBPROALRM       ;
        ANDA    #WTCHANGEALBIT
        BEQ     NOALCH          ; IS ALREADY WEIGHT CHANGE.
        LDAA    #LEAKALARM
        JSR     RMALTB          ; TO ALARM TABLE
NOALCH  RTS
WTCHANGE:
        LDAA    #$0F
        STAA    DOOROPEN        ; SET ERROR FLAG.
        TST     STOPPED         ; STOPPED ?
        BNE     ISALWC
        LDAA    MBPROALRM       ;
        ANDA    #WTCHANGEALBIT
        BNE     ISALWC          ; IS ALREADY WEIGHT CHANGE.
        LDAA    #LEAKALARM
        JSR     PTALTB          ; TO ALARM TABLE
ISALWC  JSR     TOMANUAL
        RTS



*/
/*
; CHECK FOR CLEAN PHASE 2 REQUIREMENT.

CHKFORCLEANPHASE2:
        LDAA    PHASEONEFINISHED ; HAS CLEAN CYCLE #1 FINISHED ?
        BEQ     XITCC            ; YES, IT HAS FINISHED
        CLR     PHASEONEFINISHED ; RESET
        LDAA    #CLEANPHASE2ID  ; CLEAN PHASE 2 CYCLE.
        STAA     CLEANCYCLESEQ

XITCC   RTS

*/

/*
SETMIXERTIME:
        RTS
        LDAA    BLENDERTYPE
        CMPA    #BABYTYPE
        BNE     NOTBBT          ; NO
        LDAB    MIXTIM          ; MIXING TIME BCD
        JSR     BCDHEX1         ; TO HEX
        LDAA    #10
        MUL                     ; MULTIPLY.
        STD     MIXERONCTR10HZ  ; COMPARE TO 10HZ COUNTER.
NOTBBT  RTS


*/

/*
DOPHASE2CLEAN:

; PHASE 2 CLEAN, IGNORE LEVEL SENSOR
; CHECK FOR BYPASS SENSOR OFF HIGH LEVEL SENSOR
; ACTIVATE AIR JETS 2
; SET BATSEQ TO LEVPOLL
;
        JSR     OPENFEED         ; OPEN FEED VALVE
        TST      HIGHLEVELSENSOR ; LEVEL SENSOR 1 STATUS
        BNE      XBYPONN                 ; BYPASS SENSOR IS ON
        LDD     #MBCSTATCLEANST2POS
        ORD     MBOPSTS
        STD     MBOPSTS

        LDD     #AIRJET1ONTIMEPH210S       ; AIR JET 1 ON IN 1/10 OF SECOND


; START PAHSE 2
PARAICSTOPP:

        LDAA    BLENDERTYPE             ;
        CMPA    #BABYTYPE               ; BABY TYPE BLENDER
        LBEQ     FINCC                  ; SKIP CORNER CLEANING AND GO TO MIXER CLEAN

        LDAA    #CCLACTIVEID
        STAA    BATSEQ                  ;; INITIATE CORNER CLEAN
        CLRA
        LDAB    PH2ONTIME
        JSR     BCDHEX1
        STD     CCLONCTR                 ; AIR JET 1 COUNTER ONE TIME
        CLR     DMPSEC


;        LDAB    PH2ONTIME
;        JSR     BCDHEX1
;        STD     AIRJET2ONCTR            ; AIR JET 1 COUNTER ONE TIME
;        JSR     CORNERCLEANOFF
;        LDAA    #ACTIVATINGJET2ID
;        STAA    BATSEQ                  ; STORE SEQUENCING.
XBYPONN RTS



*/
/*
CLEARUNUSED:
        LDX     #CMP1ACTWGT     ; AREA TO BE CLEARED.
        JSR     CLRCOMGEN
        LDX     #CMP1ACTWGTRR   ; AREA TO BE CLEARED.
        JSR     CLRCOMGEN
        LDX     #CMP1TARWGT     ;
        JSR     CLRCOMGEN
        LDX     #CMP1ACTWGTI    ; AREA TO BE CLEARED.
        JSR     CLRCOMGEN
        LDX     #CMP1ACTWGTRRI  ; AREA TO BE CLEARED.
        JSR     CLRCOMGEN
        LDX     #CMP1TARWGTI    ;
        JSR     CLRCOMGEN
        RTS


CLRCOMGEN:
        LDAA    #1
        LDAB   #RAMBANK
        TBZK                    ;~;SET EK FOR BANK 1, I.E. RAM
TABLECHK:
        LDZ     #SEQTABLE       ; TABLE START ADDRESS
        CLR     LOOPCTR         ; LOOP COUNTER
NEXTCOMPCHK:
        INC     LOOPCTR
        LDAB    LOOPCTR
        CMPB    #MAXCOMPONENTNO             ;
        BEQ     EXITCLR
        LDAB    0,Z
        CMPB    #SEQDELIMITER   ; AT END
        BEQ     CLEAR
        ANDB    #$0F
        CBA                     ; IS THIS COMP USED ?
        BEQ     NOCLEAR         ; IN TABLE NO CLEARANCE.
        TST     TOPUPF           ; TOP UP ?
        BEQ     NOTUPCL         ; NO TOP UP CLEAR.
        LDAB    SEQTABLE        ; LOAD 1ST COMPONENT.
        ANDB    #$0F
        LDAB    #EIGHT
        CBA                     ; CHECK FOR TOP UP COMPONENT.
        BEQ     NOCLEAR         ; IN TABLE NO CLEARANCE.
NOTUPCL AIZ     #1              ; NEXT TABLE POSITION.
        BRA     NEXTCOMPCHK
CLEAR   TAB
        PSHM    X               ; SAVE X
        DECB
        ABX
        ABX
        ABX                     ; COMPONENT DATA ADDRESS
        CLRW    0,X
        CLR     2,X             ; CLEAR VARIABLE.
        PULM    X               ; RECOVER X
NOCLEAR INCA                    ; TO NEXT COMP
        CMPA    #MAXCOMPONENTNO+1 ; FAIL SAFE EXIT
        BNE     TABLECHK
EXITCLR RTS

;

*/


/*

MAKECYC LDAA    #LEVPOLL
        STAA    BATSEQ
        JSR     CLOSEALL
        CLRW    CMPNTIM
        CLR     CMPNTIM+2
        JSR     TOMANUAL
        LDX     #SEQTABLE
        STX     SEQPOINTER      ; RESET TO START OF TABLE
        RTS

*/
/*
#define  BATSEQ_AUTOCYCLETARE       1       // AUTOCYCLETARE   EQU     1
#define  BATSEQ_CALCNTPINT          2       // CALCNTPINT      EQU     2
#define  BATSEQ_LEVPOLL             3       // LEVPOLL         EQU     3       ; LEVEL SENSOR POLL MODE
#define  BATSEQ_DUMP                4       // DUMP            EQU     4       ; DUMP MODE OF OPERATION
#define  BATSEQ_TAREID              5       // TAREID          EQU     5
#define  BATSEQ_MATERIALFALL        6       // MATERIALFALL    EQU     6
#define  BATSEQ_AFTERTAREID         7       // AFTERTAREID     EQU     7       ; TIME BETWEEN TARING AND FILLING.
#define  BATSEQ_ENDCYCLEID          8       // ENDCYCLEID      EQU     8       ; END CYCLE NOW ;
#define  BATSEQ_FILL                9       // FILL            EQU     9
#define  BATSEQ_OFFDELBDMPID        10      // OFFDELBDMPID    EQU     10
#define  BATSEQ_OFFDELADMPID        11      // OFFDELADMPID    EQU     11
#define  BATSEQ_VACLOADING          12      // VACLOADING      EQU     12      ; VAC LOADING
#define  BATSEQ_NOFILL              13      // NOFILL          EQU     13
#define  BATSEQ_WAITFORTRAN         14      // WAITFORTRAN     EQU     14      ; WAIT FOR LS TRANSITION.
#define  BATSEQ_DELAYBFILL          15      // DELAYBFILL      EQU     15      ; DELAY BEFORE FILL.
#define  BATSEQ_PAUSEBFILL          16      // PAUSEBFILL      EQU     16      ; PAUSE MODE.
#define  BATSEQ_BYPASSID            17      // BYPASSID        EQU     17      ; BY PASS MODE
#define  BATSEQ_CONFIRMPAUSEID      18      // CONFIRMPAUSEID  EQU     18      ; CONFIRM
#define  BATSEQ_CONFIRMBYPASSID     19      // CONFIRMBYPASSID EQU     19      ; CONFIRM BYPASS.
#define  BATSEQ_VOLMODE             20      // VOLMODE         EQU     20      ; VOLUMETRIC MODE.
#define  BATSEQ_SETTLING            21      // SETTLING        EQU     21      ; HOPPER WT SETTLING.
#define  BATSEQ_MIXINGID            22      // MIXINGID        EQU     22      ; MIXING ID.
#define  BATSEQ_DOUBLEDUMPCLOSEID   23      // DOUBLEDUMPCLOSEID EQU   23
#define  BATSEQ_DOUBLEDUMPOPENID    24      // DOUBLEDUMPOPENID  EQU   24
#define  BATSEQ_DOUBLEDUMPFREQID    25      // DOUBLEDUMPFREQID  EQU   25


CYCMON:
        LDAA    BATSEQ          ;
        CMPA    #BYPASSID
        BNE     NOTBYP          ; NOT BYPASS MODE.
        TST     BUBBRK     ;
        BEQ     ISOFF
        JSR     CLOSEBYPASS     ; CLOSE OFF BYPASS
        RTS
ISOFF   JSR     OPENBYPASS      ; OPEN BYPASS VALVE.
        RTS
NOTBYP  CMPA    #SETTLING       ; SETTLING
        BNE     NOTSTL          ; NO
        LDD     DMPSEC          ;
        TST     FASTFLG         ; CHECK MODE IN CALIBRATION DATA.
        BNE     SLOWMD          ; SLOW MODE.
        CPD     #FASTSETTLINGTIME   ; SETTLING TIME.
        BRA     CMPSET
SLOWMD  CPD     #SLOWSETTLINGTIME   ; SETTLING TIME.
CMPSET  BLS     NOTSTL
        JSR     RESETAVG        ; RESET AVERAGE.
        LDAA    #CALC           ; CALCULATE AVERAGE WHEN AVAILABLE.
        STAA    BATSEQ          ;
NOTSTL  TST     FIRSTBATCH      ; IS THIS THE INITIAL BATCH ?
        BEQ     NOAUTOCYCLE     ;
        LDAA    BATSEQ          ; CHECK SEQUENCER
        CMPA    #AUTOCYCLETARE  ; SHOULD WE TARE FOR AUTOCYCLER ?
        BNE     NOAUTOCYCLE     ; SKIP
        LDD     DMPSEC          ;
        CPD     #AUTOCYCLEDELAY
        BLS     NOTAREYET
        JSR     NORMTARE
        LDAA    #$0F
        STAA    AUTOCYCLEFLAG
        LDAA    #LEVPOLL
        STAA    BATSEQ          ; SET LEVEL SENSOR POLLING.

NOTAREYET:
        RTS

MAKECYC LDAA    #LEVPOLL
        STAA    BATSEQ
        JSR     CLOSEALL
        CLRW    CMPNTIM
        CLR     CMPNTIM+2
        JSR     TOMANUAL
        LDX     #SEQTABLE
        STX     SEQPOINTER      ; RESET TO START OF TABLE
        RTS


NOAUTOCYCLE:

        TST     STOPTRAN
        BEQ     NOSTTR
        CLR     STOPTRAN
;        TST     PAUSFLG         ; IS THE BLENDER PAUSED ?
;        BEQ     ISPAUSD
ISPAUSD CLR     CYCLEINDICATE   ; RESET CYCLE INDICATE
        JSR     CLEARTHROUGHPUTTOTALS
        JSR     MAKECYC
        RTS

NOSTTR  TST     RUNTRAN
        BEQ     NOCHANGE
        TST     PAUSFLG         ; PAUSED ?
        BNE     NOCHANGE        ; SKIP THIS IF PAUSED.
        CLR     RUNTRAN
MAKEDUMP:
        CLRW    DMPSEC
        LDAA    #DUMP
        STAA    BATSEQ
        RTS


NOCHANGE LDAA    BATSEQ          ;
        CMPA    #LEVPOLL        ; IS IT TIME TO CHECK THE LEVEL SENSOR
        LBNE     NOSENSOR        ; SEQUENCE ERROR OCCURRED
        LDAA    CLEANCYCLESEQ
        CMPA    #CLEANPHASE2ID   ; IS THIS NOW CLEAN PHASE 2
        LBEQ     DOPHASE2CLEAN

        LDAA    BLENDERMODE     ; CHECK MODE
        CMPA    #OFFLINEMODE    ; OFFLINE MODE
        BNE     ONLI          ;
        TST     PAUSFLG
        LBNE     NOSENSOR

ONLI    TST     DMPLEVSEN       ; CHECK LEVEL SENSOR ACTIVE FLAG
        BNE     ISACTIVE        ; NOT ACTIVE
        JMP     NEXTIDCHK
XITAL   RTS

ISACTIVE:
        TST     STOPPED
        BNE     XITAL
        CLR     DMPLEVSEN       ; CLEAR "DUMP LEVEL SENSOR" FLAG
        JSR     SETMIXERTIME    ; SET MIXER TIME FOR BLENDER

        JSR     CLOSEALL        ;
        LDD     CYCLETIME
        STD     CYCLETIMEPRN
        CLRW    CYCLETIME

        TST     FIRSTBATCH      ; IS THIS THE FIRST BATCH
        BNE     IS1STBATCH

        JSR     BATCHLENGTHCAL  ; CALCULATE LENGTH
        JSR     BATCHGPMCALC    ; GRAMS PER METER CALCULATED.
        BRA     SKIPTHIS
IS1STBATCH:
        JSR     PRINTDIAGHEADER:
        JSR     BATCHLENGTHCAL  ; CALCULATE LENGTH

SKIPTHIS:

        JSR     CALHWTAV         ; WEIGHT CALCULATION
        LDX     EREG            ;
        STX     CURBATWGH       ; CURRENT BATCH WEIGHT
        LDX     EREG+2
        STX     CURBATWGH+2     ;
        TST     FIRSTWCYCLE     ; IS THIS THE FIRST WEIGH CYCLE ?
        BNE     YES1ST          ; YES

        JSR     CHECKHOPWT      ; CHECK HOPPER WEIGHT.


 YES1ST:
        LDAA    BLENDERMODE     ; CHECK MODE
        CMPA    #OFFLINEMODE    ; OFFLINE MODE
        LBEQ     OFFLINE         ; OFFLINE MODE.
        JSR     OPENDUMP        ; DUMP MATERIAL
        JSR     BATSECAL

        TST     ESTCTRUSE       ; USE ESTIMATION COUNTER.
        BEQ     NORCALC         ; NORMAL CALCULATION.
        DEC     ESTCTRUSE
        LDAA    #1
        STAA    RUNEST          ; SIGNAL ESTIMATION TO RUN.
        BRA     NOCALC
NORCALC JSR     CALKGPHR

;        TST     MANATO          ; IF MANUAL DO NOT WORRY ABOUT CHECKING AUTO
;        BEQ     DOCALC
;        TST     CHECKLSENSORINNSECONDS
;        BEQ     DOCALC          ; DO CONTROL ACTION
;        TST     LEVSEN1STAT       ; LEVEL SENSOR STATUS
;        BEQ     DOCALC            ; SENSOR ON, IS OKAY
;        LDAA    #1
;        STAA    DISCONACTION      ; DISABLE CONTROL ACTION
;        BRA     NOCALC            ; SKIP CALC
        TST     DISCONACTION
        BNE     NOCALC             ; NO UPDATE IF CONTROL ACTION IS DISABLED
DOCALC  JSR     CALCDACPKG
NOCALC  LDD     CURBATWGH       ; READ CURRENT BATCH
        STD     PREVBATCHWEIGHT ;
        LDAA    CURBATWGH+2     ; CURRENT WEIGHT TO PREVIOUS BATCH FOR NEXT CYCLE
        STAA    PREVBATCHWEIGHT+2
        JSR     ACUMWT          ; ACCUMULATE ROLL AND ORDER WEIGHT
        JSR     CPYTWTMB
        TST     FIRSTBATCH      ;
        BNE     NOALGCALL       ; NO ALGORITHM CALL
        JSR     SETLEVELSENSORWDOG
        TST     DISCONACTION
        BNE     ACTIONDIS
        JSR     ALGOR1          ; CONTROL ALGORITHM CALLED
        JSR     CALC_ACTMIC     ; ACTUAL MICRONS CALCULATED.
        BRA     CONCYCLE
ACTIONDIS:
        CLR     DISCONACTION
CONCYCLE:
        JSR     PRINTPERERROR
        BRA     SKIPCLEAR
NOALGCALL:
        CLR     FIRSTBATCH      ; RESET
SKIPCLEAR:
;        CLRW    DMPSEC
;        LDAA    #DUMP           ;
;        STAA    BATSEQ          ; INDICATED THAT "DUMP SEQUENCE IS NEXT.
        JSR     CLOSEFEED       ; CLOSE PINCH VALVE IN THROAT.
        CLRW    DMPSEC          ; RESET 1/10 SECOND COUNTER.
        CLR     OFFLINE_TRK     ; RESET TRACKING FLAG.
        LDAA    #OFFDELBDMPID    ; "DELAY BEFORE DUMP ID
        STAA    BATSEQ          ; SET TRACKING FLAG

        RTS
;
;       OFFLINE MODE OF OPERATION.

OFFLINE TST     PAUSFLG         ; IS BLENDE PAUSED ?
        BNE     XITOFF          ; DO NOT CLOSE PINCH VALVE.
        TST     BATCHREADY
        BNE     XITOFF          ; READY TO PAUSE
        JSR     CLOSEFEED       ; CLOSE PINCH VALVE IN THROAT.
        JSR     BATSECAL
        JSR     CALKGPHR
        LDAA    OFFLINETYPE             ;CHECK OFFLINE TYPE.
        CMPA    #OFFLINETYPEHILO        ; BLEND TO XX KGS.
        BNE     NOCMR                   ; NO  CMR
        JSR     CALCCMR         ; UPDATE D/A DEPENDING ON ACTUAL KG/HR
NOCMR   LDD     CURBATWGH       ; READ CURRENT BATCH
        STD     PREVBATCHWEIGHT ;
        LDAA    CURBATWGH+2     ; CURRENT WEIGHT TO PREVIOUS BATCH FOR NEXT CYCLE
        STAA    PREVBATCHWEIGHT+2
        JSR     ACUMWT          ; ACCUMULATE ROLL AND ORDER WEIGHT
        JSR     CPYTWTMB
        CLRW    DMPSEC          ; RESET 1/10 SECOND COUNTER.
        CLR     OFFLINE_TRK     ; RESET TRACKING FLAG.
        LDAA    #OFFDELBDMPID    ; "DELAY BEFORE DUMP ID
        STAA    BATSEQ          ; SET TRACKING FLAG
XITOFF  RTS

NOSENSOR:
        LDAA    BLENDERMODE     ; CHECK MODE
        CMPA    #OFFLINEMODE    ; OFFLINE MODE
        BNE     ONLINE          ;
        TST     OFFLSF          ; OFFLINE LEVEL SENSOR FLAG.
        BEQ     ONLINE          ;
        CLR     OFFLSF          ; RESET FLAG.
        TST     BATCHREADY
        BNE     XITOF1          ; READY TO PAUSE
        TST     PAUSFLG         ; IS BLENDER PAUSED ?
        BNE     XITOF1          ; DO NOT CLOSE PINCH VALVE.
        JSR     CLOSEFEED       ;
        LDAA    #OFFDELADMPID  ; DELAY #2 NOW
        STAA    OFFLINE_TRK     ; SET SEQUENCE.
        CLR     OFFTIM1         ; OFF LINE COUNTER RESET
        TST     PAUSFLG         ; IS BLENDER PAUSED ?
        BNE     XITOF1          ; DO NOT CLOSE PINCH VALVE.
ONLINE  LDAA    BATSEQ
        CMPA    #OFFDELBDMPID  ;"OFFLINE DELAY BEFORE DUMP
        BNE     NEXTIDCHK
        LDD     DMPSEC          ; 1/10 SECOND COUNTER.
        CPD    #OFFDELAY1      ; OFFLINE DELAY #1
        BLS     XITOF1
        JSR     OPENDUMP        ; RELEASE MATERIAL.
        LDAA    #DUMP
        STAA    BATSEQ         ; NOW TARE THE HOPPER AS NORMAL.
        JSR     STARTOFFLINEDELAY
XITOF1:
        RTS


STARTOFFLINEDELAY:
        JSR     CLOSEFEED       ;
        LDAA    #OFFDELADMPID  ; DELAY #2 NOW
        STAA    OFFLINE_TRK     ; SET SEQUENCE.
        CLR     OFFTIM1         ; OFF LINE COUNTER RESET
        CLRW    DMPSEC
        RTS








NEXTIDCHK:
        LDAA    OFFLINE_TRK
        CMPA    #OFFDELADMPID   ; SEQUENCE AFTER DUMP.
        BNE     NEXTIDCHK1
        LDAA    OFFTIM1         ; OFF
        LDAB    MIXTIM          ; MIXING TIME BCD
        JSR     BCDHEX1         ; TO HEX
        CBA
        BNE     NEXTIDCHK1      ; CHECK NEXT ID ANYWAY
        JSR     OPENFEED        ; OPEN PINCH VALVE
        LDAA    #TEN
        STAA    CHECKLSENSORINNSECONDS
        TST     BATCHREADY
        BEQ     NOTRDY          ; NO
        JSR     STARTUPON
        CLR     BATCHREADY      ; RESET BATCH READY SIGNAL
NOTRDY  CLR     OFFLINE_TRK     ; RESET TRACKING FLAG.


NEXTIDCHK1:
        LDAA    BLENDERTYPE
        CMPA    #TINYTYPE       ; TINY TYPE.
        BNE     NOTMXOFF        ; NO

        LDAA    #1
        STAA     MIXOFF           ; MIXNER OFF.
NOTMXOFF:
        LDAA    BATSEQ          ; SEQUENCE COUNTER
        CMPA    #DUMP           ; HAS THE DUMPING SEQUENCE BEING STARTED.
        BNE     NOTDMPS         ; NO
        TST     AUTOCYCLEFLAG   ; AUTO CYCLE ENABLED ?
        BNE     CHECKWT         ; SKIP MIN CHECK IF AUTOCYCLER.
        LDAA    AN1BUFH
        BNE     CHECKWGT
        LDD     AN1BUFH+1
        CPD     #35000
        BLS     CONDMP          ; DUMP IF LESS THAN 40000
CHECKWGT:


        TST     STOPPED         ; STOPPED ?
        BNE     CONDMP1
CHECKWT JSR     CALHWT          ; CALCULATE WEIGHT
        LDD     EREG            ;
        CPD     #MINWEIGHT      ; CHECK IF BELOW 100 GRAMS ?
        BHI     CONDMP          ; CONTINUE TO DUMP MATERIAL
        LDAA    MBPROALRM       ;
        ANDA    #TAREALBIT      ; TARE ALARM BIT
        BEQ     NOALTR          ; NO TRANSITION.
        LDAA    #TAREALARM      ;
        JSR     RMALTB          ; INDICATE ALARM.
NOALTR  LDAA    #MATERIALFALL         ;
        STAA    BATSEQ          ; MATERIAL HAS FALLEN WAIT N SECONDS
        CLRW    DMPSEC          ; BEFORE CLOSING.
        JSR     NEEDFORTOUP
        RTS
CONDMP  JSR     OPENDUMP
        LDD     DMPSEC          ;
        CPD     #MXTARTIM       ; MAX TARE TIME.
        BLS     OKFONOW         ; OKAY
        CLRW    DMPSEC          ; COUNTER.
        LDAA    MBPROALRM       ;
        ANDA    #TAREALBIT      ; TARE ALARM BIT
        BNE     NOALRP
        LDAA    #TAREALARM      ;
        JSR     PTALTB          ; INDICATE ALARM.
NOALRP  RTS
OKFONOW RTS

CONDMP1 JSR     OPENDUMP
        RTS

NOTDMPS CMPA    #MATERIALFALL   ; IS MATERIAL STILL FALLING ?
        BNE     NOTMATF         ;  NO
        LDD     DMPSEC          ; READ SECONDS COUNTER
        CPD    #MATFALLTIME    ; TIME BEFORE TARE.
        BLS     NOTALLFALL      ; YES
        LDAA    CLEANCYCLESEQ   ; IS THIS A CLEAN CYCLE
        CMPA    #CLEANPHASE1ID  ; IS THIS PHASE 1
        LBEQ     ISPHASE1CLEAN   ; IS PHASE 1 CLEAN
        JSR     CLOSEDUMP       ; CLOSE FLAP
        TST     DOUBLEDUMP      ; IS DOUBLE DUMP REQUIRED
        BEQ     TARASN          ; TARE AS NORMAL
        LDAA    #DOUBLEDUMPCLOSEID
        BRA     STRACD          ; STORE AND CLEAR COUNTER

TARASN  LDAA    #TAREID         ;
STRACD  STAA    BATSEQ          ; TAREING IS NEXT IN THE SEQUENCE
        CLRW    DMPSEC          ; RESET DUMP SECONDS COUNTER.
NOTALLFALL:
        RTS


NOTMATF LDAA    BATSEQ          ;
        CMPA    #DOUBLEDUMPCLOSEID; MATERIAL DUMPED ?
        BNE     NOTDD           ; NO
        LDD     DMPSEC          ; READ SECONDS COUNTER
        CPD     #DDCLOSETIME     ; DOUBLE DUMP CLOSE TIME EXCEEDED.
        BLS     NONDDT          ; NO
        JSR     OPENDUMP        ; OPEN DUMP VALVE
        LDAA    #DOUBLEDUMPOPENID;
        STAA    BATSEQ
        CLRW    DMPSEC
NONDDT  RTS


NOTDD   LDAA    BATSEQ          ;
        CMPA    #DOUBLEDUMPOPENID; DOUBLE DUMP OPEN
        BNE     NOTDD1          ; NO
        LDD     DMPSEC          ; READ SECONDS COUNTER
        CPD     #DDOPENTIME     ; DOUBLE DUMP OPEN TIME EXCEEDED.
        BLS     NONDDOT         ; NO
        JSR     CLOSEDUMP       ; CLOSE FLAP
        LDAA    #TAREID         ;
        STAA    BATSEQ          ; TAREING IS NEXT IN THE SEQUENCE
        CLRW    DMPSEC          ; RESET DUMP SECONDS COUNTER.
NONDDOT RTS


NOTDD1  LDAA    BATSEQ          ;
        CMPA    #TAREID         ; MATERIAL DUMPED ?
        BNE     NOTTAR          ; NO
        LDD     DMPSEC          ; READ SECONDS COUNTER
        CPD    #TIMEBTARE      ; TIME BEFORE TARE.
        BLS     NONATCT         ; YES
        JSR     TAREND          ; TARE HOPPER
        LDAA    #$0F
        STAA    TARINGSTATUS    ; INDICATE TARING STATUS
        LDAA    BLENDERTYPE
        CMPA    #TINYTYPE       ; TINY TYPE.
        BNE     NOTTINY          ; NO
        LDAA    #MIXINGID       ; MIXING PERIOD.
        STAA    BATSEQ          ;
        CLR     MIXOFF
        CLRW    DMPSEC          ; RESET COUNTER
NONATCT RTS


NOTTAR  LDAA    BATSEQ          ;
        CMPA    #MIXINGID       ; MIXING
        BNE     NOTMIX          ; NO.
        LDAB    MIXTIM          ; MIXING TIME BCD
        JSR     BCDHEX1         ; TO HEX
        LDAA    #10
        MUL                     ; MULTIPLY.
        CPD     DMPSEC          ; COMPARE TO 10HZ COUNTER.
        BHI     NONATMX         ; YES
        LDAA    #1
        STAA     MIXOFF           ; MIXNER OFF.
NOTTINY LDAA    #AFTERTAREID    ; LOAD VACUUM OFF ID
        STAA    BATSEQ          ;
        CLRW    DMPSEC          ; RESET COUNTER
NONATMX RTS



NOTMIX  LDAA    BATSEQ          ;
        CMPA    #AFTERTAREID       ;
        LBNE     NOTVACDEL       ;
        LDAA    BLENDERTYPE     ; CHECK THE BLENDER TYPE.
        CMPA    #TINYTYPE       ; MICRA  ?
        BNE     INNRDL          ; INSTIGATE NORMAL DELAY
        LDD     DMPSEC          ;
        CPD     #MICRA_AFTER_TARE_TIME  ;
        LBLS     NONATCT1        ;
        BRA     CONMCD          ; CONTINUE MICRA DELAY
INNRDL  LDD     DMPSEC          ;
        CPD     #AFTERTARETIME  ;
        LBLS     NONATCT1        ;
CONMCD  CLR     TARINGSTATUS    ; INDICATE TARING STATUS
NONEAVAIL:
        CLR     FILLCOUNTER
        CLR     FILLALARM       ; RESET COMP FILL ALARM
        JSR     CLEARUNUSED
        CLR     FILLCTR
        JSR     RESETFILLCOUNTERS
        JSR     CHECKLLS
        CLR     ANYFILLALARM    ; RESET FILL ALARM STATUS
        CLR     LEVELSENALOC    ; LEVEL SENSOR ALARM OCC FLAG.
        CLR     RETRYCOMPNOHIS

        TST     ISVOLMODE       ; MODE ENABLED ?
        BRA     NOVOL           ; NO VOLUMETRIC MODE ON 1ST BATCH.
;        BEQ     NOVOL           ; NO VOLUMETRIC MODE ON 1ST BATCH.


        TST     FIRSTWCYCLE     ; FIRST CYCLE NO
        BNE     NOVOL           ; NO VOLUMETRIC MODE ON 1ST BATCH.
        TST     PCTHIS          ; HAS RECIPE BEING UPDATED ?
        BNE     RECCHG          ; YES
        TST     ANYFILLALARM    ; ANY ALARMS IN PREVIOUS CYCLE.
        BNE     NOVOL
        INC     VOLCYCCTR       ; INCREMENT VOLUMETRIC COUNTER.
        LDAA    VOLCYCCTR       ;
        CMPA    #2
        BNE     NOVOL
        CLR     VOLCYCCTR
        LDAA    #VOLMODE
        STAA    BATSEQ          ; ENABLE VOLUMETRIC MODE.
        STAA    CYCLEINDICATE   ; FILLLING IN PROGRESS
        RTS

RECCHG  CLR     PCTHIS          ; RESET FLAG.
NOVOL:
        TST     PAUSFLG         ; IN PAUSE MODE ?
        BNE     NONATCT1        ; PAUSED

        TST     TOPUPF           ; TOP UP ?
        BEQ     NOTUP           ; NO
        LDAA    #FILLFIRST
        STAA    TOPUPSEQUENCING ;
NOTUP   LDAA    #FILL               ;
        STAA    BATSEQ          ; SET BATCH SEQUENCE TO START AT COMP#1
        STAA    CYCLEINDICATE   ; FILLLING IN PROGRESS
        JSR     RESETFILLCOUNTERS
        JSR     CHKFORCLEANPHASE2 ; CHECK FOR CLEAN PHASE 2 CYCLE


NONATCT1:
        RTS
NOTVACDEL:
        CMPA    #VACLOADING     ; VACUUM LOADING.
        BNE     NOTVACLOAD      ; NO
        LDD     DMPSEC          ; READ 1/10 SECONDS COUNTER.
        CPD     #VACDELAY       ;
        BLS     NOTATVACD       ; NOT AT VAC DELAY.
        CLR     FILLCOUNTER
        TST     PAUSFLG         ; IN PAUSE MODE ?
        BNE     NOTATVACD       ; PAUSED
        LDAA    #FILL           ; REFILL SAME COMPONENT AGAIN.
        STAA    BATSEQ          ;
        JSR     RESETFILLCOUNTERS
        JSR     CHKFORCLEANPHASE2 ; CHECK FOR CLEAN PHASE 2 CYCLE
NOTATVACD:
        RTS

NOTVACLOAD:
        CMPA    #WAITFORTRAN    ; "WAITING FOR LS TRANSITION.
        BNE     NOTWFT
        LDX     SEQPOINTER
        LDAB    0,X
        ANDB    #$0F
        LDX     #LS1ONTR        ; TRANSITION OFF TO ON.
        DECB
        ABX
        TST     0,X             ;
        BEQ     XITWFT          ; EXIT WAIT FOR TRANSITION.
        CLR     0,X             ; RESET TRANSITION FLAG.
        CLRW    DMPSEC
        LDAA    #DELAYBFILL
        STAA    BATSEQ          ; SET SEQUENCER ID.
XITWFT  RTS

;NOTWFT  CMPA    #DELAYBFILL     ; DELAY BEFORE FILLING.
;        BNE     NOTDFF          ; NO
;        LDD     DMPSEC
;        CPD     #200
;        BLS     NOTDFF          ;
;        CLR     FILLCTR
;        CLR     FILLCOUNTER
;        CLR     LEVELSENALOC    ; LEVEL SENSOR ALARM OCC FLAG.
;        TST     PAUSFLG         ; IN PAUSE MODE ?
;        BNE     NOTDFF          ; PAUSED
;        LDAA    #FILL               ;
;        STAA    BATSEQ          ; SET BATCH SEQUENCE TO START AT COMP#1
;        STAA    CYCLEINDICATE   ; FILLLING IN PROGRESS
;NOTDFF  RTS


CHKFILLBYW:
        CLRA                    ; NO
        RTS



NOTWFT  CMPA    #DELAYBFILL     ; DELAY BEFORE FILLING.
        BNE     CHKCLC          ; CHECK CLEAN CYCLE
        LDD     DMPSEC
        CPD     #200
        BLS     NOTDFF          ;
        CLR     FILLCTR
        CLR     FILLCOUNTER
        CLR     LEVELSENALOC    ; LEVEL SENSOR ALARM OCC FLAG.
        TST     PAUSFLG         ; IN PAUSE MODE ?
        BNE     NOTDFF          ; PAUSED
        LDAA    #FILL               ;
        STAA    BATSEQ          ; SET BATCH SEQUENCE TO START AT COMP#1
        STAA    CYCLEINDICATE   ; FILLLING IN PROGRESS
        JSR     RESETFILLCOUNTERS
        JSR     CHKFORCLEANPHASE2 ; CHECK FOR CLEAN PHASE 2 CYCLE
NOTDFF  RTS

;CHKFILLBYW:
        CLRA                    ; NO
        RTS


;  CLEANING PHASE 1
;
ISPHASE1CLEAN:
        CLR    BCLEANREPETITIONSCTR    ; COUNTER INITIATED
        CLR     MCLEANREPETITIONSCTR    ; COUNTER INITIATED
        LDAA   #1
        STAA    DUMPREPETITIONS
        CLRA
        LDAB    PH1ONTIME
        JSR     BCDHEX1
        STD     AIRJET1ONCTR            ; AIR JET 1 COUNTER ONE TIME
        LDAA    #ACTIVATINGJET1ID
        STAA    BATSEQ                  ; STORE SEQUENCING.
STANXI  RTS


;       JET ONE ON
CHKCLC:
        LDAA    BATSEQ                  ; SEQUENCE.
        CMPA    #ACTIVATINGJET1ID       ; ACTIVATING JET 1
        BNE     CHKFRJ1OFF              ;  JET 1 OFF
        LDD     AIRJET1ONCTR            ; READ SECONDS COUNTER
        BNE     STBLJ1                  ; STILL BLOWING JET 1
        LDAA    #AIRJET1OFF1ID
        STAA    BATSEQ                  ; STORE SEQUENCING.
        CLRW    DMPSEC                  ; READ 1/10 SECONDS COUNTER.
STBLJ1  RTS

;       JET ONE OFF
;       INITITATE REPETITIONS

CHKFRJ1OFF:
        LDAA    BATSEQ                  ; SEQUENCE.
        CMPA    #AIRJET1OFF1ID
        BNE     CHKFRDM
        CLRA
        LDAB    PH1OFFTIME
        JSR     BCDHEX1
        CPD     DMPSEC
        BHI     LVJ1OFF                 ; LEAVE JET 1 OFF
        INC     BCLEANREPETITIONSCTR    ; COUNTER INITIATED
        LDAA    BCLEANREPETITIONSCTR
        CMPA    BCLEANREPETITIONS
        BHI     BCLFIN
        BEQ     BCLFIN

        LDAA    #ACTIVATINGJET1ID
        STAA    BATSEQ                  ; STORE SEQUENCING.
        CLRA
        LDAB    PH1ONTIME
        JSR     BCDHEX1
        STD     AIRJET1ONCTR            ; AIR JET 1 COUNTER ONE TIME
LVJ1OFF RTS
BCLFIN:
        CLR     BCLEANREPETITIONSCTR    ; =0
        LDAA    #CLOSEDUMP1ID           ;
        STAA    BATSEQ                  ; TAREING IS NEXT IN THE SEQUENCE
        CLRW    DMPSEC                  ; READ 1/10 SECONDS COUNTER.
        BRA     LVJ1OFF

;       CLOSE DUMP

CHKFRDM LDAA    BATSEQ                  ; SEQUENCE.
        CMPA    #CLOSEDUMP1ID           ; DUMP 1 ?
        BNE     CHKOD1                  ; CHECK NEXT.
        LDD      DMPSEC                  ; DUMP SECONDS
        CPD      #CLOSEDUMP1TIME10S      ; CLOSE DUMP 1 TIME IN 10TH SECOND
        BLS      XITCD1          ;
        JSR      CLOSEDUMP                ; OPEN DUMP FLAP
        LDAA     #OPENDUMP1ID           ;
        STAA     BATSEQ                  ; TAREING IS NEXT IN THE SEQUENCE
        CLRW     DMPSEC
XITCD1  RTS

;       OPEN DUMP

CHKOD1  LDAA    BATSEQ                  ; SEQUENCE.
        CMPA    #OPENDUMP1ID            ; DUMP 1 ?
        BNE     CHKCD2                  ; CHECK NEXT.
        LDD      DMPSEC                 ; DUMP SECONDS
        CPD      #OPENDUMP1TIME10S      ; CLOSE DUMP 1 TIME IN 10TH SECOND
        BLS      XITOD2                 ;
        JSR      OPENDUMP               ; OPEN DUMP FLAP
        LDAA     #CLOSEDUMP2ID           ;
        STAA     BATSEQ                  ; TAREING IS NEXT IN THE SEQUENCE
        CLRW     DMPSEC
XITOD2  RTS


;       CLOSEDUMP

CHKCD2 LDAA    BATSEQ                  ; SEQUENCE.
       CMPA    #CLOSEDUMP2ID           ; DUMP 1 ?
       BNE     CHKOD2                  ; CHECK NEXT.
       LDD      DMPSEC                  ; DUMP SECONDS
       CPD      #CLOSEDUMP1TIME10S      ; CLOSE DUMP 1 TIME IN 10TH SECOND
       BLS      XITCD2          ;
       JSR      CLOSEDUMP                ; OPEN DUMP FLAP
       TST      DUMPREPETITIONS
       BEQ      NOREP
       DEC      DUMPREPETITIONS
       LDAA     #OPENDUMP1ID           ;
       BRA      SETSEQ



NOREP  LDAA     #SETTLEAFTERBANGID
SETSEQ STAA     BATSEQ
       CLRW     DMPSEC
XITCD2 RTS

CHKOD2  LDAA    BATSEQ                   ; SEQUENCE.
        CMPA    #SETTLEAFTERBANGID       ; DUMP 1 ?
        BNE     CHKCC                    ; CHECK FOR CLEAN PHASE 2
        LDD     DMPSEC                  ; DUMP SECONDS
        CPD     #SETTLEAFTERBANGTIME10S ; CLOSE DUMP 1 TIME IN 10TH SECOND
        BNE     XITSAB                  ; EXIT
        LDAA    #1
        STAA    PHASEONEFINISHED        ; HAS CLEAN CYCLE #1 FINISHED ?
        TST     NOFILLONCLEAN           ; NO FILL ON CLEAN CYCLE.
        BEQ     TARASN1                  ; TARE AS NORMAL.
;        CLR     NOFILLONCLEAN

        CLR     PHASEONEFINISHED ; RESET
        LDAA    #CLEANPHASE2ID  ; CLEAN PHASE 2 CYCLE.
        STAA     CLEANCYCLESEQ

        LDAA    #LEVPOLL                 ;
        STAA    BATSEQ                  ; TAREING IS NEXT IN THE SEQUENCE
        BRA     XITSAB

TARASN1 LDAA    #TAREID                 ;
        STAA    BATSEQ                  ; TAREING IS NEXT IN THE SEQUENCE
        CLRW    DMPSEC                  ; RESET DUMP SECONDS COUNTER.
XITSAB  RTS

;       AIR JET 2 BEING ACTIVATED.
;


CHKCC   LDAA    BATSEQ                  ;
        CMPA    #CCLACTIVEID
        BNE     CHCCLOFF                  ;
        LDD     CCLONCTR             ; CC LEFT ON
        BNE     XITCCL                  ; NO
        LDAA    #CCLOFFID             ;      ;
        STAA    BATSEQ                  ;
        CLRW    DMPSEC                  ; READ 1/10 SECONDS COUNTER.
XITCCL  RTS


CHCCLOFF LDAA    BATSEQ                  ; SEQUENCE.
        CMPA    #CCLOFFID               ; JET 2 OFF TIME
        BNE     CHCRON                  ;  NO
        JSR     CORNERCLEANLEFTOFF
        CLRA
        LDAB    PH2OFFTIME
        JSR     BCDHEX1
        CPD     DMPSEC
        BHI     CCLOFF               ; LEAVE JET 1 OFF
        LDAA    #CCRACTIVEID
        STAA    BATSEQ                  ;
        CLRA
        LDAB    PH2ONTIME
        JSR     BCDHEX1
        STD     CCRONCTR                 ; AIR JET 1 COUNTER ONE TIME
        CLR     DMPSEC
CCLOFF  RTS

CHCRON  LDAA    BATSEQ                  ;
        CMPA    #CCRACTIVEID
        BNE     CHCCROFF                  ;
        LDD     CCRONCTR                ; CC LEFT ON
        BNE     XITCL                   ; NO
        LDAA    #CCROFFID             ;      ;
        STAA    BATSEQ                  ;
        CLRW    DMPSEC                  ; READ 1/10 SECONDS COUNTER.
XITCL   RTS

CHCCROFF LDAA    BATSEQ                  ; SEQUENCE.
        CMPA    #CCROFFID               ; JET 2 OFF TIME
        BNE     CHCLP2                  ;  NO
        JSR     CORNERCLEANRIGHTOFF:
        CLRA
        LDAB    PH2OFFTIME
        JSR     BCDHEX1
        CPD     DMPSEC
        BHI     CCLFOFF               ; LEAVE JET 1 OFF

        INC     MCLEANREPETITIONSCTR    ; COUNTER INITIATED
        LDAA    MCLEANREPETITIONSCTR
        CMPA    MCLEANREPETITIONS
        BEQ     FINCC
        BHI     FINCC
        LDAA    #CCLACTIVEID
        STAA    BATSEQ                  ;; REPEAT CORNER CLEAN
        CLRA
        LDAB    PH2ONTIME
        JSR     BCDHEX1
        STD     CCLONCTR                 ;
        CLR     DMPSEC
        BRA     CCLFOFF


FINCC   CLR     MCLEANREPETITIONSCTR
        CLRA
        LDAB    PH2ONTIME
        JSR     BCDHEX1
        STD     AIRJET2ONCTR            ; AIR JET 2 COUNTER ON TIME
        LDAA    #ACTIVATINGJET2ID
        STAA    BATSEQ                  ; STORE SEQUENCING.
CCLFOFF RTS
















CHCLP2  LDAA    BATSEQ                  ; SEQUENCE.
        CMPA    #ACTIVATINGJET2ID       ; ACTIVATING JET 2
        BNE     AJ2OFF                  ;  NO
        LDD     AIRJET2ONCTR            ; READ SECONDS COUNTER
        BNE     STBLJ2                  ; STILL BLOWING JET 2
        LDAA    #AIRJET2OFFID             ;      ;
        STAA    BATSEQ                  ;
        CLRW    DMPSEC                  ; READ 1/10 SECONDS COUNTER.
STBLJ2  RTS

;       AIR JET 2 OFF
;
AJ2OFF  LDAA    BATSEQ                  ; SEQUENCE.
        CMPA    #AIRJET2OFFID           ; JET 2 OFF TIME
        BNE     LVJ2OFF                 ;  NO
;        JSR     CORNERCLEANON
        CLRA
        LDAB    PH2OFFTIME
        JSR     BCDHEX1
        CPD     DMPSEC
        BHI     LVJ2OFF                 ; LEAVE JET 1 OFF

        INC     MCLEANREPETITIONSCTR    ; COUNTER INITIATED
        LDAA    MCLEANREPETITIONSCTR
        CMPA    MCLEANREPETITIONS
        BEQ     FINCLC
        BHI     FINCLC
        CLRA
        LDAB    PH2ONTIME
        JSR     BCDHEX1
        STD     AIRJET2ONCTR            ; AIR JET 1 COUNTER ONE TIME
        JSR     CORNERCLEANLEFTOFF
        JSR     CORNERCLEANRIGHTOFF
        LDAA    #ACTIVATINGJET2ID
        STAA    BATSEQ                  ; STORE SEQUENCING.
        BRA     LVJ2OFF

FINCLC  CLR     MCLEANREPETITIONSCTR    ; COUNTER = 0
        JSR     CORNERCLEANLEFTOFF
        JSR     CORNERCLEANRIGHTOFF
        CLR     CLEANCYCLESEQ           ; RESET CLEANING CYCLE
        JSR     SETENDCLEANMBBIT
        LDD     #MBCSTATCLEANST2POS
        COMD
        ANDA    MBOPSTS
        STD     MBOPSTS

        TST     NOFILLONCLEAN
        BEQ     NOFILC                  ; NO FILL ON CLEAN
        CLR     NOFILLONCLEAN
        JSR     PAUSEON                 ; PAUSE BLENDER
        CLR     NOFILLONCLEAN           ; EXIT.
NOFILC  LDAA    #LEVPOLL           ;
        STAA    BATSEQ                  ; TAREING IS NEXT IN THE SEQUENCE
        CLRW    DMPSEC                  ; READ 1/10 SECONDS COUNTER.
LVJ2OFF RTS



NOTDLAC RTS
CHKNXTS RTS


DOPHASE2CLEAN:

; PHASE 2 CLEAN, IGNORE LEVEL SENSOR
; CHECK FOR BYPASS SENSOR OFF HIGH LEVEL SENSOR
; ACTIVATE AIR JETS 2
; SET BATSEQ TO LEVPOLL
;
        JSR     OPENFEED         ; OPEN FEED VALVE
        TST      HIGHLEVELSENSOR ; LEVEL SENSOR 1 STATUS
        BNE      XBYPONN                 ; BYPASS SENSOR IS ON
        LDD     #MBCSTATCLEANST2POS
        ORD     MBOPSTS
        STD     MBOPSTS

        LDD     #AIRJET1ONTIMEPH210S       ; AIR JET 1 ON IN 1/10 OF SECOND


; START PAHSE 2
PARAICSTOPP:

        LDAA    BLENDERTYPE             ;
        CMPA    #BABYTYPE               ; BABY TYPE BLENDER
        LBEQ     FINCC                  ; SKIP CORNER CLEANING AND GO TO MIXER CLEAN

        LDAA    #CCLACTIVEID
        STAA    BATSEQ                  ;; INITIATE CORNER CLEAN
        CLRA
        LDAB    PH2ONTIME
        JSR     BCDHEX1
        STD     CCLONCTR                 ; AIR JET 1 COUNTER ONE TIME
        CLR     DMPSEC


;        LDAB    PH2ONTIME
;        JSR     BCDHEX1
;        STD     AIRJET2ONCTR            ; AIR JET 1 COUNTER ONE TIME
;        JSR     CORNERCLEANOFF
;        LDAA    #ACTIVATINGJET2ID
;        STAA    BATSEQ                  ; STORE SEQUENCING.
XBYPONN RTS



; CHECK FOR CLEAN PHASE 2 REQUIREMENT.

CHKFORCLEANPHASE2:
        LDAA    PHASEONEFINISHED ; HAS CLEAN CYCLE #1 FINISHED ?
        BEQ     XITCC            ; YES, IT HAS FINISHED
        CLR     PHASEONEFINISHED ; RESET
        LDAA    #CLEANPHASE2ID  ; CLEAN PHASE 2 CYCLE.
        STAA     CLEANCYCLESEQ

XITCC   RTS
NOTPH1  CLR     CLEANCYCLESEQ
        BRA     XITCC           ; EXIT CLEAN CHECK


;*****************************************************************************;
; FUNCTION NAME : INITITATECLEANCYCLE                                         ;
; FUNCTION      : INIITATE THE CLEAN CYCLE                                    ;
; INPUTS        : NONE                                                        ;                       ;
;*****************************************************************************;


INITIATECLEANCYCLE:
        CLR      OVERRIDECLEAN
        JSR      RESETCLEANFINISHBIT
        JSR      SETSTARTCLEANMBBIT
        TST      CYCLEINDICATE   ; ARE WE IN A CYCLE
        BEQ      NOCCYLE          ; NO
        LDAA     #1
        STAA     SETCLEANWHENCYCLEEND
        BRA      CLNXIT          ; DISPLAY PAGE
NOCCYLE JSR      STARTCLEANCYCLE
CLNXIT  RTS


;*****************************************************************************;
; FUNCTION NAME : STARTCLEANCYCLE                                             ;
; FUNCTION      : STARTS THE CLEAN CYCLE ONLY AND SETS MODBUS TABLE DATA      ;
; INPUTS        : NONE                                                        ;                       ;
;*****************************************************************************;


STARTCLEANCYCLE:
        LDAA     #CLEANPHASE1ID  ; IS THIS PHASE 1
        STAA     CLEANCYCLESEQ
        RTS



SETMIXERTIME:
        RTS
        LDAA    BLENDERTYPE
        CMPA    #BABYTYPE
        BNE     NOTBBT          ; NO
        LDAB    MIXTIM          ; MIXING TIME BCD
        JSR     BCDHEX1         ; TO HEX
        LDAA    #10
        MUL                     ; MULTIPLY.
        STD     MIXERONCTR10HZ  ; COMPARE TO 10HZ COUNTER.
NOTBBT  RTS





*/
/*
NOTSTL  TST     FIRSTBATCH      ; IS THIS THE INITIAL BATCH ?
        BEQ     NOAUTOCYCLE     ;
not first batch
        LDAA    BATSEQ          ; CHECK SEQUENCER
        CMPA    #AUTOCYCLETARE  ; SHOULD WE TARE FOR AUTOCYCLER ?
        BNE     NOAUTOCYCLE     ; SKIP
// not autcycletare.
        LDD     DMPSEC          ;
        CPD     #AUTOCYCLEDELAY
        BLS     NOTAREYET
        JSR     NORMTARE
        LDAA    #$0F
        STAA    AUTOCYCLEFLAG
        LDAA    #LEVPOLL
        STAA    BATSEQ          ; SET LEVEL SENSOR POLLING.

NOTAREYET:
        RTS


NOAUTOCYCLE:

        TST     STOPTRAN
        BEQ     NOSTTR
        CLR     STOPTRAN
;        TST     PAUSFLG         ; IS THE BLENDER PAUSED ?
;        BEQ     ISPAUSD
ISPAUSD CLR     CYCLEINDICATE   ; RESET CYCLE INDICATE
        JSR     CLEARTHROUGHPUTTOTALS
        JSR     MAKECYC
        RTS

NOSTTR  TST     RUNTRAN
        BEQ     NOCHANGE
        TST     PAUSFLG         ; PAUSED ?
        BNE     NOCHANGE        ; SKIP THIS IF PAUSED.
        CLR     RUNTRAN
MAKEDUMP:
        CLRW    DMPSEC
        LDAA    #DUMP
        STAA    BATSEQ
        RTS


NOCHANGE LDAA    BATSEQ          ;


MAKECYC LDAA    #LEVPOLL
        STAA    BATSEQ
        JSR     CLOSEALL
        CLRW    CMPNTIM
        CLR     CMPNTIM+2
        JSR     TOMANUAL
        LDX     #SEQTABLE
        STX     SEQPOINTER      ; RESET TO START OF TABLE
        RTS

*/

/*

MCE12-15 ASM listing

CYCMON:
        LDAA    BATSEQ          ;
        CMPA    #BYPASSID
        BNE     NOTBYP          ; NOT BYPASS MODE.
        TST     BUBBRK     ;
        BEQ     ISOFF
        JSR     CLOSEBYPASS     ; CLOSE OFF BYPASS
        RTS
ISOFF   JSR     OPENBYPASS      ; OPEN BYPASS VALVE.
        RTS
NOTBYP  CMPA    #SETTLING       ; SETTLING
        BNE     NOTSTL          ; NO
        LDD     DMPSEC          ;
        TST     FASTFLG         ; CHECK MODE IN CALIBRATION DATA.
        BNE     SLOWMD          ; SLOW MODE.
        CPD     #FASTSETTLINGTIME   ; SETTLING TIME.
        BRA     CMPSET
SLOWMD  CPD     #SLOWSETTLINGTIME   ; SETTLING TIME.
CMPSET  BLS     NOTSTL
        JSR     RESETAVG        ; RESET AVERAGE.
        LDAA    #CALC           ; CALCULATE AVERAGE WHEN AVAILABLE.
        STAA    BATSEQ          ;
NOTSTL  TST     FIRSTBATCH      ; IS THIS THE INITIAL BATCH ?
        BEQ     NOAUTOCYCLE     ;
        LDAA    BATSEQ          ; CHECK SEQUENCER
        CMPA    #AUTOCYCLETARE  ; SHOULD WE TARE FOR AUTOCYCLER ?
        BNE     NOAUTOCYCLE     ; SKIP
        LDD     DMPSEC          ;
        CPD     #AUTOCYCLEDELAY
        BLS     NOTAREYET
        JSR     NORMTARE
        LDAA    #$0F
        STAA    AUTOCYCLEFLAG
        LDAA    #LEVPOLL
        STAA    BATSEQ          ; SET LEVEL SENSOR POLLING.

NOTAREYET:
        RTS

MAKECYC LDAA    #LEVPOLL
        STAA    BATSEQ
        JSR     CLOSEALL
        CLRW    CMPNTIM
        CLR     CMPNTIM+2
        JSR     TOMANUAL
        LDX     #SEQTABLE
        STX     SEQPOINTER      ; RESET TO START OF TABLE
        RTS


NOAUTOCYCLE:
        TST     STOPTRAN
        BEQ     NOSTTR
        CLR     STOPTRAN
        LDAA    BLENDERMODE
        CMPA    #SINGLERECIPEMODE ; NO EFFECT IF SINGLE RECIPE MODE
        BEQ     XITMC
        CLR     CYCLEINDICATE   ; RESET CYCLE INDICATE
        JSR     CLEARTHROUGHPUTTOTALS
        JSR     MAKECYC
XITMC   RTS

NOSTTR  TST     RUNTRAN
        BEQ     NOCHANGE
        LDAA    BLENDERMODE
        CMPA    #SINGLERECIPEMODE
        BEQ     ISSNRC          ; IS SINGLE RECIPE MODE

        LDAA    OPTIONCODE
        ORAA    OPTIONCODE+1
        ANDA    #BLENDERRUNOPTIONBIT  ; BLENDER RUN OPTION SET ?.
        BEQ     XITTC                 ;

        TST     PAUSFLG         ; PAUSED ?
        BNE     NOCHANGE        ; SKIP THIS IF PAUSED.
        CLR     RUNTRAN
MAKEDUMP:
        CLRW    DMPSEC
        LDAA    #DUMP
        STAA    BATSEQ
XITTC   RTS
ISSNRC  CLR     RUNTRAN
        BRA     XITTC



NOCHANGE LDAA    BATSEQ          ;
        CMPA    #LEVPOLL        ; IS IT TIME TO CHECK THE LEVEL SENSOR
        LBNE     NOSENSOR        ; SEQUENCE ERROR OCCURRED
        LDAA    CLEANCYCLESEQ
        CMPA    #CLEANPHASE2ID   ; IS THIS NOW CLEAN PHASE 2
        LBEQ     DOPHASE2CLEAN

        LDAA    BLENDERMODE     ; CHECK MODE
        CMPA    #OFFLINEMODE    ; OFFLINE MODE
        BNE     ONLI          ;
        TST     PAUSFLG
        LBNE     NOSENSOR

ONLI    TST     DMPLEVSEN       ; CHECK LEVEL SENSOR ACTIVE FLAG
        BNE     ISACTIVE        ; NOT ACTIVE
        JMP     NEXTIDCHK
XITAL   RTS

ISACTIVE:
        TST     STOPPED
        BNE     XITAL
        TST     WARMUPCTR             ; WARMING UP.
        BNE     XITAL                 ; EXIT IF WARMING UP.
        LDAA     OPTIONCODE
        ANDA    #BLENDERRUNOPTIONBIT  ; SHOULD THE BLENDER BE STOPPED?
        BNE     RUNBLND               ; ALLOW BLENDER TO RUN.
        LDAA     OPTIONCODE+1
        ANDA    #BLENDERRUNOPTIONBIT  ; TEMPORY OPTION ENABLED.
        BEQ     XITAL                 ;
RUNBLND CLR     DMPLEVSEN             ; CLEAR "DUMP LEVEL SENSOR" FLAG
        TST     BLOWEVERYCYCLE
        BEQ     ISCLCYC
        LDAA    CLEANCYCLESEQ   ; IS THIS A CLEAN CYCLE
        BNE     ISCLCYC         ; NOT RUN DURING CLEAN
        LDD     #10
        STD     CCLONCTR                 ; DITHO
        STD     CCRONCTR                 ; CORNER CLEANS ON.


ISCLCYC JSR     SETMIXERTIME    ; SET MIXER TIME FOR BLENDER

        JSR     CLOSEALL        ;
        LDD     CYCLETIME
        STD     CYCLETIMEPRN
        CLRW    CYCLETIME

        TST     FIRSTBATCH      ; IS THIS THE FIRST BATCH
        BNE     IS1STBATCH

        JSR     BATCHLENGTHCAL  ; CALCULATE LENGTH
        JSR     BATCHGPMCALC    ; GRAMS PER METER CALCULATED.
        BRA     SKIPTHIS
IS1STBATCH:
        JSR     PRINTDIAGHEADER
        JSR     BATCHLENGTHCAL  ; CALCULATE LENGTH

SKIPTHIS:

        JSR     CALHWTAV         ; WEIGHT CALCULATION
        LDX     EREG            ;
        STX     CURBATWGH       ; CURRENT BATCH WEIGHT
        LDX     EREG+2
        STX     CURBATWGH+2     ;
        TST     FIRSTWCYCLE     ; IS THIS THE FIRST WEIGH CYCLE ?
        BNE     YES1ST          ; YES

        JSR     CHECKHOPWT      ; CHECK HOPPER WEIGHT.


 YES1ST:
        LDAA    BLENDERMODE     ; CHECK MODE
        CMPA    #OFFLINEMODE    ; OFFLINE MODE
        LBEQ     OFFLINE         ; OFFLINE MODE.
; *TEST ONLY* *REVIEW*
;        JSR     OPENDUMP        ; DUMP MATERIAL
;        JSR     UPDATEBATCHVARIABLES

        JSR     CLOSEFEED       ; CLOSE PINCH VALVE IN THROAT.
        CLRW    DMPSEC          ; RESET 1/10 SECOND COUNTER.
        CLR     OFFLINE_TRK     ; RESET TRACKING FLAG.
        LDAA    #OFFDELBDMPID    ; "DELAY BEFORE DUMP ID
        STAA    BATSEQ          ; SET TRACKING FLAG
        RTS



UPDATEBATCHVARIABLES:
        JSR     BATSECAL
        TST     ESTCTRUSE       ; USE ESTIMATION COUNTER.
        BEQ     NORCALC         ; NORMAL CALCULATION.
        DEC     ESTCTRUSE
        LDAA    #1
        STAA    RUNEST          ; SIGNAL ESTIMATION TO RUN.
        BRA     NOCALC
NORCALC JSR     CALKGPHR

;        TST     MANATO          ; IF MANUAL DO NOT WORRY ABOUT CHECKING AUTO
;        BEQ     DOCALC
;        TST     CHECKLSENSORINNSECONDS
;        BEQ     DOCALC          ; DO CONTROL ACTION
;        TST     LEVSEN1STAT       ; LEVEL SENSOR STATUS
;        BEQ     DOCALC            ; SENSOR ON, IS OKAY
;        LDAA    #1
;        STAA    DISCONACTION      ; DISABLE CONTROL ACTION
;        BRA     NOCALC            ; SKIP CALC
        TST     DISCONACTION
        BNE     NOCALC             ; NO UPDATE IF CONTROL ACTION IS DISABLED
DOCALC  JSR     CALCDACPKG
NOCALC  LDD     CURBATWGH       ; READ CURRENT BATCH
        STD     PREVBATCHWEIGHT ;
        LDAA    CURBATWGH+2     ; CURRENT WEIGHT TO PREVIOUS BATCH FOR NEXT CYCLE
        STAA    PREVBATCHWEIGHT+2
        JSR     ACUMWT          ; ACCUMULATE ROLL AND ORDER WEIGHT
        JSR     CPYTWTMB
        TST     FIRSTBATCH      ;
        BNE     NOALGCALL       ; NO ALGORITHM CALL
        JSR     SETLEVELSENSORWDOG
        TST     DISCONACTION
        BNE     ACTIONDIS
        JSR     ALGOR1          ; CONTROL ALGORITHM CALLED
        JSR     CALC_ACTMIC     ; ACTUAL MICRONS CALCULATED.
        BRA     CONCYCLE
ACTIONDIS:
        CLR     DISCONACTION
CONCYCLE:
        JSR     PRINTPERERROR
        BRA     SKIPCLEAR
NOALGCALL:
        CLR     FIRSTBATCH      ; RESET
SKIPCLEAR:
        RTS




;
;       OFFLINE MODE OF OPERATION.

OFFLINE TST     PAUSFLG         ; IS BLENDE PAUSED ?
        BNE     XITOFF          ; DO NOT CLOSE PINCH VALVE.
        TST     BATCHREADY
        BNE     XITOFF          ; READY TO PAUSE
        JSR     CLOSEFEED       ; CLOSE PINCH VALVE IN THROAT.
        JSR     BATSECAL
        JSR     CALKGPHR
        LDAA    OFFLINETYPE             ;CHECK OFFLINE TYPE.
        CMPA    #OFFLINETYPEHILO        ; BLEND TO XX KGS.
        BNE     NOCMR                   ; NO  CMR
        JSR     CALCCMR         ; UPDATE D/A DEPENDING ON ACTUAL KG/HR
NOCMR   LDD     CURBATWGH       ; READ CURRENT BATCH
;        STD     PREVBATCHWEIGHT ;
;        LDAA    CURBATWGH+2     ; CURRENT WEIGHT TO PREVIOUS BATCH FOR NEXT CYCLE
;        STAA    PREVBATCHWEIGHT+2
;        JSR     ACUMWT          ; ACCUMULATE ROLL AND ORDER WEIGHT
;        JSR     CPYTWTMB
        CLRW    DMPSEC          ; RESET 1/10 SECOND COUNTER.
        CLR     OFFLINE_TRK     ; RESET TRACKING FLAG.
        LDAA    #OFFDELBDMPID    ; "DELAY BEFORE DUMP ID
        STAA    BATSEQ          ; SET TRACKING FLAG
XITOFF  RTS

NOSENSOR:
        LDAA    BLENDERMODE     ; CHECK MODE
        CMPA    #OFFLINEMODE    ; OFFLINE MODE
        BNE     ONLINE          ;
        TST     OFFLSF          ; OFFLINE LEVEL SENSOR FLAG.
        BEQ     ONLINE          ;
        CLR     OFFLSF          ; RESET FLAG.
        TST     BATCHREADY
        BNE     XITOF1          ; READY TO PAUSE
        TST     PAUSFLG         ; IS BLENDER PAUSED ?
        BNE     XITOF1          ; DO NOT CLOSE PINCH VALVE.
        JSR     CLOSEFEED       ;
        LDAA    #OFFDELADMPID  ; DELAY #2 NOW
        STAA    OFFLINE_TRK     ; SET SEQUENCE.
        CLR     OFFTIM1         ; OFF LINE COUNTER RESET
        TST     PAUSFLG         ; IS BLENDER PAUSED ?
        BNE     XITOF1          ; DO NOT CLOSE PINCH VALVE.
ONLINE  LDAA    BATSEQ
        CMPA    #OFFDELBDMPID  ;"OFFLINE DELAY BEFORE DUMP
        BNE     NEXTIDCHK
        LDD     DMPSEC          ; 1/10 SECOND COUNTER.
        CPD    #OFFDELAY1      ; OFFLINE DELAY #1
        BLS     XITOF1
        JSR     OPENDUMP        ; RELEASE MATERIAL.
        LDAA    #DUMP
        STAA    BATSEQ         ; NOW TARE THE HOPPER AS NORMAL.
        JSR     STARTOFFLINEDELAY
        JSR     UPDATEBATCHVARIABLES
XITOF1:
        RTS


STARTOFFLINEDELAY:
        JSR     CLOSEFEED       ;
        LDAA    #OFFDELADMPID  ; DELAY #2 NOW
        STAA    OFFLINE_TRK     ; SET SEQUENCE.
        CLR     OFFTIM1         ; OFF LINE COUNTER RESET
        CLRW    DMPSEC
        RTS


NEXTIDCHK:
        LDAA    OFFLINE_TRK
        CMPA    #OFFDELADMPID   ; SEQUENCE AFTER DUMP.
        BNE     NEXTIDCHK1
        LDAA    OFFTIM1         ; OFF
        LDAB    MIXTIM          ; MIXING TIME BCD
        JSR     BCDHEX1         ; TO HEX
        CBA
        BNE     NEXTIDCHK1      ; CHECK NEXT ID ANYWAY
;        JSR     OPENFEED        ; OPEN PINCH VALVE
        JSR     CHECKFORVACSEQ

; *REVIEW* SHOULE THE FOLLOWING CODE RUN WHEN THE BLENDER IS IN SINGLE RECIPE MODE

        LDAA    #TEN
        STAA    CHECKLSENSORINNSECONDS
        TST     BATCHREADY
        BEQ     NOTRDY          ; NO
        JSR     STARTUPON
        CLR     BATCHREADY      ; RESET BATCH READY SIGNAL
NOTRDY  CLR     OFFLINE_TRK     ; RESET TRACKING FLAG.


NEXTIDCHK1:
        LDAA    BLENDERTYPE
        CMPA    #TINYTYPE       ; TINY TYPE.
        BNE     NOTMXOFF        ; NO

;       LDAA    #1
;       STAA     MIXOFF           ; MIXER OFF.
NOTMXOFF:
        LDAA    BATSEQ          ; SEQUENCE COUNTER
        CMPA    #DUMP           ; HAS THE DUMPING SEQUENCE BEING STARTED.
        BNE     NOTDMPS         ; NO
        TST     AUTOCYCLEFLAG   ; AUTO CYCLE ENABLED ?
        BNE     CHECKWT         ; SKIP MIN CHECK IF AUTOCYCLER.

        TST     STOPPED         ; STOPPED ?
        BNE     CONDMP1
CHECKWT JSR     CALHWT          ; CALCULATE WEIGHT
        LDD     EREG            ;
        CPD     #MINWEIGHT      ; CHECK IF BELOW 100 GRAMS ?
        BHI     CONDMP          ; CONTINUE TO DUMP MATERIAL
;        LDAA    MBPROALRM       ;
;        ANDA    #TAREALBIT      ; TARE ALARM BIT
;        BEQ     NOALTR          ; NO TRANSITION.
;        LDAA    #TAREALARM      ;
;        JSR     RMALTB          ; INDICATE ALARM.
NOALTR  LDAA    #MATERIALFALL         ;
        STAA    BATSEQ          ; MATERIAL HAS FALLEN WAIT N SECONDS
        CLRW    DMPSEC          ; BEFORE CLOSING.
        JSR     NEEDFORTOUP
        RTS
CONDMP  JSR     OPENDUMP
        LDD     DMPSEC          ;
        CPD     #MXTARTIM       ; MAX TARE TIME.
        BLS     OKFONOW         ; OKAY
        CLRW    DMPSEC          ; COUNTER.
        LDAA    MBPROALRM       ;
        ANDA    #TAREALBIT      ; TARE ALARM BIT
        BNE     NOALRP
        LDAA    #TAREALARM      ;
        JSR     PTALTB          ; INDICATE ALARM.

NOALRP  TST     LEVSEN1STAT     ; CHECK LEVEL SENSOR
        BNE     NOALTR          ; LEVEL SENSOR UNCOVERED
OKFONOW RTS

CONDMP1 JSR     OPENDUMP
        RTS

NOTDMPS CMPA    #MATERIALFALL   ; IS MATERIAL STILL FALLING ?
        BNE     NOTMATF         ;  NO
        LDD     DMPSEC          ; READ SECONDS COUNTER
        CPD    #MATFALLTIME    ; TIME BEFORE TARE.
        BLS     NOTALLFALL      ; YES

;        LDAA    BLENDERMODE
;        CMPA    #SINGLERECIPEMODE   ; IF SINGLE RECIPE MODE -> CLEANING EVERY CYCLE.
;        LBEQ     ISPHASE1CLEAN       ; YES DO CLEAN
        LDAA    CLEANCYCLESEQ       ; IS THIS A CLEAN CYCLE
        CMPA    #CLEANPHASE1ID  ; IS THIS PHASE 1
        LBEQ     ISPHASE1CLEAN   ; IS PHASE 1 CLEAN
        JSR     CLOSEDUMP       ; CLOSE FLAP
        TST     DOUBLEDUMP      ; IS DOUBLE DUMP REQUIRED
        BEQ     TARASN          ; TARE AS NORMAL
        LDAA    #DOUBLEDUMPCLOSEID
        BRA     STRACD          ; STORE AND CLEAR COUNTER

TARASN  LDAA    #TAREID         ;
STRACD  STAA    BATSEQ          ; TAREING IS NEXT IN THE SEQUENCE
        CLRW    DMPSEC          ; RESET DUMP SECONDS COUNTER.
NOTALLFALL:
        RTS


NOTMATF LDAA    BATSEQ          ;
        CMPA    #DOUBLEDUMPCLOSEID; MATERIAL DUMPED ?
        BNE     NOTDD           ; NO
        LDD     DMPSEC          ; READ SECONDS COUNTER
        CPD     #DDCLOSETIME     ; DOUBLE DUMP CLOSE TIME EXCEEDED.
        BLS     NONDDT          ; NO
        JSR     OPENDUMP        ; OPEN DUMP VALVE
        LDAA    #DOUBLEDUMPOPENID;
        STAA    BATSEQ
        CLRW    DMPSEC
NONDDT  RTS


NOTDD   LDAA    BATSEQ          ;
        CMPA    #DOUBLEDUMPOPENID; DOUBLE DUMP OPEN
        BNE     NOTDD1          ; NO
        LDD     DMPSEC          ; READ SECONDS COUNTER
        CPD     #DDOPENTIME     ; DOUBLE DUMP OPEN TIME EXCEEDED.
        BLS     NONDDOT         ; NO
        JSR     CLOSEDUMP       ; CLOSE FLAP
        LDAA    #TAREID         ;
        STAA    BATSEQ          ; TAREING IS NEXT IN THE SEQUENCE
        CLRW    DMPSEC          ; RESET DUMP SECONDS COUNTER.
NONDDOT RTS


NOTDD1  LDAA    BATSEQ          ;
        CMPA    #TAREID         ; MATERIAL DUMPED ?
        BNE     NOTTAR          ; NO
        LDD     DMPSEC          ; READ SECONDS COUNTER
        CPD    #TIMEBTARE      ; TIME BEFORE TARE.
        BLS     NONATCT         ; YES
        JSR     TAREND          ; TARE HOPPER
        LDAA    #$0F
        STAA    TARINGSTATUS    ; INDICATE TARING STATUS
        LDAA    BLENDERTYPE
        CMPA    #TINYTYPE       ; TINY TYPE.
        BNE     NOTTINY          ; NO
        LDAA    #MIXINGID       ; MIXING PERIOD.
        STAA    BATSEQ          ;
        CLR     MIXOFF
        CLRW    DMPSEC          ; RESET COUNTER
NONATCT RTS


NOTTAR  LDAA    BATSEQ          ;
        CMPA    #MIXINGID       ; MIXING
        BNE     NOTMIX          ; NO.
        LDAB    INTMIXTIME       ; MIXING TIME BCD
        JSR     BCDHEX1         ; TO HEX
        LDAA    #10
        MUL                     ; MULTIPLY.
        CPD     DMPSEC          ; COMPARE TO 10HZ COUNTER.
        BHI     NONATMX         ; YES
        LDAA    #1
        STAA     MIXOFF           ; MIXNER OFF.
NOTTINY LDAA    #AFTERTAREID    ; LOAD VACUUM OFF ID
        STAA    BATSEQ          ;
        CLRW    DMPSEC          ; RESET COUNTER
NONATMX RTS



NOTMIX  LDAA    BATSEQ          ;
        CMPA    #AFTERTAREID       ;
        LBNE     NOTVACDEL       ;
        LDAA    BLENDERTYPE     ; CHECK THE BLENDER TYPE.
        CMPA    #TINYTYPE       ; MICRA  ?
        BNE     INNRDL          ; INSTIGATE NORMAL DELAY
        LDD     DMPSEC          ;
        CPD     #MICRAAFTERTARETIME  ;
        LBLS     NONATCT1        ;
        BRA     CONMCD          ; CONTINUE MICRA DELAY
INNRDL  LDD     DMPSEC          ;
        CPD     #AFTERTARETIME  ;
        LBLS     NONATCT1        ;
CONMCD  CLR     TARINGSTATUS    ; INDICATE TARING STATUS
NONEAVAIL:
        CLR     FILLCOUNTER
        CLR     FILLALARM       ; RESET COMP FILL ALARM
        JSR     CLEARUNUSED
        CLR     FILLCTR
        JSR     RESETFILLCOUNTERS
        JSR     CHECKLLS
        CLR     ANYFILLALARM    ; RESET FILL ALARM STATUS
        CLR     LEVELSENALOC    ; LEVEL SENSOR ALARM OCC FLAG.
        CLR     RETRYCOMPNOHIS

        TST     ISVOLMODE       ; MODE ENABLED ?
        BRA     NOVOL           ; NO VOLUMETRIC MODE ON 1ST BATCH.
;        BEQ     NOVOL           ; NO VOLUMETRIC MODE ON 1ST BATCH.


        TST     FIRSTWCYCLE     ; FIRST CYCLE NO
        BNE     NOVOL           ; NO VOLUMETRIC MODE ON 1ST BATCH.
        TST     PCTHIS          ; HAS RECIPE BEING UPDATED ?
        BNE     RECCHG          ; YES
        TST     ANYFILLALARM    ; ANY ALARMS IN PREVIOUS CYCLE.
        BNE     NOVOL
        INC     VOLCYCCTR       ; INCREMENT VOLUMETRIC COUNTER.
        LDAA    VOLCYCCTR       ;
        CMPA    #2
        BNE     NOVOL
        CLR     VOLCYCCTR
        LDAA    #VOLMODE
        STAA    BATSEQ          ; ENABLE VOLUMETRIC MODE.
        STAA    CYCLEINDICATE   ; FILLLING IN PROGRESS
        RTS

RECCHG  CLR     PCTHIS          ; RESET FLAG.
NOVOL:
        TST     PAUSFLG         ; IN PAUSE MODE ?
        BNE     NONATCT1        ; PAUSED

        TST     TOPUPF           ; TOP UP ?
        BEQ     NOTUP           ; NO
        LDAA    #FILLFIRST
        STAA    TOPUPSEQUENCING ;
NOTUP   LDAA    #FILL               ;
        STAA    BATSEQ          ; SET BATCH SEQUENCE TO START AT COMP#1
        STAA    CYCLEINDICATE   ; FILLLING IN PROGRESS
        JSR     RESETFILLCOUNTERS
        JSR     CHKFORCLEANPHASE2 ; CHECK FOR CLEAN PHASE 2 CYCLE


NONATCT1:
        RTS
NOTVACDEL:
        CMPA    #VACLOADING     ; VACUUM LOADING.
        BNE     NOTVACLOAD      ; NO
        LDD     DMPSEC          ; READ 1/10 SECONDS COUNTER.
        CPD     #VACDELAY       ;
        BLS     NOTATVACD       ; NOT AT VAC DELAY.
        CLR     FILLCOUNTER
        TST     PAUSFLG         ; IN PAUSE MODE ?
        BNE     NOTATVACD       ; PAUSED
        LDAA    #FILL           ; REFILL SAME COMPONENT AGAIN.
        STAA    BATSEQ          ;
        JSR     RESETFILLCOUNTERS
        JSR     CHKFORCLEANPHASE2 ; CHECK FOR CLEAN PHASE 2 CYCLE
NOTATVACD:
        RTS

NOTVACLOAD:
        CMPA    #WAITFORTRAN    ; "WAITING FOR LS TRANSITION.
        BNE     NOTWFT
        LDX     SEQPOINTER
        LDAB    0,X
        ANDB    #$0F
        LDX     #LS1ONTR        ; TRANSITION OFF TO ON.
        DECB
        ABX
        TST     0,X             ;
        BEQ     XITWFT          ; EXIT WAIT FOR TRANSITION.
        CLR     0,X             ; RESET TRANSITION FLAG.
        CLRW    DMPSEC
        LDAA    #DELAYBFILL
        STAA    BATSEQ          ; SET SEQUENCER ID.
XITWFT  RTS

;NOTWFT  CMPA    #DELAYBFILL     ; DELAY BEFORE FILLING.
;        BNE     NOTDFF          ; NO
;        LDD     DMPSEC
;        CPD     #200
;        BLS     NOTDFF          ;
;        CLR     FILLCTR
;        CLR     FILLCOUNTER
;        CLR     LEVELSENALOC    ; LEVEL SENSOR ALARM OCC FLAG.
;        TST     PAUSFLG         ; IN PAUSE MODE ?
;        BNE     NOTDFF          ; PAUSED
;        LDAA    #FILL               ;
;        STAA    BATSEQ          ; SET BATCH SEQUENCE TO START AT COMP#1
;        STAA    CYCLEINDICATE   ; FILLLING IN PROGRESS
;NOTDFF  RTS


CHKFILLBYW:
        CLRA                    ; NO
        RTS



NOTWFT  CMPA    #DELAYBFILL     ; DELAY BEFORE FILLING.
        BNE     CHKCLC          ; CHECK CLEAN CYCLE
        LDD     DMPSEC
        CPD     #200
        BLS     NOTDFF          ;
        CLR     FILLCTR
        CLR     FILLCOUNTER
        CLR     LEVELSENALOC    ; LEVEL SENSOR ALARM OCC FLAG.
        TST     PAUSFLG         ; IN PAUSE MODE ?
        BNE     NOTDFF          ; PAUSED
        LDAA    #FILL               ;
        STAA    BATSEQ          ; SET BATCH SEQUENCE TO START AT COMP#1
        STAA    CYCLEINDICATE   ; FILLLING IN PROGRESS
        JSR     RESETFILLCOUNTERS
        JSR     CHKFORCLEANPHASE2 ; CHECK FOR CLEAN PHASE 2 CYCLE
NOTDFF  RTS

;CHKFILLBYW:
        CLRA                    ; NO
        RTS


;  CLEANING PHASE 1
;
ISPHASE1CLEAN:
        TST     MOULDING                ; MOULDING MODE
        BEQ     NORCLN                  ; NO NORMAL CLEAN

        LDAA    #CLOSEDUMP1ID           ;
        STAA    BATSEQ                  ; TAREING IS NEXT IN THE SEQUENCE
        CLRW    DMPSEC                  ; READ 1/10 SECONDS COUNTER.
        BRA     STANXI                  ; EXIT.

NORCLN  CLR    BCLEANREPETITIONSCTR    ; COUNTER INITIATED
        CLR     MCLEANREPETITIONSCTR    ; COUNTER INITIATED
        LDAA   #1
        STAA    DUMPREPETITIONS
        CLRA
        LDAB    PH1ONTIME
        JSR     BCDHEX1
        STD     AIRJET1ONCTR            ; AIR JET 1 COUNTER ONE TIME
        LDAA    #ACTIVATINGJET1ID
        STAA    BATSEQ                  ; STORE SEQUENCING.
STANXI  RTS


;       JET ONE ON
CHKCLC:
        LDAA    BATSEQ                  ; SEQUENCE.
        CMPA    #ACTIVATINGJET1ID       ; ACTIVATING JET 1
        BNE     CHKFRJ1OFF              ;  JET 1 OFF
        LDD     AIRJET1ONCTR            ; READ SECONDS COUNTER
        BNE     STBLJ1                  ; STILL BLOWING JET 1
        LDAA    #AIRJET1OFF1ID
        STAA    BATSEQ                  ; STORE SEQUENCING.
        CLRW    DMPSEC                  ; READ 1/10 SECONDS COUNTER.
STBLJ1  RTS

;       JET ONE OFF
;       INITITATE REPETITIONS

CHKFRJ1OFF:
        LDAA    BATSEQ                  ; SEQUENCE.
        CMPA    #AIRJET1OFF1ID
        BNE     CHKFRDM
        CLRA
        LDAB    PH1OFFTIME
        JSR     BCDHEX1
        CPD     DMPSEC
        BHI     LVJ1OFF                 ; LEAVE JET 1 OFF
        INC     BCLEANREPETITIONSCTR    ; COUNTER INITIATED
        LDAA    BCLEANREPETITIONSCTR
        CMPA    BCLEANREPETITIONS
        BHI     BCLFIN
        BEQ     BCLFIN

        LDAA    #ACTIVATINGJET1ID
        STAA    BATSEQ                  ; STORE SEQUENCING.
        CLRA
        LDAB    PH1ONTIME
        JSR     BCDHEX1
        STD     AIRJET1ONCTR            ; AIR JET 1 COUNTER ONE TIME
LVJ1OFF RTS
BCLFIN:
        CLR     BCLEANREPETITIONSCTR    ; =0
        LDAA    #CLOSEDUMP1ID           ;
        STAA    BATSEQ                  ; TAREING IS NEXT IN THE SEQUENCE
        CLRW    DMPSEC                  ; READ 1/10 SECONDS COUNTER.
        BRA     LVJ1OFF

;       CLOSE DUMP

CHKFRDM LDAA    BATSEQ                  ; SEQUENCE.
        CMPA    #CLOSEDUMP1ID           ; DUMP 1 ?
        BNE     CHKOD1                  ; CHECK NEXT.
        LDD      DMPSEC                  ; DUMP SECONDS
        CPD      #CLOSEDUMP1TIME10S      ; CLOSE DUMP 1 TIME IN 10TH SECOND
        BLS      XITCD1          ;
        JSR      CLOSEDUMP                ; OPEN DUMP FLAP
        LDAA     #OPENDUMP1ID           ;
        STAA     BATSEQ                  ; TAREING IS NEXT IN THE SEQUENCE
        CLRW     DMPSEC
XITCD1  RTS

;       OPEN DUMP

CHKOD1  LDAA    BATSEQ                  ; SEQUENCE.
        CMPA    #OPENDUMP1ID            ; DUMP 1 ?
        BNE     CHKCD2                  ; CHECK NEXT.
        LDD      DMPSEC                 ; DUMP SECONDS
        CPD      #OPENDUMP1TIME10S      ; CLOSE DUMP 1 TIME IN 10TH SECOND
        BLS      XITOD2                 ;
        JSR      OPENDUMP               ; OPEN DUMP FLAP
        LDAA     #CLOSEDUMP2ID           ;
        STAA     BATSEQ                  ; TAREING IS NEXT IN THE SEQUENCE
        CLRW     DMPSEC
XITOD2  RTS


;       CLOSEDUMP

CHKCD2 LDAA    BATSEQ                  ; SEQUENCE.
       CMPA    #CLOSEDUMP2ID           ; DUMP 1 ?
       BNE     CHKOD2                  ; CHECK NEXT.
       LDD      DMPSEC                  ; DUMP SECONDS
       CPD      #CLOSEDUMP1TIME10S      ; CLOSE DUMP 1 TIME IN 10TH SECOND
       BLS      XITCD2          ;
       JSR      CLOSEDUMP                ; OPEN DUMP FLAP
       TST      DUMPREPETITIONS
       BEQ      NOREP
       DEC      DUMPREPETITIONS
       LDAA     #OPENDUMP1ID           ;
       BRA      SETSEQ

NOREP  LDAA     #SETTLEAFTERBANGID
SETSEQ STAA     BATSEQ
       CLRW     DMPSEC
XITCD2 RTS

CHKOD2  LDAA    BATSEQ                   ; SEQUENCE.
        CMPA    #SETTLEAFTERBANGID       ; DUMP 1 ?
        BNE     CHKCC                    ; CHECK FOR CLEAN PHASE 2
        LDD     DMPSEC                  ; DUMP SECONDS
        CPD     #SETTLEAFTERBANGTIME10S ; CLOSE DUMP 1 TIME IN 10TH SECOND
        BLO     XITSAB                  ; EXIT
        LDAA    #1
        STAA    PHASEONEFINISHED        ; HAS CLEAN CYCLE #1 FINISHED ?

        TST     RECIPEWAITING            ; IS THERE A RECIPE WAITING ?
        BEQ     NOBTF                    ; NO BATCH FILL
        CLR     RECIPEWAITING            ; RESET RECIPE FLAG
        LDAA    #1
        STAA    BATCHHASFILLED
        JSR     LOADPERTARGETSFROMCOMMS
        BRA     TARASN1                  ; CONTINUE

; TEST ONLY *

NORCW   TST     NOFILLONCLEAN           ; NO FILL ON CLEAN CYCLE.
        BEQ     TARASN1                 ; TARE AS NORMAL.

NOBTF   CLR     BATCHHASFILLED
        CLR     PHASEONEFINISHED ; RESET
        LDAA    #CLEANPHASE2ID  ; CLEAN PHASE 2 CYCLE.
        STAA     CLEANCYCLESEQ
        LDAA    #LEVPOLL                 ;
        STAA    BATSEQ                  ; TAREING IS NEXT IN THE SEQUENCE
        BRA     XITSAB

TARASN1 LDAA    #TAREID                 ;
        STAA    BATSEQ                  ; TAREING IS NEXT IN THE SEQUENCE
        CLRW    DMPSEC                  ; RESET DUMP SECONDS COUNTER.
XITSAB  RTS

;       AIR JET 2 BEING ACTIVATED.
;

CHKCC   LDAA    BATSEQ                  ;
        CMPA    #CCLACTIVEID
        BNE     CHCCLOFF                  ;
        LDD     CCLONCTR             ; CC LEFT ON
        BNE     XITCCL                  ; NO
        LDAA    #CCLOFFID             ;      ;
        STAA    BATSEQ                  ;
        CLRW    DMPSEC                  ; READ 1/10 SECONDS COUNTER.
XITCCL  RTS


CHCCLOFF LDAA    BATSEQ                  ; SEQUENCE.
        CMPA    #CCLOFFID               ; JET 2 OFF TIME
        BNE     CHCRON                  ;  NO
        JSR     CORNERCLEANLEFTOFF
        CLRA
        LDAB    PH2OFFTIME
        JSR     BCDHEX1
        CPD     DMPSEC
        BHI     CCLOFF               ; LEAVE JET 1 OFF
        LDAA    #CCRACTIVEID
        STAA    BATSEQ                  ;
        CLRA
        LDAB    PH2ONTIME
        JSR     BCDHEX1
        STD     CCRONCTR                 ; AIR JET 1 COUNTER ONE TIME
        CLR     DMPSEC
CCLOFF  RTS

CHCRON  LDAA    BATSEQ                  ;
        CMPA    #CCRACTIVEID
        BNE     CHCCROFF                  ;
        LDD     CCRONCTR                ; CC LEFT ON
        BNE     XITCL                   ; NO
        LDAA    #CCROFFID             ;      ;
        STAA    BATSEQ                  ;
        CLRW    DMPSEC                  ; READ 1/10 SECONDS COUNTER.
XITCL   RTS

CHCCROFF LDAA    BATSEQ                  ; SEQUENCE.
        CMPA    #CCROFFID               ; JET 2 OFF TIME
        BNE     CHCLP2                  ;  NO
        JSR     CORNERCLEANRIGHTOFF
        CLRA
        LDAB    PH2OFFTIME
        JSR     BCDHEX1
        CPD     DMPSEC
        BHI     CCLFOFF               ; LEAVE JET 1 OFF

        INC     MCLEANREPETITIONSCTR    ; COUNTER INITIATED
        LDAA    MCLEANREPETITIONSCTR
        CMPA    MCLEANREPETITIONS
        BEQ     FINCC
        BHI     FINCC
        LDAA    #CCLACTIVEID
        STAA    BATSEQ                  ;; REPEAT CORNER CLEAN
        CLRA
        LDAB    PH2ONTIME
        JSR     BCDHEX1
        STD     CCLONCTR                 ;
        CLR     DMPSEC
        BRA     CCLFOFF


FINCC   CLR     MCLEANREPETITIONSCTR
        CLRA
        LDAB    PH2ONTIME
        JSR     BCDHEX1
        STD     AIRJET2ONCTR            ; AIR JET 2 COUNTER ON TIME
        LDAA    #ACTIVATINGJET2ID
        STAA    BATSEQ                  ; STORE SEQUENCING.
CCLFOFF RTS



CHCLP2  LDAA    BATSEQ                  ; SEQUENCE.
        CMPA    #ACTIVATINGJET2ID       ; ACTIVATING JET 2
        BNE     AJ2OFF                  ;  NO
        LDD     AIRJET2ONCTR            ; READ SECONDS COUNTER
        BNE     STBLJ2                  ; STILL BLOWING JET 2
        LDAA    #AIRJET2OFFID             ;      ;
        STAA    BATSEQ                  ;
        CLRW    DMPSEC                  ; READ 1/10 SECONDS COUNTER.
STBLJ2  RTS

;       AIR JET 2 OFF
;
AJ2OFF  LDAA    BATSEQ                  ; SEQUENCE.
        CMPA    #AIRJET2OFFID           ; JET 2 OFF TIME
        LBNE     LVJ2OFF                 ;  NO
;        JSR     CORNERCLEANON
        CLRA
        LDAB    PH2OFFTIME
        JSR     BCDHEX1
        CPD     DMPSEC
        LBHI     LVJ2OFF                 ; LEAVE JET 1 OFF

        INC     MCLEANREPETITIONSCTR    ; COUNTER INITIATED
        LDAA    MCLEANREPETITIONSCTR
        CMPA    MCLEANREPETITIONS
        BEQ     FINCLC
        BHI     FINCLC
        CLRA
        LDAB    PH2ONTIME
        JSR     BCDHEX1
        STD     AIRJET2ONCTR            ; AIR JET 1 COUNTER ONE TIME
        JSR     CORNERCLEANLEFTOFF
        JSR     CORNERCLEANRIGHTOFF
        LDAA    #ACTIVATINGJET2ID
        STAA    BATSEQ                  ; STORE SEQUENCING.
        BRA     LVJ2OFF

FINCLC  CLR     MCLEANREPETITIONSCTR    ; COUNTER = 0
        JSR     CORNERCLEANLEFTOFF
        JSR     CORNERCLEANRIGHTOFF
        CLR     CLEANCYCLESEQ           ; RESET CLEANING CYCLE
        JSR     SETENDCLEANMBBIT
        LDD     #MBCSTATCLEANST2POS
        COMD
        ANDA    MBOPSTS
        STD     MBOPSTS
        LDAA    BLENDERMODE             ; CHECK BLENDER MODE
        CMPA    #SINGLERECIPEMODE       ; SINGLE RECIPE MODE ?
        BNE     NOSRC                   ; NO
        CLR     OFFLINEVALVEISOPEN

; IF A BATCH IS WAITING THEN WAIT UNTIL VACUUM IS OFF AND FINISHED THE CURRENT BATCH.

        TST     BATCHHASFILLED          ; HAS BATCH FILLED ?
        BEQ     PSBLN                   ; NO PAUSE BLENDER
        LDAA    #1
        STAA    WAITFORVACUUMTOFINISH
PSBLN:
;   JSR     PAUSEON
        JSR     VACINHIBITON
; *REVIEW* RESET VACUUM STATUS
;       CLRW    MBVACUUMSTATUS           ; RESET VACUUM STATUS
;        CLR     BLENDERINHIBIT          ; ALLOW NEW ORDER IN.




; *REVIEW* NOT THAT THE RECIPE SHOULD BE ALLOW IN EARLIER


; FILL NEW BATCH IF READY.


NOSRC   TST     NOFILLONCLEAN
        BEQ     NOFILC                  ; NO FILL ON CLEAN
        CLR     NOFILLONCLEAN
        JSR     PAUSEON                 ; PAUSE BLENDER
        CLR     NOFILLONCLEAN           ; EXIT.
NOFILC  LDAA    #LEVPOLL           ;
        STAA    BATSEQ                  ; TAREING IS NEXT IN THE SEQUENCE
        CLRW    DMPSEC                  ; READ 1/10 SECONDS COUNTER.
LVJ2OFF RTS



NOTDLAC RTS
CHKNXTS RTS


DOPHASE2CLEAN:

; PHASE 2 CLEAN, IGNORE LEVEL SENSOR
; CHECK FOR BYPASS SENSOR OFF HIGH LEVEL SENSOR
; ACTIVATE AIR JETS 2
; SET BATSEQ TO LEVPOLL
;

; *REVIEW*
        LDAA    BLENDERMODE
        CMPA    #SINGLERECIPEMODE
        BNE     NOOP             ; NORMAL CLEAN OPERATION OPEN OFFLINE
        TST     OFFLINEVALVEISOPEN  ; HAS THE MATERIAL BEEN RELEASED INTO THE BIN ?
        BEQ     XBYPONN
NOOP    JSR     OPENFEED         ; OPEN FEED VALVE
PROCLN  TST      HIGHLEVELSENSOR ; LEVEL SENSOR 1 STATUS
        BNE      XBYPONN                 ; BYPASS SENSOR IS ON
        LDD     #MBCSTATCLEANST2POS
        ORD     MBOPSTS
        STD     MBOPSTS

        LDD     #AIRJET1ONTIMEPH210S       ; AIR JET 1 ON IN 1/10 OF SECOND


; START PAHSE 2

        LDAA    BLENDERTYPE             ;
        CMPA    #BABYTYPE               ; BABY TYPE BLENDER
        LBEQ     FINCC                  ; SKIP CORNER CLEANING AND GO TO MIXER CLEAN

        LDAA    #CCLACTIVEID
        STAA    BATSEQ                  ;; INITIATE CORNER CLEAN
        CLRA
        LDAB    PH2ONTIME
        JSR     BCDHEX1
        STD     CCLONCTR                 ; AIR JET 1 COUNTER ONE TIME
        CLR     DMPSEC


;        LDAB    PH2ONTIME
;        JSR     BCDHEX1
;        STD     AIRJET2ONCTR            ; AIR JET 1 COUNTER ONE TIME
;        JSR     CORNERCLEANOFF
;        LDAA    #ACTIVATINGJET2ID
;        STAA    BATSEQ                  ; STORE SEQUENCING.
XBYPONN RTS



; CHECK FOR CLEAN PHASE 2 REQUIREMENT.

CHKFORCLEANPHASE2:
        LDAA    PHASEONEFINISHED ; HAS CLEAN CYCLE #1 FINISHED ?
        BEQ     XITCC            ; YES, IT HAS FINISHED
        CLR     PHASEONEFINISHED ; RESET
        LDAA    #CLEANPHASE2ID  ; CLEAN PHASE 2 CYCLE.
        STAA     CLEANCYCLESEQ

XITCC   RTS
NOTPH1  CLR     CLEANCYCLESEQ
        BRA     XITCC           ; EXIT CLEAN CHECK


;*****************************************************************************;
; FUNCTION NAME : INITITATECLEANCYCLE                                         ;
; FUNCTION      : INIITATE THE CLEAN CYCLE                                    ;
; INPUTS        : NONE                                                        ;                       ;
;*****************************************************************************;


INITIATECLEANCYCLE:
        CLR      OVERRIDECLEAN
        JSR      RESETCLEANFINISHBIT
        JSR      SETSTARTCLEANMBBIT
        TST      CYCLEINDICATE   ; ARE WE IN A CYCLE
        BEQ      NOCCYLE          ; NO
        LDAA     #1
        STAA     SETCLEANWHENCYCLEEND
        BRA      CLNXIT          ; DISPLAY PAGE
NOCCYLE JSR      STARTCLEANCYCLE
CLNXIT  RTS


;*****************************************************************************;
; FUNCTION NAME : STARTCLEANCYCLE                                             ;
; FUNCTION      : STARTS THE CLEAN CYCLE ONLY AND SETS MODBUS TABLE DATA      ;
; INPUTS        : NONE                                                        ;                       ;
;*****************************************************************************;


STARTCLEANCYCLE:
        LDAA     #CLEANPHASE1ID  ; IS THIS PHASE 1
        STAA     CLEANCYCLESEQ
        RTS



SETMIXERTIME:
        LDAA    BLENDERTYPE
        CMPA    #BABYTYPE
        BNE     NOTBBT          ; NO
        LDAB    INTMIXTIME      ; MIXING TIME BCD
        JSR     BCDHEX1         ; TO HEX
        LDAA    #10
        MUL                     ; MULTIPLY.
        STD     MIXERONCTR10HZ  ; COMPARE TO 10HZ COUNTER.
NOTBBT  RTS


CHECKFORSINGLERECIPE:
        LDAA    BLENDERMODE
        CMPA    #SINGLERECIPEMODE
        BNE     XITSRC          ; NO

; --testonly-- for now

        TST     CLEANCYCLESEQ   ;       IS CLEANING IN PROGRESS
        BNE     ISCLN           ;       NO NEED TO
        JSR     INITIATECLEANCYCLE

ISCLN   TST     CLEANONNEXTCYCLE
        BEQ     XITSRC                  ; NO CLEANING REQUIRED
        CLR     CLEANONNEXTCYCLE
        JSR     INITIATECLEANCYCLE
XITSRC  RTS


CHECKOFFTIME:
        JSR    READTIME                 ;READ CURRENT TIME
        LDX     #TIMEBUF
        LDAA    DATE,X
        CMPA    PDNDOM     ;~DATE
        BNE     SETWRM      ; SET WARMUP
        LDAA    MON,X
        CMPA    PDNMON     ;~DATE
        BNE     SETWRM      ; SET WARMUP


        LDAB    PDNHRS
        JSR     BCDHEX1
        LDAA    #60
        MUL
        PSHM    D
        LDAB    PDNMIN
        JSR     BCDHEX1
        CLRA
        PULM    E
        ADE
        PSHM    E



        LDAB    HOUR,X
        JSR     BCDHEX1
        LDAA    #60
        MUL
        PSHM    D
        LDAB    MIN,X
        JSR     BCDHEX1
        CLRA
        PULM    E
        ADE
        PULM    D
        SDE

        CPE     #60
        BHI     SETWRM
        CPE    #5
        BHI     SETWRML   ;
        CLR     WARMUPCTR
        BRA     XITWUC

SETWRM  LDAA    #120
        STAA    WARMUPCTR
        BRA     XITWUC

SETWRML LDAA    #30
        STAA    WARMUPCTR
XITWUC  RTS


*/

