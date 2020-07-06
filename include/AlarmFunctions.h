//////////////////////////////////////////////////////
// AlarmFunctions.h
//
// Defines the data structure for the alarm history
// queue.
//
// M.Parks								19-04-2000
// First pass.
// M.McKiernan 17-10-2002	ALARM_BLENDING_PERCENTAGE definition added.
// 
// P.Smith                      23/1/06
// first pass at netburner hardware conversion.
// BOOL -> BOOL
///////////////////////////////////////////////////////

//*****************************************************************************
// MACROS
//*****************************************************************************
#ifndef __ALARMFUNCTIONS_H__
#define __ALARMFUNCTIONS_H__

//*****************************************************************************
// INCLUDES
//*****************************************************************************
#include <stdlib.h>

#include "TimeDate.h"


#define	MAX_ALARMS	100

// Alarm types
#define	ALARM_MIXER_MOTOR				0
#define	ALARM_ZERO_LINESPEED			1
#define	ALARM_BLENDER_SHUTDOWN			2
#define	ALARM_FILL						3
#define	ALARM_RESERVOIR_LOW_LEVEL		4
#define	ALARM_SEI_COMMS					5
#define	ALARM_EEPROM_WRITE				6
#define	ALARM_INVALID_CALIBRATION		7
#define	ALARM_INVALID_SETPOINTS			8
#define	ALARM_WEIGHT_CHANGE				9
#define	ALARM_EXTRUDER_STALLED			10
#define	ALARM_HOPPER_NOT_EMPTY			11
#define	ALARM_SENSOR_BEFORE_FILL		12
#define	ALARM_EXTRUDER_CONTROL			13
#define	ALARM_HOPPER_FILL				14
#define	ALARM_COMPONENT_LOW_LEVEL		15
#define	ALARM_FRONT_TIMEOUT				16
#define	ALARM_BACK_TIMEOUT				17
#define	ALARM_ORDER_TIMEOUT				18
#define	ALARM_LOADER_FILL				19
#define	ALARM_COMMS_TIMEOUT				20
#define	ALARM_AUTO_STATE				21
#define	ALARM_BLENDING_PERCENTAGE		22
#define	ALARM_TYPE_COUNT				23

#define	ALARM_INVALID_MESSAGE			-1


// Alarm states
#define	ALARM_ACTIVE	TRUE
#define	ALARM_INACTIVE	FALSE


// Alarm entry
typedef struct{
	int		m_nAlarmType;				// Indicates the type of alarm
	BOOL	m_bState;					// TRUE = Active, FALSE = inactive
	char	m_cTime[TIME_ARRAY_SIZE];	// Time of the event
	int		nIndex;						// Index into the equipment array.  Used to retrieve the source name.
	int		nComponent;					// Only used if it's a blender component alarm
										// Could maybe be used by the loaders to show which component
										// they're mounted on.
} structAlarmMessage;


// Structure holding the current alarm history
typedef struct {
	structAlarmMessage	m_arrAlarmMessages[MAX_ALARMS];
	int		m_nQueueHead;
	int		m_nQueueTail;
	BOOL	m_bEmpty;
} structAlarmHistory;



// Function declarations
void	InitialiseAlarmHistory( void );
int		GetMostRecentAlarmFromQueue( void );
int		GetPreviousAlarmFromQueue( int nCurrentIndex );
void	AddToAlarmQueue( structAlarmMessage *alarmMessage );

#endif	// __ALARMFUNCTIONS_H__
