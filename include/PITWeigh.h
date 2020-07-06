//////////////////////////////////////////////////////
// PITWeigh.h
//
// Group of functions related to weighing/cycling of batch on PIT.
//
// M.McKiernan							16-06-2004
// First Pass
// 
// P.Smith                      20/2/06
// first pass at netburner hardware conversion.
// Bool -> BOOL
// included  <basictypes.h>

//////////////////////////////////////////////////////
#ifndef __PITWEIGH_H__
#define __PITWEIGH_H__
#include <basictypes.h>


void CmpNOff( int nComponent, BOOL bMBMode );						// not component no. is 1 based, i.e. first is 1 not 0.
																				// MBMode is set (TRUE) to achieve equivalent of MBCMPNOFF.
void CmpNOn( int nComponent, BOOL bMBMode );						// note component no. is 1 based, i.e. first is 1 not 0.

void AddRawForAutoCycyle( void );					//
void TotaliseCount( void );
BOOL CheckComponentUsed( void );						//
void SetMode1( void );
void ByTime( void );
void PITWeigh( void );
void CloseAll( void );
void AtTargt( void );
void CheckWeight( void );
int GetLatency( void );
void CalculateSeconds( void );

#endif	// __PITWEIGH_H__
