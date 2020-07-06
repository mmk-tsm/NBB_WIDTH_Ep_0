///////////////////////////////////////////////////////////
// WidthForeGround.c
// P.Smith							31/5/10
// allow for width calculation for small line system.
//
// P.Smith							7/7/10
// added width control write to sd card.
// if layflat is selected, copy scanner width to g_fActualWidth
// calculate small line width
// add WidthFGOneMinute and call to AutoCalibrateReferenceSensor
// use g_fActualWidth for summation g_fWidthSummation
// this allows the correct integration to happen if the blender is either
// layflat or ultrasonic
//
// P.Smith							8/7/10
// check for hourly standardise option before standardising.
//
// P.Smith							28/3/11
// double the width averaging to 2.
//
// P.Smith							10/6/11
// added CalculateWidthOffsetMinWidthInSteps for small line.

// M.McKiernan  					22/5/2020		//wfh
//	if(g_bWriteWidthMeasurementDiagnosticsToSd)
//	g_bWriteWidthMeasurementDiagnosticsToSd = FALSE;
//			WriteWidthMeasurementDiagnosticDataToSDCard();

//////////////////////////////////////////////////////////

//*****************************************************************************
// INCLUDES
//*****************************************************************************
#include <stdio.h>
#include "WidthVars.h"
#include "WidthForeground.h"
#include "WidthDef.h"
#include "BatchCalibrationFunctions.h"
#include "Batvars.h"
#include "WidthRWToNonVolatileMemory.h"
#include "WidthMB.h"
#include "CopyWidthCalibrationDataToIniFile.h"
#include "LoadWidthCalDataFromSDCard.h"
#include "WidthUltrasonics.h"
#include "WidthDiagnostics.h"

extern CalDataStruct	g_CalibrationData;
extern int g_arrnWriteSSIFMBTable[];
extern int g_arrnReadSSIFMBTable[];

extern BOOL	g_bSSIFWriteCommandRequired;
extern BOOL	g_bSSIFWriteCalibrationRequired;
WORD	wWidthHistoryAveragingInSeconds = 0;

extern bool	g_bWriteWidthMeasurementDiagnosticsToSd;
// Locally declared global variables

void WidthForeground( void )
{
	CalculateLayFlatWidth();
	if(g_bSaveWidthControlCalibration)
	{
		g_bSaveWidthControlCalibration = FALSE;
		SaveWidthControlCalibrationToEE();
	}
	CheckForWidthMBWrite(); //nbb--widthtodo-- should this be run faster
	if(g_bCopyWidthConfigFileToSDCard)
	{
		g_bCopyWidthConfigFileToSDCard = FALSE;
		WriteWidthCalibrationDataToSDCard();
	}
	if(g_bWidthLoadConfigFromSDCard)
	{
		g_bWidthLoadConfigFromSDCard = FALSE;
        //g_nFunctionRunning = FUNCTION_18;  //nbb--todo--width
	    //SaveFuntionNosInRAM();  //nbb--todo--width
        LoadWidthConfigFromSDCard();
	}
	if(g_bWriteWidthDiagnosticsToSd)
	{
	      g_bWriteWidthDiagnosticsToSd = FALSE;
	      WriteWidthDiagnosticDataToSDCard();
	}
	//wfh
	if(g_bWriteWidthMeasurementDiagnosticsToSd)
	{
			g_bWriteWidthMeasurementDiagnosticsToSd = FALSE;
			WriteWidthMeasurementDiagnosticDataToSDCard();
			//TODO debugging 26.6.2020 WriteWidthMeasurementDiagnosticDataToSDCard();

	}
}


/* ====================================================================
Function:     CalculateLayFlatWidth
Parameters:   N/A
Returns:      N/A

======================================================================= */

void CalculateLayFlatWidth( void )
{
	if((g_stWidthCalibration.m_wWidthMode == LAYFLAT_MODE)||(g_stWidthCalibration.m_wWidthMode == ULTRASONIC_AND_LAYFLAT_MODE))
	{
		g_fRightWidth = g_nRightMotorPulses /(STEPPER_MOTOR_MM_CONVERSION_FACTOR);
		g_fLeftWidth = g_nLeftMotorPulses /(STEPPER_MOTOR_MM_CONVERSION_FACTOR);
		g_fTotalWidth = g_fRightWidth + g_fLeftWidth;
		if(g_stWidthCalibration.m_wWidthType == SMALL_LINE_TYPE)
		{
			g_fTotalScannerInstantaneousWidth = g_stWidthCalibration.m_fWidthOffset - g_fTotalWidth;
		}
		else
		{
			g_fTotalScannerInstantaneousWidth = g_stWidthCalibration.m_fWidthOffset + g_fTotalWidth;
		}
		if(g_stWidthCalibration.m_wWidthMode == LAYFLAT_MODE)
		{
			g_fActualWidth = g_fTotalScannerInstantaneousWidth;
		}
	}
}

/* ====================================================================
Function:     CalculateMinimumWidth
Parameters:   N/A
Returns:      N/A

This function calculates the layflat width
width = width offset+ (left pulses / conversion factor) + (right pulses / conversion factor)
======================================================================= */

float CalculateMinimumWidth( unsigned int nWidth )
{
	float fMinWidth;
	fMinWidth  = nWidth -  (g_fRightWidth + g_fLeftWidth);
	return(fMinWidth);
}


/* ====================================================================
Function:     WidthHourFG
Parameters:   N/A
Returns:      N/A

======================================================================= */

void WidthHourFG( void )
{
	if(g_stWidthCalibration.m_bStandardiseOnTheHour)
	{
	    InitiateStandardise();
	}
}

/* ====================================================================
Function:     InitiateStandardise
Parameters:   N/A
Returns:      N/A

======================================================================= */

void InitiateStandardise( void )
{
	g_wSSIFCommand |= SSIF_CMD_STANDARDISE_BIT;
	g_arrnWriteSSIFMBTable[MB_SSIF_COMMAND] = g_wSSIFCommand; //
	g_bSSIFWriteCommandRequired = TRUE;
	g_wResetStandardiseCommandCtr = 3;  // reset std command
}


/* ====================================================================
Function:     ResetStandardiseCommand
			  Resets the standardise command bit.
Parameters:   N/A
Returns:      N/A

======================================================================= */

void ResetStandardiseCommand( void )
{
	WORD wTemp = ~SSIF_CMD_STANDARDISE_BIT;
	g_wSSIFCommand = g_wSSIFCommand & wTemp;
	g_arrnWriteSSIFMBTable[MB_SSIF_COMMAND] = g_wSSIFCommand; //
	g_bSSIFWriteCommandRequired = TRUE;
}



/* ====================================================================
Function:     InitiateStandardise
Parameters:   N/A
Returns:      N/A

======================================================================= */

void SetMaxMotorPulses( unsigned int unMaxLeftPulses,unsigned int unMaxRightPulses)
{
	g_arrnWriteSSIFMBTable[MB_SSIF_MAX_TRAVEL_MOTOR1_LEFT_STEPS] = unMaxLeftPulses; //
	g_arrnWriteSSIFMBTable[MB_SSIF_MAX_TRAVEL_MOTOR2_RIGHT_STEPS] = unMaxRightPulses; //
	g_bSSIFWriteCalibrationRequired = TRUE;
}


/* ====================================================================
Function:     CalculateWidthAverage
Parameters:   N/A
Returns:      N/A

======================================================================= */

void CalculateWidthAverage( void)
{
	WORD	wWidthAveraging;
	// check for change in width average
	if(g_stWidthCalibration.m_wWidthAveragingInSeconds != wWidthHistoryAveragingInSeconds)
	{
		g_wWidthAverageCTR = 0;
		g_fWidthSummation = 0.0f;
	}
	wWidthHistoryAveragingInSeconds = g_stWidthCalibration.m_wWidthAveragingInSeconds;

	g_fWidthSummation += g_fActualWidth;

	g_wWidthAverageCTR++;
	if(g_stWidthCalibration.m_wWidthAveragingInSeconds ==0)
	{
		wWidthAveraging = 1;
	}
	else
	{
		wWidthAveraging = g_stWidthCalibration.m_wWidthAveragingInSeconds;
	}

	wWidthAveraging = wWidthAveraging * 2;  // make value double
	if(g_wWidthAverageCTR >= wWidthAveraging)
	{
		// calculate width average
		g_fAveragedWidth = g_fWidthSummation /float(wWidthAveraging);
		g_wWidthAverageCTR = 0;
		g_fWidthSummation = 0.0f;
	}
	if(fdTelnet>0)
	{
	//	printf("\n ctr %d summation %f average %f",g_wWidthAverageCTR,ftemp,g_fAveragedWidth);
	}
}






/* ====================================================================
Function:     SetSSIFWidthControlType
Parameters:   N/A
Returns:      N/A

======================================================================= */

void SetSSIFWidthControlType( void)
{
	if(g_stWidthCalibration.m_wWidthType == SMALL_LINE_TYPE)
	{
		g_wSSIFCommand |= SSIF_CMD_SCANNER_TYPE_BIT;
	}
	else
	{
		g_wSSIFCommand = g_wSSIFCommand & (~SSIF_CMD_SCANNER_TYPE_BIT);
	}
	g_arrnWriteSSIFMBTable[MB_SSIF_COMMAND] = g_wSSIFCommand; //
	g_bSSIFWriteCommandRequired = TRUE;
}




/* ====================================================================
Function:     WidthFGOneMinute
Parameters:   N/A
Returns:      N/A

======================================================================= */

void WidthFGOneMinute( void)
{
	AutoCalibrateReferenceSensor();
}
/* ====================================================================
Function:     CalculateWidthOffsetMinWidthInSteps
Parameters:   N/A
Returns:      N/A

======================================================================= */

void CalculateWidthOffsetMinWidthInSteps( void)
{
	int	nWidthOffsetPulses = (int)(STEPPER_MOTOR_MM_CONVERSION_FACTOR * g_stWidthCalibration.m_fWidthOffset);
	int nMinWidth = (int)(MINIMUM_WIDTH_FOR_SMALL_LINE * STEPPER_MOTOR_MM_CONVERSION_FACTOR);
	g_arrnWriteSSIFMBTable[MB_SSIF_WIDTH_OFFSET_STEPS] = nWidthOffsetPulses;
	g_arrnWriteSSIFMBTable[MB_SSIF_MIN_WIDTH_STEPS] = nMinWidth;
//	if(fdTelnet>0)
//		printf("\n width offset %f mm steps %d min width pulses %d",g_stWidthCalibration.m_fWidthOffset,nWidthOffsetPulses,nMinWidth);
	g_bSSIFWriteCalibrationRequired = TRUE;
}
