// **********************************************************
//  Alarms.c                                                    *
//   Alarm handling functions                                           *
//
//                                                          *
// M.McKiernan                      08-12-2004                  *
//  First Pass
//
//                                                          *
// M.McKiernan                      15-12-2004                  *
//  Edited to account for extra parameters stored in alarm
// entry, i.e. OrderNo and Operator No.
// Setting bits in MB Process alarm word for fill and LLS.
//
// P.Smith                      3-3-2005
// Check for zero % in PutAlarmTable is only done when the component no is non zero.
//
// P.Smith                      1-6-2005
// Correct copy component fill alarms to modbus table.
//
// P.Smith                      9-8-2005
// Copy roll info into alarm table.
//
//
// P.Smith                      5/9/05
// added copy of operator no & order no in alarm copy.
//.
// P.Smith                      28/9/05
// added license alarm.
//.
//.
// P.Smith                      7/10/05
// implement  Set ToPrintFlag, g_bPrintAlarmReport set to TRUE;
//
// P.Smith                      21/12/05
// use g_bIsSystemAlarm to determine if the alarm is a system alarm or not when
// setting / clearing alarms bits.
//
//
// P.Smith                      21/2/06
// first pass at netburner hardware conversion.
// Bool -> BOOL
// included  <basictypes.h>
// changed unions CharsAndInt to CharsAndWord
//
// P.Smith                      16/11/06
// Remove --todoh-- comments
//
// P.Smith                      27/8/07
// Implement the new alarm format leaving the alarms so that they can be read
// easily via the modbus table.
//
// M.McKiernan                      17/9/07
// Removed g_arrnMBTable[] & g_arrnMBTableFlags[], now in BatVars2
//
// P.Smith                      16/1/08
// add HARDWARELICENSEALARM
//
// P.Smith                      28/1/08
// added PERCENTAGEDEVIATIONALARM
//
// P.Smith                      28/1/08
// added check for individual deviation component alarm  in m_nComponentDeviationAlarmWord
// this is done the same way as the fill alarm.
// implement in both set and RemoveAlarmTable & PutAlarmTable
// check nComponent for non zero in RemoveAlarmTable
// set PERALARMBITPOS in system alarm word if PERCENTAGEDEVIATIONALARM
//
// P.Smith                          30/1/08
// correct compiler warnings
//
// P.Smith                          12/6/08
// Ensure that alarm output is switched on for process and system alarms
// it was originally only switched on for process alarms
//
// P.Smith                          17/7/08
// generate event logs from  occurred and cleared alarms
//
// P.Smith                          24/6/08
// remove g_arrnMBTable ,g_CurrentTime
//
// P.Smith                          26/6/08
// modify alarms put / remove to check the occurred history for new alarms.
// if they are the original alarms, < 300 then leave the alarms as they are.
// if the alarm is one of the new set of alarms, then acknowledge alarm if this is
// the first time that the alarm has occurred.
// also when clearing the alarms, clear the alarm occurred history.
// a 2 dimensional array is used to hold the occurred status for the alarm and the
// component number.
//
// P.Smith                          30/9/08
// make alarm words all ints, was originally chars
//
// P.Smith                          24/10/08
// check new alarm word to determine if the alarm beacon should be switched on.
//
// P.Smith                          21/1/09
// remove AddAlarmOccurredToEventLog and AddAlarmClearedToEventLog from here
// the alarms can be generated on the interrupt so it is essential that any events
// that are written to the event log are done from the foreground.
// g_bAddAlarmClearedToEventLog & g_bAddAlarmOccurredToEventLog are set
// store alarm type and component no so that the event can be logged in the foreground.
//
// P.Smith                          3/6/09
// modify put and remove alarms to work with the new alarm format.
// this means that the alarm function itself decides if it should acknowledge the
// alarm itself.
// check is added to check if alarm should be acknowledged.
// remove check for non zero percentage set point for lls alarms, replace with
// check for bAcknowledgeAlarm
// remove bRemove check, this is not necessary anymore.
//
// P.Smith                          10/6/09
// set g_bLowLevelSensorStatusHistory to COVERED to allow lls alarm to come back in again
//
// P.Smith                          26/8/09
// modify put and remove alarms to add event if the pit is not running.
// this allow multiple alarms to be acknowledged
//
// P.Smith                          1/9/09
// AddAlarmClearedToEventLog is called if pit is not running
//
// P.Smith                          17/9/09
// add in vacuum loader alarms.

// M.McKiernan						21/5/2020
// added //wfh in PutAlarmTable BOOL 	bValidWidthAlarm = FALSE;
// check if alarm is a valid alarm for Width controller.  Exit if not a valid width alarm.
//     if(ucAlarmCode == WIDTH_SSIF_COMMS_ALARM )
//    	bValidWidthAlarm = TRUE;
//    else if( (ucAlarmCode == US_SENSOR_1_ALARM) || (ucAlarmCode == US_SENSOR_2_ALARM) || (ucAlarmCode == US_SENSOR_3_ALARM) || (ucAlarmCode == US_SENSOR_4_ALARM) )
//    	bValidWidthAlarm = TRUE;
//    else if( ucAlarmCode == WIDTH_CONTROL_ALARM)
//    	bValidWidthAlarm = TRUE;
//    else if( (ucAlarmCode == CALIBRATIONWRITEALARM) || ( ucAlarmCode == PROCESSDATAWRITEALARM)|| (ucAlarmCode == INVALIDCONFIGDATA) )
//    	bValidWidthAlarm = TRUE;
//    else if( (ucAlarmCode == I2CSTOPPEDALARM) || ( ucAlarmCode == CALINVALARM) )
//    	bValidWidthAlarm = TRUE;
//    else if( (ucAlarmCode = HARDWARELICENSEALARM)|| (ucAlarmCode = LICENSEALARM) )
//    	bValidWidthAlarm = TRUE;
//    else if( ucAlarmCode == SDCARDFAULTALARM)
//    	bValidWidthAlarm = TRUE;

//*****************************************************************************
// INCLUDES
//*****************************************************************************
#include <stdlib.h>
#include <stdio.h>

#include "SerialStuff.h"
#include "BatchMBIndices.h"
#include "BatchCalibrationFunctions.h"
#include "General.h"
#include "ConversionFactors.h"

#include "16R40C.h"
#include "Foreground.h"
#include "ConfigurationFunctions.h"
#include "MBMHand.h"
#include "MBSHand.h"
#include "MBProgs.h"

#include "MBProgs.h"
#include "SetpointFormat.h"
#include "TimeDate.h"
#include "BatVars.h"
#include "MBTogSta.h"
#include "PITWeigh.h"
#include "InitialiseHardware.h"
#include "Alarms.h"
#include "BlRep.h"
#include "Batvars2.h"
#include <basictypes.h>
#include <string.h>
#include "NBBGpio.h"
#include "Eventlog.h"

extern  CalDataStruct   g_CalibrationData;
extern  structSetpointData  g_CurrentRecipe;
extern  structSetpointData  g_TempRecipe;
extern  structRollData      g_FrontRoll;
extern  structRollData      g_BackRoll;
extern   BYTE g_nVacLoaderAlarmsTotal;

    StructAlarmTable    g_MainAlarmTable;

//////////////////////////////////////////////////////
// InitialiseAlarmTable         asm = INALTB
//
// Initialises the alarm table etc.
// Clears table completely and points to first entry position.
//
// M.McKiernan                          08-12-2004
// First Pass
//
//////////////////////////////////////////////////////
void InitialiseAlarmTable( void )
{
    int i,j,nMBCompStatusIndex;
    unsigned int nCompAlarmMask;
    nMBCompStatusIndex = 0;
    // Zero everthing to start
    memset( &g_MainAlarmTable, 0, sizeof( StructAlarmTable ) );

// clear alarm bits in modbus.
    g_arrnMBTable[BATCH_SUMMARY_PROCESS_ALARMS] = 0;        // clear alarm word in modbus - process alarms word
    g_arrnMBTable[BATCH_SUMMARY_SYSTEM_ALARMS] = 0;     // clear sys alarm word also.

    nCompAlarmMask = FILLALARMBIT | LOWLEVALBIT;

    nCompAlarmMask ^= 0xFFFF;       // get 1's complement for anding purposes.

    // Clear alarm bits in status word for each component.
    for( i=1; i <= MAX_COMPONENTS; i++)
    {
        // get address of component's status word.
        if( i <= 10 )   // first 10 blocks are contiguous
            nMBCompStatusIndex =    BATCH_SUMMARY_BLEND_STATUS_1 + ((i - 1) * PSUM_BLOCK_SIZE);
        // status index = start of block plus offset (of status word) in the block.
        else if( i == 11)
            nMBCompStatusIndex = PSUM_BLOCK_11 + (BATCH_SUMMARY_BLEND_STATUS_1 - PSUM_BLOCK_1);
        else if( i == 12)
            nMBCompStatusIndex = PSUM_BLOCK_12 + (BATCH_SUMMARY_BLEND_STATUS_1 - PSUM_BLOCK_1);

        g_arrnMBTable[nMBCompStatusIndex] &= nCompAlarmMask;        // set the alarm bit in modbus, comp status word
    }

    for(i=0; i < MAXIMUM_ALARMS; i++)
    {
        for(j=0; j < MAX_COMPONENTS; j++)
        {
            g_bAlarmOccurredHistory[i][j] = FALSE;
        }
    }
    for(i= 0; i < MAX_COMPONENTS; i++) // make level sensor alarm come in again
    {
        g_bLowLevelSensorStatusHistory[i] = COVERED;
    }

    CopyAlarmTableToMB();   // update alarm table in MB.
}
/*
    int     m_nComponent;
    unsigned char       m_ucAlarmCode;                  //
    char        m_cMonth;
    char        m_cDate;
    char        m_cHour;
    char        m_cMinute;
*/

//////////////////////////////////////////////////////
// PutAlarmTable        asm = PTALTB (note alarm table structure is different though).
//
// Put alarm code into alarm table etc. Note the alarm goes in at the top of the table, i.e.
// previous alarms are shifted down, most recent alarm is at top.
//
// Each alarm entry has following structure:
//                                          int     m_nComponent;
//                                          int     m_ucAlarmCode;                  //
//                                          char        m_cMonth;
//                                          char        m_cDate;
//                                          char        m_cHour;
//                                          char        m_cMinute;
//                                          int     m_nFrontRollNo;
//                                          int     m_nBackRollNo;
//                                          int     m_nFrontRollLengthAccumulator;  // --review-- , may need a long or a float.
//                                          int     m_nBackRollLengthAccumulator;       // --review-- , may need a long or a float.

//
// M.McKiernan                          08-12-2004
// First Pass
//
//////////////////////////////////////////////////////
void PutAlarmTable( unsigned int ucAlarmCode,  int nComponent )
{
    unsigned int nCompMask;
    int nCompIndex = nComponent - 1;
    BOOL    bAlarmIsNew = FALSE;                // flag to check if alarm had previously existed.
    BOOL    bAcknowledgeAlarm;
//wfh
    BOOL 	bValidWidthAlarm = FALSE;
// check if alarm is a valid alarm for Width controller.
    if(ucAlarmCode == WIDTH_SSIF_COMMS_ALARM )
    	bValidWidthAlarm = TRUE;
    else if( (ucAlarmCode == US_SENSOR_1_ALARM) || (ucAlarmCode == US_SENSOR_2_ALARM) || (ucAlarmCode == US_SENSOR_3_ALARM) || (ucAlarmCode == US_SENSOR_4_ALARM) )
    	bValidWidthAlarm = TRUE;
    else if( ucAlarmCode == WIDTH_CONTROL_ALARM)
    	bValidWidthAlarm = TRUE;
    else if( (ucAlarmCode == CALIBRATIONWRITEALARM) || ( ucAlarmCode == PROCESSDATAWRITEALARM)|| (ucAlarmCode == INVALIDCONFIGDATA) )
    	bValidWidthAlarm = TRUE;
    else if( (ucAlarmCode == I2CSTOPPEDALARM) || ( ucAlarmCode == CALINVALARM) )
    	bValidWidthAlarm = TRUE;
    else if( (ucAlarmCode == HARDWARELICENSEALARM)|| (ucAlarmCode == LICENSEALARM) )
    	bValidWidthAlarm = TRUE;
    else if( ucAlarmCode == SDCARDFAULTALARM)
    	bValidWidthAlarm = TRUE;

// Add other valid width alarms
    if(!bValidWidthAlarm)		//exit if not a valid width alarm.
    	return;

    if(!g_bAlarmOccurredHistory[ucAlarmCode][nComponent])  // if cleared, acknowledge alarm
    {
        g_bAlarmOccurredHistory[ucAlarmCode][nComponent] = TRUE;
        bAcknowledgeAlarm = TRUE;
    }
    else
    {
        bAcknowledgeAlarm = FALSE;
    }

// RECORDALARM. -- expanded inline below where alarm is added to array and history. See XXX

    if(bAcknowledgeAlarm)    // dont put alarm if comp. % is zero
    {
        g_bPrintAlarmReport = TRUE;

// SNETALM
        SetModbusAlarmBits( ucAlarmCode,  nComponent ); // set relevant bits in modbus table.

        g_MainAlarmTable.m_bAlarmChangeFlag = TRUE;     // flag to show new alarm, or change in alarms. --ALRMCFG--

        if(nComponent != 0)         // if component no. non-zero, then component alarm
        {
            nCompMask = 0x0001;
            if(nComponent > 1)
            {
                nCompMask <<= nCompIndex;       // shift bit to component position (e.g. component #2, bit b1 set)
            }

            g_MainAlarmTable.m_nAlarmWord |= nCompMask;     // Or into alarm word (asm equiv = ALMBYT (2bytess))

            // Check if it is a low level alarm.
            if( ucAlarmCode  == LOWLEVALARM )
            {
                // was this comp. already in fill alarm??
                if( (g_MainAlarmTable.m_nComponentLLSAlarmWord & nCompMask) == 0 )
                {   // if not set already, set the bit
                    g_MainAlarmTable.m_nComponentLLSAlarmWord |= nCompMask;     // Or into LLS alarm word (asm equiv = LOWLEVELALST)
                    bAlarmIsNew = TRUE;     // indicate that alarm is new.
                }
            }
            else if( ucAlarmCode  == NOFILLALARM )      // Check if it is a fill alarm.
            {
                // was this comp. already in fill alarm??
                if( (g_MainAlarmTable.m_nComponentFillAlarmWord & nCompMask) == 0 )
                {   // // if not set already, set the bit
                    g_MainAlarmTable.m_nComponentFillAlarmWord |= nCompMask;        // Or into FILL alarm word (asm equiv = none)
                    bAlarmIsNew = TRUE;     // indicate that alarm is new.
                }
            }

            else if( ucAlarmCode  == PERCENTAGEDEVIATIONALARM )      // Check if it is a deviation alarm
            {
                // was this comp. already in fill alarm??
                if( (g_MainAlarmTable.m_nComponentDeviationAlarmWord & nCompMask) == 0 )
                {   // // if not set already, set the bit
                    g_MainAlarmTable.m_nComponentDeviationAlarmWord |= nCompMask;        // Or into FILL alarm word (asm equiv = none)
                    bAlarmIsNew = TRUE;     // indicate that alarm is new.
                }
            }


        }  // end of component alarm section.

    // store alarm into current and historic arrays - TRUE => alarm is being set.

    if(bAcknowledgeAlarm)
    {
        StoreAlarmInArrays( ucAlarmCode,  nComponent, TRUE );
        g_ucAlarmCode = ucAlarmCode;
        g_nAlarmComponent = nComponent;
        if(g_bPitRunning)
        {
            g_bAddAlarmOccurredToEventLog = TRUE;
        }
        else
        {
            AddAlarmOccurredToEventLog(ucAlarmCode,nComponent);             // log alarm
        }

    }

// --REVIEW-- ORINMBAL - assembler subroutine doesn't do anything logical.
    }

}

//////////////////////////////////////////////////////
// StoreAlarmInArrays       asm = ADDALARMTOLIST - loosely based.
//
// Put alarm code into alarm arrays.    Note the alarm goes in at the top of the table, i.e.
// previous alarms are shifted down, most recent alarm is at top.
//
//
// M.McKiernan                          10-12-2004
// First Pass
//
//////////////////////////////////////////////////////
void StoreAlarmInArrays( unsigned int ucAlarmCode,  int nComponent, BOOL bState )  // store alarm into current and historic arrays.
{
int i;
// store alarm into Alarm Array - in first position
      for( i = MAX_ALARMS-1; i > 0; i-- )
      { // shift each alarm entry down by 1.
        // dest, src, size  (e.g. 1 alarm, copy from array[0] to array [1].
        memcpy( &g_MainAlarmTable.m_AlarmArray[i], &g_MainAlarmTable.m_AlarmArray[i-1], sizeof( StructAlarmEntry ) );
      }

// alarm is added at top of table.
// the set/clear flag - indicates if alarm is being set or cleared.
            if( bState )
                g_MainAlarmTable.m_AlarmArray[0].m_bAlarmState = TRUE;  // setup the SET or Clear flag
            else
                g_MainAlarmTable.m_AlarmArray[0].m_bAlarmState = FALSE; // Clear the flag

            g_MainAlarmTable.m_AlarmArray[0].m_nComponent = nComponent; // store comp. no. (can be 0)
            g_MainAlarmTable.m_AlarmArray[0].m_ucAlarmCode = ucAlarmCode;   // store alarm type.

// store time.
            g_MainAlarmTable.m_AlarmArray[0].m_cMinute = g_CurrentTime[TIME_MINUTE];        // MINUTES VALUE
            g_MainAlarmTable.m_AlarmArray[0].m_cHour = g_CurrentTime[TIME_HOUR];            //
            g_MainAlarmTable.m_AlarmArray[0].m_cDate = g_CurrentTime[TIME_DATE];            //
            g_MainAlarmTable.m_AlarmArray[0].m_cMonth = g_CurrentTime[TIME_MONTH];          //

// store roll data   (roll variables are now defined 9-08-2005)
            g_MainAlarmTable.m_AlarmArray[0].m_nFrontRollNo = g_FrontRoll.m_nRollNo;   // store front roll no.
            g_MainAlarmTable.m_AlarmArray[0].m_nBackRollNo = g_BackRoll.m_nRollNo;    // store back roll no.
            g_MainAlarmTable.m_AlarmArray[0].m_nFrontRollLengthAccumulator = (int)g_FrontRoll.m_fLength;   // store front roll length
            g_MainAlarmTable.m_AlarmArray[0].m_nBackRollLengthAccumulator = (int)g_BackRoll.m_fLength;    // store back roll length  -

// store Order No and operator no
            g_MainAlarmTable.m_AlarmArray[0].m_nOrderNo = g_CurrentRecipe.m_nOrder_No;  // Order No
            g_MainAlarmTable.m_AlarmArray[0].m_nOperatorNo = g_CurrentRecipe.m_nOperator_No;   // Operator No

// Inc no. of alarms
            if( g_MainAlarmTable.m_nNoOfArrayAlarms < MAX_ALARMS )
                g_MainAlarmTable.m_nNoOfArrayAlarms++;      // inc no. of alarms in array table.


// Repeat for Alarm History table.

// store alarm into Alarm Array - in first position
      for( i = MAX_ALARMS-1; i > 0; i-- )
      { // shift each alarm entry down by 1.
        // dest, src, size  (e.g. 1 alarm, copy from array[0] to array [1].
        memcpy( &g_MainAlarmTable.m_AlarmHistory[i], &g_MainAlarmTable.m_AlarmHistory[i-1], sizeof( StructAlarmEntry ) );
      }

// alarm is added at top of table.

// the set/clear flag - indicates if alarm is being set or cleared.
            if( bState )
                g_MainAlarmTable.m_AlarmHistory[0].m_bAlarmState = TRUE;    // setup the SET or Clear flag
            else
                g_MainAlarmTable.m_AlarmHistory[0].m_bAlarmState = FALSE;   // Clear flag


            g_MainAlarmTable.m_AlarmHistory[0].m_nComponent = nComponent;   // store comp. no. (can be 0)
            g_MainAlarmTable.m_AlarmHistory[0].m_ucAlarmCode = ucAlarmCode; // store alarm type.
// store time.
            g_MainAlarmTable.m_AlarmHistory[0].m_cMinute = g_CurrentTime[TIME_MINUTE];          // MINUTES VALUE
            g_MainAlarmTable.m_AlarmHistory[0].m_cHour = g_CurrentTime[TIME_HOUR];          //
            g_MainAlarmTable.m_AlarmHistory[0].m_cDate = g_CurrentTime[TIME_DATE];          //
            g_MainAlarmTable.m_AlarmHistory[0].m_cMonth = g_CurrentTime[TIME_MONTH];            //

// store roll data   (roll variables now defined)
            g_MainAlarmTable.m_AlarmHistory[0].m_nFrontRollNo = g_FrontRoll.m_nRollNo; // store front roll no.
            g_MainAlarmTable.m_AlarmHistory[0].m_nBackRollNo = g_BackRoll.m_nRollNo;  // store back roll no.


            g_MainAlarmTable.m_AlarmHistory[0].m_nFrontRollLengthAccumulator = (int)g_FrontRoll.m_fLength; // store front roll length
            g_MainAlarmTable.m_AlarmHistory[0].m_nBackRollLengthAccumulator = (int)g_BackRoll.m_fLength;  // store back roll length

// store Order No and operator no
            g_MainAlarmTable.m_AlarmHistory[0].m_nOrderNo = g_CurrentRecipe.m_nOrder_No;        // Order No
            g_MainAlarmTable.m_AlarmHistory[0].m_nOperatorNo = g_CurrentRecipe.m_nOperator_No; // Operator No


// Inc no. of alarms
            if( g_MainAlarmTable.m_nNoOfHistoryAlarms < MAX_ALARMS )
                g_MainAlarmTable.m_nNoOfHistoryAlarms++;    // inc no. of alarms in table

}



//////////////////////////////////////////////////////
// GetAlarmBitMask          asm = CHKCODE
//
// Gets alarm bit (or mask) corresponding to alarm type.
// EXIT: Returns the required bit mask.
//
// M.McKiernan                          10-12-2004
// First Pass
//
//////////////////////////////////////////////////////
unsigned int GetAlarmBitMask( unsigned int ucAlarmCode )
{
unsigned int nBitMask = 0;

    g_bIsSystemAlarm = FALSE;

        switch( ucAlarmCode )
    {
        case LSMONITORALARM:    //
                nBitMask = LSMONITORALARMBIT;
                break;
        case CALINVALARM:           //
                nBitMask = CALINVALARMBIT;
                break;
        case ATTARWTALARM:      //
                nBitMask = ATTARWTALARMBIT;
                break;
        case SEICOMMSALARM:     //
                nBitMask = SEICOMMSALARMBIT;
                break;

        case LEVELSENSORALARM:      //
                nBitMask = LEVELSENALBIT;
                break;
        case CONTROLALARM:      //
                nBitMask = CONTROLALARMBIT;
                break;
        case SEIRESETALARM:     //
                nBitMask = STALLALARMBIT;
                break;
        case LEAKALARM:     // Weight change alarm (i.e. leak from weigh hopper)
                nBitMask = WTCHANGEALBIT;
                break;

        case TAREALARM:     //
                nBitMask = TAREALBIT;
                break;
        case PERALARM:      // Percentage (%) alarm.
                nBitMask = PERALBIT;
                break;
        case ZEROLSALARM:       // ZERO LINE SPEED ALARM
                nBitMask = ZEROLSBIT;
                break;
        case MIXSTOPALARM:          //MIXING SCREW ALARM.
                nBitMask = MIXSTOPALARMBIT;
                break;

        case SHUTDOWNALARM:         //SHUT DOWN EXTRUDER ALARM.
                nBitMask = SHUTDOWNALARMBIT;
                break;

//Component alarms.
        case NOFILLALARM:           //FILL ALARM
                nBitMask = FILLALARMBIT;
                break;
        case LOWLEVALARM:       //
                nBitMask = LOWLEVALBIT;         //LOW material LEVELs (SENSOR in RESERVOIR OR BIN) ALARM.
                break;
        case LICENSEALARM:       //
                nBitMask = LICENSEALARMBITPOS;
                g_bIsSystemAlarm = TRUE;
                break;
        case HARDWARELICENSEALARM:       //
                nBitMask = HARDWARELICENSEALARMBITPOS;
                g_bIsSystemAlarm = TRUE;
                break;

        case PERCENTAGEDEVIATIONALARM:       //
                nBitMask = PERALARMBITPOS;
                g_bIsSystemAlarm = TRUE;
                break;

        default:                    //
                break;
    }
    return( nBitMask );

}

//////////////////////////////////////////////////////
// SetModbusAlarmBits       asm = SNETALM ( includes SETFILLCMPAL & SETLLSCMPAL - expanded inline)
//
// Sets the alarm bits in the modbus table.
// EXIT:
//
// M.McKiernan                          10-12-2004
// First Pass
//
//////////////////////////////////////////////////////
void SetModbusAlarmBits( unsigned int ucAlarmCode,  int nComponent )
{
int nCompAlarmMask = 0;
int nBitMask;
int nMBCompStatusIndex;

        // get address of component's status word.
        nMBCompStatusIndex =    BATCH_SUMMARY_BLEND_STATUS_1 + ((nComponent - 1) * PSUM_BLOCK_SIZE);

    if( ucAlarmCode == NOFILLALARM || ucAlarmCode == LOWLEVALARM)   // component alarm types
    {
        if( ucAlarmCode == NOFILLALARM )
        {
            nCompAlarmMask = FILLALARMBIT;
            g_arrnMBTable[nMBCompStatusIndex] |= FILLALARMBIT;
        }
        else if( ucAlarmCode == LOWLEVALARM )
        {
            nCompAlarmMask = LOWLEVALBIT;
            g_arrnMBTable[nMBCompStatusIndex] |= LOWLEVALBIT;
        }


        // Prod. summary blocks for components 11 and 12 are not contiguous with others, hence...

        // status index = start of block plus offset (of status word) in the block.
        if( nComponent == 11)
            nMBCompStatusIndex = PSUM_BLOCK_11 + (BATCH_SUMMARY_BLEND_STATUS_1 - PSUM_BLOCK_1);
        else if( nComponent == 12)
            nMBCompStatusIndex = PSUM_BLOCK_12 + (BATCH_SUMMARY_BLEND_STATUS_1 - PSUM_BLOCK_1);

        g_arrnMBTable[nMBCompStatusIndex] |= nCompAlarmMask;        // set the alarm bit in modbus, comp status word

    }
        // all alarms
        nBitMask =  GetAlarmBitMask( ucAlarmCode ); // get bit to set for alarm code or type.

        if(g_bIsSystemAlarm)
        {
            if(nBitMask)    // check for non-zero
                g_arrnMBTable[BATCH_SUMMARY_SYSTEM_ALARMS] |= nBitMask;        // set the alarm bit in modbus - process alarms word

        }
        else
        {
            if(nBitMask)    // check for non-zero
                g_arrnMBTable[BATCH_SUMMARY_PROCESS_ALARMS] |= nBitMask;        // set the alarm bit in modbus - process alarms word
        }

}

//////////////////////////////////////////////////////
// ClearModbusAlarmBits         asm = CNETALM ( includes CLRFILLCMPAL & CLRLLSCMPAL - expanded inline)
//
// Clears the alarm bits in the modbus table.
// EXIT:
//
// M.McKiernan                          10-12-2004
// First Pass
//
//////////////////////////////////////////////////////
void ClearModbusAlarmBits( unsigned int ucAlarmCode,  int nComponent )
{
int nCompAlarmMask = 0;
int nBitMask;
int nMBCompStatusIndex;

    if( ucAlarmCode == NOFILLALARM || ucAlarmCode == LOWLEVALARM)   // component alarm types
    {
        if( ucAlarmCode == NOFILLALARM )
        {
            nCompAlarmMask = FILLALARMBIT;
        }
        else if( ucAlarmCode == LOWLEVALARM )
        {
            nCompAlarmMask = LOWLEVALBIT;
        }

        nCompAlarmMask ^= 0xFFFF;       // get 1's complement for anding purposes.

        // get address of component's status word.
        nMBCompStatusIndex =    BATCH_SUMMARY_BLEND_STATUS_1 + ((nComponent - 1) * PSUM_BLOCK_SIZE);

        // Prod. summary blocks for components 11 and 12 are not contiguous with others, hence...

        // status index = start of block plus offset (of status word) in the block.
        if( nComponent == 11)
            nMBCompStatusIndex = PSUM_BLOCK_11 + (BATCH_SUMMARY_BLEND_STATUS_1 - PSUM_BLOCK_1);
        else if( nComponent == 12)
            nMBCompStatusIndex = PSUM_BLOCK_12 + (BATCH_SUMMARY_BLEND_STATUS_1 - PSUM_BLOCK_1);

        g_arrnMBTable[nMBCompStatusIndex] &= nCompAlarmMask;        // set the alarm bit in modbus, comp status word

    }
    // all alarms
        nBitMask =  GetAlarmBitMask( ucAlarmCode ); // get bit to clear for alarm code or type.

        if(g_bIsSystemAlarm)
        {
            if(nBitMask)    // check for non-zero
            {
                nBitMask ^= 0xFFFF; // get 1's complement for AND'ing.
                g_arrnMBTable[BATCH_SUMMARY_SYSTEM_ALARMS] &= nBitMask;        // set the alarm bit in modbus - process alarms word
            }


        }
        else
        {
            if(nBitMask)    // check for non-zero
            {
                nBitMask ^= 0xFFFF; // get 1's complement for AND'ing.
                g_arrnMBTable[BATCH_SUMMARY_PROCESS_ALARMS] &= nBitMask;        // set the alarm bit in modbus - process alarms word
            }
        }


}

//////////////////////////////////////////////////////
// RemoveAlarmTable         asm = RMALTB (note alarm table structure is different though).
//
// Takes care of when alarm clears.
// Note the alarm is actually stored into array as per PutAlarmTable, with a flag indicating cleared.
// The alarm goes in at the top of the table, i.e.
// Previous alarms are shifted down, most recent alarm is at top.
//
//
// M.McKiernan                          10-12-2004
// First Pass
//
//////////////////////////////////////////////////////
void RemoveAlarmTable( unsigned int ucAlarmCode,  int nComponent )
{
unsigned int nCompMask;             // indicates bit position for component in 16 bit word. e.g. 0x0001 for component #1.
unsigned int nInvertedCompMask; //  inverse or 1's comp. of nCompMask.
int nCompIndex = nComponent - 1;
BOOL bRemove = FALSE;               // flag to indicate if comp. alarm is removed.
BOOL    bAcknowledgeAlarm;

    if(g_bAlarmOccurredHistory[ucAlarmCode][nComponent])  // was the last state occurred.
    {
        g_bAlarmOccurredHistory[ucAlarmCode][nComponent] = FALSE;
        bAcknowledgeAlarm = TRUE;
    }
    else
    {
        bAcknowledgeAlarm = FALSE;
    }

    if(bAcknowledgeAlarm)
    {
    if(nComponent != 0)         // if component no. non-zero, then component alarm
    {
            nCompMask = 0x0001;
            if(nComponent > 1)
            {
                nCompMask <<= nCompIndex;       // shift bit to component position (e.g. component #2, bit b1 set)
            }

            nInvertedCompMask = nCompMask ^ 0xFFFF;     // complement, i.e. get 1's complement, for AND'ing.

            // Check if it is a low level alarm.
            if( ucAlarmCode  == LOWLEVALARM )
            {
                if( g_MainAlarmTable.m_nComponentLLSAlarmWord & nCompMask ) // is the lls alarm bit set for this comp.
                {
                    g_MainAlarmTable.m_nComponentLLSAlarmWord &= nInvertedCompMask;     // And with LLS alarm word (asm equiv = LOWLEVELALST)
                    bRemove = TRUE;
                }
            }
            // Check if it is a fill alarm.
            if( ucAlarmCode  == NOFILLALARM )
            {
                if( g_MainAlarmTable.m_nComponentFillAlarmWord & nCompMask )    // is the fill alarm bit set for this comp.
                {
                    g_MainAlarmTable.m_nComponentFillAlarmWord &= nInvertedCompMask;        // And with FILL alarm word (asm equiv = none)
                    bRemove = TRUE;
                }
            }
       // Check if it is a deviation alarm.
            if( ucAlarmCode  == PERCENTAGEDEVIATIONALARM )
            {
                if( g_MainAlarmTable.m_nComponentDeviationAlarmWord & nCompMask )    // is the fill alarm bit set for this comp.
                {
                    g_MainAlarmTable.m_nComponentDeviationAlarmWord &= nInvertedCompMask;        // and with word to clear bit.
                    bRemove = TRUE;
                }
            }

            // check if fill or lls alarm present. - if not, clear general component alarm bit.
            if((g_MainAlarmTable.m_nComponentLLSAlarmWord & nCompMask) || (g_MainAlarmTable.m_nComponentFillAlarmWord & nCompMask))
                g_MainAlarmTable.m_nAlarmWord |= nCompMask;     // set comp. bit in alarm word (asm equiv = ALMBYT (2bytess))
            else
                g_MainAlarmTable.m_nAlarmWord &= nInvertedCompMask;     // clear comp. bit in alarm word (asm equiv = ALMBYT (2bytess))


            // If comp. alarm removed, store alarm into current and historic arrays. FALSE => alarm is cleared.
            StoreAlarmInArrays( ucAlarmCode,  nComponent, FALSE );
            g_ucAlarmCode = ucAlarmCode;
            g_nAlarmComponent = nComponent;

            if(g_bPitRunning)
            {
                g_bAddAlarmClearedToEventLog = TRUE;
            }
            else
            {
                AddAlarmClearedToEventLog(ucAlarmCode,nComponent);             // log alarm
            }

    }
    else    // non component related alarms
    {
        // store alarm into current and historic arrays. FALSE => alarm is cleared.
        StoreAlarmInArrays( ucAlarmCode,  nComponent, FALSE );
        g_ucAlarmCode = ucAlarmCode;
        g_nAlarmComponent = nComponent;

        if(g_bPitRunning)
        {
            g_bAddAlarmClearedToEventLog = TRUE;
        }
        else
        {
            AddAlarmClearedToEventLog(ucAlarmCode,nComponent);
        }
    }
// CNETALM
        ClearModbusAlarmBits( ucAlarmCode,  nComponent );   // clear relevant bits in modbus table.

        g_MainAlarmTable.m_bAlarmChangeFlag = TRUE;     // flag to show new alarm, or change in alarms. --ALRMCFG--
    }
}

//////////////////////////////////////////////////////
// CheckForAlarm        asm = CHKFORALARM
//
// Checks to see if any alarm is active, and if so, activates alarm output(s).
//
//
// M.McKiernan                          13-12-2004
// First Pass
//
//////////////////////////////////////////////////////
void CheckForAlarm( void )
{
    BOOL    bAlarmFound = FALSE;
    unsigned int nCompAlarmMask,i;
    int nMBCompStatusIndex;
    nMBCompStatusIndex = 0;

    // get component alarm bits (i.e. alarm bits in component status word in modbus)
    nCompAlarmMask = BATCH_HOPPER_FILL_MASK;                // Fill alarm bit.
    nCompAlarmMask |= BATCH_COMPONENT_LOW_LEVEL_MASK;   // or in material low level alarm bit

    if( (g_arrnMBTable[BATCH_SUMMARY_PROCESS_ALARMS] != 0) || (g_arrnMBTable[BATCH_SUMMARY_SYSTEM_ALARMS] != 0)|| (g_arrnMBTable[BATCH_SUMMARY_PROCESS_ANY_ALARM_OCCURRED]!= 0))
    {
        bAlarmFound = TRUE;
    }
    else
    {
        for(i = 1; i <= g_CalibrationData.m_nComponents && bAlarmFound == FALSE; i++ )
        {
            // get address of component's status word.
            if(i <= 10)     // prod. summ blocks 1-10 are contiguous, easy to get address.
                nMBCompStatusIndex =    BATCH_SUMMARY_BLEND_STATUS_1 + ((i - 1) * PSUM_BLOCK_SIZE);
            // Prod. summary blocks for components 11 and 12 are not contiguous with others, hence...
            // status index = start of block plus offset (of status word) in the block.
            else if( i == 11)
                nMBCompStatusIndex = PSUM_BLOCK_11 + (BATCH_SUMMARY_BLEND_STATUS_1 - PSUM_BLOCK_1);
            else if( i == 12)
                nMBCompStatusIndex = PSUM_BLOCK_12 + (BATCH_SUMMARY_BLEND_STATUS_1 - PSUM_BLOCK_1);

            if(g_arrnMBTable[nMBCompStatusIndex] & nCompAlarmMask)      // check if comp in alarm
            {
                bAlarmFound = TRUE;
            }
        }
    }
    if(bAlarmFound)
    {
        AlarmOutputsOn();       // turn on alarm relay and alarm beacon
    }
    else
    {
        AlarmOutputsOff();      // turn off alarm relay and alarm beacon
    }
    if(g_nVacLoaderAlarmsTotal != 0)
    {
        PutAlarmTable( VACUUMLOADERALARM,  0 );
    }
    else
    {
        RemoveAlarmTable( VACUUMLOADERALARM,  0 );
    }
}

//////////////////////////////////////////////////////
// CopyAlarmTableToMB       asm = CPYMBALARMS
//
// Copies alarm table (or array) into modbus.
//
//
// M.McKiernan                          13-12-2004
// First Pass
//////////////////////////////////////////////////////
void CopyAlarmTableToMB( void )
{
int i;

    for( i=0; i<MB_ALARM_BLOCKS; i++ )      //copy all blocks.
    {
        g_arrnMBTable[ALARM_BLOCK_1  + ( ALARM_BLOCK_SIZE * i ) + MB_ALARM_BLOCK_TYPE] = g_MainAlarmTable.m_AlarmArray[i].m_ucAlarmCode;
        // hours:
        g_arrnMBTable[ALARM_BLOCK_1  + ( ALARM_BLOCK_SIZE * i ) + MB_ALARM_BLOCK_HOURS] =    g_MainAlarmTable.m_AlarmArray[i].m_cHour;
        // minutes:
        g_arrnMBTable[ALARM_BLOCK_1  + ( ALARM_BLOCK_SIZE * i ) + MB_ALARM_BLOCK_MINUTES] =  g_MainAlarmTable.m_AlarmArray[i].m_cMinute;
        // month
        g_arrnMBTable[ALARM_BLOCK_1  + ( ALARM_BLOCK_SIZE * i ) + MB_ALARM_BLOCK_MONTH] = g_MainAlarmTable.m_AlarmArray[i].m_cMonth;
        // Date
        g_arrnMBTable[ALARM_BLOCK_1  + ( ALARM_BLOCK_SIZE * i ) + MB_ALARM_BLOCK_DATE] = g_MainAlarmTable.m_AlarmArray[i].m_cDate;

        // occurred / cleared
        if(g_MainAlarmTable.m_AlarmArray[i].m_bAlarmState)
        {
            g_arrnMBTable[ALARM_BLOCK_1  + ( ALARM_BLOCK_SIZE * i ) + MB_ALARM_BLOCK_OCCURRED] = 1;
        }
        else
        {
            g_arrnMBTable[ALARM_BLOCK_1  + ( ALARM_BLOCK_SIZE * i ) + MB_ALARM_BLOCK_OCCURRED] = 0;
        }

        // Component No.
        g_arrnMBTable[ALARM_BLOCK_1  + ( ALARM_BLOCK_SIZE * i ) + MB_ALARM_BLOCK_COMPONENT_NO] = (unsigned char)g_MainAlarmTable.m_AlarmArray[i].m_nComponent;

        // Order No.
        g_arrnMBTable[ALARM_BLOCK_1  + ( ALARM_BLOCK_SIZE * i ) + MB_ALARM_BLOCK_ORDER_NO] = g_MainAlarmTable.m_AlarmArray[i].m_nOrderNo;
        // Operator No.
        g_arrnMBTable[ALARM_BLOCK_1  + ( ALARM_BLOCK_SIZE * i ) + MB_ALARM_BLOCK_OPERATOR_NO] = g_MainAlarmTable.m_AlarmArray[i].m_nOperatorNo;
    }

}
/*


COPYALARMDATA:
        PSHM    Z
        LDX     0,Y                     ; READ MODBUS STORAGE
        LDD     0,Z                     ; READ ALARM TYPE AND COMP NO.
;        JSR     BCDHEX2                 ;
        STD     0,X                     ; STORE
        LDD     2,Z                     ; TIME HOURS.MIN

        LDAB    2,Z                     ; READ HOURS
        JSR     BCDHEX1
        STAB    2,X
        LDAB    3,Z                     ; READ HOURS
        JSR     BCDHEX1
        STAB    3,X

        LDD     4,Z                     ; DATE DAY:MONTH

        LDAB    4,Z                     ; READ HOURS
        JSR     BCDHEX1
        STAB    4,X
        LDAB    5,Z                     ; READ HOURS
        JSR     BCDHEX1
        STAB    5,X


        LDD     6,Z                     ; FRONT ROLL NO
        JSR     BCDHEX2                 ;
        STD     6,X                     ;
        AIZ     #8                      ; FRONT ROLL LENGTH
        PSHM    X
        TZX                             ; Z - X
        JSR     BCDHEX3X
        PULM    X
        STE     8,X
        PULM    Z
        RTS



// offsets into alarm block of data.
#define MB_ALARM_BLOCK_TYPE_COMP            0   // alarm type:component.
#define MB_ALARM_BLOCK_SET_CLR_MASK     0x0080  // alarm state (set cleared) use bit b7.
#define MB_ALARM_BLOCK_HOUR_MIN             1   // alarm Hours:Mins
#define MB_ALARM_BLOCK_MON_DATE             2   // Month:Date
#define MB_ALARM_BLOCK_FRONT_ROLL_NO        3   // Front roll no.
#define MB_ALARM_BLOCK_FRONT_ROLL_LENGTH    4   // Front roll length - length into roll at which alarm occurred
#define MB_ALARM_BLOCK_BACK_ROLL_NO     5   // Back roll no.
#define MB_ALARM_BLOCK_BACK_ROLL_LENGTH 6   // BACK roll length - length into roll at which alarm occurred

#define ALARM_BLOCK_SIZE        7                   // alarm block size in words.

#define  ALARM_BLOCK_1          1644                                // 1ST alarm block
#define  ALARM_BLOCK_2          (ALARM_BLOCK_1  + (ALARM_BLOCK_SIZE * 1 ))  // alarm block #2
#define  ALARM_BLOCK_3          (ALARM_BLOCK_1  + (ALARM_BLOCK_SIZE * 2 ))  // alarm block #3
#define  ALARM_BLOCK_4          (ALARM_BLOCK_1  + (ALARM_BLOCK_SIZE * 3 ))  // alarm block #4
#define  ALARM_BLOCK_5          (ALARM_BLOCK_1  + (ALARM_BLOCK_SIZE * 4 ))  // alarm block #5
#define  ALARM_BLOCK_6          (ALARM_BLOCK_1  + (ALARM_BLOCK_SIZE * 5 ))  // alarm block #6
#define  ALARM_BLOCK_7          (ALARM_BLOCK_1  + (ALARM_BLOCK_SIZE * 6 ))  // alarm block #7
#define  ALARM_BLOCK_8          (ALARM_BLOCK_1  + (ALARM_BLOCK_SIZE * 7 ))  // alarm block #8
#define  ALARM_BLOCK_9          (ALARM_BLOCK_1  + (ALARM_BLOCK_SIZE * 8 ))  // alarm block #9
#define  ALARM_BLOCK_10         (ALARM_BLOCK_1  + (ALARM_BLOCK_SIZE * 9 ))  // alarm block #10

#define  ALARM_BLOCK_11         (ALARM_BLOCK_1  + (ALARM_BLOCK_SIZE * 10 ))     // alarm block #11
#define  ALARM_BLOCK_12         (ALARM_BLOCK_1  + (ALARM_BLOCK_SIZE * 11 ))     // alarm block #12
#define  ALARM_BLOCK_13         (ALARM_BLOCK_1  + (ALARM_BLOCK_SIZE * 12 ))     // alarm block #13
#define  ALARM_BLOCK_14         (ALARM_BLOCK_1  + (ALARM_BLOCK_SIZE * 13 ))     // alarm block #14


*/

/*
#define BATCH_HOPPER_FILL_MASK                  0x0008
#define BATCH_COMPONENT_LOW_LEVEL_MASK      0x0010

// ALARMS DEFINITIONS
#define     NOFILLALARM           0x10      //NOFILLALARM     EQU       0x10      ; FILL ALARM.
#define     ZEROLSALARM         0x20      //ZEROLSALARM     EQU     $20      ; ZERO LINE SPEED ALARM
#define     MIXSTOPALARM        0x30      //MIXSTOPALARM    EQU     $30      ; MIXING SCREW ALARM.
#define     CONTROLALARM        0x40      //CONTROLALARM    EQU     $40      ; CONTROL ALARM
#define     CALINVALARM         0x50      //CALINVALARM     EQU     $50      ; CALIBRATION INVALID.
#define     LSMONITORALARM        0x60      //LSMONITORALARM EQU     $60      ; LEVEL SENSOR MONITOR ALARM
//#define     EEPROMALARM         0x60      //EEPROMALARM     EQU     $60      ; EEPROM ALARM
#define     ATTARWTALARM        0x70      //ATTARWTALARM    EQU     $70     ; AT TARGET WEIGHT ALARM..
#define     SEICOMMSALARM       0x80      //SEICOMMSALARM   EQU     $80      ; SEI ALARM.
#define     SEIRESETALARM       0x90      //SEIRESETALARM   EQU     $90     ; SEI RESET ALARM.
#define     PERALARM            0xA0      //PERALARM        EQU     $A0      ; LOW LS ALARM.
#define     LEVELSENSORALARM    0xB0      //LEVELSENSORALARM EQU    $B0      ; LEVEL SENSOR ALARM
#define     LEAKALARM           0xC0      //LEAKALARM       EQU     $C0      ; HOPPER LEAK ALARM
#define     LOWLEVALARM         0xD0      //LOWLEVALARM     EQU     $D0      ; LOW LEVEL ALARM.
#define     TAREALARM           0xE0      //TAREALARM       EQU     $E0      ; TARE ALARM ADDED.
#define     SHUTDOWNALARM       0xF0      //SHUTDOWNALARM   EQU     $F0      ; SHUT DOWN EXTRUDER ALARM.

//NB bits defined on a 16 bit basis, because they are part of MB Process Alarm Word.
#define     CONTROLALARMBIT     0x8000        //CONTROLALARMBIT EQU     %10000000    EXTRUDER CONTROL
#define     LEVELSENALBIT       0x4000        //LEVELSENALBIT   EQU     %01000000    LEVEL SENSOR ALARM BIT.
#define     TAREALBIT           0x2000        //TAREALBIT       EQU     %00100000    TARE ALARM BIT.
#define     STALLALARMBIT       0x1000        //STALLALARMBIT   EQU     %00010000    STALLED ALARM BIT.
#define     WTCHANGEALBIT       0x0800        //WTCHANGEALBIT   EQU     %00001000    WT CHANGE ALARM BIT
#define     PERALBIT            0x0400        //PERALBIT        EQU     %00000100    % = 100 %.
#define     CALINVALARMBIT      0x0200        //CALINVALARMBIT  EQU     %00000010    CALIBRATION DATA INVALID
#define     LSMONITORALARMBIT   0x0100        //LSMONITORALARMBIT EQU     %00000001  ; LEVEL SENSOR MONITOR ALARM..

#define     MIXSTOPBIT           0x0001        //MIXSTOPBIT      EQU     %00000001       ;
#define     ZEROLSBIT            0x0002        //ZEROLSBIT       EQU     %00000010       ; ZEROLS ALARM.
#define     SHUTDOWNALARMBIT     0x0004        //SHUTDOWNALARMBIT EQU    %00000100       ; SHUTDOWN ALARM BIT
#define     FILLALARMBIT         0x0008        //FILLALARMBIT    EQU     %00001000       ; FILL BIT
#define     LOWLEVALBIT          0x0010        //LOWLEVALBIT     EQU     %00010000       ; LOW LEVEL ALARM BIT.
#define     SEICOMMSALARMBIT     0x0020        //SEICOMMSALARMBIT EQU    %00100000       ; SEI COMMS ALARM BIT
#define     ATTARWTALARMBIT      0x0040        //ATTARWTALARMBIT EQU     %01000000       ; AT TARGET WEIGHT

CHKCODE ANDA    #$F0
        LDAB   #$01              ;~BIT 0 "EEPROM WRITE FAILURE"
       CMPA   #LSMONITORALARM
       BEQ    SNTAL3
       LDAB   #$02               ;~BIT 1  "CALIBRATION DATA ALARM "
       CMPA   #CALINVALARM
       BEQ    SNTAL3
       LDAB     #ATTARWTALARMBIT        ; SET AT TARGET ALARM BIT.
       CMPA     #ATTARWTALARM           ; AT TARGET ALARM.
       BEQ    SNTAL3
       LDAB     #SEICOMMSALARMBIT       ; SEI COMMS ALARM BIT.
       CMPA     #SEICOMMSALARM          ; SEI COMMS ALARM ?
       BEQ    SNTAL3
       LDAB   #LEVELSENALBIT     ;BIT 2 "LEVEL SENSOR UNCOVERED"
       CMPA   #$B0
       BEQ    SNTAL3
       LDAB   #CONTROLALARMBIT  ;~BIT 3  "CONTROL ALARM"
       CMPA   #$40
       BEQ    SNTAL3
       LDAB   #STALLALARMBIT    ;~BIT 6  "EXTRUDER STALLED ALARM "
       CMPA   #SEIRESETALARM
       BEQ    SNTAL3
       LDAB     #WTCHANGEALBIT  ; WEIGHT CHANGE BIT.
       CMPA     #$C0           ; WEIGHT CHANGE ALARM
       BEQ      SNTAL3
       LDAB     #TAREALBIT
       CMPA     #TAREALARM
       BEQ      SNTAL3
       LDAB     #PERALBIT       ; 100 % ALARM
       CMPA     #PERALARM       ;
       BEQ      SNTAL3
SNTAL3 RTS


; SYSTEM ALARMS
SNTAL5  RTS
       LDAB   #$02              ;~BIT 1 "A/D ERROR (OVERANGE/NEG POLARITY)"
       CMPA   #$80
       BEQ    SNTAL7
;
       LDAB    #$80                     ;~BIT 7 (MS BIT) "UNKNOWN ALARM"
SNTAL7 ORAB    SYS_ALM                  ;~SET BIT IN SYSTEM ALARM BYTE
       STAB    SYS_ALM                  ;~AND STORE NEW VALUE
SNTALX PULA                             ;~RESTORE ALARM CODE
       RTS
*/


