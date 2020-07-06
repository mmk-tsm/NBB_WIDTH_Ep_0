
//////////////////////////////////////////////////////
// Fillrep.c
//
// Fill repetitions program
//
// P.Smith                                                  19-08-2004
// First Pass
//
// M.McKiernan                                              03-09-2004
// Corrections to CalPerTarget
// M.McKiernan                                              21-09-2004
// In CalculateTargetStage3(), set filling stage before the calculation proper.

// P.Smith                                                  30/9/05
// check for min weight before allowing multiple filling.
// no multiple fill if regrind component is presen
//
// P.Smith                                                  10/1/06
// removed fDiff variable that is unused.
// make bTargetReached a volatile Bool.
//
//
// P.Smith                      23/2/06
// first pass at netburner hardware conversion.
// Bool -> BOOL
//#include <basictypes.h>
//
// P.Smith                      16/11/06
// Remove unused comments.
//
// P.Smith                                         2/3/07         ;
// Check for recipechangedbit as well as fillbyweight when setting;
// copy flags                                                      ;
//
// P.Smith                                         12/2/10         ;
// call CalculateComponentRoundRobinWeight in CalculateFinalWeight

//////////////////////////////////////////////////////

//*****************************************************************************
// INCLUDES
//*****************************************************************************
//nbb #include <hc16.h>
#include <Stdio.h>
#include <StdLib.h>

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
#include        "PITWeigh.h"
#include        "BatVars.h"
#include "CycleMonitor.h"
#include "MonBat.h"
#include "MBProgs.h"
#include "SetpointFormat.h"
#include "ChkFill.h"
#include "FillRep.h"
#include <basictypes.h>
#include "WeightRR.h"

// Locally declared global variables
// Externally declared global variables
// Data

extern CalDataStruct    g_CalibrationData;

//////////////////////////////////////////////////////
// CalcPerTarget( void )                 from ASM = CALCPERTARGET
//
// Calculates the target for the 1st stage of the Multi stage filling process.
//
//
// P.Smith                                  20-08-2004
// First pass.
// M.McKiernan                                          30-08-2004
// g_lStoredComponentTargetCounts - definition had previously existed.
// CalcCompNTime(nCompNo);      Note called for component no. (1 based)
// Using definitions (PERCENTAGEFILLBW, PERCENTAGEFILL) for percentage fill
// Changed g_bMultipleFilling = TRUE, to g_bMulFillStarted[nCompIndex] = TRUE;
//
// P.Smith                                  18/11/05
// correcte check for multiple stage filling min weight and regrind.
// note that check for regrind was not operation as expected.
// cData & REGRIND_BIT == 0
//
///////////////////////////////////////////////////////
void CalcPerTarget( void )
{

        int nCompIndex,nCompNo;
          int nFillPercent;
        unsigned char cData;
          long lValue;

         cData = g_cSeqTable[g_nSeqTableIndex];              // First component in table
         nCompNo = (int)(cData & 0x0F);

        nCompIndex = nCompNo - 1;
        // component no.s in table start at 1, index is 0 based.

        if((g_fComponentTargetWeight[nCompIndex] > MULTI_STAGE_FILL_MINWEIGHT) && ((cData & REGRIND_BIT) == 0))
        {
            if(g_nStageSeq[nCompIndex] == 0)
            {
                g_lStoredComponentTargetCounts[nCompIndex] = g_lComponentTargetCounts[nCompIndex];
                // Save the target for the 1st component
                cData = g_cSeqTable[g_nSeqTableIndex];  //load byte from SEQPOINTER
                if(cData & FILLBYWEIGHT ||(cData & RECIPECHANGEDBIT))  // fill by wt bit set??
                {
                    g_bCopyCompFlowRate[nCompIndex] = TRUE; // enable copy of flow rate
                    // if fill by weight target fill 50% of component on 1st fill
                    nFillPercent = PERCENTAGEFILLBW;
                }
                else
                {
                    nFillPercent = PERCENTAGEFILL;
                }
                lValue = (nFillPercent * g_lComponentTargetCounts[nCompIndex]) / 100;
                g_lComponentTargetCounts[nCompIndex] = lValue;

                // Converttarget is replaced by inline code
                CalcCompNTime(nCompNo);      // calculate component on time
                g_fComponentTargetWeight[nCompIndex] = (float)((g_lComponentTargetCounts[nCompIndex]) / g_CalibrationData.m_fWeightConstant);
                // comp wt Imperial units version
                g_fComponentTargetWeightI[nCompIndex] = g_fComponentTargetWeight[nCompIndex] * WEIGHT_CONVERSION_FACTOR;

                g_nFillStage = STAGE_3;   // indicate that 1st stage of fill to start              DS      1       ; STAGE FILL
                g_nStageSeq[nCompIndex] = STAGE_3; // indicate stage 3 in progress
                g_bMulFillStarted[nCompIndex] = TRUE;   //indicate multiple filling in progress for this component.
        }
    }
}

//////////////////////////////////////////////////////
// CalculateStage2Fill( int nComponent )                 from ASM = CALCSTAGE2FILL
//
// Calculates the target for the 2nd  stage of the Multi stage filling process.
//
// EXIT:    Returns a TRUE if target has been reached already, false otherwise.
//
// M.McKiernan                                          15-09-2004
// First pass.
//
///////////////////////////////////////////////////////
BOOL CalculateStage2Fill( int nComponent  )
{

    int nCompIndex = nComponent - 1;
    BOOL bTargetReached = FALSE;
    long lDiff;
    float fDiff;


    if(g_bCopyCompFlowRate[nCompIndex] == TRUE)
    {
        g_bCopyCompFlowRate[nCompIndex] = FALSE;    // Clear copy flag.
        g_ffComponentCPIStage2[nCompIndex] = g_ffComponentCPI[nCompIndex];
        g_ffComponentCPIStage3[nCompIndex] = g_ffComponentCPI[nCompIndex];
    }
    if( g_lComponentCountsActual[nCompIndex] > g_lStoredComponentTargetCounts[nCompIndex] )
    {
        //--REVIEW--
        g_lStoredComponentCountsStage1[nCompIndex] = 0;
        g_lStoredComponentCountsStage2[nCompIndex] = 0;
        CalculateFinalWeight( nComponent );
        bTargetReached = TRUE;
    }
    else
    {   // not yet at target
        lDiff = g_lStoredComponentTargetCounts[nCompIndex] - g_lComponentCountsActual[nCompIndex];
        fDiff = (float)lDiff / g_CalibrationData.m_fWeightConstant;
        if( g_CalibrationData.m_nStageFillEn == TWOSTAGEFILLINGID || lDiff < MIN3RDFILLTARGET )
        {  //SET2S
            g_lStoredComponentCountsStage2[nCompIndex] = 0;
            CalculateTargetStage3(nComponent, lDiff );
            bTargetReached = FALSE;
        }
        else
        {   // PROCEED
            g_lComponentTargetCounts[nCompIndex] = (lDiff * PERCENTAGEFILL) / 100;

            // JSR CONVERTTARGET - expanded inline
            CalcCompNTime( nComponent );
            g_fComponentTargetWeight[nCompIndex] = (float)g_lComponentTargetCounts[nCompIndex] / g_CalibrationData.m_fWeightConstant;
            // Convert to English units.
            g_fComponentTargetWeightI[nCompIndex] = g_fComponentTargetWeight[nCompIndex] * WEIGHT_CONVERSION_FACTOR;

            g_nFillStage = STAGE_2;
            g_nStageSeq[nCompIndex] = STAGE_2;
            bTargetReached = FALSE;
        }

    }

    return( bTargetReached );
}

//////////////////////////////////////////////////////
// CalculateFinalWeight( int nComponent )                 from ASM = CALCULATEFINALWEIGHT
//
// Calculates the final weight for the component by adding all stages together.
//
// EXIT:
//
// M.McKiernan                                          16-09-2004
// First pass.
//
///////////////////////////////////////////////////////
void CalculateFinalWeight( int nComponent )
{
    int nCompIndex = nComponent - 1;
    long lSum;
    // sum counts for all stages.
    lSum = g_lStoredComponentCountsStage1[nCompIndex] + g_lStoredComponentCountsStage2[nCompIndex] + g_lComponentCountsActual[nCompIndex];

    g_lComponentCountsActual[nCompIndex] = lSum;
    // calculate actual weight.
    g_fComponentActualWeight[nCompIndex] = (float)g_lComponentCountsActual[nCompIndex] / g_CalibrationData.m_fWeightConstant;

    // restore original target
    g_lComponentTargetCounts[nCompIndex] = g_lStoredComponentTargetCounts[nCompIndex];
    // CALCTARGETWT
    g_fComponentTargetWeight[nCompIndex] = (float)g_lComponentTargetCounts[nCompIndex] / g_CalibrationData.m_fWeightConstant;
    // Convert to English units.
    g_fComponentTargetWeightI[nCompIndex] = g_fComponentTargetWeight[nCompIndex] * WEIGHT_CONVERSION_FACTOR;
    CalculateComponentRoundRobinWeight(nComponent); // asm CALCRROBINFORWEIGHT

    g_nFillStage = 0;   //
   g_nStageSeq[nCompIndex] = 0; //

}
//////////////////////////////////////////////////////
// CalculateTargetStage3( int nComponent, long lDiff )                 from ASM = CALCTARGETSTAGE3
//
// Calculates the stage 3 target.
//
// EXIT:
//
// M.McKiernan                                          16-09-2004
// First pass.
//
///////////////////////////////////////////////////////
void CalculateTargetStage3( int nComponent, long lDiff )
{
    int nCompIndex = nComponent - 1;
    // target = difference, i.e. diff between original target and current actual counts.
    g_lComponentTargetCounts[nCompIndex] = lDiff;

   g_nFillStage = STAGE_1;   // indicate that stage 3 of fill to start
   g_nStageSeq[nCompIndex] = STAGE_1; // indicate stage 3 in progress

                // JSR CONVERTTARGET - expanded inline
    CalcCompNTime( nComponent );
    g_fComponentTargetWeight[nCompIndex] = (float)g_lComponentTargetCounts[nCompIndex] / g_CalibrationData.m_fWeightConstant;
            // Convert to English units.
    g_fComponentTargetWeightI[nCompIndex] = g_fComponentTargetWeight[nCompIndex] * WEIGHT_CONVERSION_FACTOR;



}

//////////////////////////////////////////////////////
// StoreStageFillData( int nComponent )                 from ASM = STORESTAGEFILLDATA
//
// Store stage fill data and decide on the calculation to be performed.
//
// EXIT:
//
// M.McKiernan                                          16-09-2004
// First pass.
//
///////////////////////////////////////////////////////
void StoreStageFillData( int nComponent )
{

volatile BOOL bTargetReached = FALSE;    //--todo-- check that this is okay.

    int nCompIndex = nComponent - 1;

    if( g_CalibrationData.m_nStageFillEn )  // multi-stage fill enabled??
    {
        if(g_nStageSeq[nCompIndex] == STAGE_1)
        {
            CalculateFinalWeight( nComponent );
        }
        else if(g_nStageSeq[nCompIndex] == STAGE_3)  // asm = NOSTRT
        {
        g_nFillStage = STAGE_2;   // set for stage 2
        g_nStageSeq[nCompIndex] = STAGE_2; // indicate stage 2
            // store counts for stage.  --REVIEW-- stage naming confusing!!
            g_lStoredComponentCountsStage1[nCompIndex] = g_lComponentCountsActual[nCompIndex];
            bTargetReached = CalculateStage2Fill( nComponent );
            // --REVIEW-- some code in assembler here (CONSTFL) but not doing anything.
        }
        else if(g_nStageSeq[nCompIndex] == STAGE_2)  // asm = NOTST1
        {
        g_nFillStage = STAGE_1;   // set for stage 1
        g_nStageSeq[nCompIndex] = STAGE_1; // indicate stage 1
            // store counts for stage.  --REVIEW-- stage naming confusing!!
            g_lStoredComponentCountsStage2[nCompIndex] = g_lComponentCountsActual[nCompIndex];

            bTargetReached = CalculateStage3Fill( nComponent );

        }

    }



}
//////////////////////////////////////////////////////
// CalculateStage3Fill( int nComponent )                 from ASM = CALCSTAGE3FILL
//
// Calculates the target for the 3rd  stage of the Multi stage filling process.
//
// EXIT:
//
// M.McKiernan                                          16-09-2004
// First pass.
//
///////////////////////////////////////////////////////
BOOL CalculateStage3Fill( int nComponent  )
{
    int nCompIndex = nComponent - 1;
    BOOL bTargetReached = FALSE;
    long lDiff,lSum1and2;

    lSum1and2 = g_lStoredComponentCountsStage1[nCompIndex] + g_lStoredComponentCountsStage2[nCompIndex];
    if( lSum1and2 > g_lStoredComponentTargetCounts[nCompIndex] )
    {
        // --REVIEW-- The stage 2 counts were being cleared in assembler, but cannot see why.
//      g_lStoredComponentCountsStage2[nCompIndex] = 0;
        CalculateFinalWeight( nComponent );
        bTargetReached = TRUE;
    }
    else
    {
        //asm = CALCTARGETSTAGE3
        lDiff = g_lStoredComponentTargetCounts[nCompIndex] - lSum1and2;
        CalculateTargetStage3( nComponent, lDiff );
        bTargetReached = FALSE;
    }

    return( bTargetReached );
}

//////////////////////////////////////////////////////
// CheckForMultipleToNormalFill                from ASM = CHECKFORMULTONORMALFILL
//
// Checks for multiple fill to normal fill transition
// If condition met, g_bResetStageSeq flag is set, indicating that multiple filling option
// should be reset in the foreground program.
//
// EXIT:
//
// M.McKiernan                                          17-09-2004
// First pass.
//
///////////////////////////////////////////////////////
void CheckForMultipleToNormalFill( void )
{
    if(g_bLayeringTemp != g_CalibrationData.m_bLayering  || g_nStageFillEnTemp != g_nStageFillEnHistory)
    {
        g_bResetStageSeq = TRUE; // Reset stage seq in when no cycling
    }
}


/*
;*****************************************************************************;
; FUNCTION NAME : CHECKFORMULTONORMALFILL                                     ;
; FUNCTION      : CHECKS FOR MULTIPLE FILL TO NORMAL FILL TRANSITION         ;
; INPUTS        : NONE                                                        ;                       ;
;*****************************************************************************;

CHECKFORMULTONORMALFILL:
        LDAA    LAYERINGTEMP
        CMPA    LAYERING
        BNE     RESSEQ                  ; RESET
        LDAA    STAGEFILLENTEMP         ; NORMAL FILLING
        CMPA    STAGEFILLENH            ; CHECK HISTORY
        BEQ     XITMLC                  ; NO CHANGE
RESSEQ  LDAA    #1
        STAA    RESETSTAGESEQ           ; RESET STAGE SEQ IN WHEN NO CYCLING
XITMLC  RTS

*/

/*
; CALCULATES CMP1CNTGSTR - (CMP1CNTGSTR1 + CMP1CNTGSTR2)


CALCSTAGE3FILL:
        JSR     CLRAC
        LDX     #CMP1CNTGSTR1
        LDAB    COMPNO
        DECB
        ABX
        ABX
        ABX
        JSR     AMOVE
        LDX     #CMP1CNTGSTR2
        LDAB    COMPNO
        DECB
        ABX
        ABX
        ABX
        JSR     CMOVE
        JSR     ADD
        LDX     #MATHCREG
        JSR     IFEREG
        LDX     #CMP1CNTGSTR
        LDAB    COMPNO
        DECB
        ABX
        ABX
        ABX
        JSR     AMOVE
        JSR     SUB

        TST     MATHDREG
        BEQ     CALCTARGETSTAGE3       ; DO STAGE 3 CALC

        LDX     #CMP1CNTGSTR2
        LDAB    COMPNO
        DECB
        ABX
        ABX
        ABX
        CLRW    0,X
        CLR     2,X
;        CLRW     CMP1CNTGSTR2
;        CLR      CMP1CNTGSTR2+2
        JSR     CALCULATEFINALWEIGHT    ; CALCULATE WEIGHT
        LBRA     ATTARST3



CALCTARGETSTAGE3:
        LDX     #CMP1CNTG
        LDAB    COMPNO
        DECB
        ABX
        ABX
        ABX

        LDAB    EREG+2
        STAB    0,X
        LDE     EREG+3
        STE     1,X

        LDAA    #ONE
        STAA    FILLSTAGE
        LDX     #STAGESEQ1
        LDAB    COMPNO
        DECB
        ABX
        STAA    0,X

        JSR     CONVERTTARGET
        CLRA
XITSTG  RTS

ATTARST3 LDAA   #1
         RTS

*/
/*
; STORE STAGE FILL DATA AND DECIDE ON THE CALCULATION TO BE PERFORMED.


STORESTAGEFILLDATA:

        TST     STAGEFILLEN
        LBEQ     XITSTR          ; NORMAL FILLING PROCESS

        LDX     #STAGESEQ1
        LDAB    COMPNO
        DECB
        ABX
        LDAA    0,X

        CMPA    #ONE
        LBNE     NOSTRT           ; NO
        JSR     CALCULATEFINALWEIGHT ; CALCULATE WEIGHT
        LBRA     XITSTR

NOSTRT:
        LDX     #STAGESEQ1
        LDAB    COMPNO
        DECB
        ABX
        LDAA    0,X
        CMPA    #THREE
        BNE     NOTST1          ; NO

        LDAA    #TWO            ; SET TO THE NEXT STAGE
        STAA    FILLSTAGE
        LDX     #STAGESEQ1
        LDAB    COMPNO
        DECB
        ABX
        STAA    0,X


        LDX     #CMP1CNAC
        LDAB    COMPNO
        DECB
        ABX
        ABX
        ABX

        LDAB    #RAMBANK
        TBZK
        LDZ     #CMP1CNTGSTR1
        LDAB    COMPNO
        DECB
        ABZ
        ABZ
        ABZ

        LDD     0,X
        STD     0,Z
        LDAB    2,X
        STAB    2,Z
        JSR     CALCSTAGE2FILL          ; CALC STAGE 2 OF FILL
        TSTA                            ; AT TARGET ?
        BNE     XITSTR

CONSTFL

        LDX     SEQPOINTER              ; IF FILL BY WEIGHT DO NOT STOP FLOW RATE CALC.
        LDAB    0,X
        ANDB    #FILLBYWEIGHT
        BNE     NOFLRCL1

NOFLRCL1:
        BRA     XITSTR
NOTST1  CMPA    #TWO
        BNE     XITSTR          ; NO
        LDX     SEQPOINTER
        LDAB    0,X
        ANDB    #$0F
        DECB
        LDX     #CMP1CNAC
        ABX
        ABX
        ABX


        LDAB    #RAMBANK
        TBZK
        LDZ     #CMP1CNTGSTR2
        LDAB    COMPNO
        DECB
        ABZ
        ABZ
        ABZ

        LDD     0,X
        STD     0,Z
;        STD     CMP1CNTGSTR2
        LDAB    2,X
        STAB    2,Z
;        STAB    CMP1CNTGSTR2+2

        LDAA    #ONE                    ; SET TO GO TO LAST STAGE.
        STAA    FILLSTAGE
        LDX     #STAGESEQ1
        LDAB    COMPNO
        DECB
        ABX
        STAA    0,X

        JSR     CALCSTAGE3FILL
        TSTA                            ; AT TARGET ?
        BNE     XITSTR
        LDAA    #ONE
        STAA    FILLSTAGE
        LDX     #STAGESEQ1
        LDAB    COMPNO
        DECB
        ABX
        STAA    0,X
XITSTR  RTS


*/
/*
CALCTARGETSTAGE3:
        LDX     #CMP1CNTG
        LDAB    COMPNO
        DECB
        ABX
        ABX
        ABX

        LDAB    EREG+2
        STAB    0,X
        LDE     EREG+3
        STE     1,X

        LDAA    #ONE
        STAA    FILLSTAGE
        LDX     #STAGESEQ1
        LDAB    COMPNO
        DECB
        ABX
        STAA    0,X

        JSR     CONVERTTARGET
        CLRA
XITSTG  RTS

ATTARST3 LDAA   #1
         RTS

*/
/*
CALCULATEFINALWEIGHT:
        LDX     #CMP1CNTGSTR1
        LDAB    COMPNO
        DECB
        ABX
        ABX
        ABX
        JSR     AMOVE
        LDX     #CMP1CNTGSTR2
        LDAB    COMPNO
        DECB
        ABX
        ABX
        ABX
        JSR     CMOVE
        JSR     ADD
        LDX     #AREG
        JSR     IFEREG

        LDAB    COMPNO
        DECB
        LDX     #CMP1CNAC
        ABX
        ABX
        ABX
        PSHM    X
        JSR     CMOVE
        JSR     ADD
        PULM    X
        JSR     EMOVE
        JSR     CMPWCAL          ; CACULATE WEIGHT FOR THIS COMPONENT
        LDX     #CMP1ACTWGT     ; COMPONENT #1 TARGET WEIGHT
        LDAB    COMPNO          ; READ COMPONENT COUNTER
        DECB                    ;
        ABX
        ABX
        ABX
        LDD     EREG
        STD     0,X             ; STORE WEIGHT
        LDAA    EREG+2          ;
        STAA    2,X             ;


; NOW REVERT BACK TO THE EXISTING TARGET
;
        LDX     #CMP1CNTG
        LDAB    COMPNO
        DECB
        ABX
        ABX
        ABX
        LDAB    #RAMBANK
        TBZK
        LDZ     #CMP1CNTGSTR
        LDAB    COMPNO
        DECB
        ABZ
        ABZ
        ABZ
;        LDD     CMP1CNTGSTR     ; READ ORIGINAL TARGET
        LDD     0,Z
        STD     0,X
;        LDAA    CMP1CNTGSTR+2
        LDAA    2,Z
        STAA    2,X             ; STORE

        JSR     CALCTARGETWT
        CLR     FILLSTAGE
        LDX     #STAGESEQ1
        LDAB    COMPNO
        DECB
        ABX
        CLR    0,X
        RTS


*/


/*


PERCENTAGEFILL  EQU     $70              ; % FILL

PERCENTAGEFILLBW EQU    $50
MINTARGETWEIGHT EQU     $0150
TWOSTAGEFILLINGID EQU   1               ; TWO STAGE FILLING ID
THREESTAGEFILLINGID EQU 2               ; THREE STAGE FILL ID

MIN3RDFILLTARGET  EQU   $0070           ; MINIMUM FILL FOR STAGE 3 TARGET.



;
;       PROGRAMS
;
        XREF    CLAREG,ITCREG,MUL,BCDHEX3X,CALCMNTIM,CONVTARTOLBS,CMPWCAL,CLRAC,ITAREG,SUB
        XREF    IFEREG,ADD,AMOVE,CMOVE,EMOVE

        PUBLIC  STORESTAGEFILLDATA,CALCPERTARGET,CHECKFORMULTONORMALFILL



MYPROGS      SECTION


; CALCULATES THE REQUIRED TARGET FOR THE VARIOUS COMPONENTS

CALCPERTARGET:
        JSR     CLRAC                 ; A = C, = 0
        LDX     SEQPOINTER
        LDAB    0,X
        ANDB    #$0F
        STAB    COMPNO

        LDX     #STAGESEQ1
        LDAB    COMPNO
        DECB
        ABX
        TST    0,X
        LBNE     XITCMC
        LDAB    COMPNO
        DECB
        LDX     #CMP1CNTG
        ABX
        ABX
        ABX
        PSHM    X                       ; SAVE ADDRESS
        LDAB    #RAMBANK
        TBZK
        LDZ     #CMP1CNTGSTR
        LDAB    COMPNO
        DECB
        ABZ
        ABZ
        ABZ

        LDD     0,X
        STD     0,Z                      ; STORE ORIGINAL TARGET
        LDAA    2,X
        STAA    2,Z
        JSR     CMOVE
        PULM    X
        PSHM    X
        LDX     SEQPOINTER
        LDAB    0,X
        ANDB    #FILLBYWEIGHT
        BEQ     NOFBW                   ; NO FILL BY WEIGHT
        LDX     #COPYFLOWRATECMP1
        LDAB    COMPNO
        DECB
        ABX                             ; PICK UP CORRECT BUFFER

        LDAA    #1
        STAA    0,X
        LDAA    #PERCENTAGEFILLBW       ; % USED WHEN FILL BY WEIGHT
        BRA     STRPER                  ; STORE %
NOFBW:
        LDAA    #PERCENTAGEFILL         ; PERCENTAGE FILL
STRPER  STAA    AREG+4
        JSR     MUL
        PULM    X
        LDAB    EREG+1
        STAB    0,X
        LDE     EREG+2
        STE     1,X
        JSR     CONVERTTARGET
        LDAA    #THREE
        STAA    FILLSTAGE
        LDX     #STAGESEQ1
        LDAB    COMPNO
        DECB
        ABX
        STAA    0,X
        LDAA    #ONE
        LDX     #MULFILL1STARTED
        LDAB    COMPNO
        DECB
        ABX
        STAA    0,X             ; SET MULTIPLE FILLING STARTED FLAG
XITCMC  RTS




;       0.7 * (CMP1CNTGSTR - CMPNCNAC)  0.7 * (1000-700)

CALCSTAGE2FILL:

        LDX     #COPYFLOWRATECMP1
        LDAB    COMPNO
        DECB
        ABX                             ; PICK UP CORRECT BUFFER
        TST     0,X
        BEQ     NOFRCPY                 ; NO FLOW RATE COPY.
        CLR     0,X
        LDX     #CMP1CPI
        LDAB    COMPNO
        DECB
        ABX
        ABX
        ABX
        LDE     0,X
        LDAA    2,X                     ; READ FLOW RATE FOR THIS COMPONENT

        LDX     #CMP1CPIST2
        LDAB    COMPNO
        DECB
        ABX
        ABX
        ABX
        STE     0,X
        STAA    2,X

        LDX     #CMP1CPIST3
        LDAB    COMPNO
        DECB
        ABX
        ABX
        ABX
        STE     0,X
        STAA    2,X



NOFRCPY JSR     CLRAC
        LDX     #CMP1CNTGSTR            ; ORIGINAL SET POINT
        LDAB    COMPNO
        DECB
        ABX
        ABX
        ABX

        JSR     AMOVE
        LDX     #CMP1CNAC
        LDAB    COMPNO
        DECB
        ABX
        ABX
        ABX
        JSR     CMOVE                  ; ACTUAL TO C
        JSR     SUB                     ;

;       CHECK IF THE DISPENSED MATERIAL IS ABOVE THE TARGET
;
        TST     MATHDREG
        BEQ     CLCSTG         ; CALCUALTE STAGE 2

        LDX     #CMP1CNTGSTR1
        LDAB    COMPNO
        DECB
        ABX
        ABX
        ABX
        CLRW    0,X
        CLR     2,X

        LDX     #CMP1CNTGSTR2
        LDAB    COMPNO
        DECB
        ABX
        ABX
        ABX
        CLRW    0,X
        CLR     2,X


        JSR     CALCULATEFINALWEIGHT    ; CALCULATE WEIGHT
        LBRA     ATTARGET


;       SAVE THE TARGET
;
CLCSTG  LDX     #TEMPS
        JSR     IFEREG
        JSR     CMPWCAL
        LDAA    STAGEFILLEN     ; READ TYPE OF STAGED FILLING
        CMPA    #TWOSTAGEFILLINGID ; TWO STAGE FILLING
        BEQ     SET2S           ; SET TO 2 STAGE

        LDD     EREG+1
        CPD     #MIN3RDFILLTARGET
        BHI     PROCEED         ; NOT AT MIN YET

; LOWER THAN MIN

SET2S   LDX     #CMP1CNTGSTR2
        LDAB    COMPNO
        DECB
        ABX
        ABX
        ABX
        CLRW    0,X
        CLR     2,X

        LDX     TEMPS
        STX     EREG
        LDX     TEMPS+2
        STX     EREG+2
        LDAA    TEMPS+4
        STAA    EREG+4
        JSR     CALCTARGETSTAGE3
        BRA     XITST2




PROCEED:
        LDX     #TEMPS
        JSR     ITCREG

        JSR     CLAREG
        LDAA    #PERCENTAGEFILL         ; PERCENTAGE FILL
        STAA    AREG+4
        JSR     MUL

        LDX     #CMP1CNTG
        LDAB    COMPNO
        DECB
        ABX
        ABX
        ABX

        LDAB    EREG+1
        STAB    0,X
        LDE     EREG+2
        STE     1,X

        JSR     CONVERTTARGET
        LDAA    #TWO
        STAA    FILLSTAGE
        LDX     #STAGESEQ1
        LDAB    COMPNO
        DECB
        ABX
        STAA    0,X

        CLRA                    ; NOT AT TARGET
XITST2  RTS


ATTARGET:
        LDAA    #1              ; INDICATE TARGET IS REACHED
        BRA     XITST2

; CALCULATES CMP1CNTGSTR - (CMP1CNTGSTR1 + CMP1CNTGSTR2)


CALCSTAGE3FILL:
        JSR     CLRAC
        LDX     #CMP1CNTGSTR1
        LDAB    COMPNO
        DECB
        ABX
        ABX
        ABX
        JSR     AMOVE
        LDX     #CMP1CNTGSTR2
        LDAB    COMPNO
        DECB
        ABX
        ABX
        ABX
        JSR     CMOVE
        JSR     ADD
        LDX     #MATHCREG
        JSR     IFEREG
        LDX     #CMP1CNTGSTR
        LDAB    COMPNO
        DECB
        ABX
        ABX
        ABX
        JSR     AMOVE
        JSR     SUB

        TST     MATHDREG
        BEQ     CALCTARGETSTAGE3       ; DO STAGE 3 CALC

        LDX     #CMP1CNTGSTR2
        LDAB    COMPNO
        DECB
        ABX
        ABX
        ABX
        CLRW    0,X
        CLR     2,X
;        CLRW     CMP1CNTGSTR2
;        CLR      CMP1CNTGSTR2+2
        JSR     CALCULATEFINALWEIGHT    ; CALCULATE WEIGHT
        LBRA     ATTARST3



CALCTARGETSTAGE3:
        LDX     #CMP1CNTG
        LDAB    COMPNO
        DECB
        ABX
        ABX
        ABX

        LDAB    EREG+2
        STAB    0,X
        LDE     EREG+3
        STE     1,X

        LDAA    #ONE
        STAA    FILLSTAGE
        LDX     #STAGESEQ1
        LDAB    COMPNO
        DECB
        ABX
        STAA    0,X

        JSR     CONVERTTARGET
        CLRA
XITSTG  RTS

ATTARST3 LDAA   #1
         RTS

; STORE STAGE FILL DATA AND DECIDE ON THE CALCULATION TO BE PERFORMED.


STORESTAGEFILLDATA:

        TST     STAGEFILLEN
        LBEQ     XITSTR          ; NORMAL FILLING PROCESS

        LDX     #STAGESEQ1
        LDAB    COMPNO
        DECB
        ABX
        LDAA    0,X

        CMPA    #ONE
        LBNE     NOSTRT           ; NO
        JSR     CALCULATEFINALWEIGHT ; CALCULATE WEIGHT
        LBRA     XITSTR

NOSTRT:
        LDX     #STAGESEQ1
        LDAB    COMPNO
        DECB
        ABX
        LDAA    0,X
        CMPA    #THREE
        BNE     NOTST1          ; NO

        LDAA    #TWO            ; SET TO THE NEXT STAGE
        STAA    FILLSTAGE
        LDX     #STAGESEQ1
        LDAB    COMPNO
        DECB
        ABX
        STAA    0,X


        LDX     #CMP1CNAC
        LDAB    COMPNO
        DECB
        ABX
        ABX
        ABX

        LDAB    #RAMBANK
        TBZK
        LDZ     #CMP1CNTGSTR1
        LDAB    COMPNO
        DECB
        ABZ
        ABZ
        ABZ

        LDD     0,X
        STD     0,Z
        LDAB    2,X
        STAB    2,Z
        JSR     CALCSTAGE2FILL          ; CALC STAGE 2 OF FILL
        TSTA                            ; AT TARGET ?
        BNE     XITSTR

CONSTFL

        LDX     SEQPOINTER              ; IF FILL BY WEIGHT DO NOT STOP FLOW RATE CALC.
        LDAB    0,X
        ANDB    #FILLBYWEIGHT
        BNE     NOFLRCL1

NOFLRCL1:
        BRA     XITSTR
NOTST1  CMPA    #TWO
        BNE     XITSTR          ; NO
        LDX     SEQPOINTER
        LDAB    0,X
        ANDB    #$0F
        DECB
        LDX     #CMP1CNAC
        ABX
        ABX
        ABX


        LDAB    #RAMBANK
        TBZK
        LDZ     #CMP1CNTGSTR2
        LDAB    COMPNO
        DECB
        ABZ
        ABZ
        ABZ

        LDD     0,X
        STD     0,Z
;        STD     CMP1CNTGSTR2
        LDAB    2,X
        STAB    2,Z
;        STAB    CMP1CNTGSTR2+2

        LDAA    #ONE                    ; SET TO GO TO LAST STAGE.
        STAA    FILLSTAGE
        LDX     #STAGESEQ1
        LDAB    COMPNO
        DECB
        ABX
        STAA    0,X

        JSR     CALCSTAGE3FILL
        TSTA                            ; AT TARGET ?
        BNE     XITSTR
        LDAA    #ONE
        STAA    FILLSTAGE
        LDX     #STAGESEQ1
        LDAB    COMPNO
        DECB
        ABX
        STAA    0,X
XITSTR  RTS


CALCULATEFINALWEIGHT:
        LDX     #CMP1CNTGSTR1
        LDAB    COMPNO
        DECB
        ABX
        ABX
        ABX
        JSR     AMOVE
        LDX     #CMP1CNTGSTR2
        LDAB    COMPNO
        DECB
        ABX
        ABX
        ABX
        JSR     CMOVE
        JSR     ADD
        LDX     #AREG
        JSR     IFEREG

        LDAB    COMPNO
        DECB
        LDX     #CMP1CNAC
        ABX
        ABX
        ABX
        PSHM    X
        JSR     CMOVE
        JSR     ADD
        PULM    X
        JSR     EMOVE
        JSR     CMPWCAL          ; CACULATE WEIGHT FOR THIS COMPONENT
        LDX     #CMP1ACTWGT     ; COMPONENT #1 TARGET WEIGHT
        LDAB    COMPNO          ; READ COMPONENT COUNTER
        DECB                    ;
        ABX
        ABX
        ABX
        LDD     EREG
        STD     0,X             ; STORE WEIGHT
        LDAA    EREG+2          ;
        STAA    2,X             ;


; NOW REVERT BACK TO THE EXISTING TARGET
;
        LDX     #CMP1CNTG
        LDAB    COMPNO
        DECB
        ABX
        ABX
        ABX
        LDAB    #RAMBANK
        TBZK
        LDZ     #CMP1CNTGSTR
        LDAB    COMPNO
        DECB
        ABZ
        ABZ
        ABZ
;        LDD     CMP1CNTGSTR     ; READ ORIGINAL TARGET
        LDD     0,Z
        STD     0,X
;        LDAA    CMP1CNTGSTR+2
        LDAA    2,Z
        STAA    2,X             ; STORE

        JSR     CALCTARGETWT
        CLR     FILLSTAGE
        LDX     #STAGESEQ1
        LDAB    COMPNO
        DECB
        ABX
        CLR    0,X
        RTS



CONVERTTARGET:
        LDAB    COMPNO
        DECB
        LDX     #CMP1CNTG
        ABX
        ABX
        ABX
        JSR     BCDHEX3X        ; CONVERT BACK TO HEX
        PSHB
        PSHM    E               ;SAVE RESULT
        LDX     #CMP1CNTGH      ; COUNTS FOR COMPONENTS.
        LDAB    COMPNO
        DECB
        ABX
        ABX
        ABX
        PULM    E
        PULB
        STAB    0,X             ; STORE HEX TARGET
        STE     1,X             ; LSB
        JSR     CALCMNTIM       ; COMP #N TIMES FOR #2...N


CALCTARGETWT:
        LDX     SEQPOINTER
        LDAB    0,X
        ANDB    #$0F
        DECB
        LDX     #CMP1CNTG       ; LOAD COMP #1 TARGET COUNT STORAGE.
        ABX
        ABX
        ABX
        CLRW    EREG
        LDD     0,X
        STD     EREG+2
        LDAA    2,X
        STAA    EREG+4          ; NEXT CALCULATION EXPECTING COUNTS IN EREG.
        JSR     CMPWCAL          ; CACULATE WEIGHT FOR THIS COMPONENT
        LDX     SEQPOINTER
        LDAB    0,X
        ANDB    #$0F
        DECB
        LDX     #CMP1TARWGT     ; COMPONENT #1 TARGET WEIGHT
        ABX
        ABX
        ABX
        LDD     EREG
        STD     0,X             ; STORE WEIGHT
        LDAA    EREG+2          ;
        STAA    2,X             ;
        JSR     CONVTARTOLBS    ; COMP NO HOLDS
        RTS



;*****************************************************************************;
; FUNCTION NAME : CHECKFORMULTONORMALFILL                                     ;
; FUNCTION      : CHECKS FOR MULTIPLE FILL TO NORMAL FILL TRANSITION         ;
; INPUTS        : NONE                                                        ;                       ;
;*****************************************************************************;

CHECKFORMULTONORMALFILL:
        LDAA    LAYERINGTEMP
        CMPA    LAYERING
        BNE     RESSEQ                  ; RESET
        LDAA    STAGEFILLENTEMP         ; NORMAL FILLING
        CMPA    STAGEFILLENH            ; CHECK HISTORY
        BEQ     XITMLC                  ; NO CHANGE
RESSEQ  LDAA    #1
        STAA    RESETSTAGESEQ           ; RESET STAGE SEQ IN WHEN NO CYCLING
XITMLC  RTS


// mce12-15


PERCENTAGEFILL  EQU     $70              ; % FILL

PERCENTAGEFILLBW EQU    $50
MINTARGETWEIGHT EQU     $0150
TWOSTAGEFILLINGID EQU   1               ; TWO STAGE FILLING ID
THREESTAGEFILLINGID EQU 2               ; THREE STAGE FILL ID

MIN3RDFILLTARGET    EQU   $0100           ; MINIMUM FILL FOR STAGE 3 TARGET.
MINMULTISTAGEWEIGHT EQU   $0100           ; MIN MULTIPLE STAGE FILL WEIGHT.



;
;       PROGRAMS
;
        XREF    CLAREG,ITCREG,MUL,BCDHEX3X,CALCMNTIM,CONVTARTOLBS,CMPWCAL,CLRAC,ITAREG,SUB
        XREF    IFEREG,ADD,AMOVE,CMOVE,EMOVE

        PUBLIC  STORESTAGEFILLDATA,CALCPERTARGET,CHECKFORMULTONORMALFILL



MYPROGS      SECTION


; CALCULATES THE REQUIRED TARGET FOR THE VARIOUS COMPONENTS

CALCPERTARGET:
        JSR     CLRAC                 ; A = C, = 0
        LDX     SEQPOINTER
        LDAB    0,X
        TBA
        ANDB    #$0F
        STAB    COMPNO
        ANDA    #REGBIT             ; IS THIS COMPONENT REGRIND.
        LBNE    XITCMC              ; NO MULTIPLE FILLING IF REGRIND COMPONENT ENABLED.

; CHECK IF THE MIN WEIGHT HAS BEEN EXCEEDED FOR MULTIPLE FILLING.
;
        LDAB   COMPNO
        DECB
        LDX     #CMP1TARWGT     ; COMPONENT #1 TARGET WEIGHT
        ABX
        ABX
        ABX
        TST     0,X             ; CHECK MSB
        BNE     DOMSFL          ; DO MULTISTAGE FILL IF NON ZERO.
        LDD     1,X
        CPD     #MINMULTISTAGEWEIGHT
        LBLS    XITCMC          ; IF LESS THAN 10 GRAMS THEN NO MULTIPLE FILL
DOMSFL  LDX     #STAGESEQ1
        LDAB    COMPNO
        DECB
        ABX
        TST    0,X
        LBNE     XITCMC
        LDAB    COMPNO
        DECB
        LDX     #CMP1CNTG
        ABX
        ABX
        ABX
        PSHM    X                       ; SAVE ADDRESS
        LDAB    #RAMBANK
        TBZK
        LDZ     #CMP1CNTGSTR
        LDAB    COMPNO
        DECB
        ABZ
        ABZ
        ABZ
        LDD     0,X
        STD     0,Z                      ; STORE ORIGINAL TARGET
        LDAA    2,X
        STAA    2,Z
        JSR     CMOVE
        PULM    X
        PSHM    X
        LDX     SEQPOINTER
        LDAB    0,X
        ANDB    #FILLBYWEIGHT
        BEQ     NOFBW                   ; NO FILL BY WEIGHT
        LDX     #COPYFLOWRATECMP1
        LDAB    COMPNO
        DECB
        ABX                             ; PICK UP CORRECT BUFFER
        LDAA    #1
        STAA    0,X
        LDAA    #PERCENTAGEFILLBW       ; % USED WHEN FILL BY WEIGHT
        BRA     STRPER                  ; STORE %
NOFBW:
        LDAA    #PERCENTAGEFILL         ; PERCENTAGE FILL
STRPER  STAA    AREG+4
        JSR     MUL
        PULM    X
        LDAB    EREG+1
        STAB    0,X
        LDE     EREG+2
        STE     1,X
        JSR     CONVERTTARGET
        LDAA    #THREE
        STAA    FILLSTAGE
        LDX     #STAGESEQ1
        LDAB    COMPNO
        DECB
        ABX
        STAA    0,X
        LDAA    #ONE
        LDX     #MULFILL1STARTED
        LDAB    COMPNO
        DECB
        ABX
        STAA    0,X             ; SET MULTIPLE FILLING STARTED FLAG
XITCMC  RTS




;       0.7 * (CMP1CNTGSTR - CMPNCNAC)  0.7 * (1000-700)

CALCSTAGE2FILL:

        LDX     #COPYFLOWRATECMP1
        LDAB    COMPNO
        DECB
        ABX                             ; PICK UP CORRECT BUFFER
        TST     0,X
        BEQ     NOFRCPY                 ; NO FLOW RATE COPY.
        CLR     0,X
        LDX     #CMP1CPI
        LDAB    COMPNO
        DECB
        ABX
        ABX
        ABX
        LDE     0,X
        LDAA    2,X                     ; READ FLOW RATE FOR THIS COMPONENT

        LDX     #CMP1CPIST2
        LDAB    COMPNO
        DECB
        ABX
        ABX
        ABX
        STE     0,X
        STAA    2,X

        LDX     #CMP1CPIST3
        LDAB    COMPNO
        DECB
        ABX
        ABX
        ABX
        STE     0,X
        STAA    2,X



NOFRCPY JSR     CLRAC
        LDX     #CMP1CNTGSTR            ; ORIGINAL SET POINT
        LDAB    COMPNO
        DECB
        ABX
        ABX
        ABX

        JSR     AMOVE
        LDX     #CMP1CNAC
        LDAB    COMPNO
        DECB
        ABX
        ABX
        ABX
        JSR     CMOVE                  ; ACTUAL TO C
        JSR     SUB                     ;

;       CHECK IF THE DISPENSED MATERIAL IS ABOVE THE TARGET
;
        TST     MATHDREG
        BEQ     CLCSTG         ; CALCUALTE STAGE 2

        LDX     #CMP1CNTGSTR1
        LDAB    COMPNO
        DECB
        ABX
        ABX
        ABX
        CLRW    0,X
        CLR     2,X

        LDX     #CMP1CNTGSTR2
        LDAB    COMPNO
        DECB
        ABX
        ABX
        ABX
        CLRW    0,X
        CLR     2,X


        JSR     CALCULATEFINALWEIGHT    ; CALCULATE WEIGHT
        LBRA     ATTARGET


;       SAVE THE TARGET
;
CLCSTG  LDX     #TEMPS
        JSR     IFEREG
        JSR     CMPWCAL
        LDAA    STAGEFILLEN     ; READ TYPE OF STAGED FILLING
        CMPA    #TWOSTAGEFILLINGID ; TWO STAGE FILLING
        BEQ     SET2S           ; SET TO 2 STAGE

        LDD     EREG+1
        CPD     #MIN3RDFILLTARGET
        BHI     PROCEED         ; NOT AT MIN YET

; LOWER THAN MIN

SET2S   LDX     #CMP1CNTGSTR2
        LDAB    COMPNO
        DECB
        ABX
        ABX
        ABX
        CLRW    0,X
        CLR     2,X

        LDX     TEMPS
        STX     EREG
        LDX     TEMPS+2
        STX     EREG+2
        LDAA    TEMPS+4
        STAA    EREG+4
        JSR     CALCTARGETSTAGE3
        BRA     XITST2




PROCEED:
        LDX     #TEMPS
        JSR     ITCREG

        JSR     CLAREG
        LDAA    #PERCENTAGEFILL         ; PERCENTAGE FILL
        STAA    AREG+4
        JSR     MUL

        LDX     #CMP1CNTG
        LDAB    COMPNO
        DECB
        ABX
        ABX
        ABX

        LDAB    EREG+1
        STAB    0,X
        LDE     EREG+2
        STE     1,X

        JSR     CONVERTTARGET
        LDAA    #TWO
        STAA    FILLSTAGE
        LDX     #STAGESEQ1
        LDAB    COMPNO
        DECB
        ABX
        STAA    0,X

        CLRA                    ; NOT AT TARGET
XITST2  RTS


ATTARGET:
        LDAA    #1              ; INDICATE TARGET IS REACHED
        BRA     XITST2

; CALCULATES CMP1CNTGSTR - (CMP1CNTGSTR1 + CMP1CNTGSTR2)


CALCSTAGE3FILL:
        JSR     CLRAC
        LDX     #CMP1CNTGSTR1
        LDAB    COMPNO
        DECB
        ABX
        ABX
        ABX
        JSR     AMOVE
        LDX     #CMP1CNTGSTR2
        LDAB    COMPNO
        DECB
        ABX
        ABX
        ABX
        JSR     CMOVE
        JSR     ADD
        LDX     #MATHCREG
        JSR     IFEREG
        LDX     #CMP1CNTGSTR
        LDAB    COMPNO
        DECB
        ABX
        ABX
        ABX
        JSR     AMOVE
        JSR     SUB

        TST     MATHDREG
        BEQ     CALCTARGETSTAGE3       ; DO STAGE 3 CALC

        LDX     #CMP1CNTGSTR2
        LDAB    COMPNO
        DECB
        ABX
        ABX
        ABX
        CLRW    0,X
        CLR     2,X
;        CLRW     CMP1CNTGSTR2
;        CLR      CMP1CNTGSTR2+2
        JSR     CALCULATEFINALWEIGHT    ; CALCULATE WEIGHT
        LBRA     ATTARST3



CALCTARGETSTAGE3:
        LDX     #CMP1CNTG
        LDAB    COMPNO
        DECB
        ABX
        ABX
        ABX

        LDAB    EREG+2
        STAB    0,X
        LDE     EREG+3
        STE     1,X

        LDAA    #ONE
        STAA    FILLSTAGE
        LDX     #STAGESEQ1
        LDAB    COMPNO
        DECB
        ABX
        STAA    0,X

        JSR     CONVERTTARGET
        CLRA
XITSTG  RTS

ATTARST3 LDAA   #1
         RTS

; STORE STAGE FILL DATA AND DECIDE ON THE CALCULATION TO BE PERFORMED.


STORESTAGEFILLDATA:

        TST     STAGEFILLEN
        LBEQ     XITSTR          ; NORMAL FILLING PROCESS

        LDX     #STAGESEQ1
        LDAB    COMPNO
        DECB
        ABX
        LDAA    0,X

        CMPA    #ONE
        LBNE     NOSTRT           ; NO
        JSR     CALCULATEFINALWEIGHT ; CALCULATE WEIGHT
        LBRA     XITSTR

NOSTRT:
        LDX     #STAGESEQ1
        LDAB    COMPNO
        DECB
        ABX
        LDAA    0,X
        CMPA    #THREE
        BNE     NOTST1          ; NO

        LDAA    #TWO            ; SET TO THE NEXT STAGE
        STAA    FILLSTAGE
        LDX     #STAGESEQ1
        LDAB    COMPNO
        DECB
        ABX
        STAA    0,X


        LDX     #CMP1CNAC
        LDAB    COMPNO
        DECB
        ABX
        ABX
        ABX

        LDAB    #RAMBANK
        TBZK
        LDZ     #CMP1CNTGSTR1
        LDAB    COMPNO
        DECB
        ABZ
        ABZ
        ABZ

        LDD     0,X
        STD     0,Z
        LDAB    2,X
        STAB    2,Z
        JSR     CALCSTAGE2FILL          ; CALC STAGE 2 OF FILL
        TSTA                            ; AT TARGET ?
        BNE     XITSTR

CONSTFL

        LDX     SEQPOINTER              ; IF FILL BY WEIGHT DO NOT STOP FLOW RATE CALC.
        LDAB    0,X
        ANDB    #FILLBYWEIGHT
        BNE     NOFLRCL1

NOFLRCL1:
        BRA     XITSTR
NOTST1  CMPA    #TWO
        BNE     XITSTR          ; NO
        LDX     SEQPOINTER
        LDAB    0,X
        ANDB    #$0F
        DECB
        LDX     #CMP1CNAC
        ABX
        ABX
        ABX


        LDAB    #RAMBANK
        TBZK
        LDZ     #CMP1CNTGSTR2
        LDAB    COMPNO
        DECB
        ABZ
        ABZ
        ABZ

        LDD     0,X
        STD     0,Z
;        STD     CMP1CNTGSTR2
        LDAB    2,X
        STAB    2,Z
;        STAB    CMP1CNTGSTR2+2

        LDAA    #ONE                    ; SET TO GO TO LAST STAGE.
        STAA    FILLSTAGE
        LDX     #STAGESEQ1
        LDAB    COMPNO
        DECB
        ABX
        STAA    0,X

        JSR     CALCSTAGE3FILL
        TSTA                            ; AT TARGET ?
        BNE     XITSTR
        LDAA    #ONE
        STAA    FILLSTAGE
        LDX     #STAGESEQ1
        LDAB    COMPNO
        DECB
        ABX
        STAA    0,X
XITSTR  RTS


CALCULATEFINALWEIGHT:
        LDX     #CMP1CNTGSTR1
        LDAB    COMPNO
        DECB
        ABX
        ABX
        ABX
        JSR     AMOVE
        LDX     #CMP1CNTGSTR2
        LDAB    COMPNO
        DECB
        ABX
        ABX
        ABX
        JSR     CMOVE
        JSR     ADD
        LDX     #AREG
        JSR     IFEREG

        LDAB    COMPNO
        DECB
        LDX     #CMP1CNAC
        ABX
        ABX
        ABX
        PSHM    X
        JSR     CMOVE
        JSR     ADD
        PULM    X
        JSR     EMOVE
        JSR     CMPWCAL          ; CACULATE WEIGHT FOR THIS COMPONENT
        LDX     #CMP1ACTWGT     ; COMPONENT #1 TARGET WEIGHT
        LDAB    COMPNO          ; READ COMPONENT COUNTER
        DECB                    ;
        ABX
        ABX
        ABX
        LDD     EREG
        STD     0,X             ; STORE WEIGHT
        LDAA    EREG+2          ;
        STAA    2,X             ;


; NOW REVERT BACK TO THE EXISTING TARGET
;
        LDX     #CMP1CNTG
        LDAB    COMPNO
        DECB
        ABX
        ABX
        ABX
        LDAB    #RAMBANK
        TBZK
        LDZ     #CMP1CNTGSTR
        LDAB    COMPNO
        DECB
        ABZ
        ABZ
        ABZ
;        LDD     CMP1CNTGSTR     ; READ ORIGINAL TARGET
        LDD     0,Z
        STD     0,X
;        LDAA    CMP1CNTGSTR+2
        LDAA    2,Z
        STAA    2,X             ; STORE

        JSR     CALCTARGETWT
        CLR     FILLSTAGE
        LDX     #STAGESEQ1
        LDAB    COMPNO
        DECB
        ABX
        CLR    0,X
        RTS



CONVERTTARGET:
        LDAB    COMPNO
        DECB
        LDX     #CMP1CNTG
        ABX
        ABX
        ABX
        JSR     BCDHEX3X        ; CONVERT BACK TO HEX
        PSHB
        PSHM    E               ;SAVE RESULT
        LDX     #CMP1CNTGH      ; COUNTS FOR COMPONENTS.
        LDAB    COMPNO
        DECB
        ABX
        ABX
        ABX
        PULM    E
        PULB
        STAB    0,X             ; STORE HEX TARGET
        STE     1,X             ; LSB
        JSR     CALCMNTIM       ; COMP #N TIMES FOR #2...N


CALCTARGETWT:
        LDX     SEQPOINTER
        LDAB    0,X
        ANDB    #$0F
        DECB
        LDX     #CMP1CNTG       ; LOAD COMP #1 TARGET COUNT STORAGE.
        ABX
        ABX
        ABX
        CLRW    EREG
        LDD     0,X
        STD     EREG+2
        LDAA    2,X
        STAA    EREG+4          ; NEXT CALCULATION EXPECTING COUNTS IN EREG.
        JSR     CMPWCAL          ; CACULATE WEIGHT FOR THIS COMPONENT
        LDX     SEQPOINTER
        LDAB    0,X
        ANDB    #$0F
        DECB
        LDX     #CMP1TARWGT     ; COMPONENT #1 TARGET WEIGHT
        ABX
        ABX
        ABX
        LDD     EREG
        STD     0,X             ; STORE WEIGHT
        LDAA    EREG+2          ;
        STAA    2,X             ;
        JSR     CONVTARTOLBS    ; COMP NO HOLDS
        RTS



;*****************************************************************************;
; FUNCTION NAME : CHECKFORMULTONORMALFILL                                     ;
; FUNCTION      : CHECKS FOR MULTIPLE FILL TO NORMAL FILL TRANSITION         ;
; INPUTS        : NONE                                                        ;                       ;
;*****************************************************************************;

CHECKFORMULTONORMALFILL:
        LDAA    LAYERINGTEMP
        CMPA    LAYERING
        BNE     RESSEQ                  ; RESET
        LDAA    STAGEFILLENTEMP         ; NORMAL FILLING
        CMPA    STAGEFILLENH            ; CHECK HISTORY
        BEQ     XITMLC                  ; NO CHANGE
RESSEQ  LDAA    #1
        STAA    RESETSTAGESEQ           ; RESET STAGE SEQ IN WHEN NO CYCLING
XITMLC  RTS












*/








