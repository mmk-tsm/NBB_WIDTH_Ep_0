/////////////////////////////////////////////////
// VacVars.h
// M. McKiernan                        9/6/09
// create variables file for Vacuum loading system
//
//
// M. McKiernan                      7/9/09
// Added variables used for manual operation of loader system 
// See  all New 7.Sept.2009
// g_bVacLoaderSystemInManual; g_bPumpManualCommand;  g_bAtmValveManualCommand; 
// g_bFilterManualCommand; g_bVacLoaderSystemClearAlarms; g_bVacLoaderManualCommandWritten 
// and .m_bManualCommand in loader structure.
// Renamed m_nDumpDelayTime -> m_nDischargeTime.
// Renamed m_nDumpDelay -> m_nDischargeTmr.
// Added g_nVAC8IOCards; g_nWriteVAC8Card2WordOps1; g_nWriteVAC8Card2WordOps2, g_nWriteVAC8Card3WordOps1; g_nWriteVAC8Card3WordOps2
// Added g_nVAC8IOPumpCardNo;
// Added m_bOutputState to loader structure
// Rearranged Vac calibration structure (stored in EEPROM in this format) structVacCalib, so that
// the loaders hash follows the no. of loaders directly. Wont be changed if no. of loaders increased.
//
// P.Smith                      29/9/09
// added MAX_FILTER_REPETITIONS, DEFAULT_FILTER_REPETITIONS        
// MAX_PUMP_IDLE_TIME increased to 15 minutes
//
// P.Smith                      29/9/09
// define DEFAULT_PUMP_IDLE_TIME as 10 extern g_nLoadersHistory
//
// P.Smith                      30/9/09
// MAX_LOADERS set to 3
/////////////////////////////////////////////////

//*****************************************************************************
// MACROS
//*****************************************************************************
#ifndef __VACVARS_H__
#define __VACVARS_H__


#include "General.h"
#include <basictypes.h>
#include "Alarms.h"
#include "TSMPeripheralsMBIndices.h"



#define  PUMP_RUN_TIME_MAX          (3600*10)      // seconds i.e. 10 hours.
#define  AV_OPEN_TO_PUMP_ON_TIME    (2)            // time Atm Valve open until pump switch on
#define  PUMP_STARTUP_TIME          (5)         // time in seconds. for pump to ramp up to full speed.
#define  SYSTEM_IDLE_TIMER_MAX      (3600)      // seconds i.e. 1 hour.
#define  AV_OPEN_TIMER_MAX          (3600)      //
#define  MAX_LOADERS                (24)         // max possible no of loaders
#define  TO_FILL_TABLE_SIZE         (MAX_LOADERS+2)   
#define  FILLS_TO_ALARM             (2)
#define  IDLE_TIME_TO_FILTER_CLEAN  (5)
#define  FILTER_ON_TIME             (10)         // tenths of second i.e. 1 second
#define  FILTER_OFF_TIME            (20)         // tenths of second i.e. 2 seconds.
#define  PUMP_START_TIME_MIN        (3)
#define  PUMP_START_TIME_MAX        (10)

// definition of bits in outputs status word from VAC8 (i.e. state of outputs on VAC8)      
#define  PUMP_STATUS_BIT   (0x0001)
#define  AV_STATUS_BIT     (0x0004)
#define  FILTER_STATUS_BIT (0x0008)
#define  MAX_LOADER_PRIORITY  (7)

#define  MAX_OVERLAP_TIME              (5)
#define  DEFAULT_OVERLAP_TIME          (2)
#define  MAX_SUCTION_TIME              (99)
#define  MIN_SUCTION_TIME              (2)
#define  DEFAULT_SUCTION_TIME          (10)
#define  MAX_POST_FILL_TIME            (5) 
#define  DEFAULT_POST_FILL_TIME        (0)
#define  MAX_DISCHARGE_TIME                (20)
#define  MIN_DISCHARGE_TIME                (4)
#define  DEFAULT_DISCHARGE_TIME            (6)
#define  DEFAULT_PRIORITY                  (3)

#define  MAX_FILTER_REPETITIONS            (5)
#define  DEFAULT_FILTER_REPETITIONS        (2)



#define  MAX_PUMP_IDLE_TIME            (15*60)     // idle time in secs, i.e. 15 minutes.
#define  DEFAULT_PUMP_IDLE_TIME        (10)     


#define  PUMP_START_TIME_MIN           (3)
#define  PUMP_START_TIME_MAX           (10)

#define  LOADER_STATUS_NORMAL          (0)
#define  LOADER_STATUS_IN_ALARM        (1)
#define  LOADER_STATUS_OVERRIDE_REQUEST (2)
#define  LOADER_STATUS_UNUSED          (3)
#define  LOADER_VALVE_OVERLAP_TIME     (2)

void InitialiseVacVars( void );
void InitialiseLoaderStructures( void );
void HardCodeVacCalibrationData( void);




// Calibration data struct
typedef struct {
   BYTE  m_nLoaders;             // no. of loaders in Vac System.
   WORD  m_nLoaderHash;          // moved here so that it wont change if no. loaders change.         
   WORD  m_nAVOverLapTime;
   
   BYTE  m_nDischargeTime;    // time it takes for a loader to empty.
   WORD  m_nPumpIdleTime;     // idle time before turn off.
   BYTE  m_nPumpStartTime;    // seconds to allow pump to start after turn on.
   BYTE  m_nFilterRepetitions;
   BYTE  m_nFilterType;    // 0 = none. 1 = TSM standard.
   WORD  m_nSpareWord1;          // leave a spare.
   WORD  m_nSpareWord2;          // leave a 2nd spare.
   
   WORD  m_nSuctionTime[MAX_LOADERS];
   BYTE  m_nPostFillTime[MAX_LOADERS];
   BYTE  m_nPriority[MAX_LOADERS];  // 1-7, 1=highest.
   bool  m_bOnFlag[MAX_LOADERS];
   
//NB: The checksum must be the last parameter inthe structure.
   unsigned int  m_nChecksum;            // CHECKSUM FOR CAL. DATA
   
} structVacCalib;
// structure for data for each loader
typedef struct {
   WORD  m_nLoaderNo;         // 1-8 etc.
//   bool  m_bOnFlag;
   bool  m_bFillValve;        // will be ON or OFF
   BYTE  m_nStatusByte;       //
   WORD  m_nFillTimer;        // 
   WORD  m_nOverLapTimer;     //
   WORD  m_nDischargeTmr;        // delay in seconds before seeking re-fill.
   BYTE  m_nFillCounter;      //
   bool  m_bAlarm;
   BYTE  m_nPriority;         //
   bool  m_bLevelFlag;
   bool  m_bBlenderFillValveFlag;   // used to inhibit loading
   bool  m_bInhibit;
   bool  m_bHiLevelSensor;
   BYTE  m_nLevelSimulator;      // for simulation.
//New 7.Sept.2009
   bool  m_bManualCommand;       // ON/OFF - loader command in manual mode.
   bool  m_bOutputState;         // state of output as read back from VAC8io.   
  
} structVacLoaderData;

extern   bool  g_bAtmValveFlag;
extern   bool  g_bFilterFlag;
extern   bool  g_bFilterSequenceInProgress;
extern   bool  g_bFilterSequenceFinished;
extern   WORD  g_nPumpRunTimer;      // time in seconds pump is running
extern   bool  g_bPumpFlag;         // whether pump should be on/off - i.e. commanded
extern   bool  g_bPumpStatus;       // current status of pump.
extern   bool  g_bPumpReady; 
extern   bool  g_bAtmValveStatus;
extern   bool  g_bFilterStatus;
extern   int   g_nFilterOnTimer;
extern   int   g_nFilterOffTimer;
extern   int   g_nFilterRepetitionsCounter;

extern   BYTE  g_nFillingUnit;
extern   WORD  g_nFillingTime;      // filling time
extern   BYTE  g_nOverRideRequest;   // will contain the no. of loader seeking Over-ride.

extern   WORD  g_nSystemIdleTimer;  //
extern   WORD  g_nAVOpenTimer;      // seconds - time while AV open
extern   WORD  g_nAVIsOpenTimer;
extern   WORD  g_nAVOverLapTimer;      // 10ths of second.

extern   BYTE  g_nToFillTable[TO_FILL_TABLE_SIZE];  // table for fill requests - 
                                                   // highest priority at top (0).

extern   WORD  g_nWriteVAC8WordOps1;
extern   WORD  g_nWriteVAC8WordOps2;

extern   BOOL        g_bVAC8IOReadDataAvailable[MAX_VAC8IO_CARDS];                                            // new variable
extern   BOOL        g_bVAC8IOReadDataReady[MAX_VAC8IO_CARDS]; 
extern   BOOL  g_bSaveLoaderCalibrationData;
extern   BOOL  g_bVacuumLoaderCalibrationWritten;
extern   BYTE  g_nVacLoaderAlarms1;
extern   BYTE  g_nVacLoaderAlarms2;
extern   BYTE  g_nVacLoaderAlarms3;

extern   BOOL  g_bVacSimulatorRunning;

//New 7.Sept.2009
extern   bool  g_bVacLoaderSystemInManual;      // 
extern   bool  g_bPumpManualCommand;            // .
extern   bool  g_bAtmValveManualCommand;        // 
extern   bool  g_bFilterManualCommand;          //
extern   bool  g_bVacLoaderSystemClearAlarmsCommand;   // 
extern   bool  g_bVacLoaderSystemEnterManual;
extern   bool  g_bVacLoaderSystemExitManual;
extern   bool  g_bVacLoaderManualCommandWritten;      //
extern   BYTE  g_nVAC8IOCards;
extern   WORD  g_nWriteVAC8Card2WordOps1;
extern   WORD  g_nWriteVAC8Card2WordOps2;
extern   BYTE  g_nVAC8IOPumpCardNo;
extern   WORD  g_nWriteVAC8Card3WordOps1;
extern   WORD  g_nWriteVAC8Card3WordOps2;
extern   structVacCalib   g_UploadedVacuumLoadingData;
extern   structVacCalib   g_VacCalibrationData;
extern   BYTE g_nVacLoaderAlarmsTotal;
extern   BYTE  g_nLoadersHistory;


#endif //__VACVARS_H_
