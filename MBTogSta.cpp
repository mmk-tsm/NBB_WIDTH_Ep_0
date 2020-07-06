// **********************************************************
//   MBTogSta.c                                                 *
//    Modbus toggle status programs for Batch blender           *
//
//   Latest Changes:                05.Nov.2004             *
//                                                              *
//   M.McKiernan                   05-11-2004                   *
//      First Pass
//
//  M.McKiernan                         13-12-2004
//  Activated InitialiseAlarmTable, RemoveAlarmTable() and PutAlarmTable.
//
//  P.Smith                         4-3-2005
//  Remove comments from ToAuto / ToManual.

//
// P.Smith                          8-04-2005
// uncomment ClearCommsCounters() FrontRollChange() BackRollChange()
//
// P.Smith                          4-08-2005
// call CopyOrderReportData in toggle status message.
//
// P.Smith                          23-08-2005
// added CheckEncryptionEntry in toggle status commmands.
// when clearing after toggle status commands clear to END_TOGGLE_STATUS_COMMANDS
//
// P.Smith                          23/9/2005
// correct license encryption write
//
// P.Smith                              4/10/05
// Rename low priority todos
//
// P.Smith                              17/10/05
// make g_arrnMBTable unsigned int
//
// P.Smith                              7/11/05
// CheckForValveDiagnostics(nOutput) added
//
// P.Smith                              18/11/05
// clear nTotal for 100% check.
// call InitiateRecipeTransferToCurrentOrder on modbus single word write mode
//
// P.Smith                              25/11/05
// nTemp,nMask defined as unsigned ints
//
// P.Smith                      20/2/06
// first pass at netburner hardware conversion.
// BOOL -> BOOL
// included  <basictypes.h>
// comment out todo header files
// removed duplicate extern in extern   int g_arrnMBTable[];
// cleanio.cpp --todos-- implemented
// CheckForValveDiagnostics,
// #include "Cleanio.h"
// implement pitweigh.cpp todos, #include "PITWeigh.h"
//
// P.Smith                      21/2/06
// implement rwsei.cpp todos added FrontRollChange(); BackRollChange()
// #include "Rwsei.h"
//
// P.Smith                      22/2/06
// call to InitialiseAlarmTable();     // asm = INALTB
// #include "Alarms.h"
// changed unions IntsAndLong to WordAndDWord
// changed unions CharsAndInt to CharsAndWord
// ToManual(); & ToAuto(); implemented in both places called.
// #include "Conalg.h"
//
// P.Smith                      28/2/06
// RemoveAlarmTable( PERALARM,  0 );       // indicate % alarm cleared, Component no = 0 i.e. not component alarm.
// PutAlarmTable( PERALARM,  0 );      // indicate % alarm, Component no = 0 i.e. not component alarm.

// P.Smith                      19/4/06
// InitiateCleanCycle(); called
// #include "CycleMonitor.h"
//
// P.Smith                              21/4/06
// use BATCH_START_TOGGLE_STATUS_COMMANDS when clearing toggle commands
//
// P.Smith                              19/5/06
// put back in increase /decrease from modbus table.
// no increase /decrease if blender in auto, also correct check for less than 0, this
// needed to be an unsigned no.
//
// P.Smith                              19/5/06
// SwitchPauseOffAndMakeCycle();   instead of pauseoff(); to ensure that the blender
// cycles when unpaused from the comms.
//
// P.Smith                              31/5/06
// CopyOrderReportData(); called    // ASM = ASSORPT ASSEMBLE ORDER REPORT .
// add toggle status reset order totals commanad.
//
//
// P.Smith                              15/6/06
// check BATCH_GENERAL_TOGGLE_STATUS_COMMANDS_SEND_ORDER_REPORT_EMAIL_BIT
// for config and order change email sending
//
// P.Smith                              28/6/06
// call CheckEncryptionEntry(uValue.lValue); // ASM = ENTERENCRYPTIONDATA check entered encryption value.
//
// P.Smith                              9/8/06
// added  ZeroLongTermWeightAccumulator
//
// P.Smith                              15/11/06
// allow download of line speed over the comms in imperial format  ;
// xxx.yy feet / minute.                                           ;
// command does not clear the toggle status area unless it is a line speed download.
//
// P.Smith                              16/11/06
// g_bModbusDToAWritten checked to determine if d/a should be written.
// remove necessity to set bit on d/a download                     ;
// dac is downloaded as 0-100.0%
// clear BATCH_START_TOGGLE_STATUS_COMMANDS2 area if required when download has been initiated.
//
// P.Smith                            11/1/07
// name change g_CalibrationData.m_nLineSpeedSignalType
//
// P.Smith                            5/2/07
// Remove unused printfs
//
// P.Smith                            13/12/06
// Do not allow increase /decrease to operate in Auto
//
// P.Smith                            6/3/07
// added BATCH_RESET_FLOW_RATES command
//
// P.Smith                              13/4/07
// added g_bUpdateSDCardSoftware set to TRUE to trigger software update from sd card
// There was a problem with this software crashing
//
// P.Smith                              24/4/07
// added Multiblend recipe download, extruder setup, clear download history,Pause multiblend and Restart Multiblend
//
// P.Smith                              24/5/07
// add g_CalibrationData.m_nResetCounter reset to 0
// download of nbb ip setting added, ensure that the toggles are cleared after they
// are acknowledged.
//
// P.Smith                              12/6/07
// added clearing of optimisation counters when operator panel command is initiated
//
// P.Smith                              21/6/07
// set g_CalibrationData.m_nCycleCounter to 0 on reset of counters
// set g_bSaveAllCalibrationToEEpromOutsideCycle to save calibration data
// reset m_lGoodTx, .m_lGoodRx , m_lChecksumErrorCounter for network comms
//
// P.Smith                              25/6/07
// added sd card related commands, software update, copy config to backup,
// load default config,load backup config, copy config to sd card
//
// P.Smith                              6/7/07
// added length and purge commands, copy length to g_fOptimisationOrderLength
//
// P.Smith                              19/7/07
// added purge command
//
// P.Smith                              2/8/07
// added  vibrate catch box at 316
//
// M.McKiernan                      17/9/07
// Removed g_arrnMBTable[] & g_arrnMBTableFlags[], now in BatVars2
//
// P.Smith                      15/11/07
// Remove check for 8000 on screw speed download.
// merge LoadExtruderRPMFromMB inline
// merge  LoadExtruderRPMFromMB
// Added screw speed tare, screw calibrate
//
// P.Smith                      23/11/07
// check g_bModbusScrewSpeedWritten to determine if extruder speed should be
// written over the comms.
// the original code had caused a problem in that when the clean was initiated
// from the panel, the clean part of the toggle status command set left bCommandClear
// true, the rpm command set bCommandClear to false,stopping the clear of the clean
// command word. This meant that when the next toggle status command was inititated
// say a write of the length to the modbus table, the clean was initiated again.
// this is what Stephen seen in KWH.
//
// P.Smith                      7/12/07
// set g_IncreaseOutputOn10hzCounter if increase is activated over modbus.
// set g_DecreaseOutputOn10hzCounter if decrease is activated over modbus
// this will switch the sei outputs on when operated from the panel.
//
// P.Smith                      28/1/08
// added ResetSampleCounter to toggle status commands,this resets the sample counter to 0.
// (BATCH_TOGGLE_STATUS_RESET_SAMPLE_COUNTER)
// **********************************************************
//
// P.Smith                          30/1/08
// correct compiler warnings
//
// P.Smith                          4/2/08
// set g_IncreaseOutputOn10hzCounter to INCREASEOUTPUTONTIME10HZ and
// g_DecreaseOutputOn10hzCounter to DECREASEOUTPUTONTIME10HZ
//
// P.Smith                          10/2/08
// if increase / decrease mode, only activate the output on increase /decrease
// do not activate the d/a value.
//
// P.Smith                          11/2/08
// for increase /decrease command, if not increase / decrease, then ensure that
// SEI output is turned back on again.
// name change CheckDToALimit to CheckDToALimit1
//
// P.Smith                          1/3/08
// comment out MBCheckIncreaseDecrease
//
//
// P.Smith                          26/3/08
// remove extern  int     g_nManagerPassword;
// extern  int     g_nOperatorPassword;
//
// P.Smith                          27/3/08
// use g_ProcessData structure for m_nResetCounter, set g_bSaveAllProcessDataToEEprom
//
// P.Smith                          23/4/08
// cycle counter renamed to g_nCycleCounter
//
// P.Smith                          6/6/08
// change 100% check nTotal > 9995 && nTotal < 10005
// name change to g_bLoadConfigFromSDCard
//
// P.Smith                          7/7/08
// add auto / manual to event log
//
// P.Smith                          8/7/08
// put back in MBCheckIncreaseDecrease, this was stopping the cp2000 from
// working the on the ramp page for a single blender.
//
// P.Smith                          17/7/08
// set  g_nSDCardDelayTime = SDCARDCOMMANDDELAYTIME for all sd card operations
// added TOGGLE_STATUS_COMMAND2_OPERATOR_PANEL_PAGE_ID check for panel screen
// change,this allows the config screens to be tracked by the logging to the sd
// card.
//
// P.Smith                          23/7/08
// remove g_arrnMBTable & g_CurrentTime extern
//
// P.Smith                          24/7/08
// check auto / manual to ensure that if the blenders gets a command to switch
// to auto that it does not try to switch to auto again.
// the same applies for switching to manual, check if already in manual already
// and do not switch to manual if already in manual
// check for TOGGLE_STATUS_COMMAND2_SAFELY_REMOVE_SD_CARD, set g_nDisableSDCardAccessCtr
// to SDCARDACCESSDISABLEINSECS
//
// P.Smith                          11/9/08
// added test to simulate a flow rate change
//
// P.Smith                          26/9/08
// added liquid additive write commands, d/a and %,  this only works if the blender is
// paused.
// added liquid additive command LiquidAdditiveCommand
//
// P.Smith                          1/10/08
// do not clear toggle status command for write d/a and write d/a %
// check g_nLiquidAdditiveOutputDAValue for max value.
//
// P.Smith                          16/10/08
// add ZeroCountersCommand to reset long term counters
//
// P.Smith                          13/11/08
// correct comparision for a/m was = should be ==
// clear tcp comms counter g_lTCPIPCommsCounter in clear comms counters
//
// P.Smith                          11/2/09
// reset usb timout counters in ClearCommscounters
//
// P.Smith                          5/3/09
// add blender test function TestFunction at TOGGLE_STATUS_TEST_FUNCTIONALITY
// clear g_ProcessData.m_n24HourOngoingResetCounter, set g_bAllowProcesDataSaveToEEPROM
// to true to save to eeprom.
//
// P.Smith                          26/3/09
// correct issue with screw speed tare, the value from the mb table was not being loaded
// into nTemp.
// put clearance of all counters in ClearCommsCounters, set g_bAllowProcesDataSaveToEEPROM
// to true to ensure that the data is stored immediately to eeprom.
// check for TOGGLE_STATUS_TEST_FUNCTIONALITY and call TestFunction if non zero
//
// P.Smith                          3/4/09
// if invalid regrind component no, set to to 0
//
// P.Smith                          11/5/09
// g_bNoLeakAlarmCheck is set to disable the leak alarm on the next cycle
//
// P.Smith                          1/9/09
// added InitiateCommand if modbus address TOGGLE_STATUS_COMMAND_INITIATION is non zero.
// InitiateCommand checks for lock / unlock commands initially.
//
// P.Smith                          10/9/09
// ClearDiagnosticData function now calls clearcommscounters
// there was a problem where the reset counter was getting cleared on reset
// because it was cleared in clearcommscounters.
//
// P.Smith                          29/9/09
// in ClearCommsCounters, clear vacuum loader counters.
//
// P.Smith                          12/11/09
// switch off mixing motor on the stop button and pause the blender.
// restart the motor when the play button is pressed.
//
// P.Smith                          9/3/10
// clear vac8 comms counter regardless of the number of vac 8 cards.
// reset g_nEIPCtr &  g_nEIPAssemblyObjectCtr to 0 in clear comms counter
// added BLENDER_WATCH_DOG_TIMER_ON_CODE to sw on watch dog timer
// added BLENDER_WATCH_DOG_TIMER_OFF_CODE to sw off watch dog timer
//
// P.Smith                          25/3/10
// allow ReadAndStoreSHAInFlash to be called comment out for now testonly
//
// P.Smith                          6/4/10
// change watch dog timer implementation to set a flag to signal
// that watch dog should be enabled or disabled.
//
// P.Smith                          8/6/10
// clear counters for ssif
// set g_bWidthLoadConfigFromSDCard to initiate load from sd card.
//
// M.McKiernan						13/5/2020
// set flag to update width data to SD card when update config to SD card pressed. - see wfh
// if( g_arrnMBTable[BATCH_SOFTWARE_COPY_CONFIG_TO_SD_CARD] )
//          g_bCopyConfigFileToSDCard = TRUE;
          //wfh
//          g_bCopyWidthConfigFileToSDCard = TRUE;
//*****************************************************************************
// INCLUDES
//*****************************************************************************
#include <stdlib.h>
#include <stdio.h>

#include "SerialStuff.h"
#include "BatchMBIndices.h"
#include "BatchCalibrationFunctions.h"
#include "General.h"
#include "ConversionFactors.h"

#include "16R40C.h"
#include "Foreground.h"
#include "ConfigurationFunctions.h"
#include "MBMHand.h"
#include "MBSHand.h"
#include "MBProgs.h"

#include "MBProgs.h"
#include "SetpointFormat.h"
#include "TimeDate.h"
#include "BatVars.h"
#include "CycleMonitor.h"
#include "MBTogSta.h"
#include "PITWeigh.h"
#include "InitialiseHardware.h"
#include "Pause.h"
#include "Alarms.h"
#include "Conalg.h"
#include "BatVars2.h"
#include "Rwsei.h"
#include "Blrep.h"
#include "License.h"
#include "Cleanio.h"
#include <basictypes.h>
#include "Email.h"
#include "AssReports.h"
#include "Acumwt.h"
#include "Lspeed.h"
#include "StoreCommsRecipe.h"
#include "MultiblendMB.h"
#include "Multiblend.h"
#include "NBBIp.h"
#include "Purge.h"
#include "Screw.h"
#include "BatchStatistics.h"
#include "Eventlog.h"
#include "Sdcard.h"
#include "LiquidAdditive.h"
#include "BlenderSelfTest.h"
#include "Vacvars.h"
#include "Watchdog.h"
#include "Security.h"
#include "WidthVars.h"
#include "WidthDef.h"
#include "WidthForeGround.h"
#include "WidthControlAlgorithm.h"



extern  CalDataStruct   g_CalibrationData;
extern  structSetpointData  g_CurrentRecipe;
extern  structSetpointData  g_TempRecipe;

extern  char    *RevisionNumber;


extern  structControllerData    structSSIFData;
extern  structControllerData    structUFP2Data;
extern  structControllerData    structSEIData;
extern  structControllerData    structLLSData;
extern  structControllerData    structOptimisationData;
extern  structCommsData         g_sModbusNetworkSlave;
extern  structControllerData    structVAC8IOData[];
extern bool g_bWatchdogStatus;
extern unsigned int	g_nEIPCtr;
extern unsigned int	g_nEIPAssemblyObjectCtr;
unsigned int g_nWatchDogCommand = 0;
unsigned int g_nWatchDogCommandInNseconds = 0;


//////////////////////////////////////////////////////
// MBToggleStatusCommands()         ASM = MBTOGSTA
//
// Toggle status commands from Modbus table
//
//
// M.McKiernan                          11-11-2004
// First Pass
//
//////////////////////////////////////////////////////
void MBToggleStatusCommands( void )
{
unsigned int nTemp,nMask;
int nDToATemp;
unsigned int i,j;
float   fValue,fTemp;

unsigned int nRegrindComp;
int nTotal,nOutput;
union WordAndDWord uValue;

BOOL bError = FALSE;
BOOL bCommandClear = TRUE;

    nTemp = g_arrnMBTable[BATCH_CONTROL_AUTO_MANUAL];
    if(nTemp & BATCH_TOGGLE_AM_MASK && (g_nAMToggleStatusDelayCtr == 0))    //toggle auto/manual.
    {
        g_nAMToggleStatusDelayCtr = HALFSECONDINPITS;
        //  AMKey();        // do same as key handler did.
        if(g_CalibrationData.m_nBlenderMode != MODE_OFFLINEMODE)
        {
            if(!g_nPauseFlag)   // blender not paused, dont toggle if paused.
            {
                if(g_bManualAutoFlag == AUTO)       // are we in auto??
                {
                    ToManual();
                    g_bToManualEvent = TRUE;
                    g_nSDCardDelayTime = SDCARDCOMMANDDELAYTIME;
                }
                else
                {      // currently in manual.
                    ToAuto();
                }
            }
        }
    }

    //MBTOG014
    if(nTemp & BATCH_CLOSE_OFFLINE_MASK)    // close offline valve?
    {
        if(g_nPauseFlag)    // blender paused.
            CloseFeed();        // close offline feed valve.
    }
    // these two commands(open/close) are mutually exclusive - give the close first priority.
    else if(nTemp & BATCH_OPEN_OFFLINE_MASK)
    {
        if(g_nPauseFlag)    // blender paused.
            OpenFeed();     // open offline feed valve.
    }

    //MBTOG012
    if(nTemp & BATCH_FRC_BIT_MASK)  // Front roll change via comms
    {
        FrontRollChange();      //
    }

    //MBTOG013
    if(nTemp & BATCH_BRC_BIT_MASK)  // Back roll change via comms
    {
        BackRollChange();       //
    }

    //NOOPDMP
    if(nTemp & BATCH_CLOSE_DUMP_MASK)   // close offline valve?
    {
        if(g_nPauseFlag)    // blender paused.
            CloseDump();        // close dump feed valve.
    }
    // these two commands(open/close) are mutually exclusive - give the close first priority.
    //NOCLOF
    else if(nTemp & BATCH_OPEN_DUMP_MASK)
    {
        if(g_nPauseFlag)    // blender paused.
        {
            OpenDump();     // open dump valve.
            g_bNoLeakAlarmCheck = TRUE;            // indicate no leak alarm check required
        }
    }

    if(nTemp & BATCH_TOGGLE_PAUSE_MASK) // Toggle pause?
    {
        PauseToggle();      // asm = PAUSETOGGLE
    }
    else if( nTemp & BATCH_EOC_PAUSE_MASK ) // endo of cycle pause??
    {
        EndOfCyclePauseOn();
    }
    else if( nTemp & BATCH_IMM_PAUSE_MASK ) //immediate pause??
    {

        g_bMixOff = TRUE;  //
        EndOfCyclePauseOn();
    }

    if(nTemp & BATCH_PAUSE_OFF_MASK)    // Pause off?
    {
//--testonly--        PauseOff();     // asm = PAUSEOFF
    	g_bMixOff = FALSE;
        SwitchPauseOffAndMakeCycle();  //sbb--todo--

    }

    if(nTemp & BATCH_AM_OFF_MASK)   // Auto off?
    {
       if(g_bManualAutoFlag == AUTO)
       {
           ToManual();     // asm = TOMANUAL
           g_bToManualEvent = TRUE;
           g_nSDCardDelayTime = SDCARDCOMMANDDELAYTIME;
       }
    }
// mutually exclusive commands, give "to Manual" priority.
    else if(nTemp & BATCH_AM_ON_MASK)   // Auto ON?
    {
       if(g_bManualAutoFlag == MANUAL)
       {
           ToAuto();       // asm = TOAUTO
       }
    }


//Check for increase/decrease command - checks g_arrnMBTable[BATCH_CONTROL_INC_DEC]
        MBCheckIncreaseDecrease();

// clear command
    nTemp = g_arrnMBTable[BATCH_CONTROL_CLEAR_COMMAND];

    if(nTemp & BATCH_CLEAR_ALARMS)  // Clear alarms bit set??
    {
         InitialiseAlarmTable();     // asm = INALTB
    }
// ASM = MBTG081
    if(nTemp & BATCH_CLEAR_KEY_PRESSED) // Clear enter key pressed bit
    {
        g_bEnterKeyPressedFlag = FALSE; //
        nMask = (BATCH_SETPOINTS_CHANGED_MASK | BATCH_CALIBRATION_CHANGED_MASK | BATCH_K_VALUES_CHANGED_MASK);
        nMask ^= 0xFFFF;    //complement
        g_arrnMBTable[BATCH_SUMMARY_NEW_SETPOINTS] &= nMask;    // clear new spt, calibration and k's bits

    }

// ASM = MBTG082
    if(nTemp & BATCH_CLEAR_FRONT_FLAG)  // CLEAR FRONT RC BIT.
    {
        g_arrnMBTable[BATCH_SUMMARY_NEW_SETPOINTS] &=(BATCH_FRONT_ROLL_MASK ^ 0xFFFF);  // clear new Front Roll bit
    }

// ASM = MBTG083
    if(nTemp & BATCH_CLEAR_BACK_FLAG)   // CLEAR Back RC BIT.
    {
        g_arrnMBTable[BATCH_SUMMARY_NEW_SETPOINTS] &=(BATCH_BACK_ROLL_MASK ^ 0xFFFF);   // clear new back Roll bit
    }

// ASM = MBTG084
    if(nTemp & BATCH_CLEAR_ORDER_FLAG)  // CLEAR Order Change BIT.
    {
        g_arrnMBTable[BATCH_SUMMARY_NEW_SETPOINTS] &= (BATCH_ORDER_MASK ^ 0xFFFF);  // clear new order change bit
    }

// ASM = MBTG085
    if(nTemp & BATCH_CLEAR_RESET_FLAG)  // CLEAR Reset Occurred bit.
    {
        g_bResetFlag = FALSE;   // clear reset flag. (asm comment - OPSTATUS)
    }

// ASM = MBTG086
    if(nTemp & BATCH_CLEAR_SHIFT_TOTALS)    // CLEAR shift accumulators
    {
        ClearShiftAccumulators();   // CLRSHFTACCS - CLEAR shift accumulators
    }
// ASM = MBTG087
    if(nTemp & BATCH_CLEAR_COMMS_TOTALS)    // CLEAR Comms counters
    {
        ClearDiagnosticData();   //
        //nbb--testonly--ReadAndStoreSHAInFlash();  //nbb--testonly--remove
    }

    if(nTemp & BATCH_CALIBRATION_ZERO_LONG_TERM_WEIGHT_ACCUMULATOR)
    {
        ZeroLongTermWeightResettableAccumulator();
    }


// --REVIEW-- BATCH_CLEAR_CLEAN_BITS - not used here in assembler, whhy?

// ASM = MBTG100
// Open command
    nTemp = g_arrnMBTable[BATCH_CONTROL_OPEN_COMMAND];  //MBOPENCMP - Open component
    if(nTemp)
    {
        //OPENTOGGLE
        if(g_nPauseFlag)        // must be paused,--REVIEW-- assembler was only checking for CYCLEPAUSETYPE
         {
            //CHKFOROPEN
            nMask = 0x0001;
            for(i=0; i < g_CalibrationData.m_nComponents; i++)
            {
                if(nTemp & nMask)
                {
                    CmpNOn( i+1, TRUE );        //MBCMPNON        ; SWITCH COMPONENT #N ON.
                }
                nMask <<= 1;    // shift mask bit 1 place to left.
            }
        }
    }

// ASM = MBTG121
// Close command
    nTemp = g_arrnMBTable[BATCH_CONTROL_CLOSE_COMMAND]; //MBCLOSECMP    - Close component
    if(nTemp)
    {
        //CLOSETOGGLE
//nbb        if(g_nPauseFlag)        // must be paused,--REVIEW-- assembler was only checking for CYCLEPAUSETYPE
        if(1)        // nbb--testonly-- no check for pause set.
        {
            //CHKFORCLOSE
            nMask = 0x0001;
            for(i=0; i < g_CalibrationData.m_nComponents; i++)
            {
                if(nTemp & nMask)
                {
                    CmpNOff( i+1, TRUE );       //MBCMPNOFF        ; SWITCH COMPONENT #N off.
                }
                nMask <<= 1;    // shift mask bit 1 place to left for next component.
            }
        }
    }

// ASM = MBTG140 - D/A value for extruder written down via Comms.
    nTemp = g_arrnMBTable[BATCH_CONTROL_WRITE_DAC]; //MBSETD_A  - Extruder d/a value

    if( (g_bManualAutoFlag == MANUAL) && (g_bModbusDToAWritten))
    {
        g_bModbusDToAWritten = FALSE;
        bCommandClear = FALSE;
        nTemp &= 0x7FFF;    // mask off "NEW_DAC_VALUE" bit.
        if(nTemp != g_nExtruderDAValue)
        {
            g_nExtruderDAValue = nTemp;
            g_nToStallTime = 15;        // TOSTALTIM
            g_nExtruderStalledCounter = 0;  // EXTSCTR
            g_bManualChangeFlag = TRUE;     // MCHGFLG
            g_bCCVsFlag = TRUE;                       // CCVSFLG
            g_bRunEst = TRUE;                       // RUNEST
        }
    }

    if(g_bModbusDToAPercentageWritten)
    {
        g_bModbusDToAPercentageWritten = FALSE;
        bCommandClear = FALSE;

        fValue = (float)g_arrnMBTable[BATCH_DTOA_PERCENTAGE] / 100.00f;
        if(fValue < 100.01)
        {
            fTemp = ((fValue / 100.00f) * MAXEXTRUDERDTOA)+ 0.50f;
            nTemp = (int)fTemp;
            if(nTemp != g_nExtruderDAValue)
            {
                g_nExtruderDAValue = nTemp;
                g_nToStallTime = 15;        // TOSTALTIM
                g_nExtruderStalledCounter = 0;  // EXTSCTR
                g_bManualChangeFlag = TRUE;     // MCHGFLG
                g_bCCVsFlag = TRUE;                       // CCVSFLG
                g_bRunEst = TRUE;                       // RUNEST
            }
        }
    }
    if((g_bManualAutoFlag == MANUAL) && (g_bModbusLiquidDToAWritten) && (g_nPauseFlag != 0))
     {
        bCommandClear = FALSE;
        g_bModbusLiquidDToAWritten = FALSE;
        g_bLiquidControlAllowed = FALSE;
        nTemp = g_arrnMBTable[TOGGLE_STATUS_COMMAND2_LIQUID_ADDITIVE_COMMAND_SET_DTOA];
        g_nLiquidAdditiveOutputDAValue = nTemp;
        if(g_nLiquidAdditiveOutputDAValue >= MAXEXTRUDERDTOA)
        {
            g_nLiquidAdditiveOutputDAValue = MAXEXTRUDERDTOA;
        }

    }
    if((g_bManualAutoFlag == MANUAL) && (g_bModbusLiquidDToAPercentageWritten) && (g_nPauseFlag != 0))
    {
        bCommandClear = FALSE;
        g_bModbusLiquidDToAPercentageWritten = FALSE;
        g_bLiquidControlAllowed = FALSE;
        fValue = (float)g_arrnMBTable[TOGGLE_STATUS_COMMAND2_LIQUID_ADDITIVE_COMMAND_SET_DTOA_PERCENTAGE] / 100.00f;
        if(fValue < 100.01)
        {
            fTemp = ((fValue / 100.00f) * MAXEXTRUDERDTOA)+ 0.50f;
            nTemp = (int)fTemp;
            if(nTemp != g_nLiquidAdditiveOutputDAValue)
            {
                g_nLiquidAdditiveOutputDAValue = nTemp;
                if(g_nLiquidAdditiveOutputDAValue >= MAXEXTRUDERDTOA)
                {
                    g_nLiquidAdditiveOutputDAValue = MAXEXTRUDERDTOA;
                }


            }
        }
    }


// ASM = MBTG150 - if single word writes enabled
    if( g_CalibrationData.m_bSingleWordWrite )
    {

        nTemp = g_arrnMBTable[BATCH_CONTROL_SETPOINT_FLAGS];    // New setpoint data written???
/*
#define BATCH_CONTROL_SETPOINT_FLAGS                277
// masks for the setpoint flags
#define BATCH_NEW_PERCENTAGES                   0x0001
#define BATCH_NEW_THROUGHPUTS                   0x0002
#define BATCH_NEW_DESCRIPTIONS              0x0004
#define BATCH_NEW_K_FACTORS                 0x0008

*/
        nMask = ( BATCH_NEW_PERCENTAGES | BATCH_NEW_THROUGHPUTS | BATCH_NEW_DESCRIPTIONS);
        if( nTemp & nMask )     // new %'s, tpt's, Order info bits
        {
            g_nLoadNewSetpointLag = 0;  // LDNSPLAG
            if((g_arrnMBTable[BATCH_SETPOINT_RESET_TOTALS] & MB_RESET_TOTALS_BIT) == 0 )
            {
                CopyOrderReportData();          // ASM = ASSORPT ASSEMBLE ORDER REPORT .
             }

// were %'s written?
            bError = FALSE;
            if( nTemp & BATCH_NEW_PERCENTAGES )
            {
//1
                // JSR CHMB100T
                nTemp = (g_arrnMBTable[BATCH_SETPOINT_REGRIND_COMPONENT] & 0x00FF);      // the regrind component no. is in the L.S.byte.
                if(nTemp <= g_CalibrationData.m_nComponents)
                {
                    nRegrindComp = nTemp;
                }
                else
                {
                    nRegrindComp = 0;
                }
                nTotal = 0;
                for(i = BATCH_SETPOINT_PERCENT_1, j=0; i <= BATCH_SETPOINT_ADDITIVE && j < g_CalibrationData.m_nComponents; i++, j++ )
                {
                    if( i != (nRegrindComp - 1))        // NB: i is 0 based, component no.s start at 1.  Dont sum the regrind component.
                    {
                        nTotal += g_arrnMBTable[i];
                    }

                }
                if(nTotal > 9995 && nTotal < 10005) // %'s to 2 decimal places, therefore 100% = 10,000. accept .01 error.
                {           //  i.e. total = 100%
                    // Remove % alarm from alarm table (if already there)
                    if( g_arrnMBTable[BATCH_SUMMARY_PROCESS_ALARMS] & PERALBIT ) // percentage alarm bit set???
                    {  //
                       RemoveAlarmTable( PERALARM,  0 );       // indicate % alarm cleared, Component no = 0 i.e. not component alarm.
                    }
                }
                else
                {
                    bError = TRUE;
                    // Reload %'s
                    CopyTargetPercentagesToMB();        // copy %'s back into MB table.
                    // Put % alarm in alarm table (if not already there)
                    if( (g_arrnMBTable[BATCH_SUMMARY_PROCESS_ALARMS] & PERALBIT) == 0 ) // percentage alarm bit not set???
                    {  //
                        PutAlarmTable( PERALARM,  0 );      // indicate % alarm, Component no = 0 i.e. not component alarm.
                    }
                }
//2
                if(!bError)
                {
                    LoadTargetPercentagesFromMB();                  // Load %'s from MB table.
                    InitiateRecipeTransferToCurrentOrder();
                }

            }
            if( nTemp & BATCH_NEW_THROUGHPUTS )
            {
                    LoadTargetThruputsFromMB();                     // load throughputs from MB table.
                    InitiateRecipeTransferToCurrentOrder();
            }
            if( nTemp & BATCH_NEW_DESCRIPTIONS )
            {
                    LoadOrderInfoFromMB();                      // load order info from MB table.
                    InitiateRecipeTransferToCurrentOrder();
            }
            g_bNewSetpointsFlag = TRUE;         // NEWSET

        }

    }


// asm = MBTG180

    // merge LoadExtruderRPMFromMB in line
    if(g_bModbusScrewSpeedWritten)
    {
        g_bModbusScrewSpeedWritten = FALSE;

        if(g_CalibrationData.m_nScewSpeedSignalType == SCEW_SPEED_SIGNAL_COMMS)
        {
            nTemp = g_arrnMBTable[BATCH_CONTROL_SCREW_SPEED];
            g_fExtruderRPM = (float)nTemp / 100.0f;     // screw rpm is to 2dp in MB.  (asm =SSRPM/SSRPM2D)
            bCommandClear = FALSE;
        }
    }

// asm = MBTG182
    if( g_arrnMBTable[BATCH_OUTPUT_DIAGNOSTICS] ) // MBPRDIAG - Production or output diagnostics
    {
        nOutput = g_arrnMBTable[BATCH_OUTPUT_DIAGNOSTICS];
        CheckForValveDiagnostics(nOutput);    // asm ACTIVATEPRODDIAGNOSTICS merged with CHECKFORVALVEDIAGNOSTICS
    }
// asm = NOPROD
    nTemp = g_arrnMBTable[BATCH_CONTROL_CLEAN]; // MBCLEAN - CLEAN COMMAND
    if( nTemp & MB_START_NORMAL_CLEAN_BIT )
    {
        InitiateCleanCycle();
    }

// asm = NONCL
    if(g_arrnMBTable[BATCH_ENCRYPTION_WRITE_CODE] || g_arrnMBTable[BATCH_ENCRYPTION_WRITE_CODE+1]) // only run if entry non zero.

    {
        uValue.nValue[0] = g_arrnMBTable[BATCH_ENCRYPTION_WRITE_CODE];
        uValue.nValue[1] = g_arrnMBTable[BATCH_ENCRYPTION_WRITE_CODE+1];

        CheckEncryptionEntry(uValue.lValue); // ASM = ENTERENCRYPTIONDATA check entered encryption value.
    }

// asm = NOENWR

    nTemp = g_arrnMBTable[BATCH_DTOA_INCREASE_COMMAND]; // asm = MBINCREASE INCREASE COMMAND  //
    if((nTemp != 0) && (g_bManualAutoFlag == MANUAL))       // are we in auto??
    {
       if(g_CalibrationData.m_nControlType == CONTROL_TYPE_INC_DEC) // asm = INCREASEDECREASE
       {
           g_IncreaseOutputOn10hzCounter = INCREASEOUTPUTONTIME10HZ;
       }
        else
       {
           g_IncreaseOutputOn10hzCounter = INCREASEOUTPUTONTIME10HZ;
           g_nTempExtruderDAValue = g_nExtruderDAValue + MODBUSINCREASEDECREASEDTOAINCREMENT;
           CheckDToALimit2();
           if(g_nTempExtruderDAValue != g_nExtruderDAValue)
           {
                g_nExtruderDAValue = g_nTempExtruderDAValue;
                g_nToStallTime = EXTRUDERSTALLINHIBITTIME;        // TOSTALTIM
                g_bCCVsFlag = TRUE;     //
                g_nExtruderStalledCounter = 0;      // EXTSCTR
                g_bRunEst = TRUE;
           }
       }
    }
    nTemp = g_arrnMBTable[BATCH_DTOA_DECREASE_COMMAND]; // asm = MBDECREASE DECREASE COMMAND  //
    if((nTemp != 0) && (g_bManualAutoFlag == MANUAL))
    {
       if(g_CalibrationData.m_nControlType == CONTROL_TYPE_INC_DEC) // asm = INCREASEDECREASE
       {
        g_DecreaseOutputOn10hzCounter = DECREASEOUTPUTONTIME10HZ;
       }
       else
       {
        g_DecreaseOutputOn10hzCounter = DECREASEOUTPUTONTIME10HZ;
        g_nTempExtruderDAValue = g_nExtruderDAValue - MODBUSINCREASEDECREASEDTOAINCREMENT;
        nDToATemp = g_nTempExtruderDAValue;  // cast from unsigned int to unsiged int.
        if(nDToATemp < 0)
        {
            g_nTempExtruderDAValue =0;
        }

        CheckDToALimit3();
        if(g_nTempExtruderDAValue != g_nExtruderDAValue)
        {
            g_nExtruderDAValue = g_nTempExtruderDAValue;
            g_nToStallTime = EXTRUDERSTALLINHIBITTIME;        // TOSTALTIM
            g_bCCVsFlag = TRUE;     //
            g_nExtruderStalledCounter = 0;      // EXTSCTR
            g_bRunEst = TRUE;
        }
       }
     }
     nTemp = g_arrnMBTable[BATCH_GENERAL_TOGGLE_STATUS_COMMANDS]; // asm = MBRESETTOTALS COMMAND  //
     if(nTemp & BATCH_GENERAL_TOGGLE_STATUS_COMMANDS_RESET_TOTALS_BIT)
     {
 //         printf("/n reset order totals"); //nbb-testonly--
          CopyOrderReportData();          // ASM = ASSORPTASSEMBLE ORDER REPORT if bit is 0.
     }

     if(nTemp & BATCH_GENERAL_TOGGLE_STATUS_COMMANDS_SEND_ORDER_REPORT_EMAIL_BIT)
     {
//          printf("/n sending test email");  //nbb-testonly--

          CopyOrderReportToBuffer();
     }

     if(nTemp & BATCH_GENERAL_TOGGLE_STATUS_COMMANDS_SEND_SYSTEM_CONFIG_EMAIL_BIT)
     {
//          printf("/n sending system config");  //nbb-testonly--

          CopySystemConfigToBuffer();
     }
     if( g_arrnMBTable[BATCH_RESET_FLOW_RATES] ) // asm MBRESETFLOWRATE
     {
          g_nResetAllFlowRates = FLOWRATERESETID;
    }

     if( g_arrnMBTable[BATCH_SOFTWARE_UPDATE_FROM_SD_CARD] ) //
     {
          g_nSDCardDelayTime = SDCARDCOMMANDDELAYTIME;
          g_bUpdateSDCardSoftware = TRUE;
          g_arrnMBTable[BATCH_SOFTWARE_UPDATE_FROM_SD_CARD] = 0;
     }

     if( g_arrnMBTable[BATCH_SOFTWARE_COPY_CONFIG_TO_SD_CARD] )
     {
          g_nSDCardDelayTime = SDCARDCOMMANDDELAYTIME;
          g_bCopyConfigFileToSDCard = TRUE;
          //wfh
          g_bCopyWidthConfigFileToSDCard = TRUE;
          g_arrnMBTable[BATCH_SOFTWARE_COPY_CONFIG_TO_SD_CARD] = 0;
     }
     if( g_arrnMBTable[BATCH_SOFTWARE_COPY_CONFIG_TO_BACKUP] )
     {
          g_nSDCardDelayTime = SDCARDCOMMANDDELAYTIME;
          g_bCopyConfigFileToBackupFile = TRUE;
          g_arrnMBTable[BATCH_SOFTWARE_COPY_CONFIG_TO_BACKUP] = 0;
     }

     if( g_arrnMBTable[BATCH_SOFTWARE_LOAD_DEFAULT_CONFIG_FROM_SD_CARD] )
     {
          g_nSDCardDelayTime = SDCARDCOMMANDDELAYTIME;
          g_bLoadConfigFromSDCard = TRUE;
          g_bWidthLoadConfigFromSDCard = TRUE;
          g_arrnMBTable[BATCH_SOFTWARE_LOAD_DEFAULT_CONFIG_FROM_SD_CARD] = 0;
     }

    if( g_arrnMBTable[BATCH_SOFTWARE_LOAD_BACKUP_CONFIG_FROM_SD_CARD] )
     {
          g_nSDCardDelayTime = SDCARDCOMMANDDELAYTIME;
          g_bLoadBackupConfigFromSDCard = TRUE;
          g_arrnMBTable[BATCH_SOFTWARE_LOAD_BACKUP_CONFIG_FROM_SD_CARD] = 0;
     }


     nTemp = (g_arrnMBTable[TOGGLE_STATUS_COMMAND2_WRITE_RECIPE_NO]);
     if(nTemp)
     {
          // g_bFlowRateChange = TRUE;
          StoreCommsRecipe();
          g_arrnMBTable[TOGGLE_STATUS_COMMAND2_WRITE_RECIPE_NO] = 0;
     }

     nTemp = (g_arrnMBTable[TOGGLE_STATUS_COMMAND2_WRITE_EXTRUDER_DATA]);
     if(nTemp)
     {
          //g_bFlowRateChange = FALSE;
          SaveMultiblendExtruderSetUp();
          g_arrnMBTable[TOGGLE_STATUS_COMMAND2_WRITE_EXTRUDER_DATA] = 0;
     }

     nTemp = (g_arrnMBTable[TOGGLE_STATUS_COMMAND2_CLEAR_DOWNLOAD_HISTORY]);
     if(nTemp)
     {
          ClearMultiblendDownloadHistory();
          g_arrnMBTable[TOGGLE_STATUS_COMMAND2_CLEAR_DOWNLOAD_HISTORY] = 0;
     }
     nTemp = (g_arrnMBTable[TOGGLE_STATUS_COMMAND2_PAUSE_MULTIBLEND]);
     if(nTemp)
     {
          PauseMultiblend();
          g_arrnMBTable[TOGGLE_STATUS_COMMAND2_PAUSE_MULTIBLEND] = 0;
     }
     nTemp = (g_arrnMBTable[TOGGLE_STATUS_COMMAND2_RESTART_MULTIBLEND]);
     if(nTemp)
     {
          RestartMultiblend();
          g_arrnMBTable[TOGGLE_STATUS_COMMAND2_RESTART_MULTIBLEND] = 0;
     }
     nTemp = (g_arrnMBTable[TOGGLE_STATUS_COMMAND2_SET_NBB_NETWORK_SETTINGS]);
     if(nTemp)
     {
          UpdateNBBIPSettings();
          g_arrnMBTable[TOGGLE_STATUS_COMMAND2_SET_NBB_NETWORK_SETTINGS] = 0;
     }

     nTemp = (g_arrnMBTable[TOGGLE_STATUS_COMMAND2_PURGE]);
     if(nTemp)
     {
         if(g_CalibrationData.m_nPurgeEnable == MANUAL_PURGE)
         {
             g_bInitiatePurge = TRUE;
         }
         g_arrnMBTable[TOGGLE_STATUS_COMMAND2_PURGE] = 0;
     }

     nTemp = (g_arrnMBTable[TOGGLE_STATUS_COMMAND2_VIBRATE_CATCH_BOX]);
     if(nTemp)
     {
         g_nVibrateCatchBoxCtr = CATCHBOXVIBRATIONDELAY;
         g_arrnMBTable[TOGGLE_STATUS_COMMAND2_VIBRATE_CATCH_BOX] = 0;
     }

     nTemp = (g_arrnMBTable[TOGGLE_STATUS_COMMAND2_OPERATOR_PANEL_PAGE_ID]);
     if(nTemp)
     {
         g_nOperatorPanelPageId = nTemp;
         g_nSDCardDelayTime = SDCARDCOMMANDDELAYTIME;
         g_arrnMBTable[TOGGLE_STATUS_COMMAND2_OPERATOR_PANEL_PAGE_ID] = 0;
     }

     nTemp = (g_arrnMBTable[TOGGLE_STATUS_COMMAND2_SAFELY_REMOVE_SD_CARD]);
     if(nTemp)
     {
         if(SDCardPresent())
         {
             g_nDisableSDCardAccessCtr = SDCARDACCESSDISABLEINSECS;
         }
     }

     nTemp = (g_arrnMBTable[TOGGLE_STATUS_COMMAND2_LIQUID_ADDITIVE_COMMAND_INITIATE]);
     if(nTemp != 0)
     {
         LiquidAdditiveCommand(nTemp);
         g_arrnMBTable[TOGGLE_STATUS_COMMAND2_LIQUID_ADDITIVE_COMMAND_INITIATE] = 0;
     }
     nTemp = (g_arrnMBTable[BATCH_TOGGLE_STATUS_TARE_SCREW_SPEED]);
     if(nTemp)
     {
         TareScrewSpeed();
         g_arrnMBTable[BATCH_TOGGLE_STATUS_TARE_SCREW_SPEED] = 0;
    }

     nTemp = (g_arrnMBTable[BATCH_TOGGLE_STATUS_CALIBRATE_SCREW_SPEED]);
     if(nTemp)
     {
         g_fScrewSpeedCalibrate = (float)nTemp;
         g_bCalculateScrewSpeedConstant = TRUE;
     }

 nTemp = (g_arrnMBTable[BATCH_TOGGLE_STATUS_RESET_SAMPLE_COUNTER]);
     if(nTemp)
     {
        ResetSampleCounter();
     }

     nTemp = (g_arrnMBTable[BATCH_TOGGLE_STATUS_RESET_LONG_TERM_COUNTERS]);
     if(nTemp != 0)
     {
        ZeroCountersCommand(nTemp);   // check command to zero counters
     }

     nTemp = (g_arrnMBTable[TOGGLE_STATUS_TEST_FUNCTIONALITY]);
     if(nTemp != 0)
     {
         TestFunction(nTemp);
     }

     nTemp = (g_arrnMBTable[TOGGLE_STATUS_COMMAND_INITIATION]);
     if(nTemp != 0)
     {
          InitiateCommand(nTemp);
     }




     if(g_bModbusLineSpeedWritten)
     {
          bCommandClear = FALSE;
          g_bModbusLineSpeedWritten = FALSE;
           //merge LDMBLSPEED inline
          if(g_CalibrationData.m_nLineSpeedSignalType == COMMUNICATION_LINE_SPEED)   //nbb--modbus download of signal type.
          {
             fValue = (float)g_arrnMBTable[BATCH_CONTROL_LINE_SPEED] / 100.0f; // asm = MBLSWRIT
              if(g_CalibrationData.m_nUnitType == UNIT_IMPERIAL_MODBUS)
              {
                  fValue /= g_fLineSpeedConversionFactor;    // convert to meters nbb--todo-- check this
               }
               g_fCommunicationsLineSpeed = fValue;
               g_bLineSpeedUpdate = TRUE;
          }
     }

// download of length

     if(g_bModbusLengthWritten)
     {
          bCommandClear = FALSE;
          g_bModbusLengthWritten = FALSE;

         uValue.nValue[0] = g_arrnMBTable[TOGGLE_STATUS_COMMAND2_LENGTH];                                                       // Length is in m.
         uValue.nValue[1] = g_arrnMBTable[TOGGLE_STATUS_COMMAND2_LENGTH+1];                                                       // Length is in m.
         g_fOptimisationOrderLength = (float) uValue.lValue;
      }

// clear all the toggle status command words in modbus table.
    if(bCommandClear)
    {
        for( i = BATCH_START_TOGGLE_STATUS_COMMANDS; i <= BATCH_END_TOGGLE_STATUS_COMMANDS; i++)
        {
            g_arrnMBTable[i] = 0;
        }
        for( i = BATCH_START_TOGGLE_STATUS_COMMANDS2; i <= BATCH_END_TOGGLE_STATUS_COMMANDS2; i++)
        {
            g_arrnMBTable[i] = 0;         //SBB--todo-- clear 2nd command area.
        }
    }
}



//////////////////////////////////////////////////////
// MBCheckIncreaseDecrease()            ASM = MBCHKINCDEC
//
// Toggle status commands - Check for increase/decrease
// Must be in Manual for inc or dec via comms.
// g_nCommsIncreaseFlag, g_nCommsDecreaseFlag contain the increase or decrease time on exit.
//
//
// M.McKiernan                          11-11-2004
// First Pass
//
//////////////////////////////////////////////////////
void MBCheckIncreaseDecrease( void )
{
    union CharsAndWord uTempCI;

    if(g_bManualAutoFlag == AUTO)       // no increase or decrease if in auto.
    {
        g_nCommsIncreaseFlag = 0;       //COMMSIFLG
        g_nCommsDecreaseFlag = 0;       //COMMSDFLG
    }
    else
    {
         uTempCI.nValue = g_arrnMBTable[BATCH_CONTROL_INC_DEC];
        if(uTempCI.cValue[1])   // decrease??
        {
            g_DecreaseOutputOn10hzCounter = DECREASEOUTPUTONTIME10HZ;
            g_nCommsIncreaseFlag = 0;
            g_nCommsDecreaseFlag = g_nCommsDecreaseFlag + (uTempCI.cValue[1] * INC_DEC_MULTIPLIER);
           if(g_nCommsDecreaseFlag > 255)
                g_nCommsDecreaseFlag = 255; // limit to 255 (ASM was one byte)
        }
        else if(uTempCI.cValue[0])  //increase??
        {
            g_IncreaseOutputOn10hzCounter = INCREASEOUTPUTONTIME10HZ;
            g_nCommsDecreaseFlag = 0;
            g_nCommsIncreaseFlag = g_nCommsIncreaseFlag + (uTempCI.cValue[0] * INC_DEC_MULTIPLIER);
            if(g_nCommsIncreaseFlag > 255)
                g_nCommsIncreaseFlag = 255; // limit to 255 (ASM was one byte)
        }

    }
}


//////////////////////////////////////////////////////
// ClearCommsCounters()          ASM = RESETDIAGCTR
//
// resets comms counters
//
//
// P.Smith                          6-04-2005
// First Pass
//
//////////////////////////////////////////////////////
void ClearCommsCounters( void )
{
    unsigned int i;
    structSSIFData.m_lGoodTx = 0;
    structUFP2Data.m_lGoodTx = 0;
    structSEIData.m_lGoodTx = 0;
    structLLSData.m_lGoodTx = 0;
    structOptimisationData.m_lGoodTx = 0;

    structSSIFData.m_lGoodRx = 0;
    structUFP2Data.m_lGoodRx = 0;
    structSEIData.m_lGoodRx = 0;
    structLLSData.m_lGoodRx = 0;
    structOptimisationData.m_lGoodRx = 0;

    structSSIFData.m_lChecksumErrorCounter = 0;
    structUFP2Data.m_lChecksumErrorCounter = 0;
    structSEIData.m_lChecksumErrorCounter = 0;
    structLLSData.m_lChecksumErrorCounter = 0;
    structOptimisationData.m_lChecksumErrorCounter = 0;

    structSSIFData.m_lRxTimeoutCounter = 0;
    structUFP2Data.m_lRxTimeoutCounter = 0;
    structSEIData.m_lRxTimeoutCounter = 0;
    structLLSData.m_lRxTimeoutCounter = 0;
    structOptimisationData.m_lRxTimeoutCounter = 0;
    g_nCycleCounter = 0;
    g_bSaveAllProcessDataOutsideCycle = TRUE;

    g_sModbusNetworkSlave.m_lGoodTx = 0;
    g_sModbusNetworkSlave.m_lGoodRx = 0;
    g_sModbusNetworkSlave.m_lChecksumErrorCounter = 0;
    g_lTCPIPCommsCounter = 0;

    g_nUSBReadTimeoutCtr = 0;
    g_nUSBCommandSendTimeoutCtr = 0;
    g_nUSBFlushTimeoutCtr = 0;
    g_nUSBWriteTimeoutCtr = 0;
    g_nUSBEchoedTimeoutCtr = 0;
    g_nUSBUSBSendFileCommandsTimeoutCtr = 0;
    g_nUSBGetPrinterStatusTimeoutCtr = 0;

    for(i=0; i < MAX_VAC8IO_CARDS; i++)
    {
        structVAC8IOData[i].m_lGoodTx = 0;
        structVAC8IOData[i].m_lGoodRx = 0;
        structVAC8IOData[i].m_lChecksumErrorCounter = 0;
        structVAC8IOData[i].m_lRxTimeoutCounter = 0;
    }
    g_nEIPCtr = 0;
    g_nEIPAssemblyObjectCtr = 0;
}


//////////////////////////////////////////////////////
// ClearDiagnosticData()
//
//////////////////////////////////////////////////////
void ClearDiagnosticData( void )
{
    ClearCommsCounters();
    g_ProcessData.m_nResetCounter = 0;       //this was put this in clearcommscounters
    g_ProcessData.m_n24HourOngoingResetCounter = 0;
    g_bSaveAllProcessDataToEEprom = TRUE;
    g_bAllowProcesDataSaveToEEPROM = TRUE;
}

//////////////////////////////////////////////////////
// ClearShiftAccumulators          ASM = CLRSHFTACCS
//
//
// M.McKiernan                          8-4-2005
// First Pass
//
//////////////////////////////////////////////////////

void ClearShiftAccumulators ( void )
{
int i;
    g_fTotalShiftWeightOngoingAccumulator = 0.0;  // reset accumulator
    for(i=0; i < MAX_COMPONENTS; i++)
    {
        g_fComponentShiftWeightOngoingAccumulator[i] = 0.0;  // reset component shift accumulators
    }
}


//////////////////////////////////////////////////////
// ZeroCountersCommand
//
//
//////////////////////////////////////////////////////

void ZeroCountersCommand ( int nCommand )
{
    if(nCommand == ZEROLONGTERMRESETTABLECOUNTERSCOMMAND)
    {
        ZeroLongTermWeightResettableAccumulator();
    }
    else if(nCommand == ZEROLONGTERMCOUNTERSCOMMAND)
    {
        ZeroLongTermWeightAccumulator();
    }
}




//////////////////////////////////////////////////////
// InitiateCommand
//
//////////////////////////////////////////////////////

void InitiateCommand ( int nCommand )
{
    if(nCommand == BLENDER_LOCK_CODE)
    {
        g_bConfigIsLocked = TRUE;
        AddLockToEventLog();
    }
    else
    if(nCommand == BLENDER_UNLOCK_CODE)
    {
        g_bConfigIsLocked = FALSE;
        AddUnLockToEventLog();
     }
    if(nCommand == BLENDER_WATCH_DOG_TIMER_ON_CODE)
    {
        g_bWatchdogStatus = GetWatchdogStatus();
        if(g_bWatchdogStatus == DISABLED)
    	{
        	g_nWatchDogCommand = ENABLE_WATCH_DOG_AND_REBOOT;
        	g_nWatchDogCommandInNseconds = TIME_TO_WATCH_DOG_REBOOT;
     	}
    }
    if(nCommand == BLENDER_WATCH_DOG_TIMER_OFF_CODE)
    {
        g_bWatchdogStatus = GetWatchdogStatus();
    	if(g_bWatchdogStatus == ENABLED)
    	{
        	g_nWatchDogCommand = DISABLE_WATCH_DOG_AND_REBOOT;
        	g_nWatchDogCommandInNseconds = TIME_TO_WATCH_DOG_REBOOT;
    	}
    }


}

/*
;* Clear shift accumulators
CLRSHFTACCS:
        LDX     #TOTWACCS9               ; TOTAL Shift WT. ACC
        LDAB    #5*9                     ; 8 BLENDS + PIB + TOTAL (6 bytes/accum.)
        JSR     CLRBBS                   ; CLEAR
        LDX     #SHFTLTACC               ; SHIFT LENGTH ACCUMULATOR (5BYTES)
        LDAB    #5
        JSR     CLRBBS

        JSR     CPYSHWTSMB              ; COPY SHIFT DATA (ZEROED) TO MB
        JSR     CPYSHFTLMB              ;
        RTS


*/



/*
RESETDIAGCTR:
        CLRW    GOODTXCTR1
        CLR     GOODTXCTR1+2
        CLRW    GOODRXCTR1
        CLR     GOODRXCTR1+2
        CLRW    CHECKSUMERR1
        CLR     CHECKSUMERR1+2
        CLRW    TIMEOUTCTR1
        CLR     TIMEOUTCTR1+2

;       TRANSCEIVER COUNTER #2
        CLRW    GOODTXCTR2
        CLR     GOODTXCTR2+2
        CLRW    GOODRXCTR2
        CLR     GOODRXCTR2+2
        CLRW    TIMEOUTCTR2
        CLR     TIMEOUTCTR2+2
        CLRW    CHECKSUMERR2
        CLR     CHECKSUMERR2+2

;       TRANSCEIVER COUNTER #3
        CLRW    GOODTXCTR3
        CLR     GOODTXCTR3+2
        CLRW    GOODRXCTR3
        CLR     GOODRXCTR3+2
        CLRW    TIMEOUTCTR3
        CLR     TIMEOUTCTR3+2
        CLRW    CHECKSUMERR3
        CLR     CHECKSUMERR3+2

        CLRW    GOODNETTXCTR
        CLR     GOODNETTXCTR+2
        CLRW    GOODNETRXCTR
        CLR     GOODNETRXCTR+2
        CLRW    NETCHECKSUMERR
        CLR     NETCHECKSUMERR+2
        CLRW    CYCLECOUNTER
        RTS
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


CYCLEPAUSETYPE
PAUSEATENDOFCYCLE        EQU    1              ; INDICATE PAUSE AT END OF CYCLE.
IMMEDIATEPAUSE           EQU    2              ; IMMEDIATE PAUSE
OFFLINEPAUSEATENDOFCYCLE EQU    3              ; PAUSE AT END OF CYCLE.

NORMALPAUSE              EQU    %00000001       ; NORMAL PAUSE MODE
OFFLINEPAUSEATENDOFCYCLEBIT EQU %00000010       ; OFFLINE PAUSE BIT
PAUSEBITS                EQU    %00000011       ; BITS TO INDICATE PAUSING.
VACINHIBIT               EQU    %00000100       ; PAUSE IMMEDIATELY.



#define BATCH_CLEAR_CLEAN_BITS              0x0100  // Reset the clean cycle bits.
// bits 9 to 15 are unused



;* LDMBSSPEED - LOADS DOWNLOADED SCREW SPEED FROM MODBUS TABLE
LDMBSSPEED:
;       CLR     MBWSSFLAG       ; CLEAR FLAG FOR SCREW SPEED DOWNLOAD
        LDAB    SSSTYPE         ; SCREW SPEED SIGNAL TYPE
        CMPB    #1              ; 1 IS COMMs
        BNE     LDMBSSPDX       ; DONT LOAD IF NOT COMMS
        TST     PROTOCOL
        BPL     LDMBSSPDX       ; EXIT ALSO IF NOT MB PROTOCOL

        LDD     MBSSWRIT        ; LOAD MB VERSION (2DP)(SS DOWNLOAD ON COMMS)
        ANDD    #$8000          ; IS MSB SET ?
        BEQ     LDMBSSPDX       ; EXIT IF NOT.
        LDD     MBSSWRIT
        ANDD    #$7FFF          ; CLEAR MSB
        JSR     HEXBCD2         ; CONVERT 2 HEX BYTES TO BCD, RESULT IN B:E
        STAB    SSRPM2D          ; MAX VALUE   = 6 55 35 Kg/H
        STE     SSRPM2D+1       ; STORE BCD VERSION.

        CLR     SSRPM
        STAB    SSRPM+1
        TED
        STAA    SSRPM+2         ; SSRPM IS integer RPM
LDMBSSPDX:
        RTS
*/

/*
MULTIPLIER EQU   1

AMKEY  LDAA   BLENDERMODE
       CMPA   #OFFLINEMODE
       BEQ    AMKYXX
       LDAB   #$AA
       CMPB   PAUSFLG  ;~PAUSED MODE??
       BNE    AMKY02
       BRA    TOMANUAL
AMKY02 CMPB   MANATO   ;~ARE WE IN AUTO OR MANUAL
       BNE    TOAUTO   ;~NO, THEN ENTER AUTO
TOMANUAL:


; CHECK INCREASE / DECREASE IF IN MANUAL.

MBCHKINCDEC:
       LDAB   MANATO
       CMPB   #$AA              ;~IN AUTO?
       BNE    MBCHKID
       CLR    COMMSIFLG         ;~AUTO, INC/DEC KEY NOT VALID
       CLR    COMMSDFLG
       BRA    XITID

MBCHKID LDD    MBINCDEC
       TSTB                     ; CHECK THE DECREASE BYTE
       BEQ      MBCHINC         ; ALLOFF   ZERO, TURN OFF ALL OUTPUTS
       CLR      COMMSIFLG       ; ENSURE INC FLAG IS TURNED OFF(DEC HAS PRECEDENCE)
;       LDAA     #1
;       CMPB     #$0F            ; SLOW DECR.
;       BEQ      MBCHDEC3
;       LDAA     #10
;       CMPB     #$F0            ; FAST DECR
;       BNE      MBCHDEC5
MBCHDEC3:
         LDAA   #MULTIPLIER
         MUL                    ; A * B
         ADDB   COMMSDFLG
         STAB   COMMSDFLG
MBCHDEC5 BRA    MBTG080           ;CHKERR

MBCHINC TAB                     ; MOVE INC BYTE INTO ACC B
       TSTB                     ;CHECK THE INCREASE BYTE
       BEQ      MBTG080          ;ZERO, DO NOTHING
;       LDAA     #1
;       CMPB     #$0F            ; SLOW INCR.
;       BEQ      MBCHINC3
;       LDAA     #10
;       CMPB     #$F0            ; FAST INCR
;       BNE      MBCHINC5
MBCHINC3 LDAA   #MULTIPLIER
         MUL                    ; A * B
        ADDB   COMMSIFLG
        STAB   COMMSIFLG
MBCHINC5:
XITID  RTS

;   TOGGLE STATUS COMMANDS
;
MBTOGSTA LDD    MBAUTMAN           ;~LOAD THE MODBUS AUTO/MAN TOGGLE
        ANDD    #TOGAMBIT          ;~CHECK THE AUTO/MANUAL BIT
        BEQ     MBTG020            ;~ZERO, DO NOTHING
        JSR     AMKEY              ;~RUN THE KEYBOARD HANDLER
        BRA     MBTG080            ; SKIP INC/DEC IF TOGGLING A/M
MBTG020:
        JSR     MBCHKINCDEC        ; INCREASE / DECREASE FOR MODBUS.
MBTG080 LDD     MBAUTMAN           ; CHECK A/M AGAIN.
        ANDD    #OPENOFFLINEBIT    ; OPEN OFFLINE VALVE
        BEQ     MBTG012            ; NO OPEN OFFLINE
        TST     PAUSFLG            ; IS BLENDER PAUSED ?
        BEQ     MBTG012            ; NO
        JSR     OPENFEED           ; OPEN

MBTG012:
        LDD      MBAUTMAN
        BITB     #FRCINITIATEBIT    ; FRONT ROLL CHANGE VIA COMMS
        BEQ      MBTG013
        JSR      FRONTROLLCHANGE


MBTG013:
        LDD      MBAUTMAN
        BITB     #BRCINITIATEBIT    ; Back ROLL CHANGE VIA COMMS
        BEQ      MBTG014
        JSR     BACKROLLCHANGE

MBTG014:
NOOPOF  LDD     MBAUTMAN           ; CHECK A/M AGAIN.
        ANDD    #CLOSEOFFLINEBIT   ; OPEN OFFLINE VALVE
        BEQ     NOCLOF             ; NO OPEN OFFLINE
        TST     PAUSFLG            ; IS BLENDER PAUSED ?
        BEQ     CHKAM              ; NO
        JSR     CLOSEFEED          ; OPEN
        BRA     CHKAM              ; CHECK A/M


NOCLOF  LDD     MBAUTMAN           ; CHECK A/M AGAIN.
        ANDD    #OPENDUMPBIT       ; OPEN DUMP
        BEQ     NOOPDMP            ; NO.
        TST     PAUSFLG
        BEQ     NOPS1
        JSR     OPENDUMP           ;
NOPS1   BRA     CHKAM              ;

NOOPDMP LDD     MBAUTMAN           ; .
        ANDD    #CLOSEDUMPBIT      ; CLOSE DUMP.
        BEQ     NOCLDMP            ; NO.
        TST     PAUSFLG
        BEQ     NOPS2
        JSR     CLOSEDUMP          ;
NOPS2   BRA     CHKAM              ; CHECK A/M


NOCLDMP LDD     MBAUTMAN           ; CHECK A/M AGAIN.
        ANDD    #TOGPAUSEBIT       ; PAUSE.
        BEQ     NOPAUSET           ; NO.
        JSR     PAUSETOGGLE        ; TOGGLE PAUSE.
        BRA     CHKAM              ; CHECK A/M
NOPAUSET:
        LDD    MBAUTMAN           ;~LOAD THE MODBUS AUTO/MAN TOGGLE
        ANDD    #TOGEOCPAUSEON     ; END OF CYCLE PAUSE.
        BEQ     NOEOCPAUSEON           ; NO.
        JSR     ENDOFCYCLEPAUSEON  ; PAUSE ON.
        BRA     CHKAM              ; CHECK A/M
NOEOCPAUSEON:
        LDD    MBAUTMAN           ;~LOAD THE MODBUS AUTO/MAN TOGGLE
        ANDD    #TOGIMDPAUSEON     ; PAUSE IMMEDIATELY.
        BEQ     NOIMDPAUSEON
        JSR     IMDPAUSEON         ; PAUSE IMMEDIATELY.
        BRA     CHKAM              ; CHECK A/M
NOIMDPAUSEON:
        LDD    MBAUTMAN           ;~LOAD THE MODBUS AUTO/MAN TOGGLE
        ANDD    #TOGPAUSEOFF        ; PAUSE IMMEDIATELY.
        BEQ     CHKAM
        JSR     PAUSEOFF           ; PAUSE OFF.
CHKAM   LDD     MBAUTMAN           ; CHECK A/M AGAIN.
        ANDD    #TOGAMONBIT        ; PAUSE.
        BEQ     NOAMON             ; NO.
        JSR     TOAUTO             ; AUTO.
        BRA     MBTG081            ;
NOAMON  LDD     MBAUTMAN           ; CHECK A/M AGAIN.
        ANDD    #TOGAMOFFBIT
        BEQ     NOAMOFF            ; NO.
        JSR     TOMANUAL           ; SWITCH TO MANUAL.

; CHECK CLEARING COMMANDS

NOAMOFF LDD     MBCLRCMD
        ANDD    #MBCLEARALARMBIT     ; $01 ~CLEAR ALARMS BIT
        BEQ     MBTG081
        JSR     INALTB             ;~INIT. ALARM TABLE, I.E. CLEAR ALARMS
MBTG081:
        LDD     MBCLRCMD
        BITB    #MBCLEARENTERKEYBIT  ; $02 CLEAR ENTER KEY PRESSED FLAG BIT??
        BEQ     MBTG082
        CLR     EKPRSFLG        ; NON-ZERO, => CLEAR FLAG
        LDD     MBNEWDATA       ;
        ANDD    #$FFF8          ; CLEAR NEW SPT & CALIB & K's FLAG bits (B2,B1&B0)
        STD     MBNEWDATA
MBTG082:
        LDD     MBCLRCMD
        BITB    #MBCLEARFRONTRCBIT ;  $04 CLEAR FRCO FLAG BIT??
        BEQ     MBTG083
        LDD     MBNEWDATA       ;
        ANDD    #$FEFF          ; CLEAR FRCO FLAG bit (B8)
        STD     MBNEWDATA
MBTG083:
        LDD     MBCLRCMD
        BITB    #MBCLEARBACKRCBIT ;  $08 CLEAR BRCO FLAG BIT??
        BEQ     MBTG084
        LDD     MBNEWDATA       ;
        ANDD    #$FDFF          ; CLEAR BRCO FLAG bit (B9)
        STD     MBNEWDATA
MBTG084:
        LDD     MBCLRCMD
        BITB    #MBCLEARORDERCHANGEBIT    ;$10            ; CLEAR ORDER CHANGE OCCURRED FLAG BIT??
        BEQ     MBTG085
        LDD     MBNEWDATA       ;
        ANDD    #$FBFF          ; CLEAR OCO FLAG bit (B10)
        STD     MBNEWDATA
MBTG085:
        LDD     MBCLRCMD
        BITB    #MBCLEARRESETOCCBIT       ;$20            ; CLEAR RESET OCCURRED FLAG BIT??
        BEQ     MBTG086
        CLR     RESETFLG         ; CLEAR RESET OCCURRED FLAG (OPSTATUS)
MBTG086:
        LDD     MBCLRCMD
        BITB    #MBCLEARSHIFTWEIGHTBIT    ;$40            ; CLEAR SHIFT WTs
        BEQ     MBTG087
        JSR     CLRSHFTACCS

MBTG087:

        LDD     MBCLRCMD
        BITB    #MBCLRCMDRESETCTRS  ; RESET COMMS COUNTERS
        BEQ     MBTG088
        JSR     RESETDIAGCTR
MBTG088:



;MBTG090 LDD     MBTOGFEN        ; TOGGLE FILL ENABLE's
;        TSTB                    ;~TOGGLE FILL ENABLE BYTE
;        BEQ     MBTG100
;        TBA                     ;~LOAD COMMAND BYTE INTO A
;        JSR     CPYHPCMB        ; HAVE THE MODBUS TABLE UPDATED WITH FILL ENABLES

MBTG100 LDD     MBOPENCMP       ; MB FILL OPEN COMMAND
        BEQ     MBTG121         ; CAN SKIP LONG FILL IF 0000
        JSR     OPENTOGGLE


MBTG121 LDD     MBCLOSECMP      ; CLOSE COMMAND
        BEQ     MBTG122         ;
        JSR     CLOSETOGGLE
MBTG122:
        LDD     MBCLRCMD
        BITB    #$02            ; CLEAR ENTER KEY PRESSED FLAG BIT??
        BEQ     MBTG130
        CLR     EKPRSFLG        ; NON-ZERO, => CLEAR FLAG
        LDD     MBNEWDATA       ;
        ANDD    #$FFF8          ; CLEAR NEW SPT & CALIB & K's FLAG bits (B2,B1&B0)
        STD     MBNEWDATA
MBTG130:
        LDD     MBSC_CMD        ;~SCREW FEEDER CALIBRATION COMMAND
        BEQ     MBTG140
        STAB    NETSCCMD        ;~STORE NETWORK SCREW CALIB. COMMAND
MBTG140:
        TST     MANATO
        BNE     MBTG150         ; DO NOT ACCEPT D/A IF NOT IN MANUAL
        LDD     MBSETD_A        ; EXTRUDER D/A VALUE (WRITTEN TO BLENDER)
        BPL     MBTG150         ; MSB MUST BE SET TO INDICATE NEW VALUE
        ANDD    #$7FFF          ; MASK OFF M.S.BIT
        CPD     DA1TMP          ; IS IT SAME AS CURRENT VALUE
        BEQ     MBTG150         ; IF SO NO ACTION NECESSARY
        STD     DA1TMP          ; WRITE NEW VALUE
        LDAB   #15                ; Set "to stall time" to 15 SECS
        STAB   TOSTALTIM
        CLR    EXTSCTR            ; extruder stalled counter = 0
        INC    MCHGFLG            ;~SET MANUAL CHANGE FLAG
        INC    CCVSFLG
        LDAA    #1
        STAA    RUNEST          ;  MAKE ESTIMATION RUN.

;       CLR    NMANENB            ;~ENABLE NMANUAL
PBNCK9X

MBTG150:
; if L5 fitted (0) then USE flag to load setpoints
        TST     SINGLEWORDWRITE          ;USE LINK L5 FOR HAVING TO SET EXTRA FLAG
        LBEQ    MBTG175         ; NOT FITTED (IGNORE MBNWDCMD)

        LDE     MBNWDCMD        ; NEW DATA COMMAND
        ANDE    #$0007          ; CHECK FOR ANY SETPTS // K's not treated as spt's
        LBEQ    MBTG175
;
        TSTW    MBRSTOTLS       ; RESET ORDER TOTALS FLAG IN MB TABLE
        BNE     MBTG151         ; IF NON-ZERO NO ACTION REQ'D

        JSR     ASSORPT         ; ASSEMBLE ORDER REPORT
        CLRW    MBRSTOTLS       ; CLEAR FLAG
;NOTE MBNEWDATA BIT 10 IS SET IN CPYORRMB

MBTG151:
        LDE     MBNWDCMD        ; NEW DATA COMMAND
        ANDE    #$0007          ; CHECK FOR %, TPT OR OI
        BEQ     MBTG158
;       CLR     LDNSPLAG
        ANDE    #$0001          ; CHECK FOR % WRITE
        BEQ     MBTG152
        COME
        ANDE    MBNWDCMD
        STE     MBNWDCMD
        CLR     LDNSPLAG

        JSR     CHMB100T
        TSTB
        BEQ     LDPDTA          ; LOAD % DATA
        LDD     MBWORFTP
        STD     MBWORFER        ; STORE TEMP 'OR' FLAG IN ERROR FLAG
        BRA     MBTG152
LDPDTA  JSR     LDMBPCTS       ; LOAD %'S FROM MB TABLE

MBTG152:
        LDE     MBNWDCMD        ; NEW DATA COMMAND
        ANDE    #$0002          ; CHECK FOR NEW TPT SETPOINTS
        BEQ     MBTG156
        COME
        ANDE    MBNWDCMD
        STE     MBNWDCMD
        CLR     LDNSPLAG
        JSR     LDMBTPTS       ; LOAD TPT'S FROM MB TABLE
MBTG156:
        LDE     MBNWDCMD        ; NEW DATA COMMAND
        ANDE    #$0004          ; CHECK FOR NEW ORDER INFO SETPOINTS
        BEQ     MBTG158
        COME
        ANDE    MBNWDCMD
        STE     MBNWDCMD
        CLR     LDNSPLAG
        JSR     LDMBOINFO       ; LOAD ORDER INFO FROM MB TABLE
MBTG158:
MBTG159:
        LDAB    #$0F
        STAB    NEWSET          ; SET FLAG FOR NEW SETPOINTS
MBTG175:

MBTG180:
        JSR     LDMBSSPEED      ; IF SO LOAD UP SCREW SPEED
MBTG182:

       LDD      MBPRDIAG
       BEQ      NOPRODD         ; NO PRODUCTION DIAGNOSTICS
       JSR      ACTIVATEPRODDIAGNOSTICS
       CLRW     MBPRDIAG
NOPRODD
        LDD     MBCLEAN           ; .
        ANDD    #MBTOGSTANORMALCLEANBITPOS   ; START CLEAN CYCLE
        BEQ     NONCL                        ; NO.
        JSR     INITIATECLEANCYCLE ; .
        CLRW    MBCLEAN                      ; CLEAN THIS FOR NOW

NONCL   LDX     #MBOPENCMP
        LDE     #TOGGLESIZE
        JSR     CLREWX          ; CLEAR ALL USED TOG STAT. COMMANDS
        RTS
;


*/
