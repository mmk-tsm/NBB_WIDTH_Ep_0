/*
//////////////////////////////////////////////////////
// CycleMonitor.h
//
// Allows the user to set the current time and date.
//
// M.McKiernan                          28-07-2004
// First pass
// M.McKiernan                          07-09-2004
// Added NoSensorOnline
// DoPhase2Clean( void );
// InitiateCleanCycle( void );
// StartCleanCycle( void );
// SetMixerTime( void );
// ResetCleanFinishMBBit( void );
// SetStartCleanMBBit( void );
// CheckForCleanPhase2( void );
// CheckHopperWeight( void );
//
// P.Smith                              31-03-2005
// SetEndCleanMBBit ( void ) SetLevelSensorWatchDog( void )
//
// P.Smith                              5/10/05
// added void ResetCleanStartMBBit( void )
// 
// P.Smith                      23/2/06
// first pass at netburner hardware conversion.
// Bool -> BOOL
//#include <basictypes.h>
// 
// P.Smith                      16/11/06
// Remove SetLevelSensorWatchDog
// 
// P.Smith                      23/6/08
// added WeighHopperHasOpened
//
// P.Smith                      26/9/08
// added DumpMaterialFromWeighHopper & CheckIfMaterialHasDumped     
//
// P.Smith                      26/6/08
// remove CheckOffTime
//
// P.Smith                      7/9/09
// added GetMinimumHopperWeight
//////////////////////////////////////////////////////

*/
#ifndef __CYCLEMONITOR_H__
#define __CYCLEMONITOR_H__

#include <basictypes.h>

void    CycleMonitor( void );
void MakeCycle( void );
void DumpingWeightCheck( void );
void StartPhase1Clean( void );
void ClearUnused( void );
void NoSensorOnline( void );
void DoPhase2Clean( void );
void InitiateCleanCycle( void );
void StartCleanCycle( void );
void SetMixerTime( void );
void ResetCleanFinishMBBit( void );
void SetStartCleanMBBit( void );
void CheckForCleanPhase2( void );
void CheckHopperWeight( void );
void SetEndCleanMBBit ( void );
void UpdateBatchVariables( void );
void CheckForSingleRecipeClean( void );
void ResetCleanStartMBBit( void );
BOOL WeighHopperHasOpened( void );
void DumpMaterialFromWeighHopper( void );     
void CheckIfMaterialHasDumped( void );     
float GetMinimumHopperWeight(unsigned int nBlenderType);


#endif  // __CYCLEMONITOR_H__


