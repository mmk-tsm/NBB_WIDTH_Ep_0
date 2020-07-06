//////////////////////////////////////////////////////
// P.Smith                      29/10/08
// history log header file
//
// P.Smith                      2/3/09
// added leak alarms, dump flap alarms, keep all alarms together
// added CopyOngoingTo24HourlyHistoryLog,ClearOngoing24HrHistoryStructures
// name change to events, remove history from these
//
// P.Smith                      27/4/10
// add CopyHistoricDataToModbus
//////////////////////////////////////////////////////

#ifndef __HISTORYLOG_H__
#define __HISTORYLOG_H__
void CalculateOngoingMaxMinTareValue( void );
void CopyOngoingTareToMaxMin( void );
void IncrementOngoingModulesHistoryGoodTxCtr( void );
void IncrementOngoingModulesHistoryGoodRxCtr( void );
void IncrementOngoingModulesHistoryChecksumErrorCtr( void );
void CalculateCommsStatistics( void );
void IncrementOngoingModulesHistoryTimeoutCtr( void );
void ClearHourHistoryStructures( void );
void ClearOngoingHistoryStructures( void );
void OneHzHistoryLog( void );
void CopyOngoingToHourlyHistoryLog( void );
void CopyOngoingTo24HourlyHistoryLog( void );
void ClearOngoing24HrHistoryStructures( void );
void CopyHistoricDataToModbus( void );




//  Component Historic data
typedef struct {
   unsigned int   m_nComponentRetryCtr[MAX_COMPONENTS];
   unsigned int   m_nHistoryComponentActivations[MAX_COMPONENTS];
   unsigned int   m_nHistoryTareComponentVarianceCtr[MAX_COMPONENTS];
   float          g_fComponentWeightMaximum[MAX_COMPONENTS];
   float          g_fComponentWeightMinimum[MAX_COMPONENTS];
} structHistoryComponentData;

//  Historic data
typedef struct {
   unsigned int   m_nMotorTrips;
   unsigned int   m_nDoorOpenings;
   long           m_lTareMaxValue;
   long           m_lTareMinValue;
   unsigned int   m_nLevelSensorSwitchesCtr;
   unsigned int   m_nLevelSensorSwitchesAcknowledged;
   BOOL           m_bAutoState;
   unsigned int   m_nLineSpeedChangesDetectedCtr;
   unsigned int   m_nTareAlarms;
   unsigned int   m_nControlAlarmsCtr;
   unsigned int   m_nLeakAlarmsCtr;
   unsigned int   m_nDumpFlapNotOpenAlarmsCtr;
   unsigned int   m_nResetCtr;

} structHistoryIndividualEvents;


// comms
// Structure holding the basic requirements for a controller (structControllerData)
typedef struct {

    // Comms message counters
    long    m_lGoodTx;
    long    m_lGoodRx;
    long    m_lRxTimeoutCounter;
    long    m_lChecksumErrorCounter;
    float   m_fPercentageGoodTxRx;
    float   m_fPercentageTimeout;
    float   m_fPercentageChecksumErrors;
    BOOL    m_bNetworkCommsOkay;
    BOOL    m_bTCPIPCommsOkay;
    BOOL    m_bSEICommsOkay;
    BOOL    m_bLLSCommsOkay;
    BOOL    m_bOPTCommsOkay;

} structHistoricCommsData;




#endif  // __HISTORYLOG_H__

