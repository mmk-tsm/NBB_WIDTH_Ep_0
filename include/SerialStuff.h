////////////////////////////////////////////////////////////////
// SerialStuff.h
//
// Include file for the serial comms definitions.
//
// M.Parks                  24-01-2000
// First pass.
// M.Parks                  27-07-2000
// Added new message to the queueing options.
// M.McKiernan                  26-03-2001
// AtmValveTime_Overlap
// M.McKiernan                  10-04-2001
// Changed MAX_COMMS_UNITS from 32 to 40.
// M.McKiernan                  25-6-2001
//  BOOL    m_bPollingEnable defined in         - flag for enabling polling
// M.McKiernan                  20-11-2003
//   char m_cGroupNumber defined in structControllerData - variable for layer group number storage.
// M.McKiernan                  02-03-2004
//   int m_nLoaderFillNo defined in structLoaderData -

//
//
// M.McKiernan                  17-01-2005
// Adapting from CP2000 version for batch blender.
// Main changes: - see mmk
//      Changes to the timeout values - based on PIT of 1170Hz and 9600 baud.
//      Added a priority for each message in message Q structure - structMessageQueue
//      Changed pointer for     Modbus read and write tables to int's.  (int        *arrnReadData )
//      Removed the structUniqueData    uniqueData from the controller structure - no use currently identified.
//
//
// P.Smith                      12/7/05
// Added m_bSeiWriteCommandCompleted
//
// P.Smith						11/1/06
// Corrected compiler warnings.
// m_cTxBuffer[COMMS_BUFFER_SIZE] to unsigned char
// m_cRxBuffer[COMMS_BUFFER_SIZE] to unsigned char
//
// P.Smith                      16/1/06
// first pass at netburner hardware conversion.
// comment out #include "StdTypes.h"
// Bool -> BOOL
//
// P.Smith                      23/1/06
// included  <basictypes.h>
// set INTER_MESSAGE_DELAY to 13 was originally set to 5
// set COMMS_TIMEOUT_LIMIT         50          //nbb
// set MODBUS_BYTE_WRITE_TIMEOUT to 150, was 60
// set MODBUS_SEI_READ_TIMEOUT to  150, was 60
// set MODBUS_UFP2_READ_TIMEOUT to 75, was 30
// set MODBUS_LLS_READ_TIMEOUT to 100, was 40
// set MODBUS_STATUS_READ_TIMEOUT to 63, was 25
// set MODBUS_WRITE_TIMEOUT to 1025, was 410
//
// P.Smith                      21/2/06
// PIT_FREQUENCY -> BLENDER_PIT_FREQUENCY #define POLL_TIMER    BLENDER_PIT_FREQUENCY   // 1 Second's worth of PIT ticks
//
//
// P.Smith                      12/6/07
// added EQUIP_OPTIMISATION
// define MODBUS_OPTIMISATION_READ_TIMEOUT
// MAX_COMMS_UNITS set to 5
//
// M.McKiernan                   30.4.2008
// COMMS_INC_DEC_WRITE_COMMAND added.
// Added m_nIncDecSetpointWriteRetries, SEND_CAL_DATA_TO_SEI_NO
//
// M.McKiernan                   06.5.2008
// MAX_COMMS_UNITS set to 5 + MAX_LIW_CARDS
// defined EQUIP_LIW,  MODBUS_LIW_READ_TIMEOUT, FIRST_LIW_ADDRESS
// Added m_cBoardID to controller structure.

//
// M.McKiernan                   09.6.2009
// Added for VAC8 io cards #define EQUIP_VAC8IO & FIRST_VAC8IO_ADDRESS & MODBUS_VAC8IO_READ_TIMEOUT
//
// P.Smith                   15/10/09
// define FIRST_VAC8IO_EXPANSION_ADDRESS as 21
//
// P.Smith                   8/6/10
// defined EQUIP_SSIF
////////////////////////////////////////////////////////////////


//*****************************************************************************
// MACROS
//*****************************************************************************
#ifndef __SERIALSTUFF_H__
#define __SERIALSTUFF_H__

//*****************************************************************************
// INCLUDES
//*****************************************************************************
//#include "StdTypes.h"
#include <basictypes.h>


//
#define MAX_COMMS_UNITS     (5 + MAX_LIW_CARDS + MAX_VAC8IO_CARDS)   // Maximum number of units that can be communicated with(UFP1,UFP2,SEI,LLS,OPT + LIW's + VAC8IO's)

// ***** THIS IS AN ARBITRARY FIGURE JUST TO GET STARTED
// Based on the number of menu keys but the software will work with any number
#define MAX_PUMPS           5

// Priority range for the loaders
#define MAX_PRIORITY        9

/*
// Supported message types for queuing
#define COMMS_FAST_STATUS_READ      0
#define COMMS_READ_SUMMARY          1
#define COMMS_READ_SETPOINT         2
#define COMMS_READ_CALIBRATION      3
#define COMMS_READ_FRONT_REPORT     4
#define COMMS_READ_BACK_REPORT      5
#define COMMS_READ_ORDER_REPORT     6
#define COMMS_WRITE_SETPOINT        7
#define COMMS_WRITE_CALIBRATION     8
#define COMMS_WRITE_SINGLE_WORD     9
#define COMMS_CLEAR_FRONT_FLAG      10
#define COMMS_CLEAR_BACK_FLAG       11
#define COMMS_CLEAR_ORDER_FLAG      12
#define COMMS_SET_TIME              13
#define COMMS_WRITE_COMP_CONFIG     14
#define COMMS_READ_SUMMARY_PART2    15
#define COMMS_WRITE_OPTIONS         16
#define COMMS_READ_DIAGNOSTICS      17

#define COMMS_INVALID_MESSAGE       -1

// Maximum size of the comms message queue
#define MAX_QUEUE_SIZE      (COMMS_READ_DIAGNOSTICS + 1)
*/
// Supported message types for queuing
#define COMMS_READ_SUMMARY          1
#define COMMS_READ_ALL                  2       // Reads complete (contiguous) MB table from peripheral
#define COMMS_READ_CALIBRATION      3
#define COMMS_READ_ID                   4       // read board id - note offset involved.
#define COMMS_WRITE_COMMANDS            5
#define COMMS_WRITE_CALIBRATION     6
#define COMMS_WRITE_SINGLE_WORD     7

#define COMMS_READ_UFP2_SUMMARY         8       // note there will be an offset involved for the UFP2.
#define COMMS_WRITE_UFP2_COMMANDS       9

#define COMMS_WRITE_DISPLAY             10

#define COMMS_INC_DEC_WRITE_COMMAND     11

#define COMMS_INVALID_MESSAGE       -1

#define MAX_QUEUE_SIZE                  8           //(COMMS_READ_DIAGNOSTICS + 1)



// Supported interface types
#define INTERFACE_RS422             0
#define INTERFACE_RS485             1
#define INTERFACE_RS232             2
#define INTERFACE_CURRENT_LOOP      3

#define COMMS_BUFFER_SIZE           275 // actual limit is 259

#define MAX_NUMBER_REGISTERS        125

#define MODBUS_REGISTER_LENGTH      2

#define READ_REQUEST_LENGTH             8
#define READ_RESPONSE_LENGTH            5       // must add the number of requested bytes to this to get the actual length
#define WRITE_RESPONSE_LENGTH           8
#define WRITE_N_WORDS_REQUEST_LENGTH    11      // minimum length for write n words including one data word
#define WRITE_A_WORD_REQUEST_LENGTH     8       // minimum length for write a word
#define ERROR_RESPONSE_LENGTH           5       // number of bytes in an error message
#define FORCE_COIL_REQUEST_LENGTH       8
#define FORCE_COIL_RESPONSE_LENGTH      8
#define FAST_STATUS_REQUEST_LENGTH      4
#define FAST_STATUS_RESPONSE_LENGTH     5

// Number of retries within one queue message
#define MODBUS_READ_RETRIES             3
#define MODBUS_WRITE_RETRIES            3
#define MODBUS_STATUS_READ_RETRIES      1

// Total number of times a write message is added
// back onto the queue in the event of a failure
#define MODBUS_WRITE_QUEUE_RETRIES      7

/*
// Timeout values
// based on a pit frequency of 500 Hz and
// a baud rate of 9600.
#define MODBUS_BLENDER_READ_TIMEOUT     160     // 263 bytes plus 20mS turnaround (320mS timeout)
#define MODBUS_LOADER_READ_TIMEOUT      30  //15        // 17 bytes plus 2mS turnaround (30mS timeout)
//mmk #define   MODBUS_LOADER_READ_TIMEOUT      30      // 17 bytes plus 2mS turnaround (30mS timeout)
#define MODBUS_STATUS_READ_TIMEOUT      20 //10     // 9 bytes plus 2mS turnaround (20mS timeout)
#define MODBUS_WRITE_TIMEOUT            170     // 267 bytes plus 20mS turnaround (340mS timeout)
#define MODBUS_BYTE_WRITE_TIMEOUT       25      // 16 bytes plus 4mS turnaround (50mS timeout)
*/

// Timeout values ased on a pit frequency of 1170Hz  and ( mmk PIT period = 0.8547mS, char time = 1.15mS)
// a baud rate of 9600.
//#define   MODBUS_BLENDER_READ_TIMEOUT     160     // 263 bytes plus 20mS turnaround (320mS timeout)
//#define   MODBUS_LOADER_READ_TIMEOUT      30  //15        // 17 bytes plus 2mS turnaround (30mS timeout)
//mmk #define   MODBUS_LOADER_READ_TIMEOUT      30      // 17 bytes plus 2mS turnaround (30mS timeout)
//nbb #define MODBUS_SEI_READ_TIMEOUT         60  //DEBUGGING -was 50.        // 20 bytes plus 20mS turnaround (~40mS timeout)
#define MODBUS_SEI_READ_TIMEOUT         150  //nbb                           // 20 bytes plus 20mS turnaround (~40mS timeout)

#//nbb define MODBUS_UFP2_READ_TIMEOUT        30      // 6 bytes plus 20mS turnaround (~26mS timeout)
#define MODBUS_UFP2_READ_TIMEOUT        75      //nbb allow for new pit frequency
//nbb #define MODBUS_LLS_READ_TIMEOUT         40      //DEBUGGING -was 30. 2 bytes plus 20mS turnaround (~22mS timeout)
#define MODBUS_LLS_READ_TIMEOUT         100     //nbb allow for new pit frequency

#define MODBUS_OPTIMISATION_READ_TIMEOUT         100     //optimisation
#define MODBUS_LIW_READ_TIMEOUT           (150)

#define MODBUS_VAC8IO_READ_TIMEOUT           (100)


//nbb #define MODBUS_STATUS_READ_TIMEOUT      25  //10        // 9 bytes plus 5mS turnaround (15mS timeout)
#define MODBUS_STATUS_READ_TIMEOUT      63  //nbb allow for new pit frequency

//nbb #define MODBUS_WRITE_TIMEOUT           410 // 267 bytes plus 20mS turnaround (350mS timeout)
#define MODBUS_WRITE_TIMEOUT                1025 // 267 bytes plus 20mS turnaround (350mS timeout)
//nbb#define MODBUS_BYTE_WRITE_TIMEOUT       60      // 16 bytes plus 4mS turnaround (50mS timeout)
#define MODBUS_BYTE_WRITE_TIMEOUT       150      //nbb 16 bytes plus 4mS turnaround (50mS timeout)

#define POLL_TIMER              BLENDER_PIT_FREQUENCY   // 1 Second's worth of PIT ticks

#define REPORT_TIMEOUT          (20 * PIT_FREQUENCY)    // 20 seconds based on a PIT ticks

// *****
// This is based on the PIT frequency and the baud rate
// Should try to find a way to calculate a value automatically
// *****
//#define INTER_MESSAGE_DELAY             5       // should be 3.5 byte times
#define INTER_MESSAGE_DELAY             13       //nbb should be 3.5 byte times

// This number of succesive timeouts will result
// in a comms alarm being triggered
//#define COMMS_TIMEOUT_LIMIT         20
#define COMMS_TIMEOUT_LIMIT         50          //nbb

// Command codes
#define READ_N_WORDS_CMD_3          0x03
#define READ_N_WORDS_CMD_4          0x04
#define WRITE_A_BIT                 0x05
#define WRITE_A_WORD                0x06
#define FAST_STATUS_READ            0x07
#define WRITE_N_WORDS               0x10

// Error response codes
#define ILLEGAL_FUNCTION_ERROR      0x01
#define ILLEGAL_ADDRESS_ERROR       0x02
#define ILLEGAL_VALUE_ERROR         0x03
#define FAILURE_PERFORM_ERROR       0x04        // Failure to perform an operation
#define NON_CONSECUTIVE_ERROR       0x07
#define PROGRAM_BUSY_ERROR          0x11
#define READ_ONLY_ERROR             0x12
#define UNAVAIL_FUNCTION_ERROR      0x13

// Low level com port status codes
#define MODBUS_GOOD_TRANSMIT        0
#define MODBUS_TRAMSMIT_FAILURE     1
#define MODBUS_GOOD_RECEIVE         2
#define MODBUS_RECEIVE_FAILURE      3
#define MODBUS_CHECKSUM_ERROR       4
#define MODBUS_RESPONSE_ERROR       5

//Offsets to data in comms buffer
#define MODBUS_READ_BYTE_COUNT      2
#define MODBUS_READ_DATA_OFFSET     3
#define MODBUS_WRITE_DATA_OFFSET    5
#define MODBUS_STATUS_DATA_OFFSET   2

// Supported equipment types
/*
#define EQUIP_BATCH_BLENDER         0
#define EQUIP_LOW_BLENDER           1
#define EQUIP_WIDTH_CONTROL         2
#define EQUIP_PROFILER              3
#define EQUIP_LOADER                4
#define EQUIP_PUMP                  5
#define EQUIP_UNKNOWN               6
*/
#define EQUIP_UFP1          0
#define EQUIP_UFP2          1
#define EQUIP_SEI           2
#define EQUIP_LLS           3   // Low Level sensor card - using VAC8.
#define EQUIP_OPTIMISATION  4   // Optimisation card  - using VAC8.
#define EQUIP_LIW           (5)  // Loss-in-Weight card.
#define FIRST_LIW_ADDRESS   (5)  // address of first LiW card.
#define EQUIP_VAC8IO         (6)  // VAC8 io card.
#define EQUIP_SSIF           (7)  // VAC8 io card.

#define FIRST_VAC8IO_ADDRESS   (21)  // address of first VAC8IO card.
#define FIRST_VAC8IO_EXPANSION_ADDRESS   (21)  // address of first VAC8IO expansion card.


// Blender Equipment sub-types
//#define EQUIP_SUBTYPE_BATCH           0
//#define EQUIP_SUBTYPE_LOW         1

// Pump Equipment sub-types
//#define EQUIP_SUBTYPE_IVR         0
//#define EQUIP_SUBTYPE_8CH         1

#define SEND_CAL_DATA_TO_SEI_NO          (3)

// Max length for equipment descriptions (inc. Null terminator)
#define MAX_DESCRIPTION_LENGTH      10
#define MAX_ID_LENGTH               (12)

typedef struct{
    int m_nMessageType; // Indicates the type of message
    int m_nValue;       // Value associated with the COMMS_WRITE_SINGLE_WORD message
                        // Represents the index of the register.
    int m_nMessagePriority; // mmk - new feature, allow priority for each message.
} structMessageQueue;

typedef struct{
        // Message flags used only by blenders
    BOOL    m_bFrontRollInProgress;
    BOOL    m_bBackRollInProgress;
    BOOL    m_bOrderInProgress;
    BOOL    m_bFrontRollAvailable;
    BOOL    m_bBackRollAvailable;
    BOOL    m_bOrderAvailable;
    int     m_nFrontRollRetries;
    int     m_nBackRollRetries;
    int     m_nOrderRetries;

} structBlenderData;

typedef struct{
    // Configuration Data
    int     m_nPumpNumber;
    int     m_nPriority;

    // Program data
    BOOL    m_bManualOverride;
    int     m_nLoaderFillNo;

} structLoaderData;

typedef struct{
    // Configuration Data
    int     m_nPumpIdleTarget;
//mmk   BOOL    m_bFilterInstalled;
    int m_bFilterInstalled;

// mmk  BOOL    m_bCommonMaterialLine;          // mmk need to remove this!!! 26-03-2001
    int m_nAtmValveTime_Overlap;        //Time atm. valve is open during loader fill overlap

    // Program data
    int     m_nPumpIdleTime;
    int     m_nPumpRunTime;
    int     m_nFilterIdleTime;
    BOOL    m_bDoFilterClean;
    BOOL    m_bFilterCleanInProgress;
    BOOL    m_nCleanCount;
} structPumpData;

typedef union
{
    structBlenderData   BlenderData;
    structLoaderData    LoaderData;
    structPumpData      PumpData;
} structUniqueData;


// Structure holding the basic requirements for a controller (structControllerData)
typedef struct {
    // System descriptors
    int     m_nSystemType;
    int     m_nSystemSubType;       // eg. Batch/LOW or IVR/8Ch
    int     m_nSystemAddress;
    char        m_arrcDescriptor[MAX_DESCRIPTION_LENGTH];

    // Message Queueing
    structMessageQueue  m_arrMessageQueue[MAX_QUEUE_SIZE];
    int     m_nQueueHead;
    int     m_nQueueTail;
    BOOL        m_bFull;
    BOOL        m_bEmpty;
    BOOL        m_bLock;

    // Message progress
    BOOL        m_bPollCalibration;
    BOOL        m_bWriteCalibration;    // new MMK
    BOOL        m_bSetpointsValid;
    BOOL        m_bCalibrationValid;
    BOOL        m_bNewSummaryData;      // new mmk
    BOOL        m_bIDValid;                 // new mmk
    BOOL        m_bSeiWriteCommandCompleted;
    char        m_arrcBoardID[MAX_ID_LENGTH];


    int     m_nSetpointRetries;
    int     m_nCalibrationRetries;
    int     m_nSingleWriteRetries;
    int     m_nOptionWriteRetries;
    int     m_nCommandWriteRetries;     // new mmk
    int     m_nDisplayWriteRetries;     // new mmk
    int     m_nIncDecSetpointWriteRetries; // new psmith 1003008
    // Error tracking
    int     m_nCommsTimeouts;
    BOOL        m_bActiveTimeoutAlarm;

    // Data arrays
//  float       *arrnReadData;
//  float       *arrnWriteData;
    int     *arrnReadData;          // Pointers to Modbus read and write tables. - mmk
    int     *arrnWriteData;

    unsigned char m_cFastStatusByte;    // storage for status word from Fast Status Read message.

    // Union containing structures which hold
    // the data unique to each of the different
    // types of equipment
//--REVIEW--    structUniqueData    uniqueData;     (dont see any need for this yet - mmk 17/1/2005)

    // Comms message counters
    long    m_lGoodTx;
    long    m_lGoodRx;
    long    m_lTxTimeoutCounter;
    long    m_lRxTimeoutCounter;
    long    m_lErrorResponseCounter;    // Device responded with an error message
    long    m_lChecksumErrorCounter;
    long    m_lInvalidInterruptCounter;
    BOOL    m_bPollingEnable;       // flag for enabling polling
    char    m_cGroupNumber;

    int m_nDeviceFirstRegisterOffset;   // e.g. in UFP2, first register is at 320, therefor offset = 320.

} structControllerData;



// Data structure required for each serial port (structCommsData)
typedef struct {
    // Flag to stop/Start the comms
    BOOL    m_bCommsEnable;

    // Index to the current unit
    int     m_nCurrentUnit;

    // Count of the number of units installed
    int     m_nInstalledUnits;

    // Pointer to the current unit
    structControllerData    *m_pController;

    // Interface type e.g. RS422 etc.
    int     m_nInterface;

    // Number of bytes to strip out of the receive message
    // when in RS485 mode.
    int     m_nStripLength;
    int     m_nMessageType;
    int     m_nMessageValue;
    int     m_nStartRegister;       // First register in the message
    int     m_nStartIndex;          // Index into the data buffer of the first piece of data.
    int     m_nTimeoutPreset;       // starting value for the timeout

    BOOL        m_bActive;
    int     m_nRetries;
    int     m_nTimeoutCounter;
    int     m_nPollTimer;
    int     m_nKeyPollTimer;
    int     m_nDisplayPollTimer;        //

    int     m_nMessageStartDelay;   // 3.5 byte times
    BOOL        m_bRunStartDelay;
    BOOL        m_bProcessingMessage;

    BOOL        m_bStartFound;

    unsigned char        m_cTxBuffer[COMMS_BUFFER_SIZE];
    int     m_nTXIndex;
    int     m_nTXEndIndex;
    BOOL        m_bTXEnable;

    unsigned char        m_cRxBuffer[COMMS_BUFFER_SIZE];
    int     m_nRXIndex;
    BOOL        m_bRXEnable;
    int     m_nRXLength;            // Expected rx length

    // Comms message counters
    long        m_lGoodTx;
    long        m_lGoodRx;
    long        m_lTxTimeoutCounter;
    long        m_lRxTimeoutCounter;
    long        m_lErrorResponseCounter;    // Device responded with an error message
    long        m_lChecksumErrorCounter;
    long        m_lInvalidInterruptCounter;

} structCommsData;


#endif  // __SERIALSTUFF_H__
