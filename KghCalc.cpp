//////////////////////////////////////////////////////
// KghCalc.c
//
// Functions for calculating the kg/h etc.
//
// M.McKiernan                              05-01-2005
// First Pass.
//
//  P.Smith                                 2/3/5
// if start up reset kg/h round robin.
//
// P.Smith                                  30/08/2005
// Correct copy of g_fRRThroughputKgPerHour to modbus table.
//
// P.Smith                            12/10/05  
// ensure that round robin is reset in manual & auto startup
//
// 
// P.Smith                      23/2/06
// first pass at netburner hardware conversion.
// included  <basictypes.h>
// 
// P.Smith                      24/2/06
// changed unions IntsAndLong to WordAndDWord
//
// P.Smith                            25/5/06 
// multiply modbus data by conversion factor.                        
//
// M.McKiernan                      17/9/07 
// Removed g_arrnMBTable[] & g_arrnMBTableFlags[], now in BatVars2
//
// P.Smith                      19/11/07 
// Remove ResetKgHRoundRobin in manual
//                                                                           ;
// P.Smith                      5/12/07             
// No kg/hr calculation if control is suspended.                
//
//
//        
// P.Smith                          30/1/08
// correct compiler warnings
//
// P.Smith                          12/6/08
// Do not enter CalculateKgPerHour unless g_nDisableDTAPerKgCtr is 0
//
// P.Smith                          12/6/08
// in CopyKghToMB only copy back round robin averaged value, do not
// use instantaneuous value.
//
// P.Smith                          23/7/08
// remove g_arrnMBTable
//
// P.Smith                          10/9/08
// if liquid additive enabled and liquid additive % is non zero, then add in 
// g_fKgperHour kg/hr set point for liquid additive

//////////////////////////////////////////////////////

//*****************************************************************************
// INCLUDES
//*****************************************************************************
#include "General.h"
#include "BatVars.h"
#include "KghCalc.h"
#include "ConversionFactors.h"
#include "BatchCalibrationFunctions.h"
#include "SetpointFormat.h"
#include "BatchMBIndices.h"
#include "BatVars2.h"
#include <basictypes.h>


extern CalDataStruct    g_CalibrationData;
extern structSetpointData   g_CurrentRecipe;


//////////////////////////////////////////////////////
// BatchSecondsCalulation( void )           from ASM = BATSECAL
// Routine to calculate the length of a batch cycle in seconds.
//
// Integer part of result comes from g_nKgHrSecondsCounter, i.e. counter incremented by RTC.
// fraction part comes from PIT - PIT_FREQUENCY - Previous PIT Time + g_nPITFractionCounter)
// g_nPITFractionCounter inc'ed on PIT and cleared by RTC.
// Previous PIT Time is the fraction counter stored when prev. batch occurred.
//
// Exit: Batch time in g_fBatchTimeInSeconds
// 
//
// M.McKiernan                          05-01-2005
// First pass.
//
// P.Smith                          2-3-2005
// Read the Round Robin No from the control average.
//
//////////////////////////////////////////////////////
void BatchSecondsCalculation( void )
{
int nPITs;
float fBatchTimeFraction;

    // time fraction = (PIT_FREQUENCY - Previous PIT Time + g_nPITFractionCounter) / PIT Frequency. 
    nPITs = (PIT_FREQUENCY - g_nPreviousPITTime) + g_nPITFractionCounter;
    fBatchTimeFraction = (float)nPITs / PIT_FREQUENCY;

    g_nPreviousPITTime = g_nPITFractionCounter; // PREVPITM
    g_nPITFractionCounter = 0;                          // PITFRACTCTR

    g_fBatchTimeInSeconds = (float)g_nKgHrSecondsCounter + fBatchTimeFraction;  // BATCHSECS
    g_nKgHrSecondsCounter = -1;     // initialise to -1, so will roll over to 0 on next RTC interrupt.
}



//////////////////////////////////////////////////////
// CalculateKgPerHour( void )           from ASM = CALKGPHR
// Routine to calculate the throughput based on batch wt. and batch time.
//  kg/h = (BATCH WEIGHT  * 3600)/ BATCH TIME (SECONDS)
//
// Exit: 
// 
//
// M.McKiernan                          05-01-2005
// First pass.
//////////////////////////////////////////////////////
void CalculateKgPerHour( void )
{

    if((g_nSuspendControlCounter == 0) && (g_nDisableDTAPerKgCtr == 0))
    {
        if(g_fBatchTimeInSeconds > 0.1)     // dont divide by  0
        g_fBatchKgPerHour = (g_fPreviousBatchWeight * 3600) / g_fBatchTimeInSeconds;        // BATCHKGPHR
        else
        g_fBatchKgPerHour = 0;

        if((g_CalibrationData.m_bLiquidEnabled) && (g_CurrentRecipe.m_fPercentage[g_nLiquidAdditiveComponent-1] > 0.01))
        {
            g_fBatchKgPerHour = g_fBatchKgPerHour +  g_fKgperHour;             

        }
        
        g_fThroughputKgPerHour = g_fBatchKgPerHour; // TPTKGH
        // convert to english units(Lb/h).  - JSR CONVKGHRI
        g_fThroughputLbPerHour = g_fBatchKgPerHour * THROUGHPUT_CONVERSION_FACTOR;  // TPTLBH
        if(g_bManualAutoFlag)  // round robin only reset if Auto.
        {    
            if(g_bExtruderControlStartUpFlag)       // if start up check if inside the alarm band.
            {
                ResetKgHRoundRobin();
            }
        }
        else
        {   // manual
//        ResetKgHRoundRobin();  //nbb--todo-- should this be removed.
        }

        if(g_CalibrationData.m_nControlAvgFactor != g_nControlAvgFactorKghHistory)
        {
            ResetKgHRoundRobin();
            g_nControlAvgFactorKghHistory = g_CalibrationData.m_nControlAvgFactor;  
        }


        CalculateKgHRoundRobin();       // GENRRKGH ( generate r.robin kg/h )
        CopyKghToMB();                 // CPYKGHMB
    }
}


//////////////////////////////////////////////////////
// CalculateKgHRoundRobin( void )           from ASM = GENRRKGH (loosely based)
// Routine to calculate the round robin kg/h value.
//  
//
// Exit: 
// 
//
// M.McKiernan                          05-01-2005
// First pass.
//////////////////////////////////////////////////////
void CalculateKgHRoundRobin( void )
{
    unsigned int i;
   float fSum = 0.0f;
    float   fAvg = 0.0f;
        // Check initially that there avg. factor is not greater than the R.R. buffer size.
        if( g_CalibrationData.m_nBatchesInKghAvg > MAX_KGH_RR_SAMPLES)
            g_CalibrationData.m_nBatchesInKghAvg = MAX_KGH_RR_SAMPLES;  

        if(!g_bFirstBatch)          // FIRSTBATCH - only do calculation if not first batch.
        {
            // move batch kg/h value into round robin
            g_fKgHRRBuffer[g_nKgHRRIndex] = g_fBatchKgPerHour;

            // increment index or pointer.
            g_nKgHRRIndex++;
            if(g_nKgHRRIndex >= g_CalibrationData.m_nControlAvgFactor)
                g_nKgHRRIndex = 0;

            // increment summation counter up to max rr size.
            if(g_nKgHRRSummationCounter < g_CalibrationData.m_nControlAvgFactor)
                g_nKgHRRSummationCounter++;     

            // assumes summation counter has no. of elements in RR.
            for(i = 0; i < g_nKgHRRSummationCounter; i++)
            {
                fSum += g_fKgHRRBuffer[i]; // add each sample to sum.
            }

            if(g_nKgHRRSummationCounter > 0)
                fAvg = fSum / g_nKgHRRSummationCounter;     // R.R. avg.

            g_fRRThroughputKgPerHour = fAvg;    //RRTPTKGH

            // convert to English units.
            g_fRRThroughputLbPerHour = fAvg * THROUGHPUT_CONVERSION_FACTOR; //RRTPTLBH
        }
}

//////////////////////////////////////////////////////
// ResetKgHRoundRobin( void )           from ASM = RESRROB (loosely based)
// Routine to reset the round robin for kg/h.
//  
//
// Exit: 
// 
//
// M.McKiernan                          05-01-2005
// First pass.
//////////////////////////////////////////////////////
void ResetKgHRoundRobin( void )
{
int i;

    g_nKgHRRIndex = 0;
    g_nKgHRRSummationCounter = 0;
        // clear each element in RR.
        for(i = 0; i < MAX_KGH_RR_SAMPLES; i++)
        {
            g_fKgHRRBuffer[i] = 0.0; // clear each sample
        }

}


//////////////////////////////////////////////////////
// CopyKghToMB( void )          from ASM = CPYKGHMB
// Copies the kg/h value into MB table.
//  
//
// Exit: 
// 
//
// M.McKiernan                          06-01-2005
// First pass.
//////////////////////////////////////////////////////
void CopyKghToMB( void )
{
    float fValue;
    union WordAndDWord uValue;

        fValue = g_fRRThroughputKgPerHour;          // RRTPTKGH 
        fValue *= g_fThroughputConversionFactor;            
        
// kg/h  
    fValue += 0.005;        // for rounding to nearest 2nd decimal digit.
    uValue.lValue = (long)(fValue * 100.0f);
    g_arrnMBTable[BATCH_SUMMARY_TOTAL_THROUGHPUT] = uValue.nValue[0];       // M.s.Byte.
    g_arrnMBTable[BATCH_SUMMARY_TOTAL_THROUGHPUT + 1] = uValue.nValue[1];       // L.s.Byte.

/* note copy of totwaccbat to mbtotwaccbat c equivalent moved to mbprogs.c program. */    
    
}
/*
;* COPY Kg/h VALUES INTO MODBUS TABLE PROD. SUMMMARY DATA AREA
CPYKGHMB:
        TST     PROTOCOL
        LBPL     CPYKGHMX

        LDAA    BLENDERMODE      ; CHECK OPERATION MODE.
        CMPA    #OFFLINEMODE     ; OFFLINE MODE 
        BEQ     NOTKGH
        TST     BLENDERMODE           ; MODE SELECTION
        BNE     DIFFMODE
ISOFF   LDX     #RRTPTKGH             
        BRA     UPMOD
DIFFMODE:                             
        LDAA     BLENDERMODE     ; READ BLENDER MODE.
        CMPA     #MAXTHROUGHPUTZT ; 0 -10VOLTS OPTION.
        BEQ     NOTKGH
        LDAA    BLENDERMODE
        CMPA    #KGHCONTROLMODE  ; KG/HR MODE
        BNE     NOTKGH           ; SOME OTHER ENTRY
// kg/h mode
        TSTW    TKGHSP           ; IS KG/HR SET = 0 ?
        BNE     RROBIN          
        TST     TKGHSP+2        ; LSB
        BNE     RROBIN
        BRA     NOTKGH          ; DISPLAY ACTUAL KG/HR VALUE.
RROBIN  LDX     #RRKGPH
        BRA     UPMOD
NOTKGH  LDX     #TPTKGH
UPMOD   JSR     BCDHEX3X        ; CONVERT 3 BYTES BCD @ X TO HEX        
        CLRA                    ; RESULT IS IN [B:E]
        STD     MBKGH
        STE     MBKGH+2           ;
        
        

CPYKGH20 TST   AVECPIB        ; LIQ. ADDITIVE?
        BEQ     CPYKGHMX
        LDX    #LAKGH
        JSR    BCDHEX3X
        CLRA
        STD    MBLKGH         ; LIQ. Kg/h VALUE
        STE    MBLKGH+2       ;
CPYKGHMX:

        LDX     TOTWACCBAT
        STX     EREG
        LDX     TOTWACCBAT+2
        STX     EREG+2
        LDAA    TOTWACCBAT+4
        STAA    EREG+4
        
        LDX     #EREG     ;
        LDAB    #5
        LDAA    #1
        JSR     GPSFTR            ;~RESULT 
        LDX     #EREG+1
        JSR     BCDHEX3X
        STE     MBTOTWACCBAT

        RTS

*/
/*
RESRROB:
       LDAA    BATINKGHAVG
       STAA     RRAVRGK2
       CLR    RRSUMCT2   ;~RESET QUE #2 SAMPLE COUNTER
       CLR    RRCDENP2   ;~RESET QUE #2 DATA POINTER
       LDX    #RRDATST2  ;~LOAD QUE #2 DATA S.A.
       STX    RRQCADR2   ;~STORE IN QUE #2 ADDRESS POINTER
       LDX    #RRSUMTT2  ;~LOAD S.A. OF SUMMATION TOTAL
       JSR    CLRNM2   ;~RESET SUMMATION TOTAL
       RTS

*/


/*
;
;       KG/HR CALCULATION.
;
;       KG/HR = (BATCH WEIGHT (1/10TH GRAMS) * 3600)/BATCH TIME (SECONDS)
;             = XXXXX.Y KG/HR
;
CALKGPHR:
        JSR     CLRAC            
        LDX     #PREVBATCHWEIGHT; LOAD CURRENT BATCH
        JSR     AMOVE           ;
        LDX     #$3600          ;
        STX     MATHCREG+3      ; NO OF SECONDS IN THE HOUR
        JSR     MUL             ; CALCULATE KG/HR
        LDX     #MATHCREG       ;
        JSR     IFEREG
        LDX     #BATCHSECS      ; SECONDS THAT BATCH WAS THEREFORE XXXX.YY
        JSR     AMOVE           ;
        JSR     DIV             ; C/A DIVIDE BY NO OF SECONDS TO CALCULATE KG/HR
        LDX     #BATCHKGPHR  
        JSR     IFEREG          ; STORE KG/HR 
        LDD     EREG+2          ;
        STD     TPTKGH          ; STORE KG/HR TO DISPLAY       
        LDAA    EREG+4          ;
        STAA    TPTKGH+2        ; STORE LSB
        JSR     CONVKGHRI       ; KG/HR TO IMPERIAL UNITS
        JSR     GENRRKGH
        JSR     CPYKGHMB
        RTS


*/
/*
;       CALCULATE NUMBER OF SECONDS FOR THE BATCH CYCLE
;
;((PITINDEX - PREVPIT TIME + PITFRACTCTR)->BCD / (PITINDEX) + BATCH TIME (SECONDS)
;       



BATSECAL:
        LDD     #PITINDEX       ; PIT SPEED.
        SUBD    PREVPITM        ; PIT SPEED - PREVIOUS PIT VALUE
        ADDD    PITFRACTCTR        ; ADD NEW PIT READING FROM PREVIOUS SECOND
        JSR     HEXBCD2         ;
        JSR     CLRAC           
        STE     MATHCREG+1          ;
        LDD     #PITINDEXBCD
        STD     AREG+3          ;
        JSR     DIV             ; C/A FRACTION OF SECOND IS CACULATED XX.YY

        LDAA   #2
        LDX    #EREG
        JSR    GPRND1            ;~ROUND TO 3 DIGITS

        JSR     CLRAC
      
        LDX     EREG+2          ; READ .YY SECONDS
        STX     AREG+3
        LDD     PITFRACTCTR     ;
        STD     PREVPITM        ;
        CLRW    PITFRACTCTR
        LDX     #-1
        LDD     KGHRCOUNT       ; READ SECONDS
        STX     KGHRCOUNT       ;
        JSR     HEXBCD2         
        STE     MATHCREG+2      ; SECONDS TRANSFERRED FOR ADDITION.
        JSR     ADD
        LDX     #BATCHSECS      ; NO OF SECONDS
        JSR     EMOVE           
        RTS                
                
  */      
