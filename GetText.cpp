//////////////////////////////////////////////////////
//
// P.Smith                          19/11/08
// gets the alarm text for different config data.
//
// P.Smith                          24/4/09
// GetSensitivityText,GetPurgeText,GetDiagnosticsText
// GetParityText,GetUSBDeviceTypeText
//
// P.Smith                          30/6/09
// added GetVacuumLoaderFilterTypeText
//
// P.Smith                          24/8/09
// in GetBlenderTypeText use name OPTIMIX150 instead of MICRABATCHUNUSED
//
// P.Smith                          18/1/10
// added GetSpeedText
//
// P.Smith                          4/5/10
// added GetBaudRateText
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
#include "Lspeed.h"
#include "InitNBBComms.h"



extern   char      *striBlendingOnly;
extern   char      *striOffline;
extern   char      *striThroughputMonitor;
extern   char      *striZeroToTenMaxThroughput;
extern   char      *striKgHrControl;
extern   char      *striGPMGPMEntry;
extern   char      *striGPMGPMEntryxxkgs;
extern   char      *striGPMWLEntry;
extern   char      *striGPMMicronEntry;
extern   char      *striGPSM;
extern   char      *striGP12SqFeet;
extern   char      *striMultiBlend;
extern   char      *striModeNotKnown;

extern   char      *strTSM1000;
extern   char      *strTSM1500;
extern   char      *strTSM350;
extern   char      *strTSM650;
extern   char      *strTSM3000;
extern   char      *strOPTIMIX;
extern   char      *strTSM150;
extern   char      *strTSM50;
extern   char      *strModelNotKnown;

extern   char      *strDisabled;
extern   char      *str2stageFilling;
extern   char      *str3stageFilling;

extern   char      *strMetric;
extern   char      *strSemiImperial;
extern   char      *strImperial;

extern   char      *strSEI;
extern   char      *strLLS;
extern   char      *strSEILLS;

extern   char      *strFullClean;
extern   char      *strMixerClean;

extern   char      *strAutoRGt100;
extern   char      *strManualRGt100;
extern   char      *strAutoREq100;
extern   char      *strManualREq100;

extern   char      *strPulses;
extern   char      *strComms;
extern   char      *strTacho;

extern   char      *strExtrusion;
extern   char      *strHaulOff;
extern   char      *strVFExtrusion;
extern   char      *strVFHaulOff;
extern   char      *strOfflineHiLo;
extern   char      *strOfflineXXkgs;
extern   char      *strZeroToTenControl;
extern   char      *strIncreaseDecreaseControl;
extern   char      *strDeviationAlarmWithPause;
extern   char      *strDeviationAlarmWithoutPause;
extern   char      *strHigh;
extern   char      *strLow;
extern   char      *strEnabled;
extern   char      *strNone;
extern   char      *strEvenParity;
extern   char      *strAutocycle;
extern   char      *strNormal;
extern   char      *strManualPurge;
extern   char      *strAutoPurge;
extern   char      *strUSBDevice;
extern   char      *strPrinterDevice;
extern   char      *strTSMFilterType;
extern   char      *strPCFilterType;
extern   char      *strFast;
extern  char      *strBaudRate9600;
extern  char      *strBaudRate19200;
extern  char      *strBaudRate38400;
extern  char      *strBaudRate57600;
extern  char      *strBaudRate115200;




/////////////////////////////////////////////////////
// GetBlenderTypeText()
//
//////////////////////////////////////////////////////



char* GetBlenderTypeText(unsigned int nMode)
{
    char *cTextString;

        switch( nMode )
        {
        case TSM1000:
            cTextString = strTSM1000;
            break;
        case TSM1500:
            cTextString = strTSM1500;
            break;
        case TSM650:
            cTextString = strTSM650;
            break;
        case TSM3000:
            cTextString = strTSM3000;
            break;
        case OPTIMIX150:
            cTextString = strOPTIMIX;
            break;
        case TSM150:
            cTextString = strTSM150;
            break;
        case TSM350:
            cTextString = strTSM350;
            break;
        case TSM50:
            cTextString = strTSM50;
            break;
        default:
            cTextString = strModelNotKnown;
            break;

        }  // end of switch statement.
        return(cTextString);
}


/////////////////////////////////////////////////////
// GetBlenderModeText()
// returns a pointer to blender mode text
//
//////////////////////////////////////////////////////

char* GetBlenderModeText(unsigned int nMode)
{
    char *cTextString;

        switch( nMode )
        {
        case MODE_BLENDINGONLY:
            cTextString = striBlendingOnly;
            break;
        case MODE_OFFLINEMODE:
            cTextString = striOffline;
            break;
        case MODE_THROUGHPUTMON:
            cTextString = striThroughputMonitor;
            break;
        case MODE_MAXTHROUGHPUTZT:
            cTextString = striZeroToTenMaxThroughput;
            break;
        case MODE_KGHCONTROLMODE:
            cTextString = striKgHrControl;
            break;
        case MODE_GPMGPMENTRY:
            cTextString = striGPMGPMEntry;
            break;
        case MODE_GPMGPMENXXKGS:
            cTextString = striGPMGPMEntryxxkgs;
            break;
        case MODE_GPMWTPLENTRY:
            cTextString = striGPMWLEntry;
            break;
        case MODE_GPMMICRONENTRY:
            cTextString = striGPMMicronEntry;
            break;
        case MODE_GPSQMENTRY:
            cTextString = striGPSM;
            break;
        case MODE_GP12SQFEET:
            cTextString = striGP12SqFeet;
            break;
        case MODE_SINGLERECIPEMODE:
            cTextString = striMultiBlend;
            break;

        default:
            cTextString = striModeNotKnown;
            break;

        }  // end of switch statement.
        return(cTextString);
}


/////////////////////////////////////////////////////
// GetMultiStageFillText()
// returns a pointer to  text
//
//////////////////////////////////////////////////////

char* GetMultiStageFillText(unsigned int nMode)
{
    char *cTextString;

        switch( nMode )
        {
        case 0:
            cTextString = strDisabled;
            break;
        case TWOSTAGEFILLINGID:
            cTextString = str2stageFilling;
            break;
        case THREESTAGEFILLINGID:
            cTextString = str3stageFilling;
            break;

        default:
            cTextString = "not recognised";
            break;

        }  // end of switch statement.
        return(cTextString);
}

/////////////////////////////////////////////////////
// GetUnitsText()
// returns a pointer to  text
//
//////////////////////////////////////////////////////


char* GetUnitsText(unsigned int nMode)
{
    char *cTextString;

        switch( nMode )
        {
        case UNIT_METRIC:
            cTextString = strMetric;
            break;
        case UNIT_IMPERIAL:
            cTextString = strImperial;
            break;
        case UNIT_IMPERIAL_MODBUS:
            cTextString = strSemiImperial;
            break;

        default:
            cTextString = "units not recognised";
            break;

        }  // end of switch statement.
        return(cTextString);
}


/////////////////////////////////////////////////////
// GetRemoteModulesText()
// returns a pointer to  text
//
//////////////////////////////////////////////////////


char* GetRemoteModulesText(unsigned int nMode)
{
    char *cTextString;

        switch( nMode )
        {
        case 0:
            cTextString = strDisabled;
            break;
        case SEIENBITPOS:
            cTextString = strSEI;
            break;
        case (LLSENBITPOS):
            cTextString = strLLS;
            break;
        case (SEIENBITPOS | LLSENBITPOS):
            cTextString = strSEILLS;
            break;

        default:
            cTextString = "remote modules not recognised";
            break;

        }  // end of switch statement.
        return(cTextString);
}

/////////////////////////////////////////////////////
// GetCleaningText()
// returns a pointer to  text
//
//////////////////////////////////////////////////////


char* GetCleaningText(unsigned int nMode)
{
    char *cTextString;

        switch( nMode )
        {
        case 0:
            cTextString = strDisabled;
            break;
        case FULLCLEAN:
            cTextString = strFullClean;
            break;
        case MIXERCLEAN:
            cTextString = strMixerClean;
            break;
        default:
            cTextString = "cleaning mode not recognised";
            break;

        }  // end of switch statement.
        return(cTextString);
}

/////////////////////////////////////////////////////
// GetRecipeEntryModeText()
// returns a pointer to  text
//
//////////////////////////////////////////////////////


char* GetRecipeEntryModeText(unsigned int nMode)
{
    char *cTextString;

        switch( nMode )
        {
        case 0:
            cTextString = strAutoRGt100;
            break;
        case 1:
            cTextString = strManualRGt100;
            break;
        case 2:
            cTextString = strAutoREq100;
            break;
        case 3:
            cTextString = strManualREq100;
            break;
        default:
            cTextString = "entry mode not recognised";
            break;

        }  // end of switch statement.
        return(cTextString);
}

/////////////////////////////////////////////////////
// GetLineSpeedSignalText()
// returns a pointer to  text
//
//////////////////////////////////////////////////////

char* GetLineSpeedSignalText(unsigned int nMode)
{
    char *cTextString;

        switch( nMode )
        {
        case PULSES_LINE_SPEED:
            cTextString = strPulses;
            break;
        case COMMUNICATION_LINE_SPEED:
            cTextString = strComms;
            break;
        default:
            cTextString = "lspeed mode not recognised";
            break;

        }  // end of switch statement.
        return(cTextString);
}

/////////////////////////////////////////////////////
// GetScrewSpeedSignalText()
// returns a pointer to  text
//
//////////////////////////////////////////////////////


char* GetScrewSpeedSignalText(unsigned int nMode)
{
    char *cTextString;

        switch( nMode )
        {
        case SCEW_SPEED_SIGNAL_COMMS:
            cTextString = strComms;
            break;
        case SCEW_SPEED_SIGNAL_TACHO:
            cTextString = strTacho;
            break;
        default:
            cTextString = "sspeed mode not recognised";
            break;

        }  // end of switch statement.
        return(cTextString);
}

/////////////////////////////////////////////////////
// GetScrewSpeedSignalText()
// returns a pointer to  text
//
//////////////////////////////////////////////////////


char* GetControlMethodText(unsigned int nMode)
{
    char *cTextString;

        switch( nMode )
        {
        case CONTROL_MODE_EXTRUSION:
            cTextString = strExtrusion;
            break;
        case CONTROL_MODE_HAULOFF:
            cTextString = strHaulOff;
            break;
        case (CONTROL_MODE_EXTRUSION | CONTROL_MODE_VOLTAGE_FOLLOWER):
            cTextString = strVFExtrusion;
            break;
        case (CONTROL_MODE_HAULOFF | CONTROL_MODE_VOLTAGE_FOLLOWER):
            cTextString = strVFHaulOff;
            break;
        default:
            cTextString = "control mode not recognised";
            break;

        }  // end of switch statement.
        return(cTextString);
}

/////////////////////////////////////////////////////
// GetScrewSpeedSignalText()
// returns a pointer to  text
//
//////////////////////////////////////////////////////


char* GetOfflineTypeText(unsigned int nMode)
{
    char *cTextString;

        switch( nMode )
        {

        case OFFLINETYPE_HILO:
            cTextString = strOfflineHiLo;
            break;
        case OFFLINETYPE_XXKG:
            cTextString = strOfflineXXkgs;
            break;
        default:
            cTextString = "offline mode not recognised";
            break;

        }  // end of switch statement.
        return(cTextString);
}


/////////////////////////////////////////////////////
// GetControlTypeText()
// returns a pointer to  text
//
//////////////////////////////////////////////////////


char* GetControlTypeText(unsigned int nMode)
{
    char *cTextString;

        switch( nMode )
        {

        case CONTROL_TYPE_0_10:
            cTextString = strZeroToTenControl;
            break;
        case CONTROL_TYPE_INC_DEC:
            cTextString = strIncreaseDecreaseControl;
            break;
        default:
            cTextString = "offline mode not recognised";
            break;

        }  // end of switch statement.
        return(cTextString);
}


/////////////////////////////////////////////////////
// GetDeviationAlarmEnable()
// returns a pointer to  text
//
//////////////////////////////////////////////////////


char* GetDeviationAlarmEnableText(unsigned int nMode)
{
    char *cTextString;

        switch( nMode )
        {
        case 0:
            cTextString = strDisabled;
            break;
        case COMPPERENABLEDWITHNOPAUSE:
            cTextString = strDeviationAlarmWithoutPause;
            break;
        case COMPPERENABLEDWITHPAUSE:
            cTextString = strDeviationAlarmWithPause;
            break;
        default:
            cTextString = "deviation alarm type not recognised";
            break;

        }  // end of switch statement.
        return(cTextString);
}



/////////////////////////////////////////////////////
// GetSensitivityText()
// returns a pointer to  text
//
//////////////////////////////////////////////////////


char* GetSensitivityText(unsigned int nMode)
{
    char *cTextString;

        switch( nMode )
        {
        case 0:
            cTextString = strLow;
            break;
        case 1:
            cTextString = strHigh;
            break;
        default:
            cTextString = "sensitivity type not recognised";
            break;

        }  // end of switch statement.
        return(cTextString);
}


/////////////////////////////////////////////////////
// GetPurgeText()
// returns a pointer to  text
//
//////////////////////////////////////////////////////
char* GetPurgeText(unsigned int nMode)
{
    char *cTextString;

        switch( nMode )
        {
        case 0:
            cTextString = strDisabled;
            break;
        case MANUAL_PURGE:
            cTextString = strManualPurge;
            break;
        case AUTO_PURGE:
            cTextString = strAutoPurge;
            break;
        default:
            cTextString = "purge type not recognised";
            break;

        }  // end of switch statement.
        return(cTextString);
}



/////////////////////////////////////////////////////
// GetDiagnosticsText()
// returns a pointer to  text
//
//////////////////////////////////////////////////////
char* GetDiagnosticsText(unsigned int nMode)
{
    char *cTextString;

        switch( nMode )
        {
        case 0:
            cTextString = strNone;
            break;
        case 1:
            cTextString = strAutocycle;
            break;
        default:
            cTextString = "diagnostics type not recognised";
            break;

        }  // end of switch statement.
        return(cTextString);
}


/////////////////////////////////////////////////////
// GetParityText()
// returns a pointer to  text
//
//////////////////////////////////////////////////////
char* GetParityText(unsigned int nMode)
{
    char *cTextString;

        switch( nMode )
        {
        case 0:
            cTextString = strEvenParity;
            break;
        case 1:
            cTextString = strNone;
            break;
        default:
            cTextString = "parity text not recognised";
            break;

        }  // end of switch statement.
        return(cTextString);
}

/////////////////////////////////////////////////////
// GetUSBDeviceType()
// returns a pointer to  text
//
//////////////////////////////////////////////////////
char* GetUSBDeviceTypeText(unsigned int nMode)
{
    char *cTextString;

        switch( nMode )
        {

        case DUMP_DEVICE_DISABLED:
            cTextString = strNone;
            break;
        case DUMP_DEVICE_USB:
            cTextString = strUSBDevice;
            break;
        case DUMP_DEVICE_PRINTER:
            cTextString = strPrinterDevice;
            break;
        default:
            cTextString = "usb type text not recognised";
            break;

        }  // end of switch statement.
        return(cTextString);
}



/////////////////////////////////////////////////////
// GetVacuumLoaderFilterTypeText()
// returns a pointer to  text
//
//////////////////////////////////////////////////////
char* GetVacuumLoaderFilterTypeText(unsigned int nMode)
{
    char *cTextString;

        switch( nMode )
        {

        case FILTER_TYPE_NONE:
            cTextString = strNone;
            break;
        case FILTER_TYPE_TSM:
            cTextString = strTSMFilterType;
            break;
        case FILTER_TYPE_PC:
            cTextString = strPCFilterType;
            break;
        default:
            cTextString = "filter type text not recognised";
            break;

        }  // end of switch statement.
        return(cTextString);
}

/////////////////////////////////////////////////////
// GetSpeedText()
// returns a pointer to  text
//
//////////////////////////////////////////////////////
char* GetSpeedText(BOOL bMode)
{
    char *cTextString;
    if(bMode)
            cTextString = strNormal;
    else
        cTextString = strFast;

    return(cTextString);
}

/////////////////////////////////////////////////////
// GetBaudRateText()
// returns a pointer to  text
//
//////////////////////////////////////////////////////
char* GetBaudRateText(unsigned int nMode)
{
    char *cTextString;
    switch( nMode )
     {

     case BAUD_RATE_9600_ID:
         cTextString = strBaudRate9600;
         break;
     case BAUD_RATE_19200_ID:
         cTextString = strBaudRate19200;
         break;
     case BAUD_RATE_38400_ID:
         cTextString = strBaudRate38400;
         break;
     case BAUD_RATE_57600_ID:
         cTextString = strBaudRate57600;
         break;
     case BAUD_RATE_115200_ID:
         cTextString = strBaudRate115200;
         break;
     default:
         cTextString = "baud rate text not recognised";
         break;

     }  // end of switch statement.
     return(cTextString);
}

