////////////////////////////////////////////////////////////////
// P.Smith                              14/9/06
// add slave serial handlers for net burner.
//
// P.Smith                              20/9/06
// change naming so that port can easily be changed over to another port.
// change the port name so that it does not refer to the port U2 concerned.
//
// P.Smith                              22/1/07
// name change U2SerialHandler -> SlaveSerialHandler
// Name change ResetSlavembrx
// 
// P.Smith                      7/3/07
// Remove printf.
// 
// P.Smith                      22/6/07
// g_sModbusNetworkSlave.m_lGoodTx added for network comms diagnostics
// In  SlaveSerialHandler, Changed to else if( cStatus & NBB_UART_ISR_TX_READY ),
//    Check for g_cTransmitBEnabled in NetworkSlaveTxHandler
// 
// P.Smith                      14/10/08
// removed i compiler warning.
//
// P.Smith                      8/12/08
// call  g_sOngoingHistoryNetworkComms.m_lGoodTx++;
                   
////////////////////////////////////////////////////////////////

#include <Stdio.h>

#include "General.h"

#include <basictypes.h>

#include "MbPanelSNBBhand.h"
#include "MBSHand.h"
#include "BatchMBIndices.h"

#include "InitComms.h"
#include "NBBGpio.h"

#include "MBProgs.h"
#include "CalculateModbusCRC.h"

#include "predef.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "includes.h"
#include "constants.h"
#include "ucos.h"
#include "cfinter.h"
#include "utils.h"
#include "buffers.h"
#include "serial.h"
#include "bsp.h"
#include "iointernal.h"
#include "iosys.h"
#include "serinternal.h"
#include "randseed.h"
#include <sim5270.h>
#include "mbnetsnbbhand.h"
#include "initnbbcomms.h"
#include "Serialstuff.h"
#include "HistoryLog.h"
#include "Batvars2.h"


extern  unsigned char	Rx1Buff[];
extern  unsigned char	Tx1Buff[];
extern  WORD g_nTx1Ptr;
extern  WORD g_nMBTx1Bytes;
extern  WORD g_nRx1Ptr;
extern  WORD g_nRx1Ptr;


extern  WORD g_nRx1ByteNo;
//unsigned char g_nMBRx1Bytes;
extern  WORD g_nMBRx1Bytes;
//unsigned char g_nMBTx1Bytes;
extern  WORD g_nMBTx1Bytes;
extern  char g_cMBRx1Func;
extern  BOOL  g_bBlenderSelected;
extern  WORD  g_nMB1BreakCounter;
extern  char g_cMB1BreakFlag;                  // set break flag on power-up
extern  WORD  g_nCRCErrorCounter;
extern  WORD	g_nSlaveGoodRxCounter;
extern  WORD	g_nSlaveGoodTxCounter;
extern  unsigned char g_cRx1Data,g_cErrorCode;
extern  char g_cTx1OffFlag;            // flag to have Tx1 switched off when TSR empty.
extern  char g_cTx1ModeFlag;
extern  char g_cLastChTimer;
extern  structCommsData         g_sModbusNetworkSlave;






void SlaveSerialHandler( void )
{
volatile uartstruct * ThisUart= & (sim.uarts[NBB_NETWORK_COMMS_PORT]);
	char    cStatus;
    BOOL    bError = TRUE;
//    cStatus = *(SERIAL_PORT_B + INTERRUPT_ID);
    cStatus = ThisUart->uisr;
//testonly				
//				if(g_bLiWDebug && fdnet>0)
//				  printf("\nI%x", cStatus);
//testonly				
//				if(g_bLiWDebug && fdnet>0)
//				  printf("\nN");
				  
    if((cStatus & NBB_UART_ISR_RX_READY) && (g_cReceiveBEnabled))
    {
		// Indicate that this is a valid interrupt
		bError = FALSE;

		// Process the rx handler

		NetworkSlaveRxHandler();
//nbb--testonly--        iprintf("\n char rxd is %x",g_cNBBRx1Data);
    }
//    else if((cStatus & NBB_UART_ISR_TX_READY) && g_cTransmitBEnabled )
    else if( cStatus & NBB_UART_ISR_TX_READY )  // Check for g_cTransmitBEnabled in NetworkSlaveTxHandler.
	{
		// Indicate that this is a valid interrupt

		// Process the tx handler               
		NetworkSlaveTxHandler();
	}
}






void NetworkSlaveTxHandler( void )
{
volatile uartstruct * ThisUart= & (sim.uarts[NBB_NETWORK_COMMS_PORT]);
   if( g_cTransmitBEnabled )
   {
        ThisUart->utb = Tx1Buff[g_nTx1Ptr];
//        iprintf("\n transmitting %d",Tx1Buff[g_nTx1Ptr]);
        g_nTx1Ptr++;
        
  
            if((g_nTx1Ptr >= g_nMBTx1Bytes) || (g_nTx1Ptr >= TX1BUFSIZE))          // LASTBYTE, OR END OF BUFFER
                {
//                    TxRxBOff();
                    g_cTx1ModeFlag = 0; // switch out of transmit mode
                    g_cLastChTimer = DELAYAFTERLASTCHTX;
                    g_cTx1OffFlag = 1;  // set flag for turning off xmtr

                    NetworkSlaveTxOff();
                    g_nSlaveGoodTxCounter++;
                    g_sModbusNetworkSlave.m_lGoodTx++;
                    g_sOngoingHistoryNetworkComms.m_lGoodTx++;
                    
                    NetworkReceiverOn();

                }
   }
   else
   {
                   NetworkSlaveTxOff();
                   g_cLastChTimer = DELAYAFTERLASTCHTX; // cause RS422 driver to be disabled.
   
   }                
}

// 
// 
// RECEIVE INTERRUPT HANDLER - MODBUS VERSION
// 






void NetworkSlaveRxHandler( void )
{
volatile uartstruct * ThisUart= & (sim.uarts[NBB_NETWORK_COMMS_PORT]);
    char    cLineStatus;
	
	 if(g_nRx1Ptr >= RX1PANELBUFSIZE)       // Ensure no write beyond buffer
         g_nRx1Ptr = RX1PANELBUFSIZE-1;

	
	g_nMB1BreakCounter = MBBREAK;        // X mS (decremented on 4 kHz to zero)(9600/19200)
	g_nMBSlaveCommsWatchDogTimer = MBSLAVECOMSWDOGPERIOD;		// decremented to 0 on 1sec (in PIT), and if -> 0, call reset mb slave comms.
	// Read the status

	// read received data while the receive ready bit is set 
//		while(*(SERIAL_PORT_B + SERIAL_STATUS_REG) & RX_BYTE_AVAILABLE)
		while(ThisUart->usr & NBB_UART_SR_RX_READY)

		{
//			cLineStatus = *(SERIAL_PORT_B + SERIAL_STATUS_REG);
			cLineStatus = ThisUart->usr;

			// If there is a pending byte, process it
			if( cLineStatus & NBB_UART_SR_RX_READY )
			{
			// Reading the byte clears the rxrdy bit
			g_cRx1Data = ThisUart->utb;
//            iprintf("\n receiving %d",g_cRx1Data);


			// check for transmission error       
//			if( cLineStatus & RX_ERROR )
			if( cLineStatus & 0x70 )
				{
				// Bad byte, reset receive handler
       
				ResetSlavembrx();
                ThisUart->ucr |= NBB_UART_RESET_ERROR_STATUS;   // reset error status
  
                return;

				}
				else
				{

			// Save the byte
 				    SaveRxdCh();	 	
				}
		
			}
		}  // end of while
}


void ResetSlavembrx(void)
{
        g_nRx1Ptr = 0;
        g_nRx1ByteNo = 0;
        g_nMBRx1Bytes = 0; // no of bytes expected
        g_cMBRx1Func = 0;
        g_bBlenderSelected = FALSE;
}
