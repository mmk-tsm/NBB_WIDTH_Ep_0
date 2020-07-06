
//////////////////////////////////////////////////////
//
// P.Smith                      1/7/08
// GetAlarmText gets the appropriate alarm text
// use tabs in event log,format is date, month, year
// put occurred with events to keep the columns lining up.
// add events for all alarms, auto manual activations, sd card operations
//
// P.Smith                          24/6/08
// g_CurrentTime removed,correct year and date copy to event log
//
// P.Smith                          24/6/08
// make event log more readable by putting spaces rather than 00 in log.
//
// P.Smith                          11/9/08
// AddEventToLog is modified to store the text to g_cEventLogFile if the blender
// is in the middle of a cycle and not to write to the sd card.
// g_bWriteEventLogToSdCard is set to indicate that a write to the sd card is required
// when the cycle is finished.
// g_cEventName & g_cEventLogFile are made globals
//
// P.Smith                          30/9/08
// add "dump flap not opened" to the alarm text strings.
// change alarm word from a char to an int.
//
// P.Smith                          14/10/08
// remove cAlarmType
//
// P.Smith                          29/10/08
// check for SDCARDFAULTALARM
//
// P.Smith                          14/11/08
// if g_bWriteEventLogToSdCard is set, do not write file to sd card.
// also if g_bSDCardAccessInProgress set, there was a flaw in the logic.
// it should have been using && not ||
// this was having the effect of g_cEventLogFile being set to \0 if two
// events came in at the same time.
// the first event was stored to g_cEventLogFile and then the next event
// was written to the sd card and g_cEventLogFile was set to \0
// there was then an attempt to write the data to a file and this caused
// the blender to reset.
// this problem happened when a blender was retrying on a component and
// a new recipe was downloaded. The result of this was two events coming in
// together and the system trying to write /0 to the sd card.
//
// P.Smith                          18/11/08
// in AddEventToLog check for length of string in buffer and do not add
// if the string length has been exceeded
//
// P.Smith                          26/11/08
// add eoc weight and current weight for leak alarm - cleared and occurred
// add text strings for ATODMAXEXCEEDEDALARM
// AddPowerOnResetToEventLog is modified to allow string pointer to be trasnferred to function
//
// P.Smith                          11/12/08
// copy event log to g_cEventLogFileCopy so that it can be sent to the usb
// DumpEvent dumps event to usb, name change g_bTelnetSDCardAccessInProgress
//
// P.Smith                          15/1/09
// name change g_bSDCardAccessInProgress to g_bTelnetSDCardAccessInProgres
// call DumpEvent to dump data to the USB if it is enabled.
//
// P.Smith                          25/5/09
// add AddIC2EventToLog to record ic2 error.
// remove double print of event time
// added I2CSTOPPEDALARM text
//
// P.Smith                          10/6/09
// added COMPOPENTIMETOOSHORTALARM and COMPOPENTIMETOOLONGALARM
//
// P.Smith                          24/8/09
// added strDeviationAlarm, this was originally showing zero speed alarm text.
// added invalid config data alarm message
//
// P.Smith                          26/8/09
// added blender name to event log power on reset message
//
// P.Smith                          1/9/09
// add alarm code to event log for alarm occurred / cleared
// added AddLockToEventLog & AddUnLockToEventLog
// added AddModbusWriteEventToLog to write modbus table change to log
//
// P.Smith                          2/9/09
// correct addition of alarm code when alarm is occurred or cleared.
//
// P.Smith                          10/9/09
// add strHopperCalibratedAtZeroAlarm to alarm strings.
// show calibrated weight in AddWeightHopperCalibratedToEventLog
//
// P.Smith                          17/9/09
// added in vacuum loading alarm.
// check m_nLogToSDCard for LOG_EVENTS_TO_SD_CARD
//
// P.Smith                          7/12/09
// added tare and calibration constant to the log file.
//
// P.Smith                          17/12/09
// a problem has been seen where the alarm code is copied into a buffer
// cAlarmCodeText, the buffer is too small at 10 characters
//
// P.Smith                          6/1/10
// in WriteEventToSDCard, allow event to be logged if sd or usb events enabled.
// call WriteToSdCardFile if sd event log enabled.
//
// P.Smith                          11/1/10
// add AddFileCouldNotBeOpenedEventToLog
//
// P.Smith                          15/3/10
// added AddSoftwareResetPendingEventToLog, this stores the event
// that is about to happen to the log.
// generate the text directly for the event as it needs to be printed
// eventhough the blender is cycling.
// make a small correction to AddIC2EventToLog, print a space instead of passing 0
//
// P.Smith                          23/3/10
// show sha hash, if different from that in flash, show D if same show E
// this is added to the power on reset
//
// P.Smith                          25/3/10
// added AddEventToLogPowerUp to print the security code to the
// log on power, this is added to the decimal part of the time.
//
// P.Smith                          30/4/10
// added AddModbusTCPFatalErrorToLog

// M.McKiernan						29/5/2020
// Cater for ultrasonic sensor alarms.
// //wfh
//       case US_SENSOR_1_ALARM:
//            cTextString = strUltrasonicSensor1Alarm;	//"Alarm - Ultrasonic Sensor 1"
//            break;
//       case US_SENSOR_2_ALARM:
//            cTextString = strUltrasonicSensor2Alarm;	//"Alarm - Ultrasonic Sensor 1"
//            break;
//       case US_SENSOR_3_ALARM:
//            cTextString = strUltrasonicSensor3Alarm;	//"Alarm - Ultrasonic Sensor 1"
//            break;
//       case US_SENSOR_4_ALARM:
//            cTextString = strUltrasonicSensor4Alarm;	//"Alarm - Ultrasonic Sensor 1"

//////////////////////////////////////////////////////


#include <stdio.h>

#include "SerialStuff.h"
#include "BatchMBIndices.h"
#include "BatchCalibrationFunctions.h"
#include "General.h"
#include "ConversionFactors.h"

#include "16R40C.h"
#include "ConfigurationFunctions.h"
#include "MBMHand.h"
#include "MBSHand.h"

#include "MBProgs.h"
#include "SetpointFormat.h"
#include "TimeDate.h"
#include "BatVars.h"
#include "BBCalc.h"
#include "MBTogSta.h"
#include "Alarms.h"
#include "KghCalc.h"
#include "BatVars2.h"
#include "Monbat.h"
#include "Blrep.h"
#include "MBProsum.h"
#include "Pause.h"
#include "CycleMonitor.h"
#include "TSMPeripheralsMBIndices.h"

#include <basictypes.h>
#include <string.h>
#include "Eventlog.h"
#include "Sdcard.h"
#include "VNC1L_g.h"
#include "Hash.h"
#include "OneWire.h"


extern  char    *strNewLine;

extern  char   *strFillAlarm;
extern  char   *strZeroLinespeedAlarm;
extern  char   *strMixingMotorStoppedAlarm;
extern  char   *strControlAlarm;
extern  char   *strInvalidCalibrationAlarm;
extern  char   *strLevelSensorMonitorAlarm;
extern  char   *strAtTargetWeightAlarm;
extern  char   *strSEICommunicationsAlarm;
extern  char   *strSEIResetAlarm;
extern  char   *strComponentNot100PercentAlarm;
extern  char   *strStarvationAlarm;
extern  char   *strLeakAlarm;
extern  char   *strBinLowLevelAlarm;
extern  char   *strTareAlarm;
extern  char   *strShutDownAlarm;
extern  char   *strLicenseAlarm;
extern  char   *strHardwareLicenseAlarm;
extern  char   *strPercentageDeviationAlarm;
extern  char   *strAlarmNotRecognised;
extern  char   *strBlenderRunToStopTransition;
extern  char   *strBlenderStopToRunTransition;

extern  char   *strBlenderRunToStopTransition;
extern  char   *strBlenderStopToRunTransition;
extern  char   *strBlenderSwitchedToAuto;
extern  char   *strBlenderSwitchedToManual;
extern  char   *strBlenderSwitchedToManualControlAlarm;
extern  char   *strBlenderSwitchedToManualPotControl;
extern  char   *strBlenderSwitchedToManualSEIForcedExit;
extern  char   *strBlenderSwitchedToManualZeroLinespeed;
extern  char   *strBlenderSwitchedToManualLeakalarm;
extern  char   *strBlenderSwitchedToManualExtruderstalled;
extern  char   *strBlenderSwitchedToManualBlenderPausedEvent;
extern  char   *strBlenderSwitchedToManualIncDecSetpointError;
extern  char   *strBlenderSwitchedToManualStarvation;
extern  char   *strBlenderInstantVolCorrDueToLspeedChange;

extern char      *strEvlEvent;
extern char      *strEvlOccurred;
extern char      *strEvlGeneral;
extern char      *strEvlAlarm;
extern char      *strEvlCleared;
extern char      *strEvlStopRun;
extern char      *strEvlAutoManual;
extern char      *strEvlControl;
extern char      *strEvlPowerOnReset;
extern char      *strEvlSDCard;
extern char      *strEvlSDCardCopyConfigToBackup;
extern char      *strEvlBypassOpened;
extern char      *strEvlBypassClosed;
extern char      *strEvlBlenderRecipeChanged;
extern char      *strEvlWeighHopperTared;
extern char      *strEvlWeighHopperCalibrated;
extern char      *strEvlSdCardLoadConfigComplete;
extern char      *strEvlSdCardLoadBackupConfigComplete;
extern char      *strEvlSdCardSaveConfigComplete;
extern char      *strEvlCleaningCycleStarted;

extern  char   *strOperatorPanelCondigPage1;
extern  char   *strOperatorPanelCondigPage2;
extern  char   *strOperatorPanelCondigPage3;
extern  char   *strOperatorPanelLspeedPage;
extern  char   *strOperatorPanelMeasurementPage;
extern  char   *strOperatorPanelDeviationPage;
extern  char   *strOperatorPanelBinConfigPage;
extern  char   *strOperatorPanelModePage;
extern  char   *strOperatorPanelUpdatePage;
extern  char   *strOperatorPanelNetworkConfigPage;
extern  char   *strOperatorPanelCleanConfigPage;
extern  char   *strOperatorPanelPasswordConfigPage;
extern  char   *strOperatorPanelLicenseConfigPage;
extern  char   *strOperatorPanelOptimisationConfigPage;
extern  char   *strEvlSpaces;
extern  char   *strDumpFlapHasNotOpenedAlarm;
extern  char   *strSDCardAlarm;
extern  char   *strAToDMaxExceededAlarm;
extern  char   *strI2CStopped;
extern  char   *strComponentOpenTimeTooShort;
extern  char   *strComponentOpenTimeTooLong;
extern  char   *strInvalidConfigData;



extern CalDataStruct    g_CalibrationData;
extern structUSB   g_sUSB;
extern  char        g_cPrinterBuffer[];
extern  char   *strDeviationAlarm;
extern  char   *strEvlConfigLocked;
extern  char   *strEvlConfigUnLocked;
extern  char   *strEvModbusWrite;
extern  char   *strAToDEOCNotCompleteAlarm;
extern  char   *strAToDNegativePolarityAlarm;
extern  char   *strAToTestPassed;
extern  char   *strAToTestFailed;
extern  char   *strHopperCalibratedAtZeroAlarm;
extern  char   *strVacuumLoaderAlarm;
extern  char   *strSDCardWriteError;
extern  char   *strWatchdogPending;
extern  char   *strModbusTCPFatalError;

extern  char    *strUltrasonicSensor1Alarm;
extern  char    *strUltrasonicSensor2Alarm;
extern  char    *strUltrasonicSensor3Alarm;
extern  char    *strUltrasonicSensor4Alarm;

extern  WORD g_nHashOfSHAIDInFlash;
extern  WORD g_nHashOfSHAID;
extern  int     g_nGeneralTickTimer;
extern VDWORD TimeTick;



//////////////////////////////////////////////////////
// AddEventToLog( void )
// adds event to log file
// example Alarm, Fill, Occurred, Component 5
//
//////////////////////////////////////////////////////

void AddEventToLog(  char *cEventType,char *cDescriptor1,char *cDescriptor2,char *cDescriptor3,char *cDescriptor4 )
{

    char    cBuffer[500];
    char    tBuffer[500];
    cBuffer[0] = '\0';
    sprintf( tBuffer, "%s\t%s\t%s\t%s\t%s\t",cEventType,cDescriptor1,cDescriptor2,cDescriptor3,cDescriptor4);
    strcat( cBuffer, tBuffer );
    sprintf( tBuffer, "%02d/%02d/%04d\t%02d:%02d:%02d\t",g_CurrentTime[TIME_DATE],g_CurrentTime[TIME_MONTH],g_CurrentTime[TIME_YEAR], g_CurrentTime[TIME_HOUR], g_CurrentTime[TIME_MINUTE], g_CurrentTime[TIME_SECOND] );
    strcat( cBuffer, tBuffer );
    sprintf( tBuffer, "%d\t", g_nCycleCounter  );    //
    strcat( cBuffer, tBuffer );

// date, time
// batch no.

// form the file name
    if(strlen(g_cEventLogFile) + strlen(cBuffer) < EVENTLOGSTRINGLENGTH)
    {
        WriteEventToSDCard(cBuffer);
    }

//    else
//    {
//        if(fdTelnet > 0)
//        iprintf("\n string length exceeded %d", strlen(g_cEventLogFile));
//    }
}



//////////////////////////////////////////////////////
// AddEventToLogPowerUp( void )
// function specially for power up.
//////////////////////////////////////////////////////

void AddEventToLogPowerUp(  char *cEventType,char *cDescriptor1,char *cDescriptor2,char *cDescriptor3,char *cDescriptor4 )
{

    char    cBuffer[500];
    char    tBuffer[500];
    char    hBuffer[30];
    WORD  wResult;
	BYTE nTimeBuf[3],byCRC;
    nTimeBuf[1] = (BYTE)( TimeTick & 0x000000FF);
    nTimeBuf[0] =  g_ProcessData.m_bySecurityFailErrorCode;
    byCRC = ComputeCRC8( nTimeBuf, 2);   //calculated cRC

    wResult = (nTimeBuf[1]*256)+ byCRC;
    sprintf(hBuffer,"%d",wResult);

//    SetupMttty();
//    iprintf("\n error code %x tick %x result is %x",nTimeBuf[0],nTimeBuf[1],wResult);

    cBuffer[0] = '\0';
    sprintf( tBuffer, "%s\t%s\t%s\t%s\t%s\t",cEventType,cDescriptor1,cDescriptor2,cDescriptor3,cDescriptor4);
    strcat( cBuffer, tBuffer );
    sprintf( tBuffer, "%02d/%02d/%04d\t%02d:%02d:%02d.",g_CurrentTime[TIME_DATE],g_CurrentTime[TIME_MONTH],g_CurrentTime[TIME_YEAR], g_CurrentTime[TIME_HOUR], g_CurrentTime[TIME_MINUTE], g_CurrentTime[TIME_SECOND] );
    strcat( cBuffer, tBuffer );
    strcat( cBuffer, hBuffer );
    sprintf( tBuffer, "\t%d\t", g_nCycleCounter  );    //
    strcat( cBuffer, tBuffer );

// date, time
// batch no.

// form the file name
    if(strlen(g_cEventLogFile) + strlen(cBuffer) < EVENTLOGSTRINGLENGTH)
    {
        WriteEventToSDCard(cBuffer);
    }

}





//////////////////////////////////////////////////////
// AddEventToLog( void )
// adds event to log file
// example Alarm, Fill, Occurred, Component 5
//
//////////////////////////////////////////////////////


void WriteEventToSDCard(char *cBuffer)
{
    if(((g_CalibrationData.m_nLogToSDCard & LOG_EVENTS_TO_SD_CARD) != 0) || g_CalibrationData.m_bEventLogDumpFlag)
    {
        strcat( g_cEventLogFile, cBuffer );
        strcat( g_cEventLogFile, strNewLine );
        sprintf( g_cEventName, "Evnt%02d%02d", g_CurrentTime[TIME_YEAR] - TIME_CENTURY,g_CurrentTime[TIME_MONTH]);     // Counts per int. stage 1.
        strcat( g_cEventName, ".txt");
        sprintf(g_cEventLogFileCopy,g_cEventLogFile);

        if(!g_bCycleIndicate && !g_bWriteEventLogToSdCard && !g_bTelnetSDCardAccessInProgress)
        {
            if((g_CalibrationData.m_nLogToSDCard & LOG_EVENTS_TO_SD_CARD) != 0)
        	{
        		WriteToSdCardFile(g_cEventName,g_cEventLogFile);
        	}
            DumpEvent();
            g_cEventLogFile[0] = '\0';
        }
        else
        {
            g_bWriteEventLogToSdCard = TRUE;
        }
    }
}



void DumpEvent(void)
{
        if((g_CalibrationData.m_nDumpDeviceType == DUMP_DEVICE_USB) && g_CalibrationData.m_bEventLogDumpFlag )  // use 1 for USB memory stick.      if( 1 )     // tthis works.
        {
          strncpy( g_sUSB.m_cEvLogFileName, g_cEventName,USBFILENAMESTRINGLENGTH);     // save the filename for diagnostics
          g_sUSB.m_bStartEvLogWrite = TRUE;
        }
        else if( (g_CalibrationData.m_nDumpDeviceType == DUMP_DEVICE_PRINTER) && g_CalibrationData.m_bEventLogDumpFlag )  // use 0 for UsB printer.
        {
           StartUSBPrint( g_cEventLogFile, FALSE );
        }


}


/////////////////////////////////////////////////////
// AddAlarmOccurredToEventLog()
//
// writes alarm occurred to alarm log
//////////////////////////////////////////////////////
void AddAlarmOccurredToEventLog(unsigned int ucAlarmCode,  int nComponent )
{
    char    cComponentNo[20];
    char    cBuffer[100];
    char    tBuffer[100];
    char    *cAlarmText;
    char    cAlarmCodeText[ALARMCODETEXTSIZE];
    sprintf( cComponentNo, "%02d",nComponent );
    cAlarmText = GetAlarmText(ucAlarmCode);
    sprintf(cAlarmCodeText, "Code=%02x  ", ucAlarmCode);

    if(ucAlarmCode == LEAKALARM)
    {
        sprintf(cBuffer,cAlarmText);
        sprintf(tBuffer," eoc kgs %2.3f dumped kgs %2.3f ",g_fOneSecondHopperWeightAtEndOfCycle,g_fOneSecondHopperWeight);
        strcat(cBuffer,tBuffer);
        cAlarmText = cBuffer;
        AddEventToLog( strEvlAlarm, strEvlOccurred,cAlarmCodeText,cAlarmText,cComponentNo );
    }
    else
    {
        AddEventToLog( strEvlAlarm, strEvlOccurred,cAlarmCodeText,cAlarmText,cComponentNo );
    }
// additional info

}

/////////////////////////////////////////////////////
// AddAlarmClearedToEventLog()
//
// writes alarm cleared to event log
//////////////////////////////////////////////////////
void AddAlarmClearedToEventLog(unsigned int ucAlarmCode,  int nComponent )
{
    char    cAlarmType[20];
    char    cBuffer[100];
    char    tBuffer[100];
    char    *cAlarmText;
    char    cComponentNo[20];
    char    cAlarmCodeText[ALARMCODETEXTSIZE];
    sprintf( cAlarmType, "%02d",ucAlarmCode );
    sprintf( cComponentNo, "%02d",nComponent );
    cAlarmText = GetAlarmText(ucAlarmCode);
    sprintf(cAlarmCodeText, "Code=%02x  ", ucAlarmCode);

    if(ucAlarmCode == LEAKALARM)
    {
        sprintf(cBuffer,cAlarmText);
        sprintf(tBuffer," eoc kgs %2.3f dumped kgs %2.3f ",g_fOneSecondHopperWeightAtEndOfCycle,g_fOneSecondHopperWeight);
        strcat(cBuffer,tBuffer);
        cAlarmText = cBuffer;
        AddEventToLog( strEvlAlarm, strEvlCleared,cAlarmCodeText,cAlarmText,cComponentNo );
    }
    else
    {
        AddEventToLog( strEvlAlarm, strEvlCleared,cAlarmCodeText,cAlarmText,cComponentNo );
    }
}

/////////////////////////////////////////////////////
// GetAlarmText()
//
//////////////////////////////////////////////////////
char* GetAlarmText(unsigned int ucAlarmCode)
{
    char *cTextString;

        switch( ucAlarmCode )
        {
        case NOFILLALARM:
            cTextString = strFillAlarm;
            break;
        case ZEROLSALARM:
            cTextString = strZeroLinespeedAlarm;
            break;
        case MIXSTOPALARM:
            cTextString = strMixingMotorStoppedAlarm;
            break;
        case CONTROLALARM:
            cTextString = strControlAlarm;
            break;
       case CALINVALARM:
            cTextString = strInvalidCalibrationAlarm;
            break;
        case LSMONITORALARM:
            cTextString = strLevelSensorMonitorAlarm;
            break;
        case ATTARWTALARM:
            cTextString = strAtTargetWeightAlarm;
            break;
        case SEICOMMSALARM:
            cTextString = strSEICommunicationsAlarm;
            break;
        case SEIRESETALARM:
            cTextString = strSEIResetAlarm;
            break;
       case PERALARM:
            cTextString = strComponentNot100PercentAlarm;
            break;
        case LEVELSENSORALARM:
            cTextString = strStarvationAlarm;
            break;
        case LEAKALARM:
            cTextString = strLeakAlarm;
            break;
        case LOWLEVALARM:
            cTextString = strBinLowLevelAlarm;
            break;
        case TAREALARM:
            cTextString = strTareAlarm;
            break;
        case SHUTDOWNALARM:
            cTextString = strShutDownAlarm;
            break;
        case LICENSEALARM:
            cTextString = strLicenseAlarm;
            break;
       case PERCENTAGEDEVIATIONALARM:
            cTextString = strDeviationAlarm;
            break;
       case HARDWARELICENSEALARM:
            cTextString = strHardwareLicenseAlarm;
            break;
       case DUMPFLAPHASNOTOPENED:
            cTextString = strDumpFlapHasNotOpenedAlarm;
            break;
       case SDCARDFAULTALARM:
            cTextString = strSDCardAlarm;
            break;
       case ATODMAXEXCEEDEDALARM:
            cTextString = strAToDMaxExceededAlarm;
            break;
       case I2CSTOPPEDALARM:
            cTextString = strI2CStopped;
            break;
       case COMPOPENTIMETOOSHORTALARM:
            cTextString = strComponentOpenTimeTooShort;
            break;
       case COMPOPENTIMETOOLONGALARM:
            cTextString = strComponentOpenTimeTooLong;
            break;
       case INVALIDCONFIGDATA:
            cTextString = strInvalidConfigData;
            break;
        case ATODENDOFCONVNOTCOMPLETE:
            cTextString = strAToDEOCNotCompleteAlarm;
            break;
        case ATODNEGATIVEPOLARITYALARM:
            cTextString = strAToDNegativePolarityAlarm;
            break;
       case HOPPERCALIBRATEDATZEROALARM:
            cTextString = strHopperCalibratedAtZeroAlarm;
            break;
       case VACUUMLOADERALARM:
            cTextString = strVacuumLoaderAlarm;
            break;
//wfh
       case US_SENSOR_1_ALARM:
            cTextString = strUltrasonicSensor1Alarm;	//"Alarm - Ultrasonic Sensor 1"
            break;
       case US_SENSOR_2_ALARM:
            cTextString = strUltrasonicSensor2Alarm;	//"Alarm - Ultrasonic Sensor 1"
            break;
       case US_SENSOR_3_ALARM:
            cTextString = strUltrasonicSensor3Alarm;	//"Alarm - Ultrasonic Sensor 1"
            break;
       case US_SENSOR_4_ALARM:
            cTextString = strUltrasonicSensor4Alarm;	//"Alarm - Ultrasonic Sensor 1"
            break;

        default:
            cTextString = strAlarmNotRecognised;
            break;

        }  // end of switch statement.

        return(cTextString);
}

/////////////////////////////////////////////////////
// AddBlenderRunToStopEventToLog()
//
//door opened to event log
/////////////////////////////////////////////////////
void AddBlenderRunToStopEventToLog( void )
{
    AddEventToLog( strEvlEvent,strEvlOccurred,strEvlStopRun,strBlenderRunToStopTransition, strEvlSpaces );
}

/////////////////////////////////////////////////////
// AddBlenderRunToStopEventToLog()
//
//door closed to event log
/////////////////////////////////////////////////////
void AddBlenderStopToRunEventToLog( void )
{
    AddEventToLog( strEvlEvent,strEvlOccurred,strEvlStopRun,strBlenderStopToRunTransition, strEvlSpaces );
}


/////////////////////////////////////////////////////
// AddBlenderToAutoEventToLog()
//
/////////////////////////////////////////////////////
void AddBlenderToAutoEventToLog( void )
{
    AddEventToLog( strEvlEvent,strEvlOccurred,strEvlAutoManual,strBlenderSwitchedToAuto, strEvlSpaces );
}

/////////////////////////////////////////////////////
// AddBlenderToManualEventToLog()
//
/////////////////////////////////////////////////////
void AddBlenderToManualEventToLog( void )
{
    AddEventToLog( strEvlEvent,strEvlOccurred,strEvlAutoManual,strBlenderSwitchedToManual, strEvlSpaces );
}



/////////////////////////////////////////////////////
// AddBlenderToManualControlAlarmEventToLog()
//
/////////////////////////////////////////////////////
void AddBlenderToManualControlAlarmEventToLog( void )
{
    AddEventToLog( strEvlEvent,strEvlOccurred,strEvlAutoManual,strBlenderSwitchedToManualControlAlarm, strEvlSpaces );
}



/////////////////////////////////////////////////////
// AddBlenderToManualControlAlarmEventToLog()
//
/////////////////////////////////////////////////////
void AddBlenderToManualPotControlEventToLog( void )
{
    AddEventToLog( strEvlEvent,strEvlOccurred,strEvlAutoManual,strBlenderSwitchedToManualPotControl, strEvlSpaces );
}

/////////////////////////////////////////////////////
// AddBlenderToManualSEIForcedExitEventToLog()
//
/////////////////////////////////////////////////////
void AddBlenderToManualSEIForcedExitEventToLog( void )
{
    AddEventToLog( strEvlEvent,strEvlOccurred,strEvlAutoManual,strBlenderSwitchedToManualSEIForcedExit, strEvlSpaces );
}


/////////////////////////////////////////////////////
// AddBlenderToManualSEIForcedExitEventToLog()
//
/////////////////////////////////////////////////////
void AddBlenderToManualZeroLinespeedEventToLog( void )
{
    AddEventToLog( strEvlEvent,strEvlOccurred,strEvlAutoManual,strBlenderSwitchedToManualZeroLinespeed, strEvlSpaces );
}

/////////////////////////////////////////////////////
// AddBlenderToManualLeakAlarmEventToLog()
//
/////////////////////////////////////////////////////
void AddBlenderToManualLeakAlarmEventToLog( void )
{
    AddEventToLog(strEvlEvent,strEvlOccurred,strEvlAutoManual, strBlenderSwitchedToManualLeakalarm, strEvlSpaces );
}

/////////////////////////////////////////////////////
// AddBlenderToManualLeakAlarmEventToLog()
//
/////////////////////////////////////////////////////
void AddBlenderToManualExtruderstalledEventToLog( void )
{
    AddEventToLog( strEvlEvent,strEvlOccurred,strEvlAutoManual,strBlenderSwitchedToManualExtruderstalled, strEvlSpaces );
}

/////////////////////////////////////////////////////
// AddBlenderToManualBlenderPausedEventToLog()
//
/////////////////////////////////////////////////////
void AddBlenderToManualBlenderPausedEventToLog( void )
{
    AddEventToLog( strEvlEvent,strEvlOccurred,strEvlAutoManual,strBlenderSwitchedToManualBlenderPausedEvent, strEvlSpaces );
}


/////////////////////////////////////////////////////
// AddBlenderToManualBlenderPausedEventToLog()
//
/////////////////////////////////////////////////////
void AddBlenderToManualIncDecSetpointErrorEventToLog( void )
{
    AddEventToLog( strEvlEvent,strEvlOccurred,strEvlAutoManual,strBlenderSwitchedToManualIncDecSetpointError, strEvlSpaces );
}

// AddBlenderToManualBlenderPausedEventToLog()
//
/////////////////////////////////////////////////////
void AddBlenderToManualStarvationEventToLog( void )
{
    AddEventToLog( strEvlEvent,strEvlOccurred,strEvlAutoManual,strBlenderSwitchedToManualStarvation, strEvlSpaces );
}

/////////////////////////////////////////////////////
// AddBlenderInstantVolCorrDueToLspeedChangeEventToLog()
//
/////////////////////////////////////////////////////
void AddBlenderInstantVolCorrDueToLspeedChangeEventToLog( void )
{
    AddEventToLog( strEvlEvent,strEvlOccurred,strEvlControl,strBlenderInstantVolCorrDueToLspeedChange, strEvlSpaces );
}

/////////////////////////////////////////////////////
// AddBlenderInstantVolCorrDueToLspeedChangeEventToLog()
//
/////////////////////////////////////////////////////
void AddPowerOnResetToEventLog( char *cText )
{
    char    cBuffer[100];

//   SetupMttty();
//    iprintf("\n error code %x tick %d hash is %d",nTimeBuf[0],nTimeBuf[1],wHash);
    if(g_nHashOfSHAIDInFlash == g_nHashOfSHAID)
    {
        sprintf( cBuffer, "Blender: %s %dE", g_CalibrationData.m_cEmailBlenderName,g_nHashOfSHAIDInFlash);     //
    }
    else
    {
        sprintf( cBuffer, "Blender: %s %dD", g_CalibrationData.m_cEmailBlenderName,g_nHashOfSHAIDInFlash);     //
     }
    AddEventToLogPowerUp( strEvlEvent,strEvlOccurred,strEvlGeneral,cText,cBuffer);
}



/////////////////////////////////////////////////////
// AddRecipeChangeToEventLog()
//
/////////////////////////////////////////////////////
void AddRecipeChangeToEventLog( void )
{
    AddEventToLog( strEvlEvent,strEvlOccurred,strEvlGeneral,strEvlBlenderRecipeChanged,strEvlSpaces);
}

/////////////////////////////////////////////////////
// AddWeightHopperTareToEventLog()
//
/////////////////////////////////////////////////////
void AddWeightHopperTareToEventLog( void )
{
    AddEventToLog( strEvlEvent,strEvlOccurred,strEvlGeneral,strEvlWeighHopperTared,strEvlSpaces);
}

/////////////////////////////////////////////////////
// AddWeightHopperCalibratedToEventLog()
//
/////////////////////////////////////////////////////
void AddWeightHopperCalibratedToEventLog( void )
{
    char    cBuffer[100];
    sprintf( cBuffer, "calibration weight %3.4f kgs tare %ld  constant %7.0f",g_fCalibrationWeight,g_CalibrationData.m_lTareCounts,g_CalibrationData.m_fWeightConstant);
    AddEventToLog( strEvlEvent,strEvlOccurred,strEvlGeneral,strEvlWeighHopperCalibrated,cBuffer);

}

/////////////////////////////////////////////////////
// AddSDCardLoadConfigCompleteToEventLog()
//
/////////////////////////////////////////////////////
void AddSDCardLoadConfigCompleteToEventLog( void )
{
    AddEventToLog( strEvlEvent,strEvlOccurred,strEvlSDCard,strEvlSdCardLoadConfigComplete,strEvlSpaces);
}

/////////////////////////////////////////////////////
// AddSDCardLoadBackupConfigCompleteToEventLog()
//
/////////////////////////////////////////////////////
void AddSDCardLoadBackupConfigCompleteToEventLog( void )
{
    AddEventToLog( strEvlEvent,strEvlOccurred,strEvlSDCard,strEvlSdCardLoadBackupConfigComplete,strEvlSpaces);
}

/////////////////////////////////////////////////////
// AddSDCardSaveConfigCompleteToEventLog()
//
/////////////////////////////////////////////////////
void AddSDCardSaveConfigCompleteToEventLog( void )
{
    AddEventToLog( strEvlEvent,strEvlOccurred,strEvlSDCard,strEvlSdCardSaveConfigComplete,strEvlSpaces);
}


/////////////////////////////////////////////////////
// AddSDCardCopyConfigToBackupCompleteToEventLog()
//
/////////////////////////////////////////////////////
void AddSDCardCopyConfigToBackupCompleteToEventLog( void )
{
    AddEventToLog( strEvlEvent,strEvlOccurred,strEvlSDCard,strEvlSDCardCopyConfigToBackup,strEvlSpaces);
}


/////////////////////////////////////////////////////
// AddSDCardCopyConfigToBackupCompleteToEventLog()
//
/////////////////////////////////////////////////////
void AddSDCardCleainingCycleInitiatedToEventLog( void )
{
    AddEventToLog( strEvlEvent,strEvlOccurred,strEvlGeneral,strEvlCleaningCycleStarted,strEvlSpaces);
}

/////////////////////////////////////////////////////
// AddSDCardBypassOpenedToEventLog()
//
/////////////////////////////////////////////////////
void AddSDCardBypassOpenedToEventLog( void )
{
    AddEventToLog( strEvlEvent,strEvlOccurred,strEvlGeneral,strEvlBypassOpened,strEvlSpaces);
}

/////////////////////////////////////////////////////
// AddSDCardBypassClosedToEventLog()
//
/////////////////////////////////////////////////////
void AddSDCardBypassClosedToEventLog( void )
{
    AddEventToLog( strEvlEvent,strEvlOccurred,strEvlGeneral,strEvlBypassClosed,strEvlSpaces);
}


/////////////////////////////////////////////////////
// AddOperatorPanelPageToEventLog()
// determine what page has been accessed on the panel
// and records the event log
/////////////////////////////////////////////////////
void AddOperatorPanelPageToEventLog( unsigned int nPageId  )
{
       switch( nPageId )
        {
        case OPPANEL_CONFIG_PAGE1_ID:
            AddEventToLog( strEvlEvent,strEvlOccurred,strEvlGeneral,strOperatorPanelCondigPage1,strEvlSpaces);
            break;
        case OPPANEL_CONFIG_PAGE2_ID:
            AddEventToLog( strEvlEvent,strEvlOccurred,strEvlGeneral,strOperatorPanelCondigPage2,strEvlSpaces);
            break;
        case OPPANEL_CONFIG_PAGE3_ID:
            AddEventToLog( strEvlEvent,strEvlOccurred,strEvlGeneral,strOperatorPanelCondigPage3,strEvlSpaces);
            break;
        case OPPANEL_LINE_SPEED_CALIBRATION_PAGE_ID:
            AddEventToLog( strEvlEvent,strEvlOccurred,strEvlGeneral,strOperatorPanelLspeedPage,strEvlSpaces);
            break;
        case OPPANEL_MEASUREMENT_CONTROL_PAGE_ID:
            AddEventToLog( strEvlEvent,strEvlOccurred,strEvlGeneral,strOperatorPanelMeasurementPage,strEvlSpaces);
            break;
        case OPPANEL_DEVIATION_PAGE_ID:
            AddEventToLog( strEvlEvent,strEvlOccurred,strEvlGeneral,strOperatorPanelDeviationPage,strEvlSpaces);
            break;
        case OPPANEL_BIN_CCONFIG_PAGE_ID:
            AddEventToLog( strEvlEvent,strEvlOccurred,strEvlGeneral,strOperatorPanelBinConfigPage,strEvlSpaces);
            break;
        case OPPANEL_MODE_PAGE_ID:
            AddEventToLog( strEvlEvent,strEvlOccurred,strEvlGeneral,strOperatorPanelModePage,strEvlSpaces);
            break;
        case OPPANEL_UPDATE_PAGE_ID:
            AddEventToLog( strEvlEvent,strEvlOccurred,strEvlGeneral,strOperatorPanelUpdatePage,strEvlSpaces);
            break;
        case OPPANEL_NETWORK_IP_CONFIG_PAGE_ID:
            AddEventToLog( strEvlEvent,strEvlOccurred,strEvlGeneral,strOperatorPanelNetworkConfigPage,strEvlSpaces);
            break;
        case OPPANEL_CLEANING_CONFIG_PAGE_ID:
            AddEventToLog( strEvlEvent,strEvlOccurred,strEvlGeneral,"Cleaning config page entered",strEvlSpaces);
            break;
        case OPPANEL_PASSWORD_CONFIG_PAGE_ID:
            AddEventToLog( strEvlEvent,strEvlOccurred,strEvlGeneral,strOperatorPanelPasswordConfigPage,strEvlSpaces);
            break;
        case OPPANEL_LICENSE_PAGE_ID:
            AddEventToLog( strEvlEvent,strEvlOccurred,strEvlGeneral,strOperatorPanelLicenseConfigPage,strEvlSpaces);
            break;
        case OPPANEL_OPTIMISATION_CONFIG_PAGE_ID:
            AddEventToLog( strEvlEvent,strEvlOccurred,strEvlGeneral,strOperatorPanelOptimisationConfigPage,strEvlSpaces);
            break;
        default:
            break;

        }  // end of switch statement.


}

/////////////////////////////////////////////////////
// AddIC2EventToLog()
//
/////////////////////////////////////////////////////
void AddIC2EventToLog( void )
{
        AddEventToLog( strI2CStopped, " ",strEvlGeneral," "," " );
}



/////////////////////////////////////////////////////
// AddLockToEventLog()
//
/////////////////////////////////////////////////////
void AddLockToEventLog( void )
{
        AddEventToLog( strEvlEvent,strEvlOccurred,strEvlGeneral,strEvlConfigLocked,strEvlSpaces);
}


/////////////////////////////////////////////////////
// AddUnLockToEventLog()
//
/////////////////////////////////////////////////////
void AddUnLockToEventLog( void )
{
    AddEventToLog( strEvlEvent,strEvlOccurred,strEvlGeneral,strEvlConfigUnLocked,strEvlSpaces);
}


/////////////////////////////////////////////////////
// AddModbusWriteEventToLog()
//
/////////////////////////////////////////////////////
void AddModbusWriteEventToLog(char *cText)
{
    WriteEventToSDCard(cText);
}

/////////////////////////////////////////////////////
// AddAToDInternalAToDTestPassedEventToLog()
//
/////////////////////////////////////////////////////
void AddAToDInternalAToDTestPassedEventToLog(void)
{
        AddEventToLog( strEvlEvent,strEvlOccurred,strEvlGeneral,strAToTestPassed,strEvlSpaces);
}

/////////////////////////////////////////////////////
// AddAToDInternalAToDTestFailedEventToLog()
//
/////////////////////////////////////////////////////
void AddAToDInternalAToDTestFailedEventToLog(void)
{
        AddEventToLog( strEvlEvent,strEvlOccurred,strEvlGeneral,strAToTestFailed,strEvlSpaces);
}

/////////////////////////////////////////////////////
// AddFileCouldNotBeOpenedEventToLog()
//
/////////////////////////////////////////////////////
void AddFileCouldNotBeOpenedEventToLog(void)
{
        AddEventToLog( strEvlEvent,strEvlOccurred,strEvlGeneral,strSDCardWriteError,strEvlSpaces);
}

/////////////////////////////////////////////////////
// AddSoftwareResetPendingEventToLog()
//
/////////////////////////////////////////////////////
void AddSoftwareResetPendingEventToLog(void)
{
       char    cBuffer[500];
       char    tBuffer[500];
       cBuffer[0] = '\0';
       sprintf( tBuffer, "%s\t%s\t%s\t%s\t", strEvlEvent,strEvlOccurred,strEvlGeneral,strWatchdogPending);
       strcat( cBuffer, tBuffer );
       sprintf( tBuffer, "%02d/%02d/%04d\t%02d:%02d:%02d\t",g_CurrentTime[TIME_DATE],g_CurrentTime[TIME_MONTH],g_CurrentTime[TIME_YEAR], g_CurrentTime[TIME_HOUR], g_CurrentTime[TIME_MINUTE], g_CurrentTime[TIME_SECOND] );
       strcat( cBuffer, tBuffer );
       sprintf( tBuffer, "%d\t", g_nCycleCounter  );    //
       strcat( cBuffer, tBuffer );
       sprintf(tBuffer,"Function %2d subfn %2d",g_nFunctionRunning,g_nFunctionSubSectionRunning);
       strcat( cBuffer, tBuffer );
       WriteEventToSDCard(cBuffer);
}

/////////////////////////////////////////////////////
// AddModbusTCPFatalErrorToLog()
//
/////////////////////////////////////////////////////
void AddModbusTCPFatalErrorToLog(void)
{
    AddEventToLog( strEvlEvent,strEvlOccurred,strEvlGeneral,strModbusTCPFatalError,strEvlSpaces);
}


