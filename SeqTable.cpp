//////////////////////////////////////////////////////
// SeqTable.c
//
// Group of functions that related to the setting up the batch cycle sequencing table.
//
// M.McKiernan							22-06-2004
// First Pass
// M.McKiernan							14-09-2004
// Changed the criterion for a regrind component. -
// Ignores g_bRegrindPresent, & now checks g_CurrentRecipe.m_nRegrindComponent.
// Modified ChkForLargest to ignore a regrind component in searching for largest component.
//
// M.McKiernan							30-09-2004
// Edited SwapMasterBatch - Swaps masterbatch (ref component for top up mode) entry
// with last entry in SeqTable
//
// P.Smith                      27/2/06
// first pass at netburner hardware conversion.
//#include <basictypes.h>
//
// P.Smith                      16/11/06
// remove unused commented out code.
//
// P.Smith                      5/2/7
// removed  printfs
//
// P.Smith                      28/2/07
// RECIPECHANGEDBIT is set when the fill by weight options is set.
//
// P.Smith                      14/5/07
// set m_nResetCPIRRCtr when round robin for flow rate needs to be reset.
//
// P.Smith                          30/1/08
// correct compiler warnings
//
// P.Smith                          17/9/08
// check for m_bLiquidEnabled enabled and comp index is g_nLiquidAdditiveComponent
// then omit this component from the sequence table.
// decrement table pointer to revert to the old position.
//
// P.Smith                          16/10/08
// todo to lp
//
// P.Smith                          18/08/09
// a problem has been seen in the top up software in that the top up can get
// screwed up if the top up reference is not entered as one of the used components.
// this is modified to check if the top up reference component is in the table
// and if it is not, then the table is untouched.
// if it is in the table, the top up reference component is put at the end of the table.
//
// P.Smith                          27/11/09
// a problem has been seen in the changing of the order in that if a rate calibration is
// done, the software did not do a fill by weight afterwards.
// the only reason that it worked before was that there a check to determine if the component
// that was being added was not in the list before.
// this part of the program was removed.
// now it is necessary to check if the flow rate is zero in the FormSeqTable
// this ensures that all components do a fill by weight after a rate calibration.
//
// P.Smith                          3/2/10
// added ChkForSmallest
//////////////////////////////////////////////////////

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
#include	"PITWeigh.h"
#include	"BatVars.h"
#include "SequenceTable.h"
#include	"SetpointFormat.h"
#include	"BatVars2.h"

#include <basictypes.h>

// Locally declared global variables
// Externally declared global variables
// Data
extern CalDataStruct	g_CalibrationData;
extern structSetpointData 	g_CurrentRecipe;


//////////////////////////////////////////////////////
// FormSeqTable( )			from ASM = FORMSEQTABLE
//
// Forms the cycle sequence table for a batch blender
// Component no.'s whose target % are non-zero are added to the table.  Table end shown by 0xAA.
// On first pass after reset (g_bFirstTime = TRUE) the fill by weight bit is set for each component, and counts per interrupt r. robin reset.
// Regrind component always added first in table.
// Largest % component placed first if no regrind, or 2nd otherwise.
// M.McKiernan							22-06-2004
// First pass.
// M.McKiernan							06-09-2004
// Changed the expansion for: ResetRoundRobinCPI( );        //RESETRROBINCPI - expanded inline.
// The ".m_bResetCPIRR" flag is set for each of the CPI RR structures, i.e. Main, Stage1 & Stage2.

//////////////////////////////////////////////////////

void FormSeqTable( void )
{
unsigned int 	nCompIndex = 0;
int	nTableIndex = 0;			// table element inserted at nTableIndex.  i.e. points to next empty slot.
int	j;
unsigned char cData;
	// repeat for each component.
	for(nCompIndex = 0; nCompIndex < g_CalibrationData.m_nComponents; nCompIndex++ )
	{
		cData = (char)(nCompIndex + 1);								// i.e. component no.
		// Is target % > 0?
		if(g_CurrentRecipe.m_fPercentage[nCompIndex] != 0)
		{
			// Regrind component?
//			if(g_bRegrindPresent == TRUE && (nCompIndex + 1) == g_CalibrationData.m_nRegrindComponentNumber )		// (nCompIndex + 1) == REGCOMP
			if((nCompIndex + 1) == g_CurrentRecipe.m_nRegrindComponent )		// (nCompIndex + 1) == REGCOMP
			{				// this is the regrind component.
				// YESREG
				cData |= REGRIND_BIT;
				// first cycle after reset??
            if(g_bFirstWCycle)
            {
					//RESETRROBINCPI
//					g_cResetRRobinCmp[nCompIndex] = RESETRRNO;			// Code for resetting round robin, cause reset of RR for this component.
					g_MainCPIRRData.m_ComponentCPIRRData[nCompIndex].m_nResetCPIRRCtr = RESETRRNO;	// cause the CPI round robins to be reset.
					g_Stage2CPIRRData.m_ComponentCPIRRData[nCompIndex].m_nResetCPIRRCtr = RESETRRNO;	//
					g_Stage3CPIRRData.m_ComponentCPIRRData[nCompIndex].m_nResetCPIRRCtr = RESETRRNO;	//

               if(g_ffComponentCPI[nCompIndex] < 0.001f)   //nbb--todo-- check that this is the correct flow rate.
               {
                   cData |= FILL_BY_WEIGHT_BIT;
               }
                   cData |= RECIPECHANGEDBIT;

            }
				// not first element in table??
				if(nTableIndex > 0)
				{
					j = nTableIndex;
					while(j > 0)
					{
						// move all data in table forward by one place.
						g_cSeqTable[j] = g_cSeqTable[j-1];
						j--;
					}
				}
				g_cSeqTable[0] = cData;									// regrind added as first element in table.
			}
			else		// not regrind.
			{
					//NOREG
				// first cycle after reset??
				if(g_bFirstWCycle)
            {
//					ResetRoundRobinCPI( );        //RESETRROBINCPI - expanded inline.
//					g_cResetRRobinCmp[nCompIndex] = RESETRRNO;	// Code for resetting round robin, cause reset of RR for this component.
					g_MainCPIRRData.m_ComponentCPIRRData[nCompIndex].m_nResetCPIRRCtr = RESETRRNO;	// cause the CPI round robins to be reset.
					g_Stage2CPIRRData.m_ComponentCPIRRData[nCompIndex].m_nResetCPIRRCtr = RESETRRNO;	//
					g_Stage3CPIRRData.m_ComponentCPIRRData[nCompIndex].m_nResetCPIRRCtr = RESETRRNO;	//

                   cData |= RECIPECHANGEDBIT;
            }
            if(g_ffComponentCPI[nCompIndex] < 0.001f)
            {
                cData |= FILL_BY_WEIGHT_BIT;
            }
				// store into sequence table

            if (g_CalibrationData.m_bLiquidEnabled && (nCompIndex+1 == g_nLiquidAdditiveComponent))  //nbb--todolp-- review
            {
                nTableIndex--;
            }
            else
            {
                g_cSeqTable[nTableIndex] = cData;
            }
			}

			nTableIndex++;					//advance the table index. after each insertion.

		}	// end of non-zero % found.

	}		// end of for loop, i.e. for each component.
// ATEND
	// Table empty?
   if( nTableIndex == 0 )
   {
         g_cSeqTable[0] = 1;     // set first element of sequence table = 1.
         nTableIndex = 1;
   }
	// Add delimiter
   g_cSeqTable[nTableIndex] = SEQDELIMITER;     // Denotes end of sequence table
	// set pointer(index) to start.
	g_nSeqTableIndex = 0;								//SEQPOINTER - set pointer/index to start of table.

//		if(g_bManualFillSort)
//			;														// not used
	// Rearrange table for largest.
	ChkForLargest( );
	//ChkForSmallest( );  //nbb--testonly--todo--decide if this should be an option

   // check if swapping is required for topup
   SwapMasterBatch( );
//   iprintf("\n form sequence table %x %x %x %x",g_cSeqTable[0],g_cSeqTable[1],g_cSeqTable[2],g_cSeqTable[3]);

}

//////////////////////////////////////////////////////
// ChkForLargest( )			from ASM = CHKFORLARGEST
//
// Rearranges the SeqTable such that the component with largest % is placed first in table.  If there is regrind, the
// regrind remains in 1st place and the component with largest target % is placed second.
//
// M.McKiernan							22-06-2004
// First pass.
//////////////////////////////////////////////////////
void ChkForLargest( void )    // ASM = CHKFORLARGEST //
{
int	nTableIndex = 0;			// nTableIndex  - index in SeqTable.
float fLargestValue = 0.0f;
int	i;
int	nCompIndex;
unsigned char cData;
unsigned char cCompNo;
	i = 0;
	while(g_cSeqTable[i] != 0xAA && i < MAX_COMPONENTS)				// AA is table end or delimiter.
	{
		cCompNo = g_cSeqTable[i] & 0x0F;  // get component no. from table, mask off the m.s. nibble
		nCompIndex = (int)(cCompNo) - 1;	 // component no.s in table start at 1, index is 0 based.
		// find largest non-regrind component.
 	 if((g_cSeqTable[i] & REGRIND_BIT) == 0)				// not a regrind??
	 {
		if(g_CurrentRecipe.m_fPercentage[nCompIndex] > fLargestValue)		// Is this target % greater than previous biggest????
		{
			nTableIndex = i;																// save index into table for largest found.
			fLargestValue = g_CurrentRecipe.m_fPercentage[nCompIndex];				// update largest value found.
		}
	 }
		i++;
	}

 	if(g_cSeqTable[0] & REGRIND_BIT)				// is first element in table a regrind??
	{
		if(g_cSeqTable[1] != 0xAA && nTableIndex > 1)	// check that 2nd table position is not end, i.e. more than one element in table.
		{
			cData = g_cSeqTable[1];
			g_cSeqTable[1] = g_cSeqTable[nTableIndex];		// swap 2nd component with largest comp.
			g_cSeqTable[nTableIndex] = cData;
		}
	}
	else  // Not regrind, swap 1st component with largest
	{
		cData = g_cSeqTable[0];
		g_cSeqTable[0] = g_cSeqTable[nTableIndex];		// move largest comp -> first position in table
		g_cSeqTable[nTableIndex] = cData;
	}
}



//////////////////////////////////////////////////////
// ChkForSmallest( )
//
// Rearranges the SeqTable such that the component with smallest % is placed first in table.  If there is regrind, the
// regrind remains in 1st place and the component with largest target % is placed second.
// test only
//////////////////////////////////////////////////////
void ChkForSmallest( void )    //  //
{
int	nTableIndex = 0;			// nTableIndex  - index in SeqTable.
float fSmallestValue = 0.0f;
int	i;
int	nCompIndex;
unsigned char cData;
unsigned char cCompNo;
	i = 0;
	while(g_cSeqTable[i] != 0xAA && i < MAX_COMPONENTS)				// AA is table end or delimiter.
	{
		cCompNo = g_cSeqTable[i] & 0x0F;  // get component no. from table, mask off the m.s. nibble
		nCompIndex = (int)(cCompNo) - 1;	 // component no.s in table start at 1, index is 0 based.
		if(i==0)
		fSmallestValue = g_CurrentRecipe.m_fPercentage[nCompIndex];
		// find largest non-regrind component.
 	 if((g_cSeqTable[i] & REGRIND_BIT) == 0)				// not a regrind??
	 {
		if(g_CurrentRecipe.m_fPercentage[nCompIndex] < fSmallestValue)		// Is this target % greater than previous biggest????
		{
			nTableIndex = i;																// save index into table for largest found.
			fSmallestValue = g_CurrentRecipe.m_fPercentage[nCompIndex];				// update largest value found.
		}
	 }
		i++;
	}

 	if(g_cSeqTable[0] & REGRIND_BIT)				// is first element in table a regrind??
	{
		if(g_cSeqTable[1] != 0xAA && nTableIndex > 1)	// check that 2nd table position is not end, i.e. more than one element in table.
		{
			cData = g_cSeqTable[1];
			g_cSeqTable[1] = g_cSeqTable[nTableIndex];		// swap 2nd component with largest comp.
			g_cSeqTable[nTableIndex] = cData;
		}
	}
	else  // Not regrind, swap 1st component with largest
	{
		cData = g_cSeqTable[0];
		g_cSeqTable[0] = g_cSeqTable[nTableIndex];		// move largest comp -> first position in table
		g_cSeqTable[nTableIndex] = cData;
	}
}



//////////////////////////////////////////////////////
//SwapMasterBatch       // ASM = SWAPMASTERBATCH //
//
// Swaps masterbatch (ref component for top up mode) entry with last entry in SeqTable.
// Must be at least 3 elements in table.
//
// M.McKiernan							22-06-2004
// First pass.
//////////////////////////////////////////////////////

void SwapMasterBatch( void )    // ASM = SWAPMASTERBATCH //
{
int	nElements = 0;			// elements in SeqTable.
int	nTopUpIndex = 0;
bool bFound = FALSE;
int	i = 0;
unsigned int	nCompIndex;
unsigned char cData;
  if(g_CalibrationData.m_bTopUpMode == ENABLED)
  {
  	i=0;
	while(g_cSeqTable[i] != SEQDELIMITER && i < MAX_COMPONENTS)				// AA is table end or delimiter.m_nTopUpCompNoRef
	{
		nCompIndex = (int)(g_cSeqTable[i] & 0x0F);							// component no. in l.s. nibble
		if(nCompIndex == g_CalibrationData.m_nTopUpCompNoRef)
		{
   	        nTopUpIndex = i;
   	        bFound = TRUE;
        }
        nElements++;									// count elements in table
		i++;
	}
    if((nElements > 2) && (nTopUpIndex != (nElements-1)) && bFound )								// only do if at least 3 elements in table.
	{
//		cData = g_cSeqTable[nTopUpIndex-1];
//		g_cSeqTable[nTopUpIndex-1] = g_cSeqTable[nTopUpIndex];		// move topup entry to previous position in table
//		g_cSeqTable[nTopUpIndex] = cData;										// finish swap.

		cData = g_cSeqTable[nElements-1];									// read last element in table
		g_cSeqTable[nElements-1] = g_cSeqTable[nTopUpIndex];		// move topup entry to last position in table
		g_cSeqTable[nTopUpIndex] = cData;									// finish swap.
	}
  }	// end of if topup mode.
}

/*
FORMSEQTABLE:
        LDAB   #RAMBANK
        TBZK                    ;~;SET EK FOR BANK 1, I.E. RAM
        LDZ     #SEQTABLE       ; SEQUENCER TABLE.
        LDAB    #1              ; COUNTER
        LDX     #PCNT1          ; START OF %
CHECKNEXTP:
        TSTW     0,X            ; IS THE % ZERO ?
        BEQ     SKIPINSERT
        ANDB    #$0F
        PSHB
        TST     REG_PRESENT     ; ANY SIGN OF REGRIND ?
        BEQ     NOREG
        CMPB    REGCOMP
        BNE     NOREG           ; NOT THE REGRIND COMP
// is regrind.
YESREG:
        ORAB    #REGBIT         ; SET REGRIND
        TST     FIRSTWCYCLE     ; IS THIS FIRST CYCLE AFTER RESET ?
        BEQ     NOTFIRST
        JSR     RESETRROBINCPI  ; ACCEPT CPI.
        ORAB    #FILLBYWEIGHT
NOTFIRST CPZ     #SEQTABLE       ; 1ST POSITION ANYWAY.
        BEQ     NOTFIRST1           ; CARRY ON.
        PSHM    Z               ; SAVE CURRENT POSITION.
REPSTZ  AIZ     #-1             ; PREVIOUS POSITION
        LDAA    0,Z             ; DATA READ
        STAA    1,Z             ; UPDATE POSITION.
        CPZ     #SEQTABLE       ; START OF TABLE
        BNE     REPSTZ
        STAB    0,Z             ; STORE REGRIND TO 1ST POSITION.
        PULM    Z               ; RECOVER.
        BRA     NEXTPOS         ; GO TO NEXT POSITION.


NOREG   TST     FIRSTWCYCLE     ; IS THIS FIRST CYCLE AFTER RESET ?
        BEQ     NOTFIRST1
        JSR     RESETRROBINCPI  ; ACCEPT CPI.
        ORAB    #FILLBYWEIGHT

NOTFIRST1:
        STAB    0,Z
NEXTPOS:
        AIZ     #1
        PULB
SKIPINSERT:
        AIX     #2
        CMPB    NOBLNDS
        BEQ     ATEND
        INCB
        BRA     CHECKNEXTP      ; GO FOR NEXT %


ATEND   LDAA    #SEQDELIMITER
        CPZ     #SEQTABLE       ; SEQUENCE TABLE.
        BNE     PROCEED         ; NOT AT BEGINNING OF TABLE.
        LDAB    #1
        STAB    0,Z
        AIZ     #1
PROCEED STAA    0,Z
        LDX     #SEQTABLE
        STX     SEQPOINTER
        TST     MANUALFILLSORT          ; CHECK FOR MANUAL FILL SORTING
        BEQ     SORTAN                  ; SORT AS NORMAL

SORTAN  JSR     CHKFORLARGEST           ; ENSURE THAT LARGEST FILLS FIRST.
        JSR     SWAPMASTERBATCH         ; CHECK IF SWAPPING IS REQUIRED FOR TOPUP
        RTS





CHKFORLARGEST:
        LDAB   #RAMBANK
        TBZK                    ;~;SET EK FOR BANK 1, I.E. RAM
        CLRW     LARGEST
        LDX     #SEQTABLE       ; TABLE ADDRESS
        LDAA    0,X
        ANDA    #REGBIT
        BEQ     NOTRG           ; NO REGRIND.
        AIX     #1
NOTRG
        PSHM    X               ; SAVE STARTING POINT
;        LDAB    #8              ; MAX NO OF PASSES.
        LDAB    #MAXCOMPONENTNO ; MAX NO OF PASSES.
REPLGC  PSHB
        LDAB    0,X
        ANDB    #$0F
        LDZ     #PCNT1
        DECB
        ASLB
        ABZ
        LDD     0,Z             ; READ %
        CPD     LARGEST         ; COMPARE TO LARGEST.
        BLS     NOTLAR          ; SMALLER
        STD     LARGEST
        STX     LARPTR          ; POINTER
NOTLAR  AIX     #1
        PULB
        DECB
        BEQ     XITLGC
        LDAA    0,X
        CMPA    #SEQDELIMITER   ; END ?
        BNE     REPLGC          ; REPEAT LARGEST CHECK.
        PULM    X
        LDZ     LARPTR
        LDAA    0,Z
        LDAB    0,X
        STAA    0,X
        STAB    0,Z             ; DO SWAP.
        RTS
XITLGC  PULM    X               ; RECOVER STACK
        RTS
;*****************************************************************************;
;               ;                               .                             ;
; FUNCTION      : SWAPS MASTER BATCH AND PREVIOUS COMPONENT.                  ;
;               ;                               .                             ;
;*****************************************************************************;

SWAPMASTERBATCH:
        TST     TOPUPF                  ; TOP UP ENABLED ?
        BEQ     XITMB                   ; EXIT IF NO TOP UP
        CLRB                            ; RESET
        LDX     #SEQTABLE
CHKCAG  LDAA    0,X                     ; READ COMP NO
        CMPA    #SEQDELIMITER
        BEQ     CMPCNT                  ; COMPONENTS ARE COUNTED
        AIX     #1
        INCB
        BRA     CHKCAG                  ; CHECK COMPONENT AGAIN

CMPCNT  CMPB    #THREE                  ;
        BLO     XITMB                   ; NO MASTERBATCH SWAP
        LDAB    #RAMBANK
        TBZK
        LDZ     #SEQTABLE               ; FIND THE COLOUR COMPONENT
REPCH   LDAA    0,Z
        CMPA    #SEQDELIMITER
        BEQ     XITMB                   ; NO COMPONENT DETECTED
        ANDA    #$0F
        CMPA    TOPUPCOMPNOREF          ; IS THS THE TOP UP COMP
        BEQ     ISATCMP                 ; IS  AT THE COMP
        AIZ     #1
        BRA     REPCH
ISATCMP AIX     #-1
        LDAA    0,Z
        LDAB    0,X
        STAB    0,Z
        STAA    0,X                     ; SWAP THE TWO COMPONENTS
XITMB   RTS

 	while(g_cSeqTable[i] != 0xAA && i < MAX_COMPONENTS)				// AA is table end or delimiter.m_nTopUpCompNoRef
	{
		nCompIndex = (int)(g_cSeqTable[i] & 0x0F);									// get component no. from table, mask off the m.s. nibble
		nCompIndex--;																		// component no.s in table start at 1, index is 0 based.
		if(g_CurrentRecipe.m_fPercentage[nCompIndex] >= fLargestValue)		// Is this target % greater than previous biggest????
		{
			nTableIndex = i;																// save index into table for largest found.
			fLargestValue = g_CurrentRecipe.m_fPercentage[nCompIndex];				// update largest value found.
		}
		i++;
RESETRROBINCPI:
        PSHM    D,X              ; SAVE REGISTERS.
        LDX     #RESETRROBINCMP1  ; FLAG TO IGNORE CPI ERROR.
        DECB                    ; COMPONENT NO.
        ABX
        LDAA    #RESETRRNO
        STAA    0,X             ; IGNORE THIS ERROR.
        PULM    D,X             ; RECOVER REGISTERS.
        RTS.


*/
