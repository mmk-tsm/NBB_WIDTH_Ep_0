/* Rev:$Revision: 1.2 $ */
/* Copyright $Date: 2009/02/27 02:14:30 $ */
/*******************************************************************************
 *
 *   Copyright 2007 - 2008 NetBurner, Inc.  ALL RIGHTS RESERVED
 *
 *   Permission is hereby granted to purchasers of NetBurner hardware to use or
 *   modify this computer program for any use as long as the resultant program
 *   is only executed on NetBurner provided hardware.
 *
 *   No other rights to use this program or its derivatives in part or in whole
 *   are granted.
 *
 *   It may be possible to license this or other NetBurner software for use on
 *   non-NetBurner hardware. Please contact licensing@netburner.com for more
 *   information.
 *
 *   NetBurner makes no representation or warranties with respect to the
 *   performance of this computer program, and specifically disclaims any
 *   responsibility for any damages, special or consequential, connected with
 *   the use of this program.
 *
 *   NetBurner, Inc.
 *   5405 Morehouse Drive
 *   San Diego, CA  92121
 *   http://www.netburner.com
 *
 *
 * P.Smith                          14/11/09
 * correct compiler warnings, WRITE_BUFSIZE defined only once
 *
 ******************************************************************************/

#include "predef.h"

#include <stdio.h>
#include <ctype.h>
#include <startnet.h>

#include <basictypes.h>
#include <utils.h>
#include "cardtype.h"
#include "FileSystemUtils.h"
#include <effs_fat/fat.h>
//#ifdef USE_MMC
//#include <effs_fat/mmc_mcf.h>
#include "C:\Nburn\include\effs_fat/mmc_mcf.h"
//#endif
//#ifdef USE_CFC
//#include <effs_fat/cfc_mcf.h>
//#endif

#include <effs_fat/effs_utils.h>
#include <ucos.h>
#include <bsp.h>

extern "C"
{
   //int get_cd( void );          // Card detection check for MMC cards
   //int get_wp( void );          // Write protection check for MMC cards
}

char driveType[20];

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
  "F_ERR_MOUNTED",             // 29
  "F_ERR_TOOLONGNAME",        // 30
  "F_ERR_NOTFORREAD",         // 31
  "F_ERR_DELFUNC",            // 32
  "F_ERR_ALLOCATION",         // 33
  "F_ERR_INVALIDPOS",         // 34
  "F_ERR_NOMORETASK",         // 35
  "F_ERR_NOTAVAILABLE",       // 36
  "F_ERR_TASKNOTFOUND",       // 37
  "F_ERR_UNUSABLE"            // 38
};

#define WRITE_BUFSIZE 100

/*-------------------------------------------------------------------
  DisplayEffsErrorCode()
 -------------------------------------------------------------------*/
void DisplayEffsErrorCode( int code )
{
   if ( code <= MAX_EFFS_ERRORCODE )
   {
      iprintf( "%s\r\n", EffsErrorCode[code] );
   }
   else
   {
      iprintf( "Unknown EFFS error code [%d]\r\n", code );
   }
}


/*-------------------------------------------------------------------
  Init() - Initialization function for setting up EFFS on
  MultiMedia/Secure Digital cards or Comact Flash cards.
 -------------------------------------------------------------------*/
BYTE InitExtFlash()
{

#if (defined USE_MMC)
   // Card detection check
   if ( get_cd() == 0 )
   {
      while ( get_cd() == 0 )
      {
         iprintf( "No MMC/SD card detected. Insert a card and then press " );
         iprintf( "<Enter>\r\n" );
         if ( getchar() == 13 )
         {
            break;
         }
      }
   }
   // Write protection check
   if ( get_wp() == 1 )
   {
      while ( get_wp() == 1 )
      {
         iprintf( "SD/MMC Card is write-protected. Disable write protection then " );
         iprintf( "press <Enter>\r\n" );
         if ( getchar() == 13 )
         {
            break;
         }
      }
   }
#endif

   int rv;

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
   siprintf(driveType, "No Drive");
#if (defined USE_MMC)
   siprintf(driveType, "SD/MMC");
   rv = f_mountfat( MMC_DRV_NUM, mmc_initfunc, F_MMC_DRIVE0 );
#elif (defined USE_CFC)
   siprintf(driveType, "CFC");
   rv = f_mountfat( CFC_DRV_NUM, cfc_initfunc, F_CFC_DRIVE0 );
#endif
   if ( rv == F_NO_ERROR )
   {
      iprintf( "FAT mount to %s successful\r\n", driveType );
   }
   else
   {
      iprintf( "FAT mount to %s failed: ", driveType );
      DisplayEffsErrorCode( rv );
      return rv;
   }

   /* Change to SD/MMC drive
      We need to call the change function to access the new drive. Note
      that ANY function other than the f_mountfat() is specific to a task.
      This means that if f_chdrive() is called in a different task to a
      different drive, it will not affect this task - this task will
      remain on the same drive.
   */

   rv = f_chdrive( EXT_FLASH_DRV_NUM );

   if ( rv == F_NO_ERROR )
   {
      iprintf( "%s drive change successful\r\n", driveType );
   }
   else
   {
      iprintf( "%s drive change failed: ", driveType );
      DisplayEffsErrorCode( rv );
   }

   return rv;
}


/*-------------------------------------------------------------------
BYTE UnmountSD()
 -------------------------------------------------------------------*/
BYTE UnmountExtFlash()
{
   int rv;
   iprintf( "Unmounting %s card\r\n\r\n", driveType );
   rv = f_delvolume( EXT_FLASH_DRV_NUM );
   if ( rv != F_NO_ERROR )
   {
      iprintf( "*** Error in f_delvolume(): " );
      DisplayEffsErrorCode( rv );
   }
   return rv;
}

/*-------------------------------------------------------------------
BYTE FormatSD()
 -------------------------------------------------------------------*/
BYTE FormatExtFlash( long FATtype )
{
   int rv;
   iprintf( "Formatting %s card\r\n\r\n", driveType );
   rv = f_format( EXT_FLASH_DRV_NUM, FATtype );
   if ( rv != F_NO_ERROR )
   {
      iprintf( "*** Error in f_format(): " );
      DisplayEffsErrorCode( rv );
   }
   return rv;
}




/*-------------------------------------------------------------------
  DisplayEffsSpaceStats() - Display space used, total and bad
 -------------------------------------------------------------------*/
BYTE DisplayEffsSpaceStats()
{
   F_SPACE space;
   volatile int rv;
   iprintf( "Retrieving external flash usage...\r\n" );
   rv = f_getfreespace( EXT_FLASH_DRV_NUM, &space );

   if ( rv == F_NO_ERROR )
   {
      iprintf( "Flash card memory usage (bytes):\r\n" );
      iprintf( "%lu total, %lu free, %lu used, %lu bad\r\n", space.total, space.free, space.used, space.bad );
   }
   else
   {
      iprintf( "\r\n*** Error in f_getfreepace(): " );
      DisplayEffsErrorCode( rv );
   }

   return rv;
}


/*-------------------------------------------------------------------
  DumpDir() - Displays a list of all directories and files in the
  file system.
 -------------------------------------------------------------------*/
BYTE DumpDir()
{
   F_FIND finder;    // location to store the information retreived

   /* Find first file or subdirectory in specified directory. First call the
      f_findfirst function, and if file was found get the next file with
      f_findnext function. Files with the system attribute set will be ignored.
      Note: If f_findfirst() is called with "*.*" and the current directory is
      not the root directory, the first entry found will be "." - the current
      directory.
   */
   volatile int rc = f_findfirst( "*.*", &finder );
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
            iprintf( "Found File [%s] : %d Bytes\r\n", finder.filename, finder.filesize );
         }
      }
      while ( !f_findnext( &finder ) );
   }

   return rc;
}

/*
typedef struct
{
	char filename[FN_MAXPATH];	// file name+ext
	char name[F_MAXNAME];		// file name
	char ext[F_MAXEXT];			// file extension
	char attr;					   // attribute of the file

	unsigned short ctime;		// creation time
	unsigned short cdate;		// creation date
	unsigned long filesize;		// length of file

	unsigned long cluster;		// current file starting position
	F_NAME findfsname;		   // find properties
	F_POS pos;					   // position of the current list
} FN_FIND;
*/



/*-------------------------------------------------------------------
  BYTE DeleteFile( char* );  - Deletes File and prints status, returns 0 for success
 -------------------------------------------------------------------*/
BYTE DeleteFile(  char* pFileName )
{
   volatile int rv;
   rv =  f_delete( pFileName );
   if ( rv != F_NO_ERROR )
   {
      iprintf( "\r\n*** Error in f_delete( %s )\r\n", pFileName);
      DisplayEffsErrorCode( rv );
   }
   return rv;
}

/*-------------------------------------------------------------------
 DWORD WriteFile( BYTE* pDataToWrite, char* pFileName, DWORD NumBytes );
  - Open, Writes, Closes File and prints status, returns 0 for success

-------------------------------------------------------------------*/
DWORD WriteFile( BYTE* pDataToWrite, char* pFileName, DWORD NumBytes)
{
   DWORD rvW, rvC;
   F_FILE* fp = f_open( pFileName, "w+" );
   if ( fp )
   {
      rvW = f_write( pDataToWrite, 1, NumBytes, fp );
      if ( rvW != NumBytes )
         iprintf( "\r\n*** Error in f_write(%s): %d out of %d bytes writte\r\n", pFileName, rvW, NumBytes );

      rvC = f_close( fp );  // Close a previously opened file of type F_FILE
      if ( rvC != F_NO_ERROR )
      {
         iprintf( "*** Error in f_close(%s): ", pFileName );
         DisplayEffsErrorCode( rvC );
   	   return 0;
      }
   }
   else
	{
	  iprintf("Failed to open file %s\r\n", pFileName);
	   return 0;
   }

   return rvW;
}


/*-------------------------------------------------------------------
 DWORD AppendFile( BYTE* pDataToWrite, char* pFileName, DWORD NumBytes );
  - Open, Writes stating at end of data in file, Closes File and prints status, returns 0 for success

-------------------------------------------------------------------*/
DWORD AppendFile( BYTE* pDataToWrite, char* pFileName, DWORD NumBytes )
{
   DWORD rvA, rvC;
   F_FILE* fp = f_open( pFileName, "a+" );
   if ( fp )
   {
      rvA = f_write( pDataToWrite, 1, NumBytes, fp );
      if ( rvA != NumBytes )
         iprintf( "\r\n*** Error in f_write(%s): %d out of %d bytes written\r\n", pFileName, rvA, NumBytes );

      rvC = f_close( fp );  // Close a previously opened file of type F_FILE
      if ( rvC != F_NO_ERROR )
      {
         iprintf( "*** Error in f_close(%s): ", pFileName );
         DisplayEffsErrorCode( rvC );
         return 0;
      }
   }
   else
	{
	   iprintf("Failed to open file %s\r\n", pFileName);
	   return 0;
   }

   return rvA;
}


/*-------------------------------------------------------------------
 DWORD ReadFile( BYTE* pReadBuffer, char* pFileName, DWORD NumBytes );
  - Open, Writes, Closes File and prints status, returns 0 for success

-------------------------------------------------------------------*/
DWORD ReadFile( BYTE* pReadBuffer, char* pFileName, DWORD NumBytes )
{
   DWORD rvR, rvC;
   F_FILE* fp = f_open( pFileName, "r" );
   if ( fp )
   {
      rvR = (DWORD)f_read( pReadBuffer, 1, NumBytes, fp );
      //if( NumBytes != rvR )
      //    iprintf( "*** Warning in ReadFile(%s): %d out of %d bytes read\r\n", pFileName, rvR, NumBytes );

      rvC = f_close( fp );  // Close a previously opened file of type F_FILE
      if ( rvC != F_NO_ERROR )
      {
         iprintf( "*** Error in f_close(%s): ", pFileName );
         DisplayEffsErrorCode( rvC );
         return 0;
      }
   }
   else
   {
	   iprintf("Failed to open file %s\r\n", pFileName);
      return 0;
   }
   return rvR;
}


/*-------------------------------------------------------------------
  ReadWriteTest() - This function will read and write files/data to
  demonstrate basic file system operation.
 -------------------------------------------------------------------*/
void ReadWriteTest()
{
   /* Create a test file
      The f_open() function opens a file for reading or writing. The following
      modes are allowed to open:
         "r"   Open existing file for reading. The stream is positioned at the
               beginning of the file.
         "r+"  Open existing file for reading and writing. The stream is
               positioned at the beginning of the file.
         "w"   Truncate file to zero length or create file for writing. The
               stream is positioned at the beginning of the file.
         "w+"  Open a file for reading and writing. The file is created if it
               does not exist, otherwise it is truncated. The stream is
               positioned at the beginning of the file.
         "a"   Open for appending (writing to end of file). The file is created
               if it does not exist. The stream is positioned at the end of the
               file.
         "a+"  Open for reading and appending (writing to end of file). The file
               is created if it does not exist. The stream is positioned at the
               end of the file.
      Note: There is no text mode. The system assumes all files to be accessed in
      binary mode only.
   */
   char* FileName = "TestFile.txt";  // 8.3 file names supported by default
   iprintf( "\r\nCreating test file: %s\r\n", FileName );
   F_FILE* fp = f_open( FileName, "w+" );
   if ( fp )
   {
      for ( int i = 0; i < 5; i++ )
      {
         char write_buf[WRITE_BUFSIZE];
         siprintf( write_buf, "Hello World %d\r\n", i );
         /* f_write( const void *buffer,  // pointer to data to be written
            long size,           // size of items to be written
            long size size_st,   // number of items to be written
            F_FILE )             // handle of target file

            Returns the number of items written.
         */
         int n = f_write( write_buf, 1, strlen( write_buf ), fp );
         iprintf( "Wrote %d bytes: %s", n, write_buf );
      }

      // Read the data in the test file
      iprintf( "\r\nRewinding file\r\n" );
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
            /* Read bytes from the current position in the target file. File has
               to be opened with “r”, "r+", "w+" or "a+".
               f_read ( const void *buffer,  // pointer to buffer to store data
                        long size,           // size of items to be read
                        long size size_st,   // number of items to be read
                        F_FILE )             // handle of target file
               Returns the number of items read.
            */
            #define READ_BUFSIZE 80
            char read_buf[READ_BUFSIZE];
            int n = f_read( read_buf, 1, READ_BUFSIZE - 1, fp );
            read_buf[n] = '\0';  // terminate string
            iprintf( "Read %d bytes:\r\n%s\r\n", n, read_buf );
         }

         iprintf( "Closing file %s\r\n\r\n", FileName );
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


/*-------------------------------------------------------------------
 fgets_test
 ------------------------------------------------------------------*/
void fgets_test( char *FileName )
{
   iprintf( "\r\nOpening test file for reading: %s\r\n", FileName );
   F_FILE* fp = f_open( FileName, "r" );
   if ( fp )
   {
      iprintf("Calling fgets() until end of file\r\n");
      char buf[128];
      while ( !f_eof( fp ) )
      {
      	 char *buf_rtn = f_fgets( buf, 128, fp );
      	 if ( buf_rtn != NULL )
      	 {
  	 		iprintf( "fgets() returned: \"");
      	 	for (int i=0; i < (int)strlen(buf); i++)
      	 	{
                if ( isprint(buf[i]) )
      	 		   iprintf( "%c", buf[i] );
      	 		else
      	 		   iprintf("<0x%X>", buf[i]);
      	 	}
  	 		iprintf("\"\r\n");
      	 }
      	 else
      	    iprintf("End of file\r\n");
      }
   }

   iprintf( "Closing file %s\r\n\r\n", FileName );
   int rv = f_close( fp );  // Close a previously opened file of type F_FILE
   if ( rv != F_NO_ERROR )
   {
      iprintf( "*** Error in f_close(): " );
      DisplayEffsErrorCode( rv );
   }
}


/*-------------------------------------------------------------------
 Display a text file
 ------------------------------------------------------------------*/
void DisplayTextFile( char *FileName )
{
   iprintf( "\r\nOpening test file for reading: %s\r\n", FileName );
   F_FILE* fp = f_open( FileName, "r" );
   if ( fp )
   {
      while ( !f_eof( fp ) )
      {
         /* Read bytes from the current position in the target file. File has
            to be opened with “r”, "r+", "w+" or "a+".
            f_read ( const void *buffer,  // pointer to buffer to store data
                     long size,           // size of items to be read
                     long size size_st,   // number of items to be read
                     F_FILE )             // handle of target file
            Returns the number of items read.
         */
         #define DISP_READ_BUFSIZE 255
         char read_buf[DISP_READ_BUFSIZE];
         int n = f_read( read_buf, 1, DISP_READ_BUFSIZE - 1, fp );
         read_buf[n] = '\0';  // terminate string
         iprintf( "Read %d bytes:\r\n%s\r\n", n, read_buf );
      }

      iprintf( "Closing file %s\r\n\r\n", FileName );
      int rv = f_close( fp );  // Close a previously opened file of type F_FILE
      if ( rv != F_NO_ERROR )
      {
         iprintf( "*** Error in f_close(): " );
         DisplayEffsErrorCode( rv );
      }
   }
   else
   {
      iprintf( "\r\n*** Error opening file in DisplayTextFile() \"%s\", fp = %d\r\n", FileName, fp );
   }
}


/*-------------------------------------------------------------------
  fprintf_test
 -------------------------------------------------------------------*/
void fprintf_test()
{
   /* Open the test file
      The f_open() function opens a file for reading or writing. The following
      modes are allowed to open:
         "r"   Open existing file for reading. The stream is positioned at the
               beginning of the file.
         "r+"  Open existing file for reading and writing. The stream is
               positioned at the beginning of the file.
         "w"   Truncate file to zero length or create file for writing. The
               stream is positioned at the beginning of the file.
         "w+"  Open a file for reading and writing. The file is created if it
               does not exist, otherwise it is truncated. The stream is
               positioned at the beginning of the file.
         "a"   Open for appending (writing to end of file). The file is created
               if it does not exist. The stream is positioned at the end of the
               file.
         "a+"  Open for reading and appending (writing to end of file). The file
               is created if it does not exist. The stream is positioned at the
               end of the file.
      Note: There is no text mode. The system assumes all files to be accessed in
      binary mode only.
   */

   char* FileName = "TestFile.txt";  // 8.3 file names supported by default
   static WORD WriteCount;

   iprintf( "\r\nOpening test file for appending: %s\r\n", FileName );
   F_FILE* fp = f_open( FileName, "a" );
   if ( fp )
   {
      f_fprintf( fp, "Write #%d, Secs = %ld, Secs = 0x%lX\r\n", WriteCount, Secs, Secs );
      int rv = f_close( fp );
      if ( rv != F_NO_ERROR )
      {
         iprintf( "\r\n*** Error in f_close(), in fprintf_test(): fp = %d\r\n", fp );
         DisplayEffsErrorCode( rv );
      }

      iprintf( "Wrote to file: \"Write #%d, Secs = %ld, Secs = 0x%lX\"\r\n", WriteCount, Secs, Secs );
      WriteCount++;
   }
   else
   {
      iprintf( "\r\n*** Error opening file \"%s\", fp = %d\r\n", FileName, fp );
   }

   DisplayTextFile( FileName );
}


/*-------------------------------------------------------------------
  fputs_test
 -------------------------------------------------------------------*/
void fputs_test( char *FileName )
{
   /* Open the test file
      The f_open() function opens a file for reading or writing. The following
      modes are allowed to open:
         "r"   Open existing file for reading. The stream is positioned at the
               beginning of the file.
         "r+"  Open existing file for reading and writing. The stream is
               positioned at the beginning of the file.
         "w"   Truncate file to zero length or create file for writing. The
               stream is positioned at the beginning of the file.
         "w+"  Open a file for reading and writing. The file is created if it
               does not exist, otherwise it is truncated. The stream is
               positioned at the beginning of the file.
         "a"   Open for appending (writing to end of file). The file is created
               if it does not exist. The stream is positioned at the end of the
               file.
         "a+"  Open for reading and appending (writing to end of file). The file
               is created if it does not exist. The stream is positioned at the
               end of the file.
      Note: There is no text mode. The system assumes all files to be accessed in
      binary mode only.
   */

   iprintf( "\r\nOpening test file for appending: %s\r\n", FileName );
   F_FILE* fp = f_open( FileName, "a" );
   if ( fp )
   {
      char s[128];
      siprintf( s, "f_fputs() executed at %ld seconds\r\n", Secs );
      int n = f_fputs( s, fp );

      int rv = f_close( fp );
      if ( rv != F_NO_ERROR )
      {
         iprintf( "\r\n*** Error in f_close(), in fprintf_test(): fp = %d\r\n", fp );
         DisplayEffsErrorCode( rv );
      }

      iprintf( "Wrote %d bytes to file: \"%s\"\r\n", n, s );
   }
   else
   {
      iprintf( "\r\n*** Error opening file \"%s\", fp = %d\r\n", FileName, fp );
   }

   DisplayTextFile( FileName );
}




/*-------------------------------------------------------------------
  ReadWriteTest2() - This function will read and write files/data to
  demonstrate basic file system operation.
 -------------------------------------------------------------------*/
void ReadWriteTest2( int nFileNo )
{
int j,n;
long lK = (long)nFileNo;
char write_buf[WRITE_BUFSIZE];
	for(j=0; j<WRITE_BUFSIZE; j++)
		write_buf[j] = 'A' + ( (char)nFileNo & 0x3F );
	write_buf[97] ='\r';
	write_buf[98] ='\n';
	write_buf[99] ='\0';

char FileNameSD[20];
char cbuffer[20];

sprintf(FileNameSD, "--SDFile.txt");    //NB: filename limited to 8 chars.
iprintf("\n FileNameSd= %s",FileNameSD );

for(j=0; j<20; j++)
	cbuffer[j] = 0;

sprintf(cbuffer, "%ld", lK);

	for(j=0; j<8; j++)	//&&!bDone
	{
		if( cbuffer[j]  )					//!= '\0'
		  FileNameSD[j] = cbuffer[j];      // e.g.
		//else
			//bDone = TRUE;
	}
	iprintf("\n FileNameSd= %s",FileNameSD );

   /* Create a test file
      The f_open() function opens a file for reading or writing. The following
      modes are allowed to open:
         "r"   Open existing file for reading. The stream is positioned at the
               beginning of the file.
         "r+"  Open existing file for reading and writing. The stream is
               positioned at the beginning of the file.
         "w"   Truncate file to zero length or create file for writing. The
               stream is positioned at the beginning of the file.
         "w+"  Open a file for reading and writing. The file is created if it
               does not exist, otherwise it is truncated. The stream is
               positioned at the beginning of the file.
         "a"   Open for appending (writing to end of file). The file is created
               if it does not exist. The stream is positioned at the end of the
               file.
         "a+"  Open for reading and appending (writing to end of file). The file
               is created if it does not exist. The stream is positioned at the
               end of the file.
      Note: There is no text mode. The system assumes all files to be accessed in
      binary mode only.
   */
 //  char* FileName = "TestFl02.txt";  // 8.3 file names supported by default
   iprintf( "\r\nCreating test file: %s\r\n", FileNameSD );
   //F_FILE* fp = f_open( FileName, "w+" );
   F_FILE* fp = f_open( FileNameSD, "w+" );
   if ( fp )
   {
      for ( int i = 0; i < 99; i++ )		//10000
      {
//         siprintf( write_buf, "Bye Bye World............... %d\r\n", i );
         /* f_write( const void *buffer,  // pointer to data to be written
            long size,           // size of items to be written
            long size size_st,   // number of items to be written
            F_FILE )             // handle of target file

            Returns the number of items written.
         */
         //int n = f_write( write_buf, strlen( write_buf ), 1,  fp );		//strlen( write_buf )
         n = f_write( write_buf, 1, strlen( write_buf ), fp );
         //iprintf( "Wrote %d bytes: %s", n, write_buf );
      }
      OSTimeDly(TICKS_PER_SECOND);

      // Read the data in the test file
      iprintf( "\r\nRewinding file\r\n" );
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
            /* Read bytes from the current position in the target file. File has
               to be opened with “r”, "r+", "w+" or "a+".
               f_read ( const void *buffer,  // pointer to buffer to store data
                        long size,           // size of items to be read
                        long size size_st,   // number of items to be read
                        F_FILE )             // handle of target file
               Returns the number of items read.
            */
            #define READ_BUFSIZE 80
            char read_buf[READ_BUFSIZE];
            int n = f_read( read_buf, 1, READ_BUFSIZE - 1, fp );
            read_buf[n] = '\0';  // terminate string
            //iprintf( "Read %d bytes:\r\n%s\r\n", n, read_buf );
         }

         iprintf( "Closing file %s\r\n\r\n", FileNameSD );
//         OSTimeDly(TICKS_PER_SECOND);
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
      iprintf( "\r\n*** Error opening file \"%s\", fp = %d\r\n", FileNameSD, fp );
   }
}




