/*  P.Smith							10/5/10
 *  added CheckForStandardise, verified that standardise is operational
	added proper copy of width control average to mb, make sure it is rounded

	P.Smith							2/7/10
	added WIDTH_CONFIG_US_AVERAGING

	P.Smith							7/7/10
	changed variables for width data to one decimal place.
	added ultrasonic averaging, auto calibration,cal lf from us
	sample no, overrun, overrun deadband
	ultrasonic reference and diameter commands written
	copy us summary data to modbus counts per mm, all ultrasonic widths

	P.Smith							7/7/10
	added bubble break and alarm on / off status copy.

	P.Smith							8/7/10
	added standardise in auto,standardise on hour,alarm in manual
	us ref auto calibrate, calibrate us from layflat

	P.Smith							19/7/10
	correct imperial conversion of integrated width.
	increase the resolution of the calculation of the distance to the centre
	make the varaible a float.
	added layflat offset and offset sign.
	added g_bWidthUltrasonicDiameterFromLayFlatEntry check,this calculates the diameter
	from the lay flat width.
	added offset sign and offset copy.
	integrated width inserted imperial conversion added.
	define M_ROUND_DECIMAL_PLACES,M_DECIMAL_PLACES_MULTIPLICATION
	I_ROUND_DECIMAL_PLACES,I_DECIMAL_PLACES_MULTIPLICATION


	P.Smith							10/8/10
	added m_bFastCorrectionEnabled

	P.Smith							12/8/10
	allow for width set point to be written, save the set point written to
	eeprom.
	set  g_TempRecipe.m_nFileNumber when writing offset value

	P.Smith							20/9/10
	set point is now 1 decimal place, allow for this when the set point is
	written down
	copy the actual width into its own area WIDTH_SUMMARY_WIDTH

	P.Smith							23/9/10
	use PSUM_I_DECIMAL_PLACES_MULTIPLICATION & PSUM_M_DECIMAL_PLACES_MULTIPLICATION
	for set point and actual width

	P.Smith							25/3/11
	copy width average to the modbus table address WIDTH_SUMMARY_AVERAGE_WIDTH
	note that this is copied to one decimal place for imperial data

	M.McKiernan						21/4/2020
// MMK wfh 21/4/2020 input Sensor to Sensor distances.
// CopyWidthConfigFromMB()   input lengths in mm and to tenths of inches in Imperial units.
// MMK wfh 21/4/2020 copy Sensor to Sensor distances to MB.
// // wfh Sensor Angles - hardcoded for now.
// CopyWidthConfigToMB( void ) -   copy Sensor to Sensor distances to MB. in mm and 10ths of inches in Imperial.
// Copy sensor angles to MB. - in degrees. no dp.
// 		g_arrnMBTable[WIDTH_COMMAND_US_LAYFLAT_OFFSET] = (WORD)( g_CurrentRecipe.m_fDesiredWidthOffset *  10.0f * g_fWidthConversionFactor );
//	else // ie  (metric) value in mm.
//		g_arrnMBTable[WIDTH_COMMAND_US_LAYFLAT_OFFSET] = (WORD)g_CurrentRecipe.m_fDesiredWidthOffset;
// Added WidthManualFastIncrease();
// 	if(g_bWidthIncreaseFastCorrectionCommandWritten)
// 	if(g_bWidthUltrasonicLayFlatOffsetWritten)
//   	enter offset in mm or 10ths of inches (imperial)  fValue = (fValue / 10.0f) / g_fWidthConversionFactor;  //  assume entry in 10ths of inches.
//  	if(g_bWidthUltrasonicDiameterFromLayFlatEntry)
//			Width entered in mm and 10ths of inches.
//  			CalculateDistanceToCentre(fDiameter);			//expecting diameter in mm.
//				CalculateM2Offset(fLayflatmm);			// calculate layflat offset for M2 method. - will be in mm.
//  In  CopyUltrasonicSummaryDataTOMB( void )
//			g_arrnMBTable[WIDTH_SUMMARY_US_SENSOR_OK_1+i] = g_bUSSensorOK[i];
//
// if(g_bWidthUltrasonicDiameterFromLayFlatEntry)     Call  CalculateSensorAngles(fRadius);			// calculate sensor angles,
// In void CopyWidthSummaryDataTOMB( void ) -  call CopyCalculatedSensorAnglesToMB();
*/
//*****************************************************************************
// INCLUDES
//*****************************************************************************
#include <stdio.h>
#include "WidthVars.h"
#include "SSIFPeripheralMBIndices.h"
#include "BatVars.h"
#include "batvars2.h"
#include "WidthDef.h"
#include "WidthMB.h"
#include "Width.h"
#include "Serialstuff.h"
#include "WidthForeground.h"
#include "WidthControlAlgorithm.h"
#include "SetpointFormat.h"
#include "UltrasonicRead.h"
#include "WidthUltrasonics.h"
#include "ConfigurationFunctions.h"


extern int g_arrnReadSSIFMBTable[];
extern  structControllerData    structSSIFData;
extern structSetpointData   g_CurrentRecipe;
extern  structSetpointData  g_TempRecipe;
extern  structSetpointData  g_CommsRecipe;
extern  StructDebug   g_nTSMDebug;

extern	int g_nUSSensorTriggeredCtr3;
extern	int g_nUSSensorEchosReceived3;
extern bool g_bUSSensorOK3;
//////////////////////////////////////////////////////
// CopyWidthSummmaryDataFromSSIF
//
//
//////////////////////////////////////////////////////
void CopyWidthSummmaryDataFromSSIF( void )
{
	g_nSSIFAlarmStatus = g_arrnReadSSIFMBTable[MB_SSIF_STATUS];
	g_nSSIFInputState = g_arrnReadSSIFMBTable[MB_SSIF_INPUTS];
	g_nLeftMotorPulses = g_arrnReadSSIFMBTable[MB_SSIF_EDGE1_STEPS];
	g_nRightMotorPulses = g_arrnReadSSIFMBTable[MB_SSIF_EDGE2_STEPS];
	g_nTotalPulsesRead = g_arrnReadSSIFMBTable[MB_SSIF_TOTAL_STEPS];
	g_nLeftSensorADC = g_arrnReadSSIFMBTable[MB_SSIF_SENSOR1_ADC];
	g_nRightSensorADC = g_arrnReadSSIFMBTable[MB_SSIF_SENSOR2_ADC];



	WORD nState = g_nSSIFInputState & SSIFLEFTSENSORMASKPOS;
	if(nState == SSIFLEFTSENSORUNCOVEREDCODE )
	{
		g_nLeftSensorStatus = SENSOR_UNCOVERED;
	}
	else
	if(nState == SSIFLEFTSSENSOREDGECODE )
	{
		g_nLeftSensorStatus = SENSOR_AT_EDGE;
	}
	else
	if(nState == SSIFLEFTSENSORCOVEREDCODE )
	{
		g_nLeftSensorStatus = SENSOR_COVERED;
	}

	nState = g_nSSIFInputState & SSIFRIGHTSENSORMASKPOS;
	if(nState == SSIFRIGHTSENSORUNCOVEREDCODE )
	{
		g_nRightSensorStatus = SENSOR_UNCOVERED;
	}
	else
	if(nState == SSIFRIGHTSSENSOREDGECODE )
	{
		g_nRightSensorStatus = SENSOR_AT_EDGE;
	}
	else
	if(nState == SSIFRIGHTSENSORCOVEREDCODE )
	{
		g_nRightSensorStatus = SENSOR_COVERED;
	}

	// limit switches
	if((g_nSSIFInputState & SSIFLEFTSENSORLIMITBITPOS) != 0)
	{
		g_bLeftLimitSwitchActive = TRUE;
	}
	else
	{
		g_bLeftLimitSwitchActive = FALSE;
	}
	if((g_nSSIFInputState & SSIFRIGHTSENSORLIMITBITPOS) != 0)
	{
		g_bRightLimitSwitchActive = TRUE;

	}
	else
	{
		g_bRightLimitSwitchActive = FALSE;
	}
	CheckForStandardise();  // check for standarise

}


//////////////////////////////////////////////////////
// CopyWidthConfigFromMB
// Copies config form modbus
//
//////////////////////////////////////////////////////
void CopyWidthConfigFromMB( void )
{
	float fInches;
	float fLen_mm;
	int		nLen;
	// min width
	g_stWidthCalibration.m_wWidthMode = g_arrnMBTable[WIDTH_CONFIG_MODE];
	// width type
	g_stWidthCalibration.m_wWidthType = g_arrnMBTable[WIDTH_CONFIG_TYPE];
	// line type
	g_stWidthCalibration.m_wLineType = g_arrnMBTable[WIDTH_CONFIG_LINE_TYPE];
	// alarm limit
	g_stWidthCalibration.m_wAlarmLimit = g_arrnMBTable[WIDTH_CONFIG_ALARM_BAND];
	// ultrasonic averaging
	g_stWidthCalibration.m_wUltrasonicAveraging = g_arrnMBTable[WIDTH_CONFIG_US_AVERAGING];

	// process delay
	g_stWidthCalibration.m_wProcessDelayInSeconds = g_arrnMBTable[WIDTH_CONFIG_PROCESS_DELAY];
	g_stWidthCalibration.m_wWidthDeadband = g_arrnMBTable[WIDTH_CONFIG_WIDTH_DEADBAND];
	g_stWidthCalibration.m_fNormalIncreaseControlGainPercentage = float(g_arrnMBTable[WIDTH_CONFIG_NORMAL_INCREASE_CONTROL_GAIN]);
	g_stWidthCalibration.m_fNormalDecreaseControlGainPercentage = float(g_arrnMBTable[WIDTH_CONFIG_NORMAL_DECREASE_CONTROL_GAIN]);
	g_stWidthCalibration.m_fFastCorrectedControlGainPercentage = float(g_arrnMBTable[WIDTH_CONFIG_FAST_CORRECTION_CONTROL_GAIN]);
	g_stWidthCalibration.m_wWidthAveragingInSeconds = g_arrnMBTable[WIDTH_CONFIG_WIDTH_AVERAGE_IN_SECONDS];
	g_stWidthCalibration.m_wWidthIntegrationTimeinSeconds = g_arrnMBTable[WIDTH_CONFIG_WIDTH_CORRECTION_INTERVAL];
    // auto ref cal
	if(g_arrnMBTable[WIDTH_CONFIG_US_AUTO_CALIBRATE_REFERENCE] == 0)
	g_stWidthCalibration.b_USReferenceAutoCalibration = FALSE;
	else
    g_stWidthCalibration.b_USReferenceAutoCalibration = TRUE;
	// calibrate us from layflat
	if(g_arrnMBTable[WIDTH_CONFIG_US_AUTO_CAL_ULTRASONIC_FROM_LF] == 0)
	g_stWidthCalibration.b_USCalibrate_layflat_from_US = FALSE;
	else
	g_stWidthCalibration.b_USCalibrate_layflat_from_US = TRUE;
	// standardise on auto
	if(g_arrnMBTable[WIDTH_CONFIG_OPTION_STD_ON_AUTO] == 0)
	g_stWidthCalibration.m_bStandardiseOnAuto = FALSE;
	else
	g_stWidthCalibration.m_bStandardiseOnAuto = TRUE;
	// hourly standardise
	if(g_arrnMBTable[WIDTH_CONFIG_OPTION_HOUTLY_STD] == 0)
	g_stWidthCalibration.m_bStandardiseOnTheHour = FALSE;
	else
	g_stWidthCalibration.m_bStandardiseOnTheHour = TRUE;
	// alarm in manual
	if(g_arrnMBTable[WIDTH_CONFIG_OPTION_ALARM_IN_MANUAL] == 0)
	g_stWidthCalibration.m_bAlarmInManual = FALSE;
	else
	g_stWidthCalibration.m_bAlarmInManual = TRUE;

	if(g_arrnMBTable[WIDTH_CONFIG_FAST_CORRECTION] == 0)
	g_stWidthCalibration.m_bFastCorrectionEnabled = FALSE;
	else
	g_stWidthCalibration.m_bFastCorrectionEnabled = TRUE;





	g_stWidthCalibration.w_USCalibrate_Layflat_From_US_Interval = g_arrnMBTable[WIDTH_CONFIG_US_AUTO_CAL_US_INTERVAL];
	g_stWidthCalibration.w_USCalibrateLayflatFromUSSampleNo = g_arrnMBTable[WIDTH_CONFIG_US_AUTO_CAL_US_SAMPLE_NO];
	g_stWidthCalibration.m_wWidthOverrun = g_arrnMBTable[WIDTH_CONFIG_OVERRUN];
	g_stWidthCalibration.m_wWidthOverrunDeadband = g_arrnMBTable[WIDTH_CONFIG_OVERRUN_DEADBAND];

// MMK wfh 21/4/2020 input Sensor to Sensor distances.
	// if unit type is metric.
	if(g_fWidthConversionFactor == 1.0f)
	{
		//lengths expected in mm.
		g_stWidthCalibration.m_fLength_AB = (float)(g_arrnMBTable[WIDTH_CONFIG_LENGTH_AB]);  // / 10.0f;
		if( (fdTelnet>0) && g_nTSMDebug.m_bUltrasonicsDebug ) printf("\n(1).%d %5.1f", g_arrnMBTable[WIDTH_CONFIG_LENGTH_AB],g_stWidthCalibration.m_fLength_AB );
		g_stWidthCalibration.m_fLength_BC = (float)(g_arrnMBTable[WIDTH_CONFIG_LENGTH_BC]);  // / 10.0f;
		g_stWidthCalibration.m_fLength_AC = (float)(g_arrnMBTable[WIDTH_CONFIG_LENGTH_AC]);  // / 10.0f;
	}
	else
	{ // English units.
//AB
		nLen = g_arrnMBTable[WIDTH_CONFIG_LENGTH_AB];	//inches to 1 dp.
		fInches = (float)(nLen) / 10.0f;		// length in inches.
		fLen_mm = fInches / g_fWidthConversionFactor;
		g_stWidthCalibration.m_fLength_AB = fLen_mm;
//BC
		nLen = g_arrnMBTable[WIDTH_CONFIG_LENGTH_BC];	//inches to 1 dp.
		fInches = (float)(nLen) / 10.0f;		// length in inches.
		fLen_mm = fInches / g_fWidthConversionFactor;
		g_stWidthCalibration.m_fLength_BC = fLen_mm;
//AC
		nLen = g_arrnMBTable[WIDTH_CONFIG_LENGTH_AC];	//inches to 1 dp.
		fInches = (float)(nLen) / 10.0f;		// length in inches.
		fLen_mm = fInches / g_fWidthConversionFactor;
		g_stWidthCalibration.m_fLength_AC = fLen_mm;

	}
// wfh Sensor Angles - hardcoded for now.
//TODO
	g_stWidthCalibration.m_fSensorAngleA = (float)g_arrnMBTable[WIDTH_CONFIG_SENSOR_A_ANGLE];
	g_stWidthCalibration.m_fSensorAngleB = (float)g_arrnMBTable[WIDTH_CONFIG_SENSOR_B_ANGLE];
	g_stWidthCalibration.m_fSensorAngleC = (float)g_arrnMBTable[WIDTH_CONFIG_SENSOR_C_ANGLE];
//TODO	- hardcode for now.
	g_stWidthCalibration.m_fSensorAngleA = 30.0f;
	g_stWidthCalibration.m_fSensorAngleB = 270.0f;
	g_stWidthCalibration.m_fSensorAngleC = 150.0f;

	g_stWidthCalibration.m_nUSMethod = (BYTE)g_arrnMBTable[WIDTH_CONFIG_US_METHOD];

	//#define  WIDTH_CONFIG_SENSOR_A_ANGLE	            8528   // sensor A angle. - degrees.
	//#define  WIDTH_CONFIG_SENSOR_B_ANGLE	            8529   // sensor B angle.
	//#define  WIDTH_CONFIG_SENSOR_C_ANGLE	            8530   // sensor C angle.


/*
    float		m_fLength_AB;				// distance between sensors A(1) and B(2)
    float		m_fLength_BC;				// distance between sensors B(2) and C(3)
    float		m_fLength_AC;				// distance between sensors A(1) and C(3)
    float		m_fLength_Ref;				// Reference Length
    float		m_fLayflatCalibrateWidth;	// Layflat width for calibration purposes.
    float		m_fTargetCalibrateDiameter;	// Diameter of a cylindrical target for calibration purposes.
*/
	if( (fdTelnet>0) && g_nTSMDebug.m_bUltrasonicsDebug ) printf("\n....%d %5.1f", g_arrnMBTable[WIDTH_CONFIG_LENGTH_AB],g_stWidthCalibration.m_fLength_AB );

	SetSSIFWidthControlType();
	g_bSaveWidthControlCalibration = TRUE;
	g_bCopyWidthConfigFileToSDCard = TRUE;
	if( (fdTelnet>0) && g_nTSMDebug.m_bUltrasonicsDebug )
		printf("\n...x:..%d %f sizeof =%ld", g_arrnMBTable[WIDTH_CONFIG_LENGTH_AB], g_stWidthCalibration.m_fLength_AB, sizeof(g_stWidthCalibration) );

}


//////////////////////////////////////////////////////
// CopyWidthConfigToMB
// Copies config to modbus
//
//////////////////////////////////////////////////////
void CopyWidthConfigToMB( void )
{
	// width mode
	g_arrnMBTable[WIDTH_CONFIG_MODE] = g_stWidthCalibration.m_wWidthMode;
	// width type
	g_arrnMBTable[WIDTH_CONFIG_TYPE] = g_stWidthCalibration.m_wWidthType;
	// line type
	g_arrnMBTable[WIDTH_CONFIG_LINE_TYPE] = g_stWidthCalibration.m_wLineType;
	// alarm limit
	g_arrnMBTable[WIDTH_CONFIG_ALARM_BAND] = g_stWidthCalibration.m_wAlarmLimit;
	// us averaging
	g_arrnMBTable[WIDTH_CONFIG_US_AVERAGING] = g_stWidthCalibration.m_wUltrasonicAveraging;
	// process delay
	g_arrnMBTable[WIDTH_CONFIG_PROCESS_DELAY] = g_stWidthCalibration.m_wProcessDelayInSeconds;
	// deadband
	g_arrnMBTable[WIDTH_CONFIG_WIDTH_DEADBAND] = g_stWidthCalibration.m_wWidthDeadband;
	// control gain
	g_arrnMBTable[WIDTH_CONFIG_NORMAL_INCREASE_CONTROL_GAIN] = (WORD)(g_stWidthCalibration.m_fNormalIncreaseControlGainPercentage);
	g_arrnMBTable[WIDTH_CONFIG_NORMAL_DECREASE_CONTROL_GAIN] = (WORD)(g_stWidthCalibration.m_fNormalDecreaseControlGainPercentage);
	// fc control gain
	g_arrnMBTable[WIDTH_CONFIG_FAST_CORRECTION_CONTROL_GAIN] = (WORD)(g_stWidthCalibration.m_fFastCorrectedControlGainPercentage);
	g_arrnMBTable[WIDTH_CONFIG_WIDTH_AVERAGE_IN_SECONDS] = (WORD)g_stWidthCalibration.m_wWidthAveragingInSeconds;
	g_arrnMBTable[WIDTH_CONFIG_WIDTH_CORRECTION_INTERVAL]= g_stWidthCalibration.m_wWidthIntegrationTimeinSeconds;
	if(g_stWidthCalibration.b_USReferenceAutoCalibration)
	{
		g_arrnMBTable[WIDTH_CONFIG_US_AUTO_CALIBRATE_REFERENCE] = 1;
	}
	else
	{
		g_arrnMBTable[WIDTH_CONFIG_US_AUTO_CALIBRATE_REFERENCE] = 0;
	}
	if(g_stWidthCalibration.b_USCalibrate_layflat_from_US)
	{
		g_arrnMBTable[WIDTH_CONFIG_US_AUTO_CAL_ULTRASONIC_FROM_LF] = 1;
	}
	else
	{
		g_arrnMBTable[WIDTH_CONFIG_US_AUTO_CAL_ULTRASONIC_FROM_LF] = 0;
	}
	// standardise on auto
	if(g_stWidthCalibration.m_bStandardiseOnAuto)
	{
		g_arrnMBTable[WIDTH_CONFIG_OPTION_STD_ON_AUTO] = 1;
	}
	else
	{
		g_arrnMBTable[WIDTH_CONFIG_OPTION_STD_ON_AUTO] = 0;
	}
	// hourly standardise
	if(g_stWidthCalibration.m_bStandardiseOnTheHour)
	{
		g_arrnMBTable[WIDTH_CONFIG_OPTION_HOUTLY_STD] = 1;
	}
	else
	{
		g_arrnMBTable[WIDTH_CONFIG_OPTION_HOUTLY_STD] = 0;
	}
	// alarm in manual
	if(g_stWidthCalibration.m_bAlarmInManual)
	{
		g_arrnMBTable[WIDTH_CONFIG_OPTION_ALARM_IN_MANUAL] = 1;
	}
	else
	{
		g_arrnMBTable[WIDTH_CONFIG_OPTION_ALARM_IN_MANUAL] = 0;
	}
	if(g_stWidthCalibration.m_bFastCorrectionEnabled)
	{
		g_arrnMBTable[WIDTH_CONFIG_FAST_CORRECTION] = 1;
	}
	else
	{
		g_arrnMBTable[WIDTH_CONFIG_FAST_CORRECTION] = 0;
	}

// MMK wfh 21/4/2020 copy Sensor to Sensor distances to MB.
	// if unit type is metric.
	if(g_fWidthConversionFactor == 1.0f)
	{
		//lengths expected in mm to 1dp
		g_arrnMBTable[WIDTH_CONFIG_LENGTH_AB] = (WORD)(g_stWidthCalibration.m_fLength_AB); // * 10);
		g_arrnMBTable[WIDTH_CONFIG_LENGTH_BC] = (WORD)(g_stWidthCalibration.m_fLength_BC); // * 10);
		g_arrnMBTable[WIDTH_CONFIG_LENGTH_AC] = (WORD)(g_stWidthCalibration.m_fLength_AC); // * 10);
	}
	else
	{ // English units.
//AB
		//inches to 1 dp. -> MB
		g_arrnMBTable[WIDTH_CONFIG_LENGTH_AB] = (WORD)(g_stWidthCalibration.m_fLength_AB * g_fWidthConversionFactor * 10);
//BC
		g_arrnMBTable[WIDTH_CONFIG_LENGTH_BC] = (WORD)(g_stWidthCalibration.m_fLength_BC * g_fWidthConversionFactor * 10);
//AC
		g_arrnMBTable[WIDTH_CONFIG_LENGTH_AC] = (WORD)(g_stWidthCalibration.m_fLength_AC * g_fWidthConversionFactor * 10);

	}

//wfh - 22/4/2020  Copy sensor angles to MB. - in degrees. no dp.
	g_arrnMBTable[WIDTH_CONFIG_SENSOR_A_ANGLE] = (WORD)g_stWidthCalibration.m_fSensorAngleA;
	g_arrnMBTable[WIDTH_CONFIG_SENSOR_B_ANGLE] = (WORD)g_stWidthCalibration.m_fSensorAngleB;
	g_arrnMBTable[WIDTH_CONFIG_SENSOR_C_ANGLE] = (WORD)g_stWidthCalibration.m_fSensorAngleC;

	g_arrnMBTable[WIDTH_CONFIG_US_METHOD] = g_stWidthCalibration.m_nUSMethod;

	g_arrnMBTable[WIDTH_COMMAND_US_LAYFLAT_OFFSET_SIGN] = g_CurrentRecipe.m_wUSLayflatOffsetSign;
	if( g_fWidthConversionFactor != 1.0f )	// imperial.  inches to 1dp.
		g_arrnMBTable[WIDTH_COMMAND_US_LAYFLAT_OFFSET] = (WORD)( g_CurrentRecipe.m_fDesiredWidthOffset *  10.0f * g_fWidthConversionFactor );
	else // ie  (metric) value in mm.
		g_arrnMBTable[WIDTH_COMMAND_US_LAYFLAT_OFFSET] = (WORD)g_CurrentRecipe.m_fDesiredWidthOffset;

	g_arrnMBTable[WIDTH_COMMAND_CALIBRATE_ULTRASONIC_REF] = g_stWidthCalibration.m_wUltrasonicMMCalibratedAt;

	g_arrnMBTable[WIDTH_CONFIG_US_AUTO_CAL_US_INTERVAL]= g_stWidthCalibration.w_USCalibrate_Layflat_From_US_Interval;
	g_arrnMBTable[WIDTH_CONFIG_US_AUTO_CAL_US_SAMPLE_NO]= g_stWidthCalibration.w_USCalibrateLayflatFromUSSampleNo;
	g_arrnMBTable[WIDTH_CONFIG_OVERRUN]= g_stWidthCalibration.m_wWidthOverrun;
	g_arrnMBTable[WIDTH_CONFIG_OVERRUN_DEADBAND]= g_stWidthCalibration.m_wWidthOverrunDeadband;
}

//////////////////////////////////////////////////////
// CopyWidthConfigDataToSSIF
//
//
//////////////////////////////////////////////////////
void CheckForWidthMBWrite( void )
{
	float	fPulses,fValue,fDiameter;
	unsigned int nPulses;
	WORD wDiameter,wLayflat;
	if(g_bWidthConfigWritten)
	{
		g_bWidthConfigWritten = FALSE;
		CopyWidthConfigFromMB();
	}
	if(g_bWidthCalibrateInConfigWritten)
	{
		g_bWidthCalibrateInConfigWritten = FALSE;
		WORD wWidth = g_arrnMBTable[WIDTH_CONFIG_CALIBRATE_WIDTH];
		if(g_stWidthCalibration.m_wWidthType == SMALL_LINE_TYPE)
		{
			g_stWidthCalibration.m_fWidthOffset = g_fLeftWidth + g_fRightWidth + (float)wWidth;
		}
		else
		{
			g_stWidthCalibration.m_fWidthOffset = CalculateMinimumWidth(wWidth);
		}

//		g_stWidthCalibration.m_fWidthOffset = wWidth;//nbb--todo--width--remove
		g_bSaveWidthControlCalibration = TRUE;
		CopyWidthConfigToMB();
//		if(fdTelnet>0)
//		{
//			printf("\n min width is %d left %f right %f",g_stWidthCalibration.m_wWidthOffset,g_fRightWidth,g_fLeftWidth);
//		}

	}
	if(g_bMaxWidthCalibrateInConfigWritten)
	{
		g_bMaxWidthCalibrateInConfigWritten = FALSE;
		if(g_stWidthCalibration.m_wWidthType == SMALL_LINE_TYPE) // only used for small line
		{
			fPulses = g_arrnMBTable[WIDTH_CONFIG_MAXIMUM_WIDTH] * (STEPPER_MOTOR_MM_CONVERSION_FACTOR);
			nPulses = (int)fPulses;  //nbb--todo--width check this out.
			SetMaxMotorPulses(nPulses,nPulses);

		}
	}
	if(g_bWidthAutoManualToggleCommandWritten)
	{
		g_bWidthAutoManualToggleCommandWritten = FALSE;
		WidthAutoManualToggle();
	}
	if(g_bWidthIncreaseCommandWritten)
	{
		g_bWidthIncreaseCommandWritten = FALSE;
		WidthManualIncrease();
	}
	if(g_bWidthIncreaseFastCorrectionCommandWritten)
	{
		g_bWidthIncreaseFastCorrectionCommandWritten = FALSE;
		//wfh
		WidthManualFastIncrease();

	}
	if(g_bWidthDecreaseCommandWritten)
	{
		g_bWidthDecreaseCommandWritten = FALSE;
		WidthManualDecrease();
	}
	if(g_bWidthUltrasonicRefCommandWritten)
	{
		g_bWidthUltrasonicRefCommandWritten = FALSE;
		WORD wTemp = g_arrnMBTable[WIDTH_COMMAND_CALIBRATE_ULTRASONIC_REF];
		CalibrateUltrasonicReferenceSensor(wTemp);
	}
	if(g_bWidthUltrasonicDiameterCommandWritten)
	{
		float fValue = 0.0f;
		g_bWidthUltrasonicDiameterCommandWritten = FALSE;
		fValue = (float)g_arrnMBTable[WIDTH_COMMAND_CALIBRATE_DIAMETER];
		// allow for imperial
        if(g_fWidthConversionFactor != 1.0f)
        {
        	fValue = fValue / g_fWidthConversionFactor;
        }
 		CalculateDistanceToCentre(fValue);
	}
	if(g_bWidthUltrasonicLayFlatOffsetSignWritten)
	{
		g_bWidthUltrasonicLayFlatOffsetSignWritten = FALSE;
		if(g_arrnMBTable[WIDTH_COMMAND_US_LAYFLAT_OFFSET_SIGN] == 0)
		{
			g_TempRecipe.m_wUSLayflatOffsetSign = POSITIVE_SIGN;
			g_CurrentRecipe.m_wUSLayflatOffsetSign = POSITIVE_SIGN;
		    g_nSaveRecipeSequence = SAVE_RECIPE_START;
		}
		else
		{
		   g_TempRecipe.m_wUSLayflatOffsetSign = NEGATIVE_SIGN;
		   g_CurrentRecipe.m_wUSLayflatOffsetSign = NEGATIVE_SIGN;
	       g_nSaveRecipeSequence = SAVE_RECIPE_START;
		}
	}
	if(g_bWidthUltrasonicLayFlatOffsetWritten)
	{
		g_bWidthUltrasonicLayFlatOffsetWritten = FALSE;
	    fValue = (float)g_arrnMBTable[WIDTH_COMMAND_US_LAYFLAT_OFFSET];	//
	    if(g_fWidthConversionFactor != 1.0f)
//		    fValue = (fValue / 100.0f) / g_fWidthConversionFactor;  //nbb--todo-- check this again
	    	fValue = (fValue / 10.0f) / g_fWidthConversionFactor;  //nbb--todo-- check this again,  assume entry in 10ths of inches.
	    g_TempRecipe.m_nFileNumber = RUNNINGRECIPEFILENO;        // Set the downloaded file number = 1 (can be changed by operator and saved to any file)
	    g_TempRecipe.m_fDesiredWidthOffset = fValue; // Width is in mm.
	    g_CurrentRecipe.m_fDesiredWidthOffset = fValue; // Width is in mm.
	    g_nSaveRecipeSequence = SAVE_RECIPE_START;
	}
	if(g_bWidthUltrasonicDiameterFromLayFlatEntry)
	{
		float fLayflatmm;
		g_bWidthUltrasonicDiameterFromLayFlatEntry = FALSE;
		wLayflat = g_arrnMBTable[WIDTH_COMMAND_US_DIAMETER_FROM_LAYFLAT_ENTRY];
		//mmk wfh 21.4.2020 - account for English units.
		if(g_fWidthConversionFactor != 1.0f)	//if not metric...
		{
			float fLayflatInches = (float)wLayflat / 10;	//In English Units,  resolution = 1dp.
			float fLayflatmm = fLayflatInches / g_fWidthConversionFactor;
			fDiameter = 2* ((float)fLayflatmm) / (PI);
		}
		else //metric
		{
			fLayflatmm = (float)wLayflat;
			fDiameter = 2* ((float)wLayflat) / (PI);
		}

		wDiameter = (WORD)fDiameter;

 		CalculateDistanceToCentre(fDiameter);			//expecting diameter in mm.
 		//wfh
 		float fRadius = fDiameter/2;
 		CalculateSensorAngles( fRadius );			// calculate sensor angles,  need sensor-sensor distances and assumes sensors aligned on centre.

 		CalculateM2Offset(fLayflatmm);			// calculate layflat offset for M2 method. - will be in mm.
	}
	if(g_bWidthSetPointWritten)
	{
		g_bWidthSetPointWritten = FALSE;
		fValue =    (float)g_arrnMBTable[WIDTH_COMMAND_WIDTH_SETPOINT];
		if(g_fWidthConversionFactor != 1.0f)
		fValue = (fValue / PSUM_I_DECIMAL_PLACES_MULTIPLICATION) / g_fWidthConversionFactor; //nbb--todo--width review
		else
		fValue = (fValue / PSUM_M_DECIMAL_PLACES_MULTIPLICATION);

	    g_TempRecipe.m_fDesiredWidth = fValue; // Width is in mm.
	    g_CurrentRecipe.m_fDesiredWidth = fValue; //
	    g_TempRecipe.m_nFileNumber = RUNNINGRECIPEFILENO;        // Set the downloaded file number = 1 (can be changed by operator and saved to any file)
	    g_nSaveRecipeSequence = SAVE_RECIPE_START;
	}
}


//////////////////////////////////////////////////////
// CheckForStandardise
// sets g_bIsStandardising flag when standardising and clears
// when sensors reach the edge.
// a history flag is used to not do the test the very first time.
//
//////////////////////////////////////////////////////
void CheckForStandardise( void )
{
	BOOL bHasComeOutOfStandardise = FALSE;
	if((g_nSSIFInputState & SSIFRIGHTANDLEFTSTANDARDISEBITPOS) != 0)
	{
		g_bIsStandardising = TRUE;
		if(g_bIsStandardisingHistory)
		{
			if(g_bIsStandardising)
			{
				if((g_nLeftSensorStatus == SENSOR_AT_EDGE)&&(g_nRightSensorStatus == SENSOR_AT_EDGE))
				{
					g_bIsStandardising = FALSE;
					bHasComeOutOfStandardise = TRUE;
				}
			}
		}
	}
	else
	{
		if(g_bIsStandardising)
		{
			if((g_nLeftSensorStatus == SENSOR_AT_EDGE)&&(g_nRightSensorStatus == SENSOR_AT_EDGE))
			{
				g_bIsStandardising = FALSE;
				bHasComeOutOfStandardise = TRUE;
			}
		}

	}
	g_bIsStandardisingHistory = g_bIsStandardising;

	if(g_bIsStandardising)
	{
		g_arrnMBTable[WIDTH_SUMMARY_STANDARDISE_STATUS] = 0x01;
	}
	else
	{
		g_arrnMBTable[WIDTH_SUMMARY_STANDARDISE_STATUS] = 0x0;
	}
	if(bHasComeOutOfStandardise)
	{
		g_wStandardiseSettlePeriod = STANDARDISE_SETTLE_PERIOD;
	}

}




//////////////////////////////////////////////////////
// CopyWidthSummaryDataTOMB
//
//
//////////////////////////////////////////////////////
void CopyWidthSummaryDataTOMB( void )
{
	float	fValue;

	g_arrnMBTable[WIDTH_SUMMARY_WIDTH_OFFSET] =(WORD)(g_stWidthCalibration.m_fWidthOffset+0.5);

	fValue = g_fTotalScannerInstantaneousWidth;
	if(g_fWidthConversionFactor != 1.0f)
	{
		fValue *= g_fWidthConversionFactor;  //nbb--todo-- check this again
		fValue += (I_ROUND_DECIMAL_PLACES);                                                                          // for rounding to nearest  decimal digit.
		fValue *= (I_DECIMAL_PLACES_MULTIPLICATION);      // when not mm, inches to 2dp in modbus.
	}
	else
	{
	    fValue += (M_ROUND_DECIMAL_PLACES);                                                                          // for rounding to nearest  decimal digit.
		fValue *= (M_DECIMAL_PLACES_MULTIPLICATION);      // 1dp
	}
	g_arrnMBTable[WIDTH_SUMMARY_WIDTH_SCANNER] = (int)(fValue);
//	left width
	fValue =    g_fLeftWidth;
	if(g_fWidthConversionFactor != 1.0f)
	{
		fValue *= g_fWidthConversionFactor;  //nbb--todo-- check this again
		fValue += (I_ROUND_DECIMAL_PLACES);                                                                          // for rounding to nearest  decimal digit.
		fValue *= (I_DECIMAL_PLACES_MULTIPLICATION);      // when not mm, inches to 2dp in modbus.
	}
	else
	{
	    fValue += (M_ROUND_DECIMAL_PLACES);                                                                          // for rounding to nearest  decimal digit.
		fValue *= (M_DECIMAL_PLACES_MULTIPLICATION);      // 1dp
	}
	g_arrnMBTable[WIDTH_SUMMARY_LEFT_WIDTH] = (int)(fValue);                        // Width is in mm.
	//	right width
	fValue =    g_fRightWidth;
	if(g_fWidthConversionFactor != 1.0f)
	{
		fValue *= g_fWidthConversionFactor;  //nbb--todo-- check this again
		fValue += (I_ROUND_DECIMAL_PLACES);                                                                          // for rounding to nearest  decimal digit.
		fValue *= (I_DECIMAL_PLACES_MULTIPLICATION);      // when not mm, inches to 2dp in modbus.
	}
	else
	{
	    fValue += (M_ROUND_DECIMAL_PLACES);                                                                          // for rounding to nearest  decimal digit.
		fValue *= (M_DECIMAL_PLACES_MULTIPLICATION);      // 1dp
	}
	g_arrnMBTable[WIDTH_SUMMARY_RIGHT_WIDTH] = (WORD)(fValue);                        // Width is in mm.

	// control average width
	fValue =    g_fWidthControlAverage;
	if(g_fWidthConversionFactor != 1.0f)
	{
		fValue *= g_fWidthConversionFactor;  //nbb--todo-- check this again
		fValue += (I_ROUND_DECIMAL_PLACES);                                                                          // for rounding to nearest  decimal digit.
		fValue *= (I_DECIMAL_PLACES_MULTIPLICATION);      // when not mm, inches to 2dp in modbus.
	}
	else
	{
	    fValue += (M_ROUND_DECIMAL_PLACES);                                                                          // for rounding to nearest  decimal digit.
		fValue *= (M_DECIMAL_PLACES_MULTIPLICATION);      // 1dp
	}
	g_arrnMBTable[WIDTH_SUMMARY_CONTROL_AVERAGE_WIDTH] = (WORD)(fValue);                        // Width is in mm.

	// control average width error
	fValue =    g_fWidthControlAverageError;
	if(g_fWidthConversionFactor != 1.0f)
	{
		fValue *= g_fWidthConversionFactor;  //nbb--todo-- check this again
		fValue += (I_ROUND_DECIMAL_PLACES);                                                                          // for rounding to nearest  decimal digit.
		fValue *= (I_DECIMAL_PLACES_MULTIPLICATION);      // when not mm, inches to 2dp in modbus.
	}
	else
	{
	    fValue += (M_ROUND_DECIMAL_PLACES);                                                                          // for rounding to nearest  decimal digit.
		fValue *= (M_DECIMAL_PLACES_MULTIPLICATION);      // 1dp
	}
	g_arrnMBTable[WIDTH_SUMMARY_CONTROL_AVERAGE_ERROR] = (WORD)(fValue);                        // Width is in mm.

	// integrated width

	fValue =    g_fIntegratedWidth;
	if(g_fWidthConversionFactor != 1.0f)
	{
		fValue *= g_fWidthConversionFactor;  //nbb--todo-- check this again
		fValue += (I_ROUND_DECIMAL_PLACES);                                                                          // for rounding to nearest  decimal digit.
		fValue *= (I_DECIMAL_PLACES_MULTIPLICATION);      // when not mm, inches to 2dp in modbus.
	}
	else
	{
	    fValue += (M_ROUND_DECIMAL_PLACES);                                                                          // for rounding to nearest  decimal digit.
		fValue *= (M_DECIMAL_PLACES_MULTIPLICATION);      // 1dp
	}
	g_arrnMBTable[WIDTH_SUMMARY_WIDTH_INTEGRATION]= (WORD)(fValue);

	// left pulses

	g_arrnMBTable[WIDTH_SUMMARY_LEFT_PULSES] = (WORD)(g_nLeftMotorPulses);                        // Width is in mm.
	g_arrnMBTable[WIDTH_SUMMARY_RIGHT_PULSES] = (WORD)(g_nRightMotorPulses);                        // Width is in mm.
	if(g_bLeftLimitSwitchActive)
	{
		g_arrnMBTable[WIDTH_SUMMARY_LEFT_LIMIT] = 0x01;
	}
	else
	{
		g_arrnMBTable[WIDTH_SUMMARY_LEFT_LIMIT] = 0x0;
	}
	if(g_bRightLimitSwitchActive)
	{
		g_arrnMBTable[WIDTH_SUMMARY_RIGHT_LIMIT] = 0x01;
	}
	else
	{
		g_arrnMBTable[WIDTH_SUMMARY_RIGHT_LIMIT] = 0x0;
	}
	g_arrnMBTable[WIDTH_SUMMARY_LEFT_SENSOR_STATUS] = (WORD)g_nLeftSensorStatus;
	g_arrnMBTable[WIDTH_SUMMARY_RIGHT_SENSOR_STATUS] = (WORD)g_nRightSensorStatus;
// left sensor a/d
	g_arrnMBTable[WIDTH_SUMMARY_LEFT_SENSOR_ATD] = (WORD)g_nLeftSensorADC;
// right sensor a/d
	g_arrnMBTable[WIDTH_SUMMARY_RIGHT_SENSOR_ATD] = (WORD)g_nRightSensorADC;



// comms counters
	// good tx counter for SSIF
	g_arrnMBTable[WIDTH_SUMMARY_SSIF_TX_MESSAGES] = (WORD)((structSSIFData.m_lGoodTx >> 16) & 0xFFFF);  //ms
	g_arrnMBTable[WIDTH_SUMMARY_SSIF_TX_MESSAGES+1] =(WORD)(structSSIFData.m_lGoodTx & 0xFFFF);  //ls
	// good rx counter for SSIF
	g_arrnMBTable[WIDTH_SUMMARY_SSIF_RX_MESSAGES] = (WORD)((structSSIFData.m_lGoodRx >> 16) & 0xFFFF);  //ms
	g_arrnMBTable[WIDTH_SUMMARY_SSIF_RX_MESSAGES+1] =(WORD)(structSSIFData.m_lGoodRx & 0xFFFF);  //ls
	// checksum errors
	g_arrnMBTable[WIDTH_SUMMARY_SSIF_RX_CHECKSUM_ERRORS] = (WORD)((structSSIFData.m_lChecksumErrorCounter >> 16) & 0xFFFF);  //ms
	g_arrnMBTable[WIDTH_SUMMARY_SSIF_RX_CHECKSUM_ERRORS+1] =(WORD)(structSSIFData.m_lChecksumErrorCounter & 0xFFFF);  //ls
	// timeouts for SSIF
	g_arrnMBTable[WIDTH_SUMMARY_SSIF_RX_TIMEOUTS] = (WORD)((structSSIFData.m_lRxTimeoutCounter >> 16) & 0xFFFF);  //ms
	g_arrnMBTable[WIDTH_SUMMARY_SSIF_RX_TIMEOUTS+1] =(WORD)(structSSIFData.m_lRxTimeoutCounter & 0xFFFF);  //ls
	if(g_bWidthAuto)
	{
		g_arrnMBTable[WIDTH_SUMMARY_AUTO_STATUS] = 0X01;
	}
	else
	{
		g_arrnMBTable[WIDTH_SUMMARY_AUTO_STATUS] = 0X0;
	}
	if(g_bWidthIncreaseIsOn)
	{
		g_arrnMBTable[WIDTH_SUMMARY_INCREASE_STATUS] = 0X01;
	}
	else
	{
		g_arrnMBTable[WIDTH_SUMMARY_INCREASE_STATUS] = 0X0;
	}
	if(g_bWidthDecreaseIsOn)
	{
		g_arrnMBTable[WIDTH_SUMMARY_DECREASE_STATUS] = 0X01;
	}
	else
	{
		g_arrnMBTable[WIDTH_SUMMARY_DECREASE_STATUS] = 0X0;
	}

	if(g_bWidthFastCorrectionIncreaseIsOn)
	{
		g_arrnMBTable[WIDTH_SUMMARY_FAST_CORRECTION_STATUS] = 0X01;
	}
	else
	{
		g_arrnMBTable[WIDTH_SUMMARY_FAST_CORRECTION_STATUS] = 0X0;
	}
	g_arrnMBTable[WIDTH_SUMMARY_WIDTH_ERROR]= 0;// nbb--todo--width
	g_arrnMBTable[WIDTH_SUMMARY_CONTROL_INCREASE_CORRECTION]= (WORD)g_nWidthIncreaseOnCtrCalculatedValue;
	g_arrnMBTable[WIDTH_SUMMARY_CONTROL_FCINCREASE_CORRECTION]= (WORD)g_nWidthFCIncreaseOnCtrCalculatedValue;
	g_arrnMBTable[WIDTH_SUMMARY_CONTROL_DECREASE_CORRECTION]= (WORD)g_nWidthDecreaseOnCtrCalculatedValue;
	g_arrnMBTable[WIDTH_SUMMARY_CONTROL_INCREASE_COUNTER]= (WORD)g_nWidthIncreaseOnCtr;
	g_arrnMBTable[WIDTH_SUMMARY_CONTROL_FCINCREASE_COUNTER]= (WORD)g_nWidthFastCorrectionIncreaseOnCtr;

	g_arrnMBTable[WIDTH_SUMMARY_CONTROL_DECREASE_COUNTER]= (WORD)g_nWidthDecreaseOnCtr;
	g_arrnMBTable[WIDTH_SUMMARY_CONTROL_PROCESS_DELAY_VALUE]= (WORD)g_nWidthProcessDelayCtrCalculatedValue;
	g_arrnMBTable[WIDTH_SUMMARY_CONTROL_PROCESS_DELAY_CTR]= (WORD)g_nWidthProcessDelayCtr;
	if(g_bWidthIsInStartUp)
	g_arrnMBTable[WIDTH_SUMMARY_START_UP]= 0x01;
	else
	g_arrnMBTable[WIDTH_SUMMARY_START_UP]= 0x0;

	// alarm status copy
	if(g_bWidthAlarmIsActive)
	g_arrnMBTable[WIDTH_SUMMARY_ALARM_STATUS]= 0x01;
	else
	g_arrnMBTable[WIDTH_SUMMARY_ALARM_STATUS]= 0x00;


    fValue = g_fActualWidth;
    if(g_fWidthConversionFactor != 1.0f)
    {
         fValue *= g_fWidthConversionFactor;  //nbb--todo-- check this again
         fValue *= (I_DECIMAL_PLACES_MULTIPLICATION);      // when not mm, inches to 2dp in modbus.
    }
    else
    {
        fValue += 0.5;                                                                          // for rounding to nearest  decimal digit.
    }
    g_arrnMBTable[BATCH_SUMMARY_WIDTH] = (WORD)fValue;

    fValue = g_fActualWidth;
	if(g_fWidthConversionFactor != 1.0f)
	{
		fValue *= g_fWidthConversionFactor;  //nbb--todo-- check this again
		fValue += (PSUM_I_ROUND_DECIMAL_PLACES);                                                                          // for rounding to nearest  decimal digit.
		fValue *= (PSUM_I_DECIMAL_PLACES_MULTIPLICATION);      // when not mm, inches to 2dp in modbus.
	}
	else
	{
	    fValue += (PSUM_M_ROUND_DECIMAL_PLACES);                                                                          // for rounding to nearest  decimal digit.
		fValue *= (PSUM_M_DECIMAL_PLACES_MULTIPLICATION);      // 1dp
	}
    g_arrnMBTable[WIDTH_SUMMARY_WIDTH] = (WORD)fValue;

// average width.
    fValue = g_fAveragedWidth;
	if(g_fWidthConversionFactor != 1.0f)
	{
		fValue *= g_fWidthConversionFactor;  //nbb--todo-- check this again
		fValue += (I_ROUND_TO_ONE_DECIMAL_PLACE);                                                                          // for rounding to nearest  decimal digit.
		fValue *= (PSUM_I_ONE_DECIMAL_PLACES_MULTIPLICATION);      // when not mm, inches to 2dp in modbus.
	}
	else
	{
	    fValue += (PSUM_M_ROUND_DECIMAL_PLACES);                                                                          // for rounding to nearest  decimal digit.
		fValue *= (PSUM_M_DECIMAL_PLACES_MULTIPLICATION);      // 1dp
	}
    g_arrnMBTable[WIDTH_SUMMARY_AVERAGE_WIDTH] = (WORD)fValue;


    if(g_bBubbleBreakActive)
    g_arrnMBTable[WIDTH_SUMMARY_BUBBLE_BREAK_STATUS] = 0x01;
    else
    g_arrnMBTable[WIDTH_SUMMARY_BUBBLE_BREAK_STATUS] = 0x0;

	CopyUltrasonicSummaryDataTOMB();

	CopyCalculatedSensorAnglesToMB();
}


//////////////////////////////////////////////////////
// CopyUltrasonicSummaryDataTOMB
//
//
//////////////////////////////////////////////////////
void CopyUltrasonicSummaryDataTOMB( void )
{
	unsigned int i;
	float fValue;
	// instantaneous counts
	for(i=0; i < NUMBER_OF_ULTRASONICS_SENSORS; i++)
	{
		// ultrasonic echo time
	    g_arrnMBTable[WIDTH_SUMMARY_US_ECHO_TIME_1+(i*2)]	= (g_lUltrasonicEchoTime[i] >> 16) & 0xFFFF;
	    g_arrnMBTable[WIDTH_SUMMARY_US_ECHO_TIME_1+1+(i*2)]	= g_lUltrasonicEchoTime[i] & 0xFFFF;
	    // ultrasonic echo time average
	    g_arrnMBTable[WIDTH_SUMMARY_US_AVERAGE_ECHO_TIME_1+(i*2)]	= (g_lUltrasonicEchoTimeAverage[i] >> 16) & 0xFFFF;
	    g_arrnMBTable[WIDTH_SUMMARY_US_AVERAGE_ECHO_TIME_1+1+(i*2)]	= g_lUltrasonicEchoTimeAverage[i] & 0xFFFF;

	    fValue = g_fUSSensorDistance[i];
        if(g_fWidthConversionFactor != 1.0f)
        {
             fValue *= g_fWidthConversionFactor;  //nbb--todo-- check this again
             fValue *= (I_DECIMAL_PLACES_MULTIPLICATION);      // when not mm, inches to 2dp in modbus.
        }
        else
        {
            fValue += (M_ROUND_DECIMAL_PLACES);                                                                          // for rounding to nearest  decimal digit.
            fValue *= (M_DECIMAL_PLACES_MULTIPLICATION);      // mm - 1 decimal place.
        }
	    g_arrnMBTable[WIDTH_SUMMARY_US_DISTANCE_1+i] = (WORD)fValue;
	}
	for(i=0; i < NUMBER_OF_ULTRASONICS_SENSORS-1; i++)
	{
	    fValue = g_fUSSensorRadius[i];
        if(g_fWidthConversionFactor != 1.0f)
        {
             fValue *= g_fWidthConversionFactor;  //nbb--todo-- check this again
             fValue *= (I_DECIMAL_PLACES_MULTIPLICATION);      // when not mm, inches to 2dp in modbus.
        }
        else
        {
            fValue += (M_ROUND_DECIMAL_PLACES);                                                                          // for rounding to nearest  decimal digit.
            fValue *= (M_DECIMAL_PLACES_MULTIPLICATION);      // mm - 1 decimal place.
        }
	    g_arrnMBTable[WIDTH_SUMMARY_US_RADIUS_1+i] = (WORD)fValue;

	    // distance to centre
	    fValue = g_stWidthCalibration.m_fDistanceToCentre[i];
        if(g_fWidthConversionFactor != 1.0f)
        {
             fValue *= g_fWidthConversionFactor;  //nbb--todo-- check this again
             fValue *= (I_DECIMAL_PLACES_MULTIPLICATION);      // when not mm, inches to 2dp in modbus.
        }
        else
        {
            fValue += (M_ROUND_DECIMAL_PLACES);                                                                          // for rounding to nearest  decimal digit.
            fValue *= (M_DECIMAL_PLACES_MULTIPLICATION);      // mm - 1 decimal place.
        }
	    g_arrnMBTable[WIDTH_SUMMARY_US_DISTANCE_TO_CENTRE_1+i] = (WORD)fValue;

	    g_arrnMBTable[WIDTH_SUMMARY_US_SENSOR_OK_1+i] = g_bUSSensorOK[i];

//	    g_arrnMBTable[WIDTH_SUMMARY_US_SENSOR4_TRIGS] = g_nUSSensorTriggeredCtr3;
//	    g_arrnMBTable[WIDTH_SUMMARY_US_SENSOR4_ECHOS] = g_nUSSensorEchosReceived3;
//	    g_arrnMBTable[WIDTH_SUMMARY_US_SENSOR_OK_4] = g_bUSSensorOK[3];

	}

    fValue = g_fUSSensorAverageRadius;
    if(g_fWidthConversionFactor != 1.0f)
    {
         fValue *= g_fWidthConversionFactor;  //nbb--todo-- check this again
         fValue *= (I_DECIMAL_PLACES_MULTIPLICATION);      // when not mm, inches to 2dp in modbus.
    }
    else
    {
        fValue += (M_ROUND_DECIMAL_PLACES);                                                                          // for rounding to nearest  decimal digit.
        fValue *= (M_DECIMAL_PLACES_MULTIPLICATION);      // mm - 1 decimal place.
    }
    g_arrnMBTable[WIDTH_SUMMARY_US_AVERAGE_RADIUS] = (WORD)fValue;

    fValue = g_fUSSensorAverageDiameter;
     if(g_fWidthConversionFactor != 1.0f)
     {
          fValue *= g_fWidthConversionFactor;  //nbb--todo-- check this again
          fValue *= (I_DECIMAL_PLACES_MULTIPLICATION);      // when not mm, inches to 2dp in modbus.
     }
     else
     {
         fValue += (M_ROUND_DECIMAL_PLACES);                                                                          // for rounding to nearest  decimal digit.
         fValue *= (M_DECIMAL_PLACES_MULTIPLICATION);      // mm - 1 decimal place.
     }
     g_arrnMBTable[WIDTH_SUMMARY_US_DIAMETER] = (WORD)fValue;


      fValue = g_fUSSensorAverageDiameterWithOffset;
      if(g_fWidthConversionFactor != 1.0f)
      {
           fValue *= g_fWidthConversionFactor;  //nbb--todo-- check this again
           fValue *= (I_DECIMAL_PLACES_MULTIPLICATION);      // when not mm, inches to 2dp in modbus.
      }
      else
      {
          fValue += (M_ROUND_DECIMAL_PLACES);                                                                          // for rounding to nearest  decimal digit.
          fValue *= (M_DECIMAL_PLACES_MULTIPLICATION);      // mm - 1 decimal place.
      }
      g_arrnMBTable[WIDTH_SUMMARY_US_DIAMETER_WITH_OFFSET] = (WORD)fValue;

      fValue = g_fUSSensorAverageLayFlatWidth;
      if(g_fWidthConversionFactor != 1.0f)
      {
           fValue *= g_fWidthConversionFactor;  //nbb--todo-- check this again
           fValue *= (I_DECIMAL_PLACES_MULTIPLICATION);      // when not mm, inches to 2dp in modbus.
      }
      else
      {
          fValue += (M_ROUND_DECIMAL_PLACES);                                                                          // for rounding to nearest  decimal digit.
          fValue *= (M_DECIMAL_PLACES_MULTIPLICATION);      // mm - 1 decimal place.
      }
      g_arrnMBTable[WIDTH_SUMMARY_US_LAYFLAT_WIDTH_] = (WORD)fValue;

      fValue = g_fUSSensorAverageLayFlatWidthWithOffset;
       if(g_fWidthConversionFactor != 1.0f)
       {
            fValue *= g_fWidthConversionFactor;  //nbb--todo-- check this again
            fValue *= (I_DECIMAL_PLACES_MULTIPLICATION);      // when not mm, inches to 2dp in modbus.
       }
       else
       {
           fValue += (M_ROUND_DECIMAL_PLACES);                                                                          // for rounding to nearest  decimal digit.
           fValue *= (M_DECIMAL_PLACES_MULTIPLICATION);      // mm - 1 decimal place.
       }
       g_arrnMBTable[WIDTH_SUMMARY_US_LAYFLAT_WIDTH_WITH_OFFSET] = (WORD)fValue;

       g_arrnMBTable[WIDTH_SUMMARY_US_COUNTS_PER_MM] = g_stWidthCalibration.m_fUltrasonicMMConversionFactor * (M_DECIMAL_PLACES_MULTIPLICATION); //nbb--todo--width-- need imperial conversion here
}
