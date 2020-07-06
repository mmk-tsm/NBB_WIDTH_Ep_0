
// **********************************************************
//   MBCyDiag.c                                                 *
//    Modbus - copying cycle (batch) diagnostics into table *
//   
//                                                          *
// M.McKiernan                      07-12-2004                  *
//  First Pass
//
//   Latest Changes:                07.12.2004                  *
// P.Smith                          18/10/05
// make g_arrnMBTable unsigned.
//
// 
// P.Smith                      28/2/06
// first pass at netburner hardware conversion.
// Bool -> BOOL
//#include <basictypes.h>
//
// P.Smith                            25/5/06 
// multiply modbus data by conversion factor.                        
/// **********************************************************
//
// P.Smith                              9/8/06
// rename union IntsAndLong uValue to union WordAndDWord uValue
//
// M.McKiernan                      17/9/07 
// Removed g_arrnMBTable[] & g_arrnMBTableFlags[], now in BatVars2
//
// P.Smith                              7/12/07
// copy in the correct component weight depending on g_bShowInstantaneousWeight flag.
//
// P.Smith                          23/7/08
// remove g_arrnMBTable & g_CurrentTime externs

//*****************************************************************************
// INCLUDES
//*****************************************************************************
#include <stdlib.h>
#include <stdio.h>

#include "SerialStuff.h"
//nbb #include "Hardware.h"
#include "BatchMBIndices.h"
#include "BatchCalibrationFunctions.h"
#include "General.h"
#include "ConversionFactors.h"

#include "16R40C.h"
#include "Foreground.h"
#include "ConfigurationFunctions.h"
#include "MBMHand.h"
#include "MBSHand.h"
#include "MBProgs.h"

#include "MBProgs.h"
#include "SetpointFormat.h"
#include "TimeDate.h"
#include "BatVars.h"
#include "BBCalc.h"
#include "CycleMonitor.h"
#include "MBTogSta.h"
#include "PITWeigh.h"
#include "InitialiseHardware.h"
#include "MBCyDiag.h"
#include "BatVars2.h"



extern  CalDataStruct   g_CalibrationData;
extern  structSetpointData  g_CurrentRecipe;
extern  structSetpointData  g_TempRecipe;





//////////////////////////////////////////////////////
// CopyCycleDiagnosticsToMB  asm = COPYMBCYCLEDIAGNOSTICS
//
// copy calibration data to MB.
//
// M.McKiernan                          06-12-2004
// First Pass   
// 
//////////////////////////////////////////////////////
void CopyCycleDiagnosticsToMB( void )
{
    union WordAndDWord uValue;
    unsigned int i;
    double ffValue;

// component set weights
    for(i = 0; i < g_CalibrationData.m_nComponents; i++ )
    {
        ffValue = g_fComponentTargetWeight[i] * 10000.0f;       // component wt. in kg, convert to 10ths of g
        ffValue *=    g_fWeightConversionFactor;

        uValue.lValue = (long)ffValue;                          // convert to integer (long)
        g_arrnMBTable[BATCH_CYCLE_TARGET_WEIGHT_1 + i*2] = uValue.nValue[0];        // M.s.word
        g_arrnMBTable[BATCH_CYCLE_TARGET_WEIGHT_1 + (i*2) + 1] = uValue.nValue[1];      // L.s.word
    }
    // Zero out unused components.
    if( g_CalibrationData.m_nComponents < MAX_COMPONENTS )
    {
        for(i = g_CalibrationData.m_nComponents; i < MAX_COMPONENTS ; i++ )
        {
            g_arrnMBTable[BATCH_CYCLE_TARGET_WEIGHT_1 + i*2] = 0;       // M.s.word
            g_arrnMBTable[BATCH_CYCLE_TARGET_WEIGHT_1 + (i*2) + 1] = 0;     // L.s.word
        }
    }

// component actual weights
    for(i = 0; i < g_CalibrationData.m_nComponents; i++ )
    {
        if(g_bShowInstantaneousWeight)
        {
            ffValue = g_fComponentActualWeight[i] * 10000.0f;       // component wt. in kg, convert to 10ths of g
        }
        else
        {
            ffValue = g_fComponentActualWeightRR[i] * 10000.0f;       // component wt. in kg, convert to 10ths of g
        }

        ffValue *=    g_fWeightConversionFactor;
        uValue.lValue = (long)ffValue;                              // convert to integer (long)
        g_arrnMBTable[BATCH_CYCLE_ACTUAL_WEIGHT_1 + (i*2)] = uValue.nValue[0];              // M.s.word
        g_arrnMBTable[BATCH_CYCLE_ACTUAL_WEIGHT_1 + (i*2) + 1] = uValue.nValue[1];      // L.s.word
    }
    // Zero out unused components.
    if( g_CalibrationData.m_nComponents < MAX_COMPONENTS )
    {
        for(i = g_CalibrationData.m_nComponents; i < MAX_COMPONENTS ; i++ )
        {
            g_arrnMBTable[BATCH_CYCLE_ACTUAL_WEIGHT_1 + (i*2)] = 0;     // M.s.word
            g_arrnMBTable[BATCH_CYCLE_ACTUAL_WEIGHT_1 + (i*2) + 1] = 0;     // L.s.word
        }
    }

// component set counts
    for(i = 0; i < g_CalibrationData.m_nComponents; i++ )
    {
        uValue.lValue = g_lComponentTargetCounts[i];            // component target counts (long)
        g_arrnMBTable[BATCH_CYCLE_TARGET_COUNTS_1 + i*2] = uValue.nValue[0];            // M.s.word
        g_arrnMBTable[BATCH_CYCLE_TARGET_COUNTS_1 + (i*2) + 1] = uValue.nValue[1];      // L.s.word
    }
    // Zero out unused components.
    if( g_CalibrationData.m_nComponents < MAX_COMPONENTS )
    {
        for(i = g_CalibrationData.m_nComponents; i < MAX_COMPONENTS ; i++ )
        {
            g_arrnMBTable[BATCH_CYCLE_TARGET_COUNTS_1 + i*2] = 0;               // M.s.word
            g_arrnMBTable[BATCH_CYCLE_TARGET_COUNTS_1 + (i*2) + 1] = 0;     // L.s.word
        }
    }

// component actual counts
    for(i = 0; i < g_CalibrationData.m_nComponents; i++ )
    {
        uValue.lValue = g_lComponentCountsActual[i];            // component actual counts (long)
        g_arrnMBTable[BATCH_CYCLE_ACTUAL_COUNTS_1 + i*2] = uValue.nValue[0];            // M.s.word
        g_arrnMBTable[BATCH_CYCLE_ACTUAL_COUNTS_1 + (i*2) + 1] = uValue.nValue[1];      // L.s.word
    }
    // Zero out unused components.
    if( g_CalibrationData.m_nComponents < MAX_COMPONENTS )
    {
        for(i = g_CalibrationData.m_nComponents; i < MAX_COMPONENTS ; i++ )
        {
            g_arrnMBTable[BATCH_CYCLE_ACTUAL_COUNTS_1 + i*2] = 0;               // M.s.word
            g_arrnMBTable[BATCH_CYCLE_ACTUAL_COUNTS_1 + (i*2) + 1] = 0;     // L.s.word
        }
    }

// component ON times in PIT's
    for(i = 0; i < g_CalibrationData.m_nComponents; i++ )
    {
        uValue.lValue = g_lCmpTime[i];          // component on time (PIT periods)
        g_arrnMBTable[BATCH_CYCLE_ON_TIME_PITS_1 + i*2] = uValue.nValue[0];         // M.s.word
        g_arrnMBTable[BATCH_CYCLE_ON_TIME_PITS_1 + (i*2) + 1] = uValue.nValue[1];       // L.s.word
    }
    // Zero out unused components.
    if( g_CalibrationData.m_nComponents < MAX_COMPONENTS )
    {
        for(i = g_CalibrationData.m_nComponents; i < MAX_COMPONENTS ; i++ )
        {
            g_arrnMBTable[BATCH_CYCLE_ON_TIME_PITS_1 + i*2] = 0;                // M.s.word
            g_arrnMBTable[BATCH_CYCLE_ON_TIME_PITS_1 + (i*2) + 1] = 0;      // L.s.word
        }
    }
// component ON times in seconds
    for(i = 0; i < g_CalibrationData.m_nComponents; i++ )
    {
        ffValue = g_fComponentOpenSeconds[i] * 10000.0f;        // component open time in tenths of mS
        uValue.lValue = (long)ffValue;                              // convert to integer (long)
        g_arrnMBTable[BATCH_CYCLE_ON_TIME_SECS_1 + (i*2)] = uValue.nValue[0];               // M.s.word
        g_arrnMBTable[BATCH_CYCLE_ON_TIME_SECS_1 + (i*2) + 1] = uValue.nValue[1];       // L.s.word
    }
    // Zero out unused components.
    if( g_CalibrationData.m_nComponents < MAX_COMPONENTS )
    {
        for(i = g_CalibrationData.m_nComponents; i < MAX_COMPONENTS ; i++ )
        {
            g_arrnMBTable[BATCH_CYCLE_ON_TIME_SECS_1 + i*2] = 0;                // M.s.word
            g_arrnMBTable[BATCH_CYCLE_ON_TIME_SECS_1 + (i*2) + 1] = 0;      // L.s.word
        }
    }

// Flow rates for each stage
    // stage 1
    for(i = 0; i < g_CalibrationData.m_nComponents; i++ )
    {
        ffValue = g_ffComponentCPI[i] * 10000.0f;       // Counts per (PIT) interrupt - to 4 dp 
        uValue.lValue = (long)ffValue;                              // convert to integer (long)
        g_arrnMBTable[BATCH_CPI_1 + (i*2)] = uValue.nValue[0];              // M.s.word
        g_arrnMBTable[BATCH_CPI_1 + (i*2) + 1] = uValue.nValue[1];          // L.s.word
    }
    // Zero out unused components.
    if( g_CalibrationData.m_nComponents < MAX_COMPONENTS )
    {
        for(i = g_CalibrationData.m_nComponents; i < MAX_COMPONENTS ; i++ )
        {
            g_arrnMBTable[BATCH_CPI_1 + (i*2)] = 0;             // M.s.word
            g_arrnMBTable[BATCH_CPI_1 + (i*2) + 1] = 0;         // L.s.word
        }
    }

    // stage 2
    for(i = 0; i < g_CalibrationData.m_nComponents; i++ )
    {
        ffValue = g_ffComponentCPIStage2[i] * 10000.0f;     // Counts per (PIT) interrupt - to 4 dp 
        uValue.lValue = (long)ffValue;                              // convert to integer (long)
        g_arrnMBTable[BATCH_CPI_STAGE2_1 + (i*2)] = uValue.nValue[0];               // M.s.word
        g_arrnMBTable[BATCH_CPI_STAGE2_1 + (i*2) + 1] = uValue.nValue[1];           // L.s.word
    }
    // Zero out unused components.
    if( g_CalibrationData.m_nComponents < MAX_COMPONENTS )
    {
        for(i = g_CalibrationData.m_nComponents; i < MAX_COMPONENTS ; i++ )
        {
            g_arrnMBTable[BATCH_CPI_STAGE2_1 + (i*2)] = 0;              // M.s.word
            g_arrnMBTable[BATCH_CPI_STAGE2_1 + (i*2) + 1] = 0;          // L.s.word
        }
    }

    // stage 3
    for(i = 0; i < g_CalibrationData.m_nComponents; i++ )
    {
        ffValue = g_ffComponentCPIStage3[i] * 10000.0f;     // Counts per (PIT) interrupt - to 4 dp 
        uValue.lValue = (long)ffValue;                              // convert to integer (long)
        g_arrnMBTable[BATCH_CPI_STAGE3_1 + (i*2)] = uValue.nValue[0];               // M.s.word
        g_arrnMBTable[BATCH_CPI_STAGE3_1 + (i*2) + 1] = uValue.nValue[1];           // L.s.word
    }
    // Zero out unused components.
    if( g_CalibrationData.m_nComponents < MAX_COMPONENTS )
    {
        for(i = g_CalibrationData.m_nComponents; i < MAX_COMPONENTS ; i++ )
        {
            g_arrnMBTable[BATCH_CPI_STAGE3_1 + (i*2)] = 0;              // M.s.word
            g_arrnMBTable[BATCH_CPI_STAGE3_1 + (i*2) + 1] = 0;          // L.s.word
        }
    }

// component actual long term weights

       ffValue = g_fTotalLongTermWeightResettableAccumulator * 10000.0f;       // component wt. in kg, convert to 10ths of g
       ffValue *=    g_fWeightConversionFactor;

        uValue.lValue = (long)ffValue;                          // convert to integer (long)
        g_arrnMBTable[BATCH_CYCLE_LONG_TERM_RESETTABLE_WEIGHT] = uValue.nValue[0];        // M.s.word
        g_arrnMBTable[BATCH_CYCLE_LONG_TERM_RESETTABLE_WEIGHT + 1] = uValue.nValue[1];      // L.s.word
         
    for(i = 0; i < g_CalibrationData.m_nComponents; i++ )
    {
        ffValue = g_fComponentLongTermWeightAccumulator[i] * 10000.0f;       // component wt. in kg, convert to 10ths of g
        ffValue *=    g_fWeightConversionFactor;

        uValue.lValue = (long)ffValue;                          // convert to integer (long)
        g_arrnMBTable[BATCH_CYCLE_LONG_TERM_WEIGHT_1 + i*2] = uValue.nValue[0];        // M.s.word
        g_arrnMBTable[BATCH_CYCLE_LONG_TERM_WEIGHT_1 + (i*2) + 1] = uValue.nValue[1];      // L.s.word
    }

// component actual long term weights
       ffValue = g_fTotalLongTermWeightAccumulator * 10000.0f;       // 
       ffValue *=    g_fWeightConversionFactor;

        uValue.lValue = (long)ffValue;                          // 
        g_arrnMBTable[BATCH_CYCLE_LONG_TERM_WEIGHT] = uValue.nValue[0];        // M.s.word
        g_arrnMBTable[BATCH_CYCLE_LONG_TERM_WEIGHT + 1] = uValue.nValue[1];      // L.s.word

    for(i = 0; i < g_CalibrationData.m_nComponents; i++ )
    {
        ffValue = g_fComponentLongTermWeightResettableAccumulator[i] * 10000.0f;       // component wt. in kg, convert to 10ths of g
        ffValue *=    g_fWeightConversionFactor;

        uValue.lValue = (long)ffValue;                          // convert to integer (long)
        g_arrnMBTable[BATCH_CYCLE_LONG_TERM_RESETTABLE_WEIGHT_1 + i*2] = uValue.nValue[0];        // M.s.word
        g_arrnMBTable[BATCH_CYCLE_LONG_TERM_RESETTABLE_WEIGHT_1 + (i*2) + 1] = uValue.nValue[1];      // L.s.word
    }
}

/*
COPYMBCYCLEDIAGNOSTICS:
        LDAB    #1                       ; COUNTER TO READ DATA
REPTR   PSHB
        LDX     #CMP1TARWGT     ; ADDRESS OF COMP #N WEIGHT  g_fComponentTargetWeight[MAX_COMPONENTS];
        PULB
        PSHB
        DECB
        ABX
        ABX
        ABX
        LDAB    #RAMBANK
        TBZK
        LDZ     #MBPCMP1TARWGT      BATCH_CYCLE_TARGET_WEIGHT_1
        PULB    
        PSHB
        DECB
        ASLB
        ASLB
        ABZ        
        JSR     BCDHEX3X        ; CONVERT 3 BYTES @X
        CLRA
        STD     0,Z
        STE     2,Z

; ACTUAL WEIGHTS

        LDX     #CMP1ACTWGT     ; g_fComponentActualWeight[MAX_COMPONENTS]
        PULB
        PSHB
        DECB
        ABX
        ABX
        ABX
        LDAB    #RAMBANK
        TBZK
        LDZ     #MBPCMP1ACTWGT  BATCH_CYCLE_TARGET_WEIGHT_1
        PULB    
        PSHB
        DECB
        ASLB
        ASLB
        ABZ        
        JSR     BCDHEX3X        ; CONVERT 3 BYTES @X
        CLRA
        STD     0,Z
        STE     2,Z



; TARGET COUNTS

        LDX     #CMP1CNTG     ; g_lComponentTargetCounts[
        PULB
        PSHB
        DECB
        ABX
        ABX
        ABX
        LDAB    #RAMBANK
        TBZK
        LDZ     #MBPCMP1CNTG
        PULB    
        PSHB
        DECB
        ASLB
        ASLB
        ABZ        
        JSR     BCDHEX3X        ; CONVERT 3 BYTES @X
        CLRA
        STD     0,Z
        STE     2,Z


        LDX     #CMP1CNAC     ; g_lComponentCountsActual[MAX_COMPONENTS];
        PULB
        PSHB
        DECB
        ABX
        ABX
        ABX
        LDAB    #RAMBANK
        TBZK
        LDZ     #MBPCMP1CNAC
        PULB    
        PSHB
        DECB
        ASLB
        ASLB
        ABZ        
        JSR     BCDHEX3X        ; CONVERT 3 BYTES @X
        CLRA
        STD     0,Z
        STE     2,Z

        LDX     #CMP1TIMS     ; g_lCmpTime[MAX_COMPONENTS];             // required time per component.
        PULB
        PSHB
        DECB
        ABX
        ABX
        ABX
        LDAB    #RAMBANK
        TBZK
        LDZ     #MBPCMP1TIMS
        PULB    
        PSHB
        DECB
        ASLB
        ASLB
        ABZ        

        CLRA
        LDAB    0,X
        LDE     1,X
        STD     0,Z
        STE     2,Z
g_ffComponentCPI[MAX_COMPONENTS]; // CMP1CPI counts per interrupt (PIT period)
c:\Hiw_Src_BB\BatchBlender_C\BatVars.c(276):double                  g_ffComponentCPIStage2[MAX_COMPONENTS]; //CMP1CPIST2
c:\Hiw_Src_BB\BatchBlender_C\BatVars.c(277):double                  g_ffComponentCPIStage3[MAX_COMPONENTS]; //CMP1CPIST3

        LDX     #CMP1CPI     ; 
        PULB
        PSHB
        DECB
        ABX
        ABX
        ABX
        LDAB    #RAMBANK
        TBZK
        LDZ     #MBPCMP1CPI
        PULB    
        PSHB
        DECB
        ASLB
        ASLB
        ABZ        
        JSR     BCDHEX3X        ; CONVERT 3 BYTES @X
        CLRA
        STD     0,Z
        STE     2,Z

        LDX     #CMP1CPIST2     ; 
        PULB
        PSHB
        DECB
        ABX
        ABX
        ABX
        LDAB    #RAMBANK
        TBZK
        LDZ     #MBPCMP1CPIST2
        PULB    
        PSHB
        DECB
        ASLB
        ASLB
        ABZ        
        JSR     BCDHEX3X        ; CONVERT 3 BYTES @X
        CLRA
        STD     0,Z
        STE     2,Z


        LDX     #CMP1CPIST3     ; 
        PULB
        PSHB
        DECB
        ABX
        ABX
        ABX
        LDAB    #RAMBANK
        TBZK
        LDZ     #MBPCMP1CPIST3
        PULB    
        PSHB
        DECB
        ASLB
        ASLB
        ABZ        
        JSR     BCDHEX3X        ; CONVERT 3 BYTES @X
        CLRA
        STD     0,Z
        STE     2,Z







        LDX     #CMP1OPENSECONDS     ; 
        PULB
        PSHB
        DECB
        ABX
        ABX
        ABX
        LDAB    #RAMBANK
        TBZK
        LDZ     #MBPCMP1OPENSECONDS
        PULB    
        PSHB
        DECB
        ASLB
        ASLB
        ABZ        
        JSR     BCDHEX3X        ; CONVERT 3 BYTES @X
        CLRA
        STD     0,Z
        STE     2,Z
        
        PULB
        INCB
        CMPB    NOBLNDS
        LBLS    REPTR

        RTS



*/
