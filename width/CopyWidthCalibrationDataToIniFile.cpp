/*-------------------------------------------------------------------
 File: CopyCalibrationDataToIniFile.cpp
 Description: SD card configuration functions.

// P.Smith                          4/5/10
// width control config file
//
// M.McKiernan
// Added all width parameters.
//
//sprintf(cval, "%d  %s", g_stWidthCalibration.m_wUltrasonicAveraging, cTempText);
//sprintf(cval, "%d  %s", g_stWidthCalibration.m_wWidthOverrun, cTempText);
//sprintf(cval, "%d  %s", g_stWidthCalibration.m_wWidthOverrunDeadband, cTempText);
//sprintf(cval, "%d  %s", g_stWidthCalibration.m_bStandardiseOnAuto, cTempText);
//sprintf(cval, "%d  %s", g_stWidthCalibration.m_bStandardiseOnTheHour, cTempText);
//sprintf(cval, "%d  %s", g_stWidthCalibration.m_bAlarmInManual, cTempText);
//sprintf(cval, "%d  %s", g_stWidthCalibration.m_bFastCorrectionEnabled, cTempText);
//sprintf(cval, "%5.2f %s", g_stWidthCalibration.m_fLength_AB,cTempText);
//sprintf(cval, "%5.2f %s", g_stWidthCalibration.m_fLength_BC,cTempText);
//sprintf(cval, "%5.2f %s", g_stWidthCalibration.m_fLength_AC,cTempText);
//sprintf(cval, "%5.1 f%s", g_stWidthCalibration.m_fSensorAngleA,cTempText);
//sprintf(cval, "%5.1f %s", g_stWidthCalibration.m_fSensorAngleB,cTempText);
//sprintf(cval, "%5.1f %s", g_stWidthCalibration.m_fSensorAngleC, cTempText);
//sprintf(cval, "%5.1f %s", g_stWidthCalibration.m_fM2WidthOffset, cTempText);
//sprintf(cval, "%d %s", g_stWidthCalibration.m_nM2WidthOffsetSign, cTempText);

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
#include "WidthCalibrationDefinitions.h"
#include "WidthDef.h"
#include "CopyWidthCalibrationDataToIniFile.h"


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

extern char      *striLayflat;
extern char      *striUltrasonic;
extern char      *striUltrasonicAndLayflat;

extern char      *striStandardScanner;
extern char      *striSmallLineScanner;

extern char      *striBlownFilmLine;
extern char      *striIBCLine;
extern char      *striCryovacLine;

extern char      *striWidthModeNotKnown;

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

extern WidthCalDataStruct	g_stWidthCalibration;


void DeleteWidthDefaultFile( void )
{
    g_bSPIBusBusy = TRUE;       // Prevent the SPI use for other uses.
    InitialiseSDCard();         // note this includes SPI_Select_SD.
    f_delete("wdefault.ini");
    f_delvolume(MMC_DRV_NUM);
    ReInitialisePinsUsedBySDCard();
    g_bSPIBusBusy = FALSE;       // free up the SPI for other uses.
}


void CreateWidthDefaultIniFile (void )

{
	char fBuffer[50];

    g_bWriteToIniFileAgain = FALSE;
	F_FILE * fp;

    sprintf( fBuffer,"[%s]\n\n", striWidthConfig);



    fp = f_open("wdefault.ini", "w" );
//    SetupMttty();
//    iprintf("%s",fBuffer);

    if(fp)
    {
    f_write(fBuffer,strlen(fBuffer),1,fp);
    g_bWriteToIniFileAgain = TRUE;
    }
    f_close(fp);

}



/* WriteBatchIniFile() writes all calibration data to a file.
Output file current used = CalibOut.ini.
The ini_name in the function parameters is the template it uses for output.
The function currently has test data used for test purposes - commented out.    */
int WriteWidthIniFile(char * ini_name)
{
    char    *cTempText;
	dictionary	*	ini ;
	char	cval[90] ;
	char	cBuffer[80];

	/* Some temporary variables to hold query results */

    g_bSPIBusBusy = TRUE;       // Prevent the SPI use for other uses.

	 InitialiseSDCard();         // note this includes SPI_Select_SD.
     ReInitialiseGPIOPinsUsedBySDCard();


	ini = iniparser_load(ini_name);
	if (ini==NULL)
    {
		CreateWidthDefaultIniFile();
      f_delvolume(MMC_DRV_NUM);
      ReInitialisePinsUsedBySDCard();
      g_bSPIBusBusy = FALSE;       // free up the SPI for other uses.

		return -1 ;
	}


// width mode
    sprintf( cBuffer,"%s:", striWidthConfig);
    strcat( cBuffer, striWidthCalibationMode );
    cTempText = " ";
    cTempText = GetWidthModeText(g_stWidthCalibration.m_wWidthMode);
    sprintf(cval, "%d  #%s", g_stWidthCalibration.m_wWidthMode,cTempText);
    dictionary_set(ini, cBuffer, cval);

 // type standard small line
    sprintf( cBuffer,"%s:", striWidthConfig);
    strcat( cBuffer, striWidthCalibationType );
    cTempText = GetWidthTypeText(g_stWidthCalibration.m_wWidthType);
    sprintf(cval, "%d  #%s", g_stWidthCalibration.m_wWidthType,cTempText);
    dictionary_set(ini, cBuffer, cval);

// line type
    sprintf( cBuffer,"%s:", striWidthConfig);
     strcat( cBuffer, striWidthCalibationLineType );
     cTempText = " ";
     cTempText = GetWidthLineText(g_stWidthCalibration.m_wLineType);
     sprintf(cval, "%d  #%s", g_stWidthCalibration.m_wLineType,cTempText);
     dictionary_set(ini, cBuffer, cval);

// alarm limit
     sprintf( cBuffer,"%s:", striWidthConfig);
     strcat( cBuffer, striWidthCalibationAlarmLimit );
     cTempText = " ";
     sprintf(cval, "%d  %s", g_stWidthCalibration.m_wAlarmLimit,cTempText);
     dictionary_set(ini, cBuffer, cval);

// process delay
     sprintf( cBuffer,"%s:", striWidthConfig);
     strcat( cBuffer, striWidthCalibationProcessDelay );
     cTempText = " ";
     sprintf(cval, "%d  %s", g_stWidthCalibration.m_wProcessDelayInSeconds,cTempText);
     dictionary_set(ini, cBuffer, cval);

// integration time
     sprintf( cBuffer,"%s:", striWidthConfig);
     strcat( cBuffer, striWidthCalibationIntegrationTime );
     cTempText = " ";
     sprintf(cval, "%d  %s", g_stWidthCalibration.m_wWidthIntegrationTimeinSeconds,cTempText);
     dictionary_set(ini, cBuffer, cval);

 // deadband
     sprintf( cBuffer,"%s:", striWidthConfig);
     strcat( cBuffer, striWidthCalibationDeadband );
     cTempText = " ";
     sprintf(cval, "%d  %s", g_stWidthCalibration.m_wWidthDeadband,cTempText);
     dictionary_set(ini, cBuffer, cval);

    //increase control gain
    sprintf( cBuffer,"%s:", striWidthConfig);
    strcat( cBuffer, striWidthCalibationNormalIncreaseControlGain );
    cTempText = " ";
    sprintf(cval, "%3.0f  %s", g_stWidthCalibration.m_fNormalIncreaseControlGainPercentage,cTempText);
    dictionary_set(ini, cBuffer, cval);

    //decrease control gain
    sprintf( cBuffer,"%s:", striWidthConfig);
    strcat( cBuffer, striWidthCalibationNormalDecreaseControlGain );
    cTempText = " ";
    sprintf(cval, "%3.0f  %s", g_stWidthCalibration.m_fNormalDecreaseControlGainPercentage,cTempText);
    dictionary_set(ini, cBuffer, cval);

    //fc control gain
    sprintf( cBuffer,"%s:", striWidthConfig);
    strcat( cBuffer, striWidthCalibationFCConrolGain );
    cTempText = " ";
    sprintf(cval, "%3.0f %s", g_stWidthCalibration.m_fFastCorrectedControlGainPercentage,cTempText);
    dictionary_set(ini, cBuffer, cval);



    //width averaging
    sprintf( cBuffer,"%s:", striWidthConfig);
    strcat( cBuffer, striWidthCalibationAveraging );
    cTempText = " ";
    sprintf(cval, "%d  %s", g_stWidthCalibration.m_wWidthAveragingInSeconds,cTempText);
    dictionary_set(ini, cBuffer, cval);

// copy ultrasonic stuff to file
// distance to centre

    sprintf( cBuffer,"%s:", striWidthConfig);
    strcat( cBuffer, striWidthUSDistanceToCentre1);
    cTempText = " ";
    sprintf(cval, "%3.1f%s", g_stWidthCalibration.m_fDistanceToCentre[0],cTempText);
    dictionary_set(ini, cBuffer, cval);

    sprintf( cBuffer,"%s:", striWidthConfig);
    strcat( cBuffer, striWidthUSDistanceToCentre2);
    cTempText = " ";
    sprintf(cval, "%3.1f%s", g_stWidthCalibration.m_fDistanceToCentre[1],cTempText);
    dictionary_set(ini, cBuffer, cval);

    sprintf( cBuffer,"%s:", striWidthConfig);
    strcat( cBuffer, striWidthUSDistanceToCentre3);
    cTempText = " ";
    sprintf(cval, "%3.1f%s", g_stWidthCalibration.m_fDistanceToCentre[2],cTempText);
    dictionary_set(ini, cBuffer, cval);

    sprintf( cBuffer,"%s:", striWidthConfig);
    strcat( cBuffer, striWidthUSCountsPerMM);
    cTempText = " ";
    sprintf(cval, "%3.2f%s", g_stWidthCalibration.m_fUltrasonicMMConversionFactor,cTempText);
    dictionary_set(ini, cBuffer, cval);

    sprintf( cBuffer,"%s:", striWidthConfig);
    strcat( cBuffer, striWidthRefDistanceCalibratedAt );
    cTempText = " ";
    sprintf(cval, "%d  %s", g_stWidthCalibration.m_wUltrasonicMMCalibratedAt,cTempText);
    dictionary_set(ini, cBuffer, cval);
//wfh
    sprintf( cBuffer,"%s:", striWidthConfig);
    strcat( cBuffer, striUltrasonicAveraging );
    cTempText = " ";
    sprintf(cval, "%d  %s", g_stWidthCalibration.m_wUltrasonicAveraging, cTempText);
    dictionary_set(ini, cBuffer, cval);

    sprintf( cBuffer,"%s:", striWidthConfig);
    strcat( cBuffer, striWidthOverrun );
    cTempText = " ";
    sprintf(cval, "%d  %s", g_stWidthCalibration.m_wWidthOverrun, cTempText);
    dictionary_set(ini, cBuffer, cval);

    sprintf( cBuffer,"%s:", striWidthConfig);
    strcat( cBuffer, striWidthOverrunDeadband );
    cTempText = " ";
    sprintf(cval, "%d  %s", g_stWidthCalibration.m_wWidthOverrunDeadband, cTempText);
    dictionary_set(ini, cBuffer, cval);

    sprintf( cBuffer,"%s:", striWidthConfig);
    strcat( cBuffer, striStandardiseOnAuto );
    cTempText = " ";
    sprintf(cval, "%d  %s", g_stWidthCalibration.m_bStandardiseOnAuto, cTempText);
    dictionary_set(ini, cBuffer, cval);

    sprintf( cBuffer,"%s:", striWidthConfig);
    strcat( cBuffer, striStandardiseOnHour );
    cTempText = " ";
    sprintf(cval, "%d  %s", g_stWidthCalibration.m_bStandardiseOnTheHour, cTempText);
    dictionary_set(ini, cBuffer, cval);

    sprintf( cBuffer,"%s:", striWidthConfig);
    strcat( cBuffer, striAlarmInManual );
    cTempText = " ";
    sprintf(cval, "%d  %s", g_stWidthCalibration.m_bAlarmInManual, cTempText);
    dictionary_set(ini, cBuffer, cval);

    sprintf( cBuffer,"%s:", striWidthConfig);
    strcat( cBuffer, striFastCorrectionEnabled );
    cTempText = " ";
    sprintf(cval, "%d  %s", g_stWidthCalibration.m_bFastCorrectionEnabled, cTempText);
    dictionary_set(ini, cBuffer, cval);

    //wfh  - new parameters added.
    sprintf( cBuffer,"%s:", striWidthConfig);
    strcat( cBuffer, striLengthAB);
    cTempText = " ";
    sprintf(cval, "%5.2f %s", g_stWidthCalibration.m_fLength_AB,cTempText);
    dictionary_set(ini, cBuffer, cval);

    sprintf( cBuffer,"%s:", striWidthConfig);
    strcat( cBuffer, striLengthBC);
    cTempText = " ";
    sprintf(cval, "%5.2f %s", g_stWidthCalibration.m_fLength_BC,cTempText);
    dictionary_set(ini, cBuffer, cval);

    sprintf( cBuffer,"%s:", striWidthConfig);
    strcat( cBuffer, striLengthAC);
    cTempText = " ";
    sprintf(cval, "%5.2f %s", g_stWidthCalibration.m_fLength_AC,cTempText);
    dictionary_set(ini, cBuffer, cval);

    sprintf( cBuffer,"%s:", striWidthConfig);
    strcat( cBuffer, striSensorAngleA);
    cTempText = " ";
    sprintf(cval, "%5.1 f%s", g_stWidthCalibration.m_fSensorAngleA,cTempText);
    dictionary_set(ini, cBuffer, cval);

    sprintf( cBuffer,"%s:", striWidthConfig);
    strcat( cBuffer, striSensorAngleB);
    cTempText = " ";
    sprintf(cval, "%5.1f %s", g_stWidthCalibration.m_fSensorAngleB,cTempText);
    dictionary_set(ini, cBuffer, cval);

    sprintf( cBuffer,"%s:", striWidthConfig);
    strcat( cBuffer, striSensorAngleC);
    cTempText = " ";
    sprintf(cval, "%5.1f %s", g_stWidthCalibration.m_fSensorAngleC, cTempText);
    dictionary_set(ini, cBuffer, cval);

    sprintf( cBuffer,"%s:", striWidthConfig);
    strcat( cBuffer, striM2WidthOffset);
    cTempText = " ";
    sprintf(cval, "%5.1f %s", g_stWidthCalibration.m_fM2WidthOffset, cTempText);
    dictionary_set(ini, cBuffer, cval);

    sprintf( cBuffer,"%s:", striWidthConfig);
    strcat( cBuffer, striM2WidthOffsetSign);
    cTempText = " ";
    sprintf(cval, "%d %s", g_stWidthCalibration.m_nM2WidthOffsetSign, cTempText);
    dictionary_set(ini, cBuffer, cval);

    //wfh end
    /*
    const	char	 *striLengthAB = 			"Length AB";
    const	char	 *striLengthBC = 			"Length BC";
    const	char	 *striLengthAC = 			"Length AC";
    const	char	 *striSensorAngleA =		"Sensor Angle A";
    const	char	 *striSensorAngleB =		"Sensor Angle B";
    const	char	 *striSensorAngleC =		"Sensor Angle C";
    const	char	 *striM2WidthOffset =		"M2 Width Offset";
    const	char	 *striM2WidthOffsetSign =	"M2 Width Offset Sign";

*/

    sprintf( cBuffer, "wconfig.txt");

    F_FILE * pFile = f_open(cBuffer, "w");
    if( pFile )
    {
      iniparser_dump_ini(ini, pFile );
      f_close( pFile );
    }

	iniparser_freedict(ini);

   f_delvolume(MMC_DRV_NUM);

    ReInitialisePinsUsedBySDCard();
    g_bSPIBusBusy = FALSE;       // free up the SPI for other uses.
//nbb--todo--width    g_bSDCardSaveConfigComplete = TRUE;
	return( 0 );
}


void WriteWidthCalibrationDataToSDCard (void )
{
    if ( SDCardWritable()&& (g_nDisableSDCardAccessCtr == 0) )
    {
        WriteWidthIniFile("wdefault.ini");
        if(g_bWriteToIniFileAgain)
        {
            g_bWriteToIniFileAgain = FALSE;
            WriteWidthIniFile("wdefault.ini");
      }
    }
}


/////////////////////////////////////////////////////
// GetWidthModeText()
// returns a pointer to text
//
//////////////////////////////////////////////////////

char* GetWidthModeText(unsigned int nMode)
{
    char *cTextString;

        switch( nMode )
        {
        case LAYFLAT_MODE:
            cTextString = striLayflat;
            break;
        case ULTRASONIC_MODE:
            cTextString = striUltrasonic;
            break;
        case ULTRASONIC_AND_LAYFLAT_MODE:
            cTextString = striUltrasonicAndLayflat;
            break;

        default:
            cTextString = striWidthModeNotKnown;
            break;

        }  // end of switch statement.
        return(cTextString);
}


/////////////////////////////////////////////////////
// GetWidthTypeText()
// returns a pointer to text
//
//////////////////////////////////////////////////////

char* GetWidthTypeText(unsigned int nMode)
{
    char *cTextString;

        switch( nMode )
        {
        case STANDARD_TYPE:
            cTextString = striStandardScanner;
            break;
        case SMALL_LINE_TYPE:
            cTextString = striSmallLineScanner;
            break;

        default:
            cTextString = striWidthModeNotKnown;
            break;

        }  // end of switch statement.
        return(cTextString);
}


/////////////////////////////////////////////////////
// GetWidthLineText()
// returns a pointer to text
//
//////////////////////////////////////////////////////

char* GetWidthLineText(unsigned int nMode)
{
    char *cTextString;

        switch( nMode )
        {
        case BLOWN_FILM_LINE_TYPE:
            cTextString = striBlownFilmLine;
            break;
        case IBC_LINE_TYPE:
            cTextString = striIBCLine;
            break;
        case CRYOVAC_LINE_TYPE:
            cTextString = striCryovacLine;
            break;

        default:
            cTextString = striWidthModeNotKnown;
            break;

        }  // end of switch statement.
        return(cTextString);
}

/*
 * //Blender Modes

#define	LAYFLAT_MODE						(0)
#define	ULTRASONIC_MODE						(1)
#define	ULTRASONIC_AND_LAYFLAT_MODE			(2)

// Width Control Type
#define	STANDARD_TYPE						(0)
#define	SMALL_LINE_TYPE						(1)

// line type
#define	BLOWN_FILM_LINE_TYPE				(0)
#define	IBC_LINE_TYPE					    (1)
#define	CRYOVAC_LINE_TYPE				    (2)

 *
 */
