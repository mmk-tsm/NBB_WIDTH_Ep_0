//////////////////////////////////////////////////////
// P.Smith                      29/10/08
//
// if sd card not inserted print this message when getting directory.
// show :> prompt when waiting for input
// added CheckForTelNetTimeout & TerminateTelnetSession
//
// P.Smith                      13/11/08
// add g_bPrintCycleDataToTelnet toggle
// more info to blender info page auto / manual status
// a problem has been seen with the memory allocation where 8192 is only available for the
// stack, in showconfig 10000 chars were define and a further 10000 defined in a file
// this screwed up the variable g_sVNC1L.m_bTransactionInProgress when the read function
// was activated and the a/d stopped working.
//
// P.Smith                      17/11/08
// add continuous update for hopper cal page and blender info page.
// show comms counters for all pages
//
// P.Smith                      18/11/08
// check for sd card present in ShowFileSegment
// if space then, show file in segments
// if "a" show all of file
//
// P.Smith                      21/11/08
// added ANALOG_SWITCH_ON() ANALOG_SWITCH_OFF() activation from telnet,commented
// out for now.
// in CheckForTelNetTimeout, check for telnet in session before timing out.
// check fdtelnet to determine if the port is open before printing data.
//
// P.Smith                      25/11/08
// ensure telnet port is open before printing
// clear comms counters added.
// show mac address as well
// show test results added, stop continuous update when this is done.
// add auto manual, pause to info / blender info page.
// add hardware id.
// in CheckForTelNetTimeout, check that telnet is in session before allow check.
//
// P.Smith                      1/12/08
// Correct telnet set % text.
//
// P.Smith                      2/12/08
// remove cycle counter from blender info page.
//
// P.Smith                      3/12/08
// add telnet session start / end time when entering or exiting session
// added show comms history, "show hourly comms history,"show ongoing events history
//
// P.Smith                      9/12/08
// added history data
// comms, events,component events, weighing events and the history of these.
// the history info is created once an hour, while the ongoing is updated continuously.
// update menu for these.
//
// P.Smith                      9/12/08
// name change g_bSDCardAccessInProgress to g_bTelnetSDCardAccessInProgress
// print component history on help menu
// show level sensor status on summary page.
// reduce hopper cal buffer to 600, was 1000.
// correct sei comms printout.
// remove instr function, now used in usb
// add usb debug
//
// P.Smith                      12/2/09
// printer usb debug on / off added
//
// P.Smith                      20/2/09
// add blender self test, removed for now
//
// P.Smith                      5/3/09
// added self test command
// update factory settings telnet command added.
// put back in "swich in" "switch out" commands but must be diagnostics password
// show no of minutes in auto
// name change to tare max min values
//
// P.Smith                      23/4/09
// added nbb serial no and sei software revision to info page
// added insert sd card at end of show test results if the card is not inserted
//
// P.Smith                      13/5/09
// allow for back space in telnet commands.
// remove show commands from help menu.
// put in prompts where necessary after running some of the commands.
// change text on ongoing comms
// change the way that the nbb serial number is accessed, add null terminator
//
// P.Smith                      25/5/09
// do not terminate telnet session if continuous update or batch log on.
// correct text in help menu noing -> ongoing
//
// P.Smith                      25/6/09
// add debug loader on / off
//
//
// P.Smith                      2/7/09
// copy command to temp_string_copy so that it can be used again when the command
// is repeated
// added enable / disable logging and csv
//
// P.Smith                      25/8/09
// debug pulsing on / off added.
// debug flow on / off added
//
// P.Smith                      10/9/09
// set g_bSelfTestInProgress when self test is initiated
//
// P.Smith                      17/9/09
// change event batch log enable to or in the new settings for the log to sd card
// this is now an integer.
//
// P.Smith                      19/11/09
// added "debug atd on" and "debug atd off"
// add short cuts for commands, ref test and lc test
// in TerminateTelnetSession clear debug structure

//
// M.McKiernan                      1/12/09
// Removed call to #include <effs_fat/mmc_drv.h> - mmc_drv.h not present in eclipse version
//
// P.Smith                          15/12/09
// compiler warning corrected by initialising n to 0.
// in ShowHopperCalibrationPage use %ld when printing long
// same in UpdateHopperCalibration when printing g_lOneSecondWeightAverage
//
// P.Smith                          19/1/10
// added CheckForTelnetCommands, there is a problem calling the sd card
// access directly from the Telnet function.
// all sd card access commands are added into this function
//
// P.Smith                          20/1/10
// add show function to show individual files on the sd card.
//
// P.Smith                          8/3/10
// set g_bSoftwareUpdateFromSDInProgress in ShowFileSegment to disable
// watch dog timer
//
// P.Smith                          16/3/10
// modified showing of test results to be run in the foreground
// as is the other access of the sd card over telnet.
//
// P.Smith                          7/4/10
// call ShowSecurityResults to show security data when the show
// info command is run.
// add header to the show test results so that the blender serial no
// etc can be recorded.
//
// P.Smith                          12/4/10
// added ShowLicenseInfo call to show license info on printout.
// change security text slightly.
//
// P.Smith                          26/4/10
// added pause on / pause off to the telnet command set.

// M.McKiernan		wfh				June/2020
// Added ultrasonics debug commands uson and usoff - g_nTSMDebug.m_bUltrasonicsDebug = TRUE;
// print the software revision at the username prompt.
//////////////////////////////////////////////////////

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
#include    "Alarms.h"
#include    "Telnetcommands.h"
#include "Eventlog.h"
#include "VNC1L.h"
#include "Conalg.h"
#include "Serialstuff.h"
#include "MBTogSta.h"
#include "BlenderSelfTest.h"
#include "Gettext.h"
#include "Watchdog.h"
#include "License.h"
#include "Pause.h"


extern char TelnetRxBuffer[];
extern StructAlarmTable    g_MainAlarmTable;
extern  structSetpointData  g_CurrentRecipe;
extern CalDataStruct    g_CalibrationData;
extern  char    *RevisionNumber;
extern int     g_nGeneralTickTimer;        // PIT ticks
extern structVNC1LData   g_sVNC1L;
extern  char    *strCR;
extern  char    *strAutomatic[];
extern  char    *strManual[];
extern  structControllerData    structSEIData;
extern  structControllerData    structLLSData;
extern  structControllerData    structOptimisationData;
extern  structCommsData         g_sModbusNetworkSlave;
extern  BYTE DS2432Buffer[];
extern bool g_bSoftwareUpdateFromSDInProgress;
extern  WORD g_nHashOfSHAIDInFlash;
extern  WORD g_nHashOfSHAID;
extern WORD g_nHashOfSHAID;
extern WORD g_nHashOfSHAIDInFlash;
extern WORD g_nSHASecretFailCtr;
extern WORD g_nSHAHashOfIdFail;
extern WORD g_nTimeHashFail;
extern WORD g_nHashOfTime;
extern  OnBoardCalDataStruct   g_OnBoardCalData;


	char 	temp_string[80];
	char 	temp_string_copy[80];
	BOOL	g_bShowConfigInTelnet = FALSE;
	BOOL	g_bShowEventLogInTelnet = FALSE;
	BOOL	g_bShowBatchLogInTelnet = FALSE;
	BOOL	g_bShowFileSegmentInTelnet = FALSE;
	BOOL	g_bShowAllFileSegmentsInTelnet = FALSE;
	BOOL	g_bShowFile1inTelnet = FALSE;
	BOOL	g_bShowFile2inTelnet = FALSE;
	BOOL	g_bShowFile3inTelnet = FALSE;
	BOOL	g_bShowFile4inTelnet = FALSE;
	BOOL	g_bShowFile5inTelnet = FALSE;
	BOOL	g_bShowFile6inTelnet = FALSE;
	BOOL	g_bShowFile7inTelnet = FALSE;
	BOOL	g_bShowFile8inTelnet = FALSE;
	BOOL	g_bShowFile9inTelnet = FALSE;
	BOOL	g_bShowFile10inTelnet = FALSE;
	BOOL	g_bShowSpecificFileinTelnet = FALSE;
	BOOL	g_bShowTestResults = FALSE;

    char    g_cFileName[20];


char *Commands[] = {
                "quit",
				"?",
				"help",
				"play",
				"record",
				"stop",
				"init",
				"reboot",
				"clear ram",
				"status"};

/////////////////////////////////////////////////////
// ProcessTelnetCommand()
// process telnet command
// if space and segment no non zero, show file segment
// if "a" show rest of file.
// scan input string for cr if found process command
//
//
//////////////////////////////////////////////////////

void ProcessTelnetCommand( char *cCommand,int n )
{
    BOOL bCRFound;
    int i,nlength;
    if((cCommand[0] == 0x20) && (g_nFileSegmentNo !=0) && g_bTSMTelnetInSession)
    {
            g_bShowFileSegmentInTelnet = TRUE;
    }
    else
    if((cCommand[0] == 'a') && (g_nFileSegmentNo !=0) && g_bTSMTelnetInSession)
    {
            g_bShowAllFileSegmentsInTelnet = TRUE;
     }
// search for cr
    bCRFound = FALSE;
    for(i=0; i< n; i++)
    {
        if(cCommand[i] == 0x0d)
        {
            bCRFound = TRUE;
	        cCommand[i] = '\0';
        }

    }
    if(cCommand[0] == 0x08)  // allow for back space
    {
        nlength = strlen(temp_string);
        temp_string[nlength-1] = '\0';  // step back one character
        if(fdTelnet > 0)
        iprintf("\n:>%s",temp_string);  // dump out string again.
    }
    else
    {
    strcat(temp_string,cCommand);
    }
      if(bCRFound)		// if enter hit
	{
        strcpy(temp_string_copy,temp_string);
        Parse_Commands(temp_string);
	    strcpy(temp_string,"");
	}
}


/////////////////////////////////////////////////////
// Parse_Commands()
// parse telnet commands
//
//
//
//////////////////////////////////////////////////////
void Parse_Commands( char *cCommand )
{
    char    cBuffer[200];
    char    tBuffer[200];

    // log in command
    if((strcmp(cCommand, "login") == 0) && !g_bTSMTelnetInSession)
    {
        ShowUserNamePrompt();
    }
    // user name
    else if(g_nTelnetSequence == TELNET_USERNAME_ENTRY)
    {
        g_nTelNetTimeoutCtr = 0;
        g_nTelnetSequence = TELNET_PASSWORD_ENTRY;
        if(fdTelnet > 0)
        {
            iprintf("\n password ");
        }
        if(strcmp(cCommand, "tsm") == 0)  // password
        {
            g_bTelnetUserNameOkay = TRUE;
        }

        else  // terminate session
        {
            g_bTelnetUserNameOkay = FALSE;
        }
    }
    else if(g_nTelnetSequence == TELNET_PASSWORD_ENTRY)
    {
        if((strcmp(cCommand, "oriel") == 0) && g_bTelnetUserNameOkay)
        {


            cBuffer[0] = '\0';
            sprintf(tBuffer,"\n\n ********TSM Telnet in session*****");
            strcat( cBuffer, tBuffer );
            sprintf( tBuffer, "%02d/%02d/%04d\t%02d:%02d:%02d\t\n",g_CurrentTime[TIME_DATE],g_CurrentTime[TIME_MONTH],g_CurrentTime[TIME_YEAR], g_CurrentTime[TIME_HOUR], g_CurrentTime[TIME_MINUTE], g_CurrentTime[TIME_SECOND] );
            strcat( cBuffer, tBuffer );
            if(fdTelnet > 0)
            iprintf("%s",cBuffer);

            g_nTelnetSequence = 0;
            g_bTSMTelnetInSession = TRUE;
            ShowHelpMenu();
            g_nTelnetContinuousUpdate = 0;

        }
        else
        {
            ShowUserNamePrompt();
        }
    }

    else if(g_bTSMTelnetInSession)
    {
        if(strcmp(cCommand, "batch log on") == 0)    // batch log on
        {
            g_bPrintCycleDataToTelnet = TRUE;
            if(fdTelnet > 0)
            {
                iprintf("batch log switched on\n");
            }
        }
        else if(strcmp(cCommand, "toauto") == 0)   // switch blender to auto
        {
            if(g_bManualAutoFlag == MANUAL)
            {
                ToAuto();       // asm = TOAUTO
                if(fdTelnet > 0)
                {
                    iprintf("blender switched to auto");
                }
            }
        }
        else if(strcmp(cCommand, "show comms") == 0)  // show comms counters
        {
            ShowCommunications();
        }

        else if(strcmp(cCommand, "tomanual") == 0)   // switch to manual
        {
            if(g_bManualAutoFlag == AUTO)
            {
               ToManual();     // asm = TOMANUAL
               g_bToManualEvent = TRUE;
               g_nSDCardDelayTime = SDCARDCOMMANDDELAYTIME;
               if(fdTelnet > 0)
               {
                   iprintf("blender switched to manual");
               }
            }
        }
        else if(strcmp(cCommand, "batch log off") == 0)   // switch batch log off
        {
            g_bPrintCycleDataToTelnet = FALSE;
             if(fdTelnet > 0)
             iprintf("batch log switched off\n");
        }
        else if(strcmp(cCommand, "help") == 0 || strcmp(cCommand, "h") == 0)
        {
            ShowHelpMenu();
        }
        else if(strcmp(cCommand, "show info") == 0 ||strcmp(cCommand, "i") == 0 )
        {
             Showinfo();
        }
        else if(strcmp(cCommand, "show config") == 0 ||strcmp(cCommand, "sc") == 0 )
        {
        	g_bShowConfigInTelnet = TRUE;
        }
        else if(strcmp(cCommand, "show event log") == 0 ||strcmp(cCommand, "sel") == 0 )
        {
        	g_bShowEventLogInTelnet = TRUE;
        }
        else if(strcmp(cCommand, "show batch log") == 0 ||strcmp(cCommand, "sbl") == 0 )
        {
        	g_bShowBatchLogInTelnet = TRUE;
        }
        else if(strcmp(cCommand, "show alarms") == 0 ||strcmp(cCommand, "sa") == 0 )
        {
            ShowAlarms();
        }

        else if(strcmp(cCommand, "clear comms") == 0 ||strcmp(cCommand, "cc") == 0 )
        {
            TelnetClearComms();
        }
        else if(strcmp(cCommand, "show test results") == 0 ||strcmp(cCommand, "str") == 0 )
        {
        	g_bShowTestResults = TRUE;
        }
        else if(strcmp(cCommand, "pause on") == 0 ||strcmp(cCommand, "pn") == 0 )
        {
        	if(fdTelnet>0)
        	{
        		iprintf("blender paused\n");
        	}
        	EndOfCyclePauseOn();
        }
        else if(strcmp(cCommand, "pause off") == 0 ||strcmp(cCommand, "pf") == 0 )
        {
        	if(fdTelnet>0)
        	{
        		iprintf("\n blender unpaused\n");
        	}
        	PauseOff();
        }
        // mmk wfh
        else if(strcmp(cCommand, "uson") == 0 )
        	g_nTSMDebug.m_bUltrasonicsDebug = TRUE;
        else if(strcmp(cCommand, "usoff") == 0 )
        	g_nTSMDebug.m_bUltrasonicsDebug = FALSE;

        else if(strcmp(cCommand, "self test") == 0 ||strcmp(cCommand, "st") == 0 )
        {
            g_bDoSelfTestDiagnostics = TRUE;
            g_bSelfTestInProgress = TRUE;
            if(g_bCycleIndicate)
            {
                if(g_bTSMTelnetInSession && (fdTelnet > 0))
                iprintf("\n Please wait, cycle in progress");
            }
        }

        else if(strcmp(cCommand, "dir") == 0  ||strcmp(cCommand, "d") == 0 )
        {
            g_bReadSDCardDirectory = TRUE;
         }
        else if(strcmp(cCommand, "show 1") == 0)
        {
            g_bShowFile1inTelnet = TRUE;
        }
        else if(strcmp(cCommand, "show 2") == 0)
        {
            g_bShowFile2inTelnet = TRUE;
        }
        else if(strcmp(cCommand, "show 3") == 0)
        {
            g_bShowFile3inTelnet = TRUE;
        }
        else if(strcmp(cCommand, "show 4") == 0)
        {
            g_bShowFile4inTelnet = TRUE;
        }
        else if(strcmp(cCommand, "show 5") == 0)
        {
            g_bShowFile5inTelnet = TRUE;
        }
        else if(strcmp(cCommand, "show 6") == 0)
        {
            g_bShowFile6inTelnet = TRUE;
        }
        else if(strcmp(cCommand, "show 7") == 0)
        {
            g_bShowFile7inTelnet = TRUE;
        }
        else if(strcmp(cCommand, "show 8") == 0)
        {
            g_bShowFile8inTelnet = TRUE;
        }
        else if(strcmp(cCommand, "show 9") == 0)
        {
            g_bShowFile9inTelnet = TRUE;
        }
        else if(strcmp(cCommand, "show 10") == 0)
        {
            g_bShowFile10inTelnet = TRUE;
        }
       else if(strcmp(cCommand, "show blender info") == 0  ||strcmp(cCommand, "bi") == 0 )
        {
            ShowBlenderInfoPage();
        }
       else if(strcmp(cCommand, "debug modbus on") == 0 ||strcmp(cCommand, "dmon") == 0 )
        {
            NetworkWritesOn();
        }

       else if(strcmp(cCommand, "debug modbus off") == 0 ||strcmp(cCommand, "dmoff") == 0 )
        {
            NetworkWritesOff();
        }
       else if(strcmp(cCommand, "debug loader on") == 0 ||strcmp(cCommand, "dlon") == 0 )
        {
            LoaderDebugOn();
        }
       else if(strcmp(cCommand, "debug loader off") == 0 ||strcmp(cCommand, "dloff") == 0 )
        {
            LoaderDebugOff();
        }
               else if(strcmp(cCommand, "debug pulsing on") == 0 ||strcmp(cCommand, "dpon") == 0 )
        {
            g_nTSMDebug.m_bPulsingDebug = TRUE;
        }
       else if(strcmp(cCommand, "debug pulsing off") == 0  ||strcmp(cCommand, "dpoff") == 0 )
        {
            g_nTSMDebug.m_bPulsingDebug = FALSE;
        }
        else if(strcmp(cCommand, "debug flow on") == 0 ||strcmp(cCommand, "dfon") == 0 )
        {
            g_nTSMDebug.m_bFlowRateDebug = TRUE;
        }
        else if(strcmp(cCommand, "debug flow off") == 0 ||strcmp(cCommand, "dfoff") == 0 )
        {
            g_nTSMDebug.m_bFlowRateDebug = FALSE;
        }

       else if(strcmp(cCommand, "debug atd on") == 0 ||strcmp(cCommand, "datdon") == 0 )
       {
            g_nTSMDebug.m_bAToDStabilityDebug = TRUE;
       }
       else if(strcmp(cCommand, "debug atd off") == 0 ||strcmp(cCommand, "datdoff") == 0 )
       {
            g_nTSMDebug.m_bAToDStabilityDebug = FALSE;
       }
       else if(strcmp(cCommand, "show hopper cal") == 0  ||strcmp(cCommand, "hc") == 0 )
        {
            ShowHopperCalibrationPage();
        }
       else if(strcmp(cCommand, "ongoing comms") == 0  ||strcmp(cCommand, "co") == 0 )
        {
            ShowOngoingCommsHistory();
        }
       else if(strcmp(cCommand, "comms history") == 0 ||strcmp(cCommand, "ch") == 0 )
        {
            ShowHourlyCommsHistory();
        }
       else if(strcmp(cCommand, "ongoing events") == 0  ||strcmp(cCommand, "e") == 0 )
        {
            ShowOngoingEventsHistory();
        }
       else if(strcmp(cCommand, "events history") == 0  ||strcmp(cCommand, "eh") == 0 )
        {
            ShowHourlyEventsHistory();
        }
       else if(strcmp(cCommand, "component events") == 0  ||strcmp(cCommand, "ce") == 0 )
        {
            ShowOngoingComponentEventsHistory();
        }
       else if(strcmp(cCommand, "component history") == 0 ||strcmp(cCommand, "che") == 0 )
        {
            ShowHourlyComponentEventsHistory();
        }
       else if(strcmp(cCommand, "weighing events") == 0 ||strcmp(cCommand, "we") == 0 )
        {
            ShowOngoingWeighingevents();
        }
       else if(strcmp(cCommand, "weighing history") == 0  ||strcmp(cCommand, "wh") == 0 )
        {
            ShowHourlyWeighingevents();
        }
       else if(strcmp(cCommand, "printer usb debug on") == 0 ||strcmp(cCommand, "pdon") == 0 )
        {
            g_nTSMDebug.m_bUSBPrinterDebug = TRUE;
            iprintf("\n usb printer debug on \n");
        }
       else if(strcmp(cCommand, "printer usb debug off") == 0 ||strcmp(cCommand, "pdoff") == 0 )
        {
            g_nTSMDebug.m_bUSBPrinterDebug = FALSE;
            iprintf("\n usb printer debug off \n");
        }
       else if(strcmp(cCommand, "update factory settings") == 0 ||strcmp(cCommand, "uf") == 0 )
        {
            UpDateFactorySettings();
            iprintf("\n load cell factory settings updated\n");
        }

       else if(strcmp(cCommand, "enable logging") == 0 ||strcmp(cCommand, "el") == 0 )
        {
            iprintf("\n logging to sd card enabled");
            g_CalibrationData.m_nLogToSDCard = (LOG_EVENTS_TO_SD_CARD | LOG_BATCH_LOG_TO_SD_CARD);
            g_bSaveAllCalibrationToEEprom = TRUE;
            CopyCalibrationDataToMB();


        }
       else if(strcmp(cCommand, "disable logging") == 0 ||strcmp(cCommand, "dl") == 0 )
        {
            iprintf("\n logging to sd card disabled");
            g_CalibrationData.m_nLogToSDCard = 0;
            g_bSaveAllCalibrationToEEprom = TRUE;
            CopyCalibrationDataToMB();
        }
       else if(strcmp(cCommand, "csv logging on") == 0)
        {
            iprintf("\n csv logging on");
            g_CalibrationData.m_bLogFormatIsCSV = TRUE;
            g_bSaveAllCalibrationToEEprom = TRUE;
            CopyCalibrationDataToMB();
        }
       else if(strcmp(cCommand, "csv logging off") == 0)
        {
            iprintf("\n csv logging off");
            g_CalibrationData.m_bLogFormatIsCSV = FALSE;
            g_bSaveAllCalibrationToEEprom = TRUE;
            CopyCalibrationDataToMB();
        }


       else if((strcmp(cCommand, "switch in") == 0)&& g_bDiagnosticsPasswordInUse)
        {
            ANALOG_SWITCH_ON();
            iprintf("\n load cell voltage switched in\n");
        }
       else if((strcmp(cCommand, "switch out") == 0)&& g_bDiagnosticsPasswordInUse)
        {
            ANALOG_SWITCH_OFF();
            iprintf("\n load cell voltage switched out\n");
        }

       else if(strcmp(cCommand, "ref test") == 0)
       {
           RefAToDStabilityCheck();  // initiate ref a/d check
       }
       else if(strcmp(cCommand, "lc test") == 0)
       {
           LoadCellAToDStabilityCheck();  // initate load cell stability check
       }

        else if(strcmp(cCommand, "exit") == 0)
        {
            TerminateTelnetSession();
        }
        else if(strcmp(cCommand, "q") == 0)
        {
            TerminateContinuousUpdate();
        }
        else if((strcmp(cCommand, "") == 0))
        {
            if((g_nFileSegmentNo !=0) && g_bTSMTelnetInSession)
            {
                ShowFileSegment(g_cTelnetFileName,FALSE);
            }
        }
        else if((strcmp(cCommand, "a") == 0))
        {
            if((g_nFileSegmentNo !=0) && g_bTSMTelnetInSession)
            {
                ShowFileSegment(g_cTelnetFileName,TRUE);
            }
        }
        else if(strncmp(cCommand, "show ", strlen("show ") ) == 0) // 15
         {
           char * pch;
           int n1= strlen("show ");
           pch = &cCommand[n1];
           strncpy (g_cFileName, pch, (strlen(cCommand)-n1) );
           g_cFileName[strlen(cCommand)-n1] = '\0';
           //        iprintf("\n............ %s %s n1= %d \n", cCommand, cValue, n1 );
           //iprintf("\nComponent no = %d", nTest);
           g_bShowSpecificFileinTelnet = TRUE; //
         }
        else
        {
              if(fdTelnet > 0)
              iprintf("\n Invalid command\n");
        }
    }
        if(g_nFileSegmentNo == 0)
        {
            ShowPrompt();
        }
}


/////////////////////////////////////////////////////
// ShowFileSegment()
// show file segment by segment
// "space" moves it onto the next segment and "a" shows
// the entire file
//
//////////////////////////////////////////////////////

void ShowFileSegment( char *cFilename, BOOL bShowAll )
{
    #define READ_BUFSIZE 400

   char cBuffer[READ_BUFSIZE];
   if(SDCardPresent())
   {
   unsigned int n,i;
       n = 0;
	   g_bSoftwareUpdateFromSDInProgress = TRUE;
       g_bTelnetSDCardAccessInProgress = TRUE;
       g_bSPIBusBusy = TRUE;
       InitialiseSDCard();
       F_FILE * fp;
       fp=f_open(cFilename,"r");
       i = 0;
       g_nFileSegmentNo++;
       if(fp)
       {
           if(!bShowAll)
           {
           while ( !f_eof( fp ) && (i<g_nFileSegmentNo) )
           {
               SPI_Select_SD();
               n = f_read( cBuffer, 1, READ_BUFSIZE-1, fp );
               if(f_eof( fp ))
               {
               g_nFileSegmentNo = 0;
                   // end of file
               }
               i++;
           }
           f_close(fp);
           cBuffer[n] = '\0';
           if(fdTelnet > 0)
           iprintf("%s",cBuffer);
           }
           else
           {
            while ( !f_eof( fp ))
           {
               SPI_Select_SD();
               n = f_read( cBuffer, 1, READ_BUFSIZE-1, fp );
               cBuffer[n] = '\0';
               if(fdTelnet > 0)
               iprintf("%s",cBuffer);
           }
           g_nFileSegmentNo = 0;   // terminate any more file segment showing
           ShowPrompt();
           }
       }
       else
       {
           iprintf("\n file cannot be opened");
       }
       f_delvolume(MMC_DRV_NUM);
       SetSPIEX1_Lo();   //  EX1 and EX2 must be both Hi for SD chip select.
       SetSPIEX2_Lo();   //  so this will de-activate SD chip selection.
       ReInitialisePinsUsedBySDCard();
       g_bSPIBusBusy = FALSE;
      g_bTelnetSDCardAccessInProgress = FALSE;
	   g_bSoftwareUpdateFromSDInProgress = FALSE;
   }
   else
   {
       if(fdTelnet > 0)
       iprintf("Please insert SD card");
   }

}



//////////////////////////////////////////////////////
// ReadSDCardDirectory( void )
// reads sd card directory and prints to memory.
//
//////////////////////////////////////////////////////
//

void ReadSDCardDirectoryForTelnet( void )
{

   if(SDCardPresent())
   {
       F_FIND finder;
       unsigned int i,rc;
       g_bSPIBusBusy = TRUE;
       InitialiseSDCard();         // initialise sd card
       rc=f_findfirst("*.*",&finder);
       i = 0;
       if (rc==0)
       g_nNoOfFilesOnSdCard++;        // increment file counter
       {
           while (!f_findnext(&finder) && i < MAX_SD_CARD_FILES)
           {
               g_nNoOfFilesOnSdCard++;        // increment file counter
               g_nSdCardFileDetails[i] = finder;                // copy file details
               i++;
            }
       }
       g_nNoOfFilesOnSdCard = i;
       g_bSPIBusBusy = FALSE;
       f_delvolume(MMC_DRV_NUM);
       ReInitialisePinsUsedBySDCard();
       for(i=0; i< g_nNoOfFilesOnSdCard; i++)
       {
            if(fdTelnet > 0)
            iprintf("\n %s \t%d bytes",g_nSdCardFileDetails[i].filename,g_nSdCardFileDetails[i].filesize);
       }
        if(fdTelnet > 0)
        iprintf("\n");
   }
   else
   {
       if(fdTelnet > 0)
       iprintf("sd card not inserted");
   }
}

//////////////////////////////////////////////////////
// ShowHelpMenu( void )
// show help menu
//
//////////////////////////////////////////////////////
//

void ShowHelpMenu( void )
{
    if( fdTelnet > 0)
    {
        iprintf("\n                     help     show list of commands");
        iprintf("\n                self test     diagnostic blender selftest");
        iprintf("\n        show test results     shows info,config & comms");
        iprintf("\n               show comms     show communications diagnostics");
        iprintf("\n             batch log on     enable batch log to Telnet");
        iprintf("\n            batch log off     disables batch log to Telnet");
        iprintf("\n                     exit     Terminate Telnet session");
        iprintf("\n              clear comms     Clear comms counters ");
        iprintf("\n                        q     Quit continuous update ");
        iprintf("\n                    space     move to next portion of file");
        iprintf("\n                    login     log onto Telnet ");
        iprintf("\n                show info     show current network settings");
        iprintf("\n        show blender info     show blender information page");
        iprintf("\n           show event log     show event log");
        iprintf("\n           show batch log     shows current batch log file");
        iprintf("\n              show alarms     show alarm log");
        iprintf("\n          show hopper cal     show hopper calibration page");
        iprintf("\n              show config     show current config file");
        iprintf("\n          debug modbus on     enable modbus write messages");
        iprintf("\n         debug modbus off     disable modbus write messages");
        iprintf("\n            ongoing comms     ongoing comms");
        iprintf("\n            comms history     comms history ");
        iprintf("\n           ongoing events     ongoing events ");
        iprintf("\n           events history     events history ");
        iprintf("\n         component events     component events");
        iprintf("\n        component history     component history");
        iprintf("\n          weighing events     weighing events");
        iprintf("\n         weighing history     weighing history");

        iprintf("\n\n");
    }
}


//////////////////////////////////////////////////////
// ShowHelpMenu( void )
// show user prompt
//
//////////////////////////////////////////////////////
//
void ShowUserNamePrompt( void )
{
    g_nTelnetSequence = TELNET_USERNAME_ENTRY;
    iprintf("\nSoftware Revision: %s \nUsername: ", RevisionNumber);
    g_bTelnetUserNameOkay = FALSE;
    g_nTelnetContinuousUpdate = 0; // ensure no continuous update
}

//////////////////////////////////////////////////////
// ShowCurrentLog( void )
// show the current batch log
//
//////////////////////////////////////////////////////
//
void ShowCurrentLog( void )
{
    char    cName[20];
    sprintf( cName, "lg%02d%02d%02d",g_CurrentTime[TIME_YEAR] - TIME_CENTURY,g_CurrentTime[TIME_MONTH],g_CurrentTime[TIME_DATE]);     // Counts per int. stage 1.
    strcat( cName, ".txt");
    ShowFileInSegments(cName);
}

//////////////////////////////////////////////////////
// Showinfo( void )
// show ip settings
//
//////////////////////////////////////////////////////
//
void Showinfo( void )
{
    union CharsAndLong   dValue;
    char    cTempBuffer[500];
    char    cBuffer[1000];
    char    *cTempText;
    int i;

    dValue.lValue = EthernetIP;
    cTempBuffer[0] = '\0';
    cBuffer[0] = '\0';
    // ip address

    sprintf(cTempBuffer,"\n********************info page********************\n");
    strcat(cBuffer,cTempBuffer);
    if(g_bManualAutoFlag == MANUAL)
    {
        sprintf( cTempBuffer, strManual[0] );
    }
    else
    {
        sprintf( cTempBuffer, strAutomatic[0] );
    }
    strcat(cBuffer,cTempBuffer);
    if(g_nPauseFlag != 0)
    {
        sprintf( cTempBuffer, " || ");   //
        strcat(cBuffer,cTempBuffer);
    }

// nbb serial no

       for(i=0; i < NBB_SERIAL_NUMBER_SIZE-1; i++ )
       {
           cTempBuffer[i] = g_sDescriptionData.m_cNBBSerialNo[i];        // M.s.Byte.
       }
       cTempBuffer[NBB_SERIAL_NUMBER_SIZE-1] = '\0';
       strcat( cBuffer, cTempBuffer );

// sei software revision no
    sprintf(cTempBuffer,"\nSEI Software Revision No : %s",g_cSEISoftwareVersionNumber);
    strcat(cBuffer,cTempBuffer);




    sprintf(cTempBuffer,"\nBlender Software Revision : %s",RevisionNumber);
    strcat(cBuffer,cTempBuffer);
    sprintf(cTempBuffer,"\nPanel Software Revision   : %s",g_cPanelVersionNumber);
    strcat(cBuffer,cTempBuffer);

    sprintf(cTempBuffer,"\nPower up %02d/%02d/%02d    %02d:%02d",g_PowerUpTime[TIME_DATE],g_PowerUpTime[TIME_MONTH],g_PowerUpTime[TIME_YEAR],g_PowerUpTime[TIME_HOUR],g_PowerUpTime[TIME_MINUTE]);
    strcat(cBuffer,cTempBuffer);
    sprintf(cTempBuffer,"\nLicense ID %d",g_OnBoardCalData.m_lSoftwareID);
    strcat(cBuffer,cTempBuffer);
    cTempText = GetBlenderTypeText(g_CalibrationData.m_nBlenderType);
    sprintf(cTempBuffer,"\nBlender type : %s",cTempText);
    strcat(cBuffer,cTempBuffer);
    sprintf( cTempBuffer,"\nResets %d ", g_ProcessData.m_nResetCounter );    //
    strcat(cBuffer,cTempBuffer);
    sprintf( cTempBuffer,"\nBatches %d ", g_nCycleCounter );    //
    strcat(cBuffer,cTempBuffer);
    sprintf( cTempBuffer, "\nBlender Address %d ", g_cBlenderNetworkAddress );    //
    strcat(cBuffer,cTempBuffer);
    sprintf(cTempBuffer,"\nHardware ID  : %2x  %2x  %2x  %2x  %2x  %2x  %2x  %2x",DS2432Buffer[0],DS2432Buffer[1],DS2432Buffer[2],DS2432Buffer[3],DS2432Buffer[4],DS2432Buffer[5],DS2432Buffer[6],DS2432Buffer[7]);
    strcat(cBuffer,cTempBuffer);
    sprintf(cTempBuffer,"\nMAC   : %2x  %2x  %2x  %2x  %2x  %2x",gConfigRec.mac_address[0],gConfigRec.mac_address[1],gConfigRec.mac_address[2],gConfigRec.mac_address[3],gConfigRec.mac_address[4],gConfigRec.mac_address[5]);
    strcat(cBuffer,cTempBuffer);
    sprintf(cTempBuffer,"\nIP     : %3d. %3d. %3d. %3d",dValue.cValue[0],dValue.cValue[1],dValue.cValue[2],dValue.cValue[3]);
    strcat(cBuffer,cTempBuffer);
    dValue.lValue = EthernetIpMask;
    sprintf(cTempBuffer,"\nSubnet : %3d. %3d. %3d. %3d",dValue.cValue[0],dValue.cValue[1],dValue.cValue[2],dValue.cValue[3]);
    strcat(cBuffer,cTempBuffer);
    dValue.lValue = EthernetIpGate;
    sprintf(cTempBuffer,"\nGateway: %3d. %3d. %3d. %3d",dValue.cValue[0],dValue.cValue[1],dValue.cValue[2],dValue.cValue[3]);
    strcat(cBuffer,cTempBuffer);
    dValue.lValue = EthernetDNS;
    sprintf(cTempBuffer,"\nDNS    : %3d. %3d. %3d. %3d",dValue.cValue[0],dValue.cValue[1],dValue.cValue[2],dValue.cValue[3]);
    strcat(cBuffer,cTempBuffer);

    // network
    sprintf(cTempBuffer,"\nNetwork Comms ");
    strcat(cBuffer,cTempBuffer);
    if(g_nBatchCommsStatusNetwork == BATCH_COMMS_PROTOCOL_UNUSED)
    {
       sprintf(cTempBuffer,"off ");
    }
    else if(g_nBatchCommsStatusNetwork == BATCH_COMMS_PROTOCOL_COMMS_OKAY)
    {
       sprintf(cTempBuffer,"ok");
    }
    else
    {
       sprintf(cTempBuffer,"error");
    }
    strcat(cBuffer,cTempBuffer);

    // tcpip
    sprintf(cTempBuffer,"\nTCPIP Comms ");
    strcat(cBuffer,cTempBuffer);
    if(g_nBatchCommsStatusTCPIP == BATCH_COMMS_PROTOCOL_UNUSED)
    {
       sprintf(cTempBuffer,"off ");
    }
    else if(g_nBatchCommsStatusTCPIP == BATCH_COMMS_PROTOCOL_COMMS_OKAY)
    {
       sprintf(cTempBuffer,"ok");
    }
    else
    {
       sprintf(cTempBuffer,"error");
    }
    strcat(cBuffer,cTempBuffer);

   // sei
    sprintf(cTempBuffer,"\nSEI Comms ");
    strcat(cBuffer,cTempBuffer);
    if(g_nBatchCommsStatusSEI == BATCH_COMMS_PROTOCOL_UNUSED)
    {
       sprintf(cTempBuffer,"off ");
    }
    else if(g_nBatchCommsStatusSEI == BATCH_COMMS_PROTOCOL_COMMS_OKAY)
    {
       sprintf(cTempBuffer,"ok");
    }
    else
    {
       sprintf(cTempBuffer,"error");
    }
    strcat(cBuffer,cTempBuffer);

    sprintf(cTempBuffer,"\n");
    strcat(cBuffer,cTempBuffer);

    iprintf("%s",cBuffer);
}


//////////////////////////////////////////////////////
// ShowAlarms( void )
// show the current alarms
//
//////////////////////////////////////////////////////
//
void ShowAlarms( void )
{
    int i;
    char    *cAlarmText;
    BOOL    bNoAlarms = TRUE;
    if(fdTelnet > 0)
    iprintf("\n alarm list");
    for( i=0; i<MB_ALARM_BLOCKS; i++ )      //copy all blocks.
    {
       if(g_MainAlarmTable.m_AlarmArray[i].m_ucAlarmCode !=0)
       {
       if(fdTelnet > 0)
       iprintf("\n");
       bNoAlarms = FALSE;
       cAlarmText = GetAlarmText(g_MainAlarmTable.m_AlarmArray[i].m_ucAlarmCode);
       if(fdTelnet > 0)
       iprintf("%s\t ",cAlarmText);
        // occurred / cleared
        if(g_MainAlarmTable.m_AlarmArray[i].m_bAlarmState)
        {
             if(fdTelnet > 0)
             iprintf(" occurred\t");
        }
        else
        {
             if(fdTelnet > 0)
             iprintf(" cleared\t");
        }

        if(fdTelnet > 0)
        {
        iprintf("c %d\t ",g_MainAlarmTable.m_AlarmArray[i].m_nComponent);
        iprintf("%02d/%02d\t ",g_MainAlarmTable.m_AlarmArray[i].m_cDate,g_MainAlarmTable.m_AlarmArray[i].m_cMonth);
        // hours:
        iprintf(" %02d:%02d\t ",g_MainAlarmTable.m_AlarmArray[i].m_cHour,g_MainAlarmTable.m_AlarmArray[i].m_cMinute);
        }
        }
    }
    if(bNoAlarms)
    {
        if(fdTelnet > 0)
        iprintf("\n no alarms in list");
    }
    if(fdTelnet > 0)
    iprintf("\n");
}

//////////////////////////////////////////////////////
// ShowAlarms( void )
// show blender info page
//
//////////////////////////////////////////////////////

void ShowBlenderInfoPage( void )
{
    unsigned int i;
    char    cBuffer[MAX_PRINT_WIDTH+1];
    char    cPrinterBuffer[1000];
    cBuffer[0] = '\0';
    cPrinterBuffer[0] = '\0';
    sprintf( cBuffer, "\n ******blender information*****\n");
    strcat( cPrinterBuffer, cBuffer );
    if(g_bManualAutoFlag == MANUAL)
    {
        sprintf( cBuffer, strManual[0] );
    }
    else
    {
        sprintf( cBuffer, strAutomatic[0] );
    }
    strcat( cPrinterBuffer, cBuffer);

    if(g_nPauseFlag != 0)
    {
        sprintf( cBuffer, " || ");   //
        strcat( cPrinterBuffer, cBuffer );
    }
    sprintf( cBuffer, "         %2d deg C    ",g_nTemperature);   //
    strcat( cPrinterBuffer, cBuffer );
    if(g_bLevSen1Stat == COVERED)
    {
        sprintf( cBuffer, "sensor covered" );
    }
    else
    {
        sprintf( cBuffer, "sensor uncovered" );
    }
    strcat( cPrinterBuffer, cBuffer );

    sprintf(cBuffer,"\n       set %%    act %%  set kg  Act kg  ");
    strcat( cPrinterBuffer, cBuffer);
    for(i=0; i<g_CalibrationData.m_nComponents; i++)
    {
        sprintf(cBuffer,"\n    %d   %3.2f   %3.2f   %7.1f  %7.1f",i+1,g_CurrentRecipe.m_fPercentage[i],g_fComponentActualPercentage[i],(1000.0 * g_fComponentTargetWeight[i]),(1000.0 * g_fComponentActualWeight[i]));
        strcat( cPrinterBuffer, cBuffer);
    }
    sprintf(cBuffer,"\n");
    strcat( cPrinterBuffer, cBuffer);

    // kg/h

    sprintf( cBuffer, "\nkg/hr Set %4.3f Act %4.3f ", g_CurrentRecipe.m_fTotalThroughput,g_fRRThroughputKgPerHour);
    strcat( cPrinterBuffer, cBuffer);


     // g/m
    sprintf( cBuffer, "\ng/m   Set %4.3f Act %4.3f ", g_CurrentRecipe.m_fTotalWeightLength,g_fWeightPerMeterRR);
    strcat( cPrinterBuffer, cBuffer);

    // line speed
    sprintf( cBuffer, "\nLine speed (m/min)    %3.2f", g_fLineSpeed5sec );  // line speed
    strcat( cPrinterBuffer, cBuffer);

    //d/a set point
    sprintf( cBuffer, " Control %3.2f", g_fExtruderDAValuePercentage );  // d/a sp       strcat( g_cPrinterBuffer, cBuffer);
    strcat( cPrinterBuffer, cBuffer);

    // d/a per kg
    sprintf( cBuffer, "\nd/a per kg %3.2f ", g_ProcessData.m_fDtoAPerKilo);
    strcat( cPrinterBuffer, cBuffer);

    sprintf(cBuffer,"\n");
    strcat( cPrinterBuffer, cBuffer);

    if(fdTelnet > 0)
    {
        iprintf("%s",cPrinterBuffer);
    }
//    if(fdTelnet > 0)
//    {
//        iprintf("string length is %d",strlen(cPrinterBuffer)); // last string length was 392
//    }
    g_nTelnetContinuousUpdate = BLENDERINFORMATIONPAGE;
}


//////////////////////////////////////////////////////
// NetworkWritesOn( void )
// enable monitoring of network modbus writes
//
//////////////////////////////////////////////////////

void NetworkWritesOn( void )
{
    g_bShowNetworkWrites = TRUE;
     if(fdTelnet > 0)
     iprintf("\n network debug write enabled\n");
}

//////////////////////////////////////////////////////
// NetworkWritesOn( void )
// enable monitoring of network modbus writes
//
//////////////////////////////////////////////////////

void LoaderDebugOn( void )
{
     if(fdTelnet > 0)
     iprintf("\n loader debug on\n");
     g_nTSMDebug.m_bLoaderDebug = TRUE;
}

//////////////////////////////////////////////////////
// LoaderDebugOff( void )
//
//
//////////////////////////////////////////////////////

void LoaderDebugOff( void )
{
     if(fdTelnet > 0)
     iprintf("\n loader debug off\n");
     g_nTSMDebug.m_bLoaderDebug = FALSE;

}



//////////////////////////////////////////////////////
// NetworkWritesOff( void )
// disable monitoring of network modbus writes
//
//////////////////////////////////////////////////////

void NetworkWritesOff( void )
{
    g_bShowNetworkWrites = FALSE;
     if(fdTelnet > 0)
     iprintf("\n network debug write disabled\n");
}


//////////////////////////////////////////////////////
// ShowHopperCalibrationPage( void )
// show hopper cal page and set continuous update to show weight and
// a/d counts
//
//////////////////////////////////////////////////////

void ShowHopperCalibrationPage( void )
{
    char    cBuffer[MAX_PRINT_WIDTH+1];
    char    cPrinterBuffer[600];
    cBuffer[0] = '\0';
    cPrinterBuffer[0] = '\0';
    sprintf( cBuffer, "\n\n ******Weight Calibration Page*****\n");
    strcat( cPrinterBuffer, cBuffer );
    sprintf( cBuffer, "\n a/d tare counts  %ld",g_CalibrationData.m_lTareCounts);
    strcat( cPrinterBuffer, cBuffer );
    sprintf( cBuffer, "\n weight (kgs)     %2.3f",g_fOneSecondHopperWeight);
    strcat( cPrinterBuffer, cBuffer );
    sprintf( cBuffer, "\n weight constant  %6.0f",g_CalibrationData.m_fWeightConstant);
    strcat( cPrinterBuffer, cBuffer );
    sprintf( cBuffer, "\n batch size       %2.1f",g_CalibrationData.m_fBatchSize);
    strcat( cPrinterBuffer, cBuffer );
    sprintf( cBuffer, "\n weight averaging    %d",g_CalibrationData.m_nWeightAvgFactor);
    strcat( cPrinterBuffer, cBuffer );
    sprintf( cBuffer, "\n double dump         ");
    strcat( cPrinterBuffer, cBuffer );
    if(g_CalibrationData.m_bDoubleDump)
    {
        sprintf( cBuffer, "enabled");
    }
    else
    {
        sprintf( cBuffer, "disabled");
    }

    strcat( cPrinterBuffer, cBuffer );
    sprintf( cBuffer, "\n");
    strcat( cPrinterBuffer, cBuffer );
    if(fdTelnet > 0)
    {
        iprintf("%s",cPrinterBuffer);
    }
//    if(fdTelnet > 0)
//    {
//        iprintf("\n\n string length is %d\n",strlen(cPrinterBuffer));
//    }
    g_nTelnetContinuousUpdate = HOPPERCALIBRATIONPAGE;
}

//////////////////////////////////////////////////////
// CheckForTelNetTimeout( void )
// checks if telnet has timed out, if it has end the session.
//
//////////////////////////////////////////////////////
//
void CheckForTelNetTimeout( void )
{
    if(g_bTSMTelnetInSession)
    {
        g_nTelNetTimeoutCtr++;
        if(g_nTelNetTimeoutCtr >= TELNETTIMEOUTINMINS)
        {
            g_nTelNetTimeoutCtr = 0;
            if((g_nTelnetContinuousUpdate == 0) &&(!g_bPrintCycleDataToTelnet)) // no termination if continuous update.
            {
                TerminateTelnetSession();
            }
        }
    }
}


//////////////////////////////////////////////////////
// TerminateTelnetSession( void )
// terminates telnet session
//
//////////////////////////////////////////////////////
//
void TerminateTelnetSession( void )
{
    char    cBuffer[200];
    char    tBuffer[200];
    cBuffer[0] = '\0';
    sprintf(tBuffer,"\nTSM Telnet Session Terminated ");
    strcat( cBuffer, tBuffer );
    sprintf( tBuffer, "%02d/%02d/%04d\t%02d:%02d:%02d\t\n",g_CurrentTime[TIME_DATE],g_CurrentTime[TIME_MONTH],g_CurrentTime[TIME_YEAR], g_CurrentTime[TIME_HOUR], g_CurrentTime[TIME_MINUTE], g_CurrentTime[TIME_SECOND] );
    strcat( cBuffer, tBuffer );
    if(fdTelnet > 0)
    iprintf("%s",cBuffer);

    g_bTSMTelnetInSession = FALSE;
    ShowUserNamePrompt();
    g_nTelnetContinuousUpdate = 0;
    g_nFileSegmentNo = 0;   // stop segment update
    g_bPrintCycleDataToTelnet = FALSE;
    memset( &g_nTSMDebug, 0, sizeof( StructDebug ) );

}

//////////////////////////////////////////////////////
// ContinuousTelnetUpdate( void )
// continuous check
//
//////////////////////////////////////////////////////
//
void ContinuousTelnetUpdate( void )
{
    if(g_bTSMTelnetInSession)  // is the session in progress
    {
        if(g_nTelnetContinuousUpdate == HOPPERCALIBRATIONPAGE)
        {
            UpdateHopperCalibration();
        }
        else
        if(g_nTelnetContinuousUpdate == COMMUNICATIONSCOUNTERS)
        {
            ShowCommunications();
        }
        else
        if(g_nTelnetContinuousUpdate == BLENDERINFORMATIONPAGE)
        {
            ShowBlenderInfoPage();
        }
        if(g_nTelnetContinuousUpdate == ONGOINGCOMMUNICATIONSHISTORY)
        {
            ShowOngoingCommsHistory();
        }
        if(g_nTelnetContinuousUpdate == HOURLYCOMMUNICATIONSHISTORY)
        {
            ShowHourlyCommsHistory();
        }
        if(g_nTelnetContinuousUpdate == ONGOINGEVENTSHISTORY)
        {
            ShowOngoingEventsHistory();
        }
        if(g_nTelnetContinuousUpdate == ONGOINGHOURLYEVENTSHISTORY)
        {
            ShowHourlyEventsHistory();
        }

        if(g_nTelnetContinuousUpdate == ONGOINGCOMPONENTEVENTSHISTORY)
        {
            ShowOngoingComponentEventsHistory();
        }
        if(g_nTelnetContinuousUpdate == HOURLYCOMPONENTEVENTSHISTORY)
        {
            ShowHourlyComponentEventsHistory();
        }
        if(g_nTelnetContinuousUpdate == ONGOINGWEIGHINGEVENTSHISTORY)
        {
            ShowOngoingWeighingevents();
        }
        if(g_nTelnetContinuousUpdate == HOURLYWEIGHINGEVENTSHISTORY)
        {
            ShowHourlyWeighingevents();
        }

    }
}

//////////////////////////////////////////////////////
// UpdateHopperCalibration( void )
// continuous update for hopper cal
//
//////////////////////////////////////////////////////
//
void UpdateHopperCalibration( void )
{
    char    cBuffer[MAX_PRINT_WIDTH+1];
    char    cPrinterBuffer[1000];
    strcat( cPrinterBuffer, strCR );
    sprintf( cBuffer, " weight kgs  %2.3f   a/d counts       %ld",g_fOneSecondHopperWeight,g_lOneSecondWeightAverage);
    strcat( cPrinterBuffer, cBuffer );
    strcat( cPrinterBuffer, strCR );
    if(fdTelnet > 0)
    iprintf("%s",cPrinterBuffer);
}


//////////////////////////////////////////////////////
// TerminateContinuousUpdate( void )
// stop continuous update
//
//////////////////////////////////////////////////////
//
void TerminateContinuousUpdate( void )
{
    g_nTelnetContinuousUpdate = 0;  // disable continuous update
    g_nFileSegmentNo = 0;   // stop segment update
}

//////////////////////////////////////////////////////
// ShowCommunications( void )
// show communications counters
//
//////////////////////////////////////////////////////
//
void ShowCommunications( void )
{
    char    cBuffer[MAX_PRINT_WIDTH+1];
    char    cPrinterBuffer[1000];
    cBuffer[0] = '\0';
    cPrinterBuffer[0] = '\0';
    sprintf( cBuffer, "\n ******Communications*****\n");
    strcat( cPrinterBuffer, cBuffer );

    sprintf( cBuffer, "\n blender address is %d",g_cBlenderNetworkAddress);
    strcat( cPrinterBuffer, cBuffer );

// network
    sprintf( cBuffer, "\n network     Tx %ld     Rx %ld  Timeouts %ld  Chksum Errors %ld",g_sModbusNetworkSlave.m_lGoodTx,g_sModbusNetworkSlave.m_lGoodRx,g_sModbusNetworkSlave.m_lRxTimeoutCounter,g_sModbusNetworkSlave.m_lChecksumErrorCounter);
    strcat( cPrinterBuffer, cBuffer );

// optimisation
    sprintf( cBuffer, "\n optim       Tx %ld     Rx %ld  Timeouts %ld  Chksum Errors %ld",structOptimisationData.m_lGoodTx,structOptimisationData.m_lGoodRx,g_sModbusNetworkSlave.m_lRxTimeoutCounter,g_sModbusNetworkSlave.m_lChecksumErrorCounter);
    strcat( cPrinterBuffer, cBuffer );

// sei
    sprintf( cBuffer, "\n sei         Tx %ld     Rx %ld  Timeouts %ld  Chksum Errors %ld",structSEIData.m_lGoodTx,structSEIData.m_lGoodRx,structSEIData.m_lRxTimeoutCounter,structSEIData.m_lChecksumErrorCounter);
    strcat( cPrinterBuffer, cBuffer );

// lls
    sprintf( cBuffer, "\n lls         Tx %ld     Rx %ld  Timeouts %ld  Chksum Errors %ld",structLLSData.m_lGoodTx,structLLSData.m_lGoodRx,g_sModbusNetworkSlave.m_lRxTimeoutCounter,g_sModbusNetworkSlave.m_lChecksumErrorCounter);
    strcat( cPrinterBuffer, cBuffer );

// tcpip
    sprintf( cBuffer, "\n tcpip        Rx %ld",g_lTCPIPCommsCounter);
    strcat( cPrinterBuffer, cBuffer );

    sprintf( cBuffer, "\n");
    strcat( cPrinterBuffer, cBuffer );
    if(fdTelnet > 0)
    {
        iprintf("%s",cPrinterBuffer);
    }

    g_nTelnetContinuousUpdate = COMMUNICATIONSCOUNTERS;
}



//////////////////////////////////////////////////////
// ShowFileInSegments( void )
// show files in segments so that they can be read
//
//////////////////////////////////////////////////////
//
void ShowFileInSegments( char *cFilename )
{
    g_nFileSegmentNo = 0;
    sprintf( g_cTelnetFileName, cFilename);
    ShowFileSegment(g_cTelnetFileName,FALSE);
}


//////////////////////////////////////////////////////
// ClearCommsCounters( void )
//
//////////////////////////////////////////////////////
//
void TelnetClearComms( void )
{
    ClearCommsCounters();   // RESETDIAGCTR - Clear Comms diagnostics counters
    g_ProcessData.m_nResetCounter = 0;
    g_bSaveAllProcessDataToEEprom = TRUE;
    if(fdTelnet > 0)
    iprintf("\n Comms counters cleared \n");

}


//////////////////////////////////////////////////////
// ShowTestResults( void )
// show info,
//////////////////////////////////////////////////////
//
void ShowTestResults( void )
{
    if(fdTelnet>0)
    {
	    iprintf("\nCustomer:_________________");
	    iprintf("\nDate    :____/____/_____");
	    iprintf("\n");
	    iprintf("\nBlender Serial No.    :_____________________");
	    iprintf("\nBox Serial No.        :	PLC-__________________");
	    iprintf("\nPSU Serial No.        :	NPS___________________");
	    iprintf("\nSEI Serial No.        :	___________-SEIM-_____");
	    iprintf("\nSEI Software Checksum : _________________");
	    iprintf("\nMaple Panel Serial No : ______-TSMP-_____");
	    iprintf("\n");
    }
	Showinfo();
    ShowFileSegment("config.txt",TRUE);
    ShowCommunications();
    g_nTelnetContinuousUpdate = 0;  // stop repeated update
    if(!SDCardPresent())
    {   if(fdTelnet > 0)
        iprintf("Insert SD card and repeat test");
    }
    ShowSecurityResults();
    ShowLicenseInfo();
}

//////////////////////////////////////////////////////
// ShowSecurityResults( void )
// show security data
//////////////////////////////////////////////////////
//
void ShowSecurityResults( void )
{
   	if(fdTelnet>0)
    iprintf("\nSEC %2d FEC %2d TEC %2d NB %5d  CL %5d ", g_nSHASecretFailCtr,g_nSHAHashOfIdFail,g_nTimeHashFail,g_nHashOfSHAIDInFlash,g_nHashOfSHAID);     //
    if(g_nHashOfSHAIDInFlash == g_nHashOfSHAID)
     {
     	if(fdTelnet>0)
         iprintf("Pass\n");     //
     }
     else
     {
     	if(fdTelnet>0)
        iprintf("Fail\n");     //
      }

}

//////////////////////////////////////////////////////
// ShowOngoingCommsHistory( void )
// show ongoing history
//
//////////////////////////////////////////////////////
//
void ShowOngoingCommsHistory( void )
{
    char    cBuffer[MAX_PRINT_WIDTH+1];
    char    cPrinterBuffer[1000];
    cBuffer[0] = '\0';
    cPrinterBuffer[0] = '\0';
    sprintf( cBuffer, "\n ******ongoing comms *****\n");
    strcat( cPrinterBuffer, cBuffer );

    sprintf( cBuffer, "\n tcpip  Rx %ld   ",g_sOngoingHistoryTCPIPComms.m_lGoodRx);
    strcat( cPrinterBuffer, cBuffer );
    sprintf( cBuffer, "\n Network  Tx %ld Rx %ld T/o %ld ChkErr %ld  txrx%% %3.2f t/o%% %3.2f  ckErr%% %3.2f  ",g_sOngoingHistoryNetworkComms.m_lGoodTx,g_sOngoingHistoryNetworkComms.m_lGoodRx,g_sOngoingHistoryNetworkComms.m_lRxTimeoutCounter,g_sOngoingHistoryNetworkComms.m_lChecksumErrorCounter,g_sOngoingHistoryNetworkComms.m_fPercentageGoodTxRx,g_sOngoingHistoryNetworkComms.m_fPercentageTimeout,g_sOngoingHistoryNetworkComms.m_fPercentageChecksumErrors);
    strcat( cPrinterBuffer, cBuffer );
    sprintf( cBuffer, "\n SEI      Tx %ld Rx %ld T/o %ld ChkErr %ld  txrx%% %3.2f t/o%% %3.2f  ckErr%% %3.2f  ",g_sOngoingHistorySEIComms.m_lGoodTx,g_sOngoingHistorySEIComms.m_lGoodRx,g_sOngoingHistorySEIComms.m_lRxTimeoutCounter,g_sOngoingHistorySEIComms.m_lChecksumErrorCounter,g_sOngoingHistorySEIComms.m_fPercentageGoodTxRx,g_sOngoingHistorySEIComms.m_fPercentageTimeout,g_sOngoingHistorySEIComms.m_fPercentageChecksumErrors);
    strcat( cPrinterBuffer, cBuffer );

    if(fdTelnet > 0)
    {
        iprintf("%s",cPrinterBuffer);
    }

    g_nTelnetContinuousUpdate = ONGOINGCOMMUNICATIONSHISTORY;
}

//////////////////////////////////////////////////////
// ShowHourlyCommsHistory( void )
// show ongoing history
//
//////////////////////////////////////////////////////
//
void ShowHourlyCommsHistory( void )
{
    char    cBuffer[MAX_PRINT_WIDTH+1];
    char    cPrinterBuffer[1000];
    cBuffer[0] = '\0';
    cPrinterBuffer[0] = '\0';
    sprintf( cBuffer, "\n ******comms history*****\n");
    strcat( cPrinterBuffer, cBuffer );


    sprintf( cBuffer, "\n tcpip  Rx %ld   ",g_sHourHistoryTCPIPComms.m_lGoodRx);
    strcat( cPrinterBuffer, cBuffer );
    sprintf( cBuffer, "\n Network  Tx %ld Rx %ld T/o %ld ChkErr %ld  txrx%% %3.2f t/o%% %3.2f  ckErr%% %3.2f  ",g_sHourHistoryNetworkComms.m_lGoodTx,g_sHourHistoryNetworkComms.m_lGoodRx,g_sHourHistoryNetworkComms.m_lRxTimeoutCounter,g_sHourHistoryNetworkComms.m_lChecksumErrorCounter,g_sHourHistoryNetworkComms.m_fPercentageGoodTxRx,g_sHourHistoryNetworkComms.m_fPercentageTimeout,g_sHourHistoryNetworkComms.m_fPercentageChecksumErrors);
    strcat( cPrinterBuffer, cBuffer );
    sprintf( cBuffer, "\n SEI      Tx %ld Rx %ld T/o %ld ChkErr %ld  txrx%% %3.2f t/o%% %3.2f  ckErr%% %3.2f  ",g_sHourHistorySEIComms.m_lGoodTx,g_sHourHistorySEIComms.m_lGoodRx,g_sHourHistorySEIComms.m_lRxTimeoutCounter,g_sHourHistorySEIComms.m_lChecksumErrorCounter,g_sHourHistorySEIComms.m_fPercentageGoodTxRx,g_sHourHistorySEIComms.m_fPercentageTimeout,g_sHourHistorySEIComms.m_fPercentageChecksumErrors);
    strcat( cPrinterBuffer, cBuffer );

    if(fdTelnet > 0)
    {
        iprintf("%s",cPrinterBuffer);
    }

    g_nTelnetContinuousUpdate = HOURLYCOMMUNICATIONSHISTORY;
}


//////////////////////////////////////////////////////
// ShowOngoingEventsHistory( void )
// show ongoing evetns history
//
//////////////////////////////////////////////////////
//
void ShowOngoingEventsHistory( void )
{
    char    cBuffer[MAX_PRINT_WIDTH+1];
    char    cPrinterBuffer[1000];
    cBuffer[0] = '\0';
    cPrinterBuffer[0] = '\0';
    sprintf( cBuffer, "\n ******ongoing events*****\n");
    strcat( cPrinterBuffer, cBuffer );

    sprintf( cBuffer, "\n Door %d motor trip %d  lsack %d lssw %d tare al %d auto min %d  ",g_sOngoingHistoryEventsLogData.m_nDoorOpenings,g_sOngoingHistoryEventsLogData.m_nMotorTrips,g_sOngoingHistoryEventsLogData.m_nLevelSensorSwitchesAcknowledged,g_sOngoingHistoryEventsLogData.m_nLevelSensorSwitchesCtr,g_sOngoingHistoryEventsLogData.m_nTareAlarms,g_nTimeInAutoMinutesCtr);
    strcat( cPrinterBuffer, cBuffer );
    if(fdTelnet > 0)
    {
        iprintf("%s",cPrinterBuffer);
    }

    g_nTelnetContinuousUpdate = ONGOINGEVENTSHISTORY;
}

//////////////////////////////////////////////////////
// ShowHourlyEventsHistory( void )
// show ongoing evetns history
//
//////////////////////////////////////////////////////
//
void ShowHourlyEventsHistory( void )
{
    char    cBuffer[MAX_PRINT_WIDTH+1];
    char    cPrinterBuffer[1000];
    cBuffer[0] = '\0';
    cPrinterBuffer[0] = '\0';
    sprintf( cBuffer, "\n ******events history*****\n");
    strcat( cPrinterBuffer, cBuffer );

    sprintf( cBuffer, "\n Door %d motor trip %d  lsack %d lssw %d tare al %d auto min %d  ",g_sHourHistoryEventsLogData.m_nDoorOpenings,g_sHourHistoryEventsLogData.m_nMotorTrips,g_sHourHistoryEventsLogData.m_nLevelSensorSwitchesAcknowledged,g_sHourHistoryEventsLogData.m_nLevelSensorSwitchesCtr,g_sHourHistoryEventsLogData.m_nTareAlarms,g_nTimeInAutoMinutesCtr);
    strcat( cPrinterBuffer, cBuffer );
    if(fdTelnet > 0)
    {
        iprintf("%s",cPrinterBuffer);
    }

    g_nTelnetContinuousUpdate = ONGOINGHOURLYEVENTSHISTORY;
}





//////////////////////////////////////////////////////
// ShowOngoingComponentEventsHistory( void )
// show ongoing evetns history
//
//////////////////////////////////////////////////////
//
void ShowOngoingComponentEventsHistory( void )
{
    char    cBuffer[MAX_PRINT_WIDTH+1];
    char    cPrinterBuffer[1000];
    cBuffer[0] = '\0';
    cPrinterBuffer[0] = '\0';
    sprintf( cBuffer, "\n ******component events*****\n");
    strcat( cPrinterBuffer, cBuffer );

    sprintf( cBuffer, "\n Activations C1 %d, C2 %d C3 %d C4 %d ",g_sOngoingHistoryComponentLogData.m_nHistoryComponentActivations[0],g_sOngoingHistoryComponentLogData.m_nHistoryComponentActivations[1],g_sOngoingHistoryComponentLogData.m_nHistoryComponentActivations[2],g_sOngoingHistoryComponentLogData.m_nHistoryComponentActivations[3]);
    strcat( cPrinterBuffer, cBuffer );
    if(fdTelnet > 0)
    {
        iprintf("%s",cPrinterBuffer);
    }

    g_nTelnetContinuousUpdate = ONGOINGCOMPONENTEVENTSHISTORY;
}

//////////////////////////////////////////////////////
// ShowHourlyComponentEventsHistory( void )
// show ongoing evetns history
//
//////////////////////////////////////////////////////
//
void ShowHourlyComponentEventsHistory( void )
{
    char    cBuffer[MAX_PRINT_WIDTH+1];
    char    cPrinterBuffer[1000];
    cBuffer[0] = '\0';
    cPrinterBuffer[0] = '\0';
    sprintf( cBuffer, "\n ******component history*****\n");
    strcat( cPrinterBuffer, cBuffer );

    sprintf( cBuffer, "\n Activations C1 %d, C2 %d C3 %d C4 %d ",g_sHourHistoryComponentLogData.m_nHistoryComponentActivations[0],g_sHourHistoryComponentLogData.m_nHistoryComponentActivations[1],g_sHourHistoryComponentLogData.m_nHistoryComponentActivations[2],g_sHourHistoryComponentLogData.m_nHistoryComponentActivations[3]);
    strcat( cPrinterBuffer, cBuffer );
    if(fdTelnet > 0)
    {
        iprintf("%s",cPrinterBuffer);
    }

    g_nTelnetContinuousUpdate = HOURLYCOMPONENTEVENTSHISTORY;
}


//////////////////////////////////////////////////////
// ShowOngoingWeighingevents( void )
// show ongoing evetns history
//
//////////////////////////////////////////////////////
//
void ShowOngoingWeighingevents( void )
{
    char    cBuffer[MAX_PRINT_WIDTH+1];
    char    cPrinterBuffer[1000];
    cBuffer[0] = '\0';
    cPrinterBuffer[0] = '\0';
    sprintf( cBuffer, "\n ******weighing events*****");
    strcat( cPrinterBuffer, cBuffer );

    sprintf( cBuffer, "\n tare high %ld tare low %ld \n ",g_sOngoingHistoryEventsLogData.m_lTareMaxValue,g_sOngoingHistoryEventsLogData.m_lTareMinValue);
    strcat( cPrinterBuffer, cBuffer );
    if(fdTelnet > 0)
    {
        iprintf("%s",cPrinterBuffer);
    }

    g_nTelnetContinuousUpdate = ONGOINGWEIGHINGEVENTSHISTORY;
}



//////////////////////////////////////////////////////
// ShowHourlyWeighingevents( void )
// show ongoing evetns history
//
//////////////////////////////////////////////////////
//
void ShowHourlyWeighingevents( void )
{
    char    cBuffer[MAX_PRINT_WIDTH+1];
    char    cPrinterBuffer[1000];
    cBuffer[0] = '\0';
    cPrinterBuffer[0] = '\0';
    sprintf( cBuffer, "\n ******weighing history*****");
    strcat( cPrinterBuffer, cBuffer );

    sprintf( cBuffer, "\n tare high %ld tare low %ld \n",g_sHourHistoryEventsLogData.m_lTareMaxValue,g_sHourHistoryEventsLogData.m_lTareMinValue);
    strcat( cPrinterBuffer, cBuffer );
    if(fdTelnet > 0)
    {
        iprintf("%s",cPrinterBuffer);
    }

    g_nTelnetContinuousUpdate = HOURLYWEIGHINGEVENTSHISTORY;
}


//////////////////////////////////////////////////////
// ShowPrompt( void )
//
//
//////////////////////////////////////////////////////
//
void ShowPrompt( void )
{
    if(fdTelnet > 0)
    iprintf(":>");
}


//////////////////////////////////////////////////////
// CheckForTelnetCommands( void )
// check in the foreground to check if a Telnet command has been
// initiated. There is a problem running this directly from the
// telnet task.
//
//////////////////////////////////////////////////////
//
void CheckForTelnetCommands( void )
{
    if(g_bReadSDCardDirectory)
    {
        g_bReadSDCardDirectory = FALSE;
       	ReadSDCardDirectoryForTelnet();
    }
    if(g_bShowConfigInTelnet)
    {
        g_bShowConfigInTelnet = FALSE;
        ShowFileInSegments("config.txt");
    }
    if(g_bShowEventLogInTelnet)
    {
    	g_bShowEventLogInTelnet = FALSE;
        ShowFileInSegments(g_cEventName);
    }
    if(g_bShowBatchLogInTelnet)
    {
    	g_bShowBatchLogInTelnet = FALSE;
    	ShowCurrentLog();
    }
    if(g_bShowFileSegmentInTelnet)
    {
    	g_bShowFileSegmentInTelnet = FALSE;
        ShowFileSegment(g_cTelnetFileName,FALSE);
    }
    if(g_bShowAllFileSegmentsInTelnet)
    {
    	g_bShowAllFileSegmentsInTelnet = FALSE;
        ShowFileSegment(g_cTelnetFileName,TRUE);
    }

    if(g_bShowFile1inTelnet)
    {
    	g_bShowFile1inTelnet = FALSE;
    	ShowFileInSegments(g_nSdCardFileDetails[0].filename);
   }
    if(g_bShowFile2inTelnet)
    {
    	g_bShowFile2inTelnet = FALSE;
    	ShowFileInSegments(g_nSdCardFileDetails[1].filename);
   }
    if(g_bShowFile3inTelnet)
    {
    	g_bShowFile3inTelnet = FALSE;
    	ShowFileInSegments(g_nSdCardFileDetails[2].filename);
   }
    if(g_bShowFile4inTelnet)
    {
    	g_bShowFile4inTelnet = FALSE;
    	ShowFileInSegments(g_nSdCardFileDetails[3].filename);
   }
    if(g_bShowFile5inTelnet)
    {
    	g_bShowFile5inTelnet = FALSE;
    	ShowFileInSegments(g_nSdCardFileDetails[4].filename);
   }
    if(g_bShowFile6inTelnet)
    {
    	g_bShowFile6inTelnet = FALSE;
    	ShowFileInSegments(g_nSdCardFileDetails[5].filename);
   }
    if(g_bShowFile7inTelnet)
    {
    	g_bShowFile7inTelnet = FALSE;
    	ShowFileInSegments(g_nSdCardFileDetails[6].filename);
   }
    if(g_bShowFile8inTelnet)
    {
    	g_bShowFile8inTelnet = FALSE;
    	ShowFileInSegments(g_nSdCardFileDetails[7].filename);
   }
    if(g_bShowFile9inTelnet)
    {
    	g_bShowFile9inTelnet = FALSE;
    	ShowFileInSegments(g_nSdCardFileDetails[8].filename);
   }
    if(g_bShowFile10inTelnet)
    {
    	g_bShowFile10inTelnet = FALSE;
    	ShowFileInSegments(g_nSdCardFileDetails[9].filename);
   }
    if(g_bShowSpecificFileinTelnet)
    {
    	g_bShowSpecificFileinTelnet = FALSE;
    	ShowFileInSegments(g_cFileName);
   }
    if(g_bShowTestResults)
    {
    	g_bShowTestResults = FALSE;
    	ShowTestResults();
    }
}

