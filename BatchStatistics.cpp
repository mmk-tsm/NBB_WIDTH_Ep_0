
//////////////////////////////////////////////////////
// P.Smith                      24/1/08
// added BatchStatisticalAnalysis,RecordComponentHighestWeight,RecordComponentLowestWeight
// CalculateComponentWeightStdev,CalculateComponentWeightMean
// checked mean, stdev, highest, lowest calculation all functioning okay.
//
// P.Smith                      28/1/08
// added check for g_nComponentWeightSampleCounter[nCompIndex] being greater than or equal to the max storage
// capacity for this buffer, if it is set the sample counter to 0
//
// P.Smith                          30/1/08
// correct compiler warnings
//
// P.Smith                          30/1/08
// calculate g_fComponentWeightStdDevPercentage
//
//
// P.Smith                          8/12/08
// add high low value for g_sOngoingHistoryComponentLogData high low value.
// check g_bHourlyResetMin to determine if it should be reset.
//
// P.Smith                          8/12/08
// batch statistics, if g_bComponentHasRetried is set then do not include this in the
// standard dev calculation.
//
// P.Smith                          4/2/10
// clear g_bComponentHasRetried after the statistical analysis has been
// run.
//////////////////////////////////////////////////////


#include <stdio.h>

#include "SerialStuff.h"
#include "BatchMBIndices.h"
#include "BatchCalibrationFunctions.h"
#include "General.h"
#include "ConversionFactors.h"

#include "16R40C.h"
#include "ConfigurationFunctions.h"
#include "MBMHand.h"
#include "MBSHand.h"

#include "MBProgs.h"
#include "SetpointFormat.h"
#include "TimeDate.h"
#include "BatVars.h"
#include "BBCalc.h"
#include "MBTogSta.h"
#include "Alarms.h"
#include "KghCalc.h"
#include "BatVars2.h"
#include "Monbat.h"
#include "Blrep.h"
#include "MBProsum.h"
#include "Pause.h"
#include "CycleMonitor.h"
#include "TSMPeripheralsMBIndices.h"

#include <basictypes.h>
#include <string.h>
#include "NBBgpio.h"
#include "ConversionFactors.h"
#include "MultiblendMB.h"
#include "Mtttymux.h"
#include "Secret.h"
#include "OneWire.h"
#include <fastmath.h>
#include "BatchStatistics.h"
#include "HistoryLog.h"


extern CalDataStruct    g_CalibrationData;



//////////////////////////////////////////////////////
// RecordComponentHighestWeight( void )
//
//////////////////////////////////////////////////////

void BatchStatisticalAnalysis( void )
{
    // copy the component weights to the sample buffer.
    int i,nCompIndex;
    i = 0;
    // copy component weights to sample buffer
    while(g_cSeqTable[i] != SEQDELIMITER && i < MAX_COMPONENTS) // table end or delimiter
    {
   	    nCompIndex = (int)(g_cSeqTable[i] & 0x0F) - 1;  // component no. in l.s. nibble
    	if(!g_bComponentHasRetried[nCompIndex])
    	{
    	    g_nComponentWeightSampleCounter[nCompIndex]++;
    	    if( g_nComponentWeightSampleCounter[nCompIndex] >= MAX_CYCLE_DIAGNOSTICS_SAMPLES)
    	    {
    	    	g_nComponentWeightSampleCounter[nCompIndex] = 0;
    	    }
    	    g_fComponentWeightSamples[nCompIndex][g_nComponentWeightSampleCounter[nCompIndex]-1] = g_fComponentActualWeight[nCompIndex];
    	}
	    i++;
    }
    CalculateComponentWeightMean();
    CalculateComponentWeightStdev();
    RecordComponentHighestWeight();
    RecordComponentLowestWeight();
    CopyComponentStatisticsToModbus();
    for(i= 0; i < MAX_COMPONENTS; i++)
    {
        g_bComponentHasRetried[i] = FALSE;   // reset retry flags
    }

}



//////////////////////////////////////////////////////
// RecordComponentHighestWeight( void )
//
//////////////////////////////////////////////////////

void RecordComponentHighestWeight( void )
{
    int i,nCompIndex,nIndex;
    BOOL bResetMax = FALSE;
    i = 0;
    float fComponentSampleData;
    if(g_bHourlyResetMax)
    {
        g_bHourlyResetMax = FALSE;
        bResetMax = TRUE;
    }
    // copy component weights to sample buffer
    while(g_cSeqTable[i] != SEQDELIMITER && i < MAX_COMPONENTS) // table end or delimiter
    {
        nCompIndex = (int)(g_cSeqTable[i] & 0x0F) - 1;  // component no. in l.s. nibble
        nIndex = g_nComponentWeightSampleCounter[nCompIndex]-1;
        fComponentSampleData = g_fComponentWeightSamples[nCompIndex][nIndex];
        if(bResetMax)
        {
            g_sOngoingHistoryComponentLogData.g_fComponentWeightMaximum[nCompIndex] = fComponentSampleData;
        }
        if(nIndex == 0)
        {
            g_fComponentWeightMaximum[nCompIndex] = fComponentSampleData;
            g_sOngoingHistoryComponentLogData.g_fComponentWeightMaximum[nCompIndex] = fComponentSampleData;
         }
        else
        {
            if(g_fComponentWeightMaximum[nCompIndex] < fComponentSampleData)
            {
                g_fComponentWeightMaximum[nCompIndex] = fComponentSampleData;
            }
            if(g_sOngoingHistoryComponentLogData.g_fComponentWeightMaximum[nCompIndex] < fComponentSampleData)
            {
                g_sOngoingHistoryComponentLogData.g_fComponentWeightMaximum[nCompIndex] = fComponentSampleData;
            }
        }
        i++;
    }
}

//////////////////////////////////////////////////////
// RecordComponentLowestWeight( void )
//
//////////////////////////////////////////////////////

void RecordComponentLowestWeight( void )
{
    int i,nCompIndex,nIndex;
    BOOL    bResetMin = FALSE;
    i = 0;
    float fComponentSampleData;
    // copy component weights to sample buffer
    if(g_bHourlyResetMin)
    {
        g_bHourlyResetMin = FALSE;
        bResetMin = TRUE;
    }
    while(g_cSeqTable[i] != SEQDELIMITER && i < MAX_COMPONENTS) // table end or delimiter
    {
        nCompIndex = (int)(g_cSeqTable[i] & 0x0F) - 1;  // component no. in l.s. nibble
        nIndex = g_nComponentWeightSampleCounter[nCompIndex]-1;
        fComponentSampleData = g_fComponentWeightSamples[nCompIndex][nIndex];
        if(bResetMin)
        {
            g_sOngoingHistoryComponentLogData.g_fComponentWeightMinimum[nCompIndex] = fComponentSampleData;
        }
        if(nIndex == 0)
        {
            g_sOngoingHistoryComponentLogData.g_fComponentWeightMinimum[nCompIndex] = fComponentSampleData;
            g_fComponentWeightMinimum[nCompIndex] = fComponentSampleData;
        }
        else
        {
            if(g_fComponentWeightMinimum[nCompIndex] > fComponentSampleData)
            {
                g_fComponentWeightMinimum[nCompIndex] = fComponentSampleData;
            }
             if(g_sOngoingHistoryComponentLogData.g_fComponentWeightMinimum[nCompIndex] > fComponentSampleData)
            {
                g_sOngoingHistoryComponentLogData.g_fComponentWeightMinimum[nCompIndex] = fComponentSampleData;
            }

        }
        i++;
    }
}


//////////////////////////////////////////////////////
// CalculateComponentWeightStdev( void )
//
//////////////////////////////////////////////////////

void CalculateComponentWeightStdev( void )
{
    int i,j,nCompIndex;
    i = 0;
    float fSumOfSquares,fTemp;
    while(g_cSeqTable[i] != SEQDELIMITER && i < MAX_COMPONENTS) // table end or delimiter
    {
        nCompIndex = (int)(g_cSeqTable[i] & 0x0F) - 1;  // component no. in l.s. nibble
    // sum  x - x bar
       fSumOfSquares = 0.0;
       for( j = 0; j < g_nComponentWeightSampleCounter[nCompIndex]; j++ )
       {
              fTemp = (g_fComponentWeightSamples[nCompIndex][j] - g_fComponentWeightMean[nCompIndex]);
              fSumOfSquares += (fTemp * fTemp);
       }
       // divide by the sample no to calculate the mean
       if(g_nComponentWeightSampleCounter[nCompIndex] == 1)
       {
           g_fComponentWeightStdDev[nCompIndex] = 0.0;
       }
       else
       {
           g_fComponentWeightStdDev[nCompIndex] = sqrtf(fSumOfSquares / (g_nComponentWeightSampleCounter[nCompIndex]-1));
       }
       g_fComponentWeightStdDevPercentage[nCompIndex] = (100.0 * g_fComponentWeightStdDev[nCompIndex])/ g_fComponentTargetWeight[nCompIndex];
       i++;
    }
}




//////////////////////////////////////////////////////
// CalculateComponentWeightMean( void )
//
//////////////////////////////////////////////////////

void CalculateComponentWeightMean( void )
{
    int i,j,nCompIndex;
    i = 0;
    float fAddition;
    while(g_cSeqTable[i] != SEQDELIMITER && i < MAX_COMPONENTS) // table end or delimiter
    {
        nCompIndex = (int)(g_cSeqTable[i] & 0x0F) - 1;  // component no. in l.s. nibble
    // sum all the sample and divide by the sample no.
       fAddition = 0.0;
       for( j = 0; j < g_nComponentWeightSampleCounter[nCompIndex]; j++ )
       {
              fAddition += g_fComponentWeightSamples[nCompIndex][j];
       }
       // divide by the sample no to calculate the mean
       g_fComponentWeightMean[nCompIndex] = fAddition / g_nComponentWeightSampleCounter[nCompIndex];
       i++;
    }
}






//////////////////////////////////////////////////////
// ResetSampleCounter( void )
// resets sample counter to 0.
//////////////////////////////////////////////////////

void ResetSampleCounter( void )
{
    int i;
    for(i=0; i < MAX_COMPONENTS; i++)
    {
        g_nComponentWeightSampleCounter[i] = 0;
    }
}



//////////////////////////////////////////////////////
// CopyComponentStatisticsToModbus( void )
// resets sample counter to 0.
//////////////////////////////////////////////////////

void CopyComponentStatisticsToModbus( void )
{
    unsigned int i;
    float fTemp;
    union WordAndDWord uValue;

    for( i=0; i<g_CalibrationData.m_nComponents; i++)
    {
    // sample counter
       g_arrnMBTable[BATCH_STATICSTICS_SAMPLE_COUNTER_1 + i] = g_nComponentWeightSampleCounter[i]; // copy msb
    }

        // max value

    for( i=0; i<g_CalibrationData.m_nComponents; i++)
    {
        fTemp = g_fComponentWeightMaximum[i];
        fTemp += 0.00005;
        uValue.lValue = (long) (10000 * fTemp);
        g_arrnMBTable[BATCH_STATICSTICS_MAXIMUM_1 + (i*2)] = uValue.nValue[0]; // copy msb
        g_arrnMBTable[BATCH_STATICSTICS_MAXIMUM_1 + (i*2)+1] = uValue.nValue[1]; // copy lsb
     }

        // min value
    for( i=0; i<g_CalibrationData.m_nComponents; i++)
    {
        fTemp = g_fComponentWeightMinimum[i];
        fTemp += 0.00005;
        uValue.lValue = (long) (10000 * fTemp);
        g_arrnMBTable[BATCH_STATICSTICS_MINIMUM_1 + (i*2)] = uValue.nValue[0]; // copy msb
        g_arrnMBTable[BATCH_STATICSTICS_MINIMUM_1 + (i*2)+1] = uValue.nValue[1]; // copy lsb
     }
 // mean
    for( i=0; i<g_CalibrationData.m_nComponents; i++)
    {
         fTemp = g_fComponentWeightMean[i];
        fTemp += 0.00005;
        uValue.lValue = (long) (10000 * fTemp);
        g_arrnMBTable[BATCH_STATICSTICS_MEAN_1 + (i*2)] = uValue.nValue[0]; // copy msb
        g_arrnMBTable[BATCH_STATICSTICS_MEAN_1 + (i*2)+1] = uValue.nValue[1]; // copy lsb
    }

// standard deviation
    for( i=0; i<g_CalibrationData.m_nComponents; i++)
    {
         fTemp = g_fComponentWeightStdDev[i];
        fTemp += 0.00005;
        uValue.lValue = (long) (10000 * fTemp);
        g_arrnMBTable[BATCH_STATICSTICS_STDEV_1 + (i*2)] = uValue.nValue[0]; // copy msb
        g_arrnMBTable[BATCH_STATICSTICS_STDEV_1 + (i*2)+1] = uValue.nValue[1]; // copy lsb
    }
}


