/////////////////////////////////////////////////
// P.Smith                          1/10/04
// Define variable for cleaning.
//
// P.Smith                          1/2/05
// Define variable used in control algorithm
//
// P.Smith                          1-04-2005
// added g_nHiLowOptionSwitchedOnOff,g_nHiLowOptionSwitchedOnOffHistory
//
// P.Smith                          4/7/05
// Added reporting variables
//
// P.Smith                          22/08/2005
// Added g_nExtruderNo,g_nCurrentExtruderNo
//
// P.Smith                          29/08/2005
// Added g_bInCalibrationMenu
//
//
// P.Smith                          5/9/2005
// moved g_fDtoAPerKilo,g_fDtoAPerMeterPerMinute to uncleared area
//
//
// P.Smith                          12/9/2005
// added g_nLowLevelSensorAlarmWord;
// defined g_bLowLevelSensorOff,g_bLowLevelSensorOnTransition,g_bLowLevelSensorOffHistory
//
//
// P.Smith                          28/9/2005
// added g_bPrintHourlyReport,g_bPrintOrderReport,g_bPrintFrontRollReport,g_bPrintBackRollReport
// addec g_unCalibrationCheckSum

//
// P.Smith                          5/10/05
// added g_bDoCMRDToAPerKgCalculation
//
// P.Smith                          7/10/05
// added g_bPotControl;
//
// P.Smith                          11/10/05
// added g_fPositiveControlErrorPercentage;

// P.Smith                          20/10/05
// rename  g_bAllowLargeAlarmBand -> g_nAllowLargeAlarmBand

// P.Smith                          11/11/05
// added  g_nDiagnosticArrayIndex,g_nSingleCycleCompNoHis;

// P.Smith                          15/11/05
// added g_nReMixOnTime, g_bBinLevelSensorHistory
//
// P.Smith                          24/11/05
// changed g_nExtruderTachoReadingAverage & g_nExtruderTachoReading to unsigned ints
// unsigned int g_nTempExtruderDAValue,g_nDA1Temp, REMOVE g_bWriteDAValueFlag
//
// P,Smith                          20/12/05
// added  g_bIsSystemAlarm
//
// P,Smith                          3/1/06
// added g_nStageFillEnHistory
//
// P,Smith                          5/1/06
// correct g_nStageFillEnHistory to g_nStageFillEnableTransitionHistory
//
// P,Smith                          25/5/06
// added conversion variables
//
// P,Smith                          30/5/06
// added BOOL        g_bProgramLoopRunnning;                                           //
//
// P.Smith                          14/6/06
// unsigned int g_nVavleLatencyTime; added
//
// P.Smith                          15/6/06
// added g_arrcEmailBuffer[EMAIL_BUFFER_SIZE] definition
//
// P.Smith                          22/6/06
// added g_fComponentActualWeightTopUpBackup;
//
// P.Smith                          10/7/06
// added g_nBatchesInKghAvgHistory,g_bLayeringHistory;
// g_nVavleCalculatedLatencyTime;
// g_nBatchesInKghAvgHistory;
// g_bLayeringHistory;
// g_bSPIAtDActive;
// g_bSPIEEActive;
// g_nLevelSensorAlarmCounter;
// g_bHighLoRegrindOptionHistory;
//
// P.Smith                          14/9/06
// added g_bNBBRevB1Hardware;                                                // rev B1 hardware
//
// P.Smith                          20/9/06
// added BOOL     g_bNBBRevB1Hardware;                                                // rev B1 hardware
//
// P.Smith                          21/9/06
// added g_nTestBuffer[];,fd0, g_bTSMCall,g_bPrintfRunning,g_nNoPanelReply;
//
// P.Smith                          15/11/06
// g_bModbusLineSpeedWritten,g_fCommunicationsLineSpeed,g_fCommsLineSpeedFrontLength,g_fCommsLineSpeedBackLength
// added g_bModbusDToAWritten,g_bModbusDToAPercentageWritten;
//
// P.Smith                          4/12/06
// change g_nCycleIndicate to a BOOL g_bCycleIndicate
// remove g_fDtoAPerKGH
// g_nAlarmOnFlag -> BOOL g_bAlarmOnFlag, g_nAlarmOnTransitionFlag -> BOOL g_bAlarmOnTransitionFlag
// g_nEEPROMWriteTimeoutCounter, g_nEEPROMReadTimeoutCounter
// g_bModbusDToAWritten, g_bModbusLineSpeedWritten & g_bModbusDToAPercentageWritten changed to a BOOL
//
// P.Smith                          18/1/07
// Name change g_bTSMCall -> g_bEnableTxRxInOpenSerial
//
// P.Smith                          5/2/07
// g_nSPIEEActiveTimeoutCounter added
//
// P.Smith                          5/2/07
// g_nRevertToOperatorPanelCounter
// change g_nPeripheralCardsPresentHistory to unsigned int.
// added g_nSPIEEActiveTimeoutCounter,g_nRevertToOperatorPanelCounter,g_bRestartPanelNow;
// g_bRedirectStdioToPort2,g_nPulseOnTime,g_bPulsed,g_cBatSeq_history;
//
// P.Smith                          2/3/07
// define g_nResetAllFlowRates
//
// P.Smith                          12/3/07
// defined ram for ultrasonic reading.
//
// P.Smith                          13/3/07
// added more ram for ultrasonic
//
// P.Smith                          30/3/07
// added g_bUpdateSDCardSoftware,g_bSoftwareUpdateRunning,g_bSDCardSPIActive;
//
// P.Smith                          25/4/07
// Added variables required for queueing of Multiblend recipes.
//
// P.Smith                          2/5/07
// define g_BlenderMapping
//
// P.Smith                          17/5/07
// added g_bFlowRateChange
//
// P.Smith                          30/5/07
// added g_nMultiBlendRecipeSaveCtr;
//
// P.Smith                          11/6/07
// added g_bDHCP
//
// P.Smith                          12/6/07
// added g_bOptimisationPresent,g_nDeviceListIndexOptimisation
//
// P.Smith                          13/6/07
// added g_bOptimisationFlapStatus,g_bOptimisationLoaderEnable & g_bOptimisationLoaderFill
// g_nuOptimisationSensorInputState & g_unOptimisationLoaderOuputs
//
// P.Smith                          20/6/07
// added g_bSaveAllCalibrationToEEpromOutsideCycle
//
// P.Smith                          22/6/07
// g_bStopSPIInterruptRunning removed
//
// P.Smith                          25/6/07
// added g_bCopyConfigFileToSDCard, g_bCopyConfigFileToBackupFile,g_bLoadDefaultConfigFromSDCard;
// g_bLoadBackupConfigFromSDCard
//
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
// Added g_bMultiBlendRecipeIsZero for
//
// M.McKiernan                      17/9/07
// Added g_arrnMBTable[] & g_arrnMBTableFlags[]
//       g_nMBWriteOrFlag, g_nMBWriteOrErrorFlag, g_nMBWriteOrFlagTemp
//
// P.Smith                      23/10/07
//  added g_cMultiblendDiagnosticsBuffer, g_bWriteToIniFileAgain,g_nOverrideCounter
//
//
// P.Smith                      24/10/07
// added g_nAllowReadOfControlInputsCtr
//
// P.Smith                      9/11/07
// added varaible to diagnose serial open / close commands.
// added g_bStartingMultiBlendRecipe
//
// P.Smith                      15/11/07
// g_bUsingSmallBatchSize defined.
//
// P.Smith                      19/11/07
// added  g_bFirstTimeToReadScrewSpeed,g_fExtruderRPMHistory,g_bCalculateScrewSpeedConstant
// g_fScrewSpeedCalibrate,g_nVolumetricEstimateCounter;
//
// P.Smith                      21/11/07
// g_bCheckOptimisationAgain[MAX_COMPONENTS,g_bFirstOptimisationCheck
//
// P.Smith                      23/11/07
// added g_bModbusScrewSpeedWritten
//
// P.Smith                      5/12/07
// added g_nHoldOffLevelSensorAlarmSecondCounter, g_nSuspendControlCounter
//
// P.Smith                      6/12/07
// added definitions for round robin averaging.
//
// P.Smith                      9/1/08
// added g_bResetComponentWeightsRoundRobin
//
// P.Smith                      10/1/08
// added g_nAutocycleLevelSensorOff10hzCounter
//
// P.Smith                      15/1/08
// added g_bSHAHardwareCheckFailed
//
// P.Smith                      17/1/08
// added g_bSaveUSBFileToUSB, g_byPrinterBuffer[PRINT_BUFFER_SIZE],g_bUSBDataWrite
//
// P.Smith                                                  22/1/08
// added g_bComponentHasFilledByWeight
//
// P.Smith                                                  28/1/08
// added variables for batch statistics
//
// P.Smith                          30/1/08
// correct compiler warnings
//
// P.Smith                          1/2/08
// added g_arrnMBTableSetpointCopy, g_nWritenwrdCounter, g_bPanelWriteHasHappened
//
//
// P.Smith                          11/2/08
// added g_bSEIReadDataReady, g_nExtruderVolumetricDAValue,g_nCheckForCorrectDownloadCtr;
//
// P.Smith                          12/2/08
// added g_cRead_buf, use USB_PRINT_BUFFER_SIZE as usb buffer size
//
// P.Smith                          25/2/08
// added g_nExtruderControlSetpoint
//
// P.Smith                          7/3/08
// added g_nSEIWriteRequiredCounter
//
// P.Smith                          10/3/08
// added g_bSEIIncDecSetpointDownload
//
// P.Smith                          26/3/08
// added unsigned int    g_bSEIIncDecSetpointDownload;
// BOOL    g_bAllowSaveToEEPROM;
// unsigned int    g_nSaveToEEPROMCounter;
//
// P.Smith                          28/3/08
//g_bAllowProcesDataSaveToEEPROM;
//g_nSaveProcessDataToEEPROMCounter;
//RepeatedDataStruct g_ProcessData;
//g_bSaveAllProcessDataToEEprom;
//g_bSaveAllProcessDataOutsideCycle;
//
// P.Smith                          23/4/08
// added g_nCycleCounter
//
// P.Smith                          30/4/08
// changed g_bSEIIncDecSetpointDownload to a BOOL
// added g_nCurrentExtruderNo not in calibration data anymore
// added g_arrnMBSetpointHistory[MB_TABLE_SETPOINT_SIZE];
//
// P.Smith                          17/5/08
// added g_bAllowMultiBlendCycle  kwh
//
// P.Smith                          6/6/08
// added sd card complete counters
//
// P.Smith                          11/6/08
// added g_nDownloadManagerNZTimeoutCtr
//
// P.Smith                          12/6/08
// added g_nDisableDTAPerKgCtr
//
// P.Smith                          23/6/08
// g_bProcessDataInvalid, g_bTelnetEnable, g_bTelnetHistory,g_nBatchCommsStatusSEI
// g_nBatchCommsStatusLLS,g_nBatchCommsStatusOptimisation,g_nBatchCommsStatusNetwork;
// g_nBatchCommsStatusTCPIP,g_nBatchCommsSEITimeoutHistory,g_nBatchCommsLLSTimeoutHistory;         // comms timeout history
// g_nBatchCommsOptimisationTimeoutHistory,g_nBatchCommsNetworkGoodTxHistory;
// g_nBatchCommsTCPIPGoodTxHistory,g_nTCPIPReadHoldRegsCtr;
//
// P.Smith                          17/7/08
// add variables for event logging, operator panel id,blender type (batch liw)
// g_nSDCardDelayTime delay before sd card write, this allows the panel comms to
// finish.
//
// P.Smith                          23/7/08
// added g_CurrentTime,g_cPowerDownTime,g_PowerUpTime, all modbus arrays
//
// P.Smith                          24/7/08
// define g_nDisableSDCardAccessCtr &g_nAMToggleStatusDelayCtr;
//
// P.Smith                          11/9/08
// added g_bWriteEventLogToSdCard, g_cEventLogFile[1000], g_cEventName[20];
//
// P.Smith                          17/9/08
// added g_nLiquidAdditiveComponent
//
// P.Smith                          26/9/08
// added all liquid additive variables,d/a written for comms, weigh hopper open
// variables,temperature variable and alarm history arrays.
//
// P.Smith                          3/10/08
// added g_fOneSecondHopperWeightAtEndOfCycle
//
// P.Smith                          10/10/08
// add g_fKgperHour
//
// P.Smith                          16/10/08
// make g_nBatchCommsSEITimeoutHistory etc longs
// added g_fLongTermLengthCounter, g_fLongTermResettableLengthCounter;
//
// P.Smith                          17/11/08
// added unsigned int    g_nTelnetSequence,g_bTSMTelnetInSession,g_bTelnetUserNameOkay;
// g_bShowNetworkWrites,g_bWriteDiagnosticsToSDCard,g_bSDCardAccessInProgress,g_nTelNetTimeoutCtr;
// g_nTelnetContinuousUpdate,g_nFileSegmentNo,g_cTelnetFileName,g_bPrintCycleDataToTelnet;
// g_nTelNetSecondsCounter,g_lTCPIPCommsCounter,g_bTelNetWindowCtr;
//
// P.Smith                          18/11/08
// added  EVENTLOGSTRINGLENGTH for  g_cEventLogFile
//
// P.Smith                          21/11/08
// added g_cOrderReportStorageBuffer with size ORDER_REPORT_BUFFER_SIZE -2000
//
// P.Smith                          26/11/08
// define g_bAToDMaxExceeded
//
// P.Smith                          2/12/08
// added std dev percentage
// added g_bApplicationSPIInProgress, g_bTaskSPIInProgress, g_bSDCardAccessInProgress;
//
// P.Smith                          8/12/08
// added structures for ongoing and variable history data.
//
// P.Smith                          11/12/08
// name change g_bTelnetSDCardAccessInProgress & g_OrderReportName, g_cEventLogFileCopy added
//
// P.Smith                          5/1/09
// added g_bUSBDebug
//
// P.Smith                          15/1/09
// added usb timeout variables,add g_cEventLogFileCopy
//
// P.Smith                          7/1/09
// added g_bHiRegLevelSensor,g_nHiRegLSCoveredCounterg_nHiRegLSUnCoveredCounter;
//
// P.Smith                          21/1/09
// added g_nFunctionRunning, g_nFunctionSubSectionRunning,g_bUSBDebug,g_bResetNetworkSlaveComms;
// g_bResetPanelSlaveComms,g_bAddAlarmOccurredToEventLog,g_bAddAlarmClearedToEventLog;
// g_ucAlarmCode, g_nAlarmComponent;
//
// P.Smith                          4/2/09
// added g_bNoParityHistory
//
// P.Smith                          11/2/09
// added timeout counters for usb
//
// P.Smith                          26/2/09
// added  g_sSelfTest, g_nSPISelect
//
// P.Smith                          31/3/09
// added g_bDoSelfTestDiagnostics,g_s24HrOngoingHistoryEventsLogData,g_s24HrOngoingHistoryEventsLogData
// added g_bInLastMinute, g_wHourHistory, g_bHourRollOver
// g_cPanelVersionNumber[PANEL_REVISION_STRING_SIZE], DescriptionStruct g_sDescriptionData;
// unsigned int    g_nStorageInvervalCtr;
//
// P.Smith                          24/4/09
//add g_lOrigBatchPulsesAccumulator,g_cSEISoftwareVersionNumber, g_bUseLineSpeedForGPMCalc;
//
// P.Smith                          18/5/09
// added  g_bNoLeakAlarmCheck,g_bHiRegLevelSensorHistory,g_bDumpFlapHasOpened;
//
// P.Smith                          25/5/09
// added g_bI2CError
//
// P.Smith                          17/6/09
//g_nblenderpulsetestctr & g_bPulseValveNow;
//
// M.McKiernan                07/05/2008
// Added g_bLIWReadDataAvailable[MAX_LIW_CARDS] and g_bLIWReadDataReady[MAX_LIW_CARDS];
//
// P.Smith                          30/6/09
// added unsigned int    g_bPulseTime,g_wForeGroundCounter,g_wProgramLoopEntry;
// g_cModbusPrintBuffer,g_bPrintModbusMessageToTelnet
//
// P.Smith                          2/7/09
// added g_nCommsVacuumLoadingTimeoutHistory,g_nBatchCommsStatusVacuumLoading
//
// P.Smith                          22/7/09
// added g_nPulsingSequence[MAX_COMPONENTS],g_lComponentTargetCountsCopy[MAX_COMPONENTS];
// g_fComponentTargetWeightCopy,g_bFastSettling,g_nComponentPulseCounter[MAX_COMPONENTS];
// g_fComponentActualWeightHistory,g_bFirstPulse,g_bCalibratePulsing,g_nPulseTime;
// g_bICSRecipeDataWritten,g_bSaveAdditionalDataToEEPROM;
//
// P.Smith                          25/8/09
// added   g_nValidFlowRateExceededCtr,g_bAcceptFlowRate[MAX_COMPONENTS],g_bLockIsEnabled
//
// P.Smith                          26/8/09
// added g_bPitRunning
//
// P.Smith                          1/9/09
// added g_cModbusEventBuffer, g_bAddEventToEventLog, g_nHoldOffEventLogWriteCtr;
//
// P.Smith                          2/9/09
// added g_bAtoDEndOfConversionErrorHistory, g_bAtoDNegativeSignHistory,g_nWaitForSPIBusToSettleCtr;
// g_bDoAToDTest;
//
// P.Smith                          10/9/09
// added  g_nBlenderTypeHistory,g_bSelfTestInProgress,g_fCalibrationWeight;
//
// P.Smith                          15/9/09
// make g_bCalibratePulsing multi component
// added g_nPulseTimeForMinWeight,g_nFillingMethodHistory,g_fPulseWeight,g_bGotHalfGramTime
// g_nHalfGramTime,g_fRequiredPulseWeightInGrams
//
//
// P.Smith                          15/9/09
// make g_nPulseTimeForMinWeight,g_nHalfGramTime,g_fRequiredPulseWeightInGrams
// component related.
//
// P.Smith                          12/10/09
// added g_fMaxKgPHrPossible,g_nAfterEndOfCycleCounter;
//
// P.Smith                          15/10/09
// added g_bVAC8ExpansionRequired,g_nVAC8ExpansionIndex,g_nVAC8ExpansionOutput,g_nVaccumLoadingVAC8IOCards;
//
// P.Smith                          16/11/09
// added variables for stability check.
//
// P.Smith                          19/11/09
// defined g_nAToDRefStabilityLog,g_bTakeRefAToDReading,g_nDelayBeforeRefLogCtr,g_nAToDRefStabilityCtr
// g_nAToDRefStabilityMaxValue,g_nAToDRefStabilityMinValue,g_bRefAToDStabilityTestInProgress
// g_bDecideOnRefStability,g_bDecideOnLoadCellStability,g_nVac8ExpansionHistory,g_nRefTimeoutCtr
//
// P.Smith                          27/11/09
// added g_nTopUpCompNoRefHistory
//
// P.Smith                          14/11/09
// change all stability logs to longs to remove compiler warnings.
//
// P.Smith                          18/1/10
// added  g_fWeightLeftAfterRegrind, g_fRegrindWeight;
//
// P.Smith                          11/1/10
//added ram to for sd card error handling.
//
// P.Smith                          11/1/10
// added g_bFileCouldNotBeOpenedEvent
//
// P.Smith                          1/2/10
// added g_bComponentHasRetried
//
// P.Smith                          8/2/10
// removed g_cMultiblendDiagnosticsBuffer
////////////////////////////////////////////////////////////////

//*****************************************************************************
// MACROS
//*****************************************************************************

//#include "Hardware.h"
#include "General.h"
#include "BatchMBIndices.h"
#include "PrinterFunctions.h"
#include <effs_fat/fat.h>
#include "BatchCalibrationFunctions.h"

//#include "BatVars.h"
//#include "BatVars2.h"
//#include        "RRFormat.h"
//#include        "Blrep.h"
#include <basictypes.h>
#include "TSMPeripheralsMBIndices.h"
#include "Historylog.h"
#include "Debug.h"
#include "BlenderSelfTest.h"
#include "MBProSum.h"


int         g_nExpansionOutput;                                                 //EXPANOUTPUT               DS      1       ; EXPANSION O/P
BOOL        g_bBlenderTopCleanOn;                                               //AIRJET1ONF                DS      1       ; AIR JET ONE ON.
BOOL        g_bBlenderMixerCleanOn;                                             //AIRJET2ONF                DS      1       ; AIR JET TWO ON.
BOOL        g_bRightCornerCleanOn;                                              //CORNERCLEANRONF           DS      1       ; CORNER CLEAN
BOOL        g_bLeftCornerCleanOn;                                               //CORNERCLEANONF            DS      1       ; CORNER CLEAN ON
BOOL        g_bOverRideClean;                                                   //OVERRIDECLEAN             DS      1       ; OVER RIDE CLEAN
BOOL        g_bExpanioFitted;                                                   // EXTERNALCLEANPCB         DS      1       ; DETERMINES IS EXTERNAL CLEAN PCB SHOULD BE WRITTEN TO.
BOOL        g_bBypassStatus;                                                    // BYPASSSTATUS             DS      1       ; BYPASS STATUS.
BOOL        g_nAfterStartUpCtr;                                                 // AFTERSTARTUPCTR          DS      1           ; AFTER START UP COUNTER.
BOOL        g_nCorrectEveryNCycleCtr;                                           //  CORRECTEVERYNCYCLECTR   DS      1           ; CYCLE COUNTER
BOOL        g_nControlErrorCounter;                                             // CERCT1                   DS      1        ;CONTROL ERROR COUNTER #1
float       g_fControlErrorPercentage;                                          // Control % error.
float       g_fPositiveControlErrorPercentage;                                  //absolute Control % error.
unsigned int         g_nTempExtruderDAValue;                                             // temp d/a storage
unsigned int g_nExtruderTachoReadingAverage;                                 // SEIADREADINGAVG DS      2       ; VOLTAGE FOLLOWER READING.
unsigned int g_nExtruderTachoReading;                                        // SEIADREADING    DS      2       ; VOLTAGE FOLLOWER READING.
BOOL        g_bScrewIncreaseDeceasePressed;                                     // SCREWINCDECPR   DS      1       ; SCREW INC/DEC PRESSED FLAG.
float       g_fLineSpeed5sec;                                                   // LSPD5S          DS     5        ;~5 SEC LINE SPEED
float       g_fWeightPerMeter;                                                  // WTPMVL5         DS      5       ; WT/M
float       g_fWeightPerMeterRR;                                                // RRGMPM          DS      3       ; G/M ROUND ROBIN.
int         g_nEstimationContinueCounter;                                       //CONESTIMATION    DS      1       ; CONTINUE ESTIMATION.
float       g_fExtruderDAValuePercentage;                                       // CH1CV           DS     2        ;~CONTROL VOLTS VALUES (0-999)
unsigned int g_nSEIDToAReadState;                                                // SEIDARDSTATE    DS      2       ; SEI D/A READ DATA.
long        g_lSEIPulses;                                                       // SEIPULSEACC     DS      4       ; SEI PULSE READING READ BACK.
int         g_nSEIAlarmState;                                                   // SEIALARMSTATE   DS      2       ; ALARM STATE READ.
int         g_nSEIDToAOutputState;                                              // SEIDAOPSTATE    DS      2       ; SEI DA OP STATE.
int         g_nSEIInputState;                                                   // SEIINPUTSTATE   DS      2       ; READ INPUTS STATE
int         g_nSEIPulsesPerSecond;                                              // SEIPULSEPSEC    DS      2       ; SEI PULSES / SECOND
int         g_nVoltageFollowerReading;                                          // VOLFOLREADING   DS      2       ; VOLTAGE FOLLOWER READING
BOOL        g_bSEIPulsesReady;                                                  // SEIPULSESREADY  DS      1       ; INDICATES THAT PULSE CALCULATION SHOULD BE DONE.
//--removed-- int         g_nLineSpeedFractionCounter;                                        // LSPDFRACTIONCTR DS      2       ; FRACTION COUNTER
int         g_nSEIIncreaseDecreaseState;                                        // SEIINCDECSTATE  DS      2       ; SEI INC/DEC STATE
BOOL        g_bLengthCalculationInProgress;                                     // LENGTHCALCINP   DS      1       ; LENGTH CACULATION IN PROGRESS
BOOL        g_bSEIFirstPulseRead;                                               // SEIFIRSTPULSER  DS      1       ; SEI FIRST PULSE READING.
long        g_lSEICurrentPulses;                                                // CURRENTPCLK     DS      4       ; CURRENT PCLK VALUE.
long        g_lSEIPreviousPulses;                                                //PREVIOUSPCLK    DS      4       ; PREVIOUS PCLK VALUE.
long        g_lDifferenceInPulses;                                              // DIFFINPCLK      DS      4       ; DIFFERENCE IN PCLK.
long        g_lBatchPulsesAccumulator;                                          // BATCHPULSEACC   DS      3       ; BATCH PULSE ACCUMULATOR
long        g_lFrontRollPulsesAccumulator;                                      //LSPDPULACC5SECF DS      3        ; FRONT ROLL PULSE ACC
long        g_lBackRollPulsesAccumulator;                                       //LSPDPULACC5SECB DS      3        ; BACK ROLL PULSE ACC
long        g_lHourlyPulsesAccumulator;                                         //HOURLYPULACC    DS      3        ; HOURLY PULSE ACC
unsigned int g_nLineSpeedPulse5SecondCounter;                                    //LSPDPULCTR5SEC  DS      1        ;~6840 CTR 3 READ NO. (0-4)
long        g_lLineSpeedPulse5SecondAccumulator;                                //LSPDPULACC5SEC  DS      3        ;~6840 CTR 3 SUM (5 SECS)
long        g_lLineSpeedPulse5SecondTotal;                                      //LSPDPULTOT5SEC  DS      3        ;~CTR 3 PULSES FOR 5 SECS
long        g_bLineSpeedUpdate;                                                 //LS5SFG DS     1        ;~LINE SPEED 5 SECS FLAG, SET WHEN 5 SEC DATA AV
BOOL        g_bSEIReadDataAvailable;                                            // new variable
int         g_nZeroLineSpeedErrorCounter;                                       //LSPEEDERRORCTR              DS      1           ; LINE SPEED ERROR COUNTER.
float       g_fLineSpeed5secHistory;                                            //WMLS5S          DS     5        ;~5 SEC LINE SPEED USED TO CALC. WT/M
BOOL        g_bLineSpeedHasChanged;                                             //LS5CFG DS     1        ;~LINE SPEED 5 SEC CHANGE FLAG
int         g_nLineSpeedChangeCounter;                                          //LSPEEDCHANGECTR DS      1       ; LINE SPEED COUNTER
float       g_fBatchLength;                                                     //BATCHLENGTH     DS      5       ; BATCH LENGTH STORAGE
int         g_nEstimatedAddedPulses;                                            //ESTADDSEIPULSES DS      2       ; ADDITIONAL PULSES IN SECOND.
int         g_nEstimatedSubtractedPulses;                                       //ESTSUBSEIPULSES DS      2       ; SEIPULSEPSEC - ADDITIONAL PULSES.
float       g_fLbsPer1000Feet;                                                  //ACTLBKF         DS    3         ; ACT LB/1000'
float       g_fGPMRRBuffer[MAX_GPM_RR_SAMPLES];                                 //g/m round robin buffer.
unsigned int g_nGPMRRIndex;                                                      // index or pointer for g/m r.r.
unsigned int g_nGPMRRSummationCounter;                                           // summation counter for g/m r.r.
int         g_nIncreaseLampStatus;                                              // INCLAMPSTATUS   DS      1
int         g_nDecreaseLampStatus;                                              // DECLAMPSTATUS   DS      1       ; DEC LAMP STATUS.
int         g_nSEITimeOutCounter;                                               // SEITIMEOUTCTR   DS      1       ; SEI TIMEOUT COUNTER.
BOOL        g_bSEIHasTimedOut;                                                  // SEITIMEOUTFLAG  DS      1       ; SEI TIMEOUT FLAG.
BOOL        g_bPrintSEIResetOccurred;                                           // PRINTTOMANUAL   DS      1
BOOL        g_bAlarmOnFlag;                                                     // ALARMONFLAG     DS      1       ; INDICATE THAT ALARM IS ON.
BOOL        g_bAlarmOnTransitionFlag;                                           // ALARMONTRFLAG   DS      1       ; ALARM TRANSITION HAS OCCURRED.
float       g_fComponentFrontWeightAccumulator[MAX_COMPONENTS];                 // CH1WACCF DS    5         ; weight accumulator,  (front roll)
float       g_fComponentBackWeightAccumulator[MAX_COMPONENTS];                  // CH1WACCB DS    5         ; weight accumulator,  (back roll)
float       g_fComponentOrderWeightAccumulator[MAX_COMPONENTS];                 // CH1WACCO DS    5         ; weight accumulator,  (order)
float       g_fComponentShiftWeightAccumulator[MAX_COMPONENTS];                 // CH1WACCS DS    5         ; weight accumulator, XX XX XX .YY YY KGS
float       g_fComponentShiftWeightOngoingAccumulator[MAX_COMPONENTS];          // CH1WACCS9 DS    5         ; weight accumulator, XX XX XX .YY YY KGS
float       g_fComponentHourlyWeightAccumulator[MAX_COMPONENTS];                // CH1WACCH DS    5         ; weight accumulator, XX XX XX .YY YY KGS
float       g_fComponentLongTermWeightAccumulator[MAX_COMPONENTS];              // CH2WACCM DS    5         ; Monthly
float       g_fComponentLongTermWeightResettableAccumulator[MAX_COMPONENTS];              // CH2WACCM DS    5         ; Monthly
float       g_fComponentBatchWeightAccumulator[MAX_COMPONENTS];                 // CH1WACCBAT  DS    5             ; weight accumulator, XX XX XX .YY YY KGS
float       g_fComponentBatchWeightAccumulatorPercentage[MAX_COMPONENTS];       //
float       g_fTotalOrderWeightAccumulator;                                     // TOTWACCO DS    5         ; TOTAL wt. acc.
float       g_fTotalFrontWeightAccumulator;                                     // TOTWACCF DS    5         ; TOTAL wt. acc.
float       g_fTotalBackWeightAccumulator;                                      // TOTWACCB DS    5         ; TOTAL wt. acc.
float       g_fTotalShiftWeightAccumulator;                                     // CURSHIFTWEIGHT  DS      5       ; CURRENT SHIFT WEIGHT.
float       g_fTotalShiftWeightOngoingAccumulator;                              // TOTWACCS9 DS    5        ; TOTAL WEIGHT ACCUMULATE FOR SHIFT.
float       g_fTotalLongTermWeightAccumulator;                                  // TOTWACCM DS    5         ; TOTAL wt. acc. XX XX XX .YY YY KG
float       g_fTotalLongTermWeightResettableAccumulator;
float       g_fTotalHourlyWeightAccumulator;                                    // TOTWACCH DS    5         ; TOTAL wt. acc. XX XX XX .YY YY KG
float       g_fTotalBatchWeightAccumulator;                                     // TOTWACCBAT  DS    5             ; TOTAL WEIGHT ACCUMULATE FOR BATCH IN OFFLINE MODE.
float       g_fTotalHistoryBatchWeightAccumulatorImp;                           // TOTWACCBATHI    DS      5        ; TOTAL WEIGHT ACCUMULATE FOR OFFLINE MODE FOR IMPERIAL MODE.
float       g_fTotalHistoryBatchWeightAccumulator;                              // TOTWACCBATH     DS      5        ; TOTAL WEIGHT ACCUMULATE FOR OFFLINE MODE.
float       g_fTotalBatchWeightAccumulatorImp;                                  // TOTWACCBATI DS    5             ; TOTAL WEIGHT ACCUMULATE FOR BATCH IN OFFLINE MODE.
int         g_nStopInNBatches;                                                  //STOPINNBATCHES  DS      1
float       g_fTotalBatchWeightAccumationLeft;                                  //TOTWACCBATLEFT DS       5       ;
float       g_fBatchSizeStorage;                                                //DFTW1STORE      DS      2
BOOL        g_bNoTargetAlarm;                                                   //NOTARGETALARM   DS      1       ; NO TARGET ALARM FLAG
BOOL        g_bBatchIsReady;                                                    //BATCHREADY      DS      1       ; BATCH READY
BOOL        g_bMixerSensorHistory;                                              //MIXSENSORHISTORY DS     1
int         g_nAToDSumCounter;                                                  //SAMPLEAD1       DS      1       ; SAMPLE COUNTER
long        g_lAToDSum;                                                         //SUMAD1          DS      3       ; SUMMATION
float       g_fLineSpeed5secFtPMinute;                                          //LSPD5SFM        DS    3         ; LINE SPEED
float       g_fTotalThroughputSetLbsPerHour;                                    //SETTLBH DS     3         ;~TOTAL THRU-PUT, SUM OF X BLENDS
float       g_fTotalThroughputSetLbsPer1000Feet;                                //SETLBKF  DS    4        ; SET LB/1000'
float       g_fTotalfDesiredWidthinInches;                                      //WIDTHSPIN DS   3         ; WIDTH SP IN INCHES (2DP)
int         g_nHiLowOptionSwitchedOnOff;                                        //HILOWOPTIONSWITCHEDONOFF    DS       1       ; HI LOW REG OPTION SWITCHED OFF.
int         g_nHiLowOptionSwitchedOnOffHistory;                                 //HILOWOPTIONSWITCHEDONOFFHIS DS       1       ; HI LOW REG OPTION SWITCHED OFF.
BOOL        g_bPauseOffActive;                                                  //PAUSEOFFACTIVE  DS      1       ; PAUSE OFF INPUT RAM DEFINED
int         g_nPauseOffActiveCounter;                                           //POACTIVECTR     DS      1
BOOL        g_bActivateFrontRollChange;                                         //RCFFLG  DS    1        ;~FRONT REEL CHG FLAG
BOOL        g_bActivateBackRollChange;                                          //RCBFLG  DS    1        ;~BACK REEL CHG FLAG
BOOL        g_bReadDToFromSEIOnReset;                                           //read d/a on reset
float       g_fUsedBatchWeight;                                                 //BATCHUSEDWT     DS      3       ; LAST ROLL COMPSENSATED WEIGHT
float       g_fUnUsedBatchWeight;                                               //BATCHUNUSEDWT   DS      3       ; LAST ROLL COMPSENSATED WEIGHT
// --removed-- float       g_fComponentOrderWeightAccumulator[MAX_COMPONENTS];                 // CH1WACCO DS    5         ; weight accumulator,  (order)
float       g_fComponentActualUnusedWeight[MAX_COMPONENTS];                     // CMP1ACTWCM      DS      3       ; COMPONENT #N ACTUAL WEIGHT
// --removed-- float       g_fComponentLongTermWeightAccumulator[MAX_COMPONENTS];              // CH1WACCM DS    5         ; weight accumulator, XX XX XX .YY YY KGS
float       g_fComponentOngoingOrderPercentage[MAX_COMPONENTS];                 // ORBPCNT1 DS      3       ; ORDER %
float       g_fComponentFrontRollPercentage[MAX_COMPONENTS];                    // CH1PERCFR DS    3         ; CALCULATED % FOR COMPONENTS.
float       g_fComponentBackRollPercentage[MAX_COMPONENTS];                     // CH1PERCBR DS    3         ; CALCULATED % FOR COMPONENTS.
float       g_fComponentOrderPercentage[MAX_COMPONENTS];                        // CH1PERCO        DS    3         ; CALCULATED % FOR COMPONENTS.
float       g_fComponentShiftPercentage[MAX_COMPONENTS];                        // CH1PERCS DS    3         ;
float       g_fComponentHourlyPercentage[MAX_COMPONENTS];                       // CH1PERCH DS    3         ;
float       g_fComponentLongTermPercentage[MAX_COMPONENTS];                     // CH1PERCM DS    3         ; CALCULATED % FOR COMPONENTS.
long        g_lFrontTotalLengthPulses;                                          // FLSPREG DS    5        ;~LINE SPEED (FRONT) TOTAL LENGTH (PULSES)
long        g_lBackTotalLengthPulses;                                           // BLSPREG DS    5        ;~LINE SPEED (BACK) TOTAL LENGTH (PULSES)
BOOL        g_bResetOrderTotalsAtNextRollChange;                                // RSTOFLAG DS    1       ; RESET ORDER FLAG
float       ftestonlyBatchPulsesAccumulator;                                    //testonly
long        g_lCurrentPulses;                                                   // current line speed pulses
long        g_lPreviousPulses;                                                  // previous line speed pulses
long        g_nPulsesDifference;                                                // differnece in line speed pulses
float       g_fTotalHourlyLengthAccumulator;                                    // HRLTACC  DS    5         ; HOURLY LENGTH ACCUMULATOR.
int         g_nHourlyReportCounter;                                             // PRHRCTR DS      1       ; HOUR COUNTER
BOOL        g_bHourDetectedBefore;                                              // HRBEFORE DS     1       ;INDICATES THAT HOUR HAS BEING DETECTED.
BOOL        g_bActivateHourlyReport;                                            // ACTHRREP        DS      1       ; ACTIVATE HOURLY REPORT.
int         g_nWarmUpCounter;                                                   // WARMUPCTR               DS      1       ; WARMUP COUNTER
BOOL        g_bRecipeWaiting;                                                   // RECIPEWAITING           DS      1       ; DEFINE RECIPE WAITING FLAG.
BOOL        g_bBatchHasFilled;                                                   // BATCHHASFILLED          DS      1       ; INDICATES THAT BATCH HAS BEEN FILLED DURING THE CLEANING CYCLE.
BOOL        g_bBlenderInhibit;                                                   // BLENDERINHIBIT          DS      1       ; BLENDER INHIBIT
int         g_nVacuumSequence;                                                   // VACSEQ                  DS      1       ; INDICATES VACUUM SEQUENCING IN SINGLE RECIPE MODE.
unsigned int g_nVacuumTimer;                                                      // VACCTR                  DS      2       ; VACUUM SEQUENCE TIMER
int         g_nVacuumWriteData;                                                  //VACUUMWR                DS      2       ; WRITE WORD SENT TO VAC8 I/O
BOOL        g_bOfflineValveIsOpen;                                               //OFFLINEVALVEISOPEN      DS      1       ; ALLOW MIXER CLEAN TO PROCEED AFTER THE OFFLINE VALVE IS OPENED IN SINGLE RECIPE MODE
BOOL        g_bCleanOnNextCycle;                                                 //CLEANONNEXTCYCLE        DS      1       ; INDICATES THAT CLEAN SHOULD BE INITIATED.
BOOL        g_bWaitForVacuumToFinish;                                            //WAITFORVACUUMTOFINISH   DS      1       ; INDICATES THAT WE ARE WAITING FOR VACUUM TO FINISH.
BOOL        g_bVacuumIsOn;                                                       //VACUUMISON              DS      1       ; VACUUM IS ON.
int         g_nVacuumLoaderNo;                                                   //VACUUMLOADER            DS      1       ; VACUUM LOADER FOR DISPLAY PURPOSES
BOOL        g_bCheckIfVacuumIsRequired;                                          //CHECKIFVACUUMREQUIREDF  DS      1       ; CHECK IF VACUUM REQUIRED
BOOL        g_bVacuumMaterialAway;                                               //VACUUMMATERIALAWAY      DS      1       ; CHECK FOR MATERIAL TO BE VACUUMED AWAY.
BOOL        g_bVacuumInProgress;                                                 //VACUUMINGINPROGRESS     DS      1       ; DEFINE VACUUMING IN PROGRESS
int         g_nBlenderReadyCounter;                                              //BLENDERREADYCTR         DS      1       ; INDICATE THAT BLENDER IS READY.
int         g_nStartUpPagePauseTimer;                                            //PAUSETIMER      DS      1       ; PAUSED TIMER
int         g_nNoLevelSensorAlarmCounter;                                        //NOLSALARMCTR            DS      1       ; NO LEVEL SENSOR ALARM COUNTER
BOOL        g_bOfflineBatchFilled;                                               //OFFLINEBATCHFILLED      DS      1       ; INDICATE THAT OFFLINE BATCH IS READY
BOOL        g_bAlarmFlash;                                                       //ALARMFLASH              DS      1       ; INDICATES IF ALARM SHOULD BE FLASHING
BOOL        g_bFlashSecondCounter;                                               //FLASHSECCTR             DS      1       ; FLASH SECONDS COUNTER
BOOL        g_bAlarmFlashStatus;                                                 //ALARMFLASHSTATUS        DS      1       ; INDICATES WHAT THE STATUS OF THE OUTPUT SHOULD BE
int         g_nLowLevelSensorInputState;                                         //LLSINPUTSTATE   DS      2       ; READ LLS INPUT STATE.
BOOL        g_bInCalibrationMenu;                                                //CALIFG DS     1        ;~CALIBRATION FLAG, SET TO AA WHEN IN CAL MENU O
BOOL        g_bSaveAllCalibrationToOnBoardEEprom;                                //SAVECAL2DATA1F  DS      1       ; SAVE CALIBRATION DATA FLAG.
                                                                                 //SAVECAL2DATA2F  DS      1       ; SAVE CALIBRATION DATA FLAG.
BOOL        g_bSignalXXKgsTargetAlarm;                                           //SIGNALATTARGETALARMFLAG DS      1       ; SIGNAL ALARM ON FEEED OPEN
int         g_nLowLevelSensorAlarmWord;                                          //LLSALARMWORD    DS      2       ; ALARM WORD WRITTEN TO LLS MODULE.
BOOL        g_bLowLevelSensorOnTransition[MAX_COMPONENTS];                       // LS1ONTR   DS    1       ; LEVEL SENSOR ON TRANSITION.
BOOL        g_bLowLevelSensorStatusHistory[MAX_COMPONENTS];                      // LS1ALHI   DS    1       ; INHIBIT REPETITION OS LLS ALARM.
int         g_nExpansionInputData;                                               //EXPANIOINPUTDATA        DS      1       ; EXPANIO PCB STORAGE
BOOL        g_bNoLowLevelSensorCommunications;                                   // NOLLSCOMMUNICATIONS     DS      1       ; NO LLS COMMUNICATIONS FLAG
int         g_nNoLowLevelSensorCommunicationsCounter;                            //NOLLSCOMMUNICATIONSCTR  DS      1       ; NO LLS COMMS COUNTER
BOOL        g_bShowExpiryPage;                                                  // SHOWEXPIRYPAGE          DS      1       ; SHOW EXPIRY PAGE.
BOOL        g_bIsMasterModule;                                                  // ISMASTERMODULE          DS      1       ; INDICATES THAT MASTER CODE IS IN THE CALIBRATION MODULE
BOOL        g_bPrintHourlyReport;                                               // TOPRNFLG  DS    1       ; "TO PRINT FLAG"
BOOL        g_bPrintOrderReport;                                                // TOPRNFLG  DS    1       ; "TO PRINT FLAG"
BOOL        g_bPrintFrontRollReport;                                            // TOPRNFLG  DS    1       ; "TO PRINT FLAG"
BOOL        g_bPrintBackRollReport;                                             // TOPRNFLG  DS    1       ; "TO PRINT FLAG"
BOOL        g_bPrintAlarmReport;                                                // TOPRNFLG  DS    1       ; "TO PRINT FLAG"
unsigned int g_unCalibrationCheckSum;                                            // CALDSM DS     2                 ;~CHECKSUM FOR CAL. DATA
int         g_nAllowLargeAlarmBand;                                             // ALLOWLARGEALARMBAND     DS      1       ; ALLOW LARGE ALARM BAND
BOOL        g_bDoCMRDToAPerKgCalculation;                                       // CMRNOW          DS      1       ; DO CMR CALCULATION.
//--removed-- BOOL        g_bDoCMRDToAPerKgCalculation;                                       // CMRNOW          DS      1       ; DO CMR CALCULATION.
BOOL        g_bPotControl;                                                      // CH1POTBIT DS  1        ; POT. CONTROL BITS (6 CHANNELS)
unsigned int         g_nControlAvgFactorGpmHistory;                                      //
unsigned int g_nControlAvgFactorKghHistory;                                      //
BOOL        g_bControlSetpointChanged;                                          //
int         g_nDiagnosticArrayIndex;                                            //
int         g_nSingleCycleCompNoHis;                                            // check for component no change in single recipe cycle.
int         g_nReMixOnTime;                                                     // MIXONTIME       DS      1       ;
int         g_bBinLevelSensorHistory;                                           // HIGHLEVELSENSORHISTORY  DS      1       ; HISTORY STORAGE OF HIGH LEVEL SENSOR ALLOWS TRANSITION DETECTION.
BOOL        g_bWriteIncreaseDecreaseDataToSEI;                                  // WRITEINCDECF    DS      1       ; WRITE INCREASE DATA TO SEI.
unsigned int         g_nPeripheralCardsPresentHistory;                                   // peripheral card history.
BOOL        g_bIsSystemAlarm;                                                   //
unsigned int g_nStageFillEnableTransitionHistory;                                              //

float       g_fWeightConversionFactor;                                          //weight conversion factor
float       g_fLengthConversionFactor;                                          //length conversion factor
float       g_fThroughputConversionFactor;                                      //throughput conversion factor
float       g_fWeightPerLengthConversionFactor;                                //weight per length conversion factor
float       g_fLineSpeedConversionFactor;                                      //line speed conversion factor
float       g_fWidthConversionFactor;                                         //width conversion factor
float       g_fWeightPerAreaConversionFactor;                                  //weight per area conversion factor
BOOL        g_bProgramLoopRunnning;                                           //
unsigned int g_nVavleLatencyTime;                                             //VALVELATENCYTIMECALC    DS      2
char        g_arrcEmailBuffer[EMAIL_BUFFER_SIZE];
float       g_fComponentActualWeightTopUpBackup;                              //
float       g_fComponentActualWeightITopUpBackup;                              //
unsigned int g_nVavleCalculatedLatencyTime;                                   //VALVELATENCYTIMECALC    DS      2
unsigned int g_nBatchesInKghAvgHistory;
BOOL        g_bLayeringHistory;
BOOL        g_bSPIAtDActive;
BOOL        g_bSPIEEActive;
unsigned int g_nLevelSensorAlarmCounter;                                        // LEVELSENSORALARMCTR     DS      1           ; LEVEL SENSOR ALARM COUNTER
BOOL         g_bHighLoRegrindOptionHistory;
unsigned char   g_CurrentTime_Date_History;
BOOL     g_bNBBRevB1Hardware;                                                // rev B1 hardware
BOOL     g_bEnableTxRxInOpenSerial;
unsigned int    fd0;
BOOL    g_bPrintfRunning;
int     g_nNoPanelReply;
int     g_nEEPROMWriteTimeoutCounter;                                       // eeprom read timeout counter
int     g_nEEPROMReadTimeoutCounter;                                       // eeprom write timeout counter
BOOL    g_bModbusLineSpeedWritten;                                       // MBPRDIAG    DS     2       ; PRODUCTION DIAGNOSTIC OPEN CLOSE               283
float   g_fCommunicationsLineSpeed;                                     //COMMSLSPD DS    3       ; LINE SPEED DOWNLOADED VIA COMMS.
float   g_fCommsLineSpeedFrontLength;                                    // front length accumulator
float   g_fCommsLineSpeedBackLength;                                    // back length accumulator
BOOL    g_bModbusDToAWritten;                                       // MBDTOAFLAG              DS      1
BOOL    g_bModbusDToAPercentageWritten;                             // MBDTOAPERFLAG           DS      1
unsigned int     g_nSPIEEActiveTimeoutCounter;                               // timeout counter
unsigned int     g_nRevertToOperatorPanelCounter;                            // timeout counter
BOOL    g_bRestartPanelNow;
BOOL    g_bRedirectStdioToPort2;
unsigned int g_nPulseOnTime;
BOOL    g_bPulsed;
int     g_cBatSeq_history;
unsigned int g_nResetAllFlowRates;                                  //CLEARALLFLOWRATES       DS      1

volatile unsigned int  g_nTimer0Count;        // count for no. of interrupts.
volatile DWORD  g_lTimer0CounterRegister; // r counter.
volatile DWORD  g_lTimer0CaptureRegister; //  capture register.
         WORD   g_nUSTimeSumEntries;
         DWORD  g_lUSTimeSum;
         DWORD  g_lUSTimeAvg;
         float  g_fUSDistance;
         DWORD       g_lTriggerTime;
         BOOL  g_bT0TriggerMode;
         BYTE  g_nT0InterruptMode;

BOOL    g_bReadOnNext;
float   g_fUltrasonicmmpkg;
float   g_fWeightInChamber;
float   g_fUltrasonicTareDistance;
// ram not to be cleared. --review--
//
int         g_nExtruderNo;                                                      //EXTRUDERNO  DS      2       ; RECIPE NO
int         g_ntestonly;

BOOL        bUSDataAvailable;
unsigned int g_nCycleSecondNo;
float g_fCycleUSDistance[USMAXENTRIES];
WORD  g_nTimeInCycle[USMAXENTRIES];
WORD  g_nUSBufferIndex = 0;
BOOL  g_bUSDataAvailable;
float g_fslope1;
float g_fslope2;
float g_fslope3;

BOOL  g_bUltrasonicLIWInstalled;
BOOL  g_bPrintWeightNow;
BOOL  g_bUpdateSDCardSoftware;
BOOL  g_bSoftwareUpdateRunning;
BOOL  g_bSDCardSPIActive;

unsigned int    g_unCurrentFlapStatus1;
unsigned int    g_unInProgressStatus;
BOOL            g_baCurrentFlapStatus[MULTIBLEND_EXTRUDER_NO];
BOOL            g_baInProgress[MULTIBLEND_EXTRUDER_NO];
BOOL            g_bRan;
BOOL            g_bClearHistory;
BOOL            g_bMultiBlendisPaused;
BOOL            g_bMultiBlendPaused;
BOOL            g_bExistsInQueue;
int             g_nRanx;
unsigned int    g_unaCurrentQueue[MULTIBLEND_EXTRUDER_NO];
int             g_nEndOfQueue;
unsigned int    g_unaDownLoadHistory[DOWNLOADHISTORYNO];
int             g_nNoCurrentFlaps;
unsigned int    g_unCurrentExtrNo;
int             g_nQueueOneSecondTimer;
int             g_BlenderMapping[SOURCEOUTPUTNO][DEFINEDOUTPUTNO];
BOOL            g_bFlowRateChange[MAX_COMPONENTS];
BOOL            g_bPITAllowed;
int             g_nMultiBlendRecipeSaveCtr;
BOOL            g_bDHCP;
BOOL            g_bOptimisationPresent;
BOOL            g_nDeviceListIndexOptimisation;
BOOL            g_bOptimisationFlapStatus[MAX_COMPONENTS];
BOOL            g_bOptimisationLoaderEnable[MAX_COMPONENTS];
BOOL            g_bOptimisationLoaderFill[MAX_COMPONENTS];
BOOL            g_bOptimisationUpdate[MAX_COMPONENTS];
unsigned int    g_nuOptimisationSensorInputState;
unsigned int    g_unOptimisationLoaderOuputs;
BOOL            g_bSaveAllCalibrationToEEpromOutsideCycle;
BOOL            g_bSPIBusBusy;
int             g_nVibrateCatchBoxCtr;

BOOL            g_bCopyConfigFileToSDCard;
BOOL            g_bCopyConfigFileToBackupFile;
BOOL            g_bLoadConfigFromSDCard;
BOOL            g_bLoadBackupConfigFromSDCard;


float           g_fComponentBinAndLoaderWeight[MAX_COMPONENTS];
float           g_fComponentBinWeight[MAX_COMPONENTS];
float           g_fComponentLoaderWeight[MAX_COMPONENTS];
float           g_fRemainingOrderLength;
float           g_fComponentRemainingOrderWeight[MAX_COMPONENTS];
float           g_fOptimisationOrderLength;
float           g_fRemainingOrderWeight;
BOOL            g_bOptimisationFlapHistoryStatus[MAX_COMPONENTS];
int             g_nOptimisationFlapDebounceCounter[MAX_COMPONENTS];
float           g_fComponentLoaderWeightAccumulator[MAX_COMPONENTS];                 // CH1WACCF DS    5         ; weight accumulator,  (front roll)
int             g_nLoaderEmptyingCounter[MAX_COMPONENTS];
BOOL            g_bModbusLengthWritten;
float           g_fWeightPerMeterFromLSpeed;
BOOL            g_bFirstPulsesReadFromSEI;
BOOL            g_bInitiatePurge;
BOOL            g_bBlenderPurged;
BOOL            g_bBlenderPurgeingInProgress;
BOOL            g_bMultiBlendRecipeIsZero[MULTIBLEND_EXTRUDER_NO];

WORD            g_arrnMBTable[MB_TABLE_SIZE];
WORD            g_arrnMBTableFlags[MB_TABLE_WRITE_SIZE];
WORD            g_nMBWriteOrFlag;
WORD            g_nMBWriteOrErrorFlag;
WORD            g_nMBWriteOrFlagTemp;
//char            g_cMultiblendDiagnosticsBuffer[PRINT_BUFFER_SIZE];
BOOL            g_bWriteToIniFileAgain;
int             g_nOverrideCounter;
int             g_nAllowReadOfControlInputsCtr;
BOOL            g_bStartingMultiBlendRecipe;
int             g_nOpenserialStatus;
int             g_nOpenserialStatusCounter;
int             g_nCloseserialStatus;
int             g_nUartDataOpenSerial;
int             g_nUartDataCloseSerial;
BOOL            g_bUsingSmallBatchSize;
BOOL            g_bFirstTimeToReadScrewSpeed;  // FIRSTTIMETOREADSSAD     DS      1 ; FIRST READ OF FLAG
float           g_fExtruderRPMHistory;
BOOL            g_bCalculateScrewSpeedConstant; // CALCSCREWCON    DS      1       ; "CALCULATE SCREW CONSTANT NOW"
float           g_fScrewSpeedCalibrate;
int             g_nVolumetricEstimateCounter;
int             g_nWriteHoldOffCounter;
int             g_nSuspendControlCounter;           // SUSPENDCONTROLCOUNTER DS    1    ; SUSPEND CONTROL COUNTER
int             g_nHoldOffLevelSensorAlarmSecondCounter; //HOLDOFFLSENSORALCTR     DS      1   ; HOLD OFF LEVEL SENSOR ALARM COUNTER


BOOL            g_bCheckOptimisationAgain[MAX_COMPONENTS];
BOOL            g_bFirstOptimisationCheck;
BOOL            g_bModbusScrewSpeedWritten;
unsigned int    g_nComponentPerDeviationErrorCounter[MAX_COMPONENTS]; // COMP1PERDEVERRORCOUNTER DS  1  1-12

float           g_fComponentActualWeightBufferRR[MAX_COMPONENTS][MAX_KGH_RR_SAMPLES];     //kg/h round robin buffer.
unsigned int    g_nComponentActualWeightRRIndex[MAX_COMPONENTS];                       //
unsigned int             g_nComponentActualWeightRRSummationCounter[MAX_COMPONENTS];           //
float           g_fComponentActualPercentageRR[MAX_COMPONENTS]; // BPRRCNT1-12        DS      2       ; ACTUAL % COMPONENT 1
int             g_IncreaseOutputOn10hzCounter; // INCREASEOUTPUTONCTR     DS      1   ;
int             g_DecreaseOutputOn10hzCounter; // DECREASEOUTPUTONCTR     DS      1   ;

BOOL            g_bResetComponentWeightsRoundRobin;
int             g_nAutocycleLevelSensorOff10hzCounter; // AUTOCYCLELEVELSENSOROFF DS  1    ; AUTOCYCLE FOR SENSOR
BOOL            g_bSHAHardwareCheckFailed;
BOOL            g_bSaveUSBFileToUSB;
//BYTE            g_byPrinterBuffer[USB_PRINT_BUFFER_SIZE];
BOOL            g_bUSBDataWrite;
BOOL            g_bComponentHasFilledByWeight[MAX_COMPONENTS];

float           g_fComponentWeightSamples[MAX_COMPONENTS][MAX_CYCLE_DIAGNOSTICS_SAMPLES];
int             g_nComponentWeightSampleCounter[MAX_COMPONENTS];
float           g_fComponentWeightMean[MAX_COMPONENTS];
float           g_fComponentWeightStdDev[MAX_COMPONENTS];
float           g_fComponentWeightStdDevPercentage[MAX_COMPONENTS];
float           g_fComponentWeightMaximum[MAX_COMPONENTS];
float           g_fComponentWeightMinimum[MAX_COMPONENTS];

WORD            g_arrnMBTableSetpointCopy[MB_TABLE_SETPOINT_SIZE];
unsigned int    g_nWritenwrdCounter;
BOOL            g_bPanelWriteHasHappened;
BOOL            g_bSEIReadDataReady;

unsigned int    g_nExtruderVolumetricDAValue;
BOOL            g_bUSBCopyInProgress;
unsigned long   g_lUSBFileDataCopied;
F_FIND g_nSdCardFileDetails[MAX_SD_CARD_FILES];
unsigned int    g_nNoOfFilesOnSdCard;
BOOL            g_bReadSDCardDirectory;
unsigned int    g_nCopyFileNoToUSB;
unsigned int    g_nCopyFileNoToUSBRequest;

long            g_lSPIBaud;
F_FILE * fpSdCard;
unsigned int    g_nUSBCopySequencer;
unsigned int    g_nExtruderControlZeroToTenSetpoint;
unsigned int    g_nExtruderControlSetpointIncDec;
unsigned int    g_nSEIWriteRequiredCounter;

unsigned int    g_nControlLagHistory;            // CONTROLLAG - seconds?
unsigned int    g_nIncreaseRateHistory;                   // INCREASERATE - Increase rate - d/a counts per second
unsigned int    g_nDecreaseRateHistory;                   // DECREASERATE - Decrease rate - d/a counts per second
unsigned int    g_nControlDeadbandHistory;                // DEADBAND - deadband for inc/dec control.
unsigned int    g_nFineIncDecHistory;                     // FINEINCDEC - Fine increase/decrease

BOOL        g_bLIWReadDataAvailable[MAX_LIW_CARDS];                                            // new variable
BOOL        g_bLIWReadDataReady[MAX_LIW_CARDS];
BOOL        g_bCalibrationDataChecksumValid;


BOOL     g_bSEIIncDecSetpointDownload;

BOOL    g_bSEIAutoStatus;                                 // SEI auto status
BOOL    g_bAllowProcesDataSaveToEEPROM;
unsigned int    g_nSaveProcessDataToEEPROMCounter;   // no of cycles to save to eeprom
RepeatedDataStruct g_ProcessData;
BOOL    g_bSaveAllProcessDataToEEprom;
BOOL    g_bSaveAllProcessDataOutsideCycle;

unsigned int    g_nCycleCounter;

unsigned int    g_nCurrentExtruderNo;
WORD            g_arrnMBSetpointHistory[MB_TABLE_SETPOINT_SIZE];
BOOL            g_bAllowMultiBlendCycle;  //kwh

unsigned int    g_nSDCardSoftwareUpdateComplete;        // shows software update is complete
unsigned int    g_nSDCardLoadConfigComplete;            // shows load of config is complete
unsigned int    g_nSDCardLoadBackupConfigComplete;      // shows load of backup config is complete
unsigned int    g_nSDCardSaveConfigComplete;            // shows save of config is complete
unsigned int    g_nSDCardCopyConfigToBackupComplete;    // shows copy of config to is complete


BOOL            g_bSDCardSoftwareUpdateComplete;        // shows software update is complete
BOOL            g_bSDCardLoadConfigComplete;            // shows load of config is complete
BOOL            g_bSDCardLoadBackupConfigComplete;      // shows load of backup config is complete
BOOL            g_bSDCardSaveConfigComplete;            // shows save of config is complete
BOOL            g_bSDCardCopyConfigToBackupComplete;    // shows copy of config to is complete

unsigned int    g_nDownloadManagerNZTimeoutCtr;
unsigned int    g_nDisableDTAPerKgCtr;                  //DISABLEDAPKGHCTR        DS      1    ;ESTIKO CHANGE

BOOL            g_bCalibrationDataInvalid;    // calibration data invalid
BOOL            g_bProcessDataInvalid;    // process data invalid
BOOL            g_bTelnetEnable;
BOOL            g_bTelnetHistory;

unsigned int    g_nBatchCommsStatusSEI;         // sei comms status
unsigned int    g_nBatchCommsStatusLLS;         // LLS comms status
unsigned int    g_nBatchCommsStatusOptimisation;// Optimisation comms status
unsigned int    g_nBatchCommsStatusNetwork;     // network comms status
unsigned int    g_nBatchCommsStatusTCPIP;       // TCPIP comms status
unsigned int    g_nBatchCommsStatusPanel;       // panel comms status


long     g_nBatchCommsSEITimeoutHistory;         // comms timeout history
long     g_nBatchCommsLLSTimeoutHistory;         // comms timeout history
long     g_nBatchCommsOptimisationTimeoutHistory;// comms timeout history
long     g_nBatchCommsNetworkGoodTxHistory;      // comms good tx history Network

long     g_nCommsVacuumLoadingTimeoutHistory;            //
long     g_nBatchCommsStatusVacuumLoading;            //

long     g_nBatchCommsPanelTimeoutRxHistory;            //
long     g_nBatchCommsPanelTimeoutRx;            //

unsigned int    g_nBatchCommsTCPIPGoodTxHistory;        // comms good tx history TCPIP
unsigned int    g_nTCPIPReadHoldRegsCtr;               // indicates no of tcpip hold reg commands complete.

BOOL            g_bToAutoEvent;
BOOL            g_bToManualEvent;
BOOL            g_bWeighHopperCalibratedEvent;
BOOL            g_bWeighHopperTaredEvent;
BOOL            g_bCleaningInitiatedEvent;

char            g_cCurrentMonthHistory;
unsigned int    g_nWriteConfigToSDInNSeconds;
unsigned int    g_nOperatorPanelPageId;
unsigned int    g_nBlenderType;
WORD            g_nSDCardDelayTime;   // variable to delay the sd card write function.
WORD    g_CurrentTime[TIME_ARRAY_SIZE];
WORD    g_PowerUpTime[TIME_ARRAY_SIZE];
WORD    g_cPowerDownTime[TIME_ARRAY_SIZE];
int g_arrnWriteSEIMBTable[SEI_TABLE_WRITE_SIZE];
int g_arrnReadSEIMBTable[SEI_TABLE_READ_SIZE];
int g_arrnWriteLLSMBTable[LLS_TABLE_WRITE_SIZE];
int g_arrnReadLLSMBTable[LLS_TABLE_READ_SIZE];
int g_arrnWriteOptimisationMBTable[LLS_TABLE_WRITE_SIZE];
int g_arrnReadOptimisationMBTable[LLS_TABLE_READ_SIZE];

unsigned int    g_nDisableSDCardAccessCtr;
unsigned int    g_nAMToggleStatusDelayCtr;

BOOL            g_bWriteEventLogToSdCard;
char            g_cEventLogFile[EVENTLOGSTRINGLENGTH];
char            g_cEventName[20];

unsigned int    g_nLiquidAdditiveComponent;
BOOL            g_bLiquidEnabledHistory;
unsigned int    g_nLiquidAdditiveOutputDAValue;             //  liquid additive d/a value
float           g_fLiquidAdditiveOutputPercentage;             //  liquid additive output %
float           g_fLiquidKgPerHourPerPercentOutput;             //

BOOL            g_bModbusLiquidDToAWritten;
BOOL            g_bModbusLiquidDToAPercentageWritten;
BOOL            g_bLiquidControlAllowed;
unsigned int    g_nCheckIfWeighHopperOpenedSecCtr;             //
float           g_fWeightAtOpenDumpFlap;             //
WORD            g_nTemperature;
unsigned int    g_nLiquidOnTimeCtr;
BOOL            g_bAlarmOccurredHistory[MAXIMUM_ALARMS][MAX_COMPONENTS];
float           g_fOneSecondHopperWeightAtEndOfCycle;             //
float           g_fThroughputCalculationBatchWeight;             //
float           g_fKgperHour;             //
float           g_fLongTermLengthCounter;
float           g_fLongTermResettableLengthCounter;

unsigned int    g_nTelnetSequence;
BOOL            g_bTSMTelnetInSession;
BOOL            g_bTelnetUserNameOkay;
BOOL            g_bShowNetworkWrites;
BOOL            g_bWriteDiagnosticsToSDCard;
unsigned int    g_nTelNetTimeoutCtr;
unsigned int    g_nTelnetContinuousUpdate;
unsigned int    g_nFileSegmentNo;
char            g_cTelnetFileName[20];
BOOL            g_bPrintCycleDataToTelnet;
unsigned int    g_nTelNetSecondsCounter;

long         g_lTCPIPCommsCounter;  // tcpip counter
unsigned int    g_bTelNetWindowCtr;
char            g_cOrderReportStorageBuffer[ORDER_REPORT_BUFFER_SIZE];
BOOL            g_bAToDMaxExceeded;
BOOL            g_bApplicationSPIInProgress;
BOOL            g_bTaskSPIInProgress;
BOOL            g_bTelnetSDCardAccessInProgress;

BOOL            g_bHourlyResetMin;
BOOL            g_bHourlyResetMax;


structHistoryComponentData   g_sOngoingHistoryComponentLogData;
structHistoryComponentData   g_sHourHistoryComponentLogData;
structHistoryIndividualEvents g_sOngoingHistoryEventsLogData;
structHistoryIndividualEvents g_sHourHistoryEventsLogData;

structHistoryIndividualEvents g_s24HrOngoingHistoryEventsLogData;
structHistoryIndividualEvents g_s24HrHistoryEventsLogData;


structHistoricCommsData       g_sOngoingHistorySEIComms;
structHistoricCommsData       g_sOngoingHistoryLLSComms;
structHistoricCommsData       g_sOngoingHistoryOPTComms;
structHistoricCommsData       g_sOngoingHistoryTCPIPComms;
structHistoricCommsData       g_sOngoingHistoryNetworkComms;

structHistoricCommsData       g_sHourHistorySEIComms;
structHistoricCommsData       g_sHourHistoryLLSComms;
structHistoricCommsData       g_sHourHistoryOPTComms;
structHistoricCommsData       g_sHourHistoryTCPIPComms;
structHistoricCommsData       g_sHourHistoryNetworkComms;

char    g_OrderReportName[20];
char    g_cEventLogFileCopy[EVENTLOGSTRINGLENGTH];
unsigned int    g_nUSBReadTimeoutInPits;
unsigned int    g_nUSBCommandSendTimeoutInPits;
unsigned int    g_nUSBFlushTimeoutInPits;
unsigned int    g_nUSBWriteTimeoutInPits;
unsigned int    g_nUSBEchoedTimeoutInPits;
unsigned int    g_nUSBUSBSendFileCommandsTimeoutInPits;
unsigned int    g_nUSBGetPrinterStatusTimeoutInPits;

unsigned int    g_nUSBReadTimeoutCtr;
unsigned int    g_nUSBCommandSendTimeoutCtr;
unsigned int    g_nUSBFlushTimeoutCtr;
unsigned int    g_nUSBWriteTimeoutCtr;
unsigned int    g_nUSBEchoedTimeoutCtr;
unsigned int    g_nUSBUSBSendFileCommandsTimeoutCtr;
unsigned int    g_nUSBGetPrinterStatusTimeoutCtr;



BOOL            g_bHiRegLevelSensor;              // regrind high ls
unsigned int    g_nHiRegLSCoveredCounter;
unsigned int    g_nHiRegLSUnCoveredCounter;
unsigned int    g_nExpansionOutput2;
BOOL            g_bWriteToExpansion2;              //
int             g_nCyclePauseTypeCopy;                                      // CYCLEPAUSETYPE  DS      1       ; INDICATES PAUSE TYPE START / END OF CYCLE.
unsigned int    g_nBlastMomentaryOnCtr[MAX_COMPONENTS];
unsigned int    g_nFunctionRunning;
unsigned int    g_nFunctionSubSectionRunning;

BOOL            g_bUSBDebug;
BOOL            g_bResetNetworkSlaveComms;
BOOL            g_bResetPanelSlaveComms;
BOOL            g_bAddAlarmOccurredToEventLog;
BOOL            g_bAddAlarmClearedToEventLog;
unsigned int    g_ucAlarmCode;
unsigned int    g_nAlarmComponent;
StructDebug     g_nTSMDebug;
structSelfTest  g_sSelfTest;
unsigned int    g_nSPISelect1;
unsigned int    g_nTimeInAutoMinutesCtr;

BOOL            g_bNoParityHistory;
BOOL            g_bDoSelfTestDiagnostics;
BOOL            g_bInLastMinute;

WORD            g_wHourHistory;  // hourly history counter
BOOL            g_bHourRollOver;

char    g_cPanelVersionNumber[PANEL_REVISION_STRING_SIZE];
DescriptionStruct g_sDescriptionData;
unsigned int    g_nStorageInvervalCtr;

long            g_lOrigBatchPulsesAccumulator;
char            g_cSEISoftwareVersionNumber[SEI_SOFTWARE_REVISION_STRING_SIZE];
BOOL            g_bUseLineSpeedForGPMCalc;
BOOL            g_bNoLeakAlarmCheck;
BOOL            g_bHiRegLevelSensorHistory;
BOOL            g_bDumpFlapHasOpened;
BOOL            g_bI2CError;

unsigned int    g_nblenderpulsetestctr;
BOOL            g_bPulseValveNow;
unsigned int    g_bPulseTime;
WORD            g_wForeGroundCounter;
WORD            g_wProgramLoopEntry;
char            g_cModbusPrintBuffer[1000];
BOOL            g_bPrintModbusMessageToTelnet;

unsigned int    g_nPulsingSequence[MAX_COMPONENTS];
long            g_lComponentTargetCountsCopy[MAX_COMPONENTS];
float           g_fComponentTargetWeightCopy[MAX_COMPONENTS];
BOOL            g_bFastSettling;
unsigned int    g_nComponentPulseCounter[MAX_COMPONENTS];
float           g_fComponentActualWeightHistory[MAX_COMPONENTS];
BOOL            g_bFirstPulse;
BOOL            g_bCalibratePulsing[MAX_COMPONENTS];
unsigned int    g_nPulseTime;
unsigned int    g_nPulseTimeForMinWeight[MAX_COMPONENTS];
BOOL            g_bICSRecipeDataWritten;
BOOL            g_bSaveAdditionalDataToEEPROM;
unsigned int    g_nValidFlowRateExceededCtr[MAX_COMPONENTS];
BOOL            g_bAcceptFlowRate[MAX_COMPONENTS];
BOOL            g_bLockIsEnabled;
BOOL            g_bConfigIsLocked;
BOOL            g_bPitRunning;
char            g_cModbusEventBuffer[MODBUS_EVENT_LOG_BUFFER_LENGTH];
BOOL            g_bAddEventToEventLog;
unsigned int    g_nHoldOffEventLogWriteCtr;
BOOL            g_bAtoDEndOfConversionErrorHistory;
BOOL            g_bAtoDNegativeSignHistory;
unsigned int    g_nWaitForSPIBusToSettleCtr;
BOOL            g_bDoAToDTest;
unsigned int    g_nBlenderTypeHistory;
BOOL            g_bSelfTestInProgress;
float           g_fCalibrationWeight;
unsigned int    g_nFillingMethodHistory[MAX_COMPONENTS];
BOOL            g_bGotHalfGramTime;
unsigned int    g_nHalfGramTime[MAX_COMPONENTS];
float           g_fRequiredPulseWeightInGrams[MAX_COMPONENTS];
float           g_fCurrentRecipePercentageHistory[MAX_COMPONENTS];
float           g_fMaxKgPHrPossible;
unsigned int    g_nAfterEndOfCycleCounter;
BOOL            g_bVAC8ExpansionRequired;
unsigned int    g_nVAC8ExpansionIndex;
unsigned int    g_nVAC8ExpansionOutput;
unsigned int    g_nVaccumLoadingVAC8IOCards;
long            g_lSettledAverageCounts[MAX_COMPONENTS];
long            g_nAToDStabilityLog[MAX_STABILITY_READINGS];
BOOL            g_bLogLoadCellReading;
unsigned int    g_nDelayBeforeLoadCellLogCtr;
unsigned int    g_nAToDStabilityCtr;
long            g_nAToDStabilityMaxValue;
long            g_nAToDStabilityMinValue;

long            g_nAToDRefStabilityLog[MAX_STABILITY_READINGS];
BOOL            g_bTakeRefAToDReading;
unsigned int    g_nDelayBeforeRefLogCtr;
unsigned int    g_nAToDRefStabilityCtr;
long            g_nAToDRefStabilityMaxValue;
long            g_nAToDRefStabilityMinValue;
BOOL            g_bRefAToDStabilityTestInProgress;
BOOL            g_bDecideOnRefStability;
BOOL            g_bDecideOnLoadCellStability;

unsigned int    g_nVac8ExpansionHistory;
unsigned int    g_nRefTimeoutCtr;
unsigned int    g_nTopUpCompNoRefHistory;
float           g_fWeightLeftAfterRegrind;
float           g_fRegrindWeight;
BOOL            g_bFileCouldNotBeOpenedEvent;
unsigned int    g_nMountFatReturnedValue;
unsigned int    g_nChangeDriveReturnedValue;
BOOL            g_bComponentHasRetried[MAX_COMPONENTS];


