//////////////////////////////////////////////////////
// PITWeigh.c
//
// Group of functions that related to the weighing/cycling on the PIT.
// a batch blender system.
// M.McKiernan                          17-06-2004
// First Pass
// M.McKiernan                          03-09-2004
// g_cLFillStatCtr[nComponent-1] = LFILLONTIME10HZ; -> g_nLFillStatCtr[nComponent-1] = LFILLONTIME10HZ;
// M.McKiernan                          14-09-2004
// Changed how a regrind is detected, use g_CurrentRecipe.m_nRegrindComponent
// M.McKiernan                          23-09-2004
// Renamed nRate to nStage in AddRawForAutoCycyle
// M.McKiernan/P.Smith              28-09-2004
// Edited AddRawForAutoCycle - if(g_CalibrationData.m_bTopUpMode == ENABLED && g_nPITCom == TOPUP_COMPONENT)...
// No longer initialise PITCom in ByTime().
//
// M.McKiernan                          13-12-2004
// Activated RemoveAlarmTable() and PutAlarmTable.
//
// M.McKiernan                          14-01-2005
// Diagnostic for measuring CheckWeight length) - has been commented out. - look for GPT_TCNT
//
// P.Smith                              4/3/5
// Remove ResetWeightRoundRobin - unused
//
//
/*
    P.smith                                         21/10/05          ;
;   check for m50 type in getting the latency.                      ;
;                                                                       ;
;   P.Smith                                                         ;
;   ensure that if the regrind component is retrying that the       ;
;   blender does not clear the fill by weight status and move on.   ;
;   the leave fill by weight bit is left set if the component is    ;
;   regrind and fill by weight is set. This ensures that the fill by;
;   weight status is not reset.                                     ;
;                                                                       ;
;   P.Smith                                         21/10/05         ;
;   check for seqable+1 if the regrind bit is set in seqtable       ;
;   this ensure that the calcrouter stays at calcmpntar when filling;
;   the regrind component.                                          ;
;                                                                       ;
;   P.Smith                                     21/10/05              ;
;   remove integrated loading vacload check.                        ;
;
;                                                                       ;
;   P.Smith                                     7/11/05              ;
;   Check for g_bOutputDiagMode in CloseAll, do not run if system in diagnostics mode
;
;   P.Smith                                     8/11/05
;   check g_bAutoCycleTestFlag properly, allows autocycle process to be interrupted.
;
;   P.Smith                                     10/1/06
;   Corrected warnings
;   g_bFillAlarm removed
;   double bracket if((g_CalibrationData.m_bTopUpMode == ENABLED) && (g_nPITCom == TOPUP_COMPONENT))                           // ASM = TOPUPF
;   comment g_nChkWtStart = GPT_TCNT;           // current TCNT time

//
// P.Smith                      20/2/06
// first pass at netburner hardware conversion.
// Bool -> BOOL
// included  <basictypes.h>
//
// P.Smith                      22/2/06
// ExtruderStallCheck();   // has extruder stalled ?
//
// P.Smith                      27/2/06
// PutAlarmTable( NOFILLALARM,  g_nPITCom );       // indicate alarm, Component no
// RemoveAlarmTable( NOFILLALARM,  g_nPITCom );        // clear alarm, Component no
//
// P.Smith                      20/6/06
// use calculated latency from the entered value.
//
// P.Smith                      30/6/06
// Read timer 2 to measure the time that the valve is open for.
// subtract 0xFFFFFFFF when counter rolls over.
//
// P.Smith                      16/11/06
// remove unused commented out code.
//
// P.Smith                      23/1/07
// Remove GenerateBeep
*/
//
// P.Smith                          5/2/07
// Remove iprintf
//
// P.Smith                          11/5/07
// g_bFlowRateChange checked to simulate different flow rate.
//
// P.Smith                          28/5/07
// Remove printf
//
// P.Smith                          14/6/07
// added flow rate change simulation for component 9.
//
// P.Smith                          22/6/07
// correct blender types.
//
//
// P.Smith                                     22/1/08             ;
// Store fill by weight to g_bComponentHasFilledByWeight (asm compnfbw)
//
// P.Smith                          30/1/08
// correct compiler warnings
// include "rta_system.h" for definitions on/ off etc
//
// P.Smith                          20/3/08
// in GetLatency remvoe chec for password 6554 and address 0
//
// P.Smith                          13/6/08
// in GetLatency, if TSM 350 or 650, then assume component 5 is a venturi feeder
// and set the latency to 0
//
// P.Smith                          23/6/08
// check for g_CalibrationData.m_bVacloading, if true then set vacloading sequence
//
// P.Smith                          26/6/08
// Correct set of FBW indication g_bComponentHasFilledByWeight, should be using
// g_nPITCom instead of nIndex when storing the component that is filling by weight
//
// P.Smith                          14/8/08
// if in top up and the component no is 8, then latency is used irrespective of the
// blender type that is being selected.
//
// P.Smith                          11/9/08
// remove vacuum loading from here, it now is part of the settling.
// change auto cycler to simulate a flow rate change on the top up component.
//
// P.Smith                          11/9/08
// flow rate change now on component 2.
//
// P.Smith                          26/9/08
// checked g_bFlowRateChange in new format.
//
// P.Smith                          07/11/08
// in CheckComponentUsed return true all the time, this will allow the weight in the
// background to be checked even if it is filling by time.
//
// P.Smith                          3/12/08
// increment ongoing activations counter for history log
//
// P.Smith                          22/12/08
// Correct compiler warning.
//
// P.Smith                          25/6/09
// if pulsing, set g_bActivateLatency to false and nLatency to 0.
// this stops the latency counter from running.
//
// P.Smith                          25/8/09
// copy sequence table into to check fill by weight in top up mode.
//
// P.Smith                          27/11/09
// added TOPUPCOMPONENT instead of specific component no.
// the top up has now been changed to change to the topped up component if
// top up sequencing is set to topping up colour.
// the component no is changed in cmpnon, cmpnoff,
//
// P.Smith                          15/12/09
// correct compiler warning by removing  unsigned char   cData;
//
// P.Smith                          19/3/10
// store the top up component in g_nTopUpComponent so that the correct weight can
// be stored on the logs
//
// P.Smith                          26/4/10
// increment component activations counter to show how many activations are
// initiated.
//*****************************************************************************
// INCLUDES
//*****************************************************************************
//nbb #include <hc16.h>
#include <Stdio.h>
//nbb #include <StdLib.h>

//nbb #include "Hardware.h"
#include "InitialiseHardware.h"
#include "SerialStuff.h"
#include "General.h"
#include "ConversionFactors.h"

#include "BatchCalibrationFunctions.h"
#include "BatchMBIndices.h"
#include "ConfigurationFunctions.h"
#include "Foreground.h"
#include "16R40C.h"
#include    "PITWeigh.h"
#include    "BatVars.h"
#include "SetpointFormat.h"
#include "Alarms.h"
#include <basictypes.h>
#include    "BatVars2.h"
#include "rta_system.h"
#include "Historylog.h"



// Locally declared global variables
// Externally declared global variables
// Data
extern CalDataStruct    g_CalibrationData;
extern  structSetpointData  g_CurrentRecipe;  //Current setpoints
//extern float  g_fWeightAtoDCounts;

extern  int g_nKeyCode;
extern  int g_nLanguage;
extern  unsigned char g_cBlenderNetworkAddress;
extern  int g_nGeneralSecondTimer;
extern  structMeasurementUnits  g_MeasurementUnits;
//extern    BOOL  g_bAutoCycleFlag;
// Text
//extern    char    *strBatchSpeedCalibrationMenuText2[];

//bb
extern  int g_nDisplayScreen;
//extern    long    g_lRawAtDCounts;
//extern    long  g_lOneSecondWeightAverage;

extern   BOOL  g_bOneSecondWeightAverageAvailable;
unsigned int g_nTopUpComponent = 0;


//////////////////////////////////////////////////////
// PITWeigh( void )         from ASM = PITWGH
// As name suggests, called on PIT at maximum PIT frequency.
// Takes care of batch cycle sequence tasks if BATSEQ = FILL or VOLMODE.
//
// M.McKiernan                          24-06-2004
// First pass.
//////////////////////////////////////////////////////
void PITWeigh( void )
{
int nIndex,i,nComp,nCompIndex;
int nLatency,nTemp;
unsigned char cTableByte;
    nTemp = 0;
    if(!g_bOutputDiagMode)                  // doesnt run in output diagnostics mode
    {
        if(g_cBatSeq == BATSEQ_VOLMODE)
        {
            //VolFill
//--REVIEW--    VolFill();   Didnt think VolFill is actually used, but program ended up stuck in it.
            g_bAnyOn = FALSE;

            i = 0;
            while(g_cSeqTable[i] != SEQDELIMITER && i < MAX_COMPONENTS) // SEQDELIMITER indicates table end
            {
                nComp = (int)(g_cSeqTable[i] & 0x0F);
                nCompIndex = nComp - 1; // index is 0 based.
                if( g_lComponentVolumetricTimer[nCompIndex] >= g_lCmpTime[nCompIndex] )
                {
                    CmpNOff( nComp, FALSE );    // switch component off
                }
                else
                {

                    CmpNOn( nComp, FALSE ); // leave or switch component on.
                    g_bAnyOn = TRUE;        // indicate at least one component on.
                    g_lComponentVolumetricTimer[nCompIndex]++; // increment vol count.
                }

            }   // end of while.

            if( g_bAnyOn == FALSE )     // no  valves open??
            {
                g_bAnyOn = FALSE; //--REVIEW-- (    not needed )
                g_cBatSeq = BATSEQ_LEVPOLL; // go on to level sensor polling.
                for(i = 0; i < MAX_COMPONENTS; i++)
                    g_lComponentVolumetricTimer[i] = 0; // clear vol. timers.
            }
        }  // end of VolFill.

        // FILLING??
        else if(g_cBatSeq == BATSEQ_FILL)
        {
            if( g_bTopUpFlag )
            {
                if(g_nTopUpSequencing == FILLCOLOUR)
                {
                    // read comp no. from LASTCOMPTABLEP
                    nIndex = g_nLastCompTableIndex;
                    g_nPITCom = (int)(g_cSeqTable[g_nLastCompTableIndex] & 0x0F);
                }
                else if(g_nTopUpSequencing == TOPUPCOLOUR)  //Top up enabled & TOP UP colour
                {
                    g_nPITCom = TOPUPCOMPONENT; // set to 8 if TopUpColour --REVIEW--
                    nIndex = g_nSeqTableIndex;
                }
                else
                {
                    g_nPITCom = (int)(g_cSeqTable[g_nSeqTableIndex] & 0x0F);
                    nIndex = g_nSeqTableIndex;
                }
                nTemp = (int)(g_cSeqTable[nIndex]);
            }
            else // top up flag not set
            {
                    nIndex = g_nSeqTableIndex;
                    nTemp = (int)(g_cSeqTable[g_nSeqTableIndex]);
                    g_nPITCom = nTemp & 0x0F;
            }
            if((nTemp & FILLBYWEIGHT) != 0)
            {
                g_bComponentHasFilledByWeight[g_nPITCom-1] = TRUE;
             }
            else
            {
                g_bComponentHasFilledByWeight[g_nPITCom-1] = FALSE;
            }

            if(g_bActivateLatency)
            {
                if(g_nPulsingSequence[g_nPITCom-1] == VALVE_PULSING_STAGE)
                {
                    g_bActivateLatency = FALSE;
                    nLatency = 0;
                }
                else
                {
                    nLatency = GetLatency();
                }
                g_nValveLatencyTime++;
                if(nLatency > g_nValveLatencyTime)
                {
                    CmpNOn( g_nPITCom, FALSE);      // Component #n on.
                }
                else
                {
                    g_bActivateLatency = FALSE;
                    g_nValveLatencyTime = 0;
                }
            }
            else // NOVLLT (No valve latency)
            {

                cTableByte = g_cSeqTable[nIndex];
                if(cTableByte & FILLBYWEIGHT)
                    g_bAnyFillByWeight = TRUE;
                else
                    g_bAnyFillByWeight = FALSE;

                if((cTableByte & FILLBYWEIGHT) == 0)
                    ByTime(); // checking if component being filled should be switched off or left on.
                else
                {
                    CmpNOn(g_nPITCom, FALSE);
                    g_lComponentOnTimer++;  // CMPNTIM
                    if(g_CalibrationData.m_nBlenderType == TSM3000 || g_nPITCom <= 4 )
                    {
                        if(g_bWeightAvailableFlag)  // has A/D conversion run?(WGTAVAIL)
                        {
                            g_bWeightAvailableFlag = FALSE;
                            CheckWeight();
                        }
                    }
                    else
                    {

                        if(g_bOneSecondWeightAverageAvailable)  // one sec wt avg available?
                        {
                            g_bOneSecondWeightAverageAvailable = FALSE;
                            CheckWeight();
                        }
                    }

                }

            }


        }   //end of if(g_cBatSeq == BATSEQ_FILL) brackets.

    } // end for if diagnostics o/p mode bracket.

}


//////////////////////////////////////////////////////
// CmpNOn( int nComponent, BOOL bMBMode)            from ASM = MBCMPNON/CMPNON-
// Note: Includes setting of the LFillStatCtr[i], which makes it the same as CMPNON.
// Turns on the output for the specified component for a batch blender  - Component no is 0-MAX_COMPONENTS.
// If bMBMode TRUE, then equates to MBCMPNON in assembler.
//
// Note, the component no. passed to the routine is 1 based, first component = 1.
// Routine sets up all flags, and starts timer if it is first pass.
// Also accounts for non standard component configuration.  For example Componet 1, could use component 5 output.
// Also adds an increment to the raw A/D counts for the Autocycler.
//
// Entry:   Component #, integer, (1 based) to be switched on
//              mode of funtion, BOOL,  if true function equates to MBCMPNON
//
// Exit:        none
//
// M.McKiernan                          17-06-2004
// First pass.
//////////////////////////////////////////////////////
void CmpNOn( int nComponent, BOOL bMBMode )     // note component no. is 1 based, i.e. first is 1 not 0.
{
   int nMask,i;
   int nCompConfig;
   if((nComponent ==TOPUPCOMPONENT) &&(g_nTopUpSequencing == TOPUPCOLOUR))
  {
        nComponent = g_cSeqTable[g_nSeqTableIndex] & 0x0f;
        g_nTopUpComponent = nComponent;
  }


//LFILLSTATCTR set up in CMPNON, not in MBCMPNON.
    if(bMBMode == FALSE)
        g_nLFillStatCtr[nComponent-1] = LFILLONTIME10HZ;    //FILL INDICATION ON TIME IN 10HZ INCREMENTS.

    g_bFillStatCmp[nComponent-1] = ON;          //flag to indicate component is on
    g_bFillStatusAll = ON;                          // for display purposes??

    g_bLFillStatCmp[nComponent-1] = ON;         //flag to indicate component is on

    if(!g_bAutoCycleTestFlag && g_bAutoCycleFlag)
    {
        AddRawForAutoCycyle();
    }

    if(!g_bFirstTime)
    {
        g_bFirstTime = TRUE;
        g_ProcessData.m_lComponentActivations[nComponent-1]++;
        g_sOngoingHistoryComponentLogData.m_nHistoryComponentActivations[nComponent-1]++;
// STARTCOUNTING:                       - expanded inline i.e startcounting function in line.
        g_nPreviousTime = sim.timer[2].tcn;         // current time
        g_bIsCounting = TRUE;
        g_lTimeMeasured = 0;                        // clear time measured variable (long)
    }
    if(g_CalibrationData.m_bStandardCompConfig)                     //STDCCFG
    {
        CmpOn( nComponent );
    }
    else        // i.e. non standard config
    {
        nCompConfig = g_CalibrationData.m_nCompConfig[nComponent - 1];
        nMask = (0x0001);
        for(i=0;  i < MAX_COMPONENTS; i++)
        {
            if(nCompConfig & nMask)
            {
                CmpOn( i + 1 );             // CmpOn is 1 based (i.e. if i= 0, swith on comp. 1.
            }
            nMask <<= 1;                        // shift mask bit one to left
        }

    }

}

//////////////////////////////////////////////////////
// AddRawForAutoCycyle( void )        ASM ADDRAWFORAUTOCYCLE
//
// Also adds an increment(weight) to the raw A/D counts for the Autocycler.
// Increment depends on g_nPITCom (PIT component), and whether multi-stage filling is being used.
//
// EXIT: g_lRawAtDCounts += nIncrement
// M.McKiernan                          17-06-2004
// First pass.
//////////////////////////////////////////////////////

void AddRawForAutoCycyle( void )
{
    int nIncrement = 0;
    int nStage;

    if(g_bActivateLatency == FALSE)                 // if no valve latency time     //ASM=ACTIVATELATENCY
    {
        if((g_CalibrationData.m_bTopUpMode == ENABLED) && (g_nPITCom == TOPUP_COMPONENT))                           // ASM = TOPUPF
        {
         //           if(g_bFlowRateChange)
         //           {
         //              nIncrement = COMPONENT_1_FLOWRATE_REDUCED;
         //           }
         //           else
         //           {
                         nIncrement = COMPONENT_1_FLOWRATE;
         //           }
        }
        else    // not top-up component.
        {
            if(g_CalibrationData.m_nStageFillEn != SINGLE_STAGE_FILLING)            // ASM = STAGEFILLEN ( can be single/two/three)
            {
                nStage = g_nStageSeq[g_nPITCom - 1];
                if(g_nPITCom == 1)
                    nIncrement = COMPONENT_1_FLOWRATE;              // if comp. 1, always use comp 1 flowrate.
                else if(nStage == STAGE_3)
                    nIncrement = STAGE_3_MFILLRATE;
                else if(nStage == STAGE_2)
                    nIncrement = STAGE_2_MFILLRATE;
                else if(nStage == STAGE_1)
                    nIncrement = STAGE_1_MFILLRATE;
                else                                                            // else assume default to stage 1 rate.
                    nIncrement = STAGE_1_MFILLRATE;
            }
            else // no stage filling
            {
                switch(g_nPITCom)
                {
                case 1:
                    //if(g_bFlowRateChange)
                   // {
                   //    nIncrement = COMPONENT_1_FLOWRATE_REDUCED;
                   // }
                   // else
                   // {
                        nIncrement = COMPONENT_1_FLOWRATE;
                   // }
                    break;
                case 2:
                if(g_bFlowRateChange[1])
                    {
                       nIncrement = COMPONENT_1_FLOWRATE_REDUCED;
                    }
                    else
                    {
                        nIncrement = COMPONENT_1_FLOWRATE;
                    }

                   // nIncrement = COMPONENT_2_FLOWRATE;
                    break;
                case 3:
                    nIncrement = COMPONENT_3_FLOWRATE;
                    break;
                case 4:
                    nIncrement = COMPONENT_4_FLOWRATE;
                    break;
               case 5:
                    if(g_bFlowRateChange[4])
                    {
                        nIncrement = COMPONENT_5_FLOWRATE_REDUCED;
                    }
                    else
                    {
                        nIncrement = COMPONENT_5_FLOWRATE;
                    }
                    break;

               case 9:
               if(g_bFlowRateChange[8])
                    {
                       nIncrement = 1;
                    }
                    else
                    {
                       nIncrement = COMPONENT_4_FLOWRATE;

                    }
                       break;

                default:
                    nIncrement = COMPONENT_4_FLOWRATE;
                    break;
                }
            }
        }

    }

    g_lRawAtDCounts += nIncrement;

}

//////////////////////////////////////////////////////
// CmpNOff( int nComponent, BOOL bMBMode)           from ASM = CMPNOFF/MBCMPNOFF
//
// Turns off the output for the specified component for a batch blender  - Component no is 0-MAX_COMPONENTS.
// If the MBmode flag is set, it clears the g_nLFillStatCtr[]'s, which makes it equivalent to ASM = MBCMPNOFF.
// Note, the component no. passed to the routine is 1 based, first component = 1.
// Routine clears all flags, and stops timer
// Also accounts for non standard component configuration.  For example Componet 1, could use component 5 output.
// Also
// M.McKiernan                          21-06-2004
// First pass.
//////////////////////////////////////////////////////

void CmpNOff( int nComponent, BOOL bMBMode )                        // not component no. is 1 based, i.e. first is 1 not 0.
{
    int nMask,i;
    int nCompConfig;
    if((nComponent ==TOPUPCOMPONENT) &&(g_nTopUpSequencing == TOPUPCOLOUR))
    {
         nComponent = g_cSeqTable[g_nSeqTableIndex] & 0x0f;
    }

    g_bLFillStatCmp[nComponent-1] = OFF;            //flag to indicate component is off
    if(bMBMode)
        g_nLFillStatCtr[nComponent-1] = 0;              //

    g_bFillStatCmp[nComponent-1] = OFF;         //flag to indicate component is off
    g_bFillStatusAll = OFF;                         // for display purposes??

    g_bFirstTime = FALSE;
// ENDCOUNTING              Subroutine EndCounting is expanded inline in the following 3 lines of c code.
    TotaliseCount( );
    g_bIsCounting = FALSE;
    g_lCmpOpenClk[nComponent-1] = g_lTimeMeasured;

// CON1 - but for the off part.
    if(g_CalibrationData.m_bStandardCompConfig)                     //STDCCFG
    {
        CmpOff( nComponent );
    }
    else        // i.e. non standard config
    {
        nCompConfig = g_CalibrationData.m_nCompConfig[nComponent - 1];
        nMask = (0x0001);
        for(i=0;  i < MAX_COMPONENTS; i++)
        {
            if(nCompConfig & nMask)
            {
                CmpOff( i + 1 );                // CmpOff is 1 based (i.e. if i= 0, swith on comp. 1.
            }
            nMask <<= 1;                        // shift mask bit one to left
        }

    }

}


//////////////////////////////////////////////////////
// TotaliseCount            from ASM = TOTALISECOUNT
//
// Totalises time, counter being used is the processor TCNT.
// Curent and previous TCNT values store in  g_nCurrentTime, g_nPreviousTime respectively.
// Check is made for TCNT rollover.
//
// EXIT:    g_lTimeMeasured has the totalised count.
//
// M.McKiernan                          21-06-2004
// First pass.
//////////////////////////////////////////////////////
void TotaliseCount( void )
{
unsigned int nDiffInPClk;

    if(g_bIsCounting)                           // only totalise if in IsCounting mode.
    {
        g_nCurrentTime = sim.timer[2].tcn;      // current time
        if(g_nCurrentTime < g_nPreviousTime)    // check for rollover of 16 bit counter.
        {
            nDiffInPClk = 0xFFFFFFFF -  g_nPreviousTime + g_nCurrentTime;
        }
        else
        {
            nDiffInPClk = g_nCurrentTime - g_nPreviousTime;
        }

        g_nPreviousTime = g_nCurrentTime;   // save the current value for next pass.
// ADDREALTIME:
        g_lTimeMeasured += nDiffInPClk;     // (ASM - TIMEMEASURED)

    }

}

//////////////////////////////////////////////////////
// CalculateSeconds()           from ASM = CALCULATESECONDS
//
// Calculates time in seconds for which each valve has been open.
//
// EXIT:
//
// M.McKiernan                          30-07-2004
// First pass.
//////////////////////////////////////////////////////
void CalculateSeconds( void )
{
unsigned int i;
float fTime;

    for( i = 0; i < g_CalibrationData.m_nComponents; i++)   //for each component.
    {
        if(g_CurrentRecipe.m_fPercentage[i] > 0)  // is set % non-zero??
        {
            //time in seconds = clocks / clock frequency.
            fTime = (float)g_lCmpOpenClk[i] / GPT_FREQUENCY;
        }
        else // Set % = 0.
        {
            fTime = 0;  // zero open time if Set % = 0.
        }

        g_fComponentOpenSeconds[i] = fTime;
    }

}

/*
CALCULATESECONDS:
        LDAB    NOBLNDS                                 ; READ NO OF BLENDS.
REPOPC  PSHB
        LDX     #CMP1OPENPCLK                   ; COMPONENT 1 OPEN TIME IN SECONDS.
        DECB
        ABX
        ABX
        ABX                                                             ;
        JSR     HEXBCD3X                ; HEX TO BCD RESULT IN D,E
        STAB    MATHCREG
        STE     MATHCREG+1
        CLRW    MATHCREG+3
        LDX     #$2464                                  ; 62464 PULSES PER SECOND.
        STX     AREG+3
        LDAA    #$06
        STAA    AREG+2                  ; MULTIPLIER.
        CLRW    AREG                    ;
        JSR     DIV                     ; C/A

        LDAB    #RAMBANK
        TBZK
        LDX     #CMP1OPENSECONDS        ; SECONDS STORAGE.
        PULB
        PSHB
        DECB
        ABX
        ABX
        ABX
        LDZ     #PCNT1                  ; SECONDS STORAGE.
        PULB
        PSHB
        DECB
        ABZ
        ABZ
        TST     0,Z
        BNE     MOVTIM
        CLRW    CMP1OPENSECONDS
        CLR     CMP1OPENSECONDS+2
        BRA     CON1
MOVTIM  JSR     EMOVE                   ; STORE.
CON1    PULB                                                    ; RECOVER COMPONENT COUNTER.
        DECB
        BNE             REPOPC                                  ; REPEAT OPEN CALCULATION.
        RTS




*/

//////////////////////////////////////////////////////
// ByTime( void )                       ASM = BYTIME
//
// Routine called at end of  PITWeigh program, hence on PIT.
// Checking if the component which is currently filling needs to be switched off, or left on.
//
// EXIT: none
// M.McKiernan                          23-06-2004
// First pass.
//////////////////////////////////////////////////////

void ByTime( void )
{
long    lTime;
BOOL  bResult;

    g_lComponentOnTimer++;                                  // CMPNTIM

//  nComponent = (int)(g_nPITCom & 0x0F);

//28092004  g_nPITCom = (int)(g_cSeqTable[g_nSeqTableIndex] & 0x0F);                                    // get component no. from table, mask off the m.s. nibble
    lTime = g_lCmpTime[g_nPITCom -1];                   // required time for this component. CMP1TIMS       ; TIME STORAGE BLOCK

    if(g_lComponentOnTimer <= lTime)
    {   //LEAVEON
      CmpNOn( g_nPITCom, FALSE );           // i.e. CMPNON
        if(g_bOneSecondWeightAverageAvailable == TRUE)          // is there a one second weight average available.
        {
            g_bOneSecondWeightAverageAvailable = FALSE;
            bResult = CheckComponentUsed( );        // check if component requires a wt. check.
            if(bResult == TRUE)
            {
                CheckWeight();      // CHECKWEIGHT
            }
        }
    }
    else
    {
        //SWCMPOFF
        CmpNOff( g_nPITCom, FALSE );
        g_lComponentOnTimer = 0;
        // SETMODE.
        g_bFillAlarm = FALSE;  //clear fill alarm
        SetMode1( );
    }

}

/*
SWCMPOFF:
        LDAB    PITCOM
        JSR     CMPNOFF         ; SWITCH OFF
        CLRW    CMPNTIM         ; CLEAR COUNTER
        CLR     CMPNTIM+2       ; RESET
        LBRA     SETMODE
        RTS

LEAVEON  LDAB    PITCOM          ; READ STORED COMP NO
        JSR     CMPNON          ; LEAVE O/P ON
        TST     WGT1SECAVG        ;  HAS THE A/D CONVERSION RUN ?
        BNE     YESAVAIL1        ;  YES AVAILABLE
        RTS
YESAVAIL1:
        CLR     WGT1SECAVG      ;  CLEAR AVAILABLE FLAG.
        JSR     CHECKCOMPONENTUSED
        TSTA                    ; WEIGHT CHECK NECESSARY ?
        LBNE    CHECKWEIGHT
        RTS

*/
//////////////////////////////////////////////////////
// SetMode1( void )                     ASM = SETMODE1
//
// Routine called at end of  PITWeigh program, in ByTime.
// Checking when a  component has been  switched off what next to do, i.e which mode to go to.
// Note: FILLALARM is not cleared.
//
// EXIT: g_cCalcRouter (ASM = CALCROUTER) has code for calculations required.
// M.McKiernan                          23-06-2004
// First pass.
//
// P.Smith                          21/10/05
// Second pass - added check for regrind bit.
//
//////////////////////////////////////////////////////

void SetMode1( void )
{
char cCode,cData;
int  nComparison;
//       g_bFillAlarm = FALSE;              // //asm = FILLALARM
         g_cBatSeq = BATSEQ_SETTLING;       // indicate settling mode.  //
         g_nDmpSec = 0;                     // reset timer.
         cData = g_cSeqTable[g_nSeqTableIndex];
         if(cData & REGRIND_BIT)
         {
            cCode = CALC1STCMPTAR;              //CALCULATE 1ST COMPONENT TARGET.
         }
         else
         {  //NOTREGC
            if(g_cSeqTable[0] & REGRIND_BIT)
            nComparison = 1;
            else
            nComparison = 0;

            if(g_nSeqTableIndex == nComparison)           // 1st element in Sequence table.
            {
                cCode = CALCMPNTAR;             // "CALCULATE REST OF COMPONENTS" TIME.
            }
            else  // not 1st component
            {
//              if(!g_bRegrindPresent)          // NO REGRIND???
                if(g_CurrentRecipe.m_nRegrindComponent == 0)            // NO REGRIND???
                    cCode = NORMCALC;               // NORMAL SEQUENCING CPI CALCULATION.
                else
                {       // there is a regrind
                    if(g_bFstCompCalFin)            // "FIRST COMP CALCULATION FINISHED ?  //FSTCOMPCALFIN
                    {
                        g_bFstCompCalFin = FALSE;
                        cCode = CALCMPNTAR;     // "CALCULATE REST OF COMPONENTS" TIME.
                    }
                    else
                        cCode = NORMCALC;           // NORMAL SEQUENCING CPI CALCULATION.
                }
            }

         }
 //CONPIT
      g_cCalcRouter = cCode;
      g_nAvgCtr = 0;              // A/D AVERAGE COUNTER CLEARED.  (MMK - dont think this is used)
      g_bWeightAvgAvail = FALSE;  // CLEAR Weight avg AVAILABLE FLAG
}

//////////////////////////////////////////////////////
// CheckComponentUsed( void )                       ASM = CHECKCOMPONENTUSED
//
// CHECK IF COMPONENT REQUIRES WEIGHT CHECK, IF >4 STD OR >6 MEGA NO WEIGHT CHECK REQUIRED.
// Component checked is in PITCom (PIT component)
//
// EXIT: Return TRUE if a weight check is needed.
// M.McKiernan                          23-06-2004
// First pass.
//////////////////////////////////////////////////////

BOOL CheckComponentUsed( void )
{
    BOOL bWtCheckNeeded = FALSE;
    bWtCheckNeeded = TRUE;
    return(bWtCheckNeeded);
}

//////////////////////////////////////////////////////
// CloseAll( void )                     ASM = CLOSEALL
//
// Closes valves/ deactivates outputs, for all blender components.
//
// EXIT: None
// M.McKiernan                          25-06-2004
// First pass.
//////////////////////////////////////////////////////
void CloseAll( void )
{
    unsigned int i;
    if( !g_bOutputDiagMode )            // dont use if in Output Diagnostics mode.
    {
        for(i=1; i <= g_CalibrationData.m_nComponents; i++)
        {
        CmpNOff(i, FALSE);
        }
    }
}


//////////////////////////////////////////////////////
// Attargt( void )                      ASM = ATTARGT (Part of PITWEIGH)
//
//
//
// EXIT: None
// M.McKiernan                          205-07-2004
// First pass.
//////////////////////////////////////////////////////
void AtTargt( void )
{
unsigned char cData;
int nIndex;
//  if(g_nPITCom == 5)
//      ;               //--REVIEW-- (assembler not doing anything.

//  cCode = (char)g_nPITCom;    // component no. in Char format.
//  cCode |= NOFILLALARM;       //
   RemoveAlarmTable( NOFILLALARM,  g_nPITCom );        // clear alarm, Component no
   CmpNOff( g_nPITCom, FALSE ); // CMPNOFF.

    if(g_bTopUpFlag && g_nTopUpSequencing == FILLCOLOUR)    //Top up enabled & fill colour
    {
      nIndex = g_nLastCompTableIndex;  //LASTCOMPTABLEP
    }
    else
    {
      nIndex = g_nSeqTableIndex;  //SEQPOINTER
    }

   cData = g_cSeqTable[nIndex];  //load byte from selected index

    if(cData & LEAVEFILLBYWEIGHT)
    {
        cData = LEAVEFILLBYWEIGHT ^ 0xFF;   // COM of LEAVEFILLBYWEIGHT
      cData &= g_cSeqTable[nIndex];
        g_cSeqTable[nIndex] = cData;
//      ResetWeightRoundRobin(g_nPITCom, 2);
    }
    else    //CLRASN
    {
        cData = FILLBYWEIGHT ^ 0xFF;    // COM of FILLBYWEIGHT
      cData &= g_cSeqTable[nIndex];
        g_cSeqTable[nIndex] = cData;    // Reset Fill by weight bit.
//      ResetWeightRoundRobin(g_nPITCom, 2);
    }
//NOFBWC
    g_lCmpTime[g_nPITCom -1] = g_lComponentOnTimer; //Store time taken for fill
    g_lComponentOnTimer = 0;    // Reset on timer.

// SETMODE.
    g_bFillAlarm = FALSE;  //clear fill alarm
    SetMode1( );
}

// checking longest CHK Wt
    int g_nChkWtEnd = 0;            //
//    int g_nChkWtStart = 0;
    int g_nChkWtLengthMax = 0;
    int g_nChkWtLength = 0;
    long    g_lChkWtTimeSum = 0;
    int g_nChkWtTimeSumCtr = 0;
    int g_nChkWtTimeAvg = 0;

//////////////////////////////////////////////////////
// CheckWeight( void )                      ASM = CHECKWEIGHT (Part of PITWEIGH)
//
// The target counts are calculated and compared such tht if filling by time
// the fill valve will be shut off when target exceeded.
//
// EXIT: None
// M.McKiernan                          21-07-2004
// First pass.
//////////////////////////////////////////////////////
void CheckWeight( void )
{
long lTargetADCounts,lCompareADCounts;
unsigned char cTableByte,cData;
int nCompIndex = g_nPITCom-1;
int nIndex,nConfig;
int nCompareTimeout;
int nTest;
BOOL  bCompGT4Used = FALSE;

//    g_nChkWtStart = GPT_TCNT;           // current TCNT time


    lTargetADCounts = g_lComponentTargetCounts[nCompIndex] + g_lCurrentLCAtDValue;

    if(g_bTopUpFlag && g_nTopUpSequencing == FILLCOLOUR)    //Top up enabled & fill colour
    {
      nIndex = g_nLastCompTableIndex;  //LASTCOMPTABLEP
    }
    else
    {
      nIndex = g_nSeqTableIndex;  //SEQPOINTER
    }

   cTableByte = g_cSeqTable[nIndex];  //load byte from selected index

    if(cTableByte & FILLBYWEIGHT)
    {
        if(g_nPITCom == 3)
            nTest = 0;  // debug purposes, want to set brkpt for component 3.

        if(g_CalibrationData.m_nBlenderType == TSM3000   || g_nPITCom <= 4 )
       {
            //USERAW - from Assembler.
            if(g_fComponentTargetWeight[nCompIndex] > 0.030)  //Target wt > 30g
                lCompareADCounts = g_lRawAtDCounts;  // use raw counts if > 30g
            else
            {
                // use 1/8 S average. (ANHSLC2)
                lCompareADCounts = g_lEighthSecondWeightAverage;
                // --REVIEW--
                if(!g_bEighthSecondWeightAverageAvailable)
                    lCompareADCounts = g_lRawAtDCounts;  // use raw counts if 1/8 sec avg not available.
            }
        }
        else // not Mega,  & comp no. > 4
        {
                // use 1/2 S average. (ANHSLC1)
                lCompareADCounts = g_lHalfSecondWeightAverage;
                // --REVIEW--
                if(!g_bHalfSecondWeightAverageAvailable)
                    lCompareADCounts = g_lRawAtDCounts;  // use raw counts if 1/2 sec avg not available.
        }
    }
    else  // FILLBYWEIGHT not set.
    {  // BYTIME1
                // use 1/2 S average. (ANHSLC1)
                lCompareADCounts = g_lHalfSecondWeightAverage;
                // --REVIEW--
                if(!g_bHalfSecondWeightAverageAvailable)
                    lCompareADCounts = g_lRawAtDCounts;  // use raw counts if 1/2 sec avg not available.
    }

    if(lCompareADCounts >= lTargetADCounts)
        AtTargt();

   else  //NOTATG
    {
            if(g_CalibrationData.m_nBlenderMode != MODE_THROUGHPUTMON)
            {
                         if(g_CalibrationData.m_nBlenderType == TSM3000)
                {
                        nCompareTimeout = FILLTIMEOUT;  // fill timeout period.
                }
                else
                {
                    if(g_CalibrationData.m_bStandardCompConfig) // standard component config??
                    {
                        if(g_nPITCom > 4)
                            bCompGT4Used = TRUE;
                    }
                    else  // non stanard comp config
                    {
                        nConfig = g_CalibrationData.m_nCompConfig[nCompIndex];
                        // component no. > 4 used??
                        if(nConfig & 0xFFF0)
                            bCompGT4Used = TRUE;
                    }

                    if(bCompGT4Used)
                    { // YESGTH4
                            if(g_CurrentRecipe.m_fPercentage[nCompIndex] > 5.00) //Target % > 5%??
                                nCompareTimeout = FILLTIMEOUTG5P;
                            else
                                nCompareTimeout = FILLTIMEOUTG4;
                    }
                    else //i.e. comp <=4 being used.
                    {
                            nCompareTimeout = FILLTIMEOUT;  // fill timeout period.
                    }


                }
                if(g_nFillCounter > nCompareTimeout)
                {   //FILLERROR
                    CmpNOff(g_nPITCom, FALSE);  //Switch off troublesome component.
                    if(g_bChangingOrder)
                        AtTargt();      //Changing order - allow to proceed.
                        // move on if this is the regrind component.
                        // --REVIEW-- sense of RegrindRetry (Assembler has going to NOREGTO if REGRETRY set)
//                  else if(g_bRegrindPresent && !g_CalibrationData.m_bRegrindRetry && g_nPITCom == g_CalibrationData.m_nRegrindComponentNumber)
                    else if(g_CurrentRecipe.m_nRegrindComponent && !g_CalibrationData.m_bRegrindRetry && g_nPITCom == g_CurrentRecipe.m_nRegrindComponent)
                    {

                        cData = g_cSeqTable[nIndex];  //load byte from selected index
                        if(cData & FILLBYWEIGHT)
                        {
                            g_cSeqTable[nIndex] |= LEAVEFILLBYWEIGHT; //Set leave fill by weight bit.
                        }

                        AtTargt();  //

                    }

                    else
                    { //NOREGTO
                        // clear On Timer
                        g_lComponentOnTimer = 0;
                        // clear Fill counter
                        g_nFillCounter = 0;
//                      cCode = (char)g_nPITCom;    // component no. in Char format.
//                      cCode |= NOFILLALARM;       // set fill alarm bit.
                       PutAlarmTable( NOFILLALARM,  g_nPITCom );       // indicate alarm, Component no
//                       iprintf("\n fill alarm occurred"); //nbb--testonly--

                        //set generic fill alarm flag
                        g_bFillAlarm = TRUE;
                  g_nAnyFillAlarm++;
                        g_nDmpSec = 0;
                        g_nFillCtr++;
                        if(g_CalibrationData.m_nFillRetries == 0 || g_nFillCtr < g_CalibrationData.m_nFillRetries)
                        { //RETRYCM (Continuous retry)
                                SetMode1();
                        }

                    }

                }
            }
        }
}


//////////////////////////////////////////////////////
// GetLatency( void )                       ASM = GETLATENCY
//
// get the latency and return time, latency based on type of blender
// and component no. being used.
//
// EXIT: Latency time period returned.
// M.McKiernan                          21-07-2004
// First pass.      g_nVavleCalculatedLatencyTime
//////////////////////////////////////////////////////
int GetLatency( void )
{
    int nLatency = 0;
    int nCompIndex = g_nPITCom - 1;  // index (for component arrays) is 0 based.

        if(g_bTopUpFlag && (g_nPITCom == TOPUPCOMPONENT))  // latency required if top up and 8th component
        {
            nLatency = g_nVavleCalculatedLatencyTime;
        }
        else if(g_CalibrationData.m_nBlenderType == TSM650 || g_CalibrationData.m_nBlenderType == TSM350 )
        {
                if(g_nPITCom <= 4)
                    nLatency = g_nVavleCalculatedLatencyTime;
                else
                    nLatency = VENTURILATENCY;  // venturi feeder latency
        }

        else if( g_CalibrationData.m_nBlenderType == TSM150 || g_CalibrationData.m_nBlenderType == TSM50 )
        {  // ISMICRA
            if(g_CalibrationData.m_bStandardCompConfig)
            {
                if(g_nPITCom > TWO)
                    nLatency = VENTURILATENCY;  // venturi feeder latency for comp >2.
                else
                    nLatency = g_nVavleCalculatedLatencyTime; // TWOINCHLATENCY;  // 2" latency for comp 1 & 2.
                    //--REVIEW-- seems to be a problem with assembler here. should it be 2" for 1&2??
            }
            else // non-standard comp config
            {
                if( g_CalibrationData.m_nCompConfig[nCompIndex] & 0x0003 )
                    nLatency = g_nVavleCalculatedLatencyTime; //TWOINCHLATENCY;  // 3" latency for comp 1 & 2.
                else
                    nLatency = VENTURILATENCY;  // venturi feeder latency for comp >2.
                    //--REVIEW-- seems to be a problem with assembler here.
            }
        }
        else
            nLatency = g_nVavleCalculatedLatencyTime; //THREEINCHLATENCY;    // use 3" latency for others. --REVIEW--

    return(nLatency);
}


/*
; GET THE LATENCY AND RETURN TIME IN D REG

GETLATENCY:
        TST     ADDR_HX
        BNE     NORML           ; READ LATENCY
        LDD     PASWRD          ;
        CPD     #$0065          ; SPECIFIC PASSWORD ?
        BNE     NORML           ; NORMAL LATENCY
        LDAA    PASWRD+2
        CMPA    #$54            ;
        BEQ     READLT          ; READ LATENCY

NORML   LDAA    BLENDERTYPE     ; READ BLENDER TYPE
        CMPA    #MINITYPE       ; MINI BLENDER
        BNE     NOTMINI         ; NO

        TST     STDCCFG         ; IS THIS A STANDARD BLENDER
        BEQ     ISSTDB          ; YES
        LDAB    PITCOM
        DECB                    ; DECREMENT COMPONENT NO.
        LDX     #COMP1CFG        ; CONFIG.
        ABX                     ; DETERMINE
        ABX                     ; DETERMINE
        LDD     0,X
        ANDD    #$03            ; SLIDES 1 OR 2 IN COMPONENT ?
        BEQ     SETSMLL         ; SET SMALL LATENCY
;;T
SETLGL:
        LDD     #THREEINCHLATENCY ; 3" VALVE LATENCY
        BRA     XITLAT          ; EXIT
SETSMLL LDD     #TWOINCHLATENCY ;
        BRA     XITLAT          ; EXIT
ISSTDB  LDAB    PITCOM
        CMPB    #TWO            ; HIGHER THAN 2ND COMP
        BHI     SETSMLL
        BRA     SETLGL          ; SET LARGE LATENCY

NOTMINI:
        CMPA    #TINYTYPE       ; MICRA BATCH ?
        BEQ     ISMICRA         ; YES
        CMPA    #MIDITYPE       ; MIDI BATCH
        BNE     NOTMIDI         ; NO
ISMICRA TST     STDCCFG         ; IS THIS A STANDARD BLENDER
        BEQ     ISSTMD          ; YES
        LDAB    PITCOM
        DECB                    ; DECREMENT COMPONENT NO.
        LDX     #COMP1CFG        ; CONFIG.
        ABX                     ; DETERMINE
        ABX                     ; DETERMINE
        LDD     0,X
        ANDD    #$03            ; SLIDES 1 OR 2 IN COMPONENT ?
        BNE     SETSMLL
SETVTU  LDD     #VENTURILATENCY
        BRA     XITLAT          ; EXIT


ISSTMD: LDAB    PITCOM
        CMPB    #TWO            ; HIGHER THAN 2ND COMP
        BHI     SETVTU          ; VENTURI FEEDERS


NOTMIDI LDD     #THREEINCHLATENCY
XITLAT  RTS

READLT
        LDD     LATENCY         ; READ LATENCY
        BRA     XITLAT          ; EXIT



*/
/*
CHECKWEIGHT:
        LDX     #CMP1CNTGH
        LDAB    PITCOM
        DECB
        ABX
        ABX
        ABX
;
;       ADD THIS TO THE CURRENT LC READING.
;
        LDE     CURRLCADH+1
        ADDE    1,X
        STE     LCTARGETH+1
        LDAB    CURRLCADH
        ADCB    0,X             ; CALCULATE NEW TARGET.
        STAB    LCTARGETH       ;
        LDX     #LCTARGETH


        PSHM    X
        TST     TOPUPF           ; TOP UP ?
        BEQ     NORMLC1         ; NORMAL COMPONENT.
        LDAA    TOPUPSEQUENCING ; SET THE SEQUENCING.
        CMPA    #FILLCOLOUR     ; FILL THE COLOUR NEXT.
        BNE     NORMLC1         ; NORMAL COMPONENT.
        LDX     LASTCOMPTABLEP
        BRA     RDCOMP1         ; READ COMPONENT

NORMLC1 LDX     SEQPOINTER      ; POINTER TO SEQ TABLE
RDCOMP1 LDAB    0,X
        PULM    X
        ANDB    #FILLBYWEIGHT   ; IS THIS FILL BY WEIGHT ?
        BEQ     BYTIME1

        LDAA    BLENDERTYPE     ; CHECK BLENDER TYPE.
        CMPA    #MEGATYPE       ; MEGABATCH BLENDER.
        BEQ     USERAW          ; MEGA USE RAW COUNTS AT ALL TIMES.
        LDAA    PITCOM          ; CHECK COMP NO
        CMPA    #4              ; 5TH COMP
        BHI     IS5TH
USERAW  LDX     #CMP1TARWGT      ; TARGET WEIGHT.
        LDAB    PITCOM          ; COMPONENT NO.
        DECB
        ABX
        ABX
        ABX                     ;
        LDAA    0,X
        BNE     USERW1          ; USE RAW VALUE.
        LDD     1,X             ;
        CPD     #$300            ; TARGET LESS THAN 30 GRAMS.
        BHI     USERW1          ; NO
        LDAA    ANHSLC2H
        LDE     ANHSLC2H+1
        BRA     CONCHK

USERW1  LDAA    RAW132          ;  READ ACTUAL COUNTS
        LDE     RAW132+1        ;
        BRA     CONCHK

IS5TH   NOP
BYTIME1
        LDAA    ANHSLC1H        ; CHECK IF 0.5 SECOND WEIGHT IS EXCEEDED.
        LDE     ANHSLC1H+1

CONCHK  LDX     #LCTARGETH
        CMPA    0,X             ;  AT TARGET.
        BHI     ATTARGT         ;
        BEQ     YESMSB          ;  MSB SAME
        LBRA     NOTATG
        RTS
YESMSB  CPE     1,X             ;  CMP1CNTGH+1      ;  COMPARE
        LBLO     NOTATG          ;  NOT AT TARGET..


*/

/*
ATTARGT LDAA    PITCOM          ; READ COMPONENT NO.
        CMPA    #5              ; IS THIS COMPONENT 5.
        BNE     NOTFIVE         ; NO
NOTFIVE LDAA    PITCOM          ; READ PIT COMP NO
        ORAA    #NOFILLALARM    ;
        JSR     RMALTB          ; INDICATE FILL ALARM
        LDAB   #HARDWAREBANK                   ; HARDWARE BANK
        TBZK                                   ; STORE BANK NO
        LDZ    #BUZZER
        STAA   0,Z
;        STAA    BUZZER
        LDAB    PITCOM
        JSR     CMPNOFF         ;  COMP #N OFF
        LDAB    PITCOM
        CMPB    #05
        BNE     NOT5TH
DIAGSTOP:
        NOP
NOT5TH:

        TST     TOPUPF           ; TOP UP ENABLED.
        BEQ     NOTTPU
        LDAA    TOPUPSEQUENCING ; SET THE SEQUENCING.
        CMPA    #FILLCOLOUR     ; FILL THE COLOUR NEXT.
        BNE     NOTTPU
        LDX     LASTCOMPTABLEP
        BRA     RDCOMDT         ; READ COMPONENT

NOTTPU  LDX     SEQPOINTER      ; RELOAD POINTER
RDCOMDT LDAA    0,X
        ANDA    #LEAVEFILLBYWEIGHT
        BEQ     CLRASN          ; CLEAR AS NORMAL
        LDAB    #LEAVEFILLBYWEIGHT
        COMB
        ANDB    0,X
        STAB    0,X             ; RESET FILL BY TIME BIT.
        LDAA    #2              ;
        LDAB    PITCOM          ; READ COMPONENT NO
        JSR     RESETWGTRROBIN
        BRA     NOFBWC          ; NO FILL BY WEIGHT CLEAR.


CLRASN  LDAB    #FILLBYWEIGHT
        COMB
        ANDB    0,X
        STAB    0,X             ; RESET FILL BY TIME BIT.
        LDAA    #2              ;
        LDAB    PITCOM          ; READ COMPONENT NO
        JSR     RESETWGTRROBIN

NOFBWC  LDX     #CMP1TIMS       ;  TIME TAKEN STORAGE BLOCK
        LDAB    PITCOM          ;
        DECB                    ;  DECREMENT TO ALLOW TO CALCULATE
;        ASLB                    ;  * 2
        ABX
        ABX
        ABX
        LDD     CMPNTIM         ;  TIME TAKEN FOR FILL
        STD     0,X             ;  STORE HEX VERSION
        LDAA    CMPNTIM+2
        STAA    2,X             ; STORE LSB
BYTIME2 CLRW     CMPNTIM         ;  CLEAR COUNTER
        CLR     CMPNTIM+2       ; CLEAR LSB
SETMODE:
        CLR     FILLALARM
SETMODE1:
;        LDAA    #CALC
        LDAA    #SETTLING
        STAA    BATSEQ          ; INDICATE THAT CALCULATION REQUIRED.
        CLRW    DMPSEC          ; RESET COUNTER
        LDX     SEQPOINTER      ;
        LDAA    0,X
        ANDA    #REGBIT         ; IS THIS THE REGRIND COMPONENT
        BEQ     NOTREGC         ; NO
        LDAA    #CALC1STCMPTAR  ; CALCULATE 1ST COMPONENT TARGET.
        BRA     CONPIT          ;
NOTREGC CPX     #SEQTABLE       ; AT THE BEGINNING OF TABLE
        BNE     NORCAL
        LDAA    #CALCMPNTAR
        BRA     CONPIT
NORCAL  TST     REG_PRESENT     ; REGRIND ?
        BEQ     NOREG
        TST     FSTCOMPCALFIN   ; "FIRST COMP CALCULATION FINISHED ?
        BEQ     NOREG           ; NO
        CLR     FSTCOMPCALFIN
        LDAA    #CALCMPNTAR     ; "CALCULATE REST OF COMPONENTS" TIME.
        BRA     CONPIT


NOREG   LDAA    #NORMCALC       ; NORMAL SEQUENCING CPI CALCULATION.
CONPIT  STAA    CALCROUTER      ; INDICATE CALCULATION REQUIRED.
        CLR     AVGCTR          ;  A/D AVERAGE COUNTER CLEARED.
        CLR     AVGAVAIL        ;  CLEAR AVAILABLE FLAG
EXITPIT RTS

*/

/*
;       CHECK FOR "FILL BY TIME" SERVICE.
;

BYTIME:
        CLRB
        LDE     CMPNTIM+1
        ADDE    #1
        STE     CMPNTIM+1
        ADCB    CMPNTIM
        STAB    CMPNTIM
        LDAB    PITCOM          ; READ COMP NO
        LDX     #CMP1TIMS       ; TIME STORAGE BLOCK
        DECB
        ABX
        ABX
        ABX
        LDAA    CMPNTIM         ; READ COUNTER
        CMPA    0,X             ; CHECK MSB
        BHI     SWCMPOFF        ; SWITCH COMPONENT OFF.
        BLO     LEAVEON         ; IF LOWER LEAVE ON.
        LDE     CMPNTIM+1       ; LSB
        CPE     1,X             ; COMPARE
        BHI     SWCMPOFF        ; IF HIGHER SWITCH COMPONENT OFF.
        BNE     LEAVEON         ; IF EQUAL SWITCH COMPONENT OFF.

SWCMPOFF:
        LDAB    PITCOM
        JSR     CMPNOFF         ; SWITCH OFF
        CLRW    CMPNTIM         ; CLEAR COUNTER
        CLR     CMPNTIM+2       ; RESET
        LBRA     SETMODE
        RTS

LEAVEON  LDAB    PITCOM          ; READ STORED COMP NO
        JSR     CMPNON          ; LEAVE O/P ON
        TST     WGT1SECAVG        ;  HAS THE A/D CONVERSION RUN ?
        BNE     YESAVAIL1        ;  YES AVAILABLE
        RTS
YESAVAIL1:
        CLR     WGT1SECAVG      ;  CLEAR AVAILABLE FLAG.
        JSR     CHECKCOMPONENTUSED
        TSTA                    ; WEIGHT CHECK NECESSARY ?
        LBNE    CHECKWEIGHT
        RTS



*/
// CHECK IF COMPONENT REQUIRES WEIGHT CHECK.
// IF >4 STD OR >6 MEGA NO WEIGHT CHECK REQUIRED.
// EXIT: Return TRUE if a weight check is needed.
/*
BOOL CheckComponentUsed( void )
{
    BOOL bWtCheckNeeded = FALSE;

    if(g_CalibrationData.m_bStandardCompConfig)
    {                                                               // Standard component config.
        if(g_CalibrationData.m_nBlenderType == TSM3000 )
        {
            if(g_nPITCom <= 6)
                bWtCheckNeeded = TRUE;
        }
        else        // not Mega
        {
            if(g_nPITCom <= 4)
                bWtCheckNeeded = TRUE;
        }
    }
    else        //non standard
    {
        nCompConfig = g_CalibrationData.m_nCompConfig[g_nPITCom - 1];       // pick up this components configuration.
        if(g_CalibrationData.m_nBlenderType == TSM3000 )
        {
            if((nCompConfig & 0xFFC0) == 0)                 // check if output used is <= 6
                bWtCheckNeeded = TRUE;
        }
        else    // i.e. not a Mega
        {
            if((nCompConfig & 0xFFF0) == 0)                 // check if output used is <= 4
                bWtCheckNeeded = TRUE;
        }
    }

    return(bWtCheckNeeded);
}
*/

/*
; CHECK IF COMPONENT REQUIRES WEIGHT CHECK.
; IF >4 STD OR >6 MEGA NO WEIGHT CHECK REQUIRED.
;

CHECKCOMPONENTUSED:
        LDAB    PITCOM          ; READ COMPONENT NO.
        TST     STDCCFG      ; NON STANDARD COMPONENT CONFIG ?
        BNE     ISNONSTD        ; YES.
        LDAA    BLENDERTYPE     ; CHECK TYPE.
        CMPA    #MEGATYPE       ; MEGA BATCH
;        BEQ     ISMEGA          ; IS MEGA BATCH.
        BEQ     WTCK            ; IS MEGA BATCH.

        CMPB    #4              ; >4
        BRA     CMP
ISMEGA  CMPB    #6
CMP     BHI     NOWTCK          ; NO WEIGHT CHECK.
        BRA     WTCK            ; WEIGHT CHECK AND EXIT.
WTCK    LDAA    #1
        BRA     XITCCK          ; EXIT CHECK.
NOWTCK  CLRA                    ;
XITCCK  RTS


ISNONSTD:
        DECB                    ; DECREMENT COMPONENT NO.
        LDX     #COMP1CFG        ; CONFIG.
        ABX                     ; DETERMINE
        ABX                     ; DETERMINE
        LDAA    BLENDERTYPE     ; CHECK BLENDER TYPE.
        CMPA    #MEGATYPE       ; MEGABATCH
        BEQ     ISMEGA1         ; YES.
        LDD     0,X             ; READ COMPONENT NO
        ANDD    #$00F0          ; ANY COMPONENT >4
        BRA     DOCHK
ISMEGA1 LDD     0,X
        ANDD    #$00C0          ; ANY COMPONENT >6
DOCHK   BNE     NOWTCK          ; NO WEIGHT CHECK
        BRA     WTCK            ; ENABLE WEIGHT CHECK.


*/
/*
SETMODE:
        CLR     FILLALARM
SETMODE1:
;        LDAA    #CALC
        LDAA    #SETTLING
        STAA    BATSEQ          ; INDICATE THAT CALCULATION REQUIRED.
        CLRW    DMPSEC          ; RESET COUNTER
        LDX     SEQPOINTER      ;
        LDAA    0,X
        ANDA    #REGBIT         ; IS THIS THE REGRIND COMPONENT
        BEQ     NOTREGC         ; NO
        LDAA    #CALC1STCMPTAR  ; CALCULATE 1ST COMPONENT TARGET.
        BRA     CONPIT          ;
NOTREGC CPX     #SEQTABLE       ; AT THE BEGINNING OF TABLE
        BNE     NORCAL
        LDAA    #CALCMPNTAR
        BRA     CONPIT
NORCAL  TST     REG_PRESENT     ; REGRIND ?
        BEQ     NOREG
        TST     FSTCOMPCALFIN   ; "FIRST COMP CALCULATION FINISHED ?
        BEQ     NOREG           ; NO
        CLR     FSTCOMPCALFIN
        LDAA    #CALCMPNTAR     ; "CALCULATE REST OF COMPONENTS" TIME.
        BRA     CONPIT


NOREG   LDAA    #NORMCALC       ; NORMAL SEQUENCING CPI CALCULATION.
CONPIT  STAA    CALCROUTER      ; INDICATE CALCULATION REQUIRED.
        CLR     AVGCTR          ;  A/D AVERAGE COUNTER CLEARED.
        CLR     AVGAVAIL        ;  CLEAR AVAILABLE FLAG
EXITPIT RTS

*/


/*
CMPNON  LDX     #LFILLSTATCTR1
        PSHB                    ; SAVE B
        DECB
        ABX
        PULB
        LDAA    #LFILLONTIME10HZ; FILL INDICATION ON TIME IN 10HZ INCREMENTS.
        STAA    0,X             ; INDICATE THAT COMPONENT IS ON.

MBCMPNON:

MBCMPNON:
        LDX     #FILLSTATCMP1
        PSHB                    ; SAVE B
        DECB
        ABX
        PULB
        LDAA    #1
        STAA    0,X             ; INDICATE THAT COMPONENT IS ON.
        STAA    FILLSTATUSALL

        LDX     #LFILLSTATCMP1
        PSHB                    ; SAVE B
        DECB
        ABX
        PULB
        LDAA    #1
        STAA    0,X             ; INDICATE THAT COMPONENT IS ON.

        TST     AUTOCYCLETESTFLAG
        BNE     NOCYCLE         ; NO AUTO CYCLE.
        TST     AUTOCYCLEFLAG
        BEQ     NOCYCLE         ; NO AUTO CYCLE.
        LDE     RAW132+2
;        LDAA    BLENDERTYPE     ; CHECK BLENDER TYPE.
;        CMPA    #MEGATYPE       ; MEGABATCH BLENDER.
;        BEQ     LESTH4          ; MEGABLENDER
;        CMPB    #4
;        BLS     LESTH4
;        ADDE    #25
;        BRA     DOADD           ; DO ADDITION.
LESTH4  JSR     ADDRAWFORAUTOCYCLE ; ADD WEIGHT FOR AUTOCYCLE.
DOADD   STE     RAW132+2
        LDE     RAW132
        ADCE    #0
        STE     RAW132

NOCYCLE:

        TST     FIRSTTIME       ; FIRST TIME ?
        BNE     ISNOTF          ; NO
        LDAA    #1
        STAA    FIRSTTIME       ; SET FIRST TIME FLAG.
        JSR     STARTCOUNTING   ; START COUNTER TO MEASURE OPENING TIME.
        BRA     ISNOTF          ; COMPONENT ON

CMPNOFFONLY:
        LDY     #OFTABLE
        CLRA                    ; CODE FOR STATUS INDICATION
        BRA     CON1

CMPNONONLY:
ISNOTF  LDY     #ONTABLE        ; TABLE START ADDRESS
        LDAA    #1              ; STATUS CODE.
CON1
        TST     STDCCFG      ; CONFIGURATION.
        BEQ     STDCCF          ; STANDARD.
        PSHB                    ; SAVE COMPONENT NO.
        LDX     #COMP1CFG        ;
        DECB
        ABX
        ABX
        LDAB    #1              ; HOPPER NO COUNTER.
        LDE     0,X             ; READ COMPONENT INFORMATION.
        BEQ     STDCCFP         ; STANDARD CONFIGURATION.
        LDAA    #MAXCOMPONENTNO ; SHIFT COUNTER.
REPSHF  LSRE                    ; SHIFT COMPONENT DATA.
        BCC     NOACTIV         ; NO ACTIVATION.
        JSR     ACTCMPN         ; ACTIVATE COMPONENT ON/OFF.
NOACTIV INCB                    ; INCREMENT HOPPER
        DECA                    ; SHIFT COUNTER -1
        BNE     REPSHF
        PULB
        RTS
STDCCFP PULB
STDCCF  JSR     ACTCMPN         ; ACTIVATE HOPPER COMPARTMENT #N
        RTS

ACTCMPN PSHM    D,Y             ; SAVE REGISTERS.
        PSHB
        CLRB
        TBYK
        PULB                    ; RECOVER B
        ANDB    #$0F            ;
        DECB                    ;
        ASLB                    ;
        ABY                     ; ADDRESS
        LDY     0,Y             ; SERVICE ROUTINE
        JSR     0,Y
        PULM    D,Y             ; RECOVER REGISTER
        RTS
// CmpOn( nComponent )

aDDRAWFORAUTOCYCLE:
        TST     ACTIVATELATENCY
        BNE     XITADD          ; NO VALVE LATENCY TIME.
        TST     TOPUPF           ; TOP UP ENABLED.
        BEQ     NOTPU            ; NOT TOP UP
        CMPB    #EIGHT           ; TOP UP COMPONENT
        BEQ     CMP1RAT
NOTPU   CMPB    #1
        BNE     NOT1            ; NO.
CMP1RAT ADDE    #COMPONENT1FLOWRATE ; COMPONENT FLOW RATE.
        BRA     XITADD          ; EXIT ADDITION.

NOT1    TST     STAGEFILLEN     ; STAGED FILLING
        BEQ     NOTMF1
        LDX     #STAGESEQ1
        LDAB    g_nPITCom          ; READ COMPONENT
        DECB
        ABX
        LDAA    0,X
        BEQ     SETS1
        CMPA    #THREE          ; STAGE ?
        BNE     NOTMF3          ; NO
SETS1   ADDE    #STAGE1MFILLRATE
        BRA     XITADD          ; EXIT

NOTMF3  LDAA    0,X
        CMPA    #TWO            ; STAGE ?
        BNE     NOTMF2          ; NO
        ADDE    #STAGE2MFILLRATE
        BRA     XITADD          ; EXIT

NOTMF2  LDAA    0,X
        CMPA    #ONE            ; STAGE ?
        BNE     NOTMF1          ; NO
        ADDE    #STAGE3MFILLRATE
        BRA     XITADD          ; EXIT


NOTMF1  LDAB    g_nPITCom
        CMPB    #2              ; COMPONENT 2.
        BNE     NOT2            ; NO.
        ADDE    #COMPONENT2FLOWRATE
        BRA     XITADD          ; EXIT ADDITION.
NOT2    CMPB    #3              ; COMPONENT 3.
        BNE     NOT3            ; NO.
        ADDE    #COMPONENT3FLOWRATE
        BRA     XITADD          ; EXIT ADDITION.
NOT3    CMPB    #4              ; COMPONENT 4.
        BNE     NOT4            ; NO.
        ADDE    #COMPONENT4FLOWRATE
        BRA     XITADD          ; EXIT ADDITION.
NOT4    ADDE    #COMPONENT4FLOWRATE
XITADD  RTS

*/

/*
MBCMPNOFF:
        LDX     #LFILLSTATCMP1
        PSHB                    ; SAVE B
        DECB
        ABX
        PULB
        CLR     0,X             ; INDICATE THAT COMPONENT IS ON.
        LDX     #LFILLSTATCTR1
        PSHB                    ; SAVE B
        DECB
        ABX
        PULB
        CLR    0,X             ; INDICATE THAT COMPONENT IS ON.

CMPNOFF LDX     #FILLSTATCMP1
        PSHB                    ; SAVE B
        DECB
        ABX
        PULB
        LDAA    #0
        STAA    0,X             ; INDICATE THAT COMPONENT IS ON.
        STAA    FILLSTATUSALL

        LDY     #OFTABLE
        CLRA                    ; CODE FOR STATUS INDICATION
        CLR     FIRSTTIME       ; RESET FLAG.
        JSR     ENDCOUNTING     ; START COUNTER TO MEASURE OPENING TIME.
        LBRA    CON1

CMPNON  LDX     #LFILLSTATCTR1
        PSHB                    ; SAVE B
        DECB
        ABX
        PULB
        LDAA    #LFILLONTIME10HZ; FILL INDICATION ON TIME IN 10HZ INCREMENTS.
        STAA    0,X             ; INDICATE THAT COMPONENT IS ON.

MBCMPNON:
        LDX     #FILLSTATCMP1
        PSHB                    ; SAVE B
        DECB
        ABX
        PULB
        LDAA    #1
        STAA    0,X             ; INDICATE THAT COMPONENT IS ON.
        STAA    FILLSTATUSALL

        LDX     #LFILLSTATCMP1
        PSHB                    ; SAVE B
        DECB
        ABX
        PULB
        LDAA    #1
        STAA    0,X             ; INDICATE THAT COMPONENT IS ON.

        TST     AUTOCYCLETESTFLAG
        BNE     NOCYCLE         ; NO AUTO CYCLE.
        TST     AUTOCYCLEFLAG
        BEQ     NOCYCLE         ; NO AUTO CYCLE.
        LDE     RAW132+2
;        LDAA    BLENDERTYPE     ; CHECK BLENDER TYPE.
;        CMPA    #MEGATYPE       ; MEGABATCH BLENDER.
;        BEQ     LESTH4          ; MEGABLENDER
;        CMPB    #4
;        BLS     LESTH4
;        ADDE    #25
;        BRA     DOADD           ; DO ADDITION.
LESTH4  JSR     ADDRAWFORAUTOCYCLE ; ADD WEIGHT FOR AUTOCYCLE.
DOADD   STE     RAW132+2
        LDE     RAW132
        ADCE    #0
        STE     RAW132

NOCYCLE:

        TST     FIRSTTIME       ; FIRST TIME ?
        BNE     ISNOTF          ; NO
        LDAA    #1
        STAA    FIRSTTIME       ; SET FIRST TIME FLAG.
        JSR     STARTCOUNTING   ; START COUNTER TO MEASURE OPENING TIME.
        BRA     ISNOTF          ; COMPONENT ON

CMPNOFFONLY:
        LDY     #OFTABLE
        CLRA                    ; CODE FOR STATUS INDICATION
        BRA     CON1

CMPNONONLY:
ISNOTF  LDY     #ONTABLE        ; TABLE START ADDRESS
        LDAA    #1              ; STATUS CODE.
CON1
        TST     STDCCFG      ; CONFIGURATION.
        BEQ     STDCCF          ; STANDARD.
        PSHB                    ; SAVE COMPONENT NO.
        LDX     #COMP1CFG        ;
        DECB
        ABX
        ABX
        LDAB    #1              ; HOPPER NO COUNTER.
        LDE     0,X             ; READ COMPONENT INFORMATION.
        BEQ     STDCCFP         ; STANDARD CONFIGURATION.
        LDAA    #MAXCOMPONENTNO ; SHIFT COUNTER.
REPSHF  LSRE                    ; SHIFT COMPONENT DATA.
        BCC     NOACTIV         ; NO ACTIVATION.
        JSR     ACTCMPN         ; ACTIVATE COMPONENT ON/OFF.
NOACTIV INCB                    ; INCREMENT HOPPER
        DECA                    ; SHIFT COUNTER -1
        BNE     REPSHF
        PULB
        RTS
STDCCFP PULB
STDCCF  JSR     ACTCMPN         ; ACTIVATE HOPPER COMPARTMENT #N
        RTS


ENDCOUNTING:
        PSHB
        LDX     #CMP1OPENPCLK           ;
        DECB
        ABX
        ABX
        ABX
        PULB
        PSHM    D,E,X,Y
        JSR     TOTALISECOUNT
        CLR     ISCOUNTING
        LDE     TIMEMEASURED            ;       READ TOTAL CLOCK PULSES.
        STE     0,X                                     ;   STORE RESULT.
        LDAA    TIMEMEASURED+2          ;   LSB
        STAA    2,X                                     ;   STORE RESULT.
        PULM    D,E,X,Y
        RTS

 TOTALISECOUNT:
        PSHM    D,E,X,Y
        TST     ISCOUNTING              ; IS COUNTING SET ?
        BEQ     XITTOT                  ; EXIT.
        LDAB   #HARDWAREBANK                   ; HARDWARE BANK
        TBZK                                   ; STORE BANK NO
        LDZ     #TCNT                    ; READ COUNTER VALUE.
        LDD     0,Z
;        LDD     TCNT            ; READ COUNTER
        STD     CURRENTTIME     ; CURRENT VALUE.
        PSHM    D               ; SAVE CURRENT VALUE.
        CPD     PREVIOUSTIME    ; COMPARE TO PREVIOUS READING.
        BLO     HASOVR          ; OVERFLOWED
        SUBD    PREVIOUSTIME    ; CALCULATE DIFFERENCE.
        PULM    E               ; RECOVER CURRENT VALUE.
        STE     PREVIOUSTIME    ; STORE.
        BRA     ADDAXT          ; EXIT.
HASOVR  LDD     #$FFFF          ; MAX VALUE.
        SUBD    PREVIOUSTIME    ; FFFF - PREVIOUS VALUE
        ADDD    CURRENTTIME     ; ADD CURRENT VALUE.
        PULM    E               ;
ADDAXT  STD     DIFFINPCLK
        STE     PREVIOUSTIME    ;
        LDX     #TIMEMEASURED   ; TIME MEASURED
        JSR     ADDREALTIME     ; ADD TOTAL
XITTOT  PULM    D,E,X,Y
        RTS


ADDREALTIME:
        PSHM    D,E,X,Y
        CLRA
        LDE     DIFFINPCLK      ; READ PULSES
        ADDE    1,X             ; ADD PULSES.
        STE     1,X             ; RESTORE.
        ADCA    0,X             ; MSB ADDED.
        STAA    0,X             ; STORE.
        PULM    D,E,X,Y
        RTS



// ENDCOUNTING


ENDCOUNTING:
        PSHB
        LDX     #CMP1OPENPCLK           ;
        DECB
        ABX
        ABX
        ABX
        PULB
        PSHM    D,E,X,Y
        JSR     TOTALISECOUNT
        CLR     ISCOUNTING
        LDE     TIMEMEASURED            ;       READ TOTAL CLOCK PULSES.
        STE     0,X                                     ;   STORE RESULT.
        LDAA    TIMEMEASURED+2          ;   LSB
        STAA    2,X                                     ;   STORE RESULT.
        PULM    D,E,X,Y
        RTS
  */
/*
; ALL OUTPUTS SWITCHED OFF.
CLOSEALL:

        LDAB    #ONE
REPOFF:
        PSHB
        JSR     CMPNOFF
        PULB
        INCB
        CMPB    NOBLNDS

mce12-15

PITWGH  TST     OUTPUTDIAG
        BEQ     NODIAG
        RTS
NODIAG  LDAA    BATSEQ          ;
        CMPA    #VOLMODE        ; VOLMETRIC FILLING MODE.
        LBEQ     VOLFILL
        LDAA    BATSEQ          ; SEQUENCER.
        CMPA    #FILL           ; IS FILL ENABLED ?
        LBNE     EXITPIT


        TST     TOPUPF           ; TOP UP ?
        BEQ     NORMLC          ; NORMAL COMPONENT.
        LDAA    TOPUPSEQUENCING ; SET THE SEQUENCING.
        CMPA    #FILLCOLOUR     ; FILL THE COLOUR NEXT.
        BEQ     RDCOLOR         ; READ COLOUR.
        CMPA    #TOPUPCOLOUR    ; IS THE COLOUR BEING TOPPED UP ?
        BNE     NORMLC          ; NO REVERT TO NORMAL.
        LDX     SEQPOINTER      ; POINTER TO SEQ TABLE
        LDAB    0,X
        ANDB    #$0F
        LDAB    #EIGHT
        BRA     CONFILL

RDCOLOR LDX     LASTCOMPTABLEP
        BRA     RDCOMP          ; READ COMPONENT

NORMLC  LDX     SEQPOINTER      ; POINTER TO SEQ TABLE
RDCOMP  LDAB    0,X
        ANDB    #$0F
CONFILL STAB    PITCOM          ; STORE COMPNO

        TST     ACTIVATELATENCY
        BEQ     NOVLLT
        JSR     GETLATENCY      ; GET LATENCY VALUE FOR COMPONENT CONCERNED
        INCW    VALVELATENCYTIME
        CPD     VALVELATENCYTIME ; COMPARE TO COUNTER
        BHI     SWCMPON          ; SWITCH COMPONENT ON
        CLR     ACTIVATELATENCY ; RESET COUNTER
        CLRW    VALVELATENCYTIME ; RESET COUNTER
        LBRA    EXITPIT          ; EXIT PIT


SWCMPON LDAB    PITCOM
        JSR     CMPNON          ; SET COMPONENT #1 ON
        LBRA    EXITPIT          ; EXIT PIT

NOVLLT  LDAB    0,X
        ANDB    #FILLBYWEIGHT   ; IS THIS FILL BY WEIGHT ?
        CLRA
        TSTB
        BEQ     BYTIM
        LDAA    #1
BYTIM   STAA    ANYFILLBYWEIGHT
        TSTB
        LBEQ     BYTIME
        LDAB    PITCOM
        JSR     CMPNON          ; SET COMPONENT #1 ON
        CLRB
        LDE     CMPNTIM+1
        ADDE    #1
        STE     CMPNTIM+1
        ADCB    CMPNTIM
        STAB    CMPNTIM

        LDAA    BLENDERTYPE     ; CHECK BLENDER TYPE.
        CMPA    #MEGATYPE       ; MEGABATCH BLENDER.
        BEQ     USERWT          ; USE RAW WEIGHT SIGNAL.
        LDAA    PITCOM
        CMPA    #4
        BHI     ONESECA

USERWT  TST     WGTAVAIL        ;  HAS THE A/D CONVERSION RUN ?
        BNE     YESAVAIL        ;  YES AVAILABLE
        RTS
YESAVAIL:
        CLR     WGTAVAIL        ;  CLEAR AVAILABLE FLAG.
        BRA     CHECKWEIGHT

ONESECA:
        TST     WGT1SECAVG        ;  HAS THE A/D CONVERSION RUN ?
        BNE     YESAVAIL2        ;  YES AVAILABLE
        RTS
YESAVAIL2:
        CLR     WGT1SECAVG      ;  CLEAR AVAILABLE FLAG.
CHECKWEIGHT:
        LDX     #CMP1CNTGH
        LDAB    PITCOM
        DECB
        ABX
        ABX
        ABX
;
;       ADD THIS TO THE CURRENT LC READING.
;
        LDE     CURRLCADH+1
        ADDE    1,X
        STE     LCTARGETH+1
        LDAB    CURRLCADH
        ADCB    0,X             ; CALCULATE NEW TARGET.
        STAB    LCTARGETH       ;
        LDX     #LCTARGETH


        PSHM    X
        TST     TOPUPF           ; TOP UP ?
        BEQ     NORMLC1         ; NORMAL COMPONENT.
        LDAA    TOPUPSEQUENCING ; SET THE SEQUENCING.
        CMPA    #FILLCOLOUR     ; FILL THE COLOUR NEXT.
        BNE     NORMLC1         ; NORMAL COMPONENT.
        LDX     LASTCOMPTABLEP
        BRA     RDCOMP1         ; READ COMPONENT

NORMLC1 LDX     SEQPOINTER      ; POINTER TO SEQ TABLE
RDCOMP1 LDAB    0,X
        PULM    X
        ANDB    #FILLBYWEIGHT   ; IS THIS FILL BY WEIGHT ?
        BEQ     BYTIME1

        LDAA    BLENDERTYPE     ; CHECK BLENDER TYPE.
        CMPA    #MEGATYPE       ; MEGABATCH BLENDER.
        BEQ     USERAW          ; MEGA USE RAW COUNTS AT ALL TIMES.
        LDAA    PITCOM          ; CHECK COMP NO
        CMPA    #4              ; 5TH COMP
        BHI     IS5TH
USERAW  LDX     #CMP1TARWGT      ; TARGET WEIGHT.
        LDAB    PITCOM          ; COMPONENT NO.
        DECB
        ABX
        ABX
        ABX                     ;
        LDAA    0,X
        BNE     USERW1          ; USE RAW VALUE.
        LDD     1,X             ;
        CPD     #$300            ; TARGET LESS THAN 30 GRAMS.
        BHI     USERW1          ; NO
        LDAA    ANHSLC2H
        LDE     ANHSLC2H+1
        BRA     CONCHK

USERW1  LDAA    RAW132          ;  READ ACTUAL COUNTS
        LDE     RAW132+1        ;
        BRA     CONCHK

IS5TH   NOP
BYTIME1
        LDAA    ANHSLC1H        ; CHECK IF 0.5 SECOND WEIGHT IS EXCEEDED.
        LDE     ANHSLC1H+1

CONCHK  LDX     #LCTARGETH
        CMPA    0,X             ;  AT TARGET.
        BHI     ATTARGT         ;
        BEQ     YESMSB          ;  MSB SAME
        LBRA     NOTATG
        RTS
YESMSB  CPE     1,X             ;  CMP1CNTGH+1      ;  COMPARE
        LBLO     NOTATG          ;  NOT AT TARGET..

ATTARGT LDAA    PITCOM          ; READ COMPONENT NO.
        CMPA    #5              ; IS THIS COMPONENT 5.
        BNE     NOTFIVE         ; NO
NOTFIVE LDAA    PITCOM          ; READ PIT COMP NO
        ORAA    #NOFILLALARM    ;
        JSR     RMALTB          ; INDICATE FILL ALARM
        LDAB   #HARDWAREBANK                   ; HARDWARE BANK
        TBZK                                   ; STORE BANK NO
        LDZ    #BUZZER
        STAA   0,Z
;        STAA    BUZZER
        LDAB    PITCOM
        JSR     CMPNOFF         ;  COMP #N OFF
        LDAB    PITCOM
        CMPB    #05
        BNE     NOT5TH
DIAGSTOP:
        NOP
NOT5TH:

        TST     TOPUPF           ; TOP UP ENABLED.
        BEQ     NOTTPU
        LDAA    TOPUPSEQUENCING ; SET THE SEQUENCING.
        CMPA    #FILLCOLOUR     ; FILL THE COLOUR NEXT.
        BNE     NOTTPU
        LDX     LASTCOMPTABLEP
        BRA     RDCOMDT         ; READ COMPONENT

NOTTPU  LDX     SEQPOINTER      ; RELOAD POINTER
RDCOMDT LDAA    0,X
        ANDA    #LEAVEFILLBYWEIGHT
        BEQ     CLRASN          ; CLEAR AS NORMAL
        LDAB    #LEAVEFILLBYWEIGHT
        COMB
        ANDB    0,X
        STAB    0,X             ; RESET FILL BY TIME BIT.
        LDAA    #2              ;
        LDAB    PITCOM          ; READ COMPONENT NO
        JSR     RESETWGTRROBIN
        BRA     NOFBWC          ; NO FILL BY WEIGHT CLEAR.


CLRASN  LDAB    #FILLBYWEIGHT
        COMB
        ANDB    0,X
        STAB    0,X             ; RESET FILL BY TIME BIT.
        LDAA    #2              ;
        LDAB    PITCOM          ; READ COMPONENT NO
        JSR     RESETWGTRROBIN

NOFBWC  LDX     #CMP1TIMS       ;  TIME TAKEN STORAGE BLOCK
        LDAB    PITCOM          ;
        DECB                    ;  DECREMENT TO ALLOW TO CALCULATE
;        ASLB                    ;  * 2
        ABX
        ABX
        ABX
        LDD     CMPNTIM         ;  TIME TAKEN FOR FILL
        STD     0,X             ;  STORE HEX VERSION
        LDAA    CMPNTIM+2
        STAA    2,X             ; STORE LSB
BYTIME2 CLRW     CMPNTIM         ;  CLEAR COUNTER
        CLR     CMPNTIM+2       ; CLEAR LSB
SETMODE:
        CLR     FILLALARM
SETMODE1:
;        LDAA    #CALC
        LDAA    #SETTLING
        STAA    BATSEQ          ; INDICATE THAT CALCULATION REQUIRED.
        CLRW    DMPSEC          ; RESET COUNTER
        LDX     SEQPOINTER      ;
        LDAA    0,X
        ANDA    #REGBIT         ; IS THIS THE REGRIND COMPONENT
        BEQ     NOTREGC         ; NO
        LDAA    #CALC1STCMPTAR  ; CALCULATE 1ST COMPONENT TARGET.
        BRA     CONPIT          ;
NOTREGC LDAA    SEQTABLE
        ANDA    #REGBIT
        BEQ     DONRC           ; CHECK FOR 1ST TABLE POSITION.
        CPX     #SEQTABLE+1     ; AT SECOND POSITION IN THE TABLE.
        BRA     DOCMP
DONRC   CPX     #SEQTABLE       ; AT THE BEGINNING OF TABLE
DOCMP   BNE     NORCAL
        LDAA    #CALCMPNTAR
        BRA     CONPIT
NORCAL  TST     REG_PRESENT     ; REGRIND ?
        BEQ     NOREG
        TST     FSTCOMPCALFIN   ; "FIRST COMP CALCULATION FINISHED ?
        BEQ     NOREG           ; NO
        CLR     FSTCOMPCALFIN
        LDAA    #CALCMPNTAR     ; "CALCULATE REST OF COMPONENTS" TIME.
        BRA     CONPIT


NOREG   LDAA    #NORMCALC       ; NORMAL SEQUENCING CPI CALCULATION.
CONPIT  STAA    CALCROUTER      ; INDICATE CALCULATION REQUIRED.
        CLR     AVGCTR          ;  A/D AVERAGE COUNTER CLEARED.
        CLR     AVGAVAIL        ;  CLEAR AVAILABLE FLAG
EXITPIT RTS

NOTATG:
        LDAA    BLENDERMODE     ; CHECK MODE.
        CMPA    #THROUGHPUTMON  ; THROUGHPUT MONITOR ?
        LBEQ     STILLFILL
        LDAA    FILLCOUNTER
        LDAB    BLENDERTYPE     ; CHECK BLENDER TYPE.
        CMPB    #MEGATYPE       ; MEGABATCH BLENDER.
        BEQ     NOEXTF          ; NO EXTENDED FILL TIME ALLOWED.
        TST     STDCCFG              ; NON STANDARD CONFIGURATION.
        BEQ     DONORCM         ; NORMAL COMPARISON.

        LDX     #COMP1CFG               ; COMPONENT CONFIGURATION.
        LDAB    PITCOM                  ; CHANNEL NO.
        DECB
        ABX
        ABX
        LDE     0,X                     ; READ COMPONENT CONFIG.
        ANDE    #$00F0                  ; ANY SCREW FEEDERS
        BNE     YESGTH4

DONORCM LDAB    PITCOM          ; NO OF COMPONENTS.
        CMPB    #4
        BHI     YESGTH4
NOEXTF  CMPA    #FILLTIMEOUT    ; AT FILL TIMEOUT IN SECONDS
        BRA     COMPARE
YESGTH4 LDAB    PITCOM          ; READ COMPONENT NO.
        LDX     #PCNT1
        DECB
        ASLB
        ABX
        LDE     0,X
        CPE     #$0500
        BHI     LONGTO
        CMPA    #FILLTIMEOUTG4  ;LONGER DELAY FOR >4 COMP.
        BRA     COMPARE
LONGTO  CMPA    #FILLTIMEOUTG5P ;LONGER DELAY FOR >4 COMP.
COMPARE LBLS     STILLFILL       ; FILL AS BEFORE


FILLERROR:
        LDAB    PITCOM
        JSR     CMPNOFF         ; SWITCH THE TROUBLESOME ONE OFF
        TST     CHANGINGORDER   ; CHANGING ORDER.
        LBNE    ATTARGT         ; CHANGING ORDER ALLOW TO PROCEED.

        TST     REG_PRESENT     ; ANY REGRIND
        BEQ     NOREGTO         ; NOT THIS ORDER.
        TST     REGRINDRETRY    ; REGRIND RETRY
        BNE     NOREGTO         ; RETRY RETRY IS ENABLED
        LDAB    PITCOM
        CMPB    REGCOMP         ; IS THIS REGRIND.
        BNE     NOREGTO         ; MOVE ONTO NEXT COMPONENT.

        LDX     SEQPOINTER      ; POINTER TO SEQ TABLE
        LDAB    0,X
        ANDB    #FILLBYWEIGHT   ; IS THIS COMPONENT FILL BY WEIGHT ?
        LBEQ    ATTARGT         ; MOVE ONTO NEXT COMPONENT.

        LDAB    #LEAVEFILLBYWEIGHT ; LEAVE FILL BY WEIGHT THIS TIME
        ORAB    0,X             ;
        STAB    0,X             ;
        LBRA    ATTARGT         ; MOVE ONTO NEXT COMPONENT.

NOREGTO CLRW    CMPNTIM         ;  CLEAR COUNTER
        CLR     CMPNTIM+2       ; CLEAR LSB
        CLR     FILLCOUNTER     ; RESET COUNTER
        LDAA    PITCOM          ; READ PIT COMP NO
        ORAA    #NOFILLALARM    ;
        JSR     PTALTB          ; INDICATE FILL ALARM
        LDAA    #$0F
        STAA    FILLALARM
        INC     ANYFILLALARM
        CLRW    DMPSEC

        TST     FILLRETRY       ; CONTINUOUS RETRY
        BEQ     RETRYCM
        INC     FILLCTR         ; FILL COUNTER
        LDAA    FILLCTR
        CMPA    FILLRETRY      ; AT COUNT
        BLO     RETRYCM         ; RETRY
        LDAA    #$0F
        STAA    MOVEON          ; MOVE ON TO NEXT
        LBRA     SETMODE1

        RTS

RETRYCM
        TST     VACLOAD         ; VACUUM LOADING ?
        LBRA    SETMODE1        ; NO VACUUM LOADING.
;        LBNE    SETMODE1        ; NO VACUUM LOADING.
;        LDAA    #VACLOADING     ; NOW DELAY TO ALLOW VAC LOADER TO FILL.
;        STAA    BATSEQ          ;
STILLFILL:

        RTS

;
;       CHECK FOR "FILL BY TIME" SERVICE.
;

BYTIME:
        CLRB
        LDE     CMPNTIM+1
        ADDE    #1
        STE     CMPNTIM+1
        ADCB    CMPNTIM
        STAB    CMPNTIM
        LDAB    PITCOM          ; READ COMP NO
        LDX     #CMP1TIMS       ; TIME STORAGE BLOCK
        DECB
        ABX
        ABX
        ABX
        LDAA    CMPNTIM         ; READ COUNTER
        CMPA    0,X             ; CHECK MSB
        BHI     SWCMPOFF        ; SWITCH COMPONENT OFF.
        BLO     LEAVEON         ; IF LOWER LEAVE ON.
        LDE     CMPNTIM+1       ; LSB
        CPE     1,X             ; COMPARE
        BHI     SWCMPOFF        ; IF HIGHER SWITCH COMPONENT OFF.
        BNE     LEAVEON         ; IF EQUAL SWITCH COMPONENT OFF.

SWCMPOFF:
        LDAB    PITCOM
        JSR     CMPNOFF         ; SWITCH OFF
        CLRW    CMPNTIM         ; CLEAR COUNTER
        CLR     CMPNTIM+2       ; RESET
        LBRA     SETMODE
        RTS

LEAVEON  LDAB    PITCOM          ; READ STORED COMP NO
        JSR     CMPNON          ; LEAVE O/P ON
        TST     WGT1SECAVG        ;  HAS THE A/D CONVERSION RUN ?
        BNE     YESAVAIL1        ;  YES AVAILABLE
        RTS
YESAVAIL1:
        CLR     WGT1SECAVG      ;  CLEAR AVAILABLE FLAG.
        JSR     CHECKCOMPONENTUSED
        TSTA                    ; WEIGHT CHECK NECESSARY ?
        LBNE    CHECKWEIGHT
        RTS


MBCMPNOFF:
        LDX     #LFILLSTATCMP1
        PSHB                    ; SAVE B
        DECB
        ABX
        PULB
        CLR     0,X             ; INDICATE THAT COMPONENT IS ON.
        LDX     #LFILLSTATCTR1
        PSHB                    ; SAVE B
        DECB
        ABX
        PULB
        CLR    0,X             ; INDICATE THAT COMPONENT IS ON.

CMPNOFF LDX     #FILLSTATCMP1
        PSHB                    ; SAVE B
        DECB
        ABX
        PULB
        LDAA    #0
        STAA    0,X             ; INDICATE THAT COMPONENT IS ON.
        STAA    FILLSTATUSALL

        LDY     #OFTABLE
        CLRA                    ; CODE FOR STATUS INDICATION
        CLR     FIRSTTIME       ; RESET FLAG.
        JSR     ENDCOUNTING     ; START COUNTER TO MEASURE OPENING TIME.
        LBRA    CON1

CMPNON  LDX     #LFILLSTATCTR1
        PSHB                    ; SAVE B
        DECB
        ABX
        PULB
        LDAA    #LFILLONTIME10HZ; FILL INDICATION ON TIME IN 10HZ INCREMENTS.
        STAA    0,X             ; INDICATE THAT COMPONENT IS ON.

MBCMPNON:
        LDX     #FILLSTATCMP1
        PSHB                    ; SAVE B
        DECB
        ABX
        PULB
        LDAA    #1
        STAA    0,X             ; INDICATE THAT COMPONENT IS ON.
        STAA    FILLSTATUSALL

        LDX     #LFILLSTATCMP1
        PSHB                    ; SAVE B
        DECB
        ABX
        PULB
        LDAA    #1
        STAA    0,X             ; INDICATE THAT COMPONENT IS ON.

        TST     AUTOCYCLETESTFLAG
        BNE     NOCYCLE         ; NO AUTO CYCLE.
        TST     AUTOCYCLEFLAG
        BEQ     NOCYCLE         ; NO AUTO CYCLE.
        LDE     RAW132+2
;        LDAA    BLENDERTYPE     ; CHECK BLENDER TYPE.
;        CMPA    #MEGATYPE       ; MEGABATCH BLENDER.
;        BEQ     LESTH4          ; MEGABLENDER
;        CMPB    #4
;        BLS     LESTH4
;        ADDE    #25
;        BRA     DOADD           ; DO ADDITION.
LESTH4  JSR     ADDRAWFORAUTOCYCLE ; ADD WEIGHT FOR AUTOCYCLE.
DOADD   STE     RAW132+2
        LDE     RAW132
        ADCE    #0
        STE     RAW132

NOCYCLE:

        TST     FIRSTTIME       ; FIRST TIME ?
        BNE     ISNOTF          ; NO
        LDAA    #1
        STAA    FIRSTTIME       ; SET FIRST TIME FLAG.
        JSR     STARTCOUNTING   ; START COUNTER TO MEASURE OPENING TIME.
        BRA     ISNOTF          ; COMPONENT ON

CMPNOFFONLY:
        LDY     #OFTABLE
        CLRA                    ; CODE FOR STATUS INDICATION
        BRA     CON1

CMPNONONLY:
ISNOTF  LDY     #ONTABLE        ; TABLE START ADDRESS
        LDAA    #1              ; STATUS CODE.
CON1
        TST     STDCCFG      ; CONFIGURATION.
        BEQ     STDCCF          ; STANDARD.
        PSHB                    ; SAVE COMPONENT NO.
        LDX     #COMP1CFG        ;
        DECB
        ABX
        ABX
        LDAB    #1              ; HOPPER NO COUNTER.
        LDE     0,X             ; READ COMPONENT INFORMATION.
        BEQ     STDCCFP         ; STANDARD CONFIGURATION.
        LDAA    #MAXCOMPONENTNO ; SHIFT COUNTER.
REPSHF  LSRE                    ; SHIFT COMPONENT DATA.
        BCC     NOACTIV         ; NO ACTIVATION.
        JSR     ACTCMPN         ; ACTIVATE COMPONENT ON/OFF.
NOACTIV INCB                    ; INCREMENT HOPPER
        DECA                    ; SHIFT COUNTER -1
        BNE     REPSHF
        PULB
        RTS
STDCCFP PULB
STDCCF  JSR     ACTCMPN         ; ACTIVATE HOPPER COMPARTMENT #N
        RTS


ADDRAWFORAUTOCYCLE:
        TST     ACTIVATELATENCY
        BNE     XITADD          ; NO VALVE LATENCY TIME.
        TST     TOPUPF           ; TOP UP ENABLED.
        BEQ     NOTPU            ; NOT TOP UP
        CMPB    #EIGHT           ; TOP UP COMPONENT
        BEQ     CMP1RAT
NOTPU   CMPB    #1
        BNE     NOT1            ; NO.
CMP1RAT ADDE    #COMPONENT1FLOWRATE ; COMPONENT FLOW RATE.
        BRA     XITADD          ; EXIT ADDITION.

NOT1    TST     STAGEFILLEN     ; STAGED FILLING
        BEQ     NOTMF1
        LDX     #STAGESEQ1
        LDAB    PITCOM          ; READ COMPONENT
        DECB
        ABX
        LDAA    0,X
        BEQ     SETS1
        CMPA    #THREE          ; STAGE ?
        BNE     NOTMF3          ; NO
SETS1   ADDE    #STAGE1MFILLRATE
        BRA     XITADD          ; EXIT

NOTMF3  LDAA    0,X
        CMPA    #TWO            ; STAGE ?
        BNE     NOTMF2          ; NO
        ADDE    #STAGE2MFILLRATE
        BRA     XITADD          ; EXIT

NOTMF2  LDAA    0,X
        CMPA    #ONE            ; STAGE ?
        BNE     NOTMF1          ; NO
        ADDE    #STAGE3MFILLRATE
        BRA     XITADD          ; EXIT


NOTMF1  LDAB    PITCOM
        CMPB    #2              ; COMPONENT 2.
        BNE     NOT2            ; NO.
        ADDE    #COMPONENT2FLOWRATE
        BRA     XITADD          ; EXIT ADDITION.
NOT2    CMPB    #3              ; COMPONENT 3.
        BNE     NOT3            ; NO.
        ADDE    #COMPONENT3FLOWRATE
        BRA     XITADD          ; EXIT ADDITION.
NOT3    CMPB    #4              ; COMPONENT 4.
        BNE     NOT4            ; NO.
        ADDE    #COMPONENT4FLOWRATE
        BRA     XITADD          ; EXIT ADDITION.
NOT4    ADDE    #COMPONENT4FLOWRATE
XITADD  RTS


; ACTIVATE HOPPER N (ACCB) FOR A PARTICULAR COMPONENT.
;

ACTCMPN PSHM    D,Y             ; SAVE REGISTERS.
        PSHB
        CLRB
        TBYK
        PULB                    ; RECOVER B
        ANDB    #$0F            ;
        DECB                    ;
        ASLB                    ;
        ABY                     ; ADDRESS
        LDY     0,Y             ; SERVICE ROUTINE
        JSR     0,Y
        PULM    D,Y             ; RECOVER REGISTER
        RTS



; VOLUMETRIC MODE OF OPERATION

VOLFILL:
        CLR     ANYON           ; RESET "ANYON" FLAG.
        LDAB    #RAMBANK
        TBZK
        LDZ     #SEQTABLE
REPVOL1:
        LDAB    0,Z
        ANDB    #$0F
        TBA
        LDX     #CMP1VCT        ; LOAD VOLUMETRIC COUNTER.
        DECB
        ABX
        ABX
        ABX
        LDAB    0,X
        LDE     1,X
        PSHB
        LDX     #CMP1TIMS       ; SET TIME.
        TAB
        DECB
        ABX
        ABX
        ABX
        PULB
        CMPB    0,X             ; AT PRESET COUNT YET.
        BNE     CMPON
        CPE     1,X             ; AT PRESET COUNT YET.
        BNE     CMPON
        TAB
        JSR     CMPNOFF         ;
        BRA     CONVL1          ; CONTINUE.
CMPON   TAB
        JSR     CMPNON          ; OFF
        LDAB    #$0F
        STAA    ANYON           ; SET FLAG.

        LDAB    0,Z
        ANDB    #$0F
        LDX     #CMP1VCT        ; LOAD VOLUMETRIC COUNTER.
        DECB
        ABX
        ABX
        ABX
        CLRB
        LDE     1,X
        ADDE    #1
        STE     1,X
        ADCB    0,X             ; INCREMENT VOLUMETRIC MODE.
        STAB    0,X


CONVL1  AIZ     #1
        LDAA    0,Z             ;
        CMPA    #SEQDELIMITER   ; AT DELIMITER.
        BNE     REPVOL1         ; REPEAT.
        TST     ANYON           ; ON
        BNE     XITVOL          ; EXIT
        CLR     ANYON           ; RESET FLAG.
        LDAA    #LEVPOLL
        STAA    BATSEQ
        LDX     #CMP1VCT
;        LDAB    #8*3
        LDAA    #THREE
        LDAB    #MAXCOMPONENTNO
        MUL
        TBA

        JSR     CLRBBS          ; RESET COUNTERS
XITVOL  RTS


; CHECK IF COMPONENT REQUIRES WEIGHT CHECK.
; IF >4 STD OR >6 MEGA NO WEIGHT CHECK REQUIRED.
;

CHECKCOMPONENTUSED:
        LDAB    PITCOM          ; READ COMPONENT NO.
        TST     STDCCFG      ; NON STANDARD COMPONENT CONFIG ?
        BNE     ISNONSTD        ; YES.
        LDAA    BLENDERTYPE     ; CHECK TYPE.
        CMPA    #MEGATYPE       ; MEGA BATCH
;        BEQ     ISMEGA          ; IS MEGA BATCH.
        BEQ     WTCK            ; IS MEGA BATCH.

        CMPB    #4              ; >4
        BRA     CMP
ISMEGA  CMPB    #6
CMP     BHI     NOWTCK          ; NO WEIGHT CHECK.
        BRA     WTCK            ; WEIGHT CHECK AND EXIT.
WTCK    LDAA    #1
        BRA     XITCCK          ; EXIT CHECK.
NOWTCK  CLRA                    ;
XITCCK  RTS


ISNONSTD:
        DECB                    ; DECREMENT COMPONENT NO.
        LDX     #COMP1CFG        ; CONFIG.
        ABX                     ; DETERMINE
        ABX                     ; DETERMINE
        LDAA    BLENDERTYPE     ; CHECK BLENDER TYPE.
        CMPA    #MEGATYPE       ; MEGABATCH
        BEQ     ISMEGA1         ; YES.
        LDD     0,X             ; READ COMPONENT NO
        ANDD    #$00F0          ; ANY COMPONENT >4
        BRA     DOCHK
ISMEGA1 LDD     0,X
        ANDD    #$00C0          ; ANY COMPONENT >6
DOCHK   BNE     NOWTCK          ; NO WEIGHT CHECK
        BRA     WTCK            ; ENABLE WEIGHT CHECK.



; GET THE LATENCY AND RETURN TIME IN D REG

GETLATENCY:
        TST     ADDR_HX
        BNE     NORML           ; READ LATENCY
        LDD     PASWRD          ;
        CPD     #$0065          ; SPECIFIC PASSWORD ?
        BNE     NORML           ; NORMAL LATENCY
        LDAA    PASWRD+2
        CMPA    #$54            ;
        BEQ     READLT          ; READ LATENCY

NORML   LDAA    BLENDERTYPE     ; READ BLENDER TYPE
        CMPA    #MINITYPE       ; MINI BLENDER
        BNE     NOTMINI         ; NO
        TST     STDCCFG         ; IS THIS A STANDARD BLENDER
        BEQ     ISSTDB          ; YES
        LDAB    PITCOM
        DECB                    ; DECREMENT COMPONENT NO.
        LDX     #COMP1CFG        ; CONFIG.
        ABX                     ; DETERMINE
        ABX                     ; DETERMINE
        LDD     0,X
        ANDD    #$03            ; SLIDES 1 OR 2 IN COMPONENT ?
        BEQ     SETSMLL         ; SET SMALL LATENCY
;;T
SETLGL:
        LDD     #THREEINCHLATENCY ; 3" VALVE LATENCY
        BRA     XITLAT          ; EXIT
SETSMLL LDD     #TWOINCHLATENCY ;
        BRA     XITLAT          ; EXIT
ISSTDB  LDAB    PITCOM
        CMPB    #TWO            ; HIGHER THAN 2ND COMP
        BHI     SETSMLL
        BRA     SETLGL          ; SET LARGE LATENCY

NOTMINI:
        CMPA    #TINYTYPE       ; MICRA BATCH ?
        BEQ     ISMICRA         ; YES
        CMPA    #BABYTYPE       ;
        BEQ     ISMICRA         ; M50 UNIT SELECTED
        CMPA    #MIDITYPE       ; MIDI BATCH
        BNE     NOTMIDI         ; NO
ISMICRA TST     STDCCFG         ; IS THIS A STANDARD BLENDER
        BEQ     ISSTMD          ; YES
        LDAB    PITCOM
        DECB                    ; DECREMENT COMPONENT NO.
        LDX     #COMP1CFG        ; CONFIG.
        ABX                     ; DETERMINE
        ABX                     ; DETERMINE
        LDD     0,X
        ANDD    #$03            ; SLIDES 1 OR 2 IN COMPONENT ?
        BNE     SETSMLL
SETVTU  LDD     #VENTURILATENCY
        BRA     XITLAT          ; EXIT


ISSTMD: LDAB    PITCOM
        CMPB    #TWO            ; HIGHER THAN 2ND COMP
        BHI     SETVTU          ; VENTURI FEEDERS


NOTMIDI LDD     #THREEINCHLATENCY
XITLAT  RTS

READLT
        LDD     LATENCY         ; READ LATENCY
        BRA     XITLAT          ; EXIT











*/
