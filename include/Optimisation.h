// 
// P.Smith                      26/6/07
// Optimisation header file.
//
// P.Smith                      2/7/07
// added ComponentOrderLengthReached
// added EnoughMaterial,PresetOptimsationvariables, CheckForFlapFullToEmptyTransition
// CalculateLoaderEmptyingRate, EstimateLoaderWeight
//
// P.Smith                      20/7/07
// ComponentInRecipe
// CheckForAutomaticPurging
// 
//////////////////////////////////////////////////////

#ifndef __OPTIMISATION_H__
#define __OPTIMISATION_H__

#include <basictypes.h>

void SetOptimisationLoaderStatus( void );
void SetOptimisationLoaderFillStatus( void );
void DecideIfLoaderShouldBeEnabled( void );
void SetOptimisationLoaderFillOutputStatus( void );
BOOL EnoughMaterial( int n );
void CheckForFlapFullToEmptyTransition( void );
void CalculateLoaderEmptyingRate( int n );
void EstimateLoaderWeight( void );
BOOL ComponentInRecipe( int n );
void CheckForAutomaticPurging( void );



#endif   // __OPTIMISATION_H__

