// P.Smith								7/7/10
// if no error or error is less than deadband, then cycle round and do
// not make a correction.
// added CheckStandardAlarmBands
// implement the standard control algorithm,
// added cryovac control algorithm, implement overrun and overrun deadband
//
// P.Smith								7/7/10
// For Cryovac control, if the error exceeds the alarm band do rapid integration
// until the error comes back inside the alarm band.
// SwitchWidthAlarmOnOrOff modified to allow for Cryovac alarm band.
// only shows alarm band if double the alarm limit
// if less or in start up, no alarm is indicated.
//
// P.Smith								8/7/10
// check for auto standardise option before standardise on auto
//
// P.Smith								19/7/10
// correct fast correction alarm limit, subtract the alarm limit from the
// fast correction calculation
// correct standard alarm bands, if fast correction, if g_wCorrectionsOutSideAlarm >
// limit,switch to manual.
// correct cryovac alarm bands, use source outputs rather than component outputs
//
// P.Smith								12/8/10
// allow for fast correction enable/disable.
// if disabled, then use the normal correction even if outside double the alarm
// limit.
//
// P.Smith								21/9/10
// do not allow inc/dec to work in auto.
//
// M.McKiernan					6/5/2020
// //wfh - MMK 6.5.2020
// Added void WidthManualFastIncrease( void );
// added WORD    g_nWidthControlInAutoTimer
// 	if( !g_bWidthIsInStartUp || (g_nWidthControlInAutoTimer > TEN_MINUTES_IN_SECS) )    // not in startup   OR in auto for > 10 mins, can show alarm.
//		bCanShowAlarm = TRUE;
// Note; only used for standard alarm, not cryovac.  TODO check
//*****************************************************************************
// INCLUDES
//*****************************************************************************
#include <stdio.h>
#include <Math.h>
#include "WidthVars.h"
#include "WidthMB.h"
#include "BatVars.h"
#include "WidthControlAlgorithm.h"
#include "InitialiseHardware.h"
#include "SetpointFormat.h"
#include "WidthDef.h"
#include "WidthDiagnostics.h"
#include "WidthForeground.h"
#include "Alarms.h"

extern structSetpointData   g_CurrentRecipe;
extern WORD    g_nWidthControlInAutoTimer;

const float g_fOverRunLookUp[6] = {0,25,50,75,100,125};  // nbb--todo--width check for max value

#define TEN_MINUTES_IN_SECS    (600)

/* ====================================================================
Function:     WidthControlAlgorithm
Parameters:   N/A
Returns:      None

This function runs the width control algorithm
======================================================================= */

void WidthControlAlgorithm( void )
{
	// calculate the average width error
	g_fWidthControlAverageError = g_fWidthControlAverage -  g_CurrentRecipe.m_fDesiredWidth;
	if(g_stWidthCalibration.m_wLineType == BLOWN_FILM_LINE_TYPE)
	{
		StandardControlAlgorithm();
	}
	else
	if(g_stWidthCalibration.m_wLineType == IBC_LINE_TYPE)
	{
	    //nbb--todo--ibc control algorithm
	}
	else
	if(g_stWidthCalibration.m_wLineType == CRYOVAC_LINE_TYPE)
	{
		CryovacControlAlgorithm();
	}

	GenerateControlDiagnostics();
}



/* ====================================================================
Function:     StandardControlAlgorithm
Parameters:   N/A
Returns:      None

This function activates the standard control algorithm
======================================================================= */
void StandardControlAlgorithm( void )
{
	float fSetPlusActual,fSetMinusActual,fTime,fControlGain;
	WORD wPositiveError,wFastCorrectionAlarmLimit;

	// calculate correction
			// (set width + actual width)(set width - actual width) * control gain * Thetha (597134)
	float fPositiveError;
	fPositiveError = fabs (g_fWidthControlAverageError);
	wPositiveError = (WORD)(fPositiveError + 0.5);  // bring to nearest mm

	if(g_bWidthAuto)  // in auto  make the correction
	{
		CheckStandardAlarmBands(wPositiveError);
		if((wPositiveError == 0) || (wPositiveError <= g_stWidthCalibration.m_wWidthDeadband))  // no correction
		{  // no correction error = 0 or less than deadband
			ResetWidthIntegration();

		}
		else
		{
			fSetPlusActual = (g_CurrentRecipe.m_fDesiredWidth + g_fWidthControlAverage);
			fSetMinusActual = (g_CurrentRecipe.m_fDesiredWidth - g_fWidthControlAverage);
			if(g_stWidthCalibration.m_wAlarmLimit < MINIMUM_FAST_CORRECTION_LIMIT)
			{
				wFastCorrectionAlarmLimit = MINIMUM_FAST_CORRECTION_LIMIT;
			}
			else
			{
				wFastCorrectionAlarmLimit = g_stWidthCalibration.m_wAlarmLimit;
			}

			if(g_fWidthControlAverageError < 0)  // must increase width
			{
				fControlGain = g_stWidthCalibration.m_fNormalIncreaseControlGainPercentage;
			}
			else
			{
				fControlGain = g_stWidthCalibration.m_fNormalDecreaseControlGainPercentage;
			}
			if((wPositiveError <= (2 * wFastCorrectionAlarmLimit))||!g_stWidthCalibration.m_bFastCorrectionEnabled)
			{
			    fTime = fSetPlusActual * fSetMinusActual * (THETHA_FOR_WIDTH_CONTROL) * fControlGain;
			}
			else   // fast correction
			{
				fTime = fSetPlusActual * (fSetMinusActual - g_stWidthCalibration.m_wAlarmLimit) * (THETHA_FOR_WIDTH_CONTROL) * g_stWidthCalibration.m_fFastCorrectedControlGainPercentage;
			}
			if(fTime < 0)
			fTime = -fTime;
			if(g_fWidthControlAverageError > 0)  // error is plus, need to reduce size
			{
				g_nWidthDecreaseOnCtr = (int)fTime;
				g_nWidthDecreaseOnCtrCalculatedValue = g_nWidthDecreaseOnCtr;
			}
			else
			{
				if((wPositiveError <= (2 * wFastCorrectionAlarmLimit))||(!g_stWidthCalibration.m_bFastCorrectionEnabled))
				{
					g_nWidthIncreaseOnCtr = (int)fTime;
					g_nWidthIncreaseOnCtrCalculatedValue = g_nWidthIncreaseOnCtr;
				}
				else
				{
					g_nWidthFastCorrectionIncreaseOnCtr = (int)fTime;
					g_nWidthFCIncreaseOnCtrCalculatedValue = g_nWidthFastCorrectionIncreaseOnCtr;
				}
			}
			g_wWidthCurrentSequence = WIDTH_CORRECTING_SEQUENCE;
		}
	}
}


/* ====================================================================
Function:     CheckStandardAlarmBands
Parameters:   N/A
Returns:      None

======================================================================= */
void CheckStandardAlarmBands( WORD wPositiveError)
{
  if(wPositiveError <= (g_stWidthCalibration.m_wAlarmLimit))
  {
	  g_wWidthControlAlarm = NO_WIDTH_ALARM;
	  g_wCorrectionsOutSideAlarm = 0;
	  g_bWidthIsInStartUp = FALSE;
	  g_bRapidIntegration = FALSE; // remove rapid integration
	  g_nControlDirectionOutsideAlarmBand = CONTROL_IS_OFF;
  }
  else
  {
	  if(g_fWidthControlAverageError <0)
	  {
		  g_nControlDirectionOutsideAlarmBand = CONTROL_IS_INCREASING;
	  }
	  else
	  {
		  g_nControlDirectionOutsideAlarmBand = CONTROL_IS_DECREASING;
	  }
	  if(wPositiveError < (2* g_stWidthCalibration.m_wAlarmLimit))
	  {
		  g_wWidthControlAlarm = WIDTH_ERROR_OUTSIDE_SINGLE_ALARM_BAND;
		  if(!g_bWidthIsInStartUp)  // not in start up
		  {
			  g_wCorrectionsOutSideAlarm++;
			  if(g_wCorrectionsOutSideAlarm > (MAX_CORRECTION_IN_ALARM))
			  {
				  WidthToManual();
			  //	ADDWidthControlToManualEventToLog //nbb--todo--width generate event for this
			  }
		  }
	  }
	  else  // greater than double alarm limit
	  {
		  g_wWidthControlAlarm = WIDTH_ERROR_OUTSIDE_DOUBLE_ALARM_BAND;
		  if(!g_bWidthIsInStartUp)  // not in start up
		  {
			  g_wCorrectionsOutSideAlarm++;
			  if(g_wCorrectionsOutSideAlarm > (MAX_CORRECTION_IN_ALARM))
			  {
				  WidthToManual();
				  //	ADDWidthControlToManualEventToLog //nbb--todo--width generate event for this
			  }
		  }
	  }
  }
}



/* ====================================================================
Function:     CryovacControlAlgorithm
Parameters:   N/A
Returns:      None

This function initiates teh cryovac control algorithm
counts = 500 * Control Gain * Error / Measured Width
======================================================================= */
void CryovacControlAlgorithm( void )
{
	float fCorrection,fabsCorrection,fControlGain,fPositiveError;
	WORD	wPositiveError;
	fPositiveError = fabs (g_fWidthControlAverageError);
	wPositiveError = (WORD)(fPositiveError + 0.5);  // bring to nearest mm
	CheckCryovacAlarmBands(wPositiveError);
	if(g_bWidthAuto && !g_bBubbleBreakActive)  // in auto  make the correction
	{
		if((wPositiveError == 0) || (wPositiveError <= g_stWidthCalibration.m_wWidthDeadband))  // no correction
		{  // no correction error = 0 or less than deadband
			ResetWidthIntegration();
		}
		else
		{
			if(g_fWidthControlAverageError < 0)  // must increase width
			{
				fControlGain = g_stWidthCalibration.m_fNormalIncreaseControlGainPercentage;
			}
			else
			{
				fControlGain = g_stWidthCalibration.m_fNormalDecreaseControlGainPercentage;
			}
			fCorrection = 500.0 * fControlGain * g_fWidthControlAverageError / g_fWidthControlAverage;
			fabsCorrection = fabs(fCorrection);



		// now decide on the direction of the correction

			if(g_fWidthControlAverageError < 0)  // must increase width
			{ // move frame down.
			// check if overrun

				if(g_stWidthCalibration.m_wWidthOverrun > 0)
				{
					if(g_stWidthCalibration.m_wWidthOverrunDeadband > 0)
					{  // check for deadband
						if((fabsCorrection - g_fOverRunLookUp[g_stWidthCalibration.m_wWidthOverrunDeadband]) > 0)
						{
							g_nWidthIncreaseOnCtr = fabsCorrection - g_fOverRunLookUp[g_stWidthCalibration.m_wWidthOverrun];
							g_nWidthIncreaseOnCtrCalculatedValue = g_nWidthIncreaseOnCtr;
						}
						else
						{
							g_nWidthIncreaseOnCtr =  fabsCorrection;
							g_nWidthIncreaseOnCtrCalculatedValue = g_nWidthIncreaseOnCtr;
						}
					}
					else
					{
						if(fabsCorrection - g_fOverRunLookUp[g_stWidthCalibration.m_wWidthOverrun] > 0)
						g_nWidthIncreaseOnCtr = fabsCorrection - g_fOverRunLookUp[g_stWidthCalibration.m_wWidthOverrun];
						else
					    g_nWidthIncreaseOnCtr =  fabsCorrection;

						g_nWidthIncreaseOnCtrCalculatedValue = g_nWidthIncreaseOnCtr;
					}
				}
				else
				{
					g_nWidthIncreaseOnCtr =  fabsCorrection;
					g_nWidthIncreaseOnCtrCalculatedValue = g_nWidthIncreaseOnCtr;
				}
			}
			else
			{
				g_nWidthDecreaseOnCtr = (int)fabs(fCorrection);
				g_nWidthDecreaseOnCtrCalculatedValue = g_nWidthDecreaseOnCtr;
			}
			g_wWidthCurrentSequence = WIDTH_CORRECTING_SEQUENCE;
		}
	}
}



/* ====================================================================
Function:     CheckCryovacAlarmBands
Parameters:   N/A
Returns:      None

======================================================================= */
void CheckCryovacAlarmBands( WORD wPositiveError)
{
   if(wPositiveError <= (g_stWidthCalibration.m_wAlarmLimit))
  {
	  g_wWidthControlAlarm = NO_WIDTH_ALARM;
	  g_wCorrectionsOutSideAlarm = 0;
	  g_bWidthIsInStartUp = FALSE;
	  g_bRapidIntegration = FALSE; // remove rapid integration
	  g_nControlDirectionOutsideAlarmBand = CONTROL_IS_OFF;
  }
  else
  {  // greater than alarm limit.
	  if(g_bWidthAuto)
	  {
	      g_bRapidIntegration = TRUE;
	  }
	  if(g_fWidthControlAverageError <0)
	  {
		  g_nControlDirectionOutsideAlarmBand = CONTROL_IS_INCREASING;
	  }
	  else
	  {
		  g_nControlDirectionOutsideAlarmBand = CONTROL_IS_DECREASING;
	  }
	  if(wPositiveError <= (2 * g_stWidthCalibration.m_wAlarmLimit))
	  {
		  g_wWidthControlAlarm = WIDTH_ERROR_OUTSIDE_SINGLE_ALARM_BAND;
		  if(!g_bWidthIsInStartUp)  // not in start up
		  {
		  }
	  }
	  else  // greater than double alarm limit
	  {
		  g_wWidthControlAlarm = WIDTH_ERROR_OUTSIDE_DOUBLE_ALARM_BAND;
	  }
  }
}


/* ====================================================================
Function:     WidthAutoManualToggle
Parameters:   N/A
Returns:      None

This function toggle width auto manual status
======================================================================= */
void WidthAutoManualToggle( void )

{
	if(g_bWidthAuto)
    {
    	WidthToManual();     // switch to manual
		//nbb--todo--width g_bWidthToAutoEvent = TRUE;
		//nbb--todo--width g_nSDCardDelayTime = SDCARDCOMMANDDELAYTIME;

    }
    else
    {
		WidthToAuto();       // switch to auto.
        //nbb--todo--width g_bWidthToManualEvent = TRUE;
    	//nbb--todo--width g_nSDCardDelayTime = SDCARDCOMMANDDELAYTIME;
    }
}

/* ====================================================================
Function:     WidthToManual
Parameters:   N/A
Returns:      None

This function switches width control to manual
======================================================================= */
void WidthToManual( void )
{
	g_bWidthAuto = FALSE;
	g_bWidthIsInStartUp = FALSE;  // clear start up mode
	SwitchOffAllControlOutputs();
	ResetWidthIntegration();
}

/* ====================================================================
Function:     WidthToAuto
Parameters:   N/A
Returns:      None

This function switches width control to Auto
======================================================================= */
void WidthToAuto( void )
{
	if(g_stWidthCalibration.m_bStandardiseOnAuto)
	{
		InitiateStandardise();	// initiate standardise
	}
	g_bWidthAuto = TRUE;
	SetStartUpMode();
}


/* ====================================================================
Function:     SetStartUpMode
Parameters:   N/A
Returns:      None

This function sets start up mode
======================================================================= */
void SetStartUpMode( void )
{
	g_bWidthIsInStartUp = TRUE;  // set start up mode
	g_bRapidIntegration = TRUE;
	SwitchOffAllControlOutputs();
	ResetWidthIntegration();
}
/* ====================================================================
Function:     SetRapidIntegration
Parameters:   N/A
Returns:      None

This function sets start up mode
======================================================================= */
void SetRapidIntegration( void )
{
	g_bRapidIntegration = TRUE;
	SwitchOffAllControlOutputs();
	ResetWidthIntegration();
}



/* ====================================================================
Function:     WidthIncreaseOn
Parameters:   N/A
Returns:      None

This function switches increase on
======================================================================= */
void WidthIncreaseOn( void )
{
	g_bWidthIncreaseIsOn = TRUE;
	SRCOn(1);
}



/* ====================================================================
Function:     WidthIncreaseOff
Parameters:   N/A
Returns:      None

This function switches increase off
======================================================================= */
void WidthIncreaseOff( void )
{
	g_bWidthIncreaseIsOn = FALSE;
	SRCOff(1);
}

/* ====================================================================
Function:     WidthIncreaseOn
Parameters:   N/A
Returns:      None

This function switches increase on
======================================================================= */
void WidthFastCorrectionIncreaseOn( void )
{
	g_bWidthFastCorrectionIncreaseIsOn = TRUE;
	SRCOn(3);
}

/* ====================================================================
Function:     WidthIncreaseOff
Parameters:   N/A
Returns:      None

This function switches increase off
======================================================================= */
void WidthFastCorrectionIncreaseOff( void )
{
	g_bWidthFastCorrectionIncreaseIsOn = FALSE;
	SRCOff(3);
}


/* ====================================================================
Function:     WidthDecreaseOn
Parameters:   N/A
Returns:      None

This function switches decrease on
======================================================================= */
void WidthDecreaseOn( void )
{
	g_bWidthDecreaseIsOn = TRUE;
	SRCOn(2);
}


/* ====================================================================
Function:     WidthDecreaseOff
Parameters:   N/A
Returns:      None

This function switches decrease off
======================================================================= */
void WidthDecreaseOff( void )
{
	g_bWidthDecreaseIsOn = FALSE;
	SRCOff(2);
}


/* ====================================================================
Function:     SwitchOffAllControlOutputs
Parameters:   N/A
Returns:      None

This function switches off all control outputs
======================================================================= */
void SwitchOffAllControlOutputs( void )
{
	g_nWidthIncreaseOnCtr = 0; // increase off
	g_nWidthDecreaseOnCtr = 0; // decrease off
	g_nWidthFastCorrectionIncreaseOnCtr = 0; // fc off
}


/* ====================================================================
Function:     ResetWidthIntegration
Parameters:   N/A
Returns:      None

This function reset width integration
======================================================================= */
void ResetWidthIntegration( void )
{
	g_wWidthIntegrationCtr = 0;
	g_fWidthIntegrationSummation = 0.0f;
	g_wWidthCurrentSequence = WIDTH_INTEGRATION_SEQUENCE;
}



/* ====================================================================
Function:     WidthManualIncrease
Parameters:   N/A
Returns:      None

Manual increase
======================================================================= */
void WidthManualIncrease( void )
{
	if(!g_bWidthAuto)  // in manual
	{
	    g_nWidthIncreaseOnCtr = 25;
	}
}

//wfh 6.5.2020

/* ====================================================================
Function:     WidthManualFastIncrease
Parameters:   N/A
Returns:      None

Manual increase
======================================================================= */
void WidthManualFastIncrease( void )
{
	if(!g_bWidthAuto)  // in manual
	{
		g_nWidthFastCorrectionIncreaseOnCtr = 25;		// decremented on 50Hz,  so 0.5s.
	}
}

/* ====================================================================
Function:     WidthManualDecrease
Parameters:   N/A
Returns:      None

Manual decrease
======================================================================= */
void WidthManualDecrease( void )
{
	if(!g_bWidthAuto)  // in manual
	{
		g_nWidthDecreaseOnCtr = 25;
	}
}


/* ====================================================================
Function:     SwitchWidthAlarmOnOrOff
Parameters:   N/A
Returns:      None

Switches alarm output on or off
======================================================================= */
void SwitchWidthAlarmOnOrOff( void )
{
	BOOL bAlarmOn = FALSE;
	BOOL bIndicateAlarm = FALSE;
	BOOL bCantShowAlarm = FALSE;

	if( g_bWidthIsInStartUp && (g_nWidthControlInAutoTimer < TEN_MINUTES_IN_SECS) )    // not in startup   OR in auto for > 10 mins, can show alarm.
		bCantShowAlarm = TRUE;

    if(g_stWidthCalibration.m_wLineType == CRYOVAC_LINE_TYPE)
    {
    	if((g_wWidthControlAlarm == WIDTH_ERROR_OUTSIDE_DOUBLE_ALARM_BAND) && !g_bWidthIsInStartUp)
    	{
    		bAlarmOn = TRUE;
    	}
    	else
    	{
    		bAlarmOn = FALSE;
        }
    }
    else
    {  // assume standard operation
//    	if((g_wWidthControlAlarm == 0) || (g_bWidthIsInStartUp))
//    	if((g_wWidthControlAlarm == 0) || !bCanShowAlarm )
//                                          in startup and InAutoTimer < 10mins
    	if( (g_wWidthControlAlarm == 0) || ( g_bWidthIsInStartUp && (g_nWidthControlInAutoTimer < TEN_MINUTES_IN_SECS) )  )
    	{
    		bAlarmOn = FALSE;
    	}
     	else
    	{
    		bAlarmOn = TRUE;
    	}

    }
    // show alarm in auto or when alarm in manual is enabled.

	if(g_bWidthAuto)  // in auto show the alarm
	{
		bIndicateAlarm = TRUE;
	}
	else
	if(g_stWidthCalibration.m_bAlarmInManual)
	{
		bIndicateAlarm = TRUE;
	}
	else
	{
		bIndicateAlarm = FALSE;
	}
    if(bIndicateAlarm)
    {
    	if(bAlarmOn)
    	{
    		if(!g_bWidthAlarmIsActive) // only record on transition.
            {
    			PutAlarmTable( WIDTH_CONTROL_ALARM,  0 );
            }
    		g_bWidthAlarmIsActive = TRUE;
    		WidthAlarmOn();
    	}
    	else
    	{
    		if(g_bWidthAlarmIsActive)
    		{
                RemoveAlarmTable( WIDTH_CONTROL_ALARM,  0 );
    		}
            WidthAlarmOff();
    		g_bWidthAlarmIsActive = FALSE;
    	}
    }
    else
    {
   		WidthAlarmOff();
    	g_bWidthAlarmIsActive = FALSE;
    }
}


/* ====================================================================
Function:     WidthAlarmOn
Parameters:   N/A
Returns:      None

Switches width alarm output on
======================================================================= */
void WidthAlarmOn( void )
{
	SRCOn(4);
}


/* ====================================================================
Function:     WidthAlarmOff
Parameters:   N/A
Returns:      None

Switches width alarm output off
======================================================================= */
void WidthAlarmOff( void )
{
	SRCOff(4);
}

