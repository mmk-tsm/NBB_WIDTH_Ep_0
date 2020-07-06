///////////////////////////////////////////////////////////
// Alarms.h
//
// Alarm handling structures and function prototypes
//
// M.McKiernan 							08-12-2004
// First Pass  
//
// M.McKiernan 							15-12-2004
// Added 3 extra fields to alarm entry structure. 
//	
// P.Smith                      16/2/06
// first pass at netburner hardware conversion.
// Bool -> BOOL
// #include <basictypes.h>
//	
// P.Smith                      28/1/08
// added m_nComponentDeviationAlarmWord
//
// P.Smith                      26/9/08
// change ucAlarmCode to int in PutAlarmTable & RemoveAlarmTable
//
// P.Smith                      1/10/08
// make m_ucAlarmCode an int
///////////////////////////////////////////////////////////

//*****************************************************************************
// MACROS
//*****************************************************************************
#ifndef __ALARMS_H__
#define __ALARMS_H__

//*****************************************************************************
// INCLUDES
//*****************************************************************************
#include <basictypes.h>

#define MAX_ALARMS	14		//
/// Alarm entry data
typedef	struct {
	int		m_nComponent;
	unsigned int m_ucAlarmCode;					// 
	char		m_cMonth;
	char		m_cDate;
	char		m_cHour;
	char		m_cMinute;
	int		m_nFrontRollNo;
	int		m_nBackRollNo;
	int		m_nFrontRollLengthAccumulator;	// --review-- , may need a long or a float.
	int		m_nBackRollLengthAccumulator;		// --review-- , may need a long or a float.
	int		m_nOrderNo;
	int		m_nOperatorNo;
	int		m_nReserved;			// for future use.
	BOOL		m_bAlarmState;									// flag to indicate if alarm is being set or cleared. (TRUE => set)
} StructAlarmEntry;

// alarm table
typedef	struct {
	StructAlarmEntry	m_AlarmArray[MAX_ALARMS];

	StructAlarmEntry	m_AlarmHistory[MAX_ALARMS];

	BOOL	m_bAlarmChangeFlag;	// flag to show alarm status has changed.	ALARMCFG
	int	m_nNoOfArrayAlarms;			// No. of alarms in array or table.
	int	m_nNoOfHistoryAlarms;		// No. of alarms in history.
	unsigned int m_nAlarmWord;				// one bit per component, if comp in alarm corresponding bit set.
	unsigned int m_nComponentLLSAlarmWord; // one bit per comp. low level sensor alarm bit.
	unsigned int m_nComponentFillAlarmWord; // one bit per comp. Fill alarm bit. // CFILLALARM 
    unsigned int m_nComponentDeviationAlarmWord; // one bit per comp. Fill alarm bit. // CFILLALARM 


} StructAlarmTable;


void PutAlarmTable( unsigned int ucAlarmCode,  int nComponent );
void RemoveAlarmTable( unsigned int ucAlarmCode,  int nComponent );

void SetModbusAlarmBits( unsigned int ucAlarmCode,  int nComponent );
void ClearModbusAlarmBits( unsigned int ucAlarmCode,  int nComponent );

unsigned int GetAlarmBitMask( unsigned int ucAlarmCode );
void StoreAlarmInArrays( unsigned int ucAlarmCode,  int nComponent, BOOL bSet );

void InitialiseAlarmTable( void );
void CheckForAlarm( void );
void CopyAlarmTableToMB( void );

#endif	// __ALARMS_H__
