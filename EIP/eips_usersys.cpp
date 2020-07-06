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
 *     Module Name: eips_usersys.c
 *          Author: Jamin D. Wendorf (jwendorf@rtaautomation.com)
 *        Language: Ansi C
 * Compile Options: N/A
 * Compile defines: N/A
 *       Libraries: N/A
 *    Link Options: N/A
 *
 * Description.
 * =======================================================================
 * This file contains system functions that need to be written by the user.
 * This includes timer, NVRAM, and task calls.
 *
 */

// M.McKiernan			19.01.2010
// Replace #if PLATFORM==CB34-EX by #if 0, ie. remove whole pile of stuff relating to LED task.
// Leave functions by moving outside #if nesting;
// 		void eips_usersys_init (uint8 init_type)
//		void eips_usersys_process (void)
// 		void eips_usersys_fatalError (char *function_name, int16 error_num)
//		uint16 eips_usersys_getIncarnationID (void)
//		void UpdateLEDTask( void* args ) - though now only a dummy function.
//

// M.McKiernan			11.02.2010
// Edited eips_usersys_fatalError () function - dont just sit there.
// PutAlarmTable( ETHERNETIP_ALARM, 0);	// Flag an alarm
// Set global g_bEIPSoftwareFatalErrorOccurred = TRUE;	- this will stop eips_rtasys_process(local_get_ticks_passed()); - see main.cpp
// gEIPTaskErrorFlag is checked in other tasks (UDP tasks in eips_usersock.cpp)
// Sets an OS_FLAGS variable gEIPTaskErrorFlag see    OSFlagSet()   used  to shutdown the UDP tasks.


//
//
/* ---------------------------- */
/* INCLUDE FILES                */
/* ---------------------------- */
#include "eips_system.h"
#include "general.h"
#include <Bsp.h>
#include "Alarms.h"

extern bool g_bEIPSoftwareFatalErrorOccurred;
extern OS_FLAGS gEIPTaskErrorFlag;		//used for shutting down UDP tasks in EthernetIP.


//#if PLATFORM==CB34-EX

#if 0

/* ---------------------------- */
/* EXTERN FUNCTIONS             */
/* ---------------------------- */

/* ---------------------------- */
/* LOCAL STRUCTURE DEFINITIONS  */
/* ---------------------------- */

/* ---------------------------- */
/* STATIC VARIABLES             */
/* ---------------------------- */
uint8 led1_enable = 0;
uint8 led1_on_color = 0;
uint8 led1_off_color = 0;
uint8 led2_enable = 0;
uint8 led2_on_color, led2_off_color;

/* ---------------------------- */
/* EXTERN VARIABLES             */
/* ---------------------------- */

/* ---------------------------- */
/* LOCAL FUNCTIONS              */
/* ---------------------------- */
void ledtask_update_led1 (uint8 enable, uint8 on_color, uint8 off_color);
void ledtask_update_led2 (uint8 enable, uint8 on_color, uint8 off_color);
void local_led_update (uint8 led, uint8 color);

/* ---------------------------- */
/* MISCELLANEOUS                */
/* ---------------------------- */
#define RTA_LED_NS	0
#define RTA_LED_IO	1

#define RTA_LED_ORG	0
#define RTA_LED_RED	1
#define RTA_LED_GRN	2
#define RTA_LED_OFF	3

#define LED_1	0
#define LED_2	1


/* ====================================================================
Function:   eips_usersys_ledTest
Parameters: N/A
Returns:    N/A

   Turn first indicator Green, all other indicators off
   Leave first indicator on Green for approximately 0.25 second
   Turn first indicator on Red for approximately 0.25 second
   Turn first indicator on Green
   Turn second indicator (if present) on Green for approx. 0.25 second
   Turn second indicator (if present) on Red for approx. 0.25 second
   Turn second indicator (if present) Off

If other indicators are present, test each indicator in sequence as
prescribed by the second indicator above. If a Module Status indicator
is present, it shall be the first indicator in the sequence, followed
by any Network Status indicators present.

After completion of this power up test, the indicator(s) shall turn
to a normal operational state (all OFF).
======================================================================= */
#if EIPS_NTWK_LED_USED || EIPS_IO_LED_USED
void eips_usersys_ledTest (void)
{
    static uint8 first_time = 1;

    eips_user_dbprint0("eips_usersys_ledTest\r\n");

    /* don't run the first time since LED test is handled in main */
    if(first_time)
    {
        first_time = 0;
        return;
    }

	/* NS Green for 250MS */
    ledtask_update_led1(0, RTA_LED_GRN, RTA_LED_OFF);
    OSTimeDly(TICKS_PER_SECOND/4);

	/* NS Red for 250MS */
    ledtask_update_led1(0, RTA_LED_RED, RTA_LED_OFF);
    OSTimeDly(TICKS_PER_SECOND/4);

	/* NS Green */
    ledtask_update_led1(0, RTA_LED_GRN, RTA_LED_OFF);

	/* IO Green for 250MS */
    ledtask_update_led2(0, RTA_LED_GRN, RTA_LED_OFF);
    OSTimeDly(TICKS_PER_SECOND/4);

	/* IO Red for 250MS */
    ledtask_update_led2(0, RTA_LED_RED, RTA_LED_OFF);
    OSTimeDly(TICKS_PER_SECOND/4);

	/* IO OFF */
    ledtask_update_led2(0, RTA_LED_OFF, RTA_LED_OFF);
}
#endif

/* ====================================================================
Function:   eips_usersys_nsLedUpdate
Parameters: led state (see eips_cnxn.h)
Returns:    N/A

This function controls the Network LED.
======================================================================= */
#ifdef EIPS_NTWK_LED_USED
void eips_usersys_nsLedUpdate (uint8 led_state)
{
    // switch on the state
    switch(led_state)
    {
        // switch on the valid LED states
        case EIPS_LEDSTATE_OFF:
//            eips_user_dbprint0("NS LED: Off\r\n");
            ledtask_update_led1(1, RTA_LED_OFF,RTA_LED_OFF);
            break;
        case EIPS_LEDSTATE_FLASH_GREEN:
//            eips_user_dbprint0("NS LED: Flash Green\r\n");
            ledtask_update_led1(1, RTA_LED_GRN,RTA_LED_OFF);
            break;
        case EIPS_LEDSTATE_FLASH_RED:
//             eips_user_dbprint0("NS LED: Flash Red\r\n");
            ledtask_update_led1(1, RTA_LED_RED,RTA_LED_OFF);
            break;
        case EIPS_LEDSTATE_STEADY_GREEN:
//            eips_user_dbprint0("NS LED: Steady Green\r\n");
            ledtask_update_led1(1, RTA_LED_GRN,RTA_LED_GRN);
            break;
        case EIPS_LEDSTATE_STEADY_RED:
//            eips_user_dbprint0("NS LED: Steady Red\r\n");
            ledtask_update_led1(1, RTA_LED_RED,RTA_LED_RED);
            break;
        case EIPS_LEDSTATE_ALT_REDGREEN:
            ledtask_update_led1(1, RTA_LED_RED,RTA_LED_GRN);
            break;
        // error
        default:
            eips_user_dbprint0("NTWK LED: ERR\r\n");
            break;
    };
}
#endif

/* ====================================================================
Function:   eips_usersys_ioLedUpdate
Parameters: led state (see eips_cnxn.h)
Returns:    N/A

This function controls the I/O LED.
======================================================================= */
#ifdef EIPS_IO_LED_USED
void eips_usersys_ioLedUpdate (uint8 led_state)
{
    // switch on the state
    switch(led_state)
    {
        // switch on the valid LED states
        case EIPS_LEDSTATE_OFF:
//            eips_user_dbprint0("IO LED: Off\r\n");
            ledtask_update_led2(1, RTA_LED_OFF,RTA_LED_OFF);
            break;
        case EIPS_LEDSTATE_FLASH_GREEN:
//            eips_user_dbprint0("IO LED: Flash Green\r\n");
            ledtask_update_led2(1, RTA_LED_GRN,RTA_LED_OFF);
            break;
        case EIPS_LEDSTATE_FLASH_RED:
//             eips_user_dbprint0("IO LED: Flash Red\r\n");
            ledtask_update_led2(1, RTA_LED_RED,RTA_LED_OFF);
            break;
        case EIPS_LEDSTATE_STEADY_GREEN:
//            eips_user_dbprint0("IO LED: Steady Green\r\n");
            ledtask_update_led2(1, RTA_LED_GRN,RTA_LED_GRN);
            break;
        case EIPS_LEDSTATE_STEADY_RED:
//            eips_user_dbprint0("IO LED: Steady Red\r\n");
            ledtask_update_led2(1, RTA_LED_RED,RTA_LED_RED);
            break;
        case EIPS_LEDSTATE_ALT_REDGREEN:
            ledtask_update_led2(1, RTA_LED_RED,RTA_LED_GRN);
            break;
        // error
        default:
            eips_user_dbprint0("NTWK LED: ERR\r\n");
            break;
    };
}
#endif



/**/
/* ******************************************************************** */
/*		                LOCAL FUNCTIONS		                            */
/* ******************************************************************** */
/* ====================================================================
Function:   N/A
Parameters: N/A
Returns:    N/A

This function
======================================================================= */
static uint8 last_led = 0xFF;
void local_led_update (uint8 led, uint8 color)
{
    uint8 led_color = 0;

    switch(color)
    {
        case RTA_LED_OFF:
        case RTA_LED_RED:
        case RTA_LED_GRN:
        case RTA_LED_ORG:
            led_color = color;
            break;
        default:
            return;
    };

    switch(led)
    {
        case LED_1:
            last_led = ((last_led & 0xC0) | (led_color << 4));
            break;
        case LED_2:
            last_led = ((last_led & 0x30) | (led_color << 6));
            break;
    };

    putleds(last_led);
}

void ledtask_update_led1 (uint8 enable, uint8 on_color, uint8 off_color)
{
    led1_enable = enable;
    if(enable)
    {
        led1_on_color = on_color;
        led1_off_color = off_color;
    }
    else
    {
        local_led_update(LED_1, on_color);
    }
}

void ledtask_update_led2 (uint8 enable, uint8 on_color, uint8 off_color)
{
    led2_enable = enable;
    if(enable)
    {
        led2_on_color = on_color;
        led2_off_color = off_color;
    }
    else
    {
        local_led_update(LED_2, on_color);
    }
}

/* ====================================================================
Function:   UpdateLEDTask
Parameters: ignored
Returns:    N/A

This function starts the LED task and performs the LED test.
======================================================================= */
void UpdateLEDTask( void* args )
{
	/* NS Green for 250MS */
    ledtask_update_led1(0, RTA_LED_GRN, RTA_LED_OFF);
    OSTimeDly(TICKS_PER_SECOND/4);

	/* NS Red for 250MS */
    ledtask_update_led1(0, RTA_LED_RED, RTA_LED_OFF);
    OSTimeDly(TICKS_PER_SECOND/4);

	/* NS Green */
    ledtask_update_led1(0, RTA_LED_GRN, RTA_LED_OFF);

	/* IO Green for 250MS */
    ledtask_update_led2(0, RTA_LED_GRN, RTA_LED_OFF);
    OSTimeDly(TICKS_PER_SECOND/4);

	/* IO Red for 250MS */
    ledtask_update_led2(0, RTA_LED_RED, RTA_LED_OFF);
    OSTimeDly(TICKS_PER_SECOND/4);

	/* IO and NS Flash Green */
    ledtask_update_led1(1, RTA_LED_GRN, RTA_LED_OFF);
    ledtask_update_led2(1, RTA_LED_GRN, RTA_LED_OFF);

    for( ;; )
    {
        /* *************************** */
        /* LED 1 PROCESSING - ON COLOR */
        /* *************************** */
		if(led1_enable)
            local_led_update(LED_1, led1_on_color);

        /* *************************** */
        /* LED 2 PROCESSING - ON COLOR */
        /* *************************** */
		if(led2_enable)
            local_led_update(LED_2, led2_on_color);

        /* **************************** */
        /* DELAY 500MS (1Hz Flash rate) */
        /* **************************** */
        OSTimeDly( TICKS_PER_SECOND / 2 );

        /* **************************** */
        /* LED 1 PROCESSING - OFF COLOR */
        /* **************************** */
		if(led1_enable)
	        local_led_update(LED_1, led1_off_color);

        /* **************************** */
        /* LED 2 PROCESSING - OFF COLOR */
        /* **************************** */
		if(led2_enable)
            local_led_update(LED_2, led2_off_color);

        /* **************************** */
        /* DELAY 500MS (1Hz Flash rate) */
        /* **************************** */
        OSTimeDly( TICKS_PER_SECOND / 2 );
  } // end of for
}

#endif

/**/
/* ******************************************************************** */
/*                  GLOBAL FUNCTIONS CALLED BY RTA                      */
/* ******************************************************************** */
/* ====================================================================
Function:   eips_usersys_init
Parameters: init type
Returns:    N/A

This function initialize all user system variables.
======================================================================= */
void eips_usersys_init (uint8 init_type)
{
    /* different initialization based on passed parameters (if needed) */
    switch (init_type)
    {
        /* Out of Box Initialization */
        case EIPSINIT_OUTOFBOX:
	        break;

        /* Normal Initialization */
        case EIPSINIT_NORMAL:
        default:
	        break;
    };
}

/* ====================================================================
Function:   eips_usersys_process
Parameters: N/A
Returns:    N/A

This function handles any user system processing.
======================================================================= */
void eips_usersys_process (void)
{
    /* do nothing */
}

/* ====================================================================
Function:   eips_usersys_fatalError
Parameters: N/A
Returns:    N/A

This function handles fatal errors.
======================================================================= */
void eips_usersys_fatalError (char *function_name, int16 error_num)
{
//    eips_user_dbprint2("FATAL ERROR: \"%s\" Error: %d\n",function_name, error_num);

    PutAlarmTable( ETHERNETIP_ALARM, 0);	// Flag an alarm

    /* we shouldn't get here, but make sure we don't return */
/*    TSM -cant just sit here
    for(;;)
    {
//    	ledtask_update_led1(0, RTA_LED_RED, RTA_LED_OFF);
//	    ledtask_update_led2(0, RTA_LED_RED, RTA_LED_OFF);
		OSTimeDly(1);
    }
*/
    // gEIPTaskErrorFlag is checked in other tasks (UDP tasks in eips_usersock.cpp)
    OSFlagSet( &gEIPTaskErrorFlag, 0x00000001 );	//set b0 to shutdown the UDP tasks.
    g_bEIPSoftwareFatalErrorOccurred = TRUE;	// this will stop eips_rtasys_process(local_get_ticks_passed()); - see main.cpp

/*
    //todo - for now, wait 5 seconds and reset the NBB.
	 while(i<5)
    {
    	OSTimeDly(TICKS_PER_SECOND);
    	i++;
    }
    ForceReboot();	// force a reset.
*/

}

/* ====================================================================
Function:   eips_usersys_getIncarnationID
Parameters: N/A
Returns:    16-bit number

This function generates the Incarnation ID.
======================================================================= */
uint16 eips_usersys_getIncarnationID (void)
{
    return((uint16)rand());
}
//Dummy version - to allow compile.
void UpdateLEDTask( void* args )
{
    /* do nothing */
	OSTimeDly(1);
}

/* *********** */
/* END OF FILE */
/* *********** */
