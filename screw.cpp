//////////////////////////////////////////////////////
// P.Smith                      15/11/07 
// added screw speed file
// CheckForScrewSpeedChange implemented
//
// P.Smith                      16/11/07 
// added TareScrewSpeed & CalculateScrewSpeedConstant
//
// P.Smith                      7/3/07 
// set g_bScrewIncreaseDeceasePressed to indicate that the d/a per kg should not 
// be calculated on the next cycle.
//////////////////////////////////////////////////////


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
#include "SetpointFormat.h"
#include "TimeDate.h"
#include "BatVars.h"
#include "BBCalc.h"
#include "MBTogSta.h"
#include "Alarms.h"
#include "KghCalc.h"
#include "BatVars2.h"
#include "Monbat.h"
#include "Blrep.h"
#include "MBProsum.h"
#include "Pause.h"
#include "CycleMonitor.h"
#include "TSMPeripheralsMBIndices.h"

#include <basictypes.h>
#include <string.h>
#include "NBBgpio.h"
#include "ConversionFactors.h"
#include "MultiblendMB.h"
#include "Mtttymux.h"


extern  CalDataStruct   g_CalibrationData;

//////////////////////////////////////////////////////
// CheckForScrewSpeedChange( void )      asm = CHECKFORSCREWSPEEDADCHANGE     
//
// Checks for change in screw speed, if change greater than deviation then 
// runest flag is set
//
//////////////////////////////////////////////////////
void CheckForScrewSpeedChange( void )
{
    float fPercentageChange;
    if((g_CalibrationData.m_nControlType == CONTROL_TYPE_INC_DEC) && (g_bManualAutoFlag == MANUAL)) // asm = INCREASEDECREASE
    {
      if(g_bFirstTimeToReadScrewSpeed)
        {
            g_bFirstTimeToReadScrewSpeed = FALSE;
            g_fExtruderRPMHistory = g_fExtruderRPM;
        }
        else  // check for change in screw speed.
        {
            if(g_fExtruderRPM > g_fExtruderRPMHistory)
            {
                fPercentageChange = 100.0 * ((g_fExtruderRPM - g_fExtruderRPMHistory)/ g_fExtruderRPMHistory);
            }
            else
            {
                fPercentageChange = 100.0 * ((g_fExtruderRPMHistory - g_fExtruderRPM)/ g_fExtruderRPMHistory);
            }
            if(fPercentageChange > SCREWSPEEDDEVIATIONLIMIT)
            {
                g_bRunEst = TRUE;  // estimation of kg/h and grams per meter
//                SetupMttty();
//                printf("\n change detected is %3.2f  history %3.2f per change %3.2f",g_fExtruderRPM,g_fExtruderRPMHistory,fPercentageChange);
                g_fExtruderRPMHistory = g_fExtruderRPM;
                g_bScrewIncreaseDeceasePressed = TRUE; // show change has taken place
            }
        }
                
    }
}

//////////////////////////////////////////////////////
// TareScrewSpeed ( void )      asm = SCREWSPEEDTARE     
// 
// Tares the screw speed
// 
//////////////////////////////////////////////////////
void TareScrewSpeed( void )
{
    g_CalibrationData.m_nScrewSpeedTareCounts = g_nExtruderTachoReadingAverage;
    g_bSaveAllCalibrationToEEprom = TRUE;

}

//////////////////////////////////////////////////////
// TareScrewSpeed ( void )      asm = SCREWSPEEDTARE     
// 
// Tares the screw speed
// 
//////////////////////////////////////////////////////
void CalculateScrewSpeedConstant( void )
{
    float fDiff;
    fDiff = float(g_nExtruderTachoReadingAverage - g_CalibrationData.m_nScrewSpeedTareCounts);
    g_CalibrationData.m_fScrewSpeedConstant = 100000 * (g_fScrewSpeedCalibrate / fDiff);
    g_bSaveAllCalibrationToEEprom = TRUE;

}



/*

*****************************************************************************;
; FUNCTION NAME : CHECKFORSCREWSPEEDADCHANGE
; FUNCTION      : CHECKS FOR CHANGE IN SCREW SPEED AND INITITATES RUN OF      ;
;                 ESTIMATE KG/HR AND GRAMS PER METER                            ;                                          ;                                          ;
; INPUTS        : SCREW SPEED A/D AVERAGE                                                        ;                       ;
;*****************************************************************************;

CHECKFORSCREWSPEEDADCHANGE:
        LDAA    CONTROLTYPE     ; CHECK MODE
        CMPA    #INCREASEDECREASE
        BNE     XITSSC
        TST     MANATO          ; 
        BNE     ADTOHIS          ; AUTO EXIT.

        
        TST     FIRSTTIMETOREADSSAD     ; FIRST TIME TO READ SCREW SPEED
        BEQ     CONADC                  ; CONTINUE IF NOT
        CLR     FIRSTTIMETOREADSSAD     ;
        BRA     ADTOHIS                 ; STORE TO HISTORY
CONADC  LDD     SEIADREADINGAVG
        BEQ     XITSSC                   ; EXIT IF ZERO.
        LDX     #SSADDEVIATION
        IDIV                            ; CALCULATE AMOUNT OF                 
        STX     SSDEVIATIONLIMIT        ; STORAGE OF DEVIATION LIMIT.
        
        LDD     SEIADREADINGAVG
        CPD     SEIADREADINGAVGHIS
        BHI     READNHI    

        LDD     SEIADREADINGAVGHIS
        SUBD    SEIADREADINGAVG
        BRA     CMPARE
                
READNHI LDD     SEIADREADINGAVG
        SUBD    SEIADREADINGAVGHIS
CMPARE  CPD     SSDEVIATIONLIMIT        ; COMPARE
        BLS     XITSSC       
        LDAA    #1
        STAA    RUNEST                  ; INITIATE ESIMTATION RUN
        LDAA    #ONE
        STAA    ESTCTRUSE               ; THE NEXT CYCLE SHOUDL ESTIMATE THE KG/H G/M

ADTOHIS LDX     SEIADREADINGAVG    
        STX     SEIADREADINGAVGHIS

XITSSC  RTS


SCREWSPEEDTARE:
       LDX      SEIADREADINGAVG
       STX    SSTARE      ;~& STORE IT
       RTS
;
;       CALCULATE SCREW SPEED CONSTANT TO ALLOW THE OPERATOR
;       TO ENTER THE SCREW SPEED THAT THE EXTRUDER IS RUNNING AT
;
;       SCREW SPEED ENTERED  SS SS 00 00 00 / A/D- TARE 00 00 TT TT TT
;       RESULT SHIFTED 12345 - 1234 CONSTANT IS 1234   
;
CALCSPCON:
        JSR     SSAD_TARE       ; A/D - TARE
        JSR     CLCREG          ; RESULT ALREADY IN A REGISTER.
        LDD     NKEYBUF+1
        STD     MATHCREG            ; RPM TO C REG
        JSR     DIV             ;C/A (SCREW SPEED) /(A/D - TARE)  

        LDX     #EREG
        LDAA    #1      
        JSR     GPRND1          ; ROUND TO 1 DECIMAL PLACE.

        LDAA     EREG+2 
        LDE      EREG+3   
        LSRA
        RORE
        LSRA
        RORE 
        LSRA
        RORE  
        LSRA
        RORE  
        STE     SSCON1
        LDAA   #$AA
        STAA   NCONSS   ;~FLAG A NEW SCREW SPD CONST
        JSR     CALCDARMP
        LDAA   #$AA
        STAA   CEEFLG1           ; SET FLAG -> AA TO STORE TO EEPROM. 
        RTS

C
                
*/
