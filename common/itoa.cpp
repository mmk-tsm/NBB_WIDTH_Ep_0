//////////////////////////////////////////////////////////////
// Writes an integral value as string into the buffer. Radix
// is the number base to be used, e.g. 10 (decimal). 
//
// M.Parks						20-01-2000
// Modified from sample Hiware code.
// M.Parks						14-02-2000
// Added code for padding out with blanks.
//
// P.Smith                      26/1/06
// first pass at netburner hardware conversion.
// included  <basictypes.h>
// included "string.h"
////////////////////////////////////////////////////////////////
#include "itoa.h"
//nbb #include "Hardware.h"
#include "General.h"
#include "string.h"


void IntToAscii(long val, char *buf, int radix, int nDigits )
{
	char *p;		// pointer to traverse string
	char *firstdig;	// pointer to first digit
	int	nLength;

	p = buf;
	if( radix == 10 && val < 0 )
	{	// negative, so output '-' and negate
		*p++ = '-';
		val = (unsigned long)(-(long)val);
	}

	// save pointer to first digit
	firstdig = p;

	do
	{
		unsigned int digval = (unsigned) (val % radix);
		val /= (long)radix;       // get next digit
		
		// convert to ascii and store
		if (digval > 9)
			*p++ = (char)(digval - 10 + 'a');  // a letter
		else
			*p++ = (char)(digval + '0');       // a digit
	}
	while( val > 0 );

	// terminate string; p points to last digit
	*p-- = '\0';

	// We now have the digit of the number in the buffer,
	// but in reverse order.  Thus we reverse them now.
	do
	{
		char temp = *p;
		*p = *firstdig;
		*firstdig = temp;	// swap *p and *firstdig
		--p;
		++firstdig;		// advance to next two digits
	}
	while( firstdig < p );		// repeat until halfway

	// If there aren't enough characters in the result,
	// pad it out with leading blanks.
	nLength = strlen( buf );
	if( nDigits > nLength )
	{
		// Shift everthing right by the required amount
		int	i;
		int	nSize = nDigits - nLength;
		for( i = nLength + 1; i >= 0 ; i-- )
			*(buf + nSize + i) = *(buf + i);

		// Insert the blanks
		for( i = 0; i < nSize; i++ )
			*(buf + i) = ASCII_BLANK;
	}
}




