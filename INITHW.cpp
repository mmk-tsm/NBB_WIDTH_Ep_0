//////////////////////////////////////////////////////
// InitialiseHardware
//
// Function initialises the RAM memory variables that
// hold the pointers to the hardware i/o addresses.
//
// M.Parks                              24-03-2000
// M.McKiernan                              14-08-2003
// Read SBB board address - g_cBlenderNetworkAddress = ReadCP2kAddress
// Defined PORTADataRegister.
// SBBIO_Latch1_Output_On/Off added.
// SBBIO_Latch15192_Output_On/Off added.
// ReadSBBIO_Input added.
// InitialiseNBBGpio();
//
//******************* For Batch Blender ***************************************

// M.McKiernan                              24-05-2004
// Removed stuff at the //bb  InitialiseModbusMaster();
// Added ToggleHBLED.
//
// M.McKiernan                              02-09-2004
// In MixerOff() g_bMixOnStatus = FALSE, had been set to TRUE
//
// M.McKiernan                              09-09-2004
// Added ReadHighLevelSensor, ReadLowLevelSensor
// M.McKiernan                     29-10-2001
//     #include "hc16.h"        // for Metrowerk CW - hc16.h copied into BatchBlender_c folder.
//  Added ReadSBBLinkOptions( )
// M.McKiernan                     08-11-2004
// Adding in stuff done by Paraic (everthing from CloseFeed() down.
// M.McKiernan                     18-11-2004
//      Corrections to OpendFeed and CloseFeed.
//      OpenFeedIfDiagnosticMode renamed, edited to OpenFeed1, similarly for CloseFeed1
// Added OpenBypass1( void ) void CloseBypass1( void ), ReadContactorInput(), BOOL ReadPauseInput( void )
// Added SBBSSROutputOn( void ), SBBSSROutputOff( void )
//
// M.McKiernan                     13-12-2004
//  Added AlarmOutputsOn() AlarmOutputsOff( ) AlarmBeaconOn( )
// Added AlarmBeaconOff( ), AlarmRelayOn( void ) AlarmRelayOff()
//
// M.McKiernan                     20-12-2004
// Added InitialiseSCISerialPort()
//
// M.McKiernan                     4-03-2005
// StartupOn/ Off routines inserted. No output assigned as there is a clash with component 10
// --review--
//
// P.Smith                          30-03-2005
// Change the sense of the way that the contactor input is read in ReadContactorInput()
//
// P.Smith                          1-06-2005
// Correct output being used by alarm beacon.should be using latch 1 output 5, not latch 2.
//
//
// P.Smith                          4/10/5
// define asm for ReadSBBLinkOptions
// add void SwitchComponentsOnOrOff( void )
//
// P.Smith                          3/11/5
// SetUpDiagnostics added for diagnostics set up.
// Check for g_bOutputDiagMode in CloseBypass
// check outputdiag for open / close bypass                        ;


//
// P.Smith                          19/1/06
// first pass at converting the software over to NBB hardware.
// add functions necessary to make i/p
// bBOOL -> BOOL g_bHeartBeatLed = FALSE;
// Bool -> BOOL
// comment out all hardware related access
// void PollForOuputDiagnostics( void ) implemented
// void CmpOn (int nComponent) implemented
// void CmpOff (int nComponent) implemented
// void SetUpDiagnostics( void ) implemented
// void AllOn( void )
// void AllOff( void )
//void CloseDump( void )
//void CloseDumpValve( void )
//void CloseDump1( void )
//void OpenDump( void )
//void OpenDumpValve( void )
//void OpenDump1( void )
//void MixerOn( void)
//void MixerOff( void)
//void OpenBypass( void )
//void OpenBypass1( void )
//void CloseBypass( void )
//void CloseBypass1( void )
//void AlarmRelayOn( void )
//void AlarmRelayOff( void )
//void AlarmBeaconOn( void )
//void AlarmBeaconOff( void )
//void AlarmOutputsOn( void )
//void AlarmOutputsOff( void )
//void SwitchComponentsOnOrOff( void )
//
// P.Smith                              23/1/06
// modified diagnostics for new hardware, the following has been
// debugged.
// Full output cycle.
// Four component cycle.
// Dual component cycle
// Dual slow component cycle.
// Single Valve cycle.
// P.Smith                              26/1/06
// The following were modified for nbb hardware,
// BOOL ReadLSensor( void )
// BOOL ReadHighLevelSensor( void )
// void ToggleHBLED( void )
// BOOL ReadStopRun( void )
//int ReadSBBIO_Input(unsigned char cBitMask)
//void ReadAllSBBIO_Inputs(void)
//BOOL ReadPauseInput( void )
//BOOL ReadPauseOffInput( void )
// corrected components 9 - 12, use SRC21Off() - SRC24Off(); in cmpon, cmpoff
// char ReadCP2kAddress(void)
//
//
//  P.Smith                             8/2/06
// call SetUpPITR in hardwareinitialisation.
// #include "Init5270.h", this holds the pit initialisation.
// #include <NBBGpio.h>, InitialiseNBBGpio();
// removed    int nTemp; in ReadCP2kAddress()
// added #include "I2CFuncs.h"
// added rtc initialisation.
// added  g_cBlenderNetworkAddress = ReadCP2kAddress();   // address is only a char.
//        InitialiseSerialPorts();
//        InitialiseModbusMaster();
// call     SetNBBInterrupts();            // set NBB interrupts
//
//  P.Smith                             20/2/06
// removed void SBBIO_Latch1_Output_On /off (int nOutput)
// removed void SBBIO_Latch1_Output_Off (int nOutput)
//
//  P.Smith                             20/2/06
// corrected read of level sensor in general.h
//
//  P.Smith                             21/3/06
// read links 1 & 2, set link 3 to TRUE to simulate installation.
//
//  P.Smith                             27/3/06
// call ReadSBBLinkOptions(); from initialise hardware
//
//  P.Smith                             31/5/06
// remove set of time on power up.
// correct various warnings.
//
//  P.Smith                             19/6/06
// swap over components 2 and 4 for the new blender piping.
//
//  P.Smith                             29/6/06
//  call set up of timer 2
//
//  P.Smith                             30/6/06
//  put source outputs back to original.
// allocate ssrop & start up outputs
//
//  P.Smith                             6/7/06
// call InitialiseU1SerialPorts();
// use l1 to invert dump output.
//
//  P.Smith                             20/9/06
// name change InitialisePanelSerialPort
//
// p.smith                                 13/11/06                ;
// Correct alarmon / off setting for sei alarm, alarmonflag set or ;
// cleared on alarmon /off call.
//
// p.smith                                 16/1/07                ;
// Allow for outputs for Rev B1
// Allocation for components 9,10
// Remove link check for dump valve orientation.
//
// p.smith                                 2/2/07                ;
// Remove     SetNBBInterrupts();
//
// P.smith                                 6/2/07                ;
// modified ReadSBBLinkOptions for new NBB RevB1 hardware
//
// P.smith                                 23/4/07                ;
// removed InitialisePCA9555();
//
// P.Smith                                 2/5/07                ;
// allow for expansion pcb for component 11 & 12
// activate outputs with mapping to allow any output to be mapped anywhere
//
// P.Smith                                 30/5/07                ;
// call expansion board initialisation.
// update expansion for components 9,10,11,12
// add bypass to expansion output 5.
//
// P.Smith                                 16/10/07                ;
// In ReadCP2kAddress, allow address to be 0
//
// P.Smith                                 28/1/08                ;
// added diagnostic print to hardware inialisaiton to determine where
// software is getting stuck on power up.
//
// P.Smith                          30/1/08
// correct compiler warnings
//
// P.Smith                          4/2/08
// add printf in intialisehardware before initialisation of serial ports.
//
// P.Smith                          7/5/08
// name change  BATCH_OFFLINEMODE to MODE_OFFLINEMODE
//
// P.Smith                          23/6/08
// Correct links l1,l2,l3 read
//
// P.Smith                          15/7/08
// add events for open / close bypass
//
// P.Smith                          26/9/08
// added InitTempSensor call
//
// P.Smith                          9/12/08
// call ResetVNC1L
//
// P.Smith                          12/1/09
// call InitialiseNBBEXMCP23S17 with address specified, MCP23S17_AT_ADDRESS_3
// initialise address at MCP23S17_AT_ADDRESS_4
//
// P.Smith                          15/1/09
// add InitialiseExpansion23S17 to allow the initialisation of the expansion card to
// be called when the blender is switched from stop to run.
//
// P.Smith                          25/3/10
// shorten printfs to hide what is being initialised
//
// P.Smith                          25/3/10
// in InitialiseHardware, check for the correct rtc.
// use calibration parameter m_bPCF8563RTCFitted to check if the old or
// new rtc should be enabled.
//
// P.Smith                          24/11/10
// check g_bCycleTestInProgress, if set dot use bypass
//////////////////////////////////////////////////////

//*****************************************************************************
// INCLUDES
//*****************************************************************************
//#include <hc16.h>
//nbb #include "hc16.h"       // for Metrowerks CW version (hc16.h in folder)
//nbb #include <StdLib.h>
//nbb #include "Hardware.h"

#include <basictypes.h>
#include "InitialiseHardware.h"
#include "NBBGpio.h"
#include <stdio.h>

//nbb#include "TimeDate.h"
//nbb# include "SPIFunctions.h"
//nbb# include "16R40C.h"
//nbb# include "InitComm.h"
//nbb# include "MBMHand.h"
//nbb #include "PrinterFunctions.h"

#include "BatVars.h"
#include "BatVars2.h"
#include "BatchCalibrationFunctions.h"
//nbb #include "SCIComms.h"
//nbb #include "PitWeigh.h"
#include "General.h"
#include "BatchMBIndices.h"
#include "Init5270.h"
#include <NBBGpio.h>
#include "I2CFuncs.h"
#include "Initcomms.h"
#include "MBMHand.h"
#include <cfinter.h>
#include "Nbbvars.h"
#include "InitNBBcomms.h"
#include "SDCard.h"
#include "MCP23Sexp.h"
#include "rta_system.h"
#include "Eventlog.h"
#include "VNC1L_g.h"
#include "ConfigurationFunctions.h"


structDaignosticsData arrsDiagnostics[100] ;

#define PORTADA  (*((int *)0xff706))    /* 16-Bit */                    // MMK

//nbb volatile  int   *far PORTADataRegister;

//nbb unsigned char   g_cBlenderNetworkAddress;



extern "C" {
/* This function sets up  the 5282 interup controller */
void SetIntc(long func, int vector, int level, int prio );  //from yahoo
void NMI_C_Part(); // The part of the ISR written in C/C++
void NMI_ASM_Part(); // The part of the ISR written in Assembly
}



unsigned char g_cSBBIOInputs;

//extern CalDataStruct    g_CalibrationData;
CalDataStruct	g_CalibrationData;
extern     BYTE   g_cIp1ToIp8,g_cLLS1ToLL8;
extern long	g_lcycleTestPitCtr;
extern BOOL g_bCycleTestInProgress;



BOOL g_bHeartBeatLed = FALSE;

BYTE g_nPCF8563Reg0B = 0x55;

void InitialiseHardware( void )
{
	bool bPCF8563Fitted = FALSE;
	bool bX1226Fitted = FALSE;

    iprintf("\ni");
    InitialiseNBBGpio();
    U0_Select_Mtty();
    iprintf("\np");
    SetUpPITR(2 /* USe PITR # 2 */,1536 /* Wait 1536 clocks */,4 /* Divide
                      by 32 */);
    iprintf("\ni2");
      I2CInit( 0, I2C_FREQ_DIV );	 //  from 147456000 clock.
    iprintf("\nr");
    LoadAllCalibrationDataFromEEprom();
    if(!g_CalibrationData.m_bPCF8563RTCFitted) // rtc fitted
    {  //old rtc x1226
	  bX1226Fitted = DetectX1226(); // old rtc
      if(!bX1226Fitted)
	  {  // old rtc not detected, look for new
		  bPCF8563Fitted = DetectRTCPCF8563();	// detect if RTC is a PCF8563.
		  if(bPCF8563Fitted)
		  {
//			  iprintf("\nset to old and new rtc detected");
			  g_CalibrationData.m_bPCF8563RTCFitted = TRUE;
			  SaveAllCalibrationDataToEEprom();
		  }
		  else
		  {
			  bPCF8563Fitted = DetectRTCPCF8563();	// try a second time
			  if(!bPCF8563Fitted)
			  {
	  	        	// generate an alarm
	          }
		  }
	  }
    }
    else   // new rtc
    {
	    bPCF8563Fitted = DetectRTCPCF8563();	// detect if RTC is a PCF8563.
		if(!bPCF8563Fitted)
	    {
			// look for old rtc
			bX1226Fitted = DetectX1226(); // old rtc
			if(bX1226Fitted)
			{
				g_CalibrationData.m_bPCF8563RTCFitted = FALSE;
				SaveAllCalibrationDataToEEprom();
			}
			else
			{
				// generate an alarm, old rtc not working
			}
	    }

    }
	  if(!g_CalibrationData.m_bPCF8563RTCFitted)
	  {
		  EnableRTCInterrupt();
	  }
	  else
	  {
		  InitialisePCF8563( );		// initialise PCF8563 RTC. 16/8/2010
	  }


      Init_EdgePort();    // initialise edge port pins.

      /* Now enable the actual interrupt controller. See users manual chapter 10 for
      more information. We are going to use the BSP helper function declared above and
      implemented in BSP.c */

      SetIntc(
      (long)&NMI_ASM_Part, /* my interrupt function */
      7, /* todo The vector number from the users manual table 10-13 */
      6, /* todo Set this to level 1 but any value from 1 to 6 would
      be valid.*/
      6 /* The priority within the gross levels; see chapter 10, any
      value from 0 to 7 is ok */
      );

      g_cBlenderNetworkAddress = ReadCP2kAddress();   // address is only a char.
      ReadSBBLinkOptions();
// Initialise the onboard temperature sensor
      InitTempSensor();

    // Initialise the DUART
      iprintf("\nsports");
      InitialiseSerialPorts();
      InitialisePanelSerialPort();
      SetupTimer2();                // set up DMA timer #2

    InitialiseModbusMaster();
    SPI_Select_EXPANSION1();
    InitialiseOnNBBMCP23S08( );
    InitialiseExpansion23S17();
    ResetVNC1L();
}
//////////////////////////////////////////////////////
// ReadCP2kAddress
//
// Reads the CP2000/Vac350 address from the SBB
//
// Entry:
//      none
// Exit:
//      address returned as unsigned char
// M.McKiernan                          11-06-2003
// First pass.
//////////////////////////////////////////////////////

char ReadCP2kAddress(void)
{
     unsigned char cAddr;

    // read board address.
    cAddr = ReadHexSwitch();
    return cAddr;
}

//////////////////////////////////////////////////////
//
// InitialiseExpansion23S17
// initialises expansion card.
//
//////////////////////////////////////////////////////

void InitialiseExpansion23S17(void)
{
    SPI_RS422_ENABLE_LOW();
    InitialiseNBBEXMCP23S17(MCP23S17_AT_ADDRESS_3);
    InitialiseNBBEXMCP23S17(MCP23S17_AT_ADDRESS_4);
    SPI_RS422_ENABLE_HIGH();
}


//////////////////////////////////////////////////////
// ReadSBBLinkOptions                  asm RD_OPTS
//
// Reads the option links (L2- L5 from the SBB
//
// Entry:
//      none
// Exit:
//      option link settings in BOOL flags (g_bSBBLxFitted)
// M.McKiernan                          04-11-2004
// First pass.
//////////////////////////////////////////////////////
void ReadSBBLinkOptions(void)
{
   unsigned int nTemp;
    nTemp = 	*(NBB_IPBUFFER);

   // link l1
   if((nTemp & LINKS_L1) == 0)
   {
      g_bL1Fitted = TRUE;
   }
   else
   {
      g_bL1Fitted = FALSE;   //nbb--todolp-- use this link for something.
   }
   if((nTemp & LINKS_L2) == 0)
   {
      g_bL2Fitted = TRUE;
  }
   else
   {
      g_bL2Fitted = FALSE;
   }
   if((nTemp & LINKS_L3) == 0)
   {
      g_bL3Fitted = TRUE;
  }
   else
   {
      g_bL3Fitted = FALSE;
   }

   g_bSBBL5Fitted = TRUE;         // nbb--todolp-- read this link from somewhere pauses blender on reset

}


//////////////////////////////////////////////////////
// FillValve1On
//
// Turns on the fill valve #1 output on SBBIO
//
// M.McKiernan                          28-11-2003
// First pass.
//////////////////////////////////////////////////////
/*nbb unused
void FillValve1On (void)
{
    *(SBBIO_OPLATCH_1 + Q0) = SBBIO_OUTPUT_ON;
}
nbb */
//////////////////////////////////////////////////////
// FillValve1Off
//
// Turns on the fill valve #1 output on SBBIO
//
// M.McKiernan                          28-11-2003
// First pass.
//////////////////////////////////////////////////////
void FillValve1Off (void)
{
/*nbb
    *(SBBIO_OPLATCH_1 + Q0) = SBBIO_OUTPUT_OFF;
nbb*/
}

//////////////////////////////////////////////////////
// SBBIO_Latch1_Output_On
//
// Turns on the specified latch #1 output on SBBIO  - output no is 0-7.
//
// M.McKiernan                          28-11-2003
// First pass.
//////////////////////////////////////////////////////
/*nbb
void SBBIO_Latch1_Output_On (int nOutput)
{
    if(nOutput <= 7)
    {
        *(SBBIO_OPLATCH_1 + nOutput) = SBBIO_OUTPUT_ON;
    }
}
nbb*/
//////////////////////////////////////////////////////
// SBBIO_Latch1_Output_Off
//
// Turns of the specified latch #1 output on SBBIO  - output no is 0-7.
//
// M.McKiernan                          28-11-2003
// First pass.
//////////////////////////////////////////////////////
/*nbb
void SBBIO_Latch1_Output_Off (int nOutput)
{
    if(nOutput <= 7)
    {
        *(SBBIO_OPLATCH_1 + nOutput) = SBBIO_OUTPUT_OFF;
    }
}
nbb*/

//////////////////////////////////////////////////////
// SBBIO_Latch2_Output_On
//
// Turns on the specified latch #2 output on SBBIO  - output no is 0-7.
//
// M.McKiernan                          28-11-2003
// First pass.
//////////////////////////////////////////////////////
//void SBBIO_Latch2_Output_On (int nOutput)
//{
/*nbb
    if(nOutput <= 7)
    {
        *(SBBIO_OPLATCH_2 + nOutput) = SBBIO_OUTPUT_ON;
    }
}
nbb*/
//////////////////////////////////////////////////////
// SBBIO_Latch2_Output_Off
//
// Turns of the specified latch #2 output on SBBIO  - output no is 0-7.
//
// M.McKiernan                          28-11-2003
// First pass.
//////////////////////////////////////////////////////
//void SBBIO_Latch2_Output_Off (int nOutput)
/*nbb
{
    if(nOutput <= 7)
    {
        *(SBBIO_OPLATCH_2 + nOutput) = SBBIO_OUTPUT_OFF;
    }
}
nbb*/

//////////////////////////////////////////////////////
// CmpOn(i)
//
// Turns on the output for the specified component for a batch blender  - Component no is 0-MAX_COMPONENTS.
// Note, the component no. passed to the routine is 1 based, first component = 1.
//
// M.McKiernan                          17-06-2004
// First pass.
//////////////////////////////////////////////////////
void CmpOn (int nComponent)
{

    if(nComponent <= MAX_COMPONENTS)                // Dont do anything if not a valid component.
    {
        switch( nComponent )
        {
        case 1:
            SRCOnWithMapping(1);
            break;
        case 2:
            SRCOnWithMapping(2);
            break;
        case 3:
            SRCOnWithMapping(3);
            break;
        case 4:
            SRCOnWithMapping(4);
            break;

        case 5:
            SRCOnWithMapping(5);
            break;
        case 6:
            SRCOnWithMapping(6);
            break;
        case 7:
            SRCOnWithMapping(7);
            break;
        case 8:
            SRCOnWithMapping(8);
            break;
       case 9:
            SRCOnWithMapping(9);
            g_nExpansionOutput = g_nExpansionOutput | EXPANSIONOUTPUT1BITPOS; // set bit position for expansion PCB
            break;
        case 10:
            SRCOnWithMapping(10);
            g_nExpansionOutput = g_nExpansionOutput | EXPANSIONOUTPUT2BITPOS; // set bit position for expansion PCB
            break;
        case 11:
            g_nExpansionOutput = g_nExpansionOutput | EXPANSIONOUTPUT3BITPOS; // set bit position for expansion PCB
            break;
        case 12:
            g_nExpansionOutput = g_nExpansionOutput | EXPANSIONOUTPUT4BITPOS; // set bit position for expansion PCB
            break;
        default:
            break;

        }  // end of switch statement.

        g_bFillIsOnCmp[nComponent - 1] = TRUE;                  // set flag to indicate component fill is ON.

    }
}

//////////////////////////////////////////////////////
// CmpOff(i)
//
// Turns off the output for the specified component for a batch blender  - Component no is 0-MAX_COMPONENTS.
// Note, the component no. passed to the routine is 1 based, first component = 1.
//
// M.McKiernan                          17-06-2004
// First pass.
//////////////////////////////////////////////////////
void CmpOff (int nComponent)
{
int     nTemp;
    if(nComponent <= MAX_COMPONENTS)                // Dont do anything if not a valid component.
    {
        switch( nComponent )
        {
        case 1:
           SRCOffWithMapping(1);
           break;
        case 2:
          SRCOffWithMapping(2);
             break;
        case 3:
          SRCOffWithMapping(3);
             break;
        case 4:
           SRCOffWithMapping(4);
            break;

        case 5:
           SRCOffWithMapping(5);
            break;
        case 6:
           SRCOffWithMapping(6);
            break;
        case 7:
           SRCOffWithMapping(7);
            break;
        case 8:
           SRCOffWithMapping(8);
            break;

        case 9:
           SRCOffWithMapping(9);
            nTemp = EXPANSIONOUTPUT1BITPOS ^ 0xFFFF;    // COM of bit position
            g_nExpansionOutput &= nTemp;
            break;
        case 10:
           SRCOffWithMapping(10);
            nTemp = EXPANSIONOUTPUT2BITPOS ^ 0xFFFF;    // COM of bit position
            g_nExpansionOutput &= nTemp;
            break;
        case 11:
            nTemp = EXPANSIONOUTPUT3BITPOS ^ 0xFFFF;    // COM of bit position
            g_nExpansionOutput &= nTemp;
            break;
        case 12:
            nTemp = EXPANSIONOUTPUT4BITPOS ^ 0xFFFF;    // COM of bit position
            g_nExpansionOutput &= nTemp;
            break;
        default:
            break;

        }  // end of switch statement.

        g_bFillIsOnCmp[nComponent - 1] = FALSE;                 // set flag to indicate component fill is OFF.
    }
}

//////////////////////////////////////////////////////
// CloseDump()
//
// Closes the dump valve
// Flag g_bDumpStatus is cleared (set  to FALSE)
// Dump flap driven by 5th o/p (+ 4) on 1st latch.
//
// M.McKiernan                          18-06-2004
// First pass.
//////////////////////////////////////////////////////
void CloseDump( void )
{
    if(g_bOutputDiagMode == FALSE)          // dont use if in Output Diagnostics mode.
    {
        g_bDumpStatus = FALSE;
        SRCOnWithMapping(15);
    }


}

//////////////////////////////////////////////////////
// CloseDumpValve()
//
// Close Dump valve is the same as close dump, except it does not check for diagnostics mode.
// Flag g_bDumpStatus is cleared (set  to FALSE)
// Dump flap driven by 5th o/p (+ 4) on 1st latch.
//
// M.McKiernan                          18-06-2004
// First pass.
//////////////////////////////////////////////////////
void CloseDumpValve( void )
{
        g_bDumpStatus = FALSE;
        SRCOnWithMapping(15);
}

//////////////////////////////////////////////////////
// CloseDump1()
//
// Turns off the dump valve solenoid.
// Flag g_bDumpStatus cleared., and no check for diagnostics mode. - Not dependant on blender type.
// Dump flap driven by 5th o/p (+ 4) on 1st latch.
//
// M.McKiernan                          18-06-2004
// First pass.
//////////////////////////////////////////////////////
void CloseDump1( void )

{
        g_bDumpStatus = FALSE;
//nbb        *(SBBIO_OPLATCH_1 + 4) = SBBIO_OUTPUT_OFF;      // for all other blender types close solenoid valve
        SRCOnWithMapping(15);
 //
}

//////////////////////////////////////////////////////
// OpenDump()
//
// Opens the dump valve
// Flag g_bDumpStatus is set (set  to TRUE)
// Dump flap driven by 5th o/p (+ 4) on 1st latch.
//
// M.McKiernan                          18-06-2004
// First pass.
//////////////////////////////////////////////////////
void OpenDump( void )
{
    if(g_bOutputDiagMode == FALSE)          // dont use if in Output Diagnostics mode.
    {
        g_bDumpStatus = TRUE;
        SRCOffWithMapping(15);
    }
}

//////////////////////////////////////////////////////
// OpenDumpValve()
//
// Open Dump valve is the same as Open dump, except it does not check for diagnostics mode.
// Flag g_bDumpStatus is set  to TRUE
// Dump flap driven by 5th o/p (+ 4) on 1st latch.
//
// M.McKiernan                          18-06-2004
// First pass.
//////////////////////////////////////////////////////
void OpenDumpValve( void )
{
        g_bDumpStatus = TRUE;
        SRCOffWithMapping(15);
}

//////////////////////////////////////////////////////
// OpenDump1()
//
// Turns on the dump valve solenoid.
// Flag g_bDumpStatus cleared., and no check for diagnostics mode. - Not dependant on blender type.
// Dump flap driven by 5th o/p (+ 4) on 1st latch.
//
// M.McKiernan                          18-06-2004
// First pass.
//////////////////////////////////////////////////////
void OpenDump1( void )
{
        g_bDumpStatus = TRUE;
        SRCOffWithMapping(15);
}

//////////////////////////////////////////////////////
// MixerOn()                            from ASM = MIXERON.
//
// Turns on the mixer o/p if not in diagnostics mode.
// Flag g_bMixOnStatus set
//
// M.McKiernan                          23-07-2004
// First pass.
//////////////////////////////////////////////////////
void MixerOn( void)
{

    if( !g_bOutputDiagMode )            // dont use if in Output Diagnostics mode.
    {
        g_bMixOnStatus = TRUE;
        SRCOnWithMapping(16);
    }
}

//////////////////////////////////////////////////////
// MixerOff()                           from ASM = MIXEROFF.
//
// Turns off the mixer o/p if not in diagnostics mode, or Throughput monitor.
// Flag g_bMixOnStatus cleared
//
// M.McKiernan                          23-07-2004
// First pass.
//////////////////////////////////////////////////////
void MixerOff( void)
{
    // dont use if in Output Diagnostics mode OR if Throughput monitor.
    if( !g_bOutputDiagMode  && g_CalibrationData.m_nBlenderMode != MODE_THROUGHPUTMON )
    {
        g_bMixOnStatus = FALSE;
        SRCOffWithMapping(16);
    }
}
/*
; MIXER SWITHCED ON.
MIXERON:
        TST     OUTPUTDIAG      ; DIAGNOSTICS IN PROGRESS ?
        BNE     XITMOFF
        LDAA    #1
        STAA    MIXON           ; INDICATE MIXER ON
L1Q7ON:
        JSR     ADL1Q7_ON       ; CONTACTOR ON.

;        LDAA    #1
;        STAA    BYPASSSTATUS
XITMOFF RTS


; MIXER SWITCHED OFF.
MIXEROFF:
        TST     OUTPUTDIAG      ; DIAGNOSTICS IN PROGRESS ?
        BNE     XITMOF
NOTDIMO:
        LDAA    BLENDERMODE     ; CHECK MODE
        CMPA    #THROUGHPUTMON  ; OFFLINE MODE
        BEQ     XITMOF          ; SKIP
        CLR     MIXON           ; INDICATE MIXER ON.
;        TST     AUTOCYCLEFLAG   ; AUTOCYCLE.
;        BNE     XITMOF
L1Q7OFF:
        JSR     ADL1Q7_OFF      ; CONTACTOR OFF.
XITMOF  RTS

*/
/*
CLOSEDUMP:
        TST     OUTPUTDIAG      ; DIAGNOSTICS IN PROGRESS ?
        BNE     XITDOFF
        CLR     DUMPSTATUS

CLOSEDUMPVALVE:
        TST     OUTPUTDIAG      ; DIAGNOSTICS IN PROGRESS ?
        BNE     XITDOFF

        LDAA    BLENDERTYPE
        CMPA    #TINYTYPE
        BEQ     ISMCDMPCL         ; MICRABATCH DUMP
        CMPA    #MIDITYPE
        BEQ     ISMCDMPCL         ; MIDI
        CMPA    #BABYTYPE
        BEQ     ISMCDMPCL         ; BABY BLENDER

L1Q4OFF:
CLOSEDUMP1:
        JSR     ADL1Q4_OFF      ; LATCHED OUPUT OFF.
XITDOFF RTS

ISMCDMPCL:
         JSR     ADL1Q4_ON      ; LATCH1 1 OUTPUT 4 ON
         BRA    XITDOFF

; OPEN DUMP FLAP.
OPENDUMP:
        TST     OUTPUTDIAG      ; DIAGNOSTICS IN PROGRESS ?
        BNE     XITDON
        LDAA    #$0F
        STAA    DUMPSTATUS
OPENDUMPVALVE:
        LDAA    BLENDERTYPE
        CMPA     #TINYTYPE
        BEQ     ISMCDMPOP       ; MICRABATCH DUMP
        CMPA     #MIDITYPE
        BEQ     ISMCDMPOP       ; MICRABATCH DUMP
        CMPA    #BABYTYPE
        BEQ     ISMCDMPOP       ; MICRABATCH DUMP


L1Q4ON:

OPENDUMP1:
         JSR     ADL1Q4_ON               ; LATCH1 1 OUTPUT 4 ON
         LDAA    #$0F
         STAA    DUMPSTATUS
XITDON   RTS

ISMCDMPOP:
         JSR     ADL1Q4_OFF              ; LATCH1 1 OUTPUT 4 ON
         BRA    XITDON

*/

//////////////////////////////////////////////////////
// ReadAllSBBIO_Inputs
//
// Reads the state of the all inputs on SBBIO  - .
//
// M.McKiernan                          25-06-2004
// First pass.
//////////////////////////////////////////////////////
void ReadAllSBBIO_Inputs(void)
{

   g_cSBBIOInputs = *(NBB_IPBUFFER);
}

/////////////////////////////////////////////////////
// ReadLSensor
//
// Reads the state of the level sensor input on SBBIO  - .
//
// M.McKiernan                          24-06-2004
// First pass.
//////////////////////////////////////////////////////
BOOL ReadLSensor( void )
{
  BOOL bState = COVERED;

   g_cSBBIOInputs = *(NBB_IPBUFFER);
   if(g_cSBBIOInputs & HW_LSENSORBIT)
        bState = UNCOVERED;                         // input active (COVERED) results in a 0, THEREFORE IF HIGH return an uncoverd.

    return(bState);
}
/*
READLSENSOR:
        LDAB   #HARDWAREBANK                   ; HARDWARE BANK
        TBZK                                   ; STORE BANK NO
        LDZ     #RDATABUF
        LDAA    0,Z                             ; READ DATA
;        LDAA     RDATABUF               ; READ
        COMA                           ;
        ANDA     #LSENSORBIT            ; 0 FOR ACTIVE LSENSOR
        RTS

*/


/////////////////////////////////////////////////////
// ReadHighLevelSensor
//
// Reads the state of the high level sensor input on SBBIO  - .
//
// M.McKiernan                          09-09-2004
// First pass.
//////////////////////////////////////////////////////
BOOL ReadHighLevelSensor( void )
{
  BOOL bState = COVERED;

   g_cSBBIOInputs = *(NBB_IPBUFFER); //nbb--todocheck-- allocate this input
   if(g_cSBBIOInputs & HW_HIGHLSENSORBIT)
        bState = UNCOVERED;                         // input active (COVERED) results in a 0, THEREFORE IF HIGH return an uncoverd.

    return(bState);
}

/*
; READ HIGH LEVEL SENSOR BIT.

READHIGHLEVELSENSOR:
        LDAB   #HARDWAREBANK                   ; HARDWARE BANK
        TBZK                                   ; STORE BANK NO
        LDZ     #RDATABUF
        LDAA    0,Z                             ; READ DATA
;        LDAA    RDATABUF
        COMA                           ;
        ANDA    #HIGHLSENSORBIT
        RTS


*/
/////////////////////////////////////////////////////
// ReadLowLevelSensor
//
// Reads the state of the low level sensor input on SBBIO  - .
//
// M.McKiernan                          09-09-2004
// First pass.
//////////////////////////////////////////////////////
BOOL ReadLowLevelSensor( void )
{
  BOOL bState = COVERED;

   g_cSBBIOInputs = *(NBB_IPBUFFER); //nbb--todocheck-- allocate this input
   if(g_cSBBIOInputs & HW_LOWLSENSORBIT)
        bState = UNCOVERED;                         // input active (COVERED) results in a 0, THEREFORE IF HIGH return an uncoverd.

    return(bState);
}

/*
; READ LOW LEVEL SENSOR LEVEL SENSOR #2 IN MCU16 SYSTEM..
READLOWLEVELSENSOR:
        LDAB   #HARDWAREBANK                   ; HARDWARE BANK
        TBZK                                   ; STORE BANK NO
        LDZ     #RDATABUF
        LDAA    0,Z                             ; READ DATA
;        LDAA    RDATABUF
        COMA                           ;
        ANDA    #LOWLSENSORBIT         ; 0 FOR ACTIVE LSENSOR
        RTS


*/


//////////////////////////////////////////////////////
// ToggleHBLED
//
// Toggles the heartbeat LED on and off
//
// M.McKiernan                          09-06-2004
// First pass.
//////////////////////////////////////////////////////
void ToggleHBLED( void )
{
           if( g_bHeartBeatLed == TRUE )
        {
            g_bHeartBeatLed = FALSE;
            SetDT1OUT_Lo();                 // led on

        }
        else
        {
            g_bHeartBeatLed = TRUE;
            SetDT1OUT_Hi();                // led off

        }
}
/////////////////////////////////////////////////////
// ReadStopRun()                from ASM =  READSTOPRUN
//
// Reads the state of Stop/Run switch on SBB console
//
// M.McKiernan                          29-07-2004
// First pass.
//////////////////////////////////////////////////////
BOOL ReadStopRun( void )
{
BOOL bState = RUN;
    g_cSBBIOInputs = *(NBB_IPBUFFER); //nbb--todocheck-- allocate this input
   if(g_cSBBIOInputs & HW_RUNSTOPBIT)
        bState = STOP;                          // --REVIEW-- input active (Switch closed= RUN) results in a 0

    return( bState );
}

/*
; READS STOP RUN MODE.
;
READSTOPRUN:
        LDAB   #HARDWAREBANK                   ; HARDWARE BANK
        TBZK                                   ; STORE BANK NO
        LDZ     #RDATABUF
        LDAA    0,Z                             ; READ DATA
;        LDAA    RDATABUF
        COMA
        ANDA    #RUNSTOPBIT
        RTS

*/
/////////////////////////////////////////////////////
// ReadContactorInput()             from ASM =  READCIN
//
// Reads the state of contactor input on SBB console (mixer motor contactor or trip)
//
// EXIT:    Returns RUN if contactor closed, or STOP if tripped out.
//
// M.McKiernan                          19-11-2004
// First pass.
//////////////////////////////////////////////////////
BOOL ReadContactorInput( void )
{
  BOOL bState = STOP;

   g_cSBBIOInputs = *(NBB_IPBUFFER); //nbb--todocheck-- allocate this input
   if(g_cSBBIOInputs & HW_CINBIT)
        bState = RUN;                          // --REVIEW-- input active

    return( bState );
}

/*
READCIN:
        LDAB   #HARDWAREBANK                   ; HARDWARE BANK
        TBZK                                   ; STORE BANK NO
        LDZ     #RDATABUF
        LDAA    0,Z                             ; READ DATA
;        LDAA     RDATABUF               ; READ
        COMA                           ;
        ANDA    #CINBIT                 ; CONTACTOR OVERLOADED.
        RTS

*/




/////////////////////////////////////////////////////
// ReadScrewDetection()             from ASM =  READSCREWDETECTION
//
// Read screw detection
//
// P.Smith                          19-11-2004
//////////////////////////////////////////////////////
BOOL ReadScrewDetection ( void )
{
 BOOL bState = TRUE;
   g_cSBBIOInputs = *(NBB_IPBUFFER); //nbb--todocheck-- allocate this input

   if(g_cSBBIOInputs & HW_SCREWDETECTIONBIT)
        bState = FALSE;                          //
    return( bState );
}

/*
READSCREWDETECTION:
READOI1CONN61:
        LDAB   #HARDWAREBANK                   ; HARDWARE BANK
        TBZK                                   ; STORE BANK NO
        LDZ     #RDATABUF
        LDAA    0,Z                             ; READ DATA
;        LDAA    RDATABUF
        COMA                             ; ZERO WHEN ACTIVE, MUST BE INVERTED
        ANDA    #IO1CONN60BIT           ;
        RTS
*/


/////////////////////////////////////////////////////
// ReadPauseInput()             from ASM =  READPAUSEINPUT
//
// Reads the state of pause input on SBB console
//
// EXIT: returns true if pause input activated.
//
// M.McKiernan                          19-11-2004
// First pass.
//////////////////////////////////////////////////////
BOOL ReadPauseInput( void )
{
  BOOL bState = TRUE;

   g_cSBBIOInputs = *(NBB_IPBUFFER); //nbb--todocheck-- allocate this input
   if(g_cSBBIOInputs & HW_PAUSEBIT)
        bState = FALSE;                         // --REVIEW-- input active -> 0

    return( bState );
}


/*
READPAUSEINPUT:
READOI3CONN60:
        LDAB   #HARDWAREBANK                   ; HARDWARE BANK
        TBZK                                   ; STORE BANK NO
        LDZ     #RDATABUF
        LDAA    0,Z                             ; READ DATA
;        LDAA    RDATABUF
        COMA                            ; CHANGE STATE 1 INDICATES ACTIVE.
        ANDA    #IO3CONN60BIT
        RTS

*/



/////////////////////////////////////////////////////
// ReadPauseInput()             from ASM =  READPAUSEINPUT
//
//
// P.Smith                          6-4-2005
//
//////////////////////////////////////////////////////

BOOL ReadPauseOffInput( void )
{
  BOOL bState = TRUE;

   g_cSBBIOInputs = *(NBB_IPBUFFER); //nbb--todocheck-- allocate this input

   if(g_cSBBIOInputs & HW_PAUSEOFFBIT)
        bState = FALSE;                         // --REVIEW-- input active -> 0

    return( bState );
}




/*
READOI2CONN60:
        LDAB   #HARDWAREBANK                   ; HARDWARE BANK
        TBZK                                   ; STORE BANK NO
        LDZ     #RDATABUF
        LDAA    0,Z                             ; READ DATA
;        LDAA    RDATABUF
        COMA                            ; CHANGE STATE 1 INDICATES ACTIVE.
        ANDA    #IO2CONN60BIT
        RTS

*/

//////////////////////////////////////////////////////
// CloseFeed
//
// Closes the offline feed valve
//
// P.Smith                              08-11-2004
// First pass.
//////////////////////////////////////////////////////
void CloseFeed( void )
{
       if( !g_bOverrideOutputs && !g_bOutputDiagMode)
        {
            SRCOnWithMapping(12); //nbb--todocheck-- check this output
            g_bFeedValveClosed = TRUE;
        }
}


//////////////////////////////////////////////////////
// CloseFeed1          ASM = CLOSEFEED1
//
// Closes the offline feed valve, no check for override
//
// P.Smith                              08-11-2004
// First pass.
//////////////////////////////////////////////////////
void CloseFeed1( void )
{

       if ( !g_bOutputDiagMode)
    {
        SRCOnWithMapping(12);//nbb--todocheck-- check this output
        g_bFeedValveClosed = TRUE;
    }
}



/*
; OFFLINE FEED UNDERNEATH THE BLENDER.



CLOSEFEED:
        TST     OVERRIDEOUTPUTS
        BNE     XITCLF
CLOSEFEED1:
L2Q6ON:
        TST     OUTPUTDIAG      ; DIAGNOSTICS IN PROGRESS ?
        BNE     XITCLF
        JSR     ADL2Q6_ON       ; LATCH OUTPUT ON.
        LDAA    #$0F
        STAA    FEEDCLOSE
XITCLF  RTS


*/
//////////////////////////////////////////////////////
// OpenFeed                             ASM = OPENFEED
//
// Opens the offline feed valve underneath the blender.
//
// P.Smith                              08-11-2004
// First pass.
// M.McKiernan                          18-11-2004
// Corrections
//////////////////////////////////////////////////////
void OpenFeed( void )
{
        if( !g_bOverrideOutputs && !g_bOutputDiagMode )
        {
                SRCOffWithMapping(12);  //nbb--todocheck-- check this output
                g_bFeedValveClosed = FALSE;
        }
}
/*

; OPEN PINCH VALVE UNDERNEATH THE BLENDER.

OPENFEED:
L2Q6OFF:
        TST     OVERRIDEOUTPUTS
        BNE     XITOPF
OPENFEED1:
        TST     OUTPUTDIAG      ; DIAGNOSTICS IN PROGRESS ?
        BNE     XITOPF
        JSR     ADL2Q6_OFF      ; LATCH OUTPUT OFF.
        CLR     FEEDCLOSE
XITOPF  RTS

*/


//////////////////////////////////////////////////////
// OpenFeed1           ASM = OPENFEED1
//
// Opens the offline feed valve, no check for override
//
// P.Smith                              08-11-2004
// First pass.
// M.McKiernan                          18-11-2004
// renamed, setting g_bFeedValveClosed status.
//////////////////////////////////////////////////////
void OpenFeed1( void )
{
    if(!g_bOutputDiagMode)
    {
        SRCOffWithMapping(12);  //nbb--todocheck-- check this output
//nbb        SBBIO_Latch2_Output_Off(6);
        g_bFeedValveClosed = FALSE;
    }
}
/*
OPENFEED1:
        TST     OUTPUTDIAG      ; DIAGNOSTICS IN PROGRESS ?
        BNE     XITOPF
        JSR     ADL2Q6_OFF      ; LATCH OUTPUT OFF.
        CLR     FEEDCLOSE
XITOPF  RTS
*/


//////////////////////////////////////////////////////
// OpenBypass                         ASM = OPENBYPASS
//
// Opens bypass valve if not overridden by diagnostic test
//
// P.Smith                              08-11-2004
// M.McKiernan                          18-11-2004
// Corrections
//////////////////////////////////////////////////////
void OpenBypass( void )
{
    if(!g_bOverrideOutputs && !g_bOutputDiagMode)
    {
            if(g_bBypassStatus == CLOSED)
            {
                AddSDCardBypassOpenedToEventLog();
            }
            g_nExpansionOutput = g_nExpansionOutput | EXPANSIONOUTPUT5BITPOS; // set bit position for expansion PCB
            SRCOnWithMapping(11); //nbb--todocheck-- check this output
            g_bBypassStatus = OPEN;
    }
}
//////////////////////////////////////////////////////
// OpenBypass1                         ASM = OPENBYPASS1
//
// Same as OpenBypass, except no check for over-ride
//
// M.McKiernan                          18-11-2004
// First Pass
//////////////////////////////////////////////////////
void OpenBypass1( void )
{
    if(g_bBypassStatus == CLOSED)
    {
        AddSDCardBypassOpenedToEventLog();
    }
    g_nExpansionOutput = g_nExpansionOutput | EXPANSIONOUTPUT5BITPOS; // set bit position for expansion PCB
    SRCOnWithMapping(11); //nbb--todocheck-- check this output
    g_bBypassStatus = OPEN;
}


//--review-- the naming for routine

/*
OPENBYPASS:
ALARM2ON:
RELAY01ON:
        TST     OVERRIDEOUTPUTS
        BNE     XITON           ; OVERRIDE
OPENBYPASS1:
        JSR     ADL2Q7_ON       ; LATCH OUTPUT ON.
        LDAA    #1
        STAA    BYPASSSTATUS    ; INDICATE CLOSED.
XITON   RTS


*/


//////////////////////////////////////////////////////
// CloseBypass                         ASM = CLOSEBYPASS
//
// Closes bypass valve if not overridden by diagnostic test
//
// P.Smith                              08-11-2004
// M.McKiernan                          18-11-2004
// Corrections
//////////////////////////////////////////////////////
void CloseBypass( void )
{
    int nTemp;
    if(!g_bOverrideOutputs && !g_bOutputDiagMode)
    {
        if(g_bBypassStatus == OPEN)
        {
            AddSDCardBypassClosedToEventLog();
        }
        nTemp = EXPANSIONOUTPUT5BITPOS ^ 0xFFFF;    // COM of bit position
        g_nExpansionOutput &= nTemp;
        SRCOffWithMapping(11); //nbb--todocheck-- check this output
        g_bBypassStatus = CLOSED;
    }
}
//////////////////////////////////////////////////////
// CloseBypass1                         ASM = CLOSEBYPASS1
//
// Same as CloseBypass, except no check for over-ride
//
// M.McKiernan                          18-11-2004
// First Pass
//////////////////////////////////////////////////////
void CloseBypass1( void )
{
    int nTemp;
    if(g_bBypassStatus == OPEN)
    {
        AddSDCardBypassClosedToEventLog();
    }
    nTemp = EXPANSIONOUTPUT5BITPOS ^ 0xFFFF;    // COM of bit position
    g_nExpansionOutput &= nTemp;
    SRCOffWithMapping(11); //nbb--todocheck-- check this output
    g_bBypassStatus = CLOSED;
}

/*

; CLOSE BYPASS VALVE.

; CLOSE BYPASS VALVE.
CLOSEBYPASS:
ALARM2OFF:
RELAY01OFF:
        TST     OVERRIDEOUTPUTS
        BNE     XITBPC          ;
CLOSEBYPASS1:
        JSR     ADL2Q7_OFF       ; LATCH OUTPUT OFF.
        CLR     BYPASSSTATUS    ; INDICATE CLOSED.
XITBPC  RTS

*/

//////////////////////////////////////////////////////
// AlarmRelayOn()                         ASM = ALARMON (PART OF)
//
// Activates the alarm relay.
//
// M.McKiernan                          13-12-2004
// First Pass
//////////////////////////////////////////////////////
void AlarmRelayOn( void )
{
    if( !g_bOutputDiagMode )            // dont use if in Output Diagnostics mode.
    {
        g_bAlarmRelayStatus = ON;                       // set flag to show alarm relay status.
        SRCOnWithMapping(13); //nbb--todocheck-- check this output
    }
}
//////////////////////////////////////////////////////
// AlarmRelayOff()                         ASM = ALARMOFF (PART OF)
//
// De-activates the alarm relay.
//
// M.McKiernan                          13-12-2004
// First Pass
//////////////////////////////////////////////////////
void AlarmRelayOff( void )
{

    if( !g_bOutputDiagMode )            // dont use if in Output Diagnostics mode.
    {
        g_bAlarmRelayStatus = OFF;                      // clear flag to show alarm relay status.
        SRCOffWithMapping(13); //nbb--todocheck-- check this output
    }

}

//////////////////////////////////////////////////////
// AlarmBeaconOn()                         ASM = ALARMON (PART OF)
//
// Activates the alarm beacon.
//
// M.McKiernan                          13-12-2004
// First Pass
//////////////////////////////////////////////////////
void AlarmBeaconOn( void )
{
    if( !g_bOutputDiagMode )            // dont use if in Output Diagnostics mode.
    {
        g_bAlarmBeaconStatus = ON;                      // set flag to show alarm beacon status.
//        SRC17On();    //nbb--todocheck-- no alarm beacon on rev b1
    }
}
//////////////////////////////////////////////////////
// AlarmBeaconOff()                         ASM = ALARMOFF (PART OF)
//
// De-activates the alarm beacon.
//
// M.McKiernan                          13-12-2004
// First Pass
//////////////////////////////////////////////////////
void AlarmBeaconOff( void )
{

    if( !g_bOutputDiagMode )            // dont use if in Output Diagnostics mode.
    {
        g_bAlarmBeaconStatus = OFF;                     // clear flag to show alarm beacon status.
//        SRC17Off(); //nbb--todoallocate-- no alarm beacon on rev b1
    }
}

//////////////////////////////////////////////////////
// AlarmOutputsOn()                         ASM = ALARMON
//
// Activates the alarm beacon & alarm relay
//
// M.McKiernan                          13-12-2004
// First Pass
//////////////////////////////////////////////////////
void AlarmOutputsOn( void )
{
    if(!g_bOutputDiagMode)
    {
        g_bAlarmOnFlag = TRUE;
        AlarmBeaconOn();
        AlarmRelayOn();
    }
}
//////////////////////////////////////////////////////
// AlarmOutputsOf()                         ASM = ALARMOFF
//
// de-activates the alarm beacon & alarm relay
//
// M.McKiernan                          13-12-2004
// First Pass
//////////////////////////////////////////////////////
void AlarmOutputsOff( void )
{

    if(!g_bOutputDiagMode)
    {
        g_bAlarmOnFlag = FALSE;
        AlarmBeaconOff();
        AlarmRelayOff();
    }
}




/****************************************************
* SwitchComponentsOnOrOff()          ASM = SWITCHCOMPONENTSONOROFF
* switch components on or off depending on the status of the actual status
* flags.
*
*
* P.Smith                          10/10/05
*****************************************************
*/

void SwitchComponentsOnOrOff( void )
{
    int nMask,nCompConfig;
    unsigned int i;
    if(!g_bOutputDiagMode || g_bAutoCycleFlag)          // diagnostic mode
    {
        for( i = 0; i < g_CalibrationData.m_nComponents; i++ )
        {
            if(g_bFillStatCmp[i])
            {
                if(g_CalibrationData.m_bStandardCompConfig)                     //STDCCFG
                {
                    CmpOn(i+1);
                }
                else        // i.e. non standard config
                {
                    nCompConfig = g_CalibrationData.m_nCompConfig[i];
                    if(nCompConfig != 0)
                    {
                        nMask = (0x0001);
                        for(i=0;  i < MAX_COMPONENTS; i++)
                        {
                            if(nCompConfig & nMask)
                            {
                                CmpOn(i + 1);         // CmpOn is 1 based (i.e. if i= 0, swith on comp. 1.
                            }
                        nMask <<= 1;                  // shift mask bit one to left
                        }
                    }
                    else
                    {
                        CmpOn(i+1);
                    }
                }
            }
            else  // SHBEOFF
            {

                if(g_CalibrationData.m_bStandardCompConfig)                     //STDCCFG
                {
                    CmpOff(i+1);
                }
                else        // i.e. non standard config
                {
                    nCompConfig = g_CalibrationData.m_nCompConfig[i];
                    if(nCompConfig != 0)
                    {
                        nMask = (0x0001);
                        for(i=0;  i < MAX_COMPONENTS; i++)
                        {
                            if(nCompConfig & nMask)
                            {
                                CmpOff( i + 1 );          // CmpOff is 1 based (i.e. if i= 0, swith on comp. 1.
                            }
                            nMask <<= 1;                  // shift mask bit one to left
                        }
                    }
                    else
                    {
                        CmpOff(i+1);
                    }
                }
            }
        }
    }
}








/*
ALARMON:
        TST     OUTPUTDIAG      ; DIAGNOSTICS IN PROGRESS ?
        BNE     XITALO
RELAY02ON:
        JSR     ADL1Q5_ON       ; BEACON ON.
        JSR     ADL2Q4_ON       ; ALARM RELAY ON.
XITALO  RTS

; ALARM RELAY OFF.
ALARMOFF:
        TST     OUTPUTDIAG      ; DIAGNOSTICS IN PROGRESS ?
        BNE     XITALOF
RELAY02OFF:
        JSR     ADL1Q5_OFF       ; BEACON OFF.
        JSR     ADL2Q4_OFF       ; ALARM RELAY OFF.
XITALOF RTS

*/
//////////////////////////////////////////////////////
// SBBSSROutputOn               ASM = SSROPLOW
//
// Turns SSR output on the SBB on.  CONN13 pin 8
//
//
// Entry:   none
// Exit:
//
// M.McKiernan                          01-12-2004
//
//////////////////////////////////////////////////////
void    SBBSSROutputOn( void )
{
//    SRC30On(); //nbb--todoallocate-- allocate this output
}

//////////////////////////////////////////////////////
// SBBSSROutputOff              ASM = SSROPHIGH
//
// Turns SSR output on the SBB off.     CONN13 pin 8
//
//
// Entry:   none
// Exit:
//
// M.McKiernan                          01-12-2004
//
//////////////////////////////////////////////////////
void    SBBSSROutputOff( void )
{
//         SRC30Off(); //nbb--todoallocate-- allocate this output
}



//////////////////////////////////////////////////////
// StartUpOn             ASM = STARTUPON
//
// Turns Startup on
//
//
// Entry:   none
// Exit:
//
// P.Smith                          4-3-2005
//
//////////////////////////////////////////////////////
void    StartUpOn( void )
{
    if( !g_bOutputDiagMode )            // dont use if in Output Diagnostics mode.
    {
//         SRC29On(); //nbb--todoallocate-- allocate this output
    }

}



//////////////////////////////////////////////////////
// StartUpOff            ASM = STARTUPOFF
//
// Turns Startup off
//
//
// Entry:   none
// Exit:
//
// P.Smith                          4-3-2005
//
//////////////////////////////////////////////////////
void    StartUpOff( void )
{
    if( !g_bOutputDiagMode )            // dont use if in Output Diagnostics mode.
    {
//         SRC29Off(); //nbb--todoallocate-- allocate this output
    }
}



//////////////////////////////////////////////////////
// SetUpDiagnostics                 ASM SETUPDIAGPTR
//
// Sets up the output cycle array structure
//
//
// P.Smith                          3-11-2005
//
//////////////////////////////////////////////////////
void SetUpDiagnostics( void )
{
    int i = 0;
    int x,j;
    if(g_CalibrationData.m_nDiagnosticsMode == DIAGNOSTICS_FULL_OUTPUT_CYCLE )           // asm TESTPROG1
    {

        arrsDiagnostics[i].m_nAction = DIAGNOSTICS_ALL_ON;
        arrsDiagnostics[i].m_nOutput = 0;
        i++;

        arrsDiagnostics[i].m_nAction = DIAGNOSTICS_ALL_OFF;
        arrsDiagnostics[i].m_nOutput = 0;
        i++;

        j = 1;
        for(x=0; x<16; x++)
        {
            arrsDiagnostics[i].m_nAction = DIAGNOSTICS_SINGLE_OUTPUT_ON;
            arrsDiagnostics[i].m_nOutput = j;
            i++;
            arrsDiagnostics[i].m_nAction = DIAGNOSTICS_SINGLE_OUTPUT_OFF;
            arrsDiagnostics[i].m_nOutput = j;
            i++;
            j++;
         }

         arrsDiagnostics[i].m_nAction = DIAGNOSTICS_FINISH_ACTION;
    }
    else
    if(g_CalibrationData.m_nDiagnosticsMode == DIAGNOSTICS_FOUR_COMPONENT_CYCLE )           // asm TESTPROG2
    {

        j = 1;
        for(x=0; x<4; x++)
        {
            arrsDiagnostics[i].m_nAction = DIAGNOSTICS_SINGLE_OUTPUT_ON;
            arrsDiagnostics[i].m_nOutput = j;
            i++;
            arrsDiagnostics[i].m_nAction = DIAGNOSTICS_SINGLE_OUTPUT_OFF;
            arrsDiagnostics[i].m_nOutput = j;
            i++;
            j++;
        }


        arrsDiagnostics[i].m_nAction = DIAGNOSTICS_SINGLE_OUTPUT_ON;
        arrsDiagnostics[i].m_nOutput = 15;
        i++;
        arrsDiagnostics[i].m_nAction = DIAGNOSTICS_SINGLE_OUTPUT_OFF;
        arrsDiagnostics[i].m_nOutput = 15;

        if(g_CalibrationData.m_nBlenderMode == MODE_OFFLINEMODE)
        {
        i++;
        arrsDiagnostics[i].m_nAction = DIAGNOSTICS_SINGLE_OUTPUT_ON;
        arrsDiagnostics[i].m_nOutput = 19;
        i++;
        arrsDiagnostics[i].m_nAction = DIAGNOSTICS_SINGLE_OUTPUT_OFF;
        arrsDiagnostics[i].m_nOutput = 19;
        i++;
        }
        arrsDiagnostics[i].m_nAction = DIAGNOSTICS_FINISH_ACTION;
    }

    else
    if(g_CalibrationData.m_nDiagnosticsMode == DIAGNOSTICS_DUAL_COMPONENT_CYCLE )           // asm TESTPROG3
    {
        arrsDiagnostics[i].m_nAction = DIAGNOSTICS_DUAL_FIRST_TWO_OUTPUTS_ON;
        arrsDiagnostics[i].m_nOutput = 0;

        i++;
        arrsDiagnostics[i].m_nAction = DIAGNOSTICS_DUAL_SECOND_TWO_OUTPUTS_ON;
        arrsDiagnostics[i].m_nOutput = 0;

        i++;
        arrsDiagnostics[i].m_nAction = DIAGNOSTICS_FINISH_ACTION;
    }
    else
    if(g_CalibrationData.m_nDiagnosticsMode == DIAGNOSTICS_DUAL_SLOW_COMPONENT_CYCLE )           // asm TESTPROG5
    {
        arrsDiagnostics[i].m_nAction = DIAGNOSTICS_DUAL_FIRST_TWO_OUTPUTS_ON;
        arrsDiagnostics[i].m_nOutput = 0;
        i++;
        arrsDiagnostics[i].m_nAction = DIAGNOSTICS_DUAL_FIRST_TWO_OUTPUTS_ON;
        arrsDiagnostics[i].m_nOutput = 0;
        i++;
        arrsDiagnostics[i].m_nAction = DIAGNOSTICS_DUAL_FIRST_TWO_OUTPUTS_ON;
        arrsDiagnostics[i].m_nOutput = 0;
        i++;
        arrsDiagnostics[i].m_nAction = DIAGNOSTICS_DUAL_FIRST_TWO_OUTPUTS_ON;
        arrsDiagnostics[i].m_nOutput = 0;
        i++;

        arrsDiagnostics[i].m_nAction = DIAGNOSTICS_DUAL_SECOND_TWO_OUTPUTS_ON;
        arrsDiagnostics[i].m_nOutput = 0;
        i++;
        arrsDiagnostics[i].m_nAction = DIAGNOSTICS_DUAL_SECOND_TWO_OUTPUTS_ON;
        arrsDiagnostics[i].m_nOutput = 0;
        i++;
        arrsDiagnostics[i].m_nAction = DIAGNOSTICS_DUAL_SECOND_TWO_OUTPUTS_ON;
        arrsDiagnostics[i].m_nOutput = 0;
        i++;
        arrsDiagnostics[i].m_nAction = DIAGNOSTICS_DUAL_SECOND_TWO_OUTPUTS_ON;
        arrsDiagnostics[i].m_nOutput = 0;

        i++;
        arrsDiagnostics[i].m_nAction = DIAGNOSTICS_FINISH_ACTION;
    }
    else
    if(g_CalibrationData.m_nDiagnosticsMode == DIAGNOSTICS_SINGLE_VALVE_CYCLE )           // asm TESTPROG6
    {
        arrsDiagnostics[i].m_nAction = DIAGNOSTICS_SINGLE_VALVE_CYCLE_ON;
        arrsDiagnostics[i].m_nOutput = 0;
        i++;
        arrsDiagnostics[i].m_nAction = DIAGNOSTICS_SINGLE_VALVE_CYCLE_OFF;
        arrsDiagnostics[i].m_nOutput = 0;

        i++;
        arrsDiagnostics[i].m_nAction = DIAGNOSTICS_FINISH_ACTION;
    }
}


/*


        LDAA    BLENDERMODE     ; CHECK MODE
        CMPA    #OFFLINEMODE    ; OFFLINE MODE
        BEQ     OFFLINE                 ; OFFLINE
        LDX     #ENDLOOKONLINE
        STX     ENDLOOKPTR              ; INDICATE END OF LOOKUP
        RTS


REP4CMP:
        FDB     ADL1Q0_ON
        FDB     ADL1Q0_OFF
        FDB     ADL1Q1_ON
        FDB     ADL1Q1_OFF
        FDB     ADL1Q2_ON
        FDB     ADL1Q2_OFF
        FDB     ADL1Q3_ON
        FDB     ADL1Q3_OFF
        FDB     ADL2Q0_ON
        FDB     ADL2Q0_OFF
        FDB     ADL2Q1_ON
        FDB     ADL2Q1_OFF
        FDB     ADL1Q4_ON
ENDLOOKONLINE:
        FDB     ADL1Q4_OFF
        FDB     ADL2Q6_ON
ENDLOOK:
        FDB     ADL2Q6_OFF

*/



//////////////////////////////////////////////////////
// PollForOuputDiagnostics                 ASM DODIAG:
//
// Poll for diagnostics
//
// P.Smith                          3-11-2005
//
//////////////////////////////////////////////////////
void PollForOuputDiagnostics( void )
{
    int nOutput,nCompNo;
    BOOL bFound;
    bFound = FALSE;

    if(g_bOutputDiagMode)            // only allow diagnostics if mode is set
    {

        if(arrsDiagnostics[g_nDiagnosticArrayIndex].m_nAction == DIAGNOSTICS_FINISH_ACTION)
        {
            g_nDiagnosticArrayIndex = 0;
        }

        nOutput = arrsDiagnostics[g_nDiagnosticArrayIndex].m_nOutput;

        switch(arrsDiagnostics[g_nDiagnosticArrayIndex].m_nAction)
        {
        case DIAGNOSTICS_ALL_ON:
            AllOn();
            bFound = TRUE;
                break;
        case DIAGNOSTICS_ALL_OFF:
            AllOff();
            bFound = TRUE;

                break;
        case DIAGNOSTICS_SINGLE_OUTPUT_ON:
            SRCOn(nOutput);
  //        *(SBBIO_OPLATCH_1 + nOutput) = SBBIO_OUTPUT_ON;      //
            bFound = TRUE;
                break;
        case DIAGNOSTICS_SINGLE_OUTPUT2_ON:
 //         *(SBBIO_OPLATCH_2 + nOutput) = SBBIO_OUTPUT_ON;      //
            SRCOn(nOutput);
           bFound = TRUE;
                break;
        case DIAGNOSTICS_SINGLE_OUTPUT_OFF:
 //         *(SBBIO_OPLATCH_1 + nOutput) = SBBIO_OUTPUT_OFF;     //
            SRCOff(nOutput);
               bFound = TRUE;
                break;
        case DIAGNOSTICS_SINGLE_OUTPUT2_OFF:
 //         *(SBBIO_OPLATCH_2 + nOutput) = SBBIO_OUTPUT_OFF;     //
            SRCOff(nOutput);
               bFound = TRUE;
                break;
        case DIAGNOSTICS_DUAL_FIRST_TWO_OUTPUTS_ON:
            CmpOn(1);
            CmpOn(2);
            CmpOff(3);
            CmpOff(4);
            bFound = TRUE;
                break;
        case DIAGNOSTICS_DUAL_SECOND_TWO_OUTPUTS_ON:
            CmpOn(3);
            CmpOn(4);
            CmpOff(1);
            CmpOff(2);
            bFound = TRUE;
                break;
        case DIAGNOSTICS_DUAL_FIRST_TWO_OUTPUTS_OFF:
            CmpOff(1);
            CmpOff(2);
            bFound = TRUE;
                break;
        case DIAGNOSTICS_DUAL_SECOND_TWO_OUTPUTS_OFF:
            CmpOff(3);
            CmpOff(4);
            bFound = TRUE;
                break;
        case DIAGNOSTICS_SINGLE_VALVE_CYCLE_ON:
            nCompNo = g_CalibrationData.m_nPurgeComponentNumber;
            if(nCompNo != g_nSingleCycleCompNoHis)               // check for component no change
            {
                g_nSingleCycleCompNoHis = nCompNo;
                AllOff();
            }

            if(nCompNo)
                {
                CmpOn(nCompNo);        // open single component
                }
            else
                OpenDump1();
            bFound = TRUE;
                break;
        case DIAGNOSTICS_SINGLE_VALVE_CYCLE_OFF:
            nCompNo = g_CalibrationData.m_nPurgeComponentNumber;

            if(nCompNo != g_nSingleCycleCompNoHis)               // check for component no change
            {
                g_nSingleCycleCompNoHis = nCompNo;
                AllOff();
            }
            if(nCompNo)
                CmpOff(nCompNo);        // close single component
            else
                CloseDump1();
            bFound = TRUE;
                break;
            default:
                break;
            }   // end of switch.

    }
    if(bFound)
    {
        g_nDiagnosticArrayIndex++;  // only increment counter if found
    }
}



//////////////////////////////////////////////////////
// AllOff                            ASM ALLOFF:
//
//
// P.Smith                          4-11-2005
//
//////////////////////////////////////////////////////
void AllOff( void )
{
    SRC1Off();
    SRC2Off();
    SRC3Off();
    SRC4Off();
    SRC5Off();
    SRC6Off();
    SRC7Off();
    SRC8Off();
    SRC9Off();
    SRC10Off();
    SRC11Off();
    SRC12Off();
    SRC13Off();
    SRC14Off();
    SRC15Off();
    SRC16Off();
}



//////////////////////////////////////////////////////
// AllOn                            ASM ALLON:
//
//
// P.Smith                          4-11-2005
//
//////////////////////////////////////////////////////
void AllOn( void )
{

    SRC1On();
    SRC2On();
    SRC3On();
    SRC4On();
    SRC5On();
    SRC6On();
    SRC7On();
    SRC8On();
    SRC9On();
    SRC10On();
    SRC11On();
    SRC12On();
    SRC13On();
    SRC14On();
    SRC15On();
    SRC16On();
}


/*

CMP1ON:
L1Q0ON:
        JSR     ADL1Q0_ON               ; LATCH1 1 OUTPUT 0 ON
        LDAA    #1
        STAA    FILLISONCMP1
        RTS

; COMPONENT #2 ACTIVATION.

CMP2ON:
L1Q1ON:
        JSR     ADL1Q1_ON               ; LATCH1 1 OUTPUT 1 ON
        LDAA    #1
        STAA    FILLISONCMP2
        RTS

; COMPONENT #3 ACTIVATION.

CMP3ON:
L1Q2ON:
        JSR     ADL1Q2_ON               ; LATCH1 1 OUTPUT 2 ON
        LDAA    #1
        STAA    FILLISONCMP3
        RTS

; COMPONENT #4 ACTIVATION.

CMP4ON:
L1Q3ON:
        JSR     ADL1Q3_ON               ; LATCH1 1 OUTPUT 3 ON
        LDAA    #1
        STAA    FILLISONCMP4
        RTS



; OFFLINE FEED UNDERNEATH THE BLENDER.



CLOSEFEED:
        TST     OVERRIDEOUTPUTS
        BNE     XITCLF
CLOSEFEED1:
L2Q6ON:
        TST     OUTPUTDIAG      ; DIAGNOSTICS IN PROGRESS ?
        BNE     XITCLF
        JSR     ADL2Q6_ON       ; LATCH OUTPUT ON.
        LDAA    #$0F
        STAA    FEEDCLOSE
XITCLF  RTS


; COMPONENT #5 ACTIVATION.(MOTOR #1)

CMP5ON:
L2Q0ON:
        JSR     ADL2Q0_ON       ; LATCH OUTPUT ON.
        LDAA    #1
        STAA    FILLISONCMP5
        RTS



; COMPONENT #6 ACTIVATION.(MOTOR #2)
CMP6ON:
L2Q1ON:
        JSR     ADL2Q1_ON       ; LATCH OUTPUT ON.
        LDAA    #1
        STAA    FILLISONCMP6
        RTS

; COMPONENT #7 ACTIVATION.(MOTOR #3)
CMP7ON:
L2Q2ON:
        JSR     ADL2Q2_ON       ; LATCH OUTPUT ON.
        LDAA    #1
        STAA    FILLISONCMP7
        RTS


; COMPONENT #8 ACTIVATION.(MOTOR #4)
CMP8ON:
L2Q3ON:
        JSR     ADL2Q3_ON       ; LATCH OUTPUT ON.
        LDAA    #1
        STAA    FILLISONCMP8
        RTS

; COMPONENT #9 ACTIVATION.(SPARE 1)

CMP9ON:
         JSR     ADL1Q6_ON
        LDAA    #1
        STAA    FILLISONCMP9
        RTS

; COMPONENT #10 ACTIVATION.(SPARE 2)

CMP10ON JSR     ADL2Q5_ON
        LDAA    #1
        STAA    FILLISONCMP10
        RTS

; COMPONENT #11 ACTIVATION.()

CMP11ON:
;        JSR     SSROPLOW
        JSR     EXPANOP2ON
        LDAA    #1
        STAA    FILLISONCMP11
        RTS

; COMPONENT #12 ACTIVATION.()

CMP12ON:
        JSR     EXPANOP1ON
        LDAA    #1
        STAA    FILLISONCMP12
        RTS


OPENBYPASS:
ALARM2ON:
RELAY01ON:
        TST     OVERRIDEOUTPUTS
        BNE     XITON           ; OVERRIDE
OPENBYPASS1:
        JSR     ADL2Q7_ON       ; LATCH OUTPUT ON.
        LDAA    #1
        STAA    BYPASSSTATUS    ; INDICATE CLOSED.
XITON   RTS


ALARMON:
        TST     OUTPUTDIAG      ; DIAGNOSTICS IN PROGRESS ?
        BNE     XITALO
RELAY02ON:
        JSR     ADL1Q5_ON       ; BEACON ON.
        JSR     ADL2Q4_ON       ; ALARM RELAY ON.
XITALO  RTS




; COMPONENT #1 DEACTIVATION.
CMP1OFF:
L1Q0OFF:
        JSR     ADL1Q0_OFF              ; LATCH OUTPUT OFF.
        CLR     FILLISONCMP1
        RTS

; COMPONENT #2 DEACTIVATION.
CMP2OFF:
L1Q1OFF:
        JSR     ADL1Q1_OFF              ; LATCH OUTPUT OFF.
        CLR     FILLISONCMP2
        RTS

; COMPONENT #3 DEACTIVATION.
CMP3OFF:
L1Q2OFF:
        JSR     ADL1Q2_OFF              ; LATCH OUTPUT OFF.
        CLR     FILLISONCMP3
        RTS


; COMPONENT #4 DEACTIVATION.
CMP4OFF:
L1Q3OFF:
        JSR     ADL1Q3_OFF              ; LATCH OUTPUT OFF.
        CLR     FILLISONCMP4
        RTS


; OPEN PINCH VALVE UNDERNEATH THE BLENDER.

OPENFEED:
L2Q6OFF:
        TST     OVERRIDEOUTPUTS
        BNE     XITOPF
OPENFEED1:
        TST     OUTPUTDIAG      ; DIAGNOSTICS IN PROGRESS ?
        BNE     XITOPF
        JSR     ADL2Q6_OFF      ; LATCH OUTPUT OFF.
        CLR     FEEDCLOSE
XITOPF  RTS

; COMPONENT #5 DEACTIVATION. (MOTOR #1)
CMP5OFF:
L2Q0OFF:
        JSR     ADL2Q0_OFF      ; LATCH OUTPUT OFF.
        CLR     FILLISONCMP5
        RTS

; COMPONENT #6 DEACTIVATION. (MOTOR #2)
CMP6OFF:
L2Q1OFF:
        JSR     ADL2Q1_OFF      ; LATCH OUTPUT OFF.
        CLR     FILLISONCMP6
        RTS


; COMPONENT #7 DEACTIVATION. (MOTOR #3)
CMP7OFF:
L2Q2OFF:
        JSR     ADL2Q2_OFF      ; LATCH OUTPUT OFF.
        CLR     FILLISONCMP7
        RTS

; COMPONENT #8 DEACTIVATION. (MOTOR #4)
CMP8OFF:
L2Q3OFF:
        JSR     ADL2Q3_OFF      ; LATCH OUTPUT OFF.
        CLR     FILLISONCMP8
        RTS


; COMPONENT #9 DEACTIVATION. (SPARE 1)

CMP9OFF:
         JSR     ADL1Q6_OFF:
         CLR     FILLISONCMP9
         RTS

; COMPONENT #10 DEACTIVATION. (SPARE 2)

CMP10OFF JSR     ADL2Q5_OFF:
         CLR     FILLISONCMP10
         RTS

; COMPONENT #11 DEACTIVATION. ()

CMP11OFF:
;         JSR     SSROPHIGH
         JSR     EXPANOP2OFF
         CLR     FILLISONCMP11
         RTS

; COMPONENT #12 DEACTIVATION. ()

CMP12OFF:
         JSR     EXPANOP1OFF
         CLR     FILLISONCMP12
         RTS

; CLOSE BYPASS VALVE.

; CLOSE BYPASS VALVE.
CLOSEBYPASS:
ALARM2OFF:
RELAY01OFF:
        TST     OVERRIDEOUTPUTS
        BNE     XITBPC          ;
CLOSEBYPASS1:
        JSR     ADL2Q7_OFF       ; LATCH OUTPUT OFF.
        CLR     BYPASSSTATUS    ; INDICATE CLOSED.
XITBPC  RTS


; ALARM RELAY OFF.
ALARMOFF:
        TST     OUTPUTDIAG      ; DIAGNOSTICS IN PROGRESS ?
        BNE     XITALOF
RELAY02OFF:
        JSR     ADL1Q5_OFF       ; BEACON OFF.
        JSR     ADL2Q4_OFF       ; ALARM RELAY OFF.
XITALOF RTS

; MIXER SWITCHED OFF.
MIXEROFF:
        TST     OUTPUTDIAG      ; DIAGNOSTICS IN PROGRESS ?
        BNE     XITMOF
NOTDIMO:
        LDAA    BLENDERMODE     ; CHECK MODE
        CMPA    #THROUGHPUTMON  ; OFFLINE MODE
        BEQ     XITMOF          ; SKIP
        CLR     MIXON           ; INDICATE MIXER ON.
;        TST     AUTOCYCLEFLAG   ; AUTOCYCLE.
;        BNE     XITMOF
L1Q7OFF:
        JSR     ADL1Q7_OFF      ; CONTACTOR OFF.
XITMOF  RTS





; OPEN DUMP FLAP.
OPENDUMP:
        TST     OUTPUTDIAG      ; DIAGNOSTICS IN PROGRESS ?
        BNE     XITDON
        LDAA    #$0F
        STAA    DUMPSTATUS
OPENDUMPVALVE:
        LDAA    BLENDERTYPE
        CMPA     #TINYTYPE
        BEQ     ISMCDMPOP       ; MICRABATCH DUMP
        CMPA     #MIDITYPE
        BEQ     ISMCDMPOP       ; MICRABATCH DUMP
        CMPA    #BABYTYPE
        BEQ     ISMCDMPOP       ; MICRABATCH DUMP


L1Q4ON:

OPENDUMP1:
         JSR     ADL1Q4_ON               ; LATCH1 1 OUTPUT 4 ON
         LDAA    #$0F
         STAA    DUMPSTATUS
XITDON   RTS

ISMCDMPOP:
         JSR     ADL1Q4_OFF              ; LATCH1 1 OUTPUT 4 ON
         BRA    XITDON

; MIXER SWITHCED ON.
MIXERON:
        TST     OUTPUTDIAG      ; DIAGNOSTICS IN PROGRESS ?
        BNE     XITMOFF
        LDAA    #1
        STAA    MIXON           ; INDICATE MIXER ON
L1Q7ON:
        JSR     ADL1Q7_ON       ; CONTACTOR ON.

;        LDAA    #1
;        STAA    BYPASSSTATUS
XITMOFF RTS


; CLOSE DUMP FLAP.
CLOSEDUMP:
        TST     OUTPUTDIAG      ; DIAGNOSTICS IN PROGRESS ?
        BNE     XITDOFF
        CLR     DUMPSTATUS

CLOSEDUMPVALVE:
        TST     OUTPUTDIAG      ; DIAGNOSTICS IN PROGRESS ?
        BNE     XITDOFF

        LDAA    BLENDERTYPE
        CMPA    #TINYTYPE
        BEQ     ISMCDMPCL         ; MICRABATCH DUMP
        CMPA    #MIDITYPE
        BEQ     ISMCDMPCL         ; MIDI
        CMPA    #BABYTYPE
        BEQ     ISMCDMPCL         ; BABY BLENDER

L1Q4OFF:
CLOSEDUMP1:
        JSR     ADL1Q4_OFF      ; LATCHED OUPUT OFF.
XITDOFF RTS

ISMCDMPCL:
         JSR     ADL1Q4_ON      ; LATCH1 1 OUTPUT 4 ON
         BRA    XITDOFF












; ACTIVATES THE START UP OUTPUT.
STARTUPON:
        TST     OUTPUTDIAG      ; DIAGNOSTICS IN PROGRESS ?
        BNE     NOSTON
        JSR     ADL2Q5_ON       ; START UP.
NOSTON  RTS



; DEACTIVATES THE START UP OUTPUT.
STARTUPOFF:
        TST     OUTPUTDIAG      ; DIAGNOSTICS IN PROGRESS ?
        BNE     NOSTOFF
        JSR     ADL2Q5_OFF      ; START UP.
NOSTOFF RTS

; READS STOP RUN MODE.
;
READSTOPRUN:
        LDAB   #HARDWAREBANK                   ; HARDWARE BANK
        TBZK                                   ; STORE BANK NO
        LDZ     #RDATABUF
        LDAA    0,Z                             ; READ DATA
;        LDAA    RDATABUF
        COMA
        ANDA    #RUNSTOPBIT
        RTS


; READ BUBBLE BREAK INPUT.
;READBUBBLEBREAK:
;        LDAA    GPTPDR     ;READ INPUT
;        ANDA    #BUBBLEBREAKBIT  ;ISOLATE
;        RTS

; READ OPTICALLY COUPLED INPUTS FOR OPTICAL INPUTS.
;



; READ POT CONTROL INPUT.
READPOTCONTROL
        CLRA                    ; NO POT CONTROL BIT
        TST     EIOMITTED       ; EI OMITTED
        BNE     ISOMIT
        LDAA    SEIINPUTSTATE+1
        ANDA   #POTCONTROLBIT   ;~POT. CONTROL BIT (SET IF IN POT CONTROL)
ISOMIT  TSTA
        RTS

READCIN:
        LDAB   #HARDWAREBANK                   ; HARDWARE BANK
        TBZK                                   ; STORE BANK NO
        LDZ     #RDATABUF
        LDAA    0,Z                             ; READ DATA
;        LDAA     RDATABUF               ; READ
        COMA                           ;
        ANDA    #CINBIT                 ; CONTACTOR OVERLOADED.
        RTS




; FLASH ON BOARD LED, CALLED FROM 1 SECOND INTERRUPT.
FLASHLED:
          LDAB   #HARDWAREBANK                   ; HARDWARE BANK
          TBZK                                   ; STORE BANK NO
          LDZ     #PORTE1
          LDAB   0,Z                    ;PORT E ON HC16
;          LDAB   PORTE1                ;PORT E ON HC16
          EORB   #LEDFLASHBIT          ;TOGGLE BIT 6, HEARTBEAT LED
          STAB  0,Z
;          STAB   PORTE1
          RTS


SSROPHIGH:
        LDAB   #HARDWAREBANK                   ; HARDWARE BANK
        TBZK                                   ; STORE BANK NO
        LDZ    #PORTE1
        LDAB   #PE6BIT
        ORAB   0,Z                   ;PORT E ON HC16
        STAB   0,Z
        RTS

SSROPLOW:

        LDAB   #HARDWAREBANK                   ; HARDWARE BANK
        TBZK                                   ; STORE BANK NO
        LDZ     #PORTE1
        LDAB    #PE6BIT
        COMB
        ANDB    0,Z                   ;PORT E ON HC16
        STAB    0,Z                   ;
        RTS





;***************************************************************;
;  DIAGNOSTIC SECTION.                                          ;
;                                                               ;
; DIAGNOSTIC MODE IS SELECTED BY SETTING THE PASSWORD TO 6553   ;
; AND SELECTING THE APPROPRIATE DIAGNOSTIC.                     ;
; DIAGNOSTIC ARE SET IN THE CONFIGURATION PAGE.                 ;

; AUTOCYCLE,4 COMPONENT CYCLE,OUTPUT CYCLE,DUAL CYCLE,DUAL SLOW ;
; CYCLE,SINGLE VALVE CYCLE.                                     ;
;***************************************************************;


; DIAGNOSTICS. SWITCH ALL OUTPUTS ON.
ALLON:
        JSR     ADL1Q0_ON
        JSR     ADL1Q1_ON
        JSR     ADL1Q2_ON
        JSR     ADL1Q3_ON
        JSR     ADL1Q4_ON
        JSR     ADL1Q5_ON
        JSR     ADL1Q6_ON
        JSR     ADL1Q7_ON
        JSR     ADL2Q0_ON
        JSR     ADL2Q1_ON
        JSR     ADL2Q2_ON
        JSR     ADL2Q3_ON
        JSR     ADL2Q4_ON
        JSR     ADL2Q5_ON
        JSR     ADL2Q6_ON
        JSR     ADL2Q7_ON
        RTS

ALLOFF1:
        JSR     L1Q7OFF
        JSR     ADL1Q7_OFF

ALLOFF:

        JSR     ADL1Q0_OFF
        JSR     ADL1Q1_OFF
        JSR     ADL1Q2_OFF
        JSR     ADL1Q3_OFF
        JSR     ADL1Q4_OFF
        JSR     ADL1Q5_OFF
        JSR     ADL1Q6_OFF
        JSR     ADL2Q0_OFF
        JSR     ADL2Q1_OFF
        JSR     ADL2Q2_OFF
        JSR     ADL2Q3_OFF
        JSR     ADL2Q4_OFF
        JSR     ADL2Q5_OFF
        JSR     ADL2Q6_OFF
        JSR     ADL2Q7_OFF
        RTS

DIAGNOSTIC:
        TST     DUMPSTATUS
        BEQ     ISDUMPING
        JSR     ALLOFF
        RTS
ISDUMPING:
        JSR     ALLON
        RTS

SINGLECYCLELU:
        FDB     ALLOFF
SINGLECYCLEREP:
        FDB     ONEON
ENDSINGLECYCLELU:
        FDB     ONEOFF


; DIAGNOSTIC TO SWITCH ONE OUTPUT ON.
ONEON:
        LDAB    REGCOMP
        LBEQ    OPENDUMP1
        TSTB
        BNE     CONON
        LDAB    #1
CONON   JSR     CMPNON
        RTS


; DIAGNOSTIC TO SWITCH ONE OUTPUT OFF.
ONEOFF:
        LDAB    REGCOMP
        LBEQ    CLOSEDUMP1
        TSTB
        BNE     CONOFF
        LDAB    #1
CONOFF  JSR     CMPNOFF
        RTS


; DIAGNOSTIC 5 COMPONENT CYCLE

DIAGLOOKUP:
CYCLE   FDB     ALLOFF
        FDB     L1Q7ON
REP4CMP:
        FDB     ADL1Q0_ON
        FDB     ADL1Q0_OFF
        FDB     ADL1Q1_ON
        FDB     ADL1Q1_OFF
        FDB     ADL1Q2_ON
        FDB     ADL1Q2_OFF
        FDB     ADL1Q3_ON
        FDB     ADL1Q3_OFF
        FDB     ADL2Q0_ON
        FDB     ADL2Q0_OFF
        FDB     ADL2Q1_ON
        FDB     ADL2Q1_OFF
        FDB     ADL1Q4_ON
ENDLOOKONLINE:
        FDB     ADL1Q4_OFF
        FDB     ADL2Q6_ON
ENDLOOK:
        FDB     ADL2Q6_OFF


; DIAGNOSTIC OUTPUT CYCLE
DIAGLOOKUP1:
        FDB     ALLON
        FDB     ALLOFF
        FDB     ADL1Q7_ON
        FDB     ADL1Q7_OFF
        FDB     ADL1Q5_ON
        FDB     ADL1Q5_OFF
        FDB     ADL1Q0_ON
        FDB     ADL1Q0_OFF
        FDB     ADL1Q1_ON
        FDB     ADL1Q1_OFF
        FDB     ADL1Q2_ON
        FDB     ADL1Q2_OFF
        FDB     ADL1Q3_ON
        FDB     ADL1Q3_OFF
        FDB     ADL1Q4_ON
        FDB     ADL1Q4_OFF
        FDB     ADL2Q7_ON
        FDB     ADL2Q7_OFF

        FDB     ADL2Q6_ON
        FDB     ADL2Q6_OFF

        FDB     ADL2Q0_ON               ; MOTOR OUTPUTS.
        FDB     ADL2Q0_OFF
        FDB     ADL2Q1_ON
        FDB     ADL2Q1_OFF
        FDB     ADL2Q2_ON
        FDB     ADL2Q2_OFF
        FDB     ADL2Q3_ON
        FDB     ADL2Q3_OFF
        FDB     ADL2Q4_ON               ; ALARM RELAY
        FDB     ADL2Q4_OFF

        FDB     ADL2Q5_ON               ; START UP.
        FDB     ADL2Q5_OFF


ENDLOOK1:
        FDB     ALLON

; DIAGNOSTIC DUAL CYCLE
DIAGLOOKUP2:
        FDB     FST2ON
ENDLOOK2:
        FDB     SEC2ON



; DIAGNOSTIC DUAL SLOW CYCLE.
DIAGLOOKUP3:
        FDB     FST2ON
        FDB     FST2ON
        FDB     FST2ON
        FDB     FST2ON
        FDB     FST2ON
        FDB     SEC2ON
        FDB     SEC2ON
        FDB     SEC2ON
        FDB     SEC2ON
ENDLOOK3:
        FDB     SEC2ON



FST2ON  JSR     CMP1ON
        JSR     CMP2ON
        JSR     CMP3OFF
        JSR     CMP4OFF
;        TST     OXXLINE
        LDAA    BLENDERMODE     ; CHECK MODE
        CMPA    #OFFLINEMODE    ; OFFLINE MODE
        BNE     ONLINE1
        JSR     OPENFEED
ONLINE1 RTS

SEC2ON  JSR     CMP3ON
        JSR     CMP4ON
        JSR     CMP1OFF
        JSR     CMP2OFF
;        TST     OXXLINE
        LDAA    BLENDERMODE     ; CHECK MODE
        CMPA    #OFFLINEMODE    ; OFFLINE MODE
        BNE     ONLINE
        JSR     CLOSEFEED
ONLINE  RTS



SETUPDIAGPTR:
        LDAA    DIAGMODE
        CMPA    #TESTPROG1              ; TEST PROGRAM #1
        BNE     CHECK2
        LDX     #REP4CMP
        STX     STARTDIAGPTR
        LDX     #DIAGLOOKUP
        STX     DIAGPOINTER             ; POINTER SETUP
        LDAA    BLENDERMODE     ; CHECK MODE
        CMPA    #OFFLINEMODE    ; OFFLINE MODE
        BEQ     OFFLINE                 ; OFFLINE
        LDX     #ENDLOOKONLINE
        STX     ENDLOOKPTR              ; INDICATE END OF LOOKUP
        RTS

OFFLINE LDX     #ENDLOOK
        STX     ENDLOOKPTR              ; INDICATE END OF LOOKUP
        RTS
CHECK2  CMPA    #TESTPROG2
        BNE     CHECK3
        LDX     #DIAGLOOKUP1
        STX     DIAGPOINTER             ; POINTER SETUP
        STX     STARTDIAGPTR
        LDX     #ENDLOOK1
        STX     ENDLOOKPTR              ; INDICATE END OF LOOKUP
        RTS
CHECK3:

        CMPA    #TESTPROG3
        BNE     CHECK4
        LDX     #DIAGLOOKUP2
        STX     DIAGPOINTER             ; POINTER SETUP
        STX     STARTDIAGPTR
        LDX     #ENDLOOK2
        STX     ENDLOOKPTR              ; INDICATE END OF LOOKUP
        RTS
CHECK4:
        CMPA    #TESTPROG4
        BNE     CHECK5
        LDX     #DIAGLOOKUP3
        STX     DIAGPOINTER             ; POINTER SETUP
        STX     STARTDIAGPTR
        LDX     #ENDLOOK3
        STX     ENDLOOKPTR              ; INDICATE END OF LOOKUP
        RTS
CHECK5:
        CMPA    #TESTPROG5
        BNE     CHECK6
        LDX     #SINGLECYCLELU          ; SINGLE CYCLE LOOKUP.
        STX     DIAGPOINTER             ; POINTER SETUP
        LDX     #SINGLECYCLEREP
        STX     STARTDIAGPTR
        LDX     #ENDSINGLECYCLELU
        STX     ENDLOOKPTR              ; INDICATE END OF LOOKUP
        RTS

CHECK6  LDX     #DIAGLOOKUP1
        STX     DIAGPOINTER             ; POINTER SETUP
        STX     STARTDIAGPTR
        LDX     #ENDLOOK1
        STX     ENDLOOKPTR              ; INDICATE END OF LOOKUP
        RTS



DODIAG:
        TST     OUTPUTDIAG      ; DIAGNOSTICS IN PROGRESS ?
        BNE     NOTDIAG3
        RTS
NOTDIAG3:
        LDAB    #0
        TBYK
        LDY     DIAGPOINTER
        LDY     0,Y                     ; SUBROUTINE TO CALL
        JSR     0,Y                     ; CALL
        LDY     DIAGPOINTER
        CPY     ENDLOOKPTR                ; AT END
        BNE     GETOUT
        LDY     STARTDIAGPTR
        STY     DIAGPOINTER
        RTS
GETOUT  AIY     #2
        STY     DIAGPOINTER
        RTS




; ALL OUTPUTS SWITCHED OFF.
CLOSEALL:

        LDAB    #ONE
REPOFF:
        PSHB
        JSR     CMPNOFF
        PULB
        INCB
        CMPB    NOBLNDS
        BLS     REPOFF

;        LDAB    #TWO
;        JSR     CMPNOFF
;        LDAB    #THREE
;        JSR     CMPNOFF
;        LDAB    #FOUR
;        JSR     CMPNOFF
;        LDAB    #FIVE
;        JSR     CMPNOFF
;        LDAB    #SIX
;        JSR     CMPNOFF
;        LDAB    #SEVEN
;        JSR     CMPNOFF
;        LDAB    #EIGHT
;        JSR     CMPNOFF
;        LDAB    #NINE
;        JSR     CMPNOFF
;        LDAB    #TEN
;        JSR     CMPNOFF
;        LDAB    #ELEVEN
;        JSR     CMPNOFF
;        LDAB    #TWELVE
;        JSR     CMPNOFF
        RTS




; USED IN CMPNON

ONTABLE FDB     CMP1ON
        FDB     CMP2ON
        FDB     CMP3ON
        FDB     CMP4ON
        FDB     CMP5ON
        FDB     CMP6ON
        FDB     CMP7ON
        FDB     CMP8ON
        FDB     CMP9ON
        FDB     CMP10ON
        FDB     CMP11ON
        FDB     CMP12ON

; USED IN CMPNOFF
OFTABLE FDB     CMP1OFF
        FDB     CMP2OFF
        FDB     CMP3OFF
        FDB     CMP4OFF
        FDB     CMP5OFF
        FDB     CMP6OFF
        FDB     CMP7OFF
        FDB     CMP8OFF
        FDB     CMP9OFF
        FDB     CMP10OFF
        FDB     CMP11OFF
        FDB     CMP12OFF


; ROUTINES TO SWITCH ON LATCH 1 OUTPUTS.
;
ADL1Q0_ON:
        LDZ     #ADDLATCH1              ;       LATCH ADDRESS.
        BRA     LATCH1ON
ADL1Q1_ON:
        LDZ     #ADDLATCH1+1            ;       LATCH ADDRESS.
        BRA     LATCH1ON
ADL1Q2_ON:
        LDZ     #ADDLATCH1+2            ;       LATCH ADDRESS.
        BRA     LATCH1ON
ADL1Q3_ON:
        LDZ     #ADDLATCH1+3            ;       LATCH ADDRESS.
        BRA     LATCH1ON
ADL1Q4_ON:
        LDZ     #ADDLATCH1+4            ;       LATCH ADDRESS.
        BRA     LATCH1ON
ADL1Q5_ON:
        LDZ     #ADDLATCH1+5            ;       LATCH ADDRESS.
        BRA     LATCH1ON
ADL1Q6_ON:
        LDZ     #ADDLATCH1+6            ;       LATCH ADDRESS.
        BRA     LATCH1ON
ADL1Q7_ON:
        LDZ     #ADDLATCH1+7            ;       LATCH ADDRESS.
LATCH1ON LDAB   #HARDWAREBANK                   ; HARDWARE BANK
         TBZK                                   ; STORE BANK NO
         LDAA    #1
         STAA    0,Z                     ;       OUTPUT ON.
         RTS

; ROUTINES TO SWITCH OFF LATCH 1 OUTPUTS.


ADL1Q0_OFF:
        LDZ     #ADDLATCH1              ;       LATCH ADDRESS.
        BRA     LATCH1OFF
ADL1Q1_OFF:
        LDZ     #ADDLATCH1+1            ;       LATCH ADDRESS.
        BRA     LATCH1OFF
ADL1Q2_OFF:
        LDZ     #ADDLATCH1+2            ;       LATCH ADDRESS.
        BRA     LATCH1OFF
ADL1Q3_OFF:
        LDZ     #ADDLATCH1+3            ;       LATCH ADDRESS.
        BRA     LATCH1OFF
ADL1Q4_OFF:
        LDZ     #ADDLATCH1+4            ;       LATCH ADDRESS.
        BRA     LATCH1OFF
ADL1Q5_OFF:
        LDZ     #ADDLATCH1+5            ;       LATCH ADDRESS.
        BRA     LATCH1OFF
ADL1Q6_OFF:
        LDZ     #ADDLATCH1+6            ;       LATCH ADDRESS.
        BRA     LATCH1OFF
ADL1Q7_OFF:
        LDZ     #ADDLATCH1+7            ;       LATCH ADDRESS.
LATCH1OFF:
        LDAB   #HARDWAREBANK                   ; HARDWARE BANK
        TBZK                                   ; STORE BANK NO
        LDAA   #0
        STAA    0,Z                     ;       TO LATCH.
        RTS



; ROUTINES TO SWITCH ON LATCH 2 OUTPUTS.
;
ADL2Q0_ON:
        LDZ     #ADDLATCH2              ;       LATCH ADDRESS.
        BRA     LATCH2ON
ADL2Q1_ON:
        LDZ     #ADDLATCH2+1            ;       LATCH ADDRESS.
        BRA     LATCH2ON
ADL2Q2_ON:
        LDZ     #ADDLATCH2+2            ;       LATCH ADDRESS.
        BRA     LATCH2ON
ADL2Q3_ON:
        LDZ     #ADDLATCH2+3            ;       LATCH ADDRESS.
        BRA     LATCH2ON
ADL2Q4_ON:
        LDZ     #ADDLATCH2+4            ;       LATCH ADDRESS.
        BRA     LATCH2ON
ADL2Q5_ON:
        LDZ     #ADDLATCH2+5            ;       LATCH ADDRESS.
        BRA     LATCH2ON
ADL2Q6_ON:
        LDZ     #ADDLATCH2+6            ;       LATCH ADDRESS.
        BRA     LATCH2ON
ADL2Q7_ON:
        LDZ     #ADDLATCH2+7            ;       LATCH ADDRESS.
LATCH2ON:
        LDAB   #HARDWAREBANK                   ; HARDWARE BANK
        TBZK                                   ; STORE BANK NO
        LDAA    #1
        STAA    0,Z                     ;       OUTPUT ON.
        RTS

; ROUTINES TO SWITCH OFF LATCH 2 OUTPUTS.


ADL2Q0_OFF:
        LDZ     #ADDLATCH2              ;       LATCH ADDRESS.
        BRA     LATCH2OFF
ADL2Q1_OFF:
        LDZ     #ADDLATCH2+1            ;       LATCH ADDRESS.
        BRA     LATCH2OFF
ADL2Q2_OFF:
        LDZ     #ADDLATCH2+2            ;       LATCH ADDRESS.
        BRA     LATCH2OFF
ADL2Q3_OFF:
        LDZ     #ADDLATCH2+3            ;       LATCH ADDRESS.
        BRA     LATCH2OFF
ADL2Q4_OFF:
        LDZ     #ADDLATCH2+4            ;       LATCH ADDRESS.
        BRA     LATCH2OFF
ADL2Q5_OFF:
        LDZ     #ADDLATCH2+5            ;       LATCH ADDRESS.
        BRA     LATCH2OFF
ADL2Q6_OFF:
        LDZ     #ADDLATCH2+6            ;       LATCH ADDRESS.
        BRA     LATCH2OFF
ADL2Q7_OFF:
        LDZ     #ADDLATCH2+7            ;       LATCH ADDRESS.
LATCH2OFF:
        LDAB   #HARDWAREBANK                   ; HARDWARE BANK
        TBZK                                   ; STORE BANK NO
        LDAA   #0
        STAA    0,Z                     ;       TO LATCH.
        RTS






; THIS PROGRAM SWITCHES COMPONENTS ON OR OFF DEPENDING ON THE STATUS FLAG.
; THIS WILL STOP SPURIOUS WRITES TO THE OUTPUTS.

SWITCHCOMPONENTSONOROFF:
        LDAA    DIAGMODE
        BEQ     UPDATEIO                ; UPDATE I/O
        LDAA    AUTOCYCLEFLAG
        BEQ     XITDGM
UPDATEIO LDAB    NOBLNDS
REPCMC  PSHB                            ; SAVE NO
        LDX     #FILLSTATCMP1           ; COMPONENT 1.
        TBA
        DECB
        ABX                             ; CHECK COMPONENT #N
        TAB
        TST     0,X                     ; CHECK COMPONENT STATUS.
        BEQ     SHBEOFF                 ; SHOULD BE OFF.
        JSR     CMPNONONLY             ; COMPONENT ON.
        BRA     CHKNXC                  ; CHECK NEXT COMPONENT.
SHBEOFF JSR     CMPNOFFONLY              ;
CHKNXC  PULB
        DECB
        BNE     REPCMC                  ; REPEAT COMPONENT CHECK.
XITDGM  RTS






    if(g_CalibrationData.m_bStandardCompConfig)                     //STDCCFG
    {
        CmpOff( nComponent );
    }
    else        // i.e. non standard config
    {
        nCompConfig = g_CalibrationData.m_nCompConfig[nComponent - 1];
        nMask = (0x0001);
        for(i=0;  i < MAX_COMPONENTS; i++)
        {
            if(nCompConfig & nMask)
            {
                CmpOff( i + 1 );                // CmpOff is 1 based (i.e. if i= 0, swith on comp. 1.
            }
            nMask <<= 1;                        // shift mask bit one to left
        }

    }





*/




