/////////////////////////////////////////////////
// BatVars.
//
// Include file containing batch global variables
// used in more than one class or function.
//
// M.McKiernan                                                  23-06-2004
// First pass
//
// P.Smith                                                          19-08-2004
// Define multi stage filling variable required
// M.McKiernan                                                  03-09-2004
// Removed some double def's
// g_lCompCountTargetStore[MAX_COMPONENTS],  g_nStageSeq[MAX_COMPONENTS],g_cLFillStatCtr[MAX_COMPONENTS]
// Fixed def's g_bMulFillStarted[MAX_COMPONENTS]
// g_cResetRRobinCmp[] and g_bIgnoreRRobinCmp[] now handled by CPI r.r. structure - see RRFormat.h.
//
// M.McKiernan                                                  16-09-2004
// Added g_lStoredComponentCountsStage1[], g_lStoredComponentCountsStage2[]
// Put in asm equivalent of g_nStageSeq[].
// g_nStageFillEnHistory
// g_lCPIStage2[MAX_COMPONENTS];    // test purposes only
// g_lCPIStage3[MAX_COMPONENTS];    // test purposes only
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
// g_bSecondRollover g_bForegroundSecondRollover, g_bPrevLevSen1Stat,g_bMixerTripped, g_nMixerTrippedCounter
// g_nPauseOnActiveCounter, g_nPauseOnInActiveCounter, g_bExtruderControlStartUpFlag
// Added g_bLowLevelSensorStatus[]
// Added g_fDtoAPerKGH, g_fDtoAPerMPM, g_nSEIInputStates, g_nSEIAlarms, g_fActualGrammesPerM2
//
// M.McKiernan                                                  25-11-2004
// Added g_bDiagnosticsPasswordInUse, g_nDeviceToPollCounter
// g_bCheckIfShouldCycleFlag, g_nCheckIfShouldCycleCounter, g_bShowInstantaneousWeight, g_bREadIncDecCalibrationData, g_nDoIncDecReadInNSeconds
// g_nIgnoreLSTime, g_nBatchMonitorCounter, g_bRunLevelSensorCheck, g_nBypassSensorUncoveredCounter
// g_nPauseInNSeconds , g_bUpdateHiLoFlag
// g_bSEIPollInputsFlag, g_bWriteDAValueFlag,   g_bSEIPollLSFlag, g_bWriteAlarmsFlag, g_bReadDAValueFlag, g_bHourFlag
// g_nDoDiagnosticsCounter, g_bDoDiagnosticsFlag, g_nPITFractionCounter
// g_nSeqTimeout, g_bPeripheralCommsSTarted, g_nLineSpeedFractionCounter, g_nTinySlideOpenTimeCounter;
// g_nForegroundTenHzFlag, g_nTinySlideCycleCounter
// g_nTenthsOfSecondCounter2, g_bSEIPollIncDecFlag, g_nCMRValidTimer, g_bRemoteKeyPoll;
// g_nSecondCounter2, g_bFGSecondFlag2, g_bFGOneMinuteFlag;
// g_nDisplayUpdateCounter, g_bUpdateDisplayNow, g_nMixerOffCounter, g_nMixerOnTime;
// g_nExtruderDAValue g_nMixerTransitionCounter
//  g_nSecondCounter g_bTwoSecondFlag g_nTenSecondCounter g_bTenSecondFlag
// g_nFrontRollChangeLag, g_nBackRollChangeLag
// g_nMixScrewPulseCounter, g_bMixScrewRotationAlarm
// g_nRTCSecondCounter, g_bRTCMinuteFlag, g_nRTCMinuteCounter
// g_bOneSecondFlag, g_nKgHrSecondsCounter, g_nLevelSensorSimulator
//
// M.McKiernan                                                  13-12-2004
// Added  g_bAlarmRelayStatus, g_bAlarmBeaconStatus
//
// M.McKiernan                                                  05-01-2005
// Added
//g_fKgHRRBuffer[MAX_KGH_RR_SAMPLES] g_nKgHRRIndex
//g_nKgHRRSummationCounter        g_fRRThroughputKgPerHour   g_fRRThroughputLbPerHour  g_fThroughputKgPerHour
//g_fThroughputLbPerHour  g_fBatchKgPerHour g_fBatchTimeInSeconds  g_nPreviousPITTime;
// g_fRRKgPerHour, g_fRRLbPerHour
//
// M.McKiernan                                                  18-01-2005
// g_bRemoteOperatorPanelPresent, g_bSEIPresent, g_bLLSPresent
// g_nVramStartDisplayIndex, g_nVramEndDisplayIndex, g_nDeviceListIndexUFP1, g_nDeviceListIndexUFP2
// g_nDeviceListIndexSEI, g_nDeviceListIndexLLS, g_nCommsUpdateStartIndex, g_nStartAttributeUpdateIndex
// g_bCommsUpdateActive, g_bCommsAttributeUpdateActive;
//
//
// P.Smith                                                  4-02-2005
// g_AutoTransition
//
// P.Smith                                                  10-02-2005
// Change g_nCCVsFlag to Bool
//
// P.Smith                                                  30-08-2005
// Remove g_fRRKgPerHour,g_fRRLbPerHour
//
// P.Smith                                                  5/9/05
// Remove g_nLSDelay
//
// P.Smith                                                  24/11/05
// make g_nExtruderDAValue,g_nDA1Temp unsigned ints
//
// P.Smith                                                  1/12/05
// remove g_bWriteDAValueFlag
//
// P.Smith                                                  29/5/05
// make g_nTinySlideCycleCounter  unsigned int
//
// P.Smith                          30/1/08
// correct compiler warnings
//
// M.McKiernan                      04/4/2008
// added fdTelnet.
//
// P.Smith                          04/4/2008
// name change g_bSBBL2Fitted to g_bL2Fitted and g_bSBBL3Fitted to g_bSBBL3Fitted
//
// P.Smith                          25/6/09
// g_nDeviceListIndexLIW[MAX_LIW_CARDS] & g_nDeviceListIndexVAC8IO[MAX_VAC8IO_CARDS];
//
// P.Smith                          8/9/09
// name change to g_fCycleLoadTimePrn and g_lLoadTime
//
// P.Smith                          14/12/09
// make g_nLastCompTableIndex an unsigned int to correct compiler warnings
//
/////////////////////////////////////////////////

//*****************************************************************************
// MACROS
//*****************************************************************************

//#include "Hardware.h"
#include "General.h"
//#include "BatVars.h"
//#include        "RRFormat.h"

// #include "BatchMBIndices.h"
// #include "BatchCalibrationFunctions.h"
#include <basictypes.h>
#include "RRFormat.h"
#include "TSMPeripheralsMBIndices.h"


BOOL    g_bResetADFLag;                                                                 // RESETAD
BOOL    g_bResetFlag;                                                                           //RESETFLG

long    g_lRawAtDCounts;                                // Raw A/D counts (long int)
float g_lWeightAtoDCounts;                      // long version of one second avg. (g_lOneSecondWeightAverage)
float g_fWeightInHopper;                                                                // WEIGHTINHOP   WEIGHT IN HOPPER AFTER FILLING Hopper weight based on Avg or settled counts.
float g_fWeightInHopperMinusRegrind;                            // WEIGHTAFTREG
float   g_fPreviousBatchWeight;                                                 // PREVBATCHWEIGHT DS      3       ; PREVIOUS BATCH WEIGHT
float   g_fCurrentBatchWeight;                                                  // CURBATWGH       DS      4       ; BATCH WEIGHT STORAGE TO 1/10 GRAM.

long    g_lHopperTareCounts;                                                            // HOPPERTARECOUNTS
long    g_lCurrentLCAtDTare;                                                            // CURRLCADTARE

long    g_lCurrentLCAtDValue;                                                   // CURRLCADH

long    g_lHalfSecondSum;                                                                       // SUMLC1
int     g_nHalfSecondSumCounter;                                                // SAMPLELC1
long    g_lHalfSecondWeightAverage;                                     // ANHSLC1H - ANHSLC1 (note Hex & BCD in assembler)
BOOL    g_bHalfSecondWeightAverageAvailable;            // WGTHSECAV

long    g_lOneSecondSum;                                                                        // SUM132
int     g_nOneSecondSumCounter;                                                 // NEN132
long    g_lOneSecondWeightAverage;                                              // AN1BUFH - AN1BUF     (note Hex & BCD in assembler)
BOOL    g_bOneSecondWeightAverageAvailable;                     // WGT1SECAVG
float g_fOneSecondHopperWeight;                                         // Hopper weight calculated from 1 sec avg.

long    g_lTwoSecondSum;                                                                        // SUMLC3
int     g_nTwoSecondSumCounter;                                                 // SAMPLELC3
long    g_lTwoSecondWeightAverage;                                              // ANHSLC3H - ANHSLC3H  (note Hex & BCD in assembler)
BOOL    g_bTwoSecondWeightAverageAvailable;                     //

long    g_lEighthSecondSum;                                                                             // SUMLC2
int     g_nEighthSecondSumCounter;                                                      // SAMPLELC2
long    g_lEighthSecondWeightAverage;                                           // ANHSLC2H - ANHSLC2   (note Hex & BCD in assembler)
BOOL    g_bEighthSecondWeightAverageAvailable;                  // WGTESECAV

unsigned int     g_nPreviousWeightAvgFactor;                     // PREVWGTAVGFACTOR
long    g_lAvgSum;                                                              // AVSUM132
int     g_nAvgSumCounter;                                               // SAMCOUNTER
long    g_lSettledAverage;                                      // AN1AVGBUF (not exactly same, g_lSettledAverage is the final average, i.e. Sum/no. of samples.
BOOL                            g_bWeightAvgAvail;                                      // AVGAVAIL


long    g_lCmpRawCounts[MAX_COMPONENTS];                                // CMP1RAWCOUNTS - CMP12RAWCOUNTS
long    g_lCmpTareCounts[MAX_COMPONENTS];                       // CMP1TARECOMPONENTCOUNTS -  CMP12TARECOMPONENTCOUNTS   DS      3                       ; TARE COUNTS


long    g_lComponentCountsActual[MAX_COMPONENTS];       // CMP1CNAC - CMP12CNAC (ACTUAL COUNTS PER COMPONENT).
float   g_fComponentActualWeight[MAX_COMPONENTS]; // CMP1ACTWGT-CMP12ACTWGT     ; COMP #1-12 ACTUAL WEIGHT (METRIC)
float   g_fComponentActualWeightI[MAX_COMPONENTS]; // CMP1ACTWGTI-CMP12ACTWGTI     ; COMP #1-12 ACTUAL WEIGHT IMPERIAL.

float   g_fComponentActualWeightRR[MAX_COMPONENTS];  //CMP1ACTWGTRR-CMP12ACTWGTRR; COMPONENT #N ACTUAL WEIGHT (round robin)
float   g_fComponentActualWeightRRI[MAX_COMPONENTS];  //CMP1ACTWGTRRI-CMP12ACTWGTRRI; COMPONENT #N ACTUAL WEIGHT (round robin, imperial)

float   g_fComponentActualPercentage[MAX_COMPONENTS]; // BPCNT1-BPCNT1     ; COMP #1-12 ACTUAL %'s

float   g_fComponentTargetWeight[MAX_COMPONENTS];  // CMP1TARWGT - CMP12TARWGT component #n target weight
float   g_fComponentTargetWeightI[MAX_COMPONENTS]; // CMP1TARWGTI - CMP12TARWGTI component #n target weight Imperial units.

long    g_lComponentTargetCounts[MAX_COMPONENTS];        // CMP1CNTG - CMP12CNTG - Component #n target wt. in A/D counts.
long    g_lStoredComponentTargetCounts[MAX_COMPONENTS];  // CMP1CNTGSTR-CMP12CNTGSTR     DS      3       ; STORAGE OF INITIAL TARGET
long    g_lComponentTargetCountsTotal[MAX_COMPONENTS];  // CMP1CNTGTOT-CMP12CNTGTOT     DS      3 ; COMPONENT #N  COUNT TARGET

float   g_fComponentTargetTopUpWeight[MAX_COMPONENTS];  // CMP1SETTOPUPWT- CMP21SETTOPUPWT  DS      3       ; COMPONENT #N  WEIGHT  DS
float   g_fComponentTargetTopUpWeightI[MAX_COMPONENTS]; // CMP1SETTOPUPWI- CMP21SETTOPUPWTI  Imperial units version

BOOL    g_bComponentInSeqTable[MAX_COMPONENTS];         // whether comp. is in SeqTable or not.

float   g_fUsedTargetWeight;                                                              // Temporary used in component weight calculations

BOOL    g_bFillIsOnCmp[MAX_COMPONENTS];                                 // CLEAR flag to indicate component fill is OFF.

BOOL    g_bFillStatCmp[MAX_COMPONENTS];
BOOL    g_bLFillStatCmp[MAX_COMPONENTS];                    // LFILLSTATCMP
BOOL    g_bFillStatusAll;                                                                               // FILLSTATUSALL
int     g_nStageSeq[MAX_COMPONENTS];                        // STAGESEQ1 - 12
BOOL    g_bDumpStatus;                                                                                  // DUMPSTATUS
BOOL    g_bMixOnStatus;                                                                         // MIXON
BOOL    g_bMixOff;                                                                                              // MIXOFF - mixer off command.

BOOL    g_bOutputDiagMode;                  // OUTPUTDIAG
BOOL    g_bActivateLatency;                                                                     // ACTIVATELATENCY
int     g_nValveLatencyTime;                                                                    // VALVELATENCYTIME DS     2       ; VALVE LATENCY TIME.

BOOL    g_bAutoCycleTestFlag;
BOOL    g_bAutoCycleFlag;
BOOL    g_bWeightAvailableFlag;                 // WGTAVAIL - indicates A/D conversion complete.

BOOL    g_bFirstTime;
unsigned int    g_nPreviousTime;
unsigned int    g_nCurrentTime;
BOOL    g_bIsCounting;
long    g_lTimeMeasured;                                                // clear time measured variable (long)

unsigned int     g_nPITCom;                                                              // PITCOM

float           g_fComponentSetWeight[MAX_COMPONENTS];
BOOL            g_bComponentValveState[MAX_COMPONENTS];
unsigned int    g_nOperationalStatus;
int             g_nLFillStatCtr[MAX_COMPONENTS];                // LFILLSTATCTRx

long            g_lCmpOpenClk[MAX_COMPONENTS];

unsigned char   g_cSeqTable[MAX_COMPONENTS + 4];                        // leave space for delimiter.
unsigned char   g_cPrevSeqTable[MAX_COMPONENTS + 4];    // leave space for delimiter.
int             g_nSeqTableIndex;                                                               // SEQPOINTER.
unsigned int    g_nLastCompTableIndex;                                          //LASTCOMPTABLEP

BOOL            g_bFirstWCycle;                                         // FIRSTWCYCLE     DS      1        ; FIRST CYCLE FLAG
BOOL            g_bRegrindPresent;                                      // REG_PRESENT
// handled by CPI RR structure unsigned char g_cResetRRobinCmp[MAX_COMPONENTS];
// unsigned char g_cLFillStatCtr[MAX_COMPONENTS];
BOOL            g_bFBWStatus[MAX_COMPONENTS];                 // fill by weight bit.

BOOL            g_bFstCompCalFin;                                       // "FIRST COMP CALCULATION FINISHED ?  //FSTCOMPCALFIN
char            g_cBatSeq;                                                              // Batch sequence                                                               //BATSEQ
BOOL            g_bFillAlarm;                                                   //                                                                                                      //FILLALARM
int             g_nAnyFillAlarm;                                                // ANYFILLALARM    DS      1       ; ANY FILL ALARMS IN CYCLE.
unsigned int    g_nDmpSec;                                                              // timer used in batching seq.                  //DMPSEQ
BOOL            g_bFirstBatch;                                                  // flag to indicate first batch                 //FIRSTBATCH
BOOL            g_bStopTransition;                                      // stop transition flag                                         // STOPTRAN
BOOL            g_bRunTransition;                                               // run transition flag                                          // RUNTRAN
BOOL            g_bCycleIndicate;                                               // CYCLEINDICATE  // Cycle indication var
BOOL            g_bStopped;                                                             // STOPPED         DS      1       ; STOPPED FLAG
int             g_nRunCounter;                                                  // RUNCTR          DS      1       ; INC WHILE RUNNING.
int             g_nStopCounter;                                         // STOPCTR          DS      1       ; INC WHILE stopped.
BOOL            g_bStopRunDisplayEnable;                        // STPRNDIS        DS      1       ; STOP RUN DISPLAY ENABLE.

long            g_lComponentOnTimer;                                                                                                                                            // CMPNTIM
long            g_lCmpTime[MAX_COMPONENTS];             // required time per component.                         //CMP1TIMS       ; TIME STORAGE BLOCK
float           g_fComponentOpenSeconds[MAX_COMPONENTS];                //CMP1OPENSECONDS-CMP12OPENSECONDS DS      3       ; OPENING SECONDS FOR COMP N

long            g_lComponentVolumetricTimer[MAX_COMPONENTS];    //CMP1VCT-CMP12VCT       DS      3       ; COMPONENT NO FILL TIME STORAGE

char            g_cCalcRouter;                                                  // CALCROUTER
int             g_nAvgCtr;                                                              // AVGCTR

int             g_nShutDownCtr;                                         //SHUTDOWNCTR
//--removed-- declared twice int             g_nHGSen1Lag;                                                   // HGSEN1LAG
BOOL            g_bDmpLevSen;                                                   // DMPLEVSEN
BOOL            g_bLevSen1Stat;                                         // LEVSEN1STAT
BOOL            g_bPrevLevSen1Stat;  // PREVLS1STAT

int             g_nPauseFlag;                                                   // PAUSFLG
BOOL            g_bPauseTransition;                                     // PAUSETRAN
BOOL            g_bOffLSF;                                                              // OFFLSF
int             g_nLevelSensorDelayCtr;                         // LEVELSENSORDELAYCTR
int             g_nHGSen1Lag;                                                   // HGSEN1LAG
unsigned int    g_nNoCycle;                      // NOCYCLE
BOOL            g_bDoorOpen;                                                    // DOOROPEN
int             g_nLevSenOffTime;                                               // LEVSENOFFTIME
unsigned char   g_cLS1OffCt;                                                    // LS1OFFCT
int             g_nEstCtrUse;                                                   // ESTCTRUSE =ESTIMATION FOR A FEW CYCLES.
BOOL            g_bRunEst;                                                              // RUNEST          DS      1       ; RUN THE ESTIMATE FLAG.
int             g_nAfterOpenCounter;                                    // AFTEROPEN  - counter
BOOL            g_nCleanCycleSeq;                                               // CLEANCYCLESEQ
unsigned int    g_nBlenderCleanRepetionsCounter;        // BCLEANREPETITIONSCTR    DS      1       ; BLENDER CLEAN REPETITIONS
unsigned int    g_nMixerCleanRepetionsCounter;  // MCLEANREPETITIONSCTR    DS      1       ; MIXER CLEAN REPETITIONS
int             g_nDumpRepetitions;                                     // DUMPREPETITIONS         DS      1       ; DUMP REP
int             g_nAirJet1OnCounter;                                    // AIRJET1ONCTR            DS      2       ; AIR JET ONE COUNTER

int             g_nPremLSCtr;                                                   // PREMLSCTR

BOOL            g_bLevelSensorAlarmOccurred;            // LEVELSENALOC

int             g_nSelectedRecipeNo;                                    // Recipe no. selected, when in Current Recipe entry.

BOOL            g_bPauseToNormal;                                                   // PTONORMAL       DS      1       ; PAUSE TO NORMAL TRANSITION.
unsigned int             g_nFillCtr;                                                             // FILLCTR         DS      1       ; FILL COUNTER
int             g_nFillCounter;                                         // FILLCOUNTER     DS      1       ; FILL COUNTER.
unsigned char   g_cRetryActive;                                         // RETRYACTIVE     DS      1       ; RETRY ACTIVE.
int             g_nRetryCounter;                                                // RETRYCOUNTER fill RETRY COUNTER
int             g_nTopUpSequencing;                                     // TOPUPSEQUENCING         DS      1       ; TOP UP SEQUENCING.
BOOL            g_bMultipleFilling;                                     // MULTIPLEFILLING DS      1       ; MULTIPLE FILLING FLAG
BOOL            g_bTopUpFlag;                                                   // TOPUPF          DS      1       ;
BOOL            g_bStageFillCalc;                                               // STAGEFILLCALC    STAGE FILL CALC
BOOL            g_bNoTopUpRequired;                                     // NOTOPUPREQUIRED DS      1       ; NO  TOP UP REQUIRED.
BOOL            g_bMoveOn;                                                              // MOVEON          DS      1       ; MOVE ON FLAG.
BOOL            g_bChangingOrder;                                               // CHANGINGORDER   DS      1       ; CHANGING ORDER.
int             g_nCorrectRetries;                                      // CORRECTRETRIES  DS      1       ; CORRECT RETRIES IN PROGRESS

unsigned char   g_cCompNoDetails;                                               // COMPNODETAILS   DS      1       ; TEMPORY STORAGE FOR COMPONENT DETAILS.
unsigned char   g_cCompNo;                                                              // COMPNO          DS      1       ; CURRENT COMPONENT POINTER

int             g_nAnyRetry;                                                    // ANYRETRY        DS      1       ; INDICATES THAT THE BLENDER HAS RETRIED.
int             g_nCyclePauseType;                                      // CYCLEPAUSETYPE  DS      1       ; INDICATES PAUSE TYPE START / END OF CYCLE.

BOOL            g_bPrintNow;                                                    // PRINTNOW        DS      1       ; PRINTER ACTIVATION FLAG
int             g_nPrintInNSeconds;                                     // PRINTINNSECONDS DS      1       ; PRINT IN N SECONDS

float           g_fCycleLoadTimePrn;                                    // CYCLELOADTIMEPRN DS     2       ; CYCLE LOAD TIME(INDICATES MAX THROUGHPUT)
long            g_lLoadTime;                                                   // CYCLETIME       DS      2       ; CYCLE TIME.
int             g_nCycleTimePrn;                                                // CYCLETIMEPRN    DS      2       ; CYCLE TIME (PRINTING??).

BOOL            g_bPctFlag;                                                             // PCTFLG DS     1        ;~THRU-PUT % FLAG, FOR UPDATE
BOOL            g_bCSAFlag;                                                             // CSAFLG DS     1        ;~CAL. SETPOINT (KG/H) FOR BLEND 1/A FLAG
BOOL            g_nTransferSetpointsFlag;                       // TRANSFERSPF     DS      1       ; TRANSFER SETPOINTS FLAG.

double          g_ffComponentCPI[MAX_COMPONENTS]; // CMP1CPI counts per interrupt (PIT period)
double          g_ffComponentCPIStage2[MAX_COMPONENTS]; //CMP1CPIST2
double          g_ffComponentCPIStage3[MAX_COMPONENTS]; //CMP1CPIST3
double          g_ffCPI;                                                                        // CPI value. (used to pass value to/from CPI R.R.
double          g_ffCPI_RR;                                                             // CPI value from Round robin.
double          g_ffInstantaneousCPI[MAX_COMPONENTS];  // only for testpurposes.

long            g_lCPI[MAX_COMPONENTS];                         // test purposes only
long            g_lCPIStage2[MAX_COMPONENTS]; // test purposes only
long            g_lCPIStage3[MAX_COMPONENTS]; // test purposes only

BOOL            g_bTaringStatus;                                                // TARINGSTATUS - Tare Status.
BOOL            g_bAnyFillByWeight;                                     // ANYFILLBYWEIGHT DS      1       ; ANY FILL BY WEIGHT FLAG.
BOOL            g_bResetStageSeq;                                               // RESETSTAGESEQ   DS      1       ;MULTIPLE FILL TO NORMAL FILL.

int             g_nOffline_Tracking;                                    // OFFLINE_TRK     DS      1       ; OFFLINE TRACKING FLAG.
BOOL            g_bDisableControlAction;                        // DISCONACTION    DS      1       ; DISABLE CONTROL ACTION
BOOL            g_bBatchReady;                                                  // BATCHREADY      DS      1       ; BATCH READY

unsigned int    g_nOffTime1;                                                    // OFFTIM1         DS      1       ; OFF LINE 1/10 SECOND COUNTER.

int             g_nVolCycCtr;                    // VOLCYCCTR       DS      2       ; VOLUMETRIC cycle COUNTER.
int             g_nRetryCompNoHis;                                      // RETRYCOMPNOHIS  DS      1       ; RETRY COMPONENT NO HISTORY
BOOL            g_bPctHis;                                                              // PCTHIS          DS      1       ; HISTORY OF % UPDATE.
BOOL            g_bAnyOn;                                                               // ANYON           DS      1       ; ONE OF THE WAS SWITCHED ON.

BOOL            g_bFillTargetEntryFlag;                         // FLAGFILLTARGETENTRYF DS     1       ; FILL TARGET FLAG

BOOL            g_bHasRetried[MAX_COMPONENTS];                  // HASRETRIED1     DS      1       ; COMPONENT HAS RETRIED #1
BOOL            g_bHasRetriedStage2[MAX_COMPONENTS];    // HASRETRIED1ST2  (STAGE 2)
BOOL            g_bHasRetriedStage3[MAX_COMPONENTS];    // HASRETRIED1ST3  (STAGE 3)

int             g_nRetryCounterStage1[MAX_COMPONENTS];   // RETRYCTR1       DS      1       ; COMPONENT RETRY COUNTER.
int             g_nRetryCounterStage2[MAX_COMPONENTS];  // RETRYCTR1ST2   STAGE 2
int             g_nRetryCounterStage3[MAX_COMPONENTS];  // RETRYCTR1ST3  STAGE 3

// BOOL            g_bIgnoreCPI[MAX_COMPONENTS];                           // IGNORECPI1      DS      1        ; IGNORE CPI #1
BOOL            g_bRegrindFillProblem;                 // REGRINDFILLPROBLEM      DS      1 ; INDICATE PROBLEM ON REGRIND
BOOL            g_bNoFlowRateCalculation;              // NOFLOWRATECALCULATION       DS      1 ; NO FLOWRATE CALCULATION.
BOOL            g_bNoFlowRateCalculationArray[MAX_COMPONENTS];  // NOFLOWRATECALCULATION1-12 - no flow rate calculation for each component.
BOOL            g_bUnderFill;                                                                   // UNDERFILL       DS      1       ; UNDERFILL FLAG.

        structCPIRRData g_MainCPIRRData;                // main CPI Round Robin structure
        structCPIRRData g_Stage2CPIRRData;      // Stage 2 CPI Round Robin structure
        structCPIRRData g_Stage3CPIRRData;      // Stage 3 CPI Round Robin structure


int                             g_nStageFillEnTemp;                  // STAGEFILLENTEMP DS      1       ; STAGE FILL TEMP, USED IN CALIBRATION MENU
BOOL                            g_bLayeringTemp;                     // LAYERINGTEMP    DS      1       ; LAYERING TEMP LOC
BOOL                            g_bCopyCompFlowRate[MAX_COMPONENTS]; // COPYFLOWRATECMP1 DS      1       ;
// redefined.. long                            g_lCompCountTargetStore[MAX_COMPONENTS];// COPYFLOWRATECMP1   DS   1       ;
int                             g_nFillStage;                         // FILLSTAGE               DS      1       ; STAGE FILL
BOOL                            g_bMulFillStarted[MAX_COMPONENTS];    // MULFILL1STARTED DS      1       ; MULTIPLE FILL STARTED

// Sept 7th 04
int            g_nCheckLSensorInNSeconds;     // CHECKLSENSORINNSECONDS     DS      1       ; CHECK LLS
BOOL                g_bNoFillOnClean;                    // NOFILLONCLEAN           DS      1       ; NO FILL ON CLEAN
BOOL                g_bPhaseOneFinished;                 // PHASEONEFINISHED        DS      1       ; INDICATION WHEN PHASE 1 CLEAN IS FINISHED

int             g_nLeftCornerCleanOnCounter;     // CCLONCTR                DS      2       ; LEFT CC
int             g_nRightCornerCleanOnCounter;    // CCRONCTR                DS      2       ; Right CC
int             g_nAirJet2OnCounter;                 // AIRJET2ONCTR

BOOL                g_bHighLevelSensor;             // HIGHLEVELSENSOR DS      1       ; HIGH LEVEL SENSOR.
BOOL                g_bHighLevelSensorHistory;      // HIGHLEVELSENSORHIS DS      1       ; HIGH LEVEL SENSOR.
BOOL                g_bLowLevelSensor;              // LOWLEVELSENSOR  DS      1       ; LOW LEVEL SENSOR.
BOOL                g_bLowLevelSensorHistory;       // LOWLEVELSENSORHIS  DS      1       ; LOW LEVEL SENSOR.
int             g_nHighLSCoveredCounter;        // HIGHONCTR
int             g_nHighLSUnCoveredCounter;      // HIGHOFFCTR
int             g_nLowLSCoveredCounter;         // LOWONCTR
int             g_nLowLSUnCoveredCounter;       // LOWOFFCTR
BOOL                g_bOverrideClean;                   // OVERRIDECLEAN
BOOL                g_bSetCleanWhenCycleEnd;        // SETCLEANWHENCYCLEEND
int             g_nMixerOnCounter10Hz;          // MIXERONCTR10HZ ,,, 10 HZ COUNTER FOR MIXING

float               g_fAvgHopperWeight;             // weight return from CalculateHopperWeightAvg (asm = CALHWTAV)

// Sept 16th 04
long    g_lStoredComponentCountsStage1[MAX_COMPONENTS];  // CMP1CNTGSTR1    DS      3       ; ACTUAL COUNTS FOR STAGE 1 OF FILL
long    g_lStoredComponentCountsStage2[MAX_COMPONENTS];  // CMP1CNTGSTR2    DS      3       ; ACTUAL COUNTS FOR STAGE 2 OF FILL
int     g_nStageFillEnHistory;                  // STAGEFILLENH    DS      1       ;STAGE FILL ENABLE HISTORY

// Sept 24th
BOOL            g_bVenturiFillByWeight;         //VENTURIFILLBYWEIGHTH DS 1       ; HISTORY OF FBW
BOOL            g_bVenturiFirstTime;                //VENTURIFIRSTTIME DS     1       ; INDICATE THAT 1ST TIME USING VENTURI
// Sept 28th
int         g_nLoadConfigurationFailure;    // status of load configuration, bits set if problem

// OCT 7th
BOOL            g_bManualAutoFlag;              //MANATO manual or automatic
BOOL            g_bAutoTransition;              //Auto transition flag  AMANTR DS     1        ;~AUTO/MAN TRAN. FLAG
BOOL            g_bSaveAllCalibrationToEEprom;      //CEEFLG1/CEEFLG INDICATE CAL DAT STORE TO EEPROM
BOOL            g_bCalibrationDataChange;       // CALFLG
BOOL            g_bNewMaxScrewSpeed;                // NMAXSS
int         g_nDACPerKgDoneFlag;                // DACPKGDONEFLAG  DS      2       ; DAC PER KG DONE

int         g_nSaveRecipeSequence;          // sequence for ForegroundSaveRecipe..
unsigned int g_nSaveRecipeByteNo;                // Byte no or index for ForegroundRecipeSave.

BOOL            g_bL2Fitted;                     // SBB link options
BOOL            g_bL3Fitted;
BOOL            g_bSBBL4Fitted;
BOOL            g_bSBBL5Fitted;

// Nov 15th
BOOL            g_bNewSetpointsFlag;                // NEWSET
int         g_nLoadNewSetpointLag;          // LDNSPLAG
BOOL         g_bCCVsFlag;                        // CCVSFLG
int         g_nToStallTime;                 // TOSTALTIM
int         g_nExtruderStalledCounter;      // EXTSCTR
BOOL            g_bManualChangeFlag;                // MCHGFLG
unsigned int    g_nDA1Temp;                   // DA1TEMP
BOOL            g_bEnterKeyPressedFlag;         // EKPRSFLG

int             g_nCommsIncreaseFlag;           //COMMSIFLG
int             g_nCommsDecreaseFlag;           //COMMSDFLG
float           g_fExtruderRPM;                 // Extruder screw rpm (asm =SSRPM/SSRPM2D

BOOL            g_bByPassMode;                      // BYPASS
BOOL            g_bOverrideOutputs;             // OVERRIDEOUTPUTS         DS      1       ; OVER RIDE OUTPUTS
BOOL            g_bFeedValveClosed;             // FEEDCLOSE
BOOL            g_bCleanAtEndOfCycle;           // CLEANATENDOFCYCLE
BOOL            g_bPauseOnActive;                   // PAUSEONACTIVE    ; PAUSE ON FLAG ACTIVE
BOOL            g_bSecondRollover;              // SECROVER - set by RTC int.
BOOL            g_bForegroundSecondRollover;    // new, second roll over flag for foreground.
BOOL            g_bMixerTripped;                    // LEVSEN2STAT
int             g_nMixerTrippedCounter;         // new, counter for debouncing mixer tripped out.
int             g_nPauseOnActiveCounter;        // PONACTIVECTR
int             g_nPauseOnInactiveCounter;      // POFFACTIVECTR
BOOL            g_bExtruderControlStartUpFlag;  // STRUPF
BOOL            g_bExtruderControlStartUpTransition;    //  STUPTR DS     1
BOOL            g_bLowLevelSensorStatus[MAX_COMPONENTS];    // LS1OFF - LS12OFF

float           g_fDtoAPerMPM;                      // DACPMPM  D/A PER M/min
int             g_nSEIInputStates;              // SEIINPUTSSTATE  inputs on SEI
int             g_nSEIAlarms;                       // SEIALARMSTATE alarms on SEI

float           g_fActualGrammesPerM2;          // GPM2VAR  g/m2 actual value.

BOOL            g_bDiagnosticsPasswordInUse;    // ISDIAGNOSTICSPASSWORD
int         g_nDeviceToPollCounter;       // DEVICETOPOLLCTR
BOOL            g_bCheckIfShouldCycleFlag;  // CHKIFCYCLEFLAG    ; CHECK IF THE BLENDER SHOULD
int         g_nCheckIfShouldCycleCounter; // CHKIFCYCLECTR
BOOL            g_bShowInstantaneousWeight; // SHOWINWEIGHT - set flag to show intan
BOOL            g_bREadIncDecCalibrationData;   // READINCDECCAL - read calibration from SE
int         g_nDoIncDecReadInNSeconds;      // DOREADINNSECONDS
int         g_nIgnoreLSTime;                    // IGNORELSTIME,,
int         g_nBatchMonitorCounter;       // BATCHMONCTR     DS      2       ;
int         g_bRunLevelSensorCheck;       //  asm = CHECKFORMAXTHP
int         g_nBypassSensorUncoveredCounter;   // BYPASSSENSOROFFCTR
int         g_nPauseInNSeconds;             // PAUSEINNSECONDS - will start pause.
BOOL            g_bUpdateHiLoFlag;              //UPDATEREGHILOF          ; UPDATE REGHILO

BOOL            g_bSEIPollInputsFlag;           // SEIPOLLINPUTSF          ; START POLLING OF INPUTS
BOOL            g_bSEIPollLSFlag;                   // SEIPOLLLLSF             ; SET LOW LEVEL SENSOR
BOOL            g_bWriteAlarmsFlag;             // WRITEALARMSF
BOOL            g_bReadDAValueFlag;             // READDAVALUEF Trigger A/D read from SEI.
BOOL            g_bHourFlag;                        // HOURFG
int         g_nDoDiagnosticsCounter;        // DIAGCOUNTER
BOOL            g_bDoDiagnosticsFlag;           // no asm equiv.  Flag set to call DoDiagnostics in foreground.
int         g_nPITFractionCounter;          // PITFRACTCTR

int         g_nSeqTimeout;                      // SEQTIMEOUT
BOOL            g_bPeripheralCommsSTarted;  //COMMSST   comms started??
int         g_nLineSpeedFractionCounter;    // LSPDFRACTIONCTR
unsigned int         g_nTinySlideCycleCounter;       // TINYSLIDECYCLECOUNTER
int         g_nTinySlideOpenTimeCounter;    // TINYSLIDEOPENTIMECOUNTER
BOOL        g_bForegroundTenHzFlag;         // 10Hz flag for foreground. loosely equates to TENHZF1. int to allow
                                                        // foreground to cater for each 10Hz period.
int         g_nTenthsOfSecondCounter2;      // LSTTHS
BOOL            g_bSEIPollIncDecFlag;           // SEIPOLLINCDECF
int         g_nCMRValidTimer;                   // CMRVALID
BOOL            g_bRemoteKeyPoll;                   // REMKEYPOLL
int         g_nSecondCounter2;              // LSSECS
BOOL            g_bFGSecondFlag2;                   // flag to call One second program in foreground.
BOOL            g_bFGOneMinuteFlag;             // flag to call One second program in foreground.

int         g_nDisplayUpdateCounter;        // DISPLAYUPDATECTR
BOOL            g_bUpdateDisplayNow;                // UPDATEDISPLAYNOW
int         g_nMixerOffCounter;             // MIXEROFFCTR
int         g_nMixerOnTime;                 // MIXONTIME    - mixer on for N s

unsigned int g_nExtruderDAValue;             // DA1TMP.
int         g_nMixerTransitionCounter;      // MIXERTRANSITIONCTR

int         g_nSecondCounter;                   // SECCNT
BOOL            g_bTwoSecondFlag;                   // TWOSFLG  - indicate 2 seconds rollover
int         g_nTenSecondCounter;                // TSCCNT
BOOL            g_bTenSecondFlag;                   // TSCFLG            ;~10 SEC. FLAG

int         g_nFrontRollChangeLag;          // RCF_LAG  -Front reel change lag
int         g_nBackRollChangeLag;           // RCB_LAG  -Back reel change lag
int         g_nMixScrewPulseCounter;        //MIXSCREWPULSECTR   READ MIXING SCREW PULSE COUNTER
BOOL            g_bMixScrewRotationAlarm;       // MIXSCREWROTALARM

int         g_nRTCSecondCounter;            //
BOOL            g_bRTCMinuteFlag;               // set flag on Minute transition.
int         g_nRTCMinuteCounter;
BOOL            g_bOneSecondFlag;               // ONESECONDFLAG, set by RTC.
int         g_nKgHrSecondsCounter;          // KGHRCOUNT
int         g_nLevelSensorSimulator;        //LEVELSIM              ; INCREMENT LEVEL SENSOR SIMULATOR

BOOL            g_bAlarmRelayStatus;                // flag to show state of alarm relay
BOOL            g_bAlarmBeaconStatus;           // flag to show state of alarm beacon

// 2005
float           g_fKgHRRBuffer[MAX_KGH_RR_SAMPLES];     //kg/h round robin buffer.
unsigned int    g_nKgHRRIndex;                                  // index or pointer for kg/h r.r.
unsigned int         g_nKgHRRSummationCounter;           // summation counter for kg/h r.r.
float           g_fRRThroughputKgPerHour;           //RRTPTKGH
float           g_fRRThroughputLbPerHour;           //RRTPTLBH
float           g_fThroughputKgPerHour;             // TPTKGH
float           g_fThroughputLbPerHour;             // TPTLBH
float           g_fBatchKgPerHour;                  // BATCHKGPHR
float           g_fBatchTimeInSeconds;              // BATCHSECS
int         g_nPreviousPITTime;                 // PREVPITM

// 18.1.2005
BOOL            g_bRemoteOperatorPanelPresent;  //
BOOL            g_bSEIPresent;                          //
BOOL            g_bLLSPresent;                          //
int         g_nVramStartDisplayIndex;           //  Address in VRAM of first char to be sent to remote panel.
int         g_nVramEndDisplayIndex;             //

int         g_nDeviceListIndexUFP1;             // Device list index for UFP1
int         g_nDeviceListIndexUFP2;             // Device list index for UFP2
int         g_nDeviceListIndexSEI;              // Device list index for SEI
int         g_nDeviceListIndexLLS;              // Device list index for LLS
int         g_nDeviceListIndexLIW[MAX_LIW_CARDS];              // Device list index for LLS
int         g_nDeviceListIndexVAC8IO[MAX_VAC8IO_CARDS];  // Device list index for VAC8IO'S

int         g_nCommsUpdateStartIndex;           //
int         g_nStartAttributeUpdateIndex;       //
BOOL            g_bCommsUpdateActive;               //
BOOL            g_bCommsAttributeUpdateActive;  //

int         fdTelnet;                           // fd for telnet task - global.


