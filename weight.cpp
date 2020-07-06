///////////////////////////////////////////////////////////
// SPIFunctions.c
//
// A library of functions for accessing the SPI EEPROM.
//
//
// M.Parks                              12-04-2000
// Converted from Batch Blender assembler.
// M.McKiernan  -   TransmitSPI(), Port C on HC16 initialised. PORTC_DataReg
// M.McKiernan -                            26-06-2001
//              Changed to ReadOnBoardEEPROMData - see //mmk260601 to //mmk260601
// M.McKiernan                                  03-09-2004
// Changed initialisation to g_nPreviousWeightAvgFactor = 0; (before was being set to FALSE)
// Added some more initialisations
//   g_fWeightInHopper = 0.0;  g_fWeightInHopperMinusRegrind = 0.0;    
//   g_fPreviousBatchWeight = 0.0;  g_fCurrentBatchWeight = 0.0;            
// M.McKiernan                     29-10-2001
//     #include "hc16.h"        // for Metrowerk CW - hc16.h copied into BatchBlender_c folder.
//
// P.Smith                              26/09/05
// Correct read of on board eeprom.
// was reading from SPIReceiveRAM + 7 instead of SPIReceiveRAM + 3
//
// P.Smith                              27/09/05
// Correct ReadOnBoardEEPROMStatus read SPIReceiveRAM+1 instead of SPIReceiveRAM
// If the eeprom data is the same as the new data, do not reprogram
//
// P.Smith                              16/12/05
// Correct SettledAverage call only call if g_cBatSeq != BATSEQ_SETTLING and 
// (g_cBatSeq == BATSEQ_CALC))
//
// P.Smith                              11/1/06
// brackets inserted in TransmitSPI in }while( cStatus & SPI_STATUS_BUSY_SET == 0);  //bit is set when finished
// removed   char    cStatus in StartSPI;
// removed nReadTemp2 in ReadOnBoardEEPROMData
// removed nData in ReadRemovableEEPROMData
// remove nData in WriteRemovableEEPROMData
// removed uLongTemp in HandleMAX132
// removed cTemp in HandleMAX132
// removed nTempLSWord,nTemp in HandleMAX132
// 
// P.Smith                      16/1/06
// first pass at netburner hardware conversion.
// Bool -> BOOL
//#include "weight.h"
// modified this file to contain only the part relating to the a/d read.
// name change from SPIFuncs.cpp to weight.cpp.
// added void ProcessLoadCellWeight (void ) to read and average weight.    
// 
// P.Smith                      27/2/06
// added set of g_bWeightAvailableFlag and check for autocycle.
// 
// P.Smith                      28/2/06
// g_lRawAtDCounts = g_lRawADCountsLTC2415 not done when autocycle is running.
// 
// P.Smith                      27/9/07
// calculate no of samples required in lsamples, this will be used in the super
// fast mode
//
// P.Smith                      11/1/08
// if speed to set to super fast, then set averaging to half a second,
// otherwise set to the weight averaging on the hopper calibration page.
// 
//
// P.Smith                      30/1/08
// add #include "rta_system.h"
//
// P.Smith                      9/12/08
// CalculateOngoingMaxMinTareValue when blender is taring.
// this stores the max / min value for the tare value every time the blender tares.
//
// P.Smith                      10/09/09
// swap over orientation of g_CalibrationData.m_bCycleFastFlag
//
// P.Smith                      17/09/09
// change speed check to the way that it was before.
//
// P.Smith                      16/11/09
// added log of a/d reading for stability check. log if g_bLogLoadCellReading is set
// record max / min values on 1st sample.
// disable logging when the a/d values have been stored.
//
// P.Smith                      19/11/09
// copy one second average ref to logged buffer, set g_bDecideOnRefStability to
// initiate analysis of the reference data.
// stop read of a/d data by setting g_bTakeRefAToDReading to false.
// record max /min value
//////////////////////////////////////////////////////////


//*****************************************************************************
// INCLUDES
//*****************************************************************************
//nbb #include <stdlib.h>
#include <stdio.h>
//nbb #include "hc16.h"       // for Metrowerk CW
//nbb #include <Math.h>

#include "General.h"
#include "InitialiseHardware.h"

#include "weight.h"
#include "16r40c.h"
#include "BatVars.h"
#include "BatVars2.h"
#include "BatchCalibrationFunctions.h"
#include "init5270.h"
#include "NBBVars.h"
#include "rta_system.h"
#include "HistoryLog.h"



BOOL    g_bCNV132Flag;                  // CONVERT for MAX132 FLAG




int g_nCNV132No;                    // conversion no.
long    g_lSUM132;                      // MAX132 SUMMATION(long) 
int g_nNEN132;                      // No. OF ENTRIES(in summation).
unsigned int    g_nMAX132ERR;       // note the error uses bits (i.e. not just 1 or 0)
//test stuff
long    g_lCAvCounter = 0;
unsigned int    g_nTestTime;
unsigned int    g_nTestTimeMax = 0;         // max value
unsigned int    g_nTemp1 = 0;
unsigned int   g_nTemp2 = 0;

extern  CalDataStruct   g_CalibrationData;




////////////////////////////////////////////////////////////////
// ReadWeightFromLoadCell                   Handl132
// transfers the weight to g_lRawAtDCounts and averages the reading. 
// Entry:   Void
//
// Exit:    
////////////////////////////////////////////////////////////////   
void ProcessLoadCellWeight (void )
{
     if(g_bAutoCycleFlag)
     {       // in autocycler.
            g_bWeightAvailableFlag = TRUE;          // Indicate a/d conversion complete
            if(g_bDumpStatus)                               // dumping??
            {
                    Tarend();                           // **todo**
            }
     }
     else        // AutoCycleFlag false - normal weighing
     {
         g_lRawAtDCounts = g_lRawADCountsLTC2415;
         g_bWeightAvailableFlag = TRUE;
        
     }
    CalculateAverage();
}


////////////////////////////////////////////////////////////////
// CalculateAverage                 ASM = CALCAVERAGE
// Generates various Weight average values (0.5S, 1S, 2S, 8S)
// Entry:   Void
//
// Exit:    
////////////////////////////////////////////////////////////////   
void CalculateAverage( void )
{
float fRawValue;
unsigned char   cData;
    fRawValue = (float)g_lRawAtDCounts;
//call avg routines inline
//  GenerateHalfSecondWeightAverage();

					 
    if(g_nHalfSecondSumCounter >= ATD_HALF_SECOND_AVERAGE)        // should never be >= 8 on entry, if so re-initialise sum and entry counter.
    {
        g_nHalfSecondSumCounter = 0;
        g_lHalfSecondSum = 0;
    }

    g_lHalfSecondSum += g_lRawAtDCounts;        // Add in current raw A/D value
    g_nHalfSecondSumCounter++;
    if(g_nHalfSecondSumCounter == ATD_HALF_SECOND_AVERAGE)
    {
        g_lHalfSecondWeightAverage = g_lHalfSecondSum / ATD_HALF_SECOND_AVERAGE;      // >>3;
        g_nHalfSecondSumCounter = 0;
        g_lHalfSecondSum = 0;
        g_bHalfSecondWeightAverageAvailable = TRUE;
    }
//  GenerateOneSecondWeightAverage();
    if(g_nOneSecondSumCounter >= ATD_ONE_SECOND_AVERAGE)        // should never be >= 16 on entry, if so re-initialise sum and entry counter.
    {
        g_nOneSecondSumCounter = 0;
        g_lOneSecondSum = 0;
    }
    g_lOneSecondSum += g_lRawAtDCounts;     // Add in current raw A/D value
    g_nOneSecondSumCounter++;
    if(g_nOneSecondSumCounter == ATD_ONE_SECOND_AVERAGE)
    {
        g_lOneSecondWeightAverage = g_lOneSecondSum / ATD_ONE_SECOND_AVERAGE;   //>>4;  // Divide by 16;
        g_nOneSecondSumCounter = 0;
        g_lOneSecondSum = 0;
        g_bOneSecondWeightAverageAvailable = TRUE;

        if(g_bTakeRefAToDReading && (g_nAToDRefStabilityCtr < MAX_STABILITY_READINGS))
        {
            if(g_nAToDRefStabilityCtr == 0) // first reading  ?
            {
            g_nAToDRefStabilityMaxValue = g_lOneSecondWeightAverage;
            g_nAToDRefStabilityMinValue = g_lOneSecondWeightAverage;
            }
            else
            {
                if(g_lOneSecondWeightAverage > g_nAToDRefStabilityMaxValue)
                {
                    g_nAToDRefStabilityMaxValue = g_lOneSecondWeightAverage; // update max value
                }
                if(g_lOneSecondWeightAverage < g_nAToDRefStabilityMinValue)
                {
                    g_nAToDRefStabilityMinValue = g_lOneSecondWeightAverage; // update max value
                }
            }
            g_nAToDRefStabilityLog[g_nAToDRefStabilityCtr] = g_lOneSecondWeightAverage;
            g_nAToDRefStabilityCtr++;
            if(g_nAToDRefStabilityCtr >= MAX_STABILITY_READINGS)
            {
               g_nAToDRefStabilityCtr = 0; 
               g_bTakeRefAToDReading = FALSE;
               g_bDecideOnRefStability = TRUE;
            }
        }



        if(g_bLogLoadCellReading && (g_nAToDStabilityCtr < MAX_STABILITY_READINGS))
        {
            if(g_nAToDStabilityCtr == 0) // first reading  ?
            {
            g_nAToDStabilityMaxValue = g_lOneSecondWeightAverage;
            g_nAToDStabilityMinValue = g_lOneSecondWeightAverage;
            }
            else
            {
                if(g_lOneSecondWeightAverage > g_nAToDStabilityMaxValue)
                {
                    g_nAToDStabilityMaxValue = g_lOneSecondWeightAverage; // update max value
                }
                if(g_lOneSecondWeightAverage < g_nAToDStabilityMinValue)
                {
                    g_nAToDStabilityMinValue = g_lOneSecondWeightAverage; // update max value
                }
            }
            g_nAToDStabilityLog[g_nAToDStabilityCtr] = g_lOneSecondWeightAverage;
            g_nAToDStabilityCtr++;
            if(g_nAToDStabilityCtr >= MAX_STABILITY_READINGS)
            {
               g_nAToDStabilityCtr = 0; 
               g_bLogLoadCellReading = FALSE;
               g_bDecideOnLoadCellStability = TRUE;
 
            }
        }
    }
//  GenerateTwoSecondWeightAverage();
    
    if(g_nTwoSecondSumCounter >= ATD_TWO_SECOND_AVERAGE)        // should never be >= 32 on entry, if so re-initialise sum and entry counter.
    {
        g_nTwoSecondSumCounter = 0;
        g_lTwoSecondSum = 0;
    }
    g_lTwoSecondSum += g_lRawAtDCounts;     // Add in current raw A/D value
    g_nTwoSecondSumCounter++;
    if(g_nTwoSecondSumCounter == ATD_TWO_SECOND_AVERAGE)
    {
        g_lTwoSecondWeightAverage = g_lTwoSecondSum / ATD_TWO_SECOND_AVERAGE;   // >>5; // Divide by  32;
        g_nTwoSecondSumCounter = 0;
        g_lTwoSecondSum = 0;
        g_bTwoSecondWeightAverageAvailable = TRUE;
    }

//  GenerateEighthSecondWeightAverage();
    if(g_nEighthSecondSumCounter >= ATD_EIGHT_SECOND_AVERAGE)      // should never be >= 2 on entry, if so re-initialise sum and entry counter.
    {
        g_nEighthSecondSumCounter = 0;
        g_lEighthSecondSum = 0;
    }
    g_lEighthSecondSum += g_lRawAtDCounts;      // Add in current raw A/D value
    g_nEighthSecondSumCounter++;
    if(g_nEighthSecondSumCounter == ATD_EIGHT_SECOND_AVERAGE)
    {
        g_lEighthSecondWeightAverage = g_lEighthSecondSum / ATD_EIGHT_SECOND_AVERAGE;      // >>7; // Divide by 2;
        g_nEighthSecondSumCounter = 0;
        g_lEighthSecondSum = 0;
        g_bEighthSecondWeightAverageAvailable = TRUE;
    }

    // Filling & Thruput monitor??
    if(g_bFillStatusAll == ON  && g_CalibrationData.m_nBlenderMode == MODE_THROUGHPUTMON)
    {   // set fill by weight bit for Thuput monitor mode.
        cData = g_cSeqTable[g_nSeqTableIndex];
        cData |= FILL_BY_WEIGHT_BIT;
        g_cSeqTable[g_nSeqTableIndex] = cData;
    }
// NOTFILLING
    // BatSeq not filling or at CALC??
     
    if((g_cBatSeq != BATSEQ_SETTLING) && (g_cBatSeq == BATSEQ_CALC))
    {
        // Calc Settled average.
       SettledAverage( );      
    }
    

}     
///////////////////////////////////////////////////////////
// Name: SettledAverage( )              ASM = SETTLEAVG
// Description:
// Calculates the average of a fixed no of A/D readings based on calibration parameter g_CalibrationData.m_nWeightAvgFactor
// The WeightAvgFactor is the time in seconds over which average is calculated.
// AD_SAMPLES_PER_SECOND (defined in General.h) is the no. of A/D samples per second.
// If the averaging factor gets changed, the averaging process is reset.
//
// Entry:   Void
//
// Exit:    g_bWeightAvgAvail flag set to TRUE if newly calculated Average available.
//          g_lSettledAverage is then the new average.
//
// M.McKiernan                  05-07-2004
// First pass.
///////////////////////////////////////////////////////////

void SettledAverage( void )
{
    long lSamples;
    // Is avg. factor different from prev pass?
    if(g_CalibrationData.m_nWeightAvgFactor != g_nPreviousWeightAvgFactor)
    {
        g_nPreviousWeightAvgFactor = g_CalibrationData.m_nWeightAvgFactor;
        // RESETAVG.
        g_nAvgSumCounter = 0;       //reset sum entry counter
        g_lAvgSum = 0;      // reset sum
    }

    // add latest raw data to sum
    g_lAvgSum += g_lRawAtDCounts;
    g_nAvgSumCounter++;

    if(g_CalibrationData.m_bCycleFastFlag)
    {
        lSamples = g_CalibrationData.m_nWeightAvgFactor * AD_SAMPLES_PER_SECOND;
    }
    else
    {
        lSamples = (AD_SAMPLES_PER_SECOND/2);
    }


    if(g_nAvgSumCounter > lSamples)
    {
        // > reqd no of samples (should never happen)
        g_nAvgSumCounter = 0;       //reset sum entry counter
        g_lAvgSum = 0;      // reset sum
    }
    else if(g_nAvgSumCounter == lSamples)
    {
        g_lSettledAverage = g_lAvgSum / g_nAvgSumCounter;
        g_bWeightAvgAvail = TRUE;
        g_nAvgSumCounter = 0;       //reset counter
        g_lAvgSum = 0;      // reset sum
    }

}

void GenerateHalfSecondWeightAverage( void )
{
    if(g_nHalfSecondSumCounter >= 8)        // should never be >= 8 on entry, if so re-initialise sum and entry counter.
    {
        g_nHalfSecondSumCounter = 0;
        g_lHalfSecondSum = 0;
    }
    g_lHalfSecondSum += g_lRawAtDCounts;        // Add in current raw A/D value
    g_nHalfSecondSumCounter++;
    if(g_nHalfSecondSumCounter == 8)
    {
        g_lHalfSecondWeightAverage = g_lHalfSecondSum >>3;
        g_nHalfSecondSumCounter = 0;
        g_lHalfSecondSum = 0;
        g_bHalfSecondWeightAverageAvailable = TRUE;
    }
}

void GenerateOneSecondWeightAverage( void )
{
    if(g_nOneSecondSumCounter >= 16)        // should never be >= 16 on entry, if so re-initialise sum and entry counter.
    {
        g_nOneSecondSumCounter = 0;
        g_lOneSecondSum = 0;
    }
    g_lOneSecondSum += g_lRawAtDCounts;     // Add in current raw A/D value
    g_nOneSecondSumCounter++;
    if(g_nOneSecondSumCounter == 16)
    {
        g_lOneSecondWeightAverage = g_lOneSecondSum >>4;    // Divide by 16;
        g_nOneSecondSumCounter = 0;
        g_lOneSecondSum = 0;
        g_bOneSecondWeightAverageAvailable = TRUE;
    }
}

void GenerateTwoSecondWeightAverage( void )
{
    if(g_nTwoSecondSumCounter >= 32)        // should never be >= 32 on entry, if so re-initialise sum and entry counter.
    {
        g_nTwoSecondSumCounter = 0;
        g_lTwoSecondSum = 0;
    }
    g_lTwoSecondSum += g_lRawAtDCounts;     // Add in current raw A/D value
    g_nTwoSecondSumCounter++;
    if(g_nTwoSecondSumCounter == 32)
    {
        g_lTwoSecondWeightAverage = g_lTwoSecondSum >>5;    // Divide by  32;
        g_nTwoSecondSumCounter = 0;
        g_lTwoSecondSum = 0;
        g_bTwoSecondWeightAverageAvailable = TRUE;
    }
}

void GenerateEighthSecondWeightAverage( void )
{
    if(g_nEighthSecondSumCounter >= 2)      // should never be >= 2 on entry, if so re-initialise sum and entry counter.
    {
        g_nEighthSecondSumCounter = 0;
        g_lEighthSecondSum = 0;
    }
    g_lEighthSecondSum += g_lRawAtDCounts;      // Add in current raw A/D value
    g_nEighthSecondSumCounter++;
    if(g_nEighthSecondSumCounter == 2)
    {
        g_lEighthSecondWeightAverage = g_lEighthSecondSum / 2;  // Divide by 2;
        g_nEighthSecondSumCounter = 0;
        g_lEighthSecondSum = 0;
        g_bEighthSecondWeightAverageAvailable = TRUE;
    }
}

////////////////////////////////////////////////////////////////
// Tarend                               // ASM = TAREND
// This is the normal blender tare run on reset ??? Comment from assembler
// Note: contains the version that runs in normal and autocycle modes.
// Entry:   Void
//
// Exit: void   
////////////////////////////////////////////////////////////////   

void Tarend( void )
{
    if(g_bAutoCycleFlag)
    {       // in Autocycle
//      g_lRawAtDCounts = g_lHopperTareCounts;
        //testonly 
// --REVIEW--
/*
        g_lRawAtDCounts = g_lHopperTareCounts;      //g_CalibrationData.m_lTareCounts;      //AUTOCYCLE_INITIAL_TARE_VALUE;
        g_lCurrentLCAtDValue = g_lHopperTareCounts;
        g_lCurrentLCAtDTare = g_lHopperTareCounts;
*/

        g_lRawAtDCounts = g_CalibrationData.m_lTareCounts;      //AUTOCYCLE_INITIAL_TARE_VALUE;
        g_lCurrentLCAtDValue = g_CalibrationData.m_lTareCounts;
        g_lCurrentLCAtDTare = g_CalibrationData.m_lTareCounts;

    }
    else        // not autocycle.
    {       // NORMTARE     normal tare weighing
        g_lHopperTareCounts = g_lOneSecondWeightAverage;            //long int
        g_lCurrentLCAtDTare = g_lOneSecondWeightAverage;            //long int
        g_lCurrentLCAtDValue = g_lOneSecondWeightAverage;           //long int
    }
    CalculateOngoingMaxMinTareValue();
}
////////////////////////////////////////////////////////////////
// NormTare
// //  This is the normal blender tare 
// Entry:   Void
//
// Exit:    
////////////////////////////////////////////////////////////////   
void NormTare( void )
{
        g_lHopperTareCounts = g_lOneSecondWeightAverage;            //long int
        g_lCurrentLCAtDTare = g_lOneSecondWeightAverage;            //long int
        g_lCurrentLCAtDValue = g_lOneSecondWeightAverage;           //long int
}
////////////////////////////////////////////////////////////////
// TareForAutoCycler                                // ASM = none (was using NORMTARE, in autocycle mode)
// This is the normal blender tare run on reset 
// Note: 
// Entry:   Void
//
// Exit: void   
////////////////////////////////////////////////////////////////   
void TareForAutoCycler( void )
{
    if(g_bAutoCycleFlag)  // only runs in AutoCycleMode.
    {       // in Autocycle
// --REVIEW--
        g_lRawAtDCounts = g_CalibrationData.m_lTareCounts;      //AUTOCYCLE_INITIAL_TARE_VALUE;
        g_lCurrentLCAtDValue = g_CalibrationData.m_lTareCounts;
        g_lCurrentLCAtDTare = g_CalibrationData.m_lTareCounts;
    }
}

////////////////////////////////////////////////////////////////
// InitialiseWeightVariables( void )
// Initialises all the weight data, raw counts, and all average calculation values.
// Entry:   Void
//
// Exit:        none
// Michael McKiernan                06-07-2004
// First pass.
////////////////////////////////////////////////////////////////   
void InitialiseWeightVariables( void )
{
    g_bWeightAvailableFlag = FALSE;
    g_lRawAtDCounts = 0;                // 
    g_lWeightAtoDCounts = 0;

    g_lHalfSecondSum = 0;                         // 
    g_nHalfSecondSumCounter = 0;                  // 
    g_lHalfSecondWeightAverage = 0;               // 
    g_bHalfSecondWeightAverageAvailable = FALSE;      // 

    g_lOneSecondSum = 0;                          // 
    g_nOneSecondSumCounter = 0;                   // 
    g_lOneSecondWeightAverage = 0;                // 
    g_bOneSecondWeightAverageAvailable = FALSE;       // 

    g_lTwoSecondSum = 0;                          // 
    g_nTwoSecondSumCounter = 0;                   // 
    g_lTwoSecondWeightAverage = 0;                // 
    g_bTwoSecondWeightAverageAvailable = FALSE;       //

    g_lEighthSecondSum = 0;                            // 
    g_nEighthSecondSumCounter = 0;                     // 
    g_lEighthSecondWeightAverage = 0;                  // 
    g_bEighthSecondWeightAverageAvailable = FALSE;         // 

    g_nPreviousWeightAvgFactor = 0;               // 
    g_lAvgSum = 0;                                // 
    g_nAvgSumCounter = 0;                         // 
    g_lSettledAverage = 0;                        // 
    g_bWeightAvgAvail = FALSE;                        // 

    g_lHopperTareCounts = g_CalibrationData.m_lTareCounts;                  //
    g_lCurrentLCAtDTare = g_CalibrationData.m_lTareCounts;
    g_lCurrentLCAtDValue = 0;           //

   g_fWeightInHopper = 0.0;                
   g_fWeightInHopperMinusRegrind = 0.0;    
   g_fPreviousBatchWeight = 0.0;           
   g_fCurrentBatchWeight = 0.0;            

}


/*
TAREND:
        TST     AUTOCYCLEFLAG
        BEQ     NORMTARE        ; TARE AS BEFORE
;        LDX     #CALBLK
;        LDAB    #OFFSOF         ; POSITION FOR TARE READING.
;        ABX        
        LDX     #HOPPERTARECOUNTS
        JSR     BCDHEX3X         ;
        STAB    RAW132
        STE     RAW132+1
        STAB    CURRLCADH       ; "CURRENT LOAD CELL A/D" HEX 
        STE     CURRLCADH+1     ;
        LDX     #RAW132
        JSR     HEXBCD3X        ; BCD TO HEX CONVERSION
        STAB    CURRLCADTARE
        STE     CURRLCADTARE+1  ; STORE TARED VALUE 
        RTS
        
NORMTARE:
        LDD     AN1BUF
        STD     CURRLCADTARE
        STD     HOPPERTARECOUNTS
        LDAA    AN1BUF+2
        STAA    CURRLCADTARE+2  ; STORE TARED VALUE 
        STAA    HOPPERTARECOUNTS+2        
        LDX     #AN1BUF
        JSR     BCDHEX3X        ; TO HEX
        STAB    CURRLCADH       ; "CURRENT LOAD CELL A/D" HEX 
        STE     CURRLCADH+1     ;                     ;
        RTS                     ;

*/

// UNUSED FUNCTIONS
        
/*

////////////////////////////////////////////////////////////////
// WRITE STATUS COMMAND EEP #1
//
// Entry:   cData holds the EEPROM status to be written out.
//
// Exit:    Void
////////////////////////////////////////////////////////////////
void WriteEEPROMStatus( char cData )
{
    union CharsAndInt   nData;

    *(SPITransmitRAM + 0x00) = EEPROM_WRITE_STATUS_REG;     // WRITE TO STATUS REG.

    nData.cValue[1] = cData;
    *(SPITransmitRAM + 0x01) = nData.nValue;                // BYTE TO WRITE.

    *SPIControl2 = 0x0100;      // SET NEWQP TO START TRANSMISSION, NEWQP=0 ENDQP=1 (2 TRANSMISSIONS)

//  StartSPI();
//  PollSPIUntilFinished();
    TransmitSPI();

}

              
// WRITE STATUS COMMAND EEP #2 ACCB HOLDS STATUS DATA
void STATUS_WRITE2( char cStatus )
{
        LDAA    #WRSR                    ; WRITE TO STATUS REG.
        STAA    QRAMTX4+1
        STAB    QRAMTX5+1                ; BYTE TO WRITE.
        LDD     #$0504                   ;NEWQP=4 ENDQP=5 (2 TRANSMISSIONS)
        STD     SPCR2                    ;SET NEWQP TO START TRANSMISSION
//  StartSPI();
//  PollSPIUntilFinished();
    TransmitSPI();

}                

CALCAVERAGE:
        JSR     HSWTAVGEN       ; GENERATE 0.5 SECOND AVERAGE.                
        JSR     ESWTAVGEN
        JSR     OSWTAVGEN
        JSR     TSWTAVGEN       ; 2 SECOND WEIGHT AVERAGING.
        TST     FILLSTATUSALL              ; IS FILL IN PROGRESS ?
        BEQ     NOTFILLING
;       LDAA    OXXLINE
        LDAA    BLENDERMODE     ; CHECK MODE OF OPERATION.
        CMPA    #THROUGHPUTMON  ; IS THE THROUGHPUT MONITOR ENABLED
        BNE     NOTFILLING
        LDX     SEQPOINTER      ; READ SEQUENCE POINTER
        LDAA    0,X
        TAB
        ORAA    #FILLBYWEIGHT   ; SET FILL BY WEIGHT 
        STAA    0,X
NOTFILLING:
        LDAA    BATSEQ          ;
        CMPA    #SETTLING       ; SETTLING MODE.
        LBEQ     XITAVG          ; EXIT AVERAGE
        CMPA    #CALC                   ; IS CALCULATION NEXT STAGE
        LBNE     XITAVG          ; NO AVERAGING COUNTER INC
        JSR     SETTLEAVG       ;
        RTS
        

RESETAVG:
        CLRW    AVSUM132
        CLRW    AVSUM132+2              ; RESET SUMMATION COUNTER
        CLRW    SAMCOUNTER              ; SAMPLE COUNTER
        RTS
                
SETTLEAVG:
        LDAA    WGTAVGFACTOR                  ; SAMPLE SIZE
        CMPA    PREVWGTAVGFACTOR              ; SAME AS BEFORE
        BEQ     NOCHANGE                ; YES
        JSR     RESETAVG
        LDAA    WGTAVGFACTOR
        STAA    PREVWGTAVGFACTOR                
NOCHANGE:       
        LDD     AVSUM132+2                ;SUMMATION OF 132 RESULTS
        ADDD    RAW132+1
        STD     AVSUM132+2
        LDAB    AVSUM132+1
        ADCB    RAW132
        STAB    AVSUM132+1              ;ADD NEW VALUE TO SUM
        LDAB    AVSUM132
        ADCB    #00
        STAB    AVSUM132
        INCW    SAMCOUNTER              ;NO. OF ENTRIES


; SET UP SAMPLE COUNTER DETERMINED BY THE ENTRY IN THE 

        LDAA    #16
        LDAB    WGTAVGFACTOR                  ; NO OF SAMPLES.
        MUL                             ;
        STD     NOSAMPLES        
        
        LDD     SAMCOUNTER
        CPD     NOSAMPLES              ;16
        BLO     XITAVG                 ;GO TO START NEXT READ
        CLRW    SAMCOUNTER              ; COUNTER RESET TO 0

        LDAA    WGTAVGFACTOR
        STAA    TEMPBUF
       
        LDD     AVSUM132                  ; SHIFT TO DIVIDE
        LDE     AVSUM132+2
        CLRW    AVSUM132
        CLRW    AVSUM132+2
        LSRD    
        RORE    
        LSRD                            ; DIVIDE DATA BY 16
        RORE    
        LSRD    
        RORE    
        LSRD 
        RORE

; WEIGHT AVERAGING DETEMINES THE NO OF TIMES THAT THE AVERAGED
; DATA SHOULD BE SHIFTED.


        STAB    AN1AVGBUFH
        STE     AN1AVGBUFH+1
        JSR     HEXBCD3
        STD      AN1AVGBUF                  ; STORE RESULT
        STE      AN1AVGBUF+2                
        LDAA    #$0F
        STAA    AVGAVAIL                ; INDICATE AVERAGE AVAILABLE
XITAVG  RTS       



*/

/*
        LDAA    WGTAVGFACTOR                  ; SAMPLE SIZE
        CMPA    PREVWGTAVGFACTOR              ; SAME AS BEFORE
        BEQ     NOCHANGE                ; YES
        JSR     RESETAVG
        LDAA    WGTAVGFACTOR
        STAA    PREVWGTAVGFACTOR                
NOCHANGE:       
        LDD     AVSUM132+2                ;SUMMATION OF 132 RESULTS
        ADDD    RAW132+1
        STD     AVSUM132+2
        LDAB    AVSUM132+1
        ADCB    RAW132
        STAB    AVSUM132+1              ;ADD NEW VALUE TO SUM
        LDAB    AVSUM132
        ADCB    #00
        STAB    AVSUM132
        INCW    SAMCOUNTER              ;NO. OF ENTRIES


; SET UP SAMPLE COUNTER DETERMINED BY THE ENTRY IN THE 

        LDAA    #16
        LDAB    WGTAVGFACTOR                  ; NO OF SAMPLES.
        MUL                             ;
        STD     NOSAMPLES        
        
        LDD     SAMCOUNTER
        CPD     NOSAMPLES              ;16
        BLO     XITAVG                 ;GO TO START NEXT READ
        CLRW    SAMCOUNTER              ; COUNTER RESET TO 0

        LDAA    WGTAVGFACTOR
        STAA    TEMPBUF
       
        LDD     AVSUM132                  ; SHIFT TO DIVIDE
        LDE     AVSUM132+2
        CLRW    AVSUM132
        CLRW    AVSUM132+2
        LSRD    
        RORE    
        LSRD                            ; DIVIDE DATA BY 16
        RORE    
        LSRD    
        RORE    
        LSRD 
        RORE

; WEIGHT AVERAGING DETEMINES THE NO OF TIMES THAT THE AVERAGED
; DATA SHOULD BE SHIFTED.


        STAB    AN1AVGBUFH
        STE     AN1AVGBUFH+1
        JSR     HEXBCD3
        STD      AN1AVGBUF                  ; STORE RESULT
        STE      AN1AVGBUF+2                
        LDAA    #$0F
        STAA    AVGAVAIL                ; INDICATE AVERAGE AVAILABLE
XITAVG  RTS       

mce12-15

EETIME EQU    $FF      ;~EEPROM WRITE DELAY
EEBUSY EQU    $20      ;~EEPROM BUSY CODE
EEWFLT EQU    $07      ;~EEPROM WRITE FAULT
EETFLT EQU    $70      ;~EEPROM TIMER FAULT
;
;*  WRITEE IS A SUBR. TO WRITE DATA IN ACC A TO EEPROM AT ADDR. IN X
;
;
;*WRITAEE  WRITES BYTE IN ACC A TO EEPROM #1  0,X
WRITAEE1 PSHM    X
        PSHA
LOOPAC1 TST     SPIATDACTIVE    ; ATD READ IN OPERATION. ?
        BNE     LOOPAC1         ; YES 
        LDE     #20    ;(20 * 5mS)
WRTAEE2 JSR     BYTE_READ1       ; READ BACK EEPROM DATA.
        TAB                     ; COPY EEPROM DATA
        PULA                    ; ORIGIIONAL.
        PSHA
        CBA                     ; VERIFY PROGRAMMING COMPLETE.   
        BEQ     WRTAEEX

        JSR     BYTE_WRITE1
        JSR     DLY05MS ;5mS SOFTWARE DELAY
        JSR     BYTE_READ1       ; READ BACK EEPROM DATA.
        TAB                     ; COPY EEPROM DATA
        PULA                    ; ORIGIIONAL.
        PSHA
        CBA                     ; VERIFY PROGRAMMING COMPLETE.   
        BEQ     WRTAEEX
        ADDE    #-1
        BNE     WRTAEE2
WRTEEFT LDAA    #EEWFLT
        STAA    EEPFAU   ;~SET EEPROM WRITE FAULT
WRTAEEX:
        CLR     SPIEEPACTIVE    ; ALLOW A/D TASK TO RUN.
        PULA
        PULM    X
        RTS

;
;*WRITAEE  WRITES BYTE IN ACC A TO EEPROM #2
WRITAEE2 PSHM    X
        PSHA
LOOPAC2 TST     SPIATDACTIVE    ; ATD READ IN OPERATION. ?
        BNE     LOOPAC2         ; YES 
        LDAB    #1
        STAB    SPIEEPACTIVE    ; INDICATE SERIAL EEPROM WRITE OPERATION.
        LDE     #20    ;(20 * 5mS)
WRTAE22 JSR     BYTE_READ2       ; READ BACK EEPROM DATA.
        TAB                     ; COPY EEPROM DATA
        PULA                    ; ORIGIIONAL.
        PSHA
        CBA                     ; VERIFY PROGRAMMING COMPLETE.   
        BEQ     WRTAE2X        
        JSR     BYTE_WRITE2
        JSR     DLY05MS ;5mS SOFTWARE DELAY
        JSR     BYTE_READ2       ; READ BACK EEPROM DATA.
        TAB                     ; COPY EEPROM DATA
        PULA                    ; ORIGIIONAL.
        PSHA
        CBA                     ; VERIFY PROGRAMMING COMPLETE.   
        BEQ     WRTAE2X
        ADDE    #-1
        BNE     WRTAE22
        LDAA    #EEWFLT
        STAA    EEPFAU   ;~SET EEPROM WRITE FAULT
WRTAE2X:
        CLR     SPIEEPACTIVE    ; ALLOW A/D TASK TO RUN.
        PULA
        PULM    X
        RTS

*/

