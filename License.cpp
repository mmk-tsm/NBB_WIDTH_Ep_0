//
// P.Smith                              20/9/05
// added CheckEncryptionEntry
//
// P.Smith                              27/09/05
// added CheckTemporaryHourlyCounter
//
// P.Smith                              23-09-2005
// added CheckLicensePowerUpStatus,DecideOnLicenseExpiryWarning
//
// P.Smith                              4/10/05
// Rename low priority todos
//
//
// P.Smith                              7/10/05
// Fully implement software & licensing.
//
// P.Smith                              19/10/05
// Software id check is corrected -> single equals instead of double.
// update the software id from the random number generator.
//
// P.Smith                              20/12/05
// in DecideOnLicenseExpiryWarning check for permanent & temporary license.
//
//
// P.Smith                              10/1/06
// removed unused Decryptedcode,
//
// P.Smith                      28/6/06
// first pass at netburner hardware conversion.
//#include <basictypes.h>
// changed unions IntsAndLong to WordAndDWord
//
// M.McKiernan                      17/9/07
// Removed g_arrnMBTable[] & g_arrnMBTableFlags[], now in BatVars2
//
// P.Smith                      29/1/08
// added GenerateHashForSoftwareID call to generate hash for software id.
// change generation of software id to be more random, use rtc seconds and minutes.
//
// P.Smith                          23/7/08
// remove g_arrnMBTable,g_CurrentTime externs
//
// P.Smith                          17/9/09
// add in LICENSE_VACUUMLOADING instead of LICENSE_REPORTOPTIONBIT
//
// P.Smith                          17/2/10
// check for LICENSE_ETHERNETIP and set m_cPermanentOptions accordingly.
//
// P.Smith                          5/3/10
// now a code for each option is used to enable the license.
// added LicenseFunction to determine what license option is selected.
// added AllLicenseOptionsOn,AllLicenseOptionsOff
// CheckOptionsForValidLicense checks if various options are on
// if they are not licensed, they are turned off.
// lls, topup,offline,bypass, cleaning
//
// P.Smith                          8/3/10
// added LICENSE_ALLOW_BATCH_WEIGHT_CHECK
//
// P.Smith                          10/3/10
// corrections to LICENSE_MODBUS_TCP_OFF, use wdata
// LICENSE_CLEANING_OFFLICENSE_CLEANING_OFF
// LICENSE_CLEANING_OFF remove unused line
//
// P.Smith                          25/3/10
// added LICENSE_LINK_NBB_TO_NETBURNER_MODULE to link nbb
// and netburner module together.
//
// P.Smith                          12/4/10
// ShowLicenseInfo formed to show the license data in the same
// format that it is shown on the touch panel.
//
// P.Smith                          12/4/10
// when enabling the run license, set hour counter to 0
// when switching off run license, clear temporary license
// set run counter to 0
// implement LICENSE_ALLOW_UNRESTRICED_ACCESS, set g_nAllowUnrestricedAccess
// to UNRESTRICED_ACCESS
// check kg/hr and grams per meter license option, if options are disabled
// switch blender to blending only mode
// added CheckForUnRestrictedAccessToLicening, check for licensing options to be written
// MapMBLicensingOptions & MapMBLicensingOptionsNo2 to allow licensing options
// and type and run license to be set.
// ReadSHAHashes called when nbb linked to netburner module
//
// P.Smith                          20/4/10
// a problem has been seen in the license check for the kg/hr mode.
// the mb bit LICENSEMB_KGHROPTIONBIT is being checked instead of LICENSE_KGHROPTIONBIT
//
// P.Smith                          21/4/10
// when the license option was set from the mb table, the result was not getting saved
// set g_bSaveAllCalibrationToOnBoardEEprom to true.
//
// P.Smith                          4/5/10
// license option entry is redirected to allow the sap code to be generated to set up
// the blenders.
// change MapMBLicensingOptions & MapMBLicensingOptionsNo2 to pass the modbus data that
// is being used to set up the blender. This allows the function to be called when
// the licensing code is entered and the blender is in unrestriced access.
//////////////////////////////////////////////////////

//*****************************************************************************
// INCLUDES
//*****************************************************************************
#include "General.h"
#include "BatVars.h"
#include "BatVars2.h"
#include "KghCalc.h"
#include "ConversionFactors.h"
#include "BatchCalibrationFunctions.h"
#include "SetpointFormat.h"
#include "BatchMBIndices.h"
#include "Conalg.h"
#include "Alarms.h"
#include "TSMPeripheralsMBIndices.h"
#include "Lspeed.h"
#include "BlRep.h"
#include "InitialiseHardware.h"
#include "Pause.h"
#include "CycleMonitor.h"

#include "License.h"
#include "Itoa.h"
#include "String.h"
#include "Stdlib.h"
#include "Vaccyc.h"
#include <basictypes.h>
#include "Hash.h"
#include <stdio.h>
#include "MBProgs.h"
#include "Mbvacldr.h"
#include "vacvars.h"
#include "security.h"



extern  CalDataStruct   g_CalibrationData;
extern  structSetpointData  g_CurrentRecipe;
extern  OnBoardCalDataStruct   g_OnBoardCalData;
extern   structVacCalib   g_VacCalibrationData;

OnBoardEndCalDataStruct g_OnBoardEndCalData;
unsigned int g_nAllowUnrestricedAccess = 0;

BOOL	g_bLicenseOptions1Written = FALSE;
BOOL	g_bLicenseOptions2Written = FALSE;
BOOL	g_bLicenseOptions3Written = FALSE;
BOOL	g_bLicenseOptions4Written = FALSE;



//////////////////////////////////////////////////////
// CheckEncryptionEntry( void )         from ASM = ENTERENCRYPTIONDATA
//
//
//////////////////////////////////////////////////////


void CheckEncryptionEntry( unsigned long ulCode )
{

unsigned long   ulEncyptionData,lValue,lSoftwareID,ulCodeTemp;

int nCarryrhs,nCarry,i,unDecryptedcode;
unsigned    int nMsOptions,nLsOptions,nTimePeriod,nOptions;
unsigned    int unEncyptionKey;
union WordAndDWord ilDecrypted;
char    cBuf[20],cSoftwareID[5],cOptions[7];
WORD	wMSData,wLSData;
	if(g_nAllowUnrestricedAccess == UNRESTRICED_ACCESS)
	{
		wMSData = (WORD)((ulCode >> 16) & 0xffff);
		wLSData = (WORD)(ulCode  & 0xffff);
		if(fdTelnet>0)
		{
			iprintf("\n wMSData %d wLSData %d",wMSData,wLSData);
		}
		MapMBLicensingOptions(wMSData);
		MapMBLicensingOptionsNo2(wLSData);
	}
	else
	{

// merge GENENCRYPTIONKEY inline

		ulCodeTemp = ulCode;
		nCarryrhs = 0;
		nCarry = 0;

		ulEncyptionData = g_OnBoardCalData.m_lSoftwareID;

		ulEncyptionData ^= 0xffff;

		for (i = 0; i < 8; i++)
		{
			nCarryrhs = ulEncyptionData & 0x0001;    /* store carryrhs */
			ulEncyptionData >>=1;           /* shift data */
			if (nCarry==0)
			{
				ulEncyptionData &= 0x7FFF;  /* and result. */
			}
			else
			{
				ulEncyptionData |= 0x8000; /* or result */
			}
			nCarry = nCarryrhs;    ; /* copy carry value before shift */
			if (nCarry == 1)
			{
				ulEncyptionData ^= 0xA001;
			}
		}

		ulEncyptionData ^= 0xffff;

		unEncyptionKey = ulEncyptionData;
		unDecryptedcode = ilDecrypted.nValue[1];   // only ls word used in decryption
		for (i=0; i<4; i++)
		{
			ulCode ^= unEncyptionKey;      /* exclusive or */
			unEncyptionKey >>= 4;             /* right shift */
		}

		IntToAscii( ulCode, cBuf, 10, 10 );

		lValue = atol(cBuf);
		memcpy(&cSoftwareID, &cBuf[6],sizeof( cSoftwareID ));

		lSoftwareID = atol(cSoftwareID);

		memcpy(&cOptions, &cBuf[0],sizeof( cOptions ));
		cOptions[6] = ASCII_NULL;
		lValue = atol(cOptions);

		nMsOptions = (int)(cBuf[3]) & 0x0f;    // read options msb
		nLsOptions = (int)(cBuf[4]) & 0x0f;    // read options lsb
		nOptions = (nMsOptions  << 4) | nLsOptions;
		nTimePeriod = (int)(cBuf[5]) & 0x0f;   // read time period

		if(lSoftwareID == g_OnBoardCalData.m_lSoftwareID)
    //  if(1)   //nbb--testonly
		{
			LicenseFunction(lValue); // check for license function
			//LicenseFunction(ulCodeTemp); // check for license function  nbb--testonly

    // merge COPYSOFTWAREID inline
			g_OnBoardCalData.m_lSoftwareID = ((rand() *  g_CurrentTime[TIME_MINUTE]^ g_CurrentTime[TIME_SECOND]) & 0x1fff);   // use random number generator.
			GenerateHashForSoftwareID();
			g_bSaveAllCalibrationToOnBoardEEprom = TRUE;        // save calibration data
		}

	}
}

//////////////////////////////////////////////////////
// CheckTemporaryHourlyCounter              from ASM = CHECKFORTEMPOPTION
//
//
//////////////////////////////////////////////////////


void CheckTemporaryHourlyCounter( void )
{
unsigned char cData,cTemporaryOptions;

    cTemporaryOptions = g_OnBoardCalData.m_cTemporaryOptions;

    if(g_OnBoardCalData.m_cTemporaryOptions & LICENSE_BLENDERRUNOPTIONBIT)
    {
        if(g_OnBoardCalData.m_nBlenderRunHourCounter != 0)
        {
            g_OnBoardCalData.m_nBlenderRunHourCounter--;
            if(g_OnBoardCalData.m_nBlenderRunHourCounter & 0x01) // only save enery second hour
            {
                g_bSaveAllCalibrationToOnBoardEEprom = TRUE;        // save calibration data
            }
            if(g_OnBoardCalData.m_nBlenderRunHourCounter == 0)
            {
                g_bShowExpiryPage = TRUE;
                cData = LICENSE_BLENDERRUNOPTIONBIT ^ 0xff;    // form complement
                g_OnBoardCalData.m_cTemporaryOptions &= cData; // reset option

            }
        }
        if(cTemporaryOptions != g_OnBoardCalData.m_cTemporaryOptions)
        {
            g_bSaveAllCalibrationToOnBoardEEprom = TRUE;        // save calibration data
        }
    }
}


//////////////////////////////////////////////////////
// LicenseFunction()
// determines license functionality
// invokes the license command
//
//////////////////////////////////////////////////////
void LicenseFunction( unsigned long lValue)
{
	unsigned char cData = 0;
	WORD wData;

	if(lValue == LICENSE_BLENDER_PERMANENT_RUN_ON)
	{
		g_OnBoardCalData.m_cPermanentOptions |= LICENSE_BLENDERRUNOPTIONBIT;
		cData = LICENSE_BLENDERRUNOPTIONBIT ^ 0xFF;
		g_OnBoardCalData.m_cTemporaryOptions &= cData;
		g_OnBoardCalData.m_nBlenderRunHourCounter = 0;
	}
	else
	if(lValue == LICENSE_BLENDER_PERMANENT_RUN_OFF)
	{
        cData = LICENSE_BLENDERRUNOPTIONBIT ^ 0xff;    // form complement
        g_OnBoardCalData.m_cPermanentOptions &= cData; // reset option
		cData = LICENSE_BLENDERRUNOPTIONBIT ^ 0xFF;
		g_OnBoardCalData.m_cTemporaryOptions &= cData;
		g_OnBoardCalData.m_nBlenderRunHourCounter = 0;
	}
	else
	if(lValue == LICENSE_BLENDER_TEMPORARY_RUN_ON_ONE_YEAR)
	{
		g_OnBoardCalData.m_nBlenderRunHourCounter = LICENSE_ONE_YEAR;
		g_OnBoardCalData.m_cTemporaryOptions |= LICENSE_BLENDERRUNOPTIONBIT;
        cData = LICENSE_BLENDERRUNOPTIONBIT ^ 0xff;    // form complement
        g_OnBoardCalData.m_cPermanentOptions &= cData; // reset option
	}
	else
	if(lValue == LICENSE_BLENDER_TEMPORARY_RUN_ON_SIX_MONTHS)
	{
		g_OnBoardCalData.m_nBlenderRunHourCounter = LICENSE_SIX_MONTHS;
		g_OnBoardCalData.m_cTemporaryOptions |= LICENSE_BLENDERRUNOPTIONBIT;
        cData = LICENSE_BLENDERRUNOPTIONBIT ^ 0xff;    // form complement
        g_OnBoardCalData.m_cPermanentOptions &= cData; // reset option
	}
	else
	if(lValue == LICENSE_BLENDER_TEMPORARY_RUN_ON_THREE_MONTHS)
	{
		g_OnBoardCalData.m_nBlenderRunHourCounter = LICENSE_THREE_MONTHS;
		g_OnBoardCalData.m_cTemporaryOptions |= LICENSE_BLENDERRUNOPTIONBIT;
        cData = LICENSE_BLENDERRUNOPTIONBIT ^ 0xff;    // form complement
        g_OnBoardCalData.m_cPermanentOptions &= cData; // reset option
	}
	else
	if(lValue == LICENSE_BLENDER_TEMPORARY_RUN_ON_ONE_MONTH)
	{
		g_OnBoardCalData.m_nBlenderRunHourCounter = LICENSE_ONE_MONTH;
		g_OnBoardCalData.m_cTemporaryOptions |= LICENSE_BLENDERRUNOPTIONBIT;
        cData = LICENSE_BLENDERRUNOPTIONBIT ^ 0xff;    // form complement
        g_OnBoardCalData.m_cPermanentOptions &= cData; // reset option
	}
	else
	if(lValue == LICENSE_BLENDER_TEMPORARY_RUN_ON_ONE_WEEK)
	{
		g_OnBoardCalData.m_nBlenderRunHourCounter = LICENSE_ONE_WEEK;
		g_OnBoardCalData.m_cTemporaryOptions |= LICENSE_BLENDERRUNOPTIONBIT;
        cData = LICENSE_BLENDERRUNOPTIONBIT ^ 0xff;    // form complement
        g_OnBoardCalData.m_cPermanentOptions &= cData; // reset option
	}
	else
	if(lValue == LICENSE_BLENDER_TEMPORARY_RUN_ON_ONE_DAY)
	{
		g_OnBoardCalData.m_nBlenderRunHourCounter = LICENSE_ONE_DAY;
		g_OnBoardCalData.m_cTemporaryOptions |= LICENSE_BLENDERRUNOPTIONBIT;
        cData = LICENSE_BLENDERRUNOPTIONBIT ^ 0xff;    // form complement
        g_OnBoardCalData.m_cPermanentOptions &= cData; // reset option
	}
	else
	if(lValue == LICENSE_BLENDER_TEMPORARY_RUN_ON_FIVE_HOURS)
	{
		g_OnBoardCalData.m_nBlenderRunHourCounter = LICENSE_FIVE_HOURS;
		g_OnBoardCalData.m_cTemporaryOptions |= LICENSE_BLENDERRUNOPTIONBIT;
        cData = LICENSE_BLENDERRUNOPTIONBIT ^ 0xff;    // form complement
        g_OnBoardCalData.m_cPermanentOptions &= cData; // reset option
	}
	else
	if(lValue == LICENSE_BLENDER_TEMPORARY_RUN_ON_TWO_HOURS)
	{
		g_OnBoardCalData.m_nBlenderRunHourCounter = LICENSE_TWO_HOURS;
		g_OnBoardCalData.m_cTemporaryOptions |= LICENSE_BLENDERRUNOPTIONBIT;
        cData = LICENSE_BLENDERRUNOPTIONBIT ^ 0xff;    // form complement
        g_OnBoardCalData.m_cPermanentOptions &= cData; // reset option
	}
	else
	if(lValue == LICENSE_ALL_OPTIONS_OFF)
	{
		AllLicenseOptionsOff();
	}
	else
	if(lValue == LICENSE_ALL_OPTIONS_ON)
	{
		AllLicenseOptionsOn();
	}
	else
	if(lValue == LICENSE_VACUUM_LOADING_ON)
	{
        g_OnBoardCalData.m_cPermanentOptions |= LICENSE_VACUUMLOADING;
	}
	else
	if(lValue == LICENSE_VACUUM_LOADING_OFF)
	{
        cData = LICENSE_VACUUMLOADING ^ 0xff;    // form complement
        g_OnBoardCalData.m_cPermanentOptions &= cData;
	}
	else
	if(lValue == LICENSE_MODBUS_TCP_ON)
	{
		g_OnBoardCalData.m_wBlenderOptions |= LICENSE_MODBUSTCP;
	}
	else
	if(lValue == LICENSE_MODBUS_TCP_OFF)
	{
		wData = LICENSE_MODBUSTCP ^ 0xffff;    // form complement
		g_OnBoardCalData.m_wBlenderOptions &= wData;
	}
	else
	if(lValue == LICENSE_ETHERNET_IP_ON)
	{
        g_OnBoardCalData.m_cPermanentOptions |= LICENSE_ETHERNETIP;
	}
	else
	if(lValue == LICENSE_ETHERNET_IP_OFF)
	{
        cData = LICENSE_ETHERNETIP ^ 0xff;    // form complement
        g_OnBoardCalData.m_cPermanentOptions &= cData;
	}
	else
	if(lValue == LICENSE_KGPH_EXTRUSION_CONTROL_ON)
	{
        g_OnBoardCalData.m_cPermanentOptions |= LICENSE_KGHROPTIONBIT;

 	}
	else
	if(lValue == LICENSE_KGPH_EXTRUSION_CONTROL_OFF)
	{
		cData = LICENSE_KGHROPTIONBIT ^ 0xff;    //
		g_OnBoardCalData.m_cPermanentOptions &= cData;
 	}
	else
	if(lValue == LICENSE_GMPM_EXTRUSION_CONTROL_ON)
	{
        g_OnBoardCalData.m_cPermanentOptions |= LICENSE_GPMOPTIONBIT;
 	}
	else
	if(lValue == LICENSE_GMPM_EXTRUSION_CONTROL_OFF)
	{
		cData = LICENSE_GPMOPTIONBIT ^ 0xff;    //
		g_OnBoardCalData.m_cPermanentOptions &= cData;
 	}

// now check if blender model should be set.
	else
	if(lValue == LICENSE_SET_BLENDER_MODEL_OPTIMIX50)
	{
		   g_CalibrationData.m_nBlenderType = OPTIMIX50;
	       g_bSaveAllCalibrationToEEprom = TRUE;       // set flag to cause all calibration data to be written to EEPROM.
			CopyConfigDataToMB();
 	}
	else
	if(lValue == LICENSE_SET_BLENDER_MODEL_OPTIMIX150)
	{
		   g_CalibrationData.m_nBlenderType = OPTIMIX150;
	       g_bSaveAllCalibrationToEEprom = TRUE;       // set flag to cause all calibration data to be written to EEPROM.
		   CopyConfigDataToMB();
 	}
	else
	if(lValue == LICENSE_SET_BLENDER_MODEL_OPTIMIX250)
	{
 	    g_CalibrationData.m_nBlenderType = OPTIMIX250;
	    g_bSaveAllCalibrationToEEprom = TRUE;       // set flag to cause all calibration data to be written to EEPROM.
	    CopyConfigDataToMB();
 	}
	else
	if(lValue == LICENSE_SET_BLENDER_MODEL_OPTIMIX350)
	{
		g_CalibrationData.m_nBlenderType = TSM350;
	    g_bSaveAllCalibrationToEEprom = TRUE;       // set flag to cause all calibration data to be written to EEPROM.
		CopyConfigDataToMB();
	}
	else
	if(lValue == LICENSE_SET_BLENDER_MODEL_OPTIMIX650)
	{
		g_CalibrationData.m_nBlenderType = TSM650;
	    g_bSaveAllCalibrationToEEprom = TRUE;       // set flag to cause all calibration data to be written to EEPROM.
		CopyConfigDataToMB();
	}
	else
	if(lValue == LICENSE_SET_BLENDER_MODEL_OPTIMIX1000)
	{
		g_CalibrationData.m_nBlenderType = TSM1000;
	    g_bSaveAllCalibrationToEEprom = TRUE;       // set flag to cause all calibration data to be written to EEPROM.
		CopyConfigDataToMB();
	}
	else
	if(lValue == LICENSE_SET_BLENDER_MODEL_OPTIMIX1500)
	{
		g_CalibrationData.m_nBlenderType = TSM1500;
	    g_bSaveAllCalibrationToEEprom = TRUE;       // set flag to cause all calibration data to be written to EEPROM.
		CopyConfigDataToMB();
	}
	else
	if(lValue == LICENSE_SET_BLENDER_MODEL_OPTIMIX3000)
	{
		g_CalibrationData.m_nBlenderType = TSM3000;
		g_bSaveAllCalibrationToEEprom = TRUE;       // set flag to cause all calibration data to be written to EEPROM.
		CopyConfigDataToMB();
	}
	if(lValue == LICENSE_SET_BLENDER_MODEL_TSM50)
	{
		g_CalibrationData.m_nBlenderType = TSM50;
		g_bSaveAllCalibrationToEEprom = TRUE;       // set flag to cause all calibration data to be written to EEPROM.
		CopyConfigDataToMB();
	}
	else
	if(lValue == LICENSE_SET_BLENDER_MODEL_TSM150)
	{
		g_CalibrationData.m_nBlenderType = TSM150;
		g_bSaveAllCalibrationToEEprom = TRUE;       // set flag to cause all calibration data to be written to EEPROM.
		CopyConfigDataToMB();
	}
	else
	if(lValue == LICENSE_CLEANING_ON)
	{
        wData = (LICENSE_FULLCLEANOPTIONBIT | LICENSE_MIXERCLEANOPTIONBIT);
        g_OnBoardCalData.m_wBlenderOptions |= wData;
	}
	else
	if(lValue == LICENSE_CLEANING_OFF)
	{
        wData = (LICENSE_FULLCLEANOPTIONBIT | LICENSE_MIXERCLEANOPTIONBIT)^ 0xffff;
		g_OnBoardCalData.m_wBlenderOptions &= wData;
	}
	else
	if(lValue == LICENSE_LLS_ON)
	{
        g_OnBoardCalData.m_cPermanentOptions |= LICENSE_LLSOPTIONBIT;
	}
	else
	if(lValue == LICENSE_LLS_OFF)
	{
 		cData = LICENSE_LLSOPTIONBIT ^ 0xff;    //
		g_OnBoardCalData.m_cPermanentOptions &= cData;
	}

	else
	if(lValue == LICENSE_TOPUP_ON)
	{
        g_OnBoardCalData.m_wBlenderOptions |= LICENSE_TOPUPOPTIONBIT;
	}
	else
	if(lValue == LICENSE_TOPUP_OFF)
	{
		wData = LICENSE_TOPUPOPTIONBIT ^ 0xffff;    //
		g_OnBoardCalData.m_wBlenderOptions &= wData;
	}
	else
	if(lValue == LICENSE_FULL_CLEAN_ON)
	{
        g_OnBoardCalData.m_wBlenderOptions |= LICENSE_FULLCLEANOPTIONBIT;
	}


	else
	if(lValue == LICENSE_MIXER_CLEAN_ON)
	{
        g_OnBoardCalData.m_wBlenderOptions |= LICENSE_MIXERCLEANOPTIONBIT;
	}

	else
	if(lValue == LICENSE_BIN_CLEAN_DRAIN_ON)
	{
        g_OnBoardCalData.m_wBlenderOptions |= LICENSE_BINCLEANOPTIONBIT;
	}
	else
	if(lValue == LICENSE_BIN_CLEAN_DRAIN_OFF)
	{
        g_OnBoardCalData.m_wBlenderOptions |= LICENSE_BINCLEANOPTIONBIT;
        wData = LICENSE_BINCLEANOPTIONBIT ^ 0xffff;    //
		g_OnBoardCalData.m_wBlenderOptions &= wData;
	}
	else
	if(lValue == LICENSE_BYASS_MODE_ON)
	{
        g_OnBoardCalData.m_wBlenderOptions |= LICENSE_BYPASSOPTIONBIT;
	}
	else
	if(lValue == LICENSE_BYASS_MODE_OFF)
	{
		wData = LICENSE_BYPASSOPTIONBIT ^ 0xffff;    //
 		g_OnBoardCalData.m_wBlenderOptions &= wData;
	}
	else
	if(lValue == LICENSE_OFFLINE_MODE_HIGH_LOW_SENSORS_ON)
	{
        g_OnBoardCalData.m_wBlenderOptions |= LICENSE_OFFLINEHIGHLOWOPTIONBIT;
	}
	else
	if(lValue == LICENSE_OFFLINE_MODE_HIGH_LOW_SENSORS_OFF)
	{
		wData = LICENSE_OFFLINEHIGHLOWOPTIONBIT ^ 0xffff;    //
 		g_OnBoardCalData.m_wBlenderOptions &= wData;
	}
	if(lValue == LICENSE_OFFLINE_MODE_BLEND_TO_XXKGS_ON)
	{
        g_OnBoardCalData.m_wBlenderOptions |= LICENSE_OFFLINEBLENDTOXXKGSBIT;
	}
	if(lValue == LICENSE_OFFLINE_MODE_BLEND_TO_XXKGS_OFF)
	{
		wData = LICENSE_OFFLINEBLENDTOXXKGSBIT ^ 0xffff;    //
 		g_OnBoardCalData.m_wBlenderOptions &= wData;
 	}

	if(lValue == LICENSE_ALLOW_BATCH_WEIGHT_CHECK)
	{
        g_OnBoardCalData.m_wBlenderOptions |= LICENSE_DO_BATCH_WEIGHT_CHECK;
	}
	if(lValue == LICENSE_DISABLE_BATCH_WEIGHT_CHECK)
	{
		wData = LICENSE_DO_BATCH_WEIGHT_CHECK ^ 0xffff;    //
 		g_OnBoardCalData.m_wBlenderOptions &= wData;
 	}
	if(lValue == LICENSE_LINK_NBB_TO_NETBURNER_MODULE)
	{
		ReadAndStoreSHAInFlash();
		ReadSHAHashes();  // read sha hashes
 	}
	if(lValue == LICENSE_ALLOW_UNRESTRICED_ACCESS)
	{
		g_nAllowUnrestricedAccess = UNRESTRICED_ACCESS;
 	}

}


//////////////////////////////////////////////////////
// DecideOnLicenseExpiryWarning          (from asm = DECIDEONEXPIRYPAGE)
//
//
//
// P.Smith                          27-09-2005
//
//////////////////////////////////////////////////////
void DecideOnLicenseExpiryWarning( void )
{
    if(((g_OnBoardCalData.m_cPermanentOptions & LICENSE_BLENDERRUNOPTIONBIT) == 0) && ((g_OnBoardCalData.m_cTemporaryOptions & LICENSE_BLENDERRUNOPTIONBIT) == 0))
    {
        // merge SETLICENSEALARM inline
        if( (g_arrnMBTable[BATCH_SUMMARY_SYSTEM_ALARMS] & LICENSEALARMBITPOS) == 0 ) // license bit not set???
        {
            PutAlarmTable( LICENSEALARM, 0 );     // indicate alarm,  //PTALTB
        }
           // --todolp-- EXPIREDPAGE display expired page

    }
    else
    {
        if(g_OnBoardCalData.m_nBlenderRunHourCounter !=0)
        {
            if(g_OnBoardCalData.m_nBlenderRunHourCounter > LICENSE_WARNINGHOURS)
            {//NOEXPRY
                if(g_arrnMBTable[BATCH_SUMMARY_SYSTEM_ALARMS] & LICENSEALARMBITPOS) // bit set?
                {
                    RemoveAlarmTable( LICENSEALARM,  0 );     // RMALTB
                }

            }
            else
            {
            // merge SETLICENSEALARM inline
                if( (g_arrnMBTable[BATCH_SUMMARY_SYSTEM_ALARMS] & LICENSEALARMBITPOS) == 0 ) // license bit not set???
                {
                    PutAlarmTable( LICENSEALARM, 0 );     // indicate alarm,  //PTALTB
                }
                    // --todolp-- WILLEXPIREPAGE display license will expire

            }
        }
    }
}

//////////////////////////////////////////////////////
// AllLicenseOptionsOn
//
//
//////////////////////////////////////////////////////
void AllLicenseOptionsOn( void )
{
	g_OnBoardCalData.m_cPermanentOptions = 0xff;        // permanent options
	g_OnBoardCalData.m_cTemporaryOptions = 0xff;        // temporary options
	g_bSaveAllCalibrationToOnBoardEEprom = TRUE;        // save calibration data
	g_OnBoardCalData.m_wBlenderOptions = 0xffff;       // switch options on
}


//////////////////////////////////////////////////////
// AllLicenseOptionsOff
//
//
//////////////////////////////////////////////////////
void AllLicenseOptionsOff( void )
{
// merge RESETALLOPTIONS inline.
g_OnBoardCalData.m_cPermanentOptions = 0;           // reset permanent options
g_OnBoardCalData.m_cTemporaryOptions = 0;           // reset temporary options
g_OnBoardCalData.m_nBlenderRunHourCounter = 0;      // blender run counter reset. //--review-- other counters are not necessary
g_bSaveAllCalibrationToOnBoardEEprom = TRUE;        // save calibration data
g_OnBoardCalData.m_wBlenderOptions = 0;			   // switch options off
}

//////////////////////////////////////////////////////
// CheckOptionsForValidLicense();
//
// function to check that the required licensing option
// are enabled for different functions
//////////////////////////////////////////////////////
void CheckOptionsForValidLicense( void )
{
	unsigned char cData;
	BOOL bConfigChanged = FALSE;

	if((g_CalibrationData.m_nPeripheralCardsPresent & LLSENBITPOS) != 0)
    {
	    if((g_OnBoardCalData.m_cPermanentOptions & LICENSE_LLSOPTIONBIT) == 0)
	    {
            cData = LLSENBITPOS ^ 0xff;    // form complement
            g_CalibrationData.m_nPeripheralCardsPresent &= cData; // reset option
	    	bConfigChanged = TRUE;
	    }
     }
    if(g_CalibrationData.m_bTopUpMode)
    {
	    if((g_OnBoardCalData.m_wBlenderOptions & LICENSE_TOPUPOPTIONBIT) == 0)
	    {
           g_CalibrationData.m_bTopUpMode = FALSE;   //
	    	bConfigChanged = TRUE;
	    }
     }
// offline mode
    if(g_CalibrationData.m_nBlenderMode == MODE_OFFLINEMODE)
    {
    	if(g_CalibrationData.m_nOfflineType == OFFLINETYPE_HILO)
    	{
    		if((g_OnBoardCalData.m_wBlenderOptions & LICENSE_OFFLINEHIGHLOWOPTIONBIT) == 0)
    		{
    			g_CalibrationData.m_nBlenderMode = MODE_BLENDINGONLY;
    	    	bConfigChanged = TRUE;
    		}
    	}
    	else
        if(g_CalibrationData.m_nOfflineType == OFFLINETYPE_XXKG)
        {
            if((g_OnBoardCalData.m_wBlenderOptions & LICENSE_OFFLINEBLENDTOXXKGSBIT) == 0)
        	{
        		g_CalibrationData.m_nBlenderMode = MODE_BLENDINGONLY;
        	    bConfigChanged = TRUE;
        	}
        }
    }
// bypass mode
    if(g_CalibrationData.m_bBypassMode)
	{
	    if((g_OnBoardCalData.m_wBlenderOptions & LICENSE_BYPASSOPTIONBIT) == 0)
	    {
	    	g_CalibrationData.m_bBypassMode = FALSE;
    	    bConfigChanged = TRUE;
	    }
	}


    // cleaning check
    if(g_CalibrationData.m_nCleaning == FULLCLEAN)
	{
	    if((g_OnBoardCalData.m_wBlenderOptions & LICENSE_FULLCLEANOPTIONBIT) == 0)
	    {
	    	g_CalibrationData.m_nCleaning = 0;
    	    bConfigChanged = TRUE;
	    }
	}
    if(g_CalibrationData.m_nCleaning == MIXERCLEAN)
	{
	    if((g_OnBoardCalData.m_wBlenderOptions & LICENSE_MIXERCLEANOPTIONBIT) == 0)
	    {
	    	g_CalibrationData.m_nCleaning = 0;
    	    bConfigChanged = TRUE;
	    }
	}
// check vacuum loading

    if(g_VacCalibrationData.m_nLoaders != 0) // vacuum loading enabled
	{
	    if((g_OnBoardCalData.m_cPermanentOptions & LICENSE_VACUUMLOADING) == 0)
	    {
	    	g_VacCalibrationData.m_nLoaders = 0;
	    	g_bSaveLoaderCalibrationData = TRUE;
	    	CopyVacuumLoaderCalibrationParametersToMB();

 	    }
	}
    // kg/hr control
    if(g_CalibrationData.m_nBlenderMode == MODE_KGHCONTROLMODE)
    {
	    if((g_OnBoardCalData.m_cPermanentOptions & LICENSE_KGHROPTIONBIT) == 0)
	    {
	    g_CalibrationData.m_nBlenderMode = MODE_BLENDINGONLY;
	    bConfigChanged = TRUE;
	    }
    }
    //g/m
    if((g_CalibrationData.m_nBlenderMode >= MODE_GPMGPMENTRY) && (g_CalibrationData.m_nBlenderMode <= MODE_GP12SQFEET))
    {
    	if((g_OnBoardCalData.m_cPermanentOptions & LICENSE_GPMOPTIONBIT) == 0)
    	{
      	    g_CalibrationData.m_nBlenderMode = MODE_BLENDINGONLY;
    		bConfigChanged = TRUE;
    	}
    }

    if(bConfigChanged)
    {
        g_bSaveAllCalibrationToEEprom = TRUE;       // set flag to cause all calibration data to be written to EEPROM.
    	CopyConfigDataToMB();
    }
}



//////////////////////////////////////////////////////
// ShowLicenseInfo
// indicates what licensing options are set
//
//////////////////////////////////////////////////////
void ShowLicenseInfo( void )
{
	char cBuffer[500];
	char tBuffer[50];
	if((g_OnBoardCalData.m_cPermanentOptions & LICENSE_BLENDERRUNOPTIONBIT) != 0)
	{
		sprintf(cBuffer,"\nRUN  : Yes");
	}
	else
	{
		sprintf(cBuffer,"\nRUN  : No");
	}

	if((g_OnBoardCalData.m_cPermanentOptions & LICENSE_KGHROPTIONBIT) != 0)
	{
		sprintf(tBuffer,"\nKGH  : Yes");
	}
	else
	{
		sprintf(tBuffer,"\nKGH  : No");
	}
	strcat(cBuffer,tBuffer);
	if((g_OnBoardCalData.m_cPermanentOptions & LICENSE_GPMOPTIONBIT) != 0)
	{
		sprintf(tBuffer,"\nGPM  : Yes");
	}
	else
	{
		sprintf(tBuffer,"\nGPM  : No");
	}
	strcat(cBuffer,tBuffer);

	// offline high low
	if((g_OnBoardCalData.m_wBlenderOptions & LICENSE_OFFLINEHIGHLOWOPTIONBIT) != 0)
	{
		sprintf(tBuffer,"\nOHL  : Yes");
	}
	else
	{
		sprintf(tBuffer,"\nOHL  : No");
	}
	strcat(cBuffer,tBuffer);

	// offline blend to xx
	if((g_OnBoardCalData.m_wBlenderOptions & LICENSE_OFFLINEBLENDTOXXKGSBIT) != 0)
	{
		sprintf(tBuffer,"\nOKG  : Yes");
	}
	else
	{
		sprintf(tBuffer,"\nOKG  : No");
	}
	strcat(cBuffer,tBuffer);
	// lls
	if((g_OnBoardCalData.m_cPermanentOptions & LICENSE_LLSOPTIONBIT) != 0)
	{
		sprintf(tBuffer,"\nLLS  : Yes");
	}
	else
	{
		sprintf(tBuffer,"\nLLS  : No");
	}
	strcat(cBuffer,tBuffer);

	// high accuracy
	if((g_OnBoardCalData.m_wBlenderOptions & LICENSE_TOPUPOPTIONBIT) != 0)
	{
		sprintf(tBuffer,"\nHA   : Yes");
	}
	else
	{
		sprintf(tBuffer,"\nHA   : No");
	}
	strcat(cBuffer,tBuffer);

	// high accuracy
	if((g_OnBoardCalData.m_wBlenderOptions & LICENSE_DO_BATCH_WEIGHT_CHECK) != 0)
	{
		sprintf(tBuffer,"\nBWC  : Yes");
	}
	else
	{
		sprintf(tBuffer,"\nBWC  : No");
	}
	strcat(cBuffer,tBuffer);

	// vacuum loading
	if((g_OnBoardCalData.m_cPermanentOptions & LICENSE_VACUUMLOADING) != 0)
	{
		sprintf(tBuffer,"\nVAC  : Yes");
	}
	else
	{
		sprintf(tBuffer,"\nVAC  : No");
	}
	strcat(cBuffer,tBuffer);

	// cleaning all
	if((g_OnBoardCalData.m_wBlenderOptions & LICENSE_FULLCLEANOPTIONBIT) != 0)
	{
		sprintf(tBuffer,"\nCTOT : Yes");
	}
	else
	{
		sprintf(tBuffer,"\nCTOT : No");
	}
	strcat(cBuffer,tBuffer);

	// cleaning mixer
	if((g_OnBoardCalData.m_wBlenderOptions & LICENSE_MIXERCLEANOPTIONBIT) != 0)
	{
		sprintf(tBuffer,"\nCMIX : Yes");
	}
	else
	{
		sprintf(tBuffer,"\nCMIX : No");
	}
	strcat(cBuffer,tBuffer);

	// cleaning BIN
	if((g_OnBoardCalData.m_wBlenderOptions & LICENSE_BINCLEANOPTIONBIT) != 0)
	{
		sprintf(tBuffer,"\nCBIN : Yes");
	}
	else
	{
		sprintf(tBuffer,"\nCBIN : No");
	}
	strcat(cBuffer,tBuffer);

	// bypass
	if((g_OnBoardCalData.m_wBlenderOptions & LICENSE_BYPASSOPTIONBIT) != 0)
	{
		sprintf(tBuffer,"\nCBYP : Yes");
	}
	else
	{
		sprintf(tBuffer,"\nCBYP : No");
	}
	strcat(cBuffer,tBuffer);
	// mbtcp
	if((g_OnBoardCalData.m_wBlenderOptions & LICENSE_MODBUSTCP) != 0)
	{
		sprintf(tBuffer,"\nCTCP : Yes");
	}
	else
	{
		sprintf(tBuffer,"\nCTCP : No");
	}
	strcat(cBuffer,tBuffer);

	// eip
	if((g_OnBoardCalData.m_cPermanentOptions & LICENSE_ETHERNETIP) != 0)
	{
		sprintf(tBuffer,"\nCEIP : Yes");
	}
	else
	{
		sprintf(tBuffer,"\nCEIP : No");
	}
	strcat(cBuffer,tBuffer);
	if(fdTelnet >0)
	{
		iprintf("\n%s",cBuffer);
	}

}


//////////////////////////////////////////////////////
// CheckForUnRestrictedAccessToLicening
// if unrestricted access and license options are written
// the appropriate action is taken
//
//////////////////////////////////////////////////////
void CheckForUnRestrictedAccessToLicening( void )
{
	if(g_nAllowUnrestricedAccess == UNRESTRICED_ACCESS)
	{
		if(g_bLicenseOptions1Written)
		{
			g_bLicenseOptions1Written = FALSE;
			MapMBLicensingOptions(g_arrnMBTable[BATCH_SET_LICENSE_OPTIONS_1]);
		}
		if(g_bLicenseOptions2Written)
		{
			g_bLicenseOptions2Written = FALSE;
			MapMBLicensingOptionsNo2(g_arrnMBTable[BATCH_SET_LICENSE_OPTIONS_2]);
		}
		if(g_bLicenseOptions3Written)
		{
			long lCode = g_arrnMBTable[BATCH_SET_LICENSE_OPTIONS_3];
			g_bLicenseOptions3Written = FALSE;
			LicenseFunction(lCode);
			g_bSaveAllCalibrationToOnBoardEEprom = TRUE;        // save calibration data
		}
		if(g_bLicenseOptions4Written)
		{
		}
	}
}



//////////////////////////////////////////////////////
// MapMBLicensingOptions
//
//
//////////////////////////////////////////////////////
void MapMBLicensingOptions(WORD wOptions )
{
	WORD wData;
	if((wOptions & LICENSEMB_KGHROPTIONBIT)!=0)
	{
        g_OnBoardCalData.m_cPermanentOptions |= LICENSE_KGHROPTIONBIT;
	}
	else
	{
		wData = LICENSE_KGHROPTIONBIT ^ 0xffff;    // form complement
		g_OnBoardCalData.m_cPermanentOptions &= wData;

	}
	if((wOptions & LICENSEMB_ETHERNETIP)!=0)
	{
        g_OnBoardCalData.m_cPermanentOptions |= LICENSE_ETHERNETIP;
	}
	else
	{
		wData = LICENSE_ETHERNETIP ^ 0xffff;    // form complement
		g_OnBoardCalData.m_cPermanentOptions &= wData;
	}
	if((wOptions & LICENSEMB_GPMOPTIONBIT)!=0)
	{
        g_OnBoardCalData.m_cPermanentOptions |= LICENSE_GPMOPTIONBIT;
	}
	else
	{
		wData = LICENSE_GPMOPTIONBIT ^ 0xffff;    // form complement
		g_OnBoardCalData.m_cPermanentOptions &= wData;
	}
	if((wOptions & LICENSEMB_LLSOPTIONBIT)!=0)
	{
	       g_OnBoardCalData.m_cPermanentOptions |= LICENSE_LLSOPTIONBIT;
	}
	else
	{
		wData = LICENSE_LLSOPTIONBIT ^ 0xffff;    // form complement
		g_OnBoardCalData.m_cPermanentOptions &= wData;
	}
	if((wOptions & LICENSEMB_VACUUMLOADING)!=0)
	{
        g_OnBoardCalData.m_cPermanentOptions |= LICENSE_VACUUMLOADING;
	}
	else
	{
		wData = LICENSE_VACUUMLOADING ^ 0xffff;    // form complement
		g_OnBoardCalData.m_cPermanentOptions &= wData;
	}
	if((wOptions & LICENSEMB_MODBUSTCP)!=0)
	{
		g_OnBoardCalData.m_wBlenderOptions |= LICENSE_MODBUSTCP;
	}
	else
	{
		wData = LICENSE_MODBUSTCP ^ 0xffff;    // form complement
		g_OnBoardCalData.m_wBlenderOptions &= wData;
	}
	if((wOptions & LICENSEMB_TOPUPOPTIONBIT)!=0)
	{
        g_OnBoardCalData.m_wBlenderOptions |= LICENSE_TOPUPOPTIONBIT;
	}
	else
	{
		wData = LICENSE_TOPUPOPTIONBIT ^ 0xffff;    // form complement
		g_OnBoardCalData.m_wBlenderOptions &= wData;
	}
	if((wOptions & LICENSEMB_FULLCLEANOPTIONBIT)!=0)
	{
		g_OnBoardCalData.m_wBlenderOptions |= LICENSE_FULLCLEANOPTIONBIT;
	}
	else
	{
		wData = LICENSE_FULLCLEANOPTIONBIT ^ 0xffff;    // form complement
		g_OnBoardCalData.m_wBlenderOptions &= wData;
	}
	if((wOptions & LICENSEMB_MIXERCLEANOPTIONBIT)!=0)
	{
		g_OnBoardCalData.m_wBlenderOptions |= LICENSE_MIXERCLEANOPTIONBIT;

	}
	else
	{
		wData = LICENSE_MIXERCLEANOPTIONBIT ^ 0xffff;    // form complement
		g_OnBoardCalData.m_wBlenderOptions &= wData;
	}
	if((wOptions & LICENSEMB_BINCLEANOPTIONBIT)!=0)
	{
		g_OnBoardCalData.m_wBlenderOptions |= LICENSE_BINCLEANOPTIONBIT;
	}
	else
	{
		wData = LICENSE_BINCLEANOPTIONBIT ^ 0xffff;    // form complement
		g_OnBoardCalData.m_wBlenderOptions &= wData;
	}
	if((wOptions & LICENSEMB_BYPASSOPTIONBIT)!=0)
	{
		g_OnBoardCalData.m_wBlenderOptions |= LICENSE_BYPASSOPTIONBIT;
	}
	else
	{
		wData = LICENSE_BYPASSOPTIONBIT ^ 0xffff;    // form complement
		g_OnBoardCalData.m_wBlenderOptions &= wData;
	}
	if((wOptions & LICENSEMB_OFFLINEHIGHLOWOPTIONBIT)!=0)
	{
		g_OnBoardCalData.m_wBlenderOptions |= LICENSE_OFFLINEHIGHLOWOPTIONBIT;
	}
	else
	{
		wData = LICENSE_OFFLINEHIGHLOWOPTIONBIT ^ 0xffff;    // form complement
		g_OnBoardCalData.m_wBlenderOptions &= wData;
	}
	if((wOptions & LICENSEMB_OFFLINEBLENDTOXXKGSBIT)!=0)
	{
		g_OnBoardCalData.m_wBlenderOptions |= LICENSE_OFFLINEBLENDTOXXKGSBIT;
	}
	else
	{
		wData = LICENSE_OFFLINEBLENDTOXXKGSBIT ^ 0xffff;    // form complement
		g_OnBoardCalData.m_wBlenderOptions &= wData;
	}
	if((wOptions & LICENSEMB_DO_BATCH_WEIGHT_CHECK)!=0)
	{
		g_OnBoardCalData.m_wBlenderOptions |= LICENSE_DO_BATCH_WEIGHT_CHECK;
	}
	else
	{
		wData = LICENSE_DO_BATCH_WEIGHT_CHECK ^ 0xffff;    // form complement
		g_OnBoardCalData.m_wBlenderOptions &= wData;
	}
    g_bSaveAllCalibrationToOnBoardEEprom = TRUE;        // save calibration data
}


//////////////////////////////////////////////////////
// MapMBLicensingOptionsNo2
//
//
//////////////////////////////////////////////////////
void MapMBLicensingOptionsNo2( WORD w2Options )
{
	WORD wData,wmsData,wlsData;

	wData = w2Options;
	wlsData = wData & 0xFF;
	wmsData = wData >> 8;

	// ms is the blender type
	// ls is the blender run period.
	if((wlsData != 0)&&(wlsData <= 10))
	{
		LicenseFunction(LICENSE_BLENDER_PERMANENT_RUN_ON + wlsData-1);
	}
	if((wmsData != 0)&&(wmsData <= 10))
	{
		LicenseFunction(LICENSE_SET_BLENDER_MODEL_OPTIMIX50 + wmsData-1);
	}
	g_bSaveAllCalibrationToOnBoardEEprom = TRUE;        // save calibration data
}

/*

CLEARLICENSEALARM:
        LDD     MBSYSALRM             ; SET BIT POS
        ANDD    #LICENSEALARMBITPOS
        BEQ     XITLCC                ; EXIT IF ALREADY SET.
        LDD     #LICENSEALARMBITPOS
        COMD
        ANDD    MBSYSALRM             ; CLEAR BIT POS
        STD     MBSYSALRM
        LDD     #$0100                ; LOAD LICENSE ALARM
        JSR     ADDALARMTOLIST
        LDAA    #1
        STAA   ALRMCFG                  ;SET LICENSE
XITLCC  RTS



BLENDERRUNOPTIONBIT EQU     %00000001
CLEANOPTIONBIT      EQU     %00000010
KGHROPTIONBIT       EQU     %00000100
GPMOPTIONBIT        EQU     %00010000
LLSOPTIONBIT        EQU     %00100000
REPORTOPTIONBIT     EQU     %01000000
RESETOPTIONBIT      EQU     %10000000


KGHRCODE            EQU     $10      ; KG/H CODE.
GPMCODE             EQU     $20      ; GPM CODE.
CLEANCODE           EQU     $30      ; CLEAN CODE
REPORTCODE          EQU     $40      ; REPORTS.
LLSCODE             EQU     $50      ; LOW LEVEL SENSOR.
BLENDERRUNCODE      EQU     $60      ; BROWSER CODE.
RESETCODE           EQU     $70      ; RESET ALL OPTIONS.
ENABLEALLCODE       EQU     $80      ; ENABLE ALL OPTIONS.

LICENSEHOURS        EQU     336

LICENSE2WEEKHRS     EQU     672
LICENSE1MONTHHRS    EQU     672
LICENSE2MONTHSHRS   EQU     1344
LICENSE3MONTHSHRS   EQU     2016
LICENSE6MONTHSHRS   EQU     4032


ID1HOURS            EQU     8760        ; 1 YEAR
ID2HOURS            EQU     4380        ; 6 MONTHS
ID3HOURS            EQU     2190        ; 3 MONTHS
ID4HOURS            EQU     730         ; 1 MONTH
ID5HOURS            EQU     168         ; 1 WEEK
ID6HOURS            EQU     24          ; 1 DAY
ID7HOURS            EQU     10          ;
ID8HOURS            EQU     5          ;
ID9HOURS            EQU     2          ;

POWERUPSTATUSID     EQU     $AA55       ;INDICATES THAT BLENDER HAS RUN BEFORE.
TESTCODEID          EQU     $9876       ; INDICATES BLENDER IN TEST MODE

MASTERMODULECODE1   EQU     $1234
MASTERMODULECODE2   EQU     $5678
WARNINGHRS          EQU     504                ;


;
BANK1PROGS:        SECTION


SETCLROPTIONBIT:
        TBA
        ORAB    OPTIONCODE              ; SET BIT
        STAB    OPTIONCODE              ; STORE
        TAB
        COMB
        ANDB    OPTIONCODE+1            ; CLEAR TEMPORARY BIT.
        STAB    OPTIONCODE+1            ; STORE
        RTS

SETTEMPBIT:
;       ACCB HOLDS THE OPTION
;
        ORAB    OPTIONCODE+1            ; SET BIT
        STAB    OPTIONCODE+1            ; STORE
STRHRS  STE     0,X                     ; RESTORE
        LDAA    #$AA
        STAA    SAVECAL2DATA1F          ; STORE DATA TO CALIBRATION DATA
        RTS

;       KG/H MODE

OPTIONENTRY:
        JSR     ENTERENCRYPTIONDATA
        JMP     OPTIONSPAGES



ENTERENCRYPTIONDATA:
        JSR     DECRYPTCODE             ; DECRYPT DATA.
        CPE     SOFTWAREID              ; COMPARE TO SOFTWARE ID.
        LBNE    XITOP                  ; SERIAL NO CORRECT.
        STD     TEMPCODE
        JSR     RESETALLOPTIONS
        JSR     COPYSOFTWAREID
;   CODE RETURNED IS XXY WHERE XX ARE THE OPTIONS AND Y INDICATES THE TIME PERIOD.

        LDD     TEMPCODE
        LSRD                            ; SHIFT DATA TO THE RIGHT
        LSRD                            ;
        LSRD                            ;
        LSRD                            ;
        TBA                             ; TAKE LSB
        LSRA
        PSHA                            ; SAVE A
        BCC     NOTOP1                  ; NOT 1ST OPTION
        LDE     TEMPCODE
        ANDE    #$000F
        BNE     SETTO                   ; SET TEMP OPTION
        LDAB    #BLENDERRUNOPTIONBIT       ;
        JSR     SETCLROPTIONBIT         ; SET PERMANENT, CLEAR TEMP.
        JSR     CLEARLICENSEALARM
        BRA     NOTOP1                   ; CHECK OTHER OPTIONS


SETTO:
        LDAB    #BLENDERRUNOPTIONBIT       ;
        LDX     #BLENDERRUNHRCTR           ; HOUR COUNTER
        LDAA    TEMPCODE+1
        ANDA    #$0F                       ; ISOLATE THE TEMP OPTION
        JSR     SETTEMPOPTION
        JSR     CLEARLICENSEALARM

NOTOP1  PULA                               ; RECOVER  SHIFTED DATA
        LSRA
        PSHA
        BCC     NOTOP2                     ; OPTION NOT SET
        LDAB    #CLEANOPTIONBIT
        JSR     SETCLROPTIONBIT         ; SET PERMANENT, CLEAR TEMP.
NOTOP2  PULA                            ;
        LSRA
        PSHA
        BCC     NOTOP3                  ;
        LDAB    #KGHROPTIONBIT          ;
        JSR     SETCLROPTIONBIT         ; SET PERMANENT, CLEAR TEMP.
NOTOP3  PULA                            ;
        LSRA
        LSRA                            ; SKIP THIS BIT.
        PSHA
        BCC     NOTOP4                  ;
        LDAB    #GPMOPTIONBIT           ;
        JSR     SETCLROPTIONBIT         ; SET PERMANENT, CLEAR TEMP.
NOTOP4  PULA                            ;
        LSRA
        PSHA
        BCC     NOTOP5                  ;
        LDAB    #LLSOPTIONBIT           ;
        JSR     SETCLROPTIONBIT         ; SET PERMANENT, CLEAR TEMP.
NOTOP5  PULA                            ;
        LSRA
        PSHA
        BCC     NOTOP6                  ;
        LDAB    #REPORTOPTIONBIT        ;
        JSR     SETCLROPTIONBIT         ; SET PERMANENT, CLEAR TEMP.
NOTOP6  PULA                            ;
        LSRA
        PSHA
        BCC     NOTOP7                  ;
        JSR     RESETALLOPTIONS
NOTOP7:
        PULA                            ; RECOVER STACK
XITOP   RTS


; ENABLE ALL OPTIONS.
;
ENABLEALLOPTIONS:
        LDAA    #KGHROPTIONBIT          ; KG/HR.
        ORAA    #GPMOPTIONBIT           ; G/M
        ORAA    #CLEANOPTIONBIT            ; MODBUS
        ORAA    #REPORTOPTIONBIT           ; REPORT
        ORAA    #LLSOPTIONBIT            ; LLS
        ORAA    #BLENDERRUNOPTIONBIT            ; WEB BROWSER
        STAA    OPTIONCODE               ; STORE VALUE.
        LDAA    #$AA
        STAA    SAVECAL2DATA1F           ; INITIATE CALIBRATION DATA
        RTS




;       RESET ALL OPTIONS.
RESETALLOPTIONS:
        CLRW    OPTIONCODE              ; RESET ALL OPTIONS.
        CLRW    KGHHRCTR                ; HOUR COUNTER = 0
        CLRW    GPMHRCTR                ;
        CLRW    MBHRCTR                 ;
        CLRW    REPHRCTR                ;
        CLRW    LLSHRCTR                ;
        CLRW    BLENDERRUNHRCTR                ;
        LDAA    #$AA
        STAA    SAVECAL2DATA1F           ; INITIATE CALIBRATION DATA
        RTS




; TAKE ENCRYPTED CODE AND RETURN SERIAL NO IN E, FUNCTION CODE IN A.

DECRYPTCODE
        JSR     ALIGNDATA               ; MOVE DATA TO THE CORRECT POSITION.
        LDX     #NKEYBUF
        JSR     BCDHEX4X                ; RESULT IN B:E
        CLR     NKEYBUF+0
        STD     NKEYBUF+1
        STE     NKEYBUF+3
        JSR     GENENCRYPTIONKEY        ; GENERATE ENCRYPTION.
        STE     ENCCODE+3               ; STORE ENCRYPTED VALUE.
NOCHGE  LDAB    #4                      ; NO OF LOOPS FOR ENCRYPTION ALGORITHM.
ENCAGN  PSHB
        LDD     ENCCODE+3                ; READ ENCRYPTION DATA.
        EORD    NKEYBUF+3
        STD     NKEYBUF+3
        LSRW    ENCCODE+3
        LSRW    ENCCODE+3
        LSRW    ENCCODE+3
        LSRW    ENCCODE+3
        PULB                             ; COUNTER
        DECB
        BNE     ENCAGN                   ; DO ENCRYPTIION AGAIN.
        LDX     #NKEYBUF+2
        JSR     HEXBCD3X
        RTS



ALIGNDATA:
        LDAA    ISTRSIZ             ; READ STRING SIZE
        CMPA    #8
        BEQ     XITSH
        CMPA    #7
        BNE     NOSHIFT             ;
        LDAA    #1                      ; NO OF SHIFTS.
DOSHIFT LDX    #NKEYBUF
        LDAB   #5
        JSR    GPSFTR            ;~RESULT
XITSH   RTS

NOSHIFT LDAA    #2               ; NO OF SHIFTS.
        LDX    #NKEYBUF
        LDAB   #5
        JSR    GPSFTR            ;~RESULT
        RTS




; ROUTINE TO CHECK FOR TEMPORARY OPTION.
;

CHECKFORTEMPOPTION:
; KG/H CONTROL.
        LDAA    OPTIONCODE              ; CHECK OPTION.
        BITA    #KGHROPTIONBIT          ; KG/H OPTION.
        BNE     NOTKGO                  ; NO.
        LDAA    #KGHROPTIONBIT          ;
        LDX     #KGHHRCTR               ; HR COUNTER.
        JSR     CHECKHRCTR              ;
NOTKGO  LDAA    OPTIONCODE              ; CHECK OPTION.
        BITA    #GPMOPTIONBIT           ; OPTION.
        BNE     NOTGMO                   ; NO.
        LDAA    #GPMOPTIONBIT          ;
        LDX     #GPMHRCTR               ; HR COUNTER.
        JSR     CHECKHRCTR              ;
NOTGMO  LDAA    OPTIONCODE              ; CHECK OPTION.
        BITA    #CLEANOPTIONBIT        ; OPTION.
        BNE     NOTMBO                  ; NO.
        LDAA    #CLEANOPTIONBIT        ;
        LDX     #MBHRCTR                ; HR COUNTER.
        JSR     CHECKHRCTR              ;
NOTMBO  LDAA    OPTIONCODE              ; CHECK OPTION.
        BITA    #REPORTOPTIONBIT        ; OPTION ?.
        BNE     NOTREPO                 ; NO.
        LDAA    #REPORTOPTIONBIT          ;
        LDX     #REPHRCTR               ; HR COUNTER.
        JSR     CHECKHRCTR              ;
NOTREPO LDAA    OPTIONCODE              ; CHECK OPTION.
        BITA    #LLSOPTIONBIT           ; OPTION ?.
        BNE     NOTLLSO                 ; NO.
        LDAA    #LLSOPTIONBIT           ;
        LDX     #LLSHRCTR               ; HR COUNTER.
        JSR     CHECKHRCTR              ;
NOTLLSO LDAA    OPTIONCODE              ; CHECK OPTION.
        BITA    #BLENDERRUNOPTIONBIT           ; OPTION ?.
        BNE     NOTWEBO                 ; NO.
        LDAA    #BLENDERRUNOPTIONBIT           ;
        LDX     #BLENDERRUNHRCTR               ; HR COUNTER.
        JSR     CHECKHRCTR              ;
NOTWEBO:
CHKAXIT RTS


; COUNTER = EXIT.

CHECKHRCTR:
        LDE     OPTIONCODE              ; READ OPTION CODE
        TAB
        ANDA    OPTIONCODE+1            ; CHECK FOR OPTION CODE.
        BEQ     XITHRC                  ; EXIT IF NO TEMP OPTION
        TSTW    0,X
        BEQ     DELCOM                  ; DELETE OPTION
        DECW    0,X                     ; DECREMENT HOUR COUNTER
        LDD     0,X                     ; READ HOURLY COUNTER
        ANDD    #0001                   ;
        BNE     NOEESV                  ; ONLY SAVE EEPROM EVERY N HOURS.
        LDAA    #$AA
        STAA    SAVECAL2DATA1F           ; INITIATE CALIBRATION DATA
NOEESV  TSTW    0,X                     ;
        BNE     XITHRC                  ; EXIT HOUR CHECK.
        LDAA    #1
        STAA    SHOWEXPIRYPAGE          ; SHOW THE EXPIRY PAGE
DELCOM  TBA                             ; RECOVER CODE.
        COMA
        ANDA    OPTIONCODE+1            ; DELETE OPTION.
        STAA    OPTIONCODE+1
        CPE     OPTIONCODE              ; HAS THE OPTION CODE CHANGED ?
        BEQ     NOCHOC                  ; NO CHANGE IN OPTION CODE.
        LDAA    #$AA
        STAA    SAVECAL2DATA1F           ; INITIATE CALIBRATION DATA
NOCHOC  LDAA    PGMNUM                  ; CHECK PAGE NO.
        CMPA    #OPTIONSPAGEID          ; ON OPTIONS PAGE ?
        BNE     XITHRC
        JSR     OPTIONSPAGES            ; UPDATE OPTIONS PAGE.
XITHRC  RTS                             ; EXIT.




;  GENERATION OF ENCRYPTION KEY.
;
GENENCRYPTIONKEY:
        LDD     SOFTWAREID              ; READ SERIAL NO.
        JSR     BCDHEX2                 ;
        TDE
        EORE    #$FFFF                  ; INITIAL EXOR.
        ANDP    #$FEFF                    ;~ENSURE CARRY CLEARED
        LDAB    #8                      ; NO OF PASSES.
REPENC  RORE                            ; ROTATE TO RIGHT.
        BCC     NOEXOR                  ; NO EXOR.
        EORE    #$A001                  ; EXCLUSIVE OR.
NOEXOR  DECB                            ; DECREMENT COUNTER.
        BNE     REPENC                  ; REPEAT ENCRYPTION.
        COME                            ; INVERT CODE
        RTS


;  COPY SOFTWARE ID FROM REAL TIME CLOCK.

COPYSOFTWAREID:
        PSHM    D
        LDAB   #HARDWAREBANK                   ; HARDWARE BANK
        TBZK                                   ; STORE BANK NO
        LDZ     #TCNT                    ; READ COUNTER VALUE.
        LDD     0,Z
        ANDD    #$1FFF
        JSR     HEXBCD2
        STE     SOFTWAREID
        LDAA     #$AA
        STAA     SAVECAL2DATA1F           ; INITIATE CALIBRATION DATA SAVE
        PULM    D
        RTS

DECODECODE:
        LDAA    TEMPCODE                ; READ CODE
        ANDA    #$0F                    ; ISOLATE LSB
        CMPA    #0
        BNE     NOT0
        LDAA    #KGHROPTIONBIT          ;
        JSR     SETCLROPTIONBIT         ; SET PERMANENT, CLEAR TEMP.
        LBRA     XITOP                   ; EXIT.

SETTEMPOPTION:
NOT0    CMPA    #1
        BNE     NOT1
        LDE     #ID1HOURS
        JSR     SETTEMPBIT
        LBRA     XITOP                   ;
NOT1    CMPA    #2
        BNE     NOT2
        LDE     #ID2HOURS
        JSR     SETTEMPBIT
        LBRA     XITOP                   ;
NOT2    CMPA    #3
        BNE     NOT3
        LDE     #ID3HOURS
        JSR     SETTEMPBIT
        LBRA     XITOP                   ;
NOT3    CMPA    #4
        BNE     NOT4
        LDE     #ID4HOURS
        JSR     SETTEMPBIT
        LBRA     XITOP                   ;
NOT4    CMPA    #5
        BNE     NOT5
        LDE     #ID5HOURS
        JSR     SETTEMPBIT
        LBRA     XITOP                   ;
NOT5    CMPA    #6
        BNE     NOT6
        LDE     #ID6HOURS
        JSR     SETTEMPBIT
        LBRA     XITOP                   ;
NOT6    CMPA    #7
        BNE     NOT7
        LDE     #ID7HOURS
        JSR     SETTEMPBIT
        LBRA     XITOP                   ;
NOT7    CMPA    #8
        BNE     NOT8
        LDE     #ID8HOURS
        JSR     SETTEMPBIT
        LBRA     XITOP                   ;
NOT8    CMPA    #9
        BNE     NOT9
        LDE     #ID9HOURS
        JSR     SETTEMPBIT
        LBRA     XITOP                   ;
NOT9    RTS

CUTOFFHWSERIALNO    EQU     $0350

CHECKPOWERUPSTATUS:
       JSR      CHECKFORMASTERCODE
       TST      ISMASTERMODULE          ; MASTER MODULE
       BEQ      NOMMM                   ; NO
       LDD      SBBSERNO+1
       STD      SOFTWAREID              ; SOFTWARE SET TO HARDWARE SERIAL NO
       CLRW     OPTIONCODE
       CLRW     TESTCODE                ; RESET THE TEST CODE.
       LDAA     #$AA
       STAA     SAVECAL2DATA1F           ; INITIATE CALIBRATION DATA SAVE
       BRA      XITPWC

NOMMM  LDD      TESTCODE                ; READ TEST CODE
       CPD      #TESTCODEID             ; IS THIS THE TEST CODE.
       BNE      CHKPUT                  ; CHECK POWER UP CODE.
       LDD      #$FE00
       STD      OPTIONCODE              ; ENABLE ALL OPTIONS WITH TEMPORARY RUN LICENSE
       LDAB    #BLENDERRUNOPTIONBIT     ;
       LDX     #BLENDERRUNHRCTR         ; SET TEMPORARY RUN OPTION TO 3 MONTHS.
       LDAA    #3
       JSR     SETTEMPOPTION
       LDAA     #$AA
       STAA     SAVECAL2DATA1F           ; INITIATE CALIBRATION DATA SAVE
       BRA      XITPWC

CHKPUT LDD      POWERUPSTATUS           ; CHECK POWER UP STATUS
       CPD      #POWERUPSTATUSID        ; HAS THIS SOFTWARE RUN BEFORE.
       BEQ      XITPWC                  ; YES IT HAS RUN BEFORE.
       LDD      #POWERUPSTATUSID
       STD      POWERUPSTATUS
       LDAA     #$AA
       STAA     SAVECAL2DATA1F           ; INITIATE CALIBRATION DATA SAVE

;      CHECK FOR HARDWARE SERIAL NO

;       LDD      SBBSERNO+1
;       CPD      #CUTOFFHWSERIALNO
;       BHI      XITPWC                  ; ALL HIGHER MACHINES
;       LDD      #$FFFF
;       STD      OPTIONCODE
;       LDAA     #$AA
;       STAA     SAVECAL2DATA1F           ; INITIATE CALIBRATION DATA SAVE
XITPWC  RTS


CHECKFORMASTERCODE:
       LDD      MASTERMODULE
       CPD      #MASTERMODULECODE1
       BNE      NOMMC                   ; NOT SPECIAL CODE.
       LDD      MASTERMODULE+2
       CPD      #MASTERMODULECODE2
       BNE      NOMMC                   ; NOT SPECIAL CODE.
       LDAA     #1
       STAA     ISMASTERMODULE
       RTS
NOMMC  CLR      ISMASTERMODULE
       RTS


DECIDEONEXPIRYPAGE:
        LDAA     OPTIONCODE
        ANDA    #BLENDERRUNOPTIONBIT    ; SHOULD THE BLENDER BE STOPPED?
        BNE     NOTEXP                  ;
        LDAA     OPTIONCODE+1
        ANDA    #BLENDERRUNOPTIONBIT    ; SHOULD THE BLENDER BE STOPPED?
        BNE     NOTEXP                  ;
        JSR     SETLICENSEALARM         ; SET LICENSE ALARM
        LDAA     CALFLG
        CMPA     #$AA
        BEQ     XITEXP                  ; EXIT IF CALIBRATION PAGE
        JMP     EXPIREDPAGE
NOTEXP:
        LDD      BLENDERRUNHRCTR
        BEQ     XITEXP                  ; NO UPDATE IF ZERO
        CPD      #WARNINGHRS            ; AT WARNING HOURS.
        BHI     NOEXPRY                 ; NO EXPIRY PAGE DISPLAY.
        JSR     SETLICENSEALARM         ; SET LICENSE ALARM
        LDAA     CALFLG
        CMPA     #$AA
        BEQ     XITEXP                  ; EXIT IF CALIBRATION PAGE
        JMP     WILLEXPIREPAGE          ; EXPIRY TIME
NOEXPRY JSR     CLEARLICENSEALARM
XITEXP  RTS


  from alarm12.asm

CLEARLICENSEALARM:
        LDD     MBSYSALRM             ; SET BIT POS
        ANDD    #LICENSEALARMBITPOS
        BEQ     XITLCC                ; EXIT IF ALREADY SET.
        LDD     #LICENSEALARMBITPOS
        COMD
        ANDD    MBSYSALRM             ; CLEAR BIT POS
        STD     MBSYSALRM
        LDD     #$0100                ; LOAD LICENSE ALARM
        JSR     ADDALARMTOLIST
        LDAA    #1
        STAA   ALRMCFG                  ;SET LICENSE
XITLCC  RTS

*/
