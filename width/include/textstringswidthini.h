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
//
// P.Smith                              7/7/10
// added strings for width control dtc,counts pmm,ref distance

// M.McKiernan                         5/6/2020
// Added following
// const	char	 *striUltrasonicAveraging =	"Ultrasonic Averaging";
// const	char	 *striWidthOverrun = 		"Width Overrun";
// const	char	 *striWidthOverrunDeadband = "Width Overrun Deadband";
// const	char	 *striStandardiseOnAuto = 	"Standardise on Auto";
// const	char	 *striStandardiseOnHour = 	"Standardise on Hour";
// const	char	 *striAlarmInManual  = 		"Alarm in Manual";
// const	char	 *striFastCorrectionEnabled = "Fast Correction Enabled";

// new parameters
// const	char	 *striLengthAB = 			"Length AB";
// const	char	 *striLengthBC = 			"Length BC";
// const	char	 *striLengthAC = 			"Length AC";
// const	char	 *striSensorAngleA =		"Sensor Angle A";
// const	char	 *striSensorAngleB =		"Sensor Angle B";
// const	char	 *striSensorAngleC =		"Sensor Angle C";
// const	char	 *striM2WidthOffset =		"M2 Width Offset";
// const	char	 *striM2WidthOffsetSign =	"M2 Width Offset Sign";

////////////////////////////////////////////////

#ifndef __TEXTSTRINGSWIDTHINI_H__
#define __TEXTSTRINGSWIDTHINI_H__

#include "General.h"
#include "AlarmFunctions.h"

// These will not be translated and so are not set up
// as part of the language arrays.

const   char      *striWidthConfig              	= "width";

const   char      *striWidthCalibationMode     = "mode";
const   char      *striWidthCalibationType     = "type";
const   char      *striWidthCalibationLineType     = "line type";
const   char      *striWidthCalibationAlarmLimit   = "alarm limit";
const   char      *striWidthCalibationProcessDelay = "process delay";
const   char      *striWidthCalibationIntegrationTime = "integ time";
const   char      *striWidthCalibationDeadband = "width deadband";
const   char      *striWidthCalibationNormalIncreaseControlGain = "increase control gain";
const   char      *striWidthCalibationNormalDecreaseControlGain = "decrease control gain";
const   char      *striWidthCalibationFCConrolGain = "fc control gain";
const   char      *striWidthCalibationAveraging = 	 "averaging";

const   char      *striWidthUSDistanceToCentre1 = 	 "Distance to centre sensor #1";
const   char      *striWidthUSDistanceToCentre2 = 	 "Distance to centre sensor #2";
const   char      *striWidthUSDistanceToCentre3 = 	 "Distance to centre sensor #3";
const   char      *striWidthUSCountsPerMM = 	 "counts per mm";
const   char      *striWidthRefDistanceCalibratedAt = 	 "reference calibration distance";



const   char      *striLayflat 	  =  "Layflat";
const   char      *striUltrasonic =  "Ultrasonic";
const   char      *striUltrasonicAndLayflat =  "Ultrasonic And Layflat";

const   char      *striStandardScanner =  "Standard Scanner";
const   char      *striSmallLineScanner =  "Small Line Scanner";

const   char      *striBlownFilmLine = 	 "Blown Film";
const   char      *striIBCLine       = 	 "IBC";
const   char      *striCryovacLine   = 	 "Cryovac";
const   char      *striWidthModeNotKnown   = 	 "Unrecognised mode";

//wfh

const	char	 *striUltrasonicAveraging =	"Ultrasonic Averaging";
const	char	 *striWidthOverrun = 		"Width Overrun";
const	char	 *striWidthOverrunDeadband = "Width Overrun Deadband";
const	char	 *striStandardiseOnAuto = 	"Standardise on Auto";
const	char	 *striStandardiseOnHour = 	"Standardise on Hour";
const	char	 *striAlarmInManual  = 		"Alarm in Manual";
const	char	 *striFastCorrectionEnabled = "Fast Correction Enabled";

// new parameters
const	char	 *striLengthAB = 			"Length AB";
const	char	 *striLengthBC = 			"Length BC";
const	char	 *striLengthAC = 			"Length AC";
const	char	 *striSensorAngleA =		"Sensor Angle A";
const	char	 *striSensorAngleB =		"Sensor Angle B";
const	char	 *striSensorAngleC =		"Sensor Angle C";
const	char	 *striM2WidthOffset =		"M2 Width Offset";
const	char	 *striM2WidthOffsetSign =	"M2 Width Offset Sign";



#endif	// __TEXTSTRINGSWIDTHINI_H__



