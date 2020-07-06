// **********************************************************
//   Pause.c                                                *
//    Pause programs for Batch blender          *
//   
//   Latest Changes:                18.Nov.2004             *
//                                                              *
//   M.McKiernan                   18-11-2004                   *
//      First Pass
//                                                              *
//   M.McKiernan                   28-11-2004                   *
//      Removed Pause definitions to General.h.
//  
// P.Smith                          4-3-2005
// Remove comments from ToManual
//  
// P.Smith                          1-4-2005
// Remove comments from StartMouldClean()
//
// P.Smith                          29/09/05        
// check if g_bMultiplefilling is set in CheckForEndOfCyclePause
// no pause unless cleared.
//
// P.Smith                      9/2/06
// first pass at netburner hardware conversion.
// #include <basictypes.h>
// comment out 
// ToManual();
// MakeCycle( );                   
// StartMouldClean();  // STARTMOULDCLEAN
//
// P.Smith                      20/2/06
// implement cleanio.cpp functions,put back StartMouldClean();  // STARTMOULDCLEAN
// #include "Cleanio.h"
//
// P.Smith                      28/2/06
// ToManual();
// #include "Conalg.h"
//
// P.Smith                      19/4/06
// implement MakeCycle( ); call
// #include "CycleMonitor.h"
//
//
// P.Smith                      29/5/06
// SwitchPauseOffAndMakeCycle()          ASM = SWITCHPAUSEOFF
//
// P.Smith                      30/6/06
// call  StartMouldClean();
//
// P.Smith                      16/11/06
// name change g_nCycleIndicate -> g_bCycleIndicate
// remove unused commented out code.
//
// P.Smith                      27/8/08
// Remove cycle when blender is unpaused, this is a  problem if the there is material in 
// the mixing chamber and the blender cannot dump the material, recults in a tare alarm - sbb--todo--
//
// P.Smith                      26/9/08
// in SwitchPauseOffAndMakeCycle set g_bLiquidControlAllowed to true to allow pib control to
// resume.
//
// P.Smith                      12/1/09
// make a copy of the g_nCyclePauseType so  that the pause type used can be checked.
// this was used in conjunction with the blender being restarted after pausing.
// if eoc pause type is end of cycle, in SwitchPauseOffAndMakeCycle, make the blender 
// cycle again.
//
// P.Smith                      12/5/09
// in SwitchPauseOffAndMakeCycle, make the blender cycle if g_nCyclePauseTypeCopy is set to
// OFFLINE_END_OF_CYCLE_PAUSE_TYPE
// set g_bNoLeakAlarmCheck to disable leak alarm on next cycle
/// **********************************************************

//*****************************************************************************
// INCLUDES
//*****************************************************************************
//nbb #include <stdlib.h>
#include <stdio.h>

#include "SerialStuff.h"
//nbb #include "Hardware.h"
#include "BatchMBIndices.h"
#include "BatchCalibrationFunctions.h"
#include "General.h"
#include "ConversionFactors.h"

#include "16R40C.h"
#include "ConfigurationFunctions.h"
#include "MBMHand.h"
#include "MBSHand.h"
#include "SetpointFormat.h"
#include "BatVars.h"
#include "CycleMonitor.h"
#include "InitialiseHardware.h"
#include "Pause.h"
#include "Conalg.h"
#include "Cleanio.h"
#include "Batvars2.h"

#include <basictypes.h>

//////////////////////////////////////////////////////
// PauseToggle()            ASM = PAUSETOGGLE
//
// Toggle the pause mode from on to off and v.v.
// 
// 
//
// M.McKiernan                          15-11-2004
// First Pass   
// 
//////////////////////////////////////////////////////
void PauseToggle( void )
{
    if( g_nPauseFlag )
    {
        PauseOff(); // Pause exit.
    }
    else
    {
        PauseOn();  // Pause blender
    }
}


//////////////////////////////////////////////////////
// PauseOn()            ASM = PAUSEON
//
// Turn the pause mode on
// 
// 
//
// M.McKiernan                          17-11-2004
// First Pass   
// 
//////////////////////////////////////////////////////
void PauseOn( void )
{
    g_bDmpLevSen = FALSE;       // Reset level sensor flag
    g_nPauseFlag |= NORMAL_PAUSE_BIT;
    g_bPauseTransition = TRUE;
    ToManual();
}

//////////////////////////////////////////////////////
// ImmediatePauseOn()           ASM = IMDPAUSEON
//
// Turn the immediate pause mode on
// 
// 
//
// M.McKiernan                          17-11-2004
// First Pass   
// 
//////////////////////////////////////////////////////
void ImmediatePauseOn( void )
{
    g_nCyclePauseType = IMMEDIATE_PAUSE_TYPE;       // IMMEDIATE PAUSE.
    g_nCyclePauseTypeCopy = IMMEDIATE_PAUSE_TYPE;
    PauseOn(); 
}
//////////////////////////////////////////////////////
// OffLinePauseOn()         ASM = OFFLINEPAUSEON
//
// Turn the immediate pause mode on
// 
// 
//
// M.McKiernan                          17-11-2004
// First Pass   
// 
//////////////////////////////////////////////////////
void OffLinePauseOn( void )
{
    g_nCyclePauseType = OFFLINE_END_OF_CYCLE_PAUSE_TYPE;
    g_nCyclePauseTypeCopy = OFFLINE_END_OF_CYCLE_PAUSE_TYPE;

    g_bDmpLevSen = FALSE;       // Reset level sensor flag
    g_nPauseFlag |= OFFLINE_END_OF_CYCLE_PAUSE_BIT;  //OFFLINEPAUSEATENDOFCYCLEBIT ; OFFLINE PAUSE
    g_bPauseTransition = TRUE;
    ToManual();
}

/*
OFFLINEPAUSEON:
        LDAB    #OFFLINEPAUSEATENDOFCYCLEBIT ; OFFLINE PAUSE
        BRA     CONPON                  ; CONTINUE
CONPON  CLR     DMPLEVSEN       ; RESET LEVEL SENSOR FLAG
        ORAB    PAUSFLG                 ; SET PAUSE FLAG.
        STAB    PAUSFLG        
        STAB    PAUSETRAN
        JSR     TOMANUAL        ; SWITCH TO MANUAL MODE.
XITP    RTS
*/

//////////////////////////////////////////////////////
// EndOfCyclePauseOn()          ASM = ENDOFCYCLEPAUSEON
//
// Turn the immediate pause mode on
// 
// 
//
// M.McKiernan                          18-11-2004
// First Pass   
// 
//////////////////////////////////////////////////////
void EndOfCyclePauseOn( void )
{
    g_nCyclePauseType = END_OF_CYCLE_PAUSE_TYPE;
    g_nCyclePauseTypeCopy = END_OF_CYCLE_PAUSE_TYPE;
    
    if( !g_bCycleIndicate )
    {
        CheckForEndOfCyclePause();      // CHKFOREOCPAUSE          ; CHECK FOR PAUSE
    }
}

/*
ENDOFCYCLEPAUSEON:
        LDAA    #PAUSEATENDOFCYCLE      ; PAUSE AT END OF CYCLE.
        STAA    CYCLEPAUSETYPE          ; SET PAUSE TYPE.
        TST     CYCLEINDICATE           ; IN CYCLE ?
        BNE     NOIMPA                  ; NO IMMEDIATE PAUSE
        JSR     CHKFOREOCPAUSE          ; CHECK FOR PAUSE
NOIMPA  RTS

*/
//////////////////////////////////////////////////////
// PauseOff()           ASM = PAUSEOFF
//
// Turn the pause mode off
// 
// 
//
// M.McKiernan                          17-11-2004
// First Pass   
// 
//////////////////////////////////////////////////////
void PauseOff( void )  //nbb--todolp-- this program seems to be redundant. nbb--sbbswchange--
{
int nTemp;

    g_bPauseToNormal = TRUE;        // Pause to Normal flag
    g_bBatchReady = FALSE;  // reset batch ready and allow blender to continue on.
    nTemp = ANY_PAUSE_BITS;
    nTemp ^= 0xFFFF;            // get complement for "anding"
    g_nPauseFlag &= nTemp;  // i.e. clear pause bits (not accounting for VAC_INHIBIT
    g_nCyclePauseType = 0;
    if(g_bByPassMode == TRUE)   // asm = BYPASS
    {
        g_bByPassMode = FALSE;      //   ; BACK TO NORMAL MODE.
        CloseBypass( );
        MakeCycle( );                   // 
     //MAKEDUMP: - inline statements in C
     g_nDmpSec = 0; //nbb--todo-- this is dangerous to do this as the counter is used in cycle.
     // set Sequence to Dump.
     g_cBatSeq = BATSEQ_DUMP;

    }
}

//////////////////////////////////////////////////////
// CheckForEndOfCyclePause()            ASM = CHKFOREOCPAUSE
//
// Turn the pause mode off
// 
// 
//
// M.McKiernan                          18-11-2004
// First Pass   
// 
//////////////////////////////////////////////////////
void CheckForEndOfCyclePause( void )
{
int nTemp;
    nTemp = g_nCyclePauseType;

    if( g_nCyclePauseType && !g_bMultipleFilling )
    {
        if(g_bPauseOnActive)    // PAUSEONACTIVE    ; PAUSE ON FLAG ACTIVE (comes from hardware input)
        {       // asm = ISEXPA
            PauseOn();
            CloseFeed();
            CloseDump();
            // --REVIEW-- this was not in asm, but think the external pause input should have same functionality.
         if(g_bCleanAtEndOfCycle)
         {
            g_bCleanAtEndOfCycle = FALSE;
            StartMouldClean();
         }
        }
        else
        {
            g_nCyclePauseType = 0;
            if( nTemp == OFFLINE_END_OF_CYCLE_PAUSE_TYPE )
            {
                OffLinePauseOn();
                OpenFeed();
                OpenDump();
                g_bNoLeakAlarmCheck = TRUE;            // indicate no leak alarm check required
                if(g_bCleanAtEndOfCycle)
                {
                    g_bCleanAtEndOfCycle = FALSE;
                    StartMouldClean();  // STARTMOULDCLEAN
                }
            }
            else        // --review-- all other values for CyclePauseType cause same, ie. NORPAUS
            {  //NORPAUS
                PauseOn();
                OpenFeed();
                OpenDump();
                g_bNoLeakAlarmCheck = TRUE;            // indicate no leak alarm check required
                if(g_bCleanAtEndOfCycle)
                {
                    g_bCleanAtEndOfCycle = FALSE;
                    StartMouldClean();  // STARTMOULDCLEAN
                }
            }
        }
    }

}
/*
    g_nCyclePauseType = END_OF_CYCLE_PAUSE_TYPE;

;       CHECK FOR END OF CYCLE PAUSE.
;
CHKFOREOCPAUSE:
        LDAA    CYCLEPAUSETYPE          ; ANY PAUSING.
        BEQ     XITEOCP                 ; NO END OF CYCLE PAUSE.
        TST     PAUSEONACTIVE           ; PAUSE ON FLAG ACTIVE
        BNE     ISEXPA                  ; EXTERNAL PAUSE

        CLR     CYCLEPAUSETYPE          ; RESET FLAG.
        CMPA    #PAUSEATENDOFCYCLE      ; END OF CYCLE PAUSE ?
        BEQ     NORPAUS                ; EXIT.
        CMPA    #OFFLINEPAUSEATENDOFCYCLE      ; OFFLINE PAUSE ?
        BEQ     OFFPAUS                ; EXIT.
NORPAUS JSR     PAUSEON                ; PAUSE !!
XITDMP  JSR     OPENFEED               ; OPEN PINCH VALVE.
        JSR     OPENDUMP               ; OPEN DUMP FLAP
        TST     CLEANATENDOFCYCLE
        BEQ     XITEOCP
        CLR     CLEANATENDOFCYCLE
        JSR     STARTMOULDCLEAN
XITEOCP RTS        
OFFPAUS JSR     OFFLINEPAUSEON
        BRA     XITDMP


ISEXPA  JSR     PAUSEON                ; PAUSE !!
        JSR     CLOSEFEED              ; CLOSE FEED.
        JSR     CLOSEDUMP              ; CLOSE DUMP FLAG
        BRA     XITEOCP


*/
//////////////////////////////////////////////////////
// VacInhibitOn()           ASM = VACINHIBITON
//
// Set the Vac inhibit bit in the pause flag
// 
// 
//
// M.McKiernan                          17-11-2004
// First Pass   
// 
//////////////////////////////////////////////////////
void VacInhibitOn( void )
{
    g_nPauseFlag |= VAC_INHIBIT_PAUSE_BIT;
}


//////////////////////////////////////////////////////
// VacInhibitOff()          ASM = VACINHIBITOFF
//
// Clear the Vac inhibit bit in the pause flag
// 
// 
//
// M.McKiernan                          17-11-2004
// First Pass   
// 
//////////////////////////////////////////////////////
void VacInhibitOff( void )
{
int nTemp = VAC_INHIBIT_PAUSE_BIT;
    nTemp ^= 0xFFFF;
    g_nPauseFlag &= nTemp;  // and with complement to clear bit

    g_bPauseToNormal = TRUE;        // Pause to Normal flag
}



void SwitchPauseOffAndMakeCycle( void )
{
    g_bOfflineBatchFilled = FALSE;                                      //OFFLINEBATCHFILLED      DS      1       ; INDICATE THAT OFFLINE BATCH IS READY
    PauseOff();
    g_bLiquidControlAllowed = TRUE;
    if((g_nCyclePauseTypeCopy == END_OF_CYCLE_PAUSE_TYPE)||(g_nCyclePauseTypeCopy == OFFLINE_END_OF_CYCLE_PAUSE_TYPE))
    {
        MakeCycle();
        g_nDmpSec = 0; //       asm MAKEDUMP added inline
    // set Sequence to Dump. 
        g_cBatSeq = BATSEQ_DUMP;
    }
}

/*
SWITCHPAUSEOFF:
                CLR     OFFLINEBATCHFILLED  ; RESET OFFLINE BATCH FILLED FLAG.
                JSR     PAUSEOFF
                JSR     MAKECYC ; MAKE THIS SYSTEM CYCLE.
                JSR     MAKEDUMP
                RTS        
*/

/*
CYCLEPAUSETYPE
PAUSEATENDOFCYCLE        EQU    1              ; INDICATE PAUSE AT END OF CYCLE.
IMMEDIATEPAUSE           EQU    2              ; IMMEDIATE PAUSE
OFFLINEPAUSEATENDOFCYCLE EQU    3              ; PAUSE AT END OF CYCLE.

NORMALPAUSE              EQU    %00000001       ; NORMAL PAUSE MODE
OFFLINEPAUSEATENDOFCYCLEBIT EQU %00000010       ; OFFLINE PAUSE BIT
PAUSEBITS                EQU    %00000011       ; BITS TO INDICATE PAUSING.
VACINHIBIT               EQU    %00000100       ; PAUSE IMMEDIATELY.


OFFLINEPAUSEON:
        LDAB    #OFFLINEPAUSEATENDOFCYCLEBIT ; OFFLINE PAUSE
        BRA     CONPON                  ; CONTINUE
IMDPAUSEON:
        LDAB    #IMMEDIATEPAUSE         ; IMMEDIATE PAUSE.
        STAB    CYCLEPAUSETYPE
PAUSEON LDAB    #$01
CONPON  CLR     DMPLEVSEN       ; RESET LEVEL SENSOR FLAG
        ORAB    PAUSFLG                 ; SET PAUSE FLAG.
        STAB    PAUSFLG        
        STAB    PAUSETRAN
        JSR     TOMANUAL        ; SWITCH TO MANUAL MODE.
XITP    RTS



VACINHIBITOFF:
        LDAA    #1
        STAA    PTONORMAL
        LDAB    #VACINHIBIT     ; VACUUM INHIBIT BIT
        COMB                    ; COMPLEMENT
        ANDB    PAUSFLG                 ; SET PAUSE FLAG.
        STAB    PAUSFLG                 ;        
        RTS

VACINHIBITON:
        LDAB    #VACINHIBIT     ; VACUUM INHIBIT BIT
        ORAB    PAUSFLG                 ; SET PAUSE FLAG.
        STAB    PAUSFLG                 ;        
        BRA     XITP                    ; CONTINUE PAUSEON.       

PAUSEOFF LDAA    #1
        STAA    PTONORMAL
        CLR     BATCHREADY      ; RESET BATCH READY AND ALLOW BLENDER TO CONTINUE ON.
        LDAA    #PAUSEBITS      ; RESET PAUSE BITS.
        COMA                    ; COMPLEMENT.
        ANDA    PAUSFLG         ; RESET BITS
        STAA    PAUSFLG         ; RESTORE
        CLR     CYCLEPAUSETYPE  ; RESET CYCLE PAUSE FLAG
        TST     BYPASS  ; IN BYPASS MODE
        BEQ     XITPOFF
        CLR     BYPASS  ; BACK TO NORMAL MODE.
        JSR     CLOSEBYPASS
        JSR     MAKECYC ; MAKE THIS SYSTEM CYCLE.
        JSR     MAKEDUMP
XITPOFF RTS

PAUSEON LDAB    #$01
CONPON  CLR     DMPLEVSEN       ; RESET LEVEL SENSOR FLAG
        ORAB    PAUSFLG                 ; SET PAUSE FLAG.
        STAB    PAUSFLG        
        STAB    PAUSETRAN
        JSR     TOMANUAL        ; SWITCH TO MANUAL MODE.
XITP    RTS

; PAUSE ON.


VACINHIBITON:
        LDAB    #VACINHIBIT     ; VACUUM INHIBIT BIT
        ORAB    PAUSFLG                 ; SET PAUSE FLAG.
        STAB    PAUSFLG                 ;        
        BRA     XITP                    ; CONTINUE PAUSEON.       
OFFLINEPAUSEON:
        LDAB    #OFFLINEPAUSEATENDOFCYCLEBIT ; OFFLINE PAUSE
        BRA     CONPON                  ; CONTINUE
IMDPAUSEON:
        LDAB    #IMMEDIATEPAUSE         ; IMMEDIATE PAUSE.
        STAB    CYCLEPAUSETYPE
PAUSEON LDAB    #$01
CONPON  CLR     DMPLEVSEN       ; RESET LEVEL SENSOR FLAG
        ORAB    PAUSFLG                 ; SET PAUSE FLAG.
        STAB    PAUSFLG        
        STAB    PAUSETRAN
        JSR     TOMANUAL        ; SWITCH TO MANUAL MODE.
XITP    RTS


; PAUSE / BYPASS OFF.


VACINHIBITOFF:
        LDAA    #1
        STAA    PTONORMAL
        LDAB    #VACINHIBIT     ; VACUUM INHIBIT BIT
        COMB                    ; COMPLEMENT
        ANDB    PAUSFLG                 ; SET PAUSE FLAG.
        STAB    PAUSFLG                 ;        
        RTS





PAUSEOFF LDAA    #1
        STAA    PTONORMAL
        CLR     BATCHREADY      ; RESET BATCH READY AND ALLOW BLENDER TO CONTINUE ON.
        LDAA    #PAUSEBITS      ; RESET PAUSE BITS.
        COMA                    ; COMPLEMENT.
        ANDA    PAUSFLG         ; RESET BITS
        STAA    PAUSFLG         ; RESTORE
        CLR     CYCLEPAUSETYPE  ; RESET CYCLE PAUSE FLAG
        TST     BYPASS  ; IN BYPASS MODE
        BEQ     XITPOFF
        CLR     BYPASS  ; BACK TO NORMAL MODE.
        JSR     CLOSEBYPASS
        JSR     MAKECYC ; MAKE THIS SYSTEM CYCLE.
        JSR     MAKEDUMP
XITPOFF RTS




*/
/*
; TOGGLE PAUSE.
PAUSETOGGLE:
        TST     PAUSFLG         ; CHECK PAUSE
        BEQ     PISOFF          ; IS OFF SWITCH ON.
        JSR     PAUSEOFF        ; ON TO OFF.
        BRA     XITPTG          ; EXIT.
PISOFF  JSR     PAUSEON         ; OFF TO ON.
XITPTG  RTS                
*/

