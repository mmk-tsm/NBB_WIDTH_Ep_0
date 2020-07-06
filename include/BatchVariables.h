/////////////////////////////////////////////////
// BatchVariables.h
//
// Include file containing batch global variables
// used in more than one class or function.
//
// M.McKiernan							17-06-2004
// First pass
//
// P.Smith							   23/1/06
// Conversion to netburner pass #1
// BOOL - > BOOL
//
// P.Smith							   30/1/08
// removed g_nPITCom
//////////////////////////////////////////////////

//*****************************************************************************
// MACROS
//*****************************************************************************
#ifndef __BATCHVARIABLES_H__
#define __BATCHVARIABLES_H__

//#include "Hardware.h"
#include "General.h"
// #include "BatchMBIndices.h"
// #include "BatchCalibrationFunctions.h"

long	g_lRawAtDCounts;				// Raw A/D counts (long int)
float 	g_fWeightAtoDCounts;
long	g_lOneSecondWeightAverage;

long	g_lHopperTareCounts;			//long int
long	g_lCurrentLCAtDTare;			//long int
long	g_lCurrentLCAtDValue;		//long int



BOOL	g_bFillIsOnCmp[MAX_COMPONENTS];					// CLEAR flag to indicate component fill is OFF.

int	g_bFillStatCmp[MAX_COMPONENTS];
int	g_bLFillStatCmp[MAX_COMPONENTS];
BOOL	g_bFillStatusAll;
int	g_nStageSeq[MAX_COMPONENTS];
BOOL	g_bDumpStatus;
BOOL	g_bOutputDiagMode;
BOOL	g_bActivateLatency;

BOOL	g_bAutoCycleTestFlag;
BOOL	g_bAutoCycleFlag;
BOOL	g_bWeightAvailableFlag;

BOOL	g_bFirstTime;
unsigned int	g_nPreviousTime;
unsigned int	g_nCurrentTime;
BOOL	g_bIsCounting;
long	g_lTimeMeasured;						// clear time measured variable (long)

float		g_fComponentSetWeight[MAX_COMPONENTS];
float		g_fComponentActWeight[MAX_COMPONENTS];
BOOL		g_bComponentValveState[MAX_COMPONENTS];
unsigned int g_nOperationalStatus;
int		g_nLFillStatCtr[MAX_COMPONENTS];

long	g_lCmpOpenClk[MAX_COMPONENTS];

unsigned char	g_cSeqTable[MAX_COMPONENTS + 4];			// leave space for delimiter.
BOOL		g_bFirstWCycle;
BOOL		g_bRegrindPresent;
unsigned char g_cResetRRobinCmp[MAX_COMPONENTS];



#endif	// __BATCHVARIABLES_H__
