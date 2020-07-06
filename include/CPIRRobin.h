//////////////////////////////////////////////////////
// CPIRRobin.h
//
// Round robin functions for CPI
//
// M.McKiernan							12-08-2004
// First pass
// 
// P.Smith                      27/2/06
// first pass at netburner hardware conversion.
// Bool -> BOOL
//#include <basictypes.h>
//
// P.Smith                      25/8/09
// added MAX_FLOW_RATE_EXCEEDED_CTR_NO, MAX_FLOW_RATE_PERCENTAGE_DEVIATION_ALLOWED
// added GetRRobinData and CheckForValidFlowRate
//
// P.Smith                      17/10/09
// changed MAX_FLOW_RATE_PERCENTAGE_DEVIATION_ALLOWED to 10%
///////////////////////////////////////////////////////

#ifndef __CPIRROBIN_H__
#define __CPIRROBIN_H__
#include <basictypes.h>

#define     MAX_FLOW_RATE_EXCEEDED_CTR_NO                   3
#define     MAX_FLOW_RATE_PERCENTAGE_DEVIATION_ALLOWED      10.00f

BOOL CalculateCPIRoundRobin( int nComponent );
void CalculateWorkCPIRoundRobin( int nComponent );
void ResetWorkCPIRoundRobin( int nComponent );
int GetFillingStage( int nComponent );
void ResetRoundRobinFlowRateForAllComponents( void );
void GetRRobinData( int nComponent);
void CheckForValidFlowRate( float ffCPIInst,int nComponent);




#endif	// __CPIRROBIN_H__


