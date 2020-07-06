// P.Smith                             30/5/07
// InitialisePinsUsedBySDCard called in parse_Batch_ini_file
// Remove iniparser_dump
// Correct check of returned value when parsing, should have been checking for -1
// added blender mode, blender type, material transport delay, material dump delay.

//  Michael McKiernan                                    20/6/07
//      Edited parse_Batch_ini_file(char * ini_name)
// at ~ L137 replaced  InitialisePinsUsedBySDCard() by
//                         g_bSPIBusBusy = TRUE;       // Prevent the SPI use for other uses.
//	                        InitialiseSDCard();         // note this includes SPI_Select_SD.
// comment  multi stage fill	corrected to Units.
//  In the latency, replaced float by int, i.e assume value in Ini file is stored directly in mS.
//	 i = iniparser_getint(ini, cBuffer, -1);

//	         //d = iniparser_getdouble(ini, cBuffer, -1.0);
//          g_UploadedCalibrationData.m_nLatency = i;   // (int)(d * 1000.0f);  //mmk store latency as ms on SD
//  At end of function call;
//          f_delvolume(MMC_DRV_NUM);
//          ReInitialisePinsUsedBySDCard();
//          g_bSPIBusBusy = FALSE;       // free up the SPI for other uses.
//
// P.Smith                             21/6/07
// ensure that if the function to load the calibration data from the sd card returns early that the
// flag g_bSPIBusBusy is reset and any other reinitialisation and demounting of fat is
// also done.
//
// P.Smith                             31/7/07
// remove printf in load of cal data
//
// P.Smith                             31/7/07
// Correct load of cal data, moulding, high low regrind, cycle speed, m_bFillOrderManualSelect
// cleaning name change m_nCleaning, recipe entry mode, max component % limit
// extruder shut down time, control method % limit
//
// P.Smith                             7/1/07
// Correct latency load from sd card.
//
// P.Smith                             7/5/07
// call InitialisePinsUsedBySDCard so that the spi pins are set up correctly.
// for the parsing to run properly.
//
// P.Smith                             26/6/08
// correct component layering,moulding, continuous download,telnet,
// vacuum loading, deviation alarm enable,recipe compomponent no, pause on power up
// latency, line speed average, screw speed signal type
//
// P.Smith                             3/10/08
// upload liquid additive enable / disable
//
// P.Smith                             14/10/08
// remove cval
//
// P.Smith                             26/03/09
// correct read of sd card log, this was being read as a int instead of a bool
// it is a yes / no option.
//
// P.Smith                             24/04/09
// added parity, non std config,usb settings,
//
// P.Smith                             13/5/09
// when the target weight is loaded from the sd card, set g_bFillTargetEntryFlag
// to ensure that the component target weight is calculated.
//
// P.Smith                             30/6/09
// added load of vacuum loading data from sd card.
//
// P.Smith                          2/7/09
// name change dump delay to discharge time
//
// P.Smith                          17/9/09
// change log to sd card to load it is now an integer, not a bool
//
// P.Smith                          7/12/09
// added vac8 expansion, min opening time, flow rate check
//
// M.McKiernan                      1/12/09
// added predef.h
//
// P.Smith                          8/3/10
// remove upload of blender type
//
// P.Smith                          12/4/10
// remove mixing screw detection
//
// P.Smith                          4/5/10
// add baud rate to config load.

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

extern  char    *striHopperWeightPage;
extern  char    *striDoubleDump;
extern  char    *strNewLine;
extern  char    *striCalibrationWeightConstant;
extern  char    *striFillTargetWeight;
extern  char    *striWeightAveraging;
extern  char    *striTare;
extern  char    *striConfigPage1;
extern  char    *striBlenderType;
extern   char   *striBlenderMode;
extern   char   *striNumberOfBlends;
extern   char   *striMixingScrewDetection;
extern   char   *striOfflineMode;
extern   char   *striFillRetry;
extern   char   *striFirstComponentCompensation;
extern   char   *striMultiStagedFilling;
extern   char   *striBatchesInKghAverage;
extern   char      *striConfigPage2;
extern   char      *striBlenderUits;
extern   char      *striRemoteModules;
extern   char      *striStandardCompConfig;
extern   char      *striOfflineMixingTime;
extern   char      *striMixingMotorShutOffTime;
extern   char      *striDiagnostics;
extern   char      *striLevelSensorAlarmDelay;
extern   char      *striVolumetricMode;
extern   char      *striCycleSpeed;
extern   char      *striMaxkghThroughput;


extern   char      *striConfigPage3;
extern   char      *striRegrindFillRetry;
extern   char      *striRegrindAsReference;
extern   char      *striSingleWordWrite;
extern   char      *striCleaning;
extern   char      *striTopUp;
extern   char      *striTopUpReference;
extern   char      *striFirstComponentLayering;
extern   char      *striPercentageTopUp;
extern   char      *striMaxkgsForTopUp;
extern   char      *striLatency;
extern   char      *striPeriodicMixingTime;
extern   char      *striRecipeEntry;
extern   char      *striComponentPercentageAlarm;
extern   char      *striMaxComponentPercentagelimit;
extern   char      *striBypass;

extern   char      *striControlMeasAndControlPage;
extern   char      *striControlGain;
extern   char      *striControlAveraging;
extern   char      *striFastControlResponse;
extern   char      *striExtruderStarvationShutdown;
extern   char      *striExtruderShutdownTime;
extern   char      *striControlMethod;
extern   char      *striCorrectEveryCycle;
extern   char      *striThroughputDeadband;
extern   char      *striControlType;
extern   char      *striControlLag;
extern   char      *striIncreaseRate;
extern   char      *striDeadband;
extern   char      *striFineband;



extern   char      *striCleaningPage;
extern   char      *striTopOnTime;
extern   char      *striTopOffTime;
extern   char      *striTopRepetitions;
extern   char      *striBottomOnTime;
extern   char      *striBottomOffTime;
extern   char      *striBottomRepetitions;
extern   char      *striBypassTimeLimit;
extern   char      *striBlowEveryCycle;
extern   char      *striBypassActivation;

extern   char      *striControlLineSpeedPage;
extern   char      *striLineSpeedConstant;
extern   char      *striMaxExtruderSetpoint;
extern   char      *striLineSpeedSignalType;
extern   char      *striLineSpeedConstant;

extern   char      *striMaterialTransportDelay;
extern   char      *striMaterialDumpDelay;
extern   char      *striPurge;
extern   char      *striPurgeComponentNumber;
extern   char      *striSensitivity;
extern  char       *striHighLowRegrind;
extern  char       *striMoulding;
extern  char       *striContinuousDownload;
extern  char       *striTelnet;
extern  char       *striVacuumLoading;
extern  char       *striAlphaNumeric;
extern  char       *striPauseBlenderOnPowerUp;
extern  char       *striDeviationAlarmEnable;
extern  char       *striLogToSDCard;
extern  char       *striLineSpeedAverage;
extern  char       *striScrewSpeedSignalType;
extern  char       *striLiquidAdditive;
extern   char      *striParity;
extern   char      *striNonStdConfig;


extern  char      *striUSBConfigPage;
extern  char      *strUSBEnable;
extern  char      *strUSBPrinterAlarmEnable;
extern  char      *strUSBPrinterOrderEnable;
extern  char      *strUSBPrinterRollEnable;
extern  char      *strUSBPrinterHourEnable;
extern  char      *strUSBPrinterBatchEnable;
extern  char      *strUSBPrinterEventEnable;
extern  char      *strUSBPrinterHistoryEnable;
extern  char      *strUSBPrinterShiftEnable;
extern  char      *strUSBPrinterMonthlyEnable;

extern  char      *striVacuumLoadingPage;
extern  char      *striNumberOfLoaders;
extern  char      *striIdleTime;
extern  char      *striPumpStartTime;
extern  char      *striAtmValveOverlap;
extern  char      *striFilterRepetitions;
extern  char      *striFilterType;
extern  char      *striDumpDelay;

extern  char      *striIndividualLoaderPage;
extern  char      *striLoaderOnOffStatus;
extern  char      *striLoaderSuctionTime;
extern  char      *striLoaderPostFillTime;
extern  char      *striPriority;
extern  char      *striMinOpeningTimeCheck;
extern  char      *striFlowRateCheck;
extern  char      *striVAC8Expansion;
extern  char      *striBaudRate;



extern CalDataStruct    g_CalibrationData;

CalDataStruct	g_UploadedCalibrationData;


int parse_Batch_ini_file(char * ini_name)
{
	dictionary	*	ini ;
	char cTemp[80];
	/* Some temporary variables to hold query results */
	int				b ;
	int				i ;
	unsigned int    j;
	double			d ;
	double        ffNewWtConstant;
	float         fNewBatchWt;
	long          lNewTareValue;
	long    lTemp;
	char	cBuffer[80];

    InitialisePinsUsedBySDCard();

   ffNewWtConstant = 7.9979;
   lNewTareValue = 1909;
   fNewBatchWt = 1.777;


	 g_UploadedCalibrationData = g_CalibrationData;  // make a copy of cal data

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


// weight constant
    sprintf( cBuffer,"%s:", striHopperWeightPage);   //
    strcat( cBuffer, striCalibrationWeightConstant );
    strcat( cBuffer, "\0" );

	d = iniparser_getdouble(ini, cBuffer, -1.0);
	if (d!=-1)
    {
	   	    g_UploadedCalibrationData.m_fWeightConstant = d ;
//    		printf("Weight Constant:   [%f]\n", g_UploadedCalibrationData.m_fWeightConstant);
	   }
     else
     {
//		printf("Weight Constant:   not found\n");
	 }
// fill target weight

    sprintf( cBuffer,"%s:", striHopperWeightPage);   //
    strcat( cBuffer, striFillTargetWeight );
    strcat( cBuffer, "\0" );
	d = iniparser_getdouble(ini, cBuffer, -1.0);
	if (d!=-1)
    {
       g_UploadedCalibrationData.m_fBatchSize = d ;
//		printf("Batch Weight:   [%f]\n", g_UploadedCalibrationData.m_fBatchSize);
	}
   else
   {
//		printf("Batch Weight:   not found\n");
	}

// weight averaging.
    sprintf( cBuffer,"%s:", striHopperWeightPage);   //
    strcat( cBuffer, striWeightAveraging );
    strcat( cBuffer, "\0" );
	i = iniparser_getint(ini, cBuffer, -1);
	if (i!=-1)
    {
	    g_UploadedCalibrationData.m_nWeightAvgFactor = i;
//		printf("weight averaging:   [%d]\n", g_UploadedCalibrationData.m_nWeightAvgFactor);
	}
   else
   {
//		printf("weight averaging:   not found\n");
	}

// Double dump
	/* Get hopper attributes */
    sprintf( cBuffer,"%s:", striHopperWeightPage);   // strWeightDisplayUnits[g_MeasurementUnits.m_nWeightUnits]
    strcat( cBuffer, striDoubleDump );
    strcat( cBuffer, "\0" );
	b = iniparser_getboolean(ini, cBuffer, -1);

	if (b!=-1)
    {
	    g_UploadedCalibrationData.m_bDoubleDump = b;
    }
    else
    {
//		printf("Double dump not found\n");
    }

//    printf("Double Dump:       [%d]\n", g_UploadedCalibrationData.m_bDoubleDump);

// tare value.
    sprintf( cBuffer,"%s:", striHopperWeightPage);   //
    strcat( cBuffer,striTare);
    strcat( cBuffer, "\0" );
	lTemp = iniparser_getlong(ini, cBuffer, -1);
	if (lTemp!= -1)
    {

    	g_UploadedCalibrationData.m_lTareCounts = lTemp;
//		printf("Tare Value:   [%ld]\n", g_UploadedCalibrationData.m_lTareCounts);
	}
   else
   {
//		printf("Tare Value:   not found\n");
	}


// Config page 1
// Blender type

//    sprintf( cBuffer,"%s:", striConfigPage1);   //
//    strcat( cBuffer,striBlenderType);
//    strcat( cBuffer, "\0" );
//	i = iniparser_getint(ini, cBuffer, -1);
//	if (i!= -1)
//    {
//	    g_UploadedCalibrationData.m_nBlenderType = i;
//		printf("Blender type:   [%d]\n", g_UploadedCalibrationData.m_nBlenderType);
//	}
//   else
//   {
//		printf("blender type:   not found\n");
//	}


// Blender mode

    sprintf( cBuffer,"%s:", striConfigPage1);   //
    strcat( cBuffer,striBlenderMode);
    strcat( cBuffer, "\0" );
	i = iniparser_getint(ini, cBuffer, -1);
	if (i!= -1)
    {
	    g_UploadedCalibrationData.m_nBlenderMode = i;
        //printf("Blender mode:   [%d]\n", g_UploadedCalibrationData.m_nBlenderMode);
	}
   else
   {
        //printf("blender mode:   not found\n");
	}


// No of blends
    sprintf( cBuffer,"%s:", striConfigPage1);   //
    strcat( cBuffer,striNumberOfBlends);
    strcat( cBuffer, "\0" );
	i = iniparser_getint(ini, cBuffer, -1);
	if (i!= -1)
    {
	    g_UploadedCalibrationData.m_nComponents = i;
		//printf("No of blends:   [%d]\n", g_UploadedCalibrationData.m_nComponents);
	}
   else
   {
		//printf("No of blends:   not found\n");
	}


// mixing screw detection
//
// Mixing screw detection
	/* Get hopper attributes */
/*    sprintf( cBuffer,"%s:", striConfigPage1);
    strcat( cBuffer, striMixingScrewDetection );
    strcat( cBuffer, "\0" );
	b = iniparser_getboolean(ini, cBuffer, -1);

	if (b!= -1)
    {
	    g_UploadedCalibrationData.m_bMixerScrewDetect = b;
    }
    else
    {
//		printf("mixing screw detection not found\n");
    }
*/
//    printf("mixing screw detection:       [%d]\n", g_UploadedCalibrationData.m_bMixerScrewDetect);


// offline mode
    sprintf( cBuffer,"%s:", striConfigPage1);   //
    strcat( cBuffer,striOfflineMode);
    strcat( cBuffer, "\0" );
	i = iniparser_getint(ini, cBuffer, -1);
	if (i!= -1)
    {
	    g_UploadedCalibrationData.m_nOfflineType = i;
//		printf("offline mode:   [%d]\n", g_UploadedCalibrationData.m_nOfflineType);
	}
   else
   {
//		printf("offline mode:   not found\n");
	}
//		printf("offline mode:   [%d]\n", g_UploadedCalibrationData.m_nOfflineType);

// fill retry
    sprintf( cBuffer,"%s:", striConfigPage1);   //
    strcat( cBuffer,striFillRetry);
    strcat( cBuffer, "\0" );
	i = iniparser_getint(ini, cBuffer, -1);
	if (i!= -1)
    {

	    g_UploadedCalibrationData.m_nFillRetries = i;
//		printf("fill retry:   [%d]\n", g_UploadedCalibrationData.m_nFillRetries);
	}
   else
   {
//		printf("fill retry:   not found\n");
	}
//		printf("fill retry:   [%d]\n", g_UploadedCalibrationData.m_nFillRetries);

// First component compensation
	/* Get hopper attributes */
    sprintf( cBuffer,"%s:", striConfigPage1);
    strcat( cBuffer, striFirstComponentCompensation );
    strcat( cBuffer, "\0" );
	b = iniparser_getboolean(ini, cBuffer, -1);

	if (b != -1)
    {
	    g_UploadedCalibrationData.m_bFirstComponentCompensation = b;
    }
    else
    {
//		printf("first component compensation not found\n");
    }

// high low regrind

    sprintf( cBuffer,"%s:", striConfigPage1);
    strcat( cBuffer, striHighLowRegrind );
    strcat( cBuffer, "\0" );
	b = iniparser_getboolean(ini, cBuffer, -1);

	if (b != -1)
    {
	    g_UploadedCalibrationData.m_bHighLoRegrindOption = b;
    }
    else
    {
//		printf("first component compensation not found\n");
    }


// first component layering
    sprintf( cBuffer,"%s:", striConfigPage3);   //
    strcat( cBuffer,striFirstComponentLayering);
    strcat( cBuffer, "\0" );
	b = iniparser_getboolean(ini, cBuffer, -1);

	if (b != -1)
    {
	    g_UploadedCalibrationData.m_bLayering = b;
//  	    printf("layering:   [%d]\n", g_UploadedCalibrationData.m_bLayering);
    }
    else
    {
//  	    printf("layering not found\n");
    }



// multi stage fill
    sprintf( cBuffer,"%s:", striConfigPage1);   //
    strcat( cBuffer,striMultiStagedFilling);
    strcat( cBuffer, "\0" );
	i = iniparser_getint(ini, cBuffer, -1);
	if (i != -1)
    {
	    g_UploadedCalibrationData.m_nStageFillEn = i;
//		printf("stage fill:   [%d]\n", g_UploadedCalibrationData.m_nStageFillEn);
	}
   else
   {
//		printf("stage fill:   not found\n");
	}


// batches in kg/h average

    sprintf( cBuffer,"%s:", striConfigPage1);   //
    strcat( cBuffer,striBatchesInKghAverage);
    strcat( cBuffer, "\0" );
    i = iniparser_getint(ini, cBuffer, -1);
	if (i != -1)
    {
	    g_UploadedCalibrationData.m_nBatchesInKghAvg = i;
	}
   else
   {
//		printf("batches in kg/h average   not found\n");
   }




// moulding

    sprintf( cBuffer,"%s:", striConfigPage1);
    strcat( cBuffer, striMoulding );
    strcat( cBuffer, "\0" );
	b = iniparser_getboolean(ini, cBuffer, -1);

	if (b != -1)
    {
	    g_UploadedCalibrationData.m_bMouldingOption = b;
    }

// continuous download
    sprintf( cBuffer,"%s:", striConfigPage1);
    strcat( cBuffer, striContinuousDownload );
    strcat( cBuffer, "\0" );
	b = iniparser_getboolean(ini, cBuffer, -1);

	if (b != -1)
    {
	    g_UploadedCalibrationData.m_bAllowForContinuousDownload = b;
    }

// Telnet
    sprintf( cBuffer,"%s:", striConfigPage1);
    strcat( cBuffer, striTelnet );
    strcat( cBuffer, "\0" );
	b = iniparser_getboolean(ini, cBuffer, -1);

	if (b != -1)
    {
	    g_UploadedCalibrationData.m_bTelnet = b;
    }

// vacuum loading

    sprintf( cBuffer,"%s:", striConfigPage1);
    strcat( cBuffer, striVacuumLoading );
    strcat( cBuffer, "\0" );
	b = iniparser_getboolean(ini, cBuffer, -1);

	if (b != -1)
    {
	    g_UploadedCalibrationData.m_bVacLoading = b;
    }

// parity

    sprintf( cBuffer,"%s:", striConfigPage1);
    strcat( cBuffer, striParity);
    strcat( cBuffer, "\0" );
	b = iniparser_getboolean(ini, cBuffer, -1);

	if (b != -1)
    {
	    g_UploadedCalibrationData.m_bNoParity = b;
    }

// baud rate
    sprintf( cBuffer,"%s:", striConfigPage1);   //
    strcat( cBuffer,striBaudRate);
    strcat( cBuffer, "\0" );
	i = iniparser_getint(ini, cBuffer, -1);
    if (i != -1)
    {
	    g_UploadedCalibrationData.m_nBaudRate = i;
	}

// vac8 expansion

    sprintf( cBuffer,"%s:", striConfigPage1);
    strcat( cBuffer, striVAC8Expansion);
    strcat( cBuffer, "\0" );
	b = iniparser_getboolean(ini, cBuffer, -1);

	if (b != -1)
    {
	    g_UploadedCalibrationData.m_bVac8Expansion = b;
    }

// min opening time check

    sprintf( cBuffer,"%s:", striConfigPage1);
    strcat( cBuffer, striMinOpeningTimeCheck);
    strcat( cBuffer, "\0" );
	b = iniparser_getboolean(ini, cBuffer, -1);

	if (b != -1)
    {
	    g_UploadedCalibrationData.m_bMinimumOpeningTimeCheck = b;
    }



// config page 2

// UNITS
    sprintf( cBuffer,"%s:", striConfigPage2);   //
    strcat( cBuffer,striBlenderUits);
    strcat( cBuffer, "\0" );
	i = iniparser_getint(ini, cBuffer, -1);
	if (i != -1)
    {
	    g_UploadedCalibrationData.m_nUnitType = i;
//		printf("units:   [%d]\n", g_UploadedCalibrationData.m_nUnitType);
	}
   else
   {
        //printf("units:   not found\n");
	}

// remote modules
    sprintf( cBuffer,"%s:", striConfigPage2);   //
    strcat( cBuffer,striRemoteModules);
    strcat( cBuffer, "\0" );
	i = iniparser_getint(ini, cBuffer, -1);
	if (i != -1)
    {
	    g_UploadedCalibrationData.m_nPeripheralCardsPresent = i;
//		printf("remote modules:   [%d]\n", g_UploadedCalibrationData.m_nPeripheralCardsPresent);
	}
   else
   {
//		printf("remote modules:   not found\n");
   }

 // Standard component config
	/* Get hopper attributes */
    sprintf( cBuffer,"%s:", striConfigPage2);
    strcat( cBuffer, striStandardCompConfig );
    strcat( cBuffer, "\0" );
	b = iniparser_getboolean(ini, cBuffer, -1);
	if (b != -1)
    {
              g_UploadedCalibrationData.m_bStandardCompConfig = b;
    }
    else
    {
//		printf("comp config not found\n");
    }

  // offline mixing time
    sprintf( cBuffer,"%s:", striConfigPage2);   //
    strcat( cBuffer,striOfflineMixingTime);
    strcat( cBuffer, "\0" );
	i = iniparser_getint(ini, cBuffer, -1);
	if (i != -1)
    {
	    g_UploadedCalibrationData.m_nMixingTime = i;
//		printf("offline mixing time:   [%d]\n", g_UploadedCalibrationData.m_nMixingTime);
	}
   else
   {
//		printf("offline mixing time not found\n");
   }

  // mixing motor shut off time
    sprintf( cBuffer,"%s:", striConfigPage2);   //
    strcat( cBuffer,striMixingMotorShutOffTime);
    strcat( cBuffer, "\0" );
	i = iniparser_getint(ini, cBuffer, -1);
//    iprintf("\n i is %d",i);
    if (i != -1)
    {
	    g_UploadedCalibrationData.m_nMixShutoffTime = i;
//		printf("mixing motor shut off time:   [%d]\n", g_UploadedCalibrationData.m_nMixShutoffTime);
	}
   else
   {
//		printf("mixing motor shut off time not found\n");
   }

//		printf("mixing motor shut off time:   [%d]\n", g_UploadedCalibrationData.m_nMixShutoffTime);

  // diagnostics
    sprintf( cBuffer,"%s:", striConfigPage2);   //
    strcat( cBuffer,striDiagnostics);
    strcat( cBuffer, "\0" );
	i = iniparser_getint(ini, cBuffer, -1);
    if (i != -1)
    {
	    g_UploadedCalibrationData.m_nDiagnosticsMode = i;
//		printf("diagnostics:   [%d]\n", g_UploadedCalibrationData.m_nDiagnosticsMode);
	}
   else
   {
//		printf("diagnostics not found\n");
   }
  // level sensor alarm delay

    sprintf( cBuffer,"%s:", striConfigPage2);   //
    strcat( cBuffer,striLevelSensorAlarmDelay);
    strcat( cBuffer, "\0" );
	i = iniparser_getint(ini, cBuffer, -1);
    if (i != -1)
    {
	    g_UploadedCalibrationData.m_nLevelSensorDelay = i;
//		printf("level sensor alarm delay:   [%d]\n", g_UploadedCalibrationData.m_nLevelSensorDelay);
	}
   else
   {
//		printf("diagnostics not found\n");
   }

// volumetric mode
    sprintf( cBuffer,"%s:", striConfigPage2);
    strcat( cBuffer, striVolumetricMode );
    strcat( cBuffer, "\0" );
	b = iniparser_getboolean(ini, cBuffer, -1);

	if (b != -1)
    {
	    g_UploadedCalibrationData.m_bVolumetricModeFlag = b;
//        printf("volumetric mode:       [%d]\n", g_UploadedCalibrationData.m_bVolumetricModeFlag);
    }
    else
    {
//		printf("volumetric mode not found\n");
    }

  // cycle speed

    sprintf( cBuffer,"%s:", striConfigPage2);   //
    strcat( cBuffer,striCycleSpeed);
    strcat( cBuffer, "\0" );
	b = iniparser_getboolean(ini, cBuffer, -1);
    if (b != -1)
    {
	    if(b)
	    {
             g_UploadedCalibrationData.m_bCycleFastFlag = 0;
	    }
	    else
	    {
             g_UploadedCalibrationData.m_bCycleFastFlag = 1;
	    }
//		printf("cycle speed:   [%d]\n", g_UploadedCalibrationData.m_bCycleFastFlag);

	}
   else
   {
//		printf("cycle speed not found\n");
   }

// Max kg/h throughput
    sprintf( cBuffer,"%s:", striConfigPage2);   //
    strcat( cBuffer,striMaxkghThroughput);
    strcat( cBuffer, "\0" );
	i = iniparser_getint(ini, cBuffer, -1);
    if (i != -1)
    {
	    g_UploadedCalibrationData.m_nMaxThroughput = i;
//		printf("max throughput:   [%d]\n", g_UploadedCalibrationData.m_nMaxThroughput);
	}
   else
   {
//		printf("max throughput not found\n");
   }

// Material Transport delay
    sprintf( cBuffer,"%s:", striConfigPage2);   //
    strcat( cBuffer,striMaterialTransportDelay);
    strcat( cBuffer, "\0" );
	i = iniparser_getint(ini, cBuffer, -1);
    if (i != -1)
    {
	    g_UploadedCalibrationData.m_nMaterialTransportDelay = i;
 	}
   else
   {
//		printf("material transport delay not found\n");
   }

// Material Dump delay
    sprintf( cBuffer,"%s:", striConfigPage2);   //
    strcat( cBuffer,striMaterialDumpDelay);
    strcat( cBuffer, "\0" );
	i = iniparser_getint(ini, cBuffer, -1);
    if (i != -1)
    {
	    g_UploadedCalibrationData.m_nPostMaterialTransportDelay = i;
	}
   else
   {
//		printf("material transport delay not found\n");
   }

// Sensitivity
    sprintf( cBuffer,"%s:", striConfigPage2);   //
    strcat( cBuffer,striSensitivity);
    strcat( cBuffer, "\0" );
	b = iniparser_getboolean(ini, cBuffer, -1);
    if (b != -1)
    {
	    if(b)
	    {
             g_UploadedCalibrationData.m_bFillOrderManualSelect = 1;
	    }
	    else
	    {
             g_UploadedCalibrationData.m_bFillOrderManualSelect = 0;
	    }
//		printf("Sensitivity:   [%d]\n", g_UploadedCalibrationData.m_bFillOrderManualSelect);

    }

// deviation alarm
    sprintf( cBuffer,"%s:", striConfigPage2);   //
    strcat( cBuffer,striDeviationAlarmEnable);
    strcat( cBuffer, "\0" );
	i = iniparser_getint(ini, cBuffer, -1);
    if (i != -1)
    {
	    g_UploadedCalibrationData.m_nDeviationAlarmEnable = i;
	}

// alpha numeric
//

    sprintf( cBuffer,"%s:", striConfigPage2);   //
    strcat( cBuffer,striAlphaNumeric);
    strcat( cBuffer, "\0" );
	b = iniparser_getboolean(ini, cBuffer, -1);
    if (b != -1)
    {
	    g_UploadedCalibrationData.m_bComponentNamingAlphanumeric = b;
    }

// Pause on power up


    sprintf( cBuffer,"%s:", striConfigPage2);   //
    strcat( cBuffer,striPauseBlenderOnPowerUp);
    strcat( cBuffer, "\0" );
	b = iniparser_getboolean(ini, cBuffer, -1);
    if (b != -1)
    {
	    g_UploadedCalibrationData.m_bPauseBlenderOnPowerup = b;
//        iprintf("\n found pause on st up");
    }


// non standard config

    for(j=0; j < g_CalibrationData.m_nComponents; j++)
    {

        sprintf( cBuffer,"%s:", striNonStdConfig);   //
        sprintf( cTemp,"component %d",j+1);
        strcat(cBuffer,cTemp);
        strcat( cBuffer, "\0" );
	    i = iniparser_getint(ini, cBuffer, -1);

        if (i!= -1)
	    {
	        g_UploadedCalibrationData.m_nCompConfig[j] = i;
	        //SetupMttty();
	        //iprintf("\n picked up non config data");
	    }
	    else
	    {
	        //SetupMttty();
	        //iprintf("\n no non standard config data");
	    }
    }


//  config page 3


// regrind fill retry
    sprintf( cBuffer,"%s:",striConfigPage3);
    strcat( cBuffer, striRegrindFillRetry );
    strcat( cBuffer, "\0" );
	b = iniparser_getboolean(ini, cBuffer, -1);

	if (b != -1)
    {
	    g_UploadedCalibrationData.m_bRegrindRetry = b;
//		printf("fill retry:   [%d]\n", g_UploadedCalibrationData.m_bRegrindRetry);
    }
    else
    {
//		printf("regrind fill retry not found\n");
    }

// use regrind as reference
    sprintf( cBuffer,"%s:",striConfigPage3);
    strcat( cBuffer, striRegrindAsReference );
    strcat( cBuffer, "\0" );
	b = iniparser_getboolean(ini, cBuffer, -1);

	if (b != -1)
    {
	    g_UploadedCalibrationData.m_nUseRegrindAsReference = b;
//		printf("regrind as ref:   [%d]\n", g_UploadedCalibrationData.m_nUseRegrindAsReference);
    }
    else
    {
//		printf("regrind as ref not found\n");
    }
// single word write
    sprintf( cBuffer,"%s:",striConfigPage3);
    strcat( cBuffer, striSingleWordWrite );
    strcat( cBuffer, "\0" );
	b = iniparser_getboolean(ini, cBuffer, -1);

	if (b != -1)
    {
	    g_UploadedCalibrationData.m_bSingleWordWrite = b;
//		printf("single word write:   [%d]\n", g_UploadedCalibrationData.m_bSingleWordWrite);
    }
    else
    {
//		printf("single word write not found\n");
    }
// bypass mode
    sprintf( cBuffer,"%s:",striConfigPage3);
    strcat( cBuffer, striBypass );
    strcat( cBuffer, "\0" );
	b = iniparser_getboolean(ini, cBuffer, -1);

	if (b != -1)
    {
	    g_UploadedCalibrationData.m_bBypassMode = b;
//		printf("bypass mode:   [%d]\n", g_UploadedCalibrationData.m_bBypassMode);
    }
    else
    {
//		printf("bypass mode not found\n");
    }

// cleaning
    sprintf( cBuffer,"%s:", striConfigPage3);   //
    strcat( cBuffer,striCleaning);
    strcat( cBuffer, "\0" );
	i = iniparser_getint(ini, cBuffer, -1);
    if (i != -1)
    {
	    g_UploadedCalibrationData.m_nCleaning = i;
//		printf("cleaning:   [%d]\n", g_UploadedCalibrationData.m_nCleaning);
	}
   else
   {
//		printf("cleaning not found\n");
   }
// top up
    sprintf( cBuffer,"%s:", striConfigPage3);   //
    strcat( cBuffer,striTopUp);
    strcat( cBuffer, "\0" );
	b = iniparser_getboolean(ini, cBuffer, -1);

	if (b != -1)
    {
	    g_UploadedCalibrationData.m_bTopUpMode = b;
//		printf("top up:   [%d]\n", g_UploadedCalibrationData.m_bTopUpMode);
    }
    else
    {
//		printf("top up not found\n");
    }

// top up reference
    sprintf( cBuffer,"%s:", striConfigPage3);   //
    strcat( cBuffer,striTopUpReference);
    strcat( cBuffer, "\0" );
	i = iniparser_getint(ini, cBuffer, -1);
    if (i != -1)
    {
	    g_UploadedCalibrationData.m_nTopUpCompNoRef = i;
//		printf("top comp reference:   [%d]\n", g_UploadedCalibrationData.m_nTopUpCompNoRef);
	}
   else
   {
//		printf("top comp reference not found\n");
   }



    // percentage top up
    sprintf( cBuffer,"%s:", striConfigPage3);   //
    strcat( cBuffer,striPercentageTopUp);
    strcat( cBuffer, "\0" );
	i = iniparser_getint(ini, cBuffer, -1);
    if (i != -1)
    {
	    g_UploadedCalibrationData.m_nPercentageTopUp = i;
//		printf("% top up :   [%d]\n", g_UploadedCalibrationData.m_nPercentageTopUp);
	}
   else
   {
//		printf("% top up not found\n");
   }
    // max kg for top up
    sprintf( cBuffer,"%s:", striConfigPage3);   //
    strcat( cBuffer,striMaxkgsForTopUp);
    strcat( cBuffer, "\0" );
	d = iniparser_getdouble(ini, cBuffer, -1.0);

    if (d != -1)
    {
	    g_UploadedCalibrationData.m_fMaxBatchSizeInKg = d;
//		printf(" maxkg for top up :   [%f]\n", g_UploadedCalibrationData.m_fMaxBatchSizeInKg);
	}
   else
   {
//		printf("% max kg for top up not found\n");
   }


 // latency
    sprintf( cBuffer,"%s:", striConfigPage3);   //
    strcat( cBuffer,striLatency);
    strcat( cBuffer, "\0" );
	 i = iniparser_getint(ini, cBuffer, -1);

	if (i != -1)
    {
       g_UploadedCalibrationData.m_fLatency = ((float)i)/1000.0;   // (int)(d * 1000.0f);  //mmk store latency as ms on SD
//        SetupMttty();
//		printf("latency :   [%2.4f]\n", g_UploadedCalibrationData.m_fLatency);
	}
   else
   {
//		printf("% latency not found\n");
   }


   // periodic mixing time
    sprintf( cBuffer,"%s:", striConfigPage3);   //
    strcat( cBuffer,striPeriodicMixingTime);
    strcat( cBuffer, "\0" );
	i = iniparser_getint(ini, cBuffer, -1);
    if (i != -1)
    {
	    g_UploadedCalibrationData.m_nIntermittentMixTime = i;
//		printf(" periodic mixing time :   [%d]\n", g_UploadedCalibrationData.m_nIntermittentMixTime);
	}
   else
   {
//		printf(" periodic mixing time\n");
   }

// liquid additive
//
    sprintf( cBuffer,"%s:",striConfigPage3);
    strcat( cBuffer, striLiquidAdditive );
    strcat( cBuffer, "\0" );
	b = iniparser_getboolean(ini, cBuffer, -1);

	if (b != -1)
    {
	    g_UploadedCalibrationData.m_bLiquidEnabled = b;
    }


// recipe entry mode

    sprintf( cBuffer,"%s:", striConfigPage3);   //
    strcat( cBuffer,striRecipeEntry);
    strcat( cBuffer, "\0" );
	i = iniparser_getint(ini, cBuffer, -1);
    if (i != -1)
    {
	    g_UploadedCalibrationData.m_nRecipeEntryType = i;
//		printf(" recipe entry :   [%d]\n", g_UploadedCalibrationData.m_nRecipeEntryType);
	}
   else
   {
   }

// maximum component % limit
//

    sprintf( cBuffer,"%s:",striConfigPage3);
    strcat( cBuffer, striRegrindFillRetry );
    strcat( cBuffer, "\0" );
	b = iniparser_getboolean(ini, cBuffer, -1);

	if (b != -1)
    {
	    g_UploadedCalibrationData.m_bMaxComponentPercentageCheck = b;
//		printf("max compoent % limit:   [%d]\n", g_UploadedCalibrationData.m_bMaxComponentPercentageCheck);
    }
    else
    {
    }

// purge enable / disable
//
    sprintf( cBuffer,"%s:", striConfigPage3);   //
    strcat( cBuffer,striPurge);
    strcat( cBuffer, "\0" );
	i = iniparser_getint(ini, cBuffer, -1);
    if (i != -1)
    {
	    g_UploadedCalibrationData.m_nPurgeEnable = i;
//		printf(" purge :   [%d]\n", g_UploadedCalibrationData.m_nPurgeEnable);
	}
   else
   {
   }

// purge component no
    sprintf( cBuffer,"%s:", striConfigPage3);   //
    strcat( cBuffer,striPurgeComponentNumber);
    strcat( cBuffer, "\0" );
	i = iniparser_getint(ini, cBuffer, -1);
    if (i != -1)
    {
	    g_UploadedCalibrationData.m_nPurgeComponentNumber = i;
//		printf(" purge component no :   [%d]\n", g_UploadedCalibrationData.m_nPurgeComponentNumber);
	}

// flow rate check

    sprintf( cBuffer,"%s:", striConfigPage3);
    strcat( cBuffer, striFlowRateCheck);
    strcat( cBuffer, "\0" );
	b = iniparser_getboolean(ini, cBuffer, -1);

	if (b != -1)
    {
	    g_UploadedCalibrationData.m_bFlowRateCheck = b;
    }



// log to sd card
    sprintf( cBuffer,"%s:", striConfigPage3);   //
    strcat( cBuffer,striLogToSDCard);
    strcat( cBuffer, "\0" );
	i = iniparser_getint(ini, cBuffer, -1);
    if (i != -1)
    {
	    g_UploadedCalibrationData.m_nLogToSDCard = i;
	}

// control line speed page
//
// line speed constant.

    sprintf( cBuffer,"%s:", striControlLineSpeedPage);   //
    strcat( cBuffer,striLineSpeedConstant);
    strcat( cBuffer, "\0" );
	d = iniparser_getdouble(ini, cBuffer, -1.0);

    if (d != -1)
    {
	    g_UploadedCalibrationData.m_fLineSpeedConstant = d;
//		printf("line speed constant :   [%f]\n", g_UploadedCalibrationData.m_fLineSpeedConstant);
	}
   else
   {
//		printf("line speed constant not found\n");
   }

// max extruder setpoint

    sprintf( cBuffer,"%s:", striControlLineSpeedPage);   //
    strcat( cBuffer,striMaxExtruderSetpoint);
    strcat( cBuffer, "\0" );
	d = iniparser_getdouble(ini, cBuffer, -1.0);

    if (d != -1)
    {
	    g_UploadedCalibrationData.m_fMaxExtruderSpeedPercentage = d;
//		printf("max extruder setpoint :   [%f]\n", g_UploadedCalibrationData.m_fMaxExtruderSpeedPercentage);
	}
   else
   {
//		printf("max extruder setpoint not found\n");
   }

    // line speed signal type
    sprintf( cBuffer,"%s:", striControlLineSpeedPage);   //
    strcat( cBuffer,striLineSpeedSignalType);
    strcat( cBuffer, "\0" );
	i = iniparser_getint(ini, cBuffer, -1);
    if (i != -1)
    {
	    g_UploadedCalibrationData.m_nLineSpeedSignalType = i;
//		printf("line speed signal type :   [%d]\n", g_UploadedCalibrationData.m_nLineSpeedSignalType);
	}


// line speed average
//
    sprintf( cBuffer,"%s:", striControlLineSpeedPage);   //
    strcat( cBuffer,striLineSpeedAverage);
    strcat( cBuffer, "\0" );
	i = iniparser_getint(ini, cBuffer, -1);
    if (i != -1)
    {
	    g_UploadedCalibrationData.m_nLineSpeedAverage = i;
	}

// screw speed signal type
//
    sprintf( cBuffer,"%s:", striControlLineSpeedPage);   //
    strcat( cBuffer,striScrewSpeedSignalType);
    strcat( cBuffer, "\0" );
	i = iniparser_getint(ini, cBuffer, -1);
    if (i != -1)
    {
	    g_UploadedCalibrationData.m_nScewSpeedSignalType = i;
	}



   // Measurement and control page
//
// control gain.
    // max kg for top up
    sprintf( cBuffer,"%s:", striControlMeasAndControlPage);   //
    strcat( cBuffer,striControlGain);
    strcat( cBuffer, "\0" );
	d = iniparser_getdouble(ini, cBuffer, -1.0);

    if (d != -1)
    {
	    g_UploadedCalibrationData.m_fControlGain = d;
//		printf(" control gain :   [%f]\n", g_UploadedCalibrationData.m_fControlGain);
	}
   else
   {
//		printf("% control gain not found\n");
   }

// control averaging

    sprintf( cBuffer,"%s:", striControlMeasAndControlPage);   //
    strcat( cBuffer,striControlAveraging);
    strcat( cBuffer, "\0" );
	i = iniparser_getint(ini, cBuffer, -1);
    if (i != -1)
    {
	    g_UploadedCalibrationData.m_nControlAvgFactor = i;
//		printf("control averaging:   [%d]\n", g_UploadedCalibrationData.m_nControlAvgFactor);
	}

// fast control response
    sprintf( cBuffer,"%s:", striControlMeasAndControlPage);   //
    strcat( cBuffer,striFastControlResponse);
    strcat( cBuffer, "\0" );
	b = iniparser_getboolean(ini, cBuffer, -1);

	if (b != -1)
    {
	    g_UploadedCalibrationData.m_bFastControlResponse = b;
//		printf("fast control response:   [%d]\n", g_UploadedCalibrationData.m_bFastControlResponse);
    }

// extruder starvation shutdown

    sprintf( cBuffer,"%s:", striControlMeasAndControlPage);   //
    strcat( cBuffer,striExtruderStarvationShutdown);
    strcat( cBuffer, "\0" );
	b = iniparser_getboolean(ini, cBuffer, -1);

	if (b != -1)
    {
	    g_UploadedCalibrationData.m_bShutDownFlag = b;
//		printf("extruder starvation shutdown:   [%d]\n", g_UploadedCalibrationData.m_bShutDownFlag);
    }

 // extruder shutdown time
    sprintf( cBuffer,"%s:", striControlMeasAndControlPage);   //
    strcat( cBuffer,striExtruderShutdownTime);
    strcat( cBuffer, "\0" );
	i = iniparser_getint(ini, cBuffer, -1);
    if (i != -1)
    {
	    g_UploadedCalibrationData.m_nShutDownTime = i;
//		printf("shut down time:   [%d]\n", g_UploadedCalibrationData.m_nShutDownTime);
	}


  // control method

    sprintf( cBuffer,"%s:", striControlMeasAndControlPage);   //
    strcat( cBuffer,striControlMethod);
    strcat( cBuffer, "\0" );
	i = iniparser_getint(ini, cBuffer, -1);
    if (i != -1)
    {
	    g_UploadedCalibrationData.m_nControlMode = i;
//		printf("control method:   [%d]\n", g_UploadedCalibrationData.m_nControlMode);
    }


   // correct every cycle

    sprintf( cBuffer,"%s:", striControlMeasAndControlPage);   //
    strcat( cBuffer,striCorrectEveryCycle);
    strcat( cBuffer, "\0" );
	b = iniparser_getboolean(ini, cBuffer, -1);

	if (b != -1)
    {
	    g_UploadedCalibrationData.m_bCorrectEveryNCycles = b;
//		printf("correct every cycle   [%d]\n", g_UploadedCalibrationData.m_bCorrectEveryNCycles);
    }

// throughput deadband

    sprintf( cBuffer,"%s:", striControlMeasAndControlPage);   //
    strcat( cBuffer,striThroughputDeadband);
    strcat( cBuffer, "\0" );
	i = iniparser_getint(ini, cBuffer, -1);
    if (i != -1)
    {
	    g_UploadedCalibrationData.m_nControlPercentageLimit = i;
//        SetupMttty();
//		printf("throughput deadband:   [%d]\n", g_UploadedCalibrationData.m_nControlPercentageLimit);
	}


// control type

    sprintf( cBuffer,"%s:", striControlMeasAndControlPage);   //
    strcat( cBuffer,striControlType);
    strcat( cBuffer, "\0" );
	i = iniparser_getint(ini, cBuffer, -1);
    if (i != -1)
    {
	    g_UploadedCalibrationData.m_nControlType = i;
//		printf("control type:   [%d]\n", g_UploadedCalibrationData.m_nControlType);
	}


// Cleaning page

// top on time
    sprintf( cBuffer,"%s:", striCleaningPage);   //
    strcat( cBuffer,striTopOnTime);
    strcat( cBuffer, "\0" );
	d = iniparser_getdouble(ini, cBuffer, -1.0);

    if (d != -1)
    {
	    g_UploadedCalibrationData.m_nPhase1OnTime = (int) (d *10.0f);
//		printf("top on time:   [%d]\n", g_UploadedCalibrationData.m_nPhase1OnTime);
	}

// top off time
    sprintf( cBuffer,"%s:", striCleaningPage);   //
    strcat( cBuffer,striTopOffTime);
    strcat( cBuffer, "\0" );
	d = iniparser_getdouble(ini, cBuffer, -1.0);

    if (d != -1)
    {
	    g_UploadedCalibrationData.m_nPhase1OffTime = (int) (d *10.0f);
//		printf("top off time:   [%d]\n", g_UploadedCalibrationData.m_nPhase1OffTime);
	}

// top repetitions
    sprintf( cBuffer,"%s:", striCleaningPage);   //
    strcat( cBuffer,striTopRepetitions);
    strcat( cBuffer, "\0" );
	i = iniparser_getint(ini, cBuffer, -1);
    if (i != -1)
    {
	    g_UploadedCalibrationData.m_nCleanRepetitions = i;
//		printf("top repetitions:   [%d]\n", g_UploadedCalibrationData.m_nCleanRepetitions);
	}

// bottom on time
    sprintf( cBuffer,"%s:", striCleaningPage);   //
    strcat( cBuffer,striBottomOnTime);
    strcat( cBuffer, "\0" );
	d = iniparser_getdouble(ini, cBuffer, -1.0);

    if (d != -1)
    {
	    g_UploadedCalibrationData.m_nPhase2OnTime = (int) (d *10.0f);
//		printf("bottom on time:   [%d]\n", g_UploadedCalibrationData.m_nPhase2OnTime);
	}

// bottom off time
    sprintf( cBuffer,"%s:", striCleaningPage);   //
    strcat( cBuffer,striBottomOffTime);
    strcat( cBuffer, "\0" );
	d = iniparser_getdouble(ini, cBuffer, -1.0);

    if (d != -1)
    {
	    g_UploadedCalibrationData.m_nPhase2OffTime = (int) (d *10.0f);
//		printf("bottom off time:   [%d]\n", g_UploadedCalibrationData.m_nPhase2OffTime);
	}

// mixer clean repetitions
    sprintf( cBuffer,"%s:", striCleaningPage);   //
    strcat( cBuffer,striBottomRepetitions);
    strcat( cBuffer, "\0" );
	i = iniparser_getint(ini, cBuffer, -1);
    if (i != -1)
    {
	    g_UploadedCalibrationData.m_nMixerCleanRepetitions = i;
//		printf("mixer clean repetitions:   [%d]\n", g_UploadedCalibrationData.m_nMixerCleanRepetitions);
	}


// bypass time limit

    sprintf( cBuffer,"%s:", striCleaningPage);   //
    strcat( cBuffer,striBypassTimeLimit);
    strcat( cBuffer, "\0" );
	i = iniparser_getint(ini, cBuffer, -1);
    if (i != -1)
    {
	    g_UploadedCalibrationData.m_nBypassTimeLimit = i;
//		printf("bypass time limit:   [%d]\n", g_UploadedCalibrationData.m_nBypassTimeLimit);
	}

// blow every cycle

    sprintf( cBuffer,"%s:", striCleaningPage);   //
    strcat( cBuffer,striBlowEveryCycle);
    strcat( cBuffer, "\0" );
	b = iniparser_getboolean(ini, cBuffer, -1);

	if (b != -1)
    {
	    g_UploadedCalibrationData.m_bBlowEveryCycle = b;
//		printf("blow every cycle   [%d]\n", g_UploadedCalibrationData.m_bBlowEveryCycle);
    }

// bypass activation time

    sprintf( cBuffer,"%s:", striCleaningPage);   //
    strcat( cBuffer,striBypassActivation);
    strcat( cBuffer, "\0" );
	i = iniparser_getint(ini, cBuffer, -1);
    if (i != -1)
    {
	    g_UploadedCalibrationData.m_nBypassTimeLimitClean = i;
//		printf("bypass activation time:   [%d]\n", g_UploadedCalibrationData.m_nBypassTimeLimitClean);
	}


// usb device type

    sprintf( cBuffer,"%s:", striUSBConfigPage);   //
    strcat( cBuffer,strUSBEnable);
    strcat( cBuffer, "\0" );
	i = iniparser_getint(ini, cBuffer, -1);
    if (i != -1)
    {
	    g_UploadedCalibrationData.m_nDumpDeviceType = i;
	}

// alarm dump
    sprintf( cBuffer,"%s:", striUSBConfigPage);   // strWeightDisplayUnits[g_MeasurementUnits.m_nWeightUnits]
    strcat( cBuffer, strUSBPrinterAlarmEnable );
    strcat( cBuffer, "\0" );
	b = iniparser_getboolean(ini, cBuffer, -1);
	if (b!=-1)
    {
	    g_UploadedCalibrationData.m_bDoubleDump = b;
    }

// order dump
    sprintf( cBuffer,"%s:", striUSBConfigPage);   // strWeightDisplayUnits[g_MeasurementUnits.m_nWeightUnits]
    strcat( cBuffer, strUSBPrinterOrderEnable );
    strcat( cBuffer, "\0" );
	b = iniparser_getboolean(ini, cBuffer, -1);
	if (b!=-1)
    {
	    g_UploadedCalibrationData.m_bOrderReportDumpFlag = b;
    }

// roll report dump
    sprintf( cBuffer,"%s:", striUSBConfigPage);   // strWeightDisplayUnits[g_MeasurementUnits.m_nWeightUnits]
    strcat( cBuffer, strUSBPrinterRollEnable );
    strcat( cBuffer, "\0" );
	b = iniparser_getboolean(ini, cBuffer, -1);
	if (b!=-1)
    {
	    g_UploadedCalibrationData.m_bRollReportDumpFlag = b;
    }

// hour report dump
    sprintf( cBuffer,"%s:", striUSBConfigPage);   // strWeightDisplayUnits[g_MeasurementUnits.m_nWeightUnits]
    strcat( cBuffer, strUSBPrinterHourEnable );
    strcat( cBuffer, "\0" );
	b = iniparser_getboolean(ini, cBuffer, -1);
	if (b!=-1)
    {
	    g_UploadedCalibrationData.m_bHourlyDumpFlag = b;
    }
// batch log dump
    sprintf( cBuffer,"%s:", striUSBConfigPage);   // strWeightDisplayUnits[g_MeasurementUnits.m_nWeightUnits]
    strcat( cBuffer, strUSBPrinterBatchEnable );
    strcat( cBuffer, "\0" );
	b = iniparser_getboolean(ini, cBuffer, -1);
	if (b!=-1)
    {
	    g_UploadedCalibrationData.m_bBatchLogDumpFlag = b;
    }

// event log
    sprintf( cBuffer,"%s:", striUSBConfigPage);   // strWeightDisplayUnits[g_MeasurementUnits.m_nWeightUnits]
    strcat( cBuffer, strUSBPrinterEventEnable );
    strcat( cBuffer, "\0" );
	b = iniparser_getboolean(ini, cBuffer, -1);
	if (b!=-1)
    {
	    g_UploadedCalibrationData.m_bEventLogDumpFlag = b;
    }

// history log
    sprintf( cBuffer,"%s:", striUSBConfigPage);   // strWeightDisplayUnits[g_MeasurementUnits.m_nWeightUnits]
    strcat( cBuffer, strUSBPrinterHistoryEnable );
    strcat( cBuffer, "\0" );
	b = iniparser_getboolean(ini, cBuffer, -1);
	if (b!=-1)
    {
	    g_UploadedCalibrationData.m_bHistoryLogDumpFlag = b;
    }
// monthly log
    sprintf( cBuffer,"%s:", striUSBConfigPage);   // strWeightDisplayUnits[g_MeasurementUnits.m_nWeightUnits]
    strcat( cBuffer, strUSBPrinterMonthlyEnable );
    strcat( cBuffer, "\0" );
	b = iniparser_getboolean(ini, cBuffer, -1);
	if (b!=-1)
    {
	    g_UploadedCalibrationData.m_bMonthlyDumpFlag = b;
    }

// shift log
    sprintf( cBuffer,"%s:", striUSBConfigPage);   // strWeightDisplayUnits[g_MeasurementUnits.m_nWeightUnits]
    strcat( cBuffer, strUSBPrinterShiftEnable );
    strcat( cBuffer, "\0" );
	b = iniparser_getboolean(ini, cBuffer, -1);
	if (b!=-1)
    {
	    g_UploadedCalibrationData.m_bShiftReportDumpFlag = b;
    }

	 g_CalibrationData = g_UploadedCalibrationData;
     g_bFillTargetEntryFlag = TRUE;  // calculate component target weight
     g_bSaveAllCalibrationToEEprom = TRUE;

   // vacuum loading

    // no of loaders
    sprintf( cBuffer,"%s:", striVacuumLoadingPage);   //
    strcat( cBuffer,striNumberOfLoaders);
    strcat( cBuffer, "\0" );
	i = iniparser_getint(ini, cBuffer, -1);
	if (i!= -1)
    {
	    g_UploadedVacuumLoadingData.m_nLoaders = i;
	}
   // pump idle time
    sprintf( cBuffer,"%s:", striVacuumLoadingPage);   //
    strcat( cBuffer,striIdleTime);
    strcat( cBuffer, "\0" );
	i = iniparser_getint(ini, cBuffer, -1);
	if (i!= -1)
    {
	    g_UploadedVacuumLoadingData.m_nPumpIdleTime = i;
	}

   // pump start time
    sprintf( cBuffer,"%s:", striVacuumLoadingPage);   //
    strcat( cBuffer,striPumpStartTime);
    strcat( cBuffer, "\0" );
	i = iniparser_getint(ini, cBuffer, -1);
	if (i!= -1)
    {
	    g_UploadedVacuumLoadingData.m_nPumpStartTime = i;
	}

 // atm valve overlap
    sprintf( cBuffer,"%s:", striVacuumLoadingPage);   //
    strcat( cBuffer,striAtmValveOverlap);
    strcat( cBuffer, "\0" );
	i = iniparser_getint(ini, cBuffer, -1);
	if (i!= -1)
    {
	    g_UploadedVacuumLoadingData.m_nAVOverLapTime = i;
	}

// filter repetitions
    sprintf( cBuffer,"%s:", striVacuumLoadingPage);   //
    strcat( cBuffer,striFilterRepetitions);
    strcat( cBuffer, "\0" );
	i = iniparser_getint(ini, cBuffer, -1);
	if (i!= -1)
    {
	    g_UploadedVacuumLoadingData.m_nFilterRepetitions = i;
	}
// filter type
    sprintf( cBuffer,"%s:", striVacuumLoadingPage);   //
    strcat( cBuffer,striFilterType);
    strcat( cBuffer, "\0" );
	i = iniparser_getint(ini, cBuffer, -1);
	if (i!= -1)
    {
	    g_UploadedVacuumLoadingData.m_nFilterType = i;
	}

// dump delay
    sprintf( cBuffer,"%s:", striVacuumLoadingPage);   //
    strcat( cBuffer,striDumpDelay);
    strcat( cBuffer, "\0" );
	i = iniparser_getint(ini, cBuffer, -1);
	if (i!= -1)
    {
	    g_UploadedVacuumLoadingData.m_nDischargeTime = i;
	}
	// individual loader data
    for(j=0; j<g_UploadedVacuumLoadingData.m_nLoaders; j++)
    {
        // on off status
        sprintf( cBuffer,"%s%d:", striIndividualLoaderPage,j+1);   //
        strcat( cBuffer,striLoaderOnOffStatus);
        strcat( cBuffer, "\0" );
        i = iniparser_getint(ini, cBuffer, -1);
        if (i!= -1)
        {
	        g_UploadedVacuumLoadingData.m_bOnFlag[j] = i;
        }

        // sunction time
        sprintf( cBuffer,"%s%d:", striIndividualLoaderPage,j+1);   //
        strcat( cBuffer,striLoaderSuctionTime);
        strcat( cBuffer, "\0" );
        i = iniparser_getint(ini, cBuffer, -1);
        if (i!= -1)
        {
            g_UploadedVacuumLoadingData.m_nSuctionTime[j] = i;
        }
        // post fill
        sprintf( cBuffer,"%s%d:", striIndividualLoaderPage,j+1);   //
        strcat( cBuffer,striLoaderPostFillTime);
        strcat( cBuffer, "\0" );
        i = iniparser_getint(ini, cBuffer, -1);
        if (i!= -1)
        {
            g_UploadedVacuumLoadingData.m_nPostFillTime[j] = i;
        }
        // priority
        sprintf( cBuffer,"%s%d:", striIndividualLoaderPage,j+1);   //
        strcat( cBuffer,striPriority);
        strcat( cBuffer, "\0" );
        i = iniparser_getint(ini, cBuffer, -1);
        if (i!= -1)
        {
	        g_UploadedVacuumLoadingData.m_nPriority[j] = i;
        }
    }

     g_VacCalibrationData = g_UploadedVacuumLoadingData;
	 g_bSaveLoaderCalibrationData = TRUE;

    f_delvolume(MMC_DRV_NUM);

    ReInitialisePinsUsedBySDCard();

    g_bSPIBusBusy = FALSE;       // free up the SPI for other uses.

	return 0 ;
}

