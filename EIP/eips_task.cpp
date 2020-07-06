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
 *     Module Name: eips_task.c
 *          Author: Jamin D. Wendorf (jwendorf@rtaautomation.com)
 *        Language: Ansi C
 * Compile Options: N/A
 * Compile defines: N/A
 *       Libraries: N/A
 *    Link Options: N/A
 *
 * Description.
 * =======================================================================
 * This file contains the EIPS Task.
 *
 */

// M.McKiernan			19.01.2010
 //	Renamed current to   g_nCurrentTick
 //	Renamed old to   g_nOldTick



/* ========================== */
/*	 INCLUDE FILES	      */
/* ========================== */
#include "eips_system.h"

/* ========================== */
/* GLOBAL FUNCTION PROTOTYPES */
/* ========================== */

/* ========================== */
/*	 GLOBAL DATA	      */
/* ========================== */
VDWORD g_nCurrentTick, g_nOldTick;
extern VDWORD TimeTick;

/* ========================= */
/* LOCAL FUNCTION PROTOTYPES */
/* ========================= */
uint32 local_get_ticks_passed (void);

/* ========================= */
/*	 LOCAL DATA	     */
/* ========================= */

/* ========================= */
/*	 MISCELLANEOUS	     */
/* ========================= */

/* NEW PAGE */
/* ==================================================== */
/*		   GLOBAL FUNCTIONS			*/
/* ==================================================== */
/* ****************************************************
  Function: EipTaskMain
Parameters: pointer to unused data
   Returns: never returns

This function processes EIP.
******************************************************* */
void EipTaskMain (void * pd)
{
    eips_user_dbprint1("RTA - EIP Sample (%d Ticks Per Second)\r\n",EIPS_USER_TICKS_PER_1SEC);

    // call the initialization code (RTA's init routines)
    eips_rtasys_init();

    // get the base time
    g_nCurrentTick = TimeTick;
    g_nOldTick = g_nCurrentTick;

    // ----------------------------------------------
    //		   RUN FOREVER
    // ----------------------------------------------
    for(;;)
    {
        /* process the state */
        eips_rtasys_process(local_get_ticks_passed());
        OSTimeDly(1);
    }

    eips_user_dbprint0("RTA - You should never get here\r\n");
}

/* ****************************************************
  Function: local_get_ticks_passed
Parameters: N/A
   Returns: ticks that passed since the last call

This function returns the number of ticks that passed
since the last call to this function.
******************************************************* */
uint32 local_get_ticks_passed (void)
{
   uint32 return_val;

   // see how much time has passed
   g_nCurrentTick = TimeTick;

   // we rolled over, ignore g_nOldTick to the end, just use the g_nCurrentTick
   if(g_nOldTick > g_nCurrentTick)
   {
      return_val = (uint32)g_nCurrentTick;
   }

   // normally time passed is g_nCurrentTick minus g_nOldTick
   else
   {
      return_val = (uint32)(g_nCurrentTick - g_nOldTick);
   }

   // save the g_nCurrentTick time
   g_nOldTick = g_nCurrentTick;

   // return the number of ticks that have passed
   return(return_val);
}
/* *********** */
/* END OF FILE */
/* *********** */

