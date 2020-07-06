///////////////////////////////////////////////////////////
// P.Smith						7/7/2010
// added CheckForValidWidthConfigData to check for valid config data
//
// P.Smith						9/7/2010
// check for m_bStandardiseOnAuto,m_bStandardiseOnTheHour,m_bAlarmInManual
//
// P.Smith						16/9/2010
// added check on w_USCalibrate_Layflat_From_US_Interval,w_USCalibrateLayflatFromUSSampleNo
// m_bFastCorrectionEnabled,b_USReferenceAutoCalibration,b_USCalibrate_layflat_from_US

// M.McKiernan					26.6.2020.
// Added new checks for new width parameters. void CheckForValidWidthConfigData( void ).
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
#include "BatVars2.h"
#include "EEPROMdriver.h"
#include "CsumCalc.h"
#include "WidthRWToNonVolatileMemory.h"
#include "Alarms.h"
#include "WidthMB.h"

extern CalDataStruct	g_CalibrationData;

// Locally declared global variables


/* ====================================================================
Function:     LoadWidthCalibrationFromEE
Parameters:   N/A
Returns:      N/A

======================================================================= */

int LoadWidthCalibrationFromEE( void )
{

    int nResult = 0;
    int nChipSelectUsed,nSize;
    unsigned char    g_arrcTemp[sizeof(g_stWidthCalibration)];

    int     i,nBytes;
    char    *pPtr;
    pPtr = (char*)&g_stWidthCalibration;
    g_bSPIEEActive = TRUE;                // indicate eeprom is active.
    nChipSelectUsed = g_nEEPROMSPIChipSelect;    // for NBB.
    SPI_Select_EEPROM();
    UnProtectEEPROM( nChipSelectUsed );    // enable writes to eeprom (all).

    // Write out the data
    nBytes = sizeof( g_stWidthCalibration );

    for( i = 0; i < nBytes; i++ )
    {
        *(pPtr + i) =  EEPROMRead1Byte( nChipSelectUsed, WIDTH_CONTROL_CALIBRATION_DATA_START + i );
    }

    memcpy(&g_arrcTemp, &g_stWidthCalibration, sizeof(g_stWidthCalibration));    // copy structure to array.
    nSize = sizeof(g_stWidthCalibration)- sizeof(g_stWidthCalibration.m_nChecksum);
    g_unCalibrationCheckSum = CalculateCheckSum(g_arrcTemp, nSize);

    if(g_unCalibrationCheckSum != g_stWidthCalibration.m_nChecksum)
    {
    //nbb--todo--vac add to ram    g_bLoadCalibrationDataInvalid = TRUE;
    }
    else
    {
    //nbb--todo--vac add to ram     g_bLoadCalibrationDataInvalid = FALSE;
    }

    g_bSPIEEActive = FALSE;                // indicate eeprom is inactive.
    return( nResult );

}




/* ====================================================================
Function:     SaveWidthControlCalibrationToEE
Parameters:   N/A
Returns:      N/A

======================================================================= */

void SaveWidthControlCalibrationToEE( void )
{
    int     i,nBytes,nSize;
    WORD  nChipSelectUsed;
    char    *pPtr;
    BOOL    bOkay = FALSE;
    BOOL    bEEPROMCalibrationWriteError = FALSE;

    unsigned char    g_arrcTemp[sizeof(g_stWidthCalibration)];
    unsigned char   cData;

    g_bSPIEEActive = TRUE;                // indicate eeprom is active.

    if( (g_arrnMBTable[BATCH_SUMMARY_PROCESS_ALARMS] & CALINVALARMBIT) ) /* tare alarm bit set???  */
    {
        RemoveAlarmTable( CALINVALARM,  0 );      /* indicate alarm cleared, RMALTB */
    }

    pPtr = (char*)&g_stWidthCalibration;
    nSize = sizeof(g_stWidthCalibration)- sizeof(g_stWidthCalibration.m_nChecksum);
    // merge equivalent of CALCULATEEEPROMCHECKSUM inline
    memcpy(&g_arrcTemp, &g_stWidthCalibration,nSize );    // copy structure to array.

    g_stWidthCalibration.m_nChecksum = CalculateCheckSum(g_arrcTemp, nSize);
    // Write out the data
    nBytes = sizeof( g_stWidthCalibration );

    nChipSelectUsed = g_nEEPROMSPIChipSelect;    // for NBB.
    SPI_Select_EEPROM();
    Remove_EEPROM_Write_Protect();
    UnProtectEEPROM( nChipSelectUsed );    //
//    iprintf("\n No.5 g_bSPIAtDActive is  %d g_bSPIEEActive is %d pits are %d",g_bSPIAtDActive,g_bSPIEEActive,g_nPIT_Int_CounterU);  //nbb--testonly--

    for( i = 0; i < nBytes ; i++ )
      {
          cData = *(pPtr + i);
          bOkay = EEPROMWrite1Byte(nChipSelectUsed,  cData, WIDTH_CONTROL_CALIBRATION_DATA_START + i);      //
          if(!bOkay)
          {
              bEEPROMCalibrationWriteError = TRUE;
          }
      }
    g_bSPIEEActive = FALSE;
    if(bEEPROMCalibrationWriteError)
    {
        PutAlarmTable( CALIBRATIONWRITEALARM,  0 );
    }
    else
    {
       RemoveAlarmTable( CALIBRATIONWRITEALARM,  0 );
    }
     Set_EEPROM_Write_Protect();
}



/* ====================================================================
Function:     CheckForValidWidthConfigData
Parameters:   N/A
Returns:      N/A

======================================================================= */

void CheckForValidWidthConfigData( void )
{
	BOOL bInvalidData = FALSE;
	// reference auto cal
	if(((g_stWidthCalibration.b_USReferenceAutoCalibration)!= TRUE) && (g_stWidthCalibration.b_USReferenceAutoCalibration != FALSE))
	{
	    g_stWidthCalibration.b_USReferenceAutoCalibration = FALSE;
	    bInvalidData = TRUE;
    }
	// width type
	if(g_stWidthCalibration.m_wWidthType > LAST_WIDTH_TYPE)
    {
		g_stWidthCalibration.m_wWidthType = STANDARD_TYPE;
        bInvalidData = TRUE;
    }
	// width mode
	if(g_stWidthCalibration.m_wWidthMode > LAST_WIDTH_MODE)
    {
		g_stWidthCalibration.m_wWidthMode = LAYFLAT_MODE;
        bInvalidData = TRUE;
    }
	// line type  standard blown film,ibc,cryovac
	if(g_stWidthCalibration.m_wLineType > LAST_LINE_TYPE)
    {
		g_stWidthCalibration.m_wLineType = BLOWN_FILM_LINE_TYPE;
        bInvalidData = TRUE;
    }
	// alarm limit
	if((g_stWidthCalibration.m_wAlarmLimit == 0) || (g_stWidthCalibration.m_wAlarmLimit > MAX_ALARM_LIMIT))
	{
		g_stWidthCalibration.m_wAlarmLimit = DEFAULT_ALARM_LIMIT;
        bInvalidData = TRUE;
	}
	// process delay
	if((g_stWidthCalibration.m_wProcessDelayInSeconds == 0) || (g_stWidthCalibration.m_wProcessDelayInSeconds > MAX_PROCESS_DELAY))
	{
		g_stWidthCalibration.m_wProcessDelayInSeconds = DEFAULT_PROCESS_DELAY;
        bInvalidData = TRUE;
	}

	// integration time
	if((g_stWidthCalibration.m_wWidthIntegrationTimeinSeconds == 0) || (g_stWidthCalibration.m_wWidthIntegrationTimeinSeconds > MAX_INTEGRATION_TIME))
	{
		g_stWidthCalibration.m_wWidthIntegrationTimeinSeconds = DEFAULT_INTEGRATION_TIME;
        bInvalidData = TRUE;
	}

	//width deadband
	if(g_stWidthCalibration.m_wWidthDeadband > MAX_WIDTH_DEADBAND)
    {
		g_stWidthCalibration.m_wWidthDeadband = DEFAULT_WIDTH_DEADBAND;
        bInvalidData = TRUE;
    }

	if((g_stWidthCalibration.m_fNormalIncreaseControlGainPercentage < 0.01)||(g_stWidthCalibration.m_fNormalIncreaseControlGainPercentage > MAX_INCREASE_CONTROL_GAIN))
	{
		g_stWidthCalibration.m_fNormalIncreaseControlGainPercentage = DEFAULT_INCREASE_CONTROL_GAIN;
        bInvalidData = TRUE;
	}


	if((g_stWidthCalibration.m_fNormalDecreaseControlGainPercentage < 0.01)||(g_stWidthCalibration.m_fNormalDecreaseControlGainPercentage > MAX_DECREASE_CONTROL_GAIN))
	{
		g_stWidthCalibration.m_fNormalDecreaseControlGainPercentage = DEFAULT_DECREASE_CONTROL_GAIN;
        bInvalidData = TRUE;
	}

	if((g_stWidthCalibration.m_fFastCorrectedControlGainPercentage < 0.01)||(g_stWidthCalibration.m_fFastCorrectedControlGainPercentage > MAX_FAST_CORRECTION_CONTROL_GAIN))
	{
		g_stWidthCalibration.m_fFastCorrectedControlGainPercentage = DEFAULT_FAST_CORRECTION_CONTROL_GAIN;
        bInvalidData = TRUE;
	}
	if((g_stWidthCalibration.m_wWidthAveragingInSeconds == 0) || (g_stWidthCalibration.m_wWidthAveragingInSeconds > MAX_WIDTH_AVERAGING_IN_SECONDS))
	{
		g_stWidthCalibration.m_wWidthAveragingInSeconds = DEFAULT_WIDTH_AVERAGING_IN_SECONDS;
        bInvalidData = TRUE;
	}
	if((g_stWidthCalibration.m_fWidthOffset < 0.0f)||(g_stWidthCalibration.m_fWidthOffset >10000.0))
	{
		g_stWidthCalibration.m_fWidthOffset = 0.0f;
        bInvalidData = TRUE;
	}
	if((g_stWidthCalibration.m_wUltrasonicAveraging == 0) || (g_stWidthCalibration.m_wUltrasonicAveraging > MAX_ULTRASONIC_AVERAGING))
	{
		g_stWidthCalibration.m_wUltrasonicAveraging = DEFAULT_ULTRASONIC_AVERAGING;
        bInvalidData = TRUE;
	}
	// over run
	if(g_stWidthCalibration.m_wWidthOverrun > MAX_WIDTH_OVERRUN)
    {
		g_stWidthCalibration.m_wWidthOverrun = DEFAULT_WIDTH_OVERRUN;
        bInvalidData = TRUE;
    }
	// over run deadband
	if(g_stWidthCalibration.m_wWidthOverrunDeadband > MAX_WIDTH_OVERRUN_DEADBAND)
    {
		g_stWidthCalibration.m_wWidthOverrunDeadband = DEFAULT_WIDTH_OVERRUN_DEADBAND;
        bInvalidData = TRUE;
    }
	if(g_stWidthCalibration.w_USCalibrate_Layflat_From_US_Interval > MAX_CAL_LAYFLAT_FROM_US_INTERVAL)
    {
		g_stWidthCalibration.w_USCalibrate_Layflat_From_US_Interval = DEFAULT_CAL_LAYFLAT_FROM_US_INTERVAL;
        bInvalidData = TRUE;
    }
	if(g_stWidthCalibration.w_USCalibrateLayflatFromUSSampleNo > MAX_CAL_LAYFLAT_FROM_US_SAMPLE_NO)
    {
		g_stWidthCalibration.w_USCalibrateLayflatFromUSSampleNo = DEFAULT_CAL_LAYFLAT_FROM_US_SAMPLE_NO;
        bInvalidData = TRUE;
    }

	//bools checked here
	if(((g_stWidthCalibration.m_bStandardiseOnAuto)!= TRUE) && (g_stWidthCalibration.m_bStandardiseOnAuto != FALSE))
	{
	    g_stWidthCalibration.m_bStandardiseOnAuto = FALSE;
	    bInvalidData = TRUE;
    }
	if(((g_stWidthCalibration.m_bStandardiseOnTheHour)!= TRUE) && (g_stWidthCalibration.m_bStandardiseOnTheHour != FALSE))
	{
	    g_stWidthCalibration.m_bStandardiseOnTheHour = FALSE;
	    bInvalidData = TRUE;
    }
	if(((g_stWidthCalibration.m_bAlarmInManual)!= TRUE) && (g_stWidthCalibration.m_bAlarmInManual != FALSE))
	{
	    g_stWidthCalibration.m_bAlarmInManual = FALSE;
		bInvalidData = TRUE;
	}
	// fast correction enable
	if(((g_stWidthCalibration.m_bFastCorrectionEnabled)!= TRUE) && (g_stWidthCalibration.m_bFastCorrectionEnabled != FALSE))
	{
	    g_stWidthCalibration.m_bFastCorrectionEnabled = TRUE;  // default is enabled
		bInvalidData = TRUE;
	}
	if(((g_stWidthCalibration.b_USReferenceAutoCalibration)!= TRUE) && (g_stWidthCalibration.b_USReferenceAutoCalibration != FALSE))
	{
	    g_stWidthCalibration.b_USReferenceAutoCalibration = FALSE;  // default is enabled
		bInvalidData = TRUE;
	}
	if(((g_stWidthCalibration.b_USCalibrate_layflat_from_US)!= TRUE) && (g_stWidthCalibration.b_USCalibrate_layflat_from_US != FALSE))
	{
	    g_stWidthCalibration.b_USCalibrate_layflat_from_US = FALSE;  // default is enabled
		bInvalidData = TRUE;
	}

// wfh 26.6.2020 ---  Check for newly added parameters.
	//valid lenghts between 0.3 and 5m.
//AB
	if( (g_stWidthCalibration.m_fLength_AB > 300.0f) && (g_stWidthCalibration.m_fLength_AB < 5000.0f) )
			;
	else
	{
		g_stWidthCalibration.m_fLength_AB = 777.7f;			// default to 777
		bInvalidData = TRUE;
	}
//BC
	if( (g_stWidthCalibration.m_fLength_BC > 300.0f) && (g_stWidthCalibration.m_fLength_BC < 5000.0f) )
		;
	else
	{
		g_stWidthCalibration.m_fLength_BC = 777.7f;			// default to 777
		bInvalidData = TRUE;
	}
//AC
	if( (g_stWidthCalibration.m_fLength_AC > 300.0f) && (g_stWidthCalibration.m_fLength_AC < 5000.0f) )
		;
	else
	{
		g_stWidthCalibration.m_fLength_AC = 777.7f;			// default to 777
		bInvalidData = TRUE;
	}
//ref.	 expect between 0.1 and 1m.
	if( (g_stWidthCalibration.m_fLength_Ref > 99.9f) && (g_stWidthCalibration.m_fLength_Ref < 1000.0f) )
		;
	else
	{
		g_stWidthCalibration.m_fLength_Ref = 399.0f;			// default to 399.
		bInvalidData = TRUE;
	}
// sensor Angles
// Sensor A.   expecting 30.  accept 15-45
	if( (g_stWidthCalibration.m_fSensorAngleA > 15.0f) && (g_stWidthCalibration.m_fSensorAngleA < 45.0f) )
			;
	else
	{
		g_stWidthCalibration.m_fSensorAngleA = 30.0f;			// default to 30
		bInvalidData = TRUE;
	}
// Sensor B   expecting 270    accept 250-290.
	if( (g_stWidthCalibration.m_fSensorAngleB > 250.0f) && (g_stWidthCalibration.m_fSensorAngleB < 290.0f) )
			;
	else
	{
		g_stWidthCalibration.m_fSensorAngleB = 270.0f;			// default to 270
		bInvalidData = TRUE;
	}
// sensor C   expecting 150    accept 120=170.
	if( (g_stWidthCalibration.m_fSensorAngleC > 120.0f) && (g_stWidthCalibration.m_fSensorAngleC < 170.0f) )
			;
	else
	{
		g_stWidthCalibration.m_fSensorAngleC = 150.0f;			// default to 150
		bInvalidData = TRUE;
	}

// Ultrasonic Measurement method. 1 or 2.
	if(  (g_stWidthCalibration.m_nUSMethod != US_METHOD_1) && (g_stWidthCalibration.m_nUSMethod != US_METHOD_2) )
	{
	    g_stWidthCalibration.m_nUSMethod =  US_METHOD_1;  // default is method 1.
		bInvalidData = TRUE;
	}

// Method 2 M2 width offset. expecting 0 to 99mm
	if( (g_stWidthCalibration.m_fM2WidthOffset >= 0.0f) && (g_stWidthCalibration.m_fM2WidthOffset < 99.0f) )
			;
	else
	{
		g_stWidthCalibration.m_fM2WidthOffset = 0.0f;			// default to 0
		bInvalidData = TRUE;
	}

// M2 width offset sign.
	if(  (g_stWidthCalibration.m_nM2WidthOffsetSign != POSITIVE_SIGN) && (g_stWidthCalibration.m_nM2WidthOffsetSign != NEGATIVE_SIGN) )
	{
	    g_stWidthCalibration.m_nM2WidthOffsetSign = POSITIVE_SIGN;  // default is +ve.
		bInvalidData = TRUE;
	}

	//       float		m_fLength_AB;				// distance between sensors A(1) and B(2)
	//       float		m_fLength_BC;				// distance between sensors B(2) and C(3)
	//       float		m_fLength_AC;				// distance between sensors A(1) and C(3)
	//       float		m_fLength_Ref;				// Reference Length
	//       float		m_fLayflatCalibrateWidth;	// Layflat width for calibration purposes.
	//       float		m_fTargetCalibrateDiameter;	// Diameter of a cylindrical target for calibration purposes.
	//        float		m_fSensorAngleA;			// angle of sesor A wrt horizontal.
	//       float		m_fSensorAngleB;			// angle of sesor B wrt horizontal.
	//       float		m_fSensorAngleC;			// angle of sesor C wrt horizontal.
	//       BYTE 		m_nUSMethod;				// Ultrasonic measurement method.
	//       float		m_fM2WidthOffset;			// Width offset for M2 measurement
	//       BYTE			m_nM2WidthOffsetSign;		// sign of of offset


	if(bInvalidData)
	{
		SaveWidthControlCalibrationToEE();
	}
}
