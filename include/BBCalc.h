//////////////////////////////////////////////////////
// BBCalc.h
//
// Batch calculation functions
//
// M.McKiernan							14-09-2004
// First pass
// M.McKiernan							20-09-2004
// Added CheckForStagedFill()
// Added SetCopyFlowRatesCommand()
// M.McKiernan							28-09-2004
// Added CalculateColourCompensation()
// Added NewTargetWeightEntry()
// M.McKiernan							06-10-2004
// added CheckProductionSetpointsTotal100Percent
// Added TareWeighHopper
// 
// P.Smith                      23/2/06
// first pass at netburner hardware conversion.
// Bool -> BOOL
//#include <basictypes.h>
// 
// P.Smith                      5/12/07
// added CheckComponentPercentages
//
// P.Smith                          30/1/08
// correct compiler warnings return void from CheckComponentPercentages
//
// P.Smith                          8/6/09
// added CheckForCorrectComponentTargets
//////////////////////////////////////////////////////

#ifndef __BBCALC_H__
#define __BBCALC_H__

#include <basictypes.h>

void CalculateWeightAfterRegrind( void );
void CheckForStagedFill( void );
void SetCopyFlowRatesCommand( void );
void CalculateColourCompensation( void );
void NewTargetWeightEntry( void );
BOOL CheckProductionSetpointsTotal100( void );
void TareWeighHopper( void );
void CalculateValveLatency( void );
void CheckComponentPercentages( void );
void CheckForCorrectComponentTargets( void );


#endif	// __BBCALC_H__


