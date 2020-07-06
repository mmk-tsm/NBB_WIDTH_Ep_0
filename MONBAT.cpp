//////////////////////////////////////////////////////
// MonBat.c
//
// Foreground program called when component has filled and settled.
//
//
// M.McKiernan                          19-07-2004
// First Pass
// M.McKiernan                          03-09-2004
// Routines CheckForSkippedComponent and CheckForRetries added.  SetToIgnoreCPI expanded inline
// M.McKiernan                          08-09-2004
// Removed the disabling of g_bActivateLatency in StartFillSequence at L. 595.
//
// M.McKiernan                          10-09-2004
// In CalculateHopperWeightAvg - Weight returned in g_fAvgHopperWeight, used to be in g_fWeightInHopper
// Edited CalculateComponentTargetN see 10.09.2004
// Edited StartFillSequence - if(g_cRetryActive || !g_CalibrationData.m_nStageFillEn),
// Quits if StageFill not enabled,  (was opposite).
//
// M.McKiernan                          14-09-2004
// Added CalculateWeightAfterRegrind();
// Pick up regrind comp. from recipe structure, not calibration data. - in CalculateActualComponentPercentages()
// M.McKiernan                          15-09-2004
// CalculateActualComponentPercentages() -% for regrind component based on full hopper weight, others based on hopper wt. - regrind.
// M.McKiernan                          17-09-2004
// Call StoreStageFillData()
// Call CheckForStagedFill()
// P.Smith                              27-09-2004
// Corrected read of component no details for MonitorBatch when in top up mode
// and not normaloperation, fill colour or top up component.
// Call to CalculateColourCompensation.
// M.McKiernan                          28-09-2004
// Added FindColourComponent(), Call to FindColourComponent.
// M.McKiernan                          03-11-2004
// CopyActualPercentagesToMB() activated.
//
// M.McKiernan                          19-11-2004
// Replaced g_bPauseFlag by g_nPauseFlag - more than 1 type of pause.
// Removed unused variables: fCounts, *nRetryPointer, *bHasRetriedPointer;
// Renamed g_bPToNormal to g_bPauseToNormal.
// NeedForTopUp edited - code added 18.11.2004 to cater for transition Topup -> Normal.
// CheckForEndOfCyclePause() called in EndCycle().
// ImmediatePauseOn() called in CompAgain().
//
// M.McKiernan                          26-11-2004
// PAUSEATENDOFCYCLE has been renamed to END_OF_CYCLE_PAUSE_TYPE
//
// M.McKiernan                          13-12-2004
// Activated PutAlarmTable & RemoveAlarmTable()
//
// M.McKiernan                          16-12-2004
// Added PrintCycleDiagnostics
//
// P.Smith                              4-08-2005
// Remove % data based on RR data
//
//
// P.Smith                              17-08-2005
// decrement of g_nNoLevelSensorAlarmCounter at end of cycle.
//
// P.Smith                              12/9/2005
// call CheckForSingleRecipeClean
//
// P.Smith                              29/09/05
// No level sensor alarm if single recipe mode.
// decrement g_bAllowLargeAlarmBand if > 0
//
//
// P.Smith                              30/09/05
// check for regrind bit in Comp1stxTotWgt
// modify CalculateComponentTargetN to check regrind and layering.
// Move CheckForEndOfCyclePause
// move g_nCycleCounter at multiple filling flag check.
// call CalculateActualPercentages at same place
//
// P.Smith                              4/10/05
// Rename low priority todos

// P.Smith                          20/10/05
// rename  g_bAllowLargeAlarmBand -> g_nAllowLargeAlarmBand
//
// P.Smith                          24/10/05
// when picking up 1st component for percentage calculation
// Name change g_bAllowLargeAlarmBand to g_nAllowLargeAlarmBand
// Correct regrind bit check in Comp1stxTotWgt
// Add check for regrind in CalculateActualComponentPercentages
//
// P.Smith                          16/11/05
// check for g_CalibrationData.m_bVolumetricModeFlag in CalcCompNTime
//
// P.Smith                          22/11/05
// call CopyComponentWeightsToMB at then end of the cycle.
//
//
// P.Smith                          22/11/05
// added check for g_CalibrationData.m_bFirstComponentCompensation
//
// P.Smith                          3/1/06
// Remove todolp BBDIAPRN already completed.
//
// P.Smith                          10/1/06
// removed cData = g_cSeqTable[g_nSeqTableIndex];  // ?? not used ??  --REVIEW--
// Bool bWentToNextComp = FALSE removed is unused
// make bQuit volatile Bool
// if(g_cSeqTable[0] & REGRIND_BIT == 0) changed to if((g_cSeqTable[0] & REGRIND_BIT) == 0) // regrind ?
// removed lTime = g_lCmpTime[nCompIndex] in CalcCompNTime
//
// P.Smith                      23/2/06
// first pass at netburner hardware conversion.
//#include <basictypes.h>
///
// P.Smith                      24/2/06
// bResult = CheckFillAccuracy();  // Check fill accuracy.
//#include "ChkFill.h"
// StoreStageFillData( nComp );
// #include "Fillrep.h"
// bUseRR = CalculateCPIRoundRobin( (int)g_cCompNo );  // CALCCPIRROBIN calculate CPI based on RR data.
// #include "CPIRRobin.h"
//
// P.Smith                      24/2/06
// nStage = GetFillingStage( (int)g_cCompNo );
//
// P.Smith                      1/6/06
// correct warning unsigned int i;
//
// P.Smith                      29/6/06
// call CheckForSingleRecipeClean();
//#include "CycleMonitor.h"
//
// P.Smith                      30/6/06
// call CalculateValveLatency()
// store actual weights in top up mode so that they can be printed out
//
// P.Smith                      11/7/06
// call PrintCycleDiagnostics
//#include "PrntDiag.h"
//
// P.Smith                      14/7/06
// remove level sensor delay at the end of the cycle.
// This is also done on the 50hz program as well and is a duplication.
//
// P.Smith                      17/11/06
// name change g_nCycleIndicate -> g_bCycleIndicate
// Remove reference to level sensor alarm.
//
// P.Smith                      28/2/07
// use added weights to calculate the component percentages rather     ;
// than the actual hopper weight.
//
// P.Smith                      2/3/07
// ensure that the on time is calculated eventhough the first batch    ;
// component on time is being calculated.                              ;
//
// P.Smith                      5/3/07
// Call SaveFlowRatesToNonVolatileMemory & GenerateFlowRateChecksum
//
// P.Smith                      20/6/07
// added set of g_bSaveAllCalibrationToEEpromOutsideCycle in SaveFlowRatesToNonVolatileMemory
//
// P.Smith                      21/6/07
//  g_CalibrationData.m_nCycleCounter incremented, now in calibration data
//  set g_bSaveAllCalibrationToEEpromOutsideCycle to save data outside the blender cycle.
//
//
// M.McKiernan                      17/9/07
// Removed g_arrnMBTable[] & g_arrnMBTableFlags[], now in BatVars2
//
// P.Smith                          28/9/07
// Correct component percentage calculation.
//
// P.Smith                          18/12/07
// call ResetComponentWeightRoundRobin & CalculateComponentRoundRobinWeight when retry.
// this resets that averaging for the component weights.
// CalculateComponentRoundRobinWeight
// CheckComponentPercentages called
// implement CalculateActualComponentPercentagesRR to calculate round robin component %
//
// P.Smith                          28/1/08
// BatchStatisticalAnalysis called at the end of the cycle to calculate the standard deviation.
//
//
// P.Smith                          30/1/08
// correct compiler warnings
//
// P.Smith                          7/3/08
// correct flow rate checksum check, if was checking for g_bRedirectStdioToPort2
// in this function which would stop the clearance of the flow rate from working.
//
// P.Smith                          27/3/08
// if g_ProcessData.m_nCycleCounter is divisible by SAVETOEEPROMCYCLENO, set
// g_bAllowProcesDataSaveToEEPROM to allow save of process data to eeprom.
// if g_nSaveProcessDataToEEPROMCounter non zero decrement and set g_bAllowProcesDataSaveToEEPROM
// in SaveFlowRatesToNonVolatileMemory use g_ProcessData structure for flow rates and
// set g_bSaveAllProcessDataOutsideCycle to save process data
// use g_ProcessData in GenerateFlowRateChecksum & CheckFlowRateChecksum
//
// P.Smith                          23/4/08
// cycle counter renamed to g_nCycleCounter
//
// P.Smith                          1/5/08
// Stephen has seen a problem with the percentages on the blender.
// if regrind is used, the blender is not showing the correct component percentages.
// CalculateActualComponentPercentages & CalculateActualComponentPercentagesRR are modified to
// divide by the total weight g_fWeightInHopper when regrind component is calculated and
// divide by  g_fWeightInHopperMinusRegrind when calculating normal components
//
// P.Smith                          2/5/08
// Peter has seen a problem with the blender in that the actual component
// percentages on the screen do not add to 100%, and the logged file is different.
// this is due to the fact that the actual component percentage is being used in the log
// file and the actual averaged percentage weight is being returned to the panel.
// also they do not add up to 100% due to the fact that the component weight is being divided by
// the batch weigth. it is modified to divide by the total averaged component weight rather
// than the current total batch weight.
//
// P.Smith                          17/5/08
// there is a problem with the retries in that if averaging is selected, ie
// sensitivity set to high, the blender will reset the average and calculate the component
// weight, but if the material comes to the component the next time, the blender
// will use the average of say 0 for the previous retry and 60 for the sucessful fill
// and get 30 as the result. Therefore it is necessary to reset the round robin average
// again after the weight has been calculated.
//
// P.Smith                          10/6/08
// another problem has been seen in offline mode where the dump flap is not closing in offline
// mode, this is due to the blender being paused between dump and tare.
// the blender when unpaused does not close the dump flap.
// to correct this the dump flap is closed before filling starts.
// in CalculateCPI if the flow rate is 0, then revert to fill by weight and reset
// averaging.
//
// P.Smith                          18/6/08
// In CompAgain, check for g_CalibrationData.m_bVacLoading,set g_cBatSeq to BATSEQ_VACLOADING
//
// P.Smith                          23/7/08
// remove g_arrnMBTable extern
//
// P.Smith                          14/8/08
// a problem has been seen where the top up mode does not work when first component
// compensation is disabled. The reason for this is that the top up is not checked
// in CalculateComponentTargetN when fcc is disabled, this is modified to check
// top up at this point.
// a problem has been seen where when the recipe is changed from 85,10,5 in top up
// mode back to 95,0,5, the last table pointer is not set up correctly.
// this is dues to the fact that the function FindColourComponent is not operating
// correctly. if there are 2 delimiters in the sequence table, the blender will pick
// up the second one and set the last table pointer to the incorrect value.
// the software then takes the delimiter as the component and the sequence gets screwed
// up and top up no longer works
// the solution is to exit the for loop when the first delimiter is found by setting
// bExit to TRUE.
//
// P.Smith                          11/9/08
// if top up is enabled, ensure that the retry counter is cleared if it has not retried.
// Remove vacuumloading from this stage of the filling process, it will now be added
// to the settling time.
//
// P.Smith                          26/9/08
// EstimateLiquidComponentWeight is called to estimate the weight of the liquid component.
// change the addition to add up all the component weights eventhough it is not in the sequence table.
// this will allow the component % to be calculated correctly even if it is not in the sequence
// table.
//
// P.Smith                          3/10/08
// set g_fOneSecondHopperWeightAtEndOfCycle to g_fOneSecondHopperWeight at end of cycle
//
// P.Smith                          14/10/08
// correct compiler warning, removed unused variable
//
// P.Smith                          14/11/08
// remove g_nCycleCounter increment, this is now done when the batch is dropped
//
// P.Smith                          3/12/08
// increment ongoing counter for retry counters
//
// P.Smith                          8/1/09
// problem with leak alarm check in fast mode.
// the one second average has not updated properly when the reading is taken at
// this means that it shows a leak alarm.
// the end of the cycle.this is corrected by taking the average reading at the end of
// the cycle, ie the last reading used in filling the last component.
// dumpevent called to dump data to the usb.
//
// P.Smith                          6/2/09
// add check for non zero regrind component before actually using the regrind weight
// for the regrind component.
//
// P.Smith                          5/3/09
// name change m_nComponentRetryCtr, removed history from strucure element
//
// P.Smith                          27/3/09
// call SaveOrderWeightToNonVolatileMemory
//
// P.Smith                          1/5/09
// call CalculateSeconds when unit is in volumetric mode to calculate the opening times
// for the valves
//
// P.Smith                          22/7/09
// added pusling stuff.
// name change to fillerr
// in CalculateComponentTargets, calculate 0.9 of target for pulsed component.
// added PulsingTargetReached
//
// P.Smith                          25/8/09
// added check for MAXIMUM_COMPONENT_PULSES if the number of pulses has exceeded
// the max, then revert to old filling sequence and stop calculation of flow rate.
// this had been causing a problem in Rithai when the blender got to the max pulses.
// call CheckForValidFlowRate to ensure that the blender does not get an invalid flow
// rate.
//
// P.Smith                          7/9/09
// copy load time to g_fCycleLoadTimePrn, this is now counted on the pit for greter accuracy.
// calculate load time from g_lLoadTime which is incremented on the pit.
// add in latency time for each component plus one second.
//
// P.Smith                          15/9/09
// allow pulsing to be calibrated on any component.
// add component no to telnet info
// if pulsing is disabled make sure the clear g_nPulsingSequence
// removed PulsingTargetReached from this file
//
// P.Smith                          17/9/09
// if volumetric mode is checked for, it only does not update the time
// if the diagnostics password is enabled.
//
// P.Smith                          28/9/09
// remove fill by weight activation if flow rate is zero.
// the old flow rate is now used instead.
// added check for min open time of 5 pits.
//
// P.Smith                          9/10/09
// modified CalculateComponentTargetN to correct the layering option
// so that it works correctly and resembles the assembler code.
//
// P.Smith                          17/10/09
// added set g_nAfterEndOfCycleCounter to ENDOFCYCLETIMEINSECONDS at end of cycle.
//
// P.Smith                          16/11/09
// copied settled reading to buffer so that it can be printed out in the log.
// set g_nDelayBeforeLoadCellLogCtr to initiate log of a/d value after n seconds
//
// P.Smith                          19/11/09
// call LoadCellAToDStabilityCheck at end of cycle
//
// P.Smith                          27/11/09
// added TOPUPCOMPONENT instead of specific component no.
//
// P.Smith                          7/12/09
// if in top up mode, do not set g_bFstCompCalFin, this is not needed in top up
// mode of operation.
//
// P.Smith                          8/12/09
// problem has been seen if the blender is in top up mode and regrind is used.
// if the there is no master batch, then the blender stops the cycle.
// this is corrected by checking for the regrind component use in GoToNextComp
//
// P.Smith                          9/12/09
// modified gotonextcomponent, if 1 component, no top up, if regrind and one component
// then no top up, read table index as normal.
//
// P.Smith                          12/2/10
// only call CalculateComponentRoundRobinWeight to calculate averaged weight if no
// multistage filling.
//
// P.Smith                          12/3/10
// at the moment when the blender is changed to top up mode, the first cycle top up
// is not instigated.
// this was due to the fact that it could have been doing a fill by weight.
// remove check for first cycle in CalculateComponentTargetN.
// this allows the top up mode to work the very first time.
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
#include "MonBat.h"
#include "MBProgs.h"
#include "SetpointFormat.h"
#include "ChkFill.h"
#include "CPIRRobin.h"
#include "BBCalc.h"
#include "Fillrep.h"
#include "Pause.h"
#include "PrntDiag.h"
#include "Alarms.h"
#include "Batvars2.h"
#include <basictypes.h>
#include "Csumcalc.h"
#include "WeightRR.h"
#include "BatchStatistics.h"
#include "LiquidAdditive.h"
#include "Historylog.h"
#include "Acumwt.h"
#include "PulseValve.h"


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
extern  structCPIRRData     g_WorkCPIRRData;

//////////////////////////////////////////////////////
// MonitorBatch( void )         from ASM = MONBAT
//
// Monitors the batching process - forground program.
// This routine has been broken up into individual functions from the orig ASM
// Stores weights, and calculates times etc, and handles sequencing.
//
// M.McKiernan                          09-07-2004
// First pass.
//////////////////////////////////////////////////////
void MonitorBatch( void )
{
int nComp;
BOOL bWentToNoStorD = FALSE;
BOOL bWentToCompAgain  = FALSE;

BOOL bFillError;
unsigned char cData;

    if(g_cBatSeq == BATSEQ_CALC)
    {
        if(g_bWeightAvgAvail)   // if average ready??
        {
            g_bWeightAvgAvail = FALSE; //reset average flag for next pass.
            if(g_bTopUpFlag)
            {
                if(g_nTopUpSequencing == FILLCOLOUR) // fill colur??
                {
                    //ISCOLOR
                    cData = g_cSeqTable[g_nLastCompTableIndex];  //LASTCOMPTABLEP
                }
                else if(g_nTopUpSequencing == TOPUPCOLOUR)  // topup colour??
                { // NOTNRL
                    if(g_bNoTopUpRequired)
                    {
                        cData = g_cSeqTable[g_nSeqTableIndex];  // ?? not used ??  --REVIEW--
                        g_cCompNoDetails = cData;
                        g_cCompNo = (cData & 0x0F); // isolate component no.
                        //--REVIEW--  (CompNo was not set up in assembler, which compno should be used??)
                        NoStorD();
                        bWentToNoStorD = TRUE;
                    }
                    else
                    {
//                        cData = g_cSeqTable[g_nSeqTableIndex];  // ?? not used ??  --REVIEW--
                        cData = TOPUPCOMPONENT;
                        g_cCompNoDetails = cData;
                        g_cCompNo = (cData & 0x0F); // isolate component no.
                    }
                }
                else if(g_nTopUpSequencing == NORMALOPERATION)  // topup sequencing = normal??
                {
                    cData = g_cSeqTable[g_nSeqTableIndex];
                    g_cCompNoDetails = cData;
                    g_cCompNo = (cData & 0x0F); // isolate component no.
                        NoStorD();
                        bWentToNoStorD = TRUE;
                }
                else
                {

// P.Smith - added transfer of component no details if not normal operation
                        cData = g_cSeqTable[g_nSeqTableIndex];
                        g_cCompNoDetails = cData;
                        g_cCompNo = (cData & 0x0F); // isolate component no.
                }

            }
            // topup flag not set.
            else  //RDTBASN
            {
                    cData = g_cSeqTable[g_nSeqTableIndex];  // read table as normal
                    g_cCompNoDetails = cData;
                g_cCompNo = (cData & 0x0F); // isolate component no.

            }

            //STRCMP
            if(!bWentToNoStorD)  // i.e. skip this if bWentToNoStorD set (go to NOSTORD)
            {
            g_cCompNoDetails = cData;
            g_cCompNo = (cData & 0x0F); // isolate component no.

                // STORNCT - store and settle count.
                StoreEndCounts();
                CalculateSeconds(); // calculate opening time in seconds for each comp.
                if(g_nPulsingSequence[g_cCompNo-1] == VALVE_PULSING_STAGE)
                {
                    bWentToCompAgain = TRUE;
                   // if(fdTelnet>0)
                   // iprintf("\n pulsing valve now on comp %d g_nPulsingSequence[g_cCompNo-1] is %d",g_cCompNo,g_nPulsingSequence[g_cCompNo-1]);
                    if(PulsingTargetReached(g_cCompNo))
                    {
                        //if(fdTelnet >0)
                        //iprintf("\n pulsing at target");
                        g_lCurrentLCAtDTare = g_lCurrentLCAtDValue;  // store current a/d as tare for next comp.
                        bWentToCompAgain = FALSE;  // allow it to move on
                        g_bNoFlowRateCalculation = TRUE;  // no flow rate calc
                        g_bFastSettling = FALSE;  // stop fast cycle

                    }
                    else
                    {
                        //if(fdTelnet >0)
                        //iprintf("\n pulsing target not reached");
                        g_lCmpTime[g_cCompNo-1] = g_nPulseTime;//


                        g_nComponentPulseCounter[g_cCompNo-1]++;  // increment pulse counter
                        if(g_nComponentPulseCounter[g_cCompNo-1] > MAXIMUM_COMPONENT_PULSES)
                        {
                            g_nPulsingSequence[g_cCompNo-1] = 0;  // revert to old type filling
                            g_bCalibratePulsing[g_cCompNo-1] = TRUE;
                            g_bNoFlowRateCalculationArray[g_cCompNo-1] = TRUE;

                            if(fdTelnet > 0 && g_nTSMDebug.m_bPulsingDebug)
                            iprintf("\n component %d max pulses exceeded",g_cCompNo);
                        }
                        StartFillSequence(); // JSR CHECKLLS (exit to fill mode)

                    }
                }
                else if(g_CalibrationData.m_bVolumetricModeFlag) //ISVOLMODE
                {  //VOLM
                    // StoreComponentTare()
                    g_lCurrentLCAtDTare = g_lCurrentLCAtDValue;  // store current a/d as tare for next comp.
                    CalculateSeconds(); // calculate opening time in seconds for each comp.
                }
                else  // not volumetric
                {
                    if(g_bMoveOn)
                    {
                        g_bMoveOn = FALSE;
                        // StoreComponentTare()
                        g_lCurrentLCAtDTare = g_lCurrentLCAtDValue;  // store current a/d as tare for next comp.
                    }
                    else   //MoveOn false (0)
                    {
                        //CHECK
                        CalculateSeconds(); // calculate opening time in seconds for each comp.
                        bFillError = CheckFillAccuracy();  // Check fill accuracy.
                        if(bFillError)  // return value from ChkFillAcc()
                        {
                            //FILLERR
                            // B = CompNO
                            // A = 2
                          ResetComponentWeightRoundRobin(g_cCompNo, 2 ); // asm RESETWGTRROBIN
                          CalculateComponentRoundRobinWeight(g_cCompNo); // asm CALCRROBINFORWEIGHT
                          ResetComponentWeightRoundRobin(g_cCompNo, 2 ); // asm RESETWGTRROBIN
                            if(!g_bChangingOrder)
                            {
                                g_sOngoingHistoryComponentLogData.m_nComponentRetryCtr[g_cCompNo-1]++;
                                CompAgain();    // call COMPAGAIN as function.
                                bWentToCompAgain = TRUE;  // indicate bypassed other sections.
                            }
                        }
                        else   // no fill error
                        {
                            if(g_nPulsingSequence[g_cCompNo-1] == PRE_PULSING_STAGE)
                            {
                                //if(fdTelnet >0)
                                //iprintf("\n pre pulsing has finished");
                                bWentToCompAgain = TRUE;
                                g_nPulsingSequence[g_cCompNo-1] = VALVE_PULSING_STAGE;
                                g_bFastSettling = TRUE;
                                g_bFirstPulse = TRUE;

                                // reload the original target
                                CalculateCPI( g_cCompNo );   // calculate flow rate
                                g_lComponentTargetCounts[g_cCompNo-1] = g_lComponentTargetCountsCopy[g_cCompNo-1];
                                g_fComponentTargetWeight[g_cCompNo-1] = g_lComponentTargetCounts[g_cCompNo-1] / g_CalibrationData.m_fWeightConstant;
                                g_nComponentPulseCounter[g_cCompNo-1] = 0;
                            }
                            else  // do not tare -- staying on same component
                            // StoreComponentTare()
                            g_lCurrentLCAtDTare = g_lCurrentLCAtDValue;  // store current a/d as tare for next comp.
                        }
                    }

                }  // end of "not volumetric"
                // CALCNOW
                if(!bWentToCompAgain)  // run this section only if had not gone to COMPAGAIN.
                {
                    g_nCorrectRetries = 0;
                    nComp = (int)g_cCompNo;
                    CalculateCPI( nComp );      // CALCNTPI calculate count per interrupt for comp n

                    if(g_CalibrationData.m_nStageFillEn && (g_CalibrationData.m_lManagerPassWord == DIAGNOSTICS_PASSWORD))
                    {
                        CalculateActualPercentages();// calculate component % asm BPCNTNCAL
                        PrintCycleDiagnostics();    //  BBDiaPrn();
                    }
                    // NOTPRI
                    StoreStageFillData( nComp );
                    if(g_bMultipleFilling)
                    {
                        GoToNextComp();
                    }
                    else
                        NoStorD();  //NoStorD section of MONBAT.
                }
            }
            // NOSTORD in assembler
//          if(bGoToNextComp)
//              GoToNextComp();
//          else if(bGoToCompAgain)
//              CompAgain();
//          else



        } // end of g_bWeightAvgAvail
    }  // end of BatSeq == BATSEQ_CALC

    //NOTCALC  --REVIEW-- (SHOULD GO IN SEPERATE SWITCH STATEMENT FOR g_cBatSeq)
    else if(g_cBatSeq == BATSEQ_PAUSEBFILL)     // OTHER SWITCH CASE??
    {
        if(!g_nPauseFlag && g_bPauseToNormal)
        {
            g_bPauseToNormal = FALSE;   // clear "back to normal" flag
            StartFillSequence();    // JSR CHECKLLS (exit pause to fill mode)
            g_bFillAlarm = FALSE;   // reset comp. fill alarm
            ResetFillCounters();        // reset fill counters
            g_nFillCtr = 0;
            g_nFillCounter = 0;
        }

    }

} // end of MonitorBatch

//////////////////////////////////////////////////////
// NoStorD( void )          from ASM = NOSTORD. (Part of MONBAT )
//
//
//
// M.McKiernan                          09-07-2004
// First pass.
//////////////////////////////////////////////////////
void NoStorD( void )
{
unsigned char cTemp;

    if(g_cCalcRouter == CALCMPNTAR) //Calculate rest of targets ?
    {
        CalcRest();
    }
    else if(g_cCalcRouter != CALC1STCMPTAR) // 1st comp calculation required??
        ChkCPI();
    else
    {
        CalculateWeightAfterRegrind();      //CALCWGTAFTREG -  weight after regrind and next target
        cTemp = g_cSeqTable[g_nSeqTableIndex];
        g_cCompNoDetails = cTemp;
        g_cCompNo = (cTemp & 0x0F); //mask off, comp. number.

        CalcCompNTime( (int)g_cCompNo );    //Calculate component #n updated time
        if(!g_bTopUpFlag)
        {
            g_bFstCompCalFin = TRUE;    //Indicate that this is finished.
        }
        GoToNextComp();
    }
}

//////////////////////////////////////////////////////
// CalcRest( void )         from ASM = CALCREST. (Part of MONBAT )
//
// CALCULATE CMP2,,,,N
//
// M.McKiernan                          12-07-2004
// First pass.
//////////////////////////////////////////////////////
void CalcRest( void )
{
    CalcCompNTime( (int)g_cCompNo );    //Calculate component #n updated time
    if(!g_bTopUpFlag)
        NotTup1();  // no top up.
    else
    {
        if(g_nTopUpSequencing == FILLFIRST) // First comp. filled?
        {
            if(g_cSeqTable[g_nSeqTableIndex + 1] == SEQDELIMITER)  //ONLY ONE COMPONENT??
            {
                EndCycle();
            }
            else
            {
                g_nTopUpSequencing = FILLCOLOUR;
                //if(fdTelnet > 0)
                //iprintf("\n calculating comp targets");
                CalculateComponentTargets();
                FindColourComponent();  //Set colour component pointer.
                IncToNx();
            }
        }
        else    // NO1STF
        {
            if(g_nTopUpSequencing == FILLCOLOUR)     // Is the colour being filled ??
            {
                CalculateColourCompensation();  //Calculate top up.
                if(!g_bNoTopUpRequired)
                    IncToNx();                  // top up as normal.
                else
                    ;       // XITCYC ???
            }
            else    //NOTCLF
            {
                if(g_nTopUpSequencing == TOPUPCOLOUR)     // Has top up taken place ??
                {
                    g_nTopUpSequencing = NORMALOPERATION;   // Indicate normal operation.
                    // check if last component??
                    if(g_cSeqTable[g_nSeqTableIndex + 2] == SEQDELIMITER)  //ONLY ONE COMPONENT??
                        {
                        EndCycle();
                        }
                    else
                        ;           // XITCYC     nbb--todo-- check this
                }
                else
                {
                    NotTup1();
                }
            }
        }
    }
}


//////////////////////////////////////////////////////
// NotTup1( void )          from ASM = NOTTUP1. (Part of MONBAT )
//
//
//
// M.McKiernan                          12-07-2004
// First pass.
//////////////////////////////////////////////////////
void NotTup1( void )
{
BOOL bWentToNx = FALSE;

    CalculateComponentTargets();
    if(g_nTopUpSequencing == NORMALOPERATION)   // is this normal operation?
    {
        g_nTopUpSequencing = 0;
    }
    else
    // ASM = FILLNOW
    {
        if(g_bTopUpFlag && (g_nTopUpSequencing != NORMALOPERATION))     // top-up enabled?
        {
            IncToNx();
            bWentToNx = TRUE;
            // --REVIEW-- was a PULM in asm, no corresponding push.
        }
    }
    //NOTUP
    if(!bWentToNx)  // this runs only if hadn't run IncToNx.
    {
        // last component??
        if(g_cSeqTable[g_nSeqTableIndex + 1] == SEQDELIMITER)
            EndCycle();  //end cycle.
        else
        {
            g_nSeqTableIndex++;     // update sequence table pointer(index)
            g_nFillCounter = 0;     // fill counter = 0
            g_bFillAlarm = FALSE;   // Reset comp fill alarm
            ResetFillCounters();
            g_nFillCtr = 0;         //
            if(g_nPauseFlag)    //Paused??
            {
                // not paused.
                g_cBatSeq = BATSEQ_PAUSEBFILL;  // -> Pause before fill
            }
            else
            {
                // paused.
                StartFillSequence();    //ASM = CHECKLLS
            }
        }
    }
}


//////////////////////////////////////////////////////
// EndCycle( void )         from ASM = ENDCYCLE. (Part of MONBAT )
// Ends batch cycle
//
//
// M.McKiernan                          12-07-2004
// First pass.
//////////////////////////////////////////////////////
void EndCycle( void )
{
    int    i;
    float fAdditionalTime;
    unsigned char cData;
    CalculateValveLatency();
    CheckForRetries();          // CHKFORRETRIES check for retries
    g_nSeqTableIndex = 0;   // reset to start of SeqTable.
    LoadCellAToDStabilityCheck();
    if(!g_bLevelSensorAlarmOccurred) // level sensor alarm?
    {  // no L.S. alarm
            // L.S. alarm bit set in Modbus??
            if( g_arrnMBTable[BATCH_SUMMARY_PROCESS_ALARMS] & LEVELSENALBIT )
                RemoveAlarmTable( LEVELSENSORALARM,  0 );       // indicate Lev. sensor alarm cleared, Component no = 0 i.e. not component alarm.
    }

    //NOLSAL
    // not paused & pause transition & covered??
    if(!g_nPauseFlag && g_bPauseTransition && (g_bDmpLevSen == FALSE ))
    {
        g_bPauseTransition = FALSE;  // clear Pause Transition??
    }

    //ISPAUSED
    //not paused & Throughput monitor & covered??
    // --todolp--     --REVIEW---
    //CONCYC
    g_cBatSeq = BATSEQ_LEVPOLL; // sequence to level sensor polling
    g_bCycleIndicate = FALSE;
    g_bFirstWCycle = FALSE;
    g_bChangingOrder = FALSE;
    g_fCycleLoadTimePrn = float(g_lLoadTime) / PIT_FREQUENCY;
    i=0;
    while(g_cSeqTable[i] != SEQDELIMITER && i < MAX_COMPONENTS) // SEQDELIMITER indicates table end
    {
        i++;
    }
    fAdditionalTime = float((i) * 0.07)+ 1.0;
    g_fCycleLoadTimePrn = g_fCycleLoadTimePrn + fAdditionalTime;

    CheckForStagedFill();
    // multiple filling
    CheckForEndOfCyclePause();          // check for endo of cycle pause

    if(!g_bMultipleFilling)
    {   //no

        CheckForSingleRecipeClean(); //ASM  CHECKFORSINGLERECIPE
        //g_nCycleCounter++;      // indicate cycle has finished.
        g_bSaveAllProcessDataOutsideCycle = TRUE;
        if((g_nCycleCounter % SAVETOEEPROMCYCLENO) == 0)
        {
            g_bAllowProcesDataSaveToEEPROM = TRUE;
        }
        else
        {
            g_bAllowProcesDataSaveToEEPROM = FALSE;
            if(g_nSaveProcessDataToEEPROMCounter != 0)
            {
                g_nSaveProcessDataToEEPROMCounter--;
                g_bAllowProcesDataSaveToEEPROM = TRUE;
            }
        }



    i = 0;
    while(g_cSeqTable[i] != SEQDELIMITER && i < MAX_COMPONENTS) // SEQDELIMITER indicates table end
    {    //asm = RESETRECIPECHANGEDBIT
         cData = RECIPECHANGEDBIT ^ 0xFF;    // Complement of bit
         g_cSeqTable[i] &= cData;
         i++;
    }




        if(g_nNoLevelSensorAlarmCounter > 0)
        {
            g_nNoLevelSensorAlarmCounter--;
        }
        if(g_nAllowLargeAlarmBand > 0)
        {
            g_nAllowLargeAlarmBand--;
        }

        CalculateActualPercentages();
        CalculateHopperWeightAvg();

        g_fOneSecondHopperWeightAtEndOfCycle = g_fAvgHopperWeight;
        g_nAfterEndOfCycleCounter = ENDOFCYCLETIMEINSECONDS;
        BatchStatisticalAnalysis();
        SaveOrderWeightToNonVolatileMemory();
        CopyComponentWeightsToMB();

        g_nPrintInNSeconds = 2;
    }

}

//////////////////////////////////////////////////////
// ChkCPI( void )           from ASM = CHKCPI. (Part of MONBAT )
//
//
//
// M.McKiernan                          12-07-2004
// First pass.
//////////////////////////////////////////////////////
void ChkCPI( void )
{
    if(g_nSeqTableIndex == 0)  // Is this the beginning of the table. ?
    {
        // Is there only 1 component??
        if(g_cSeqTable[g_nSeqTableIndex + 1] != SEQDELIMITER)
        {
            CalcNextTargets();  // > 1 comp, Calculate next target
        }
    }
    GoToNextComp();
}


//////////////////////////////////////////////////////
// GoToNextComp( void )         from ASM = GOTONEXTCOMP. (Part of MONBAT )
// Advance to next component - checking for Top-up mode.
// In Top Up mode, assumes last component has already been filled, and ends cycle.
//
// M.McKiernan                          09-07-2004
// First pass.
//////////////////////////////////////////////////////
void GoToNextComp( void )
{
    unsigned char cByte;
   BOOL bTopupRequired = TRUE;;
   g_nSeqTableIndex++; // SEQPOINTER + 1

// cases where top up is not required
   // one component, no regrind
   if((g_cSeqTable[1] == SEQDELIMITER) && (g_CurrentRecipe.m_nRegrindComponent == 0))
   bTopupRequired = FALSE;

   // 2 component, one is regrind.
   else if((g_cSeqTable[2] == SEQDELIMITER) && (g_CurrentRecipe.m_nRegrindComponent != 0))
   bTopupRequired = FALSE;

    if(g_bTopUpFlag && bTopupRequired)        // top-up enabled?
        cByte = g_cSeqTable[g_nSeqTableIndex + 1];
    else
        cByte = g_cSeqTable[g_nSeqTableIndex];

    if(cByte == SEQDELIMITER)  // last component??
    {
        EndCycle();
    }
    else
    {
        CheckForSkippedComponent();
        cByte = g_cSeqTable[g_nSeqTableIndex];
        if(cByte == SEQDELIMITER)
            EndCycle();
        else
            IncToNx();
    }
}


//////////////////////////////////////////////////////
// CompAgain( void )            from ASM = COMPAGAIN. (Part of MONBAT )
//
//
//
// M.McKiernan                          09-07-2004
// First pass.
//////////////////////////////////////////////////////
void CompAgain( void )
{
int nStage;
unsigned int nCompIndex = g_cCompNo - 1;

    if(g_nCorrectRetries == 0)  //retrying?
    {
//--REVIEW--  This is new in C, where the ignore is set for the relevant stage if multistage filling.
//SETTOIGNORECPI - expanded inline.
// Set relevant ignore CPI.
    if(g_CalibrationData.m_nStageFillEn)
    {
        nStage = GetFillingStage( (int)g_cCompNo );
        if(nStage == STAGE_3)
            g_Stage3CPIRRData.m_ComponentCPIRRData[nCompIndex].m_bIgnoreCPIRR = TRUE;
        else if(nStage == STAGE_2)
            g_Stage2CPIRRData.m_ComponentCPIRRData[nCompIndex].m_bIgnoreCPIRR = TRUE;
        else
            g_MainCPIRRData.m_ComponentCPIRRData[nCompIndex].m_bIgnoreCPIRR = TRUE;
    }
    else // i.e. not multistage fill
    {
        g_MainCPIRRData.m_ComponentCPIRRData[nCompIndex].m_bIgnoreCPIRR = TRUE;
    }
    }
   g_nCorrectRetries = 0;
    g_bPrintNow = TRUE;
    g_nAnyRetry = (int)g_cCompNo;   //set any retry flag
    if(g_nCyclePauseType == END_OF_CYCLE_PAUSE_TYPE)
    {
        ImmediatePauseOn();
        g_nCyclePauseType = 0;
    }

    Same1();  // New function from MONBAT.
}

//////////////////////////////////////////////////////
// IncToNx( void )          from ASM = INCTONX. (Part of MONBAT )
//
//
//
// M.McKiernan                          09-07-2004
// First pass.
//////////////////////////////////////////////////////
void IncToNx( void )
{
    g_bFillAlarm = FALSE; // reset comp fill alarm
    g_nFillCtr = 0;
    ResetFillCounters();    // reset fill counters.

    Same1();                // New function from MONBAT.
}

//////////////////////////////////////////////////////
// Same1( void )            from ASM = SAME1. (Part of MONBAT )
//
//
//
// M.McKiernan                          098-07-2004
// First pass.
//////////////////////////////////////////////////////
void Same1( void )
{
    g_nFillCounter = 0; //
    if(g_nPauseFlag)    // paused??
    {
        g_cBatSeq = BATSEQ_PAUSEBFILL;  //pause before fill.
    }
    else // NOPAUS2
    { // JSR CHECKLLS
        StartFillSequence();
    }
}

//////////////////////////////////////////////////////
// ResetFillCounters( void )            from ASM = RESETFILLCOUNTERS
// Resets fill (retry) counters/flags
//
//
// M.McKiernan                          08-07-2004
// First pass.
//////////////////////////////////////////////////////
void ResetFillCounters( void )
{
    g_nRetryCounter = 0;        // CLR     RETRYCOUNTER
    g_cRetryActive = 0; // CLR     RETRYACTIVE
}

//////////////////////////////////////////////////////
// StartFillSequence( void )            from ASM = CHECKLLS
// Initialises for fill sequence.
//
//
// M.McKiernan                          08-07-2004
// First pass.
//////////////////////////////////////////////////////
void StartFillSequence( void )
{
BOOL volatile bQuit = FALSE;

        if(!g_nPauseFlag) // only runs if Pause flag already cleared
        {
            g_cBatSeq = BATSEQ_FILL;    // advance sequence to filling
            CloseDump();
            g_bCycleIndicate = TRUE; //
            g_bActivateLatency = TRUE;      // set the activatelatency
            if(g_cRetryActive || !g_CalibrationData.m_nStageFillEn) // retrying or stage fill enabled
            {
                bQuit = TRUE;
            }
            else if(g_CalibrationData.m_bLayering)  // layering needed???
            {
                g_bStageFillCalc = TRUE;
                bQuit = TRUE;
            }
            else if(g_nSeqTableIndex == 0)  // 1st component being dispensed??
            {
                //ACCL
                if(!g_nTopUpSequencing)
                {
               // JSR COMP1STXTOTWGT -1ST COMP X TOTAL BATCH WEIGHT.
                    Comp1stxTotWgt();  //Ensure target for comp 1 recalculated.
                }
            }
            else if(!g_bTopUpFlag || !g_nTopUpSequencing)
            {  //DOSTFL
                    if(!g_bMultipleFilling) //multiple filling activated??
                {
                        g_bStageFillCalc = TRUE;
                }
            }
        }
}


//////////////////////////////////////////////////////
// StoreEndCounts( void )           from ASM = STORNCT
// Store end counts/weights after component fill.
//
//
// M.McKiernan                          13-07-2004
// First pass.
//////////////////////////////////////////////////////
void StoreEndCounts( void )
{
unsigned char cTest;
int nStage;
long lTest;
long lDiff;
float fWeight;

    cTest = g_cCompNo;

    //CALCVIBAVG already taken care of in SettledAvg().
    // let Current LC A/D = settled average.
    g_lCurrentLCAtDValue = g_lSettledAverage;
    //STORERAWWEIGHT - diagnostics only.

    g_lSettledAverageCounts[g_cCompNo - 1] = g_lSettledAverage; // settled average counts

    g_lCmpTareCounts[g_cCompNo - 1] = g_lCurrentLCAtDTare; // component tare counts

    g_lCmpRawCounts[g_cCompNo - 1] = g_lRawAtDCounts; // save Raw A/d counts.
    // diagnostics only. --REVIEW--
    g_lCmpTareCounts[g_cCompNo - 1] =  g_lCurrentLCAtDTare;

    if(g_lCurrentLCAtDValue > g_lCurrentLCAtDTare)
        lDiff = g_lCurrentLCAtDValue - g_lCurrentLCAtDTare;
    else
        lDiff = 0;
    // save difference, i.e. counts for this component.
    g_lComponentCountsActual[g_cCompNo - 1] = lDiff;    // CMP1CNAC       ; ACTUAL COUNTS #1

    fWeight = (float)lDiff / g_CalibrationData.m_fWeightConstant; // counts / const. = kg.

    g_fComponentActualWeight[g_cCompNo - 1] = fWeight;
    if(cTest == TOPUPCOMPONENT)
        lTest = (long)(fWeight* 100);
    // calculate english (Imperial) units.
    g_fComponentActualWeightI[g_cCompNo - 1] = fWeight * WEIGHT_CONVERSION_FACTOR;
    // Calculate round robin weight.
    nStage =  GetFillingStage( g_cCompNo );    // get stage of filling.
    if(nStage == 0) //only calculate here if multi stage fill is off
    {
        CalculateComponentRoundRobinWeight(g_cCompNo); // asm CALCRROBINFORWEIGHT
    }

    // Copy weights to Modbus
    CopyComponentWeightsToMB();

}

//////////////////////////////////////////////////////
// CalculateComponentTargetWeight( void )           from ASM = CMPNTAR
// Calculate target wt and counts for specified component.
// Entry: Component no. passed in g_cCompNo.
//
// Exit: Target wt in g_fComponentTargetWeight[]
//          Imperial units Target wt in g_fComponentTargetWeightI[]
//          Target counts in g_lComponentTargetCounts[]
// M.McKiernan                          14-07-2004
// First pass.
//////////////////////////////////////////////////////
void CalculateComponentTargetWeight( void )
{
    int nCompIndex = (int)(g_cCompNo - 1);
    // comp wt = % * target wt.
    g_fComponentTargetWeight[nCompIndex] = (g_CurrentRecipe.m_fPercentage[nCompIndex] * g_fUsedTargetWeight) / 100.0f;
    // CONVTARTOLBS  (expanded inline)
    // comp wt Imperial units version
    g_fComponentTargetWeightI[nCompIndex] = g_fComponentTargetWeight[nCompIndex] * WEIGHT_CONVERSION_FACTOR;
    // Target in A/D counts
    g_lComponentTargetCounts[nCompIndex] = (long)(g_fComponentTargetWeight[nCompIndex] * g_CalibrationData.m_fWeightConstant);

}

//////////////////////////////////////////////////////
// CalculateComponentTargets( void )            from ASM = CALCULATECOMPONENTTARGETS
// Calculate targets for each component in SeqTable, from current index to end.
//
//
//
// Entry: First component no. is pointed at in SeqTable (g_nSeqTableIndex)
//
// Exit:
//
// M.McKiernan                          15-07-2004
// First pass.
//////////////////////////////////////////////////////
void CalculateComponentTargets( void )
{
    int i = g_nSeqTableIndex;
    int nCompIndex;
    int nCompNo;

    while(g_cSeqTable[i+1] != SEQDELIMITER && i < MAX_COMPONENTS)   // SEQDELIMITER indicates table end
    {
        nCompNo = (int)(g_cSeqTable[i+1] & 0x0F);   // get component no. from table, mask off the m.s. nibble
        nCompIndex = nCompNo - 1;                       // component no.s in table start at 1, index is 0 based.
        CalculateComponentTargetN( nCompNo );   // calculate targets for this component.
        if(g_CalibrationData.m_wFillingMethod[nCompIndex] == FILLING_METHOD_PULSING_MODE)
        {

            g_lComponentTargetCountsCopy[nCompIndex] = g_lComponentTargetCounts[nCompIndex];
            g_fComponentTargetWeightCopy[nCompIndex] = g_fComponentTargetWeight[nCompIndex];
             g_lComponentTargetCounts[nCompIndex] = (long)(0.9 * (float)g_lComponentTargetCounts[nCompIndex]);
            g_fComponentTargetWeight[nCompIndex] = g_lComponentTargetCounts[nCompIndex] / g_CalibrationData.m_fWeightConstant;
            g_nPulsingSequence[nCompIndex] = PRE_PULSING_STAGE;
        }
        else
        {
            g_nPulsingSequence[nCompIndex] = 0;
        }
        CalcCompNTime( nCompNo );                   // COMP #N TIMES FOR #2...N
        i++;
    }

}


//////////////////////////////////////////////////////
// Comp1stxTotWgt( void )           from ASM = COMP1STXTOTWGT
// Calculate 1st comp * total batch weight.
// Entry:
//
// Exit:
//
//
// M.McKiernan                          14-07-2004
// First pass.
//////////////////////////////////////////////////////
void Comp1stxTotWgt( void )
{
    int nCompIndex;

    g_cCompNoDetails = g_cSeqTable[0];      // First component in table
    g_cCompNo = g_cCompNoDetails & 0x0F;    //isolate component number.

    nCompIndex = (int)(g_cCompNo -1);

    // let used target wt be the calibration batch size.
    g_fUsedTargetWeight = g_CalibrationData.m_fBatchSize;

    // Calculate target wt. for this component.
    CalculateComponentTargetWeight( );

    //Top up enabled?
    if(g_bTopUpFlag && !(g_cCompNoDetails & REGRIND_BIT))
    {
        g_nTopUpSequencing = FILLFIRST;
        g_lComponentTargetCountsTotal[nCompIndex] = g_lComponentTargetCounts[nCompIndex];

        //CMP1SETTOPUPWT // COMPONENT #1 TARGET WEIGHT
        g_fComponentTargetTopUpWeight[nCompIndex] = (float)g_lComponentTargetCountsTotal[nCompIndex] / g_CalibrationData.m_fWeightConstant;
        // Comp. top up wt in Imperial
        g_fComponentTargetTopUpWeightI[nCompIndex] = g_fComponentTargetTopUpWeight[nCompIndex] * WEIGHT_CONVERSION_FACTOR;
        // Wt used in CalculateComponentTarget() = (Topup% * batch size) / 100.
        g_fUsedTargetWeight = (g_CalibrationData.m_nPercentageTopUp * g_CalibrationData.m_fBatchSize) / 100.0f;
        // Calculate target wt. for this component.
        CalculateComponentTargetWeight( );
    }

// NOTUP1
        CalcCompNTime( (int)g_cCompNo );       //CALCMNTIM CALCULATE TIME FOR 1ST COMP IF FILL BY TIME, i.e. not first cycle.

}


//////////////////////////////////////////////////////
// CalculateComponentTargetN( nCompno )         from ASM = CALTARN
// Target counts #N = (TARGET #1 / %1)*(%N)
// calculates targets for components other than the main comp
// If firstcompcpmendis flag is set, then use the target as reference
//
// Entry:   Component no. for which targets calculated.
//              Component pointed at in SeqTable (g_nSeqTableIndex) must be reference component.
//
//
// Exit: Target wt in g_fComponentTargetWeight[]
//          Imperial units Target wt in g_fComponentTargetWeightI[]
//          Target counts in g_lComponentTargetCounts[]
// M.McKiernan                          15-07-2004
// First pass.
//////////////////////////////////////////////////////
void CalculateComponentTargetN( int nTargCompNo )
{
    BOOL bChk1st;
    int nRefCompNo;
    int nRefCompIndex;
    long lCounts;
//  float fCounts;
    double ffCounts;
    long lTargCounts,lRefPercentage,lTargPercentage;

    int nTargCompIndex = nTargCompNo - 1;

    // get component no.
    nRefCompNo = (int)(g_cSeqTable[g_nSeqTableIndex] & 0x0F);
//15.09.2004    nRefCompNo = (int)(g_cSeqTable[0] & 0x0F);  // ref comp. = first in seq. table.
    //index for component data arrays.
    nRefCompIndex = nRefCompNo - 1;


    bChk1st = FALSE;

    if(!g_CalibrationData.m_nStageFillEn)
    {
        bChk1st = TRUE;
    }
    else
    if((g_cSeqTable[0] & REGRIND_BIT) == 0) // regrind ?
    {
        if(g_nSeqTableIndex != 0)
        {
            bChk1st = TRUE;
        }
        else
        if(!g_CalibrationData.m_bLayering)
        {
            bChk1st = TRUE;
        }
    }
    else   // regrind active
    {
        if(g_nSeqTableIndex != 1)
        {
            bChk1st = TRUE;
        }
        else
        if(!g_CalibrationData.m_bLayering)
        {
            bChk1st = TRUE;
        }

    }

    if(bChk1st)

    {   //CHK1ST
        // --REVIEW-- Next 2 lines had been removed in assembler. "ch"
        //TST     FIRSTCOMPCOMPENDIS      ; 1ST COMPONENT COMPENSATION.
        //BNE     USETAR                  ; USE TARGET RATHER THAN THE ACTUAL
        if((g_bFillAlarm || !g_CalibrationData.m_bFirstComponentCompensation) && !g_bTopUpFlag)
        {
            lCounts = g_lComponentTargetCounts[nRefCompIndex];
        }
        else // no fill alarm
        {

            if(g_bTopUpFlag)
            {
                if(g_nTopUpSequencing == NORMALOPERATION)
                {
                    lCounts = g_lComponentCountsActual[nRefCompIndex];  //WEIGHT COUNTS FOR COMP #1
                    lCounts += g_lComponentCountsActual[TOPUPCOMPONENT-1];   // ADD COUNTS FOR #1 AND TOP UP COMPONENT. --REVIEW--
                }
                else // NOTNRM
                {

//                    if(g_bFirstWCycle)
//                    {
//                        lCounts = g_lComponentCountsActual[nRefCompIndex];  //WEIGHT COUNTS FOR COMP #1
//                    }
//                    else
//                    {
                            lCounts = g_lComponentTargetCountsTotal[nRefCompIndex];
//                    }
                }
            }
            else // Top Up flag not set.
            {
                lCounts = g_lComponentCountsActual[nRefCompIndex];  // WEIGHT COUNTS FOR COMP #1
            }
        }
    }
    else
    {
        // used stored target.
        lCounts = g_lStoredComponentTargetCounts[nRefCompIndex];
    }
//CALOTT
    // Counts = counts(1) *  %(n) / %(1).
//10.09.2004    fCounts = ( (float)lCounts * g_CurrentRecipe.m_fPercentage[nTargCompIndex] ) / g_CurrentRecipe.m_fPercentage[0];
    lTargPercentage = (long)(g_CurrentRecipe.m_fPercentage[nTargCompIndex] * 100.0);
    lRefPercentage = (long)(g_CurrentRecipe.m_fPercentage[nRefCompIndex] * 100.0);

    ffCounts = ( (double)lCounts * g_CurrentRecipe.m_fPercentage[nTargCompIndex] ) / g_CurrentRecipe.m_fPercentage[nRefCompIndex];

    lTargCounts = (long)ffCounts;

    g_lComponentTargetCounts[nTargCompIndex] = (long)ffCounts;

    // calculate wt based on these counts.
    //COMPONENT #n TARGET WEIGHT
    g_fComponentTargetWeight[nTargCompIndex] = ffCounts / g_CalibrationData.m_fWeightConstant;
    // Convert to imperial.
    g_fComponentTargetWeightI[nTargCompIndex] = g_fComponentTargetWeight[nTargCompIndex] * WEIGHT_CONVERSION_FACTOR;

}

//////////////////////////////////////////////////////
// CalculateHopperWeightAvg(void )          from ASM = CALHWTAV
//
// Calculater hopper weight based on settled avg A/D counts
//
//
// Entry:   Must have a valid settled average counts
//
//
// Exit:    Weight returned in g_fAvgHopperWeight, used to be in g_fWeightInHopper
//
// M.McKiernan                          16-07-2004
// First pass.
//////////////////////////////////////////////////////
void CalculateHopperWeightAvg( void )
{
long lDiff;

    if(g_lSettledAverage > g_lHopperTareCounts)
        lDiff = g_lSettledAverage - g_lHopperTareCounts;
    else
        lDiff = 0;
    // Caluclate weight in hopper.
//10.09.2004    g_fWeightInHopper = (float)lDiff / g_CalibrationData.m_fWeightConstant; // counts / const. = kg.
    g_fAvgHopperWeight = (float)lDiff / g_CalibrationData.m_fWeightConstant; // counts / const. = kg.
}

//////////////////////////////////////////////////////
// CalculateHopperWeight(void )         from ASM = CALHWT
//
// Calculater hopper weight based on
//
//
// Entry:   Must have a valid A/D counts
//
//
// Exit:    Weight in g_fOneSecondHopperWeight
//
// M.McKiernan                          27-07-2004
// First pass.
//////////////////////////////////////////////////////
void CalculateHopperWeight( void )
{
long lDiff;

    lDiff = 0;
    if(g_lOneSecondWeightAverage > g_lHopperTareCounts)
        lDiff = g_lOneSecondWeightAverage - g_lHopperTareCounts;

    // Caluclate weight in hopper (one sec avg)
    g_fOneSecondHopperWeight = (float)lDiff / g_CalibrationData.m_fWeightConstant; // counts / const. = kg.
}

//////////////////////////////////////////////////////
// CalculateActualPercentages(void )            from ASM = BPCNTNCAL
//
// Calculation of the actual % for each component.
// %N = (comp weight / total weight - regrind comp weight) if regrind.

//
// Entry:   Must have a valid settled average counts
//
//
// Exit:
//
// M.McKiernan                          16-07-2004
// First pass.
//////////////////////////////////////////////////////
void CalculateActualPercentages( void )
{
    float   fAddition;
    int i;

    EstimateLiquidComponentWeight();   // estimate liquid pib is required

    fAddition = 0.0f;        // set counter to 0
    if(g_bShowInstantaneousWeight)
    {
        for(i = 0; i < MAX_COMPONENTS; i++)  // .
        {
            fAddition += g_fComponentActualWeight[i];
        }
    }
    else
    {
        for(i = 0; i < MAX_COMPONENTS; i++)  // .
        {
            fAddition += g_fComponentActualWeightRR[i];
        }
    }
    g_fWeightInHopper = fAddition;

    CalculateActualComponentPercentages(); // calculate % data
    CalculateActualComponentPercentagesRR(); // calculate % data nbb--todo-- check for liquid additive
    CheckComponentPercentages();
}
//////////////////////////////////////////////////////
// CalculateActualComponentPercentages(void )           from ASM = CALCULATEPERDATA
//
// Calculation of the actual % for each component in sequence table.
// %N = (comp weight / total weight - regrind comp weight) if not a regrind component
// %N = (comp weight / total weight in hopper) if regrind component.
//
// Entry:   Weight in hopper has already been calculated
//
//
// Exit:    Actual %'s stored in g_fComponentActualPercentage[i]

//
// M.McKiernan                          19-07-2004
// First pass.
// M.McKiernan                          14-09-2004
// Pick up regrind comp. from recipe structure, not calibration data.
// M.McKiernan                          15-09-2004
// % for regrind component based on full hopper weight, others based on hopper wt. - regrind.
//////////////////////////////////////////////////////
void CalculateActualComponentPercentages( void )
{
    unsigned int i,nFirstCompIndex;
//  int nRegCompIndex = g_CalibrationData.m_nRegrindComponentNumber - 1;    //Regrind component index.
    int nRegCompIndex = g_CurrentRecipe.m_nRegrindComponent - 1;    //Regrind component index.
    unsigned char cData;


    for(i=0; i < MAX_COMPONENTS; i++)
    {
        g_fComponentActualPercentage[i] = 0;    // clear %'s initially.
    }

//   if(g_bRegrindPresent)   // Is there regrind??
   if((g_CurrentRecipe.m_nRegrindComponent != 0)&& (g_CurrentRecipe.m_fPercentage[nRegCompIndex] > 0.0001))   // Is there regrind??
   {
        // subtract wt of regrind component.
      g_fWeightInHopperMinusRegrind = g_fWeightInHopper - g_fComponentActualWeight[nRegCompIndex];
   }
   else
   {     // no regrind, same as hopper wt.
    g_fWeightInHopperMinusRegrind = g_fWeightInHopper;
   }

    if(g_bTopUpFlag)    //Top up enabled?
    {

        cData = g_cSeqTable[0];
        if(cData & REGRIND_BIT)
            nFirstCompIndex = (int)(g_cSeqTable[1] & 0x0F) - 1; /* advance by one if regrind */
        else
            nFirstCompIndex = (int)(g_cSeqTable[0] & 0x0F) - 1;

        // Add component 8's weight to first component.  Assuming 8 is top-up component.
        g_fComponentActualWeight[nFirstCompIndex] += g_fComponentActualWeight[TOPUPCOMPONENT -1];    // --REVIEW-- (DANGEROUS USING 8 FOR TOP-UP.
        // convert to Imperial units
        g_fComponentActualWeightI[nFirstCompIndex] = g_fComponentActualWeight[nFirstCompIndex] * WEIGHT_CONVERSION_FACTOR;

        // Clear top-up component weights.
        g_fComponentActualWeightTopUpBackup = g_fComponentActualWeight[TOPUPCOMPONENT -1];
        g_fComponentActualWeightITopUpBackup = g_fComponentActualWeightI[TOPUPCOMPONENT -1];
        g_fComponentActualWeight[TOPUPCOMPONENT -1] = 0;  // --REVIEW--
        g_fComponentActualWeightI[TOPUPCOMPONENT -1] = 0; //
    }
    i = 0;
    // repeat for each component in Sequence table.
//    while(g_cSeqTable[i] != SEQDELIMITER && i < MAX_COMPONENTS)
     for(i=0; i < MAX_COMPONENTS; i++)
     {
//        nCompIndex = (int)(g_cSeqTable[i] & 0x0F) - 1;                      // component no.s in table start at 1, index is 0 based.
//    if( (nCompIndex + 1) == g_CurrentRecipe.m_nRegrindComponent)   // Is this the regrind??
    if(( (i + 1) == g_CurrentRecipe.m_nRegrindComponent)&& (g_CurrentRecipe.m_fPercentage[nRegCompIndex] > 0.0001))   // Is this the regrind??
            {
                g_fComponentActualPercentage[i] = (g_fComponentActualWeight[i] * 100.0f) /  g_fWeightInHopper;
            }
        else  //not regrind component.
            {
                g_fComponentActualPercentage[i] = (g_fComponentActualWeight[i] * 100.0f) / g_fWeightInHopperMinusRegrind;
            }
//        i++;

    }
    if(g_bTopUpFlag)    //Top up enabled?
    {
        g_fComponentActualWeight[TOPUPCOMPONENT -1] = g_fComponentActualWeightTopUpBackup;
        g_fComponentActualWeightI[TOPUPCOMPONENT -1] = g_fComponentActualWeightITopUpBackup;
    }

    CopyActualPercentagesToMB();    // copy %'s into MB table.
}

void CalculateActualComponentPercentagesRR( void )
{
    unsigned int i,nFirstCompIndex,nCompIndex;
//  int nRegCompIndex = g_CalibrationData.m_nRegrindComponentNumber - 1;    //Regrind component index.
    int nRegCompIndex = g_CurrentRecipe.m_nRegrindComponent - 1;    //Regrind component index.
    unsigned char cData;


    for(i=0; i < MAX_COMPONENTS; i++)
    {
        g_fComponentActualPercentageRR[i] = 0;    // clear %'s initially.
    }

//   if(g_bRegrindPresent)   // Is there regrind??
   if((g_CurrentRecipe.m_nRegrindComponent != 0) && (g_CurrentRecipe.m_fPercentage[nRegCompIndex] > 0.0001))   // Is there regrind??
   {
        // subtract wt of regrind component.
      g_fWeightInHopperMinusRegrind = g_fWeightInHopper - g_fComponentActualWeightRR[nRegCompIndex];
   }
   else
        // no regrind, same as hopper wt.
    g_fWeightInHopperMinusRegrind = g_fWeightInHopper;


    if(g_bTopUpFlag)    //Top up enabled?
    {

        cData = g_cSeqTable[0];
        if(cData & REGRIND_BIT)
            nFirstCompIndex = (int)(g_cSeqTable[1] & 0x0F) - 1; /* advance by one if regrind */
        else
            nFirstCompIndex = (int)(g_cSeqTable[0] & 0x0F) - 1;

        // Add component 8's weight to first component.  Assuming 8 is top-up component.
        g_fComponentActualWeightRR[nFirstCompIndex] += g_fComponentActualWeightRR[TOPUPCOMPONENT -1];    // --REVIEW-- (DANGEROUS USING 8 FOR TOP-UP.
        // convert to Imperial units
        g_fComponentActualWeightI[nFirstCompIndex] = g_fComponentActualWeightRR[nFirstCompIndex] * WEIGHT_CONVERSION_FACTOR;

        // Clear top-up component weights.
        g_fComponentActualWeightTopUpBackup = g_fComponentActualWeightRR[TOPUPCOMPONENT -1];
        g_fComponentActualWeightITopUpBackup = g_fComponentActualWeightI[TOPUPCOMPONENT -1];
        g_fComponentActualWeightRR[TOPUPCOMPONENT -1] = 0;  // --REVIEW--
        g_fComponentActualWeightI[TOPUPCOMPONENT -1] = 0; //
    }
    i = 0;
    // repeat for each component in Sequence table.
    while(g_cSeqTable[i] != SEQDELIMITER && i < MAX_COMPONENTS)
    {
        nCompIndex = (int)(g_cSeqTable[i] & 0x0F) - 1;                      // component no.s in table start at 1, index is 0 based.
    if(( (nCompIndex + 1) == g_CurrentRecipe.m_nRegrindComponent)&& (g_CurrentRecipe.m_fPercentage[nRegCompIndex] > 0.0001))   // Is this the regrind??
            {
                g_fComponentActualPercentageRR[nCompIndex] = (g_fComponentActualWeightRR[nCompIndex] * 100.0f) / g_fWeightInHopper;
            }
        else  //not regrind component.
            {
                g_fComponentActualPercentageRR[nCompIndex] = (g_fComponentActualWeightRR[nCompIndex] * 100.0f) / g_fWeightInHopperMinusRegrind;
            }
        i++;

    }
    if(g_bTopUpFlag)    //Top up enabled?
    {
        g_fComponentActualWeightRR[TOPUPCOMPONENT -1] = g_fComponentActualWeightTopUpBackup;
        g_fComponentActualWeightI[TOPUPCOMPONENT -1] = g_fComponentActualWeightITopUpBackup;
    }
    CopyActualPercentagesToMB();    // copy %'s into MB table.
}


//////////////////////////////////////////////////////
// CalculateCPI(int nComp )         from ASM = CALCNTPI
//
// Calculation of Counts Per Interrupt for Component N
//

//
// Entry:
//
//
// Exit:

//
// M.McKiernan                          19-07-2004
// First pass.
//////////////////////////////////////////////////////
void CalculateCPI( int nComponent )
{
    double ffCPI;
    double *ffPointer;
    int nCompIndex = nComponent - 1;    // index is 0 based.
    BOOL bUseRR;
    // select array to store result
    //if(fdTelnet > 0)
    //iprintf("\n calculate cpi for comp %d",nComponent);
    ffPointer = SetCPIPointer( nComponent );

    if(g_lCmpTime[nCompIndex] > 0) // if not zero (no divide by 0 allowed
    {
        // CPI = counts / open time
        ffCPI = (double)g_lComponentCountsActual[nCompIndex] / (double)g_lCmpTime[nCompIndex];
        CheckForValidFlowRate(ffCPI,nComponent);
    }
    else
    {
        ffCPI = 0;  // if time = 0, set CPI = 0.
    }
    g_ffInstantaneousCPI[nCompIndex] = ffCPI;       // store for test purposes ----testonly----  --REVIEW--
    // no flow calc for this component??
    if( g_bNoFlowRateCalculationArray[nCompIndex] )
    {
        g_bNoFlowRateCalculationArray[nCompIndex] = FALSE;
        g_bNoFlowRateCalculation = FALSE;
    }
    else
    {
        if(g_bNoFlowRateCalculation)
        {
            g_bNoFlowRateCalculation = FALSE;
        }
        else
        {
//          bUseRR = FALSE;
            g_ffCPI = ffCPI;    // store in global for passing into RR calculation function.
            bUseRR = CalculateCPIRoundRobin( (int)g_cCompNo );  // CALCCPIRROBIN calculate CPI based on RR data.
            if(bUseRR)
            {
                // use R.R. cpi value.
                *(ffPointer + nCompIndex) = g_ffCPI_RR; // store Round robin value from CalculateRoundRobinCPI()
            }
            else
            {
                *(ffPointer + nCompIndex) = ffCPI;  // store calculated value.
            }

    if(ffCPI < 0.001)
    {
                *(ffPointer + nCompIndex) = g_ffCPI_RR; // store Round robin value from CalculateRoundRobinCPI()

/*                  nIndex = g_nSeqTableIndex;
                    cData = g_cSeqTable[nIndex];  //load byte from selected index
                    cData |= FILLBYWEIGHT;          // Fill by wt.
                    cData |= LEAVEFILLBYWEIGHT; // indicate f.b.w. should not be cleared.
                    g_cSeqTable[nIndex] = cData;    //
                    //SetupMttty();
                    //iprintf("\n flow rate is zero seq table is %x",cData);


                    // cause the CPI round robins to be reset.
                    g_MainCPIRRData.m_ComponentCPIRRData[nCompIndex].m_nResetCPIRRCtr = RESETRRNO;
                    g_Stage2CPIRRData.m_ComponentCPIRRData[nCompIndex].m_nResetCPIRRCtr = RESETRRNO;    //
                    g_Stage3CPIRRData.m_ComponentCPIRRData[nCompIndex].m_nResetCPIRRCtr = RESETRRNO;    //
*/
    }

    // test purposes only.
    g_lCPI[nCompIndex] = (long)(100 * g_ffComponentCPI[nCompIndex]);
    g_lCPIStage2[nCompIndex] = (long)(100 * g_ffComponentCPIStage2[nCompIndex]);
    g_lCPIStage3[nCompIndex] = (long)(100 * g_ffComponentCPIStage3[nCompIndex]);
    SaveFlowRatesToNonVolatileMemory();

        }

    }

}

//////////////////////////////////////////////////////
// SaveFlowRatesToNonVolatileMemory
//
// Save flow rates to non volatile memory and generate checksum for same
//
//////////////////////////////////////////////////////

void SaveFlowRatesToNonVolatileMemory( void )
{
    unsigned int i;
    for(i= 0; i < MAX_COMPONENTS; i++)
    {
        g_ProcessData.m_ffComponentCPI[i] =  g_ffComponentCPI[i];
        g_ProcessData.m_ffComponentCPIStage2[i] = g_ffComponentCPIStage2[i];
        g_ProcessData.m_ffComponentCPIStage3[i] = g_ffComponentCPIStage3[i];
    }
    g_ProcessData.m_nFlowRateChecksum = (int) (GenerateFlowRateChecksum()* 100);
    g_bSaveAllProcessDataOutsideCycle = TRUE;

}
//////////////////////////////////////////////////////
// GenerateFlowRateChecksum
//
// Checksum generation for flow rates
//
//////////////////////////////////////////////////////

double GenerateFlowRateChecksum( void )
{
    double   ffAddition;
    ffAddition = 0.0f;
    unsigned int i;
    for(i = 0; i < MAX_COMPONENTS; i++)  // .
    {
        ffAddition += g_ProcessData.m_ffComponentCPI[i];
    }
    for(i = 0; i < MAX_COMPONENTS; i++)  // .
    {
        ffAddition += g_ProcessData.m_ffComponentCPIStage2[i];
    }
    for(i = 0; i < MAX_COMPONENTS; i++)  // .
    {
        ffAddition += g_ProcessData.m_ffComponentCPIStage3[i];
    }
    return (ffAddition);
}

//////////////////////////////////////////////////////
// CheckFlowRateChecksum
//
// Checksum generation for flow rates
//
//////////////////////////////////////////////////////

void CheckFlowRateChecksum( void )
{
    unsigned int nChecksum,i;
    nChecksum = (int) (GenerateFlowRateChecksum()* 100);
    if(nChecksum != g_ProcessData.m_nFlowRateChecksum)
    {
        for(i = 0; i < MAX_COMPONENTS; i++)  // .
        {
            g_ffComponentCPI[i] = 0.0f;
        }
    }
}

/*
CALCNTPI:
        JSR     CLRAC
        LDX     #CMP1CNAC
        LDAB    COMPNO          ; CURRENT COMPONENT
        DECB                    ;
        ABX
        ABX
        ABX

ADDSEL1 LDAA    0,X             ; A/D COUNTS FOR COMP #N
        LDE     1,X             ;
        STAA    MATHCREG+0          ; EG 20000 00
        STE     MATHCREG+1          ; A/D COUNTS TO C REG

        LDX     #CMP1TIMS       ;
        LDAB    COMPNO          ; CURRENT COMPONENT
        DECB
        ABX
        ABX
        ABX
        LDAB     0,X             ; READ COUNTER VALUE
        LDE     1,X             ; READ COUNTER VALUE

        JSR     HEXBCD3
        STD     AREG+1
        STE     AREG+3          ;




;  HEXBCD2  CONVERTS A 2 BYTE in D [A;B]) HEX VALUE TO BCD in B:E (MAX 65535)
;           RESULTING BCD IS IN B:E   (IE 6 5535 FOR $FFFF)

        JSR     DIV             ; C/A XXX.YY COUNTS PER INTERRUPT EG 16666 166.66
                                ; XX.YYYY COUNTS PER INTERRUPT EG 16666 - > 1.6666
;        LDX     #CMP1CPI        ; COMP #1 COUNTS/INT STORAGE
        JSR     SETCPIPTR
        LDAB    COMPNO          ; CURRENT COMPONENT
        DECB
        ABX
        ABX
        ABX
        LDAA    EREG+2          ; RESULT
        LDE     EREG+3
        LDAB    COMPNO          ; READ TARGET NO.
        CMPB    #4              ; COMPONENT 4
        BNE    OKAY1           ; NO
        CMPA    #$10
        BLS     OKAY1
        NOP
OKAY1

        LDAB    #RAMBANK
        TBZK
        LDZ     #NOFLOWRATECALCULATION1
        LDAB    COMPNO
        DECB
        ABZ
        TST     0,Z                             ; CHECK FOR NON ZERO.
        BEQ     CHKFR
        CLR     0,Z                             ; RESET FLOW RATE FLAG
        CLR     NOFLOWRATECALCULATION
        BRA     XITFRC

CHKFR   TST     NOFLOWRATECALCULATION
        BEQ     DOCALCFR
        CLR     NOFLOWRATECALCULATION
        BRA     XITFRC
DOCALCFR:
        PSHM    D,E

        JSR     CALCCPIRROBIN

; CHECK FOR RESULT OF 00.00

        TSTA                    ; A =0
        BNE     STRCPI
        TSTE                    ; E =0
;        CPE     #MINFLOWRATE    ; COMPARE TO MIN FLOW RATE
        BNE     STRCPI          ;
        PULM    D,E             ; RECOVER ORIGINAL VALUE.
        STAA    0,X             ;
        STE     1,X             ; COUNTER PER INTERRUPT STORED.
        BRA     XITFRC

STRCPI  STAA    0,X             ;
        STE     1,X             ; COUNTER PER INTERRUPT STORED.
        PULM    D,E             ; RECOVER ORIGINAL VALUE.
XITFRC  RTS                                ;


*/
//////////////////////////////////////////////////////
// SetCPIPointer(nComp )            from ASM = SETCPIPTR
//
// Decide where to store the CPI value
//
//
// Entry:
//
//
// Exit:
//
// M.McKiernan                          19-07-2004
// First pass.
//////////////////////////////////////////////////////
double *SetCPIPointer( int nComponent )
{
int nStage;
double *ffPtr;
// --REVIEW-- Note the storage areas and stages (names) are confusing, ...Store 3 for Stage1.
            if(g_CalibrationData.m_nStageFillEn)            // Stage filling enabled?? can be single/two/three)
            {
                // Stage filling enabled
                nStage = g_nStageSeq[nComponent - 1];
                if(nStage == STAGE_2)
                    ffPtr = g_ffComponentCPIStage2; // use Store 2 for Stage 2
                else if(nStage == STAGE_1)
                    ffPtr = g_ffComponentCPIStage3;  // use Stage 3 for Stage_1  --REVIEW--
                else
                    ffPtr = g_ffComponentCPI;       // else assume default to stage 1 rate.

            }
            else
            { // no stage filling.
                ffPtr = g_ffComponentCPI;
            }

            return(ffPtr);  // return address of array chosen.
}

//////////////////////////////////////////////////////
// CalcCompNTime(int nComponent )           from ASM = CALCMNTIM
//                                                      includes ASM CALTIM.
//
// Calculation of time in PIT periods for Component N
// Time (in PIT periods) = target cts / CPI

//
// Entry:   Component no for calculation.
//
//
// Exit:

//
// M.McKiernan                          28-07-2004
// First pass.
//////////////////////////////////////////////////////
void CalcCompNTime( int nComponent )
{
double *ffPointer;
double ffCPI,ffTime;
long    lCPI,lTemp;

    int nCompIndex = nComponent - 1;    // index is 0 based.


    // select CPI array to use
    ffPointer = SetCPIPointer( nComponent );

    ffCPI = *(ffPointer + nCompIndex);  //get the CPI value
    //--testonly--
    lCPI = (long)ffCPI;
    // ensure no divide by 0
    if(ffCPI != 0)
        // time (in PIT periods) = target cts / CPI
        ffTime = g_lComponentTargetCounts[nCompIndex] / ffCPI;
    else
        ffTime = (float)MINIMUM_PIT_OPEN_TIME;

    if(!(g_bDiagnosticsPasswordInUse && g_CalibrationData.m_bVolumetricModeFlag))
    {
        lTemp = (long)ffTime;
        if(lTemp < MINIMUM_PIT_OPEN_TIME)
        {
            lTemp = MINIMUM_PIT_OPEN_TIME;
        }
        g_lCmpTime[nCompIndex] = lTemp;      // store time.
    }
}

//////////////////////////////////////////////////////
// NeedForTopUp                     from ASM = NEEDFORTOUP
//
//
// CHECKS IS TOP IS REQUIRED.                                  ;
// IF NOT THEN RESET TOP UP FLAG.
//
// Exit:

//
// M.McKiernan                          29-07-2004
// First pass.
//////////////////////////////////////////////////////
void NeedForTopUp( void )
{
    if(g_CalibrationData.m_bTopUpMode)
    {
        if(g_cSeqTable[1] == SEQDELIMITER ) // Only one component??
        {
            if(g_bTopUpFlag) //topup flag set??
            {
                g_bTopUpFlag = FALSE;
                g_bFillTargetEntryFlag = TRUE;        // SET FILL TARGET
            }
        }
        else    // > 1 component
        {
            if(!g_bTopUpFlag)   // Flag not yet set??
            {
                g_bTopUpFlag = TRUE;
                g_bFillTargetEntryFlag = TRUE;        // SET FILL TARGET
            }

        }
    }
    else  // Not in top up mode  //code added 18.11.2004 to cater for transition Topup -> Normal.
    {
            if(g_bTopUpFlag) // Not in top up mode but topup flag set, i.e. transition
            {
                g_bTopUpFlag = FALSE;
                g_bFillTargetEntryFlag = TRUE;        // SET FILL TARGET
            }
    }
}

/*
The program needfortoup is modified to allow this to happen.The line
        BEQ     ON1CM                   ; CHECK FOR TOPUP TO NO TOP UP
is changed to ensure that the topup to "no topup" transition is checked for.
This line was originally
        BEQ     XITTUC                  ; CHECK FOR TOPUP TO NO TOP UP


NEEDFORTOUP:
        TST     TOPUP                   ; IS TOP UP ENABLED
        BEQ     ON1CM                   ; CHECK FOR TOPUP TO NO TOP UP
TRANSITION.
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

//////////////////////////////////////////////////////
// CalcNextTargets( void )          from ASM = CALNEXTAR
//                                                      .
//
// Calculation of targets from current component + 1 to end of sequence table.
//
// Entry:
//
//
// Exit:

//
// M.McKiernan                          10-08-2004
// First pass.
//////////////////////////////////////////////////////
void CalcNextTargets( void )
{
int i;
unsigned char cCompNo;

    i = g_nSeqTableIndex + 1;   // Skip past current component.
    while(g_cSeqTable[i] != SEQDELIMITER && i < MAX_COMPONENTS) // table end or delimiter
    {
        cCompNo = g_cSeqTable[i] & 0x0F;                            // component no. in l.s. nibble
        CalculateComponentTargetN( (int)cCompNo );  // calculate component wt & target cts.
        CalcCompNTime( (int)cCompNo );  // calculate filling time for this component.
        i++;
    }
}

//////////////////////////////////////////////////////
// CheckForRetries( void )          from ASM = CHKFORRETRIES
//                                                      .
// Called at the end of the cycle.
//
// Check each component in the sequence table, if it hasnt retried on a fill,
// clear the corresponding retry counter.  Repeat also for each fill stage if multi-stage filling.
//
// Entry:
//
//
// Exit:

//
// M.McKiernan                          01-09-2004
// First pass.
//////////////////////////////////////////////////////
void CheckForRetries( void )
{
//int *nRetryPointer;
//BOOL *bHasRetriedPointer;
int i;
 int    nCompIndex;
// unsigned char cData;
unsigned char cCompNo;

    i = 0;
    while(g_cSeqTable[i] != SEQDELIMITER && i < MAX_COMPONENTS)             // AA is table end or delimiter.
    {
        cCompNo = g_cSeqTable[i] & 0x0F;  // get component no. from table, mask off the m.s. nibble
        nCompIndex = (int)(cCompNo) - 1;     // component no.s in table start at 1, index is 0 based.

// if it hasnt retried on a stage, clear the corresponding retry counter.
        if(!g_bHasRetried[nCompIndex])
        {
            g_nRetryCounterStage1[nCompIndex] = 0;
        }
        if(g_bTopUpFlag)
        {
            if(!g_bHasRetried[TOPUPCOMPONENT-1])
            {
                g_nRetryCounterStage1[TOPUPCOMPONENT-1] = 0;
            }
        }
      if(g_CalibrationData.m_nStageFillEn)         // Stage filling enabled?? can be single/two/three)
      {
            if(!g_bHasRetriedStage2[nCompIndex])
                g_nRetryCounterStage2[nCompIndex] = 0;

            if(!g_bHasRetriedStage3[nCompIndex])
                g_nRetryCounterStage3[nCompIndex] = 0;
        }

        i++;
    }
// clear all the "has retried" flags, i.e. for each component.
    for(i = 0; i < MAX_COMPONENTS; i++)
    {
        g_bHasRetried[i] = FALSE;
        g_bHasRetriedStage2[i] = FALSE;
        g_bHasRetriedStage3[i] = FALSE;
    }


}

//////////////////////////////////////////////////////
// CheckForSkippedComponent( void )         from ASM = CHKFORSKIPPEDCOMP
//                                                      .
// Check if component should be skipped
// Entry:
//
//
// Exit:

//
// M.McKiernan                          03-09-2004
// First pass.
//////////////////////////////////////////////////////
void CheckForSkippedComponent( void )
{
int nTableIndex;
int nCompIndex;
unsigned char cData;
BOOL bDone = FALSE;

    if(g_CalibrationData.m_nStageFillEn)
    {
        nTableIndex = g_nSeqTableIndex;

        while(g_cSeqTable[nTableIndex] != SEQDELIMITER && !bDone)
        {
            cData = g_cSeqTable[nTableIndex];  //load byte from SEQ table
            nCompIndex = (int)(cData & 0x0F) - 1;

            if( g_bMulFillStarted[nCompIndex] )
            {
                // multiple fill started for this component.
                if( g_nStageSeq[nCompIndex] )
                {
                    g_nSeqTableIndex = nTableIndex; // save off the current index
                    bDone = TRUE;
                }
                else
                {
                    g_bMulFillStarted[nCompIndex]   = FALSE;    //reset multiple fill started flag.
                }
            }
            else
            {
                bDone = TRUE;   // if multiple fill not started, then finished.
            }

        }  //end of while loop.
    }
}

//////////////////////////////////////////////////////
// FindColourComponent( void )          from ASM = FINDCOLOURCOMPONENT
//                                                      .
// Find the colour component assume that it is the last component
// Entry:
//
//
// Exit:    Return index (g_nLastCompTableIndex) of last component in the sequence table.

//
// M.McKiernan                          28-09-2004
// First pass.
//////////////////////////////////////////////////////
void FindColourComponent( void )
{
    unsigned int i;
    g_nLastCompTableIndex = 0;  // initialise to 0, in case no delimiter.
    i = 0;
    BOOL bExit = FALSE;
    for( i=0; i <= MAX_COMPONENTS && i <= g_CalibrationData.m_nComponents && !bExit; i++)
    {
        if(g_cSeqTable[i] == SEQDELIMITER)
        {
            g_nLastCompTableIndex = i - 1;  // last component in seq. table
            bExit = TRUE;
        }
    }
}



/*
; FIND THE COLOUR COMPONENT ASSUME THAT IT IS THE LAST COMPONENT.

FINDCOLOURCOMPONENT:
        LDX     SEQPOINTER
CHKFOL  LDAA    0,X
        CMPA    #SEQDELIMITER
        BEQ     ISATDEL         ; DELIMITER FOUND.
        AIX     #1
        BRA     CHKFOL          ; CHECK FOR LAST.
ISATDEL AIX     #-1             ; STEP BACK TO THE LAST COMPONENT.            ;
        STX     LASTCOMPTABLEP  ; LAST COMPONENT TABLE POINTER.
        RTS

*/
/*
; CHECK IF COMPONENT SHOULD BE SKIPPED
CHKFORSKIPPEDCOMP:
        TST     STAGEFILLEN
        BEQ     SKPCMC                  ; SKIP COMPONENT CHECK
        LDAB    #RAMBANK
        TBZK
        LDX     SEQPOINTER             ; LOAD START
CHKNX1  LDZ     #STAGESEQ1
        LDAB    0,X
        CMPB    #SEQDELIMITER          ; SEQUENCE DELIMITER
        BEQ     SKPCMC                 ; AT END, EXIT
        ANDB    #$0F

; CHECK IF THE PARTICULAR COMPONENT HAS BEEN STARTED
;
        TBA                             ; SAVE B
        LDZ     #MULFILL1STARTED
        DECB
        ABZ
        TAB                             ; RECOVER B
        STZ     TEMPX                   ; STORE TEMPORARILY
        TST     0,Z
        BEQ     SKPCMC                  ; NOT STARTED -> NEXT COMPONENT
        LDZ     #STAGESEQ1
        DECB
        ABZ
        TST     0,Z
        BNE     NOINC1
        LDZ     TEMPX                   ;
        CLR     0,Z                     ; RESET MULTIPLE FILL STARTED FLAG
INC1    AIX     #1                      ; TO NEXT COMPONENT
        BRA     CHKNX1
NOINC1  STX     SEQPOINTER
SKPCMC  RTS


*/

/*
SETTOIGNORECPI:
        PSHM    D,X              ; SAVE REGISTERS.
        LDX     #IGNORECPI1      ; FLAG TO IGNORE CPI ERROR.
        DECB                    ; COMPONENT NO.
        ABX
        LDAA    #IGNORCPINO
        STAA    0,X             ; IGNORE THIS ERROR.
        PULM    D,X             ; RECOVER REGISTERS.
        RTS.

*/
/*
CHKFORRETRIES:
        LDAB   #RAMBANK
        TBZK                    ;~;SET EK FOR BANK 1, I.E. RAM
        LDX     #SEQTABLE
REPRETC PSHM    X
        LDAB    0,X
        CMPB    #SEQDELIMITER   ; AT END.
        BEQ     FINRET
        LDAB    0,X             ; READ COMPONENT NO.
        ANDB    #$0F            ; READ COMPONENT.
        PSHB
;        LDZ     #RETRYCTR1
        PSHM    X
        JSR     GETRETRYCTR
        XGEX
        XGEZ                    ; X -> Z
        PULM    X

        DECB
        ABZ                     ; Z POINTS TO RETRY COUNTER.
        PULB
;        LDX     #HASRETRIED1    ; LOAD RETRY FLAG.
        JSR     GETHASRETRIEDPTR
        DECB
        ABX
        TST     0,X             ; HAS THIS COMPONENT RETRIED.
        BNE     HASRETR         ; NO
        CLR     0,Z
HASRETR PULM    X               ; PULL X
        AIX     #1              ; INCREMENT
        BRA     REPRETC         ; REPEAT RETRY CHECK.

FINRET  PULM    X
;        LDX     #HASRETRIED1
        JSR     GETHASRETRIEDPTR
        LDAA    #MAXCOMPONENTNO
        JSR     CLRMEM          ; CLEAR
        RTS


*/

/*
CALNEXTAR:
        LDX     SEQPOINTER      ; LOAD CURRENT POINTER POSITION
        AIX     #1              ; GET PAST THIS COMPONENT.

CALNEC  PSHM    X
        LDAB    0,X
        ANDB    #$0F
        STAB    TARGNO
        STAB    COMPNO
        STX     TEMP1S          ; CURRENT TABLE POSITION.
        JSR     CALTARN         ; TARGETS FOR COMP #2....N
        JSR     CALCMNTIM       ; COMP #N TIMES FOR #2...N
        PULM    X
        AIX     #1              ; TABLE POINTER +1
        LDAA    0,X
        CMPA    #SEQDELIMITER
        BEQ     ATLASC          ; GET OUT OF HERE.

        BRA     CALNEC          ; NEXT COMPONENT
ATLASC  RTS


*/
/*
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
;       CALCULATION OF THE ACTUAL % FOR EACH COMPONENT.
;
;    %N = ((COMPNWEIGHT (*0000)/ BATCH WEIGHT) ROUNDED TO 1DP)SHIFT 1 DIGIT RIGHT
;       = XXX.Y %
;       RESULT STORED IN BPCNT1....BPCNTN.
;
;    %N = (COMP WEIGHT / TOTAL WEIGHT - REGRIND COMP WEIGHT) IF REGRIND.

BPCNTNCAL:

        JSR     CALHWTAV        ; CURRENT WEIGHT
        LDD     EREG
        STD     WEIGHTINHOP     ;
        LDAA    EREG+2
        STAA    WEIGHTINHOP+2    ; STORE WEIGHT IN HOPPER
        LDX     #BPRRCNT1
        STX     PPERCENT            ; POINTER TO %
        LDX     #CMP1ACTWGTRR    ; ROUND ROBIN VALUE.
        STX     PACTWEIGHT               ; POINTER TO ACTUAL WEIGHT
        JSR     CALCULATEPERDATA ; CALCULATE % DATA
        LDX     #BPCNT1
        STX     PPERCENT        ; POINTER TO %
        LDX     #CMP1ACTWGT      ; ROUND ROBIN VALUE.
        STX     PACTWEIGHT               ; POINTER TO ACTUAL WEIGHT
        JSR     CALCULATEPERDATA ; CALCULATE % DATA
        RTS


CALCULATEPERDATA:
        LDX     #SEQTABLE
        STX     TEMPX           ; SET START OF SEQ TABLE.
        LDX     PPERCENT
        LDAB    #MAXCOMPONENTNO
REPCLEAR:
        CLRW    0,X
        AIX     #2
        DECB
        BNE     REPCLEAR       ; IS REGRIND PRESENT.
        JSR     CLAREG


        LDD     WEIGHTINHOP
        STD     AREG+2          ;
        LDAA    WEIGHTINHOP+2
        STAA    AREG+4          ;  STORE WEIGHT IN HOPPER

        LDAB    REGCOMP         ; REGRIND COMPONENT.
;        LDX     #CMP1ACTWGT     ; ADDRESS OF COMP #N WEIGHT
        LDX     PACTWEIGHT              ; POINTER TO ACTUAL WEIGHT
        DECB
        ABX
        ABX
        ABX
        JSR     CMOVE
        JSR     SUB             ; TOTAL - REG COMP

        LDD     EREG+2
        STD     WEIGHTAFTREG    ;
        LDAA    EREG+4
        STAA    WEIGHTAFTREG+2   ; STORE WEIGHT IN HOPPER


NOREGNORM:

; IF TOPUP AND 1ST COMPONENT, ADD 1ST AND 4TH COMPONENTS TO DETERMINE THE ACTUAL % REQUIRED.

        TST     TOPUPF           ; IS TOP UP ENABLED ?
        LBEQ     NORMPCL         ; NORMAL % CALCULATION.
        LDAB    SEQTABLE
        ANDB    #$0F
        STAB    TEMPC           ; STORE COMPONENT NO.

        LDAB    TEMPC           ;
        LDX     #CMP1ACTWGT     ; ADDRESS OF COMP #N WEIGHT
        DECB
        ABX
        ABX
        ABX
        JSR     AMOVE           ; MOVE 1ST COMPONENT TO TABLE.

        LDAB    #EIGHT
        LDX     #CMP1ACTWGT     ; ADDRESS OF COMP #N WEIGHT
        DECB
        ABX
        ABX
        ABX
        JSR     CMOVE
        JSR     ADD             ; ADD THESE TWO COMPONENTS TOGETHER.
        JSR     CLAREG          ; CLEAR A
        LDAB    TEMPC           ;
        LDX     #CMP1ACTWGT     ; ADDRESS OF COMP #N WEIGHT
        DECB
        ABX
        ABX
        ABX
        JSR     EMOVE

; CONVERT WEIGHT TO IMPERIAL FORMAT.

        LDX     #CMP1ACTWGT     ; COMP #1 ACTUAL WEIGHT (METRIC)
        LDAB    TEMPC
        DECB
        ABX
        ABX
        ABX
        JSR    CNV3KGLB         ; CONVERT KGS TO LBS
        LDX     #CMP1ACTWGTI    ; COMP #1 ACTUAL WEIGHT (IMPERIAL)
        LDAB    TEMPC
        DECB
        ABX
        ABX
        ABX
        LDD     EREG+2          ; WEIGHT CONVERTED TO LBS
        STD     0,X
        LDAA    EREG+4          ;
        STAA    2,X             ; LSB OF LBS

        LDX     #CMP1ACTWGT     ; ADDRESS OF COMP #N WEIGHT
;        LDAB    TEMPC           ;
        LDAB    #EIGHT
        DECB
        ABX
        ABX
        ABX
        CLRW    0,X
        CLR     2,X             ;


NORMPCL:         ;


REPPERC JSR     CLRAC
        LDX     TEMPX           ; SEQUENCE TABLE POINTER.
        LDAB    0,X             ; READ CONTENTS
        ANDB    #$0F
        STAB    TEMPC           ; STORE COMPONENT NO.

        TST     REG_PRESENT     ; REGRIND USED ?
        BEQ     USEFULLWGT      ; USE TOTAL WEIGHT
        CMPB    REGCOMP         ; IS THIS THE REG COMP
        BEQ     USEFULLWGT      ; YES

        LDD     #$0006
        STD     AREG
        LDD     WEIGHTAFTREG    ;
        STD     AREG+2
        LDAA    WEIGHTAFTREG+2   ; RELOAD
        STAA    AREG+4          ; WT -> CREG XX XX X.Y 00 00
        BRA     CONPER1


USEFULLWGT:
        LDD     #$0006
        STD     AREG
        LDD     WEIGHTINHOP    ;
        STD     AREG+2
        LDAA    WEIGHTINHOP+2   ; RELOAD
        STAA    AREG+4          ; WT -> CREG XX XX X.Y 00 00


CONPER1:
        LDAB    TEMPC           ; RELOAD COMP NO
        LDX     #CMP1ACTWGT     ; ADDRESS OF COMP #N WEIGHT
        DECB
        ABX
        ABX
        ABX

        LDD     #$0006
        STD     MATHCREG

        LDD     0,X             ; COMP N WEIGHT -> CREG  XX XX X.Y 00 00
        STD     MATHCREG+2
        LDAA    2,X
        STAA    MATHCREG+4          ;

        JSR     FDIV             ; C/A CALCULATE % EREG -> 00 00 0X XX .YY
        JSR     FPTINC
        LDX    #MATHDREG+3
        LDAA   #2
        JSR    GPRND1            ;~ROUND A FURTHER 2 DIGITS


        LDX     #BPCNT1         ; %1 ADDRESS
        LDAB    TEMPC
        DECB
        ASLB
        ABX                     ; ADDRESS FOR DATA STORAGE
        LDD     EREG+1
        CPD     #$0100          ; 100 %
        BNE     NOT100          ; NO
        LDD     #$9999          ; 99.99 %
        BRA     STO9999
NOT100  LDD     EREG+2          ; READ ADDRESS OF DATA
STO9999 STD     0,X             ; STORE ACTUAL %



;       CHECK IF % ERROR IS > .5 % IF IT IS PRINT THE COMPONENT WEIGHTS
;
        JSR     CLRAC           ; A AND C REGISTER
        LDX     #EREG
        JSR     ITCREG          ; CALCULATED %
        LDX     #PCNT1          ;
        LDAB    TEMPC           ;
        DECB                    ;
        ASLB
        ABX
        LDD     0,X             ; READ % SET FOR THIS COMPONENT
        STD     AREG+3
        JSR     SUB             ; A-C % SET - % ACTUAL.
        LDD     EREG+2          ; CHECK
        BNE     PRINTWGT        ; PRINT WEIGHTS
        LDAA    EREG+4          ; CHECK DECIMAL % .Y
        CMPA    #05             ; %
        BLT     SKIPPRINT
PRINTWGT:

SKIPPRINT:
        LDAA    TEMPC
        CMPA    #4              ; IS THIS THE LAST COMPONENT ?
        BNE     NOLEAVE
        LDD     EREG+2
        BNE     PRINTWGT1
        LDAA    EREG+4
        CMPA    #3             ; OUTSIDE .3 %
        BLT     NOLEAVE
PRINTWGT1:

NOLEAVE LDX     TEMPX           ; COMP TRACKING COUNTER
        AIX     #1              ; POINTER INCREMENTED.
        LDAA    0,X
        CMPA    #SEQDELIMITER   ; AT END ?
        BEQ     EXIT            ; YES
        STX     TEMPX
        LBRA    REPPERC         ; CONTINUE
EXIT    JSR     CPYAPCTMB       ; COPY ACTUAL % TO MODBUS TABLE.
        RTS


*/
/*
CALHWTAV:
        LDX     #AN1AVGBUF      ; AVERAGE READING
        JSR     CALCVIBAVG      ; CALCULATE VIBRATION AVERAGE.
        BRA     MOVTOA
       LDX      #AN1BUF         ;
MOVTOA JSR    AMOVE            ;~COPY AVG TO AREG FOR MULTIP. BY C1
;       LDX    #CALBLK
;       STX    CDZTMP            ;~SAVE ADDR. OF CAL. DATA
;       LDAB   #OFFSOF           ;~OFFSET FOR CHANNEL ZERO (OR OFFSET)
;       ABX
       LDX      #HOPPERTARECOUNTS
       JSR    CMOVE             ;~COPY VALUE TO MATHCREG
       JSR    SUB               ;~AREG - MATHCREG  > RESULT IS NOW IN EREG
       TST    MATHDREG              ;~CHECK FOR NEG. RESULT
       BEQ    POSRSL            ;~0, RESULT POSITIVE
       LDX    #EREG
       JSR    CLRNM1            ;~SET EREG, = 00
       BRA    CHWTXX            ;~CONTINUE  BUT SKIP CALC. OF KGS
POSRSL LDX    #EREG
       JSR    ITAREG            ;~COPY RESULT OF SUBT'N TO AREG
       LDAB   #CON1OF
       LDX    #CALBLK
       ABX                        ;~X NOW POINTS TO C1 FOR THIS CHANNEL
       JSR    ITCRG4            ;~COPY 4 BYTE CONST TO MATHCREG
       JSR    MUL               ;~AREG * MATHCREG > EREG (I.E. KG)
CHWTXX RTS

CALCULATECOMPONENTTARGETS:
        LDX     SEQPOINTER
        STX     TEMP1S
REPTAC  LDX     TEMP1S
        AIX      #1
        STX      TEMP1S
        LDAA     0,X
        CMPA     #SEQDELIMITER
        BEQ      XITCMCL
        STAA     COMPNODETAILS
        ANDA     #$0F
        STAA     TARGNO
        STAA     COMPNO
        JSR      CALTARN
        JSR     CALCMNTIM       ; COMP #N TIMES FOR #2...N
        BRA      REPTAC
XITCMCL RTS


CALTARN JSR     CLRAC
        LDX     SEQPOINTER
        LDAB    0,X
        ANDB    #$0F
        PSHB
        TST     STAGEFILLEN             ; STAGED FILLING ENABLED ?
        BEQ     CHK1ST                  ; CHECK 1ST COMPONENT COMPENSATION
        TST     LAYERING                ; LAYERING 1ST COMPONENT ENABLED ?
        BEQ     CHK1ST
        CPX     #SEQTABLE               ; 1ST COMPONENT
        BNE     CHK1ST                  ; YES
        LDX     #CMP1CNTGSTR            ; USE STORED TARGET
        BRA     GETCOUNTS
CHK1ST:
;       TST     FIRSTCOMPCOMPENDIS      ; 1ST COMPONENT COMPENSATION.
;       BNE     USETAR                  ; USE TARGET RATHER THAN THE ACTUAL.

        TST     FILLALARM       ; HAS THE PREVIOUS COMP ALARMED ?
        BEQ     NOALARM         ; NO
USETAR  LDX     #CMP1CNTG       ; WEIGHT COUNTS FOR COMP #1
        BRA     GETCOUNTS

NOALARM TST     TOPUPF
        BEQ     PROCAN          ; PROCEED AS NORMAL.

        LDAA    TOPUPSEQUENCING ; CHECK SEQUENCING. AS THE REFERENCE.
        CMPA    #NORMALOPERATION; TOPPPING UP COLOUR
        BNE      NOTNRM

        PULB
        PSHB
        LDX     #CMP1CNAC       ; WEIGHT COUNTS FOR COMP #1
        DECB
        ABX
        ABX
        ABX                     ; 1ST COMPONENT USED.
        JSR     AMOVE           ;
        PULB
        PSHB

        LDX     #CMP1CNAC       ; WEIGHT COUNTS FOR COMP #1
        LDAB    #EIGHT
        DECB
;        ADDB    #4              ; SKIP TO TOP UP COMPONENT.
        ABX
        ABX
        ABX                     ; 1ST COMPONENT USED.
        JSR     CMOVE           ;
        JSR     ADD             ; ADD 1ST AND 5TH COMPONENTS.
        LDX     #EREG+2
        BRA     CALOTT          ; CALCULATE OTHER TARGETS USING THIS SETTING.


NOTNRM  TST     FIRSTWCYCLE      ; FIRST BATCH
        BNE     PROCAN          ; YES
        LDX     #CMP1CNTGTOT    ;
        BRA     GETCOUNTS
PROCAN  LDX     #CMP1CNAC       ; WEIGHT COUNTS FOR COMP #1


GETCOUNTS:
        DECB
        ABX
        ABX
        ABX                     ; 1ST COMPONENT USED.
CALOTT  JSR     AMOVE           ;
        JSR     CLCREG          ; RESET C REGISTER.

        LDX     #PCNT1          ; % N
        LDAB    TARGNO          ; CURRENT COMPONENT NO
        DECB                    ;
        ASLB                    ; X2
        ABX                     ; %N
        LDD     0,X             ;
        STD     MATHCREG+3          ;
        JSR     MUL             ; COMP #1 COUNTS X %N

        LDX     #MATHCREG           ;
        JSR     IFEREG          ; COMP1 A/D X %N
        JSR     CLAREG          ;
        LDX     #PCNT1          ; %1
        PULB
        DECB                    ; CALCULATE 1ST COMPONENT % POSITION.
        ASLB
        ABX
        LDD     0,X             ;
        STD     AREG+3          ; 1ST TO AREG
        JSR     DIV             ; C/A COMP1 A/D X %N / %1

;      WEIGHT CALCULATION REQUIRED HERE.

         LDX     #CMP1CNTG       ; LOAD COMP #1 TARGET COUNT STORAGE.
        LDAB    TARGNO          ; READ COMPONENT COUNTER
        DECB                    ;
        ABX
        ABX
        ABX
        STX     TEMPX           ; SAVE X

        JSR     EMOVE           ;
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
        JSR     CONVTARTOLBS

CALCTGWT LDX     #CMP1CNTG       ; COUNTS FOR COMPONENTS.
        LDAB    TARGNO          ; CURRENT COMPONENT.
        DECB
        ABX
        ABX
        ABX
        JSR     BCDHEX3X        ; CONVERT BACK TO HEX
        PSHB
        PSHM    E               ;SAVE RESULT
        LDX     #CMP1CNTGH      ; COUNTS FOR COMPONENTS.
        LDAB    TARGNO
        DECB
        ABX
        ABX
        ABX
        PULM    E
        PULB
        STAB    0,X             ; STORE HEX TARGET
        STE     1,X             ; LSB
        RTS                     ;

;

*/


/*
CALCVIBAVG:
        LDX     #AN1AVGBUF
        JSR     ITCREG4         ; MOVE AVERAGED COUNTS TO C REG.
        JSR     CLAREG
        LDAA    WGTAVGFACTOR
        STAA    AREG+4
        JSR     DIV             ;
        LDX     #EREG+2         ; RESULTING DATA.
        RTS

STORNCT
        JSR     CALCVIBAVG      ; CALCULATE VIBRATION AVERAGE.
        LDD     EREG+2
        STD     CURRLCAD        ; STORE CURRENT A/D
        LDAA    EREG+4
        STAA    CURRLCAD+2

        JSR     BCDHEX3X        ; TO HEX
        STAB    CURRLCADH       ; "CURRENT LOAD CELL A/D" HEX
        STE     CURRLCADH+1     ;
        JSR     STORERAWWEIGHT  ;

; NOW CALCULATE WEIGHT EREG - CURRLCADTARE
;
        LDX     #CURRLCAD
        JSR     AMOVE           ; CURRENT LOAD CELL A/D
        LDX     #CURRLCADTARE   ;
        JSR     CMOVE           ;
        JSR     SUB             ; A-C
        TST     MATHDREG         ; IS ANSWER NEGATIVE ?
        BPL     STRCNS           ; NO
        CLRW    EREG
        CLRW    EREG+2
        CLR     EREG+4          ; RESET RESULT
        ;
STRCNS  LDX     #CMP1CNAC       ; ACTUAL COUNTS #1
        LDAB    COMPNO          ; COMP NO
        DECB
        ABX
        ABX
        ABX

ADDSEL  JSR     EMOVE           ; STORE RESULTING COUNTS
;
;       WEIGHT CALCULATION PER COMPONENT
;
        JSR     CMPWCAL          ; CALCULATE COMPONENT WEIGHT
        LDX     #CMP1ACTWGT     ; COMPONENT #1 TARGET WEIGHT
        LDAB    COMPNO          ; READ COMPONENT COUNTER
        DECB                    ;
        ABX
        ABX
        ABX
        LDD     EREG
        STD     0,X             ; STORE WEIGHT
        LDAA    EREG+2          ;
        STAA    2,X

        LDX     #CMP1ACTWGT     ; COMP #1 ACTUAL WEIGHT (METRIC)
        LDAB    COMPNO
        DECB
        ABX
        ABX
        ABX
        JSR    CNV3KGLB         ; CONVERT KGS TO LBS
        LDX     #CMP1ACTWGTI    ; COMP #1 ACTUAL WEIGHT (IMPERIAL)
        LDAB    COMPNO
        DECB
        ABX
        ABX
        ABX
        LDD     EREG+2          ; WEIGHT CONVERTED TO LBS
        STD     0,X
        LDAA    EREG+4          ;
        STAA    2,X             ; LSB OF LBS

        JSR     CALCRROBINFORWEIGHT ; CALCULATE ROUND ROBIN WEIGHT.

        JSR     CPYHWTSMB       ; STORE HOPPER WEIGHT
        RTS
*/

/*
; CHECK IF LOW LEVEL SENSOR IS ACTIVE IF SO WAIT UNTIL IT IS COVERED.

CHECKLLS:
        TST     PAUSFLG         ; PAUSE MODE
        BNE     XITLL
        TST     FITHAIF         ; VACUUM PCB CONNECTED.
        BEQ     XITLLS          ; NO
XITLLS  LDAA    #FILL
        STAA    BATSEQ          ; STORE
        STAA    CYCLEINDICATE   ; FILLLING IN PROGRESS
        LDAA    #1
        STAA    ACTIVATELATENCY
        TST     RETRYACTIVE     ; ARE WE RETRYING
        BNE     XITLL           ; YES EXIT
        TST     STAGEFILLEN             ; STAGED FILL ENABLE
        BEQ     XITLL           ; YES EXIT
;       CHECK IF LAYERING IS NOT REQUIRED
;;T
        TST     LAYERING        ; LAYERING REQUIRED
        BNE     STGFAN          ; STAGED FILL AS NORMAL
        LDX     SEQPOINTER
        CPX     #SEQTABLE       ; 1ST COMPONENT BEING DISPENSED
        BEQ     ACCL            ; ACTIVATE CALCULATION FOR COMPONENT 1 AGAIN.
        TST     TOPUPF          ; CHECK FOR TOP UP
        BEQ     DOSTFL          ; DO STAGED FILL
        LDAA    TOPUPSEQUENCING ; IS THE TOP UP IN PROGRESS ?
        BNE     XITLL           ; YES IT IS.
DOSTFL  TST     MULTIPLEFILLING ; IS MULTIPLE FILLING ACTIVATED
        BNE     XITLL

STGFAN  LDAA    #1
        STAA    STAGEFILLCALC   ; SET CALCULATION FLAG
XITLL   RTS

ACCL    TST     TOPUPSEQUENCING
        BNE     ISTOPU
        JSR     COMP1STXTOTWGT
ISTOPU  BRA     XITLL           ; ENSURE THAT TARGET FOR COMPONENT 1 IS CALCULATED AGAIN.


*/
/*
NOTCALC CMPA    #PAUSEBFILL     ; PAUSED MODE
        BNE     QITMONB
        TST     PAUSFLG         ; IS THIS PAUSE MODE ?
        BNE     QITMONB         ; PAUSE MODE
        TST     PTONORMAL       ; BACK TO NORMAL
        BEQ     QITMONB         ; NO
        CLR     PTONORMAL       ; RESET FLAG
        JSR     CHECKLLS        ; RE ENABLE FILLING MODE.
        CLR     FILLALARM       ; RESET COMP FILL ALARM
        JSR     RESETFILLCOUNTERS ; RESET FILL COUNTER
        CLR     FILLCTR
        CLR     FILLCOUNTER     ; RESET FILL COUNTER
QITMONB RTS

*/

/*
;*****************************************************************************;
; FUNCTION NAME : RESETFILLCOUNTERS                                           ;
; FUNCTION      : THIS IS CALLED ON THE 1ST FILL.                             ;                                          ;                                          ;
; INPUTS        : NONE                                                        ;                       ;
;*****************************************************************************;


RESETFILLCOUNTERS:
        CLR     RETRYCOUNTER
        CLR     RETRYACTIVE
        RTS


MONBAT  LDAA    BATSEQ          ; READ SEQUENCE FLAG
        CMPA    #CALC           ; SHOULD BE REST OF THE CALCULATIONS BE RUN
        LBNE     NOTCALC         ; NO
        TST     AVGAVAIL        ;
        LBEQ     NOCAL           ; AVERAGE NO AVAILABLE
        CLR     AVGAVAIL        ; RESET AVERAGING FLAG
        LDAA    TOPUPF           ; IS TOP UP ENABLED
        BEQ     RDTBASN         ; READ TABLE AS NORMAL.

        LDAA    TOPUPSEQUENCING
        CMPA    #FILLCOLOUR
        BEQ     ISCOLOR         ; YES.
        CMPA    #TOPUPCOLOUR    ; TOP UP
        BEQ     NOTNRL

        CMPA    #NORMALOPERATION ; INDICATE NORMAL OPERATION.
        BNE     RDTBASN         ; NO STORAGE OF DATA.
        LDX     SEQPOINTER
        LDAA    0,X
        STAA    COMPNODETAILS
        ANDA    #$0F            ; COMP NO
        STAA    COMPNO          ; STORE COMP NO
        LBRA     NOSTORD         ; NO


NOTNRL  TST     NOTOPUPREQUIRED ; NO TOP UP REQUIRED
        LBNE     NOSTORD         ; NO STORAGE OF DATA.
        LDX     SEQPOINTER
        LDAA    0,X
        ANDA    #$0F            ; ISOLATE COMPONENT NO.
        LDAA    #EIGHT
        BRA     STRCMP          ; STORE COMPONENT NO.

ISCOLOR LDX     LASTCOMPTABLEP  ; READ LAST COMPONENT.
        BRA     READT           ; READ TABLE.
RDTBASN LDX     SEQPOINTER
READT   LDAA    0,X
STRCMP  STAA    COMPNODETAILS
        ANDA    #$0F            ; COMP NO
        STAA    COMPNO          ; STORE COMP NO
        JSR     STORNCT         ; STORE THE SETTLE COUNT
        TST     ISVOLMODE
        BNE     VOLM            ; NO RETRY IF VOLUMETRIC MODE
        TST     MOVEON          ; SHOULD WE MOVE ON
        BEQ     CHECK
        CLR     MOVEON
VOLM    JSR     STORECOMPONENTTARE
        BRA     CALCNOW
CHECK:
        JSR     CALCULATESECONDS
        JSR     CHKFILLACC
        TSTA                    ; CHECK RESULT.
        BNE     FILLERR         ; ERROR IN FILLING.
        JSR     STORECOMPONENTTARE
        BRA     CALCNOW

FILLERR LDAB    COMPNO          ; READ COMPONENT NO.
        LDAA    #2              ; RESET THE ROUND ROBIN TWICE
        JSR     RESETWGTRROBIN
        JSR     CALCRROBINFORWEIGHT
        TST     CHANGINGORDER   ; % CHANGE.
        LBEQ     COMPAGAIN       ; RETRY IF NOT CHANGING ORDER.

CALCNOW CLR     CORRECTRETRIES
        JSR     CALCNTPI        ; COUNT PER INTERRUPT CALULATED FOR COMP #N

        TST     STAGEFILLEN
        BEQ     NOPRI           ; NO DIAGNOSTICS PRINT ON EACH COMP
;;T
;        TST     PURGECOMPNO
;        BEQ     NOPRI
        JSR     BPCNTNCAL
        JSR     BBDIAPRN

NOPRI:

        JSR     STORESTAGEFILLDATA

        TST     MULTIPLEFILLING
        LBNE    GOTONEXTCOMP

NOSTORD LDAA    CALCROUTER      ; CHECK WHICH SECTION TO RUN
        CMPA    #CALCMPNTAR     ; CALCULATE REST OF TARGETS ?
        BEQ     CALCREST
        CMPA    #CALC1STCMPTAR  ; 1ST COMP CALCULATION REQUIRED
        LBNE     CHKCPI
        JSR     CALCWGTAFTREG   ; WEIGHT AFTER REGRIND AND NEXT TARGET
        LDX     SEQPOINTER
        LDAA    0,X
        STAA    COMPNODETAILS
        ANDA    #$0F            ; COMP NO
        STAA    COMPNO          ; STORE COMP NO

        JSR     CALCMNTIM       ; CALCULATE COMPONENT #1 UPDATED TIME
        LDAA    #$0F
        STAA    FSTCOMPCALFIN   ; INDICATE THAT THIS IS FINISHED.
        JMP     GOTONEXTCOMP    ; NEXT COMPONENT.



;

;       CALCULATE CMP2,,,,N

CALCREST:
        JSR     CALCMNTIM       ; CALCULATE COMPONENT #1 UPDATED TIME
        TST     TOPUPF
        BEQ     NOTTUP1         ; NO TOP UP.

        LDAA    TOPUPSEQUENCING
        CMPA    #FILLFIRST      ; FIRST COMPONENT FILLED ?
        BNE     NOT1STF         ; NO
        LDX     SEQPOINTER
        LDAA    1,X             ; READ TABLE CONTENTS
        CMPA    #SEQDELIMITER   ; ONLY ONE COMPONENT
        LBEQ    ENDCYCLE        ; SINGLE COMPONENT

        LDAA    #FILLCOLOUR     ; FILL THE COLOUR NEXT.
        STAA    TOPUPSEQUENCING ; SET THE SEQUENCING.
        JSR     CALCULATECOMPONENTTARGETS
        JSR     FINDCOLOURCOMPONENT ; SET COLOUR COMPONENT POINTER.
        LBRA    INCTONX        ; NO INCREMENT

NOT1STF LDAA    TOPUPSEQUENCING
        CMPA    #FILLCOLOUR     ; IS THE COLOUR BEING FILLED ?
        BNE     NOTCLF          ; NOT COLOUR FILLING..
        JSR     CALCULATECOLOURCOMPENSATION  ; CALCULATE TOP UP.
        TST     NOTOPUPREQUIRED
        LBEQ    INCTONX         ; TOP UP AS NORMAL
        LBRA    XITCYC          ; NO INCREMENT IF TOP UP.
NOTCLF  LDAA    TOPUPSEQUENCING
        CMPA    #TOPUPCOLOUR    ; HAS THE TOPUP TAKEN PLACE.
        BNE     NOTTUP1         ; NO
        LDAA    #NORMALOPERATION ; INDICATE NORMAL OPERATION.
        STAA    TOPUPSEQUENCING ; SEQUENCE INDICATED.

;       CHECK IF THIS IS THE LAST COMPONENT.
;
        LDX     SEQPOINTER
        LDAA    2,X
        CMPA    #SEQDELIMITER   ; IS THIS THE LAST COMPONENT
        LBEQ     ENDCYCLE          ; YES END CYCLE.

;       LDAA    #1
;       STAA    NOTOPUPREQUIRED
        LBRA    XITCYC          ; NO FILLING REQUIRED



NOTTUP1:
;        JSR     CALCMNTIM       ; CALCULATE COMPONENT #1 UPDATED TIME

        JSR     CALCULATECOMPONENTTARGETS
        LDAA    TOPUPSEQUENCING
        CMPA    #NORMALOPERATION        ; IS THIS THE NORMAL OPERATION.
        BNE     FILLNOW                 ; NO
        CLR     TOPUPSEQUENCING         ; RESET
        BRA     NOTUP                   ; GO TO NEXT.


; DECIDE ON THE NEXT COMPONENT TO BE FILLED

; NOTE NOT INCREMENT UNLESS NORMAL OPERATION IS SET.


FILLNOW:

        TST     TOPUPF
        BEQ     NOTUP          ; NORMAL OPERATION.
        LDAA    TOPUPSEQUENCING ;
        CMPA    #NORMALOPERATION ; BACK TO NORMAL OPERATION ?
        BEQ     NOTUP           ; YES
        PULM    X
        LBRA     INCTONX          ; NO CHANGE TO TABLE POINTER


NOTUP   LDX     SEQPOINTER
        AIX     #1
        LDAA    0,X             ; READ TABLE CONTENTS
        CMPA    #SEQDELIMITER   ; IS IT DELIMITED ?
        BEQ     ENDCYCLE        ; SINGLE COMPONENT
        STX     SEQPOINTER      ; UPDATE SEQTABLE POINTER
SKIPINCR:
        CLR     FILLCOUNTER     ; RESET FILL COUNTER
        CLR     FILLALARM       ; RESET COMP FILL ALARM
        JSR     RESETFILLCOUNTERS ; RESET FILL COUNTER
        CLR     FILLCTR         ; RETRYFILL = 0
        TST     PAUSFLG         ; PAUSED ?
        BEQ     NOPAUS1         ; NO
        LDAA    #PAUSEBFILL     ; IS SET
        STAA    BATSEQ          ; INDICATE PAUSED
        BRA     NOCAL           ; EXIT
NOPAUS1 JSR     CHECKLLS        ; CHECK LOW LEVEL SENSOR IF APP
NOCAL   RTS







CHKCPI:
        LDX     SEQPOINTER      ;
        CPX     #SEQTABLE       ; IS THIS THE BEGINNING OF THE TABLE. ?
        BNE     GOTONEXTCOMP    ; THIS IS ANOTHER COMPONENT.
        LDAA    1,X             ; IS THERE ONLY 1 COMPONENT
        CMPA    #SEQDELIMITER   ;
        BEQ     GOTONEXTCOMP    ; CALCULATION OF NEXT TARGET.
        JSR     CALNEXTAR       ; CALCULATE NEXT TARGET

GOTONEXTCOMP:

        LDX     SEQPOINTER      ;
        AIX     #1
        STX     SEQPOINTER
        TST     TOPUPF           ; TOP UP ENABLED ?
        BEQ     USELSTC         ; USE LAST COMPONENT
        LDAA    1,X             ; DO NOT FILL LAST COMPONENT.
        BRA     COMPDEL         ; COMPARE DELIMITER.
USELSTC LDAA    0,X
COMPDEL CMPA    #SEQDELIMITER   ; IS THIS THE FINAL COMPONENT ?
        BEQ     ENDCYCLE
        JSR     CHKFORSKIPPEDCOMP
        LDAA    0,X
        CMPA    #SEQDELIMITER   ; IS THIS THE FINAL COMPONENT ?
        BEQ     ENDCYCLE        ; END CYCLE
        LBRA    INCTONX         ; INCREMENT TO NEXT BLEND


ENDCYCLE:
        JSR     CHKFOREOCPAUSE  ; CHECK FOR END OF CYCLE PAUSE.
        JSR     CHKFORRETRIES   ; CHECK FOR RETRIES
        LDX     #SEQTABLE
        STX     SEQPOINTER      ; RESET TO START OF TABLE

        INCW    CYCLECOUNTER    ; INDICATE THAT CYCLE HAS FINISHED.
        JSR     BPCNTNCAL
        TST     LEVELSENALOC    ; HAS THERE BEING AN ALARM.
        BNE     NOLSAL          ; YES
        LDAA    MBPROALRM       ; CHECK ALARM WORD.
        ANDA    #LEVELSENALBIT  ; LEVEL SENSOR ALARM BIT
        BEQ     NOLSAL          ; ALARM HAD BEING CLEARED.
        LDAA    #LEVELSENSORALARM
        JSR     RMALTB          ; RESET ALARM.
NOLSAL:
        TST     PAUSFLG         ; ARE WE IN PAUSE MODE ?
        BNE     ISPAUSED        ; PAUSED EXIT
        TST     PAUSETRAN       ; TRANITION FLAG
        BEQ     ISPAUSED
        TST     DMPLEVSEN       ; IS LEVEL SENSOR UNCOVERED ?
        BNE     ISPAUSED
        CLR     PAUSETRAN       ; PROCEED NO PROBLEMS.
ISPAUSED:
        TST     PAUSFLG
        BNE     CONCYC         ; IS PAUSED
        LDAA    BLENDERMODE
        CMPA    #THROUGHPUTMON ;
        BNE     CONCYC
        TST     LEVSEN1STAT       ; LEVEL SENSOR STATUS
        BNE     CONCYC
        LDAA    #LEVELSENSORALARM
        JSR     PTALTB          ; TO ALARM TABLE
CONCYC  LDAA    #LEVPOLL
        STAA    BATSEQ          ; SET "LEVEL SENSOR POLLING SEQUENCE"         ;
        CLR     CYCLEINDICATE   ; RESET TO INDICATE THE END OF THE CYCLE.
        CLR     FIRSTWCYCLE
        CLR     CHANGINGORDER
        LDD     CYCLETIME
        STD     CYCLELOADTIMEPRN
        JSR     CHECKFORSTAGEDFILL
        TST     MULTIPLEFILLING ; MULTIPLE FILLING
        BNE     XITAL1          ; YES
        LDAA    #2
        STAA    PRINTINNSECONDS
;        LDAA    #1
;        STAA    PRINTNOW
XITAL1  RTS


INCTONX:
        CLR     FILLALARM       ; RESET COMP FILL ALARM
        CLR     FILLCTR
        JSR     RESETFILLCOUNTERS ; RESET FILL COUNTER
SAME1   CLR     FILLCOUNTER     ; RESET FILL COUNTER
        TST     PAUSFLG         ; PAUSED ?
        BEQ     NOPAUS2         ; NO
        LDAA    #PAUSEBFILL     ; IS SET
        STAA    BATSEQ          ; INDICATE PAUSED
        BRA     XITCYC          ; EXIT
NOPAUS2
        JSR     CHECKLLS        ; CHECK LOW LEVEL SENSOR IF APP
XITCYC  RTS



COMPAGAIN:

RETRYCM:

ISREFL  TST     CORRECTRETRIES
        BNE     ISRET           ; IT IS RETRYING.
        LDAB    COMPNO          ; LOAD COMPONENT NO.
        JSR     SETTOIGNORECPI
ISRET   CLR     CORRECTRETRIES
        LDAA    #$0F
        STAA    PRINTNOW        ; SET PRINTING FLAG   ; PRINT OUT REPORT SO FAR.
        LDAA    COMPNO
        STAA    ANYRETRY        ; SET ANY RETRY FLAG.
NORETPR LDAA    CYCLEPAUSETYPE  ;
        CMPA    #PAUSEATENDOFCYCLE
        BNE     NOTEOC           ; NO
        JSR     IMDPAUSEON       ; PAUSE BLENDER FULLY.
        CLR     CYCLEPAUSETYPE
NOTEOC  LBRA     SAME1



NOTCALC CMPA    #PAUSEBFILL     ; PAUSED MODE
        BNE     QITMONB
        TST     PAUSFLG         ; IS THIS PAUSE MODE ?
        BNE     QITMONB         ; PAUSE MODE
        TST     PTONORMAL       ; BACK TO NORMAL
        BEQ     QITMONB         ; NO
        CLR     PTONORMAL       ; RESET FLAG
        JSR     CHECKLLS        ; RE ENABLE FILLING MODE.
        CLR     FILLALARM       ; RESET COMP FILL ALARM
        JSR     RESETFILLCOUNTERS ; RESET FILL COUNTER
        CLR     FILLCTR
        CLR     FILLCOUNTER     ; RESET FILL COUNTER
QITMONB RTS


NOTFILLPROBLEM:
        RTS


mce12-15





MAXRETRYCYCLES          EQU     5
TENTHOUSAND             EQU     10000
MINDAPKGCALC            EQU     205     ; 5% OF MAX NO D/A PER KG CALCULATION BELOW THIS.
MINFLOWRATE             EQU     0700

MULTIPLIERLSWDOG        EQU     15      ; MULTIPLIER FOR LEVEL SENSOR WATCHDOG


LOWTARGETLIMIT          EQU     $40
LOWERCOMPPERLIMIT       EQU     $20      ; LOWER COMPONENT % LIMIT.
HIGHERCOMPPERLIMIT      EQU     $10      ; HIGHER COMPONENT % LIMIT.
MAXRETRYNO              EQU     2        ; NO OF RETRIES BEFORE INTRODUCING MINIMUM FILL


VENTURIBANDLIMIT        EQU     $0050    ; ALLOW HIGHER BAND BELOW XX GRAMS


MINIMUMGRAMLIMIT        EQU     $200      ; 20 GRAMS
MINSLIDEVALVEOPENTIME   EQU     40




BANK2PROGS      SECTION

CALTAR1 RTS


DIV1    LDX     EREG+3          ;
        STX     MATHCREG+2
        LDX     EREG+1          ; X BY 100
        STX     MATHCREG
        CLR     MATHCREG+4

        LDX     #CALBLK         ; CALIBRATION BLOCK START ADDRESS
        LDAB    #CON1OF         ; CALIBRATION CONSTANT POSITION
        ABX
        JSR     A4MOVE
        JSR     DIV             ;C/A  %1 x TARGET WEIGHT x100 / CAL CONSTANT
        RTS




;    1ST COMP X TOTAL BATCH WEIGHT.
;

COMP1STXTOTWGT:
        LDX     #SEQTABLE
        LDAA    0,X
        TAB
        STAB    COMPNODETAILS
        ANDB    #$0F
        STAB    COMPNO
        LDX     #PCNT1
        DECB
        ASLB
        ABX                     ; % CALCULATION ON THIS COMPONENT.
        LDX     0,X             ; READ ACTUAL %
        LDE     DFTW1           ; TOTAL BATCH WEIGHT
        LDAA    DFTW1+2         ; LSB OF TARGET
CALTARGETN:
        JSR     CMPNTAR           ; CALCULATE COMP ONE TARGET. 100%

;       STORE WEIGHT TO CMPNCNTGTOT
;
        LDAA    SEQTABLE
        ANDA    #REGBIT         ; IS THERE REGRIND ?
        BNE     NOTU            ; NO TOP UP CALCULATION AT THIS STAGE.
        TST     TOPUPF          ; TOP UP ENABLED ?
        BEQ     NOTU            ; NO
        LDE     DFTW1           ; TOTAL BATCH WEIGHT
        LDAA    DFTW1+2         ; LSB OF TARGET
        JSR     CALCULATETOPUPTARGET    ; CALCULATE THE NEW BATCH WEIGHT
        JSR     CMPNTAR           ; CALCULATE COMP ONE TARGET. 100%
NOTU    TST     FIRSTWCYCLE
        BNE     FIRSTPASS       ; FIRST CYCLE.
        JSR     CALCMNTIM       ; CALCULATE TIME FOR 1ST COMP IF FILL BY TIME.
FIRSTPASS:
        RTS



; CALCULATE THE TOP UP BATCH WEIGHT
;   BATCH WEIGHT PASSED IN E/A * PERTOPUP (% TOPUP) - RETURNED IN E/A


CALCULATETOPUPTARGET:
        STE     TEMPBATCHWEIGHT ; STORE BATCH WEIGHT TEMPORARILY
        STAA    TEMPBATCHWEIGHT+2
        LDAA    #FILLFIRST      ; FILL THE FIRST COMPONENT.
        STAA    TOPUPSEQUENCING
        LDAB    #RAMBANK
        TBZK
        LDZ     #CMP1CNTG       ; STORE 100% VALUE
        LDAB    COMPNO          ; COMPONENT NO
        DECB
        ABZ
        ABZ
        ABZ
        LDX     #CMP1CNTGTOT    ; STORE 100% VALUE
        LDAB    COMPNO          ; COMPONENT NO
        DECB
        ABX
        ABX
        ABX
        LDD     0,Z
        STD     0,X             ; TARGET FOR COMPONENT #N
        LDAA    2,Z          ;
        STAA    2,X             ; LSB

        LDX     #CMP1CNTGTOT    ; STORE 100% VALUE
        LDAB    COMPNO          ; COMPONENT NO
        DECB                    ;
        ABX
        ABX
        ABX
        CLRW    EREG
        LDD     0,X
        STD     EREG+2
        LDAA    2,X
        STAA    EREG+4          ; NEXT CALCULATION EXPECTING COUNTS IN EREG.
        JSR     CMPWCAL         ; CACULATE WEIGHT FOR THIS COMPONENT
        LDX     #CMP1SETTOPUPWT ; COMPONENT #1 TARGET WEIGHT
        LDAB    COMPNO          ; COMPONENT NO
        DECB                    ;
        ABX
        ABX
        ABX
        LDD     EREG
        STD     0,X             ; STORE WEIGHT
        LDAA    EREG+2          ;
        STAA    2,X             ;
        LDX     #CMP1SETTOPUPWT ; COMPONENT #1 TARGET WEIGHT
        LDAB    COMPNO          ; COMPONENT NO
        DECB                    ;
        ABX
        ABX
        ABX
        JSR    CNV3KGLB         ; CONVERT KGS TO LBS
        LDX     #CMP1SETTOPUPWTI ; COMPONENT #1 TARGET WEIGHT
        LDAB    COMPNO          ; COMPONENT NO
        DECB                    ;
        ABX
        ABX
        ABX
        LDD     EREG+2          ; WEIGHT CONVERTED TO LBS
        STD     0,X
        LDAA    EREG+4
        STAA    2,X             ; LSB OF LBS
        JSR     CLRAC
;       LDE     DFTW1           ; TOTAL BATCH WEIGHT
        LDE     TEMPBATCHWEIGHT ; STORE BATCH WEIGHT TEMPORARILY
        STE     AREG+2
;        LDAA    DFTW1+2         ; LSB OF TARGET
        LDAA    TEMPBATCHWEIGHT+2
        STAA    AREG+4
        CLRA
        LDAB    PERTOPUP        ; READ % FROM HERE FOR NOW.
        STD     MATHCREG+3
        JSR     MUL             ; MULTIPLY
        LDAB    COMPNO
        LDX     #PCNT1
        DECB
        ASLB
        ABX                     ; % CALCULATION ON THIS COMPONENT.
        LDX     0,X             ; READ ACTUAL %
        LDE     EREG+1          ; TOTAL BATCH WEIGHT
        LDAA    EREG+3          ; LSB OF TARGET
        RTS




  ;
;       CALCULATE WEIGHT AFTER REGRIND.
;       (ACTUAL WEIGHT / REG %)   * REMAINDER 100 -

;
CALCWGTAFTREG:

        TST     USEREGRINDASREF     ; SHOULD THE REGRIND BE USED AS THE REFERENCE.
        LBEQ    OLDTRGC             ; NO.
        TST     ANYFILLBYWEIGHT     ; WAS THE LAST COMPONENT FILLED BY WEIGHT?
        LBNE    OLDTRGC
        TST     REGRINDFILLPROBLEM
        LBNE    OLDTRGC
        JSR     CLRAC7
        LDX     SEQPOINTER
        LDAB    0,X             ;
        ANDB    #$0F            ; COMP NO
        PSHB                    ; SAVE
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

        PSHM    E
        PSHA                    ; SAVE NEW TARGET WEIGHT FOR LATER USE.
        LDX     SEQPOINTER
        AIX     #1
        LDAB    0,X             ;
        STAB    COMPNODETAILS   ; COMP NO DETAILS STORED.
        ANDB    #$0F            ; COMP NO
        STAB    COMPNO
        JSR     CMPNTAR         ; COMPONENT TARGET CALCULATION
        JSR     CALCMNTIM       ; CALCULATE TIME ANYWAY.
        PULA
        PULM    E               ; RECOVER TARGET WEIGTH
        TST     TOPUPF          ; TOP UP ENABLED ?
        BEQ     NOTU1           ; NO
        JSR     CALCULATETOPUPTARGET    ; CALCULATE THE NEW BATCH WEIGHT FOR TOP UP
        JSR     CMPNTAR         ; COMPONENT TARGET CALCULATION
        JSR     CALCMNTIM       ; CALCULATE TIME ANYWAY.
NOTU1   RTS


OLDTRGC:
        JSR     CLRAC
        LDX     SEQPOINTER
        LDAB    0,X             ;
        ANDB    #$0F            ; COMP NO
        LDX     #CMP1ACTWGT     ;
        DECB
        ABX
        ABX
        ABX                     ; ACCESS WEIGHT
        LDAA    0,X
        LDE     1,X
        LSRA
        RORE
        LSRA
        RORE
        LSRA
        RORE                    ; SHIFT 1 DIGIT FOR GRAMS
        LSRA
        RORE                    ;
        STAA    MATHCREG+2
        STE     MATHCREG+3
        LDX     DFTW1           ; TOTAL BATCH WEIGHT
        STX     AREG+2
        LDAA    DFTW1+2
        STAA    AREG+4          ; TARGET BATCH WEIGHT
        JSR     SUB             ; A-C
        LDX     #TARWGTAFTREG   ; WEIGHT LEFT AFTER REGRIND HAS FINISHED.
        JSR     EMOVE
        LDE     TARWGTAFTREG
        LDAA    TARWGTAFTREG+2

        PSHM    E
        PSHA                    ; SAVE NEW TARGET WEIGHT FOR LATER USE.
        LDX     SEQPOINTER
        AIX     #1
        LDAB    0,X             ;
        STAB    COMPNODETAILS   ; COMP NO DETAILS STORED.
        ANDB    #$0F            ; COMP NO
        STAB    COMPNO
        JSR     CMPNTAR         ; COMPONENT TARGET CALCULATION
        JSR     CALCMNTIM       ; CALCULATE TIME ANYWAY.
        PULA
        PULM    E               ; RECOVER TARGET WEIGTH
        TST     TOPUPF          ; TOP UP ENABLED ?
        BEQ     NOTU2           ; NO
        JSR     CALCULATETOPUPTARGET    ; CALCULATE THE NEW BATCH WEIGHT FOR TOP UP
NOTU2   LDX     SEQPOINTER
        JSR     CMPNTAR         ; COMPONENT TARGET CALCULATION
        JSR     CALCMNTIM       ; CALCULATE TIME ANYWAY.
        RTS










CMPNTAR:
        JSR     CLRAC
        LDX     #PCNT1
        LDAB    COMPNO
        DECB
        ASLB
        ABX
        LDX     0,X             ; READ %
        STX     AREG+3          ;   % CALCULATION.
        STE     MATHCREG+2
        STAA    MATHCREG+4      ; STORE GRAMS SECTION
        JSR     MUL             ;  %#1 X TARGET WEIGHT

        LDX    #EREG
        LDAB   #5
        LDAA   #1
        JSR    GPSFTR            ;~RESULT


        LDX     #CMP1TARWGT     ; COMP #1 TARGET WEIGHT STORAGE
        LDAB    COMPNO
        DECB
        ABX
        ABX
        ABX
        LDD     EREG+1
        STD     0,X
        LDAA    EREG+3
        STAA    2,X             ;
        LDX     #TEMP1S           ;
        JSR     IFEREG          ; STORE TARGET IN GRAMS
        JSR     DIV1            ; CALCULATE COUNTS FOR COMP #1
        LDX     #CMP1CNTG        ; TARGET FOR COMPONENT #1
        LDAB    COMPNO          ; COMPONENT NO
        DECB
        ABX
        ABX
        ABX
        LDD     EREG+2
        STD     0,X             ; TARGET FOR COMPONENT #N
        LDAA    EREG+4          ;
        STAA    2,X             ; LSB
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
        JSR     CONVTARTOLBS
        RTS



CONVTARTOLBS:
        LDX     #CMP1TARWGT     ; COMP #1 TARGET WEIGHT STORAGE
        LDAB    COMPNO
        DECB
        ABX
        ABX
        ABX
        JSR    CNV3KGLB         ; CONVERT KGS TO LBS
        LDX     #CMP1TARWGTI     ; COMP #1 TARGET WEIGHT STORAGE
        LDAB    COMPNO
        DECB
        ABX
        ABX
        ABX
        LDD     EREG+2          ; WEIGHT CONVERTED TO LBS
        STD     0,X
        LDAA    EREG+4          ;
        STAA    2,X             ; LSB OF LBS
METRIC  RTS




;*****************************************************************************;
; FUNCTION NAME : CALTARN                                          ;          ;
; FUNCTION      : TARGET COUNTS #N = (TARGET #1 / %1)*(%N)                    ;
;               ; CALCULATES TARGETS FOR COMPONENTS OTHER THAN THE MAIN COMP  ;
;               ; IF FIRSTCOMPCPMENDIS FLAG IS SET, THEN USE THE TARGET AS    ;
;               ; THE REFERENCE.                                              ;
;               ; WEIGHT FOR COMPONENTS ARE ALSO CALCULATED                   ;
                                                                              ;
                                                                              ;
; INPUTS        : CMPNCNTG TARGETS, FIRSTCOMPCOMPENDIS                        ;                                                         ;                       ;
;*****************************************************************************;

;

CALTARN JSR     CLRAC
        LDX     SEQPOINTER
        LDAB    0,X
        ANDB    #$0F
        PSHB
        TST     STAGEFILLEN             ; STAGED FILLING ENABLED ?
        BEQ     CHK1ST                  ; CHECK 1ST COMPONENT COMPENSATION
;        TST     LAYERING                ; LAYERING 1ST COMPONENT ENABLED ?
;        BEQ     CHK1ST
        LDAA    SEQTABLE                ;
        ANDA    #REGBIT                 ; IS THIS A REGRIND COMPONENT COMPONENT ?
        BEQ     DONRCM                  ; DO NORMAL COMPARISON
        CPX     #SEQTABLE+1             ; 1ST COMPONENT AFTER REGRIND ?
        BRA     DOCCCM                  ; DO COMPONENT COMPARE
DONRCM  CPX     #SEQTABLE               ; 1ST COMPONENT
DOCCCM  BNE     CHK1ST                  ; YES
        LDX     #CMP1CNTGSTR            ; USE STORED TARGET
        TST     LAYERING                ; LAYERING 1ST COMPONENT ENABLED ?
        BEQ     CHK1ST

        BRA     GETCOUNTS
CHK1ST:
;       TST     FIRSTCOMPCOMPENDIS      ; 1ST COMPONENT COMPENSATION.
;       BNE     USETAR                  ; USE TARGET RATHER THAN THE ACTUAL.

        TST     FILLALARM       ; HAS THE PREVIOUS COMP ALARMED ?
        BEQ     NOALARM         ; NO
USETAR  LDX     #CMP1CNTG       ; WEIGHT COUNTS FOR COMP #1
        BRA     GETCOUNTS

NOALARM TST     TOPUPF
        BEQ     PROCAN          ; PROCEED AS NORMAL.

        LDAA    TOPUPSEQUENCING ; CHECK SEQUENCING. AS THE REFERENCE.
        CMPA    #NORMALOPERATION; TOPPPING UP COLOUR
        BNE      NOTNRM

        PULB
        PSHB
        LDX     #CMP1CNAC       ; WEIGHT COUNTS FOR COMP #1
        DECB
        ABX
        ABX
        ABX                     ; 1ST COMPONENT USED.
        JSR     AMOVE           ;
        PULB
        PSHB

        LDX     #CMP1CNAC       ; WEIGHT COUNTS FOR COMP #1
        LDAB    #EIGHT
        DECB
;        ADDB    #4              ; SKIP TO TOP UP COMPONENT.
        ABX
        ABX
        ABX                     ; 1ST COMPONENT USED.
        JSR     CMOVE           ;
        JSR     ADD             ; ADD 1ST AND 5TH COMPONENTS.
        LDX     #EREG+2
        BRA     CALOTT          ; CALCULATE OTHER TARGETS USING THIS SETTING.


NOTNRM  TST     FIRSTWCYCLE      ; FIRST BATCH
        BNE     PROCAN          ; YES
        LDX     #CMP1CNTGTOT    ;
        BRA     GETCOUNTS
PROCAN  LDX     #CMP1CNAC       ; WEIGHT COUNTS FOR COMP #1
GETCOUNTS:
        DECB
        ABX
        ABX
        ABX                     ; 1ST COMPONENT USED.
CALOTT  JSR     AMOVE           ;
        JSR     CLCREG          ; RESET C REGISTER.

        LDX     #PCNT1          ; % N
        LDAB    TARGNO          ; CURRENT COMPONENT NO
        DECB                    ;
        ASLB                    ; X2
        ABX                     ; %N
        LDD     0,X             ;
        STD     MATHCREG+3          ;
        JSR     MUL             ; COMP #1 COUNTS X %N

        LDX     #MATHCREG           ;
        JSR     IFEREG          ; COMP1 A/D X %N
        JSR     CLAREG          ;
        LDX     #PCNT1          ; %1
        PULB
        DECB                    ; CALCULATE 1ST COMPONENT % POSITION.
        ASLB
        ABX
        LDD     0,X             ;
        STD     AREG+3          ; 1ST TO AREG
        JSR     DIV             ; C/A COMP1 A/D X %N / %1

;      WEIGHT CALCULATION REQUIRED HERE.

         LDX     #CMP1CNTG       ; LOAD COMP #1 TARGET COUNT STORAGE.
        LDAB    TARGNO          ; READ COMPONENT COUNTER
        DECB                    ;
        ABX
        ABX
        ABX
        STX     TEMPX           ; SAVE X

        JSR     EMOVE           ;
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
        JSR     CONVTARTOLBS

CALCTGWT LDX     #CMP1CNTG       ; COUNTS FOR COMPONENTS.
        LDAB    TARGNO          ; CURRENT COMPONENT.
        DECB
        ABX
        ABX
        ABX
        JSR     BCDHEX3X        ; CONVERT BACK TO HEX
        PSHB
        PSHM    E               ;SAVE RESULT
        LDX     #CMP1CNTGH      ; COUNTS FOR COMPONENTS.
        LDAB    TARGNO
        DECB
        ABX
        ABX
        ABX
        PULM    E
        PULB
        STAB    0,X             ; STORE HEX TARGET
        STE     1,X             ; LSB
        RTS                     ;

;
;       CALCULATE TIME

CALTB1  JSR     CLCREG
        LDE     #6
        STE     MATHCREG

        LDX     TEMPX           ; RELOAD X (COMPONENT TARGET)
        LDD     0,X
        STD     MATHCREG+2        ; XX XX XX 00 00 COUNTS TO C REG
        LDAA    2,X
        STAA    MATHCREG+4      ;2
        CLRW    MATHCREG+5      ;3

;        LDX     #CMP1CPI        ;
        JSR     SETCPIPTR
CALTIM
        LDD     #$0002
        STD     AREG            ; 4 DECIMAL DIGITS
        LDE     0,X             ; COUNTS PER INTERRUPT
        STE     AREG+2
        LDAA    2,X
        STAA    AREG+4
        CLRW    AREG+5

        JSR     FDIV             ; THIS RESULT IS THE NUMBER OF INTERRUPTS
                                ; REQUIRED TO FILL TO THIS WEIGHT BASED ON
                                ; COMP #1
        JSR     FPTINC
        LDAA   #2
        LDX    #MATHDREG+1          ;EREG
        JSR    GPRND1            ;~ROUND TO 2 DIGITS

        LDX     #CMP1TIMS       ; COMPONENT #1 INTERRUPT TIME STORAGE
        LDAB    TARGNO          ; CURRENT COMPONENT
        DECB
        ABX                     ;
        ABX                     ;
        ABX                     ;
        LDAB    EREG+1
        LDE     EREG+2          ; READ CALCULATED NO OF INTERRUPTS
        JSR     BCDHEX3

;  BCDHEX3 CONVERTS A 3 BYTE (in B:E]) BCD VALUE TO HEXADECIMAL (in B:E)
;       Max value 999,999 -> $F423F

;;P
;        LDE     #525           ; ALLOW TO CYCLE

        TST     ISVOLMODE       ; MODE ENABLED ?
        BNE     NOVOL           ; NO VOLUMETRIC MODE ON 1ST BATCH.
;;T
;        TST     HILOREGENABLED          ; IS THIS OPTION ENABLED.
;        BEQ     STRASN                  ; NO
;        LDD     HIREGPCNT
;        JSR     BCDHEX2
;        TDE
;        CLRB

STRASN  STAB    0,X             ;
        STE     1,X             ; STORE RESULT
NOVOL   RTS                     ;


STORECOMPONENTTARE:
        LDD     CURRLCAD
        STD     CURRLCADTARE    ; A/D HISTORY
        LDAA    CURRLCAD+2
        STAA    CURRLCADTARE+2  ; C
        RTS



MONBAT  LDAA    BATSEQ          ; READ SEQUENCE FLAG
        CMPA    #CALC           ; SHOULD BE REST OF THE CALCULATIONS BE RUN
        LBNE     NOTCALC         ; NO
        TST     AVGAVAIL        ;
        LBEQ     NOCAL           ; AVERAGE NO AVAILABLE
        CLR     AVGAVAIL        ; RESET AVERAGING FLAG
        LDAA    TOPUPF           ; IS TOP UP ENABLED
        BEQ     RDTBASN         ; READ TABLE AS NORMAL.

        LDAA    TOPUPSEQUENCING
        CMPA    #FILLCOLOUR
        BEQ     ISCOLOR         ; YES.
        CMPA    #TOPUPCOLOUR    ; TOP UP
        BEQ     NOTNRL

        CMPA    #NORMALOPERATION ; INDICATE NORMAL OPERATION.
        BNE     RDTBASN         ; NO STORAGE OF DATA.
        LDX     SEQPOINTER
        LDAA    0,X
        STAA    COMPNODETAILS
        ANDA    #$0F            ; COMP NO
        STAA    COMPNO          ; STORE COMP NO
        LBRA     NOSTORD         ; NO


NOTNRL  TST     NOTOPUPREQUIRED ; NO TOP UP REQUIRED
        LBNE     NOSTORD         ; NO STORAGE OF DATA.
        LDX     SEQPOINTER
        LDAA    0,X
        ANDA    #$0F            ; ISOLATE COMPONENT NO.
        LDAA    #EIGHT
        BRA     STRCMP          ; STORE COMPONENT NO.

ISCOLOR LDX     LASTCOMPTABLEP  ; READ LAST COMPONENT.
        BRA     READT           ; READ TABLE.
RDTBASN LDX     SEQPOINTER
READT   LDAA    0,X
STRCMP  STAA    COMPNODETAILS
        ANDA    #$0F            ; COMP NO
        STAA    COMPNO          ; STORE COMP NO
        JSR     STORNCT         ; STORE THE SETTLE COUNT
        TST     ISVOLMODE
        BNE     VOLM            ; NO RETRY IF VOLUMETRIC MODE
        TST     MOVEON          ; SHOULD WE MOVE ON
        BEQ     CHECK
        CLR     MOVEON
VOLM    JSR     STORECOMPONENTTARE
        BRA     CALCNOW
CHECK:
        JSR     CALCULATESECONDS
        JSR     CHKFILLACC
        TSTA                    ; CHECK RESULT.
        BNE     FILLERR         ; ERROR IN FILLING.
        JSR     STORECOMPONENTTARE
        BRA     CALCNOW

FILLERR LDAB    COMPNO          ; READ COMPONENT NO.
        LDAA    #2              ; RESET THE ROUND ROBIN TWICE
        JSR     RESETWGTRROBIN
        JSR     CALCRROBINFORWEIGHT
        TST     CHANGINGORDER   ; % CHANGE.
        LBEQ     COMPAGAIN       ; RETRY IF NOT CHANGING ORDER.

CALCNOW CLR     CORRECTRETRIES
        JSR     CALCNTPI        ; COUNT PER INTERRUPT CALULATED FOR COMP #N

        TST     STAGEFILLEN
        BEQ     NOPRI           ; NO DIAGNOSTICS PRINT ON EACH COMP

        LDD     PASWRD          ; ONLY PRINT OUT ON DIAGNOSTICS PASSWORD
        CPD     #$0065          ; SPECIFIC PASSWORD ?
        BNE     NOPRI
        LDAA    PASWRD+2
        CMPA    #$53           ;
        BNE     NOPRI
        JSR     BPCNTNCAL
        JSR     BBDIAPRN

NOPRI   JSR     STORESTAGEFILLDATA

        TST     MULTIPLEFILLING
        LBNE    GOTONEXTCOMP

NOSTORD LDAA    CALCROUTER      ; CHECK WHICH SECTION TO RUN
        CMPA    #CALCMPNTAR     ; CALCULATE REST OF TARGETS ?
        BEQ     CALCREST
        CMPA    #CALC1STCMPTAR  ; 1ST COMP CALCULATION REQUIRED
        LBNE     CHKCPI
        JSR     CALCWGTAFTREG   ; WEIGHT AFTER REGRIND AND NEXT TARGET
        LDX     SEQPOINTER
        LDAA    0,X
        STAA    COMPNODETAILS
        ANDA    #$0F            ; COMP NO
        STAA    COMPNO          ; STORE COMP NO

        JSR     CALCMNTIM       ; CALCULATE COMPONENT #1 UPDATED TIME
        LDAA    #$0F
        STAA    FSTCOMPCALFIN   ; INDICATE THAT THIS IS FINISHED.
        JMP     GOTONEXTCOMP    ; NEXT COMPONENT.



;

;       CALCULATE CMP2,,,,N

CALCREST:
        JSR     CALCMNTIM       ; CALCULATE COMPONENT #1 UPDATED TIME
        TST     TOPUPF
        BEQ     NOTTUP1         ; NO TOP UP.

        LDAA    TOPUPSEQUENCING
        CMPA    #FILLFIRST      ; FIRST COMPONENT FILLED ?
        BNE     NOT1STF         ; NO
        LDX     SEQPOINTER
        LDAA    1,X             ; READ TABLE CONTENTS
        CMPA    #SEQDELIMITER   ; ONLY ONE COMPONENT
        LBEQ    ENDCYCLE        ; SINGLE COMPONENT

        LDAA    #FILLCOLOUR     ; FILL THE COLOUR NEXT.
        STAA    TOPUPSEQUENCING ; SET THE SEQUENCING.
        JSR     CALCULATECOMPONENTTARGETS
        JSR     FINDCOLOURCOMPONENT ; SET COLOUR COMPONENT POINTER.
        LBRA    INCTONX        ; NO INCREMENT

NOT1STF LDAA    TOPUPSEQUENCING
        CMPA    #FILLCOLOUR     ; IS THE COLOUR BEING FILLED ?
        BNE     NOTCLF          ; NOT COLOUR FILLING..
        JSR     CALCULATECOLOURCOMPENSATION  ; CALCULATE TOP UP.
        TST     NOTOPUPREQUIRED
        LBEQ    INCTONX         ; TOP UP AS NORMAL
        LBRA    XITCYC          ; NO INCREMENT IF TOP UP.
NOTCLF  LDAA    TOPUPSEQUENCING
        CMPA    #TOPUPCOLOUR    ; HAS THE TOPUP TAKEN PLACE.
        BNE     NOTTUP1         ; NO
        LDAA    #NORMALOPERATION ; INDICATE NORMAL OPERATION.
        STAA    TOPUPSEQUENCING ; SEQUENCE INDICATED.

;       CHECK IF THIS IS THE LAST COMPONENT.
;
        LDX     SEQPOINTER
        LDAA    2,X
        CMPA    #SEQDELIMITER   ; IS THIS THE LAST COMPONENT
        LBEQ     ENDCYCLE          ; YES END CYCLE.

;       LDAA    #1
;       STAA    NOTOPUPREQUIRED
        LBRA    XITCYC          ; NO FILLING REQUIRED



NOTTUP1:
;        JSR     CALCMNTIM       ; CALCULATE COMPONENT #1 UPDATED TIME

        JSR     CALCULATECOMPONENTTARGETS
        LDAA    TOPUPSEQUENCING
        CMPA    #NORMALOPERATION        ; IS THIS THE NORMAL OPERATION.
        BNE     FILLNOW                 ; NO
        CLR     TOPUPSEQUENCING         ; RESET
        BRA     NOTUP                   ; GO TO NEXT.


; DECIDE ON THE NEXT COMPONENT TO BE FILLED

; NOTE NOT INCREMENT UNLESS NORMAL OPERATION IS SET.


FILLNOW:

        TST     TOPUPF
        BEQ     NOTUP          ; NORMAL OPERATION.
        LDAA    TOPUPSEQUENCING ;
        CMPA    #NORMALOPERATION ; BACK TO NORMAL OPERATION ?
        BEQ     NOTUP           ; YES
;        PULM    X
        LBRA     INCTONX          ; NO CHANGE TO TABLE POINTER


NOTUP   LDX     SEQPOINTER
        AIX     #1
        LDAA    0,X             ; READ TABLE CONTENTS
        CMPA    #SEQDELIMITER   ; IS IT DELIMITED ?
        BEQ     ENDCYCLE        ; SINGLE COMPONENT
        STX     SEQPOINTER      ; UPDATE SEQTABLE POINTER
SKIPINCR:
        CLR     FILLCOUNTER     ; RESET FILL COUNTER
        CLR     FILLALARM       ; RESET COMP FILL ALARM
        JSR     RESETFILLCOUNTERS ; RESET FILL COUNTER
        CLR     FILLCTR         ; RETRYFILL = 0
        TST     PAUSFLG         ; PAUSED ?
        BEQ     NOPAUS1         ; NO
        LDAA    #PAUSEBFILL     ; IS SET
        STAA    BATSEQ          ; INDICATE PAUSED
        BRA     NOCAL           ; EXIT
NOPAUS1 JSR     CHECKLLS        ; CHECK LOW LEVEL SENSOR IF APP
NOCAL   RTS







CHKCPI:
        LDX     SEQPOINTER      ;
        CPX     #SEQTABLE       ; IS THIS THE BEGINNING OF THE TABLE. ?
        BNE     GOTONEXTCOMP    ; THIS IS ANOTHER COMPONENT.
        LDAA    1,X             ; IS THERE ONLY 1 COMPONENT
        CMPA    #SEQDELIMITER   ;
        BEQ     GOTONEXTCOMP    ; CALCULATION OF NEXT TARGET.
        JSR     CALNEXTAR       ; CALCULATE NEXT TARGET

GOTONEXTCOMP:

        LDX     SEQPOINTER      ;
        AIX     #1
        STX     SEQPOINTER
        TST     TOPUPF           ; TOP UP ENABLED ?
        BEQ     USELSTC         ; USE LAST COMPONENT
        LDAA    1,X             ; DO NOT FILL LAST COMPONENT.
        BRA     COMPDEL         ; COMPARE DELIMITER.
USELSTC LDAA    0,X
COMPDEL CMPA    #SEQDELIMITER   ; IS THIS THE FINAL COMPONENT ?
        BEQ     ENDCYCLE
        JSR     CHKFORSKIPPEDCOMP
        LDAA    0,X
        CMPA    #SEQDELIMITER   ; IS THIS THE FINAL COMPONENT ?
        BEQ     ENDCYCLE        ; END CYCLE
        LBRA    INCTONX         ; INCREMENT TO NEXT BLEND


ENDCYCLE:
;        JSR     CHKFOREOCPAUSE  ; CHECK FOR END OF CYCLE PAUSE.
        JSR     CHKFORRETRIES   ; CHECK FOR RETRIES
        LDX     #SEQTABLE
        STX     SEQPOINTER      ; RESET TO START OF TABLE

        TST     LEVELSENALOC    ; HAS THERE BEING AN ALARM.
        BNE     NOLSAL          ; YES
        LDAA    BLENDERMODE
        CMPA    #SINGLERECIPEMODE   ; SINGLE RECIPE MODE
        BEQ     NOLSAL          ; NO LS ALARM IF SRM
        LDAA    MBPROALRM       ; CHECK ALARM WORD.
        ANDA    #LEVELSENALBIT  ; LEVEL SENSOR ALARM BIT
        BEQ     NOLSAL          ; ALARM HAD BEING CLEARED.
        LDAA    #LEVELSENSORALARM
        JSR     RMALTB          ; RESET ALARM.
NOLSAL:
        TST     PAUSFLG         ; ARE WE IN PAUSE MODE ?
        BNE     ISPAUSED        ; PAUSED EXIT
        TST     PAUSETRAN       ; TRANITION FLAG
        BEQ     ISPAUSED
        TST     DMPLEVSEN       ; IS LEVEL SENSOR UNCOVERED ?
        BNE     ISPAUSED
        CLR     PAUSETRAN       ; PROCEED NO PROBLEMS.
ISPAUSED:
        TST     PAUSFLG
        BNE     CONCYC         ; IS PAUSED
        LDAA    BLENDERMODE
        CMPA    #THROUGHPUTMON ;
        BNE     CONCYC
        TST     LEVSEN1STAT       ; LEVEL SENSOR STATUS
        BNE     CONCYC
        LDAA    BLENDERMODE
        CMPA    #SINGLERECIPEMODE   ; SINGLE RECIPE MODE
        BEQ     CONCYC              ; NO LS ALARM IF SRM
        LDAA    MBPROALRM       ; CHECK ALARM WORD.
        ANDA    #LEVELSENALBIT  ; LEVEL SENSOR ALARM BIT
        BNE     CONCYC          ; ALARM HAD BEING CLEARED.
        LDAA    #LEVELSENSORALARM
        JSR     PTALTB          ; TO ALARM TABLE
CONCYC  LDAA    #LEVPOLL
        STAA    BATSEQ          ; SET "LEVEL SENSOR POLLING SEQUENCE"         ;
        CLR     CYCLEINDICATE   ; RESET TO INDICATE THE END OF THE CYCLE.
        CLR     FIRSTWCYCLE
        CLR     CHANGINGORDER
        JSR     CHECKFORSTAGEDFILL
        JSR     CHKFOREOCPAUSE  ; CHECK FOR END OF CYCLE PAUSE.
        TST     MULTIPLEFILLING ; MULTIPLE FILLING
        BNE     XITAL1          ; YES
        JSR     CHECKFORSINGLERECIPE
        INCW    CYCLECOUNTER    ; INDICATE THAT CYCLE HAS FINISHED.
        TST     NOLSALARMCTR
        BEQ     CTRA0
        DEC     NOLSALARMCTR
CTRA0   TST     ALLOWLARGEALARMBAND
        BEQ     NODEAB          ; NO DECREMENT REQUIRED
        DEC     ALLOWLARGEALARMBAND ; DECREMENT COUNTER
NODEAB  LDD     CYCLETIME
        STD     CYCLELOADTIMEPRN
        JSR     BPCNTNCAL
        LDAA    #2
        STAA    PRINTINNSECONDS
;        LDAA    #1
;        STAA    PRINTNOW
XITAL1  RTS

INCTONX:
        CLR     FILLALARM       ; RESET COMP FILL ALARM
        CLR     FILLCTR
        JSR     RESETFILLCOUNTERS ; RESET FILL COUNTER
SAME1   CLR     FILLCOUNTER     ; RESET FILL COUNTER
        TST     PAUSFLG         ; PAUSED ?
        BEQ     NOPAUS2         ; NO
        LDAA    #PAUSEBFILL     ; IS SET
        STAA    BATSEQ          ; INDICATE PAUSED
        BRA     XITCYC          ; EXIT
NOPAUS2
        JSR     CHECKLLS        ; CHECK LOW LEVEL SENSOR IF APP
XITCYC  RTS



COMPAGAIN:

RETRYCM:

ISREFL  TST     CORRECTRETRIES
        BNE     ISRET           ; IT IS RETRYING.
        LDAB    COMPNO          ; LOAD COMPONENT NO.
        JSR     SETTOIGNORECPI
ISRET   CLR     CORRECTRETRIES
        LDAA    #$0F
        STAA    PRINTNOW        ; SET PRINTING FLAG   ; PRINT OUT REPORT SO FAR.
        JSR     COPYMBCYCLEDIAGNOSTICS
        LDAA    COMPNO
        STAA    ANYRETRY        ; SET ANY RETRY FLAG.
NORETPR LDAA    CYCLEPAUSETYPE  ;
        CMPA    #PAUSEATENDOFCYCLE
        BNE     NOTEOC           ; NO
        JSR     IMDPAUSEON       ; PAUSE BLENDER FULLY.
        CLR     CYCLEPAUSETYPE
NOTEOC  LBRA     SAME1



NOTCALC CMPA    #PAUSEBFILL     ; PAUSED MODE
        BNE     QITMONB
        TST     PAUSFLG         ; IS THIS PAUSE MODE ?
        BNE     QITMONB         ; PAUSE MODE
        TST     PTONORMAL       ; BACK TO NORMAL
        BEQ     QITMONB         ; NO
        CLR     PTONORMAL       ; RESET FLAG
        JSR     CHECKLLS        ; RE ENABLE FILLING MODE.
        CLR     FILLALARM       ; RESET COMP FILL ALARM
        JSR     RESETFILLCOUNTERS ; RESET FILL COUNTER
        CLR     FILLCTR
        CLR     FILLCOUNTER     ; RESET FILL COUNTER
QITMONB RTS


NOTFILLPROBLEM:
        RTS


; FIND THE COLOUR COMPONENT ASSUME THAT IT IS THE LAST COMPONENT.

FINDCOLOURCOMPONENT:
        LDX     SEQPOINTER
CHKFOL  LDAA    0,X
        CMPA    #SEQDELIMITER
        BEQ     ISATDEL         ; DELIMITER FOUND.
        AIX     #1
        BRA     CHKFOL          ; CHECK FOR LAST.
ISATDEL AIX     #-1             ; STEP BACK TO THE LAST COMPONENT.            ;
        STX     LASTCOMPTABLEP  ; LAST COMPONENT TABLE POINTER.
        RTS

CALCULATECOMPONENTTARGETS:
        LDX     SEQPOINTER
        STX     TEMP1S
REPTAC  LDX     TEMP1S
        AIX      #1
        STX      TEMP1S
        LDAA     0,X
        CMPA     #SEQDELIMITER
        BEQ      XITCMCL
        STAA     COMPNODETAILS
        ANDA     #$0F
        STAA     TARGNO
        STAA     COMPNO
        JSR      CALTARN
        JSR     CALCMNTIM       ; COMP #N TIMES FOR #2...N
        BRA      REPTAC
XITCMCL RTS



; CHECK IF LOW LEVEL SENSOR IS ACTIVE IF SO WAIT UNTIL IT IS COVERED.

CHECKLLS:
        TST     PAUSFLG         ; PAUSE MODE
        BNE     XITLL
        TST     FITHAIF         ; VACUUM PCB CONNECTED.
        BEQ     XITLLS          ; NO
XITLLS  LDAA    #FILL
        STAA    BATSEQ          ; STORE
        STAA    CYCLEINDICATE   ; FILLLING IN PROGRESS
        LDAA    #1
        STAA    ACTIVATELATENCY
        TST     RETRYACTIVE     ; ARE WE RETRYING
        BNE     XITLL           ; YES EXIT
        TST     STAGEFILLEN             ; STAGED FILL ENABLE
        BEQ     XITLL           ; YES EXIT
;       CHECK IF LAYERING IS NOT REQUIRED
;;T
        TST     LAYERING        ; LAYERING REQUIRED
        BNE     STGFAN          ; STAGED FILL AS NORMAL
        LDX     SEQPOINTER
        CPX     #SEQTABLE       ; 1ST COMPONENT BEING DISPENSED
        BEQ     ACCL            ; ACTIVATE CALCULATION FOR COMPONENT 1 AGAIN.
        TST     TOPUPF          ; CHECK FOR TOP UP
        BEQ     DOSTFL          ; DO STAGED FILL
        LDAA    TOPUPSEQUENCING ; IS THE TOP UP IN PROGRESS ?
        BNE     XITLL           ; YES IT IS.
DOSTFL  TST     MULTIPLEFILLING ; IS MULTIPLE FILLING ACTIVATED
        BNE     XITLL

STGFAN  LDAA    #1
        STAA    STAGEFILLCALC   ; SET CALCULATION FLAG
XITLL   RTS
ACCL    TST     TOPUPSEQUENCING
        BNE     ISTOPU
        JSR     COMP1STXTOTWGT
ISTOPU  BRA     XITLL           ; ENSURE THAT TARGET FOR COMPONENT 1 IS CALCULATED AGAIN.



















CALCVIBAVG:
        LDX     #AN1AVGBUF
        JSR     ITCREG4         ; MOVE AVERAGED COUNTS TO C REG.
        JSR     CLAREG
        LDAA    WGTAVGFACTOR
        STAA    AREG+4
        JSR     DIV             ;
        LDX     #EREG+2         ; RESULTING DATA.
        RTS
;
;       STORE COUNTS FOR COMPONENT #N

;       COMPONENT COUNTS = AN1BUF -(TARE OFFSET+ COMP1 - - COMPN)
;       FOR COMP #1 NO COMPONENT IS ADDED, FOR #2 COMP1,#3 COMP1,2 ETC



STORERAWWEIGHT:
        LDX     #CMP1RAWCOUNTS
        LDAB    COMPNO
        DECB
        ABX
        ABX
        ABX
        LDD     RAW132
        STD     0,X
        LDAA    RAW132+2
        STAA    2,X

        LDX     #CMP1TARECOMPONENTCOUNTS
        LDAB    COMPNO
        DECB
        ABX
        ABX
        ABX
        LDD     CURRLCADTARE
        STD     0,X
        LDAA    CURRLCADTARE+2
        STAA    2,X


        LDX     #CMP1TARECOUNTS
        LDAB    COMPNO
        DECB
        ABX
        ABX
        ABX
        LDD     CALBLK+10
        STD     0,X
        LDAA    CALBLK+12
        STAA    2,X



        LDX     #CMP1CONST
        LDAB    COMPNO
        DECB
        ABX
        ABX
        ABX
        LDD     CALBLK+2
        STD     0,X
        LDAA    CALBLK+4
        STAA    2,X
        RTS

;
STORNCT
        JSR     CALCVIBAVG      ; CALCULATE VIBRATION AVERAGE.
        LDD     EREG+2
        STD     CURRLCAD        ; STORE CURRENT A/D
        LDAA    EREG+4
        STAA    CURRLCAD+2

        JSR     BCDHEX3X        ; TO HEX
        STAB    CURRLCADH       ; "CURRENT LOAD CELL A/D" HEX
        STE     CURRLCADH+1     ;
        JSR     STORERAWWEIGHT  ;

; NOW CALCULATE WEIGHT EREG - CURRLCADTARE
;
        LDX     #CURRLCAD
        JSR     AMOVE           ; CURRENT LOAD CELL A/D
        LDX     #CURRLCADTARE   ;
        JSR     CMOVE           ;
        JSR     SUB             ; A-C
        TST     MATHDREG         ; IS ANSWER NEGATIVE ?
        BPL     STRCNS           ; NO
        CLRW    EREG
        CLRW    EREG+2
        CLR     EREG+4          ; RESET RESULT
        ;
STRCNS  LDX     #CMP1CNAC       ; ACTUAL COUNTS #1
        LDAB    COMPNO          ; COMP NO
        DECB
        ABX
        ABX
        ABX

ADDSEL  JSR     EMOVE           ; STORE RESULTING COUNTS
;
;       WEIGHT CALCULATION PER COMPONENT
;
        JSR     CMPWCAL          ; CALCULATE COMPONENT WEIGHT
        LDX     #CMP1ACTWGT     ; COMPONENT #1 TARGET WEIGHT
        LDAB    COMPNO          ; READ COMPONENT COUNTER
        DECB                    ;
        ABX
        ABX
        ABX
        LDD     EREG
        STD     0,X             ; STORE WEIGHT
        LDAA    EREG+2          ;
        STAA    2,X

        LDX     #CMP1ACTWGT     ; COMP #1 ACTUAL WEIGHT (METRIC)
        LDAB    COMPNO
        DECB
        ABX
        ABX
        ABX
        JSR    CNV3KGLB         ; CONVERT KGS TO LBS
        LDX     #CMP1ACTWGTI    ; COMP #1 ACTUAL WEIGHT (IMPERIAL)
        LDAB    COMPNO
        DECB
        ABX
        ABX
        ABX
        LDD     EREG+2          ; WEIGHT CONVERTED TO LBS
        STD     0,X
        LDAA    EREG+4          ;
        STAA    2,X             ; LSB OF LBS
        JSR     CALCRROBINFORWEIGHT ; CALCULATE ROUND ROBIN WEIGHT.

        JSR     CPYHWTSMB       ; STORE HOPPER WEIGHT
        RTS
;
;
;
;       CALCULATE A/D COUNTS PER INTERRUPT FOR COMP #N
;
; CREG    00 XX XX XX 00 COUNTS /   AREG  00 00 XX XX XX INTERRUPTS.
; CREG    XX XX XX 00 00 COUNTS /   AREG  00 00 XX XX XX INTERRUPTS.
;
;
CALCNTPI:
        JSR     CLRAC
        LDX     #CMP1CNAC
        LDAB    COMPNO          ; CURRENT COMPONENT
        DECB                    ;
        ABX
        ABX
        ABX

ADDSEL1 LDAA    0,X             ; A/D COUNTS FOR COMP #N
        LDE     1,X             ;
        STAA    MATHCREG+0          ; EG 20000 00
        STE     MATHCREG+1          ; A/D COUNTS TO C REG

        LDX     #CMP1TIMS       ;
        LDAB    COMPNO          ; CURRENT COMPONENT
        DECB
        ABX
        ABX
        ABX
        LDAB     0,X             ; READ COUNTER VALUE
        LDE     1,X             ; READ COUNTER VALUE

        JSR     HEXBCD3
        STD     AREG+1
        STE     AREG+3          ;




;  HEXBCD2  CONVERTS A 2 BYTE in D [A;B]) HEX VALUE TO BCD in B:E (MAX 65535)
;           RESULTING BCD IS IN B:E   (IE 6 5535 FOR $FFFF)

        JSR     DIV             ; C/A XXX.YY COUNTS PER INTERRUPT EG 16666 166.66
                                ; XX.YYYY COUNTS PER INTERRUPT EG 16666 - > 1.6666
;        LDX     #CMP1CPI        ; COMP #1 COUNTS/INT STORAGE
        JSR     SETCPIPTR
        LDAB    COMPNO          ; CURRENT COMPONENT
        DECB
        ABX
        ABX
        ABX
        LDAA    EREG+2          ; RESULT
        LDE     EREG+3
        LDAB    COMPNO          ; READ TARGET NO.
        CMPB    #4              ; COMPONENT 4
        BNE    OKAY1           ; NO
        CMPA    #$10
        BLS     OKAY1
        NOP
OKAY1

        LDAB    #RAMBANK
        TBZK
        LDZ     #NOFLOWRATECALCULATION1
        LDAB    COMPNO
        DECB
        ABZ
        TST     0,Z                             ; CHECK FOR NON ZERO.
        BEQ     CHKFR
        CLR     0,Z                             ; RESET FLOW RATE FLAG
        CLR     NOFLOWRATECALCULATION
        BRA     XITFRC

CHKFR   TST     NOFLOWRATECALCULATION
        BEQ     DOCALCFR
        CLR     NOFLOWRATECALCULATION
        BRA     XITFRC
DOCALCFR:
        PSHM    D,E

        JSR     CALCCPIRROBIN

; CHECK FOR RESULT OF 00.00

        TSTA                    ; A =0
        BNE     STRCPI
        TSTE                    ; E =0
;        CPE     #MINFLOWRATE    ; COMPARE TO MIN FLOW RATE
        BNE     STRCPI          ;
        PULM    D,E             ; RECOVER ORIGINAL VALUE.
        STAA    0,X             ;
        STE     1,X             ; COUNTER PER INTERRUPT STORED.
        BRA     XITFRC

STRCPI  STAA    0,X             ;
        STE     1,X             ; COUNTER PER INTERRUPT STORED.
        PULM    D,E             ; RECOVER ORIGINAL VALUE.
XITFRC  RTS                                ;

;
;       CALCULATE COMP N TIMED COUNT BASED ON THE COUNTS PER INTERRUPT
;       CALCULATED FOR THE PREVIOUS COMPONENT.
;
;       CREG    XX XX XX 00 00  A/D COUNTS / 00 00 XX XX .YY COUNTS PER IN


CALCMNTIM:
        LDX     #CMP1CNTG       ;
        LDAB    COMPNO          ; CURRENT COMPONENT
        DECB                    ;
        ABX
        ABX
        ABX

        LDE     #6
        STE     MATHCREG
        LDD     0,X
        STD     MATHCREG+2
        LDAA    2,X
        STAA    MATHCREG+4      ;+2
        CLRW    MATHCREG+5
;
;       LOAD COUNTS PER INTERRUPT FOR THE CURRENT COMPONENT.

;        LDX     #CMP1CPI        ;
        JSR     SETCPIPTR
        LDAB    COMPNO          ; CURRENT COMPONENT
        DECB                    ;
        ABX
        ABX
        ABX
        LDAA    COMPNO
        STAA    TARGNO          ; STORE TO TARGET NO USE EXISTING ROUTINE
        JSR     CALTIM          ; USE EXISTING TIME CALCULATION.
;;P        TEST

        RTS



CALNEXTAR:
        LDX     SEQPOINTER      ; LOAD CURRENT POINTER POSITION
        AIX     #1              ; GET PAST THIS COMPONENT.

CALNEC  PSHM    X
        LDAB    0,X
        ANDB    #$0F
        STAB    TARGNO
        STAB    COMPNO
        STX     TEMP1S          ; CURRENT TABLE POSITION.
        JSR     CALTARN         ; TARGETS FOR COMP #2....N
        JSR     CALCMNTIM       ; COMP #N TIMES FOR #2...N
        PULM    X
        AIX     #1              ; TABLE POINTER +1
        LDAA    0,X
        CMPA    #SEQDELIMITER
        BEQ     ATLASC          ; GET OUT OF HERE.

        BRA     CALNEC          ; NEXT COMPONENT
ATLASC  RTS




;
;       KG/HR CALCULATION.
;
;       KG/HR = (BATCH WEIGHT (1/10TH GRAMS) * 3600)/BATCH TIME (SECONDS)
;             = XXXXX.Y KG/HR
;
CALKGPHR:
        JSR     CLRAC
        LDX     #PREVBATCHWEIGHT; LOAD CURRENT BATCH
        JSR     AMOVE           ;
        LDX     #$3600          ;
        STX     MATHCREG+3      ; NO OF SECONDS IN THE HOUR
        JSR     MUL             ; CALCULATE KG/HR
        LDX     #MATHCREG       ;
        JSR     IFEREG
        LDX     #BATCHSECS      ; SECONDS THAT BATCH WAS THEREFORE XXXX.YY
        JSR     AMOVE           ;
        JSR     DIV             ; C/A DIVIDE BY NO OF SECONDS TO CALCULATE KG/HR
        LDX     #BATCHKGPHR
        JSR     IFEREG          ; STORE KG/HR
        LDD     EREG+2          ;
        STD     TPTKGH          ; STORE KG/HR TO DISPLAY
        LDAA    EREG+4          ;
        STAA    TPTKGH+2        ; STORE LSB
        JSR     CONVKGHRI       ; KG/HR TO IMPERIAL UNITS
        JSR     GENRRKGH
        JSR     CPYKGHMB
        RTS

;
;       CALCULATE NUMBER OF SECONDS FOR THE BATCH CYCLE
;
;((PITINDEX - PREVPIT TIME + PITFRACTCTR)->BCD / (PITINDEX) + BATCH TIME (SECONDS)
;



BATSECAL:
        LDD     #PITINDEX       ; PIT SPEED.
        SUBD    PREVPITM        ; PIT SPEED - PREVIOUS PIT VALUE
        ADDD    PITFRACTCTR        ; ADD NEW PIT READING FROM PREVIOUS SECOND
        JSR     HEXBCD2         ;
        JSR     CLRAC
        STE     MATHCREG+1          ;
        LDD     #PITINDEXBCD
        STD     AREG+3          ;
        JSR     DIV             ; C/A FRACTION OF SECOND IS CACULATED XX.YY

        LDAA   #2
        LDX    #EREG
        JSR    GPRND1            ;~ROUND TO 3 DIGITS

        JSR     CLRAC

        LDX     EREG+2          ; READ .YY SECONDS
        STX     AREG+3
        LDD     PITFRACTCTR     ;
        STD     PREVPITM        ;
        CLRW    PITFRACTCTR
        LDX     #-1
        LDD     KGHRCOUNT       ; READ SECONDS
        STX     KGHRCOUNT       ;
        JSR     HEXBCD2
        STE     MATHCREG+2      ; SECONDS TRANSFERRED FOR ADDITION.
        JSR     ADD
        LDX     #BATCHSECS      ; NO OF SECONDS
        JSR     EMOVE
        RTS


;
;       CALCULATION OF THE ACTUAL % FOR EACH COMPONENT.
;
;    %N = ((COMPNWEIGHT (*0000)/ BATCH WEIGHT) ROUNDED TO 1DP)SHIFT 1 DIGIT RIGHT
;       = XXX.Y %
;       RESULT STORED IN BPCNT1....BPCNTN.
;
;    %N = (COMP WEIGHT / TOTAL WEIGHT - REGRIND COMP WEIGHT) IF REGRIND.

BPCNTNCAL:

        JSR     CALHWTAV        ; CURRENT WEIGHT
        LDD     EREG
        STD     WEIGHTINHOP     ;
        LDAA    EREG+2
        STAA    WEIGHTINHOP+2    ; STORE WEIGHT IN HOPPER
        LDX     #BPRRCNT1
        STX     PPERCENT            ; POINTER TO %
        LDX     #CMP1ACTWGTRR    ; ROUND ROBIN VALUE.
        STX     PACTWEIGHT               ; POINTER TO ACTUAL WEIGHT
        JSR     CALCULATEPERDATA ; CALCULATE % DATA
        LDX     #BPCNT1
        STX     PPERCENT        ; POINTER TO %
        LDX     #CMP1ACTWGT      ; ROUND ROBIN VALUE.
        STX     PACTWEIGHT               ; POINTER TO ACTUAL WEIGHT
        JSR     CALCULATEPERDATA ; CALCULATE % DATA
        RTS


CALCULATEPERDATA:
        LDX     #SEQTABLE
        STX     TEMPX           ; SET START OF SEQ TABLE.
        LDX     PPERCENT
        LDAB    #MAXCOMPONENTNO
REPCLEAR:
        CLRW    0,X
        AIX     #2
        DECB
        BNE     REPCLEAR       ; IS REGRIND PRESENT.
        JSR     CLAREG


        LDD     WEIGHTINHOP
        STD     AREG+2          ;
        LDAA    WEIGHTINHOP+2
        STAA    AREG+4          ;  STORE WEIGHT IN HOPPER

        LDAB    REGCOMP         ; REGRIND COMPONENT.
;        LDX     #CMP1ACTWGT     ; ADDRESS OF COMP #N WEIGHT
        LDX     PACTWEIGHT              ; POINTER TO ACTUAL WEIGHT
        DECB
        ABX
        ABX
        ABX
        JSR     CMOVE
        JSR     SUB             ; TOTAL - REG COMP

        LDD     EREG+2
        STD     WEIGHTAFTREG    ;
        LDAA    EREG+4
        STAA    WEIGHTAFTREG+2   ; STORE WEIGHT IN HOPPER

NOREGNORM:

; IF TOPUP AND 1ST COMPONENT, ADD 1ST AND 8TH COMPONENTS TO DETERMINE THE ACTUAL % REQUIRED.

        TST     TOPUPF           ; IS TOP UP ENABLED ?
        LBEQ     NORMPCL         ; NORMAL % CALCULATION.
        LDAB    SEQTABLE
        BITB    #REGBIT          ; IS THIS COMPONENT REGRIND.
        BEQ     NONTREG
        LDAB    SEQTABLE+1       ; LOAD THE COMPONENT AFTER THE REGRIND COMPONENT.
NONTREG ANDB    #$0F
        STAB    TEMPC           ; STORE COMPONENT NO.

        LDAB    TEMPC           ;
        LDX     #CMP1ACTWGT     ; ADDRESS OF COMP #N WEIGHT
        DECB
        ABX
        ABX
        ABX
        JSR     AMOVE           ; MOVE 1ST COMPONENT TO TABLE.

        LDAB    #EIGHT
        LDX     #CMP1ACTWGT     ; ADDRESS OF COMP #N WEIGHT
        DECB
        ABX
        ABX
        ABX
        JSR     CMOVE
        JSR     ADD             ; ADD THESE TWO COMPONENTS TOGETHER.
        JSR     CLAREG          ; CLEAR A
        LDAB    TEMPC           ;
        LDX     #CMP1ACTWGT     ; ADDRESS OF COMP #N WEIGHT
        DECB
        ABX
        ABX
        ABX
        JSR     EMOVE

; CONVERT WEIGHT TO IMPERIAL FORMAT.

        LDX     #CMP1ACTWGT     ; COMP #1 ACTUAL WEIGHT (METRIC)
        LDAB    TEMPC
        DECB
        ABX
        ABX
        ABX
        JSR    CNV3KGLB         ; CONVERT KGS TO LBS
        LDX     #CMP1ACTWGTI    ; COMP #1 ACTUAL WEIGHT (IMPERIAL)
        LDAB    TEMPC
        DECB
        ABX
        ABX
        ABX
        LDD     EREG+2          ; WEIGHT CONVERTED TO LBS
        STD     0,X
        LDAA    EREG+4          ;
        STAA    2,X             ; LSB OF LBS

        LDX     #CMP1ACTWGT     ; ADDRESS OF COMP #N WEIGHT
;        LDAB    TEMPC           ;
        LDAB    #EIGHT
        DECB
        ABX
        ABX
        ABX
        CLRW    0,X
        CLR     2,X             ;


NORMPCL:         ;


REPPERC JSR     CLRAC7
        LDX     TEMPX           ; SEQUENCE TABLE POINTER.
        LDAB    0,X             ; READ CONTENTS
        ANDB    #$0F
        STAB    TEMPC           ; STORE COMPONENT NO.

        TST     REG_PRESENT     ; REGRIND USED ?
        BEQ     USEFULLWGT      ; USE TOTAL WEIGHT
        CMPB    REGCOMP         ; IS THIS THE REG COMP
        BEQ     USEFULLWGT      ; YES

        LDD     #$0006
        STD     AREG
        LDD     WEIGHTAFTREG    ;
        STD     AREG+2
        LDAA    WEIGHTAFTREG+2   ; RELOAD
        STAA    AREG+4          ; WT -> CREG XX XX X.Y 00 00
        BRA     CONPER1


USEFULLWGT:
        LDD     #$0006
        STD     AREG
        LDD     WEIGHTINHOP    ;
        STD     AREG+2
        LDAA    WEIGHTINHOP+2   ; RELOAD
        STAA    AREG+4          ; WT -> CREG XX XX X.Y 00 00


CONPER1:
        LDAB    TEMPC           ; RELOAD COMP NO
        LDX     #CMP1ACTWGT     ; ADDRESS OF COMP #N WEIGHT
        DECB
        ABX
        ABX
        ABX

        LDD     #$0006
        STD     MATHCREG

        LDD     0,X             ; COMP N WEIGHT -> CREG  XX XX X.Y 00 00
        STD     MATHCREG+2
        LDAA    2,X
        STAA    MATHCREG+4          ;

        JSR     FDIV             ; C/A CALCULATE % EREG -> 00 00 0X XX .YY
        JSR     FPTINC
        LDX    #MATHDREG+3
        LDAA   #2
        JSR    GPRND1            ;~ROUND A FURTHER 2 DIGITS


        LDX     #BPCNT1         ; %1 ADDRESS
        LDAB    TEMPC
        DECB
        ASLB
        ABX                     ; ADDRESS FOR DATA STORAGE
        LDD     EREG+1
        CPD     #$0100          ; 100 %
        BNE     NOT100          ; NO
        LDD     #$9999          ; 99.99 %
        BRA     STO9999
NOT100  LDD     EREG+2          ; READ ADDRESS OF DATA
STO9999 STD     0,X             ; STORE ACTUAL %



;       CHECK IF % ERROR IS > .5 % IF IT IS PRINT THE COMPONENT WEIGHTS
;
        JSR     CLRAC           ; A AND C REGISTER
        LDX     #EREG
        JSR     ITCREG          ; CALCULATED %
        LDX     #PCNT1          ;
        LDAB    TEMPC           ;
        DECB                    ;
        ASLB
        ABX
        LDD     0,X             ; READ % SET FOR THIS COMPONENT
        STD     AREG+3
        JSR     SUB             ; A-C % SET - % ACTUAL.
        LDD     EREG+2          ; CHECK
        BNE     PRINTWGT        ; PRINT WEIGHTS
        LDAA    EREG+4          ; CHECK DECIMAL % .Y
        CMPA    #05             ; %
        BLT     SKIPPRINT
PRINTWGT:

SKIPPRINT:
        LDAA    TEMPC
        CMPA    #4              ; IS THIS THE LAST COMPONENT ?
        BNE     NOLEAVE
        LDD     EREG+2
        BNE     PRINTWGT1
        LDAA    EREG+4
        CMPA    #3             ; OUTSIDE .3 %
        BLT     NOLEAVE
PRINTWGT1:

NOLEAVE LDX     TEMPX           ; COMP TRACKING COUNTER
        AIX     #1              ; POINTER INCREMENTED.
        LDAA    0,X
        CMPA    #SEQDELIMITER   ; AT END ?
        BEQ     EXIT            ; YES
        STX     TEMPX
        LBRA    REPPERC         ; CONTINUE
EXIT    JSR     CPYAPCTMB       ; COPY ACTUAL % TO MODBUS TABLE.
        RTS

; G/M CALCULATION.

BATCHGPMCALC:
        JSR     CLRAC         ;
        LDX     #BATCHLENGTH    ; LENGTH TO XX XX X.Y
        JSR     ITAREG          ;
        LDX    #AREG
        LDAA   #1
        JSR    GPRND1            ;~ROUND
        LDX    #EREG
        LDAB   #5
        LDAA   #1
        JSR    GPSFTR            ;~RESULT
        JSR     CLCREG
        LDX     #AREG
        JSR     IFEREG

        LDD     PREVBATCHWEIGHT
        STD     MATHCREG       ; PREVIOUS BATCH WEIGHT
        LDAA    PREVBATCHWEIGHT+2
        STAA    MATHCREG+2

        JSR     DIV             ;       C/A
        LDX     #BATCHGPERMETER  ;
        JSR     IFEREG          ; STORE WEIGHT PER METER CALCULATED.
        LDX     #WTPMVL
        JSR     EMOVE           ; TRANSFER RESULT FOR DISPLAY
        LDX     #WTPMVL5
        JSR     IFEREG           ; TRANSFER RESULT FOR DISPLAY
        JSR     CONVGPM         ; G/M TO IMPERIAL UNITS
        JSR     CPYGPMMB
        JSR     GPMTOGRAMPERSM
        RTS



NEWTARWEIGHTENTRY:


        JSR     COMP1STXTOTWGT        ; CALCULATE TARGET COUNTS FOR COMP #1
        LDX     SEQPOINTER
        LDAA    0,X                   ; READ 1ST COMP NO
        ANDA    #$0F                  ; 1ST COMP
        STAA    COMPNO
        JSR     CALCMNTIM
        RTS

;
;       CALCULATION OF ROUND ROBIN AVERAGED VALUE, KG/HR OR G/M
;       THIS PROGRAM CALCULATES THE ROUND ROBIN SET AND ACTUAL VALUES FROM THE ACTUAL ERROR VALUES.
;
;

;
CALCRRAVERAGE:
        LDX     AWTMERAC
        STX     AWTSTO
        LDX     AWTMERAC+2
        STX     AWTSTO+2
        LDX     AWTMERAC+4
        STX     AWTSTO+4
        LDAA    AWTMERAC+6
        STAA    AWTSTO+6


        LDX     #AWTMERAC         ; CALCULATED AVERAGED VALUE
        JSR     FTAREG          ;
        LDAA    CRLMODE
        CMPA    #HAULOFF        ;HAUL OFF CONTROL
        BNE     EXTRCON

        LDAA    AREG
        EORA    #$80            ; SIGN INVERTED FOR CALCULATION.
        STAA    AREG
EXTRCON LDX     #MATHCREG
        JSR     CLRNM2
        LDD     #$0001
        STD     MATHCREG
        LDAA    #$10
        STAA    MATHCREG+2
        JSR     FADD            ;  % ERROR +1

        LDX     #MATHDREG
        JSR     FTAREG          ; %ERROR+1 -> A REGISTER

        LDX     #MATHCREG
        JSR     CLRNM2          ; CLEAR
        LDD     #$0004          ; DP POSITION XXXXXX.Y
        STD     MATHCREG


;        LDAA    SPTMODE         ; CHECK THE SET POINT MODE IN OPERATION
        LDAA    BLENDERMODE     ; CHECK OPERATING MODE
        CMPA    #GPMGPMENTRY-1  ; IS THIS GRAM PER METER MODE ?
        BHI     YESGPMMODE      ;
        CMPA    #KGHCONTROLMODE ; KG/HR MODE
        BNE     EXIT04          ; SOME OTHER ENTRY
        LDX     #RRKGPH
        PSHM    X

        LDX     #TKGHSP         ;
        BRA     CONCLC1         ; CONTINUE
;
;       GRAM PER METER MODE
;

YESGPMMODE:
        LDX     #RRGMPM5
        PSHM    X
        LDD     #$0006          ; DP POSITION XXXXXX.YY
        STD     MATHCREG
        LDX     #WTPMSP
        LDE     0,X             ; KG/HR OR G/M TO C REG
        STE     MATHCREG+2
        LDE     2,X
        STE     MATHCREG+4


;        LDAA    2,X
;        STAA    MATHCREG+4

        JSR     FDIV            ; SETPOINT / (%ERROR+1)
        JSR     FPTINC          ; CONVERT TO INTEGER
        PULM    X
        LDE     MATHDREG+1
        STE     0,X

        LDE     MATHDREG+3
        STE     2,X
        LDAA    EREG
        STAA    4,X
        BRA     EXIT04

CONCLC1
        LDE     0,X             ; KG/HR OR G/M TO C REG
        STE     MATHCREG+2
        LDAA    2,X
        STAA    MATHCREG+4

        JSR     FDIV            ; SETPOINT / (%ERROR+1)
        JSR     FPTINC          ; CONVERT TO INTEGER
        PULM    X
        LDE     MATHDREG+3
        STE     0,X
        LDAA    EREG
        STAA    2,X
EXIT04:
        JSR     CONVKGHRRRI     ; KG/HR RR TO IMPERIAL
        JSR     CONVGPMRRI      ; G/M RR TO IMPERIAL
        JSR     CPYKGHMB
        JSR     CPYGPMMB        ; COPY MODBUS G/M ON UPDATE.
;        JSR     GPMTOGRAMPERSM
        RTS


; CALCULATION OF D/A COUNTS PER KG/HR VALUE AND CALCULATION OF NEW D/A TARGET
;
;
CALCDACPKG:
        TST     DISCONACTION    ; DISABLE CONTROL ACTION ON THE NEXT CYCLE.
        LBNE     NOESTIM
        TST     SCREWINCDECPR   ; HAS SOMEBODY PRESSED "INC/DEC" KEYS
        LBNE     YESSOMEONEHAS
        LDAA    CONTROLTYPE     ; CHECK MODE
        CMPA    #INCREASEDECREASE
        BNE     NORDA           ; NORMAL D/A
        LDD     SEIADREADING
        BRA     CHKIDDA
NORDA   LDD     DA1TMP          ;
CHKMNDA CPD     #MINDAPKGCALC   ; MINIMUM D/A PER KG CALCULATION.
        LBLO     NOESTIM         ; NO ESTIMATE
CHKIDDA PSHM    D               ; SAVE D.

; CALCULATE D/A PER METER PER MINUTE.
;
; LINE SPEED STORED AS XX XX .YY IN LSPD2S + 2.


        JSR     CLCREG          ; C REGISTER CLEARED.
        PULM    D
        PSHM    D
        JSR     HEXBCD2         ; CONVERT TO BCD
        STE     MATHCREG
        JSR     CLAREG          ;
        LDX     LSPD5S+2        ; METERS / MINUTES INTEGER.
        STX     AREG+2
        LDAA    LSPD5S+4
        STAA    AREG+4          ; DAC VALUE / KG/HR
        JSR     DIV             ; XX XX .00 00 00 / 00 00 XX XX YY
        LDX     #DACPMPM        ; D/A PER METER PER MINUTE
        JSR     IFEREG
        JSR     CLCREG7
        JSR     CLAREG7

        PULM    D
        JSR     HEXBCD2         ; CONVERT TO BCD
        STAB    MATHCREG+2
        STE     MATHCREG+3
        LDX     #$0006
        STX     MATHCREG
        LDX     #$0004
        STX     AREG
        LDX     TPTKGH          ; KG/HR READING
        STX     AREG+2
        LDAA    TPTKGH+2
        STAA    AREG+4          ; DAC VALUE / KG/HR
        JSR     FDIV             ; XX XX .00 00 00 / 00 00 XX XX YY
        JSR     FPTINC          ; TO INTEGER.
        LDX     MATHDREG+2
        STX     DACPKGHR
        LDX     MATHDREG+4
        STX     DACPKGHR+2
        LDAA    MATHDREG+6
        STAA    DACPKGHR+4
        LDD     #DACPKGDONEID   ; D/A PER KG CALCULATED.
        STD     DACPKGDONEFLAG  ;
NOESTIM RTS

YESSOMEONEHAS:
        CLR     SCREWINCDECPR   ; RESET FOR THE NEXT CYCLE.
        RTS


CLDACPKGCMR:
;        LDAA     BLENDERMODE     ; READ BLENDER MODE.
;        CMPA     #MAXTHROUGHPUTZT ; 0 -10VOLTS OPTION.
;        BNE     XITDAK
        JSR     CLCREG          ; C REGISTER CLEARED.
        LDE     #$4095
        STE     MATHCREG
        JSR     CLAREG          ;
        LDX     MXTPT1          ; KG/HR READING
        STX     AREG+2
        LDAA    MXTPT1+2
        STAA    AREG+4          ; DAC VALUE / KG/HR
        JSR     DIV             ; XX XX .00 00 00 / 00 00 XX XX YY
        LDX     #DACPKGCMR      ; D/A PER KG/HR
        JSR     EMOVE
XITDAK  RTS


;;

ESTKGHR LDAA    CRLMODE         ; CHECK MODE OF OPERATION.
        CMPA    #HAULOFF        ; HAUL OFF CONTROL.
        BEQ     NOKGHRE         ; SKIP KG/H ESTIMATION.
        JSR     CLCREG7
        JSR     CLAREG7

        LDX     #$0006          ;
        STX     AREG
        LDX     DACPKGHR        ; D/A PER KG
        STX     AREG+2
        LDX     DACPKGHR+2      ;
        STX     AREG+4
        LDAA    DACPKGHR+4      ;
        STAA    AREG+6

        LDD     #$0006          ; SEI A/D READING
        STD     MATHCREG        ; TO C REGISTER.

        LDAA    CONTROLTYPE     ; CHECK MODE
        CMPA    #INCREASEDECREASE
        BNE     NORDA1          ; NORMAL D/A
        LDD     SEIADREADING
        BRA     CHKEST          ; CALCULATE ESTIMATE

NORDA1  LDD     DA1TMP          ; READ D/A VALUE
CHKEST  JSR     HEXBCD2         ; CONVERT TO BCD
        STAB    MATHCREG+2
        STE     MATHCREG+3
        JSR     FDIV             ; C/A
        JSR     FPTINC
        LDD     MATHDREG+3
        STD     TPTKGH
        STD     RRKGPH
        LDAA    EREG
        STAA    TPTKGH+2
        STAA    RRKGPH+2
NOKGHRE:
        JSR     CLAREG
        LDX     #LSPD5S         ; ESTIMATE G/M
        JSR     ITCREG
        LDAA    #6
        STAA    AREG+4
        JSR     MUL             ; LSPEED * 6

        JSR     CLCREG
        LDX     TPTKGH
        STX     MATHCREG
        LDAA    TPTKGH+2
        STAA    MATHCREG+2
        LDX     #EREG
        JSR     ITAREG          ; L SPEED XX.YY METERS / HOUR
        JSR     DIV             ; C/A KG.HR XXXX.YY0000 / XX.YYM *6
        LDD     EREG+2
        STD     WTPMVL
        STD     RRGMPM
        LDAA    EREG+4
        STAA    WTPMVL+2
        STAA    RRGMPM+2
        LDX     #WTPMVL5
        JSR     IFEREG
        LDX     #RRGMPM5
        JSR     IFEREG

        JSR     CALC_ACTMIC     ; RELATE THIS BACK TO MICRONS.
        JSR     CONVGPMRRI      ; G/M IN IMPERIAL
        JSR     CONVKGHRRRI     ; KG/HR IN IMPERIAL
        JSR     CONVKGHRI       ; AS ABOVE
        JSR     CONVGPM
        JSR     CPYKGHMB
        JSR     CPYGPMMB
        JSR     GPMTOGRAMPERSM
        RTS


;
;       THIS PROGRAM CALCULATES THE NEW TARGET FOR THE SYSTEM USING THE
;       D/A PER KG VALUE, IF IN G/M MODE THE SYSTEM WILL CALCULATE THE
;       KG/HR TARGET FROM THE G/M SET POINT AND USE THIS TO CALCULATE THE
;       NEW D/A TARGET.



NEWVOLSET:
        LDAA    #TWO
        STAA    IGNORELSTIME
        LDAA    CONTROLTYPE
        CMPA    #INCREASEDECREASE
        BNE     NOINDE           ; NOT INCREASE /DECREASE
        LDD     SEIADREADINGAVG
        STD     DA1TMP          ; STORE INITIAL READING
NOINDE  LDAA     CRLMODE         ; CHECK CONTROL MODE.
        CMPA     #HAULOFF        ; HAUL OFF CONTROL ?
        LBEQ      NEWLSVOLSET     ; VOLUMETRIC SET VIA LINE SPEED.
        LDD     DACPKGDONEFLAG
        CPD     #DACPKGDONEID
        LBNE     XITVLS          ; EXIT VOLUMETRIC SET.
        LDAA    BLENDERMODE     ; CHECK THE SET POINT MODE IN OPERATION
        CMPA    #KGHCONTROLMODE       ; KG/HR MODE
        BEQ     ISKGPHRMODE     ; SOME OTHER ENTRY
        CMPA    #GPMGPMENTRY    ; IS THIS GRAM PER METER MODE ?
        LBLO     ERROREXIT

        JSR     CLRAC           ;
        LDX     #LSPD5S
        JSR     ITAREG
        LDX     WTPMSP
        STX     MATHCREG+1
        LDX     WTPMSP+2
        STX     MATHCREG+3
        CLR     MATHCREG

;        LDX     #WTPMSP         ;
;        JSR     CMOVE
        JSR     MUL             ; LINE SPEED ABCD.EF * G/M XX XX XX XX .YY
        LDX     #AREG
        JSR     IFEREG
        JSR     CLCREG
        LDAA    #6              ; ABOVE * 6 -> KG/HR XX XX .YY
        STAA    MATHCREG+4
        JSR     MUL


USETHISKG:
        LDX     EREG
        STX     MATHCREG+2
        LDAA    EREG+2
        STAA    MATHCREG+4
        CLRW    MATHCREG
        BRA     CALCNEW

ISKGPHRMODE:
        LDX     #TKGHSP
        JSR     CMOVE

CALCNEW LDX     #DACPKGHR
        JSR     ITAREG

CONTODA JSR     MUL             ; KG/HR SETPOINT * D/A PER KG/HR
        LDD     EREG
        LDX     #MATHDREG+4
        JSR     BCDHEX3X         ; CONVERSION
        TED
        TST    CONTROLTYPE
        BNE    NOMXID          ; NO MAX CHECK IN INCREASE DECREASE.
        JSR     DACLIM          ; CHECK DAC LIMIT
NOMXID  JSR     LIMITCORR       ; LIMIT CORRECTION TO XX %
        LDAA    #$0F
        STAA    DISCONACTION    ; DISABLE CONTROL ACTION ON THE NEXT CYCLE.

        LDAA    CONTROLTYPE
        CMPA    #INCREASEDECREASE
        BNE     NOTID
        LDAA    #NOOFESTIMATIONS        ;
        STAA    CONESTIMATION           ; NO OF TIMES TO CONTINUE ESTIMATION CALCULATION.
NOTID   LDAA    #1
        STAA    RUNEST
XITVLS  RTS



; CALCULATION OF LINE SPEED FROM KG/H AND GRAMS PER METER.
;



;NEWLSVOLSET:
;        JSR     CLRAC                   ; RESET REGISTERS.
;        LDAA    #6
;        STAA    AREG+4
;        LDD     WTPMSP                  ; GRAM PER METER SET POINT.
;        STD     MATHCREG+2
;        LDAA    WTPMSP+2
;        STAA    MATHCREG+4              ; STORE.
;        JSR     MUL                     ; 6 X G/M SET POINT.
;        LDX     #AREG                   ;
;        JSR     IFEREG                  ; RESULT TO A.
;        LDX     TPTKGH                  ; ACTUAL KG/H.
;        STX     MATHCREG
;        LDAA    TPTKGH+2
;        STAA    MATHCREG+2
;        JSR     DIV                     ; (KG /HR) / (6 X GRAMS PER METER.)
;
;        JSR     CLRAC                   ; RESET.
;        LDX     #AREG                   ; READ LINE SPEED. XX.YY METERS PER MINUTE.
;        JSR     IFEREG                  ; LINE SPEED RESULT.
;        LDX     #DACPMPM                ; D/A PER METER PER MINUTE.
;        JSR     ITCREG                  ; TRANSFER.
;        JSR     CONTODA                 ; CONVERT TO D/A
;        LDAA    #NOOFESTIMATIONS        ;
;        STAA    CONESTIMATION           ; NO OF TIMES TO CONTINUE ESTIMATION CALCULATION.
;        RTS



NEWLSVOLSET:
        JSR     CLRAC                   ; RESET REGISTERS.
        LDAA    #6
        STAA    AREG+4
;        LDD     WTPMSP                  ; GRAM PER METER SET POINT.
;        STD     MATHCREG+2
;        LDAA    WTPMSP+2
;        STAA    MATHCREG+4              ; STORE.

        LDD     WTPMSP                  ; GRAM PER METER SET POINT.
        STD     MATHCREG+1
        LDD     WTPMSP+2                ; .
        STD     MATHCREG+3              ; THIS WAS INCORRECT. (+2)


        JSR     MUL                     ; 6 X G/M SET POINT.
        LDX     #AREG                   ;
        JSR     IFEREG                  ; RESULT TO A.
        LDX     TPTKGH                  ; ACTUAL KG/H.
        STX     MATHCREG
        LDAA    TPTKGH+2
        STAA    MATHCREG+2
        JSR     DIV                     ; (KG /HR) / (6 X GRAMS PER METER.)

        JSR     CLRAC                   ; RESET.
        LDX     #AREG                   ; READ LINE SPEED. XX.YY METERS PER MINUTE.
        JSR     IFEREG                  ; LINE SPEED RESULT.
        LDX     #DACPMPM                ; D/A PER METER PER MINUTE.
        JSR     ITCREG                  ; TRANSFER.
        JSR     CONTODA                 ; CONVERT TO D/A
        LDAA    #NOOFESTIMATIONS        ;
        STAA    CONESTIMATION           ; NO OF TIMES TO CONTINUE ESTIMATION CALCULATION.
        RTS







CALCCMR LDAA    #10
        STAA    CMRVALID        ; INDICATE CMR IS VALID
        LDX     #TPTKGH
        JSR     CMOVE
        LDX     #DACPKGCMR
        JSR     AMOVE
        JSR     MUL             ; KG/HR SETPOINT * D/A PER KG/HR
        LDD     EREG
        JSR     BCDHEX2         ; CONVERSION
        JSR     DACLIM          ; CHECK DAC LIMIT
        STD     DA1TMP
        JSR     STARTSEIDAWR
        RTS



;
;       THIS LIMITS CORRECTION TO 'PERLIM' EG THE LIMIT IS SET AT 819, IE 20%
;       IF INCREASE /DECREASE USE PERLIMIDC % LIMIT FOR INCREASE / DECREASE CONTROL.
;       ELSE PERLIMNC FOR NORMAL CONTROL.


LIMITCORR:
        PSHM    D               ; SAVE CORRECTION.
        LDAA    CONTROLTYPE     ; CHECK CONTROL TYPE.
        CMPA    #INCREASEDECREASE ;
        BNE     NORMLC          ; NORMAL CONTROL.
        LDD     #PERLIMIDC      ; PER LMIIT FOR INCREASE DECREASE CONTROL.
        BRA     PROCLC          ; PROCEED WITH LIMIT CHECK.
NORMLC  LDD     #PERLIMNC       ; % LIMIT FOR NORMAL CONTROL.
PROCLC  STD     PERLIMSTO       ; % LIMIT STORAGE
        PULM    D
        PSHM    D               ; SAVE NEW D/A SETTING
        LDAB    OPTIONCODE
        LDAA    BLENDERMODE     ; CHECK MODE OF OPERATION.
        CMPA    #KGHCONTROLMODE ; KG/HR MODE ?
        BNE     NOTKGM          ; NO.
        TBA                     ; CHECK BIT SETTING
        ANDA    #KGHROPTIONBIT  ; KG/H OPTION ENABLED ?
        BNE     DOCORR          ; FUNCTION ENABLED - DO CORRECTION.
        PULM    D
        BRA     ERROREXIT       ; EXIT.

NOTKGM  CMPA    #GPMGPMENTRY    ; G/M
        BLO     DOCORR          ; THIS SITUATION SHOULD NOT ARISE
        TBA                     ; CHECK BIT SETTING
        ANDA    #GPMOPTIONBIT   ; KG/H OPTION ENABLED ?
        BNE     DOCORR          ; FUNCTION ENABLED - DO CORRECTION.
        PULM    D               ; RECOVER D REGISTER
        BRA     ERROREXIT       ; EXIT ON ERROR.

DOCORR  PULM    D               ; RECOVER D/A SETPOINT.
        PSHM    D               ; SAVE AGAIN.
        CPD     DA1TMP          ;COMPARE NEW WITH OLD
        BHI     POSCHANGE       ; CHANGE IS POSITIVE
        LDE     DA1TMP          ; READ PREVIOUS SETTING
        SDE                     ; CHECK DIFFERENCE.
        CPE     PERLIMSTO         ;
        BHI     OVERLIMITN      ; OVER LIMIT
        PULM    D
        BRA     STOREMSET
OVERLIMITN:
        PULM    D
        LDD     DA1TMP
        SUBD    PERLIMSTO         ;
        BRA     STOREMSET

POSCHANGE:
        LDE     DA1TMP
        XGDE                    ; CHANGE TO GET ERROR DIRECTION CORRECT.
        SDE
        CPE     PERLIMSTO
        BHI     OVERLIMITP      ; OVER LIMIT
        PULM    D
        BRA     STOREMSET
OVERLIMITP:
        PULM    D

        LDD     DA1TMP
        ADDD    PERLIMSTO

STOREMSET:
        TDE                     ; SAVE RESULT.
        TST    CONTROLTYPE
        BNE     UPDATE          ; NO MAX CHECK..
        CPD    MAXDA1
        BLS    UPDATE
        LDE    MAXDA1
UPDATE  STE    DA1TMP  ; NEW CONTROL SETTING.
        JSR     STARTSEIDAWR
        LDAA    #$0F
        STAA    CCVSFLG            ;~FLAG TO CALL CALCVS
ERROREXIT:
        RTS


FORMSEQTABLE:
        LDAB   #RAMBANK
        TBZK                    ;~;SET EK FOR BANK 1, I.E. RAM
        LDZ     #SEQTABLE       ; SEQUENCER TABLE.
        LDAB    #1              ; COUNTER
        LDX     #PCNT1          ; START OF %
CHECKNEXTP:
        TSTW     0,X            ; IS THE % ZERO ?
        BEQ     SKIPINSERT
        ANDB    #$0F
        PSHB
        TST     REG_PRESENT     ; ANY SIGN OF REGRIND ?
        BEQ     NOREG
        CMPB    REGCOMP
        BNE     NOREG           ; NOT THE REGRIND COMP
        ORAB    #REGBIT         ; SET REGRIND
        TST     FIRSTWCYCLE     ; IS THIS FIRST CYCLE AFTER RESET ?
        BEQ     NOTFIRST
        JSR     RESETRROBINCPI  ; ACCEPT CPI.
        ORAB    #FILLBYWEIGHT
NOTFIRST CPZ     #SEQTABLE       ; 1ST POSITION ANYWAY.
        BEQ     NOTFIRST1           ; CARRY ON.
        PSHM    Z               ; SAVE CURRENT POSITION.
REPSTZ  AIZ     #-1             ; PREVIOUS POSITION
        LDAA    0,Z             ; DATA READ
        STAA    1,Z             ; UPDATE POSITION.
        CPZ     #SEQTABLE       ; START OF TABLE
        BNE     REPSTZ
        STAB    0,Z             ; STORE REGRIND TO 1ST POSITION.
        PULM    Z               ; RECOVER.
        BRA     NEXTPOS         ; GO TO NEXT POSITION.


NOREG   TST     FIRSTWCYCLE     ; IS THIS FIRST CYCLE AFTER RESET ?
        BEQ     NOTFIRST1
        JSR     RESETRROBINCPI  ; ACCEPT CPI.
        ORAB    #FILLBYWEIGHT

NOTFIRST1:
        STAB    0,Z
NEXTPOS:
        AIZ     #1
        PULB
SKIPINSERT:
        AIX     #2
        CMPB    NOBLNDS
        BEQ     ATEND
        INCB
        BRA     CHECKNEXTP      ; GO FOR NEXT %
ATEND   LDAA    #SEQDELIMITER
        CPZ     #SEQTABLE       ; SEQUENCE TABLE.
        BNE     PROCEED         ; NOT AT BEGINNING OF TABLE.
        LDAB    #1
        STAB    0,Z
        AIZ     #1
PROCEED STAA    0,Z
        LDX     #SEQTABLE
        STX     SEQPOINTER
        TST     MANUALFILLSORT          ; CHECK FOR MANUAL FILL SORTING
        BEQ     SORTAN                  ; SORT AS NORMAL

SORTAN  JSR     CHKFORLARGEST           ; ENSURE THAT LARGEST FILLS FIRST.
        JSR     SWAPMASTERBATCH         ; CHECK IF SWAPPING IS REQUIRED FOR TOPUP
        RTS

CHKFORLARGEST:
        LDAB   #RAMBANK
        TBZK                    ;~;SET EK FOR BANK 1, I.E. RAM
        CLRW     LARGEST
        LDX     #SEQTABLE       ; TABLE ADDRESS
        LDAA    0,X
        ANDA    #REGBIT
        BEQ     NOTRG           ; NO REGRIND.
        AIX     #1
NOTRG
        PSHM    X               ; SAVE STARTING POINT
;        LDAB    #8              ; MAX NO OF PASSES.
        LDAB    #MAXCOMPONENTNO ; MAX NO OF PASSES.
REPLGC  PSHB
        LDAB    0,X
        ANDB    #$0F
        LDZ     #PCNT1
        DECB
        ASLB
        ABZ
        LDD     0,Z             ; READ %
        CPD     LARGEST         ; COMPARE TO LARGEST.
        BLS     NOTLAR          ; SMALLER
        STD     LARGEST
        STX     LARPTR          ; POINTER
NOTLAR  AIX     #1
        PULB
        DECB
        BEQ     XITLGC
        LDAA    0,X
        CMPA    #SEQDELIMITER   ; END ?
        BNE     REPLGC          ; REPEAT LARGEST CHECK.
        PULM    X
        LDZ     LARPTR
        LDAA    0,Z
        LDAB    0,X
        STAA    0,X
        STAB    0,Z             ; DO SWAP.
        RTS
XITLGC  PULM    X               ; RECOVER STACK
        RTS









;
;       TRANSFER CURRENT TABLE TO PREVIOUS TABLE.
;       COMPARE SEQTABLE AND PREVSEQTABLE, IF ANY NEW COMPONENT PRESENT
;       THEN FILL BY WEIGHT.
;


CURRTO_PREV:
        LDAB   #RAMBANK
        TBZK                    ;~;SET EK FOR BANK 1, I.E. RAM
        LDZ     #SEQTABLE       ; Z  TO CURRENT TABLE
        LDX     #PREVSEQTABLE   ; X TO PREVIOUS TABLE.
CONTRAN:
        LDAA    0,Z             ; CURRENT COMP
        STAA    0,X             ; TO PREVIOUS
        CMPA    #SEQDELIMITER   ; END ?
        BEQ     TRANCOMP        ; TRANSFER COMPLETE
        AIX     #1
        AIZ     #1
        BRA     CONTRAN

TRANCOMP:
        RTS


;
;       CHECK SEQ TABLE FOR NEW COMPONENT NO.

;       THIS PROGRAM CHECK FOR A NEW COMP ADDED BY COMPARING THE CURRENT
;       AND PREVIOUS SEQUENCE TABLES.

CHKTABNEWCOMP:

        LDAB   #RAMBANK
        TBZK                    ;~;SET EK FOR BANK 1, I.E. RAM
        LDZ     #SEQTABLE
NEXTCOMPCMP:
        LDX     #PREVSEQTABLE   ; PREVIOUS.
NEXTPREVCOMP:
        LDAA    0,Z
        ANDA    #$0F
        LDAB    0,X
        ANDB    #$0F
        CBA                     ; IS THIS COMP IN THE PREVIOUS TABLE ?
        BEQ     ISINBOTH        ; YES IT IS IN BOTH TABLES.
        AIX     #1              ; INC PREVIOUS TABLE POINTER
        LDAA    0,X             ;
        CMPA    #SEQDELIMITER   ; AT END ?
        BNE     NEXTPREVCOMP

        LDX     #CMP1CPI
        LDAB    0,Z             ; NEW COMP POSITION.
        DECB
        ABX
        ABX
        ABX
        TSTW    0,X              ;
        BNE     NEXTCOMP         ; NON ZERO FLOW RATE
        TST     2,X
        BNE     NEXTCOMP
        LDAB    0,Z             ; NEW COMP POSITION.
        JSR     RESETRROBINCPI  ; ACCEPT CPI.
        ORAB    #FILLBYWEIGHT   ; FILL BY WEIGHT
        STAB    0,Z
NEXTCOMP:
        AIZ     #1              ; NEXT CURRENT COMPONENT
        LDAA    0,Z             ;
        CMPA    #SEQDELIMITER   ; END OF CURRENT COMP'S
        BNE     NEXTCOMPCMP     ; START NEXT COMP COMPARE
        RTS


ISINBOTH:
        LDAA    0,X             ; READ COMPONENT NO.
        ANDA    #FILLBYWEIGHT   ; IS IT FILL BY WEIGHT
        BEQ     NEXTCOMP        ; NO NOT FILL BY WEIGHT.
        LDAA    0,Z             ; READ CURRENT COMPONENT NO POSITION.
        ORAA    #FILLBYWEIGHT
        STAA    0,Z             ; MAKE FILL BY WEIGHT AGAIN.
        BRA     NEXTCOMP        ; CONTINUE ON REGARDLESS.




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
;
;    CALCULATE G/M = CALCULATED AVERAGE DENSITY * WIDTH * MICRONS * 2
;    AVERAGE DENSITY CALCULATED FROM %XX.Y * DENSITY
;
;
CALCSETGPM:
;        LDAA    SPTMODE         ; CHECK THE SET POINT MODE IN OPERATION
;        CMPA    #GRAMPMETERMODE ; IS THIS GRAM PER METER MODE ?
;        LBNE     NOTGPMMODE      ;

        LDAA     BLENDERMODE        ;       CHECK EXTRUSION PROCESS
        CMPA     #GPMMICRONENTRY    ; ARE DENSITY ENABLED
        LBNE     NOTGPMMODE      ;

        CLRW    TEMPS
        CLRW    TEMPS+2
        CLR     TEMPS+4
        LDX     #SEQTABLE       ; TABLE START ADDRESS
CONAVGCALC:
        PSHM    X               ; STORE X REGISTER
        JSR     CLRAC           ; CLEAR
        LDAA    0,X             ; COMP NO
        TAB                     ; SAVE.
        ANDA    #REGBIT         ; IS COMPONENT REGRIND.
        BNE     NXTCMP
        ANDB    #$0F
        TBA
        LDX     #DENSITY1
        DECB
        ASLB
        ABX
        LDE     0,X             ; READ DENSITY CONCERNED.
        STE     AREG+3

        TAB
        LDX     #PCNT1
        DECB
        ASLB                    ; PCNT #N  XXX.Y TO C
        ABX
        LDE     0,X
        STE     MATHCREG+3
        JSR     MUL             ;

        LDX     #EREG
        JSR     ITCREG          ; RELOAD RESULT.
        LDX     #TEMPS          ; ADDITION BUFFER
        JSR     ITAREG          ; ADD DENSITIES

        JSR     ADD
        LDX     #TEMPS
        JSR     IFEREG
NXTCMP  PULM    X               ; RECOVER TABLE POSITION
        AIX     #1              ; TO NEXT COMP
        LDAA    0,X
        CMPA    #SEQDELIMITER   ; END OF TABLE ?
        BNE     CONAVGCALC      ; CONTINUE CALCULATION.

CALCAVERAGE:

        LDX    #EREG
        LDAB   #5
        LDAA   #1
        JSR    GPSFTR            ;~RESULT

        LDAA   #2
        LDX    #EREG             ;EREG
        JSR    GPRND1            ;~ROUND TOTAL DENSITY TO 2 DP


        LDAA    EREG+1
        LDE     EREG+2
        STAA    TOT_DENSITY
        STE     TOT_DENSITY+1   ; SAVE TOTAL DENSITY.

;
;       MULTIPLY RESULT BY WIDTH SETPOINT
;
        JSR     CLCREG
        STAA    MATHCREG+2
        STE     MATHCREG+3          ; AVERAGE DENSITY TO C REGISTER.
        JSR     CLAREG          ; CLEAR A
        LDX     WIDTHSP         ; WIDTH
        STX     AREG+3          ; TO A
        JSR     MUL             ; CREG XX XX XX XX XX * AREG 00 00 00 XX XX

;
;       RESULT BY MICRON SET
;
        LDX     #EREG
        JSR     ITCREG          ; RELOAD RESULT.
        JSR     CLAREG
        LDX     MICRONSET       ; MICRON SET POINT
        STX     AREG+3          ;
        JSR     MUL

;
;       FINALLY RESULT BY 2
;
        LDX     #EREG
        JSR     ITCREG          ; RELOAD RESULT.
        JSR     CLAREG
        LDAA    #2
        STAA    AREG+4
        JSR     MUL

        LDD     MATHDREG+4      ; LOAD COMPLETE RESULT.
        LDE     EREG+1
        LSRD
        RORE                    ;
        LSRD
        RORE                    ; I DIGIT SHIFT
        LSRD
        RORE                    ;
        LSRD
        RORE                    ;

        CLR     WTPMSP          ;
        STAB    WTPMSP+1        ; MSW
        STE     WTPMSP+2        ; LSB
NOTGPMMODE:
        RTS                     ;



;       CALCULATION OF ACTUAL MICRONS.
;       MICRONS = G/M  / ( 2*WIDTH*DENSITY)

CALC_ACTMIC:
        JSR     CLRAC           ; CLEAR THE WAY
        LDAA    #2              ;
        STAA    AREG+4          ;
        LDX     #TOT_DENSITY    ; DENSITY FROM CALCULATION
        JSR     CMOVE
        JSR     MUL             ; 2 * DENSITY

        LDX     #EREG
        JSR     ITCREG          ; RELOAD RESULT.
        JSR     CLAREG

        LDX     WIDTHSP
        STX     AREG+3
        JSR     MUL             ; * WIDTH

        LDX     #3              ; XXX.YYYYYYY
        STX     AREG            ; EXPONENT
        LDX     #AREG+2
        JSR     IFEREG          ; 2 * DENSITY * WIDTH

        LDX     #8
        STX     MATHCREG
        LDX     RRGMPM5          ; READ G/M
        STX     MATHCREG+2
        LDX     RRGMPM5+2
        STX     MATHCREG+4
        LDAA    RRGMPM5+4
        STAA    MATHCREG+6
        JSR     FDIV            ;G/M XXX.YY / (2 * WIDTH XXXX * DENSITY X.YYYY)
        JSR     FPTINC          ; INTEGER FORMAT
        LDX     MATHDREG+3
        STX     ACT_MICRONS
        LDAA    EREG            ; DECIMAL PART.
        STAA    ACT_MICRONS+2
        RTS



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



ORDERPER:
        LDX     #ORBPCNT1
        LDAA    #24
        JSR    CLRMEM   ;~CLEAR MEMEORY

        LDX     #SEQTABLE       ; COMPONENT START ADDRESS
NEXTCOMPPER:
        LDAB    0,X             ; READ COMP NO
        ANDB    #$0F
        PSHM    X               ; SAVE INFO AS REQUIRED.
        PSHB
        LDX     #$0006          ;EXPONENT FOR DATA
        STX     AREG
        STX     MATHCREG        ;
        LDX     #TOTWACCO       ; ORDER WEIGHT
        LDD     0,X
        STD     AREG+2
        LDD     2,X
        STD     AREG+4
        LDAA    4,X
        STAA    AREG+6          ; DENOMINATOR.
        TST     REG_PRESENT     ; ANY REGRIND.
        BEQ     NORPCL          ; DO NORMAL % CALCULATION.
        PULB                    ; READ COMPONENT NO
        PSHB
        CMPB    REGCOMP         ; IS THIS THE REGRIND COMPONENT ?
        BEQ     NORPCL          ; NORMAL CALCULATION
        LDX     #CH1WACCO       ; SUBTRACT OFF REGRIND COMPONENT
        LDAB    REGCOMP
        DECB
        ABX
        ABX
        ABX
        ABX
        ABX
        LDD     0,X
        STD     MATHCREG+2
        LDD     2,X
        STD     MATHCREG+4
        LDAA    4,X
        STAA    MATHCREG+6      ; COMPONENT WEIGHT
        JSR     FSUB            ; SUBTRACT OFF REGRIND WEIGHT FROM TOTAL
        LDX     #MATHDREG
        JSR     FTAREG          ; RESULT BACK TO A  (TOTAL WEIGHT - REGRIND COMPONENT)


NORPCL  LDX     #$0006          ;EXPONENT FOR DATA
        STX     MATHCREG        ;
        LDX     #CH1WACCO       ; ORDER WEIGHT FOR CHANNEL #1
        PULB
        PSHB                    ; COMP NO
        DECB
        ABX
        ABX
        ABX
        ABX
        ABX
        LDD     0,X
        STD     MATHCREG+2
        LDD     2,X
        STD     MATHCREG+4
        LDAA    4,X
        STAA    MATHCREG+6      ; COMPONENT WEIGHT
        JSR     FDIV
        JSR     FPTINC          ; INTEGER FORMAT
        PULB                    ; COMP NO
        LDX     #ORBPCNT1       ;
        DECB
        ABX
        ABX
        ABX
        LDAA    MATHDREG+4
        STAA    0,X             ; COMP % STORED.
        LDD     EREG
        STD     1,X             ; XXXX.YY % STORED.
        PULM    X
        AIX     #1
        LDAA    0,X
        CMPA    #SEQDELIMITER   ; AT END OF TABLE ?
        LBNE     NEXTCOMPPER
        RTS



;       BATCH WEIGHT COMPENSATION.
;
;       XX XX YY YY GRAMS / 3600 SECONDS  * ( XX XX .YY SECONDS SINCE LAST DUMP )
;       ( GRAMS / HOUR FOR LAST BATCH) / 3600 SECONDS ( BATCH SECONDS SO FAR)


BATCHWTCOMP:
        JSR     CLCREG7         ; RESET REG
        JSR     CLAREG7          ; C=0
        LDX     #0008           ;
        STX     MATHCREG        ;
        LDX     TPTKGH          ; A = TPTKGH
        STX     MATHCREG+2
        LDAA    TPTKGH+2
        STAA    MATHCREG+4
        LDX     #0004
        STX     AREG            ;
        LDX     #$3600          ; 1 HOUR OF SECONDS.
        STX     AREG+2
        JSR     FDIV            ;
        LDX     #MATHDREG
        JSR     FTAREG         ; RESULT TO AREG
        JSR     CLCREG7         ;  C = 0

        LDD     KGHRCOUNT       ; READ SECONDS
        JSR     HEXBCD2
        LDX     #0004
        STX     MATHCREG        ; C = KGHRCOUNT (BCD)
        STE     MATHCREG+2      ; SECONDS TRANSFERRED FOR ADDITION.
        JSR     FMUL            ; MULTIPLICATION.
        JSR     FPTINC          ; INTEGER FORMAT.
        LDD     MATHDREG+2      ; EXTRA ON LAST ROLL
        STD     BATCHUSEDWT  ;
        LDAA    MATHDREG+4      ;
        STAA    BATCHUSEDWT+2  ;

;       CALCULATE UNUSED BATCH WEIGHT
;

        JSR     CLRAC
        LDX     #PREVBATCHWEIGHT
        JSR     AMOVE           ; BATCH WEIGHT
        LDX     #BATCHUSEDWT
        JSR     CMOVE
        JSR     SUB
        LDX     #BATCHUNUSEDWT
        JSR     EMOVE
        JSR      CALBTPER         ; CALCULATE COMP COMPSENSATED WEIGHT
        RTS



; CALCULATION OF COMPONENT WEIGHTS FOR SYSTEM

CALBTPER:
        JSR     CLAREG          ; RESET A AND C REGISTERS.
        LDAB   #RAMBANK
        TBZK                    ;~;SET EK FOR BANK 1, I.E. RAM
        LDZ     #SEQTABLE       ;
REPCLW  LDX     #BATCHUNUSEDWT
        JSR     AMOVE           ; TO A REG
        JSR     CLCREG
        LDAB    0,Z             ; READ COMPONENT NO.
        ANDB    #$0F
        PSHB
        LDX     #PCNT1
        DECB
        ASLB
        ABX
        LDD     0,X
        STD     MATHCREG+3
        JSR     MUL             ; XXX.Y% X XXXXX.Y GRAMS
        LDX     #CMP1ACTWCM
        PULB
        DECB
        ABX
        ABX
        ABX
        LDD     EREG
        STD     0,X             ; STORE ACTUAL WEIGHT
        LDAA    EREG+2          ;
        STAA    2,X             ;
        AIZ     #1
        LDAA    0,Z
        CMPA    #SEQDELIMITER
        BNE     REPCLW
        RTS


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
        TST     ALLOWLARGEALARMBAND     ; CHECK IF A LARGE ERROR IS ALLOWED
        BNE     LRGLMCH
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
LRGLMCH:
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
        STAA    NOFLOWRATECALCULATION        ; NO FLOW RATE CALCULATION
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
        TST     REGRINDRETRY    ; CHECK FOR RETRY ON REGRIND COMPONENT
        BNE     NORMCOMP        ; RETRY ON REGRIND COMPONENT
        JSR     GETSEQPOINTER
        LDAB    0,X             ; READ POINTER CONTENTS
        ANDB    #REGBIT
        BEQ     NORMCOMP        ; DO NORMAL COMPARISION

        LDX     #CMP1ACTWGT     ; COMPONENT WEIGHT
        LDAB    COMPNO
        DECB
        ABX
        ABX
        ABX
        TSTW    0,X             ; IF ERROR IS LESS THAN 10 GRAMS THEN DO NOT INCREMENT RETRY COUNTER.
        BEQ     HASRET

NORMCOMP CMPA    #MAXRETRYCYCLES ; MAX NO OF RETRY CYCLES.
DOMCCM   BLS     HASRET          ; EXIT.

        LDAA    #1
        STAA    CORRECTRETRIES
        JSR     GETSEQPOINTER

        LDAA    0,X             ;
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

GETSEQPOINTER:
        PSHM    D
        TST     TOPUPF           ; TEST TOP UP MODE.
        BEQ     RDNRTB1         ; READ NORMAL TABLE.
        LDAA    TOPUPSEQUENCING ;
        CMPA    #FILLCOLOUR
        BNE     RDNRTB1         ; NO.
        LDX     LASTCOMPTABLEP  ;
        BRA     RDASN1          ; READ
RDNRTB1 LDX     SEQPOINTER      ; READ POINTER
RDASN1  PULM    D
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


LSPCHG  LDAA    LS5CFG          ; CHECK FOR CHANGE IN LINE SPEED
        BEQ     XITNCH          ; NO
        TST     MANATO          ; AUTO
        BEQ     CHKFCR          ; MANUAL EXIT.
        LDAA    CRLMODE         ; MODE OF OPERATION.
        CMPA    #HAULOFF        ;
        BEQ     XITNCH
        LDAA    #1
        STAA    STRUPF          ; SET SYSTEM TO START UP MODE.
        CLR     LS5CFG          ; RESET
        LDD     LSPD5S+2
        CPD     #ZEROLINESPEEDLIMIT
        BLS     XITNCH          ; EXIT IF LINE SPEED
CHKFCR  TST     FSTCTRL         ; FAST CONTROL RESPONSE
        BNE     XITNCH
        LDAA    BLENDERMODE     ; CHECK THE SET POINT MODE IN OPERATION
        CMPA    #GPMGPMENTRY    ; IS THIS GRAM PER METER MODE ?
        BLO     XITNCH
SPCHANG TST     MANATO          ; AUTO
        BEQ     XITNCH          ; MANUAL EXIT.
        LDAA    #1
        STAA    ESTCTRUSE       ; USE ESTIMATED VALUE FOR THE NEXT CORRECTION.
        JSR     NEWVOLSET
XITNCH  RTS


FRPER   LDX     #CH1PERCFR      ; % STORAGE.
        STX      PERSTORE
        LDX     #CH1WACCF
        LDAB   #RAMBANK
        TBZK                    ;~;SET EK FOR BANK 1, I.E. RAM
        LDZ     #TOTWACCF
        JSR     CALCRPPER       ; % CALC
        RTS

BRPER   LDX     #CH1PERCBR      ; % STORAGE.
        STX      PERSTORE
        LDX     #CH1WACCB
        LDAB   #RAMBANK
        TBZK                    ;~;SET EK FOR BANK 1, I.E. RAM
        LDZ     #TOTWACCB
        JSR     CALCRPPER       ; % CALC
        RTS

ORPER   LDX     #CH1PERCOR      ; % STORAGE.
        STX      PERSTORE
        LDX     #CH1WACCO
        LDAB   #RAMBANK
        TBZK                    ;~;SET EK FOR BANK 1, I.E. RAM
        LDZ     #TOTWACCO
        JSR     CALCRPPER       ; % CALC
        RTS

SHPER   LDX     #CH1PERCS      ; % STORAGE.
        STX      PERSTORE
        LDX     #CH1WACCS
        LDAB   #RAMBANK
        TBZK                    ;~;SET EK FOR BANK 1, I.E. RAM
        LDZ     #CURSHIFTWEIGHT
        JSR     CALCRPPER       ; % CALC
        RTS

HRPER   LDX     #CH1PERCH      ; % STORAGE.
        STX      PERSTORE
        LDX     #CH1WACCH
        LDAB   #RAMBANK
        TBZK                    ;~;SET EK FOR BANK 1, I.E. RAM
        LDZ     #TOTWACCH
        JSR     CALCRPPER       ; % CALC
        RTS

LTPER   LDX     #CH1PERCM      ; % STORAGE.
        STX      PERSTORE
        LDX     #CH1WACCM
        LDAB   #RAMBANK
        TBZK                    ;~;SET EK FOR BANK 1, I.E. RAM
        LDZ     #TOTWACCM
        JSR     CALCRPPER       ; % CALC
        RTS



;       CALCULATE REPORT % GIVEN (PERSTOR) - % STORAGE
;                                    X     - COMP WT ADDRESS
;                                    Z     - TOTAL WT

CALCRPPER:
        LDAB   #RAMBANK
        TBZK                    ;~;SET EK FOR BANK 1, I.E. RAM
        STX     COMPWTADD       ; COMPONENT WT ADDRESS
        LDX     PERSTORE
;        LDAA    #3*8            ;
        LDAA    #THREE
        LDAB    #MAXCOMPONENTNO
        MUL
        TBA
        JSR     CLRMEM
        LDX     #SEQTABLE       ; TABLE OF COMPONENTS.
NXTPER  PSHM    X               ;
        LDAB    0,X
        CMPB    #SEQDELIMITER
        BEQ     XITPER          ; EXIT
        ANDB    #$0F
        PSHB                    ; SAVE COMP NO.
        LDX     COMPWTADD       ; ADDRESS
        DECB
        ABX
        ABX
        ABX
        ABX
        ABX
        LDD     #8              ; EXPONENT
        STD     MATHCREG        ;
        LDD     0,X
        STD     MATHCREG+2
        LDD     2,X
        STD     MATHCREG+4      ; CREG = XX XX XX .YY YY KGS
        LDAA    4,X
        STAA    MATHCREG+6      ;
        LDD     #6
        STD     AREG
        LDD     0,Z
        STD     AREG+2
        LDD     2,Z
        STD     AREG+4          ; AREG = XX XX XX .YY YY KGS
        LDAA    4,Z
        STAA    AREG+6          ;
        JSR     FDIV            ;C/A
        JSR     FPTINC
        LDX     PERSTORE        ; STORAGE
        PULB
        DECB
        ABX
        ABX
        ABX
        LDD     MATHDREG+3
        STD     0,X
        LDAA    EREG
        STAA    2,X
        PULM    X
        AIX     #1              ; TABLE POINTER ADVANCED.
        LBRA     NXTPER
        RTS
XITPER  PULM    X
        RTS





;
; THIS S/R INSERTS A 7 BYTE DATA BLOCK INTO QUE #2
;
Q2DAEN LDAA   RRCDENP2   ;~LOAD QUE #2 DATA POINTER
       CMPA   RRAVRGK2   ;~COMPARE WITH AVERAGING FACTOR
       BNE    INCQU2   ;~JUMP IF NOT AT TOP OF QUE #2
       CLRA            ;~AT TOP OF QUE:RESET POINTER
       LDX    #RRDATST2  ;~RESET DATA ADDRESS POINTER
       BRA    QUEDA2   ;~INSERT DATA INTO QUE #2
INCQU2 LDX    RRQCADR2   ;~LOAD DATA ADDRESS POINTER #2
       INCA            ;~INCREMENT DATA POINTER #2
       AIX     #7
QUEDA2 STAA   RRCDENP2   ;~STORE NEW DATA POINTER
       STX    RRQCADR2   ;~STORE NEW ADDRESS POINTER
       JSR    DAMOV2   ;~COPY DATA INTO QUE
       RTS             ;~END OF INSERTION S/R #2
;
; THIS S/R PERFORMS THE SUMMATION OF ALL READINGS
; IN QUE #2. IT OPERATES IN A 'ROUND ROBIN' FASHION
; AS DESCRIBED PREVIOUSLY.
;
SUMDT2 LDAA   RRSUMCT2   ;~LOAD SUMMATION #2 DATA COUNTER
       CMPA   RRAVRGK2   ;~COMPARE WITH AVERAGING CONSTANT
       BEQ    SUMFL2   ;~SUMMATION FULL IF EQUAL
       BLS    SMSTF2   ;~SUMMATION STILL FILLING IF LESS
       LDX    #RRSUMTT2  ;~SUMMATION TOO FULL :RESET
       JSR    CLRNM2   ;~CLEAR SUMMATION TOTAL
       CLRA            ;~RESET SUMMATION COUNT
SMSTF2 INCA            ;~INCREMENT SUMMATION DATA COUNT
       STAA   RRSUMCT2   ;~STORE NEW COUNT
       LDX    RRQCADR2   ;~LOAD CURRENT DATA ADDRESS
       JSR    FTCREG   ;~COPY TO C-REG
       LDX    #RRSUMTT2  ;~LOAD SUMMATION TOTAL S.A.
       JSR    FTAREG   ;~COPY TO A-REG
       JSR    FADD     ;~ADD NEW READING TO SUMMATION
       LDX    #RRSUMTT2  ;~LOAD S.A. OF SUMMATION TOTAL
       JSR    FFDREG   ;~COPY NEW SUMMATION TOTAL
       RTS             ;~END OF SUMMMATION S/R
SUMFL2 LDAA   RRCDENP2   ;~LOAD CURRENT DATA POINTER
       CMPA   RRAVRGK2   ;~CHECK FOR TOP OF QUE #2
       BNE    SMADR2   ;~NOT AT TOP: FORM DATA ADDRESS
       LDX    #RRDATST2  ;~RESET DATA ADDRESS TO BOTTOM
       BRA    SMNUM2   ;~PERFORM F.I.F.O. SUMMATION
SMADR2 LDX    RRQCADR2   ;~LOAD CURRENT DATA ADDRESS
       AIX     #7
SMNUM2 JSR    FTCREG   ;~COPY DATA TO C-REG
       LDX    #RRSUMTT2  ;~LOAD SUMMATION #2 S.A.
       JSR    FTAREG   ;~COPY TO A-REG
       JSR    FSUB     ;~SUBTRACT OLDEST ENTRY
       LDX    #AREG    ;~LOAD S.A. OF A-REG
       JSR    FFDREG   ;~COPY RESULT BACK TO A-REG
       LDX    RRQCADR2   ;~LOAD CURRENT ENTRY ADDRESS
       JSR    FTCREG   ;~COPY DATA TO C-REG
       JSR    FADD     ;~ADD NEW ENTRY TO SUMMATION
       LDX    #RRSUMTT2  ;~LOAD S.A. OF SUMMATION #2 STORE
       JSR    FFDREG   ;~STORE NEW SUMMATION TOTAL
       RTS             ;~EXIT S/R
;
; THIS S/R DIVIDES SUMMATION TOTAL #2 BY ITS SAMPLE
; SIZE TO PRODUCE THE AVERAGED ERROR. THE RESULT IS THEN
; STORED.
;
DIVDT2 LDX    #RRSUMTT2  ;~LOAD S.A. OF SUMMATION #2
       JSR    FTCREG   ;~COPY TO C-REG
       LDX    #AREG    ;~LOAD S.A. OF A-REG
       JSR    CLRNM2   ;~CLEAR A-REG
       LDAA   #$02     ;~LOAD EXP. SIZE
       STAA   AREG+1   ;~STORE EXP.
       LDAA   RRSUMCT2   ;~LOAD SUMMATION DATA SIZE COUNTER
       STAA   AREG+2   ;~STORE MAGNITUDE TERM
       JSR    FDIV     ;~DIVIDE SUMMATION BY SAMPLE SIZE
       JSR      FPTINC  ; CONVERT BACK TO INTEGER
       LDD      MATHDREG+3
       STD      RRTPTKGH
       LDAA    EREG ;
       STAA    RRTPTKGH+2
       RTS             ;~END OF DIVISION
;
; THIS ROUTINE COPIES THE INSTANTENOUS CONTROL ERROR
; FROM ITS STORE TO THE ADDRESS POINTED TO BY THE X-REG.
;
DAMOV2 LDD     #4      ;EXPONENT
       STD     0,X
       LDD    TPTKGH
       STD      2,X
       LDAA   TPTKGH+2
       STAA     4,X
       CLRW     5,X
       RTS
;

;       STAA   RRAVRGK2   ;~STORE NEW AVERAGING FACTOR
RESRROB:
       LDAA    BATINKGHAVG
       STAA     RRAVRGK2
       CLR    RRSUMCT2   ;~RESET QUE #2 SAMPLE COUNTER
       CLR    RRCDENP2   ;~RESET QUE #2 DATA POINTER
       LDX    #RRDATST2  ;~LOAD QUE #2 DATA S.A.
       STX    RRQCADR2   ;~STORE IN QUE #2 ADDRESS POINTER
       LDX    #RRSUMTT2  ;~LOAD S.A. OF SUMMATION TOTAL
       JSR    CLRNM2   ;~RESET SUMMATION TOTAL
       RTS
GENRRKGH:
       TST     FIRSTBATCH
       BNE     XITRRB
       JSR    Q2DAEN   ;~CALL INSERTION S/R #2
       JSR    SUMDT2   ;~CALL SUMMATION S/R #2
       JSR    DIVDT2   ;~CALL DIVISION S/R #2
       JSR    CONVRRKGHRI ; IMPERIAL CONVERSION.
XITRRB RTS




; THIS PROGRAM SUBTRACTS THE COMPENSATED WEIGHT FROM THE TOTAL ROLL
; COMPONENT WEIGHTS WEIGHTS.

SUBCOMPWT:
        LDAB    #1
REPADDIT:
        PSHM    X               ; SAVE ADDRESS STORAGE
        PSHB                    ;       SAVE COMPONENT NO
        DECB
        ABX
        ABX
        ABX
        ABX
        ABX                     ; ADDRESS FOR DATA STORAGE.
        JSR     ITAREG          ;
        LDX     #CMP1ACTWCM     ; COMP 1 ACTUAL WEIGHT
        PULB
        PSHB
        DECB                    ;
        ABX
        ABX
        ABX
        JSR     CMOVE           ; COMP WEIGHT TO A REG 00 00 XX XX X.Y GRAMS
        JSR     SUB
        PULB
        PULM    X               ; COMP 1 ACTUAL WEIGHT
        PSHM    X
        PSHB
        DECB                    ;
        ABX
        ABX
        ABX
        ABX
        ABX
        JSR     IFEREG          ; STORE ACCUMULATED DATA.
        PULB
        PULM    X
        CMPB    NOBLNDS         ; AT LAST ONE
        BEQ     YESEXIT         ;
        INCB                    ; TO NEXT BLEND.
        BRA     REPADDIT

YESEXIT:
        JSR     GENTOTALS       ; GENERATE TOTALS
        JSR     ORDERPER        ; CALCULATE ACTUAL %
        RTS



;       ADDS COMPONENT COMPENSATED WEIGHT TO THE INDIVIDUAL COMPONENT DATA.
;       X REG POINTS TO THE COMPONENT WEIGHTS


ADDCOMPWT:
        LDAB    #1
REPADD:
        PSHM    X               ; SAVE ADDRESS STORAGE
        PSHB                    ;       SAVE COMPONENT NO
        DECB
        ABX
        ABX
        ABX
        ABX
        ABX                     ; ADDRESS FOR DATA STORAGE.
        JSR     ITAREG          ;
        LDX     #CMP1ACTWCM     ; COMP 1 ACTUAL WEIGHT
        PULB
        PSHB
        DECB                    ;
        ABX
        ABX
        ABX
        JSR     CMOVE           ; COMP WEIGHT TO A REG 00 00 XX XX X.Y GRAMS
        JSR     ADD
        PULB
        PULM    X               ; COMP 1 ACTUAL WEIGHT
        PSHM    X
        PSHB
        DECB                    ;
        ABX
        ABX
        ABX
        ABX
        ABX
        JSR     IFEREG          ; STORE ACCUMULATED DATA.
        PULB
        PULM    X
        CMPB    NOBLNDS         ; AT LAST ONE
        BEQ     YESEXAD         ;
        INCB                    ; TO NEXT BLEND.
        BRA     REPADD

YESEXAD:
        JSR     GENTOTALS       ; GENERATE TOTALS
        JSR     ORDERPER        ; CALCULATE ACTUAL %
        RTS

;       FRONT ROLL COMPENSATION.
;

PRECOMPFROLL:
        JSR     BATCHWTCOMP     ; CALCULATE UNUSED WEIGHT
        LDX     #CH1WACCF
        JSR     SUBCOMPWT
        RTS

POSTCOMPFROLL:
        LDX     #CH1WACCF
        JSR     ADDCOMPWT
        RTS

;       BACK ROLL COMPENSATION.
;

PRECOMPBROLL:
        JSR     BATCHWTCOMP     ; CALCULATE UNUSED WEIGHT
        LDX     #CH1WACCB
        JSR     SUBCOMPWT
        RTS

POSTCOMPBROLL:
        LDX     #CH1WACCB
        JSR     ADDCOMPWT
        RTS


;       HOURLY COMPENSATION.
;
PRECOMPHOUR:
        JSR     BATCHWTCOMP     ; CALCULATE UNUSED WEIGHT
        LDX     #CH1WACCH
        JSR     SUBCOMPWT
        RTS

POSTCOMPHOUR:
        LDX     #CH1WACCH
        JSR     ADDCOMPWT
        RTS


;       SHIFT COMPENSATION.
;
PRECOMPSHIFT:
        JSR     BATCHWTCOMP     ; CALCULATE UNUSED WEIGHT
        LDX     #CH1WACCS
        JSR     SUBCOMPWT
        RTS

POSTCOMPSHIFT:
        LDX     #CH1WACCS
        JSR     ADDCOMPWT
        RTS

;       ORDER COMPENSATION
;
PRECOMPORDER:
        JSR     BATCHWTCOMP     ; CALCULATE UNUSED WEIGHT
        LDX     #CH1WACCO
        JSR     SUBCOMPWT
        RTS

POSTCOMPORDER:
        LDX     #CH1WACCO
        JSR     ADDCOMPWT
        RTS


; CALCULATE MAX KG/H.
; MAX BATCHWEIGHT * 3600/LOADTIME.
; RESULT IS XXX.Y


CALCMAXKGHR:
        JSR     CLRAC
        LDX     #$360
        STX     AREG+3          ; 360 TO A.
        LDX     DFTW1           ; TARGET WT TO C
        STX     MATHCREG+3      ;
        JSR     MUL
        LDX     #MATHCREG
        JSR     IFEREG          ; RESTORE VALUE.
        JSR     CLAREG
        LDD     CYCLELOADTIMEPRN
        STD     AREG+3          ;
        JSR     DIV             ; C/A
        RTS




; CALCULATE ADDITIONAL PULSES FOR BATCH LENGTH CALCULATION.
;
;  ESTADDSEIPULSES=(LSPDFRACTIONCTR ->BCD / PITINDEX) * PULSES PER SECOND.
;
;
CALCSEIADDPULSES:
        LDX     #BATCHPULSEACC   ; PULSES FOR ALGORITHM.
        LDD     ESTADDSEIPULSES  ;
        JSR     SUBEFMX          ; ADDITION OF PULSES.
        LDD     LSPDFRACTIONCTR        ; LSPD FRACTION OF 1 SECOND
        JSR     HEXBCD2         ;
        JSR     CLRAC
        STE     MATHCREG+2          ;
        LDD     SEIPULSEPSEC        ; PULSES PER SECOND.
        JSR     HEXBCD2             ; TO BCD
        STE     AREG+3              ;
        JSR     MUL                 ; MULTIPLY.
        LDX     #MATHCREG
        JSR     IFEREG              ; TRANSFER FROM E REGISTER.
        JSR     CLAREG              ; A = 0
        LDD     #PITINDEXBCD
        STD     AREG+3          ;
        JSR     DIV             ; C/A FRACTION OF SECOND IS CACULATED XX.YY
        LDAA    #2
        LDX     #EREG
        JSR     GPRND1            ;~ROUND TO 3 DIGITS
        LDD     EREG+2
        JSR     BCDHEX2
        STD     ESTADDSEIPULSES  ; STORE ADDITIONAL PULSES.
        LDD     SEIPULSEPSEC
        SUBD    ESTADDSEIPULSES  ; PULSES LEFT IN SECOND.
        STD     ESTSUBSEIPULSES  ; STORE PULSES LEFT.
        LDX     #BATCHPULSEACC   ; PULSES FOR ALGORITHM.
        LDE     ESTADDSEIPULSES
        JSR     ADDETOX          ; ADDITION OF PULSES.
        RTS


CALCWEIGHTPERLENGTH:
        JSR     CLRAC           ; RESET REGISTERS
        LDX     SETROLLWEIGHT   ; SET ROLL WEIGHT
        STX     MATHCREG+1
        LDAA    SETROLLWEIGHT+2 ;
        STAA    MATHCREG+3      ;
        CLR     MATHCREG+4      ; RESET LSB
        LDX     #SETROLLLENGTH  ; ROLL LENGTH
        JSR     AMOVE           ; A REG
        JSR     DIV             ; WEIGHT PER LENGTH
        LDE     EREG+2          ; RESULT.
        STE     WTPMSP          ; W/M SETPOINT.
        LDAA    EREG+4          ; RESULT.
        STAA    WTPMSP+2        ; W/M SETPOINT.
        RTS

;
;  CMXDA1 - CALCULATE MAX D/A # 1 VALUE BASED ON MAX RPM AND D/A CTS PER RPM
;           MAX D/A #1 = MAX RPM * D/A COUNTS PER RPM
CMXDA1  JSR     CLAREG
        JSR     CLCREG          ; RESET A AND C REG
        LDD     MAXEXSP         ;
        STD     AREG+3
        LDD     #$4096
        STD     MATHCREG+3
        JSR     MUL             ; MAX SP * 4096
        LDX     #EREG
        JSR     ITCREG
        LDD     #$1000
        STD     AREG+3          ;
        JSR     DIV             ; C/A MAX (SP% * 4096) / 99.9
        LDD     EREG+3          ; RESULT.
        JSR     BCDHEX2         ; CONVERT TO HEX.
        STD     MAXDA1          ; STORE MAX RESULT.
        RTS



; ROUTINE TO CALCULATE 0 - 10VOLTS OUTPUT.
;

CALCZEROTO10VOLTS:
        LDD     NKEYBUF+2       ; READ DATA TO BE STORED
        STD     ZEROTO10VSTO    ; STORE DATA.
        LDAA    NKEYBUF+4       ;
        STAA    ZEROTO10VSTO+2  ; 0 - 10 VOLTS.
        TST     MANATO          ; AUTO ?
        BNE     XITZTT          ; EXIT
        LDAA    PASWRD
        BNE     XITZTT          ; XIT ZERO TO 10.
        LDD     PASWRD+1
        CPD    #$6553
        BNE     XITZTT
        JSR     CLRAC           ; A=C=0
        LDD     #$4095
        STD     AREG+3          ; A = 4095
        LDD     NKEYBUF+2       ; READ VOLTAGE.
        STD     MATHCREG+2      ;
        LDAA    NKEYBUF+4       ; LSB
        STAA    MATHCREG+4      ;
        JSR     MUL             ; CALCULATE 4095 * XX.YYYY VOLTS ENTRY.
        LDX     #EREG
        JSR     ITCREG          ; RESULT TO A.
        JSR     CLAREG          ; A = 0
        LDAA    #$10
        STAA    AREG+2          ; 10.0000 VOLTS.
        JSR     DIV             ; C/A  4095 * XX,YYYY / 10.0000
        LDD     EREG+3
        JSR     BCDHEX2         ; CONVERT TO HEX
        STD     DA1TMP          ; STORE
        JSR     STARTSEIDAWR    ; INITIATE WRITE TO D/A
XITZTT  RTS




; THIS ROUTINE CHECKS IF THE BLENDER IN OFFLINE MODE HAS REACHED THE BATCH WEIGHT
; IF IT HAS THEN PAUSE  MODE IS INITIATED.


CHECKIFATBATCHWEIGHT:
        LDAA    BLENDERMODE             ; CHECK MODE.
        CMPA    #GPMGPMENXXKGS          ; ALARM ON XX KGS
        BEQ     CHKATT                  ; CHECK IF AT TARGET.
        CMPA    #OFFLINEMODE            ; OFFLINE MODE.
        LBNE     XITBWC                  ; EXIT.
        LDAA    OFFLINETYPE             ; CHECK TYPE.
        CMPA    #OFFLINETYPEXXKG        ; DISPENSE XX KGS.
        LBNE     XITBWC                  ;
CHKATT  TST     STOPINNBATCHES          ; TIME TO STOP ?
        LBNE     CHKIFSTOP
        JSR     CLRAC                   ; A =C = 0
        LDX     TKGHSP
        STX     AREG
        LDAA    TKGHSP+2
        STAA    AREG+2
        LDX     #TOTWACCBAT             ; TOTAL WEIGHT.
        JSR     ITCREG                  ; XX XX .YY YY KGS TO
        JSR     SUB                     ; SETPOINT - ACTUAL.
        LDX     #TOTWACCBATLEFT         ; AMOUNT LEFT
        JSR     IFEREG
        LDAA    BLENDERMODE             ; CHECK MODE.
        CMPA    #GPMGPMENXXKGS          ; ALARM ON XX KGS
        LBEQ     CHKDB                   ; CHECK IF AT TARGET.
        JSR     CLRAC
        LDX     #AREG
        JSR     IFEREG
        LDD     DFTW1                   ; READ BATCH SIZE
        ASLD
        ASLD
        ASLD
        ASLD
        PSHM    D
        STD     MATHCREG+2
        JSR     SUB                     ; A - C

        JSR     CLRAC
        PULM    D
        STD     MATHCREG+2
        LDX     #AREG
        JSR     IFEREG                  ;
        JSR     SUB
        TST     MATHDREG
        LBEQ     NOTATT                ; NOT WITHIN 2 BATCHES

        JSR     CLRAC
        LDX     #TOTWACCBATLEFT         ; AMOUNT LEFT
        JSR     ITCREG
        LDAA    #2
        STAA    AREG+4
        JSR     DIV


        LDX     DFTW1
        STX     DFTW1STORE
        LDE     EREG+2
        LSRE
        RORB
        LSRE
        RORB
        LSRE
        RORB
        LSRE
        RORB
        STE     DFTW1
        STAB     DFTW1+2
        JSR     CALCULATEEEPROMCHECKSUM
        JSR     NEWTARWEIGHTENTRY
        LDAA    #2
        STAA    STOPINNBATCHES          ; STOP IN N BATCHES
        LBRA     XITBWC

CHKIFSTOP:
        LDAA    STOPINNBATCHES
        CMPA    #2                      ; ONE BATCH LEFT ?
        BNE     NOTLST                  ; NO
        DEC     STOPINNBATCHES
        JSR     CALCULATELASTBATCH      ; CALCULATE LAST BATCH.
        BRA     XITBWC

NOTLST  DEC     STOPINNBATCHES
        BNE     XITBWC

        LDX     DFTW1STORE
        STX     DFTW1
        JSR     CALCULATEEEPROMCHECKSUM
        JSR     NEWTARWEIGHTENTRY


;        TST     MATHDREG                    ; CHECK POLARITY OF RESULT.
;        BEQ     NOTATT                  ; NOT AT TOTAL.
        LDAA    #1
        STAA    BATCHREADY
        STAA    OFFLINEBATCHFILLED

;        JSR     STARTUPON
        LDX     #TOTWACCBAT             ; TOTAL Shift WT. ACC
        LDAB    #5*9                    ; 8 BLENDS + PIB + TOTAL (6 bytes/accum.)
        JSR     CLRBBS                  ; CLEAR
        JSR     GENBATREPORT            ; BATCH REPORT GENERATION.
        TST     NOTARGETALARM
        BNE     ATTALA
        LDAA    #1
        STAA    SIGNALATTARGETALARMFLAG  ; SIGNAL ALARM


ATTALA:
        LDAA    BLENDERMODE             ; CHECK MODE.
        CMPA    #GPMGPMENXXKGS          ; ALARM ON XX KGS
        BEQ     XITBWC                  ; CHECK IF AT TARGET.
        JSR     PAUSEON                 ; PAUSE BLENDER IMMEDIATELY
        BRA     XITBWC                  ; EXIT.
NOTATT  CLR     BATCHREADY
        LDAA    BLENDERMODE             ; CHECK BLENDER MODE.
        CMPA    #GPMGPMENXXKGS
        BNE     XITBWC                  ; NO PAUSE IF GPM MODE.
CHKDB   LDX     #AREG
        JSR     IFEREG                  ; DIFFERENCE TO A
        JSR     CLCREG                  ; C = 0
        LDD     DBXXKGS                 ; DEADBANDD IN KILOS.
        STD     MATHCREG+1              ; STORE.
        JSR     SUB                     ; DIFFERENCE - DEADBAND.
        TST     MATHDREG                ; CHECK POLARITY OF RESULT.
        BEQ     XITBWC                  ; NOT AT TOTAL.
        JSR     INDICATETOTALREACHED
XITBWC  RTS

CALCULATELASTBATCH:
        JSR     CLRAC                   ; A =C = 0
        LDX     TKGHSP
        STX     AREG
        LDAA    TKGHSP+2
        STAA    AREG+2
        LDX     #TOTWACCBAT             ; TOTAL WEIGHT.
        JSR     ITCREG                  ; XX XX .YY YY KGS TO
        JSR     SUB                     ; SETPOINT - ACTUAL.
        LDX     #TOTWACCBATLEFT         ; AMOUNT LEFT
        JSR     IFEREG
        LDE     EREG+2
        LSRE
        RORB
        LSRE
        RORB
        LSRE
        RORB
        LSRE
        RORB
        STE     DFTW1                   ; TARGET FOR LAST BATCH.
        STAB     DFTW1+2
        JSR     CALCULATEEEPROMCHECKSUM
        JSR     NEWTARWEIGHTENTRY
        RTS








SIGNALATTARGETALARM:
        LDAA    MBPROALRM+1               ;
        ANDA    #ATTARWTALARMBIT        ; AT TARGET WEIGHT BIT SET.
        BNE     ATTAL1                  ; AT TARGET ALREADY ACTIVE.
        LDAA    #ATTARWTALARM           ; AT TARGET WEIGHT ALARM.
        JSR     PTALTB                  ; GENERATE ALARM.
ATTAL1  RTS


INDICATETOTALREACHED:
        TST     NOTARGETALARM
        BNE     ATTALA1
        LDAA    MBPROALRM+1               ;
        ANDA    #ATTARWTALARMBIT        ; AT TARGET WEIGHT BIT SET.
        BNE     ATTALA1                 ; AT TARGET ALREADY ACTIVE.
        LDAA    #ATTARWTALARM           ; AT TARGET WEIGHT ALARM.
        JSR     PTALTB                  ; GENERATE ALARM.
ATTALA1 RTS


;
;
;       CHECK FOR END OF CYCLE PAUSE.
;
CHKFOREOCPAUSE:
        LDAA    CYCLEPAUSETYPE          ; ANY PAUSING.
        BEQ     XITEOCP                 ; NO END OF CYCLE PAUSE.
        TST     MULTIPLEFILLING
        BNE     XITEOCP
        TST     PAUSEONACTIVE           ; PAUSE ON FLAG ACTIVE
        BNE     ISEXPA                  ; EXTERNAL PAUSE

        CLR     CYCLEPAUSETYPE          ; RESET FLAG.
        CMPA    #PAUSEATENDOFCYCLE      ; END OF CYCLE PAUSE ?
        BEQ     NORPAUS                ; EXIT.
        CMPA    #OFFLINEPAUSEATENDOFCYCLE      ; OFFLINE PAUSE ?
        BEQ     OFFPAUS                ; EXIT.
NORPAUS JSR     PAUSEON                ; PAUSE !!
XITDMP  JSR     OPENFEED               ; OPEN PINCH VALVE.
        JSR     OPENDUMP               ; OPEN DUMP FLAP
        TST     CLEANATENDOFCYCLE
        BEQ     XITEOCP
        CLR     CLEANATENDOFCYCLE
        JSR     STARTMOULDCLEAN
XITEOCP RTS
OFFPAUS JSR     OFFLINEPAUSEON
        BRA     XITDMP


ISEXPA  JSR     PAUSEON                ; PAUSE !!
        JSR     CLOSEFEED              ; CLOSE FEED.
        JSR     CLOSEDUMP              ; CLOSE DUMP FLAG
        BRA     XITEOCP






;       FUNCTION NAME :         RESETRROBINCPI
;       DESCRIPTION             SETS FLAG IGNORECPIERRN TO ENSURE THAT THE CPI ERROR IS IGNORED
;                               WHEN THE FLOW RATE (CPI) IS CALCULATED.
;       PARAMTERS               B POINTS TO THE COMPONENT NO.

;
RESETRROBINCPI:
        PSHM    D,X              ; SAVE REGISTERS.
        LDX     #RESETRROBINCMP1  ; FLAG TO IGNORE CPI ERROR.
        DECB                    ; COMPONENT NO.
        ABX
        LDAA    #RESETRRNO
        STAA    0,X             ; IGNORE THIS ERROR.
        PULM    D,X             ; RECOVER REGISTERS.
        RTS.



SETTOIGNORECPI:
        PSHM    D,X              ; SAVE REGISTERS.
        LDX     #IGNORECPI1      ; FLAG TO IGNORE CPI ERROR.
        DECB                    ; COMPONENT NO.
        ABX
        LDAA    #IGNORCPINO
        STAA    0,X             ; IGNORE THIS ERROR.
        PULM    D,X             ; RECOVER REGISTERS.
        RTS.



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



CHKFORRETRIES:
        LDAB   #RAMBANK
        TBZK                    ;~;SET EK FOR BANK 1, I.E. RAM
        LDX     #SEQTABLE
REPRETC PSHM    X
        LDAB    0,X
        CMPB    #SEQDELIMITER   ; AT END.
        BEQ     FINRET
        LDAB    0,X             ; READ COMPONENT NO.
        ANDB    #$0F            ; READ COMPONENT.
        PSHB
;        LDZ     #RETRYCTR1
        PSHM    X
        JSR     GETRETRYCTR
        XGEX
        XGEZ                    ; X -> Z
        PULM    X

        DECB
        ABZ                     ; Z POINTS TO RETRY COUNTER.
        PULB
;        LDX     #HASRETRIED1    ; LOAD RETRY FLAG.
        JSR     GETHASRETRIEDPTR
        DECB
        ABX
        TST     0,X             ; HAS THIS COMPONENT RETRIED.
        BNE     HASRETR         ; NO
        CLR     0,Z
HASRETR PULM    X               ; PULL X
        AIX     #1              ; INCREMENT
        BRA     REPRETC         ; REPEAT RETRY CHECK.

FINRET  PULM    X
;        LDX     #HASRETRIED1
        JSR     GETHASRETRIEDPTR
        LDAA    #MAXCOMPONENTNO
        JSR     CLRMEM          ; CLEAR
        RTS



CHECKIFCOMPCFGCHANGED:
        TST     CONFIGCHANGED   ; HAS THIS CHANGED.
        BEQ     NOCFC           ; NO CONFIG CHAGNED.
        CLR     CONFIGCHANGED
        JSR     SETALLCOMPTOFILLBYWEIGHT
NOCFC   RTS




; SET ALL COMPONENTS TO FILL BY WEIGHT.



SETALLCOMPTOFILLBYWEIGHT:

;        LDAA    #8              ;
        LDAA    #MAXCOMPONENTNO ; OUT COUNT
        LDX     #SEQTABLE       ; TABLE OF COMPONENTS.
REPFBW  LDAB    0,X
        CMPB    #SEQDELIMITER
        BEQ     XITFBW          ; EXIT
        LDAB    0,X
        ORAB    #FILLBYWEIGHT   ; FILL BY WEIGHT
        STAB    0,X
        AIX     #1
        DECA
        BNE     REPFBW
XITFBW  RTS





;*****************************************************************************;
; FUNCTION NAME : GRAMPERQTOGPM                                               ;
; FUNCTION      : CONVERT GRAMS PER SQUARE METER TO GRAMS PER METER           ;
; INPUTS        : WIDTHSP WIDTH SET POINT.                                    ;
;               ; GPM2SPT GRAMS PER SQUARE METER.                             ;
;*****************************************************************************;


GRAMPERQTOGPM:
        JSR     CLRAC
        LDX     WIDTHSP
        STX     AREG+3

        LDX    GPM2SPT          ; g/m2 SETPOINT
        STX    MATHCREG+2
        LDAA   GPM2SPT+2
        STAA   MATHCREG+4
        JSR    MUL              ; CALCULATE G/M

        LDX      #EREG
        LDAA     #3
        JSR      GPRND1            ;~ROUND A FURTHER 2 DIGITS
        LDX      #EREG
        LDAB     #5
        LDAA     #1
        JSR      GPSFTR            ;~MOVE 2 DIGITS RIGHT
        LDX      EREG
        STX      WTPMSP
        LDX      EREG+2
        STX      WTPMSP+2
        RTS






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
        JSR      FPTINC            ; FLOAT TO INTEGER CONVERSION.

        LDX      MATHDREG+3
        STX      GPM2VAR
        LDAA     EREG
        STAA     GPM2VAR+2
        RTS


;
; SSPEED IS A ROUTINE TO CALCULATE MAIN SCREW'S SPEED IN RPM
;        SSRPM = SS A/D CTS * SS CONST.

SSAD_TARE:
       LDD    SEIADREADINGAVG
       JSR    HEXBCD2           ; HEX TO BCD
       STAB   HCAD0BCD
       STE    HCAD0BCD+1
       LDD      SEIADREADING
       CPD      SSTARE
       BHI      CALDIF          ; CALCULATE DIFFERENCE.
       LDE      #0
       CLRB
       BRA      STRDIF          ; STORE DIFFERENCE
CALDIF LDD    SEIADREADINGAVG
       SUBD   SSTARE            ; SUBTRACT TARE VALUE
       JSR    HEXBCD2           ; HEX TO BCD
STRDIF CLRW   AREG              ; RESET A REGISTER.
       STAB   AREG+2
       STE    AREG+3            ; STORE TO A REGISTER.
SSPEED2 RTS



SSPEED:
;       TST    SSSTYPE            ; TACHO(0) SIGNAL?
;       LBNE   SSPEEDC            ; GO TO SCREW SPEED ON COMMs
       JSR    SSAD_TARE          ; A/D - TARE COUNTS
       JSR    CLCREG           ;~CLEAR CREG
       LDX    SSCON1            ;~SCREW SPEED CONST (4 DIGITS)
       STX    MATHCREG+3        ;~PUT IN L.S. BYTES OF CREG
       JSR    MUL               ;~SS A/D CTS * CONST.
       LDX    #EREG
       LDAA   #3
       JSR    GPRND1            ;~ROUND ANSWER TO 3 DIGITS
       LDX    #EREG
       LDAB   #5
       LDAA   #3
       JSR    GPSFTR            ;~MOVE 3 DIGITS RIGHT
       LDX    #SSRPM2D          ;~RPM TO 2 DECIMAL PLACES
       JSR    EMOVE             ;~MOVE TO RPM STORAGE

       LDX    #EREG
       LDAA   #2
       JSR    GPRND1            ;~ROUND A FURTHER 2 DIGITS
       LDX    #EREG
       LDAB   #5
       LDAA   #2
       JSR    GPSFTR            ;~MOVE 2 DIGITS RIGHT

       LDX    #SSRPM                    ;~RESULT RPM STORAGE
       JSR    EMOVE             ;~STORE VALUE
SSPEEDC:
       JSR    CPYERPMMB         ; COPY EXT. RPM -> MODBUS TABLE
       LDX      SSRPM
       BNE      SSOK
       NOP
SSOK:
       RTS

AVERAGEADREADING:
        LDD     SUMAD1+1                ;SUMMATION OF 132 RESULTS
        ADDD    SEIADREADING
        STD     SUMAD1+1
        LDAB    SUMAD1
        ADCB    #0
        STAB    SUMAD1                  ;ADD NEW VALUE TO SUM
        INC     SAMPLEAD1               ;NO. OF ENTRIES
        LDAB    SAMPLEAD1
        CMPB    #4                      ; 0.5 SECOND AVERAGE.
        BLO     XITHSS                  ; EXIT
        CLR     SAMPLEAD1               ; COUNTER RESET TO 0
        LDAB    SUMAD1                  ; SHIFT TO DIVIDE
        LDE     SUMAD1+1
        CLRW    SUMAD1
        CLR     SUMAD1+2
        LSRB
        RORE
        LSRB
        RORE
        STE      SEIADREADINGAVG
XITHSS  RTS


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


; CHECK IF COMPONENT SHOULD BE SKIPPED

CHKFORSKIPPEDCOMP:
        TST     STAGEFILLEN
        BEQ     SKPCMC                  ; SKIP COMPONENT CHECK
        LDAB    #RAMBANK
        TBZK
        LDX     SEQPOINTER             ; LOAD START
CHKNX1  LDZ     #STAGESEQ1
        LDAB    0,X
        CMPB    #SEQDELIMITER          ; SEQUENCE DELIMITER
        BEQ     SKPCMC                 ; AT END, EXIT
        ANDB    #$0F

; CHECK IF THE PARTICULAR COMPONENT HAS BEEN STARTED
;
        TBA                             ; SAVE B
        LDZ     #MULFILL1STARTED
        DECB
        ABZ
        TAB                             ; RECOVER B
        STZ     TEMPX                   ; STORE TEMPORARILY
        TST     0,Z
        BEQ     SKPCMC                  ; NOT STARTED -> NEXT COMPONENT
        LDZ     #STAGESEQ1
        DECB
        ABZ
        TST     0,Z
        BNE     NOINC1
        LDZ     TEMPX                   ;
        CLR     0,Z                     ; RESET MULTIPLE FILL STARTED FLAG
INC1    AIX     #1                      ; TO NEXT COMPONENT
        BRA     CHKNX1
NOINC1  STX     SEQPOINTER
SKPCMC  RTS


CHECKLSENSOR:
        TST     LEVSEN1STAT       ; LEVEL SENSOR STATUS
        BNE     NOCALC
XITDA   RTS
NOCALC  LDAA    #$0F
        STAA    DISCONACTION    ; DISABLE CONTROL ACTION ON THE NEXT CYCLE.
        LDAA    #THREE          ; NO D/A UPDATE FOR 3 CYCLES
        STAA    ESTCTRUSE       ; USE ESTIMATED VALUE FOR THE NEXT CORRECTION.
        BRA     XITDA



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









;*****************************************************************************;
; FUNCTION NAME : SETCPIPTR                                                   ;
; FUNCTION      : SETS CPI PTR DEPENDING ON STAGE OF FILLING                  ;                                  ;
;*****************************************************************************;

SETCPIPTR:
        PSHB
        LDX     #CMP1CPI        ;
        TST     STAGEFILLEN             ; MULTI STAGED FILLING
        BEQ     XITIGC                  ; EXIT IGNORE CHECK
        LDX     #STAGESEQ1
        LDAB    COMPNO
        DECB
        ABX
        TST     0,X
        BEQ     SETNM                    ; NORMAL POINTER SET UP
        LDAB    0,X
        CMPB    #THREE                  ; 1ST STAGE OF FILL
        BEQ     SETNM
        CMPB    #TWO                    ; 2ND STAGE OF FILL
        BNE     CHFRL                   ; CHECK FOR LAST STAGE
        LDX     #CMP1CPIST2        ;
        BRA     XITIGC
CHFRL   CMPB    #ONE                  ; CHECK FOR LAST STAGE
        BNE     SETNM
        LDX     #CMP1CPIST3        ;
        BRA     XITIGC
SETNM   LDX     #CMP1CPI        ;
XITIGC  PULB
        RTS

; SET UP HASRETRIED POINTER

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




;*****************************************************************************;
; FUNCTION NAME : SETCOPYCOMMAND                                              ;
; FUNCTION      : COPY COMMAND INITIATED                                      ;
; INPUTS        : NONE                                                        ;                       ;
;*****************************************************************************;


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



;*****************************************************************************;
; FUNCTION NAME : CHECKCALCHECKSUM                                            ;
; FUNCTION      : CHECK CHECKSUM ON EEPROM DATA, RELOAD IF DIFFERENCE AND     ;
;               ; GENERATE AN ALARM.                                          ;                                          ;
; INPUTS        : NONE                                                        ;                       ;
;*****************************************************************************;


CHECKCALCHECKSUM:
       TST    CALFLG    ; NO CHECK IN CAL MENU
       BNE    LDCEEX
       TST    CEEFLG1
       BNE    LDCEEX
       TST    CEEFLG
       BNE    LDCEEX

       LDX    #CALDEN  ;~CHECK THAT CHECKSUM OK
       STX    ENDADD
       LDX    #CALBLK
       JSR    CSUM16   ;~CALC. 2 BYTE C'SUM
       CPD    CALDSM
       BEQ    LDCEEX   ;~OK; JUMP
       JSR    RELOADCALIBRATIONDATA
       LDAA   MBPROALRM
       ANDA   #CALINVALARMBIT  ;~CALIBRATION DATA ALARM
       BNE    LDCEEX
       LDAA   #CALINVALARM     ;~CALIBRATION DATA ALARM
       JSR    PTALTB   ;~STORE IN ALARM TABLE
       LDAA    #CALINVALARMBIT
       ORAA    MBPROALRM    ; TRANSITION.
       STAA    MBPROALRM    ; RESTORE MB ALARM.

LDCEEX RTS



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
        BEQ     ON1CM                   ; CHECK FOR TOPUP TO NO TOP UP TRANSITION.
        LDX     #SEQTABLE               ; LOAD TABLE START ADDRESS.
        LDAA    1,X                     ;
        CMPA    #SEQDELIMITER           ; IS THERE ONLY 1 COMPONENT.
        BEQ     ON1CM                   ; ONLY 1 COMPONENT.
        TST     TOPUPF
        BNE     XITTUC                  ; ALREADY SET
        LDAA    #1
        STAA    TOPUPF                  ; SET TOP UP FLAG.
        STAA    FILLTARGETENTRYF        ; SET FILL TARGET
        TST     LAYERING                ; IS LAYERING SET ?
        BEQ     NOLAY                   ; NO LAYERING
        CLR     LAYERING                ;
        JSR     CALCULATEEEPROMCHECKSUM
NOLAY   BRA     XITTUC                  ; EXIT NEED FOR TOP UP.

ON1CM   TST     TOPUPF                  ; CHECK TOP UP FLAG FOR NOW.
        BEQ     XITTUC                  ; EXIT
        CLR     TOPUPF                  ; NO TOP UP
        LDAA    #1
        STAA    FILLTARGETENTRYF        ; SET FILL TARGET

XITTUC  RTS




;*****************************************************************************;
;               ;                               .                             ;
; FUNCTION      : SWAPS MASTER BATCH AND PREVIOUS COMPONENT.                  ;
;               ;                               .                             ;
;*****************************************************************************;

SWAPMASTERBATCH:
        TST     TOPUPF                  ; TOP UP ENABLED ?
        BEQ     XITMB                   ; EXIT IF NO TOP UP
        CLRB                            ; RESET
        LDX     #SEQTABLE
CHKCAG  LDAA    0,X                     ; READ COMP NO
        CMPA    #SEQDELIMITER
        BEQ     CMPCNT                  ; COMPONENTS ARE COUNTED
        AIX     #1
        INCB
        BRA     CHKCAG                  ; CHECK COMPONENT AGAIN
CMPCNT  CMPB    #THREE                  ;
        BLO     XITMB                   ; NO MASTERBATCH SWAP
        LDAB    #RAMBANK
        TBZK
        LDZ     #SEQTABLE               ; FIND THE COLOUR COMPONENT
REPCH   LDAA    0,Z
        CMPA    #SEQDELIMITER
        BEQ     XITMB                   ; NO COMPONENT DETECTED
        ANDA    #$0F
        CMPA    TOPUPCOMPNOREF          ; IS THS THE TOP UP COMP
        BEQ     ISATCMP                 ; IS  AT THE COMP
        AIZ     #1
        BRA     REPCH
ISATCMP AIX     #-1
        LDAA    0,Z
        LDAB    0,X
        STAB    0,Z
        STAA    0,X                     ; SWAP THE TWO COMPONENTS
XITMB   RTS


;*****************************************************************************;
; FUNCTION NAME : RESETFILLCOUNTERS                                           ;
; FUNCTION      : THIS IS CALLED ON THE 1ST FILL.                             ;                                          ;                                          ;
; INPUTS        : NONE                                                        ;                       ;
;*****************************************************************************;


RESETFILLCOUNTERS:
        CLR     RETRYCOUNTER
        CLR     RETRYACTIVE
        RTS




*/
