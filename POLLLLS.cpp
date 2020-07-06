//
// P.Smith                              12/7/07
//
// Polling of low level sensor card to determine if a low level alarm has occurred.
// Note that if a particular component alarm is active and not in the sequence table
// then the alarm is not generated.
//
// P.Smith                              4/10/05
// Rename low priority todos
//
// P.Smith                              23/11/05
// Correct read of low level sensors, store as COVERED & UNCOVERED
// Correct "ClearUnusedAlarms" is corrected.
//
// P.Smith                              15/12       /05
// check g_OnBoardCalData.m_cPermanentOptions for LICENSE_LLSOPTIONBIT
// before reading lls data.
//
// P.Smith                      20/2/06
// first pass at netburner hardware conversion.
// Bool -> BOOL
// included  <basictypes.h>
// P.Smith                      19/4/06
// nLLSbits = g_nLowLevelSensorInputState; reads from here even if no comms
//
// remove check for nolowlevel sensor comms, expansion interface redundant.
//
// P.Smith                      16/11/06
// remove unused commented out code.
//
// M.McKiernan                      17/9/07
// Removed g_arrnMBTable[] & g_arrnMBTableFlags[], now in BatVars2
//
// P.Smith                      11/7/08
// if two alarms come in together, then there is a problem on power up.
// the reason for this is that for the alarm occurred, there is a check
// for the low level sensor alarm bit being clear before setting the alarm.
// this is removed
// in CopyLowLevelSensorAlarmbits, read in 16 bits from the lls card instead of 8
// this allow low level sensor inputs for 12 components and associated outputs
//
// P.Smith                      21/7/08
// remove licensing check
//
// P.Smith                          23/7/08
// remove g_arrnMBTable and array comms extern
//
// P.Smith                          2/6/08
// no lls alarm cleared occurred unless the component set % is non zero.
//
// P.Smith                          18/1/10
// correction to CopyLowLevelSensorAlarmbits, problem found by coverity
// possibility of overrunning end of g_bLowLevelSensorStatus
//
// P.Smith                          24/2/10
// add back in licensing check for polling of lls card

////////////////////////////////////////////////////

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
#include "InitialiseHardware.h"
#include "Pause.h"
#include "Polllls.h"
#include <basictypes.h>
#include <stdio.h>
#include "License.h"



extern  CalDataStruct   g_CalibrationData;
extern  structSetpointData  g_CurrentRecipe;
extern  OnBoardCalDataStruct   g_OnBoardCalData;


//////////////////////////////////////////////////////
// CheckLowLevelSensorAlarms( void )         from ASM = CHKLLSALM
//
// Check lls for alarms
//
//////////////////////////////////////////////////////


void CheckLowLevelSensorAlarms( void )  //
{
int i,nCompIndex;
    i = 0;
    nCompIndex = 0;
    if((g_CalibrationData.m_nPeripheralCardsPresent & LLSENBITPOS) && (g_OnBoardCalData.m_cPermanentOptions & LICENSE_LLSOPTIONBIT))   // sei enabled
    {
        CopyLowLevelSensorAlarmbits();  //       ASM COPYLLSBITS     ; COPY lls bits

        if(g_CalibrationData.m_nBlenderMode != MODE_SINGLERECIPEMODE)

        {
            ClearUnusedAlarms();
            while((g_cSeqTable[i] != SEQDELIMITER) && (i < MAX_COMPONENTS)) // SEQDELIMITER indicates table end
            {
                nCompIndex = (int)(g_cSeqTable[i] & 0x0F) - 1;  // get component no. from table, mask off the m.s. nibble
                if(g_bLowLevelSensorStatus[nCompIndex] == UNCOVERED)            // level sensor off ? asm LS1OFF
                {
                     if(g_bLowLevelSensorStatusHistory[nCompIndex] == COVERED)
                    {
                        g_bLowLevelSensorStatusHistory[nCompIndex] = UNCOVERED;
                        if(g_CurrentRecipe.m_fPercentage[nCompIndex] > 0.001f)
                        {
                            PutAlarmTable(LOWLEVALARM,nCompIndex+1);     // indicate alarm,  //PTALTB
                        }
                    }

                }
                else
                {   //  asm NOALARM
// merge CLEARALARM inline
                    if(g_bLowLevelSensorStatusHistory[nCompIndex] == UNCOVERED)
                    {
                       if(g_CurrentRecipe.m_fPercentage[nCompIndex] > 0.001f)
                       {
                           RemoveAlarmTable(LOWLEVALARM,nCompIndex+1);     // remove alarm,  //RMALTB
                       }
                        g_bLowLevelSensorStatusHistory[nCompIndex] = COVERED;
                    }
                }
            i++;
            }
        }
    }
}

//////////////////////////////////////////////////////
// CopyLowLevelSensorAlarmbits( void )         from ASM = COPYLLSBITS
//
// Copy lls alarm bits into alarm word.
// Read bit read back from low level sensor card and transfer them into
// BOOLs indicating if off (TRUE) or on (FALSE)
//
//////////////////////////////////////////////////////


void CopyLowLevelSensorAlarmbits( void )
{
int nLLSbits,i;

    nLLSbits = g_nLowLevelSensorInputState; // use inputs from low level sensor card.

    for( i = 0; i < MAX_COMPONENTS; i++ )
    {
        if(nLLSbits & 0x01)
        {
            g_bLowLevelSensorStatus[i] = COVERED;    // lls is on
        }
        else
        {
            g_bLowLevelSensorStatus[i] = UNCOVERED;   // lls is off
        }

        nLLSbits >>= 1;     // shift right
    }
}
//////////////////////////////////////////////////////
// ClearUnusedAlarms( void )         from ASM = CLEARUNUSEDALARMS
//
// Clear unused alarms
//
//////////////////////////////////////////////////////


void ClearUnusedAlarms( void )
{
BOOL bFound;
int i,j,nCompIndex;

    for( i = 0; i < 8; i++)
    {

        bFound = FALSE;
        j = 0;

        while((g_cSeqTable[j] != SEQDELIMITER) && (j < MAX_COMPONENTS) && !bFound) // SEQDELIMITER indicates table end
        {
            nCompIndex = (int)(g_cSeqTable[j] & 0x0F) - 1;  // get component no. from table, mask off the m.s. nibble
            if(nCompIndex == i)
            bFound = TRUE;
            j++;
        }
        if(!bFound)  //  if not sequence table, remove the alarm
        {   // clearalarm inline

            if(g_bLowLevelSensorStatusHistory[i] == UNCOVERED)
            {
                RemoveAlarmTable(LOWLEVALARM,i+1);     // remove alarm,  //RMALTB
                g_bLowLevelSensorStatusHistory[i] = COVERED;
            }
        }
    }
}


 /*
 ;***********************************************************************;
;                                                                       ;
;       P.SMITH                                 30/9/98                 ;
;       POLHA01.ASM                                                     ;
;                                                                       ;
;       P.SMITH                                 22/12/98                ;
;       POLLING ROUTINE TO CHECK FOR LEVEL SENSOR, SET INHIBIT FLAG IS  ;
;       THIS HAS OCCURRED.                                              ;
;       ONLY RUN ALARM CHECK IF "FITHAIF" IS SET.                       ;
;       POLHA01.ASM - POLHA02.ASM                                       ;
;                                                                       ;
;       P.SMITH                                 18/2/99                 ;
;       CALL "RMALTB" TO CLEAR ALARM IF SENSOR TRANSITION IS OFF TO ON. ;
;       THIS IS ONLY CALLED ON THIS TRANSITION.                         ;
;       POLHA02.ASM - POLHA03.ASM                                       ;
;                                                                       ;
;       P.SMITH                                 16/5/00                 ;
;       CHECK LLSINPUTSTATE AND TRANSFER CONTENTS TO LS1OFF.            ;
;       POLHA03.ASM - POLLLS01.ASM                                      ;
;                                                                       ;
;                                                                       ;
;       P.SMITH                                 26/3/3                  ;
;       SAVE COMP NO IN "LLSCOMPNO"                                     ;
;                                                                       ;
;       P.SMITH                                 18/5/4                  ;
;       ENSURE THAT ALARMS ARE SELF CLEARING.                           ;
;                                                                       ;
;        P.SMITH                                6/10/04                 ;
;        ADD OPTIONS TO READ LLS FROM EXPANSION PCB                     ;
;        NOLLSCOMMUNICATIONS IS SET THEN INPUTS ARE READ FROM EXPANSION ;
;        PCB.                                                           ;
;                                                                       ;
;                                                                       ;
;       P.SMITH                                 12/1/05                 ;
;       NO LLS ALARMS IF SINGLE RECIPE MODE.                            ;
;***********************************************************************;

;       HARDWARE REF

        XREF    OP0_2592,OP1_2592,V2IORA

;       RAM DEFINITIONS
        XREF    LS1ONCT,LS1VOFFCT,LS1OFF,LS1ONTR,NOBLNDS,SEQDELIMITER,SEQTABLE,LS1ALHI
        XREF    LLSINPUTSTATE,EIOMITTED,LLSCOMPNO,EXPANIOINPUTDATA,NOLLSCOMMUNICATIONS,BLENDERMODE

;       ROUTINES
        PUBLIC  CHKLLSALM
        XREF    PTALTB,RMALTB

;       CONSTANTS
        XREF    LOWLEVALARM,LLSENBITPOS,ONE,EIGHT,SINGLERECIPEMODE

        PUBLIC  CHKLLSALM

BANK1PROGS:        SECTION

CLEARUNUSEDALARMS:
       LDAA     #ONE
RECC   JSR      CHECKIFINTABLE
       TSTE
       BNE      TONXC           ; IS IN TABLE
       PSHA
       JSR     CLEARALARM
       PULA
TONXC  INCA
       CMPA     #8
       BLS      RECC
       RTS


CHECKIFINTABLE:
       LDE      #0
       LDX      #SEQTABLE       ; POINT TO TABLE START.
REPTC  LDAB     0,X             ;
       ANDB     #$0F
       CBA
       BEQ      ISINTB          ; YES
       AIX      #1
       LDAB     0,X
       CMPB     #SEQDELIMITER   ; END OF TABLE.
       BNE      REPTC
XITTBC RTS

ISINTB LDE      #1
       BRA      XITTBC


CHKLLSALM:
       LDAA     EIOMITTED       ; EI OMITTED ?
       ANDA     #LLSENBITPOS    ; LLS ENABLE BIT POS.
       BEQ      XITALC          ; EXIT.
       JSR      COPYLLSBITS     ; COPY LLS BITS.
       LDAA     BLENDERMODE
       CMPA     #SINGLERECIPEMODE   ; SINGLE RECIPE MODE
       BEQ      XITALC              ; NO ALARM INFO TRANSFER IN THIS MODE
       JSR      CLEARUNUSEDALARMS
       LDX      #SEQTABLE       ; POINT TO TABLE START.
REPCHK PSHM     X               ; SAVE TABLE POSITION.
       LDAB     0,X             ;
       ANDB     #$0F
       STAB     LLSCOMPNO       ; LLS COMP NO
       TBA                      ; COPY COMPONENT.
       LDX      #LS1OFF         ; OFF STATUS.
       DECB
       ABX
       TST      0,X
       BEQ      NOALARM           ;
       LDX      #LS1ALHI          ; LEVEL SENSOR ALARM HISTORY.
       TAB
       DECB
       ABX
       TST      0,X
       BNE      NOALM
       LDAB     #$0F            ; STOP REPETITION OF ALARM.
       STAB     0,X
       ORAA     #LOWLEVALARM
       JSR      PTALTB          ; ALARM !!
NOALM  PULM     X
       AIX      #1
       LDAA     0,X
       CMPA     #SEQDELIMITER   ; END OF TABLE.
       BNE      REPCHK
XITALC RTS



NOALARM JSR     CLEARALARM
        BRA      NOALM           ; EXIT.



CLEARALARM:
        LDX      #LS1ALHI        ; LEVEL SENSOR ALARM HISTORY.
        TAB
        DECB
        ABX
        TST      0,X
        BEQ      NOALMRS           ; LEVEL SENSOR WAS O
        ORAA     #LOWLEVALARM
        PSHA
        JSR      RMALTB          ; CLEAR ALARM.
        PULA
NOALMRS LDX      #LS1ALHI        ; LEVEL SENSOR ALARM HISTORY.
        ANDA   #$0F
        TAB
        DECB
        ABX
        CLR      0,X
        RTS




; COPY THE LLS ALARM BITS TO LS1OFF, 1 IS OFF.

COPYLLSBITS:
        LDE     #8              ; NO OF LLS INPUTS.
        LDAA    LLSINPUTSTATE+1 ;
        TST     NOLLSCOMMUNICATIONS
        BEQ     ISLLSCMS

        LDAA    EXPANIOINPUTDATA
ISLLSCMS:
        LDX     #LS1OFF         ; INITIAL LS STATUS.
CONLLC  LSRA                    ; CHECK DATA.
        BCC     LSISOFF         ; LS IS OFF.
        CLR     0,X             ; LS IS ON.
        BRA     CHKNXLS         ; CHECK NEXT LS.
LSISOFF LDAB    #1
        STAB    0,X             ; SET TO INDICATE ON.
CHKNXLS AIX     #1
        ADDE    #-1             ; DECREMENT COUNTER
        BNE     CONLLC          ; CONTINUE LLS CHECK.
        RTS




*/
