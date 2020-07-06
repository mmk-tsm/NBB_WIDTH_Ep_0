//////////////////////////////////////////////////////
// ftoa
//
// Float to Ascii conversion
//
// M.Parks				21-01-2000
// M.McKiernan				20-06-2001
// Quite a few changes - to fix bug in displaying no. like 0.0321.  'old' version in old_ftoa.c.
//
// P.Smith                      26/1/06
// first pass at netburner hardware conversion.
// included  <basictypes.h>
// included "string.h"
//
// P.Smith                      29/5/06
// remvoed int nZeroes = 0;

//////////////////////////////////////////////////////

//*****************************************************************************
// INCLUDES
//*****************************************************************************
#include <basictypes.h>

#include "itoa.h"
#include "ftoa.h"
#include <Stdio.h>
#include "General.h"
#include "string.h"

void ftoa( char *cBuf, double fValue, int nDigits, int nDecimals )
{
	float	fCompareValue = fValue;
	long	lValue;
	int		i;

	// Remove the decimal value
	lValue = (long)fCompareValue;
	fCompareValue = (float)fValue;

	// save the number of decimal places
	if( nDecimals )
	{
		if( nDecimals > MAX_DISPLAY_DECIMALS )
			nDecimals = MAX_DISPLAY_DECIMALS;

          switch( nDecimals )
          {
          case 1:   // 
               fValue *= 10;
               fCompareValue *= 10;
               break;
          case 2:   // 
               fValue *= 100;
               fCompareValue *= 100;
               break;
          case 3:   // 
               fValue *= 1000;
               fCompareValue *= 1000;
               break;
          case 4:
          default:   //max nDecimals = 4 
               fCompareValue *= 10000;
               fValue *= 10000;
               break;
          }
		

//		for( i = 0; i < nDecimals; i++ )
//		{
//			fValue *= 10;
//			fCompareValue *= 10;
//		}

		// There is a rounding off problem with floats so we compensate
		// for this by adding 1 to the final value.   We only do this if
		// there are decimal places and the value is non-zero.
		if( fValue != 0.0f && fValue != fCompareValue )
			fValue += 1;
	}

	// Convert to an ascii string
	lValue = (long)fValue;

	// The integer to ascii conversion will pad the string
	// out with leading blanks to the required length
	IntToAscii( lValue, cBuf, 10, nDigits );

	// Insert the decimal point in the
	// correct position in the string
	if( nDecimals )
	{
		int nShifts = 0;
		int nLength;
		int nPosition;
		nLength = strlen( cBuf );
//
		if(nLength <= nDecimals)
			{			// must insert leading zeroes  0.0123 will be 1,2,3, nead 2 zeroes
						// .1234 will be 1234 -> 01234.
				i = 0;
				while(cBuf[i] != '\0')  // replace leading blanks by zeroes.
				{
					if(cBuf[i] == ASCII_BLANK)
						cBuf[i] = '0';
					i++;
				}

				nShifts = nDecimals - nLength + 1;
				for( i = nLength + nShifts; i >= nShifts; i-- )
				  cBuf[i] = cBuf[i - nShifts];
				for( i = nShifts-1; i>-1; i--)
					cBuf[i] = '0';

			}

//		
		// Move back one more to ensure a leading zero
//		nPosition = nLength - nDecimals - 1;

		// By doing the check for <= we can
		// ensure that there is always a zero in
		// front of the decimal place.  eg 0.1234
//		while( nPosition <= nLength )
//		{
			// Insert the zero
//			if( cBuf[nPosition] == ASCII_BLANK )
//				cBuf[nPosition] = '0';

//			nPosition++;
//		}

//n		nLength = nLength + nShifts;		//new length of buffer including leading zeroes
		nLength = strlen( cBuf );

		// make a space for the decimal place
		nPosition = nLength - nDecimals;
		if( nDecimals )
		{
			for( i = nLength + 1; i >= nPosition; i-- )
			{
				if(cBuf[i-1] == ASCII_BLANK )  //replace blanks after dec point by zeroes.
					cBuf[i-1] = '0';

				cBuf[i] = cBuf[i - 1];
			}

			// Put the decimal character into the space in the string
			cBuf[nPosition] = '.';
			
			nLength++;
		}
		// Ensure that the string is null terminated
		cBuf[nLength] = '\0';
	}
}





