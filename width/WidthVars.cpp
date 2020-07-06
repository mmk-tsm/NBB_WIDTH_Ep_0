// P.Smith                          7/7/10
// added width ultrasonic variables
// name change g_fTotalInstantaneousWidth to g_fTotalScannerInstantaneousWidth
//
// P.Smith                          9/7/10
// added g_wAlarmOnSecondsCounter
//
// P.Smith                          19/7/10
// added 	g_bWidthUltrasonicLayFlatOffsetSignWritten,g_bWidthUltrasonicLayFlatOffsetWritten
// g_bWidthUltrasonicDiameterFromLayFlatEntry = FALSE;
//
// P.Smith                          12/8/10
// added g_bWidthSetPointWritten
//
// P.Smith                          20/9/10
// added g_wSSIFNoCommsCtr
//
// P.Smith                          10/6/11
// define g_fWidthOffsetHistory
/////////////////////////////////////////////////

//*****************************************************************************
// MACROS
//*****************************************************************************

#include "General.h"
#include <basictypes.h>
#include "WidthCalibrationDefinitions.h"
#include "WidthDef.h"


unsigned int    g_nSSIFAlarmStatus = 0;
unsigned int    g_nSSIFInputState = 0;
unsigned int    g_nLeftMotorPulses = 0;
unsigned int    g_nRightMotorPulses = 0;
unsigned int    g_nTotalPulsesRead = 0;
unsigned int    g_nLeftSensorADC = 0;
unsigned int    g_nRightSensorADC = 0;
unsigned int    g_nInitiateMinimumWidthCalculate = 0;
unsigned int    g_nMinimumScannerWidth = 0;
unsigned int    g_nLeftSensorStatus = 0;
unsigned int    g_nRightSensorStatus = 0;
BOOL	g_bLeftLimitSwitchActive = FALSE;
BOOL	g_bRightLimitSwitchActive = FALSE;
BOOL	g_bIsStandardising = FALSE;
BOOL	g_bIsStandardisingHistory = FALSE;
BOOL	g_bSaveWidthControlCalibration = FALSE;
BOOL	g_bWidthConfigWritten = FALSE;
BOOL	g_bWidthCalibrateInConfigWritten = FALSE;
BOOL	g_bMaxWidthCalibrateInConfigWritten = FALSE;
BOOL	g_bWidthAutoManualToggleCommandWritten = FALSE;
BOOL	g_bWidthIncreaseCommandWritten = FALSE;
BOOL	g_bWidthIncreaseFastCorrectionCommandWritten= FALSE;
BOOL	g_bWidthDecreaseCommandWritten= FALSE;
BOOL	g_bWidthAuto = FALSE;
BOOL	g_bWidthFastCorrectionIncreaseIsOn = FALSE;
BOOL	g_bWidthIncreaseIsOn = FALSE;
BOOL	g_bWidthDecreaseIsOn = FALSE;
BOOL	g_bAutoCorrectionInProgress = FALSE;

WidthCalDataStruct	g_stWidthCalibration;

unsigned int    g_nLeftMotorStatus = 0;
unsigned int    g_nRightMotorStatus = 0;
unsigned int    g_nWidthIncreaseOnCtr = 0;
unsigned int    g_nWidthIncreaseOnCtrCalculatedValue = 0;
unsigned int    g_nWidthFastCorrectionIncreaseOnCtr = 0;
unsigned int    g_nWidthFCIncreaseOnCtrCalculatedValue = 0;

unsigned int    g_nWidthDecreaseOnCtr = 0;
unsigned int    g_nWidthDecreaseOnCtrCalculatedValue = 0;

unsigned int    g_nWidthProcessDelayCtr = 0;
unsigned int    g_nWidthProcessDelayCtrCalculatedValue = 0;
unsigned int    g_nControlDirectionOutsideAlarmBand = CONTROL_IS_OFF;


WORD	g_wWidthAverageCTR = 0;


float	g_fTotalScannerInstantaneousWidth = 0.0f;
float	g_fRightWidth = 0.0f;
float	g_fLeftWidth = 0.0f;
float	g_fTotalWidth = 0.0f;
float	g_fWidthSummation = 0.0f;
float	g_fAveragedWidth = 0.0f;

// integration stuff
float	g_fWidthIntegrationSummation = 0.0f;
float	g_fIntegratedWidth = 0.0f;

WORD	g_wWidthIntegrationCtr = 0;
float	g_fWidthControlAverage = 0.0f;
float	g_fWidthControlAverageError = 0.0f;

BOOL	g_bRapidIntegration = FALSE;
BOOL	g_bWidthIsInStartUp = FALSE;
BOOL	g_bCopyWidthConfigFileToSDCard = FALSE;
BOOL	g_bWidthLoadConfigFromSDCard = FALSE;

WORD	g_wSampleIntegationIntervalin50hz = 0;
WORD	g_wWidthIntegrationSampleCtr = 0;
WORD	g_wWidthCurrentSequence = 0;
WORD	g_wStandardiseSettlePeriod = 0;
WORD	g_wSSIFCommand = 0;
WORD	g_wResetStandardiseCommandCtr = 0;
WORD	g_wWidthControlAlarm = 0;
WORD	g_wCorrectionsOutSideAlarm = 0;
WORD	g_wBubbleBreakInputActiveCtr = 0;
WORD	g_wBubbleBreakInputInActiveCtr = 0;
WORD	g_wAlarmOnSecondsCounter = 0;
WORD	g_wSSIFNoCommsCtr = 0;


// ultrasonic variables
BOOL	g_bWidthUltrasonicRefCommandWritten = FALSE;
BOOL	g_bWidthUltrasonicDiameterCommandWritten = FALSE;

float	g_fUltrasonicDivisor = 0.0f;
WORD	g_wUltrasonicSensorNo = 0;
long	g_lUltrasonicEchoTime[NUMBER_OF_ULTRASONICS_SENSORS];
float	g_fUltrasonicEchoTimeSummation[NUMBER_OF_ULTRASONICS_SENSORS];
WORD	g_wUltrasonicSummationCtr[NUMBER_OF_ULTRASONICS_SENSORS];
long	g_lUltrasonicEchoTimeAverage[NUMBER_OF_ULTRASONICS_SENSORS];
float	g_fUSSensorDistance[NUMBER_OF_ULTRASONICS_SENSORS];
float	g_fUSSensorRadius[NUMBER_OF_ULTRASONICS_SENSORS];
float	g_fUSSensorAverageRadius= 0.0f;
float	g_fUSSensorAverageDiameter= 0.0f;

float	g_fUSSensorAverageRadiusWithOffset= 0.0f;
float	g_fUSSensorAverageDiameterWithOffset= 0.0f;
float	g_fUSSensorAverageLayFlatWidth= 0.0f;
float   g_fUSSensorAverageLayFlatWidthWithOffset= 0.0f;
float	g_fLayFlatOffset= 0.0f;
float	g_fActualWidth = 0.0f;

WORD	g_wDeviationExceededCounter[NUMBER_OF_ULTRASONICS_SENSORS];
BOOL	g_bAllowFirstUltrasonicRead[NUMBER_OF_ULTRASONICS_SENSORS];
BOOL	g_bFirstReferenceAutoCalibrate = TRUE;
BOOL	g_bWriteWidthDiagnosticsToSd = FALSE;
BOOL	g_bBubbleBreakActive = FALSE;
BOOL	g_bBubbleBreakActiveTransition = FALSE;
BOOL	g_bBubbleBreakInActiveTransition = FALSE;
BOOL	g_bWidthAlarmIsActive = FALSE;
BOOL	g_bWidthUltrasonicLayFlatOffsetSignWritten = FALSE;
BOOL	g_bWidthUltrasonicLayFlatOffsetWritten = FALSE;
BOOL	g_bWidthUltrasonicDiameterFromLayFlatEntry = FALSE;
BOOL	g_bWidthSetPointWritten = FALSE;

float	g_fDesiredWidthHistory = 0;
float	g_fWidthOffsetHistory = 0.0f;
StructDebugWidth g_st_WidthDebug;
