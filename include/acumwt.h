//////////////////////////////////////////////////////
// ControlAlgorithm.h
//
//
// P.Smith                        1-02-2005
// First pass
//
// 
// P.Smith                      23/2/06
// first pass at netburner hardware conversion.
// Bool -> BOOL
//#include <basictypes.h>
// 
// P.Smith                      28/2/06
// added  TerminateBatching
// 
// P.Smith                      16/10/08
// added  ZeroLongTermWeightResettableAccumulator
// 
// P.Smith                      27/3/08
// added SaveOrderWeightToNonVolatileMemory, LoadOrderWeightFromVolatileMemory( void )
// added STORAGE_INTERVAL_IN_MINS
//////////////////////////////////////////////////////

#ifndef __ACUMWT_H__
#define __ACUMWT_H__

#include <basictypes.h>

#define     STORAGE_INTERVAL_IN_MINS        (5)

void AccumulateWeights( void );
void AccumulateComponentWeights( void );    
void CheckIfAtBatchWeight( void );   
void ZeroLongTermWeightResettableAccumulator( void );   
void ZeroLongTermWeightAccumulator( void );   

void TerminateBatching( void );   
void SaveOrderWeightToNonVolatileMemory( void );
void LoadOrderWeightFromVolatileMemory( void );

#endif   // __ACUMWT_H__


