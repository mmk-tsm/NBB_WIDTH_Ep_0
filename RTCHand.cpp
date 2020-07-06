//////////////////////////////////////////////////////
// RTCHandler
//
// PIT interrupt handler.
//
// M.Parks                              24-05-2000
// First Pass.
// M.McKiernan          16-08-2001
// Fixed routing for HB LED - was turning on/off the SSROP as well.
// M.McKiernan          18-10-2004
// Diagnostic to calculate the PITs per second average.
// M.McKiernan          22-11-2004
// Set the second rollover g_bSecondRollover, flag, to be used in 50Hz routine.
// M.McKiernan          02-12-2004
// Edited to include everything from teh corresponding assembler interrupt handler.- RTCINTH:
//
// M.McKiernan          20-12-2004
// Diagnostic to count foreground passes per second - g_nForegndPassesPerSec
//
// P.Smith       3-3-2005
// Correct Autocycler
// Ensure that counter only increment if the auto cycler is on.
//
// P.Smith                              4/10/05
// Rename low priority todos
// 
// P.Smith                              20/12/05
// No autocycler if single recipe mode
// 
// P.Smith                              8/2/06
// first pass at netburner hardware conversion.
// remove interrupt keyword in function void RTCHandler( void )
// remove if( *RTC_CD & 0x04 )  {
//           *RTC_CD = 0x00;
// removed //nbb extern  volatile    char *far   RTC_CD;
           //nbb extern  volatile    char *far   PORTE;
// included  <basictypes.h>
//  
// P.Smith                                 9/2/06
// remove g_nForegndPassCounter reference, will be added in later.
//
// P.Smith                                 22/2/06
// put back in      g_bSecondRollover = TRUE;       // SECROVER
//                  g_nForegndPassesPerSec = g_nForegndPassCounter;
//                  g_nForegndPassCounter = 0;

//
// P.Smith                                 4/7/07
// increment g_nLoaderEmptyingCounter counter on 1 seconds interrupt
//
// P.Smith                                 10/1/08
// set g_nAutocycleLevelSensorOff10hzCounter when autocycler is enabled
//
// P.Smith                                 16/9/09
// remove reference to g_nCycleTime
//////////////////////////////////////////////////////

//*****************************************************************************
// INCLUDES
//*****************************************************************************
#include "TimeDate.h"
#include "BatVars.h"
#include "BatchCalibrationFunctions.h"
#include <basictypes.h>
#include <stdio.h>
#include "BatVars2.h"


// Externally defined global variables
//nbb extern  volatile    char *far   RTC_CD;
//nbb extern  volatile    char *far   PORTE;
extern  int g_nPIT_Int_Counter;
extern  int g_nPITIntsPerSec;
extern  unsigned int g_nForegndPassCounter;

extern  int g_nPIT_Int_CounterU;
extern  int g_nPITIntsPerSecU;
extern CalDataStruct    g_CalibrationData;
        int    ntemp,ntemp1,ntemp2,ntemp3,ntemp4,ntemp5;
                  

// Global variables declared locally
long g_lPITsInMinSum = 0;
int g_nPITsInMinSumCtr = 0;
int g_nPITsPerSecAvg = 0;
int g_nMinuteCtr = 0;
int g_nForegndPassesPerSec = 0;

void RTCHandler( void )
{
    int i;
    g_bSecondRollover = TRUE;       // SECROVER
    for(i=0; i < MAX_COMPONENTS; i++)
    {
        g_nLoaderEmptyingCounter[i]++;
    } 
    g_nForegndPassesPerSec = g_nForegndPassCounter;
    g_nForegndPassCounter = 0;
    g_nRTCSecondCounter++;
    if( g_nRTCSecondCounter == 60 )
    {
        g_nRTCSecondCounter = 0;
        g_bRTCMinuteFlag = TRUE;        // set flag on Minute transition.
        g_nRTCMinuteCounter++;
        if( g_nRTCMinuteCounter == 60 )
        {
            g_nRTCMinuteCounter = 0;
            g_bHourFlag = TRUE;         // HOURFG set flag to show reached hour.
        }
    }
//          RTCCOUNT              ;SECONDS COUNTER  --not actually used in assembler--
//              RTCFLAG                 --not actually used in assembler-

    g_bOneSecondFlag = TRUE;        // ONESECONDFLAG, set by RTC.

    g_nKgHrSecondsCounter++;            // KGHRCOUNT             ; INCREMENT KG/HR COUNTER
    g_nPITFractionCounter = 0;          //  PITFRACTCTR

    if(g_bAutoCycleFlag )
    {
        g_nLevelSensorSimulator++;      //LEVELSIM              ; INCREMENT LEVEL SENSOR SIMULATOR
        if(g_nLevelSensorSimulator >= 60)
        {
            if(!g_nPauseFlag)       // not paused??
            {
                g_nNoCycle = 0;                 // NOCYCLE -- minute no. in the cycle
                g_nMixerOffCounter = 0;         // MIXEROFFCTR
                if(g_CalibrationData.m_nBlenderMode != MODE_SINGLERECIPEMODE)
                {
                    g_nAutocycleLevelSensorOff10hzCounter = AUTOCYCLELEVELSENSOROFFTIME10HZ;
                }
            }
        
            g_nLevelSensorSimulator = 0;        // LEVELSIM         
        }     
    }


// --REVIEW-- if an SEI is being used, then the pulse accumulation is not time critical and should be moved
//                  to OneHz program in foreground.

// --todolp-- OneSecondPulseAccumulate();  //ONESECPULACC:        ROUTINES TO READ THE COUNTERS WITH LINE SPEED INPUTS


// BGNFLG --not used in batch blender--

    g_nPITIntsPerSec = g_nPIT_Int_Counter;      // for test purposes mmk
    g_nPIT_Int_Counter = 0;

    g_nPITIntsPerSecU = g_nPIT_Int_CounterU;        // for test purposes mmk
    g_nPIT_Int_CounterU = 0;


/* testonly
// ---REVIEW-- diagnostic purposes only.
    g_lPITsInMinSum += g_nPITIntsPerSec;
    g_nPITsInMinSumCtr++;
    if(g_nPITsInMinSumCtr == 60)
    {
        g_nPITsPerSecAvg = g_lPITsInMinSum / 60;
        g_lPITsInMinSum = 0;
        g_nPITsInMinSumCtr = 0;
        g_nMinuteCtr++;
    }
*/

}


/*
RTCINTH:
          PSHM    D,E,X,Y,Z,K
          INCW   RTCCOUNT              ;SECONDS COUNTER
          INC    RTCFLAG               ;SET FLAG FOR RTC INTERRUPT OCCURRED.
          INC    SECROVER              ;SET SECOND ROLL OVER FLAG
          LDAA   #1
          STAA   ONESECONDFLAG
          INCW   KGHRCOUNT             ; INCREMENT KG/HR COUNTER
          CLRB
          LDAA   CYCLETIME+1  
          ADDA   #1
          DAA
          STAA   CYCLETIME+1       
          ADCB   CYCLETIME
          STAB   CYCLETIME               ;
          CLRW   PITFRACTCTR           ; CLEAR PIT FRACTION COUNTER     
ACYCLE    INC    LEVELSIM              ; INCREMENT LEVEL SENSOR SIMULATOR
          LDAA   LEVELSIM               
          CMPA   #60
          BNE    NOLESEN
          TST   AUTOCYCLEFLAG
          BEQ   NOLESEN                 ; IS AUTO CYCLER ENABLED.
          TST     PAUSFLG
          BNE     NOSETOFFLAG1            ; IN PAUSE MODE.
          CLR   NOCYCLE                  ; RESET COUNTER.        
          CLR   MIXEROFFCTR              ; RESET COUNTER ON CYCLE
          LDAA  #$0F
          STAA    DMPLEVSEN               ; DUMP LEVEL SENSOR ACTIVATED.
NOSETOFFLAG1:
          CLR    LEVELSIM
NOLESEN:   
         
          INC     MINCTR
          LDAA    MINCTR
          CMPA    #60
          BNE     NO1
          LDAA    #1
          STAA    HOURFG                ; SET HOUR COUNTER.
          CLR     MINCTR
NO1       JSR    ONESECPULACC      ;~READ L.SPEED COUNTER (SECONDS)

          TST    STBGNFLG       
          BEQ    RTCINT02  
          LDAA   #$AA
          STAA   BGNFLG                 ;SET FLAG TO START A/D CONVERSION
          CLR    STBGNFLG
RTCINT02:          
          LDAB   #HARDWAREBANK                   ; HARDWARE BANK
          TBZK                                   ; STORE BANK NO           
          LDZ   #RTC_CD                ;READ REG CD (IRQFLAG)
          LDAB  0,Z
;          LDAB   RTC_CD                ;READ REG CD (IRQFLAG)
          BITB   #$04                  ;TEST IRQ FLAG BIT
          BEQ    RTCINTX
          JSR    DLY05US               ;SOFTWARE DELAY AT LEAST 3 uS
          LDZ   #RTC_CD                ;READ REG CD (IRQFLAG)
          LDAB   #$00
          STAB  0,Z
;          STAB   RTC_CD                ;CLR IRQ FLAG BIT TO CLR INTERRUPT
RTCINTX:  PULM    D,E,X,Y,Z,K
          RTI

*/
