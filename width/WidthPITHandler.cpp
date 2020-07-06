////////////////////////////////////////////////////////////////////////////////
//
// P.Smith                          7/7/10
// correct set of g_bAutoCorrectionInProgress must include fast correction counter.
// call MonitorBubbleBreak to monitor bubble break on the 50hz
// if paused, then no ultrasonic read is initiated.

// M.McKiernan						11/5/2020
// in void WidthFiftyHertzHandler( void ) added code to check ultrasonic sensors.
//     	g_nUSSensorTriggeredCtr[g_wUltrasonicSensorNo]++;		// increase the number of triggers.  (for diagnostics)
// wfh Check sensors are replying,   check runs after 50 triggers of each sensor. OK if 40 replies.
// Set and clear alarms.
// g_nUSSensorTriggeredCtr[g_wUltrasonicSensorNo]++;		// increase the number of triggers.  (for diagnostics
////////////////////////////////////////////////////////////////////////////////

#include <basictypes.h>
#include <stdio.h>
#include "WidthPitHandler.h"
#include "WidthControlAlgorithm.h"
#include "WidthVars.h"
#include "Width.h"
#include "WidthDef.h"
#include "Batvars.h"
#include "UltrasonicRead.h"
#include "Alarms.h"

int    	g_nUSSensorTriggeredCtr[NUMBER_OF_ULTRASONICS_SENSORS];		//
int    	g_nUSSensorEchosReceived[NUMBER_OF_ULTRASONICS_SENSORS];
bool	g_bUSSensorOK[NUMBER_OF_ULTRASONICS_SENSORS];
bool    g_bUSSensorError[NUMBER_OF_ULTRASONICS_SENSORS];

int g_nUSSensorTriggeredCtr3 = 0;
int g_nUSSensorEchosReceived3 = 0;
bool g_bUSSensorOK3 = FALSE;
bool g_bUSSensorInAlarm[NUMBER_OF_ULTRASONICS_SENSORS];
void WidthPITHandler( void )
{
}
void WidthFiftyHertzHandler( void )
{
	BOOL	bGoToProcessDelayStage = FALSE;
	int i;
    if(g_nPauseFlag == 0)  //nbb--todo--width--remove testonly
    {
    	// wfh Check sensors are replying,   check runs after 50 triggers of each sensor.
    	for(i=0; i < NUMBER_OF_ULTRASONICS_SENSORS; i++)
    	{	//for each sensor
    		if(g_nUSSensorTriggeredCtr[i] >= 50)	//ie for each 50 times sensor is triggered.
    		{
    			if(i==3)
    			{
    				g_nUSSensorTriggeredCtr3 = g_nUSSensorTriggeredCtr[i];
    				g_nUSSensorEchosReceived3 = g_nUSSensorEchosReceived[i];
    			}

    			g_nUSSensorTriggeredCtr[i] = 0;		// reset trigger counter to 0.
    			if(g_nUSSensorEchosReceived[i] > 40)	// expect a minimum of 40 replies or echos for each 50 triggers.
    			{
    				g_bUSSensorError[i] = FALSE;
    				g_bUSSensorOK[i] = TRUE;
    			}
    			else
    			{
    				g_bUSSensorError[i] = TRUE;
    				g_bUSSensorOK[i] = FALSE;
    			}
    			if(i==3)	g_bUSSensorOK3 = g_bUSSensorOK[i];
    			g_nUSSensorEchosReceived[i]  = 0;		//zero no of echos for next pass.
    		}
// US sensor alarms.
    		int nAlarmCode = US_SENSOR_1_ALARM +i;

    		if( g_bUSSensorError[i] && !g_bUSSensorInAlarm[i])
    		{
    			//flag an an alarm
    			g_bUSSensorInAlarm[i] = TRUE;
    			PutAlarmTable( nAlarmCode, 0 );
    		}
    		else if( g_bUSSensorOK[i] && g_bUSSensorInAlarm[i])
    		{
    			// clear the alarm.
    			g_bUSSensorInAlarm[i] = FALSE;
    			RemoveAlarmTable( nAlarmCode, 0 );
    		}
    	}  //end of sensors check.

    	ReadUltrasonicSensorTime();
    	TriggerUltrasonic();
    	//wfh
    	g_nUSSensorTriggeredCtr[g_wUltrasonicSensorNo]++;		// increase the number of triggers.  (for diagnostics)
    }
	WidthIntegration();
	if(g_nWidthIncreaseOnCtr > 0)
	{
		g_nWidthIncreaseOnCtr--;
		if(g_nWidthIncreaseOnCtr == 0)
		{
			if(g_bWidthAuto)
			{
				bGoToProcessDelayStage = TRUE;
			}
		}
		WidthIncreaseOn();

	}
	else
	{
		WidthIncreaseOff();
	}


	if(g_nWidthFastCorrectionIncreaseOnCtr > 0)
	{
		g_nWidthFastCorrectionIncreaseOnCtr--;
		if(g_nWidthFastCorrectionIncreaseOnCtr == 0)
		{
			if(g_bWidthAuto)
			{
				bGoToProcessDelayStage = TRUE;
			}
		}
		WidthFastCorrectionIncreaseOn();

	}
	else
	{
		WidthFastCorrectionIncreaseOff();
	}


	if(g_nWidthDecreaseOnCtr >0)
	{
		g_nWidthDecreaseOnCtr--;
		if(g_nWidthDecreaseOnCtr == 0)
		{
			if(g_bWidthAuto)
			{
				bGoToProcessDelayStage = TRUE;
			}
		}

		WidthDecreaseOn();
	}
	else
	{
		WidthDecreaseOff();
	}
	if(g_nWidthProcessDelayCtr > 0)
	{
		g_nWidthProcessDelayCtr--;
		if(g_nWidthProcessDelayCtr == 0)
		{
			if(g_bWidthAuto)
			{
				g_wWidthCurrentSequence = WIDTH_INTEGRATION_SEQUENCE;
			}
		}
	}
	if(bGoToProcessDelayStage)
	{
		g_wWidthCurrentSequence = WIDTH_PROCESS_DELAY_SEQUENCE;
		g_nWidthProcessDelayCtr = g_stWidthCalibration.m_wProcessDelayInSeconds * 50;
		g_nWidthProcessDelayCtrCalculatedValue = g_nWidthProcessDelayCtr;
	}
	if(g_bWidthAuto && (g_nWidthIncreaseOnCtr >0 || g_nWidthDecreaseOnCtr >0 || g_nWidthFastCorrectionIncreaseOnCtr >0))
	{
		g_bAutoCorrectionInProgress = TRUE;
	}
	else
	{
		g_bAutoCorrectionInProgress = FALSE;
	}
	MonitorBubbleBreak();
}



