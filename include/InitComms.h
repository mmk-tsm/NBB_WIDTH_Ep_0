///////////////////////////////////////////////////////////
//InitComm.h
//
//
// M.Parks					24-01-2000
// M.McKiernan					14-08-2003 - InitialiseSerialPortB(void)
// M.McKiernan					19-12-2005 - Transported across for NBB.
//													 Included the 16C552 register definitions.
//                                      Incorporated RxBOn().
// P.Smith                  20/9/06
//added SERIALPORT_TO_USE,BAUDRATE_TO_USE,STOP_BITS,DATA_BITS           
// added NETWORK_BAUDRATE,NBB_NETWORK_COMMS_PORT,NBB_PANEL_COMMS_PORT
// added void InitialiseNetworkPort( void );
// void NetworkReceivTerOn( void );
//                                      
// P.Smith                  27/9/06
// added NBB_MASTER_COMMS_PORT
//                                      
// P.Smith                  9/11/06
// added MASTER_BAUDRATE     
// define NBB_MASTER_COMMS_PORT
//
//                                      
// P.Smith                  24/1/07
// change uart to the correct settings for the new rev B1 hardware.
// name change EnableTxA to  EnableMasterTransmitter
//
// P.Smith                  6/2/06
// name change EnableSlaveTransmitter, DisableSlaveTransmitter;
//
// P.Smith                  9/1/08
// remove InitialiseSerialPortB

//////////////////////////////////////////////////////////////

#ifndef _INITCOMMS_H                       
#define _INITCOMMS_H                               

//#define  MBSLAVECOMSWDOGPERIOD   (3)   // testonly - value from MBSHand.h 

// Constants
// Line control register bits
#define	DATA_BITS_8			0x03		// Bits 0, 1
#define	DATA_BITS_7			0x02

#define	STOP_BIT_1			0x00		// Bit 2
#define	STOP_BITS_2			0x04

#define	PARITY_OFF_BIT		0x00		// Bit 3
#define	PARITY_ON_BIT		0x08

#define	PARITY_ODD_BIT		0x00		// Bit 4
#define	PARITY_EVEN_BIT		0x10

#define	DIVISOR_LATCH_DIS	0x00		// Bit 7
#define	DIVISOR_LATCH_ENB	0x80		// Bit 7

// Divisor Latch Baud Rates
#define	BAUD_19200_MSB		0x00
#define	BAUD_19200_LSB		0x06

#define	BAUD_9600_MSB		0x00
#define	BAUD_9600_LSB		0x0c

#define	BAUD_4800_MSB		0x00
#define	BAUD_4800_LSB		0x18

#define	BAUD_2400_MSB		0x00
#define	BAUD_2400_LSB		0x30

#define	BAUD_1200_MSB		0x00
#define	BAUD_1200_LSB		0x60


// FIFO control register bits
#define	FIFO_DISABLE		0x00	// Bit 0
#define	FIFO_ENABLE			0x01

// These remaining bits only work with DMA_MODE_ON
#define	RX_FIFO_RESET		0x02	// Bit 1
#define	TX_FIFO_RESET		0x04	// Bit 2

#define	DMA_MODE_0			0x00	// Bit 3
#define DMA_MODE_1			0x08

#define	RX_FIFO_LEVEL_1		0x00	// Bits 6, 7
#define	RX_FIFO_LEVEL_4		0x40
#define	RX_FIFO_LEVEL_8		0x80
#define	RX_FIFO_LEVEL_14	0xc0

// Modem control bits
#define	FORCE_RTS_0			0x00	// Bit 1
#define	FORCE_RTS_1			0x02
#define	CLEAR_RTS_BIT		0xfd

#define	ACTIVATE_INT_AB		0x08	// Bit 3

#define	ENABLE_LOOPBACK		0x10	// Bit 4

// Interrupt enable bits
#define	NO_INTERRUPTS		0x00

#define	RX_FIFO_ENABLE		0x01	// Bit 0
#define	RX_FIFO_DISABLE		0xfe

#define	TX_ENABLE			0x02	// Bit 1
#define TX_DISABLE			0xfd

#define	RX_BYTE_ENABLE		0x04	// Bit 2
#define	RX_BYTE_DISABLE		0xfb

// Line status bits
#define	RX_BYTE_AVAILABLE	0x01	// Bit 0
#define	OVERRUN_ERROR		0x02	// Bit 1
#define	PARITY_ERROR		0x04	// Bit 2
#define	FRAMING_ERROR		0x08	// Bit 3
#define	BREAK_CHARACTER		0x10	// bit 4
#define	TX_FIFO_EMPTY		0x20
#define	TX_ALL_EMPTY		0x40
#define	RX_ERROR			0x80

// Interrupt status bits
#define	RX_LINE_STATUS		0x06
#define	RX_DATA_READY		0x04
#define	RX_DATA_TIMEOUT		0x0c
#define	TX_READY			0x02

// Offsets from the basic port address
#define	RX_REG				0		// RECEIVE HOLDING REGISTER
#define	TX_REG				0		// TRANSMIT HOLDING REGISTER
#define	DIVISOR_LATCH_LSB	0		// DIVISOR LATCH LSB
#define	INTERRUPT_ENABLE	1		// INTERRUPT ENABLE REGISTER
#define	DIVISOR_LATCH_MSB	1		// DIVISOR LATCH MSB
#define	INTERRUPT_ID		2		// INTERRUPT INDENTIFICATION REGISTER
#define	FIFO_CONTROL		2		// FIFO CONTROL REGISTER
#define	LINE_CONTROL		3		// LINE CONTROL REGISTER
#define	MODEM_CONTROL		4		// MODEM CONTROL REGISTER
#define	SERIAL_STATUS_REG	5		// LINE STATUS REGISTER
#define	MODEM_STATUS		6		// MODEM STATUS REGISTER
#define	SCRATCH_REG			7		// SCRATCH REGISTER

#define	PORT_REG			0		// Bi-directional data register
#define	PARALLEL_STATUS_REG	1
#define	IO_SELECT_REG		1
#define	COMMAND_REG			2
#define	CONTROL_REG			2

#define SERIALPORT_TO_USE   1 //0 for the main port, 1 for the 10pin aux serial port
#define PANEL_BAUDRATE      9600
#define STOP_BITS           1
#define DATA_BITS           8

#define NETWORK_BAUDRATE    9600
#define MASTER_BAUDRATE     19200

#define NBB_NETWORK_COMMS_PORT    2
#define NBB_PANEL_COMMS_PORT    0
#define NBB_MASTER_COMMS_PORT    1



// exported methods
void	EnableSlaveTransmitter( void );

void	DisableSlaveTransmitter( void );

void	InitialiseSerialPorts( void );

void InitialiseNetworkPort( void );
void NetworkReceivTerOn( void );


	
// void RxBOn( void );                 //todo - taken from MBSHand.c
#endif //
	

