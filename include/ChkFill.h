//////////////////////////////////////////////////////
// ChkFill.h
//
// Batch monitoring routines.
//
// M.McKiernan							04-08-2004
// First pass
//
// P.Smith                      23/2/06
// first pass at netburner hardware conversion.
// Bool -> BOOL
//#include <basictypes.h>
//
// P.Smith                      7/4/10
// added MAX_FILL_TIME_TO_INCREMENT_TO
// FILL_TIME_INCREMENT

//////////////////////////////////////////////////////

#ifndef __CHKFILL_H__
#define __CHKFILL_H__
#include <basictypes.h>

BOOL CheckFillAccuracy( void );
BOOL *GetHasRetriedPointer( int nComponent );
int *GetRetryCounter( int nComponent );
long GetComponentMinTime( int nComponent);
void CalculateNewTargetTime( void );

#define MAX_FILL_TIME_TO_INCREMENT_TO	(160)
#define FILL_TIME_INCREMENT	            (8)



#endif	// __CHKFILL_H__


