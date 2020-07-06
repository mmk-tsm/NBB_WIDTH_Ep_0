//////////////////////////////////////////////////////////////
// Writes an integral value as string into the buffer. Radix
// is the number base to be used, e.g. 10 (decimal). 
//
//	Entry	val	- long value to be converted.
//			buf - pointer to result buffer.
//			radix - number base for the conversion.
//			nDigits - minimum number of digits to be displayed.
//					result is padded with blanks out to match.
//////////////////////////////////////////////////////////////
#ifndef __ITOA_H__
#define __ITOA_H__

void IntToAscii( long val, char *buf, int radix, int nDigits );



#endif  // __ITOA_H__

