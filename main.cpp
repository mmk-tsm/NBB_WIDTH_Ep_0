extern  int g_ntestonly;

void  TriggerUltrasonic1(void);
void SetUpPIT2At3kHz( void );


//
// P.Smith                      24/1/06
// added modbus master initialisation, include "MBMaster.h"
// call InitialiseModbusMaster();
//      ConfigureModbusMaster();
// include "MBMHand.h"
// call InitialiseSerialPorts();
//      Init_EdgePort();
//      void SetIntc(long func, int vector, int level, int prio );  //from yahoo
//      SetIntc
// added INTERRUPT(IRQ5_pin_isr, 0x2500 )
// call Duart A serial handler. SerialHandler();
// set g_CalibrationData.m_nPeripheralCardsPresent = SEIENBITPOS;
// set g_bSBBL3Fitted to false to ensure that remote display does not operate

// P.Smith                    26/1/06
// added parallel printer test.
// include "PrinterFunctions.h"
// added extern  char        g_cPrinterBuffer[];
//
// P.Smith                  8/2/06
// removed SetUpPITR from this module.
// remove INTERRUPT(my_pitr_func,0x2600), now in seperate file.
// added "L" option as the blender application.  case 'L':
// added extern int g_nFlashRate and setup of this variable when blender application
// is selected.
//
// Removed long g_lRawADCountsLTC2415 = 0;  //global
// removed unsigned int g_nLTC2415Err;  //
// long lTemp;
// removed //nbb   WORD g_nOneSecondSumCounter = 0;
           //nbb   DWORD g_lOneSecondSum = 0;
           //nbb   DWORD g_lOneSecondWeightAverage = 0;
           //nbb   BOOL g_bOneSecondWeightAverageAvailable = FALSE;
//
//
// P.Smith                  14/2/06
//	It was noted that he a/d conversion cannot run at the required 15hz conversion rate.
// It will only run at 12 hz without giving a conversion error.
//  remove DWORD g_lRawAtDCounts;
//
// P.Smith                  14/2/06
// renamed master comms interrupt handler IRQ5
// added initialisation required to allow the master comms to run.
// verified that the master commms is running from the blender application.
//
// P.Smith                  15/2/06
// call     CopyProdSummDataToMB();          //  Copy production summary to modbus table.
// only display diagnostics if debug flag is set
// call foreground program from here.
//
// P.Smith                  24/2/06
// remove 1 second delay, allow foreground program to be called continuously.
//
// P.Smith                  27/2/06
// called InitialiseBlender();
//
// P.Smith                  21/3/06
// removed    g_bSBBL3Fitted  = TRUE; from blender application start up

// P.Smith                  28/3/06
// remove diagnostics recipe set up from this program. also remove copy of
// test data to modbus table.

// P.Smith                  28/3/06
// Remove set of % on power up

// P.Smith                  29/5/06
/// correct printf in ltc2415 test, too many variables for the arguments
// Go straight to the blender program, no prompt to the user

// P.Smith                  9/6/06
// call new serial comms test
// name change g_nDebug -> g_nTSMDegbug
// TestNBBHardware( ); called for now for test "c"
// Removed debug structure from here and put in debug.h
//
// P.Smith                  20/6/06
// email added by Mmk Testemail();
// #include "Email.h"
// #include <mailto.h>
//
// P.Smith                  28/6/06
// removed CopyTimeToMB();
//
// P.Smith                  19/7/06
// added sd card test software, added correct read of 2432 chip
//
// P.Smith                  13/10/06
// added g_cMasterCommsBuffer & g_nMasterCommsPointer;

//
// P.Smith                  15/1/07
// Remove reference to Duart.
//
// P.Smith                  16/1/07
// call U0_Select_Mtty on power up.
//
// P.Smith                  24/1/07
// Put in the latest a/d check test no 4.
// removed TestNBBHardware
// Remove reference to parallel printing
// Redirect standard i/o to port U2
// Remove InitialiseNBBGpio secondly, this was causing the application not to start up if the panel was connected
// on reset
// Correct test 3 & 4
//
// P.Smith                  5/2/07
// Remove hardware access relating to rev a3 nbb
// check operation of uart 0 multiplexing
//
// P.Smith                  5/2/07
// added pulse on time measurement
// check for g_bRedirectStdioToPort2 on power up to determine if the port should
// be redirected.
// remove all printf functions
//
// P.Smith                  5/2/07
// check for g_bRedirectStdioToPort2 put before initialise of blender varaibles.
// call EnableSlaveTransmitter
// call CheckFlowRateChecksum
//
// P.Smith                  8/3/07
// add ultrasonic test M
//
// P.Smith                  23/4/07
// add software update from sd card test W, test to save config file to ds card
// test 5 modified to read inputs from sd card etc.
// Test k, parse of ini file added
// sd card test N added
//
// P.Smith                  3/5/07
// GetNBBBoardRevision called to get the revision no of the board and InitialiseSPIChipSelects
// InitialiseSPIChipSelects is called
//
// P.Smith                  23/5/07
// added initmmc and load of default calibration file.
// remove get_cd & get_wp
// added expansion card test A
// added in CheckForSoftwareUpdate, put initialisation of blender before this.
// Show power up time and date & software revision no.
// remove RestartPanel
//
//
// P.Smith                  11/6/07
// call CheckIfDHCP at start up.
//
// P.Smith                  25/6/07
// some start up functions out of this program into initblnd.cpp.
// CheckFlowRateChecksum, SetUpConversionFactors, g_OnBoardCalData.m_cPermanentOptions
// g_nFlashRate flash rate set up.
//
//
// P.Smith                  3/7/07
// call PresetOptimsationvariables
//
// M.McKiernan                      17/9/07
// Removed g_arrnMBTable[] & g_arrnMBTableFlags[] - now in BatVars2
// Added MBTCP software:
//       Call mbtcp_rtasys_init(); on initialisation
//       Call mbtcp_usersock_process(); continuously in foreground - just after Foreground();
// MBTCP can be called by "O" of maing startup menu, also in main application code at "L".
// Can be disabled by commenting out calls to above 2 functions.
//
// P.Smith                  23/10/07
// added diagnostic print out on initialisation to show the different stage of the
// initialisation.
// only load default calibration data from sd card if bcd switch address is set to 0.
// call WriteBatchIniFile to write out batch ini file on reset.
// remove PresetOptimsationvariables
//
// P.Smith                  12/11/07
// Remove printfs after intialisation of hardware, remove restart panel
// from this program.
// put in time delay before hardware initialisation to allow printf to finish.
//
//
// P.Smith                  29/11/07
// remove setupmttty from intialisation.
//
// P.Smith                  9/1/08
// only do software update if address switch is set to 0
//
// P.Smith                  10/1/08
// call WriteCalibrationDataToSDCard as one function.
//
// P.Smith                  16/1/08
// call CheckForValidHardware to check SHA function
//
//
// P.Smith                  22/1/08
// added test code for usb done by mmk, also call initialisation of usb in user main
// added usb test under test T.
// added PIT2Function & SetUpPIT2At3kHz for usb test.
//
// P.Smith                  28/1/08
// remove OSTimeDly before hardware initialisation
//
// P.Smith                  29/1/08
// added OSTimeDly of 1 second to allow the printf to finish, this was causing
// a problem with the eeprom write that happened immediately in the foreground.
//
// P.Smith                          30/1/08
// correct compiler warnings
//
// P.Smith                          12/2/08
// set g_bPITAllowed to true, this ensures that the pit runs for the ic2 init etc
//
//
// P.Smith                          25/2/08
// added ReadSDCardDirectory and comment out for now.
// added intitialise usb and comment out for now.
// remove stuff for debug check in foreground.
// add case statements for Y & Z for usb test and sd card read directory.
//
// P.Smith                          19/3/08
// remove CheckForValidHardware for now  cirrus test
//
// P.Smith                          20/3/08
// put back CheckForValidHardware
// M.McKiernan                      14/4/08
// Added Telnet task and OsTaskCreate for telnet, and redirection.
// g_bTelnetEnable controls OsTaskCreate for telnet, and initialised to false in this version
// i.e. Telnet is disabled.
//
// P.Smith                          6/6/08
// add back in CheckForValidHardware  take out for kwh
//
// P.Smith                          23/6/08
// remove Telnet functionality from here, name change g_bL3Fitted
//
// P.Smith                          15/7/08
// remove initmmc and WriteCalibrationDataToSDCard
//
// P.Smith                          23/7/08
// remove g_arrnMBTable,g_PowerUpTime & g_CurrentTime externs
//
// P.Smith                          16/10/08
// remove TelnetTaskStk, add ftp test
//
// P.Smith                          24/10/08
// added sd card continuous test
//
// P.Smith                          29/10/08
// added Multiple file SDRepetitiveTest put in as test N, rapid  write sd card test put in at X
//
// P.Smith                          26/11/08
// added Enable_watchdog, comment out for now.
//
// P.Smith                          12/1/09
// use MCP23S17_AT_ADDRESS_3 in InitialiseNBBEXMCP23S17
//
// P.Smith                          12/2/09
/// call InitialiseNetworkSerialPort, change expansion address to address 4.
// correct expansion test, use address 4
// call InitialiseNetworkSerialPort again as the calibration data has not been loaded
// when it is first initialised, this allows the parity option to work
//
// P.Smith                          26/2/09
// remove InitMMC
//
// P.Smith                          26/3/09
// CheckForValidTime,check for valid time called
// remove ReadSDCardDirectory & InitialiseUSB
//
// P.Smith                          24/4/09
// call PrintResetSource, this was causing a problem with the default load from the sd
// card. it needs to be called after the parse function.
//
// P.Smith                          30/6/09
// added CopyConfigDataToMB instead of CopyCalibrationDataToMB
//
// P.Smith                          15/9/09
// remove load of default config when address switch is set to 0.
//
// P.Smith                          29/9/09
// call DeleteDefaultFile to delete the default file on power up.
// this ensures that the default file is deleted
// a problem has arose where if software was updated and the something
// was added to the config, like loading, the config was not updated automatically
// due to the fact that the default.env file was not updated.
// now any change in the config will always be recorded.
//
// P.Smith                          15/12/09
// removed unused test code.
// Eclipse
// M.McKiernan                      2/12/09
// Added include for FileSystemUtils.h & cardtype.h
// Call f_enterFS() prior to starting main application, i.e. at 'L'

// Ethernet/IP
// M.McKiernan                      29/01/2010
// Added following includes;
//			#include "eips_system.h"
//			#include "eips_usersys.h"
// Added following externs Ethernet/ip.
//			extern VDWORD g_nCurrentTick, g_nOldTick;
//			extern VDWORD TimeTick;
//			extern uint32 local_get_ticks_passed (void);
// Added following task related stuff: (between #if and #enfif.

// M.McKiernan						04/03/2010
 // Variable g_bSoftwareUpdateFromSDInProgress  initialised to false, dont call watchdog service in SPI routines.
//  bool g_bSoftwareUpdateFromSDInProgress = FALSE;
// On entry to UserMain() service the watchdog timer.
 // Note from Netburner.
	//autoupdate has a function pointer watchdog_service_function
	//that should point to your watch dog service function.
//	watchdog_service_function = Service_Watchdog;
//	Service_Watchdog();	// service watchdog timer in case it is running.
// Call Service_Watchdog at case 'L' - i.e. at start of blender application.
// After PrintResetSource() call InitialiseWatchdog();	//enables watchdog if NBB address non-zero. else disables.
// In main application while loop, call Service_Watchdog() - just before call to Foreground( FALSE );
//

#if 0
//Make sure they're 4 byte aligned to keep the ColdFire happy
// DWORD EIPTaskStk[USER_TASK_STK_SIZE] __attribute__((aligned(4)));
 DWORD UdpTestStk[USER_TASK_STK_SIZE] __attribute__((aligned(4)));
 DWORD UdpIOTestStk[USER_TASK_STK_SIZE] __attribute__((aligned(4)));
// DWORD LEDStk[USER_TASK_STK_SIZE] __attribute__((aligned(4)));

// extern void EipTaskMain (void * pd);
 extern void UdpReaderMain (void * pd);
 extern void UdpIOReaderMain (void * pd);
 //extern void UpdateLEDTask( void * pd);
//#define RTA_TASK_EIP 	(MAIN_PRIO+1)
#define RTA_TASK_UDP 	(MAIN_PRIO-2)	//(MAIN_PRIO+2)  //TSM see if can run.
#define RTA_TASK_UDP_IO (MAIN_PRIO-3)
//#define RTA_TASK_LED	(MAIN_PRIO+4)
#endif
// Ethernet/IP initialisation - see Start  of Ethernet/IP initialisation to end of eips initialisation.
// continuous call for Ethernet/IP
//  eips_rtasys_process(local_get_ticks_passed());
 // M.McKiernan                      29/01/2010
// Added define g_nDRAMmask=sim.sdram.dmr0 and printf'ed it for diagnostic only.
 //
// M.McKiernan                      11/02/2010
// Added an OS flag (OS_FLAGS) gEIPTaskErrorFlag  use gEIPTaskErrorFlag to halt
// the Ethernet/IP UDP tasks if fatal error.
// gEIPTaskErrorFlag cleared on initialisation -  OSFlagClear( &gEIPTaskErrorFlag, 0xFFFFFFFF );	//clearS all bits.
// Added global variables - bool g_bEIPSoftwareEnabled = TRUE;		//NB license will set/clear this, to enable/disable Ethernet/IP.
// Added bool g_bEIPSoftwareFatalErrorOccurred = FALSE; - use to halt Ethernet/IP process.
//
// P.Smith                      17/02/2010
// call ethernet ip initialisation after initialisation of blender.
// this ensures that the licensing is set up and  the flag g_bEIPSoftwareEnabled
// will be set true or false depending on the licensing status.
// SetupMttty is added to print ethernet ip message.
// only call eips_rtasys_process if  g_bEIPSoftwareEnabled is TRUE.
//
// P.Smith                      24/02/2010
// only run modbus tcp if LICENSE_MODBUSTCP bit set in g_OnBoardCalData.m_wBlenderOptions
 //
// P.Smith                      19/3/10
// there is a problem in the checkifdhcp function regarding the watch dog.
// this function takes 10 seconds to timeout.
// this timeout causes the watch dog to come in.
// this has been corrected by making a change in the file dhcp.cpp
// to eliminate this problem.
// also the address switch needs to be checked earlier to allow the
// blender to get out of this problem by disabling the watch dog timer.
 //
 // P.Smith                          25/3/10
 // shorten printfs to hide what is being initialised
//
// P.Smith                          6/4/10
// ReadDataFromRAM and store function running info to g_wFunctionRunningAtPowerUp
// and g_wFunctionSubSectionRunningAtPowerUp
// comment out EnableTaskMonitor
//
// P.Smith                          7/4/10
// remove nNBBAddress
//
// P.Smith                          15/4/10
// if platform is 5270B and module is 5270B, then set g_wPlatFormInUse to 5270B flatform
//
// P.Smith                          26/4/10
// InitialiseNetworkSerialPort is removed from the main program.
// This is causing the config to be loaded a second time and any data that is
// already changed is getting reloaded again.
//
// P.Smith                          4/5/10
// moved mbtcp_rtasys_init down at the end of the initialisation.
// define g_bSimulateMBTCPFatalError and initialise to false.
// set g_nMBTCPProcessHoldoffTime to 2.
 // only call mbtcp_usersock_process if not fatal error
//
// P.Smith                          8/6/10
//  call WidthInitialisation & WidthForeground
//
// M.McKiernan
// Call       if(EtherLink())
//		before  call to CheckIfDHCP();
// call to CheckForSoftwareUpdate(); removed at: if(g_cBlenderNetworkAddress == DEFAULT_LOAD_ADDRESS)
 /*-------------------------------------------------------------------
 File: Main.cpp
 Description: This file uses functions that will read and write to
              an EEPROM.
 -------------------------------------------------------------------*/

#include "predef.h"
#include <stdio.h>
#include <ctype.h>
#include <startnet.h>
#include <autoupdate.h>
#include <dhcpclient.h>
#include <http.h>
#include <htmlfiles.h>
//#include <..\mod5272\system\sim5272.h>
#include <C:\Nburn\MOD5270\system\sim5270.h>
#include <cfinter.h>
#include <smarttrap.h>

//#include "QSPIdriver.h"
//#include "EEPROMconstants.h"
//#include "EEPROMdriver.h"

#include "LTC2415.h"
#include "I2CFuncs.h"

#include "InitComms.h"
//#include <C:\Nburn\examples\MySPI\InitComm.h>

#include "MAX132.h"
#include "MBSHand.h"
#include "PITHandler.h"

#include <gpio5270.h>
#include <NBBGpio.h>
#include "MBProgs.h"



//#include <C:\Nburn\examples\MySPI\QSPIdriver.h>
#include "QSPIdriver.h"
#include "EEPROMconstants.h"
#include "EEPROMdriver.h"
#include "QSPIconstants.h"
// for file system:
#include <effs_fat/fat.h>
#include <effs_fat/mmc_mcf.h>
#include <effs_fat/cfc_mcf.h>



#include "InitialiseHardware.h"
#include "BatchCalibrationFunctions.h"
#include "Batvars.h"
#include "Batvars2.h"
#include "MBMaster.h"
#include "MBMHand.h"
#include "PrinterFunctions.h"
#include "string.h"
#include "initblnd.h"
#include "init5270.h"
#include "RTCHand.h"
#include "NBBVars.h"
#include "MBProsum.h"
#include "foreground.h"
#include "Setpointformat.h"  //nbb--testonly--
#include "Mbprogs.h"  //nbb--testonly--
#include "TimeDate.h"
#include "Textstrings.h"
#include "Textstringsini.h"
#include "TextstringsWidthini.h"

#include "Textstringsevents.h"
#include <NetworkDebug.h>
#include "TestNBB.h"
#include "Debug.h"
#include "Email.h"
#include <mailto.h>
#include "SHA1.h"
#include <Serial.h>
#include "LSpeed.h"
#include "Mtttymux.h"
#include "PrntDiag.h"
#include "Monbat.h"
#include "UltrasonicRead.h"
#include "fileup.h"
#include <Bsp.h>
#include "Dictionary.h"
#include "Iniexample.h"
#include "Transfercaldatasdcard.h"
#include "Sdcard.h"
#include <pins.h>
#include "SoftwareUpdate.h"
#include "ConfigManipulation.h"
#include "Onewire.h"
#include "Loadcaldatafromsdcard.h"
#include "MCP23Sexp.h"
#include "System.h"
#include "NBBip.h"
#include "Optimisation.h"
#include "InitFTP.h"

#include "rta_system.h"
#include "mbtcp_system.h"
#include "BatchMBIndices.h"

#include "CopyCalibrationDataToIniFile.h"
#include "Mapping.h"
#include "General.h"
#include "Secret.h"
//#include "USB.h"
#include "VNC1L.h"
#include <fastmath.h>
// stuff for telnet...
#include <tcp.h>
#include <ip.h>
#include <ftpd.h>
#include <C:\Nburn\examples\EFFS\EFFS-FTP\ftp_f.h>
#include "Watchdog.h"
#include "InitNBBcomms.h"
#include "BlenderSelfTest.h"

#include "FileSystemUtils.h"
#include "CardType.h"

#include "eips_system.h"
#include "eips_usersys.h"
#include "License.h"
#include "security.h"
#include "WidthForeGround.h"
#include "WidthInitialisation.h"
#include "ethernet.h"


//Added for Ethernet/ip.
extern VDWORD g_nCurrentTick, g_nOldTick;
extern VDWORD TimeTick;
extern uint32 local_get_ticks_passed (void);

OS_FLAGS gEIPTaskErrorFlag;
bool g_bEIPSoftwareEnabled = TRUE;		//NB license will set/clear this.
bool g_bEIPSoftwareFatalErrorOccurred = FALSE;

WORD g_wPlatFormInUse;
extern  DWORD g_nVNC1LPITCounter;
extern structVNC1LData  g_sVNC1L;


extern unsigned char	Rx1Buff[];
extern  char        g_cMB1BreakFlag;
extern  WORD  g_nMB1BreakCounter;
extern char g_cRx1Data;
extern  WORD g_test1,g_test2;
extern  unsigned char 	g_cBlenderNetworkAddress;
extern CalDataStruct    g_CalibrationData;
extern structCommsData         g_sModbusMaster;
extern  char        g_cPrinterBuffer[];
extern BOOL    g_bPrintInProgress;
extern  BOOL   g_bPrintAssembleInProgress;
char	cBuffer[80];
extern unsigned int    g_nPIT_Int_CounterU;
extern int g_nFlashRate;
extern int     g_nTickCounter;         // General purpose tick counter
extern  structSetpointData  g_TempRecipe; //--nbb--testonly
extern  structSetpointData  g_CurrentRecipe; //--nbb--testonly

extern  int g_nProtocol;
extern  OnBoardCalDataStruct   g_OnBoardCalData; //nbb--testonly--
extern  StructDebug   g_nTSMDebug;
extern BYTE g_nNBBBoardRevision;

char    g_cMasterCommsBuffer[500];
int     g_nMasterCommsPointer;

long g_lSGCountsValue;
long g_lRawValueBuffer[5] = {0,0,0,0,0};
long g_lOneSecondWeightAverageSG =0;
long g_lOneSecondSumSG = 0;
long lMaxValue = 0;
long lMinValue = LTC2415_MAX_VALUE;
long lLowestValue,lHighestValue;
int nIntervalTicks = TICKS_PER_SECOND/11;
WORD g_wFunctionRunningAtPowerUp;
WORD g_wFunctionSubSectionRunningAtPowerUp;

struct tm bts;


void flushScreen();
void SDRepetitiveTest( void );


//void Delay_1uS( void );

//void Delay_uS( WORD nMicroSeconds );

//----- global variables -----
volatile DWORD pitr_count = 0;
DWORD g_nDRAMmask=sim.sdram.dmr0;

//volatile WORD g_nI2CWaitPits=0;
volatile DWORD g_nPit2TimeoutCounter = 0;

volatile WORD g_nPit2Counter=0;
volatile BOOL g_bPIT2HalfSecondFlag = FALSE;
    BOOL bHBLEDON = FALSE;

extern  BYTE DS2401Buffer[];
extern  BYTE DS2432Buffer[];
extern WORD SRAMData[];
extern const char *PlatformName;

BYTE *pDS2432Buffer = DS2432Buffer;

   BYTE  DS2432ReadBuffer[42];
   BYTE *pDS2432ReadBuffer = DS2432ReadBuffer;
    BYTE  SecretBuf[8];
    BYTE  DS2432IDBuf[10];
    BYTE  ChallengeBuf[8];

 WORD nMsgCRC16[1];
 WORD *ptrMsgCRC16 = nMsgCRC16;
 int i, j, err;
 extern int g_nMBTCPProcessHoldoffTime;

 extern bool g_bMBTCPFatalErrorOccurredFlag;
 bool g_bSimulateMBTCPFatalError = FALSE;

  SHA1Context sha2;
BYTE vBuffer[2000];


    int NBBBoardAddress;
    BOOL g_bAutoUpdateEnable = TRUE;
extern  unsigned int g_nForegndPassCounter;

volatile WORD g_lRTCCount = 0;

extern "C" {
void UserMain(void * pd);
void OSDumpTCBs();


/* This function sets up  the 5282 interup controller */
void SetIntc(long func, int vector, int level, int prio );  //from yahoo
}

extern "C" {
void NMI_C_Part(); // The part of the ISR written in C/C++
void NMI_ASM_Part(); // The part of the ISR written in Assembly
}
/* The “Function_Holder()” is just a place holder so we can create some inline
assembly language code. It will never be called. Note that in the __asm__
statements, you need a leading space between the (“ and the instruction. However,
a space is not used for labels. */
void Function_Holder()
{
__asm__ (" .global NMI_ASM_Part"); // export the label for ASM part
__asm__ (" .extern NMI_C_Part"); // label for the C part of the ISR
__asm__ ("NMI_ASM_Part:");       // label for the assembly part of the ISR
__asm__ (" move.w #0x2700,%sr "); // set the IRQ mask to mask all
__asm__ (" lea -60(%a7),%a7 "); // make space on the system stack
__asm__ (" movem.l %d0-%d7/%a0-%a6,(%a7) "); // save all registers
__asm__ (" jsr NMI_C_Part ");
__asm__ (" movem.l (%a7),%d0-%d7/%a0-%a6 ");
__asm__ (" lea 60(%a7),%a7 ");
__asm__ (" rte");
}

volatile DWORD Irq7_count = 0;
volatile BOOL  bIRQ7Flag = FALSE;
volatile BYTE   g_cIp1ToIp8,g_cLLS1ToLL8;


/* This is the C/C++ part of the ISR that is called from the assembly code */
void NMI_C_Part()
{
// Your C/C++ application code goes here
  sim.eport.epfr=0x80; /* Clear the interrupt edge 1 0 0 0 0 0 0 0 */
//  Irq7_count++;
  g_lRTCCount++;
  bIRQ7Flag = TRUE;
  Irq7_count++;
  RTCHandler();

}




const char * AppName= * RevisionNumber;


#define APPFILENAME "nbb_APP.S19"      // name of file on SD card.

#define tmp_buffer_size (256)
char tmp_buffer1[tmp_buffer_size];
//int tmp_buffer_end;
//int tmp_buffer_start;
int n;
 WORD       nPassCounter = 0;

/* The stack for the second task - telnet */
//static DWORD   TelnetTaskStk[USER_TASK_STK_SIZE] __attribute__( ( aligned( 4 ) ) );

//tj's
#define TCP_LISTEN_PORT (23)     //Telnet port.

 //Make sure they're 4 byte aligned to keep the ColdFire happy
// DWORD EIPTaskStk[USER_TASK_STK_SIZE] __attribute__((aligned(4)));
 DWORD UdpTestStk[USER_TASK_STK_SIZE] __attribute__((aligned(4)));
 DWORD UdpIOTestStk[USER_TASK_STK_SIZE] __attribute__((aligned(4)));
// DWORD LEDStk[USER_TASK_STK_SIZE] __attribute__((aligned(4)));

// extern void EipTaskMain (void * pd);
 extern void UdpReaderMain (void * pd);
 extern void UdpIOReaderMain (void * pd);
 //extern void UpdateLEDTask( void * pd);
//#define RTA_TASK_EIP 	(MAIN_PRIO+1)
#define RTA_TASK_UDP 	(MAIN_PRIO-2)	//(MAIN_PRIO+2)  //TSM see if can run.
#define RTA_TASK_UDP_IO (MAIN_PRIO-3)
//#define RTA_TASK_LED	(MAIN_PRIO+4)

 // initialise to false, dont call watchdog service in SPI routines.
  bool g_bSoftwareUpdateFromSDInProgress = FALSE;

void UserMain(void * pd)
{


	InitializeStack();

    ReadDataFromRAM();
    g_wFunctionRunningAtPowerUp = SRAMData[0];
    g_wFunctionSubSectionRunningAtPowerUp = SRAMData[1];

	OSChangePrio(MAIN_PRIO);


	// Note from Netburner.
	//autoupdate has a function pointer watchdog_service_function
	//that should point to your watch dog service function.

	watchdog_service_function = Service_Watchdog;

	Service_Watchdog(); // service watchdog timer in case it is running.


	// Following basic initialisation needed to read hex switch, needed to enable/disable watchdog.

	Init_RevB1ChipSelects(); // initialise CS2

	InitialiseNBBGpio(); // initialise the gPIO pins.

	U0_Select_Mtty(); // set u0 mux for MTTTY.

	//watchdog

	InitialiseWatchdog(); //enables watchdog if NBB address non-zero. else disables.

	//watchdog end.


    g_bTelnetEnable = TRUE;
    Init_RevB1ChipSelects();         // initialise CS2
    U0_Select_Mtty();
    iprintf("\ntp");

//reset the UsB chip
      VNC1L_RESET_LOW();
       OSTimeDly(TICKS_PER_SECOND/10);      // wait 1/10 second.
      VNC1L_RESET_HIGH();
      if( EtherLink() )		//wfh 27.5.2020
      {
    	  CheckIfDHCP();
      }
      EnableAutoUpdate();
      StartHTTP();
      //EnableTaskMonitor();
      int fd1;
      Service_Watchdog();
 //start of SD card test
      SPI_Select_SD();
      f_enterFS();


                    // call the MBTCP initialization code (RTA's init routines)
                     iprintf("\nMT(V%s)\r\n",MBTCP_VERSION_STRING);
              	   if((strcmp(PlatformName, "MOD5270B") == 0) && (g_nDRAMmask == 0x7c0001))
              	   {
              		   g_wPlatFormInUse = PLATFORM_5270B;
              	   }
              	   else
              	   {
            		   g_wPlatFormInUse = 0;
              	   }

                    g_bPITAllowed = TRUE;
                    InitialiseHardware();
                     InitialiseBlender();
                     WidthInitialisation();
                     // Start  of Ethernet/IP initialisation.
                      //g_bEIPSoftwareEnabled from license
                      //g_bEIPSoftwareFatalErrorOccurred true if fatal error occurs.

                      //NB: will use EIPTaskErrorFlag to halt the UDP tasks if fatal error.
                      OSFlagCreate( &gEIPTaskErrorFlag );
                      OSFlagClear( &gEIPTaskErrorFlag, 0xFFFFFFFF );	//clearS all bits.


                      if(g_bEIPSoftwareEnabled && !g_bEIPSoftwareFatalErrorOccurred )
                      {
                      //eips
                          eips_rtasys_init();

                          SetupMttty();
                          iprintf("\nE S\r\n" );
                          OSTimeDly( TICKS_PER_SECOND/5 );	// wait 1/5 second

 						// get the base time
                          g_nCurrentTick = TimeTick;
                          g_nOldTick = g_nCurrentTick;
                          //eips
                      	 //According to Jamin of RTA, need to leave the UDP tasks in place.
                      	/* open a UDP socket for UCMM Messages */
                      	if(OSTaskCreate(UdpReaderMain,(void  *)EIP_CIP_PORT,&UdpTestStk[USER_TASK_STK_SIZE] ,UdpTestStk,RTA_TASK_UDP) != OS_NO_ERR)
                      	{
                      		eips_usersys_fatalError("UserMain",3);
                      	}

                      	/* open a UDP I/O socket */
                      	if(OSTaskCreate(UdpIOReaderMain,(void  *)EIPS_UDPIO_PORT,&UdpIOTestStk[USER_TASK_STK_SIZE] ,UdpIOTestStk,RTA_TASK_UDP_IO) != OS_NO_ERR)
                      	{
                      		eips_usersys_fatalError("UserMain",4);
                      	}
                      }
                      //end of eips initialisation.


                     CheckForValidTime();
                     //InitMMC();
                    if(g_cBlenderNetworkAddress == DEFAULT_LOAD_ADDRESS)
                    {
                       // CheckForSoftwareUpdate();
                    }
               //     if(g_cBlenderNetworkAddress == DEFAULT_LOAD_ADDRESS)
               //     {
               //        status1 =  parse_Batch_ini_file( "default.ini" );
               //     }

                   CopyConfigDataToMB();

                  DeleteDefaultFile();
                  WriteCalibrationDataToSDCard();

                   PrintResetSource();
                   SetupMttty();
                   iprintf("\nPower up %d/%d/%d    %d:%d",g_PowerUpTime[TIME_DATE],g_PowerUpTime[TIME_MONTH],g_PowerUpTime[TIME_YEAR],g_PowerUpTime[TIME_HOUR],g_PowerUpTime[TIME_MINUTE]);
                   iprintf("\nSoftware Revision %s",*RevisionNumber);

                   CheckForValidHardware();


               if(g_bRedirectStdioToPort2)
                {
                    iprintf("\n starting blender application with redirection");
                    g_bEnableTxRxInOpenSerial = TRUE;
                    g_bPrintfRunning = TRUE;
                    SerialClose(2);
                    fd1 = OpenSerial( 2, 115200, 1, DATA_BITS, eParityNone );

                    ReplaceStdio( 0, fd1 );
                    ReplaceStdio( 1, fd1 );
                    ReplaceStdio( 2, fd1 );
                    EnableSlaveTransmitter();

                }
//          InitialiseUSB();    //nbb--todo- put back in
//          RestartPanel1();

          //Enable_watchdog();
          OSTimeDly(TICKS_PER_SECOND);
          mbtcp_rtasys_init();
          g_nMBTCPProcessHoldoffTime = 2; //wait 2 seconds before servicing mbtcp process.


          while(1)
          {
              Foreground( FALSE );
              WidthForeground();
        	  Service_Watchdog();		// Watchdog timer
              nPassCounter++;
              if((g_OnBoardCalData.m_wBlenderOptions & LICENSE_MODBUSTCP) != 0)
              {
            	  if(!g_bMBTCPFatalErrorOccurredFlag && (g_nMBTCPProcessHoldoffTime == 0))
                  {
            		  mbtcp_usersock_process();
                  }
              }
              // call for Ethernet/IP
              // from -  EipTaskMain (void * pd)
              // process the state //
              if(g_bEIPSoftwareEnabled)
              {
                  eips_rtasys_process(local_get_ticks_passed());
              }
         }

}




