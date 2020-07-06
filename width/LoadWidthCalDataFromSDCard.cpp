// P.Smith                          7/7/10
// load distance to centre and counts per mm from sd card.
//
// M.McKiernan						5/6/2020
// Load all width calibration parameters from SD card.
// // wfh
// Ultrasonic Averaging
//g_UploadedWidthCalibrationData.m_wUltrasonicAveraging = i;
// Width Overrun
//g_UploadedWidthCalibrationData.m_wWidthOverrun = i;
// 	Width Overrun deadband.
//g_UploadedWidthCalibrationData.m_wWidthOverrunDeadband = i;
// Standardise on Auto.
//g_UploadedWidthCalibrationData.m_bStandardiseOnAuto = i;
// Standardise on Hour.
//g_UploadedWidthCalibrationData.m_bStandardiseOnTheHour = i;
// Alarm in Manual.
//g_UploadedWidthCalibrationData.m_bAlarmInManual = i;
// Fast Correction enabled.
//g_UploadedWidthCalibrationData.m_bFastCorrectionEnabled = i;
// Length AB
//g_UploadedWidthCalibrationData.m_fLength_AB = d;
// Length BC
//g_UploadedWidthCalibrationData.m_fLength_BC = d;
// Length AC
//g_UploadedWidthCalibrationData.m_fLength_AC = d;
// Sensor angle A
//g_UploadedWidthCalibrationData.m_fSensorAngleA = d;
// Sensor angle B
//g_UploadedWidthCalibrationData.m_fSensorAngleB = d;
// Sensor angle C
//g_UploadedWidthCalibrationData.m_fSensorAngleC = d;
// M2 Width Offset
//g_UploadedWidthCalibrationData.m_fM2WidthOffset = d;
// M2 Width Offset sign.
// g_UploadedWidthCalibrationData.m_nM2WidthOffsetSign = i;
// - if a valid reference length was uploaded,  set flat to cause reference to be calibrated.
//		if( (g_stWidthCalibration.m_wUltrasonicMMCalibratedAt > 100) && ( g_stWidthCalibration.m_wUltrasonicMMCalibratedAt < 500 ) )
//			g_bUploadedRefLength = TRUE;
#include "predef.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <basictypes.h>
//#include <unistd.h>
#include <effs_fat/fat.h>
#include <effs_fat/mmc_mcf.h>
#include <effs_fat/cfc_mcf.h>

#include "iniparser.h"
#include "iniexample.h"

#include <effs_fat/fat.h>
#include <math.h>
#include "BatchCalibrationFunctions.h"
#include "Batvars.h"
#include "Batvars2.h"
#include "Sdcard.h"
#include "VacVars.h"
#include "WidthCalibrationDefinitions.h"
#include "WidthMB.h"
#include "WidthVars.h"

extern   char      *striMaxkghThroughput;


extern  char      *striBaudRate;

//CalDataStruct	g_UploadedCalibrationData;

//extern CalDataStruct    g_CalibrationData;

extern char      *striWidthConfig;
extern char      *striWidthCalibationMode;
extern char      *striWidthCalibationType;
extern char      *striWidthCalibationLineType;
extern char      *striWidthCalibationAlarmLimit;
extern char      *striWidthCalibationProcessDelay;
extern char      *striWidthCalibationIntegrationTime;
extern char      *striWidthCalibationDeadband;
extern char      *striWidthCalibationNormalIncreaseControlGain;
extern char      *striWidthCalibationNormalDecreaseControlGain;
extern char      *striWidthCalibationFCConrolGain;
extern char      *striWidthCalibationAveraging;

extern char      *striWidthUSDistanceToCentre1;
extern char      *striWidthUSDistanceToCentre2;
extern char      *striWidthUSDistanceToCentre3;
extern char      *striWidthUSCountsPerMM;
extern char      *striWidthRefDistanceCalibratedAt;

//wfh
extern char	 	 *striUltrasonicAveraging;
extern char	 	*striWidthOverrun;
extern char	 	*striWidthOverrunDeadband;
extern char	 	*striStandardiseOnAuto;
extern char	 	*striStandardiseOnHour;
extern char	 	*striAlarmInManual;
extern char	 	*striFastCorrectionEnabled;

//wfh new parameters.
extern char	 *striLengthAB;
extern char	 *striLengthBC;
extern char	 *striLengthAC;
extern char	 *striSensorAngleA;
extern char	 *striSensorAngleB;
extern char	 *striSensorAngleC;
extern char	 *striM2WidthOffset;
extern char	 *striM2WidthOffsetSign;
bool g_bUploadedRefLength = FALSE;

//CalDataStruct	g_UploadedCalibrationData;
extern WidthCalDataStruct	g_stWidthCalibration;
WidthCalDataStruct	g_UploadedWidthCalibrationData;


int parse_Width_ini_file(char * ini_name)
{
	dictionary	*	ini ;
//	char cTemp[80];
	/* Some temporary variables to hold query results */
	int				i ;
	int 			b;
	double			d ;
	char	cBuffer[80];

    InitialisePinsUsedBySDCard();



	 g_UploadedWidthCalibrationData = g_stWidthCalibration;  // make a copy of cal data

    g_bSPIBusBusy = TRUE;       // Prevent the SPI use for other uses.


	 InitialiseSDCard();         // note this includes SPI_Select_SD.

	ini = iniparser_load(ini_name);
	if (ini==NULL) {
//      iprintf("\n iniparser_load failed.... parse file [%s]", ini_name);
      g_bSPIBusBusy = FALSE;       // Prevent the SPI use for other uses.
      f_delvolume(MMC_DRV_NUM);
      ReInitialisePinsUsedBySDCard();


		return -1 ;
	}
	else
	{
//	 SetupMttty();
//        iprintf("\n can parse file [%s]", ini_name);
	}
//	iniparser_dump(ini, stderr);

// width mode
	    sprintf( cBuffer,"%s:", striWidthConfig);   //
	    strcat( cBuffer, striWidthCalibationMode );
	    strcat( cBuffer, "\0" );
		i = iniparser_getint(ini, cBuffer, -1);
		if (i!=-1)
	    {
		    g_UploadedWidthCalibrationData.m_wWidthMode = i;
		}
	   else
	   {
	//		printf("width mode:   not found\n");
		}
		// width type
		sprintf( cBuffer,"%s:", striWidthConfig);   //
		strcat( cBuffer, striWidthCalibationType );
		strcat( cBuffer, "\0" );
		i = iniparser_getint(ini, cBuffer, -1);
		if (i!=-1)
		{
	        g_UploadedWidthCalibrationData.m_wWidthType = i;
		}
		else
		{
			//		printf("width type not found\n");
		}

		// line type
		sprintf( cBuffer,"%s:", striWidthConfig);   //
		strcat( cBuffer, striWidthCalibationLineType );
		strcat( cBuffer, "\0" );
		i = iniparser_getint(ini, cBuffer, -1);
		if (i!=-1)
		{
	        g_UploadedWidthCalibrationData.m_wLineType = i;
		}
		else
		{
			//		printf("width type not found\n");
		}

		// alarm limit
		sprintf( cBuffer,"%s:", striWidthConfig);   //
		strcat( cBuffer, striWidthCalibationAlarmLimit );
		strcat( cBuffer, "\0" );
		i = iniparser_getint(ini, cBuffer, -1);
		if (i!=-1)
		{
	        g_UploadedWidthCalibrationData.m_wAlarmLimit = i;
		}
		else
		{
			//		printf("width type not found\n");
		}

		// process delay
		sprintf( cBuffer,"%s:", striWidthConfig);   //
		strcat( cBuffer, striWidthCalibationProcessDelay );
		strcat( cBuffer, "\0" );
		i = iniparser_getint(ini, cBuffer, -1);
		if (i!=-1)
		{
	        g_UploadedWidthCalibrationData.m_wProcessDelayInSeconds = i;
		}
		else
		{
			//		printf("width type not found\n");
		}

		// 	 time
		sprintf( cBuffer,"%s:", striWidthConfig);   //
		strcat( cBuffer, striWidthCalibationIntegrationTime );
		strcat( cBuffer, "\0" );
		i = iniparser_getint(ini, cBuffer, -1);
		if (i!=-1)
		{
	        g_UploadedWidthCalibrationData.m_wWidthIntegrationTimeinSeconds = i;
		}
		else
		{			if(fdTelnet>0)
					printf("width integ not found\n");
		}

		// deadband
		sprintf( cBuffer,"%s:", striWidthConfig);   //
		strcat( cBuffer, striWidthCalibationDeadband );
		strcat( cBuffer, "\0" );
		i = iniparser_getint(ini, cBuffer, -1);
		if (i!=-1)
		{
	        g_UploadedWidthCalibrationData.m_wWidthDeadband = i;
		}
		else
		{
			//		printf("width type not found\n");
		}

		// increase control gain
		sprintf( cBuffer,"%s:", striWidthConfig);   //
		strcat( cBuffer, striWidthCalibationNormalIncreaseControlGain );
		strcat( cBuffer, "\0" );
		d = iniparser_getdouble(ini, cBuffer, -1.0);
		if (i!=-1)
		{
	        g_UploadedWidthCalibrationData.m_fNormalIncreaseControlGainPercentage = d;
		}
		else
		{
			//		printf("width type not found\n");
		}

		// decrease control gain
			sprintf( cBuffer,"%s:", striWidthConfig);   //
			strcat( cBuffer, striWidthCalibationNormalDecreaseControlGain );
			strcat( cBuffer, "\0" );
			d = iniparser_getdouble(ini, cBuffer, -1.0);
			if (i!=-1)
			{
		        g_UploadedWidthCalibrationData.m_fNormalDecreaseControlGainPercentage = d;
			}
			else
			{
				//		printf("width type not found\n");
			}

		// fc control gain
		sprintf( cBuffer,"%s:", striWidthConfig);   //
		strcat( cBuffer, striWidthCalibationFCConrolGain );
		strcat( cBuffer, "\0" );
		d = iniparser_getdouble(ini, cBuffer, -1.0);
		if (i!=-1)
		{
	        g_UploadedWidthCalibrationData.m_fFastCorrectedControlGainPercentage = d;
		}
		else
		{
			//		printf("width type not found\n");
		}
		// width average
		sprintf( cBuffer,"%s:", striWidthConfig);   //
		strcat( cBuffer, striWidthCalibationAveraging );
		strcat( cBuffer, "\0" );
		i = iniparser_getint(ini, cBuffer, -1);
		if (i!=-1)
		{
	        g_UploadedWidthCalibrationData.m_wWidthAveragingInSeconds = i;
		}
		else
		{
			//		printf("width type not found\n");
		}

		// load of distance to centre for sensor 1,2,3
		//
		// dtc #1
			sprintf( cBuffer,"%s:", striWidthConfig);   //
			strcat( cBuffer, striWidthUSDistanceToCentre1);
			strcat( cBuffer, "\0" );
			d = iniparser_getdouble(ini, cBuffer, -1.0);
			if (i!=-1)
			{
		        g_UploadedWidthCalibrationData.m_fDistanceToCentre[0] = d;
			}
			else
			{
				//		printf("dtc not found\n");
			}
			// dtc #2
			sprintf( cBuffer,"%s:", striWidthConfig);   //
			strcat( cBuffer, striWidthUSDistanceToCentre2);
			strcat( cBuffer, "\0" );
			d = iniparser_getdouble(ini, cBuffer, -1.0);
			if (i!=-1)
			{
				g_UploadedWidthCalibrationData.m_fDistanceToCentre[1] = d;
			}
			else
			{
			//		printf("dtc not found\n");
			}

			// dtc #3
			sprintf( cBuffer,"%s:", striWidthConfig);   //
			strcat( cBuffer, striWidthUSDistanceToCentre3);
			strcat( cBuffer, "\0" );
			d = iniparser_getdouble(ini, cBuffer, -1.0);
			if (i!=-1)
			{
				g_UploadedWidthCalibrationData.m_fDistanceToCentre[2] = d;
			}
			else
			{
			//		printf("dtc not found\n");
			}

			// distance calibrated at

			sprintf( cBuffer,"%s:", striWidthConfig);   //
			strcat( cBuffer, striWidthRefDistanceCalibratedAt);
			strcat( cBuffer, "\0" );
			i = iniparser_getint(ini, cBuffer, -1);
			if (i!=-1)
			{
				g_UploadedWidthCalibrationData.m_wUltrasonicMMCalibratedAt = i;
			}
			else
			{
			//		printf("dtc not found\n");
			}

			// counts pmm
			sprintf( cBuffer,"%s:", striWidthConfig);   //
			strcat( cBuffer, striWidthUSCountsPerMM);
			strcat( cBuffer, "\0" );
			d = iniparser_getdouble(ini, cBuffer, -1.0);
			if (i!=-1)
			{
				g_UploadedWidthCalibrationData.m_fUltrasonicMMConversionFactor = d;
			}
			else
			{
			//		printf("dtc not found\n");
			}
// wfh
// Ultrasonic Averaging
		    sprintf( cBuffer,"%s:", striWidthConfig);
		    strcat( cBuffer, striUltrasonicAveraging );
		    strcat( cBuffer, "\0" );
		    i = iniparser_getint(ini, cBuffer, -1);
			if (i!=-1)
			{
				g_UploadedWidthCalibrationData.m_wUltrasonicAveraging = i;
			}
			else
			{
				//
			}

// Width Overrun
		    sprintf( cBuffer,"%s:", striWidthConfig);
		    strcat( cBuffer, striWidthOverrun );
		    strcat( cBuffer, "\0" );
		    i = iniparser_getint(ini, cBuffer, -1);
			if (i!=-1)
			{
				g_UploadedWidthCalibrationData.m_wWidthOverrun = i;
			}
			else
			{
				//
			}

// 	Width Overrun deadband.
		    sprintf( cBuffer,"%s:", striWidthConfig);
		    strcat( cBuffer, striWidthOverrunDeadband  );
		    strcat( cBuffer, "\0" );
		    i = iniparser_getint(ini, cBuffer, -1);
			if (i!=-1)
			{
				g_UploadedWidthCalibrationData.m_wWidthOverrunDeadband = i;
			}
			else
			{
				//
			}

// Standardise on Auto.
		    sprintf( cBuffer,"%s:", striWidthConfig);
		    strcat( cBuffer, striStandardiseOnAuto );
		    strcat( cBuffer, "\0" );
		    b = iniparser_getboolean(ini, cBuffer, -1);
			if (b!=-1)
			{
				g_UploadedWidthCalibrationData.m_bStandardiseOnAuto = i;
			}
			else
			{
				//
			}
// Standardise on Hour.
		    sprintf( cBuffer,"%s:", striWidthConfig);
		    strcat( cBuffer, striStandardiseOnHour );
		    strcat( cBuffer, "\0" );
		    b = iniparser_getboolean(ini, cBuffer, -1);
			if (b!=-1)
			{
				g_UploadedWidthCalibrationData.m_bStandardiseOnTheHour = i;
			}
			else
			{
				//
			}
// Alarm in Manual.
		    sprintf( cBuffer,"%s:", striWidthConfig);
		    strcat( cBuffer, striAlarmInManual );
		    strcat( cBuffer, "\0" );
		    b = iniparser_getboolean(ini, cBuffer, -1);
			if (b!=-1)
			{
				g_UploadedWidthCalibrationData.m_bAlarmInManual = i;
			}
			else
			{
				//
			}
// Fast Correction enabled.
		    sprintf( cBuffer,"%s:", striWidthConfig);
		    strcat( cBuffer, striFastCorrectionEnabled );
		    strcat( cBuffer, "\0" );
		    b = iniparser_getboolean(ini, cBuffer, -1);
			if (b!=-1)
			{
				g_UploadedWidthCalibrationData.m_bFastCorrectionEnabled = i;
			}
			else
			{
				//
			}
// Length AB
			sprintf( cBuffer,"%s:", striWidthConfig);   //
			strcat( cBuffer, striLengthAB);
			strcat( cBuffer, "\0" );
			d = iniparser_getdouble(ini, cBuffer, -1.0);
			if (d!=-1)
			{
				g_UploadedWidthCalibrationData.m_fLength_AB = d;
			}
			else
			{
				//
			}

// Length BC
			sprintf( cBuffer,"%s:", striWidthConfig);   //
			strcat( cBuffer, striLengthBC);
			strcat( cBuffer, "\0" );
			d = iniparser_getdouble(ini, cBuffer, -1.0);
			if (d!=-1)
			{
				g_UploadedWidthCalibrationData.m_fLength_BC = d;
			}
			else
			{
				//
			}
// Length AC
			sprintf( cBuffer,"%s:", striWidthConfig);   //
			strcat( cBuffer, striLengthAC);
			strcat( cBuffer, "\0" );
			d = iniparser_getdouble(ini, cBuffer, -1.0);
			if (d!=-1)
			{
				g_UploadedWidthCalibrationData.m_fLength_AC = d;
			}
			else
			{
				//
			}
// Sensor angle A
			sprintf( cBuffer,"%s:", striWidthConfig);   //
			strcat( cBuffer, striSensorAngleA);
			strcat( cBuffer, "\0" );
			d = iniparser_getdouble(ini, cBuffer, -1.0);
			if (d!=-1)
			{
				g_UploadedWidthCalibrationData.m_fSensorAngleA = d;
			}
			else
			{
				//
			}

// Sensor angle B
			sprintf( cBuffer,"%s:", striWidthConfig);   //
			strcat( cBuffer, striSensorAngleB);
			strcat( cBuffer, "\0" );
			d = iniparser_getdouble(ini, cBuffer, -1.0);
			if (d!=-1)
			{
				g_UploadedWidthCalibrationData.m_fSensorAngleB = d;
			}
			else
			{
				//
			}

// Sensor angle C
			sprintf( cBuffer,"%s:", striWidthConfig);   //
			strcat( cBuffer, striSensorAngleC);
			strcat( cBuffer, "\0" );
			d = iniparser_getdouble(ini, cBuffer, -1.0);
			if (d!=-1)
			{
				g_UploadedWidthCalibrationData.m_fSensorAngleC = d;
			}
			else
			{
				//
			}

// M2 Width Offset
			sprintf( cBuffer,"%s:", striWidthConfig);   //
			strcat( cBuffer, striM2WidthOffset);
			strcat( cBuffer, "\0" );
			d = iniparser_getdouble(ini, cBuffer, -1.0);
			if (d!=-1)
			{
				g_UploadedWidthCalibrationData.m_fM2WidthOffset = d;
			}
			else
			{
				//
			}

// M2 Width Offset sign.
			sprintf( cBuffer,"%s:", striWidthConfig);   //
			strcat( cBuffer, striM2WidthOffsetSign);
			strcat( cBuffer, "\0" );
			i = iniparser_getint(ini, cBuffer, -1.0);
			if (i!=-1)
			{
				g_UploadedWidthCalibrationData.m_nM2WidthOffsetSign = i;
			}
			else
			{
				//
			}

		g_stWidthCalibration = g_UploadedWidthCalibrationData;
		g_bSaveWidthControlCalibration = TRUE;
// if a valid reference length was uploaded,  set flat to cause reference to be calibrated.
		if( (g_stWidthCalibration.m_wUltrasonicMMCalibratedAt > 100) && ( g_stWidthCalibration.m_wUltrasonicMMCalibratedAt < 500 ) )
			g_bUploadedRefLength = TRUE;

    f_delvolume(MMC_DRV_NUM);

    ReInitialisePinsUsedBySDCard();

    g_bSPIBusBusy = FALSE;       // free up the SPI for other uses.


	return 0 ;
}

// load config from sd card

void LoadWidthConfigFromSDCard( void )
{

    int     status1;
    char    cBuffer[20];
    if ( SDCardPresent() )
    {
        sprintf( cBuffer, "wconfig.txt");
        status1 =  parse_Width_ini_file( cBuffer );
        CopyWidthConfigToMB();
        if(status1 == 0)
        {
            g_bSDCardLoadConfigComplete = TRUE;
        }
    }
}
