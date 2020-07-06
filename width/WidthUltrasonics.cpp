/*  P.Smith							19/7/2010
 * CalculateDistanceToCentre is converted to allow a float to be passed
 * to the function.
 * allow for positive or negative offset value.
 */
// M.McKiernan	wfh					21/4/2020
// DisplayLedStatus( void ) - was empty function.
// not currently working.   Has bones of code,  but needs testing and debugging to make work.
//
// wfh	21/4/2020	stuff to allow selection of reference sensor or 4th sensor - has been removed.
//		void AdvanceToNextChannel( void )
// 8.6.2020
// void TestDrivingLEDs(void) added.
// void PITTestForUSMuxLEDs(void)
// added functions for driving use when driving LEDs.
// void SetA0Lo( void )    SRC7On();
// void SetA0Hi( void )    SRC7Off();
// void USMuxClockLo( void ) SRC9On();
// void USMuxClockHi( void ) SRC9Off();
// void DisableUSMux1(void)  SRC10On();
// void EnableUSMux1(void)   SRC10Off();
// Added new function:- void CalculateM2Offset( float fLayflatmm)
//		g_stWidthCalibration.m_fM2WidthOffset = g_fM2Offset;				// Width offset for M2 measurement
//		g_stWidthCalibration.m_nM2WidthOffsetSign = g_nM2OffsetSign;		// sign of of offset
// Functions in preparation for 4 measurement sensors.   Not used in this version.
// // wfh	21/4/2020	stuff to allow selection of reference sensor or 4th sensor - removed.
//			void AdvanceToNextChannel( void )
//void SetUS4RelayForReferenceSensor(void)
//		SRC6On();		// will activate US4 relay.
//void SetUS4RelayFor4thSensor(void)
//		SRC6Off();		// will deactivate US4 relay.
// wfh - 29.6.2020.
// Corrected void SetUltrasonicMux( void ).
//	if((g_wUltrasonicSensorNo & 0x04) == 0)	// ie. sensors 0-3,  first USMUX #1 should be enabled.
//	{ // mux 1 enable
//        SRC10Off();		// output off enables the card. - USMUX 1

//*****************************************************************************
// INCLUDES
//*****************************************************************************
#include <stdio.h>
#include "WidthVars.h"
#include "SSIFPeripheralMBIndices.h"
#include "BatVars.h"
#include "BatVars2.h"
#include "WidthDef.h"
#include "WidthMB.h"
#include "UltrasonicRead.h"
#include "WidthUltrasonics.h"
#include "SetpointFormat.h"
#include "OneWire.h"

extern structSetpointData   g_CurrentRecipe;

//wfh
float	g_fM2Offset;
float	g_nM2OffsetSign;

extern float 	g_fUSMethod2Diameter;
extern float 	g_fUSMethod2Layflat;
bool g_bTestDLs = FALSE;
int  g_nPITUSMuxCtr = 0;
/* ====================================================================
Function:     SetUltrasonicMux - sets ultrasonic mux for reading data from us
Parameters:   N/A
Returns:      N/A

======================================================================= */

void SetUltrasonicMux( void )
{
//	DisplayLedStatus();
	if((g_wUltrasonicSensorNo & 0x01) == 0)
	{ // set A0 to low
        SRC7On();
	}
	else
	{ // set A0 to high
        SRC7Off();
	}
	if((g_wUltrasonicSensorNo & 0x02) == 0)
	{ // set A1 to low
        SRC8On();
	}
	else
	{ // set A1 to high
        SRC8Off();
	}
	if((g_wUltrasonicSensorNo & 0x04) == 0)	// ie. sensors 0-3,  first USMUX #1 should be enabled.
	{ // mux 1 enable
        SRC10Off();		// output off enables the card. - USMUX 1
        SRC6On();		// output on disables the card. - USMUX 2

//        SRC6Off();
//        SRC10On();
	}
	else
	{ // set mux 2 enable
        SRC6Off();
        SRC10On();

//        SRC10Off();
//        SRC6On();
	}
}



/* ====================================================================
Function:     DisplayLedStatus -  asm DISPLAYLEDSTATUS1
Parameters:   N/A
Returns:      N/A

======================================================================= */

void DisplayLedStatus( void )
{
	// Test version,   will display sensor being accessed.
	BYTE nMask = 0xFF;
	int i=0;
	int nSensor = g_wUltrasonicSensorNo;
//TEST....can set nSensor = a value 1-4.
//TODO testonly.
	nSensor = (BYTE)g_cBlenderNetworkAddress;			// set it equal to the hex switch value

	DisableUSMux1(); 	// cos DisplayLedStatus() changes A0.
	if(nSensor == 4)
		nMask = 0xFE; 							//1111 1110 -  0 turns on LED. 4TH gets clocked on to pos. Q4.
	else if(nSensor == 3)
		nMask = 0xFD;							//1111 1101 -
	else if(nSensor == 2)
		nMask = 0xFB;							//1111 1011 -
	else if(nSensor == 1)
		nMask = 0xF7;							//1111 01111 - NoTE 1 IS LAST CLOCKED IN.
// Called in Timer0_ISR interrupt so caused problems.
	if( (fdTelnet>0) && g_nTSMDebug.m_bUltrasonicsDebug )
			iprintf("\n Sensor= %d Mask= %02x HexSw= %d", nSensor, nMask, g_cBlenderNetworkAddress);
	i=0;
	//while(i<4)

	for( i = 0; i < 4; i++ )
	{
		USMuxClockLo();
		//7.5.2020 this seems arseways but going to try.
		if(nMask & 0x01)
			SetA0Hi();
		else
			SetA0Lo();

		Delay_uS(20);

//		Delay_uS(5);
		USMuxClockHi();
		Delay_uS(5);

		//i++;
		nMask >>= 1;
	}

/*
	//1.
	USMuxClockLo();
	SetA0Lo();			//0
	Delay_uS(20);
	USMuxClockHi();
	Delay_uS(5);
	USMuxClockLo();
	//2.
	USMuxClockLo();
	SetA0Lo();			//0
	Delay_uS(20);
	USMuxClockHi();
	Delay_uS(5);
	USMuxClockLo();
	//3.
	USMuxClockLo();
	SetA0Hi();			//1
	Delay_uS(20);
	USMuxClockHi();
	Delay_uS(5);
	USMuxClockLo();
	//4.
	USMuxClockLo();
	SetA0Lo();			//0
	Delay_uS(20);
	USMuxClockHi();
	Delay_uS(5);
	USMuxClockLo();
*/

	EnableUSMux1(); 	// cos DisplayLedStatus() changes A0.

}

void TestDrivingLEDs( void )
{
	// this will output a 1 followed by 2 0's.   At input to mux chip CD4015.
	if( (fdTelnet>0) && g_nTSMDebug.m_bUltrasonicsDebug )
			iprintf("\n TestDrivingLEDs 1.0.0 ");

		USMuxClockLo();
		Delay_uS(5);
		SetA0Lo();			//  "0"			//should turn on LED.
		Delay_uS(20);
		USMuxClockHi();		// clocking the input.
		Delay_uS(5);
//..
		USMuxClockLo();
		Delay_uS(5);
		SetA0Hi();			//  "1"		//should turn off LED.
		Delay_uS(20);
		USMuxClockHi();		// clocking the input.
		Delay_uS(5);
//....
		USMuxClockLo();
		Delay_uS(5);
		SetA0Hi();			//  "1"		//should turn off LED.
		Delay_uS(20);
		USMuxClockHi();		// clocking the input.
		Delay_uS(5);

		USMuxClockLo();
		Delay_uS(5);
		SetA0Hi();			//  "1"		//should turn off LED.
		Delay_uS(20);
		USMuxClockHi();		// clocking the input.
		Delay_uS(5);

		USMuxClockLo();		//return clk to low.

}
// Basic test for USMux i/p's
void PITTestForUSMuxLEDs(void)
{
	g_nPITUSMuxCtr++;
	switch(g_nPITUSMuxCtr)
     {
     case 1:   //
 		SetA0Hi();			//  "1"		//should turn off LED.
 		USMuxClockLo();
          break;
     case 2:   //
    	 USMuxClockHi();		// clocking the input.
          break;
     case 3:   //
    	 USMuxClockLo();		//return clk to low.
          break;
     case 4:
    	 SetA0Lo();			//  "0"			//should turn on LED.
    	 break;
     case 5:   //
    	 USMuxClockHi();		// clocking the input.
          break;
     case 6:   //
    	 USMuxClockLo();		//return clk to low.
          break;
     case 7:   //
    	 SetA0Hi();			// A0 hi
          break;
     case 8:   //
    	 SetA0Lo();			//  "0"
          break;
     case 9:   //
    	 g_nPITUSMuxCtr = 0;		// reset counter.
          break;
     default:   //
    	 g_nPITUSMuxCtr = 0;
    	 USMuxClockLo();
          break;
     }

}

void SetA0Lo( void )
{
	SRC7On();
}
void SetA0Hi( void )
{
	SRC7Off();
}
void USMuxClockLo( void )
{
	SRC9On();
}
void USMuxClockHi( void )
{
	SRC9Off();
}
void DisableUSMux1(void)
{
	SRC10On();
}
void EnableUSMux1(void)
{
	SRC10Off();
}


/* ====================================================================
Function:     AdvanceToNextChannel -  asm ADVANCETONEXTCHANNEL
Parameters:   N/A
Returns:      N/A

======================================================================= */

void AdvanceToNextChannel( void )
{
	g_wUltrasonicSensorNo++;
	if(g_wUltrasonicSensorNo >= NUMBER_OF_ULTRASONICS_SENSORS)
	{
		g_wUltrasonicSensorNo = 0;
	}
	SetUltrasonicMux();
}

/* ====================================================================
Function:     AverageUltrasonicReading  asm AVERAGEULREADINGS
Parameters:   N/A
Returns:      N/A

======================================================================= */

void AverageUltrasonicReading( void )
{
	unsigned int i;
	WORD wAverageFactor;
	float fAverage,fAllowedDeviation,fDiff;
	i = g_wUltrasonicSensorNo;
	g_fUltrasonicEchoTimeSummation[i] += (float)g_lUltrasonicEchoTime[i];
	g_wUltrasonicSummationCtr[i]++;
	if(g_wUltrasonicSensorNo == REFERENCE_SENSOR_NUMBER-1)
	{
		wAverageFactor = ULTRASONIC_SAMPLE_AVERAGE;
	}
	else
	{
		wAverageFactor = ULTRASONIC_AVERAGING_FACTOR * g_stWidthCalibration.m_wUltrasonicAveraging;
	}
	if(g_wUltrasonicSummationCtr[i] > wAverageFactor)
	{
		fAverage = g_fUltrasonicEchoTimeSummation[i] /(float)g_wUltrasonicSummationCtr[i];
		g_wUltrasonicSummationCtr[i] = 0;
		g_fUltrasonicEchoTimeSummation[i] = 0.0f;
		// decide if the value should be accepted.

		if(g_wUltrasonicSensorNo == REFERENCE_SENSOR_NUMBER-1)
		{
			fAllowedDeviation = g_lUltrasonicEchoTimeAverage[i] * (REFERENCE_SENSOR_PER_DEVIATION_ALLOWED)/100.0f;
		}
		else
		{
			fAllowedDeviation = g_lUltrasonicEchoTimeAverage[i] * (SENSOR_PER_DEVIATION_ALLOWED)/100.0f;
		}
		fDiff = fAverage - g_lUltrasonicEchoTimeAverage[i];
		if(fDiff < 0)
		{
		    fDiff *= -1.0;
		}
//		if(fdTelnet>0)
//		{
//			printf("\n difference for sensor %d is %f",i+1,fDiff);
//		}
		if(fDiff > fAllowedDeviation)
		{
			g_wDeviationExceededCounter[i]++;
			if(g_wDeviationExceededCounter[i] >= MAX_ULTRASONIC_DEVIATIONS_ALLOWED)
			{
				g_wDeviationExceededCounter[i] = 0;
				g_lUltrasonicEchoTimeAverage[i] = fAverage;
			}
		}
		else
		{
			g_lUltrasonicEchoTimeAverage[i] = fAverage;
		}
		if(g_bAllowFirstUltrasonicRead[i])
		{
			g_bAllowFirstUltrasonicRead[i] = FALSE;
			g_lUltrasonicEchoTimeAverage[i] = fAverage;
		}
	}
}

/* ====================================================================
Function:     CalibrateUltrasonicReferenceSensor  asm CALIBRATESENSOR4
Parameters:   N/A
Returns:      N/A

======================================================================= */

void CalibrateUltrasonicReferenceSensor(WORD wWidthMMCalibrateAt)
{
	if(g_lUltrasonicEchoTimeAverage[REFERENCE_SENSOR_NUMBER-1] > 0)
	{
		g_stWidthCalibration.m_fUltrasonicMMConversionFactor = g_lUltrasonicEchoTimeAverage[REFERENCE_SENSOR_NUMBER-1]/(float)wWidthMMCalibrateAt;
		g_stWidthCalibration.m_wUltrasonicMMCalibratedAt = wWidthMMCalibrateAt;
	}
	g_bSaveWidthControlCalibration = TRUE;

	if( (fdTelnet>0) && g_nTSMDebug.m_bUltrasonicsDebug )
		printf("\n  cts per mm is %f",g_stWidthCalibration.m_fUltrasonicMMConversionFactor);
}

/* ====================================================================
Function:     CalibrateUltrasonicReferenceSensor  asm CALIBRATESENSOR4
Parameters:   N/A
Returns:      N/A

======================================================================= */

void AutoCalibrateReferenceSensor(void)
{
	if(g_stWidthCalibration.b_USReferenceAutoCalibration)
	{
		if(g_lUltrasonicEchoTimeAverage[REFERENCE_SENSOR_NUMBER-1] > 0)
		{
			g_stWidthCalibration.m_fUltrasonicMMConversionFactor = g_lUltrasonicEchoTimeAverage[REFERENCE_SENSOR_NUMBER-1]/(float)g_stWidthCalibration.m_wUltrasonicMMCalibratedAt;
			if(g_bFirstReferenceAutoCalibrate)
			{
				g_bSaveWidthControlCalibration = TRUE;
			}
		}
	}
}



/* ====================================================================
Function:     CalculateUltrasonicDistances  asm CALCULATEULTRASONICDISTANCES
Parameters:   N/A
Returns:      N/A

======================================================================= */

void CalculateUltrasonicDistances(void)
{
	unsigned int i;
	if((g_stWidthCalibration.m_wWidthMode == ULTRASONIC_MODE)||(g_stWidthCalibration.m_wWidthMode == ULTRASONIC_AND_LAYFLAT_MODE))
	{
		for(i=0; i < NUMBER_OF_ULTRASONICS_SENSORS; i++)
		{
			g_fUSSensorDistance[i] = (float)g_lUltrasonicEchoTimeAverage[i] /g_stWidthCalibration.m_fUltrasonicMMConversionFactor;
			// radius = distance to centre - distance = radius
			g_fUSSensorRadius[i] = g_stWidthCalibration.m_fDistanceToCentre[i] - g_fUSSensorDistance[i];
		}
		g_fUSSensorAverageRadius = (g_fUSSensorRadius[0]+g_fUSSensorRadius[1]+g_fUSSensorRadius[2])/3.0f;
		// now add offset to get diameter
		g_fUSSensorAverageDiameter = 2.0f * g_fUSSensorAverageRadius;
		if(g_CurrentRecipe.m_wUSLayflatOffsetSign == NEGATIVE_SIGN)
		g_fUSSensorAverageRadiusWithOffset = g_fUSSensorAverageRadius - ((float)g_CurrentRecipe.m_fDesiredWidthOffset / PI);
		else
		g_fUSSensorAverageRadiusWithOffset = g_fUSSensorAverageRadius + ((float)g_CurrentRecipe.m_fDesiredWidthOffset / PI);


		g_fUSSensorAverageDiameterWithOffset = 2.0 * g_fUSSensorAverageRadiusWithOffset;
		// now calculate lay flat width
		g_fUSSensorAverageLayFlatWidth = g_fUSSensorAverageRadius * PI; // average lay flat
		g_fUSSensorAverageLayFlatWidthWithOffset = g_fUSSensorAverageRadiusWithOffset * PI; // average lay flat with offset
		g_fActualWidth = g_fUSSensorAverageLayFlatWidthWithOffset;
	}
}

/* ====================================================================
Function:     CalculateDistanceToCentre  asm CALCULATEDISTANCETOCENTRE
Distance to centre =  distance + radius
Parameters:   N/A
Returns:      N/A

======================================================================= */

void CalculateDistanceToCentre(float fDiamter)
{
	unsigned int i;
	float fRadius;
	fRadius = (fDiamter)/ 2.0f;  // calculate the radius
	for(i=0; i < NUMBER_OF_ULTRASONICS_SENSORS-1; i++)
	{
		g_stWidthCalibration.m_fDistanceToCentre[i] = g_fUSSensorDistance[i] + fRadius;
		if(fdTelnet>0)
		printf("\n dis to c for %d is %f",i+1,g_stWidthCalibration.m_fDistanceToCentre[i]);
	}
	g_stWidthCalibration.m_fDistanceToCentre[REFERENCE_SENSOR_NUMBER-1] = 0.0f;
	g_bSaveWidthControlCalibration = TRUE;
}

//wfh
void CalculateM2Offset( float fLayflatmm)
{
	if( fLayflatmm > g_fUSMethod2Layflat )
	{
		//Calibrate value > measured.
		g_fM2Offset = fLayflatmm - g_fUSMethod2Layflat;
		g_nM2OffsetSign = POSITIVE_SIGN;
	}
	else
	{
		//Calibrate value < measured.
		g_fM2Offset = g_fUSMethod2Layflat - fLayflatmm;
		g_nM2OffsetSign = NEGATIVE_SIGN;

	}

    g_stWidthCalibration.m_fM2WidthOffset = g_fM2Offset;				// Width offset for M2 measurement
    g_stWidthCalibration.m_nM2WidthOffsetSign = g_nM2OffsetSign;		// sign of of offset

	g_bSaveWidthControlCalibration = TRUE;
}

/////////WIP////////////////1.4.2020/////////////////////

/* ====================================================================
Function:     AdvanceToNextChannel -  asm ADVANCETONEXTCHANNEL
Parameters:   N/A
Returns:      N/A

======================================================================= */
bool 	g_bUS4TimeForReference = FALSE;
bool	g_bUS4ReferenceSelected = FALSE;	// UltraSonic sensor 4 is not reference sensor.
int		g_nUS4SelectTimer = 0;				// timer for selecting 4th ultrasonic sensor
										// incremented on 1 Hz to US4_REFERENCE_TIME.
#define US4_REFERENCE_TIME		(300)	//300 seconds,  5 minutes.

/*   wfh   stuff to allow selection of reference sensor or 4th sensor.
	g_nUS4SelectTimer++;
	if(g_nUS4SelectTimer > US4_REFERENCE_TIME )
	{
		g_bUS4TimeForReference = TRUE;
		g_nUS4SelectTimer = 0;
	}
*/
/*  // wfh	21/4/2020	stuff to allow selection of reference sensor or 4th sensor - removed.
void AdvanceToNextChannel( void )
{
	g_wUltrasonicSensorNo++;		// range for g_wUltrasonicSensorNo = 0-3

	if(g_wUltrasonicSensorNo >= NUMBER_OF_ULTRASONICS_SENSORS)
	{
		g_wUltrasonicSensorNo = 0;
	}
	SetUltrasonicMux();

	//If Advancing from Sensor 4 (sensor no will now be 0.)
	//new 1.4.2020

	if(g_wUltrasonicSensorNo == 0)		// sensor had been in 4th sensor position on entry. - can be sensor 4 or reference.
	{
		if(g_bUS4TimeForReference)
		{
			g_bUS4TimeForReference = FALSE;
			SetUS4RelayForReferenceSensor();		// set US4 relay so that reference sensor switched in to sensor 4 position.
			g_bUS4ReferenceSelected = TRUE;			// Indicates reference sensor selected.
		}
		else if(g_bUS4ReferenceSelected)			// had been using Ref sensor.
		{
			g_bUS4ReferenceSelected = FALSE;
			SetUS4RelayFor4thSensor();		// set US4 relay so that 4th sensor switched in to sensor 4 position.
			g_nUS4SelectTimer = 0;			// clear the US4 select timer.

		}

	}


}
*/   //mmk end wfh	21/4/2020

void SetUS4RelayForReferenceSensor(void)
{
	SRC6On();		// will activate US4 relay.
}
void SetUS4RelayFor4thSensor(void)
{
	SRC6Off();		// will deactivate US4 relay.
}

