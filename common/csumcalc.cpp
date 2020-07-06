

///////////////////////////////////////////////////////////
//
// P.Smith                              10/1/06
// returned int from CalculateCheckSum is unsigned
// nChecksum is also unsigned.
//
//
// P.Smith                              11/1/06
// Correct warnings
// int j removed from unsigned int CalculateCheckSum( unsigned char *buffer, int count )

///////////////////////////////////////////////////////////
// Calculates the polynomial checksum used by the Modbus
// protocol.
// 
// Entry:
//      'buffer'    is a pointer to the tx or rx buffer
//                  holding the message data.
//      'count'     is the number of bytes in the buffer.
//                  the checksum calculation should include.
//  Exit:
//      returns     the calculated checksum.
///////////////////////////////////////////////////////////
#include "CsumCalc.h"



///////////////////////////////////////////////////////////
// CalculateCheckSum 
// 
// Entry:
//      'buffer'    is a pointer to the data 
//
//      returns     the calculated checksum.
///////////////////////////////////////////////////////////


unsigned int CalculateCheckSum( unsigned char *buffer, int count )

{
    unsigned int     nChecksum = 0;
    char    *ptr;
    int     i;

    // calculate the checksum

    ptr = (char*)&nChecksum;

    // calculate the checksum

    for( i = 0; i < count; i++ )
    {

        // add data
        nChecksum += *(buffer + i);
    }

    return nChecksum;
}

