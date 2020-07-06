///////////////////////////////////////////////////////////
//SCIComms.h
//
//
// M.McKiernan					17-12-2004 
//	First pass
//
// P.Smith                      26/1/06
// first pass at netburner hardware conversion.
// Bool -> BOOL
// included  <basictypes.h>
////////////////////////////////////////////////////////////
//*****************************************************************************
// MACROS
//*****************************************************************************
#ifndef __SCICOMMS_H__
#define __SCICOMMS_H__

#include <basictypes.h>



// Register definitions
#define  SCI_CR0 		0	// SCCR0
#define  SCI_CR1 		1	// SCCR1
#define  SCI_STATUS 	2	// SCSR
#define  SCI_DATA 	3	// SCDR

// SCCR0 
#define  SCI_BAUD_19200	0x001A	// 19200 BAUD
#define  SCI_BAUD_9600	0x0034	// 9600 BAUD
#define  SCI_BAUD_4800	0x0068	// 4800 BAUD
#define  SCI_BAUD_2400	0x00D0	// 2400 BAUD

// SCCR1 
#define	SCI_PARITY_ODD 	0x0800		// Bit b11
#define	SCI_PARITY_EVEN	0x0000		// Bit b11

#define	SCI_PARITY_ON 		0x0400		// Bit b10
#define	SCI_PARITY_OFF		0x0000		// Bit b10

#define	SCI_10_BITS_MODE 		0x0000		// Bit b9
#define	SCI_11_BITS_MODE 		0x0200		// Bit b9

#define	SCI_8_DATA_NO_PARITY	0x0000		// M=0, PE=0
#define	SCI_8_DATA_PARITY		0x0600		// M=1, PE=1  (SEE PAGE 7-30 OF MANUAL)

// Interrupt enable bits
#define	SCI_NO_INTERRUPTS		0x0000

#define	SCI_TX_INT_ENABLE		0x0080	// Bit b7
#define	SCI_TC_INT_ENABLE		0x0040	// Bit b6 - transmit complete
#define	SCI_RX_INT_ENABLE		0x0020	// Bit b5

#define	SCI_TX_ENABLE			0x0008	// Bit b3
#define	SCI_RX_ENABLE			0x0004	// Bit b2

// status bits
#define	SCI_TDRE				0x0100	// Bit b8
#define	SCI_TC 				0x0080	// Bit b7
#define	SCI_RDRF 			0x0040	// Bit b6

#define	SCI_OR_ERR			0x0008	// Bit b3	//OVERRUN ERROR
#define	SCI_N_ERR			0x0004	// Bit b2	//NOISE ERROR
#define	SCI_F_ERR			0x0002	// Bit b1	//FRAMING ERROR
#define	SCI_P_ERR			0x0001	// Bit b0	//PARITY ERROR


// Function prototypes
void InitialiseSCISerialPort( void );
void DisableTxSCI( void );
void EnableTxSCI( void );

void EnableSCITxInterrupt( void );
void DisableSCITxInterrupt( void );

void TransmitCharSCI( unsigned char cChar );

BOOL IsSCITxEmpty( void );
void TestSCI( void );
void TestSCIInterrupt(void);

#endif	// __SCICOMMS_H__

