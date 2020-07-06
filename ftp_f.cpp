/* Rev:$Revision: 1.6 $ */
/* Copyright $Date: 2008/02/26 01:04:15 $ */ 

/*******************************************************************************
 *
 *   Copyright (c) 2003 by HCC Embedded 
 *
 *   This software is copyrighted by and is the sole property of HCC.  All
 *   rights, title, ownership, or other interests in the software remain the
 *   property of HCC.  This software may only be used in accordance with the
 *   corresponding license agreement.  Any unauthorized use, duplication,
 *   transmission, distribution, or disclosure of this software is expressly
 *   forbidden.
 *
 *   This copyright notice may not be removed or modified without prior written
 *   consent of HCC.
 *
 *   HCC reserves the right to modify this software without notice.
 *
 *   HCC Embedded
 *   Budapest 1132
 *   Victor Hugo Utca 11-15
 *   Hungary
 *
 *   Tel:    +36 (1) 450 1302
 *   Fax:    +36 (1) 450 1303
 *   http:   www.hcc-embedded.com
 *   E-mail: info@hcc-embedded.com
 *
 ******************************************************************************/

#include <predef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <startnet.h>
#include <autoupdate.h>
#include <nbtime.h>
#include <time.h>
#include "cardtype.h"
#include <effs_fat/udefs_f.h>
#include <effs_fat/api_f.h>
#include <effs_fat/port_f.h>
#include <effs_fat/fat.h>
#ifdef USE_MMC
#include <effs_fat/mmc_mcf.h>
#endif
#ifdef USE_CFC
#include <effs_fat/cfc_mcf.h>
#endif
#include <ftpd.h>
#include "ftp_f.h"


#define FTP_BUFFER_SIZE   ( 32 * 1024 )            // Make a 32KB BUFFER
#define MAX_STRING    128


static char FTP_buffer[FTP_BUFFER_SIZE] __attribute__( ( aligned( 16 ) ) );
static const char mstr[12][4] = { "Jan", "Feb", "Mar", "Apr", "May", "Jun",
                                  "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };


////////////////////////////////////////////////////////////////////////////////
// settimedate() - Set the date.
//
static void settimedate( F_FIND *f )
{
   int nret = f_settimedate( f->filename, f_gettime(), f_getdate() );
   if ( nret == F_NO_ERROR )
   {
      iprintf( "Time stamping successful\r\n" );
   }
   else
   {
      iprintf( "Time stamping failed: %d\r\n", nret );
   }
}


////////////////////////////////////////////////////////////////////////////////
// gettimedate() - This function displays information in MTTTY of subdirectories
// and files in a current directory.  The information displayed is: the
// filename, modified time stamp, modified date stamp, and filesize.
//
static void gettimedate( F_FIND *f )
{
   unsigned short t, d;

   int nret = f_gettimedate( f->filename, &t, &d );
   if ( nret == F_NO_ERROR )
   {
      iprintf( "%15s   |", f->filename );
      iprintf( "%2.2d:%2.2d:%2.2d   |", ( ( t & 0xF800 ) >> 11 ), ( ( t & 0x07E0 ) >> 5 ), 2 * ( t & 0x001F ) );
      iprintf( "%2.2d/%2.2d/%4d   |", ( ( d & 0x01E0 ) >> 5 ), ( d & 0x001F ), ( 1980 + ( ( d & 0xFE00 ) >> 9 ) ) );
      iprintf( "%9ld Bytes\r\n", f->filesize );
   }
   else
   {
      iprintf( "Time stamp retrieval failed: %d\r\n", nret );
   }
}


////////////////////////////////////////////////////////////////////////////////
// getdatestring() - Generate date string.
//
static void getdatestring( F_FIND *f, char *tmp )
{
   // Converts file time and date stamp to a value that can be interpreted by
   // the user.
   //unsigned short sec = 2 * ( ( f->ctime ) & 0x1F );
   unsigned short minute = ( ( f->ctime ) & 0x7E0 ) >> 5;
   unsigned short hour = ( ( f->ctime ) & 0xF800 ) >> 11;
   unsigned short day = ( f->cdate ) & 0x1F;
   unsigned short month = ( ( f->cdate ) & 0x01E0 ) >> 5;
   unsigned short year = 1980 + ( ( ( f->cdate ) & 0xFE00 ) >> 9 );

   // For FTP file properties: If the current year matches the year stamp of the
   // associated file, then the hour and minutes are displayed.  Otherwise, the
   // year is used in place of hour and minutes.
   if  ( ( 1980 + ( ( ( f_getdate() ) & 0xFE00 ) >> 9 ) ) == year )
   {
      sprintf( tmp, "%3s %2d %2.2d:%2.2d", mstr[month - 1], day, hour, minute );
   }
   else
   {
      sprintf( tmp, "%3s %2d  %4d", mstr[month - 1], day, year );
   }
}


////////////////////////////////////////////////////////////////////////////////
// getdirstring() - Generate dir entry string.
//
static void getdirstring( F_FIND *f, char *dst )
{
   char tmp[32];

   if ( ( f->attr ) & F_ATTR_DIR )
   {
      dst[0] = 'd';
   }
   else
   {
      dst[0] = '-';
   }

   dst[1] = 0;
   strcat( dst, "rw-rw-rw-   1 none" );
   strcat( dst, " " );

   sprintf( tmp, "%9ld", f->filesize );
   strcat( dst, tmp );
   strcat( dst, " " );

   getdatestring( f, tmp );
   strcat( dst, tmp );
   strcat( dst, " " );

   strcat( dst, f->filename );
}


////////////////////////////////////////////////////////////////////////////////
// FTPDSessionStart() - Start FTP session.
//
void *FTPDSessionStart( const char *user, const char *passwd,
                        const IPADDR hi_ip )
{
#ifdef USE_MMC
   f_chdrive( MMC_DRV_NUM );
#endif /* USE_MMC */

#ifdef USE_CFC
   f_chdrive( CFC_DRV_NUM );
#endif /* USE_CFC */

   f_chdir( "\\" );

   return( ( void * ) 1 );
}


////////////////////////////////////////////////////////////////////////////////
// FTPDSessionEnd() - Finish FTP session.
//
void FTPDSessionEnd( void *pSession ) {}


////////////////////////////////////////////////////////////////////////////////
// FTPD_DirectoryExists() - Check for a directory.
//
int FTPD_DirectoryExists( const char *full_directory, void *pSession )
{
   if ( *full_directory == 0 )
   {
      return( FTPD_OK );
   }

   f_chdir( "/" );

   if ( f_chdir( ( char * ) full_directory ) )
   {
      return( FTPD_FAIL );
   }
   else
   {
      return( FTPD_OK );
   }

   return( FTPD_OK );
}


////////////////////////////////////////////////////////////////////////////////
// FTPD_CreateSubDirectory() - Create directory.
//
int FTPD_CreateSubDirectory( const char *current_directory, const char *new_dir,
                             void *pSession )
{
   int rc;

   f_chdir( "/" );

   if ( *current_directory )
   {
      rc = f_chdir( ( char * ) current_directory );

      if ( rc )
      {
         return( FTPD_FAIL );
      }
   }

   rc = f_mkdir( ( char * ) new_dir );

   if ( rc == 0 )
   {
      return( FTPD_OK );
   }

   return( FTPD_FAIL );
}


////////////////////////////////////////////////////////////////////////////////
// FTPD_DeleteSubDirectory() - Delete directory.
//
int FTPD_DeleteSubDirectory( const char *current_directory, const char *sub_dir,
                             void *pSession )
{
   int rc;

   f_chdir( "/" );

   if ( *current_directory )
   {
      rc = f_chdir( ( char * ) current_directory );

      if ( rc )
      {
         return( FTPD_FAIL );
      }
   }

   rc = f_rmdir( ( char * ) sub_dir );

   if ( rc == 0 )
   {
      return( FTPD_OK );
   }  

   return( FTPD_FAIL );
}


////////////////////////////////////////////////////////////////////////////////
// FTPD_ListSubDirectories() - List directories.
//
int FTPD_ListSubDirectories( const char *current_directory, void *pSession,
                             FTPDCallBackReportFunct *pFunc, int socket )
{
   F_FIND find;
   long rc;
   char s[128];

   f_chdir( "/" );

   if ( *current_directory )
   {
      if ( f_chdir( ( char * ) current_directory ) )
      {
         return( FTPD_FAIL );
      }
   }

   rc = f_findfirst( "*.*", &find );

   if ( !rc )
   {
      do
      {
         if ( find.attr & F_ATTR_DIR )
         {
            getdirstring( &find, s );
            pFunc( socket, s );
         }
      }
      while ( !f_findnext( &find ) );
   }

   return( FTPD_OK );
}


////////////////////////////////////////////////////////////////////////////////
// FTPD_FileExists() - Check for a file.
//
int FTPD_FileExists( const char *full_directory, const char *file_name,
                     void *pSession )
{
   F_FILE *t;

   if ( strcmp( file_name, "_format" ) == 0 || strcmp( file_name, "_hformat" ) == 0 
#ifdef USE_CFC
        || strcmp( file_name, "_cfc" ) == 0 )
#endif /* USE_CFC */

#ifdef USE_HDD
        || strcmp( file_name, "_hdd" ) == 0 )
#endif /* USE_HDD */

#ifdef USE_MMC
        || strcmp( file_name, "_mmc" ) == 0 )
#endif /* USE_MMC */

#ifdef USE_FATRAM
        || strcmp( file_name, "_fram" ) == 0 )
#endif /* USE_FATRAM */

#ifdef USE_NOR
        || strcmp( file_name, "_nor" ) == 0 )
#endif /* USE_NOR */

#ifdef USE_STDRAM
        || strcmp( file_name, "_sram" ) == 0 )
#endif /* USE_STDRAM */
   {
      return( FTPD_OK );
   }

   f_chdir( "/" );

   if ( *full_directory )
   {
      if ( f_chdir( ( char * ) full_directory ) )
      {
         return( FTPD_FAIL );
      }
   }

   t = f_open( ( char * ) file_name, "r" );

   if ( t )
   {
      f_close( t );
      return( FTPD_OK );
   }

   return( FTPD_FAIL );
}


////////////////////////////////////////////////////////////////////////////////
// FTPD_SendFileToClient() - Retrieve function for FTP (get).
//
int FTPD_SendFileToClient( const char *full_directory, const char *file_name,
                           void *pSession, int fd )
{
   F_FILE *rfile;   // Actual opened file
   int l, m;

#ifdef USE_NOR
   if ( strcmp( file_name, "_nor" ) == 0 )
   {
      //iprintf( "Change to NOR.\r\n" );
      f_chdrive( NOR_DRV_NUM );
      return( FTPD_FAIL );
   }
#endif /* USE_NOR */

#ifdef USE_STDRAM
   if ( strcmp( file_name, "_sram" ) == 0 )
   {
      //iprintf( "Change to STDRAM.\r\n" );
      f_chdrive( STDRAM_DRV_NUM );
      return( FTPD_FAIL );
   }
#endif /* USE_STDRAM */

#ifdef USE_MMC
   if ( strcmp( file_name, "_mmc" ) == 0 )
   {
      //iprintf( "Change to MMC.\r\n" );
      f_chdrive( MMC_DRV_NUM );
      return( FTPD_FAIL );
   }
#endif /* USE_MMC */

#ifdef USE_CFC
   if ( strcmp( file_name, "_cfc" ) == 0 )
   {
      //iprintf( "Change to CFC.\r\n" );
      f_chdrive( CFC_DRV_NUM );
      return( FTPD_FAIL );
   }
#endif /* USE_CFC */

#ifdef USE_HDD
   if ( strcmp( file_name, "_hdd" ) == 0 )
   {
      //iprintf( "Change to HDD.\r\n" );
      f_chdrive( HDD_DRV_NUM );
      return( FTPD_FAIL );
   }
#endif /* USE_HDD */

#ifdef USE_FATRAM
   if ( strcmp( file_name, "_fram" ) == 0 )
   {
      //iprintf( "Change to FATRAM.\r\n" );
      f_chdrive( FATRAM_DRV_NUM );
      return( FTPD_FAIL );
   }
#endif /* USE_FATRAM */

   if ( strcmp( file_name, "_format" ) == 0 )
   {
      int rc = 0;

      //iprintf( "Formatting...\r\n" );
      rc = f_format( f_getdrive(), F_FAT32_MEDIA );

      //if ( rc )
      //{
      //   iprintf( "Failed.\r\n" );
      //}
      //else
      //{
      //   iprintf( "OK.\r\n" );
      //}
    
      return( FTPD_FAIL );
   }


   f_chdir( "/" );

   if ( *full_directory )
   {
      if ( f_chdir( ( char * ) full_directory ) )
      {
         return( FTPD_FAIL );
      }
   }

   /* If file is not open, then try to open it. */
   rfile = f_open( ( char * ) file_name, "r" );

   /* Return with error if not successful. */
   if ( !rfile )
   {
      return( FTPD_FAIL );
   }

   //l = f_filelength( ( char * ) file_name );
   l=1;
   
   while ( l )
   {
      m = f_read( FTP_buffer, 1, FTP_BUFFER_SIZE, rfile );   // Read from file
      if(!(m == FTP_BUFFER_SIZE))
         l=0;
      writeall( fd, FTP_buffer, m );
   }

   f_close( rfile );                   // Close the file

   return( FTPD_OK );
}


////////////////////////////////////////////////////////////////////////////////
// FTPD_AbleToCreateFile() - Check if it is possible to create a file.
//
int FTPD_AbleToCreateFile( const char *full_directory, const char *file_name,
                           void *pSession )
{
   return( FTPD_OK );
}


////////////////////////////////////////////////////////////////////////////////
// FTPD_GetFileFromClient() - Write file to drive.
//
int FTPD_GetFileFromClient( const char *full_directory, const char *file_name,
                            void *pSession, int fd )
{
   F_FILE *wfile = 0;   // Actual opened file
   F_FIND find;
   int l, m;
   //int T = 0;

   f_chdir( "/" );

   if ( *full_directory )
   {
      if ( f_chdir( ( char * ) full_directory ) )
      {
         return( FTPD_FAIL );
      }
   }

   /* Only accept file names of size 12 (8+1+3). */
   //if ( strlen( ( char * ) file_name ) > 12 ) return( FTPD_FAIL );

   wfile = f_open( ( char * ) file_name, "w" );   // Open it for write

   int rc = f_findfirst( file_name, &find );
   if ( rc == 0 )
   {
      if ( !( find.attr & F_ATTR_DIR ) )
      {
         settimedate( &find );
      }
   }
   else
   {
      iprintf( "f_findfirst failed\r\n" );
   }

   /* Return error if not successful. */
   if ( !wfile )
   {
      return( FTPD_FAIL );
   }

   //printf( "Write open %s\r\n", file_name );

   l = 0;

   do
   {
      //printf( "!!!!!!!!>\r\n" );
      //m = read( fd, FTP_buffer + l, FTP_BUFFER_SIZE - l );
      m = ReadWithTimeout( fd, FTP_buffer + l, FTP_BUFFER_SIZE - l, 20 );
      //printf( "read = %d (%d)\r\n", m, T );

      if ( m > 0 )
      {
         l += m;
      }

      if ( l == FTP_BUFFER_SIZE )
      {
         //printf( "----------->\r\n" );
         f_write( FTP_buffer, 1, l, wfile );
         //printf( "w (%d)\r\n", l );
         //T += l;
         l = 0;
      }
   }

   while ( m > 0 );
   if ( l )
   {
      f_write( FTP_buffer, 1, l, wfile );
   }

   //T += l;
   //printf( "TOTAL = %d (m = %d, l = %d)\r\n", T, m, l );

   f_close( wfile );
   //print( "Write OK.\r\n" );

   return( FTPD_OK );
}


////////////////////////////////////////////////////////////////////////////////
// FTPD_DeleteFile() - Delete a file.
//
int FTPD_DeleteFile( const char *current_directory, const char *file_name,
                     void *pSession )
{
   f_chdir( "/" );

   if ( *current_directory )
   {
      if ( f_chdir( ( char * ) current_directory ) )
      {
         return( FTPD_FAIL );
      }
   }

   if ( f_delete( ( char * ) file_name ) )
   {
      return( FTPD_FAIL );
   }

   return( FTPD_OK );
}


////////////////////////////////////////////////////////////////////////////////
// FTPD_ListFile() - Get file list.
//
int FTPD_ListFile( const char *current_directory, void *pSession,
                   FTPDCallBackReportFunct *pFunc, int socket )
{
   F_FIND find;
   char s[128];
   int rc;

   f_chdir( "/" );

   if ( *current_directory )
   {
      if ( f_chdir( ( char * ) current_directory ) )
      {
         return( FTPD_FAIL );
      }
   }

   rc = f_findfirst( "*.*", &find );

   if ( rc == 0 )
   {
      iprintf("\r\nDisplaying time/date information\r\n");
      do {
         if ( !( find.attr & F_ATTR_DIR ) )
         {
            getdirstring( &find, s );
            gettimedate( &find );
            pFunc( socket, s );
         }
      }
      while ( !f_findnext( &find ) );
      iprintf("\r\n");
   }
   //else
   //{
   //   sprintf( s, "Card error: %d", rc );
   //   pFunc( socket, s );
   //}

   return( FTPD_OK );
}


////////////////////////////////////////////////////////////////////////////////
// FTPD_Rename() - Rename a file.
//
int FTPD_Rename( const char *full_directory, const char *old_file_name,
                 const char *new_file_name, void *pSession )
{
   
   f_chdir( "/" );

   if ( *full_directory )
   {
      if ( f_chdir( ( char * ) full_directory ) )
      {
         return( FTPD_FAIL );
      }
   }

   if ( f_rename( old_file_name, new_file_name ) )
   {
      return FTPD_FAIL;
   }

   return FTPD_OK;
}
