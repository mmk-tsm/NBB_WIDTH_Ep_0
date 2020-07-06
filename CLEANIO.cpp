//////////////////////////////////////////////////////
// BBCalc.c
//

// P.Smith                          1-10-2004
// Initial pass
//
// P.Smith                          16-3-2005
// Add clean i/o toggles
//
// added void StartMouldClean( void );
//
// P.Smith                              4/10/05
// Rename low priority todos
// 
// P.Smith                              4/11/05
// correct check of g_nLeftCornerCleanOnCounter & g_nRightCornerCleanOnCounter
//
// 
// P.Smith                              7/11/05
// Ensure tha g_bOverrideOutputs is set in ToggleOffline, ToggleBypass,ToggleDump
// Correct CheckForCleanJetOnOff when checking clean counters
// 
// P.Smith                              10/1/06
// correct warnings
// change #include <Batvars2.h> to #include "Batvars2.h"
// 
// P.Smith                      20/2/06
// first pass at netburner hardware conversion.
// Bool -> BOOL
// included  <basictypes.h>
// assign clean outputs for NBB hardware.
// scr25 - top clean
// scr26 left corner clean
// scr27 right corner clean
// scr28 mixer clean
// implement pitweigh.cpp todos #include    "PITWeigh.h"
// 
// P.Smith                          19/4/06
// call InitiateCleanCycle();, StartMouldClean not called anywhere
// #include "CycleMonitor.h"
//
// P.Smith                          4/12/06
// Remove comments that are no longer valid
//
// P.Smith                          16/1/07
// Removed cleaning outputs, no longer available on RevB1
//
// P.Smith                          5/2/07
// Remove iprintf
//
// P.Smith                          3/5/07
// added activation of src17,18,19 to allow mapping to
// work.
// change expansion card outputs so that they are sequential on the card output
//
// P.Smith                          23/5/07
// remove seto of expansion outputs for cleaning
// allow mapping of blender mixer clean to source output 20.
//
// P.Smith                          16/10/07
// Ensure that all cleans are switched off continuously when not in use.
// Previous to this, the cleans were only switched off at when the cleans 
// were active.
// g_nOverrideCounter set to ensure that clean override is cancelled after a period of time
//
// P.Smith                          20/11/07
// function SwitchAllCleansOff() switches off all clean outputs
// called on the clean toggles to ensure that only one clean is switched on at a time.
//        
// P.Smith                          30/1/08
// correct compiler warnings
// remove commented out code.
//
//////////////////////////////////////////////////////

//*****************************************************************************
// INCLUDES
//*****************************************************************************
#include <Stdio.h>
#include <StdLib.h>

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
#include    "FiftyHz.h"
#include    "BatVars.h"
#include "SetpointFormat.h"
#include "Cleanio.h"
#include "Batvars2.h"
#include "CycleMonitor.h"
#include <basictypes.h>
#include "NBBGpio.h"


extern  BOOL        g_bExpanioFitted;                                             // EXTERNALCLEANPCB        DS      1       ; DETERMINES IS EXTERNAL CLEAN PCB SHOULD BE WRITTEN TO.
extern  int     g_nKeyCode;


// Locally declared global variables
// Externally declared global variables
// Data


//////////////////////////////////////////////////////
// CheckForCleanJetOnOff( void )            from ASM = CHECKFORCLEANJETONOFF
// 
//
// P.Smith                          3-11-2004
//////////////////////////////////////////////////////
void CheckForCleanJetOnOff ( void )
{

    if(!g_bOverrideClean)
    {
        if(g_nAirJet1OnCounter != 0)
        {
            g_nAirJet1OnCounter--;
            BlenderTopCleanOn();
        }
        else
        {
            BlenderTopCleanOff();
        }
            
        if(g_nAirJet2OnCounter != 0)         // check for non zero mixer clean counter
        {
                g_nAirJet2OnCounter--;
                BlenderMixerCleanOn();  // mixer clean on
        }
        else
        {                
            BlenderMixerCleanOff(); // mixer clean off
        }
        

        if(g_nLeftCornerCleanOnCounter != 0)
        {
            g_nLeftCornerCleanOnCounter--;
            LeftCornerCleanOn();
        }
        else
        {
            LeftCornerCleanOff();
        }
            
    
        if(g_nRightCornerCleanOnCounter != 0)
        {
            g_nRightCornerCleanOnCounter--;
            RightCornerCleanOn();
        }
        else
        {
                RightCornerCleanOff();
        }
        
    }    

}





//////////////////////////////////////////////////////
// BlenderTopCleanOn( void )            from ASM = AIRJETONEON
// 
//
// P.Smith                          1-10-2004
//////////////////////////////////////////////////////
void BlenderTopCleanOn ( void )
{

    g_bBlenderTopCleanOn = TRUE;
    SRCOnWithMapping(17); //nbb--todoallocate-- allocate this output
}


//////////////////////////////////////////////////////
// BlenderTopCleanOff( void )           from ASM = AIRJET1OFF
// Switch the top cleaning output off
//
// P.Smith                          1-10-2004
// First pass.
//////////////////////////////////////////////////////
void BlenderTopCleanOff ( void )
{
    g_bBlenderTopCleanOn = FALSE;
    SRCOffWithMapping(17); //nbb--todoallocate-- allocate this output
}



//////////////////////////////////////////////////////
// BlenderMixerCleanOn( void )          from ASM = AIRJETTWOON
// Switches on mixer clean output
//
//////////////////////////////////////////////////////
void BlenderMixerCleanOn ( void )
{
    g_bBlenderMixerCleanOn = TRUE;
    SRCOnWithMapping(14); 
    SRCOnWithMapping(20); //nbb--todoallocate-- allocate this output
}



//////////////////////////////////////////////////////
// BlenderMixerCleanOff( void )         from ASM = AIRJETTWOOFF
// Switches on mixer clean output
//
//////////////////////////////////////////////////////
void BlenderMixerCleanOff ( void )
{
    g_bBlenderMixerCleanOn = FALSE;
    SRCOffWithMapping(14); //nbb--todoallocate-- allocate this output
    SRCOffWithMapping(20); //nbb--todoallocate-- allocate this output
    
}




//////////////////////////////////////////////////////
// LeftCornerCleanOn( void )            from ASM = CORNERCLEANLEFTON
// Switches left corner clean output on
//
//////////////////////////////////////////////////////
void LeftCornerCleanOn ( void )
{

//    g_nExpansionOutput = g_nExpansionOutput | EXPANSIONOUTPUT6BITPOS; // set bit position for expansion PCB
    g_bLeftCornerCleanOn = TRUE;
    SRCOnWithMapping(19); //nbb--todoallocate-- allocate this output
    
}



//////////////////////////////////////////////////////
// RightCornerCleanOn( void )           from ASM = CORNERCLEANRIGHTON
// Switches right corner clean output on
//
//////////////////////////////////////////////////////
void RightCornerCleanOn ( void )
{

//    g_nExpansionOutput = g_nExpansionOutput | EXPANSIONOUTPUT7BITPOS; // set bit position for expansion PCB
    g_bRightCornerCleanOn = TRUE;
    SRCOnWithMapping(18); //nbb--todoallocate-- allocate this output
}

//////////////////////////////////////////////////////
// LeftCornerCleanOff( void )           from ASM = CORNERCLEANLEFTOFF
// Switch the left corner clean off
//
//////////////////////////////////////////////////////
void LeftCornerCleanOff ( void )
{
    g_bLeftCornerCleanOn = FALSE;
    SRCOffWithMapping(19); //nbb--todoallocate-- allocate this output   
}


//////////////////////////////////////////////////////
// RightCornerCleanOff( void )          from ASM = CORNERCLEANRIGHTOFF
// Switch the left corner clean off
//
//////////////////////////////////////////////////////
void RightCornerCleanOff ( void )
{
    g_bRightCornerCleanOn = FALSE;
    SRCOffWithMapping(18); //nbb--todoallocate-- allocate this output
}



//////////////////////////////////////////////////////
// ToggleBlenderTopClean( void )          from ASM = TOGGLEAIRJET1
//
//
// P.Smith                                  16-03-2005
// Toggle blender top clean
//////////////////////////////////////////////////////
void ToggleBlenderTopClean ( void )
{
    if(g_bBlenderTopCleanOn)
    {
        BlenderTopCleanOff();
        g_bOverrideClean = TRUE;
        g_nOverrideCounter = OVERRIDETIMEOUT;
    }
    else
    {
        SwitchAllCleansOff();
        BlenderTopCleanOn();
        g_bOverrideClean = TRUE;
        g_nOverrideCounter = OVERRIDETIMEOUT;
    }
}



//////////////////////////////////////////////////////
// ToggleBlenderMixerClean( void )          from ASM = TOGGLEAIRJET2
//
//
// P.Smith                                  16-03-2005
// Toggle blender top clean
//////////////////////////////////////////////////////
void ToggleBlenderMixerClean ( void )
{
    if(g_bBlenderMixerCleanOn)
    {
        BlenderMixerCleanOff();
        g_bOverrideClean = TRUE;
        g_nOverrideCounter = OVERRIDETIMEOUT;
    }
    else
    {
        SwitchAllCleansOff();
        BlenderMixerCleanOn();
        g_bOverrideClean = TRUE;
        g_nOverrideCounter = OVERRIDETIMEOUT;
    }
}




//////////////////////////////////////////////////////
// ToggleRightCornerClean( void )          from ASM = TOGGLECCAIRJETR
//
//
// P.Smith                                  16-03-2005
// Toggle blender top clean
//////////////////////////////////////////////////////
void ToggleRightCornerClean ( void )
{
    if(g_bRightCornerCleanOn)
    {
        RightCornerCleanOff();
        g_bOverrideClean = TRUE;
        g_nOverrideCounter = OVERRIDETIMEOUT;
    }
    else
    {
        SwitchAllCleansOff();
        RightCornerCleanOn();
        g_bOverrideClean = TRUE;
        g_nOverrideCounter = OVERRIDETIMEOUT;
    }
}



//////////////////////////////////////////////////////
// ToggleLeftCornerClean( void )          from ASM = TOGGLEAIRJETL
//
//
// P.Smith                                  16-03-2005
// Toggle blender top clean
//////////////////////////////////////////////////////
void ToggleLeftCornerClean ( void )
{
    if(g_bLeftCornerCleanOn)
    {
        LeftCornerCleanOff();
        g_bOverrideClean = TRUE;
        g_nOverrideCounter = OVERRIDETIMEOUT;
    }
    else
    {
        SwitchAllCleansOff();
        LeftCornerCleanOn();
        g_bOverrideClean = TRUE;
        g_nOverrideCounter = OVERRIDETIMEOUT;
    }
}


//////////////////////////////////////////////////////
// SwitchAllCleansOff( void )          
// switches all the cleans off, this is to correct a problem
// that has been seen on the multiblend, in that when all the 
// outputs are switched on together, the valves do not switch
// off until all the valves have sufficient back pressure to
// switch off all the pilots.
//
//////////////////////////////////////////////////////
void SwitchAllCleansOff ( void )
{
    BlenderTopCleanOff();
    LeftCornerCleanOff();
    RightCornerCleanOff();
    BlenderMixerCleanOff();
}



//////////////////////////////////////////////////////
// ToggleDump( void )          from ASM = TOGGLEDUMP
//
//
// P.Smith                                  16-03-2005
// Toggle blender dump
//////////////////////////////////////////////////////
void ToggleDump ( void )
{
    if( g_bDumpStatus )
    {
        CloseDump1();
        g_bOverrideOutputs = FALSE;
    }
    else
    {
        OpenDump1();
        g_bOverrideOutputs = TRUE;
    }
}
    

//////////////////////////////////////////////////////
// ToggleBypass( void )          from ASM = TOGGLEBYPASS
//
//
// P.Smith                                  16-03-2005
// Toggle blender dump
//////////////////////////////////////////////////////
void ToggleBypass ( void )
{
    if(g_bBypassStatus)  // if open
    {
        CloseBypass1();  // close bypass
        g_bOverrideOutputs = FALSE;
    }
    else
    {
        OpenBypass1();   // open bypass
        g_bOverrideOutputs = TRUE;
    }
}
    
 
 
 
 
 //////////////////////////////////////////////////////
// ToggleOffline( void )          from ASM = TOGGLEOFFLINE
//
//
// P.Smith                                  16-03-2005
// Toggle offline
//////////////////////////////////////////////////////
void ToggleOffline ( void )
{
    if(g_bFeedValveClosed)  //
    {
        OpenFeed1();  // open feed
        g_bOverrideOutputs = FALSE;
    }
    else
    {
        CloseFeed1();   // close offline
        g_bOverrideOutputs = TRUE;
    }
}





 
 //////////////////////////////////////////////////////
// CheckForCleanTest( void )          from ASM = CHECKFORCLEANTEST
//
//
// P.Smith                                  21-03-2005
//////////////////////////////////////////////////////

;// --review-- should be run inline with diagnostics page.

/*nbb--todolp-- comment out for now, seems unused void CheckForCleanTest ( void )
{
    if(g_bDiagnosticsPasswordInUse)        // only allow tests on diagnostics password
    {
        switch( g_nKeyCode )
        {
        case KEY_DIGIT_1:
        ToggleBlenderTopClean();
        break;
  
        case KEY_DIGIT_4:
        ToggleBlenderMixerClean();
        break;

        case KEY_DIGIT_7:
//        ToggleCornerCleans(); unused for the moment.
        break;

        default:
//        bChanged = TRUE;
        break;
        }

    }
}

nbb*/

 
 //////////////////////////////////////////////////////
// DecimalPointKeyDiagnostics      from ASM = DPKEYDIAG
//
//
// P.Smith                                  21-03-2005
// Activates Right corner clean
//////////////////////////////////////////////////////


void DecimalPointKeyDiagnostics ( void )
{
    if(g_bDiagnosticsPasswordInUse)        // only allow tests on diagnostics password
    {
        ToggleRightCornerClean();
    }
}



 
 //////////////////////////////////////////////////////
// CheckForValveDiagnostics      from ASM = CHECKFORVALVEDIAGNOSTICS
// Merge ACTIVATEPRODDIAGNOSTICS into this file
//
//
// P.Smith                                  7-11-2005
// Indicates what valve should be toggle via the code passed to the function.
//////////////////////////////////////////////////////

void CheckForValveDiagnostics (int nCode)
{
    if(g_bDiagnosticsPasswordInUse)        // only allow tests on diagnostics password
    {
        if(nCode < TOGGLE_STATUS_COMMAND_OUTPUT_DUMP)
        {
            /* merge OPENCLOSESLIDEN INLINE */
            if(g_bFillStatCmp[nCode-1])
            CmpNOff( nCode, TRUE );        //MBCMPNOFF      ; SWITCH COMPONENT #N OFF.
            else
            CmpNOn( nCode, TRUE );        //MBCMPNON        ; SWITCH COMPONENT #N ON.
       }
        else
        {
            switch(nCode)
            {
            case TOGGLE_STATUS_COMMAND_OUTPUT_DUMP:
                ToggleDump();
                break;
  
            case TOGGLE_STATUS_COMMAND_OUTPUT_OFFLINE:
                ToggleOffline();
                break;

            case TOGGLE_STATUS_COMMAND_OUTPUT_BYPASS:
                ToggleBypass();
                break;

            case TOGGLE_STATUS_COMMAND_OUTPUT_TOP_CLEAN:
                ToggleBlenderTopClean();              // 
                break;

            case TOGGLE_STATUS_COMMAND_OUTPUT_LEFT_CORNER:
                ToggleLeftCornerClean();        // toggle left cc
                break;
            case TOGGLE_STATUS_COMMAND_OUTPUT_RIGHT_CORNER_CLEAN:
                ToggleRightCornerClean();
                break;
            case TOGGLE_STATUS_COMMAND_OUTPUT_MIXER_CLEAN :   
                ToggleBlenderMixerClean();
                break;

            default:
            break;
            }
        }
    }
}



/*

TOGGLECCAIRJET:
        TST     CORNERCLEANONF
        BEQ     SWCCON
        JSR     CORNERCLEANLEFTOFF     
        JSR     CORNERCLEANRIGHTOFF     
        CLR     OVERRIDECLEAN
        RTS

SWCCON  JSR     CORNERCLEANLEFTON    ; LEFT CORNER CLEAN ON
        JSR     CORNERCLEANRIGHTON   ; RIGHT CORNER CLEAN ON
        LDAA    #1
        STAA    OVERRIDECLEAN
        RTS


TOGGLECCAIRJETL:
        TST     CORNERCLEANONF
        BEQ     SWLCCON
        JSR     CORNERCLEANLEFTOFF    
        CLR     OVERRIDECLEAN
        RTS

SWLCCON JSR     CORNERCLEANLEFTON    ; LEFT CORNER CLEAN ON
        LDAA    #1
        STAA    OVERRIDECLEAN
        RTS


TOGGLECCAIRJETR:
        TST     CORNERCLEANRONF
        BEQ     SWRCCON
        JSR     CORNERCLEANRIGHTOFF     
        CLR     OVERRIDECLEAN
        RTS

SWRCCON JSR     CORNERCLEANRIGHTON   ; RIGHT CORNER CLEAN ON
        LDAA    #1
        STAA    OVERRIDECLEAN
        RTS

*/



// --todolp-- void UpdateExpanio ( void )  //Updates expand io PCB outputs ASM - UPDATEEXPANIO


 
 //////////////////////////////////////////////////////
// StartMouldClean      from ASM = STARTMOULDCLEAN
//
//
// P.Smith                                  1-04-2005
// Starts mould cleaning, note no refilling of batch
//////////////////////////////////////////////////////

void StartMouldClean( void )  //
{
    g_bNoFillOnClean = TRUE;
    InitiateCleanCycle();
}    

/*

STARTMOULDCLEAN:
        LDAA     #1
        STAA     NOFILLONCLEAN   ; SET FLAG
        JSR     INITIATECLEANCYCLE
        RTS

 */









/*        
        
AIRJETONEON:

        JSR     EXPANOP8ON              ; O/P ON
        LDAA    #1
        STAA    AIRJET1ONF              ; AIR JET 1 ON FLAG
        TST     EXTERNALCLEANPCB        ; EXTERNAL PCB
        BNE     XTAJ1ON
        JSR     ADL1Q6_ON
XTAJ1ON RTS
                             
        
AIRJETONEOFF:
        JSR     EXPANOP8OFF             ; O/P OFF
        CLR     AIRJET1ONF              ; AIR JET 1 ON FLAG
        TST     EXTERNALCLEANPCB        ; EXTERNAL PCB
        BNE     XTAJ1OFF
        JSR     ADL1Q6_OFF
XTAJ1OFF RTS
        
        

        
AIRJETTWOON:
        JSR     EXPANOP5ON              ; O/P ON
        LDAA    #1
        STAA    AIRJET2ONF
        TST     EXTERNALCLEANPCB        ; EXTERNAL PCB
        BNE     XTAJ2ON
        JSR     ADL2Q5_ON       ; START UP ON
XTAJ2ON RTS
        

AIRJETTWOOFF:
        JSR     EXPANOP5OFF              ; O/P OFF
        CLR     AIRJET2ONF
        TST     EXTERNALCLEANPCB        ; EXTERNAL PCB
        BNE     XTAJ2OFF
        JSR     ADL2Q5_OFF 
XTAJ2OFF RTS
        

; 6TH COMP OUTPUT ON
        
CORNERCLEANLEFTON        
        JSR     EXPANOP7ON              ; O/P ON
        LDAA    #1
        STAA    CORNERCLEANONF
        TST     EXTERNALCLEANPCB        ; EXTERNAL PCB
        BNE     XTCCLON
        JSR     ADL2Q3_ON        ; LATCH OUTPUT ON.
XTCCLON RTS   

CORNERCLEANRIGHTON        
        JSR     EXPANOP6ON              ; O/P ON
        LDAA    #1
        STAA    CORNERCLEANRONF
        TST     EXTERNALCLEANPCB        ; EXTERNAL PCB
        BNE     XTCCRON
        JSR     ADL2Q2_ON       ; LATCH OUTPUT ON.
XTCCRON RTS   


        
; 6TH COMP OUTPUT OFF

     
CORNERCLEANLEFTOFF:     
        JSR     EXPANOP7OFF              ; O/P OFF
        LDAA    #0
        STAA    CORNERCLEANONF        
        TST     EXTERNALCLEANPCB        ; EXTERNAL PCB
        BNE     XTCCLOF
        JSR     ADL2Q3_OFF       ; LATCH OUTPUT OFF.
XTCCLOF RTS           


CORNERCLEANRIGHTOFF:     
        JSR     EXPANOP6OFF              ; O/P OFF
        LDAA    #0
        STAA    CORNERCLEANRONF        
        TST     EXTERNALCLEANPCB        ; EXTERNAL PCB
        BNE     XTCCROF
        JSR     ADL2Q2_OFF      ; LATCH OUTPUT OFF.
XTCCROF RTS           
          
          
CHECKFORCLEANTEST:
        LDAA    PGMNUM  
        CMPA    #CLEANDIAGPAGEID
        BNE     XITCLD
        LDD     PASWRD          ;
        CPD     #$0065          ; SPECIFIC PASSWORD ?
        BNE     XITCLD          ; EXIT CLEAN DIAG
        LDAA    PASWRD+2
        CMPA    #$53            ;
        BNE     XITCLD  
        LDAA    KYDATA          ; READ KEY
        CMPA    #ONE
        BNE     NOT1            ; NO
        JSR     TOGGLEAIRJET1   ; TOGGLE JET
        BRA     XITCLD          ;
NOT1    CMPA    #FOUR              ;
        BNE     NOT2            ;        
        JSR     TOGGLEAIRJET2   ; 
        BRA     XITCLD          ;
NOT2    CMPA    #SEVEN
        BNE     XITCLD            ; 
        JSR     TOGGLECCAIRJET   ; 
XITCLD  RTS                
          


TOGGLEAIRJET1:
        TST     AIRJET1ONF
        BEQ     SW1ON
        JSR     AIRJETONEOFF
        CLR     OVERRIDECLEAN
        RTS
        
SW1ON   JSR     AIRJETONEON      ; AIR JET 2 ON
        LDAA    #1
        STAA    OVERRIDECLEAN
        RTS


TOGGLEAIRJET2:
        TST     AIRJET2ONF
        BEQ     SW2ON
        JSR     AIRJETTWOOFF
        CLR     OVERRIDECLEAN
        RTS
SW2ON   JSR     AIRJETTWOON     ; AIR JET 2 ON
        LDAA    #1
        STAA    OVERRIDECLEAN
        RTS



TOGGLECCAIRJET:
        TST     CORNERCLEANONF
        BEQ     SWCCON
        JSR     CORNERCLEANLEFTOFF:     
        JSR     CORNERCLEANRIGHTOFF:     
        CLR     OVERRIDECLEAN
        RTS

SWCCON  JSR     CORNERCLEANLEFTON    ; LEFT CORNER CLEAN ON
        JSR     CORNERCLEANRIGHTON   ; RIGHT CORNER CLEAN ON
        LDAA    #1
        STAA    OVERRIDECLEAN
        RTS


TOGGLECCAIRJETL:
        TST     CORNERCLEANONF
        BEQ     SWLCCON
        JSR     CORNERCLEANLEFTOFF:     
        CLR     OVERRIDECLEAN
        RTS

SWLCCON JSR     CORNERCLEANLEFTON    ; LEFT CORNER CLEAN ON
        LDAA    #1
        STAA    OVERRIDECLEAN
        RTS


TOGGLECCAIRJETR:
        TST     CORNERCLEANRONF
        BEQ     SWRCCON
        JSR     CORNERCLEANRIGHTOFF:     
        CLR     OVERRIDECLEAN
        RTS

SWRCCON JSR     CORNERCLEANRIGHTON   ; RIGHT CORNER CLEAN ON
        LDAA    #1
        STAA    OVERRIDECLEAN
        RTS



CHECKFORVALVEDIAGNOSTICS:
        LDAA    PGMNUM  
        CMPA    #OPDIAGPAGEID
        BNE     XITVD
        TST     ISDIAGNOSTICSPASSWORD
        BEQ     XITVD
        LDAB    KYDATA               ; READ THE NUMERIC KEY
        CMPB    #FOUR
        BHI     CHOTD                ; CHECK FOR OTHER DIAGNOSTICS
        CMPB    #0
        BNE     OPCLSL
        JSR     TOGGLEAIRJET2      
        BRA     XITVD

OPCLSL  JSR     OPENCLOSESLIDEN      ; OPEN CLOSE SLIDE N
XITVD   RTS     
        
CHOTD   CMPB    #FIVE                ; FIVE
        BNE     NOT5                 ; NO
        JSR     TOGGLEDUMP
        BRA     XITVD

NOT5    CMPB    #SIX
        BNE     NOT6                 ; NO
        JSR     TOGGLEOFFLINE
        BRA     XITVD
NOT6    CMPB    #SEVEN
        BNE     NOT7                 ;NO
        JSR     TOGGLEBYPASS
        BRA     XITVD

NOT7    CMPB    #EIGHT
        BNE     NOT8                 ; NO
        JSR     TOGGLEAIRJET1
        BRA     XITVD
NOT8    CMPB    #NINE
        BNE     NOT9                 ;NO
        JSR     TOGGLECCAIRJETL
NOT9    BRA     XITVD
                        


DPKEYDIAG:
        LDAA    PGMNUM  
        CMPA    #OPDIAGPAGEID
        BNE     XITDP
        TST     ISDIAGNOSTICSPASSWORD
        BEQ     XITDP
        JSR     TOGGLECCAIRJETR
XITDP   RTS                
        
                  
TOGGLEDUMP:
        TST     DUMPSTATUS              ; CHECK DUMP STATUS
        BEQ     OPDM                    ; OPEN DUMP
        JSR     CLOSEDUMP1                  
        CLR     OVERRIDEOUTPUTS
        BRA     XITDMO                  ; EXIT
OPDM    JSR     OPENDUMP1               ; OPEN DUMP
        LDAA    #1
        STAA    OVERRIDEOUTPUTS
XITDMO  RTS                             ;                          


TOGGLEBYPASS:
        TST    BYPASSSTATUS    ; INDICATE CLOSED.
        BEQ    OPBY            ; OPEN IT IF CLOSED
        JSR     CLOSEBYPASS1
        CLR     OVERRIDEOUTPUTS
        BRA     XITBYT         ; EXIT BYPASS TOGGLE
OPBY    JSR     OPENBYPASS1     ;
        LDAA    #1
        STAA    OVERRIDEOUTPUTS
XITBYT  RTS

TOGGLEOFFLINE:
        TST     FEEDCLOSE
        BEQ     CLFEED         ; CLOSE VALVE
        JSR     OPENFEED1
        CLR     OVERRIDEOUTPUTS
        BRA     XITTGO         ; EXIT
CLFEED  JSR     CLOSEFEED1       
        LDAA    #1
        STAA    OVERRIDEOUTPUTS
XITTGO  RTS                    ; EXIT                



UPDATEEXPANIO:
        TST     CLEANING
        BNE     UPEX                            ; UPDATE IF CLEANING ENABLED
        LDAA    NOBLNDS                         ; READ NO OF BLENDS
        CMPA    #TEN                            ; MORE THAN 10 COMPONENTS
        BLS     XITEXU                          ; EXIT IS 10 OR LESS

UPEX    TST     SPIEEPACTIVE                    ; SPI ACTIVE ?
        BNE     XITEX1     
        TST     SPIATDACTIVE                    ; SPI A/D ACTIVE
        BNE     XITEX1
        JSR     WRITE_EXPAN_OPSFROMEXPANOUTPUT
XITEXU  RTS
XITEX1  RTS
*/
