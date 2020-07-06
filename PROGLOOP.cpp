///////////////////////////////////////////////////////////
// ProgLoop.c
//
// This is essentially the foreground loop of the batch blender
// Corresponds to PGLOOP in SBBGND18.ASM
// Note: Unlike in the assembler, the CPU does not stay in this loop,
//                      rather it is called continuously in the foregnd.
//
// Entry:       None
//
// Exit:        Void
//
// M.McKiernan                                          23-07-2004
//      First Pass.
//
//      P.Smith                                                 19/08/04
//      if g_stagefillcalc then call
// M.McKiernan                                          03-08-2004
// Removed brackets from first part... (g_CalibrationData.m_bLayering) = g_bLayeringTemp;
// M.McKiernan                                          14-09-2004
// Set g_bRegrindPresent based on g_CurrentRecipe.m_nRegrindComponent when new recipe loaded.
// M.McKiernan                                          29-09-2004
// Call NewTargetWeightEntry() if g_bFillTargetEntryFlag set.
// M.McKiernan                                          15-10-2004
// Added code to load setpoints from Comms but not fully complete  see - if(g_nTransferSetpointsFlag == TRANSFERSPFROMCOMMSID)
// Added code for saving recipe and calibration data to eeprom in foreground.
// M.McKiernan                                          25-10-2004
// Added check for 1 hz roll-over - g_bForegroundSecondRollover
// All programs that need to be called once per second in foreground should be added here
// Call CopyProdSummDataToMB1Hz();
// M.McKiernan                                          26-11-2004
// save the regrind % so that it can be reloaded after use of "High Regrind" %. - see "save-original"
// Calls to ForegroundOneHzProgram(), ForegroundOneHzProgram2() and ForegroundOneMinuteProgram()
// DoDiagnostics() called when g_bDoDiagnosticsFlag set.
//
// P.Smith                                              9/2/05
// Call EstimateKgPerHour if estimation is required
//
// P.Smith                                              9/2/05
// SetPointChange called on copy of set points to current order
// LineSpeedCalculation() and AccumulateSEIPulses() called
//
//
// P.Smith                                              7-04-2005
// Added check for Front / Back roll change.
// ResetRoundRobinCPI implemented by adding set of flags to reset round robin.
//
//
// P.Smith                                              4/7/05
// Uncomment BatchWeightCompensation & CopyFrontRollChangeData
//
// P.Smith                                              12/7/05
// Uncomment CopyBackRollChangeData
//
// P.Smith                                              12/7/05
// Accumulate front / back roll length called
// Copy of hourly report called on Activate hourly flag TRUE.
//
// P.Smith                                              16-08-2005
// Called CheckForBinPresence
//
// P.Smith                                              24-08-2005
// when g_nTransferSetpointsFlag == TRANSFERSPFROMCOMMSID call
// initiatecleancycle
//
//
// P.Smith                                              12/9/2005
// MonitorVacuumSequence() call added
// WaitForMaterialToBeVacuumed() call
//
// P.Smith                                              13/9/2005
// MonitorVacuumSequence() call added
// CheckLowLevelSensorAlarms() called on 2 second call
//
// P.Smith                                              28/9/2005
// Save stage seq data to eeprom ->     g_bSaveAllCalibrationToEEprom = TRUE;
//
// P.Smith                                              4/10/05
// uncomment CalculateScrewSpeed()
// uncomment ResetAllFillAlarms call and add function
//
// P.Smith                              4/10/05
// Rename low priority todos
//
// P.Smith                              5/10/05
// if g_bDoCMRDToAPerKgCalculation set g_bDoCMRDToAPerKgCalculation to FALSE
// and run CalculateDAPerKgCMR
//
// P.Smith                              6/10/05
// Merge CHECKCALCHECKSUM checks for difference in checksum calculated and the
// EEPROM checksum.
// correct CHECKCALCHECKSUM check.
//
// P.Smith                              20/10/05
// set g_nAllowLargeAlarmBand to ALLOWLARGEBANDINCYCLES on % set point change.
// call SetExtruderControlStartup on set point change.
//
// P.Smith                              3/11/05
// Call PollForOuputDiagnostics()  // DODIAG
//.
// P.Smith                              8/11/05
// added CheckForPrintedReport   asm CHKPRFLG
//
// P.Smith                              14/11/05
// Check for non zero flow rate in CheckTableForNewComponent
// If the flow rate is non zero then use this flow rate to
// calculate the new target for this component.
//
// name change g_bHighLevelSensorHistory to g_bBinLevelSensorHistory
//
// P.Smith                              1/12/05
// check for change in perhiperals, if yes call configuration of modbus master
// again.
//
// P.Smith                              5/12/05
// added g_nCurrentExtruderNo = g_CurrentRecipe.m_nExtruder_No; // indicate current extruder no.
// where the recipe data is transferred.
// Remove check on g_ffComponentCPI[j] for 0, causing a problem for now.
//
//
// P.Smith                              5/1/06
// check for change in g_CalibrationData.m_nStageFillEn, if change initiate
// copyflowrates function.
// Ensure that blender only fills by weight if the flow rate is less than a particular value.
// modify CheckTableForNewComponent not to fill by weight if there is a existing flowrate
// calculated for that component.
//
// P.Smith                              10/1/06
// corrected warnings.
// make g_arrcTemp[sizeof(g_CalibrationData)] unsigned chars
// removed     ntemp1 = (int) (100 * g_fExtruderDAValuePercentage);
//
// P.Smith                      16/2/06
// first pass at netburner hardware conversion.
// Bool -> BOOL
// #include <basictypes.h>
// comment out all un compiled functions.
// defined temporarly StructAlarmTable    g_MainAlarmTable;
//
// P.Smith                      20/2/06
// implement SaveAllCalibrationDataToEEprom( );
//
// P.Smith                      21/2/06
// StructAlarmTable    g_MainAlarmTable; now defined in alarm.cpp, make extern here.
// #include "Lspeed.h"
// LineSpeedCalculation();           // call line speed calculation
// AccumulateSEIPulses();              // accumulate length pulses
//
//
// P.Smith                      21/2/06
// call ForegroundOneHzProgram();           // Foreground stuff that needs doing once per second.
// #include "FGOneHz.h"
//
// P.Smith                      22/2/06
// call ForegroundOneHzProgram2();      // this is stuff that originally was on the 10Hz part of PIT when reached 1 second.
// added CheckLowLevelSensorAlarms
//
// P.Smith                      23/2/06
// #include "Polllls.h"
// CheckLowLevelSensorAlarms();
//
// P.Smith                      24/2/06
// NewTargetWeightEntry();     // routine calculates new targets for comp. #1
// #include "BBCalc.h"
// CycleMonitor( );                                                                // batch cycling overview program.
// #include "CycleMonitor.h"
//
//
// P.Smith                      24/2/06
// FormSeqTable(); //Form Sequence table.
// ClearUnused();          // Clear data for unused components.
// #include "MonBat.h"
// #include "SequenceTable.h"
// Comp1stxTotWgt();               //
//
// P.Smith                      28/2/06
// MonitorBatch( );                                                                // batch monitor. (MONBAT)
// AccumulateFrontLength();      // ASM = ACCLT_F
// AccumulateBackLength();       // ASM = ACCLT_B
// ForegroundOneMinuteProgram();           // this is stuff that originally was on the 10Hz part of PIT when reached 1 minute
//
// P.Smith                      28/2/06
// SetExtruderControlStartup(); /* asm = SETSTART */
// #include "Conalg.h"
// EstimateKgPerHour();
// SetCopyFlowRatesCommand();
//
// P.Smith                      19/4/06
// call ReactToChangeInLineSpeed();
//
// P.Smith                      19/4/06
// Call CopyFrontRollChangeData(); // ASM = FRELCHG
// Call CopyBackRollChangeData(); // ASM = BRELCHG
// call InitiateCleanCycle();   // ASM = INITIATECLEANCYCLE
// call MonitorVacuumSequence();   // ASM VACMON
// #include "Vaccyc.h"
// call CheckIfVacuumRequired();      // ASM = CHECKIFVACUUMREQUIRED
//
// P.Smith                      31/5/06
// call SetSpeedForSetPointChange(); when order is being changed
// call CalculateScrewSpeed();              // asm SSPEED
// #include "rwsei.h"
// call CopyHourlyData();           /* ASM = GENHRREP   */
// set g_bProgramLoopRunnning = TRUE;
// call  SetUpConversionFactors();
//
// P.Smith                      12/6/06
// call ForegroundSaveRecipe( );    // save recipe while running in foreground (1 byte per pass).
//
// P.Smith                      28/6/06
// NeedForTopUp(); called
// call unCalibrationCheckSum = CalculateCheckSum(g_arrcTemp, nSize);
// call CalcPerTarget();        // calculate % target for the 1st component.
// #include "FillRep.h"
// call CalculateDAPerKgCMR();                  /* asm CLDACPKGCMR  */
// call WaitForMaterialToBeVacuumed();
//
// P.Smith                      17/7/06
// check g_CalibrationData.m_bHighLoRegrindOption for transition from 1 to 0
// g_CalibrationData.m_nBatchesInKghAvg,g_CalibrationData.m_bLayering
//
// P.Smith                       7/11/06
// name change g_nCycleIndicate -> g_bCycleIndicate
// Remove reference to level sensor alarm.
// remove unused commented out code.
//
// P.Smith                       5/2/07
// remove all printfs, check for g_bRedirectStdioToPort2 bbefore printing.
// remove check for g_bSaveAllCalibrationToOnBoardEEprom
//
// P.Smith                       28/2/07
// Modify CheckTableForNewComponent to set RECIPECHANGEDBIT and only swt fbw if very small
// flow rate (ie flow rate equal to zero)
//
// P.Smith                       13/3/07
// if g_bUSDataAvailable, call ProcessUSData
//
// P.Smith                       13/4/07
// check for g_bUpdateSDCardSoftware and call CheckForSoftwareUpdate
// Correct eeprom update, this was inadvertently deleted when the printfs were
// removed.
//
// P.Smith                       3/5/07
// called CalculateSetGramsPerMeter
//
// P.Smith                       14/5/07
// m_nResetCPIRRCtr set when flow rate round robin needs to be reset.
//
// P.Smith                       30/5/07
// call CheckForMultiBlendRecipeSave
//
// P.Smith                       8/6/07
// call ResetRoundRobinFlowRateForAllComponents on recipe change
//
// P.Smith                       20/6/07
// added g_bSaveAllCalibrationToEEpromOutsideCycle check if TRUE and not running cycle set g_bSaveAllCalibrationToEEprom
//
// P.Smith                       25/6/07
// add other updates required for sd card.
//
// P.Smith                       26/6/07
// call ForegroundTenHzProgram
//
// P.Smith                       19/7/07
// if g_bInitiatePurge set, set to false and PurgeBlender
//
//
// P.Smith                       20/7/07
// set g_bBlenderPurged to false on recipe change, this will allow the purge to
// happen the next time.
// Set g_bBlenderPurgeingInProgress if purge requested, set to false if not.
// This stops the purge from coming in again.
//
// P.Smith                       2/8/07
// current extruder no now read from calibration, this allows the last extruder no
// to be displayed.
//
// M.McKiernan                      17/9/07
// Removed g_arrnMBTable[] & g_arrnMBTableFlags[], now in BatVars2
//
// P.Smith                       15/10/07
// SaveAllCalibrationDataToOnBoardEEprom called if g_bSaveAllCalibrationToOnBoardEEprom set
//
// P.Smith                       1/11/07
// if g_bRestartPanelNow is true call RestartPanel and set to false
//
// P.Smith                       9/11/07
// remove RestartPanel
//
// P.Smith                       13/11/07
// added check for MODE_GPSQMENTRY or MODE_GP12SQFEET call CalculateSetGramsPerMeterFromGramsPerSqMeter
// this also allows for multiplication factor for the grams per 12 square feet.
//
// P.Smith                       16/11/07
// added check for screw constant calculation, checks g_bCalculateScrewSpeedConstant
//
//
// P.Smith                       21/11/07
// added check for repeat optimisation
//
// P.Smith                       9/1/08
// added CheckForSoftwareUpdate, load of default cal data and reset of
// weight rr averaging if g_bResetComponentWeightsRoundRobin set
// call ResetAllWeightRoundRobin on order set point change
//
// P.Smith                                 10/1/08
// convertbatset modified to check for imperial modbus
// only allow conversion if imperial mode or metric mode
// No conversion if imperial modbus as the data is already
// in an imperial format.
// added WriteCalibrationDataToSDCard call to copy cal data to sd card
//
// P.Smith                          30/1/08
// correct compiler warnings
// call ResetSampleCounter on order change, this resets the batch statistical sample counter to 0
//
// P.Smith                          12/2/08
// added CheckCalibrationDataChecksum, this checks the calibration data every 10 seconds.
// if the ram checksum has changed, the calibration data is reloaded again.
// check for g_bSaveAllCalibrationToEEpromOutsideCycle in CheckCalibrationDataChecksum
// set g_bPITAllowed to TRUE
//
// P.Smith                          15/2/08
// if g_bReadSDCardDirectory is set, ReadSDCardDirectory
// PollUSBMB called from prog loop
//
// P.Smith                          18/2/08
// called PollForUSBCopy
//
// P.Smith                          25/2/08
// removed ReadSDCardDirectory  & CopyFileFromSdToUSB call for now
// only allow CalculateDToAPercentage to run if 0 - 10 v option.
//
// P.Smith                          1/3/08
// in CalculateDToAPercentage only calculate control % is mode
// is increase /decrease
//
// P.Smith                          6/3/08
// in CalculateDToAPercentage, use tacho to calculate the %
// % = tacho / 32768
//
// P.Smith                          18/3/08
// put CheckCalibrationDataChecksum back in to be called every second.
//
// P.Smith                          26/3/08
// g_bSaveAllProcessDataOutsideCycle checked for and g_bSaveAllProcessDataToEEprom set
// if true.
// if g_bSaveAllProcessDataToEEprom set SaveAllProcessDataToEEprom called
//
// P.Smith                          27/3/08
// only save process data if g_bAllowProcesDataSaveToEEPROM is true.
// set g_nSaveProcessDataToEEPROMCounter to  NOOFWRITESTOEEPROM to allow save of data to eeprom
// for n cycles
//
// P.Smith                          28/3/08
// use a difference name for config file cfaabbcc.ini, where a,b,c is the id
// set g_nSaveProcessDataToEEPROMCounter to NOOFWRITESTOEEPROM on order change
//
// P.Smith                          29/4/08
// name change g_CalibrationData.m_nCurrentExtruderNo to g_nCurrentExtruderNo
// remove save to eeprom.
//
// P.Smith                          4/6/08
// call LoadConfigFromSDCard rather than doing the functionality here.
// implement CopyConfigFileToBackup, LoadBackupConfigFromSDCard
//
// P.Smith                          19/6/08
// call AnalyseCommunications every 2 seconds
//
// P.Smith                          16/7/08
// SoftwareUpdateViaPanel called for software update from panel.
// add AddRecipeChangeToEventLog
// when calibraiton data is saved, set g_nWriteConfigToSDInNSeconds to allow the
// event log to be updated in a few seconds time.
// delay sd card writes by checking g_nSDCardDelayTime for 0
// check for g_nOperatorPanelPageId to add panel event to event log file.
//
// P.Smith                          23/7/08
// remove g_arrnMBTable and array comms extern
//
// P.Smith                          16/10/08
// --todo-- to lp
//
// P.Smith                          17/11/08
// remove ultrasonic stuff
//
// P.Smith                          26/11/08
// Service_Watchdog added but commented out
//
// P.Smith                          16/1/09
// copy g_nCyclePauseType to g_nCyclePauseTypeCopy
//
// P.Smith                          21/1/09
// set function codes for all functions.
// if g_bResetPanelSlaveComms set, call ResetPanelMBSlaveComms
// if g_bResetNetworkSlaveComms set , call ResetMBSlaveComms
// if g_bAddAlarmOccurredToEventLog set, call AddAlarmOccurredToEventLog
// if g_bAddAlarmClearedToEventLog, call AddAlarmClearedToEventLog
//
// P.Smith                          6/2/09
// when the set point is changed, set g_nHiLowOptionSwitchedOnOff to HILOTRANSITIONOFF_TO_ON to inidiate the regrind
// high low check.
// check for 0 - > 1 transition for regrind option.
// set g_nHiLowOptionSwitchedOnOffHistory directly, this will make the high low software work.
//
//
// P.Smith                          26/2/09
// added test to write to usb every 4 seconds
//
// P.Smith                          5/3/09
// added check for g_bDoSelfTestDiagnostics to initiate self test
// remove usb.h
//
// P.Smith                          25/5/09
// if g_bI2CError set, then add ic2 event to log
//
// P.Smith                          16/6/09
// allow save of vacuum loader calibration data.
// if g_bSaveLoaderCalibrationData is set, call SaveLoaderDataToEEprom
//
// P.Smith                          25/6/09
// call UpdateLoaderDataFromVAC8IO & LoadVacuumLoaderCalibrationParametersFromMB
// check for g_bSaveLoaderCalibrationData, call save of calibration data and
// generation of hash
//
// P.Smith                          30/6/09
// use g_wForeGroundCounter to show the number of passes in the foreground
// use _wProgramLoopEntry show that progloop is entered and exited.
// set g_nWriteConfigToSDInNSeconds to save loader config data
// if g_bPrintModbusMessageToTelnet set, print modbus print buffer
//
// P.Smith                          22/7/09
// added GenerateHashForPulsing, check for g_bICSRecipeDataWritten and
// g_bSaveAdditionalDataToEEPROM
//
// P.Smith                          1/9/09
// call AddModbusWriteEventToLog if g_bAddEventToEventLog set and g_nHoldOffEventLogWriteCtr
// is zero.
//
// P.Smith                          7/9/09
// if g_bDoAToDTest is set do AtoDInternalTest and add event to log if the test
// has passed.
//
// P.Smith                          15/9/09
// call RecalibratePulsingForComponents when order is changed
//
// P.Smith                          18/9/09
// remove a/d test
//
// P.Smith                          29/9/09
// reconfigure modbus if the no of loaders has changed.
//
// P.Smith                          19/11/09
// call ConfigureModbusMaster if vac 8 expansion enable / disable has changed
// if g_bDecideOnRefStability set, call DecideOnRefAToDStabilityTest
// if g_bDecideOnLoadCellStability set, call DecideOnLoadCellAToDStabilityTest
//
// P.Smith                          23/11/09
// remove CheckTableForNewComponent as this is causing the blender to put the component
// into fill by weight if it is being added in new.
//
// P.Smith                          11/1/10
// if g_bFileCouldNotBeOpenedEvent set then add event to log.
//
// P.Smith                          20/1/10
// added CheckForTelnetCommands to allow the telnet commands to be run in the
// foreground.
//
// M.McKiernan						8/02/2010
// HandleEIPControlCommands(); called in ProgLoop().
// Section added to handle setpoint download from Ethernet/IP, see //EIP start - //EIP finish
// else if(g_nTransferSetpointsFlag == TRANSFERSPFROM_EIP)
//
// P.Smith                          23/3/10
// call CompareSHAHashes & CheckTimeHash to compare has and to check
// the time hash.
//
// P.Smith                          6/4/10
// call SaveFuntionNosInRAM to save function running in non volatile ram
//
// P.Smith                          7/7/10
// removed monitorbatch and cyclemonitor
//
// M.McKiernan						27/4/2020
// Added     if( g_arrnMBTable[MODBUS_FORCE_RESET_COMMAND] == FORCE_RESET_VALUE )
//...............ForceReboot();		//RESET
//
//     if( g_arrnMBTable[MODBUS_FORCE_RESET_COMMAND] == FORCE_FAST_INFLATE )
// WidthManualFastIncrease();		//ACU - Fast Increase for Manual mode only.
// if( g_arrnMBTable[MODBUS_FORCE_RESET_COMMAND] == FORCE_USMUX_A0_LOW )  g_bTestUSMuxA0Low = TRUE;
// else if( g_arrnMBTable[MODBUS_FORCE_RESET_COMMAND] == FORCE_USMUX_A0_HIGH ) g_bTestUSMuxA0High = TRUE;
// if( g_arrnMBTable[MODBUS_FORCE_RESET_COMMAND] == FORCE_USMUX_CLK_LOW )  g_bTestUSMuxClockLow = TRUE;
//    else if( g_arrnMBTable[MODBUS_FORCE_RESET_COMMAND] == FORCE_USMUX_CLK_HIGH ) g_bTestUSMuxClockLow = FALSE; g_bTestUSMuxClockHigh = TRUE;
//   if( g_arrnMBTable[MODBUS_FORCE_RESET_COMMAND] == CLR_USMUX_TESTS )
//    {
//    	g_bTestUSMuxClockLow = FALSE;
//    	g_bTestUSMuxClockHigh = FALSE;
//    	g_bTestUSMuxA0Low = FALSE;
//    	g_bTestUSMuxA0High = FALSE;
////////////////////////////////////////////////////////////

//*****************************************************************************
// INCLUDES
//*****************************************************************************
#include <stdio.h>

#include "SerialStuff.h"

#include "Foreground.h"
#include "16R40C.h"
#include "TimeDate.h"
//bb #include "GenerateRollReport.h"
#//bb include "GenerateOrderReport.h"
//bb #include "SequencerAlgorithm.h"
#include "PrinterFunctions.h"
//bb #include "GenerateRollReport.h"
//bb #include "GenerateOrderReport.h"
//bb #include "VAC350MBProgs.h"
//nbb #include "Watchdog.h"
#include "InitialiseHardware.h"
#include "ConfigurationFunctions.h"
#include "MBProgs.h"
#include "CycleMonitor.h"
#include "MonBat.h"
#include "BatVars.h"
#include        "SequenceTable.h"
#include "SetpointFormat.h"
#include "ProgLoop.h"
#include "BatchCalibrationFunctions.h"
#include "FillRep.h"
#include "BBCalc.h"
#include "MBProSum.h"
#include "FGOneHz.h"
#include "Conalg.h"
#include "Batvars2.h"
#include "TSMPeripheralsMBIndices.h"
#include "Lspeed.h"
#include "ConversionFactors.h"
#include "BlRep.h"
#include "Pause.h"
#include "BatchMBIndices.h"
#include "Vaccyc.h"
#include "Polllls.h"
#include "rwsei.h"
#include "Alarms.h"
#include "CsumCalc.h"
#include "String.h"
#include "MBMaster.h"
#include <basictypes.h>
#include "KghCalc.h"
#include "Print.h"
#include "Mtttymux.h"
#include "UltrasonicRead.h"
#include "ProcessUSData.h"
#include "SoftwareUpdate.h"
#include "Gpmcalc.h"
#include "StoreCommsRecipe.h"
#include "CPIRRobin.h"
#include "FgTenHz.h"
#include "Purge.h"
#include "Mtttymux.h"
#include "Screw.h"
#include "Optimisation.h"
#include "Weightrr.h"
#include "Loadcaldatafromsdcard.h"
#include "SoftwareUpdate.h"
#include "CopyCalibrationDataToIniFile.h"
#include "BatchStatistics.h"
//#include "USB.h"
#include "PrntDiag.h"
#include "sdcard.h"
#include "Eventlog.h"
#include "SdCardMaintenance.h"
#include "Watchdog.h"
#include "MBPanelSNBBhand.h"
#include "MBSHand.h"
#include "Vacldee.h"
#include "VacVars.h"
#include "VacSeq.h"
#include "Mbvacldr.h"
#include "Hash.h"
#include "PulseValve.h"
#include "BlenderSelfTest.h"
#include    "Telnetcommands.h"
#include "EIPProgs.h"
#include "security.h"
#include <Bsp.h>
#include "WidthControlAlgorithm.h"
// Locally declared global variables
extern  structSetpointData      g_TempRecipe,g_CurrentRecipe,g_CommsRecipe;
extern  structSetpointData 		g_EIPRecipe;


// Externally defined global variables
extern  unsigned char g_cBlenderNetworkAddress;
extern  unsigned char g_cAutoCycleSymbol;
extern  CalDataStruct   g_CalibrationData;
extern  StructAlarmTable    g_MainAlarmTable;
extern  char        g_cMB1BreakFlag;
extern  BYTE DS2432Buffer[];

extern  structCPIRRData     g_WorkCPIRRData;
extern int g_nFlashRate;		// LED flash rate period in PIT ticks.
extern   structVacCalib   g_VacCalibrationData;
extern bool g_bTestUSMuxA0Low;
extern bool g_bTestUSMuxA0High;
extern bool g_bTestUSMuxClockLow;
extern bool g_bTestUSMuxClockHigh;

unsigned int g_nTwoSecondCounter = 0;

//////////////////////////////////////////////////////
// ProgramLoop( VOID )                  from ASM = PGLOOP (full program loop in SBBGND18)
//
//
// M.McKiernan                                                  11-08-2004
// First pass.
//////////////////////////////////////////////////////
/*  following for diagnostics only
int g_nTestPcts[8];     // --testonly--
int g_nTestkgh;
int g_nTestgm;
int g_nTestgm2;
int g_nTestls;
int g_nTestdw;
int g_nTesttw;

*/

void ProgramLoop( void )
{
    int     i;
    unsigned int     nRegCompIndex;
    g_bPITAllowed = TRUE;
    g_bProgramLoopRunnning = TRUE;
    g_wForeGroundCounter++;
    g_wProgramLoopEntry = 1;
//    PollForUSBCopy();
//    PollUSBMB();
    //Service_Watchdog();  //nbb--todo-- put back in
    HandleEIPControlCommands();
    CheckForMultiBlendRecipeSave();
    if( g_nSaveRecipeSequence )
    {
        g_nFunctionRunning = FUNCTION_1;
        SaveFuntionNosInRAM();
        ForegroundSaveRecipe( );    // save recipe while running in foreground (1 byte per pass).
    }


    if(g_bSaveAllCalibrationToEEpromOutsideCycle && !g_bCycleIndicate)
    {
        g_bSaveAllCalibrationToEEpromOutsideCycle = FALSE;
        g_bSaveAllCalibrationToEEprom = TRUE;
    }
    if(g_bSaveAllProcessDataOutsideCycle && !g_bCycleIndicate)
    {
        g_bSaveAllProcessDataOutsideCycle = FALSE;
        g_bSaveAllProcessDataToEEprom = TRUE;
    }

    if( g_arrnMBTable[MODBUS_FORCE_RESET_COMMAND] == FORCE_RESET_VALUE )
    {
    	g_arrnMBTable[MODBUS_FORCE_RESET_COMMAND] = 123;
//    	OSTimeDly(TICKS_PER_SECOND*3);

    	ForceReboot();		//RESET
    }
    if( g_arrnMBTable[MODBUS_FORCE_RESET_COMMAND] == FORCE_FAST_INFLATE )
    {
    	g_arrnMBTable[MODBUS_FORCE_RESET_COMMAND] = 123;
//    	OSTimeDly(TICKS_PER_SECOND*3);

    	WidthManualFastIncrease();		//ACU - Fast Increase for Manual mode only.
    }


    if( g_arrnMBTable[MODBUS_FORCE_RESET_COMMAND] == FORCE_USMUX_A0_LOW )
    {
    	g_bTestUSMuxA0Low = TRUE;
    	g_bTestUSMuxA0High = FALSE;
    }
    else if( g_arrnMBTable[MODBUS_FORCE_RESET_COMMAND] == FORCE_USMUX_A0_HIGH )
    {
    	g_bTestUSMuxA0Low = FALSE;
    	g_bTestUSMuxA0High = TRUE;
    }

    if( g_arrnMBTable[MODBUS_FORCE_RESET_COMMAND] == FORCE_USMUX_CLK_LOW )
    {
    	g_bTestUSMuxClockLow = TRUE;
    	g_bTestUSMuxClockHigh = FALSE;
    }
    else if( g_arrnMBTable[MODBUS_FORCE_RESET_COMMAND] == FORCE_USMUX_CLK_HIGH )
    {
    	g_bTestUSMuxClockLow = FALSE;
    	g_bTestUSMuxClockHigh = TRUE;
    }
    if( g_arrnMBTable[MODBUS_FORCE_RESET_COMMAND] == CLR_USMUX_TESTS )
    {
    	g_bTestUSMuxClockLow = FALSE;
    	g_bTestUSMuxClockHigh = FALSE;
    	g_bTestUSMuxA0Low = FALSE;
    	g_bTestUSMuxA0High = FALSE;
    }


    if( g_bSaveAllCalibrationToEEprom )
    {
        g_nFunctionRunning = FUNCTION_2;
        SaveFuntionNosInRAM();
        GenerateHashForPulsing();
        SaveAllCalibrationDataToEEprom( );
        g_bSaveAllCalibrationToEEprom = FALSE;  // clear flag.
        g_nWriteConfigToSDInNSeconds = NOOFSECONDSTOWRITETOSDCARD;

    }

    if(g_bSaveAllCalibrationToOnBoardEEprom)
    {
        g_bSaveAllCalibrationToOnBoardEEprom = FALSE;
        g_nFunctionRunning = FUNCTION_3;
        SaveFuntionNosInRAM();
        SaveAllCalibrationDataToOnBoardEEprom();
    }

    if(g_bSaveAllProcessDataToEEprom)
    {
        g_bSaveAllProcessDataToEEprom = FALSE;
        if(g_bAllowProcesDataSaveToEEPROM)
         {
            g_nFunctionRunning = FUNCTION_4;
            SaveFuntionNosInRAM();
            SaveAllProcessDataToEEprom();
         }
    }

   if(g_VacCalibrationData.m_nLoaders > 0 )
   {
      UpdateLoaderDataFromVAC8IO();    // update loader data
   }
    if(g_bVacuumLoaderCalibrationWritten)
    {
        g_bVacuumLoaderCalibrationWritten = FALSE;
        LoadVacuumLoaderCalibrationParametersFromMB();
    }
    if(g_bICSRecipeDataWritten)
    {
        g_bICSRecipeDataWritten = FALSE;
//nbb--todo-- put back    LoadICSRecipeMBData();
    }
    if(g_bSaveAdditionalDataToEEPROM)
    {
        g_bSaveAdditionalDataToEEPROM = FALSE;
//nbb--todo--put back        SaveAdditionalDataToEEPROM();
    }


    if(g_bSaveLoaderCalibrationData)
    {
        g_bSaveLoaderCalibrationData = FALSE;
        g_nFunctionRunning = FUNCTION_5;
        SaveFuntionNosInRAM();
        GenerateHashForLoader();
        SaveLoaderDataToEEprom();
        g_nWriteConfigToSDInNSeconds = NOOFSECONDSTOWRITETOSDCARD;

    }



    //check for 1 hz roll-over.
    if( g_bForegroundSecondRollover )   // all programs that need to be called once per second in foreground should be added here
    {
    	CompareSHAHashes(); //security--tsm--
        g_nFunctionRunning = FUNCTION_5;
        SaveFuntionNosInRAM();

        g_bForegroundSecondRollover = FALSE;    // Rollover flag is set by PIT when RTC second rolls over.
        g_nFunctionSubSectionRunning = FUNCTION_SUB_SECTION_1;
        SaveFuntionNosInRAM();
        ForegroundOneHzProgram();           // Foreground stuff that needs doing once per second.
        g_nFunctionSubSectionRunning = FUNCTION_SUB_SECTION_2;
        SaveFuntionNosInRAM();
        CheckCalibrationDataChecksum();    //nbb--todo-- is this okay here
        g_nFunctionSubSectionRunning = FUNCTION_SUB_SECTION_3;
        SaveFuntionNosInRAM();
        SetUpConversionFactors();         //nbb--testonly--
        CheckTimeHash();//security--tsm--
//      check for To Auto Event
        if((g_bToAutoEvent)&& (g_nSDCardDelayTime == 0))
        {
            g_nFunctionRunning = FUNCTION_6;
            SaveFuntionNosInRAM();
            g_bToAutoEvent = FALSE;
            AddBlenderToAutoEventToLog();
        }

//      check for To Manual Event
        if((g_bToManualEvent)&& (g_nSDCardDelayTime == 0))
        {
            g_bToManualEvent = FALSE;
            g_nFunctionRunning = FUNCTION_7;
            SaveFuntionNosInRAM();
            AddBlenderToManualEventToLog();
        }
//      check for Weigh hopper calibration Event
            if((g_bWeighHopperCalibratedEvent) && (g_nSDCardDelayTime == 0))
            {
              g_nFunctionRunning = FUNCTION_8;
              SaveFuntionNosInRAM();
              g_bWeighHopperCalibratedEvent = FALSE;
              AddWeightHopperCalibratedToEventLog();
            }
//      check for Weigh hopper tared Event

            if(g_bWeighHopperTaredEvent && (g_nSDCardDelayTime == 0))
            {
              g_nFunctionRunning = FUNCTION_9;
              SaveFuntionNosInRAM();
              g_bWeighHopperTaredEvent = FALSE;
              AddWeightHopperTareToEventLog();
            }
//      check for cleaning Event
            if((g_bCleaningInitiatedEvent)&& (g_nSDCardDelayTime == 0))
            {
                g_nFunctionRunning = FUNCTION_10;
                SaveFuntionNosInRAM();
                g_bCleaningInitiatedEvent = FALSE;
                AddSDCardCleainingCycleInitiatedToEventLog();
            }
        // trigger write to sd card
        if(g_nWriteConfigToSDInNSeconds != 0)
        {
            g_nWriteConfigToSDInNSeconds--;
            if(g_nWriteConfigToSDInNSeconds == 0)
            {
                g_bCopyConfigFileToSDCard = TRUE;    //initiate copy to sd card
            }
        }
        if((g_nOperatorPanelPageId != 0) && (g_nSDCardDelayTime == 0))
        {
            g_nFunctionRunning = FUNCTION_11;
		    SaveFuntionNosInRAM();
            AddOperatorPanelPageToEventLog(g_nOperatorPanelPageId);
            g_nOperatorPanelPageId = 0;
        }




      }

//check for 1 hz  - (PIT 10 Hz)
    if( g_bFGSecondFlag2 )  // all programs that need to be called once per second in foreground should be added here
    {
        g_nFunctionRunning = FUNCTION_12;
		SaveFuntionNosInRAM();
        g_bFGSecondFlag2 = FALSE;       // Rollover flag is set by PIT when second rolls over.
        ForegroundOneHzProgram2();      // this is stuff that originally was on the 10Hz part of PIT when reached 1 second.
    }

//check for 1 minute  - (orignially in PIT 10 Hz)
    if( g_bFGOneMinuteFlag )    // all programs that need to be called once per minute in foreground
    {
        g_nFunctionRunning = FUNCTION_13;
	    SaveFuntionNosInRAM();
        g_bFGOneMinuteFlag = FALSE; // flag is set by PIT when RTC minute rolls over.
        ForegroundOneMinuteProgram();           // this is stuff that originally was on the 10Hz part of PIT when reached 1 minute
    }


    //check for 2 hz roll-over.
    if(g_bTwoSecondFlag)   // all programs that need to be called once per second in foreground should be added here
    {
        g_nFunctionRunning = FUNCTION_14;
        g_bTwoSecondFlag = FALSE;           // asm  TWOSFLG     .
        g_nFunctionRunning = FUNCTION_SUB_SECTION_1;
        SaveFuntionNosInRAM();
		CheckLowLevelSensorAlarms();        //
        g_nFunctionRunning = FUNCTION_SUB_SECTION_2;
        SaveFuntionNosInRAM();
		CalculateScrewSpeed();              // asm SSPEED
        g_nFunctionRunning = FUNCTION_SUB_SECTION_3;
		SaveFuntionNosInRAM();
        AnalyseCommunications();
//        g_nTwoSecondCounter++;
//        if(g_nTwoSecondCounter >= 2)
//        {
//            g_nTwoSecondCounter = 0;
//            PrintCycleDiagnostics();    //  BBDiaPrn();      //nbb--todo--put back
//            g_nCycleCounter++;           // nbb--todo--put back
//        }
    }

    if(g_bUpdateSDCardSoftware && (g_nSDCardDelayTime == 0))
    {
        g_nFunctionRunning = FUNCTION_15;
	    SaveFuntionNosInRAM();

        g_bUpdateSDCardSoftware = FALSE;
        SoftwareUpdateViaPanel();  // update from panel swupdate file.
    }

   if(g_bCopyConfigFileToSDCard && (g_nSDCardDelayTime == 0))
    {
        g_nFunctionRunning = FUNCTION_16;
	    SaveFuntionNosInRAM();
        g_bCopyConfigFileToSDCard = FALSE;
        WriteCalibrationDataToSDCard();
    }

  if(g_bCopyConfigFileToBackupFile && (g_nSDCardDelayTime == 0))
    {
        g_nFunctionRunning = FUNCTION_17;
	    SaveFuntionNosInRAM();
        g_bCopyConfigFileToBackupFile = FALSE;
        CopyConfigFileToBackup();
    }
   if(g_bLoadConfigFromSDCard && (g_nSDCardDelayTime == 0))
   {
        g_nFunctionRunning = FUNCTION_18;
	    SaveFuntionNosInRAM();
        g_bLoadConfigFromSDCard = FALSE;
        LoadConfigFromSDCard();
   }
   if(g_bLoadBackupConfigFromSDCard && (g_nSDCardDelayTime == 0))
   {
        g_nFunctionRunning = FUNCTION_19;
	    SaveFuntionNosInRAM();
        g_bLoadBackupConfigFromSDCard = FALSE;
        LoadBackupConfigFromSDCard();
   }


/*
CHKFT   TST     CYCLEINDICATE   ; IN THE MIDDLE OF A CYCLE
        BNE     CONFOR1         ; YES
        TST     FILLTARGETENTRYF
        BEQ     CONFOR1
        CLR     FILLTARGETENTRYF
        JSR     NEWTARWEIGHTENTRY

CONFOR1:
*/
//CONSPP
 if(!g_bCycleIndicate)  //if not cycling
 {
        if(g_bResetComponentWeightsRoundRobin)
        {
            g_nFunctionRunning = FUNCTION_20;
            SaveFuntionNosInRAM();
            g_bResetComponentWeightsRoundRobin = FALSE;
            ResetAllWeightRoundRobin();           // JSR     RESETALLWGTRROBIN

        }

    //asm = CHKFT - CONFOR1
        if(g_bFillTargetEntryFlag)  // new target weight entered??
        {
            g_nFunctionRunning = FUNCTION_21;
            SaveFuntionNosInRAM();
            g_bFillTargetEntryFlag = FALSE;
            NewTargetWeightEntry();     // routine calculates new targets for comp. #1
        }

        if(g_bResetStageSeq)
        {
                g_bResetStageSeq = FALSE;

                g_CalibrationData.m_nStageFillEn = g_nStageFillEnTemp;      // Set Stage fill
                g_CalibrationData.m_bLayering = g_bLayeringTemp; // Set layering

                for(i= 0; i < MAX_COMPONENTS; i++)
                {       // zero the stage fill sequencing indication
                                g_nStageSeq[i] = 0;
                }
                g_bSaveAllCalibrationToEEprom = TRUE;       /* set flag to cause all calibration data to be written to EEPROM.*/
                g_nFunctionRunning = FUNCTION_22;
                SaveFuntionNosInRAM();
                CopyCalibrationDataToMB();      /* copy calibration data to MB. COPYCDMB  */


        }

        // component % changed
        // ASM = CHKPER
        if(g_bPctFlag || g_bCSAFlag)
        {
                if(g_bPctFlag)
                {
                    g_bPctFlag = FALSE;
                }
                if(g_bCSAFlag)
                {
                    g_bCSAFlag = FALSE;
                g_nFunctionRunning = FUNCTION_23;
                SaveFuntionNosInRAM();
                ConvertBatchSetpointstoImperial(); // convert set points to imperial asm = CONVERTBATSET

                }
                     if(g_CurrentRecipe.m_nRegrindComponent == 0)   // is there a regrind component??
                        g_bRegrindPresent = FALSE;
                     else
                     {
                        g_bRegrindPresent = TRUE;
                        // save-original
                        // save the regrind % so that it can be reloaded after use of "High Regrind" %.

                        nRegCompIndex = g_CurrentRecipe.m_nRegrindComponent - 1;
//                        iprintf("\n regrind index is %d",nRegCompIndex); //nbb--testonly--
                        g_CurrentRecipe.m_fPercentageRegrindOriginal = g_CurrentRecipe.m_fPercentage[nRegCompIndex];
                     }

                // copy setpoints into Modbus
                g_nFunctionRunning = FUNCTION_24;
                SaveFuntionNosInRAM();
                CopySetpointsToMB();
//CONFOR11
                if(g_bFirstWCycle)  //first weigh cycle??
                {
                      g_nFunctionRunning = FUNCTION_25;
					  SaveFuntionNosInRAM();
                      FormSeqTable(); //Form Sequence table.
                      CalculateSetGramsPerMeter();   // asm CALCSETGPM         // g/m calculation from Microns
                     ClearUnused();          // Clear data for unused components.
                     NeedForTopUp(); //
                }
                else  // not first weigh cycle.
                {  // ASM = CURRTOPREV
                        g_nFunctionRunning = FUNCTION_26;
					    SaveFuntionNosInRAM();
                        CopySeqTableToPrevious();               // SeqTable -> Prev. seq. table.
                        FormSeqTable(); //Form Sequence table.
                        CalculateSetGramsPerMeter();   // asm CALCSETGPM         // g/m calculation from Microns
                         ClearUnused();          // Clear data for unused components.
//nbb--todo--review               CheckTableForNewComponent();    //
                }

                g_nFunctionRunning = FUNCTION_27;
			    SaveFuntionNosInRAM();
                Comp1stxTotWgt();               //

        }  // end for %'s changed section.
 }
// CONFOR2

        g_nFunctionRunning = FUNCTION_28;
 //nbb--width--todo--put back MonitorBatch( );                                                                // batch monitor. (MONBAT)
        g_nFunctionRunning = FUNCTION_29;
//nbb--width--todo--put back         CycleMonitor( );                                                                // batch cycling overview program.
        g_nFunctionRunning = FUNCTION_30;
        MonitorVacuumSequence();   // ASM VACMON


// --REVIEW--
        // --REVIEW--
                if(!g_bCycleIndicate || g_bStopped)     // not cycling or stopped
                {
                        //CHKTRF (in assembler)
                        g_nFunctionRunning = FUNCTION_31;
                        if(g_nTransferSetpointsFlag == TRANSFERSPID)
                        {
                                g_nTransferSetpointsFlag = 0;   // clear flag

                                // copy loaded recipe into current recipe area.
                                // TRANSFERPRODSETPOINTS
                                memcpy(&g_CurrentRecipe, &g_TempRecipe, sizeof( g_TempRecipe ) );
                                if( (g_CalibrationData.m_nBlenderMode == MODE_GPSQMENTRY) || (g_CalibrationData.m_nBlenderMode == MODE_GP12SQFEET))
                                {
                                    CalculateSetGramsPerMeterFromGramsPerSqMeter();
                                }
                                g_bPctFlag = TRUE; // --REVIEW--
                                SetSpeedForSetPointChange();                   // ASM = SPCHANG
                                SetExtruderControlStartup(); /* asm = SETSTART */
                                ConvertBatchSetpointstoImperial(); // convert set points to imperial asm = CONVERTBATSET
                                g_nCurrentExtruderNo = g_CurrentRecipe.m_nExtruder_No; // indicate current extruder no.
                                g_nSaveProcessDataToEEPROMCounter = NOOFWRITESTOEEPROM;
                                ResetRoundRobinFlowRateForAllComponents();
                                ResetAllWeightRoundRobin();
                                ResetSampleCounter();
                                if(g_bBlenderPurged)
                                {
                                    g_bBlenderPurged = FALSE;
                                    g_bBlenderPurgeingInProgress = TRUE;  // this stops purging from happening again
                                }
                                else
                                {
                                    g_bBlenderPurgeingInProgress = FALSE;
                                }
                                if(g_CalibrationData.m_bHighLoRegrindOption)
                                {
                                    g_nHiLowOptionSwitchedOnOffHistory = HILOTRANSITIONOFF_TO_ON;          //2
                                }

                        }
                        else if(g_nTransferSetpointsFlag == TRANSFERSPFROMCOMMSID)
                        {
                                g_nTransferSetpointsFlag = 0;           //Clear flag.
                                TransferSetPercentagesFromComms();
                                if( (g_CalibrationData.m_nBlenderMode == MODE_GPSQMENTRY) || (g_CalibrationData.m_nBlenderMode == MODE_GP12SQFEET))
                                {
                                    CalculateSetGramsPerMeterFromGramsPerSqMeter();
                                }
                               SetSpeedForSetPointChange();                   // ASM = SPCHANG
                                SetExtruderControlStartup(); /* asm = SETSTART */
                                ConvertBatchSetpointstoImperial(); // convert set points to imperial asm = CONVERTBATSET
                                g_nCurrentExtruderNo = g_CurrentRecipe.m_nExtruder_No; // indicate current extruder no.
                                g_nSaveProcessDataToEEPROMCounter = NOOFWRITESTOEEPROM;
                                ResetRoundRobinFlowRateForAllComponents();
                                ResetAllWeightRoundRobin();
                                ResetSampleCounter();

                                if(g_bBlenderPurged)
                                {
                                    g_bBlenderPurged = FALSE;
                                    g_bBlenderPurgeingInProgress = TRUE;  // this stops purging from happening again
                                }
                                else
                                {
                                    g_bBlenderPurgeingInProgress = FALSE;
                                }
        					AddRecipeChangeToEventLog();
        					if(g_CalibrationData.m_bHighLoRegrindOption)
        					{
        						g_nHiLowOptionSwitchedOnOffHistory = HILOTRANSITIONOFF_TO_ON;          //2
        					}

                        }
                        else if(g_nTransferSetpointsFlag == TRANSFERSPFROM_EIP)
                         {		//EIP start
                                 g_nTransferSetpointsFlag = 0;           //Clear flag.

                                 //TransferSetPercentagesFromComms();
                                 memcpy(&g_CurrentRecipe, &g_EIPRecipe, sizeof( g_EIPRecipe ) );
                                 g_bPctFlag = TRUE; // set this flag directly
                                 ResetAllFillAlarms();  // asm RESETALLFILLALARMS
                                 g_nAllowLargeAlarmBand = ALLOWLARGEBANDINCYCLES;
        //                         if(g_arrnMBTable[BATCH_SETPOINT_RESET_TOTALS] & MB_START_CLEAN_BIT)
                                 if( g_EIPRecipe.m_nResetTotalsFlag & MB_START_CLEAN_BIT )
                                 {
                                      InitiateCleanCycle();   // ASM = INITIATECLEANCYCLE
                                 }

                                 if( (g_CalibrationData.m_nBlenderMode == MODE_GPSQMENTRY) || (g_CalibrationData.m_nBlenderMode == MODE_GP12SQFEET))
                                 {
                                     CalculateSetGramsPerMeterFromGramsPerSqMeter();
                                 }
                                SetSpeedForSetPointChange();                   // ASM = SPCHANG
                                 SetExtruderControlStartup(); /* asm = SETSTART */
                                 ConvertBatchSetpointstoImperial(); // convert set points to imperial asm = CONVERTBATSET
                                 g_nCurrentExtruderNo = g_CurrentRecipe.m_nExtruder_No; // indicate current extruder no.
                                 g_nSaveProcessDataToEEPROMCounter = NOOFWRITESTOEEPROM;
                                 ResetRoundRobinFlowRateForAllComponents();
                                 ResetAllWeightRoundRobin();
                                 ResetSampleCounter();

                                 if(g_bBlenderPurged)
                                 {
                                     g_bBlenderPurged = FALSE;
                                     g_bBlenderPurgeingInProgress = TRUE;  // this stops purging from happening again
                                 }
                                 else
                                 {
                                     g_bBlenderPurgeingInProgress = FALSE;
                                 }
                                 AddRecipeChangeToEventLog();
                                 if(g_CalibrationData.m_bHighLoRegrindOption)
                                 {
                                	 g_nHiLowOptionSwitchedOnOffHistory = HILOTRANSITIONOFF_TO_ON;          //2
                                 }

                         } //EIP finish.


                }

//      ASM = NODSUP - CON24


    if  (g_bStageFillCalc)
        {
                g_bStageFillCalc = FALSE;
                CalcPerTarget();        // calculate % target for the 1st component.

        }

    if( g_bDoDiagnosticsFlag == TRUE )      //  flag set in TenHzProgram.
    {
        g_nFunctionRunning = FUNCTION_32;
        g_bDoDiagnosticsFlag = FALSE;
        PollForOuputDiagnostics();          // DODIAG - run diagnostics program switching on/off outputs.
    }

    if(g_bRunEst)      //  estimation required ?
    {
        g_nFunctionRunning = FUNCTION_33;
        g_bRunEst = FALSE;
        EstimateKgPerHour();
    }



    if(g_bRunEst)      //  estimation required ?
    {
        g_nFunctionRunning = FUNCTION_34;
        g_bRunEst = FALSE;
        EstimateKgPerHour();
    }

    if(g_bCCVsFlag)
    {
        g_nFunctionRunning = FUNCTION_35;
        g_bCCVsFlag = FALSE;    //CCVSFLG        // control voltage calculation flag reset
        CalculateDToAPercentage();

    }


    if(g_bLineSpeedUpdate)
    {
        g_nFunctionRunning = FUNCTION_36;
        g_bLineSpeedUpdate = FALSE;             //
        LineSpeedCalculation();           // call line speed calculation
    }
        g_nFunctionRunning = FUNCTION_37;
        AccumulateSEIPulses();              // accumulate length pulses
        g_nFunctionRunning = FUNCTION_38;
        ReactToChangeInLineSpeed();         // check if change in line speed has been signalled

// check for front roll change activation flag

    if(g_bActivateFrontRollChange)
    {
        g_nFunctionRunning = FUNCTION_39;
        g_bActivateFrontRollChange = FALSE;  // reset reel change flag.
        CopyFrontRollChangeData(); // ASM = FRELCHG
    }


    if(g_bActivateBackRollChange)
    {
        g_nFunctionRunning = FUNCTION_40;
        g_bActivateBackRollChange = FALSE;  // reset reel change flag.
        CopyBackRollChangeData(); // ASM = BRELCHG
    }

    if(g_bTenSecondFlag) // 10 second flag set ?
    {
        g_nFunctionRunning = FUNCTION_41;
        g_bTenSecondFlag = 0; //ASM = TSCFLG
        AccumulateFrontLength();      // ASM = ACCLT_F
        AccumulateBackLength();       // ASM = ACCLT_B
        CheckIfVacuumRequired();      // ASM = CHECKIFVACUUMREQUIRED     ; CHECK IF VACUUMING IS REQUIRED ON STARTUP
    }

    if(g_bActivateHourlyReport)
    {
        g_nFunctionRunning = FUNCTION_42;
        g_bActivateHourlyReport = FALSE;
        CopyHourlyData();           /* ASM = GENHRREP   */
    }
    /* asm CON17 */
    if(g_bDoCMRDToAPerKgCalculation)
    {
        g_nFunctionRunning = FUNCTION_43;
        g_bDoCMRDToAPerKgCalculation = FALSE;   /* asm CMRNOW  */
        CalculateDAPerKgCMR();                  /* asm CLDACPKGCMR  */
    }

        g_nFunctionRunning = FUNCTION_44;
        CheckForBinPresence();
    g_nFunctionRunning = FUNCTION_45;
    WaitForMaterialToBeVacuumed();


    if(g_bForegroundTenHzFlag)
    {
        g_bForegroundTenHzFlag = FALSE;
        g_nFunctionRunning = FUNCTION_46;
        ForegroundTenHzProgram();

    }

    g_nFunctionRunning = FUNCTION_47;
    CheckForPrintedReport();            //       JSR    CHKPRFLG

    if(g_CalibrationData.m_nPeripheralCardsPresent != g_nPeripheralCardsPresentHistory) // check for change in peripherals.
    {
        g_nFunctionRunning = FUNCTION_48;
        g_nPeripheralCardsPresentHistory = g_CalibrationData.m_nPeripheralCardsPresent; /* different */
        ConfigureModbusMaster();            /* rerun config again. */
    }

    if(g_VacCalibrationData.m_nLoaders != g_nLoadersHistory) // check for change in peripherals.
    {
        g_nFunctionRunning = FUNCTION_48;
        g_nLoadersHistory = g_VacCalibrationData.m_nLoaders;
        ConfigureModbusMaster();
    }
    if(g_CalibrationData.m_bVac8Expansion != g_nVac8ExpansionHistory) // check for change in peripherals.
    {
        g_nFunctionRunning = FUNCTION_48;
        g_nVac8ExpansionHistory = g_CalibrationData.m_bVac8Expansion;
        ConfigureModbusMaster();
    }


    if(g_CalibrationData.m_nStageFillEn != g_nStageFillEnableTransitionHistory)
    {
        if(g_CalibrationData.m_nStageFillEn && !g_nStageFillEnableTransitionHistory) // check for change .
        {
            g_nFunctionRunning = FUNCTION_49;
            SetCopyFlowRatesCommand();
        }
        g_nStageFillEnableTransitionHistory = g_CalibrationData.m_nStageFillEn;
    }
    if(g_CalibrationData.m_nBatchesInKghAvg != g_nBatchesInKghAvgHistory)
    {
         g_nFunctionRunning = FUNCTION_50;
         ResetKgHRoundRobin();
    }
    g_nBatchesInKghAvgHistory = g_CalibrationData.m_nBatchesInKghAvg;

    if(g_CalibrationData.m_bLayering)
    {
        if(!g_bLayeringHistory)
        {
             g_nFunctionRunning = FUNCTION_51;
             SetCopyFlowRatesCommand();     //
        }
    }
    g_bLayeringHistory = g_CalibrationData.m_bLayering;


    if(!g_CalibrationData.m_bHighLoRegrindOption)     //SBB--todo--
    {
        if(g_bHighLoRegrindOptionHistory)  // transition 1->0
        {
           g_nHiLowOptionSwitchedOnOffHistory = HILOTRANSITIONON_TO_OFF;   //
        }
    }
    else
    {
        if(!g_bHighLoRegrindOptionHistory)  // transition 0 -> 1
        {
           g_nHiLowOptionSwitchedOnOffHistory = HILOTRANSITIONOFF_TO_ON;   //
        }

    }
   g_bHighLoRegrindOptionHistory = g_CalibrationData.m_bHighLoRegrindOption;

    if(g_bInitiatePurge)
    {
        g_nFunctionRunning = FUNCTION_52;
        g_bInitiatePurge = FALSE;
        PurgeBlender();
    }
    if(g_bRestartPanelNow)
    {
        g_bRestartPanelNow = FALSE;
//        RestartPanel();
    }

    if(g_bCalculateScrewSpeedConstant)
    {
        g_nFunctionRunning = FUNCTION_53;
        g_bCalculateScrewSpeedConstant = FALSE;
        CalculateScrewSpeedConstant();
    }
    if(g_nCopyFileNoToUSBRequest != 0)
    {
        g_nFunctionRunning = FUNCTION_54;
        g_nCopyFileNoToUSB = g_nCopyFileNoToUSBRequest;
//        CopyFileFromSdToUSB(g_nCopyFileNoToUSB);      // nbb--todolp-- put back in
        g_nCopyFileNoToUSBRequest = 0;
    }
    if(g_bResetPanelSlaveComms)
    {
        g_bResetPanelSlaveComms = FALSE;
        ResetPanelMBSlaveComms();
        g_nFlashRate = FLASH_RATE_1HZ;          // Reset LED flash rate to 1Hz.
    }
    if(g_bResetNetworkSlaveComms)
    {
        g_bResetNetworkSlaveComms = FALSE;
        ResetMBSlaveComms();
        g_nFlashRate = FLASH_RATE_1HZ;          // Reset LED flash rate to 1Hz.
    }

    if(g_bAddAlarmOccurredToEventLog)
    {
        g_bAddAlarmOccurredToEventLog = FALSE;
        AddAlarmOccurredToEventLog(g_ucAlarmCode,g_nAlarmComponent);             // log alarm
    }
    if(g_bAddAlarmClearedToEventLog)
    {
        g_bAddAlarmClearedToEventLog = FALSE;
        AddAlarmClearedToEventLog(g_ucAlarmCode,g_nAlarmComponent);             // log alarm
    }

    if(g_bDoSelfTestDiagnostics && !g_bCycleIndicate)
    {
        g_bDoSelfTestDiagnostics = FALSE;
        BlenderSelfTestDiagnostic();
    }
    if(g_bI2CError)
    {
        g_bI2CError = FALSE;
        AddIC2EventToLog();
    }
    if(g_bPrintModbusMessageToTelnet)
    {
        g_bPrintModbusMessageToTelnet = FALSE;
        if((fdTelnet > 0)&& g_bShowNetworkWrites)
        iprintf("%s",g_cModbusPrintBuffer);
    }
    if(g_bAddEventToEventLog && g_nHoldOffEventLogWriteCtr == 0)
    {
        g_bAddEventToEventLog = FALSE;
        AddModbusWriteEventToLog(g_cModbusEventBuffer);
        g_cModbusEventBuffer[0] = '\0';
    }
    if(g_bDoAToDTest)
    {
        g_bDoAToDTest = FALSE;
   //     if(AtoDInternalTest())
   //     {
   //         AddAToDInternalAToDTestPassedEventToLog();
   //     }
   //     else
   //     {
   //         AddAToDInternalAToDTestFailedEventToLog();
   //    }
    }

    if(g_bDecideOnRefStability)
    {
        g_bDecideOnRefStability = FALSE;
        DecideOnRefAToDStabilityTest();
    }
    if(g_bDecideOnLoadCellStability)
    {
        g_bDecideOnLoadCellStability = FALSE;
        DecideOnLoadCellAToDStabilityTest();

    }

    if(g_bFileCouldNotBeOpenedEvent)
     {
         g_bFileCouldNotBeOpenedEvent = FALSE;
         AddFileCouldNotBeOpenedEventToLog();
     }
    CheckForTelnetCommands();

    g_wProgramLoopEntry = 0;


}





//////////////////////////////////////////////////////
// TransferSetPercentagesFromComms               ASM = TRANSFERSETPOINTSFROMCOMMS
//
// transfers set % from comms
//
// P.Smith                              19-9-2005
//////////////////////////////////////////////////////


void TransferSetPercentagesFromComms( void )
{
    // TRANSFERSETPOINTSFROMCOMMS
    memcpy(&g_CurrentRecipe, &g_CommsRecipe, sizeof( g_CommsRecipe ) );
    g_bPctFlag = TRUE; // set this flag directly,
//    printf(" recipe 1 is %2.1f" , g_CurrentRecipe.m_fPercentage[0]);  //nbb--testonly--
//    printf(" recipe 2 is %2.1f" , g_CurrentRecipe.m_fPercentage[1]);  //nbb--testonly--
//    printf(" recipe 3 is %2.1f" , g_CurrentRecipe.m_fPercentage[2]);  //nbb--testonly--

    ResetAllFillAlarms();  // asm RESETALLFILLALARMS
    g_nAllowLargeAlarmBand = ALLOWLARGEBANDINCYCLES;
    if(g_arrnMBTable[BATCH_SETPOINT_RESET_TOTALS] & MB_START_CLEAN_BIT)
    {
         InitiateCleanCycle();   // ASM = INITIATECLEANCYCLE
    }
    //nbb--todo--put back decide if necessary RecalibratePulsingForComponents(); // recalibrate pulsing if required
}



//////////////////////////////////////////////////////
// CalculateDToAPercentage               ASM = CALCVS
// Calculates D/A percentage
// d/a =
//
//
// P.Smith                              16-3-2005
//////////////////////////////////////////////////////



void CalculateDToAPercentage()
{
float ftemp;

    if(g_CalibrationData.m_nControlType == CONTROL_TYPE_0_10)
    {
        ftemp = (float) g_nExtruderDAValue;   // convert to float
        g_fExtruderDAValuePercentage = (100 * ftemp / MAXEXTRUDERDTOA);   //CH1CV  DS     2        ;~CONTROL VOLTS VALUES (0-999)
        g_arrnWriteSEIMBTable[MB_SEI_DA] = g_nExtruderDAValue;  //nbb--todolp-- check this
    }
    else
    if(g_CalibrationData.m_nControlType == CONTROL_TYPE_INC_DEC) // asm = INCREASEDECREASE
    {
        ftemp = (float) g_nExtruderTachoReadingAverage;   // convert to float
        g_fExtruderDAValuePercentage = (100 * ftemp / MAXEXTRUDERTACHO);   //CH1CV  DS     2        ;~CONTROL VOLTS VALUES (0-999)
    }
}



//////////////////////////////////////////////////////
// CopySeqTableToPrevious()                     from ASM = CURRTO_PREV
// Copy current seqtable to PrevSeqTalbe.
// Entry:
// Exit:
//
// M.McKiernan                                                  30-07-2004
// First pass.
//////////////////////////////////////////////////////
void CopySeqTableToPrevious( void )
{
int     i=0;
BOOL bFinished = FALSE;
        while(!bFinished)
        {
                // move current seqtable to PrevSeqTable.
                g_cPrevSeqTable[i] = g_cSeqTable[i];
                if(g_cSeqTable[i] == SEQDELIMITER)
                        bFinished = TRUE;
                i++;
        }
}


//////////////////////////////////////////////////////
// CheckTableForNewComponent                    from ASM = CHKTABNEWCOMP
//       check seq table for new component no.

//       THIS PROGRAM CHECK FOR A NEW COMP ADDED BY COMPARING THE CURRENT
//       AND PREVIOUS SEQUENCE TABLES.

// Entry:
// Exit:
//
// M.McKiernan                                                  30-07-2004
// First pass.
//////////////////////////////////////////////////////
void CheckTableForNewComponent( void )
{
unsigned char cCompNo;
BOOL bFinished = FALSE;
BOOL bFound;
int j;
int i=0;
        while(!bFinished)
        {
                // move current seqtable to PrevSeqTable.
                cCompNo = g_cSeqTable[i] & 0x0F;  // get component no
                // check if component in SeqTable.
                bFound = FALSE;
                j=0;
                // go thru prevseqtable to see if cCompNo is in it
                while(g_cPrevSeqTable[j] != SEQDELIMITER && j < MAX_COMPONENTS && !bFound )     //
                {
                    if( cCompNo == (g_cPrevSeqTable[j] & 0x0F))
                    {       // has been found ( i.e is in both)
                        bFound = TRUE;
                        if(g_cPrevSeqTable[j] & FILLBYWEIGHT)
                        g_cSeqTable[j] |= FILLBYWEIGHT; //Set fill by weight bit if set in previous.
                    }
                    j++;
                }
                if(!bFound) // not in prev table
                {

// --review-- that the reset of the round robin is operating.


                    if(g_WorkCPIRRData.m_ComponentCPIRRData[cCompNo-1].m_ffCPIAverage < 0.001)  //nbb--todo-- check that this is the flow rate that is reset on flow calibrate
                    {
                        g_cSeqTable[i] |= FILLBYWEIGHT;         // Set fill by weight bit in new table.
                    }
                        g_cSeqTable[i] |= RECIPECHANGEDBIT;         // Indicate that recipe has changed
                        g_MainCPIRRData.m_ComponentCPIRRData[cCompNo].m_nResetCPIRRCtr = RESETRRNO;      // asm RESETRROBINCPI
                        g_Stage2CPIRRData.m_ComponentCPIRRData[cCompNo].m_nResetCPIRRCtr = RESETRRNO;    //
                        g_Stage3CPIRRData.m_ComponentCPIRRData[cCompNo].m_nResetCPIRRCtr = RESETRRNO;    //
                }

                if(g_cSeqTable[i+1] == SEQDELIMITER)    // is next element in table the delimiter??
                bFinished = TRUE;

                i++;
        }
}


void CheckCalibrationDataChecksum( void )
{
/*      merge CHECKCALCHECKSUM     */
        int nTemp,nSize;
        unsigned int unCalibrationCheckSum;
        unsigned char    g_arrcTemp[sizeof(g_CalibrationData)];
        nSize = sizeof(g_CalibrationData)- sizeof(g_CalibrationData.m_nChecksum);
        if(!g_bSaveAllCalibrationToEEprom && !g_bSaveAllCalibrationToEEpromOutsideCycle)
        {
            memcpy(&g_arrcTemp, &g_CalibrationData, nSize);    // copy structure to array.
            unCalibrationCheckSum = CalculateCheckSum(g_arrcTemp, nSize);
//            SetupMttty();
//            iprintf("\n calc is %d  act is %d",unCalibrationCheckSum,g_CalibrationData.m_nChecksum);
            if(unCalibrationCheckSum != g_CalibrationData.m_nChecksum)
            {
                nTemp = LoadAllCalibrationDataFromEEprom();  //reload calibration data   */
                if( (g_arrnMBTable[BATCH_SUMMARY_PROCESS_ALARMS] & CALINVALARMBIT) == 0 ) // tare alarm bit not set???
                {
                    PutAlarmTable( CALINVALARM,  0 );     /* indicate alarm, PTALTB   */
                }
//                SetupMttty();
//                iprintf("\n reload cal data unCalibrationCheckSum is %d g_CalibrationData.m_nChecksum is %d",unCalibrationCheckSum,g_CalibrationData.m_nChecksum);
            }
        }
}



/*
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


*/

//////////////////////////////////////////////////////
// ConvertBatchSetpointstoImperial                  from ASM = CONVERTBATSET
//
// Convert set points to imperial format
// P.Smith                                               30-07-2004
//////////////////////////////////////////////////////

void ConvertBatchSetpointstoImperial( void )
{
    if (g_CalibrationData.m_nUnitType != UNIT_IMPERIAL_MODBUS)
    {
        g_fTotalThroughputSetLbsPerHour = g_CurrentRecipe.m_fTotalThroughput * WEIGHT_CONVERSION_FACTOR;  // asm = CONVKGHRSI
        g_fTotalThroughputSetLbsPer1000Feet  = g_CurrentRecipe.m_fTotalWeightLength * WEIGHT_PER_LENGTH_CONVERSION_FACTOR; //asm = CONVKGHRSI
        g_fTotalfDesiredWidthinInches  = g_CurrentRecipe.m_fDesiredWidth * WIDTH_CONVERSION_FACTOR;  // asm = CONVWIDS
    }
}





/////////////////////////////////////////////////////
// CheckForBinPresence()        from CHECKFORBINPRESENCE
//
// This program is called continuously in the foreground.
//

// Checks if bin present in offline blend to xx kgs.
// Note that he level sensor input in inverted to ensure
// that the blender runs normally in this mode when the
// level sensor is not installed.
// Therefore when the actual level sensor is covered (inverted uncovered)
// the blender will continue blending.
// When the actual level sensor is uncovered (inverted covered) the blender
// will pause the cycle.
//
//
// P.Smith                  15-08-2005
//
//////////////////////////////////////////////////////


void CheckForBinPresence ( void )
{
    if(g_nBlenderReadyCounter ==0 )
    {
        if(g_CalibrationData.m_nBlenderMode == MODE_OFFLINEMODE && g_CalibrationData.m_nOfflineType == OFFLINETYPE_XXKG)
        {
            if( g_bHighLevelSensor == COVERED )
            {
                CloseFeed();        // close feed valve
                if(g_nPauseFlag == 0)// not paused
                {
                    g_nCyclePauseType = OFFLINE_END_OF_CYCLE_PAUSE_TYPE;    // PAUSE BLENDER AT END OF CYCLE.
                    g_nCyclePauseTypeCopy = OFFLINE_END_OF_CYCLE_PAUSE_TYPE;    // PAUSE BLENDER AT END OF CYCLE.
                }
            }
            else
            { //XITKGO
                if(g_bBinLevelSensorHistory == COVERED) // was it covered the last time
                {
                //JSR STARTOFFLINEDELAY (expanded inline)
                    CloseFeed();    // Close pinch valve in throat.
                    g_nOffline_Tracking = BATSEQ_OFFDELADMPID;  // OFFLINE_TRK = OFFDELADMPID
                    g_nOffTime1 = 0;
                    g_nDmpSec = 0;          // 1/10 sec counter.
                }
                if(!g_bOfflineBatchFilled) // unpause blender if not at target
                {
                    PauseOff();
                }
            }
        }
        g_bBinLevelSensorHistory = g_bHighLevelSensor;
    }

}




/////////////////////////////////////////////////////
// ResetAllFillAlarms()        from RESETALLFILLALARMS
//
// Resets fill alarms in modbus
//
//
// P.Smith                  4-10-2005
//
//////////////////////////////////////////////////////


void ResetAllFillAlarms ( void )
{
int i, nMBCompStatusIndex;
    nMBCompStatusIndex = 0;
    g_MainAlarmTable.m_nAlarmWord &= 0x8000;
    g_MainAlarmTable.m_nComponentFillAlarmWord = 0;
    // review is HFILLALARM necedssary

    for( i=1; i <= MAX_COMPONENTS; i++)
    {

       if( i <= 10 )   // first 10 blocks are contiguous
            nMBCompStatusIndex =    BATCH_SUMMARY_BLEND_STATUS_1 + ((i - 1) * PSUM_BLOCK_SIZE);
        // status index = start of block plus offset (of status word) in the block.
        else if( i == 11)
            nMBCompStatusIndex = PSUM_BLOCK_11 + (BATCH_SUMMARY_BLEND_STATUS_1 - PSUM_BLOCK_1);
        else if( i == 12)
            nMBCompStatusIndex = PSUM_BLOCK_12 + (BATCH_SUMMARY_BLEND_STATUS_1 - PSUM_BLOCK_1);

        g_arrnMBTable[nMBCompStatusIndex] = 0;        // clear alarm word
    }
}


/*

; IF SET POINTS UPDATED, THEN CONVERT TO IMPERIAL UNITS EVEN THOUGH IN METRIC.
;


CONVERTBATSET:
        TST     UNITTYPE
        BNE     IMPMOD          ; NO CONVERSION IF IMPERIAL MODE.
CONVERTBATSETALW:
        JSR     CONVKGHRSI
        JSR     CONVWTPMS1
        JSR     CONVWIDS
IMPMOD  RTS

CONVKGHRSI:
        LDX     #TKGHSP
        JSR     CNV3KGLB
        LDX     #SETTLBH
        JSR     EMOVE
        RTS

CONVWTPMS1 LDX     #WTPMSP       ; SETPOINT IN G/M
        JSR     CNV4GMLKF
;        LDX     #SETLBKF        ; SPT IN LB/1000'
;        JSR     EMOVE
        LDX     EREG+1
        STX     SETLBKF
        LDX     EREG+3
        STX     SETLBKF+2
CNVWPMSXX:
        RTS


CONVWIDS TST   UNITTYPE
        BEQ     CNVWIDSXX
        LDD     WIDTHSP         ; SETPOINT IN mm
        JSR     CNV2MMIN        ; CONVERT TO INCHES
        LDX     #WIDTHSPIN
        JSR     EMOVE
CNVWIDSXX:
        RTS



*/



/*
CONFOR11:
        TST     FIRSTWCYCLE     ; 1ST CYCLE ?
        BEQ     CURTOPREV
        JSR     FORMSEQTABLE    ; TABLE
        JSR     CALCSETGPM      ; G/M CALCULATION
        JSR     CLEARUNUSED
        JSR     NEEDFORTOUP     ;
        BRA     FSTCOMP
CURTOPREV:
        JSR     CURRTO_PREV     ; SEQTABLE TO PREVSEQTABLE
        JSR     FORMSEQTABLE            ; FORM SEQUENCING TABLE
        JSR     CALCSETGPM      ; G/M CALCULATION
        JSR     CLEARUNUSED
        JSR     CHKTABNEWCOMP
FSTCOMP:
        JSR     COMP1STXTOTWGT
CONFOR2:

*/


/*
;  FOLLOWING IS BACKGROUND PROGRAM, READING AND DISPLAYING DATA IS
;  HANDLED UNDER INTERRUPT CONTROL.
PGLOOP  JSR    STORTIM           ;~STORE TIME (HOURS/MINS)
        JSR   CPYTIMMB          ; COPY TIME INTO MODBUS TABLE IF MODBUS PROTOCOL

       JSR    RD_OPTS
       JSR    SERWATDOG         ;SERVICE SOFTWARE WATCHDOG (ON HC16)

       JSR    UPOPSTATS         ; UPDATE OPERATING STATUS (OPSTATUS)
       JSR    CHKPRFLG
       JSR    POSTMBWRT         ; ROUTINE TO HANDLE MB WRITE

       TST     PRINTTOMANUAL   ; PRINT TO MANUAL ?
       BEQ     NOTPRN          ; NO
       JSR     DIAGPRINTTOMANUAL ; PRINT TO MANUAL.
       CLR     PRINTTOMANUAL     ; RESET FLAG.
NOTPRN PSHM   CCR               ;SEI  ;~DISABLE IRQ'S WHILE DISPLAYING DATA
       JSR    DSVRAM            ;~DISPLAY THE VDU RAM
       LDAA   PGMNUM            ;~DEFINES WHICH DATA TO DISPLAY
       BNE    NEXTPG            ; CHECK
       JSR    UPDPG1            ;~PROG. #0, PAGE 1 DISPLAY
       JMP    CONLOOP
NEXTPG CMPA   #2                ;~PAGE 2 OF PRODUCTION SUMMARY DATA
       BNE    NEXTPG1
       JSR    UBIPAGE                   ;~DISPLAY BLENDER INFO. PAGE DATA
       JMP    CONLOOP                   ;

NEXTPG1:
       CMPA   #1                ;~1 IS CALIBRATION PROG.
       BNE    PGL008
;       TST    CPGMNO            ;~CALIB. PROG NO. =0?
;       BNE    PGL008            ;~SKIP COPYING DATA TO VRAM IF NOT 0
PL006B JSR    CDVRAM            ;~COPY A/D DATA TO VRAM
       BRA    CONLOOP

PGL008:
;        CMPA    #SEQUENCEID      ; SEQUENCE PAGE ?
;        BNE     NOTSEQ
;        JSR     SEQINFOPAGE
;        BRA     CONLOOP
NOTSEQ:
       CMPA   #12               ;~L. SPEED CAL. PGM
       BNE    PGL009
       JSR    DISSECMINLS:      ;~DISPLAY LINE SPEED (SECONDS)
       JSR    DISAD2            ;~DISPLAY A/D #2 (MACCS) DATA
       BRA      CONLOOP
PGL009:
        CMPA   #60      ;ORDER STATUS PAGE
        BNE   PGL010
        JSR   CONVWACCS ; CONVERT Wt. ACCUMULATORS IF ENGLISH UNITS
        JSR   CONVLACCS ; CONVERT Lt. ACCUMULATORS IF ENGLISH UNITS
        JSR   UPORDSTS  ; UPDATE ORDER STATUS INFO
        BRA     CONLOOP

PGL010:
        CMPA    #CURSHIFTPAGEID
        BNE     PGL0101
        JSR     CURSHIFTUP
        BRA     CONLOOP

PGL0101:
        CMPA   #30       ;PL009E ALARM PAGE
        BNE   CHKDIAGP  ;PL009F
        JSR   UALMPAG   ;DISPLAY ALARMS
        BRA     CONLOOP

CHKDIAGP:
        CMPA    #DIAGPAGEID
        BNE     CONLOOP
        JSR     UPDATEDIAG      ; UPDATE DIAGNOSTIC PAGE.

CONLOOP:
       PULM   CCR       ;CLI     ;~RE-ENABLE IRQ'S
       LDAA   CEEFLG            ;~SAVE CAL. DATA TO EEPROM?
       CMPA   #$AA
       BNE    CHKFT             ;~CHECK FILL TARGET.
       CLR    CEEFLG
       JSR    SAVCEE            ;~STORE CAL. DATA IN EEPROM.

CHKFT   TST     CYCLEINDICATE   ; IN THE MIDDLE OF A CYCLE
        BNE     CONFOR1         ; YES
        TST     FILLTARGETENTRYF
        BEQ     CONFOR1
        CLR     FILLTARGETENTRYF
        JSR     NEWTARWEIGHTENTRY

CONFOR1:
        LDAA    SAVECAL2DATA1F  ; CHECK FOR CALIBRATION DATA STORAGE ?
        CMPA    #$AA            ;
        BNE     NOTST1          ; STAGE 1 ?
        CLR     SAVECAL2DATA1F  ; RESET.
        LDAA    #$AA            ;
        STAA    SAVECAL2DATA2F  ; INITIATE STAGE 2.
        BRA     NOCAL2DW        ; CONTINUE
NOTST1  LDAA    SAVECAL2DATA2F  ;
        CMPA    #$AA            ; STAGE 2 ?
        BNE     NOCAL2DW        ; NO CALIBRATION 2 DATA WRITE.
        JSR     SAVECAL2DATA    ; SAVE CALIBRATION
        CLR     SAVECAL2DATA2F
NOCAL2DW:
        TST     HOURFG
        BEQ     NOTHR
        CLR     HOURFG
        JSR     CHECKFORTEMPOPTION      ; CHECK FOR TEMPORARY OPTION
NOTHR:
      LDAA      DISPLAYSPFILE
      CMPA      #SPFILEID
      BNE       NOTSPD1         ; NO
      JSR       SPTFILE         ; DISPLAY PAGE.
      CLR       DISPLAYSPFILE   ;
      BRA       CONSPP
NOTSPD1:
      CMPA      #DESPMNOID
      BNE       NOTSPD2         ; NO

      JSR       DESPMNO
      CLR       DISPLAYSPFILE   ;
      BRA       CONSPP
NOTSPD2:
      CMPA      #INSPFNOID
      BNE       NOTSPD3         ; NO

      JSR       INSPFNO         ; + KEY HANDLER
      CLR       DISPLAYSPFILE   ;
      BRA       CONSPP

NOTSPD3:
      CMPA      #DESPFNOID
      BNE       NOTSPD4         ; NO
      JSR      DESPFNO
      CLR       DISPLAYSPFILE   ;
NOTSPD4:



CONSPP TST      CYCLEINDICATE   ; CHECK IF IN FILLING CYCLE.
       BNE      CONFOR2         ; NO CHECK IF FILLING
       TST      RESETSTAGESEQ   ;
       BEQ      CHKPER          ; CHECK FOR % CHANGE AS NORMAL
       CLR      RESETSTAGESEQ   ; RESET FLAG
       LDAA     STAGEFILLENTEMP
       STAA     STAGEFILLEN             ; UPDATE
       LDAA     LAYERINGTEMP
       STAA     LAYERING                ; TRANSFER LAYER INORMATION.
       LDX      #STAGESEQ1              ; RESET
       LDAA     #MAXCOMPONENTNO
       JSR      CLRMEM
       LDAA   #$AA
       STAA   CEEFLG            ;~SAVE CAL. DATA TO EEPROM?




       BRA      CONFOR11
CHKPER LDAA   PCTFLG            ;~CHANNEL %'S FLAG, (AA IND.'S CHANGE)
       CMPA   #$AA
       BEQ      PERCHANGE
       TST      CSAFLG          ; HAS SET POINTS BEING UPDATED ?
       BEQ      CONFOR2        ; NO
       CLR      CSAFLG
       JSR      CONVERTBATSET   ; SET BATCH TO IMP CONV
       BRA      CONPCT
PERCHANGE:
       CLR    PCTFLG
CONPCT
       JSR    COPYSPMB          ; COPY SETPT DATA TO MODBUS TABLE

CONFOR11:
        TST     FIRSTWCYCLE     ; 1ST CYCLE ?
        BEQ     CURTOPREV
        JSR     FORMSEQTABLE    ; TABLE
        JSR     CALCSETGPM      ; G/M CALCULATION
        JSR     CLEARUNUSED
        JSR     NEEDFORTOUP     ;
        BRA     FSTCOMP
CURTOPREV:
        JSR     CURRTO_PREV     ; SEQTABLE TO PREVSEQTABLE
        JSR     FORMSEQTABLE            ; FORM SEQUENCING TABLE
        JSR     CALCSETGPM      ; G/M CALCULATION
        JSR     CLEARUNUSED
        JSR     CHKTABNEWCOMP
FSTCOMP:
        JSR     COMP1STXTOTWGT
CONFOR2:


        JSR     MONBAT          ; MONITOR BATCHING SYSTEM.
        JSR     CHKPRFLG
        JSR     LSPCHG          ; CHECK FOR CHANGE IN LSPEED.
NOPRINT:
;
CON1   LDAA   LS5SFG            ;~5 SEC LINE SPEED FLAG
       BEQ    CON1A
       CLR    LS5SFG
       JSR    LPEED5            ;~CALC LINE SPEED BASED ON 5 SEC PERIOD
CON1A  JSR     CYCMON          ; CHECK FOR BATCHING CYCLE


;PGL068
       TST   CCVSFLG            ;~FLAG TO CALL CALCVS
       BEQ   CON3
       CLR   CCVSFLG
       JSR   CALCVS             ;~CALCULATE CONTROL VOLTAGES (MOTOR SET.'S)

CON3:
;PGL072
       LDAA  NDAPRPM                    ;~NEW D/A CTS PER RPM VALUE??
       CMPA  #$AA
       BNE   PGL052
       CLR   NDAPRPM                    ;~CLR FLAG
       BRA     CON4

PGL052 LDAA   #$AA
       CMPA   NCONSS            ;~NEW SCREW SPD CONST?
       BNE    PGL056
       CLR    NCONSS            ;~CLR FLAG
       BRA      CON4
PGL056 CMPA   NMAXSS            ;~NEW MAX RPM FOR SCREW SPD
       BNE    CON4
       CLR    NMAXSS            ;~CLR FLAG

CON4:

CON5:
;                               ;PGL012
       LDAA   CALWGTCON
       BEQ    CON6              ; PGL020
       JSR    CALWGTCONST       ; CALCULATE WEIGHT CONSTANT
       CLR    CALWGTCON        ;
CON6:

       LDAA   CALCSCREWCON
       BEQ    CON6A             ; PGL020
       JSR    CALCSPCON       ; CALCULATE SCREW SPEED CONSTANT
       CLR    CALCSCREWCON
CON6A:
;;PGL022
        LDAA   TSCFLG            ;~TEN SECOND ROLLOVER FLAG
       CMPA   #$AA              ;~IS IT SET?
       BNE    CON7              ;PGL023
       CLR    TSCFLG            ;~CLEAR 10 SEC FLAG
       JSR    ACCLT_F                   ;~ACCUMULATE LENGTH FRONT & ORDER
       JSR    ACCLT_B                   ;~ACC. LENGTH BACK
       JSR    CHECKCALCHECKSUM

CON7:

       TST      RUNEST          ; RUN ESTIMATE ?
       BEQ      NOEST
       JSR     ESTKGHR
       CLR     RUNEST

NOEST  TST    TWOSFLG           ;~2 SECOND FLAG
       BEQ    CON8
       CLR    TWOSFLG
       JSR      CHKLLSALM
       JSR      PAGEDISPLAY
       JSR    UPDATEOPTIONSPAGE ; UPDATE OPTIONS PAGE.
       JSR    SSPEED            ;~CALCULATE THE (EXTRUDER) SCREW SPEED


CON8:

        TST     ONESECONDFLAG   ; CHECK ONE SECOND FLAG
        BEQ     NO1SEC          ; CLEAR
        CLR     ONESECONDFLAG
        JSR    EXTSCHK           ;~CHECK FOR EXTRUDER STALLED

        TST    SHOWCLEANSELECTION
        BEQ     NOCLN            ; 0 IGNORE.
        DEC     SHOWCLEANSELECTION
        TST     SHOWCLEANSELECTION
        BNE     NO1SEC          ; NOT ZERO YET.
        JSR     DISPLAYCLEANSELECTPAGE

; UPDATE SETPOINT PAGE FOR DOUBLE ENTRY OF REGRIND % AND MAX VALUE

NOCLN   TST     UPDATERECIPEINNSECONDS
        BEQ     CON81           ; NO PAGE UPDATE
        DEC     UPDATERECIPEINNSECONDS
        BNE     CON81           ; EXIT IF NOT ZERO
        LDAA    SHOWHIGHREGPCNTTEMP
        STAA    SHOWHIGHREGPCNT
        JSR     PTPG2A           ;~RESHOW % DATA PAGE


CON81:



NO1SEC  TST     TENHZF1
        BEQ     CON9
        CLR     TENHZF1
        JSR    CPYPSAMB          ; COPY PROD. SUMM DATA (ON 10S) TO MODBUS TABLE
        JSR    COPYMBPSUM        ; COPY D/A VALUES TO MODBUS TABLE

CON9:

       TST    RCFFLG            ;~FRONT REEL CHANGE?
       BEQ    CON11
       JSR      BATCHWTCOMP     ; CALCULATE BATCH WEIGHT USED TO NOW.
       JSR    FRELCHG           ;~FRONT REEL CHANGE SUBR
       CLR    RCFFLG            ;~CLEAR FLAG
       LDAA     PGMNUM          ; READ PROGRAM NO
       CMPA     #FROLLPAGEID
       BNE      CON11
       JSR     UPDATEFRONTROLLREPORT   ; UPDATE PAGE


CON11  TST   RCBFLG             ;~BACK REEL CHANGE?
       BEQ   CON12              ;~RC  IF MSB SET
       JSR      BATCHWTCOMP     ; CALCULATE BATCH WEIGHT USED TO NOW.
       JSR    BRELCHG           ;~FRONT REEL CHANGE SUBR
       LDAA     PGMNUM
       CMPA     #BROLLPAGEID      ; BACK ROLL PAGE DISPLAYED ?
       BNE      NOTONBACK
       JSR     UPDATEBACKROLLREPORT ; DISPLAY BACK ROLL PAGE

NOTONBACK:
       CLR   RCBFLG
CON12:



       LDAA    NEWCAL                   ;~NEW CALIB. DATA DOWNLOADED FLAG
       BEQ    CON13             ;PGL044
       CLR    NEWCAL            ;~CLR FLAG
       JSR    NETCLBN           ;~PROG. TO HANDLE NETWORK CALIB. DATA
CON13:

       JSR     DECODEKEY        ; REMOTE KEY CHECK
       LDAA     CEEFLG1         ; CALIBRATION FLAG.
       CMPA     #$AA            ; SELECT CODE FOR CALIBRATION WRITE.
       BNE      CON14           ; NO
       LDAA     #$AA            ;
       STAA     CEEFLG          ; SET CALIBRATION DATA FLAG.
       CLR      CEEFLG1
CON14:
       TST      PAGECHANGE      ; REMOTE PAGE CHANGE.
       BEQ      CON15           ; NO
       CLR      PAGECHANGE      ;
       JSR      COMPGDIS
CON15:
       TST    NEWSET            ;~NEW SETPTS DOWNLOADED FLAG
       BEQ    CON17            ;PGL048
       CLR    NEWSET
       LDAA   #$AA
       STAA   PCTFLG            ;~SET %'S CHANGED FLAG
       STAA   PCTHIS            ; SET HISTORY FLAG.
;       INC    OFLSCFG           ;~SET FLAG FOR OFFLINE APPLICATION (NEEDED IN PAUSE MODE)
       CLR    RUNFLNO           ;~NO SETPT FILE RUNNING

CON17  TST      CMRNOW         ; CMRR CALCULATION
       BEQ     CON18           ; NO
       CLR      CMRNOW
       JSR     CLDACPKGCMR     ; CALCULATE DACPKGCMR
CON18:
       TST      ACTHRREP        ; HOURLY REPORT REQUIRED ?
       BEQ      CON19           ; NO
        CLR     ACTHRREP
       JSR      GENHRREP        ; GENERATE SAME.
CON19:
        TST     ACTSHREP        ; SHIFT REPORT ?
        BEQ     CON20           ; NO
        CLR     ACTSHREP
        JSR     STORESHIFT      ; YES.
CON20:
        TST     STOPPED         ; ALLOW ORDER CHANGE IF BLENDER IS STOPPED.
        BNE     CHKTRF          ; CHECK FOR TRANSFER.
        TST     CYCLEINDICATE   ; IS THE BLENDER CYCLING ?
        BNE     CON21           ; YES

CHKTRF  LDAA    TRANSFERSPF     ; READ SET POINT FLAG.
        CMPA    #TRANSFERSPID   ; SHOULD SETPOINTS BE CHANGED.
        BNE     CON21           ; CON 21
        CLR     TRANSFERSPF     ; RESET
        JSR     TRANSFERPRODSETPOINTS ; TRANSFER PRODUCTION SET POINTS.
        LDAA    PGMNUM          ;
        CMPA    #CONFIRMCLEANPAGEID
        BEQ     XITNPS          ; NO PROD SUMMARY
        CMPA    #CLEANDIAGPAGEID
        BEQ     XITNPS          ; NO PROD SUMMARY
        CMPA    #CLEANSELECTPAGEID
        BEQ     XITNPS          ; NO PROD SUMMARY
        JSR    INIPG1           ;~PRODUCTION SUMMARY UPDATE.
XITNPS  LBRA     PGLOOP

CON21:
        CMPA    #TRANSFERSPFROMCOMMSID  ; SET POINT TRANSFER FROM COMMS
        BNE     CON22                   ; NO
        CLR     TRANSFERSPF     ; RESET
        JSR     TRANSFERSETPOINTSFROMCOMMS   ; TRANSFER PRODUCTION SET POINTS.
        JSR    INIPG1           ;~PRODUCTION SUMMARY UPDATE.

CON22:  LDAA    UPDATEDISPLAYNOW ; UPDATE DISPLAY NOW SET  ?
        BEQ     NODSUP          ; NO DISPLAY UPDATE
        CLR     UPDATEDISPLAYNOW ; RESET
        JSR     REFRESHDISPLAY   ; REFRESH DISPLAY.

NODSUP:
        JSR     ACCSEIPULSES    ; SEI PULSES ACC
        TST     STAGEFILLCALC
        BEQ     CON24
        CLR     STAGEFILLCALC
        JSR     CALCPERTARGET
CON24:
        TST     CHECKFORMAXTHP
        BEQ     CON25           ; NO
        CLR     CHECKFORMAXTHP
        JSR     CHECKLSENSOR  ; CHECK LEVEL SENSOR

CON25:

        LDAA    PGMNUM
        CMPA    #CLEANDIAGPAGEID
        BNE     CON26           ; NO
        JSR     UPDATECLEANDIAG

CON26:
        TST     STAGEFILLCALC
        BEQ     CON27
        CLR     STAGEFILLCALC
        JSR     CALCPERTARGET
CON27:

        LDAA    PGMNUM
        CMPA    #OPDIAGPAGEID
        BNE     CON28
        JSR     UPDATEOPDIAG
CON28:
        LBRA     PGLOOP


file name sbbgnd18.asm mce12-15

;  FOLLOWING IS BACKGROUND PROGRAM, READING AND DISPLAYING DATA IS
;  HANDLED UNDER INTERRUPT CONTROL.
PGLOOP  JSR    STORTIM           ;~STORE TIME (HOURS/MINS)
        JSR   CPYTIMMB          ; COPY TIME INTO MODBUS TABLE IF MODBUS PROTOCOL

       JSR    RD_OPTS
       JSR    SERWATDOG         ;SERVICE SOFTWARE WATCHDOG (ON HC16)

       JSR    UPOPSTATS         ; UPDATE OPERATING STATUS (OPSTATUS)
       JSR    CHKPRFLG
       JSR    POSTMBWRT         ; ROUTINE TO HANDLE MB WRITE

       TST     PRINTTOMANUAL   ; PRINT TO MANUAL ?
       BEQ     NOTPRN          ; NO
       JSR     DIAGPRINTTOMANUAL ; PRINT TO MANUAL.
       CLR     PRINTTOMANUAL     ; RESET FLAG.
NOTPRN PSHM   CCR               ;SEI  ;~DISABLE IRQ'S WHILE DISPLAYING DATA
       JSR    DSVRAM            ;~DISPLAY THE VDU RAM
       LDAA   PGMNUM            ;~DEFINES WHICH DATA TO DISPLAY
       BNE    NEXTPG            ; CHECK
       JSR    UPDPG1            ;~PROG. #0, PAGE 1 DISPLAY
       JMP    CONLOOP
NEXTPG CMPA   #2                ;~PAGE 2 OF PRODUCTION SUMMARY DATA
       BNE    NEXTPG1
       JSR    UBIPAGE                   ;~DISPLAY BLENDER INFO. PAGE DATA
       JMP    CONLOOP                   ;

NEXTPG1:
       CMPA   #1                ;~1 IS CALIBRATION PROG.
       BNE    PGL008
;       TST    CPGMNO            ;~CALIB. PROG NO. =0?
;       BNE    PGL008            ;~SKIP COPYING DATA TO VRAM IF NOT 0
PL006B JSR    CDVRAM            ;~COPY A/D DATA TO VRAM
       BRA    CONLOOP

PGL008:
;        CMPA    #SEQUENCEID      ; SEQUENCE PAGE ?
;        BNE     NOTSEQ
;        JSR     SEQINFOPAGE
;        BRA     CONLOOP
NOTSEQ:
       CMPA   #12               ;~L. SPEED CAL. PGM
       BNE    PGL009
       JSR    DISSECMINLS       ;~DISPLAY LINE SPEED (SECONDS)
       JSR    DISAD2            ;~DISPLAY A/D #2 (MACCS) DATA
       BRA      CONLOOP
PGL009:
        CMPA   #60      ;ORDER STATUS PAGE
        BNE   PGL010
        JSR   CONVWACCS ; CONVERT Wt. ACCUMULATORS IF ENGLISH UNITS
        JSR   CONVLACCS ; CONVERT Lt. ACCUMULATORS IF ENGLISH UNITS
        JSR   UPORDSTS  ; UPDATE ORDER STATUS INFO
        BRA     CONLOOP

PGL010:
        CMPA    #CURSHIFTPAGEID
        BNE     PGL0101
        JSR     CURSHIFTUP
        BRA     CONLOOP

PGL0101:
        CMPA   #30       ;PL009E ALARM PAGE
        BNE   CHKDIAGP  ;PL009F
        JSR   UALMPAG   ;DISPLAY ALARMS
        BRA     CONLOOP

CHKDIAGP:
        CMPA    #DIAGPAGEID
        BNE     NOTD1           ; NO
        JSR     UPDATEDIAG      ; UPDATE DIAGNOSTIC PAGE.


NOTD1   CMPA    #DIAGDUMPPAGEID
        BNE     CONLOOP
        JSR     UPDATEMEMORYDUMP

CONLOOP:
       PULM   CCR       ;CLI     ;~RE-ENABLE IRQ'S
       LDAA   CEEFLG            ;~SAVE CAL. DATA TO EEPROM?
       CMPA   #$AA
       BNE    CHKFT             ;~CHECK FILL TARGET.
       CLR    CEEFLG
       JSR    SAVCEE            ;~STORE CAL. DATA IN EEPROM.

CHKFT   TST     CYCLEINDICATE   ; IN THE MIDDLE OF A CYCLE
        BNE     CONFOR1         ; YES
        TST     FILLTARGETENTRYF
        BEQ     CONFOR1
        CLR     FILLTARGETENTRYF
        JSR     NEWTARWEIGHTENTRY

CONFOR1:
        LDAA    SAVECAL2DATA1F  ; CHECK FOR CALIBRATION DATA STORAGE ?
        CMPA    #$AA            ;
        BNE     NOTST1          ; STAGE 1 ?
        CLR     SAVECAL2DATA1F  ; RESET.
        LDAA    #$AA            ;
        STAA    SAVECAL2DATA2F  ; INITIATE STAGE 2.
        BRA     NOCAL2DW        ; CONTINUE
NOTST1  LDAA    SAVECAL2DATA2F  ;
        CMPA    #$AA            ; STAGE 2 ?
        BNE     NOCAL2DW        ; NO CALIBRATION 2 DATA WRITE.
;        JSR     SAVECAL2DATA    ; SAVE CALIBRATION
        JSR     SAVETOONBOARDEEPROM ; SAVE DATA TO ON BOARD EEPROM INSTEAD OF CAL MODULE.
        CLR     SAVECAL2DATA2F
NOCAL2DW:
        TST     HOURFG
        BEQ     NOTHR
        CLR     HOURFG
        JSR     CHECKFORTEMPOPTION      ; CHECK FOR TEMPORARY OPTION
        JSR     DECIDEONEXPIRYPAGE

NOTHR:
      LDAA      DISPLAYSPFILE
      CMPA      #SPFILEID
      BNE       NOTSPD1         ; NO
      JSR       SPTFILE         ; DISPLAY PAGE.
      CLR       DISPLAYSPFILE   ;
      BRA       CONSPP
NOTSPD1:
      CMPA      #DESPMNOID
      BNE       NOTSPD2         ; NO

      JSR       DESPMNO
      CLR       DISPLAYSPFILE   ;
      BRA       CONSPP
NOTSPD2:
      CMPA      #INSPFNOID
      BNE       NOTSPD3         ; NO

      JSR       INSPFNO         ; + KEY HANDLER
      CLR       DISPLAYSPFILE   ;
      BRA       CONSPP

NOTSPD3:
      CMPA      #DESPFNOID
      BNE       NOTSPD4         ; NO
      JSR      DESPFNO
      CLR       DISPLAYSPFILE   ;
NOTSPD4:



CONSPP TST      CYCLEINDICATE   ; CHECK IF IN FILLING CYCLE.
       BNE      CONFOR2         ; NO CHECK IF FILLING
       TST      RESETSTAGESEQ   ;
       BEQ      CHKPER          ; CHECK FOR % CHANGE AS NORMAL
       CLR      RESETSTAGESEQ   ; RESET FLAG
       LDAA     STAGEFILLENTEMP
       STAA     STAGEFILLEN             ; UPDATE
       LDAA     LAYERINGTEMP
       STAA     LAYERING                ; TRANSFER LAYER INORMATION.
       LDX      #STAGESEQ1              ; RESET
       LDAA     #MAXCOMPONENTNO
       JSR      CLRMEM
       JSR    COPYCDMB  ; COPY CALIB. DATA INTO MODBUS TABLE

       LDAA   #$AA
       STAA   CEEFLG            ;~SAVE CAL. DATA TO EEPROM?




       BRA      CONFOR11
CHKPER LDAA   PCTFLG            ;~CHANNEL %'S FLAG, (AA IND.'S CHANGE)
       CMPA   #$AA
       BEQ      PERCHANGE
       TST      CSAFLG          ; HAS SET POINTS BEING UPDATED ?
       BEQ      CONFOR2        ; NO
       CLR      CSAFLG
       JSR      CONVERTBATSET   ; SET BATCH TO IMP CONV
       BRA      CONPCT
PERCHANGE:
       CLR    PCTFLG
CONPCT
       JSR    COPYSPMB          ; COPY SETPT DATA TO MODBUS TABLE

CONFOR11:
        TST     FIRSTWCYCLE     ; 1ST CYCLE ?
        BEQ     CURTOPREV
        JSR     FORMSEQTABLE    ; TABLE
        JSR     CALCSETGPM      ; G/M CALCULATION
        JSR     CLEARUNUSED
        JSR     NEEDFORTOUP     ;
        BRA     FSTCOMP
CURTOPREV:
        JSR     CURRTO_PREV     ; SEQTABLE TO PREVSEQTABLE
        JSR     FORMSEQTABLE            ; FORM SEQUENCING TABLE
        JSR     CALCSETGPM      ; G/M CALCULATION
        JSR     CLEARUNUSED
        JSR     CHKTABNEWCOMP
FSTCOMP:
        JSR     COMP1STXTOTWGT
CONFOR2:


        JSR     MONBAT          ; MONITOR BATCHING SYSTEM.
        JSR     CHKPRFLG
        JSR     LSPCHG          ; CHECK FOR CHANGE IN LSPEED.
NOPRINT:
;
CON1   LDAA   LS5SFG            ;~5 SEC LINE SPEED FLAG
       BEQ    CON1A
       CLR    LS5SFG
       JSR    LPEED5            ;~CALC LINE SPEED BASED ON 5 SEC PERIOD
CON1A  JSR     CYCMON          ; CHECK FOR BATCHING CYCLE
       JSR    VACMON            ; CHECK IF SPECIAL SEQUENCE IS REQUIRED FOR SINGLE RECIPE MODE.


;PGL068
       TST   CCVSFLG            ;~FLAG TO CALL CALCVS
       BEQ   CON3
       CLR   CCVSFLG
       JSR   CALCVS             ;~CALCULATE CONTROL VOLTAGES (MOTOR SET.'S)

CON3:
;PGL072
       LDAA  NDAPRPM                    ;~NEW D/A CTS PER RPM VALUE??
       CMPA  #$AA
       BNE   PGL052
       CLR   NDAPRPM                    ;~CLR FLAG
       BRA     CON4

PGL052 LDAA   #$AA
       CMPA   NCONSS            ;~NEW SCREW SPD CONST?
       BNE    PGL056
       CLR    NCONSS            ;~CLR FLAG
       BRA      CON4
PGL056 CMPA   NMAXSS            ;~NEW MAX RPM FOR SCREW SPD
       BNE    CON4
       CLR    NMAXSS            ;~CLR FLAG

CON4:

CON5:
;                               ;PGL012
       LDAA   CALWGTCON
       BEQ    CON6              ; PGL020
       JSR    CALWGTCONST       ; CALCULATE WEIGHT CONSTANT
       CLR    CALWGTCON        ;
CON6:

       LDAA   CALCSCREWCON
       BEQ    CON6A             ; PGL020
       JSR    CALCSPCON       ; CALCULATE SCREW SPEED CONSTANT
       CLR    CALCSCREWCON
CON6A:
;;PGL022
        LDAA   TSCFLG            ;~TEN SECOND ROLLOVER FLAG
       CMPA   #$AA              ;~IS IT SET?
       BNE    CON7              ;PGL023
       CLR    TSCFLG            ;~CLEAR 10 SEC FLAG
       JSR    ACCLT_F                   ;~ACCUMULATE LENGTH FRONT & ORDER
       JSR    ACCLT_B                   ;~ACC. LENGTH BACK
       JSR    CHECKCALCHECKSUM
       JSR    CHECKIFVACUUMREQUIRED     ; CHECK IF VACUUMING IS REQUIRED ON STARTUP
CON7:

       TST      RUNEST          ; RUN ESTIMATE ?
       BEQ      NOEST
       JSR     ESTKGHR
       CLR     RUNEST

NOEST  TST    TWOSFLG           ;~2 SECOND FLAG
       BEQ    CON8
       CLR    TWOSFLG

       JSR      CHKLLSALM
       JSR      PAGEDISPLAY
       JSR    UPDATEOPTIONSPAGE ; UPDATE OPTIONS PAGE.
       JSR    SSPEED            ;~CALCULATE THE (EXTRUDER) SCREW SPEED


CON8:

        TST     ONESECONDFLAG   ; CHECK ONE SECOND FLAG
        BEQ     NO1SEC          ; CLEAR
        CLR     ONESECONDFLAG
        JSR     EXTSCHK           ;~CHECK FOR EXTRUDER STALLED
        JSR     CHECKFORMASTERCODE
        JSR     CHECKLICFORFLASH
        JSR     CHECKFORPENDINGRECIPE
        JSR     STRPDNT
        JSR     UPDATESTARTUPPAGE

;        JSR     CHECKFORBINPRESENCE

        TST    SHOWCLEANSELECTION
        BEQ     NOCLN            ; 0 IGNORE.
        DEC     SHOWCLEANSELECTION
        TST     SHOWCLEANSELECTION
        BNE     NO1SEC          ; NOT ZERO YET.
        JSR     PROMPTFORCLEANING

; UPDATE SETPOINT PAGE FOR DOUBLE ENTRY OF REGRIND % AND MAX VALUE

NOCLN   TST     UPDATERECIPEINNSECONDS
        BEQ     CON81           ; NO PAGE UPDATE
        DEC     UPDATERECIPEINNSECONDS
        BNE     CON81           ; EXIT IF NOT ZERO
        LDAA    SHOWHIGHREGPCNTTEMP
        STAA    SHOWHIGHREGPCNT
        JSR     PTPG2A           ;~RESHOW % DATA PAGE


CON81:



NO1SEC  TST     TENHZF1
        BEQ     CON9
        CLR     TENHZF1
        JSR     UPDATEEXPANIO   ; UPDATE CLEAN I/O
        JSR    CPYPSAMB          ; COPY PROD. SUMM DATA (ON 10S) TO MODBUS TABLE
        JSR    COPYMBPSUM        ; COPY D/A VALUES TO MODBUS TABLE

CON9:

       TST    RCFFLG            ;~FRONT REEL CHANGE?
       BEQ    CON11
       JSR      BATCHWTCOMP     ; CALCULATE BATCH WEIGHT USED TO NOW.
       JSR    FRELCHG           ;~FRONT REEL CHANGE SUBR
       CLR    RCFFLG            ;~CLEAR FLAG
       LDAA     PGMNUM          ; READ PROGRAM NO
       CMPA     #FROLLPAGEID
       BNE      CON11
       JSR     UPDATEFRONTROLLREPORT   ; UPDATE PAGE


CON11  TST   RCBFLG             ;~BACK REEL CHANGE?
       BEQ   CON12              ;~RC  IF MSB SET
       JSR      BATCHWTCOMP     ; CALCULATE BATCH WEIGHT USED TO NOW.
       JSR    BRELCHG           ;~FRONT REEL CHANGE SUBR
       LDAA     PGMNUM
       CMPA     #BROLLPAGEID      ; BACK ROLL PAGE DISPLAYED ?
       BNE      NOTONBACK
       JSR     UPDATEBACKROLLREPORT ; DISPLAY BACK ROLL PAGE

NOTONBACK:
       CLR   RCBFLG
CON12:

       LDAA    NEWCAL                   ;~NEW CALIB. DATA DOWNLOADED FLAG
       BEQ    CON13             ;PGL044
       CLR    NEWCAL            ;~CLR FLAG
       JSR    NETCLBN           ;~PROG. TO HANDLE NETWORK CALIB. DATA
CON13:

       JSR     DECODEKEY        ; REMOTE KEY CHECK
       LDAA     CEEFLG1         ; CALIBRATION FLAG.
       CMPA     #$AA            ; SELECT CODE FOR CALIBRATION WRITE.
       BNE      CON14           ; NO
       LDAA     #$AA            ;
       STAA     CEEFLG          ; SET CALIBRATION DATA FLAG.
       CLR      CEEFLG1
CON14:
       TST      PAGECHANGE      ; REMOTE PAGE CHANGE.
       BEQ      CON15           ; NO
       CLR      PAGECHANGE      ;
       JSR      COMPGDIS
CON15:
       TST    NEWSET            ;~NEW SETPTS DOWNLOADED FLAG
       BEQ    CON17            ;PGL048
       CLR    NEWSET
       LDAA   #$AA
       STAA   PCTFLG            ;~SET %'S CHANGED FLAG
       STAA   PCTHIS            ; SET HISTORY FLAG.
;       INC    OFLSCFG           ;~SET FLAG FOR OFFLINE APPLICATION (NEEDED IN PAUSE MODE)
       CLR    RUNFLNO           ;~NO SETPT FILE RUNNING

CON17  TST      CMRNOW         ; CMRR CALCULATION
       BEQ     CON18           ; NO
       CLR      CMRNOW
       JSR     CLDACPKGCMR     ; CALCULATE DACPKGCMR
CON18:
       TST      ACTHRREP        ; HOURLY REPORT REQUIRED ?
       BEQ      CON19           ; NO
        CLR     ACTHRREP
       JSR      GENHRREP        ; GENERATE SAME.
CON19:
        TST     ACTSHREP        ; SHIFT REPORT ?
        BEQ     CON20           ; NO
        CLR     ACTSHREP
        JSR     STORESHIFT      ; YES.
CON20:
        TST     STOPPED         ; ALLOW ORDER CHANGE IF BLENDER IS STOPPED.
        BNE     CHKTRF          ; CHECK FOR TRANSFER.
        TST     CYCLEINDICATE   ; IS THE BLENDER CYCLING ?
        BNE     CON21           ; YES

CHKTRF  LDAA    TRANSFERSPF     ; READ SET POINT FLAG.
        CMPA    #TRANSFERSPID   ; SHOULD SETPOINTS BE CHANGED.
        BNE     CON21           ; CON 21
        CLR     TRANSFERSPF     ; RESET
        JSR     TRANSFERPRODSETPOINTS ; TRANSFER PRODUCTION SET POINTS.
        LDAA    PGMNUM          ;
        CMPA    #CONFIRMCLEANPAGEID
        BEQ     XITNPS          ; NO PROD SUMMARY
        CMPA    #CLEANDIAGPAGEID
        BEQ     XITNPS          ; NO PROD SUMMARY
        CMPA    #CLEANSELECTPAGEID
        BEQ     XITNPS          ; NO PROD SUMMARY
        JSR    INIPG1           ;~PRODUCTION SUMMARY UPDATE.
XITNPS  LBRA     PGLOOP

CON21:
        CMPA    #TRANSFERSPFROMCOMMSID  ; SET POINT TRANSFER FROM COMMS
        BNE     CON22                   ; NO
        CLR     TRANSFERSPF     ; RESET
        JSR     TRANSFERSETPOINTSFROMCOMMS   ; TRANSFER PRODUCTION SET POINTS.
        JSR    INIPG1           ;~PRODUCTION SUMMARY UPDATE.

CON22:  LDAA    UPDATEDISPLAYNOW ; UPDATE DISPLAY NOW SET  ?
        BEQ     NODSUP          ; NO DISPLAY UPDATE
        CLR     UPDATEDISPLAYNOW ; RESET
        JSR     REFRESHDISPLAY   ; REFRESH DISPLAY.

NODSUP:
        JSR     ACCSEIPULSES    ; SEI PULSES ACC
        TST     STAGEFILLCALC
        BEQ     CON24
        CLR     STAGEFILLCALC
        JSR     CALCPERTARGET
CON24:
        TST     CHECKFORMAXTHP
        BEQ     CON25           ; NO
        CLR     CHECKFORMAXTHP
        JSR     CHECKLSENSOR  ; CHECK LEVEL SENSOR

CON25:

        LDAA    PGMNUM
        CMPA    #CLEANDIAGPAGEID
        BNE     CON26           ; NO
        JSR     UPDATECLEANDIAG

CON26:
        TST     STAGEFILLCALC
        BEQ     CON27
        CLR     STAGEFILLCALC
        JSR     CALCPERTARGET
CON27:

        LDAA    PGMNUM
        CMPA    #OPDIAGPAGEID
        BNE     CON28
        JSR     UPDATEOPDIAG
CON28:
        LDAA    PGMNUM
        CMPA    #EXPIRYPAGEID           ; EXPIRY PAGE UPDATE
        BNE     CON29
        JSR     UPDATEEXPIRYPAGE        ; UPDATE EXPIRY PAGE


CON29:
        TST     SHOWEXPIRYPAGE          ; SHOW THE EXPIRY PAGE
        BEQ     CON30
        CLR     SHOWEXPIRYPAGE
        JSR     DECIDEONEXPIRYPAGE
CON30:
        JSR      WAITFORMATERIALVACAWAY
        JSR     CHECKFORBINPRESENCE

        LBRA     PGLOOP

        LBRA     PGLOOP



REFRESHDISPLAY:
        RTS
;       JSR      INILCD
       TST    CURSORON           ; WAS CURSOR ON?
       BEQ    CURSWOFF
       JSR    CRSON             ;~TURN CURSOR ON
CURSWOFF JSR    UPVRAM            ;~HAVE ALL THE VRAM REFRESHED EACH MINUTE
        JSR    DSVRAM
        INC    ATTCFG                    ;~SET ATTRIB. CHANGE FLAG
        RTS






;;
;  ALRMON IS A SUBR. TO TURN THE ALARM O/P ON
ALRMON
        RTS
       RTS
;
;  ALRMOF IS A SUBR. TO TURN THE ALARM O/P OFF
ALRMOF RTS
;
;  AMLMPON IS A SUBR. TO TURN AUTO/MAN LAMP ON
AMLMPON PSHB
        PULB
        RTS
;
;  AMLMPOF IS A SUBR. TO TURN AUTO/MAN LAMP OFF
AMLMPOF RTS
;

;
;  GTFTIM  IS A S/R TO GET FILL TIME FOR CHANNEL IN ACC. B
;         [A,B] HAS FILL TIME, X POINTS TO FILL TIME ADDR. ON EXIT
GTFTIM LDX    #FLTIM1
GTFTM2 DECB                     ;~CHANNEL NO
       BEQ    GTFTMX            ;~FILL TIME FOUND
       AIX   #2
;;       AIX   #1                       ;~INCREMENT TO NEXT FILL TIME
       BRA    GTFTM2
GTFTMX LDD    0,X
;       LDAB   1,X               ;~D I.E.[A,B] HOLDS FILL TIME
       RTS
;
;  GTCFEN  - GET CHANNEL FILL ENABLE STATUS FOR CHANNEL IN ACC B
;            STATUS RETURNED IN ACC A, ADDRESS IS IN REG X
GTCFEN PSHB
       LDX    #CH1FEN           ;~CHANNEL 1 FILL ENABLE ADDR.
GTCFE2 DECB
       BEQ    GTCFE4
       AIX   #1
       BRA    GTCFE2
GTCFE4 LDAA   0,X               ;~LOAD STATUS OF FILL ENABLE
       PULB
       RTS
;
;
;  TIMEOFF  IS A SUBROUTINE TO CALCULATE THE TIME IN MINUTES
;           THAT THE BLENDER WAS OFF.  SUBTRACTS PREVIOUSLY
;           SAVED TIME (SVDHRS/SVDMIN) FROM CURRENT TIME (CURHRS/CURMIN)
;           C BIT SET IF TIME DIFF GREATER THAN 2 MINS
TIMEOFF JSR    READTIME                 ;READ CURRENT TIME
        LDAB   SVDHRS                   ;~FIRST CONVERT SAVED TIME TO HEX
        JSR    BCDHX1
        STAB   SVDHRS
        LDAB   SVDMIN
        JSR    BCDHX1
        STAB   SVDMIN
        LDX    #TIMEBUF
        LDAB   MIN,X
        JSR    BCDHX1                   ;~CONVERT TO HEX
        STAB   CURMIN                   ;~STORE CURRENT TIME
        LDX    #TIMEBUF
        LDAB   HOUR,X
        JSR    BCDHX1
        STAB   CURHRS

        CMPB   SVDHRS                   ;~STORED VALUE
        BHI    TMOFF2
        BEQ    TMOFF2
        ADDB   #24                      ;~IF < STORED VALUE THEN ADD 24
        STAB   CURHRS
TMOFF2  SUBB   SVDHRS                   ;~CHECK DIFF IN HOURS
        CMPB   #1
        BHI    TMOFFS                   ;~IF > 1 HOUR DIFF SET FLAG
        BLO    TMOFF4                   ;~IF = 1 TAKE CARE OF HOUR DIFF
        LDAA   CURMIN
        ADDA   #60                      ;~ADD 60 FOR HOUR DIFFERENCE
        STAA   CURMIN
TMOFF4  LDAA   CURMIN                   ;~CURRENT MINUTES
        SUBA   SVDMIN                   ;~TAKE AWAY STORED MINS
        CMPA   #2
        BHI    TMOFFS                   ;~IF > 2 MINS GO TO SET FLAG
        ANDP   #$FEFF                   ;CLC
        BRA    TMOFFX
TMOFFS  ORP    #$0100                   ;SEC ;~SET C TO SHOW > 2 MINS OFF
TMOFFX  RTS
;
;  STORTIM IS A ROUTINE TO STORE CURRENT TIME HOURS/SECS
STORTIM PSHM    X
        JSR     READTIME
        JSR     READDATE
        LDX     #TIMEBUF
;        LDAA   RTCMIN
        LDAA    MIN,X
        STAA   SVDMIN                   ;~STORE CURRENT TIME
;        LDAA   RTCHRS
        LDAA    HOUR,X
        STAA   SVDHRS
        PULM    X
        RTS
;
;  SFONFLG  IS A SUBROUTINE TO SET FILL BITS FOR A CHANNEL IN FONFLAG
;                ON INPUT, CHANNEL NO. (1-5) IS IN ACC. A
;                IF ACC. A > 7, ALL FILL O/PS ARE TURNED ON
SFONFLG TSTA                            ;~CH. NO. IN A
        LBEQ    SFONFGX                  ;~EXIT IF 0
        LDAB   #$1F                     ;~IF  > 7, SET ALL FILL BITS
        CMPA   #7
        LBHI    SFONFG8                  ;~TURN ALL O/PS ON IF > 7

        PSHA
        TAB                             ;~CH. NO. IN B
        JSR    GTCFEN                   ;~GET CHANNEL FILL ENABLE STATUS
        CMPA   #$AA
        PULA                            ;~RESTORE CH. NO
        BNE    SFONFGX                  ;~EXIT IF NOT ENABLED

        LDAB   #$20
        CMPA   #6                       ; CH. 6??
        BEQ    SFONFG8
        TAB                             ;~CH. NO -> B
        CMPA   #5
        BLO    SFONFG1                  ; <5
        LDAB   #$10                     ; ELSE ASSUME CH. 5
        BRA    SFONFG8

SFONFG1 CMPA   #3
        BLO    SFONFG8                  ;~CH. 1 OR 2, BITS IN B ALREADY OK
        BHI    SFONFG2
        LDAB   #$04                     ;~CH. 3, SET BIT B2
        BRA    SFONFG8
SFONFG2 LDAB   #$08                     ;~BIT B3 IF CH. 4
SFONFG8 ORAB   FONFLAG                  ;~SET BITS IN FILL ON FLAG
        STAB   FONFLAG
SFONFGX RTS
;
;

;       WEIGHT CALIBRATION CONSTANT CHECK
;
CALWGTCONST:

       LDX      #AN1BUF         ;
       JSR    AMOVE            ;~COPY AVG TO AREG FOR MULTIP. BY C1
       LDX    #CALBLK
       LDAB   #OFFSOF           ;~OFFSET FOR CHANNEL ZERO (OR OFFSET)
       ABX
       JSR    CMOVE             ;~COPY VALUE TO MATHCREG
       JSR    SUB               ;~AREG - MATHCREG  > RESULT IS NOW IN EREG
       LDX     #TEMPSTORE
       JSR     IFEREG          ; STORE E REG
       TST    MATHDREG              ;~CHECK FOR NEG. RESULT
       BEQ    POSRSL            ;~0, RESULT POSITIVE
       RTS
POSRSL LDAA     CALWGTCON
       CMPA     #MODBUSWEIGHTCAL
       BEQ      METRIC1
       TST      UNITTYPE        ; IMPERIAL
       BEQ      METRIC1          ; METRIC
       LDX      #NKEYBUF+2
       JSR      CNV3LBKG        ; TO KGS
       LDE      EREG+2
       LDAA     EREG+4          ; CONVERTED VALUE
       BRA      CONCAL
METRIC1:
       LDE      NKEYBUF+2
       LDAA     NKEYBUF+4

CONCAL JSR      CLCREG
       STE      MATHCREG
       STAA     MATHCREG+2
       LDX      #TEMPSTORE
       JSR      ITAREG            ;~COPY RESULT OF SUBT'N TO AREG

       JSR      DIV             ; WEIGHT XX XX X.Y 00 00 / A/D - TARED COUNTS

       LDAB   #CON1OF
       LDX    #CALBLK
       ABX                        ;~X NOW POINTS TO C1 FOR THIS CHANNEL
       LDD      EREG+1
       LDE      EREG+3          ; READ CALIBRATION CONSTANT.
       STD      0,X
       STE      2,X
       JSR      CPYWTCMB
       LDAA     #$AA
       STAA     PCTFLG          ; UPDATE COMP #1 TARGET.
       STAA     CEEFLG1           ; SET FLAG TO HAVE DATA -> EEPROM
       LDAA   PGMNUM
       CMPA   #1
       BNE    ENTR02
;
ENTR01 LDAA   CPGMNO   ;~WHICH CALIB. PROG?
       BNE    ENTR02   ;~=  0, CALIBRATION CONST
       JSR      CALBT1  ; UPDATE PAGE.
ENTR02 RTS




;
;       CALCULATE SCREW SPEED CONSTANT TO ALLOW THE OPERATOR
;       TO ENTER THE SCREW SPEED THAT THE EXTRUDER IS RUNNING AT
;
;       SCREW SPEED ENTERED  SS SS 00 00 00 / A/D- TARE 00 00 TT TT TT
;       RESULT SHIFTED 12345 - 1234 CONSTANT IS 1234
;
CALCSPCON:
        JSR     SSAD_TARE       ; A/D - TARE
        JSR     CLCREG          ; RESULT ALREADY IN A REGISTER.
        LDD     NKEYBUF+1
        STD     MATHCREG            ; RPM TO C REG
        JSR     DIV             ;C/A (SCREW SPEED) /(A/D - TARE)

        LDX     #EREG
        LDAA    #1
        JSR     GPRND1          ; ROUND TO 1 DECIMAL PLACE.

        LDAA     EREG+2
        LDE      EREG+3
        LSRA
        RORE
        LSRA
        RORE
        LSRA
        RORE
        LSRA
        RORE
        STE     SSCON1
        LDAA   #$AA
        STAA   NCONSS   ;~FLAG A NEW SCREW SPD CONST
;        INC    LPGMNO
        JSR     CALCDARMP
        LDAA    #$AA
        STAA    CALFLG
        RTS

CALCDARMP:

        JSR     CLRAC   ; A=C=0
        ;  HEXBCD2  CONVERTS A 2 BYTE in D [A;B]) HEX VALUE TO BCD in B:E (MAX 65535)
;           RESULTING BCD IS IN B:E   (IE 6 5535 FOR $FFFF)
        LDD     DA1TMP  ; READ  D/A
        JSR     HEXBCD2 ;
        STE     MATHCREG+1
        LDD     NKEYBUF+1 ; RPM
        STD     AREG+2
        JSR     DIV     ;C/A
        LDD     EREG+2  ; D/A PER RPM
        STD     DAPRPM  ;
        LDAA    EREG+4
        STAA    DAPRPM+2
ENT12X  STAA   CALFLG   ;~SET CALIB. DATA CHANGED FLAG
        JSR    COPYCDMB ; COPY CALIB. DATA INTO MODBUS TABLE
        LDAA    #$0F
        STAA    NDAPRPM
        JMP    CSPDS2   ;~DISPLAY LINE SPEED WITH NEW CONS

        RTS
        ;
;  UPOPSTATS    - UPDATE OPERATIONAL STATUS
;               B0 - CALIBRATION MODE (CALIFG)
;               B1 - SCREW CALIBRATION MODE (SCFLAG = $Ax)
;               B2 - SCREW CALIBRATION IN PROGRESS(SCFLAG = $A1)
;               B3 - SCREW CALIBRATION COMPLETED(SCFLAG = $A4)
;               B7 - MAN(0)/AUTO(1)
;               B4-B6 UNASSIGNED YET


UPOPSTATS:
        LDD     #MBSTATINCALBPOS
        TST     CALIFG
        BEQ     NOTINC                   ; CALIFG CLEAR WHEN NOT CALIB. MODE
        ORD     MBOPSTS                  ; AUTO MANUAL BIT POS.
        STD     MBOPSTS                  ;
        BRA     CONST1
NOTINC  COMD
        ANDD    MBOPSTS                  ;
        STD     MBOPSTS
CONST1  LDD     #MBSTATRESETBPOS
        TST     RESETFLG        ; RESET OCCURRED FLAG(CLEARED BY COMMs)
        BEQ     NOTRES
        ORD     MBOPSTS                  ; AUTO MANUAL BIT POS.
        STD     MBOPSTS                  ;
        BRA     CONST2
NOTRES  COMD
        ANDD    MBOPSTS                  ;
        STD     MBOPSTS
CONST2  LDD     #MBSTATAUTOBPOS
        TST     MANATO
        BEQ     INMAN
        ORD     MBOPSTS                  ; AUTO MANUAL BIT POS.
        STD     MBOPSTS                  ;
        BRA     XITUP1
INMAN   COMD
        ANDD    MBOPSTS                  ;
        STD     MBOPSTS
XITUP1  RTS
;


TRFERXTZ:
        LDAA    #8
        LDAB    #RAMBANK
        TBZK
        LDAB    NOBLNDS
TRFER   LDE     0,X
        STE     0,Z
        AIX     #2
        AIZ     #2
        DECA            ; CHECK FOR ABSOLUTE LIMIT
        BEQ     XITTRF
        DECB
        BNE     TRFER
XITTRF  RTS

;  CLRBBS  CLEARS ACC. B NO. OF BYTES FROM X
CLRBBS TSTB
       BEQ    CLRBBX            ;~CHK FOR 0 BYTES
CLRBB1 CLR    0,X
       AIX   #1
       DECB
       BNE    CLRBB1
CLRBBX RTS


        END

// file name   BBDISP73.ASM

RESETALLFILLALARMS:
       LDD    ALMBYT            ;~ALARM BYTE -> 00
       ANDD     #$8000
       STD     ALMBYT
       CLRW   CFILLALARM        ; CURRENT FILL ALARMS
       CLRW   HFILLALARM        ; ALARM HISTORY
       CLRW   MB1STATUS         ; RESET COMPONENT ALARMS.
       CLRW   MB2STATUS         ;
       CLRW   MB3STATUS         ;
       CLRW   MB4STATUS         ;
       CLRW   MB5STATUS         ;
       CLRW   MB6STATUS         ;
       CLRW   MB7STATUS         ;
       CLRW   MB8STATUS         ;
       CLRW   MB9STATUS         ;
       CLRW   MB10STATUS         ;
       CLRW   MB11STATUS         ;
       CLRW   MB12STATUS         ;
       RTS
;


*/
