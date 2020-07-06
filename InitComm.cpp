///////////////////////////////////////////////////////////
// InitComm.c
//
//
// Initialise serial port A for 9600 baud, 8 data bits,
// 1 stop 1 start and no parity.
//
// NOTE:
// For an SBB Blender running Modbus L2 must be fitted
// for no parity.    We run with no parity because the
// PIC based IVR modules cannot support parity checking.
//
// initialise serial port B for 9600 baud, 8 data bits,
// 1 stop 1 start and no parity.
//
// M.Parks								24-01-2000
// Adapted from SigmaBlend assembler.
// M.Parks								13-04-2000
// Modified for the SBB 16C522.
// P.Smith								3/5/3				
// Modify to set up port B for modbus slave port		
// M.McKiernan								12-06-2003
//		InitialiseSerialPortB defined as function. - nothing changed functionally.
// M.McKiernan								13-10-2003
// Call ReadSBBLinkOptions() and initialise Serial port B, for no/even parity.
//
// M.McKiernan								24-01-2005
// Changed the baud rate on port A to 19k2 - see BAUD_19200_

// M.McKiernan					19-12-2005 - Transported across for NBB.
//                                      Incorporated RxBOn().
// Following defined to allow compile. g_bSBBL2Fitted; g_nMBSlaveCommsWatchDogTimer; g_cReceiveBEnabled; g_cTransmitBEnabled;
//
// P.Smith					     22/03/06
// call  ReadSBBLinkOptions(); from InitialiseSerialPortB(void)			
// #include "InitialiseHardware.h"
//
// P.Smith					     20/09/06
// name change from InitialiseSerialPortB to InitialiseNetworkPort to allow for either port
// RxBOn to NetworkReceiverOn
// InitialiseNetworkPort implemented to check which port should be initialised.
//
// P.Smith					     9/11/06
// remove InitialiseU2SerialPort nbb--testonly--
// call netburner port init if g_bNBBRevB1Hardware set, else call duart init.
//
// P.Smith                      23/1/07
// Remove all reference to Duart set up.
// removed DisableTxA( void )
// Name change InitialiseU2SerialPort -> InitialiseNetworkSerialPort
//
// P.Smith                          5/2/07
// remove unused iprintf
//
// P.Smith                          6/2/07
// name change EnableTxB - > EnableSlaveTransmitter
// name change DisableTxB - > DisableSlaveTransmitter
//
// P.Smith                          9/1/08
// remove InitialiseSerialPortB
////////////////////////////////////////////////////////////////

//*****************************************************************************
// INCLUDES
//*****************************************************************************

#include <startnet.h>

#include <basictypes.h>
#include <stdio.h> 

#include "gpio5270.h"
//#include "NBBGpio.h"
#include <C:\Nburn\MOD5270\system\sim5270.h>

#include "InitComms.h"

//#include "Hardware.h"
#include "Mbshand.h"
#include "InitialiseHardware.h"
#include "general.h"
#include "BatVars2.h"
#include "initnbbcomms.h"
#include "MbMasterNBBHand.h"

   

void InitialiseSerialPorts( void )
{
//        iprintf("\n init serial ports");  //nbb--testonly--

         InitialiseMasterCommsSerialPort();  
         InitialiseNetworkSerialPort();        

}

//
// ENABLES TRANSMITTER
// RTS OUTPUT LOGIC 0,INVERTED THROUGH PLD.
//
void EnableSlaveTransmitter( void )
{
    SetU1CTS_Hi();    //
}

void DisableSlaveTransmitter( void )
{
    SetU1CTS_Lo();    //

} 




//////////////////////////////////////////////////////
// InitialiseNetworkPort                    
//
// Initialise network port
// 
//////////////////////////////////////////////////////
void InitialiseNetworkPort( void )
{
 	    InitialiseNetworkSerialPort();			// Initialise Netburner port  nbb--todo-- should t his be taken out ?
	g_nMBSlaveCommsWatchDogTimer = MBSLAVECOMSWDOGPERIOD;		// decremented to 0 on 1sec (in PIT), and if -> 0, call reset mb slave comms.

}

