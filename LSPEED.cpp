//////////////////////////////////////////////////////
// P.Smith                              25-02-2005
// AccumulateSEIPulses, AddPulsesToLineSpeedCounters

// P.Smith                              1-03-2005
// Added CheckForZeroLineSpeed                      
// CheckForLineSpeedChange
//
// P.Smith                              3-3-2005
// Copy Line speed to modbus table
// Ensure that g/m is calculated not kg/m for weight per meter value.
// Add code to remove zero line speed alarm
//
//
// P.Smith                              31-05-2005      
// Ensure that zero line speed check is done when the blender is in 
// grams per meter mode. >= MODE_GPMGPMENTRY and <= MODE_GPSQMENTRY                                    
//
//
// P.Smith                              12/7/07 
// read of line speed pulses changed to read ongoing counter from SEI.
// difference in current and previous values calculated and resulting 
// difference is added on to the line speed counters.
// The line speed calculation uses the one second reading from the SEI as normal.
//
// P.Smith                              4/10/05
// Rename low priority todos
//
// P.Smith                              24/10/05
// Removed "testonly" code
//
// P.Smith                              17/11/05
// only transfer g_fLineSpeed5sec to g_fLineSpeed5secHistory when the activation 
// no has been reached.
// call SetExtruderControlStartup
//
// P.Smith                              10/1/06
// correct warnings, remove unused local variable fTemp,fTemp1
//
// P.Smith                      21/2/06
// first pass at netburner hardware conversion.
// included  <basictypes.h>
// changed unions IntsAndLong to WordAndDWord
// PIT_FREQUENCY -> BLENDER_PIT_FREQUENCY
//
// P.Smith                              19/5/06
// check for zero line speed g_fLineSpeed5secHistory
//
// P.Smith                            29/5/06 
// add imperial conversion factor 
//
// check for MINIMUM_LENGTH to avoid a divide by zero.
//
// P.Smith                            14/11/06 
// if g_CalibrationData.m_bLineSpeedSignalType is line speed over the comms.
// use the comms line speed value.
//
// P.Smith                            15/11/06 
// added IntegrateCommsLineSpeed
// Modify AccumulateFrontLength and AccumulateBackLength to accumulate length 
// from comms line speed.
//
// P.Smith                      16/11/06  
//  call INTEGLSPD (integrate line speed - Comms) on second rollover.
//
// P.Smith                            11/1/07 
// name change g_CalibrationData.m_nLineSpeedSignalType
//
// P.Smith                            19/7/07 
// when the SEI pulses is read for the first time, g_bFirstPulsesReadFromSEI (TRUE)
// the current pulses is stored in the previous pulses and no pulse differnce is 
// calculated, this originally had the effect of showing a large pulse count when
// the NBB was reset and the SEI was left powered up.
// Copy of order length to modbus table, remove original copy, this was in the back roll
// weight accumualte which is not run if in tube
//
// M.McKiernan                      17/9/07 
// Removed g_arrnMBTable[] & g_arrnMBTableFlags[], now in BatVars2
//
// P.Smith                                            14/11/07      
// calculate multiplication factor for line speed, divide line     
// speed averaging factor into 60.     
// Modified control mode format implemented
// allow for new bit set up for control  mode.                     
//
// P.Smith                          30/1/08
// correct compiler warnings
//
// P.Smith                          7/5/08
// in CheckForZeroLineSpeed & ReactToChangeInLineSpeed include MODE_GP12SQFEET in the check for the correct mode
//
// P.Smith                          5/6/08
// in CheckForZeroLineSpeed correct auto manual check and mode check, the line speed
// check was being run even when the blender was in manual
//
// P.Smith                          15/7/08
// add zero line speed and line speed change to event log.
//
// P.Smith                          23/7/08
// remove g_arrnMBTable,comms arrays externs
//
// P.Smith                          14/10/08
// add length to g_fLongTermLengthCounter & g_fLongTermResettableLengthCounter
//
// P.Smith                          4/12/08
// increment history line speed changed counter
//
// P.Smith                          30/3/09
// copy order length to g_ProcessData.m_fOrderLength to allow it to be held in 
// non volatile memory
//
// P.Smith                          30/3/09
// corrected calculation of estimated pulses. there was a problem when the divide
// by BLENDER_PIT_FREQUENCY was not working unless it was inside brackets 
// added nOldEstimatedPulses to hold old value & g_lOrigBatchPulsesAccumulator
//
// P.Smith                          8/6/09
// modify ReactToChangeInLineSpeed to automatically update the g/m / kg/hr when
// the blender is in manual mode of operation
//
//////////////////////////////////////////////////////

//*****************************************************************************
// INCLUDES
//*****************************************************************************
#include "General.h"
#include "BatVars.h"
#include "BatVars2.h"
#include "ConversionFactors.h"       
#include "BatchCalibrationFunctions.h"
#include "SetpointFormat.h"
#include "BatchMBIndices.h"   
#include "Conalg.h"
#include "Alarms.h"
#include "TSMPeripheralsMBIndices.h"
#include "Lspeed.h"
#include "BlRep.h"
#include <Stdio.h>
#include "Eventlog.h"



extern  CalDataStruct   g_CalibrationData;                                                                
extern  structSetpointData  g_CurrentRecipe;


extern  structRollData      g_FrontRoll;
extern  structRollData      g_BackRoll;
extern  structRollData      g_Order;
extern  structRollData      g_CurrentShiftFront;
extern  structRollData      g_OngoingShift;






//////////////////////////////////////////////////////
// AccumulateSEIPulses( void )         from ASM = ACCSEIPULSES
//
// Add one second pulses from the SEI to the various pulse counters
// Check for the 1st time
//
// 
//
//////////////////////////////////////////////////////

void AccumulateSEIPulses( void )
{        
    union WordAndDWord lPulses;
    
    if(!g_bLengthCalculationInProgress)   // only run calculation if length calc not in progress
    {
               
        if(g_CalibrationData.m_nPeripheralCardsPresent & SEIENBITPOS)   // sei enabled
        {  
            if(g_bSEIPulsesReady)
            {
                g_bSEIPulsesReady = FALSE;  // reset flag
                lPulses.nValue[0] = g_arrnReadSEIMBTable[MB_SEI_LSPD_ACCUM_MSW];
                lPulses.nValue[1] = g_arrnReadSEIMBTable[MB_SEI_LSPD_ACCUM_LSW];
                g_lCurrentPulses = lPulses.lValue;
                if(g_bFirstPulsesReadFromSEI)
                {
                    g_bFirstPulsesReadFromSEI = FALSE;
                    g_lPreviousPulses = g_lCurrentPulses;
                }  
                else
                {
                
                    if(g_lCurrentPulses >= g_lPreviousPulses) // normal condition - no over flow
                    {
                        g_nPulsesDifference = g_lCurrentPulses - g_lPreviousPulses;
                    }
                    else  // overflow  --review that this over flow calculatin is operational.
                    {
                        g_nPulsesDifference = g_lCurrentPulses + (0xffffffff - g_lPreviousPulses);
                    }
                    g_lPreviousPulses = g_lCurrentPulses; 
                    
                    AddPulsesToLineSpeedCounters(g_nPulsesDifference); // add pulses to counters                
                }
            }
        }
        else  // SEI not enabled - on board line speed
        {
            
//          --todolp-- on board line speed read --review-- is on board line speed required ?
//            g_nCurrentOnBoardPulses = GPT_TCNT;          // current TCNT time
//            if(g_nCurrentOnBoardPulses < g_nPreviousOnBoardPulses)  // counter has overflowed
//            {
//                g_lDifferenceInPulses = 0xffff - g_nPreviousOnBoardPulses + g_nCurrentOnBoardPulses;  // counter has over flowed ffff - previoius + current
//            }  
//              else
//              {
//                g_lDifferenceInPulses = g_nCurrentOnBoardPulses - g_nPreviousOnBoardPulses;  // counter has over flowed ffff - previoius + current
//
//              }  

//            AddPulsesToLineSpeedCounters(g_lDifferenceInPulses);                
//            g_lSEIPreviousPulses = g_lSEICurrentPulses; // current to previous pulses
        }
    }    
}



//////////////////////////////////////////////////////
// AddPulsesToLineSpeedCounters(int nPulses)         from ASM = ADDTOLSPDCTRS
//
// 
//
//////////////////////////////////////////////////////

void AddPulsesToLineSpeedCounters( int nPulses )
{  
    g_lBatchPulsesAccumulator     = g_lBatchPulsesAccumulator + nPulses;
//            g_l30SecondPulsesAccumulator  = g_l30SecondPulsesAccumulator + l_Pulses;  
    g_lFrontRollPulsesAccumulator = g_lFrontRollPulsesAccumulator + nPulses;
    g_lBackRollPulsesAccumulator  = g_lBackRollPulsesAccumulator + nPulses;
    g_lHourlyPulsesAccumulator    = g_lHourlyPulsesAccumulator + nPulses;
    
// --review-- that this is okay for linespeed calculation.
    g_lLineSpeedPulse5SecondAccumulator = g_lLineSpeedPulse5SecondAccumulator + g_nSEIPulsesPerSecond;


//    g_lLineSpeedPulse5SecondAccumulator = g_lLineSpeedPulse5SecondAccumulator + nPulses;
    if(!g_bLengthCalculationInProgress)   // only run calculation if length calc not in progress
    {
        g_nLineSpeedPulse5SecondCounter++;
        if(g_nLineSpeedPulse5SecondCounter >= g_CalibrationData.m_nLineSpeedAverage)
       {
            g_nLineSpeedPulse5SecondCounter = 0;
            g_lLineSpeedPulse5SecondTotal = g_lLineSpeedPulse5SecondAccumulator;
            g_lLineSpeedPulse5SecondAccumulator = 0;
            g_bLineSpeedUpdate = TRUE;              // indicate line speed has changed 
        }
    }                                 
}




//////////////////////////////////////////////////////
// LineSpeedCalculation( void )         from ASM = LPEED5
//
// Calculates line speed from line speed constant.
//
// P.Smith                              1/3/5
//
//////////////////////////////////////////////////////

void LineSpeedCalculation( void )
{
float   fTemp;


    fTemp = (float)g_lLineSpeedPulse5SecondTotal;   // cast pulsesto float
    if(g_CalibrationData.m_nLineSpeedSignalType == PULSES_LINE_SPEED)   //
    {
        if(g_CalibrationData.m_nLineSpeedAverage > 0)
        {
            g_fLineSpeed5sec = ((float)(60/g_CalibrationData.m_nLineSpeedAverage) * fTemp *  g_CalibrationData.m_fLineSpeedConstant)/(1000000.0);//  
        }
        else  // avoid divide by 0
        {
            g_fLineSpeed5sec = ((12) * fTemp *  g_CalibrationData.m_fLineSpeedConstant)/(1000000.0);//  
        }
    }
    else
    if(g_CalibrationData.m_nLineSpeedSignalType == COMMUNICATION_LINE_SPEED)   //nbb--modbus download of signal type.
    {
        g_fLineSpeed5sec = g_fCommunicationsLineSpeed;
    }
// add CONVLSPD inline    
    g_fLineSpeed5secFtPMinute = g_fLineSpeed5sec / LINE_SPEED_CONVERSION_FACTOR; // asm = CONVLSPD


// merge ASM TOTFLSP in line.
   g_lFrontTotalLengthPulses += g_lFrontRollPulsesAccumulator ;  
   g_lFrontRollPulsesAccumulator = 0; // set pulse accumulator to 0.    

// merge ASM TOTBLSP in line.
   g_lBackTotalLengthPulses += g_lBackRollPulsesAccumulator ;  
   g_lBackRollPulsesAccumulator = 0; // set 5 seconds counter to zero.    

    CheckForZeroLineSpeed();    // check for zero line speed
    CheckForLineSpeedChange(); //ASM = LS5CCH
    
    fTemp =    g_fLineSpeed5sec;
    fTemp *=   g_fLineSpeedConversionFactor;
        
    g_arrnMBTable[BATCH_SUMMARY_LINE_SPEED] = (int)(fTemp * 100);        // ASM = CPYLSPMB
    if(g_nEstimationContinueCounter)    
    {
        g_nEstimationContinueCounter--;  //decremenet estimation counter
        g_bRunEst = TRUE;                // indicate that estimation should be run.
    }
}

//////////////////////////////////////////////////////
// CheckForZeroLineSpeed( void )         from ASM = CHECKLS
//
// Checks is line speed is less  than min value
// If it is switch the blender to manual and generate alarm
// 
//////////////////////////////////////////////////////

void CheckForZeroLineSpeed( void )
{
    if( g_fLineSpeed5sec < MINIMUMLINESPEED ) // if less than min assume 0
    {
        g_nZeroLineSpeedErrorCounter++;
        if( g_nZeroLineSpeedErrorCounter >=3 )
        {
            g_nZeroLineSpeedErrorCounter = 0;
            if( (g_bManualAutoFlag)&&((g_CalibrationData.m_nBlenderMode != MODE_BLENDINGONLY)&&(g_CalibrationData.m_nBlenderMode !=MODE_OFFLINEMODE)) )           // only valid is in Auto
            {
                if( (g_CalibrationData.m_nBlenderMode >= MODE_GPMGPMENTRY)&&(g_CalibrationData.m_nBlenderMode <= MODE_GP12SQFEET) ) // if any of the gpm modes are selected.                                
                {                
                    ToManual();     // switch blender to manual 
                    AddBlenderToManualZeroLinespeedEventToLog();
                    if( (g_arrnMBTable[BATCH_SUMMARY_PROCESS_ALARMS] & ZEROLSBIT) == 0 ) // Zero line speed bit not set???
                    {  //
                        PutAlarmTable( ZEROLSALARM,  0 );     // indicate zero line speed alarm, 
                    }

                       
                }
                    
            }
        }    
    }
    else
    {
        g_nZeroLineSpeedErrorCounter = 0;
        if(g_arrnMBTable[BATCH_SUMMARY_PROCESS_ALARMS] & ZEROLSBIT) // bit set???
        {  
            RemoveAlarmTable( ZEROLSALARM,  0 );     // Remove alarm
        }
 

    }
           
}




/*

;* CHECKLS      CHECKS FOR LINE SPEED OF < 3 m/MIN
;               ON ENTRY X HAS ADDRES OF LINE SPEED ABCD.EF m/Min
CHECKLS LDAB    #1     
        STAB    LSGE3FG                         ;~SET  >= 3 m/MIN FLAG AT START.
        LDD     0,X
        BNE     LSPDOK                          ;~EXIT IF > 2, I.E. >= 3.
        LDAA    2,X                             ; READ
        CMPA    #$25                            ; LESS THAN 0.25
        BHI     LSPDOK                          ;
        INC     LSPEEDERRORCTR                  ; INCREMENT ERROR COUNTER.
        LDAA    LSPEEDERRORCTR                  ;
        CMPA    #3
        BLO     CHCKLSX
        CLR     LSPEEDERRORCTR
        CLR     LSGE3FG                         ;~ELSE < 3 m/Min, CLEAR FLAG
        LDAA    MANATO                          ; 
        BEQ     CHCKLSX                         ; EXIT IF MANUAL.
        LDAA    BLENDERMODE                          
        BEQ    CHCKLSX                           ;~EXIT IF BLENDING ONLY.
        CMPA     #OFFLINEMODE                    ; OFFLINE MODE.
        BEQ    CHCKLSX                           ; NO NOT ONLINE.  
       
        LDAA    BLENDERMODE                        ; CHECK OPERATING MODE.
        CMPA    #GPMGPMENTRY                    ; G/M MODE.
        BLO     CHCKLSX                                ; NO       
        JSR     TOMANUAL                        ; SWITCH SYSTEM TO MANUAL
        LDAA    MBPROALRM+1
        ANDA    #ZEROLSBIT
        BNE     CHCKLSX                         ; EXIT IF ALREADY SET.
        LDAA    #ZEROLSALARM
        JSR     PTALTB                          ; ZERO LINE SPEED ALARM.
CHCKLSX RTS

*/




//////////////////////////////////////////////////////
//    CheckForLineSpeedChange(); //ASM = LS5CCH
//
// Check for line speed change.
// If line speed changes by a minimum amount and is 
// greater than a %, then the line speed change flag
// is set.
// A historic value is kept to compare the line speed 
//
// P.Smith                              1/3/5
//
//////////////////////////////////////////////////////

void CheckForLineSpeedChange( void )
{
float fDiffTemp,fPercentageTemp; 
    fPercentageTemp = 0.0f;   

    if(g_fLineSpeed5secHistory == 0.0f)  
    {
        g_fLineSpeed5secHistory = g_fLineSpeed5sec;         // transfer to history on line speed = 0                 
        g_nLineSpeedChangeCounter = 0;               
    }
    else
    {
        fDiffTemp = g_fLineSpeed5secHistory - g_fLineSpeed5sec; // difference in current and historic value
        if(fDiffTemp < 0.0)
        {
            fDiffTemp = -1 * fDiffTemp; // make the no positive
        }
        if(fDiffTemp > MINIMUMLINESPEEDCHANGE)  // Check for min line speed change  ASM = MINLSPDCHANGE
        {
           
            if(g_fLineSpeed5secHistory > MINIMUM_LENGTH)  // check for divide by zero 
            fPercentageTemp = 100 * fDiffTemp / g_fLineSpeed5secHistory;   // calculate % difference
            if(fPercentageTemp > MINIMUMLINESPEEDPERCHANGE)  // only accept a min change in line speed
            {
                g_nLineSpeedChangeCounter ++;           
                if(g_nLineSpeedChangeCounter >= LINESPEEDACTIONNO) // check if line speed has changed for n readings ?
                {
                    g_nLineSpeedChangeCounter = 0;               
                    g_bLineSpeedHasChanged = TRUE;          // indicate that line speed has changed                    
                    g_fLineSpeed5secHistory = g_fLineSpeed5sec;

                }
            }
            else
            {
                g_nLineSpeedChangeCounter = 0;               
            }
        }          

    }    
}




/*
;  LS5CCH  CHECKS IF 5 SEC LINE SPEED HAS CHANGED BY MORE THAN 5%
;          FROM THE VALUE USED IN CALCULATING SETPOINTS. 
;          IF CHANGE > 5% THEN LS5CFG, L.S.CHANGE FLAG) -> AA.
LS5CCH LDX    WMLS5S+2          ; CHECK HISTORY VALUE.
       BNE    LS5CH2            ;
       LDAA   WMLS5S+4
       BEQ    TRANTOHIS         ;  TRANSFER VALUE TO HISTORY.
LS5CH2 LDX    #WMLS5S+2         ;~WT/M LINE SPEED
       JSR    AMOVE
       LDX    #LSPD5S+2         ;~CURRENT VALUE
       JSR    CMOVE
       JSR    SUB               ;~GET DIFFERENCE.
       LDD      EREG+2          ;           
       CPD     #MINLSPDCHANGE            ;
       BLS      NOCHANGE        ; < 500 EXIT 
       LDX    #MATHCREG         ;~MOVE DIFF*10000 > CREG
       JSR    EMOVE
       LDX    #0
       STX    MATHCREG+3
       LDX    #WMLS5S+2
       JSR    AMOVE             ;~DIV. DIFF. BY WMLS5S
LS5CH4 JSR    DIV
       LDX    EREG
       BNE    LS5DIF            ;~NON-ZERO IMPLIES DIFF > 10000%
       LDAA   EREG+2
       BNE    LS5DIF            ;~AGAIN PERCENTAGE DIFF > 100%
       LDAA   EREG+3
JJJ002 CMPA   #MINLSPDPERCHG    ;
       BLS    NOCHANGE
LS5DIF INC    LSPEEDCHANGECTR   ; INCREMENT COUNTER.                      
       LDAA   LSPEEDCHANGECTR   ; 
       CMPA     #LSPEEDACTNO    ; ACTIVATION IS DETECTED.
       BLO    LS5CHX            ; IGNORE CHANGE FOR NOW.
       LDAA   #$AA
       STAA   LS5CFG            ;~SET FLAG TO SHOW CHANGE >= 5%
TRANTOHIS:
       CLR      LSPEEDCHANGECTR ; RESET COUNTER
       LDX    LSPD5S            ;~CURRENT VALUE
       STX    WMLS5S            ;
       LDX    LSPD5S+2
       STX    WMLS5S+2
       LDAA   LSPD5S+4
       STAA   WMLS5S+4
LS5CHX RTS
NOCHANGE CLR LSPEEDCHANGECTR   ; RESET COUNTER
        BRA     LS5CHX          ; EXIT.
;

*/



//////////////////////////////////////////////////////
//    BatchLengthCalculation(); //ASM = BATCHLENGTHCAL
//
//  Calculates batch length from pulses accumulated from batch to batch
//
// P.Smith                              2/3/5
//
//////////////////////////////////////////////////////

void BatchLengthCalculation( void )
{
float   fTemp;
    
    g_bLengthCalculationInProgress = TRUE;      // indicate that batch length calculation in progress
    if(g_CalibrationData.m_nPeripheralCardsPresent & SEIENBITPOS)   // sei enabled
    {
        CalculateAdditionalSEIPulses();        // calculate pulses to present time //ASM = CALCSEIADDPULSES
    }
    else
    {
// --todolp--     CalculateAdditionalOnBoardPulses(); //ASM = ALGPULSEACC        
    }
    fTemp = (float)g_lBatchPulsesAccumulator;
    g_fBatchLength = fTemp *  (g_CalibrationData.m_fLineSpeedConstant/1000000);  // batch length = pulses * constant    
    g_bLengthCalculationInProgress = FALSE;  // indicate that calculation is finished.
    g_lBatchPulsesAccumulator = 0; // reset pulse accumulator
}    



//////////////////////////////////////////////////////
//    CalculateAdditionalSEIPulses(); //ASM = CALCSEIADDPULSES
//
// Estimates the no of pulses to be added on to allow the exact no of pulses to be determined.
// The estimated pulses from the previous read is subtracted as this was used in calculating
// the no of pulses the previous batch.
// Estimated pulses = fraction counter * 
//
// P.Smith                              2/3/5
//
//////////////////////////////////////////////////////

void CalculateAdditionalSEIPulses( void )
{
int nOldEstimatedPulses;
long    lTemp;
    g_lOrigBatchPulsesAccumulator = g_lBatchPulsesAccumulator;
    nOldEstimatedPulses = g_nEstimatedAddedPulses;
    g_lBatchPulsesAccumulator = g_lBatchPulsesAccumulator - g_nEstimatedAddedPulses; // subtract off previous pulses
    lTemp = (long)g_nSEIPulsesPerSecond * (long)g_nLineSpeedFractionCounter;
    g_nEstimatedAddedPulses = (int) (lTemp / (BLENDER_PIT_FREQUENCY)); //pulses to be added on
    g_lBatchPulsesAccumulator = g_lBatchPulsesAccumulator + g_nEstimatedAddedPulses;    
}                                                           



//////////////////////////////////////////////////////
//    ReactToChangeInLineSpeed(); //ASM = LSPCHG
//
// If the line speed change flag has been set the new speed is set 
// volumetrically.
//
// P.Smith                              2/3/5
//
//////////////////////////////////////////////////////

void ReactToChangeInLineSpeed( void )
{
    if(g_bLineSpeedHasChanged)
    {    
        g_bLineSpeedHasChanged = FALSE;
        g_sOngoingHistoryEventsLogData.m_nLineSpeedChangesDetectedCtr++;
        if((g_CalibrationData.m_nControlMode & CONTROL_MODE_HAULOFF) == 0) // exit if haul off nbb--todolp-- check in voltage follower
        {
            if(g_fLineSpeed5sec > MINLINESPEEDFORREACTION)  // no change unless greater than limit
            {
                if(g_CalibrationData.m_bFastControlResponse)  // is the option enabled
                {                    
                    if((g_CalibrationData.m_nBlenderMode >= MODE_GPMGPMENTRY) && (g_CalibrationData.m_nBlenderMode <= MODE_GP12SQFEET))
                    {
                        if(g_bManualAutoFlag) // auto ?        
                        {
                            SetSpeedForSetPointChange();  // set point change initiated  ASM = SPCHANG       
                            SetExtruderControlStartup();
                            AddBlenderInstantVolCorrDueToLspeedChangeEventToLog();
                        }
                        else // manual
                        {
                            g_bRunEst = TRUE;  // estimation of kg/h and grams per meter
                            g_nEstCtrUse = 1;  // Use estimation for 1st batch after this
                        }
                    }                     
                }
            }
        }
    }
}                                                           




//////////////////////////////////////////////////////
//    SetSpeedForSetPointChange(); //ASM = SPCHANG
//
//
// P.Smith                              4/3/5
//
//////////////////////////////////////////////////////

void SetSpeedForSetPointChange( void )
{
    if( g_bManualAutoFlag ) // auto ?        
    {
        g_nEstCtrUse = 1;       // indicate that the next cycles will estimate the g/m / kg/h reading        
        SetVolumetricExtruderSpeed();
    }
}



//////////////////////////////////////////////////////
//    AccumulateFrontLength(); //ASM = ACCLT_F
//
// adds length to front roll, order, shift.
//
// P.Smith                              6/7/5
//
//////////////////////////////////////////////////////

void AccumulateFrontLength( void )
{
    float   fValue,fLength;
    union WordAndDWord uValue;

    fLength = 0.0f;
// merge ASM TOTFLSP in line.
   g_lFrontTotalLengthPulses += g_lFrontRollPulsesAccumulator ;  
   g_lFrontRollPulsesAccumulator = 0; // set pulse accumulator to 0.

   if(g_CalibrationData.m_nLineSpeedSignalType == PULSES_LINE_SPEED)   
   {
       fValue = (float) g_lFrontTotalLengthPulses;  // convert pulses to float
       fLength = fValue * (g_CalibrationData.m_fLineSpeedConstant / 1000000.0f);
       g_lFrontTotalLengthPulses = 0;
   }
   else
   if(g_CalibrationData.m_nLineSpeedSignalType == COMMUNICATION_LINE_SPEED)   //nbb--modbus download of signal type.
   {
      fLength = g_fCommsLineSpeedFrontLength;
      g_fCommsLineSpeedFrontLength = 0.0f;
   }
   g_FrontRoll.m_fLength += fLength; // add on length
   
   g_Order.m_fLength += fLength; // order
   g_ProcessData.m_fOrderLength = g_Order.m_fLength; // save order length to non volatile memory
   if(g_nStorageInvervalCtr == 0)
   {
       g_bSaveAllProcessDataToEEprom = TRUE;
       g_bAllowProcesDataSaveToEEPROM = TRUE;   
   }
   g_CurrentShiftFront.m_fLength += fLength; // shift
   g_OngoingShift.m_fLength += fLength; // ongoing shift length
   
   g_fLongTermResettableLengthCounter += fLength;
   g_fLongTermLengthCounter += fLength;
   
   
// copy front roll length to modbus table.

   g_arrnMBTable[BATCH_SUMMARY_FRONT_ROLL_LENGTH] =  (int)g_FrontRoll.m_fLength; //ASM = CPYFRLMB 
     
// copy order length to modbus table.

        uValue.lValue = (long)g_Order.m_fLength;  //nbb--todo-- if optimisation copy downloaded 
        g_arrnMBTable[BATCH_SUMMARY_ORDER_LENGTH] = uValue.nValue[0];
        g_arrnMBTable[BATCH_SUMMARY_ORDER_LENGTH+1] = uValue.nValue[1];   // ASM = CPYOLMB
 
// --review-- done already not necessary.

}


//////////////////////////////////////////////////////
//    AccumulateBackLength(); //ASM = ACCLT_B
//
// adds length to back roll.
//
// P.Smith                              6/7/5
//
//////////////////////////////////////////////////////

void AccumulateBackLength( void )
{
    float   fValue,fLength;
    fLength = 0.0f;

    if(g_CurrentRecipe.m_bSheetTube)  // --review-- is this check the right way around
    {

// merge ASM TOTBLSP in line.
        g_lBackTotalLengthPulses += g_lBackRollPulsesAccumulator ;  
        g_lBackRollPulsesAccumulator = 0; // set pulse accumulator to 0.    

   if(g_CalibrationData.m_nLineSpeedSignalType == PULSES_LINE_SPEED)   //nbb--modbus download of signal type.
   {
        fValue = (float) g_lBackTotalLengthPulses;  // convert pulses to float
        fLength = fValue * (g_CalibrationData.m_fLineSpeedConstant / 1000000.0f);
        g_lBackTotalLengthPulses = 0;
   }
   else 
   if(g_CalibrationData.m_nLineSpeedSignalType == COMMUNICATION_LINE_SPEED)   //nbb--modbus download of signal type.
   {
      fLength = g_fCommsLineSpeedFrontLength;
      g_fCommsLineSpeedBackLength = 0.0f;
   }

// update roll length counter

        g_BackRoll.m_fLength += fLength; // add on length

   
// copy back roll length to modbus table.

        g_arrnMBTable[BATCH_SUMMARY_BACK_ROLL_LENGTH] =  (int)g_BackRoll.m_fLength; //ASM = CPYBRLMB 
     
// copy order length to modbus table.

//        uValue.lValue = (long)g_Order.m_fLength;
//        g_arrnMBTable[BATCH_SUMMARY_ORDER_LENGTH] = uValue.nValue[0];
//        g_arrnMBTable[BATCH_SUMMARY_ORDER_LENGTH+1] = uValue.nValue[1];   // ASM = CPYOLMB
    }    
}


/*
LSPCHG  LDAA    LS5CFG          ; CHECK FOR CHANGE IN LINE SPEED
        BEQ     XITNCH          ; NO
        TST     MANATO          ; AUTO
        BEQ     CHKFCR          ; MANUAL EXIT.
        LDAA    CRLMODE         ; MODE OF OPERATION.
        CMPA    #HAULOFF        ;
        BEQ     XITNCH     
        LDAA    #1
        STAA    STRUPF          ; SET SYSTEM TO START UP MODE.
        CLR     LS5CFG          ; RESET
        LDD     LSPD5S+2
        CPD     #ZEROLINESPEEDLIMIT
        BLS     XITNCH          ; EXIT IF LINE SPEED        
CHKFCR  TST     FSTCTRL         ; FAST CONTROL RESPONSE
        BNE     XITNCH
        LDAA    BLENDERMODE     ; CHECK THE SET POINT MODE IN OPERATION
        CMPA    #GPMGPMENTRY    ; IS THIS GRAM PER METER MODE ?
        BLO     XITNCH
SPCHANG TST     MANATO          ; AUTO
        BEQ     XITNCH          ; MANUAL EXIT.
        LDAA    #1
        STAA    ESTCTRUSE       ; USE ESTIMATED VALUE FOR THE NEXT CORRECTION.
        JSR     NEWVOLSET
XITNCH  RTS



*/
/*
;
CALCSEIADDPULSES:
        LDX     #BATCHPULSEACC   ; PULSES FOR ALGORITHM.
        LDD     ESTADDSEIPULSES  ;
        JSR     SUBEFMX          ; ADDITION OF PULSES.
        LDD     LSPDFRACTIONCTR        ; LSPD FRACTION OF 1 SECOND
        JSR     HEXBCD2         ;
        JSR     CLRAC           
        STE     MATHCREG+2          ;
        LDD     SEIPULSEPSEC        ; PULSES PER SECOND.
        JSR     HEXBCD2             ; TO BCD
        STE     AREG+3              ;
        JSR     MUL                 ; MULTIPLY.
        LDX     #MATHCREG
        JSR     IFEREG              ; TRANSFER FROM E REGISTER.
        JSR     CLAREG              ; A = 0      
        LDD     #PITINDEXBCD
        STD     AREG+3          ;
        JSR     DIV             ; C/A FRACTION OF SECOND IS CACULATED XX.YY
        LDAA    #2
        LDX     #EREG
        JSR     GPRND1            ;~ROUND TO 3 DIGITS
        LDD     EREG+2
        JSR     BCDHEX2
        STD     ESTADDSEIPULSES  ; STORE ADDITIONAL PULSES.
        LDD     SEIPULSEPSEC
        SUBD    ESTADDSEIPULSES  ; PULSES LEFT IN SECOND.
        STD     ESTSUBSEIPULSES  ; STORE PULSES LEFT.
        LDX     #BATCHPULSEACC   ; PULSES FOR ALGORITHM.
        LDE     ESTADDSEIPULSES
        JSR     ADDETOX          ; ADDITION OF PULSES.
        RTS    

SUBEFMX:
        CLRE
        LDE     1,X             ; LOAD VALUE.
        SDE                    ; SUBTRACT PULSES
        STE     1,X             ;
        LDAA    0,X
        SBCA    #0
        STAA    0,X
        RTS



*/
/*

; ROUTINE TO ACCUMULATE PULSES FROM SEI.

ACCSEIPULSES:
        TST     LENGTHCALCINP   ; LENGTH CALCULATION IN PROGRESS
        LBNE    XITSEIA         ; EXIT IF YES 
        TST     SEIPULSESREADY  ; SEI PULSES AVAILABLE.
        BEQ     XITSEIA         ; NO.
        CLR     SEIPULSESREADY  ; RESET FLAG.
        TST     SEIFIRSTPULSER  ; FIRST READ AFTER RESET ?
        BEQ     NORMRD          ; NORMAL READ.
        CLR     SEIFIRSTPULSER  ; RESET FLAG.
        LDD     SEIPULSEACC     ; READ SEI PULSES.
        STD     PREVIOUSPCLK    ; STORE.      
        LDD     SEIPULSEACC+2   ; READ SEI PULSES.
        STD     PREVIOUSPCLK+2  ; STORE.      
        RTS
NORMRD  LDD     SEIPULSEACC     ;
        STD     CURRENTPCLK     ;
        LDD     SEIPULSEACC+2   ;
        STD     CURRENTPCLK+2   ;
        LDD     CURRENTPCLK      ;
        LDE     CURRENTPCLK+2    ; 
        SUBE    PREVIOUSPCLK+2 ;
        SBCD    PREVIOUSPCLK  ;         
        STE     DIFFINPCLK
        JSR     ADDTOLSPDCTRS   ; ADD TO LINE SPEED COUNTERS.
        LDD     CURRENTPCLK      ;
        LDE     CURRENTPCLK+2    ; 
        STD     PREVIOUSPCLK     ;      CURRENT TO PREVIOUS.
        STE     PREVIOUSPCLK+2   ;        
XITSEIA RTS



;
;  ROUTINES TO READ THE COUNTERS WITH LINE SPEED INPUTS
;
ONESECPULACC:
        LDAA    EIOMITTED       ; CHECK REMOTE MODULES
        ANDA    #SEIENBITPOS    ; SEI ENABLED BIT POSITION.          
        BNE     EXITINP         ; YES
        TST     LENGTHCALCINP   ; LENGTH CALCULATION IN PROGRESS
        BRA      EXITINP        ; EXIT IF YES 
        LBNE     EXITINP        ; EXIT IF YES 
ALGPULSEACC:
        LDAB   #HARDWAREBANK                   ; HARDWARE BANK
        TBZK                                   ; STORE BANK NO          
        LDZ     #TCNT                    ; READ COUNTER VALUE.
        LDD     0,Z
;        LDD     TCNT            ; READ COUNTER
        STD     CURRENTPCLK     ; CURRENT VALUE.
        PSHM    D               ; SAVE CURRENT VALUE.
        CPD     PREVIOUSPCLK    ; COMPARE TO PREVIOUS READING.
        BLO     HASOVR          ; OVERFLOWED 
        SUBD    PREVIOUSPCLK    ; CALCULATE DIFFERENCE.
        PULM    E               ; RECOVER CURRENT VALUE.
        STE     PREVIOUSPCLK    ; STORE.
        BRA     ADDAXT          ; EXIT.
HASOVR  LDD     #$FFFF          ; MAX VALUE.
        SUBD    PREVIOUSPCLK    ; FFFF - PREVIOUS VALUE
        ADDD    CURRENTPCLK     ; ADD CURRENT VALUE.
        PULM    E               ;
ADDAXT  STE     PREVIOUSPCLK    ;
        STD     DIFFINPCLK
        JSR     ADDTOLSPDCTRS   ; ADD TO LINE SPEED COUNTERS.
EXITINP RTS                

; ADD TO LINE SPEED COUNTERS.

ADDTOLSPDCTRS:                
        LDX     #BATCHPULSEACC  ; PULSES FOR ALGORITHM.
        JSR     ADDLSPDPULSES   ; ADDITION OF PULSES.
        LDX     #LSPDPULACC5SEC ; 5 SECOND COUNTER 
        LDAA    EIOMITTED       ; SEI OMITTED ?
        ANDA    #SEIENBITPOS    ; SEI ENABLED BIT POSITION.          
        BNE     ISSEI           ; THERE IS AN SEI 
;        LDE     #0              ; MAKE DIFFERENCE 0.
        LDE     DIFFINPCLK      ; READ DIFFERENCE 
        BRA     ADDPUL
ISSEI   LDE     SEIPULSEPSEC
ADDPUL  CLRA                    ; RESET CARRY.
        ADDE     LSPDPULACC5SEC+1     
        STE     LSPDPULACC5SEC+1  ;
        ADCA    #0    
        STAA    LSPDPULACC5SEC
        LDX     #LSPDPULACCLSCK; 30 SECOND COUNTER 
        JSR     ADDLSPDPULSES   ;
        LDX     #LSPDPULACC5SECF; FRONT PULSE COUNTER 
        JSR     ADDLSPDPULSES   ;
        LDX     #LSPDPULACC5SECB; BACK PULSE COUNTER 
        JSR     ADDLSPDPULSES   ;
        LDX     #HOURLYPULACC     ; BACK PULSE COUNTER 
        JSR     ADDLSPDPULSES   ;
        TST     LENGTHCALCINP   ; ALGORITHM CALCULATION.
        BNE     XITTOT          ; EXIT TOTALISER
        INC     LSPDPULCTR5SEC  ; 5 SECOND COUNTER.
        LDAA    LSPDPULCTR5SEC  ;
        CMPA    #5              ;
        BLO     NOT5ACC         ; NOT FINISHED.
        CLR     LSPDPULCTR5SEC  ;
        LDD     LSPDPULACC5SEC
        STD     LSPDPULTOT5SEC
        LDAA    LSPDPULACC5SEC+2
        STAA    LSPDPULTOT5SEC+2
        CLRW    LSPDPULACC5SEC    ; CLEAR ONGOING COUNTER   
        CLR     LSPDPULACC5SEC+2
        INC     LS5SFG          ; SET LINE SPEED FLAG.
NOT5ACC         
        INC     LSPDPULCTRLSCK  ; 5 SECOND COUNTER.
        LDAA    LSPDPULCTRLSCK  ;
        CMPA    #5              ;
        BLO     NOT30ACC         ; NOT FINISHED.
        CLR     LSPDPULCTRLSCK  ;
        LDD     LSPDPULACCLSCK
        STD     LSPDPULTOTLSCK
        LDAA    LSPDPULACCLSCK+2
        STAA    LSPDPULTOTLSCK+2
        CLRW    LSPDPULACCLSCK    ; CLEAR ONGOING COUNTER   
        CLR     LSPDPULACCLSCK+2
        INC     LS2CAL          ; INDICATION FOR 30 SECOND CALCULATION.
NOT30ACC:
XITTOT  RTS        
        
        
; ADD PULSES IN DIFFINPCLK TO 3 BYTE COUNTER IN X.
;        
        
ADDLSPDPULSES:        
        LDE     DIFFINPCLK      ; READ PULSES
ADDETOX CLRA    
        ADDE    1,X             ; ADD PULSES.
        STE     1,X             ; RESTORE.
        ADCA    0,X             ; MSB ADDED.
        STAA    0,X             ; STORE.
        RTS
        
SUBEFMX:
        CLRE
        LDE     1,X             ; LOAD VALUE.
        SDE                    ; SUBTRACT PULSES
        STE     1,X             ;
        LDAA    0,X
        SBCA    #0
        STAA    0,X
        RTS


; CALCULATION OF BATCH LENGTH FROM PULSES ACCUMULATED.
; BATCHPULSEACC * LINE SPEED CALIBRATION CONSTANT.

BATCHLENGTHCAL:
        LDAA    #$0F
        STAA    LENGTHCALCINP   ; INDICATE LENGTH CACULATION IN PROGRESS
        LDAA    EIOMITTED       ; SEI PRESENT.
        ANDA    #SEIENBITPOS    ; SEI ENABLED BIT POSITION.          
        BNE     EINTOM          ; NO.
        JSR     ALGPULSEACC     ; READ UPDATED COUNT
EINTOM  JSR     CALCSEIADDPULSES; CALCULATE ADDITIONAL PULSES
        LDX     #LSC1           ; LENGTH CALIBRATION FACTOR
        JSR     AMOVE
        LDX     #BATCHPULSEACC  ;
        JSR     HEXBCD3X        ; HEX TO BCD RESULT IN D,E
        STD     MATHCREG+1
        STE     MATHCREG+3      ; STORE RESULT.
        CLRW    BATCHPULSEACC
        CLR     BATCHPULSEACC+2 ; CLEAR PULSE ACC

        JSR     MUL             ; (PULSES * LENGTH CONSTANT) 
        CLR     LENGTHCALCINP   ; CLEAR "LENGTH CALCULATION IN PROGRESS" FLAG
        LDX    #EREG
        LDAA   #2
        JSR    GPRND1            ;~ROUND TO 4 PLACES OF DECIMAL
        LDX    #EREG
        LDAB   #5
        LDAA   #2
        JSR    GPSFTR            ;~RESULT IS M TO 4 DECIMAL PLACES
        LDX     #BATCHLENGTH
        JSR     IFEREG          ; STORE BATCH LENGTH
        RTS




;
;  LPEED5 IS A SUBR TO CALC LINE SPEED (M/MIN)
;         FROM 5 SECONDS PULSE COUNT.
LPEED5 LDX    #LSPDPULTOT5SEC
       JSR      HEXBCD3X        ; CONVERT TO BCD VALUE.
       CLR      MATHCREG        ; 
       STD      MATHCREG+1
       STE      MATHCREG+3      ;
             
       JSR    CLAREG
       LDAB   #$12              ;~12 DECIMAL
       STAB   AREG+4
       JSR    MUL               ;~NOW EREG HAS COUNTS/PULSES FOR 60S (1 MIN)
       LDX    #AREG
       JSR    IFEREG            ;~MOVE  TO AREG
       LDX    #LSC1             ;~LINE SPEEED CONST
       JSR    CMOVE             ;~MOVE CONST TO CREG
       JSR    MUL
       LDX    #EREG
       LDAA   #4
       JSR    GPRND1            ;~ROUND TO 4 PLACES
       LDX    EREG
       LDAA   EREG+2
       STX    LINESPD           ;~VALUE (INTEGER) FOR 9000
       STX    LSPD5S+2
       STAA   LSPD5S+4                  ;~MOVE RESULT TO 5 SEC LINE SPEED
       LDD      LSPD5S+2        ; CHECK LINE SPEED.
       CPD      #$0061          ; AT CORRECT SPEED
       BEQ      OK
       NOP       
OK     JSR    CONVLSPD          ; CONVERT TO m/min IF IN IMPERIAL UNITS
       JSR    TOTFLSP           ;~TOTALISE LINE PULSES (FRONT)
       JSR    TOTBLSP           ;~TOTALISE LINE SPD PULSES (BACK)
       LDX    #LSPD5S+2           ;~ADDRESS OF LINE SPEED
       JSR    CHECKLS         ; CHECK FOR ZERO LINE SPEED.
       JSR    LS5CCH            ;~CHECK FOR CHANGE IN 5 SEC L.S.
       JSR    CPYLSPMB        ; COPY LINE SPEED TO MODBUS TABLE.
       TST    CONESTIMATION
       BEQ      XITSPD          ; EXIT.
       DEC    CONESTIMATION
       LDAA     #1
       STAA     RUNEST          ; RUN ESTIMATION.
XITSPD RTS
        
        
;  TOTFLSP  IS A SUBR. TO TOTALISE FRONT LINE SPEED PULSES.  SO THE
;          LENGTH CAN BE CALCULATED.
;          ROUTINE IS CALLED ON 5 SECS AND BY ACCLT_F SUBR.

TOTFLSP LDX    #LSPDPULACC5SECF  ; FRONT PULSE ACCUMULATOR                 
        JSR     HEXBCD3X         ; TO BCD
        CLR     MATHCREG         ;
        STD     MATHCREG+1
        STE     MATHCREG+3
        CLRW    LSPDPULACC5SECF  ; RESET COUNTER.
        CLR     LSPDPULACC5SECF+2        
        LDX    #FLSPREG          ;~ACCUMULATOR FOR TOTAL
        JSR    ITAREG            ;~> AREG
        JSR    ADD
        LDX    #FLSPREG                  ;~MOVE RESULT BACK TO ACCUM.
        JSR    IFEREG
        RTS
;
;  TOTBLSP  IS A SUBR. TO TOTALISE BACK LINE SPEED PULSES.  SO THE
;          LENGTH CAN BE CALCULATED.
;          ROUTINE IS CALLED ON 5 SECS AND BY ACCLT_B SUBR.

TOTBLSP LDX    #LSPDPULACC5SECB  ; BACK  PULSE ACCUMULATOR                 
        JSR     HEXBCD3X         ; TO BCD
        CLR     MATHCREG         ;
        STD     MATHCREG+1       
        STE     MATHCREG+3
        CLRW    LSPDPULACC5SECB  ; RESET COUNTER.
        CLR     LSPDPULACC5SECB+2        
        LDX    #BLSPREG                  ;~ACCUMULATOR FOR TOTAL
        JSR    ITAREG            ;~> AREG
        JSR    ADD
        LDX    #BLSPREG                  ;~MOVE RESULT BACK TO ACCUM.
        JSR    IFEREG
        RTS
        
        
        
; CALCULATION OF HOURLY LENGTH.
;
CALCHRLENGTH:
       LDX    #HOURLYPULACC     ;~FRONT L.S. PULSES TOTALISER
       JSR     HEXBCD3X        ; HEX TO BCD RESULT IN D,E
       STD     MATHCREG+1
       STE     MATHCREG+3      ; STORE RESULT.
       CLR     MATHCREG         ; 
       LDX    #LSC1             ;~LINE SPEED CONST
       JSR    AMOVE
       JSR    MUL               ;~PULSES * LSC1 =-> LENGTH
       LDX    #EREG
       LDAA   #2
       JSR    GPRND1            ;~ROUND TO 4 PLACES OF DECIMAL
       LDX    #EREG
       LDAB   #5
       LDAA   #2
       JSR    GPSFTR            ;~RESULT IS M TO 4 DECIMAL PLACES
       LDX    #HRLTACC          ; HOURLY LENGTH ACCUMULATED.
       JSR    IFEREG            ;~MOVE LENGTH INCREMENT TO AREG
       CLRW   HOURLYPULACC
       CLR    HOURLYPULACC+2    ;~CLEAR TOTAL PULSES  (FRONT) COUNTER
       RTS

        
        
; PULSE SUBROUTINE *
; OUTPUT A 2 MS PERIOD PULSE ON O/P 4 OF VIA
;PULSELSPEED:
;       LDD     PASWRD          ;
;       CPD     #$0065          ; SPECIFIC PASSWORD ?
;       BNE     NODIAG          ; NO CONTROL.
;       LDAA    PASWRD+2
;       CMPA    #$53            ;
;       BNE     NODIAG          ; ALLOW CONTROL.        
;       PSHB
;       JSR      SSROPLOW
;       JSR      DLY05US         ; CALL DELAY
;       JSR      SSROPHIGH       
;       PULB
;NODIAG RTS



// from sblspd38.asm        

;
;  ACCLT_F  ACCUMULATES LENGTH (FRONT) IN m
;            CALCULATES LENGTH INCREMENT FROM ACCUMULATED PULSES
;            SINCE PREV. CALLING OF PROG. RESETS ACCUMULATED PULSES
ACCLT_F JSR    TOTFLSP                  ;~FIRST TOTALISE LINE SPEED PULSES
       LDX    #LSC1             ;~LINE SPEED CONST
       JSR    AMOVE
       LDX    #FLSPREG                  ;~FRONT L.S. PULSES TOTALISER
       JSR    ITCREG
       LDX    #FLSPREG
       JSR    CLRNM1            ;~CLEAR PULSE ACCUMULATOR
;
       JSR    MUL               ;~PULSES * LSC1 =-> LENGTH
       LDX    #EREG
       LDAA   #2
       JSR    GPRND1            ;~ROUND TO 4 PLACES OF DECIMAL
       LDX    #EREG
       LDAB   #5
       LDAA   #2
       JSR    GPSFTR            ;~RESULT IS M TO 4 DECIMAL PLACES
       LDX    #AREG
       JSR    IFEREG            ;~MOVE LENGTH INCREMENT TO AREG
;
        LDX    #FRLTACC                 ;~FRONT ROLL LENGTH ACCUMULATOR IN M
        JSR    ITCREG
        JSR    ADD
        LDX    #FRLTACC                 ;~RESTORE NEW TOTAL LENGTH
        JSR    IFEREG
;        
        LDX    #OLTACC                  ;~ORDER LENGTH ACCUMULATOR IN M
        JSR    ITCREG
        JSR    ADD
        LDX    #OLTACC                  ;~RESTORE NEW TOTAL LENGTH
        JSR    IFEREG
        
        LDX    #CURSHIFTLENGTH          ;~ORDER LENGTH ACCUMULATOR IN M
        JSR    ITCREG
        JSR    ADD
        LDX    #CURSHIFTLENGTH          ;~RESTORE NEW TOTAL LENGTH
        JSR    IFEREG

        LDX    #SHFTLTACC              ;~SHIFT LENGTH ACCUMULATOR IN M
        JSR    ITCREG
        JSR    ADD
        LDX    #SHFTLTACC                  ;~RESTORE NEW TOTAL LENGTH
        JSR    IFEREG

;
        JSR     CPYFRLMB        ; COPY LENGTHS INTO MODBUS TABLE
        JSR     CPYOLMB

;      JSR    CALCTTFRC        ; CALC. TIME TO FRONT R.C.
       RTS
;
;  ACCLT_B  ACCUMULATES LENGTH (BACK) IN m
;            CALCULATES LENGTH INCREMENT FROM ACCUMULATED PULSES
;            SINCE PREV. CALLING OF PROG. RESETS ACCUMULATED PULSES
ACCLT_B:
       TST    SHTTUBE           ; ARE WE MAKING TUBE(00)?
       BEQ    ACCL_BX           ; IF SO ASSUME NO BACK ROLL.
       
       JSR    TOTBLSP           ;~FIRST TOTALISE LINE SPEED PULSES
       LDX    #LSC1             ;~LINE SPEED CONST
       JSR    AMOVE
       LDX    #BLSPREG                  ;~BACK L.S. PULSES TOTALISER
       JSR    ITCREG
       LDX    #BLSPREG
       JSR    CLRNM1            ;~CLEAR PULSE ACCUMULATOR
;
       JSR    MUL               ;~PULSES * LSC1 =-> LENGTH
       LDX    #EREG
       LDAA   #2
       JSR    GPRND1            ;~ROUND TO 4 PLACES OF DECIMAL
       LDX    #EREG
       LDAB   #5
       LDAA   #2
       JSR    GPSFTR            ;~RESULT IS M TO 4 DECIMAL PLACES
       LDX    #AREG
       JSR    IFEREG            ;~MOVE LENGTH INCREMENT TO AREG
;
        LDX    #BRLTACC         ;~BACK ROLL LENGTH ACCUMULATOR IN M
        JSR    ITCREG
        JSR    ADD
        LDX    #BRLTACC         ;~RESTORE NEW TOTAL LENGTH
        JSR    IFEREG
;* add length increment to order total also
        bra    ACCL_BY          ; DO NOT INCLUDE BACK LT (M.P. 4/July/96)
        
        LDX    #OLTACC          ;~ORDER LENGTH ACCUMULATOR IN M
        JSR    ITCREG
        JSR    ADD
        LDX    #OLTACC          ;~RESTORE NEW TOTAL LENGTH
        JSR    IFEREG
ACCL_BY:
;       JSR    CALCTTBRC        ; CALC. TIME TO BACK R.C.
;
        JSR     CPYBRLMB        ; COPY LENGTHS INTO MODBUS TABLE
        JSR     CPYOLMB

ACCL_BX RTS


;* COPY F. ROLL LENGTH  INTO MODBUS TABLE PROD. SUMMMARY DATA 
CPYFRLMB:
        TST     PROTOCOL
        BPL     CPYFRLMX
        
        LDX     #FRLTACC
        JSR     BCDHEX3X        ; CONVERT 3 BYTES BCD  TO HEX        
        STE     MBFRL           ; MAX VALUE EXPECTED = 65535 m
CPYFRLMX RTS        

;* COPY O. ROLL LENGTH  INTO MODBUS TABLE PROD. SUMMMARY DATA 
CPYOLMB:
        TST     PROTOCOL
        BPL     CPYOLMX
        
        LDX     #OLTACC
        JSR     BCDHEX3X         ; CONVERT 3 BYTES BCD  TO HEX 
        CLRA
        STD     MBORDL       
        STE     MBORDL+2           ; MAX VALUE EXPECTED = 999999 m
CPYOLMX RTS        

        
        
*/       


//////////////////////////////////////////////////////
// IntegrateCommsLineSpeed( void )         from ASM = INTEGLSPD
//
// integrates length for line speed over comms.
// Integration is run once per second.
// Integrated line speed is the line speed divided by 60 
//
//////////////////////////////////////////////////////

void IntegrateCommsLineSpeed( void )
{
    float   fLength;
    if(g_CalibrationData.m_nLineSpeedSignalType == COMMUNICATION_LINE_SPEED)   //nbb--modbus download of signal type.
    {
       fLength = g_fCommunicationsLineSpeed / 60.0f;
       g_fCommsLineSpeedFrontLength += fLength;
       g_fCommsLineSpeedBackLength += fLength;
    }
}





 
        
               
                             




