//////////////////////////////////////////////////////
// MonBat.h
//
// Batch monitoring routines.
//
// M.McKiernan							28-07-2004
// First pass
// M.McKiernan							28-09-2004
// added FindColourComponent.
// 
// P.Smith                      23/2/06
// first pass at netburner hardware conversion.
//#include <basictypes.h>
// 
// P.Smith                      2/3/07
// added SaveFlowRatesToNonVolatileMemory,GenerateFlowRateChecksum,CheckFlowRateChecksum( void );
// SaveFlowRatesToNonVolatileMemory
// 
// P.Smith                      6/12/07
// added CalculateActualComponentPercentagesRR
//
// P.Smith                      22/7/09
// added PulsingTargetReached
//
// P.Smith                      15/9/09
// removed PulsingTargetReached
////////////////////////////////////////////////////////

#ifndef __MONBAT_H__
#define __MONBAT_H__

#include <basictypes.h>

void MonitorBatch( void );
void ResetFillCounters( void );
void StartFillSequence( void );

void CompAgain( void );
void IncToNx( void );
void Same1( void );
void NoStorD( void );
void GoToNextComp( void );
void EndCycle( void );
void ChkCPI( void );
void NotTup1( void );
void CalcRest( void );

void StoreEndCounts( void );

void CalculateComponentTargetWeight( void );
void Comp1stxTotWgt( void );
void CalculateComponentTargetN( int nTargCompNo );
void CalculateComponentTargets( void );

void CalculateActualPercentages( void );
void CalculateActualComponentPercentages( void );
void CalculateActualComponentPercentagesRR( void );
void CalculateHopperWeightAvg( void );
void CalculateHopperWeight( void );

double *SetCPIPointer( int nComponent );
void CalculateCPI( int nComponent );
void CalcCompNTime( int nComponent );
void NeedForTopUp( void );
void CalcNextTargets( void );
void CheckForRetries( void );
void CheckForSkippedComponent( void );

void FindColourComponent( void );
void SaveFlowRatesToNonVolatileMemory( void );
double GenerateFlowRateChecksum( void );
void CheckFlowRateChecksum( void );
void SaveFlowRatesToNonVolatileMemory( void );

#endif	// __MONBAT_H__


