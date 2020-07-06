/////////////////////////////////////////////////
// P.Smith                          1/10/04
// Define variable for cleaning.
//
// P.Smith                          2/2/05
// Added control algorithm variables

//
// P.Smith                          1-04-2005
// added g_nHiLowOptionSwitchedOnOff,g_nHiLowOptionSwitchedOnOffHistory
//
//
// P.Smith                          5/9/2005
// moved g_fDtoAPerKilo,g_fDtoAPerMeterPerMinute to uncleared area
//
// P.Smith                          11/10/05
// added g_fPositiveControlErrorPercentage;
//
// P.Smith                          24/11/05
// changed g_nExtruderTachoReadingAverage & g_nExtruderTachoReading to unsigned ints
//
// P.Smith                          14/6/06
// unsigned int g_nVavleLatencyTime; added
//
// P.Smith                          15/6/06
// added g_arrcEmailBuffer[EMAIL_BUFFER_SIZE] definition
//
// P.Smith                          10/7/06
// added int         g_nBatchesInKghAvgHistory;
//
// P.Smith                          21/9/06
// added g_nTestBuffer[];,fd0, g_bTSMCall,g_bPrintfRunning,g_nNoPanelReply;
//
// P.Smith                          4/12/06
// change g_nCycleIndicate to a BOOL g_bCycleIndicate
// remove g_fDtoAPerKGH
// g_nAlarmOnFlag -> BOOL g_bAlarmOnFlag, g_nAlarmOnTransitionFlag -> BOOL g_bAlarmOnTransitionFlag
// g_nEEPROMWriteTimeoutCounter, g_nEEPROMReadTimeoutCounter
// g_bModbusDToAWritten, g_bModbusLineSpeedWritten & g_bModbusDToAPercentageWritten changed to a BOOL
//
// P.Smith                          2/3/07
// g_nResetAllFlowRates added
//
// P.Smith                          25/4/07
// Added variables required for queueing of Multiblend recipes.
//
// P.Smith                          11/5/07
// added g_bFlowRateChange
//
// P.Smith                          24/5/07
// defined  g_bStopSPIInterruptRunning & g_bPITAllowed;
//
// P.Smith                          30/5/07
// added g_nMultiBlendRecipeSaveCtr;
//
// P.Smith                          11/6/07
// added g_bDHCP
//
// P.Smith                          12/6/07
// added g_bOptimisationPresent
//
// P.Smith                          13/6/07
// added g_bOptimisationFlapStatus,g_bOptimisationLoaderEnable & g_bOptimisationLoaderFill
//
// P.Smith                          20/6/07
// added g_bSaveAllCalibrationToEEpromOutsideCycle
//
// P.Smith                          20/6/07
// remove g_bStopSPIInterruptRunning
//
// P.Smith                          25/6/07
// added g_bCopyConfigFileToSDCard, g_bCopyConfigFileToBackupFile,g_bLoadDefaultConfigFromSDCard;
// g_bLoadBackupConfigFromSDCard
//
// P.Smith                          28/6/07
// added g_bOptimisationUpdate
//
// P.Smith                          4/7/07
// added variables for optimisation
//
// P.Smith                          19/7/07
// added g_bFirstPulsesReadFromSEI
//
// P.Smith                          25/7/07
// added g_bBlenderPurgeingInProgress, g_bInitiatePurge,g_bBlenderPurged
// g_bBlenderPurgeingInProgress
//
// P.Smith                          1/8/07
// Remove g_nCurrentExtruderNo
//
// P.Smith                          27/8/07
// Added g_bMultiBlendRecipeIsZero
//
// M.McKiernan                      17/9/07
// Added g_arrnMBTable[] & g_arrnMBTableFlags[]
//       g_nMBWriteOrFlag, g_nMBWriteOrErrorFlag, g_nMBWriteOrFlagTemp
//
// P.Smith                      24/10/07
// added g_nAllowReadOfControlInputsCtr
//
// P.Smith                      23/11/07
// added g_bModbusScrewSpeedWritten
//
// P.Smith                          30/1/08
// correct compiler warnings by making some ints unsigned
//
// P.Smith                          17/5/08
// added g_bAllowMultiBlendCycle  kwh
//
// P.Smith                          17/5/08
// add event logs for
//
// P.Smith                          10/10/08
// added g_fKgperHour
//
// P.Smith                          11/12/08
// name change g_bTelnetSDCardAccessInProgress
// added g_OrderReportName, g_cEventLogFileCopy
//
// M.McKiernan                07/05/2008
// Added g_bLIWReadDataAvailable[MAX_LIW_CARDS] and g_bLIWReadDataReady[MAX_LIW_CARDS];

//
// M.McKiernan                16/06/2008
// Added g_bCalibrationDataChecksumValid
//
// P.Smith                    15/12/2009
// changed stability variables to longs
//////////////////////////////////////////////////
#ifndef __BATVARS2_H__
#define __BATVARS2_H__

#include "General.h"
#include "BatchMBIndices.h"
#include "PrinterFunctions.h"
#include <effs_fat/fat.h>
#include "BatchCalibrationFunctions.h"
#include "TSMPeripheralsMBIndices.h"
#include "Historylog.h"
#include "Debug.h"
#include "BlenderSelfTest.h"
#include "MBProSum.h"


extern  int         g_nExpansionOutput;                                           //EXPANOUTPUT             DS      1       ; EXPANSION O/P
extern  BOOL        g_bBlenderTopCleanOn;                                         //AIRJET1ONF              DS      1       ; AIR JET ONE ON.
extern  BOOL        g_bBlenderMixerCleanOn;                                       //AIRJET2ONF              DS      1       ; AIR JET TWO ON.
extern  BOOL        g_bRightCornerCleanOn;                                        //CORNERCLEANRONF         DS      1       ; CORNER CLEAN
extern  BOOL        g_bLeftCornerCleanOn;                                         //CORNERCLEANONF          DS      1       ; CORNER CLEAN ON
extern  BOOL        g_bOverRideClean;                                             //OVERRIDECLEAN            DS      1       ; OVER RIDE CLEAN
extern  BOOL        g_bExpanioFitted;                                             // EXTERNALCLEANPCB        DS      1       ; DETERMINES IS EXTERNAL CLEAN PCB SHOULD BE WRITTEN TO.
extern  BOOL        g_bBypassStatus;                                              // BYPASSSTATUS    DS      1       ; BYPASS STATUS.

extern  BOOL        g_nAfterStartUpCtr;                                             // AFTERSTARTUPCTR             DS      1           ; AFTER START UP COUNTER.
extern  BOOL        g_nCorrectEveryNCycleCtr;                                       //  CORRECTEVERYNCYCLECTR      DS      1           ; CYCLE COUNTER
extern  BOOL        g_nControlErrorCounter;                                         // CERCT1                   DS      1        ;CONTROL ERROR COUNTER #1
extern  float       g_fControlErrorPercentage;                                      // Control % error.
extern  float       g_fPositiveControlErrorPercentage;                              //absolute Control % error.
extern  unsigned int g_nTempExtruderDAValue;                                         // temp d/a storage
extern  unsigned int g_nExtruderTachoReadingAverage;                                 // SEIADREADINGAVG DS      2       ; VOLTAGE FOLLOWER READING.
extern  unsigned int g_nExtruderTachoReading;                                        // SEIADREADING    DS      2       ; VOLTAGE FOLLOWER READING.
extern  BOOL        g_bScrewIncreaseDeceasePressed;                                 // SCREWINCDECPR   DS      1       ; SCREW INC/DEC PRESSED FLAG.
extern  float       g_fLineSpeed5sec;                                               // LSPD5S          DS     5        ;~5 SEC LINE SPEED
extern  float       g_fWeightPerMeter;                                              // WTPMVL5         DS      5       ; WT/M
extern  float       g_fWeightPerMeterRR;                                            // RRGMPM          DS      3       ; G/M ROUND ROBIN.
extern  int         g_nEstimationContinueCounter;                                   //CONESTIMATION    DS      1       ; CONTINUE ESTIMATION.
extern  float       g_fExtruderDAValuePercentage;                                   // CH1CV  DS     2        ;~CONTROL VOLTS VALUES (0-999)
extern  unsigned int g_nSEIDToAReadState;                                            // SEIDARDSTATE    DS      2       ; SEI D/A READ DATA.
extern  long        g_lSEIPulses;                                                   // SEIPULSEACC     DS      4       ; SEI PULSE READING READ BACK.
extern  int         g_nSEIAlarmState;                                               // SEIALARMSTATE   DS      2       ; ALARM STATE READ.
extern  int         g_nSEIDToAOutputState;                                          // SEIDAOPSTATE    DS      2       ; SEI DA OP STATE.
extern  int         g_nSEIInputState;                                               // SEIINPUTSTATE   DS      2       ; READ INPUTS STATE
extern  int         g_nSEIPulsesPerSecond;                                          // SEIPULSEPSEC    DS      2       ; SEI PULSES / SECOND
extern  int         g_nVoltageFollowerReading;                                      // VOLFOLREADING   DS      2       ; VOLTAGE FOLLOWER READING
extern  BOOL        g_bSEIPulsesReady;                                              // SEIPULSESREADY  DS      1       ; INDICATES THAT PULSE CALCULATION SHOULD BE DONE.
extern  int         g_nLineSpeedFractionCounter;                                    // LSPDFRACTIONCTR DS      2       ; FRACTION COUNTER
extern  int         g_nSEIIncreaseDecreaseState;                                         // SEIINCDECSTATE  DS      2       ; SEI INC/DEC STATE
extern  BOOL        g_bLengthCalculationInProgress;                                     // LENGTHCALCINP   DS      1       ; LENGTH CACULATION IN PROGRESS
extern  BOOL        g_bSEIFirstPulseRead;                                               // SEIFIRSTPULSER  DS      1       ; SEI FIRST PULSE READING.
extern  long        g_lSEICurrentPulses;                                                // CURRENTPCLK     DS      4       ; CURRENT PCLK VALUE.
extern  long        g_lSEIPreviousPulses;                                                //PREVIOUSPCLK    DS      4       ; PREVIOUS PCLK VALUE.
extern  long        g_lDifferenceInPulses;                                              // DIFFINPCLK      DS      4       ; DIFFERENCE IN PCLK.
extern  long        g_lBatchPulsesAccumulator;                                          // BATCHPULSEACC   DS      3       ; BATCH PULSE ACCUMULATOR
extern  long        g_lFrontRollPulsesAccumulator;                                      //LSPDPULACC5SECF DS      3        ; FRONT ROLL PULSE ACC
extern  long        g_lBackRollPulsesAccumulator;                                       //LSPDPULACC5SECB DS      3        ; BACK ROLL PULSE ACC
extern  long        g_lHourlyPulsesAccumulator;                                         //HOURLYPULACC    DS      3        ; HOURLY PULSE ACC
extern  unsigned int g_nLineSpeedPulse5SecondCounter;                                    //LSPDPULCTR5SEC  DS      1        ;~6840 CTR 3 READ NO. (0-4)
extern  long        g_lLineSpeedPulse5SecondAccumulator;                                //LSPDPULACC5SEC  DS      3        ;~6840 CTR 3 SUM (5 SECS)
extern  long        g_lLineSpeedPulse5SecondTotal;                                      //LSPDPULTOT5SEC  DS      3        ;~CTR 3 PULSES FOR 5 SECS
extern  long        g_bLineSpeedUpdate;                                                 //LS5SFG DS     1        ;~LINE SPEED 5 SECS FLAG, SET WHEN 5 SEC DATA AV
extern  BOOL        g_bSEIReadDataAvailable;                                            // new variable
extern  int         g_nZeroLineSpeedErrorCounter;                                       //LSPEEDERRORCTR              DS      1           ; LINE SPEED ERROR COUNTER.
extern  float       g_fLineSpeed5secHistory;                                            //WMLS5S          DS     5        ;~5 SEC LINE SPEED USED TO CALC. WT/M
extern  BOOL        g_bLineSpeedHasChanged;                                             //LS5CFG DS     1        ;~LINE SPEED 5 SEC CHANGE FLAG
extern  int         g_nLineSpeedChangeCounter;                                          //LSPEEDCHANGECTR DS      1       ; LINE SPEED COUNTER
extern  float       g_fBatchLength;                                                     //BATCHLENGTH     DS      5       ; BATCH LENGTH STORAGE
extern  int         g_nEstimatedAddedPulses;                                            //ESTADDSEIPULSES DS      2       ; ADDITIONAL PULSES IN SECOND.
extern  int         g_nEstimatedSubtractedPulses;                                       //ESTSUBSEIPULSES DS      2       ; SEIPULSEPSEC - ADDITIONAL PULSES.
extern  float       g_fLbsPer1000Feet;                                                  //ACTLBKF         DS    3         ; ACT LB/1000'
extern  float       g_fGPMRRBuffer[MAX_GPM_RR_SAMPLES];                                 //g/m round robin buffer.
extern  unsigned int g_nGPMRRIndex;                                                      // index or pointer for g/m r.r.
extern  unsigned int g_nGPMRRSummationCounter;                                           // summation counter for g/m r.r.
extern  int         g_nIncreaseLampStatus;                                              // INCLAMPSTATUS   DS      1
extern  int         g_nDecreaseLampStatus;                                              // DECLAMPSTATUS   DS      1       ; DEC LAMP STATUS.
extern  int         g_nSEITimeOutCounter;                                               // SEITIMEOUTCTR   DS      1       ; SEI TIMEOUT COUNTER.
extern  BOOL        g_bSEIHasTimedOut;                                                  // SEITIMEOUTFLAG  DS      1       ; SEI TIMEOUT FLAG.
extern  int         g_bPrintSEIResetOccurred;                                           // PRINTTOMANUAL   DS      1
extern  BOOL        g_bAlarmOnFlag;                                                     // ALARMONFLAG     DS      1       ; INDICATE THAT ALARM IS ON.
extern  BOOL         g_bAlarmOnTransitionFlag;                                           // ALARMONTRFLAG   DS      1       ; ALARM TRANSITION HAS OCCURRED.
extern  float       g_fComponentFrontWeightAccumulator[MAX_COMPONENTS];                 // CH1WACCF DS    5         ; weight accumulator,  (front roll)
extern  float       g_fComponentBackWeightAccumulator[MAX_COMPONENTS];                  // CH1WACCB DS    5         ; weight accumulator,  (back roll)
extern  float       g_fComponentOrderWeightAccumulator[MAX_COMPONENTS];                 // CH1WACCO DS    5         ; weight accumulator,  (order)
extern  float       g_fComponentShiftWeightAccumulator[MAX_COMPONENTS];                 // CH1WACCS DS    5         ; weight accumulator, XX XX XX .YY YY KGS
extern  float       g_fComponentShiftWeightOngoingAccumulator[MAX_COMPONENTS];          // CH1WACCS9 DS    5         ; weight accumulator, XX XX XX .YY YY KGS
extern  float       g_fComponentHourlyWeightAccumulator[MAX_COMPONENTS];                // CH1WACCH DS    5         ; weight accumulator, XX XX XX .YY YY KGS
extern  float       g_fComponentLongTermWeightAccumulator[MAX_COMPONENTS];              // CH2WACCM DS    5         ; Monthly
extern  float       g_fComponentLongTermWeightResettableAccumulator[MAX_COMPONENTS];              // CH2WACCM DS    5         ; Monthly

extern  float       g_fComponentBatchWeightAccumulator[MAX_COMPONENTS];                 // CH1WACCBAT  DS    5             ; weight accumulator, XX XX XX .YY YY KGS
extern  float       g_fComponentBatchWeightAccumulatorPercentage[MAX_COMPONENTS];       //
extern  float       g_fTotalOrderWeightAccumulator;                                     // TOTWACCO DS    5         ; TOTAL wt. acc.
extern  float       g_fTotalFrontWeightAccumulator;                                     // TOTWACCF DS    5         ; TOTAL wt. acc.
extern  float       g_fTotalBackWeightAccumulator;                                      // TOTWACCB DS    5         ; TOTAL wt. acc.
extern  float       g_fTotalShiftWeightAccumulator;                                     // CURSHIFTWEIGHT  DS      5       ; CURRENT SHIFT WEIGHT.
extern  float       g_fTotalShiftWeightOngoingAccumulator;                              // TOTWACCS9 DS    5        ; TOTAL WEIGHT ACCUMULATE FOR SHIFT.
extern  float       g_fTotalLongTermWeightAccumulator;                                  // TOTWACCM DS    5         ; TOTAL wt. acc. XX XX XX .YY YY KG
extern  float       g_fTotalLongTermWeightResettableAccumulator;

extern  float       g_fTotalHourlyWeightAccumulator;                                    // TOTWACCH DS    5         ; TOTAL wt. acc. XX XX XX .YY YY KG
extern  float       g_fTotalBatchWeightAccumulator;                                     // TOTWACCBAT  DS    5             ; TOTAL WEIGHT ACCUMULATE FOR BATCH IN OFFLINE MODE.
extern  float       g_fTotalHistoryBatchWeightAccumulatorImp;                           // TOTWACCBATHI    DS      5        ; TOTAL WEIGHT ACCUMULATE FOR OFFLINE MODE FOR IMPERIAL MODE.
extern  float       g_fTotalHistoryBatchWeightAccumulator;                              // TOTWACCBATH     DS      5        ; TOTAL WEIGHT ACCUMULATE FOR OFFLINE MODE.
extern  float       g_fTotalBatchWeightAccumulatorImp;                                  // TOTWACCBATI DS    5             ; TOTAL WEIGHT ACCUMULATE FOR BATCH IN OFFLINE MODE.
extern  int         g_nStopInNBatches;                                                  //STOPINNBATCHES  DS      1
extern  float       g_fTotalBatchWeightAccumationLeft;                                  //TOTWACCBATLEFT DS       5       ;
extern  float       g_fBatchSizeStorage;                                                //DFTW1STORE      DS      2
extern  BOOL        g_bNoTargetAlarm;                                                   //NOTARGETALARM   DS      1       ; NO TARGET ALARM FLAG
extern  BOOL        g_bBatchIsReady;                                                    //BATCHREADY      DS      1       ; BATCH READY
extern  BOOL        g_bMixerSensorHistory;                                              //MIXSENSORHISTORY DS     1
extern  int         g_nAToDSumCounter;                                                  //SAMPLEAD1       DS      1       ; SAMPLE COUNTER
extern  long        g_lAToDSum;                                                         //SUMAD1          DS      3       ; SUMMATION
extern  float       g_fLineSpeed5secFtPMinute;                                          //LSPD5SFM        DS    3         ; LINE SPEED
extern  float       g_fTotalThroughputSetLbsPerHour;                                    //SETTLBH DS     3         ;~TOTAL THRU-PUT, SUM OF X BLENDS
extern  float       g_fTotalThroughputSetLbsPer1000Feet;                                //SETLBKF  DS    4        ; SET LB/1000'
extern  float       g_fTotalfDesiredWidthinInches;                                      //WIDTHSPIN DS   3         ; WIDTH SP IN INCHES (2DP)
extern  int         g_nHiLowOptionSwitchedOnOff;                                        //HILOWOPTIONSWITCHEDONOFF    DS       1       ; HI LOW REG OPTION SWITCHED OFF.
extern  int         g_nHiLowOptionSwitchedOnOffHistory;                                 //HILOWOPTIONSWITCHEDONOFFHIS DS       1       ; HI LOW REG OPTION SWITCHED OFF.
extern  BOOL        g_bPauseOffActive;                                                  //PAUSEOFFACTIVE  DS      1       ; PAUSE OFF INPUT RAM DEFINED
extern  int         g_nPauseOffActiveCounter;                                           //POACTIVECTR     DS      1
extern  BOOL        g_bActivateFrontRollChange;                                         //RCFFLG  DS    1        ;~FRONT REEL CHG FLAG
extern  BOOL        g_bActivateBackRollChange;                                          //RCBFLG  DS    1        ;~BACK REEL CHG FLAG
extern  BOOL        g_bReadDToFromSEIOnReset;                                           //read d/a on reset
extern  float       g_fUsedBatchWeight;                                                 //BATCHUSEDWT     DS      3       ; LAST ROLL COMPSENSATED WEIGHT
extern  float       g_fUnUsedBatchWeight;                                               //BATCHUNUSEDWT   DS      3       ; LAST ROLL COMPSENSATED WEIGHT
extern  float       g_fComponentActualUnusedWeight[MAX_COMPONENTS];                     // CMP1ACTWCM      DS      3       ; COMPONENT #N ACTUAL WEIGHT
//--removed--extern  float       g_fComponentLongTermWeightAccumulator[MAX_COMPONENTS];              // CH1WACCM DS    5         ; weight accumulator, XX XX XX .YY YY KGS
extern  float       g_fComponentOngoingOrderPercentage[MAX_COMPONENTS];                 // ORBPCNT1 DS      3       ; ORDER %
extern  float       g_fComponentFrontRollPercentage[MAX_COMPONENTS];                    // CH1PERCFR DS    3         ; CALCULATED % FOR COMPONENTS.
extern  float       g_fComponentBackRollPercentage[MAX_COMPONENTS];                     // CH1PERCBR DS    3         ; CALCULATED % FOR COMPONENTS.
extern  float       g_fComponentOrderPercentage[MAX_COMPONENTS];                        // CH1PERCO        DS    3         ; CALCULATED % FOR COMPONENTS.
extern  float       g_fComponentShiftPercentage[MAX_COMPONENTS];                        // CH1PERCS DS    3         ;
extern  float       g_fComponentHourlyPercentage[MAX_COMPONENTS];                       // CH1PERCH DS    3         ;
extern  float       g_fComponentLongTermPercentage[MAX_COMPONENTS];                     // CH1PERCM DS    3         ; CALCULATED % FOR COMPONENTS.
extern  long        g_lFrontTotalLengthPulses;                                          // FLSPREG DS    5        ;~LINE SPEED (FRONT) TOTAL LENGTH (PULSES)
extern  long        g_lBackTotalLengthPulses;                                           // BLSPREG DS    5        ;~LINE SPEED (BACK) TOTAL LENGTH (PULSES)
extern  BOOL        g_bResetOrderTotalsAtNextRollChange;                                // RSTOFLAG DS    1       ; RESET ORDER FLAG
extern  float       ftestonlyBatchPulsesAccumulator;                                    //testonly
extern  long        g_lCurrentPulses;
extern  long        g_lPreviousPulses;
extern  long        g_nPulsesDifference;
extern  float       g_fTotalHourlyLengthAccumulator;                                    // HRLTACC  DS    5         ; HOURLY LENGTH ACCUMULATOR.
extern  int         g_nHourlyReportCounter;                                             // PRHRCTR DS      1       ; HOUR COUNTER
extern  BOOL        g_bHourDetectedBefore;                                              // HRBEFORE DS     1       ;INDICATES THAT HOUR HAS BEING DETECTED.
extern  BOOL        g_bActivateHourlyReport;                                            // ACTHRREP        DS      1       ; ACTIVATE HOURLY REPORT.
extern  int         g_nWarmUpCounter;                                                   // WARMUPCTR               DS      1       ; WARMUP COUNTER
extern  BOOL        g_bRecipeWaiting;                                                   // RECIPEWAITING           DS      1       ; DEFINE RECIPE WAITING FLAG.
extern  BOOL        g_bBatchHasFilled;                                                   // BATCHHASFILLED          DS      1       ; INDICATES THAT BATCH HAS BEEN FILLED DURING THE CLEANING CYCLE.
extern  BOOL        g_bBlenderInhibit;                                                   // BLENDERINHIBIT          DS      1       ; BLENDER INHIBIT
extern  int         g_nVacuumSequence;                                                   // VACSEQ                  DS      1       ; INDICATES VACUUM SEQUENCING IN SINGLE RECIPE MODE.
extern  unsigned int g_nVacuumTimer;                                                      // VACCTR                  DS      2       ; VACUUM SEQUENCE TIMER
extern  int         g_nVacuumWriteData;                                                  //VACUUMWR                DS      2       ; WRITE WORD SENT TO VAC8 I/O
extern  BOOL        g_bOfflineValveIsOpen;                                               //OFFLINEVALVEISOPEN      DS      1       ; ALLOW MIXER CLEAN TO PROCEED AFTER THE OFFLINE VALVE IS OPENED IN SINGLE RECIPE MODE
extern  BOOL        g_bCleanOnNextCycle;                                                 //CLEANONNEXTCYCLE        DS      1       ; INDICATES THAT CLEAN SHOULD BE INITIATED.
extern  BOOL        g_bWaitForVacuumToFinish;                                            //WAITFORVACUUMTOFINISH   DS      1       ; INDICATES THAT WE ARE WAITING FOR VACUUM TO FINISH.
extern  BOOL        g_bVacuumIsOn;                                                       //VACUUMISON              DS      1       ; VACUUM IS ON.
extern  int         g_nVacuumLoaderNo;                                                   //VACUUMLOADER            DS      1       ; VACUUM LOADER FOR DISPLAY PURPOSES
extern  BOOL        g_bCheckIfVacuumIsRequired;                                          //CHECKIFVACUUMREQUIREDF  DS      1       ; CHECK IF VACUUM REQUIRED
extern  BOOL        g_bVacuumMaterialAway;                                              //VACUUMMATERIALAWAY      DS      1       ; CHECK FOR MATERIAL TO BE VACUUMED AWAY.
extern  BOOL        g_bVacuumInProgress;                                                //VACUUMINGINPROGRESS     DS      1       ; DEFINE VACUUMING IN PROGRESS
extern  int         g_nBlenderReadyCounter;                                              //BLENDERREADYCTR         DS      1       ; INDICATE THAT BLENDER IS READY.
extern  int         g_nStartUpPagePauseTimer;                                            //PAUSETIMER      DS      1       ; PAUSED TIMER
extern  int         g_nNoLevelSensorAlarmCounter;                                        //NOLSALARMCTR            DS      1       ; NO LEVEL SENSOR ALARM COUNTER
extern  BOOL        g_bOfflineBatchFilled;                                               //OFFLINEBATCHFILLED      DS      1       ; INDICATE THAT OFFLINE BATCH IS READY
extern  BOOL        g_bAlarmFlash;                                                       //ALARMFLASH              DS      1       ; INDICATES IF ALARM SHOULD BE FLASHING
extern  BOOL        g_bFlashSecondCounter;                                               //FLASHSECCTR             DS      1       ; FLASH SECONDS COUNTER
extern  BOOL        g_bAlarmFlashStatus;                                                 //ALARMFLASHSTATUS        DS      1       ; INDICATES WHAT THE STATUS OF THE OUTPUT SHOULD BE
extern  int         g_nLowLevelSensorInputState;                                         //LLSINPUTSTATE   DS      2       ; READ LLS INPUT STATE.
extern  BOOL        g_bInCalibrationMenu;                                                //CALIFG DS     1        ;~CALIBRATION FLAG, SET TO AA WHEN IN CAL MENU O
extern  BOOL        g_bSaveAllCalibrationToOnBoardEEprom;                                //SAVECAL2DATA1F  DS      1       ; SAVE CALIBRATION DATA FLAG.
                                                                                        //SAVECAL2DATA2F  DS      1       ; SAVE CALIBRATION DATA FLAG.
extern  BOOL        g_bSignalXXKgsTargetAlarm;                                          //SIGNALATTARGETALARMFLAG DS      1       ; SIGNAL ALARM ON FEEED OPEN
extern  int         g_nLowLevelSensorAlarmWord;                                         //LLSALARMWORD    DS      2       ; ALARM WORD WRITTEN TO LLS MODULE.
extern  BOOL        g_bLowLevelSensorOff[MAX_COMPONENTS];                                // LS1OFF    DS    1       ; LEVEL SENSOR OFF FLAG.
extern  BOOL        g_bLowLevelSensorOnTransition[MAX_COMPONENTS];                       // LS1ONTR   DS    1       ; LEVEL SENSOR ON TRANSITION.
extern  BOOL        g_bLowLevelSensorStatusHistory[MAX_COMPONENTS];                      // LS1ALHI   DS    1       ; INHIBIT REPETITION OS LLS ALARM.

extern  int         g_nExpansionInputData;                                               //EXPANIOINPUTDATA        DS      1       ; EXPANIO PCB STORAGE
extern  BOOL        g_bNoLowLevelSensorCommunications;                                   // NOLLSCOMMUNICATIONS     DS      1       ; NO LLS COMMUNICATIONS FLAG
extern  int         g_nNoLowLevelSensorCommunicationsCounter;                            //NOLLSCOMMUNICATIONSCTR  DS      1       ; NO LLS COMMS COUNTER
extern  BOOL        g_bShowExpiryPage;                                                  // SHOWEXPIRYPAGE          DS      1       ; SHOW EXPIRY PAGE.
extern  BOOL        g_bIsMasterModule;                                                  // ISMASTERMODULE          DS      1       ; INDICATES THAT MASTER CODE IS IN THE CALIBRATION MODULE
extern  BOOL        g_bPrintHourlyReport;                                               // TOPRNFLG  DS    1       ; "TO PRINT FLAG"
extern  BOOL        g_bPrintOrderReport;                                                // TOPRNFLG  DS    1       ; "TO PRINT FLAG"
extern  BOOL        g_bPrintFrontRollReport;                                            // TOPRNFLG  DS    1       ; "TO PRINT FLAG"
extern  BOOL        g_bPrintBackRollReport;                                             // TOPRNFLG  DS    1       ; "TO PRINT FLAG"
extern  BOOL        g_bPrintAlarmReport;                                                // TOPRNFLG  DS    1       ; "TO PRINT FLAG"

extern  unsigned int g_unCalibrationCheckSum;                                            // CALDSM DS     2                 ;~CHECKSUM FOR CAL. DATA
extern  int         g_nAllowLargeAlarmBand;                                             // ALLOWLARGEALARMBAND     DS      1       ; ALLOW LARGE ALARM BAND
extern  BOOL        g_bDoCMRDToAPerKgCalculation;                                       // CMRNOW          DS      1       ; DO CMR CALCULATION.
extern  BOOL        g_bPotControl;                                                      // CH1POTBIT DS  1        ; POT. CONTROL BITS (6 CHANNELS)
extern  unsigned int g_nControlAvgFactorGpmHistory;                                      //
extern  unsigned int g_nControlAvgFactorKghHistory;                                      //
extern  BOOL        g_bControlSetpointChanged;                                          //
extern  int         g_nDiagnosticArrayIndex;                                            //
extern  int         g_nSingleCycleCompNoHis;                                            // check for component no change in single recipe cycle.
extern  int         g_nReMixOnTime;                                                     // MIXONTIME       DS      1       ;
extern  int         g_bBinLevelSensorHistory;                                           // HIGHLEVELSENSORHISTORY  DS      1       ; HISTORY STORAGE OF HIGH LEVEL SENSOR ALLOWS TRANSITION DETECTION.
extern  BOOL        g_bWriteIncreaseDecreaseDataToSEI;                                  // WRITEINCDECF    DS      1       ; WRITE INCREASE DATA TO SEI.
extern  unsigned int         g_nPeripheralCardsPresentHistory;                                   // peripheral card history.
extern  BOOL        g_bIsSystemAlarm;                                                   //
extern  unsigned int g_nStageFillEnableTransitionHistory;                                              //

extern  float       g_fWeightConversionFactor;                                          //weight conversion factor
extern  float       g_fLengthConversionFactor;                                          //length conversion factor
extern  float       g_fThroughputConversionFactor;                                      //throughput conversion factor
extern  float       g_fWeightPerLengthConversionFactor;                                //weight per length conversion factor
extern  float       g_fLineSpeedConversionFactor;                                      //line speed conversion factor
extern  float       g_fWidthConversionFactor;                                         //width conversion factor
extern  float       g_fWeightPerAreaConversionFactor;                                  //weight per area conversion factor
extern  BOOL        g_bProgramLoopRunnning;                                           //
extern  unsigned int g_nVavleLatencyTime;                                             //VALVELATENCYTIMECALC    DS      2
extern  char        g_arrcEmailBuffer[2500];
extern unsigned int g_nVavleCalculatedLatencyTime;                                   //
extern  float       g_fComponentActualWeightTopUpBackup;                              //
extern  float       g_fComponentActualWeightITopUpBackup;                              //
extern  unsigned int g_nBatchesInKghAvgHistory;
extern  BOOL        g_bLayeringHistory;
extern  BOOL    g_bSPIAtDActive;
extern  BOOL    g_bSPIEEActive;
extern  unsigned int g_nLevelSensorAlarmCounter;                                        // LEVELSENSORALARMCTR     DS      1           ; LEVEL SENSOR ALARM COUNTER
extern  BOOL         g_bHighLoRegrindOptionHistory;
extern  unsigned char   g_CurrentTime_Date_History;
extern  BOOL    g_bNBBRevB1Hardware;                                                // rev B1 hardware
extern  unsigned int    fd0;
extern  BOOL     g_bEnableTxRxInOpenSerial;
extern  BOOL    g_bPrintfRunning;
extern  int     g_nNoPanelReply;
extern  int     g_nEEPROMWriteTimeoutCounter;                                       // eeprom read timeout counter
extern  int     g_nEEPROMReadTimeoutCounter;                                       // eeprom write timeout counter
extern  BOOL    g_bModbusLineSpeedWritten;                                       // MBPRDIAG    DS     2       ; PRODUCTION DIAGNOSTIC OPEN CLOSE               283
extern  float   g_fCommunicationsLineSpeed;                                     //COMMSLSPD DS    3       ; LINE SPEED DOWNLOADED VIA COMMS.
extern  float   g_fCommsLineSpeedFrontLength;                                    // front length accumulator
extern  float   g_fCommsLineSpeedBackLength;                                    // back length accumulator
extern  BOOL    g_bModbusDToAWritten;                                       // MBDTOAFLAG              DS      1
extern  BOOL    g_bModbusDToAPercentageWritten;                             // MBDTOAPERFLAG           DS      1
extern  unsigned int     g_nSPIEEActiveTimeoutCounter;                               // timout counter
extern  unsigned int     g_nRevertToOperatorPanelCounter;                            // timeout counter
extern  BOOL    g_bRestartPanelNow;
extern  BOOL    g_bRedirectStdioToPort2;
extern  unsigned int g_nPulseOnTime;
extern  BOOL    g_bPulsed;
extern  int g_cBatSeq_history;
extern unsigned int g_nResetAllFlowRates;                                  //CLEARALLFLOWRATES       DS      1

extern volatile WORD  g_nTimer0Count;        // count for no. of interrupts.
extern  volatile DWORD  g_lTimer0CounterRegister; // r counter.
extern  volatile DWORD  g_lTimer0CaptureRegister; //  capture register.
extern WORD   g_nUSTimeSumEntries;
extern DWORD  g_lUSTimeSum;
extern DWORD  g_lUSTimeAvg;
extern float  g_fUSDistance;
extern DWORD       g_lTriggerTime;
extern BOOL  g_bT0TriggerMode;
extern BYTE  g_nT0InterruptMode;
extern  BOOL    g_bReadOnNext;
extern float   g_fUltrasonicmmpkg;
extern float g_fWeightInChamber;
extern float g_fUltrasonicTareDistance;


// ram not to be cleared. --review--
//

extern  int         g_nExtruderNo;                                                      //EXTRUDERNO  DS      2       ; RECIPE NO
extern  int         g_ntestonly;

extern  BOOL        bUSDataAvailable;
extern  unsigned int g_nCycleSecondNo;
extern float g_fCycleUSDistance[USMAXENTRIES];
extern WORD  g_nTimeInCycle[USMAXENTRIES];
extern WORD  g_nUSBufferIndex;
extern BOOL  g_bUSDataAvailable;

extern float g_fslope1;
extern float g_fslope2;
extern float g_fslope3;
extern BOOL  g_bUltrasonicLIWInstalled;
extern BOOL  g_bPrintWeightNow;
extern BOOL  g_bUpdateSDCardSoftware;
extern BOOL  g_bSoftwareUpdateRunning;
extern BOOL g_bSDCardSPIActive;

extern unsigned int    g_unCurrentFlapStatus1;
extern unsigned int    g_unInProgressStatus;
extern BOOL            g_baCurrentFlapStatus[MULTIBLEND_EXTRUDER_NO];
extern BOOL            g_baInProgress[MULTIBLEND_EXTRUDER_NO];
extern BOOL            g_bRan;
extern BOOL            g_bExistsInQueue;
extern BOOL            g_bClearHistory;
extern BOOL            g_bMultiBlendisPaused;
extern BOOL            g_bMultiBlendPaused;
extern int             g_nRanx;
extern unsigned int    g_unaCurrentQueue[MULTIBLEND_EXTRUDER_NO];
extern int             g_nEndOfQueue;
extern unsigned int    g_unaDownLoadHistory[DOWNLOADHISTORYNO];
extern int             g_nNoCurrentFlaps;
extern unsigned int    g_unCurrentExtrNo;
extern int             g_nQueueOneSecondTimer;
extern int             g_BlenderMapping[SOURCEOUTPUTNO][DEFINEDOUTPUTNO];
extern BOOL            g_bFlowRateChange[MAX_COMPONENTS];
extern BOOL            g_bPITAllowed;
extern int             g_nMultiBlendRecipeSaveCtr;
extern  BOOL           g_bDHCP;
extern  BOOL           g_bOptimisationPresent;
extern  BOOL           g_nDeviceListIndexOptimisation;
extern  BOOL           g_bOptimisationFlapStatus[MAX_COMPONENTS];
extern  BOOL           g_bOptimisationLoaderEnable[MAX_COMPONENTS];
extern  BOOL           g_bOptimisationLoaderFill[MAX_COMPONENTS];
extern  BOOL           g_bOptimisationUpdate[MAX_COMPONENTS];

extern  unsigned int   g_nuOptimisationSensorInputState;
extern  unsigned int   g_unOptimisationLoaderOuputs;
extern  BOOL           g_bSaveAllCalibrationToEEpromOutsideCycle;
extern  BOOL           g_bSPIBusBusy;
extern  int            g_nVibrateCatchBoxCtr;
extern  BOOL           g_bCopyConfigFileToSDCard;
extern  BOOL           g_bCopyConfigFileToBackupFile;
extern  BOOL           g_bLoadConfigFromSDCard;
extern  BOOL           g_bLoadBackupConfigFromSDCard;

extern float           g_fComponentBinAndLoaderWeight[MAX_COMPONENTS];
extern float           g_fComponentBinWeight[MAX_COMPONENTS];
extern float           g_fComponentLoaderWeight[MAX_COMPONENTS];
extern float           g_fRemainingOrderLength;
extern float           g_fComponentRemainingOrderWeight[MAX_COMPONENTS];
extern float           g_fOptimisationOrderLength;
extern float           g_fRemainingOrderWeight;
extern BOOL            g_bOptimisationFlapHistoryStatus[MAX_COMPONENTS];
extern int             g_nOptimisationFlapDebounceCounter[MAX_COMPONENTS];
extern float           g_fComponentLoaderWeightAccumulator[MAX_COMPONENTS];                 // CH1WACCF DS    5         ; weight accumulator,  (front roll)
extern int             g_nLoaderEmptyingCounter[MAX_COMPONENTS];
extern BOOL            g_bModbusLengthWritten;
extern float           g_fWeightPerMeterFromLSpeed;
extern BOOL            g_bFirstPulsesReadFromSEI;
extern BOOL            g_bInitiatePurge;
extern BOOL            g_bBlenderPurged;
extern BOOL            g_bBlenderPurgeingInProgress;
extern BOOL            g_bMultiBlendRecipeIsZero[MULTIBLEND_EXTRUDER_NO];

extern WORD            g_arrnMBTable[MB_TABLE_SIZE];
extern WORD            g_arrnMBTableFlags[MB_TABLE_WRITE_SIZE];
extern WORD            g_nMBWriteOrFlag;
extern WORD            g_nMBWriteOrErrorFlag;
extern WORD            g_nMBWriteOrFlagTemp;
//extern char            g_cMultiblendDiagnosticsBuffer[PRINT_BUFFER_SIZE];
extern BOOL            g_bWriteToIniFileAgain;
extern int             g_nOverrideCounter;
extern int             g_nAllowReadOfControlInputsCtr;
extern BOOL            g_bStartingMultiBlendRecipe;
extern int             g_nOpenserialStatus;
extern int             g_nOpenserialStatusCounter;
extern int             g_nCloseserialStatus;
extern int             g_nUartDataOpenSerial;
extern int             g_nUartDataCloseSerial;
extern BOOL            g_bUsingSmallBatchSize;
extern BOOL            g_bFirstTimeToReadScrewSpeed;  // FIRSTTIMETOREADSSAD     DS      1 ; FIRST READ OF FLAG
extern float           g_fExtruderRPMHistory;
extern BOOL            g_bCalculateScrewSpeedConstant; // CALCSCREWCON    DS      1       ; "CALCULATE SCREW CONSTANT NOW"
extern float           g_fScrewSpeedCalibrate;
extern  int            g_nVolumetricEstimateCounter;
extern BOOL            g_bCheckOptimisationAgain[MAX_COMPONENTS];
extern BOOL            g_bFirstOptimisationCheck;
extern BOOL            g_bModbusScrewSpeedWritten;
extern int             g_nWriteHoldOffCounter;
extern int             g_nSuspendControlCounter;           // SUSPENDCONTROLCOUNTER DS    1    ; SUSPEND CONTROL COUNTER
extern int             g_nHoldOffLevelSensorAlarmSecondCounter; //HOLDOFFLSENSORALCTR     DS      1   ; HOLD OFF LEVEL SENSOR ALARM COUNTER
extern unsigned int    g_nComponentPerDeviationErrorCounter[MAX_COMPONENTS]; // COMP1PERDEVERRORCOUNTER DS  1  1-12

extern float           g_fComponentActualWeightBufferRR[MAX_COMPONENTS][MAX_KGH_RR_SAMPLES];     //kg/h round robin buffer.
extern unsigned int    g_nComponentActualWeightRRIndex[MAX_COMPONENTS];                       //
extern unsigned int    g_nComponentActualWeightRRSummationCounter[MAX_COMPONENTS];           //
extern float           g_fComponentActualPercentageRR[MAX_COMPONENTS]; // BPCNT1-BPCNT1     ; COMP #1-12 ACTUAL %'s
extern int             g_IncreaseOutputOn10hzCounter; // INCREASEOUTPUTONCTR     DS      1   ;
extern int             g_DecreaseOutputOn10hzCounter; // DECREASEOUTPUTONCTR     DS      1   ;
extern BOOL            g_bResetComponentWeightsRoundRobin;
extern int             g_nAutocycleLevelSensorOff10hzCounter; // AUTOCYCLELEVELSENSOROFF DS  1    ; AUTOCYCLE FOR SENSOR
extern BOOL            g_bSHAHardwareCheckFailed;
extern BOOL            g_bSaveUSBFileToUSB;
//extern BYTE            g_byPrinterBuffer[USB_PRINT_BUFFER_SIZE];
extern BOOL            g_bUSBDataWrite;
extern  BOOL           g_bComponentHasFilledByWeight[MAX_COMPONENTS];

extern float           g_fComponentWeightSamples[MAX_COMPONENTS][MAX_CYCLE_DIAGNOSTICS_SAMPLES];
extern int             g_nComponentWeightSampleCounter[MAX_COMPONENTS];
extern float           g_fComponentWeightMean[MAX_COMPONENTS];
extern float           g_fComponentWeightStdDev[MAX_COMPONENTS];
extern float           g_fComponentWeightStdDevPercentage[MAX_COMPONENTS];
extern float           g_fComponentWeightMaximum[MAX_COMPONENTS];
extern float           g_fComponentWeightMinimum[MAX_COMPONENTS];
extern WORD            g_arrnMBTableSetpointCopy[MB_TABLE_SETPOINT_SIZE];
extern unsigned int    g_nWritenwrdCounter;
extern BOOL            g_bPanelWriteHasHappened;
extern BOOL            g_bSEIReadDataReady;
extern unsigned int    g_nExtruderVolumetricDAValue;
extern BOOL            g_bUSBCopyInProgress;
extern unsigned long   g_lUSBFileDataCopied;

extern F_FIND g_nSdCardFileDetails[MAX_SD_CARD_FILES];
extern unsigned int    g_nNoOfFilesOnSdCard;
extern BOOL            g_bReadSDCardDirectory;
extern unsigned int    g_nCopyFileNoToUSB;
extern unsigned int    g_nCopyFileNoToUSBRequest;

extern long            g_lSPIBaud;
extern  F_FILE * fpSdCard;
extern unsigned int    g_nUSBCopySequencer;
extern unsigned int    g_nExtruderControlZeroToTenSetpoint;
extern unsigned int    g_nExtruderControlSetpointIncDec;
extern unsigned int    g_nSEIWriteRequiredCounter;

extern unsigned int    g_nControlLagHistory;            // CONTROLLAG - seconds?
extern unsigned int    g_nIncreaseRateHistory;                   // INCREASERATE - Increase rate - d/a counts per second
extern unsigned int    g_nDecreaseRateHistory;                   // DECREASERATE - Decrease rate - d/a counts per second
extern unsigned int    g_nControlDeadbandHistory;                // DEADBAND - deadband for inc/dec control.
extern unsigned int    g_nFineIncDecHistory;                     // FINEINCDEC - Fine increase/decrease

extern BOOL        g_bLIWReadDataAvailable[MAX_LIW_CARDS];                                            // new variable
extern BOOL        g_bLIWReadDataReady[MAX_LIW_CARDS];                                            // new variable
extern BOOL        g_bCalibrationDataChecksumValid;

extern BOOL    g_bSEIIncDecSetpointDownload;
extern BOOL    g_bSEIAutoStatus;                                 // SEI auto status
extern BOOL    g_bAllowProcesDataSaveToEEPROM;
extern unsigned int    g_nSaveProcessDataToEEPROMCounter;   // no of cycles to save to eeprom
extern RepeatedDataStruct g_ProcessData;
extern BOOL    g_bSaveAllProcessDataToEEprom;
extern BOOL    g_bSaveAllProcessDataOutsideCycle;
extern unsigned int    g_nCycleCounter;
extern unsigned int    g_nCurrentExtruderNo;
extern WORD            g_arrnMBSetpointHistory[MB_TABLE_SETPOINT_SIZE];
extern BOOL            g_bAllowMultiBlendCycle;  //kwh

extern unsigned int    g_nSDCardSoftwareUpdateComplete;        // shows software update is complete
extern unsigned int    g_nSDCardLoadConfigComplete;            // shows load of config is complete
extern unsigned int    g_nSDCardLoadBackupConfigComplete;      // shows load of backup config is complete
extern unsigned int    g_nSDCardSaveConfigComplete;            // shows save of config is complete
extern unsigned int    g_nSDCardCopyConfigToBackupComplete;    // shows copy of config to is complete
extern unsigned int    g_nDownloadManagerNZTimeoutCtr;


extern BOOL     g_bSDCardSoftwareUpdateComplete;        // shows software update is complete
extern BOOL     g_bSDCardLoadConfigComplete;            // shows load of config is complete
extern BOOL     g_bSDCardLoadBackupConfigComplete;      // shows load of backup config is complete
extern BOOL     g_bSDCardSaveConfigComplete;            // shows save of config is complete
extern BOOL     g_bSDCardCopyConfigToBackupComplete;    // shows copy of config to is complete
extern unsigned int    g_nDisableDTAPerKgCtr;           //DISABLEDAPKGHCTR        DS      1    ;ESTIKO CHANGE

extern BOOL            g_bCalibrationDataInvalid;    // calibration data invalid
extern BOOL            g_bProcessDataInvalid;    // process data invalid
extern BOOL            g_bTelnetEnable;
extern BOOL            g_bTelnetHistory;

extern unsigned int    g_nBatchCommsStatusSEI;         // sei comms status
extern unsigned int    g_nBatchCommsStatusLLS;         // LLS comms status
extern unsigned int    g_nBatchCommsStatusOptimisation;// Optimisation comms status
extern unsigned int    g_nBatchCommsStatusNetwork;     // network comms status
extern unsigned int    g_nBatchCommsStatusTCPIP;       // TCPIP comms status
extern unsigned int    g_nBatchCommsStatusPanel;       // panel comms status

extern long     g_nBatchCommsSEITimeoutHistory;         // comms timeout history
extern long     g_nBatchCommsLLSTimeoutHistory;         // comms timeout history
extern long     g_nBatchCommsOptimisationTimeoutHistory;// comms timeout history
extern long     g_nBatchCommsNetworkGoodTxHistory;      // comms good tx history Network
extern long     g_nCommsVacuumLoadingTimeoutHistory;            //
extern long     g_nBatchCommsStatusVacuumLoading;            //

extern long     g_nBatchCommsPanelTimeoutRxHistory;      //
extern long     g_nBatchCommsPanelTimeoutRx;            //

extern unsigned int    g_nBatchCommsTCPIPGoodTxHistory;        // comms good tx history TCPIP
extern unsigned int    g_nTCPIPReadHoldRegsCtr;              // indicates no of tcpip hold reg commands complete.

extern BOOL            g_bToAutoEvent;
extern BOOL            g_bToManualEvent;
extern BOOL            g_bWeighHopperCalibratedEvent;
extern BOOL            g_bWeighHopperTaredEvent;
extern BOOL            g_bCleaningInitiatedEvent;

extern char            g_cCurrentMonthHistory;
extern unsigned int    g_nWriteConfigToSDInNSeconds;
extern unsigned int    g_nOperatorPanelPageId;
extern unsigned int    g_nBlenderType;
extern WORD            g_nSDCardDelayTime;

extern WORD    g_CurrentTime[TIME_ARRAY_SIZE];
extern WORD    g_PowerUpTime[TIME_ARRAY_SIZE];
extern WORD    g_cPowerDownTime[TIME_ARRAY_SIZE];
extern int g_arrnWriteSEIMBTable[SEI_TABLE_WRITE_SIZE];
extern int g_arrnReadSEIMBTable[SEI_TABLE_READ_SIZE];
extern int g_arrnWriteLLSMBTable[LLS_TABLE_WRITE_SIZE];
extern int g_arrnReadLLSMBTable[LLS_TABLE_READ_SIZE];
extern int g_arrnWriteOptimisationMBTable[LLS_TABLE_WRITE_SIZE];
extern int g_arrnReadOptimisationMBTable[LLS_TABLE_READ_SIZE];
extern unsigned int    g_nDisableSDCardAccessCtr;
extern unsigned int    g_nAMToggleStatusDelayCtr;
extern BOOL            g_bWriteEventLogToSdCard;
extern char            g_cEventLogFile[EVENTLOGSTRINGLENGTH];
extern char            g_cEventName[20];
extern unsigned int    g_nLiquidAdditiveComponent;
extern BOOL            g_bLiquidEnabledHistory;
extern unsigned int    g_nLiquidAdditiveOutputDAValue;             //  liquid additive d/a value
extern float           g_fLiquidAdditiveOutputPercentage;             //  liquid additive d/a value
extern float           g_fLiquidKgPerHourPerPercentOutput;             //

extern BOOL            g_bModbusLiquidDToAWritten;
extern BOOL            g_bModbusLiquidDToAPercentageWritten;
extern BOOL            g_bLiquidControlAllowed;
extern unsigned int    g_nCheckIfWeighHopperOpenedSecCtr;             //
extern float           g_fWeightAtOpenDumpFlap;             //
extern WORD            g_nTemperature;
extern unsigned int    g_nLiquidOnTimeCtr;
extern BOOL            g_bAlarmOccurredHistory[MAXIMUM_ALARMS][MAX_COMPONENTS];
extern float           g_fOneSecondHopperWeightAtEndOfCycle;             //
extern float           g_fThroughputCalculationBatchWeight;             //
extern float           g_fKgperHour;             //
extern float           g_fLongTermResettableLengthCounter;
extern float           g_fLongTermLengthCounter;
extern unsigned int    g_nTelnetSequence;
extern BOOL            g_bTSMTelnetInSession;
extern BOOL            g_bTelnetUserNameOkay;
extern BOOL            g_bShowNetworkWrites;
extern BOOL            g_bWriteDiagnosticsToSDCard;
extern unsigned int    g_nTelNetTimeoutCtr;
extern unsigned int    g_nTelnetContinuousUpdate;
extern unsigned int    g_nFileSegmentNo;
extern char            g_cTelnetFileName[20];
extern BOOL            g_bPrintCycleDataToTelnet;
extern long            g_lTCPIPCommsCounter;  // tcpip counter
extern unsigned int    g_nTelNetSecondsCounter;
extern unsigned int    g_bTelNetWindowCtr;
extern char            g_cOrderReportStorageBuffer[ORDER_REPORT_BUFFER_SIZE];
extern BOOL            g_bAToDMaxExceeded;
extern BOOL            g_bApplicationSPIInProgress;
extern BOOL            g_bTaskSPIInProgress;
extern BOOL            g_bTelnetSDCardAccessInProgress;
extern BOOL            g_bHourlyResetMax;
extern BOOL            g_bHourlyResetMin;


extern structHistoryComponentData   g_sOngoingHistoryComponentLogData;
extern structHistoryComponentData   g_sHourHistoryComponentLogData;
extern structHistoryIndividualEvents g_sOngoingHistoryEventsLogData;
extern structHistoryIndividualEvents g_sHourHistoryEventsLogData;

extern structHistoryIndividualEvents g_s24HrOngoingHistoryEventsLogData;
extern structHistoryIndividualEvents g_s24HrHistoryEventsLogData;

extern structHistoricCommsData       g_sOngoingHistorySEIComms;
extern structHistoricCommsData       g_sOngoingHistoryLLSComms;
extern structHistoricCommsData       g_sOngoingHistoryOPTComms;
extern structHistoricCommsData       g_sOngoingHistoryTCPIPComms;
extern structHistoricCommsData       g_sOngoingHistoryNetworkComms;

extern structHistoricCommsData       g_sHourHistorySEIComms;
extern structHistoricCommsData       g_sHourHistoryLLSComms;
extern structHistoricCommsData       g_sHourHistoryOPTComms;
extern structHistoricCommsData       g_sHourHistoryTCPIPComms;
extern structHistoricCommsData       g_sHourHistoryNetworkComms;

extern   char    g_OrderReportName[20];
extern   char    g_cEventLogFileCopy[EVENTLOGSTRINGLENGTH];
extern unsigned int    g_nUSBReadTimeoutInPits;
extern unsigned int    g_nUSBCommandSendTimeoutInPits;
extern unsigned int    g_nUSBFlushTimeoutInPits;
extern unsigned int    g_nUSBWriteTimeoutInPits;
extern unsigned int    g_nUSBEchoedTimeoutInPits;
extern unsigned int    g_nUSBUSBSendFileCommandsTimeoutInPits;
extern unsigned int    g_nUSBGetPrinterStatusTimeoutInPits;

extern unsigned int    g_nUSBReadTimeoutCtr;
extern unsigned int    g_nUSBCommandSendTimeoutCtr;
extern unsigned int    g_nUSBFlushTimeoutCtr;
extern unsigned int    g_nUSBWriteTimeoutCtr;
extern unsigned int    g_nUSBEchoedTimeoutCtr;
extern unsigned int    g_nUSBUSBSendFileCommandsTimeoutCtr;
extern unsigned int    g_nUSBGetPrinterStatusTimeoutCtr;

extern BOOL            g_bUSBDebug;

extern BOOL            g_bHiRegLevelSensor;              // regrind high ls
extern unsigned int    g_nHiRegLSCoveredCounter;
extern unsigned int    g_nHiRegLSUnCoveredCounter;
extern unsigned int    g_nExpansionOutput2;
extern BOOL            g_bWriteToExpansion2;              //
extern int             g_nCyclePauseTypeCopy;                                      // CYCLEPAUSETYPE  DS      1       ; INDICATES PAUSE TYPE START / END OF CYCLE.
extern unsigned int    g_nBlastMomentaryOnCtr[MAX_COMPONENTS];
extern unsigned int    g_nFunctionRunning;
extern unsigned int    g_nFunctionSubSectionRunning;
extern BOOL            g_bResetNetworkSlaveComms;
extern BOOL            g_bResetPanelSlaveComms;
extern BOOL            g_bAddAlarmOccurredToEventLog;
extern BOOL            g_bAddAlarmClearedToEventLog;
extern unsigned int    g_ucAlarmCode;
extern unsigned int    g_nAlarmComponent;
extern BOOL            g_bNoParityHistory;
extern StructDebug     g_nTSMDebug;
extern structSelfTest  g_sSelfTest;
extern unsigned int    g_nSPISelect1;
extern unsigned int    g_nTimeInAutoMinutesCtr;

extern BOOL            g_bDoSelfTestDiagnostics;
extern BOOL            g_bInLastMinute;
extern WORD            g_wHourHistory;  // hourly history counter
extern BOOL            g_bHourRollOver;
extern char            g_cPanelVersionNumber[PANEL_REVISION_STRING_SIZE];
extern DescriptionStruct g_sDescriptionData;
extern unsigned int    g_nStorageInvervalCtr;
extern long            g_lOrigBatchPulsesAccumulator;
extern char            g_cSEISoftwareVersionNumber[SEI_SOFTWARE_REVISION_STRING_SIZE];
extern BOOL            g_bUseLineSpeedForGPMCalc;
extern BOOL            g_bNoLeakAlarmCheck;
extern BOOL            g_bHiRegLevelSensorHistory;
extern BOOL            g_bDumpFlapHasOpened;
extern BOOL            g_bI2CError;
extern unsigned int    g_nblenderpulsetestctr;
extern BOOL            g_bPulseValveNow;
extern unsigned int    g_bPulseTime;
extern WORD            g_wForeGroundCounter;
extern WORD            g_wProgramLoopEntry;
extern char            g_cModbusPrintBuffer[1000];
extern BOOL            g_bPrintModbusMessageToTelnet;

extern unsigned int    g_nPulsingSequence[MAX_COMPONENTS];
extern long            g_lComponentTargetCountsCopy[MAX_COMPONENTS];
extern float           g_fComponentTargetWeightCopy[MAX_COMPONENTS];
extern BOOL            g_bFastSettling;
extern unsigned int    g_nComponentPulseCounter[MAX_COMPONENTS];
extern float           g_fComponentActualWeightHistory[MAX_COMPONENTS];
extern BOOL            g_bFirstPulse;
extern BOOL            g_bCalibratePulsing[MAX_COMPONENTS];
extern unsigned int    g_nPulseTime;
extern unsigned int    g_nPulseTimeForMinWeight[MAX_COMPONENTS];
extern BOOL            g_bICSRecipeDataWritten;
extern BOOL            g_bSaveAdditionalDataToEEPROM;
extern unsigned int    g_nValidFlowRateExceededCtr[MAX_COMPONENTS];
extern BOOL            g_bAcceptFlowRate[MAX_COMPONENTS];
extern BOOL            g_bLockIsEnabled;
extern BOOL            g_bPitRunning;
extern BOOL            g_bConfigIsLocked;
extern char            g_cModbusEventBuffer[MODBUS_EVENT_LOG_BUFFER_LENGTH];
extern BOOL            g_bAddEventToEventLog;
extern unsigned int    g_nHoldOffEventLogWriteCtr;
extern BOOL            g_bAtoDEndOfConversionErrorHistory;
extern BOOL            g_bAtoDNegativeSignHistory;
extern unsigned int    g_nWaitForSPIBusToSettleCtr;
extern BOOL            g_bDoAToDTest;
extern unsigned int    g_nBlenderTypeHistory;
extern BOOL            g_bSelfTestInProgress;
extern float           g_fCalibrationWeight;
extern unsigned int    g_nFillingMethodHistory[MAX_COMPONENTS];
extern BOOL            g_bGotHalfGramTime;
extern unsigned int    g_nHalfGramTime[MAX_COMPONENTS];
extern float           g_fRequiredPulseWeightInGrams[MAX_COMPONENTS];
extern float           g_fCurrentRecipePercentageHistory[MAX_COMPONENTS];
extern float           g_fMaxKgPHrPossible;
extern unsigned int    g_nAfterEndOfCycleCounter;
extern BOOL            g_bVAC8ExpansionRequired;
extern unsigned int    g_nVAC8ExpansionIndex;
extern unsigned int    g_nVAC8ExpansionOutput;
extern unsigned int    g_nVaccumLoadingVAC8IOCards;
extern long            g_lSettledAverageCounts[MAX_COMPONENTS];
extern long            g_nAToDStabilityLog[MAX_STABILITY_READINGS];
extern BOOL            g_bLogLoadCellReading;
extern unsigned int    g_nDelayBeforeLoadCellLogCtr;
extern unsigned int    g_nAToDStabilityCtr;
extern long            g_nAToDStabilityMaxValue;
extern long            g_nAToDStabilityMinValue;

extern long            g_nAToDRefStabilityLog[MAX_STABILITY_READINGS];
extern BOOL            g_bTakeRefAToDReading;
extern unsigned int    g_nDelayBeforeRefLogCtr;
extern unsigned int    g_nAToDRefStabilityCtr;
extern long            g_nAToDRefStabilityMaxValue;
extern long    		   g_nAToDRefStabilityMinValue;
extern BOOL            g_bRefAToDStabilityTestInProgress;
extern BOOL            g_bDecideOnRefStability;
extern BOOL            g_bDecideOnLoadCellStability;
extern unsigned int    g_nVac8ExpansionHistory;
extern unsigned int    g_nRefTimeoutCtr;
extern unsigned int    g_nTopUpCompNoRefHistory;
extern float           g_fWeightLeftAfterRegrind;
extern float           g_fRegrindWeight;
extern BOOL            g_bFileCouldNotBeOpenedEvent;
extern unsigned int    g_nMountFatReturnedValue;
extern unsigned int    g_nChangeDriveReturnedValue;
extern BOOL            g_bComponentHasRetried[MAX_COMPONENTS];


#endif  // __BATVARS2_H__


