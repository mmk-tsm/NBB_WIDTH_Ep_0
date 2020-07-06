/*********************************************************************
Module: RTA useful Functions (rta_utils.c)
Author: Jamin D. Wendorf     10/2002
        (c)2002 Real Time Automation

This module implements some useful functions used by the application.

NOTE: Since these functions may be called by multiple tasks, all
      functions must be re-entrant!

These functions assume the processor is Little-Endian. If Big-Endian
needs to be used, the user needs to define "RTA_BIG_ENDIAN" in the
compile options.
***********************************************************************/

/*------------------------------*/
/* INCLUDE FILES                */
/*------------------------------*/
#include <string.h>
#include "rta_system.h"

/************************************************************************/
/*  GLOBAL FUNCTIONS (all must be re-entrant)                           */
/************************************************************************/
/* ====================================================================
Function:     rta_ByteMove
Parameters:   destination pointer
              source pointer
              length of bytes to move
Returns:      N/A

This function moves n bytes from the src pointer to the destination
pointer.
======================================================================= */
void rta_ByteMove (uint8 *dst, uint8 *src, uint16 len)
{
   memmove(dst,src,len);
}

/*=======================================================================
Function:   rta_rta_PutBigEndian16
Parameters: 16 bit number to store
            pointer to where to store the Big-Endian data
Returns:    N/A

This function stores the passed value (in Big-Endian) to the location
pointed to by the passed pointer.
=========================================================================*/
void rta_PutBigEndian16 (uint16 value, uint8 *pos)
{
   /* Big-Endian */
   #ifdef RTA_BIG_ENDIAN
      pos[0] = (uint8)((value>>0) & 0x00FF);
      pos[1] = (uint8)((value>>8) & 0x00FF);

   /* Little-Endian */
   #else
      pos[0] = (uint8)((value>>8) & 0x00FF);
      pos[1] = (uint8)((value>>0) & 0x00FF);
   #endif
}

/*=======================================================================
Function:   rta_PutBigEndian32
Parameters: 32 bit number to store
            pointer to where to store the Big-Endian data
Returns:    N/A

This function stores the passed value (in Big-Endian) to the location
pointed to by the passed pointer.
=========================================================================*/
void rta_PutBigEndian32 (uint32 value, uint8 *pos)
{
   /* Big-Endian */
   #ifdef RTA_BIG_ENDIAN
      pos[0] = (uint8)((value>> 0) & 0x000000FFL);
      pos[1] = (uint8)((value>> 8) & 0x000000FFL);
      pos[2] = (uint8)((value>>16) & 0x000000FFL);
      pos[3] = (uint8)((value>>24) & 0x000000FFL);

   /* Little-Endian */
   #else
      pos[3] = (uint8)((value>> 0) & 0x000000FFL);
      pos[2] = (uint8)((value>> 8) & 0x000000FFL);
      pos[1] = (uint8)((value>>16) & 0x000000FFL);
      pos[0] = (uint8)((value>>24) & 0x000000FFL);
   #endif
}

/*=======================================================================
Function:   rta_PutLitEndian16
Parameters: 16 bit number to store
            pointer to where to store the Little-Endian data
Returns:    N/A

This function stores the passed value (in Little-Endian) to the location
pointed to by the passed pointer.
=========================================================================*/
void rta_PutLitEndian16 (uint16 value, uint8 *pos)
{
   /* Big-Endian */
   #ifdef RTA_BIG_ENDIAN
      pos[1] = (uint8)((value>>0) & 0x00FF);
      pos[0] = (uint8)((value>>8) & 0x00FF);

   /* Little-Endian */
   #else
      pos[0] = (uint8)((value>>0) & 0x00FF);
      pos[1] = (uint8)((value>>8) & 0x00FF);
   #endif
}

/*=======================================================================
Function:   rta_PutLitEndian32
Parameters: 32 bit number to store
            pointer to where to store the Little-Endian data
Returns:    N/A

This function stores the passed value (in Little-Endian) to the location
pointed to by the passed pointer.
=========================================================================*/
void rta_PutLitEndian32 (uint32 value, uint8 *pos)
{
   /* Big-Endian */
   #ifdef RTA_BIG_ENDIAN
      pos[3] = (uint8)((value>> 0) & 0x000000FFL);
      pos[2] = (uint8)((value>> 8) & 0x000000FFL);
      pos[1] = (uint8)((value>>16) & 0x000000FFL);
      pos[0] = (uint8)((value>>24) & 0x000000FFL);

   /* Little-Endian */
   #else
      pos[0] = (uint8)((value>> 0) & 0x000000FFL);
      pos[1] = (uint8)((value>> 8) & 0x000000FFL);
      pos[2] = (uint8)((value>>16) & 0x000000FFL);
      pos[3] = (uint8)((value>>24) & 0x000000FFL);
   #endif
}

/*=======================================================================
Function:   rta_rta_GetBigEndian16
Parameters: pointer where the Big-Endian data is stored
Returns:    16 bit number

This function returns the value (in Big-Endian) of the passed pointer.
=========================================================================*/
uint16 rta_GetBigEndian16 (uint8 *pos)
{
   uint16 return_val;

   /* Big-Endian */
   #ifdef RTA_BIG_ENDIAN
      return_val = pos[1];
      return_val = (return_val << 8) | pos[0];

   /* Little-Endian */
   #else
      return_val = pos[0];
      return_val = (return_val << 8) | pos[1];
   #endif

   return(return_val);
}

/*=======================================================================
Function:   rta_GetBigEndian32
Parameters: pointer where the Big-Endian data is stored
Returns:    32 bit number

This function returns the value (in Big-Endian) of the passed pointer.
=========================================================================*/
uint32 rta_GetBigEndian32 (uint8 *pos)
{
   uint32 return_val;

   /* Big-Endian */
   #ifdef RTA_BIG_ENDIAN
      return_val = pos[3];
      return_val = (return_val << 8) | pos[2];
      return_val = (return_val << 8) | pos[1];
      return_val = (return_val << 8) | pos[0];

   /* Little-Endian */
   #else
      return_val = pos[0];
      return_val = (return_val << 8) | pos[1];
      return_val = (return_val << 8) | pos[2];
      return_val = (return_val << 8) | pos[3];
   #endif

   return(return_val);
}

/*=======================================================================
Function:   rta_GetLitEndian16
Parameters: pointer where the Little-Endian data is stored
Returns:    16 bit number

This function returns the value (in Little-Endian) of the passed pointer.
=========================================================================*/
uint16 rta_GetLitEndian16 (uint8 *pos)
{
   uint16 return_val;

   /* Big-Endian */
   #ifdef RTA_BIG_ENDIAN
      return_val = pos[0];
      return_val = (return_val << 8) | pos[1];

   /* Little-Endian */
   #else
      return_val = pos[1];
      return_val = (return_val << 8) | pos[0];
   #endif

   return(return_val);
}

/*=======================================================================
Function:   rta_GetLitEndian32
Parameters: pointer where the Little-Endian data is stored
Returns:    32 bit number

This function returns the value (in Little-Endian) of the passed pointer.
=========================================================================*/
uint32 rta_GetLitEndian32 (uint8 *pos)
{
   uint32 return_val;

   /* Big-Endian */
   #ifdef RTA_BIG_ENDIAN
      return_val = pos[0];
      return_val = (return_val << 8) | pos[1];
      return_val = (return_val << 8) | pos[2];
      return_val = (return_val << 8) | pos[3];

   /* Little-Endian */
   #else
      return_val = pos[3];
      return_val = (return_val << 8) | pos[2];
      return_val = (return_val << 8) | pos[1];
      return_val = (return_val << 8) | pos[0];
   #endif

   return(return_val);
}

/* *********** */
/* END OF FILE */
/* *********** */
