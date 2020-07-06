///////////////////////////////////////////////////////////
// InitBlnd.c
//
// This is the initialisation routine the batch blender
// Called in main after reset.
// Corresponds to stuff BLBGND - PGLOOP in SBBGND18.ASM
// Note: Is intended to initialise the blender process data, not system hardware.
//
//
// Entry:       None
//
// Exit:        Void
//
// M.McKiernan                                          23-07-2004
//      First Pass.

//
//  P.Smith                                                     19-08-2004
//      Initialise multiple stage filling variables
// M.McKiernan                                          06-09-2004
// Removed test stuff for TopUp/Layering.
// CloseAll() c alled
// g_bMixOff set depending on blender type.
// g_bRegrindPresent, defined as false.
// g_bFstCompCalFin set to FALSE.
// g_bRunEst set to FALSE.
// All variables (in BatVars.c) initialised in InitialiseBatchVariables().
//
// M.McKiernan                                          07-09-2004
// Added intialisation for:g_nCheckLSensorInNSeconds, g_bNoFillOnClean, g_bPhaseOneFinished
//                                  g_nLeftCornerCleanOnCounter, g_nRightCornerCleanOnCounter, g_nAirJet2OnCounter
// M.McKiernan                                          09-09-2004
// Following initialised to "UNCOVERED" g_bHighLevelSensor,g_bHighLevelSensorHistory,g_bLowLevelSensor,g_bLowLevelSensorHistory
// Initialise: g_nHighLSCoveredCounter,g_nLowLSCoveredCounter,g_nHighLSUnCoveredCounter,g_nLowLSUnCoveredCounter
// Initialise: g_bOverrideClean, g_bSetCleanWhenCycleEnd, g_nMixerOnCounter10Hz
// Initialise: g_fAvgHopperWeight
// M.McKiernan                                          14-09-2004
// Removed initialisation for g_CalibrationData.m_nRegrindComponentNumber
// M.McKiernan                                          17-09-2004
//  Initialise: g_lStoredComponentCountsStage1[], g_lStoredComponentCountsStage2[], g_nStageFillEnHistory                   // STAGEFILLENH    DS      1       ;STAGE FILL ENABLE HISTORY
// M.McKiernan                                          20-09-2004
// Enable layering(g_CalibrationData.m_bLayering,g_CalibrationData.m_nStageFillEn = THREESTAGEFILLINGID)
// for test purposes, i.e. testing multistage filling.
// M.McKiernan                                          22-09-2004
// Disabled layering - g_CalibrationData.m_bLayering = FALSE
//
// M.McKiernan /P.Smith                                 28-09-2004
// Disabled multistage filling - testing topup mode.
// Initialise: G_bVenturiFirstTime, g_bVenturiFillByWeight, g_nLoadConfigurationFailure.
// Result of LoadConfiguration stored into g_nLoadConfigurationFailure
// Set no of components to 8. g_CalibrationData.m_nComponents = 8
// Initialised g_CalibrationData.m_bCycleFastFlag = TRUE
// m_fMaxGramsPerPercent renamed to m_fMaxBatchSizeInKg, and initialised to 10.0.
// Set for non-standard component configuration when in top-up mode.
// M.McKiernan                                          11-10-2004
// Initialisation for: g_bManualAutoFlag & g_bSaveAllCalibrationToEEprom, g_bCalibrationDataChange, g_bNewMaxScrewSpeed
//                          g_nDACPerKgDoneFlag
// Added  init for g_nSaveRecipeSequence g_nSaveRecipeByteNo
// Load Recipe #1 at start and do a 100% check.
// M.McKiernan                                                          01-11-2004
//      Replaced LoadConfiguration() by LoadAllCalibrationDataFromEEprom()
//          No longer hardcode the no. of components, use eeprom value.
// M.McKiernan                                                          08-11-2004
// Diagnostic for checking CalculateModbusCRC(). - has been removed.
//  P.Smith                                                     09-11-2004
//  Initialise cleaning variables
// M.McKiernan                                                  15-11-2004
// Pause flag redefined from BOOL to an int g_nPauseFlag
// Renamed g_bPToNormal to g_bPauseToNormal.
// g_bByPassMode, g_bOverrideOutputs, g_bFeedValveClosed g_bCleanAtEndOfCycle, g_bPauseOnActive
// g_bSecondRollover g_bForegroundSecondRollover, g_bPrevLevSen1Stat, g_bMixerTripped, g_nMixerTrippedCounter
// g_nPauseOnActiveCounter, g_nPauseOnInActiveCounter, g_bExtruderControlStartUpFlag
// Added g_bLowLevelSensorStatus[]
// Added g_fDtoAPerKGH, g_fDtoAPerMPM, g_nSEIInputStates, g_nSEIAlarms, g_fActualGrammesPerM2
// Remove the hardware setting of cleaning parameters & top-up mode.
//
// M.McKiernan                                                  25-11-2004
// Added g_bDiagnosticsPasswordInUse, g_nDeviceToPollCounter
// g_bCheckIfShouldCycleFlag, g_nCheckIfShouldCycleCounter, g_bShowInstantaneousWeight, g_bREadIncDecCalibrationData, g_nDoIncDecReadInNSeconds
// g_nIgnoreLSTime, g_nBatchMonitorCounter, g_bRunLevelSensorCheck, g_nBypassSensorUncoveredCounter
// g_bUpdateHiLoFlag, g_nPauseInNSeconds
// g_bSEIPollInputsFlag, g_bWriteDAValueFlag,   g_bSEIPollLSFlag, g_bWriteAlarmsFlag, g_bReadDAValueFlag, g_bHourFlag
// g_nDoDiagnosticsCounter, g_bDoDiagnosticsFlag, g_nPITFractionCounter
// g_nSeqTimeout, g_bPeripheralCommsSTarted, g_nLineSpeedFractionCounter, g_nTinySlideOpenTimeCounter;
// g_nForegroundTenHzFlag, g_nTinySlideCycleCounter
// g_nTenthsOfSecondCounter2, g_bSEIPollIncDecFlag, g_nCMRValidTimer, g_bRemoteKeyPoll
// g_nSecondCounter2, g_bFGSecondFlag2, g_bFGOneMinuteFlag
// g_nDisplayUpdateCounter, g_bUpdateDisplayNow, g_nMixerOffCounter, g_nMixerOnTime;
// g_nExtruderDAValue g_nMixerTransitionCounter
//  g_nSecondCounter g_bTwoSecondFlag g_nTenSecondCounter g_bTenSecondFlag
// g_nFrontRollChangeLag, g_nBackRollChangeLag
// g_nMixScrewPulseCounter, g_bMixScrewRotationAlarm
// g_nRTCSecondCounter, g_bRTCMinuteFlag, g_nRTCMinuteCounter
// g_bOneSecondFlag, g_nKgHrSecondsCounter, g_nLevelSensorSimulator
//
// M.McKiernan                                                  07-12-2004
// Removed hard coding of g_lCmpTime[]'s at start-up.
// M.McKiernan                                                  13-12-2004
// Added initialisation for g_bAlarmRelayStatus, g_bAlarmBeaconStatus
// call to InitialiseAlarmTable().
//
// M.McKiernan                                                  14-12-2004
// Moved testing for alarm functions to after Modbus table initialisation. ( has been commented out now)
//
// M.McKiernan                                                  20-12-2004
// Hardcode the serial print options: see --SCI--
//
// M.McKiernan                                                  25-11-2004
// Added g_fRRThroughputKgPerHour, g_fRRThroughputLbPerHour g_fThroughputKgPerHour
// g_fThroughputLbPerHour  g_fBatchKgPerHour g_fBatchTimeInSeconds g_nPreviousPITTime
// g_fRRKgPerHour, g_fRRLbPerHour
// Call ResetKghRoundRobin().
//
// M.McKiernan                                                  18-01-2005
// g_bRemoteOperatorPanelPresent, g_bSEIPresent, g_bLLSPresent;
// configure and initialise the Modbus master module.  ConfigureModbusMaster() InitialiseModbusMaster()
// Added g_nCommsUpdateStartIndex, g_bCommsUpdateActive, g_nStartAttributeUpdateIndex, g_bCommsAttributeUpdateActive
//
// P.Smith                                                  8-02-2005
// Initialise variables for control
// Set kg/ h set point to 400 as a test.
//
// P.Smith                                                  10-02-2005
// Copy d/a to modbus table on restart g_arrnWriteSEIMBTable[MB_SEI_DA]
// Also set g_arrnWriteSEIMBTable[MB_SEI_COMMAND] to 3 to set SEI to remote mode
//
// P.Smith                                                  10-02-2005
// Change g_nCCVsFlag to g_bCCVsFlag
//
// P.Smith                                                 14-02-2005
// g_nDACPerKgDoneFlag set to TRUE on startup
//
// P.Smith                                                  2/3/5
// Added ResetKgHRoundRobin,ResetGPMRoundRobin
//
// P.Smith                                                  3/3/5
// g_bDmpLevSen set to FALSE
// Remove preset of mode to kg/h
//
// P.Smith                                                  29/3/05
// Set g_CurrentRecipe.m_fDeadbandForTargetWeight to 20kgs as a test
// Initialisation of all varaible brought up todate.
//
// P.Smith                                                  7-04-2005
// added reset of variables
//
// P.smith                                                  12/07/05
// update reset of variables.
//
// P.smith                                                  9/08/05
// update reset of variables.
//
// P.smith                                                  12/08/05
// g_nBlenderReadyCounter set to BLENDERREADYTIME
//
// P.smith                                                  16/08/05
// Initialisation of g_OnBoardCalData.m_cPermanentOptions to allow blender to run
//
// P.Smith                                                  18-08-2005
// set g_nNoLevelSensorAlarmCounter = IGNORELSALARMNO
//
// P.Smith                                                  30/08-2005
// Remove         g_fRRKgPerHour,g_fRRLbPerHour
//
// P.Smith                                                  1/09/2005
// clear g_cSeqTable array
//
// P.Smith                                                  5/9/05
// Remove g_nLSDelay
//
// P.Smith                                                  5/9/05
// Update initialisation of variables
//
// P.Smith                                                  12/9/05
// Initialisation for single recipe mode
//
// P.Smith                                                  13/9/05
// Bring initialisation of variables up to date
//
//
// P.Smith                                                  27/9/05
// call CheckLicensePowerUpStatus,DecideOnLicenseExpiryWarning
//
//
// P.Smith                                                  28/9/05
// removed // SeqInit();   // for serial comms - peripherals.
//
// P.Smith                                                  3/10/05
// Bring initialisation of variables up to date
//
// P.Smith                                                  3/10/05
// if l5 fitted, then pause blender.
// generate percentage alarm if component % not equal to 100 %
// set g_nPreviousPITTime = PIT_FREQUENCY
// if offline mode set g_nOffline_Tracking = BATSEQ_OFFDELADMPID and Closefeed
// merge asm CHECKSTDSETUP inline
// no need to generate checksum as this is generated when the data is saved.
// g_bCheckIfShouldCycleFlag set to TRUE
// added RightCornerCleanOff();  turn off right corner clean
//       LeftCornerCleanOff();   turn off left corner clean
// ResetCleanStartMBBit(),ResetCleanFinishMBBit();
// merge CHECKFORCLEANOPTION inline no need for CALCULATEEEPROMCHECKSUM
//
// P.Smith                                                  10/10/05
// Correct CHECKSTDSETUP when checking g_CalibrationData.m_bStandardCompConfig
// was checking the wrong sense.
//
// P.Smith                                                  17/10/05
// Remove initialisation of set points, now downloaded from B&R panel
//
//
// P.Smith                                                  17/10/05
// update initialisation.
//
// P.Smith                          20/10/05
// rename  g_bAllowLargeAlarmBand -> g_nAllowLargeAlarmBand
//
// P.Smith                          3/11/05
// call SetUpDiagnostics();         //asm SETUPDIAGPTR
//
// set g_bOutputDiagMode if g_CalibrationData.m_nDiagnosticsMode >= TEST_PROGRAM_1
// was originally ==
//
// P.Smith                          11/11/05
// Clear g_arrnWriteUFP2MBTable on power up, this was causing a problem with comms
// to the remote display when the array was not cleared.
// set g_nSingleCycleCompNoHis to zero.
//
//
// P.Smith                          15/11/05
// g_nReMixOnTime set to 0
// g_bLowLevelSensorStatusHistory set to covered on power up.
//
// P.Smith                          29/11/05
// if increase /decrease mode set g_nDoIncDecReadInNSeconds to initiate a read
// from the sei to obtain all increase /decrease data.
//
// P.Smith                          5/12/05
// clear g_arrnReadUFP2MBTable & other write areas
//
// P.Smith                          20/12/05
// Move InitialiseAlarmTable before the eeprom data is loaded.
// This was causing a problem with the alarms being generated.
// The invalid calibration data alarm was being generated and
// then the alarm table was cleared effectively wiping out the
// alarm generated.
//
// P.Smith                          5/12/05
// update reset of initialisation variables.
// clear cpi rr structures
// WorkCPIRRData,MainCPIRRData,Stage2CPIRRData,Stage3CPIRRData
// clear g_ffInstantaneousCPI array
//
// P.Smith                          10/1/06
// correct if(g_OnBoardCalData.m_cPermanentOptions & LICENSE_CLEANOPTIONBIT ==0)
// needs a second bracket.
//
// P.Smith                      23/1/06
// first pass at netburner hardware conversion.
// BOOL -> BOOL
// included  <basictypes.h>
// added #include "SerialStuff.h"
//       #include "General.h"
//       #include "ConversionFactors.h"
//       #include "SetpointFormat.h"
//       #include "BatchMBIndices.h"
//       #include "BatchCalibrationFunctions.h"
//       #include "Blrep.h"
//       #include "16R40C.h"
//       #include "InitBlnd.h"
//       #include <String.h>
//       #include <stdio.h>
//       #include "TSMPeripheralsMBIndices.h"
//       #include "InitialiseHardware.h"
//       #include "NBBGpio.h"
//       #include "MBMHand.h"
//       #include "MBMaster.h"


// remove InitialiseAlarmTable();
// remove g_nLoadConfigurationFailure = LoadAllCalibrationDataFromEEprom();
// remove g_nLoadConfigurationFailure = LoadAllCalibrationDataFromOnBoardEEprom();
// remove InitialiseWeightVariables();
// remove LoadRecipe(1);                                                                  //load recipe 5  to TempRecipe.
//remove  PauseOn();    /* pause blender if not equal to 100 % */
// remove PutAlarmTable( PERALARM,  0 );      /* indicate alarm, Component no. is zero.  */
// remove RemoveAlarmTable( PERALARM,  0 );       /* clear % alarm., Component no. is zero. */
// remove CloseAll();       // close all feed valves
// remove ResetKgHRoundRobin();               // RESRROB - Reset kg/h round robin.
// remove ResetGPMRoundRobin();               //ASM = RESRROB
// remove ResetCleanStartMBBit();                             /* asm RESETCLEANBITS  */
// remove ResetCleanFinishMBBit();                            /* asm RESETCLEANFINISHBIT */
// remove RightCornerCleanOff();  /* turn off right corner clean */
// remove LeftCornerCleanOff();   /* turn off left corner clean  */
// remove InitialiseMBTable();                            // initialise the ModBus table.
// remove ModbusMasterTest();   // --REVIEW-- test purposes only, to be removed.
// remove VacInhibitOn();      // ASM VACINHIBITON  sets vacuum inhibit in pause flag
// remove CheckLicensePowerUpStatus(); // ASM CHECKPOWERUPSTATUS check power up status for license
// remove DecideOnLicenseExpiryWarning();     // ASM DECIDEONEXPIRYPAGE decide if a warning message is required
// remove PrintResetSource();
// volatile        char    *far    HC16RSR; removed
//
// P.Smith                      17/2/06
// put back in InitialiseMBTable call for modbus table set up.
// #include "MBProgs.h"
//
// P.Smith                      20/2/06
// put back in #include "ConfigurationFunctions.h"
// call load calibration data from eeprom.
//
// P.Smith                      20/2/06
// RightCornerCleanOff();  /* turn off right corner clean */
// LeftCornerCleanOff();   /* turn off left corner clean  */
// #include "Cleanio.h"
// implement pitweigh.cpp todos, #include "PITWeigh.h"
//
// P.Smith                      22/2/06
// g_nPreviousPITTime = PIT_FREQUENCY ->      g_nPreviousPITTime = BLENDER_PIT_FREQUENCY;
//
// P.Smith                      24/2/06
// InitialiseAlarmTable();
// #include    "Alarms.h"
//  InitialiseWeightVariables();
// #include "Weight.h"
// ResetKgHRoundRobin();               // RESRROB - Reset kg/h round robin.
// ResetGPMRoundRobin();               //ASM = RESRROB
//#include "Kghcalc.h"
//#include "Gpmcalc.h"
 // remove structCPIRRData g_WorkCPIRRData;
//
// P.Smith                      28/2/06
// ResetCleanStartMBBit();                             /* asm RESETCLEANBITS  */
// ResetCleanFinishMBBit();                            /* asm RESETCLEANFINISHBIT */
// #include "CycleMonitor.h"
//
// P.Smith                      19/4/06
// Remove ModbusMasterTest call.
// VacInhibitOn called
// make CheckLicensePowerUpStatus() & DecideOnLicenseExpiryWarning() --todolp--;     // ASM DECIDEONEXPIRYPAGE decide if a warning message is required
// #include "Pause.h"







//
// P.Smith                          27/4/06
// call modbus table initialisation, before checks are done on calibration data.

//
// P.Smith                          4/5/06
// set g_bFillStatCmp[i] = 0 on reset.
// set  g_nModbusMSWord = 0;, g_nModbusLSWord = 1;
//
// P.Smith                          12/5/06
// removed g_fDtoAPerKGH = 0;
//           g_fDtoAPerMPM = 0;
// set g_nModbusMSWord = 0; & g_nModbusLSWord = 1 before initalise of modbus table

//


//
// P.Smith                          15/5/06
// call  ClearCommsCounters();
// #include "MBTogSta.h"
// initialise g_OnBoardCalData.m_cPermanentOptions to 0x00ff;          // enable licensing.  sbb--testonly
//    g_CalibrationData.m_bStandardCompConfig = 1;            // standard config.   sbb--testonly
//   g_CalibrationData.m_nControlAvgFactor = 3;                                    //sbb--testonly
//    g_CalibrationData.m_nWeightAvgFactor = 1;                                     //sbb--testonly
//    g_CalibrationData.m_nBatchesInKghAvg = 3;

//    g_bSaveAllCalibrationToEEprom = TRUE;       /* set flag to cause all calibration data to be written to EEPROM.*/
//    CopyCalibrationDataToMB();      /* copy calibration data to MB. COPYCDMB  */

//
// P.Smith                          29/5/06
// removed     BOOL    bGoodData = FALSE;
// allow autocycler to be enabled on reset
//  g_bProgramLoopRunnning = FALSE;                                           //
// correct various warnings
//
// P.Smith                          8/6/06
// call    SetUpConversionFactors();
// set  g_nTSMDebug.m_bCycleDiagnostics to TRUE
// #include "Debug.h"
//
// M. McKiernan                     12/6/2006
// Moved  InitialiseMBTable() and  InitialiseAlarmTable(); to after configuration load from EEPROM.
//
// P.Smith                          13/6/06
// LoadRecipe(1);                                                                  //load recipe 5  to TempRecipe.
// PauseOn();    /* pause blender if not equal to 100 % */
// PutAlarmTable( PERALARM,  0 );      /* indicate alarm, Component no. is zero.  */
// RemoveAlarmTable( PERALARM,  0 );       /* clear % alarm., Component no. is zero. */
//
// P.Smith                          14/6/06
// call CalculateValveLatency();
// #include "BBCalc.h"
// set g_CurrentRecipe.m_nRegrindComponent to 1 on power up
//
// P.Smith                          19/6/06
// add check on regrind component no to determine if it is greater than the max
// no of components.
// set g_CalibrationData.m_nTopUpCompNoRef to 4
// check for topup and set to non standard component config for now.
// component 2 is the top up component.
// uncomment pause of blender on 100% alarm.
// call PrintResetSource();
// call  CalculateValveLatency();
//
// P.Smith                         21/6/06
// allow the top up to work for component 3, set non standard config to allow
// this to operate.
// set g_CalibrationData.m_fMaxBatchSizeInKg to 1.5kgs
//
// P.Smith                         28/6/06
// call IOBoardRTCGetTime( bts ); at end of initialise blender
// GetTime(); called
// GetDate(); called
// #include "I2CFuncs.h"
// copy of g_PowerUpTime[i] moved
// CheckForValidCalibrationData();
// set g_nTSMDebug.m_bMasterComms  & g_nTSMDebug.m_bNetworkComms to FALSE;
//
// P.Smith                         5/7/06
// call I2CInit before read of real time clock
//
// P.Smith                         10/7/06
// set g_nBatchesInKghAvgHistory = 0;
// set g_bSPIAtDActive & g_bSPIEEActive to FALSE;
//
// P.Smith                          18/7/06
// call  ReadDS2401() on reset
//
// P.Smith                          9/8/06
// set g_fComponentLongTermWeightResettableAccumulator to 0
//
// P.Smith                          20/9/06
// initialise g_bNBBRevB1Hardware to TRUE;
//
//
// P.Smith                          6/11/06
// init of g_bCycleIndicate, g_bAlarmOnFlag,_bAlarmOnTransitionFlag, g_bModbusLineSpeedWritten = FALSE;
// g_fCommsLineSpeedFrontLength,g_fCommsLineSpeedBackLength,g_bModbusDToAWritten
// g_bModbusDToAPercentageWritten
//  g_bNBBRevB1Hardware set to FALSE;
//
// P.Smith                          18/1/07
// set g_bPrintfRunning  &  g_bEnableTxRxInOpenSerial set to FALSE
//
// P.Smith                          5/2/07
// remove unused iprintf
//
// P.Smith                          6/2/07
// Remove set of m_nControlAvgFactor and other calibration data on reset
// set g_bRedirectStdioToPort2 to FALSE
//
// P.Smith                          6/3/07
// Transfer flow rate to g_ffComponentCPI,g_ffComponentCPIStage2,g_ffComponentCPIStage3
// Remove clear of g_ffComponentCPI, g_ffComponentCPIStage2 & g_ffComponentCPIStage3
//
// P.Smith                          12/3/07
// reset ultrasonic variables to zero.
//
// P.Smith                          12/3/07
// set g_nCycleSecondNo, g_nUSBufferIndex to 0,g_bUltrasonicLIWInstalled = FALSE;
//
// P.Smith                          23/4/07
// g_bRedirectStdioToPort2,g_bSoftwareUpdateRunning,g_bSDCardSPIActive initialsed to FALSE;
//
// P.Smith                          24/4/07
// added definitions of Multiblend variables.
//
// P.Smith                          30/4/07
// load recipe from RUNNINGRECIPEFILENO
//
// P.Smith                          2/5/07
// called LoadOutputMapping()
//
// P.Smith                          11/5/07
// Set m_nResetCPIRRCtr,g_bFlowRateChange set to false,g_nTSMDebug.m_bCycleDiagnostics set to false
//
// P.Smith                          11/5/07
// Set m_nResetCPIRRCtr,g_bFlowRateChange set to false,g_nTSMDebug.m_bCycleDiagnostics set to false

//
// P.Smith                          23/5/07
// g_nTSMDebug.m_bCycleDiagnostics set to TRUE,g_bStopSPIInterruptRunning,
// increment  g_CalibrationData.m_nResetCounter, set  g_CalibrationData.m_bDiagPrintFlag to TRUE
// g_bPITAllowed to TRUE  ensure that this is set every second.
// call DecideIfExpansionisRequired
//
// P.Smith                          2/6/07
// call CopyMultiblendRecipesToMB
//
//
// P.Smith                          2/6/07
// call CopyMultiblendSetupToMB on start up.
//
//
// P.Smith                          11/6/07
// CopyMultiblendRecipesToMB() & CopyMultiblendSetupToMB() called on reset
// g_nMultiBlendRecipeSaveCtr set to zero.
//
//
// P.Smith                          13/6/07
// set g_bOptimisationFlapStatus,g_bOptimisationLoaderEnable & g_bOptimisationLoaderFill to false
//
// P.Smith                          20/6/07
// g_bSaveAllCalibrationToEEpromOutsideCycle set to false
//
// P.Smith                                                 22/6/07
// name change to proper blender names.
// remove g_nCycleCounter, g_bSPIBusBusy set to FALSE, remove g_bStopSPIInterruptRunning
//
// P.Smith                                                 25/6/07
// call CheckFlowRateChecksum(); ,SetUpConversionFactors();
// g_OnBoardCalData.m_cPermanentOptions = 0xff;
// g_nFlashRate = FLASH_RATE_1HZ;              // set up to flash LED at 1Hz.
//
// P.Smith                                                 4/7/07
// Initialise all variables for optimisation
// remove set up of blender in standard config if not topup
//
// P.Smith                                                 27/7/07
// g_bFirstPulsesReadFromSEI set to TRUE
// g_bInitiatePurge,g_bBlenderPurged,g_bBlenderPurgeingInProgress set to false
// g_fWeightPerMeterFromLSpeed set to 0.0
//
// P.Smith                                                 31/7/07
// Set up g_nProtocol on power up.
//
// P.Smith                                                 27/8/07
// Set g_bMultiBlendRecipeIsZero to false for MULTIBLEND_EXTRUDER_NO
// call CheckForZeroMultiBlendRecipe
//
// M.McKiernan                      17/9/07
// Removed g_arrnMBTable[] & g_arrnMBTableFlags[], now in BatVars2
//
// P.Smith                          27/9/07
// switch top and mixer clean outputs off on reset, corner cleans are already
// being switched off.
//
// P.Smith                          15/10/07
// implement LoadAllCalibrationDataFromOnBoardEEprom for licensing etc.
// leave licensing out for now until kwh is updated.
// name change to cleaning m_nCleaning
//
// P.Smith                          15/10/07
// remove the set up of the licensing.
// initialise g_nAllowReadOfControlInputsCtr, g_nOverrideCounter ,g_bWriteToIniFileAgain
// Omit licensing for now by setting permanent license
// remove set of  g_arrnWriteSEIMBTable[MB_SEI_COMMAND] to 3, this set the sei in remove mode
// now done in handler in sei comms
//
// P.Smith                      15/11/07
// added set of g_bUsingSmallBatchSize to false
//
// P.Smith                      15/11/07
// Set g_bFirstTimeToReadScrewSpeed to TRUE
//
// P.Smith                      16/11/07
// Set g_bCalculateScrewSpeedConstant to FALSE;
// g_nVolumetricEstimateCounter set up to run estimte in n seconds.
//
// P.Smith                      21/11/07
// set g_bCheckOptimisationAgain to false for all components
// set g_bFirstOptimisationCheck to TRUE
//
// P.Smith                      23/11/07
// set g_bModbusScrewSpeedWritten to false
//
// P.Smith                      28/11/07
// if mode is MODE_MAXTHROUGHPUTZT, set the estimation counter to 0
// otherwise set it to 3.
// remove set of g_bDoCMRDToAPerKgCalculation to FALSE, this is set to true to allow
// the cmr d/a per kg to be calculated.
//
// P.Smith                      5/12/07
// g_nSuspendControlCounter
// g_nHoldOffLevelSensorAlarmSecondCounter,g_IncreaseOutputOn10hzCounter
// g_DecreaseOutputOn10hzCounter, g_nComponentPerDeviationErrorCounter all set to 0
// set g_nWriteHoldOffCounter to SEIWRITEHOLDOFFTIMEINSECONDS to hold off write to
// sei for n seconds.
//
// P.Smith                      9/1/08
// set  g_bResetComponentWeightsRoundRobin,g_bMultiBlendPaused,g_bMultiBlendisPaused to FALSE
//
// P.Smith                      10/1/08
// if g_CalibrationData.m_bPauseBlenderOnPowerup is set, pause the blender on power up
//
// P.Smith                      15/1/08
// set g_bSHAHardwareCheckFailed to false
//
// P.Smith                      22/1/08
// set g_bSaveUSBFileToUSB & g_bUSBDataWrite to FALSE;
//
// P.Smith                      23/1/08
// set g_bComponentHasFilledByWeight[] to false, set g_bFlowRateChange to false
//
// P.Smith                      25/1/08
// clear all statistical sample data
//
// P.Smith                      30/1/08
// call CheckForValidSoftwareIDHash, correct clear of g_fComponentWeightSamples
// comment out CheckForValidSoftwareIDHash
//
// P.Smith                      1/2/08
// call LoadRecipe before initialisation of modbus table.
// this ensures that the current recipe is loaded when copied to the
// modbus table.
// g_bPanelWriteHasHappened set to false <= to < for MULTIBLEND_EXTRUDER_NO
// and DOWNLOADHISTORYNO
// set g_arrnMBTableSetpointCopy array to 0
//
// P.Smith                      12/2/08
// initialise g_bSEIReadDataReady,g_nExtruderVolumetricDAValue,g_nCheckForCorrectDownloadCtr = 0;
//
// P.Smith                      12/2/08
// set g_nTSMDebug.m_bPrintfDebug to false
//
// P.Smith                      15/2/08
// set g_bReadSDCardDirectory to FALSE & g_nCopyFileNoToUSB to 0;
//
// P.Smith                      18/2/08
// set g_nUSBCopySequencer to 0
//
// P.Smith                      7/3/08
// set to 0,  g_nSEIWriteRequiredCounter,g_nControlLagHistory,g_nIncreaseRateHistory
// g_nDecreaseRateHistory,g_nControlDeadbandHistory,g_nFineIncDecHistory
//
// P.Smith                      26/3/08
// set g_bAllowSaveToEEPROM to TRUE, g_nSaveProcessDataToEEPROMCounter to 0
// LoadAllProcessDataFromEEprom called to load process data from eeprom.
// use g_ProcessData structure for m_ffComponentCPI,m_ffComponentCPIStage2,m_ffComponentCPIStage3
// set g_nSaveProcessDataToEEPROMCounter to allow data to be saved for the 1st few cycles.
// set g_bSaveAllProcessDataToEEprom & g_bAllowProcesDataSaveToEEPROM to TRUE
//
// P.Smith                      7/5/08
// initialise g_bAllowTransferToCurrentOrder to false
// call LoadRecipe(RUNNINGRECIPEFILENO) before doing 100% check
// call CalculateSetGramsPerMeterFromGramsPerSqMeter
// g_bSEIIncDecSetpointDownload set to false, g_nCurrentExtruderNo set to 1
// g_nCycleCounter seet to 0, clear g_arrnMBSetpointHistory array
//
// P.Smith                      17/5/08
// set g_bAllowMultiBlendCycle to FALSE;  //kwh
//
// P.Smith                      6/6/08
// set g_nSDCardSoftwareUpdateComplete = 0;
//     g_nSDCardLoadConfigComplete = 0;
//     g_nSDCardLoadBackupConfigComplete = 0;
//     g_nSDCardSaveConfigComplete = 0;
//     g_nSDCardCopyConfigToBackupComplete = 0;
//     set associated Bools to false
//
// P.Smith                     12/6/08
// reset g_nDownloadManagerNZTimeoutCtr, g_nDisableDTAPerKgCtr to 0
//
// P.Smith                     13/6/08
// set g_nNoLevelSensorAlarmCounter to 0, this allows the level sensor alarm to come
// in straight away.
//
// P.Smith                     17/6/08
// set g_bTelnetHistory to false
// check for g_bCalibrationDataInvalid or g_bProcessDataInvalid if set, generate
// an alarm
// Initialise g_nBatchCommsStatusSEI,g_nBatchCommsStatusLLS,g_nBatchCommsStatusOptimisation
// g_nBatchCommsStatusOptimisation,g_nBatchCommsStatusNetwork,g_nBatchCommsStatusTCPIP
// g_nBatchCommsSEITimeoutHistory,g_nBatchCommsLLSTimeoutHistory,g_nBatchCommsOptimisationTimeoutHistory
// g_nTCPIPReadHoldRegsCtr
// Initialise start up relay limit on sei by setting MB_SEI_STARTUP_RELAY_DA
// set g_nSEIWriteRequiredCounter to activate writeto sei
//
// P.Smith                     27/6/08
// set  g_CalibrationData.m_bTelnet,g_CalibrationData.m_bVacLoading = FALSE;
//
// P.Smith                     17/7/08
// initialise g_bToAutoEvent,g_bToManualEvent,g_CalibrationData.m_bLiquidEnabled
// g_cCurrentMonthHistory, g_nWriteConfigToSDInNSeconds,g_bWeighHopperCalibratedEvent
// g_bWeighHopperTaredEvent,g_bCleaningInitiatedEvent,g_nWriteConfigToSDInNSeconds
// g_nOperatorPanelPageId,g_nBlenderType, g_nSDCardDelayTime
// set blender type to BATCH
//
// P.Smith                     21/7/08
// Remove licensing, remove todo
//
// P.Smith                     23/7/08
// removed comms array g_PowerUpTime,g_CurrentTime externs
//
// P.Smith                     24/7/08
// set g_nDisableSDCardAccessCtr & g_nAMToggleStatusDelayCtr = 0;
//
// P.Smith                     14/8/08
// modified sofware to set g_CalibrationData.m_nCompConfig[7] to 0001 if
// in top up mode.
//
// P.Smith                     11/9/08
// added g_bWriteEventLogToSdCard set to false
// added g_cEventLogFile[0] = 0
//
// P.Smith                     16/9/08
// if liquid additive enabled,add in liquid additive % for 100% check.
// reset liquid additive variables and temperature
// clear g_bAlarmOccurredHistory arrays
//
// P.Smith                     3/10/08
// set g_fOneSecondHopperWeightAtEndOfCycle to 0
//
// P.Smith                     14/10/08
// g_fLongTermLengthCounter ,g_fLongTermResettableLengthCounter set to 0
// g_fKgperHour set to 0
//
// P.Smith                     17/11/08
// reset to 0
// g_bShowNetworkWrites, g_bWriteDiagnosticsToSDCard,g_bSDCardAccessInProgress
// g_nTelNetTimeoutCtr, g_nTelnetContinuousUpdate, g_nFileSegmentNo,g_bPrintCycleDataToTelnet = FALSE;
// g_lTCPIPCommsCounter, g_nTelNetSecondsCounter, g_bTelNetWindowCtr,g_nTelnetSequence
//
// P.Smith                     25/11/08
// g_bAToDMaxExceeded set to false
//
// P.Smith                     2/12/08
// clear g_fComponentWeightStdDevPercentage,g_bApplicationSPIInProgress
// g_bTaskSPIInProgress,g_bSDCardAccessInProgress set to false.
//
// P.Smith                     8/12/08
// add clearance of history log,   g_bHourlyResetMin = FALSE &   g_bHourlyResetMin = FALSE;
//
// P.Smith                     11/12/08
// remove m_nBaudRate,g_bTelnetSDCardAccessInProgress name change
// remove m_bDiagPrintFlag
//
// P.Smith                     12/12/08
// set g_nUSBTimeoutInPits to 0
// P.Smith                     7/1/09
// reset g_nHiRegLSCoveredCounter, g_nHiRegLSUnCoveredCounter

//
// P.Smith                     12/1/09
// reset g_nBlastMomentaryOnCtr,g_nExpansionOutput2, g_bWriteToExpansion2
//
// P.Smith                     21/1/09
// reset g_bResetNetworkSlaveComms ,g_bResetPanelSlaveComms,g_bAddAlarmOccurredToEventLog
// g_bAddAlarmClearedToEventLog, g_bAddAlarmOccurredToEventLog,g_bAddAlarmClearedToEventLog
//
// P.Smith                     22/1/09
// call CheckForValidSoftwareIDHash to check that the license software id is valid.
// comment out  g_OnBoardCalData.m_cPermanentOptions = 0xff
//
// P.Smith                     13/2/09
// clear usb debug flags, usb timeout counters
//
// P.Smith                     26/2/09
// reset g_sSelfTest and g_nTSMDebug structures as 1 instruction
//
// P.Smith                     26/3/09
// LoadAllDescriptionDataFromEEprom called to load serial numbers etc from eeprom
// this includes the nbb serial number and the panel software revision
// Reset g_bDoSelfTestDiagnostics, g_bInLastMinute,g_nTimeInAutoMinutesCtr
// g_wHourHistory, g_bHourRollOver
// increment m_n24HourOngoingResetCounter
// call CheckCalibrationHash to check the hash
// terminate panel string g_cPanelVersionNumber
// set g_nStorageInvervalCtr to 0
// clear ClearOngoing24HrHistoryStructures
//
// P.Smith                     24/4/09
// remove PrintResetSource from initialiseblender, this needs to be called
// after the sd card access
// call CopyEncryptionDataToMB to ensure that the licensing is up to date
// when the panel starts up.
// set g_CalibrationData.m_bLogFormatIsCSV  to false to disable csv format for now
// initialise   g_lOrigBatchPulsesAccumulator & g_bUseLineSpeedForGPMCalc
//
// P.Smith                     25/5/09
// added g_bNoLeakAlarmCheck,g_bHiRegLevelSensorHistory & g_bDumpFlapHasOpened set to false
//
// P.Smith                     25/5/09
// set g_bI2CError to false
//
// P.Smith                     8/6/09
// added g_CalibrationData.m_bCheckForValidComponentTargetPercentage set to false
//
// P.Smith                     10/6/09
// remove m_bCheckForValidComponentTargetPercentage set
//
// P.Smith                     17/6/09
// call LoadLoaderCalibrationDataFromEEprom
//
// P.Smith                     25/6/09
// call InitialiseVacSystem & CheckHashForLoader
//
// P.Smith                     1/7/09
// call CopyConfigDataToMB to copy all the config data to modbus at the same time.
// remove CopyEncryptionDataToMB
// remove CheckHashForLoader from bottom
// added CheckForValidLoaderConfigData and save loader config to eeprom immediately.
// added nCalDataValidResult for CheckForValidCalibrationData();
// initialise g_wForeGroundCounter,g_wProgramLoopEntry,g_bPrintModbusMessageToTelnet
//
// P.Smith                     22/7/09
// added CheckHashForPulsing and save data to eeprom if update is required.
// added LoadAdditionalDataFromEEPROM
// initialise g_bICSRecipeDataWritten, g_fComponentActualWeightHistory,g_bSaveAdditionalDataToEEPROM = FALSE;
// initialise m_wFillingMethod to 0 for now
// set g_nPulsingSequence to 0 for all comps
// g_bFastSettling set to false
//
// P.Smith                     25/8/09
// initialise g_bAcceptFlowRate & g_nValidFlowRateExceededCtr, g_bLockIsEnabled
// disable pulsing for now.
//
//
// P.Smith                     26/8/09
// initialise g_bPitRunning
//
// P.Smith                     1/9/09
// initialise g_bConfigIsLocked, g_cModbusEventBuffer & g_nHoldOffEventLogWriteCtr
//
// P.Smith                     2/9/09
// added init of g_bAtoDEndOfConversionErrorHistory, g_bAtoDNegativeSignHistory,g_nWaitForSPIBusToSettleCtr;
// g_bDoAToDTest;
// g_bLoadConfigDataCheckSumOkay holds result of LoadAllCalibrationDataFromEEprom();
// g_bLoadProcessDataCheckSumOkay holds LoadAllProcessDataFromEEprom
// remove LoadAllDescriptionDataFromEEprom
// g_bLoadLoaderConfigDataCheckSumOkay holds result of LoadLoaderCalibrationDataFromEEprom
// name change   g_fCycleLoadTimePrn, g_lLoadTime
//
// P.Smith                     10/9/09
// added LoadAllDescriptionDataFromEEprom, this was stopping the serial no from being read
// set g_nBlenderTypeHistory to g_CalibrationData.m_nBlenderType
// set g_fCalibrationWeight to 0
// set g_bSelfTestInProgress to false
//
// P.Smith                     15/9/09
// initialise  g_nPulseTimeForMinWeight,g_fPulseWeight,g_bGotHalfGramTime,g_nHalfGramTime
// g_fRequiredPulseWeightInGrams
// put back in check for 998 throughput
//
// P.Smith                     15/9/09
// added   g_fCurrentRecipePercentageHistory[i], clear g_fRequiredPulseWeightInGrams
// g_nPulseTimeForMinWeight,g_nHalfGramTime as component related
//
//
// P.Smith                     17/9/09
// added check for license vacuum loading option, if disable set no of loaders to 0
// if throughput 2758 enable pulsing on component 3.
//
// P.Smith                     29/9/09
// initialise g_nLoadersHistory
//
// P.Smith                     12/10/09
// added initialisation of g_fMaxKgPHrPossible & g_nAfterEndOfCycleCounter
//
// P.Smith                     16/10/09
// initialise g_nVAC8ExpansionIndex, g_nVAC8ExpansionOutput,g_nVaccumLoadingVAC8IOCards
// remove enable of pulsing
//
// P.Smith                     22/10/09
// remove m_bLogFormatIsCSV set to false
//
// P.Smith                     16/11/09
// added reset g_lSettledAverageCounts[i], g_nAToDStabilityLog[i]
// added reset g_bLogLoadCellReading, g_nDelayBeforeLoadCellLogCtr,g_nAToDStabilityCtr
// g_nAToDStabilityMaxValue, g_nAToDStabilityMinValue = 0;
//
// P.Smith                     19/11/09
// reset g_nAToDStabilityLog[i],g_nAToDRefStabilityLog[i] = 0;
// reset g_bTakeRefAToDReading,g_nDelayBeforeRefLogCtr,g_nAToDRefStabilityCtr,g_nAToDRefStabilityMaxValue
// g_nAToDRefStabilityMinValue,g_bRefAToDStabilityTestInProgress,g_bDecideOnRefStability
// g_bDecideOnLoadCellStability,g_nRefTimeoutCtr
// initialise g_nVac8ExpansionHistory to g_CalibrationData.m_bVac8Expansion;
// remove  g_CalibrationData.m_bVac8Expansion set to TRUE
//
// P.Smith                     27/11/09
// remove "FlowRRobin.h"
// remove set of non standard config for top up component.
// set g_nTopUpCompNoRefHistory to g_CalibrationData.m_nTopUpCompNoRef;
//
// P.Smith                     14/12/09
// compiler warning corrected by settings  nCompNo,nTemp to unsigned int.
//
// P.Smith                     6/1/10
// reset g_fWeightLeftAfterRegrind & g_fRegrindWeight
//
// P.Smith                     11/1/10
// added g_bFileCouldNotBeOpenedEvent reset
//
// P.Smith                     15/1/10
// CheckHashForSDCardErrorCtr, if g_bSaveAllProcessDataToEEprom set,
// save process data to eeprom.
//
// P.Smith                     27/1/10
// reset g_bComponentHasRetried
//
// P.Smith                     17/2/10
// call CheckHashForEthernetIP, save process data to eeprom
// and process data to onboard eeprom.
// set g_bEIPSoftwareEnabled is license has been enabled.
//
// P.Smith                     5/3/10
// removed commented license check for cleaning
//
// P.Smith                     8/3/10
// removed CheckHashForEthernetIP
// moved set/clear of g_bEIPSoftwareEnabled further down so that it is done after
// the licensing options have been set up in software id hash check
//
// P.Smith                     15/3/10
// set g_bVenturiFillByWeight to false to ensure that top up is initiated
// the first time.
//
// P.Smith                     25/3/10
// call ReadSHAHashes
//
// P.Smith                     26/3/10
// added ReadDS2432ID and AssembleSecretData
//
// P.Smith                     6/4/10
// reset g_nWatchDogCommand,g_nWatchDogCommandInNseconds
//
// P.Smith                     27/4/10
// call CheckHashHistoryLog, after load cal data from eeprom check if data
// should be saved to eeprom
//
// P.Smith                     8/6/10
// added g_wHourHistory initialisation to ensure that the hourly transition is not picked up initially.
//
// P.Smith                     16/9/10
// remove 100% check, this is not valid for the width control.
//
// P.Smith                     27/10/11
// check g_CalibrationData.m_bPCF8563RTCFitted to determine what rtc should be read.
//
// M.McKiernan					23/4/2020
// After loading the calibration data, fix machine type to Width.
//    g_bLoadConfigDataCheckSumOkay = LoadAllCalibrationDataFromEEprom();
//wfh - 23.4.2020.
//    g_CalibrationData.m_nMachineType = MACHINE_TYPE_WIDTH_CONTROL;
// 24.4.2020
// After loading recipe, check the offset is sensible.
//  if( (g_CurrentRecipe.m_fDesiredWidthOffset > 30.0f) || (g_CurrentRecipe.m_fDesiredWidthOffset < -30.0f) )
//	   g_CurrentRecipe.m_fDesiredWidthOffset = 0.0f;
//   wfh ...  remove this second call of LoadRecipe.  TODO
//
//
/////////////////////////////////////////////////////////////


//*****************************************************************************
// INCLUDES
//*****************************************************************************


#include "predef.h"
#include <stdio.h>
#include <ctype.h>
#include <startnet.h>
#include <autoupdate.h>

#include "16R40C.h"
#include "SerialStuff.h"
#include "General.h"
#include "ConversionFactors.h"

#include "InitialiseHardware.h"
#include "ConfigurationFunctions.h"
#include "MBMHand.h"
#include "TimeDate.h"

#include "MBProgs.h"
#include "BatchCalibrationFunctions.h"
#include "SetpointFormat.h"
#include "BatchMBIndices.h"

#include "BatVars.h"
#include "BatVars2.h"
#include "InitBlnd.h"
#include "PITWeigh.h"
#include    "Alarms.h"
#include "MBMaster.h"
#include "TSMPeripheralsMBIndices.h"
#include "Kghcalc.h"
#include "Gpmcalc.h"
#include "Blrep.h"
#include "Pause.h"
//nbb #include "ProgLoop.h"
//nbb #include "CsumCalc.h"
#include <String.h>
#include "CycleMonitor.h"
#include "Cleanio.h"
#include "PrntDiag.h"
#include <basictypes.h>
#include "NBBGpio.h"
#include "Weight.h"
#include "MBTogSta.h"   //
#include "Debug.h"
#include "BBCalc.h"
#include "I2CFuncs.h"
#include <autoupdate.h>
#include "OneWire.h"
#include "UltrasonicRead.h"
#include    "Mtttymux.h"
#include    "Mapping.h"
#include    "Expansion.h"
#include    "Multiblendmb.h"
#include    "Monbat.h"
#include    "StoreCommsRecipe.h"
#include "Hash.h"
#include "rta_system.h"
#include "Liquidadditive.h"
#include "HistoryLog.h"
#include "Acumwt.h"
#include "MBProsum.h"
#include "Vacldee.h"
#include "Vacseq.h"
#include "VacVars.h"
#include "License.h"
#include "security.h"
#include "InitNBBComms.h"
#include "WidthDef.h"


/* Externally defined variables  */
extern  structControllerData    *arrpDeviceList[];
extern  structCommsData                 g_sModbusMaster;
extern  structMeasurementUnits  g_MeasurementUnits;
extern  int             g_nKeyCode;
extern  int             g_nLayerCount;
extern  int             g_nLanguage;
//extern  char    g_PowerUpTime[];
extern  int             g_nGeneralTickTimer;
extern  int     g_nVAC350MBCalUpdateTimer;
extern  int     g_nBlendersInGroup[];
extern  char    g_cProcessType;
extern  int g_arrnWriteUFP2MBTable[];
extern  int g_arrnWriteUFP2MBTable[];
extern  int g_arrnReadUFP2MBTable[];
extern  StructDebug   g_nTSMDebug;
extern  BYTE *pDS2432Buffer;






char    g_cResetStatusRegister;

// Locally declared global variables

extern  char            g_cGroupSelected;
extern  char            g_cGroup2Description[];
extern  char            g_cGroup1Description[];

extern  structSetpointData      g_TempRecipe;
extern  structSetpointData      g_CurrentRecipe;
extern CalDataStruct    g_CalibrationData;
extern  OnBoardCalDataStruct   g_OnBoardCalData;

extern  structRollData      g_FrontRoll;
extern  structRollData      g_BackRoll;
extern  structRollData      g_Order;
extern  structRollData      g_CurrentShiftFront;
extern  structRollData      g_OngoingShift;
extern  structCPIRRData     g_WorkCPIRRData;
OnBoardCalDataStruct   g_OnBoardCalData; //nbb--todolp--
BOOL g_bLoadConfigDataCheckSumOkay = FALSE;
BOOL g_bLoadProcessDataCheckSumOkay = FALSE;
BOOL g_bLoadLoaderConfigDataCheckSumOkay = FALSE;


extern  structCPIRRData     g_MainCPIRRData;
extern  structCPIRRData     g_Stage2CPIRRData;
extern  structCPIRRData     g_Stage3CPIRRData;
extern  struct tm bts;
extern int g_nFlashRate;
extern  int g_nProtocol;
extern bool g_bEIPSoftwareEnabled;
extern unsigned int g_nWatchDogCommand;
extern unsigned int g_nWatchDogCommandInNseconds;


float g_fMyWOffset = 1.1f;
WORD  g_nMyWOffsetSign = 22;




//////////////////////////////////////////////////////
// InitialiseBlender( void )                    from ASM = no direct equivalent. stuff from BLBGND - PGLOOP
// Initialisation for batch blender. Called on reset.
// Entry: none
//
// Exit: none
//
// M.McKiernan                                                  14-07-2004
// First pass.
//
//////////////////////////////////////////////////////
void InitialiseBlender( void )
{
    unsigned int     nCompNo,nTemp;
    int  nCalDataValidResult;
    unsigned int i;
    float   fPercentageTotal;
    BOOL  bVacCalibrationDataIsValid;

        char cBuffer2[COLUMNS/2];
        // Load and initialise the configuration
          // any errors returned in failure word.

//    g_nLoadConfigurationFailure = LoadConfiguration();


    g_bLoadConfigDataCheckSumOkay = LoadAllCalibrationDataFromEEprom();
//wfh - 23.4.2020.
    g_CalibrationData.m_nMachineType = MACHINE_TYPE_WIDTH_CONTROL;

    LoadAllCalibrationDataFromOnBoardEEprom();
    g_bLoadProcessDataCheckSumOkay = LoadAllProcessDataFromEEprom();
    LoadAllDescriptionDataFromEEprom();
    g_bLoadLoaderConfigDataCheckSumOkay = LoadLoaderCalibrationDataFromEEprom();
  // check if Vac loader data is valid
    bVacCalibrationDataIsValid = CheckForValidLoaderConfigData();
    CheckHashForLoader();
    if( g_bSaveLoaderCalibrationData || !bVacCalibrationDataIsValid )
    {
      g_bSaveLoaderCalibrationData = FALSE;
      SaveLoaderDataToEEprom();
    }
    CheckHashForPulsing();
    if(g_bSaveAllCalibrationToEEprom)
    {
        g_bSaveAllCalibrationToEEprom = FALSE;
        SaveAllCalibrationDataToEEprom();
    }

    CheckHashForSDCardErrorCtr();
    CheckHashHistoryLog();
    if(g_bSaveAllProcessDataToEEprom)
    {
        g_bSaveAllProcessDataToEEprom = FALSE;
        SaveAllProcessDataToEEprom();
    }
    //CheckHashForEthernetIP();
    if(g_bSaveAllCalibrationToOnBoardEEprom)
    {
        g_bSaveAllCalibrationToOnBoardEEprom = FALSE;
        SaveAllCalibrationDataToOnBoardEEprom();
    }




    for(i= 0; i < MAX_COMPONENTS; i++)
    {
        g_ffComponentCPI[i] = g_ProcessData.m_ffComponentCPI[i];
        g_ffComponentCPIStage2[i] =  g_ProcessData.m_ffComponentCPIStage2[i];
        g_ffComponentCPIStage3[i] =  g_ProcessData.m_ffComponentCPIStage3[i];
    }


//      SetupMttty();
//      iprintf("\n before check for valid cal load std config is %d",g_CalibrationData.m_bStandardCompConfig);

        nCalDataValidResult = CheckForValidCalibrationData(TRUE);
        if(g_bSaveAllCalibrationToEEprom)
        {
            g_bSaveAllCalibrationToEEprom = FALSE;
            SaveAllCalibrationDataToEEprom();
        }

//
//      SetupMttty();
//      iprintf("\n after check for valid cal load std config is %d",g_CalibrationData.m_bStandardCompConfig);

   LoadRecipe(RUNNINGRECIPEFILENO); 	// -> loads into Temp Recipe                                                                 //load recipe 5  to TempRecipe.
   //LoadAdditionalDataFromEEPROM();
   //wfh
   // transfer the setpoints  into the used (current) setpoint area.
   memcpy(&g_CurrentRecipe, &g_TempRecipe, sizeof( g_TempRecipe ) );
   //validity check on offset.
   if( (g_CurrentRecipe.m_fDesiredWidthOffset > 0.0f) && (g_CurrentRecipe.m_fDesiredWidthOffset < 30.0f) )
	   ;
   else
	   g_CurrentRecipe.m_fDesiredWidthOffset = 0.0f;
   if((g_CurrentRecipe.m_wUSLayflatOffsetSign != NEGATIVE_SIGN) && (g_CurrentRecipe.m_wUSLayflatOffsetSign != POSITIVE_SIGN) )
		   g_CurrentRecipe.m_wUSLayflatOffsetSign = POSITIVE_SIGN;

   g_fMyWOffset = g_CurrentRecipe.m_fDesiredWidthOffset;
   g_nMyWOffsetSign = g_CurrentRecipe.m_wUSLayflatOffsetSign;
   //if(((g_stWidthCalibration.m_bStandardiseOnAuto)!= TRUE) && (g_stWidthCalibration.m_bStandardiseOnAuto != FALSE))
/*        SetupMttty();
        // Check percentages (not including regrind component) add up to 100%
        for( i = 0; i < g_CalibrationData.m_nComponents; i++ )
        {
                 printf("\n percentage for comp %d is    %2.1f",i+1,g_TempRecipe.m_fPercentage[i]); //nbb--testonly--
        }
*/

   InitialiseMBTable();                            // initialise the ModBus table. // must be called here

    InitialiseAlarmTable();
    // Initialise weight/AD variables.


    InitialiseWeightVariables();
    // Initialise batch or cycling variables.
    // Do after eeprom calib data load.
    InitialiseBatchVariables();


    if( g_CalibrationData.m_bPauseBlenderOnPowerup )
    {
        PauseOn();  // Pause blender
    }

//--REVIEW--
//        LoadRecipe(RUNNINGRECIPEFILENO);                                                                  //load recipe 5  to TempRecipe.

//        SetupMttty();
        // Check percentages (not including regrind component) add up to 100%
//        for( i = 0; i < g_CalibrationData.m_nComponents; i++ )
//        {
//            printf("\n density is for comp %d is    %2.1f",i,g_TempRecipe.m_fDensity[i]); //nbb--testonly--
//        }

/*        SetupMttty();
        // Check percentages (not including regrind component) add up to 100%
        for( i = 0; i < g_CalibrationData.m_nComponents; i++ )
        {
                 printf("\n percentage for comp %d is    %2.1f",i+1,g_TempRecipe.m_fPercentage[i]); //nbb--testonly--
        }
*/

/*   wfh ...  remove this second call of LoadRecipe.
        LoadRecipe(RUNNINGRECIPEFILENO);                                                                  //load recipe 5  to TempRecipe.
//        SetupMttty();
        fPercentageTotal = 0.0;
        for( i = 0; i < g_CalibrationData.m_nComponents; i++ )
        {
                      nCompNo = i+1;
              if(nCompNo != g_TempRecipe.m_nRegrindComponent )      // sum if not regrind
                 fPercentageTotal += g_TempRecipe.m_fPercentage[i];
//                 printf("\n percentage for comp %d is    %2.1f",nCompNo,g_TempRecipe.m_fPercentage[i]); //nbb--testonly--
        }

          // verify that the total is acceptable
        if( fPercentageTotal < 99.94f || fPercentageTotal > 100.06f )    // asm CHK100 //
        {
                // Set component 1 to 100%.
            for( i = 0; i < g_CalibrationData.m_nComponents; i++ )
            {
                g_TempRecipe.m_fPercentage[i] = 0.0;
            }
            g_TempRecipe.m_fPercentage[0] = 100.0;
//            PauseOn();    // pause blender if not equal to 100 % //
//            PutAlarmTable( PERALARM,  0 );      // indicate alarm, Component no. is zero.  //

        }
        else
        {
            if( g_arrnMBTable[BATCH_SUMMARY_PROCESS_ALARMS] & PERALBIT ) // percentage alarm bit set???  //
            {
                RemoveAlarmTable( PERALARM,  0 );       // clear % alarm., Component no. is zero. //
            }
        }

        // transfer the setpoints  into the used (current) setpoint area.
        memcpy(&g_CurrentRecipe, &g_TempRecipe, sizeof( g_TempRecipe ) );
*/ //end wfh
        g_bPctFlag = TRUE;      // cause calculations to happen   //
        g_bCSAFlag = TRUE;

        g_TempRecipe.m_nFileNumber = 5;


        sprintf( cBuffer2, "abcdefghifklmn");
        for(i = 0; i<10; i++)
        {
                g_TempRecipe.m_arrCustomerCode[i]       = cBuffer2[i];
        }
        sprintf( cBuffer2, "ABCDEFGHIJKLMN");
        for(i = 0; i<10; i++)
        {
                g_TempRecipe.m_arrMaterialCode[i]       = cBuffer2[i];
        }
        sprintf( cBuffer2, "1234567890");
        for(i = 0; i<10; i++)
        {
                g_TempRecipe.m_arrOrderNumber[i]        = cBuffer2[i];
        }


       // --test-- --REVIEW--  --SCI--
        g_CalibrationData.m_nPrinterType = 1;       // serial print option.
        //g_CalibrationData.m_nBaudRate = BAUD_RATE_9600_ID;   // nbb--testonly--this is now set from the panel
        g_CalibrationData.m_nDataFormat = 2;        // even parity.



            // --REVIEW-- does what CALM did.  CALM was called on entry to the Calibration menu.
            g_nStageFillEnHistory = g_CalibrationData.m_nStageFillEn;       // STAGEFILLENH
            g_nStageFillEnTemp = g_CalibrationData.m_nStageFillEn;      // STAGEFILLENTEMP
            g_bLayeringTemp = g_CalibrationData.m_bLayering;

/*
CALM   LDAA     STAGEFILLEN
       STAA     STAGEFILLENH         ; STORE HISTORY.
       STAA     STAGEFILLENTEMP
       LDAA     LAYERING
       STAA     LAYERINGTEMP
*/

// --testonly-- ps   // set extruder d/a to value on power up.
//    g_nExtruderDAValue = 2000;  //
    g_nDACPerKgDoneFlag = TRUE;  // --review this flag may not be necessary ?
    g_arrnWriteSEIMBTable[MB_SEI_DA] = g_nExtruderDAValue;
    g_CurrentRecipe.m_fDeadbandForTargetWeight = 10.0;


        g_bResetFlag = TRUE;                            //RESETFLG
        g_bResetADFLag = TRUE;
        g_bFirstBatch = TRUE;   //SET 1ST BATCH FLAG
        g_bFirstWCycle = TRUE;  // Indicate first weigh cycle.
        CloseDump();                            // close dump flap.
        CloseAll();       // close all feed valves

        g_nPreviousPITTime = BLENDER_PIT_FREQUENCY;
        g_cBatSeq = BATSEQ_LEVPOLL;     // initialise to the start polling level sensor

        //asm = CHKDIA
        g_bOutputDiagMode = FALSE;
        if(g_CalibrationData.m_nDiagnosticsMode == AUTOCYCLE_PROGRAM)
        {
           g_bAutoCycleFlag = TRUE;        // ENABLE autocycle program. //nbb--testonly--
//           iprintf("\nautocycler is on");
       }

        else
        {
//           iprintf("\nautocycler is off");
            g_bAutoCycleFlag = FALSE;        // ENABLE autocycle program.
            if(g_CalibrationData.m_nDiagnosticsMode >= TEST_PROGRAM_1)
            {
                g_bOutputDiagMode = TRUE;
            }
        }

//CONRESET:

//nbb--removed--        FormSeqTable();  // ************testonly***************
        g_bFstCompCalFin = FALSE;


        SetUpDiagnostics();         //asm SETUPDIAGPTR

        if(g_bAutoCycleFlag)
        {
            g_cBatSeq = BATSEQ_AUTOCYCLETARE;
            g_bAutoCycleFlag = FALSE;                                                               // testonly //
        }
        if(g_CalibrationData.m_nBlenderMode == MODE_OFFLINEMODE)
        {
            CloseFeed();            /* asm CLOSEFEED  */
            g_nOffline_Tracking = BATSEQ_OFFDELADMPID;  /* OFFLINE_TRK = OFFDELADMPID   */
        }

        g_bDoCMRDToAPerKgCalculation = TRUE;        /* asm CMRNOW  */



        MixerOn();
        g_bMixOff = FALSE;               //

       ResetKgHRoundRobin();               // RESRROB - Reset kg/h round robin.
       ResetGPMRoundRobin();               //ASM = RESRROB

        /* merge asm CHECKSTDSETUP inline    */
        if(g_CalibrationData.m_bStandardCompConfig)
        {
            nTemp = 1;
            for(i=0; i<12; i++)
             {
                g_CalibrationData.m_nCompConfig[i] = nTemp;     /* set component used */
                nTemp <<= 1;                                    /* shift left by 1 */
             }
            g_bSaveAllCalibrationToEEprom = TRUE;
        }

       // if(g_CalibrationData.m_bTopUpMode)  //nbb--todo--put back
       // {
       //     g_CalibrationData.m_bStandardCompConfig = FALSE;   //nbb--testonly--             g_CalibrationData.m_nCompConfig[0] = 0x0001;
       //     g_CalibrationData.m_nCompConfig[1] = 0x0002;
       //     g_CalibrationData.m_nCompConfig[2] = 0x0004;
       //     g_CalibrationData.m_nCompConfig[3] = 0x0008;
       //     g_CalibrationData.m_nCompConfig[4] = 0x0010;
       //     g_CalibrationData.m_nCompConfig[5] = 0x0020;
       //     g_CalibrationData.m_nCompConfig[6] = 0x0040;
       //     g_CalibrationData.m_nCompConfig[7] = 0x0001;
       //     g_CalibrationData.m_nCompConfig[8] = 0x0100;
       //     g_CalibrationData.m_nCompConfig[9] = 0x0200;
       //     g_CalibrationData.m_nCompConfig[10] = 0x0400;
       //     g_CalibrationData.m_nCompConfig[11] = 0x0800;
       //     g_bSaveAllCalibrationToEEprom = TRUE;
       //}


        if(g_CalibrationData.m_nBlenderMode == MODE_MAXTHROUGHPUTZT)
        {
            g_nEstCtrUse = 0;               // no estimation
        }
        else
        {
            g_nEstCtrUse = 3;               // Use estimation for 1st few.
        }
        g_bRunEst = FALSE;


        g_lHopperTareCounts = g_CalibrationData.m_lTareCounts;  /* set to tare from EEPROM. */

        g_bTopUpFlag = g_CalibrationData.m_bTopUpMode;          /* set topup flag as per eeprom value */

        ResetCleanStartMBBit();                             /* asm RESETCLEANBITS  */
        ResetCleanFinishMBBit();                            /* asm RESETCLEANFINISHBIT */

        BlenderTopCleanOff();
        BlenderMixerCleanOff();

        RightCornerCleanOff();  /* turn off right corner clean */
        LeftCornerCleanOff();   /* turn off left corner clean  */


        /* merge CHECKFORCLEANOPTION inline no need for CALCULATEEEPROMCHECKSUM  */

     InitialiseVacSystem();        // Integrated Vacuum loading.

// Initialise and configure the Modbus master module.
        InitialiseModbusMaster();
        ConfigureModbusMaster();


          g_nBlenderReadyCounter = BLENDERREADYTIME;  /* ASM BLENDERREADYCTR  */
          g_nNoLevelSensorAlarmCounter = IGNORELSALARMNO; /* NOLSALARMCTR     */

    if(g_CalibrationData.m_nBlenderMode == MODE_SINGLERECIPEMODE)
    {
        g_bCheckIfVacuumIsRequired  = TRUE;
        VacInhibitOn();      // ASM VACINHIBITON  sets vacuum inhibit in pause flag
        OpenFeed();
    }
    else
    {
        g_bCheckIfVacuumIsRequired  = FALSE;
    }

//nbb--todolp-- License.cpp   CheckLicensePowerUpStatus(); // ASM CHECKPOWERUPSTATUS check power up status for license
//nbb--todolp-- License.cpp   DecideOnLicenseExpiryWarning();     // ASM DECIDEONEXPIRYPAGE decide if a warning message is required
    g_nControlAvgFactorGpmHistory = g_CalibrationData.m_nControlAvgFactor;
    g_nControlAvgFactorKghHistory = g_CalibrationData.m_nControlAvgFactor;

    I2CInit( 0, I2C_FREQ_DIV );	 //
    if(!g_CalibrationData.m_bPCF8563RTCFitted)
    {
     	IOBoardRTCGetTime( bts );
   	}
    else
    {
    	PCF8563RTCGetTime( bts );
    }

    GetTime();
    GetDate();

    if(g_bCalibrationDataInvalid || g_bProcessDataInvalid)
    {
        if( (g_arrnMBTable[BATCH_SUMMARY_PROCESS_ALARMS] & CALINVALARMBIT) == 0 ) // tare alarm bit not set???
        {
            PutAlarmTable( CALINVALARM,  0 );     /* indicate alarm, PTALTB   */
        }
    }
    else
    {
        if( (g_arrnMBTable[BATCH_SUMMARY_PROCESS_ALARMS] & CALINVALARMBIT) ) // tare alarm bit set???
        {
            RemoveAlarmTable( CALINVALARM,  0 );      /* indicate alarm cleared, RMALTB */
        }
    }



    ReadDS2432ID( pDS2432Buffer );       //testing 1-wire.

    for(i= 0; i < MAX_COMPONENTS; i++)
    {
        g_bLowLevelSensorStatusHistory[i] = COVERED;
    }

    if(g_CalibrationData.m_nControlType == CONTROL_TYPE_INC_DEC) // asm = INCREASEDECREASE
    {
        g_nDoIncDecReadInNSeconds = 4;                  /* will read them back in 4 seconds asm INITIATEINCDECWRRD */
    }

    for( i = 0; i < TIME_ARRAY_SIZE; i++ )   ////
    {
        g_PowerUpTime[i] = g_CurrentTime[i]; //
    }


    if(g_CurrentRecipe.m_nRegrindComponent > MAX_COMPONENTS)
    {
        g_CurrentRecipe.m_nRegrindComponent = 0;
    }
    g_bSaveAllCalibrationToEEprom = TRUE;       /* set flag to cause all calibration data to be written to EEPROM.*/
    CheckForValidSoftwareIDHash();

    g_nBlenderType = BATCH;        // set blender type to batch

    CopyConfigDataToMB();      /* copy calibration data to MB. COPYCDMB  */
    CalculateSetGramsPerMeterFromGramsPerSqMeter();
    if((g_OnBoardCalData.m_cPermanentOptions & LICENSE_ETHERNETIP) != 0)
    {
    	g_bEIPSoftwareEnabled = TRUE;
    }
    else
    {
    	g_bEIPSoftwareEnabled = FALSE;
    }
    g_wHourHistory = g_CurrentTime[TIME_HOUR];
}

//////////////////////////////////////////////////////
// InitialiseBatchVariables( void )                     from ASM = no direct equivalent.
// Initialisation for batch variable data. Called on reset.
// Entry: none
//
// Exit: none
//                                               l
// M.McKiernan                                                  14-07-2004
// First pass.
//////////////////////////////////////////////////////
void InitialiseBatchVariables( void )
{
        unsigned int i,j;

        g_nFillStage = STAGE_0;                   // fill stage indication
        g_bMultipleFilling = FALSE;          //

          g_bActivateLatency = FALSE;
          g_nValveLatencyTime = 0;

            g_lTimeMeasured = 0;

        g_bAutoCycleTestFlag = FALSE;
            g_nLastCompTableIndex = 0;

        g_bRegrindPresent = FALSE;
            g_bFstCompCalFin = FALSE;
            g_bFillAlarm = FALSE;
         g_nAnyFillAlarm = FALSE;
            g_nDmpSec = 0;

         g_bStopTransition = FALSE;
         g_bRunTransition = FALSE;
         g_bCycleIndicate = FALSE;
         g_bStopped = FALSE;
         g_nRunCounter = 0;
         g_nStopCounter = 0;
         g_bStopRunDisplayEnable = TRUE;

        g_lComponentOnTimer = 0; // CMPNTIM
          g_cCalcRouter = 0;
          g_nAvgCtr = 0;
         g_nShutDownCtr = 0;
         g_nHGSen1Lag = 0;
         g_bDmpLevSen = COVERED;
         g_bLevSen1Stat = COVERED;
           g_bPrevLevSen1Stat = g_bLevSen1Stat;
         g_nPauseFlag = 0;
         g_bPauseTransition = FALSE;
         g_bOffLSF = FALSE;
         g_nLevelSensorDelayCtr = 0;
         g_nHGSen1Lag = 0;
         g_nNoCycle = 0;
         g_bDoorOpen = FALSE;
         g_nLevSenOffTime = 0;
         g_cLS1OffCt = 0;

            g_nAfterOpenCounter = 0;
            g_nCleanCycleSeq = 0;
            g_nBlenderCleanRepetionsCounter = 0;
            g_nMixerCleanRepetionsCounter = 0;
            g_nDumpRepetitions = 0;
            g_nAirJet1OnCounter = 0;

            g_nPremLSCtr = 0;

            g_bLevelSensorAlarmOccurred = FALSE;

            g_nSelectedRecipeNo = 1;  // --REVIEW--

            g_bPauseToNormal = FALSE;
            g_nFillCtr = 0;
            g_nFillCounter = 0;     // Fill active counter, inc'ed on 1Hz.
            g_cRetryActive = 0;
            g_nRetryCounter = 0;
            g_nTopUpSequencing = 0;
            g_bMultipleFilling = FALSE;
            g_bTopUpFlag = FALSE;   // indicate no top up.
            g_bNoTopUpRequired = FALSE;
            g_bMoveOn = FALSE;
            g_bChangingOrder = FALSE;
            g_nCorrectRetries = 0;

            g_cCompNo = 1;
            g_cCompNoDetails = 1;
            g_nAnyRetry = 0;

         g_nCyclePauseType = 0;
         g_bPrintNow = FALSE;
         g_nPrintInNSeconds = 0;

         g_fCycleLoadTimePrn = 0.0;
         g_lLoadTime = 0;
         g_nCycleTimePrn = 0;

         g_nTransferSetpointsFlag = 0;

         g_ffCPI = 0.0; // any value at start.
         g_ffCPI_RR = 0.0; // any value at start.

            g_bTaringStatus = FALSE;
            g_bAnyFillByWeight = FALSE;
        g_bResetStageSeq = FALSE;               // reset stage sequence

            g_nOffline_Tracking = 0;
            g_bDisableControlAction = FALSE;
            g_bBatchReady = FALSE;
            g_nOffTime1 = 0;

            g_nVolCycCtr = 0;               // volumetric cycle counter.
            g_nRetryCompNoHis = 0;
            g_bPctHis = FALSE;
            g_bAnyOn = FALSE;

         g_bFillTargetEntryFlag = FALSE;

            g_bRegrindFillProblem = FALSE;
            g_bNoFlowRateCalculation = FALSE;
            g_bUnderFill = FALSE;

        g_nStageFillEnTemp = FALSE;             // stage fill enable temp
        g_bLayeringTemp = FALSE;            //
        g_bStageFillCalc = FALSE;           // initiates 1st stage calculation
//07.09.2004
            g_nCheckLSensorInNSeconds = 0;     // CHECKLSENSORINNSECONDS  DS      1       ; CHECK LLS
            g_bNoFillOnClean = FALSE;          // NOFILLONCLEAN           DS      1       ; NO FILL ON CLEAN
         g_bPhaseOneFinished = FALSE;       // PHASEONEFINISHED        DS      1       ; INDICATION WHEN PHASE 1 CLEAN IS FINISHED

            g_nLeftCornerCleanOnCounter = 0;   // CCLONCTR                DS      2       ; LEFT CC
            g_nRightCornerCleanOnCounter = 0;  // CCRONCTR                DS      2       ; Right CC
            g_nAirJet2OnCounter = 0;           // AIRJET2ONCTR
//09.09.2004

            g_bHighLevelSensor = UNCOVERED;           // HIGHLEVELSENSOR DS      1       ; HIGH LEVEL SENSOR.
            g_bHighLevelSensorHistory = UNCOVERED;    // HIGHLEVELSENSORHIS DS      1       ; HIGH LEVEL SENSOR.
            g_bLowLevelSensor = UNCOVERED;            // LOWLEVELSENSOR  DS      1       ; LOW LEVEL SENSOR.
            g_bLowLevelSensorHistory = UNCOVERED;     // LOWLEVELSENSORHIS  DS      1       ; LOW LEVEL SENSOR.

            g_nHighLSCoveredCounter = 0;      // HIGHONCTR
            g_nHighLSUnCoveredCounter = 0;    // HIGHOFFCTR
            g_nLowLSCoveredCounter = 0;       // LOWONCTR
            g_nLowLSUnCoveredCounter = 0;     // LOWOFFCTR
            g_bOverrideClean = FALSE;             // OVERRIDECLEAN
            g_bSetCleanWhenCycleEnd = FALSE;      // SETCLEANWHENCYCLEEND
            g_nMixerOnCounter10Hz = 0;        // MIXERONCTR10HZ ,,, 10 HZ COUNTER FOR MIXING
            g_fAvgHopperWeight = 0.0;           //

//17.09.2004
            g_nStageFillEnHistory = 0;                  // STAGEFILLENH

//28.09.2004
        g_bVenturiFirstTime = TRUE;             // indicate first fill
        g_bVenturiFillByWeight = FALSE;

        g_nLoadConfigurationFailure = 0;
// 11.10.2004
        g_bManualAutoFlag = MANUAL;             //MANATO manual or automatic
        g_bSaveAllCalibrationToEEprom = FALSE;
        g_bCalibrationDataChange = FALSE;       // CALFLG

        g_bNewMaxScrewSpeed = FALSE;
        g_nDACPerKgDoneFlag = 0;
        g_nSaveRecipeSequence = 0;
        g_nSaveRecipeByteNo = 0;

// 5th Nov 2004
// Initialisation for clean variables
        g_bRightCornerCleanOn = FALSE;
        g_bLeftCornerCleanOn = FALSE;
        g_bOverrideClean = FALSE;
        g_bBlenderTopCleanOn = FALSE;
        g_bBlenderMixerCleanOn = FALSE;


        g_nAirJet1OnCounter = 0;
        g_nAirJet2OnCounter = 0;
        g_nLeftCornerCleanOnCounter = 0;
        g_nRightCornerCleanOnCounter = 0;

        g_bNewSetpointsFlag = FALSE;           // NEWSET
        g_nLoadNewSetpointLag = 0;       // LDNSPLAG
        g_bCCVsFlag = FALSE;                 // CCVSFLG
        g_nToStallTime = 0;              // TOSTALTIM --REVIEW--
        g_nExtruderStalledCounter = 0;      // EXTSCTR
        g_bManualChangeFlag = FALSE;           // MCHGFLG
        g_nDA1Temp = 0;                   // DA1TEMP
        g_bEnterKeyPressedFlag = FALSE;        // EKPRSFLG

        g_nCommsIncreaseFlag = 0;        //COMMSIFLG
        g_nCommsDecreaseFlag = 0;        //COMMSDFLG
        g_fExtruderRPM = 0;              // Extruder screw rpm (asm =SSRPM/SSRPM2D

        g_bByPassMode = FALSE;              // BYPASS not in ByPass mode.
        g_bOverrideOutputs = FALSE;     // OVERRIDEOUTPUST
        g_bFeedValveClosed = CLOSED;        // FEEDCLOSE
        g_bCleanAtEndOfCycle = FALSE;
        g_bPauseOnActive = FALSE;
        g_bSecondRollover = FALSE;              // SECROVER - set by RTC int.
        g_bForegroundSecondRollover = FALSE;    // new, second roll over flag for foreground.
        g_bMixerTripped = FALSE;
        g_nMixerTrippedCounter = 0;
        g_nPauseOnActiveCounter = 0;
        g_nPauseOnInactiveCounter = 0;
        g_bExtruderControlStartUpFlag = FALSE;  // STRUPF

        g_nSEIInputStates = 0;
        g_nSEIAlarms = 0;
        g_fActualGrammesPerM2 = 0;

// 25th Nov 2004
        g_bDiagnosticsPasswordInUse = FALSE;
        g_nDeviceToPollCounter = 0;
      g_bCheckIfShouldCycleFlag = FALSE;    // CHKIFCYCLEFLAG    ; CHECK IF THE BLENDER SHOULD
        g_nCheckIfShouldCycleCounter = 0;       // CHKIFCYCLECTR
      g_bShowInstantaneousWeight = FALSE;       // SHOWINWEIGHT - set flag to show intan
      g_bREadIncDecCalibrationData = FALSE;     // READINCDECCAL - read calibration from SE
        g_nDoIncDecReadInNSeconds = 0;          // DOREADINNSECONDS
        g_nIgnoreLSTime  = 0;               // IGNORELSTIME,,
        g_nBatchMonitorCounter = 0;         // BATCHMONCTR     DS      2       ;
      g_bRunLevelSensorCheck = FALSE;         //  asm = CHECKFORMAXTHP
        g_nBypassSensorUncoveredCounter = 0;    // BYPASSSENSOROFFCTR
        g_nPauseInNSeconds = 0;
        g_bUpdateHiLoFlag = FALSE;              //
        g_bSEIPollInputsFlag = FALSE;         // SEIPOLLINPUTSF          ; START POLLING OF INPUTS
        g_bSEIPollLSFlag = FALSE;             // SEIPOLLLLSF             ; SET LOW LEVEL SENSOR
        g_bWriteAlarmsFlag = FALSE;           // WRITEALARMSF
        g_bReadDAValueFlag = FALSE;           // READDAVALUEF Trigger A/D read from SEI.
        g_bHourFlag = FALSE;                            //
        g_nDoDiagnosticsCounter = 0;            // DIAGCOUNTER
        g_bDoDiagnosticsFlag = FALSE;           // no asm equiv.  Flag set to call DoDiagnostics in foreground.
        g_nPITFractionCounter = 0;          //
        g_nSeqTimeout = 0;                // SEQTIMEOUT
        g_bPeripheralCommsSTarted = FALSE;    //COMMSST   comms started??
        g_nLineSpeedFractionCounter = 0;  // LSPDFRACTIONCTR
        g_nTinySlideCycleCounter = 0;
        g_nTinySlideOpenTimeCounter = 0;  // TINYSLIDEOPENTIMECOUNTER
        g_bForegroundTenHzFlag = FALSE;         //
        g_nTenthsOfSecondCounter2 = 0;
        g_bSEIPollIncDecFlag = FALSE;
        g_nCMRValidTimer = 0;
        g_bRemoteKeyPoll = FALSE;
        g_nSecondCounter2 = 0;
        g_bFGSecondFlag2 = FALSE;
        g_bFGOneMinuteFlag = FALSE;

        g_nDisplayUpdateCounter = 0;
      g_bUpdateDisplayNow = FALSE;
      g_nMixerOffCounter = 0;
      g_nMixerOnTime = 0;
      g_nExtruderDAValue = 0;           // DA1TMP.
      g_nMixerTransitionCounter = 0;    // MIXERTRANSITIONCTR  // For each component:

        g_nSecondCounter = 0;               // SECCNT
        g_bTwoSecondFlag = FALSE;             // TWOSFLG  - indicate 2 seconds rollover
        g_nTenSecondCounter = 0;            // TSCCNT
        g_bTenSecondFlag = FALSE;            // TSCFLG            ;~10 SEC. FLAG
        g_nFrontRollChangeLag = 0;              // RCF_LAG  -Front reel change lag
        g_nBackRollChangeLag = 0;               // RCB_LAG  -Back reel change lag
        g_nMixScrewPulseCounter = 0;            //MIXSCREWPULSECTR   READ MIXING SCREW PULSE COUNTER
        g_bMixScrewRotationAlarm = FALSE;   // MIXSCREWROTALARM

      g_nRTCSecondCounter = 0;       //
      g_bRTCMinuteFlag = FALSE;          // set flag on Minute transition.
      g_nRTCMinuteCounter = 0;
      g_bOneSecondFlag = FALSE;          // ONESECONDFLAG, set by RTC.
      g_nKgHrSecondsCounter = 0;        // KGHRCOUNT
      g_nLevelSensorSimulator = 0;      //LEVELSIM              ; INCREMENT LEVEL SENSOR SIMULATOR

// 23th Dec 2004
        g_bAlarmRelayStatus = OFF;              // flag to show state of alarm relay
        g_bAlarmBeaconStatus = OFF;         // flag to show state of alarm beacon

      g_fRRThroughputKgPerHour = 0;         //RRTPTKGH
      g_fRRThroughputLbPerHour = 0;         //RRTPTLBH
      g_fThroughputKgPerHour = 0;            // TPTKGH
      g_fThroughputLbPerHour = 0;           // TPTLBH
      g_fBatchKgPerHour = 0;               // BATCHKGPHR
      g_fBatchTimeInSeconds = 0;           // BATCHSECS
      g_nPreviousPITTime = 0;              // PREVPITM

// 18.1.2005
        g_bRemoteOperatorPanelPresent = FALSE;  //          The presence will be set up later in configuration
        g_bSEIPresent = FALSE;                          //      Assume not present initially.
        g_bLLSPresent = FALSE;                          //

        g_nCommsUpdateStartIndex = 0;           //
        g_bCommsUpdateActive = FALSE;           //
        g_nStartAttributeUpdateIndex = 0;   //
        g_bCommsAttributeUpdateActive = FALSE;

        for(i= 0; i < MAX_COMPONENTS; i++)
        {       // zero comp actual wt's
                g_fComponentActualWeight[i] = 0.0;
                     // zero comp set wt's
                g_fComponentSetWeight[i] = 0.0;

                // zero comp actual wt's - Imperial
                g_fComponentActualWeightI[i] = 0;

                     g_fComponentActualWeightRR[i] = 0.0;
                g_fComponentActualWeightRRI[i] = 0.0;

                     // zero comp actual %'s
                g_fComponentActualPercentage[i] = 0;
                     // zero target wt.s
                g_fComponentTargetWeight[i] = 0;
                g_fComponentTargetWeightI[i] = 0;

               g_lComponentTargetCounts[i] = 0;
                    g_lStoredComponentTargetCounts[i] = 0;

               g_lComponentTargetCountsTotal[i] = 0;
               g_fComponentTargetTopUpWeight[i] = 0.0;
               g_fComponentTargetTopUpWeightI[i] = 0.0;
               g_bComponentInSeqTable[i] = FALSE;

               // zero volumetric timers
                   g_lComponentVolumetricTimer[i] = 0;

                    // zero the component raw & tare counts.
                     g_lComponentCountsActual[i] = 0;
                g_lCmpRawCounts[i] = 0;
                     g_lCmpTareCounts[i] = 0;

                g_lCmpTime[i] = 0;
                g_fComponentOpenSeconds[i] = 0;
                     g_bComponentValveState[i] = OFF;
                     g_nLFillStatCtr[i] = 0;
                     g_bFillStatCmp[i] = 0;
                     g_bFBWStatus[i] = FALSE;

                g_bNoFlowRateCalculationArray[i] = FALSE;

                g_MainCPIRRData.m_ComponentCPIRRData[i].m_nResetCPIRRCtr = RESETRRNO;   // cause the CPI round robins to be reset.
                g_Stage2CPIRRData.m_ComponentCPIRRData[i].m_nResetCPIRRCtr = RESETRRNO; //
                g_Stage3CPIRRData.m_ComponentCPIRRData[i].m_nResetCPIRRCtr = RESETRRNO; //
                    // clear the CPI RR "ignore" flags.
                    g_MainCPIRRData.m_ComponentCPIRRData[i].m_bIgnoreCPIRR = FALSE;
                    g_Stage3CPIRRData.m_ComponentCPIRRData[i].m_bIgnoreCPIRR = FALSE;
                    g_Stage2CPIRRData.m_ComponentCPIRRData[i].m_bIgnoreCPIRR = FALSE;



                    // Clear "has retried" flags
                    g_bHasRetried[i] = FALSE;
                    g_bHasRetriedStage2[i] = FALSE;
                    g_bHasRetriedStage3[i] = FALSE;

                    // Clear the cycle retry counters.
                    g_nRetryCounterStage1[i] = 0;
                    g_nRetryCounterStage2[i] = 0;
                    g_nRetryCounterStage3[i] = 0;


                g_nStageSeq[i] = STAGE_0;
                g_bCopyCompFlowRate[i] = FALSE;         // disable copy of flow rate
                     g_bMulFillStarted[i] = FALSE;  // multiple filling not started at reset!!
//17.09.2004
                    g_lStoredComponentCountsStage1[i] = 0;
                    g_lStoredComponentCountsStage2[i] = 0;
//23.11.2004
                    g_bLowLevelSensorStatus[i] = COVERED;       // initialise to "good" state initially.



                    g_fComponentFrontWeightAccumulator[i] = 0.0;                // CH1WACCF
                    g_fComponentBackWeightAccumulator[i] = 0.0;                 // CH1WACCB
                    g_fComponentOrderWeightAccumulator[i] = 0.0;                // CH1WACCO
                    g_fComponentShiftWeightAccumulator[i] = 0.0;                // CH1WACCS
                    g_fComponentShiftWeightOngoingAccumulator[i] = 0.0;         // CH1WACCS9
                    g_fComponentHourlyWeightAccumulator[i] = 0.0;               // CH1WACCH
                    g_fComponentLongTermWeightAccumulator[i] = 0.0;             // CH2WACCM
                    g_fComponentLongTermWeightResettableAccumulator[i] = 0.0;
                    g_fComponentBatchWeightAccumulator[i] = 0.0;                // CH1WACCBAT

// initialise component data 12th July 2005

                   g_fComponentOrderWeightAccumulator[i] = 0.0f;                 // CH1WACCO DS    5         ; weight accumulator,  (order)
                   g_fComponentActualUnusedWeight[i] = 0.0f;                     // CMP1ACTWCM      DS      3       ; COMPONENT #N ACTUAL WEIGHT
                   g_fComponentLongTermWeightAccumulator[i] = 0.0f;              // CH1WACCM DS    5         ; weight accumulator, XX XX XX .YY YY KGS
                   g_fComponentOngoingOrderPercentage[i] = 0.0f;                 // ORBPCNT1 DS      3       ; ORDER %
                   g_fComponentFrontRollPercentage[i] = 0.0f;                    // CH1PERCFR DS    3         ; CALCULATED % FOR COMPONENTS.
                   g_fComponentBackRollPercentage[i] = 0.0f;                     // CH1PERCBR DS    3         ; CALCULATED % FOR COMPONENTS.
                   g_fComponentOrderPercentage[i] = 0.0f;                        // CH1PERCO        DS    3         ; CALCULATED % FOR COMPONENTS.
                   g_fComponentShiftPercentage[i] = 0.0f;                        // CH1PERCS DS    3         ;
                   g_fComponentHourlyPercentage[i] = 0.0f;                       // CH1PERCH DS    3         ;
                   g_fComponentLongTermPercentage[i] = 0.0f;                     // CH1PERCM DS    3         ; CALCULATED % FOR COMPONENTS.

// 14/09/05
                   g_bLowLevelSensorOnTransition[i] = FALSE;                     // LS1ONTR   DS    1       ; LEVEL SENSOR ON TRANSITION.
// 5/1/06
                   g_ffInstantaneousCPI[i] = 0.0f;

                   g_bOptimisationFlapStatus[i] = FULL;
                   g_bOptimisationLoaderEnable[i] = FALSE;
                   g_bOptimisationLoaderFill[i] = FALSE;
                   g_nOptimisationFlapDebounceCounter[i] = 0;
                   g_bOptimisationFlapHistoryStatus[i] = FULL;
                   g_fComponentLoaderWeightAccumulator[i] = 0.0f;
                   g_nLoaderEmptyingCounter[i] = 0;
                   g_fComponentRemainingOrderWeight[i] = 0.0f;
                   g_fComponentLoaderWeight[i] = 0.0f;
                   g_fComponentBinWeight[i] = 0.0f;
                   g_fComponentBinAndLoaderWeight[i] = 0.0f;
                   g_bCheckOptimisationAgain[i] = FALSE;
                   g_nComponentPerDeviationErrorCounter[i] = 0;
                   g_bComponentHasFilledByWeight[i] = FALSE;
                   g_fComponentWeightMean[i] = 0.0f;
                   g_fComponentWeightStdDev[i] = 0.0f;
                   g_fComponentWeightMaximum[i] = 0.0f;
                   g_fComponentWeightMinimum[i] = 0.0f;
                   g_fComponentWeightStdDevPercentage[i] = 0.0f;
                   g_nComponentWeightSampleCounter[i] = 0;
                   g_bFlowRateChange[i] = FALSE;
                   g_nBlastMomentaryOnCtr[i] = 0;
                   g_bAcceptFlowRate[i] = FALSE;
                   g_nValidFlowRateExceededCtr[i] = 0;
                   g_nFillingMethodHistory[i] = 0;
                   g_bCalibratePulsing[i] = FALSE;
                   g_fCurrentRecipePercentageHistory[i] = 0.0f;
                   g_fRequiredPulseWeightInGrams[i] = 0.0f;
                   g_nPulseTimeForMinWeight[i] = 0;
                   g_nHalfGramTime[i] = 0;
                   g_lSettledAverageCounts[i] = 0;
                   g_bComponentHasRetried[i] = FALSE;

}
// add component clearances here !!


     for(i=0; i<sizeof(g_cSeqTable); i++)
     {
        g_cSeqTable[i] = 0;                                          // SEQTABLE        DS      16      ; SEQUENCE TABLE TO ALLOW
     }

     for(i=0; i<MAX_STABILITY_READINGS; i++)
     {
        g_nAToDStabilityLog[i] = 0;
        g_nAToDRefStabilityLog[i] = 0;
     }

     for(i= 0; i < MAX_GPM_RR_SAMPLES; i++)
     {
         g_fGPMRRBuffer[i] = 0.0;
     }

        g_nExpansionOutput = 0;                                                 //EXPANOUTPUT
        g_bExpanioFitted = FALSE;                                               // EXTERNALCLEANPCB
        g_bBypassStatus = FALSE;                                                // BYPASSSTATUS
        g_nAfterStartUpCtr = 0;                                                 // AFTERSTARTUPCTR
        g_nCorrectEveryNCycleCtr = 0;                                           //  CORRECTEVERYNCYCLECTR
        g_nControlErrorCounter = 0;                                             // CERCT1
        g_fControlErrorPercentage = 0.0;                                        // Control % error.
        g_nTempExtruderDAValue = 0;                                             // temp d/a storage
        g_nExtruderTachoReadingAverage = 0;                                     // SEIADREADINGAVG
        g_nExtruderTachoReading = 0;                                            // SEIADREADING
        g_bScrewIncreaseDeceasePressed = 0;                                     // SCREWINCDECPR
        g_fLineSpeed5sec = 0.0;                                                 // LSPD5S
        g_fWeightPerMeter = 0.0;                                                // WTPMVL5
        g_fWeightPerMeterRR = 0.0;                                              // RRGMPM5
        g_nEstimationContinueCounter = 0;                                       //CONESTIMATION

        g_fExtruderDAValuePercentage = 0.0;                                     // CH1CV
        g_nSEIDToAReadState = 0;                                                // SEIDARDSTATE
        g_lSEIPulses = 0;                                                       // SEIPULSEACC
        g_nSEIAlarmState = 0;                                                   // SEIALARMSTATE
        g_nSEIDToAOutputState = 0;                                              // SEIDAOPSTATE
        g_nSEIInputState = 0;                                                   // SEIINPUTSTATE
        g_nSEIPulsesPerSecond = 0;                                              // SEIPULSEPSEC
        g_nVoltageFollowerReading = 0;                                          // VOLFOLREADING
        g_bSEIPulsesReady = FALSE;                                              // SEIPULSESREADY
        g_nLineSpeedFractionCounter = 0;                                        // LSPDFRACTIONCTR
        g_nSEIIncreaseDecreaseState = 0;                                        // SEIINCDECSTATE
        g_bLengthCalculationInProgress = FALSE;                                 // LENGTHCALCINP
        g_bSEIFirstPulseRead = TRUE;                                           // SEIFIRSTPULSER
        g_lSEICurrentPulses = 0;                                                // CURRENTPCLK
        g_lSEIPreviousPulses = 0;                                               //PREVIOUSPCLK
        g_lDifferenceInPulses = 0;                                              // DIFFINPCLK
        g_lBatchPulsesAccumulator = 0;                                          // BATCHPULSEACC
        g_lFrontRollPulsesAccumulator = 0;                                      //LSPDPULACC5SECF
        g_lBackRollPulsesAccumulator = 0;                                       //LSPDPULACC5SECB
        g_lHourlyPulsesAccumulator = 0;                                         //HOURLYPULACC
        g_nLineSpeedPulse5SecondCounter = 0;                                    //LSPDPULCTR5SEC
        g_lLineSpeedPulse5SecondAccumulator = 0;                                //LSPDPULACC5SEC
        g_lLineSpeedPulse5SecondTotal = 0;                                      //LSPDPULTOT5SEC
        g_bLineSpeedUpdate = 0;                                                 //LS5SFG
        g_bSEIReadDataAvailable = 0;                                            // new variable
        g_bDmpLevSen = FALSE;

// 29th March 2005
        g_nZeroLineSpeedErrorCounter = 0;                                       //LSPEEDERRORCTR
        g_fLineSpeed5secHistory = 0.0;                                          //WMLS5S
        g_bLineSpeedHasChanged = FALSE;                                         //LS5CFG DS     1
        g_nLineSpeedChangeCounter = 0;                                          //LSPEEDCHANGECTR
        g_fBatchLength = 0.0;                                                   //BATCHLENGTH
        g_nEstimatedAddedPulses = 0;                                            //ESTADDSEIPULSES
        g_nEstimatedSubtractedPulses = 0;                                       //ESTSUBSEIPULSES
        g_fLbsPer1000Feet = 0.0;                                                //ACTLBKF
        g_nGPMRRIndex = 0;                                                      // index or pointe
        g_nGPMRRSummationCounter = 0;                                           // summation count
        g_nIncreaseLampStatus = 0;                                              // INCLAMPSTATUS
        g_nDecreaseLampStatus = 0 ;                                             // DECLAMPSTATUS
        g_nSEITimeOutCounter = 0;                                               // SEITIMEOUTCTR
        g_bSEIHasTimedOut = FALSE;                                              // SEITIMEOUTFLAG
        g_bPrintSEIResetOccurred = FALSE;                                       // PRINTTOMANUAL
        g_bAlarmOnFlag = FALSE;                                                 // ALARMONFLAG
        g_bAlarmOnTransitionFlag = FALSE ;                                      // ALARMONTRFLAG
        g_fTotalOrderWeightAccumulator = 0.0;                                   // TOTWACCO DS
        g_fTotalFrontWeightAccumulator = 0.0;                                   // TOTWACCF DS
        g_fTotalBackWeightAccumulator =  0.0;                                   // TOTWACCB DS
        g_fTotalShiftWeightAccumulator = 0.0;                                   // CURSHIFTWEIGHT
        g_fTotalShiftWeightOngoingAccumulator = 0.0;                            // TOTWACCS9 DS
        g_fTotalLongTermWeightAccumulator = 0.0;                                // TOTWACCM DS
        g_fTotalHourlyWeightAccumulator = 0.0;                                  // TOTWACCH DS
        g_fTotalBatchWeightAccumulator = 0.0;                                   // TOTWACCBAT  DS
        g_fTotalHistoryBatchWeightAccumulator = 0.0;                            // TOTWACCBATHI
        g_fTotalBatchWeightAccumulatorImp = 0.0;                                // TOTWACCBATI DS
        g_nStopInNBatches = 0;                                                  //STOPINNBATCHES
        g_fTotalBatchWeightAccumationLeft = 0.0;                                //TOTWACCBATLEFT D
        g_fBatchSizeStorage = 0.0;                                              //DFTW1STORE
        g_bNoTargetAlarm = FALSE;                                               //NOTARGETALARM
        g_bBatchIsReady = FALSE;                                                //BATCHREADY


// 7th April 2005

       g_bMixerSensorHistory = FALSE;                                           //MIXSENSORHISTORY DS     1
       g_nAToDSumCounter = 0;                                                   //SAMPLEAD1
       g_lAToDSum = 0;                                                          //SUMAD1
       g_fLineSpeed5secFtPMinute = 0.0;                                         //LSPD5SFM
       g_fTotalThroughputSetLbsPerHour = 0.0;                                   //SETTLBH
       g_fTotalThroughputSetLbsPer1000Feet = 0.0;                               //SETLBKF
       g_fTotalfDesiredWidthinInches = 0.0;                                     //WIDTHSPIN
       g_nHiLowOptionSwitchedOnOff = 0;                                         //HILOWOPTIONSWITCHEDONOFF
       g_nHiLowOptionSwitchedOnOffHistory = FALSE;                              //HILOWOPTIONSWITCHEDONOFFHIS
       g_bPauseOffActive = FALSE;                                               //PAUSEOFFACTIVE
       g_nPauseOffActiveCounter = 0;                                            //POACTIVECTR
       g_bActivateFrontRollChange = FALSE;                                      //RCFFLG
       g_bActivateBackRollChange = FALSE;                                       //RCBFLG
       g_bReadDToFromSEIOnReset = TRUE;                                         //read d/a on reset


// 12th July 2005

       g_fUsedBatchWeight = 0.0f;                                               //BATCHUSEDWT     DS      3       ; LAST ROLL COMPSENSATED WEIGHT
       g_fUnUsedBatchWeight = 0.0f;                                             //BATCHUNUSEDWT   DS      3       ; LAST ROLL COMPSENSATED WEIGHT
       g_lFrontTotalLengthPulses = 0;                                           // FLSPREG DS    5        ;~LINE SPEED (FRONT) TOTAL LENGTH (PULSES)
       g_lBackTotalLengthPulses = 0;                                            // BLSPREG DS    5        ;~LINE SPEED (BACK) TOTAL LENGTH (PULSES)
       g_bResetOrderTotalsAtNextRollChange = FALSE;                             // RSTOFLAG DS    1       ; RESET ORDER FLAG
       g_lCurrentPulses = 0;
       g_lPreviousPulses = 0;
       g_nPulsesDifference = 0;



        g_FrontRoll.m_nRollNo = 1;                                              // roll no set to 1, roll lengrh set to 0
        g_FrontRoll.m_fLength = 0.0f;
        g_BackRoll.m_nRollNo = 1;
        g_BackRoll.m_fLength = 0.0f;
        g_Order.m_nRollNo = 1;
        g_Order.m_fLength = 0.0f;
        g_CurrentShiftFront.m_nRollNo = 1;
        g_CurrentShiftFront.m_fLength = 0.0f;

        g_OngoingShift.m_nRollNo = 1;
        g_OngoingShift.m_fLength = 0.0f;
        g_fTotalHourlyLengthAccumulator = 0.0f;             // HRLTACC  DS    5         ; HOURLY LENGTH ACCUMULATOR.
        g_nHourlyReportCounter = 0;                         // PRHRCTR DS      1       ; HOUR COUNTER
        g_bHourDetectedBefore = FALSE;                      // HRBEFORE DS     1       ;INDICATES THAT HOUR HAS BEING DETECTED.
        g_bActivateHourlyReport = FALSE;                    // ACTHRREP        DS      1       ; ACTIVATE HOURLY REPORT.


// 5th Sept 2005
//

        g_nWarmUpCounter = 0;                                               // WARMUPCTR               DS      1       ; WARMUP COUNTER
        g_bRecipeWaiting = FALSE;                                           // RECIPEWAITING           DS      1       ; DEFINE RECIPE WAITING FLAG.
        g_bBatchHasFilled= FALSE;                                           // BATCHHASFILLED          DS      1       ; INDICATES THAT BATCH HAS BEEN FILLED DURING THE CLEANING CYCLE.
        g_bBlenderInhibit= FALSE;                                           // BLENDERINHIBIT          DS      1       ; BLENDER INHIBIT
        g_nVacuumSequence = 0;                                              // VACSEQ                  DS      1       ; INDICATES VACUUM SEQUENCING IN SINGLE RECIPE MODE.
        g_nVacuumTimer = 0;                                                 // VACCTR                  DS      2       ; VACUUM SEQUENCE TIMER
        g_nVacuumWriteData = 0;                                             //VACUUMWR                DS      2       ; WRITE WORD SENT TO VAC8 I/O
        g_bOfflineValveIsOpen= FALSE;                                       //OFFLINEVALVEISOPEN      DS      1       ; ALLOW MIXER CLEAN TO PROCEED AFTER THE OFFLINE VALVE IS OPENED IN SINGLE RECIPE MODE
        g_bCleanOnNextCycle= FALSE;                                         //CLEANONNEXTCYCLE        DS      1       ; INDICATES THAT CLEAN SHOULD BE INITIATED.
        g_bWaitForVacuumToFinish = FALSE;                                   //WAITFORVACUUMTOFINISH   DS      1       ; INDICATES THAT WE ARE WAITING FOR VACUUM TO FINISH.
        g_bVacuumIsOn= FALSE;                                               //VACUUMISON              DS      1       ; VACUUM IS ON.
        g_nVacuumLoaderNo = FALSE;                                          //VACUUMLOADER            DS      1       ; VACUUM LOADER FOR DISPLAY PURPOSES
        g_bCheckIfVacuumIsRequired = FALSE;                                 //CHECKIFVACUUMREQUIREDF  DS      1       ; CHECK IF VACUUM REQUIRED
        g_bVacuumMaterialAway= FALSE;                                       //VACUUMMATERIALAWAY      DS      1       ; CHECK FOR MATERIAL TO BE VACUUMED AWAY.
        g_bVacuumInProgress = FALSE;                                        //VACUUMINGINPROGRESS     DS      1       ; DEFINE VACUUMING IN PROGRESS
        g_nBlenderReadyCounter = 0;                                         //BLENDERREADYCTR         DS      1       ; INDICATE THAT BLENDER IS READY.
        g_nStartUpPagePauseTimer = 0;                                       //PAUSETIMER      DS      1       ; PAUSED TIMER
        g_bOfflineBatchFilled = FALSE;                                      //OFFLINEBATCHFILLED      DS      1       ; INDICATE THAT OFFLINE BATCH IS READY
        g_bAlarmFlash = FALSE;                                              //ALARMFLASH              DS      1       ; INDICATES IF ALARM SHOULD BE FLASHING
        g_bFlashSecondCounter = FALSE;                                      //FLASHSECCTR             DS      1       ; FLASH SECONDS COUNTER
        g_bAlarmFlashStatus = FALSE;                                        //ALARMFLASHSTATUS        DS      1       ; INDICATES WHAT THE STATUS OF THE OUTPUT SHOULD BE
        g_nLowLevelSensorInputState = 0;                                    //LLSINPUTSTATE   DS      2       ; READ LLS INPUT STATE.
        g_bInCalibrationMenu = FALSE;                                        //CALIFG DS     1        ;~CALIBRATION FLAG, SET TO AA WHEN IN CAL MENU O

// 14/09/05

       g_bSaveAllCalibrationToOnBoardEEprom = FALSE;                       //SAVECAL2DATA1F  DS      1       ; SAVE CALIBRATION DATA FLAG.
       g_bSignalXXKgsTargetAlarm = FALSE;                                  //SIGNALATTARGETALARMFLAG DS      1       ; SIGNAL ALARM ON FEEED OPEN
       g_nLowLevelSensorAlarmWord = 0;                                     //LLSALARMWORD    DS      2       ; ALARM WORD WRITTEN TO LLS MODULE.
       g_nExpansionInputData = 0;                                          //EXPANIOINPUTDATA        DS      1       ; EXPANIO PCB STORAGE
       g_bNoLowLevelSensorCommunications = FALSE;                          // NOLLSCOMMUNICATIONS     DS      1       ; NO LLS COMMUNICATIONS FLAG
       g_nNoLowLevelSensorCommunicationsCounter = 0;                       //NOLLSCOMMUNICATIONSCTR  DS      1       ; NO LLS COMMS COUNTER


// 3/10/05

       g_bShowExpiryPage= FALSE;                                           // SHOWEXPIRYPAGE          DS      1       ; SHOW EXPIRY PAGE.
       g_bIsMasterModule= FALSE;                                           // ISMASTERMODULE          DS      1       ; INDICATES THAT MASTER CODE IS IN THE CALIBRATION MODULE
       g_bPrintHourlyReport= FALSE;                                        // TOPRNFLG  DS    1       ; "TO PRINT FLAG"
       g_bPrintOrderReport= FALSE;                                         // TOPRNFLG  DS    1       ; "TO PRINT FLAG"
       g_bPrintFrontRollReport= FALSE;                                     // TOPRNFLG  DS    1       ; "TO PRINT FLAG"
       g_bPrintBackRollReport= FALSE;                                      // TOPRNFLG  DS    1       ; "TO PRINT FLAG"
       g_unCalibrationCheckSum =0;                                         // CALDSM DS     2                 ;~CHECKSUM FOR CAL. DATA
       g_nAllowLargeAlarmBand= FALSE;                                      // ALLOWLARGEALARMBAND     DS      1       ; ALLOW LARGE ALARM BAND


/* 18/10/05 */
        g_bPotControl = FALSE;                                              // CH1POTBIT DS  1        ; POT. CONTROL BITS (6 CHANNELS)


/* 3/11/05  */
        g_nDiagnosticArrayIndex = 0;
/* 11/11/05  */
        g_nSingleCycleCompNoHis = 0;
/* 15/11/05  */
        g_nReMixOnTime = 0;

/* 5/1/06 */
        g_bControlSetpointChanged = FALSE;
        g_bWriteIncreaseDecreaseDataToSEI = FALSE;                          // WRITEINCDECF    DS      1       ; WRITE INCREASE DATA TO SEI.
        g_nPeripheralCardsPresentHistory = 0;                               // peripheral card history.
        g_bIsSystemAlarm = FALSE;
        g_nStageFillEnableTransitionHistory = 0;
        g_bProgramLoopRunnning = FALSE;                                           //

        g_nBatchesInKghAvgHistory = 0;
        g_bSPIAtDActive = FALSE;
        g_bSPIEEActive = FALSE;

         g_bModbusLineSpeedWritten = FALSE;
        g_fCommsLineSpeedFrontLength = 0.0f;
        g_fCommsLineSpeedBackLength = 0.0f;
        g_bModbusDToAWritten = FALSE;
        g_bModbusDToAPercentageWritten = FALSE;
        g_bRedirectStdioToPort2 = FALSE;
        g_bSoftwareUpdateRunning = FALSE;
        g_bSDCardSPIActive = FALSE;
        g_bSaveAllCalibrationToEEpromOutsideCycle = FALSE;
        g_bSPIBusBusy = FALSE;
        g_bModbusLengthWritten = FALSE;
        g_fRemainingOrderWeight = 0.0f;
        g_fOptimisationOrderLength = 0.0f;
        g_fRemainingOrderLength = 0.0f;
        g_bFirstPulsesReadFromSEI = TRUE;
        g_bInitiatePurge = FALSE;
        g_bBlenderPurged = FALSE;
        g_bBlenderPurgeingInProgress = FALSE;
        g_fWeightPerMeterFromLSpeed = 0.0f;
        g_nAllowReadOfControlInputsCtr = 0;
        g_nOverrideCounter = 0;
        g_bWriteToIniFileAgain = FALSE;
        g_bUsingSmallBatchSize = FALSE;
        g_bModbusScrewSpeedWritten = FALSE;
        g_nSuspendControlCounter = 0;
        g_nHoldOffLevelSensorAlarmSecondCounter = 0;
        g_IncreaseOutputOn10hzCounter = 0;  // asm INCREASEOUTPUTONCTR
        g_DecreaseOutputOn10hzCounter = 0;  // asm DECREASEOUTPUTONCTR
        g_bResetComponentWeightsRoundRobin = FALSE;
        g_bMultiBlendPaused = FALSE;
        g_bMultiBlendisPaused = FALSE;
        g_nAutocycleLevelSensorOff10hzCounter = 0;
        g_bSHAHardwareCheckFailed = FALSE;
        g_bSaveUSBFileToUSB = FALSE;
        g_bUSBDataWrite = FALSE;
        g_bPanelWriteHasHappened = FALSE;
        g_bSEIReadDataReady = FALSE;
        g_nExtruderVolumetricDAValue = 0;
        g_bReadSDCardDirectory = FALSE;
        g_nCopyFileNoToUSB = 0;
        g_nCopyFileNoToUSBRequest = 0;
        g_nUSBCopySequencer = 0;
        g_bUSBCopyInProgress = FALSE;
        g_lUSBFileDataCopied = 0;

        g_nSEIWriteRequiredCounter = 0;
        g_nControlLagHistory = 0;
        g_nIncreaseRateHistory = 0;
        g_nDecreaseRateHistory = 0;
        g_nControlDeadbandHistory = 0;
        g_nFineIncDecHistory = 0;
        g_bSEIIncDecSetpointDownload = FALSE;
        g_bSaveAllProcessDataToEEprom = TRUE;
        g_bAllowProcesDataSaveToEEPROM = TRUE;
        g_nCurrentExtruderNo = 1;
        g_nCycleCounter = 0;
        g_bAllowMultiBlendCycle = FALSE;  //kwh
        g_nSDCardSoftwareUpdateComplete = 0;
        g_nSDCardLoadConfigComplete = 0;
        g_nSDCardLoadBackupConfigComplete = 0;
        g_nSDCardSaveConfigComplete = 0;
        g_nSDCardCopyConfigToBackupComplete = 0;

        g_bSDCardSoftwareUpdateComplete = FALSE;
        g_bSDCardLoadConfigComplete = FALSE;
        g_bSDCardLoadBackupConfigComplete = FALSE;
        g_bSDCardSaveConfigComplete = FALSE;
        g_bSDCardCopyConfigToBackupComplete = FALSE;
        g_nDownloadManagerNZTimeoutCtr = 0;
        g_nDisableDTAPerKgCtr = 0;
        g_bTelnetHistory = FALSE;
        g_nBatchCommsStatusSEI = 0;
        g_nBatchCommsStatusLLS = 0;
        g_nBatchCommsStatusOptimisation = 0;
        g_nBatchCommsStatusOptimisation = 0;
        g_nBatchCommsStatusNetwork = 0;
        g_nBatchCommsStatusTCPIP = 0;
        g_nBatchCommsSEITimeoutHistory = 0;
        g_nBatchCommsLLSTimeoutHistory = 0;
        g_nBatchCommsOptimisationTimeoutHistory = 0;
        g_nTCPIPReadHoldRegsCtr = 0;
        g_bToAutoEvent = FALSE;
        g_bToManualEvent = FALSE;
        g_cCurrentMonthHistory = 0;
        g_nWriteConfigToSDInNSeconds = 0;

        g_bWeighHopperCalibratedEvent = FALSE;
        g_bWeighHopperTaredEvent = FALSE;
        g_bCleaningInitiatedEvent = FALSE;

        g_nWriteConfigToSDInNSeconds = 0;
        g_nOperatorPanelPageId = 0;
        g_nBlenderType = 0;
        g_nSDCardDelayTime = 0;
        g_nDisableSDCardAccessCtr = 0;
        g_nAMToggleStatusDelayCtr = 0;
        g_bWriteEventLogToSdCard = FALSE;
        g_cEventLogFile[0] = 0;
        g_nLiquidAdditiveComponent = 0;
        g_bLiquidEnabledHistory = FALSE;
        g_bModbusLiquidDToAWritten = FALSE;
        g_bModbusLiquidDToAPercentageWritten = FALSE;
        g_bLiquidControlAllowed = TRUE;
        g_nCheckIfWeighHopperOpenedSecCtr = 0;
        g_fWeightAtOpenDumpFlap = 0.0f;
        g_nLiquidAdditiveOutputDAValue = 0;
        g_fLiquidAdditiveOutputPercentage = 0.0f;
        g_fLiquidKgPerHourPerPercentOutput = 0.0f;
        g_nTemperature = 0;
        g_nLiquidOnTimeCtr = 0;
        g_fOneSecondHopperWeightAtEndOfCycle = 0.0f;
        g_fThroughputCalculationBatchWeight = 0.0f;
        g_fLongTermResettableLengthCounter = 0.0f;
        g_fLongTermLengthCounter = 0;
        g_fKgperHour = 0.0f;
        g_bShowNetworkWrites = FALSE;
        g_bWriteDiagnosticsToSDCard = FALSE;
        g_nTelNetTimeoutCtr = 0;
        g_nTelnetContinuousUpdate = 0;
        g_nFileSegmentNo = 0;
        g_bPrintCycleDataToTelnet = FALSE;
        g_lTCPIPCommsCounter = 0;
        g_nTelNetSecondsCounter = 0;
        g_bTelNetWindowCtr = 0;
        g_nTelnetSequence = 0;
        g_bAToDMaxExceeded = FALSE;
        g_bApplicationSPIInProgress = FALSE;
        g_bTaskSPIInProgress = FALSE;
        g_bTelnetSDCardAccessInProgress = FALSE;
        g_bHourlyResetMin = FALSE;
        g_bHourlyResetMax = FALSE;
        g_nUSBReadTimeoutInPits = 0;
        g_nUSBFlushTimeoutInPits = 0;
        g_nUSBEchoedTimeoutInPits = 0;
        g_nUSBUSBSendFileCommandsTimeoutInPits = 0;
        g_nUSBGetPrinterStatusTimeoutInPits = 0;
        g_nUSBFlushTimeoutInPits = 0;
        g_nUSBWriteTimeoutInPits = 0;
        g_bUSBDebug = FALSE;
        g_nHiRegLSCoveredCounter = 0;
        g_nHiRegLSUnCoveredCounter = 0;
        g_nExpansionOutput2 = 0;
        g_bWriteToExpansion2 = FALSE;
        g_bResetNetworkSlaveComms = FALSE;
        g_bResetPanelSlaveComms = FALSE;
        g_bAddAlarmOccurredToEventLog = FALSE;
        g_bAddAlarmClearedToEventLog = FALSE;
        g_bAddAlarmOccurredToEventLog = FALSE;
        g_bAddAlarmClearedToEventLog = FALSE;
        g_bNoParityHistory = FALSE;
        g_nUSBReadTimeoutCtr = 0;
        g_nUSBCommandSendTimeoutCtr = 0;
        g_nUSBFlushTimeoutCtr = 0;
        g_nUSBWriteTimeoutCtr = 0;
        g_nUSBEchoedTimeoutCtr = 0;
        g_nUSBUSBSendFileCommandsTimeoutCtr = 0;
        g_nUSBGetPrinterStatusTimeoutCtr = 0;
        g_bDoSelfTestDiagnostics = FALSE;
        g_bInLastMinute = FALSE;
        g_nTimeInAutoMinutesCtr = 0;
        g_wHourHistory = 0;
        g_bHourRollOver = FALSE;
        g_nStorageInvervalCtr = 0;
        g_lOrigBatchPulsesAccumulator = 0;
        g_bUseLineSpeedForGPMCalc = FALSE;
        g_bNoLeakAlarmCheck = FALSE;
        g_bHiRegLevelSensorHistory = FALSE;
        g_bDumpFlapHasOpened = FALSE;
        g_bI2CError = FALSE;
        g_wForeGroundCounter = 0;
        g_wProgramLoopEntry = 0;
        g_bPrintModbusMessageToTelnet = FALSE;
        g_bICSRecipeDataWritten = FALSE;
        g_bSaveAdditionalDataToEEPROM = FALSE;
        g_bLockIsEnabled = FALSE;
        g_bPitRunning = FALSE;
        g_bConfigIsLocked = FALSE;
        g_nHoldOffEventLogWriteCtr = 0;
        g_bAtoDEndOfConversionErrorHistory = FALSE;
        g_bAtoDNegativeSignHistory = FALSE;
        g_nWaitForSPIBusToSettleCtr = 0;
        g_bDoAToDTest = FALSE;
        g_nBlenderTypeHistory = g_CalibrationData.m_nBlenderType;
        g_fCalibrationWeight = 0.0f;
        g_bSelfTestInProgress = FALSE;
        g_bGotHalfGramTime = FALSE;
        g_fMaxKgPHrPossible = 0.0f;
        g_nAfterEndOfCycleCounter = 0;
       g_nVAC8ExpansionIndex = 0;
        g_nVAC8ExpansionOutput = 0;
        g_nVaccumLoadingVAC8IOCards = 0;
        g_bLogLoadCellReading = FALSE;
        g_nDelayBeforeLoadCellLogCtr = 0;
        g_nAToDStabilityCtr = 0;
        g_nAToDStabilityMaxValue = 0;
        g_nAToDStabilityMinValue = 0;
        g_bTakeRefAToDReading = FALSE;
        g_nDelayBeforeRefLogCtr = 0;
        g_nAToDRefStabilityCtr = 0;
        g_nAToDRefStabilityMaxValue = 0;
        g_nAToDRefStabilityMinValue = 0;
        g_bRefAToDStabilityTestInProgress = FALSE;
        g_bDecideOnRefStability = FALSE;
        g_bDecideOnLoadCellStability = FALSE;
        g_nRefTimeoutCtr = 0;
        g_fWeightLeftAfterRegrind = 0.0f;
        g_fRegrindWeight = 0.0f;
        g_bFileCouldNotBeOpenedEvent = FALSE;
        g_nWatchDogCommand = 0;
        g_nWatchDogCommandInNseconds = 0;

        for(i=0; i < UFP2_TABLE_WRITE_SIZE; i++)
    {
        g_arrnWriteUFP2MBTable[i] = 0;
    }

    for(i=0; i < UFP2_TABLE_READ_SIZE; i++)
    {
        g_arrnReadUFP2MBTable[i] = 0;
    }

    for(i=0; i < SEI_TABLE_WRITE_SIZE; i++)
    {
        g_arrnWriteSEIMBTable[i] = 0;
    }

    for(i=0; i < LLS_TABLE_WRITE_SIZE; i++)
    {
        g_arrnWriteLLSMBTable[i] = 0;
    }


    for(i=0; i < LLS_TABLE_READ_SIZE; i++)
    {
        g_arrnReadLLSMBTable[i] = 0;
    }

    for(i=0; i < MULTIBLEND_EXTRUDER_NO; i++)
    {
        g_bMultiBlendRecipeIsZero[i] = FALSE;
    }

    for(i=0; i < MAX_COMPONENTS; i++)
    {
        for(j=0; j < MAX_CYCLE_DIAGNOSTICS_SAMPLES; j++)
        {
            g_fComponentWeightSamples[i][j] = 0;
        }
    }

    for(i=0; i < MB_TABLE_SETPOINT_SIZE; i++)
    {
        g_arrnMBSetpointHistory[i] = 0;
    }

    for(i=0; i < MAXIMUM_ALARMS; i++)
    {
        for(j=0; j < MAX_COMPONENTS; j++)
        {
            g_bAlarmOccurredHistory[i][j] = FALSE;
        }
    }
    for(i=0; i < MAX_COMPONENTS; i++)
    {
        g_nPulsingSequence[i] = 0;
        g_lComponentTargetCountsCopy[i] = 0;
        g_fComponentTargetWeightCopy[i] = 0;
        g_nComponentPulseCounter[i] = 0;
        g_fComponentActualWeightHistory[0] = 0.0f;


    }

    memset( &g_WorkCPIRRData, 0, sizeof( structCPIRRData ) );
    memset( &g_MainCPIRRData, 0, sizeof( structCPIRRData ) );
    memset( &g_Stage2CPIRRData, 0, sizeof( structCPIRRData ) );
    memset( &g_Stage3CPIRRData, 0, sizeof( structCPIRRData ) );
    memset( &g_sSelfTest, 0, sizeof( structSelfTest ) );
    memset( &g_nTSMDebug, 0, sizeof( StructDebug ) );


    ClearHourHistoryStructures(); // set structure to 0
    ClearOngoingHistoryStructures();
    ClearOngoing24HrHistoryStructures();



    g_bWriteIncreaseDecreaseDataToSEI = FALSE;
    g_nPeripheralCardsPresentHistory = 0;

    g_unCurrentFlapStatus1 = 0;
    g_unInProgressStatus = 0;
    for(i=0; i < MULTIBLEND_EXTRUDER_NO; i++)
    {
        g_baCurrentFlapStatus[i] = FALSE;
        g_baInProgress[i] = FALSE;
        g_unaCurrentQueue[i] = 0;
    }
    g_bRan = FALSE;
    g_bExistsInQueue = FALSE;
    g_bClearHistory = FALSE;
    g_bMultiBlendisPaused = FALSE;
    g_nRanx = FALSE;
    g_nEndOfQueue = 0;
    g_nNoCurrentFlaps = 0;
    for(i=0; i < DOWNLOADHISTORYNO; i++)
    {
        g_unaDownLoadHistory[i] = 0;
    }

    ClearCommsCounters();  //
    SetUpConversionFactors();
    g_CurrentTime_Date_History = 0;

    g_bNBBRevB1Hardware = FALSE;
    g_bPrintfRunning = FALSE;
    g_bEnableTxRxInOpenSerial = FALSE;
    CalculateValveLatency();   // CALCULATELATENCY  //nbb--todolp-- load nominal latency value
    g_bT0TriggerMode = FALSE;
    g_nT0InterruptMode = T0_TRIGGER;
    g_nUSTimeSumEntries = 0;
    g_lUSTimeSum = 0;
    g_lUSTimeAvg = 0;
    g_bReadOnNext = FALSE;
    g_nCycleSecondNo = 0;
    g_nUSBufferIndex = 0;
    g_bUltrasonicLIWInstalled = FALSE;
    g_nMultiBlendRecipeSaveCtr = 0;
    SetupTimer0();
    LoadOutputMapping();

    g_ProcessData.m_nResetCounter++;
    g_ProcessData.m_n24HourOngoingResetCounter++;
//    g_CalibrationData.m_bDiagPrintFlag = TRUE;
    g_bSaveAllProcessDataToEEprom = TRUE;
    g_bPITAllowed = TRUE;
    DecideIfExpansionisRequired();
    CopyMultiblendRecipesToMB();
    CopyMultiblendSetupToMB();
    CheckFlowRateChecksum();
    CheckForZeroMultiBlendRecipe();
    CheckCalibrationHash();
    LoadOrderWeightFromVolatileMemory();
    g_nFlashRate = FLASH_RATE_1HZ;              // set up to flash LED at 1Hz.
    g_nProtocol = PROTOCOL_MODBUS;  // Hardcode for modbus protocol.
    g_bFirstTimeToReadScrewSpeed = TRUE;
    g_bCalculateScrewSpeedConstant = FALSE;
    g_nVolumetricEstimateCounter = VOLUMETRICESTIMATETIMEINSECONDS;
    g_bFirstOptimisationCheck = TRUE;
    g_nWriteHoldOffCounter = SEIWRITEHOLDOFFTIMEINSECONDS;
    g_nSaveProcessDataToEEPROMCounter = NOOFWRITESTOEEPROM;
    g_arrnWriteSEIMBTable[MB_SEI_STARTUP_RELAY_DA] = 204;
    g_nSEIWriteRequiredCounter = SENDCALDATATOSEINO;   // this activates the long write message to sei
    g_cPanelVersionNumber[PANEL_REVISION_STRING_SIZE-1] = '\0';
    g_cModbusEventBuffer[0] = '\0';
    for(i= 0; i < MAX_COMPONENTS; i++)  //nbb--todo--put back remove this
    {
        g_CalibrationData.m_wFillingMethod[i] = 0;
        g_nPulsingSequence[i] = 0;
    }
//    if(g_CalibrationData.m_nMaxThroughput == 2758)  //nbb--todo--put back check
//    {
//       g_CalibrationData.m_wFillingMethod[2] = FILLING_METHOD_PULSING_MODE;
//        g_bCalibratePulsing[2] = TRUE;// nbb--todo--remove
//    }

    g_bFirstPulse = FALSE;
    g_bFastSettling = FALSE;
    g_nLoadersHistory = g_VacCalibrationData.m_nLoaders;
    g_nVac8ExpansionHistory = g_CalibrationData.m_bVac8Expansion;
    g_nTopUpCompNoRefHistory = g_CalibrationData.m_nTopUpCompNoRef;
     if((g_OnBoardCalData.m_cPermanentOptions & LICENSE_VACUUMLOADING) == 0)
     {
         if(g_VacCalibrationData.m_nLoaders != 0)
         {
             g_VacCalibrationData.m_nLoaders = 0;
             g_bVacuumLoaderCalibrationWritten = TRUE;
         }
     }
     ReadSHAHashes();// security--tsm-- read hash information
     ReadDS2432ID( DS2432Data.m_cIDBuf );
     AssembleSecretData( DS2432Data.m_cSecretBuf, DS2432Data.m_cIDBuf );

//nbb--todo--put back    if(!g_bLoadConfigDataCheckSumOkay)
//    {
//        PutAlarmTable( CONFIGCHECKSUMALARM,  0 );
//    }
//    else
//    {
//         RemoveAlarmTable( CONFIGCHECKSUMALARM,  0 );
//    }

}


/*
PSTART:         ;SEI
BLBGND:                         ; LDS    #STACK
       JSR    SERWATDOG         ;SERVICE SOFTWARE WATCHDOG (ON HC16)
;

        LDX     #STARTCL        ;
REPCLR  CLR     0,X
        AIX     #1
        CPX     #ENDCLEAR
        BNE     REPCLR
        JSR     CLRREMRAM

        LDX     #STARTCLEAR2        ;
REPCLR2 CLR     0,X
        AIX     #1
        CPX     #ENDCLEAR2
        BNE     REPCLR2

        LDX     #STARTCLEANRAM        ;
REPCLR3 CLR     0,X
        AIX     #1
        CPX     #ENDCLEANRAM
        BNE     REPCLR3







       CLR    BGNFLG            ;~FLAG MUST BE SET TO ALLOW A/D READING
       CLR    STBGNFLG

       CLR    EPRBLK
       LDAB   #20
       STAB   TOSTALTIM         ; DISABLE EXTRUDER STALL CHECK FOR 20 SECS
       CLR    FONFLAG           ;~CLEAR FLAG USED TO TURN FILL'S ON
       CLR    NFILCMD           ;~NETWORK FILL COMMAND BYTE
       CLR    LFILCMD           ;~LONG FILL
       JSR    ALRMOF            ;~TURN ALARM O/P OFF
       JSR    RCINIT            ;~INITIALISE REEL CHANGE VARIABLES
;  INITIALISE THE FRONT PANEL
;       LDAA   #$D3
;       STAA   FIO792            ;~RESET THE COM REG.
;       LDAA   #$2A
;       STAA   FIO792            ;~REFRESH RATE
;       CLR    KYBAMF            ;~CLR KEYBD MAN CONTR. WORD
       CLR    IKYDOWN           ;~INC KEY DOWN FLAG
       CLR    DKYDOWN           ;~DEC  "    "
       CLR    MCHGFLG           ;~MANUAL CHANGE(INC/DEC) OCCURRED FLAG
;       LDAA   #$08
;       STAA   FIO792            ;~SET MODE
;       LDAA   #$C2              ;~RESET 8279 FIFO & STATUS RREG
;       STAA   FIO792
;*      JSR    RSFPCT   SUB IN FPHND FILE

;;      JSR    INPRAM   **PRINT  **TESTPURPOSESONLY**
;;TESTAA2       JSR    TESTSSG  TEST THE SEVEN SEG LED'S

       CLR    TENCNT            ;~INITIALISE THE 10HZ COUNTER
       CLR    TSECT3            ;~KEY DEBOUNCE COUNTER (10HZ)
       CLR    MINFLG            ;~CLEAR MINUTE ROLL-OVER FLAG
       CLR    MIN4FLG           ; 4 MIN         "
       CLR    SECFLG            ;~CLR SECOND ROLL-OVER FLAG
       CLR    TWOSFLG           ;~TWO SECOND FLAG
       CLR    FIVSFLG           ;~5 SEC. FLAG
       CLR    TSCFLG            ;~10 SEC. FLAG
       CLR    HOURFG            ;~CLR HOUR ROLLOVER FLAG
       CLR    SECCNT            ;~CLR SECONDS COUNTER
       CLR    MINCNT            ;~CLR MINUTES COUNTER
       CLR    EXTSCTR           ;~EXTRUDER STALL COUNTER

       JSR    STRPUPT           ;~STORE POWER UP TIME
;

       JSR    RD_OPTS           ; READ OPTIONS(LINKS) ON MCU16
       LDAA   #$80
       TST    MCU16_L5
       BEQ    SETPROCL
       CLRA
SETPROCL STAA   PROTOCOL                ; SERIAL COMMS PROTOCOL (L6)


       JSR      INIT550         ; INITIALISE 550
       JSR    SIORES            ;~ÿNIT BUFFERS ETC. USED IN NETWORK PROG.
;       JSR    RD_OPTS           ; READ OPTIONS(LINKS) ON MCU16
JJTST1 JSR    INITRTC           ;~INITIALISE THE RTC

       JSR    CLARAM            ; INIT LCD ATTR. RAM
       JSR    INILCD            ;~INITIALISE 5010N LCD MODULE
       JSR    LCDTST            ;~TEST GRAPHICS LCD
       JSR    DLYP1SEC          ;~.1 SECOND PAUSE FOR VIEWING
;       JSR    DLYP2SEC          ;~.2 SECOND PAUSE FOR VIEWING
;
       JSR    CLRLCD            ;~CLEAR LCD MODULE DISPLAY RAM
;       JSR    DEL1S

       CLR    CALMOD            ;~CALMOD = $AA, IN CALIBRATION
;;       CLR    PGMNUM   PROG. NO. (KEY HANDLER)
       CLR    CPGMNO            ;~Set calib. prog no. = 0
       CLR    TAREFG            ;~TARE FLAG (NONZERO CAUSES TARE OP.)
       CLR    AMSWNO            ;~AUTO/MANUAL SWITCH DEBOUNCE COUNTER
       CLR    MANATO            ;~SET MAN/AUTO FLAG TO MANUAL
       JSR    AMLMPOF           ;~TURN AUTO/MAN LAMP OFF
       CLR    EATOFG            ;~ENTERING AUTO FLAG
       CLR    SCFLAG            ;~SCREW CALIB. FLAG > NOT CALIBRATING
;      JSR    OFLINIT                   ;~INITIALISE FOR OFF-LINE BLENDER (HG INPUTS)
;
       JSR     SETALPTR        ; SET ALARM TABLE POINTERS
       JSR    INALTB            ;~INITIALISE ALARM TABLE
       JSR    LODCEE            ;~LOAD CAL. DATA FROM EEPROM
       JSR    LODCAL2           ; LOAD CALIBRATION BLOCK #2
       JSR    CHECKCD           ; CHECK CAL. DATA

       LDD      MBPROALRM       ; READ ALARMS.
       PSHM     D               ; SAVE
       JSR     INITMBTB        ; INIT. MODBUS TABLE
       PULM     D
       STD      MBPROALRM

       LDAA     #PAUSETIME     ;
       STAA     PAUSETIMER     ; PAUSE TIME
        LDAB    #00
NOTNEW  TST     MCU16_L4        ; NO MAINS SIGNIFICATION
        BEQ     NOTL4
        ORAB    #$01
NOTL4   TST     MCU16_L3        ; CMR UNIT INSTALLED.
        BEQ     NOTL3           ; NO
        ORAB    #02             ; SET CMR BIT IN VERSION NO.
NOTL3   TST     MCU16_L5        ;
        BEQ     STOVNUM         ; NO
        ORAB    #04             ; ACTIVATE O/P 8 AS WELL AS COMP 1
STOVNUM STAB    VERNUMSTO       ; STORE VERSION NO.
        JSR    INITPRNT          ; INIT. PRINT VARIABLES
        JSR     SCIMODINIT
        JSR    CHK100
        LBCC    CONT001A
        LDX      #PCNT1
        LDAB   #RAMBANK
        TBZK                    ;~;SET EK FOR BANK 1, I.E. RAM
        LDZ      #BADPCNT1
        JSR      TRFERXTZ
        JSR      CHK100_BAK
        BCS      DISERROR
        LDAA     #USEOLD            ; USE OLD SET POINTS.
        STAA     NEXTPAGE
        LDAA     #1             ;
        STAA     PAUSETIMER     ; PAUSE TIME
        LDX      #BAKPCNT1
        LDAB   #RAMBANK
        TBZK                    ;~;SET EK FOR BANK 1, I.E. RAM
        LDZ      #PCNT1          ;
        JSR      TRFERXTZ        ; TRANSFER X TO Z FOR BLEND NO..
        LDAA     BAKREG_PRES
        STAA     REG_PRESENT
        BRA      ALON
DISERROR:
       LDD    #$9999            ; 9999 IS REPRESENTATION USED FOR 100% (2DP)
       STD    PCNT1
       CLRW   PCNT2
       CLRW   PCNT3
       CLRW   PCNT4
       CLRW   PCNT5
       CLRW   PCNT6
       CLRW   PCNT7
       CLRW   PCNT8
       CLRW   PCNT9
       CLRW   PCNT10
       CLRW   PCNT11
       CLRW   PCNT12
       LDAA     #1             ;
       STAA     PAUSETIMER     ; PAUSE TIME
       LDAA     #1
       STAA     PAUSFLG
       LDAA     #INVPAUSE      ; USE OLD SET POINTS.
       STAA     NEXTPAGE
ALON   LDAA    #PERALARM
       JSR     PTALTB          ; TO ALARM TABLE
       JSR      PRINTPER        ; PRINT %
       BRA      NOSUMM

CONT001A LDAA    MBPROALRM               ;
         ANDA    #PERALBIT
         BEQ     NOPERAL                 ; NO TRANSITION
         LDAA    #PERALARM
         JSR     RMALTB                     ;~
NOPERAL:
       LDX      #PCNT1          ;
       LDAB   #RAMBANK
       TBZK                    ;~;SET EK FOR BANK 1, I.E. RAM
       LDZ      #BAKPCNT1
       JSR      TRFERXTZ        ; TRANSFER X TO Z FOR BLEND NO..
       LDAA     REG_PRESENT
       STAA     BAKREG_PRES
       JSR     INITPRT         ; INITIALISE PARALEL PRINTER.
       JSR     SCIMODINIT
       JSR      PRINTRESETSOURCE ;
CONTNOP JSR    INLIQAD  ;LIQ ADDIT.
NOSUMM:

        JSR     STARTUPPAGE     ; SHOW START UP PAGE.

       JSR    TIMEOFF           ;~HAS MACHINE BEEN OFF FOR > 2 MINS
       BCC    CONT002           ;~NO, THEREFORE SKIP MOTOR SETTING
       LDD    #0                ;~VALUE FOR 12BIT = 4095 D/A
       STD      DAC1TMP
       CLR      PAUSFLG
       STD    DA1TMP            ;~SET ALL DAC VALUES TO ONE VALUE.
       STD    DA2TMP
       STD    DA3TMP
       STD    DA4TMP
       STD    DA5TMP
       STD    DA6TMP
       STD    LADATMP
       LDE    DAPRPM
       LDAB   DAPRPM+2
       STE    MDAPRPM
       STAB   MDAPRPM+2         ; MOVE D/A PER RPM FROM EEPROM -> MDAPRPM

CONT002 JSR    CALCVS                   ;~CALCULATE CONTROL VOLTAGE VALUES (0-999)
;       JSR    RIANBS            ;~RE-INIT ANALOG DATA BUF'S
       CLR    FNFLAG            ;~CLR A/D CONVERSION FINISHED FLAG
       CLR    CALFLG            ;~CAL. DATA CHANGE FLAG
       CLR    CEEFLG            ;~CAL. DATA > EEPROM FLAB
       CLR    DCP2FG            ;~CA. PAGE 2 DISPLAY DATA FLG
       LDX    #SP1KGH           ;~SET-PT. FOR BLEND 1 IN KG/H
       LDAB   #12               ;~CLEAR 4 SETPT.S 3 BYTES EACH
       JSR    CLRBBS            ;~CLEAR ACC. B BYTES FROM X
       LDAA   #$AA
       STAA   PCTFLG             ;~%'S CHANGE FLAG, SET TO GENERATE C3'S
       STAA   PCTHIS            ; SET HISTORY FLAG.
       CLR    CSSLSFLG          ; FLAG TO CALC. SS/LS RATIO
       STAA   NCONSS            ;~SET FLAG TO GENERATE NEW MAX DAC #1 VALUE
       CLR    NDAPRPM           ;~CLEAR NEW D/A PER RPM VALUE FLAG
       STAA   CSAFLG            ;~SET FLAG TO CALC SETPT FOR BLEND A (1)
       JSR    LSINIT            ;~INITIALISE FOR LINE SPEED CALC.'S
       CLR    CALIFG            ;~CALIBRATION FLAG, SET TO AA WHEN IN CALIBRATIO
;;T       JSR    ACUMIN            ;~INITIALISE WEIGHT & LENGTH ACCUMULATED VALUES
       CLR    NEWCAL            ;~NEW CALIB. DATA DOWNLOADED FLAG
       CLR    NEWSET            ;~NEW SETPT. DATA DOWNLOADED FLAG
       CLR    MFILFG            ;~CLR MANUAL FILL ON FLAG
       CLR      LFILFG
       CLR      UPDK1FLG
       CLR    EDMPFLG           ;~CLEAR EMERGENCY DUMP FLAG
       LDAB   #40
       STAB   EDMPLAG           ;~DISABLE EM. DUMP INPUT FOR 40 SEC.S
       CLR    RSTOFLAG          ; ORDER TOTALS RESET FLAG (AT RC)
;
        LDAB    #1
        STAB    RESETFLG        ; SET RESET OCCURRED FLAG

        LDAA    #$0F
        STAA    RESETAD        ;A/D RESET FLAG
        STAA    FIRSTBATCH      ; SET 1ST BATCH FLAG
        STAA    FIRSTWCYCLE     ; INDICATE 1ST WEIGH CYCLE
        JSR     CLOSEDUMP       ; CLOSE DUMP
        LDD     #PITINDEX
        STD     PREVPITM        ; SET PREV PIT TO FULL PIT INDEX.

        LDAA    #LEVPOLL
        STAA    BATSEQ          ; SET BATCH SEQUENCER TO START INTIAL FILLING

        LDAA    PASWRD
        CMPA    #02
        BNE     RESVOL
        LDD     PASWRD+1
        CPD     #$2443
        BEQ     CONFOR
RESVOL  CLR     ISVOLMODE                ; NO VOLUMETRIC MODE.

CONFOR  LDAA    PASWRD
        BNE     CONRESET
        LDD     PASWRD+1
        CPD    #$6553
        BNE     CONRESET


CHKDIA  LDAA    DIAGMODE        ; CHECK DIAG MODE
CHKMODE CMPA    #ACPROG         ; AUTOCYCLE ENABLED
        BNE     NOTAC
        LDAA    #$0F
        STAA    AUTOCYCLEFLAG
        BRA     CONRESET
NOTAC:  CMPA    #TESTPROG1
        BEQ     ISDIAG
        BLS     CONRESET
ISDIAG  LDAA    #$0F
        STAA    OUTPUTDIAG


CONRESET:
        JSR     FORMSEQTABLE    ; TABLE
        JSR     SETUPDIAGPTR
        TST     AUTOCYCLEFLAG   ; IS AUTOCYCLE ENABLED. ?
        BEQ     NOAUTOCYCLE     ; NO
        LDAA    #AUTOCYCLETARE  ; SET INITIAL TARE
        STAA    BATSEQ          ;
        CLR     AUTOCYCLEFLAG   ; ALLOW NORMAL READING OF A/D
NOAUTOCYCLE:
;        JSR     STMUX132
        JSR     SEQINIT
;        TST     OXXLINE         ; OFFLINE ?
        LDAA    BLENDERMODE     ; CHECK MODE.
        CMPA    #OFFLINEMODE    ; OFFLINE ?
        BNE     ONLINE          ; NO
        JSR     CLOSEFEED
        LDAA    #OFFDELADMPID   ; SEQUENCE AFTER DUMP.
        STAA    OFFLINE_TRK
ONLINE  JSR     RESETSTART      ; RESET CURSTARTTIME
        CLRW    BPCNT1
        CLRW    BPCNT2
        CLRW    BPCNT3
        CLRW    BPCNT4
        CLRW    BPCNT5
        CLRW    BPCNT6
        CLRW    BPCNT7
        CLRW    BPCNT8
        CLRW    BPCNT9
        CLRW    BPCNT10
        CLRW    BPCNT11
        CLRW    BPCNT12
        CLR     AVECPIB
        CLR     STRUPF
        JSR     MIXERON
        JSR     RESRROB
        JSR     INITCURPREV     ; INITIALISE CURRENT AND PREVIOUS VALUES.
        LDAA    #$0F
        STAA    CMRNOW
        LDAA    #3
        STAA    ESTCTRUSE       ; USE ESTIMATION FOR THE 1ST FEW CYCLES.
        LDAA    #1
        STAA    READDAVALUEF
        STAA    SEIPOLLINPUTSF

        STAA    SEIFIRSTPULSER  ; SET FIRST PULSE FLAG.
        STAA    UPDATEREGHILOF  ; UPDATE OF HILO REG RUN IN 1ST SECOND
        LDAA    BLENDERTYPE     ; CHECK BLENDER TYPE
        CMPA    #TINYTYPE       ; TINYTYPE MESSAGE.
        BNE     NOTTNY          ; LEAVE MIXER ON.
        STAA    MIXOFF          ;
NOTTNY  JSR     CMXDA1          ; CALCULATE MAX EXTRUDER SETPOINT.
        JSR     CHECKSTDSETUP   ; CHECK FOR STANDARD CONFIG SET UP.
        CLR     KYDATA
        LDAA    #1
        STAA    CHKIFCYCLEFLAG

        LDX     #CALBLK         ; CALIBRATION BLOCK START ADDRESS
        LDAB    #OFFSOF         ;
        ABX
        LDD     0,X
        STD     HOPPERTARECOUNTS
        LDAA    2,X
        STAA    HOPPERTARECOUNTS+2

        LDAB   #HARDWAREBANK                   ; HARDWARE BANK
        TBZK                                   ; STORE BANK NO
        LDZ      #PICR
;        LDD     #$0738
        LDD     #$0638
        STD     0,Z              ; store high ram array, bank F

        JSR     RESETCLEANBITS
        JSR     CORNERCLEANLEFTOFF
        JSR     CORNERCLEANRIGHTOFF
        JSR     CHECKFORCLEANOPTION
        JSR     INIT_MAX7301
        LDAA    #1
        STAA    VENTURIFIRSTTIME
        LDAA    TOPUP
        STAA    TOPUPF          ; STORE TOP UP
        LDAA    #1
        STAA    VENTURIFILLBYWEIGHTH    ; VENTURI FILL BY WEIGHT HISTORY.
        JSR     TESTEXPANBOARD
        ANDP    #$FF1F          ;CLI                      ;~ENABLE IRQ'S


;
;  FOLLOWING IS BACKGROUND PROGRAM, READING AND DISPLAYING DATA IS
;  HANDLED UNDER INTERRUPT CONTROL.
PGLOOP  JSR    STORTIM           ;~STORE TIME (HOURS/MINS)

mce12-15


SIMVALU FCB   $00,$03,$24               ;~A/D SIM. VALUE = 324 DECIMAL
       EVEN                             ;ALIGN TO EVEN ADDRESS
;
PSTART:         ;SEI
BLBGND:                         ; LDS    #STACK
       JSR    SERWATDOG         ;SERVICE SOFTWARE WATCHDOG (ON HC16)
;

        LDX     #STARTCL        ;
REPCLR  CLR     0,X
        AIX     #1
        CPX     #ENDCLEAR
        BNE     REPCLR
        JSR     CLRREMRAM

        LDX     #STARTCLEAR2        ;
REPCLR2 CLR     0,X
        AIX     #1
        CPX     #ENDCLEAR2
        BNE     REPCLR2

        LDX     #STARTCLEANRAM        ;
REPCLR3 CLR     0,X
        AIX     #1
        CPX     #ENDCLEANRAM
        BNE     REPCLR3







       CLR    BGNFLG            ;~FLAG MUST BE SET TO ALLOW A/D READING
       CLR    STBGNFLG

       CLR    EPRBLK
       LDAB   #20
       STAB   TOSTALTIM         ; DISABLE EXTRUDER STALL CHECK FOR 20 SECS
       CLR    FONFLAG           ;~CLEAR FLAG USED TO TURN FILL'S ON
       CLR    NFILCMD           ;~NETWORK FILL COMMAND BYTE
       CLR    LFILCMD           ;~LONG FILL
       JSR    ALRMOF            ;~TURN ALARM O/P OFF
       JSR    RCINIT            ;~INITIALISE REEL CHANGE VARIABLES
;  INITIALISE THE FRONT PANEL
;       LDAA   #$D3
;       STAA   FIO792            ;~RESET THE COM REG.
;       LDAA   #$2A
;       STAA   FIO792            ;~REFRESH RATE
;       CLR    KYBAMF            ;~CLR KEYBD MAN CONTR. WORD
       CLR    IKYDOWN           ;~INC KEY DOWN FLAG
       CLR    DKYDOWN           ;~DEC  "    "
       CLR    MCHGFLG           ;~MANUAL CHANGE(INC/DEC) OCCURRED FLAG
;       LDAA   #$08
;       STAA   FIO792            ;~SET MODE
;       LDAA   #$C2              ;~RESET 8279 FIFO & STATUS RREG
;       STAA   FIO792
;*      JSR    RSFPCT   SUB IN FPHND FILE

;;      JSR    INPRAM   **PRINT  **TESTPURPOSESONLY**
;;TESTAA2       JSR    TESTSSG  TEST THE SEVEN SEG LED'S

       CLR    TENCNT            ;~INITIALISE THE 10HZ COUNTER
       CLR    TSECT3            ;~KEY DEBOUNCE COUNTER (10HZ)
       CLR    MINFLG            ;~CLEAR MINUTE ROLL-OVER FLAG
       CLR    MIN4FLG           ; 4 MIN         "
       CLR    SECFLG            ;~CLR SECOND ROLL-OVER FLAG
       CLR    TWOSFLG           ;~TWO SECOND FLAG
       CLR    FIVSFLG           ;~5 SEC. FLAG
       CLR    TSCFLG            ;~10 SEC. FLAG
       CLR    HOURFG            ;~CLR HOUR ROLLOVER FLAG
       CLR    SECCNT            ;~CLR SECONDS COUNTER
       CLR    MINCNT            ;~CLR MINUTES COUNTER
       CLR    EXTSCTR           ;~EXTRUDER STALL COUNTER

       JSR    STRPUPT           ;~STORE POWER UP TIME

       JSR    RD_OPTS           ; READ OPTIONS(LINKS) ON MCU16
       TST    MCU16_L5
       BEQ    NOPSE
       JSR    PAUSEON           ; PAUSE BLENDER IMMEDIATELY.
NOPSE:
       LDAA   #$80
       STAA   PROTOCOL                ; SERIAL COMMS PROTOCOL (L6)
       JSR      INIT550         ; INITIALISE 550
       JSR    SIORES            ;~ÿNIT BUFFERS ETC. USED IN NETWORK PROG.
;       JSR    RD_OPTS           ; READ OPTIONS(LINKS) ON MCU16
JJTST1 JSR    INITRTC           ;~INITIALISE THE RTC

       JSR    CLARAM            ; INIT LCD ATTR. RAM
       JSR    INILCD            ;~INITIALISE 5010N LCD MODULE
       JSR    LCDTST            ;~TEST GRAPHICS LCD
       JSR    DLYP1SEC          ;~.1 SECOND PAUSE FOR VIEWING
;       JSR    DLYP2SEC          ;~.2 SECOND PAUSE FOR VIEWING
;
       JSR    CLRLCD            ;~CLEAR LCD MODULE DISPLAY RAM
;       JSR    DEL1S

       CLR    CALMOD            ;~CALMOD = $AA, IN CALIBRATION
;;       CLR    PGMNUM   PROG. NO. (KEY HANDLER)
       CLR    CPGMNO            ;~Set calib. prog no. = 0
       CLR    TAREFG            ;~TARE FLAG (NONZERO CAUSES TARE OP.)
       CLR    AMSWNO            ;~AUTO/MANUAL SWITCH DEBOUNCE COUNTER
       CLR    MANATO            ;~SET MAN/AUTO FLAG TO MANUAL
       JSR    AMLMPOF           ;~TURN AUTO/MAN LAMP OFF
       CLR    EATOFG            ;~ENTERING AUTO FLAG
       CLR    SCFLAG            ;~SCREW CALIB. FLAG > NOT CALIBRATING
;      JSR    OFLINIT                   ;~INITIALISE FOR OFF-LINE BLENDER (HG INPUTS)
;
       JSR     SETALPTR        ; SET ALARM TABLE POINTERS
       JSR    INALTB            ;~INITIALISE ALARM TABLE
       JSR    LODCEE            ;~LOAD CAL. DATA FROM EEPROM
       JSR    LODCAL2           ; LOAD CALIBRATION BLOCK #2
       JSR    CHECKCD           ; CHECK CAL. DATA

       LDD      MBPROALRM       ; READ ALARMS.
       PSHM     D               ; SAVE
       JSR     INITMBTB        ; INIT. MODBUS TABLE
       PULM     D
       STD      MBPROALRM

       LDAA     #PAUSETIME     ;
       STAA     PAUSETIMER     ; PAUSE TIME
        LDAB    #00
NOTNEW  TST     MCU16_L4        ; NO MAINS SIGNIFICATION
        BEQ     NOTL4
        ORAB    #$01
NOTL4   TST     MCU16_L3        ; CMR UNIT INSTALLED.
        BEQ     NOTL3           ; NO
        ORAB    #02             ; SET CMR BIT IN VERSION NO.
NOTL3   TST     MCU16_L5        ;
        BEQ     STOVNUM         ; NO
        ORAB    #04             ; ACTIVATE O/P 8 AS WELL AS COMP 1
STOVNUM STAB    VERNUMSTO       ; STORE VERSION NO.
        JSR    INITPRNT          ; INIT. PRINT VARIABLES
        JSR     SCIMODINIT
        JSR    CHK100
        LBCC    CONT001A
        LDX      #PCNT1
        LDAB   #RAMBANK
        TBZK                    ;~;SET EK FOR BANK 1, I.E. RAM
        LDZ      #BADPCNT1
        JSR      TRFERXTZ
        JSR      CHK100_BAK
        BCS      DISERROR
        LDAA     #USEOLD            ; USE OLD SET POINTS.
        STAA     NEXTPAGE
        LDAA     #1             ;
        STAA     PAUSETIMER     ; PAUSE TIME
        LDX      #BAKPCNT1
        LDAB   #RAMBANK
        TBZK                    ;~;SET EK FOR BANK 1, I.E. RAM
        LDZ      #PCNT1          ;
        JSR      TRFERXTZ        ; TRANSFER X TO Z FOR BLEND NO..
        LDAA     BAKREG_PRES
        STAA     REG_PRESENT
        BRA      ALON
DISERROR:
       LDD    #$9999            ; 9999 IS REPRESENTATION USED FOR 100% (2DP)
       STD    PCNT1
       CLRW   PCNT2
       CLRW   PCNT3
       CLRW   PCNT4
       CLRW   PCNT5
       CLRW   PCNT6
       CLRW   PCNT7
       CLRW   PCNT8
       CLRW   PCNT9
       CLRW   PCNT10
       CLRW   PCNT11
       CLRW   PCNT12
       LDAA     #1             ;
       STAA     PAUSETIMER     ; PAUSE TIME
       LDAA     #1
       STAA     PAUSFLG
       LDAA     #INVPAUSE      ; USE OLD SET POINTS.
       STAA     NEXTPAGE
ALON   LDAA    #PERALARM
       JSR     PTALTB          ; TO ALARM TABLE
       JSR      PRINTPER        ; PRINT %
       BRA      NOSUMM

CONT001A LDAA    MBPROALRM               ;
         ANDA    #PERALBIT
         BEQ     NOPERAL                 ; NO TRANSITION
         LDAA    #PERALARM
         JSR     RMALTB                     ;~
NOPERAL:
       LDX      #PCNT1          ;
       LDAB   #RAMBANK
       TBZK                    ;~;SET EK FOR BANK 1, I.E. RAM
       LDZ      #BAKPCNT1
       JSR      TRFERXTZ        ; TRANSFER X TO Z FOR BLEND NO..
       LDAA     REG_PRESENT
       STAA     BAKREG_PRES
       JSR     INITPRT         ; INITIALISE PARALEL PRINTER.
       JSR     SCIMODINIT
       JSR      PRINTRESETSOURCE ;
CONTNOP JSR    INLIQAD  ;LIQ ADDIT.
NOSUMM:

        JSR     CHECKPOWERUPSTATUS      ; CHECK POWER UP STATUS.

        LDAB    #BANK1
        TBYK
        LDY     #ENCRYPTCODE
        LDD     0,Y                     ; READ ENCRYPTION ENABLE
        CPD     #OPTIONSENABLEDID
        BEQ     NOEN                    ; DO NOT ENABLE OPTIONS
        LDD     OPTIONCODE
        CPD     #$FFFF
        BEQ     NOEN                    ; ALREADY SET.
        LDD     #$FFFF
        STD     OPTIONCODE             ; ENABLE ALL OPTIONS
        LDAA     #$AA
        STAA     SAVECAL2DATA1F           ; INITIATE CALIBRATION DATA SAVE
NOEN:

        JSR     DECIDEONEXPIRYPAGE
        LDAA    PGMNUM
        CMPA    #EXPIRYPAGEID   ; HAS THE EXPIRY PAGE BEEN DISPLAYED?
        BEQ     NOSTUPP
       JSR     STARTUPPAGE     ; SHOW START UP PAGE.
NOSTUPP JSR    TIMEOFF           ;~HAS MACHINE BEEN OFF FOR > 2 MINS
       BCC    CONT002           ;~NO, THEREFORE SKIP MOTOR SETTING
       LDD    #0                ;~VALUE FOR 12BIT = 4095 D/A
       STD      DAC1TMP
       CLR      PAUSFLG
       STD    DA1TMP            ;~SET ALL DAC VALUES TO ONE VALUE.
       STD    DA2TMP
       STD    DA3TMP
       STD    DA4TMP
       STD    DA5TMP
       STD    DA6TMP
       STD    LADATMP
       LDE    DAPRPM
       LDAB   DAPRPM+2
       STE    MDAPRPM
       STAB   MDAPRPM+2         ; MOVE D/A PER RPM FROM EEPROM -> MDAPRPM

CONT002 JSR    CALCVS                   ;~CALCULATE CONTROL VOLTAGE VALUES (0-999)
;       JSR    RIANBS            ;~RE-INIT ANALOG DATA BUF'S
       CLR    FNFLAG            ;~CLR A/D CONVERSION FINISHED FLAG
       CLR    CALFLG            ;~CAL. DATA CHANGE FLAG
       CLR    CEEFLG            ;~CAL. DATA > EEPROM FLAB
       CLR    DCP2FG            ;~CA. PAGE 2 DISPLAY DATA FLG
       LDX    #SP1KGH           ;~SET-PT. FOR BLEND 1 IN KG/H
       LDAB   #12               ;~CLEAR 4 SETPT.S 3 BYTES EACH
       JSR    CLRBBS            ;~CLEAR ACC. B BYTES FROM X
       LDAA   #$AA
       STAA   PCTFLG             ;~%'S CHANGE FLAG, SET TO GENERATE C3'S
       STAA   PCTHIS            ; SET HISTORY FLAG.
       CLR    CSSLSFLG          ; FLAG TO CALC. SS/LS RATIO
       STAA   NCONSS            ;~SET FLAG TO GENERATE NEW MAX DAC #1 VALUE
       CLR    NDAPRPM           ;~CLEAR NEW D/A PER RPM VALUE FLAG
       STAA   CSAFLG            ;~SET FLAG TO CALC SETPT FOR BLEND A (1)
       JSR    LSINIT            ;~INITIALISE FOR LINE SPEED CALC.'S
       CLR    CALIFG            ;~CALIBRATION FLAG, SET TO AA WHEN IN CALIBRATIO
;;T       JSR    ACUMIN            ;~INITIALISE WEIGHT & LENGTH ACCUMULATED VALUES
       CLR    NEWCAL            ;~NEW CALIB. DATA DOWNLOADED FLAG
       CLR    NEWSET            ;~NEW SETPT. DATA DOWNLOADED FLAG
       CLR    MFILFG            ;~CLR MANUAL FILL ON FLAG
       CLR      LFILFG
       CLR      UPDK1FLG
       CLR    EDMPFLG           ;~CLEAR EMERGENCY DUMP FLAG
       LDAB   #40
       STAB   EDMPLAG           ;~DISABLE EM. DUMP INPUT FOR 40 SEC.S
       CLR    RSTOFLAG          ; ORDER TOTALS RESET FLAG (AT RC)
;
        LDAB    #1
        STAB    RESETFLG        ; SET RESET OCCURRED FLAG

        LDAA    #$0F
        STAA    RESETAD        ;A/D RESET FLAG
        STAA    FIRSTBATCH      ; SET 1ST BATCH FLAG
        STAA    FIRSTWCYCLE     ; INDICATE 1ST WEIGH CYCLE
        JSR     CLOSEDUMP       ; CLOSE DUMP
        LDD     #PITINDEX
        STD     PREVPITM        ; SET PREV PIT TO FULL PIT INDEX.

        LDAA    #LEVPOLL
        STAA    BATSEQ          ; SET BATCH SEQUENCER TO START INTIAL FILLING

        LDAA    PASWRD
        CMPA    #02
        BNE     RESVOL
        LDD     PASWRD+1
        CPD     #$2443
        BEQ     CONFOR
RESVOL  CLR     ISVOLMODE                ; NO VOLUMETRIC MODE.
        JSR     CALCULATEEEPROMCHECKSUM

CONFOR  LDAA    PASWRD
        BNE     CONRESET
        LDD     PASWRD+1
        CPD    #$6553
        BNE     CONRESET


CHKDIA  LDAA    DIAGMODE        ; CHECK DIAG MODE
CHKMODE CMPA    #ACPROG         ; AUTOCYCLE ENABLED
        BNE     NOTAC
        LDAA    #$0F
        STAA    AUTOCYCLEFLAG
        BRA     CONRESET
NOTAC:  CMPA    #TESTPROG1
        BEQ     ISDIAG
        BLS     CONRESET
ISDIAG  LDAA    #$0F
        STAA    OUTPUTDIAG


CONRESET:
        JSR     FORMSEQTABLE    ; TABLE
        JSR     SETUPDIAGPTR
        TST     AUTOCYCLEFLAG   ; IS AUTOCYCLE ENABLED. ?
        BEQ     NOAUTOCYCLE     ; NO
        LDAA    #AUTOCYCLETARE  ; SET INITIAL TARE
        STAA    BATSEQ          ;
        CLR     AUTOCYCLEFLAG   ; ALLOW NORMAL READING OF A/D
NOAUTOCYCLE:
;        JSR     STMUX132
        JSR     SEQINIT
;        TST     OXXLINE         ; OFFLINE ?
        LDAA    BLENDERMODE     ; CHECK MODE.
        CMPA    #OFFLINEMODE    ; OFFLINE ?
        BNE     ONLINE          ; NO
        JSR     CLOSEFEED
        LDAA    #OFFDELADMPID   ; SEQUENCE AFTER DUMP.
        STAA    OFFLINE_TRK
ONLINE  JSR     RESETSTART      ; RESET CURSTARTTIME
        CLRW    BPCNT1
        CLRW    BPCNT2
        CLRW    BPCNT3
        CLRW    BPCNT4
        CLRW    BPCNT5
        CLRW    BPCNT6
        CLRW    BPCNT7
        CLRW    BPCNT8
        CLRW    BPCNT9
        CLRW    BPCNT10
        CLRW    BPCNT11
        CLRW    BPCNT12
        CLR     AVECPIB
        CLR     STRUPF
        JSR     MIXERON
        JSR     RESRROB
        JSR     INITCURPREV     ; INITIALISE CURRENT AND PREVIOUS VALUES.
        LDAA    #$0F
        STAA    CMRNOW
        LDAA    #3
        STAA    ESTCTRUSE       ; USE ESTIMATION FOR THE 1ST FEW CYCLES.
        LDAA    #1
        STAA    READDAVALUEF
        STAA    SEIPOLLINPUTSF

        STAA    SEIFIRSTPULSER  ; SET FIRST PULSE FLAG.
        STAA    UPDATEREGHILOF  ; UPDATE OF HILO REG RUN IN 1ST SECOND
        LDAA    BLENDERTYPE     ; CHECK BLENDER TYPE
        CMPA    #TINYTYPE       ; TINYTYPE MESSAGE.
        BNE     NOTTNY          ; LEAVE MIXER ON.
        STAA    MIXOFF          ;
NOTTNY  JSR     CMXDA1          ; CALCULATE MAX EXTRUDER SETPOINT.
        JSR     CHECKSTDSETUP   ; CHECK FOR STANDARD CONFIG SET UP.
        CLR     KYDATA
        LDAA    #1
        STAA    CHKIFCYCLEFLAG

        LDX     #CALBLK         ; CALIBRATION BLOCK START ADDRESS
        LDAB    #OFFSOF         ;
        ABX
        LDD     0,X
        STD     HOPPERTARECOUNTS
        LDAA    2,X
        STAA    HOPPERTARECOUNTS+2

        LDAB   #HARDWAREBANK                   ; HARDWARE BANK
        TBZK                                   ; STORE BANK NO
        LDZ      #PICR

        LDD     #$0638
        STD     0,Z              ; store high ram array, bank F

        JSR     RESETCLEANBITS
        JSR     CORNERCLEANLEFTOFF
        JSR     CORNERCLEANRIGHTOFF
        JSR     CHECKFORCLEANOPTION
        JSR     INIT_MAX7301
        LDAA    #1
        STAA    VENTURIFIRSTTIME
        LDAA    TOPUP
        STAA    TOPUPF          ; STORE TOP UP
        LDAA    #1
        STAA    VENTURIFILLBYWEIGHTH    ; VENTURI FILL BY WEIGHT HISTORY.
        JSR     TESTEXPANBOARD
        LDAA     BLENDERMODE
        CMPA    #SINGLERECIPEMODE       ; SINGLE RECIPE MODE
        BNE     NOTSNR                  ; NO
        LDAA    #1
        STAA    CHECKIFVACUUMREQUIREDF  ; CHECK IF VACUUM IS REQUIRED
        JSR     VACINHIBITON
        JSR     OPENFEED                ; OPEN FEED VALVE ON STARTUP
;        JSR     ENDOFCYCLEPAUSEON       ; PAUSE BLENDER
NOTSNR  JSR     CHECKFORIMMEDIATEDATAENTRY      ; CHECK FOR IMMEDIATE DATA ENTRY.
        LDAA    #BLENDERREADYTIME
        STAA    BLENDERREADYCTR
        LDAA    #IGNORELSALARMNO
        STAA    NOLSALARMCTR




; COMPONENT CONFIGURATION SET UP AS STANDARD ON RESET IF STANDARD CONFIGURATION

CHECKSTDSETUP:
        TST     STDCCFG
        BNE     XITCHS
        LDD     #ONE
        STD     COMP1CFG        ; SET STANDARD CONFIG
        ASLD
        STD     COMP2CFG        ;
        ASLD
        STD     COMP3CFG        ;
        ASLD
        STD     COMP4CFG        ;
        ASLD
        STD     COMP5CFG        ;
        ASLD
        STD     COMP6CFG        ;
        ASLD
        STD     COMP7CFG        ;
        ASLD
        STD     COMP8CFG        ;
        ASLD
        STD     COMP9CFG        ;
        ASLD
        STD     COMP10CFG        ;
        ASLD
        STD     COMP11CFG        ;
        ASLD
        STD     COMP12CFG        ;
        JSR     CALCULATEEEPROMCHECKSUM
XITCHS  RTS


file name cleanio.asm

CHECKFORCLEANOPTION:
        LDAA    OPTIONCODE
        ANDA    #CLEANOPTIONBIT         ; CLEAN OPTION SET
        BNE     CLISEN                  ; YES
        CLR     CLEANING                ; RESET CLEANING OPTION
        JSR     CALCULATEEEPROMCHECKSUM
CLISEN  RTS



*/


