///////////////////////////////////////////////////////////
//
// P.Smith								7/7/10
// in CheckForChangeInErrorDirection if fast correcting and error comes inside
// the alarm band, then switch off and do a rapid integration again.
// CheckForNewWidthSetPoint checks for change in width set point and
// sets the width into start up mode.
// implement MonitorBubbleBreak to monitor bubble break input
// if active switch off control outputs and standardise
// when inactive set rapid integration when bubble break goes away.
//
// P.Smith								8/7/10
// Added check for cryovac alarm period. If in alarm for 10 minutes, then the
// system will switch to manual.
//
// P.Smith								19/7/10
// correct the fast correction transition from the fast correction alarm limit
// to the standard alarm limit.

// M.McKiernan							23/4/2020
// void WidthCalculation3Points( void )
// float CalculateRadiusFrom3Points( float Xa, float Ya, float Xb, float Yb, float Xc, float Yc )
////wfh
// void CalculateBubbleCoordinates( void );
// void CalculateSensorCoordinates( void );
// void CopyCalculatedSensorAnglesToMB( void );
//float  CalculateTriangleAngleA( float fAdj1, float fAdj2, float fOpp);
//void CalculateSensorAngles( float fRadius );
//////////////////////////////////////////////////////////

//*****************************************************************************
// INCLUDES
//*****************************************************************************
#include <stdio.h>
#include <math.h>
#include "WidthVars.h"
#include "WidthForeground.h"
#include "WidthDef.h"
#include "BatchCalibrationFunctions.h"
#include "Batvars.h"
#include "WidthRWToNonVolatileMemory.h"
#include "WidthControlAlgorithm.h"
#include "Width.h"
#include "Debug.h"
#include "SetpointFormat.h"
#include "InitialiseHardware.h"
#include <math.h>
#include "batvars2.h"
#include "WidthMB.h"

extern structSetpointData   g_CurrentRecipe;
extern  StructDebug   g_nTSMDebug;
// Angle A. between AB and aC
float g_fAngleA;
// sensor coordinates.
float g_fSAx;
float g_fSAy;
float g_fSBx;
float g_fSBy;
float g_fSCx;
float g_fSCy;
//coordinates on bubble.  sensor to bubble intersections.
float g_fB1x;		// where sensor A (1) touches bubble.
float g_fB1y;
float g_fB2x;		// where sensor B (2) touches bubble.
float g_fB2y;
float g_fB3x;		// where sensor C (3) touches bubble.
float g_fB3y;

//calculated sensor angles.   Initialise to what would be expected.
float g_fSensorAAngle = 30.0f;	// Sensor A angle.
float g_fSensorBAngle = 270.0f;	// Sensor B angle.
float g_fSensorCAngle = 150.0f;	// Sensor C angle.

/* ====================================================================
Function:     WidthIntegration - called on 50hz generates the width integration
Parameters:   N/A
Returns:      N/A

======================================================================= */

void WidthIntegration( void )
{
	unsigned int	nSampleTime;
	// decide if the integration should be done
	if(g_bIsStandardising)  // no integration during standardise
	{
		InitialisationWhileStandardising();
	}
	else
	if((g_wWidthCurrentSequence == WIDTH_INTEGRATION_SEQUENCE)&& !g_bIsStandardising)
	{
		if(g_wStandardiseSettlePeriod > 0)
		{
			ResetWidthIntegration();
		}
		else
		{
			if(g_bRapidIntegration)
			{
				nSampleTime = (RAPID_INTEGRATION_SAMPLE_TIME * 50)/ CONTROL_AVERAGE_SAMPLES;
			}
			else
			{
				nSampleTime = (g_stWidthCalibration.m_wWidthIntegrationTimeinSeconds * 50)/ CONTROL_AVERAGE_SAMPLES;
			}

			g_wSampleIntegationIntervalin50hz = nSampleTime;

			g_wWidthIntegrationSampleCtr++;
			if(g_wWidthIntegrationSampleCtr >= nSampleTime)  // take sampe
			{
				g_wWidthIntegrationSampleCtr = 0;
				g_wWidthIntegrationCtr++;
				g_fWidthIntegrationSummation += g_fAveragedWidth;
				g_fIntegratedWidth = g_fWidthIntegrationSummation / (CONTROL_AVERAGE_SAMPLES);
				if(g_wWidthIntegrationCtr >= CONTROL_AVERAGE_SAMPLES)
				{
					g_wWidthIntegrationCtr = 0;
					g_fWidthControlAverage = g_fWidthIntegrationSummation / (CONTROL_AVERAGE_SAMPLES);
					g_fWidthIntegrationSummation = 0.0f;
//				if(fdTelnet>0)
//					printf("\n control average is !!!!!!!!!!!!!!!!!!!!! %f",g_fWidthControlAverage);
				// calculate control average
					WidthControlAlgorithm(); // initiate control action.
				}
				else
				{
//			if(fdTelnet>0)
//		    printf("\n summation is %f %d",g_fWidthIntegrationSummation,g_wWidthIntegrationCtr);
				}
			}
		}
	}
}



/* ====================================================================
Function:     InitialisationWhileStandardising -
Parameters:   N/A
Returns:      N/A

======================================================================= */

void InitialisationWhileStandardising( void )
{
	SwitchOffAllControlOutputs();
	ResetWidthIntegration();

}


/* ====================================================================
Function:     CheckForChangeInErrorDirection -
              if control direction is outside alarm band and the direction of the error
              crosses over zero, then a rapid integration is initiated.
Parameters:   N/A
Returns:      N/A

======================================================================= */

void CheckForChangeInErrorDirection( void )
{
	float fWidthError,fPositiveWidthError;
	// only do check if correcting outside alarm band

	if(g_bAutoCorrectionInProgress)
	{
		fWidthError = g_fAveragedWidth -  g_CurrentRecipe.m_fDesiredWidth;
		fPositiveWidthError = fabs(fWidthError);
		if((g_nControlDirectionOutsideAlarmBand == CONTROL_IS_INCREASING) && (fWidthError >0))
		{
			if((fdTelnet>0) && g_st_WidthDebug.m_bWidthControlDebug)
			iprintf("\n error has gone negative");
			SwitchOffAllControlOutputs();
			ResetWidthIntegration();
		}
		else
		if((g_nControlDirectionOutsideAlarmBand == CONTROL_IS_DECREASING) && (fWidthError <0))
		{
			if((fdTelnet>0) && g_st_WidthDebug.m_bWidthControlDebug)
			iprintf("\n error has gone positive");
			SwitchOffAllControlOutputs();
			ResetWidthIntegration();
		}
		else
		if((g_nWidthFastCorrectionIncreaseOnCtr > 0)&&(fPositiveWidthError <= g_stWidthCalibration.m_wAlarmLimit))
		{
			SwitchOffAllControlOutputs();
			ResetWidthIntegration();
			if((fdTelnet>0) && g_st_WidthDebug.m_bWidthControlDebug)
			printf("\n fast correction error within normal band %f alarm limit %d",fWidthError,g_stWidthCalibration.m_wAlarmLimit);
		}
	}
}



/* ====================================================================
Function:     CheckForNewWidthSetPoint -
Parameters:   N/A
Returns:      N/A

======================================================================= */

void CheckForNewWidthSetPoint( void )
{
	if(g_bWidthAuto)  // in auto
	{
		if(g_CurrentRecipe.m_fDesiredWidth != g_fDesiredWidthHistory)
		{
			g_fDesiredWidthHistory = g_CurrentRecipe.m_fDesiredWidth;
			SetStartUpMode(); // change of sp detected
			if((fdTelnet>0) && g_st_WidthDebug.m_bWidthControlDebug)
			iprintf("change of sp detected");
		}
	}
	else
	{
		g_fDesiredWidthHistory = g_CurrentRecipe.m_fDesiredWidth;
	}

}

/* ====================================================================
Function:     MonitorBubbleBreak -
Parameters:   N/A
Returns:      N/A

======================================================================= */

void MonitorBubbleBreak( void )
{
	BOOL	bState;
	bState = ReadLSensor();                     // raw signal state
	if(bState == ON)
	{
		g_wBubbleBreakInputInActiveCtr = 0;
		g_wBubbleBreakInputActiveCtr++;
		if(g_wBubbleBreakInputActiveCtr >= BUBBLE_BREAK_ACTIVE_TIME_PERIOD)
		{
			g_wBubbleBreakInputActiveCtr = 0;
			if(!g_bBubbleBreakActive)
			{
				g_bBubbleBreakActiveTransition = TRUE;
				InitiateStandardise();	// initiate standardise
				SwitchOffAllControlOutputs();
			}
			g_bBubbleBreakActive = TRUE;
		}
	}
	else
	{
		g_wBubbleBreakInputActiveCtr = 0;
		g_wBubbleBreakInputInActiveCtr++;
		if(g_wBubbleBreakInputInActiveCtr >= BUBBLE_BREAK_ACTIVE_TIME_PERIOD)
		{
			g_wBubbleBreakInputInActiveCtr = 0;
			if(g_bBubbleBreakActive)
			{
				g_bBubbleBreakInActiveTransition = TRUE;
				SetRapidIntegration();
			 }
			 g_bBubbleBreakActive = FALSE;
		 }
	}
}




/* ====================================================================
Function:     CheckForCryovacAlarmPeriod -
if alarm has been on for n minutes then the width control is switched to manual
Parameters:   N/A
Returns:      N/A

======================================================================= */

void CheckForCryovacAlarmPeriod( void )
{
	if(g_stWidthCalibration.m_wLineType == CRYOVAC_LINE_TYPE)
	{
		if(g_bWidthAlarmIsActive)
		{
		    g_wAlarmOnSecondsCounter++;
		    if(g_wAlarmOnSecondsCounter >= CRYOVAC_TO_MANUAL_ALARM_TIMEOUT)
		    {
		    	g_wAlarmOnSecondsCounter = 0;
		    	WidthToManual();
		    }
		}
		else
		{
			g_wAlarmOnSecondsCounter = 0;
		}
	}
}

/* ====================================================================
Function:     WidthCalculation3Points
Calculate the radius of the bubble from 3 points on circumference.
Parameters:   N/A
Returns:      N/A
EQuations used:  r = (a * b * c) / (4*Area ABC)		where a = lenght AB,  b = length BC, c = length CA.
				Area ABC = sqrtf(s(s-a)(s-b)(s-c))   where s = (a+b+c)/2
				Point coordinates: A = (Xa, Ya),  B = (Xb, Yb),  C = (Xc, Yc).
======================================================================= */

void WidthCalculation3Points( void )
{
// Starting work.
	float a,b,c;
	float Xa, Xb, Xc, Ya, Yb, Yc;
	float r,s;
	float fArea,fRadius;

	// Test values
	/*
	//A = 0,2
	Xa = 0;
	Ya = 2;
	//B = 2,4
	Xb = 2;
	Yb = 4;
	//C = 2,0
	Xc = 2;
	Yc = 0;
	*/
//	147.16, 385.06
//	147.16, 294.68
//	240.00, 318.34

//	451.10	310.44
//	391.10	250.44
//	494.58	209.09
	//A
	Xa = 451.10;
	Ya = 310.44;
	//B =
	Xb = 391.10;
	Yb = 250.44;
	//C =
	Xc = 494.58;
	Yc = 209.09;
//.


//calculate lengths.
	a = sqrtf( (Xa-Xb)*(Xa-Xb) + (Ya-Yb)*(Ya-Yb) );
	b = sqrtf( (Xb-Xc)*(Xb-Xc) + (Yb-Yc)*(Yb-Yc) );
	c = sqrtf( (Xc-Xa)*(Xc-Xa) + (Yc-Ya)*(Yc-Ya) );

	//s
	s = (a+b+c)/2.0f;

	//Area
	fArea = sqrtf( s*(s-a)*(s-b)*(s-c) );

	r = (a * b * c)/(4.0f*fArea);

	fRadius = r;
	if( (fdTelnet>0) && g_nTSMDebug.m_bUltrasonicsDebug )	printf("\nWidthCalculation3Points: r = %5.2f Area= %5.2f", fRadius, fArea);

}


//16.3.2020
/* ====================================================================
Function:     CalculateRadiusFrom3Points
Calculate the radius of the bubble from 3 points on circumference.
Parameters:   N/A
Returns:      N/A
EQuations used:  r = (a * b * c) / (4*Area ABC)		where a = lenght AB,  b = length BC, c = length CA.
				Area ABC = sqrtf(s(s-a)(s-b)(s-c))   where s = (a+b+c)/2
				Point coordinates: A = (Xa, Ya),  B = (Xb, Yb),  C = (Xc, Yc).
======================================================================= */

float CalculateRadiusFrom3Points( float Xa, float Ya, float Xb, float Yb, float Xc, float Yc )
{
// Starting work.
	float a,b,c;
//	float Xa, Xb, Xc, Ya, Yb, Yc;
	float r,s;
	float fArea,fRadius;

	// Test values
	/*
	//A = 0,2
	Xa = 0;
	Ya = 2;
	//B = 2,4
	Xb = 2;
	Yb = 4;
	//C = 2,0
	Xc = 2;
	Yc = 0;
	*/
//	147.16, 385.06
//	147.16, 294.68
//	240.00, 318.34

//	451.10	310.44
//	391.10	250.44
//	494.58	209.09
	//A
	/*
	Xa = 451.10;
	Ya = 310.44;
	//B =
	Xb = 391.10;
	Yb = 250.44;
	//C =
	Xc = 494.58;
	Yc = 209.09;
	*/
//.


//calculate lengths.
	a = sqrtf( (Xa-Xb)*(Xa-Xb) + (Ya-Yb)*(Ya-Yb) );
	b = sqrtf( (Xb-Xc)*(Xb-Xc) + (Yb-Yc)*(Yb-Yc) );
	c = sqrtf( (Xc-Xa)*(Xc-Xa) + (Yc-Ya)*(Yc-Ya) );

	//s
	s = (a+b+c)/2.0f;

	//Area
	fArea = sqrtf( s*(s-a)*(s-b)*(s-c) );

	r = (a * b * c)/(4.0f*fArea);

	fRadius = r;
	if( (fdTelnet>0) && g_nTSMDebug.m_bUltrasonicsDebug )
		printf("\nCalculateRadiusFrom3Points: r = %5.2f Area= %5.2f", fRadius, fArea);

	return(fRadius);

}


/*    Sensor coordinates
Coordinates of Sensors: (assuming  sensor 1 is at origin, 0,0 and  1-2 is the X axis).

Snsr1 (A)  = (0, 0)
Snsr 2 (B)  = (aCos(A),  aSin(A) )
Snsr 3 (C ) = (c, 0).

.a = BC  side opposite A
.b = AC   side opposite B
.c = AB   side opposite C

Coordinates of intercepts on bubble:
B1 = (d1*Cos(T), d1*Sin(T) )
B2 (case 1) = (a*Cos(B) – d2*Sin(U), a*Sin(B)-d2*Cos(U) ).
 B2 (case 2) = (a*Cos(B) – d2*Sin(U), a*Sin(B)+d2*Cos(V) ).
B3 = ( c - d3*Cos(S),  d3*Sin(S) ).

Angle A
	Cos(A) = b2 + c2 –a2 / 2*b*c

	Cos(A) = (AC2+AB2 – BC2 )  /  (2 BC * AC)

	Cos(A) =  (AC*AC + AB *AB  – BC*BC )  /  (2 AB * AC)
	A =  Cos-1(AC*AC + AB *AB  – BC*BC )  /  (2 AB * AC)
	A =  acos (AC*AC + AB *AB  – BC*BC )  /  (2 AB * AC)
	Assume A = 60.
                                        B
                                       / \
                                  AB  /   \ BC
                                   c /     \a
                                    /       \
                                   /         \
                            A(0,0)/___________\C
                                       b  AC
 */
// sensor coordinates.
extern	float g_fLAB;
extern	float g_fLBC;
extern	float g_fLAC;
void CalculateSensorCoordinates( void )
{
	float fBC = g_stWidthCalibration.m_fLength_BC;
	float fAC = g_stWidthCalibration.m_fLength_AC;
	float fAB = g_stWidthCalibration.m_fLength_AB;
	float fDegRad = PI/180;		// convert degrees to Radians multiplier.
	float fRadDeg =  180.0f/PI;  // convert degrees to Radians multiplier.

	float fAngleA;				// angle at sensor A - sensor to sensor angle,  AB to AC;
	float fTemp;

//	Cos(A) = (b*b + c*c –a*a) / 2*a*b;
//	A =  acos (AC*AC + AB *AB  – BC*BC )  /  (2 AB * AC)
//	A = cos(fAB);

	fTemp = ( (fAC*fAC + fAB*fAB) - fBC*fBC ) / (2*fAB*fAC);
	fAngleA =  acos(fTemp) * fRadDeg;

	if( (fdTelnet>0) && g_nTSMDebug.m_bUltrasonicsDebug ) printf("\n Angle A = %5.2f ", fAngleA);
	if( (fdTelnet>0) && g_nTSMDebug.m_bUltrasonicsDebug ) printf("\n\n AB= %5.1f BC= %5.1f AC= %5.1f  gAB= %5.2f \n\n", fAB, fBC, fAC, g_stWidthCalibration.m_fLength_AB);
	if( (fdTelnet>0) && g_nTSMDebug.m_bUltrasonicsDebug ) printf("\n AngleA= %5.1f AngleB= %5.1f AngleC= %5.1f",
			g_stWidthCalibration.m_fSensorAngleA, g_stWidthCalibration.m_fSensorAngleB, g_stWidthCalibration.m_fSensorAngleC);
	if( (fdTelnet>0) && g_nTSMDebug.m_bUltrasonicsDebug ) printf("\n:LAB = %5.1fLBC = %5.1f LAC = %5.1f ", g_fLAB, g_fLBC, g_fLAC);
	if( (fdTelnet>0) && g_nTSMDebug.m_bUltrasonicsDebug ) printf("\ng_fWidthConversionFactor= %5.2f", g_fWidthConversionFactor );

// hardcode for now.

	fAngleA = 60.0f;			// if sensors equi-spaced, should be at 60 degrees.
	g_fAngleA = fAngleA;

// Origin at A.
	g_fSAx = 0.0f;
	g_fSAy = 0.0f;

// Sensor B at top.
	if( (fdTelnet>0) && g_nTSMDebug.m_bUltrasonicsDebug ) printf("\nRADs Cos fAngleA= %5.3f  Cosf fAngleA= %5.3f", cos(60.0f*PI/180), cosf(60.0f*PI/180) );
	g_fSBx = fAB * cos(fAngleA*fDegRad);		//
	g_fSBy = fAB * sin(fAngleA*fDegRad);		//
	if( (fdTelnet>0) && g_nTSMDebug.m_bUltrasonicsDebug )
		printf("\ng_fSBy= %5.1f SinfAngleA= %5.2f fAngleA= %5.1f", g_fSBy, sin(fAngleA*fDegRad), fAngleA );
// Sensor C on X axis.
	g_fSCx = fAC;			// AC along X axis.
	g_fSCy = 0.0f;		//

	if( (fdTelnet>0) && g_nTSMDebug.m_bUltrasonicsDebug )
		printf("\nSenA Coords (%5.1f, %5.1f) SenB Coords (%5.1f, %5.1f) SenC Coords (%5.1f, %5.1f) ", g_fSAx, g_fSAy, g_fSBx, g_fSBy, g_fSCx, g_fSCy);

}


// Bubble coordinates are the coordinates of points on the bubble where each sensor touches the bubble.
// B1 = (B1x, B1y) where sensor 1 (A) intersects the buble.
// B2 = (B2x, B2y) where sensor 2 (B) intersects the buble.
// B3 = (B3x, B3y) where sensor 3 (C) intersects the buble.

void CalculateBubbleCoordinates( void )
{
	float fDegRad = PI/180;		// convert degrees to Radians.

	float fBC = g_stWidthCalibration.m_fLength_BC;
	float fAC = g_stWidthCalibration.m_fLength_AC;
	float fAB = g_stWidthCalibration.m_fLength_AB;

	float d1, d2, d3;			// distance to bubble for sensors 1, 2 and 3.
	float fVb;					// angle of sensor B from vertical.

	float fSnsrAngleA = g_stWidthCalibration.m_fSensorAngleA;
	float fSnsrAngleB = g_stWidthCalibration.m_fSensorAngleB;
	float fSnsrAngleC = 180.0f - g_stWidthCalibration.m_fSensorAngleC;		//internal angle.


//sensor B coords.
//	float fSBx = g_fSBx;
//	float fSBy = g_fSBy;

//sensor to bubble distances.
	d1 = g_fUSSensorDistance[0];
	d2 = g_fUSSensorDistance[1];
	d3 = g_fUSSensorDistance[2];

// B1 coordinates
//  d1cos@, d1sin@
	g_fB1x = d1*cos(fSnsrAngleA*fDegRad);
	g_fB1y = d1*sin(fSnsrAngleA*fDegRad);
// B2 coordinates
	// angle of sensor B from vertical.
	if(fSnsrAngleB > 270)
	{
		fVb = fSnsrAngleB - 270;
		g_fB2x = g_fSBx + d2*sin(fVb*fDegRad);
	}
	else
	{
		fVb = 270 - fSnsrAngleB;
		g_fB2x = g_fSBx - d2*sin(fVb*fDegRad);
	}
	g_fB2y = g_fSBy - d2*cos(fVb*fDegRad);

/*
    if(fSnsrAngleB > 270)
		g_fB2x = g_fSBx + d2*Sin(fVb);
	else
		g_fB2x = g_fSBx - d2*Sin(fVb); */



	//assuming sensor B angle = 270 degrees.  => intersect vertically below the sensor.
	g_fB2x = g_fSBx;
	g_fB2y = g_fSBy - d2;		// sensor Y - sensor to bubble distance (d2).
/*

*/
// B3 coordinates.
	g_fB3x = fAC - d3*cos(fSnsrAngleC*fDegRad);
	g_fB3y = 	   d3*sin(fSnsrAngleC*fDegRad);

	if( (fdTelnet>0) && g_nTSMDebug.m_bUltrasonicsDebug )
		printf("\nB1 C0ords (%5.1f, %5.1f) B2 Coords (%5.1f, %5.1f) B3 Coords (%5.1f, %5.1f) Ang C= %5.1f StAng C= %5.1f  ", g_fB1x, g_fB1y, g_fB2x, g_fB2y, g_fB3x, g_fB3y, fSnsrAngleC, g_stWidthCalibration.m_fSensorAngleC );

}
// CalculateSensorAnglesAtLayflatCalibration() calculates sensor angles from the layflat calibration.   Angles are calculated based on sensor-sensor lengths and
// 		distances to bubble,  and bubble radius.  Bubble radius comes from the layflat calibration,  ie it gives the bubble diameter.
void CalculateSensorAnglesAtLayflatCalibration( void )
{
// TODO
	;

}
///////////////////////////////////////////////////////////////////////
// CalculateTriangleAngleA() calculates an angle in a triangel based on the lengths of the sides
// fAdj1 and fAdj2 - are the adjacent triangle side lengths.
// fOpp	 is the opposite side of the triangle to Angle.
/* Angle A
	Cos(A) = b2 + c2 –a2 / 2*b*c

	Cos(A) = (AC2+AB2 – BC2 )  /  (2 BC * AC)

	Cos(A) =  (AC*AC + AB *AB  – BC*BC )  /  (2 AB * AC)
	A =  Cos-1(AC*AC + AB *AB  – BC*BC )  /  (2 AB * AC)
	A =  acos (AC*AC + AB *AB  – BC*BC )  /  (2 AB * AC)

                                        B
                                       / \
                                  AB  /   \ BC
                                   c /     \a
                                    /       \
                                   /         \
                            A(0,0)/___________\C
                                       b  AC
 */
float  CalculateTriangleAngleA( float fAdj1, float fAdj2, float fOpp)
{
	// using cosine rule.   Cos(A) = (b2 + c2 - a2)/2bc. = (adj1**2 + adj2**2 - Opp**2) / 2*adj1*adj2.
	float fAngleA;
	float fRadDeg =  180.0f/PI;  // convert degrees to Radians multiplier.
//          fV1 = (b2   +         c2        - a2)      / 2bc
	float fV1 = (fAdj1*fAdj1 + fAdj2*fAdj2 - fOpp*fOpp)/(2*fAdj1*fAdj2);
// Cos(fAngleA) = fV1
// fAngleA = Cos-1(fV1);
//	fAngleA =  acos(fTemp) * fRadDeg;

	fAngleA = acos(fV1) * fRadDeg;
	return(fAngleA);
}
//
// void CalculateSensorAngles( void ) - intended to be called on the layflat calibration,   ie when radius of bubble has been calculated.
// It assummes that the sensors are aligned, pointing to the centre of the die,  and the bubble is centred on die.
void CalculateSensorAngles( float fRadius )
{
float d1 = g_fUSSensorDistance[0];
float d2 = g_fUSSensorDistance[1];
float d3 = g_fUSSensorDistance[2];
bool bCanDo = TRUE;						// checks will be done to see if can do calculations,  need valid distances.
	if(d1 < 100.0f)	bCanDo = FALSE;		// sensor to bubble distances must not be zero,  must be > 100mm
	if(d2 < 100.0f)	bCanDo = FALSE;		// sensor to bubble distances must not be zero,  must be > 100mm
	if(d3 < 100.0f)	bCanDo = FALSE;		// sensor to bubble distances must not be zero,  must be > 100mm
//sensor to sensor distances must be valid
	if( g_stWidthCalibration.m_fLength_AB  < 200.0f )  bCanDo = FALSE;
	if( g_stWidthCalibration.m_fLength_BC  < 200.0f )  bCanDo = FALSE;
	if( g_stWidthCalibration.m_fLength_AC  < 200.0f )  bCanDo = FALSE;


/*
		            B
		           / \
		      AB  /   \ BC
		       c /     \a
		        /       \
			   /         \
		A(0,0)/___________\C
		          b  AC
*/
	if( bCanDo )
	{
// 1.  sensor A angle.   angle between line AC,  and line from A to centre of bubble/circle.
// triangle formed by AC, d1+r d3+r
//
// 						CalculateTriangleAngleA( float fAdj1, float fAdj2, float fOpp)
		float fSenAAngle = CalculateTriangleAngleA( g_stWidthCalibration.m_fLength_AC, d1+fRadius, d3+fRadius);

// 2. Sensor B angle.
	// need some intermediary steps.
	// angle y.  (angle between sensor B - centre,  and sensor B - sensor A
		float fAngley =  CalculateTriangleAngleA( g_stWidthCalibration.m_fLength_AB, d2+fRadius, d1+fRadius);
	// angle x (angle between AB and horizontal.  Equivalent to angle between AB and AC
		float fAnglex =  CalculateTriangleAngleA( g_stWidthCalibration.m_fLength_AB, g_stWidthCalibration.m_fLength_AC, g_stWidthCalibration.m_fLength_BC);
	//
		float fSenBAngle = 180 + fAnglex + fAngley;

// 3. sensor C angle.  angle between line AC,  and line from C to centre of bubble/circle.
		float fSenCAngle = CalculateTriangleAngleA( g_stWidthCalibration.m_fLength_AC, d3+fRadius, d1+fRadius);
// store in globals:
		g_fSensorAAngle = fSenAAngle;
		g_fSensorBAngle = fSenBAngle;
		g_fSensorCAngle = fSenCAngle;

		if( (fdTelnet>0) && g_nTSMDebug.m_bUltrasonicsDebug )
			printf("\n..CalculateSensorAngles  SenA= %5.1f SenB= %5.1f  SenC= %5.1f", fSenAAngle, fSenBAngle, fSenCAngle );

	}
	else	// ie. cant do.
		if( (fdTelnet>0) && g_nTSMDebug.m_bUltrasonicsDebug )  iprintf("\n..CalculateSensorAngles -  Cant Do");
}
// copy the Calcluated sensor angles to MB.
void CopyCalculatedSensorAnglesToMB( void )
{
	g_arrnMBTable[WIDTH_SUMMARY_CALC_ANGLE_A] = (WORD)(g_fSensorAAngle * 10);
	g_arrnMBTable[WIDTH_SUMMARY_CALC_ANGLE_B] = (WORD)(g_fSensorBAngle * 10);
	g_arrnMBTable[WIDTH_SUMMARY_CALC_ANGLE_C] = (WORD)(g_fSensorCAngle * 10);

}
