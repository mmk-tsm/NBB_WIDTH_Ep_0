//////////////////////////////////////////////////////
// TenHz.c
//
// Group of functions that are called on 10Hz, on PIT.  equates to routines 
//  
// M.McKiernan                          24-11-2004
// First Pass
//
// M.McKiernan                          06-12-2004
// TenHzProgram() edited to incorporate all of all programs that need to run on the PIT 10hz
// CheckForBlowCycleSequence();  not doing anything in assembler, so commented out in C.
//
// M.McKiernan                          13-12-2004
// Comment only has changed. // CHKFORALARM:  C equivalent = CheckForAlarm(); has been 
// moved to foreground. - see ForegroundOneHzProgram()
//
// P.Smith                              29-03-2005
// added analyse rotation detector ANALYSEROTDETECTOR inline
//
//
// P.Smith                              7-04-2005
// added ScanPauseOffInput
//
// P.Smith                              12-08-2005
// added increment of g_nVacuumTimer
// added CheckForMixerOff();
//
//
// P.Smith                              15-08-2005
// Uncomment checkfor60kgmixeron
// Remove check for babytype in checkfor60kgmixeron, this allows intermittent mixing#
// to operate for all types.
//
//
// P.Smith                              14-09-2005
// name change g_nForegroundTenHzFlag to g_nForegroundTenHzFlag
//
// P.Smith                              15/11/05
// exit CheckFor60KgMixerOn MICRABATCH
// 
// P.Smith                      9/2/06
// first pass at netburner hardware conversion.
// Bool -> BOOL
// #include <basictypes.h>
// remove //nbb #include "Hardware.h"
// removed all unused header files.
// comment out 
// CheckForCleanJetOnOff( );
// TotaliseCount();                // total GPT timer counts for open valve.
// CheckForCompStatusOff( );   // decrementing "long fill" status counters for each component.
// DecrementMBLags( );         // decrement lags associated with Modbus (see MBProgs.c)
// SEIIncreaseDecreaseLampsOnOff(); //SWITCHSEILAMPONOFF
// CheckForMixerOff();
// PauseOff();                 // pause off
// RemoveAlarmTable( ATTARWTALARM,  0 );     // RMALTB
// CheckForMixerOff();
// CheckForMixerOff();
// #include "pause.h"  add back in PauseOff();
// 
// P.Smith                      21/2/06
// implement rwsei.cpp todos added FrontRollChange(); BackRollChange() 
// call SEIIncreaseDecreaseLampsOnOff()
// #include    "Rwsei.h"
// 
// P.Smith                      28/2/06
// DecrementMBLags( );         // decrement lags associated with Modbus (see MBProgs.c)
// #include    "MBProgs.h"
// CheckForMixerOff();
// #include    "FgOneHz.h"
// CheckForCompStatusOff( );   // decrementing "long fill" status counters for each component.
// #include    "MBProSum.h"
// 
// P.Smith                      19/4/06
// CheckForCleanJetOnOff( ); called
// RemoveAlarmTable( ATTARWTALARM,  0 ); called
// #include "Cleanio.h"
// #include    "Alarms.h"
//
// P.Smith                      30/6/06
// call TotaliseCount();                // total GPT timer counts for open valve.
//
// P.Smith                      16/11/06
// remove unused commented out code.
//
// P.Smith                      22/6/07
// call CheckForCatchBoxVibration
// correct blender name types
//
// P.Smith                      4/7/07
// added decremenet of g_nOptimisationFlapDebounceCounter
//
// M.McKiernan                      17/9/07 
// Removed g_arrnMBTable[] & g_arrnMBTableFlags[], now in BatVars2
//
// P.Smith                        19/12/07 
// if g_IncreaseOutputOn10hzCounter non zero, decrement and initiate inc lamp on.
// if g_DecreaseOutputOn10hzCounter non zero, decrement and initiate dec lamp on
//
//
// P.Smith                       10/1/08 
// if g_nAutocycleLevelSensorOff10hzCounter non zero, decrement 
//
// P.Smith                       7/7/08 
// add stop to run and run to stop transition to the log file.
// do not update unless it is the stop / run or run / stop transition.
// remove reference to micrabatch
//
// P.Smith                          23/7/08
// remove g_arrnMBTable extern 
//
// P.Smith                          21/10/08
// remove RunStop
//
// P.Smith                          9/12/08
// increment g_sOngoingHistoryEventsLogData.m_nHistoryDoorOpenings when door is open
//
// P.Smith                          5/3/09
// name change to m_nDoorOpenings
//////////////////////////////////////////////////////

//*****************************************************************************
// INCLUDES
//*****************************************************************************
//nbb #include <hc16.h>
//nbb #include <Stdio.h>
//nbb #include <StdLib.h>

//nbb #include "Hardware.h"
#include "InitialiseHardware.h"
#include "SerialStuff.h"
#include "General.h"
#include "ConversionFactors.h"

#include "BatchCalibrationFunctions.h"
#include "BatchMBIndices.h"
#include "ConfigurationFunctions.h"
#include "16R40C.h"
#include    "FiftyHz.h"
#include    "BatVars.h"
#include    "Pause.h"
#include    "MBProSum.h"
#include    "MBProgs.h"
#include "Cleanio.h"
#include    "TenHz.h"
#include    "Rwsei.h"
#include    "Batvars2.h"
#include    "Alarms.h"
#include    "FgOneHz.h"
#include <basictypes.h>
#include    "Pitweigh.h"
#include    "Vaccyc.h"
#include <stdio.h>
#include "Eventlog.h"
#include "Historylog.h"


// Locally declared global variables
// Externally declared global variables
// Data
extern CalDataStruct    g_CalibrationData;
//extern float  g_fWeightAtoDCounts;

extern  int g_nKeyCode;
extern  int g_nLanguage;
extern  int g_nGeneralSecondTimer;
extern  structMeasurementUnits  g_MeasurementUnits;

extern  int g_nDisplayScreen;

int     g_nAutoCycleSymbolCtr;
int     g_nAutoCycleSymbolPeriodCtr;

unsigned char       g_cAutoCycleSymbol;

#define AUTOCYCLE_SYMBOL_PERIOD 4
/*
TENHZHAND:
        JSR     DECELAG         ; DECREMENT SENSOR LAG COUNTER.
        JSR     PIT10HZ         ; CALL PIT 10HZ TO CALCULATE LINE SPEED
        JSR     ARI005          ; ORIGIONAL 10HZ CALL FOR LOW BLENDER
;        JSR     POLLHA          ; POLL HA LEVEL SENSORS.
        RTS 
*/

/////////////////////////////////////////////////////
// TenHzHandler()                       from ASM = TENHZHAND
//
// Call all programs that need to run on the 10hz
// NB: This is called on PIT interrupt - need to keep efficient.
//
// Michael McKiernan                        24.11.2004
// First Pass.
//////////////////////////////////////////////////////
void TenHzProgram( void )
{
    int i;

    if(g_nAutocycleLevelSensorOff10hzCounter != 0)
    {
        g_nAutocycleLevelSensorOff10hzCounter--;
    }

    if(g_IncreaseOutputOn10hzCounter != 0)
    {
        g_IncreaseOutputOn10hzCounter--;
        if(g_IncreaseOutputOn10hzCounter == 0)
        {
            SEIIncreaseLampOffFlag();
        }
        else
        {
            SEIIncreaseLampOnFlag();
        }
    }
    if(g_DecreaseOutputOn10hzCounter != 0)
    {
        g_DecreaseOutputOn10hzCounter--;
        if(g_DecreaseOutputOn10hzCounter == 0)
        {
            SEIDecreaseLampOffFlag();
        }
        else
        {
            SEIDecreaseLampOnFlag();
        }
    }

    for(i=0; i < MAX_COMPONENTS; i++)
    {
        if(g_nOptimisationFlapDebounceCounter[i] != 0)
        {
            g_nOptimisationFlapDebounceCounter[i]--;   
        }
    }

   CheckForCatchBoxVibration();
// DECELAG - expanded inline
// EDMPLAG was not being used for anything.
    if( g_nHGSen1Lag )      // HGSEN1LAG
        g_nHGSen1Lag--;

//JSR     ARI005          ; ORIGIONAL 10HZ CALL FOR LOW BLENDER
// ARI005 expanded here:
    if( g_cBatSeq == BATSEQ_LEVPOLL || g_bLevSen1Stat == COVERED )
    {
        g_nPremLSCtr = 0;       // premature level sensor
    }
    else
    {
        g_nPremLSCtr++;     //
    }

        g_nDmpSec++;                    // DMPSEC from assembler incremented on 10Hz.
        g_nVacuumTimer++;
    
// TSECT3 - not being used for anything

// end of ARI005

// PIT10HZ Programs.
    CheckFor60KgMixerOn();  // CHECKFOR60KGMIXERON

    CheckForCleanJetOnOff( );

//CheckForBlowCycleSequence();  not doing anything in assembler.        JSR CHECKFORBLOWCYCLESEQ - converted below, but not called.

    ScanPauseOffInput();        // READPAUSEOFFINPUT

    CheckSlideInterval();           // CHECKSLIDEINTERVAL 

// --REVIEW-- CHECKFORVIBRATIONACT - not doing anything in assembler.  CloseFeed had been commented out.

    TotaliseCount();                // total GPT timer counts for open valve.

    CheckForCompStatusOff( );   // decrementing "long fill" status counters for each component.

    g_bForegroundTenHzFlag = TRUE;   // set 10Hz flag for foreground. loosely equates to TENHZF1

    DecrementMBLags( );         // decrement lags associated with Modbus (see MBProgs.c)

    SEIIncreaseDecreaseLampsOnOff(); //SWITCHSEILAMPONOFF
    


    if( (g_nTenthsOfSecondCounter2 & 0x0001) == 0 ) // LSTTHS
        g_bSEIPollIncDecFlag = TRUE;                        // SEIPOLLINCDECF - Trigger polling of inc/dec inputs

// --REVIEW-- could move this to foreground!!
    if( g_bMixOff )
        MixerOff();
    else
        MixerOn();
//CONP10
// CHKFORALARM:  C equivalent = CheckForAlarm(); has been moved to foreground. - see ForegroundOneHzProgram()

//--REVIEW--        The "on" condition for this output (open collector) is low, it can sink current in the
//                      Low state.  It cannot source current in the high state. 
//  Logic kept as per assembler.
    if( g_nCMRValidTimer )      // CMRVALID
    {
        g_nCMRValidTimer--;
        SBBSSROutputOff();          // SSROPHIGH -drive the SSR output on the SBB. (CONN12)
    }
    else
    {
        SBBSSROutputOn();           // SSROPLOW  - 
    }

// TENCNT was not being used for anything is asm. 
    g_bRemoteKeyPoll = TRUE;            // REMKEYPOLL

    g_nTenthsOfSecondCounter2++;        // LSTTHS
    if( g_nTenthsOfSecondCounter2 == 10 )
    {                                                   //This section is running at 1Hz.
        g_nTenthsOfSecondCounter2 = 0;
        g_bFGSecondFlag2 = TRUE;            // set flag to call all ForegroundOneHzProgram2(), 
                                                    // stuff between "LDD DA1TMP" and ARI001EXIT.

        g_nSecondCounter2++;        // LSSECS
        if(g_nSecondCounter2 == 60)
        {                                       //This section is running once per minute.
            g_nSecondCounter2 = 0;
            g_bFGOneMinuteFlag = TRUE;          // set flag to call all FGOneMinuteProgram() - stuff between 
                                                // "NOTATPRESET: and NOT1MIN:

        }   // end of per minute section.
    }   //end of per second section.

// --REVIEW-- AMDBNC not being used anywhere in assembler.
/*
NTLSEC  TST    AMDBNC            ;~AUTO/MANUAL DEBOUNCE TIMER
        BEQ    NODEBNC
        DEC    AMDBNC
NODEBNC:
*/

    ReadAllSBBIO_Inputs( );     // test purposes.

    UpdateAutoCycleSymbol( );   // Display purposes only.

// This has been taken from the main PIT routine.  But timing is same, i.e. DO_DIAGNOSTICS_PERIOD tailored for 10Hz.
    g_nDoDiagnosticsCounter++;
    if(g_nDoDiagnosticsCounter > DO_DIAGNOSTICS_PERIOD)
    {
            g_nDoDiagnosticsCounter = 0;            // DIAGCOUNTER 
            if( g_bOutputDiagMode )                 // only in output diagnostics mode. OUTPUTDIAG
                g_bDoDiagnosticsFlag = TRUE;        // set flag to call DoDiagnostics() in foreground.
    }

}
/*
PIT10HZ:                                ;PIT 10 Hz ROUTINES
       JSR      CHECKFOR60KGMIXERON
       JSR      CHECKFORCLEANJETONOFF   ; CHECK IF CLEANING JETS SHOULD BE SWITCHED ON / OFF
       JSR      CHECKFORBLOWCYCLESEQ
       JSR      READPAUSEOFFINPUT       ; READ INPUT ACTIVATED TO SWITCH PAUSE OFF
       JSR      CHECKSLIDEINTERVAL
       JSR      CHECKFORVIBRATIONACT    ; VIBRATION ACTUATION.
       JSR      TOTALISECOUNT           ; TOTALISE COUNT
       JSR      CHECKFORCOMPSTATUSOFF   ; CHECK IF COMPONENT STATUS SHOULD BE SWITCHED OFF.
       LDAA     #1
       STAA     TENHZF1                 ; 10 HZ FLAG.
       JSR      MBLAGS            ; ROUTINE FOR LAGS IN MODBUS PROG.

*/
/*
TENHZHAND:
        JSR     DECELAG         ; DECREMENT SENSOR LAG COUNTER.
        JSR     PIT10HZ         ; CALL PIT 10HZ TO CALCULATE LINE SPEED
        JSR     ARI005          ; ORIGIONAL 10HZ CALL FOR LOW BLENDER
;        JSR     POLLHA          ; POLL HA LEVEL SENSORS.
        RTS 
       
ARI005 LDAA     BATSEQ
       CMPA     #LEVPOLL
       BEQ      LS1ISON         ; NORMAL MODE OKAY.
       TST      LEVSEN1STAT
       BEQ      LS1ISON
       INCW     PREMLSCTR
       BRA      SKIPRS
LS1ISON CLRW    PREMLSCTR
SKIPRS  INCW   DMPSEC            ; SECONDS COUNTER FOR DUMPING.
;       JSR    SNDD_A            ; SEND DATA FOR ON-BOARD D/A'S
       JSR     RUNSTOP         ; CHECK RUN STOP INPUT.

ARI006 TST    TSECT3            ;~KEY DEBOUNCE COUNTER
       BEQ    ARI007            ;~0, CONTINUE ARI LEAT
       DEC    TSECT3            ;~ELSE DECREMENT COUNTER ON 10HZ INTERRUPT
ARI007:                         ; PULB ;~RESTORE TENCNT VALUE
        RTS
;

*/
/////////////////////////////////////////////////////
// RunStop()                        from ASM = RUNSTOP
//
// Reads the state of Stop/Run switch and debounces it
// Called on 10Hz.
//
// M.McKiernan                          29-07-2004
// First pass.
//////////////////////////////////////////////////////
void RunStop( void )
{

BOOL bState;
    bState = ReadStopRun(); // read state of Run/stop switch.
    if(bState == RUN)
    {
        g_nStopCounter = 0;
        g_nRunCounter++;
        if( g_nRunCounter > RUN_STOP_FILTER )
        {
            if(g_bStopped)  //stopped flag set?? I.e is there a change?
            {
                CheckForMixerOff();
                g_bMixOff = FALSE;  // clear flag to allow mixer to run.
                g_bRunTransition = TRUE;    // stop -> run
                g_bStopRunDisplayEnable = TRUE;
            }

            if(g_bStopped)
            {
                AddBlenderStopToRunEventToLog();
            }
            g_bStopped = FALSE;
            g_nRunCounter = 0;
        }
    }
    else
    {  //stop
        g_nRunCounter = 0;
        g_nStopCounter++;
        if( g_nStopCounter > RUN_STOP_FILTER )
        {
            g_nStopCounter = 0;
            if(!g_bStopped) //stopped flag not set??
            {
                AddBlenderRunToStopEventToLog();
                g_sOngoingHistoryEventsLogData.m_nDoorOpenings++;
                g_bStopped = TRUE;
                g_bStopTransition = TRUE;
                g_bStopRunDisplayEnable = TRUE;
                g_nEstCtrUse = NO_OF_ESTIMATED_CYCLES;  
            }
        }
    }
}


/////////////////////////////////////////////////////
// UpdateAutoCycleSymbol
//
// Updates the symbol to be used to show in Autocylce mode.
// Called on 10Hz.
//
// M.McKiernan                          29-07-2004
// First pass.
//////////////////////////////////////////////////////
void UpdateAutoCycleSymbol( void )
{
  if(g_bAutoCycleFlag)
  {
        g_nAutoCycleSymbolPeriodCtr++;
        if(g_nAutoCycleSymbolPeriodCtr > AUTOCYCLE_SYMBOL_PERIOD)
        {
            g_nAutoCycleSymbolPeriodCtr = 0;

            g_nAutoCycleSymbolCtr++;
            if(g_nAutoCycleSymbolCtr > 3)
                g_nAutoCycleSymbolCtr = 0;          // Ctr goes from 0 - 3, -> 0...

                switch( g_nAutoCycleSymbolCtr )
                {
                case 0:
                    g_cAutoCycleSymbol = 0x41;      //A
                    break;
                case 1:
                    g_cAutoCycleSymbol = 0x61;      //a
                    break;
                case 2:
                    g_cAutoCycleSymbol = 0x41; //   A        0x8E;      //A with 2 dots
                    break;
                case 3:
                    g_cAutoCycleSymbol = 0x61;  // a            0x83;       //a with 2 dots
                    break;
                default:
                    break;
                }
        }
 }  
 else
    g_cAutoCycleSymbol = ASCII_BLANK;
}

/*
/////////////////////////////////////////////////////
// CheckForBlowCycleSequence        ASM = CHECKFORBLOWCYCLESEQ
//
// Caters for timers in cleaning mode.
// Called on 10Hz.
//
// M.McKiernan                          30-11-2004
// First pass.
//////////////////////////////////////////////////////
void CheckForBlowCycleSequence( void )
{
    if(g_nRightCornerCleanOnInXCounter)     // RRCCONINCTR
    {
        g_nRightCornerCleanOnInXCounter--;
        if(g_nRightCornerCleanOnInXCounter == 0)    // has it reached 0??
            g_nRightCornerCleanOnCounter = BLOW_CLEAN_TIME;
    }

    if(g_nMixCleanOnInXCounter) // MIXCLEANONINCTR
    {
        g_nMixCleanOnInXCounter--;
        if(g_nMixCleanOnInXCounter == 0)    // has it reached 0??
            g_nAirJet2OnCounter = BLOW_CLEAN_TIME;
    }
}
*/

/*
CHECKFORBLOWCYCLESEQ:
        TST     RRCCONINCTR     ; RCC ON
        BEQ     NORCC           ; NO
        DEC     RRCCONINCTR   ; DEC CTR
        BNE     NORCC           ; NO = 0
        LDD     #BLOWCLEANTIME
        STD     CCRONCTR
        RTS
NORCC   TST     MIXCLEANONINCTR
        BEQ     NOMC            ; NO
        DEC     MIXCLEANONINCTR
        BNE     NOMC            ; NON ZERO
        LDD     #BLOWCLEANTIME
        STD     AIRJET2ONCTR    ; AIR JET 1 COUNTER ONE TIME
NOMC    RTS
*/

/////////////////////////////////////////////////////
// CheckSlideInterval()     ASM = CHECKSLIDEINTERVAL
//
// Caters for slide opening.  Purely a diagnostic or test feature.  Not for normal machine running.
// The purge component no. is used for the frequency. (in 10Hz)
// The line no. is used for the time period. (in PIT periods).
// See CHECKFORSLIDECYCLE which is expanded inline in the PIT interrupt handler. PITHand.c.
// Called on 10Hz.
//
// M.McKiernan                          30-11-2004
// First pass.
//////////////////////////////////////////////////////
void CheckSlideInterval( void )
{
    g_nTinySlideCycleCounter++;
    if( g_nTinySlideCycleCounter > g_CalibrationData.m_nPurgeComponentNumber )
    {
        g_nTinySlideCycleCounter = 0;   // reset counter
        g_nTinySlideOpenTimeCounter = g_CalibrationData.m_nLineNumber;  //LINENO
    }
}
/*g_nTinySlideOpenTimeCounter
CHECKSLIDEINTERVAL:
        INC     TINYSLIDECYCLECOUNTER   ; SLIDE CYCLE COUNTER.        
        LDAB    PURGECOMPNO             ; CYCLE FREQUENCY.
        JSR     BCDHEX1                 ; BCD TO HEX.
        CMPB    TINYSLIDECYCLECOUNTER   ;
        BHI     NOTATF                  ; NOT AT FREQ
;       TST     CYCLEINDICATE           ; IN MIDDLE OF CYCLE.
;       BNE     NOTATF                  ; YES
        CLR     TINYSLIDECYCLECOUNTER   ; RESET COUNTER.
        LDD     LINENO                  ; READ ON TIME
        JSR     BCDHEX2                 ; BCD TO HEX.
        STD     TINYSLIDEOPENTIMECOUNTER ; OPEN TIME COUNTER.
NOTATF           
;        INC     VIBRATIONOFFCYCLECOUNTER; INCREMENT OFF COUNTER
;        LDAB    LSC1+1                  ; READ LINE SPEED DATA.
;        JSR     BCDHEX1                 ; BCD TO HEX.
;        CMPB    VIBRATIONOFFCYCLECOUNTER;
;        BHI     NOTATV                  ; NOT AT FREQ
;        CLR     VIBRATIONOFFCYCLECOUNTER; RESET COUNTER.
;        LDAB    LSC1+2                  ; READ ON TIME     
;        JSR     BCDHEX1                 ; BCD TO HEX.
;        STAB    VIBRATIONONTIMECOUNTER  ; OPEN TIME COUNTER.                                        
NOTATV  RTS

*/


/////////////////////////////////////////////////////
// ScanPauseOffInput()     ASM = READPAUSEOFFINPUT
//

// P.Smith                          6-04-2005
//////////////////////////////////////////////////////

void ScanPauseOffInput( void )
{
BOOL    bState;
    bState = ReadPauseOffInput();     
    if(bState)
    {
        g_bPauseOffActive = TRUE;
        g_nPauseOffActiveCounter++;     // increment pause off counter
        if(g_nPauseOffActiveCounter >= PAUSEOFFACTIVENO)
        {
            g_nPauseOffActiveCounter = 0;
            g_bNoTargetAlarm = TRUE;
            PauseOff();                 // pause off
            g_bOfflineBatchFilled = FALSE;
            if(g_arrnMBTable[BATCH_SUMMARY_PROCESS_ALARMS] & ATTARWTALARMBIT) // bit set?
            {  
                        RemoveAlarmTable( ATTARWTALARM,  0 );     // RMALTB
            }
            StartUpOff();   // --review-- allocate another output for this when available
            g_bPauseOffActive = FALSE;    

        }
        else
        {
            g_bPauseOffActive = FALSE;    
        }
    }
    else
    {
        g_bPauseOffActive = FALSE;    
    }
}

/*
READPAUSEOFFINPUT:
        JSR     READOI2CONN60
        BEQ     NOTAC                   ; PAUSE OFF NOT ACTIVE
        LDAA    #1
        STAA    PAUSEOFFACTIVE          ; PAUSE OFF FLAG ACTIVE
        INC     POACTIVECTR             ; PAUSE OFF ACTIVE
        LDAA    POACTIVECTR
        CMPA    #POACTIVENO             ; AT PRESET
        BLO     NOTAC
        CLR     POACTIVECTR             ; 
        LDAA    #1
        STAA    NOTARGETALARM           ; NO TARGET ALARM IN FUTURE
        JSR     PAUSEOFF                ; INPUT ACTIVE SWITCH PAUSE OFF.
                          
        LDAA    MBPROALRM+1               ; 
        ANDA    #ATTARWTALARMBIT        ; AT TARGET WEIGHT BIT SET.
        BEQ     ALCLR                   ; AT TARGET ALREADY ACTIVE.
        LDAA    #ATTARWTALARM           ; AT TARGET WEIGHT ALARM.
        JSR     RMALTB                  ; GENERATE ALARM.
ALCLR   JSR     STARTUPOFF              ; RESET AT TARGET WEIGHT INDICATION.
NOTAC   CLR     PAUSEOFFACTIVE
        RTS        
        

*/





/////////////////////////////////////////////////////
// CheckFor60KgMixerOn()     ASM = CHECKFOR60KGMIXERON
//
//
// P.Smith                          15-08-2005
//////////////////////////////////////////////////////

void CheckFor60KgMixerOn( void )
{
    if(g_nCleanCycleSeq == 0)  // not cleaning
    {
        if(g_CalibrationData.m_nIntermittentMixTime != 99)
        {
                if(g_nMixerOnCounter10Hz)
                {
                    g_nMixerOnCounter10Hz--;
                    CheckForMixerOff();
                    g_bMixOff = FALSE;  // clear flag to allow mixer to run.
                }
                else
                {  // MXOFF
                    g_bMixOff = TRUE;  // clear flag to allow mixer to run.
                }
        }
    } 
    else
    { //LEVON
        CheckForMixerOff();
        g_bMixOff = FALSE;  // clear flag to allow mixer to run.
    }

}




/*
CHECKFOR60KGMIXERON:
        TST     CLEANCYCLESEQ   ; CLEAN CYCLE ?
        BNE     LEVON           ; LEAVE MIXER ON.
        LDAB    INTMIXTIME
        JSR     BCDHEX1
        CMPB    #99
        BEQ     NOTBBT          ; LEAVE MIXER ON.
        
        LDAA    BLENDERTYPE
        CMPA    #BABYTYPE
        BNE     NOTBBT          ; NO
        TSTW    MIXERONCTR10HZ
        BEQ     MXOFF           ; NO
        DECW    MIXERONCTR10HZ
LEVON   JSR     CHECKFORMIXEROFF
        CLR     MIXOFF          ; MIXER IS ON
NOTBBT  RTS        
MXOFF   LDAA    #1
        STAA    MIXOFF
        BRA     NOTBBT
*/

