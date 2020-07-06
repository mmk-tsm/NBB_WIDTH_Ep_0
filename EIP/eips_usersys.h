/*
 *            Copyright (c) 2002-2009 by Real Time Automation, Inc.
 *
 *  This software is copyrighted by and is the sole property of
 *  Real Time Automation, Inc. (RTA).  All rights, title, ownership,
 *  or other interests in the software remain the property of RTA.
 *  This software may only be used in accordance with the corresponding
 *  license agreement.  Any unauthorized use, duplication, transmission,
 *  distribution, or disclosure of this software is expressly forbidden.
 *
 *  This Copyright notice MAY NOT be removed or modified without prior
 *  written consent of RTA.
 *
 *  RTA reserves the right to modify this software without notice.
 *
 *  Real Time Automation
 *  150 S. Sunny Slope Road            USA 262.439.4999
 *  Suite 130                          http://www.rtaautomation.com
 *  Brookfield, WI 53005               software@rtaautomation.com
 *
 *************************************************************************
 *
 *    Version Date: 12/16/2009
 *         Version: 2.14
 *    Conformed To: EtherNet/IP Protocol Conformance Test A-7 (17-AUG-2009)
 *     Module Name: eips_usersys.h
 *          Author: Jamin D. Wendorf (jwendorf@rtaautomation.com)
 *        Language: Ansi C
 * Compile Options: N/A
 * Compile defines: N/A
 *       Libraries: N/A
 *    Link Options: N/A
 *
 * Description.
 * =======================================================================
 * This file contains user definitions specific to EtherNet/IP General
 * behavior.
 *
 */
// M.McKiernan			19.01.2010
// Undefined the following definitions; to remove LED and PCCC functionality.
// EIPS_PCCC_USED      1
// EIPS_NTWK_LED_USED  1
// EIPS_IO_LED_USED	   1

// M.McKiernan			11.02.2010
// Disabled RTA's debug printing, see TSM - disable debug printing - EIPS_DEBUG 0

#ifndef __EIPS_USERSYS_H__
#define __EIPS_USERSYS_H__

/*  ******************************************* */
/*           RTA "ADD ON" SUPPORT */
/*  ******************************************* */
/*  we are using this service (comment out if we aren't) */

/* define if PCCC is supported */
//#define EIPS_PCCC_USED      1

//TSM no EIP LED's
//#if PLATFORM==CB34-EX
#if 0
	// define if Network Status LED is supported //
	#define EIPS_NTWK_LED_USED  1

	// define if I/O Status LED is supported //
	#define EIPS_IO_LED_USED    1
#endif

/* define if EIP Client is supported */
    /* #define EIPC_CLIENT_USED    1 */

/* define if the Connection Configuration Object Clas 0xF3 is supported */
    /* #define EIPC_CCO_USED       1 */

/* include files needed for the sample application */

/* include files needed for the sample application */
//#include <time.h>
#include "predef.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <startnet.h>
#include <serial.h>
#include <iosys.h>
#include <utils.h>
#include <ip.h>
#include <tcp.h>
#include <udp.h>
#include <autoupdate.h>
#include <ucos.h>

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
  #define NULLPTR NULL
#endif

/* we put lookup tables in code space to save RAM */
#define EIPS_CODESPACE const

/* --------------------------------------------------------------- */
/*      MACROS                                                     */
/* --------------------------------------------------------------- */
#define Xmod(v,b) (v - ((v/b)*b))
#define RTA_UNUSED_PARAM(a) if(a){}

#define EIPS_GET_MUTEX
#define EIPS_PUT_MUTEX

/* define macros to get the high and low bytes of a word */
#define EIPS_LO(l) (uint8)(l&0x00FF)
#define EIPS_HI(h) (uint8)((h&0xFF00)>>8)

#define RTA_MAX(a,b) (a>b?a:b)
#define RTA_MIN(a,b) (a<b?a:b)

/* Define the amount of data that can sent in a single
   explicit message. */
#define EIPS_USER_MSGRTR_DATA_SIZE     512

/* Define the number of path words included in a single
   explicit message.  This should be at least 10. */
#define EIPS_USER_MSGRTR_PATH_WORDS    16

/* Define the number of extended error words included in
   a single explicit message.  This should be at least 2. */
#define EIPS_USER_MSGRTR_EXTSTAT_SIZE  16

/* Define the max size of the encapsulated message.  This should
   be at least 600, but can go up to 64K based on the needs of
   the customer.  The NOP command is the only command that is
   supported that can be larger than 600 bytes, but the stack
   handles the NOP in a special fashion to save on RAM. */
#define EIPS_USER_MAX_ENCAP_BUFFER_SIZ 600

/* Define the max number of I/O bytes that can be transmitted in
   each direction. */
#define EIPS_USER_IOBUF_SIZ 600

/* Define the max number of I/O connections supported.	This
   can be 0 to (EIPS_USER_MAX_NUM_EM_CNXNS_PER_SESSION *
   EIPS_USER_MAX_NUM_EIP_SESSIONS). */
#define EIPS_USER_MAX_NUM_IO_CNXNS 10

/* Define the max number of Encapsulation Sessions supported
   by the device.  This should be smaller than the total
   number of sockets supported. */
#define EIPS_USER_MAX_NUM_EIP_SESSIONS 10

/* Define the max number of Explicit connections
   supported by the server. */
#define EIPS_USER_MAX_NUM_EM_CNXNS_PER_SESSION 2

/* define the resolution of the ticker in in microseconds.
	    1L - one microsecond ticker
	 1000L - one millisecond ticker
	10000L - ten millisecond ticker
      1000000L - one second ticker
*/
#define EIPS_USER_MIN_TICK_RES_IN_USEC	10000l		/*  10ms min */
#define EIPS_USER_TICK_RES_IN_USECS (1000000L/TICKS_PER_SECOND) /*  us in 1 tick */
#define EIPS_USER_TICKS_PER_1SEC    TICKS_PER_SECOND     /*  ticks per second */
#define EIPS_USER_TICKS_PER_10SEC   (10*EIPS_USER_TICKS_PER_1SEC)

/* RTA depends on a timer subsystem with at least 2 timers for each
   connection.	This number should be large since the user can use the
   timers too. */
#define EIPS_USER_MAX_NUM_TIMERS    260

/* we need a semaphore to ensure we don't print too fast */
// RTA as was.
//#define EIPS_DEBUG 1 /* define this to 1 to enable debug printing */
//#define EIPS_PRINTALL 1
// TSM - disable debug printing.
#define EIPS_DEBUG 0 /* define this to 1 to enable debug printing */
#define EIPS_PRINTALL 0
/*
    define EIPS_PRINTALL to the following...
        > 0 show assembly configuration print at start up
        > 1 show all TCP and UDP traffic
        > 2 show all Forward Open and Forward Close Messages
*/
#if EIPS_DEBUG > 0
  #define eips_user_dbprint0(f) printf(f)
  #define eips_user_dbprint1(f,a1) printf(f,a1)
  #define eips_user_dbprint2(f,a1,a2) printf(f,a1,a2)
  #define eips_user_dbprint3(f,a1,a2,a3) printf(f,a1,a2,a3)
  #define eips_user_dbprint4(f,a1,a2,a3,a4) printf(f,a1,a2,a3,a4)
  #define eips_user_dbprint5(f,a1,a2,a3,a4,a5) printf(f,a1,a2,a3,a4,a5)
  #define eips_user_dbprint6(f,a1,a2,a3,a4,a5,a6) printf(f,a1,a2,a3,a4,a5,a6)
#else
  #define eips_user_dbprint0(f)
  #define eips_user_dbprint1(f,a1)
  #define eips_user_dbprint2(f,a1,a2)
  #define eips_user_dbprint3(f,a1,a2,a3)
  #define eips_user_dbprint4(f,a1,a2,a3,a4)
  #define eips_user_dbprint5(f,a1,a2,a3,a4,a5)
  #define eips_user_dbprint6(f,a1,a2,a3,a4,a5,a6)
#endif

#endif /* __EIPS_USERSYS_H__ */
