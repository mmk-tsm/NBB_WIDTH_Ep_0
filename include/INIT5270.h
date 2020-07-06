///////////////////////////////////////////////////////////
// Calculates the polynomial checksum used by the Modbus
// protocol.
// 
// Entry:
//		'buffer'	is a pointer to the tx or rx buffer
//					holding	the message data.
//		'count'		is the number of bytes in the buffer.
//		'index'		is the point in the buffer at which
//					the checksum calculation should start.
//	Exit:
//		returns		the calculated checksum.
//  P.Smith								11/1/06
// change to unsigned - unsigned int CalculateModbusCRC( unsigned char *buffer, int count );

//  P.Smith								11/1/06
// corrected prototype for CalculateModbusCRC use WORD instead of int
// include basictypes.h

//  P.Smith								14/2/06
// added void SetNBBInterrupts (void);

//  P.Smith								29/6/06
// added SetupTimer2( void );

//  P.Smith								29/6/06
//  Remove SetNBBInterrupts
//
//  P.Smith								14/2/07
// added SetUpPITRT
//
//  P.Smith								23/7/08
// name change ReadAToD_1
///////////////////////////////////////////////////////////

#ifndef __INIT5270_H__
#define __INIT5270_H__

#include <basictypes.h>


void SetUpPITR(int pitr_ch, WORD clock_interval, BYTE pcsr_pre /* See table
13-13 in the users manual */);
void SetUpPITRT(int pitr_ch, WORD clock_interval, BYTE pcsr_pre /* See table
13-13 in the users manual */);

WORD ReadAToD_1 (void);
void SetupTimer2( void );



#endif	//__INIT5270_H__
