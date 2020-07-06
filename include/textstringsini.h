// P.Smith                              13/3/07
// Beta 2.05 ultrasonic transducer read of sensor.
//
// P.Smith                              9/5/07
// striHopperWeightPage set to lower case
// added striYes, No
//
// P.Smith                              24/5/07
// added striMaterialTransportDelay & striMaterialDumpDelay
// M.McKiernan                   06/09/2007
// added striDecreaseRate
//
//
// P.Smith                              12/10/07
// added striHighLowRegrind , striMachineName, striMoulding,striPurge,striPurgeComponentNumber
// striSensitivity
//
// P.Smith                              27/6/08
// striCycleSpeed text changed to lower case,added striSensitivity,striDeviationAlarmEnable
// striDeviationAlarmEnable,striPauseBlenderOnPowerUp,striAlphaNumeric,striLogToSDCard,striLineSpeedAverage
// striScrewSpeedSignalType
//
// P.Smith                              3/10/08
// added striLiquidAdditive
//
// P.Smith                              25/11/08
// add strings for config file.
//
// P.Smith                              16/4/09
// added striParity
// added striNonStdConfig,low,high strings,purge strings,usb strings
//
// P.Smith                              1/7/09
// added text strings for loading config.
//
// P.Smith                              7/12/09
// added text strings for loading config for min opening time check
// vac8 expansion, min opening time check, flow rate check
//
// P.Smith                              18/1/10
// added strFast
//
// P.Smith                              7/4/10
// striVacuumLoading change to ivr retry delay.
// added striWatchDog
//
// P.Smith                              13/4/10
// striComponentPercentageAlarm changed to deviation text.
//
// P.Smith                              4/5/10
// add strings for baud rates
////////////////////////////////////////////////

#ifndef __TEXTSTRINGSINI_H__
#define __TEXTSTRINGSINI_H__

#include "General.h"
#include "AlarmFunctions.h"

// These will not be translated and so are not set up
// as part of the language arrays.

const   char      *striHopperWeightPage              = "hopper";

const   char      *striCalibrationWeightConstant     = "cal weight constant";
const   char      *striFillTargetWeight              = "fill target weight";
const   char      *striWeightAveraging               = "weight averaging";
const   char      *striDoubleDump                    = "double dump";
const   char      *striTare                          = "tare value";

const   char      *striConfigPage1                   = "config page 1";
const   char      *striBlenderType                   = "blender type";
const   char      *striBlenderMode                   = "mode";
const   char      *striNumberOfBlends                = "number of blends";
const   char      *striMixingScrewDetection          = "mixing screw detection";
const   char      *striOfflineMode                   = "offline mode";
const   char      *striFillRetry                     = "fill retry";
const   char      *striFirstComponentCompensation    = "first component compensation";
const   char      *striHighLowRegrind                = "high low regrind";
const   char      *striMultiStagedFilling            = "multi staged filling";
const   char      *striBatchesInKghAverage           = "batches in kg/h average";
const   char      *striMachineName                   = "machine name";
const   char      *striMoulding                      = "moulding";
const   char      *striContinuousDownload            = "continuous download";
const   char      *striTelnet                        = "telnet";
const   char      *striVacuumLoading                 = "IVR Retry Delay";
const   char      *striParity                        = "Parity";

const   char      *striMinOpeningTimeCheck           = "minimum open check";
const   char      *striFlowRateCheck                 = "flow rate check";
const   char      *striWatchDog                      = "watch dog";

const   char      *striVAC8Expansion                 = "vac8 expansion";


const   char      *striConfigPage2                   = "config page 2";
const   char      *striBlenderUits                   = "units";
const   char      *striRemoteModules                 = "remote modules";
const   char      *striStandardCompConfig            = "standard comp config";
const   char      *striOfflineMixingTime             = "offline mixing time";
const   char      *striMixingMotorShutOffTime       = "mixing motor shut-off time";
const   char      *striDiagnostics                   = "diagnostics";
const   char      *striLevelSensorAlarmDelay         = "level sensor alarm delay";
const   char      *striVolumetricMode                = "volumetric mode";
const   char      *striCycleSpeed                    = "cycle speed";
const   char      *striMaxkghThroughput              = "Max kg/h throughput";
const   char      *striMaterialTransportDelay        = "material transport delay";
const   char      *striMaterialDumpDelay             = "material dump delay";
const   char      *striSensitivity                   = "sensitivity";
const   char      *striDeviationAlarmEnable          = "deviation alarm";
const   char      *striAlphaNumeric                  = "alphanumeric";
const   char      *striPauseBlenderOnPowerUp         = "pause on start up";
const   char      *striNonStdConfig                  = "non standard config";



const   char      *striConfigPage3                   = "config page 3";
const   char      *striRegrindFillRetry              = "regrind fill retry";
const   char      *striRegrindAsReference            = "regrind as reference";
const   char      *striSingleWordWrite               = "single word write";
const   char      *striBypass                        = "bypass mode";
const   char      *striCleaning                      = "cleaning";
const   char      *striTopUp                         = "top up";
const   char      *striTopUpReference                = "top up reference";
const   char      *striFirstComponentLayering        = "first component layering";
const   char      *striPercentageTopUp               = "percentage top up";
const   char      *striMaxkgsForTopUp                = "max kgs for top up";
const   char      *striLatency                       = "latency";
const   char      *striPeriodicMixingTime            = "periodic mixing time";
const   char      *striLiquidAdditive                = "liquid additive";

const   char      *striRecipeEntry                   = "recipe entry";
const   char      *striComponentPercentageAlarm      = "Deviation Alarm";
const   char      *striMaxComponentPercentagelimit   = "max component percentage limit";
const   char      *striPurge                         = "purge";
const   char      *striPurgeComponentNumber          = "purge component number";
const   char      *striLogToSDCard                   = "log to sd card";



const   char      *striControlLineSpeedPage          = "control line speed";
const   char      *striLineSpeedConstant             = "line speed constant";
const   char      *striMaxExtruderSetpoint           = "max extruder setpoint";
const   char      *striLineSpeedSignalType           = "line speed signal type";
const   char      *striLineSpeedAverage              = "line speed average";
const   char      *striScrewSpeedSignalType          = "screw speed signal type";


const   char      *striControlMeasAndControlPage      = "measurement and control";
const   char      *striControlGain                    = "control gain";
const   char      *striControlAveraging               = "control averaging";
const   char      *striFastControlResponse            = "fast control response";
const   char      *striExtruderStarvationShutdown     = "extruder starvation shutdown";
const   char      *striExtruderShutdownTime           = "extruder shutdown time";
const   char      *striControlMethod                  = "control method";
const   char      *striCorrectEveryCycle              = "correct every cycle";
const   char      *striThroughputDeadband             = "throughput deadband";
const   char      *striControlType                    = "control type";
const   char      *striControlLag                     = "control lag";
const   char      *striIncreaseRate                   = "increase rate";
const   char      *striDecreaseRate                   = "decrease rate";
const   char      *striDeadband                       = "deadband";
const   char      *striFineband                       = "fineband";



const   char      *striCleaningPage                   = "cleaning";
const   char      *striTopOnTime                      = "top on time";
const   char      *striTopOffTime                     = "top off time";
const   char      *striTopRepetitions                 = "top repetitions";
const   char      *striBottomOnTime                   = "bottom on time";
const   char      *striBottomOffTime                  = "bottom off time";
const   char      *striBottomRepetitions              = "bottom repetitions";
const   char      *striBypassTimeLimit                = "bypass time limit";
const   char      *striBlowEveryCycle                 = "blow every cycle";
const   char      *striBypassActivation               = "bypass activation";
const   char      *striYes                            = "yes";
const   char      *striNo                             = "no";

const   char      *striVacuumLoadingPage              = "vacuum loading";

const   char      *striNumberOfLoaders               = "number of loaders";
const   char      *striIdleTime                      = "idle time";
const   char      *striPumpStartTime                 = "pump start time";
const   char      *striAtmValveOverlap               = "atm valve overlap";
const   char      *striFilterRepetitions             = "filter repetitions";
const   char      *striFilterType                    = "filter type";
const   char      *striDumpDelay                     = "dump delay";

const   char      *striLoaderOnOffStatus             = "on off status";
const   char      *striLoaderSuctionTime             = "suction time";
const   char      *striLoaderPostFillTime            = "post fill time";
const   char      *striPriority                      = "priority";

const   char      *striIndividualLoaderPage          = "loader";
const   char      *strTSMFilterType                     = "TSM";
const   char      *strPCFilterType                      = "PC";


// new strings added

const   char      *striBlendingOnly                   = "Blending Only";
const   char      *striOffline                        = "Offline";
const   char      *striThroughputMonitor              = "Throughput Monitor";
const   char      *striZeroToTenMaxThroughput         = "0-10 volts for max throughput";
const   char      *striKgHrControl                    = "kg/hr Control";
const   char      *striGPMGPMEntry                    = "G/m g/m entry";
const   char      *striGPMGPMEntryxxkgs               = "G/m g/m entry xx kgs";
const   char      *striGPMWLEntry                     = "G/m W/L entry";
const   char      *striGPMMicronEntry                 = "G/m Micron Entry";
const   char      *striGPSM                           = "G/SQM Setpoint Entry";
const   char      *striGP12SqFeet                     = "G/12 sq feet";
const   char      *striMultiBlend                     = "Multi Blend";
const   char      *striModeNotKnown                   = "Mode not recoginsed";


const   char      *strTSM1000                         = "TSM 1000";
const   char      *strTSM1500                         = "TSM 1500";
const   char      *strTSM650                          = "TSM 650";
const   char      *strTSM350                          = "TSM 350";
const   char      *strTSM3000                         = "TSM 3000";
const   char      *strOPTIMIX                         = "TSM OPTIMIX";
const   char      *strTSM150                          = "TSM 150";
const   char      *strTSM50                           = "TSM 50";
const   char      *strModelNotKnown                   = "Model not recognised";

// units

const   char      *strMetric                          = "Metric";
const   char      *strSemiImperial                    = "Semi-Imperial";
const   char      *strImperial                        = "Imperial";

// remote modules
const   char      *strSEI                             = "SEI";
const   char      *strLLS                             = "LLS";
const   char      *strSEILLS                          = "SEI-LLS";

// cleaning modes
const   char      *strFullClean                       = "Top & Mixer";
const   char      *strMixerClean                      = "Mixer";

// entry modes
const   char      *strAutoRGt100                       = "Auto R+ > 100";
const   char      *strManualRGt100                     = "Manual R+ > 100";
const   char      *strAutoREq100                       = "Auto R+ = 100";
const   char      *strManualREq100                     = "Manual R+ = 100";


// signal types
const   char      *strPulses                       = "Pulses";
const   char      *strComms                        = "Comms ";

// control type
const   char      *strExtrusion                       = "Extrusion";
const   char      *strHaulOff                         = "Haul Off ";
const   char      *strVFExtrusion                     = "VF Extrusion";
const   char      *strVFHaulOff                       = "VF Haul Off ";

// offline type
const   char      *strOfflineHiLo                     = "Offline mode High/Low Level Sensors";
const   char      *strOfflineXXkgs                    = "Offline mode Blend until xx kgs";

const   char      *strZeroToTenControl                = "Zero To Ten";
const   char      *strIncreaseDecreaseControl         = "Increase Decrease  ";

const   char      *strDeviationAlarmWithoutPause       = "Enabled";
const   char      *strDeviationAlarmWithPause          = "Enabled with pause";

const   char      *strLow                              = "Low";
const   char      *strHigh                             = "High";

const   char      *strNone                              = "None";
const   char      *strAutocycle                         = "AutoCycle";

const   char      *strEvenParity                        = "Even";
const   char      *striBaudRate                          = "Baud Rate";
const   char      *strNormal                            = "Normal";


const   char      *strBaudRate9600                           = "9600";
const   char      *strBaudRate19200                          = "19200";
const   char      *strBaudRate38400                          = "38400";
const   char      *strBaudRate57600                          = "57600";
const   char      *strBaudRate115200                         = "115200";






const   char      *strManualPurge                       = "Manual";
const   char      *strAutoPurge                         = "Auto";

const   char      *strUSBDevice                         = "USB";
const   char      *strPrinterDevice                     = "Printer";

const   char      *striUSBConfigPage                    = "usb config";
const   char      *strUSBEnable                         = "usb/printer";
const   char      *strUSBPrinterAlarmEnable             = "alarm";
const   char      *strUSBPrinterOrderEnable             = "order";
const   char      *strUSBPrinterRollEnable              = "roll";
const   char      *strUSBPrinterHourEnable              = "hour";
const   char      *strUSBPrinterBatchEnable             = "batch";
const   char      *strUSBPrinterEventEnable             = "event";
const   char      *strUSBPrinterHistoryEnable           = "history";
const   char      *strUSBPrinterShiftEnable             = "shift";
const   char      *strUSBPrinterMonthlyEnable           = "month";

const   char      *strFast                              = "fast";



#endif	// TEXTSTRINGSINI_H__



