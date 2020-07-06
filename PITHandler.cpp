/*
// P.Smith                      24/1/06
// first pass at netburner hardware conversion.
// call  PITMBMHandler() for master comms

// P.Smith                      26/1/06
// call  ParallelPrintHandler();

// P.Smith                      7/2/06
// bring in all variable definitions.
// int changed to WORD g_nMBSlaveCommsWatchDogTimer;
// Bool -> BOOL
// include "BatchCalibrationFunctions.h"
// removed  g_nPITStart = GPT_TCNT;         // current PIT
// removed  Gtemp = GPT_PDR;   //read port G
// added #include "InitialiseHardware.h"
// added #include    "BatVars.h"
// added #include    "BatVars2.h"
// added #include "NBBGpio.h"
// added #include "TSMPeripheralsMBIndices.h"
// removed start a/d on reset        StartMAX132();                          // start the MAX132 a/d converter.
// removed HandleMAX132();                 // Handle data read from MAX132 A/D converter.
// removed Gtemp = GPT_PDR;   //read port G
// removed FiftyHzHandler();       // located in FiftyHz.c. (asm equivalent was FIFTYHZHAND)
// removed TenHzProgram();     // located in TenHz.c - ASM equiv. was TENHZHAND.
// removed PITWeigh();                             // Main batch cycling program.
// removed CheckForEndOfCyclePause();
// removed ReadSEIData();          // read SEI data
// removed operator wait time

// removed g_nPITEnd = GPT_TCNT;  unused for now.
// removed break counter g_nMB1BreakCounter decrement, done at full pit speed.
// removed g_cMB1txdlytime decrement,done at full pit speed
// removed extern  int     g_nIncreaseKeyCounter;
// removed extern  int     g_nDecreaseKeyCounter;
// removed reference to these counters
// define these in this file for now
//BOOL g_bSPIAtDActive;   //nbb
//BOOL g_bSPIEEActive;    //nbb
//

// P.Smith                      8/2/06
// PITCall called from pit, this is the one that the existing blender is run at.
// #include "PITHandler.h"
// clear g_nPitDivider when the blender pit program is called.
// remove ParallelPrintHandler();
// remove PITMBMHandler();   called further down.
// called        FiftyHzHandler(),TenHzProgram()
// #include    "FiftyHz.h"
// #include    "TenHz.h"
// #include    "Pause.h"
// added back in CheckForEndOfCyclePause();
//
// P.Smith                      13/2/06
// set g_bSPIAtDActive to FALSE;
// removed BOOL g_bSPIAtDActive;   //nbb
//         BOOL g_bSPIEEActive;    //nbb
// #include "Weight.h" added
// call    ProcessLoadCellWeight()  and set g_bSPIAtDActive = FALSE;
//
// P.Smith                      21/2/06
// implement rwsei.cpp todos added FrontRollChange(); BackRollChange()
// ReadSEIData();          // read SEI data
// #include "Rwsei.h"
//
// P.Smith                      27/2/06
//   PITWeigh();                             // Main batch cycling program.
// #include    "PITWeigh.h"
//
// P.Smith                              19/5/06
// remove RxBOn(); when last character has been transmitted.
//
// P.Smith                              29/5/06
// Removed unsigned char Gtemp;    char cT = 0;
// do not run blender pit unless g_bProgramLoopRunnning
//
// P.Smith                              28/6/06
// removed SPIFuncs.cpp start a/d on reset        StartMAX132();                          // start the MAX132 a/d converter.
//
// P.Smith                              28/6/06
// correct check for g_bSPIAtDActive & g_bSPIEEActive
// ensure that g_bSPIAtDActive is reset to FALSE when  g_bSPIEEActive is FALSE
// added NBB slave comms counters decrement etc.
//
// P.Smith                              20/9/06
// name changes,NetworkTransmitterOn,PanelTxOn,ResetPanelMBSlaveComms
//
// P.Smith                      10/11/06
// name change g_nCycleIndicate -> g_bCycleIndicate
// remove unused commented out code.
// decrement of g_nEEPROMReadTimeoutCounter & g_nEEPROMWriteTimeoutCounter
// remove unused commented out code.
//
// P.Smith                      5/2/07
// decrement g_nSPIEEActiveTimeoutCounter if non zero.
// decremenet of g_nRevertToOperatorPanelCounter and resetart of panel called on 1-> 0
// name change EnableTxB -> EnableSlaveTransmitter
// name change DisableTxB -> DisableSlaveTransmitter
// #include "Mtttymux.h"
//
// P.Smith                      8/3/07
// read of ultrasonic data from sensor called on fifty hz.
//
// P.Smith                      14/3/07
// check for g_bUltrasonicLIWInstalled before calling ultrasonic read.
//
// P.Smith                      23/5/07
// correct flashing of led for 1 second, was called in blender pit, this had the effect
// of the led flashing too slow.
// g_bPITAllowed checked to determine if the pit should be run, this is used on reset to
// disable the pit when flashing the led.
// check g_bStopSPIInterruptRunning to determine if pit should run.
// call UpdateExpanIO from blender pit function, this will now run at 1500hz.
//
// M. McKiernan                      20/6/07
//  see ....//mmk - 7/6/2007 - added SPIBusBusy flag...cannot run if SPI is busy.
//        if(g_bSPIAtDActive && !g_bStopSPIInterruptRunning && !g_bSPIBusBusy)
//
// P.Smith                           25/6/07
// Remove g_bStopSPIInterruptRunning
// correct names for blender types
//
// M.McKiernan                      17/9/07
// Removed g_arrnMBTable[] & g_arrnMBTableFlags[], now in BatVars2
//
// P.Smith                          1/11/07
// remove RestartPanel direct call, set g_bRestartPanelNow to initiate same in foreground
//
// P.Smith                          9/11/07
// RestartPanel call directly, this cannot be called in the foreground.
//
// P.Smith                          13/11/07
// Modified control mode format implemented
// allow for new bit set up for control  mode.
//
// P.Smith                          16/1/08
// Get software ready for usb stuff, call VNC1LPITISRHandler on 3khz pit
//
// P.Smith                          22/1/08
// leave commented out VNC1LPITISRHandler call on 3khz pit
//
// P.Smith                          12/2/08
// put back in vnc polling for usb.
//
// P.Smith                          22/2/08
// remove VNC1LPITISRHandler
// only store g_nExtruderDAValue to  g_arrnWriteSEIMBTable[MB_SEI_DA] if in manual
// and mode is 0 - 10 v.
//
// P.Smith                          1/3/08
// only transfer g_nExtruderDAVAlue to sei mb_sei_da if manual and control type is
// 0 - 10 volt mode.
//
// P.Smith                          17/7/08
// derement g_nSDCardDelayTime
//
// P.Smith                          23/7/08
// remove g_arrnMBTable and array comms extern
//
// P.Smith                          23/7/08
// when reading a/d check for eoc / sign error, do not process a/d reading if error
//
// P.Smith                          24/7/08
// if g_nAMToggleStatusDelayCtr is non zero decrement it
//
// P.Smith                          26/9/08
// if g_nLiquidOnTimeCtr is non zero, decrement g_nLiquidOnTimeCtr, LiquidAdditiveOnTimeExpired
// is called.
//
// P.Smith                          16/10/08
// removed ultrasonic stuff.
//
// P.Smith                          14/11/08
// remove check on g_sVNC1L.m_bTransactionInProgress
// decrement g_bTelNetWindowCtr
// set g_bTelNetWindowCtr  to TELNETWINDOWINPITS unused for now
//
// P.Smith                          22/12/08
// decrement timeout counters on pit.
// remove timeouts for now.
//
// P.Smith                          14/1/09
// modify timeout for panel comms to be reactivated continuously when the
// panel is not communicating.
// a problem was seen in that when the panel was not connected and telnet was started
// the panel did not start up again.
//
// P.Smith                          21/1/09
// call CopyFunctionRunningToMB to copy function data to mb, this will still continue to
// run if the foreground is held up.
// remove ResetPanelMBSlaveComms, set g_bResetPanelSlaveComms instead and call the
// ResetPanelMBSlaveComms in the foreground.
// set g_bResetPanelSlaveComms instead of calling ResetPanelMBSlaveComms directly from the pit.
// there is a problem doing this, the open port causes some of the pins to be changed.
// and the pins are reinitialised to work as spi.
// however if this is called in the pit, it means that if the spi pins have been set up
// for a particular function, say usb, then this can set the select to the expansion 0,0
// and the usb will no longer work.
//
//
// P.Smith                          22/1/09
// remove check for max a/d value, this stops the auto cycle from working.
// this is done seperately anyway in fgonehz
//
// P.Smith                          12/2/09
// put back in timeout for usb
//
// P.Smith                          5/3/09
// added g_nBatchCommsPanelTimeoutRx increment
//
// P.Smith                          12/5/09
// set g_nCyclePauseTypeCopy to OFFLINE_END_OF_CYCLE_PAUSE_TYPE
//
// P.Smith                          26/8/09
// set flag to show that pit is running.
// this is used for the alarms.
// the event is written to the sd card
//
// P.Smith                          7/9/09
// added g_lLoadTime increment, decrement g_nWaitForSPIBusToSettleCtr
// call CheckForAToDError to check a/d status.
//
// P.Smith                          10/9/09
// added check for status value of FFFF, this is invalid and the check on
// the various errors should not be done in this case.
//
// P.Smith                          18/9/09
// remove a/d test initiation, this was causing a problem when no unit was connected.
//
// P.Smith                          12/3/10
// increment software watch dog counter, if it reaches SOFTWARE_WATCHTIME_SECONDS_TIMEOUT
// initiate a reboot
// five seconds before the reboot,a warning is given indicating that a watch dog reset
// is pending.
//
// P.Smith                          16/3/10
// remove software watch dog for now
//
// P.Smith                          6/4/10
// added function running 56,57,58,59,60,61, call SaveFuntionNosInRAM
//
// P.Smith                          8/6/10
// call WidthPITHandler & WidthFiftyHertzHandler
//
// M.McKiernan						20/5/2020
/ //wfh
//    if(g_nPIT50HzCounter == (PIT_FIFTY_HZ_PERIOD-1) )	// advance the Ultrasonic Mux 1 PIT before triggering the US sensor. Time to settle.
//    	AdvanceToNextChannel();
//    	//Test stuff for USMux card.
//    	if(g_bTestUSMuxA0Low)
//    		SetA0Lo();
//    	else if(g_bTestUSMuxA0High)
//    		SetA0Hi();
//    	else if(g_bTestUSMuxClockLow)
//    		USMuxClockLo();
//    	else if(g_bTestUSMuxClockHigh)
//    		USMuxClockHi();
// //wfh
//    if(g_nPIT50HzCounter == (PIT_FIFTY_HZ_PERIOD-2) )	// advance the Ultrasonic Mux 1 PIT before triggering the US sensor. Time to settle.
//    {
//    	if(g_bTestDLs)		// set on 1 Hz.
//    	{
//    		g_bTestDLs = FALSE;
//    		//TestDrivingLEDs();
//    	}
//    }
//    PITTestForUSMuxLEDs();		// some test for driving LEDs.

//    if(g_nPIT50HzCounter == (PIT_FIFTY_HZ_PERIOD-1) )	// advance the Ultrasonic Mux 1 PIT before triggering the US sensor. Time to settle.
//    	AdvanceToNextChannel();
//   29.6.2020
//  EnableUSMux1(); - Force the EN for the USMux #1.
//  removed EnableUSMux1()
////////////////////////////////////////////////////////////////////////////////

*/

#include "InitComms.h"
#include "MBShand.h"
#include "MBPanelSNBBHand.h"

#include "MBMaster.h"
#include "PrinterFunctions.h"
#include "Nbbvars.h"


#include "General.h"
#include "NBBGpio.h"

#include "InitialiseHardware.h"
#include    "PITWeigh.h"
#include    "BatVars.h"
#include "PITHandler.h"
#include    "FiftyHz.h"
#include    "TenHz.h"
#include "BatchCalibrationFunctions.h"
#include    "BatVars2.h"
#include    "RwSEI.h"
#include    "Pause.h"
#include "TSMPeripheralsMBIndices.h"
#include "Init5270.h"
#include "Weight.h"
#include "InitNBBComms.h"
#include "Mtttymux.h"
#include "UltrasonicRead.h"


#include <stdio.h>
#include "Expansion.h"
#include "VNC1L.h"
#include "LTC2415.h"
#include "LiquidAdditive.h"
#include    "MBProgs.h"
#include "Alarms.h"
#include <Bsp.h>
#include "Eventlog.h"
#include "Security.h"
#include "WidthPithandler.h"

#include "WidthUltrasonics.h"

extern  WORD  g_nMB1BreakCounter;
extern  char        g_cMB1BreakFlag;
extern  unsigned char g_cMB1txdlytime;
extern  char    g_cLastChTimer;
extern  volatile WORD g_lRTCCount;


// Externally declared global variables
extern  structCommsData g_sModbusMaster;
extern  int     g_nLayerCount;
extern  int     g_cTx1OffFlag;
extern  int g_nMBSlaveCommsWatchDogTimer;
extern  structControllerData    *arrpDeviceList[];
extern CalDataStruct    g_CalibrationData;

extern BOOL g_bPauseSequencerFlag;


// Locally declared global variables
int     g_nGeneralTickTimer;        // PIT ticks
int     g_nGeneralSecondTimer;      // 1 second ticks
int     g_nSequencerTimer = 0;      // PIT ticks
int     g_nTickCounter = 0;         // General purpose tick counter
int     g_bCheckingComms = FALSE;

// Report variables
int     g_nFrontRollReportTimer = 0;
int     g_nBackRollReportTimer = 0;
int     g_nOrderReportTimer = 0;
int     g_nG7ActivatedCounter = 0;
BOOL    g_bFrontRollReportStart = FALSE;
BOOL    g_bBackRollReportStart = FALSE;
BOOL    g_bOrderReportStart = FALSE;
BOOL    g_bGenerateFrontRollReport = FALSE;
BOOL    g_bGenerateBackRollReport = FALSE;
BOOL    g_bGenerateOrderReport = FALSE;
BOOL    g_bFrontReportStatus[MAX_LAYERS];
BOOL    g_bBackReportStatus[MAX_LAYERS];
BOOL    g_bOrderReportStatus[MAX_LAYERS];

BOOL    g_bPITHandlerInProgress = FALSE;        // flag to indicate that PIT interrupt routine is already running
//                              // Used to prevent another PIT int. coming in on top.

int     g_nHalfSecondTimer = 0;
int     g_nIncreaseKeyWatchDogCounter = 0;
int     g_nDecreaseKeyWatchDogCounter = 0;
int     g_nPIT_Int_Counter = 0;
int     g_nPITIntsPerSec = 0;
//int       g_nPITCounter = 0;
int         g_nVAC350MBCalUpdateTimer;
int     g_nPIT50HzCounter;
int     g_nPIT10HzCounter;
int     g_nPITADCounter;

int     g_nFlashCounter = 0;
int     g_nFlashRate;

int g_nPITTimeSumCtr = 0;
long    g_lPITTimeSum = 0;
int g_nPITTimeAvg = 0;

int g_nScreenDataRefreshCounter = 0;
BOOL    g_bScreenDataRefreshed = FALSE;
int		SoftwareWatchDogOneSecondCtr = 0;


extern int g_nOperatorWaitTime;
extern BOOL g_bResetADFLag;
extern BOOL g_bSPIAtDActive;   //nbb
extern BOOL  g_bSPIEEActive;    //nbb
extern structVNC1LData   g_sVNC1L;



unsigned int  g_nPITLength;
unsigned int  g_nPITLengthMax = 0;        // max value
unsigned int  g_nPITStart,g_nPITEnd;

unsigned int    g_nPITsGT20 = 0;
unsigned int    g_nPITsGT40 = 0;
unsigned int    g_nPITsGT50 = 0;
unsigned int    g_nPITsGT60 = 0;
unsigned int    g_nPITsGT70 = 0;
unsigned int    g_nPITsGT80 = 0;
unsigned int    g_nPITsGT90 = 0;

unsigned int    g_nPIT_Int_CounterU = 0;
unsigned    int g_nPITIntsPerSecU = 0;
extern volatile WORD g_nI2CWaitPits;			// Value - decremented each PIT.


extern  WORD  g_nNBBMB1BreakCounter;
extern  char        g_cNBBMB1BreakFlag;
extern  unsigned char g_cNBBMB1txdlytime;
extern  char    g_cNBBLastChTimer;
extern  int     g_nMBSlaveNBBCommsWatchDogTimer;

bool g_bTestUSMuxA0Low = FALSE;
bool g_bTestUSMuxA0High = FALSE;
bool g_bTestUSMuxClockLow = FALSE;
bool g_bTestUSMuxClockHigh = FALSE;
extern bool g_bTestDLs;

void PITHandler( void )
{
	WidthPITHandler();
    g_nFunctionRunning = FUNCTION_56;
    SaveFuntionNosInRAM();
    g_bPitRunning = TRUE;
    g_lLoadTime++;                     // CYCLETIME

    g_nTickCounter++;
    if(g_nWaitForSPIBusToSettleCtr != 0)
    {
        g_nWaitForSPIBusToSettleCtr--;
    }

    if( g_nUSBGetPrinterStatusTimeoutInPits != 0 )
    {
       g_nUSBGetPrinterStatusTimeoutInPits--;
    }
    if( g_nUSBEchoedTimeoutInPits != 0 )
    {
        g_nUSBEchoedTimeoutInPits--;
    }
    if( g_nUSBUSBSendFileCommandsTimeoutInPits != 0 )
    {
        g_nUSBUSBSendFileCommandsTimeoutInPits--;
    }
    if( g_nUSBWriteTimeoutInPits != 0 )
    {
        g_nUSBWriteTimeoutInPits--;
    }
    if( g_nUSBFlushTimeoutInPits != 0 )
    {
        g_nUSBFlushTimeoutInPits--;
    }
    if( g_nUSBCommandSendTimeoutInPits != 0 )
    {
        g_nUSBCommandSendTimeoutInPits--;
    }
    if( g_nUSBReadTimeoutInPits != 0 )
    {
        g_nUSBReadTimeoutInPits--;
    }
   if(g_bPITAllowed)
   {

    if(g_bTelNetWindowCtr != 0)
    {
        g_bTelNetWindowCtr--;
    }

    if( g_nLiquidOnTimeCtr != 0 )
    {
        g_nLiquidOnTimeCtr--;
        if(g_nLiquidOnTimeCtr == 0)
        {
             LiquidAdditiveOnTimeExpired();
        }
    }

    if( g_nAMToggleStatusDelayCtr != 0 )
    {
        g_nAMToggleStatusDelayCtr--;
    }

    if( g_nSDCardDelayTime != 0 )
    {
        g_nSDCardDelayTime--;
    }

    if( g_nEEPROMReadTimeoutCounter )
   	    g_nEEPROMReadTimeoutCounter--;


    if( g_nEEPROMWriteTimeoutCounter )
   	    g_nEEPROMWriteTimeoutCounter--;

    if( g_nSPIEEActiveTimeoutCounter )
    {
        g_nSPIEEActiveTimeoutCounter--;
    }
     if( g_nRevertToOperatorPanelCounter )
    {
        g_nRevertToOperatorPanelCounter--;
        if(g_nRevertToOperatorPanelCounter == 0)
        {
//                  g_bRestartPanelNow = TRUE;
             RestartPanel();
        }
    }


    if( g_nI2CWaitPits )
   	g_nI2CWaitPits--;
     if(g_nMB1BreakCounter)
     {
             g_nMB1BreakCounter--;       // dec MB break counter to 0
             if(g_nMB1BreakCounter == 0)
             {
             g_cMB1BreakFlag =1;
             }
     }
     if(g_nNBBMB1BreakCounter)
     {
             g_nNBBMB1BreakCounter--;       // dec MB break counter to 0
             if(g_nNBBMB1BreakCounter == 0)
             {
                 g_cNBBMB1BreakFlag =1;
             }
     }


// decrement delay before starting modbus data transmission

    if(g_cMB1txdlytime)
        {
            g_cMB1txdlytime--;        // dec MB tx delay time to 0
            if(g_cMB1txdlytime == 0)
            {

                EnableSlaveTransmitter();          // enable transmitter
                NetworkTransmitterOn();
            }
        }
   if(g_cNBBMB1txdlytime)
        {
            g_cNBBMB1txdlytime--;        // dec MB tx delay time to 0
            if(g_cNBBMB1txdlytime == 0)
            {

 //nbb--todolp--               EnableU1TxB();          // enable transmitter
                PanelTxOn();
            }
        }



     if(g_cLastChTimer)     // ASM  equivalent = POLLFORLASTCH.
     {
             g_cLastChTimer--;        // dec MB tx delay time to 0
             if(g_cLastChTimer == 0)
            {
                 DisableSlaveTransmitter();         // disable transmitter
            }
     }

      if(g_cNBBLastChTimer)     // ASM  equivalent = POLLFORLASTCH.
     {
             g_cNBBLastChTimer--;        // dec MB tx delay time to 0
             if(g_cNBBLastChTimer == 0)
            {
                 DisableNBBTxB();         // disable transmitter
            }
     }
     g_nPitDivider++;
     if((g_nPitDivider >=2) && (g_bProgramLoopRunnning))  // pit speed now 1500 hz.
     {
    	   g_nFunctionRunning = FUNCTION_57;
           SaveFuntionNosInRAM();
    	   BlenderPIT();
           g_nPitDivider = 0;
     }
    g_nFlashCounter++;
    if(g_nFlashCounter >= g_nFlashRate)
    {
       g_nFlashCounter = 0;
       ToggleHBLED( );
    }
  }
  //VNC1LPITISRHandler();
  g_bPitRunning = FALSE;


}

void BlenderPIT( void )
{

       g_nPIT_Int_CounterU++;      // U is for unstoppable, i.e before pit in progress is checked.

//mmk Try to prevent another PIT interrupt coming in on top of one being implemented
 if(g_bPITHandlerInProgress == FALSE)  //i.e.. if not already running
 {                      // run PIT handler
    char c50Hz = 0;
    char c10Hz = 0;

    g_bPITHandlerInProgress = TRUE;


    // This part of the PIT is running at the highest possible speed 1170 hz

    g_nPIT_Int_Counter++;

    g_nGeneralTickTimer++;

//bb    ********************* PIT handler from batch from here.***************************** //bb


    if(g_bResetADFLag)      // RESETAD
    {
        g_bResetADFLag = FALSE;
    }
    else
    {
    	//Test stuff for USMux card.
    	if(g_bTestUSMuxA0Low)
    		SetA0Lo();
    	else if(g_bTestUSMuxA0High)
    		SetA0Hi();
    	else if(g_bTestUSMuxClockLow)
    		USMuxClockLo();
    	else if(g_bTestUSMuxClockHigh)
    		USMuxClockHi();


  // POLLMAX132TASK
  //mmk - 7/6/2007 - added SPIBusBusy flag...cannot run if SPI is busy.
        if(g_bSPIAtDActive && !g_bSPIBusBusy)
        {
            g_bSPIAtDActive = FALSE;
            if(!g_bSPIEEActive && (g_nWaitForSPIBusToSettleCtr == 0))
            {
                g_bTelNetWindowCtr = TELNETWINDOWINPITS;
                g_nFunctionRunning = FUNCTION_61;
                SaveFuntionNosInRAM();

                WORD nStatus = ReadAToD_1();                 // Handle data read from MAX132 A/D converter.
                CheckForAToDError(nStatus);
                // check if the a/d reading is valid
                if( (nStatus & LTC2415_EOC_BIT) == 0 )          // EOC is high during conversion.
                {
                    ProcessLoadCellWeight();
                }
                else
                {
//                    iprintf("error in a/d ");  //nbb--todo-- generate alarm
                }

            }
         g_bSPIAtDActive = FALSE;
        }
    }
    UpdateExpanIO();

    g_nPITFractionCounter++;            // PITFRACTCTR - used in batch timing.


// check for 50Hz

    g_nPIT50HzCounter++;
//wfh
    if(g_nPIT50HzCounter == (PIT_FIFTY_HZ_PERIOD-2) )	// advance the Ultrasonic Mux 1 PIT before triggering the US sensor. Time to settle.
    {
    	if(g_bTestDLs)		// set on 1 Hz.
    	{
    		g_bTestDLs = FALSE;
    		//TestDrivingLEDs();
    	}
    }

    //PITTestForUSMuxLEDs();		// some test for driving LEDs.
//    EnableUSMux1();					// call on PIT to force the EN to correct state.   (SRC10OFF() - looks like something is turning SRC10 on.)

    if(g_nPIT50HzCounter == (PIT_FIFTY_HZ_PERIOD-1) )	// advance the Ultrasonic Mux 1 PIT before triggering the US sensor. Time to settle.
    	AdvanceToNextChannel();

    if(g_nPIT50HzCounter >= PIT_FIFTY_HZ_PERIOD)
    {
        g_nPIT50HzCounter = 0;
        g_nFunctionRunning = FUNCTION_60;
        SaveFuntionNosInRAM();
        FiftyHzHandler();       // located in FiftyHz.c. (asm equivalent was FIFTYHZHAND)
        WidthFiftyHertzHandler();
        c50Hz = 1;  // testonly
    }

// check for 10Hz

    g_nPIT10HzCounter++;
    if(g_nPIT10HzCounter >= PIT_TEN_HZ_PERIOD && !c50Hz)        //dont run if 50Hz has been run on this PIT..
    {
        g_nPIT10HzCounter = 0;  // asm = TENHERCNT
        g_nFunctionRunning = FUNCTION_59;
        SaveFuntionNosInRAM();
        TenHzProgram();     // located in TenHz.c - ASM equiv. was TENHZHAND.
// mechanism to control data refresh rate on any screen.
// In this case data only refreshed every 5*0.1S, i.e. every 0.5S.
        g_nScreenDataRefreshCounter++;
        if( g_nScreenDataRefreshCounter > 5 )
        {
            g_nScreenDataRefreshCounter = 0;
            g_bScreenDataRefreshed = FALSE;
        }
        c10Hz = 1;  // testonly

    }

// Check for time to poll a/D.
    g_nPITADCounter++;      // PITADCTR
    if(g_nPITADCounter >= PIT_AD_POLL_PERIOD)
    {
        g_nPITADCounter = 0;
        g_bSPIAtDActive = TRUE;                     // SetMAX132Task();
    }


    g_nFunctionRunning = FUNCTION_58;
    SaveFuntionNosInRAM();
    PITWeigh();                             // Main batch cycling program.

//--todolp--  UpdateExpanIO();        //  UPDATEEXPANIO

    if( g_bPeripheralCommsSTarted ) //COMMSST   comms started??
    {
        g_nSeqTimeout++;        // SEQTIMEOUT
    }
    else
    {
        g_nSeqTimeout = 0;
    }

    g_nLineSpeedFractionCounter++;  // LSPDFRACTIONCTR

// NETCOMMSCTR - Checking for MB break. - handled above by g_nMB1BreakCounter

// CHECKFORSLIDECYCLE:  --REVIEW-- This is a diagnostics function only, Opentime counter set via LINE No.
//                              Could potentially cause problems if Line no. for Micra(TINY) was non 0.
// TINYSLIDEOPENTIMECOUNTER gets set in CHECKSLIDEINTERVAL

//
// one second here - 1Hz
//

    if( g_nGeneralTickTimer >= GENERAL_TIMER_SECOND && !c50Hz && !c10Hz)        //dont run if 50Hz or 10Hz has been run on this PIT..
    {
 //   	SoftwareWatchDogOneSecondCtr++;
 //    	if(SoftwareWatchDogOneSecondCtr == SOFTWARE_WATCHTIME_SECONDS_TIMEOUT-5)
 //    	{
 //    		AddSoftwareResetPendingEventToLog();
 //    	}
 //    		if(SoftwareWatchDogOneSecondCtr > SOFTWARE_WATCHTIME_SECONDS_TIMEOUT)
 //   	{
 //   		SoftwareWatchDogOneSecondCtr = 0;
 //nbb--todo-put back           ForceReboot();
 //   	}

//          g_nPITStart = GPT_TCNT;         // current PIT (--testonly--)

        CopyFunctionRunningToMB();
        g_nGeneralSecondTimer++;
        g_nGeneralTickTimer = 0;

        g_nDeviceToPollCounter = 0;         // DEVICETOPOLLCTR ; RESET DEVICE TO POLL CTR.

        g_bSEIPollInputsFlag    = TRUE;     // SEIPOLLINPUTSF          ; START POLLING OF INPUTS


        if((g_bManualAutoFlag == MANUAL)&& (g_CalibrationData.m_nControlType == CONTROL_TYPE_0_10))
        {
            g_arrnWriteSEIMBTable[MB_SEI_DA] = g_nExtruderDAValue;
        }
        g_bSEIPollLSFlag = TRUE;            // SEIPOLLLLSF             ; SET LOW LEVEL SENSOR
        g_bWriteAlarmsFlag = TRUE;          // WRITEALARMSF
        if(((g_CalibrationData.m_nControlMode & CONTROL_MODE_VOLTAGE_FOLLOWER) != 0) && (g_bManualAutoFlag == MANUAL))
        {
            g_bReadDAValueFlag = TRUE;      // Trigger D/A read from SEI.
        }


// --review-- this could be moved to the foreground
        if(g_nPauseInNSeconds)     //pause in n seconds set ?
        {
            g_nPauseInNSeconds--;  // decrement counter
            if(g_nPauseInNSeconds == 0)
            {
                g_nCyclePauseType = OFFLINE_END_OF_CYCLE_PAUSE_TYPE;
                g_nCyclePauseTypeCopy = OFFLINE_END_OF_CYCLE_PAUSE_TYPE;

                if(!g_bCycleIndicate)  //if not cycling
                {
                     CheckForEndOfCyclePause();
                }

            }

        }


        if(g_nMBSlaveCommsWatchDogTimer)
        {
            g_nMBSlaveCommsWatchDogTimer--;
            if(g_nMBSlaveCommsWatchDogTimer == 0)   // on decrement to 0 call reset of slave comms.
            {
                g_bResetNetworkSlaveComms = TRUE;
            }
        }

        if(g_nMBSlaveNBBCommsWatchDogTimer)
        {
            g_nMBSlaveNBBCommsWatchDogTimer--;
        }
        if(g_nMBSlaveNBBCommsWatchDogTimer == 0)   // on decrement to 0 call reset of slave comms.
        {
            g_bResetPanelSlaveComms = TRUE;
            g_nBatchCommsPanelTimeoutRx++;
        }

        if(g_bSEIReadDataAvailable)
        {
            g_bSEIReadDataAvailable = FALSE;
            ReadSEIData();          // read SEI data
        }
}



    // Print data if necessary
    if( (g_nPIT_Int_Counter & 0x0001) == 1 )        // print on 2nd PIT's (every 2nd pit)
        ParallelPrintHandler();

    PITMBMHandler();        // Modbus master handling function for PIT. (see MBMastr.c)

    g_bPITHandlerInProgress = FALSE;        // clear flag to allow next PIT int to operate normally
  }
  else  // pit handler was in progress
  {
//    int i = 0;
  }

}


//////////////////////////////////////////////////////
// CheckForAToDError( void )
// check for a/d errors eoc, negative polarity
//
//////////////////////////////////////////////////////

void CheckForAToDError( WORD wStatus )
{
    if(wStatus != 0xffff)
    {
        if((wStatus & LTC2415_EOC_BIT)!= 0)  // check for eoc error
        {
            if(!g_bAtoDEndOfConversionErrorHistory)
            {
                PutAlarmTable( ATODENDOFCONVNOTCOMPLETE,  0 );
                g_bAtoDEndOfConversionErrorHistory = TRUE;
            }
        }
        else
        {
            if(g_bAtoDEndOfConversionErrorHistory)
            {
                RemoveAlarmTable( ATODENDOFCONVNOTCOMPLETE,  0 );
                g_bAtoDEndOfConversionErrorHistory = FALSE;
            }
        }
        if((wStatus & LTC2415_SIG_BIT) == 0)  // 0 means sign is negative
        {
            if(!g_bAtoDNegativeSignHistory)
            {
                //g_bDoAToDTest = TRUE;
                PutAlarmTable( ATODNEGATIVEPOLARITYALARM,  0 );
                g_bAtoDNegativeSignHistory = TRUE;
            }
        }
        else
        {
            if(g_bAtoDNegativeSignHistory)
            {
                RemoveAlarmTable( ATODNEGATIVEPOLARITYALARM,  0 );
                g_bAtoDNegativeSignHistory = FALSE;
            }
        }
    }
}




