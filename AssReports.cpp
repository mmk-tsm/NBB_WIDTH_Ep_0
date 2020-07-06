//
// P.Smith                      1/6/06
// copy of config & order report data to email buffer.
//
// P.Smith                      26/6/06
// use proper labels for Remote modules checking.
//
// P.Smith                      5/2/07
// Remove iprintf
//
// P.Smith                      22/6/07
// name change to proper blender names.
// correct copy of string to print buffer for 50 and micrabatch
//
// P.Smith                      18/10/07
// use cleaning variable name m_nCleaning
//
// P.Smith                      17/7/08
// Remove Micrabatch reference
//
// P.Smith                      16/10/08
// correct compiler warnings
//
// P.Smith                      24/11/08
// generate order report and write same to sd card.
//
// P.Smith                      15/1/09
// store order report name to a global to so the name can be used
// to write to the usb
//
// P.Smith                      3/12/09
// when writing order report to sd card, only check that the event log is enabled
//
// P.Smith                      15/1/10
// when writing the order name, append the data.

#include <basictypes.h>
#include <Stdlib.h>
#include <Stdio.h>
#include <String.h>
#include "General.h"
#include "BatVars.h"
#include "GPMCalc.h"
#include "ConversionFactors.h"
#include "BatchCalibrationFunctions.h"
#include "SetpointFormat.h"
#include "BatchMBIndices.h"
#include "BatVars2.h"
#include "Acumwt.h"
#include "Mbprogs.h"
#include "Bbcalc.h"
#include "BatVars2.h"
#include "Alarms.h"
#include "BlRep.h"
#include "Lspeed.h"
#include <basictypes.h>
#include "PrinterFunctions.h"
#include "AssReports.h"
#include "Email.h"
#include "Sdcard.h"


extern  char    *strOrderReport;
extern  char    *strNewLine;
extern  char    *strOrderStartedAt;
extern  char    *RevisionNumber;
extern  char    *strWeightTotal;
extern  char    *strWeightTarget;
extern  char    *strSystemConfig;
extern  char    *strBlenderType;
extern  char    *strMicraBatch;
extern  char    *strTSM50;
extern  char    *strTSM150;
extern  char    *strTSM350;
extern  char    *strTSM650;
extern  char    *strTSM1000;
extern  char    *strTSM1500;
extern  char    *strTSM3000;
extern  char    *strOfflineMode;
extern  char    *strMode;
extern  char    *strFillRetry;
extern  char    *strFirstComponentCompensation;
extern  char    *strEnabled;
extern  char    *strDisabled;
extern  char    *strMultiStageFilling;
extern  char    *strLayering;
extern  char    *strBatchesinKGHRAverage;

extern  structOrderReport   g_OrderReport;
extern CalDataStruct    g_CalibrationData;

extern  char    *strOrderEndedAt;
extern  char   *strOrderName;
extern  char   *strOrderLength;




//////////////////////////////////////////////////////
// CopyOrderReportToBuffer()
// Copy order report to buffer save to sd card.
//////////////////////////////////////////////////////

void CopyOrderReportToBuffer( void )
{
    char    cBuffer[MAX_PRINT_WIDTH+1];
    char tBuffer[ORDERNOCHARACTERNO+1];

    unsigned int i;
    g_cOrderReportStorageBuffer[0] = '\0';
    sprintf( cBuffer, "\nTSM Order Report for %s blender", g_CalibrationData.m_cEmailBlenderName );     //
    strcat( g_cOrderReportStorageBuffer, cBuffer);

    strcpy( g_cOrderReportStorageBuffer, strNewLine );
    strcat( g_cOrderReportStorageBuffer, strOrderReport);
    strcat( g_cOrderReportStorageBuffer, strNewLine);
    strcat( g_cOrderReportStorageBuffer, strNewLine);

// copy order name
    for(i = 0; i < ORDERNOCHARACTERNO; i++)             // order no.
    {
        tBuffer[i] = g_OrderReport.m_arrOrderNo[i];
    }
    tBuffer[ORDERNOCHARACTERNO] = '\0';


    sprintf(cBuffer,"\n%s     %s",strOrderName,tBuffer);
    strcat( g_cOrderReportStorageBuffer, cBuffer);
    strcat( g_cOrderReportStorageBuffer, strNewLine);

    strcat( g_cOrderReportStorageBuffer, strOrderStartedAt);

    sprintf( cBuffer, " %02d/%02d  %02d:%02d ", g_OrderReport.m_cStartTime[TIME_DATE],g_OrderReport.m_cStartTime[TIME_MONTH],g_OrderReport.m_cStartTime[TIME_HOUR],g_OrderReport.m_cStartTime[TIME_MINUTE]);
    strcat( g_cOrderReportStorageBuffer, cBuffer);
    strcat( g_cOrderReportStorageBuffer, strNewLine);

    strcat( g_cOrderReportStorageBuffer, strOrderEndedAt);
    sprintf( cBuffer, " %02d/%02d  %02d:%02d ", g_OrderReport.m_cEndTime[TIME_DATE],g_OrderReport.m_cEndTime[TIME_MONTH],g_OrderReport.m_cEndTime[TIME_HOUR],g_OrderReport.m_cEndTime[TIME_MINUTE]);
    strcat( g_cOrderReportStorageBuffer, cBuffer);


    sprintf(cBuffer,"\n%s     %7.0f",strOrderLength,g_OrderReport.m_fOrderLength);
    strcat( g_cOrderReportStorageBuffer, cBuffer);

    strcat( g_cOrderReportStorageBuffer, strNewLine );
    strcat( g_cOrderReportStorageBuffer, strWeightTarget);
    strcat( g_cOrderReportStorageBuffer, strNewLine);

    for( i=0; i<g_CalibrationData.m_nComponents; i++)
    {
        sprintf( cBuffer, "  %3d ", i+1 );   // comp no.
        strcat( g_cOrderReportStorageBuffer, cBuffer);

        sprintf( cBuffer, "          %7.3f ", g_OrderReport.m_ComponentData[i].m_fWeight );     //
        strcat( g_cOrderReportStorageBuffer, cBuffer);

        sprintf( cBuffer, "       %3.2f ", g_OrderReport.m_ComponentData[i].m_fPercentageSetpoint );     //
        strcat( g_cOrderReportStorageBuffer, cBuffer);

        strcat( g_cOrderReportStorageBuffer, strNewLine );
    }
        strcat( g_cOrderReportStorageBuffer, "  ");
        strcat( g_cOrderReportStorageBuffer, strWeightTotal);

        sprintf( cBuffer, "         %7.3f ", g_OrderReport.m_fOrderWeight );     //
        strcat( g_cOrderReportStorageBuffer, cBuffer);
        strcat( g_cOrderReportStorageBuffer, strNewLine );  //nbb--todo-- save to file
        for(i = 0; i < 8; i++)             // order no.
        {
            g_OrderReportName[i] = g_OrderReport.m_arrOrderNo[i];
        }
        g_OrderReportName[8] = '\0';
        strcat( g_OrderReportName, ".rpt");
        if((g_CalibrationData.m_nLogToSDCard & LOG_EVENTS_TO_SD_CARD) != 0)
        {
            WriteToSdCardFile(g_OrderReportName,g_cOrderReportStorageBuffer);
        }
}


//////////////////////////////////////////////////////
// CopySystemConfigToBuffer
//

// P.Smith                              9-3-2005
//////////////////////////////////////////////////////

void CopySystemConfigToBuffer( void )
{
/*
    char    cBuffer[MAX_PRINT_WIDTH+1];
    unsigned int i;
    float fTemp;

    sprintf( cBuffer, "\t\t%s", strSystemConfig );     //
    strcpy( g_cPrinterStorageBuffer, cBuffer);
    strcat( g_cPrinterStorageBuffer, strNewLine );

 // page 1 of config

    strcat( g_cPrinterStorageBuffer, strNewLine );
    strcat( g_cPrinterStorageBuffer, strNewLine );
    strcat( g_cPrinterStorageBuffer, "System Configuration Page 1");
    strcat( g_cPrinterStorageBuffer, strNewLine );

    strcat( g_cPrinterStorageBuffer, strNewLine );
    strcat( g_cPrinterStorageBuffer, strBlenderType );
    strcat( g_cPrinterStorageBuffer, "\t");

    if(g_CalibrationData.m_nBlenderType == TSM50)
    {
        strcat( g_cPrinterStorageBuffer,strTSM50  );
    }
    else
//    if(g_CalibrationData.m_nBlenderType == MICRABATCH)  //nbb--todo-- do for optimix mode
//    {
//        strcat( g_cPrinterStorageBuffer, strMicraBatch );
//    }

   if(g_CalibrationData.m_nBlenderType == TSM150)
    {
        strcat( g_cPrinterStorageBuffer, strTSM150 );
    }

   if(g_CalibrationData.m_nBlenderType == TSM650)
    {
        strcat( g_cPrinterStorageBuffer, strTSM650 );
    }

   if(g_CalibrationData.m_nBlenderType == TSM1000)
    {
        strcat( g_cPrinterStorageBuffer, strTSM1000 );
    }

   if(g_CalibrationData.m_nBlenderType == TSM1500)
    {
        strcat( g_cPrinterStorageBuffer, strTSM1500 );
    }

   if(g_CalibrationData.m_nBlenderType == TSM3000)
    {
        strcat( g_cPrinterStorageBuffer, strTSM3000 );
    }

    // blender mode

    strcat( g_cPrinterStorageBuffer, strNewLine );
    strcat( g_cPrinterStorageBuffer, strMode );
    strcat( g_cPrinterStorageBuffer, "\t");
    strcat( g_cPrinterStorageBuffer, "Blending only");

// Offline Mode

    strcat( g_cPrinterStorageBuffer, strNewLine );
    strcat( g_cPrinterStorageBuffer, strOfflineMode );
    strcat( g_cPrinterStorageBuffer, "\t");
    strcat( g_cPrinterStorageBuffer, "High / Low Level Sensors");

// Fill Retry

    strcat( g_cPrinterStorageBuffer, strNewLine );
    strcat( g_cPrinterStorageBuffer, strFillRetry );
    strcat( g_cPrinterStorageBuffer, "\t");
    sprintf( cBuffer, "%2d", g_CalibrationData.m_nFillRetries );     //
    strcat( g_cPrinterStorageBuffer, cBuffer);

// First component compensation
    strcat( g_cPrinterStorageBuffer, strNewLine );
    strcat( g_cPrinterStorageBuffer, strFirstComponentCompensation );
    strcat( g_cPrinterStorageBuffer, "\t");
    StoreEnabledorDisabled(g_CalibrationData.m_bFirstComponentCompensation);

 // Multistage filling
    strcat( g_cPrinterStorageBuffer, strNewLine );
    strcat( g_cPrinterStorageBuffer, strMultiStageFilling );
    strcat( g_cPrinterStorageBuffer, "\t");
    strcat( g_cPrinterStorageBuffer, strDisabled );

 // Layering
    strcat( g_cPrinterStorageBuffer, strNewLine );
    strcat( g_cPrinterStorageBuffer, strLayering );
    strcat( g_cPrinterStorageBuffer, "\t");
    StoreEnabledorDisabled(g_CalibrationData.m_bLayering);

 // Batches in kg /hr average
    strcat( g_cPrinterStorageBuffer, strNewLine );
    strcat( g_cPrinterStorageBuffer, strBatchesinKGHRAverage );
    strcat( g_cPrinterStorageBuffer, "\t");
    sprintf( cBuffer, "%2d", g_CalibrationData.m_nBatchesInKghAvg );     //
    strcat( g_cPrinterStorageBuffer, cBuffer);


 // Machine Name
    strcat( g_cPrinterStorageBuffer, strNewLine );
    strcat( g_cPrinterStorageBuffer, "Machine Name                ");
    strcat( g_cPrinterStorageBuffer, "\t");
    sprintf( cBuffer, "%s", g_CalibrationData.m_cEmailBlenderName );     //
    strcat( g_cPrinterStorageBuffer, cBuffer);

// page 2 of config

    strcat( g_cPrinterStorageBuffer, strNewLine );
    strcat( g_cPrinterStorageBuffer, strNewLine );
    strcat( g_cPrinterStorageBuffer, "System Configuration Page 2");
    strcat( g_cPrinterStorageBuffer, strNewLine );

// units
    strcat( g_cPrinterStorageBuffer, strNewLine );
    strcat( g_cPrinterStorageBuffer, "Units                         ");
    strcat( g_cPrinterStorageBuffer, "\t");
    if(g_CalibrationData.m_nUnitType == UNIT_IMPERIAL)
    {
        strcat( g_cPrinterStorageBuffer, "Imperial");
    }
    else
    {
        strcat( g_cPrinterStorageBuffer, "Metric");
    }

// Remote modules
    strcat( g_cPrinterStorageBuffer, strNewLine );
    strcat( g_cPrinterStorageBuffer, "Remote Modules                ");
    strcat( g_cPrinterStorageBuffer, "\t");
    if( g_CalibrationData.m_nPeripheralCardsPresent == 0x00 )  // is SEI present??
    {
        strcat( g_cPrinterStorageBuffer, "Disabled");
    }
    else
    if( g_CalibrationData.m_nPeripheralCardsPresent == SEIENBITPOS )  // is SEI present??
    {
        strcat( g_cPrinterStorageBuffer, "SEI");
    }
    else
    if( g_CalibrationData.m_nPeripheralCardsPresent == LLSENBITPOS )  // is lls present??
    {
        strcat( g_cPrinterStorageBuffer, "LLS");
    }
    if( g_CalibrationData.m_nPeripheralCardsPresent == (SEIENBITPOS || LLSENBITPOS) )  // is lls present??
    {
        strcat( g_cPrinterStorageBuffer, "SEI LLS");
    }

// standard component config
    strcat( g_cPrinterStorageBuffer, strNewLine );
    strcat( g_cPrinterStorageBuffer, "Standard comp config          ");
    strcat( g_cPrinterStorageBuffer, "\t");
    StoreEnabledorDisabled(g_CalibrationData.m_bStandardCompConfig);


// Offline mixing time
    strcat( g_cPrinterStorageBuffer, strNewLine );
    sprintf( cBuffer, "Offline Mixing Time                 %2d", g_CalibrationData.m_nMixingTime );     //
    strcat( g_cPrinterStorageBuffer, cBuffer);

// Mixing Motor Shut off time
    strcat( g_cPrinterStorageBuffer, strNewLine );
    sprintf( cBuffer, "Mixing motor shut-off time          %2d", g_CalibrationData.m_nMixShutoffTime );     //
    strcat( g_cPrinterStorageBuffer, cBuffer);

// Auto Cycle
    strcat( g_cPrinterStorageBuffer, strNewLine );
    sprintf( cBuffer, "Autocycle (1=ON, 0=OFF)             %2d", g_CalibrationData.m_nDiagnosticsMode );     //
    strcat( g_cPrinterStorageBuffer, cBuffer);

// Level Sensor Alarm Delay
    strcat( g_cPrinterStorageBuffer, strNewLine );
    sprintf( cBuffer, "Level sensor alarm delay (s)        %2d", g_CalibrationData.m_nLevelSensorDelay );     //
    strcat( g_cPrinterStorageBuffer, cBuffer);

// Volumetric mode
    strcat( g_cPrinterStorageBuffer, strNewLine );
    strcat( g_cPrinterStorageBuffer, "Volumetric Mode               ");
    strcat( g_cPrinterStorageBuffer, "\t");
    StoreEnabledorDisabled(g_CalibrationData.m_bVolumetricModeFlag);

// Cycle speed
    strcat( g_cPrinterStorageBuffer, strNewLine );
    strcat( g_cPrinterStorageBuffer, "Cycle Speed                   ");
    strcat( g_cPrinterStorageBuffer, "\t");

    if(g_CalibrationData.m_bCycleFastFlag)
    {
        strcat( g_cPrinterStorageBuffer, "Fast");
    }
    else
    {
        strcat( g_cPrinterStorageBuffer, "Slow");
    }


// Max throughput
    strcat( g_cPrinterStorageBuffer, strNewLine );
    sprintf( cBuffer, "Maximum throughput (lb/h)  \t       0");     //
    strcat( g_cPrinterStorageBuffer, cBuffer);




// Page 3 of config
//
    strcat( g_cPrinterStorageBuffer, strNewLine );
    strcat( g_cPrinterStorageBuffer, strNewLine );
    strcat( g_cPrinterStorageBuffer, "System Configuration Page 3");
    strcat( g_cPrinterStorageBuffer, strNewLine );

// regrind fill retry
    strcat( g_cPrinterStorageBuffer, strNewLine );
    strcat( g_cPrinterStorageBuffer, "Regrind Fill Retry            ");
    strcat( g_cPrinterStorageBuffer, "\t");
    StoreEnabledorDisabled(g_CalibrationData.m_bRegrindRetry);

// regrind as ref
    strcat( g_cPrinterStorageBuffer, strNewLine );
    strcat( g_cPrinterStorageBuffer, "Regrind as reference          ");
    strcat( g_cPrinterStorageBuffer, "\t");
    StoreEnabledorDisabled(g_CalibrationData.m_nUseRegrindAsReference);

// single word write
    strcat( g_cPrinterStorageBuffer, strNewLine );
    strcat( g_cPrinterStorageBuffer, "Single word write             ");
    strcat( g_cPrinterStorageBuffer, "\t");
    StoreEnabledorDisabled(g_CalibrationData.m_bSingleWordWrite);

// bypass mode
    strcat( g_cPrinterStorageBuffer, strNewLine );
    strcat( g_cPrinterStorageBuffer, "Bypass mode                   ");
    strcat( g_cPrinterStorageBuffer, "\t");
    StoreEnabledorDisabled(g_CalibrationData.m_bBypassMode);

// cleaning
    strcat( g_cPrinterStorageBuffer, strNewLine );
    strcat( g_cPrinterStorageBuffer, "Cleaning                      ");
    strcat( g_cPrinterStorageBuffer, "\t");
    StoreEnabledorDisabled(g_CalibrationData.m_nCleaning);

// topup
    strcat( g_cPrinterStorageBuffer, strNewLine );
    strcat( g_cPrinterStorageBuffer, "Top up                        ");
    strcat( g_cPrinterStorageBuffer, "\t");
    StoreEnabledorDisabled(g_CalibrationData.m_bTopUpMode);

// Fill sorting selection
    strcat( g_cPrinterStorageBuffer, strNewLine );
    strcat( g_cPrinterStorageBuffer, "Fill sorting Selection        ");
    strcat( g_cPrinterStorageBuffer, "\t");
    StoreEnabledorDisabled(g_CalibrationData.m_bFillOrderManualSelect);


// Percentage top up
    strcat( g_cPrinterStorageBuffer, strNewLine );
    sprintf( cBuffer, "Percentage Top up                   %2d", g_CalibrationData.m_nPercentageTopUp );     //
    strcat( g_cPrinterStorageBuffer, cBuffer);

// Latency
    strcat( g_cPrinterStorageBuffer, strNewLine );
    sprintf( cBuffer, "Latency                           %4.0f", (g_CalibrationData.m_fLatency * 1000.0) );     //
    strcat( g_cPrinterStorageBuffer, cBuffer);



// Weight calibratin page
//
    strcat( g_cPrinterStorageBuffer, strNewLine );
    strcat( g_cPrinterStorageBuffer, strNewLine );
    strcat( g_cPrinterStorageBuffer, "Weight Calibration");
    strcat( g_cPrinterStorageBuffer, strNewLine );

// a/d counts

    strcat( g_cPrinterStorageBuffer, strNewLine );
    sprintf( cBuffer, "a/d counts                          %ld", g_lRawAtDCounts );     //
    strcat( g_cPrinterStorageBuffer, cBuffer);

// a/d tare

    strcat( g_cPrinterStorageBuffer, strNewLine );
    sprintf( cBuffer, "a/d Tare Counts                     %ld", g_CalibrationData.m_lTareCounts );     //
    strcat( g_cPrinterStorageBuffer, cBuffer);

// weight

    fTemp =  g_fOneSecondHopperWeight;
    if(g_fWeightConversionFactor != 1.0f)
    fTemp *= g_fWeightConversionFactor;
    strcat( g_cPrinterStorageBuffer, strNewLine );
    sprintf( cBuffer, "Weight                              %2.4f",fTemp );     //
    strcat( g_cPrinterStorageBuffer, cBuffer);

 // calibration weight constant

    strcat( g_cPrinterStorageBuffer, strNewLine );
    sprintf( cBuffer, "Cal Weight Constant                 %5.0f",g_CalibrationData.m_fWeightConstant );     //
    strcat( g_cPrinterStorageBuffer, cBuffer);

    strcat( g_cPrinterStorageBuffer, strNewLine );

// fill target weight
    fTemp =  g_CalibrationData.m_fBatchSize;
    if(g_fWeightConversionFactor != 1.0f)
    fTemp *= g_fWeightConversionFactor;
    strcat( g_cPrinterStorageBuffer, strNewLine );
    sprintf( cBuffer, "Fill target Weight                  %1.1f", fTemp );     //
    strcat( g_cPrinterStorageBuffer, cBuffer);

// Weight averaging
    strcat( g_cPrinterStorageBuffer, strNewLine );
    sprintf( cBuffer, "Weight Averaging                    %2d", g_CalibrationData.m_nWeightAvgFactor);     //
    strcat( g_cPrinterStorageBuffer, cBuffer);

// Double dump
    strcat( g_cPrinterStorageBuffer, strNewLine );
    strcat( g_cPrinterStorageBuffer, "Double Dump                   ");
    strcat( g_cPrinterStorageBuffer, "\t");
    StoreEnabledorDisabled(g_CalibrationData.m_bDoubleDump);




//    iprintf("%s",g_cPrinterStorageBuffer); //nbb--testonly--

     for(i=0; i<2000; i++)
     {
          g_arrcEmailBuffer[i] = g_cPrinterStorageBuffer[i];
     }

     sprintf( cBuffer, "TSM System Configuration for %s blender", g_CalibrationData.m_cEmailBlenderName );     //
     TestEmail(g_CalibrationData.m_cEmailSupervisorEmailAddress,cBuffer);
*/
}


void StoreEnabledorDisabled( BOOL bEnabled )
{
/*
    if(bEnabled)
    {
           strcat( g_cPrinterStorageBuffer, strEnabled);
    }
    else
    {
           strcat( g_cPrinterStorageBuffer, strDisabled);
    }
*/
}






