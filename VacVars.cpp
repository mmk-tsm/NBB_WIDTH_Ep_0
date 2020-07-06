/////////////////////////////////////////////////
//
// M. McKiernan                      9/6/09
// create variables file for Vacuum loading
//
//
// M. McKiernan                      7/9/09
// Added variables used for manual operation of loader system 
// See  all New 7.Sept.2009
// g_bVacLoaderSystemInManual; g_bPumpManualCommand;  g_bAtmValveManualCommand; g_bFilterManualCommand; 
// g_bVacLoaderSystemClearAlarms;  g_bVacLoaderManualCommandWritten
// Initialisation for commands and .m_bManualCommand in loader structures.
// Renamed m_nDumpDelayTime -> m_nDischargeTime.  -- calibration parameter
// Renamed m_nDumpDelay -> m_nDischargeTmr.  - loader variable.
// Added g_nVAC8IOCards; g_nWriteVAC8Card2WordOps1; g_nWriteVAC8Card2WordOps2, g_nWriteVAC8Card3WordOps1; g_nWriteVAC8Card3WordOps2
// Added g_nVAC8IOPumpCardNo
// Added m_bOutputState to loader structure
//
// P.Smith                      29/9/09
// added g_nLoadersHistory
/////////////////////////////////////////////////

//*****************************************************************************
// MACROS
//*****************************************************************************

#include "General.h"
#include <basictypes.h>
#include "VacVars.h"
#include "BatchCalibrationFunctions.h"
#include "BatVars.h"
#include "TSMPeripheralsMBIndices.h"

structVacLoaderData g_sLoader[MAX_LOADERS];
structVacCalib   g_VacCalibrationData;
structVacCalib   g_UploadedVacuumLoadingData;

extern CalDataStruct    g_CalibrationData;



bool  g_bAtmValveFlag;
bool  g_bFilterFlag;
bool  g_bFilterSequenceInProgress;
bool  g_bFilterSequenceFinished;
WORD  g_nPumpRunTimer;      // time in seconds pump is running
bool  g_bPumpFlag;         // whether pump should be on/off - i.e. commanded
bool  g_bPumpReady;        // whether pump is ready to use.
bool  g_bPumpStatus;       // current status of pump.
bool  g_bAtmValveStatus;
bool  g_bFilterStatus;
int   g_nFilterOnTimer;
int   g_nFilterOffTimer;
int   g_nFilterRepetitionsCounter;

BYTE  g_nFillingUnit;
WORD  g_nFillingTime;      // filling time
BYTE  g_nOverRideRequest;   // will contain the no. of loader seeking Over-ride.

WORD  g_nSystemIdleTimer;  //
WORD  g_nAVOpenTimer;      // seconds - time while AV open
WORD  g_nAVIsOpenTimer;    // tenths of second, increments when AV is open..
WORD  g_nAVOverLapTimer;      // 10ths of second.

BYTE  g_nToFillTable[TO_FILL_TABLE_SIZE];  // table for fill requests - highest priority at top (0).

WORD  g_nWriteVAC8WordOps1;
WORD  g_nWriteVAC8WordOps2;

WORD  g_nWriteVAC8Card2WordOps1;
WORD  g_nWriteVAC8Card2WordOps2;

WORD  g_nWriteVAC8Card3WordOps1;
WORD  g_nWriteVAC8Card3WordOps2;

BOOL        g_bVAC8IOReadDataAvailable[MAX_VAC8IO_CARDS];                                            // new variable
BOOL        g_bVAC8IOReadDataReady[MAX_VAC8IO_CARDS]; 
BOOL  g_bSaveLoaderCalibrationData;
BOOL  g_bVacuumLoaderCalibrationWritten;
BYTE  g_nVacLoaderAlarms1;       // loader alarms on VAC8IO #1
BYTE  g_nVacLoaderAlarms2;
BYTE  g_nVacLoaderAlarms3;
BOOL  g_bVacSimulatorRunning;

//New 7.Sept.2009
bool  g_bVacLoaderSystemInManual;      // True=yes, => manual, else normal.
bool  g_bPumpManualCommand;            // True = ON.
bool  g_bAtmValveManualCommand;        // True = On or OPEN
bool  g_bFilterManualCommand;          // True = ON.
bool  g_bVacLoaderSystemClearAlarmsCommand;   // True = clear alarms.
bool  g_bVacLoaderManualCommandWritten;   // indicates that manual or test command written.
bool  g_bVacLoaderSystemEnterManual;
bool  g_bVacLoaderSystemExitManual;
BYTE  g_nVAC8IOCards;
BYTE  g_nVAC8IOPumpCardNo;
BYTE  g_nVacLoaderAlarmsTotal = 0;
BYTE  g_nLoadersHistory = 0;

void InitialiseVacVars( void ) 
{
int i;

// ToDo.
//   HardCodeVacCalibrationData();    // testonly....todo...will be read from EEPROM.
   InitialiseLoaderStructures();
 
 // first all outputs.
   g_bPumpFlag = OFF;
   g_bAtmValveFlag = OPEN;
   g_bFilterFlag = OFF;
   
   for(i=0; i < MAX_LOADERS; i++)
	{
      g_sLoader[i].m_bFillValve = OFF;       // TURN off fill relay  
      g_sLoader[i].m_bAlarm = FALSE;       
   }
   
// CLEAR THE "TO FILL" TABLE.  
	for(i=0; i<TO_FILL_TABLE_SIZE; i++)
	{
	  g_nToFillTable[i] = 0;     
   }
// related to unit (loader) filling.
   g_nFillingTime = 0;
   g_nFillingUnit = 0;
   g_nOverRideRequest = 0;
   
   g_nSystemIdleTimer = 1;  //
   g_nAVOpenTimer = 1;
   g_nAVOverLapTimer = 0;
   g_nPumpRunTimer = 0;      // time in seconds pump is running
   
   g_bPumpStatus = OFF;       // current status of pump.
   g_bPumpReady = FALSE;
   g_bAtmValveStatus = CLOSED;
   
   g_nFilterOnTimer = 0;
   g_nFilterOffTimer = 0;
   g_nFilterRepetitionsCounter = 0;
   g_bFilterStatus = OFF;
   
   g_bFilterSequenceInProgress = FALSE;
   g_bFilterSequenceFinished = FALSE;
    
   for(i=0; i<MAX_VAC8IO_CARDS; i++)
   { 
      g_bVAC8IOReadDataAvailable[i] = FALSE;       
      g_bVAC8IOReadDataReady[i] = FALSE; 
   }

   g_bSaveLoaderCalibrationData = FALSE; 
   g_bVacuumLoaderCalibrationWritten = FALSE;                
   
   g_nVacLoaderAlarms1 = 0;
   g_nVacLoaderAlarms2 = 0;
   g_nVacLoaderAlarms3 = 0;
   // testonly.....will have to enable/disable simulator somehow.
//   g_bVacSimulatorRunning = FALSE;   

//New 7.Sept.2009
   g_bVacLoaderSystemInManual = FALSE;      // initialise to normal.
   g_bVacLoaderSystemEnterManual = FALSE;
   g_bVacLoaderSystemExitManual = FALSE;
   
   g_bPumpManualCommand = OFF;            // pump = OFF.
   g_bAtmValveManualCommand = ON;        // Atm Valve OPEN
   g_bFilterManualCommand = OFF;          // True = ON.
   g_bVacLoaderSystemClearAlarmsCommand = FALSE;   // 
   
   g_nWriteVAC8WordOps1 = 0x0000;
   g_nWriteVAC8WordOps2 = 0x0000;

   g_nWriteVAC8Card2WordOps1 = 0x0000;
   g_nWriteVAC8Card2WordOps2 = 0x0000;

   g_nWriteVAC8Card3WordOps1 = 0x0000;
   g_nWriteVAC8Card3WordOps2 = 0x0000;
   
   g_bVacLoaderManualCommandWritten = FALSE; 
   g_nVAC8IOCards = 0;
   g_nVAC8IOPumpCardNo = 1;               // Initialise assuming pump connected to 1st VAC8IO.
}
   
void InitialiseLoaderStructures( void )
{
int i;
   for(i=0; i<MAX_LOADERS; i++)
   {
	   g_sLoader[i].m_bAlarm = FALSE;
      g_sLoader[i].m_nLoaderNo = i+1;         // loader no's are 1 based.
      g_sLoader[i].m_nStatusByte = 0x00;       //
      g_sLoader[i].m_nFillTimer = 0;        // 
      g_sLoader[i].m_nOverLapTimer = 0;     //
      g_sLoader[i].m_nDischargeTmr = g_VacCalibrationData.m_nDischargeTime;        // delay in seconds before seeking re-fill. DELAY 3 SECS AT START.
      g_sLoader[i].m_nFillCounter = 0;      //
      g_sLoader[i].m_bLevelFlag = FULL;
      g_sLoader[i].m_bBlenderFillValveFlag = FALSE;   // used to inhibit loading
      g_sLoader[i].m_bInhibit = FALSE;
      g_sLoader[i].m_bHiLevelSensor = UNCOVERED;
      
      // note the following 2 variables control outputs.
         g_sLoader[i].m_bFillValve = OFF;       // TURN off fill relay  
         g_sLoader[i].m_bAlarm = FALSE;   
         
      g_sLoader[i].m_nLevelSimulator = 7 + i;    

// New 7.Sept.2009.      
      g_sLoader[i].m_bManualCommand = OFF;        // 
      g_sLoader[i].m_bOutputState = OFF;        // 
	  	     	  
   }	  
	
} 


// for test purposes - to get going.
void HardCodeVacCalibrationData( void)
{
int i;
   for(i=0; i < 5; i++)
	{ 
      g_VacCalibrationData.m_nSuctionTime[i] = 7;
      g_VacCalibrationData.m_nPostFillTime[i] = 1;
      g_VacCalibrationData.m_bOnFlag[i] = ON;      // enable first 4 loaders.
      g_VacCalibrationData.m_nPriority[i] = i+1;
   } 
   for(i=5; i < MAX_LOADERS; i++)
	{ 
      g_VacCalibrationData.m_nSuctionTime[i] = 8;
      g_VacCalibrationData.m_nPostFillTime[i] = 0;
      g_VacCalibrationData.m_bOnFlag[i] = ON;     // all loaders ON
      g_VacCalibrationData.m_nPriority[i] = 6;
   } 
 
   g_VacCalibrationData.m_nLoaders = 8;
      
   g_VacCalibrationData.m_nAVOverLapTime = 2;
   g_VacCalibrationData.m_nDischargeTime = 5;
   g_VacCalibrationData.m_nPumpIdleTime = 30;
   g_VacCalibrationData.m_nPumpStartTime = 5;      // allow 5 seconds for pump to rev up.
   g_VacCalibrationData.m_nFilterRepetitions = 3;
   g_VacCalibrationData.m_nFilterType = 1;         // assume TSM standard for now.
}

     
