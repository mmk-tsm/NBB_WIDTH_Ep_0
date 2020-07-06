/*-------------------------------------------------------------------
 File: CopyCalibrationDataToIniFile.cpp
 Description: SD card configuration functions.

   P.Smith
    Started.

   Michael McKiernan                                    20/6/07
      Edited WriteBatchIniFile()

  P.Smith                                                28/06/07
  ensure that spi busy flag is cleared on exiting the write ini function.
  call f_delvolume(MMC_DRV_NUM) & ReInitialisePinsUsedBySDCard() also

  P.Smith                                              23/10/07
  CreateDefaultIniFile added to create a default ini file if it is not present.
  add high low regrind, machine name,moulding
  Name change to cleaning m_nCleaning, top up reference
  WriteBatchIniFile is fully implemented

  P.Smith                                              10/1/08
  add WriteCalibrationDataToSDCard as a single file

//
// P.Smith                          30/1/08
// correct compiler warnings
//
// P.Smith                          27/3/08
// name change to config file, cf + id
//
// P.Smith                          10/6/08
// set g_bSDCardSaveConfigComplete to TRUE.
//
// P.Smith                          10/6/08
// Copy calibration file to ini file
// continuous download,Telnet,vacuum loading, alpha numeric,pause on power up
// deviation alarm,log to sd,line speed average,screw speed signal type,
// sensitivity, purge, purge component no
// Correct m_nControlPercentageLimit
//
// P.Smith                          16/7/08
// Remove double hopper double sprintf to sd card
// rename config file to config.txt
//
// P.Smith                          23/7/08
// correct CreateDefaultIniFile, ensure that first data is sprintf
//
// P.Smith                          24/7/08
// only allow WriteCalibrationDataToSDCard to proceed if g_nDisableSDCardAccessCtr is 0
// this allows the sd card to be taken out without being written to.
//
// P.Smith                          3/10/08
// copy liquid additive to config file.
//
// P.Smith                          16/10/08
// todo to lp
//
// P.Smith                          20/11/08
// add text for config.txt file
// blender type, blender mode, offline mode, stage filling,
// units, remote modules, cleaning, recipe entry mode, line speed signal type
// screw speed signal type, control method
//
// P.Smith                          25/11/08
// added GetControlTypeText
//
//
// P.Smith                          26/3/09
// problem with  remote modules copy,  dictionary_set was not called after the string was formatted
//
// P.Smith                          2/4/09
// correct remote modules copy to sd card.
//
// P.Smith                          16/4/09
// correct cycle speed,this was shown as page 3 on the config.
// changed to page 2
// add parity yes / no
// correct addition of sensitivity
// added sensitivity text function call to display text high / low
// add purge  text
// add usb config
// add text for diagnostics
//
// P.Smith                          30/6/09
// added copy of vacuum loading data to config on sd card
//
// P.Smith                          2/7/09
// name change dump delay to discharge time
//
// P.Smith                          17/9/09
// change copy of log to sd card, this is no longer a bool as several
// options are required.
//
// P.Smith                          29/9/09
// added DeleteDefaultFile, this function basically deletes the default.env file
//
// P.Smith                          30/11/09
// call ReInitialiseGPIOPinsUsedBySDCard after initialisation of sd card.
//
// P.Smith                          7/12/09
// added vac8 expansion, min opening time, flow rate check
//
// M.McKiernan                      1/12/09
// added predef.h
//
// P.Smith                          18/1/2010
// added speed text normal, fast to log.
//
// P.Smith                          7/4/2010
// added watch dog timer to sd card log
// for cycle speed, just print out the value of  g_CalibrationData.m_bCycleFastFlag
// was printing out yes /no before.
//
// P.Smith                          13/4/2010
//  Remove Mixing screw detection, correct high low regrind so that it is recorded.
//  Remove max component percentage alarm
//
// P.Smith                          4/5/10
// add baud rate to configuration
 -----------------------------------------------------------------*/
#include "predef.h"				//Eclipse

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
#include "Sdcard.h"
#include <ucosmcfc.h>
#include "Batvars2.h"
#include "Gettext.h"

#include "System.h"
#include <ucos.h>
#include <constants.h>
#include "VacVars.h"


extern CalDataStruct    g_CalibrationData;
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
extern   char      *striNonStdConfig;


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
extern   char      *striDecreaseRate;
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
extern   char      *striYes;
extern   char      *striNo;
extern   char      *striMaterialTransportDelay;
extern   char      *striMaterialDumpDelay;
extern   char      *striHighLowRegrind;
extern   char      *striMoulding;
extern   char      *striMachineName;
extern   BYTE DS2432Buffer[];

extern  char       *striContinuousDownload;
extern  char       *striTelnet;
extern  char       *striVacuumLoading;
extern  char       *striAlphaNumeric;
extern  char       *striPauseBlenderOnPowerUp;
extern  char       *striDeviationAlarmEnable;
extern  char       *striLogToSDCard;
extern  char       *striLineSpeedAverage;
extern  char       *striScrewSpeedSignalType;
extern   char      *striSensitivity;
extern   char      *striPurge;
extern   char      *striPurgeComponentNumber;
extern   char      *striLiquidAdditive;
extern   char      *striParity;


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
extern   structVacCalib   g_VacCalibrationData;

extern  char      *striMinOpeningTimeCheck;
extern  char      *striFlowRateCheck;
extern  char      *striVAC8Expansion;
extern  char      *striWatchDog;
extern  char      *striBaudRate;


extern bool g_bWatchdogStatus;






void CreateDefaultIniFile (void )

{
	char	cBuffer[80],fBuffer[1000];
	unsigned int i;

 g_bWriteToIniFileAgain = FALSE;
	F_FILE * fp;

    sprintf( fBuffer,"[%s]\n\n", striHopperWeightPage);

    sprintf( cBuffer,"[%s]\n\n", striConfigPage1);
    strcat( fBuffer, cBuffer );

    sprintf( cBuffer,"[%s]\n\n", striConfigPage2);
    strcat( fBuffer, cBuffer );

    sprintf( cBuffer,"[%s]\n\n", striNonStdConfig);
    strcat( fBuffer, cBuffer );


    sprintf( cBuffer,"[%s]\n\n", striConfigPage3);
    strcat( fBuffer, cBuffer );

    sprintf( cBuffer,"[%s]\n\n", striControlLineSpeedPage);
    strcat( fBuffer, cBuffer );

    sprintf( cBuffer,"[%s]\n\n", striControlMeasAndControlPage);
    strcat( fBuffer, cBuffer );

    sprintf( cBuffer,"[%s]\n\n", striCleaningPage);
    strcat( fBuffer, cBuffer );

    sprintf( cBuffer,"[%s]\n\n", striUSBConfigPage);
    strcat( fBuffer, cBuffer );

    if(g_VacCalibrationData.m_nLoaders >0)
    {
        sprintf( cBuffer,"[%s]\n\n", striVacuumLoadingPage);
        strcat( fBuffer, cBuffer );
    }
    for(i=0; i<g_VacCalibrationData.m_nLoaders; i++)
    {
        sprintf( cBuffer,"[%s%d]\n\n", striIndividualLoaderPage,i+1);
        strcat( fBuffer, cBuffer );
    }


    fp = f_open("default.ini", "w" );
//    SetupMttty();
//    iprintf("%s",fBuffer);

    if(fp)
    {
    f_write(fBuffer,strlen(fBuffer),1,fp);
    g_bWriteToIniFileAgain = TRUE;
    }
    f_close(fp);

}


//////////////////////////////////////////////////////
// DeleteDefaultFile( void )
// deletes default.env file.
//////////////////////////////////////////////////////

void DeleteDefaultFile( void )
{
    g_bSPIBusBusy = TRUE;       // Prevent the SPI use for other uses.
    InitialiseSDCard();         // note this includes SPI_Select_SD.
    f_delete("default.ini");
    f_delvolume(MMC_DRV_NUM);
    ReInitialisePinsUsedBySDCard();
    g_bSPIBusBusy = FALSE;       // free up the SPI for other uses.
}

/* WriteBatchIniFile() writes all calibration data to a file.
Output file current used = CalibOut.ini.
The ini_name in the function parameters is the template it uses for output.
The function currently has test data used for test purposes - commented out.    */
int WriteBatchIniFile(char * ini_name)
{
	dictionary	*	ini ;
	unsigned int i;
	char	cval[90] ;
	char	cBuffer[80],cTemp[80];

	/* Some temporary variables to hold query results */

    g_bSPIBusBusy = TRUE;       // Prevent the SPI use for other uses.

	 InitialiseSDCard();         // note this includes SPI_Select_SD.
     ReInitialiseGPIOPinsUsedBySDCard();


	ini = iniparser_load(ini_name);
	if (ini==NULL)
    {
      CreateDefaultIniFile();
      f_delvolume(MMC_DRV_NUM);
      ReInitialisePinsUsedBySDCard();
      g_bSPIBusBusy = FALSE;       // free up the SPI for other uses.

		return -1 ;
	}

// Calibration Wt. constant.
    sprintf( cBuffer,"%s:", striHopperWeightPage);
    strcat( cBuffer, striCalibrationWeightConstant );
    sprintf(cval, "%6.2f", g_CalibrationData.m_fWeightConstant );
    dictionary_set(ini, cBuffer, cval);

// Fill target weight.
    sprintf( cBuffer,"%s:", striHopperWeightPage);
    strcat( cBuffer, striFillTargetWeight );
    sprintf(cval, "%3.1f", g_CalibrationData.m_fBatchSize );
    dictionary_set(ini, cBuffer, cval);

//Weight Averaging.
    sprintf( cBuffer,"%s:", striHopperWeightPage);
    strcat( cBuffer, striWeightAveraging );
    sprintf(cval, "%1d", g_CalibrationData.m_nWeightAvgFactor );
    dictionary_set(ini, cBuffer, cval);
//    iniparser_dump(d, stderr);

// double dump
    sprintf( cBuffer,"%s:", striHopperWeightPage);
    strcat( cBuffer, striDoubleDump );
//    bBoolean = g_CalibrationData.m_bDoubleDump;
    if( g_CalibrationData.m_bDoubleDump )
    {
        sprintf(cval,"%s",striYes);
    }
    else
    {
        sprintf(cval,"%s",striNo);
    }
    dictionary_set(ini, cBuffer, cval);

// Tare value
    sprintf( cBuffer,"%s:", striHopperWeightPage);   //
    strcat( cBuffer,striTare);
    sprintf(cval, "%ld", g_CalibrationData.m_lTareCounts );
    dictionary_set(ini, cBuffer, cval);

// Blender type

    sprintf( cBuffer,"%s:", striConfigPage1);   //
    strcat( cBuffer,striBlenderType);
    char    *cTempText;
    cTempText = GetBlenderTypeText(g_CalibrationData.m_nBlenderType);
    sprintf(cval, "%d  #%s", g_CalibrationData.m_nBlenderType,cTempText );
    dictionary_set(ini, cBuffer, cval);

// Blender mode

    sprintf( cBuffer,"%s:", striConfigPage1);   //
    strcat( cBuffer,striBlenderMode);
    cTempText = GetBlenderModeText(g_CalibrationData.m_nBlenderMode);
    sprintf(cval, "%d  #%s", g_CalibrationData.m_nBlenderMode,cTempText);
    dictionary_set(ini, cBuffer, cval);

// No of blends
    sprintf( cBuffer,"%s:", striConfigPage1);   //
    strcat( cBuffer,striNumberOfBlends);
    sprintf(cval, "%d", g_CalibrationData.m_nComponents );
    dictionary_set(ini, cBuffer, cval);

// Mixing screw detection
/*    sprintf( cBuffer,"%s:", striConfigPage1);
    strcat( cBuffer, striMixingScrewDetection );
    if( g_CalibrationData.m_bMixerScrewDetect )
    {
        sprintf(cval,"%s",striYes);
    }
    else
    {
        sprintf(cval,"%s",striNo);
    }
    dictionary_set(ini, cBuffer, cval);
*/

//  11.6.07
// offline mode
    sprintf( cBuffer,"%s:", striConfigPage1);   //
    strcat( cBuffer,striOfflineMode);

    cTempText = GetOfflineTypeText(g_CalibrationData.m_nOfflineType);
    sprintf(cval, "%d  #%s", g_CalibrationData.m_nOfflineType,cTempText);
    dictionary_set(ini, cBuffer, cval);

// fill retry
    sprintf( cBuffer,"%s:", striConfigPage1);   //
    strcat( cBuffer,striFillRetry);
    sprintf(cval, "%d", g_CalibrationData.m_nFillRetries );
    dictionary_set(ini, cBuffer, cval);

// First component compensation
    sprintf( cBuffer,"%s:", striConfigPage1);
    strcat( cBuffer, striFirstComponentCompensation );
    if( g_CalibrationData.m_bFirstComponentCompensation )
    {
        sprintf(cval,"%s",striYes);
    }
    else
    {
        sprintf(cval,"%s",striNo);
    }
    dictionary_set(ini, cBuffer, cval);

// high low regrind
//
    sprintf( cBuffer,"%s:", striConfigPage1);   //
    strcat( cBuffer,striHighLowRegrind);
    sprintf(cval, "%d", g_CalibrationData.m_bHighLoRegrindOption );
    dictionary_set(ini, cBuffer, cval);


// multi stage fill
    sprintf( cBuffer,"%s:", striConfigPage1);   //
    strcat( cBuffer,striMultiStagedFilling);
    cTempText = GetMultiStageFillText(g_CalibrationData.m_nStageFillEn);
    sprintf(cval, "%d #%s", g_CalibrationData.m_nStageFillEn,cTempText );
    dictionary_set(ini, cBuffer, cval);

// batches in kg/h average

    sprintf( cBuffer,"%s:", striConfigPage1);   //
    strcat( cBuffer,striBatchesInKghAverage);
    sprintf(cval, "%d", g_CalibrationData.m_nBatchesInKghAvg );
    dictionary_set(ini, cBuffer, cval);

// Machine Name

    sprintf( cBuffer,"%s:", striConfigPage1);   //
    strcat( cBuffer,striMachineName);

    sprintf(cval, "%s", g_CalibrationData.m_cEmailBlenderName );  //nbb--todolp-- correct this
    dictionary_set(ini, cBuffer, cval);

// Moulding
    sprintf( cBuffer,"%s:", striConfigPage1);   //
    strcat( cBuffer,striMoulding);
    if( g_CalibrationData.m_bMouldingOption )
    {
        sprintf(cval,"%s",striYes);
    }
    else
    {
        sprintf(cval,"%s",striNo);
    }
    dictionary_set(ini, cBuffer, cval);

// continuous download

   sprintf( cBuffer,"%s:", striConfigPage1);   //
    strcat( cBuffer,striContinuousDownload);
    if( g_CalibrationData.m_bAllowForContinuousDownload )
    {
        sprintf(cval,"%s",striYes);
    }
    else
    {
        sprintf(cval,"%s",striNo);
    }
    dictionary_set(ini, cBuffer, cval);

//   Telnet

   sprintf( cBuffer,"%s:", striConfigPage1);   //
    strcat( cBuffer,striTelnet);
    if( g_CalibrationData.m_bTelnet )
    {
        sprintf(cval,"%s",striYes);
    }
    else
    {
        sprintf(cval,"%s",striNo);
    }
    dictionary_set(ini, cBuffer, cval);

// vacuum loading

    sprintf( cBuffer,"%s:", striConfigPage1);   //
    strcat( cBuffer,striVacuumLoading);
    if( g_CalibrationData.m_bVacLoading )
    {
        sprintf(cval,"%s",striYes);
    }
    else
    {
        sprintf(cval,"%s",striNo);
    }
    dictionary_set(ini, cBuffer, cval);

// parity

    sprintf( cBuffer,"%s:", striConfigPage1);   //
    strcat( cBuffer,striParity);
    if(g_CalibrationData.m_bNoParity )
    {
        sprintf(cval,"%s",striYes);
    }
    else
    {
        sprintf(cval,"%s",striNo);
    }
    cTempText = GetParityText(g_CalibrationData.m_bNoParity);
    sprintf(cTemp, " #%s",cTempText );
    strcat(cval,cTemp);
    dictionary_set(ini, cBuffer, cval);

    // baud rate


    sprintf( cBuffer,"%s:", striConfigPage1);   //
    strcat( cBuffer,striBaudRate);

    cTempText = GetBaudRateText(g_CalibrationData.m_nBaudRate);
    sprintf(cval, "%d  #%s", g_CalibrationData.m_nBaudRate,cTempText );
    dictionary_set(ini, cBuffer, cval);

// vac8 expansion

    sprintf( cBuffer,"%s:", striConfigPage1);   //
    strcat( cBuffer,striVAC8Expansion);
    if( g_CalibrationData.m_bVac8Expansion )
    {
        sprintf(cval,"%s",striYes);
    }
    else
    {
        sprintf(cval,"%s",striNo);
    }
    dictionary_set(ini, cBuffer, cval);

// min open time check

    sprintf( cBuffer,"%s:", striConfigPage1);   //
    strcat( cBuffer,striMinOpeningTimeCheck);
    if( g_CalibrationData.m_bMinimumOpeningTimeCheck )
    {
        sprintf(cval,"%s",striYes);
    }
    else
    {
        sprintf(cval,"%s",striNo);
    }
    dictionary_set(ini, cBuffer, cval);


//


// CONFIG PAGE 2.
//......................Testonly Data..........................
/*
   g_CalibrationData.m_nUnitType = 1;
   g_CalibrationData.m_nPeripheralCardsPresent = 2;
   g_CalibrationData.m_bStandardCompConfig = TRUE;
   g_CalibrationData.m_nMixingTime = 23;
   g_CalibrationData.m_nMixShutoffTime = 24;
   g_CalibrationData.m_nDiagnosticsMode = 19;
   g_CalibrationData.m_nLevelSensorDelay = 18;
   g_CalibrationData.m_bVolumetricModeFlag = TRUE;
   g_CalibrationData.m_bCycleFastFlag = TRUE;
   g_CalibrationData.m_nMaxThroughput = 177;
   g_CalibrationData.m_nMaterialTransportDelay = 35;
   g_CalibrationData.m_nPostMaterialTransportDelay = 37;
*/
// UNITS
    sprintf( cBuffer,"%s:", striConfigPage2);   //
    strcat( cBuffer,striBlenderUits);
    cTempText = GetUnitsText(g_CalibrationData.m_nUnitType);
    sprintf(cval, "%d  #%s", g_CalibrationData.m_nUnitType,cTempText );
    dictionary_set(ini, cBuffer, cval);

// remote modules
    sprintf( cBuffer,"%s:", striConfigPage2);   //
    strcat( cBuffer,striRemoteModules);
    cTempText = GetRemoteModulesText(g_CalibrationData.m_nPeripheralCardsPresent);
    sprintf(cval, "%d  #%s", g_CalibrationData.m_nPeripheralCardsPresent,cTempText );
    dictionary_set(ini, cBuffer, cval);

 // Standard component config
    sprintf( cBuffer,"%s:", striConfigPage2);
    strcat( cBuffer, striStandardCompConfig );
    if( g_CalibrationData.m_bStandardCompConfig )
    {
        sprintf(cval,"%s",striYes);
    }
    else
    {
        sprintf(cval,"%s",striNo);
    }
    dictionary_set(ini, cBuffer, cval);

// offline mixing time
    sprintf( cBuffer,"%s:", striConfigPage2);   //
    strcat( cBuffer,striOfflineMixingTime);
    sprintf(cval, "%d", g_CalibrationData.m_nMixingTime );
    dictionary_set(ini, cBuffer, cval);

// mixing motor shut off time
    sprintf( cBuffer,"%s:", striConfigPage2);   //
    strcat( cBuffer,striMixingMotorShutOffTime);
    sprintf(cval, "%d", g_CalibrationData.m_nMixShutoffTime );
    dictionary_set(ini, cBuffer, cval);

// diagnostics
    sprintf( cBuffer,"%s:", striConfigPage2);   //
    strcat( cBuffer,striDiagnostics);
    cTempText = GetDiagnosticsText(g_CalibrationData.m_nDiagnosticsMode);
    sprintf(cval, "%d  #%s", g_CalibrationData.m_nDiagnosticsMode,cTempText );
    dictionary_set(ini, cBuffer, cval);

// level sensor alarm delay
    sprintf( cBuffer,"%s:", striConfigPage2);   //
    strcat( cBuffer,striLevelSensorAlarmDelay);
    sprintf(cval, "%d", g_CalibrationData.m_nLevelSensorDelay );
    dictionary_set(ini, cBuffer, cval);

// volumetric mode
    sprintf( cBuffer,"%s:", striConfigPage2);
    strcat( cBuffer, striVolumetricMode );
    if( g_CalibrationData.m_bVolumetricModeFlag )
    {
        sprintf(cval,"%s",striYes);
    }
    else
    {
        sprintf(cval,"%s",striNo);
    }
    dictionary_set(ini, cBuffer, cval);

// cycle speed
    sprintf( cBuffer,"%s:", striConfigPage2);   //
    strcat( cBuffer,striCycleSpeed);

    sprintf(cval, "%d", g_CalibrationData.m_bCycleFastFlag );    //
    dictionary_set(ini, cBuffer, cval);

    cTempText = GetSpeedText(g_CalibrationData.m_bCycleFastFlag);
    sprintf(cTemp, " #%s",cTempText );
    strcat(cval,cTemp);
    dictionary_set(ini, cBuffer, cval);

// Max kg/h throughput
    sprintf( cBuffer,"%s:", striConfigPage2);   //
    strcat( cBuffer,striMaxkghThroughput);
    sprintf(cval, "%d", g_CalibrationData.m_nMaxThroughput );
    dictionary_set(ini, cBuffer, cval);

// Material Transport delay
    sprintf( cBuffer,"%s:", striConfigPage2);   //
    strcat( cBuffer,striMaterialTransportDelay);
    sprintf(cval, "%d", g_CalibrationData.m_nMaterialTransportDelay );
    dictionary_set(ini, cBuffer, cval);


// Material Dump delay
    sprintf( cBuffer,"%s:", striConfigPage2);   //
    strcat( cBuffer,striMaterialDumpDelay);
    sprintf(cval, "%d", g_CalibrationData.m_nPostMaterialTransportDelay );
    dictionary_set(ini, cBuffer, cval);


// sensitivity

    sprintf( cBuffer,"%s:", striConfigPage2);   //
    strcat( cBuffer,striSensitivity);
    sprintf(cval, "%d", g_CalibrationData.m_bFillOrderManualSelect );
    if( g_CalibrationData.m_bFillOrderManualSelect )
    {
        sprintf(cval,"%s",striYes);
    }
    else
    {
        sprintf(cval,"%s",striNo);
    }
    cTempText = GetSensitivityText(g_CalibrationData.m_bFillOrderManualSelect);
    sprintf(cTemp, " #%s",cTempText );
    strcat(cval,cTemp);
    dictionary_set(ini, cBuffer, cval);

// deviation alarm
    sprintf( cBuffer,"%s:", striConfigPage2);   //
    strcat( cBuffer,striDeviationAlarmEnable);

    cTempText = GetDeviationAlarmEnableText(g_CalibrationData.m_nDeviationAlarmEnable);
    sprintf(cval, "%d  #%s", g_CalibrationData.m_nDeviationAlarmEnable,cTempText );
    dictionary_set(ini, cBuffer, cval);

// alpha numeric

    sprintf( cBuffer,"%s:", striConfigPage2);   //
    strcat( cBuffer,striAlphaNumeric);
    if( g_CalibrationData.m_bComponentNamingAlphanumeric )
    {
        sprintf(cval,"%s",striYes);
    }
    else
    {
        sprintf(cval,"%s",striNo);
    }
    dictionary_set(ini, cBuffer, cval);

// pause on power up

    sprintf( cBuffer,"%s:", striConfigPage2);   //
    strcat( cBuffer,striPauseBlenderOnPowerUp);
    if( g_CalibrationData.m_bPauseBlenderOnPowerup )
    {
        sprintf(cval,"%s",striYes);
    }
    else
    {
        sprintf(cval,"%s",striNo);
    }
    dictionary_set(ini, cBuffer, cval);

    // non standard config data

    for(i=0; i < g_CalibrationData.m_nComponents; i++)
    {
        sprintf( cBuffer,"%s:", striNonStdConfig);   //
        sprintf( cTemp,"Component %d",i+1);
        strcat(cBuffer,cTemp);
        sprintf(cval, "%d", g_CalibrationData.m_nCompConfig[i] );
        dictionary_set(ini, cBuffer, cval);
    }


// regrind fill retry
    sprintf( cBuffer,"%s:",striConfigPage3);
    strcat( cBuffer, striRegrindFillRetry );
    if( g_CalibrationData.m_bRegrindRetry )
    {
        sprintf(cval,"%s",striYes);
    }
    else
    {
        sprintf(cval,"%s",striNo);
    }
    dictionary_set(ini, cBuffer, cval);

// use regrind as reference
    sprintf( cBuffer,"%s:",striConfigPage3);
    strcat( cBuffer, striRegrindAsReference );
    if( g_CalibrationData.m_nUseRegrindAsReference )
    {
        sprintf(cval,"%s",striYes);
    }
    else
    {
        sprintf(cval,"%s",striNo);
    }
    dictionary_set(ini, cBuffer, cval);

// single word write
    sprintf( cBuffer,"%s:",striConfigPage3);
    strcat( cBuffer, striSingleWordWrite );
    if( g_CalibrationData.m_bSingleWordWrite )
    {
        sprintf(cval,"%s",striYes);
    }
    else
    {
        sprintf(cval,"%s",striNo);
    }
    dictionary_set(ini, cBuffer, cval);

// bypass mode
    sprintf( cBuffer,"%s:",striConfigPage3);
    strcat( cBuffer, striBypass );
    if( g_CalibrationData.m_bBypassMode )
    {
        sprintf(cval,"%s",striYes);
    }
    else
    {
        sprintf(cval,"%s",striNo);
    }
    dictionary_set(ini, cBuffer, cval);


// cleaning
    sprintf( cBuffer,"%s:", striConfigPage3);   //
    strcat( cBuffer,striCleaning);
    cTempText = GetCleaningText(g_CalibrationData.m_nCleaning);
    sprintf(cval, "%d  #%s", g_CalibrationData.m_nCleaning,cTempText );
    dictionary_set(ini, cBuffer, cval);


// top up
    sprintf( cBuffer,"%s:", striConfigPage3);   //
    strcat( cBuffer,striTopUp);
    if( g_CalibrationData.m_bTopUpMode )
    {
        sprintf(cval,"%s",striYes);
    }
    else
    {
        sprintf(cval,"%s",striNo);
    }
    dictionary_set(ini, cBuffer, cval);


// top up reference
    sprintf( cBuffer,"%s:", striConfigPage3);   //
    strcat( cBuffer,striTopUpReference);
    sprintf(cval, "%d", g_CalibrationData.m_nTopUpCompNoRef );    //
    dictionary_set(ini, cBuffer, cval);

// top up reference
    sprintf( cBuffer,"%s:", striConfigPage3);   //
    strcat( cBuffer,striFirstComponentLayering);
    if( g_CalibrationData.m_bLayering )
    {
        sprintf(cval,"%s",striYes);
    }
    else
    {
        sprintf(cval,"%s",striNo);
    }

    dictionary_set(ini, cBuffer, cval);

// percentage top up
    sprintf( cBuffer,"%s:", striConfigPage3);   //
    strcat( cBuffer,striPercentageTopUp);
    sprintf(cval, "%d", g_CalibrationData.m_nPercentageTopUp );    //
    dictionary_set(ini, cBuffer, cval);

// max kg for top up
    sprintf( cBuffer,"%s:", striConfigPage3);   //
    strcat( cBuffer,striMaxkgsForTopUp);
    sprintf(cval, "%f", g_CalibrationData.m_fMaxBatchSizeInKg );    //
    dictionary_set(ini, cBuffer, cval);

 // latency
    sprintf( cBuffer,"%s:", striConfigPage3);   //
    strcat( cBuffer,striLatency);
    sprintf(cval, "%2.0f", (g_CalibrationData.m_fLatency * 1000) );    //  nb STORE latency as ms on SD card.
    dictionary_set(ini, cBuffer, cval);


// periodic mixing time
    sprintf( cBuffer,"%s:", striConfigPage3);   //
    strcat( cBuffer,striPeriodicMixingTime);
    sprintf(cval, "%d", g_CalibrationData.m_nIntermittentMixTime );    //
    dictionary_set(ini, cBuffer, cval);

// liquid additive
    sprintf( cBuffer,"%s:", striConfigPage3);   //
    strcat( cBuffer,striLiquidAdditive);
    if( g_CalibrationData.m_bLiquidEnabled )
    {
        sprintf(cval,"%s",striYes);
    }
    else
    {
        sprintf(cval,"%s",striNo);
    }
    dictionary_set(ini, cBuffer, cval);

// Recipe entry.
    sprintf( cBuffer,"%s:", striConfigPage3);   //
    strcat( cBuffer,striRecipeEntry);
    cTempText = GetRecipeEntryModeText(g_CalibrationData.m_nRecipeEntryType);
    sprintf(cval, "%d  #%s", g_CalibrationData.m_nRecipeEntryType,cTempText );
    dictionary_set(ini, cBuffer, cval);

// Component percentage alarm
/*    sprintf( cBuffer,"%s:", striConfigPage3);   //
    strcat( cBuffer,striComponentPercentageAlarm);
    if( g_CalibrationData.m_bMaxComponentPercentageCheck )
    {
        sprintf(cval,"%s",striYes);
    }
    else
    {
        sprintf(cval,"%s",striNo);
    }
    dictionary_set(ini, cBuffer, cval);
*/
// Purge.
    sprintf( cBuffer,"%s:", striConfigPage3);   //
    strcat( cBuffer,striPurge);
    cTempText = GetPurgeText(g_CalibrationData.m_nPurgeEnable);
    sprintf(cval, "%d  #%s", g_CalibrationData.m_nPurgeEnable,cTempText );
    dictionary_set(ini, cBuffer, cval);

// purge component no
    sprintf( cBuffer,"%s:", striConfigPage3);   //
    strcat( cBuffer,striPurgeComponentNumber);
    sprintf(cval, "%d", g_CalibrationData.m_nPurgeComponentNumber );    //
    dictionary_set(ini, cBuffer, cval);

// flow rate check
    sprintf( cBuffer,"%s:", striConfigPage3);   //
    strcat( cBuffer,striFlowRateCheck);
    if( g_CalibrationData.m_bFlowRateCheck )
    {
        sprintf(cval,"%s",striYes);
    }
    else
    {
        sprintf(cval,"%s",striNo);
    }
    dictionary_set(ini, cBuffer, cval);

    // watch dog timer
        sprintf( cBuffer,"%s:", striConfigPage3);   //
        strcat( cBuffer,striWatchDog);
     	if(g_bWatchdogStatus == ENABLED )
        {
            sprintf(cval,"%s",striYes);
        }
        else
        {
            sprintf(cval,"%s",striNo);
        }
        dictionary_set(ini, cBuffer, cval);



// log to sd card
    sprintf( cBuffer,"%s:", striConfigPage3);   //
    strcat( cBuffer,striLogToSDCard);
    sprintf(cval, "%d", g_CalibrationData.m_nLogToSDCard );    //
    dictionary_set(ini, cBuffer, cval);


// TODO  ....not sure which calibration parameter is intended here.
// max component percentage limit
/*
    sprintf( cBuffer,"%s:", striConfigPage3);   //
    strcat( cBuffer,striMaxComponentPercentagelimit);
    if( g_CalibrationData.m_bMaxComponentPercentageCheck )
    {
        sprintf(cval,"%s",striYes);
    }
    else
    {
        sprintf(cval,"%s",striNo);
    }
    dictionary_set(ini, cBuffer, cval);
*/
//19.June.2007
// Control & Line Speed
//......................Testonly Data..........................
/*
   g_CalibrationData.m_fLineSpeedConstant = 2003;
   g_CalibrationData.m_nLineSpeedSignalType = 1;
   g_CalibrationData.m_fMaxExtruderSpeedPercentage = 98.7;
*/
// line speed constant.
    sprintf( cBuffer,"%s:", striControlLineSpeedPage);   //
    strcat( cBuffer,striLineSpeedConstant);
    sprintf(cval, "%6.2f", g_CalibrationData.m_fLineSpeedConstant );    //
    dictionary_set(ini, cBuffer, cval);

// max extruder setpoint
    sprintf( cBuffer,"%s:", striControlLineSpeedPage);   //
    strcat( cBuffer,striMaxExtruderSetpoint);
    sprintf(cval, "%6.2f", g_CalibrationData.m_fMaxExtruderSpeedPercentage );    //
    dictionary_set(ini, cBuffer, cval);

// line speed signal type
    sprintf( cBuffer,"%s:", striControlLineSpeedPage);   //
    strcat( cBuffer,striLineSpeedSignalType);

    cTempText = GetLineSpeedSignalText(g_CalibrationData.m_nLineSpeedSignalType);
    sprintf(cval, "%d  #%s", g_CalibrationData.m_nLineSpeedSignalType,cTempText );
    dictionary_set(ini, cBuffer, cval);

// line speed average
    sprintf( cBuffer,"%s:", striControlLineSpeedPage);   //
    strcat( cBuffer,striLineSpeedAverage);
    sprintf(cval, "%d", g_CalibrationData.m_nLineSpeedAverage );    //
    dictionary_set(ini, cBuffer, cval);

// screw speed signal type

    sprintf( cBuffer,"%s:", striControlLineSpeedPage);   //
    strcat( cBuffer,striScrewSpeedSignalType);

    cTempText = GetScrewSpeedSignalText(g_CalibrationData.m_nScewSpeedSignalType);
    sprintf(cval, "%d  #%s", g_CalibrationData.m_nScewSpeedSignalType,cTempText );
    dictionary_set(ini, cBuffer, cval);




// Measurement and control page
//......................Testonly Data..........................
/*
   g_CalibrationData.m_fControlGain = 0.81;
   g_CalibrationData.m_nControlAvgFactor = 2;
   g_CalibrationData.m_bFastControlResponse = FALSE;
   g_CalibrationData.m_bShutDownFlag = TRUE;
   g_CalibrationData.m_nShutDownTime = 23;
   g_CalibrationData.m_nControlMode = 1;
   g_CalibrationData.m_bCorrectEveryNCycles = FALSE;
   g_CalibrationData.m_nControlDeadband = 11;
   g_CalibrationData.m_nControlType = 1;

   g_CalibrationData.m_nControlLag = 7;
   g_CalibrationData.m_nIncreaseRate = 77;
   g_CalibrationData.m_nDecreaseRate = 17;
   g_CalibrationData.m_nControlDeadband = 55;
   g_CalibrationData.m_nFineIncDec = 5;
*/

// control gain.
    sprintf( cBuffer,"%s:", striControlMeasAndControlPage);   //
    strcat( cBuffer,striControlGain);
    sprintf(cval, "%6.2f", g_CalibrationData.m_fControlGain );    //
    dictionary_set(ini, cBuffer, cval);

// control averaging
    sprintf( cBuffer,"%s:", striControlMeasAndControlPage);   //
    strcat( cBuffer,striControlAveraging);
    sprintf(cval, "%d", g_CalibrationData.m_nControlAvgFactor );    //
    dictionary_set(ini, cBuffer, cval);

// fast control response
    sprintf( cBuffer,"%s:", striControlMeasAndControlPage);   //
    strcat( cBuffer,striFastControlResponse);
    if( g_CalibrationData.m_bFastControlResponse )
    {
        sprintf(cval,"%s",striYes);
    }
    else
    {
        sprintf(cval,"%s",striNo);
    }
    dictionary_set(ini, cBuffer, cval);

// extruder starvation shutdown
    sprintf( cBuffer,"%s:", striControlMeasAndControlPage);   //
    strcat( cBuffer,striExtruderStarvationShutdown);
    if( g_CalibrationData.m_bShutDownFlag )
    {
        sprintf(cval,"%s",striYes);
    }
    else
    {
        sprintf(cval,"%s",striNo);
    }
    dictionary_set(ini, cBuffer, cval);

// Extruder shutdown time.
    sprintf( cBuffer,"%s:", striControlMeasAndControlPage);   //
    strcat( cBuffer, striExtruderShutdownTime );
    sprintf(cval, "%d", g_CalibrationData.m_nShutDownTime );    //
    dictionary_set(ini, cBuffer, cval);

// control method
    sprintf( cBuffer,"%s:", striControlMeasAndControlPage);   //
    strcat( cBuffer,striControlMethod);

    cTempText = GetControlMethodText(g_CalibrationData.m_nControlMode);
    sprintf(cval, "%d  #%s", g_CalibrationData.m_nControlMode,cTempText );
    dictionary_set(ini, cBuffer, cval);

// correct every cycle
    sprintf( cBuffer,"%s:", striControlMeasAndControlPage);   //
    strcat( cBuffer,striCorrectEveryCycle);
    if( g_CalibrationData.m_bCorrectEveryNCycles )
    {
        sprintf(cval,"%s",striYes);
    }
    else
    {
        sprintf(cval,"%s",striNo);
    }
    dictionary_set(ini, cBuffer, cval);

// throughput deadband
    sprintf( cBuffer,"%s:", striControlMeasAndControlPage);   //
    strcat( cBuffer,striThroughputDeadband);
    sprintf(cval, "%d", g_CalibrationData.m_nControlPercentageLimit );    //
    dictionary_set(ini, cBuffer, cval);

// control type
    sprintf( cBuffer,"%s:", striControlMeasAndControlPage);   //
    strcat( cBuffer,striControlType);
    cTempText = GetControlTypeText(g_CalibrationData.m_nControlType);
    sprintf(cval, "%d  #%s", g_CalibrationData.m_nControlType,cTempText );
    dictionary_set(ini, cBuffer, cval);

// control lag
    sprintf( cBuffer,"%s:", striControlMeasAndControlPage );   //
    strcat( cBuffer, striControlLag );
    sprintf(cval, "%d", g_CalibrationData.m_nControlLag );    //
    dictionary_set(ini, cBuffer, cval);


// Increase Rate
    sprintf( cBuffer,"%s:", striControlMeasAndControlPage );   //
    strcat( cBuffer, striIncreaseRate  );
    sprintf(cval, "%d", g_CalibrationData.m_nIncreaseRate );    //
    dictionary_set(ini, cBuffer, cval);



// Decrease Rate
    sprintf( cBuffer,"%s:", striControlMeasAndControlPage );   //
    strcat( cBuffer, striDecreaseRate  );
    sprintf(cval, "%d", g_CalibrationData.m_nDecreaseRate );    //
    dictionary_set(ini, cBuffer, cval);

// Deadband
    sprintf( cBuffer,"%s:", striControlMeasAndControlPage );   //
    strcat( cBuffer, striDeadband  );
    sprintf(cval, "%d", g_CalibrationData.m_nControlDeadband );    //
    dictionary_set(ini, cBuffer, cval);

// Fineband
    sprintf( cBuffer,"%s:", striControlMeasAndControlPage );   //
    strcat( cBuffer, striFineband  );
    sprintf(cval, "%d", g_CalibrationData.m_nFineIncDec );    //
    dictionary_set(ini, cBuffer, cval);

// Cleaning page
//......................Testonly Data..........................
/*
   g_CalibrationData.m_nPhase1OnTime = 32;      // 3.3s
   g_CalibrationData.m_nPhase1OffTime = 34;      // 3.4s
   g_CalibrationData.m_nCleanRepetitions = 3;
   g_CalibrationData.m_nPhase2OnTime = 31;
   g_CalibrationData.m_nPhase2OffTime = 30;
   g_CalibrationData.m_nMixerCleanRepetitions = 33;
   g_CalibrationData.m_nBypassTimeLimit = 37;
   g_CalibrationData.m_bBlowEveryCycle = TRUE;
   g_CalibrationData.m_nBypassTimeLimitClean = 32;
*/

// top on time
    sprintf( cBuffer,"%s:", striCleaningPage);   //
    strcat( cBuffer,striTopOnTime);
    sprintf(cval, "%6.2f", (g_CalibrationData.m_nPhase1OnTime / 10.0f) );    // in seconds in ini file
    dictionary_set(ini, cBuffer, cval);

// top off time
    sprintf( cBuffer,"%s:", striCleaningPage);   //
    strcat( cBuffer,striTopOffTime);
    sprintf(cval, "%6.2f", (g_CalibrationData.m_nPhase1OffTime / 10.0f) );    // in seconds in ini file
    dictionary_set(ini, cBuffer, cval);

// top repetitions
    sprintf( cBuffer,"%s:", striCleaningPage);   //
    strcat( cBuffer,striTopRepetitions);
    sprintf(cval, "%d", g_CalibrationData.m_nCleanRepetitions );    //
    dictionary_set(ini, cBuffer, cval);

// bottom on time
    sprintf( cBuffer,"%s:", striCleaningPage);   //
    strcat( cBuffer,striBottomOnTime);
    sprintf(cval, "%6.2f", (g_CalibrationData.m_nPhase2OnTime / 10.0f) );    // in seconds in ini file
    dictionary_set(ini, cBuffer, cval);

// bottom off time
    sprintf( cBuffer,"%s:", striCleaningPage);   //
    strcat( cBuffer,striBottomOffTime);
    sprintf(cval, "%6.2f", (g_CalibrationData.m_nPhase2OffTime / 10.0f) );    // in seconds in ini file
    dictionary_set(ini, cBuffer, cval);

// Bottom repetitions
    sprintf( cBuffer,"%s:", striCleaningPage);   //
    strcat( cBuffer,striBottomRepetitions);
    sprintf(cval, "%d", g_CalibrationData.m_nMixerCleanRepetitions );    //
    dictionary_set(ini, cBuffer, cval);

// bypass time limit
    sprintf( cBuffer,"%s:", striCleaningPage);   //
    strcat( cBuffer,striBypassTimeLimit);
    sprintf(cval, "%d", g_CalibrationData.m_nBypassTimeLimit );    //
    dictionary_set(ini, cBuffer, cval);

// blow every cycle
    sprintf( cBuffer,"%s:", striCleaningPage);   //
    strcat( cBuffer,striBlowEveryCycle);
    if( g_CalibrationData.m_bBlowEveryCycle )
    {
        sprintf(cval,"%s",striYes);
    }
    else
    {
        sprintf(cval,"%s",striNo);
    }
    dictionary_set(ini, cBuffer, cval);

// bypass activation time
    sprintf( cBuffer,"%s:", striCleaningPage);   //
    strcat( cBuffer,striBypassActivation);
    sprintf(cval, "%d", g_CalibrationData.m_nBypassTimeLimitClean );    //
    dictionary_set(ini, cBuffer, cval);

// usb config
//
    sprintf( cBuffer,"%s:", striUSBConfigPage);   //
    strcat( cBuffer,strUSBEnable);
    cTempText = GetUSBDeviceTypeText(g_CalibrationData.m_nDumpDeviceType);
    sprintf(cval, "%d  #%s", g_CalibrationData.m_nDumpDeviceType,cTempText );




    dictionary_set(ini, cBuffer, cval);

// alarm dump
    sprintf( cBuffer,"%s:", striUSBConfigPage);   //
    strcat( cBuffer,strUSBPrinterAlarmEnable);
    if( g_CalibrationData.m_bAlarmDumpFlag )
    {
        sprintf(cval,"%s",striYes);
    }
    else
    {
        sprintf(cval,"%s",striNo);
    }
    dictionary_set(ini, cBuffer, cval);

// order report
    sprintf( cBuffer,"%s:", striUSBConfigPage);   //
    strcat( cBuffer,strUSBPrinterOrderEnable);
    if( g_CalibrationData.m_bOrderReportDumpFlag )
    {
        sprintf(cval,"%s",striYes);
    }
    else
    {
        sprintf(cval,"%s",striNo);
    }
    dictionary_set(ini, cBuffer, cval);

// roll report
    sprintf( cBuffer,"%s:", striUSBConfigPage);   //
    strcat( cBuffer,strUSBPrinterRollEnable);
    if( g_CalibrationData.m_bRollReportDumpFlag )
    {
        sprintf(cval,"%s",striYes);
    }
    else
    {
        sprintf(cval,"%s",striNo);
    }
    dictionary_set(ini, cBuffer, cval);

// hourly report
    sprintf( cBuffer,"%s:", striUSBConfigPage);   //
    strcat( cBuffer,strUSBPrinterHourEnable);
    if( g_CalibrationData.m_bHourlyDumpFlag )
    {
        sprintf(cval,"%s",striYes);
    }
    else
    {
        sprintf(cval,"%s",striNo);
    }
    dictionary_set(ini, cBuffer, cval);

// batch log
    sprintf( cBuffer,"%s:", striUSBConfigPage);   //
    strcat( cBuffer,strUSBPrinterBatchEnable);
    if( g_CalibrationData.m_bBatchLogDumpFlag )
    {
        sprintf(cval,"%s",striYes);
    }
    else
    {
        sprintf(cval,"%s",striNo);
    }
    dictionary_set(ini, cBuffer, cval);

// event log
    sprintf( cBuffer,"%s:", striUSBConfigPage);   //
    strcat( cBuffer,strUSBPrinterEventEnable);
    if( g_CalibrationData.m_bEventLogDumpFlag )
    {
        sprintf(cval,"%s",striYes);
    }
    else
    {
        sprintf(cval,"%s",striNo);
    }
    dictionary_set(ini, cBuffer, cval);

// history log
    sprintf( cBuffer,"%s:", striUSBConfigPage);   //
    strcat( cBuffer,strUSBPrinterHistoryEnable);
    if( g_CalibrationData.m_bHistoryLogDumpFlag )
    {
        sprintf(cval,"%s",striYes);
    }
    else
    {
        sprintf(cval,"%s",striNo);
    }
    dictionary_set(ini, cBuffer, cval);

    sprintf( cBuffer,"%s:", striUSBConfigPage);   //
    strcat( cBuffer,strUSBPrinterMonthlyEnable);
    if( g_CalibrationData.m_bMonthlyDumpFlag )
    {
        sprintf(cval,"%s",striYes);
    }
    else
    {
        sprintf(cval,"%s",striNo);
    }
    dictionary_set(ini, cBuffer, cval);

// vacuum loading stuff

// number of loaders
    sprintf( cBuffer,"%s:", striVacuumLoadingPage );   //
    strcat( cBuffer, striNumberOfLoaders  );
    sprintf(cval, "%d", g_VacCalibrationData.m_nLoaders );    //
    dictionary_set(ini, cBuffer, cval);

// pump idle time
    sprintf( cBuffer,"%s:", striVacuumLoadingPage );   //
    strcat( cBuffer, striIdleTime  );
    sprintf(cval, "%d", g_VacCalibrationData.m_nPumpIdleTime );    //
    dictionary_set(ini, cBuffer, cval);
// pump start time
    sprintf( cBuffer,"%s:", striVacuumLoadingPage );   //
    strcat( cBuffer, striPumpStartTime  );
    sprintf(cval, "%d", g_VacCalibrationData.m_nPumpStartTime );    //
    dictionary_set(ini, cBuffer, cval);

// atm valve overlap
    sprintf( cBuffer,"%s:", striVacuumLoadingPage );   //
    strcat( cBuffer, striAtmValveOverlap  );
    sprintf(cval, "%d", g_VacCalibrationData.m_nAVOverLapTime );    //
    dictionary_set(ini, cBuffer, cval);
// filter repetitions
    sprintf( cBuffer,"%s:", striVacuumLoadingPage );   //
    strcat( cBuffer, striFilterRepetitions );
    sprintf(cval, "%d", g_VacCalibrationData.m_nFilterRepetitions );    //
    dictionary_set(ini, cBuffer, cval);
//filter type
    sprintf( cBuffer,"%s:", striVacuumLoadingPage );   //
    strcat( cBuffer, striFilterType  );
    cTempText = GetVacuumLoaderFilterTypeText(g_VacCalibrationData.m_nFilterType);
    sprintf(cval, "%d  #%s", g_VacCalibrationData.m_nFilterType,cTempText );
    dictionary_set(ini, cBuffer, cval);


// dump delay
    sprintf( cBuffer,"%s:", striVacuumLoadingPage );   //
    strcat( cBuffer, striDumpDelay );
    sprintf(cval, "%d", g_VacCalibrationData.m_nDischargeTime );    //
    dictionary_set(ini, cBuffer, cval);

// individual loader data
    for(i=0; i<g_VacCalibrationData.m_nLoaders; i++)
    {
    // on off status
        sprintf( cBuffer,"%s%d:", striIndividualLoaderPage,i+1 );   //
        strcat( cBuffer, striLoaderOnOffStatus  );
        sprintf(cval, "%d", g_VacCalibrationData.m_bOnFlag[i] );    //
        dictionary_set(ini, cBuffer, cval);

        // sunction time
        sprintf( cBuffer,"%s%d:", striIndividualLoaderPage,i+1 );   //
        strcat( cBuffer, striLoaderSuctionTime  );
        sprintf(cval, "%d", g_VacCalibrationData.m_nSuctionTime[i] );    //
        dictionary_set(ini, cBuffer, cval);

		// post fill
        sprintf( cBuffer,"%s%d:", striIndividualLoaderPage,i+1 );   //
        strcat( cBuffer, striLoaderPostFillTime  );
        sprintf(cval, "%d", g_VacCalibrationData.m_nPostFillTime[i] );    //
        dictionary_set(ini, cBuffer, cval);

      // priority
         sprintf( cBuffer,"%s%d:", striIndividualLoaderPage,i+1 );   //
         strcat( cBuffer, striPriority  );
         sprintf(cval, "%d", g_VacCalibrationData.m_nPriority[i] );    //
         dictionary_set(ini, cBuffer, cval);
    }
    sprintf( cBuffer, "config.txt");    // hardware serial no.


    F_FILE * pFile = f_open(cBuffer, "w");
    if( pFile )
    {
      iniparser_dump_ini(ini, pFile );
      f_close( pFile );
    }


	iniparser_freedict(ini);

//      iprintf("\ndictionary freed\n");
   f_delvolume(MMC_DRV_NUM);

    ReInitialisePinsUsedBySDCard();
    g_bSPIBusBusy = FALSE;       // free up the SPI for other uses.
    g_bSDCardSaveConfigComplete = TRUE;
	return( 0 );
}


void WriteCalibrationDataToSDCard (void )
{
    if ( SDCardWritable()&& (g_nDisableSDCardAccessCtr == 0) )
    {
        WriteBatchIniFile("default.ini");
        if(g_bWriteToIniFileAgain)
        {
            g_bWriteToIniFileAgain = FALSE;
            WriteBatchIniFile("default.ini");
      }
    }
}
