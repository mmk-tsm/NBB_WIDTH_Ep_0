///////////////////////////////////////////////////////////
// WidthForeGround.c
//
// P.Smith							10/6/11
// call CalculateWidthOffsetMinWidthInSteps
// set up g_fWidthOffsetHistory
// M.McKiernan						15/5/2020
// Initialisation of sensor errors, alarms.
//////////////////////////////////////////////////////////

//*****************************************************************************
// INCLUDES
//*****************************************************************************
#include <stdio.h>
#include "WidthVars.h"
#include "WidthForeground.h"
#include "WidthDef.h"
#include "WidthCalibrationDefinitions.h"
#include "Batvars.h"
#include "WidthRWToNonVolatileMemory.h"
#include "WidthMB.h"
#include "CopyWidthCalibrationDataToIniFile.h"
#include "SetpointFormat.h"
#include <String.h>

extern structSetpointData   g_CurrentRecipe;
extern bool g_bUSSensorInAlarm[];
// Locally declared global variables


/* ====================================================================
Function:     WidthInitialisation
Parameters:   N/A
Returns:      N/A

This function initialises the width control
======================================================================= */
float g_fLAB;
float g_fLBC;
float g_fLAC;

void WidthInitialisation( void )
{
	unsigned int i;
	// initialise variables

	for(i=0; i < NUMBER_OF_ULTRASONICS_SENSORS; i++)
	{
		g_lUltrasonicEchoTime[i] = 0;
		g_fUltrasonicEchoTimeSummation[i]= 0.0f;
		g_wUltrasonicSummationCtr[i]= 0.0f;
		g_lUltrasonicEchoTimeAverage[i]= 0.0f;
		g_fUSSensorDistance[i]= 0.0f;
		g_fUSSensorRadius[i] = 0.0f;
		g_wDeviationExceededCounter[i] = 0;
		g_bAllowFirstUltrasonicRead[i] = TRUE;
//wfh 11.5.2020
		g_nUSSensorTriggeredCtr[i] = 0;		//
		g_nUSSensorEchosReceived[i] = 0;
		g_bUSSensorOK[i] = FALSE;
		g_bUSSensorError[i] = FALSE;
		g_bUSSensorInAlarm[i] = FALSE;
	}

    memset( &g_st_WidthDebug, 0, sizeof( StructDebugWidth ) );
	LoadWidthCalibrationFromEE();
	//wfh

	g_fLAB = g_stWidthCalibration.m_fLength_AB;
	g_fLBC = g_stWidthCalibration.m_fLength_BC;
	g_fLAC = g_stWidthCalibration.m_fLength_AC;

	CheckForValidWidthConfigData(); // check for valid config data
	DeleteWidthDefaultFile();
	WriteWidthCalibrationDataToSDCard(); // write width calibration to sd card
	g_wWidthCurrentSequence = WIDTH_INTEGRATION_SEQUENCE;
	SetMaxMotorPulses(9999,9999); //nbb--widthtestonly--remove
    SetSSIFWidthControlType();  // sets small line if required

	g_wStandardiseSettlePeriod = STANDARDISE_SETTLE_PERIOD;
	g_fDesiredWidthHistory = g_CurrentRecipe.m_fDesiredWidth;
	CalculateWidthOffsetMinWidthInSteps();
	CopyWidthConfigToMB();
	g_fWidthOffsetHistory = g_stWidthCalibration.m_fWidthOffset;

}




