//////////////////////////////////////////////////////
// ConfigureSystem.h
//
// Allows the user to configure the system for the
// number and type of devices connected to it.
//
// M.Parks                              27-03-2000
// First Pass
// M.Parks                              18-04-2000
// Added offset definitions for the 
// M.McKiernan                          20-11-2003
// Added process definitions (fibre stuff), GROUP NO IN EEPROM COMMS BLOCK.
// M.McKiernan                          02-03-2004
// Added EEPROM_FILTER_CLEAN_FREQUENCY
// M.McKiernan                          03-11-2004
// Rearranged initial definitions for calibration block.
// The old data structure( EEPROM_WEIGHT_CONSTANT etc) was used initially, and has now been superseded, i.e. no longer used,
//  and it can and should be removed.  --todolp--
//      The calibration data is now stored as one block always (not individual pieces) and it starts from
//      EEPROM_CALIBRATION_DATA_START in the EEPROM.
//
// P.Smith                              26/09/06
// ONBOARD_EEPROM_CALIBRATION_DATA_START define as a hex no 0x0
//
// P.Smith                              6/3/07
// modify EEPROM_RECIPE_0_START to start at 3000
//
// P.Smith                              15/10/07
// ONBOARD_EEPROM_CALIBRATION_DATA_START set to 8000
//
// P.Smith                              27/3/08
// PROCESS_DATA_EEPROM_CALIBRATION_DATA defined as 2500
//
// P.Smith                              1/4/09
// added DESCRIPTION_DATA_EEPROM at 0
//
// P.Smith                              22/7/09
// added ONBOARD_EEPROM_ADDITIONAL_RECIPE_DATA_START
//////////////////////////////////////////////////////


//*****************************************************************************
// MACROS
//*****************************************************************************
#ifndef __CONFIGURESYSTEM_H__
#define __CONFIGURESYSTEM_H__


void    ConfigureSystem( void );


// ONBOARD EEPROM is 8Kb (8192 bytes)
#define EEPROM_SIZE_BYTES       8192

#define DESCRIPTION_DATA_EEPROM         0   // 
#define   STANDARD_FILTER_CLEAN_FREQUENCY 0  //standard or normal filter clean mode.

// REMOVABLE EEPROM is 8Kb (8192 bytes)

// We skip the first 20 bytes because they may be used for
// the serial number and options
#define EEPROM_START_BLOCK  20

// #define  EEPROM_LANGUAGE                 (EEPROM_START_BLOCK + 0)
/*
#define EEPROM_MEASUREMENT_UNITS_START  (EEPROM_START_BLOCK + 1)
// leave room for 15 measurement units.
#define EEPROM_RECIPE_ENTRY_MODE            (EEPROM_START_BLOCK + 16)

// Leave space for other unit configuration options
#define EEPROM_COMMS_UNITS              (EEPROM_START_BLOCK + 30)

#define EEPROM_COMMS_TYPE_1             (EEPROM_START_BLOCK + 31)
#define EEPROM_COMMS_SUB_TYPE_1         (EEPROM_START_BLOCK + 32)
#define EEPROM_COMMS_ADDRESS_1          (EEPROM_START_BLOCK + 33)
#define EEPROM_COMMS_SUB_ADDRESS_1      (EEPROM_START_BLOCK + 34)
#define EEPROM_COMMS_GROUP_NUMBER_1     (EEPROM_START_BLOCK + 35)       //GROUP No.
#define EEPROM_COMMS_SPARE2_1           (EEPROM_START_BLOCK + 36)
#define EEPROM_COMMS_SPARE3_1           (EEPROM_START_BLOCK + 37)

#define EEPROM_COMMS_DESCRIPTION_1      (EEPROM_START_BLOCK + 38)
#define EEPROM_COMMS_UNIQUE_DATA_1      (EEPROM_START_BLOCK + 39 + MAX_DESCRIPTION_LENGTH)

#define EEPROM_COMMS_UNIQUE_DATA_SIZE   22
#define EEPROM_COMMS_BLOCK_SIZE     (8 + MAX_DESCRIPTION_LENGTH + EEPROM_COMMS_UNIQUE_DATA_SIZE)
*/

// The end address for the configuration is worked out as follows
//  = EEPROM_START_BLOCK + EEPROM_COMMS_BLOCK_SIZE * MAX_COMMS_UNITS
//  = 20 + (40 * 32)
//  = 1300
// Increasing the number of comms units to 64 would bring the size up
// to 2580

// the recipes are the last thing written out because the size
// and quantity are variable so it's not possible to reserve space
// for them
#define  EEPROM_START_OLD       4       //bb 

// --REVIEW-- Initial block of 30 bytes in EEPROM can be freed up, for ID's etc.
//NB: The following data structure( EEPROM_WEIGHT_CONSTANT etc) was used initially, and has now been superseded, i.e. no longer used. 
//      The calibration data is now stored as one block always (not individual pieces) and it starts from
//      EEPROM_CALIBRATION_DATA_START in the EEPROM.
#define EEPROM_START_SPARE      (EEPROM_START_OLD + 0)
#define EEPROM_UNIT_TYPE            (EEPROM_START_OLD + 1)              // Units type, metric/english.
#define EEPROM_WEIGHT_CONSTANT  (EEPROM_START_OLD + 2)              // WEIGHT CONST = FLOAT, 4 BYTES
#define EEPROM_BATCH_SIZE           (EEPROM_START_OLD + 6)          // bATCH SIZE = FLOAT, 4 BYTES
#define EEPROM_TARE_COUNTS      (EEPROM_START_OLD + 10)     // tARE COUNTS = FLOAT, 4 BYTES.
#define EEPROM_DIAG_MODE            (EEPROM_START_OLD + 14)     // Diagnostics mode = int, 2 BYTES.
#define EEPROM_LANGUAGE         (EEPROM_START_OLD + 16)     // language = int, 2 BYTES.

#define EEPROM_MANAGER_PASSWORD     EEPROM_START_OLD + 18
#define EEPROM_OPERATOR_PASSWORD    EEPROM_START_OLD + 20

// This is the Correct Calibration data 04.11.2004
#define  EEPROM_CALIBRATION_DATA_START 30       //bb Reserve 1st 30 bytes for ID's etc.

#define  PROCESS_DATA_EEPROM_CALIBRATION_DATA  2500  //nbb--todo-- check this

#define  ONBOARD_EEPROM_CALIBRATION_DATA_START  8000     //EEP1STARTADR    EQU     0000            ; START ADDRESS IN EEPROM.



//#define   EEPROM_FILTER_CLEAN_FREQUENCY   2982        //reserve 1 byte for Filter clean frequency.

//#define   EEPROM_TIMER_1_PERIOD       2983        //reserve 2 bytes for TIMER #1 PERIOD
//#define   EEPROM_TIMER_2_PERIOD       2985        //reserve 2 bytes for TIMER #2 PERIOD

//#define   EEPROM_PROCESS_TYPE         2987        //reserve 1 bytes for PROCESS E.G. Fibre process.
//#define   EEPROM_GROUP_1_DESCRIPTION  2988            //reserve 4 bytes for layer group names - Fibre process.
//#define   EEPROM_GROUP_2_DESCRIPTION  2992


#define ONBOARD_EEPROM_ADDITIONAL_RECIPE_DATA_START  2850  // Allow  bytes for calibration data,
#define EEPROM_RECIPE_0_START           3000         // Allow  bytes for calibration data,


#endif  // __CONFIGURESYSTEM_H__
