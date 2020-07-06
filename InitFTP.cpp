/******************************************************************************
 *  InitFTP.cpp
 * Copyright 1998-2004 NetBurner, Inc.  ALL RIGHTS RESERVED
 *   Permission is hereby granted to purchasers of NetBurner Hardware
 *   to use or modify this computer program for any use as long as the
 *   resultant program is only executed on NetBurner provided hardware.
 *
 *   No other rights to use this program or it's derivitives in part or
 *   in whole are granted.
 *
 *   It may be possible to license this or other NetBurner software for
 *   use on non NetBurner Hardware. Please contact Licensing@Netburner.com
 *   for more information.
 *
 *   NetBurner makes no representation or warranties with respect to the
 *   performance of this computer program, and specifically disclaims any
 *   responsibility for any damages, special or consequential, connected
 *   with the use of this program.
 *
 *   NetBurner, Inc
 *   5405 Morehouse Drive
 *   San Diego Ca, 92121
 *   http://www.netburner.com
 *
 *****************************************************************************/

/********************************************************************
 FTP Client Example Program

 This program will run a FTP Client program on the NetBurner board
 to do the following:
 - Connect to a FTP Server
 - Change to a remote directory called "test1"
 - Obtain a directory listing and print the listing to stdout
 - Create a file on the FTP Server called FOOBAR.TXT
 - Read back contents of FOOBAR.TXT and send to stdout

 Setup Requirements:
 1. Access to a FTP Server
 2. User name and password for the FTP Server
 3. A directory called "test1" must exist on the FTP Server
 4. Write permissions to the test1 directory


 *******************************************************************/

// Eclipse
// M.McKiernan                      2/12/09
// Removed
// Added include for FileSystemUtils.h & cardtype.h
// Removed DumpDir() and  DisplayEffsSpaceStats()
// Removed void ReadWriteTest()

#include <predef.h>
#include <stdio.h>
#include <ctype.h>
#include <startnet.h>
#include <autoupdate.h>
#include <ftp.h>
#include <ftpd.h>
#include <taskmon.h>
#include <InitFTP.h>
#include <ucos.h>
#include <dhcpclient.h>
// for file system:
#include <effs_fat/fat.h>
#include <effs_fat/mmc_mcf.h>
#include <effs_fat/cfc_mcf.h>
#include "Sdcard.h"


#include "FileSystemUtils.h"
#include "CardType.h"


#define SERVER_IP_ADDR "10.1.1.3"
#define FTP_PORT 21
#define USER "username"
#define PASS "password"

char tmp_resultbuff[255];

// This function reads the data stream from the fd and
// displays it to stdout, which is usually the com1 serial
// port on the NetBurner board.
void ShowFileContents( int fdr )
{
   iprintf( "\r\n[" );
   int rv;
   do
   {
      rv = ReadWithTimeout( fdr, tmp_resultbuff, 255, 20 );
      if ( rv < 0 )
      {
         iprintf( "RV = %d\r\n", rv );
      }
      else
      {
         tmp_resultbuff[rv] = 0;
         iprintf( "%s", tmp_resultbuff );
      }
   }
   while ( rv > 0 );
   iprintf( "]\r\n" );
}


//const char *AppName = "FTPClient Example";


// UserMain Function
/*
void UserMain( void *pd )
{
   InitializeStack();        // Initialize TCP/IP Stack
   EnableAutoUpdate();       // Enable NetBurner AutoUpdate downloads
   EnableTaskMonitor();
   OSChangePrio( MAIN_PRIO );  // Set UserMain priority level to default
*/
/*
//void InitialiseFTP( void )
{
   // Open FTP session at specified IP Address with specified user name
   // and password. There is a 5 second timeout.
   int ftp = FTP_InitializeSession( AsciiToIp( SERVER_IP_ADDR ),
                                    FTP_PORT,
                                    USER,
                                    PASS,
                                    5 * TICKS_PER_SECOND );

   if ( ftp > 0 ) // if the var ftp is > 0, it is the session handle
   {
      int rv = 0;

      // Change to the test directory
      // ************************************WARNING***************************
      // To run this sample a test1 directory must exist on the test server.
      // ************************************WARNING***************************

      // Change to test1 directory
      rv = FTPSetDir( ftp, "test1", 5 * TICKS_PER_SECOND );
      if ( rv == FTP_OK )
      {
         iprintf( "Reading file names from test1 directory\r\n" );
         int fdr = FTPGetList( ftp, NULL, 5 * TICKS_PER_SECOND );
         if ( fdr > 0 )
         {
            ShowFileContents( fdr );
            close( fdr );
            // Read the command result code from the FTPGetList command
            rv = FTPGetCommandResult( ftp, tmp_resultbuff, 255, 5 * TICKS_PER_SECOND );
            if ( rv != 226 )
            {
               iprintf( "Error Command result = %d %s\r\n", rv, tmp_resultbuff );
            }
         }
         else
         {
            iprintf( "Failed to get file list\r\n" );
         }


         // Create sample file
         iprintf( "Now creating the sample file FOOBAR.TXT\r\n" );
         int fdw = FTPSendFile( ftp, "FOOBAR.TXT", FALSE, 5 * TICKS_PER_SECOND );
         if ( fdw > 0 )
         {
            writestring( fdw, "This is a test file\r\n" );
            writestring( fdw, "This is line 2 of the test file\r\n" );
            writestring( fdw, "Last Line\r\n" );
            close( fdw );

            rv = FTPGetCommandResult( ftp, tmp_resultbuff, 255, 5 * TICKS_PER_SECOND );
            if ( rv != 226 )
            {
               iprintf( "Error Command result = %d %s\r\n", rv, tmp_resultbuff );
            }

            iprintf( "Now trying to readback the file we created \r\n" );
            fdr = FTPGetFile( ftp, "FOOBAR.TXT", FALSE, 5 * TICKS_PER_SECOND );
            if ( fdr > 0 )
            {
               ShowFileContents( fdr );
               close( fdr );
               // Read the command result code from the FTPGetFile command
               rv = FTPGetCommandResult( ftp, tmp_resultbuff, 255, 5 * TICKS_PER_SECOND );
               if ( rv != 226 )
               {
                  iprintf( "Error Command result = %d %s\r\n", rv, tmp_resultbuff );
               }
            }
            else
            {
               iprintf( "Failed to get file FOOBAR.TXT\r\n" );
            }
         }
         else
         {
            iprintf( "Failed to create file FOOBAR.TXT\r\n" );
         }
      }
      else
      {
         iprintf( "Failed to change to test directory" );
      }

      FTP_CloseSession( ftp );
   }
   else
   {
      iprintf( "Failed to open FTP Session\r\n" );
   }

   while ( 1 )
      OSTimeDly(1);     //allow main task to run.
}

*/

void InitialiseFTP( void )
{
   if ( EthernetIP == 0 )
   {
      iprintf( "Trying DHCP\r\n" );
      if ( GetDHCPAddress() == DHCP_OK )
      {
         iprintf( "DHCP assigned the IP address of :" );
         ShowIP( EthernetIP );
         iprintf( "\n" );
      }
      else
      {
         iprintf( "\r\n*** ERROR - DHCP Failed\r\n" );
      }
   }
         iprintf( "\r\n*** Got this far...1\r\n" );

//   DisplayEffsSpaceStats();  // Display file space usage
         iprintf( "\r\n*** Got this far...2\r\n" );
   DumpDir();                // Display flash card files and directories
         iprintf( "\r\n*** Got this far...3\r\n" );

   // This is where the read/write file system calls are made
//   ReadWriteTest();
         iprintf( "\r\n*** Got this far...4\r\n" );

   // Start FTP server with task priority higher than UserMain()
   int status = FTPDStart( 21, MAIN_PRIO - 2 );
   if ( status == FTPD_OK )
   {
      iprintf("Started FTP Server\r\n");
      iprintf("You can use Internet Explorer to drag and drop files to url \"ftp://");
      ShowIP(EthernetIP);
      iprintf("\"\r\n");
      iprintf("Long file names are not supported - only 8.3 format\r\n");
   }
   else
   {
      iprintf( "** Error: %d. Could not start FTP Server\r\n", status);
   }
}

/*-------------------------------------------------------------------
  DumpDir() - Displays a list of all directories and files in the
  file system.
 -------------------------------------------------------------------*/
/*
void DumpDir()
{
   F_FIND finder;    // location to store the information retreived

  // Find first file or subdirectory in specified directory. First call the
  //    f_findfirst function, and if file was found get the next file with
  //    f_findnext function. Files with the system attribute set will be ignored.
  //    Note: If f_findfirst() is called with "*.*" and the current directory is
  //    not the root directory, the first entry found will be "." - the current
  //    directory.
  //
   int rc = f_findfirst( "*.*", &finder );
   if ( rc == F_NO_ERROR )  // found a file or directory
   {
      do
      {
         if ( ( finder.attr & F_ATTR_DIR ) )
         {
            iprintf( "Found Directory [%s]\r\n", finder.filename );

            if ( finder.filename[0] != '.' )
            {
               f_chdir( finder.filename );
               DumpDir();
               f_chdir( ".." );
            }
         }
         else
         {
            iprintf( "Found File [%s]\r\n", finder.filename );
         }
      }
      while ( !f_findnext( &finder ) );
   }
}
*/

/*-------------------------------------------------------------------
  DisplayEffsSpaceStats() - Display space used, total and bad
 -------------------------------------------------------------------*/
/*
void DisplayEffsSpaceStats()
{
   F_SPACE space;
   int rv;
#ifdef USE_MMC
   rv = f_getfreespace( MMC_DRV_NUM, &space );
#else
   rv = f_getfreespace( CFC_DRV_NUM, &space );
#endif

   if ( rv == F_NO_ERROR )
   {
      iprintf( "Flash card memory usage (bytes):\r\n" );
      iprintf( "%d total, %d free, %d used, %d bad\r\n", space.total, space.free, space.used, space.bad );
   }
   else
   {
      iprintf( "\r\n*** Error in f_getfreepace(): " );
      DisplayEffsErrorCode( rv );
   }
}
*/

/*-------------------------------------------------------------------
  ReadWriteTest() - This function will read and write files/data to
  demonstrate basic file system operation.
 -------------------------------------------------------------------*/
/*
void ReadWriteTest()
{
 //  Create a test file
 //     The f_open() function opens a file for reading or writing. The following
 //     modes are allowed to open:
 //        "r"   Open existing file for reading. The stream is positioned at the
 //              beginning of the file.
 //        "r+"  Open existing file for reading and writing. The stream is
 //              positioned at the beginning of the file.
 //        "w"   Truncate file to zero length or create file for writing. The
 //              stream is positioned at the beginning of the file.
 //        "w+"  Open a file for reading and writing. The file is created if it
 //              does not exist, otherwise it is truncated. The stream is
 //              positioned at the beginning of the file.
 //        "a"   Open for appending (writing to end of file). The file is created
 //              if it does not exist. The stream is positioned at the end of the
 //              file.
  //       "a+"  Open for reading and appending (writing to end of file). The file
 //              is created if it does not exist. The stream is positioned at the
   //            end of the file.
  //    Note: There is no text mode. The system assumes all files to be accessed in
 //     binary mode only.

   char* FileName = "TestFile.txt";  // 8.3 file names supported by default
   iprintf( "Creating test file: %s\r\n", FileName );
   F_FILE* fp = f_open( FileName, "w+" );
   if ( fp )
   {
      for ( int i = 0; i < 5; i++ )
      {
         #define WRITE_BUFSIZE 80
         char write_buf[WRITE_BUFSIZE];
         siprintf( write_buf, "Hello World %d\r\n", i );
      //    f_write( const void *buffer,  // pointer to data to be written
        //    long size,           // size of items to be written
        //    long size size_st,   // number of items to be written
        //    F_FILE )             // handle of target file

        //    Returns the number of items written.
        //
         int n = f_write( write_buf, strlen( write_buf ), 1, fp );
         iprintf( "Wrote %d items: \"%s\"", n, write_buf );
      }

      // Read the data in the test file
      iprintf( "Rewinding file\r\n" );
      int rv = f_rewind( fp );   // set current file pointer to start of file
      if ( rv != F_NO_ERROR )
      {
         iprintf( "\r\n*** Error in f_close(): fp = %d\r\n", fp );
         DisplayEffsErrorCode( rv );
      }
      else
      {
         while ( !f_eof( fp ) )
         {
        //     Read bytes from the current position in the target file. File has
        //       to be opened with “r”, "r+", "w+" or "a+".
        //       f_read ( const void *buffer,  // pointer to buffer to store data
        //                long size,           // size of items to be read
       //                 long size size_st,   // number of items to be read
      //                  F_FILE )             // handle of target file
    //           Returns the number of items read.
       //
            #define READ_BUFSIZE 80
            char read_buf[READ_BUFSIZE];
            int n = f_read( read_buf, 1, READ_BUFSIZE - 1, fp );
            read_buf[n] = '\0';  // terminate string
            iprintf( "Read %d items:\r\n\"%s\"\r\n", n, read_buf );
         }

         iprintf( "Closing file %s\r\n", FileName );
         rv = f_close( fp );  // Close a previously opened file of type F_FILE
         if ( rv != F_NO_ERROR )
         {
            iprintf( "*** Error in f_close(): " );
            DisplayEffsErrorCode( rv );
         }
      }
   }
   else
   {
      iprintf( "\r\n*** Error opening file \"%s\", fp = %d\r\n", FileName, fp );
   }
}
*/

/*
// #define MAX_EFFS_ERRORCODE   ( 29 )
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
  "F_ERR_BUSY",                // 22
  "F_ERR_WRITEPROTECT",        // 23
  "F_ERR_INVFATTYPE",          // 24
  "F_ERR_MEDIATOOSMALL",       // 25
  "F_ERR_MEDIATOOLARGE",       // 26
  "F_ERR_NOTSUPPSECTORSIZE",   // 27
  "F_ERR_DELFUNC",             // 28
  "F_ERR_MOUNTED"              // 29
};
*/

/*-------------------------------------------------------------------
  DisplayEffsErrorCode()
 -------------------------------------------------------------------*/
 /*
void DisplayEffsErrorCode( int code )
{
   if ( code <= MAX_EFFS_ERRORCODE )
   {
      iprintf( "%s\r\n", EffsErrorCode[code] );
   }
   else
   {
      iprintf("Unknown EFFS error code [%d]\r\n", code );
   }
}
*/

