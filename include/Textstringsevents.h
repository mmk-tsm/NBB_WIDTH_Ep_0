// P.Smith                              15/7/08
// text strings for event file
//
// P.Smith                              23/7/08
// general text to ------------
//
// P.Smith                              24/7/08
// added strEvlSDCard, extend strings to make file more readable
//
// P.Smith                              1/9/09
// added strEvlConfigLocked,strEvlConfigUnLocked
// strEvModbusWrite, strEvModbusWriteToPanel
//
// P.Smith                              8/9/09
// added   *strAToTestPassed & *strAToTestFailed
//
// P.Smith                              10/9/09
// added strHopperCalibratedAtZeroAlarm
//
// P.Smith                              17/9/09
// added strVacuumLoaderAlarm
//
// P.Smith                              12/1/10
// added strSDCardWriteError
//
// P.Smith                              15/3/10
// added strWatchdogPending
//
// P.Smith                              4/5/10
// added strModbusTCPFatalError

// M.McKiernan							21/5/2020
// Added const  char    *strUltrasonicSensor1Alarm =	"Alarm - Ultrasonic Sensor 1";
//		const  char    *strUltrasonicSensor2Alarm =		"Alarm - Ultrasonic Sensor 2";
//		const  char    *strUltrasonicSensor3Alarm =		"Alarm - Ultrasonic Sensor 3";
//		const  char    *strUltrasonicSensor4Alarm =		"Alarm - Ultrasonic Sensor 4";

#ifndef __TEXTSTRINGSEVENTS_H__
#define __TEXTSTRINGSEVENTS_H__

#include "General.h"
#include "AlarmFunctions.h"

// These will not be translated and so are not set up
// as part of the language arrays.

const   char      *strEvlEvent                       = "Event";
const   char      *strEvlOccurred                    = "Occurred";
const   char      *strEvlGeneral                     = "----------";
const   char      *strEvlAlarm                       = "Alarm";
const   char      *strEvlCleared                     = "Cleared";
const   char      *strEvlStopRun                     = "Stop / Run";
const   char      *strEvlAutoManual                  = "AutoManual";
const   char      *strEvlControl                     = "Control";
const   char      *strEvlSDCard                      = "SD card     ";
const   char      *strEvlPowerOnReset                = "Power On Reset                            ";
const   char      *strEvlSDCardCopyConfigToBackup    = "SD card copy config to backup completed   ";
const   char      *strEvlBypassOpened                = "Bypass opened                             ";
const   char      *strEvlBypassClosed                = "Bypass closed                             ";
const   char      *strEvlBlenderRecipeChanged        = "Blender Recipe Changed                    ";
const   char      *strEvlWeighHopperTared            = "Weigh hopper tared to zero                ";
const   char      *strEvlWeighHopperCalibrated       = "Weigh hopper calibrated                   ";
const   char      *strEvlSdCardLoadConfigComplete    = "SD card load config completed             ";
const   char      *strEvlSdCardLoadBackupConfigComplete    = "SD card load backup config completed    ";
const   char      *strEvlSdCardSaveConfigComplete    = "SD card Save config completed             ";
const   char      *strEvlCleaningCycleStarted    = "Cleaning cycle started                        ";
const   char      *strEvlSpaces    = "        --            ";
const   char      *strEvlConfigLocked                = "Config locked                             ";
const   char      *strEvlConfigUnLocked              = "Config unlocked                           ";
const   char      *strEvModbusWrite                  = "Modbus Write                              ";
const   char      *strEvModbusWriteToPanel           = "Modbus Panel Write                        ";
const   char      *strAToTestPassed                  = "Internal AToD Test Passed                 ";
const   char      *strAToTestFailed                  = "Internal AToD Test Failed                 ";
const   char      *strHopperCalibratedAtZeroAlarm    = "Weight Hopper Calibrated At 0 kgs         ";
const   char      *strVacuumLoaderAlarm              = "Vacuum loader alarms                      ";
const   char      *strModbusTCPFatalError            = "Modbus TCP Error                          ";


const   char   *strBlenderRunToStopTransition =  "Blender stopped / Door opened             ";
const   char   *strBlenderStopToRunTransition =  "Blender started / Door closed             ";
const   char   *strBlenderSwitchedToAuto      =  "Blender switched to Automatic control     ";
const   char   *strBlenderSwitchedToManual    =  "Blender switched to Manual control        ";
const   char   *strBlenderSwitchedToManualControlAlarm =  "Blender switched to Manual control(Control alarm)";
const   char   *strBlenderSwitchedToManualPotControl =  "Blender switched to Manual control(Pot Control)";
const   char   *strBlenderSwitchedToManualSEIForcedExit =  "Blender switched to Manual control(SEI forced exit to manual)";
const   char   *strBlenderSwitchedToManualZeroLinespeed =  "Blender switched to Manual control(zero line speed)";
const   char   *strBlenderSwitchedToManualLeakalarm =  "Blender switched to Manual control(leak alarm)";
const   char   *strBlenderSwitchedToManualExtruderstalled =  "Blender switched to Manual control(extruder stalled)";

const   char   *strBlenderSwitchedToManualBlenderPausedEvent =  "Blender switched to Manual control(Blender paused)";
const   char   *strBlenderSwitchedToManualIncDecSetpointError =  "Blender switched to Manual control(Inc/Dec control setpoint error)";
const   char   *strBlenderSwitchedToManualStarvation =  "Blender switched to Manual control(extruder starvation)";
const   char   *strBlenderInstantVolCorrDueToLspeedChange =  "Volumetric speed change due to line speed change";


const   char   *strOperatorPanelCondigPage1     =  "System config page 1 entered             ";
const   char   *strOperatorPanelCondigPage2     =  "System config page 2 entered             ";
const   char   *strOperatorPanelCondigPage3     =  "System config page 3 entered             ";
const   char   *strOperatorPanelLspeedPage      =  "Line & Screw Speed config page entered    ";
const   char   *strOperatorPanelMeasurementPage = "Measurement and Control config Page entered";
const   char   *strOperatorPanelDeviationPage  = "Deviation config Page entered               ";
const   char   *strOperatorPanelBinConfigPage  = "Bin config calibration Page entered         ";
const   char   *strOperatorPanelModePage       = "Mode Page entered                           ";
const   char   *strOperatorPanelUpdatePage     = "Update config page entered                  ";
const   char   *strOperatorPanelNetworkConfigPage = "Network config page entered              ";
const   char   *strOperatorPanelCleanConfigPage = "Cleaning config page entered               ";
const   char   *strOperatorPanelPasswordConfigPage = "Password config page entered            ";
const   char   *strOperatorPanelLicenseConfigPage = "License config page entered              ";
const   char   *strOperatorPanelOptimisationConfigPage = "Optimisation config page entered        ";
const   char   *strSDCardWriteError =                    "SD Card Write Error                     ";
const   char   *strWatchdogPending =                     "Watchdog Reset Pending                  ";
//wfh
const  char    *strUltrasonicSensor1Alarm =		"Alarm - Ultrasonic Sensor 1";
const  char    *strUltrasonicSensor2Alarm =		"Alarm - Ultrasonic Sensor 2";
const  char    *strUltrasonicSensor3Alarm =		"Alarm - Ultrasonic Sensor 3";
const  char    *strUltrasonicSensor4Alarm =		"Alarm - Ultrasonic Sensor 4";



#endif	// __TEXTSTRINGSEVENTS_H__



