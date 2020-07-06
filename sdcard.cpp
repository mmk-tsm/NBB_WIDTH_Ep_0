//////////////////////////////////////////////////////
// Filename = sdcard.cpp
//
// P.Smith                              19/06/06
// Added writing of data to sd card
//
// P.Smith                              23/01/07
// Correct writing to sd card, call SPI_Select_SD
//
// P.Smith                              5/2/07
// added todo
//
// P.Smith                              5/2/07
// removed set of g_bSPIAtDActive, remove printfs
//
// P.Smith                              11/4/07
// Moved InitMMC into this file.
// removed getchar if card not inserted.
//
// P.Smith                              11/5/07
// removed printfs, removed get_cd for now.
//
// P.Smith                              28/5/07
// remove check on the mixing motor shut off time to enable diagnostics.
// this will now be enabled by the diagnostics password.
// set flag to stop a/d spi from running when writing to the sd card.
//
// P.Smith                              21/6/07
// set g_bSPIBusBusy to stop spi access
// remove g_bStopSPIInterruptRunning
//
// P.Smith                              25/6/07
// remove sd card detection, no way to report the error at the moment
//
//
// P.Smith                              23/10/07
// WriteMultiBlendDiagnosticsToSDCard formed to write diagnostics to sd card for
// multiblend monitoring
//
// P.Smith                          30/1/08
// correct compiler warnings
//
// P.Smith                          4/6/08
// implement LoadConfigFromSDCard
//
// P.Smith                          5/6/08
// added LoadBackupConfigFromSDCard
// added CopyConfigFileToBackup
// add CopyConfigFileToBackup
//
// P.Smith                          27/6/08
// back up file name changed to cfbackup.bak
//
// P.Smith                          17/7/08
// check SDCardWritable when accessing sd card.
// check if present when loading config
// config name changed to config.txt
// copy in text file relating to machine name.
// call ReInitialisePinsUsedBySDCard at end of WriteToSdCardFile
//
// P.Smith                          21/7/08
// implement CopySDCardStatusToMB to copy sd card status to modbus
//
// P.Smith                          23/7/08
// remove g_CurrentTime extern
//
// P.Smith                          24/7/08
// remove g_CurrentTime extern
// in WriteToSdCardFile only allow write if g_nDisableSDCardAccessCtr is 0 && m_bLogToSDCard
// same for CopyConfigFileToBackup & WriteMultiBlendDiagnosticsToSDCard and CopyConfigFileToBackup
//
// P.Smith                          24/7/08
// Correct year access subtract century from it.
//
// P.Smith                          24/10/08
// if the sd card file does not open, then generate an sd card alarm.
// set bSdCardAlarmHasOccurred / bSdCardAlarmHasCleared and generate an alarm.
// ensure that it is not possible to write to the sd card while it is being
// written to.
// this is why the alarm is generated afterwards.
//
// P.Smith                          26/11/08
// added WriteToSdCardFileNoAppend
//
// P.Smith                          30/6/09
// call CopyConfigDataToMB instead of CopyCalibrationDataToMB
//
// P.Smith                          25/8/09
// added WriteDiagnosticCSVDataToSDCard
//
// P.Smith                          7/9/09
// a problem has been seen where the a/d read after an sd card write
// and reinitialisation of spi pins causes the status reg to go to
// FFFF
// g_nWaitForSPIBusToSettleCtr is set to 500 to allow the bus to settle.
//
// P.Smith                          10/9/09
// WriteDiagnosticDataToSDCard now checks for g_CalibrationData.m_bLogToSDCard
// also WriteDiagnosticCSVDataToSDCard
// remove check from WriteToSdCardFile
//
// P.Smith                          17/9/09
// change check for sd card log to check LOG_BATCH_LOG_TO_SD_CARD
// remove check from WriteMultiBlendDiagnosticsToSDCard
//
// P.Smith                          30/11/09
// call ReInitialiseGPIOPinsUsedBySDCard after initialisation of sd card.
//
// P.Smith                          3/12/09
// in WriteToSdCardFileNoAppend remove check for LOG_BATCH_LOG_TO_SD_CARD bit
// in g_CalibrationData.m_nLogToSDCard
//
// M.McKiernan                      1/12/09
// Removed call to #include <effs_fat/mmc_drv.h> - mmc_drv.h not present in eclipse version
// Removed definition for EffsErrorCode[][80] - this is now in Nburn file, FileSystemUtils.cpp
// Removed DisplayEffsErrorCode( int code ) - this is now in Nburn file, FileSystemUtils.cpp
// Added include for FileSystemUtils.h & cardtype.h
// P.Smith                          11/1/10
// copy mount fat and ch drive returned value so that it can be recorded. later
// modify the open command to try to open the file 10 times, if this does not succeed
// change the name of the file by one letter and try to open the file again.
// only at this point, declare an alarm if the file has not opened.
//
// P.Smith                          11/1/10
// added g_bFileCouldNotBeOpenedEvent set, remove telnet access
//
// P.Smith                          13/1/10
// make function WriteToSdCardFile return a BOOL to check if the write was okay
//
// P.Smith                          13/1/10
// in WriteDiagnosticDataToSDCard check sd card write,WriteToSdCardFile returns
// pass / fail, if it fails then increment an error counter, then it is attempted
// 10 times to write to the sd card.
//
// P.Smith                          8/2/10
// comment out code in WriteMultiBlendDiagnosticsToSDCard unused for now
#include "predef.h"
#include <stdio.h>
#include <ctype.h>
#include <startnet.h>
#include <autoupdate.h>
#include <mailto.h>
#include <taskmon.h>
#include "string.h"
#include "BatchCalibrationFunctions.h"
#include "Batvars.h"
#include "Batvars2.h"
#include "NBBGpio.h"
#include <effs_fat/fat.h>
#include <effs_fat/mmc_mcf.h>
#include <effs_fat/cfc_mcf.h>
#include "TimeDate.h"
#include <effs_fat/fat.h>
#include <effs_fat/mmc_mcf.h>
#include <effs_fat/cfc_mcf.h>
// eclipse #include <effs_fat/mmc_drv.h>
#include "sdcard.h"
#include "Setpointformat.h"
#include "Loadcaldatafromsdcard.h"
#include "MBProgs.h"
#include "iniparser.h"
#include "iniexample.h"
#include  "Alarms.h"
#include    "Hash.h"

//
#include "FileSystemUtils.h"
#include "CardType.h"

/*
char EffsErrorCode[][80] =
{
  "F_NO_ERROR",                // 0
  "F_ERR_INVALIDDRIVE",        // 1
  "F_ERR_NOTFORMATTED",        // 2
  "F_ERR_INVALIDDIR",          // 3
  "F_ERR_INVALIDNAME",         // 4
  "F_ERR_NOTFOUND",            // 5
  "F_ERR_DUPLICATED",          // 6
  "F_ERR_NOMOREENTRY",         // 7
  "F_ERR_NOTOPEN",             // 8
  "F_ERR_EOF",                 // 9
  "F_ERR_RESERVED",            // 10
  "F_ERR_NOTUSEABLE",          // 11
  "F_ERR_LOCKED",              // 12
  "F_ERR_ACCESSDENIED",        // 13
  "F_ERR_NOTEMPTY",            // 14
  "F_ERR_INITFUNC",            // 15
  "F_ERR_CARDREMOVED",         // 16
  "F_ERR_ONDRIVE",             // 17
  "F_ERR_INVALIDSECTOR",       // 18
  "F_ERR_READ",                // 19
  "F_ERR_WRITE",               // 20
  "F_ERR_INVALIDMEDIA",        // 21
  "F_ERR_BUSY",                  // 22
  "F_ERR_WRITEPROTECT",        // 23
  "F_ERR_INVFATTYPE",          // 24
  "F_ERR_MEDIATOOSMALL",       // 25
  "F_ERR_MEDIATOOLARGE",       // 26
  "F_ERR_NOTSUPPSECTORSIZE",   // 27
  "F_ERR_DELFUNC",             // 28
  "F_ERR_MOUNTED"              // 29
};
*/

extern  char        g_cPrinterBuffer[];
extern  CalDataStruct   g_CalibrationData;
extern  char   *RevisionNumber;
extern  char    *strNewLine;
extern  structSetpointData   g_CurrentRecipe;
extern  structSetpointData  g_CommsDownloadedRecipe[];
extern  BYTE DS2432Buffer[];



/////////////////////////////////////////////////////
// InitialiseSDCard()
//
// Innitialises SD card
//////////////////////////////////////////////////////
void InitialiseSDCard( void )
{
 unsigned int rv,rv1;
  g_bSPIEEActive = TRUE;
//       iprintf("\n setting up sd drive"); //nbb--testonly--
       SPI_Select_SD();
//       iprintf("About to mount FAT %d\r\n", MMC_DRV_NUM ); //nbb--testonly--

         rv=f_mountfat(MMC_DRV_NUM,mmc_initfunc,F_MMC_DRIVE0);
         g_nMountFatReturnedValue = rv;
//               iprintf("Reached L.34\r\n" ); //nbb--testonly--

         rv1 = f_chdrive(MMC_DRV_NUM);
//        if(fdTelnet>0)
//         iprintf("\n rv %d rv1 %d",rv,rv1);
         g_nChangeDriveReturnedValue = rv1;
//               iprintf("Reached L.38\r\n" ); //nbb--testonly--

               f_chdir("\\");

//               iprintf("Reached L.42\r\n" ); //nbb--testonly--

                 g_bSPIEEActive = FALSE;


}


/////////////////////////////////////////////////////
// WriteDiagnosticDataToSDCard()
//
// writes data to SD card
//////////////////////////////////////////////////////
void WriteDiagnosticDataToSDCard( void )
{
    char    cName[20];
    unsigned int i=0;
    BOOL    bOkay = FALSE;

    if((g_CalibrationData.m_nLogToSDCard & LOG_BATCH_LOG_TO_SD_CARD) != 0)
    {
        sprintf( cName, "lg%02d%02d%02d",g_CurrentTime[TIME_YEAR] - TIME_CENTURY,g_CurrentTime[TIME_MONTH],g_CurrentTime[TIME_DATE]);     // Counts per int. stage 1.
        strcat( cName, ".txt");
        bOkay = WriteToSdCardFile(cName,g_cPrinterBuffer);
        if(bOkay)
        {
            RemoveAlarmTable(SDCARDFAULTALARM,  0);      /* indicate alarm, Component no. is zero.  */
        }
        else // error handling, try 10 time to write to sd card
        {
            g_ProcessData.m_nSdCardErrorCtr++;
            GenerateHashForSDCardErrorCtr();
            StoreSDCardErrorDetailsForTextLog(cName);
            g_bSaveAllProcessDataToEEprom = TRUE;
            g_bAllowProcesDataSaveToEEPROM = TRUE;
            i = MAX_SDCARD_WRITE_ATTEMPTS;
            do
            {
                bOkay = WriteToSdCardFile(cName,g_cPrinterBuffer);
                OSTimeDly(1);      // 10ms delay
                i--;
            }
            while(!bOkay && (i >0));

            if(!bOkay) // change the name and try again.
            {
                sprintf( cName, "le%02d%02d%02d",g_CurrentTime[TIME_YEAR] - TIME_CENTURY,g_CurrentTime[TIME_MONTH],g_CurrentTime[TIME_DATE]);     // Counts per int. stage 1.
                strcat( cName, ".txt");
                i = MAX_SDCARD_WRITE_ATTEMPTS;
                do
                {
                    bOkay = WriteToSdCardFile(cName,g_cPrinterBuffer);
                    OSTimeDly(1);      // 10ms delay
                    i--;
                }
                while(!bOkay && (i >0));
                if(!bOkay)
                {
                    PutAlarmTable(SDCARDFAULTALARM,  0);      /* indicate alarm, Component no. is zero.  */
                }
                else
                {
                    RemoveAlarmTable(SDCARDFAULTALARM,  0);      /* indicate alarm, Component no. is zero.  */
                }
            }
            else
            {
                RemoveAlarmTable(SDCARDFAULTALARM,  0);      /* indicate alarm, Component no. is zero.  */
            }
        }
    }
}


/////////////////////////////////////////////////////
// StoreSDCardErrorDetailsForTextLog()
//
//////////////////////////////////////////////////////
void StoreSDCardErrorDetailsForTextLog( char *cFileName )
{
    unsigned int i;
    g_ProcessData.m_nMountFatReturnedValueOnSdError = g_nMountFatReturnedValue;
    g_ProcessData.m_nChangeDriveReturnedValueOnSdError = g_nChangeDriveReturnedValue;
    g_ProcessData.m_nFilePointerOnSdError = g_nChangeDriveReturnedValue;
    // take a note of the file name
    for(i = 0; i < SD_CARD_NAME_SIZE; i++ )
    {
        g_ProcessData.m_cFileNameOnSdCardError[i] = cFileName[i];
    }
}


/////////////////////////////////////////////////////
// WriteDiagnosticCSVDataToSDCard()
//
//////////////////////////////////////////////////////
void WriteDiagnosticCSVDataToSDCard( void )
{
    char    cName[20];
    unsigned int i=0;
    BOOL    bOkay = FALSE;

    if((g_CalibrationData.m_nLogToSDCard & LOG_BATCH_LOG_TO_SD_CARD) != 0)
    {
        sprintf( cName, "lg%02d%02d%02d",g_CurrentTime[TIME_YEAR] - TIME_CENTURY,g_CurrentTime[TIME_MONTH],g_CurrentTime[TIME_DATE]);     // Counts per int. stage 1.
        strcat( cName, ".csv");
        bOkay = WriteToSdCardFile(cName,g_cPrinterBuffer);
        if(bOkay)
        {
            RemoveAlarmTable(SDCARDFAULTALARM,  0);      /* indicate alarm, Component no. is zero.  */
        }
        else // error handling, try 10 time to write to sd card
        {
            g_ProcessData.m_nSdCardErrorCtr++;
            GenerateHashForSDCardErrorCtr();
            StoreSDCardErrorDetailsForTextLog(cName);
            g_bSaveAllProcessDataToEEprom = TRUE;
            g_bAllowProcesDataSaveToEEPROM = TRUE;
            i = MAX_SDCARD_WRITE_ATTEMPTS;
            do
            {
                bOkay = WriteToSdCardFile(cName,g_cPrinterBuffer);
                OSTimeDly(1);      // 10ms delay
                i--;
            }
            while(!bOkay && (i >0));
            if(!bOkay) // change the name and try again.
            {
                sprintf( cName, "le%02d%02d%02d",g_CurrentTime[TIME_YEAR] - TIME_CENTURY,g_CurrentTime[TIME_MONTH],g_CurrentTime[TIME_DATE]);     // Counts per int. stage 1.
                strcat( cName, ".csv");
                i = MAX_SDCARD_WRITE_ATTEMPTS;
                do
                {
                    bOkay = WriteToSdCardFile(cName,g_cPrinterBuffer);
                    OSTimeDly(1);      // 10ms delay
                    i--;
                }
                while(!bOkay && (i >0));
                if(!bOkay)
                {
                    PutAlarmTable(SDCARDFAULTALARM,  0);      /* indicate alarm, Component no. is zero.  */
                }
                else
                {
                    RemoveAlarmTable(SDCARDFAULTALARM,  0);      /* indicate alarm, Component no. is zero.  */
                }
            }
            else
            {
                RemoveAlarmTable(SDCARDFAULTALARM,  0);      /* indicate alarm, Component no. is zero.  */
            }
        }
    }
}

/////////////////////////////////////////////////////
// WriteToSdCardFile()
//
// writes data to SD card if inserted
//////////////////////////////////////////////////////
BOOL WriteToSdCardFile( char *cFileName, char *cString )
{
    int m,n;
    BOOL  bSDCardWriteOk = TRUE;
    if ( SDCardWritable()&& (g_nDisableSDCardAccessCtr == 0))
    {
        g_bSPIBusBusy = TRUE;

        InitialiseSDCard();
        F_FILE * fp;
        fp=f_open(cFileName,"a");
        if(fp)
        {
            n = f_write(cString,strlen(cString),1,fp);
            m = f_close(fp);
        }
        else
        {  // indicate that the file could not be opened.
            bSDCardWriteOk = FALSE;
        }

        f_delvolume(MMC_DRV_NUM);
        SetSPIEX1_Lo();   //  EX1 and EX2 must be both Hi for SD chip select.
        SetSPIEX2_Lo();   //  so this will de-activate SD chip selection.
        ReInitialisePinsUsedBySDCard();
        g_nWaitForSPIBusToSettleCtr = TIME_FOR_ATOD_TO_SETTLE_AFTER_SDWRITE;
        g_bSPIBusBusy = FALSE;

    }

    return(bSDCardWriteOk);
}


/////////////////////////////////////////////////////
// WriteToSdCardFileNoAppend()
//
// writes data to SD  with no append
//////////////////////////////////////////////////////
void WriteToSdCardFileNoAppend( char *cFileName, char *cString )
{
    int m,n;

    if ( SDCardWritable()&& (g_nDisableSDCardAccessCtr == 0))
    {
        g_bSPIBusBusy = TRUE;
        BOOL  bSdCardAlarmHasOccurred = FALSE;
        BOOL  bSdCardAlarmHasCleared = FALSE;

        InitialiseSDCard();
        ReInitialiseGPIOPinsUsedBySDCard();
        F_FILE * fp;
        fp=f_open(cFileName,"w");
        if(fp)
        {
            n = f_write(cString,strlen(cString),1,fp);
            bSdCardAlarmHasCleared = TRUE;
            m = f_close(fp);
        }
        else
        {
            bSdCardAlarmHasOccurred = TRUE;
        }

        f_delvolume(MMC_DRV_NUM);
        SetSPIEX1_Lo();   //  EX1 and EX2 must be both Hi for SD chip select.
        SetSPIEX2_Lo();   //  so this will de-activate SD chip selection.
        g_bSPIBusBusy = FALSE;
        ReInitialisePinsUsedBySDCard();
        if(bSdCardAlarmHasOccurred)
        {
            bSdCardAlarmHasOccurred = FALSE;
            PutAlarmTable(SDCARDFAULTALARM,  0);      /* indicate alarm, Component no. is zero.  */
        }
        if(bSdCardAlarmHasCleared)
        {
            bSdCardAlarmHasCleared = FALSE;
            RemoveAlarmTable(SDCARDFAULTALARM,  0);      /* indicate alarm, Component no. is zero.  */
        }

    }
//    if( fdTelnet > 0)
//    {
//      iprintf("data written is %d string length is %d",n,strlen(cString));  //nbb--testonly--
//   }

}



/*-------------------------------------------------------------------
  InitMMC() - Initialization function for setting up EFFS on
  MultiMedia/Secure Digital cards.
 -------------------------------------------------------------------*/
void InitMMC()
{
 //tsm
                //beg SDTEST
                  SPIEX1_Hi();   //  EX1 and EX2 must be both Hi for SD chip select.
                  SPIEX2_Hi();   //

 //tsm end
   // Card detection check
/*
   if ( get_cd1() == 0 )  //
   {
      while ( get_cd() == 0 )
      {
//         iprintf( "No MMC/SD card detected. Insert a card and then press " );
//         iprintf( "<Enter>\r\n" );
         break;
      }
   }
*/
   /* The f_mountfat() function is called to mount a FAT drive. Although
      there are paramters in this function, they should not be modified.
      The function call should be used as it appears for a Compact Flash
      card. For reference, the parameters are:

      drive_num:   Set to MMC_DRV_NUM, the drive to be mounted
      p_init_func: Points to the initialization function for the drive.
                   For the Compact Flash drive, the function is located
                   in \nburn\<platform>\system\cfc_mem_nburn.c.
      p_user_info: Used to pass optional information. In this case, the
                   drive number.

      The return values are:
         F_NO_ERROR:  drive successfully mounted
         Any other value: Error occurred. See page 22 in the HCC-Embedded
         file system manual for the list of error codes.
   */

   int rv = f_mountfat( MMC_DRV_NUM, mmc_initfunc, F_MMC_DRIVE0 );
   if ( rv == F_NO_ERROR )
   {
//      iprintf( "FAT mount to SD/MMC successful\r\n" );
   }
   else
   {
//      iprintf( "FAT mount to SD/MMC failed: " );
      DisplayEffsErrorCode( rv );
   }

   /* Change to SD/MMC drive
      We need to call the change function to access the new drive. Note
      that ANY function other than the f_mountfat() is specific to a task.
      This means that if f_chdrive() is called in a different task to a
      different drive, it will not affect this task - this task will
      remain on the same drive.
   */
   rv = f_chdrive( MMC_DRV_NUM );
//   iprintf("\n rv is %d \n",rv);
   if ( rv == F_NO_ERROR )
   {
//      iprintf( "SD/MMC drive change successful\r\n" );
   }
   else
   {
//      iprintf( "SD/MMC drive change failed: " );
      DisplayEffsErrorCode( rv );
   }
}


/*-------------------------------------------------------------------
  DisplayEffsErrorCode()
 -------------------------------------------------------------------*/
/* eclipse
void DisplayEffsErrorCode( int code )
{
   if ( code <= MAX_EFFS_ERRORCODE )
   {
//      iprintf( "%s\r\n", EffsErrorCode[code] );
   }
   else
   {
//      iprintf("Unknown EFFS error code [%d]\r\n", code );
   }
}
*/

int get_cd1( void )
{
	BYTE cValue = sim.gpio.ppdsdr_datah;  	// d15-d8
	if(cValue & 0x80)	   				// check  m.s. bit
	  return (0);	   				// return 1 or 0
   else
      return (1);
}


void WriteMultiBlendDiagnosticsToSDCard( void )
{
/*    char    cBuffer[200];
    unsigned int i,j;
    F_FILE * fp;
    if ( SDCardWritable() && (g_nDisableSDCardAccessCtr == 0))
    {

    g_cMultiblendDiagnosticsBuffer[0] = ASCII_NULL;
    strcat(g_cMultiblendDiagnosticsBuffer,RevisionNumber);

    sprintf( cBuffer, " %02d-%02d-%04d  %02d:%02d:%02d ", g_CurrentTime[TIME_DATE], g_CurrentTime[TIME_MONTH], g_CurrentTime[TIME_YEAR], g_CurrentTime[TIME_HOUR], g_CurrentTime[TIME_MINUTE], g_CurrentTime[TIME_SECOND] );
    strcat( g_cMultiblendDiagnosticsBuffer, cBuffer );

    sprintf( cBuffer, "vacuuming to loader %d",g_nVacuumLoaderNo);
    strcat( g_cMultiblendDiagnosticsBuffer, cBuffer );
    strcat( g_cMultiblendDiagnosticsBuffer, strNewLine );

    sprintf( cBuffer, "Set Percentages");
    strcat( g_cMultiblendDiagnosticsBuffer, cBuffer );

    for( i = 0; i < g_CalibrationData.m_nComponents; i++ )
    {
        sprintf( cBuffer, " %d  %2.2f ",i+1,g_CurrentRecipe.m_fPercentage[i]);
        strcat( g_cMultiblendDiagnosticsBuffer, cBuffer );
    }

    strcat( g_cMultiblendDiagnosticsBuffer, cBuffer );
    strcat( g_cMultiblendDiagnosticsBuffer, strNewLine );

// now check if the right recipe is loaded.
//


    i = g_nVacuumLoaderNo-1;

    BOOL bIsTheSame = TRUE;
    for(j = 0; j < g_CalibrationData.m_nComponents; j++ )
    {
        if(g_CommsDownloadedRecipe[i].m_fPercentage[j] != g_CurrentRecipe.m_fPercentage[j])
        {
            bIsTheSame = FALSE;
        }
    }

    if(!bIsTheSame)
    {
        sprintf( cBuffer, "Error loaded the wrong material");
        strcat( g_cMultiblendDiagnosticsBuffer, cBuffer );
        strcat( g_cMultiblendDiagnosticsBuffer, strNewLine );

    }
    strcat( g_cMultiblendDiagnosticsBuffer, strNewLine );

    g_bSPIBusBusy = TRUE;
    InitialiseSDCard();
    ReInitialiseGPIOPinsUsedBySDCard();

    sprintf( cBuffer, "mb%d%d%d", g_CurrentTime[TIME_DATE],g_CurrentTime[TIME_MONTH],g_CurrentTime[TIME_YEAR] - TIME_CENTURY);     // Counts per int. stage 1.
    strcat( cBuffer, ".txt");


    fp=f_open(cBuffer,"a");
    if(fp)
    {
       f_write(g_cMultiblendDiagnosticsBuffer,strlen(g_cMultiblendDiagnosticsBuffer),1,fp);
       f_close(fp);

    }
    f_delvolume(MMC_DRV_NUM);
    SetSPIEX1_Lo();   //  EX1 and EX2 must be both Hi for SD chip select.
    SetSPIEX2_Lo();   //  so this will de-activate SD chip selection.
    ReInitialisePinsUsedBySDCard();
    g_bSPIBusBusy = FALSE;
    }
*/
}


// load config from sd card

void LoadConfigFromSDCard( void )
{

    int     status1;
    char    cBuffer[20];
    if ( SDCardPresent() )
    {
        sprintf( cBuffer, "config.txt");
        status1 =  parse_Batch_ini_file( cBuffer );
        CopyConfigDataToMB();
        if(status1 == 0)
        {
            g_bSDCardLoadConfigComplete = TRUE;
        }
    }
}

// load backup config from sd card

void LoadBackupConfigFromSDCard( void )
{
    if ( SDCardWritable() )
    {
        int     status1;
        char    cBuffer[20];
        sprintf( cBuffer, "config.bak");    // hardware serial no.
        status1 =  parse_Batch_ini_file( cBuffer );
        CopyConfigDataToMB();
        if(status1 == 0)
        {
            g_bSDCardLoadBackupConfigComplete = TRUE;
        }
    }
}

// copy config to backup

void CopyConfigFileToBackup( void )
{
    if ( SDCardWritable() && (g_nDisableSDCardAccessCtr == 0) )
    {
	     dictionary	*	ini ;
	/* Some temporary variables to hold query results */

        InitialisePinsUsedBySDCard();
        g_bSPIBusBusy = TRUE;       // Prevent the SPI use for other uses.

        InitialiseSDCard();         // note this includes SPI_Select_SD.
        ReInitialiseGPIOPinsUsedBySDCard();

        char    cBuffer[20];
        sprintf( cBuffer, "config.txt");    //

	    ini = iniparser_load(cBuffer);
	    if (ini==NULL)
        {
            g_bSPIBusBusy = FALSE;       // Prevent the SPI use for other uses.
            f_delvolume(MMC_DRV_NUM);
            ReInitialisePinsUsedBySDCard();
        }
        else
        {

            sprintf( cBuffer, "config.bak");    // hardware serial no.
            F_FILE * pFile = f_open(cBuffer, "w");
            if( pFile )
            {
                iniparser_dump_ini(ini, pFile );
                f_close( pFile );
                g_bSDCardCopyConfigToBackupComplete = TRUE;
            }

// generate config file relating to machine name

            sprintf( cBuffer, g_CalibrationData.m_cEmailBlenderName);    // hardware serial no.
            strcat(cBuffer,".ini");
            pFile = f_open(cBuffer, "w");
            if( pFile )
            {
                iniparser_dump_ini(ini, pFile );
                f_close( pFile );
                g_bSDCardCopyConfigToBackupComplete = TRUE;
            }


            iniparser_freedict(ini);
            g_bSPIBusBusy = FALSE;       // Prevent the SPI use for other uses.
        }
        f_delvolume(MMC_DRV_NUM);
        ReInitialisePinsUsedBySDCard();
    }
}

// SDCardWritable returns TRUE if SD Card present and not locked
// CD and WP bits both 0.
BOOL SDCardWritable( void )
{
 BYTE cValue = sim.gpio.ppdsdr_datah;   // d15-d8
 if( (cValue & 0xC0) == 0x00)        // check  m.s. 2 bits
   return (TRUE);        // return 1 or 0
   else
      return (FALSE);
}
// SDCardPresent returns TRUE if SD Card present
// CD = 0.
BOOL SDCardPresent( void )
{
 BYTE cValue = sim.gpio.ppdsdr_datah;   // d15-d8
 if( (cValue & 0x80) == 0x00)        // check  m.s. bit
   return (TRUE);        // return 1 or 0
   else
      return (FALSE);
}

//////////////////////////////////////////////////////
// CopySDCardStatusToMB
// copies sd card status to modbus
// returns one of the following
// not inserted, inserted and writable, inserted and locked
//
//////////////////////////////////////////////////////


void CopySDCardStatusToMB( void )
{
    unsigned char nStatus = sim.gpio.ppdsdr_datah;
    if((nStatus & SDCARD_INSERTED_BITPOS) == 0x00)  // sd card inserted ?
    {
        if((nStatus & SDCARD_LOCK_BITPOS) == 0x00)  // locked ?
        {
            g_arrnMBTable[SD_CARD_STATUS] = SD_CARD_INSERTED_AND_WRITABLE;
        }
        else
        {
            g_arrnMBTable[SD_CARD_STATUS] = SD_CARD_INSERTED_AND_LOCKED;
        }
    }
    else
    {
        g_arrnMBTable[SD_CARD_STATUS] = SD_CARD_NOT_INSERTED;
    }
}
/*

   int rc = f_findfirst( cBuffer, &find );
   if ( rc == 0 )
   {
      if ( !( find.attr & F_ATTR_DIR ) )
      {
          int nHours = 0x08 << 11;
          int nMins  = 0x0c << 5;
          int nSecs = 0x2d;
          int nTime = 0;
          nTime = nHours | nMins | nSecs;

          int nDay = 0x0a << 5;
          int nMonth  = 0x06;
          int nYear = 0x14 << 9;
          int nDate = 0;
          nDate = nDay | nMonth | nYear;

          int nret = f_settimedate( find.filename, nTime, nDate );

          //SetupMttty();
          //iprintf("\n time date %x %x,",find.ctime,find.cdate);
//          SetupMttty();
//          iprintf("\n file name is %s",find.filename);
   if ( nret == F_NO_ERROR )
   {
//      SetupMttty();
//      iprintf( "Time stamping successful\r\n" );
   }
   else
   {
      SetupMttty();
      iprintf( "Time stamping failed: %d\r\n", nret );
   }

      }

    }
*/



