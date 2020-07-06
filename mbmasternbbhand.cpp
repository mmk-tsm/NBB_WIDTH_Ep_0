////////////////////////////////////////////////////////////////
// P.Smith                              27/9/06
// modbus master handlera
//
// P.Smith                              5/12/06
// complete master comms handler for the netburner module.
//
// P.Smith                              2/2/07
// MasterRxOn called on error in serial tranceiver.
//
// P.Smith                              7/3/07
// remove  iprintf
//
// P.Smith                              20/3/08
// added COMMS_INC_DEC_WRITE_COMMAND before DecodeWriteResponse
// 
//
// P.Smith                              7/5/08
// put in proper test retries for the new command COMMS_INC_DEC_WRITE_COMMAND 
//
// P.Smith                              4/12/08
// call IncrementOngoingHistoryGoodTxCtr to increment good tx counter
////////////////////////////////////////////////////////////////

#include <Stdio.h>

#include "General.h"
#include "SerialStuff.h"
#include "InitComms.h"

#include "TimeDate.h"
#include "ConfigurationFunctions.h"
#include "16R40C.h"
#include "BatVars.h"
#include "BatchCalibrationFunctions.h"
#include    "TSMPeripheralsMBIndices.h"
#include "MBMaster.h"
#include "CalculateModbusCRC.h"
#include "BatVars2.h"
#include <basictypes.h>
#include "NBBGpio.h"
#include <String.h>
#include "MBPanelSNBBHand.h"
#include "mbmasternbbhand.h"
extern  structCommsData         g_sModbusMaster;
extern  char    g_cMasterCommsBuffer[];
extern  int     g_nMasterCommsPointer;       


void MasterSerialHandler( void )
{
volatile uartstruct * ThisUart= & (sim.uarts[NBB_MASTER_COMMS_PORT]);
	char    cStatus;
    BOOL    bError = TRUE;
    cStatus = ThisUart->uisr;

    if((cStatus & NBB_UART_ISR_RX_READY) && (g_sModbusMaster.m_bRXEnable))
    {
		// Indicate that this is a valid interrupt
		bError = FALSE;

		// Process the rx handler

		MasterRxHandler();
    }
//    else if((cStatus & NBB_UART_ISR_TX_READY) && g_sModbusMaster.m_bTXEnable )
    else if((cStatus & NBB_UART_ISR_TX_READY))
	{
		// Indicate that this is a valid interrupt

		// Process the tx handler               
		MasterTxHandler();
	}
}




////////////////////////////////////////////////////////////////
// TXHandler()                      // TRANSMIT INTERRUPT HANDLER 
//
// Copied from CP2000
// M.McKiernan                         18-01-2005
// First pass.
////////////////////////////////////////////////////////////////
void MasterTxHandler( void )
{
    volatile uartstruct * ThisUart= & (sim.uarts[NBB_MASTER_COMMS_PORT]);
    
    // If that was the last byte, wait for it to go
    // and turn the transmitter off.
    if( g_sModbusMaster.m_nTXIndex > g_sModbusMaster.m_nTXEndIndex )
    {
            // Increment the Good TX counter
            g_sModbusMaster.m_lGoodTx++;
            g_sModbusMaster.m_pController->m_lGoodTx++;
            IncrementOngoingModulesHistoryGoodTxCtr();
            
            // Switch to Rx mode
            MasterRxOn();
    }
    else
    {
        ThisUart->utb = g_sModbusMaster.m_cTxBuffer[g_sModbusMaster.m_nTXIndex];

        // Transmit the next byte
//        *(SERIAL_PORT_A + TX_REG) = g_sModbusMaster.m_cTxBuffer[g_sModbusMaster.m_nTXIndex];

        g_sModbusMaster.m_nTXIndex++;
    }
}


void MasterTxOn( void )
{
   volatile uartstruct * ThisUart = &sim.uarts[NBB_MASTER_COMMS_PORT];

    // Point to the start of the Tx buffer
    g_sModbusMaster.m_nTXIndex = 0;
    g_sModbusMaster.m_nRXIndex = -1;

    // Indicate that the Tx is enabled
    g_sModbusMaster.m_bTXEnable = TRUE;             // SET THE TX ENABLED FLAG
    g_sModbusMaster.m_bRXEnable = FALSE;            // CLEAR THE RX ENABLED FLAG

    // initialise the timeout counter with the
    // start value
    g_sModbusMaster.m_nTimeoutCounter = g_sModbusMaster.m_nTimeoutPreset;

    // Start the timeout timer
    g_sModbusMaster.m_bActive = TRUE;

    // The start delay is finished
    g_sModbusMaster.m_bRunStartDelay = FALSE;

   ThisUart->ucr = 0x04;   // 0000 0100 - Enable  tx
   ThisUart->uisr = 0x01; // 00000001 - enable tx interrupt

}


void MasterRxOn( void )
{
   volatile uartstruct * ThisUart = &sim.uarts[NBB_MASTER_COMMS_PORT];

    // Point to the start of the Rx buffer
    g_sModbusMaster.m_nRXIndex = 0;

    // Indicate that the Rx is enabled
    g_sModbusMaster.m_bTXEnable = FALSE;
    g_sModbusMaster.m_bRXEnable = TRUE;

    // Reset the start of message flag
    g_sModbusMaster.m_bStartFound = FALSE;
 
    ThisUart->ucr = RX_FIFO_ENABLE;   // 
    ThisUart->uisr = 0x02;

}


void MasterTxRxOff( void )
{
   volatile uartstruct * ThisUart = &sim.uarts[NBB_MASTER_COMMS_PORT];
   ThisUart->uisr = NO_INTERRUPTS;

    // Stop the timeout timer
    g_sModbusMaster.m_bActive = FALSE;

    g_sModbusMaster.m_bTXEnable = FALSE;

    g_sModbusMaster.m_bRunStartDelay = FALSE;

}


void MasterTxOff( void )
{
   volatile uartstruct * ThisUart = &sim.uarts[NBB_MASTER_COMMS_PORT];
   ThisUart->uisr = NO_INTERRUPTS;
   ThisUart->ucr = 0x08; 
  
    g_sModbusMaster.m_bActive = FALSE;

    g_sModbusMaster.m_bTXEnable = FALSE;

    g_sModbusMaster.m_bRunStartDelay = FALSE;
  	
}



////////////////////////////////////////////////////////////////
// RXHandler()                      // RECEIVE INTERRUPT HANDLER - MODBUS VERSION
//
// Copied from CP2000
// M.McKiernan                         18-01-2005
// First pass.
////////////////////////////////////////////////////////////////

void MasterRxHandler( void )
{
    char    cRxByte;
    char    cLineStatus;
    BOOL    bFinished = FALSE;
   
    volatile uartstruct * ThisUart = &sim.uarts[NBB_MASTER_COMMS_PORT];
    
    // Loop until we read all of the bytes in the FIFO
    while( !bFinished )
    {
        // Read the status
//        cLineStatus = *(SERIAL_PORT_A + SERIAL_STATUS_REG);
 		cLineStatus = ThisUart->usr;


			if( cLineStatus & NBB_UART_SR_RX_READY )
			{
			// Reading the byte clears the rxrdy bit
			cRxByte = ThisUart->utb;

			if( cLineStatus & 0x70 )
			{
               ThisUart->ucr |= NBB_UART_RESET_ERROR_STATUS;   // reset error status
				// Bad byte, reset receive handler
				MasterRxOn();  //nbb--todocheck--
				return;
            }

            else
            {
                // Save the byte
                g_sModbusMaster.m_cRxBuffer[g_sModbusMaster.m_nRXIndex] = cRxByte;

                // have we found the start of a message ?
                if( !g_sModbusMaster.m_bStartFound )
                {
                    // Does this byte match the address we transmitted to ?
                    if( cRxByte == g_sModbusMaster.m_pController->m_nSystemAddress )
                        g_sModbusMaster.m_bStartFound = TRUE;
                    else
                        g_sModbusMaster.m_nRXIndex = 0;
                }

                if( g_sModbusMaster.m_bStartFound )
                {
                    // Point to the next free location;
                    g_sModbusMaster.m_nRXIndex++;

                    if( g_sModbusMaster.m_nRXIndex >= COMMS_BUFFER_SIZE )
                    {
                        // Too many bytes coming in.
                        // panic and reset
                        MasterRxOn();
                    }
                    else
                    {
                        // Have we found what we're looking for ?
                        if( g_sModbusMaster.m_nRXIndex == g_sModbusMaster.m_nRXLength )
                        {
                            int nStatus;

                            // Show that we're processing the message
                            g_sModbusMaster.m_bProcessingMessage = TRUE;

                            // Disable the comms and the timeout counter
                            MasterTxRxOff();

                            // Remove the message from the top of the queue
                            RemoveFromQueue( g_sModbusMaster.m_pController );

                            // Switch on the message type to decide if it's
                            // a read or a write.

                            switch( g_sModbusMaster.m_nMessageType )
                            {
//                          case COMMS_FAST_STATUS_READ:
//                              nStatus = DecodeStatusReadResponse();
//                              break;
                            case COMMS_READ_SUMMARY:
                            case COMMS_READ_CALIBRATION:
                            case COMMS_READ_ID:
                                nStatus = DecodeReadResponse();
                                break;
                            case COMMS_WRITE_COMMANDS:
                            case COMMS_INC_DEC_WRITE_COMMAND:
                                nStatus = DecodeWriteResponse();
                                if( nStatus != MODBUS_GOOD_RECEIVE )
                                {
                                    // If a write message fails we need to queue
                                    // it again for the required number of retries
                                    g_sModbusMaster.m_pController->m_nCommandWriteRetries++;
                                    if(g_sModbusMaster.m_nMessageType == COMMS_WRITE_COMMANDS)
                                    { 
                                    
                                        if( g_sModbusMaster.m_pController->m_nCommandWriteRetries < MODBUS_WRITE_QUEUE_RETRIES )
                                        {
                                            AddToQueue( COMMS_WRITE_COMMANDS, 0, g_sModbusMaster.m_pController );
                                        }
                                        else
                                        {
                                            g_sModbusMaster.m_pController->m_nCommandWriteRetries = 0;
                                        
                                        // generate an alarm
                                        }
                                    }
                                    else
                             
                                    if(g_sModbusMaster.m_nMessageType == COMMS_INC_DEC_WRITE_COMMAND)
                                    { 
                                    
                                        if( g_sModbusMaster.m_pController->m_nIncDecSetpointWriteRetries < MODBUS_WRITE_QUEUE_RETRIES )
                                        {
                                            AddToQueue( COMMS_INC_DEC_WRITE_COMMAND, 0, g_sModbusMaster.m_pController );
                                        }
                                        else
                                        {
                                            g_sModbusMaster.m_pController->m_nIncDecSetpointWriteRetries = 0;
                                        
                                        // generate an alarm
                                        }
                                    }
                                }
                                else
                                {       // was a good write
                                    g_bCommsAttributeUpdateActive = FALSE;      // signal that ready for more attributes.
                                }
                                break;
                            case COMMS_WRITE_DISPLAY:
                                nStatus = DecodeWriteResponse();
                                if( nStatus != MODBUS_GOOD_RECEIVE )
                                {
                                    // If a write message fails we need to queue
                                    // it again for the required number of retries
                                    g_sModbusMaster.m_pController->m_nDisplayWriteRetries++;
                                    if( g_sModbusMaster.m_pController->m_nDisplayWriteRetries < MODBUS_WRITE_QUEUE_RETRIES )
                                        AddToQueue( COMMS_WRITE_DISPLAY, 0, g_sModbusMaster.m_pController );
                                    else
                                    {
                                        g_sModbusMaster.m_pController->m_nDisplayWriteRetries = 0;
                                        
                                        // generate an alarm
                                    }
                                }
                                else    // display write worked fine.
                                {
                                        GetNextDisplayWriteIndices();
                                        if( g_bCommsUpdateActive)   // has it found something to update.
                                            AddToQueue( COMMS_WRITE_DISPLAY, 0, g_sModbusMaster.m_pController );
                                }
                                break;
                            case COMMS_WRITE_CALIBRATION:
                                nStatus = DecodeWriteResponse();
                                if( nStatus != MODBUS_GOOD_RECEIVE )
                                {
                                    // If a write message fails we need to queue
                                    // it again for the required number of retries
                                    g_sModbusMaster.m_pController->m_nCalibrationRetries++;
                                    if( g_sModbusMaster.m_pController->m_nCalibrationRetries < MODBUS_WRITE_QUEUE_RETRIES )
                                        AddToQueue( COMMS_WRITE_CALIBRATION, 0, g_sModbusMaster.m_pController );
                                    else
                                    {
                                        g_sModbusMaster.m_pController->m_nCalibrationRetries = 0;
                                        
                                        // generate an alarm
                                    }
                                }
                                break;
                            case COMMS_WRITE_SINGLE_WORD:
                                nStatus = DecodeWriteResponse();
                                if( nStatus != MODBUS_GOOD_RECEIVE )
                                {
                                    // If a write message fails we need to queue
                                    // it again for the required number of retries
                                    g_sModbusMaster.m_pController->m_nSingleWriteRetries++;
                                    if( g_sModbusMaster.m_pController->m_nSingleWriteRetries < MODBUS_WRITE_QUEUE_RETRIES )
                                        AddToQueue( COMMS_WRITE_SINGLE_WORD, g_sModbusMaster.m_nMessageValue, g_sModbusMaster.m_pController );
                                    else
                                    {
                                        g_sModbusMaster.m_pController->m_nSingleWriteRetries = 0;
                                        
                                        // generate an alarm
                                    }
                                }
                                break;
                            default:
                                // Unsupported message type - flag an error somewhere.
                                break;
                            }

                            // Show that we're finished processing the message
                            g_sModbusMaster.m_bProcessingMessage = FALSE;
                        }
                        else
                        {
                            // something might be wrong in the receive
                            // Check for a message the length of an error result response
// Add RS485 check here using m_nStripLength
                            if( g_sModbusMaster.m_nRXIndex == ERROR_RESPONSE_LENGTH )
                            {
                                // The result is indicated by setting the ms bit of the
                                // command code (second byte)
                                if( g_sModbusMaster.m_cTxBuffer[1] & 0x80 )
                                {
                                    g_sModbusMaster.m_lErrorResponseCounter++;
                                    g_sModbusMaster.m_pController->m_lErrorResponseCounter++;

                                    // Finish up, no point wasting time waiting for
                                    // the rx timeout to elapse.
                                    MasterTxRxOff();

                                    // Remove the message from the top of the queue
                                    RemoveFromQueue( g_sModbusMaster.m_pController );
                                }
                            }
                        }
                    }
                }
            }
        }
        else
            bFinished = TRUE;
    }
}








