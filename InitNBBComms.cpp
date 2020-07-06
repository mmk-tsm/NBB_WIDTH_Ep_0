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
// name change InitialisePanelSerialPort,PanelRxOn,InitialiseU2SerialPort added
// name change to reflect the port being used, u2 in this case.
//
// P.Smith					     2/11/06
// specify NBB_PANEL_COMMS_PORT,PANEL_BAUDRATE in openserial
//
// P.Smith					     9/11/06
// implement InitialiseMasterCommsSerialPort
//
// P.Smith					     16/1/07
// Remove references to Duart.
//
// P.Smith					     19/1/07
// call InitialiseNBBUartPins after open serial
//
// P.Smith					     23/1/07
// Name change InitialiseU2SerialPort -> InitialiseNetworkSerialPort
// Remove set up of port U3 as this is now used as the diagnostics port temporarily
//
// P.Smith					     5/2/07
// Correct InitialisePanelSerialPort function set up of seral port
// More meaningful names are added.
//
// P.Smith                      6/2/07
// check for g_bRedirectStdioToPort2 if true call EnableSlaveTransmitter to allow diagnostics
// to be routed via port 2
// no open of network serial port if g_bPrintfRunning set
// Put back in Network based tx/rx on off stuff.
//
// P.Smith                      23/6/08
// name change to g_bSBBL2Fitted
//
// P.Smith                      14/10/08
// remove compiler warnings fdserial
//
// P.Smith                      13/2/09
// check  g_CalibrationData.m_bNoParity to determine if no parity should be set up.
// clear the serial port before opening it again as it will not open if is already
// open.
//
// P.Smith                      27/4/10
// added selection of baud rate from modbus table, the configuration must be loaded
// from eeprom to allow the baud rate configuration to be read.
// it is no longer necessary to call InitialiseNetworkSerialPort in main again
////////////////////////////////////////////////////////////////

//*****************************************************************************
// INCLUDES
//*****************************************************************************

#include <startnet.h>

#include <basictypes.h>
#include <stdio.h>

#include "gpio5270.h"
#include <C:\Nburn\MOD5270\system\sim5270.h>

#include "InitComms.h"

//#include "Hardware.h"
#include "Mbshand.h"
#include "MbPanelSNBBhand.h"
#include "InitialiseHardware.h"
#include <Serial.h>
#include "InitNBBComms.h"
#include "General.h"
#include "InitComms.h"
#include "NBBGpio.h"
#include "BatVars2.h"
#include "BatVars.h"
#include "BatchCalibrationFunctions.h"
#include "ConfigurationFunctions.h"

extern  CalDataStruct   g_CalibrationData;


void	InitialisePanelSerialPort(void)			// Initialise Port B
{
   volatile uartstruct *ThisUart;

   ThisUart = &( sim.uarts[NBB_PANEL_COMMS_PORT] );
	// Enable the divisor latches
// Blender
    SerialClose(NBB_PANEL_COMMS_PORT);
    int fdserial;
    fdserial = OpenSerial( NBB_PANEL_COMMS_PORT, PANEL_BAUDRATE, STOP_BITS, DATA_BITS, eParityEven );

    //Uart pins

    InitialiseNBBUartPins();
    U1TXEN_Hi();

    DisableNBBTxB();
	PanelRxOn();		// Turn B receiver on
	g_nMBSlaveCommsWatchDogTimer = MBSLAVECOMSWDOGPERIOD;		// decremented to 0 on 1sec (in PIT), and if -> 0, call reset mb slave comms.

}

//
// ENABLES TRANSMITTER
//
void EnableNBBTxB( void )
{

	// Clear the RTS bit
}

void DisableNBBTxB( void )
{
	// Set the RTS bit
}





void	InitialiseNetworkSerialPort(void) //
{
   BOOL bok;
   unsigned int nBaud;
   bok = LoadAllCalibrationDataFromEEprom();
   volatile uartstruct *ThisUart;
   ThisUart = &( sim.uarts[NBB_PANEL_COMMS_PORT] );

    if(g_bRedirectStdioToPort2)
    {
        EnableSlaveTransmitter();
    }
    else if(!g_bPrintfRunning)
    {
        g_bEnableTxRxInOpenSerial = FALSE;
        if(g_CalibrationData.m_nBaudRate == BAUD_RATE_9600_ID)
        nBaud = BAUD_RATE_9600;
        else
        if(g_CalibrationData.m_nBaudRate == BAUD_RATE_19200_ID)
        nBaud = BAUD_RATE_19200;
        else
        if(g_CalibrationData.m_nBaudRate == BAUD_RATE_38400_ID)
        nBaud = BAUD_RATE_38400;
        else
        if(g_CalibrationData.m_nBaudRate == BAUD_RATE_57600_ID)
        nBaud = BAUD_RATE_57600;
        else
        if(g_CalibrationData.m_nBaudRate == BAUD_RATE_115200_ID)
        nBaud = BAUD_RATE_115200;
        else
        nBaud = BAUD_RATE_9600;
        //SetupMttty();  //nbb--testonly--remove
        //iprintf("\n baud is %d",nBaud); //nbb--testonly-- remove

            if(g_CalibrationData.m_bNoParity)
            {
               int fdserial;
               SerialClose(NBB_NETWORK_COMMS_PORT);
                fdserial = OpenSerial( NBB_NETWORK_COMMS_PORT, nBaud, STOP_BITS, DATA_BITS, eParityNone);
            }
            else
            {
                int fdserial;
                SerialClose(NBB_NETWORK_COMMS_PORT);
                fdserial = OpenSerial( NBB_NETWORK_COMMS_PORT, nBaud, STOP_BITS, DATA_BITS, eParityEven);
            }

	DisableSlaveTransmitter();  // disable transmitter
	NetworkRxOn();		// Turn B receiver on
	}
}


void	InitialiseMasterCommsSerialPort(void)
{
   volatile uartstruct *ThisUart;
   ThisUart = &( sim.uarts[NBB_MASTER_COMMS_PORT] );
    int fdserial;
    fdserial = OpenSerial( NBB_MASTER_COMMS_PORT, MASTER_BAUDRATE, STOP_BITS, DATA_BITS, eParityNone);

}






///////////////////////////////////////////////////////////
// U2TxOn()
//
///////////////////////////////////////////////////////////

void NetworkTxOn( void )
{
   volatile uartstruct * ThisUart;
   ThisUart = &sim.uarts[NBB_NETWORK_COMMS_PORT];
   ThisUart->ucr = 0x04;   // 0000 0100 - Enable  tx
   ThisUart->uisr = 0x01; // 00000001 - enable tx interrupt

//   iprintf("\n u2 tx on"); //nbb--testonly--

}


///////////////////////////////////////////////////////////
// U2RxOn()
//
///////////////////////////////////////////////////////////

void NetworkRxOn( void )
{
   volatile uartstruct * ThisUart;
   ThisUart = &sim.uarts[NBB_NETWORK_COMMS_PORT];
   ThisUart->ucr = RX_FIFO_ENABLE;   //
   ThisUart->uisr = 0x02;
   g_cReceiveBEnabled = 1;
   g_cTransmitBEnabled = 0;
}

///////////////////////////////////////////////////////////
// U2TxOff()
//
///////////////////////////////////////////////////////////

void NetworkSlaveTxOff( void )
{
   volatile uartstruct * ThisUart;
   ThisUart = &sim.uarts[NBB_NETWORK_COMMS_PORT];
   ThisUart->uisr = NO_INTERRUPTS;
   ThisUart->ucr = 0x08;

//   iprintf("\n u2 tx off");  //nbb--testonly--

}



