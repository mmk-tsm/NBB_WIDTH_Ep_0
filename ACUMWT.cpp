//////////////////////////////////////////////////////
// Acumwt.c
//
// Accumulate batch weights

// P.Smith                              09-03-2005
// First pass 
//
// P.Smith                              28/09/05                              
// Modify offline xxkgs mode to calculate the last batch total required
// This allow the exact amount to be dispensed for the blender.
//
//
// P.Smith                              7/10/05                              
// uncomment CopyBatchData();  
//
// P.Smith                              10/1/06
// Corrected warnings
// make g_arrcTemp unsigned char array.
//
// P.Smith                              29/6/06
// call CopyBatchData();  // asm = GENBATREPORT

//
// P.Smith                              9/8/06
// add long term accumulation counter
// added ZeroLongTermWeightAccumulator
//
// P.Smith                              6/2/7
// Remove printf 
//
// P.Smith                                         28/2/07             
// Modify off target weight check to check if the offline batch weight 
// has exceeded the target weight 
// if it has then terminate the batch & set g_fTotalBatchWeightAccumationLeft to zero                                 
//
// P.Smith                                         4/7/07             
// added g_fComponentLoaderWeightAccumulator for loader weight accumulation
//
// M.McKiernan                      17/9/07 
// Removed g_arrnMBTable[] & g_arrnMBTableFlags[], now in BatVars2
//
// P.Smith                          14/11/07
// g_bUsingSmallBatchSize set when the batch size is changed.
// in terminatecycle this is checked, if set copy batch size back and reset flag.
// recalculate checksum and calculate new target.
// set g_nStopInNBatches to 0 to ensure that there is no possibility of the batch
// pause coming in on the next batch.
// ensure that g_nStopInNBatches is set to 0
//
// P.Smith                          16/11/07
// avoid the possibility of the set weight for the batch exceeding the normal
// batch target weight by checking if the weight is greater than the target, if it
// is then set it to the target weight.
//
// P.Smith                          30/1/08
// correct compiler warnings
//
// P.Smith                          23/6/08
// Correct check sum generation ensure that the size used is correct,
// ie cal data - checksum size and store checksum to g_CalibrationData.m_nChecksum
// this ensures that the checksum will not fail the next time.
// ensure that when the target weight is changed that hte calibration checksum
// is recalculated.
//
// P.Smith                          24/6/08
// remove g_arrnMBTable
//
// P.Smith                          16/10/08
// add ZeroLongTermWeightResettableAccumulator & ZeroLongTermWeightAccumulator
// zero g_fLongTermResettableLengthCounter & g_fLongTermLengthCounter
//
// P.Smith                          27/3/09
// added SaveOrderWeightToNonVolatileMemory
// call SaveOrderWeightToNonVolatileMemory to save order accumulator to non volatile memory
//
// P.Smith                          30/3/09
// load order length g_Order.m_fLength from g_ProcessData.m_fOrderLength
//
// P.Smith                          1/4/09
// there was a problem with the top up mode in that the order weights were not correct.
// this was due to the fact that component 8 was not being reset to 0 and it was 
// getting added in again.
// this is temporarily solved by resetting this component to 0 if in top up mode
// when the totals are being added in.
//
// P.Smith                          27/11/09
// added TOPUPCOMPONENT instead of specific component no.

// M.McKiernan                      6/7/2020...........no changes............testing for GIT.
//////////////////////////////////////////////////////

//*****************************************************************************
// INCLUDES
//*****************************************************************************

#include "General.h"
#include "BatVars.h"
#include "GPMCalc.h"
#include "ConversionFactors.h"
#include "BatchCalibrationFunctions.h"
#include "SetpointFormat.h"
#include "BatchMBIndices.h"
#include "BatVars2.h"
#include "Acumwt.h"
#include "Mbprogs.h"
#include "Bbcalc.h"
#include "BatVars2.h"
#include "Alarms.h"
#include "Pause.h"
#include "CsumCalc.h"
#include "String.h"
#include "Blrep.h"
#include <stdio.h>
#include "Hash.h"

extern CalDataStruct    g_CalibrationData;
extern structSetpointData   g_CurrentRecipe;
extern  OnBoardCalDataStruct   g_OnBoardCalData; 
extern  structRollData      g_Order;


extern  char    Rx1Buff[];
            



//////////////////////////////////////////////////////
// AccumulateWeights()               ASM = ACUMWT
// Adds batch weight to different counters

// P.Smith                              9-3-2005
//////////////////////////////////////////////////////

void AccumulateWeights( void )   // Check for round robin reset.
{
int i;
    AccumulateComponentWeights(); // ASM = ACCCOMPWEIGHTS accumulate component weights

// --review-- this call to CopyWeightsToMB  is not really necessary here.

      CopyWeightsToMB(); // ASM = CPYTWTMB copy weights to modbus
//     merge  ASM = SHPER in line //


    for(i = 0; i < MAX_COMPONENTS; i++)  // calculate component %.
    {
        g_fComponentShiftPercentage[i] = (100 * g_fComponentShiftWeightAccumulator[i]) / g_fTotalShiftWeightAccumulator; 
    }

    CheckIfAtBatchWeight(); //ASM = CHECKIFATBATCHWEIGHT   // checks if at target weight for blend to xx kgs in offline mode
}



//////////////////////////////////////////////////////
// AccumulateComponentWeights()          ASM = ACCCOMPWEIGHTS
// Adds batch weight to component weight accumulators

// P.Smith                              9-3-2005
//////////////////////////////////////////////////////

                                 
void AccumulateComponentWeights( void )   // Check for round robin reset.
{
int i;
float fAddition;
    if(g_bTopUpFlag)    //Top up enabled?
    {
        g_fComponentActualWeight[TOPUPCOMPONENT-1] = 0.0f;   //nbb--todo--put back check this out 
    }
    for(i=0; i < MAX_COMPONENTS; i++)
    {
        g_fComponentFrontWeightAccumulator[i] += g_fComponentActualWeight[i];  // front roll weights
        g_fComponentBackWeightAccumulator[i] += g_fComponentActualWeight[i];  //  back roll weights
        g_fComponentOrderWeightAccumulator[i] += g_fComponentActualWeight[i];  // Order weights
        g_fComponentShiftWeightAccumulator[i] += g_fComponentActualWeight[i];  // Shift weights
        g_fComponentShiftWeightOngoingAccumulator[i] += g_fComponentActualWeight[i];  // Shift weights for 9000
        g_fComponentHourlyWeightAccumulator[i] += g_fComponentActualWeight[i];  //  Hourly weights
        g_fComponentLongTermWeightAccumulator[i] += g_fComponentActualWeight[i];  // Long term weights
        g_fComponentLongTermWeightResettableAccumulator[i] += g_fComponentActualWeight[i];  // Long term weights
        g_fComponentBatchWeightAccumulator[i] += g_fComponentActualWeight[i];  //  Offline batch weight
        g_fComponentLoaderWeightAccumulator[i] += g_fComponentActualWeight[i]; // loader weight 

    }
    
// Total Order Weight
    i = 0;
    fAddition = 0.0;
    for(i=0; i < MAX_COMPONENTS; i++)
    {
        fAddition += g_fComponentOrderWeightAccumulator[i];
    }
    g_fTotalOrderWeightAccumulator = fAddition;             // ASM = TOTWACCO


// Total Front Weight
    i = 0;
    fAddition = 0.0;
    for(i=0; i < MAX_COMPONENTS; i++)
    {
        fAddition += g_fComponentFrontWeightAccumulator[i];
    }
    g_fTotalFrontWeightAccumulator = fAddition;
                                                           // ASM = TOTWACCF

// Total Back Weight
    i = 0;
    fAddition = 0.0;
    for(i=0; i < MAX_COMPONENTS; i++)
    {
        fAddition += g_fComponentBackWeightAccumulator[i];
    }
    g_fTotalBackWeightAccumulator = fAddition;              // ASM = TOTWACCB


// Total Shift Weight
    i = 0;
    fAddition = 0.0;
    for(i=0; i < MAX_COMPONENTS; i++)
    {
        fAddition += g_fComponentShiftWeightAccumulator[i];
    }
    g_fTotalShiftWeightAccumulator = fAddition;             // ASM = TOTWACCS



// Total Shift Weight Ongoing
    i = 0;
    fAddition = 0.0;
    for(i=0; i < MAX_COMPONENTS; i++)
    {
        fAddition += g_fComponentShiftWeightOngoingAccumulator[i];
    }
    g_fTotalShiftWeightOngoingAccumulator = fAddition;   // ASM = TOTWACCS9



// Long Term Weight Ongoing
    i = 0;
    fAddition = 0.0;
    for(i=0; i < MAX_COMPONENTS; i++)
    {
        fAddition += g_fComponentLongTermWeightAccumulator[i];
    }
    g_fTotalLongTermWeightAccumulator = fAddition;      // ASM = TOTWACCBAT

// Long Term Weight Ongoing (resettable)
    i = 0;
    fAddition = 0.0;
    for(i=0; i < MAX_COMPONENTS; i++)
    {
        fAddition += g_fComponentLongTermWeightResettableAccumulator[i];
    }
    g_fTotalLongTermWeightResettableAccumulator = fAddition;      // 
    





// Hourly Weight Ongoing
    i = 0;
    fAddition = 0.0;
    for(i=0; i < MAX_COMPONENTS; i++)
    {
        fAddition += g_fComponentHourlyWeightAccumulator[i];
    }
    g_fTotalHourlyWeightAccumulator = fAddition;        // ASM = TOTWACCH


// Total Batch weight accumulated for offline mode
    i = 0;
    fAddition = 0.0;
    for(i=0; i < MAX_COMPONENTS; i++)
    {
        fAddition += g_fComponentBatchWeightAccumulator[i];
    }
    g_fTotalBatchWeightAccumulator = fAddition;         // ASM = TOTWACCBAT
    g_fTotalHistoryBatchWeightAccumulator = g_fTotalBatchWeightAccumulator;  // ASM = TOTWACCBATHI    
    g_fTotalBatchWeightAccumulatorImp = g_fTotalBatchWeightAccumulator * WEIGHT_CONVERSION_FACTOR;
}



//////////////////////////////////////////////////////
// CheckIfAtBatchWeight()          ASM = CHECKIFATBATCHWEIGHT
// Checks if the has reached a particular batch weight in offline mode
// If it is within 2 batch, split this in half and then dispense the 
// remainder.
//
// P.Smith                              24-3-2005
//////////////////////////////////////////////////////

// --review-- bring this up to the same status unit shipped for the powder coating industry.


void CheckIfAtBatchWeight( void )   
{
int nTemp,nSize;
unsigned char    g_arrcTemp[sizeof(g_CalibrationData)];

   nTemp = sizeof(g_CalibrationData);

    if((g_CalibrationData.m_nBlenderMode == MODE_GPMGPMENXXKGS)||((g_CalibrationData.m_nBlenderMode == MODE_OFFLINEMODE) && (g_CalibrationData.m_nOfflineType == OFFLINETYPE_XXKG)))
    {
        if(g_fTotalBatchWeightAccumulator > g_CurrentRecipe.m_fTotalThroughput)
        { 
                TerminateBatching();
        }
        else
        if(!g_nStopInNBatches)
        {
            g_fTotalBatchWeightAccumationLeft = g_CurrentRecipe.m_fTotalThroughput - g_fTotalBatchWeightAccumulator;
            if(g_CalibrationData.m_nBlenderMode != MODE_GPMGPMENXXKGS)
            {
                if((g_fTotalBatchWeightAccumationLeft) < (2.0 * g_CalibrationData.m_fBatchSize)) // is the remainder less than twice the batch size
                {
                    g_fBatchSizeStorage = g_CalibrationData.m_fBatchSize;                       // store current batch
                    
                    g_CalibrationData.m_fBatchSize = g_fTotalBatchWeightAccumationLeft / 2;     // divide remainder by 2                    
                    nSize = sizeof(g_CalibrationData)- sizeof(g_CalibrationData.m_nChecksum);
                    memcpy(&g_arrcTemp, &g_CalibrationData, sizeof(g_CalibrationData));    // copy structure to array.
                    g_CalibrationData.m_nChecksum = CalculateCheckSum(g_arrcTemp, nSize);
                    g_bUsingSmallBatchSize = TRUE;
                    NewTargetWeightEntry();                                                     // Calculate new target
                    g_nStopInNBatches = 2;                                                      // blending will finished in two batches
                }
                else // NOTATT in ASM
                {
                    g_bBatchIsReady = FALSE;   // indicate that batch is not ready.
                    if(g_CalibrationData.m_nBlenderMode == MODE_GPMGPMENXXKGS)    // 
                    {
                        if(g_fTotalBatchWeightAccumationLeft < g_CurrentRecipe.m_fDeadbandForTargetWeight )  // check is less than deadband
                        {
                            if( (g_arrnMBTable[BATCH_SUMMARY_PROCESS_ALARMS] & ATTARWTALARMBIT) == 0 ) // at target alarm bit not set???
                            {  
                                PutAlarmTable( ATTARWTALARM,  0 );     // indicate alarm,  //PTALTB
                            }
                        }
                        
                    }
                }
            }
            else  // code at ASM = CHKDB is repeated
            {
                if(g_fTotalBatchWeightAccumationLeft < g_CurrentRecipe.m_fDeadbandForTargetWeight )  // check is less than deadband
                {
                    if( (g_arrnMBTable[BATCH_SUMMARY_PROCESS_ALARMS] & ATTARWTALARMBIT) == 0 ) // at target alarm bit not set???
                    {  
                        PutAlarmTable( ATTARWTALARM,  0 );     // indicate alarm,  //PTALTB
                    }
                }
            }
        }
        else   // CHECKIFSTOP in ASM
        {
            if(g_nStopInNBatches == 2)   // last batch the next time
            {
                g_nStopInNBatches--;                
// merge CALCULATELASTBATCH inline
                g_fTotalBatchWeightAccumationLeft = g_CurrentRecipe.m_fTotalThroughput - g_fTotalBatchWeightAccumulator;
                g_CalibrationData.m_fBatchSize = g_fTotalBatchWeightAccumationLeft;         // set batch target                   
                if(g_CalibrationData.m_fBatchSize > g_fBatchSizeStorage)
                {
                    g_CalibrationData.m_fBatchSize = g_fBatchSizeStorage;
                }
  
                g_bUsingSmallBatchSize = TRUE;

                NewTargetWeightEntry();                                                 // Copy batch target

                // merge equivalent of CALCULATEEEPROMCHECKSUM inline 
                
                nSize = sizeof(g_CalibrationData)- sizeof(g_CalibrationData.m_nChecksum);
                memcpy(&g_arrcTemp, &g_CalibrationData, sizeof(g_CalibrationData));    // copy structure to array.
                g_CalibrationData.m_nChecksum = CalculateCheckSum(g_arrcTemp, nSize);
                
            }
            else
            {
                g_nStopInNBatches--;
                if(g_nStopInNBatches == 0)
                {
                    g_CalibrationData.m_fBatchSize = g_fBatchSizeStorage;                               // restore store current batch
                    g_bUsingSmallBatchSize = FALSE;
    
                // merge equivalent of CALCULATEEEPROMCHECKSUM inline 
                    nSize = sizeof(g_CalibrationData)- sizeof(g_CalibrationData.m_nChecksum);
                    memcpy(&g_arrcTemp, &g_CalibrationData, sizeof(g_CalibrationData));    // copy structure to array.
                    g_CalibrationData.m_nChecksum = CalculateCheckSum(g_arrcTemp, nSize);
                    
                    NewTargetWeightEntry();                                                             // Calculate new target
                    TerminateBatching();
                    
                }       
            }   
        }
    }
}


//////////////////////////////////////////////////////
// ZeroLongTermWeightResettableAccumulator()
// zero long term weight accumulator
//
//////////////////////////////////////////////////////



void ZeroLongTermWeightResettableAccumulator( void )   
{
unsigned int i;

    for(i=0; i < MAX_COMPONENTS; i++)
    {
        g_fComponentLongTermWeightResettableAccumulator[i] = 0.0f;
    }
    g_fTotalLongTermWeightResettableAccumulator = 0.0f;
    g_fLongTermResettableLengthCounter = 0.0f;
}

//////////////////////////////////////////////////////
// ZeroLongTermWeightResettableAccumulator()
// zero long term weight accumulator
//
//////////////////////////////////////////////////////

void ZeroLongTermWeightAccumulator( void )   
{
unsigned int i;
    for(i=0; i < MAX_COMPONENTS; i++)
    {
        g_fComponentLongTermWeightAccumulator[i] = 0.0f;
    }
    g_fTotalLongTermWeightAccumulator = 0.0f;
    g_fLongTermLengthCounter = 0.0f;
}


//////////////////////////////////////////////////////
// TerminateBatching()
// Terminates batching as blender has reached target batch weight.
//
//////////////////////////////////////////////////////



void TerminateBatching( void )   

{
    int i,nSize;
    unsigned char    g_arrcTemp[sizeof(g_CalibrationData)];
    if(g_bUsingSmallBatchSize)
    {
        g_bUsingSmallBatchSize = FALSE;
        g_CalibrationData.m_fBatchSize = g_fBatchSizeStorage;                               // restore store current batch
        nSize = sizeof(g_CalibrationData)- sizeof(g_CalibrationData.m_nChecksum);
        memcpy(&g_arrcTemp, &g_CalibrationData, sizeof(g_CalibrationData));    // copy structure to array.
        g_CalibrationData.m_nChecksum = CalculateCheckSum(g_arrcTemp, nSize);
        NewTargetWeightEntry();                                                     // Calculate new target
        
    }
    
    g_nStopInNBatches = 0;
    g_bBatchIsReady = TRUE;                 // indicate batch is ready.
    g_fTotalBatchWeightAccumationLeft = 0.0f;
    g_bOfflineBatchFilled = TRUE;
    g_fTotalBatchWeightAccumulator = 0.0f;  // reset accumulator    
    for(i=0; i < MAX_COMPONENTS; i++)
    {
        g_fComponentBatchWeightAccumulator[i] = 0;  // reset component batch accumulators
        CopyBatchData();  // asm = GENBATREPORT

    }
    if(!g_bNoTargetAlarm) // should an alarm be sounded.  --review-- this is not necessary for the powder coating industry
    {
        g_bSignalXXKgsTargetAlarm = TRUE;
    }
    if(g_CalibrationData.m_nBlenderMode != MODE_GPMGPMENXXKGS)    // ATTALA in ASM
    {
        PauseOn();  // Pause blender
    }

}


//////////////////////////////////////////////////////
// SaveOrderWeightToNonVolatileMemory( void )           
// 
//
//////////////////////////////////////////////////////

void SaveOrderWeightToNonVolatileMemory( void )
{
    int i;
    if(g_nStorageInvervalCtr == 0)
    {
        for(i= 0; i < MAX_COMPONENTS; i++)
        {
            g_ProcessData.m_fComponentOrderWeightAccumulator[i] = g_fComponentOrderWeightAccumulator[i];
        }
        GenerateHashForOrderWeight(); // generate the hash
        g_nStorageInvervalCtr = STORAGE_INTERVAL_IN_MINS;
        g_bSaveAllProcessDataToEEprom = TRUE;
        g_bAllowProcesDataSaveToEEPROM = TRUE;   
    }
}

//////////////////////////////////////////////////////
// LoadOrderWeightFromVolatileMemory( void )           
// loads the order weight from memory if the hash is okay
// if it is not, then weights are cleared.
//
//////////////////////////////////////////////////////

void LoadOrderWeightFromVolatileMemory( void )
{
    int i;
    WORD wTemp;
    int nSize = sizeof(g_ProcessData.m_fComponentOrderWeightAccumulator);
    unsigned char cBuffer[nSize];
    memcpy(&cBuffer,&g_ProcessData.m_fComponentOrderWeightAccumulator,nSize);    // copy structure to array.
    wTemp = TSMHash(cBuffer,nSize);
    
    //SetupMttty();
    //iprintf("\n calc hash %d actual %d",wTemp,g_ProcessData.m_nOrderWeightHash);
    
    if(wTemp == g_ProcessData.m_nOrderWeightHash)
    {
        for(i= 0; i < MAX_COMPONENTS; i++)
        {
            g_fComponentOrderWeightAccumulator[i] = g_ProcessData.m_fComponentOrderWeightAccumulator[i];
        }
        g_Order.m_fLength = g_ProcessData.m_fOrderLength;
    }
    else
    {
        for(i= 0; i < MAX_COMPONENTS; i++)
        {
            g_fComponentOrderWeightAccumulator[i] = 0;
        }
        memcpy(&cBuffer,&g_ProcessData.m_fComponentOrderWeightAccumulator,nSize);    // copy structure to array.
        g_ProcessData.m_nOrderWeightHash = TSMHash(cBuffer,nSize);
        g_bSaveAllProcessDataToEEprom = TRUE;
        g_bAllowProcesDataSaveToEEPROM = TRUE;   
    }
            
}




/*  


}.



*/

/*
;  ACUMWT  IS A ROUTINE TO ACCUMULATE WEIGHT  IN FRWTACC, BRWTACC & OWTACC
;          ASSUMED TO BE CALLED EVERY 2 SECS
ACUMWT: 
       JSR    ADDSHIFTWT        ; ACCUMULATE SHIFT WEIGHT.
       JSR     ACCCOMPWEIGHTS
       JSR    CPYTWTMB          ; COPY TOTAL Wt.S -> MODBUS        
       JSR     SHPER           ;
       JSR     CHECKIFATBATCHWEIGHT
       RTS

*/
/*

ACCCOMPWEIGHTS:
        LDAB    #1      
REPADDIT:
        PSHB                    ;       SAVE COMPONENT NO
        LDX     #CMP1ACTWGT     ; COMP 1 ACTUAL WEIGHT
        DECB                    ;
        ABX
        ABX
        ABX
        JSR     CMOVE           ; COMP WEIGHT TO A REG 00 00 XX XX X.Y GRAMS
        
ISTUBES:        
        PULB
        PSHB                    ; RECOVER B
        LDX     #CH1WACCF        ; ACCUMULATOR FOR FRONT CMP WEIGHT
        DECB
        ABX
        ABX
        ABX
        ABX
        ABX                     ; ADDRESS FOR DATA STORAGE.
        PSHM    X               ; SAVE ADDRESS
        JSR     ITAREG          ;
        JSR     ADD
        PULM    X               ; RECOVER.
        JSR     IFEREG          ; STORE ACCUMULATED DATA.
        PULB
        PSHB                    ; RECOVER B
        LDX     #CH1WACCB        ; ACCUMULATOR FOR FRONT CMP WEIGHT
        DECB
        ABX
        ABX
        ABX
        ABX
        ABX                     ; ADDRESS FOR DATA STORAGE.
        PSHM    X               ; SAVE ADDRESS
        JSR     ITAREG          ;
        JSR     ADD
        PULM    X               ; RECOVER.
        JSR     IFEREG          ; STORE ACCUMULATED DATA.        
        LDX     #CMP1ACTWGT     ; COMP 1 ACTUAL WEIGHT
        PULB
        PSHB
        DECB                    ;
        ABX
        ABX
        ABX
        JSR     CMOVE           ; COMP WEIGHT TO A REG 00 00 XX XX X.Y GRAMS
        PULB
        PSHB
        LDX     #CH1WACCO        ; ACCUMULATOR FOR FRONT CMP WEIGHT
        DECB
        ABX
        ABX
        ABX
        ABX
        ABX                     ; ADDRESS FOR DATA STORAGE.
        PSHM    X               ; SAVE ADDRESS
        JSR     ITAREG          ;
        JSR     ADD
        PULM    X               ; RECOVER.
        JSR     IFEREG          ; STORE ACCUMULATED DATA.   

        PULB
        PSHB                    ; RECOVER B
        LDX     #CH1WACCS        ; ACCUMULATOR FOR SHIFT WEIGHT DATA
        DECB
        ABX
        ABX
        ABX
        ABX
        ABX                     ; ADDRESS FOR DATA STORAGE.
        PSHM    X               ; SAVE ADDRESS
        JSR     ITAREG          ;
        JSR     ADD
        PULM    X               ; RECOVER.
        JSR     IFEREG          ; STORE ACCUMULATED DATA.        

        PULB
        PSHB                    ; RECOVER B
        LDX     #CH1WACCS9        ; ACCUMULATOR FOR SHIFT WEIGHT DATA
        DECB
        ABX
        ABX
        ABX
        ABX
        ABX                     ; ADDRESS FOR DATA STORAGE.
        PSHM    X               ; SAVE ADDRESS
        JSR     ITAREG          ;
        JSR     ADD
        PULM    X               ; RECOVER.
        JSR     IFEREG          ; STORE ACCUMULATED DATA.        


        PULB
        PSHB                    ; RECOVER B
        LDX     #CH1WACCH       ; ACCUMULATOR FOR HOURLY WEIGHT DATA
        DECB
        ABX
        ABX
        ABX
        ABX
        ABX                     ; ADDRESS FOR DATA STORAGE.
        PSHM    X               ; SAVE ADDRESS
        JSR     ITAREG          ;
        JSR     ADD
        PULM    X               ; RECOVER.
        JSR     IFEREG          ; STORE ACCUMULATED DATA.        


        PULB
        PSHB                    ; RECOVER B
        LDX     #CH1WACCM       ; ACCUMULATOR FOR MONTHLY WEIGHT DATA
        DECB
        ABX
        ABX
        ABX
        ABX
        ABX                     ; ADDRESS FOR DATA STORAGE.
        PSHM    X               ; SAVE ADDRESS
        JSR     ITAREG          ;
        JSR     ADD
        PULM    X               ; RECOVER.
        JSR     IFEREG          ; STORE ACCUMULATED DATA.        

        PULB
        PSHB                    ; RECOVER B
        LDX     #CH1WACCBAT     ; ACCUMULATOR FOR MONTHLY WEIGHT DATA
        DECB
        ABX
        ABX
        ABX
        ABX
        ABX                     ; ADDRESS FOR DATA STORAGE.
        PSHM    X               ; SAVE ADDRESS
        JSR     ITAREG          ;
        JSR     ADD
        PULM    X               ; RECOVER.
        JSR     IFEREG          ; STORE ACCUMULATED DATA.        


        PULB
        CMPB    NOBLNDS         ; AT LAST ONE 
        BEQ     YESEXIT         ;
        INCB                    ; TO NEXT BLEND.
        LBRA     REPADDIT                     
        
YESEXIT:        
        JSR     GENTOTALS       ; GENERATE TOTALS
        JSR     ORDERPER        ; CALCULATE ACTUAL %
        RTS

;
;
;       
GENTOTALS:
        LDX     #TOTWACCO       ;       TOTAL WEIGHT
        STX     TOTSTORAGE      ;       BUFFER
        LDX     #CH1WACCO       ;       COMPONENT TOTAL
;        JSR     ADDCMPTOTALS    ;        
        JSR     ADDCMPTOTALSFR
        LDX     #TOTWACCF       ;       TOTAL WEIGHT
        STX     TOTSTORAGE      ;       BUFFER
        LDX     #CH1WACCF       ;       COMPONENT TOTAL
        JSR     ADDCMPTOTALS    ;                
        LDX     #TOTWACCB       ;       TOTAL WEIGHT
        STX     TOTSTORAGE      ;       BUFFER
        LDX     #CH1WACCB       ;       COMPONENT TOTAL
        JSR     ADDCMPTOTALS    ;        

        LDX     #CURSHIFTWEIGHT ;       
        STX     TOTSTORAGE      ;       
        LDX     #CH1WACCS       ;       
        JSR     ADDCMPTOTALS  ;        

        LDX     #TOTWACCS9      ;       
        STX     TOTSTORAGE      ;       
        LDX     #CH1WACCS9      ;       
        JSR     ADDCMPTOTALS  ;        

        LDX     #TOTWACCM       ;       
        STX     TOTSTORAGE      ;       
        LDX     #CH1WACCM       ;       
        JSR     ADDCMPTOTALS  ;        

        LDX     #TOTWACCH       ;       
        STX     TOTSTORAGE      ;       
        LDX     #CH1WACCH       ;       
        JSR     ADDCMPTOTALS  ;        

        LDX     #TOTWACCBAT   ;       
        STX     TOTSTORAGE    ;       
        LDX     #CH1WACCBAT   ;       
        JSR     ADDCMPTOTALS  ;        
        LDX     TOTWACCBAT    ; TRANSFER TO HISTORY
        STX     TOTWACCBATH   ;
        LDX     TOTWACCBAT+2  ;
        STX     TOTWACCBATH+2 ;
        LDAA    TOTWACCBAT+4  ;
        STAA    TOTWACCBATH+4 ;
        JSR     CONVKGHRI     ; CONVERT TOT IMPERIAL MODE.
        LDX     TOTWACCBATI    ; TRANSFER TO HISTORY
        STX     TOTWACCBATHI   ;
        LDX     TOTWACCBATI+2    ; TRANSFER TO HISTORY
        STX     TOTWACCBATHI+2   ;
        LDAA    TOTWACCBATI+4  ;
        STAA    TOTWACCBATHI+4 ;
        RTS


*/

/*
; THIS ROUTINE CHECKS IF THE BLENDER IN OFFLINE MODE HAS REACHED THE BATCH WEIGHT
; IF IT HAS THEN PAUSE  MODE IS INITIATED.

                
CHECKIFATBATCHWEIGHT:        
        LDAA    BLENDERMODE             ; CHECK MODE.
        CMPA    #GPMGPMENXXKGS          ; ALARM ON XX KGS
        BEQ     CHKATT                  ; CHECK IF AT TARGET.
        CMPA    #OFFLINEMODE            ; OFFLINE MODE.
        LBNE     XITBWC                  ; EXIT.
        LDAA    OFFLINETYPE             ; CHECK TYPE.
        CMPA    #OFFLINETYPEXXKG        ; DISPENSE XX KGS.
        LBNE     XITBWC                  ;
CHKATT  TST     STOPINNBATCHES          ; TIME TO STOP ?
        LBNE     CHKIFSTOP
        JSR     CLRAC                   ; A =C = 0
        LDX     TKGHSP        
        STX     AREG
        LDAA    TKGHSP+2
        STAA    AREG+2
        LDX     #TOTWACCBAT             ; TOTAL WEIGHT.
        JSR     ITCREG                  ; XX XX .YY YY KGS TO 
        JSR     SUB                     ; SETPOINT - ACTUAL.
        LDX     #TOTWACCBATLEFT         ; AMOUNT LEFT
        JSR     IFEREG                
        LDAA    BLENDERMODE             ; CHECK MODE.
        CMPA    #GPMGPMENXXKGS          ; ALARM ON XX KGS
        LBEQ     CHKDB                   ; CHECK IF AT TARGET.
        JSR     CLRAC        
        LDX     #AREG
        JSR     IFEREG
        LDD     DFTW1                   ; READ BATCH SIZE
        ASLD
        ASLD
        ASLD
        ASLD
        PSHM    D
        STD     MATHCREG+2
        JSR     SUB                     ; A - C

        JSR     CLRAC
        PULM    D
        STD     MATHCREG+2
        LDX     #AREG
        JSR     IFEREG                  ;
        JSR     SUB
        TST     MATHDREG
        LBEQ     NOTATT                ; NOT WITHIN 2 BATCHES
        
        JSR     CLRAC
        LDX     #TOTWACCBATLEFT         ; AMOUNT LEFT
        JSR     ITCREG                
        LDAA    #2
        STAA    AREG+4
        JSR     DIV
        
        
        LDX     DFTW1   
        STX     DFTW1STORE
        LDE     EREG+2
        LSRE        
        RORB
        LSRE
        RORB
        LSRE
        RORB
        LSRE
        RORB
        STE     DFTW1
        STAB     DFTW1+2
        JSR     NEWTARWEIGHTENTRY
        LDAA    #2
        STAA    STOPINNBATCHES          ; STOP IN N BATCHES
        BRA     XITBWC
                
CHKIFSTOP:
        DEC     STOPINNBATCHES
        BNE     XITBWC
        
        LDX     DFTW1STORE
        STX     DFTW1   
        JSR     NEWTARWEIGHTENTRY


;        TST     MATHDREG                    ; CHECK POLARITY OF RESULT.
;        BEQ     NOTATT                  ; NOT AT TOTAL.
        LDAA    #1
        STAA    BATCHREADY

;        JSR     STARTUPON
        LDX     #TOTWACCBAT             ; TOTAL Shift WT. ACC
        LDAB    #5*9                    ; 8 BLENDS + PIB + TOTAL (6 bytes/accum.)
        JSR     CLRBBS                  ; CLEAR
        JSR     GENBATREPORT            ; BATCH REPORT GENERATION.
        TST     NOTARGETALARM
        BNE     ATTALA
        LDAA    MBPROALRM+1               ; 
        ANDA    #ATTARWTALARMBIT        ; AT TARGET WEIGHT BIT SET.
        BNE     ATTALA                  ; AT TARGET ALREADY ACTIVE.
        LDAA    #ATTARWTALARM           ; AT TARGET WEIGHT ALARM.
        JSR     PTALTB                  ; GENERATE ALARM.
        
ATTALA  LDAA    BLENDERMODE             ; CHECK MODE.
        CMPA    #GPMGPMENXXKGS          ; ALARM ON XX KGS
        BEQ     XITBWC                  ; CHECK IF AT TARGET.
        JSR     PAUSEON                 ; PAUSE BLENDER IMMEDIATELY
        BRA     XITBWC                  ; EXIT.
NOTATT  CLR     BATCHREADY
        LDAA    BLENDERMODE             ; CHECK BLENDER MODE.
        CMPA    #GPMGPMENXXKGS
        BNE     XITBWC                  ; NO PAUSE IF GPM MODE.
CHKDB   LDX     #AREG
        JSR     IFEREG                  ; DIFFERENCE TO A
        JSR     CLCREG                  ; C = 0
        LDD     DBXXKGS                 ; DEADBANDD IN KILOS.
        STD     MATHCREG+1              ; STORE.
        JSR     SUB                     ; DIFFERENCE - DEADBAND.
        TST     MATHDREG                ; CHECK POLARITY OF RESULT.
        BEQ     XITBWC                  ; NOT AT TOTAL.
        JSR     INDICATETOTALREACHED               
XITBWC  RTS        



INDICATETOTALREACHED:
        TST     NOTARGETALARM
        BNE     ATTALA1
        LDAA    MBPROALRM+1               ; 
        ANDA    #ATTARWTALARMBIT        ; AT TARGET WEIGHT BIT SET.
        BNE     ATTALA1                 ; AT TARGET ALREADY ACTIVE.
        LDAA    #ATTARWTALARM           ; AT TARGET WEIGHT ALARM.
        JSR     PTALTB                  ; GENERATE ALARM.
ATTALA1 RTS

*/

/*
ADDCMPTOTALSFR:
        LDAB    #1
        STAB    FULLRES
        BRA     CONAD

ADDCMPTOTALS:
        CLR     FULLRES
CONAD   LDAB    #RAMBANK
        TBZK
        LDZ     #SEQTABLE       ; SEQUENCE TABLE SET UP
        JSR     CLRAC
;        LDX     #CH1WACCO        ; CHANNEL WEIGHT ACCUMULATOR
NEXTADD:
        PSHM    X
        LDAB    0,Z
        ANDB    #$0F
        DECB
        ABX
        ABX
        ABX
        ABX
        ABX
        JSR     ITAREG          
        TST     FULLRES
        BNE     FULRES
        CLR     AREG+4          ; DISPLAYED ADDITION ADDS UP
FULRES  JSR     ADD             ; DO ADDITION
        LDX     #MATHCREG
        JSR     IFEREG          ; TRANSFER RESULT.
        PULM    X               ; RECOVER X REG
        AIZ     #1              ; TABLE POINTER
        LDAA    0,Z             ; NEXT COMP ?
        CMPA    #SEQDELIMITER   ; DELIMITER
        BNE     NEXTADD         ; NEXT COMP
        LDX     TOTSTORAGE      ; TOTAL STORAGE POINTER
        JSR     IFEREG
        RTS
        
*/

/*

mce12-15

; THIS ROUTINE CHECKS IF THE BLENDER IN OFFLINE MODE HAS REACHED THE BATCH WEIGHT
; IF IT HAS THEN PAUSE  MODE IS INITIATED.

                
CHECKIFATBATCHWEIGHT:        
        LDAA    BLENDERMODE             ; CHECK MODE.
        CMPA    #GPMGPMENXXKGS          ; ALARM ON XX KGS
        BEQ     CHKATT                  ; CHECK IF AT TARGET.
        CMPA    #OFFLINEMODE            ; OFFLINE MODE.
        LBNE     XITBWC                  ; EXIT.
        LDAA    OFFLINETYPE             ; CHECK TYPE.
        CMPA    #OFFLINETYPEXXKG        ; DISPENSE XX KGS.
        LBNE     XITBWC                  ;
CHKATT  TST     STOPINNBATCHES          ; TIME TO STOP ?
        LBNE     CHKIFSTOP
        JSR     CLRAC                   ; A =C = 0
        LDX     TKGHSP        
        STX     AREG
        LDAA    TKGHSP+2
        STAA    AREG+2
        LDX     #TOTWACCBAT             ; TOTAL WEIGHT.
        JSR     ITCREG                  ; XX XX .YY YY KGS TO 
        JSR     SUB                     ; SETPOINT - ACTUAL.
        LDX     #TOTWACCBATLEFT         ; AMOUNT LEFT
        JSR     IFEREG                
        LDAA    BLENDERMODE             ; CHECK MODE.
        CMPA    #GPMGPMENXXKGS          ; ALARM ON XX KGS
        LBEQ     CHKDB                   ; CHECK IF AT TARGET.
        JSR     CLRAC        
        LDX     #AREG
        JSR     IFEREG
        LDD     DFTW1                   ; READ BATCH SIZE
        ASLD
        ASLD
        ASLD
        ASLD
        PSHM    D
        STD     MATHCREG+2
        JSR     SUB                     ; A - C

        JSR     CLRAC
        PULM    D
        STD     MATHCREG+2
        LDX     #AREG
        JSR     IFEREG                  ;
        JSR     SUB
        TST     MATHDREG
        LBEQ     NOTATT                ; NOT WITHIN 2 BATCHES
        
        JSR     CLRAC
        LDX     #TOTWACCBATLEFT         ; AMOUNT LEFT
        JSR     ITCREG                
        LDAA    #2
        STAA    AREG+4
        JSR     DIV
        
        
        LDX     DFTW1   
        STX     DFTW1STORE
        LDE     EREG+2
        LSRE        
        RORB
        LSRE
        RORB
        LSRE                   
        RORB
        LSRE
        RORB
        STE     DFTW1
        STAB     DFTW1+2
        JSR     CALCULATEEEPROMCHECKSUM
        JSR     NEWTARWEIGHTENTRY
        LDAA    #2
        STAA    STOPINNBATCHES          ; STOP IN N BATCHES
        LBRA     XITBWC
                
CHKIFSTOP:
        LDAA    STOPINNBATCHES
        CMPA    #2                      ; ONE BATCH LEFT ?
        BNE     NOTLST                  ; NO
        DEC     STOPINNBATCHES
        JSR     CALCULATELASTBATCH      ; CALCULATE LAST BATCH.
        BRA     XITBWC

NOTLST  DEC     STOPINNBATCHES
        BNE     XITBWC
        
        LDX     DFTW1STORE
        STX     DFTW1   
        JSR     CALCULATEEEPROMCHECKSUM
        JSR     NEWTARWEIGHTENTRY


;        TST     MATHDREG                    ; CHECK POLARITY OF RESULT.
;        BEQ     NOTATT                  ; NOT AT TOTAL.
        LDAA    #1
        STAA    BATCHREADY
        STAA    OFFLINEBATCHFILLED

;        JSR     STARTUPON
        LDX     #TOTWACCBAT             ; TOTAL Shift WT. ACC
        LDAB    #5*9                    ; 8 BLENDS + PIB + TOTAL (6 bytes/accum.)
        JSR     CLRBBS                  ; CLEAR
        JSR     GENBATREPORT            ; BATCH REPORT GENERATION.
        TST     NOTARGETALARM
        BNE     ATTALA
        LDAA    #1
        STAA    SIGNALATTARGETALARMFLAG  ; SIGNAL ALARM


ATTALA:  
        LDAA    BLENDERMODE             ; CHECK MODE.
        CMPA    #GPMGPMENXXKGS          ; ALARM ON XX KGS
        BEQ     XITBWC                  ; CHECK IF AT TARGET.
        JSR     PAUSEON                 ; PAUSE BLENDER IMMEDIATELY
        BRA     XITBWC                  ; EXIT.
NOTATT  CLR     BATCHREADY
        LDAA    BLENDERMODE             ; CHECK BLENDER MODE.
        CMPA    #GPMGPMENXXKGS
        BNE     XITBWC                  ; NO PAUSE IF GPM MODE.
CHKDB   LDX     #AREG
        JSR     IFEREG                  ; DIFFERENCE TO A
        JSR     CLCREG                  ; C = 0
        LDD     DBXXKGS                 ; DEADBANDD IN KILOS.
        STD     MATHCREG+1              ; STORE.
        JSR     SUB                     ; DIFFERENCE - DEADBAND.
        TST     MATHDREG                ; CHECK POLARITY OF RESULT.
        BEQ     XITBWC                  ; NOT AT TOTAL.
        JSR     INDICATETOTALREACHED               
XITBWC  RTS        

CALCULATELASTBATCH:
        JSR     CLRAC                   ; A =C = 0
        LDX     TKGHSP        
        STX     AREG
        LDAA    TKGHSP+2
        STAA    AREG+2
        LDX     #TOTWACCBAT             ; TOTAL WEIGHT.
        JSR     ITCREG                  ; XX XX .YY YY KGS TO 
        JSR     SUB                     ; SETPOINT - ACTUAL.
        LDX     #TOTWACCBATLEFT         ; AMOUNT LEFT
        JSR     IFEREG                
        LDE     EREG+2
        LSRE        
        RORB
        LSRE
        RORB
        LSRE                   
        RORB
        LSRE
        RORB
        STE     DFTW1                   ; TARGET FOR LAST BATCH.
        STAB     DFTW1+2
        JSR     CALCULATEEEPROMCHECKSUM
        JSR     NEWTARWEIGHTENTRY
        RTS








SIGNALATTARGETALARM:
        LDAA    MBPROALRM+1               ; 
        ANDA    #ATTARWTALARMBIT        ; AT TARGET WEIGHT BIT SET.
        BNE     ATTAL1                  ; AT TARGET ALREADY ACTIVE.
        LDAA    #ATTARWTALARM           ; AT TARGET WEIGHT ALARM.
        JSR     PTALTB                  ; GENERATE ALARM.
ATTAL1  RTS


INDICATETOTALREACHED:
        TST     NOTARGETALARM
        BNE     ATTALA1
        LDAA    MBPROALRM+1               ; 
        ANDA    #ATTARWTALARMBIT        ; AT TARGET WEIGHT BIT SET.
        BNE     ATTALA1                 ; AT TARGET ALREADY ACTIVE.
        LDAA    #ATTARWTALARM           ; AT TARGET WEIGHT ALARM.
        JSR     PTALTB                  ; GENERATE ALARM.
ATTALA1 RTS


*/



