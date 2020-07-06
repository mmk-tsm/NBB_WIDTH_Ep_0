// **********************************************************
//   MBProSum.c                                                 *
//    Modbus production summary programs for Batch blender  *
//
//   Latest Changes:                                                *
//                                                              *
//   M.McKiernan                   22-11-2004                   *
//      First Pass
//
//   M.McKiernan                   08-12-2004                   *
//  Copy g_nCycleCounter (batch cycle counter) into MB. *
//
//   P.Smith                        15-03-2005
//   uncomment CopyDACountsToMB
//   uncomment and debug  CopyDACountsToMB(), CopyHopperWeightToMB();
//   CopyHopperSetWeightsToMB
//
//   P.Smith                        1-06-2005
//
//   Set sensor data (dump, high,low)in modbus table to 1 if uncovered (was originally covered)
//
//
//  P.Smith                         31/8/2005
//  update CopyFBWtsToMB to allow copy of 12 components.
//
//  P.Smith                         1/9/2005
//  added CopyFrontComponentWeightsToMB,CopyBackComponentWeightsToMB,CopyOrderComponentWeightsToMB
//  CopyShiftComponentWeightsToMB,CopyShiftLengthToMB( void )
//
//  P.Smith                              14/09/05
//  Name change for void CopyProdSummDataToMB( void )
//
// P.Smith                              17/10/05
// make g_arrnMBTable unsigned int
//
// P.Smith                      15/2/06
// first pass at netburner hardware conversion.
// #include <basictypes.h>
// #include "NBBgpio.h"
//
// P.Smith                      15/2/06
// changed unions IntsAndLong to WordAndDWord
//#include "NBBGpio.h"
//
// P.Smith                            25/5/06
// multiply modbus data by conversion factor.
//
// P.Smith                            31/5/06
// put back correct no of places in CopyOrderWeightsToMB
//
// P.Smith                            23/6/06
// copy of one second average load cell a/d reading to modbus table
//
// P.Smith                            25/4/07
// call CopyMultiblendSummaryDataToMB on production summary update
//
// P.Smith                            13/6/07
// call CopyOptimisationDataToMB on summary
//
// P.Smith                            18/7/07
// correct copy of component order weight to modbus table
//
// P.Smith                            2/8/07
// copy in fill by weight to modbus table
//
// M.McKiernan                      17/9/07
// Removed g_arrnMBTable[] & g_arrnMBTableFlags[], now in BatVars2
//
// P.Smith                      27/9/07
// Correct Fill by weight status read back to modbus table.
//
// P.Smith                      16/11/07
// Copy screw speed a/d counts, a/d tare and rpm to modbus table on a
// continuous basis.
//
// P.Smith                      30/1/08
// do not use nNotFBW to clear fbw bit as it is not initialised.
//
// P.Smith                      25/2/08
// added CopySdCardDetailsToMB
//
// P.Smith                      26/3/08
// remove extern  int     g_nManagerPassword;
// extern  int     g_nOperatorPassword;
//
// P.Smith                      10/6/08
// add copy of production summary width to modbus table.
//
// P.Smith                      12/6/08
// call CopyAlarmsToIndividualMBWords
//
// P.Smith                      17/6/08
// copy extruder d/a to BATCH_SUMMARY_EXTRUDER_AD position in modbus table
//
// P.Smith                      17/6/08
// CopySDCardStatusToMB called to copy sd card status to modbus table
//
// P.Smith                          23/7/08
// remove g_arrnMBTable,g_CurrentTime externs
//
// P.Smith                          26/9/08
// added CopyLiquidAdditiveToMB, copy temperature to modbus table.
//
// P.Smith                          26/2/09
// correct copy of component weights when comp no > 10
// this was causing the front roll weight to be everwritten.
// PSUM_BLOCK_1 - > PSUM_BLOCK_11
//
// P.Smith                          19/3/09
// added CopyModbusDiagnosticData to copy diagnostics modbus data to modbus
// so that it can be viewed on the operator panel
// LoadPanelVersionNumber called to copy panel revision no.
//
// P.Smith                          20/4/09
// added CopyEncryptionDataToMB to copy license data to mb
//
// P.Smith                          1/9/09
// copy g_bConfigIsLocked to BATCH_LOCK_CONFIG_STATUS
//
// P.Smith                          10/9/09
// copy g_bSelfTestInProgress to modbus table BATCH_BLENDER_SELF_TEST_IN_PROGRESS
//
// P.Smith                          17/9/09
// added CopyVAC8IODiagnosticsToMB for vacuuming stuff
//
// P.Smith                          19/11/09
// call CopySelfResultsToMB
//
// P.Smith                          5/3/10
// in CopyEncryptionDataToMB copy new options word to mb
//
// P.Smith                          9/3/10
// g_bWatchdogStatus, eip counter and assembly counter to modbus
//
// P.Smith                          15/4/10
// added check for 5270 module type if B send back "B" otherwise "-"
//
// P.Smith                          19/4/10
// copy unrestriced access status to modbus table.
//
// P.Smith                          23/4/10
// CopyHistoricDataToModbus called to copy historic data to modbus
//
// P.Smith                          7/7/10
// removed copy of set width to summary width, this will now hold the actual
// width
//// **********************************************************

//*****************************************************************************
// INCLUDES
//*****************************************************************************
#include <stdio.h>

#include "SerialStuff.h"
#include "BatchMBIndices.h"
#include "BatchCalibrationFunctions.h"
#include "General.h"
#include "ConversionFactors.h"

#include "16R40C.h"
#include "ConfigurationFunctions.h"
#include "MBMHand.h"
#include "MBSHand.h"
#include "MBProgs.h"
#include "MBProgs.h"
#include "SetpointFormat.h"
#include "TimeDate.h"
#include "BatVars.h"
#include "BatVars2.h"
#include "MBProSum.h"
#include "Blrep.h"
#include <basictypes.h>
#include "NBBgpio.h"
#include "MultiblendMB.h"
#include "OptimisationMB.h"
#include "rta_system.h"
#include "Sdcard.h"
#include "Liquidadditive.h"
#include "VacVars.h"
#include "BlenderSelfTest.h"
#include "License.h"
#include "Historylog.h"



extern  CalDataStruct   g_CalibrationData;
extern  structSetpointData  g_CurrentRecipe;
extern  structSetpointData  g_TempRecipe;

extern  int g_nProtocol;

extern  char    *RevisionNumber;
extern  structRollData      g_FrontRoll;
extern  structRollData      g_BackRoll;
extern  structRollData      g_OngoingShift;
extern  OnBoardCalDataStruct   g_OnBoardCalData;
extern  structControllerData    structVAC8IOData[];
extern bool g_bWatchdogStatus;		//ENABLED or DISABLED
extern bool g_bEIPSoftwareEnabled;
extern unsigned int	g_nEIPCtr;
extern unsigned int	g_nEIPAssemblyObjectCtr;
extern DWORD g_nDRAMmask;
extern unsigned int g_nAllowUnrestricedAccess;

//startnew
//////////////////////////////////////////////////////
// CopyProdSummDataToMB  asm = COPYMBPSUM
//
// Copy poduction summary data to MB. - called on 10 hz.
//
// M.McKiernan                          03-11-2004
// First Pass
//
//////////////////////////////////////////////////////
void CopyProdSummDataToMB( void )
{
    float fValue;
    CopyADCountsToMB();             // CPYADCMB copy a/d counts to MB.
    CopyDACountsToMB();             // CPYDACMB                ; DAC DATA.
    CopyHopperWeightToMB();            // COPYHOPPERWEIGHTMB
    CopyHopperSetWeightsToMB();     // CPYHOPPERSETWTMB
    CopyBBAuxToMB();                    // COPYBBAUXMB             ; RESET OF PSUM DATA.
    CopyDiagnosticsToMB();          // COPYMBDIAGNOSTICS
    CopyMultiblendSummaryDataToMB();
    CopyOptimisationDataToMB();
    CopySdCardDetailsToMB();

// merge inline asm CPYPSAMB
    if(g_nProtocol == PROTOCOL_MODBUS)
    {
        g_arrnMBTable[BATCH_SUMMARY_FRONT_ROLL_COUNT] = g_FrontRoll.m_nRollNo;
        g_arrnMBTable[BATCH_SUMMARY_BACK_ROLL_COUNT] = g_BackRoll.m_nRollNo;
        CopyFrontComponentWeightsToMB();  // ASM = CPYFWTSMB
        CopyBackComponentWeightsToMB();  // ASM = CPYBWTSMB
        CopyOrderComponentWeightsToMB();  // ASM = CPYOWTSMB
        CopyShiftComponentWeightsToMB();  // ASM = CPYSHWTSMB
        CopyShiftLengthToMB();           // ASM = CPYSHFTLMB
    }




// CPYGM2MB
    // Store the actual wt/area or g/m2 to MB
    g_arrnMBTable[BATCH_SUMMARY_TOTAL_WEIGHT_AREA] = (int)(100 * g_fActualGrammesPerM2);    // g/m2 to 2dp.
                                               // CYCLECOUNTER

   // merge CPYERPMMB inline

        if(g_nProtocol == PROTOCOL_MODBUS)
        {
            g_arrnMBTable[BATCH_SUMMARY_EXTRUDER_SPEED] = (int)(g_fExtruderRPM * 100);        //  asm MBERPM.
            g_arrnMBTable[BATCH_SUMMARY_EXTRUDER_AD] = g_nExtruderDAValue;        // asm MBEA_D
            g_arrnMBTable[BATCH_SUMMARY_SCREW_SPEED_ATD] = g_nExtruderTachoReadingAverage;        // asm MBEA_D
            // Screw speed tare counts  -  asm = SSTARE
            g_arrnMBTable[BATCH_CALIBRATION_SCREW_TARE_COUNTS] = g_CalibrationData.m_nScrewSpeedTareCounts;

        }

// copy width to summary
        CopyAlarmsToIndividualMBWords();
        CopySDCardStatusToMB();
        CopyLiquidAdditiveToMB();
        g_arrnMBTable[BATCH_TEMPERATURE] = g_nTemperature;                            // Width is in mm.
        LoadPanelVersionNumber();
        CopyModbusDiagnosticData();
        g_arrnMBTable[BATCH_LOCK_CONFIG_STATUS] = g_bConfigIsLocked;
        g_arrnMBTable[BATCH_BLENDER_SELF_TEST_IN_PROGRESS] = g_bSelfTestInProgress;
        CopyVAC8IODiagnosticsToMB();  // copy diagnostics for each VAC8io card to MB.
        CopySelfResultsToMB();// copy results of test to modbus
        CopyHistoricDataToModbus();
        g_arrnMBTable[BATCH_SUMMARY_WATCH_DOG_TIMER_STATUS] = g_bWatchdogStatus;
        g_arrnMBTable[BATCH_SUMMARY_EIP_ENABLED] = g_bEIPSoftwareEnabled;
        g_arrnMBTable[BATCH_SUMMARY_EIP_CTR] = g_nEIPCtr;
        g_arrnMBTable[BATCH_SUMMARY_EIP_ASSEMBLY_OBJECT_CTR] = g_nEIPAssemblyObjectCtr;
        if(g_nDRAMmask == 0x7c0001)
        {
            g_arrnMBTable[BATCH_SUMMARY_NETBURNER_MODULE_TYPE] = 'B';
        }
        else
        {
            g_arrnMBTable[BATCH_SUMMARY_NETBURNER_MODULE_TYPE] = '-';
        }
        if(g_nAllowUnrestricedAccess == UNRESTRICED_ACCESS)
        {
            g_arrnMBTable[BATCH_SUMMARY_UNLIMITED_LICENSE_ACCESS] = 0x01;
        }
        else
        {
            g_arrnMBTable[BATCH_SUMMARY_UNLIMITED_LICENSE_ACCESS] = 0x0;
        }

}
/*
COPYMBPSUM:
        JSR     CPYDACMB                ; DAC DATA.
        JSR     COPYHOPPERWEIGHTMB      ; WEIGHT INFO.
        JSR     COPYBBAUXMB             ; RESET OF PSUM DATA.
        JSR     COPYMBDIAGNOSTICS
        JSR     CPYHOPPERSETWTMB
        JSR     CPYGM2MB
        RTS

        LDX     #GPM2VAR        ; Wt/m2 VALUE
        JSR     BCDHEX3X        ; CONVERT 3 BYTES BCD @ X TO HEX
        STE     MBGPM2          ; MAX VALUE EXPECTED = 655.35 g/m2

*/

/*

COPYFBWTOMB:
        LDAB   #RAMBANK
        TBZK                    ;

        LDAB   #BANK4
        TBYK                    ;~;SET EK FOR BANK 1, I.E. RAM

        LDX     #SEQTABLE
REPFBWC LDAB    0,X
        TBA
        CMPB    #SEQDELIMITER
        BEQ     XIT

        LDY     #FBWLOOKUP
        ANDB    #$0F
        DECB
        ABY
        ABY
        LDZ     0,Y             ; READ MODBUS STATUS ADDRESS
        TAB
        ANDB    #FILLBYWEIGHT
        BEQ     NOFBW
        LDD     #MBFILLBYWEIGHT
        ORD     0,Z
        STD     0,Z
        BRA     CONFBWC                 ; CONTINUE

NOFBW   LDD     #MBFILLBYWEIGHT
        COMD
        ANDD    0,Z
        STD     0,Z
CONFBWC AIX     #1                      ; NEXT POSITION
        BRA     REPFBWC
XIT     RTS


// Operat. status mask
#define BATCH_STATUS_CALIBRATION_MASK           0x0001      // IN CALIBRATION MODE.
#define BATCH_STATUS_BLENDER_STOPPED            0x0002      //  BLENDER IS STOPPED
#define BATCH_STATUS_MIXER_ON                   0x0004      //  MIXER IS ON.
#define BATCH_STATUS_DUMP_FLAP_OPEN         0x0008      //  DUMP FLAP IS OPEN.
#define BATCH_STATUS_RESET_OCCURRED_MASK        0x0020      //  RESET BIT
#define BATCH_STATUS_FLOW_CONTROL_VALVE     0x0040      //B6 FIBRE - NEW  (OFFLINE VALVE)

#define BATCH_STATUS_AUTOMANUAL_MASK            0x0080      //  AUTO MANUAL  ON
#define BATCH_STATUS_PAUSE                      0x0100      //   PAUSE STATUS ON
#define BATCH_STATUS_LOW_LEVEL_SENSOR           0x0200      //  LOW LEVEL SENSOR. OFF
#define  BATCH_STATUS_HIGH_LEVEL_SENSOR      0x0400     //  HIGH LEVEL SENSOR OFF
#define  BATCH_STATUS_MAIN_LEVEL_SENSOR      0x0800         //  DUMP LEVEL SENSOR
#define  BATCH_STATUS_IN_CLEAN_MODE          0x1000      // CLEAN STARTED               12
#define  BATCH_STATUS_FINISHED_CLEAN_MODE    0x2000      // CLEAN ENDED                 13
#define  BATCH_STATUS_BYPASS_VALVE           0x4000      // BYPASS O/P STATUS           14
#define  BATCH_STATUS_IN_CLEAN_STAGE_2      0x8000      // CLEAN STATUS STAGE 2        //b15

//MBOPSTS2
#define  BATCH_STATUS_2_TARE_MASK               0x0001   //
#define  BATCH_STATUS_2_CLEAN_LAST_BATCH_MASK   0x0002   //
#define  BATCH_STATUS_2_BLENDER_CLEAN_MASK      0x0004   //
#define  BATCH_STATUS_2_MIXER_CLEAN_MASK        0x0008   //
#define  BATCH_STATUS_2_RIGHT_CORNER_CLEAN_MASK 0x0010   //
#define  BATCH_STATUS_2_LEFT_CORNER_CLEAN_MASK  0x0020   //
#define BATCH_STATUS_2_CYCLE_INDICATE_MASK      0x0040  // MBSTATCYCLEINDICATE      EQU     %0000000001000000       ;  CYCLE STATUS IN PROGRESS
#define  BATCH_STATUS_2_CLEAN_PHASE_1_MASK      0x0080   // MBBLENDERCLEANBITPOS     EQU     %0000000010000000       ;  BLENDER CLEAN IN PROGRESS
#define  BATCH_STATUS_2_CLEAN_PHASE_2_MASK      0x0100   // MBMIXERCLEANBITPOS       EQU     %0000000100000000       ;  MIXER CLEAN in progress

; COPY BATCH BLENDER AUXILIARY DATA TO MODBUS TABLE.

; COPY BATCH BLENDER AUXILIARY DATA TO MODBUS TABLE.

; COPY BATCH BLENDER AUXILIARY DATA TO MODBUS TABLE.

COPYBBAUXMB:
        TST     PROTOCOL
        LBPL     CPYAUXX
        LDD     #MBSTATSTOPPEDBPOS      ; STOPPED BIT POSITION.
        TST     STOPPED
        BEQ     ISRUN                   ; IS RUNNING.
        ORD     MBOPSTS
        STD     MBOPSTS
        BRA     CONCPA                  ; CONTINUE COPY OF AUX DATA.
ISRUN   COMD
        ANDD    MBOPSTS
        STD     MBOPSTS                 ; STORE OPERATING STATUS.
CONCPA  LDD     #MBSTATPAUSEBPOS        ; PAUSE BIT POSITION.
        TST     PAUSFLG                 ;
        BEQ     NOPAUS
        ORD     MBOPSTS
        STD     MBOPSTS
        BRA     CONCP1
NOPAUS  COMD
        ANDD    MBOPSTS
        STD     MBOPSTS         ; STORE OPERATING STATUS.
CONCP1  LDD     #MBSTATMIXONBPOS
        TST     MIXON           ;
        BEQ     MISOFF          ; MIXER IS ON.
        ORD     MBOPSTS
        STD     MBOPSTS
        BRA     CONCP2          ; CONTINUE
MISOFF  COMD
        ANDD    MBOPSTS
        STD     MBOPSTS         ; STORE OPERATING STATUS.
; START UP MODE
CONCP2  LDD     #MBBLENDERSTARTUPMODE
        TST     STRUPF           ;
        BEQ     NOSTU            ; NO START UP.
        ORD     MBOPSTS
        STD     MBOPSTS
        BRA     CONST            ; CONTINUE
NOSTU   COMD
        ANDD    MBOPSTS
        STD     MBOPSTS         ; STORE OPERATING STATUS.
CONST:

        LDAB    #RAMBANK
        TBYK
        LDD     #MBSTATTAREBPOS   ; .
        LDX     #TARINGSTATUS
        LDY     #MBOPSTS2
        JSR     SETCLRBITDEPONX

        LDD     #MBSTATBLENDERCLEANBPOS   ; .
        LDX     #AIRJET1ONF
        LDY     #MBOPSTS2
        JSR     SETCLRBITDEPONX

        LDD     #MBSTATMIXERCLEANBPOS   ; .
        LDX     #AIRJET2ONF
        LDY     #MBOPSTS2
        JSR     SETCLRBITDEPONX

        LDD     #MBSTATRCCBPOS   ; .
        LDX     #CORNERCLEANONF
        LDY     #MBOPSTS2
        JSR     SETCLRBITDEPONX

        LDD     #MBSTATLCCBPOS   ; .
        LDX     #CORNERCLEANRONF
        LDY     #MBOPSTS2
        JSR     SETCLRBITDEPONX

        LDD     #MBSTATCLEANLASTBATCHBPOS   ; .
        LDX     #SETCLEANWHENCYCLEEND
        LDY     #MBOPSTS2
        JSR     SETCLRBITDEPONX


        LDD     #MBSTATCYCLEINDICATE   ; .
        LDX     #CYCLEINDICATE
        LDY     #MBOPSTS2
        JSR     SETCLRBITDEPONX

        LDAB   CLEANCYCLESEQ
        CMPB   #CLEANPHASE1ID
        BNE    NOTPH1
        LDY     #MBOPSTS2
        LDD    #MBBLENDERCLEANBITPOS
        ORD     0,Y
        STD     0,Y
        BRA     CHPH2

NOTPH1  LDD    #MBBLENDERCLEANBITPOS
        COMD
        ANDD    0,Y
        STD     0,Y

CHPH2   LDAB   CLEANCYCLESEQ
        CMPB   #CLEANPHASE2ID
        BNE    NOTPH2

        LDD    #MBMIXERCLEANBITPOS
        ORD     0,Y
        STD     0,Y
        BRA     CONCP

NOTPH2  LDD    #MBMIXERCLEANBITPOS
        COMD
        ANDD    0,Y
        STD     0,Y


CONCP   CLRA
        LDAB    LFILLSTATCMP1    ;
        STD     MBFILLSTATCMP1   ; FILL STATUS.
        LDAB    LFILLSTATCMP2    ;
        STD     MBFILLSTATCMP2   ; .
        LDAB    LFILLSTATCMP3    ;
        STD     MBFILLSTATCMP3   ; .
        LDAB    LFILLSTATCMP4    ;
        STD     MBFILLSTATCMP4   ; FILL STATUS.
        LDAB    LFILLSTATCMP5    ;
        STD     MBFILLSTATCMP5   ; FILL STATUS.
        LDAB    LFILLSTATCMP6    ;
        STD     MBFILLSTATCMP6   ; FILL STATUS.
        LDAB    LFILLSTATCMP7    ;
        STD     MBFILLSTATCMP7   ; FILL STATUS.
        LDAB    LFILLSTATCMP8    ;
        STD     MBFILLSTATCMP8   ; FILL STATUS.
        LDAB    LFILLSTATCMP9    ;
        STD     MBFILLSTATCMP9   ; FILL STATUS.
        LDAB    LFILLSTATCMP10   ;
        STD     MBFILLSTATCMP10  ; FILL STATUS.
        LDAB    LFILLSTATCMP11   ;
        STD     MBFILLSTATCMP11  ; FILL STATUS.
        LDAB    LFILLSTATCMP12   ;
        STD     MBFILLSTATCMP12  ; FILL STATUS.

        LDD     #MBSTATDUMPSTATUSBPOS
        TST     DUMPSTATUS      ;
        BEQ     NODUMP          ; NOT DUMPING.
        ORD     MBOPSTS
        STD     MBOPSTS
        BRA     CONCP3          ; CONTINUE
NODUMP  COMD
        ANDD    MBOPSTS
        STD     MBOPSTS                 ; STORE OPERATING STATUS.
CONCP3  LDD     #MBSTATHIGHLSPOS        ; HIGH LEVEL SENSOR
        TST     HIGHLEVELSENSOR    ;
        BEQ     NOHIGH          ; OFF
        ORD     MBOPSTS
        STD     MBOPSTS
        BRA     CONCP4          ; CONTINUE
NOHIGH  COMD
        ANDD    MBOPSTS
        STD     MBOPSTS        ; STORE OPERATING STATUS.
CONCP4  LDD     #MBSTATLOWLSPOS        ; HIGH LEVEL SENSOR
        TST     LOWLEVELSENSOR    ;
        BEQ     NOLOW           ; OFF
        ORD     MBOPSTS
        STD     MBOPSTS
        BRA     CONCP5          ; CONTINUE
NOLOW   COMD
        ANDD    MBOPSTS
        STD     MBOPSTS        ; STORE OPERATING STATUS.
CONCP5  LDD     #MBSTATDUMPLSSTATUSBPOS ; DUMP SENSOR
        TST     LEVSEN1STAT    ;
        BEQ     NODMPLS           ; OFF
        ORD     MBOPSTS
        STD     MBOPSTS
        BRA     CONCP6         ; CONTINUE
NODMPLS COMD
        ANDD    MBOPSTS
        STD     MBOPSTS        ; STORE OPERATING STATUS.
CONCP6
        LDD     #MBCSTATBYPASSSTPOS   ; .
        LDX     #BYPASSSTATUS
        LDY     #MBOPSTS
        JSR     SETCLRBITDEPONX

        CLRA
        LDAB    LS1OFF         ; LS1 OFF ?
        STD     MBLS1OFF       ;
        LDAB    LS2OFF         ;
        STD     MBLS2OFF       ;
        LDAB    LS3OFF         ;
        STD     MBLS3OFF       ;
        LDAB    LS4OFF         ;
        STD     MBLS4OFF       ;
        LDAB    LS5OFF         ;
        STD     MBLS5OFF       ;
        LDAB    LS6OFF         ;
        STD     MBLS6OFF       ;
        LDAB    LS7OFF         ;
        STD     MBLS7OFF       ;
        LDAB    LS8OFF         ;
        STD     MBLS8OFF       ;
        LDAB    LS9OFF         ;
        STD     MBLS9OFF       ;
        LDAB    LS10OFF         ;
        STD     MBLS10OFF       ;
        LDAB    LS11OFF         ;
        STD     MBLS11OFF       ;
        LDAB    LS12OFF         ;
        STD     MBLS12OFF       ;

;       OFFLINE O/P STATUS

CONCL22 LDD     #MBCSTATOFFLINESTPOS
        TST     FEEDCLOSE
        BEQ     FEEDOP
        ORD     MBOPSTS
        STD     MBOPSTS
        BRA     CONCL3         ; CONTINUE
FEEDOP  COMD                   ; RESET BIT
        ANDD    MBOPSTS
        STD     MBOPSTS  ;
CONCL3:
        JSR     COPYFBWTOMB     ; COPY FILL BY WEIGHT STATUS TO MODBUS TABLE.

        CLRA
        LDAB    ADDR_HX         ; SERIAL ADDRESS
        STD     MBADDRESS

        LDX     DACPKGHR        ; D/A PER KG  XX XX XX.YY YY
        STX     EREG
        LDX     DACPKGHR+2
        STX     EREG+2
        LDAA    DACPKGHR+4
        STAA    EREG+4

        LDX     #EREG     ;
        LDAB    #5
        LDAA    #1
        JSR     GPSFTR            ;~RESULT
        LDX     #EREG+1
        JSR     BCDHEX3X
        STE     MBDACPKGHR


        LDX     DACPMPM         ; D/A PER METER PER MINUTE
        STX     EREG
        LDX     DACPMPM+2
        STX     EREG+2
        LDAA    DACPMPM+4
        STAA    EREG+4

        LDX     #EREG     ;
        LDAB    #5
        LDAA    #1
        JSR     GPSFTR            ;~RESULT
        LDX     #EREG+1
        JSR     BCDHEX3X
        STE     MBDACPMPM

        LDD     SEIINPUTSTATE
        STD     MBSEIINPUTSTATE

        LDD     SEIALARMSTATE
        STD     MBSEIALARMSTATE

        LDAB    #RAMBANK
        TBYK
        LDD     #MBLINKL2BPOS   ; .
        LDX     #MCU16_L2
        LDY     #MBDIAGOPTIONS
        JSR     SETCLRBITDEPONX

        LDD     #MBLINKL3BPOS   ; .
        LDX     #MCU16_L3
        LDY     #MBDIAGOPTIONS
        JSR     SETCLRBITDEPONX

        LDD     #MBLINKL4BPOS   ; .
        LDX     #MCU16_L4
        LDY     #MBDIAGOPTIONS
        JSR     SETCLRBITDEPONX

        LDD     #MBLINKL5BPOS   ; .
        LDX     #MCU16_L5
        LDY     #MBDIAGOPTIONS
        JSR     SETCLRBITDEPONX









CPYAUXX RTS


*/
/*
COPYMBPSUM:
        JSR     CPYDACMB                ; DAC DATA.
        JSR     COPYHOPPERWEIGHTMB      ; WEIGHT INFO.
        JSR     COPYBBAUXMB             ; RESET OF PSUM DATA.
        JSR     COPYMBDIAGNOSTICS
        JSR     CPYHOPPERSETWTMB
        RTS


*/
//////////////////////////////////////////////////////
// CopyADCountsToMB  asm = CPYADCMB
//
// copy A/D counts to MB. - call once per second.
//
// M.McKiernan                          03-11-2004
// First Pass
//
//////////////////////////////////////////////////////
void CopyADCountsToMB( void )
{
    union WordAndDWord uValue;
    // A/D counts; long in MB. // MB1ADC
    uValue.lValue = g_lOneSecondWeightAverage;   //sbb--todo--
    g_arrnMBTable[BATCH_SUMMARY_WEIGHT_ADC_1] = uValue.nValue[0];       // M.s.Byte.
    g_arrnMBTable[BATCH_SUMMARY_WEIGHT_ADC_1+1] = uValue.nValue[1];     // L.s.Byte.
}


//endnew
//////////////////////////////////////////////////////
// CheckForCompStatusOff  asm = CHECKFORCOMPSTATUSOFF
//
// Decrements the "long fill" status counter for each component, and
// clears "on" status when = 0. - call on 10Hz
//
// M.McKiernan                          23-11-2004
// First Pass
//
//////////////////////////////////////////////////////
void CheckForCompStatusOff( void )
{
    unsigned int i;
    for( i = 0; i < g_CalibrationData.m_nComponents; i++ )
    {
        if( g_nLFillStatCtr[i] )
        {
            g_nLFillStatCtr[i]--;   // decrement "long fill" counter to zero
            if( g_nLFillStatCtr[i] == 0 )   // have we reached 0??
                g_bLFillStatCmp[i] = OFF;   // indicate valve or slide is off.
        }
//      else
//              g_bLFillStatCmp[i] = OFF;   // indicate valve or slide is off.

    }

}
/*
; CHECK FOR COMPONENT STATUS ON / OFF.

CHECKFORCOMPSTATUSOFF:
        LDAB    #RAMBANK
        TBZK
        LDZ     #LFILLSTATCTR1          ; ON TIMER FOR COMPONENT 1 - N.
        LDX     #LFILLSTATCMP1          ; OPEN / CLOSE STATUS FOR COMPONENT #1
        LDAB    NOBLNDS                 ; NO OF BLENDS.
GOTONC  TST     0,Z                     ; TIMER NON ZERO.
        BEQ     CHKNCT                  ; CHECK NEXT COMP TIMER.
        DEC     0,Z                     ; DECREMENT COUNTER.
        BNE     CHKNCT                  ; NOT ZERO
        CLR     0,X                     ; RESET OPEN FLAG.
CHKNCT  AIZ     #1                      ; INCREMENT TIMER POINTER.
        AIX     #1                      ; INCREMENT STATUS POINTER.
        DECB                            ; ALL BLENDS EXAMINED ?
        BNE     GOTONC                  ; GO TO NEXT COMPONENT.                      ;
        RTS

*/
//////////////////////////////////////////////////////
// CopyFBWtsToMB  asm = COPYFBWTOMB
//
// Copy fill by weight" bits into MB
//
//
// M.McKiernan                          23-11-2004
// First Pass
//
//////////////////////////////////////////////////////
void CopyFBWtsToMB( void )
{
unsigned int i;
int nMBIndex, nCompIndex;
int nNotFBW;
    // First clear FBW bits for all components whose target % is 0.
    for(i = 0; i < g_CalibrationData.m_nComponents; i++)
    {
        if(i < 10)
        {
        nMBIndex = BATCH_SUMMARY_BLEND_STATUS_1 + (i * PSUM_BLOCK_SIZE );   // status is in component block.
        if(g_CurrentRecipe.m_fPercentage[i] == 0.0 )
        g_arrnMBTable[nMBIndex] &= ~MBFILLBYWEIGHT_MASK;     // clear fill by wt bit.
        }
        else     //--review-- modbus expansion
        {
        nMBIndex = BATCH_SUMMARY_BLEND_STATUS_1 + PSUM_BLOCK_11 + ((i-10) * PSUM_BLOCK_SIZE );   // status is in component block.
        if(g_CurrentRecipe.m_fPercentage[i] == 0.0 )
        g_arrnMBTable[nMBIndex] &= ~MBFILLBYWEIGHT_MASK;     // clear fill by wt bit.

        }

    }
    nNotFBW = MBFILLBYWEIGHT_MASK ^ 0xFFFF; // complement of MB fill by wt. bit.
    i = 0;      // i in this case is index into sequence table, nCompIndex relates to component no.
    while(g_cSeqTable[i] != SEQDELIMITER && i < MAX_COMPONENTS) // SEQDELIMITER indicates table end
    {
        nCompIndex = (int)(g_cSeqTable[i] & 0x0F) - 1;  // get component no. from table, mask off the m.s. nibble, 0 based.

        if(nCompIndex < 10)
        nMBIndex = BATCH_SUMMARY_BLEND_STATUS_1 + (nCompIndex * PSUM_BLOCK_SIZE );  // status is in component block.
        else // work out address of isolated block.   --review-- modbus expansion
        nMBIndex = PSUM_STATUS_OFFSET + PSUM_BLOCK_11 + ((nCompIndex - 10) * PSUM_BLOCK_SIZE );  // status is in component block.

         if(g_cSeqTable[i] & FILLBYWEIGHT )
        {
            g_arrnMBTable[nMBIndex] |= MBFILLBYWEIGHT_MASK;
            g_arrnMBTable[BATCH_FILL_BY_WEIGHT_1 + nCompIndex] = 1;
        }
        else
        {
            g_arrnMBTable[nMBIndex] &= nNotFBW;     // clear fill by wt bit.
            g_arrnMBTable[BATCH_FILL_BY_WEIGHT_1+nCompIndex] = 0;
        }
        i++;
   }
}



//////////////////////////////////////////////////////
// CopyFrontComponentWeightsToMB  asm = CPYFWTSMB
//
// Copy front roll component weights to modbus table
// Allow for up to 12 components
//
// P.Smith                          1-9-2005
//
//////////////////////////////////////////////////////
void CopyFrontComponentWeightsToMB( void )
{
unsigned int i;
float   fValue;
union WordAndDWord uValue;
    if(g_nProtocol == PROTOCOL_MODBUS)
    {
        if(g_CalibrationData.m_nComponents <= 10)
        {
            for(i = 0; i < g_CalibrationData.m_nComponents; i++ )
            {
                fValue =    g_fComponentFrontWeightAccumulator[i];
                fValue *=    g_fWeightConversionFactor;
                // rounding to nearest 4TH decimal digit.
                fValue += 0.00005;
                // Wt to 2dp in MB.
                uValue.lValue = (long)(fValue * 100.0f);
                // M.s.Byte.
                g_arrnMBTable[PSUM_BLOCK_1 + (i*PSUM_BLOCK_SIZE) + PSUM_FROLL_WT_OFFSET] = uValue.nValue[0];
                // l.s.Byte.
                g_arrnMBTable[PSUM_BLOCK_1 + (i*PSUM_BLOCK_SIZE) + PSUM_FROLL_WT_OFFSET + 1] = uValue.nValue[1];
            }
        }
        else  //--review-- modbus expansion
        {
            for(i = 0; i < 10; i++ )  // copy 10 component & then rest of components
            {
                fValue =    g_fComponentFrontWeightAccumulator[i];
                fValue *=    g_fWeightConversionFactor;
                // rounding to nearest 4TH decimal digit.
                fValue += 0.00005;
                // Wt to 2dp in MB.
                uValue.lValue = (long)(fValue * 100.0f);
                // M.s.Byte.
                g_arrnMBTable[PSUM_BLOCK_1 + (i*PSUM_BLOCK_SIZE) + PSUM_FROLL_WT_OFFSET] = uValue.nValue[0];
                // l.s.Byte.
                g_arrnMBTable[PSUM_BLOCK_1 + (i*PSUM_BLOCK_SIZE) + PSUM_FROLL_WT_OFFSET + 1] = uValue.nValue[1];
            }
            for(i = 10; i < g_CalibrationData.m_nComponents; i++ )
            {

                fValue =    g_fComponentFrontWeightAccumulator[i];
                fValue *=    g_fWeightConversionFactor;
                 // rounding to nearest 4TH decimal digit.
                fValue += 0.00005;
                // Wt to 2dp in MB.
                uValue.lValue = (long)(fValue * 100.0f);
                // M.s.Byte.
                g_arrnMBTable[PSUM_BLOCK_11 + (i*PSUM_BLOCK_SIZE) + PSUM_FROLL_WT_OFFSET] = uValue.nValue[0];
                // l.s.Byte.
                g_arrnMBTable[PSUM_BLOCK_11 + ((i-10)*PSUM_BLOCK_SIZE) + PSUM_FROLL_WT_OFFSET + 1] = uValue.nValue[1];
            }
        }
    }
}


//////////////////////////////////////////////////////
// CopyBackComponentWeightsToMB  asm = CPYBWTSMB
//
// Copy back roll component weights to modbus table
// Allow for up to 12 components
//
// P.Smith                          1-9-2005
//
//////////////////////////////////////////////////////
void CopyBackComponentWeightsToMB( void )
{
unsigned int i;
float   fValue;
union WordAndDWord uValue;
    if(g_nProtocol == PROTOCOL_MODBUS)
    {
        if(g_CalibrationData.m_nComponents <= 10)
        {
            for(i = 0; i < g_CalibrationData.m_nComponents; i++ )
            {
                // actual component weights
                fValue =    g_fComponentBackWeightAccumulator[i];
                fValue *=    g_fWeightConversionFactor;
                          // rounding to nearest 4TH decimal digit.
                fValue += 0.00005;
                // Wt to 2dp in MB.
                uValue.lValue = (long)(fValue * 100.0f);
                // M.s.Byte.
                g_arrnMBTable[PSUM_BLOCK_1 + (i*PSUM_BLOCK_SIZE) + PSUM_BROLL_WT_OFFSET] = uValue.nValue[0];
                // l.s.Byte.
                g_arrnMBTable[PSUM_BLOCK_1 + (i*PSUM_BLOCK_SIZE) + PSUM_BROLL_WT_OFFSET + 1] = uValue.nValue[1];

            }
        }
        else  //--review-- modbus expansion
        {
            for(i = 0; i < 10; i++ )  // copy 10 component & then rest of components
            {
                fValue =    g_fComponentBackWeightAccumulator[i];
                fValue *=    g_fWeightConversionFactor;
                          // rounding to nearest 4TH decimal digit.
                fValue += 0.00005;
                // Wt to 4dp in MB.
                uValue.lValue = (long)(fValue * 100.0f);
                // M.s.Byte.
                g_arrnMBTable[PSUM_BLOCK_1 + (i*PSUM_BLOCK_SIZE) + PSUM_BROLL_WT_OFFSET] = uValue.nValue[0];
                // l.s.Byte.
                g_arrnMBTable[PSUM_BLOCK_1 + (i*PSUM_BLOCK_SIZE) + PSUM_BROLL_WT_OFFSET + 1] = uValue.nValue[1];

            }
            for(i = 10; i < g_CalibrationData.m_nComponents; i++ )
            {

                fValue =    g_fComponentBackWeightAccumulator[i];
                fValue *=    g_fWeightConversionFactor;
                          // rounding to nearest 4TH decimal digit.
                fValue += 0.00005;
                // Wt to 2dp in MB.
                uValue.lValue = (long)(fValue * 100.0f);
                // M.s.Byte.
                g_arrnMBTable[PSUM_BLOCK_11 + ((i-10)*PSUM_BLOCK_SIZE) + PSUM_BROLL_WT_OFFSET] = uValue.nValue[0];
                // l.s.Byte.
                g_arrnMBTable[PSUM_BLOCK_11 + ((i-10)*PSUM_BLOCK_SIZE) + PSUM_BROLL_WT_OFFSET + 1] = uValue.nValue[1];

            }
        }
    }
}





//////////////////////////////////////////////////////
// CopyOrderComponentWeightsToMB  asm = CPYOWTSMB
//
// Copy order component weights to modbus table
// Allow for up to 12 components
//
// P.Smith                          1-9-2005
//
//////////////////////////////////////////////////////
void CopyOrderComponentWeightsToMB( void )
{
unsigned int i;
float   fValue;
union WordAndDWord uValue;
    if(g_nProtocol == PROTOCOL_MODBUS)
    {
        if(g_CalibrationData.m_nComponents <= 10)
        {
            for(i = 0; i < g_CalibrationData.m_nComponents; i++ )
            {

                // actual component weights
                fValue =    g_fComponentOrderWeightAccumulator[i];
                fValue *=    g_fWeightConversionFactor;
                         // rounding to nearest 4TH decimal digit.
                fValue += 0.005;
                // Wt to 2dp in MB.
                uValue.lValue = (long)(fValue * 100.0f);
                // M.s.Byte.
                g_arrnMBTable[PSUM_BLOCK_1 + (i*PSUM_BLOCK_SIZE) + PSUM_ORDER_WT_OFFSET] = uValue.nValue[0];
                // l.s.Byte.
                g_arrnMBTable[PSUM_BLOCK_1 + (i*PSUM_BLOCK_SIZE) + PSUM_ORDER_WT_OFFSET + 1] = uValue.nValue[1];
            }
        }
        else  //--review-- modbus expansion
        {
            for(i = 0; i < 10; i++ )  // copy 10 component & then rest of components
            {
                // actual component weights
                fValue =    g_fComponentOrderWeightAccumulator[i];
                fValue *=    g_fWeightConversionFactor;
                          // rounding to nearest 4TH decimal digit.
                fValue += 0.005;
                // Wt to 2dp in MB.
                uValue.lValue = (long)(fValue * 100.0f);
                // M.s.Byte.
                g_arrnMBTable[PSUM_BLOCK_1 + (i*PSUM_BLOCK_SIZE) + PSUM_ORDER_WT_OFFSET] = uValue.nValue[0];
                // l.s.Byte.
                g_arrnMBTable[PSUM_BLOCK_1 + (i*PSUM_BLOCK_SIZE) + PSUM_ORDER_WT_OFFSET + 1] = uValue.nValue[1];
            }
                for(i = 10; i < g_CalibrationData.m_nComponents; i++ )
            {
            // actual component weights
            fValue =    g_fComponentOrderWeightAccumulator[i];
            fValue *=    g_fWeightConversionFactor;
                      // rounding to nearest 4TH decimal digit.
            fValue += 0.005;
            // Wt to 2dp in MB.
            uValue.lValue = (long)(fValue * 100.0f);
            // M.s.Byte.
            g_arrnMBTable[PSUM_BLOCK_11 + ((i-10)*PSUM_BLOCK_SIZE) + PSUM_ORDER_WT_OFFSET] = uValue.nValue[0];
            // l.s.Byte.
            g_arrnMBTable[PSUM_BLOCK_11 + ((i-10)*PSUM_BLOCK_SIZE) + PSUM_ORDER_WT_OFFSET + 1] = uValue.nValue[1];
            }
        }
    }
}


//////////////////////////////////////////////////////
// CopyShiftComponentWeightsToMB  asm = CPYSHWTSMB
//
// Copy shift component weights to modbus table
// Allow for up to 12 components
//
// P.Smith                          1-9-2005
//
//////////////////////////////////////////////////////
void CopyShiftComponentWeightsToMB( void )
{
unsigned int i;
float   fValue;
union WordAndDWord uValue;
    if(g_nProtocol == PROTOCOL_MODBUS)
    {
        if(g_CalibrationData.m_nComponents <= 10)
        {
            for(i = 0; i < g_CalibrationData.m_nComponents; i++ )
            {
                // actual component weights
                fValue =    g_fComponentShiftWeightOngoingAccumulator[i];
                // rounding to nearest 4TH decimal digit.
                fValue *=    g_fWeightConversionFactor;
                          fValue += 0.00005;
                // Wt to 4dp in MB.
                uValue.lValue = (long)(fValue * 10000.0f);
                // M.s.Byte.
                g_arrnMBTable[PSUM_BLOCK_1 + (i*PSUM_BLOCK_SIZE) + PSUM_SHIFT_WT_OFFSET] = uValue.nValue[0];
                // l.s.Byte.
                g_arrnMBTable[PSUM_BLOCK_1 + (i*PSUM_BLOCK_SIZE) + PSUM_SHIFT_WT_OFFSET + 1] = uValue.nValue[1];
            }
        }
        else  //--review-- modbus expansion
        {
            for(i = 0; i < 10; i++ )  // copy 10 component & then rest of components
            {
                fValue =    g_fComponentShiftWeightOngoingAccumulator[i];
                // rounding to nearest 4TH decimal digit.
               fValue *=    g_fWeightConversionFactor;
                          fValue += 0.00005;
                // Wt to 4dp in MB.
                uValue.lValue = (long)(fValue * 10000.0f);
                // M.s.Byte.
                g_arrnMBTable[PSUM_BLOCK_1 + (i*PSUM_BLOCK_SIZE) + PSUM_SHIFT_WT_OFFSET] = uValue.nValue[0];
                // l.s.Byte.
                g_arrnMBTable[PSUM_BLOCK_1 + (i*PSUM_BLOCK_SIZE) + PSUM_SHIFT_WT_OFFSET + 1] = uValue.nValue[1];
            }
            for(i = 10; i < g_CalibrationData.m_nComponents; i++ )
            {
                fValue =    g_fComponentShiftWeightOngoingAccumulator[i];
                // rounding to nearest 4TH decimal digit.
               fValue *=    g_fWeightConversionFactor;
                          fValue += 0.00005;
                // Wt to 4dp in MB.
                uValue.lValue = (long)(fValue * 10000.0f);
                // M.s.Byte.
                g_arrnMBTable[PSUM_BLOCK_11 + ((i-10)*PSUM_BLOCK_SIZE) + PSUM_SHIFT_WT_OFFSET] = uValue.nValue[0];
                // l.s.Byte.
                g_arrnMBTable[PSUM_BLOCK_11 + ((i-10)*PSUM_BLOCK_SIZE) + PSUM_SHIFT_WT_OFFSET + 1] = uValue.nValue[1];
            }
        }
    }
}





//////////////////////////////////////////////////////
// CopyShiftLengthToMB  asm = CPYSHFTLMB
//
// Copy shift length to modbus table
//
// P.Smith                          1-9-2005
//
//////////////////////////////////////////////////////
void CopyShiftLengthToMB( void )
{
    union   WordAndDWord uValue;
    float   fValue;
    if(g_nProtocol == PROTOCOL_MODBUS)
    {
        fValue = g_OngoingShift.m_fLength;
        fValue *=    g_fLengthConversionFactor;
        fValue += 0.005;        // for rounding to nearest 2nd decimal digit.
        uValue.lValue = (long)(fValue * 100.0f);
        g_arrnMBTable[BATCH_SUMMARY_SHIFT_LENGTH] = uValue.nValue[0];     // M.s.Byte.
        g_arrnMBTable[BATCH_SUMMARY_SHIFT_LENGTH + 1] = uValue.nValue[1];     // L.s.Byte.
    }
}


//////////////////////////////////////////////////////
// CopyVAC8IODiagnosticsToMB()
//
// Copy Diagnostic data for each VAC8io card to  MODBUS TABLE
//
// M.McKiernan                          29-05-2008
// First Pass
//
//////////////////////////////////////////////////////
void CopyVAC8IODiagnosticsToMB(void)
{
 int i;
 int nDiagBlockSize = VAC8_2_GOOD_TX_CTR - VAC8_1_GOOD_TX_CTR;
// int nVAC8IOCards = 1;     //todo....hard code to 1 card for now.
 union WordAndDWord   uValue;

//  if( g_CalibrationData.m_nComponents > 1 )     // no LiW cards for single component
   if(g_nVAC8IOCards > 0)
  {

   for(i=0; i<g_nVAC8IOCards && i<MAX_VAC8IO_CARDS; i++)
   {
  // good tx counter to Low Level Sensor card
       uValue.lValue = structVAC8IOData[i].m_lGoodTx;
       g_arrnMBTable[VAC8_1_GOOD_TX_CTR+(i*nDiagBlockSize)]   = uValue.nValue[0];
       g_arrnMBTable[VAC8_1_GOOD_TX_CTR+(i*nDiagBlockSize)+1] = uValue.nValue[1];

       uValue.lValue = structVAC8IOData[i].m_lGoodRx;
       g_arrnMBTable[VAC8_1_GOOD_RX_CTR+(i*nDiagBlockSize)]   = uValue.nValue[0];
       g_arrnMBTable[VAC8_1_GOOD_RX_CTR+(i*nDiagBlockSize)+1] = uValue.nValue[1];

       uValue.lValue = structVAC8IOData[i].m_lRxTimeoutCounter;
       g_arrnMBTable[VAC8_1_TIMEOUT_CTR+(i*nDiagBlockSize)]   = uValue.nValue[0];
       g_arrnMBTable[VAC8_1_TIMEOUT_CTR+(i*nDiagBlockSize)+1] = uValue.nValue[1];

       uValue.lValue = structVAC8IOData[i].m_lChecksumErrorCounter;
       g_arrnMBTable[VAC8_1_CHECKSUM_ERROR_CTR+(i*nDiagBlockSize)]   = uValue.nValue[0];
       g_arrnMBTable[VAC8_1_CHECKSUM_ERROR_CTR+(i*nDiagBlockSize)+1] = uValue.nValue[1];

   }

  }
}



//////////////////////////////////////////////////////
// CopyModbusDiagnosticData
//
// Copies modbus diagnostic data
//
//////////////////////////////////////////////////////
void CopyModbusDiagnosticData( void )
{
    unsigned int i,nIndex;
    if(g_arrnMBTable[BATCH_BLENDER_MODBUS_DIAGNOSTIC_ADDRESS] == 0)
    {
        g_arrnMBTable[BATCH_BLENDER_MODBUS_DIAGNOSTIC_ADDRESS] = 1;
    }
    nIndex = g_arrnMBTable[BATCH_BLENDER_MODBUS_DIAGNOSTIC_ADDRESS]-1;
    // check for end of table
    if(nIndex+NUMBER_OF_MODBUS_DIAGNOSTIC_CELLS > LAST_MODBUS_DATA)
    {
      nIndex =  LAST_MODBUS_DATA - NUMBER_OF_MODBUS_DIAGNOSTIC_CELLS-1;
      g_arrnMBTable[BATCH_BLENDER_MODBUS_DIAGNOSTIC_ADDRESS] = nIndex;
    }

// check for change in the data and diagnostic password, allow write to happen
    for( i = 0; i < NUMBER_OF_MODBUS_DIAGNOSTIC_CELLS; i++ )
    {
        g_arrnMBTable[BATCH_BLENDER_MODBUS_DIAGNOSTIC_DATA+i] = g_arrnMBTable[nIndex+i];
    }
}


//////////////////////////////////////////////////////
// CopyEncryptionDataToMB
//
// Copies modbus  data
//
//////////////////////////////////////////////////////
void CopyEncryptionDataToMB( void )
{
   union CharsAndWord   nSmall;
   nSmall.cValue[0] = g_OnBoardCalData.m_cPermanentOptions;     /* move options into one word */
    nSmall.cValue[1] = g_OnBoardCalData.m_cTemporaryOptions;
    g_arrnMBTable[BATCH_ENCRYPTION_OPTIONS] = nSmall.nValue;     /* asm MBOPTIONCODE */
    g_arrnMBTable[BATCH_ENCRYPTION_BLENDERRUN_HOUR_COUNTER] = g_OnBoardCalData.m_nBlenderRunHourCounter;  /* asm MBBLENDERRUNHRCTR */
    g_arrnMBTable[BATCH_ENCRYPTION_SOFTWAREID] = g_OnBoardCalData.m_lSoftwareID; /* asm MBSOFTWAREID */
    g_arrnMBTable[BATCH_ENCRYPTION_OPTIONS_2] = g_OnBoardCalData.m_wBlenderOptions;
}

/* mce12-15

;* COPY PROD. SUMM. DATA 'A'(EVERY 10S)  INTO MODBUS TABLE
CPYPSAMB:
        TST     PROTOCOL
        BPL     CPYPSAMX

        LDD     FROLLNO         ; FRONT ROLL NO.
        JSR     BCDHEX2
        STD     MBFROLLNO

        LDD     BROLLNO         ; BACK ROLL NO.
        JSR     BCDHEX2
        STD     MBBROLLNO
        JSR     CPYADCMB        ; COPY A/D VALUES INTO MB TABLE
;       JSR     CPYSTVMB        ; SCREW TYPE AND RPM's
        JSR     CPYFWTSMB       ; FRONT ROLL WT ACCUMULATORS (EACH COMP)
        JSR     CPYBWTSMB       ; BACK ROLL WT ACCUMULATORS (EACH COMP)
        JSR     CPYOWTSMB       ; ORDER WT ACCUMULATORS (EACH COMP)
        JSR     CPYSHWTSMB      ; SHFT WTS
        JSR     CPYSHFTLMB     ; SHIFT LENGTH
CPYPSAMX RTS


;* COPY FRONT ROLL ACCUMULATED WT.S INTO MODBUS TABLE PROD. SUMMMARY
CPYFWTSMB:
        TST     PROTOCOL
        BPL     CPYFWTSMX

        LDAB   #RAMBANK
        TBZK                    ;~;SET EK FOR BANK 1, I.E. RAM
        LDZ     #MB1FRW         ; MODBUS TABLE, CH #1 FRONT ROLL WT
        LDAB    #1              ; BLEND #, START AT 1

CPYFWTS10 PSHB
        LDX     #CH1WACCF
        DECB
        LDAA    #5              ; 5 BYTES/ACCUMULATOR
        MUL
        ABX                     ; X NOW POINTS TO THIS CHANNELS FRONT ROLL ACC.
        JSR     BCDHEX3X        ; CONVERT 3 BYTES @X
        CLRA
        STD     0,Z
        STE     2,Z

        AIZ     #MBPSBKSIZ       ; INC. ON TO NEXT CH. MB PROD SUMM BLOCK
        PULB
        INCB


        LDAA    NOBLNDS
        CMPA    #TEN
        BLS     CMPNFW
        CMPB    #TEN
        BLS     CPYFWTS10

        LDX     #CH11WACCF
        JSR     BCDHEX3X        ; CONVERT 3 BYTES @X
        CLRA
        STD     MB11FRW
        STE     MB11FRW+2

        LDX     #CH12WACCF
        JSR     BCDHEX3X        ; CONVERT 3 BYTES @X
        CLRA
        STD     MB12FRW
        STE     MB12FRW+2
        BRA     CONRWC

CMPNFW  CMPB    NOBLNDS
        BLS     CPYFWTS10


CONRWC  TST     AVECPIB         ; IS THERE PIB ASSOCIATED?
        BEQ     CPYFWTSMX       ; NO, THEN SKIP

        LDX     #LIQWACCF       ; LIQUID WT. ACCUMULATOR
        JSR     BCDHEX3X
        CLRA
        STD     MBLFRW+0
        STE     MBLFRW+2

CPYFWTSMX:
        RTS
; COPY BACK ROLL ACCUMULATED WT.S INTO MODBUS TABLE PROD. SUMMMARY
CPYBWTSMB:
        TST     PROTOCOL
        LBPL     CPYBWTSMX

        TST     SHTTUBE
        BEQ     CPYBWTSMX       ; IF TUBE (00) THEN SKIP BACK ROLL WTS

        LDAB   #RAMBANK
        TBZK                    ;~;SET EK FOR BANK 1, I.E. RAM
        LDZ     #MB1BRW         ; MODBUS TABLE, CH #1 BACK ROLL WT
        LDAB    #1              ; BLEND #, START AT 1

CPYBWTS10 PSHB
        LDX     #CH1WACCB
        DECB
        LDAA    #5              ; 5 BYTES/ACCUMULATOR
        MUL
        ABX                     ; X NOW POINTS TO THIS CHANNELS FRONT ROLL ACC.
        JSR     BCDHEX3X        ; CONVERT 3 BYTES @X
        CLRA
        STD     0,Z
        STE     2,Z

        AIZ     #MBPSBKSIZ       ; INC. ON TO NEXT CH. MB PROD SUMM BLOCK
        PULB
        INCB
        CMPB    NOBLNDS
        BLS     CPYBWTS10


        LDAA    NOBLNDS
        CMPA    #TEN
        BLS     CMPBRW
        CMPB    #TEN
        BLS     CPYBWTS10

        LDX     #CH11WACCB
        JSR     BCDHEX3X        ; CONVERT 3 BYTES @X
        CLRA
        STD     MB11FRW
        STE     MB11FRW+2

        LDX     #CH12WACCB
        JSR     BCDHEX3X        ; CONVERT 3 BYTES @X
        CLRA
        STD     MB12BRW
        STE     MB12BRW+2
        BRA     CONBRW

CMPBRW  CMPB    NOBLNDS
        BLS     CPYBWTS10


CONBRW  TST     AVECPIB         ; IS THERE PIB ASSOCIATED?
        BEQ     CPYBWTSMX       ; NO, THEN SKIP


        LDX     #LIQWACCB       ; LIQUID WT. ACCUMULATOR back roll
        JSR     BCDHEX3X
        CLRA
        STD     MBLBRW+0
        STE     MBLBRW+2

CPYBWTSMX:
        RTS

; COPY ORDER ACCUMULATED WT.S INTO MODBUS TABLE PROD. SUMMMARY
CPYOWTSMB:
        TST     PROTOCOL
        BPL     CPYOWTSMX

        LDAB   #RAMBANK
        TBZK                    ;~;SET EK FOR BANK 1, I.E. RAM
        LDZ     #MB1ORDW        ; MODBUS TABLE, CH #1 ORDER WT
        LDAB    #1              ; BLEND #, START AT 1

CPYOWTS10 PSHB
        LDX     #CH1WACCO
        DECB
        LDAA    #5              ; 5 BYTES/ACCUMULATOR
        MUL
        ABX                     ; X NOW POINTS TO THIS CHANNELS ORDER ACC.
        JSR     BCDHEX4X        ; CONVERT 3 BYTES @X
        STD     0,Z
        STE     2,Z

        AIZ     #MBPSBKSIZ       ; INC. ON TO NEXT CH. MB PROD SUMM BLOCK
        PULB
        INCB


        LDAA    NOBLNDS
        CMPA    #TEN
        BLS     CMPNOW
        CMPB    #TEN
        BLS     CPYOWTS10

        LDX     #CH11WACCO
        JSR     BCDHEX4X        ;
        STD     MB11ORDW
        STE     MB11ORDW+2

        LDX     #CH12WACCO
        JSR     BCDHEX4X        ;
        STD     MB12ORDW
        STE     MB12ORDW+2
        BRA     CONORW

CMPNOW  CMPB    NOBLNDS
        BLS     CPYOWTS10

CONORW  TST     AVECPIB         ; IS THERE PIB ASSOCIATED?
        BEQ     CPYOWTSMX       ; NO, THEN SKIP

        LDX     #LIQWACCO       ; LIQUID WT. ACCUMULATOR back roll
        JSR     BCDHEX3X
        CLRA
        STD     MBLORDW+0
        STE     MBLORDW+2

CPYOWTSMX:
        RTS


;* COPY SHIFT LENGTH  INTO MODBUS TABLE PROD. SUMMMARY DATA
CPYSHFTLMB:
        TST     PROTOCOL
        BPL     CPYSHFTLMX
        LDX     #SHFTLTACC
        JSR     BCDHEX3X         ; CONVERT 3 BYTES BCD  TO HEX
        CLRA
        STD     MBSHFTL
        STE     MBSHFTL+2        ; MAX VALUE EXPECTED = 999999 m
CPYSHFTLMX RTS



; COPY SHIFT ACCUMULATED WT.S INTO MODBUS TABLE PROD. SUMMMARY
CPYSHWTSMB:
        TST     PROTOCOL
        LBPL    CPYSHWTSMX

        LDAB   #RAMBANK
        TBZK                    ;~;SET EK FOR BANK 1, I.E. RAM
        LDZ     #MB1SHFTW        ; MODBUS TABLE, CH #1 SHIFT WT
        LDAB    #1              ; BLEND #, START AT 1

CPYSHWTS10 PSHB
        LDX     #CH1WACCS9
        DECB
        LDAA    #5              ; 5 BYTES/ACCUMULATOR
        MUL
        ABX                     ; X NOW POINTS TO THIS CHANNELS SHIFT ACC.
        JSR     BCDHEX4X        ; CONVERT 4 BYTES @X
        STD     0,Z
        STE     2,Z

        AIZ     #MBPSBKSIZ       ; INC. ON TO NEXT CH. MB PROD SUMM BLOCK
        PULB
        INCB
        CMPB    NOBLNDS
        BLS     CPYSHWTS10

CPYSHWTS20:
;total (no dp)
        LDX     #TOTWACCS9         ; SHIFT Wt.
        JSR     BCDHEX3X          ; CONVERT 3 BYTES BCD  TO HEX
        CLRA
        STD     MBSHFTW+0
        STE     MBSHFTW+2

CPYSHWTSMX:
        RTS


*/
