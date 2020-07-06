///////////////////////////////////////////////////////////
// RRFormat.h
//
// Declaration of the format of the Round Robin (CPI) data
// structure
//
//
// M.McKiernan								11-08-2004
// First pass.
// M.McKiernan								01-09-2004
// m_bIgnoreCPIRR defined as BOOL, had been an int.
// 
// P.Smith                      2/2/06
// first pass at netburner hardware conversion.
// Bool -> BOOL
// included  <basictypes.h>
// 
// P.Smith                      14/5/07
// added m_nResetCPIRRCtr


//////////////////////////////////////////////////////////

//*****************************************************************************
// MACROS
//*****************************************************************************
#ifndef __RRFORMAT_H__
#define __RRFORMAT_H__

//*****************************************************************************
// INCLUDES
//*****************************************************************************
#include "General.h"
#include <basictypes.h>


#define	MAX_CPI_RR_SAMPLES	5	// max samples in CPI round robin

// g_CurrentRecipe.m_LayerData[i].m_fLayerRatio

//  Component CPI rr data
typedef struct {
   BOOL   m_bIgnoreCPIRR;     // when set, RR CPI is not updated.
	int	 m_nResetCPIRRCtr;
	int	m_nSummationCounter;	// samples in CPI RR, i.e. in summation.
	int	m_nCPIRRIndex;		// index or pointer for next CPI value.
   double m_ffCPI[MAX_CPI_RR_SAMPLES];  // each CPI entry in RR
   double m_ffCPISummation;
   double m_ffCPIAverage;
	int	m_nErrorCounter;

} structComponentCPIRRData;

// overall CPI RR structure.
typedef struct {
	
	structComponentCPIRRData	m_ComponentCPIRRData[MAX_COMPONENTS];

} structCPIRRData;

#endif	// __RRFORMAT_H__
