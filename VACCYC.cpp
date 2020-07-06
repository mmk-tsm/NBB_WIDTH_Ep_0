//
// P.Smith                              12/7/07
//
// Checks for vacuum sequencing
//
// P.Smith                      19/2/06
// first pass at netburner hardware conversion.
// #include <basictypes.h>
//
// P.Smith                      20/4/07
// set g_bSignalXXKgsTargetAlarm to false when it is detected true, this had the
// effect of the alarm coming in continuously.
//
// P.Smith                      25/6/07
// set g_nVibrateCatchBoxCtr to allow catch box vibrator to operate
// use EXPANSIONOUTPUT6BITPOS from the expansion card
//
// P.Smith                      1/8/07
// corrected WaitForMaterialToBeVacuumed functionality
// when the current extruder was non zero on start up, the program stayed in this
// loop. this was corrected to set g_bVacuumMaterialAway to false immediately, not
// when the clean is initated, this is too late, this function should only run once
// on reset and not again.
//  g_CalibrationData.m_nCurrentExtruderNo used to hold current extruder no, this value
// is stored in non volatile memory.
// set / clear g_arrnMBTable[BATCH_VACUUM_ON] for vacuum on /off
//
// M.McKiernan                      17/9/07
// Removed g_arrnMBTable[] & g_arrnMBTableFlags[], now in BatVars2
//
// P.Smith                         23/10/07
// call WriteMultiBlendDiagnosticsToSDCard
//
// P.Smith                         9/1/08
// remove mtprintf
//
// P.Smith                          30/1/08
// correct compiler warnings
//
// P.Smith                          29/4/08
// name change g_CalibrationData.m_nCurrentExtruderNo to g_nCurrentExtruderNo
//
// P.Smith                          17/5/08  kwh
// in CheckForVacuumSequence, check for g_bAllowMultiBlendCycle before allowing the
// vacuum sequence to start. this stops the blender from starting this sequence before
// the batch is filled.
// set g_bAllowMultiBlendCycle to false at the end of the vacuum cycle.
// this allows the vacuum to be disable before the next fill, ie when the level
// sensor becomes uncovered.
// only call WriteMultiBlendDiagnosticsToSDCard if logtosdcard is true
//
// P.Smith                          23/7/08
// remove g_arrnMBTable & comms arrays extern
//
// P.Smith                          16/10/08
// todo to lp
//
// P.Smith                          17/9/09
// change the check sd card log to check LOG_BATCH_LOG_TO_SD_CARD for new log enable
//
// P.Smith                          8/2/10
// remove WriteMultiBlendDiagnosticsToSDCard
//////////////////////////////////////////////////////

//*****************************************************************************
// INCLUDES
//*****************************************************************************
#include "General.h"
#include "BatVars.h"
#include "BatVars2.h"
#include "KghCalc.h"
#include "ConversionFactors.h"
#include "BatchCalibrationFunctions.h"
#include "SetpointFormat.h"
#include "BatchMBIndices.h"
#include "Conalg.h"
#include "Alarms.h"
#include "TSMPeripheralsMBIndices.h"
#include "Lspeed.h"
#include "BlRep.h"
#include "InitialiseHardware.h"
#include "Pause.h"
#include "CycleMonitor.h"
#include "Vaccyc.h"
#include <basictypes.h>
#include <stdio.h>
#include "Sdcard.h"



extern  CalDataStruct   g_CalibrationData;
extern  structSetpointData  g_CurrentRecipe;

extern  structRollData      g_FrontRoll;
extern  structRollData      g_BackRoll;
extern  structRollData      g_Order;
extern  structRollData      g_CurrentShiftFront;
extern  structRollData      g_OngoingShift;






//////////////////////////////////////////////////////
// CheckForVacuumSequence( void )         from ASM = CHECKFORVACSEQ
//
// Start single vacuum sequence if in single recipe mode
//
// called from cycle monitor when it is time to open the offline feed valve
//////////////////////////////////////////////////////


void CheckForVacuumSequence( void )
{
    if((g_CalibrationData.m_nBlenderMode == MODE_SINGLERECIPEMODE)&& g_bAllowMultiBlendCycle)  //kwh
    {
            g_nVacuumSequence = VACSEQ_WAITFOREMPTYBINPRIORTOVACUUMID; // ASM WAITFORPRECBEMPTYID
//            SetupMttty();
//            iprintf("\n starting vac seq");
    }
    else  // asm NOVAC
    {
        OpenFeed();
        if(g_bSignalXXKgsTargetAlarm)       // signal target alarm reached ?
        {
            g_bSignalXXKgsTargetAlarm = FALSE;
//        merge SIGNALATTARGETALARM inline

            if( (g_arrnMBTable[BATCH_SUMMARY_PROCESS_ALARMS] & ATTARWTALARMBIT) == 0 ) // at target alarm bit not set???
            {
                PutAlarmTable( ATTARWTALARM,  0 );     // indicate alarm,  //PTALTB
            }
        }
    }
}



//////////////////////////////////////////////////////
// MonitorVacuumSequence( void )         from ASM = VACMON
//
// Monitor Vacuum sequence
//
//////////////////////////////////////////////////////

void MonitorVacuumSequence( void )
{
unsigned int nTemp,i;
// CHECKFORVACON merged inline

    if(g_bWaitForVacuumToFinish && !g_bVacuumInProgress)   // if waiting for vacuum to finish and not in actual vacuuming not in progress
    {
        g_bWaitForVacuumToFinish = FALSE;
        g_nCurrentExtruderNo = g_CurrentRecipe.m_nExtruder_No; // indicate current extruder no.
        VacInhibitOff();                // ASM VACINHIBITOFF
        InitiateCleanCycle();           // asm INITIATECLEANCYCLE
    }
    if(g_CalibrationData.m_nBlenderMode == MODE_SINGLERECIPEMODE)
    {
        switch(g_nVacuumSequence)
        {
            case VACSEQ_WAITFOREMPTYBINPRIORTOVACUUMID:
                if( g_bLowLevelSensor == UNCOVERED )  // bin sensor uncovered ?
                {
                    g_nVacuumTimer = 0;
                    g_nVacuumSequence = VACSEQ_DELAYBEFOREOFFLINEVALVEOPENEDID; // ASM PREVACDELAYID   // delay before opening offline slide valve
                }
                else
                {  // ASM STRMAT
                    VacuumOn();
                }
                break;
            // NOTID1
            case VACSEQ_DELAYBEFOREOFFLINEVALVEOPENEDID:     // ASM PREVACDELAYID
                if(g_nVacuumTimer >= VACSEQ_OFFLINEVALVEOPENDELAY)
                {
                    OpenFeed();
                    VacuumOn();
                    g_bOfflineValveIsOpen = TRUE;
                    g_nVacuumTimer = 0;    // asm VACCTR
                    g_nVacuumSequence = VACSEQ_MATERIALDROPDELAYDELAYID; //  asm DROPDELAYID
                }
                break;
            // NOTID2
            case VACSEQ_MATERIALDROPDELAYDELAYID:   //asm =DROPDELAYID
                if(g_nVacuumTimer >= VACSEQ_MATERIALDROPDELAY)
                {
                    g_nVacuumTimer = 0;    // asm VACCTR
                    g_nVacuumSequence = VACSEQ_WAITFOREMPTYBINPRIORAFTERVACUUMID; //    asm WAITFORPOSTCBEMPTYID
                }
                break;
            // NOTID3
            case VACSEQ_WAITFOREMPTYBINPRIORAFTERVACUUMID:
                if(g_nVacuumTimer >= VACSEQ_MATERIALDROPDELAY)
                {
                    if( g_bLowLevelSensor == UNCOVERED )  // bin sensor uncovered ?
                    {
                        g_nVacuumTimer = 0;    // asm VACCTR
                        g_nVacuumSequence = VACSEQ_MATERIALTRANSPORTDELAYID; //  asm VACTRANSPORTDELAYID
                        g_nVibrateCatchBoxCtr = CATCHBOXVIBRATIONDELAY;
                    }

                }
                break;
            // NOTID4
            case VACSEQ_MATERIALTRANSPORTDELAYID:
                nTemp = g_CalibrationData.m_nMaterialTransportDelay * 10; // convert delay to tenths of seconds
                if(g_nVacuumTimer >= nTemp)
                {
                    if(g_nCleanCycleSeq == 0)
                    {
                        g_nVacuumTimer = 0;    // asm VACCTR
//  merge VacuumOff inline
                        g_arrnMBTable[BATCH_SINGLE_RECIPE_VACUUM_INFO] = 0; // asm  COPYMBVACUUMOFFSTATUS
                        g_nVacuumWriteData = 0; // asm VACUUMWR
                        g_bVacuumIsOn = FALSE;  // asm VACUUMISON
                        g_nVacuumSequence = VACSEQ_POSTMATERIALTRANSPORTDELAYID; //  asm = POSTVACUUMDELAYID
                        g_arrnMBTable[BATCH_VACUUM_ON] = 0; //
                        if((g_CalibrationData.m_nLogToSDCard & LOG_BATCH_LOG_TO_SD_CARD) != 0)
                        {
                //nbb--todo--review            WriteMultiBlendDiagnosticsToSDCard();
                        }

                    }
                }
                break;
            // NOTID5
            case VACSEQ_POSTMATERIALTRANSPORTDELAYID:
                nTemp = g_CalibrationData.m_nPostMaterialTransportDelay * 10; // convert post delay to tenths of seconds
                if(g_nVacuumTimer >= nTemp)
                {
//--merge CLEARVACUUMSTATUS inline

                    nTemp = 1;
                    if(g_nCurrentExtruderNo != 0)
                    {

                        for(i = 1; i < g_nCurrentExtruderNo; i++)
                        {
                            nTemp <<= 1;                // shift bit left to the required "in progress" position
                        }
                        nTemp ^= 0xFFFF;    // asm CLRIPS complement
                        g_arrnMBTable[BATCH_SINGLE_RECIPE_VACUUM_STATUS] &= nTemp;
                    }

                    if(!g_bBatchHasFilled && !g_bRecipeWaiting)
                    {
                        g_arrnMBTable[BATCH_SINGLE_RECIPE_VACUUM_STATUS] = 0;       // reset inhibit bits
                        g_bAllowMultiBlendCycle = FALSE;  //nbb--todolp--kwh

                    }
                    // NOREWT
                    g_nCurrentExtruderNo = g_CurrentRecipe.m_nExtruder_No; // indicate current extruder no.


                    g_bVacuumInProgress = FALSE;
                    g_nVacuumSequence = VACSEQ_NOSEQUENCEID;                // asm = DISABLEVACSEQID
                    if(!g_bRecipeWaiting)
                    {
                        g_bBlenderInhibit = FALSE;
                    }
                    else  // YESRCW
                    {
                        VacInhibitOff();                // ASM VACINHIBITOFF
                    }
                }
                break;

            default:
                break;
        } // end of switch
    }
}





//////////////////////////////////////////////////////
// VacuumOn( void )         from ASM = VACUUMON
//
// switches the required output on the vac 8 card via modbus
//
//////////////////////////////////////////////////////


void VacuumOn( void )
{
int x,i;
    x = 1;      // variable to be shifted
    g_nVacuumLoaderNo = g_nCurrentExtruderNo & 0x0f;            // ensure range 1-9

    if(g_nVacuumLoaderNo != 0)
    {
        for(i = 1; i < g_nVacuumLoaderNo; i++)
        {
            x <<= 1;
        }
        g_nVacuumWriteData |= x;                                // or in shifted data
        g_bVacuumIsOn = TRUE;
        g_bVacuumInProgress = TRUE;
        // merge COPYMBVACUUMONSTATUS inline
        g_arrnMBTable[BATCH_SINGLE_RECIPE_VACUUM_INFO] = g_nCurrentExtruderNo; // asm COPYMBVACUUMONSTATUS
        g_arrnMBTable[BATCH_VACUUM_ON] = 1; //



    }

}



//////////////////////////////////////////////////////
// CheckIfVacuumRequired( void )         from ASM = CHECKIFVACUUMREQUIRED
//
// checks if the blender should initiate a vacuum.
// called every 10 hz, only applies to start up.
//
//
//////////////////////////////////////////////////////


void CheckIfVacuumRequired( void )
{
    if(g_CalibrationData.m_nBlenderMode == MODE_SINGLERECIPEMODE)
    {
        if(g_bCheckIfVacuumIsRequired)
        {
            g_bCheckIfVacuumIsRequired = FALSE;
            if(g_nPauseFlag & VAC_INHIBIT_PAUSE_BIT)
            {
                g_bVacuumMaterialAway=  TRUE;
            }

        }
    }

}





//////////////////////////////////////////////////////
// WaitForMaterialToBeVacuumed( void )         from ASM = WAITFORMATERIALVACAWAY
//
// If catch bin sensor is covered and loader is empty allow the blender to initiate clean
//
// called continuously from foreground
//
//////////////////////////////////////////////////////


void WaitForMaterialToBeVacuumed( void )   //--review how this actually works ?
{
int nTemp,i;
    if(g_CalibrationData.m_nBlenderMode == MODE_SINGLERECIPEMODE)
    {
        if(g_bVacuumMaterialAway)
        {
//        SetupMttty();
//        iprintf("\n current extruder no is %d",g_nCurrentExtruderNo);
            g_bVacuumMaterialAway = FALSE;
            if(g_bLowLevelSensor == COVERED)  // catch box bin sensor covered ?
// merge  GETLOADERSTATUS inline
            {
                if(g_nCurrentExtruderNo != 0) // check for zero
                {
                    nTemp = g_nLowLevelSensorInputState;  // status read back from lls card
                    i = g_nCurrentExtruderNo-1;
                    while(i != 0)

                    {
                        i--;
                        nTemp >>= 1;
                    }

                    if(nTemp & 0x01)        // empty ?
                    {
                        g_bVacuumMaterialAway = FALSE;  // reset vacuum flag.
                        VacInhibitOff();                // ASM VACINHIBITOFF
                        InitiateCleanCycle();           // asm INITIATECLEANCYCLE

                    }
                }
             }
         }
    }
}


//////////////////////////////////////////////////////
// CheckForCatchBoxVibration( void )
//
// Checks for catch box vibration
//
//////////////////////////////////////////////////////


void CheckForCatchBoxVibration( void )
{
    int     nTemp;
    if(g_CalibrationData.m_nBlenderMode == MODE_SINGLERECIPEMODE)
    {
        if(g_nVibrateCatchBoxCtr != 0)
        {
            g_nVibrateCatchBoxCtr--;
            if(g_nVibrateCatchBoxCtr == 0)
            {
                nTemp = EXPANSIONOUTPUT6BITPOS ^ 0xFFFF;    // COM of bit position
                g_nExpansionOutput &= nTemp;
            }
            else
            {
                g_nExpansionOutput = g_nExpansionOutput | EXPANSIONOUTPUT6BITPOS; // set bit position for expansion PCB
            }
        }
    }
}

/*

#define     VACSEQ_WAITFOREMPTYBINPRIORTOVACUUMID    1      // WAITFORPRECBEMPTYID     EQU     1
#define     VACSEQ_DELAYBEFOREOFFLINEVALVEOPENEDID   2      // PREVACDELAYID           EQU     2
#define     VACSEQ_MATERIALDROPDELAYDELAYID          3      // DROPDELAYID             EQU     3
#define     VACSEQ_WAITFOREMPTYBINPRIORAFTERVACUUMID 4      // WAITFORPOSTCBEMPTYID    EQU     4
#define     VACSEQ_MATERIALTRANSPORTDELAYID          5      // VACTRANSPORTDELAYID     EQU     5
#define     VACSEQ_POSTMATERIALTRANSPORTDELAYID      6      // POSTVACUUMDELAYID       EQU     6
#define     VACSEQ_NOSEQUENCEID                      0      // DISABLEVACSEQID         EQU     0



#define     VACSEQ_OFFLINEVALVEOPENDELAY            50 // PREVACDELAY             EQU     0050                ; DELAY BEFORE STARTING THE VACUUM
#define     VACSEQ_MATERIALDROPDELAY                50 // DROPDELAY               EQU     0050                ; DELAY BEFORE CHECKING CATCH BOX SENSOR
#define     VACSEQ_MATERIALTRANSPORTDELAY           50 // VACUUMTRANSPORTDELAY    EQU     0050                ; VACUUM TRANSPORT


PREVACDELAY             EQU     0050                ; DELAY BEFORE STARTING THE VACUUM
DROPDELAY               EQU     0050                ; DELAY BEFORE CHECKING CATCH BOX SENSOR
VACUUMTRANSPORTDELAY    EQU     0050                ; VACUUM TRANSPORT

BANK1PROGS:        SECTION


;*****************************************************************************;
; FUNCTION NAME : CHECKFORVACSEQ                                              ;
; FUNCTION      : WHEN OFFLINE VALVE IS OPENED, CHECK FOR SINGLE RECIPE MODE  ;
;               ; SEQUENCE SHOULD BE RUN.                                     ;
; INPUTS        : BLENDERMODE - CHECK FOR SINGLE RECIPE MODE                  ;
;*****************************************************************************;

CHECKFORVACSEQ:
        LDAA    BLENDERMODE                     ; READ BLENDER MODE
        CMPA    #SINGLERECIPEMODE               ; SINGLE RECIPE MODE
        BNE     NOVAC                           ; NO EXIT
        LDAA    #WAITFORPRECBEMPTYID            ; SIGNAL VAC SEQUENCE TO START.
        STAA    VACSEQ                          ; SET UP SEQUENCE INDICATOR
XITVC   RTS
NOVAC   JSR     OPENFEED                        ; OPEN FEED VALVE AS NORMAL
        TST     SIGNALATTARGETALARMFLAG
        BEQ     NOATA                           ; NO TARGET ALARM.
        CLR     SIGNALATTARGETALARMFLAG
        JSR     SIGNALATTARGETALARM
NOATA   BRA     XITVC                           ; EXIT.


;*****************************************************************************;
; FUNCTION NAME : VACMON                                                      ;
; FUNCTION      : MONITORS VACSEQ AND DETERMINES IF WHAT STAGE OF THE VAC     ;
;               ; SEQUENCE SHOULD BE RUN.                 ;
; INPUTS        : VACSEQ                                                        ;                       ;
;*****************************************************************************;

VACMON:

; 1ST STAGE WAITING FOR CATCH BOX TO CLEAR BEFORE OPENING THE OFFLINE VALVE
;
        JSR     CHECKFORVACON                   ; SHOULD WE CHECK FOR VACUUM ON.
        LDAA    BLENDERMODE                     ; READ BLENDER MODE
        CMPA    #SINGLERECIPEMODE               ; SINGLE RECIPE MODE
        BNE     XITPC                           ; NO EXIT
        LDAA    VACSEQ          ;
        CMPA    #WAITFORPRECBEMPTYID            ; WAITING FOR CATCH BOX EMPTY
        BNE     NOTID1                          ; NO
        TST     LOWLEVELSENSOR
        BNE     STLMAT                          ; STILL MATERIAL IN THE BIN.
        CLRW    VACCTR                          ; RESET COUNTER
        LDAA    #PREVACDELAYID                  ; LOAD PRE VACUUM DELAY
        STAA    VACSEQ                          ; STORE
XITPC   RTS
STLMAT  JSR     VACUUMON                        ; VACUUM MATERIAL IN BIN.
        BRA     XITPC


; DELAY BEFORE THE VACUUM IS STARTED.

NOTID1  LDAA    VACSEQ                          ;
        CMPA    #PREVACDELAYID
        BNE     NOTID2
        LDD     VACCTR                          ; READ TIMER
        CPD     #PREVACDELAY                    ; DELAY BEFORE VACUUMING
        BLO     XITPV                           ; EXIT IF LOWER
        JSR     OPENFEED
        JSR     VACUUMON                        ; SWITCH VACUUM LOADER ON FOR THIS RECIPE
        LDAA    #1
        STAA    OFFLINEVALVEISOPEN               ; ALLOW MIXER CLEAN TO PROCEED.
        CLRW    VACCTR                          ; RESET COUNTER
        LDAA    #DROPDELAYID
        STAA    VACSEQ                          ; DROP DELAY REQUIRED BEFORE CHECKING SENSOR.
XITPV   RTS

; DELAY IS REQUIRED BEFORE CHECKING SENSOR IN CATCH BOX.
; VACUUM HAS BEEN STARTED.

NOTID2  LDAA    VACSEQ                          ;
        CMPA    #DROPDELAYID
        BNE     NOTID3
        LDD     VACCTR                          ; READ TIMER
        CPD     #DROPDELAY                      ; DELAY AFTER DROPPING MATERIAL INTO CATCH BOX
        BLO     XITDD                           ; EXIT IF LOWER
        CLRW    VACCTR                          ; RESET COUNTER
        LDAA    #WAITFORPOSTCBEMPTYID           ; NO CHECK FOR VACUUM FINISHED.
        STAA    VACSEQ
XITDD   RTS

; NOW CHECK CB SENSOR

NOTID3  LDAA    VACSEQ                          ;
        CMPA    #WAITFORPOSTCBEMPTYID
        BNE     NOTID4
        TST     LOWLEVELSENSOR                     ; CHECK CB SENSOR
        BNE     XITPSC                          ; EXIT IF STILL ON
        CLRW    VACCTR
        LDAA    #VACTRANSPORTDELAYID            ; NOW DELAY FOR TRANSPORT IS REQUIRED
        STAA    VACSEQ
XITPSC  RTS

NOTID4  LDAA    VACSEQ                          ;
        CMPA    #VACTRANSPORTDELAYID
        BNE     NOTID5
        LDAB    VACUUMDELAY                     ; READ VACUUM DELAY
        JSR     BCDHEX1                         ; ENTRY TO HEX
        TBA                     ;
        LDAB    #TEN                            ; MULTIPLY BY 10
        MUL
        STD     TEMPX
        LDD     VACCTR                          ; READ TIMER
        CPD     TEMPX                           ; VACUUM TRANSPORT DELAY EXPIRED
        BLO     XITVT                           ; EXIT IF LOWER
        TST     CLEANCYCLESEQ                   ; HAS THE CLEANING FINISHED.
        BNE     XITVT                           ; NO
        CLRW    VACCTR                          ; RESET COUNTER
        JSR     VACUUMOFF                       ; SWITCH VACUUM OFF.
        CLRW    VACCTR
        LDAA    #POSTVACUUMDELAYID              ; POST VACUUM DELAY.
        STAA    VACSEQ
XITVT   RTS

NOTID5  LDAA    VACSEQ                          ;
        CMPA    #POSTVACUUMDELAYID
        BNE     NOTID6
        LDAB    VACUUMPOSTDELAY                 ; READ VACUUM DELAY
        JSR     BCDHEX1                         ; ENTRY TO HEX
        TBA                     ;
        LDAB    #TEN                            ; MULTIPLY BY 10
        MUL
        STD     TEMPX
        LDD     VACCTR                          ; READ TIMER
        CPD     TEMPX                           ; VACUUM TRANSPORT DELAY EXPIRED
        BLO     XITPVD                          ; EXIT IF LOWER
        LDAA    CURRENTEXTRUDERNO+1             ; READ EXTRUDER NO
        JSR     CLEARVACUUMSTATUS
        TST     BATCHHASFILLED                  ; HAS BATCH FILLED ?
        BNE     NOREWT                          ; YES
        TST     RECIPEWAITING                   ; RECIPE WAITING SET ?
        BNE     NOREWT                          ; RESET VACUUM STATUS
RESVS   CLRW    MBVACUUMSTATUS                  ; RESET
NOREWT  LDD     EXTRUDERNO
        STD     CURRENTEXTRUDERNO               ; STORE CURRENT EXTRUDER NO
;        CLR     VACUUMISON                      ; INDICATE VACCUM IS FINISHED.
        CLR     VACUUMINGINPROGRESS
        LDAA    #DISABLEVACSEQID                ; NO CHECK FOR VACUUM FINISHED.
        STAA    VACSEQ

; *REVIEW *
        TST     RECIPEWAITING
        BNE     YESRCW                          ; RECIPE IS WAITING
        CLR     BLENDERINHIBIT                  ; ALLOW NEW ORDER IN.
XITPVD  RTS


YESRCW  JSR     VACINHIBITOFF                   ; ENSURE THAT BLENDER CAN CYCLE
        BRA     XITPVD





NOTID6  RTS









;*****************************************************************************;
; FUNCTION NAME : VACUUMON                                                    ;
; FUNCTION      : SWITCHES VACUUM ON VIA VAC 8 CARD.                                ;
; INPUTS        : RECEIPE NO                                                        ;                       ;
;*****************************************************************************;


VACUUMON:
        LDD     CURRENTEXTRUDERNO                 ; READ CURRENT EXTRUDER
        ANDD    #$000F                          ; MAX OF 16 RECIPES
        STAB    VACUUMLOADER                    ; STORE VACUUM LOADERS CONCERNED.
        BEQ     XITVON
        LDE     #$0001                          ; LOAD VACUUM 1ST BIT POS
REPD    ADDD    #-1
        BEQ     XITFR
        ASLE                                    ; NEXT BIT POS
        BRA     REPD
XITFR   ORE     VACUUMWR                        ;
        STE     VACUUMWR                        ; SET VACUUM WRITE
        LDAA    #1
        STAA    VACUUMISON                      ; INDICATE VACUUM IS ON.
        STAA    VACUUMINGINPROGRESS
        JSR     COPYMBVACUUMONSTATUS                    ;
XITVON  RTS



;*****************************************************************************;
; FUNCTION NAME : VACUUMOFF                                                   ;
; FUNCTION      : SWITCHES VACUUM OFF 8 CARD.                                 ;
; INPUTS        : RECEIPE NO                                                  ;                       ;
;*****************************************************************************;


VACUUMOFF:
        JSR     COPYMBVACUUMOFFSTATUS
        CLRW    VACUUMWR                        ; RESET ALL OUTPUTS
        CLR     VACUUMISON                      ; INDICATE VACCUM IS FINISHED.
XITVOF  RTS




CHECKFORVACON:
        TST     WAITFORVACUUMTOFINISH           ; SHOULD WE WAIT FOR VACUUM  TO FINISH
        BEQ     XITVO                           ; NO
        TST     VACUUMINGINPROGRESS
;        TST     VACUUMISON                      ; VACUUM COMPLETELY FINISHED
        BNE     XITVO                           ; YES ON
        CLR     WAITFORVACUUMTOFINISH           ; RESET FLAG
        LDD     EXTRUDERNO
        STD     CURRENTEXTRUDERNO         ; STORE CURRENT EXTRUDER
        JSR     VACINHIBITOFF
        JSR     INITIATECLEANCYCLE      ; START THE CLEAN CYCLE AGAIN
XITVO   RTS



;*****************************************************************************;
; FUNCTION NAME : CHECKIFVACUUMREQUIRED                                       ;
; FUNCTION      : CHECK IF THE BLENDER SHOULD INITIATE A VACUUM.              ;
; INPUTS        : RECIPE NO                                                   ;
;*****************************************************************************;


CHECKIFVACUUMREQUIRED:
        LDAA    BLENDERMODE
        CMPA    #SINGLERECIPEMODE               ; SINGLE RECIPE
        BNE     XITVCC
        TST     CHECKIFVACUUMREQUIREDF          ; IS VACUUMING REQUIRED ?
        BEQ     XITVCC                          ; EXIT
        CLR     CHECKIFVACUUMREQUIREDF          ; RESET FLAG
        LDAA    PAUSFLG                         ; READ PAUSE FLAG
        ANDA    #VACINHIBIT                     ; IS THE BLENDER INHIBITED OR PAUSED ?
        BEQ     XITVCC                          ; NO CHECK REQUIRED IF INHIBIT SET
        LDAA    #1
        STAA    VACUUMMATERIALAWAY              ; VACUUM MATERIAL AWAY
XITVCC  RTS




WAITFORMATERIALVACAWAY:
        LDAA    BLENDERMODE
        CMPA    #SINGLERECIPEMODE               ; SINGLE RECIPE
        BNE     XITV
        TST     VACUUMMATERIALAWAY              ; SHOULD BE CHECK BE DONE ?
        BEQ     XITV                            ; NO
        TST     LOWLEVELSENSOR
        BEQ     XITV                            ; EXIT IF OFF
        JSR     GETLOADERSTATUS                 ; RETURN THE LOADER STATUS OF THE EXTRUDER CONCERNED
        ANDE    #0001
        BEQ     XITV                            ; EXIT IF FULL. EMPTY =1
        CLR     VACUUMMATERIALAWAY              ; RESET FLAG
        JSR     VACINHIBITOFF                   ;
        JSR     INITIATECLEANCYCLE              ; START THE CLEAN CYCLE AGAIN
XITV    RTS
CIV1    CLR     VACUUMMATERIALAWAY              ; RESET FLAG
        BRA     XITV



GETLOADERSTATUS:
        LDE     LLSINPUTSTATE                   ; READ FLAP STATUS
        LDD     CURRENTEXTRUDERNO               ;
        BEQ     XITF                            ; ERROR CONDITION
REP     ADDD    #-1
        BEQ     XITF
        LSRE                                    ; NEXT BIT POS
        BRA     REP
XITF    RTS



*/













