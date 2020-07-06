//
//  P.Smith							7/7/2010
//  CheckForNewWidthSetPoint,CalculateUltrasonicDistances calls added.
//
//  P.Smith							9/7/2010
//  added CheckForCryovacAlarmPeriod call
//
// P.Smith							20/9/10
// added CheckForSSIFComms to check the ssif comms, if the comms has not run for
// more than 10 seconds.
//
// P.Smith							10/6/11
// check for a change in the width offset value, if it changes then calculate
// minumum steps
// M.McKiernan						23/4/2020
// See wfh
// #define WIDTH_MEASUREMENT_DIAGNOSTICS_PERIOD	(20)
// In void WidthOneSecond( void )
// Call GenerateWidthMeasurementDiagnostics( ); every WIDTH_MEASUREMENT_DIAGNOSTICS_PERIOD.
// Calibrater reference if new reference value uploaded from SD card.
// if( g_bUploadedRefLength ) - CalibrateUltrasonicReferenceSensor( g_stWidthCalibration.m_wUltrasonicMMCalibratedAt );
// .
// 	// wfh MMK 23.4.2020. - Method 2 for calculating bubble diameter.
//		CalculateSensorCoordinates();	// coordinates of faces of sensors.
//		CalculateBubbleCoordinates();	// coordinates of intersects on bubble.
//		float fRad2 = CalculateRadiusFrom3Points( g_fB1x, g_fB1y, g_fB2x, g_fB2y, g_fB3x, g_fB3y);
//		float fDiam2 = fRad2*2;
//		float fLayflat2 = (PI * fDiam2)/2;
//	g_fUSMethod2Diameter = fDiam2;
//	g_fUSMethod2Layflat = fLayflat2;
	//sensor to bubble distances.   //
			//d1 = g_fUSSensorDistance[0];
			////d2 = g_fUSSensorDistance[1];
			//d3 = g_fUSSensorDistance[2];

//	float fDValue = g_fUSMethod2Diameter*10;		// in both cases display to 10ths.  0.1mm and 0.1".
//	float fLFValue= g_fUSMethod2Layflat*10;

//	if(g_fWidthConversionFactor != 1.0f)
//	{
//		fDValue *= g_fWidthConversionFactor;  //nbb--todo-- check this again
//		fLFValue *= g_fWidthConversionFactor;  //nbb--todo-- check this again
//	}
//	g_arrnMBTable[WIDTH_SUMMARY_D_METHOD2] 	= (WORD)(fDValue);
//	g_arrnMBTable[WIDTH_SUMMARY_LF_METHOD2] = (WORD)(fLFValue);
// 15.6.2020 - the values for Method2 are not taking account of the layflat offset.
//
//*****************************************************************************
// INCLUDES
//*****************************************************************************
#include <stdio.h>
#include "WidthVars.h"
#include "WidthMB.h"
#include "BatVars.h"
#include "WidthForeground.h"
#include "Width.h"
#include "WidthControlAlgorithm.h"
#include "WidthUltrasonics.h"
#include "WidthOneHz.h"
#include "Alarms.h"
#include "SerialStuff.h"
#include "Debug.h"
#include "BatVars2.h"
#include "WidthDiagnostics.h"

extern  structControllerData    structSSIFData;
extern  StructDebug   g_nTSMDebug;
extern  float g_fB1x;		// where sensor A (1) touches bubble.
extern  float g_fB1y;
extern  float g_fB2x;		// where sensor B (2) touches bubble.
extern  float g_fB2y;
extern  float g_fB3x;		// where sensor C (3) touches bubble.
extern  float g_fB3y;

extern  bool  g_bUploadedRefLength;

long	g_structSSIFDataGoodRxHistory = 0;
float 	g_fUSMethod2Diameter;
float 	g_fUSMethod2Layflat;

int 	g_nWidthMeasurementDiagnosticsTimer = 0;
WORD    g_nWidthControlInAutoTimer = 0;       //timer for when in Auto.

#define WIDTH_MEASUREMENT_DIAGNOSTICS_PERIOD	(20)
//////////////////////////////////////////////////////
// CopyWidthSummmaryDataFromSSIF
//
//
//////////////////////////////////////////////////////
void WidthOneSecond( void )
{
	if( g_bWidthAuto )
	{
		if(g_nWidthControlInAutoTimer < 60000)
		{
			g_nWidthControlInAutoTimer++;
		}
	}
	else
		g_nWidthControlInAutoTimer = 0;

	// Generate width measurement diagnostics.
	g_nWidthMeasurementDiagnosticsTimer++;
	if( g_nWidthMeasurementDiagnosticsTimer > WIDTH_MEASUREMENT_DIAGNOSTICS_PERIOD  )
	{
		g_nWidthMeasurementDiagnosticsTimer = 0;
		GenerateWidthMeasurementDiagnostics( );
	}
	if( g_bUploadedRefLength )
	{
		// check for valid data from US reference sensor.
		if( (g_lUltrasonicEchoTimeAverage[REFERENCE_SENSOR_NUMBER-1] > 10) && g_bUSSensorOK[REFERENCE_SENSOR_NUMBER-1] )
		{
			CalibrateUltrasonicReferenceSensor( g_stWidthCalibration.m_wUltrasonicMMCalibratedAt );
			g_bUploadedRefLength = FALSE;
		}
	}

	// check for change in width offset.
	if(g_stWidthCalibration.m_fWidthOffset != g_fWidthOffsetHistory)
	{
		g_fWidthOffsetHistory = g_stWidthCalibration.m_fWidthOffset;
		if(g_stWidthCalibration.m_wWidthType == SMALL_LINE_TYPE)
		{
			CalculateWidthOffsetMinWidthInSteps();
		}
	}
	CheckForSSIFComms();
	if(g_wResetStandardiseCommandCtr >0)
	{
		g_wResetStandardiseCommandCtr--;
		if(g_wResetStandardiseCommandCtr == 0)
		{
			ResetStandardiseCommand();
		}

	}
	if(g_wStandardiseSettlePeriod > 0)
	{
		g_wStandardiseSettlePeriod--;
	}
	CopyWidthSummmaryDataFromSSIF();
	CopyWidthSummaryDataTOMB();
	CalculateWidthAverage();
	CheckForChangeInErrorDirection();
	CheckForNewWidthSetPoint();
	SwitchWidthAlarmOnOrOff();
	if(fdTelnet>0)
	{
//		printf("\n average width is inst %f avg %f",g_fTotalScannerInstantaneousWidth,g_fAveragedWidth);
//		iprintf("\n dir is %d corrinp %d",g_nControlDirectionOutsideAlarmBand,g_bAutoCorrectionInProgress);
//		printf("\n r pulses %d width %f LPulses %d width %f total %f ",g_nRightMotorPulses,g_fRightWidth,g_nLeftMotorPulses,g_fLeftWidth,g_fTotalInstantaneousWidth);
	}
	CalculateUltrasonicDistances();

	// wfh MMK 23.4.2020. - Method 2 for calculating bubble diameter.
	CalculateSensorCoordinates();	// coordinates of faces of sensors.
	CalculateBubbleCoordinates();	// coordinates of intersects on bubble.

	if( (fdTelnet>0) && g_nTSMDebug.m_bUltrasonicsDebug )
		printf("\nCalculateRadiusFrom3Points:  g_fB1x= %5.1f g_fB1y= %5.1f g_fB2x= %5.1f g_fB2y= %5.1f g_fB3x= %5.1f g_fB3y= %5.1f", g_fB1x, g_fB1y, g_fB2x, g_fB2y, g_fB3x, g_fB3y);

	float fRad2 = CalculateRadiusFrom3Points( g_fB1x, g_fB1y, g_fB2x, g_fB2y, g_fB3x, g_fB3y);
	float fDiam2 = fRad2*2;
	float fLayflat2 = (PI * fDiam2)/2;
		g_fUSMethod2Diameter = fDiam2;
		g_fUSMethod2Layflat = fLayflat2;
	if( (fdTelnet>0) && g_nTSMDebug.m_bUltrasonicsDebug )
			printf("\nMethod2 - r2= %5.1f D2 = %5.1f  LF2 = %5.1f d1= %5.1f d2= %5.1f d3= %5.1f M2Offset= %5.1f M2Osign= %d",fRad2, fDiam2, fLayflat2,
					g_fUSSensorDistance[0], g_fUSSensorDistance[1], g_fUSSensorDistance[2],
					g_stWidthCalibration.m_fM2WidthOffset, g_stWidthCalibration.m_nM2WidthOffsetSign  );
	//sensor to bubble distances.   //

		//d1 = g_fUSSensorDistance[0];
		////d2 = g_fUSSensorDistance[1];
		//d3 = g_fUSSensorDistance[2];

float fDValue = g_fUSMethod2Diameter*10;		// in both cases display to 10ths.  0.1mm and 0.1".
float fLFValue= g_fUSMethod2Layflat*10;

	if(g_fWidthConversionFactor != 1.0f)
	{
		fDValue *= g_fWidthConversionFactor;  //nbb--todo-- check this again
		fLFValue *= g_fWidthConversionFactor;  //nbb--todo-- check this again
	}

	g_arrnMBTable[WIDTH_SUMMARY_D_METHOD2] 	= (WORD)(fDValue);
	g_arrnMBTable[WIDTH_SUMMARY_LF_METHOD2] = (WORD)(fLFValue);

	CheckForCryovacAlarmPeriod(); // check if alarm is on for a specific period -> manual
}




/* ====================================================================
Function:     CheckForSSIFComms    // asm = CHECKFORSSIFCOMMS
Parameters:   N/A
Returns:      N/A

======================================================================= */
void CheckForSSIFComms( void )
{
	if((g_stWidthCalibration.m_wWidthMode == LAYFLAT_MODE)||(g_stWidthCalibration.m_wWidthMode == ULTRASONIC_AND_LAYFLAT_MODE))
	{
		g_wSSIFNoCommsCtr++; // increment counter
		if(g_wSSIFNoCommsCtr >= SSIF_COMMUNICATIONS_TIMEOUT_SECS)
		{

            PutAlarmTable( WIDTH_SSIF_COMMS_ALARM,  0 );     // indicate comms alarm
			g_wSSIFNoCommsCtr = 0;
			WidthToManual();
		}
		// check if good rx ctr is changing - clear no comms ctr
		if( structSSIFData.m_lGoodRx !=  g_structSSIFDataGoodRxHistory)
		{
			g_wSSIFNoCommsCtr = 0;  // counter must be incrementing
            RemoveAlarmTable( WIDTH_SSIF_COMMS_ALARM,  0 );     // alarm cleared
		}
		g_structSSIFDataGoodRxHistory = structSSIFData.m_lGoodRx;
	}
}
