extern  int g_ntestonly;



//
// P.Smith                      24/1/06
// added modbus master initialisation, include "MBMaster.h"
// call InitialiseModbusMaster();
//      ConfigureModbusMaster();
// include "MBMHand.h"
// call InitialiseSerialPorts();
//      Init_EdgePort();
//      void SetIntc(long func, int vector, int level, int prio );  //from yahoo
//      SetIntc
// added INTERRUPT(IRQ5_pin_isr, 0x2500 )
// call Duart A serial handler. SerialHandler();
// set g_CalibrationData.m_nPeripheralCardsPresent = SEIENBITPOS;
// set g_bSBBL3Fitted to false to ensure that remote display does not operate

// P.Smith                    26/1/06
// added parallel printer test.
// include "PrinterFunctions.h"
// added extern  char        g_cPrinterBuffer[];
//
// P.Smith                  8/2/06
// removed SetUpPITR from this module.
// remove INTERRUPT(my_pitr_func,0x2600), now in seperate file.
// added "L" option as the blender application.  case 'L':
// added extern int g_nFlashRate and setup of this variable when blender application
// is selected.
//
// Removed long g_lRawADCountsLTC2415 = 0;  //global
// removed unsigned int g_nLTC2415Err;  //
// long lTemp;
// removed //nbb   WORD g_nOneSecondSumCounter = 0;
           //nbb   DWORD g_lOneSecondSum = 0;
           //nbb   DWORD g_lOneSecondWeightAverage = 0;
           //nbb   BOOL g_bOneSecondWeightAverageAvailable = FALSE;
//
//
// P.Smith                  14/2/06
//	It was noted that he a/d conversion cannot run at the required 15hz conversion rate.
// It will only run at 12 hz without giving a conversion error.
//  remove DWORD g_lRawAtDCounts;
//
// P.Smith                  14/2/06
// renamed master comms interrupt handler IRQ5
// added initialisation required to allow the master comms to run.
// verified that the master commms is running from the blender application.
//
// P.Smith                  15/2/06
// call     CopyProdSummDataToMB();          //  Copy production summary to modbus table.
// only display diagnostics if debug flag is set
// call foreground program from here.
//
// P.Smith                  24/2/06
// remove 1 second delay, allow foreground program to be called continuously.
//
// P.Smith                  27/2/06
// called InitialiseBlender();
//
// P.Smith                  21/3/06
// removed    g_bSBBL3Fitted  = TRUE; from blender application start up

// P.Smith                  28/3/06
// remove diagnostics recipe set up from this program. also remove copy of
// test data to modbus table.

// P.Smith                  28/3/06
// Remove set of % on power up

// P.Smith                  29/5/06
/// correct printf in ltc2415 test, too many variables for the arguments
// Go straight to the blender program, no prompt to the user

// P.Smith                  9/6/06
// call new serial comms test
// name change g_nDebug -> g_nTSMDegbug
// TestNBBHardware( ); called for now for test "c"
// Removed debug structure from here and put in debug.h
//
// P.Smith                  20/6/06
// email added by Mmk Testemail();
// #include "Email.h"
// #include <mailto.h>
//
// P.Smith                  28/6/06
// removed CopyTimeToMB();
//
// P.Smith                  19/7/06
// added sd card test software, added correct read of 2432 chip
//
// P.Smith                  13/10/06
// added g_cMasterCommsBuffer & g_nMasterCommsPointer;

//
// P.Smith                  15/1/07
// Remove reference to Duart.
//
// P.Smith                  16/1/07
// call U0_Select_Mtty on power up.
//
// P.Smith                  24/1/07
// Put in the latest a/d check test no 4.
// removed TestNBBHardware
// Remove reference to parallel printing
// Redirect standard i/o to port U2
// Remove InitialiseNBBGpio secondly, this was causing the application not to start up if the panel was connected
// on reset
// Correct test 3 & 4
//
// P.Smith                  5/2/07
// Remove hardware access relating to rev a3 nbb
// check operation of uart 0 multiplexing
//
// P.Smith                  5/2/07
// added pulse on time measurement
// check for g_bRedirectStdioToPort2 on power up to determine if the port should
// be redirected.
// remove all printf functions
//
// P.Smith                  5/2/07
// check for g_bRedirectStdioToPort2 put before initialise of blender varaibles.
// call EnableSlaveTransmitter
// call CheckFlowRateChecksum
//
// P.Smith                  8/3/07
// add ultrasonic test M
//
// P.Smith                  11/5/07
// remove printfs
//
// P.Smith                  28/5/07
// added in check for files used for software update.
// flash led to indicate that software update has taken place.
//
// P.Smith                  19/12/07
// call SetupMttty
//
// P.Smith                  9/1/08
// put back in InitialiseSDCard,remove OSTimeDly, call ReInitialisePinsUsedBySDCard
// change speed of ShowThatUpdateDateOccurred
//
// P.Smith                  22/1/08
// remove ReInitialisePinsUsedBySDCard, this is stopping the default load from working.
//
// P.Smith                  28/1/08
// remove unused variables in software update
//
// P.Smith                  7/5/08
// correct check on software version so that any length of file name can be
// checked, remove iprintf("\nUpdate complete rebooting in 2 seconds\r\n");
// this was causing the software udpate not to work properly from the panel.
// call ReInitialisePinsUsedBySDCard
//
// P.Smith                  6/6/08
// set g_bSDCardSoftwareUpdateComplete to show sofware update is complete
//
// M.McKiernan                         15.7.2008
// Added SoftwareUpdateViaPanel( void )....called from ProgLoop when triggered from panel.
//
// P.Smith                  18/7/08
// check for sd card present before accessing sd card by calling SDCardPresent
//
// P.Smith                          23/7/08
// remove g_PowerUpTime & g_CurrentTime extern
//
// P.Smith                          23/7/08
// call ReInitialisePinsUsedBySDCard at end of panel software update
//
//
// P.Smith                           14/10/08
// correct compiler warnings
//
// P.Smith                          16/1/09
// set and clear g_bSPIBusBusy when checking for software update.

//
// M.McKiernan                      1/12/09
// Removed call to #include <effs_fat/mmc_drv.h> - mmc_drv.h not present in eclipse version

//
// M.McKiernan                      2/03/2010
// force service of watchdog in spi routines. - g_bSoftwareUpdateFromSDInProgress = TRUE;

//
// P.Smith                      16/03/10
// no software update if watch dog is enabled
//

#include "predef.h"
#include <stdio.h>
#include <ctype.h>
#include <startnet.h>
#include <autoupdate.h>
#include <dhcpclient.h>
#include <http.h>
#include <htmlfiles.h>
//#include <..\mod5272\system\sim5272.h>
#include <C:\Nburn\MOD5270\system\sim5270.h>
#include <cfinter.h>
#include <smarttrap.h>

//#include "QSPIdriver.h"
//#include "EEPROMconstants.h"
//#include "EEPROMdriver.h"

#include "LTC2415.h"
#include "OneWire.h"
#include "I2CFuncs.h"

#include "InitComms.h"
//#include <C:\Nburn\examples\MySPI\InitComm.h>

#include "MAX132.h"
#include "MBSHand.h"
#include "PITHandler.h"

#include <gpio5270.h>
#include <NBBGpio.h>


//#include <C:\Nburn\examples\MySPI\QSPIdriver.h>
#include "QSPIdriver.h"
#include "EEPROMconstants.h"
#include "EEPROMdriver.h"
#include "QSPIconstants.h"
// for file system:
#include <effs_fat/fat.h>
#include <effs_fat/mmc_mcf.h>
#include <effs_fat/cfc_mcf.h>
#include "InitialiseHardware.h"
#include "BatchCalibrationFunctions.h"
#include "Batvars.h"
#include "Batvars2.h"
#include "MBMaster.h"
#include "MBMHand.h"
#include "PrinterFunctions.h"
#include "string.h"
#include "initblnd.h"
#include "init5270.h"
#include "RTCHand.h"
#include "NBBVars.h"
#include "MBProsum.h"
#include "foreground.h"
#include "Setpointformat.h"  //nbb--testonly--
#include "Mbprogs.h"  //nbb--testonly--
#include "TimeDate.h"
#include <NetworkDebug.h>
#include "Debug.h"
#include <mailto.h>
#include "SHA1.h"
#include <Serial.h>
#include "LSpeed.h"
#include "fileup.h"
#include <Bsp.h>
#include "Dictionary.h"
#include "Iniexample.h"
#include "Sdcard.h"
#include <pins.h>
#include <String.h>
#include <ucosmcfc.h>
#include "SoftwareUpdate.h"
#include <effs_fat/fat.h>
#include <effs_fat/mmc_mcf.h>
#include <effs_fat/cfc_mcf.h>
// eclipse #include <effs_fat/mmc_drv.h>
#include "InitialiseHardware.h"


extern bool g_bSoftwareUpdateFromSDInProgress;
extern bool g_bWatchdogStatus;

/////////////////////////////////////////////////////
// CheckForSoftwareUpdate()
//
// If update.txt exists, open file, read software file name (e.g. bb-software-12-34.s19)
// If AUTOUPDATE text exists, remove text, and update the software using the specified s19 file.
// If protected update exists, method works the same as update.txt, except auto update string is not removed.
// This will be used on master SD-cards during production.
// A new screen in the blender will allow a manual triggering of a software update.
// This will work from update.txt only, and will read the software file name from update.txt before flashing.

//////////////////////////////////////////////////////



void CheckForSoftwareUpdate( void )
{
    g_bSPIBusBusy = TRUE;
    if( SDCardPresent() )
    {
    int rv;
    int rc;
    char cFileData[20] = "";
    char cFileName[20] = "";
    char cBuffer[40];


    F_FILE * fp = 0;
    F_FIND finder;
    BOOL    bFoundUpdateFile = FALSE;
    BOOL    bFoundProtectedFile = FALSE;
    g_bSDCardSoftwareUpdateComplete = FALSE;

    USER_ENTER_CRITICAL(); //We can't interrupt the insert

    g_bSoftwareUpdateRunning = TRUE;
//    iprintf("\n checking sd card");
    InitialiseSDCard();         // initialise sd card
    rc=f_findfirst("*.*",&finder);
//    SetupMttty();
//    iprintf("\n rc is %d",rc);
    if (rc==0)
    {
        do
        {
//            SetupMttty();
//            iprintf("Found [%s]\r\n",finder.filename);
            if ( strcmp( "PUPDATE.TXT", finder.filename ) == 0 )

            {
//                SetupMttty();
//                iprintf("\n protected update found");
                bFoundProtectedFile = TRUE;
            }

            if ( strcmp( "UPDATE.TXT", finder.filename ) == 0 )

            {
//                iprintf("\n found updated file");
                bFoundUpdateFile = TRUE;
            }
        } while (!f_findnext(&finder));
//        iprintf("\n found undate is %d",bFoundUpdateFile);

        if(bFoundProtectedFile)
        {
          fp = f_open("PUPDATE.TXT", "r" );
//            iprintf("\n open protected updated file");
        }
        else
        if(bFoundUpdateFile)
        {
            fp = f_open("UPDATE.TXT", "r" );
        }
        if ( fp )
            {
                // now read the software name
                int nCharPosition = 0;
                int i,n;
                n = f_read( cBuffer, 1, 30, fp );
                for( i= 0; i < 40; i++ )
                {
                    if(cBuffer[i] == '>')  // search for >
                    nCharPosition = i;
                }
// copy in file name
                for( i= 0; i < nCharPosition; i++ )
                {
                    cFileName[i] = cBuffer[i];
                }

// copy in file name
                for( i= 0; i < 11; i++ )
                {
                    cFileData[i] = cBuffer[i+nCharPosition];
                }

                if ( strcmp( ">autoupdate", cFileData ) == 0 )
                {
                if(bFoundUpdateFile)
                    {
                       fp = f_open("UPDATE.TXT", "w" );
                       f_write(cFileName,strlen(cFileName),1,fp);
                       f_write("                    ",20,1,fp);
                       f_close(fp);
                    }

          //          SetupMttty();
          //          iprintf("\n update is required file name is %s111",cFileName);
             	    fp = f_open(cFileName, "r" );
//                   iprintf("\n calling fat update now");

                	//this will force service of watchdog in spi routines.
                    g_bSoftwareUpdateFromSDInProgress = TRUE;

                    rv = UpdateFromFat( fp, false );
//                    SetupMttty();
//                    iprintf("\n rv is %d",rv);

                    if ( rv == FAT_UPDATE_OK )
         		    {
                              f_delvolume( CFC_DRV_NUM );
                              f_close(fp);
                              g_bSDCardSoftwareUpdateComplete = TRUE;
                               ShowThatUpdateDateOccurred();
                    //          SetupMttty();
                    //          iprintf("\nUpdate complete rebooting in 2 seconds\r\n");
                              ForceReboot();
                    }
                    else if ( rv == FAT_UPDATE_WRONG_PLATFORM )
                    {
                        iprintf("Update not done, wrong Platform\r\n");
                    }
                    else if ( rv == FAT_UPDATE_SAMEVERSION )
                    {
                        iprintf("Update not needed already running latest version\r\n");
                    }

                    if ( rv == FAT_UPDATE_NO_MEM )
                    {
                        iprintf("Update faile dunable to malloc memory buffer\r\n");
                    }

                    if ( rv == FAT_UPDATE_BAD_FORMAT )
                    {
                        iprintf("Update failed file is in wrong forrmat or corrupt\r\n");
                    }


                }
            }
      }
      f_close(fp);
      ReInitialisePinsUsedBySDCard();
      g_bSoftwareUpdateRunning = FALSE;

      USER_EXIT_CRITICAL(); //We can't interrupt the insert
    }
    g_bSPIBusBusy = FALSE;

}


/////////////////////////////////////////////////////
// SoftwareUpdateViaPanel()
//
// SoftwareUpdateViaPanel is similar to CheckForSoftwareUpdate, except that it loads the software
// from a fixed filename "SwUpdate.s19" if it exists on the SD card
// It is triggered by a command from the panel (modbus).
//
//////////////////////////////////////////////////////
void SoftwareUpdateViaPanel( void )
{
    if( SDCardPresent() && (g_bWatchdogStatus == DISABLED))
    {
        int rv;
        int rc;

        F_FILE * fp;
        F_FIND finder;

        InitialiseSDCard();         // initialise sd card

        USER_ENTER_CRITICAL(); //We can't interrupt the insert
        g_bSoftwareUpdateRunning = TRUE;
        rc=f_findfirst("*.*",&finder);
        if (rc==0)
        {

       	    fp = f_open("SWUPDATE.S19", "r" );
            if ( fp )
            {
            	//this will force service of watchdog in spi routines.
                g_bSoftwareUpdateFromSDInProgress = TRUE;

                rv = UpdateFromFat( fp, false );
                OSTimeDly( 2 * TICKS_PER_SECOND );
                if ( rv == FAT_UPDATE_OK )
      		    {
                    f_delvolume( CFC_DRV_NUM );
                    OSTimeDly( 2 * TICKS_PER_SECOND );
                    f_close(fp);
                    g_bSDCardSoftwareUpdateComplete = TRUE;
                    ShowThatUpdateDateOccurred();
                    ReInitialisePinsUsedBySDCard();
                    ForceReboot();
                }
                else if ( rv == FAT_UPDATE_WRONG_PLATFORM )
                {
//                        iprintf("Update not done, wrong Platform\r\n");
                }
                else if ( rv == FAT_UPDATE_SAMEVERSION )
                {
//                        iprintf("Update not needed already running latest version\r\n");
                }

                if ( rv == FAT_UPDATE_NO_MEM )
                {
//                        iprintf("Update faile dunable to malloc memory buffer\r\n");
                }

                if ( rv == FAT_UPDATE_BAD_FORMAT )
                {
//                        iprintf("Update failed file is in wrong forrmat or corrupt\r\n");
                }
           }
       }
       g_bSoftwareUpdateRunning = FALSE;
       ReInitialisePinsUsedBySDCard();
       USER_EXIT_CRITICAL(); //We can't interrupt the insert
    }
}




void ShowThatUpdateDateOccurred( void )
{
    int i;
    g_bPITAllowed = FALSE;

    for( i= 0; i < 50; i++ )
    {
        ToggleHBLED();
        OSTimeDly(TICKS_PER_SECOND/15);
    }
    for( i= 0; i < 50; i++ )
    {
        ToggleHBLED();
        OSTimeDly(TICKS_PER_SECOND/30);
    }
    for( i= 0; i < 5; i++ )
    {
       ToggleHBLED();
        OSTimeDly(TICKS_PER_SECOND/1);
    }
    g_bPITAllowed = TRUE;

}




