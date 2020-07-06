// M. McKiernan                                                 28/11/08
// USB Memory Stick related functions - file transfer
//
// P.Smith                                                 19/11/09
// correct compiler warning
//
// M.McKiernan                      1/12/09
// bug fix in TransferFileSegment - see eclipse
// Removed call to #include <effs_fat/mmc_drv.h> - mmc_drv.h not present in eclipse version
// added predef.h

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
//eclipse #include <effs_fat/mmc_drv.h>
#include "sdcard.h"
#include "Setpointformat.h"
#include "Loadcaldatafromsdcard.h"
#include "MBProgs.h"
#include "iniparser.h"
#include "iniexample.h"
#include    "Alarms.h"
#include "USBDisk.h"

extern structUSB g_sUSB;
extern  char     g_cPrinterBuffer[];



   #define XFR_BLK_SIZE 4096

   char XfrBuf[XFR_BLK_SIZE+4];
   int  g_nXfrBytes = 0;
   bool g_bXfrBufBusy = FALSE;
   bool g_bSDCardAccessInProgress = FALSE;

   bool g_bXfrFinished = FALSE;
   bool g_bFileXfrInProgress = FALSE;
   bool g_bXfrFileReadFinished = FALSE;
   int  g_nSegmentNo = 0;
   char g_cXfrFilename[20];
   long g_lXfrFileSize = 0;



// HandleFileTransferFromSD - handles a file transfer from SD to USB stick.
// Runs repetetively while the transfer is in progress - g_bFileXfrInProgress
// Waits for transfer buffer to be available (g_bXfrBufBusy clear) - cleared by USB side of software.
// Sets g_bXfrFileReadFinished when SD file read completed.
// If bytes to transfer, sets g_sUSB.m_bStartXfrWrite to start USB write from XfrBuf.
void HandleFileTransferFromSD( void )
{

   if( g_bFileXfrInProgress && !g_bXfrFileReadFinished && !g_bXfrBufBusy)
   {
      if(fdTelnet > 0)
         iprintf("\n %s Seg no %d", g_cXfrFilename, g_nSegmentNo );

      TransferFileSegment( g_cXfrFilename, g_nSegmentNo);
      g_nSegmentNo++;
      if( (g_nSegmentNo * XFR_BLK_SIZE) >= g_lXfrFileSize)
      {
         g_bXfrFileReadFinished = TRUE;
      }

      if( g_bXfrBufBusy && g_nXfrBytes > 0 )
      {
         sprintf( g_sUSB.m_cXfrFileName, g_cXfrFilename );
         g_sUSB.m_nXfrBytes = g_nXfrBytes;
         g_sUSB.m_bStartXfrWrite = TRUE;
         if(fdTelnet > 0)
            iprintf("\nStart Xfr to %s of %d bytes", g_sUSB.m_cXfrFileName, g_nXfrBytes);
         // need to start transfer write to USB??
      }

   }

}

// StartFileTransferFromSD( char * cFileName ) - sets up to transfer a file from SD card to USB.
// Returns true if file transfer started.
// Initialies file name, sets segment no. to 0 (first segment) and g_bFileXfrInProgress set
bool StartFileTransferFromSD( char * cFileName )
{
 bool bStarted=FALSE;

   if( !g_bFileXfrInProgress )
   {
      sprintf( g_cXfrFilename, cFileName);
      g_nSegmentNo = 0;                      // first segment of file.
      g_bXfrFileReadFinished = FALSE;
      g_bFileXfrInProgress = TRUE;

      bStarted = TRUE;
   }
   else
      bStarted = FALSE;

   return( bStarted );
}

// Transfer a file segment, size=XFR_BLK_SIZE from SD card file.
// Segment no passed - nb first segment is no. 0.
// file name passed.
// file size returned in g_lXfrFileSize.
// File segment read into XfrBuf.  No. of bytes read returned in g_nXfrBytes.
// g_bXfrBufBusy set if > 0 bytes read into XfrBuf.
void TransferFileSegment( char *cFileName, int nSegmentNo )
{
    bool bDone=FALSE;      // set if end reached.

   int n;
   long lOffset = nSegmentNo*XFR_BLK_SIZE;


   if(SDCardPresent() && !g_bSPIBusBusy)
   {
      g_bSPIBusBusy = TRUE;
       g_bSDCardAccessInProgress = TRUE;
       InitialiseSDCard();
       SPI_Select_SD();
       F_FILE * fp;
       fp=f_open(cFileName, "r");		//eclipse.
       if(fp)
       {
//          fp=f_open(cFileName, "r");
            g_lXfrFileSize = f_filelength( cFileName );

          f_seek(fp, lOffset, SEEK_SET);     // seeks to the segment in file
          if( !f_eof( fp ) )    //feof = 0 => not end.
            n = f_read( XfrBuf, 1, XFR_BLK_SIZE, fp );
          else
            n = 0;      // at file end.

          XfrBuf[n] = '\0';     // terminate buffer if ASCII - testonly
          g_nXfrBytes = n;       // save no. of bytes read.

          if(n>0)
            g_bXfrBufBusy = TRUE;

          if( f_eof( fp ) || n<XFR_BLK_SIZE )
          {
            bDone = TRUE;
            g_bXfrFinished = TRUE;
          }
          f_close(fp);

           if(fdTelnet > 0)   ;
//            iprintf("%s",XfrBuf);

          f_delvolume(MMC_DRV_NUM);
          SetSPIEX1_Lo();   //  EX1 and EX2 must be both Hi for SD chip select.
          SetSPIEX2_Lo();   //  so this will de-activate SD chip selection.
          ReInitialisePinsUsedBySDCard();
          g_bSPIBusBusy = FALSE;
          g_bSDCardAccessInProgress = FALSE;


       }
       else
       {
         if(fdTelnet > 0)
           iprintf("\n File cannot be opened");
       }
   }
   else
   {
       if(fdTelnet > 0)
         iprintf("SD card not inserted or SPI busy");
   }

}


