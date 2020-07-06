//////////////////////////////////////////////////////
// 
// P.Smith                              24/1/08
// Batch statistical data functions
// 
// P.Smith                              7/5/08
// remove #define NOSHACHECKNO double definition
//////////////////////////////////////////////////////



#ifndef __BATCHSTATISTICS_H__
#define __BATCHSTATISTICS_H__

#include <basictypes.h>
#include "General.h"

#define HARDWARESERIALNO              (8)

void BatchStatisticalAnalysis( void );
void RecordComponentHighestWeight( void );
void RecordComponentLowestWeight( void );
void CalculateComponentWeightStdev( void );
void CalculateComponentWeightMean( void );
void ResetSampleCounter( void );
void CopyComponentStatisticsToModbus( void );


 #endif   // __BATCHSTATISTICS_H__

        
       
