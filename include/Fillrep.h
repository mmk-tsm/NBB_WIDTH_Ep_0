//////////////////////////////////////////////////////
// FillRep.h
//
// Multiple filling routines.
//
// P.Smith                                 19-08-2004
// First pass
// M.McKiernan                             16-09-2004
// Added CalculateStage2Fill, CalculateStage3Fill,CalculateFinalWeight, CalculateTargetStage3 
// Added StoreStageFillData, CheckForMultipleToNormalFill
// 
// P.Smith                      23/2/06
// first pass at netburner hardware conversion.
// Bool -> BOOL
//#include <basictypes.h>
//////////////////////////////////////////////////////

#ifndef __FILLREP_H__
#define __FILLREP_H__
#include <basictypes.h>


void CalcPerTarget( void );
BOOL CalculateStage2Fill( int nComponent  );
BOOL CalculateStage3Fill( int nComponent  );
void CalculateFinalWeight( int nComponent ); 
void CalculateTargetStage3( int nComponent, long lDiff ); 
void StoreStageFillData( int nComponent );
void CheckForMultipleToNormalFill( void );


#endif  // __FILLREP_H__


