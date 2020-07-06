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

///////////////////////////////////////////////////////////

#ifndef __CALCULATEMODBUSCRC_H__
#define __CALCULATEMODBUSCRC_H__

#include <basictypes.h>


WORD	CalculateModbusCRC( unsigned char *buffer, WORD count );
#endif	//__CALCULATEMODBUSCRC_H__
