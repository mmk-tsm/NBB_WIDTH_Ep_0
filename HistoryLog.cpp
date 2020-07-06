//////////////////////////////////////////////////////
//
// P.Smith                      3/12/08
// history log pass 1
//
// P.Smith                      4/12/08
// added CalculateOngoingMaxMinTareValue
//
// P.Smith                      9/12/08
// in CalculateOngoingMaxMinTareValue call CopyOngoingTareToMaxMin if first weigh cycle.
//
// P.Smith                      15/1/09
// move CopyOngoingTareToMaxMin, this is in the wrong place as the value will be cleared after it has
// been set up.
//
// P.Smith                      15/1/09
// added CheckFor24HourRollOver to check for 24 hour roll over
// name change m_lTareMaxValue, m_lTareMinValue remove history ref
// CopyOngoingTo24HourlyHistoryLog copies 24 hour data to 24 hour storage and
// clears ongoing
// reset and motor trips are copied seperately.
//
// P.Smith                      19/11/09
// correct copy of counters for comms, only check the current unit, not the
// no of units installed.
//
// P.Smith                      27/4/10
// added CopyHistoricDataToModbus to copy historic data to modbus
// no of hours blender is running,valve activations per component
// component retries in previous hour
// motor trips, door openings,level sensor activations, leak alarms
// dump alarms, max/min tare value, line speed change detected
// control alarms, no of minutes in auto
//
// P.Smith                      28/4/10
// corrected the copy of historic log valve activations, the indexing of the data was
// incorrect.
//////////////////////////////////////////////////////




#include <stdio.h>

#include "SerialStuff.h"
#include "BatchMBIndices.h"
#include "BatchCalibrationFunctions.h"
#include "General.h"
#include "ConversionFactors.h"

#include "16R40C.h"
#include "ConfigurationFunctions.h"
#include "MBMHand.h"
#include "MBSHand.h"

#include "MBProgs.h"
#include "SetpointFormat.h"
#include "TimeDate.h"
#include "BatVars.h"
#include "BBCalc.h"
#include "MBTogSta.h"
#include "Alarms.h"
#include "KghCalc.h"
#include "BatVars2.h"
#include "Monbat.h"
#include "Blrep.h"
#include "MBProsum.h"
#include "Pause.h"
#include "CycleMonitor.h"
#include "TSMPeripheralsMBIndices.h"
#include "BSP.h"
#include <System.h>

extern structCommsData         g_sModbusMaster;
extern  structControllerData    *arrpDeviceList[];

/******************************************************************
 *  CopyTareToMaxMin
 ******************************************************************/
void CopyOngoingTareToMaxMin( void )
{
    g_sOngoingHistoryEventsLogData.m_lTareMaxValue = g_lHopperTareCounts;
    g_sOngoingHistoryEventsLogData.m_lTareMinValue = g_lHopperTareCounts;
}



/******************************************************************
 *  CalculateOngoingMaxMinTareValue
 store tare to max / min
 ******************************************************************/
void CalculateOngoingMaxMinTareValue( void )
{
    if(g_lHopperTareCounts > g_sOngoingHistoryEventsLogData.m_lTareMaxValue)
    {
        g_sOngoingHistoryEventsLogData.m_lTareMaxValue = g_lHopperTareCounts;
    }
    if(g_lHopperTareCounts < g_sOngoingHistoryEventsLogData.m_lTareMinValue)
    {
        g_sOngoingHistoryEventsLogData.m_lTareMinValue = g_lHopperTareCounts;
    }
    if(g_bFirstWCycle)  // initialise if 1st weigh cycle.
    {
        CopyOngoingTareToMaxMin();
    }

}




/******************************************************************
   IncrementOngoingModulesHistoryGoodTxCtr
   increment comms counter
 ******************************************************************/
void IncrementOngoingModulesHistoryGoodTxCtr( void )
{
    switch(g_sModbusMaster.m_pController->m_nSystemType)
    {
        case EQUIP_SEI:
            g_sOngoingHistorySEIComms.m_lGoodTx++;
            break;
        case EQUIP_LLS:
            g_sOngoingHistoryLLSComms.m_lGoodTx++;
            break;
        case EQUIP_OPTIMISATION:
            g_sOngoingHistoryOPTComms.m_lGoodTx++;
            break;
        default:
            break;

    }  // end of switch statement.
}





/******************************************************************
 *  IncrementOngoingModulesHistoryGoodTxCtr
   increment comms counter
 ******************************************************************/
void IncrementOngoingModulesHistoryGoodRxCtr( void )
{
    switch(g_sModbusMaster.m_pController->m_nSystemType)
    {
    case EQUIP_SEI:
        g_sOngoingHistorySEIComms.m_lGoodRx++;
        break;
    case EQUIP_LLS:
        g_sOngoingHistoryLLSComms.m_lGoodRx++;
        break;
    case EQUIP_OPTIMISATION:
        g_sOngoingHistoryOPTComms.m_lGoodRx++;
        break;

    default:
    break;
    }  // end of switch statement.
}


/******************************************************************
 *  IncrementOngoingModulesHistoryChecksumErrorCtr
   increment comms counter
 ******************************************************************/
void IncrementOngoingModulesHistoryChecksumErrorCtr( void )
{
    switch(g_sModbusMaster.m_pController->m_nSystemType)
    {
    case EQUIP_SEI:
        g_sOngoingHistorySEIComms.m_lChecksumErrorCounter++;
        break;
    case EQUIP_LLS:
        g_sOngoingHistoryLLSComms.m_lChecksumErrorCounter++;
        break;
    case EQUIP_OPTIMISATION:
        g_sOngoingHistoryOPTComms.m_lChecksumErrorCounter++;
        break;

    default:
    break;
    }  // end of switch statement.
}

/******************************************************************
 *  IncrementOngoingModulesHistoryTimeoutCtr
   increment comms counter
 ******************************************************************/
void IncrementOngoingModulesHistoryTimeoutCtr( void )
{
    switch(g_sModbusMaster.m_pController->m_nSystemType)
    {
    case EQUIP_SEI:
        g_sOngoingHistorySEIComms.m_lRxTimeoutCounter++;
        break;
    case EQUIP_LLS:
        g_sOngoingHistoryLLSComms.m_lRxTimeoutCounter++;
        break;
    case EQUIP_OPTIMISATION:
        g_sOngoingHistoryOPTComms.m_lRxTimeoutCounter++;
        break;
        default:
        break;
    }
}


/******************************************************************
 *  CopyOngoingToHourlyHistoryLog
 ******************************************************************/
void CopyOngoingToHourlyHistoryLog( void )
{
    CalculateCommsStatistics(); // calculate comms statistics.
    g_sHourHistoryComponentLogData = g_sOngoingHistoryComponentLogData;
    g_sHourHistoryEventsLogData = g_sOngoingHistoryEventsLogData;
    g_sHourHistorySEIComms = g_sOngoingHistorySEIComms;
    g_sOngoingHistoryLLSComms = g_sHourHistoryLLSComms;
    g_sOngoingHistoryOPTComms = g_sHourHistoryOPTComms;
    g_sOngoingHistoryTCPIPComms = g_sHourHistoryTCPIPComms;
    g_sOngoingHistoryNetworkComms = g_sHourHistoryNetworkComms;
    g_bHourlyResetMin = TRUE;
    g_bHourlyResetMax = TRUE;   // hourly max will become the reading on the next cycle
    ClearOngoingHistoryStructures();
    CopyOngoingTareToMaxMin();

}

/******************************************************************
 *  CalculateCommsStatistics
 ******************************************************************/
void CalculateCommsStatistics( void )
{
// calculate comms statistics
//sei
   // rx/tx %
   if(g_sOngoingHistorySEIComms.m_lGoodTx > 0)
   {
       g_sOngoingHistorySEIComms.m_fPercentageGoodTxRx = 100.0 * (float)g_sOngoingHistorySEIComms.m_lGoodRx/(float)g_sOngoingHistorySEIComms.m_lGoodTx;
       // rx/tx %
       g_sOngoingHistorySEIComms.m_fPercentageChecksumErrors = 100.0 * (float)g_sOngoingHistorySEIComms.m_lChecksumErrorCounter/(float)g_sOngoingHistorySEIComms.m_lGoodTx;
       g_sOngoingHistorySEIComms.m_fPercentageTimeout = 100.0 * (float)g_sOngoingHistorySEIComms.m_lRxTimeoutCounter/(float)g_sOngoingHistorySEIComms.m_lGoodTx;
   }
//lls
   // rx/tx %
   if(g_sOngoingHistoryLLSComms.m_lGoodTx > 0)
   {
       g_sOngoingHistoryLLSComms.m_fPercentageGoodTxRx = 100.0 * (float)g_sOngoingHistoryLLSComms.m_lGoodRx/(float)g_sOngoingHistorySEIComms.m_lGoodTx;
       // rx/tx %
       g_sOngoingHistoryLLSComms.m_fPercentageChecksumErrors = 100.0 * (float)g_sOngoingHistoryLLSComms.m_lChecksumErrorCounter/(float)g_sOngoingHistorySEIComms.m_lGoodTx;
       g_sOngoingHistoryLLSComms.m_fPercentageTimeout = 100.0 * (float)g_sOngoingHistoryLLSComms.m_lRxTimeoutCounter/(float)g_sOngoingHistoryLLSComms.m_lGoodTx;
   }
//OPT
   // rx/tx %
   if(g_sOngoingHistoryOPTComms.m_lGoodTx > 0)
   {
       g_sOngoingHistoryOPTComms.m_fPercentageGoodTxRx = 100.0 * (float)g_sOngoingHistoryOPTComms.m_lGoodRx/(float)g_sOngoingHistoryOPTComms.m_lGoodTx;
       // rx/tx %
       g_sOngoingHistoryOPTComms.m_fPercentageChecksumErrors = 100.0 * (float)g_sOngoingHistoryOPTComms.m_lChecksumErrorCounter/(float)g_sOngoingHistorySEIComms.m_lGoodTx;
       g_sOngoingHistoryOPTComms.m_fPercentageTimeout = 100.0 * (float)g_sOngoingHistoryOPTComms.m_lRxTimeoutCounter/(float)g_sOngoingHistoryOPTComms.m_lGoodTx;
   }

//network
   // rx/tx %
   if(g_sOngoingHistoryNetworkComms.m_lGoodTx > 0)
   {
       g_sOngoingHistoryNetworkComms.m_fPercentageChecksumErrors =  100.0 * (float)g_sOngoingHistoryNetworkComms.m_lChecksumErrorCounter/(float)g_sOngoingHistoryNetworkComms.m_lGoodTx;
   }


}


/******************************************************************
 *  ClearOngoingHistoryStructures
 ******************************************************************/
void ClearHourHistoryStructures( void )

{
    memset( &g_sHourHistoryComponentLogData, 0, sizeof( structHistoryComponentData ) );
    memset( &g_sHourHistoryEventsLogData, 0, sizeof( structHistoryIndividualEvents ) );
    memset( &g_sHourHistorySEIComms, 0, sizeof( structHistoricCommsData ) );
    memset( &g_sHourHistoryLLSComms, 0, sizeof( structHistoricCommsData ) );
    memset( &g_sHourHistoryOPTComms, 0, sizeof( structHistoricCommsData ) );
    memset( &g_sHourHistoryTCPIPComms, 0, sizeof( structHistoricCommsData ) );
    memset( &g_sHourHistoryNetworkComms, 0, sizeof( structHistoricCommsData ) );
}


/******************************************************************
 *  ClearOngoingHistoryStructures
 ******************************************************************/
void ClearOngoingHistoryStructures( void )
{
    memset( &g_sOngoingHistoryComponentLogData, 0, sizeof( structHistoryComponentData ) );
    memset( &g_sOngoingHistoryEventsLogData, 0, sizeof( structHistoryIndividualEvents ) );
    memset( &g_sOngoingHistorySEIComms, 0, sizeof( structHistoricCommsData ) );
    memset( &g_sOngoingHistoryLLSComms, 0, sizeof( structHistoricCommsData ) );
    memset( &g_sOngoingHistoryOPTComms, 0, sizeof( structHistoricCommsData ) );
    memset( &g_sOngoingHistoryTCPIPComms, 0, sizeof( structHistoricCommsData ) );
    memset( &g_sOngoingHistoryNetworkComms, 0, sizeof( structHistoricCommsData ) );
}

/******************************************************************
 *  OneHzHistoryLog
 ******************************************************************/
void OneHzHistoryLog( void )
{
    CalculateCommsStatistics();
}


/******************************************************************
 *  CopyOngoingTo24HourlyHistoryLog
 copies 24 hour data and clear ongoing structure
 ******************************************************************/
void CopyOngoingTo24HourlyHistoryLog( void )
{
    g_s24HrHistoryEventsLogData = g_s24HrOngoingHistoryEventsLogData;  // copy 24 hour data
    g_s24HrHistoryEventsLogData.m_nResetCtr = g_ProcessData.m_n24HourOngoingResetCounter;
    g_s24HrHistoryEventsLogData.m_nMotorTrips = g_ProcessData.m_n24HourOngoingMotorTripCounter;
    // reset counter
    g_ProcessData.m_n24HourOngoingResetCounter = 0;
    g_ProcessData.m_n24HourOngoingMotorTripCounter = 0;
    g_bSaveAllProcessDataToEEprom = TRUE;
    g_bAllowProcesDataSaveToEEPROM = TRUE;
    ClearOngoing24HrHistoryStructures();
}


/******************************************************************
 *  ClearOngoing24HrHistoryStructures
 ******************************************************************/
void ClearOngoing24HrHistoryStructures( void )
{
    memset( &g_s24HrOngoingHistoryEventsLogData, 0, sizeof( structHistoryIndividualEvents ) );
}

/******************************************************************
 *  CopyHistoricDataToModbus
 *  copy historic log data to modbus
 *
 ******************************************************************/
void CopyHistoricDataToModbus( void )
{
	unsigned int i;
	g_arrnMBTable[BATCH_SUMMARY_BLENDER_RUN_TIME] = (g_ProcessData.m_lHoursBlenderIsRunning >> 16)& 0xffff;
	g_arrnMBTable[BATCH_SUMMARY_BLENDER_RUN_TIME+1] = (WORD)g_ProcessData.m_lHoursBlenderIsRunning & 0xffff;
    for(i = 0; i < MAX_COMPONENTS; i++ )  // copy component activations
    {
    	g_arrnMBTable[BATCH_SUMMARY_VALVE_OPERATIONS_1+(i*2)] = (WORD)(g_ProcessData.m_lComponentActivations[i] >> 16)& 0xffff;
    	g_arrnMBTable[BATCH_SUMMARY_VALVE_OPERATIONS_1+(i*2)+1] =(WORD) g_ProcessData.m_lComponentActivations[i] & 0xffff;
    	g_arrnMBTable[BATCH_HISTORIC_LOG_RETRY_C1_IN_HOUR+i] = g_sHourHistoryComponentLogData.m_nComponentRetryCtr[i];
    }
	g_arrnMBTable[BATCH_HISTORIC_LOG_MOTOR_TRIPS_IN_HOUR] = (WORD)g_sHourHistoryEventsLogData.m_nMotorTrips;
	g_arrnMBTable[BATCH_HISTORIC_LOG_DOOR_OPENINGS_IN_HOUR] = (WORD)g_sHourHistoryEventsLogData.m_nDoorOpenings;
	g_arrnMBTable[BATCH_HISTORIC_LEVEL_SENSOR_ACTIVATIONS_IN_HOUR] = (WORD)g_sHourHistoryEventsLogData.m_nLevelSensorSwitchesAcknowledged;
	g_arrnMBTable[BATCH_HISTORIC_LOG_TARE_ALARMS_IN_HOUR] = (WORD)g_sHourHistoryEventsLogData.m_nTareAlarms;
	g_arrnMBTable[BATCH_HISTORIC_LEAK_ALARMS_IN_HOUR] = (WORD)g_sHourHistoryEventsLogData.m_nLeakAlarmsCtr;
	g_arrnMBTable[BATCH_HISTORIC_DUMP_FLAP_ALARMS_IN_HOUR] = (WORD)g_sHourHistoryEventsLogData.m_nDumpFlapNotOpenAlarmsCtr;
	g_arrnMBTable[BATCH_HISTORIC_LOG_MAX_TARE_VALUE_IN_HOUR] = (WORD)g_sHourHistoryEventsLogData.m_lTareMaxValue;
	g_arrnMBTable[BATCH_HISTORIC_LOG_MIN_TARE_VALUE_IN_HOUR] = (WORD)g_sHourHistoryEventsLogData.m_lTareMinValue;
	g_arrnMBTable[BATCH_HISTORIC_LOG_LINE_SPEED_CHANGE_DETECTED] = (WORD)g_sHourHistoryEventsLogData.m_nLineSpeedChangesDetectedCtr;
	g_arrnMBTable[BATCH_HISTORIC_LOG_CONTROL_ALARMS_IN_HOUR] = (WORD)g_sHourHistoryEventsLogData.m_nControlAlarmsCtr;
	g_arrnMBTable[BATCH_HISTORIC_LOG_MINUTES_IN_AUTO] = (WORD)g_nTimeInAutoMinutesCtr;
}

