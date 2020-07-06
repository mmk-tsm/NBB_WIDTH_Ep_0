///////////////////////////////////////////////////////////
//
// P.Smith                              10/1/06
// returned int from CalculateCheckSum is unsigned

///////////////////////////////////////////////////////////
// Calculates checksum 
// 
// Entry:
//      'buffer'    is a pointer to the tx or rx buffer
//                  holding the message data.
//      'count'     is the number of bytes in the buffer.
//      'index'     is tc che point in the buffer at which
//                  the checksum calculation should start.
//  Exit:
//      returns     the calculated checksum.
///////////////////////////////////////////////////////////

#ifndef __CSUMCALC_H__
#define __CSUMCALC_H__


unsigned int CalculateCheckSum( unsigned char *buffer, int count );

#endif	//__CSUMCALC_H__


