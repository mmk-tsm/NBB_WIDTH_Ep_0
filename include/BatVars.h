/////////////////////////////////////////////////
// BatVars.h
//
// Include file containing batch global variables
// used in more than one class or function.
//
// M.McKiernan                                                  14-07-2004
// First pass
//
// P.Smith                                                          19-08-2004
// Define multi stage filling variable required
// M.McKiernan                                                  16-09-2004
// Added g_lStoredComponentCountsStage1[], g_lStoredComponentCountsStage2[]
// added g_nStageFillEnHistory
// P.Smith                                                          24-09-2004
// Added g_bVenturiFillByWeight,g_bVenturiFirstTime
// M.McKiernan                                                  28-09-2004
// Added g_nLoadConfigurationFailure
//
// M.McKiernan                                                  07-10-2004
// Added g_bManualAutoFlag, g_bSaveAllCalibrationToEEprom, g_bCalibrationDataChange, g_bNewMaxScrewSpeed
// Added g_nDACPerKgDoneFlag
// Added g_nSaveRecipeSequence g_nSaveRecipeByteNo
// Added g_bSBBL2Fitted - g_bSBBL5Fitted                    // SBB link options
//
// M.McKiernan                                                  15-11-2004
// Pause flag redefined from bool to an int g_nPauseFlag
// New definitions - see: // Nov 15th
// Renamed g_bPToNormal to g_bPauseToNormal.
// g_bByPassMode, g_bOverrideOutputs, g_bFeedValveClosed g_bCleanAtEndOfCycle, g_bPauseOnActive
// g_bSecondRollover g_bForegroundSecondRollover, g_bPrevLevSen1Stat, g_bMixerTripped, g_nMixerTrippedCounter
// g_nPauseOnActiveCounter, g_nPauseOnInActiveCounter, g_bExtruderControlStartUpFlag
// Added g_bLowLevelSensorStatus[]
// Added g_fDtoAPerKGH, g_fDtoAPerMPM, g_nSEIInputStates, g_nSEIAlarms, g_fActualGrammesPerM2
//
// M.McKiernan                                                  25-11-2004
// Added g_bDiagnosticsPasswordInUse, g_nDeviceToPollCounter
// g_bCheckIfShouldCycleFlag, g_nCheckIfShouldCycleCounter, g_bShowInstantaneousWeight, g_bREadIncDecCalibrationData, g_nDoIncDecReadInNSeconds
// g_nIgnoreLSTime, g_nBatchMonitorCounter, g_bRunLevelSensorCheck, g_nBypassSensorUncoveredCounter, g_nPauseInNSeconds
// g_bUpdateHiLoFlag
// g_bSEIPollInputsFlag, g_bWriteDAValueFlag,   g_bSEIPollLSFlag, g_bWriteAlarmsFlag, g_bReadDAValueFlag, g_bHourFlag
// g_nDoDiagnosticsCounter, g_bDoDiagnosticsFlag, g_nPITFractionCounter
// g_nSeqTimeout, g_bPeripheralCommsSTarted, g_nLineSpeedFractionCounter, g_nTinySlideOpenTimeCounter;
// g_nForegroundTenHzFlag, g_nTinySlideCycleCounter
// g_nTenthsOfSecondCounter2, g_bSEIPollIncDecFlag, g_nCMRValidTimer, g_bRemoteKeyPoll;
// g_nSecondCounter2, g_bFGSecondFlag2, g_bFGOneMinuteFlag;
// g_nDisplayUpdateCounter, g_bUpdateDisplayNow, g_nMixerOffCounter, g_nMixerOnTime
// g_nExtruderDAValue g_nMixerTransitionCounter
//  g_nSecondCounter g_bTwoSecondFlag g_nTenSecondCounter g_bTenSecondFlag
// g_nFrontRollChangeLag, g_nBackRollChangeLag
// g_nMixScrewPulseCounter, g_bMixScrewRotationAlarm
// g_nRTCSecondCounter, g_bRTCMinuteFlag, g_nRTCMinuteCounter
// g_bOneSecondFlag, g_nKgHrSecondsCounter, g_nLevelSensorSimulator
// M.McKiernan                                                  13-12-2004
// Added  g_bAlarmRelayStatus, g_bAlarmBeaconStatus
//
// M.McKiernan                                                  05-01-2005
// Added
//  g_fKgHRRBuffer[MAX_KGH_RR_SAMPLES] g_nKgHRRIndex
//  g_nKgHRRSummationCounter        g_fRRThroughputKgPerHour   g_fRRThroughputLbPerHour  g_fThroughputKgPerHour
//  g_fThroughputLbPerHour  g_fBatchKgPerHour g_fBatchTimeInSeconds  g_nPreviousPITTime;
// g_fRRKgPerHour, g_fRRLbPerHour
//
// M.McKiernan                                                  18-01-2005
// g_bRemoteOperatorPanelPresent, g_bSEIPresent, g_bLLSPresent
// g_nVramStartDisplayIndex, g_nVramEndDisplayIndex, g_nDeviceListIndexUFP1, g_nDeviceListIndexUFP2
// g_nDeviceListIndexSEI, g_nDeviceListIndexLLS, g_nCommsUpdateStartIndex, g_nStartAttributeUpdateIndex
// g_bCommsUpdateActive, g_bCommsAttributeUpdateActive;
//
// P.Smith                                                  10-02-2005
// Change g_nCCVsFlag to Bool
//
// P.Smith                                                  5/9/05
// Remove g_nLSDelay
//
// P.Smith                                                  14/9/05
// Correct g_bForegroundTenHzFlag flag
//
// P.Smith                                                  24/11/05
// make g_nExtruderDAValue,g_nDA1Temp unsigned ints
//
// P.Smith                                                  1/12/05
// remove g_bWriteDAValueFlag
//
// P.Smith                                                  19/1/06
// added "extern to unsigned int            g_nDA1Temp;                   // DA1TEMP
// extern added to
// extern long   g_lCPIStage2[MAX_COMPONENTS]; // test purposes only
// extern long   g_lCPIStage3[MAX_COMPONENTS]; // test purposes only
// extern added to
// extern unsigned int            g_nExtruderDAValue;             // DA1TMP.
//
// P.Smith                      2/2/06
// first pass at netburner hardware conversion.
// comment out #include "StdTypes.h"
// Bool -> BOOL
//
// P.Smith                                                  29/5/05
// make g_nTinySlideCycleCounter  unsigned int
//
// P.Smith                                                  6/11/06
// name change g_bCycleIndicate
//
// P.Smith                          30/1/08
// correct compiler warnings by making some ints unsigned
// M.McKiernan                      04/4/2008
// added fdTelnet.
//
// P.Smith                          8/9/09
// name change to g_fCycleLoadTimePrn and g_lLoadTime
//
// P.Smith                          15/12/09
// changed g_nLastCompTableIndex to insigned int
/////////////////////////////////////////////////

//*****************************************************************************
// MACROS
//*****************************************************************************
#ifndef __BATVARS_H__
#define __BATVARS_H__


//#include "Hardware.h"
#include "General.h"
//#include <StdLib.h>
#include "RRFormat.h"
#include <basictypes.h>
#include "TSMPeripheralsMBIndices.h"

// #include "BatchMBIndices.h"
// #include "BatchCalibrationFunctions.h"

extern  BOOL    g_bResetFlag;                                                                           //RESETFLG
extern  BOOL    g_bResetADFLag;                                                                 // RESETAD

extern  long    g_lRawAtDCounts;                                                                        // Raw A/D counts (long int)
extern  float   g_lWeightAtoDCounts;
extern  float g_fWeightInHopper;                                                                // WEIGHTINHOP   WEIGHT IN HOPPER AFTER FILLING Hopper weight based on Avg or settled counts.
extern  float g_fWeightInHopperMinusRegrind;                            // WEIGHTAFTREG
extern  float   g_fPreviousBatchWeight;                                                 // PREVBATCHWEIGHT DS      3       ; PREVIOUS BATCH WEIGHT
extern  float   g_fCurrentBatchWeight;                                                  // CURBATWGH       DS      4       ; BATCH WEIGHT STORAGE TO 1/10 GRAM.

extern  long    g_lHalfSecondSum;                                                                       // SUMLC1
extern  int     g_nHalfSecondSumCounter;                                                // SAMPLELC1
extern  long    g_lHalfSecondWeightAverage;                                     // ANHSLC1H - ANHSLC1 (note Hex & BCD in assembler)
extern  BOOL    g_bHalfSecondWeightAverageAvailable;            // WGTHSECAV

extern  long    g_lOneSecondSum;                                                                        // SUM132
extern  int     g_nOneSecondSumCounter;                                                 // NEN132
extern  long    g_lOneSecondWeightAverage;                                              // AN1BUFH - AN1BUF     (note Hex & BCD in assembler)
extern  BOOL    g_bOneSecondWeightAverageAvailable;                     // WGT1SECAVG
extern  float g_fOneSecondHopperWeight;                                         // Hopper weight calculated from 1 sec avg.

extern  long    g_lTwoSecondSum;                                                                        // SUMLC3
extern  int     g_nTwoSecondSumCounter;                                                 // SAMPLELC3
extern  long    g_lTwoSecondWeightAverage;                                              // ANHSLC3H - ANHSLC3H  (note Hex & BCD in assembler)
extern  BOOL    g_bTwoSecondWeightAverageAvailable;                     //

extern  long    g_lEighthSecondSum;                                                                             // SUMLC2
extern  int     g_nEighthSecondSumCounter;                                                      // SAMPLELC2
extern  long    g_lEighthSecondWeightAverage;                                           // ANHSLC2H - ANHSLC2   (note Hex & BCD in assembler)
extern  BOOL    g_bEighthSecondWeightAverageAvailable;                  // WGTESECAV

extern   unsigned int   g_nPreviousWeightAvgFactor;                      // PREVWGTAVGFACTOR
extern   long  g_lAvgSum;                                               // AVSUM132
extern   int   g_nAvgSumCounter;                                        // SAMCOUNTER
extern   long  g_lSettledAverage;                                       // AN1AVGBUF (not exactly same, g_lSettledAverage is the final average, i.e. Sum/no. of samples.
extern   BOOL  g_bWeightAvgAvail;                                       // AVGAVAIL

extern  long    g_lHopperTareCounts;                                                            // HOPPERTARECOUNTS
extern  long    g_lCurrentLCAtDTare;                                                            // CURRLCADTARE


extern  long    g_lCurrentLCAtDValue;                                                   // CURRLCADH

extern  long    g_lCmpRawCounts[MAX_COMPONENTS];                                // CMP1RAWCOUNTS - CMP12RAWCOUNTS
extern  long    g_lCmpTareCounts[MAX_COMPONENTS];                       // New - diagnostic purposes.
extern  long    g_lCmpTareCounts[MAX_COMPONENTS];                       // CMP1TARECOMPONENTCOUNTS -  CMP12TARECOMPONENTCOUNTS   DS      3                       ; TARE COUNTS



extern  long    g_lComponentCountsActual[MAX_COMPONENTS];        // CMP1CNAC - CMP12CNAC (ACTUAL COUNTS PER COMPONENT).
extern  float g_fComponentActualWeight[MAX_COMPONENTS];  // CMP1ACTWGT-CMP12ACTWGT     ; COMP #1-12 ACTUAL WEIGHT (METRIC)
extern  float g_fComponentActualWeightI[MAX_COMPONENTS]; // CMP1ACTWGTI-CMP12ACTWGTI     ; COMP #1-12 ACTUAL WEIGHT IMPERIAL.

extern  float   g_fComponentActualWeightRR[MAX_COMPONENTS];  //CMP1ACTWGTRR-CMP12ACTWGTRR; COMPONENT #N ACTUAL WEIGHT (round robin)
extern  float   g_fComponentActualWeightRRI[MAX_COMPONENTS];  //CMP1ACTWGTRRI-CMP12ACTWGTRRI; COMPONENT #N ACTUAL WEIGHT (round robin, imperial)

extern  float g_fComponentActualPercentage[MAX_COMPONENTS]; // BPCNT1-BPCNT1     ; COMP #1-12 ACTUAL %'s

extern  float g_fComponentTargetWeight[MAX_COMPONENTS];  // CMP1TARWGT - CMP12TARWGT component #n target weight
extern  float g_fComponentTargetWeightI[MAX_COMPONENTS]; // CMP1TARWGTI - CMP12TARWGTI component #n target weight Imperial units.
extern  long    g_lComponentTargetCounts[MAX_COMPONENTS];        // CMP1CNTG - CMP12CNTG - Component #n target wt. in A/D counts.
extern  long    g_lStoredComponentTargetCounts[MAX_COMPONENTS];  // CMP1CNTGSTR-CMP12CNTGSTR     DS      3       ; STORAGE OF INITIAL TARGET
extern  long    g_lComponentTargetCountsTotal[MAX_COMPONENTS];  // CMP1CNTGTOT-CMP12CNTGTOT     DS      3 ; COMPONENT #N  COUNT TARGET

extern  float   g_fComponentTargetTopUpWeight[MAX_COMPONENTS];  // CMP1SETTOPUPWT- CMP21SETTOPUPWT  DS      3       ; COMPONENT #N  WEIGHT  DS
extern  float   g_fComponentTargetTopUpWeightI[MAX_COMPONENTS]; // CMP1SETTOPUPWI- CMP21SETTOPUPWTI  Imperial units version

extern  BOOL    g_bComponentInSeqTable[MAX_COMPONENTS];                 // whether comp. is in SeqTable or not.

extern  float g_fUsedTargetWeight;                                                              // Temporary used in component weight calculations

extern BOOL     g_bFillStatCmp[MAX_COMPONENTS];
extern BOOL     g_bLFillStatCmp[MAX_COMPONENTS];
extern int      g_nStageSeq[MAX_COMPONENTS];                //STAGESEQ1- 12
extern  BOOL  g_bFillStatusAll;
extern  BOOL    g_bDumpStatus;                                                                                  // DUMPSTATUS
extern  BOOL    g_bMixOnStatus;                                                                         // MIXON
extern  BOOL    g_bMixOff;                                                                                              // MIXOFF - mixer off command.

extern  BOOL    g_bOutputDiagMode;
extern BOOL     g_bActivateLatency;
extern  int     g_nValveLatencyTime;                                                                    // VALVELATENCYTIME DS     2       ; VALVE LATENCY TIME.

extern BOOL     g_bFillIsOnCmp[MAX_COMPONENTS];                                 // CLEAR flag to indicate component fill is OFF.

extern BOOL     g_bAutoCycleTestFlag;
extern BOOL     g_bAutoCycleFlag;
extern BOOL     g_bWeightAvailableFlag;

extern BOOL     g_bFirstTime;
extern unsigned int     g_nPreviousTime;
extern unsigned int     g_nCurrentTime;
extern BOOL     g_bIsCounting;
extern long     g_lTimeMeasured;                                                // clear time measured variable (long)

extern unsigned int      g_nPITCom;

extern float            g_fComponentSetWeight[MAX_COMPONENTS];
extern BOOL             g_bComponentValveState[MAX_COMPONENTS];
extern unsigned int g_nOperationalStatus;
extern int              g_nLFillStatCtr[MAX_COMPONENTS];

extern long     g_lCmpOpenClk[MAX_COMPONENTS];

extern unsigned char    g_cSeqTable[MAX_COMPONENTS + 4];                        // leave space for delimiter.
extern unsigned char    g_cPrevSeqTable[MAX_COMPONENTS + 4];    // leave space for delimiter.

extern BOOL             g_bFirstWCycle;
extern BOOL             g_bRegrindPresent;                                                      // REG_PRESENT
// extern unsigned char g_cResetRRobinCmp[MAX_COMPONENTS];
extern int              g_nSeqTableIndex;                                                               // SEQPOINTER.
extern unsigned int     g_nLastCompTableIndex;                                          //LASTCOMPTABLEP

// extern unsigned char g_cLFillStatCtr[MAX_COMPONENTS];
extern BOOL                     g_bFBWStatus[MAX_COMPONENTS];                   // fill by weight bit.

extern BOOL                             g_bFstCompCalFin;                                                               // "FIRST COMP CALCULATION FINISHED ?  //FSTCOMPCALFIN
extern char                             g_cBatSeq;                                                              // Batch sequence                                                               //BATSEQ
extern BOOL                     g_bFillAlarm;                                                   //                                                                                                      //FILLALARM
extern int                              g_nAnyFillAlarm;                                                // ANYFILLALARM    DS      1       ; ANY FILL ALARMS IN CYCLE.
extern unsigned int     g_nDmpSec;                                                              // timer used in batching seq.                  //DMPSEQ
extern  BOOL                    g_bFirstBatch;                                                  // flag to indicate first batch                 //FIRSTBATCH
extern  BOOL                    g_bStopTransition;                                              // stop transition flag                                         // STOPTRAN
extern  BOOL                    g_bRunTransition;                                               // run transition flag                                          // RUNTRAN
extern  BOOL                    g_bCycleIndicate;                                               // Cycle indication variable                            // CYCLEINDICATE
extern  BOOL                    g_bStopped;                                                             // STOPPED         DS      1       ; STOPPED FLAG
extern  int                     g_nRunCounter;                                                  // RUNCTR          DS      1       ; INC WHILE RUNNING.
extern  int                     g_nStopCounter;                                         // STOPCTR          DS      1       ; INC WHILE stopped.
extern  BOOL                    g_bStopRunDisplayEnable;                        // STPRNDIS        DS      1       ; STOP RUN DISPLAY ENABLE.

extern long                             g_lComponentOnTimer;                                                                                                                                            // CMPNTIM
extern long                             g_lCmpTime[MAX_COMPONENTS];             // required time per component.                         //CMP1TIMS       ; TIME STORAGE BLOCK
extern float                    g_fComponentOpenSeconds[MAX_COMPONENTS];                //CMP1OPENSECONDS-CMP12OPENSECONDS DS      3       ; OPENING SECONDS FOR COMP N
extern long                             g_lComponentVolumetricTimer[MAX_COMPONENTS];    //CMP1VCT-CMP12VCT    ; COMPONENT NO FILL TIME STORAGE

extern  char                            g_cCalcRouter;                                                  // CALCROUTER
extern  int                             g_nAvgCtr;                                                              // AVGCTR

extern  int                             g_nShutDownCtr;                                         //SHUTDOWNCTR
extern  int                             g_nHGSen1Lag;                                                   // HGSEN1LAG
extern  BOOL                            g_bDmpLevSen;                                                   // DMPLEVSEN
extern  BOOL                            g_bLevSen1Stat;                                         // LEVSEN1STAT
extern  BOOL                g_bPrevLevSen1Stat;  // PREVLS1STAT
//extern  BOOL                            g_bPauseFlag;                                                   // PAUSFLG
extern  int                             g_nPauseFlag;                                                   // PAUSFLG
extern  BOOL                            g_bPauseTransition;                                     // PAUSETRAN
extern  BOOL                            g_bOffLSF;                                                              // OFFLSF
extern  int                             g_nLevSenOffTime;                                               // LEVSENOFFTIME
extern  unsigned char   g_cLS1OffCt;                                                    // LS1OFFCT
extern   int                            g_nLevelSensorDelayCtr;                         // LEVELSENSORDELAYCTR
extern   int                            g_nHGSen1Lag;                                                   // HGSEN1LAG
extern   unsigned int                   g_nNoCycle;                                                             // NOCYCLE
extern  BOOL                            g_bDoorOpen;                                                    // DOOROPEN
extern  int                             g_nEstCtrUse;                                                   // ESTCTRUSE            ESTIMATION FOR A FEW CYCLES.
extern  BOOL                            g_bRunEst;                                                              // RUNEST          DS      1       ; RUN THE ESTIMATE FLAG.
extern  int                             g_nAfterOpenCounter;                                    // AFTEROPEN  - counter
extern  BOOL                            g_nCleanCycleSeq;                                               // CLEANCYCLESEQ
extern  unsigned int                    g_nBlenderCleanRepetionsCounter;        // BCLEANREPETITIONSCTR    DS      1       ; BLENDER CLEAN REPETITIONS
extern  unsigned int                    g_nMixerCleanRepetionsCounter;  // MCLEANREPETITIONSCTR    DS      1       ; MIXER CLEAN REPETITIONS
extern  int                             g_nDumpRepetitions;                                     // DUMPREPETITIONS         DS      1       ; DUMP REP
extern  int                             g_nAirJet1OnCounter;                                    // AIRJET1ONCTR            DS      2       ; AIR JET ONE COUNTER

extern  int                             g_nPremLSCtr;                                                   // PREMLSCTR
extern  BOOL                            g_bLevelSensorAlarmOccurred;            // LEVELSENALOC

extern  int                             g_nSelectedRecipeNo;                                    // Recipe no. selected, when in Current Recipe entry.

extern  BOOL                            g_bPauseToNormal;                                                   // PTONORMAL       DS      1       ; PAUSE TO NORMAL TRANSITION.
extern  unsigned int                    g_nFillCtr;                                                             //FILLCTR         DS      1       ; FILL COUNTER
extern  int                             g_nFillCounter;                                         //FILLCOUNTER     DS      1       ; FILL COUNTER.
extern  unsigned char   g_cRetryActive;                                         // RETRYACTIVE     DS      1       ; RETRY ACTIVE.
extern  int                             g_nRetryCounter;                                                // RETRYCOUNTER fill RETRY COUNTER
extern  int                             g_nTopUpSequencing;                                     // TOPUPSEQUENCING         DS      1       ; TOP UP SEQUENCING.
extern  BOOL                            g_bMultipleFilling;                                     // MULTIPLEFILLING DS      1       ; MULTIPLE FILLING FLAG
extern  BOOL                            g_bTopUpFlag;                                                   // TOPUPF          DS      1       ;
extern  BOOL                            g_bStageFillCalc;                                               // STAGEFILLCALC    STAGE FILL CALC
extern  BOOL                            g_bNoTopUpRequired;                                     // NOTOPUPREQUIRED DS      1       ; NO  TOP UP REQUIRED.
extern  BOOL                            g_bMoveOn;                                                              // MOVEON          DS      1       ; MOVE ON FLAG.
extern  BOOL                            g_bChangingOrder;                                               // CHANGINGORDER   DS      1       ; CHANGING ORDER.
extern  int                             g_nCorrectRetries;                                      // CORRECTRETRIES  DS      1       ; CORRECT RETRIES IN PROGRESS

extern  unsigned char   g_cCompNoDetails;                                               // COMPNODETAILS   DS      1       ; TEMPORY STORAGE FOR COMPONENT DETAILS.
extern  unsigned char   g_cCompNo;                                                              // COMPNO          DS      1       ; CURRENT COMPONENT POINTER

extern  int                             g_nAnyRetry;                                                    // ANYRETRY        DS      1       ; INDICATES THAT THE BLENDER HAS RETRIED.
extern  int                             g_nCyclePauseType;                                              // CYCLEPAUSETYPE  DS      1       ; INDICATES PAUSE TYPE START / END OF CYCLE.

extern  BOOL                            g_bPrintNow;                                                    // PRINTNOW        DS      1       ; PRINTER ACTIVATION FLAG
extern  int                             g_nPrintInNSeconds;                                     // PRINTINNSECONDS DS      1       ; PRINT IN N SECONDS
extern  float                           g_fCycleLoadTimePrn;                                    // CYCLELOADTIMEPRN DS      2       ; CYCLE LOAD TIME(INDICATES MAX THROUGHPUT)
extern  long                            g_lLoadTime;                                                   // CYCLETIME       DS      2       ; CYCLE TIME.
extern  int                             g_nCycleTimePrn;                                                // CYCLETIMEPRN    DS      2       ; CYCLE TIME (PRINTING??).

extern  BOOL                            g_bPctFlag;                                                             // PCTFLG DS     1        ;~THRU-PUT % FLAG, FOR UPDATE
extern  BOOL                            g_bCSAFlag;                                                             // CSAFLG DS     1        ;~CAL. SETPOINT (KG/H) FOR BLEND 1/A FLAG
extern  BOOL                            g_nTransferSetpointsFlag;                       // TRANSFERSPF     DS      1       ; TRANSFER SETPOINTS FLAG.

extern  double                          g_ffComponentCPI[MAX_COMPONENTS]; // CMP1CPI counts per interrupt (PIT period)
extern  double                          g_ffComponentCPIStage2[MAX_COMPONENTS]; //CMP1CPIST2
extern  double                          g_ffComponentCPIStage3[MAX_COMPONENTS]; //CMP1CPIST3
extern  double                          g_ffCPI;                                                                        // CPI value. (used to pass value to/from CPI R.R.
extern  double                          g_ffCPI_RR;                                                             // CPI value from Round robin.
extern  long                            g_lCPI[MAX_COMPONENTS];                         // test purposes only
extern long                                      g_lCPIStage2[MAX_COMPONENTS]; // test purposes only
extern long                                      g_lCPIStage3[MAX_COMPONENTS]; // test purposes only

extern  double  g_ffInstantaneousCPI[MAX_COMPONENTS];  // only for testpurposes.

extern  BOOL                            g_bTaringStatus;                                                // TARINGSTATUS - Tare Status.
extern  BOOL                            g_bAnyFillByWeight;                                     // ANYFILLBYWEIGHT DS      1       ; ANY FILL BY WEIGHT FLAG.
extern  BOOL                            g_bResetStageSeq;                                               // RESETSTAGESEQ   DS      1       ;MULTIPLE FILL TO NORMAL FILL.

extern  int                             g_nOffline_Tracking;                                    // OFFLINE_TRK     DS      1       ; OFFLINE TRACKING FLAG.
extern  BOOL                            g_bDisableControlAction;                        // DISCONACTION    DS      1       ; DISABLE CONTROL ACTION
extern  BOOL                            g_bBatchReady;                                                  // BATCHREADY      DS      1       ; BATCH READY
extern  unsigned int                    g_nOffTime1;                                                    // OFFTIM1         DS      1       ; OFF LINE 1/10 SECOND COUNTER.

extern  int                             g_nVolCycCtr;                    // VOLCYCCTR       DS      2       ; VOLUMETRIC cycle COUNTER.
extern  int                             g_nRetryCompNoHis;                                      // RETRYCOMPNOHIS  DS      1       ; RETRY COMPONENT NO HISTORY
extern  BOOL                            g_bPctHis;                                                              // PCTHIS          DS      1       ; HISTORY OF % UPDATE.
extern  BOOL                            g_bAnyOn;                                                               // ANYON           DS      1       ; ONE OF THE WAS SWITCHED ON.
extern  BOOL                            g_bFillTargetEntryFlag;                         // FLAGFILLTARGETENTRYF DS     1       ; FILL TARGET FLAG

extern  BOOL                            g_bHasRetried[MAX_COMPONENTS];                  // HASRETRIED1     DS      1       ; COMPONENT HAS RETRIED #1
extern  BOOL                            g_bHasRetriedStage2[MAX_COMPONENTS];    // HASRETRIED1ST2  (STAGE 2)
extern  BOOL                            g_bHasRetriedStage3[MAX_COMPONENTS];    // HASRETRIED1ST3  (STAGE 3)

extern  int                             g_nRetryCounterStage1[MAX_COMPONENTS];   // RETRYCTR1       DS      1       ; COMPONENT RETRY COUNTER.
extern  int                             g_nRetryCounterStage2[MAX_COMPONENTS];  // RETRYCTR1ST2   STAGE 2
extern  int                             g_nRetryCounterStage3[MAX_COMPONENTS];  // RETRYCTR1ST3  STAGE 3

// extern  BOOL                            g_bIgnoreCPI[MAX_COMPONENTS];                           // IGNORECPI1      DS      1        ; IGNORE CPI #1
extern  BOOL                            g_bRegrindFillProblem;                                          // REGRINDFILLPROBLEM      DS      1        ; INDICATE PROBLEM ON REGRIND

extern  BOOL                            g_bNoFlowRateCalculation;              // NOFLOWRATECALCULATION       DS      1 ; NO FLOWRATE CALCULATION.
extern  BOOL                            g_bNoFlowRateCalculationArray[MAX_COMPONENTS];  // NOFLOWRATECALCULATION1-12 - no flow rate calculation for each component.

extern  BOOL                            g_bUnderFill;                                                                   // UNDERFILL       DS      1       ; UNDERFILL FLAG

extern  structCPIRRData g_MainCPIRRData;                // main CPI Round Robin structure
extern  structCPIRRData g_Stage2CPIRRData;      // Stage 2 CPI Round Robin structure
extern  structCPIRRData g_Stage3CPIRRData;      // Stage 3 CPI Round Robin structure



extern  int                             g_nStageFillEnTemp;                  // STAGEFILLENTEMP DS      1       ; STAGE FILL TEMP, USED IN CALIBRATION MENU
extern  BOOL                            g_bLayeringTemp;                     // LAYERINGTEMP    DS      1       ; LAYERING TEMP LOC
extern  BOOL                            g_bCopyCompFlowRate[MAX_COMPONENTS]; // COPYFLOWRATECMP1 DS      1       ;
// redefined..extern  long                            g_lCompCountTargetStore[MAX_COMPONENTS];// CMP1CNTGSTR     DS      3       ; STORAGE OF INITIAL TARGET
extern  int                             g_nFillStage;                         // FILLSTAGE               DS      1       ; STAGE FILL
extern  BOOL                            g_bMulFillStarted[MAX_COMPONENTS];    // MULFILL1STARTED DS      1       ; MULTIPLE FILL STARTED

// Sept 7th 04
extern  int            g_nCheckLSensorInNSeconds;     // CHECKLSENSORINNSECONDS     DS      1       ; CHECK LLS
extern  BOOL                g_bNoFillOnClean;                    // NOFILLONCLEAN           DS      1       ; NO FILL ON CLEAN
extern  BOOL                g_bPhaseOneFinished;                 // PHASEONEFINISHED        DS      1       ; INDICATION WHEN PHASE 1 CLEAN IS FINISHED

extern  int             g_nLeftCornerCleanOnCounter;     // CCLONCTR                DS      2       ; LEFT CC
extern  int             g_nRightCornerCleanOnCounter;    // CCRONCTR                DS      2       ; Right CC
extern  int             g_nAirJet2OnCounter;                 // AIRJET2ONCTR

extern  BOOL                g_bHighLevelSensor;             // HIGHLEVELSENSOR DS      1       ; HIGH LEVEL SENSOR.
extern  BOOL                g_bHighLevelSensorHistory;      // HIGHLEVELSENSORHIS DS      1       ; HIGH LEVEL SENSOR.
extern  BOOL                g_bLowLevelSensor;              // LOWLEVELSENSOR  DS      1       ; LOW LEVEL SENSOR.
extern  BOOL                g_bLowLevelSensorHistory;       // LOWLEVELSENSORHIS  DS      1       ; LOW LEVEL SENSOR.

extern  int             g_nHighLSCoveredCounter;        // HIGHONCTR
extern  int             g_nHighLSUnCoveredCounter;      // HIGHOFFCTR
extern  int             g_nLowLSCoveredCounter;         // LOWONCTR
extern  int             g_nLowLSUnCoveredCounter;       // LOWOFFCTR
extern  BOOL                g_bOverrideClean;                   // OVERRIDECLEAN
extern  BOOL                g_bSetCleanWhenCycleEnd;        // SETCLEANWHENCYCLEEND
extern  int             g_nMixerOnCounter10Hz;          // MIXERONCTR10HZ ,,, 10 HZ COUNTER FOR MIXING

extern  float               g_fAvgHopperWeight;             // weight return from CalculateHopperWeightAvg (asm = CALHWTAV)

// Sept 16th 04
extern  long    g_lStoredComponentCountsStage1[MAX_COMPONENTS];  // CMP1CNTGSTR1    DS      3       ; ACTUAL COUNTS FOR STAGE 1 OF FILL
extern  long    g_lStoredComponentCountsStage2[MAX_COMPONENTS];  // CMP1CNTGSTR2    DS      3       ; ACTUAL COUNTS FOR STAGE 2 OF FILL
extern  int     g_nStageFillEnHistory;                  // STAGEFILLENH    DS      1       ;STAGE FILL ENABLE HISTORY

// Sept 24th
extern  BOOL            g_bVenturiFillByWeight;         //VENTURIFILLBYWEIGHTH DS 1       ; HISTORY OF FBW
extern  BOOL            g_bVenturiFirstTime;                //VENTURIFIRSTTIME DS     1       ; INDICATE THAT 1ST TIME USING VENTURI
// Sept 28th
extern  int         g_nLoadConfigurationFailure;    // status of load configuration, bits set if problem

// OCT 7th
extern  BOOL            g_bManualAutoFlag;              //MANATO manual or automatic
extern  BOOL            g_bAutoTransition;              //Auto transition flag  AMANTR DS     1        ;~AUTO/MAN TRAN. FLAG

extern  BOOL            g_bSaveAllCalibrationToEEprom;      //CEEFLG1/CEEFLG INDICATE CAL DAT STORE TO EEPROM
extern  BOOL            g_bCalibrationDataChange;       // CALFLG
extern  BOOL            g_bNewMaxScrewSpeed;                // NMAXSS
extern  int         g_nDACPerKgDoneFlag;                // DACPKGDONEFLAG  DS      2       ; DAC PER KG DONE

extern  int         g_nSaveRecipeSequence;          // sequence for ForegroundSaveRecipe..
extern  unsigned int g_nSaveRecipeByteNo;                // Byte no or index for ForegroundRecipeSave.

extern   BOOL           g_bL2Fitted;                     // SBB link options
extern   BOOL           g_bL3Fitted;
extern   BOOL           g_bSBBL4Fitted;
extern   BOOL           g_bSBBL5Fitted;

// Nov 15th
extern   BOOL           g_bNewSetpointsFlag;                // NEWSET
extern  int         g_nLoadNewSetpointLag;          // LDNSPLAG
extern   BOOL            g_bCCVsFlag;                        // CCVSFLG
extern   int            g_nToStallTime;                 // TOSTALTIM
extern   int            g_nExtruderStalledCounter;      // EXTSCTR
extern   BOOL           g_bManualChangeFlag;                // MCHGFLG
extern  unsigned int            g_nDA1Temp;                   // DA1TEMP
extern  BOOL            g_bEnterKeyPressedFlag;         // EKPRSFLG

extern  int         g_nCommsIncreaseFlag;           //COMMSIFLG
extern  int         g_nCommsDecreaseFlag;           //COMMSDFLG
extern  float           g_fExtruderRPM;                 // Extruder screw rpm (asm =SSRPM/SSRPM2D
extern  BOOL            g_bByPassMode;                      // BYPASS
extern  BOOL            g_bOverrideOutputs;             // OVERRIDEOUTPUTS         DS      1       ; OVER RIDE OUTPUTS
extern  BOOL            g_bFeedValveClosed;             // FEEDCLOSE
extern  BOOL            g_bCleanAtEndOfCycle;           // CLEANATENDOFCYCLE
extern  BOOL            g_bPauseOnActive;                   // PAUSEONACTIVE    ; PAUSE ON FLAG ACTIVE
extern  BOOL            g_bSecondRollover;              // SECROVER - set by RTC int.
extern  BOOL            g_bForegroundSecondRollover;    // new, second roll over flag for foreground.
extern  BOOL            g_bMixerTripped;                    // LEVSEN2STAT
extern  int         g_nMixerTrippedCounter;         // new, counter for debouncing mixer tripped out.
extern  int         g_nPauseOnActiveCounter;        // PONACTIVECTR
extern  int         g_nPauseOnInactiveCounter;      // POFFACTIVECTR
extern  BOOL            g_bExtruderControlStartUpFlag;  // STRUPF
extern  BOOL            g_bExtruderControlStartUpTransition;    //  STUPTR DS     1

extern  BOOL            g_bLowLevelSensorStatus[MAX_COMPONENTS];    // LS1OFF - LS12OFF

extern   float          g_fDtoAPerMPM;                      // DACPMPM  D/A PER M/min
extern   int            g_nSEIInputStates;              // SEIINPUTSSTATE  inputs on SEI
extern   int            g_nSEIAlarms;                       // SEIALARMSTATE alarms on SEI
extern  float           g_fActualGrammesPerM2;          // GPM2VAR  g/m2 actual value.


extern  BOOL            g_bDiagnosticsPasswordInUse;    // ISDIAGNOSTICSPASSWORD
extern  int         g_nDeviceToPollCounter;       // DEVICETOPOLLCTR

extern   BOOL           g_bCheckIfShouldCycleFlag;  // CHKIFCYCLEFLAG    ; CHECK IF THE BLENDER SHOULD
extern   int            g_nCheckIfShouldCycleCounter; // CHKIFCYCLECTR
extern   BOOL           g_bShowInstantaneousWeight; // SHOWINWEIGHT - set flag to show intan
extern   BOOL           g_bREadIncDecCalibrationData;   // READINCDECCAL - read calibration from SE
extern   int            g_nDoIncDecReadInNSeconds;      // DOREADINNSECONDS
extern   int            g_nIgnoreLSTime;                    // IGNORELSTIME,,
extern   int         g_nBatchMonitorCounter;       // BATCHMONCTR     DS      2       ;
extern   int         g_bRunLevelSensorCheck;       //  asm = CHECKFORMAXTHP
extern  int         g_nBypassSensorUncoveredCounter;   // BYPASSSENSOROFFCTR
extern  int         g_nPauseInNSeconds;             // PAUSEINNSECONDS - will start pause.
extern  BOOL            g_bUpdateHiLoFlag;              //UPDATEREGHILOF          ; UPDATE REGHILO

extern   BOOL           g_bSEIPollInputsFlag;           // SEIPOLLINPUTSF          ; START POLLING OF INPUTS
extern   BOOL           g_bSEIPollLSFlag;                   // SEIPOLLLLSF             ; SET LOW LEVEL SENSOR
extern   BOOL           g_bWriteAlarmsFlag;             // WRITEALARMSF
extern   BOOL           g_bReadDAValueFlag;             // READDAVALUEF Trigger A/D read from SEI.
extern  BOOL            g_bHourFlag;                        // HOURFG
extern  int         g_nDoDiagnosticsCounter;        // DIAGCOUNTER
extern  BOOL            g_bDoDiagnosticsFlag;           // no asm equiv.  Flag set to call DoDiagnostics in foreground.
extern  int         g_nPITFractionCounter;          // PITFRACTCTR
extern   int            g_nSeqTimeout;                      // SEQTIMEOUT
extern   BOOL           g_bPeripheralCommsSTarted;  //COMMSST   comms started??
extern   int            g_nLineSpeedFractionCounter;    // LSPDFRACTIONCTR
extern	unsigned int   g_nTinySlideCycleCounter;       // TINYSLIDECYCLECOUNTER
extern   int            g_nTinySlideOpenTimeCounter;    // TINYSLIDEOPENTIMECOUNTER
extern  BOOL        g_bForegroundTenHzFlag;         // 10Hz flag for foreground. loosely equates to TENHZF1. int to allow
                                                        // foreground to cater for each 10Hz period.

extern   int            g_nTenthsOfSecondCounter2;      // LSTTHS
extern   BOOL           g_bSEIPollIncDecFlag;           // SEIPOLLINCDECF
extern   int            g_nCMRValidTimer;                   // CMRVALID
extern   BOOL           g_bRemoteKeyPoll;                   // REMKEYPOLL
extern   int         g_nSecondCounter2;             // LSSECS
extern   BOOL           g_bFGSecondFlag2;                   // flag to call One second program in foreground.
extern   BOOL           g_bFGOneMinuteFlag;             // flag to call One second program in foreground.

extern   int            g_nDisplayUpdateCounter;        // DISPLAYUPDATECTR
extern   BOOL           g_bUpdateDisplayNow;                // UPDATEDISPLAYNOW
extern   int            g_nMixerOffCounter;             // MIXEROFFCTR
extern   int            g_nMixerOnTime;                 // MIXONTIME    - mixer on for N s
extern unsigned int            g_nExtruderDAValue;             // DA1TMP.

extern   int            g_nMixerTransitionCounter;      // MIXERTRANSITIONCTR

extern   int            g_nSecondCounter;                   // SECCNT
extern   BOOL           g_bTwoSecondFlag;                   // TWOSFLG  - indicate 2 seconds rollover
extern   int            g_nTenSecondCounter;                // TSCCNT
extern   BOOL           g_bTenSecondFlag;                   // TSCFLG            ;~10 SEC. FLAG

extern   int            g_nFrontRollChangeLag;          // RCF_LAG  -Front reel change lag
extern   int            g_nBackRollChangeLag;           // RCB_LAG  -Back reel change lag
extern   int            g_nMixScrewPulseCounter;        //MIXSCREWPULSECTR   READ MIXING SCREW PULSE COUNTER
extern   BOOL           g_bMixScrewRotationAlarm;       // MIXSCREWROTALARM

extern   int            g_nRTCSecondCounter;            //
extern   BOOL           g_bRTCMinuteFlag;               // set flag on Minute transition.
extern   int            g_nRTCMinuteCounter;
extern   BOOL           g_bOneSecondFlag;       // ONESECONDFLAG, set by RTC.
extern   int            g_nKgHrSecondsCounter;          // KGHRCOUNT
extern   int            g_nLevelSensorSimulator;        //LEVELSIM              ; INCREMENT LEVEL SENSOR SIMULATOR

extern  BOOL            g_bAlarmRelayStatus;                // flag to show state of alarm relay
extern  BOOL            g_bAlarmBeaconStatus;           // flag to show state of alarm beacon

// 2005

extern   float          g_fKgHRRBuffer[MAX_KGH_RR_SAMPLES];     // kg/h round robin buffer.
extern   unsigned int   g_nKgHRRIndex;                                  // index or pointer for kg/h r.r.
extern   unsigned int   g_nKgHRRSummationCounter;           // summation counter for kg/h r.r.
extern   float          g_fRRThroughputKgPerHour;           //RRTPTKGH
extern   float          g_fRRThroughputLbPerHour;           //RRTPTLBH
extern   float          g_fThroughputKgPerHour;             // TPTKGH
extern   float          g_fThroughputLbPerHour;             // TPTLBH
extern   float          g_fBatchKgPerHour;                  // BATCHKGPHR
extern   float          g_fBatchTimeInSeconds;              // BATCHSECS
extern   int            g_nPreviousPITTime;                 // PREVPITM
// 18.1.2005
extern  BOOL            g_bRemoteOperatorPanelPresent;  //
extern  BOOL            g_bSEIPresent;                          //
extern  BOOL            g_bLLSPresent;                          //
extern  int         g_nVramStartDisplayIndex;           //  Address in VRAM of first char to be sent to remote panel.
extern  int         g_nVramEndDisplayIndex;             //

extern   int            g_nDeviceListIndexUFP1;             // Device list index for UFP1
extern   int            g_nDeviceListIndexUFP2;             // Device list index for UFP2
extern   int            g_nDeviceListIndexSEI;              // Device list index for SEI
extern   int            g_nDeviceListIndexLLS;              // Device list index for LLS
extern   int         g_nDeviceListIndexLIW[MAX_LIW_CARDS];  // Device list index for LIW'S
extern   int         g_nDeviceListIndexVAC8IO[MAX_VAC8IO_CARDS];  // Device list index for VAC8IO'S


extern  int         g_nCommsUpdateStartIndex;           //
extern  int         g_nStartAttributeUpdateIndex;       //
extern  BOOL            g_bCommsUpdateActive;               //
extern  BOOL            g_bCommsAttributeUpdateActive;  //

extern  int         fdTelnet;                           // fd for telnet task - global.
#endif  // __BATVARS_H__

