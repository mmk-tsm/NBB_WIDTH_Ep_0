// P.Smith                          7/7/10
// ultrasonic variables defined
//
// P.Smith                          10/6/11
// added g_fWidthOffsetHistory

// M.McKiernan						5/6/2020
// Added
// extern int g_nUSSensorTriggeredCtr[];		//
// extern int g_nUSSensorEchosReceived[];
// extern bool g_bUSSensorOK[];
// extern bool g_bUSSensorError[];

/////////////////////////////////////////////////

//*****************************************************************************
// MACROS
//*****************************************************************************
#ifndef __WIDTHVARS_H__
#define __WIDTHVARS_H__

#include "General.h"
#include <basictypes.h>
#include "WidthCalibrationDefinitions.h"


extern	unsigned int    g_nSSIFAlarmStatus;
extern	unsigned int    g_nSSIFInputState;
extern  unsigned int    g_nLeftMotorPulses;
extern  unsigned int    g_nRightMotorPulses;
extern  unsigned int    g_nTotalPulsesRead;
extern  unsigned int    g_nLeftSensorADC;
extern  unsigned int    g_nRightSensorADC;
extern  unsigned int    g_nInitiateMinimumWidthCalculate;
extern  unsigned int    g_nMinimumScannerWidth;
extern  unsigned int    g_nLeftSensorStatus;
extern  unsigned int    g_nRightSensorStatus;
extern 	BOOL	g_bLeftLimitSwitchActive;
extern 	BOOL	g_bRightLimitSwitchActive;
extern unsigned int    g_nLeftMotorStatus;
extern unsigned int    g_nRightMotorStatus;
extern unsigned int    g_nWidthIncreaseOnCtr;
extern unsigned int    g_nWidthDecreaseOnCtrCalculatedValue;
extern unsigned int    g_nWidthFastCorrectionIncreaseOnCtr;
extern unsigned int    g_nWidthFCIncreaseOnCtrCalculatedValue;

extern unsigned int    g_nWidthDecreaseOnCtr;
extern unsigned int    g_nWidthIncreaseOnCtrCalculatedValue;

extern unsigned int    g_nWidthProcessDelayCtr;
extern unsigned int    g_nWidthProcessDelayCtrCalculatedValue;
extern unsigned int    g_nControlDirectionOutsideAlarmBand;



extern BOOL	g_bIsStandardising;
extern BOOL	g_bIsStandardisingHistory;
extern BOOL	g_bWidthConfigWritten;
extern BOOL	g_bWidthCalibrateInConfigWritten;
extern BOOL	g_bMaxWidthCalibrateInConfigWritten;
extern BOOL	g_bWidthAutoManualToggleCommandWritten;
extern BOOL	g_bWidthIncreaseCommandWritten;
extern BOOL	g_bWidthIncreaseFastCorrectionCommandWritten;
extern BOOL	g_bWidthDecreaseCommandWritten;

extern BOOL	g_bWidthAuto;
extern BOOL	g_bWidthFastCorrectionIncreaseIsOn;
extern BOOL	g_bWidthIncreaseIsOn;
extern BOOL	g_bWidthDecreaseIsOn;
extern BOOL	g_bAutoCorrectionInProgress;

extern WORD	g_wWidthAverageCTR;

extern  float			g_fTotalScannerInstantaneousWidth;

extern  float	g_fRightWidth;
extern  float	g_fLeftWidth;
extern  float	g_fTotalWidth;
extern  float	g_fWidthSummation;
extern  float	g_fIntegratedWidth;

extern  float	g_fAveragedWidth;

extern float	g_fWidthIntegrationSummation;
extern WORD	g_wWidthIntegrationCtr;
extern float	g_fWidthControlAverage;
extern float	g_fWidthControlAverageError;

extern BOOL	g_bRapidIntegration;
extern BOOL	g_bWidthIsInStartUp;
extern BOOL	g_bCopyWidthConfigFileToSDCard;
extern BOOL	g_bWidthLoadConfigFromSDCard;

extern WORD	g_wSampleIntegationIntervalin50hz;
extern WORD	g_wWidthIntegrationSampleCtr;
extern WORD	g_wWidthCurrentSequence;
extern WORD	g_wStandardiseSettlePeriod;
extern WORD	g_wSSIFCommand;
extern WORD	g_wResetStandardiseCommandCtr;
extern WORD	g_wWidthControlAlarm;
extern WORD	g_wCorrectionsOutSideAlarm;
extern WORD	g_wBubbleBreakInputActiveCtr;
extern WORD	g_wBubbleBreakInputInActiveCtr;
extern BOOL	g_bBubbleBreakActiveTransition;
extern BOOL	g_bBubbleBreakInActiveTransition;
extern WORD	g_wAlarmOnSecondsCounter;
extern WORD	g_wSSIFNoCommsCtr;

extern WidthCalDataStruct	g_stWidthCalibration;
extern BOOL	g_bSaveWidthControlCalibration;

extern BOOL		g_bWidthUltrasonicRefCommandWritten;
extern BOOL		g_bWidthUltrasonicDiameterCommandWritten;

extern float	g_fUltrasonicDivisor;
extern WORD		g_wUltrasonicSensorNo;
extern long	 	g_lUltrasonicEchoTime[];
extern float 	g_fUltrasonicEchoTimeSummation[];
extern WORD		g_wUltrasonicSummationCtr[];
extern long	 	g_lUltrasonicEchoTimeAverage[];
extern float	g_fUSSensorDistance[];
extern float	g_fUSSensorRadius[];
extern float	g_fUSSensorAverageRadius;
extern float	g_fUSSensorAverageDiameter;

extern float	g_fUSSensorAverageRadiusWithOffset;
extern float	g_fUSSensorAverageDiameterWithOffset;
extern float	g_fLayFlatOffset;
extern float	g_fUSSensorAverageLayFlatWidth;
extern float    g_fUSSensorAverageLayFlatWidthWithOffset;
extern WORD	    g_wDeviationExceededCounter[];
extern float	g_fActualWidth;
extern BOOL	g_bAllowFirstUltrasonicRead[];
extern BOOL	g_bFirstReferenceAutoCalibrate;
extern BOOL	g_bWriteWidthDiagnosticsToSd;
extern BOOL	g_bBubbleBreakActive;
extern BOOL	g_bWidthAlarmIsActive;
extern BOOL	g_bWidthUltrasonicLayFlatOffsetSignWritten;
extern BOOL	g_bWidthUltrasonicLayFlatOffsetWritten;
extern BOOL	g_bWidthUltrasonicDiameterFromLayFlatEntry;
extern BOOL	g_bWidthSetPointWritten;

extern float	g_fDesiredWidthHistory;
extern float	g_fWidthOffsetHistory;
extern StructDebugWidth g_st_WidthDebug;

//wfh
extern int g_nUSSensorTriggeredCtr[];		//
extern int g_nUSSensorEchosReceived[];
extern bool g_bUSSensorOK[];
extern bool g_bUSSensorError[];

#endif //__WIDTHVARS_H__
