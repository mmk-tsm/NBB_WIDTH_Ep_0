// **********************************************************
//   MBProgs.c                                                  *
//  Modbus programs for Batch blender                               *
//   Initialising & moving data to and from Modbus Table    *
//   Latest Changes:                04.June.2004                *
//                                                              *
//   M.McKiernan                   10-06-2004                   *
//
//    M.McKiernan                           14-09-2004
//   Copy regrind component no. (.m_nRegrindComponent) to and from modbus table in
//    CopyTargetPercentagesToMB, and LoadTargetPercentagesFromMB
//   M.McKiernan                    30-09-2004
//      Added CopySystemConfigurationToMB().
//      Added CopyLiquidCalibrationToMB(),  CopySpeedParametersToMB()
//
//   M.McKiernan                    07-10-2004
//      Added LoadSystemConfigurationFromMB( ), CopyVersionDataToMB()
//      Added CopyCalibrationDataToMB(), CopyComponentConfigurationToMB()
//      Added   CopyMeasurementAndControlParametersToMB( ), CopyHopperConstantsToMB(), CopyWeightConstantsToMB()
//      Added LoadComponentConfigFromMB( ), LoadWeightCalibrationFromMB, LoadHopperCalibrationFromMB()
//      Added LoadSpeedParametersFromMB(),  LoadMeasParametersFromMB(), LoadLiquidCalibrationFromMB()
//     Added LoadCalibrationWeightFromMB()
//    Added  CopyADCountsToMB( void );
//    Added  CopyProdSummDataToMB1Hz( void );
//    Added  CopyActualPercentagesToMB( void );
//      M.McKiernan                    22-11-2004
//      Removed CopyADCountsToMB( void ), CopyProdSummDataToMB1Hz
//
//  M.McKiernan                         13-12-2004
//  Activated RemoveAlarmTable() and PutAlarmTable.
//
//  M.McKiernan                         06-01-2005
//      Call ResetKgHRoundRobin() when batches in kg/h avg updated from MB. - LoadMeasParametersFromMB( )
//
//
// P.Smith                              10-03-2005
// Change ExitControl call to ToManual and uncomment
//
// P.Smith                              11-03-2005
// CopyDACountsToMB,CopyHopperSetWeightsToMB
//
// P.Smith                              16-08-2005
// name change g_OnBoardCalDataStruct to g_OnBoardCalData
//
// P.Smith                              17-08-2005
// Correct copy of intermittent mix to m_nIntermittentMixTime
// Copy of g_CalibrationData.m_nMixShutoffTime to / from modbus table
// Correct end of toggle status command flags initialisation.
// END_TOGGLE_STATUS_COMMANDS
//
//
// P.Smith                              18-08-2005
// Max throughput asm = MXTPT1
// Offline mixing time asm MIXTIM
// Vacuum delay used in single recipe mode ASM = VACUUMDELAY
// Post Vacuum delay used in single recipe mode ASM = POSTVACUUMDELAY
//
// remove BATCH_SETPOINT_MATERIAL_CODE reference.
// added operator no, order no, recipe no, extruder no.
// added check for single recipe mode in LoadTargetPercentagesFromMB
// move CopyBBAuxToMB to here.
// check g_nPauseFlag by anding with ANY_PAUSE_BITS

//
// P.Smith                              24-08-2005
// check for clean initiate on set point transfer.
//
// P.Smith                              29-08-2005
// added g_bInCalibrationMenu,g_bResetFlag & g_bManualAutoFlag to operating statue
//
//
// P.Smith                              30/08/2005
// Copy of production summary data for > 10 components
// CheckMBValidData allow % 9-12 to be checked for 100 %
// Copy of % set point to modbus allow for components 9 - 12
//
// P.Smith                              31/08/2005
// Correct copy of component % 9-12 index to storage array off by 1.
//
// Correct copy of g_arrnReadLLSMBTable[MB_LLS_INPUTS] to g_nLowLevelSensorInputState
//
//
// P.Smith                              31/08/2005
// only enable cleaning if g_OnBoardCalData.m_cPermanentOptions LICENSE_CLEANOPTIONBIT bit is set
//
//
// P.Smith                              10/10/2005
// merge COPYENCRYPTIONDATATOMB inline
// copy g_fTotalBatchWeightAccumulator to g_arrnMBTable[BATCH_SUMMARY_WEIGHT_ACCUMULATOR] asm MBTOTWACCBAT
// if timed out, copy set points back to modbus. CopySetpointsToMB();   asm COPYSPMB
//
//
//
// P.Smith                              12/10/2005
// do not switch blender to manual on calibration data download.
//
// P.Smith                              12/10/2005
// g_CalibrationData.m_bCorrectEveryNCycles data copy instead of bypass word
//
// P.Smith                              7/11/05
// Correct password transfer to calibration structure
//
// P.Smith                              15/11/05
// set g_CalibrationData.m_bRegrindRetry to FALSE on download of 0, was setting
// to TRUE for both instances.
// add transfer of fast / slow cycle option.g_CalibrationData.m_bCycleFastFlag
//
// P.Smith                              17/11/05
// correct transfer of fast control response.
//
// P.Smith                              5/12/05
// Removed g_nCurrentExtruderNo = g_CurrentRecipe.m_nExtruder_No; // indicate current extruder no.
// This needs to be done in progloop.c where the recipe is transferred.
//
// P.Smith                              9/1/06
// Remove stage fill enabled transition check when transferring modbus data to calibration.
//
// P.Smith                              10/1/06
// correct warnings
// removed         nTest = BATCH_SETPOINT_WIDTH;
//                 nTest = (int)g_CommsRecipe.m_fDesiredWidth;
// comment out nTest1 = g_arrnMBTable[BATCH_SUMMARY_HOPPER_WEIGHT];
//             nTest2 = g_arrnMBTable[BATCH_SUMMARY_HOPPER_WEIGHT+1];
// removed     nTemp = (int)(g_CommsRecipe.m_fPercentage[0] * 100) in CheckForPendingRecipe
// multiplication by 1 removed from uValue.lValue = (long)(ffWtConst * 1.0) in CopyWeightConstantsToMB;
// removed nTemp from void CopyWeightsToMB( void )
// int casting added to g_arrnMBTable[BATCH_SUMMARY_WEIGHT_ACCUMULATOR] = (int) g_fTotalBatchWeightAccumulator;  /* asm MBTOTWACCBAT */
//
//
// P.Smith                      15/2/06
// first pass at netburner hardware conversion.
/// Bool -> BOOL
//
// P.Smith                      23/1/06
// #include <basictypes.h>
// no type defined for g_arrnReadLLSMBTable[]; extern  int g_arrnReadLLSMBTable[];
// comment out all unavailable functions.
//
// P.Smith                      17/2/06
// changed unions IntsAndLong to WordAndDWord
// removed rtc ram defined in this file.
// changed unions CharsAndInts to CharsAndWord
// verified copyTimeToMB();
// call SetTime();
//      SetDate();                                                             // write time and date to RTC.
// #include "TimeDate.h"
// CopyFBWtsToMB uncommented.
//#include "MBProsum.h"
//
// P.Smith                      20/2/06
//#include "MBTogSta.h"
// MBToggleStatusCommands() uncommented
//
//
// P.Smith                      24/2/06
// TareWeighHopper
// #include "BBCalc.h"
//CalculateHopperWeight(); // calculate current weight in hopper ASM = CALHWT
// #include "Monbat.h"
// in LoadCalibrationWeightFromMB use g_lOneSecondWeightAverage instead of g_lSettledAverage
//
// P.Smith                      28/2/06
// RemoveAlarmTable( PERALARM,  0 );       // indicate alarm cleared, Component no = 0 i.e. not component alarm.
// PutAlarmTable( PERALARM,  0 );      // indicate alarm, Component no = 0 i.e. not component alarm.
// #include "Alarms.h"
// SetStartCleanMBBit();
// ResetCleanFinishMBBit();
// ResetKgHRoundRobin();                                   // Reset kg/h R.R. if the batches in avg has changed.
// #include "CycleMonitor.h"
// #include "KghCalc.h"
// SetCopyFlowRatesCommand();
//
// P.Smith                      19/4/06
// implemented CheckProductionSetpointsTotal100 call
//
//
// P.Smith                              19/5/06
// use BATCH_START_TOGGLE_STATUS_COMMANDS when setting up parameter flags.

//
// P.Smith                              19/5/06
// added     g_arrnMBTable[BATCH_EXTRUDER_SET_PERCENTAGE] = g_fExtruderDAValuePercentage;       //
// g_arrnMBTable[BATCH_SUMMARY_ORDER_WEIGHT] = 0;
// g_arrnMBTable[BATCH_SUMMARY_ORDER_WEIGHT+1] = (int)(g_fTotalOrderWeightAccumulator);  //
// correct copy of version no.
// removed  g_arrnMBTable[BATCH_DAC_KGH] = (int)(10 * g_fDtoAPerKGH);   //DACKGHR  D/A PER KGH - to 1 DP.
// removed  g_arrnMBTable[BATCH_DAC_MPM] = (int)(10 * g_fDtoAPerMPM);   //DACPMPM  D/A PER M/min - to 1 DP.
// if(!g_CalibrationData.m_bCorrectEveryNCycles)    // asm = CORRECTEVERYNCYCLES
// chars and int changed to chars and word in Copyversiondata
//
// P.Smith                            25/5/06
// multiply modbus data by conversion factor.
//
// P.Smith                            26/5/06
// add conversion factor in void CopyHopperSetWeightsToMB( void )
//
// P.Smith                            31/5/06
// call CopyOrderReportData();          // ASM = ASSORPTASSEMBLE ORDER REPORT if bit is 0.
// #include "Blrep.h"
//
// P.Smith                            8/6/06
// g_CalibrationData.m_fBatchSize imperial conversion added in  LoadHopperCalibrationFromMB
// also added imperial conversion  for read of  batch size in LoadHopperCalibrationFromMB
// imperial conversion of g_CalibrationData.m_nMaxThroughput;
// conversion of g_fTotalOrderWeightAccumulator in CopyWeightsToMB

// M. McKiernan                     12/6/2006
// Increased the size of cBuffer in CopyVersionDataToMB( void ) i.e. char cBuffer[20];
//
//
// P.Smith                            13/6/06
// call ForegroundSaveRecipe( );    // save recipe while running in foreground (1 byte per pass).
// check g_CalibrationData.m_nUnitType == UNIT_IMPERIAL in SetupConversionFactors
//
// P.Smith                            15/6/06
// added email config of parameter table oring with MB_WRITE_EMAIL_CONFIG_BIT
// check for email in  PostMBWrite (MB_WRITE_EMAIL_CONFIG_BIT )
//
// P.Smith                            19/6/06
// added copy of blender name
// call CopyEmailConfigDataToMB(); called in InitialiseMBTable
// implement LoadEmailConfigFromMB for download of email definitions
// set of g_CalibrationData.m_bSendEmailOnOrderChange from modbus table
// Latency load and copy to modbus table
//
// P.Smith                            21/6/06
// remove CopyEmailConfigDataToMB causing problems if the text strings are not
// terminated.
// put back in CopyEmailConfigDataToMB();
//
// P.Smith                            10/7/06
// remove ResetKgHRoundRobin() in  g_CalibrationData.m_nBatchesInKghAvg check
// only check MB_WRITE_PERCENTAGES_BIT for order reset totals
// remove g_CalibrationData.m_bHighLoRegrindOption transition check from here
// remove layering transition check from here g_CalibrationData.m_bLayering
//
// P.Smith                            16/11/06
// remove reference to monitoring factor.
// clear area 2 of toggle status commands.
// use BOOL g_bCycleIndicate
// remove reference to level monitoring.
// implement line speed signal type
// Reference g_CalibrationData.m_fDtoAPerKilo & g_CalibrationData.m_fDtoAPerMeterPerMinute
// in SetUpConversionFactors check for modbus imperial mode
//
// P.Smith                            11/1/07
// name change g_CalibrationData.m_nLineSpeedSignalType
//
// P.Smith                              20/2/07
// added BATCH_START_SYSTEM_CONFIG2 & BATCH_END_SYSTEM_CONFIG2
// remove printfs
//
// P.Smith                              30/4/07
// Ensure set up of g_CommsRecipe.m_nFileNumber to RUNNINGRECIPEFILENO before recipe save
//
// P.Smith                              3/5/07
// allow for microns and density in set point download
//
// P.Smith                              23/5/07
// Copy Power up time date to modbus table, this was originally copied into a word
// now each piece of data is in a single word.
//
// P.Smith                              28/5/07
// Copy Reset counter, network settings for netburner to modbus
//
// P.Smith                              8/5/07
// ensure that  g_CalibrationData.m_nRecipeEntryType is copied to / from modbus table.
//
// P.Smith                              11/6/07
// g_nMBWriteOrFlagTemp set to 0 on transfer to g_nMBWriteOrFlag
// allow entry type to be read and written from / to modbus.
// added CheckForRecipeDownLoad - not implemented yet.
// a new download option has been added,the recipes can be written down in two stages
// a bit is set in BATCH_SETPOINT_DOWNLOAD_MANAGER word to indicate that no download should take
// place, then it is set to 2 to indicate that the download should take place.
//
// P.Smith                              11/6/07
// read ip address from EthernetIP,EthernetIpMask,EthernetIpGate,EthernetDNS
// copy mac address and DHCP status
//
// P.Smith                              12/6/07
// copy comms counters for optimisation card to diagnostics location in modbus table.
//
// P.Smith                              22/6/07
// copied network comms counters to modbus.
// read cycle counter from modbus
//
// P.Smith                              27/6/07
// added purge enable / disable and optimisation in calibration data
//
// P.Smith                              4/7/07
// added copy of bin volume to /from modbus table
//
// P.Smith                              19/7/07
// m_bPurgeEnable changed to m_nPurgeEnable
//
// P.Smith                              2/8/07
// copy in is diagnostics password to modbus table
// g_CalibrationData.m_nCurrentExtruderNo copied into modbus table
//
// M.McKiernan                      17/9/07
// Removed g_arrnMBTable[] & g_arrnMBTableFlags[], now in BatVars2
//       g_nMBWriteOrFlag, g_nMBWriteOrErrorFlag, g_nMBWriteOrFlagTemp
//
// P.Smith                          15/10/07
// added copy of g_CalibrationData.m_bComponentNamingAlphanumeric to modbus table
//
// P.Smith                          18/10/07
// g_CalibrationData.m_nCleaning has changed to an int, load and copy cleaning
// to new modbus position at modbus address BATCH_CALIBRATION_CLEANING
//
// P.Smith                          18/10/07
// g_fTotalHistoryBatchWeightAccumulator is copied into modbus table to 1 decimal place.
// originally this was copied in as kgs.
//
// P.Smith                          12/11/07
// add copy and load of line speed average m_nLineSpeedAverage BATCH_CALIBRATION_LINE_SPEED_AVERAGE
// added linespeedavg read and write
// if blender is in single word write and in g/12 sq feet allow immediate download of the g/12sq feet.                          ;
//
// P.Smith                          14/11/07
// Correct width download, ensure that 2 decimal places are used in the download
//
// P.Smith                          15/11/07
// allow for imperial conversion for g_fTotalHistoryBatchWeightAccumulator
// correct CopyHopperSetWeightsToMB for imperial mode if > 10 components
//
// P.Smith                          29/11/07
// Set g_bDoCMRDToAPerKgCalculation = TRUE if in cmr mode on download of max kg/hr
//
// P.Smith                          6/12/07
// Add % deviation to modbus table . some corrections required     ;
// added m_nPauseOnErrorNo copy to / from modbus table
// added m_bDeviationAlarmEnable copy to /from modbus table
// added check for g_bShowInstantaneousWeight CopyActualPercentagesToMB
// copy of m_fMaxPercentageDeviation to modbus table.
// copy of component % depending on g_bShowInstantaneousWeight, copy g_fComponentActualPercentageRR
// if g_bShowInstantaneousWeight is false
//
//
//  P.Smith                                 4/1/07
//  Call resetallwgtrrobin on target entry.
//
//  P.Smith                                 10/1/08
//  added copy and load of g_CalibrationData.m_bPauseBlenderOnPowerup;
//
// P.Smith                          30/1/08
// correct compiler warnings
//
// P.Smith                          30/1/08
// a problem has been seen in the software in that when the througput / g/m set points
// are sent down on their own, ie no percentages, this does not get transferred to the
// set point area. This is due to the fact that the or flag is only checking for the
// percentage flag in postmbwrite. this is corrected.
// This now leads to another problem in that when the kh/h / g/m is sent down after a power up,
// the comms buffer is not set up correctly, it is 0 initially and when the
// initiate transfer is done the component percentage set points are set to 0.
// by calling LoadPercentageTargetsFromComms & LoadTargetThruputsFromMB, the set component percentages are reloaded into the comms recipe
// structure, the same applies to the g/m and kg/h
// In post mb write, the or flag is checked for percentage, order and throughput download
// this allows the throughput download to operate properly.
// reset order totals is only run if if MB_WRITE_PERCENTAGES_BIT is set in or word.
// if g_bPanelWriteHasHappened is set to true then the it is cleared in modbus post mb write

// another problem was the spilling over of the write or flags into the modbus table.
// the first modbus word was being set to 8000, this was due to incorrect initialisation
// of the parameter flags, this was due to a <= comparision being used in the set up
// function InitialiseMBTable.
// in InitialiseMBTable, check for < MB_TABLE_WRITE_SIZE, not <= MB_TABLE_WRITE_SIZE
// as this accesses one word extra that is not necessary and is the 1st word of the modbus table
// check LoadPercentageTargetsFromComms for NewSetPercentageAreTheSame, if same exit
// check LoadTargetThruputsFromMB for NewSetThroughputsAreTheSame, if same exit
//
// P.Smith                          14/2/08
// CopySdCardDetailsToMB

// P.Smith                          15/2/08
// added void PollUSBMB( void )
// in CopySdCardDetailsToMB, copy g_bUSBCopyInProgress & g_lUSBFileDataCopied to modbus
//

// P.Smith                          19/2/08
// add m_nLogToSDCard & m_bLogFormatIsCSV copy to / from modbus
// implement PollUSBMB, if mb : USB_READ_SD_CARD_DIRECTORY non zero, set g_bReadSDCardDirectory
// if USB_COPY_FILE_TO_SD_CARD mb non zero, set g_nCopyFileNoToUSBRequest to file no.
// reset modbus cell when detected as non zero.
//
// P.Smith                          25/2/08
// correct CopyTargetThruputsToMB copy of desired and trimmed width, problem found by mmk
//
// P.Smith                          7/3/08
// if increase /decrease copy back m_fRPMPerKilo to modbus table
// in void CopyDACountsToMB copy g_arrnWriteSEIMBTable[MB_TACHO_SETPOINT] to
// mb table
//
// P.Smith                          20/3/08
// remove variable g_nManagerPassword, use g_CalibrationData.m_lManagerPassWord
// remove download of screw speed constant and tare value on download
//
// P.Smith                          28/3/08
// use g_ProcessData for cycle counter and reset counter, m_fDtoAPerKilo,m_fRPMPerKilo ,m_fDtoAPerMeterPerMinute
//
// P.Smith                          23/4/08
// added BATCH_CALIBRATION_ALLOW_CONTINUOUS_DOWNLOAD with seting of g_CalibrationData.m_bAllowForContinuousDownload
//
// P.Smith                          24/4/08
// g_bAllowTransferToCurrentOrder set to allow the current order to be transferred.
// if the percentage or set point data is  the same and the repeated download option
// is enabled, then there is no need to initiate the transfer of the current order.
// g_bAllowTransferToCurrentOrder is checked before InitiateRecipeTransferToCurrentOrder is called
//
// P.Smith                          28/4/08
// copy g_nExtruderDAValue to modbus table
// in SetUpConversionFactors check for MODE_GP12SQFEET mode, if it is set the
// the conversion factor to 1.0, this will allow the g/m to go back in metric
// format and show the calculated grams per meter from the set g/12sq feet value
//
// P.Smith                          29/4/08
// name change g_CalibrationData.m_nCurrentExtruderNo to g_nCurrentExtruderNo
//
// P.Smith                             29/4/08
// correct passwords BATCH_CALIBRATION_PASSWORD_SUPERVISOR & BATCH_CALIBRATION_PASSWORD_OPERATOR
//
// P.Smith                             29/4/08
// added mmk compares, better to do the comparision before load of data
// call StoreMBSetpointHistoryArray in CopyTargetPercentagesToMB, CopyTargetThruputsToMB
// CopyOrderInfoToMB
// added check for g_bPanelWriteHasHappened to allow download to happen eventhough
// the data is the same. there was a problem here when the multiblend was tried out.
// it did not work initially due to this.
// remove duplicate check in postmbwrite
// correct copy CopyDACountsToMB of d/a value.
// name change m_bLogToSDCard
// cycle counter now named g_nCycleCounter no longer in eeprom
// in SetUpConversionFactors if gp12sq feet conversion factor is 1.
//
// P.Smith                             4/6/08
// added copy of hardware id to modbus table
//
// P.Smith                             6/6/08
// change % check to > 9995 && nTotal < 10005
// check for m_bAllowForContinuousDownload
// copy complete bools for sd card to mb table.
//
// P.Smith                          12/6/08
// added CopyAlarmsToIndividualMBWords
// This copies the alarm and process alarm words to individual bits in the modbus
// table.
//
// P.Smith                          17/6/08
// added Telnet option
//
// P.Smith                          30/6/08
// add InitialiseCommsRecipe, was missing LoadOrderInfoFromMB
// vacuum loading
// return status for various communicating devices.
// return sei version no.
// copy links l1,l2,l3 to modbus table.
//
// P.Smith                          15/7/08
// set g_bWeighHopperCalibratedEvent to TRUE to show hopper calibrated event
// this cannot be run immediately as the comms cannot complete due to the write to
// the sd card.
// copy blender type to PSUM_BLENDER_TYPE
//
// P.Smith                          15/7/08
// remove commented out code, remove PollUSBMB
//
// P.Smith                          23/7/08
// remove g_arrnMBTable,g_PowerUpTime,g_CurrentTime & comms flags externs
// correct year addition / subtraction of century is done when setting/ reading time
//
// P.Smith                          24/7/08
// copy g_nDisableSDCardAccessCtr to     g_arrnMBTable[BATCH_SD_CARD_DISABLE_ACCESS_CTR]
//
// P.Smith                          25/7/08
// remove copy of usb file data size copy, this was over writing the sd card status.
//
// P.Smith                          12/8/08
// Correct conversion of time when time is being set from modbus
//
// P.Smith                          18/9/08
// add _CalibrationData.m_nLiquidAdditiveKgPerHour copy to /from modbus
//
// P.Smith                          13/10/08
// copy g_CalibrationData.m_fLiquidAdditiveScalingFactor to modbus table
// copy long term counters to BATCH_CYCLE_LONG_TERM_LENGTH & BATCH_CYCLE_LONG_TERM_LENGTH_RESETTABLE
//
// P.Smith                          17/10/08
// if g_nDisableSDCardAccessCtr is non zero, then set modbus address BATCH_SD_CARD_DISABLE_ACCESS_CTR
// to 1, otherwise set it to zero.
//
// P.Smith                          29/10/08
// check if blender type is within the normal batch blender range.
// if it is not,then set to the first blender type
//
// P.Smith                          21/11/08
// when copying g_CommsRecipe.m_arrOrderNumber swap the bytes to allow for the panel.
// download and copy.
//
// P.Smith                          9/12/08
// add MB_WRITE_SYSTEM_CONFIG3_BIT initialisation
// CopySystemConfiguration3ToMB added
// set mb flags for new message.
// added LoadSystemConfiguration3FromMB
// remove original print options
//
// P.Smith                          7/1/09
// copy max regrind % back to mb table in CopyTargetPerctagesToMB
// this was causing the mb cell 10 to go to 0 when the max value was downloaded.
// if hilo regrind option,copy data from g_CurrentRecipe into the modbus table for the
// set points.
// this allows the set point to be seen on the summary screen
// copy high reg level sensor status to op status word BATCH_STATUS_HIGH_REGRIND_LEVEL_SENSOR
//
// P.Smith                          12/1/09
// set the pause bit in the modbus table if pausflg or g_nCyclePauseType is eoc.
//
// P.Smith                          21/1/09
// CopyFunctionRunningToMB implemented to copy function data to modbus
// this is called on the pit so that if the foreground is held up, the data will
// still be available in modbus.
//
// P.Smith                          21/1/09
// added parity check enable BATCH_COMMUNICATIONS_NO_PARITY copy to mb table
//
// P.Smith                          6/2/09
// removed set of g_nHiLowOptionSwitchedOnOff from here this is now done in the foreground
//
// P.Smith                          11/2/09
// copy usb timeout counters to mb
//
// P.Smith                          5/3/09
// if diagnostics password, then save calibration constant to factory settings
// copy panel status to BATCH_COMMS_STATUS_PANEL
//
// P.Smith                          1/4/09
// copy serial no BATCH_NBB_SERIAL_NUMBER to mb
// added LoadPanelVersionNumber to copy panel software version to ram
// correct g_cPanelVersionNumber null terminator addition
//
// P.Smith                          3/4/09
// check for valid character for order number, if char are not valid, name it
// order.
// check for a valid regrind component number, if not valid set it to 0.
//
// P.Smith                          20/4/09
// call CopyEncryptionDataToMB (no function originally), this needs to be called
// on reset as well
//
// P.Smith                          24/4/09
// copy sei software revision to g_cSEISoftwareVersionNumber
// copy nbb pass / fail to mb
//
// P.Smith                          18/5/09
// reset totals flag set to only set the write bit and not the reset totals bit.
// call CheckForValidTime on time download
//
// P.Smith                          3/6/09
// added BATCH_CHECK_FOR_VALID_COMPONENT_PER_TARGET data transfer to m_bCheckForValidComponentTargetPercentage
//
// P.Smith                          16/6/09
// copy CopyVacuumLoaderCalibrationParametersToMB in InitialiseMBTable
//
// P.Smith                          30/6/09
// CopyConfigDataToMB formed to group all config data copy together.
//
// P.Smith                          2/7/09
// copy g_nBatchCommsStatusVacuumLoading to modbus table BATCH_COMMS_STATUS_VAC8S
//
// P.Smith                          8/7/09
// added m_wFillingMethod copy to / from modbus table
//
// P.Smith                          21/7/09
// added LoadICSRecipeMBData
//
// P.Smith                          24/8/09
// correct CopyDACountsToMB, copy g_nExtruderControlSetpointIncDec to modbus
// table if increase / decrease mode
// call CheckForValidCalibrationData to when config and weight cal data is downloaded
//
// P.Smith                          25/8/09
// copy offline batch weight to its new location.
// it needs to be a double to allow a large kgs to be displayed.
// modbus table address BATCH_SUMMARY_WEIGHT_ACCUMULATOR_DOUBLE 657
//
// P.Smith                          1/9/09
// if g_bConfigIsLocked and download is tried, then config data is reloaded.
// by CopyHopperConstantsToMB in LoadHopperCalibrationFromMB
// also in LoadSystemConfigurationFromMB if g_bConfigIsLocked reload config data
// also in LoadSystemConfiguration3FromMB if locked copy old config back to mb table
//
// P.Smith                          4/9/09
// TSM or Engineering password. - g_CalibrationData.m_lTSMPassWord -- g_arrnMBTable[BATCH_CALIBRATION_PASSWORD_TSM];
//
// P.Smith                          10/9/09
// change orientation of m_bCycleFastFlag
// call CheckForValidCalibrationData with Bool to show that batch size should be checked
// only calibration if weight is greater than 0.001 kgs.
// generate alarm if not, store calibrated weight to g_fCalibrationWeight
//
// P.Smith                          15/9/09
// corrected the copy of the filling method.
//
// P.Smith                          17/9/09
// check for diagnostics password before allowing blender to go into volumetric mode.
// if not allowed reload modbus data to mb table.
// change m_bCycleFastFlag back to the way that it was.
// m_bCycleFastFlag = true is normal speed.
// m_bCycleFastFlag = false is fast speed
//
// P.Smith                          1/10/09
// added BATCH_MIN_OPENING_TIME_CHECK
//
// P.Smith                          5/10/09
// in LoadPanelVersionNumber swap the panel version no
// add modbus BATCH_FLOW_RATE_CHECK copy and load
// added g_fMaxKgPHrPossible copy to BATCH_MAX_KGPH_POSSIBLE
//
// P.Smith                          15/10/09
// added vac 8 expansion enabled / disable. BATCH_VAC8_EXPANSION
//
// P.Smith                          12/11/09
// correct single word write check. the problem is that the CheckMBValidData
// function is allowing the download of the % to be initiated when the % is added
// to the modbus table.
// allow the panel download to bypass the single word write.
//
// P.Smith                          3/12/09
// correct comparison = > == g_sDescriptionData.m_cNBBTestPassFailStatus
// MAX_SD_CARD_FILE_NAME_SIZE - > MAX_SD_CARD_FILE_NAME_SIZE/2 in CopySdCardDetailsToMB
//
// P.Smith                          7/12/09
// correct max throughput in imperial mode
// remove download of max throughput from hopper calibration
//
// P.Smith                          11/1/10
// copy sd card error condititons to modbus,copy error file name as well.
//
// P.Smith                          14/1/10
// modify software to copy the data from eeprom for sd card error data
// add long term error counter.
// use SD_CARD_NAME_SIZE definition when copying name.
// allow a 1 in the order file name.
//
// P.Smith                          27/1/10
// modify order name check to terminate when a space is detected.
// check for invalid character or 0 characters is done.
// if invalid, then the order name is set to order
// the new panel sends 0's as the order number.
// modified to check for 0's or spaces to terminate the string
//
// P.Smith                          5/3/10
// put in //    g_arrnMBTable[BATCH_CALIBRATION_BLENDER_TYPE] = g_CalibrationData.m_nBlenderType;  //update MB table with default.
// this is used when the blender model is fixed.
// disable setting of blender model via modbus, this will now be done by the licensing progam.
//
// P.Smith                          16/3/10
// Peter Sloan has been a problem where the actual accumulated data lbs on
// the production summary page is 2.2 times what it should be.
// this is corrected.
//
// P.Smith                          15/4/10
// if unrestricted access, then allow the blender model to be changed.
// if not, then the blender model cannot be changed.
//
// P.Smith                          27/4/10
// add copy of g_CalibrationData.m_nBaudRate variable to /from modbus
//
// P.Smith                          7/7/10
// added width offset for width ultrasonic
//
// P.Smith                          16/9/10
// add new width setpoint, remove width offset transfer,this no longer comes from
// here.
//
// P.Smith                          20/9/10
// copy WIDTH_COMMAND_WIDTH_SETPOINT for the width control from modbus to the setpoint
// area
//
// P.Smith                          21/9/10
// remove 100% check as this is not relevant for the width control.
//
// P.Smith                          23/9/10
// PSUM_I_ROUND_DECIMAL_PLACES & PSUM_I_DECIMAL_PLACES_MULTIPLICATION
// used for summary data
// PSUM_M_ROUND_DECIMAL_PLACES & PSUM_M_DECIMAL_PLACES_MULTIPLICATION
// used for summary data
//
// M.McKiernan						23/4/2020
// g_arrnMBTable[BATCH_CALIBRATION_VAC_LOADING] replaced by g_arrnMBTable[CALIBRATION_MACHINE_TYPE]
// Machine type fixed at Width control - g_CalibrationData.m_nMachineType = MACHINE_TYPE_WIDTH_CONTROL;
/* wfh.
    if(g_arrnMBTable[BATCH_CALIBRATION_VAC_LOADING] !=0)
    {
        g_CalibrationData.m_bVacLoading = TRUE;
    }
    else
    {
        g_CalibrationData.m_bVacLoading = FALSE;
    }
*/
//    g_CalibrationData.m_nMachineType = MACHINE_TYPE_WIDTH_CONTROL;
//****************************************************************************
// INCLUDES
//*****************************************************************************
#include <stdio.h>

#include "predef.h"
#include <ctype.h>
#include <startnet.h>
#include <autoupdate.h>
#include <dhcpclient.h>
#include <http.h>
#include <htmlfiles.h>


#include "SerialStuff.h"
#include "BatchMBIndices.h"
#include "BatchCalibrationFunctions.h"
#include "General.h"
#include "ConversionFactors.h"

#include "16R40C.h"
#include "ConfigurationFunctions.h"
#include "MBMHand.h"
#include "MBSHand.h"

#include "MBProgs.h"
#include "SetpointFormat.h"
#include "TimeDate.h"
#include "BatVars.h"
#include "BBCalc.h"
#include "MBTogSta.h"
#include "Alarms.h"
#include "KghCalc.h"
#include "BatVars2.h"
//nbb-todoh-- #include "Conalg.h"
#include "Monbat.h"
#include "Blrep.h"
#include "MBProsum.h"
#include "Pause.h"
#include "CycleMonitor.h"
#include "TSMPeripheralsMBIndices.h"

#include <basictypes.h>
#include <string.h>
#include "NBBgpio.h"
#include "ConversionFactors.h"
#include <system.h>
#include "Weightrr.h"
#include "rta_system.h"
#include "Secret.h"
#include "Eventlog.h"
#include "Mbvacldr.h"
#include "License.h"
#include "WidthMB.h"


//mmk unsigned int g_arrnMBTable[MB_TABLE_SIZE];
//mmk unsigned int g_arrnMBTableFlags[MB_TABLE_WRITE_SIZE];
//int g_nMBWriteOrFlag;
//int g_nMBWriteOrErrorFlag;
//int g_nMBWriteOrFlagTemp;
int g_nLoadNewSetpointsLag = 0;
BOOL g_bNewCommsRecipeFlag  = FALSE;
BOOL g_bMBWriteScrewSpeedFlag;  // MBWSSFLAG DS    1       ; FLAG TO SHOW SCREW SPEED DOWNLOAD VIA MB

int g_nProtocol;


extern  CalDataStruct   g_CalibrationData;
extern  structSetpointData  g_CurrentRecipe;
extern  structSetpointData  g_TempRecipe;
structSetpointData  g_CommsRecipe;
structSetpointDataAdditionalData  g_CommsRecipeAdditionalData;
structSetpointDataAdditionalData  g_TempRecipeAdditionalData;


extern  unsigned char g_cBlenderNetworkAddress;


extern  structControllerData    structUFP1Data;
extern  structControllerData    structUFP2Data;
extern  structControllerData    structSEIData;
extern  structControllerData    structLLSData;
extern  structControllerData    structOptimisationData;
extern  OnBoardCalDataStruct   g_OnBoardCalData;
extern  char   *RevisionNumber;
extern  structCommsData         g_sModbusNetworkSlave;
extern  BYTE DS2432Buffer[];
extern  BOOL           g_bL1Fitted;                     // link L1 fitted
extern  char           g_cBatSeq;                                                              // Batch sequence                                                               //BATSEQ
extern  unsigned int g_nAllowUnrestricedAccess;


//////////////////////////////////////////////////////
// InitialiseMBTable
//
// Initialises MB table.
// Initially clears table, sets up flags, and moves data into table.
//
// M.McKiernan                          04-06-2004
// First Pass
//
//////////////////////////////////////////////////////
void InitialiseMBTable( void )
{
    int i;
    for(i=0; i < MB_TABLE_SIZE; i++)
    {
        g_arrnMBTable[i] = 0;
    }
    // testonly //
//  for(i=0; i < MB_TABLE_SIZE; i++)
//  {
//      g_arrnMBTable[i] = i+1;                 // fill up modbus table with register no.'s
//  }

    CopySetpointsToMB();                // COPYSPMB Copy setpoint data into MB table.
    CopyConfigDataToMB();
    // endtestonly
    InitialiseMBTableFlags();
    InitialiseCommsRecipe();
    g_bMBWriteScrewSpeedFlag = FALSE;   // FLAG TO SHOW DOWNLOAD OF SCREW SPEED VIA MB asm = MBWSSFLAG

}
//////////////////////////////////////////////////////
// CopyConfigDataToMB
//
///////////////////////////////////////////////////////

void CopyConfigDataToMB( void )
{
    CopyCalibrationDataToMB();      // copy calibration data to MB. COPYCDMB
    CopyVersionDataToMB();          // ; COPY VERS. NO. -> MB TABLE
    CopyEmailConfigDataToMB();  //
    CopyVacuumLoaderCalibrationParametersToMB();
}



// InitialiseCommsRecipe()
//
// Loads the Comms recipe structure with data from MB table
// Call after the setpoints have been stored in MB table.
//
//////////////////////////////////////////////////////
void InitialiseCommsRecipe( void )
{
   LoadPercentageTargetsFromComms();
   LoadTargetThruputsFromMB();
   LoadOrderInfoFromMB();

}



//////////////////////////////////////////////////////
// CopyCalibrationDataToMB  asm = COPYCDMB
//
// copy calibration data to MB.
//
// M.McKiernan                          07-10-2004
// First Pass
//
//////////////////////////////////////////////////////
void CopyCalibrationDataToMB( void )
{
    if(g_nProtocol == PROTOCOL_MODBUS)
    {
        CopyWeightConstantsToMB();              // CPYWTCMB

        CopyHopperConstantsToMB();              // CPYHPCMB

        CopyMeasurementAndControlParametersToMB();  // CPYMCPMB

        CopySpeedParametersToMB();              // CPYLSCMB

        CopySystemConfigurationToMB();      // CPYSYCMB
        CopySystemConfiguration3ToMB();

        CopyLiquidCalibrationToMB();            // CPYLIQCMB

        CopyComponentConfigurationToMB();       // CPYCCFGTMB

        // Set the new calibration data bit in the setpoint flags word. (asm MBNEWDATA)
        g_arrnMBTable[BATCH_SUMMARY_NEW_SETPOINTS] |= BATCH_CALIBRATION_CHANGED_MASK;

    }

}
//////////////////////////////////////////////////////
// CopyVersionDataToMB  asm = COPYVNMB
//
// copy VERSION data to MB.
//
// M.McKiernan                          07-10-2004
// First Pass
//
//////////////////////////////////////////////////////
void CopyVersionDataToMB( void )
{
    char cBuffer[20];
    int i;
    union CharsAndWord uTempRev;

    for(i=0; i<10; i++)
    {
        cBuffer[i] = 0;
    }
    sprintf( cBuffer, RevisionNumber );

    for(i=0; i<10; i++) // copy 10 bytes into MB table.
    {

        uTempRev.cValue[0] = cBuffer[i*2];
        uTempRev.cValue[1] = cBuffer[(i*2)+1];

        g_arrnMBTable[BATCH_VERSION_NO + i] = uTempRev.nValue;
    }
    g_arrnMBTable[PSUM_BLENDER_TYPE] = g_nBlenderType;

}/*
; COPYVNMB - COPY VERSION NO (SOFTWARE REVISION) TO MODBUS TABLE
COPYVNMB:
        LDX     #MBVERNUM       ; VERNUM IN MODBUS TABLE
        LDY     #VERNUM
        JSR     YCS_TOX         ; COPY ASCII STRING FROM Y -> X
        RTS
BATCH_VERSION_NO
*/
/*
; INITMBTB - INITIALISE THE MODBUS TABLE
INITMBTB LDE    #MBTABSIZ         ; OVERALL SIZE OF TABLE (BYTES)
        ASRE                      ; DIV. BY 2 TO GET WORDS
        LDX     #MBTABLE
        JSR     CLREWX
        JSR     COPYSPMB
        JSR     COPYKSMB  -  nb: in assembler this is an empty subroutine.
        JSR     COPYCDMB
        JSR     COPYVNMB        ; COPY VERS. NO. -> MB TABLE
        JSR     IMBTBFLGS       ; INITIALISE TABLE FLAGS
        CLR     MBWSSFLAG       ; FLAG TO SHOW DOWNLOAD OF SCREW SPEED VIA MB

        RTS

*/
/*
; COPYCLMB - COPY ALL CALIB. DATA TO MODBUS TABLE
COPYCDMB:
        TST     PROTOCOL
        BPL     CPYCDMBX        ; EXIT IF NOT MB PROTOCOL

        JSR     CPYWTCMB        ; WT. CONTSANTS
        JSR     CPYHPCMB        ; HOPPER CONSTANTS
        JSR     CPYMCPMB        ; MEAS & CONTROL PARAMETERS
        JSR     CPYLSCMB        ; LINE & SCREW SPEED CONSTANTS
        JSR     CPYSYCMB        ; SYSTEM CONFIGURATION INFO
        JSR     CPYLIQCMB       ; LIQUID CONSTANTS
        JSR     CPYCCFGTMB      ; COMPONENT CONFIGURATION.
        LDD     MBNEWDATA
        ORD     #$0002          ; SET B1 TO INDICATE NEW CALIB DATA
        STD     MBNEWDATA
CPYCDMBX:
        RTS


*/
//////////////////////////////////////////////////////
// InitialiseMBTableFlags
//
// Initialises MB table flags.
// M.McKiernan                          08-06-2004
// First Pass
// Sets up flags, for writeable area of MB table.
//       B15     $8000   - INDICATES WRITEABLE
//       B14     $4000   - RESET ORDER TOTALS (A TOGGLE STATUS)
//       B13     $2000   - WEIGH HOPPER CALIBRATION
//       B12     $1000   - TOGGLE STATUS, FILL, INC/DEC, A/M
//       B11     $0800   - OPEN / CLOSE VALVES.
//       B10     $0400   - TIME
//       B9      $0200   - CALIBRATION DATA, LIQ. ADD. CALIBRATION
//       B8      $0100   - CALIBRATION DATA, SYSTEM CONFIG. DATA
//       B7      $0080   - CAL. DATA, LINE & SCREW SPEED CONSTANTS
//       B6      $0040   - CAL. DATA, MEAS. PARAMETERS, AF1 ETC
//       B5      $0020   - CAL. DATA, HOPPER CONSTANTS, FILL TIME ETC
//       B4      $0010   - CAL. DATA, WEIGHT CONSTANTS, CON1,TARE & MBxCALC
//       B3      $0008   - COMPONENT CONFIGURATION
//       B2      $0004   - SETPOINT DATA, ORDER INFO
//       B1      $0002   - SETPOINT DATA, THROUGH-PUT DATA
//       B0      $0001   - SETPOINT DATA, TARGET %'s
//
//
/////////////////////////////////////////////////////

void InitialiseMBTableFlags( void )
{
    int i;

    for(i=0; i < MB_TABLE_WRITE_SIZE; i++)
    {
        g_arrnMBTableFlags[i] = MB_WRITEABLE_BIT;       // |  0x8000;           // MS bit indicates writeable.
    }

// setpoint percentages
    for(i=BATCH_SETPOINT_PERCENT_1; i <= BATCH_SETPOINT_REGRIND_COMPONENT; i++)
    {
        g_arrnMBTableFlags[i] = (MB_WRITEABLE_BIT | MB_WRITE_PERCENTAGES_BIT); // 0x8001;
    }

// setpoint percentages components 9 - 12

    for(i=BATCH_SETPOINT_PERCENT_9; i <= BATCH_SETPOINT_PERCENT_12; i++)
    {
        g_arrnMBTableFlags[i] = (MB_WRITEABLE_BIT | MB_WRITE_PERCENTAGES_BIT); // 0x8001;
    }

// Throughput setpoints
    for(i = BATCH_SETPOINT_WEIGHT_LENGTH; i <= BATCH_SETPOINT_WIDTH_OFFSET; i++)
    {
        g_arrnMBTableFlags[i] = (MB_WRITEABLE_BIT | MB_WRITE_THRUPUTS_BIT); // 0x8002;
    }

// Order Info
    for(i = BATCH_SETPOINT_FRONT_ROLL_LENGTH; i <= BATCH_SETPOINT_TRIM_FLAG; i++)
    {
        g_arrnMBTableFlags[i] = (MB_WRITEABLE_BIT | MB_WRITE_ORDER_INFO_BIT); // 0x8004;
    }

// SYSTEM CONFIGURATION. (--REVIEW-- There are other types of calib data mixed in here,
//                                  therefore important to write first, and let other types overwrite further down)
    for(i = BATCH_CALIBRATION_LINE_NUMBER; i <= BATCH_END_SYSTEM_CONFIG; i++)
    {
        g_arrnMBTableFlags[i] = (MB_WRITEABLE_BIT | MB_WRITE_SYSTEM_CONFIG_BIT); //  0x8100;
    }


// component config
    for(i = BATCH_CALIBRATION_COMP_CONFIG; i <= BATCH_CALIBRATION_COMP12_CONFIG; i++)
    {
        g_arrnMBTableFlags[i] = (MB_WRITEABLE_BIT | MB_WRITE_COMP_CONFIG_BIT); // 0x8008;
    }


// SYSTEM CONFIGURATION. block 2

    for(i = BATCH_START_SYSTEM_CONFIG2; i <= BATCH_END_SYSTEM_CONFIG2; i++)
    {
        g_arrnMBTableFlags[i] = (MB_WRITEABLE_BIT | MB_WRITE_SYSTEM_CONFIG_BIT); //  0x8100;
    }

    for(i = BATCH_START_SYSTEM_CONFIG3; i <= BATCH_END_SYSTEM_CONFIG3; i++)
    {
        g_arrnMBTableFlags[i] = (MB_WRITEABLE_BIT | MB_WRITE_SYSTEM_CONFIG3_BIT); //  0x8100;
    }


// Reset Totals flag
        g_arrnMBTableFlags[BATCH_SETPOINT_RESET_TOTALS] = (MB_WRITEABLE_BIT ); // 0xC000;


// Weight calibration data.
//      g_arrnMBTableFlags[BATCH_CALIBRATION_UNUSED6] = (MB_WRITEABLE_BIT | MB_WRITE_WEIGHT_CALIB_BIT); //  0x8010;
        g_arrnMBTableFlags[BATCH_CALIBRATION_CODE] = (MB_WRITEABLE_BIT | MB_WRITE_WEIGHT_CALIB_BIT);            // 0x8010;
//      g_arrnMBTableFlags[BATCH_CALIBRATION_TARE_COUNTS] = (MB_WRITEABLE_BIT | MB_WRITE_WEIGHT_CALIB_BIT);     // 0x8010;
//      g_arrnMBTableFlags[BATCH_CALIBRATION_TARE_COUNTS+1] = (MB_WRITEABLE_BIT | MB_WRITE_WEIGHT_CALIB_BIT); // 0x8010;

// Hopper calibration data.
        g_arrnMBTableFlags[BATCH_CALIBRATION_BATCH_SIZE] = (MB_WRITEABLE_BIT | MB_WRITE_HOPPER_CALIB_BIT); //  0x8020;
        g_arrnMBTableFlags[BATCH_CALIBRATION_MAX_THROUGHPUT] = (MB_WRITEABLE_BIT | MB_WRITE_HOPPER_CALIB_BIT); //  0x8020;


// Measurement parameters.
    for(i = BATCH_CALIBRATION_CONTROL_AVERAGE; i <= BATCH_CALIBRATION_MIXING_TIME; i++)
    {
        g_arrnMBTableFlags[i] = (MB_WRITEABLE_BIT | MB_WRITE_MEAS_PARAMETERS_BIT); //  0x8040;
    }

// line and screw speed parameters  (nb were 6 in assembler, but only 4 defined)
    for(i = BATCH_CALIBRATION_LINE_SPEED_CONST; i <= BATCH_CALIBRATION_MAX_SCREW_SPEED; i++)
    {
        g_arrnMBTableFlags[i] = (MB_WRITEABLE_BIT | MB_WRITE_SPEED_PARAMETERS_BIT); //  0x8080;
    }


// liquid Additive.
    for(i = BATCH_CALIBRATION_LA_CONSTANT; i <= BATCH_CALIBRATION_LA_TARE; i++)
    {
        g_arrnMBTableFlags[i] = (MB_WRITEABLE_BIT | MB_WRITE_LIQUID_CALIB_BIT); //  0x8200;
    }

// Time
    for(i = BATCH_TIME_MIN_SEC; i <= BATCH_TIME_YEAR; i++)
    {
        g_arrnMBTableFlags[i] = (MB_WRITEABLE_BIT | MB_WRITE_TIME_BIT); //  0x8400;
    }

// Toggle status commands.
    for(i = BATCH_START_TOGGLE_STATUS_COMMANDS; i <= BATCH_END_TOGGLE_STATUS_COMMANDS; i++)
    {
        g_arrnMBTableFlags[i] = (MB_WRITEABLE_BIT | MB_TOGGLE_STATUS_BIT); //  0x9000;
    }

   for(i = BATCH_START_TOGGLE_STATUS_COMMANDS2; i <= BATCH_END_TOGGLE_STATUS_COMMANDS2; i++)  // sbb--todo--
    {
        g_arrnMBTableFlags[i] = (MB_WRITEABLE_BIT | MB_TOGGLE_STATUS_BIT); //  0x9000;
    }


// Calibration weight.
        g_arrnMBTableFlags[BATCH_CALIBRATION_CALIB_WEIGHT] = (MB_WRITEABLE_BIT | MB_WRITE_CALIBRATION_WEIGHT_BIT); //  0xA000;

// Email config commands.
    for(i = EMAIL_CONFIG_START; i <= EMAIL_CONFIG_END; i++)
    {
        g_arrnMBTableFlags[i] = (MB_WRITEABLE_BIT | MB_WRITE_EMAIL_CONFIG_BIT); //  email
    }



    g_nMBWriteOrFlag = 0;                           // clear the OR flags.
    g_nMBWriteOrErrorFlag = 0;

}

//////////////////////////////////////////////////////
// CheckMBValidData                     (from asm = CHMBVDATA)
//
// Checks that the data which has been written to the MB table is valid.
//
// M.McKiernan                          09-06-2004
// First Pass
// EXIT: Returns error code (0 if no error)
//////////////////////////////////////////////////////
char CheckMBValidData( void )
{
    int nTotal = 0;
    unsigned int nRegrindComp,nTemp;
    BOOL bError = FALSE;
    BOOL bSingleWordPercentageWrite = FALSE;
    unsigned int i,j;
    if(g_nMBWriteOrFlagTemp)                    // only apply checks if data has been written.
    {
        if(g_nMBWriteOrFlagTemp & MB_WRITE_PERCENTAGES_BIT) // B0 SET => CHECK TARGET %'S VALID
        {
            if(g_CalibrationData.m_bSingleWordWrite == FALSE)       // dont do 100% check if singlewordwrite enabled (set)
            {
                // JSR CHMB100T
//              CheckMBPercentagesTotal100( );
                nTemp = (g_arrnMBTable[BATCH_SETPOINT_REGRIND_COMPONENT] & 0x00FF);      // the regrind component no. is in the L.S.byte.
                if(nTemp <=  g_CalibrationData.m_nComponents)
                {
                     nRegrindComp = nTemp;
                }
                else
                {
                     nRegrindComp = 0;
                }

                if(g_CalibrationData.m_nComponents <= 8)
                {
                    for(i = BATCH_SETPOINT_PERCENT_1, j=0; i <= BATCH_SETPOINT_PERCENT_8 && j < g_CalibrationData.m_nComponents; i++, j++ )    //--review-- leave out additive for now
                    {
                        if( i != (nRegrindComp - 1))        // NB: i is 0 based, component no.s start at 1.  Dont sum the regrind component.
                        {
                            nTotal += g_arrnMBTable[i];
                       }
                    }
                }
                else  // > 8
                {
                    for(i = BATCH_SETPOINT_PERCENT_1; i <= BATCH_SETPOINT_PERCENT_8; i++, j++ )   //
                    {
                        if( i != (nRegrindComp - 1))        // NB: i is 0 based, component no.s start at 1.  Dont sum the regrind component.
                        {
                            nTotal += g_arrnMBTable[i];
                        }
                    }

                    for(i = BATCH_SETPOINT_PERCENT_9, j=0; i <= BATCH_SETPOINT_PERCENT_12 && j < g_CalibrationData.m_nComponents - 8; i++, j++ )
                        {
                            if( i != (nRegrindComp - 1))        // NB: i is 0 based, component no.s start at 1.  Dont sum the regrind component.
                            {
                                nTotal += g_arrnMBTable[i];
                             }
                        }
                }
                if(nTotal > 9995 && nTotal < 10005) // %'s to 2 decimal places, therefore 100% = 10,000. accept .01 error.
                {           //  i.e. total = 100%
                    // Remove % alarm from alarm table (if already there)
                    if( g_arrnMBTable[BATCH_SUMMARY_PROCESS_ALARMS] & PERALBIT ) // percentage alarm bit set???
                    {  //
                        RemoveAlarmTable( PERALARM,  0 );       // indicate alarm cleared, Component no = 0 i.e. not component alarm.
                    }
                }
                else
                {
                    bError = TRUE;
                    // Reload %'s
                    CopyTargetPercentagesToMB();        // copy %'s back into MB table.
                    // Put % alarm in alarm table (if not already there)
                    if( (g_arrnMBTable[BATCH_SUMMARY_PROCESS_ALARMS] & PERALBIT) == 0 ) // percentage alarm bit not set???
                    {  //
                        PutAlarmTable( PERALARM,  0 );      // indicate alarm, Component no = 0 i.e. not component alarm.
                    }

                }  // end of 100% check found in error.


            }
            else                // singlewordwrite enabled.
            {
                bSingleWordPercentageWrite = TRUE;
                g_nLoadNewSetpointsLag = LAG_TIME_FOR_LOADING_MB_SETPOINTS;     // NB: lag period in 10ths of second.
            }

        }

        if(g_nMBWriteOrFlagTemp & MB_WRITE_THRUPUTS_BIT)    // B1 SET => CHECK Thruputs VALID
        {
            //routine to check for valid throughputs
        }
        if(g_nMBWriteOrFlagTemp & MB_WRITE_ORDER_INFO_BIT)  // B2 SET => CHECK ORDER INFO DATA
        {
            //routine to check for valid order info

        }
        if(g_nMBWriteOrFlagTemp & MB_WRITE_COMP_CONFIG_BIT) // B3 SET =>  Check comp. config.
        {
            //routine to check for valid component configuration

        }
        // add other checks for the other writeable MB parameters.

    }

    if(bError == TRUE)
    {
        g_nMBWriteOrErrorFlag = g_nMBWriteOrFlagTemp;           // sTORE TEMP 'OR' FLAG INto WRITE Error FLAG
        return(ILLEGAL_MB_DATA_VALUE);                              // return "illegal data value code"  to calling routine to indicate problem
    }
    else   // i.e. no error, data valid
    {
        //if(fdTelnet>0)
        //iprintf("\n g_nMBWriteOrFlagTemp %d g_nMBWriteOrFlag is ok %d",g_nMBWriteOrFlagTemp,g_nMBWriteOrFlag);
        if(!bSingleWordPercentageWrite||g_bPanelWriteHasHappened)
        {
            g_nMBWriteOrFlag = g_nMBWriteOrFlagTemp;            // sTORE TEMP 'OR' FLAG IN WRITE FLAG
            g_nMBWriteOrFlagTemp = 0;
        }
        return(0);                                                  // return 0 to calling routine to indicate OK.
    }
}


//////////////////////////////////////////////////////
// DecrementMBLags                      (from asm = MBLAGS)
//
// ROUTINE TO DECREMENT LAGS USED IN MODBUS - CALLED ON 10Hz IRQ
//
// M.McKiernan                          10-06-2004
// First Pass
//
//////////////////////////////////////////////////////
void DecrementMBLags( void )
{
    if(g_nLoadNewSetpointsLag)
    {
        g_nLoadNewSetpointsLag--;
        if(g_nLoadNewSetpointsLag == 0)
        {
            // IF TIME-OUT LOAD CURRENT SPT'S BACK TO MB
            CopySetpointsToMB();   /* asm COPYSPMB   */
        }

    }

}
//////////////////////////////////////////////////////
// CopyTargetPerctagesToMB                      (from asm = CPYPCTSMB)
//
// COPY TARGET %'s INTO MODBUS TABLE SETPOINT AREA - Note %'s loaded from temp recipe, g_TempRecipe, structure.
//
// M.McKiernan                          10-06-2004
// First Pass
//
//////////////////////////////////////////////////////
void CopyTargetPercentagesToMB( void )
{
    unsigned int i,j;
    float fPercentage,fDensity;
    union CharsAndWord uTempCI;


    for(i = BATCH_SETPOINT_PERCENT_1; i <= BATCH_SETPOINT_ADDITIVE; i++ )
    {
        g_arrnMBTable[i] = 0;       // Clear all %'s first, will cause unused to become 0.
    }

    for(i = BATCH_SETPOINT_PERCENT_9; i <= BATCH_SETPOINT_PERCENT_12; i++ )
    {
        g_arrnMBTable[i] = 0;       // Clear all %'s first, will cause unused to become 0.
    }

    if(g_CalibrationData.m_nComponents <= 8)
    {
        for(i = BATCH_SETPOINT_PERCENT_1, j=0; i <= BATCH_SETPOINT_ADDITIVE && j < g_CalibrationData.m_nComponents; i++, j++ )
        {
            if(g_CalibrationData.m_bHighLoRegrindOption)
            {
                fPercentage =   g_CurrentRecipe.m_fPercentage[j]; //nbb--todo--implement this better
            }
            else
            {
                fPercentage =   g_TempRecipe.m_fPercentage[j];
            }
            fPercentage += 0.005;       // for rounding to nearest 2nd decimal digit.
            g_arrnMBTable[i] = (int)(fPercentage * 100);        // NB: Percentages are to 2DP in MB table. (9999 = 99.99%)
        }
     }
    else
    {
        for(i = BATCH_SETPOINT_PERCENT_1, j=0; i <= BATCH_SETPOINT_ADDITIVE && j < 8; i++, j++ )
        {
            if(g_CalibrationData.m_bHighLoRegrindOption)
            {
                fPercentage =   g_CurrentRecipe.m_fPercentage[j]; //nbb--todo--implement this better
            }
            else
            {
                fPercentage =   g_TempRecipe.m_fPercentage[j];
            }
            fPercentage += 0.005;       // for rounding to nearest 2nd decimal digit.
            g_arrnMBTable[i] = (int)(fPercentage * 100);        // NB: Percentages are to 2DP in MB table. (9999 = 99.99%)
        }
        for(i = BATCH_SETPOINT_PERCENT_9, j=8; i <= BATCH_SETPOINT_PERCENT_12; i++, j++ )
        {
            if(g_CalibrationData.m_bHighLoRegrindOption)
            {
                fPercentage =   g_CurrentRecipe.m_fPercentage[j]; //nbb--todo--implement this better
            }
            else
            {
                fPercentage =   g_TempRecipe.m_fPercentage[j];
            }
            fPercentage += 0.005;       // for rounding to nearest 2nd decimal digit.
            g_arrnMBTable[i] = (int)(fPercentage * 100);        // NB: Percentages are to 2DP in MB table. (9999 = 99.99%)
        }
       StoreMBSetpointHistoryArray();        // store current values into history array.
    }


// Regrind component.
    uTempCI.nValue = g_arrnMBTable[BATCH_SETPOINT_REGRIND_COMPONENT];
    uTempCI.cValue[1] = (char)g_TempRecipe.m_nRegrindComponent;         // put regr. comp. no. in L.s. byte
    g_arrnMBTable[BATCH_SETPOINT_REGRIND_COMPONENT] = (WORD)g_CurrentRecipe.m_nRegrindComponent;  // regrind component no. is in the L.S.byte.;//nbb--todo-- check this

// Max Regrind Component%
    fPercentage =   g_TempRecipe.m_fPercentageHighRegrind;
    fPercentage += 0.005;       // for rounding to nearest 2nd decimal digit.
    g_arrnMBTable[BATCH_SETPOINT_HIGH_REGRIND] = (int)(fPercentage * 100);        // NB: Percentages are to 2DP in MB table. (9999 = 99.99%)

// Liquid %
    if(g_CalibrationData.m_bLiquidEnabled)
    {
        fPercentage =   g_TempRecipe.m_fPercentageLiquid;
        fPercentage += 0.005;       // for rounding to nearest 2nd decimal digit.
        g_arrnMBTable[BATCH_SETPOINT_LIQUID] = (int)(fPercentage * 100);        // Liquid %NB: Percentages are to 2DP in MB table. (9999 = 99.99%)
    }
    else
        g_arrnMBTable[BATCH_SETPOINT_LIQUID] = 0;   //not enabled, set to 0.

    for(i = BATCH_SETPOINT_DENSITY_1, j=0; i <= BATCH_SETPOINT_DENSITY_12 && j < g_CalibrationData.m_nComponents; i++, j++ )
    {
        fDensity =   g_TempRecipe.m_fDensity[j];
        fDensity += 0.0005;       // for rounding to nearest 2nd decimal digit.
        g_arrnMBTable[i] = (int)(fDensity * 1000);        //
    }

}
//////////////////////////////////////////////////////
// CopyTargetThruputsToMB                       (from asm = CPYTPTSMB)
//
// COPY TARGET Throughputs INTO MODBUS TABLE SETPOINT AREA - (Tpt, wt/len, wt/area, line speed, widths)
// Note data loaded from temp recipe, g_TempRecipe, structure.
//
// M.McKiernan                          11-06-2004
// First Pass
//
//////////////////////////////////////////////////////
void CopyTargetThruputsToMB( void )
{
    union WordAndDWord uValue;
    int i;
    float fValue;

    for(i = BATCH_SETPOINT_WEIGHT_LENGTH; i <= BATCH_SETPOINT_TRIMMED_WIDTH; i++ )
    {
        g_arrnMBTable[i] = 0;       // Clear all thruputs first, will cause unused to become 0.
    }
// wt/len
        fValue =    g_TempRecipe.m_fTotalWeightLength;
        fValue *=    g_fWeightPerLengthConversionFactor;

        fValue += 0.005;        // for rounding to nearest 2nd decimal digit.
        uValue.lValue = (long)(fValue * 100.0f);
        g_arrnMBTable[BATCH_SETPOINT_WEIGHT_LENGTH] = uValue.nValue[0];     // M.s.Byte.
        g_arrnMBTable[BATCH_SETPOINT_WEIGHT_LENGTH + 1] = uValue.nValue[1];     // L.s.Byte.

// weight/area
        fValue =    g_TempRecipe.m_fTotalWeightArea;
        fValue += 0.005;                                                                        // for rounding to nearest 2nd decimal digit.
        g_arrnMBTable[BATCH_SETPOINT_WEIGHT_AREA] = (int)(fValue * 100);        // wt/area is to 2dp

// throughput
        fValue =    g_TempRecipe.m_fTotalThroughput;
        if(g_fWeightConversionFactor != 1.0f)
        fValue *=    g_fWeightConversionFactor;
        fValue += 0.05;                                                                         // for rounding to nearest  decimal digit.
        g_arrnMBTable[BATCH_SETPOINT_TOTAL_THROUGHPUT] = (int)(fValue * 10);        // Throughput is to 1dp

// Linespeed
        fValue =    g_TempRecipe.m_fLineSpeed;
        fValue *=   g_fLineSpeedConversionFactor;

        fValue += 0.05;                                                                     // for rounding to nearest  decimal digit.
        g_arrnMBTable[BATCH_SETPOINT_LINE_SPEED] = (int)(fValue * 10);          // Linespeed is to 1dp

// Width
        fValue =    g_TempRecipe.m_fDesiredWidth;
// MMK - 22.2.2008....in imperial mode...width is in inches to 2dp in Modbus.

      if(g_fWidthConversionFactor != 1.0f)
      {
    	  fValue *= g_fWidthConversionFactor;
    	  fValue *= 100.0f;      // when not mm, inches to 2dp in modbus.
      }
      fValue += 0.5;                                                                           // for rounding to nearest  decimal digit.
      g_arrnMBTable[BATCH_SETPOINT_WIDTH] = (int)(fValue);                        // Width is in mm.


        fValue =    g_TempRecipe.m_fDesiredWidth;
        if(g_fWidthConversionFactor != 1.0f)
        {
         fValue *= g_fWidthConversionFactor;  //nbb--todo-- check this again
 		fValue += (PSUM_I_ROUND_DECIMAL_PLACES);                                                                          // for rounding to nearest  decimal digit.
 		fValue *= (PSUM_I_DECIMAL_PLACES_MULTIPLICATION);      // when not mm, inches to 2dp in modbus.
        }
        else
        {
    	    fValue += (PSUM_M_ROUND_DECIMAL_PLACES);                                                                          // for rounding to nearest  decimal digit.
    		fValue *= (PSUM_M_DECIMAL_PLACES_MULTIPLICATION);      // 1dp
        }
        g_arrnMBTable[WIDTH_COMMAND_WIDTH_SETPOINT] = (int)(fValue);                        // Width is in mm.


// Trimmed Width
        fValue =    g_TempRecipe.m_fTrimmedWidth;
        if(g_fWidthConversionFactor != 1.0f)
        {
         fValue *= g_fWidthConversionFactor;  //nbb--todo-- check this again
         fValue *= 100.0f;      // when not mm, inches to 2dp in modbus.
        }
//        fValue *=    g_fWidthConversionFactor;
        fValue += 0.5;                                                                          // for rounding to nearest  decimal digit.
        g_arrnMBTable[BATCH_SETPOINT_TRIMMED_WIDTH] = (int)(fValue);                        // Width is in mm.

        // width offset
//        fValue =    g_TempRecipe.m_fDesiredWidthOffset;
//        if(g_fWidthConversionFactor != 1.0f)
//        {
//       	fValue *= g_fWidthConversionFactor;  //nbb--todo-- check this again
//        	fValue *= 100.0f;      // when not mm, inches to 2dp in modbus.
//        }
//        fValue += 0.5; //nbb--width--todo--check this
//        g_arrnMBTable[BATCH_SETPOINT_WIDTH_OFFSET] = (int)(fValue);                        // Width is in mm.




// Microns
        fValue =    g_TempRecipe.m_fFilmThickness;
        fValue += 0.1;                                                                        // for rounding to nearest 2nd decimal digit.
        g_arrnMBTable[BATCH_SETPOINT_MICRONS] = (int) fValue;        // wt/area is to 2dp
        StoreMBSetpointHistoryArray();        // store current values into history array.

}
//////////////////////////////////////////////////////
// CopyOrderInfoToMB                    (from asm = CPYOINFMB)
//
// COPY Order Information INTO MODBUS TABLE SETPOINT AREA - ( )
// Note data loaded from temp recipe, g_TempRecipe, structure.
//
// M.McKiernan                          11-06-2004
// First Pass
//
//////////////////////////////////////////////////////
void CopyOrderInfoToMB( void )
{
    union WordAndDWord uValue;
    union CharsAndWord uTempCI;
    int i;
    float fValue;

    for(i = BATCH_SETPOINT_FRONT_ROLL_LENGTH; i <= BATCH_SETPOINT_TRIM_FLAG; i++ )
    {
        g_arrnMBTable[i] = 0;       // Clear all thruputs first, will cause unused to become 0.
    }
// Front Roll Length
        fValue =    g_TempRecipe.m_fFrontRollLength;
        fValue *=    g_fLengthConversionFactor;

        fValue += 0.5;                                                                          // for rounding to nearest  decimal digit.
        g_arrnMBTable[BATCH_SETPOINT_FRONT_ROLL_LENGTH] = (int)(fValue);        // Length is in m.

// Back Roll Length
        fValue =    g_TempRecipe.m_fBackRollLength;
        fValue *=    g_fLengthConversionFactor;
        fValue += 0.5;                                                                          // for rounding to nearest  decimal digit.
        g_arrnMBTable[BATCH_SETPOINT_BACK_ROLL_LENGTH] = (int)(fValue);     // Length is in m.

// Order Length (long)
        fValue =    g_TempRecipe.m_fOrderLength;
        fValue *=    g_fLengthConversionFactor;
        fValue += 0.5;                                                                          // for rounding to nearest  integer.
        uValue.lValue = (long)fValue;                                                       // Length is in m.
        g_arrnMBTable[BATCH_SETPOINT_ORDER_LENGTH] = uValue.nValue[0];          // M.s.Byte. -
        g_arrnMBTable[BATCH_SETPOINT_ORDER_LENGTH + 1] = uValue.nValue[1];  // L.s.Byte.

// Order Weight (long)
        fValue =    g_TempRecipe.m_fOrderWeight;
        if(g_fWeightConversionFactor != 1.0f)
        fValue *=    g_fWeightConversionFactor;
        fValue += 0.5;                                                                          // for rounding to nearest integer
        uValue.lValue = (long)fValue;                                                       // Wt. is in kg.
        g_arrnMBTable[BATCH_SETPOINT_ORDER_WEIGHT] = uValue.nValue[0];          // M.s.Byte. -
        g_arrnMBTable[BATCH_SETPOINT_ORDER_WEIGHT + 1] = uValue.nValue[1];  // L.s.Byte.

// front slit reels/rolls
        g_arrnMBTable[BATCH_SETPOINT_FRONT_SLIT_REELS] = g_TempRecipe.m_nFrontSlitReels;        // reels is already an integer

// Back slit reels/rolls
        g_arrnMBTable[BATCH_SETPOINT_BACK_SLIT_REELS] = g_TempRecipe.m_nBackSlitReels;      // reels is already an integer

// Sheet/Tube flag
        if(g_TempRecipe.m_bSheetTube)
            g_arrnMBTable[BATCH_SETPOINT_SHEET_TUBE] = 1;                   //
        else
            g_arrnMBTable[BATCH_SETPOINT_SHEET_TUBE] = 0;                   //

// Trim Feedback flag
        if(g_TempRecipe.m_bTrimFeed)
            g_arrnMBTable[BATCH_SETPOINT_TRIM_FLAG] = 1;                    //
        else
            g_arrnMBTable[BATCH_SETPOINT_TRIM_FLAG] = 0;                    //

// Order Number, in ASCII in MB, and in setpoint file.
        for(i=0; i<5; i++)                  // Fill 5 MB registers, i.e. 10 ASCII chars.
        {
            uTempCI.cValue[1] = g_TempRecipe.m_arrOrderNumber[i*2];
            uTempCI.cValue[0] = g_TempRecipe.m_arrOrderNumber[i*2 + 1];
            g_arrnMBTable[BATCH_SETPOINT_ORDER_NUMBER + i] = uTempCI.nValue;        // Modbus has two ascii chars in each register.
        }


// customer code, in ASCII in MB, and in setpoint file.
        for(i=0; i<5; i++)                  // Fill 5 MB registers, i.e. 10 ASCII chars.
        {
            uTempCI.cValue[0] = g_TempRecipe.m_arrCustomerCode[i*2];
            uTempCI.cValue[1] = g_TempRecipe.m_arrCustomerCode[i*2 + 1];
            g_arrnMBTable[BATCH_SETPOINT_CUSTOMER_CODE + i] = uTempCI.nValue;       // Modbus has two ascii chars in each register.
        }

// --review-- if material code is required.
// material code, in ASCII in MB, and in setpoint file.
//        for(i = 0; i < 5; i++)                  // Fill 5 MB registers, i.e. 10 ASCII chars.
//        {
//            uTempCI.cValue[0] = g_TempRecipe.m_arrMaterialCode[i*2];
//            uTempCI.cValue[1] = g_TempRecipe.m_arrMaterialCode[i*2 + 1];
//            g_arrnMBTable[BATCH_SETPOINT_MATERIAL_CODE + i] = uTempCI.nValue;       // Modbus has two ascii chars in each register.
//        }


// operator no.
        g_arrnMBTable[BATCH_SETPOINT_OPERATOR_NO] = g_TempRecipe.m_nOperator_No;      // operator no transferred

// order no.
        g_arrnMBTable[BATCH_SETPOINT_ORDER_NO_INTEGER] = g_TempRecipe.m_nOrder_No;    // order no


// recipe no

        g_arrnMBTable[BATCH_RECIPE_NO] = g_TempRecipe.m_nRecipe_No;      // recipe no

// extruder no.

        g_arrnMBTable[BATCH_EXTRUDER_NO] = g_TempRecipe.m_nExtruder_No; // Extruder no
        StoreMBSetpointHistoryArray();        // store current values into history array.

}


//////////////////////////////////////////////////////
// CopySetpointsToMB                    (from asm = COPYSPMB)
//
// COPY all Setpoint data INTO MODBUS TABLE SETPOINT AREA
// Note data loaded from temp recipe, g_TempRecipe, structure.
//
// M.McKiernan                          11-06-2004
// First Pass
//
//////////////////////////////////////////////////////
void CopySetpointsToMB( void )
{
    if(g_nProtocol == PROTOCOL_MODBUS)
    {
        CopyTargetPercentagesToMB();
        CopyTargetThruputsToMB();
        CopyOrderInfoToMB();
        //nbb--todo-- put back CopyICSRecipeMBData();// copy isc material codes to mb
        // regrind component in target %'s
        g_arrnMBTable[BATCH_SUMMARY_NEW_SETPOINTS] |= BATCH_SETPOINTS_CHANGED_MASK;     // Set the new percentages bit in the setpoint flags word.
    }

}

/*  from assembler.
#define BATCH_SUMMARY_NEW_SETPOINTS             421
// New setpoints masks
#define BATCH_SETPOINTS_CHANGED_MASK            0x0001
*/

//////////////////////////////////////////////////////
// LoadTargetPerctagesFromMB                    (from asm = LDMBPCTS)
//
// Load TARGET %'s From MODBUS TABLE SETPOINT AREA - Note %'s loaded into temporary recipe, g_CommsRecipe, structure.
//
// M.McKiernan                          10-06-2004
// First Pass
//
//////////////////////////////////////////////////////
void LoadTargetPercentagesFromMB( void )
{
    if(g_CalibrationData.m_nBlenderMode == MODE_SINGLERECIPEMODE)  // single recipe mode ?
    {
        g_bRecipeWaiting = TRUE;
        SetInProgressStatus();              // set in progress bit // asm SETVACUUMSTATUS
        CopyBBAuxToMB();                    // COPYBBAUXMB
    }
    else
    {
        LoadPercentageTargetsFromComms();
    }
}



//////////////////////////////////////////////////////
// LoadPercentageTargetsFromComms              (from asm = LOADPERTARGETSFROMCOMMS)
//
//
// P.Smith                          15-09-2005
// Load percentage targets from commsm make this a function on its own.
//
//////////////////////////////////////////////////////
void LoadPercentageTargetsFromComms( void )
{
    unsigned int i,j,nTemp;
    float fPercentage,fDensity;


// Clear all %'s first,

        for(i = 0; i < MAX_COMPONENTS; i++ )
        {
            g_CommsRecipe.m_fPercentage[i] = 0.0f;
        }
        g_CommsRecipe.m_fPercentageLiquid = 0.0f;
    //  g_CommsRecipe.m_fPercentageRegrind = 0.0f;
        g_CommsRecipe.m_fPercentageHighRegrind = 0.0f;
        g_CommsRecipe.m_fPercentageAdditive = 0.0f;

        g_CommsRecipe.m_nRegrindComponent = 0;

        if(g_CalibrationData.m_nComponents <= 8)
        {
            for(i = BATCH_SETPOINT_PERCENT_1, j=0; i <= BATCH_SETPOINT_ADDITIVE && j < g_CalibrationData.m_nComponents; i++, j++ )
            {
                fPercentage = (float)g_arrnMBTable[i];              //nb MB table has %'s to 2dp.
                fPercentage /= 100.0f;                                  // div by 100.
                g_CommsRecipe.m_fPercentage[j] = fPercentage;
  //            printf("\n per# %d is %2.1f",j,fPercentage);  //nbb--testonly--
            }
        }
        else
        {
            for(i = BATCH_SETPOINT_PERCENT_1, j=0; i <= BATCH_SETPOINT_ADDITIVE && j < 8; i++, j++ )
            {
                fPercentage = (float)g_arrnMBTable[i];              //nb MB table has %'s to 2dp.
                fPercentage /= 100.0f;                                  // div by 100.
                g_CommsRecipe.m_fPercentage[j] = fPercentage;
            }

            for(i = BATCH_SETPOINT_PERCENT_9, j=8; i <= BATCH_SETPOINT_PERCENT_12; i++, j++ )    // --review-- copy all component data
            {
                fPercentage = (float)g_arrnMBTable[i];              //nb MB table has %'s to 2dp.
                fPercentage /= 100.0f;                                  // div by 100.
                g_CommsRecipe.m_fPercentage[j] = fPercentage;
            }
        }


// Liquid %
        if(g_CalibrationData.m_bLiquidEnabled)
        {
            fPercentage = (float)g_arrnMBTable[BATCH_SETPOINT_LIQUID];              //nb MB table has %'s to 2dp.
            fPercentage /= 100.0f;                                                          // div by 100.
            g_CommsRecipe.m_fPercentageLiquid = fPercentage;
        }

        fPercentage = (float)g_arrnMBTable[BATCH_SETPOINT_HIGH_REGRIND];                //nb MB table has %'s to 2dp.
        fPercentage /= 100.0f;                                                              // div by 100.
        g_CommsRecipe.m_fPercentageHighRegrind = fPercentage;


    // Regrind component no.
        nTemp = g_arrnMBTable[BATCH_SETPOINT_REGRIND_COMPONENT] & 0x00FF;
        if(nTemp <= g_CalibrationData.m_nComponents)
        {
            g_CommsRecipe.m_nRegrindComponent = nTemp;  // the regrind component no. is in the L.S.byte.;
        }
        else
        {
            g_CommsRecipe.m_nRegrindComponent = 0;
        }
   // extruder no.

        g_CommsRecipe.m_nExtruder_No = g_arrnMBTable[BATCH_EXTRUDER_NO];    // extruder no

    // --review-- if this is done correctly.
        for(i = BATCH_SETPOINT_DENSITY_1, j=0; i <= BATCH_SETPOINT_DENSITY_12 && j < g_CalibrationData.m_nComponents; i++, j++ )
        {
            fDensity = (float)g_arrnMBTable[i];              //nb MB table has %'s to 2dp.
            fDensity /= 1000.0f;                                  // div by 100.
            g_CommsRecipe.m_fDensity[j] = fDensity;
        }


        if(g_bCycleIndicate)
        {
            g_bChangingOrder = TRUE;
            if(g_arrnMBTable[BATCH_SETPOINT_RESET_TOTALS] & MB_START_CLEAN_BIT)
            {
                SetStartCleanMBBit();
                ResetCleanFinishMBBit();
            }
        }
}

//////////////////////////////////////////////////////
// LoadTargetThruputsFromMB                     (from asm = LDMBTPTS)
//
// Load Target Thruputs from MODBUS TABLE SETPOINT AREA - Note Thruputs loaded into temp storage, g_CommsRecipe, structure.
//
// M.McKiernan                          15-06-2004
// First Pass
//
//////////////////////////////////////////////////////
void LoadTargetThruputsFromMB( void )
{
    union WordAndDWord uValue;

    float fValue;

// wt/len (Long)

        uValue.nValue[0] = g_arrnMBTable[BATCH_SETPOINT_WEIGHT_LENGTH];     // M.s. word
        uValue.nValue[1] = g_arrnMBTable[BATCH_SETPOINT_WEIGHT_LENGTH + 1];     // L.s.word.
        fValue = (float)uValue.lValue;
        if(g_fWeightPerLengthConversionFactor != 1.0f)
        fValue /= g_fWeightPerLengthConversionFactor;
        g_CommsRecipe.m_fTotalWeightLength = fValue / 100.0f;                   // Wt/length is to 2dp in MB.


// weight/area
        fValue =    (float)g_arrnMBTable[BATCH_SETPOINT_WEIGHT_AREA];
        if(g_fWeightPerAreaConversionFactor != 1.0f)
        fValue /= g_fWeightPerAreaConversionFactor;
        g_CommsRecipe.m_fTotalWeightArea = fValue / 100.0f;                         // wt/area is to 2dp

// throughput
        fValue =    (float)g_arrnMBTable[BATCH_SETPOINT_TOTAL_THROUGHPUT];
        if(g_fWeightConversionFactor != 1.0f)
        fValue /= g_fWeightConversionFactor;
        g_CommsRecipe.m_fTotalThroughput = fValue / 10.0f;                                  // Throughput is to 1dp in MB.

// Linespeed
        fValue =    (float)g_arrnMBTable[BATCH_SETPOINT_LINE_SPEED];
        if(g_fLineSpeedConversionFactor != 1.0f)
        fValue /= g_fLineSpeedConversionFactor;
        g_CommsRecipe.m_fLineSpeed = fValue / 10.0f;                                        // Linespeed is to 1dp in MB.

// Width
        fValue =    (float)g_arrnMBTable[BATCH_SETPOINT_WIDTH];
        if(g_fWidthConversionFactor != 1.0f)
        fValue = (fValue / 100.0f) / g_fWidthConversionFactor;  //nbb--todo-- check this again
        g_CommsRecipe.m_fDesiredWidth = fValue; // Width is in mm.

// Width offset for ultrasonic
//        SHORT sOffset;
//        sOffset = (SHORT)g_arrnMBTable[BATCH_SETPOINT_WIDTH_OFFSET];
//        fValue =    (float)sOffset;
//        if(g_fWidthConversionFactor != 1.0f)
//        fValue = (fValue / 100.0f) / g_fWidthConversionFactor;  //nbb--todo-- check this again
//        g_CommsRecipe.m_fDesiredWidthOffset = fValue; // Width is in mm.

// Trimmed Width
        fValue =    (float)g_arrnMBTable[BATCH_SETPOINT_TRIMMED_WIDTH];
        if(g_fWidthConversionFactor != 1.0f)
        fValue /= g_fWidthConversionFactor;
        g_CommsRecipe.m_fTrimmedWidth = fValue; // Trimmed Width is in mm.
// microns
       fValue =    (float)g_arrnMBTable[BATCH_SETPOINT_MICRONS];
       g_CommsRecipe.m_fFilmThickness = fValue;                   // microns.

//        SetPointChange called when the recipe is transferred in the progloop.c
}





//////////////////////////////////////////////////////
// LoadOrderInfoFromMB                      (from asm = LDMBOINFO)
//
// Load Order Info from MODBUS TABLE SETPOINT AREA - Note Info loaded into temp storage, g_CommsRecipe, structure.
//
// M.McKiernan                          15-06-2004
// First Pass
//
//////////////////////////////////////////////////////
void LoadOrderInfoFromMB( void )
{
    union WordAndDWord uValue;
    union CharsAndWord uTempCI;
    int i,j,nCharNo;
    BOOL bInvalidChar = FALSE;
    char  cTemp;
    char  arrOrderNumber[10];

// roll lengths
        g_CommsRecipe.m_fFrontRollLength = (float)g_arrnMBTable[BATCH_SETPOINT_FRONT_ROLL_LENGTH];      // Length is in m.
        g_CommsRecipe.m_fBackRollLength = (float)g_arrnMBTable[BATCH_SETPOINT_BACK_ROLL_LENGTH];        // Length is in m.

// Order Length (long)
        uValue.nValue[0] = g_arrnMBTable[BATCH_SETPOINT_ORDER_LENGTH];          // M.s.WORD. -
        uValue.nValue[1] = g_arrnMBTable[BATCH_SETPOINT_ORDER_LENGTH + 1];  // L.s.word.
        g_CommsRecipe.m_fOrderLength = (float)uValue.lValue;

// Order Weight (long)
        uValue.nValue[0] = g_arrnMBTable[BATCH_SETPOINT_ORDER_WEIGHT];          // M.s.WORD. -
        uValue.nValue[1] = g_arrnMBTable[BATCH_SETPOINT_ORDER_WEIGHT + 1];  // L.s.word.
        g_CommsRecipe.m_fOrderWeight = (float)uValue.lValue;

// front slit reels/rolls
        g_CommsRecipe.m_nFrontSlitReels = g_arrnMBTable[BATCH_SETPOINT_FRONT_SLIT_REELS];       // reels is already an integer

// Back slit reels/rolls
        g_CommsRecipe.m_nBackSlitReels = g_arrnMBTable[BATCH_SETPOINT_BACK_SLIT_REELS];     // reels is already an integer

// Sheet/Tube flag
        if(g_arrnMBTable[BATCH_SETPOINT_SHEET_TUBE] == 0)
             g_CommsRecipe.m_bSheetTube = FALSE;                    //
        else
            g_CommsRecipe.m_bSheetTube = TRUE;                      //

// Trim Feedback flag
        if(g_arrnMBTable[BATCH_SETPOINT_TRIM_FLAG] == 0)
             g_CommsRecipe.m_bTrimFeed = FALSE;                 //
        else
            g_CommsRecipe.m_bTrimFeed = TRUE;                       //

        // Order Number, in ASCII in MB, and in setpoint file.
        for(i=0; i<10; i++) // blank buffer
        {
            g_CommsRecipe.m_arrOrderNumber[i] = ' ';  // reset buffer
        }
        for(i=0; i<5; i++)                  // Data from 5 MB registers, i.e. 10 ASCII chars.
        {
        	uTempCI.nValue = g_arrnMBTable[BATCH_SETPOINT_ORDER_NUMBER + i];        // Modbus has two ascii chars in each register.
            arrOrderNumber[i*2] = uTempCI.cValue[1];
            arrOrderNumber[i*2 + 1] = uTempCI.cValue[0];
        }
        for(i=0; i<10 && ((arrOrderNumber[i]!=' ')&& (arrOrderNumber[i]!= 0x0)); i++)                 //terminate if space
        {
            g_CommsRecipe.m_arrOrderNumber[i] = arrOrderNumber[i];
        }
        nCharNo = i;  // pick up no of characters in name

        for(j=0; j<i; j++)                  // check for valid chars
        {
            cTemp = tolower(g_CommsRecipe.m_arrOrderNumber[j]); // switch to lower case
            if((!(cTemp >= 'a' && cTemp <= 'z')&&!(cTemp >= '0' && cTemp <= '9')&&(cTemp != '-')))
            {
                bInvalidChar = TRUE;
            }
        }
        if((bInvalidChar)||(nCharNo == 0x0))
        {
            sprintf(g_CommsRecipe.m_arrOrderNumber,"order   ");
        }


// Customer Code, in ASCII in MB, and in setpoint file.
        for(i=0; i<5; i++)                  // Data from 5 MB registers, i.e. 10 ASCII chars.
        {
            uTempCI.nValue = g_arrnMBTable[BATCH_SETPOINT_CUSTOMER_CODE + i];       // Modbus has two ascii chars in each register.
            g_CommsRecipe.m_arrCustomerCode[i*2] = uTempCI.cValue[0];
            g_CommsRecipe.m_arrCustomerCode[i*2 + 1] = uTempCI.cValue[1];
        }

// --review-- if material code is required.
// Material Code, in ASCII in MB, and in setpoint file.
//        for(i=0; i<5; i++)                  // Data from 5 MB registers, i.e. 10 ASCII chars.
//        {
//            uTempCI.nValue = g_arrnMBTable[BATCH_SETPOINT_MATERIAL_CODE + i];       // Modbus has two ascii chars in each register.
//            g_CommsRecipe.m_arrMaterialCode[i*2] = uTempCI.cValue[0];
//            g_CommsRecipe.m_arrMaterialCode[i*2 + 1] = uTempCI.cValue[1];
//        }



// operator no.
        g_CommsRecipe.m_nOperator_No = g_arrnMBTable[BATCH_SETPOINT_OPERATOR_NO];       // operator no


// order no.
        g_CommsRecipe.m_nOrder_No = g_arrnMBTable[BATCH_SETPOINT_ORDER_NO_INTEGER];       // order no.


// recipe no

        g_CommsRecipe.m_nRecipe_No = g_arrnMBTable[BATCH_RECIPE_NO];       // recipe no

}


//////////////////////////////////////////////////////
// LoadEmailConfigFromMB
//
// Loads email config downloaded.
//
//////////////////////////////////////////////////////
void LoadEmailConfigFromMB( void )
{
    unsigned int i;
    union CharsAndWord uTempCI;
    BOOL    bSwap = TRUE;

// mail server
       for(i=0; i<(EMAIL_STRING_SIZE/2); i++)                  //
        {
            uTempCI.nValue = g_arrnMBTable[EMAIL_MAIL_SERVER + i];        // Modbus has two ascii chars in each register.
            if(bSwap)
            {
                g_CalibrationData.m_cEmailMailServer[i*2] = uTempCI.cValue[1];
                g_CalibrationData.m_cEmailMailServer[i*2 + 1] = uTempCI.cValue[0];
            }
            else
            {
                g_CalibrationData.m_cEmailMailServer[i*2] = uTempCI.cValue[0];
                g_CalibrationData.m_cEmailMailServer[i*2 + 1] = uTempCI.cValue[1];
            }
        }

// user name
       for(i=0; i<(EMAIL_STRING_SIZE/2); i++)                  //
        {
            uTempCI.nValue = g_arrnMBTable[EMAIL_USER_NAME + i];        // Modbus has two ascii chars in each register.
            if(bSwap)
            {
                g_CalibrationData.m_cEmailUserName[i*2] = uTempCI.cValue[1];
                g_CalibrationData.m_cEmailUserName[i*2 + 1] = uTempCI.cValue[0];
            }
            else
            {
                g_CalibrationData.m_cEmailUserName[i*2] = uTempCI.cValue[0];
                g_CalibrationData.m_cEmailUserName[i*2 + 1] = uTempCI.cValue[1];
            }
        }

// password

       for(i=0; i<(EMAIL_STRING_SIZE/2); i++)                  //
        {
            uTempCI.nValue = g_arrnMBTable[EMAIL_PASSWORD + i];        // Modbus has two ascii chars in each register.
            if(bSwap)
            {
                g_CalibrationData.m_cEmailPassword[i*2] = uTempCI.cValue[1];
                g_CalibrationData.m_cEmailPassword[i*2 + 1] = uTempCI.cValue[0];
            }
            else
            {
                g_CalibrationData.m_cEmailPassword[i*2] = uTempCI.cValue[0];
                g_CalibrationData.m_cEmailPassword[i*2 + 1] = uTempCI.cValue[1];
            }
        }
// supervisor email address

       for(i=0; i<(EMAIL_STRING_SIZE/2); i++)                  //
        {
            uTempCI.nValue = g_arrnMBTable[EMAIL_SUPERVISOR_EMAIL_ADDRESS + i];        // Modbus has two ascii chars in each register.
            if(bSwap)
            {
                g_CalibrationData.m_cEmailSupervisorEmailAddress[i*2] = uTempCI.cValue[1];
                g_CalibrationData.m_cEmailSupervisorEmailAddress[i*2 + 1] = uTempCI.cValue[0];
            }
            else
            {
                g_CalibrationData.m_cEmailSupervisorEmailAddress[i*2] = uTempCI.cValue[0];
                g_CalibrationData.m_cEmailSupervisorEmailAddress[i*2 + 1] = uTempCI.cValue[1];
            }
        }
// operator email address

       for(i=0; i<(EMAIL_STRING_SIZE/2); i++)                  //
        {
            uTempCI.nValue = g_arrnMBTable[EMAIL_SUPPORT_EMAIL_ADDRESS + i];        // Modbus has two ascii chars in each register.
            if(bSwap)
            {
                g_CalibrationData.m_cEmailSupportEmailAddress[i*2] = uTempCI.cValue[1];
                g_CalibrationData.m_cEmailSupportEmailAddress[i*2 + 1] = uTempCI.cValue[0];
            }
            else
            {
                g_CalibrationData.m_cEmailSupportEmailAddress[i*2] = uTempCI.cValue[0];
                g_CalibrationData.m_cEmailSupportEmailAddress[i*2 + 1] = uTempCI.cValue[1];
            }
        }
// From email address

       for(i=0; i<(EMAIL_STRING_SIZE/2); i++)                  //
        {
            uTempCI.nValue = g_arrnMBTable[EMAIL_FROM_EMAIL_ADDRESS + i];        // Modbus has two ascii chars in each register.
            if(bSwap)
            {
                g_CalibrationData.m_cEmailFromEmailAddress[i*2] = uTempCI.cValue[1];
                g_CalibrationData.m_cEmailFromEmailAddress[i*2 + 1] = uTempCI.cValue[0];
            }
            else
            {
                g_CalibrationData.m_cEmailFromEmailAddress[i*2] = uTempCI.cValue[0];
                g_CalibrationData.m_cEmailFromEmailAddress[i*2 + 1] = uTempCI.cValue[1];
            }
        }

// Blender Name

       for(i=0; i<(EMAIL_STRING_SIZE/2); i++)                  //
        {
            uTempCI.nValue = g_arrnMBTable[EMAIL_BLENDERNAME + i];        // Modbus has two ascii chars in each register.
            if(bSwap)
            {
                g_CalibrationData.m_cEmailBlenderName[i*2] = uTempCI.cValue[1];
                g_CalibrationData.m_cEmailBlenderName[i*2 + 1] = uTempCI.cValue[0];
            }
            else
            {
                g_CalibrationData.m_cEmailBlenderName[i*2] = uTempCI.cValue[0];
                g_CalibrationData.m_cEmailBlenderName[i*2 + 1] = uTempCI.cValue[1];
            }
        }

g_bSaveAllCalibrationToEEprom = TRUE;       // set flag to cause all calibration data to be written to EEPROM.

}

///////////////////////////////////////////////////
// CopyEmailConfigDataToMB  asm = COPYVNMB
//
// copy email config data to modbus
//
//////////////////////////////////////////////////////
void CopyEmailConfigDataToMB( void )
{
   char cBuffer[(EMAIL_STRING_SIZE*2)];
   unsigned int i;
    BOOL bSwap = TRUE;
    union CharsAndWord uTempRev;

// Email server
    for(i=0; i<(EMAIL_STRING_SIZE*2); i++)
    {
        cBuffer[i] = 0;
    }
    sprintf( cBuffer, g_CalibrationData.m_cEmailMailServer );
    for(i=0; i<(EMAIL_STRING_SIZE/2); i++) // copy into MB table.
    {
        if(bSwap)
        {
            uTempRev.cValue[1] = cBuffer[i*2];
            uTempRev.cValue[0] = cBuffer[(i*2)+1];

        }
        else
        {
            uTempRev.cValue[0] = cBuffer[i*2];
            uTempRev.cValue[1] = cBuffer[(i*2)+1];
        }
        g_arrnMBTable[EMAIL_MAIL_SERVER + i] = uTempRev.nValue;

    }

// Email user name
    for(i=0; i<(EMAIL_STRING_SIZE*2); i++)
    {
        cBuffer[i] = 0;
    }
    sprintf( cBuffer, g_CalibrationData.m_cEmailUserName );
    for(i=0; i<(EMAIL_STRING_SIZE/2); i++) // copy into MB table.
    {
        if(bSwap)
        {
            uTempRev.cValue[1] = cBuffer[i*2];
            uTempRev.cValue[0] = cBuffer[(i*2)+1];

        }
        else
        {
            uTempRev.cValue[0] = cBuffer[i*2];
            uTempRev.cValue[1] = cBuffer[(i*2)+1];
        }


        g_arrnMBTable[EMAIL_USER_NAME + i] = uTempRev.nValue;
     }
// Email password
    for(i=0; i<(EMAIL_STRING_SIZE*2); i++)
    {
        cBuffer[i] = 0;
    }
    sprintf( cBuffer, g_CalibrationData.m_cEmailPassword );
    for(i=0; i<(EMAIL_STRING_SIZE/2); i++) // copy into MB table.
    {
        if(bSwap)
        {
            uTempRev.cValue[1] = cBuffer[i*2];
            uTempRev.cValue[0] = cBuffer[(i*2)+1];

        }
        else
        {
            uTempRev.cValue[0] = cBuffer[i*2];
            uTempRev.cValue[1] = cBuffer[(i*2)+1];
        }
        g_arrnMBTable[EMAIL_PASSWORD + i] = uTempRev.nValue;
    }


// Supervisor email address
    for(i=0; i<(EMAIL_STRING_SIZE*2); i++)
    {
        cBuffer[i] = 0;
    }
    sprintf( cBuffer, g_CalibrationData.m_cEmailSupervisorEmailAddress );
    for(i=0; i<(EMAIL_STRING_SIZE/2); i++) // copy into MB table.
    {
        if(bSwap)
        {
            uTempRev.cValue[1] = cBuffer[i*2];
            uTempRev.cValue[0] = cBuffer[(i*2)+1];

        }
        else
        {
            uTempRev.cValue[0] = cBuffer[i*2];
            uTempRev.cValue[1] = cBuffer[(i*2)+1];
        }
        g_arrnMBTable[EMAIL_SUPERVISOR_EMAIL_ADDRESS + i] = uTempRev.nValue;
    }

// Support email address
    for(i=0; i<(EMAIL_STRING_SIZE*2); i++)
    {
        cBuffer[i] = 0;
    }
    sprintf( cBuffer, g_CalibrationData.m_cEmailSupportEmailAddress );
    for(i=0; i<(EMAIL_STRING_SIZE/2); i++) // copy into MB table.
    {
        if(bSwap)
        {
            uTempRev.cValue[1] = cBuffer[i*2];
            uTempRev.cValue[0] = cBuffer[(i*2)+1];

        }
        else
        {
            uTempRev.cValue[0] = cBuffer[i*2];
            uTempRev.cValue[1] = cBuffer[(i*2)+1];
        }
        g_arrnMBTable[EMAIL_SUPPORT_EMAIL_ADDRESS + i] = uTempRev.nValue;
    }

// From email address
    for(i=0; i<(EMAIL_STRING_SIZE*2); i++)
    {
        cBuffer[i] = 0;
    }
    sprintf( cBuffer, g_CalibrationData.m_cEmailFromEmailAddress );
    for(i=0; i<(EMAIL_STRING_SIZE/2); i++) // copy into MB table.
    {
        if(bSwap)
        {
            uTempRev.cValue[1] = cBuffer[i*2];
            uTempRev.cValue[0] = cBuffer[(i*2)+1];

        }
        else
        {
            uTempRev.cValue[0] = cBuffer[i*2];
            uTempRev.cValue[1] = cBuffer[(i*2)+1];
        }
        g_arrnMBTable[EMAIL_FROM_EMAIL_ADDRESS + i] = uTempRev.nValue;
    }

// Blender Name
    for(i=0; i<(EMAIL_STRING_SIZE*2); i++)
    {
        cBuffer[i] = 0;
    }
    sprintf( cBuffer, g_CalibrationData.m_cEmailBlenderName );
    for(i=0; i<(EMAIL_STRING_SIZE/2); i++) // copy into MB table.
    {
        if(bSwap)
        {
            uTempRev.cValue[1] = cBuffer[i*2];
            uTempRev.cValue[0] = cBuffer[(i*2)+1];

        }
        else
        {
            uTempRev.cValue[0] = cBuffer[i*2];
            uTempRev.cValue[1] = cBuffer[(i*2)+1];
        }
        g_arrnMBTable[EMAIL_BLENDERNAME + i] = uTempRev.nValue;
    }




}





//////////////////////////////////////////////////////
// CopyComponentWeightsToMB         (from asm = CPYHWTSMB)
//
// COPY component weights INTO MODBUS TABLE
//
//
// M.McKiernan                          13-07-2004
// First Pass
//
//////////////////////////////////////////////////////
void CopyComponentWeightsToMB( void )
{
    union WordAndDWord uValue;
    unsigned int i;
    float fValue;
    // for each component.

    if(g_CalibrationData.m_nComponents <= 10)
    {
        for(i = 0; i < g_CalibrationData.m_nComponents; i++ )
        {
            // actual component weights

            if(g_bShowInstantaneousWeight)
            {
                fValue =    g_fComponentActualWeight[i];
            }
            else
            {
                fValue =    g_fComponentActualWeightRR[i];
            }
            if(g_fWeightConversionFactor != 1.0f)
            fValue *=    g_fWeightConversionFactor;
            // rounding to nearest 4TH decimal digit.
            fValue += 0.00005;
            // Wt to 4dp in MB.
            uValue.lValue = (long)(fValue * 10000.0f);
            // M.s.Byte.
            g_arrnMBTable[PSUM_BLOCK_1 + (i*PSUM_BLOCK_SIZE) + PSUM_BATCH_WT_OFFSET] = uValue.nValue[0];
            // l.s.Byte.
            g_arrnMBTable[PSUM_BLOCK_1 + (i*PSUM_BLOCK_SIZE) + PSUM_BATCH_WT_OFFSET + 1] = uValue.nValue[1];
        }
    }


    else // > 10 components     --review-- modbus expansion
    {
        for(i = 0; i < 10; i++ )                        // copy 10 components first.
        {
            // actual component weights
            if(g_bShowInstantaneousWeight)
            {
                fValue =    g_fComponentActualWeight[i];
            }
            else
            {
                fValue =    g_fComponentActualWeightRR[i];
            }
 if(g_fWeightConversionFactor != 1.0f)
            fValue *=    g_fWeightConversionFactor;
            // rounding to nearest 4TH decimal digit.
            fValue += 0.00005;
            // Wt to 4dp in MB.
            uValue.lValue = (long)(fValue * 10000.0f);
            // M.s.Byte.
            g_arrnMBTable[PSUM_BLOCK_1 + (i*PSUM_BLOCK_SIZE) + PSUM_BATCH_WT_OFFSET] = uValue.nValue[0];
            // l.s.Byte.
            g_arrnMBTable[PSUM_BLOCK_1 + (i*PSUM_BLOCK_SIZE) + PSUM_BATCH_WT_OFFSET + 1] = uValue.nValue[1];
        }

        for(i = 10; i < g_CalibrationData.m_nComponents; i++ )                        // copy last two components.
        {
            // actual component weights
            fValue =    g_fComponentActualWeight[i];
            if(g_fWeightConversionFactor != 1.0f)
            fValue *=    g_fWeightConversionFactor;
           // rounding to nearest 4TH decimal digit.
            fValue += 0.00005;
            // Wt to 4dp in MB.
            uValue.lValue = (long)(fValue * 10000.0f);
            // M.s.Byte.
            g_arrnMBTable[PSUM_BLOCK_11 + ((i-10) * PSUM_BLOCK_SIZE) + PSUM_BATCH_WT_OFFSET] = uValue.nValue[0];
            // l.s.Byte.
            g_arrnMBTable[PSUM_BLOCK_11 + ((i-10) * PSUM_BLOCK_SIZE) + PSUM_BATCH_WT_OFFSET + 1] = uValue.nValue[1];
        }
    }
}


/*
;* COPY HOPPER Wt's INTO MODBUS TABLE PROD. SUMMMARY DATA AREA
CPYHWTSMB:
        TST     PROTOCOL
        BPL     CPYHWTSMX
        LDAB   #RAMBANK
        TBZK                    ;~;SET EK FOR BANK 1, I.E. RAM
        LDZ     #MB1HWT         ; MODBUS TABLE, CH #1 HOPPER WT
        LDAB    #1              ; BLEND #, START AT 1
CPYHWTS10 PSHB
        LDX     #CMP1ACTWGT     ; ADDRESS OF COMP #N WEIGHT
        DECB
        ABX
        ABX
        ABX
        JSR     BCDHEX3X        ; CONVERT 3 BYTES @X
        CLRA
        STD     0,Z
        STE     2,Z
        AIZ     #MBPSBKSIZ       ; INC. ON TO NEXT CH. MB PROD SUMM BLOCK
        PULB
        INCB
        CMPB    NOBLNDS
        BLS     CPYHWTS10
CPYHWTSMX:
        RTS

*/


//////////////////////////////////////////////////////
// PostMBWrite                      (from asm = POSTMBWRT)
//
// Funtion to handle writes into MB table from comms.
// Function called in Foregnd, and does whatever is required depending on data written.
//
// M.McKiernan                          11-06-2004
// First Pass
//
//////////////////////////////////////////////////////
void PostMBWrite( void )
{
    if(g_nProtocol == PROTOCOL_MODBUS)      // nothing to do if not Modbus protocol

    {
        if( g_nMBWriteOrErrorFlag )                             // if the "write or error" flag set
        {

//  were %'s written?
            if(g_nMBWriteOrErrorFlag & MB_WRITE_PERCENTAGES_BIT )   // were %'s written?
            {
                CopyTargetPercentagesToMB();                                    // copy %'s back into MB table.
            }

//  were throughputs written?
            if(g_nMBWriteOrErrorFlag & MB_WRITE_THRUPUTS_BIT )
            {
                CopyTargetThruputsToMB();                                       // copy throughputs back into MB table.
            }
// was Order info written?
            if(g_nMBWriteOrErrorFlag & MB_WRITE_ORDER_INFO_BIT )
            {
                CopyOrderInfoToMB();                                                // copy order info back into MB table.
            }

// was Component config. written?
            if(g_nMBWriteOrErrorFlag & MB_WRITE_COMP_CONFIG_BIT )
            {
                CopyComponentConfigurationToMB();                       // copy comp. config. back into MB table.
            }

// was weight calibration written?
            if(g_nMBWriteOrErrorFlag & MB_WRITE_WEIGHT_CALIB_BIT )
            {
                CopyWeightConstantsToMB();                      // copy weight calib. back into MB table.
            }

// was hopper calibration written?
            if(g_nMBWriteOrErrorFlag & MB_WRITE_HOPPER_CALIB_BIT )
            {
                CopyHopperConstantsToMB();                      // copy hopper calib. back into MB table.
            }

// were Measurement parameters written?
            if(g_nMBWriteOrErrorFlag & MB_WRITE_MEAS_PARAMETERS_BIT )
            {
                CopyMeasurementAndControlParametersToMB();  // copy Measurement parameters back into MB table.
            }

// were speed parameters written?
            if(g_nMBWriteOrErrorFlag & MB_WRITE_SPEED_PARAMETERS_BIT )
            {
                CopySpeedParametersToMB();      // copy Speed parameters back into MB table.
            }

// was system configuration written?
            if(g_nMBWriteOrErrorFlag & MB_WRITE_SYSTEM_CONFIG3_BIT )
            {
                CopySystemConfiguration3ToMB();  // copy system configuration back into MB table.
            }

// was system configuration written?
            if(g_nMBWriteOrErrorFlag & MB_WRITE_SYSTEM_CONFIG_BIT )
            {
                CopySystemConfigurationToMB();  // copy system configuration back into MB table.
            }


// was Liquid calibration written?
            if(g_nMBWriteOrErrorFlag & MB_WRITE_LIQUID_CALIB_BIT )
            {
                CopyLiquidCalibrationToMB();                        // copy liquid calib. back into MB table.
            }

// was time written?
            if(g_nMBWriteOrErrorFlag & MB_WRITE_TIME_BIT )
            {
                CopyTimeToMB();                             // copy time back into MB table.
            }

 // OTHER ERROR CODES TO BE HANDLED HERE //
            g_nMBWriteOrErrorFlag = 0;                      // clear flag for next pass.
        }

// GOOD DATA HANDLED  here  //
        if( g_nMBWriteOrFlag )
        {
 // Write, good data, copy data from  MB TABLE into system

            if(g_nMBWriteOrFlag & (MB_WRITE_PERCENTAGES_BIT | MB_WRITE_THRUPUTS_BIT | MB_WRITE_ORDER_INFO_BIT))  // ANY SETPT DATA?  sbb--todo--
            {
            if( !CompareMBSetpointHistoryArray() || g_bPanelWriteHasHappened || !g_CalibrationData.m_bAllowForContinuousDownload )  // has any of the setpoint data changed - comparing vs history(copy) array
             {
                StoreMBSetpointHistoryArray();        // store current values into history array.

                    if(((g_arrnMBTable[BATCH_SETPOINT_RESET_TOTALS] & MB_RESET_TOTALS_BIT) == 0) && (g_nMBWriteOrFlag & (MB_WRITE_PERCENTAGES_BIT) ))
                    {
                        CopyOrderReportData();          // ASM = ASSORPTASSEMBLE ORDER REPORT if bit is 0.
                    }
                    else
                    {
                        g_arrnMBTable[BATCH_SETPOINT_RESET_TOTALS] &= MB_RESET_TOTALS_CLEAR_MASK;       // CLEAR BIT FOR NEXT PASS.
                    }

// were %'s written?
                    if(g_nMBWriteOrFlag & MB_WRITE_PERCENTAGES_BIT ) // were %'s written?
                    {
                        LoadTargetPercentagesFromMB();                  // Load %'s from MB table.
                    }

// were throughputs written?
                    if(g_nMBWriteOrFlag & MB_WRITE_THRUPUTS_BIT )
                    {
                        LoadTargetThruputsFromMB();                     // load throughputs from MB table.
                    }
// was Order info written?
                    if(g_nMBWriteOrFlag & MB_WRITE_ORDER_INFO_BIT )
                    {
                        LoadOrderInfoFromMB();                      // load order info from MB table.
                    }

                    if(!g_bRecipeWaiting) // if recipe not waiting and transfer allowed
                    {
                        InitiateRecipeTransferToCurrentOrder();
                    }

              }


            }  // end of check for setpt data.

// was Component config. written?
            if(g_nMBWriteOrFlag & MB_WRITE_COMP_CONFIG_BIT)  // New Component configuration DATA?
            {
               LoadComponentConfigFromMB();                 // load component configurations from MB table.
            }
// was weight calibration written?
            if(g_nMBWriteOrFlag & MB_WRITE_WEIGHT_CALIB_BIT )
            {
                LoadWeightCalibrationFromMB();                      // Load weight calib. from MB table.
            }

// was hopper calibration written?
            if(g_nMBWriteOrFlag & MB_WRITE_HOPPER_CALIB_BIT )
            {
                LoadHopperCalibrationFromMB();                      // Load hopper calib. from MB table.
            }

// were Measurement parameters written?
            if(g_nMBWriteOrFlag & MB_WRITE_MEAS_PARAMETERS_BIT )
            {
                LoadMeasParametersFromMB(); // Load Measurement parameters from MB table.
            }

// were speed parameters written?
            if(g_nMBWriteOrFlag & MB_WRITE_SPEED_PARAMETERS_BIT )
            {
                LoadSpeedParametersFromMB();        // Load Speed parameters from MB table.
            }

// was system configuration written?
            if(g_nMBWriteOrFlag & MB_WRITE_SYSTEM_CONFIG_BIT )
            {
                LoadSystemConfigurationFromMB();                        // Load system configuration from MB table.
            }

            if(g_nMBWriteOrFlag & MB_WRITE_SYSTEM_CONFIG3_BIT )
            {
                LoadSystemConfiguration3FromMB();                        // Load system configuration from MB table.
            }

// was Liquid calibration written?
            if(g_nMBWriteOrFlag & MB_WRITE_LIQUID_CALIB_BIT )
            {
                LoadLiquidCalibrationFromMB();                      // Load liquid calib. from MB table.
            }

// was  calibration weight written?
            if(g_nMBWriteOrFlag & MB_WRITE_CALIBRATION_WEIGHT_BIT )
            {
                LoadCalibrationWeightFromMB();  // LDMBHOPPERCAL Load calib. wt. from MB table.
            }
// was  there any toggle status commands
            if(g_nMBWriteOrFlag & MB_TOGGLE_STATUS_BIT )
            {
                MBToggleStatusCommands();   // ()           ASM = MBTOGSTA
            }

// was time written?
            if(g_nMBWriteOrFlag & MB_WRITE_TIME_BIT )
            {
                LoadTimeFromMB();                               // Load time from MB table.
            }

            if(g_nMBWriteOrFlag & MB_WRITE_EMAIL_CONFIG_BIT )
            {
                LoadEmailConfigFromMB();
            }

            g_nMBWriteOrFlag = 0;                       // clear flag for next pass.
    if(g_bPanelWriteHasHappened)
    {
        g_bPanelWriteHasHappened = FALSE;
    }

        } // end of check for g_nMBWriteOrFlag


// to here.

    }  // end of protocol=MB check.


}




//////////////////////////////////////////////////////
// CopyDACountsToMB         (from asm = CPYDACMB)
//
// COPY d/a to modbus
//
//
// P.Smith                               10-3-2005
//
//////////////////////////////////////////////////////
void InitiateRecipeTransferToCurrentOrder( void )
{

// recipe no. set to 1.

//                   SetupMttty();
//                   iprintf("\n transfer to current order");
                   g_CommsRecipe.m_nFileNumber = RUNNINGRECIPEFILENO;        // Set the downloaded file number = 1 (can be changed by operator and saved to any file)
                   g_bNewCommsRecipeFlag = TRUE;                   // set flag to indicate new setpoint data from Comms.

                    g_nTransferSetpointsFlag = TRANSFERSPFROMCOMMSID;   // set flag to cause setpoints to get loaded at end of cycle.

                    memcpy(&g_TempRecipe, &g_CommsRecipe, sizeof( g_CommsRecipe ) );
//nbb--todo--put back      memcpy(&g_TempRecipeAdditionalData, &g_CommsRecipeAdditionalData, sizeof( g_CommsRecipeAdditionalData ) );

                    g_nSaveRecipeSequence = SAVE_RECIPE_START;
                    ForegroundSaveRecipe( );    // save recipe while running in foreground (1 byte per pass).
                    g_nMBWriteOrFlag &= 0xFFF8; // clear the "new setpts" bits.
//nbb--todo--put back                    g_bSaveAdditionalDataToEEPROM = TRUE;

}



//////////////////////////////////////////////////////
// CopyDACountsToMB         (from asm = CPYDACMB)
//
// COPY d/a to modbus
//
//
// P.Smith                               10-3-2005
//
//////////////////////////////////////////////////////
void CopyDACountsToMB( void )
{
    if(g_CalibrationData.m_nControlType == CONTROL_TYPE_INC_DEC)
    {
        g_arrnMBTable[BATCH_SUMMARY_SCREW_DAC_1] = g_nExtruderControlSetpointIncDec;  //
    }
    else
    {
        g_arrnMBTable[BATCH_SUMMARY_SCREW_DAC_1] = g_nExtruderDAValue;  //
    }
}



//////////////////////////////////////////////////////
// CopyHopperSetWeightsToMB         (from asm = CPYHOPPERSETWTMB)
//
// COPY set weights to modbus
//
// P.Smith                               10-3-2005
//
//////////////////////////////////////////////////////
void CopyHopperSetWeightsToMB( void )
{
unsigned int i;
float fTemp;
union WordAndDWord uValue;


    if(g_CalibrationData.m_nComponents <= 10)
    {
        for(i = 0; i < g_CalibrationData.m_nComponents; i++ )  // only allow 10 components of summary data --review-- modbus expansion
        {
            fTemp = g_fComponentTargetWeight[i];
            if(g_fWeightConversionFactor != 1.0f)
            fTemp *= g_fWeightConversionFactor;
            uValue.lValue = (long) (10000 * fTemp);
            g_arrnMBTable[(i*PSUM_BLOCK_SIZE) + BATCH_SUMMARY_TARGET_WEIGHT_1] = uValue.nValue[0]; // copy msb
            g_arrnMBTable[(i*PSUM_BLOCK_SIZE) + BATCH_SUMMARY_TARGET_WEIGHT_1+1] = uValue.nValue[1]; // copy lsb
        }
    }
    else   //--review-- modbus expansion
    {
        for(i = 0; i < 10; i++ )  //
        {

            fTemp = g_fComponentTargetWeight[i];
            if(g_fWeightConversionFactor != 1.0f)
            fTemp *= g_fWeightConversionFactor;
            uValue.lValue = (long) (10000 * fTemp);
            g_arrnMBTable[(i*PSUM_BLOCK_SIZE) + BATCH_SUMMARY_TARGET_WEIGHT_1] = uValue.nValue[0]; // copy msb
            g_arrnMBTable[(i*PSUM_BLOCK_SIZE) + BATCH_SUMMARY_TARGET_WEIGHT_1+1] = uValue.nValue[1]; // copy lsb
        }

        for(i = 10; i < g_CalibrationData.m_nComponents; i++ )  //
        {
            fTemp = g_fComponentTargetWeight[i];
            if(g_fWeightConversionFactor != 1.0f)
            fTemp *= g_fWeightConversionFactor;
            uValue.lValue = (long) (10000 * fTemp);
            g_arrnMBTable[PSUM_BLOCK_11 + ((i-10) * PSUM_BLOCK_SIZE) + PSUM_TARGET_WT_OFFSET] = uValue.nValue[0]; // copy msb
            g_arrnMBTable[PSUM_BLOCK_11 + ((i-10) * PSUM_BLOCK_SIZE) + PSUM_TARGET_WT_OFFSET+1] = uValue.nValue[1]; // copy lsb
        }



    }

}

//

/*
;* COPY HOPPER Wt's INTO MODBUS TABLE PROD. SUMMMARY DATA AREA
CPYHOPPERSETWTMB:
        LDAB   #RAMBANK
        TBZK                    ;~;SET EK FOR BANK 1, I.E. RAM
        LDZ     #MB1KGH         ; MODBUS TABLE, CH #1 HOPPER WT
        LDAB    #1              ; BLEND #, START AT 1
REPCPY  PSHB
        LDX     #CMP1TARWGT     ; ADDRESS OF COMP #N WEIGHT
        DECB
        ABX
        ABX
        ABX
        JSR     BCDHEX3X        ; CONVERT 3 BYTES @X
        CLRA
        STD     0,Z
        STE     2,Z
        AIZ     #MBPSBKSIZ       ; INC. ON TO NEXT CH. MB PROD SUMM BLOCK
        PULB
        INCB
        CMPB    NOBLNDS
        BLS     REPCPY
        RTS


*/


//////////////////////////////////////////////////////
// CopyHopperSetWeightsToMB         (from asm = CPYHOPPERSETWTMB)
//
// COPY hopper weight to modbus
//
// P.Smith                               10-3-2005
//
//////////////////////////////////////////////////////
void CopyHopperWeightToMB( void )
{
union WordAndDWord uValue;
float   fTemp;
//int nTest1,nTest2;

// --review resolution is being lost here somewhere in the hopper weight calculation.

    CalculateHopperWeight(); // calculate current weight in hopper ASM = CALHWT
    fTemp =  g_fOneSecondHopperWeight;
    if(g_fWeightConversionFactor != 1.0f)
    fTemp *= g_fWeightConversionFactor;
    uValue.lValue = (long) (10000 * fTemp); // convert weight to 1/10ths of grams
    g_arrnMBTable[BATCH_SUMMARY_HOPPER_WEIGHT] = uValue.nValue[0]; // copy msb
    g_arrnMBTable[BATCH_SUMMARY_HOPPER_WEIGHT + 1] = uValue.nValue[1]; // copy lsb
//    nTest1 = g_arrnMBTable[BATCH_SUMMARY_HOPPER_WEIGHT];
//    nTest2 = g_arrnMBTable[BATCH_SUMMARY_HOPPER_WEIGHT+1];
}


//////////////////////////////////////////////////////
// SetInProgressStatus         (from asm = SETVACUUMSTATUS)
//
// Sets appropriate in progress bit depending on extruder no.
//
// P.Smith                               23-8-2005
//
//////////////////////////////////////////////////////
void SetInProgressStatus( void )
{
unsigned int i,j;
    j = 1;
    if(g_arrnMBTable[BATCH_EXTRUDER_NO])
    {
        for(i = 1; i < g_arrnMBTable[BATCH_EXTRUDER_NO]; i++)
        {
            j <<= 1;       // shift 1 place to the left
        }
        g_arrnMBTable[BATCH_SINGLE_RECIPE_VACUUM_STATUS] |= j;
    }
}


//////////////////////////////////////////////////////
// ClearInProgressStatus         (from asm = CLEARVACUUMSTATUS)
//
// Clears appropriate in progress bit depending on extruder no.
//
// P.Smith                               23-8-2005
//
//////////////////////////////////////////////////////
void ClearInProgressStatus( void )
{
unsigned int i,j;
    j = 1;
    if(g_arrnMBTable[BATCH_EXTRUDER_NO])
    {
        for(i = 1; i < g_arrnMBTable[BATCH_EXTRUDER_NO]; i++)
        {
            j <<= 1;       // shift 1 place to the left
        }

        j ^= 0xFFFF;       // get 1's complement for anding purposes.

        g_arrnMBTable[BATCH_SINGLE_RECIPE_VACUUM_STATUS] &= j;
    }
}


//////////////////////////////////////////////////////
// CheckForPendingRecipe         (from asm = CHECKFORPENDINGRECIPE)
//
// Checks for a pending recipe in single recipe mode.
//
// P.Smith                               23-8-2005
//
//////////////////////////////////////////////////////
void CheckForPendingRecipe( void )
{
    if(g_CalibrationData.m_nBlenderMode == MODE_SINGLERECIPEMODE && g_bRecipeWaiting && !g_bBatchHasFilled && !g_bBlenderInhibit  )  // single recipe mode ?
    {
        g_bRecipeWaiting = FALSE;
        LoadPercentageTargetsFromComms();
        InitiateRecipeTransferToCurrentOrder();

        g_bBlenderInhibit = TRUE;       //  ASM BLENDERINHIBIT set blender inhibit
        g_bCleanOnNextCycle = TRUE;     // ASM CLEANONNEXTCYCLE indicate that the blender should clean on the next cycle.
        VacInhibitOff();                // ASM VACINHIBITOFF
//        mtprintf("\n vacuum inhibit off");
    }
}


/*

;COPYHOPPERWEIGHTMB - COPY HOPPER WEIGHT

COPYHOPPERWEIGHTMB:
        JSR     CALHWT          ;  CALCULATE HOPPER WEIGHT
        LDD     EREG
        STD     HOPPERWEIGHT    ; STORE TO HOPPER WEIGHT.
        LDD     EREG+2
        STD     HOPPERWEIGHT+2  ; DITHO.
        LDX     #EREG
        JSR     BCDHEX3X         ; HOPPER WEIGHT -> HEX.
        CLRA                     ; RESET MSB
        STD     MBHOPPERWEIGHT   ; STORE TO HOPPER WEIGHT.
        STE     MBHOPPERWEIGHT+2 ; DITHO.
        RTS

*/





/*
;* COPY D/A VALUES INTO MODBUS TABLE PROD. SUMMMARY DATA AREA
CPYDACMB:
        TST     PROTOCOL
        BPL     CPYDACMX

        LDAB   #RAMBANK
        TBZK                    ;~;SET EK FOR BANK 1, I.E. RAM
        LDZ     #MB1DAC         ; MODBUS TABLE, CH #1 D/A VALUE
        LDAB    #1              ; BLEND #, START AT 1

CPYDAC10 PSHB

        LDX     #DA1TMP        ; D/A VALUE
        DECB
        ASLB                    ; *2
        ABX                     ; X NOW POINTS TO D/A VALUE FOR CH.
        LDD     0,X
        STD     0,Z

        PULB
        INCB
        AIZ     #MBPSBKSIZ       ; INC. ON TO NEXT CH. MB PROD SUMM BLOCK
        CMPB    NOBLNDS
        BLS     CPYDAC10

        LDAB    #MAXCOMPONENTNO ; PLACE FOR 8 BLENDS IN MB TABLE
        SUBB    NOBLNDS         ; GET NO. OF UNUSED BLENDS
        BEQ     CPYDAC20
CPYDAC15 CLRW   0,Z             ; SET UNUSED D/A's TO 0000
        AIZ     #MBPSBKSIZ
        DECB
        BNE     CPYDAC15

CPYDAC20:
        LDX     DA1TMP
        STX     MBEA_D

CPYDACMX:
        RTS


*/

/*

        CopyTargetPercentagesToMB();
        CopyTargetThruputsToMB();
        CopyOrderInfoToMB();

// masks for the MB write flags
#define MB_WRITE_PERCENTAGES_BIT            0x0001
#define MB_WRITE_THRUPUTS_BIT               0x0002
#define MB_WRITE_ORDER_INFO_BIT             0x0004
#define MB_WRITE_COMP_CONFIG_BIT            0x0008
#define MB_WRITE_WEIGHT_CALIB_BIT           0x0010
#define MB_WRITE_HOPPER_CALIB_BIT           0x0020
#define MB_WRITE_MEAS_PARAMETERS_BIT        0x0040
#define MB_WRITE_SPEED_PARAMETERS_BIT       0x0080
#define MB_WRITE_SYSTEM_CONFIG_BIT          0x0100
#define MB_WRITE_LIQUID_CALIB_BIT           0x0200
#define MB_WRITE_TIME_BIT                       0x0400
#define MB_WRITE_NOT_ALLOCATED_BIT          0x0800          //  ***SPARE - HAS NOT BEEN ALLOCATED TO ANYTHING.***
#define MB_TOGGLE_STATUS_BIT                    0x1000          // TOGGLE STATUS COMMANDS
#define MB_WRITE_CALIBRATION_WEIGHT_BIT 0x2000          // CALIBRATION WEIGHT
#define MB_WRITE_RESET_TOTALS_BIT           0x4000          // Reset order totals
#define MB_WRITEABLE_BIT                        0X8000          // INDICATES PARAMETER IN MB IS WRITEABLE.



;* POSTMBWRT - ROUTINE CALLED AFTER A WRITE INTO MODBUS TABLE
;               THIS ROUTINE IS CALLED IN THE BACKGROUND
POSTMBWRT:
        TST     PROTOCOL
        LBPL    PTMBWRT_X
        LDE     MBWORFER        ; 'OR' FLAG ERROR
        LBEQ     PTMBWRT50
; Error, illegal data, copy data from system into MB TABLE
        ANDE    #$0001          ; CHECK FOR % ERROR
        BEQ     PTMBWRT02
        JSR     CPYPCTSMB       ; COPY %'S BACK INTO MB TABLE
PTMBWRT02:
        LDE     MBWORFER        ; ERROR FLAG
        ANDE    #$0002          ; CHECK FOR TPT ERROR
        BEQ     PTMBWRT06
        JSR     CPYTPTSMB       ; COPY TPT SETPOINTS BACK INTO MB TABLE
PTMBWRT06:
        LDE     MBWORFER        ; ERROR FLAG
        ANDE    #$0004          ; CHECK FOR ORDER INFO ERROR
        BEQ     PTMBWRT08
        JSR     CPYOINFMB       ; COPY ORDER INFO SETPOINTS BACK INTO MB TABLE
PTMBWRT08:
        LDE     MBWORFER        ; ERROR FLAG
        ANDE    #$0008          ; CHECK FOR K DATA ERROR
        BEQ     PTMBWRT12
        JSR     COPYKSMB       ; COPY K VALUES BACK INTO MB TABLE
PTMBWRT12:
        LDE     MBWORFER        ; ERROR FLAG WT CONSTs DATA ERROR
        ANDE    #$0010          ; CHECK FOR WT CONSTs DATA ERROR
        BEQ     PTMBWRT16
        JSR     CPYWTCMB       ; COPY Wt CONSTs BACK INTO MB TABLE
PTMBWRT16:
        LDE     MBWORFER        ; ERROR FLAG HOPPER CONSTs DATA ERROR
        ANDE    #$0020          ; CHECK FOR  DATA ERROR
        BEQ     PTMBWRT20
        JSR     CPYHPCMB       ; COPY HOPPER CONSTs BACK INTO MB TABLE
PTMBWRT20:
        LDE     MBWORFER        ; ERROR FLAG MEASUREMENT & CONTROL PARAMETERS
        ANDE    #$0040          ; CHECK FOR  DATA ERROR
        BEQ     PTMBWRT24
        JSR     CPYMCPMB       ; COPY PARAMETERS BACK INTO MB TABLE
PTMBWRT24:
        LDE     MBWORFER        ; ERROR FLAG SPEED CONST.S
        ANDE    #$0080          ; CHECK FOR  DATA ERROR
        BEQ     PTMBWRT28
        JSR     CPYLSCMB        ; COPY SPEED CONST BACK INTO MB TABLE
PTMBWRT28:
        LDE     MBWORFER        ; ERROR FLAG SYSTEM CONFIGURATION
        ANDE    #$0100          ; CHECK FOR  DATA ERROR
        BEQ     PTMBWRT32
        JSR     CPYSYCMB        ; COPY SYS. CONFIG. BACK INTO MB TABLE
PTMBWRT32:
        LDE     MBWORFER        ; ERROR FLAG LIQ CALIBRATION
        ANDE    #$0200          ; CHECK FOR  DATA ERROR
        BEQ     PTMBWRT36
        JSR     CPYLIQCMB        ; COPY LIQ.CONST's BACK INTO MB TABLE
PTMBWRT36:
        LDE     MBWORFER        ; ERROR FLAG TIME
        ANDE    #$0400          ; CHECK FOR  ERROR
        BEQ     PTMBWRT40
        JSR     CPYTIMMB        ; COPY TIME BACK INTO MB TABLE
PTMBWRT40:
;; OTHER ERROR CODES TO BE HANDLED HERE
        LBRA     PTMBWRT_Q       ; EXIT

PTMBWRT50:                      ; GOOD DATA WRITE
        LDE     MBWORFLG        ; 'OR' FLAG - WRITE
        LBEQ     PTMBWRT_X       ; IF CLEAR, NO DATA WRITTEN

; Write, good data, copy data from  MB TABLE into system
        ANDE    #$0007          ; ANY SETPT DATA?  // WAS 000F
        LBEQ     PTMBWRT58       ; IF NOT SKIP PAST SETPTS WAS 66 ORIGIONALLY

; if L5 fitted (0) then will need to set flag to load setpoints
        TST     SINGLEWORDWRITE          ;SINGLE WORD WRITE ?
        LBNE     PTMBWRT66

        TSTW    MBRSTOTLS       ; RESET ORDER TOTALS FLAG IN MB TABLE
        BNE     PTMBWRT50B        ; IF NON-ZERO NO ACTION REQ'D
        JSR     ASSORPT         ; ASSEMBLE ORDER REPORT
;        LDAB    #$0F
;        STAB    OCOFLG          ; SET ORDER CHANGE OCCURRED FLAG
        CLRW    MBRSTOTLS       ; CLEAR FLAG
PTMBWRT50B:
        LDE     MBWORFLG        ; 'OR' FLAG - WRITE
        ANDE    #$0001          ; CHECK FOR % WRITE
        BEQ     PTMBWRT52
        JSR     LDMBPCTS       ; LOAD %'S FROM MB TABLE
PTMBWRT52:
        LDE     MBWORFLG        ; 'OR' FLAG - WRITE
        ANDE    #$0002          ; CHECK FOR NEW TPT SETPOINTS
        BEQ     PTMBWRT56
        JSR     LDMBTPTS       ; LOAD TPT'S FROM MB TABLE
PTMBWRT56:
        LDE     MBWORFLG        ; 'OR' FLAG - WRITE
        ANDE    #$0004          ; CHECK FOR NEW ORDER INFO SETPOINTS
        BEQ     PTMBWRT58
        JSR     LDMBOINFO       ; LOAD ORDER INFO FROM MB TABLE

PTMBWRT58:
        LDE     MBWORFLG        ; 'OR' FLAG - WRITE
        ANDE    #$0008          ;
        BEQ     PTMBWRT62
        JSR     LDMBCOMPCFG     ; LOAD COMPONENT CONFIGURATION.
PTMBWRT62:

        LDE     MBWORFLG        ; 'OR' FLAG - WRITE
        ANDE    #$2000          ; CHECK FOR NEW K VALUES
        BEQ     PTMBWRT63
        JSR     LDMBHOPPERCAL   ; LOAD HOPPER CALIBRATION.
PTMBWRT63:

;NOTE MBNEWDATA BIT 10 IS SET IN CPYORRMB

PTMBWRT66:
        LDE     MBWORFLG        ; 'OR' FLAG - WRITE
        ANDE    #$0010          ; CHECK FOR NEW Wt CONSTS (C1s & TARES)
        BEQ     PTMBWRT70
        JSR     LDMBWCS         ; LOAD Wt CONSTs FROM MB TABLE
PTMBWRT70:
        LDE     MBWORFLG        ; 'OR' FLAG - WRITE
        ANDE    #$0020          ; CHECK FOR NEW HOPPER CONSTS (TARGET WTs ETC)
        BEQ     PTMBWRT74
        JSR     LDMBHPCS         ; LOAD HOPPER CONSTs FROM MB TABLE
PTMBWRT74:
        LDE     MBWORFLG        ; 'OR' FLAG - WRITE
        ANDE    #$0040          ; CHECK FOR NEW MEAS. PARAMETERS (AF1, AF2, CG, DT)
        BEQ     PTMBWRT78
        JSR     LDMBMCPS         ; LOAD MEAS. & CONTROL PARAMETERS FROM MB TABLE
PTMBWRT78:
        LDE     MBWORFLG        ; 'OR' FLAG - WRITE
        ANDE    #$0080          ; CHECK FOR SPEED CONSTANT (LINE & SCREW SPEED)
        BEQ     PTMBWRT82
        JSR     LDMBSCS         ; LOAD SPEED CONST's FROM MB TABLE
PTMBWRT82:
        LDE     MBWORFLG        ; 'OR' FLAG - WRITE
        ANDE    #$0100          ; CHECK FOR SYS. CONFIGURATION
        BEQ     PTMBWRT86
        JSR     LDMBSYC        ; LOAD SYS CONFIG. FROM MB TABLE
PTMBWRT86:
        LDE     MBWORFLG        ; 'OR' FLAG - WRITE
        ANDE    #$0200          ; CHECK FOR LIQ. CALIBRATION DATA
        BEQ     PTMBWRT89
        JSR     LDMBLIQC       ; LOAD SYS CONFIG. FROM MB TABLE
PTMBWRT89:
        LDE     MBWORFLG        ; 'OR' FLAG - WRITE
        ANDE    #$0400          ; CHECK TIME DATA
        BEQ     PTMBWRT92
        JSR     LDMBTIME       ; LOAD TIME FROM MB TABLE

PTMBWRT92:
        LDE     MBWORFLG        ; 'OR' FLAG - WRITE
        ANDE    #$1000          ; CHECK FOR TOGGLE STATUS COMMANDS
        BEQ     PTMBWRT94
        JSR     MBTOGSTA        ; ROUTINE TO ACTIVATE COMMANDS FROM MB TABLE
PTMBWRT94:
PTMBWRT95:
PTMBWRT_Q:
        CLRW    MBWORFER        ; CLEAR 'OR' ERROR FLAG
        CLRW    MBWORFLG        ; CLEAR GOOD DATA WRITE FLAG ALSO
PTMBWRT_X RTS
*/


/*
;* COPY TARGET %'s INTO MODBUS TABLE SETPOINT AREA
CPYPCTSMB:
        LDAB   #RAMBANK
        TBZK                    ;~;SET EK FOR BANK 1, I.E. RAM
        LDX     #PCNT1          ; PRODUCTION SETPOINTS, % #1
        LDZ     #MBPCNT1        ; MODBUS TABLE, % #1
        LDAB    NOBLNDS         ; NO. OF BLENDS/COMPONENTS ON MACHINE

CPYPTS10 PSHB
        LDD     0,X
        JSR     BCDHEX2         ; CONVERT TO HEX
        CPD     #9999           ; NOTE 9999 IS USED TO STORE 100.00%
        BNE     CPYPTS12
        LDD     #10000          ; 100% (NOTE 2DP)
CPYPTS12 STD     0,Z
        AIX     #2
        AIZ     #2
        PULB
        DECB
        BNE     CPYPTS10

        LDAB    #MAXCOMPONENTNO ; PLACE FOR 8 BLENDS IN MB TABLE
        SUBB    NOBLNDS         ; GET NO. OF UNUSED BLENDS
        BEQ     CPYPTS20
CPYPTS15 CLRW   0,Z             ; SET UNUSED BLEND %'s TO 0000
        AIZ     #2
        DECB
        BNE     CPYPTS15

CPYPTS20 CLRW   MBPCNTLIQ       ; LIQ. %'s
        CLRW    MBPCNTLQ2       ; AT PRESENT THERE IS NO LIQ. #2(15/7/96)
        TST     AVECPIB         ; IS THERE PIB?
        BEQ     CPYPTS30
        LDD     PCNTLIQ
        JSR     BCDHEX2
        STD     MBPCNTLIQ
CPYPTS30:
        LDD     PCT_ADD         ; % DRY ADDITIVE
        JSR     BCDHEX2
        STD     MBPCT_ADD
        RTS

;* COPY THRU-PUT DATA INTO MODBUS SETPOINTS AREA OF TABLE
CPYTPTSMB:
        LDX     #WTPMSP+1
        JSR     BCDHEX3X        ; CONVERT 3 BYTES BCD @ X TO HEX
        CLRA
        STD     MBWTPMSP        ;
        STE     MBWTPMSP+2      ;

;        LDX     #WTPMSP
;        JSR     BCDHEX3X        ; CONVERT 3 BYTES BCD @ X TO HEX
;        STE     MBWTPMSP        ; NOTE: MAX VALUE EXPECTED IS 655.35g/m

        LDAB    TKGHSP
        LDE     TKGHSP+1        ; Kg/H SETPOINT (2DP)
        JSR     SHF_BE4R        ; SHIFT[B:E] 4 PLACES TO RIGHT ( ONLY HANDLE 1DP)
        JSR     BCDHEX3         ; CONVERT TO HEX
        STE     MBTKGHSP        ; NOTE: MAX EXPECTED VALUE IS 6553.5 Kg/H

        LDX     #GPM2SPT        ; g/sq. m SETPOINT
        JSR     BCDHEX3X        ; CONVERT 3 BYTES BCD @ X TO HEX
        STE     MBGPM2SPT       ; NOTE: MAX VALUE EXPECTED IS 655.35g/m

        CLRB
        LDE     LSPDSP          ; LINE SPEED SETPOINT (m/Min) INTEGER ONLY
        JSR     SHF_BE4L        ; MBUS VERSION HAS 1 DP
        JSR     BCDHEX3
        STE     MBLSPDSP        ; NOTE: MAX EXPECTED VALUE IS 6553.5 Kg

        LDD     WIDTHSP
        JSR     BCDHEX2         ; WIDTH SPT mm
        STD     MBWIDTHSP

        LDD     TRIMWID         ; TRIMMED WIDTH (mm)
        JSR     BCDHEX2
        STD     MBTRIMWID

        RTS


*/


/*

;* LOAD TIME FROM MODBUS TABLE
LDMBTIME:

        LDX     #TIMEBUF
        LDD     MBMINSEC        ; [MIN:SEC]
        PSHA
        JSR     HEXBCD1         ; CONVERT SECS TO HEX
        STAB    SEC,X
        PULB                    ; RESTORE MINUTES
        JSR     HEXBCD1
        STAB    MIN,X

;        LDX     #TIMEBUF
        LDD     MBDAYHOUR       ; [DAY:HOUR]
        STAA    DAY,X           ; DAY DOES NOT NEED CONVERSION (1-7)
        JSR     HEXBCD1         ; CONVERT HOURS TO HEX
        STAB    HOUR,X

;        LDX     #TIMEBUF
        LDD     MBMONDATE       ; [MON:DATE]
        PSHA
        JSR     HEXBCD1         ; CONVERT DATE TO HEX
        STAB    DATE,X
        PULB                    ; RESTORE MONTH
        JSR     HEXBCD1
        STAB    MON,X

        LDD     MBYEAR
        JSR     HEXBCD2         ;
        TED                     ; MOVE RESULT -> REG D
        STAB    YEAR,X          ; YEAR
        STAA    YEAR_MSB        ; RAM ONLY.

       JSR    WRITDATE
       JSR    WRITTIME          ; WRITE TO RTC
       RTS


*/
//////////////////////////////////////////////////////
// LoadTimeFromMB                       (from asm = LDMBTIME)
//
// Load Time From MODBUS TABLE
//
// M.McKiernan                          14-06-2004
// First Pass
//
//////////////////////////////////////////////////////
void LoadTimeFromMB( void )
{
union CharsAndWord uData;

    uData.nValue = g_arrnMBTable[BATCH_TIME_MIN_SEC];
    g_CurrentTime[TIME_SECOND] = uData.cValue[1];
    g_CurrentTime[TIME_MINUTE] = uData.cValue[0];

    uData.nValue = g_arrnMBTable[BATCH_TIME_DAY_HOUR];
    if(!uData.cValue[0])
        uData.cValue[0] = 1;                                                // 0 is invalid value for day of week. - substitute 1.
    g_CurrentTime[TIME_HOUR] = uData.cValue[1];
    g_CurrentTime[TIME_DAY_OF_WEEK] = uData.cValue[0];

    uData.nValue = g_arrnMBTable[BATCH_TIME_MONTH_DATE];
    if(!uData.cValue[0])
        uData.cValue[0] = 1;                                                // 0 is invalid value for month.
    if(!uData.cValue[1])
        uData.cValue[1] = 1;                                                // 0 is invalid value for date
    g_CurrentTime[TIME_DATE] = uData.cValue[1];
    g_CurrentTime[TIME_MONTH] = uData.cValue[0];

    uData.nValue = g_arrnMBTable[BATCH_TIME_YEAR];
    g_CurrentTime[TIME_YEAR] = (WORD)(uData.nValue);
    CheckForValidTime();
     SetTime();
     SetDate();                                                             // write time and date to RTC.
}
//////////////////////////////////////////////////////
// CopyTimeToMB                     (from asm = CPYTIMMB)
//
// Copy Time into MODBUS TABLE
//
// M.McKiernan                          14-06-2004
// First Pass
//
//////////////////////////////////////////////////////
void CopyTimeToMB( void )
{
union CharsAndWord uData;
// Mins & Secs  [MINS:SECS]
    uData.cValue[1] = g_CurrentTime[TIME_SECOND];
    uData.cValue[0] = g_CurrentTime[TIME_MINUTE];
    g_arrnMBTable[BATCH_TIME_MIN_SEC] = uData.nValue;           // copy into MB table.


// Day & Hours [DAY:HOURS]
    uData.cValue[1] = g_CurrentTime[TIME_HOUR];
    uData.cValue[0] = g_CurrentTime[TIME_DAY_OF_WEEK];
    g_arrnMBTable[BATCH_TIME_DAY_HOUR] = uData.nValue;          // copy into MB table.

// Month & Date [MONTH:DATE]
    uData.cValue[1] = g_CurrentTime[TIME_DATE];
    uData.cValue[0] = g_CurrentTime[TIME_MONTH];
    g_arrnMBTable[BATCH_TIME_MONTH_DATE] = uData.nValue;            // copy into MB table.

// Year - in hex, eg 2004 = 0x07D4.
    g_arrnMBTable[BATCH_TIME_YEAR] = g_CurrentTime[TIME_YEAR];           // copy into MB table.

}

//////////////////////////////////////////////////////
// CopyLiquidCalibrationToMB                    (from asm = CPYLIQCMB)
//
// Copy Liquid calibration constants into MODBUS TABLE
//
// M.McKiernan                          04-10-2004
// First Pass
//
//////////////////////////////////////////////////////
void CopyLiquidCalibrationToMB(void)
{
union WordAndDWord uValue;

    // Liquid cal constant  -  asm = LAPCON  --REVIEW-- No. of decimals assuming 2 here.
   g_arrnMBTable[BATCH_CALIBRATION_LA_CONSTANT] = (int)(g_CalibrationData.m_fLiquidCalibrationConstant * 100);

    // Liquid Tare counts.
    uValue.lValue = g_CalibrationData.m_lLiquidTareValue;       //
    g_arrnMBTable[BATCH_CALIBRATION_LA_TARE] = uValue.nValue[0];        // M.s.Byte.
    g_arrnMBTable[BATCH_CALIBRATION_LA_TARE + 1] = uValue.nValue[1];        // M.s.Byte.

    // Liquid control lag.
    g_arrnMBTable[BATCH_CALIBRATION_LA_LAG] = g_CalibrationData.m_nLiquidControlLag;

    // Liquid signal type
    g_arrnMBTable[BATCH_CALIBRATION_LA_SIGNAL] = g_CalibrationData.m_nLiquidSignalType;

    // Liquid mode
    g_arrnMBTable[BATCH_CALIBRATION_LA_MODE] = g_CalibrationData.m_nLiquidMode;

    // Liquid control gain  Assuming 2 decimal places in MB. // LACG
    g_arrnMBTable[BATCH_CALIBRATION_LA_GAIN] = (int)(g_CalibrationData.m_fLiquidControlGain * 100.0f);

    g_arrnMBTable[BATCH_CALIBRATION_LA_CALIBRATION_KGHR] = (int)((g_CalibrationData.m_fLiquidAdditiveKgPerHour + 0.005f) * 100.0f);
    g_arrnMBTable[BATCH_CALIBRATION_LA_SCALING_FACTOR] = (int)((g_CalibrationData.m_fLiquidAdditiveScalingFactor + 0.0005f) * 1000.0f);



}
/*



*/
//////////////////////////////////////////////////////
// CopySpeedParametersToMB                      (from asm = CPYLSCMB)
//
// Copy Line & screw calibration constants into MODBUS TABLE
//
// M.McKiernan                          04-10-2004
// First Pass
//
//////////////////////////////////////////////////////
void CopySpeedParametersToMB(void)
{
    // Line speed constant  -  asm = LSC1  --REVIEW-- Will format of constant be different from previously??
   g_arrnMBTable[BATCH_CALIBRATION_LINE_SPEED_CONST] = (int)g_CalibrationData.m_fLineSpeedConstant;

    // Screw speed constant  -  asm = SSCON1  --REVIEW-- Will format of constant be different from previously??
   g_arrnMBTable[BATCH_CALIBRATION_SCREW_SPEED_CONST] = (int)g_CalibrationData.m_fScrewSpeedConstant;

    // Screw speed maximum % (1DP)  -  asm = MAXEXSP
   g_arrnMBTable[BATCH_CALIBRATION_MAX_SCREW_SPEED] = (int)(g_CalibrationData.m_fMaxExtruderSpeedPercentage * 10.0);

    // Screw speed tare counts  -  asm = SSTARE
   g_arrnMBTable[BATCH_CALIBRATION_SCREW_TARE_COUNTS] = g_CalibrationData.m_nScrewSpeedTareCounts;

}
/*

*/

//////////////////////////////////////////////////////
// CopySystemConfigurationToMB                      (from asm = CPYSYCMB)
//
// Copy System Configuration into MODBUS TABLE
//
// M.McKiernan                          30-09-2004
// First Pass
//
//////////////////////////////////////////////////////
void CopySystemConfigurationToMB( void )
{
unsigned int nTemp,i;
float fPercentage,fValue;

    // No. of components(blends) asm = NOBLNDS
    g_arrnMBTable[BATCH_CALIBRATION_COMPONENTS] = g_CalibrationData.m_nComponents;

    // Fill retries = asm = FILLRETRY
   g_arrnMBTable[BATCH_CALIBRATION_FILL_RETRIES] = g_CalibrationData.m_nFillRetries;

   // Blender mode asm = BLENDERMODE
   g_arrnMBTable[BATCH_CALIBRATION_BLENDER_MODE] = g_CalibrationData.m_nBlenderMode;

   // control mode - asm = CRLMODE
   g_arrnMBTable[BATCH_CALIBRATION_CONTROL_MODE] = g_CalibrationData.m_nControlMode;

    // Line no. asm = LINENO
   g_arrnMBTable[BATCH_CALIBRATION_LINE_NUMBER] = g_CalibrationData.m_nLineNumber;

    // Regrind component no. asm = REGCOMP
    // --REVIEW-- will the regrind component no. be coming from the recipe from now on??
   g_arrnMBTable[BATCH_CALIBRATION_REGRIND_COMPONENT] = g_CalibrationData.m_nRegrindComponentNumber;

    // Fast control response.. asm = FSTCTRL
    if(g_CalibrationData.m_bFastControlResponse)
    g_arrnMBTable[BATCH_CALIBRATION_FAST_CONTROL] = 0;
   else
    g_arrnMBTable[BATCH_CALIBRATION_FAST_CONTROL] = 1;

    // Diagnostics mode  -  asm = DIAGMODE
   g_arrnMBTable[BATCH_CALIBRATION_DIAG_MODE] = g_CalibrationData.m_nDiagnosticsMode;

    // Vacuum Loading  -  asm = VACLOAD
/*
    if(g_CalibrationData.m_bVacLoading)
    {
        g_arrnMBTable[BATCH_CALIBRATION_VAC_LOADING] = 1;
    }
    else
    {
        g_arrnMBTable[BATCH_CALIBRATION_VAC_LOADING] = 0;
    }
*/
   //wfh MMK -23.4.2020.
   if(1)
	   g_arrnMBTable[CALIBRATION_MACHINE_TYPE] = MACHINE_TYPE_WIDTH_CONTROL;  //width control software,  fix machine type to width

    //--REVIEW-- bypass now in CONFIG OPTIONs word.
    // Bypass  -  asm = BYPASSE
//   g_arrnMBTable[BATCH_CALIBRATION_BYPASS] = g_CalibrationData.m_bBypassModeFlag;



    // Volumetric mode  -  asm = ISVOLMODE
   g_arrnMBTable[BATCH_CALIBRATION_VOLUMETRIC_MODE] = g_CalibrationData.m_bVolumetricModeFlag;

    // Serial EI fitted  -  asm = EIOMITTED
   g_arrnMBTable[BATCH_CALIBRATION_SEI_INSTALLED] = g_CalibrationData.m_nPeripheralCardsPresent;

    // Liquid Additive  -  asm = AVECPIB
   g_arrnMBTable[BATCH_CALIBRATION_PIB_ENABLE] = g_CalibrationData.m_bLiquidEnabled;

    // Blender type  -  asm = BLENDERTYPE
   g_arrnMBTable[BATCH_CALIBRATION_BLENDER_TYPE] = g_CalibrationData.m_nBlenderType;

    // Shutdown enable  -  asm = SHUTDOWNEN
   g_arrnMBTable[BATCH_CALIBRATION_EXTR_SHUTDOWN] = g_CalibrationData.m_bShutDownFlag;

    // Shutdown time -  asm = SHUTDOWNTM
   g_arrnMBTable[BATCH_CALIBRATION_SHUTDOWN_TIME] = g_CalibrationData.m_nShutDownTime;

    // Units type  -  asm = UNITTYPE
   g_arrnMBTable[BATCH_CALIBRATION_MEASUREMENT_UNITS] = g_CalibrationData.m_nUnitType;

    // Screw speed signal  -  asm = SSSTYPE
   g_arrnMBTable[BATCH_CALIBRATION_SPEED_SIGNAL] = g_CalibrationData.m_nScewSpeedSignalType;

    // Extruder stall enable  -  asm = EXSENAB
   g_arrnMBTable[BATCH_CALIBRATION_STALL_CHECK] = g_CalibrationData.m_bExtruderStallCheck;

    // Level sensor delay  -  asm = LSDELAY
   g_arrnMBTable[BATCH_CALIBRATION_DELAY_TO_LS_ALARM] = g_CalibrationData.m_nLevelSensorDelay;

    // Offline type  -  asm = OFFLINETYPE
   g_arrnMBTable[BATCH_CALIBRATION_OFFLINE_TYPE] = g_CalibrationData.m_nOfflineType;

    // Purge component no.  -  asm = PURGECOMPNO
   g_arrnMBTable[BATCH_CALIBRATION_PURGE_COMPONENT] = g_CalibrationData.m_nPurgeComponentNumber;


    // Control type  -  asm = CONTROLTYPE
   g_arrnMBTable[BATCH_CALIBRATION_CONTROL_TYPE] = g_CalibrationData.m_nControlType;

    // Control Lag.  -  asm = CONTROLLAG
   g_arrnMBTable[BATCH_CALIBRATION_CONTROL_LAG] = g_CalibrationData.m_nControlLag;

    // Increase rate  -  asm = INCREASERATE
   g_arrnMBTable[BATCH_CALIBRATION_INC_RATE] = g_CalibrationData.m_nIncreaseRate;

    // Decrease rate  -  asm = DECREASERATE
   g_arrnMBTable[BATCH_CALIBRATION_DEC_RATE] = g_CalibrationData.m_nDecreaseRate;

    // Control Deadband  -  asm = DEADBAND
   g_arrnMBTable[BATCH_CALIBRATION_DEADBAND] = g_CalibrationData.m_nControlDeadband;

    // Fine inc-dec rate  -  asm = FINEINCDEC
   g_arrnMBTable[BATCH_CALIBRATION_FINE_INC_DEC] = g_CalibrationData.m_nFineIncDec;

    // Max component percentage  -  asm = MAXPCNT1 - 12
    for(i = 0;  i < MAX_COMPONENTS; i++ )
    {
        g_arrnMBTable[BATCH_CALIBRATION_MAX_PERCENT_1 + i] = 0; //ZERO INITIALLY
    }

    for(i = 0; i < g_CalibrationData.m_nComponents && i < MAX_COMPONENTS; i++ )
    {
        fPercentage = g_CalibrationData.m_fMaxPercentage[i];
        fPercentage += 0.005;       // for rounding to nearest 2nd decimal digit.
        g_arrnMBTable[BATCH_CALIBRATION_MAX_PERCENT_1 + i] = (int)(fPercentage * 100);      // NB: Percentages are to 2DP in MB table. (9999 = 99.99%)
    }

    // Stage Fill enable  -  asm = STAGEFILLEN
   g_arrnMBTable[BATCH_CALIBRATION_STAGE_FILL_EN] = g_CalibrationData.m_nStageFillEn;

    // Moulding  -  asm = moulding
   g_arrnMBTable[BATCH_CALIBRATION_MOULDING] = g_CalibrationData.m_bMouldingOption;

//Clean stuff
    // Phase 1 On time  -  asm = PH1ONTIME
   g_arrnMBTable[BATCH_CALIBRATION_PHASE_1_ON_TIME] = g_CalibrationData.m_nPhase1OnTime;
    // Phase 1 Off time  -  asm = PH1OFFTIME
   g_arrnMBTable[BATCH_CALIBRATION_PHASE_1_OFF_TIME] = g_CalibrationData.m_nPhase1OffTime;

    // Phase 2 On time  -  asm = PH2ONTIME
   g_arrnMBTable[BATCH_CALIBRATION_PHASE_2_ON_TIME] = g_CalibrationData.m_nPhase2OnTime;
    // Phase 2 Off time  -  asm = PH2OFFTIME
   g_arrnMBTable[BATCH_CALIBRATION_PHASE_2_OFF_TIME] = g_CalibrationData.m_nPhase2OffTime;

    // Blender clean repetitions  -  asm = BCLEANREPETITIONS
   g_arrnMBTable[BATCH_CALIBRATION_CLEAN_REPS] = g_CalibrationData.m_nCleanRepetitions;

    // Mixer clean repetitions  -  asm = MCLEANREPETITIONS
   g_arrnMBTable[BATCH_CALIBRATION_MIX_CLEAN_REPS] = g_CalibrationData.m_nMixerCleanRepetitions;

    // Bypass time limit  -  asm = BYPASSTIMELIMIT
   g_arrnMBTable[BATCH_CALIBRATION_BYPASS_TIME_LIMIT] = g_CalibrationData.m_nBypassTimeLimit;

    // Top up %  -  asm = PERTOPUP
   g_arrnMBTable[BATCH_CALIBRATION_TOPUP_PERCENT] = g_CalibrationData.m_nPercentageTopUp;

    // Bypass time limit clean -  asm = BYPASSDCLNTIMELIMIT
   g_arrnMBTable[BATCH_CALIBRATION_BYP_TIME_LIMIT_CLN] = g_CalibrationData.m_nBypassTimeLimitClean;

    // Max batch size, relates to top up mode -  asm = MAXGRAMSPERPER  --REVIEW-- kgs to 1dp.
   g_arrnMBTable[BATCH_CALIBRATION_MAX_BATCH_SIZE_KGS] = (int)(10 * g_CalibrationData.m_fMaxBatchSizeInKg); //kg to 1dp


    // Top up comp no. ref  asm = TOPUPCOMPNOREF
   g_arrnMBTable[BATCH_CALIBRATION_TOPUP_COMP_REF] = g_CalibrationData.m_nTopUpCompNoRef;


    // Intermittent Mix time  -  asm = INTMIXTIME   //--REVIEW--    not sure have right time here!!
   g_arrnMBTable[BATCH_CALIBRATION_INT_MIX_TIME] = g_CalibrationData.m_nIntermittentMixTime;


    // Shut off time asm = MIXINGSHUTOFF
   g_arrnMBTable[BATCH_CALIBRATION_MIXING_SHUTOFF_TIME] = g_CalibrationData.m_nMixShutoffTime;

   fValue =     g_CalibrationData.m_nMaxThroughput;
   if(g_fWeightConversionFactor != 1.0f)
   fValue *=    g_fWeightConversionFactor;
   fValue += 0.05;
   g_arrnMBTable[BATCH_CALIBRATION_MAXIMUM_THROUGHPUT] = (int)(fValue);

    // Offline mixing time asm MIXTIM
   g_arrnMBTable[BATCH_CALIBRATION_OFFLINE_MIXING_TIME] = g_CalibrationData.m_nMixingTime;

    // Vacuum delay used in single recipe mode ASM = VACUUMDELAY
   g_arrnMBTable[BATCH_CALIBRATION_MATERIAL_TRANSPORT_DELAY] = g_CalibrationData.m_nMaterialTransportDelay;

    // Post Vacuum delay used in single recipe mode ASM = POSTVACUUMDELAY
   g_arrnMBTable[BATCH_CALIBRATION_POST_VACUUM_DELAY] = g_CalibrationData.m_nPostMaterialTransportDelay;




//passwords
    // Password  -  asm = PASWRD
   g_arrnMBTable[BATCH_CALIBRATION_PASSWORD_SUPERVISOR] = g_CalibrationData.m_lManagerPassWord;
    // Password #2  -  asm = PASWRD2
   g_arrnMBTable[BATCH_CALIBRATION_PASSWORD_OPERATOR] = g_CalibrationData.m_lOperatorPassWord;
    // Password #3  -  TSM or Engineering
   g_arrnMBTable[BATCH_CALIBRATION_PASSWORD_TSM] = (WORD)g_CalibrationData.m_lTSMPassWord;

   g_arrnMBTable[BATCH_MIN_OPENING_TIME_CHECK] = (WORD)g_CalibrationData.m_bMinimumOpeningTimeCheck;

    g_arrnMBTable[BATCH_FLOW_RATE_CHECK] = (WORD)g_CalibrationData.m_bFlowRateCheck;

     g_arrnMBTable[BATCH_VAC8_EXPANSION] = (WORD)g_CalibrationData.m_bVac8Expansion;



// Config options.
    nTemp = 0x0000;
    // High/low regrind
    if(g_CalibrationData.m_bHighLoRegrindOption)    // asm = HILOREGENABLED
        nTemp |= BATCH_CONFIG_HILO_REG_BIT;

    // Max component % check enabled??
    if(g_CalibrationData.m_bMaxComponentPercentageCheck)    // asm = MAXPCNTCHECK
        nTemp |= BATCH_CONFIG_MAX_COMP_PER_EN_BIT;

    // First component compensation enabled?
    if(!g_CalibrationData.m_bFirstComponentCompensation) // asm = FIRSTCOMPCOMPENDIS
        nTemp |= BATCH_CONFIG_FIRST_COMP_COMPENSATION_BIT;

    // Layering enable?
    if(g_CalibrationData.m_bLayering)   // asm = LAYERING
        nTemp |= BATCH_CONFIG_LAYERING_BIT;

    // Mixing screw detection.
    if(g_CalibrationData.m_bMixerScrewDetect)   // asm = MIXSCREWDETECTION.
        nTemp |= BATCH_CONFIG_MIXER_DETECT_BIT;

    // Double dump
    if(g_CalibrationData.m_bDoubleDump) // asm = DOUBLEDUMP
        nTemp |= BATCH_CONFIG_DOUBLE_DUMP_BIT;

    // Regrind retry.
    if(g_CalibrationData.m_bRegrindRetry)   // asm = REGRINDRETRY
        nTemp |= BATCH_CONFIG_REGRIND_RETRY_BIT;


    // Bypass mode enable
    if(g_CalibrationData.m_bBypassMode) // asm = BYPASSMODE
        nTemp |= BATCH_CONFIG_BYPASS_EN_BIT;

    // Manual fill sort enable  --REVIEW-- Manual sorting not actually used in current version.
    if(g_CalibrationData.m_bFillOrderManualSelect)  // asm = MANUALFILLSORT
        nTemp |= BATCH_CONFIG_FILL_MAN_SORT_BIT;

    // Top-up enable.
   if(g_CalibrationData.m_bTopUpMode)          // asm = TOPUP
        nTemp |= BATCH_CONFIG_TOPUP_EN_BIT;

    // Blow every cycle (clean function)
    if(g_CalibrationData.m_bBlowEveryCycle) // asm = BLOWEVERYCYCLE
        nTemp |= BATCH_CONFIG_BLOW_EVERY_CYCLE_BIT;

    // Cleaning enable
    if(g_CalibrationData.m_nCleaning)           // asm = CLEANING
        nTemp |= BATCH_CONFIG_CLEAN_EN_BIT;

    // Single word write enable
    if(g_CalibrationData.m_bSingleWordWrite)            // asm = SINGLEWORDWRITE
        nTemp |= BATCH_CONFIG_SINGLE_WORD_WRITE_BIT;

    if(!g_CalibrationData.m_bCycleFastFlag)
        nTemp |= BATCH_CONFIG_FAST_SLOW_CYCLE_BIT;


   g_arrnMBTable[BATCH_CALIBRATION_CONFIG_OPTIONS] = nTemp;


//--review-- if fast / slow option is required

    nTemp = 0;

    // Use regrind as reference
    if(g_CalibrationData.m_nUseRegrindAsReference)            // asm = USEREGRINDASREF
        nTemp |= BATCH_CALIBRATION_USE_REGRIND_AS_REF_BITPOS;   //copy to modbus table

    if(g_CalibrationData.m_bSendEmailOnOrderChange)
        nTemp |= BATCH_CALIBRATION_SEND_EMAIL_ON_ORDER_CHANGE_BITPOS;
    g_arrnMBTable[BATCH_CALIBRATION_OPTIONS_2] = nTemp;

                    //


// Printing options:


// Config options.
    nTemp = 0x0000;
    // Correct every n cycles

    if(!g_CalibrationData.m_bCorrectEveryNCycles)    // asm = CORRECTEVERYNCYCLES
        nTemp |= BATCH_CAL_OPTIONS_CORRECTEVERYCYCLEBITPOS;

       g_arrnMBTable[BATCH_CALIBRATION_OPTIONS] = nTemp;


    //--REVIEW-- in assembler was in the meas & control parameters section
    // control band/ control % limit. CONTROLPER
    g_arrnMBTable[BATCH_CALIBRATION_CONTROL_BAND] = g_CalibrationData.m_nControlPercentageLimit;

    g_arrnMBTable[BATCH_CALIBRATION_NETWORK_BAUD_RATE] = g_CalibrationData.m_nBaudRate;

    g_arrnMBTable[BATCH_CALIBRATION_LATENCY] = (int)(g_CalibrationData.m_fLatency * 1000.0);

    g_arrnMBTable[BATCH_CALIBRATION_LINE_SPEED_SIGNAL_TYPE] = g_CalibrationData.m_nLineSpeedSignalType;

    g_arrnMBTable[BATCH_CALIBRATION_ENTRY_TYPE] = g_CalibrationData.m_nRecipeEntryType;

     g_arrnMBTable[BATCH_CALIBRATION_CLEANING] = g_CalibrationData.m_nCleaning;

    g_arrnMBTable[BATCH_CALIBRATION_PURGE] = g_CalibrationData.m_nPurgeEnable;

    g_arrnMBTable[BATCH_CALIBRATION_LINE_SPEED_AVERAGE] = g_CalibrationData.m_nLineSpeedAverage;

    g_arrnMBTable[BATCH_CALIBRATION_COMPONENT_NAMING_ALPHANUMERIC] = g_CalibrationData.m_bComponentNamingAlphanumeric;

    g_arrnMBTable[BATCH_OPTIMISATION] = g_CalibrationData.m_bOptimisation;

    g_arrnMBTable[BATCH_DEVIATION_ALARM_ENABLE] = g_CalibrationData.m_nDeviationAlarmEnable;

    g_arrnMBTable[BATCH_LOG_TO_SD_CARD] = (WORD)g_CalibrationData.m_nLogToSDCard;

    if(g_CalibrationData.m_bPauseBlenderOnPowerup)
    {
        g_arrnMBTable[BATCH_PAUSE_BLENDER_ON_POWER_UP] = 1;
    }
    else
    {
        g_arrnMBTable[BATCH_PAUSE_BLENDER_ON_POWER_UP] = 0;
    }
    if(g_CalibrationData.m_bLogFormatIsCSV)
    {
        g_arrnMBTable[BATCH_LOG_FILE_TYPE_CSV] = 1;
    }
    else
    {
        g_arrnMBTable[BATCH_LOG_FILE_TYPE_CSV] = 0;
    }

    if(g_CalibrationData.m_bAllowForContinuousDownload)
    {
        g_arrnMBTable[BATCH_CALIBRATION_ALLOW_CONTINUOUS_DOWNLOAD] = 1;
    }
    else
    {
        g_arrnMBTable[BATCH_CALIBRATION_ALLOW_CONTINUOUS_DOWNLOAD] = 0;
    }

    if(g_CalibrationData.m_bTelnet)
    {
        g_arrnMBTable[BATCH_CALIBRATION_TELNET] = 1;
    }
    else
    {
        g_arrnMBTable[BATCH_CALIBRATION_TELNET] = 0;
    }


    for(i = 0; i < MAX_COMPONENTS; i++ )
    {
        g_arrnMBTable[BATCH_OPTIMISATION_VOLUME_1+i] = g_CalibrationData.m_nComponentBinVolume[i];
    }

    for(i = 0; i < MAX_COMPONENTS; i++ )
    {
        g_arrnMBTable[BATCH_CALIBRATION_MAX_PERCENTAGE_DEVIATION_1+i] = (int)((g_CalibrationData.m_fMaxPercentageDeviation[i] + 0.005f) * 100.0f);
    }

    g_arrnMBTable[BATCH_CALIBRATION_MAX_PAUSE_ON_ERROR_NO] = g_CalibrationData.m_nPauseOnErrorNo;

    if(g_CalibrationData.m_bNoParity)
    {
        g_arrnMBTable[BATCH_COMMUNICATIONS_NO_PARITY] = 1;
    }
    else
    {
        g_arrnMBTable[BATCH_COMMUNICATIONS_NO_PARITY] = 0;
    }
    if(g_CalibrationData.m_bCheckForValidComponentTargetPercentage)
    {
        g_arrnMBTable[BATCH_CHECK_FOR_VALID_COMPONENT_PER_TARGET] = 1;
    }
    else
    {
        g_arrnMBTable[BATCH_CHECK_FOR_VALID_COMPONENT_PER_TARGET] = 0;
    }
    for(i = 0; i < MAX_COMPONENTS; i++ )
    {
        g_arrnMBTable[BATCH_FILLING_METHOD_COMPONENT_1+i] = g_CalibrationData.m_wFillingMethod[i];
    }


}
//////////////////////////////////////////////////////
// LoadComponentConfigFromMB                    (from asm = LDMBCOMPCFG)
//
// Load Component Configuration from the MODBUS TABLE
//
// M.McKiernan                          11-10-2004
// First Pass
//
//////////////////////////////////////////////////////
void LoadComponentConfigFromMB( void )
{
int i;
   // STDCCFG  Standard component config (YES/NO)STDCCFG
   if(g_arrnMBTable[BATCH_CALIBRATION_COMP_CONFIG])
      g_CalibrationData.m_bStandardCompConfig = FALSE;  // 1 => non-standard in Modbus
   else
      g_CalibrationData.m_bStandardCompConfig = TRUE;   // 0 in MB => standard comp config.


    if(!g_CalibrationData.m_bStandardCompConfig)        //i.e. only load comp. config's if non-standard.
    {
        // copy each component config's from MB  - COMP1CFG
        for(i=0; i < MAX_COMPONENTS && i < 12; i++)     // nb max 12 contiguous words in MB available.
        {
            g_CalibrationData.m_nCompConfig[i] = g_arrnMBTable[BATCH_CALIBRATION_COMP1_CONFIG + i];
        }
    }

        // --REVIEW-- do we want to save off calibration data in chunks??

        g_bSaveAllCalibrationToEEprom = TRUE;       // set flag to cause all calibration data to be written to EEPROM.
}

//////////////////////////////////////////////////////
// CopySystemConfigurationToMB
//
// Copy System Configuration into MODBUS TABLE
//
//////////////////////////////////////////////////////
void CopySystemConfiguration3ToMB( void )
{

        // printer type
   //g_arrnMBTable[BATCH_CONFIG_PRINTER_TYPE] = g_CalibrationData.m_nPrinterType;
        // dump device type
   g_arrnMBTable[BATCH_CONFIG_PRINTER_USB_ENABLE] = g_CalibrationData.m_nDumpDeviceType;
        // alarms dump enable
    if(g_CalibrationData.m_bAlarmDumpFlag)
    {
        g_arrnMBTable[BATCH_CONFIG_ALARMS_DUMP_ENABLE] = 1;
    }
    else
    {
        g_arrnMBTable[BATCH_CONFIG_ALARMS_DUMP_ENABLE] = 0;
    }

        // order dump enable
    if(g_CalibrationData.m_bOrderReportDumpFlag)
    {
        g_arrnMBTable[BATCH_CONFIG_ORDER_DUMP_ENABLE] = 1;
    }
    else
    {
        g_arrnMBTable[BATCH_CONFIG_ORDER_DUMP_ENABLE] = 0;
    }

        // roll dump enable
    if(g_CalibrationData.m_bRollReportDumpFlag)
    {
        g_arrnMBTable[BATCH_CONFIG_ROLL_DUMP_ENABLE] = 1;
    }
    else
    {
        g_arrnMBTable[BATCH_CONFIG_ROLL_DUMP_ENABLE] = 0;
    }

        // hourly dump enable
    if(g_CalibrationData.m_bHourlyDumpFlag)
    {
        g_arrnMBTable[BATCH_CONFIG_HOURLY_DUMP_ENABLE] = 1;
    }
    else
    {
        g_arrnMBTable[BATCH_CONFIG_HOURLY_DUMP_ENABLE] = 0;
    }

        // batch log dump enable
    if(g_CalibrationData.m_bBatchLogDumpFlag)
    {
        g_arrnMBTable[BATCH_CONFIG_BATCH_LOG_DUMP_ENABLE] = 1;
    }
    else
    {
        g_arrnMBTable[BATCH_CONFIG_BATCH_LOG_DUMP_ENABLE] = 0;
    }
        // event log dump enable
    if(g_CalibrationData.m_bEventLogDumpFlag)
    {
        g_arrnMBTable[BATCH_CONFIG_EVENT_LOG_DUMP_ENABLE] = 1;
    }
    else
    {
        g_arrnMBTable[BATCH_CONFIG_EVENT_LOG_DUMP_ENABLE] = 0;
    }
         // history log dump enable
    if(g_CalibrationData.m_bHistoryLogDumpFlag)
    {
        g_arrnMBTable[BATCH_CONFIG_HISTORY_LOG_DUMP_ENABLE] = 1;
    }
    else
    {
        g_arrnMBTable[BATCH_CONFIG_HISTORY_LOG_DUMP_ENABLE] = 0;
    }

        // history log dump enable
         // history log dump enable
    if(g_CalibrationData.m_bShiftReportDumpFlag)
    {
        g_arrnMBTable[BATCH_CONFIG_SHIFT_DUMP_ENABLE] = 1;
    }
    else
    {
        g_arrnMBTable[BATCH_CONFIG_SHIFT_DUMP_ENABLE] = 0;
    }

}

//////////////////////////////////////////////////////
// LoadWeightCalibrationFromMB                      (from asm = LDMBWCS)
//
// Load Weight calibration from the MODBUS TABLE
//
// M.McKiernan                          12-10-2004
// First Pass
//
//////////////////////////////////////////////////////
void LoadWeightCalibrationFromMB( void )
{
 int nCalCode;

    if(g_CalibrationData.m_nComponents != 0)    // must not be 0, 0 illegal no.
    {
        nCalCode = g_arrnMBTable[BATCH_CALIBRATION_CODE];   // wt. calibration code
        if(nCalCode & BATCH_TARE_HOPPER)    // Tare bit set??
        {
            TareWeighHopper();  // TAREWEIGHHOPPER
            CopyWeightConstantsToMB();  // copy new weight contstants into MB.
            // --REVIEW-- need routine in foreground to do right thing (save data probably to EEPROM)
            g_bCalibrationDataChange = TRUE;    //CALFLG

            nCalCode = BATCH_TARE_HOPPER ^ 0xFFFF;      // get complement of tare bit
            g_arrnMBTable[BATCH_CALIBRATION_CODE] &= nCalCode;  // clear the tare bit.
            // --REVIEW-- do we want to save off calibration data in chunks??
            g_bSaveAllCalibrationToEEprom = TRUE;       // set flag to cause all calibration data to be written to EEPROM.

        }
    }
}

//////////////////////////////////////////////////////
// LoadHopperCalibrationFromMB                      (from asm = LDMBHPCS)
//
// Load Hopper calibration from the MODBUS TABLE
//
// M.McKiernan                          12-10-2004
// First Pass
//
//////////////////////////////////////////////////////
void LoadHopperCalibrationFromMB( void )
{
float   fValue;
//
    if(g_bConfigIsLocked)
    {
        CopyHopperConstantsToMB();
    }
    else
    {
    // Batch size; 1 decimal places in MB. // DFTW1
    fValue = (float)g_arrnMBTable[BATCH_CALIBRATION_BATCH_SIZE];
    if(g_fWeightConversionFactor != 1.0f)
    fValue /= g_fWeightConversionFactor;
    g_CalibrationData.m_fBatchSize = fValue / 10.0f;                                  // Throughput is to 1dp in MB.


    // --REVIEW-- do we want to save off calibration data in chunks??
    g_bSaveAllCalibrationToEEprom = TRUE;       // set flag to cause all calibration data to be written to EEPROM.

    g_bFillTargetEntryFlag = TRUE;        // SET FILL TARGET
    g_bResetComponentWeightsRoundRobin = TRUE; //  ASM JSR     RESETALLWGTRROBIN

    int nResult = 0;
    nResult = CheckForValidCalibrationData(FALSE); // check that the calibration data is valid.
    }
}

//////////////////////////////////////////////////////
// LoadSpeedParametersFromMB                    (from asm = LDMBSCS)
//
// Load line and screw speed parameters from the MODBUS TABLE
//
// M.McKiernan                          12-10-2004
// First Pass
//
//////////////////////////////////////////////////////
void LoadSpeedParametersFromMB( void )
{
    // Line speed constant  -  asm = LSC1  --REVIEW-- Will format of constant be different from previously??
   g_CalibrationData.m_fLineSpeedConstant = (float)g_arrnMBTable[BATCH_CALIBRATION_LINE_SPEED_CONST];

    // Screw speed maximum % (1DP)  -  asm = MAXEXSP
   g_CalibrationData.m_fMaxExtruderSpeedPercentage = (float)g_arrnMBTable[BATCH_CALIBRATION_MAX_SCREW_SPEED] / 10.0f;
    if(g_CalibrationData.m_fMaxExtruderSpeedPercentage > 99.9)
        g_CalibrationData.m_fMaxExtruderSpeedPercentage = 99.9f;    // limit maximum to 99.9%

//  NMAXSS = 0xAA;  // Wasn't actually doing anything in assembler.
    //--REVIEW--
    g_bNewMaxScrewSpeed = TRUE;   //    NMAXSS = 0xAA should trigger call to CMXDA1 in ProgLoop.

    if(g_CalibrationData.m_nBlenderMode != MODE_KGHCONTROLMODE)
    {
        g_bCSAFlag = TRUE; // Set flag to calc. new kg/h setpoint
    }

    // --REVIEW-- do we want to save off calibration data in chunks??
    g_bSaveAllCalibrationToEEprom = TRUE;       // set flag to cause all calibration data to be written to EEPROM.

}


//////////////////////////////////////////////////////
// LoadMeasParametersFromMB                     (from asm = LDMBMCPS)
//
// Load Measurement & control parameters from the MODBUS TABLE
//
// M.McKiernan                          12-10-2004
// First Pass
//
//////////////////////////////////////////////////////
void LoadMeasParametersFromMB( void )
{
    // control average factor  // CONTAVGFACTOR
    g_CalibrationData.m_nControlAvgFactor = g_arrnMBTable[BATCH_CALIBRATION_CONTROL_AVERAGE];


    //--review-- downloaded from measurement & control parameter page weight average factor    // WGTAVGFACTOR
    g_CalibrationData.m_nWeightAvgFactor = g_arrnMBTable[BATCH_CALIBRATION_WEIGHT_AVERAGE];


    // Mixing time  // MIXTIM
    g_CalibrationData.m_nMixingTime = g_arrnMBTable[BATCH_CALIBRATION_MIXING_TIME];

    // Batches in kg/h average. // BATINKGHAVG
    g_CalibrationData.m_nBatchesInKghAvg = g_arrnMBTable[BATCH_CALIBRATION_BATCHES_KGH_AVERAGE];

    // control gain Assuming 2 decimal places in MB. // CLGAIN
    g_CalibrationData.m_fControlGain = (float)g_arrnMBTable[BATCH_CALIBRATION_CONTROL_GAIN] / 100.0f;

    g_nDACPerKgDoneFlag = 0;    // DACPKGDONEFLAG    Reset this flag to ensure no volset correction.        ;

    // --REVIEW-- do we want to save off calibration data in chunks??
    g_bSaveAllCalibrationToEEprom = TRUE;       // set flag to cause all calibration data to be written to EEPROM.


}

//////////////////////////////////////////////////////
// LoadLiquidCalibrationFromMB                      (from asm = LDMBMCPS)
//
// Load Liquid calibration parameters from the MODBUS TABLE
//
// M.McKiernan                          12-10-2004
// First Pass
//
//////////////////////////////////////////////////////
void LoadLiquidCalibrationFromMB( void )
{
union WordAndDWord uValue;

    // Liquid cal constant  -  asm = LAPCON  --REVIEW-- No. of decimals assuming 2 here.
   g_CalibrationData.m_fLiquidCalibrationConstant = (float)g_arrnMBTable[BATCH_CALIBRATION_LA_CONSTANT] / 100.0f;

    // Liquid Tare counts.
    uValue.nValue[0] = g_arrnMBTable[BATCH_CALIBRATION_LA_TARE];        // M.s.Byte.
    uValue.nValue[1] = g_arrnMBTable[BATCH_CALIBRATION_LA_TARE + 1];        // M.s.Byte.
    g_CalibrationData.m_lLiquidTareValue = uValue.lValue;                   // move into calibration block.

    // Liquid control lag.
    g_CalibrationData.m_nLiquidControlLag = g_arrnMBTable[BATCH_CALIBRATION_LA_LAG];

    // Liquid signal type
    g_CalibrationData.m_nLiquidSignalType = g_arrnMBTable[BATCH_CALIBRATION_LA_SIGNAL];

    // Liquid mode
    g_CalibrationData.m_nLiquidMode = g_arrnMBTable[BATCH_CALIBRATION_LA_MODE];

    // Liquid control gain  Assuming 2 decimal places in MB. // LACG
    g_CalibrationData.m_fLiquidControlGain = (float)g_arrnMBTable[BATCH_CALIBRATION_LA_GAIN] / 100.0f;

    g_CalibrationData.m_fLiquidAdditiveKgPerHour = ((float)g_arrnMBTable[BATCH_CALIBRATION_LA_CALIBRATION_KGHR])/100.0f;
    g_CalibrationData.m_fLiquidAdditiveScalingFactor = ((float)g_arrnMBTable[BATCH_CALIBRATION_LA_SCALING_FACTOR])/1000.0f;


    // --REVIEW-- do we want to save off calibration data in chunks??
    g_bSaveAllCalibrationToEEprom = TRUE;       // set flag to cause all calibration data to be written to EEPROM.


}

//////////////////////////////////////////////////////
// LoadCalibrationWeightFromMB                      (from asm = LDMBHOPPERCAL, and integrates CALWGTCONST)
//
// Load  calibration weight from the MODBUS TABLE
// Assembler set CALWGTCON, which in turn called CALWGTCONST which calculated the wt. constant,
// set the PCTFLG flag, and copied the weight constants into MB table.
//
// M.McKiernan                          13-10-2004
// First Pass
//
//////////////////////////////////////////////////////
void LoadCalibrationWeightFromMB( void )
{
float fCalibrationWeight,fTemp;
    if(g_bAutoCycleFlag || g_bStopped)  // must be stopped or in auto-cycle mode
    {
        // pick up weight from MB table. - 3DP's
        fTemp = (float)g_arrnMBTable[BATCH_CALIBRATION_CALIB_WEIGHT] /1000.0;
        if(g_fWeightConversionFactor != 1.0f)
        fTemp /= g_fWeightConversionFactor;
        fCalibrationWeight = fTemp;
        g_fCalibrationWeight = fTemp;

      if(g_lOneSecondWeightAverage > g_CalibrationData.m_lTareCounts)         // dont use unless there is a positive weight.
      {
         if(fTemp > 0.0001)
         {
             RemoveAlarmTable( HOPPERCALIBRATEDATZEROALARM,  0 );
             g_CalibrationData.m_fWeightConstant = (float)(g_lOneSecondWeightAverage - g_CalibrationData.m_lTareCounts) / fCalibrationWeight;
             if(g_bDiagnosticsPasswordInUse)
             {
                 g_CalibrationData.m_fFactoryWeightConstant = g_CalibrationData.m_fWeightConstant;
             }
             g_bPctFlag = TRUE;
             CopyWeightConstantsToMB();  // copy new weight contstants into MB.
            // --REVIEW-- need routine in foreground to do right thing (save data probably to EEPROM)
            g_bCalibrationDataChange = TRUE;    //CALFLG

            // --REVIEW-- do we want to save off calibration data in chunks??
            g_bSaveAllCalibrationToEEprom = TRUE;       // set flag to cause all calibration data to be written to EEPROM.
            g_bWeighHopperCalibratedEvent = TRUE;
            g_nSDCardDelayTime = SDCARDCOMMANDDELAYTIME; // delay before event initiated

//         SaveHopperCalibration( );     //save hopper calibration to eeprom
            }
            else
            {
                PutAlarmTable( HOPPERCALIBRATEDATZEROALARM,  0 );
            }

        }
    }


}
/*
; LOADS THE HOPPER CALIBRATION TO MODBUS TABLE AND INITIATES WEIGHT CALIBRATION.
;
LDMBHOPPERCAL:

       TST      AUTOCYCLEFLAG   ; AUTOCYCLER.
       BNE      LDHPCL          ; CALIBRATE
       TST      STOPPED ;
       LBEQ      XITHPC
LDHPCL LDD      MB1CON1
       JSR      HEXBCD2         ;
       JSR      SHF_BE4L         ; SHIFT 4 PLACES LEFT, (*10), PROD. HAS 2DP
       STAB     NKEYBUF+2
       STE      NKEYBUF+3       ;
       LDAA     #MODBUSWEIGHTCAL
       STAA     CALWGTCON         ; INITITIATE CALIBRATION.
XITHPC RTS

MODBUSWEIGHTCAL EQU     $AB

*/
/*

;* LOAD LIQUID CONSTS FROM MODBUS TABLE INTO CALIBRATION DATA AREA
LDMBLIQC:
        LDAA   MANATO
        BEQ    LDMBLQ04          ;~BRANCH IF IN MANUAL
        CLR    MANATO           ;~ELSE SET FOR MANUAL MODE
        JSR    EXCTRL           ;~EXIT AUTO CONTROL IF IT WAS IN AUTO
LDMBLQ04:
        LDD     MBLCON1
        JSR     HEXBCD2
        STAB    LAPCON
        STE     LAPCON+1

        LDD     MBLTARE         ; TARE COUNTS (FLOW RATE)
        JSR     HEXBCD2
        STAB    LAATARE
        STE     LAATARE+1

        LDD     MBLCLAG         ; CONTROL LAG IN MB TABLE
        JSR     HEXBCD1         ; MAX EXPECTED IS 99 SECS
        STAB    LACLAG

        LDD     MBLCG           ; CONTROL GAIN IN MB TABLE
        JSR     HEXBCD1         ; CONVERT TO BCD
        STAB    LACG            ; CONTROL GAIN

        LDD     MBLSTYP
        STAB    LASTYPE         ; FLOW SIGNAL TYPE

        LDD     MBLMODE
        TSTB
        BEQ     LDMBLQ60
        LDAB    #$AA            ; NOTE: $AA USED IN CALIB DATA TO INDIC. CONTROL
LDMBLQ60:
        STAB    LAMODE          ; MEASUREMENT MODE (MEAS. ONLY/MEAS & CTRL)

        LDAA    #$AA
        STAA    CEEFLG1          ; SET FLAG TO HAVE DATA SAVED TO EEPROM

        RTS

// Liquid Calibration.
#define  BATCH_CALIBRATION_LA_CONSTANT              229     // FLOW RATE CONST FOR Liquid Additive. (PIB)        229
#define  BATCH_CALIBRATION_LA_UNUSED                230     // FLOW RATE UNUSED - **USED TO BE TARE**               229
#define  BATCH_CALIBRATION_LA_LAG                   231     // FLOW RATE control lag FOR L.A. (PIB)        231
#define  BATCH_CALIBRATION_LA_GAIN                  232     // FLOW RATE CONTROL GAIN FOR L.A. (PIB)        232
#define  BATCH_CALIBRATION_LA_SIGNAL                233     // SIGNAL TYPE FOR LIQ. ADDIT. (PIB)     233
//                                                                          // (0/1/2/3 = NONE/PULSES/ANALOG/COMMS)
#define  BATCH_CALIBRATION_LA_MODE                  234     // MODE (0/1 = MEAS/MEAS & CTRL)         234
#define BATCH_CALIBRATION_LA_UNUSED2                235     // (235/6) reserve for future
#define  BATCH_CALIBRATION_LA_TARE                  237     // LONG (237/8) FLOW RATE tare FOR L.A. (PIB)               237/8

       float         m_fLiquidCalibrationConstant;          //LAPCON - LOW RATE CONST FOR Liquid Additive
         long               m_lLiquidTareValue;                     //LAATARE - FLOW RATE tare FOR L.A. (PIB)
       float         m_fLiquidControlGain;                  //LACG - CONTROL GAIN FOR L.A.
       int           m_nLiquidControlLag;                   //LACLAG - control lag FOR L.A.
       int           m_nLiquidSignalType;                   //LASTYPE - SIGNAL TYPE FOR LIQ. ADDIT.
       int           m_nLiquidMode;                         //LAMODE - MODE (0/1 = MEAS/MEAS & CTRL)

*/

/*
    // control average factor  // CONTAVGFACTOR
    g_arrnMBTable[BATCH_CALIBRATION_CONTROL_AVERAGE] = g_CalibrationData.m_nControlAvgFactor;
    // weight average factor    // WGTAVGFACTOR
    g_arrnMBTable[BATCH_CALIBRATION_WEIGHT_AVERAGE] = g_CalibrationData.m_nWeightAvgFactor;
    // Mixing time  // MIXTIM
    g_arrnMBTable[BATCH_CALIBRATION_MIXING_TIME] = g_CalibrationData.m_nMixingTime;
    // Batches in kg/h average. // BATINKGHAVG
    g_arrnMBTable[BATCH_CALIBRATION_BATCHES_KGH_AVERAGE] = g_CalibrationData.m_nBatchesInKghAvg;

// --REVIEW-- JSR    RESRROB   ; RESET AVERAGING

    // control gain Assuming 2 decimal places in MB. // CLGAIN
    g_arrnMBTable[BATCH_CALIBRATION_CONTROL_GAIN] = (int)(g_CalibrationData.m_fControlGain * 100.0);

    // Set the new calibration data bit in the setpoint flags word. (asm MBNEWDATA)
    g_arrnMBTable[BATCH_SUMMARY_NEW_SETPOINTS] |= BATCH_CALIBRATION_CHANGED_MASK;



;*  - LOAD MEAS. & CONTROL PARAMETERS FROM MODBUS TABLE
LDMBMCPS:
        LDAA   MANATO
        BEQ    LDMBMCP12          ;~BRANCH IF IN MANUAL
        CLR    MANATO           ;~ELSE SET FOR MANUAL MODE
        JSR    EXCTRL           ;~EXIT AUTO CONTROL IF IT WAS IN AUTO
LDMBMCP12:

        LDD     MBAF1           ; MODBUS TABLE, AVG. FACTOR 1
        STAB    CONTAVGFACTOR          ; NOTE MAX EXPECTED IS 10
        JSR     HEXBCD1
        STAB    CONTAVGFACTORDS          ; MUST ALSO STORE DECIMAL VERSION

        LDD     MBAF2           ; MB TABLE AVG. FACTOR 2
        STAB    WGTAVGFACTOR          ; AVERAGING FACTOR 2 (MAX = 74)
        JSR     HEXBCD1
        STAB    WGTAVGFACTORDS          ; MUST ALSO STORE DECIMAL VERSION


        LDD    MBBATINKGHAVG
        CLRA
        JSR    HEXBCD1
        STAB   BATINKGHAVG

        LDD    MBMIXTIM
        CLRA
        JSR    HEXBCD1
        STAB   MIXTIM

        LDD     MBCG            ; MODBUS TABLE CONTROL GAIN (MAX = 99 DEC)
        JSR     HEXBCD1         ; CONVERT BYTE IN ACC B
        STAB    CLGAIN          ; CONTROL GAIN


        CLRW     DACPKGDONEFLAG        ; RESET THIS FLAG TO ENSURE NO VOLSET CORRECTION.        ;
        STAA   CEEFLG1           ; SET FLAG -> AA TO STORE TO EEPROM.

        RTS


*/
/*
    // Line speed constant  -  asm = LSC1  --REVIEW-- Will format of constant be different from previously??
   g_arrnMBTable[BATCH_CALIBRATION_LINE_SPEED_CONST] = (int)g_CalibrationData.m_fLineSpeedConstant;

    // Screw speed constant  -  asm = SSCON1  --REVIEW-- Will format of constant be different from previously??
   g_arrnMBTable[BATCH_CALIBRATION_SCREW_SPEED_CONST] = (int)g_CalibrationData.m_fScrewSpeedConstant;

    // Screw speed maximum  -  asm = MAXEXSP
   g_arrnMBTable[BATCH_CALIBRATION_MAX_SCREW_SPEED] = g_CalibrationData.m_fMaxExtruderSpeedPercentage;

    // Screw speed tare counts  -  asm = SSTARE
   g_arrnMBTable[BATCH_CALIBRATION_SCREW_TARE_COUNTS] = g_CalibrationData.m_nScrewSpeedTareCounts;


 LOAD LINE & SCREW SPEED CONSTS FROM MODBUS TABLE TO CALIBRATION DATA AREA
LDMBSCS:
        LDAA   MANATO
        BEQ    LDMBSC04          ;~BRANCH IF IN MANUAL
        CLR    MANATO           ;~ELSE SET FOR MANUAL MODE
        JSR    EXCTRL           ;~EXIT AUTO CONTROL IF IT WAS IN AUTO
LDMBSC04:
        LDD     MBLSC1          ; NOTE: MAX VALUE EXPECTED IS 65535
        JSR     HEXBCD2         ; CONVERT 2 BYTES TO BCD
        STAB    LSC1
        STE     LSC1+1          ; LINE SPEED CONST.

        LDD     MBSSC1          ; SCREW SPEED CONST. IN MB TABLE
        JSR     HEXBCD2
        STE     SSCON1          ; SCREW SPEED CONST(2 BYTES)

        LDD     MBSSMAX         ; MB MAX SCREW SPEED
        JSR     HEXBCD2         ; CONVERT TO BCD
        CPE     #$0999          ; CHECK IF >999
        BLS     MAXSOK          ; OK
        LDE     #$0999
        STE     MAXEXSP
        LDE     #999
        STE     MBSSMAX         ; COPY BACK TO TABLE.
        BRA     CONLS1

MAXSOK  STE     MAXEXSP
CONLS1  LDAA     #$AA
        STAA    NMAXSS
        LDD     MBSSTARE        ; SS TARE IN MB TABLE
        JSR     HEXBCD2
        STE     SSTARE          ; SCREW SPEED TARE CONSTANT

        LDAA    BLENDERMODE
        CMPA    #KGHCONTROLMODE  ; KG/HR MODE
        BEQ    LDMBSC76
        LDAA   #$AA
        STAA   CSAFLG           ;~SET FLAG TO CALC. NEW KG/H SETPOINT
LDMBSC76:
        LDAA   #$AA
        STAA   CEEFLG1           ; SET FLAG TO HAVE DATA -> EEPROM
        RTS

*/
/*

    // Maximum kg/h throughput. // MXTPT1
    g_arrnMBTable[BATCH_CALIBRATION_MAX_THROUGHPUT] = g_CalibrationData.m_nMaxThroughput;

    // Set the new calibration data bit in the setpoint flags word. (asm MBNEWDATA)
    g_arrnMBTable[BATCH_SUMMARY_NEW_SETPOINTS] |= BATCH_CALIBRATION_CHANGED_MASK;


;* LDMBHPCS       - LOADS HOPPER CONSTS (FEN,FTIM,FTW,MHW,MTPT) FROM MBTABLE
;                   INTO CALIB. AREA
LDMBHPCS:
        LDD     MB1FTW
        JSR     HEXBCD2         ; CONVERT TO BCD
        STE     DFTW1           ; STORE IN CALIB DATA

        LDD     MB1MXTP         ; MAX THRUPUT (Kg/H)
        JSR     HEXBCD2         ; CONVERT TO HEX
        STE     MXTPT1            ; STORE IN CALIB. DATA
LDMBHC30:

        CLR    FTWFLG           ;~FLAG NEW TARGET WTS
        CLR    MHWFLG           ;~FLAG NEW MIN HOP WTS
        LDAA   #$AA
        STAA   MXTPFG           ;~FLAG NEW MAX THRU-PUTS
        STAA   CEEFLG1          ; SET FLAG TO STORE DATA TO EEPROM
LDMBHC80:
        RTS
;

*/



/*
//
;;
;* LDMBWCS       - LOADS Wt CONSTS (C1 & TARE) FROM MBTABLE INTO CALIB. AREA
LDMBWCS:
        LDAA   MANATO
        BEQ    LDMBWC04          ;~BRANCH IF IN MANUAL
        CLR    MANATO           ;~ELSE SET FOR MANUAL MODE
        JSR    EXCTRL           ;~EXIT AUTO CONTROL IF IT WAS IN AUTO
LDMBWC04:
        TST     NOBLNDS         ; NO. OF BLENDS/COMPONENTS ON MACHINE
        LBEQ    LDMBWC80        ; NOTE 0 IS ILLEGAL NO.

        LDD     MB1CALC         ;
        CPD    #2                ; 1 FOR TARE.
        BNE     LDMBWC80
        JSR     TAREWEIGHHOPPER  ; TARE WITH NO DISPLAY

        JSR     CPYWTCMB        ; WT. CONTSANTS
        LDAA   #$AA
        STAA   CALFLG   ;~SET 'CAL DATA CHANGE' FLAG

LDMBWC80:
        RTS

*/
/*
    if(g_nProtocol == PROTOCOL_MODBUS)
    {

    }


; LOADS COMPONENT CONFIGURATION FROM MODBUS TABLE.

LDMBCOMPCFG:
        LDD     MBNONSTDCCFG            ; READ CONFIGURATION.
        STAB    STDCCFG              ;
        TSTB
        BEQ     ISSTDCF                 ; IS STANDARD CONFIG.
        LDD     MBCOMP1CFG              ; COMPONENT CONFIG #1
        STD     COMP1CFG                ;
        LDD     MBCOMP2CFG              ; COMPONENT CONFIG #2
        STD     COMP2CFG                ;
        LDD     MBCOMP3CFG              ; COMPONENT CONFIG #3
        STD     COMP3CFG                ;
        LDD     MBCOMP4CFG              ; COMPONENT CONFIG #4
        STD     COMP4CFG                ;
        LDD     MBCOMP5CFG              ; COMPONENT CONFIG #5
        STD     COMP5CFG                ;
        LDD     MBCOMP6CFG              ; COMPONENT CONFIG #6
        STD     COMP6CFG                ;
        LDD     MBCOMP7CFG              ; COMPONENT CONFIG #7
        STD     COMP7CFG                ;
        LDD     MBCOMP8CFG              ;
        STD     COMP8CFG                ;
        LDD     MBCOMP9CFG              ;
        STD     COMP9CFG                ;
        LDD     MBCOMP10CFG              ;
        STD     COMP10CFG                ;
        LDD     MBCOMP11CFG              ;
        STD     COMP11CFG                ;
        LDD     MBCOMP12CFG              ;
        STD     COMP12CFG                ;
ISSTDCF LDAA    #$AA
        STAA    CEEFLG1          ; SET FLAG TO SAVE TO EEPROM
        RTS


*/
//////////////////////////////////////////////////////
// LoadSystemConfigurationFromMB                    (from asm = LDMBSYC)
//
// Load System Configuration from the MODBUS TABLE
//
// M.McKiernan                          06-10-2004
// First Pass
//
//////////////////////////////////////////////////////
void LoadSystemConfigurationFromMB( void )
{
    BOOL bResult;
    int nTemp,i;
    float fPercentage,fTemp,fValue;

    if(g_bConfigIsLocked)
    {
        CopySystemConfigurationToMB();
    }
    else
    if(g_arrnMBTable[BATCH_CALIBRATION_COMPONENTS] == 0)     // 0 components is illegal.
    {
        CopySystemConfigurationToMB();  // copy current config back into MB table.
    }
    else
    {
    // Line no. asm = LINENO
    g_CalibrationData.m_nLineNumber = g_arrnMBTable[BATCH_CALIBRATION_LINE_NUMBER];

        // No. of components(blends) asm = NOBLNDS
         g_CalibrationData.m_nComponents = g_arrnMBTable[BATCH_CALIBRATION_COMPONENTS];
         // asm = CHECKPRODSETE100P
//nbb--todo--width remove         bResult = CheckProductionSetpointsTotal100();  // do 100% check.

        // Fill retries = asm = FILLRETRY
    g_CalibrationData.m_nFillRetries = g_arrnMBTable[BATCH_CALIBRATION_FILL_RETRIES];

    // Blender mode asm = BLENDERMODE
    g_CalibrationData.m_nBlenderMode = g_arrnMBTable[BATCH_CALIBRATION_BLENDER_MODE];

    // control mode - asm = CRLMODE
    g_CalibrationData.m_nControlMode = g_arrnMBTable[BATCH_CALIBRATION_CONTROL_MODE];

        // Regrind component no. asm = REGCOMP
        // --REVIEW-- will the regrind component no. be coming from the recipe from now on??
        g_CalibrationData.m_nRegrindComponentNumber = g_arrnMBTable[BATCH_CALIBRATION_REGRIND_COMPONENT];

        // Fast control response.. asm = FSTCTRL
    if(g_arrnMBTable[BATCH_CALIBRATION_FAST_CONTROL])
            g_CalibrationData.m_bFastControlResponse = FALSE;
        else
            g_CalibrationData.m_bFastControlResponse = TRUE;

        // Diagnostics mode  -  asm = DIAGMODE
    g_CalibrationData.m_nDiagnosticsMode = g_arrnMBTable[BATCH_CALIBRATION_DIAG_MODE];

        // Vacuum Loading  -  asm = VACLOAD
/* wfh.
    if(g_arrnMBTable[BATCH_CALIBRATION_VAC_LOADING] !=0)
    {
        g_CalibrationData.m_bVacLoading = TRUE;
    }
    else
    {
        g_CalibrationData.m_bVacLoading = FALSE;
    }
*/
    g_CalibrationData.m_nMachineType = MACHINE_TYPE_WIDTH_CONTROL;
    //--REVIEW-- bypass now in CONFIG OPTIONs word.
    // Bypass  -  asm = BYPASSE
//   g_CalibrationData.m_bBypassModeFlag = g_arrnMBTable[BATCH_CALIBRATION_BYPASS];


        // Volumetric mode  -  asm = ISVOLMODE
    if((g_arrnMBTable[BATCH_CALIBRATION_VOLUMETRIC_MODE] == 1)&&(g_bDiagnosticsPasswordInUse))
    {
        g_CalibrationData.m_bVolumetricModeFlag = TRUE;
    }
    else
    {
        g_CalibrationData.m_bVolumetricModeFlag = FALSE;
        g_arrnMBTable[BATCH_CALIBRATION_VOLUMETRIC_MODE] = 0;
    }
        // Serial EI fitted  -  asm = EIOMITTED
    g_CalibrationData.m_nPeripheralCardsPresent = g_arrnMBTable[BATCH_CALIBRATION_SEI_INSTALLED];

        // Liquid Additive  -  asm = AVECPIB
    g_CalibrationData.m_bLiquidEnabled = g_arrnMBTable[BATCH_CALIBRATION_PIB_ENABLE];

        // Blender type  -  asm = BLENDERTYPE
    if(g_nAllowUnrestricedAccess == UNRESTRICED_ACCESS)
    {
          WORD nType = g_arrnMBTable[BATCH_CALIBRATION_BLENDER_TYPE];

          if( (nType >= FIRSTBLENDERTYPE) && (nType <= LASTBLENDERTYPE))
          {
        	  g_CalibrationData.m_nBlenderType = nType;
          }
          else
          {
        	  nType = FIRSTBLENDERTYPE;      // default to first type
        	  g_CalibrationData.m_nBlenderType = nType;
        	  g_arrnMBTable[BATCH_CALIBRATION_BLENDER_TYPE] = nType;  //update MB table with default.
          }
    }
    else
    {
    	g_arrnMBTable[BATCH_CALIBRATION_BLENDER_TYPE] = g_CalibrationData.m_nBlenderType;  //update MB table with default.
    }

        // Shutdown enable  -  asm = SHUTDOWNEN
    g_CalibrationData.m_bShutDownFlag = g_arrnMBTable[BATCH_CALIBRATION_EXTR_SHUTDOWN];

        // Shutdown time -  asm = SHUTDOWNTM
    g_CalibrationData.m_nShutDownTime = g_arrnMBTable[BATCH_CALIBRATION_SHUTDOWN_TIME];

        // Units type  -  asm = UNITTYPE
    g_CalibrationData.m_nUnitType = g_arrnMBTable[BATCH_CALIBRATION_MEASUREMENT_UNITS];

        // Screw speed signal  -  asm = SSSTYPE
    g_CalibrationData.m_nScewSpeedSignalType = g_arrnMBTable[BATCH_CALIBRATION_SPEED_SIGNAL];

        // Extruder stall enable  -  asm = EXSENAB
    if(g_arrnMBTable[BATCH_CALIBRATION_STALL_CHECK])
            g_CalibrationData.m_bExtruderStallCheck = TRUE;
        else
            g_CalibrationData.m_bExtruderStallCheck = FALSE;

        // Level sensor delay  -  asm = LSDELAY
    g_CalibrationData.m_nLevelSensorDelay = g_arrnMBTable[BATCH_CALIBRATION_DELAY_TO_LS_ALARM];

        // Offline type  -  asm = OFFLINETYPE
    g_CalibrationData.m_nOfflineType = g_arrnMBTable[BATCH_CALIBRATION_OFFLINE_TYPE];

        // Purge component no.  -  asm = PURGECOMPNO
    g_CalibrationData.m_nPurgeComponentNumber = g_arrnMBTable[BATCH_CALIBRATION_PURGE_COMPONENT];

        // Control type  -  asm = CONTROLTYPE
    g_CalibrationData.m_nControlType = g_arrnMBTable[BATCH_CALIBRATION_CONTROL_TYPE];

        // Control Lag.  -  asm = CONTROLLAG
    g_CalibrationData.m_nControlLag = g_arrnMBTable[BATCH_CALIBRATION_CONTROL_LAG];

        // Increase rate  -  asm = INCREASERATE
    g_CalibrationData.m_nIncreaseRate = g_arrnMBTable[BATCH_CALIBRATION_INC_RATE];

        // Decrease rate  -  asm = DECREASERATE
    g_CalibrationData.m_nDecreaseRate = g_arrnMBTable[BATCH_CALIBRATION_DEC_RATE];

        // Control Deadband  -  asm = DEADBAND
    g_CalibrationData.m_nControlDeadband = g_arrnMBTable[BATCH_CALIBRATION_DEADBAND];

        // Fine inc-dec rate  -  asm = FINEINCDEC
    g_CalibrationData.m_nFineIncDec = g_arrnMBTable[BATCH_CALIBRATION_FINE_INC_DEC];

    g_bWriteIncreaseDecreaseDataToSEI = TRUE;       /* initiate copy of data to SEI */

        // Max component percentage  -  asm = MAXPCNT1 - 12
        for(i = 0; i < MAX_COMPONENTS; i++ )  // note accept for all possible components, even if not present.
        {
            fPercentage = (float)g_arrnMBTable[BATCH_CALIBRATION_MAX_PERCENT_1 + i];
            fPercentage /= 100.0f;  // NB: Percentages are to 2DP in MB table. (9999 = 99.99%)  - div by 100.
            g_CalibrationData.m_fMaxPercentage[i] = fPercentage;
        }

        // Stage Fill enable  -  asm = STAGEFILLEN
        g_CalibrationData.m_nStageFillEn = g_arrnMBTable[BATCH_CALIBRATION_STAGE_FILL_EN];

        // Moulding  -  asm = moulding
    g_CalibrationData.m_bMouldingOption = g_arrnMBTable[BATCH_CALIBRATION_MOULDING];

//Clean stuff
        // Phase 1 On time  -  asm = PH1ONTIME
    g_CalibrationData.m_nPhase1OnTime = g_arrnMBTable[BATCH_CALIBRATION_PHASE_1_ON_TIME];
        // Phase 1 Off time  -  asm = PH1OFFTIME
    g_CalibrationData.m_nPhase1OffTime = g_arrnMBTable[BATCH_CALIBRATION_PHASE_1_OFF_TIME];

        // Phase 2 On time  -  asm = PH2ONTIME
    g_CalibrationData.m_nPhase2OnTime = g_arrnMBTable[BATCH_CALIBRATION_PHASE_2_ON_TIME];
        // Phase 2 Off time  -  asm = PH2OFFTIME
    g_CalibrationData.m_nPhase2OffTime = g_arrnMBTable[BATCH_CALIBRATION_PHASE_2_OFF_TIME];

        // Blender clean repetitions  -  asm = BCLEANREPETITIONS
    g_CalibrationData.m_nCleanRepetitions = g_arrnMBTable[BATCH_CALIBRATION_CLEAN_REPS];

        // Mixer clean repetitions  -  asm = MCLEANREPETITIONS
    g_CalibrationData.m_nMixerCleanRepetitions = g_arrnMBTable[BATCH_CALIBRATION_MIX_CLEAN_REPS];

        // Bypass time limit  -  asm = BYPASSTIMELIMIT
    g_CalibrationData.m_nBypassTimeLimit = g_arrnMBTable[BATCH_CALIBRATION_BYPASS_TIME_LIMIT];

        // Bypass time limit clean -  asm = BYPASSDCLNTIMELIMIT
    g_CalibrationData.m_nBypassTimeLimitClean = g_arrnMBTable[BATCH_CALIBRATION_BYP_TIME_LIMIT_CLN];

        // Top up %  -  asm = PERTOPUP
    g_CalibrationData.m_nPercentageTopUp = g_arrnMBTable[BATCH_CALIBRATION_TOPUP_PERCENT];

        // Max batch size, relates to top up mode -  asm = MAXGRAMSPERPER  --REVIEW-- kgs to 1dp.
    g_CalibrationData.m_fMaxBatchSizeInKg = (float)g_arrnMBTable[BATCH_CALIBRATION_MAX_BATCH_SIZE_KGS] / 10.0f; //kg to 1dp

        // Top up comp no. ref  asm = TOPUPCOMPNOREF
    g_CalibrationData.m_nTopUpCompNoRef = g_arrnMBTable[BATCH_CALIBRATION_TOPUP_COMP_REF];

        // Intermittent Mix time  -  asm = INTMIXTIME   //
    g_CalibrationData.m_nIntermittentMixTime = g_arrnMBTable[BATCH_CALIBRATION_INT_MIX_TIME];


        // Intermittent Mix time  -  asm = MIXINGSHUTOFF   //
    g_CalibrationData.m_nMixShutoffTime = g_arrnMBTable[BATCH_CALIBRATION_MIXING_SHUTOFF_TIME];

    // Max throughput asm = MXTPT1

    fValue = (float)g_arrnMBTable[BATCH_CALIBRATION_MAXIMUM_THROUGHPUT];
    if(g_fWeightConversionFactor != 1.0f)
    fValue /= g_fWeightConversionFactor;
    g_CalibrationData.m_nMaxThroughput = fValue;                                  // Throughput is to 1dp in MB.

    if(g_CalibrationData.m_nBlenderMode == MODE_MAXTHROUGHPUTZT)
    {
        g_bDoCMRDToAPerKgCalculation = TRUE;        /* asm CMRNOW  */
    }


    // Offline mixing time asm MIXTIM
    g_CalibrationData.m_nMixingTime = g_arrnMBTable[BATCH_CALIBRATION_OFFLINE_MIXING_TIME];

    // Vacuum delay used in single recipe mode ASM = VACUUMDELAY
    g_CalibrationData.m_nMaterialTransportDelay = g_arrnMBTable[BATCH_CALIBRATION_MATERIAL_TRANSPORT_DELAY];

    // Post Vacuum delay used in single recipe mode ASM = POSTVACUUMDELAY
    g_CalibrationData.m_nPostMaterialTransportDelay = g_arrnMBTable[BATCH_CALIBRATION_POST_VACUUM_DELAY];



//passwords --REVIEW--
        // Password  -  asm = PASWRD
   g_CalibrationData.m_lManagerPassWord  = g_arrnMBTable[BATCH_CALIBRATION_PASSWORD_SUPERVISOR];
        // Password #2  -  asm = PASWRD2

   g_CalibrationData.m_lOperatorPassWord = g_arrnMBTable[BATCH_CALIBRATION_PASSWORD_OPERATOR];

   // TSM or Engineering password.
   g_CalibrationData.m_lTSMPassWord = g_arrnMBTable[BATCH_CALIBRATION_PASSWORD_TSM];

// minimum opening time check
   if(g_arrnMBTable[BATCH_MIN_OPENING_TIME_CHECK] !=0)
   {
       g_CalibrationData.m_bMinimumOpeningTimeCheck = TRUE;
   }
   else
   {
       g_CalibrationData.m_bMinimumOpeningTimeCheck = FALSE;
   }

   if(g_arrnMBTable[BATCH_FLOW_RATE_CHECK] !=0)
   {
       g_CalibrationData.m_bFlowRateCheck = TRUE;
   }
   else
   {
       g_CalibrationData.m_bFlowRateCheck = FALSE;
   }

   if(g_arrnMBTable[BATCH_VAC8_EXPANSION] !=0)
   {
       g_CalibrationData.m_bVac8Expansion = TRUE;
   }
   else
   {
       g_CalibrationData.m_bVac8Expansion = FALSE;
   }

// Config options.
    nTemp = g_arrnMBTable[BATCH_CALIBRATION_CONFIG_OPTIONS];

        // High/low regrind
        if(nTemp & BATCH_CONFIG_HILO_REG_BIT)
        {
            g_CalibrationData.m_bHighLoRegrindOption = TRUE;    // asm = HILOREGENABLED
        }
        else
        {
            g_CalibrationData.m_bHighLoRegrindOption = FALSE;
        }
        // Max component % check enabled??
        if(nTemp & BATCH_CONFIG_MAX_COMP_PER_EN_BIT)
            g_CalibrationData.m_bMaxComponentPercentageCheck = TRUE;    // asm = MAXPCNTCHECK
        else
            g_CalibrationData.m_bMaxComponentPercentageCheck = FALSE;

        // First component compensation enabled?
        if(nTemp & BATCH_CONFIG_FIRST_COMP_COMPENSATION_BIT)
            g_CalibrationData.m_bFirstComponentCompensation = FALSE;
        else
            g_CalibrationData.m_bFirstComponentCompensation = TRUE; // asm = FIRSTCOMPCOMPENDIS

        // Layering enable?
        if(nTemp & BATCH_CONFIG_LAYERING_BIT)
        {
            g_CalibrationData.m_bLayering = TRUE;   /* asm = LAYERING */
        }
        else
            g_CalibrationData.m_bLayering = FALSE;

        // Mixing screw detection.
        if(nTemp & BATCH_CONFIG_MIXER_DETECT_BIT)
            g_CalibrationData.m_bMixerScrewDetect = TRUE;   // asm = MIXSCREWDETECTION.
        else
            g_CalibrationData.m_bMixerScrewDetect = FALSE;  // asm = MIXSCREWDETECTION.

        // Double dump
        if(nTemp & BATCH_CONFIG_DOUBLE_DUMP_BIT)
            g_CalibrationData.m_bDoubleDump = TRUE; // asm = DOUBLEDUMP
        else
            g_CalibrationData.m_bDoubleDump = FALSE;

        // Regrind retry.
        if(nTemp & BATCH_CONFIG_REGRIND_RETRY_BIT)
            g_CalibrationData.m_bRegrindRetry = TRUE;   // asm = REGRINDRETRY
        else
            g_CalibrationData.m_bRegrindRetry = FALSE;


        // Bypass mode enable
        if(nTemp & BATCH_CONFIG_BYPASS_EN_BIT)
            g_CalibrationData.m_bBypassMode = TRUE; // asm = BYPASSMODE
        else
            g_CalibrationData.m_bBypassMode = FALSE;

        // Manual fill sort enable  --REVIEW-- Manual sorting not actually used in current version.
        if(nTemp & BATCH_CONFIG_FILL_MAN_SORT_BIT)
            g_CalibrationData.m_bFillOrderManualSelect = TRUE;  // asm = MANUALFILLSORT
        else
            g_CalibrationData.m_bFillOrderManualSelect = FALSE;

        // Top-up enable.
        if(nTemp & BATCH_CONFIG_TOPUP_EN_BIT)
            g_CalibrationData.m_bTopUpMode = TRUE;          // asm = TOPUP
        else
            g_CalibrationData.m_bTopUpMode = FALSE;

        // Blow every cycle (clean function)
        if(nTemp & BATCH_CONFIG_BLOW_EVERY_CYCLE_BIT)
            g_CalibrationData.m_bBlowEveryCycle = TRUE; // asm = BLOWEVERYCYCLE
        else
            g_CalibrationData.m_bBlowEveryCycle = FALSE;

        // Cleaning enable
        g_CalibrationData.m_nCleaning =  g_arrnMBTable[BATCH_CALIBRATION_CLEANING];

        // Single word write enable
        if(nTemp & BATCH_CONFIG_SINGLE_WORD_WRITE_BIT)
            g_CalibrationData.m_bSingleWordWrite = TRUE;            /* asm = SINGLEWORDWRITE */
        else
            g_CalibrationData.m_bSingleWordWrite = FALSE;

        // Fast / Slow option

        if(nTemp & BATCH_CONFIG_FAST_SLOW_CYCLE_BIT)
            g_CalibrationData.m_bCycleFastFlag = FALSE;
        else
            g_CalibrationData.m_bCycleFastFlag = TRUE;            /* asm = FASTFLG  */





// distribute option word #2 asm = MBOPTIONS2
// use regrind as reference ASM USEREGRINDASREF

    nTemp = g_arrnMBTable[BATCH_CALIBRATION_OPTIONS_2];

    if(nTemp & BATCH_CALIBRATION_USE_REGRIND_AS_REF_BITPOS)
        g_CalibrationData.m_nUseRegrindAsReference = TRUE;      // asm = USEREGRINDASREF
        else
        g_CalibrationData.m_nUseRegrindAsReference = FALSE;

    if(nTemp & BATCH_CALIBRATION_SEND_EMAIL_ON_ORDER_CHANGE_BITPOS)
        g_CalibrationData.m_bSendEmailOnOrderChange = TRUE;      //
        else
        g_CalibrationData.m_bSendEmailOnOrderChange = FALSE;


// Printing options:


// Config options.
    nTemp = g_arrnMBTable[BATCH_CALIBRATION_OPTIONS];

        // Correct Every N cycles
        if(nTemp & BATCH_CAL_OPTIONS_CORRECTEVERYCYCLEBITPOS)
        {
            g_CalibrationData.m_bCorrectEveryNCycles = FALSE;
        }
        else
        {
            g_CalibrationData.m_bCorrectEveryNCycles = TRUE;    // asm CORRECTEVERYNCYCLES
        }



        //--REVIEW-- in assembler was in the meas & control parameters section
        // control band/ control % limit. CONTROLPER
        g_CalibrationData.m_nControlPercentageLimit = g_arrnMBTable[BATCH_CALIBRATION_CONTROL_BAND];

        g_CalibrationData.m_nBaudRate = g_arrnMBTable[BATCH_CALIBRATION_NETWORK_BAUD_RATE];

        fTemp = (float)(g_arrnMBTable[BATCH_CALIBRATION_LATENCY]);
        g_CalibrationData.m_fLatency = fTemp / 1000.0f;
//        printf("\n valve latency is %3.3f",g_CalibrationData.m_fLatency);   //nbb--testonly--
        // --REVIEW-- do we want to save off calibration data in chunks??

        g_CalibrationData.m_nLineSpeedSignalType = g_arrnMBTable[BATCH_CALIBRATION_LINE_SPEED_SIGNAL_TYPE];  //sbb--todo-- add to SBB software

        g_CalibrationData.m_nRecipeEntryType = g_arrnMBTable[BATCH_CALIBRATION_ENTRY_TYPE];

        g_CalibrationData.m_nLineSpeedAverage = g_arrnMBTable[BATCH_CALIBRATION_LINE_SPEED_AVERAGE];

        g_CalibrationData.m_nPurgeEnable = g_arrnMBTable[BATCH_CALIBRATION_PURGE];

        g_CalibrationData.m_bComponentNamingAlphanumeric = g_arrnMBTable[BATCH_CALIBRATION_COMPONENT_NAMING_ALPHANUMERIC];

        g_CalibrationData.m_bOptimisation = g_arrnMBTable[BATCH_OPTIMISATION];

        g_CalibrationData.m_nDeviationAlarmEnable = g_arrnMBTable[BATCH_DEVIATION_ALARM_ENABLE];

        g_CalibrationData.m_nLogToSDCard = (BYTE)g_arrnMBTable[BATCH_LOG_TO_SD_CARD];

         if(g_arrnMBTable[BATCH_PAUSE_BLENDER_ON_POWER_UP] !=0)
         {
               g_CalibrationData.m_bPauseBlenderOnPowerup = TRUE;
         }
         else
         {
            g_CalibrationData.m_bPauseBlenderOnPowerup = FALSE;
         }


         if(g_arrnMBTable[BATCH_LOG_FILE_TYPE_CSV] !=0)
         {
               g_CalibrationData.m_bLogFormatIsCSV = TRUE;
         }
         else
         {
            g_CalibrationData.m_bLogFormatIsCSV = FALSE;
         }

         if(g_arrnMBTable[BATCH_CALIBRATION_ALLOW_CONTINUOUS_DOWNLOAD] !=0)
         {
               g_CalibrationData.m_bAllowForContinuousDownload = TRUE;
         }
         else
         {
            g_CalibrationData.m_bAllowForContinuousDownload = FALSE;
         }

// Telnet option

         if(g_arrnMBTable[BATCH_CALIBRATION_TELNET] !=0)
         {
               g_CalibrationData.m_bTelnet = TRUE;
         }
         else
         {
            g_CalibrationData.m_bTelnet = FALSE;
         }

        if(g_arrnMBTable[BATCH_COMMUNICATIONS_NO_PARITY] == 0)
        {
              g_CalibrationData.m_bNoParity = FALSE;                    //
        }
        else
        {
              g_CalibrationData.m_bNoParity = TRUE;                      //
        }
        if(g_arrnMBTable[BATCH_CHECK_FOR_VALID_COMPONENT_PER_TARGET] == 0)
        {
              g_CalibrationData.m_bCheckForValidComponentTargetPercentage = FALSE;                    //
        }
        else
        {
              g_CalibrationData.m_bCheckForValidComponentTargetPercentage = TRUE;                      //
        }


        for(i = 0; i < MAX_COMPONENTS; i++ )
        {
           g_CalibrationData.m_nComponentBinVolume[i] =  g_arrnMBTable[BATCH_OPTIMISATION_VOLUME_1+i];
        }

        for(i = 0; i < MAX_COMPONENTS; i++ )
        {
           g_CalibrationData.m_fMaxPercentageDeviation[i] = ((float) (g_arrnMBTable[BATCH_CALIBRATION_MAX_PERCENTAGE_DEVIATION_1+i]))/100.0f;
        }

        for(i = 0; i < MAX_COMPONENTS; i++ )
        {
            g_CalibrationData.m_wFillingMethod[i] = g_arrnMBTable[BATCH_FILLING_METHOD_COMPONENT_1+i];
        }

        g_CalibrationData.m_nPauseOnErrorNo = g_arrnMBTable[BATCH_CALIBRATION_MAX_PAUSE_ON_ERROR_NO];


        g_bSaveAllCalibrationToEEprom = TRUE;       // set flag to cause all calibration data to be written to EEPROM.
        int nResult = 0;
        nResult = CheckForValidCalibrationData(FALSE); // check that the calibration data is valid.
    }

}

//////////////////////////////////////////////////////
// LoadSystemConfiguration3FromMB           // new command added P.Smith
//
// Load System Configuration from the MODBUS TABLE
//
//
//////////////////////////////////////////////////////
void LoadSystemConfiguration3FromMB( void )
{
    if(g_bConfigIsLocked)
    {
        CopySystemConfiguration3ToMB();
    }
    else
    {
        // dump device type
        g_CalibrationData.m_nDumpDeviceType = g_arrnMBTable[BATCH_CONFIG_PRINTER_USB_ENABLE];
        // alarms dump enable
        g_CalibrationData.m_bAlarmDumpFlag = g_arrnMBTable[BATCH_CONFIG_ALARMS_DUMP_ENABLE];
        // alarms dump enable
        g_CalibrationData.m_bOrderReportDumpFlag = g_arrnMBTable[BATCH_CONFIG_ORDER_DUMP_ENABLE];
        // order dump enable
        g_CalibrationData.m_bRollReportDumpFlag = g_arrnMBTable[BATCH_CONFIG_ROLL_DUMP_ENABLE];
        // hourly dump enable
        g_CalibrationData.m_bHourlyDumpFlag = g_arrnMBTable[BATCH_CONFIG_HOURLY_DUMP_ENABLE];
        // batch log dump enable
        g_CalibrationData.m_bBatchLogDumpFlag = g_arrnMBTable[BATCH_CONFIG_BATCH_LOG_DUMP_ENABLE];
        // event log dump enable
        g_CalibrationData.m_bEventLogDumpFlag = g_arrnMBTable[BATCH_CONFIG_EVENT_LOG_DUMP_ENABLE];
        // history log dump enable
        g_CalibrationData.m_bHistoryLogDumpFlag = g_arrnMBTable[BATCH_CONFIG_HISTORY_LOG_DUMP_ENABLE];
        // history log dump enable
        g_CalibrationData.m_bShiftReportDumpFlag = g_arrnMBTable[BATCH_CONFIG_SHIFT_DUMP_ENABLE];
        g_bSaveAllCalibrationToEEprom = TRUE;       // set flag to cause all calibration data to be written to EEPROM.
    }
}



//////////////////////////////////////////////////////
// CopyComponentConfigurationToMB  asm = CPYCCFGTMB
//
// copy component configuration data to MB.
//
// M.McKiernan                          07-10-2004
// First Pass
//
//////////////////////////////////////////////////////
void CopyComponentConfigurationToMB( void )
{
int i;

    if(g_nProtocol == PROTOCOL_MODBUS)
    {
        // STDCCFG  Standard component config (YES/NO)STDCCFG
        if(g_CalibrationData.m_bStandardCompConfig)
            g_arrnMBTable[BATCH_CALIBRATION_COMP_CONFIG] = 0;
        else
            g_arrnMBTable[BATCH_CALIBRATION_COMP_CONFIG] = 1;   // 1 => non-standard in Modbus

        // copy each component config's to MB  - COMP1CFG
        for(i=0; i < MAX_COMPONENTS && i < 12; i++)     // nb max 12 contiguous words in MB available.
        {
            g_arrnMBTable[BATCH_CALIBRATION_COMP1_CONFIG + i] = g_CalibrationData.m_nCompConfig[i];
        }

    }

}

//////////////////////////////////////////////////////
// CopyMeasurementAndControlParametersToMB  asm = CPYMCPMB
//
// copy Measurement and control parameter data to MB.
//
// M.McKiernan                          11-10-2004
// First Pass
//
//////////////////////////////////////////////////////
void CopyMeasurementAndControlParametersToMB( void )
{
float fValue;

    // control average factor  // CONTAVGFACTOR
    g_arrnMBTable[BATCH_CALIBRATION_CONTROL_AVERAGE] = g_CalibrationData.m_nControlAvgFactor;
    // weight average factor    // WGTAVGFACTOR
    g_arrnMBTable[BATCH_CALIBRATION_WEIGHT_AVERAGE] = g_CalibrationData.m_nWeightAvgFactor;
    // Mixing time  // MIXTIM
    g_arrnMBTable[BATCH_CALIBRATION_MIXING_TIME] = g_CalibrationData.m_nMixingTime;
    // Batches in kg/h average. // BATINKGHAVG
    g_arrnMBTable[BATCH_CALIBRATION_BATCHES_KGH_AVERAGE] = g_CalibrationData.m_nBatchesInKghAvg;

// --REVIEW-- JSR    RESRROB   ; RESET AVERAGING

    // control gain Assuming 2 decimal places in MB. // CLGAIN
    fValue = g_CalibrationData.m_fControlGain + 0.005;  // round to nearest 2nd dp
    g_arrnMBTable[BATCH_CALIBRATION_CONTROL_GAIN] = (int)(fValue * 100.0);  // NB: gain is to 2DP in MB table. (99 = 0.99)

    // Set the new calibration data bit in the setpoint flags word. (asm MBNEWDATA)
    g_arrnMBTable[BATCH_SUMMARY_NEW_SETPOINTS] |= BATCH_CALIBRATION_CHANGED_MASK;


}

//////////////////////////////////////////////////////
// CopyHopperConstantsToMB  asm = CPYHPCMB
//
// copy Hopper Constants to MB.
//
// M.McKiernan                          11-10-2004
// First Pass
//
//////////////////////////////////////////////////////
void CopyHopperConstantsToMB( void )
{
float   fValue;
    // Batch size; 1 decimal places in MB. // DFTW1
    fValue =     g_CalibrationData.m_fBatchSize;
    if(g_fWeightConversionFactor != 1.0f)
    fValue *=    g_fWeightConversionFactor;
    fValue += 0.05;
    g_arrnMBTable[BATCH_CALIBRATION_BATCH_SIZE] = (int)(fValue * 10.0);

    // Maximum kg/h throughput. // MXTPT1
    fValue =     g_CalibrationData.m_nMaxThroughput;
    if(g_fWeightConversionFactor != 1.0f)
    fValue *=    g_fWeightConversionFactor;
    g_arrnMBTable[BATCH_CALIBRATION_MAX_THROUGHPUT] = (int)(fValue);

    // Set the new calibration data bit in the setpoint flags word. (asm MBNEWDATA)
    g_arrnMBTable[BATCH_SUMMARY_NEW_SETPOINTS] |= BATCH_CALIBRATION_CHANGED_MASK;

}


//////////////////////////////////////////////////////
// CopyWeightConstantsToMB  asm = CPYWTCMB
//
// copy weight Constants (weight calibration and tare counts) to MB.
//
// M.McKiernan                          11-10-2004
// First Pass
//
//////////////////////////////////////////////////////
void CopyWeightConstantsToMB( void )
{
    union WordAndDWord uValue;
    double ffWtConst;

    // Weight constant; 3 decimal places in MB. // MBCALCON
    ffWtConst = (double)(g_CalibrationData.m_fWeightConstant);      //use double to preserve resolution.
    uValue.lValue = (long)(ffWtConst);
    g_arrnMBTable[BATCH_CALIBRATION_WEIGHT_CONSTANT] = uValue.nValue[0];        // M.s.Byte.
    g_arrnMBTable[BATCH_CALIBRATION_WEIGHT_CONSTANT+1] = uValue.nValue[1];      // L.s.Byte.

    // Tare counts; integer in MB. // MB1TARE
    uValue.lValue = g_CalibrationData.m_lTareCounts;
    g_arrnMBTable[BATCH_CALIBRATION_TARE_COUNTS] = uValue.nValue[0];        // M.s.Byte.
    g_arrnMBTable[BATCH_CALIBRATION_TARE_COUNTS+1] = uValue.nValue[1];      // L.s.Byte.

    // Set the new calibration data bit in the setpoint flags word. (asm MBNEWDATA)
    g_arrnMBTable[BATCH_SUMMARY_NEW_SETPOINTS] |= BATCH_CALIBRATION_CHANGED_MASK;

}

//////////////////////////////////////////////////////
// CopyActualPercentagesToMB  asm = CPYAPCTMB
//
// copy %'s into MB table.
//
// M.McKiernan                          03-11-2004
// First Pass
//
//////////////////////////////////////////////////////
void CopyActualPercentagesToMB( void )
{
unsigned int nIndex,j;
float fPercentage;
    nIndex = BATCH_SUMMARY_PERCENTAGE_1;

    if(g_CalibrationData.m_nComponents <= 10)
    {
        for(j=0; j < g_CalibrationData.m_nComponents; j++ )   // note only 10 contiguous blocks
        {
            if(g_bShowInstantaneousWeight)
            {
                fPercentage =   g_fComponentActualPercentage[j];
            }
            else
            {
                fPercentage =   g_fComponentActualPercentageRR[j];
            }
            fPercentage += 0.005;       // for rounding to nearest 2nd decimal digit.
            g_arrnMBTable[nIndex] = (int)(fPercentage * 100);       // NB: Percentages are to 2DP in MB table. (9999 = 99.99%)
            nIndex += PSUM_BLOCK_SIZE;
        }
    }
    else
    {
        for(j=0; j<10; j++ )   // note only 10 contiguous blocks
        {
            if(g_bShowInstantaneousWeight)
            {
                fPercentage =   g_fComponentActualPercentage[j];
            }
            else
            {
                fPercentage =   g_fComponentActualPercentageRR[j];
            }
            fPercentage += 0.005;       // for rounding to nearest 2nd decimal digit.
            g_arrnMBTable[nIndex] = (int)(fPercentage * 100);       // NB: Percentages are to 2DP in MB table. (9999 = 99.99%)
            nIndex += PSUM_BLOCK_SIZE;
        }
        for(j=10; j < g_CalibrationData.m_nComponents; j++ )   // note only 10 contiguous blocks
        {
            if(g_bShowInstantaneousWeight)
            {
                fPercentage =   g_fComponentActualPercentage[j];
            }
            else
            {
                fPercentage =   g_fComponentActualPercentageRR[j];
            }
            fPercentage += 0.005;       // for rounding to nearest 2nd decimal digit.
            g_arrnMBTable[PSUM_BLOCK_11 + ((j-10) * PSUM_BLOCK_SIZE) + PSUM_PERCENTAGE_OFFSET] = (int)(fPercentage * 100);       // NB: Percentages are to 2DP in MB table. (9999 = 99.99%); // copy msb
        }

    }

}




//////////////////////////////////////////////////////
// CopyWeightsToMB  asm = CPYTWTMB
//
//
// P.Smith                          10-3-2005
// Copy total order, front / back roll weights to modbus
//
//////////////////////////////////////////////////////
void CopyWeightsToMB( void )
{
float fTemp;
union WordAndDWord uValue;

    fTemp = g_fTotalOrderWeightAccumulator;
    if(g_fWeightConversionFactor != 1.0f)
    fTemp *=    g_fWeightConversionFactor;
    g_arrnMBTable[BATCH_SUMMARY_ORDER_WEIGHT] = 0;
    g_arrnMBTable[BATCH_SUMMARY_ORDER_WEIGHT+1] = (int)(fTemp);  //


    if(g_CurrentRecipe.m_bSheetTube)
    {
        fTemp = g_fTotalFrontWeightAccumulator / 2.0f;
        if(g_fWeightConversionFactor != 1.0f)
        fTemp *=    g_fWeightConversionFactor;
        g_arrnMBTable[BATCH_SUMMARY_FRONT_ROLL_WEIGHT] = (int)fTemp;
    }
    else
    {
        fTemp = g_fTotalFrontWeightAccumulator;
        if(g_fWeightConversionFactor != 1.0f)
        fTemp *=    g_fWeightConversionFactor;
        g_arrnMBTable[BATCH_SUMMARY_FRONT_ROLL_WEIGHT] = (int)fTemp;
    }
    g_arrnMBTable[BATCH_SUMMARY_FRONT_SLIT_WEIGHT] = (int)(fTemp / (float)g_CurrentRecipe.m_nFrontSlitReels);

    if(g_CurrentRecipe.m_bSheetTube) // if sheet, do the back
    {
        g_arrnMBTable[BATCH_SUMMARY_BACK_SLIT_WEIGHT] = (int) (g_fTotalBackWeightAccumulator / (2.0 * g_CurrentRecipe.m_nBackSlitReels));   // ASM = BSWTACC
    }
    else
    {
        g_arrnMBTable[BATCH_SUMMARY_BACK_SLIT_WEIGHT] = 0; // set back to zero
    }


    fTemp = g_fTotalHistoryBatchWeightAccumulator;
    if(g_fWeightConversionFactor != 1.0f)
    fTemp *=    g_fWeightConversionFactor;
    fTemp += 0.05;         // for rounding to nearest 1st decimal digit.
    g_arrnMBTable[BATCH_SUMMARY_WEIGHT_ACCUMULATOR] = (int) (fTemp * 10.0);  /* asm MBTOTWACCBAT */

// copy this value to a different position
    fTemp = g_fTotalHistoryBatchWeightAccumulator;
    if(g_fWeightConversionFactor != 1.0f)
    fTemp *= g_fWeightConversionFactor;
    uValue.lValue = (long) (10 * fTemp);
    g_arrnMBTable[BATCH_SUMMARY_WEIGHT_ACCUMULATOR_DOUBLE] = uValue.nValue[0]; // copy msb
    g_arrnMBTable[BATCH_SUMMARY_WEIGHT_ACCUMULATOR_DOUBLE + 1] = uValue.nValue[1]; // copy lsb



}



//////////////////////////////////////////////////////
// CopyWeightsToMB  asm = COPYMBDIAGNOSTICS
//
//
// P.Smith                          4-4-2005
//
//////////////////////////////////////////////////////
void CopyDiagnosticsToMB( void )
{
   union CharsAndWord   nSmall;
   union WordAndDWord   uValue;
   union CharsAndLong   dValue;
   union CharsAndWord   uTempCI;
   int   i;

   nSmall.cValue[0] = g_PowerUpTime[TIME_MINUTE];        // M.s.Byte.
   nSmall.cValue[1] = g_PowerUpTime[TIME_HOUR];          //   l.s.Byte.

   g_arrnMBTable[BATCH_POWER_UP_TIME_HOUR_SW] = g_PowerUpTime[TIME_HOUR];
   g_arrnMBTable[BATCH_POWER_UP_TIME_MIN_SW] = g_PowerUpTime[TIME_MINUTE];


   g_arrnMBTable[BATCH_POWER_UP_TIME] = nSmall.nValue;   //.

   nSmall.cValue[0] = g_PowerUpTime[TIME_DATE];        // M.s.Byte.
   nSmall.cValue[1] = g_PowerUpTime[TIME_MONTH];          //   l.s.Byte.
   g_arrnMBTable[BATCH_POWER_UP_TIME_DATE_SW]   = g_PowerUpTime[TIME_DATE];
   g_arrnMBTable[BATCH_POWER_UP_TIME_MONTH_SW] = g_PowerUpTime[TIME_MONTH];
   g_arrnMBTable[BATCH_POWER_UP_DATE] = nSmall.nValue;   //.

   g_arrnMBTable[BATCH_POWER_UP_RESET_COUNTER] = g_ProcessData.m_nResetCounter;


   g_arrnMBTable[BATCH_CAL_MODULE_NO] = g_CalibrationData.m_nCalibrationModuleSerialNo; //calibration module serial no

// nbb serial no
   for(i=0; i < 8; i++ )
   {
       nSmall.cValue[0] = g_sDescriptionData.m_cNBBSerialNo[i*2];        // M.s.Byte.
       nSmall.cValue[1] = g_sDescriptionData.m_cNBBSerialNo[(i*2)+1];          //   l.s.Byte.
       g_arrnMBTable[BATCH_NBB_SERIAL_NUMBER+i] = nSmall.nValue;   //.
   }

// good tx counter for network comms
   uValue.lValue = g_sModbusNetworkSlave.m_lGoodTx;
   g_arrnMBTable[BATCH_NETWORK_TX_CTR]   = uValue.nValue[0];
   g_arrnMBTable[BATCH_NETWORK_TX_CTR+1] = uValue.nValue[1];


// good tx counter to optimisation card
   uValue.lValue = structOptimisationData.m_lGoodTx;
   g_arrnMBTable[BATCH_TX_CTR_1]   = uValue.nValue[0];
   g_arrnMBTable[BATCH_TX_CTR_1+1] = uValue.nValue[1];




// good tx counter to SEI
   uValue.lValue = structSEIData.m_lGoodTx;
   g_arrnMBTable[BATCH_TX_CTR_2]   = uValue.nValue[0];
   g_arrnMBTable[BATCH_TX_CTR_2+1] = uValue.nValue[1];

// good tx counter to Low Level Sensor card
   uValue.lValue = structLLSData.m_lGoodTx;
   g_arrnMBTable[BATCH_TX_CTR_3]   = uValue.nValue[0];
   g_arrnMBTable[BATCH_TX_CTR_3+1] = uValue.nValue[1];

// good rx counter to network comms
   uValue.lValue =g_sModbusNetworkSlave.m_lGoodRx;
   g_arrnMBTable[BATCH_NETWORK_RX_CTR] = uValue.nValue[0];  //msb
   g_arrnMBTable[BATCH_NETWORK_RX_CTR+1] = uValue.nValue[1];  //lsb

// good rx counter to optimisation card
   uValue.lValue =structOptimisationData.m_lGoodRx;
   g_arrnMBTable[BATCH_RX_CTR_1] = uValue.nValue[0];  //msb
   g_arrnMBTable[BATCH_RX_CTR_1+1] = uValue.nValue[1];  //lsb


// good rx counter for SEI
   uValue.lValue = structSEIData.m_lGoodRx;
   g_arrnMBTable[BATCH_RX_CTR_2] = uValue.nValue[0];  //msb
   g_arrnMBTable[BATCH_RX_CTR_2+1] = uValue.nValue[1];  //lsb

// good rx counter for Low Level Sensor card
   uValue.lValue = structLLSData.m_lGoodRx;
   g_arrnMBTable[BATCH_RX_CTR_3] = uValue.nValue[0];  //msb
   g_arrnMBTable[BATCH_RX_CTR_3+1] = uValue.nValue[1];  //lsb


// network checksum error counter
   uValue.lValue =g_sModbusNetworkSlave.m_lChecksumErrorCounter;
   g_arrnMBTable[BATCH_NETWORK_CSUM_ERR_CTR] = uValue.nValue[0];  //msb
   g_arrnMBTable[BATCH_NETWORK_CSUM_ERR_CTR+1] = uValue.nValue[1];  //lsb


// checksum error counter for remote display
   uValue.lValue = structOptimisationData.m_lChecksumErrorCounter;
   g_arrnMBTable[BATCH_CSUM_ERR_CTR_1]   = uValue.nValue[0];
   g_arrnMBTable[BATCH_CSUM_ERR_CTR_1+1] = uValue.nValue[1];

// checksum error counter for SEI
   uValue.lValue = structSEIData.m_lChecksumErrorCounter;
   g_arrnMBTable[BATCH_CSUM_ERR_CTR_2]   = uValue.nValue[0];
   g_arrnMBTable[BATCH_CSUM_ERR_CTR_2+1] = uValue.nValue[1];

// checksum error counter for Low Level Sensor card
   uValue.lValue = structLLSData.m_lChecksumErrorCounter;
   g_arrnMBTable[BATCH_CSUM_ERR_CTR_3]   = uValue.nValue[0];
   g_arrnMBTable[BATCH_CSUM_ERR_CTR_3+1] = uValue.nValue[1];

// timeout error counter for remote display
   uValue.lValue = structOptimisationData.m_lRxTimeoutCounter;
   g_arrnMBTable[BATCH_TOUT_CTR_1]   = uValue.nValue[0];
   g_arrnMBTable[BATCH_TOUT_CTR_1+1] = uValue.nValue[1];

// timeout error counter for SEI
   uValue.lValue = structSEIData.m_lRxTimeoutCounter;
   g_arrnMBTable[BATCH_TOUT_CTR_2]   = uValue.nValue[0];
   g_arrnMBTable[BATCH_TOUT_CTR_2+1] = uValue.nValue[1];

// timeout error counter for Low Level Sensor
   uValue.lValue = structLLSData.m_lRxTimeoutCounter;
   g_arrnMBTable[BATCH_TOUT_CTR_3]   = uValue.nValue[0];
   g_arrnMBTable[BATCH_TOUT_CTR_3+1] = uValue.nValue[1];

    g_arrnMBTable[BATCH_SUMMARY_CYCLE_COUNTER] = g_nCycleCounter;   // batch cycle counter.
// --review-- copy on board eeprom data to g_OnBoardCalData on board EEPROM
    g_arrnMBTable[BATCH_SBB_MODULE_NO] = g_OnBoardCalData.m_nSBBModuleSerialNo;



/* merge COPYENCRYPTIONDATATOMB inline */

    CopyEncryptionDataToMB();

//    dValue.lValue = gConfigRec.ip_Addr;
   dValue.lValue = EthernetIP;

    g_arrnMBTable[NETBURNER_CONFIG_IPADDRESS] = dValue.cValue[0];
    g_arrnMBTable[NETBURNER_CONFIG_IPADDRESS+1] = dValue.cValue[1];
    g_arrnMBTable[NETBURNER_CONFIG_IPADDRESS+2] = dValue.cValue[2];
    g_arrnMBTable[NETBURNER_CONFIG_IPADDRESS+3] = dValue.cValue[3];
//    iprintf("\n ip address is %x",dValue.lValue);


//    dValue.lValue = gConfigRec.ip_Mask;
    dValue.lValue = EthernetIpMask;

    g_arrnMBTable[NETBURNER_CONFIG_IPMASK] = dValue.cValue[0];
    g_arrnMBTable[NETBURNER_CONFIG_IPMASK+1] = dValue.cValue[1];
    g_arrnMBTable[NETBURNER_CONFIG_IPMASK+2] = dValue.cValue[2];
    g_arrnMBTable[NETBURNER_CONFIG_IPMASK+3] = dValue.cValue[3];

//    dValue.lValue = gConfigRec.ip_GateWay;
    dValue.lValue = EthernetIpGate;

    g_arrnMBTable[NETBURNER_CONFIG_IPGATEWAY] = dValue.cValue[0];
    g_arrnMBTable[NETBURNER_CONFIG_IPGATEWAY+1] = dValue.cValue[1];
    g_arrnMBTable[NETBURNER_CONFIG_IPGATEWAY+2] = dValue.cValue[2];
    g_arrnMBTable[NETBURNER_CONFIG_IPGATEWAY+3] = dValue.cValue[3];


//    dValue.lValue = gConfigRec.ip_DNS_server;
    dValue.lValue = EthernetDNS;

    g_arrnMBTable[NETBURNER_CONFIG_IPDNS] = dValue.cValue[0];
    g_arrnMBTable[NETBURNER_CONFIG_IPDNS+1] = dValue.cValue[1];
    g_arrnMBTable[NETBURNER_CONFIG_IPDNS+2] = dValue.cValue[2];
    g_arrnMBTable[NETBURNER_CONFIG_IPDNS+3] = dValue.cValue[3];

    for(i=0; i < 6; i++ )
    {
        g_arrnMBTable[NETBURNER_CONFIG_MACADDRESS + i] = gConfigRec.mac_address[i];
    }
    g_arrnMBTable[NETBURNER_DHCP_STATUS] = g_bDHCP;
    g_arrnMBTable[BATCH_IS_DIAGNOSTICS_PASSWORD] =  g_bDiagnosticsPasswordInUse;

    for(i=0; i<HARDWARESERIALNO; i++)
    {
       g_arrnMBTable[BATCH_HARDWARE_ID+i] = DS2432Buffer[i];
    }
// comms status copied to modbus table

    g_arrnMBTable[BATCH_COMMS_STATUS_SEI] = g_nBatchCommsStatusSEI;
    g_arrnMBTable[BATCH_COMMS_STATUS_LLS] = g_nBatchCommsStatusLLS;
    g_arrnMBTable[BATCH_COMMS_STATUS_OPTIMISATION] = g_nBatchCommsStatusOptimisation;
    g_arrnMBTable[BATCH_COMMS_STATUS_NETWORK] = g_nBatchCommsStatusNetwork;
    g_arrnMBTable[BATCH_COMMS_STATUS_TCPIP] = g_nBatchCommsStatusTCPIP;
    g_arrnMBTable[BATCH_COMMS_STATUS_PANEL] = g_nBatchCommsStatusPanel;
    g_arrnMBTable[BATCH_COMMS_STATUS_VAC8S] = g_nBatchCommsStatusVacuumLoading;


// sei software version no

    for(i=0; i<(SEI_SOFTWARE_REVISION_STRING_SIZE/2); i++)
    {
        g_arrnMBTable[BATCH_SEI_SOFTWARE_VERSION_NO+i] = g_arrnReadSEIMBTable[22+i];
        uTempCI.nValue = g_arrnMBTable[BATCH_SEI_SOFTWARE_VERSION_NO+i];        //
        g_cSEISoftwareVersionNumber[i*2] = uTempCI.cValue[0];
        g_cSEISoftwareVersionNumber[(i*2)+1] = uTempCI.cValue[1];

    }
    g_cSEISoftwareVersionNumber[SEI_SOFTWARE_REVISION_STRING_SIZE-1] = '\0';  // terminate string


// sei hardware revision no
    for(i=0; i<5; i++)
    {
        g_arrnMBTable[BATCH_SEI_HARDWARE_ID+i] = g_arrnReadSEIMBTable[27+i];
    }

    if(g_nDisableSDCardAccessCtr !=0)
    {
        g_arrnMBTable[BATCH_SD_CARD_DISABLE_ACCESS_CTR] = 1;
    }
    else
    {
        g_arrnMBTable[BATCH_SD_CARD_DISABLE_ACCESS_CTR] = 0;
    }

    if(g_sDescriptionData.m_cNBBTestPassFailStatus == 'P')
    {
        g_arrnMBTable[BATCH_NBB_PASS_FAIL_STATUS] = NBBTEST_PASS_STATUS;
    }
    else if(g_sDescriptionData.m_cNBBTestPassFailStatus == 'F')
    {
        g_arrnMBTable[BATCH_NBB_PASS_FAIL_STATUS] = NBBTEST_FAIL_STATUS;
    }
    else
    {
        g_arrnMBTable[BATCH_NBB_PASS_FAIL_STATUS] = NBBTEST_UNRECOGNISED_STATUS;
    }



}


//////////////////////////////////////////////////////
// CopyBBAuxToMB                    asm = COPYBBAUXMB
//
// Copy auxiliary or extra poduction summary data to MB. - called once per second from CopyProdSummDataToMB1Hz
//
// M.McKiernan                          22-11-2004
// First Pass
//
//////////////////////////////////////////////////////
void CopyBBAuxToMB( void )
{
int nTemp = 0;
int nIndex,i;
float fValue;
union WordAndDWord uValue;


    if(g_bStopped)
        nTemp |= BATCH_STATUS_BLENDER_STOPPED;
    if((g_nPauseFlag & ANY_PAUSE_BITS) || (g_nCyclePauseType == END_OF_CYCLE_PAUSE_TYPE))
        nTemp |= BATCH_STATUS_PAUSE;
    if( g_bMixOnStatus )
        nTemp |= BATCH_STATUS_MIXER_ON;
    if( g_bExtruderControlStartUpFlag )     // STRUPF - Auto start-up
        nTemp |= BATCH_STATUS_STARTUP_MODE;

    if( g_bDumpStatus)  // DUMPSTATUS
        nTemp |= BATCH_STATUS_DUMP_FLAP_OPEN;

    if(g_bHighLevelSensor == UNCOVERED)       //asm = HIGHLEVELSENSOR
        nTemp |= BATCH_STATUS_HIGH_LEVEL_SENSOR;

    if(g_bLowLevelSensor == UNCOVERED)            //asm = LOWLEVELSENSOR.
        nTemp |= BATCH_STATUS_LOW_LEVEL_SENSOR;

    if(g_bLevSen1Stat == UNCOVERED)               //asm = LEVSEN1STAT
        nTemp |= BATCH_STATUS_MAIN_LEVEL_SENSOR;

    if( g_bBypassStatus == OPEN  )
        nTemp |= BATCH_STATUS_BYPASS_VALVE;

    if( g_bFeedValveClosed )
        nTemp |= BATCH_STATUS_FLOW_CONTROL_VALVE;

// asm = UPOPSTATS copied inline

    if(g_bHiRegLevelSensor)
    nTemp |= BATCH_STATUS_HIGH_REGRIND_LEVEL_SENSOR;

    if(g_bResetFlag)
        nTemp |= BATCH_STATUS_RESET_OCCURRED_MASK;

    if(g_bManualAutoFlag)   // Auto ?
        nTemp |= BATCH_STATUS_AUTOMANUAL_MASK;



    // Store the assembled operational status to Modbus. (asm MBOPSTS)
    g_arrnMBTable[BATCH_SUMMARY_OPERATIONAL_STATUS] = nTemp;

//2nd operational status word. - MBOPSTS2
    nTemp = 0;
    if( g_bTaringStatus )
        nTemp |= BATCH_STATUS_2_TARE_MASK;  //TARINGSTATUS.

    if( g_bBlenderTopCleanOn )                                  //asm = AIRJET1ONF
        nTemp |= BATCH_STATUS_2_BLENDER_CLEAN_MASK;     // MBSTATBLENDERCLEANBPOS

    if( g_bBlenderMixerCleanOn )                            //
        nTemp |= BATCH_STATUS_2_MIXER_CLEAN_MASK;       // MBSTATMIXERCLEANBPOS

    if( g_bRightCornerCleanOn )
        nTemp |= BATCH_STATUS_2_RIGHT_CORNER_CLEAN_MASK;        // MBSTATRCCBPOS

    if( g_bLeftCornerCleanOn )
        nTemp |= BATCH_STATUS_2_LEFT_CORNER_CLEAN_MASK;     // MBSTATLCCBPOS

    if( g_bSetCleanWhenCycleEnd )                                   // SETCLEANWHENCYCLEEND
        nTemp |= BATCH_STATUS_2_CLEAN_LAST_BATCH_MASK;      // MBSTATCLEANLASTBATCHBPOS

    if( g_bCycleIndicate )                                      // Cycle in progress
        nTemp |= BATCH_STATUS_2_CYCLE_INDICATE_MASK;        // MBSTATCYCLEINDICATE

    if(g_bRecipeWaiting)
        nTemp |= BATCH_STATUS_2_RECIPE_WAITING;        //  MBRECIPEWAITINGBITPOS

    if(g_nPauseFlag & VAC_INHIBIT_PAUSE_BIT)
        nTemp |= BATCH_STATUS_2_BLENDER_IDLE;           //  MBINHIBITBITPOS

    if(g_nCleanCycleSeq == CLEANPHASE1ID)
        nTemp |= BATCH_STATUS_2_CLEAN_PHASE_1_MASK;     // MBBLENDERCLEANBITPOS

    if(g_nCleanCycleSeq == CLEANPHASE2ID)
        nTemp |= BATCH_STATUS_2_CLEAN_PHASE_2_MASK;     // MBMIXERCLEANBITPOS


    g_arrnMBTable[BATCH_SUMMARY_OPERATIONAL_STATUS_2] = nTemp;


// fill status.

    for(i=0; i < MAX_COMPONENTS; i++)    //--review-- modbus expan;sion
    {

        if(i < 10)
        {
            nIndex = PSUM_BLOCK_1 + (i * PSUM_BLOCK_SIZE) + PSUM_SLIDE_OFFSET;
        }
        else
        {
            nIndex = PSUM_BLOCK_11 + ((i-10) * PSUM_BLOCK_SIZE) + PSUM_SLIDE_OFFSET;
        }

        if( g_bLFillStatCmp[i] == ON )  // fill is on??
        {
            g_arrnMBTable[nIndex] = 1;
        }
        else
            g_arrnMBTable[nIndex] = 0;
    }

// Low level sensor status.
    for(i=0; i < MAX_COMPONENTS; i++)
    {
        nIndex = BATCH_SUMMARY_LOW_LEVEL_SENSOR_1 + (i * PSUM_BLOCK_SIZE ); // low level sensor status is in component block.
        if( g_bLowLevelSensorStatus[i] == UNCOVERED )   // Uncovered, sensor is off??
       {
            g_arrnMBTable[nIndex] = 1;
        }
        else
            g_arrnMBTable[nIndex] = 0;
    }

    CopyFBWtsToMB();    // Copy fill by weight" bits into MB

    g_arrnMBTable[BATCH_SBB_ADDRESS] = (int)g_cBlenderNetworkAddress;

    nTemp = g_arrnMBTable[BATCH_DIAG_OPTIONS];

    if( g_bL1Fitted )
    {
        g_arrnMBTable[BATCH_DIAG_LINK_L1] = 1;
    }
    else
    {
        g_arrnMBTable[BATCH_DIAG_LINK_L1] = 0;
    }

    if( g_bL2Fitted )
    {
        g_arrnMBTable[BATCH_DIAG_LINK_L2] = 1;
    }
    else
    {
        g_arrnMBTable[BATCH_DIAG_LINK_L2] = 0;
    }

    if( g_bL3Fitted )
    {
        g_arrnMBTable[BATCH_DIAG_LINK_L3] = 1;
    }
    else
    {
        g_arrnMBTable[BATCH_DIAG_LINK_L3] = 0;
    }

        g_arrnMBTable[BATCH_DAC_KGH] = (int)(10 * g_ProcessData.m_fDtoAPerKilo );   //DACKGHR  D/A PER KGH - to 1 DP.

    if(g_CalibrationData.m_nControlType == CONTROL_TYPE_INC_DEC) // asm = INCREASEDECREASE
    {
        g_arrnMBTable[BATCH_DAC_MPM] = (int)(100 * (g_ProcessData.m_fRPMPerKilo+0.005 ));   // rpm per kg nbb--todo--remove
    }
    else
    {
        g_arrnMBTable[BATCH_DAC_MPM] = (int)(10 * g_ProcessData.m_fDtoAPerMeterPerMinute);   //DACPMPM  D/A PER M/min - to 1 DP.
    }



    g_arrnMBTable[BATCH_SEI_INPUTS] = g_nSEIInputStates;    //SEIINPUTSSTATE  inputs on SEI
    g_arrnMBTable[BATCH_SEI_ALARMS] = g_nSEIAlarms;         //SEIALARMSTATE  alarms on SEI

    g_nLowLevelSensorInputState = g_arrnReadLLSMBTable[MB_LLS_INPUTS] ;

    g_arrnMBTable[BATCH_DIAG_LLS_INPUT_STATUS] = g_nLowLevelSensorInputState;

    g_arrnMBTable[BATCH_SINGLE_RECIPE_CURRENT_EXTRUDER] =   g_nCurrentExtruderNo; // copy current extruder no

    g_arrnMBTable[BATCH_SINGLE_RECIPE_RECIPE_PENDING] = g_nExtruderNo;            // copy pending extruder no
    g_arrnMBTable[BATCH_SINGLE_RECIPE_VACUUM_SEQUENCE] = g_nVacuumSequence;       // copy sequence to show current status of vacuum
    g_arrnMBTable[BATCH_EXTRUDER_SET_PERCENTAGE] = (int)(g_fExtruderDAValuePercentage * 10);       // d/a percentage copy to modbus


    fValue =    g_fLongTermLengthCounter;
    fValue *=    g_fLengthConversionFactor;
    fValue += 0.5;                                                                          // for rounding to nearest  integer.
    uValue.lValue = (long)fValue;                                                       // Length is in m.
    g_arrnMBTable[BATCH_CYCLE_LONG_TERM_LENGTH] = uValue.nValue[0];          // M.s.Byte. -
    g_arrnMBTable[BATCH_CYCLE_LONG_TERM_LENGTH + 1] = uValue.nValue[1];  // L.s.Byte.

    fValue =    g_fLongTermResettableLengthCounter;
    fValue *=    g_fLengthConversionFactor;
    fValue += 0.5;                                                                          // for rounding to nearest  integer.
    uValue.lValue = (long)fValue;                                                       // Length is in m.
    g_arrnMBTable[BATCH_CYCLE_LONG_TERM_LENGTH_RESETTABLE] = uValue.nValue[0];          // M.s.Byte. -
    g_arrnMBTable[BATCH_CYCLE_LONG_TERM_LENGTH_RESETTABLE + 1] = uValue.nValue[1];  // L.s.Byte.

    fValue = g_fMaxKgPHrPossible;
    if(g_fWeightConversionFactor != 1.0f)
    fValue *=    g_fWeightConversionFactor;
    fValue += 0.5;   // round to nearest kg                                                                      // for rounding to nearest  decimal digit.
    g_arrnMBTable[BATCH_MAX_KGPH_POSSIBLE] = (int)fValue;        // Throughput is to 1dp

}

//////////////////////////////////////////////////////
// SetUpConversionFactors
//
// Set up modbus conversion factors   25/5/06
//
//////////////////////////////////////////////////////
void SetUpConversionFactors( void )
{
    if((g_CalibrationData.m_nUnitType == UNIT_IMPERIAL) || (g_CalibrationData.m_nUnitType == UNIT_IMPERIAL_MODBUS))
    {
        g_fWeightConversionFactor = WEIGHT_CONVERSION_FACTOR;
        g_fLengthConversionFactor = LENGTH_CONVERSION_FACTOR;
        if(g_CalibrationData.m_nBlenderMode == MODE_GP12SQFEET)
        {
            g_fWeightPerLengthConversionFactor = 1.0f;         // return calculated g/m value
        }
        else
        {
            g_fWeightPerLengthConversionFactor = WEIGHT_PER_LENGTH_CONVERSION_FACTOR;
        }
        g_fLineSpeedConversionFactor = LINE_SPEED_CONVERSION_FACTOR;
        g_fWidthConversionFactor = WIDTH_CONVERSION_FACTOR;
        g_fWeightPerAreaConversionFactor = WEIGHT_PER_AREA_CONVERSION_FACTOR;
        g_fThroughputConversionFactor = THROUGHPUT_CONVERSION_FACTOR;

    }
    else  //metric
    {
        g_fWeightConversionFactor = 1.0f;
        g_fLengthConversionFactor = 1.0f;
        g_fWeightPerLengthConversionFactor = 1.0f;
        g_fLineSpeedConversionFactor = 1.0f;
        g_fWidthConversionFactor = 1.0f;
        g_fWeightPerAreaConversionFactor = 1.0f;
        g_fThroughputConversionFactor = 1.0f;
    }
}



//////////////////////////////////////////////////////
// CheckForRecipeDownLoad
//
// Checks for recipe download
//
//////////////////////////////////////////////////////
void CheckForRecipeDownLoad( void )
{
    unsigned char cErrorCode;
	if(g_arrnMBTable[BATCH_SETPOINT_DOWNLOAD_MANAGER] == ALLOW_DOWNLOAD)
    {
        cErrorCode = CheckMBValidData();						// some checks to see if data written is valid. (asm = CHMBVDATA)
        g_arrnMBTable[BATCH_SETPOINT_DOWNLOAD_MANAGER] = 0;
    }
}


///////////////////////////////////////////////////
// CopySdCardDetailsToMB
//
// copy email config data to modbus
//
//////////////////////////////////////////////////////
void CopySdCardDetailsToMB( void )
{
   char cBuffer[MAX_SD_CARD_FILE_NAME_SIZE];
   unsigned int i,j,k;
    BOOL bSwap = TRUE;
    union CharsAndWord uTempRev;
    union WordAndDWord uValue;


// Email server
    // copy file directory to mb table.

    for(i=0; i<(MAX_SD_CARD_FILES); i++)
    {
        for(k=0; k<(MAX_SD_CARD_FILE_NAME_SIZE); k++)
        {
            cBuffer[k] = 0;
        }

        sprintf( cBuffer, g_nSdCardFileDetails[i].filename );

        for(j=0; j<(MAX_SD_CARD_FILE_NAME_SIZE/2); j++)
        {
            if(bSwap)
            {
                uTempRev.cValue[1] = cBuffer[j*2];
                uTempRev.cValue[0] = cBuffer[(j*2)+1];
//                SetupMttty();
//                iprintf("\n chars is %c %c  %x  %x",uTempRev.cValue[1],uTempRev.cValue[0],uTempRev.cValue[1],uTempRev.cValue[0]);
            }
            else
            {
                uTempRev.cValue[0] = cBuffer[j*2];
                uTempRev.cValue[1] = cBuffer[(j*2)+1];
            }
            g_arrnMBTable[SD_CARD_FILE_NAME_1 + (i * MB_SD_CARD_FILE_NAME_SIZE)+j] = uTempRev.nValue;
        }
    }
    g_arrnMBTable[SD_CARD_FILE_NO_COUNTER] = g_nNoOfFilesOnSdCard;  // copy no of files on sd card.
    g_arrnMBTable[USB_COPY_IN_PROGRESS] = g_bUSBCopyInProgress;  // copy no of files on sd card.
    uValue.lValue = g_lUSBFileDataCopied;
//    g_arrnMBTable[USB_FILE_DATA_COPIED] = uValue.nValue[0];     // M.s.Byte.  //nbb--todo-- put back
//    g_arrnMBTable[USB_FILE_DATA_COPIED + 1] = uValue.nValue[1];     // L.s.Byte.

    g_arrnMBTable[SD_CARD_SOFTWARE_UPDATE_COMPLETE] =  g_bSDCardSoftwareUpdateComplete;
    g_arrnMBTable[SD_CARD_LOAD_CONFIG_COMPLETE] =  g_bSDCardLoadConfigComplete;
    g_arrnMBTable[SD_CARD_LOAD_BACKUP_CONFIG_COMPLETE] =  g_bSDCardLoadBackupConfigComplete;
    g_arrnMBTable[SD_CARD_SAVE_CONFIG_COMPLETE] =  g_bSDCardSaveConfigComplete;
    g_arrnMBTable[SD_CARD_COPY_CONFIG_TO_BACKUP_COMPLETE] =  g_bSDCardCopyConfigToBackupComplete;

    g_arrnMBTable[BATCH_SUMMARY_SD_CARD_ERROR_COUNTER] = g_ProcessData.m_nSdCardErrorCtr;
    g_arrnMBTable[BATCH_SUMMARY_SD_CARD_MOUNT_FAT_RETURN_ON_ERROR] = g_ProcessData.m_nMountFatReturnedValueOnSdError;
    g_arrnMBTable[BATCH_SUMMARY_SD_CARD_CHDRIVE_RETURN_ON_ERROR] = g_ProcessData.m_nChangeDriveReturnedValueOnSdError;
    g_arrnMBTable[BATCH_SUMMARY_SD_CARD_FP_RETURN_ON_ERROR] = g_ProcessData.m_nFilePointerOnSdError;
    for(j=0; j<(SD_CARD_NAME_SIZE/2); j++)
    {
        uTempRev.cValue[0] = g_ProcessData.m_cFileNameOnSdCardError[j*2];
        uTempRev.cValue[1] = g_ProcessData.m_cFileNameOnSdCardError[(j*2)+1];
        g_arrnMBTable[BATCH_SUMMARY_SD_CARD_FILE_NAME_ON_ERROR+j] = uTempRev.nValue;
    }
}




///////////////////////////////////////////////////
// PollUSBMB
//
// check for usb related commands.
//
//////////////////////////////////////////////////////
/*
void PollUSBMB( void )
{
    if(g_arrnMBTable[USB_READ_SD_CARD_DIRECTORY] != 0 )
    {
        g_arrnMBTable[USB_READ_SD_CARD_DIRECTORY] = 0;
        g_bReadSDCardDirectory = TRUE;
    }
    if(g_arrnMBTable[USB_COPY_FILE_TO_SD_CARD] != 0 )
    {
        g_nCopyFileNoToUSBRequest = g_arrnMBTable[USB_COPY_FILE_TO_SD_CARD];
//        SetupMttty();
//        iprintf("\n g_nCopyFileNoToUSB is %d",g_nCopyFileNoToUSB);
        g_arrnMBTable[USB_COPY_FILE_TO_SD_CARD] = 0;
    }
}



*/


//////////////////////////////////////////////////////
// StoreMBSetpointHistoryArray
//
// COPY all Setpoint data a duplicate array for comparison purposes.
//
//
// M.McKiernan                          25-04-2008
// First Pass
//
//////////////////////////////////////////////////////
void StoreMBSetpointHistoryArray( void )
{
int i;

   for(i=0; i<MB_TABLE_SETPOINT_SIZE; i++)
   {
      g_arrnMBSetpointHistory[i] = g_arrnMBTable[i];
   }
}
//////////////////////////////////////////////////////
// CompareMBSetpointHistoryArray
//
// Compares all Setpoint data with a duplicate (history) array for comparison purposes.
// returns TRUE is all data same.
//
// M.McKiernan                          25-04-2008
// First Pass
//
//////////////////////////////////////////////////////
BOOL CompareMBSetpointHistoryArray( void )
{
int i;
BOOL bSame = TRUE;
   for(i=0; i<MB_TABLE_SETPOINT_SIZE; i++)
   {
      if( g_arrnMBSetpointHistory[i] != g_arrnMBTable[i] )
         bSame = FALSE;
   }

   return(bSame);
}

//////////////////////////////////////////////////////
// CopyAlarmsToIndividualMBWords     (from asm = COPYSYSTEMPROCESSALARMSTOWORDS)
// Copy mb process and system alarms to individual modbus cells
//
//
//
//////////////////////////////////////////////////////
void CopyAlarmsToIndividualMBWords( void )
{
unsigned int i;
WORD nAlarms;
// system alarms
    nAlarms = g_arrnMBTable[BATCH_SUMMARY_SYSTEM_ALARMS];
    for(i=0; i<16; i++)
    {
        if(nAlarms & 0x01 !=0)
        {
            (g_arrnMBTable[BATCH_SUMMARY_SYSTEM_INDIVIDUAL_WORDS_START + i]) = 1;   // set individual word to 1
        }
        else
        {
            (g_arrnMBTable[BATCH_SUMMARY_SYSTEM_INDIVIDUAL_WORDS_START + i]) = 0;
        }
    nAlarms >>= 1;                  // shift alarm one bit to the right
    }

// process alarms
    nAlarms = g_arrnMBTable[BATCH_SUMMARY_PROCESS_ALARMS];
    for(i=0; i<16; i++)
    {
        if(nAlarms & 0x01 !=0)
        {
            (g_arrnMBTable[BATCH_SUMMARY_PROCESS_INDIVIDUAL_WORDS_START + i]) = 1;   // set individual word to 1
        }
        else
        {
            (g_arrnMBTable[BATCH_SUMMARY_PROCESS_INDIVIDUAL_WORDS_START + i]) = 0;
        }
    nAlarms >>= 1;                  // shift alarm one bit to the right
    }
}

//////////////////////////////////////////////////////
// CopyFunctionRunningToMB
// Copy function running to mb table
//
//
//////////////////////////////////////////////////////
void CopyFunctionRunningToMB( void )
{
    g_arrnMBTable[BATCH_FUNCTION_RUNNING] =  g_nFunctionRunning;
    g_arrnMBTable[BATCH_FUNCTION_SUBSECTION_RUNNING] =  g_nFunctionSubSectionRunning;
    g_arrnMBTable[BATCH_BATSEQ] =  (int)g_cBatSeq;
    g_arrnMBTable[BATCH_USB_READ_TIMEOUT_CTR] =  (int)g_nUSBReadTimeoutCtr;
    g_arrnMBTable[BATCH_USB_COMMAND_SEND_TIMEOUT_CTR] =  (int)g_nUSBCommandSendTimeoutCtr;
    g_arrnMBTable[BATCH_USB_FLUSH_TIMEOUT_CTR] =  (int)g_nUSBFlushTimeoutCtr;
    g_arrnMBTable[BATCH_USB_WRITE_TIMEOUT_CTR] =  (int)g_nUSBWriteTimeoutCtr;
    g_arrnMBTable[BATCH_USB_ECHOED_TIMEOUT_CTR] =  (int)g_nUSBEchoedTimeoutCtr;
    g_arrnMBTable[BATCH_USB_SEND_FILE_TIMEOUT_CTR] =  (int)g_nUSBUSBSendFileCommandsTimeoutCtr;
    g_arrnMBTable[BATCH_USB_PRINTER_STATUS_TIMEOUT_CTR] =  (int)g_nUSBGetPrinterStatusTimeoutCtr;
    g_arrnMBTable[BATCH_FOREGROUND_CTR] =  g_wForeGroundCounter;
    g_arrnMBTable[BATCH_FOREGROUND_ENTRY_EXIT] =  g_wProgramLoopEntry;

}



//////////////////////////////////////////////////////
// LoadPanelVersionNumber
// copies panel no from mb table to ram
//
//////////////////////////////////////////////////////
void LoadPanelVersionNumber( void )
{
    union CharsAndWord uTempCI;
    int  i;
    for(i=0; i<((PANEL_REVISION_STRING_SIZE-1)/2); i++)
    {
        uTempCI.nValue = g_arrnMBTable[BATCH_PANEL_VERSION_NUMBER + i];        // Modbus has two ascii chars in each register.
        // note that the panel version no needs to be swapped
        g_cPanelVersionNumber[i*2] = uTempCI.cValue[1];
        g_cPanelVersionNumber[(i*2)+1] = uTempCI.cValue[0];
    }
    g_cPanelVersionNumber[PANEL_REVISION_STRING_SIZE-1] = '\0';  // terminate string
}




//////////////////////////////////////////////////////
// LoadICSRecipeMBData
// saves isc recipe data
//
//////////////////////////////////////////////////////
void LoadICSRecipeMBData( void )
{
    union CharsAndWord uTempCI;
    unsigned int    i,j;
    for(j=0; j < g_CalibrationData.m_nComponents && j < MAX_COMPONENTS; j++)
    {
        for(i=0; i < (RECIPE_DESCRIPTION_STRING_LENGTH/2); i++)
        {
            uTempCI.nValue = g_arrnMBTable[BATCH_ICS_RECIPE_MATERIAL_CODE_1 + i];       // Modbus has two ascii chars in each register.
            g_CommsRecipeAdditionalData.m_arrMaterialDescription[j][i*2] = uTempCI.cValue[0];
            g_CommsRecipeAdditionalData.m_arrMaterialDescription[j][(i*2)+ 1] = uTempCI.cValue[1];
        }
    }
    g_bSaveAdditionalDataToEEPROM = TRUE;
}

//////////////////////////////////////////////////////
// CopyICSRecipeMBData
// reload recipe data
//
//////////////////////////////////////////////////////

void CopyICSRecipeMBData( void )
{
    union CharsAndWord uTempCI;
    unsigned int    i,j;

    for(j=0; j < g_CalibrationData.m_nComponents && j < MAX_COMPONENTS; j++)
    {
       for(i=0; i < (RECIPE_DESCRIPTION_STRING_LENGTH/2); i++)
        {
            uTempCI.cValue[0] = g_TempRecipeAdditionalData.m_arrMaterialDescription[j][i*2];
            uTempCI.cValue[1] = g_TempRecipeAdditionalData.m_arrMaterialDescription[j][(i*2)+1];
            g_arrnMBTable[BATCH_ICS_RECIPE_MATERIAL_CODE_1 + i] = uTempCI.nValue;
       }
    }
}

/*
;*****************************************************************************;
; FUNCTION NAME : COPYSYSTEMANDPROCESSALARMS                                             ;
; FUNCTION      : COPIES SYSTEM AND PROCESS ALARMS BITS TO WORDS
;*****************************************************************************;



COPYSYSTEMANDPROCESSALARMSTOWORDS:
        LDAB    #RAMBANK
        TBXK
        LDE     #16
        LDD     MBSYSALRM
        LDX     #MBSYSALARM1                        ;
REPMBW  PSHM    D
        ANDD    #01
        BNE     SETWR
        CLRW    0,X
        BRA     CONWR
SETWR   STD     0,X
CONWR   PULM    D
        LSRD
        AIX     #2
        ADDE    #-1
        BNE     REPMBW

        LDE     #16
        LDD     MBPROALRM
        LDX     #MBPROALARM1                        ;
REPMBW1 PSHM    D
        ANDD    #01
        BNE     SETWR1
        CLRW    0,X
        BRA     CONWR1
SETWR1  STD     0,X
CONWR1  PULM    D
        LSRD
        AIX     #2
        ADDE    #-1
        BNE     REPMBW1
        RTS


*/




/*        ;
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

*/




/*

;* COPY TOTAL WT.S INTO MODBUS TABLE PROD. SUMMMARY DATA
CPYTWTMB:
;       TST     PROTOCOL
;       LBPL     CPYTWTMX

        LDD     TOTWACCO+1       ; ORDER Wt.
        JSR     BCDHEX2          ; CONVERT 2 BYTES BCD  TO HEX
        STD     MBORDW+2
        CLRW    MBORDW

        LDX      #TOTWACCF       ;~FRONT ROLL WT ACCUMULATOR
        JSR      DIVWTBY2          ; WEIGHT / 2
        LDD     EREG+1            ;
        JSR     BCDHEX2          ; CONVERT 2 BYTES BCD  TO HEX
        STD     MBFRW
        LDX     #EREG
        JSR     CALCFSLITX      ; FRONT SLIT CALC.


        LDD     FSWTACC+1        ; FRONT SLIT ROLL WT.
        JSR     BCDHEX2          ; CONVERT 2 BYTES BCD  TO HEX
        STD     MBFSW

        TST    SHTTUBE
        BNE    CPYTWTBB           ;~IF SHEET (01) THEN DO BACK
        CLRW   MBBRW
        CLRW   MBBSW              ; TUBE, CLEAR BACK WT.S
        BRA    CPYTWTMX

CPYTWTBB:

        LDX      #TOTWACCB       ;~FRONT ROLL WT ACCUMULATOR
        JSR      DIVWTBY2          ; WEIGHT / 2
ISTUBFWTB:

        LDD     EREG+1            ;
        JSR     BCDHEX2          ; CONVERT 2 BYTES BCD  TO HEX
        STD     MBBRW

        LDX     #EREG
CALCSB  JSR     CALCFSLITX      ; FRONT SLIT CALC.

        LDD     BSWTACC+1        ; BACK SLIT ROLL WT.
        JSR     BCDHEX2          ; CONVERT 2 BYTES BCD  TO HEX
        STD     MBBSW
CPYTWTMX RTS
*/




/*
;* COPY ACTUAL BLEND %'s INTO MODBUS TABLE SETPOINT AREA
CPYAPCTMB:
        TST     PROTOCOL
        BPL     CPYBPTMX
        TST     SHOWINWEIGHT    ; AARE WE SHOW THE INSTANTANEOUS WEIGHT
        BNE     YESINWT
        LDX     #BPRRCNT1       ;  % #1
        BRA     SHOWP1          ; SHOW %

YESINWT LDX     #BPCNT1         ;  % #1
SHOWP1  LDAB   #RAMBANK
        TBZK                    ;~;SET EK FOR BANK 1, I.E. RAM
        LDZ     #MB1APCT        ; MODBUS TABLE, % #1
        LDAB    NOBLNDS         ; NO. OF BLENDS/COMPONENTS ON MACHINE

CPYBPT10 PSHB
        LDD     0,X
        JSR     BCDHEX2         ; CONVERT TO HEX
        STD     0,Z
        AIX     #2
        AIZ     #MBPSBKSIZ      ; BLOCK SIZE (PER COMPONENT)
        PULB
        DECB
        BNE     CPYBPT10

CPYBPT20 LDD    BPCNTLA         ; LIQUID ADD. %
        JSR     BCDHEX2
        STD     MBLAPCT
CPYBPTMX:
        RTS


*/
/*
;* COPY A/D VALUES INTO MODBUS TABLE PROD. SUMMMARY DATA AREA
CPYADCMB:
        TST     PROTOCOL
        BPL     CPYADCMX

        LDD     RAW132
        CLR     MB1ADC
        STD     MB1ADC+1
        LDAA    RAW132+2
        STAA    MB1ADC+3
CPYADCMX:
        RTS

*/
/*
;* COPY WEIGHT CONSTANTS INTO MODBUS TABLE CALIBRATION DATA AREA
CPYWTCMB:
        TST     PROTOCOL
        LBPL     CPYWTCMX
;           // Caib. weight constant.
       long          m_lTareCounts

        LDAB   #CON1OF
        LDX    #CALBLK
        ABX                        ;~X NOW POINTS TO C1 FOR THIS CHANNEL
        LDAB     1,X
        LDE      2,X
        JSR     BCDHEX3         ; BCD TO HEX.
        CLRW    MBCALCON        ; CLEAR MSB
        STE     MBCALCON+2


        LDX     #CH1CBLK       ; CALIBRATION BLOCK
; TARE VALUE.
        LDAB    OFFSOF,X
        LDE     OFFSOF+1,X      ; LOAD UP BCD TARE VALUE (3 BYTES)
        JSR     BCDHEX3         ; CONVERT TO HEX
        CLRA
        STD     MB1TARE
        STE     MB1TARE+2       ;TARE WAS MOVED TO +16 - VALUE T0 999,999

        LDD     MBNEWDATA
        ORD     #$0002          ; SET B1 TO INDICATE NEW CAL. DATA
        STD     MBNEWDATA
CPYWTCMX:
        RTS

*/
/*
;* COPY HOPPER CONSTANTS INTO MODBUS TABLE CALIBRATION DATA AREA
CPYHPCMB:
        LDAB   #RAMBANK
        TBZK                    ;~;SET EK FOR BANK 1, I.E. RAM
        LDZ     #MBCBLK1        ; MODBUS TABLE, CH #1 CAL BLOCK
        LDAB    #1              ; BLEND #, START AT 1
CPYHPC10 PSHB
        LDX      #DFTW1          ; FILL TARGET WT. (.1Kg)
        LDD     0,X             ; PICK UP TARGET WT
        JSR     BCDHEX2         ; CONVERT TO HEX
        STD     MB1FTW          ; STORE IN MODBUS TABLE

        PULB
        PSHB

        LDX     #MXTPT1         ; MAX THRUPUT (Kg/H)
        LDD     0,X             ; PICK UP MAX TPT
        LDD     #$0999          ; MAX KG/HR SET AT 99.9
        JSR     BCDHEX2         ; CONVERT TO HEX
        STD     MB1MXTP

        PULB

CPYHPC20:
        LDD     MBNEWDATA
        ORD     #$0002          ; SET B1 TO INDICATE NEW CALIB DATA
        STD     MBNEWDATA

        RTS

*/
/*
;* COPY MEAS. & CONTROL PARAMETERS INTO MODBUS TABLE CALIBRATION DATA AREA
CPYMCPMB:  BATCH_CALIBRATION_CONTROL_GAIN
        LDAB    CONTAVGFACTOR
        CLRA
        STD     MBAF1           ; MODBUS TABLE, AVG. FACTOR 1
        LDAB    WGTAVGFACTOR          ; AVERAGING FACTOR 2
        STD     MBAF2           ; MB TABLE AVG. FACTOR 2

        LDAB   MIXTIM
        JSR    BCDHEX1
        CLRA
        STD    MBMIXTIM

        LDAB   BATINKGHAVG
        JSR    BCDHEX1
        CLRA
        STD    MBBATINKGHAVG
        JSR    RESRROB   ; RESET AVERAGING

        LDAB    CLGAIN          ; CONTROL GAIN
        JSR     BCDHEX1
        CLRA
        STD     MBCG

        LDAB    CONTROLPER       ; CONTROL PERCENTAGE
        JSR     BCDHEX1
        CLRA
        STD     MBCONTROLPER     ; CONTROL %.


        RTS

*/
/*
       Bool          m_bStandardCompConfig;             // STDCCFG  Standard component config (YES/NO)STDCCFG
       int           m_nCompConfig[MAX_COMPONENTS];     // COMP1CFG Component config word for each comCOMP1CFGponent.
; COPY COMPONENT CONFIGURATION
// Component Configuration types
#define BATCH_STANDARD_LAYOUT           0
#define BATCH_NONSTANDARD_LAYOUT        1

#define BATCH_CALIBRATION_COMP1_CONFIG          131     // COMPONENT #1 CONGIGURATION   131 Bits 0 to 7 represent the output controlling
#define BATCH_CALIBRATION_COMP2_CONFIG          132         // each component. ls bit = Bin 1
#define BATCH_CALIBRATION_COMP3_CONFIG          133
#define BATCH_CALIBRATION_COMP4_CONFIG          134
#define BATCH_CALIBRATION_COMP5_CONFIG          135
#define BATCH_CALIBRATION_COMP6_CONFIG          136
#define BATCH_CALIBRATION_COMP7_CONFIG          137
#define BATCH_CALIBRATION_COMP8_CONFIG          138
#define BATCH_CALIBRATION_COMP9_CONFIG          139
#define BATCH_CALIBRATION_COMP10_CONFIG         140
#define BATCH_CALIBRATION_COMP11_CONFIG         141
#define BATCH_CALIBRATION_COMP12_CONFIG         142


CPYCCFGTMB:
        TST     PROTOCOL
        BPL     CPYCCFGX
        LDAB    STDCCFG            ; READ CONFIGURATION.
        STD     MBNONSTDCCFG
        LDD     COMP1CFG
        STD     MBCOMP1CFG
        LDD     COMP2CFG
        STD     MBCOMP2CFG
        LDD     COMP3CFG
        STD     MBCOMP3CFG
        LDD     COMP4CFG
        STD     MBCOMP4CFG
        LDD     COMP5CFG
        STD     MBCOMP5CFG
        LDD     COMP6CFG
        STD     MBCOMP6CFG
        LDD     COMP7CFG
        STD     MBCOMP7CFG
        LDD     COMP8CFG
        STD     MBCOMP8CFG
        LDD     COMP9CFG
        STD     MBCOMP9CFG
        LDD     COMP10CFG
        STD     MBCOMP10CFG
        LDD     COMP11CFG
        STD     MBCOMP11CFG
        LDD     COMP12CFG
        STD     MBCOMP12CFG
CPYCCFGX RTS


*/
/*

;* LOAD SYSTEM CONFIG. PARAMETERS FROM MODBUS TABLE -> CALIBRATION DATA AREA
LDMBSYC:
        LDD    MBNOBDS          ; SYSTEM CONFIGURATION INFO
        LBEQ   RELOADMB       ; RELOAD MOBUS AND EXIT.
        LDAA   MANATO
        BEQ    LDMBSYC04          ;~BRANCH IF IN MANUAL
        CLR    MANATO           ;~ELSE SET FOR MANUAL MODE
        JSR    EXCTRL           ;~EXIT AUTO CONTROL IF IT WAS IN AUTO
LDMBSYC04:
        LDD     MBLINENO        ; LINE NO.
        JSR     HEXBCD2
        STE     LINENO          ; NOTE; MAX VALUE IS 9999

        LDD     MBNOBDS         ; MODBUS TABLE, NO. OF BLENDS
        STAB    NOBLNDS         ; NOTE, EXPECTING 8 MAX
        JSR     CHECKPRODSETE100P ; DO 100% CHECK INCASE THIS HAS CHANGED.



        LDD     MBFILLRETRY     ; FILL RETRYS
        STAB    FILLRETRY       ;

        LDD     MBBLENDERMODE   ; READ BLENDER MODE.
        STAB    BLENDERMODE     ; MODE OF BLENDER OPERATION.

        LDD     MBCRLMOD
        STAB    CRLMODE         ; CONTROL MODE (EXTRUDER/PROP./HAULOFF)

        LDD     MBREGCOMP      ; REGRIND COMPONENT NO.
        JSR     HEXBCD1         ; TO BCD
        STAB    REGCOMP         ; REGRIND COMPONENT.

        LDD     MBFSTCTRL       ; FAST RESPONSE.
        STAB    FSTCTRL         ; FAST CONTROL RESPONSE.

        LDD     MBDIAGMODE      ; DIAGNOSTICS.
        STAB    DIAGMODE        ; DIAGNOSTIC MODE.

        LDD     MBVACLOAD       ; VACUUM LOADING.
        STAB    VACLOAD         ; VACUUM LOADING.

        LDD     MBBYPASSE       ; BYPASS
        STAB    BYPASSE         ; STORE

        LDD     MBISVOLMODE       ;
        STAB    ISVOLMODE         ; VOLUMETRIC MODE.

        LDD     MBEIOMITTED       ; SEI OMITTED ?
        STAB    EIOMITTED

        LDD     MBAVECPIB         ;
        STAB    AVECPIB           ; PIB

        LDD     MBBLENDERTYPE     ; BLENDER TYPE
        STAB    BLENDERTYPE       ;

        LDD     MBSHUTDOWNEN      ; BLENDER SHUTDOWN ENABLE 1 = ENABLED
        STAB    SHUTDOWNEN        ;

        LDD     MBSHUTDOWNTM      ; SHUT DOWN TIME MINS.
        JSR     HEXBCD1           ;
        STAB    SHUTDOWNTM      ;

        LDD     MBUNITTYPE
        STAB    UNITTYPE        ; UNITS TYPE

        LDD     MBEXSEN
        BEQ     LDMBSY30
        LDAB    #$AA
LDMBSY30:
        STAB    EXSENAB         ; EXTR. STALL CHECK ENABLE FLAG

        LDD     MBSSSTYPE
        STAB    SSSTYPE         ; SCREW SPEED SIGNAL TYPE (TACHO/COMMS)

        LDD     MBLSDELAY
        JSR     HEXBCD1         ; CONVERT TO BCD
        STAB    LSDELAY         ; DELAY TO LEVEL SENSOR ALARM.

        LDD     MBOFFLINETYPE
        JSR     HEXBCD1         ; CONVERT TO BCD
        STAB    OFFLINETYPE

        LDD     MBHILOREGENABLED; MODBUS TABLE CONTROL GAIN (MAX = 99 DEC)
        CLRA
        JSR     HEXBCD1         ; CONVERT BYTE IN ACC B
        STAB    HILOREGENABLED  ; CONTROL GAIN

; PURGE COMP NO

        LDD     MBPURGECOMPNO;
        CLRA
        JSR     HEXBCD1         ;
        STAB    PURGECOMPNO  ;

; CONTROL TYPE
        LDD     MBCONTROLTYPE;
        CLRA
        JSR     HEXBCD1         ;
        STAB    CONTROLTYPE     ;

; CONTROL LAG

        LDD     MBCONTROLLAG    ; .
        JSR     HEXBCD2
        STE     CONTROLLAG          ;

; INCREASE RATE

        LDD     MBINCREASERATE        ; .
        JSR     HEXBCD2
        STE     INCREASERATE          ;

; DECREASE RATE

        LDD     MBDECREASERATE        ; .
        JSR     HEXBCD2
        STE     DECREASERATE          ;

; DEADBAND

        LDD     MBDEADBAND            ; .
        JSR     HEXBCD2
        STE     DEADBAND              ;

; FINE INCREASE / DECREASE

        LDD     FINEINCDEC
        JSR     HEXBCD2
        STE     MBFINEINCDEC              ;

; CONTROL %

        LDD     MBCONTROLPER;
        CLRA
        JSR     HEXBCD1         ;
        STAB    CONTROLPER      ;


; MAX COMPONENT %

        LDD     MBMAXPCNT1
        JSR     HEXBCD2
        STE     MAXPCNT1              ;

        LDD     MBMAXPCNT2
        JSR     HEXBCD2
        STE     MAXPCNT2              ;

        LDD     MBMAXPCNT3
        JSR     HEXBCD2
        STE     MAXPCNT3              ;

        LDD     MBMAXPCNT4
        JSR     HEXBCD2
        STE     MAXPCNT4              ;

        LDD     MBMAXPCNT5
        JSR     HEXBCD2
        STE     MAXPCNT5              ;

        LDD     MBMAXPCNT6
        JSR     HEXBCD2
        STE     MAXPCNT6              ;

        LDD     MBMAXPCNT7
        JSR     HEXBCD2
        STE     MAXPCNT7              ;

        LDD     MBMAXPCNT8
        JSR     HEXBCD2
        STE     MAXPCNT8              ;

        LDD     MBMAXPCNT9
        JSR     HEXBCD2
        STE     MAXPCNT9              ;

        LDD     MBMAXPCNT10
        JSR     HEXBCD2
        STE     MAXPCNT10             ;

        LDD     MBMAXPCNT11
        JSR     HEXBCD2
        STE     MAXPCNT11             ;

        LDD     MBMAXPCNT12
        JSR     HEXBCD2
        STE     MAXPCNT12             ;

; MULTIPLE STAGE FILLING

        LDD     MBSTAGEFILLEN
        CLRA
        JSR     HEXBCD1         ;
        STAB    STAGEFILLEN     ;

; MOULDING

        LDD     MBMOULDING
        CLRA
        JSR     HEXBCD1         ;
        STAB    MOULDING        ;

; MONITOR FACTOR

        LDD     MBMONITORFACTOR
        CLRA
        JSR     HEXBCD1         ;
        STAB    MONITORFACTOR   ;

; TOPUP

        LDD     MBTOPUP
        CLRA
        JSR     HEXBCD1         ;
        STAB    TOPUP           ;

; BLENDER CLEAN ON TIME

        LDD     MBPH1ONTIME
        CLRA
        JSR     HEXBCD1         ;
        STAB    PH1ONTIME           ;

; BLENDER CLEAN OFF TIME

        LDD     MBPH1OFFTIME
        CLRA
        JSR     HEXBCD1         ;
        STAB    PH1OFFTIME           ;

; MIXER CLEAN ON TIME

        LDD     MBPH2ONTIME
        CLRA
        JSR     HEXBCD1         ;
        STAB    PH2ONTIME           ;

; MIXER CLEAN OFF TIME

        LDD     MBPH2OFFTIME
        CLRA
        JSR     HEXBCD1         ;
        STAB    PH2OFFTIME           ;

; BLENDER CLEAN REPS

        LDD     MBBCLEANREPETITIONS
        CLRA
        JSR     HEXBCD1         ;
        STAB    BCLEANREPETITIONS           ;

; MIXER CLEAN REPS

        LDD     MBMCLEANREPETITIONS
        CLRA
        JSR     HEXBCD1         ;
        STAB    MCLEANREPETITIONS           ;

; BYPASS TIME LIMIT

        LDD     MBBYPASSTIMELIMIT
        CLRA
        JSR     HEXBCD1         ;
        STAB    BYPASSTIMELIMIT

                   ;
; TOP UP % ALLOWED

        LDD     MBPERTOPUP
        CLRA
        JSR     HEXBCD1         ;
        STAB    PERTOPUP

; MAX GRAMS PER %

        LDD     MBMAXGRAMSPERPER
        CLRA
        JSR     HEXBCD1         ;
        STAB    MAXGRAMSPERPER

; BYPASS DELAY TIME DURING CLEANING

        LDD     MBBYPASSDCLNTIMELIMIT
        CLRA
        JSR     HEXBCD1         ;
        STAB    BYPASSDCLNTIMELIMIT

; TOP UP COMPONENT REFERENCE

        LDD     MBTOPUPCOMPNOREF
        CLRA
        JSR     HEXBCD1         ;
        STAB    TOPUPCOMPNOREF

; INTERMITTENT MIX TIME
;
        LDD     MBINTMIXTIME
        CLRA
        JSR     HEXBCD1         ;
        STAB    INTMIXTIME

; PASSWORDS

        LDD     PASWRD
        JSR     HEXBCD2
        STE     MBPASWRD             ;

        LDD     PASWRD2
        JSR     HEXBCD2
        STE     MBPASWRD2             ;


;MBOPTIONS

        LDAB    #RAMBANK
        TBYK                            ; SET UP YK

; HI LO REGRIND

        LDD     #HILOREGENABLEDBITPOS   ; .
        LDX     #HILOREGENABLED
        LDY     #MBOPTIONS
        JSR     SETCLRBITPOS

; MAX % CHECK

        LDD     #MAXPCNTCHECKBITPOS   ; .
        LDX     #MAXPCNTCHECK
        LDY     #MBOPTIONS
        JSR     SETCLRBITPOS

; 1ST COMPONENT COMPENTATION

        LDD     #FIRSTCOMPCOMPBITPOS   ; .
        LDX     #FIRSTCOMPCOMPENDIS
        LDY     #MBOPTIONS
        JSR     SETCLRBITPOS

; LAYERING
        LDD     #LAYERINGBITPOS
        LDX     #LAYERING
        LDY     #MBOPTIONS
        JSR     SETCLRBITPOS

; MIXING SCREW DETECTION

        LDD     #MIXSCREWDETBITPOS
        LDX     #MIXSCREWDETECTION
        LDY     #MBOPTIONS
        JSR     SETCLRBITPOS

; DOUBLE DUMP

        LDD     #DOUBLEDUMPBITPOS
        LDX     #DOUBLEDUMP
        LDY     #MBOPTIONS
        JSR     SETCLRBITPOS

; REGRIND RETRY

        LDD     #REGRINDRETRYBITPOS
        LDX     #REGRINDRETRY
        LDY     #MBOPTIONS
        JSR     SETCLRBITPOS

; LEVEL MONITORING

        LDD     #LEVELMONITORBITPOS
        LDX     #LEVELMONITOR
        LDY     #MBOPTIONS
        JSR     SETCLRBITPOS

; BYPASS

        LDD     #BYPASSMODEBITPOS
        LDX     #BYPASSMODE
        LDY     #MBOPTIONS
        JSR     SETCLRBITPOS

; MANUAL FILL SORTING

        LDD     #MANUALFILLSORTBITPOS
        LDX     #MANUALFILLSORT
        LDY     #MBOPTIONS
        JSR     SETCLRBITPOS

; TOPUP

        LDD     #TOPUPBITPOS
        LDX     #TOPUP
        LDY     #MBOPTIONS
        JSR     SETCLRBITPOS


; BLOW EVERY CYCLE

        LDD     #BLOWEVERYCYCLEBITPOS
        LDX     #BLOWEVERYCYCLE
        LDY     #MBOPTIONS
        JSR     SETCLRBITPOS

; CLEANING

        LDD     #CLEANINGBITPOS
        LDX     #CLEANING
        LDY     #MBOPTIONS
        JSR     SETCLRBITPOS

; MODBUS SINGLE WORD WRITE

        LDD     #SINGLEWORDWRITEBITPOS
        LDX     #SINGLEWORDWRITE
        LDY     #MBOPTIONS
        JSR     SETCLRBITPOS

; MODBUS SINGLE WORD WRITE


; DIAGNOSTICS PRINTING

        LDD     #DIAGPENABBPOS
        LDX     #DIAGPENAB
        LDY     #MBPRINTOPTIONS
        JSR     SETCLRBITPOS


; MONTHLY PRINTING

        LDD     #MTHPENABBITPOS
        LDX     #MTHPENAB
        LDY     #MBPRINTOPTIONS
        JSR     SETCLRBITPOS

; HOURLY PRINTING

        LDD     #HRPENABBITPOS
        LDX     #HRPENAB
        LDY     #MBPRINTOPTIONS
        JSR     SETCLRBITPOS

; SHIFT REPORT PRINTING

        LDD     #SHFPENABBITPOS
        LDX     #SHFPENAB
        LDY     #MBPRINTOPTIONS
        JSR     SETCLRBITPOS


; ALARM REPORT PRINTING

        LDD     #ALPENABBITPOS
        LDX     #ALPENAB
        LDY     #MBPRINTOPTIONS
        JSR     SETCLRBITPOS


; ROLL PRINTING

        LDD     #RRPENABBITPOS
        LDX     #RRPENAB
        LDY     #MBPRINTOPTIONS
        JSR     SETCLRBITPOS

; ORDER REPORT PRINTING


        LDD     #ORPENABBITPOS
        LDX     #ORPENAB
        LDY     #MBPRINTOPTIONS
        JSR     SETCLRBITPOS


; SERIAL / PARALLEL PRINTING


        LDD     #SERPTROPBITPOS
        LDX     #SERPTROP
        LDY     #MBPRINTOPTIONS
        JSR     SETCLRBITPOS

; SERIAL / PARALLEL PRINTING


        LDD     #SCPENABBITPOS
        LDX     #SCPENAB
        LDY     #MBPRINTOPTIONS
        JSR     SETCLRBITPOS

        LDAA    #$AA
        STAA    CEEFLG1          ; SET FLAG TO SAVE TO EEPROM
        RTS
RELOADMB:
        JSR     CPYSYCMB        ; COPY SYSTEM CONFIG DATA.
        RTS.

*/
/*
       Bool          m_bRollReportPrintFlag;            // Enabled/disabled.
       Bool          m_bOrderReportPrintFlag;           // Enabled/disabled.
       Bool          m_bShiftReportPrintFlag;           // Enabled/disabled.
       Bool          m_bHourlyPrintFlag;                // hourly printout Enabled/disabled.
       Bool          m_bMonthlyPrintFlag;               // monthly printout Enabled/disabled.
       Bool          m_bDiagPrintFlag;                  // printing of diagnostics Enabled/disabled.
       Bool          m_bAlarmPrintFlag;                 // printing of alarms Enabled/disabled.
 m_bScreenPrintFlag m_nPrinterType
// Printing Option Masks
#define BATCH_PRINT_DIAGNOSTICS_BIT       0x0001 //DIAGNOSTICS PRNTINGBIT
#define BATCH_PRINT_MONTHLY_BIT             0x0002 //MONTHLY PRINTING   BIT
#define BATCH_PRINT_HOURLY_BIT              0x0004 //HOURLY PRINTING    BIT
#define BATCH_PRINT_SHIFT_BIT             0x0008 //SHIFT PRINTING     BIT
#define BATCH_PRINT_ALARM_BIT               0x0010 //ALARM PRINTING     BIT
#define BATCH_PRINT_ORDER_BIT             0x0020 //ORDER PRINTING     BIT
#define BATCH_PRINT_ROLL_BIT            0x0040 //ROLL PRINTING      BIT
#define BATCH_PRINT_SER_PAR_BIT           0x0080 //SERIAL / PARALLEL  BIT
#define BATCH_PRINT_SCREEN_BIT              0x0100 //SCREEN PRINT       BIT




#define BATCH_CALIBRATION_CONFIG_OPTIONS            155     // CONFIGURATION OPTIONS.
// Option Masks
#define BATCH_CONFIG_HILO_REG_BIT                       0x0001 //HI-LO REGRIND BIT
#define BATCH_CONFIG_MAX_COMP_PER_EN_BIT                0x0002 //Max Component % enableD BIT
#define BATCH_CONFIG_FIRST_COMP_COMPENSATION_BIT    0x0004 //1st component compensation BIT
#define BATCH_CONFIG_LAYERING_BIT                       0x0008 //Layering BIT
#define BATCH_CONFIG_MIXER_DETECT_BIT                   0x0010 //MIXER DETECT BIT
#define BATCH_CONFIG_DOUBLE_DUMP_BIT                    0x0020 //DOUBLE_DUMP BIT
#define BATCH_CONFIG_REGRIND_RETRY_BIT                  0x0040 //REGRIND RETRY BIT
#define BATCH_CONFIG_LEV_SEN_MON_BIT                    0x0080 //Level sensor monitoring BIT

#define BATCH_CONFIG_BYPASS_EN_BIT                      0x0100 //bypass enable BIT
#define BATCH_CONFIG_FILL_MAN_SORT_BIT              0x0200 //Fill manual sort BIT
#define BATCH_CONFIG_TOPUP_EN_BIT                       0x0400 //Top Up BIT
#define BATCH_CONFIG_BLOW_EVERY_CYCLE_BIT               0x0800 //Blow every cycle in clean BIT
#define BATCH_CONFIG_CLEAN_EN_BIT                       0x1000 //Clean enable BIT
#define BATCH_CONFIG_SINGLE_WORD_WRITE_BIT          0x2000 //Single word write BIT



;* COPY SYSTEM CONFIG. PARAMETERS INTO MODBUS TABLE CALIBRATION DATA AREA
CPYSYCMB:
        PSHM    D
        LDAB    NOBLNDS
        CLRA
        STD     MBNOBDS         ; MODBUS TABLE, NO. OF BLENDS

        LDAB    FILLRETRY       ; FILL RETRY
        CLRA
        STD     MBFILLRETRY

        LDAB    CRLMODE         ; CONTROL MODE (EXTRUDER/PROP./HAULOFF)
        CLRA
        STD     MBCRLMOD

        LDD     LINENO          ; LINE NO. (COMMS PURPOSES)
        JSR     BCDHEX2
        STD     MBLINENO

        LDAB    BLENDERMODE        ; EXTRUSION PROCESS, BLOWN FILM ETC
        STD     MBBLENDERMODE

        LDAB    REGCOMP          ; MIN BLEND 1 %
        JSR     BCDHEX1
        CLRA
        STD     MBREGCOMP

        LDAB    FSTCTRL         ; FAST CONTROL RESPONSE.
        CLRA
        STD     MBFSTCTRL

        LDAB    DIAGMODE        ; DIAGNOSTICS.
        CLRA
        STD     MBDIAGMODE      ;

        LDAB    VACLOAD         ; VACUUM LOADING ON.
        CLRA
        STD     MBVACLOAD       ;

        LDAB    BYPASSE         ; BYPASS ENABLED..
        CLRA
        STD     MBBYPASSE       ;

        LDAB    ISVOLMODE       ; VOLUMETRIC MODE.
        CLRA
        STD     MBISVOLMODE     ;

        LDAB    EIOMITTED       ; SERIAL EI
        CLRA
        STD     MBEIOMITTED     ;

        LDAB    ISVOLMODE       ; VOLUMETRIC MODE.
        CLRA
        STD     MBISVOLMODE     ;

        LDAB    AVECPIB         ; PIB ASSOCIATED
        CLRD
        STD     MBAVECPIB

        LDAB    BLENDERTYPE     ; BLENDER TYPE
        CLRA
        STD     MBBLENDERTYPE

        LDAB    SHUTDOWNEN      ; SHUTDOWN
        CLRA
        STD     MBSHUTDOWNEN    ;

        LDAB    SHUTDOWNTM      ; SHUTDOWN TIME
        JSR     BCDHEX1
        CLRA
        STD     MBSHUTDOWNTM    ;

        LDAB    UNITTYPE        ; TYPE OF UNITS (METRIC-0/IMP-1)
        STD     MBUNITTYPE

        LDAB    SSSTYPE         ; SCREW SPEED SIGNAL TYPE
        CLRA
        STD     MBSSSTYPE

        LDAB    EXSENAB         ; EXTR. STALL CHECK ENABLE FLAG
        BEQ     CPYSYC20
        LDAB    #1              ; USE 1 IN MODBUS TO SHOW ENABLED
CPYSYC20:
        CLRA
        STD     MBEXSEN

        LDAB    LSDELAY
        JSR     BCDHEX1
        CLRA
        STD     MBLSDELAY       ; DELAY TO LEVEL SENSOR ALARM.

        LDAB    OFFLINETYPE
        JSR     BCDHEX1         ; TO HEX.
        CLRA
        STD     MBOFFLINETYPE

        LDAB    HILOREGENABLED       ; HI/LO REG OPTIONS
        JSR     BCDHEX1
        CLRA
        STD     MBHILOREGENABLED     ; TO MODBUS TABLE %.

; COMBINE IN MODBUS OPTIONS
        LDAB    PURGECOMPNO
        JSR     BCDHEX1
        CLRA
        STD     MBPURGECOMPNO         ; .

; CONTROL TYPE
        LDAB    CONTROLTYPE
        JSR     BCDHEX1
        CLRA
        STD     MBCONTROLTYPE         ; .

; CONTROL LAG
        LDD     CONTROLLAG         ; .
        JSR     BCDHEX2
        STD     MBCONTROLLAG

; INCREASE RATE FOR INC/ DEC CONTROL

        LDD     INCREASERATE        ; .
        JSR     BCDHEX2
        STD     MBINCREASERATE

; DECREASE RATE FOR INC/ DEC CONTROL

        LDD     DECREASERATE        ; .
        JSR     BCDHEX2
        STD     MBDECREASERATE

; DEADBAND FOR INC/DEC CONTROL

        LDD     DEADBAND          ; .
        JSR     BCDHEX2
        STD     MBDEADBAND

; FINE INC/DEC BAND

        LDD     FINEINCDEC          ; .
        JSR     BCDHEX2
        STD     MBFINEINCDEC

        LDD     MAXPCNT1          ; .
        JSR     BCDHEX2
        STD     MBMAXPCNT1

        LDD     MAXPCNT2          ; .
        JSR     BCDHEX2
        STD     MBMAXPCNT2

        LDD     MAXPCNT3          ; .
        JSR     BCDHEX2
        STD     MBMAXPCNT3

        LDD     MAXPCNT4          ; .
        JSR     BCDHEX2
        STD     MBMAXPCNT4

        LDD     MAXPCNT5          ; .
        JSR     BCDHEX2
        STD     MBMAXPCNT5

        LDD     MAXPCNT6          ; .
        JSR     BCDHEX2
        STD     MBMAXPCNT6

        LDD     MAXPCNT7          ; .
        JSR     BCDHEX2
        STD     MBMAXPCNT7

        LDD     MAXPCNT8          ; .
        JSR     BCDHEX2
        STD     MBMAXPCNT8

        LDD     MAXPCNT9          ; .
        JSR     BCDHEX2
        STD     MBMAXPCNT9

        LDD     MAXPCNT10         ; .
        JSR     BCDHEX2
        STD     MBMAXPCNT10

        LDD     MAXPCNT11         ; .
        JSR     BCDHEX2
        STD     MBMAXPCNT11

        LDD     MAXPCNT12         ; .
        JSR     BCDHEX2
        STD     MBMAXPCNT12

        LDAB    STAGEFILLEN
        JSR     BCDHEX1
        CLRA
        STD     MBSTAGEFILLEN         ; .

; MOULDING

        LDAB    MOULDING
        JSR     BCDHEX1
        CLRA
        STD     MBMOULDING         ; .

        LDAB    MONITORFACTOR
        JSR     BCDHEX1
        CLRA
        STD     MBMONITORFACTOR         ; .

        LDAB    TOPUP
        JSR     BCDHEX1
        CLRA
        STD     MBTOPUP         ; .

        LDAB    PH1ONTIME
        JSR     BCDHEX1
        CLRA
        STD     MBPH1ONTIME         ; .

        LDAB    PH1OFFTIME
        JSR     BCDHEX1
        CLRA
        STD     MBPH1OFFTIME        ; .

        LDAB    PH2ONTIME
        JSR     BCDHEX1
        CLRA
        STD     MBPH2ONTIME        ; .

        LDAB    PH2OFFTIME
        JSR     BCDHEX1
        CLRA
        STD     MBPH2OFFTIME        ; .


; BLENDER CLEAN REPS

        LDAB    BCLEANREPETITIONS
        JSR     BCDHEX1
        CLRA
        STD     MBBCLEANREPETITIONS        ; .


; MIXER CLEAN REPS

        LDAB    MCLEANREPETITIONS
        JSR     BCDHEX1
        CLRA
        STD     MBMCLEANREPETITIONS        ; .

; BYPASS TIME LIMIT

        LDAB    BYPASSTIMELIMIT
        JSR     BCDHEX1
        CLRA
        STD     MBBYPASSTIMELIMIT        ; .

                   ;
; TOP UP % ALLOWED

        LDAB    PERTOPUP
        JSR     BCDHEX1
        CLRA
        STD     MBPERTOPUP        ; .

; MAX GRAMS PER %

        LDAB    MAXGRAMSPERPER
        JSR     BCDHEX1
        CLRA
        STD     MBMAXGRAMSPERPER        ; .

; BYPASS DELAY TIME DURING CLEANING

        LDAB    BYPASSDCLNTIMELIMIT
        JSR     BCDHEX1
        CLRA
        STD     MBBYPASSDCLNTIMELIMIT        ; .

; TOP UP COMPONENT REFERENCE

        LDAB    TOPUPCOMPNOREF
        JSR     BCDHEX1
        CLRA
        STD     MBTOPUPCOMPNOREF        ; .

; INTERMITTENT MIX TIME
;
        LDAB    INTMIXTIME
        JSR     BCDHEX1
        CLRA
        STD     MBINTMIXTIME        ; .

; PASSWORD
;
        LDX     #PASWRD
        JSR     BCDHEX3X        ;
        STE     MBPASWRD          ;

        LDX     #PASWRD2
        JSR     BCDHEX3X        ;
        STE     MBPASWRD2          ;


; MBOPTIONS

        LDAB    #RAMBANK
        TBYK                            ; SET UP YK

; HI LO REGRIND

        LDD     #HILOREGENABLEDBITPOS   ; .
        LDX     #HILOREGENABLED
        LDY     #MBOPTIONS
        JSR     SETCLRMBBITPOS

; MAX % CHECK

        LDD     #MAXPCNTCHECKBITPOS   ; .
        LDX     #MAXPCNTCHECK
        LDY     #MBOPTIONS
        JSR     SETCLRMBBITPOS

; 1ST COMPONENT COMPENTATION

        LDD     #FIRSTCOMPCOMPBITPOS   ; .
        LDX     #FIRSTCOMPCOMPENDIS
        LDY     #MBOPTIONS
        JSR     SETCLRMBBITPOS

; LAYERING
        LDD     #LAYERINGBITPOS
        LDX     #LAYERING
        LDY     #MBOPTIONS
        JSR     SETCLRMBBITPOS

; MIXING SCREW DETECTION

        LDD     #MIXSCREWDETBITPOS
        LDX     #MIXSCREWDETECTION
        LDY     #MBOPTIONS
        JSR     SETCLRMBBITPOS

; DOUBLE DUMP

        LDD     #DOUBLEDUMPBITPOS
        LDX     #DOUBLEDUMP
        LDY     #MBOPTIONS
        JSR     SETCLRMBBITPOS

; REGRIND RETRY

        LDD     #REGRINDRETRYBITPOS
        LDX     #REGRINDRETRY
        LDY     #MBOPTIONS
        JSR     SETCLRMBBITPOS

; LEVEL MONITORING

        LDD     #LEVELMONITORBITPOS
        LDX     #LEVELMONITOR
        LDY     #MBOPTIONS
        JSR     SETCLRMBBITPOS

; BYPASS

        LDD     #BYPASSMODEBITPOS
        LDX     #BYPASSMODE
        LDY     #MBOPTIONS
        JSR     SETCLRMBBITPOS

; MANUAL FILL SORTING

        LDD     #MANUALFILLSORTBITPOS
        LDX     #MANUALFILLSORT
        LDY     #MBOPTIONS
        JSR     SETCLRMBBITPOS

; TOPUP

        LDD     #TOPUPBITPOS
        LDX     #TOPUP
        LDY     #MBOPTIONS
        JSR     SETCLRMBBITPOS


; BLOW EVERY CYCLE

        LDD     #BLOWEVERYCYCLEBITPOS
        LDX     #BLOWEVERYCYCLE
        LDY     #MBOPTIONS
        JSR     SETCLRMBBITPOS

; CLEANING

        LDD     #CLEANINGBITPOS
        LDX     #CLEANING
        LDY     #MBOPTIONS
        JSR     SETCLRMBBITPOS

; MODBUS SINGLE WORD WRITE

        LDD     #SINGLEWORDWRITEBITPOS
        LDX     #SINGLEWORDWRITE
        LDY     #MBOPTIONS
        JSR     SETCLRMBBITPOS

; DIAGNOSTICS PRINTING

        LDD     #DIAGPENABBPOS
        LDX     #DIAGPENAB
        LDY     #MBPRINTOPTIONS
        JSR     SETCLRMBBITPOS

; MONTHLY PRINTING

        LDD     #MTHPENABBITPOS
        LDX     #MTHPENAB
        LDY     #MBPRINTOPTIONS
        JSR     SETCLRMBBITPOS

; HOURLY PRINTING

        LDD     #HRPENABBITPOS
        LDX     #HRPENAB
        LDY     #MBPRINTOPTIONS
        JSR     SETCLRMBBITPOS

; SHIFT REPORT PRINTING

        LDD     #SHFPENABBITPOS
        LDX     #SHFPENAB
        LDY     #MBPRINTOPTIONS
        JSR     SETCLRMBBITPOS

; ALARM REPORT PRINTING

        LDD     #ALPENABBITPOS
        LDX     #ALPENAB
        LDY     #MBPRINTOPTIONS
        JSR     SETCLRMBBITPOS

; ROLL PRINTING

        LDD     #RRPENABBITPOS
        LDX     #RRPENAB
        LDY     #MBPRINTOPTIONS
        JSR     SETCLRMBBITPOS

; ORDER REPORT PRINTING

        LDD     #ORPENABBITPOS
        LDX     #ORPENAB
        LDY     #MBPRINTOPTIONS
        JSR     SETCLRMBBITPOS


; SERIAL / PARALLEL PRINTING

        LDD     #SERPTROPBITPOS
        LDX     #SERPTROP
        LDY     #MBPRINTOPTIONS
        JSR     SETCLRMBBITPOS


; SCREEN PRINT

        LDD     #SCPENABBITPOS
        LDX     #SCPENAB
        LDY     #MBPRINTOPTIONS
        JSR     SETCLRMBBITPOS

        PULM    D
        RTS



SETCLRMBBITPOS:
        TST     0,X                     ; CHECK OPTION
        BEQ     CLRBIT                  ; OPTION CLEARED
        ORD     0,Y
        STD     0,Y
        BRA     XITSC                   ; EXIT
CLRBIT  COMD
        ANDD    0,Y
        STD     0,Y
XITSC   RTS


*/
/*
;* COPY TIME INTO MODBUS TABLE
CPYTIMMB:
        TST     PROTOCOL
        BPL     CPYTIMBX        ; EXIT IF NOT MB PROTOCOL

        LDD     MBWORFLG        ; CHECK IF TIME IS BEING WRITTEN
        ANDD    #$0400
        BNE     CPYTIMBX        ; NO UPDATE UNTIL TIME IS UPDATED.

        LDX     #TIMEBUF
        LDAB    MIN,X
        JSR     BCDHEX1
        PSHB
        LDAB    SEC,X
        JSR     BCDHEX1         ; CONVERT SECs TO HEX
        PULA                    ; RESTORE MINUTES
        STD     MBMINSEC        ; [MIN:SEC]

        LDX     #TIMEBUF
        LDAB    HOUR,X          ; HOURs
        JSR     BCDHEX1
        LDAA    DAY,X           ; WEEKDAY
        STD     MBDAYHOUR       ; [DAY:HOUR]

        LDX     #TIMEBUF
        LDAB    MON,X           ; MONTH
        JSR     BCDHEX1
        PSHB
        LDAB    DATE,X          ; DATE (DAY OF MONTH)
        JSR     BCDHEX1
        PULA                    ; RESTORE DATE IN acc A
        STD     MBMONDATE       ; [MONTH:DATE]

        LDX     #TIMEBUF
        LDAB    YEAR,X          ; YEAR
        LDAA    YEAR_MSB        ; CENTURY - RAM ONLY
        JSR     BCDHEX2         ; CONVERT YEAR e.g. 1996
        STD     MBYEAR
CPYTIMBX:
        RTS


;* LDMBTPTS    - LOAD TARGET THROUGHPUT DATA FROM MODBUS TABLE
LDMBTPTS:

        LDX     #MBWTPMSP+1
        JSR     HEXBCD3X        ; RESULT IN B:E

        CLR     WTPMSP          ; RESET GRAMS PER METER.
        STAB    WTPMSP+1
        STE     WTPMSP+2        ; BCD, 2DP


;        LDD     MBWTPMSP        ; NOTE: MAX VALUE EXPECTED IS 655.35g/m
;        JSR     HEXBCD2         ; CONVERT WT/m TO BCD
;        STAB    WTPMSP
;        STE     WTPMSP+1        ; BCD, 2DP

        LDD     MBTKGHSP        ; NOTE: MAX EXPECTED VALUE IS 6553.5 Kg/H
        JSR     HEXBCD2
        JSR     SHF_BE4L         ; SHIFT 4 PLACES LEFT, (*10), PROD. HAS 2DP
        STAB    TKGHSP
        STE     TKGHSP+1        ; Kg/H SETPOINT (2DP)

        LDD     MBGPM2SPT       ; NOTE: MAX VALUE EXPECTED IS 655.35g/m
        JSR     HEXBCD2
        STAB    GPM2SPT        ; g/sq. m SETPOINT
        STE     GPM2SPT+1

        LDD     MBLSPDSP        ; LINE SPD SPT NOTE: MBUS VERSION HAS 1 DP
        JSR     HEXBCD2
        JSR     SHF_BE4R        ; PROD. LSPD. HAS NO DP, I.E. DIV. BY 10
        STE     LSPDSP          ; LINE SPEED SETPOINT (m/Min) INTEGER ONLY

        LDD     MBWIDTHSP       ; MODBUS  WIDTH SPT mm
        JSR     HEXBCD2
        STE     WIDTHSP         ; MAX EXPECTED IS 9999mm

        LDD     MBTRIMWID       ; MODBUS TRIMMED WIDTH (mm)
        JSR     HEXBCD2
        STE     TRIMWID         ; MAX EXPECTED IS 9999mm

        JSR     SPCHANG         ; INITIATE CONTROL ACTION IF IN AUTO.
        JSR     CONVERTBATSETALW
        RTS


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

;       PROCEND

;       END


COPYMBDIAGNOSTICS:

        LDAB    PUPMIN
        JSR     BCDHEX1                 ; TO HEX
        STAB    MBPOWERUPTIME           ; POWER UP TIME.

        LDAB    PUPHRS
        JSR     BCDHEX1                 ; TO HEX
        STAB    MBPOWERUPTIME+1        ; POWER UP TIME.

        LDAB    PUPDOM
        JSR     BCDHEX1                 ; TO HEX
        STAB    MBPOWERUPTIME+2        ; POWER UP TIME.


        LDAB    PUPMON
        JSR     BCDHEX1                 ; TO HEX
        STAB    MBPOWERUPTIME+3        ; POWER UP TIME.


        LDD     CALMODSERNO+1           ; READ CAL NO.
        JSR     BCDHEX2                 ; TO HEX
        STD     MBCALMODSERNO           ; STORE.


        LDX     #SBBSERNO
        JSR     BCDHEX3X
        STAB    MBSBBSERNO+1
        STE     MBSBBSERNO+2

        LDX     #GOODTXCTR1
        JSR     BCDHEX3X                 ; CONVERT TO HEX.
        CLRA
        STD     MBGOODTXCTR1
        STE     MBGOODTXCTR1+2             ; COMMS COUNTER 1 REMOTE DISPLAY.


        LDX     #GOODTXCTR2
        JSR     BCDHEX3X                 ; CONVERT TO HEX.
        CLRA
        STD     MBGOODTXCTR2           ; COMMS COUNTER 2 LLS
        STE     MBGOODTXCTR2+2

        LDX     #GOODTXCTR3
        JSR     BCDHEX3X                 ; CONVERT TO HEX.
        CLRA
        STD     MBGOODTXCTR3
        STE     MBGOODTXCTR3+2

        LDX     #GOODRXCTR1
        JSR     BCDHEX3X                 ; CONVERT TO HEX.
        CLRA
        STD     MBGOODRXCTR1
        STE     MBGOODRXCTR1+2            ; COMMS COUNTER 1 REMOTE DISPLAY.

        LDX     #GOODRXCTR2
        JSR     BCDHEX3X                 ; CONVERT TO HEX.
        CLRA
        STD     MBGOODRXCTR2
        STE     MBGOODRXCTR2+2            ; COMMS COUNTER 2 LLS

        LDX     #GOODRXCTR3
        JSR     BCDHEX3X                 ; CONVERT TO HEX.
        CLRA
        STD     MBGOODRXCTR3             ; COMMS COUNTER 3  LLS
        STE     MBGOODRXCTR3+2           ; COMMS COUNTER 3  LLS

        LDX     #CHECKSUMERR1
        JSR     BCDHEX3X                 ; CONVERT TO HEX.
        CLRA
        STD     MBCHECKSUMERR1            ; COMMS CHECKSUM ERROR COUNTER.
        STE     MBCHECKSUMERR1+2            ; COMMS CHECKSUM ERROR COUNTER.

        LDX     #CHECKSUMERR2
        JSR     BCDHEX3X                 ; CONVERT TO HEX.
        CLRA
        STD     MBCHECKSUMERR2           ; COMMS CHECKSUM ERROR COUNTER
        STE     MBCHECKSUMERR2+2            ; COMMS CHECKSUM ERROR COUNTER

        LDX     #CHECKSUMERR3
        JSR     BCDHEX3X                 ; CONVERT TO HEX.
        CLRA
        STD     MBCHECKSUMERR3              ; COMMS CHECKSUM ERROR COUNTER
        STE     MBCHECKSUMERR3+2            ; COMMS CHECKSUM ERROR COUNTER

        LDX     #TIMEOUTCTR1
        JSR     BCDHEX3X                 ; CONVERT TO HEX.
        CLRA
        STD     MBTIMEOUTCTR1              ; COMMS TIMEOUT COUNTER
        STE     MBTIMEOUTCTR1+2            ; COMMS TIMEOUT COUNTER

        LDX     #TIMEOUTCTR2
        JSR     BCDHEX3X                 ; CONVERT TO HEX.
        CLRA
        STD     MBTIMEOUTCTR2              ; COMMS TIMEOUT COUNTER
        STE     MBTIMEOUTCTR2+2            ; COMMS TIMEOUT COUNTER

        LDX     #TIMEOUTCTR3
        JSR     BCDHEX3X                 ; CONVERT TO HEX.
        CLRA
        STD     MBTIMEOUTCTR3              ; COMMS TIMEOUT COUNTER
        STE     MBTIMEOUTCTR3+2            ; COMMS TIMEOUT COUNTER

        LDD     CYCLECOUNTER
        STD     MBCYCLECOUNTER

        RTS


*/


/*


MCE12-15



;*****************************
;                            *
;   START OF THE CODE.       *
;                            *
;*****************************
;
;
;       PSCT
;
;        PROC
BANK2PROGS:        SECTION


; NOTE THAT THIS PROGRAM USES LOOKUP TABLES AND HAS SET YK TO BANK 4

MBSYSCFGNO      EQU     23              ; NO OF SYSTEM CONFIGURATION PARAMETERS.

       PUBLIC   COPYSPMB,COPYKSMB,IMBTBFLGS,INITMBTB
       PUBLIC   WRTEBX,WRTEDX,CLREWX,C2ASCII
       PUBLIC   LDMBOINFO,LDMBTPTS,LDMBPCTS
       PUBLIC   LDMBWCS
       PUBLIC   POSTMBWRT,CHMBVDATA,COPYCDMB
       PUBLIC   CPYLSCMB,CPYMCPMB,CPYHPCMB
       PUBLIC   CPYDACMB,CPYKGHMB,CPYGPMMB,CPYGM2MB,CPYLSPMB
       PUBLIC   CPYERPMMB,CPYPSAMB,CPYAMFMB
       PUBLIC   CPYFRLMB,CPYBRLMB,CPYOLMB,CPYTWTMB
       PUBLIC   CPYLPPMB,CPYAPCTMB,CPYHWTSMB,CPYTIMMB,LDMBTIME
       PUBLIC   CPYWTCMB,COPYHOPPERWEIGHTMB,CHECKFORPENDINGRECIPE



; WRTEBX - WRITE REG. E Acc B times @ X
WRTEBX  TSTB
        BEQ     WREBXQ          ; QUIT IF B = 0
WREBX2  STE     0,X
        AIX     #2
        DECB
        BNE     WREBX2
WREBXQ  RTS

; WRTEDX - WRITE REG. E Reg D times at X
WRTEDX  TSTD
        BEQ     WREDXQ          ; QUIT IF D = 0
WREDX2  STE     0,X
        AIX     #2
        SUBD    #1
        BNE     WREDX2
WREDXQ  RTS

; CLREWX - CLEAR E WORDS AT X
CLREWX TSTE
        BEQ     CLEWXQ
CLEWX2  CLRW    0,X
        AIX     #2
        SUBE    #1
        BNE     CLEWX2
CLEWXQ  RTS

;  C2ASCII CONVERTS 2 DIGITS IN ACCA TO 2 ASCII IN REG D
C2ASCII:
       PSHA
       ANDA   #$0F
       ORAA   #$30                     ;~ASCII &
       TAB                      ; SAVE L.S. DIGIT ASCII IN ACC B
       PULA
       LSRA
       LSRA                     ;~SHIFT M.S DIGIT TO BITS 0-3
       LSRA
       LSRA
       ORAA   #$30              ;~M.S. DIGIT ASCII NOW IN ACC A

       RTS

; * ASCII2D CONVERTS 2 ASCII CHARS IN REG D TO 2 DIGITS IN ACC A
;               NOTE: ASSUMES NUMERICAL DATA.
ASCII2D:
        ANDA    #$0F            ; MASK OFF M.S.NIBBLE
        ANDB    #$0F            ; "     "       "
        LSLA                    ; SHIFT A 4 BITS LEFT.
        LSLA
        LSLA
        LSLA
        ABA                     ; ASSEMBLE RESULT IN ACC A
        RTS
;
;*  YCS_TOX  COPIES A STRING OF DATA (0,Y) TO _X.
;*  LAST CHAR IS INDICATED BY "00"
;*  IT IS ASSUMED THAT THE STRING IS IN THE TEXTBANK.
YCS_TOX PSHM    K
        LDAB    #TEXTBANK
        TBYK            ;ENSURE Y POINTS AT TEXT BANK
YCS__X2 LDAA   0,Y
        BEQ    YCS__XX
        STAA    0,X     ; MOVE CHAR TO X
        AIX     #1
        AIY      #1      ;;INX
        BRA    YCS__X2
YCS__XX PULM    K       ;RESTORE K TO VALUE ON ENTRY
        RTS

; INITMBTB - INITIALISE THE MODBUS TABLE
INITMBTB LDE    #MBTABSIZ         ; OVERALL SIZE OF TABLE (BYTES)
        ASRE                      ; DIV. BY 2 TO GET WORDS
        LDX     #MBTABLE
        JSR     CLREWX
        JSR     COPYSPMB
        JSR     COPYKSMB
        JSR     COPYCDMB
        JSR     COPYVNMB        ; COPY VERS. NO. -> MB TABLE
        JSR     IMBTBFLGS       ; INITIALISE TABLE FLAGS
        CLR     MBWSSFLAG       ; FLAG TO SHOW DOWNLOAD OF SCREW SPEED VIA MB

        RTS
; COPYSPMB - ROUTINE TO COPY ALL SETPOINT DATA TO MODBUS TABLE
COPYSPMB:
        TST     PROTOCOL
        BPL     CPYSPMBX        ; EXIT IF NOT MB PROTOCOL
        JSR     CPYPCTSMB       ; COPY TARGET %'S INTO MB TABLE
        JSR     CPYTPTSMB       ; COPY THRUPUT DATA "   "
        JSR     CPYOINFMB       ; COPY ORDER INFO "     "
        JSR     CPYREGINFMB     ; COPY REGRIND INFORMATION
        LDD     MBNEWDATA
        ORD     #$0001          ; SET B0 TO INDICATE NEW SETPT DATA
        STD     MBNEWDATA
CPYSPMBX:
        RTS

;  CPYREGINFMB

CPYREGINFMB:
        TST     REG_PRESENT
        BEQ     NOR             ; NO REG
        CLRA
        LDAB    REGCOMP
        STD     MBREGCOMPNOSP
        BRA     XITR            ; EXIT
NOR     CLRW    MBREGCOMPNOSP
XITR    RTS

COPYMBPSUM:
        JSR     CPYDACMB                ; DAC DATA.
        JSR     COPYHOPPERWEIGHTMB      ; WEIGHT INFO.
        JSR     COPYBBAUXMB             ; RESET OF PSUM DATA.
        JSR     COPYMBDIAGNOSTICS
        JSR     CPYHOPPERSETWTMB
        JSR     CPYGM2MB
        RTS


;
; COPYVNMB - COPY VERSION NO (SOFTWARE REVISION) TO MODBUS TABLE
COPYVNMB:
        LDX     #MBVERNUM       ; VERNUM IN MODBUS TABLE
        LDY     #VERNUM
        JSR     YCS_TOX         ; COPY ASCII STRING FROM Y -> X
        RTS
; COPYCLMB - COPY ALL CALIB. DATA TO MODBUS TABLE
COPYCDMB:
        TST     PROTOCOL
        BPL     CPYCDMBX        ; EXIT IF NOT MB PROTOCOL

        JSR     CPYWTCMB        ; WT. CONTSANTS
        JSR     CPYHPCMB        ; HOPPER CONSTANTS
        JSR     CPYMCPMB        ; MEAS & CONTROL PARAMETERS
        JSR     CPYLSCMB        ; LINE & SCREW SPEED CONSTANTS
        JSR     CPYSYCMB        ; SYSTEM CONFIGURATION INFO
        JSR     CPYLIQCMB       ; LIQUID CONSTANTS
        JSR     CPYCCFGTMB      ; COMPONENT CONFIGURATION.
        LDD     MBNEWDATA
        ORD     #$0002          ; SET B1 TO INDICATE NEW CALIB DATA
        STD     MBNEWDATA
CPYCDMBX:
        RTS




PERLOOKUP FDB   MBPCNT1
          FDB   MBPCNT2
          FDB   MBPCNT3
          FDB   MBPCNT4
          FDB   MBPCNT5
          FDB   MBPCNT6
          FDB   MBPCNT7
          FDB   MBPCNT8
          FDB   MBPCNT9
          FDB   MBPCNT10
          FDB   MBPCNT11
          FDB   MBPCNT12



;* COPY TARGET %'s INTO MODBUS TABLE SETPOINT AREA
CPYPCTSMB:
        LDAB   #RAMBANK
        TBZK                    ;~;SET EK FOR BANK 1, I.E. RAM
        LDAB   #BANK4
        TBYK                    ;~;SET EK FOR BANK 1, I.E. RAM

        LDAB    NOBLNDS
        LDX     #PCNT1          ; PRODUCTION SETPOINTS, % #1
;        LDZ     #MBPCNT1        ; MODBUS TABLE, % #1
         LDY     #PERLOOKUP
CPYPTS10 PSHB
         LDD     0,X
         JSR     BCDHEX2         ; CONVERT TO HEX
         CPD     #9999           ; NOTE 9999 IS USED TO STORE 100.00%
         BNE     CPYPTS12
         LDD     #10000          ; 100% (NOTE 2DP)
CPYPTS12 LDZ    0,Y
        STD     0,Z
        AIX     #2
        AIY     #2
        PULB
        DECB
        BNE     CPYPTS10

         LDAB    #MAXCOMPONENTNO ; PLACE FOR 8 BLENDS IN MB TABLE
         SUBB    NOBLNDS         ; GET NO. OF UNUSED BLENDS
         BEQ     CPYPTS20
CPYPTS15 LDZ    0,Y
         CLRW   0,Z             ; SET UNUSED BLEND %'s TO 0000
         AIY     #2
         DECB
         BNE     CPYPTS15

CPYPTS20:
;        CLRW   MBPCNTLIQ       ; LIQ. %'s
;        CLRW    MBPCNTLQ2       ; AT PRESENT THERE IS NO LIQ. #2(15/7/96)
        TST     AVECPIB         ; IS THERE PIB?
        BEQ     CPYPTS30
        LDD     PCNTLIQ
        JSR     BCDHEX2
;        STD     MBPCNTLIQ
CPYPTS30:
;        LDD     PCT_ADD         ; % DRY ADDITIVE
;        JSR     BCDHEX2
;        STD     MBPCT_ADD

        LDD     HIREGPCNT         ; HIGH REG %
        JSR     BCDHEX2
        STD     MBHIREGPCNT


        RTS




;* COPY THRU-PUT DATA INTO MODBUS SETPOINTS AREA OF TABLE
CPYTPTSMB:
        LDX     #WTPMSP+1
        JSR     BCDHEX3X        ; CONVERT 3 BYTES BCD @ X TO HEX
        CLRA
        STD     MBWTPMSP        ;
        STE     MBWTPMSP+2      ;



        LDAB    TKGHSP
        LDE     TKGHSP+1        ; Kg/H SETPOINT (2DP)
        LDAA    BLENDERMODE
        CMPA    #GPMGPMENXXKGS
        BEQ     NOSH            ; NO SHIFT OF DATA
        CMPA    #OFFLINEMODE    ; OFFLINE MODE
        BNE     NRMDNL          ; NORMAL DOWNLOAD

        LDAA    OFFLINETYPE             ; CHECK TYPE.
        CMPA    #OFFLINETYPEXXKG        ; DISPENSE XX KGS.
        BEQ     NOSH

NRMDNL  JSR     SHF_BE4R        ; SHIFT[B:E] 4 PLACES TO RIGHT ( ONLY HANDLE 1DP)
NOSH    JSR     BCDHEX3         ; CONVERT TO HEX
        STE     MBTKGHSP        ; NOTE: MAX EXPECTED VALUE IS 6553.5 Kg/H


        LDX     #GPM2SPT        ; g/sq. m SETPOINT
        JSR     BCDHEX3X        ; CONVERT 3 BYTES BCD @ X TO HEX
        STE     MBGPM2SPT       ; NOTE: MAX VALUE EXPECTED IS 655.35g/m

        CLRB
        LDE     LSPDSP          ; LINE SPEED SETPOINT (m/Min) INTEGER ONLY
        JSR     SHF_BE4L        ; MBUS VERSION HAS 1 DP
        JSR     BCDHEX3
        STE     MBLSPDSP        ; NOTE: MAX EXPECTED VALUE IS 6553.5 Kg

        LDD     WIDTHSP
        JSR     BCDHEX2         ; WIDTH SPT mm
        STD     MBWIDTHSP

        LDD     TRIMWID         ; TRIMMED WIDTH (mm)
        JSR     BCDHEX2
        STD     MBTRIMWID

        LDD     DBXXKGS         ; DEADBAND USED IN XX KGS MODE
        STD     EREG
        CLR     EREG+2
        LDX     #EREG
        JSR     BCDHEX3X
        STE     MBDBXXKGS


        RTS

;* COPY ORDER INFO INTO MODBUS TABLE SETPOINT
CPYOINFMB:
        LDX     #FROLLSP        ; FRONT ROLL LENGTH SPT (m)
        JSR     BCDHEX3X        ; 3 BCD BYTES @ X
        STE     MBFROLLSP       ; MAX LENGTH EXPECTED = 65,536m

        LDX     #BROLLSP        ; BACK ROLL LENGTH SPT (m)
        JSR     BCDHEX3X        ; 3 BCD BYTES @ X
        STE     MBBROLLSP       ; MAX LENGTH EXPECTED = 65,536m

        LDX     #ORDLSP         ; ORDER LENGTH SPT (m)
        JSR     BCDHEX3X        ; 3 BCD BYTES @ X (999,999m)
        CLR     MBORDLSP+0
        STAB    MBORDLSP+1
        STE     MBORDLSP+2      ; MAX LENGTH ALLOWABLE = 16 MILLION m

        LDX     #ORDWSP         ; ORDER WEIGHT SPT (Kg)
        JSR     BCDHEX3X        ; 3 BCD BYTES @ X (999,999Kg)
        CLR     MBORDWSP+0
        STAB    MBORDWSP+1
        STE     MBORDWSP+2      ; MAX WEIGHT ALLOWABLE = 16 MILLION Kg

        LDX     #CUSCODE
        LDAA    0,X
        JSR     C2ASCII         ; CONVERT 2 DIGITS TO 2 ASCII CHAR.S
        STD     MBCUSCODE
        LDAA    1,X
        JSR     C2ASCII         ; CONVERT 2 DIGITS TO 2 ASCII CHAR.S
        STD     MBCUSCODE+2
        LDAA    2,X
        JSR     C2ASCII         ; CONVERT 2 DIGITS TO 2 ASCII CHAR.S
        STD     MBCUSCODE+4

;        LDX     #MATCODE        ; MATERIAL CODE
;        LDAA    0,X
;        JSR     C2ASCII         ; CONVERT 2 DIGITS TO 2 ASCII CHAR.S
;        STD     MBMATCODE
;        LDAA    1,X
;        JSR     C2ASCII         ; CONVERT 2 DIGITS TO 2 ASCII CHAR.S
;        STD     MBMATCODE+2
;        LDAA    2,X
;        JSR     C2ASCII         ; CONVERT 2 DIGITS TO 2 ASCII CHAR.S
;        STD     MBMATCODE+4

        LDX     #ORDERNO        ; MATERIAL CODE
        LDAA    0,X
        JSR     C2ASCII         ; CONVERT 2 DIGITS TO 2 ASCII CHAR.S
        STD     MBORDERNO
        LDAA    1,X
        JSR     C2ASCII         ; CONVERT 2 DIGITS TO 2 ASCII CHAR.S
        STD     MBORDERNO+2
        LDAA    2,X
        JSR     C2ASCII         ; CONVERT 2 DIGITS TO 2 ASCII CHAR.S
        STD     MBORDERNO+4

        LDE     OPERATORNO      ;
        STE     MBOPERATORNO    ; READ OPERATOR NO
        LDE     IORDERNO
        STE     MBIORDERNO



        LDAB    SHTTUBE
        STAB    MBSHTTUBE+1     ; MODBUS SHEET(1)/TUBE(0) FLAG
        CLR     MBSHTTUBE+0     ; NOTE: MODBUS PARAMETERS ARE ALL 2 BYTES MIN

        LDAB    FSLITRS         ; FRONT SLIT REELS
        JSR     BCDHEX1         ; MAX 99
        STAB    MBFSLITRS+1
        CLR     MBFSLITRS+0

        LDAB    BSLITRS         ; BACK SLIT REELS
        JSR     BCDHEX1         ; MAX 99
        STAB    MBBSLITRS+1
        CLR     MBBSLITRS+0

        LDAB    TRIMFED
        STAB    MBORRTFED+1     ; TRIM FED BACK FLAG
        CLR     MBORRTFED+0

;        CLRW    MBSPTSPAR
;        CLRW    MBSPTSPAR+2     ; SET SPARE DATA = 0
;        CLRW    MBSPTSPAR+4

        RTS

;COPYHOPPERWEIGHTMB - COPY HOPPER WEIGHT

COPYHOPPERWEIGHTMB:
        JSR     CALHWT          ;  CALCULATE HOPPER WEIGHT
        LDD     EREG
        STD     HOPPERWEIGHT    ; STORE TO HOPPER WEIGHT.
        LDD     EREG+2
        STD     HOPPERWEIGHT+2  ; DITHO.
        LDX     #EREG
        JSR     BCDHEX3X         ; HOPPER WEIGHT -> HEX.
        CLRA                     ; RESET MSB
        STD     MBHOPPERWEIGHT   ; STORE TO HOPPER WEIGHT.
        STE     MBHOPPERWEIGHT+2 ; DITHO.
        RTS




; COPYKSMB - COPY K's (SCREW FEEDER CALIBRATION) TO MODBUS TABLE
COPYKSMB
        RTS

;---REVIEW--- COPY OF DENSITIES TO BE INVESTIGATED


;        LDAB   #RAMBANK
;        TBZK                    ;~;SET EK FOR BANK 1, I.E. RAM
;        LDX     #CH1K1
;        LDZ     #MBDENSITY1     ; MODBUS TABLE, K FOR #1
;        LDAB    NOBLNDS         ; NO. OF BLENDS/COMPONENTS ON MACHINE
;;
;CPYKMB10 PSHB
;        JSR     BCDHEX3X         ; CONVERT 3 BCD BYTES @ X TO HEX
;        STE     0,Z             ; MAX VALUE ANTICIPATED = 655.35 Kg/H
;        AIX     #3
;        AIZ     #2
;        PULB
;        DECB
;        BNE     CPYKMB10
;
;        LDAB    #MAXCOMPONENTNO ; PLACE FOR 8 BLENDS IN MB TABLE
;        SUBB    NOBLNDS         ; GET NO. OF UNUSED BLENDS
;        BEQ     CPYKMB20
;CPYKMB15 CLRW   0,Z             ; SET UNUSED BLEND %'s TO 0000
;        AIZ     #2
;        DECB
;        BNE     CPYKMB15
;CPYKMB20:
;        LDX     #LA_K
;        JSR     BCDHEX3X
;        STE     MBCH9_K
;        CLRW    MBCHA_K         ; LIQUID #2, SET TO ZERO
;        CLRW    MBCHX_K         ; SPARE K VALUE
;        LDD     MBNEWDATA
;        ORD     #$0004          ; SET B2 TO INDICATE NEW K's
;        STD     MBNEWDATA
;        RTS

;* LDMBOINFO    - LOAD ORDER INFO FROM MODBUS TABLE
LDMBOINFO:
        LDD     MBFROLLSP        ; FRONT ROLL LENGTH SPT (m)
        JSR     HEXBCD2         ; 2 HEX BYTES
        STAB    FROLLSP
        STE     FROLLSP+1       ; MAX LENGTH EXPECTED = 65,536m

        LDD     MBBROLLSP       ; BACK ROLL LENGTH SPT (m)
        JSR     HEXBCD2         ; 2 HEX BYTES
        STAB    BROLLSP
        STE     BROLLSP+1       ; MAX LENGTH EXPECTED = 65,536m

        LDAB    MBORDLSP+1
        LDE     MBORDLSP+2      ; MAX LENGTH ALLOWABLE = 16 MILLION m
        JSR     HEXBCD3
        STAB    ORDLSP          ; ORDER LENGTH SPT (m)
        STE     ORDLSP+1        ; 3 BCD BYTES  (999,999m)

        LDAB    MBORDWSP+1
        LDE     MBORDWSP+2      ; MAX WEIGHT ALLOWABLE = 16 MILLION Kg
        JSR     HEXBCD3
        STAB    ORDWSP          ; ORDER WEIGHT SPT (Kg)
        STE     ORDWSP+1        ; 3 BCD BYTES  (999,999Kg)

        LDD     MBCUSCODE       ; MODBUS VERSION
        JSR     ASCII2D         ; CONVERT TO 2 DIGITS IN ACC A
        STAA    CUSCODE
        LDD     MBCUSCODE+2     ; NEXT 2 ASCII CHARS
        JSR     ASCII2D         ; CONVERT TO 2 DIGITS
        STAA    CUSCODE+1
        LDD     MBCUSCODE+4     ; L.S. 2 ASCII CHARS
        JSR     ASCII2D         ; CONVERT TO 2 DIGITS
        STAA    CUSCODE+2

;        LDD     MBMATCODE       ; MODBUS VERSION
;        JSR     ASCII2D         ; CONVERT TO 2 DIGITS IN ACC A
;        STAA    MATCODE
;        LDD     MBMATCODE+2     ; NEXT 2 ASCII CHARS
;        JSR     ASCII2D         ; CONVERT TO 2 DIGITS
;        STAA    MATCODE+1
;        LDD     MBMATCODE+4     ; L.S. 2 ASCII CHARS
;        JSR     ASCII2D         ; CONVERT TO 2 DIGITS
;        STAA    MATCODE+2


        LDD     MBORDERNO       ; MODBUS VERSION
        JSR     ASCII2D         ; CONVERT TO 2 DIGITS IN ACC A
        STAA    ORDERNO
        LDD     MBORDERNO+2     ; NEXT 2 ASCII CHARS
        JSR     ASCII2D         ; CONVERT TO 2 DIGITS
        STAA    ORDERNO+1
        LDD     MBORDERNO+4     ; L.S. 2 ASCII CHARS
        JSR     ASCII2D         ; CONVERT TO 2 DIGITS
        STAA    ORDERNO+2

        LDE     MBIORDERNO      ; READ ORDER NO
        STE     IORDERNO

        LDE     MBOPERATORNO    ; READ OPERATOR NO
        STE     OPERATORNO      ;

        LDE     MBRECIPENO
        STE     RECIPENO        ; STORE RECIPE NO

;        LDE     MBEXTRUDERNO
;        STE     EXTRUDERNO      ; STORE RECIPE NO

        LDAB    MBSHTTUBE+1     ; MODBUS SHEET(1)/TUBE(0) FLAG
        STAB    SHTTUBE

        LDAB    MBFSLITRS+1
        JSR     HEXBCD1         ; MAX OF 99 EXPECTED
        STAB    FSLITRS         ; FRONT SLIT REELS IN PROD. SPTS

        LDAB    MBBSLITRS+1
        JSR     HEXBCD1         ; MAX OF 99 EXPECTED
        STAB    BSLITRS         ; BACK SLIT REELS IN PROD. SPTS

        LDAB    MBTRIMFED+1     ; TRIM FED BACK FLAG
        STAB    TRIMFED
        RTS

;* LDMBPCTS    - LOAD TARGET %'S FROM MODBUS TABLE

;* LDMBPCTS    - LOAD TARGET %'S FROM MODBUS TABLE
LDMBPCTS:
;*TEST ONLY*
;        INC     DIAGWORD+1
;        LDD     MBOPSTS2
;        ANDD    $0200
;        BEQ     OKA
        NOP
;;OKA     NOP

;        LDD     MBEXTRUDERNO
;        CPD    #1              ; IS THIS IS NO 1
;        BNE     NO1             ; NO
;        LDD     MBVACUUMSTATUS
;        ANDD    #$0001
;        BEQ     OK1
;PARAIC:
;        NOP
;OK1:
;NO1     LDD     MBEXTRUDERNO
;        CPD    #2              ; IS THIS IS NO 1
;        BNE     NO2             ; NO
;        LDD     MBVACUUMSTATUS
;        ANDD    #$0002
;        BEQ     OK2
;PARAIC1:
;        NOP
;OK2:
;
;NO2:



        LDAA    BLENDERMODE     ; CHECK BLENDER MODE
        CMPA    #SINGLERECIPEMODE   ; SINGLE RECIPE MODE
        BNE     LOADPERTARGETSFROMCOMMS ; LOAD
        LDAA    #1
        STAA    RECIPEWAITING         ; INDICATE THAT RECIPE IS READY
        LDAA    MBEXTRUDERNO+1
        JSR     SETVACUUMSTATUS
        JSR     COPYBBAUXMB
        BRA     XITLD1              ; EXIT.


LOADPERTARGETSFROMCOMMS:
        TST     CYCLEINDICATE   ; IS THE BLENDER CYCLING ?
        BEQ     TRANSFERSETPOINTSFROMCOMMS ; NO
        LDAA    #TRANSFERSPFROMCOMMSID
        STAA    TRANSFERSPF     ; TRANSFER SET POINTS FLAG SET.
        JSR     DISPWILLTRANSFERSETPOINTS       ; DISPLAY WILL TRANSFER SETPOINTS.
        LDAA    #$0F              ;
        STAA     CHANGINGORDER   ; SET CHANGING ORDER FLAG.
        LDD     MBRSTOTLS           ; .
        ANDD    #MBSTARTCLEANBPOS   ; START CLEAN CYCLE
        BEQ     XITLD1               ; NO.
        JSR      SETSTARTCLEANMBBIT
        JSR      RESETCLEANFINISHBIT
XITLD1  RTS





TRANSFERSETPOINTSFROMCOMMS:
        LDAA    MBREGCOMPNOSP+1 ; READ SP COMPONENT NO.
        STAA    REGCOMP         ; STORE TO CALIBRATION DATA ENTRY.
        STAA    REG_PRESENT     ; INDICATE REGRIND.
        LDAA   #$AA
        STAA   CEEFLG1           ; SET FLAG TO HAVE DATA -> EEPROM

        LDAB   #BANK4
        TBYK                    ;~;SET EK FOR BANK 1, I.E. RAM
        LDY     #PERLOOKUP
;        LDX     #MBPCNT1        ; MODBUS TABLE, % #1
        LDAB   #RAMBANK
        TBZK                    ;~;SET EK FOR BANK 1, I.E. RAM
        LDX     #PCNT1          ; PRODUCTION SETPOINTS, % #1
        LDAB    NOBLNDS         ; NO. OF BLENDS/COMPONENTS ON MACHINE
LDMBPT10 PSHB
        LDZ     0,Y             ; READ DATA POINTER
        LDD     0,Z
        CPD     #10000           ; NOTE 100% ON BLENDER STORED AS 9999
        BNE     LDMBPT12
        LDD     #9999          ; 100% (NOTE 2DP)
LDMBPT12:
        JSR     HEXBCD2         ; CONVERT TO BCD
        STE     0,X
NEXCMP  AIX     #2
        AIY     #2
        PULB
        DECB
        BNE     LDMBPT10

        LDAB    #MAXNOBLNDS     ; PLACE FOR 6 BLENDS IN PROD. SPTS
        SUBB    NOBLNDS         ; GET NO. OF UNUSED BLENDS
        BEQ     LDMBPT20
LDMBPT15 CLRW   0,X             ; SET UNUSED BLEND %'s TO 0000
        AIY     #2
        DECB
        BNE     LDMBPT15

LDMBPT20 CLRE
        TST     AVECPIB         ; IS THERE PIB?
        BEQ     LDMBPT25
;        LDD     MBPCNTLIQ       ; MODBUS VERSION OF LIQ %
;        JSR     HEXBCD2
LDMBPT25:
;        STE    PCNTLIQ
LDMBPT30:
;        LDD     MBPCT_ADD         ; % DRY ADDITIVE
;        JSR     HEXBCD2
;        STE     PCT_ADD

        LDD     MBHIREGPCNT        ; READ HIGH REGRIND %
        JSR     HEXBCD2
        STE     HIREGPCNT

        LDE     MBEXTRUDERNO
        STE     EXTRUDERNO      ; STORE RECIPE NO


        LDAB    #$0F
        STAB    NEWSET          ; SET FLAG FOR NEW SETPOINTS //
        JSR     RESETALLFILLALARMS

        LDD     MBRSTOTLS           ; .
        ANDD    #MBSTARTCLEANBPOS   ; START CLEAN CYCLE
        BEQ     XITLD               ; NO.
        JSR     INITIATECLEANCYCLE ; SWITCH TO MANUAL.
XITLD   RTS





;* LDMBTPTS    - LOAD TARGET THROUGHPUT DATA FROM MODBUS TABLE
LDMBTPTS:
        LDX     #MBWTPMSP+1
        JSR     HEXBCD3X        ; RESULT IN B:E

        CLR     WTPMSP          ; RESET GRAMS PER METER.
        STAB    WTPMSP+1
        STE     WTPMSP+2        ; BCD, 2DP

        LDD     MBTKGHSP        ; NOTE: MAX EXPECTED VALUE IS 6553.5 Kg/H
        JSR     HEXBCD2

        LDAA    BLENDERMODE
        CMPA    #GPMGPMENXXKGS
        BEQ     NOSH1           ; NO SHIFT OF DATA
        CMPA    #OFFLINEMODE    ; OFFLINE MODE
        BNE     NRMDNL1         ; NORMAL DOWNLOAD

        LDAA    OFFLINETYPE             ; CHECK TYPE.
        CMPA    #OFFLINETYPEXXKG        ; DISPENSE XX KGS.
        BEQ     NOSH1

NRMDNL1 JSR     SHF_BE4L         ; SHIFT 4 PLACES LEFT, (*10), PROD. HAS 2DP
NOSH1   STAB    TKGHSP
        STE     TKGHSP+1        ; Kg/H SETPOINT (2DP)

        LDD     MBGPM2SPT       ; NOTE: MAX VALUE EXPECTED IS 655.35g/m
        JSR     HEXBCD2
        STAB    GPM2SPT        ; g/sq. m SETPOINT
        STE     GPM2SPT+1

        LDD     MBLSPDSP        ; LINE SPD SPT NOTE: MBUS VERSION HAS 1 DP
        JSR     HEXBCD2
        JSR     SHF_BE4R        ; PROD. LSPD. HAS NO DP, I.E. DIV. BY 10
        STE     LSPDSP          ; LINE SPEED SETPOINT (m/Min) INTEGER ONLY

        LDD     MBWIDTHSP       ; MODBUS  WIDTH SPT mm
        JSR     HEXBCD2
        STE     WIDTHSP         ; MAX EXPECTED IS 9999mm

        LDD     MBTRIMWID       ; MODBUS TRIMMED WIDTH (mm)
        JSR     HEXBCD2
        STE     TRIMWID         ; MAX EXPECTED IS 9999mm

        LDD     MBDBXXKGS          ; DEADBAND XX KGS
        JSR     HEXBCD2
        STAB    DBXXKGS
        STE     EREG
        LDAA    EREG
        STAA    DBXXKGS+1      ; ONLY STORE KGS

        JSR     SPCHANG         ; INITIATE CONTROL ACTION IF IN AUTO.
        JSR     CONVERTBATSETALW
        RTS


;* IMBTBFLGS        - INITIALISE MODBUS TABLE FLAGS
;       B15     $8000   - INDICATES WRITEABLE
;       B14     $4000   - RESET ORDER TOTALS (A TOGGLE STATUS)
;       B13     $2000   - WEIGH HOPPER CALIBRATION
;       B12     $1000   - TOGGLE STATUS, FILL, INC/DEC, A/M
;       B11     $0800   - OPEN / CLOSE VALVES.
;       B10     $0400   - TIME
;       B9      $0200   - CALIBRATION DATA, LIQ. ADD. CALIBRATION
;       B8      $0100   - CALIBRATION DATA, SYSTEM CONFIG. DATA
;       B7      $0080   - CAL. DATA, LINE & SCREW SPEED CONSTANTS
;       B6      $0040   - CAL. DATA, MEAS. PARAMETERS, AF1 ETC
;       B5      $0020   - CAL. DATA, HOPPER CONSTANTS, FILL TIME ETC
;       B4      $0010   - CAL. DATA, WEIGHT CONSTANTS, CON1,TARE & MBxCALC
;       B3      $0008   - COMPONENT CONFIGURATION
;       B2      $0004   - SETPOINT DATA, ORDER INFO
;       B1      $0002   - SETPOINT DATA, THROUGH-PUT DATA
;       B0      $0001   - SETPOINT DATA, TARGET %'s

IMBTBFLGS LDX   #MBTBLFLGS      ; FIRSTLY CLEAR COMPLETE TABLE
        LDD     #MBTABWSZ       ; SIZE OF MODBUS TABLE WRITE AREA IN BYTE
        LSRD                    ; WORDS = BYTES/2
        LDE     #$8000          ; MSB INDICATES WRITEABLE
        JSR     WRTEDX          ; WRITE E D TIMES AT X

        LDX     #MBPCNT1
        LDD     #PARFLGOF       ; OFFSET TO PARAM. FLAGS
        ADX                     ; X NOW POINTS TO PAR FLAG FOR % 1
        LDE     #$8001          ; MSBIT => WRITEABLE, B0 INDICATES SPT %
        LDAB    #12             ; 12 TARGET %'S
        JSR     WRTEBX          ; WRITE E ACC B TIMES AT X

        LDX     #MBPCNT9
        LDD     #PARFLGOF       ; OFFSET TO PARAM. FLAGS
        ADX                     ; X NOW POINTS TO PAR FLAG FOR % 1
        LDE     #$8001          ; MSBIT => WRITEABLE, B0 INDICATES SPT %
        LDAB    #4              ; TARGET %'S
        JSR     WRTEBX          ; WRITE E ACC B TIMES AT X

        LDX     #MBWTPMSP
        LDD     #PARFLGOF       ; OFFSET TO PARAM. FLAGS
        ADX                     ; X NOW POINTS TO PAR FLAG FOR WT/M SETPT
        LDE     #$8002          ; B1 INDICATES A THROUGPUT PARAMETER
;        LDAB    #6              ; 6 TPT PARAMETERS
        LDAB    #7              ; 7 TPT PARAMETERS ONE EXTRA FOR WEIGHT PER METER DOWNLOAD 2 WRODS
        JSR     WRTEBX

        LDX     #MBMICRONSET
        LDD     #PARFLGOF       ;
        ADX                     ;
        LDE     #$8002          ;
        LDAB    #2              ;
        JSR     WRTEBX

        LDX     #MBFROLLSP
        LDD     #PARFLGOF       ; OFFSET TO PARAM. FLAGS
        ADX                     ; X NOW POINTS TO PAR FLAG FOR F. ROLL LT. SETPT
        LDE     #$8004          ; B2 INDICATES ORDER INFO.
        LDAB    #34             ; 34 WORDS OF ORDER INFO
        JSR     WRTEBX          ;

        LDX     #MBNONSTDCCFG    ; NON STANDARD COMPONENT CONFIGURATION.
        LDD     #PARFLGOF       ; OFFSET TO PARAM. FLAGS
        ADX                     ; X NOW POINTS TO PAR FLAG FOR CH1 K VALUE
        LDE     #$8008          ; COMPONENT CONFIGURATION.
        LDAB    #9              ;
        JSR     WRTEBX
;
        LDX     #MBRSTOTLS      ; RESET ORDER TOTALS
        LDD     #PARFLGOF       ; OFFSET TO PARAM. FLAGS
        ADX                     ; X NOW POINTS RESET O. TOTALS PAR.FLAG
        LDE     #$C000          ; RESET ORDER TOTALS FLAG, WRITE,TOG. STAT. BIT
        STE     0,X
        AIX     #1

        LDX     #MB1CON1
        LDD     #PARFLGOF       ; OFFSET TO PARAM. FLAGS
        ADX                     ; X NOW POINTS TO PAR FLAG FOR BLOCK 1 OF CAL DATA
        LDE     #$A000          ; CALIB. DATA , WT CONST's (CON1 & TARE)
        STE     0,X             ; WT CONST



        LDX     #MBCBLK1
        LDD     #PARFLGOF       ; OFFSET TO PARAM. FLAGS
        ADX                     ; X NOW POINTS TO PAR FLAG FOR BLOCK 1 OF CAL DATA
        PSHM    X               ; SAVE ADDR. OF PARFLG FOR CAL BLOCK 1
        LDE     #$8010          ; CALIB. DATA , WT CONST's (CON1 & TARE)
        LDAB    #1              ; START WITH COMPONENT 1
IMBTPF30:
        STE     0,X             ; WT CONST
        STE     6,X            ; TARES HAVE MOVED TO AFTER THE CAL CODE
        STE     16,X            ; TARE
        STE     18,X            ; TARE (LONG)
        STE     14,X            ; CALIB. CODE WORDS

        PULM    X               ; RESTORE PAR. FLAG ADDR. FOR CAL. BLOCK 1
        PSHM    X

        LDE     #$8020          ; CALIB. DATA , HOPPER CONST.S)
        LDAB    #1              ; START WITH COMPONENT 1
IMBTPF33:
        STE     2,X             ; FILL TARGET WT.
        STE     4,X            ; MAX THRUPUT
        PULM    X               ; RESTORE PAR FLAG ADDR. FOR CAL BLOCK 1
        LDE     #$8000          ; CALIB. DATA BLOCK, SPARES, WRITE BUT NO ACTION
        LDAB    #1              ; START WITH COMPONENT 1
IMBTPF36:
        LDX     #MBAF1          ; MEASUREMENT PARAMETERS ( AVERAGE FACTOR 1)
        LDD     #PARFLGOF       ; OFFSET TO PARAM. FLAGS
        ADX                     ; X NOW POINTS TO PAR FLAG FOR MEAS. PAR.
        LDE     #$8040          ; MSBIT => WRITEABLE, B6 INDICATES MEAS. PARAMETER
        LDAB    #5              ; 5 PARAMETERS
        JSR     WRTEBX          ; WRITE E ACC B TIMES AT X

        LDX     #MBLSC1         ; LINE & SCREW SPD PARAMETERS (L.S. CONST.)
        LDD     #PARFLGOF       ; OFFSET TO PARAM. FLAGS
        ADX                     ; X NOW POINTS TO PAR FLAG FOR L.S. CONST.
        LDE     #$8080          ; MSBIT => WRITEABLE, B7 INDICATES L&S S.
        LDAB    #6              ; 6 PARAMETERS
        JSR     WRTEBX          ; WRITE E ACC B TIMES AT X


; SYSTEM CONFIGURATION.

        LDX     #MBLINENO       ; SYS. CONFIG. PARAMETERS (LINE NO)
        LDD     #PARFLGOF       ; OFFSET TO PARAM. FLAGS
        ADX                     ; X NOW POINTS TO PAR FLAG FOR NOBLNDS
        LDE     #$8100          ; MSBIT => WRITEABLE, B8 INDICATES SYS CFG
        LDAB    #MBSYSCFGNO     ;  PARAMETERS
        JSR     WRTEBX          ; WRITE E ACC B TIMES AT X

; SYSTEM CONFIG CALIBRATION DATA ADDED

        LDX     #MBCBLK2       ;
        LDD     #PARFLGOF       ;
        ADX                     ;
        LDE     #$8100          ;
        LDAB    #44             ;
        JSR     WRTEBX          ;

        LDX     #MBCBLKL        ; LIQ ADD. CAL. PARAMETERS ( L. SIGNAL TYPE)
        LDD     #PARFLGOF       ; OFFSET TO PARAM. FLAGS
        ADX                     ; X NOW POINTS TO PAR FLAG FOR NOBLNDS
        LDE     #$8200          ; MSBIT => WRITEABLE, B9 INDICATES LIQ. CAL
        LDAB    #6              ; 6 PARAMETERS
        JSR     WRTEBX          ; WRITE E ACC B TIMES AT X

        LDX     #MBOPENCMP      ; TOGGLE STATUS FOR FILL COMMAND.
        LDD     #PARFLGOF       ; OFFSET TO PARAM. FLAGS
        ADX                     ; X NOW POINTS TO PAR FLAG FOR NOBLNDS
        LDE     #$9000          ; MSBIT => WRITEABLE, B12 INDICATES TOG STATUS
        LDAB    #15             ; 15 PARAMETERS
        JSR     WRTEBX          ; WRITE E ACC B TIMES AT X


        LDX     #MBMINSEC       ; TIME [MIN:SEC]
        LDD     #PARFLGOF       ; OFFSET TO PARAM. FLAGS
        ADX                     ; X NOW POINTS TO PAR FLAG FOR NOBLNDS
        LDE     #$8400          ; MSBIT => WRITEABLE, B10 INDICATES TOG STATUS
        LDAB    #4              ; 4 PARAMETERS
        JSR     WRTEBX          ; WRITE E ACC B TIMES AT X


; CONTINUE TABLE FLAGS HERE

        CLRW    MBWORFLG        ; WRITE 'OR' FLAG
        CLRW    MBWORFER        ; WRITE 'OR' FLAG ERROR
        RTS

;
;* CHMBVDATA - CHECKS FOR VALID MODBUS DATA, BASED ON WRITE OR FLAGS
;              IF INVALID DATA, THEN ILLEGAL DATA CODE RETURNED IN B
;                               WRITE OR FLAGS STORED IN MBWORFER (ERROR)
;              IF VALID DATA, B IS CLEARED, AND WORF STORED IN MBWORFLG
CHMBVDATA:
        CLRB
        LDE     MBWORFTP        ;TEMP CONTAINING OR'ed FLAGS
        BEQ     CHMBVDEX        ; NO FLAG SET, EXIT
        ANDE    #$000F          ; ANY SETPT DATA?
        BEQ     CHMBVD10        ; IF NOT SKIP PAST SETPOINT CHECKS

        ANDE    #$0001          ; B0 SET => CHECK TARGET %'S VALID
        BEQ     CHMBVTPTS
CHMBVPTS:
        TST     SINGLEWORDWRITE        ; LINK INSTALLED
        BNE     CHMBVTPTS       ; NO 100% IF SINGLE WORD WRITE
        JSR     CHMB100T
        TSTB
        BNE     CHMBVDERR       ; ERROR IF B NOT 0, REJECT ALL DATA WRITTEN
        BRA     SKLG            ; SKIP LAG

CHMBVTPTS:
        LDAB    #LAGTIMEFORSPS
        STAB    LDNSPLAG        ; TIME ALLOWED FOR COMMAND TO LOAD SPT's (5.0SEC)

SKLG    LDE     MBWORFTP        ;TEMP CONTAINING OR'ed FLAGS
        ANDE    #$0002          ; B1 SET => CHECK TPT DATA
        BEQ     CHMBVOI
;routine to check for valid throughputs
CHMBVOI:                        ; CHECK FOR VALID ORDER INFO
        LDE     MBWORFTP        ;TEMP CONTAINING OR'ed FLAGS
        ANDE    #$0004          ; B2 SET => CHECK ORDER INFO DATA
        BEQ     CHMBVKS
;routine to check for valid order info

CHMBVKS:                        ; CHECK FOR VALID K DATA
        LDE     MBWORFTP        ;TEMP CONTAINING OR'ed FLAGS
        ANDE    #$0008          ; B3 SET => CHECK K DATA
        BEQ     CHMBVD10
;routine to check for valid K values

CHMBVD10:

; WILL NEED TO ADD CHECKS AS APPROP.
;        BRA     CHMBVDEX
CHMBVD_OK:                      ; GET
        LDD     MBWORFTP
        STD     MBWORFLG        ; STORE TEMP 'OR' FLAG IN WRITE FLAG
        CLRB
        BRA     CHMBVDEX
CHMBVDERR:
        LDD     MBWORFTP
        STD     MBWORFER        ; STORE TEMP 'OR' FLAG IN ERROR FLAG
        LDAB    #ILL_DAT        ; ILLEGAL DATA CODE
CHMBVDEX:
        RTS

CHMB100T PSHM   E

        LDAB   #BANK4
        TBYK                    ;~;SET EK FOR BANK 1, I.E. RAM
        LDY     #PERLOOKUP
;        LDX    #MBPCNT1        ; FIRST %
        LDAB   #1
        CLRE
CHM100T2:
        LDX     0,Y
        CMPB    MBREGCOMPNOSP+1 ; READ SP COMPONENT NO.
        BEQ     CHM100T4        ; MOVE ONTO NEXT COMP.
        ADDE     0,X
CHM100T4:
        AIY     #2
        INCB
        CMPB    NOBLNDS
        BLS     CHM100T2

CHM100T6:
        CLRB
        CPE     #10000           ; CHECK IF 100.00 % (NOTE % IS 2DP)
        BEQ     PERISOK
;        LDX      #MBPCNT1
;        LDAB   #RAMBANK
;        TBZK                    ;~;SET EK FOR BANK 1, I.E. RAM
;        LDZ      #BADPCNT1
;        JSR      TRFERXTZ
        LDAA    MBPROALRM       ;
        ANDA    #PERALBIT       ; IS THE ALARM ALREADY ACTIVE ?
        BNE     ISACTVE
        LDAA    #PERALARM
        JSR     PTALTB           ; ACTIVATE ALARM.
ISACTVE LDAB    #1
CHM100TX PULM   E
        RTS

EXITPOK LDAB   #0
        BRA     CHM100TX

PERISOK LDAA    MBPROALRM       ; CHECK % SETTING
        ANDA    #PERALBIT
        BEQ     EXITPOK         ; BIT IS CLEARED.
        LDAA    #PERALARM
        JSR     RMALTB
        BRA     EXITPOK         ; EXIT.


;
;* POSTMBWRT - ROUTINE CALLED AFTER A WRITE INTO MODBUS TABLE
;               THIS ROUTINE IS CALLED IN THE BACKGROUND
POSTMBWRT:
        TST     PROTOCOL
        LBPL    PTMBWRT_X
        LDE     MBWORFER        ; 'OR' FLAG ERROR
        LBEQ     PTMBWRT50
; Error, illegal data, copy data from system into MB TABLE
        ANDE    #$0001          ; CHECK FOR % ERROR
        BEQ     PTMBWRT02
        JSR     CPYPCTSMB       ; COPY %'S BACK INTO MB TABLE
PTMBWRT02:
        LDE     MBWORFER        ; ERROR FLAG
        ANDE    #$0002          ; CHECK FOR TPT ERROR
        BEQ     PTMBWRT06
        JSR     CPYTPTSMB       ; COPY TPT SETPOINTS BACK INTO MB TABLE
PTMBWRT06:
        LDE     MBWORFER        ; ERROR FLAG
        ANDE    #$0004          ; CHECK FOR ORDER INFO ERROR
        BEQ     PTMBWRT08
        JSR     CPYOINFMB       ; COPY ORDER INFO SETPOINTS BACK INTO MB TABLE
PTMBWRT08:
        LDE     MBWORFER        ; ERROR FLAG
        ANDE    #$0008          ; CHECK FOR K DATA ERROR
        BEQ     PTMBWRT12
        JSR     COPYKSMB       ; COPY K VALUES BACK INTO MB TABLE
PTMBWRT12:
        LDE     MBWORFER        ; ERROR FLAG WT CONSTs DATA ERROR
        ANDE    #$0010          ; CHECK FOR WT CONSTs DATA ERROR
        BEQ     PTMBWRT16
        JSR     CPYWTCMB       ; COPY Wt CONSTs BACK INTO MB TABLE
PTMBWRT16:
        LDE     MBWORFER        ; ERROR FLAG HOPPER CONSTs DATA ERROR
        ANDE    #$0020          ; CHECK FOR  DATA ERROR
        BEQ     PTMBWRT20
        JSR     CPYHPCMB       ; COPY HOPPER CONSTs BACK INTO MB TABLE
PTMBWRT20:
        LDE     MBWORFER        ; ERROR FLAG MEASUREMENT & CONTROL PARAMETERS
        ANDE    #$0040          ; CHECK FOR  DATA ERROR
        BEQ     PTMBWRT24
        JSR     CPYMCPMB       ; COPY PARAMETERS BACK INTO MB TABLE
PTMBWRT24:
        LDE     MBWORFER        ; ERROR FLAG SPEED CONST.S
        ANDE    #$0080          ; CHECK FOR  DATA ERROR
        BEQ     PTMBWRT28
        JSR     CPYLSCMB        ; COPY SPEED CONST BACK INTO MB TABLE
PTMBWRT28:
        LDE     MBWORFER        ; ERROR FLAG SYSTEM CONFIGURATION
        ANDE    #$0100          ; CHECK FOR  DATA ERROR
        BEQ     PTMBWRT32
        JSR     CPYSYCMB        ; COPY SYS. CONFIG. BACK INTO MB TABLE
PTMBWRT32:
        LDE     MBWORFER        ; ERROR FLAG LIQ CALIBRATION
        ANDE    #$0200          ; CHECK FOR  DATA ERROR
        BEQ     PTMBWRT36
        JSR     CPYLIQCMB        ; COPY LIQ.CONST's BACK INTO MB TABLE
PTMBWRT36:
        LDE     MBWORFER        ; ERROR FLAG TIME
        ANDE    #$0400          ; CHECK FOR  ERROR
        BEQ     PTMBWRT40
        JSR     CPYTIMMB        ; COPY TIME BACK INTO MB TABLE
PTMBWRT40:
;; OTHER ERROR CODES TO BE HANDLED HERE
        LBRA     PTMBWRT_Q       ; EXIT

PTMBWRT50:                      ; GOOD DATA WRITE
        LDE     MBWORFLG        ; 'OR' FLAG - WRITE
        LBEQ     PTMBWRT_X       ; IF CLEAR, NO DATA WRITTEN

; Write, good data, copy data from  MB TABLE into system
        ANDE    #$0007          ; ANY SETPT DATA?  // WAS 000F
        LBEQ     PTMBWRT58       ; IF NOT SKIP PAST SETPTS WAS 66 ORIGIONALLY

; if L5 fitted (0) then will need to set flag to load setpoints
        TST     SINGLEWORDWRITE          ;SINGLE WORD WRITE ?
        LBNE     PTMBWRT66

        TSTW    MBRSTOTLS       ; RESET ORDER TOTALS FLAG IN MB TABLE
        BNE     PTMBWRT50B        ; IF NON-ZERO NO ACTION REQ'D
        JSR     ASSORPT         ; ASSEMBLE ORDER REPORT
;        LDAB    #$0F
;        STAB    OCOFLG          ; SET ORDER CHANGE OCCURRED FLAG
        CLRW    MBRSTOTLS       ; CLEAR FLAG
PTMBWRT50B:
        LDE     MBWORFLG        ; 'OR' FLAG - WRITE
        ANDE    #$0001          ; CHECK FOR % WRITE
        BEQ     PTMBWRT52
        JSR     LDMBPCTS       ; LOAD %'S FROM MB TABLE
PTMBWRT52:
        LDE     MBWORFLG        ; 'OR' FLAG - WRITE
        ANDE    #$0002          ; CHECK FOR NEW TPT SETPOINTS
        BEQ     PTMBWRT56
        JSR     LDMBTPTS       ; LOAD TPT'S FROM MB TABLE
PTMBWRT56:
        LDE     MBWORFLG        ; 'OR' FLAG - WRITE
        ANDE    #$0004          ; CHECK FOR NEW ORDER INFO SETPOINTS
        BEQ     PTMBWRT58
        JSR     LDMBOINFO       ; LOAD ORDER INFO FROM MB TABLE

PTMBWRT58:
        LDE     MBWORFLG        ; 'OR' FLAG - WRITE
        ANDE    #$0008          ;
        BEQ     PTMBWRT62
        JSR     LDMBCOMPCFG     ; LOAD COMPONENT CONFIGURATION.
PTMBWRT62:

        LDE     MBWORFLG        ; 'OR' FLAG - WRITE
        ANDE    #$2000          ; CHECK FOR NEW K VALUES
        BEQ     PTMBWRT63
        JSR     LDMBHOPPERCAL   ; LOAD HOPPER CALIBRATION.
PTMBWRT63:

;NOTE MBNEWDATA BIT 10 IS SET IN CPYORRMB

PTMBWRT66:
        LDE     MBWORFLG        ; 'OR' FLAG - WRITE
        ANDE    #$0010          ; CHECK FOR NEW Wt CONSTS (C1s & TARES)
        BEQ     PTMBWRT70
        JSR     LDMBWCS         ; LOAD Wt CONSTs FROM MB TABLE
PTMBWRT70:
        LDE     MBWORFLG        ; 'OR' FLAG - WRITE
        ANDE    #$0020          ; CHECK FOR NEW HOPPER CONSTS (TARGET WTs ETC)
        BEQ     PTMBWRT74
        JSR     LDMBHPCS         ; LOAD HOPPER CONSTs FROM MB TABLE
PTMBWRT74:
        LDE     MBWORFLG        ; 'OR' FLAG - WRITE
        ANDE    #$0040          ; CHECK FOR NEW MEAS. PARAMETERS (AF1, AF2, CG, DT)
        BEQ     PTMBWRT78
        JSR     LDMBMCPS         ; LOAD MEAS. & CONTROL PARAMETERS FROM MB TABLE
PTMBWRT78:
        LDE     MBWORFLG        ; 'OR' FLAG - WRITE
        ANDE    #$0080          ; CHECK FOR SPEED CONSTANT (LINE & SCREW SPEED)
        BEQ     PTMBWRT82
        JSR     LDMBSCS         ; LOAD SPEED CONST's FROM MB TABLE
PTMBWRT82:
        LDE     MBWORFLG        ; 'OR' FLAG - WRITE
        ANDE    #$0100          ; CHECK FOR SYS. CONFIGURATION
        BEQ     PTMBWRT86
        JSR     LDMBSYC        ; LOAD SYS CONFIG. FROM MB TABLE
PTMBWRT86:
        LDE     MBWORFLG        ; 'OR' FLAG - WRITE
        ANDE    #$0200          ; CHECK FOR LIQ. CALIBRATION DATA
        BEQ     PTMBWRT89
        JSR     LDMBLIQC       ; LOAD SYS CONFIG. FROM MB TABLE
PTMBWRT89:
        LDE     MBWORFLG        ; 'OR' FLAG - WRITE
        ANDE    #$0400          ; CHECK TIME DATA
        BEQ     PTMBWRT92
        JSR     LDMBTIME       ; LOAD TIME FROM MB TABLE

PTMBWRT92:
        LDE     MBWORFLG        ; 'OR' FLAG - WRITE
        ANDE    #$1000          ; CHECK FOR TOGGLE STATUS COMMANDS
        BEQ     PTMBWRT94
        JSR     MBTOGSTA        ; ROUTINE TO ACTIVATE COMMANDS FROM MB TABLE
PTMBWRT94:
PTMBWRT95:
PTMBWRT_Q:
        CLRW    MBWORFER        ; CLEAR 'OR' ERROR FLAG
        CLRW    MBWORFLG        ; CLEAR GOOD DATA WRITE FLAG ALSO
PTMBWRT_X RTS


;
CHMBTOGS:             ; MODBUS TOGGLE STATUS FLAGS
CHMBTOGS20:
        RTS

; OPEN COMMAND

OPENTOGGLE:
        LDAA    PAUSFLG           ; READ PAUSE
        ANDA    #PAUSEATENDOFCYCLE
        BEQ     OPX               ; EXIT.
        TSTW    MBOPENCMP         ; OPEN COMPONENT COMMMAND.
        BEQ     OPX
        JSR     CHKFOROPEN        ; CHECK FOR OPEN
OPX     RTS

; CLOSE COMMAND

CLOSETOGGLE:
        LDAA    PAUSFLG           ; READ PAUSE
        ANDA    #PAUSEATENDOFCYCLE
        BEQ     CLX               ; EXIT.
        TSTW    MBCLOSECMP         ; CLOSE COMMAND.
        BEQ     CLX
        JSR     CHKFORCLOSE        ; CHECK FOR OPEN
CLX     RTS


; ACCA HAS THE VALVE TO BE OPENED.

CHKFOROPEN:
        LDE     MBOPENCMP
        LDAA    NOBLNDS         ; NO OF BLENDS.
        LDAB    #1              ; COMPONENT COUNTER.
REPOPC  LSRE                    ;
        PSHM    D,E
        BCC     NXTCMP          ; GO TO NEXT COMPONENT.
        JSR     MBCMPNON        ; SWITCH COMPONENT #N ON.
NXTCMP  PULM    D,E             ; RECOVER REGISTERS.
        INCB                    ; INCREMENT COMPONENT NO.
        DECA                    ; DECREMENT COUNTER.
        BNE     REPOPC          ; REPEAT OPEN CHECK
        CLRW    MBOPENCMP       ; RESET
        RTS


CHKFORCLOSE:
        LDE     MBCLOSECMP
        LDAA    NOBLNDS         ; NO OF BLENDS.
        LDAB    #1              ; COMPONENT COUNTER.
REPCLC  LSRE                    ;
        PSHM    D,E
        BCC     NXTCMPC         ; GO TO NEXT COMPONENT.
        JSR     MBCMPNOFF       ; SWITCH COMPONENT #N OFF.
NXTCMPC PULM    D,E             ; RECOVER REGISTERS.
        INCB                    ; COMPONENT NO ++
        DECA                    ; DECREMENT COUNTER.
        BNE     REPCLC          ; REPEAT CLOSE CHECK
        CLRW    MBCLOSECMP      ; RESET
        RTS                     ; FINISH.

;
;* COPY WEIGHT CONSTANTS INTO MODBUS TABLE CALIBRATION DATA AREA
CPYWTCMB:
        TST     PROTOCOL
        LBPL     CPYWTCMX

        LDAB   #CON1OF
        LDX    #CALBLK
        ABX                        ;~X NOW POINTS TO C1 FOR THIS CHANNEL
        LDAB     1,X
        LDE      2,X
        JSR     BCDHEX3         ; BCD TO HEX.
        CLRW    MBCALCON        ; CLEAR MSB
        STE     MBCALCON+2


        LDX     #CH1CBLK       ; CALIBRATION BLOCK
; TARE VALUE.
        LDAB    OFFSOF,X
        LDE     OFFSOF+1,X      ; LOAD UP BCD TARE VALUE (3 BYTES)
        JSR     BCDHEX3         ; CONVERT TO HEX
        CLRA
        STD     MB1TARE
        STE     MB1TARE+2       ;TARE WAS MOVED TO +16 - VALUE T0 999,999

        LDD     MBNEWDATA
        ORD     #$0002          ; SET B1 TO INDICATE NEW CAL. DATA
        STD     MBNEWDATA
CPYWTCMX:
        RTS

;* COPY HOPPER CONSTANTS INTO MODBUS TABLE CALIBRATION DATA AREA
CPYHPCMB:
        LDAB   #RAMBANK
        TBZK                    ;~;SET EK FOR BANK 1, I.E. RAM
        LDZ     #MBCBLK1        ; MODBUS TABLE, CH #1 CAL BLOCK
        LDAB    #1              ; BLEND #, START AT 1
CPYHPC10 PSHB
        LDX      #DFTW1          ; FILL TARGET WT. (.1Kg)
        LDD     0,X             ; PICK UP TARGET WT
        JSR     BCDHEX2         ; CONVERT TO HEX
        STD     MB1FTW          ; STORE IN MODBUS TABLE

        PULB
        PSHB

        LDX     #MXTPT1         ; MAX THRUPUT (Kg/H)
        LDD     0,X             ; PICK UP MAX TPT
        LDD     #$0999          ; MAX KG/HR SET AT 99.9
        JSR     BCDHEX2         ; CONVERT TO HEX
        STD     MB1MXTP

        PULB

CPYHPC20:
        LDD     MBNEWDATA
        ORD     #$0002          ; SET B1 TO INDICATE NEW CALIB DATA
        STD     MBNEWDATA

        RTS
;* COPY MEAS. & CONTROL PARAMETERS INTO MODBUS TABLE CALIBRATION DATA AREA
CPYMCPMB:
        LDAB    #RAMBANK
        TBYK
        LDAB    CONTAVGFACTOR
        CLRA
        STD     MBAF1           ; MODBUS TABLE, AVG. FACTOR 1
        LDAB    WGTAVGFACTOR          ; AVERAGING FACTOR 2
        STD     MBAF2           ; MB TABLE AVG. FACTOR 2

        LDAB   MIXTIM
        JSR    BCDHEX1
        CLRA
        STD    MBMIXTIM

        LDAB   BATINKGHAVG
        JSR    BCDHEX1
        CLRA
        STD    MBBATINKGHAVG
        JSR    RESRROB   ; RESET AVERAGING

        LDAB    CLGAIN          ; CONTROL GAIN
        JSR     BCDHEX1
        CLRA
        STD     MBCG

        LDAB    CONTROLPER       ; CONTROL PERCENTAGE
        JSR     BCDHEX1
        CLRA
        STD     MBCONTROLPER     ; CONTROL %.


        RTS
;* COPY SYSTEM CONFIG. PARAMETERS INTO MODBUS TABLE CALIBRATION DATA AREA
CPYSYCMB:
        PSHM    D
        LDAB    NOBLNDS
        CLRA
        STD     MBNOBDS         ; MODBUS TABLE, NO. OF BLENDS

        LDAB    FILLRETRY       ; FILL RETRY
        CLRA
        STD     MBFILLRETRY

        LDAB    CRLMODE         ; CONTROL MODE (EXTRUDER/PROP./HAULOFF)
        CLRA
        STD     MBCRLMOD

        LDD     LINENO          ; LINE NO. (COMMS PURPOSES)
        JSR     BCDHEX2
        STD     MBLINENO

        LDAB    BLENDERMODE        ; EXTRUSION PROCESS, BLOWN FILM ETC
        STD     MBBLENDERMODE

        LDAB    REGCOMP          ; MIN BLEND 1 %
        JSR     BCDHEX1
        CLRA
        STD     MBREGCOMP

        LDAB    FSTCTRL         ; FAST CONTROL RESPONSE.
        CLRA
        STD     MBFSTCTRL

        LDAB    DIAGMODE        ; DIAGNOSTICS.
        CLRA
        STD     MBDIAGMODE      ;

        LDAB    VACLOAD         ; VACUUM LOADING ON.
        CLRA
        STD     MBVACLOAD       ;

;        LDAB    PURGEOPTION         ; PURGE ENABLED..
;        CLRA
;        STD     MBPURGEOPTION       ;

        LDAB    ISVOLMODE       ; VOLUMETRIC MODE.
        CLRA
        STD     MBISVOLMODE     ;

        LDAB    EIOMITTED       ; SERIAL EI
        CLRA
        STD     MBEIOMITTED     ;

        LDAB    ISVOLMODE       ; VOLUMETRIC MODE.
        CLRA
        STD     MBISVOLMODE     ;

        LDAB    AVECPIB         ; PIB ASSOCIATED
        CLRD
        STD     MBAVECPIB

        LDAB    BLENDERTYPE     ; BLENDER TYPE
        CLRA
        STD     MBBLENDERTYPE

        LDAB    SHUTDOWNEN      ; SHUTDOWN
        CLRA
        STD     MBSHUTDOWNEN    ;

        LDAB    SHUTDOWNTM      ; SHUTDOWN TIME
        JSR     BCDHEX1
        CLRA
        STD     MBSHUTDOWNTM    ;

        LDAB    UNITTYPE        ; TYPE OF UNITS (METRIC-0/IMP-1)
        STD     MBUNITTYPE

        LDAB    SSSTYPE         ; SCREW SPEED SIGNAL TYPE
        CLRA
        STD     MBSSSTYPE

        LDAB    EXSENAB         ; EXTR. STALL CHECK ENABLE FLAG
        BEQ     CPYSYC20
        LDAB    #1              ; USE 1 IN MODBUS TO SHOW ENABLED
CPYSYC20:
        CLRA
        STD     MBEXSEN

        LDAB    LSDELAY
        JSR     BCDHEX1
        CLRA
        STD     MBLSDELAY       ; DELAY TO LEVEL SENSOR ALARM.

        LDAB    OFFLINETYPE
        JSR     BCDHEX1         ; TO HEX.
        CLRA
        STD     MBOFFLINETYPE

        LDAB    HILOREGENABLED       ; HI/LO REG OPTIONS
        JSR     BCDHEX1
        CLRA
        STD     MBHILOREGENABLED     ; TO MODBUS TABLE %.

; COMBINE IN MODBUS OPTIONS
        LDAB    PURGECOMPNO
        JSR     BCDHEX1
        CLRA
        STD     MBPURGECOMPNO         ; .

; CONTROL TYPE
        LDAB    CONTROLTYPE
        JSR     BCDHEX1
        CLRA
        STD     MBCONTROLTYPE         ; .

; CONTROL LAG
        LDD     CONTROLLAG         ; .
        JSR     BCDHEX2
        STD     MBCONTROLLAG

; INCREASE RATE FOR INC/ DEC CONTROL

        LDD     INCREASERATE        ; .
        JSR     BCDHEX2
        STD     MBINCREASERATE

; DECREASE RATE FOR INC/ DEC CONTROL

        LDD     DECREASERATE        ; .
        JSR     BCDHEX2
        STD     MBDECREASERATE

; DEADBAND FOR INC/DEC CONTROL

        LDD     DEADBAND          ; .
        JSR     BCDHEX2
        STD     MBDEADBAND

; FINE INC/DEC BAND

        LDD     FINEINCDEC          ; .
        JSR     BCDHEX2
        STD     MBFINEINCDEC

        LDD     MAXPCNT1          ; .
        JSR     BCDHEX2
        STD     MBMAXPCNT1

        LDD     MAXPCNT2          ; .
        JSR     BCDHEX2
        STD     MBMAXPCNT2

        LDD     MAXPCNT3          ; .
        JSR     BCDHEX2
        STD     MBMAXPCNT3

        LDD     MAXPCNT4          ; .
        JSR     BCDHEX2
        STD     MBMAXPCNT4

        LDD     MAXPCNT5          ; .
        JSR     BCDHEX2
        STD     MBMAXPCNT5

        LDD     MAXPCNT6          ; .
        JSR     BCDHEX2
        STD     MBMAXPCNT6

        LDD     MAXPCNT7          ; .
        JSR     BCDHEX2
        STD     MBMAXPCNT7

        LDD     MAXPCNT8          ; .
        JSR     BCDHEX2
        STD     MBMAXPCNT8

        LDD     MAXPCNT9          ; .
        JSR     BCDHEX2
        STD     MBMAXPCNT9

        LDD     MAXPCNT10         ; .
        JSR     BCDHEX2
        STD     MBMAXPCNT10

        LDD     MAXPCNT11         ; .
        JSR     BCDHEX2
        STD     MBMAXPCNT11

        LDD     MAXPCNT12         ; .
        JSR     BCDHEX2
        STD     MBMAXPCNT12

        LDAB    STAGEFILLEN
        JSR     BCDHEX1
        CLRA
        STD     MBSTAGEFILLEN         ; .

; MOULDING

        LDAB    MOULDING
        JSR     BCDHEX1
        CLRA
        STD     MBMOULDING         ; .

        LDAB    MONITORFACTOR
        JSR     BCDHEX1
        CLRA
        STD     MBMONITORFACTOR         ; .

        LDAB    PH1ONTIME
        JSR     BCDHEX1
        CLRA
        STD     MBPH1ONTIME         ; .

        LDAB    PH1OFFTIME
        JSR     BCDHEX1
        CLRA
        STD     MBPH1OFFTIME        ; .

        LDAB    PH2ONTIME
        JSR     BCDHEX1
        CLRA
        STD     MBPH2ONTIME        ; .

        LDAB    PH2OFFTIME
        JSR     BCDHEX1
        CLRA
        STD     MBPH2OFFTIME        ; .


; BLENDER CLEAN REPS

        LDAB    BCLEANREPETITIONS
        JSR     BCDHEX1
        CLRA
        STD     MBBCLEANREPETITIONS        ; .


; MIXER CLEAN REPS

        LDAB    MCLEANREPETITIONS
        JSR     BCDHEX1
        CLRA
        STD     MBMCLEANREPETITIONS        ; .

; BYPASS TIME LIMIT

        LDAB    BYPASSTIMELIMIT
        JSR     BCDHEX1
        CLRA
        STD     MBBYPASSTIMELIMIT        ; .

                   ;
; VACUUM DELAY

        LDAB    VACUUMDELAY
        JSR     BCDHEX1
        CLRA
        STD     MBVACUUMDELAY        ; .

; VACUUM POST DELAY

        LDAB    VACUUMPOSTDELAY
        JSR     BCDHEX1
        CLRA
        STD     MBVACUUMPOSTDELAY        ; .



                   ;
; TOP UP % ALLOWED

        LDAB    PERTOPUP
        JSR     BCDHEX1
        CLRA
        STD     MBPERTOPUP        ; .

; MAX GRAMS PER %

        LDD     MAXGRAMSPERPER
        JSR     BCDHEX2
        STD     MBMAXGRAMSPERPER        ; .


; BYPASS DELAY TIME DURING CLEANING

        LDAB    BYPASSDCLNTIMELIMIT
        JSR     BCDHEX1
        CLRA
        STD     MBBYPASSDCLNTIMELIMIT        ; .

; TOP UP COMPONENT REFERENCE

        LDAB    TOPUPCOMPNOREF
        JSR     BCDHEX1
        CLRA
        STD     MBTOPUPCOMPNOREF        ; .

; INTERMITTENT MIX TIME
;
        LDAB    INTMIXTIME
        JSR     BCDHEX1
        CLRA
        STD     MBINTMIXTIME        ; .

; PASSWORD
;
        LDX     #PASWRD
        JSR     BCDHEX3X        ;
        STE     MBPASWRD          ;

        LDX     #PASWRD2
        JSR     BCDHEX3X        ;
        STE     MBPASWRD2          ;


        LDD     MXTPT1
        JSR     BCDHEX2
        STD     MBMAXTHRUPUT        ; .

        LDAB    MIXTIM
        JSR     BCDHEX1
        CLRA
        STD     MBOFFLINEMTIME        ; .

; MIXING MOTOR SHUT OFF TIME

        LDAB    MIXINGSHUTOFF
        JSR     BCDHEX1
        CLRA
        STD     MBMIXINGSHUTOFF        ; .




; MBOPTIONS

        LDAB    #RAMBANK
        TBYK                            ; SET UP YK

; HI LO REGRIND

        LDD     #HILOREGENABLEDBITPOS   ; .
        LDX     #HILOREGENABLED
        LDY     #MBOPTIONS
        JSR     SETCLRMBBITPOS

; MAX % CHECK

        LDD     #MAXPCNTCHECKBITPOS   ; .
        LDX     #MAXPCNTCHECK
        LDY     #MBOPTIONS
        JSR     SETCLRMBBITPOS

; 1ST COMPONENT COMPENTATION

        LDD     #FIRSTCOMPCOMPBITPOS   ; .
        LDX     #FIRSTCOMPCOMPENDIS
        LDY     #MBOPTIONS
        JSR     SETCLRMBBITPOS

; LAYERING
        LDD     #LAYERINGBITPOS
        LDX     #LAYERING
        LDY     #MBOPTIONS
        JSR     SETCLRMBBITPOS

; MIXING SCREW DETECTION

        LDD     #MIXSCREWDETBITPOS
        LDX     #MIXSCREWDETECTION
        LDY     #MBOPTIONS
        JSR     SETCLRMBBITPOS

; DOUBLE DUMP

        LDD     #DOUBLEDUMPBITPOS
        LDX     #DOUBLEDUMP
        LDY     #MBOPTIONS
        JSR     SETCLRMBBITPOS

; REGRIND RETRY

        LDD     #REGRINDRETRYBITPOS
        LDX     #REGRINDRETRY
        LDY     #MBOPTIONS
        JSR     SETCLRMBBITPOS

; LEVEL MONITORING

        LDD     #LEVELMONITORBITPOS
        LDX     #LEVELMONITOR
        LDY     #MBOPTIONS
        JSR     SETCLRMBBITPOS

; BYPASS

        LDD     #BYPASSMODEBITPOS
        LDX     #BYPASSMODE
        LDY     #MBOPTIONS
        JSR     SETCLRMBBITPOS

; MANUAL FILL SORTING

        LDD     #MANUALFILLSORTBITPOS
        LDX     #MANUALFILLSORT
        LDY     #MBOPTIONS
        JSR     SETCLRMBBITPOS

; TOPUP

        LDD     #TOPUPBITPOS
        LDX     #TOPUP
        LDY     #MBOPTIONS
        JSR     SETCLRMBBITPOS


; BLOW EVERY CYCLE

        LDD     #BLOWEVERYCYCLEBITPOS
        LDX     #BLOWEVERYCYCLE
        LDY     #MBOPTIONS
        JSR     SETCLRMBBITPOS

; CLEANING

        LDD     #CLEANINGBITPOS
        LDX     #CLEANING
        LDY     #MBOPTIONS
        JSR     SETCLRMBBITPOS

; MODBUS SINGLE WORD WRITE

        LDD     #SINGLEWORDWRITEBITPOS
        LDX     #SINGLEWORDWRITE
        LDY     #MBOPTIONS
        JSR     SETCLRMBBITPOS


; MODBUS FAST / SLOW OPTION

        LDD     #FASTSLOWBITPOS
        LDX     #FASTFLG
        LDY     #MBOPTIONS
        JSR     SETCLRMBBITPOS


; PURGE OPTION

        LDD     #PURGEBITPOS
        LDX     #PURGEOPTION
        LDY     #MBOPTIONS
        JSR     SETCLRMBBITPOS

; USE REGRIND AS REFERENCE

        LDD     #USEREGRINDASREFBITPOS
        LDX     #USEREGRINDASREF
        LDY     #MBOPTIONS2
        JSR     SETCLRMBBITPOS



; DIAGNOSTICS PRINTING

        LDD     #DIAGPENABBPOS
        LDX     #DIAGPENAB
        LDY     #MBPRINTOPTIONS
        JSR     SETCLRMBBITPOS

; MONTHLY PRINTING

        LDD     #MTHPENABBITPOS
        LDX     #MTHPENAB
        LDY     #MBPRINTOPTIONS
        JSR     SETCLRMBBITPOS

; HOURLY PRINTING

        LDD     #HRPENABBITPOS
        LDX     #HRPENAB
        LDY     #MBPRINTOPTIONS
        JSR     SETCLRMBBITPOS

; SHIFT REPORT PRINTING

        LDD     #SHFPENABBITPOS
        LDX     #SHFPENAB
        LDY     #MBPRINTOPTIONS
        JSR     SETCLRMBBITPOS

; ALARM REPORT PRINTING

        LDD     #ALPENABBITPOS
        LDX     #ALPENAB
        LDY     #MBPRINTOPTIONS
        JSR     SETCLRMBBITPOS

; ROLL PRINTING

        LDD     #RRPENABBITPOS
        LDX     #RRPENAB
        LDY     #MBPRINTOPTIONS
        JSR     SETCLRMBBITPOS

; ORDER REPORT PRINTING

        LDD     #ORPENABBITPOS
        LDX     #ORPENAB
        LDY     #MBPRINTOPTIONS
        JSR     SETCLRMBBITPOS


; SERIAL / PARALLEL PRINTING

        LDD     #SERPTROPBITPOS
        LDX     #SERPTROP
        LDY     #MBPRINTOPTIONS
        JSR     SETCLRMBBITPOS


; SCREEN PRINT

        LDD     #SCPENABBITPOS
        LDX     #SCPENAB
        LDY     #MBPRINTOPTIONS
        JSR     SETCLRMBBITPOS


; CORRECT EVERY CYCLE


        LDD     #CORRECTEVERYCYCLEBITPOS
        LDX     #CORRECTEVERYNCYCLES
        LDY     #MBMACOPTIONS
        JSR     SETCLRMBBITPOS


        PULM    D
        RTS



SETCLRMBBITPOS:
        TST     0,X                     ; CHECK OPTION
        BEQ     CLRBIT                  ; OPTION CLEARED
        ORD     0,Y
        STD     0,Y
        BRA     XITSC                   ; EXIT
CLRBIT  COMD
        ANDD    0,Y
        STD     0,Y
XITSC   RTS






;* COPY LINE & SCREW SPEED CONSTS INTO MODBUS TABLE CALIBRATION DATA AREA
CPYLSCMB:
        LDX     #LSC1
        JSR     BCDHEX3X        ; CONVERT 3 BYTES BCD @ X TO HEX
        STE     MBLSC1          ; NOTE: MAX VALUE EXPECTED IS 65535

        LDD     SSCON1          ; SCREW SPEED CONST
        JSR     BCDHEX2
        STD     MBSSC1          ; STORE IN MB TABLE

        LDD     MAXEXSP         ; MAX EXTRUDER SETPOINT.
        JSR     BCDHEX2         ; CONVERT 3 BYTES BCD @ X TO HEX
        STD     MBSSMAX         ; NOTE: MAX VALUE EXPECTED IS 65535

        LDD     SSTARE          ; SCREW SPEED TARE CONSTANT
        JSR     BCDHEX2
        STD     MBSSTARE         ; STORE IN MB TABLE
        RTS


;* COPY LIQUID CONSTS INTO MODBUS TABLE CALIBRATION DATA AREA
CPYLIQCMB:
        LDX     #LAPCON         ; PULSES CONST (FLOW RATE)
        JSR     BCDHEX3X        ; CONVERT 3 BYTES BCD @ X TO HEX
        STE     MBLCON1          ; NOTE: MAX VALUE EXPECTED IS 65535

        LDX     #LAATARE        ; TARE COUNTS (FLOW RATE)
        JSR     BCDHEX3X        ; CONVERT 3 BYTES BCD @ X TO HEX
        STE     MBLTARE          ; NOTE: MAX VALUE EXPECTED IS 65535

        LDAB    LACLAG          ; CONTROL LAG SECS
        JSR     BCDHEX1
        CLRA
        STD     MBLCLAG         ; STORE IN MB TABLE

        LDAB    LACG            ; CONTROL GAIN
        JSR     BCDHEX1
        CLRA
        STD     MBLCG           ; STORE IN MB TABLE

        CLRA
        LDAB    LASTYPE         ; FLOW SIGNAL TYPE
        STD     MBLSTYP

        CLRA
        LDAB    LAMODE          ; MEASUREMENT MODE
        BEQ     CPYLQMB30
        LDAB    #1              ; USE 0/1 IN MODBUS FOR MEAS/MEAS & CONTROL
CPYLQMB30:
        STD     MBLMODE

        RTS


;* COPY Kg/h VALUES INTO MODBUS TABLE PROD. SUMMMARY DATA AREA
CPYKGHMB:
        TST     PROTOCOL
        LBPL     CPYKGHMX

        LDAA    BLENDERMODE      ; CHECK OPERATION MODE.
        CMPA    #OFFLINEMODE     ; OFFLINE MODE
        BEQ     NOTKGH
        TST     BLENDERMODE           ; MODE SELECTION
        BNE     DIFFMODE
ISOFF   LDX     #RRTPTKGH
        BRA     UPMOD
DIFFMODE:
        LDAA     BLENDERMODE     ; READ BLENDER MODE.
        CMPA     #MAXTHROUGHPUTZT ; 0 -10VOLTS OPTION.
        BEQ     NOTKGH
        LDAA    BLENDERMODE
        CMPA    #KGHCONTROLMODE  ; KG/HR MODE
        BNE     NOTKGH           ; SOME OTHER ENTRY
        TSTW    TKGHSP           ; IS KG/HR SET = 0 ?
        BNE     RROBIN
        TST     TKGHSP+2        ; LSB
        BNE     RROBIN
        BRA     NOTKGH          ; DISPLAY ACTUAL KG/HR VALUE.
RROBIN  LDX     #RRKGPH
        BRA     UPMOD
NOTKGH  LDX     #TPTKGH
UPMOD   JSR     BCDHEX3X        ; CONVERT 3 BYTES BCD @ X TO HEX
        CLRA                    ; RESULT IS IN [B:E]
        STD     MBKGH
        STE     MBKGH+2           ;



CPYKGH20 TST   AVECPIB        ; LIQ. ADDITIVE?
        BEQ     CPYKGHMX
        LDX    #LAKGH
        JSR    BCDHEX3X
        CLRA
        STD    MBLKGH         ; LIQ. Kg/h VALUE
        STE    MBLKGH+2       ;
CPYKGHMX:

        LDX     TOTWACCBAT
        STX     EREG
        LDX     TOTWACCBAT+2
        STX     EREG+2
        LDAA    TOTWACCBAT+4
        STAA    EREG+4

        LDX     #EREG     ;
        LDAB    #5
        LDAA    #1
        JSR     GPSFTR            ;~RESULT
        LDX     #EREG+1
        JSR     BCDHEX3X
        STE     MBTOTWACCBAT

        RTS

;* COPY Kg/h  SETPOINT VALUES INTO MODBUS TABLE PROD. SUMMMARY DATA AREA
;CPYSPKMB:
;        TST     PROTOCOL
;        BPL     CPYSPKMX
;
;
;        LDAB    #1              ; BLEND #, START AT 1
;        LDZ     #MB1SETKGH      ; Kg/h SETPOINT FOR BLEND 1 IN MB TABLE
;CPYSPK10 PSHB
;
;        LDX     #SET1KGH         ; Kg/H VALUE
;        DECB
;        LDAA    #3
;        MUL                     ; 3 BYTES/COMP.
;        ABX                     ; X NOW POINTS TO D/A VALUE FOR CH.
;        JSR     BCDHEX3X        ; CONVERT 3 BYTES @ X -> HEX
;        CLRA
;        STD     0,Z
;        STE     2,Z
;
;        PULB
;        INCB
;        AIZ     #MBPSBKSIZ       ; INC. ON TO NEXT CH. MB PROD SUMM BLOCK
;        CMPB    NOBLNDS
;        BLS     CPYSPK10


;CPYSPK20 TST   AVECPIB        ; LIQ. ADDITIVE?
;        BEQ    CPYSPKMX
;        LDX    #LAKGHSP         ; LIQ ADD KG/H SETPT
;        JSR    BCDHEX3X
;        CLRA
;        STD    MBLSETKGH         ; LIQ. Kg/h VALUE
;        STE    MBLSETKGH+2       ;
;CPYSPKMX:
;        RTS

;* COPY g/m VALUE  INTO MODBUS TABLE PROD. SUMMMARY DATA AREA
CPYGPMMB:
        TST     PROTOCOL
        BPL     CPYGPMMX

       LDAA     BLENDERMODE        ;       CHECK EXTRUSION PROCESS
       CMPA     #GPMMICRONENTRY  ; ARE DENSITY ENABLED
       BNE      NOTDEN
       LDX      #ACT_MICRONS    ;~ACTUAL MICRONS
       BRA      UPKGMOD

NOTDEN
;        LDAA    SPTMODE
;        CMPA    #KGHRMODE
        LDAA    BLENDERMODE
        CMPA    #KGHCONTROLMODE  ; KG/HR MODE
        BEQ     WTPM
        TSTW    WTPMSP           ; IS KG/HR SET = 0 ?
        BNE     RROBIN1
        TST     WTPMSP+2        ; LSB
        BNE     RROBIN1
WTPM    LDX     #WTPMVL         ;~ACTUAL WT/M
        LDX    #RRGMPM                   ;~RR AVERAGE VALUE
        JSR     BCDHEX3X        ; CONVERT 3 BYTES BCD @ X TO HEX
        CLRA                    ; RESULT IS IN [B:E]
        STD     MBGPM
        STE     MBGPM+2           ;
        BRA     STRGPM
RROBIN1:
        LDX     #RRGMPM5+1      ;~RR AVERAGE VALUE
UPKGMOD JSR     BCDHEX4X        ; CONVERT 3 BYTES BCD @ X TO HEX
STRGPM  STD     MBGPM
        STE     MBGPM+2           ;
CPYGPMMX RTS



;* COPY g/m2 VALUE  INTO MODBUS TABLE PROD. SUMMMARY DATA AREA
CPYGM2MB:
        TST     PROTOCOL
        BPL     CPYGM2MX

        LDX     #GPM2VAR        ; Wt/m2 VALUE
        JSR     BCDHEX3X        ; CONVERT 3 BYTES BCD @ X TO HEX
        STE     MBGPM2          ; MAX VALUE EXPECTED = 655.35 g/m2
CPYGM2MX RTS

;* COPY LINE SPEED VALUE  INTO MODBUS TABLE PROD. SUMMMARY DATA AREA
CPYLSPMB:
        TST     PROTOCOL
        BPL     CPYLSPMX

        LDAB    LSPD5S+2        ; 5 SECOND LINE SPEED.
        LDE     LSPD5S+3
        JSR     BCDHEX3         ; CONVERT 3 BYTES BCD  TO HEX
        STE     MBLSPD          ; MAX VALUE EXPECTED = 655.35 g/m2
CPYLSPMX RTS

;* COPY EXTRUDER RPM  INTO MODBUS TABLE PROD. SUMMMARY DATA
CPYERPMMB:
        TST     PROTOCOL
        BPL     CPYERPMX

        LDX     #SSRPM2D        ; RPM TO 2DP
        JSR     BCDHEX3X        ; CONVERT 3 BYTES BCD  TO HEX
        STE     MBERPM          ; MAX VALUE EXPECTED = 655.35 RPM

        LDD     HCAD0RES        ; A/D COUNTS
        STD     MBEA_D
CPYERPMX RTS

;* COPY PROD. SUMM. DATA 'A'(EVERY 10S)  INTO MODBUS TABLE
CPYPSAMB:
        TST     PROTOCOL
        BPL     CPYPSAMX

        LDD     FROLLNO         ; FRONT ROLL NO.
        JSR     BCDHEX2
        STD     MBFROLLNO

        LDD     BROLLNO         ; BACK ROLL NO.
        JSR     BCDHEX2
        STD     MBBROLLNO
        JSR     CPYADCMB        ; COPY A/D VALUES INTO MB TABLE
;       JSR     CPYSTVMB        ; SCREW TYPE AND RPM's
        JSR     CPYFWTSMB       ; FRONT ROLL WT ACCUMULATORS (EACH COMP)
        JSR     CPYBWTSMB       ; BACK ROLL WT ACCUMULATORS (EACH COMP)
        JSR     CPYOWTSMB       ; ORDER WT ACCUMULATORS (EACH COMP)
        JSR     CPYSHWTSMB      ; SHFT WTS
        JSR     CPYSHFTLMB     ; SHIFT LENGTH
CPYPSAMX RTS


;* COPY SHIFT LENGTH  INTO MODBUS TABLE PROD. SUMMMARY DATA
CPYSHFTLMB:
        TST     PROTOCOL
        BPL     CPYSHFTLMX
        LDX     #SHFTLTACC
        JSR     BCDHEX3X         ; CONVERT 3 BYTES BCD  TO HEX
        CLRA
        STD     MBSHFTL
        STE     MBSHFTL+2        ; MAX VALUE EXPECTED = 999999 m
CPYSHFTLMX RTS



; COPY SHIFT ACCUMULATED WT.S INTO MODBUS TABLE PROD. SUMMMARY
CPYSHWTSMB:
        TST     PROTOCOL
        LBPL    CPYSHWTSMX

        LDAB   #RAMBANK
        TBZK                    ;~;SET EK FOR BANK 1, I.E. RAM
        LDZ     #MB1SHFTW        ; MODBUS TABLE, CH #1 SHIFT WT
        LDAB    #1              ; BLEND #, START AT 1

CPYSHWTS10 PSHB
        LDX     #CH1WACCS9
        DECB
        LDAA    #5              ; 5 BYTES/ACCUMULATOR
        MUL
        ABX                     ; X NOW POINTS TO THIS CHANNELS SHIFT ACC.
        JSR     BCDHEX4X        ; CONVERT 4 BYTES @X
        STD     0,Z
        STE     2,Z

        AIZ     #MBPSBKSIZ       ; INC. ON TO NEXT CH. MB PROD SUMM BLOCK
        PULB
        INCB
        CMPB    NOBLNDS
        BLS     CPYSHWTS10

CPYSHWTS20:
;total (no dp)
        LDX     #TOTWACCS9         ; SHIFT Wt.
        JSR     BCDHEX3X          ; CONVERT 3 BYTES BCD  TO HEX
        CLRA
        STD     MBSHFTW+0
        STE     MBSHFTW+2

CPYSHWTSMX:
        RTS







;* COPY AUTO/MAN. FLAG TO MODBUS TABLE
CPYAMFMB:                       ; NOW DONE IN UPOPSTATUS
        RTS

;* COPY F. ROLL LENGTH  INTO MODBUS TABLE PROD. SUMMMARY DATA
CPYFRLMB:
        TST     PROTOCOL
        BPL     CPYFRLMX

        LDX     #FRLTACC
        JSR     BCDHEX3X        ; CONVERT 3 BYTES BCD  TO HEX
        STE     MBFRL           ; MAX VALUE EXPECTED = 65535 m
CPYFRLMX RTS

;* COPY B. ROLL LENGTH  INTO MODBUS TABLE PROD. SUMMMARY DATA
CPYBRLMB:
        TST     PROTOCOL
        BPL     CPYBRLMX

        LDX     #BRLTACC
        JSR     BCDHEX3X        ; CONVERT 3 BYTES BCD  TO HEX
        STE     MBBRL           ; MAX VALUE EXPECTED = 65535 m
CPYBRLMX RTS

;* COPY O. ROLL LENGTH  INTO MODBUS TABLE PROD. SUMMMARY DATA
CPYOLMB:
        TST     PROTOCOL
        BPL     CPYOLMX

        LDX     #OLTACC
        JSR     BCDHEX3X         ; CONVERT 3 BYTES BCD  TO HEX
        CLRA
        STD     MBORDL
        STE     MBORDL+2           ; MAX VALUE EXPECTED = 999999 m
CPYOLMX RTS

;* COPY TOTAL WT.S INTO MODBUS TABLE PROD. SUMMMARY DATA
CPYTWTMB:
;       TST     PROTOCOL
;       LBPL     CPYTWTMX

        LDD     TOTWACCO+1       ; ORDER Wt.
        JSR     BCDHEX2          ; CONVERT 2 BYTES BCD  TO HEX
        STD     MBORDW+2
        CLRW    MBORDW

        LDX      #TOTWACCF       ;~FRONT ROLL WT ACCUMULATOR
        JSR      DIVWTBY2          ; WEIGHT / 2
        LDD     EREG+1            ;
        JSR     BCDHEX2          ; CONVERT 2 BYTES BCD  TO HEX
        STD     MBFRW
        LDX     #EREG
        JSR     CALCFSLITX      ; FRONT SLIT CALC.


        LDD     FSWTACC+1        ; FRONT SLIT ROLL WT.
        JSR     BCDHEX2          ; CONVERT 2 BYTES BCD  TO HEX
        STD     MBFSW

        TST    SHTTUBE
        BNE    CPYTWTBB           ;~IF SHEET (01) THEN DO BACK
        CLRW   MBBRW
        CLRW   MBBSW              ; TUBE, CLEAR BACK WT.S
        BRA    CPYTWTMX

CPYTWTBB:

        LDX      #TOTWACCB       ;~FRONT ROLL WT ACCUMULATOR
        JSR      DIVWTBY2          ; WEIGHT / 2
ISTUBFWTB:

        LDD     EREG+1            ;
        JSR     BCDHEX2          ; CONVERT 2 BYTES BCD  TO HEX
        STD     MBBRW

        LDX     #EREG
CALCSB  JSR     CALCFSLITX      ; FRONT SLIT CALC.

        LDD     BSWTACC+1        ; BACK SLIT ROLL WT.
        JSR     BCDHEX2          ; CONVERT 2 BYTES BCD  TO HEX
        STD     MBBSW
CPYTWTMX RTS

;* COPY D/A VALUES INTO MODBUS TABLE PROD. SUMMMARY DATA AREA
CPYDACMB:
        TST     PROTOCOL
        BPL     CPYDACMX

        LDAB   #RAMBANK
        TBZK                    ;~;SET EK FOR BANK 1, I.E. RAM
        LDZ     #MB1DAC         ; MODBUS TABLE, CH #1 D/A VALUE
        LDAB    #1              ; BLEND #, START AT 1

CPYDAC10 PSHB

        LDX     #DA1TMP        ; D/A VALUE
        DECB
        ASLB                    ; *2
        ABX                     ; X NOW POINTS TO D/A VALUE FOR CH.
        LDD     0,X
        STD     0,Z

        PULB
        INCB
        AIZ     #MBPSBKSIZ       ; INC. ON TO NEXT CH. MB PROD SUMM BLOCK
        CMPB    NOBLNDS
        BLS     CPYDAC10

        LDAB    #MAXCOMPONENTNO ; PLACE FOR 8 BLENDS IN MB TABLE
        SUBB    NOBLNDS         ; GET NO. OF UNUSED BLENDS
        BEQ     CPYDAC20
CPYDAC15 CLRW   0,Z             ; SET UNUSED D/A's TO 0000
        AIZ     #MBPSBKSIZ
        DECB
        BNE     CPYDAC15

CPYDAC20:
        LDX     DA1TMP
        STX     MBEA_D

CPYDACMX:
        RTS
;* COPY A/D VALUES INTO MODBUS TABLE PROD. SUMMMARY DATA AREA
CPYADCMB:
        TST     PROTOCOL
        BPL     CPYADCMX

        LDD     RAW132
        CLR     MB1ADC
        STD     MB1ADC+1
        LDAA    RAW132+2
        STAA    MB1ADC+3
CPYADCMX:
        RTS
; LIQUID ADD. PULSES/MIN -> MODBUS TABLE
CPYLPPMB:
        TST    PROTOCOL         ; MODBUS PROTOCOL ($80)??
        BPL    CPYLPPMX
        LDX    #LAPPMIN         ; LIQ. ADDITIVE PULSES/MIN
        JSR    BCDHEX3X         ; CONVERT 3 BYTES @ X
        CLRA
        STD    MBLADC
        STE    MBLADC+2         ; LIQ. A/D VALUE
CPYLPPMX:
        RTS


;* COPY ACTUAL BLEND %'s INTO MODBUS TABLE SETPOINT AREA
CPYAPCTMB:
        TST     PROTOCOL
        BPL     CPYBPTMX
        TST     SHOWINWEIGHT    ; AARE WE SHOW THE INSTANTANEOUS WEIGHT
        BNE     YESINWT
        LDX     #BPRRCNT1       ;  % #1
        BRA     SHOWP1          ; SHOW %

YESINWT LDX     #BPCNT1         ;  % #1
SHOWP1  LDAB   #RAMBANK
        TBZK                    ;~;SET EK FOR BANK 1, I.E. RAM
        LDZ     #MB1APCT        ; MODBUS TABLE, % #1
        LDAB    NOBLNDS         ; NO. OF BLENDS/COMPONENTS ON MACHINE


        LDAB    #1              ; BLEND #, START AT 1
CPYBPT10 PSHB
        LDD     0,X
        JSR     BCDHEX2         ; CONVERT TO HEX
        STD     0,Z
        AIX     #2
        AIZ     #MBPSBKSIZ      ; BLOCK SIZE (PER COMPONENT)
        PULB
        INCB

        LDAA    NOBLNDS
        CMPA    #TEN
        BLS     CMPNOP
        CMPB    #TEN
        BLS     CPYBPT10

        LDD     BPCNT11
        JSR     BCDHEX2         ; CONVERT TO HEX
        STD     MB11APCT

        LDD     BPCNT12
        JSR     BCDHEX2         ; CONVERT TO HEX
        STD     MB12APCT
        BRA     CPYBPT20


CMPNOP  CMPB    NOBLNDS
        BLS     CPYBPT10

CPYBPT20 LDD    BPCNTLA         ; LIQUID ADD. %
        JSR     BCDHEX2
        STD     MBLAPCT
CPYBPTMX:
        RTS


;* COPY HOPPER Wt's INTO MODBUS TABLE PROD. SUMMMARY DATA AREA
CPYHOPPERSETWTMB:
        LDAB   #RAMBANK
        TBZK                    ;~;SET EK FOR BANK 1, I.E. RAM
        LDZ     #MB1KGH         ; MODBUS TABLE, CH #1 HOPPER WT
        LDAB    #1              ; BLEND #, START AT 1
REPCPY  PSHB
        LDX     #CMP1TARWGT     ; ADDRESS OF COMP #N WEIGHT
        DECB
        ABX
        ABX
        ABX
        JSR     BCDHEX3X        ; CONVERT 3 BYTES @X
        CLRA
        STD     0,Z
        STE     2,Z
        AIZ     #MBPSBKSIZ       ; INC. ON TO NEXT CH. MB PROD SUMM BLOCK
        PULB
        INCB


        LDAA    NOBLNDS
        CMPA    #TEN
        BLS     CMPNTW
        CMPB    #TEN
        BLS     REPCPY

        LDX     #CMP11TARWGT    ;
        JSR     BCDHEX3X        ;
        CLRA
        STD     MB11KGH
        STE     MB11KGH+2

        LDX     #CMP12TARWGT    ;
        JSR     BCDHEX3X        ;
        CLRA
        STD     MB12KGH
        STE     MB12KGH+2
        BRA     CMPNTWX

CMPNTW  CMPB    NOBLNDS
        BLS     REPCPY
CMPNTWX RTS


;* COPY HOPPER Wt's INTO MODBUS TABLE PROD. SUMMMARY DATA AREA
CPYHWTSMB:
        TST     PROTOCOL
        BPL     CPYHWTSMX
        LDAB   #RAMBANK
        TBZK                    ;~;SET EK FOR BANK 1, I.E. RAM
        LDZ     #MB1HWT         ; MODBUS TABLE, CH #1 HOPPER WT
        LDAB    #1              ; BLEND #, START AT 1
CPYHWTS10 PSHB
        LDX     #CMP1ACTWGT     ; ADDRESS OF COMP #N WEIGHT
        DECB
        ABX
        ABX
        ABX
        JSR     BCDHEX3X        ; CONVERT 3 BYTES @X
        CLRA
        STD     0,Z
        STE     2,Z
        AIZ     #MBPSBKSIZ       ; INC. ON TO NEXT CH. MB PROD SUMM BLOCK
        PULB
        INCB
        LDAA    NOBLNDS
        CMPA    #TEN
        BLS     CMPNOB
        CMPB    #TEN
        BLS     CPYHWTS10
        LDX     #CMP11ACTWGT
        JSR     BCDHEX3X        ;
        CLRA
        STD     MB11HWT
        STE     MB11HWT+2

        LDX     #CMP12ACTWGT
        JSR     BCDHEX3X        ;
        CLRA
        STD     MB12HWT
        STE     MB12HWT+2
        BRA     CPYHWTSMX

CMPNOB  CMPB    NOBLNDS
        BLS     CPYHWTS10
CPYHWTSMX:
        RTS

;* COPY FRONT ROLL ACCUMULATED WT.S INTO MODBUS TABLE PROD. SUMMMARY
CPYFWTSMB:
        TST     PROTOCOL
        BPL     CPYFWTSMX

        LDAB   #RAMBANK
        TBZK                    ;~;SET EK FOR BANK 1, I.E. RAM
        LDZ     #MB1FRW         ; MODBUS TABLE, CH #1 FRONT ROLL WT
        LDAB    #1              ; BLEND #, START AT 1

CPYFWTS10 PSHB
        LDX     #CH1WACCF
        DECB
        LDAA    #5              ; 5 BYTES/ACCUMULATOR
        MUL
        ABX                     ; X NOW POINTS TO THIS CHANNELS FRONT ROLL ACC.
        JSR     BCDHEX3X        ; CONVERT 3 BYTES @X
        CLRA
        STD     0,Z
        STE     2,Z

        AIZ     #MBPSBKSIZ       ; INC. ON TO NEXT CH. MB PROD SUMM BLOCK
        PULB
        INCB


        LDAA    NOBLNDS
        CMPA    #TEN
        BLS     CMPNFW
        CMPB    #TEN
        BLS     CPYFWTS10

        LDX     #CH11WACCF
        JSR     BCDHEX3X        ; CONVERT 3 BYTES @X
        CLRA
        STD     MB11FRW
        STE     MB11FRW+2

        LDX     #CH12WACCF
        JSR     BCDHEX3X        ; CONVERT 3 BYTES @X
        CLRA
        STD     MB12FRW
        STE     MB12FRW+2
        BRA     CONRWC

CMPNFW  CMPB    NOBLNDS
        BLS     CPYFWTS10


CONRWC  TST     AVECPIB         ; IS THERE PIB ASSOCIATED?
        BEQ     CPYFWTSMX       ; NO, THEN SKIP

        LDX     #LIQWACCF       ; LIQUID WT. ACCUMULATOR
        JSR     BCDHEX3X
        CLRA
        STD     MBLFRW+0
        STE     MBLFRW+2

CPYFWTSMX:
        RTS
; COPY BACK ROLL ACCUMULATED WT.S INTO MODBUS TABLE PROD. SUMMMARY
CPYBWTSMB:
        TST     PROTOCOL
        LBPL     CPYBWTSMX

        TST     SHTTUBE
        BEQ     CPYBWTSMX       ; IF TUBE (00) THEN SKIP BACK ROLL WTS

        LDAB   #RAMBANK
        TBZK                    ;~;SET EK FOR BANK 1, I.E. RAM
        LDZ     #MB1BRW         ; MODBUS TABLE, CH #1 BACK ROLL WT
        LDAB    #1              ; BLEND #, START AT 1

CPYBWTS10 PSHB
        LDX     #CH1WACCB
        DECB
        LDAA    #5              ; 5 BYTES/ACCUMULATOR
        MUL
        ABX                     ; X NOW POINTS TO THIS CHANNELS FRONT ROLL ACC.
        JSR     BCDHEX3X        ; CONVERT 3 BYTES @X
        CLRA
        STD     0,Z
        STE     2,Z

        AIZ     #MBPSBKSIZ       ; INC. ON TO NEXT CH. MB PROD SUMM BLOCK
        PULB
        INCB
        CMPB    NOBLNDS
        BLS     CPYBWTS10


        LDAA    NOBLNDS
        CMPA    #TEN
        BLS     CMPBRW
        CMPB    #TEN
        BLS     CPYBWTS10

        LDX     #CH11WACCB
        JSR     BCDHEX3X        ; CONVERT 3 BYTES @X
        CLRA
        STD     MB11FRW
        STE     MB11FRW+2

        LDX     #CH12WACCB
        JSR     BCDHEX3X        ; CONVERT 3 BYTES @X
        CLRA
        STD     MB12BRW
        STE     MB12BRW+2
        BRA     CONBRW

CMPBRW  CMPB    NOBLNDS
        BLS     CPYBWTS10


CONBRW  TST     AVECPIB         ; IS THERE PIB ASSOCIATED?
        BEQ     CPYBWTSMX       ; NO, THEN SKIP


        LDX     #LIQWACCB       ; LIQUID WT. ACCUMULATOR back roll
        JSR     BCDHEX3X
        CLRA
        STD     MBLBRW+0
        STE     MBLBRW+2

CPYBWTSMX:
        RTS

; COPY ORDER ACCUMULATED WT.S INTO MODBUS TABLE PROD. SUMMMARY
CPYOWTSMB:
        TST     PROTOCOL
        BPL     CPYOWTSMX

        LDAB   #RAMBANK
        TBZK                    ;~;SET EK FOR BANK 1, I.E. RAM
        LDZ     #MB1ORDW        ; MODBUS TABLE, CH #1 ORDER WT
        LDAB    #1              ; BLEND #, START AT 1

CPYOWTS10 PSHB
        LDX     #CH1WACCO
        DECB
        LDAA    #5              ; 5 BYTES/ACCUMULATOR
        MUL
        ABX                     ; X NOW POINTS TO THIS CHANNELS ORDER ACC.
        JSR     BCDHEX4X        ; CONVERT 3 BYTES @X
        STD     0,Z
        STE     2,Z

        AIZ     #MBPSBKSIZ       ; INC. ON TO NEXT CH. MB PROD SUMM BLOCK
        PULB
        INCB


        LDAA    NOBLNDS
        CMPA    #TEN
        BLS     CMPNOW
        CMPB    #TEN
        BLS     CPYOWTS10

        LDX     #CH11WACCO
        JSR     BCDHEX4X        ;
        STD     MB11ORDW
        STE     MB11ORDW+2

        LDX     #CH12WACCO
        JSR     BCDHEX4X        ;
        STD     MB12ORDW
        STE     MB12ORDW+2
        BRA     CONORW

CMPNOW  CMPB    NOBLNDS
        BLS     CPYOWTS10

CONORW  TST     AVECPIB         ; IS THERE PIB ASSOCIATED?
        BEQ     CPYOWTSMX       ; NO, THEN SKIP

        LDX     #LIQWACCO       ; LIQUID WT. ACCUMULATOR back roll
        JSR     BCDHEX3X
        CLRA
        STD     MBLORDW+0
        STE     MBLORDW+2

CPYOWTSMX:
        RTS

;;
;* LDMBWCS       - LOADS Wt CONSTS (C1 & TARE) FROM MBTABLE INTO CALIB. AREA
LDMBWCS:
        LDAA   MANATO
        BEQ    LDMBWC04          ;~BRANCH IF IN MANUAL
        CLR    MANATO           ;~ELSE SET FOR MANUAL MODE
        JSR    EXCTRL           ;~EXIT AUTO CONTROL IF IT WAS IN AUTO
LDMBWC04:
        TST     NOBLNDS         ; NO. OF BLENDS/COMPONENTS ON MACHINE
        LBEQ    LDMBWC80        ; NOTE 0 IS ILLEGAL NO.

        LDD     MB1CALC         ;
        CPD    #2                ; 1 FOR TARE.
        BNE     LDMBWC80
        JSR     TAREWEIGHHOPPER  ; TARE WITH NO DISPLAY
        LDD     #TARECOMMANDBITPOS
        COMD
        ANDD    MB1CALC
        STD     MB1CALC
        LDAA    PGMNUM   ;~
        CMPA    #WEIGHCALIBRATIONPAGEID ;~ON WEIGHT CALIBRATION PAGE
        BNE     NOTWTCLP                ; NO
        JSR     CALBT1                  ; UPDATE CALIBRATION PAGE
NOTWTCLP JSR     CPYWTCMB        ; WT. CONTSANTS
        LDAA   #$AA
        STAA   CALFLG   ;~SET 'CAL DATA CHANGE' FLAG
        STAA   CEEFLG1           ; SET FLAG -> AA TO STORE TO EEPROM.
LDMBWC80:
        RTS
;
;*  - LOAD MEAS. & CONTROL PARAMETERS FROM MODBUS TABLE
LDMBMCPS:
        LDAA   MANATO
        BEQ    LDMBMCP12          ;~BRANCH IF IN MANUAL
        CLR    MANATO           ;~ELSE SET FOR MANUAL MODE
        JSR    EXCTRL           ;~EXIT AUTO CONTROL IF IT WAS IN AUTO
LDMBMCP12:

        LDD     MBAF1           ; MODBUS TABLE, AVG. FACTOR 1
        STAB    CONTAVGFACTOR          ; NOTE MAX EXPECTED IS 10
        JSR     HEXBCD1
        STAB    CONTAVGFACTORDS          ; MUST ALSO STORE DECIMAL VERSION

        LDD     MBAF2           ; MB TABLE AVG. FACTOR 2
        STAB    WGTAVGFACTOR          ; AVERAGING FACTOR 2 (MAX = 74)
        JSR     HEXBCD1
        STAB    WGTAVGFACTORDS          ; MUST ALSO STORE DECIMAL VERSION


        LDD    MBBATINKGHAVG
        CLRA
        JSR    HEXBCD1
        STAB   BATINKGHAVG

        LDD    MBMIXTIM
        CLRA
        JSR    HEXBCD1
        STAB   MIXTIM

        LDD     MBCG            ; MODBUS TABLE CONTROL GAIN (MAX = 99 DEC)
        JSR     HEXBCD1         ; CONVERT BYTE IN ACC B
        STAB    CLGAIN          ; CONTROL GAIN


        CLRW     DACPKGDONEFLAG        ; RESET THIS FLAG TO ENSURE NO VOLSET CORRECTION.        ;
        STAA   CEEFLG1           ; SET FLAG -> AA TO STORE TO EEPROM.

        RTS
;
;* LDMBHPCS       - LOADS HOPPER CONSTS (FEN,FTIM,FTW,MHW,MTPT) FROM MBTABLE
;                   INTO CALIB. AREA
LDMBHPCS:
        LDD     MB1FTW
        JSR     HEXBCD2         ; CONVERT TO BCD
        STE     DFTW1           ; STORE IN CALIB DATA

        LDD     MB1MXTP         ; MAX THRUPUT (Kg/H)
        JSR     HEXBCD2         ; CONVERT TO HEX
        STE     MXTPT1            ; STORE IN CALIB. DATA
LDMBHC30:

        CLR    FTWFLG           ;~FLAG NEW TARGET WTS
        CLR    MHWFLG           ;~FLAG NEW MIN HOP WTS
        LDAA   #$AA
        STAA   MXTPFG           ;~FLAG NEW MAX THRU-PUTS
        STAA   CEEFLG1          ; SET FLAG TO STORE DATA TO EEPROM
LDMBHC80:
        RTS
;
;* LOAD LINE & SCREW SPEED CONSTS FROM MODBUS TABLE TO CALIBRATION DATA AREA
LDMBSCS:
        LDAA   MANATO
        BEQ    LDMBSC04          ;~BRANCH IF IN MANUAL
        CLR    MANATO           ;~ELSE SET FOR MANUAL MODE
        JSR    EXCTRL           ;~EXIT AUTO CONTROL IF IT WAS IN AUTO
LDMBSC04:
        LDD     MBLSC1          ; NOTE: MAX VALUE EXPECTED IS 65535
        JSR     HEXBCD2         ; CONVERT 2 BYTES TO BCD
        STAB    LSC1
        STE     LSC1+1          ; LINE SPEED CONST.

        LDD     MBSSC1          ; SCREW SPEED CONST. IN MB TABLE
        JSR     HEXBCD2
        STE     SSCON1          ; SCREW SPEED CONST(2 BYTES)

        LDD     MBSSMAX         ; MB MAX SCREW SPEED
        JSR     HEXBCD2         ; CONVERT TO BCD
        CPE     #$0999          ; CHECK IF >999
        BLS     MAXSOK          ; OK
        LDE     #$0999
        STE     MAXEXSP
        LDE     #999
        STE     MBSSMAX         ; COPY BACK TO TABLE.
        BRA     CONLS1

MAXSOK  STE     MAXEXSP
CONLS1  LDAA     #$AA
        STAA    NMAXSS
        LDD     MBSSTARE        ; SS TARE IN MB TABLE
        JSR     HEXBCD2
        STE     SSTARE          ; SCREW SPEED TARE CONSTANT

        LDAA    BLENDERMODE
        CMPA    #KGHCONTROLMODE  ; KG/HR MODE
        BEQ    LDMBSC76
        LDAA   #$AA
        STAA   CSAFLG           ;~SET FLAG TO CALC. NEW KG/H SETPOINT
LDMBSC76:
        LDAA   #$AA
        STAA   CEEFLG1           ; SET FLAG TO HAVE DATA -> EEPROM
        RTS
;
;* LOAD SYSTEM CONFIG. PARAMETERS FROM MODBUS TABLE -> CALIBRATION DATA AREA
LDMBSYC:
        LDD    MBNOBDS          ; SYSTEM CONFIGURATION INFO
        LBEQ   RELOADMB       ; RELOAD MOBUS AND EXIT.
        LDAA   MANATO
        BEQ    LDMBSYC04          ;~BRANCH IF IN MANUAL
        CLR    MANATO           ;~ELSE SET FOR MANUAL MODE
        JSR    EXCTRL           ;~EXIT AUTO CONTROL IF IT WAS IN AUTO
LDMBSYC04:
        LDD     MBLINENO        ; LINE NO.
        JSR     HEXBCD2
        STE     LINENO          ; NOTE; MAX VALUE IS 9999

        LDD     MBNOBDS         ; MODBUS TABLE, NO. OF BLENDS
        STAB    NOBLNDS         ; NOTE, EXPECTING 8 MAX
        JSR     CHECKPRODSETE100P ; DO 100% CHECK INCASE THIS HAS CHANGED.



        LDD     MBFILLRETRY     ; FILL RETRYS
        STAB    FILLRETRY       ;

        LDD     MBBLENDERMODE   ; READ BLENDER MODE.
        STAB    BLENDERMODE     ; MODE OF BLENDER OPERATION.

        LDD     MBCRLMOD
        STAB    CRLMODE         ; CONTROL MODE (EXTRUDER/PROP./HAULOFF)

        LDD     MBREGCOMP      ; REGRIND COMPONENT NO.
        JSR     HEXBCD1         ; TO BCD
        STAB    REGCOMP         ; REGRIND COMPONENT.

        LDD     MBFSTCTRL       ; FAST RESPONSE.
        STAB    FSTCTRL         ; FAST CONTROL RESPONSE.

        LDD     MBDIAGMODE      ; DIAGNOSTICS.
        STAB    DIAGMODE        ; DIAGNOSTIC MODE.

        LDD     MBVACLOAD       ; VACUUM LOADING.
        STAB    VACLOAD         ; VACUUM LOADING.

;        LDD     MBPURGEOPTION       ; BYPASS
;        STAB    PURGEOPTION         ; STORE

        LDD     MBISVOLMODE       ;
        STAB    ISVOLMODE         ; VOLUMETRIC MODE.

        LDD     MBEIOMITTED       ; SEI OMITTED ?
        STAB    EIOMITTED

        LDD     MBAVECPIB         ;
        STAB    AVECPIB           ; PIB

        LDD     MBBLENDERTYPE     ; BLENDER TYPE
        STAB    BLENDERTYPE       ;

        LDD     MBSHUTDOWNEN      ; BLENDER SHUTDOWN ENABLE 1 = ENABLED
        STAB    SHUTDOWNEN        ;

        LDD     MBSHUTDOWNTM      ; SHUT DOWN TIME MINS.
        JSR     HEXBCD1           ;
        STAB    SHUTDOWNTM      ;

        LDD     MBUNITTYPE
        STAB    UNITTYPE        ; UNITS TYPE

        LDD     MBEXSEN
        BEQ     LDMBSY30
        LDAB    #$AA
LDMBSY30:
        STAB    EXSENAB         ; EXTR. STALL CHECK ENABLE FLAG

        LDD     MBSSSTYPE
        STAB    SSSTYPE         ; SCREW SPEED SIGNAL TYPE (TACHO/COMMS)

        LDD     MBLSDELAY
        JSR     HEXBCD1         ; CONVERT TO BCD
        STAB    LSDELAY         ; DELAY TO LEVEL SENSOR ALARM.

        LDD     MBOFFLINETYPE
        JSR     HEXBCD1         ; CONVERT TO BCD
        STAB    OFFLINETYPE

        LDD     MBHILOREGENABLED; MODBUS TABLE CONTROL GAIN (MAX = 99 DEC)
        CLRA
        JSR     HEXBCD1         ; CONVERT BYTE IN ACC B
        STAB    HILOREGENABLED  ; CONTROL GAIN

; PURGE COMP NO

        LDD     MBPURGECOMPNO;
        CLRA
        JSR     HEXBCD1         ;
        STAB    PURGECOMPNO  ;

; CONTROL TYPE
        LDD     MBCONTROLTYPE;
        CLRA
        JSR     HEXBCD1         ;
        STAB    CONTROLTYPE     ;

; CONTROL LAG

        LDD     MBCONTROLLAG    ; .
        JSR     HEXBCD2
        STE     CONTROLLAG          ;

; INCREASE RATE

        LDD     MBINCREASERATE        ; .
        JSR     HEXBCD2
        STE     INCREASERATE          ;

; DECREASE RATE

        LDD     MBDECREASERATE        ; .
        JSR     HEXBCD2
        STE     DECREASERATE          ;

; DEADBAND

        LDD     MBDEADBAND            ; .
        JSR     HEXBCD2
        STE     DEADBAND              ;

; FINE INCREASE / DECREASE

        LDD     FINEINCDEC
        JSR     HEXBCD2
        STE     MBFINEINCDEC              ;

; CONTROL %

        LDD     MBCONTROLPER;
        CLRA
        JSR     HEXBCD1         ;
        STAB    CONTROLPER      ;


; MAX COMPONENT %

        LDD     MBMAXPCNT1
        JSR     HEXBCD2
        STE     MAXPCNT1              ;

        LDD     MBMAXPCNT2
        JSR     HEXBCD2
        STE     MAXPCNT2              ;

        LDD     MBMAXPCNT3
        JSR     HEXBCD2
        STE     MAXPCNT3              ;

        LDD     MBMAXPCNT4
        JSR     HEXBCD2
        STE     MAXPCNT4              ;

        LDD     MBMAXPCNT5
        JSR     HEXBCD2
        STE     MAXPCNT5              ;

        LDD     MBMAXPCNT6
        JSR     HEXBCD2
        STE     MAXPCNT6              ;

        LDD     MBMAXPCNT7
        JSR     HEXBCD2
        STE     MAXPCNT7              ;

        LDD     MBMAXPCNT8
        JSR     HEXBCD2
        STE     MAXPCNT8              ;

        LDD     MBMAXPCNT9
        JSR     HEXBCD2
        STE     MAXPCNT9              ;

        LDD     MBMAXPCNT10
        JSR     HEXBCD2
        STE     MAXPCNT10             ;

        LDD     MBMAXPCNT11
        JSR     HEXBCD2
        STE     MAXPCNT11             ;

        LDD     MBMAXPCNT12
        JSR     HEXBCD2
        STE     MAXPCNT12             ;

; MULTIPLE STAGE FILLING

        LDD     MBSTAGEFILLEN
        CLRA
        JSR     HEXBCD1         ;
        STAB    STAGEFILLEN     ;

; MOULDING

        LDD     MBMOULDING
        CLRA
        JSR     HEXBCD1         ;
        STAB    MOULDING        ;

; MONITOR FACTOR

        LDD     MBMONITORFACTOR
        CLRA
        JSR     HEXBCD1         ;
        STAB    MONITORFACTOR   ;


; BLENDER CLEAN ON TIME

        LDD     MBPH1ONTIME
        CLRA
        JSR     HEXBCD1         ;
        STAB    PH1ONTIME           ;

; BLENDER CLEAN OFF TIME

        LDD     MBPH1OFFTIME
        CLRA
        JSR     HEXBCD1         ;
        STAB    PH1OFFTIME           ;

; MIXER CLEAN ON TIME

        LDD     MBPH2ONTIME
        CLRA
        JSR     HEXBCD1         ;
        STAB    PH2ONTIME           ;

; MIXER CLEAN OFF TIME

        LDD     MBPH2OFFTIME
        CLRA
        JSR     HEXBCD1         ;
        STAB    PH2OFFTIME           ;

; BLENDER CLEAN REPS

        LDD     MBBCLEANREPETITIONS
        CLRA
        JSR     HEXBCD1         ;
        STAB    BCLEANREPETITIONS           ;

; MIXER CLEAN REPS

        LDD     MBMCLEANREPETITIONS
        CLRA
        JSR     HEXBCD1         ;
        STAB    MCLEANREPETITIONS           ;

; BYPASS TIME LIMIT

        LDD     MBBYPASSTIMELIMIT
        CLRA
        JSR     HEXBCD1         ;
        STAB    BYPASSTIMELIMIT

                   ;
; TOP UP % ALLOWED

        LDD     MBPERTOPUP
        CLRA
        JSR     HEXBCD1         ;
        STAB    PERTOPUP

; VACUUM DELAY

        LDD     MBVACUUMDELAY
        CLRA
        JSR     HEXBCD1         ;
        STAB    VACUUMDELAY

; VACUUM POST DELAY

        LDD     MBVACUUMPOSTDELAY
        CLRA
        JSR     HEXBCD1         ;
        STAB    VACUUMPOSTDELAY

; MAX GRAMS PER %

        LDD     MBMAXGRAMSPERPER
        JSR     HEXBCD2
        STE     MAXGRAMSPERPER

; BYPASS DELAY TIME DURING CLEANING

        LDD     MBBYPASSDCLNTIMELIMIT
        CLRA
        JSR     HEXBCD1         ;
        STAB    BYPASSDCLNTIMELIMIT

; TOP UP COMPONENT REFERENCE

        LDD     MBTOPUPCOMPNOREF
        CLRA
        JSR     HEXBCD1         ;
        STAB    TOPUPCOMPNOREF

; INTERMITTENT MIX TIME
;
        LDD     MBINTMIXTIME
        CLRA
        JSR     HEXBCD1         ;
        STAB    INTMIXTIME

; PASSWORDS

        LDD     MBPASWRD             ;
        JSR     HEXBCD2
        STE     PASWRD

        LDD     MBPASWRD2
        JSR     HEXBCD2
        STE     PASWRD2             ;

; MAXIMUM THROUGHPUT

        LDD     MBMAXTHRUPUT              ;
        JSR     HEXBCD2
        STE     MXTPT1


; OFFLINE MIXING TIME

        LDD     MBOFFLINEMTIME
        CLRA
        JSR     HEXBCD1         ;
        STAB    MIXTIM

; MIXING MOTOR SHUT OFF TIME

        LDD     MBMIXINGSHUTOFF
        CLRA
        JSR     HEXBCD1         ;
        STAB    MIXINGSHUTOFF


;MBOPTIONS

        LDAB    #RAMBANK
        TBYK                            ; SET UP YK

; HI LO REGRIND

        LDD     #HILOREGENABLEDBITPOS   ; .
        LDX     #HILOREGENABLED
        LDY     #MBOPTIONS
        JSR     SETCLRBITPOS

; MAX % CHECK

        LDD     #MAXPCNTCHECKBITPOS   ; .
        LDX     #MAXPCNTCHECK
        LDY     #MBOPTIONS
        JSR     SETCLRBITPOS

; 1ST COMPONENT COMPENTATION

        LDD     #FIRSTCOMPCOMPBITPOS   ; .
        LDX     #FIRSTCOMPCOMPENDIS
        LDY     #MBOPTIONS
        JSR     SETCLRBITPOS

; LAYERING
        LDD     #LAYERINGBITPOS
        LDX     #LAYERING
        LDY     #MBOPTIONS
        JSR     SETCLRBITPOS

; MIXING SCREW DETECTION

        LDD     #MIXSCREWDETBITPOS
        LDX     #MIXSCREWDETECTION
        LDY     #MBOPTIONS
        JSR     SETCLRBITPOS

; DOUBLE DUMP

        LDD     #DOUBLEDUMPBITPOS
        LDX     #DOUBLEDUMP
        LDY     #MBOPTIONS
        JSR     SETCLRBITPOS

; REGRIND RETRY

        LDD     #REGRINDRETRYBITPOS
        LDX     #REGRINDRETRY
        LDY     #MBOPTIONS
        JSR     SETCLRBITPOS

; LEVEL MONITORING

        LDD     #LEVELMONITORBITPOS
        LDX     #LEVELMONITOR
        LDY     #MBOPTIONS
        JSR     SETCLRBITPOS

; BYPASS

        LDD     #BYPASSMODEBITPOS
        LDX     #BYPASSMODE
        LDY     #MBOPTIONS
        JSR     SETCLRBITPOS

; MANUAL FILL SORTING

        LDD     #MANUALFILLSORTBITPOS
        LDX     #MANUALFILLSORT
        LDY     #MBOPTIONS
        JSR     SETCLRBITPOS

; TOPUP

        LDD     #TOPUPBITPOS
        LDX     #TOPUP
        LDY     #MBOPTIONS
        JSR     SETCLRBITPOS


; BLOW EVERY CYCLE

        LDD     #BLOWEVERYCYCLEBITPOS
        LDX     #BLOWEVERYCYCLE
        LDY     #MBOPTIONS
        JSR     SETCLRBITPOS

; CLEANING

        LDD     #CLEANINGBITPOS
        LDX     #CLEANING
        LDY     #MBOPTIONS
        JSR     SETCLRBITPOS

; MODBUS SINGLE WORD WRITE

        LDD     #SINGLEWORDWRITEBITPOS
        LDX     #SINGLEWORDWRITE
        LDY     #MBOPTIONS
        JSR     SETCLRBITPOS

; MODBUS FAST / SLOW

        LDD     #FASTSLOWBITPOS
        LDX     #FASTFLG
        LDY     #MBOPTIONS
        JSR     SETCLRBITPOS


;  PURGE OPTION
;
        LDD     #PURGEBITPOS
        LDX     #PURGEOPTION
        LDY     #MBOPTIONS
        JSR     SETCLRBITPOS


; USE REGRIND AS REFERENCE

        LDD     #USEREGRINDASREFBITPOS
        LDX     #USEREGRINDASREF
        LDY     #MBOPTIONS2
        JSR     SETCLRBITPOS



; DIAGNOSTICS PRINTING

        LDD     #DIAGPENABBPOS
        LDX     #DIAGPENAB
        LDY     #MBPRINTOPTIONS
        JSR     SETCLRBITPOS


; MONTHLY PRINTING

        LDD     #MTHPENABBITPOS
        LDX     #MTHPENAB
        LDY     #MBPRINTOPTIONS
        JSR     SETCLRBITPOS

; HOURLY PRINTING

        LDD     #HRPENABBITPOS
        LDX     #HRPENAB
        LDY     #MBPRINTOPTIONS
        JSR     SETCLRBITPOS

; SHIFT REPORT PRINTING

        LDD     #SHFPENABBITPOS
        LDX     #SHFPENAB
        LDY     #MBPRINTOPTIONS
        JSR     SETCLRBITPOS


; ALARM REPORT PRINTING

        LDD     #ALPENABBITPOS
        LDX     #ALPENAB
        LDY     #MBPRINTOPTIONS
        JSR     SETCLRBITPOS


; ROLL PRINTING

        LDD     #RRPENABBITPOS
        LDX     #RRPENAB
        LDY     #MBPRINTOPTIONS
        JSR     SETCLRBITPOS

; ORDER REPORT PRINTING


        LDD     #ORPENABBITPOS
        LDX     #ORPENAB
        LDY     #MBPRINTOPTIONS
        JSR     SETCLRBITPOS


; SERIAL / PARALLEL PRINTING


        LDD     #SERPTROPBITPOS
        LDX     #SERPTROP
        LDY     #MBPRINTOPTIONS
        JSR     SETCLRBITPOS

; SERIAL / PARALLEL PRINTING


        LDD     #SCPENABBITPOS
        LDX     #SCPENAB
        LDY     #MBPRINTOPTIONS
        JSR     SETCLRBITPOS

        LDD     #CORRECTEVERYCYCLEBITPOS
        LDX     #CORRECTEVERYNCYCLES
        LDY     #MBMACOPTIONS
        JSR     SETCLRBITPOS

        LDAA    #$AA
        STAA    CEEFLG1          ; SET FLAG TO SAVE TO EEPROM
        RTS
RELOADMB:
        JSR     CPYSYCMB        ; COPY SYSTEM CONFIG DATA.
        RTS.


SETCLRBITPOS:
        ANDD     0,Y             ; READ DATA
        BEQ     CLRWD            ; OPTION CLEARED
        LDAA    #1
        STAA    0,X
        BRA     XTSC            ;
CLRWD   CLR     0,X              ; RESET FLAG
XTSC    RTS





;* LOAD LIQUID CONSTS FROM MODBUS TABLE INTO CALIBRATION DATA AREA
LDMBLIQC:
        LDAA   MANATO
        BEQ    LDMBLQ04          ;~BRANCH IF IN MANUAL
        CLR    MANATO           ;~ELSE SET FOR MANUAL MODE
        JSR    EXCTRL           ;~EXIT AUTO CONTROL IF IT WAS IN AUTO
LDMBLQ04:
        LDD     MBLCON1
        JSR     HEXBCD2
        STAB    LAPCON
        STE     LAPCON+1

        LDD     MBLTARE         ; TARE COUNTS (FLOW RATE)
        JSR     HEXBCD2
        STAB    LAATARE
        STE     LAATARE+1

        LDD     MBLCLAG         ; CONTROL LAG IN MB TABLE
        JSR     HEXBCD1         ; MAX EXPECTED IS 99 SECS
        STAB    LACLAG

        LDD     MBLCG           ; CONTROL GAIN IN MB TABLE
        JSR     HEXBCD1         ; CONVERT TO BCD
        STAB    LACG            ; CONTROL GAIN

        LDD     MBLSTYP
        STAB    LASTYPE         ; FLOW SIGNAL TYPE

        LDD     MBLMODE
        TSTB
        BEQ     LDMBLQ60
        LDAB    #$AA            ; NOTE: $AA USED IN CALIB DATA TO INDIC. CONTROL
LDMBLQ60:
        STAB    LAMODE          ; MEASUREMENT MODE (MEAS. ONLY/MEAS & CTRL)

        LDAA    #$AA
        STAA    CEEFLG1          ; SET FLAG TO HAVE DATA SAVED TO EEPROM

        RTS

MULTIPLIER EQU   1

; CHECK INCREASE / DECREASE IF IN MANUAL.

MBCHKINCDEC:
       LDAB   MANATO
       CMPB   #$AA              ;~IN AUTO?
       BNE    MBCHKID
       CLR    COMMSIFLG         ;~AUTO, INC/DEC KEY NOT VALID
       CLR    COMMSDFLG
       BRA    XITID

MBCHKID LDD    MBINCDEC
       TSTB                     ; CHECK THE DECREASE BYTE
       BEQ      MBCHINC         ; ALLOFF   ZERO, TURN OFF ALL OUTPUTS
       CLR      COMMSIFLG       ; ENSURE INC FLAG IS TURNED OFF(DEC HAS PRECEDENCE)
;       LDAA     #1
;       CMPB     #$0F            ; SLOW DECR.
;       BEQ      MBCHDEC3
;       LDAA     #10
;       CMPB     #$F0            ; FAST DECR
;       BNE      MBCHDEC5
MBCHDEC3:
         LDAA   #MULTIPLIER
         MUL                    ; A * B
         ADDB   COMMSDFLG
         STAB   COMMSDFLG
MBCHDEC5 BRA    MBTG080           ;CHKERR

MBCHINC TAB                     ; MOVE INC BYTE INTO ACC B
       TSTB                     ;CHECK THE INCREASE BYTE
       BEQ      MBTG080          ;ZERO, DO NOTHING
;       LDAA     #1
;       CMPB     #$0F            ; SLOW INCR.
;       BEQ      MBCHINC3
;       LDAA     #10
;       CMPB     #$F0            ; FAST INCR
;       BNE      MBCHINC5
MBCHINC3 LDAA   #MULTIPLIER
         MUL                    ; A * B
        ADDB   COMMSIFLG
        STAB   COMMSIFLG
MBCHINC5:
XITID  RTS

;
;   TOGGLE STATUS COMMANDS
;
MBTOGSTA LDD    MBAUTMAN           ;~LOAD THE MODBUS AUTO/MAN TOGGLE
        ANDD    #TOGAMBIT          ;~CHECK THE AUTO/MANUAL BIT
        BEQ     MBTG020            ;~ZERO, DO NOTHING
        JSR     AMKEY              ;~RUN THE KEYBOARD HANDLER
        BRA     MBTG080            ; SKIP INC/DEC IF TOGGLING A/M
MBTG020:
        JSR     MBCHKINCDEC        ; INCREASE / DECREASE FOR MODBUS.
MBTG080 LDD     MBAUTMAN           ; CHECK A/M AGAIN.
        ANDD    #OPENOFFLINEBIT    ; OPEN OFFLINE VALVE
        BEQ     MBTG012            ; NO OPEN OFFLINE
        TST     PAUSFLG            ; IS BLENDER PAUSED ?
        BEQ     MBTG012            ; NO
        JSR     OPENFEED           ; OPEN

MBTG012:
        LDD      MBAUTMAN
        BITB     #FRCINITIATEBIT    ; FRONT ROLL CHANGE VIA COMMS
        BEQ      MBTG013
        JSR      FRONTROLLCHANGE


MBTG013:
        LDD      MBAUTMAN
        BITB     #BRCINITIATEBIT    ; Back ROLL CHANGE VIA COMMS
        BEQ      MBTG014
        JSR     BACKROLLCHANGE

MBTG014:
NOOPOF  LDD     MBAUTMAN           ; CHECK A/M AGAIN.
        ANDD    #CLOSEOFFLINEBIT   ; OPEN OFFLINE VALVE
        BEQ     NOCLOF             ; NO OPEN OFFLINE
        TST     PAUSFLG            ; IS BLENDER PAUSED ?
        BEQ     CHKAM              ; NO
        JSR     CLOSEFEED          ; OPEN
        BRA     CHKAM              ; CHECK A/M


NOCLOF  LDD     MBAUTMAN           ; CHECK A/M AGAIN.
        ANDD    #OPENDUMPBIT       ; OPEN DUMP
        BEQ     NOOPDMP            ; NO.
        TST     PAUSFLG
        BEQ     NOPS1
        JSR     OPENDUMP           ;
NOPS1   BRA     CHKAM              ;

NOOPDMP LDD     MBAUTMAN           ; .
        ANDD    #CLOSEDUMPBIT      ; CLOSE DUMP.
        BEQ     NOCLDMP            ; NO.
        TST     PAUSFLG
        BEQ     NOPS2
        JSR     CLOSEDUMP          ;
NOPS2   BRA     CHKAM              ; CHECK A/M


NOCLDMP LDD     MBAUTMAN           ; CHECK A/M AGAIN.
        ANDD    #TOGPAUSEBIT       ; PAUSE.
        BEQ     NOPAUSET           ; NO.
        JSR     PAUSETOGGLE        ; TOGGLE PAUSE.
        BRA     CHKAM              ; CHECK A/M
NOPAUSET:
        LDD    MBAUTMAN           ;~LOAD THE MODBUS AUTO/MAN TOGGLE
        ANDD    #TOGEOCPAUSEON     ; END OF CYCLE PAUSE.
        BEQ     NOEOCPAUSEON           ; NO.
        JSR     ENDOFCYCLEPAUSEON  ; PAUSE ON.
        BRA     CHKAM              ; CHECK A/M
NOEOCPAUSEON:
        LDD    MBAUTMAN           ;~LOAD THE MODBUS AUTO/MAN TOGGLE
        ANDD    #TOGIMDPAUSEON     ; PAUSE IMMEDIATELY.
        BEQ     NOIMDPAUSEON
        JSR     IMDPAUSEON         ; PAUSE IMMEDIATELY.
        BRA     CHKAM              ; CHECK A/M
NOIMDPAUSEON:
        LDD    MBAUTMAN           ;~LOAD THE MODBUS AUTO/MAN TOGGLE
        ANDD    #TOGPAUSEOFF        ; PAUSE IMMEDIATELY.
        BEQ     CHKAM
        JSR     PAUSEOFF           ; PAUSE OFF.
CHKAM   LDD     MBAUTMAN           ; CHECK A/M AGAIN.
        ANDD    #TOGAMONBIT        ; PAUSE.
        BEQ     NOAMON             ; NO.
        JSR     TOAUTO             ; AUTO.
        BRA     MBTG081            ;
NOAMON  LDD     MBAUTMAN           ; CHECK A/M AGAIN.
        ANDD    #TOGAMOFFBIT
        BEQ     NOAMOFF            ; NO.
        JSR     TOMANUAL           ; SWITCH TO MANUAL.

; CHECK CLEARING COMMANDS

NOAMOFF LDD     MBCLRCMD
        ANDD    #MBCLEARALARMBIT     ; $01 ~CLEAR ALARMS BIT
        BEQ     MBTG081
        JSR     INALTB             ;~INIT. ALARM TABLE, I.E. CLEAR ALARMS
MBTG081:
        LDD     MBCLRCMD
        BITB    #MBCLEARENTERKEYBIT  ; $02 CLEAR ENTER KEY PRESSED FLAG BIT??
        BEQ     MBTG082
        CLR     EKPRSFLG        ; NON-ZERO, => CLEAR FLAG
        LDD     MBNEWDATA       ;
        ANDD    #$FFF8          ; CLEAR NEW SPT & CALIB & K's FLAG bits (B2,B1&B0)
        STD     MBNEWDATA
MBTG082:
        LDD     MBCLRCMD
        BITB    #MBCLEARFRONTRCBIT ;  $04 CLEAR FRCO FLAG BIT??
        BEQ     MBTG083
        LDD     MBNEWDATA       ;
        ANDD    #$FEFF          ; CLEAR FRCO FLAG bit (B8)
        STD     MBNEWDATA
MBTG083:
        LDD     MBCLRCMD
        BITB    #MBCLEARBACKRCBIT ;  $08 CLEAR BRCO FLAG BIT??
        BEQ     MBTG084
        LDD     MBNEWDATA       ;
        ANDD    #$FDFF          ; CLEAR BRCO FLAG bit (B9)
        STD     MBNEWDATA
MBTG084:
        LDD     MBCLRCMD
        BITB    #MBCLEARORDERCHANGEBIT    ;$10            ; CLEAR ORDER CHANGE OCCURRED FLAG BIT??
        BEQ     MBTG085
        LDD     MBNEWDATA       ;
        ANDD    #$FBFF          ; CLEAR OCO FLAG bit (B10)
        STD     MBNEWDATA
MBTG085:
        LDD     MBCLRCMD
        BITB    #MBCLEARRESETOCCBIT       ;$20            ; CLEAR RESET OCCURRED FLAG BIT??
        BEQ     MBTG086
        CLR     RESETFLG         ; CLEAR RESET OCCURRED FLAG (OPSTATUS)
MBTG086:
        LDD     MBCLRCMD
        BITB    #MBCLEARSHIFTWEIGHTBIT    ;$40            ; CLEAR SHIFT WTs
        BEQ     MBTG087
        JSR     CLRSHFTACCS

MBTG087:

        LDD     MBCLRCMD
        BITB    #MBCLRCMDRESETCTRS  ; RESET COMMS COUNTERS
        BEQ     MBTG088
        JSR     RESETDIAGCTR
MBTG088:



;MBTG090 LDD     MBTOGFEN        ; TOGGLE FILL ENABLE's
;        TSTB                    ;~TOGGLE FILL ENABLE BYTE
;        BEQ     MBTG100
;        TBA                     ;~LOAD COMMAND BYTE INTO A
;        JSR     CPYHPCMB        ; HAVE THE MODBUS TABLE UPDATED WITH FILL ENABLES

MBTG100 LDD     MBOPENCMP       ; MB FILL OPEN COMMAND
        BEQ     MBTG121         ; CAN SKIP LONG FILL IF 0000
        JSR     OPENTOGGLE


MBTG121 LDD     MBCLOSECMP      ; CLOSE COMMAND
        BEQ     MBTG122         ;
        JSR     CLOSETOGGLE
MBTG122:
        LDD     MBCLRCMD
        BITB    #$02            ; CLEAR ENTER KEY PRESSED FLAG BIT??
        BEQ     MBTG130
        CLR     EKPRSFLG        ; NON-ZERO, => CLEAR FLAG
        LDD     MBNEWDATA       ;
        ANDD    #$FFF8          ; CLEAR NEW SPT & CALIB & K's FLAG bits (B2,B1&B0)
        STD     MBNEWDATA
MBTG130:
        LDD     MBSC_CMD        ;~SCREW FEEDER CALIBRATION COMMAND
        BEQ     MBTG140
        STAB    NETSCCMD        ;~STORE NETWORK SCREW CALIB. COMMAND
MBTG140:
        TST     MANATO
        BNE     MBTG150         ; DO NOT ACCEPT D/A IF NOT IN MANUAL
        LDD     MBSETD_A        ; EXTRUDER D/A VALUE (WRITTEN TO BLENDER)
        BPL     MBTG150         ; MSB MUST BE SET TO INDICATE NEW VALUE
        ANDD    #$7FFF          ; MASK OFF M.S.BIT
        CPD     DA1TMP          ; IS IT SAME AS CURRENT VALUE
        BEQ     MBTG150         ; IF SO NO ACTION NECESSARY
        STD     DA1TMP          ; WRITE NEW VALUE
        LDAB   #15                ; Set "to stall time" to 15 SECS
        STAB   TOSTALTIM
        CLR    EXTSCTR            ; extruder stalled counter = 0
        INC    MCHGFLG            ;~SET MANUAL CHANGE FLAG
        INC    CCVSFLG
        LDAA    #1
        STAA    RUNEST          ;  MAKE ESTIMATION RUN.

;       CLR    NMANENB            ;~ENABLE NMANUAL
PBNCK9X

MBTG150:
; if L5 fitted (0) then USE flag to load setpoints
        TST     SINGLEWORDWRITE          ;USE LINK L5 FOR HAVING TO SET EXTRA FLAG
        LBEQ    MBTG175         ; NOT FITTED (IGNORE MBNWDCMD)

        LDE     MBNWDCMD        ; NEW DATA COMMAND
        ANDE    #$0007          ; CHECK FOR ANY SETPTS // K's not treated as spt's
        LBEQ    MBTG175
;
        TSTW    MBRSTOTLS       ; RESET ORDER TOTALS FLAG IN MB TABLE
        BNE     MBTG151         ; IF NON-ZERO NO ACTION REQ'D

        JSR     ASSORPT         ; ASSEMBLE ORDER REPORT
        CLRW    MBRSTOTLS       ; CLEAR FLAG
;NOTE MBNEWDATA BIT 10 IS SET IN CPYORRMB

MBTG151:
        LDE     MBNWDCMD        ; NEW DATA COMMAND
        ANDE    #$0007          ; CHECK FOR %, TPT OR OI
        BEQ     MBTG158
;       CLR     LDNSPLAG
        ANDE    #$0001          ; CHECK FOR % WRITE
        BEQ     MBTG152
        COME
        ANDE    MBNWDCMD
        STE     MBNWDCMD
        CLR     LDNSPLAG

        JSR     CHMB100T
        TSTB
        BEQ     LDPDTA          ; LOAD % DATA
        LDD     MBWORFTP
        STD     MBWORFER        ; STORE TEMP 'OR' FLAG IN ERROR FLAG
        BRA     MBTG152
LDPDTA  JSR     LDMBPCTS       ; LOAD %'S FROM MB TABLE

MBTG152:
        LDE     MBNWDCMD        ; NEW DATA COMMAND
        ANDE    #$0002          ; CHECK FOR NEW TPT SETPOINTS
        BEQ     MBTG156
        COME
        ANDE    MBNWDCMD
        STE     MBNWDCMD
        CLR     LDNSPLAG
        JSR     LDMBTPTS       ; LOAD TPT'S FROM MB TABLE
MBTG156:
        LDE     MBNWDCMD        ; NEW DATA COMMAND
        ANDE    #$0004          ; CHECK FOR NEW ORDER INFO SETPOINTS
        BEQ     MBTG158
        COME
        ANDE    MBNWDCMD
        STE     MBNWDCMD
        CLR     LDNSPLAG
        JSR     LDMBOINFO       ; LOAD ORDER INFO FROM MB TABLE
MBTG158:
MBTG159:
        LDAB    #$0F
        STAB    NEWSET          ; SET FLAG FOR NEW SETPOINTS
MBTG175:

MBTG180:
        JSR     LDMBSSPEED      ; IF SO LOAD UP SCREW SPEED
MBTG182:

       LDD      MBPRDIAG
       BEQ      NOPRODD         ; NO PRODUCTION DIAGNOSTICS
       JSR      ACTIVATEPRODDIAGNOSTICS
       CLRW     MBPRDIAG
NOPRODD
        LDD     MBCLEAN           ; .
        ANDD    #MBTOGSTANORMALCLEANBITPOS   ; START CLEAN CYCLE
        BEQ     NONCL                        ; NO.
        JSR     INITIATECLEANCYCLE ; .
        CLRW    MBCLEAN                      ; CLEAN THIS FOR NOW

NONCL   TST     MBENCRYPTIONCODE
        BNE     ENENCR                      ; ENTER ENCRYPTION.
        TST     MBENCRYPTIONCODE+2
        BEQ     NOENWR
ENENCR  LDX     #MBENCRYPTIONCODE+1
        JSR     HEXBCD3X
        STAB    NKEYBUF
        STE     NKEYBUF+1
        JSR     ENTERENCRYPTIONDATA ;

NOENWR  LDX     #MBOPENCMP
        LDE     #TOGGLESIZE
        JSR     CLREWX          ; CLEAR ALL USED TOG STAT. COMMANDS

       RTS
;

;* MBCHHPCAL - CHECK FOR HOPPER CALIBRATION VIA NETWORK (MODBUS)
;              CALIB. CODE BYTE (MBxCALC) IN  CALIB. BLOCK/COMP.
;              INDICATES HOPPER CALIBRATION IN PROGRESS
;              B0 -> HOPPER WEIGHT (CONST C1) CALIBRATION
;              B1 -> HOPPER AUTO TARE
;              ON EXIT ACC A = 0 IF NO HOPPER CALIB., ELSE SET TO $AA
MBCHHPCAL:
        LDAB    #1              ; COMPONENT #1
        LDAB   #RAMBANK
        TBZK                    ;~;SET EK FOR BANK 1, I.E. RAM
        LDZ     #CALBLK
        LDX     #MB1CALC        ; CALIB. CODE
MBCHHPC02 STAB    NCCHNUM
        LDAA    1,X             ; GET CAL. CODE
        ANDA    #$03
        BNE     MBCHHPC04       ; HAVE FOUND A UNIT REQUIRING CALIB.
        AIX     #MBCBKSIZ       ; MOVE ON TO NEXT COMP's NET CAL BLOCK
        AIZ     #CCBKSZ         ; MOVE ON TO NEXT COMP's  CAL BLOCK
        INCB
        CMPB    NOBLNDS
        BLS     MBCHHPC02       ; REPEAT FOR MORE BLENDS

        LBRA    MBCHHPCNW       ; NO HOPPERS BEING CALIBRATED (NO WRITE)

MBCHHPC04:
        BITA    #$01            ; HOPPER CALIB. CONST 1
        BEQ     CHKHPCTAR       ; NOT C1 => TARE

        AIZ     #CON1OF          ;~CONST. C1 OFFSET
;                               ;~Z NOW POINTS TO CAL CONST (4 BYTES)
        LDX     #MB1CON1
        LDAB    NCCHNUM
        DECB
        LDAA    #MBCBKSIZ       ; BLOCK SIZE FOR MB CALIB. (/COMPONENT)
        MUL
        ADX                     ; NOW X POINTS TO COMP's NET C1

;
       LDD     0,X             ; LOAD MB VERSION OF CON1
       JSR     HEXBCD2         ; CONVERT 2 HEX BYTES TO BCD, RESULT IN B:E
       CLR     0,Z             ;~M.S. BYTE IS 00
       STAB    1,Z             ; MAX VALUE   = 6 55 35 Kg/H
       STE     2,Z

       LBRA   MBCHHPCWC        ; WROTE NEW HOPPER CAL. INFOR

CHKHPCTAR:                      ; AUTO TARE

       CLR    MHWFLG           ;~CAUSE MIN HOPPER WTS TO BE UPDATED

MBCHHPCWC:
        JSR     CLR_CALCS       ; CLEAR ALL CAL CODES TO PREVENT REPETITIVE ..
        JSR     CPYWTCMB        ; COPY WT. CONST.s TO MODBUS TABLE
        LDAB    #50             ; 50 SECS
        STAB    NHOPCFLG        ; SET FLAG TO INDICATE HOPPER CALIB. IN PROG.
        LDAA    #$AA
        STAA    CALIFG          ; SET FLAG TO INDICATE CALIB. MODE (NO SMOOTHING)
        STAA    CEEFLG1          ; SET FLAG TO HAVE DATA STORED TO EEPROM
        BRA     MBCHHPCXX
MBCHHPCNW:                      ; NO DATA WRITE
        CLRA                    ; FLAG NO HOP. CAL. DATA WRITE
MBCHHPCXX:
        RTS

; CLR_CALCS - Clear all modbus component calibration write codes
CLR_CALCS LDX   #MB1CALC        ; CALIB. CODE FOR COMPONENT 1
        LDAB    NOBLNDS         ; NO. OF BLENDS/COMPONENTS
CLRCALC02:
        CLRW    0,X
        AIX     #MBCBKSIZ       ; ADVANCE TO NEXT COMPONENT
        DECB
        BNE     CLRCALC02
        RTS

;* COPY TIME INTO MODBUS TABLE
CPYTIMMB:
        TST     PROTOCOL
        BPL     CPYTIMBX        ; EXIT IF NOT MB PROTOCOL

        LDD     MBWORFLG        ; CHECK IF TIME IS BEING WRITTEN
        ANDD    #$0400
        BNE     CPYTIMBX        ; NO UPDATE UNTIL TIME IS UPDATED.

        LDX     #TIMEBUF
        LDAB    MIN,X
        JSR     BCDHEX1
        PSHB
        LDAB    SEC,X
        JSR     BCDHEX1         ; CONVERT SECs TO HEX
        PULA                    ; RESTORE MINUTES
        STD     MBMINSEC        ; [MIN:SEC]

        LDX     #TIMEBUF
        LDAB    HOUR,X          ; HOURs
        JSR     BCDHEX1
        LDAA    DAY,X           ; WEEKDAY
        STD     MBDAYHOUR       ; [DAY:HOUR]

        LDX     #TIMEBUF
        LDAB    MON,X           ; MONTH
        JSR     BCDHEX1
        PSHB
        LDAB    DATE,X          ; DATE (DAY OF MONTH)
        JSR     BCDHEX1
        PULA                    ; RESTORE DATE IN acc A
        STD     MBMONDATE       ; [MONTH:DATE]

        LDX     #TIMEBUF
        LDAB    YEAR,X          ; YEAR
        LDAA    YEAR_MSB        ; CENTURY - RAM ONLY
        JSR     BCDHEX2         ; CONVERT YEAR e.g. 1996
        STD     MBYEAR
CPYTIMBX:
        RTS
;* LOAD TIME FROM MODBUS TABLE
LDMBTIME:

        LDX     #TIMEBUF
        LDD     MBMINSEC        ; [MIN:SEC]
        PSHA
        JSR     HEXBCD1         ; CONVERT SECS TO HEX
        STAB    SEC,X
        PULB                    ; RESTORE MINUTES
        JSR     HEXBCD1
        STAB    MIN,X

;        LDX     #TIMEBUF
        LDD     MBDAYHOUR       ; [DAY:HOUR]
        STAA    DAY,X           ; DAY DOES NOT NEED CONVERSION (1-7)
        JSR     HEXBCD1         ; CONVERT HOURS TO HEX
        STAB    HOUR,X

;        LDX     #TIMEBUF
        LDD     MBMONDATE       ; [MON:DATE]
        PSHA
        JSR     HEXBCD1         ; CONVERT DATE TO HEX
        STAB    DATE,X
        PULB                    ; RESTORE MONTH
        JSR     HEXBCD1
        STAB    MON,X

        LDD     MBYEAR
        JSR     HEXBCD2         ;
        TED                     ; MOVE RESULT -> REG D
        STAB    YEAR,X          ; YEAR
        STAA    YEAR_MSB        ; RAM ONLY.

       JSR    WRITDATE
       JSR    WRITTIME          ; WRITE TO RTC
       RTS

;* LDMBSSPEED - LOADS DOWNLOADED SCREW SPEED FROM MODBUS TABLE
LDMBSSPEED:
;       CLR     MBWSSFLAG       ; CLEAR FLAG FOR SCREW SPEED DOWNLOAD
        LDAB    SSSTYPE         ; SCREW SPEED SIGNAL TYPE
        CMPB    #1              ; 1 IS COMMs
        BNE     LDMBSSPDX       ; DONT LOAD IF NOT COMMS
        TST     PROTOCOL
        BPL     LDMBSSPDX       ; EXIT ALSO IF NOT MB PROTOCOL

        LDD     MBSSWRIT        ; LOAD MB VERSION (2DP)(SS DOWNLOAD ON COMMS)
        ANDD    #$8000          ; IS MSB SET ?
        BEQ     LDMBSSPDX       ; EXIT IF NOT.
        LDD     MBSSWRIT
        ANDD    #$7FFF          ; CLEAR MSB
        JSR     HEXBCD2         ; CONVERT 2 HEX BYTES TO BCD, RESULT IN B:E
        STAB    SSRPM2D          ; MAX VALUE   = 6 55 35 Kg/H
        STE     SSRPM2D+1       ; STORE BCD VERSION.

        CLR     SSRPM
        STAB    SSRPM+1
        TED
        STAA    SSRPM+2         ; SSRPM IS integer RPM
LDMBSSPDX:
        RTS


;* Clear shift accumulators
CLRSHFTACCS:
        LDX     #TOTWACCS9               ; TOTAL Shift WT. ACC
        LDAB    #5*9                     ; 8 BLENDS + PIB + TOTAL (6 bytes/accum.)
        JSR     CLRBBS                   ; CLEAR
        LDX     #SHFTLTACC               ; SHIFT LENGTH ACCUMULATOR (5BYTES)
        LDAB    #5
        JSR     CLRBBS

        JSR     CPYSHWTSMB              ; COPY SHIFT DATA (ZEROED) TO MB
        JSR     CPYSHFTLMB              ;
        RTS


; LOADS COMPONENT CONFIGURATION FROM MODBUS TABLE.

LDMBCOMPCFG:
        LDD     MBNONSTDCCFG            ; READ CONFIGURATION.
        STAB    STDCCFG              ;
        TSTB
        BEQ     ISSTDCF                 ; IS STANDARD CONFIG.
        LDD     MBCOMP1CFG              ; COMPONENT CONFIG #1
        STD     COMP1CFG                ;
        LDD     MBCOMP2CFG              ; COMPONENT CONFIG #2
        STD     COMP2CFG                ;
        LDD     MBCOMP3CFG              ; COMPONENT CONFIG #3
        STD     COMP3CFG                ;
        LDD     MBCOMP4CFG              ; COMPONENT CONFIG #4
        STD     COMP4CFG                ;
        LDD     MBCOMP5CFG              ; COMPONENT CONFIG #5
        STD     COMP5CFG                ;
        LDD     MBCOMP6CFG              ; COMPONENT CONFIG #6
        STD     COMP6CFG                ;
        LDD     MBCOMP7CFG              ; COMPONENT CONFIG #7
        STD     COMP7CFG                ;
        LDD     MBCOMP8CFG              ;
        STD     COMP8CFG                ;
        LDD     MBCOMP9CFG              ;
        STD     COMP9CFG                ;
        LDD     MBCOMP10CFG              ;
        STD     COMP10CFG                ;
        LDD     MBCOMP11CFG              ;
        STD     COMP11CFG                ;
        LDD     MBCOMP12CFG              ;
        STD     COMP12CFG                ;
ISSTDCF LDAA    #$AA
        STAA    CEEFLG1          ; SET FLAG TO SAVE TO EEPROM
        RTS


; COPY COMPONENT CONFIGURATION

CPYCCFGTMB:
        TST     PROTOCOL
        BPL     CPYCCFGX
        LDAB    STDCCFG            ; READ CONFIGURATION.
        STD     MBNONSTDCCFG
        LDD     COMP1CFG
        STD     MBCOMP1CFG
        LDD     COMP2CFG
        STD     MBCOMP2CFG
        LDD     COMP3CFG
        STD     MBCOMP3CFG
        LDD     COMP4CFG
        STD     MBCOMP4CFG
        LDD     COMP5CFG
        STD     MBCOMP5CFG
        LDD     COMP6CFG
        STD     MBCOMP6CFG
        LDD     COMP7CFG
        STD     MBCOMP7CFG
        LDD     COMP8CFG
        STD     MBCOMP8CFG
        LDD     COMP9CFG
        STD     MBCOMP9CFG
        LDD     COMP10CFG
        STD     MBCOMP10CFG
        LDD     COMP11CFG
        STD     MBCOMP11CFG
        LDD     COMP12CFG
        STD     MBCOMP12CFG
CPYCCFGX RTS



; LOADS THE HOPPER CALIBRATION TO MODBUS TABLE AND INITIATES WEIGHT CALIBRATION.
;
LDMBHOPPERCAL:

       TST      AUTOCYCLEFLAG   ; AUTOCYCLER.
       BNE      LDHPCL          ; CALIBRATE
       TST      STOPPED ;
       LBEQ      XITHPC
LDHPCL LDD      MB1CON1
       JSR      HEXBCD2         ;
       JSR      SHF_BE4L         ; SHIFT 4 PLACES LEFT, (*10), PROD. HAS 2DP
       STAB     NKEYBUF+2
       STE      NKEYBUF+3       ;
       LDAA     #MODBUSWEIGHTCAL
       STAA     CALWGTCON         ; INITITIATE CALIBRATION.
XITHPC RTS

MODBUSWEIGHTCAL EQU     $AB



; COPY BATCH BLENDER AUXILIARY DATA TO MODBUS TABLE.

COPYBBAUXMB:
        TST     PROTOCOL
        LBPL     CPYAUXX
        LDD     #MBSTATSTOPPEDBPOS      ; STOPPED BIT POSITION.
        TST     STOPPED
        BEQ     ISRUN                   ; IS RUNNING.
        ORD     MBOPSTS
        STD     MBOPSTS
        BRA     CONCPA                  ; CONTINUE COPY OF AUX DATA.
ISRUN   COMD
        ANDD    MBOPSTS
        STD     MBOPSTS                 ; STORE OPERATING STATUS.
CONCPA  LDE     #MBSTATPAUSEBPOS        ; PAUSE BIT POSITION.
        LDAA    PAUSFLG                 ;
        ANDA    #PAUSEBITS
        BEQ     NOPAUS
        ORE     MBOPSTS
        STE     MBOPSTS
        BRA     CONCP1
NOPAUS  COME
        ANDE    MBOPSTS
        STE     MBOPSTS         ; STORE OPERATING STATUS.
CONCP1  LDD     #MBSTATMIXONBPOS
        TST     MIXON           ;
        BEQ     MISOFF          ; MIXER IS ON.
        ORD     MBOPSTS
        STD     MBOPSTS
        BRA     CONCP2          ; CONTINUE
MISOFF  COMD
        ANDD    MBOPSTS
        STD     MBOPSTS         ; STORE OPERATING STATUS.
; START UP MODE
CONCP2  LDD     #MBBLENDERSTARTUPMODE
        TST     STRUPF           ;
        BEQ     NOSTU            ; NO START UP.
        ORD     MBOPSTS
        STD     MBOPSTS
        BRA     CONST            ; CONTINUE
NOSTU   COMD
        ANDD    MBOPSTS
        STD     MBOPSTS         ; STORE OPERATING STATUS.
CONST:

        LDAB    #RAMBANK
        TBYK
        LDD     #MBSTATTAREBPOS   ; .
        LDX     #TARINGSTATUS
        LDY     #MBOPSTS2
        JSR     SETCLRBITDEPONX

        LDD     #MBSTATBLENDERCLEANBPOS   ; .
        LDX     #AIRJET1ONF
        LDY     #MBOPSTS2
        JSR     SETCLRBITDEPONX

        LDD     #MBSTATMIXERCLEANBPOS   ; .
        LDX     #AIRJET2ONF
        LDY     #MBOPSTS2
        JSR     SETCLRBITDEPONX

        LDD     #MBSTATRCCBPOS   ; .
        LDX     #CORNERCLEANONF
        LDY     #MBOPSTS2
        JSR     SETCLRBITDEPONX

        LDD     #MBSTATLCCBPOS   ; .
        LDX     #CORNERCLEANRONF
        LDY     #MBOPSTS2
        JSR     SETCLRBITDEPONX

        LDD     #MBSTATCLEANLASTBATCHBPOS   ; .
        LDX     #SETCLEANWHENCYCLEEND
        LDY     #MBOPSTS2
        JSR     SETCLRBITDEPONX


        LDD     #MBSTATCYCLEINDICATE   ; .
        LDX     #CYCLEINDICATE
        LDY     #MBOPSTS2
        JSR     SETCLRBITDEPONX

        LDD     #MBRECIPEWAITINGBITPOS   ; .
        LDX     #RECIPEWAITING
        LDY     #MBOPSTS2
        JSR     SETCLRBITDEPONX


        LDAA    PAUSFLG
        ANDA    #VACINHIBIT
        STAA    TEMPX

        LDD     #MBINHIBITBITPOS   ; .
        LDX     #TEMPX
        LDY     #MBOPSTS2
        JSR     SETCLRBITDEPONX

        LDAB   CLEANCYCLESEQ
        CMPB   #CLEANPHASE1ID
        BNE    NOTPH1
        LDY     #MBOPSTS2
        LDD    #MBBLENDERCLEANBITPOS
        ORD     0,Y
        STD     0,Y
        BRA     CHPH2

NOTPH1  LDD    #MBBLENDERCLEANBITPOS
        COMD
        ANDD    0,Y
        STD     0,Y

CHPH2   LDAB   CLEANCYCLESEQ
        CMPB   #CLEANPHASE2ID
        BNE    NOTPH2

        LDD    #MBMIXERCLEANBITPOS
        ORD     0,Y
        STD     0,Y
        BRA     CONCP

NOTPH2  LDD    #MBMIXERCLEANBITPOS
        COMD
        ANDD    0,Y
        STD     0,Y


CONCP   CLRA
        LDAB    LFILLSTATCMP1    ;
        STD     MBFILLSTATCMP1   ; FILL STATUS.
        LDAB    LFILLSTATCMP2    ;
        STD     MBFILLSTATCMP2   ; .
        LDAB    LFILLSTATCMP3    ;
        STD     MBFILLSTATCMP3   ; .
        LDAB    LFILLSTATCMP4    ;
        STD     MBFILLSTATCMP4   ; FILL STATUS.
        LDAB    LFILLSTATCMP5    ;
        STD     MBFILLSTATCMP5   ; FILL STATUS.
        LDAB    LFILLSTATCMP6    ;
        STD     MBFILLSTATCMP6   ; FILL STATUS.
        LDAB    LFILLSTATCMP7    ;
        STD     MBFILLSTATCMP7   ; FILL STATUS.
        LDAB    LFILLSTATCMP8    ;
        STD     MBFILLSTATCMP8   ; FILL STATUS.
        LDAB    LFILLSTATCMP9    ;
        STD     MBFILLSTATCMP9   ; FILL STATUS.
        LDAB    LFILLSTATCMP10   ;
        STD     MBFILLSTATCMP10  ; FILL STATUS.
        LDAB    LFILLSTATCMP11   ;
        STD     MBFILLSTATCMP11  ; FILL STATUS.
        LDAB    LFILLSTATCMP12   ;
        STD     MBFILLSTATCMP12  ; FILL STATUS.

        LDD     #MBSTATDUMPSTATUSBPOS
        TST     DUMPSTATUS      ;
        BEQ     NODUMP          ; NOT DUMPING.
        ORD     MBOPSTS
        STD     MBOPSTS
        BRA     CONCP3          ; CONTINUE
NODUMP  COMD
        ANDD    MBOPSTS
        STD     MBOPSTS                 ; STORE OPERATING STATUS.
CONCP3  LDD     #MBSTATHIGHLSPOS        ; HIGH LEVEL SENSOR
        TST     HIGHLEVELSENSOR    ;
        BEQ     NOHIGH          ; OFF
        ORD     MBOPSTS
        STD     MBOPSTS
        BRA     CONCP4          ; CONTINUE
NOHIGH  COMD
        ANDD    MBOPSTS
        STD     MBOPSTS        ; STORE OPERATING STATUS.
CONCP4  LDD     #MBSTATLOWLSPOS        ; HIGH LEVEL SENSOR
        TST     LOWLEVELSENSOR    ;
        BEQ     NOLOW           ; OFF
        ORD     MBOPSTS
        STD     MBOPSTS
        BRA     CONCP5          ; CONTINUE
NOLOW   COMD
        ANDD    MBOPSTS
        STD     MBOPSTS        ; STORE OPERATING STATUS.
CONCP5  LDD     #MBSTATDUMPLSSTATUSBPOS ; DUMP SENSOR
        TST     LEVSEN1STAT    ;
        BEQ     NODMPLS           ; OFF
        ORD     MBOPSTS
        STD     MBOPSTS
        BRA     CONCP6         ; CONTINUE
NODMPLS COMD
        ANDD    MBOPSTS
        STD     MBOPSTS        ; STORE OPERATING STATUS.
CONCP6
        LDD     #MBCSTATBYPASSSTPOS   ; .
        LDX     #BYPASSSTATUS
        LDY     #MBOPSTS
        JSR     SETCLRBITDEPONX

        CLRA
        LDAB    LS1OFF         ; LS1 OFF ?
        STD     MBLS1OFF       ;
        LDAB    LS2OFF         ;
        STD     MBLS2OFF       ;
        LDAB    LS3OFF         ;
        STD     MBLS3OFF       ;
        LDAB    LS4OFF         ;
        STD     MBLS4OFF       ;
        LDAB    LS5OFF         ;
        STD     MBLS5OFF       ;
        LDAB    LS6OFF         ;
        STD     MBLS6OFF       ;
        LDAB    LS7OFF         ;
        STD     MBLS7OFF       ;
        LDAB    LS8OFF         ;
        STD     MBLS8OFF       ;
        LDAB    LS9OFF         ;
        STD     MBLS9OFF       ;
        LDAB    LS10OFF         ;
        STD     MBLS10OFF       ;
        LDAB    LS11OFF         ;
        STD     MBLS11OFF       ;
        LDAB    LS12OFF         ;
        STD     MBLS12OFF       ;

;       OFFLINE O/P STATUS

CONCL22 LDD     #MBCSTATOFFLINESTPOS
        TST     FEEDCLOSE
        BEQ     FEEDOP
        ORD     MBOPSTS
        STD     MBOPSTS
        BRA     CONCL3         ; CONTINUE
FEEDOP  COMD                   ; RESET BIT
        ANDD    MBOPSTS
        STD     MBOPSTS  ;
CONCL3:
        JSR     COPYFBWTOMB     ; COPY FILL BY WEIGHT STATUS TO MODBUS TABLE.

        CLRA
        LDAB    ADDR_HX         ; SERIAL ADDRESS
        STD     MBADDRESS

        LDX     DACPKGHR        ; D/A PER KG  XX XX XX.YY YY
        STX     EREG
        LDX     DACPKGHR+2
        STX     EREG+2
        LDAA    DACPKGHR+4
        STAA    EREG+4

        LDX     #EREG     ;
        LDAB    #5
        LDAA    #1
        JSR     GPSFTR            ;~RESULT
        LDX     #EREG+1
        JSR     BCDHEX3X
        STE     MBDACPKGHR


        LDX     DACPMPM         ; D/A PER METER PER MINUTE
        STX     EREG
        LDX     DACPMPM+2
        STX     EREG+2
        LDAA    DACPMPM+4
        STAA    EREG+4

        LDX     #EREG     ;
        LDAB    #5
        LDAA    #1
        JSR     GPSFTR            ;~RESULT
        LDX     #EREG+1
        JSR     BCDHEX3X
        STE     MBDACPMPM

        LDD     SEIINPUTSTATE
        STD     MBSEIINPUTSTATE

        LDD     SEIALARMSTATE
        STD     MBSEIALARMSTATE


        LDD     LLSINPUTSTATE
        STD     MBLLSINPUTSTATE

        LDAB    #RAMBANK
        TBYK
        LDD     #MBLINKL2BPOS   ; .
        LDX     #MCU16_L2
        LDY     #MBDIAGOPTIONS
        JSR     SETCLRBITDEPONX

        LDD     #MBLINKL3BPOS   ; .
        LDX     #MCU16_L3
        LDY     #MBDIAGOPTIONS
        JSR     SETCLRBITDEPONX

        LDD     #MBLINKL4BPOS   ; .
        LDX     #MCU16_L4
        LDY     #MBDIAGOPTIONS
        JSR     SETCLRBITDEPONX

        LDD     #MBLINKL5BPOS   ; .
        LDX     #MCU16_L5
        LDY     #MBDIAGOPTIONS
        JSR     SETCLRBITDEPONX

        LDD     CURRENTEXTRUDERNO
        STD     MBCURRENTEXTRUDERNO

        LDE     EXTRUDERNO      ;
        STE     MBPENDINGEXTRUDERNO   ; STORE MODBUS PENDING EXTRUDER NO.

        CLRA
        LDAB    VACSEQ                  ; READ VACUUM SEQUENCING
        STD     MBVACUUMSEQUENCE        ; STORE THE VACUUM SEQUENCE

CPYAUXX RTS


; COPY THE VACUUM NO THAT IS ON.


COPYMBVACUUMONSTATUS:
        LDD     CURRENTEXTRUDERNO                 ; READ CURRENT EXTRUDER
        STD     MBVACUUMINFO
        RTS


; RESET VACUUM STATUS.

COPYMBVACUUMOFFSTATUS
       CLRW      MBVACUUMINFO
       RTS



SETCLRBITDEPONX:
        TST     0,X             ; CHECK FOR ZERO
        BEQ     CLRBT           ;
        ORD     0,Y
        STD     0,Y
        BRA     XITS
CLRBT   COMD
        ANDD    0,Y
        STD     0,Y
XITS    RTS


FBWLOOKUP FDB   MB1STATUS
          FDB   MB2STATUS
          FDB   MB3STATUS
          FDB   MB4STATUS
          FDB   MB5STATUS
          FDB   MB6STATUS
          FDB   MB7STATUS
          FDB   MB8STATUS
          FDB   MB9STATUS
          FDB   MB10STATUS
          FDB   MB11STATUS
          FDB   MB12STATUS



COPYFBWTOMB:
        LDAB   #RAMBANK
        TBZK                    ;

        LDAB   #BANK4
        TBYK                    ;~;SET EK FOR BANK 1, I.E. RAM

        LDX     #SEQTABLE
REPFBWC LDAB    0,X
        TBA
        CMPB    #SEQDELIMITER
        BEQ     XIT

        LDY     #FBWLOOKUP
        ANDB    #$0F
        DECB
        ABY
        ABY
        LDZ     0,Y             ; READ MODBUS STATUS ADDRESS
        TAB
        ANDB    #FILLBYWEIGHT
        BEQ     NOFBW
        LDD     #MBFILLBYWEIGHT
        ORD     0,Z
        STD     0,Z
        BRA     CONFBWC                 ; CONTINUE

NOFBW   LDD     #MBFILLBYWEIGHT
        COMD
        ANDD    0,Z
        STD     0,Z
CONFBWC AIX     #1                      ; NEXT POSITION
        BRA     REPFBWC
XIT     RTS






; COPY X TO Z E TIMES.
;
CPYXTZETIMES
REPXTZ  LDAA     0,X             ; READ X REGISTER
        STAA    0,Z             ;
        AIZ     #1              ; INCREMENT POINTERS
        AIX     #1              ;
        ADDE    #-1             ; E--
        BNE     REPXTZ          ; REPEAT X TO Z
        RTS


COPYMBDIAGNOSTICS:

        LDAB    PUPMIN
        JSR     BCDHEX1                 ; TO HEX
        STAB    MBPOWERUPTIME           ; POWER UP TIME.

        LDAB    PUPHRS
        JSR     BCDHEX1                 ; TO HEX
        STAB    MBPOWERUPTIME+1        ; POWER UP TIME.

        LDAB    PUPDOM
        JSR     BCDHEX1                 ; TO HEX
        STAB    MBPOWERUPTIME+2        ; POWER UP TIME.


        LDAB    PUPMON
        JSR     BCDHEX1                 ; TO HEX
        STAB    MBPOWERUPTIME+3        ; POWER UP TIME.


        LDD     CALMODSERNO+1           ; READ CAL NO.
        JSR     BCDHEX2                 ; TO HEX
        STD     MBCALMODSERNO           ; STORE.


        LDX     #SBBSERNO
        JSR     BCDHEX3X
        STAB    MBSBBSERNO+1
        STE     MBSBBSERNO+2

        LDX     #GOODTXCTR1
        JSR     BCDHEX3X                 ; CONVERT TO HEX.
        CLRA
        STD     MBGOODTXCTR1
        STE     MBGOODTXCTR1+2             ; COMMS COUNTER 1 REMOTE DISPLAY.


        LDX     #GOODTXCTR2
        JSR     BCDHEX3X                 ; CONVERT TO HEX.
        CLRA
        STD     MBGOODTXCTR2           ; COMMS COUNTER 2 LLS
        STE     MBGOODTXCTR2+2

        LDX     #GOODTXCTR3
        JSR     BCDHEX3X                 ; CONVERT TO HEX.
        CLRA
        STD     MBGOODTXCTR3
        STE     MBGOODTXCTR3+2

        LDX     #GOODRXCTR1
        JSR     BCDHEX3X                 ; CONVERT TO HEX.
        CLRA
        STD     MBGOODRXCTR1
        STE     MBGOODRXCTR1+2            ; COMMS COUNTER 1 REMOTE DISPLAY.

        LDX     #GOODRXCTR2
        JSR     BCDHEX3X                 ; CONVERT TO HEX.
        CLRA
        STD     MBGOODRXCTR2
        STE     MBGOODRXCTR2+2            ; COMMS COUNTER 2 LLS

        LDX     #GOODRXCTR3
        JSR     BCDHEX3X                 ; CONVERT TO HEX.
        CLRA
        STD     MBGOODRXCTR3             ; COMMS COUNTER 3  LLS
        STE     MBGOODRXCTR3+2           ; COMMS COUNTER 3  LLS

        LDX     #CHECKSUMERR1
        JSR     BCDHEX3X                 ; CONVERT TO HEX.
        CLRA
        STD     MBCHECKSUMERR1            ; COMMS CHECKSUM ERROR COUNTER.
        STE     MBCHECKSUMERR1+2            ; COMMS CHECKSUM ERROR COUNTER.

        LDX     #CHECKSUMERR2
        JSR     BCDHEX3X                 ; CONVERT TO HEX.
        CLRA
        STD     MBCHECKSUMERR2           ; COMMS CHECKSUM ERROR COUNTER
        STE     MBCHECKSUMERR2+2            ; COMMS CHECKSUM ERROR COUNTER

        LDX     #CHECKSUMERR3
        JSR     BCDHEX3X                 ; CONVERT TO HEX.
        CLRA
        STD     MBCHECKSUMERR3              ; COMMS CHECKSUM ERROR COUNTER
        STE     MBCHECKSUMERR3+2            ; COMMS CHECKSUM ERROR COUNTER

        LDX     #TIMEOUTCTR1
        JSR     BCDHEX3X                 ; CONVERT TO HEX.
        CLRA
        STD     MBTIMEOUTCTR1              ; COMMS TIMEOUT COUNTER
        STE     MBTIMEOUTCTR1+2            ; COMMS TIMEOUT COUNTER

        LDX     #TIMEOUTCTR2
        JSR     BCDHEX3X                 ; CONVERT TO HEX.
        CLRA
        STD     MBTIMEOUTCTR2              ; COMMS TIMEOUT COUNTER
        STE     MBTIMEOUTCTR2+2            ; COMMS TIMEOUT COUNTER

        LDX     #TIMEOUTCTR3
        JSR     BCDHEX3X                 ; CONVERT TO HEX.
        CLRA
        STD     MBTIMEOUTCTR3              ; COMMS TIMEOUT COUNTER
        STE     MBTIMEOUTCTR3+2            ; COMMS TIMEOUT COUNTER

        LDD     CYCLECOUNTER
        STD     MBCYCLECOUNTER
        JSR     CPYMBALARMS
        JSR     COPYENCRYPTIONDATATOMB

        RTS


; COPY ENCRYPTION DATA TO MODBUS TABLE.

COPYENCRYPTIONDATATOMB:

        LDD     OPTIONCODE      ; OPTIONS
        STD     MBOPTIONCODE

        LDD     KGHHRCTR
        STD     MBKGHHRCTR
        LDD     GPMHRCTR
        STD     MBGPMHRCTR
        LDD     GPMHRCTR
        STD     MBGPMHRCTR
        LDD     MBHRCTR
        STD     MBMBHRCTR
        LDD     BLENDERRUNHRCTR
        STD     MBBLENDERRUNHRCTR
        LDD     REPHRCTR
        STD     MBREPHRCTR
        LDD     LLSHRCTR
        STD     MBLLSHRCTR
        LDD     SOFTWAREID
        STD     MBSOFTWAREID
        RTS












;
;* MBLAGS - ROUTINE TO DECREMENT LAGS - CALLED ON 10Hz IRQ
; 2/2/98 DISABLED (MMK)
MBLAGS  TST     LDNSPLAG        ; LOAD NEW SPTS LAG
        BEQ     MBLAGS10
        DEC     LDNSPLAG
        BNE     MBLAGS10
        JSR     COPYSPMB        ; IF TIME-OUT LOAD CURRENT SPT'S BACK TO MB
MBLAGS10:
        RTS




;*****************************************************************************;
; FUNCTION NAME : RESETCLEANBITS                                              ;
; FUNCTION      : RESET CLEAN START END BITS.                                 ;
; INPUTS        : NONE                                                        ;                       ;
;*****************************************************************************;



RESETCLEANBITS:
        LDD     #MBSTATCLEANSTARTEDBPOS
        COMD
        ANDD    MBOPSTS
        STD     MBOPSTS
RESETCLEANFINISHBIT:
        LDD     #MBSTATCLEANENDEDBPOS
        COMD
        ANDD    MBOPSTS
        STD     MBOPSTS
        RTS



;*****************************************************************************;
; FUNCTION NAME : SETSTARTCLEANMBBIT                                          ;
; FUNCTION      : RESET CLEAN START                                           ;
; INPUTS        : NONE                                                        ;                       ;
;*****************************************************************************;



SETSTARTCLEANMBBIT:
        LDD     #MBSTATCLEANSTARTEDBPOS
        ORD     MBOPSTS
        STD     MBOPSTS
        RTS


;*****************************************************************************;
; FUNCTION NAME : SETSTARTCLEANMBBIT                                          ;
; FUNCTION      : RESET CLEAN START                                           ;
; INPUTS        : NONE                                                        ;                       ;
;*****************************************************************************;


SETENDCLEANMBBIT:
        LDD     #MBSTATCLEANENDEDBPOS
        ORD     MBOPSTS
        STD     MBOPSTS
        RTS


;*****************************************************************************;
; FUNCTION NAME : SETSTARTCLEANMBBIT                                          ;
; FUNCTION      : RESET CLEAN START                                           ;
; INPUTS        : NONE                                                        ;                       ;
;*****************************************************************************;




COPYMBCYCLEDIAGNOSTICS:
        LDAB    #1                       ; COUNTER TO READ DATA
REPTR   PSHB
        LDX     #CMP1TARWGT     ; ADDRESS OF COMP #N WEIGHT
        PULB
        PSHB
        DECB
        ABX
        ABX
        ABX
        LDAB    #RAMBANK
        TBZK
        LDZ     #MBPCMP1TARWGT
        PULB
        PSHB
        DECB
        ASLB
        ASLB
        ABZ
        JSR     BCDHEX3X        ; CONVERT 3 BYTES @X
        CLRA
        STD     0,Z
        STE     2,Z

; ACTUAL WEIGHTS

        LDX     #CMP1ACTWGT     ;
        PULB
        PSHB
        DECB
        ABX
        ABX
        ABX
        LDAB    #RAMBANK
        TBZK
        LDZ     #MBPCMP1ACTWGT
        PULB
        PSHB
        DECB
        ASLB
        ASLB
        ABZ
        JSR     BCDHEX3X        ; CONVERT 3 BYTES @X
        CLRA
        STD     0,Z
        STE     2,Z



; TARGET COUNTS

        LDX     #CMP1CNTG     ;
        PULB
        PSHB
        DECB
        ABX
        ABX
        ABX
        LDAB    #RAMBANK
        TBZK
        LDZ     #MBPCMP1CNTG
        PULB
        PSHB
        DECB
        ASLB
        ASLB
        ABZ
        JSR     BCDHEX3X        ; CONVERT 3 BYTES @X
        CLRA
        STD     0,Z
        STE     2,Z


        LDX     #CMP1CNAC     ;
        PULB
        PSHB
        DECB
        ABX
        ABX
        ABX
        LDAB    #RAMBANK
        TBZK
        LDZ     #MBPCMP1CNAC
        PULB
        PSHB
        DECB
        ASLB
        ASLB
        ABZ
        JSR     BCDHEX3X        ; CONVERT 3 BYTES @X
        CLRA
        STD     0,Z
        STE     2,Z

        LDX     #CMP1TIMS     ;
        PULB
        PSHB
        DECB
        ABX
        ABX
        ABX
        LDAB    #RAMBANK
        TBZK
        LDZ     #MBPCMP1TIMS
        PULB
        PSHB
        DECB
        ASLB
        ASLB
        ABZ

        CLRA
        LDAB    0,X
        LDE     1,X
        STD     0,Z
        STE     2,Z


        LDX     #CMP1CPI     ;
        PULB
        PSHB
        DECB
        ABX
        ABX
        ABX
        LDAB    #RAMBANK
        TBZK
        LDZ     #MBPCMP1CPI
        PULB
        PSHB
        DECB
        ASLB
        ASLB
        ABZ
        JSR     BCDHEX3X        ; CONVERT 3 BYTES @X
        CLRA
        STD     0,Z
        STE     2,Z

        LDX     #CMP1CPIST2     ;
        PULB
        PSHB
        DECB
        ABX
        ABX
        ABX
        LDAB    #RAMBANK
        TBZK
        LDZ     #MBPCMP1CPIST2
        PULB
        PSHB
        DECB
        ASLB
        ASLB
        ABZ
        JSR     BCDHEX3X        ; CONVERT 3 BYTES @X
        CLRA
        STD     0,Z
        STE     2,Z


        LDX     #CMP1CPIST3     ;
        PULB
        PSHB
        DECB
        ABX
        ABX
        ABX
        LDAB    #RAMBANK
        TBZK
        LDZ     #MBPCMP1CPIST3
        PULB
        PSHB
        DECB
        ASLB
        ASLB
        ABZ
        JSR     BCDHEX3X        ; CONVERT 3 BYTES @X
        CLRA
        STD     0,Z
        STE     2,Z







        LDX     #CMP1OPENSECONDS     ;
        PULB
        PSHB
        DECB
        ABX
        ABX
        ABX
        LDAB    #RAMBANK
        TBZK
        LDZ     #MBPCMP1OPENSECONDS
        PULB
        PSHB
        DECB
        ASLB
        ASLB
        ABZ
        JSR     BCDHEX3X        ; CONVERT 3 BYTES @X
        CLRA
        STD     0,Z
        STE     2,Z

        PULB
        INCB
        CMPB    NOBLNDS
        LBLS    REPTR







        RTS




MBALLOOKUP FDB   MBALBUF1
           FDB   MBALBUF2
           FDB   MBALBUF3
           FDB   MBALBUF4
           FDB   MBALBUF5
           FDB   MBALBUF6
           FDB   MBALBUF7
           FDB   MBALBUF8
           FDB   MBALBUF9
           FDB   MBALBUF10
           FDB   MBALBUF11
           FDB   MBALBUF12

CPYMBALARMS:
        LDAB    #BANK4
        TBYK                            ; SET UP YK
        LDY     #MBALLOOKUP             ; ALARM LOOKUP TABLE
        LDAB    #RAMBANK
        TBZK                            ;
        LDZ     #ALARMARRAY             ; ALARM ARRAY
        LDAB    #12                     ; NO OF ALARM OCCURENCES TO COPY
REPALTR PSHB
        JSR     COPYALARMDATA           ; COPY ALARM DATA
        AIZ     #ALMSIZE                ; TO NEXT ALARM
        AIY     #2                      ; TO NEXT MODBUS STORAGE
        PULB
        DECB
        BNE     REPALTR                 ; REPEAT
        RTS


COPYALARMDATA:
        PSHM    Z
        LDX     0,Y                     ; READ MODBUS STORAGE
        LDD     0,Z                     ; READ ALARM TYPE AND COMP NO.
        STD     0,X                     ; STORE

        LDAB    4,Z                     ; HOURS
        JSR     BCDHEX1
        STAB    2,X

        LDAB    5,Z                     ; MINUTES
        JSR     BCDHEX1
        STAB    3,X

        LDAB    3,Z                     ; MONTH
        JSR     BCDHEX1
        STAB    4,X

        LDAB    2,Z                     ; DAY
        JSR     BCDHEX1
        STAB    5,X

        LDD     6,Z                     ; FRONT ROLL NO
        JSR     BCDHEX2                 ;
        STD     6,X                     ;

        LDAB    8,Z                     ; READ FRONT ROLL LENGTH
        LDE     9,Z                     ;
        JSR     BCDHEX3                 ; CONVERT TO HEX
        STE     8,X                     ; STORE DATA

        LDD     11,Z                    ; FRONT ROLL NO
        JSR     BCDHEX2                 ;
        STD     10,X                    ;

        LDAB    13,Z                    ; READ BACK ROLL LENGTH
        LDE     14,Z                    ;
        JSR     BCDHEX3                 ; CONVERT TO HEX
        STE     12,X                    ; STORE TO MODBUS

        LDD     16,Z                    ; ORDER NO
        STD     14,X                    ;

        LDD     18,Z                    ; OPERATOR NO.
        STD     16,X                    ;
        PULM    Z
        RTS



;*****************************************************************************;
; FUNCTION NAME : CHECKFORPENDINGRECIPE                                       ;
; FUNCTION      : CHECKS IF RECIPE WAITING IN SINGLE RECIPE MODE              ;                                           ;
; INPUTS        : NONE                                                        ;                       ;
;*****************************************************************************;



CHECKFORPENDINGRECIPE:
        LDAA    BLENDERMODE
        CMPA    #SINGLERECIPEMODE       ; SINGLE RECIPE MODE
        BNE     XITRCC                  ; EXIT RECIPE CHECK
        TST     RECIPEWAITING           ; IS THERE A RECIPE WAITING ?
        BEQ     XITRCC                  ; NO
        TST     BATCHHASFILLED          ; IF A BATCH HAS FILLED THEN THE ORDER CHANGE WILL BE GOT ON THE NEXT CYCLE
        BNE     XITRCC
        TST     BLENDERINHIBIT          ; RECIPE DOWNLOAD INHIBITED ?
        BNE     XITRCC                  ; YES
        CLR     RECIPEWAITING             ; RESET RECIPE FLAG
        JSR     LOADPERTARGETSFROMCOMMS
        LDAA    #1
        STAA    BLENDERINHIBIT          ; BLENDER IS NOW INHIBITED FROM ACCEPTING ANOTHER ORDER.
        LDAA    #1
        STAA    CLEANONNEXTCYCLE
        LDD     EXTRUDERNO
        STD     CURRENTEXTRUDERNO       ; STORE CURRENT RECIPE NO.
;       JSR     PAUSEOFF                ; UN PAUSE BLENDER
        JSR     VACINHIBITOFF
XITRCC  RTS



;*****************************************************************************;
; FUNCTION NAME : SETVACUUMSTATUS                                             ;
; FUNCTION      : SETS IN PROGRESS BIT                                        ;
; INPUTS        : A HOLDS THE COMPONENT BEING VACUUMED                                                        ;                       ;
;*****************************************************************************;



SETVACUUMSTATUS:
        LDE     #1
RECIP   TSTA
        BEQ     XITSTS  ; ERROR CONDITION
        DECA            ; DECREMENT VACUUM NO.
        BEQ     SETIPB  ; SET IN PROGRESS BIT.
        ASLE
        BRA     RECIP   ; REPEAT
SETIPB  ORE     MBVACUUMSTATUS
        STE     MBVACUUMSTATUS  ; STORE STATUS
XITSTS  RTS


;*****************************************************************************;
; FUNCTION NAME : CLEARVACUUMSTATUS                                             ;
; FUNCTION      : SETS IN PROGRESS BIT                                        ;
; INPUTS        : A HOLDS THE COMPONENT BEING VACUUMED                                                        ;                       ;
;*****************************************************************************;



CLEARVACUUMSTATUS:
        LDE     #1
RECIPC  TSTA
        BEQ     XITCLS  ; ERROR CONDITION
        DECA            ; DECREMENT VACUUM NO.
        BEQ     CLRIPS  ; CLEAR IN PROGRESS BIT.
        ASLE
        BRA     RECIPC  ; REPEAT
CLRIPS  COME
        ANDE    MBVACUUMSTATUS
        STE     MBVACUUMSTATUS  ; STORE STATUS
XITCLS  RTS

























*/


