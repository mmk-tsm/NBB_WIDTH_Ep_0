//////////////////////////////////////////////////////
// FGOneHz.c
//
// Group of functions that are called on 1Hz, in the foreground
//
// M.McKiernan                          25-11-2004
// First Pass
//
// M.McKiernan                          07-12-2004
// Added function - CopyCycleDiagnosticsToMB( ), to copy cycle diagnostics into
// MB table into ForegroundOneHzProgram.
// M.McKiernan                          13-12-2004
// Added CheckForAlarm() & CopyAlarmTableToMB (in ForegroundOneHzProgram().
// Activated PutAlarmTable()'s
// M.McKiernan                          16-12-2004
// Call to PrintCycleDiagnostics()
//
// P.Smith                          15-3-2005
// uncomment ClearThroughputTotals
//
// P.Smith                          16-3-2005
// call ExtruderStallCheck
//
// P.Smith                          29-3-2005
// Added CheckMixerContactor
//
// P.Smith                          1-4-2005
// Added CheckHighLowRegrindSensors
// uncomment CheckHighLowRegrindSensors call
//
//
// P.Smith                          7-4-2005
// added void CheckForMixerOff ( void )
//
// P.Smith                          12-8-2005
// decrement of counters added
// expansion comes in at 6 not 4.
//
// P.Smith                          15-8-2005
// added set of pauseinnseconds, removal of offlinepauseateoc.
// correct bypass activation, if cleaning use clean bypass time, otherwise use
// normal bypass time entered.
// added CheckLicenseForFlashing
// added StorePowerDownTime call.
//
//
// P.Smith                          12/9/2005
// copy of low level sensor data to g_arrnWriteLLSMBTable
// call CheckForPendingRecipe
//
// P.Smith                          23/9/2005
// call DecideOnLicenseExpiryWarning on the hour
//
//
// P.Smith                          23/9/2005
// remove // CheckForMasterCode() no need to call continuously.
//
//
// P.Smith                          17/10/2005
// Make  g_arrnMBTable unsigned int
//
// P.Smith                          22/11/2005
// use g_nReMixOnTime, name confusion with g_nMixerOnTime
//
//
// P.Smith                      20/2/06
// first pass at netburner hardware conversion.
// Bool -> BOOL
// #include <basictypes.h>
//
// P.Smith                      22/2/06
// PutAlarmTable( SEICOMMSALARM,  0 );     // indicate alarm, //PTALTB
// set expansion flag g_bExpanioFitted = FALSE;
// CopyAlarmTableToMB();   // copies alarm table into modbus.
//  CheckForAlarm();            // checks for alarm(s) and switches on/off the alarm outputs accordingly.
// #include "Exstall.h"
// ExtruderStallCheck();   // has extruder stalled ?
// #include "Conalg.h"
//
//
// P.Smith                      28/2/06
// PutAlarmTable( MIXSTOPALARM,  0 );
// CopyCycleDiagnosticsToMB( );        // copy cycle diagnostics into MB table.
// ClearThroughputTotals();    // CLEARTHROUGHPUTTOTALS
//
// P.Smith                      19/4/06
// added PutAlarmTable( SHUTDOWNALARM,  0 );     // indicate alarm, Component no. is zero.
// RemoveAlarmTable( MIXSTOPALARM,  0 );     // Remove alarm
//
// P.Smith                      1/6/06
// PrintCycleDiagnostics();    // Start print of  batch cycle diagnostics.
// #include "PrntDiag.h"
//
// P.Smith                      28/6/06
// call IOBoardRTCGetTime( bts );
// #include "I2CFuncs.h"
// call CheckTemporaryHourlyCounter
// #include "License.h"
// call DecideOnLicenseExpiryWarning(); // asm DECIDEONEXPIRYPAGE
//  call StorePowerDownTime(); // ASM  STRPDNT
//
// P.Smith                      14/7/06
/// remove old lsdelay functionality                                ;
// new functionality delays the level sensor alarm for the time    ;
// that has been indicated in the lsdelay entry. after the time has;
// expired, the alarm is generated if it is still there.           ;
//
// P.Smith                      16/11/06
// Remove reference to level sensor monitoring.
// modify level sensor off time to check CalibrationData.m_nMixShutoffTime  to
// determine when the valve should be switched off.
// call IntegrateCommsLineSpeed
// g_bCycleIndicate is used instead of integer

//   P.Smith                                               5/2/07
//   Remove unused iprintf

//   P.Smith                                               6/3/07
//   if g_nResetAllFlowRates set, then clear all flow rates, set all components to fill
//   by weight and reset flow rate round robin for each all components.
//
//
//   P.Smith                                               23/4/07
//   Call MultiBlendCycle
//
//   P.Smith                                               11/5/07
//   set m_nResetCPIRRCtr instead of true / false toggle
//
//   P.Smith                                               24/5/07
//   call DecideIfExpansionisRequired
//
//   P.Smith                                               8/6/07
//   do not reset the flow rates to zero on a flow rate calibrate.
//
//   P.Smith                                               8/6/07
//   call CheckForRecipeDownLoad for mult blend, call ResetRoundRobinFlowRateForAllComponents
//
//   P.Smith                                               18/6/07
//   call SendOuputsToOptimisationCard
//   reintroduce reset of flow rates when rate calibration is initiated.
//   add "LeaveFillByWeight" when rate calibration is initiated.
//
// P.Smith                                                 22/6/07
// name change to proper blender names.
//
// P.Smith                                                 26/6/07
// call DecideIfLoaderShouldBeEnabled, not doing much for now.
// ConvertOutputToLogicalFormat function to convert output format to ensure that
// the lls card outputs use one conector for input and output.
// Originally one conector was used for one input and the first two outputs.
//
// P.Smith                                                 4/7/07
// call EstimateLoaderWeight every second.
//
// P.Smith                                                 19/7/07
// call LoadSetPointDataFromModbusTable
//
//
// M.McKiernan                      17/9/07
// Removed g_arrnMBTable[] & g_arrnMBTableFlags[], now in BatVars2
//
// P.Smith                      16/10/07
// if the override counter is non zero, then decrement same, when it
// goes to zero, reset the over ride bool  to false, this now allows the
// cleans to work normally.
//
// P.Smith                      24/10/07
// g_nAllowReadOfControlInputsCtr counter decremented if non zero.
// set sei command to 0 when counter goes to zero.
//
// P.Smith                      15/11/07
// call CheckForScrewSpeedChange every second
// decrement g_nVolumetricEstimateCounter and set runest if going to zero.
//
// P.Smith                      21/11/07
// remove EstimateLoaderWeight
//
// P.Smith                      22/11/07
// put back in loader weight estimation.
//
// P.Smith                      28/11/07
// Write extruder d/a to max5812 d/a
//
// P.Smith                      30/11/07
// decrement g_nWriteHoldOffCounter if non zero.
//
// P.Smith                      5/12/07
// no remix of material every 10 minutes.
// set of g_bShowInstantaneousWeight depending on g_CalibrationData.m_bFillOrderManualSelect                  ;
// decrement of g_nHoldOffLevelSensorAlarmSecondCounter
// remove reference to Micrabatch
//
// P.Smith                      5/12/07
// Correct g_bShowInstantaneousWeight set, it should be set when the sensitivity is high
//
// P.Smith                          30/1/08
// correct compiler warnings
//
// P.Smith                          11/2/08
// if g_bSEIReadDataReady is true,set to false, check g_nCheckForCorrectDownloadCtr
// if non zero, check if the volumetric d/a value g_nExtruderVolumetricDAValue is the same
// as the value read from the sei modbus table, if it is not transfer d/a to g_nExtruderDAValue
// again.
//
//
// P.Smith                          7/3/08
// if control type is increase / decrease, then make control % calculation run
// every second.
// added CheckForAChangeInSEICalibrationData called on one hz to check if
// there is a change in the inc /dec data, if there is, then set flag
// to initiate a long write message.
//
// P.Smith                          27/3/08
// use g_ProcessData for flow rate stuff.
//
// M.McKiernan                      14.4.2008
// Added test for telnet - see "testonly for telnet"
//
// P.Smith                          10/6/08
// if g_bSDCard complete are set, increment g_bSDCard counter and reset
// bools after so many seconds. this status is passed back to the panel
// so show that the sd card operation has taken place.
// g_bCheckIfShouldCycleFlag set to false if sensor covered.
//
// P.Smith                                  23/6/08
// added AnalyseCommunications to check if the comms is running to the different
// devices.Add enable of Telnet.
//
// P.Smith                          1/7/08
// Remove Micrabatch related stuff
//
// P.Smith                          11/7/08
// copy lls alarm to low level sensor card to indicate where the alarm is on the
// blender.
//
// P.Smith                          15/7/08
// call CheckForSDCardMaintainenance every minute to determine if the sd card maintenance
// is required.
// add events for sd card operations.
//
// P.Smith                          21/7/08
// Remove StartTelnet for now, there are some issues that have been seen when the Telnet
// task is being stopped.
//
// P.Smith                          23 /7/08
// remove externs for comms arrays from here, put back in Telnet
//
// P.Smith                          24 /7/08
// decrement g_nDisableSDCardAccessCtr if non zero
//
// P.Smith                          11/9/08
// if g_bWriteEventLogToSdCard has been set and not in the middle of a cycle.
// write the text to the sd card that has been generated in the middle of the cycle.
//
// P.Smith                          17/9/08
//  if pib is enabled, read the pib component as the last component.
// also if there is a change in the pib enable option then initiate the set up of the
// sequence table again to either omit or add this component to the table.
// if m_bLiquidEnabled is enabled, then send g_nLiquidAdditiveOutputDAValue to WriteMAX5812DAC
//
// P.Smith                          26/9/08
// read temperature,call CheckIfMaterialHasDumped to check if material has dumped properly.
// call LiquidAdditiveOnehz.
// remove WriteMAX5812DAC(g_nExtruderDAValue) from here.
//
// P.Smith                          26/9/08
// put back in write of d/a on the second.
//
// P.Smith                                  30/9/08
// check for any alarm on the new alarm array, if there is an alarm, then
// set modbus table BATCH_SUMMARY_PROCESS_ANY_ALARM_OCCURRED to indicate that
// there is an alarm.
//
// P.Smith                                  30/9/08
// if g_bAlarmOccurredHistory array is non zero, then set BATCH_SUMMARY_PROCESS_ANY_ALARM_OCCURRED
//
// P.Smith                                  16/10/08
// todo to lp
//
// P.Smith                                  11/11/08
// added CheckForTelNetTimeout
//
// P.Smith                                  14/11/08
// call ContinuousTelnetUpdate to determine if a one second update is required
// decrement g_nTelNetSecondsCounter
// if g_bSDCardAccessInProgress false and g_bWriteDiagnosticsToSDCard true
// call WriteDiagnosticDataToSDCard and set g_bWriteDiagnosticsToSDCard to false
// call CheckForTelNetTimeout
//
// P.Smith                                  26/11/08
// check a/d value for max MAXLTC2415ATODVALUE (262000), pause blender and
// close all valves, it the reading comes back again restart the blender and move
// on.
//
// P.Smith                                  9/12/08
// incremenet mixing motor stopped history counter, level sensor ack counter
// increment minutes in auto counter and copy auto state.
// call OneHzHistoryLog
// call CopyOngoingToHourlyHistoryLog on the hour.
//
// P.Smith                                  11/12/08
// name change g_bTelnetSDCardAccessInProgress
// call DumpEvent to dump to usb if on.
// set g_nTCPIPReadHoldRegsCtr to 0 to allow tcpip comms status to work.
// this was being incremented but was not cleared.
// P.Smith                                  7/1/09
// use g_bHiRegLevelSensor as the high regrind sensor
//
// P.Smith                                  7/1/09
// correct high / low regrind, it should use max regrind when hi/lo covered.
// use max when hi uncovered, use nominal when hi/lo uncovered
// use lo until high / low covered - use high etc.
// it was originally using low when the high was uncovered which was incorrect.
// if low covered and high uncovered, do nothing.
// use variable g_bHiRegLevelSensor instead of g_bHighLevelSensor
// this is the mixing screw detection input, it is used as the high regrind input
//
// P.Smith                                  12/1/09
// set g_bWriteToExpansion2 to true to initiate spi write to expansion card #2 every
// second.
//
// P.Smith                                  22/1/09
// remove alarm flash set on license alarm
//
// P.Smith                                  13/2/09
// if the parity changes initialise the network serial port
//
// P.Smith                                  2/3/09
// call CheckFor24HourRollOver
//
// P.Smith                                  5/3/09
// check g_bHourRollOver, this flag is activated when the clock
// hour changes.
// call CopyOngoingToHourlyHistoryLog on clock hour rollover.
// remove auto minutes counter from the history structure, this allow it to
// count indefinitely.
// mixing motor trip counter is incremented in eeprom.
// add in comms check on the panel to determine if it is communicating
//
// P.Smith                                  27/3/09
// decrement g_nStorageInvervalCtr on one minute
//
// P.Smith                                  12/5/09
// in offline mode, when the blender is unpaused, make make the blender cycle again.
// to fill the weigh hopper. only run the pause if already not in offline pause
// if blender is in offline mode, do not enter CheckHighLowRegrindSensors
// use g_bHiRegLevelSensorHistory, there is a conflict with g_bHighLevelSensorHistory
// as it is used in offline mode as well as high regrind.
//
// P.Smith                                  8/6/09
// call CheckForCorrectComponentTargets every second to check the component targets.
//
// P.Smith                                  17/6/09
// call VacuumLoaderOneHertz
//
// P.Smith                                  25/6/09
// call LoaderDiagnostics, VAC8OneHzPrograms, check for m_bLoaderDebug before printing
// to telnet
//
// P.Smith                                  2/7/09
// added comms status for vacuum loading
//
// P.Smith                                  22/7/09
// added g_bActivateHourlyReport
//
// P.Smith                                  18/08/09
// a problem has been seen where when the rate calibration is done, the ignore cpi
// flag is set for the component that is retrying.
// when the rate calibration is done, the value is not accepted because the ignore flag
// is still set.
// this is corrected by clearing the ignore flag when the rate calibration is about to be
// initiated.
//
// P.Smith                                  1/09/09
// decrement g_nHoldOffEventLogWriteCtr if non zero.
//
// P.Smith                                  15/09/09
// added CheckForChangeInComponentFillingMethod
//
// P.Smith                                  15/09/09
// name change  CheckForChangeInComponentFillingMethod to CheckIfPulsingCalibrateIsRequired
//
// P.Smith                                  17/10/09
// added g_nAfterEndOfCycleCounter decremented
//
// P.Smith                                  16/11/09
// clear autocycle flag is  not diagnostics password
// decrement g_nDelayBeforeLoadCellLogCtr and set load cell reading flag g_bLogLoadCellReading
// to true when ctr is going to 0.
// this then allows the loadcell a/d values to be logged.
//
// P.Smith                                  19/11/09
// if ref a/d has been enabled for more than one minute, revert to load cell
// decrement g_nDelayBeforeRefLogCtr and when going to 0,set g_bTakeRefAToDReading to TRUE
// decrement g_nDelayBeforeRefLogCtr, set g_bTakeRefAToDReading when decremented to 0
//
// P.Smith                                  23/11/09
// a problem has been seen where the reference is being switched out too quickly.
// this is caused by the fact that the reference timeout counter is not being reset
// when the reference is not bing outputted.
//
// P.Smith                                  27/11/09
// if top up component reference no changes,set g_bPctFlag to initiate percentage update
//
// P.Smith                                  6/1/10
// check that event log is enabled before writing to sd card.
//
// P.Smith								29/01/2010.
// Call EIPOneHzProgram once a second, in ForegroundOneHzProgram( void )
//
// P.Smith								5/3/10.
// call CheckOptionsForValidLicense on one hz
//
// P.Smith								25/3/10.
// CopySecurityDataToModbus, if g_nSHASecretFailCtr is non zero increment
//
// P.Smith								6/4/10.
// check for watch dog command ENABLE_WATCH_DOG_AND_REBOOT
// if set decrement g_nWatchDogCommandInNseconds and enable
// watch dog and reboot when 0.
// if DISABLE_WATCH_DOG_AND_REBOOT disable watch dog and reboot
//
// P.Smith								15/4/10.
// CheckForUnRestrictedAccessToLicening
//
// P.Smith 									27/4/10
// increment hours running counter on the hour
// generate the hash for the log and initiate save to eeprom
//
// P.Smith 									4/5/10
// call FGOneHzMBTCPErrorChecking
// removed check for mixing screw detection
// remove AnalyseRotationDetector
// in CheckMixerContactor, remove check for mixing screw pulse detection
//
// P.Smith 									8/6/10
// call WidthHourFG & WidthOneSecond
//
// P.Smith 									7/7/10
// called WidthFGOneMinute
//
// P.Smith 									16/9/10
// remove check for max /ad value
//
// P.Smith 									27/10/11
// added g_wTimeAfterResetCtr decrement.
// check g_CalibrationData.m_bPCF8563RTCFitted read the new or old rtc

// M.McKiernan
// Diagnostic to test functions for 3 points on a circle measuerement method.
// Only runs when US telnet diagnostic running.
// 					WidthCalculation3Points();
//			float 	fRad1 =  CalculateRadiusFrom3Points( Xa, Ya, Xb, Yb, Xc, Yc );

//////////////////////////////////////////////////////

//*****************************************************************************
// INCLUDES
//*****************************************************************************
#include <Stdio.h>
#include "iosys.h"

#include "InitialiseHardware.h"
#include "SerialStuff.h"
#include "General.h"
#include "ConversionFactors.h"

#include "BatchCalibrationFunctions.h"
#include "BatchMBIndices.h"
#include "ConfigurationFunctions.h"
#include "Foreground.h"
#include "16R40C.h"
#include    "FiftyHz.h"
#include    "BatVars.h"
#include    "BatVars2.h"
#include    "Pause.h"
#include    "MBProSum.h"
#include    "MBProgs.h"
#include "Cleanio.h"
#include    "TenHz.h"
#include    "FGOneHz.h"
#include "SetpointFormat.h"
#include    "MBCyDiag.h"
#include "Alarms.h"
#include "PrntDiag.h"
#include "Exstall.h"
#include "Conalg.h"
#include "TimeDate.h"
#include "TSMPeripheralsMBIndices.h"
#include "License.h"
#include <basictypes.h>
#include <string.h>
#include "I2CFuncs.h"
#include "TimeDate.h"
#include "Lspeed.h"
#include "Multiblend.h"
#include "Expansion.h"
#include "CPIRRobin.h"
#include "OptimisationMB.h"
#include "Optimisation.h"
#include "Sdcard.h"
#include "Screw.h"
#include "Hash.h"
#include "Debug.h"
#include "Telnet.h"
#include <ucos.h>
#include "Eventlog.h"
#include "SdCardMaintenance.h"
#include "Eventlog.h"
#include "Liquidadditive.h"
#include "CycleMonitor.h"
#include <NBBGpio.h>
#include "VNC1L.h"
#include "Telnetcommands.h"
#include "HistoryLog.h"
#include "VNC1L_G.h"
#include "InitNBBcomms.h"
#include "BBCalc.h"
#include "Mbvacldr.h"
#include "vacvars.h"
#include "VacSeq.h"
#include "PulseValve.h"
#include "EIPProgs.h"
#include "License.h"
#include "Security.h"
#include "Watchdog.h"
#include "MBTogSta.h"
#include <ucos.h>
#include "mbtcp_usersys.h"
#include "WidthOneHz.h"
#include "WidthForeground.h"
#include "WidthVars.h"

#include "Width.h"
#include "WidthUltrasonics.h"



// Locally declared global variables
// Externally declared global variables
// Data
extern CalDataStruct    g_CalibrationData;
extern  structSetpointData  g_CurrentRecipe;
extern  structSetpointData      g_TempRecipe;
extern  structCPIRRData g_MainCPIRRData;

//extern float  g_fWeightAtoDCounts;

extern  int g_nKeyCode;
extern  int g_nLanguage;
extern  int g_nGeneralSecondTimer;
extern  structMeasurementUnits  g_MeasurementUnits;


extern  int g_nDisplayScreen;
extern  struct tm bts;
extern  int ntemp1;
extern  int g_arrnReadOptimisationMBTable[];
extern  OnBoardCalDataStruct   g_OnBoardCalData;
extern structSetpointData  g_CommsRecipe;
extern structCommsData  g_sModbusMaster;
extern  StructDebug   g_nTSMDebug;
extern  BOOL           g_bL1Fitted;                     // link L1 fitted
extern  structControllerData    structSEIData;
extern  structControllerData    structLLSData;
extern  structControllerData    structOptimisationData;
extern  structCommsData         g_sModbusNetworkSlave;
extern  StructAlarmTable    g_MainAlarmTable;
extern structVNC1LData   g_sVNC1L;
extern  long        g_lRawADCountsLTC2415;  //global
extern structUSB   g_sUSB;
extern structControllerData        structVAC8IOData[];
extern   structVacLoaderData g_sLoader[];
extern   structVacCalib   g_VacCalibrationData;
extern   int g_arrnWriteVAC8IOMBTable[MAX_VAC8IO_CARDS][VAC8IO_TABLE_WRITE_SIZE];
extern  structCPIRRData     g_WorkCPIRRData;
extern WORD g_nSHASecretFailCtr;
extern unsigned int g_nWatchDogCommand;
extern unsigned int g_nWatchDogCommandInNseconds;
extern bool g_bTestDLs;

/////////////////////////////////////////////////////
// ForegroundOneHzProgram()                     from ASM = part of PITCALL, moving from PIT.
//
// NB: This was called on PIT interrupt in assembler and run on second rollover
// stuff that needs to be handled once per second (was in PITCALL
// from just after SECROVER to NOVF: and PITINT1: to NOTHR
//
// Michael McKiernan                        25.11.2004
// First Pass.
//////////////////////////////////////////////////////
void ForegroundOneHzProgram( void )
{

    BOOL    btest;
    int i,j;
    unsigned int unTemp;
    BOOL IsAlarm = FALSE;
    float Xa, Xb, Xc, Ya, Yb, Yc;
    float Xa1, Xb1, Xc1, Ya1, Yb1, Yc1;
    float Xa2, Xb2, Xc2, Ya2, Yb2, Yc2;
    float Xa3, Xb3, Xc3, Ya3, Yb3, Yc3;
    float Xa3e2, Ya3e2, Xb3e2, Yb3e2, Xc3e2, Yc3e2;
    // 5 degree error values
    float Xa4e5, Ya4e5, Xb4e5, Yb4e5, Xc4e5, Yc4e5;
    float Xa5e5, Ya5e5, Xb5e5, Yb5e5, Xc5e5, Yc5e5;
    float	Xa4, Ya4, Xa5, Ya5;
    WidthOneSecond();
    //testonly
//    DisplayLedStatus();

    g_bTestDLs = TRUE;	//TestDrivingLEDs();

    //testonly
    //	451.10	310.44
    //	391.10	250.44
    //	494.58	209.09
    	//A

    	Xa = 451.10;
    	Ya = 310.44;
    	//B =
    	Xb = 391.10;
    	Yb = 250.44;
    	//C =
    	Xc = 494.58;
    	Yc = 209.09;


// Sensor Circle - Equispaced on circle of radius 600mm. - Test purposes only/error analysis.
    	Xa1 = 0.0;		//A
    	Ya1 = 0.0;
    	//B =
    	Xb1 = 259.8;	//B
    	Yb1 = 450.0;
    	//C =
    	Xc1 = 519.6;	//C
    	Yc1 = 0.0;

// Bubble coordinates for 330mm radius circle.  assuming sensors aligned correctly.
    	Xa2 = 116.9;		//A
    	Ya2 = 67.5;
    	//B =
    	Xb2 = 259.8;	//B
    	Yb2 = 315.0;
    	//C =
    	Xc2 = 402.7;	//C
    	Yc2 = 67.5;

// Bubble coordinates for 250mm radius circle.  assuming sensors aligned correctly.
		Xa3 = 151.6;		//A
		Ya3 = 87.5;
		//B =
		Xb3 = 259.8;	//B
		Yb3 = 275.0;
		//C =
		Xc3 = 368.1;	//C
		Yc3 = 87.5;
	// Bubble coordinates for 250mm radius circle.  with sensor  with +2 degrees anticlockwise..
			Xa3e2 = 154.7; // 151.6;		//A
			Ya3e2 = 82.3;  //87.5;
			//B =
			Xb3e2 = 253.7; //259.8;	//B
			Yb3e2 = 274.9; //275.0;
			//C =
			Xc3e2 = 371;   //368.1; ;	//C
			Yc3e2 = 92.9;  //87.5;

// Bubble coordinates for 330mm radius(D=660mm) circle with sensors at +5 degrees cw.

			//A
			Xa4 = 233.8; //
			Ya4 = 135.0;  //;

			Xa4e5 = 222.7; //
			Ya4e5 = 155.9;  //;
			//B =
			Xb4e5 = 543.3; //	//B
			Yb4e5 = 629.1; //
			//C =
			Xc4e5 = 792.8;   // ;	//C
			Yc4e5 = 114.1;  //

// Bubble coordinates for 250mm radius(D=500mm) circle with sensors at +5 degrees cw.
			//A
			Xa5	= 	303.1;
			Ya5 = 	175.0;

			Xa5e5 = 289.4; //
			Ya5e5 = 202.6;  //;
			//B =
			Xb5e5 = 550.4; //	//B
			Yb5e5 = 548.1; //
			//C =
			Xc5e5 = 719.1;   // ;	//C
			Yc5e5 = 147.9;  //


//TODO - testonly.
//wfh
#define	US_RUN_THEORY_TESTS	(0)
    	if(fdTelnet>0 && g_nTSMDebug.m_bUltrasonicsDebug && US_RUN_THEORY_TESTS)
    	{
			iprintf("\n Testonly stuff in FgOneHz.cpp\n");
					WidthCalculation3Points();
			float 	fRadt1 =  CalculateRadiusFrom3Points( Xa, Ya, Xb, Yb, Xc, Yc );
			iprintf("\n Sensors Circle:");
			float 	fRad1 =  CalculateRadiusFrom3Points( Xa1, Ya1, Xb1, Yb1, Xc1, Yc1 );
			iprintf("\n 330mm Circle:");
			float 	fRad2 =  CalculateRadiusFrom3Points( Xa2, Ya2, Xb2, Yb2, Xc2, Yc2 );
			iprintf("\n 250mm Circle:");
			float 	fRad3 =  CalculateRadiusFrom3Points( Xa3, Ya3, Xb3, Yb3, Xc3, Yc3 );

			iprintf("\n 250mm Circle - 2degrees sensor angles error all +2 ccw:");
			float 	fRad250e2ABC =  CalculateRadiusFrom3Points( Xa3e2, Ya3e2, Xb3e2, Yb3e2, Xc3e2, Yc3e2 );

			iprintf("\n 250mm Circle - 2degrees ccw sensor C angle error:");
			float 	fRad250e2C =  CalculateRadiusFrom3Points(  Xa3, Ya3, Xb3, Yb3, Xc3e2, Yc3e2 );

			iprintf("\n 330mm radius(D=660mm) circle err +5 degrees cw:");
			float 	fRad330e5C =  CalculateRadiusFrom3Points( Xa4e5, Ya4e5, Xb4e5, Yb4e5, Xc4e5, Yc4e5 );

			iprintf("\n 250mm radius(D=500mm) circle err +5 degrees cw:");
			float 	fRad250e5C =  CalculateRadiusFrom3Points( Xa5e5, Ya5e5, Xb5e5, Yb5e5, Xc5e5, Yc5e5 );

		//A bubble coord's correct.
			iprintf("\n 330mm radius(D=660mm) circle A-ok err +5 degrees cw:");
			float 	fRad330Ae5C =  CalculateRadiusFrom3Points( Xa4, Ya4, Xb4e5, Yb4e5, Xc4e5, Yc4e5 );

			iprintf("\n 250mm radius(D=500mm) circle A-OK err +5 degrees cw:");
			float 	fRad250Ae5C =  CalculateRadiusFrom3Points( Xa5, Ya5, Xb5e5, Yb5e5, Xc5e5, Yc5e5 );

			//Testing angle calculations
			iprintf("\n Angles of 3 4 5 triangle");
			//float  CalculateTriangleAngleA( float fAdj1, float fAdj2, float fOpp);
			float fA = CalculateTriangleAngleA( 3.0f, 5.0f, 4.0f);
			float fB = CalculateTriangleAngleA( 4.0f, 5.0f, 3.0f);
			float fC = CalculateTriangleAngleA( 3.0f, 4.0f, 5.0f);		//should be 90.
			printf(" fA = %5.2f fB = %5.2f fC = %5.2f ", fA, fB, fC);

    	}

    FGOneHzMBTCPErrorChecking();
    CheckForUnRestrictedAccessToLicening();
    if(g_nWatchDogCommand == ENABLE_WATCH_DOG_AND_REBOOT)
    {
    	if(g_nWatchDogCommandInNseconds > 0)
    	{
    		g_nWatchDogCommandInNseconds--;
    		if(g_nWatchDogCommandInNseconds == 0)
    		{
    			g_nWatchDogCommand = 0;
    			EnableWatchdogAndReboot();
    		}
    	}
    }
    else
    if(g_nWatchDogCommand == DISABLE_WATCH_DOG_AND_REBOOT)
    {
       if(g_nWatchDogCommandInNseconds > 0)
       {
           g_nWatchDogCommandInNseconds--;
    	   if(g_nWatchDogCommandInNseconds == 0)
    	   {
    	       g_nWatchDogCommand = 0;
    	       DisableWatchdogAndReboot();
    	   }
    	}

   }

    CopySecurityDataToModbus(); //security--tsm--
    if(g_nSHASecretFailCtr > 0) //security--tsm--
    {
 	   g_nSHASecretFailCtr++; //security--tsm--
    }
    CheckOptionsForValidLicense();
    if(g_CalibrationData.m_nTopUpCompNoRef != g_nTopUpCompNoRefHistory)
    {
        g_nTopUpCompNoRefHistory = g_CalibrationData.m_nTopUpCompNoRef;
        g_bPctFlag = TRUE; // set this flag directly,

    }

    EIPOneHzProgram();
    CheckIfPulsingCalibrateIsRequired();
    if(!g_bDiagnosticsPasswordInUse)
    g_bAutoCycleFlag = FALSE;                                                               // testonly //

    if(g_bRefAToDStabilityTestInProgress)
    {
        g_nRefTimeoutCtr++;
        if(g_nRefTimeoutCtr > ATOD_REFERENCE_TIMEOUT_SECONDS)
        {
            g_nRefTimeoutCtr = 0;
            g_bRefAToDStabilityTestInProgress = FALSE;   // switch back to load cell
            AnalogMux_Select_FANI();
        }
    }
    else
    {
        g_nRefTimeoutCtr = 0;
    }

    if(g_nDelayBeforeLoadCellLogCtr > 0)
    {
       g_nDelayBeforeLoadCellLogCtr--;
       if(g_nDelayBeforeLoadCellLogCtr == 0)
       {
           g_bLogLoadCellReading = TRUE;
       }
    }
    if(g_nDelayBeforeRefLogCtr > 0)
    {
       g_nDelayBeforeRefLogCtr--;
       if(g_nDelayBeforeRefLogCtr == 0)
       {
           g_bTakeRefAToDReading = TRUE;
       }
    }

    if(g_nAfterEndOfCycleCounter > 0)
    {
        g_nAfterEndOfCycleCounter--;
    }
    if(g_nHoldOffEventLogWriteCtr > 0)
    {
        g_nHoldOffEventLogWriteCtr--;
    }

   LoaderDiagnostics();
   VAC8OneHzPrograms();
   if(g_CurrentTime[TIME_HOUR] != g_wHourHistory )
    {
        g_bHourRollOver = TRUE;
        g_bActivateHourlyReport = TRUE;
        g_wHourHistory = g_CurrentTime[TIME_HOUR];
    }
    VacuumLoaderOneHertz();
    CheckForCorrectComponentTargets();
    CheckFor24HourRollOver();  // check for 24 hour roll over
    if(g_CalibrationData.m_bNoParity != g_bNoParityHistory)
    {
        g_bNoParityHistory = g_CalibrationData.m_bNoParity;
        InitialiseNetworkSerialPort();
    }

    g_bWriteToExpansion2 = TRUE;
    OneHzHistoryLog();
 /* nbb--todo--width
    if(g_CalibrationData.m_nDiagnosticsMode == 0)
    {
        if(g_lRawADCountsLTC2415 > MAXLTC2415ATODVALUE)
        {
            g_bAToDMaxExceeded = TRUE;
            PutAlarmTable( ATODMAXEXCEEDEDALARM,  0 );      // indicate a/d max exceeded alarm
            if(g_nPauseFlag == 0)
            {
                PauseOn();
                MakeCycle(); // terminate the cycle
            }
        }
        else
        {
            if(g_bAToDMaxExceeded)
            {
                g_bAToDMaxExceeded = FALSE;
                PauseOff();  // allow blender to move on.
                RemoveAlarmTable( ATODMAXEXCEEDEDALARM,  0 );     // Remove alarm
            }
        }
    }
*/
    if(g_nTelNetSecondsCounter != 0)
    {
        g_nTelNetSecondsCounter--;
    }
    ContinuousTelnetUpdate();

    if(!g_bTelnetSDCardAccessInProgress && g_bWriteDiagnosticsToSDCard) // nbb--todo-- check for cycle here
    {
        g_bWriteDiagnosticsToSDCard = FALSE;
        WriteDiagnosticDataToSDCard();
    }

    for(i=0; i < MAXIMUM_ALARMS; i++)
    {
        for(j=0; j < MAX_COMPONENTS; j++)
        {
            if(g_bAlarmOccurredHistory[i][j]) // check if any alarm is active
            {
                IsAlarm = TRUE;
            }
        }
    }
     if(IsAlarm)
     {
         g_arrnMBTable[BATCH_SUMMARY_PROCESS_ANY_ALARM_OCCURRED] = 0x01;
     }
     else
     {
         g_arrnMBTable[BATCH_SUMMARY_PROCESS_ANY_ALARM_OCCURRED] = 0x00;
     }


    g_nTemperature = ReadTempSensor();
    CheckIfMaterialHasDumped();
    if(g_CalibrationData.m_bLiquidEnabled)
    {
        WriteMAX5812DAC(g_nLiquidAdditiveOutputDAValue);//nbb--todolp-- review this
    }
    else
    {
        WriteMAX5812DAC(g_nExtruderDAValue);
    }

    LiquidAdditiveOnehz();
    if(!g_bCycleIndicate && g_bWriteEventLogToSdCard)
    {
        g_bWriteEventLogToSdCard = FALSE;
        if((g_CalibrationData.m_nLogToSDCard & LOG_EVENTS_TO_SD_CARD) != 0)
    	{
    		WriteToSdCardFile(g_cEventName,g_cEventLogFile);
    	}

        DumpEvent();
//        if( fdTelnet > 0)
//        {
//        iprintf("\nfg string name is %s  string is %s ",g_cEventName,g_cEventLogFile);
//        }
        g_cEventLogFile[0] = '\0';  // set string back to the beginning

    }
    if(g_nDisableSDCardAccessCtr != 0)
    {
        g_nDisableSDCardAccessCtr--;
    }

    if(g_CalibrationData.m_bTelnet)
    {
        if(!g_bTelnetHistory) // was it disabled beforehand
        {
            StartTelnet();
            g_bTelnetHistory = g_CalibrationData.m_bTelnet;
        }
    }

    if(g_bSDCardSoftwareUpdateComplete)
    {
        g_nSDCardSoftwareUpdateComplete++;
        if(g_nSDCardSoftwareUpdateComplete >= SD_CARD_OPERATION_SHOWN_TIME_SECONDS)
        {
            g_bSDCardSoftwareUpdateComplete = FALSE;
            g_nSDCardSoftwareUpdateComplete = 0;
        }
    }
    if(g_bSDCardLoadConfigComplete)
    {
        g_nSDCardLoadConfigComplete++;
        if(g_nSDCardLoadConfigComplete >= SD_CARD_OPERATION_SHOWN_TIME_SECONDS)
        {
            g_bSDCardLoadConfigComplete = FALSE;
            g_nSDCardLoadConfigComplete = 0;
            AddSDCardLoadConfigCompleteToEventLog();
        }
    }

    if(g_bSDCardLoadBackupConfigComplete)
    {
        g_nSDCardLoadBackupConfigComplete++;
        if(g_nSDCardLoadBackupConfigComplete >= SD_CARD_OPERATION_SHOWN_TIME_SECONDS)
        {
            g_bSDCardLoadBackupConfigComplete = FALSE;
            g_nSDCardLoadBackupConfigComplete = 0;
            AddSDCardLoadBackupConfigCompleteToEventLog();
        }
    }
    if(g_bSDCardSaveConfigComplete)
    {
        g_nSDCardSaveConfigComplete++;
        if(g_nSDCardSaveConfigComplete >= SD_CARD_OPERATION_SHOWN_TIME_SECONDS)
        {
            g_bSDCardSaveConfigComplete = FALSE;
            g_nSDCardSaveConfigComplete = 0;
            AddSDCardSaveConfigCompleteToEventLog();
        }
    }
    if(g_bSDCardCopyConfigToBackupComplete)
    {
        g_nSDCardCopyConfigToBackupComplete++;
        if(g_nSDCardCopyConfigToBackupComplete >= SD_CARD_OPERATION_SHOWN_TIME_SECONDS)
        {
            g_bSDCardCopyConfigToBackupComplete = FALSE;
            g_nSDCardCopyConfigToBackupComplete = 0;
            AddSDCardCopyConfigToBackupCompleteToEventLog();
        }
    }


    if( g_CalibrationData.m_nControlType == CONTROL_TYPE_INC_DEC) // asm = INCREASEDECREASE
    {
        g_bSEIIncDecSetpointDownload = TRUE;
    }
    CheckForAChangeInSEICalibrationData();
    if( g_CalibrationData.m_nControlType == CONTROL_TYPE_INC_DEC) // asm = INCREASEDECREASE
    {
        g_bCCVsFlag = TRUE; // ensure that calculation is done
    }

     if(g_bSEIReadDataReady)
     {
          g_bSEIReadDataReady = FALSE;  //nbb--todolp--remove this
     }



      if(g_CalibrationData.m_bFillOrderManualSelect)  // asm = MANUALFILLSORT
      {
          g_bShowInstantaneousWeight = FALSE;  //
      }
      else
      {
          g_bShowInstantaneousWeight = TRUE;  // SHOWINWEIGHT - set flag to show intantan. wt.
      }


    if(g_nHoldOffLevelSensorAlarmSecondCounter != 0)
    {
        g_nHoldOffLevelSensorAlarmSecondCounter--;
    }

    if(g_nWriteHoldOffCounter != 0)
    {
        g_nWriteHoldOffCounter--;
    }

    if(g_nVolumetricEstimateCounter != 0)
     {
        g_nVolumetricEstimateCounter --;
        if(g_nVolumetricEstimateCounter == 0)
        {
            g_bRunEst = TRUE;  // estimation of kg/h and grams per meter
        }
    }

  if(g_nAllowReadOfControlInputsCtr != 0)
    {
        g_nAllowReadOfControlInputsCtr --;
        if(g_nAllowReadOfControlInputsCtr == 0)
        {
                g_arrnWriteSEIMBTable[MB_SEI_COMMAND] &= 0xFF00;
        }
    }

    if(g_nOverrideCounter !=0)
    {
        g_nOverrideCounter--;
        if(g_nOverrideCounter == 0)
        {
            g_bOverrideClean = FALSE;
        }
    }

    CheckForScrewSpeedChange();
    LoadSetPointDataFromModbusTable();
    EstimateLoaderWeight();
    DecideIfExpansionisRequired();
    CheckForRecipeDownLoad();

    MultiBlendCycle();
    //Reading the Real time clock.
    if(!g_CalibrationData.m_bPCF8563RTCFitted)
    {
    	IOBoardRTCGetTime( bts );
    }
    else
    {
    	PCF8563RTCGetTime( bts );
    }

    if( g_nLevelSensorDelayCtr != 0 )
    {
        g_nLevelSensorDelayCtr--;   //  Level sensor delay counter
        if((g_nLevelSensorDelayCtr == 0) && g_bLevelSensorAlarmOccurred)
        {
            if( (g_arrnMBTable[BATCH_SUMMARY_PROCESS_ALARMS] & LEVELSENALBIT) == 0 )
            {  //
                PutAlarmTable( LEVELSENSORALARM,  0 );      // indicate alarm, Component no. is zero. //PTALTB
            }
        }
    }
// CHECKFORDIAGNOSTICSPASSWORD:
    if( g_CalibrationData.m_lManagerPassWord == 6553 )
        g_bDiagnosticsPasswordInUse = TRUE;         // ISDIAGNOSTICSPASSWORD
    else
        g_bDiagnosticsPasswordInUse = FALSE;

//NOLSALRM
    if( g_nCheckLSensorInNSeconds ) // CHECKLSENSORINNSECONDS
    {
        g_nCheckLSensorInNSeconds--;
        if( g_nCheckLSensorInNSeconds  == 0 )   // if reach 0, set flag to run l.s. check.
            g_bRunLevelSensorCheck = TRUE;      //  asm = CHECKFORMAXTHP
    }
// NOUPR
//Check for Bypass valve activation
        CheckForBypassActivation();

        if( g_nPrintInNSeconds )
        {
            g_nPrintInNSeconds--;
            if( !g_nPrintInNSeconds)    // reached 0???
            btest =  PrintCycleDiagnostics();    // Start print of  batch cycle diagnostics.
//              g_bPrintNow = TRUE;     // set flag to trigger printing.
        }
// Increase-Decrease mode of control.
    if( g_CalibrationData.m_nControlType == CONTROL_TYPE_INC_DEC) // asm = INCREASEDECREASE
    {
        if( g_nDoIncDecReadInNSeconds )     // DOREADINNSECONDS
        {
            g_nDoIncDecReadInNSeconds--;
            if( g_nDoIncDecReadInNSeconds == 0 )
                g_bREadIncDecCalibrationData = TRUE;    // READINCDECCAL - read calibration from SEI.
        }
    }

// asm = NOIDRD
    if( g_bCheckIfShouldCycleFlag ) // CHKIFCYCLEFLAG    ; CHECK IF THE BLENDER SHOULD CYCLE
    {
        g_nCheckIfShouldCycleCounter++;
        if( g_nCheckIfShouldCycleCounter >= SET_TO_CYCLE )  // SECTOCYCLE    ; NO OF SECONDS TO CYCLE.
        {//nbb--todolp--kwh if covered then no cycle
             if( g_bStopped || g_nWarmUpCounter || g_nPauseFlag || g_bLevSen1Stat == COVERED || g_fOneSecondHopperWeight > ZERO_WEIGHT )
            {
                g_bCheckIfShouldCycleFlag = FALSE;
                g_nCheckIfShouldCycleCounter = 0;
            }
            else    // hopper is empty, cause to cycle.
            {
                g_bDmpLevSen = TRUE;        // Make the blender cycle.
                g_bCheckIfShouldCycleFlag = FALSE;
                g_nCheckIfShouldCycleCounter = 0;
            }
        }

    }

    if(!g_bCycleIndicate && (g_nResetAllFlowRates == FLOWRATERESETID))
    {
            g_nResetAllFlowRates = 0;  // reset flag



//nbb--todo-- is it necessary to reset the flow rates to zero here, should only set units to fill by weight
            for(i= 0; i < MAX_COMPONENTS; i++)
            {
                g_ffComponentCPI[i] = 0.0f;
                g_ffComponentCPIStage2[i] =  0.0f;
                g_ffComponentCPIStage3[i] =  0.0f;
                g_ProcessData.m_ffComponentCPI[i] =  g_ffComponentCPI[i];
                g_ProcessData.m_ffComponentCPIStage2[i] = g_ffComponentCPIStage2[i];
                g_ProcessData.m_ffComponentCPIStage3[i] = g_ffComponentCPIStage3[i];
                g_MainCPIRRData.m_ComponentCPIRRData[i].m_bIgnoreCPIRR = FALSE;
            }


            g_bSaveAllCalibrationToEEprom = TRUE;

            j = 0;
            while(g_cSeqTable[j] != SEQDELIMITER && j < MAX_COMPONENTS) // SEQDELIMITER indicates table end
            {
                 g_cSeqTable[j] |= FILLBYWEIGHT;    //asm = SETALLCOMPTOFILLBYWEIGHT
                 g_cSeqTable[j] |= LEAVEFILLBYWEIGHT;    //asm = SETALLCOMPTOFILLBYWEIGHT  nbb--todo-- is this necessary
                 j++;
            }

            ResetRoundRobinFlowRateForAllComponents();
    }


//

// g_nMBSlaveCommsWatchDogTimer now used instead of RXCOMMSWATCHDOG, and handled on PIT directly.

//  g_nDeviceToPollCounter = 0;       // DEVICETOPOLLCTR and handled on PIT directly.

    CheckHighLowSensors();              // CHKHILOSENSORS          ; CEHCK FOR HIGH LOW SENSORS.
    CheckHighLowRegrindSensors();               // CHKHILOREGSENSORS          ; CEHCK FOR HIGH LOW SENSORS.
    if(g_bUpdateHiLoFlag)               //UPDATEREGHILOF          ; UPDATE REGHILO
    {
        UpdateRegrindHiLoPercentage();              // UPDATEREGHILO
        g_bUpdateHiLoFlag = FALSE;
    }

//NOREGU
        CheckForSEITimeOuts();          // JSR     CHKFORSEITO             ; SEI TIMEOUT
    // stuff relating to setting flags for polling SEI data areas kept on PIT 1 second.
    // PITCNTR was not being used for anything is asm.
    // TENCNT was not being used for anything is asm.
    if(g_bHourFlag) // HOURFG
    {
        g_bHourFlag = FALSE;
        g_ProcessData.m_lHoursBlenderIsRunning++;  // increment hour running counter
        GenerateHashForHistoryLog();
        g_bSaveAllProcessDataToEEprom = TRUE;
        g_bAllowProcesDataSaveToEEPROM = TRUE;
        CheckTemporaryHourlyCounter();   // asm CHECKFORTEMPOPTIONCHECK CHECK FOR TEMPORARY OPTION
        DecideOnLicenseExpiryWarning(); // asm DECIDEONEXPIRYPAGE
    }
    if(g_bHourRollOver)
    {
    	WidthHourFG();
        g_bHourRollOver = FALSE;
        CopyOngoingToHourlyHistoryLog();
    }

 if(!g_bCycleIndicate)  //if not cycling
 {
    CopyCycleDiagnosticsToMB( );        // copy cycle diagnostics into MB table.
 }

    CheckForAlarm();            // checks for alarm(s) and switches on/off the alarm outputs accordingly.
    CopyAlarmTableToMB();   // copies alarm table into modbus.
    ExtruderStallCheck();   // has extruder stalled ?
    CheckLicenseForFlashing();
    CheckForPendingRecipe(); // check for pending recipe in single recipe mode asm CHECKFORPENDINGRECIPE



    if(g_nWarmUpCounter) //            // ASM WARMUPCTR
    {
        g_nWarmUpCounter--;            // a/d warm up counter decremented
        g_nStartUpPagePauseTimer = 0;  // ASM PAUSETIMER leave start up page  --review-- if this is required ?
    }

    if(g_nBlenderReadyCounter)          // WRMUPO ASM BLENDERREADYCTR
    {
        g_nBlenderReadyCounter--;       // decrement blender ready counter - allows hi/lo sensors to be read on reset
                                        // used in bin presence check.
    }

    if(g_bMixOff)
    {                                   // BLRDY
        g_nMixerOnTime = 0;             // mixer off reset counter  ASM MIXERONTIME
    }
    else   // MIXN
    {
        if(g_nMixerOnTime != 0xffff)    // if not at limit increment --review-- if this is necessary ?
        {
            g_nMixerOnTime++;
        }
    }
    StorePowerDownTime(); // ASM  STRPDNT

    if(g_CalibrationData.m_nBlenderMode == MODE_SINGLERECIPEMODE)
    {
          unTemp = ConvertOutputToLogicalFormat(g_nVacuumWriteData);
          g_arrnWriteLLSMBTable[MB_LLS_COMMAND] = unTemp;     //
    }
    else
    {
        unTemp = ConvertOutputToLogicalFormat(g_MainAlarmTable.m_nComponentLLSAlarmWord);
        g_nLowLevelSensorAlarmWord = unTemp;
        g_arrnWriteLLSMBTable[MB_LLS_COMMAND] = g_nLowLevelSensorAlarmWord;     //
    }
    IntegrateCommsLineSpeed();

    DecideIfLoaderShouldBeEnabled();
}

/*
NOREGU  JSR     CHKFORSEITO             ; SEI TIMEOUT
        LDAA    #1
        STAA    SEIPOLLINPUTSF          ; START POLLING OF INPUTS.
        STAA    WRITEDAVALUEF           ; INITIATE D/A WRITE EVERY SECOND.
        STAA    SEIPOLLLLSF             ; SET LOW LEVEL SENSOR
        STAA    WRITEALARMSF
        LDAB    CRLMODE                 ; BLENDER MODE.
        CMPB    #VOLTAGEFOLLOWER        ; VOLTAGE FOLLOWER MODE.
        BNE     NOVF                    ; NO.
        TST     MANATO                  ; AUTO MODE
        BNE     NOVF                    ; NO READ IF IN AUTO.
        STAA    READDAVALUEF
NOVF:

*/


/*

        JSR     CHKHILOREGSENSORS
        TST     UPDATEREGHILOF          ; UPDATE REGHILO
        BEQ     NOREGU                  ; NO
        JSR     UPDATEREGHILO         ; NO CHECK FOR CYCLE.
        CLR     UPDATEREGHILOF          ; RESET FLAG.
NOREGU  JSR     CHKFORSEITO             ; SEI TIMEOUT

NOIDRD  TST     CHKIFCYCLEFLAG          ; CHECK IF THE BLENDER SHOULD CYCLE.
        BEQ     NOCYC                   ; NO CHECK OF CYCLE.
        INC     CHKIFCYCLECTR
        LDAA    CHKIFCYCLECTR           ;
        CMPA    #SECTOCYCLE             ; NO OF SECONDS TO CYCLE.
        BLO     NOCYC                   ; NO CYCLE CHECK.
        TST     STOPPED                 ;
        BNE     NOCYCRS                 ; SENSOR OFF SYSTEM WILL CYCLE ANYWAY
        TST     LEVSEN1STAT
        BNE     NOCYCRS                 ; SENSOR OFF SYSTEM WILL CYCLE ANYWAY
        TST     HOPPERWEIGHT
        BNE     NOCYCRS                 ; SENSOR OFF SYSTEM WILL CYCLE ANYWAY
        LDD     HOPPERWEIGHT+1
        CPD     #ZEROWEIGHT
        BHI     NOCYCRS                 ; NO CYCLE RESET.
        LDAA    #$0F
        STAA    DMPLEVSEN               ; MAKE THE BLENDER CYCLE.
NOCYCRS CLR     CHKIFCYCLEFLAG
        CLR     CHKIFCYCLECTR

NOCYC   JSR     CHECKFORRRWEIGHTDISPLAY ; SET OR CLEAR FLAG TO ALLOW DISPLAY OF RR DATA
CHECKFORRRWEIGHTDISPLAY:
        LDAA    #1
        STAA    SHOWINWEIGHT            ;
XITPWC  RTS


*/

/////////////////////////////////////////////////////
// CheckForBypassActivation()                       from ASM = CHECKFORBYPASSACTIVATION
//
// Logic is as follows:
//      High level sensor input used for bypass sensor.  If sensor becomes uncovered for greater than
//      the bypass time limit, the bypass valve is opened.
//      The "bypass" sensor is above the entry point for the bypass material in the throat.
//
// Michael McKiernan                        26.11.2004
// First Pass.
//////////////////////////////////////////////////////
void CheckForBypassActivation( void )
{
int nTimeLimit;

    if( g_CalibrationData.m_bBypassMode )   // bypass mode enabled
    {
        if(g_nCleanCycleSeq != 0)
        {
            nTimeLimit = g_CalibrationData.m_nBypassTimeLimitClean;
        }
        else
        {
            nTimeLimit = g_CalibrationData.m_nBypassTimeLimit;
        }


        if(g_bHighLevelSensor == COVERED)
        {
            g_nBypassSensorUncoveredCounter = 0;    // BYPASSSENSOROFFCTR
            CloseBypass();                                  // CLOSE BYPASS valve.
        }
        else    // bypass sensor uncovered.
        {
            g_nBypassSensorUncoveredCounter++;  // BYPASSSENSOROFFCTR  by pass sensor off counter
            if( g_nBypassSensorUncoveredCounter > nTimeLimit )
            {
                OpenBypass();                                   // Open BYPASS valve.
                g_nBypassSensorUncoveredCounter = 0;    // BYPASSSENSOROFFCTR
            }

        }
    }
    else // BYPASNON    bypass mode disabled
    {
        g_nBypassSensorUncoveredCounter = 0;    // BYPASSSENSOROFFCTR
        CloseBypass();                          // CLOSE BYPASS valve.
    }

}

/////////////////////////////////////////////////////
// CheckHighLowSensors()                        from ASM = CHKHILOSENSORS
//
// Called once per second.
//
// Check the high/low level sensors if off-line mode, and offline mode type = hi-lo
///
// Michael McKiernan                        26.11.2004
// First Pass.
//////////////////////////////////////////////////////
void CheckHighLowSensors( void )
{
    if( g_CalibrationData.m_nBlenderMode == MODE_OFFLINEMODE  && g_CalibrationData.m_nOfflineType == OFFLINETYPE_HILO)
    {
        if(g_bHighLevelSensor == COVERED)
        {
                if(g_bHighLevelSensorHistory == UNCOVERED)
                {
                    //JSR STARTOFFLINEDELAY (expanded inline)
                    CloseFeed();    // Close pinch valve in throat.
                    g_nOffline_Tracking = BATSEQ_OFFDELADMPID;  // OFFLINE_TRK = OFFDELADMPID
                    g_nOffTime1 = 0;
                    g_nDmpSec = 0;          // 1/10 sec counter.
                    g_nPauseInNSeconds = 5;    // PAUSEINNSECONDS - will start pause.
                }
                g_bHighLevelSensorHistory = COVERED;
//              g_nCyclePauseType = OFFLINE_END_OF_CYCLE_PAUSE_TYPE;    // SET PAUSE TYPE.

        }
        else // high sensor uncovered
        {
         g_bHighLevelSensorHistory = UNCOVERED;
         if( g_bLowLevelSensor == UNCOVERED )
         {
            // no pause off if pause generated from keyboard etc, i.e. only if by offline sensors.
            if( g_nCyclePauseType != END_OF_CYCLE_PAUSE_TYPE && g_nCyclePauseType != IMMEDIATE_PAUSE_TYPE )
            {
               if( (g_nPauseFlag & NORMAL_PAUSE_BIT) == 0 ) // check normal pause bit not set!!
               {
                  if((g_nPauseFlag & OFFLINE_END_OF_CYCLE_PAUSE_BIT) != 0)
                  {
                      SwitchPauseOffAndMakeCycle();  //
                  }
               }
            }
         }
        }
    }
}



/////////////////////////////////////////////////////
// UpdateRegrindHiLoPercentage()                        from ASM = UPDATEREGHILO
//
// Called once per second.
//
// Updates the regrind % based on hi-lo level sensors.  If the high sensor is covered
// the High Regind % gets used.
// When the low sensor becomes uncovered, it reverts back to the original regrind %.
///
// Michael McKiernan                        26.11.2004
// First Pass.
//////////////////////////////////////////////////////
void UpdateRegrindHiLoPercentage( void )
{

int nRegCompIndex = g_CurrentRecipe.m_nRegrindComponent - 1;    //Regrind component index.

    if( g_CalibrationData.m_bHighLoRegrindOption && g_bRegrindPresent )
    {
        if(g_CalibrationData.m_nBlenderMode != MODE_OFFLINEMODE || g_CalibrationData.m_nOfflineType != OFFLINETYPE_HILO )
        {
            //CONCHK check for change in level sensor status.
            if( g_bLowLevelSensor == COVERED )
            {
                // CHKHIGH
                if( g_bHiRegLevelSensor == COVERED )
                {
                    // Regrind comp % set to HIREGPCNT
                    g_CurrentRecipe.m_fPercentage[nRegCompIndex] = g_CurrentRecipe.m_fPercentageHighRegrind;
                    g_bPctFlag = TRUE;

                }

            }
            else    // low level sensor  uncovered - revert back to original regrind %.
            {
//              USEOLD - Handled differently in C.  The "old" or origninal regrind % is always stored in recipe
//                          in the m_fPercentageRegrindOriginal location.  Must be stored when recipe is loaded.
                // Regrind comp % set to original value from recipe.
                g_CurrentRecipe.m_fPercentage[nRegCompIndex] = g_CurrentRecipe.m_fPercentageRegrindOriginal;
                g_bPctFlag = TRUE;
               // ISOLD was being set - but it is not used anywhere --REVIEW--
            }
        }
    }
}

/////////////////////////////////////////////////////
// ForegroundOneHzProgram2()                        from ASM = part of PIT10HZ, moving from 10Hz part ofPIT.
//
// NB: This was called on PIT interrupt in assembler and run on 10Hz when second reached.
// stuff between "LDD DA1TMP" and ARI001EXIT, minus the one minute section see ForegroundOneMinuteProgram().
//
//
// Michael McKiernan                        01.12.2004
// First Pass.
//////////////////////////////////////////////////////
void ForegroundOneHzProgram2( void )
{
    // Check for Startup(DA1TMP) had been commented out in assembler.
int nSecs;

    if( (g_CalibrationData.m_nBlenderMode != MODE_BLENDINGONLY) && (g_CalibrationData.m_nBlenderMode != MODE_OFFLINEMODE) && (g_CalibrationData.m_nBlenderMode != MODE_THROUGHPUTMON))
    {
        if( g_CalibrationData.m_bShutDownFlag == ENABLED )
        {
            g_nShutDownCtr++;
            nSecs = g_CalibrationData.m_nShutDownTime * 60; //shutdown time is in minutes.
            if( g_nShutDownCtr > nSecs )
            {
                g_nShutDownCtr = 0; //reset the counter
                g_nExtruderDAValue = 0; // DA1TMP.  - Shut down the extruder, setpoint voltage -> 0.
            if( (g_arrnMBTable[BATCH_SUMMARY_PROCESS_ALARMS] & SHUTDOWNALARMBIT) == 0 ) // shut down alarm bit not set???
                {  //
                                PutAlarmTable( SHUTDOWNALARM,  0 );     // indicate alarm, Component no. is zero.
                }

            }
        }
    }
//asm = NOSHUT:
// From NOTATPRESET: - NOT1MIN happens once per minute and is in ForegroundOneMinuteProgram()
// asm = ARI001:
    if( g_nMixerTransitionCounter == 0 )        // MIXERTRANSITIONCTR
    {
        CheckMixerContactor();              // CHKMIXCONT
    }
// asm = NOALCH
    g_nFillCounter++;
    g_nOffTime1++;      // OFFTIM1  ; OFF LINE SECOND COUNTER.

    g_nSecondCounter++;     // SECCNT
    if( g_nSecondCounter & 0x0001)  // checking for odd/even second number
    {
        g_bTwoSecondFlag = TRUE;    // TWOSFLG  - indicate 2 seconds rollover.
    }

    //SECFLG - Was being set but never used.

    g_nTenSecondCounter++;      // TSCCNT
    if( (g_nTenSecondCounter == 10) || ( g_nTenSecondCounter == 20 ))
    {
        g_bTenSecondFlag = TRUE;    // TSCFLG            ;~10 SEC. FLAG
        if ( g_nTenSecondCounter == 20 )
        {
            g_nTenSecondCounter = 0;
 //           AnalyseRotationDetector();
        }
    }

    if( g_nToStallTime )                    // TOSTALTIM
        g_nToStallTime--;                   // "to stall time" - extruder stall check.

    DecrementRollChangeLags();          // DECRCLG

}

/////////////////////////////////////////////////////
// ForegroundOneMinuteProgram()     from ASM = part of PIT10HZ, moving from 10Hz part ofPIT.
//
// NB: This was called on PIT interrupt in assembler and run on 10Hz when minute reached.
// stuff between "NOTATPRESET: and NOT1MIN:
//
//
// Michael McKiernan                        01.12.2004
// First Pass.
//////////////////////////////////////////////////////
void ForegroundOneMinuteProgram( void )
{
	WidthFGOneMinute();
    if(g_nStorageInvervalCtr != 0)
    {
        g_nStorageInvervalCtr--;
    }
    if(g_bManualAutoFlag == AUTO)       // are we in auto??
    {
        g_sOngoingHistoryEventsLogData.m_bAutoState = TRUE;
        g_nTimeInAutoMinutesCtr++;
    }
    else
    {
        g_sOngoingHistoryEventsLogData.m_bAutoState = FALSE;
        g_nTimeInAutoMinutesCtr = 0;
    }
    CheckForTelNetTimeout();          // added check for telnet timeout.
    CheckForSDCardMaintainenance();   // maintain sd card if month has changed
    g_nDisplayUpdateCounter++;
    if( g_nDisplayUpdateCounter == UPDATE_TIME_MINS)
    {
        g_nDisplayUpdateCounter = 0;
        g_bUpdateDisplayNow = TRUE;
    }

    g_nNoCycle++;       // NOCYCLE -- minute no. in the cycle
    if( g_nNoCycle > g_CalibrationData.m_nMixShutoffTime)   // MIXINGSHUTOFF shut off time in minutes.
    {
        g_nNoCycle = 0;     // reset minutes counter.
        ClearThroughputTotals();    // CLEARTHROUGHPUTTOTALS
        g_bMixOff = TRUE;
    }

//asm = LEAVEON
   g_cLS1OffCt++;
   if( g_cLS1OffCt > g_CalibrationData.m_nMixShutoffTime ) // asm MIXINGSHUTOFF nbb--todo-- is this duplicated ?
   {
      g_cLS1OffCt = 0;
      g_bMixOff = TRUE;
   }
//asm = LEAVONN
//asm = SKMXCL
        g_cLS1OffCt++;
        if( g_cLS1OffCt > g_CalibrationData.m_nMixShutoffTime )  // asm MIXINGSHUTOFF
        {
            g_cLS1OffCt = 0;
            g_bMixOff = TRUE;
        }

}

/////////////////////////////////////////////////////
// DecrementRollChangeLags()        from ASM = DECRCLG
//
//  Is a subroutine to decrement debounce lag after reel chg
//  Called on the one second, but in foreground.
//  Funtion is to prevent erroneous roll change signals.
//
// Michael McKiernan                        02.12.2004
// First Pass.
//////////////////////////////////////////////////////
void DecrementRollChangeLags( void )
{
    if( g_nFrontRollChangeLag ) // RCF_LAG  -Front reel change lag
        g_nFrontRollChangeLag--;

    if( g_nBackRollChangeLag )      // RCB_LAG  - Back reel change lag
        g_nBackRollChangeLag--;

}
/*
;*  DECRCLG  IS A SUBROUTINE TO DECREMENT DEBOUNCE LAG AFTER REEL CHG
;            IT IS CALLED ON THE ONE SECOND INTERRUPT.
DECRCLG TST   RCF_LAG                   ;~FRONT REEL CHANGE LAG
        BEQ   DECRCL2
        DEC   RCF_LAG                   ;~DECREMENT LAG IF NON-ZERO
DECRCL2 TST   RCB_LAG                   ;~BACK REEL CHANGE LAG
        BEQ   DECRCLX
        DEC   RCB_LAG
DECRCLX RTS

*/

/////////////////////////////////////////////////////
// AnalyseRotationDetector()        from ASM = ANALYSEROTDETECTOR
//
//  Analyse rotation detector
//  Called every 10 seconds in foreground.
//
//
// Michael McKiernan                        02.12.2004
// First Pass.
//////////////////////////////////////////////////////
void AnalyseRotationDetector( void )
{
//MIXSCREWPULSECTR   READ MIXING SCREW PULSE COUNTER  >MINROTATIONCOUNT MINIMUM ROTATION COUNT IN 10 SECONDS
    if( g_nMixScrewPulseCounter > MIN_ROTATION_PULSES_IN_10S )
    {
        g_bMixScrewRotationAlarm = FALSE;   // MIXSCREWROTALARM
    }
    else
    {
        g_bMixScrewRotationAlarm = TRUE;    // Mixing screw rotation alarm set
    }

    g_nMixScrewPulseCounter = 0;
}
/*
;*****************************************************************************;
; FUNCTION NAME : ANALYSEROTDETECTOR                                          ;                                      ;
; FUNCTION      : ANALYSE ROTATION DETECTOR                                   ;
; INPUTS        : CALLED EVERY 10 SECONDS                                     ;                 ;                       ;
;*****************************************************************************;

ANALYSEROTDETECTOR:
       LDD      MIXSCREWPULSECTR        ; READ MIXING SCREW PULSE COUNTER
       CPD      #MINROTATIONCOUNT       ; MINIMUM ROTATION COUNT IN 10 SECONDS
       BHI      SCROT                   ; SCREW IS ROTATING
       LDAA     #1
       STAA     MIXSCREWROTALARM        ; MIXING SCREW ROTATION ALARM SET
       BRA      XITSCA                  ; EXIT ANALYSIS
SCROT  CLR      MIXSCREWROTALARM        ; SCREW ROTATING OKAY
XITSCA CLRW     MIXSCREWPULSECTR        ;
       RTS


*/


/////////////////////////////////////////////////////
// CheckMixerContactor()        from ASM = CHKMIXCONT
//
// checks for mixer contactor alarm
//
// P.Smith
//////////////////////////////////////////////////////
void CheckMixerContactor( void )
{
    if(!g_bStopped & g_bMixOnStatus) // not stopped
    {
//        if(g_CalibrationData.m_bMixerScrewDetect)   // asm = MIXSCREWDETECTION.
        if(0)   // asm = MIXSCREWDETECTION. //remove mixing screw detection code for now
        {
            if(g_bMixScrewRotationAlarm)
            {
                if( (g_arrnMBTable[BATCH_SUMMARY_PROCESS_ALARMS] & MIXSTOPALARMBIT) == 0 ) // bit not set???
                {  //
                    g_sOngoingHistoryEventsLogData.m_nMotorTrips++;
                    g_ProcessData.m_n24HourOngoingMotorTripCounter++;
                    g_bSaveAllProcessDataToEEprom = TRUE;
                    g_bAllowProcesDataSaveToEEPROM = TRUE;
                    PutAlarmTable( MIXSTOPALARM,  0 );     // indicate alarm
                }
            }
            else
            {
                CheckIfTripped();
            }
        }
        else
        {
            CheckIfTripped();
        }
    }
}




/////////////////////////////////////////////////////
// CheckIfTripped()        from ASM NOMIXDT
//
// sets / clears alarm depending on tripped status
//
// P.Smith
//////////////////////////////////////////////////////



void CheckIfTripped( void )
{
    if(g_bMixerTripped)
    {
        if( (g_arrnMBTable[BATCH_SUMMARY_PROCESS_ALARMS] & MIXSTOPALARMBIT) == 0 ) // bit not set???
        {  //
            g_sOngoingHistoryEventsLogData.m_nMotorTrips++;
            g_ProcessData.m_n24HourOngoingMotorTripCounter++;
            g_bSaveAllProcessDataToEEprom = TRUE;
            g_bAllowProcesDataSaveToEEPROM = TRUE;
            PutAlarmTable( MIXSTOPALARM,  0 );     // indicate alarm
        }
    }
    else  // not tripped
    {
        if( (g_arrnMBTable[BATCH_SUMMARY_PROCESS_ALARMS] & MIXSTOPALARMBIT) == 1 ) // tare alarm bit not set???
        {  //
                RemoveAlarmTable( MIXSTOPALARM,  0 );     // Remove alarm
        }
    }

}


/////////////////////////////////////////////////////
// CheckHighLowRegrindSensors()        from ASM CHKHILOREGSENSORS
//
// Check for high low regrind
//
// P.Smith
//////////////////////////////////////////////////////


void CheckHighLowRegrindSensors( void )
{
    if( !g_bCycleIndicate && (g_CalibrationData.m_nBlenderMode != MODE_OFFLINEMODE) )
    {
        if( g_nHiLowOptionSwitchedOnOffHistory == HILOTRANSITIONON_TO_OFF )    // has the hi/lo regrind option been switched off
        {
//          ASM = USEOLD
            memcpy(&g_CurrentRecipe,&g_TempRecipe, sizeof( g_TempRecipe ) );
            g_bPctFlag = TRUE;   // indicate that % set points have changed

            g_nHiLowOptionSwitchedOnOffHistory = 0;
        }
        else
        if( g_nHiLowOptionSwitchedOnOffHistory == HILOTRANSITIONOFF_TO_ON )    // has the hi/lo regrind option been switched on
        {
            UpdateRegrindHighLow();     // review - put
            g_nHiLowOptionSwitchedOnOffHistory = 0;

        }
        else
        {

            if( g_bHiRegLevelSensor != g_bHiRegLevelSensorHistory )     // HIGHLEVELSENSOR.
            {
                g_bHiRegLevelSensorHistory = g_bHiRegLevelSensor;

                if(g_CalibrationData.m_bHighLoRegrindOption)
                {
                    UpdateRegrindHighLow();     // check change in level sensor status
                }
            }
            else
            {
                if( g_bLowLevelSensor != g_bLowLevelSensorHistory )
                {
                    g_bLowLevelSensorHistory = g_bLowLevelSensor;
                    if(g_CalibrationData.m_bHighLoRegrindOption)
                    {
                       UpdateRegrindHighLow();     // check change in level sensor status
                    }

                }

            }

        }

    }

}


/////////////////////////////////////////////////////
// UpdateRegrindHighLow()        from UPDATEREGHILONOC
//
// Check for high low regrind
//
// P.Smith
//////////////////////////////////////////////////////

void UpdateRegrindHighLow( void )
{
int nIndex;

    if( g_bRegrindPresent )
    {
        if( !(g_CalibrationData.m_nBlenderMode == MODE_OFFLINEMODE  && g_CalibrationData.m_nOfflineType == OFFLINETYPE_HILO))
        {
            if( g_bLowLevelSensor == UNCOVERED )
            {
//          ASM = USEOLD

                memcpy(&g_CurrentRecipe,&g_TempRecipe, sizeof( g_TempRecipe ) );
                g_bPctFlag = TRUE;   // indicate that % set points have changed
            }
            else
            {
                if( g_bHiRegLevelSensor == COVERED )
                {
                    nIndex = g_CurrentRecipe.m_nRegrindComponent - 1;
                    g_CurrentRecipe.m_fPercentage[nIndex] = g_CurrentRecipe.m_fPercentageHighRegrind;
                    g_bPctFlag = TRUE;   // indicate that % set points have changed
                }
                else
                {
//                    ASM = USEOLD
//                    memcpy(&g_CurrentRecipe,&g_TempRecipe, sizeof( g_TempRecipe ) );
//                    g_bPctFlag = TRUE;   // indicate that % set points have changed
                }

            }
        }

    }
}



/*

; CHECK HIGH / LOW LEVEL SENSORS FOR TRANSITION FOR HI/LO REGRIND OPTION.

CHKHILOREGSENSORS:
        TST     CYCLEINDICATE           ;
        BNE     NOCHLO                  ; EXIT
        LDAA    HILOWOPTIONSWITCHEDONOFFHIS
        CMPA    #1
        BNE     NOT1                    ; NOT ON
        JSR     USEOLD                  ; USE OLD VALUE.
        CLR     HILOWOPTIONSWITCHEDONOFFHIS
        BRA     NOCHLO                  ; EXIT.
NOT1    CMPA    #2                      ; OFF
        BNE     NOEEUP                  ; NO.
        JSR     UPDATEREGHILONOC        ; NO CHECK ON MODE.
        CLR     HILOWOPTIONSWITCHEDONOFFHIS
        BRA     NOCHLO                  ; EXIT.
NOEEUP  LDAA    HIGHLEVELSENSOR         ; HIGH LEVEL SENSOR.
        CMPA    HIGHLEVELSENSORHIS      ; COMPARE TO HISTORY.
        BEQ     NOCHHI                  ; NO CHANGE.
        LDAA    HIGHLEVELSENSOR
        STAA    HIGHLEVELSENSORHIS      ;
        JSR     UPDATEREGHILO              ; UPDATE HIGH LOW %
NOCHHI  LDAA    LOWLEVELSENSOR
        CMPA    LOWLEVELSENSORHIS       ;
        BEQ     NOCHLO                  ; NO CHANGE.
        LDAA    LOWLEVELSENSOR
        STAA    LOWLEVELSENSORHIS
        JSR     UPDATEREGHILO
NOCHLO  RTS






UPDATEREGHILO:
        TST     HILOREGENABLED          ; IS THIS OPTION ENABLED.
        BEQ     XITHLR                  ; NO

UPDATEREGHILONOC:
        TST     REG_PRESENT             ; IS THERE REGRIND.
        BEQ     XITHLR                  ; NO

        LDAA    BLENDERMODE             ; CHECK BLENDER MODE.
        CMPA    #OFFLINEMODE            ; OFFLINE MODE.
        BNE     CONCHK                  ; NO
        LDAA    OFFLINETYPE             ; OFF LINE TYPE ?
        CMPA    #OFFLINETYPEHILO        ; HIGH LOW ?
        BEQ     XITHLR                  ; YES

; CHECK FOR CHANGE IN LEVEL SENSOR STATUS.
;

CONCHK  TST     LOWLEVELSENSOR          ; CHECK LOW.
        BNE     CHKHIGH                 ; LOW ON.

; LOW OFF

;        TST     ISOLD              ; IS IT OLD ALREADY ?
;        BNE     XITHLR
        JSR     USEOLD             ; USE OLD VALUE.

;        LDE     LOREGPCNT               ; READ HIGH %
;        LDX     #PCNT1
;        LDAB    REGCOMP
;        DECB
;        ABX
;        ABX
;        STE     0,X                     ; STORE DATA
;        LDAA    #$AA
;        STAA    PCTFLG             ;~%'S CHANGE FLAG, SET TO GENERATE C3'S
        BRA     XITHLR
CHKHIGH TST     HIGHLEVELSENSOR         ;
        BEQ     HIISOFF                 ; OFF
        LDE     HIREGPCNT               ; READ HIGH %
        LDX     #PCNT1
        LDAB    REGCOMP
        DECB
        ABX
        ABX
        STE     0,X                     ; STORE DATA
        LDAA    #$AA
        STAA    PCTFLG             ;~%'S CHANGE FLAG, SET TO GENERATE C3'S
;        CLR     ISOLD                   ; SHOW THAT MAX IS BEING USED
        BRA     XITHLR                  ; NO

HIISOFF:
;        JSR     USEOLD             ; USE OLD VALUE.
XITHLR  RTS


USEOLD  LDX     #BAKPCNT1
        LDAB    REGCOMP
        DECB
        ABX
        ABX
        LDE     0,X                     ; STORE DATA
        LDX     #PCNT1
        LDAB    REGCOMP
        DECB
        ABX
        ABX
        STE     0,X                     ; STORE DATA
        LDAA    #$AA
        STAA    PCTFLG             ;~%'S CHANGE FLAG, SET TO GENERATE C3'S
        LDAA    #1
        STAA    ISOLD
        RTS
*/

/////////////////////////////////////////////////////
// CheckForSEITimeOuts()        from CHKFORSEITO
//
// Check for SEI timeouts & generate SEI comms alarm.
//
// P.Smith
//////////////////////////////////////////////////////


void CheckForSEITimeOuts ( void )
{
    g_nSEITimeOutCounter++;
    if(g_nSEITimeOutCounter >= SEITIMEOUTMAX)       // SEI timed out ?
    {
        g_nSEITimeOutCounter = 0;
        if(g_CalibrationData.m_nPeripheralCardsPresent & SEIENBITPOS)
        {
            if( (g_arrnMBTable[BATCH_SUMMARY_PROCESS_ALARMS] & SEICOMMSALARMBIT) == 0) // alarm bit clear?
            {  //
//                    iprintf("\nsei comms alarm"); //nbb--testonly--
                    PutAlarmTable( SEICOMMSALARM,  0 );     // indicate alarm, //PTALTB
                    g_bSEIHasTimedOut = TRUE;
            }
        }
    }
}


/*

CHKFORSEITO:
        INC     SEITIMEOUTCTR           ; INCREMENT WATCHFOG TIMER.
        LDAA    SEITIMEOUTCTR
        CMPA    #SEITIMEOUTMAX+1        ; SEI TIMEOUT
        BLO     NOSEIT                  ; NO
        LDAA    EIOMITTED               ;
        ANDA    #SEIENBITPOS
        BEQ     NOSEI
        LDAA    MBPROALRM+1             ; MODBUS ALARMS.
        ANDA    #SEICOMMSALARMBIT       ; SEI COMMS ALARM ?
        BNE     NOSEIT                  ; ALREADY SET
        LDAA    #SEICOMMSALARM          ; SEI COMMS ALARM
        JSR     PTALTB                  ; INDICATE ALARM.
NOSEI   LDAA    #1
        STAA    SEITIMEOUTFLAG
NOSEIT  RTS

*/

/////////////////////////////////////////////////////
// CheckForMixerOff()        from CHECKFORMIXEROFF
//
// if mixer if off set mixer transition counter to MIXERTRANSITIONDELAY.
//
// P.Smith                  7-04-2005
//////////////////////////////////////////////////////


void CheckForMixerOff ( void )
{
    if(g_bMixOff)  // mixer off
    {
        g_nMixerTransitionCounter = MIXERTRANSITIONDELAY;
    }

}


/*
CHECKFORMIXEROFF:
        TST    MIXOFF           ; WAS THE MIXER OFF
        BEQ    MIXISN1         ; MIXER IS ON
        LDAA   #MIXERTRANSITIONDELAY
        STAA   MIXERTRANSITIONCTR   ; SET DELAY COUNTER.
MIXISN1 RTS
*/



/////////////////////////////////////////////////////
// CheckLicenseForFlashing()        from CHECKLICFORFLASH
//
// If license has expired, ie modbus license alarm, flash alarm
//
//
// P.Smith                  7-04-2005
//////////////////////////////////////////////////////


void CheckLicenseForFlashing ( void )
{
    if(g_arrnMBTable[BATCH_SUMMARY_SYSTEM_ALARMS] & LICENSEALARMBITPOS)
    {
//        g_bAlarmFlash = TRUE;       // ASM ALARMFLASH indicate that alarm should be flashed to show license has expired.
    }
    else
    {
        g_bAlarmFlash = FALSE;       //
    }

// ASM ALARMFLASHCHECK
    if(g_bAlarmFlash)               // should alarm be flashed ?
    {
        g_bFlashSecondCounter++;
        if(g_bFlashSecondCounter > ALARMFLASHRATE)      // check for flash rate
        {
            g_bFlashSecondCounter = 0;
            if(g_bAlarmFlashStatus)
            {
                g_bAlarmFlashStatus = FALSE;
            }
            else
            { // SETFLON
                g_bAlarmFlashStatus = TRUE;
            }
        }
    }
    // CHECKFORALARMONOFF
    if(g_bAlarmFlash)           // should alarm be flashed ?
    {
        if(g_bAlarmFlashStatus)
        {  // ALOFF
            AlarmOutputsOff();   // ASM ALARMOFF
        }
        else
        {
            AlarmOutputsOn();    // ALARMON
        }
    }
}


/////////////////////////////////////////////////////
// CheckForAChangeInSEICalibrationData
//
//
// if there is a change in the inc /dec data initiate the long write to the SEI by
// setting g_nSEIWriteRequiredCounter to SENDCALDATATOSEINO
//
//////////////////////////////////////////////////////


void CheckForAChangeInSEICalibrationData ( void )
{
    if(g_nWriteHoldOffCounter == 0)  // do not check until initial read is completed from sei.
    {
        if(g_nControlLagHistory != g_CalibrationData.m_nControlLag)
        {
            g_nControlLagHistory = g_CalibrationData.m_nControlLag;
            g_nSEIWriteRequiredCounter = SENDCALDATATOSEINO;   // this activates the long write message to sei
        }
        else
        if(g_nIncreaseRateHistory != g_CalibrationData.m_nIncreaseRate)
        {
           g_nIncreaseRateHistory = g_CalibrationData.m_nIncreaseRate;
           g_nSEIWriteRequiredCounter = SENDCALDATATOSEINO;
        }
        else
        if(g_nDecreaseRateHistory != g_CalibrationData.m_nDecreaseRate)
        {
           g_nDecreaseRateHistory = g_CalibrationData.m_nDecreaseRate;
           g_nSEIWriteRequiredCounter = SENDCALDATATOSEINO;
         }
         else
         if(g_nControlDeadbandHistory != g_CalibrationData.m_nControlDeadband)
         {
             g_nControlDeadbandHistory = g_CalibrationData.m_nControlDeadband;
             g_nSEIWriteRequiredCounter = SENDCALDATATOSEINO;
         }
         if(g_nFineIncDecHistory != g_CalibrationData.m_nFineIncDec)
         {
             g_nFineIncDecHistory = g_CalibrationData.m_nFineIncDec;
             g_nSEIWriteRequiredCounter = SENDCALDATATOSEINO;
          }
    }
}

/////////////////////////////////////////////////////
// AnalyseCommunications
//
// sei, lls and optimisation time counter are checked against previous
// values and error / okay status is indicated
//
//////////////////////////////////////////////////////


void AnalyseCommunications ( void )
{
    if( (g_CalibrationData.m_nPeripheralCardsPresent & SEIENBITPOS) != 0 )  // is SEI present??
    {
        if(g_nBatchCommsSEITimeoutHistory == structSEIData.m_lRxTimeoutCounter)
        {
            g_nBatchCommsStatusSEI = BATCH_COMMS_PROTOCOL_COMMS_OKAY;
        }
        else
        {
            g_nBatchCommsStatusSEI = BATCH_COMMS_PROTOCOL_COMMS_ERROR;
            g_nBatchCommsSEITimeoutHistory = structSEIData.m_lRxTimeoutCounter;
        }
    }
    else
    {
        g_nBatchCommsStatusSEI = BATCH_COMMS_PROTOCOL_UNUSED;
    }

    if( (g_CalibrationData.m_nPeripheralCardsPresent & LLSENBITPOS) != 0 )  // is LLS present??  // EIOMITTED - YES/NO (b0 = SEI, b1 = LLS,...)
    {
        if(g_nBatchCommsLLSTimeoutHistory == structLLSData.m_lRxTimeoutCounter)
        {
            g_nBatchCommsStatusLLS = BATCH_COMMS_PROTOCOL_COMMS_OKAY;
        }
        else
        {
            g_nBatchCommsStatusLLS = BATCH_COMMS_PROTOCOL_COMMS_ERROR;
            g_nBatchCommsLLSTimeoutHistory = structLLSData.m_lRxTimeoutCounter;
        }
    }
    else
    {
       g_nBatchCommsStatusLLS = BATCH_COMMS_PROTOCOL_UNUSED;
    }
    if( (g_CalibrationData.m_nPeripheralCardsPresent & OPTIMISATIONENABLEBITPOS) != 0 )  // is LLS present??  // EIOMITTED - YES/NO (b0 = SEI, b1 = LLS,...)
    {
        if(g_nBatchCommsOptimisationTimeoutHistory == structOptimisationData.m_lRxTimeoutCounter)
        {
            g_nBatchCommsStatusOptimisation = BATCH_COMMS_PROTOCOL_COMMS_OKAY;
        }
        else
        {
            g_nBatchCommsStatusOptimisation = BATCH_COMMS_PROTOCOL_COMMS_ERROR;
            g_nBatchCommsOptimisationTimeoutHistory = structOptimisationData.m_lRxTimeoutCounter;
        }
    }
    else
    {
        g_nBatchCommsStatusOptimisation = BATCH_COMMS_PROTOCOL_UNUSED;
    }

    if(g_nBatchCommsNetworkGoodTxHistory == g_sModbusNetworkSlave.m_lGoodTx)
    {
        g_nBatchCommsStatusNetwork = BATCH_COMMS_PROTOCOL_UNUSED;
    }
    else
    {
        g_nBatchCommsStatusNetwork = BATCH_COMMS_PROTOCOL_COMMS_OKAY;
        g_nBatchCommsNetworkGoodTxHistory = g_sModbusNetworkSlave.m_lGoodTx;
    }

    // tcpip comms
    if(g_nTCPIPReadHoldRegsCtr != 0)
    {
        g_nBatchCommsStatusTCPIP = BATCH_COMMS_PROTOCOL_COMMS_OKAY;
        g_nTCPIPReadHoldRegsCtr = 0;
    }
    else
    {
        g_nBatchCommsStatusTCPIP = BATCH_COMMS_PROTOCOL_UNUSED;
    }
    g_nTCPIPReadHoldRegsCtr = 0; // set to 0 to allow it to be checked the next time

    if(g_nBatchCommsPanelTimeoutRxHistory == g_nBatchCommsPanelTimeoutRx)
    {
        g_nBatchCommsStatusPanel = BATCH_COMMS_PROTOCOL_COMMS_OKAY;
    }
    else
    {
        g_nBatchCommsStatusPanel = BATCH_COMMS_PROTOCOL_COMMS_ERROR;
        g_nBatchCommsPanelTimeoutRxHistory = g_nBatchCommsPanelTimeoutRx;
    }

    if( g_VacCalibrationData.m_nLoaders > 0 )
    {
        if(g_nCommsVacuumLoadingTimeoutHistory == structVAC8IOData[0].m_lRxTimeoutCounter)
        {
            g_nBatchCommsStatusVacuumLoading = BATCH_COMMS_PROTOCOL_COMMS_OKAY;
        }
        else
        {
            g_nBatchCommsStatusVacuumLoading = BATCH_COMMS_PROTOCOL_COMMS_ERROR;
            g_nCommsVacuumLoadingTimeoutHistory = structVAC8IOData[0].m_lRxTimeoutCounter;
        }
    }
    else
    {
        g_nBatchCommsStatusVacuumLoading = BATCH_COMMS_PROTOCOL_UNUSED;
    }


}

/******************************************************************
 *  CheckFor24HourRollOver
 check for clock going from 23 59 - 00 00
 when it sees 23 59 , flag set to indicate in the last minute, then
 when the hour goes to 0, this is when 24hours has elapsed.

 ******************************************************************/
void CheckFor24HourRollOver( void )
{
    if(g_bInLastMinute)  // in the last minute
    {
        if(g_CurrentTime[TIME_HOUR] == 0x0)  // hour rolled over
        {
            g_bInLastMinute = FALSE;  // reset flag
            CopyOngoingTo24HourlyHistoryLog();
        }
    }
    else
    {
        if((g_CurrentTime[TIME_HOUR] == 23)&& (g_CurrentTime[TIME_MINUTE] == 59))
        {
            g_bInLastMinute = TRUE;
        }
        else
        {
            g_bInLastMinute = FALSE;
        }
    }
}

void LoaderDiagnostics( void )
{
// loader-diagnostics.
    unsigned int i;
    char cBuf[80];

   if(fdTelnet>0 && g_nTSMDebug.m_bLoaderDebug)
   {
//      printf("\n No.Loaders= %d Tx= %d Rx= %d", g_VacCalibrationData.m_nLoaders, structVAC8IOData[0].m_lGoodTx, structVAC8IOData[0].m_lGoodRx );
         i=0;
/*
        int nIndex = PSUM_BLOCK_1 + (PSUM_BLOCK_SIZE * i) + PSUM_SET_KGH_OFFSET;
      printf("\nSet1Msb= %d Set1Lsb= %d", g_arrnMBTable[nIndex],g_arrnMBTable[nIndex+1] );
        nIndex = PSUM_BLOCK_1 + (PSUM_BLOCK_SIZE * i) + LIW_PSUM_ACTUAL_KGH_OFFSET;
      printf("\nAct1Msb= %d Act1Lsb= %d", g_arrnMBTable[nIndex],g_arrnMBTable[nIndex+1] );
*/
      printf("\n");

         sprintf(cBuf, " PUMP= ");
         if( g_bPumpStatus == ON )
            strcat(cBuf, "  ON");
         else
            strcat(cBuf, " OFF");

         strcat(cBuf, " AV= ");
         if( g_bAtmValveStatus == OPEN )
            strcat(cBuf, " OPEN");
         else
            strcat(cBuf, " CLOSED");

         strcat(cBuf, " FLTR= ");
         if( g_bFilterStatus == ON )
            strcat(cBuf, "  ON");
         else
            strcat(cBuf, " OFF");
         printf("%s", cBuf);
         WORD nWordTemp =  g_arrnWriteVAC8IOMBTable[0][MB_VAC8IO_COMMAND1];
         printf(" AVisOpenTmr= %d Idle= %d PumpRun= %d cmd1= %4x FReps= %d", g_nAVIsOpenTimer, g_nSystemIdleTimer, g_nPumpRunTimer, nWordTemp, g_nFilterRepetitionsCounter );

      i=0;
      for(i=0; i<8; i++)
      {
         sprintf(cBuf, "\nLdr= %2d", g_sLoader[i].m_nLoaderNo);
         if( g_sLoader[i].m_bLevelFlag == FULL )
            strcat(cBuf, " FULL");
         else
            strcat(cBuf, " EMPTY");

            printf("%s", cBuf);

         if( g_VacCalibrationData.m_bOnFlag[i] == ON)
            printf("  ON");
         else
            printf(" OFF");

          printf(" Fill Time=%2d", g_VacCalibrationData.m_nSuctionTime[i]);
          printf(" Prty=%2d", g_VacCalibrationData.m_nPriority[i]);
          printf(" Post=%2d", g_VacCalibrationData.m_nPostFillTime[i]);


         if( g_sLoader[i].m_nFillTimer > 0)
         {
            sprintf(cBuf, " filling %d", g_sLoader[i].m_nFillTimer);
//            strcat(cBuf, cBuf2 );
            printf("%s", cBuf);

         }

      }

        sprintf(cBuf, "\nPUMP= ");
         if( g_bPumpStatus == ON )
            strcat(cBuf, "  ON");
         else
            strcat(cBuf, " OFF");
       printf("%s", cBuf);
       printf(" nLdrs= %d PumpStrt= %d PumpIdle= %d", g_VacCalibrationData.m_nLoaders, g_VacCalibrationData.m_nPumpStartTime, g_VacCalibrationData.m_nPumpIdleTime );
  }
}



/*
NOTATPRESET:

       INC      DISPLAYUPDATECTR ; DISPLAY UPDATE COUNTER.
       LDAA     DISPLAYUPDATECTR ;
       CMPA     #UPDATETIMEMINS  ; UPDATE TIME IN MINUTES.
       BLO      NOTATT          ; NOT AT TIME.
       LDAA     #1
       STAA    UPDATEDISPLAYNOW ; UPDATE THE DISPLAY NOW
       CLR     DISPLAYUPDATECTR ;

NOTATT INC    NOCYCLE           ;~INCREMENT THE MINUTES COUNTER
       LDAA   NOCYCLE
       LDAB   MIXINGSHUTOFF
       JSR    BCDHEX1
       CBA
       BHI    SWOFF
       BNE    LEAVEON           ;~NO!
SWOFF  CLR    NOCYCLE
       LDAA     BLENDERTYPE
       CMPA     #TINYTYPE       ; TINY TYPE.
       BEQ      ISTNY1          ; IS TINY
       JSR     CLEARTHROUGHPUTTOTALS
       LDAA     #1
       STAA     MIXOFF
ISTNY1:



LEAVEON:
       INC    LS1OFFCT
       LDAA   LS1OFFCT
       CMPA   #LSUMIXS
       BLS    LEAVONN
       LDAA     #1
       STAA     MIXOFF
       CLR      LS1OFFCT
LEAVONN:
        LDAA    BLENDERTYPE     ;
        CMPA    #TINYTYPE       ; MICRA
        BNE     SKMXCL          ; NO
        TST     CYCLEINDICATE
        BNE     SKMXCL          ; NO
        TST     MIXOFF          ; MIXER IS OFF ?
        BEQ     MIXISON         ; MIXER IS ON.
        TST     LEVSEN1STAT     ; LEVEL SENSOR COVERED
        BNE     MIXISON         ; OFF EXIT
        INC     MIXEROFFCTR     ;
        LDAA    MIXEROFFCTR
        CMPA    #MIXOFFTIME     ; MIXER OFF MAX TIME
        BLO     NOMXON          ; NO MIX ON
        LDAB    MIXTIM          ; MIXING TIME BCD
        JSR     BCDHEX1         ; TO HEX
        STAB    MIXONTIME       ; MIXER ON FOR N SECONDS
        CLR     MIXEROFFCTR     ; RESET COUNTER.
NOMXON  BRA     SKMXCL          ; SKIP MIXER OFF CLEAR
MIXISON CLR     MIXEROFFCTR     ; IS OFF.

SKMXCL:
        INC    LS1OFFCT
        LDAA   LS1OFFCT
        CMPA   #LSUMIXS
        BLS    LEAVON
        LDAA     BLENDERTYPE
        CMPA     #TINYTYPE       ; TINY TYPE.
        BEQ      ISTNY2          ; IS TINY
        LDAA     #1
        STAA     MIXOFF
ISTNY2  CLR      LS1OFFCT
LEAVON:

NOT1MIN:


*/


/*
UPDATEREGHILO:
        TST     HILOREGENABLED          ; IS THIS OPTION ENABLED.
        BEQ     XITHLR                  ; NO

UPDATEREGHILONOC:
        TST     REG_PRESENT             ; IS THERE REGRIND.
        BEQ     XITHLR                  ; NO

        LDAA    BLENDERMODE             ; CHECK BLENDER MODE.
        CMPA    #OFFLINEMODE            ; OFFLINE MODE.
        BNE     CONCHK                  ; NO
        LDAA    OFFLINETYPE             ; OFF LINE TYPE ?
        CMPA    #OFFLINETYPEHILO        ; HIGH LOW ?
        BEQ     XITHLR                  ; YES

; CHECK FOR CHANGE IN LEVEL SENSOR STATUS.
;

CONCHK  TST     LOWLEVELSENSOR          ; CHECK LOW.
        BNE     CHKHIGH                 ; LOW ON.

; LOW OFF

;        TST     ISOLD              ; IS IT OLD ALREADY ?
;        BNE     XITHLR
        JSR     USEOLD             ; USE OLD VALUE.

;        LDE     LOREGPCNT               ; READ HIGH %
;        LDX     #PCNT1
;        LDAB    REGCOMP
;        DECB
;        ABX
;        ABX
;        STE     0,X                     ; STORE DATA
;        LDAA    #$AA
;        STAA    PCTFLG             ;~%'S CHANGE FLAG, SET TO GENERATE C3'S
        BRA     XITHLR
CHKHIGH TST     HIGHLEVELSENSOR         ;
        BEQ     HIISOFF                 ; OFF
        LDE     HIREGPCNT               ; READ HIGH %
        LDX     #PCNT1
        LDAB    REGCOMP
        DECB
        ABX
        ABX
        STE     0,X                     ; STORE DATA
        LDAA    #$AA
        STAA    PCTFLG             ;~%'S CHANGE FLAG, SET TO GENERATE C3'S
;        CLR     ISOLD                   ; SHOW THAT MAX IS BEING USED
        BRA     XITHLR                  ; NO

HIISOFF:
;        JSR     USEOLD             ; USE OLD VALUE.
XITHLR  RTS


USEOLD  LDX     #BAKPCNT1
        LDAB    REGCOMP
        DECB
        ABX
        ABX
        LDE     0,X                     ; STORE DATA
        LDX     #PCNT1
        LDAB    REGCOMP
        DECB
        ABX
        ABX
        STE     0,X                     ; STORE DATA
        LDAA    #$AA
        STAA    PCTFLG             ;~%'S CHANGE FLAG, SET TO GENERATE C3'S
        LDAA    #1
        STAA    ISOLD
        RTS
*/

/*
// This is from Paraic's file - 26.11.2004

CHKHILOSENSORS:
        LDAA    BLENDERMODE             ; CHECK BLENDER MODE.
        CMPA    #OFFLINEMODE            ; OFFLINE MODE.
        BNE     XITHLC                  ; EXIT HIGH LOW CHECK.
        LDAA    OFFLINETYPE             ; OFF LINE TYPE ?
        CMPA    #OFFLINETYPEHILO        ; HIGH LOW ?
        BNE     XITHLC                  ; NO
        TST     HIGHLEVELSENSOR         ; HIGH LEVEL SENSOR ON ?
        BEQ     CHKFORL                 ; NO.

        TST     HIGHSENSORHISTORY
        BNE     NOSTUP
        JSR     STARTOFFLINEDELAY
        LDAA    #FIVE
        STAA    PAUSEINNSECONDS

NOSTUP:
        LDAA    #1
        STAA    HIGHSENSORHISTORY
;        LDAA    #OFFLINEPAUSEATENDOFCYCLE      ; PAUSE AT END OF CYCLE.
;        STAA    CYCLEPAUSETYPE          ; SET PAUSE TYPE.
        BRA     XITHLC                  ; EXIT.
CHKFORL:
        CLR     HIGHSENSORHISTORY
        TST     LOWLEVELSENSOR          ; LOW
        BNE     XITHLC                  ; EXIT IF ON.
        LDAA    CYCLEPAUSETYPE          ; SET PAUSE TYPE.
        CMPA    #PAUSEATENDOFCYCLE
        BEQ     XITHLC                  ; NO PAUSE OFF IF PAUSE GENERATED BY K/B                  ;
        CMPA    #IMMEDIATEPAUSE         ; IMMEDIATE PAUSE IN OPERATION.
        BEQ     XITHLC                  ; NO PAUSE OFF IF PAUSE GENERATED BY K/B                  ;
        LDAA    PAUSFLG
        ANDA    #NORMALPAUSE            ; NORMAL PAUSE ?
        BNE     XITHLC                  ; NO PAUSE OFF IF PAUSE GENERATED BY K/B                  ;
        CLR     CYCLEPAUSETYPE          ; RESET PAUSE FLAG.
        JSR     PAUSEOFF                ; SENSOR IS OFF REMOVE PAUSE.
XITHLC  RTS
*/
/*
CHECKFORBYPASSACTIVATION:
        TST     BYPASSMODE
        BEQ     BYPASNON                ; BYPASS IS ON.
        TST     HIGHLEVELSENSOR         ; READ SENSOR
        BNE     BYPASNON                ; BYPASS SENSOR IS ON.
        INC     BYPASSSENSOROFFCTR      ; BY PASS SENSOR OFF COUNTER
        LDAB    #BYPASSTIMELIMIT        ; BY PASS SENSOR OFF LIMIT DURING NORMAL CYCLE
DOBCTHX CMPB    BYPASSSENSOROFFCTR      ; COMPARE TO COUNTER
        BHI     XITBYC
        CLR     BYPASSSENSOROFFCTR
        JSR     OPENBYPASS
XITBYC  RTS


BYPASNON:
        CLR     BYPASSSENSOROFFCTR
        JSR     CLOSEBYPASS             ; CLOSE BYPASS
        BRA     XITBYC                  ; EXIT BYPASS

*/
/*
//NOLSALRM TST     CHECKLSENSORINNSECONDS
        BEQ     NOUPR                   ; NO UPDATE REQUIRED
        DEC     CHECKLSENSORINNSECONDS
        TST     CHECKLSENSORINNSECONDS
        BNE     NOUPR                   ; HAS IT JUST GONE TO ZERO
        LDAA    #1
        STAA    CHECKFORMAXTHP          ; RUN D/A PER KG CALC

        TST     SECROVER                ;HAS SECOND TURNED OVER
        LBEQ     PITINT1
        CLR     SECROVER


; CHECK IF THE EXPAN PCB SHOULD BE USED FOR THE CLEANING

        LDAA    NOBLNDS
        CMPA    #FOUR
        BHI     ENEXPCB
        CLR     EXTERNALCLEANPCB
        BRA     CONS1
ENEXPCB LDAA    #1
        STAA    EXTERNALCLEANPCB
CONS1:  TST     LEVELSENSORDELAYCTR
        BEQ     NOLSD                           ; NO DECREMENT REQUIRED
        DEC     LEVELSENSORDELAYCTR
        TST     LEVELSENSORDELAYCTR
        BNE     NOLSD
        LDAA    #$0F
        STAA    DMPLEVSEN                       ; SET "DUMP" LEVEL SENSOR FLAG

NOLSD:
        JSR     CHECKFORDIAGNOSTICSPASSWORD

;       CHECK FOR LEVEL SENSOR MONITORING ALARM

        TST     LEVELMONITOR
        BEQ     NOLSALRM                       ; EXIT LEVEL SENSOR WATCH DOG
        TST     IGNORELSTIME
        BNE     NOLSALRM                       ; NO CHECK FOR ALARM
        TSTW    BATCHMONCTR
        BEQ     NOLSALRM
        DECW    BATCHMONCTR
        BNE     NOLSALRM
        LDAA     MBPROALRM               ; CHECK EXISTING ALARM.
        ANDA     #LSMONITORALARMBIT           ;
        BNE      NOLSALRM                ; ALARM ALREADY SET.
        LDAA    #LSMONITORALARM
        JSR     PTALTB



NOLSALRM TST     CHECKLSENSORINNSECONDS
        BEQ     NOUPR                   ; NO UPDATE REQUIRED
        DEC     CHECKLSENSORINNSECONDS
        TST     CHECKLSENSORINNSECONDS
        BNE     NOUPR                   ; HAS IT JUST GONE TO ZERO
        LDAA    #1
        STAA    CHECKFORMAXTHP          ; RUN D/A PER KG CALC

NOUPR   JSR     CHECKFORBYPASSACTIVATION
        LDAA    PRINTINNSECONDS
        BEQ     NOPR1                   ; NO PRINTING.
        DEC     PRINTINNSECONDS         ; DECREMENT
        BNE     NOPR1
        LDAA    #1
        STAA    PRINTNOW
NOPR1   LDAA    CONTROLTYPE
        CMPA    #INCREASEDECREASE
        BNE     NOIDRD
        TST     DOREADINNSECONDS
        BEQ     NOIDRD                  ; NO INCREASE DECREASE READ.
        DEC     DOREADINNSECONDS
        BNE     NOIDRD
        LDAA    #1
        STAA    READINCDECCAL
NOIDRD  TST     CHKIFCYCLEFLAG          ; CHECK IF THE BLENDER SHOULD CYCLE.
        BEQ     NOCYC                   ; NO CHECK OF CYCLE.
        INC     CHKIFCYCLECTR
        LDAA    CHKIFCYCLECTR           ;
        CMPA    #SECTOCYCLE             ; NO OF SECONDS TO CYCLE.
        BLO     NOCYC                   ; NO CYCLE CHECK.
        TST     STOPPED                 ;
        BNE     NOCYCRS                 ; SENSOR OFF SYSTEM WILL CYCLE ANYWAY
        TST     LEVSEN1STAT
        BNE     NOCYCRS                 ; SENSOR OFF SYSTEM WILL CYCLE ANYWAY
        TST     HOPPERWEIGHT
        BNE     NOCYCRS                 ; SENSOR OFF SYSTEM WILL CYCLE ANYWAY
        LDD     HOPPERWEIGHT+1
        CPD     #ZEROWEIGHT
        BHI     NOCYCRS                 ; NO CYCLE RESET.
        LDAA    #$0F
        STAA    DMPLEVSEN               ; MAKE THE BLENDER CYCLE.
NOCYCRS CLR     CHKIFCYCLEFLAG
        CLR     CHKIFCYCLECTR
NOCYC   JSR     CHECKFORRRWEIGHTDISPLAY ; SET OR CLEAR FLAG TO ALLOW DISPLAY OF RR DATA


        INC     RXCOMMSWATCHDOG
        LDAA    RXCOMMSWATCHDOG
        CMPA    #2                      ; 3 SECONDS.
        BLS     NOLEDCH                 ; NO LED CHANGE.
        CLR     FLASHRATE               ; RESET FLASH COUNTER TO NORMAL.
        CLR     RXCOMMSWATCHDOG
NOLEDCH:
        CLRW    DEVICETOPOLLCTR         ; RESET DEVICE TO POLL CTR.
        JSR     CHKHILOSENSORS          ; CEHCK FOR HIGH LOW SENSORS.
        JSR     CHKHILOREGSENSORS
        TST     UPDATEREGHILOF          ; UPDATE REGHILO
        BEQ     NOREGU                  ; NO
        JSR     UPDATEREGHILO         ; NO CHECK FOR CYCLE.
        CLR     UPDATEREGHILOF          ; RESET FLAG.
NOREGU  JSR     CHKFORSEITO             ; SEI TIMEOUT
        LDAA    #1
        STAA    SEIPOLLINPUTSF          ; START POLLING OF INPUTS.
        STAA    WRITEDAVALUEF           ; INITIATE D/A WRITE EVERY SECOND.
        STAA    SEIPOLLLLSF             ; SET LOW LEVEL SENSOR
        STAA    WRITEALARMSF
        LDAB    CRLMODE                 ; BLENDER MODE.
        CMPB    #VOLTAGEFOLLOWER        ; VOLTAGE FOLLOWER MODE.
        BNE     NOVF                    ; NO.
        TST     MANATO                  ; AUTO MODE
        BNE     NOVF                    ; NO READ IF IN AUTO.
        STAA    READDAVALUEF
NOVF:
        LDAB    #9
        STAB    TENCNT                  ;SET 10HZ COUNTER TO GO TO 0
        CLR     PITCNTR                 ;PITCNTR RESET ON RTC SECOND
        JSR     STHCAD0                ;START CONVERSION SEQ FOR HC16 AD CH. 0
        BRA     NOADREAD               ;;P PITINT2
PITINT1:
        TST     HOURFG                  ; TIME FOR HOURLY COUNTER ?
        BEQ     NOTHR                   ; NO.
        JSR     CHECKFORTEMPOPTION      ; CHECK FOR TEMPORARY OPTION

        CLR     HOURFG                  ; RESET FLAG.
NOTHR   TST     HCADFIN                 ;32 CONV. SEQ FINISHED??
*/

