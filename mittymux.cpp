//////////////////////////////////////////////////////
//
// P.Smith                      25/1/07
// multiplexing of Mitty port.

// P.Smith                      5/2/07
// check for g_bRedirectStdioToPort2 in SetupMttty & RestartPanel not run if redirected.
// g_nRevertToOperatorPanelCounter set to MITTYTPRINTPERIOD to allow expiry of printing.
//
// P.Smith                      27/4/07
// call InitialiseNBBUartPins on setting up Mitty, the pins are affected by the open serial command
//
// P.Smith                      30/4/07
// only allow SetupMttty to run if the g_nRevertToOperatorPanelCounter is 0
// This prevents multiple set up on this function on multiple printfs to the mitty port
//
// P.Smith                      15/5/07
// added RestartPanel1
//
// P.Smith                          30/1/08
// correct compiler warnings
//////////////////////////////////////////////////////

//*****************************************************************************
// INCLUDES
//*****************************************************************************
#include <stdio.h>
#include <basictypes.h>

#include "MBPanelSNBBHand.h"
#include "InitNBBComms.h"
#include <Serial.h>


#include "InitComms.h"
#include "MBShand.h"

#include "MBMaster.h"
#include "PrinterFunctions.h"
#include "Nbbvars.h"

#include "General.h"
#include "NBBGpio.h"

#include    "BatVars.h"
#include    "BatVars2.h"
#include    "Mtttymux.h"
#include    "NBBgpio.h"


void SetupMttty( void )
{
   if(!g_bRedirectStdioToPort2 && (g_nRevertToOperatorPanelCounter == 0))
   { 
   PanelTxRxOff();
   g_bPrintfRunning = TRUE;
   g_bEnableTxRxInOpenSerial = TRUE;
   U0_Select_Mtty();
   SerialClose(0);
   g_nRevertToOperatorPanelCounter = MITTYTPRINTPERIOD;  // time after which to revert to panel
   int fd1;
   fd1 = OpenSerial( 0, 115200, 1, DATA_BITS, eParityNone );
   InitialiseNBBUartPins();
/*
   sim.gpio.par_uart &= 0x7F0C; // b15 = 0 => irq2 function
                                // b7-4 = 0 => U1CTS and U1RTS are gpio.
                                // b1 = 0 => U0CTS is gpio
                                // b0 = 0 => U0RTS is gpio.
*/
    }
}

void RestartPanel( void )
{
   if(!g_bRedirectStdioToPort2)
   {
   g_bPrintfRunning = FALSE;
   g_bEnableTxRxInOpenSerial = FALSE;
   U0_Select_Operator_Panel();
   ResetPanelMBSlaveComms();
    }
}  

void RestartPanel1( void )
{
    g_bPrintfRunning = FALSE;
   g_bEnableTxRxInOpenSerial = FALSE;
   U0_Select_Operator_Panel();
   ResetPanelMBSlaveComms();
}  

