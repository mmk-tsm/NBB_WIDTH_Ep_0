//////////////////////////////////////////////////////
// FiftyHz.c
//
// Group of functions that are called on 50Hz, on PIT.  equates to routines that were in PITCALL, or FIFTYHZHAND.
//
// M.McKiernan                          25-06-2004
// First Pass
// M.McKiernan                          09.09.2004
// Added ReadHighLowLevelSensors( void )            from ASM = READHIGHLOWSENSORS
//
// M.McKiernan                          17-11-2004
// Replaced g_bPauseFlag by g_nPauseFlag, there is more than one type of Pause.
// Added FiftyHzHandler - contains all the stuff from PITCALL, though broken up and some stuff earmarked
// to be shifted to the foreground program e.g. ForegroundOneHzProgram() & SHIFTTIMECHK
// CheckPauseOnInput( void )
//
// M.McKiernan                          03-12-2004
// Changed comment where g_bForegroundSecondRollover is set.  Calling function now written.
//
// M.McKiernan                          13-12-2004
// Activated PutAlarmTable( LEVELSENSORALARM,  0 );
//
// P.Smith                          10-02-2005
// PollRemoteEI() implemented
//
// P.Smith                          4-3-2005
// Remove comments from ToManual
// SETDUMPOUTPUT equivalent merged inline.
//
// P,Smith                          30-06-2005
// Correct the start of the mixer when level sensor goes from covered to uncovered
//
//
// P.Smith                          4-08-2005
// Uncomment Shift time check call.
//
//
// P.Smith                          12-08-2005
// decrement g_nMixerTransitionCounter if not zero
//
// Added intermittent mixing check, ensure that logic is correct.LSIACTIVE
// Check for mixer off "CheckForMixerOff & SetMixerTimer. Set Mixer Timer is to
// allow a delay before reading the motor status.
// g_nNoLevelSensorAlarmCounter set -> no level sensor alarm.
// Remove g_nPauseOnActiveCounter check in CheckPauseOnInput.
// Reset target alarm when the Pause has been released, reset g_bofflinebatchfilled
// Implement CheckFor60KgMixerOn.
//
//
// P.Smith                          18-08-2005
// correct if statement if(bOk = TRUE) ->     if(bOk == TRUE)
//
// P.Smith                          5/9/05
// Correct read of level sensor delay in calibration
//
// P.Smith                          7/9/05
// Correct pause read & tare alarm moving on.
//
// P.Smith                          7/10/05
// read of pot control implmentation.
//
// P.Smith                          11/10/05
// call SwitchComponentsOnOrOff
// call CheckForMaxExtruderPercentage();    // asm = CMXDA1
//
// P.Smith                          24/10/05
// Remove clear of dmpsec, if offline_Tracking not equal to BATSEQ_OFFDELADMPID
// set mode to offline tracking.
//
//
// P.Smith                          3/11/05
// correct close dump valve call, ensure that the dump valve open/close checks if
// the diagnostics output is enabled.
//
// P.Smith                      9/2/06
// first pass at netburner hardware conversion.
// Bool -> BOOL
// #include <basictypes.h>
// remove //nbb #include "Hardware.h"
// removed all unused header files for now.
// comment out SetMixerTime();
// comment out PutAlarmTable( LEVELSENSORALARM,  0 );      // indicate alarm, Component no. is zero. //PTALTB
// comment out ToManual();                         // TOMANUAL no more Auto control.
// comment out ShiftTimeCheck(); //--REVIEW-- Should be called in foreground and max once per second  JSR     SHIFTTIMECHK            ; CHECK FOR SHIFT TIME ?
// comment out StartCleanCycle();          // Initiate clean cycle
// comment out EndOfCyclePauseOn();            //  END OF CYCLE PAUSE ON.
// comment out RemoveAlarmTable( ATTARWTALARM,  0 );     // remove alarm,  //RMALTB
// comment out PauseOff();
// comment out CheckForMaxExtruderPercentage();    // asm = CMXDA1
// comment out CheckDToALimit();           // check d/a limit for max
// comment out CheckForMixerOff(); // CHECKFORMIXEROFF
// add back in #include    "Pause.h"
// add back in PauseOff();
//
//
// P.Smith                      21/2/06
// add in to IncreaseOn(), DecreaseOn()
// CheckDToALimit();           // check d/a limit for max
// CheckForMaxExtruderPercentage();
// #include "Conalg.h"
//
// P.Smith                      28/2/06
// SetMixerTime();
// #include "CycleMonitor.h"
// CheckForMixerOff(); // CHECKFORMIXEROFF
// #include "FgOnehz.h"
// PutAlarmTable( LEVELSENSORALARM,  0 );      // indicate alarm, Component no. is zero. //PTALTB
// ToManual();                         // TOMANUAL no more Auto control.
// #include "Alarms.h"
//
// P.Smith                      19/4/06
// StartCleanCycle(); called
// EndOfCyclePauseOn(); called
// RemoveAlarmTable( ATTARWTALARM,  0 ); called
//
// P.Smith                      1/6/06
// slight change to printf on 50hz
//
// P.Smith                      14/7/06
// if feedclose clear call openfeed, if set call closefeed.        ;
// the blender does not switch to manual on a level sensor alarm   ;
// instead in the control algorithm, the averaging is reset and    ;
// no control action is taken.                                     ;
// removed dooropen flag.                                          ;
// remove level monitoring check in the software                   ;
// correct intermittent mixing, allow for all types except micra   ;
// call StartCleanCycle();          // Initiate clean cycle
//
// P.Smith                      16/11/06
// correct problem with level sensor alarm, the alarm should only  ;
// be activated when the level sensor becomes uncovered in the     ;
// filling cycle, previous to this the alarm was being generated   ;
// during any other state rather than level polling.               ;
// use g_bCycleIndicate as Bool
//
// P.Smith                                                 22/6/07
// name change to proper blender names.
//
// M.McKiernan                      17/9/07
// Removed g_arrnMBTable[] & g_arrnMBTableFlags[], now in BatVars2
//
// P.Smith                          13/11/07
// Modified control mode format implemented
//
// P.Smith                          5/12/07
// decrement holdofflsensoralctr if non zero.                      ;
// no level sensor alarm if holdofflsensoralctr,cleancycleseq set  ;
// No level sensor alarm unless g_nHoldOffLevelSensorAlarmSecondCounter
// and g_nCleanCycleSeq is 0
//
// P.Smith                          10/1/08
// update autocycler to simulate the level sensor becoming         ;
// uncovered for 2 seconds. during the autocycle period, the level ;
// sensor is not checked.
// correct autocycler, even if autocycler is enabled, then ack     ;
// level sensor being off.                                         ;
//
// P.Smith                          11/2/08
// name change CheckDToALimit to CheckDToALimit1
//
// P.Smith                          25/2/08
// only run IcreaseOn, DecreaseOn when in manual
//
// P.Smith                          11/2/08
// put in check for download manager being non zero, if it is non zero for
// 10 seconds, it is set to zero again.
//
// P.Smith                          1/7/08
// Remove micrabatch related stuff
//
// P.Smith                          17/7/08
// AddBlenderToManualPotControlEventToLog added
//
// P.Smith                          24/6/08
// remove g_arrnMBTable and modbus arrays
//
// P.Smith                          16/10/08
// removed todo
//
// P.Smith                          4/12/08
// increment level sensor ack counter, level sensor
//
// P.Smith                          15/1/09
// removed commented out section in ReadProcessMainLevelSensor
//
// P.Smith                          7/1/09
// use screw detection input as the high level sensor input, there is a conflict
// with the cleaning high input.
//
// P.Smith                          7/9/09
// a problem has been seen where the blender will not cycle at full speed.
// there was a delay of a few seconds at the end of the cycle.
// this is due to the level sensor lag being set continuously eventhough the
// level sensor status has been acknowledged.
// this is fixed by only setting the level sensor lag when the sensor flag
// g_bDmpLevSen has been set
//
// P.Smith                          12/11/09
// removed mixing screw detection
// only restart the mixing motor if the blender is not paused.
//
// P.Smith                          7/7/10
// remove SwitchComponentsOnOrOff and writes to dump and offline valve
///////////////////////////////////////////////////////

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
#include "16R40C.h"
#include    "FiftyHz.h"
#include    "BatVars.h"
#include "CycleMonitor.h"
#include    "Pause.h"
#include "Alarms.h"
#include "Batvars2.h"
#include "Conalg.h"
#include "TSMPeripheralsMBIndices.h"
#include "FgOnehz.h"
#include "TSMPeripheralsMBIndices.h"
#include "Eventlog.h"

#include <basictypes.h>


// Locally declared global variables
// Externally declared global variables
// Data
extern CalDataStruct    g_CalibrationData;
//extern float  g_fWeightAtoDCounts;

extern  int g_nKeyCode;
extern  int g_nLanguage;
extern  int g_nGeneralSecondTimer;
extern  structMeasurementUnits  g_MeasurementUnits;

//bb
extern  int g_nDisplayScreen;

//////////////////////////////////////////////////////
// ReadProcessMainLevelSensor( void )           from ASM = HGREAD, from LS2NOA to HGLOL1
// Reads the level sensor input, and processes it.
// Called on 50Hz PIT interrupt.
//
//
// M.McKiernan                          25-06-2004
// First pass.
//////////////////////////////////////////////////////
void ReadProcessMainLevelSensor( void )
{

BOOL bState,bNoLevelActivation;
    bState = ReadLSensor();                     // raw signal state
    bNoLevelActivation = FALSE;
    BOOL bAutoCycleDumpActive = FALSE;
    if(g_bAutoCycleFlag && (g_nAutocycleLevelSensorOff10hzCounter != 0))
    {
        bAutoCycleDumpActive = TRUE;
    }

    if((bState == COVERED) && !bAutoCycleDumpActive)
    {
        g_nShutDownCtr = 0;                     // reset counter        //SHUTDOWNCTR
        g_cLS1OffCt = 0;                            // LEVEL SENSOR ON  // LS1OFFCT
        g_bLevSen1Stat = COVERED;                                           // LEVSEN1STAT

        g_nLevSenOffTime = 0;                                               // LEVSENOFFTIME
    }
    else    // uncovered
    {
        bAutoCycleDumpActive = FALSE;
        if(g_nLevSenOffTime == 0) // only do this the first time detected.
        {
            g_sOngoingHistoryEventsLogData.m_nLevelSensorSwitchesCtr++;
        }

        g_nLevSenOffTime++;                                             //LEVSENOFFTIME
        if(g_nLevSenOffTime > LEVEL_SENSOR_OFF_TIME)                //LEVELSENSOROFFTIME
        {
            g_nLevSenOffTime = 0;
            g_bLevSen1Stat = UNCOVERED;
            g_nNoCycle = 0;
            if(g_nHGSen1Lag == 0)                                                       //HGSEN1LAG
            {
                 if(g_CalibrationData.m_nIntermittentMixTime != 99 )
                 {

                 if(g_bMixOff)                 // mixer off
                 {
                    SetMixerTime();
                    if(g_nMixerOnTime <= 5)
                    {
                        g_bDmpLevSen = FALSE;
                        bNoLevelActivation = TRUE;
                    }
                 }
             }
                if(!bNoLevelActivation)
                {
                 //jsr LSISACTIVE

//                   if(g_nPauseFlag == 0 && g_cBatSeq == BATSEQ_LEVPOLL)     // Paused or in level sensor polling mode
                   if(g_nPauseFlag == 0)
                    {
                        g_bDmpLevSen = TRUE;
                        g_nHGSen1Lag = LEVSEN1LAG;
                    }                //
                    else
                    if(g_CalibrationData.m_nBlenderMode == MODE_OFFLINEMODE)
                    {    // SETOFF
                        g_bOffLSF = TRUE;          // OFFLINE LEVEL SENSOR FLAG.       // OFFLSF
                        g_nHGSen1Lag = LEVSEN1LAG;
                    }

                CheckIfPremLS();                // CHECK IF LEVEL SENSOR IS PREMATURE.
     //           g_nHGSen1Lag = LEVSEN1LAG;
                }

            }
            else
            {
            //    if(fdTelnet >0)
            //    iprintf("\n  g_nHGSen1Lag is  %d",g_nHGSen1Lag);
            }

         }
    }
}



//////////////////////////////////////////////////////
// CheckIfPremLS( void )            from ASM = CHECKIFPREMLS
// Check if level sensor uncovering is premature.
// If it deems it to be premature, it sets the level sensor alarm
// if not already set, and switches blender out of AUTO control.
//
//
// M.McKiernan                          25-06-2004
// First pass.
//////////////////////////////////////////////////////
void CheckIfPremLS( void)
{
BOOL bOk = FALSE;

    if(g_bFirstWCycle)
        bOk = TRUE;
    else if(g_cBatSeq == BATSEQ_LEVPOLL)
        bOk = TRUE;
    else if(g_bPauseTransition)
        bOk = TRUE;
    else if(g_CalibrationData.m_nBlenderMode == MODE_OFFLINEMODE)
        bOk = TRUE;
    else if(g_nCleanCycleSeq)
        bOk = TRUE;

    if(bOk == TRUE)
    {   // YESISOK
        g_nPremLSCtr  = 0;                                          //PREMLSCTR       ; RESET.
    }
    else
    { // LSALARM                                                                                      ASM = NOLSALARMCTR




        if((g_CalibrationData.m_nBlenderMode != MODE_THROUGHPUTMON) && !g_bLevelSensorAlarmOccurred && (g_nNoLevelSensorAlarmCounter == 0) && (g_CalibrationData.m_nBlenderMode != MODE_SINGLERECIPEMODE) && g_bCycleIndicate )       //LEVELSENALOC YES, NO ALARM HERE
        {

// --review-- check that single recipe mode check here is correct, not assemble software allow the level sensor alarm flag to be set

            g_bLevelSensorAlarmOccurred = TRUE;
            if( (g_arrnMBTable[BATCH_SUMMARY_PROCESS_ALARMS] & LEVELSENALBIT) == 0 )
            {
                if(g_CalibrationData.m_nLevelSensorDelay == 0)
                {
                    if((g_nHoldOffLevelSensorAlarmSecondCounter == 0) && (g_nCleanCycleSeq == 0))
                    {
                        PutAlarmTable( LEVELSENSORALARM,  0 );      // indicate alarm, Component no. is zero. //PTALTB
                    }
                 }
                else
                {
                    if( g_nLevelSensorDelayCtr == 0)
                    {
                        g_nLevelSensorDelayCtr = g_CalibrationData.m_nLevelSensorDelay;
                    }

                }

            }
                //ALSET
            if(g_bManualAutoFlag == AUTO)       // no increase or decrease if in auto.
            {
                g_nEstCtrUse = NO_OF_ESTIMATED_CYCLES;                              //ESTCTRUSE = NOESTIMATEDCYCLES  ; USE ESTIMATION FOR A FEW CYCLES.
            }
            g_nAfterOpenCounter = 0;                    // RESET COUNTER AFTEROPEN       ; RESET COUNTER
            g_nPremLSCtr  = 0;                          //PREMLSCTR       ; RESET.
        }
    }

}

//////////////////////////////////////////////////////
// ReadHighLowLevelSensors( void )          from ASM = READHIGHLOWSENSORS
// READs and debounces HIGH AND LOW LEVEL SENSORS.
// Called on 50Hz PIT interrupt.
//
// M.McKiernan                          09-09-2004
// First pass.
//////////////////////////////////////////////////////
void ReadHighLowLevelSensors( void)
{
BOOL bState;

// read and debounce high level sensor.
    bState = ReadHighLevelSensor(); // returns hi sensor state.
    if(bState == COVERED)
    {
        g_nHighLSUnCoveredCounter = 0;
        g_nHighLSCoveredCounter++;
        if(g_nHighLSCoveredCounter > HILOFILTER50HZ)
        {
            g_bHighLevelSensor = COVERED;       // Flag sensor covered.
            g_nHighLSCoveredCounter = 0;        // reset counter.
        }
    }
    else // uncovered
    {
        g_nHighLSCoveredCounter = 0;
        g_nHighLSUnCoveredCounter++;
        if(g_nHighLSUnCoveredCounter > HILOFILTER50HZ)
        {
            g_bHighLevelSensor = UNCOVERED;     // Flag sensor Uncovered.
            g_nHighLSUnCoveredCounter = 0;      // reset counter.
        }
    }

// read and debounce low level sensor.
    bState = ReadLowLevelSensor();  // returns low sensor state.
    if(bState == COVERED)
    {
        g_nLowLSUnCoveredCounter = 0;   // clear the uncovered count
        g_nLowLSCoveredCounter++;
        if(g_nLowLSCoveredCounter > HILOFILTER50HZ)
        {
            g_bLowLevelSensor = COVERED;        // Flag sensor covered.
            g_nLowLSCoveredCounter = 0;     // reset counter.
        }
    }
    else // uncovered
    {
        g_nLowLSCoveredCounter = 0; // clear the covered count
        g_nLowLSUnCoveredCounter++;
        if(g_nLowLSUnCoveredCounter > HILOFILTER50HZ)
        {
            g_bLowLevelSensor = UNCOVERED;      // Flag sensor Uncovered.
            g_nLowLSUnCoveredCounter = 0;       // reset counter.
        }
    }
    // read IP6 for high regrind input

    bState = ReadScrewDetection();  // returns low sensor state.
    if(bState == COVERED)
    {
        g_nHiRegLSUnCoveredCounter = 0;   // clear the uncovered count
        g_nHiRegLSCoveredCounter++;
        if(g_nHiRegLSCoveredCounter > HILOFILTER50HZ)
        {
            g_bHiRegLevelSensor = COVERED;        // Flag sensor covered.
            g_nHiRegLSCoveredCounter = 0;     // reset counter.
        }
    }
    else // uncovered
    {
        g_nHiRegLSCoveredCounter = 0; // clear the covered count
        g_nHiRegLSUnCoveredCounter++;
        if(g_nHiRegLSUnCoveredCounter > HILOFILTER50HZ)
        {
            g_bHiRegLevelSensor = UNCOVERED;      // Flag sensor Uncovered.
            g_nHiRegLSUnCoveredCounter = 0;       // reset counter.
        }
    }



}

/////////////////////////////////////////////////////
// FiftyHzHandler()                     from ASM = PITCALL
//
// Call all programs that need to run on the 50hz
// NB: This is called on PIT interrupt - need to keep efficient.
//
// Michael McKiernan                        19.11.2004
// First Pass.
//////////////////////////////////////////////////////
void FiftyHzHandler( void )
{

BOOL bState;

    if(g_arrnMBTable[BATCH_SETPOINT_DOWNLOAD_MANAGER] != 0)
    {
        g_nDownloadManagerNZTimeoutCtr++;
        if(g_nDownloadManagerNZTimeoutCtr > DOWNLOADMANAGERTIMEOUT50HZ)
        {
            g_nDownloadManagerNZTimeoutCtr = 0;
            g_arrnMBTable[BATCH_SETPOINT_DOWNLOAD_MANAGER] = 0;  // set back to 0
        }

    }
    else
    {
        g_nDownloadManagerNZTimeoutCtr = 0;
    }


//nbb--todo--review     ReadMixerScrewRotationDetector(); // READMIXSCREWROTATIONDETECTOR
    CheckPauseOnInput();                    // READPAUSEONINPUT
//nbb--width--todo--put back    SwitchComponentsOnOrOff();        // switch component on or off.  --review-- would be better to call from the foreground

 // SETDUMPOUTPUT is merged inline
//    if(g_bDumpStatus)
//    {
//        OpenDump();
//    }
//    else
//    {
//        CloseDump();
//    }

//    if(g_bFeedValveClosed)
//    {
//        CloseFeed();
//    }
//    else
//    {
//        OpenFeed();
//    }

//--  Flash rate handled seperately - by setting g_nFlashRate in functions     JSR     SETLEDFLASHRATE
// HGREAD
//--break up HGREAD into constituent parts        JSR     HGREAD             ;~READ I/PS ON "HG"
        ReadHighLowLevelSensors();  // Read Hi and Lo level sensors.
        bState = ReadContactorInput();
        if(bState == STOP)
        {
            g_nMixerTrippedCounter++;
            if(g_nMixerTrippedCounter > 50)     // this is 50Hz so 1 second debounce on input.
            {
                g_bMixerTripped = TRUE; // LEVSEN2STAT
                g_nMixerTrippedCounter = 50;        // limit to 50.
            }
        }
        else // contactor (trip) is OK.
        {
            g_bMixerTripped = FALSE;
            g_nMixerTrippedCounter = 0;
        }

        ReadProcessMainLevelSensor();

//      check for pot control /* incorporates from HGLOL1 - STMIX from HGREAD */
        if(g_CalibrationData.m_nPeripheralCardsPresent & SEIENBITPOS)   /* sei enabled */
        {
            /*  asm READPOTCONTROL  */
            if(g_nSEIInputStates & SEI_INPUT_POT_CONTROL_BIT)
            {
                g_bPotControl  = TRUE;
                ToManual();
                AddBlenderToManualPotControlEventToLog();
            }
            else
            {
                g_bPotControl = FALSE;
            }
        }

        /* asm = STMIX:  */

        if(g_bLevSen1Stat == COVERED)         // is now covered
        {
            if((g_bPrevLevSen1Stat == UNCOVERED)&&(g_nPauseFlag ==0))    // uncovered to covered
            {
                    CheckForMixerOff(); // CHECKFORMIXEROFF
                    g_bMixOff = FALSE;  // clear flag to allow mixer to run.
                    SetMixerTime();  //.
            }
        }
        else  // uncovered
        {
            if((g_bPrevLevSen1Stat == COVERED)&&(g_nPauseFlag ==0))    // covered to uncovered
            {
                    CheckForMixerOff(); // ASM CHECKFORMIXEROFF
                    g_bMixOff = FALSE;  // clear flag to allow mixer to run.
            }
        }

        g_bPrevLevSen1Stat = g_bLevSen1Stat;  // PREVLS1STAT = LEVSEN1STAT  (history)

// end of HGREAD.

// --review-- unused anymore      PollRemoteModules();    JSR     POLLREM                 ; POLL SERIAL DATA.
    PollIncreaseDecrease();  // renamed  JSR     POLLREMEI               ; POLL REMOTE EI.
// PITCNTR not used anywhere        INC     PITCNTR
// PITCOUNT not used anywhere        INCW    PITCOUNT                ;16 BIT VALUE

//nbb--todolp-- shift.cpp    ShiftTimeCheck(); //--REVIEW-- Should be called in foreground and max once per second  JSR     SHIFTTIMECHK            ; CHECK FOR SHIFT TIME ?

    if(!g_bCycleIndicate)
    {
        if(g_bSetCleanWhenCycleEnd)
        {
            g_bSetCleanWhenCycleEnd = FALSE; // reset flag
            StartCleanCycle();          // Initiate clean cycle
        }

    }
// asm = NOSET:
    if(g_bSecondRollover)       //SecondRollover set by RTC interrupt..
    {
        g_bSecondRollover = FALSE;
        g_bForegroundSecondRollover = TRUE; // FGSecondRollover is sec rollover flag for foreground routines
            //Calls ForegroundOneHzProgram() in ProgLoop.c. Function located in FGOneHz.c.

    }

    if(g_nMixerTransitionCounter)
    {
        g_nMixerTransitionCounter--; // decrement counter
    }
}

/////////////////////////////////////////////////////
// CheckPauseOnInput()                      from ASM = READPAUSEONINPUT
// Note, the name was altered to distinguish from the hardware read i.e. ReadPauseInput.
// Reads the "pause on" input.
// NB: This is called on PIT (50Hz) interrupt - need to keep efficient.
//
// Michael McKiernan                        19.11.2004
// First Pass.
//////////////////////////////////////////////////////
void CheckPauseOnInput( void )
{
BOOL bPauseState;

    bPauseState = ReadPauseInput();

    if(bPauseState)
    {
        g_nPauseOnInactiveCounter = 0;
        g_nPauseOnActiveCounter++;
        if( g_nPauseOnActiveCounter > PAUSE_INPUT_TRIGGER )
        {
            g_nPauseOnActiveCounter = 0;
            if(!g_bPauseOnActive)
            {
                g_bPauseOnActive = TRUE;    // set "pause on" flag
                EndOfCyclePauseOn();            //  END OF CYCLE PAUSE ON.
            }
        }
    }
    else  // pause input not active // NOPNAC
    {
        g_nPauseOnActiveCounter = 0;
        g_nPauseOnInactiveCounter++;
        if( g_nPauseOnInactiveCounter > PAUSE_INPUT_TRIGGER )
        {
            g_nPauseOnInactiveCounter = 0;
            if(g_bPauseOnActive)    // was pause active previously.
            {
                g_bPauseOnActive = FALSE;
                g_bOfflineBatchFilled = FALSE;

                if( g_arrnMBTable[BATCH_SUMMARY_PROCESS_ALARMS] & ATTARWTALARMBIT) // at target alarm bit not set???
                {
                    RemoveAlarmTable( ATTARWTALARM,  0 );     // remove alarm,  //RMALTB
                }

                PauseOff();
                /* note only valid for Rotamould application where bin is removed  */

                if(g_nOffline_Tracking != BATSEQ_OFFDELADMPID && g_CalibrationData.m_nOfflineType == OFFLINETYPE_XXKG)
                {
                    g_nOffline_Tracking = BATSEQ_OFFDELADMPID;   // OFFLINE_TRK = OFFDELADMPID
                    g_nOffTime1 = 0;    // OFF LINE COUNTER RESET
                }

            }

        }

    }
}


/////////////////////////////////////////////////////
// PollIncreaseDecrease()                      from ASM = POLLREMEI
// Checks if increase / decrease d/a should be changed
//
// P.Smith                              10.02.2005
// First Pass.
//////////////////////////////////////////////////////


void PollIncreaseDecrease( void )
{
// Note check for Auto / manual lamp is no longer required
    if(g_nCommsIncreaseFlag)
    {
        IncreaseOn();// increase handler
        g_nCommsIncreaseFlag--;
    }
    else
    if(g_nCommsDecreaseFlag)
    {
        DecreaseOn();// decrease key on
        g_nCommsDecreaseFlag--;
    }

}



/////////////////////////////////////////////////////
// DecreaseOn()                      from ASM = DKEYON
//
// P.Smith                              10.02.2005
// First Pass.
//////////////////////////////////////////////////////


void DecreaseOn( void )
{
    g_nIgnoreLSTime = 2;    // level sensor monitor alarm is now disabled for two cycles
    if(((g_CalibrationData.m_nControlMode & CONTROL_MODE_VOLTAGE_FOLLOWER) == 0)&& (g_bManualAutoFlag == MANUAL))
    {
        g_bRunEst = TRUE;  // estimation of kg/h and grams per meter
        g_bScrewIncreaseDeceasePressed = TRUE; // indicate that increase has taken place
        g_nEstimationContinueCounter = NUMBEROFESTIMATIONS;  // used only in increase / decrease mode
        g_nExtruderStalledCounter = 0;  // set extruder stalled counter to 0
        g_nToStallTime = EXTRUDERSTALLINHIBITTIME;      //15;        // TOSTALTIM
        // note MCHGFLG no longer used
        if( g_CalibrationData.m_nControlType != CONTROL_TYPE_INC_DEC)
        {
            g_bCCVsFlag = TRUE;     //--review-- this should be a BOOLean
            if(g_nExtruderDAValue > DTOADECREMENTVALUE)
            {
                g_nTempExtruderDAValue = g_nExtruderDAValue - DTOADECREMENTVALUE;
                CheckForMaxExtruderPercentage();    // asm = CMXDA1
                g_nExtruderDAValue = g_nTempExtruderDAValue;
//                Note : UPDK1FLG currently unused
            }


        }

    }


}



/////////////////////////////////////////////////////
// IncreaseOn()                      from ASM = IKEYON
//
// P.Smith                              10.02.2005
// First Pass.
//////////////////////////////////////////////////////


void IncreaseOn( void )  //nbb--todo-- look at taking this out.
{
    g_nIgnoreLSTime = 2;    // level sensor monitor alarm is now disabled for two cycles
    if(((g_CalibrationData.m_nControlMode & CONTROL_MODE_VOLTAGE_FOLLOWER) == 0) && (g_bManualAutoFlag == MANUAL))
    {
        g_bRunEst = TRUE;  // estimation of kg/h and grams per meter
        g_bScrewIncreaseDeceasePressed = TRUE; // indicate that increase has taken place
        g_nEstimationContinueCounter = NUMBEROFESTIMATIONS;  // used only in increase / decrease mode
        g_nExtruderStalledCounter = 0;  // set extruder stalled counter to 0
        g_nToStallTime = EXTRUDERSTALLINHIBITTIME;        // TOSTALTIM
        // note MCHGFLG no longer used
        if( g_CalibrationData.m_nControlType != CONTROL_TYPE_INC_DEC)
        {
            g_bCCVsFlag = TRUE;     //--review-- this should be a BOOLean
            g_nTempExtruderDAValue = g_nExtruderDAValue + DTOAINCREMENTVALUE;
//nbb--testonly--            iprintf("\n d/a value is %d ",g_nTempExtruderDAValue);
//          Note : UPDK1FLG currently unused
            CheckDToALimit1();           // check d/a limit for max
            CheckForMaxExtruderPercentage();
            g_nExtruderDAValue = g_nTempExtruderDAValue;
            g_arrnWriteSEIMBTable[MB_SEI_DA] = g_nExtruderDAValue;
        }
    }
}




/////////////////////////////////////////////////////
// ReadMixerScrewRotationDetector()    from ASM = READMIXSCREWROTATIONDETECTOR
// Detects mixing screw
//
//
// P.Smith                              29.03.2005
//////////////////////////////////////////////////////


void ReadMixerScrewRotationDetector ( void )
{
BOOL bState;
    if(g_bMixOff)  // mixer off
    {
        g_nMixScrewPulseCounter = 0;  // reset pulse counter if off
    }
    if(g_CalibrationData.m_bMixerScrewDetect)  // is the option enabled ?
    {
        bState = ReadScrewDetection();
        if(bState != g_bMixerSensorHistory)       // has the state changed ?
        {
            g_bMixerSensorHistory = bState;        // store historic value
            g_nMixScrewPulseCounter++;              // increment pulse counter
        }
    }
}



/*
; INPUT IS INACTIVE
NOPNAC  CLR     PONACTIVECTR
        INC     POFFACTIVECTR
        LDAA    POFFACTIVECTR
        CMPA    #PONACTIVENO            ; AT PRESET
        BLO     XITPCH                  ; NOT INACTIVE FOR PRESET TIME.
        CLR     POFFACTIVECTR             ;
        TST     PAUSEONACTIVE           ; WAS PAUSE ON ACTIVE PREIOUSLY
        BEQ     XITPCH                  ; NO
        JSR     PAUSEOFF
        CLRW    DMPSEC          ; RESET 1/10 SECOND COUNTER.
        LDAA     #OFFDELADMPID   ; SEQUENCE AFTER DUMP.
        STAA     OFFLINE_TRK
        CLR     OFFTIM1         ; OFF LINE COUNTER RESET

        CLR     PAUSEONACTIVE           ; PAUSE ON FLAG ACTIVE
XITPCH  RTS

;*****************************************************************************;
; FUNCTION NAME : READPAUSEONINPUT                                            ;                                      ;
; FUNCTION      : CHECKS FOR PAUSE INPUT DETECTION.                           ;
; INPUTS        : CALLED ON THE 50HZ                                          ;                 ;                       ;
;*****************************************************************************;


READPAUSEONINPUT:
        JSR     READPAUSEINPUT
        BEQ     NOPNAC                   ; PAUSE ON NOT ACTIVE

; INPUT IS ACTIVE

        CLR     POFFACTIVECTR
        INC     PONACTIVECTR             ; PAUSE OFF ACTIVE
        LDAA    PONACTIVECTR
        CMPA    #PONACTIVENO             ; AT PRESET
        BLO     XITPCH                   ; PAUSE ON NOT ACTIVE.
        CLR     PONACTIVECTR             ;
        TST     PAUSEONACTIVE            ; EXIT IF ALREADY ACTIVE
        BNE     XITPCH
        LDAA    #1
        STAA    PAUSEONACTIVE           ; PAUSE ON FLAG ACTIVE
        JSR     ENDOFCYCLEPAUSEON        ; END OF CYCLE PAUSE ON.
        BRA     XITPCH

; INPUT IS INACTIVE
NOPNAC  CLR     PONACTIVECTR
        INC     POFFACTIVECTR
        LDAA    POFFACTIVECTR
        CMPA    #PONACTIVENO            ; AT PRESET
        BLO     XITPCH                  ; NOT INACTIVE FOR PRESET TIME.
        CLR     POFFACTIVECTR             ;
        TST     PAUSEONACTIVE           ; WAS PAUSE ON ACTIVE PREIOUSLY
        BEQ     XITPCH                  ; NO
        JSR     PAUSEOFF
        CLRW    DMPSEC          ; RESET 1/10 SECOND COUNTER.
        LDAA     #OFFDELADMPID   ; SEQUENCE AFTER DUMP.
        STAA     OFFLINE_TRK
        CLR     OFFTIM1         ; OFF LINE COUNTER RESET

        CLR     PAUSEONACTIVE           ; PAUSE ON FLAG ACTIVE
XITPCH  RTS


*/
/*
;*****************************************************************************;
; FUNCTION NAME : PITCALL                                                     ;                                      ;
; FUNCTION      : CALLED ON THE 50HZ                                        ;
; INPUTS        : NONE                                       ;                 ;                       ;
;*****************************************************************************;


;;P  PITINTH:                             ;PERIODIC INT HANDLER
PITCALL:
; CHECK FOR CHANGE IN CAL CONST
;
        JSR     READMIXSCREWROTATIONDETECTOR
        JSR     READPAUSEONINPUT
        JSR     SETDUMPOUTPUT
        JSR     SETLEDFLASHRATE
        JSR     HGREAD             ;~READ I/PS ON "HG"
        JSR     POLLREM                 ; POLL SERIAL DATA.
        JSR     POLLREMEI               ; POLL REMOTE EI.
        INC     PITCNTR
        INCW    PITCOUNT                ;16 BIT VALUE
        JSR     SHIFTTIMECHK            ; CHECK FOR SHIFT TIME ?
        TST     CYCLEINDICATE           ; IN CYCLE
        BNE     NOSET                   ; YES
        TST     SETCLEANWHENCYCLEEND
        BEQ     NOSET                   ; NO
        CLR     SETCLEANWHENCYCLEEND
        JSR     STARTCLEANCYCLE
NOSET:


        TST     SECROVER                ;HAS SECOND TURNED OVER
        LBEQ     PITINT1
        CLR     SECROVER


; CHECK IF THE EXPAN PCB SHOULD BE USED FOR THE CLEANING

        LDAA    NOBLNDS
        CMPA    #FOUR
        BHI     ENEXPCB
        CLR     EXTERNALCLEANPCB
        BRA     CONS1
ENEXPCB LDAA    #1
        STAA    EXTERNALCLEANPCB
CONS1:  TST     LEVELSENSORDELAYCTR
        BEQ     NOLSD                           ; NO DECREMENT REQUIRED
        DEC     LEVELSENSORDELAYCTR
        TST     LEVELSENSORDELAYCTR
        BNE     NOLSD
        LDAA    #$0F
        STAA    DMPLEVSEN                       ; SET "DUMP" LEVEL SENSOR FLAG

NOLSD:
        JSR     CHECKFORDIAGNOSTICSPASSWORD

;       CHECK FOR LEVEL SENSOR MONITORING ALARM

        TST     LEVELMONITOR
        BEQ     NOLSALRM                       ; EXIT LEVEL SENSOR WATCH DOG
        TST     IGNORELSTIME
        BNE     NOLSALRM                       ; NO CHECK FOR ALARM
        TSTW    BATCHMONCTR
        BEQ     NOLSALRM
        DECW    BATCHMONCTR
        BNE     NOLSALRM
        LDAA     MBPROALRM               ; CHECK EXISTING ALARM.
        ANDA     #LSMONITORALARMBIT           ;
        BNE      NOLSALRM                ; ALARM ALREADY SET.
        LDAA    #LSMONITORALARM
        JSR     PTALTB



NOLSALRM TST     CHECKLSENSORINNSECONDS
        BEQ     NOUPR                   ; NO UPDATE REQUIRED
        DEC     CHECKLSENSORINNSECONDS
        TST     CHECKLSENSORINNSECONDS
        BNE     NOUPR                   ; HAS IT JUST GONE TO ZERO
        LDAA    #1
        STAA    CHECKFORMAXTHP          ; RUN D/A PER KG CALC
NOUPR   JSR     CHECKFORBYPASSACTIVATION
        LDAA    PRINTINNSECONDS
        BEQ     NOPR1                   ; NO PRINTING.
        DEC     PRINTINNSECONDS         ; DECREMENT
        BNE     NOPR1
        LDAA    #1
        STAA    PRINTNOW
NOPR1   LDAA    CONTROLTYPE
        CMPA    #INCREASEDECREASE
        BNE     NOIDRD
        TST     DOREADINNSECONDS
        BEQ     NOIDRD                  ; NO INCREASE DECREASE READ.
        DEC     DOREADINNSECONDS
        BNE     NOIDRD
        LDAA    #1
        STAA    READINCDECCAL
NOIDRD  TST     CHKIFCYCLEFLAG          ; CHECK IF THE BLENDER SHOULD CYCLE.
        BEQ     NOCYC                   ; NO CHECK OF CYCLE.
        INC     CHKIFCYCLECTR
        LDAA    CHKIFCYCLECTR           ;
        CMPA    #SECTOCYCLE             ; NO OF SECONDS TO CYCLE.
        BLO     NOCYC                   ; NO CYCLE CHECK.
        TST     STOPPED                 ;
        BNE     NOCYCRS                 ; SENSOR OFF SYSTEM WILL CYCLE ANYWAY
        TST     LEVSEN1STAT
        BNE     NOCYCRS                 ; SENSOR OFF SYSTEM WILL CYCLE ANYWAY
        TST     HOPPERWEIGHT
        BNE     NOCYCRS                 ; SENSOR OFF SYSTEM WILL CYCLE ANYWAY
        LDD     HOPPERWEIGHT+1
        CPD     #ZEROWEIGHT
        BHI     NOCYCRS                 ; NO CYCLE RESET.
        LDAA    #$0F
        STAA    DMPLEVSEN               ; MAKE THE BLENDER CYCLE.
NOCYCRS CLR     CHKIFCYCLEFLAG
        CLR     CHKIFCYCLECTR
NOCYC   JSR     CHECKFORRRWEIGHTDISPLAY ; SET OR CLEAR FLAG TO ALLOW DISPLAY OF RR DATA


        INC     RXCOMMSWATCHDOG
        LDAA    RXCOMMSWATCHDOG
        CMPA    #2                      ; 3 SECONDS.
        BLS     NOLEDCH                 ; NO LED CHANGE.
        CLR     FLASHRATE               ; RESET FLASH COUNTER TO NORMAL.
        CLR     RXCOMMSWATCHDOG
NOLEDCH:
        CLRW    DEVICETOPOLLCTR         ; RESET DEVICE TO POLL CTR.
        JSR     CHKHILOSENSORS          ; CEHCK FOR HIGH LOW SENSORS.
        JSR     CHKHILOREGSENSORS
        TST     UPDATEREGHILOF          ; UPDATE REGHILO
        BEQ     NOREGU                  ; NO
        JSR     UPDATEREGHILO         ; NO CHECK FOR CYCLE.
        CLR     UPDATEREGHILOF          ; RESET FLAG.
NOREGU  JSR     CHKFORSEITO             ; SEI TIMEOUT
        LDAA    #1
        STAA    SEIPOLLINPUTSF          ; START POLLING OF INPUTS.
        STAA    WRITEDAVALUEF           ; INITIATE D/A WRITE EVERY SECOND.
        STAA    SEIPOLLLLSF             ; SET LOW LEVEL SENSOR
        STAA    WRITEALARMSF
        LDAB    CRLMODE                 ; BLENDER MODE.
        CMPB    #VOLTAGEFOLLOWER        ; VOLTAGE FOLLOWER MODE.
        BNE     NOVF                    ; NO.
        TST     MANATO                  ; AUTO MODE
        BNE     NOVF                    ; NO READ IF IN AUTO.
        STAA    READDAVALUEF
NOVF:
        LDAB    #9
        STAB    TENCNT                  ;SET 10HZ COUNTER TO GO TO 0
        CLR     PITCNTR                 ;PITCNTR RESET ON RTC SECOND
        JSR     STHCAD0                ;START CONVERSION SEQ FOR HC16 AD CH. 0
        BRA     NOADREAD               ;;P PITINT2
PITINT1:
        TST     HOURFG                  ; TIME FOR HOURLY COUNTER ?
        BEQ     NOTHR                   ; NO.
        JSR     CHECKFORTEMPOPTION      ; CHECK FOR TEMPORARY OPTION

        CLR     HOURFG                  ; RESET FLAG.
NOTHR   TST     HCADFIN                 ;32 CONV. SEQ FINISHED??
        BNE     NOADREAD                ;P PITINT2                 ;SKIP AHEAD IF FINISHED
        JSR     GTHC16AD                ;LOAD UP A/D RESULTS
        JSR     SMHC16AD                ;SUMMATE VALUES
        TST     HCADFIN                 ;IS SEQ. FINISHED??
        BNE     NOADREAD                ;PITINT2
        LDAB    #0                      ;A/D CHANNEL 0
        JSR     CNVHC16                 ;INITIATE CONVERSION ON HC16 A/D
NOADREAD:
        RTS
*/

/*

NTLSEC  TST    AMDBNC            ;~AUTO/MANUAL DEBOUNCE TIMER
        BEQ    NODEBNC
        DEC    AMDBNC
NODEBNC:

        RTS

;*****************************************************************************;
; FUNCTION NAME : HGREAD                                                      ;
; FUNCTION      : READS LEVEL SENSOR STATUS                                   ;
;                 IF SENSOR ACTIVE FOR "LEVELSENSOROFFTIME" AND NOT PAUSED    ;
;                 THEN "DMPLEVSEN" FLAG IS SET.                               ;
;                 CHECKS FOR PREMATURE LEVEL SENSOR UNCOVERING.               ;
;
;*****************************************************************************;


HGREAD  JSR     READHIGHLOWSENSORS      ; READ HIGH AND LOW SENSORS
;        JSR     READOI1CONN61 ; OPTICALLY COUPLED INPUT..
;        BNE     BBOFF
;        LDAA    #$0F
;        STAA    BUBBRK          ; BUBBLE BREAK INPUT ACTIVE
;        BRA     CONHG1
;BBOFF   CLR     BUBBRK

CONHG1  CLR     LEVSEN2STAT
        JSR     READCIN
        BNE     LS2NOA                          ; LEVEL SENSOR NOT ACTIVE.
        LDAA    #$0F                            ;
        STAA    LEVSEN2STAT

LS2NOA  JSR     READLSENSOR                     ; LSENSOR IN BLENDER.
        BEQ     HGHIL2                         ;~CHANGE OCCURRING
        CLRW     SHUTDOWNCTR                     ; RESET COUNTER.
        CLR     LS1OFFCT                        ; LEVEL SENSOR ON.
        CLR     LEVSEN1STAT
        CLRW    LEVSENOFFTIME
        BRA     HGLOL1
HGHIL2  INCW    LEVSENOFFTIME                   ; INCREMENT COUNTER.
        LDE     LEVSENOFFTIME

        LDAA    BLENDERMODE     ; CHECK MODE
        CMPA    #OFFLINEMODE    ; OFFLINE MODE
        BNE     NORMLC                          ; NORMAL CHECK.
        LDAA    BLENDERTYPE                     ;
        CMPA    #TINYTYPE
        BEQ     LESSTIM
NORMLC  CPE     #LEVELSENSOROFFTIME             ; OFFLINE TIME FOR MICRA IS SHORTER
        BRA     DOLSCK                          ; CHECK.
LESSTIM CPE     #LEVELSENSOROFFTIMEOFFLINE      ; COMPARE TO VALID OFF PERIOD.
DOLSCK  BLS     HGLOL1                          ;
        CLRW    LEVSENOFFTIME
        LDAA    #$0F
        STAA    LEVSEN1STAT                    ; LEVEL SENSOR STATUS
        CLR     NOCYCLE

        TST     LSDELAY                        ; CHECK IF LS DELAY REQUIRED
        BEQ     NRMAC                          ; NORMAL ACTIVATION
        TST     LEVELSENSORDELAYCTR
        BNE     HGLOL1
        LDAB    LSDELAY                         ; READ LEVEL SENSOR DELAY
        JSR     BCDHEX1                 ; BCD TO HEX.
        STAB    LEVELSENSORDELAYCTR
        BRA     HGLOL1

NRMAC   TST      HGSEN1LAG
        BNE      HGLOL1                         ;
        JSR     LSISACTIVE


HGLOL1
; BITS 1-6 OF HG PORT USED AS INDICATION OF POTENTIOMETER CONTROL
; L.S. BIT IS FOR CH. 1
HG_PBITS:                       ;SET POT CONTROL BITS
        CLR     CH1POTBIT
        JSR     READPOTCONTROL  ; READ POT CONTROL INPUT.
        BEQ     NOPOTCT
        BSET    CH1POTBIT,#1     ;SET L.S.BIT OF POT CONTROL BYTE
        PSHA
        JSR     TOMANUAL        ; SWITCH TO MANUAL.
        PULA
NOPOTCT CMPA    POTBITH                         ;SAME AS PREV. VALUE
        BEQ     ST_PBIT
        INC     POTCFLG         ;SET FLAG TO SHOW CHANGE
ST_PBIT STAA    POTBITH                         ;AND STORE NEW VALUE
        LDAA    LEVSEN1STAT     ;
        BNE     LS1OFF
        LDAA    PREVLS1STAT     ; HISTORY.
        BNE     STMIX           ; ON TO OFF.
        BRA     XITMIX          ; EXIT MIXER CHECK.


LS1OFF  LDAA    PREVLS1STAT
        BEQ     STMIX
XITMIX  LDAA    LEVSEN1STAT
        STAA    PREVLS1STAT                     ; HISTORY
        RTS

STMIX:
       LDAA     BLENDERTYPE
       CMPA     #TINYTYPE       ; TINY TYPE.
       BEQ      ISTNY3          ; IS TINY
       CLR     MIXOFF
ISTNY3 BRA    XITMIX
        RTS


NTLSEC  TST    AMDBNC            ;~AUTO/MANUAL DEBOUNCE TIMER
        BEQ    NODEBNC
        DEC    AMDBNC
NODEBNC:

        RTS

*/


/*
; READ OF HIGH AND LOW LEVEL SENSORS.
;
READHIGHLOWSENSORS:
        JSR     READHIGHLEVELSENSOR     ; HIGH LEVEL SENSOR.
        BNE     HIGHISON                ; ON.
        CLR     HIGHONCTR               ; RESET ON COUNTER
        INC     HIGHOFFCTR              ; OFF COUNTER INCREMENTED.
        LDAA    HIGHOFFCTR
        CMPA    #HILOFILTER50HZ         ; HIGH LOW TIME ON 50 HZ.
        BLS     CHKLOW                  ; CHECK LOW.
        CLR     HIGHOFFCTR              ; RESET
        CLR     HIGHLEVELSENSOR
        BRA     CHKLOW                  ; CHECK LOW SENSOR.
HIGHISON:
        CLR     HIGHOFFCTR
        INC     HIGHONCTR               ; INCREMENT HIGH ON CTR.
        LDAA    HIGHONCTR
        CMPA    #HILOFILTER50HZ         ; CHECKING FILTER.
        BLS     CHKLOW
        CLR     HIGHONCTR               ; RESET
        LDAA    #01
        STAA    HIGHLEVELSENSOR         ; HIGH LEVEL SENSOR.
CHKLOW  JSR     READLOWLEVELSENSOR      ; LOW LEVEL SENSOR.
        BNE     LOWISON                 ; ON.
        CLR     LOWONCTR                ; RESET ON COUNTER
        INC     LOWOFFCTR               ; OFF COUNTER INCREMENTED.
        LDAA    LOWOFFCTR
        CMPA    #HILOFILTER50HZ         ; HIGH LOW TIME ON 50 HZ.
        BLS     XITLSC                  ; EXIT LEVEL SENSOR CHECK.
        CLR     LOWOFFCTR               ; OFF COUNTER INCREMENTED.
        CLR     LOWLEVELSENSOR
        BRA     XITLSC
LOWISON:
        CLR     LOWOFFCTR
        INC     LOWONCTR               ; INCREMENT HIGH ON CTR.
        LDAA    LOWONCTR
        CMPA    #HILOFILTER50HZ         ; CHECKING FILTER.
        BLS     XITLSC
        LDAA    #01
        STAA    LOWLEVELSENSOR          ; HIGH LEVEL SENSOR.
        CLR     LOWONCTR                ; OFF COUNTER INCREMENTED.
XITLSC  RTS                             ; EXIT.


*/

/*

LSISACTIVE:
        TST     PAUSFLG
        BNE     SETOFF                   ; IN PAUSE MODE.
        LDAA    BATSEQ
        CMPA    #LEVPOLL                ; LEVEL SENSOR POLLING MODE.
        BNE     SETOFF                  ; NO
        LDAB    #$0F
        STAB    DMPLEVSEN               ; DUMP LEVEL SENSOR ACTIVATED.
        BRA     NOSETOFFLAG
SETOFF  LDAB    #$0F
        STAA    OFFLSF                  ; OFFLINE LEVEL SENSOR FLAG.

NOSETOFFLAG:
        JSR     CHECKIFPREMLS           ; CHECK IF LEVEL SENSOR IS PREMATURE.
        LDAB    #LEVSEN1LAG             ;40
        STAB    HGSEN1LAG
        STAA    HILSNSR                         ;~ACCEPT NEW SENSOR VALUE
UNCOVERED:
        LDAA     #$AA
        STAA    HILFLAG                         ;~SET FLAG FOR CHANGE
        RTS


NRMAC   TST      HGSEN1LAG
        BNE      HGLOL1                         ;
        JSR     LSISACTIVE



        CLRW    LEVSENOFFTIME
        LDAA    #$0F
        STAA    LEVSEN1STAT                    ; LEVEL SENSOR STATUS
        CLR     NOCYCLE

        TST     LSDELAY                        ; CHECK IF LS DELAY REQUIRED
        BEQ     NRMAC                          ; NORMAL ACTIVATION
        TST     LEVELSENSORDELAYCTR
        BNE     HGLOL1
        LDAB    LSDELAY                         ; READ LEVEL SENSOR DELAY
        JSR     BCDHEX1                 ; BCD TO HEX.
        STAB    LEVELSENSORDELAYCTR
        BRA     HGLOL1

NRMAC   TST      HGSEN1LAG
        BNE      HGLOL1                         ;
        JSR     LSISACTIVE

*/



/*
        LDAA    MBPROALRM       ; CHECK ALARM WORD.
        ANDA    #LEVELSENALBIT  ; LEVEL SENSOR ALARM BIT
        BNE     ALSET           ; ALARM HAD BEING CLEARED.

CHECKIFPREMLS:
        TST     FIRSTWCYCLE     ; 1ST CYCLE
        BNE     YESISOK
        LDAA    BATSEQ          ; CHECK SEQUENCER
        CMPA    #LEVPOLL        ; IS THE SYSTEM WAITING FOR LEVEL SENSOR ?
        BEQ     YESISOK
        TST     PAUSETRAN       ; SHOULD THE ALARM BE CHECKED YET ?
        BNE     YESISOK
        LDAA    BLENDERMODE        ;
        CMPA    #OFFLINEMODE    ; OFFLINE
        BEQ     YESISOK         ;OFFLINE

LSALARM:
        LDAA    CLEANCYCLESEQ
        BNE     YESISOK         ;OFFLINE

;        LDAA    OXXLINE
;        CMPA    #TMONID         ; THROUGHPUT MONITOR OPTION
        LDAA    BLENDERMODE        ;
        CMPA    #THROUGHPUTMON  ; OFFLINE
        BEQ     XITPLS          ; YES, NO ALARM HERE
        TST     LEVELSENALOC    ; LEVEL SENSOR ALARM
        BNE     XITPLS          ; ALREADY SIGNALLED.
        LDAA    #1
        STAA    LEVELSENALOC    ; INDICATE ALARM OCCURRED.
        LDAA    MBPROALRM       ; CHECK ALARM WORD.
        ANDA    #LEVELSENALBIT  ; LEVEL SENSOR ALARM BIT
        BNE     ALSET           ; ALARM HAD BEING CLEARED.
        LDAA    #LEVELSENSORALARM
        JSR     PTALTB          ; TO ALARM TABLE
        LDAA    #NOESTIMATEDCYCLES
        STAA    ESTCTRUSE       ; USE ESTIMATION FOR A FEW CYCLES.
ALSET   JSR     TOMANUAL        ; NO MORE AUTO CONTROL.
        LDAA    #$0F
        STAA    DOOROPEN        ; SET "DOOROPEN" FLAG.
        CLR     AFTEROPEN       ; RESET COUNTER
YESISOK CLR     PREMLSCTR       ; RESET.
XITPLS  RTS




;*****************************************************************************;
; FUNCTION NAME : READMIXSCREWROTATIONDETECTOR                                ;                                      ;
; FUNCTION      : CHECKS FOR SCREW DETECTION.                                 ;
; INPUTS        : CALLED ON THE 50HZ                                       ;                 ;                       ;
;*****************************************************************************;


READMIXSCREWROTATIONDETECTOR:
        TST      MIXOFF                  ; IS THE MIXER OFF
        BEQ      MNOTOFF                 ; NO
        CLRW     MIXSCREWPULSECTR        ; MOTOR IS OFF
MNOTOFF TST     MIXSCREWDETECTION       ; IS THE OPTION ENABLED ?
        BEQ     XITSCD                  ; NO EXIT DETECTION
        JSR     READSCREWDETECTION      ; READ SCREW DETECTION
        TSTA                            ; CHECK RESULT
        CMPA    MIXSENSORHISTORY        ; WAS THE LAST LEVEL THE SAME ?
        BEQ     XITSCD                  ; NOT DETECTED
        STAA    MIXSENSORHISTORY        ; SET NEW STATUS
        INCW    MIXSCREWPULSECTR        ; INCREMENT PULSE COUNTER
XITSCD  RTS


;*****************************************************************************;
; FUNCTION NAME : ANALYSEROTDETECTOR                                          ;                                      ;
; FUNCTION      : ANALYSE ROTATION DETECTOR                                   ;
; INPUTS        : CALLED EVERY 10 SECONDS                                     ;                 ;                       ;
;*****************************************************************************;

ANALYSEROTDETECTOR:
       LDD      MIXSCREWPULSECTR        ; READ MIXING SCREW PULSE COUNTER
       CPD      #MINROTATIONCOUNT       ; MINIMUM ROTATION COUNT IN 10 SECONDS
       BHI      SCROT                   ; SCREW IS ROTATING
       LDAA     #1
       STAA     MIXSCREWROTALARM        ; MIXING SCREW ROTATION ALARM SET
       BRA      XITSCA                  ; EXIT ANALYSIS
SCROT  CLR      MIXSCREWROTALARM        ; SCREW ROTATING OKAY
XITSCA CLRW     MIXSCREWPULSECTR        ;
       RTS

MCE12-15

;*****************************************************************************;
; FUNCTION NAME : PITCALL                                                     ;                                      ;
; FUNCTION      : CALLED ON THE 50HZ                                        ;
; INPUTS        : NONE                                       ;                 ;                       ;
;*****************************************************************************;


;;P  PITINTH:                             ;PERIODIC INT HANDLER
PITCALL:
; CHECK FOR CHANGE IN CAL CONST
;
        JSR     READMIXSCREWROTATIONDETECTOR
        JSR     READPAUSEONINPUT
        JSR     SETDUMPOUTPUT
        JSR     SETLEDFLASHRATE
        JSR     HGREAD             ;~READ I/PS ON "HG"
        JSR     POLLREM                 ; POLL SERIAL DATA.
        JSR     POLLREMEI               ; POLL REMOTE EI.
        INC     PITCNTR
        INCW    PITCOUNT                ;16 BIT VALUE
        JSR     SHIFTTIMECHK            ; CHECK FOR SHIFT TIME ?
        TST     CYCLEINDICATE           ; IN CYCLE
        BNE     NOSET                   ; YES
        TST     SETCLEANWHENCYCLEEND
        BEQ     NOSET                   ; NO
        CLR     SETCLEANWHENCYCLEEND
        JSR     STARTCLEANCYCLE
NOSET:
        TST     MIXERTRANSITIONCTR
        BEQ     MIXCT0                 ; EXIT IF 0
        DEC     MIXERTRANSITIONCTR


*/

