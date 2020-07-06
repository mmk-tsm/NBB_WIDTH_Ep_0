/* ********************************************************************
Module:  rta_system.h
Author:  Jamin D. Wendorf       10/02
         (c)2002 REAL TIME AUTOMATION

This file contains generic application level definitions.
*********************************************************************** */
#ifndef __RTA_SYSTEM_H__
#define __RTA_SYSTEM_H__

/* --------------------------------------------------------------- */
/*      TYPE DEFINITIONS                                           */
/* --------------------------------------------------------------- */
#ifndef int8
  #define int8   signed char
#endif

#ifndef uint8
  #define uint8  unsigned char
#endif

#ifndef int16
  #define int16  short
#endif

#ifndef uint16
  #define uint16 unsigned short
#endif

#ifndef int32
  #define int32  long
#endif

#ifndef uint32
  #define uint32 unsigned long
#endif

#ifndef NULLFNPTR
  #define NULLFNPTR ((void (*)()) 0)
#endif

/* --------------------------------------------------------------- */
/*      GENERIC DEFINITIONS                                        */
/* --------------------------------------------------------------- */
#ifndef SUCCESS
  #define SUCCESS 0
#endif

#ifndef FAILURE
  #define FAILURE 1
#endif

#ifndef FAIL
  #define FAIL 1
#endif

#ifndef TRUE
  #define TRUE 1
#endif

#ifndef FALSE
  #define FALSE 0
#endif

#ifndef YES
  #define YES 1
#endif

#ifndef NO
  #define NO 0
#endif

#ifndef ON
  #define ON 1
#endif

#ifndef OFF
  #define OFF 0
#endif

#ifndef NULL
  #define NULL 0x00
#endif

#ifndef NULLPTR
  #define NULLPTR (void *) NULL
#endif

/* --------------------------------------------------------------- */
/*      MACROS                                                     */
/* --------------------------------------------------------------- */
#define Xmod(v,b) (v - ((v/b)*b))
#define Xmin(a,b) ((a<b)?a:b)
#define Xmax(a,b) ((a>b)?a:b)
#define BYTESWAP(a)  ((uint16)((((unsigned int)a)>>8)&0x00FF)|((((uint16)a)<<8)&0xFF00))

/* --------------------------------------------------------------- */
/*      RTA STANDARD INCLUDE FILES                                 */
/* --------------------------------------------------------------- */
#include "rta_proto.h"

#endif /* __RTA_SYSTEM_H__ */
