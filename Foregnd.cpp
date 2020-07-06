///////////////////////////////////////////////////////////
// Foreground.c
//
// Executes all of the foreground routines that are not
// time critical such as the display update.
//
// Entry:   Bool indicating if time should be displayed in
//          the top right hand corner of the screen.
//
// Exit:    Void
//
// M.Parks                      20-01-2000
// First pass.
// M.Parks                      04-04-2000
// Added time display.
// M.Parks                      04-08-2000
// Added report printing.
// M.McKiernan                      06-06-2003
// Added calls to CopyCalibrationDataVAC350MB,CopyProdSummaryDataVAC350MB() and
// handle Modbus writes.
// M.McKiernan                      14-08-2003
// Read cp2000 address from SBB - g_cBlenderNetworkAddress = ReadCP2kAddress()
// M.McKiernan                      07-03-2004
// Bug fix at L. 123  Generate a back roll report, was calling for a front roll report.
// M.McKiernan                      11-05-2004
//  Call routinge to check for blender alarms. (g_nBlendersInAlarm = CheckForBlenderAlarms();)

//******************* For Batch Blender ***************************************

// M.McKiernan                              24-05-2004
// Removed stuff relating to VAC350 (See //bb )
// Removed stuff relating to reports
// Removed check for blender alarms.
//
// M.McKiernan                      25-06-2004
//  Call CycleMon() - batch cycling monitor routine.
// M.McKiernan                      23-07-2004
//  Removed the batch blender program loop.  Now call ProgramLoop().
//
// M.McKiernan                      04-11-2004
//      Call ReadSBBLinkOptions()
//
// M.McKiernan                      22-12-2004
//  Removed call to ParallelPrintHandler() - now moved to PIT.
// Diagnostic g_nForegndPassCounter added to count passes thru foreground prog per second.
//
// P.Smith                          11/11/05
// Correct  unsigned char   cBuffer[COLUMNS], was being initialised every pass
// while show flag was set.
//
// P.Smith                          11/1/06
// Correct warnings
// cBuffer set as char not unsigned
///
// P.Smith                      16/2/06
// first pass at netburner hardware conversion.
// bool -> BOOL
//#include <basictypes.h>
// comment out watch dog service & display update
//    GetTime(); implemented
//    GetDate(); implemented
///
// P.Smith                      1/6/06
// removed cBuffer definition, causing a warning in the compiler.
//
// P.Smith                      16/11/06
// removed unused comments
//
// P.Smith                      23/7/08
// removed g_CurrentTime extern
//
// P.Smith                      12/3/10
// clear SoftwareWatchDogOneSecondCtr when the foreground runs.
// if the foreground gets stuck in a loop, then the interrupt sytem will
// initiate a reboot in n seconds.
//////////////////////////////////////////////////////////

//*****************************************************************************
// INCLUDES
//*****************************************************************************
#include <stdio.h>
#include "SerialStuff.h"
#include "Foreground.h"
#include "16R40C.h"
#include "TimeDate.h"
#include "PrinterFunctions.h"
#include "InitialiseHardware.h"
#include "ConfigurationFunctions.h"
#include "MBProgs.h"
#include "BatVars.h"
#include "SetpointFormat.h"
#include "ProgLoop.h"
#include "MBMaster.h"
#include "BatchCalibrationFunctions.h"

// Locally declared global variables
BOOL    g_bSuspendSequencer = FALSE;
extern  structSetpointData  g_TempRecipe,g_CurrentRecipe;
extern CalDataStruct    g_CalibrationData;


// Externally defined global variables
extern  int     g_nKeyCode;
extern  unsigned char g_cBlenderNetworkAddress;
extern  unsigned char g_cAutoCycleSymbol;
extern  int     g_nDisplayScreen;
extern  int		SoftwareWatchDogOneSecondCtr;

// in hc16.h -  #define SIM_SWSR   (*((char *)0xffa27))    /* 8-Bit */                  // PS

int g_nMenuLevel;

int g_nBlendersInAlarm;
unsigned int g_nForegndPassCounter = 0;

void Foreground( BOOL bShowTime )
{
 int i,nCompIndex;
 unsigned char cData;
//  XREF    SIM_SWSR;
	 SoftwareWatchDogOneSecondCtr = 0;
//nbb--removed--    ServiceWatchdogTimer();     // service the watch dog timer (mmk 21-2-2001)
	 // Run the sequencer algorithm only if the comms
    // is running.

    g_cBlenderNetworkAddress = ReadCP2kAddress();   // Read cp2000 address from SBB

    ReadSBBLinkOptions();       // Read option links on SBB.
// get fill by weight status for each component.
    i = 0;
    while(g_cSeqTable[i] != SEQDELIMITER && i < MAX_COMPONENTS) // SEQDELIMITER indicates table end
    {
        nCompIndex = (int)(g_cSeqTable[i] & 0x0F) - 1;  // get component no. from table, mask off the m.s. nibble
        cData = g_cSeqTable[i];
        if(cData & FILLBYWEIGHT)
            g_bFBWStatus[nCompIndex] = TRUE;
        else
            g_bFBWStatus[nCompIndex] = FALSE;
        i++;
    }

    // The UFP has been reset so we must re-initialise
    // the LCD.
/*nbb    if( g_nKeyCode == KEY_UFP_RESET )
    {
        InitialiseDisplay( FALSE );
        RefreshDisplay();
        g_nKeyCode = KEY_INVALID_CODE;
    }
nbb*/

//bb
/*
    if( g_sModbusMaster.m_bCommsEnable && !g_bSuspendSequencer )
    {
        // Has one second elapsed ?
        if( g_nSequencerTimer >= PIT_FREQUENCY )
        {
            CheckLoaderConfiguration();     // check pump no is valid in loader data.
            // Run the algorithm
            SequencerAlgorithm();
            HandleVAC350MBWrites();         //handle any writes to VAC350 Modbus Table.
            if( g_nVAC350MBCalUpdateTimer )
            {
                CopyCalibrationDataVAC350MB();  //only copy calib. data when update timer > 0.
            }

            CopyProdSummaryDataVAC350MB();

            // reset the timer
            g_nSequencerTimer = 0;
        }
    }
*/

//bb
/*
    // Check for Report generation

    if( g_bGenerateFrontRollReport )
    {
        // Generate a front roll report
        GenerateRollReport( FALSE, g_bFrontReportStatus );

        // Finished
        g_bGenerateFrontRollReport = FALSE;
    }

    if( g_bGenerateBackRollReport )
    {
        // Generate a back roll report
        GenerateRollReport( TRUE, g_bBackReportStatus );

        // Finished
        g_bGenerateBackRollReport = FALSE;
    }

    if( g_bGenerateOrderReport )
    {
        // Generate a front roll report
        GenerateOrderReport( g_bOrderReportStatus );

        // Finished
        g_bGenerateOrderReport = FALSE;
    }

    // Check if any reports need printing
    if( !g_FrontRollReport.m_bPrinted )
        PrintRollReport( FALSE );
    if( !g_BackRollReport.m_bPrinted )
        PrintRollReport( TRUE );
    if( !g_OrderReport.m_bPrinted )
        PrintOrderReport();
*/
    // Print data if necessary
//  ParallelPrintHandler();
    // handle writes into ModBus.
    PostMBWrite();
    // Keep the time and date current, whether or
    // not it is displayed.   It's used by the alarm
    // system as a time stamp.
    GetTime();
    GetDate();
    CopyTimeToMB();         // copy into MB table.

 /*nbb   if( bShowTime )
    {
        sprintf( cBuffer, "%02d:%02d", g_CurrentTime[TIME_HOUR], g_CurrentTime[TIME_MINUTE] );
        DisplayStringDataJustified( cBuffer, 0, COLUMNS - 2, JUSTIFY_RIGHT );
        DisplayCharacterDataWithPosition( g_cAutoCycleSymbol, 0, COLUMNS - 1);
    }
nbb*/
//  if(g_nDisplayScreen != INFORMATION_PAGE)        // --REVIEW--   testpurposes only.
//nbb        UpdateDisplay();

// update attribute indices if remote operator panel present. - for master comms.
//      if(  g_bRemoteOperatorPanelPresent && !g_bCommsAttributeUpdateActive )
//          GetNextAttributeIndices();          // --

////////////// Batch Blender Foreground specific programs from here //////////////////

    ProgramLoop();      // Essentially the foreground for the batch blender.

    g_nForegndPassCounter++;    // test purposes only.
}
