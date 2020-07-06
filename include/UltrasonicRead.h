//////////////////////////////////////////////////////
// P.Smith                        1-02-2005
// First pass
//
// M.McKiernan					  8-03-2010
//Added functions PutDataInUSBuffer(  void ) InitialiseUSBuffer(  void );
// Added structure USDataStruct
//
// P.Smith					  16/09/10
// moved ultrasonic definitions to here.
//////////////////////////////////////////////////////

#ifndef __ULTRASONICREAD_H__
#define __ULTRASONICREAD_H__

#include <basictypes.h>


#define  TMR_ORRI_BIT (0x0010)
#define  T0_TRIGGER (0)
#define  T0_OSCILLATION (1)
#define  T0_INPUT_CAPTURE (2)
#define  T0_IDLE (3)
//#define  WMS_TRIGGER_TIME (6015)      //80uS for WMS30 (13.3nS per time period)
#define  WMS_TRIGGER_TIME (3760)      //50uS for WMS30 (13.3nS per time period)//nbb--todo--width--remove from here

//#define  WMS_OSCILLATION_TIME (26000)  // ~350uS for WMS30
#define  WMS_OSCILLATION_TIME (90000)  // ~350uS for WMS30 nbb--todo--width--remove

#define  ULTRASONIC_TIMER    0


#define  MAX_ENTRIES_USBUF		(60)

typedef struct{
	DWORD	m_lUSBuf[MAX_ENTRIES_USBUF];
	DWORD 	m_lAverageUSBuf;
	DWORD 	m_lSumUSBuf;
	int		m_nIndexUSBuf;
	int		m_nNoOfEntriesUSBuf;
	int		m_nEntriesInSumUSBuf;
	bool  	m_bNewDataAvailable;

}USDataStruct;

void SetupTimer2( void );
void SetupTimer0( void );
void ReadUltrasonicTime( void );
void SetupTimer0( void );
void TriggerUltrasonic( void );
void ReadUltrasonicSensorTime ( void );

void PutDataInUSBuffer(  DWORD lNewValue );
void InitialiseUSBuffer(  void );

void UltrasonicCycleReset( void );
float CalculateUSRRAvgUSDistance( void );
void CalculateCountsPermm(  void );


#endif   // __ULTRASONICREAD_H__


