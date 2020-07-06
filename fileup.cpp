/* Rev:$Revision: 1.0 $ */
/******************************************************************************
 *
 * Copyright 2006 NetBurner, Inc.  ALL RIGHTS RESERVED
 *   Permission is hereby granted to purchasers of NetBurner Hardware
 *   to use or modify this computer program for any use as long as the
 *   resultant program is only executed on NetBurner provided hardware.
 *
 *   No other rights to use this program or it's derivitives in part or
 *   in whole are granted.
 *
 *   It may be possible to license this or other NetBurner software for
 *   use on non NetBurner Hardware. Please contact Licensing Netburner.com
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
     P.Smith                                           23/4/07
     removed printfs

 // M.McKiernan                      1/12/09
// Added include <ucos.h>.

 *****************************************************************************/
#include "predef.h"
#include <basictypes.h>
#include <stdio.h>
#include <ctype.h>
#include <effs_fat/fat.h>
#include <effs_fat/mmc_mcf.h>
#include <effs_fat/cfc_mcf.h>
#include <system.h>
#include <malloc.h>
#include <bsp.h>
#include <ucosmcfc.h>
#include "fileup.h"
#include <ucos.h>

extern const char* PlatformName;

char fat_read_buffer[256];
#define tmp_buffer_size (256)
char tmp_buffer[tmp_buffer_size];
int tmp_buffer_end = 0;
int tmp_buffer_start = 0;


int my_f_read_line( char* buffer, int buf_siz, F_FILE* fp )
{
//  int i;
   int nr = 0;

   do
   {
      if ( tmp_buffer_end <= tmp_buffer_start )
      {
//testonly
 //        iprintf("\n..tmp_b_end <= tmp_b_start");
         if ( f_eof( fp ) )
            return 0;
//testonly
//         iprintf("\n..past f_eof( fp ) ");

         int n = f_read( tmp_buffer, 1, tmp_buffer_size, fp );
         tmp_buffer_start = 0;
         tmp_buffer_end = n;
 //testonly
 //        for(i=0; i<20; i++)
 //           iprintf("%c", tmp_buffer[i]);
 //        iprintf("\n %d Any key to contintue..\n", n);
//         getchar();

         if ( n == 0 )
         {
            buffer[nr + 1] = 0;
            return nr;
         }
      }

      *( buffer + nr ) = tmp_buffer[tmp_buffer_start++];

      if ( ( buffer[nr] == '\r' ) || ( buffer[nr] == '\n' ) )
      {
         if ( nr != 0 )
         {
            buffer[nr + 1] = 0;
            return nr;
         }
      }
      else
         nr++;
   }
   while ( nr < buf_siz );

   buffer[nr + 1] = 0;
   return nr;
}


extern BYTE user_flash_buffer[];
extern int ProcessS3( const char* cp, DWORD base_Addr, PBYTE CopyTo, DWORD& cur_pos, DWORD maxlen );


/* This is the structure of an APP File */
struct FlashStartUpStruct
{
    unsigned long dwBlockRamStart ;
    unsigned long dwExecutionAddr ;
    unsigned long dwBlockSize     ;
    unsigned long dwSrcBlockSize  ;
    unsigned long dwBlockSum      ;
    unsigned long dwStructSum     ;
};

/* The Flash application must start at address 0xFFC08000. The code block should follow.
 * The entire file size is guarenteed to be less than dwSrcBlockSize+24, so we can malloc
 * that space when we start.
 */
#define BASE_FLASH_ADDR 0xFFC08000


/* This function can be used to update the application in the flash memory of a NetBurner
 * device. The function has the ability to compare the cksum of the existing application
 * and the flash card file to determine if an update should be performed.
 * Parameters:
 *    fp = A pointer to a EFFS flash file type F_FILE. This file must have been opened
 *         by the code that called this function.
 *    bUpdateEvenIfCurrent = A value of 0 = do not update if cksums match,
 *                                      1 = ignore cksum result
 *
 *    Return Value: 0 = success, any other value is an error as defined in fileup.h
 *
 */
int UpdateFromFat( F_FILE* fp, BOOL bUpdateEvenIfCurrent )
{
   int i;
   int n = my_f_read_line( fat_read_buffer, 255, fp );

//         iprintf("\n n = %d \n", n);
         for(i=0; i<10; i++)
//            iprintf("%c", fat_read_buffer[i]);
//         iprintf("\n n = %d \n", n);



   if ( n <= 0 )
   {
      f_close( fp );
      return FAT_UPDATE_BAD_FORMAT;
   }

   if ( ( fat_read_buffer[0] == 'S' ) && ( fat_read_buffer[1] == '0' ) )
   {
      char* cp = fat_read_buffer;
      cp += 2;
      const char* cpt = PlatformName;

      while ( *cpt )
      {
//         iprintf("%c %c \n", *cp, *cpt);

         if ( *cp != *cpt )
         {
            f_close( fp );
            return FAT_UPDATE_WRONG_PLATFORM;
         }
         cp++;
         cpt++;
      }
   }

   int nlines = 0;
   DWORD addr;
   unsigned char* CopyTo = NULL;
   DWORD cur_pos;
   DWORD maxlen;
   BOOL bAlloced = FALSE;
   CopyTo = user_flash_buffer;
   addr = BASE_FLASH_ADDR;
   maxlen = 8192;
   cur_pos = 0;

   while ( nlines < 5 )
   {
   n = my_f_read_line( fat_read_buffer, 255, fp );
      if ( n > 0 )
      {
         if ( ProcessS3( fat_read_buffer, addr, CopyTo, cur_pos, maxlen ) != 0 )
            nlines++;
      }
   }
//   iprintf("\n Update From FAT..header read..\n", n);

   /* We have read enough lines to get the header for size */
   FlashStartUpStruct* ps = ( FlashStartUpStruct* )user_flash_buffer;

//    iprintf("\n Reached Line 199.\n", n);

   if ( ( ps->dwBlockRamStart + ps->dwExecutionAddr + ps->dwBlockSize + ps->dwSrcBlockSize + ps->dwBlockSum + ps->dwStructSum ) != 0x4255524E )
   {
     f_close( fp );
      return FAT_UPDATE_BAD_FORMAT;
   }
//    iprintf("\n Reached Line 206.\n", n);

   DWORD siz = ps->dwSrcBlockSize + 28;
   PBYTE pb = ( PBYTE )malloc( siz );
//    iprintf("\n Reached Line 210. siz = %ld\n", siz);
   if ( pb == NULL )
   {
//     iprintf("\n Reached Line 212.\n", n);
     f_close( fp );
      return FAT_UPDATE_NO_MEM;
   }
   /* Now copy what we have */
   memcpy( pb, user_flash_buffer, cur_pos );
   CopyTo = pb;
   bAlloced = TRUE;
   maxlen = siz;

//     iprintf("\n,,,,,, Reached Line 223.\n" );

   while ( n > 0 )
   {
   n = my_f_read_line( fat_read_buffer, 255, fp );
      if ( n > 0 )
      {
        if ( ProcessS3( fat_read_buffer, addr, CopyTo, cur_pos, maxlen ) != 0 )
            nlines++;
      }
   }
//     iprintf("\n,,,,,, Reached Line 234.\n");
//     iprintf("\ncur_pos = %d, maxlen = %d \n", cur_pos, maxlen);

   if ( cur_pos == ( maxlen - 4 ) )
   {
//    iprintf("\n Reached Line 239.\n" );
      if ( !bUpdateEvenIfCurrent )
      {
         PBYTE pbis = ( BYTE* )BASE_FLASH_ADDR;
         PBYTE psb = CopyTo;
         if ( memcmp( pbis, psb, cur_pos ) == 0 )
         {
            if ( bAlloced )
            {
               free( CopyTo );
            }
            f_close( fp );
            return FAT_UPDATE_SAMEVERSION;
         }
      }
//    iprintf("\n Reached Line 246.\n" );

      USER_ENTER_CRITICAL();
      FlashErase( ( void* )addr, cur_pos );
      FlashProgram( ( void* )addr, ( void* )CopyTo, cur_pos );
      USER_EXIT_CRITICAL();
      if ( bAlloced )
      {
         free( CopyTo );
      }

      f_close( fp );
      return 0;
   }
//    iprintf("\n Reached Line 266.\n" );

   if ( bAlloced )
   {
      free( CopyTo );
   }

   f_close( fp );
   return FAT_UPDATE_BAD_FORMAT;
}


