// P.Smith                                                 26/6/07
// fg ten hz
//
// P.Smith                                                 17/7/07
// call CheckForFlapFullToEmptyTransition
//
// P.Smith                                                 21/10/08
// add RunStop
//
// P.Smith                                                 11/12/08
// added usb stuff mmk
//
// P.Smith                                                 21/1/09
// set the function sub section number when any program is being called.
//
//
// P.Smith                                                 25/6/09
// call VacTenHzPrograms() if no of loaders is greater than 0
//
// M.McKiernan                      1/12/09
// Removed call to #include <effs_fat/mmc_drv.h> - mmc_drv.h not present in eclipse version
// Added #include "predef.h"				//Eclipse
//
// P.Smith                                                 6/4/10
// call SaveFuntionNosInRAM to save g_nFunctionSubSectionRunning
//////////////////////////////////////////////////////

//*****************************************************************************
// INCLUDES
//*****************************************************************************
#include "predef.h"				//Eclipse
#include <Stdio.h>

#include "InitialiseHardware.h"
#include "SerialStuff.h"
#include "General.h"
#include "ConversionFactors.h"

#include "BatchCalibrationFunctions.h"
#include "BatchMBIndices.h"
#include "ConfigurationFunctions.h"
#include "Foreground.h"
#include "16R40C.h"
#include    "FiftyHz.h"
#include    "BatVars.h"
#include    "BatVars2.h"
#include    "Pause.h"
#include    "MBProSum.h"
#include    "MBProgs.h"
#include "Cleanio.h"
#include    "TenHz.h"
#include    "FGOneHz.h"
#include "SetpointFormat.h"
#include    "MBCyDiag.h"
#include "Alarms.h"
#include "PrntDiag.h"
#include "Exstall.h"
#include "Conalg.h"
#include "TimeDate.h"
#include "TSMPeripheralsMBIndices.h"
#include "License.h"
#include <basictypes.h>
#include <string.h>
#include "I2CFuncs.h"
#include "TimeDate.h"
#include "Lspeed.h"
#include "Multiblend.h"
#include "Expansion.h"
#include "CPIRRobin.h"
#include "OptimisationMB.h"
#include "Optimisation.h"
#include "VNC1L_g.h"


#include <effs_fat/fat.h>
#include <effs_fat/mmc_mcf.h>
#include <effs_fat/cfc_mcf.h>
//#include <effs_fat/mmc_drv.h>
#include "sdcard.h"
#include "Setpointformat.h"
#include "Loadcaldatafromsdcard.h"
#include "MBProgs.h"
#include "iniparser.h"
#include "iniexample.h"
#include    "Alarms.h"
#include "USBDisk.h"
#include "VacSeq.h"
#include "vacvars.h"
#include "security.h"


int g_FG10HzLoopCounter = 0;
int g_nTenHzCount = 0;
extern structUSB   g_sUSB;
extern CalDataStruct    g_CalibrationData;
extern  char        g_cPrinterBuffer[];
extern  char        g_cEventLogFileCopy[];

extern   char XfrBuf[];
extern   int  g_nXfrBytes;
extern   bool g_bXfrBufBusy;
extern   bool g_bSDCardAccessInProgress;

extern   bool g_bXfrFinished;
extern   bool g_bFileXfrInProgress;
extern   bool g_bXfrFileReadFinished;
extern   int  g_nSegmentNo;
extern   char g_cXfrFilename[];
extern   long g_lXfrFileSize;
extern   structVacCalib   g_VacCalibrationData;


//////////////////////////////////////////////////////
void ForegroundTenHzProgram( void )
{
  bool bStarted = FALSE;
   g_nTenHzCount++;
   if(g_VacCalibrationData.m_nLoaders > 0 )
   VacTenHzPrograms();

   g_FG10HzLoopCounter++;
   if(g_FG10HzLoopCounter >= 10)
   {
      g_FG10HzLoopCounter = 0;         //reset counter.
   }

    g_nFunctionSubSectionRunning = FUNCTION_SUB_SECTION_1;
    SaveFuntionNosInRAM();
    CopyProdSummDataToMB();          //  Copy production summary to modbus table.
    SetOptimisationLoaderFillOutputStatus();
    SetOptimisationLoaderStatus();
    SendOuputsToOptimisationCard();
    CheckForFlapFullToEmptyTransition();
    RunStop();

// To cater for USB call from here to...XX1
   if(g_CalibrationData.m_nDumpDeviceType != 0)
   {
    g_nFunctionSubSectionRunning = FUNCTION_SUB_SECTION_2;
    SaveFuntionNosInRAM();
    MonitorPrinterPort10Hz();


   if( g_CalibrationData.m_nDumpDeviceType == DUMP_DEVICE_USB )  // use 1 for USB memory stick.      if( 1 )     // tthis works.
   {
     g_nFunctionSubSectionRunning = FUNCTION_SUB_SECTION_3;
     SaveFuntionNosInRAM();
     HandleUSBFlashWriteJob();  // checks if a USB flash write is in progress and terminates accordingly.

      // need to start diagnostics write to USB??
       if(!g_sUSB.m_bFlashWriteInProgress && g_sUSB.m_bUSBDriveReady && g_sUSB.m_bStartDiagWrite)
       {
            bStarted = StartUSBFlashWrite( g_cPrinterBuffer, g_sUSB.m_cDiagFileName, strlen(g_cPrinterBuffer) );
            if( bStarted )     // did start write succeed.
               g_sUSB.m_bStartDiagWrite = FALSE;
       }
      // need to start Event log write to USB??
       if(!g_sUSB.m_bFlashWriteInProgress && g_sUSB.m_bUSBDriveReady && g_sUSB.m_bStartEvLogWrite )
       {
           bStarted = StartUSBFlashWrite( g_cEventLogFileCopy, g_sUSB.m_cEvLogFileName, strlen(g_cEventLogFileCopy) );
           if( bStarted )     // did start write succeed.
           g_sUSB.m_bStartEvLogWrite = FALSE;
      }

       // need to start Order report write to USB??
       if(!g_sUSB.m_bFlashWriteInProgress && g_sUSB.m_bUSBDriveReady && g_sUSB.m_bStartOrderReportLogWrite )
       {
            bStarted = StartUSBFlashWrite( g_cOrderReportStorageBuffer, g_sUSB.m_cOrderReportFileName, strlen(g_cOrderReportStorageBuffer) );
            if( bStarted )     // did start write succeed.
               g_sUSB.m_bStartOrderReportLogWrite = FALSE;
       }


     g_nFunctionSubSectionRunning = FUNCTION_SUB_SECTION_4;
     SaveFuntionNosInRAM();
     HandleFileTransferFromSD();     //takes care of file transfer on SD card side.

       // is there a need to start a transfer to USB??
       if(!g_sUSB.m_bFlashWriteInProgress && g_sUSB.m_bUSBDriveReady && g_sUSB.m_bStartXfrWrite)
       {
            bStarted = StartUSBFlashWrite( XfrBuf, g_sUSB.m_cXfrFileName, g_sUSB.m_nXfrBytes  );
            if( bStarted )     // did start write succeed.
               g_sUSB.m_bStartXfrWrite = FALSE;
       }


   }
   else if( g_CalibrationData.m_nDumpDeviceType == DUMP_DEVICE_PRINTER )  // use 0 for USB printer.      if( 1 )     // tthis works.
   {
       g_nFunctionSubSectionRunning = FUNCTION_SUB_SECTION_5;
       SaveFuntionNosInRAM();
       HandleUSBPrintJob();    // Checks if a USB print in progress and prints accordingly.
   }




    // testonly trigger a one time transfer.
    if(g_nTenHzCount == 150)     //i.e. start after 15 seconds.
    {
//         StartFileTransferFromSD( "NBLW112.s19" );

    }

// XX1 To cater for USB call from above to here....

    }

}


