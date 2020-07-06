/*********************************************************************
Module:  mbtcp_usersys.c
Author:  Jamin D. Wendorf     12/2003
         (c)2003 Real Time Automation

This file contains system functions that need to be written by the user.
This includes the fatal error function.
*********************************************************************** */

// M.McKiernan		30.4.2010
// Edited fatal error function mbtcp_usersys_fatalError (char *function_name, int16 error_num)
// to close down MBTCP and return. - calls mbtcp_usersock_closeall()
// Added function FGOneHzMBTCPErrorChecking(void ).

/* ---------------------------- */
/* INCLUDE FILES                */
/* ---------------------------- */
#include "rta_system.h"
#include "mbtcp_system.h"
#include "LiWVars.h"
#include "Batvars.h"
#include "Eventlog.h"
/* ---------------------------- */
/* EXTERN FUNCTIONS             */
/* ---------------------------- */

/* ---------------------------- */
/* LOCAL STRUCTURE DEFINITIONS  */
/* ---------------------------- */

/* ---------------------------- */
/* STATIC VARIABLES             */
/* ---------------------------- */

// global variables.
bool g_bMBTCPFatalErrorOccurredFlag = FALSE;
int g_nMBTCPRestartLag = 0;
extern bool g_bSimulateMBTCPFatalError;
int g_nMBTCPProcessHoldoffTime = 0;
//bool g_bMBTCPInitialisedFlag = FALSE;		// use to stop call of MBTCP process.

/* ---------------------------- */
/* LOCAL FUNCTIONS              */
/* ---------------------------- */
extern void mbtcp_usersock_closeall (void);
/* ---------------------------- */
/* MISCELLANEOUS                */
/* ---------------------------- */

/**/
/* ******************************************************************** */
/*               GLOBAL FUNCTIONS CALLED BY RTA                         */
/* ******************************************************************** */
/* ====================================================================
Function:     mbtcp_usersys_init
Parameters:   N/A
Returns:      N/A

This function initialize all user system variables.
======================================================================= */
void mbtcp_usersys_init (void)
{
}

/* ====================================================================
Function:     mbtcp_usersys_fatalError
Parameters:   N/A
Returns:      N/A

This function handles fatal errors.
======================================================================= */
void mbtcp_usersys_fatalError (char *function_name, int16 error_num)
{
    printf("FATAL ERROR: \"%s\" Error: %d\n",function_name, error_num);
    g_bMBTCPFatalErrorOccurredFlag = TRUE;
//    g_bMBTCPInitialisedFlag = FALSE;

    mbtcp_usersock_closeall();		//TSM function
    OSTimeDly(1);
    return;
/*    while(1)
    {
        OSTimeDly(100);
    }
*/
}
// Routine to call in foreground once per second.
void FGOneHzMBTCPErrorChecking(void )
{
	//TESTONLY
	g_bSimulateMBTCPFatalError = FALSE;  //nbb--todo--testonly--
	if( g_bSimulateMBTCPFatalError )
	{
		g_bSimulateMBTCPFatalError = FALSE;
		mbtcp_usersys_fatalError("mbtcp_only_testing",1);
	}
	if(g_bMBTCPFatalErrorOccurredFlag && g_nMBTCPRestartLag==0 )
	{
//		g_bMBTCPFatalErrorOccurredFlag = FALSE;	// clear error flag.

		g_nMBTCPRestartLag = 10;	// set to restart MBTCP in 10 seconds.
		AddModbusTCPFatalErrorToLog();
	}
	if(g_nMBTCPProcessHoldoffTime)
	{
		g_nMBTCPProcessHoldoffTime--;
	}

	if(g_nMBTCPRestartLag)
	{
		g_nMBTCPRestartLag--;
		if(fdTelnet>0)
			iprintf("ModbusTCP Server Halted. Will re-start in %d seconds \r\n", g_nMBTCPRestartLag );

		if(g_nMBTCPRestartLag==0)
		{
			// re-initialise MBTCP
		       // call the MBTCP initialization code (RTA's init routines)
				if(fdTelnet>0)
					iprintf("ModbusTCP Server Started (V%s)\r\n",MBTCP_VERSION_STRING);
			   g_bMBTCPFatalErrorOccurredFlag = FALSE;	// clear fatal error flag.

		       mbtcp_rtasys_init();

		       g_nMBTCPProcessHoldoffTime = 3;			// will hold off process for 3 seconds.
		       //OSTimeDly( 1 );
		       //g_bMBTCPInitialisedFlag = TRUE;

		}
	}
}






/**/
/* ******************************************************************** */
/*               LOCAL FUNCTIONS                                        */
/* ******************************************************************** */

/* *********** */
/* END OF FILE */
/* *********** */
