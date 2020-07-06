///////////////////////////////////////////////////////
// BatchMBIndices.h
//
//
// M.McKiernan                                  04-06-2004
// First pass.  (Adapted from MBIndices.h)
//
// Added - BATCH_CALIBRATION_PURGE_COMPONENT        156
// Added whole pile of system config. definitions, between BATCH_CALIBRATION_WEIGHT_CONSTANT    152
// and BATCH_CALIBRATION_PASSWORD_2 192
// Added some parameters from 1200 - 1207. BATCH_POWER_UP_TIME etc.
// M.McKiernan                                  15-11-2004
// Added some MBToggle Status parameters from 279.
// Added BATCH_SUMMARY_OPERATIONAL_STATUS_2
// Definition for PSUM_BLOCK_SIZE corrected.
// Added more definitions after 1200.
// Added definitions for cycle diagnostics, PSUM_BLOCK_11 and PSUM_BLOCK_12, and alarm blocks
// M.McKiernan                                  15-12-2004
// Changed the alarm block definitions.  Block size increased to 10. Order no and Operator No added.

// P.Smith                                      4-04-2005
// added BATCH_POWER_UP_DATE
// P.Smith                                      11-08-2005
// added MBCSTATCLEANST2POS
//
// P.Smith                                      17-08-2005
// added modbus definitions for 192-197
// added END_TOGGLE_STATUS_COMMANDS definition to ensure that the command to do
// the clean command is allow to operate.
//
// P.Smith                                      18-08-2005
// added BATCH_CALIBRATION_USE_REGRIND_AS REF_BITPOS
// added encryption definitions from 1248-1255
// added single recipe mode defintions 1256-1261
// added BATCH_ENCRYPTION_WRITE_CODE at 281
// added MBRECIPEWAITINGBITPOS,MBINHIBITBITPOS
//
// P.SMITH                                      26/08/05
// ADDED BATCH_END_SYSTEM_CONFIG
//
// P.SMITH                                      10/10/05
//  Correct position of MBTOTWACCBAT, move to input state and alarm state to 1246,1247
//  and insert total weight accumulator to position 432
//
// P.SMITH                                      17/10/05
// Name change to BATCH_CALIBRATION_OPTIONS at 114 was bypass.
// define BATCH_CAL_OPTIONS_CORRECTEVERYCYCLEBITPOS
//
// P.Smith                                      11/11/05
// added toggle commands
//
//
// P.Smith                                      15/11/05
// added BATCH_CONFIG_SINGLE_WORD_WRITE_BIT,BATCH_CONFIG_FAST_SLOW_CYCLE_BIT,BATCH_CONFIG_PURGE_BIT
//
// P.Smith                                      19/5/06
// added BATCH_DTOA_INCREASE_COMMAND at 268 and BATCH_DTOA_DECREASE_COMMAND at 269
// added BATCH_START_TOGGLE_STATUS_COMMANDS
//added BATCH_EXTRUDER_SET_PERCENTAGE at 1262
//
// P.Smith                                      31/5/06
// added #define BATCH_GENERAL_TOGGLE_STATUS_COMMANDS        284 //
// defineed BATCH_GENERAL_TOGGLE_STATUS_COMMANDS_RESET_TOTALS_BIT   0x01
//
// P.Smith                                      13/6/06
// added #define  BATCH_CALIBRATION_LATENCY                  198    //
//
// P.Smith                                      15/6/06
// define EMAIL_MAIL_SERVER
// define EMAIL_USER_NAME
// define EMAIL_PASSWORD
// define EMAIL_SUPERVISOR_EMAIL_ADDRESS
// define EMAIL_SUPPORT_EMAIL_ADDRESS
// define MB_WRITE_EMAIL_CONFIG_BIT
// define  BATCH_LAST_WRITE_REGISTER  EMAIL_END               //
//
// P.Smith                                      16/6/06
// added #define BATCH_GENERAL_TOGGLE_STATUS_COMMANDS_SEND_SYSTEM_CONFIG_EMAIL_BIT 0x04
// added #define BATCH_CALIBRATION_SEND_EMAIL_ON_ORDER_CHANGE_BITPOS 0x02//
// added  BATCH_CALIBRATION_LATENCY at 198
// redefine  BATCH_END_SYSTEM_CONFIG
// refdefine  BATCH_LAST_WRITE_REGISTER to allow write of email parameter data
// redefine MB_TABLE_END to indicate end of modbus table
//
// P.Smith                                      9/8/06
// define BATCH_CALIBRATION_ZERO_LONG_TERM_WEIGHT_ACCUMULATOR
//
// P.Smith                                      14/11/06
// added BATCH_CONTROL_LINE_SPEED at 279, moved BATCH_OUTPUT_DIAGNOSTICS to 283
// added BATCH_CALIBRATION_LINE_SPEED_SIGNAL_TYPE at 201
//
// P.Smith                                      15/11/06
// added  BATCH_DTOA_PERCENTAGE in toggle status command
// ADDED MBENTRYTYPE AT 177                                ;
// Remove monitoring reference.
// remove monitor factor from this position.               ;
// leave BATCH_END_TOGGLE_STATUS_COMMANDS as it was
// P.Smith                              2/1/07
// BATCH_CALIBRATION_LINE_SPEED_SIGNAL_TYPE change to address 128
// BATCH_CALIBRATION_LATENCY to 201
//
// P.Smith                              20/2/07
// added BATCH_START_SYSTEM_CONFIG2 & BATCH_END_SYSTEM_CONFIG2
//
// P.Smith                              6/3/07
// define BATCH_RESET_FLOW_RATES at 301
//
// P.Smith                              13/4/07
// added BATCH_SOFTWARE_UPDATE_FROM_SD_CARD at 302
//
// P.Smith                              13/4/07
// add recipe download storage at 3000
// add Multiblend extruder set up
// add toggle status commands associated with multiblend at 310
// allow all of the table to be written to.
//
// P.Smith                              3/5/07
// added BATCH_SETPOINT_MICRONS at 72 to allow set microns to be downloaded
//
// P.Smith                              23/5/07
// added power up time in location 1263 - 1266
//
// P.Smith                              24/5/07
// added BATCH_POWER_UP_RESET_COUNTER at 1267
// added ip setting at 1270 & download af 2351
//
// P.Smith                              11/6/07
// change multi blend so that all recipes are store in the blender.
// assign blocks for extruder recipes.
// added BATCH_SETPOINT_DOWNLOAD_MANAGER
// defined #define INHIBIT_DOWNLOAD,ALLOW_DOWNLOAD
//
// P.Smith                              11/6/07
// added NETBURNER_CONFIG_MACADDRESS,NETBURNER_DHCP_STATUS
//
// P.Smith                              19/6/07
// added at 4000 OPTIMISATION_SUMMARY_FLAP_STATUS_1 - 16, OPTIMISATION_SUMMARY_ENABLE_STATUS_1-16
// OPTIMISATION_SUMMARY_FILL_STATUS_1-16
// entend MB_TABLE_END
//
// P.Smith                              21/6/07
// added BATCH_NETWORK_TX_CTR, BATCH_NETWORK_RX_CTR,BATCH_NETWORK_CSUM_ERR_CTR at 1293
//
// P.Smith                              25/6/07
// added additional SD card commands from 302 onwards.
//
// P.Smith                              27/6/07
// added BATCH_CALIBRATION_PURGE & BATCH_OPTIMISATION
//
// P.Smith                              4/7/07
// added bin volume at 202 - 213
//
// P.Smith                              5/7/07
// added optimisation weight and length summary data
// added TOGGLE_STATUS_COMMAND2_PURGE & TOGGLE_STATUS_COMMAND2_LENGTH
//
// P.Smith                              25/7/07
// changed purge option to address 215 (was originally 198 - component per alarm enable)
// change multiblend recipe offset down to 0 to suit the plc application
//
// P.Smith                              26/7/07
// added MULTIBLEND_RECIPEDOWNLOAD_SUMMARY_BLOCK1 - 16 to show summary data
// END_OF_MODBUS extended to 5000
//
// P.Smith                              27/7/07
// added OPTIMISATION__ORDER_LENGTH at 4649
//
// P.Smith                              2/8/07
// added TOGGLE_STATUS_COMMAND2_VIBRATE_CATCH_BOX at 316
// added BATCH_VACUUM_ON at 1268, BATCH_FILL_BY_WEIGHT_1-12 at 1546
// BATCH_IS_DIAGNOSTICS_PASSWORD at 1558
// define LAST_MODBUS_DATA, this was causing a problem when the modbus
// table array was not big enough to hold all the required data.
// when the multiblend download recipe was activated, the copy function
// was overwriting part of the temp recipe file, clearing some of the
// set component % and there by storing the incorrect recipe into eeprom.
// MB_TABLE_SIZE is used to define the array size, originally it was defined
// at 5000, where a size of 5600 was required.
// MULTIBLEND_RECIPEDOWNLOAD_SUMMARY_BLOCK1-16 was corrected
//
// P.Smith                              27/8/07
// added new alarm format to allow alarm data to be read easily via the modbus table.
//
// M.McKiernan                          25/9/07
// Moved alarm blocks into correct position in  table. (same definition).
// Defined Read only area is area of table which we want to prevent people (external) writing into.
//  MB_TABLE_READ_ONLY_START       (BATCH_SUMMARY_START)
//  MB_TABLE_READ_ONLY_END     		(BATCH_END_OF_READ_ONLY_SECTION)
//  MB_TABLE_READ_ONLY_SIZE    		(MB_TABLE_READ_ONLY_START - MB_TABLE_READ_ONLY_END)
//
// P.Smith                           3/10/07
// added BATCH_CALIBRATION_CLEANING at 199,BATCH_CALIBRATION_LINE_SPEED_AVERAGE at 216
// BATCH_CALIBRATION_MAX_PERCENTAGE_DEVIATION 217 - 228, BATCH_CALIBRATION_MAX_PAUSE_ON_ERROR_NO 229
// BATCH_CALIBRATION_COMPONENT_NAMING_ALPHANUMERIC - 230
// BATCH_END_OF_CONFIG 265
//
//  P.Smith                             16/11/07            ;
//  added mbscrewspeedatd (433), mbtarescrewspeed (302)     ;
//  mbcalscewspeed (303)
//  added BATCH_SUMMARY_SCREW_SPEED_ATD added at 433
//
//  P.Smith                             21/11/07            ;
//  change optimsiation to address 216, was 202
// changed line speed average from 216 to 231
// correct BATCH_CYCLE_LONG_TERM_WEIGHT_1 modbus address is being conflited with
// the fill by weight status being fed back, this was causing the fill by weight
// and the long term totals to conflict in modbus and one overwrites the other.
//
//  P.Smith                             6/12/07            ;
// added BATCH_DEVIATION_ALARM_ENABLE at 198
//
//  P.Smith                             9/1/08            ;
// BATCH_OPTIMISATION UNUSED name corrected to BATCH_OPTIMISATION_UNUSED
//
// P.Smith                             10/1/08            ;
// Pause blender on power up BATCH_PAUSE_BLENDER_ON_POWER_UP added at 232
//
// P.Smith                             28/1/08            ;
// added usb related modbus data at 323-327, added reset sample counter at 330
// added batch statistical data at 6000.
//
// P.Smith                             20/3/08            ;
// added BATCH_LOG_TO_SD_CARD at 233 & BATCH_LOG_FILE_TYPE_CSV at 234
// added usb stuff at 6109
// added #define INCREASE_DECREASE_TEST & INCREASE_DECREASE_TEST_READ 6990, 6991
//
// P.Smith                             23/4/08            ;
// added BATCH_CALIBRATION_ALLOW_CONTINUOUS_DOWNLOAD
//
// P.Smith                             29/4/08
// correct passwords BATCH_CALIBRATION_PASSWORD_SUPERVISOR & BATCH_CALIBRATION_PASSWORD_OPERATOR
//
// P.Smith                             4/6/08
// added BATCH_HARDWARE_ID at 1581
//
// P.Smith                             6/6/08
// added sd card complete messages at 6420
//
// P.Smith                             12/6/08
// added individual process and system alarms at 368 - 399
//
// P.Smith                             17/6/08
// added BATCH_CALIBRATION_TELNET at 236
//
// P.Smith                             23/6/08
// added BATCH_SEI_SOFTWARE_VERSION_NO at 6425
// added BATCH_SEI_HARDWARE_ID at 6430
// comms status added at 1589
// BATCH_SEI_SOFTWARE_VERSION_NO at 6425, BATCH_SEI_HARDWARE_ID at 6430
// remove
//#define BATCH_ENCRYPTION_KGHR_COUNTER               1249 // unused
//#define BATCH_ENCRYPTION_GPM_COUNTER                1250 // unused
//#define BATCH_ENCRYPTION_MODBUS_HOUR_COUNTER        1251 // unused
//
// P.Smith                             17/7/08
// added TOGGLE_STATUS_COMMAND2_OPERATOR_PANEL_PAGE_ID at 317 and
// PSUM_BLENDER_TYPE at 656
//
// P.Smith                             21/7/08
// added SD_CARD_STATUS at 6118
//
// P.Smith                             24/7/08
// add TOGGLE_STATUS_COMMAND2_SAFELY_REMOVE_SD_CARD at 318 and
// BATCH_SD_CARD_DISABLE_ACCESS_CTR at 6435
//
// P.Smith                             26/9/08
// add TOGGLE_STATUS_COMMAND2_LIQUID_ADDITIVE_COMMAND
// define BATCH_CALIBRATION_LA_CALIBRATION_KGHR at 256
// add summary data for pib at 6500
// add temperature to modbus table, set kg/hr, output d/a value
// changed BATCH_END_OF_CONFIG to 254
//
// P.Smith                             26/9/08
// add BATCH_SUMMARY_PROCESS_ANY_ALARM_OCCURRED at 367
//
// P.Smith                             13/10/08
// added BATCH_SUMMARY_LIQUID_ADDITIVE_ON_TIMER at 6503
//
// P.Smith                             16/10/08
// added BATCH_CALIBRATION_LA_SCALING_FACTOR at 261
// BATCH_TOGGLE_STATUS_RESET_LONG_TERM_COUNTERS at 329
// BATCH_CYCLE_LONG_TERM_LENGTH_RESETTABLE at 1596
// BATCH_CYCLE_LONG_TERM_LENGTH at 1598
// BATCH_SUMMARY_LIQUID_ADDITIVE_ON_TIMER at 6503
//
// P.Smith                             26/11/08
// remove BATCH_BLENDINGONLY and reset of defn
//
// P.Smith                             1/12/08
// added USB_INSERTED at 6109
// extended modbus to 9000 to allow for new data at 8000 for write of
//
// P.Smith                             10/12/08
// define usb / printer enable options at 5600
// define option to print reports at 334
//
// P.Smith                             7/1/09
// added BATCH_STATUS_HIGH_REGRIND_LEVEL_SENSOR instead of BATCH_STATUS_CALIBRATION_MASK
//
//
// P.Smith                             8/1/09
// added commands to blast air into bins 2370 - 2381
// added commands to drain bins 2382 - 2393
//
// P.Smith                             21/1/09
// added BATCH_FUNCTION_RUNNING, BATCH_FUNCTION_SUBSECTION_RUNNING & BATCH_BATSEQ from 6436
//
// P.Smith                             29/1/09
// added BATCH_COMMUNICATIONS_NO_PARITY
//
// P.Smith                             13/2/09
// added usb timeout counters at 6439
//
// P.Smith                             26/3/09
// added TOGGLE_STATUS_TEST_FUNCTIONALITY at 334
// BATCH_COMMS_STATUS_PANEL at 1594
// BATCH_NBB_SERIAL_NUMBER at 6436
// BATCH_PANEL_VERSION_NUMBER at 6444
// move BATCH_PANEL_VERSION_NUMBER down to 6448
// self test stuff at 6458
// BATCH_BLENDER_MODBUS_DIAGNOSTIC_ADDRESS at 6504
// BATCH_BLENDER_MODBUS_DIAGNOSTIC_DATA at 6505
//
// P.Smith                             24/4/09
// added #define BATCH_NBB_PASS_FAIL_STATUS at 6525
// and definitions of states
//
// P.Smith                             18/5/09
// correct flap status at 4514, 4515
//
// P.Smith                             8/6/09
// added BATCH_CHECK_FOR_VALID_COMPONENT_PER_TARGET at 237
//
// P.Smith                             11/6/09
// define vacuum loading stuff at 6600
//
// P.Smith                             25/6/09
// added BATCH_COMMS_STATUS_VAC8S at 1595
// BATCH_BLENDER_SELF_TEST_COMP_STDEV               6477
// BATCH_BLENDER_SELF_TEST_LOAD_CELL_SWITCH         6478
// add vac8 counters at 6611 - 6633
// loader block was changed to 6650
//
// P.Smith                             30/6/09
// added BATCH_FOREGROUND_CTR,BATCH_FOREGROUND_ENTRY_EXIT
//
// P.Smith                          2/7/09
// name change dump delay to discharge time
//
// P.Smith                          8/7/09
// added filling method at 238 add ics stuff
//
// P.Smith                          25/08/09
// added BATCH_UNLOCK_CONFIG at 6481
// it was also necessary to show the accumulated weight as a double
// this was added at position 657
//
// P.Smith                          1/9/09
// added TOGGLE_STATUS_COMMAND_INITIATION at 335
// added BATCH_LOCK_CONFIG_STATUS at 6481
//
// P.Smith                          8/9/09
// added BATCH_CALIBRATION_PASSWORD_TSM at 250
//
// P.Smith                          10/9/09
// added BATCH_BLENDER_SELF_TEST_IN_PROGRESS at 6479
//
// P.Smith                          17/9/09
// added mmk stuff for loader test
//
// P.Smith                          30/9/09
// add loaders 16-24 to modbus table.
//
// P.Smith                          2/10/09
// added min open time check enable / disable
//
// P.Smith                          12/10/09
// added BATCH_FLOW_RATE_CHECK at 252, BATCH_MAX_KGPH_POSSIBLE at 6484
//
// P.Smith                          15/10/09
// added BATCH_VAC8_EXPANSION at 253
//
// P.Smith                          19/11/09
// added BATCH_BLENDER_SELF_TEST_REF_STABILITY at 6485
// added BATCH_BLENDER_SELF_TEST_LOAD_CELL_STABILITY at 6486
//
// P.Smith                          11/1/2010
// added sd card error data at 6526
//
// P.Smith                          15/1/2010
// add BATCH_SUMMARY_SD_CARD_ERROR_COUNTER, change addresses to allow this to be added
//
// P.Smith                          5/3/10
// added BATCH_ENCRYPTION_OPTIONS_2
//
// P.Smith                          9/3/10
// added BATCH_SUMMARY_WATCH_DOG_TIMER_STATUS
//       BATCH_SUMMARY_EIP_ENABLED
//       BATCH_SUMMARY_EIP_CTR
//       BATCH_SUMMARY_EIP_ASSEMBLY_OBJECT_CTR
//
// P.Smith                          25/3/10
// added security fail counters to modbus table.
//
// P.Smith                          15/4/10
// BATCH_SUMMARY_NETBURNER_MODULE_TYPE allocated at 6545
// BATCH_SET_LICENSE_OPTIONS_1 - 4 allocated at 8000
//
// P.Smith                          19/4/10
// added BATCH_SUMMARY_UNLIMITED_LICENSE_ACCESS at 6547
//
// P.Smith                          22/4/10
// added BATCH_SUMMARY_BLENDER_RUN_TIME &  BATCH_SUMMARY_VALVE_OPERATIONS
// for component 1-12, other event data at 7000
// define network baud rate BATCH_CALIBRATION_NETWORK_BAUD_RATE at 127
//
// P.Smith                          16/6/10
// added BATCH_SETPOINT_WIDTH_OFFSET at 19  nbb--todo--width at seperate off at width mb addresses
//
// M.McKiernan						23/4/2020
//  Changed //#define BATCH_CALIBRATION_VAC_LOADING               117 // VACUUM LOADING ON  0 = yes, 1 = No                    117
// to #define CALIBRATION_MACHINE_TYPE                117 //	wfh - 23.4.2020
// wfh
// #define FORCE_RESET_VALUE							(11011)	//
// #define FORCE_FAST_INFLATE							(11012) // for testing Fast Inflate o/p.
// #define FORCE_USMUX_A0_LOW				(11014)
// #define FORCE_USMUX_A0_HIGH				(11015)
// #define FORCE_USMUX_CLK_LOW				(11016)
// #define FORCE_USMUX_CLK_HIGH			    (11017)
// #define CLR_USMUX_TESTS					 (11013)
////////////////////////////////////////////////////////////////////
//*****************************************************************************
#ifndef __BATCHMBINDICES_H__
#define __BATCHMBINDICES_H__

#include "General.h"

// Batch BATCH

// masks for the MB write flags
#define MB_WRITE_PERCENTAGES_BIT            0x0001
#define MB_WRITE_THRUPUTS_BIT               0x0002
#define MB_WRITE_ORDER_INFO_BIT             0x0004
#define MB_WRITE_COMP_CONFIG_BIT            0x0008
#define MB_WRITE_WEIGHT_CALIB_BIT           0x0010
#define MB_WRITE_HOPPER_CALIB_BIT           0x0020
#define MB_WRITE_MEAS_PARAMETERS_BIT        0x0040
#define MB_WRITE_SPEED_PARAMETERS_BIT       0x0080
#define MB_WRITE_SYSTEM_CONFIG_BIT          0x0100
#define MB_WRITE_LIQUID_CALIB_BIT           0x0200
#define MB_WRITE_TIME_BIT                       0x0400
#define MB_WRITE_EMAIL_CONFIG_BIT         0x0800          //  email config bit
#define MB_TOGGLE_STATUS_BIT                    0x1000          // TOGGLE STATUS COMMANDS
#define MB_WRITE_CALIBRATION_WEIGHT_BIT 0x2000          // CALIBRATION WEIGHT

#define MB_WRITE_SYSTEM_CONFIG3_BIT          0x4000

#define MB_WRITEABLE_BIT                        0X8000          // INDICATES PARAMETER IN MB IS WRITEABLE.


#define MB_WRITE_RESET_TOTALS_BIT           0x4000          // rESET ORDER TOTALS.





// ******************************************
// BATCH BLENDER
// ******************************************

// SETPOINTS
#define  BATCH_SETPOINT_PERCENT_1            0
#define  BATCH_SETPOINT_PERCENT_2            1
#define  BATCH_SETPOINT_PERCENT_3            2
#define  BATCH_SETPOINT_PERCENT_4            3
#define  BATCH_SETPOINT_PERCENT_5            4
#define  BATCH_SETPOINT_PERCENT_6            5
#define  BATCH_SETPOINT_PERCENT_7            6
#define  BATCH_SETPOINT_PERCENT_8            7
#define  BATCH_SETPOINT_LIQUID                 8
#define  BATCH_SETPOINT_LIQUID_2             9
#define  BATCH_SETPOINT_ADDITIVE                10
#define  BATCH_SETPOINT_HIGH_REGRIND            BATCH_SETPOINT_ADDITIVE          // USED FOR HIGH REGRIND %

#define  BATCH_SETPOINT_REGRIND_COMPONENT    11
//Target throughputs.
#define  BATCH_SETPOINT_WEIGHT_LENGTH        12                 // LONG WT/M SETPOINT (2DP 655.35g/M)        ; 0012,13
#define BATCH_SETPOINT_TOTAL_THROUGHPUT     14                  // TOTAL KG/H SETPT (1DP 6553.5 KG/H)   ; 0014

#define BATCH_SETPOINT_WEIGHT_AREA              15                  // g/m2 SETPT (2DP 655.35 g/m2)         ; 0015
#define BATCH_SETPOINT_LINE_SPEED               16                  // LINE SPEED SP (1DP 6553.5 m/Min)     ; 0016
#define BATCH_SETPOINT_WIDTH                        17                  // WIDTH S.P. (mm 65.535m)              ; 0017
#define BATCH_SETPOINT_TRIMMED_WIDTH            18                  // TRIMMED WIDTH (mm)                   ; 0018

#define BATCH_SETPOINT_WIDTH_OFFSET             19                  // WIDTH OFFSET FOR ULTRASONIC
#define BATCH_SETPOINT_SETSPARES_1              20                  // SPARES 20-21.

// Order info.
#define BATCH_SETPOINT_FRONT_ROLL_LENGTH        21              // FRONT ROLL LENGTH SETPOINT (m)       ; 0021
#define BATCH_SETPOINT_BACK_ROLL_LENGTH     22              // BACK  ROLL LENGTH SETPOINT (m)       ; 0022
#define BATCH_SETPOINT_ORDER_LENGTH         23              // ORDER LENGTH SETPOINT (m)            ; 0023/24

#define BATCH_SETPOINT_ORDER_WEIGHT         25              // ORDER WT SETPT (NO DP, Kg)           ; 0025/26
#define BATCH_SETPOINT_CUSTOMER_CODE            27              // CUSTOMER CODE NEW **VERS03**         ; 0027-34

//--review if material code should be put else where
//#define BATCH_SETPOINT_MATERIAL_CODE            35              // MATERIAL CODE NEW **VERS03**         ; 0035-42

#define BATCH_SETPOINT_ORDER_NO_INTEGER         35          // MBIORDERNO      DS      2   ; ORDER NO                            ; 35
#define BATCH_SETPOINT_OPERATOR_NO              36          // MBOPERATORNO    DS      2   ; OPERATOR NO.                        ; 36
#define BATCH_EXTRUDER_NO                       37          // MBEXTRUDERNO    DS      2   ; EXTRUDER NO                         ; 37
#define BATCH_RECIPE_NO                         38          // MBRECIPENO      DS      2   ; RECIPE NO                           ; 38

// 39-42 spare


#define BATCH_SETPOINT_ORDER_NUMBER             43          // ORDER NO      NEW **VERS03**         ; 0043-50
#define BATCH_SETPOINT_SHEET_TUBE               51              // SHEET/TUBE FLAG                      ; 0051
#define BATCH_SETPOINT_FRONT_SLIT_REELS         52              // NO OF FRONT SLIT REELS               ; 0052
#define BATCH_SETPOINT_BACK_SLIT_REELS          53              // NO OF BACK SLIT REELS                ; 0053
#define BATCH_SETPOINT_TRIM_FLAG                54              // 1 = TRIM BEING FED BACK              ; 0054
#define BATCH_SETPOINT_PERCENT_9                55              //MBPCNT9         DS     2        ; Component 9 % set point      ; 0055
#define BATCH_SETPOINT_PERCENT_10               56              //MBPCNT10        DS     2        ; Component 10 % set point     ; 0056
#define BATCH_SETPOINT_PERCENT_11               57              //MBPCNT11        DS     2        ; Component 11 % set point     ; 0057
#define BATCH_SETPOINT_PERCENT_12               58              //MBPCNT12        DS     2        ; Component 12 % set point     ; 0058





// K values
#define BATCH_SETPOINT_DENSITY_1                59             // MBDENSITY1      DS      2       ; density for comp #1                   ; 0059
#define BATCH_SETPOINT_DENSITY_2                60             // MBDENSITY2      DS      2       ; density for comp #2                   ; 0060
#define BATCH_SETPOINT_DENSITY_3                61             // MBDENSITY3      DS      2       ; density for comp #3                   ; 0061
#define BATCH_SETPOINT_DENSITY_4                62             // MBDENSITY4      DS      2       ; density for comp #4                   ; 0062
#define BATCH_SETPOINT_DENSITY_5                63             // MBDENSITY5      DS      2       ; density for comp #5                   ; 0063
#define BATCH_SETPOINT_DENSITY_6                64             // MBDENSITY6      DS      2       ; density for comp #6                   ; 0064
#define BATCH_SETPOINT_DENSITY_7                65             // MBDENSITY7      DS      2       ; density for comp #7                   ; 0065
#define BATCH_SETPOINT_DENSITY_8                66             // MBDENSITY8      DS      2       ; density for comp #8                   ; 0066
#define BATCH_SETPOINT_DENSITY_9                67             // MBDENSITY9      DS      2       ; density for comp #9                   ; 0067
#define BATCH_SETPOINT_DENSITY_10               68             // MBDENSITY10     DS      2       ; density for comp #10                  ; 0068
#define BATCH_SETPOINT_DENSITY_11               69             // MBDENSITY11     DS      2       ; density for comp #11                  ; 0069
#define BATCH_SETPOINT_DENSITY_12               70             // MBDENSITY12     DS      2       ; density for comp #12                  ; 0070


#define BATCH_SETPOINT_RESET_TOTALS             71             // Reset Totals.
#define  BATCH_SETPOINT_MICRONS                 72             // MBMICRONSET      DS     2       ; MICRON SET                            ;0072

// Bit allocations for the Reset Totals flag.
#define MB_RESET_TOTALS_BIT 0x0001              //MBRSTOTLSBPOS    EQU     %0000000000000001       ; RESET TOTALS

#define MB_RESET_TOTALS_CLEAR_MASK  0xFFFE
#define MB_START_CLEAN_BIT      0x0002          //MBSTARTCLEANBPOS EQU     %0000000000000010       ; Start Clean.

// --review-- if necessary



#define BATCH_SETPOINT_SPARES_3                 72              // SPARES 72-89
#define BATCH_SETPOINT_DOWNLOAD_MANAGER         74              //


#define INHIBIT_DOWNLOAD                        01
#define ALLOW_DOWNLOAD                          02



#define BATCH_MB_START      BATCH_SETPOINT_PERCENT_1            // DEFINE TABLE START.
#define BATCH_SETPOINT_START        BATCH_SETPOINT_PERCENT_1
#define BATCH_SETPOINT_END      BATCH_SETPOINT_RESET_TOTALS


/****************************************************************
*                      CALIBRATION DATA                         *
****************************************************************/

// Averages.
#define BATCH_CALIBRATION_CONTROL_AVERAGE       90                  // Control Average Factor
#define BATCH_CALIBRATION_WEIGHT_AVERAGE            91                  // Weight Average.
#define BATCH_CALIBRATION_BATCHES_KGH_AVERAGE   92                  //batches in kg/h average.
#define BATCH_CALIBRATION_CONTROL_GAIN          93                  // Control gain
#define BATCH_CALIBRATION_MIXING_TIME               94                  // Mixing time.
#define BATCH_CALIBRATION_SPARES_1                  95                  // SPARES 95-96

// LINE AND SCREW SPEED CALIBRATION DATA.
#define BATCH_CALIBRATION_LINE_SPEED_CONST      97                  // LINE SPEED CONST (Max 65535)          97
#define BATCH_CALIBRATION_SCREW_SPEED_CONST     98                  // SCREW SPEED CONST (Max 65535)         98
#define BATCH_CALIBRATION_SCREW_TARE_COUNTS     99                  // SCREW SPEED TARE COUNTS (integer)     99
#define BATCH_CALIBRATION_MAX_SCREW_SPEED       100
#define BATCH_CALIBRATION_SPARES_2                  101             // SPARES 101-104

// SYSTEM CONFIGURATION

#define BATCH_CALIBRATION_LINE_NUMBER           105         //LINE No (FOR COMMS)                   105
#define BATCH_CALIBRATION_COMPONENTS            106         //  # OF BLENDS                         106
#define BATCH_CALIBRATION_FILL_RETRIES      107         // NO OF FILL RETRY(00 = ALWAYS)         107
#define BATCH_CALIBRATION_BLENDER_MODE      108         // BLENDER MODE                          108
#define BATCH_CALIBRATION_CONTROL_MODE          109     // CONTROL MODE

#define EXTRUDER_MODE           0
#define HAULOFF_MODE                1
// 0 = Extruder
// 1 = Haul-Off

#define BATCH_CALIBRATION_REGRIND_COMPONENT     110     // REGRIND COMPONENT NO.                 110
#define BATCH_CALIBRATION_FAST_CONTROL          111     // FAST CONTROL RESPONSE                 111  0 = yes, 1 = No
#define BATCH_CALIBRATION_DIAG_MODE             112     // MODBUS DIAGNOSTICS.                   112
// Diagnostic modes
#define BATCH_DIAG_NONE                 0
#define BATCH_DIAG_AUTOCYCLE                1
#define BATCH_DIAG_4_COMP_CYCLE         2
#define BATCH_DIAG_OUTPUT_CYCLE         3
#define BATCH_DIAG_DUAL_CYCLE           4
#define BATCH_DIAG_DUAL_SLOW_CYCLE      5
#define BATCH_DIAG_SINGLE_VALVE_CYCLE   6

#define BATCH_CALIBRATION_PIB_ENABLE            113     // AVEC PIB (PIB ENABLED)                113
#define BATCH_CALIBRATION_OPTIONS               114     // MBMACOPTIONS  DS 2       ;                                       114

#define BATCH_CAL_OPTIONS_CORRECTEVERYCYCLEBITPOS   0x01    //CORRECTEVERYCYCLEBITPOS EQU   %0000000000000001



#define BATCH_CALIBRATION_VOLUMETRIC_MODE       115     // VOLUMETRIC MODE.                      115
#define BATCH_CALIBRATION_SEI_INSTALLED         116     // asm MBEIOMITTED   DS 2       ; EI IS OMITTED  .                      116
 // Bit allocations for the various modules
#define BATCH_SEI_INSTALLED_MASK        0x0001
#define BATCH_LLSAM_INSTALLED_MASK  0x0002
#define BATCH_SEI_CLEAR_MASK            0xfffe
#define BATCH_LLSAM_CLEAR_MASK      0xfffd

//#define BATCH_CALIBRATION_VAC_LOADING               117 // VACUUM LOADING ON  0 = yes, 1 = No                    117
#define CALIBRATION_MACHINE_TYPE                117 //	wfh - 23.4.2020
#define MACHINE_TYPE_BATCH_BLENDER     						(0)
#define MACHINE_TYPE_BATCH_BLENDER_B         				(1)	// RESERVE for future
#define MACHINE_TYPE_BATCH_BLENDER_C         				(2)	// RESERVE for future
#define MACHINE_TYPE_BATCH_BLENDER_INTEGRATED_LOADING		(3)
#define MACHINE_TYPE_BATCH_BLENDER_LIW						(4)
#define MACHINE_TYPE_BATCH_BLENDER_INTEGRATED_LOADING_LIW 	(5)
#define MACHINE_TYPE_VACUUM_LOADING_ONLY_1_PUMP				(6)
#define MACHINE_TYPE_VACUUM_LOADING_ONLY_MULTIPLE_PUMPS		(7)
#define MACHINE_TYPE_LIW_SINGLE_COMPONENT					(8)
#define MACHINE_TYPE_LIW_SIGMABLEND							(9)
#define MACHINE_TYPE_LIW_XG									(10)
#define MACHINE_TYPE_WIDTH_CONTROL							(11)
#define LAST_MACHINE_TYPE				(MACHINE_TYPE_WIDTH_CONTROL)

#define BATCH_CALIBRATION_BLENDER_TYPE          118
//
#define BATCH_BLENDER_TYPE_STANDARD     0
#define BATCH_BLENDER_TYPE_MINI         1
#define BATCH_BLENDER_TYPE_MEGA         2
#define BATCH_BLENDER_TYPE_EXTENDED     3

#define BATCH_CALIBRATION_EXTR_SHUTDOWN         119     // EXTRUSION SHUTDOWN 1= ON.             119
#define BATCH_CALIBRATION_SHUTDOWN_TIME         120     // EXTRUSION SHUTDOWN TIME.              120
#define BATCH_CALIBRATION_MEASUREMENT_UNITS     121     // UNITS TYPE (METRIC(0)IMP(1))          121
#define BATCH_CALIBRATION_SPEED_SIGNAL          122     // SPEED SIGNAL TYPE TACHO=0/COMMs=1    122
// Tacho feedback signal types
#define BATCH_SPEED_SIGNAL_TACHO        0
#define BATCH_SPEED_SIGNAL_COMMS        1

#define BATCH_CALIBRATION_STALL_CHECK               123     // EXTR. STALL CHECK ENABLE FLAG         123
#define BATCH_CALIBRATION_DELAY_TO_LS_ALARM     124     // DELAY TO LEVEL SENSOR ALARM           124

#define BATCH_CALIBRATION_OFFLINE_TYPE          125     // OFFLINE TYPE                          125
#define BATCH_CALIBRATION_CONTROL_BAND          126     // CONTROL PERCENTAGE LIMIT              126

#define BATCH_CALIBRATION_NETWORK_BAUD_RATE         127     // baud rate selection
#define  BATCH_CALIBRATION_LINE_SPEED_SIGNAL_TYPE   128    //
#define BATCH_CALIBRATION_CONFIG_3                  129     // UNUSED.

// Component configuration.
#define BATCH_CALIBRATION_COMP_CONFIG               130
// Component Configuration types
#define BATCH_STANDARD_LAYOUT           0
#define BATCH_NONSTANDARD_LAYOUT        1

#define BATCH_CALIBRATION_COMP1_CONFIG          131     // COMPONENT #1 CONGIGURATION   131 Bits 0 to 7 represent the output controlling
#define BATCH_CALIBRATION_COMP2_CONFIG          132         // each component. ls bit = Bin 1
#define BATCH_CALIBRATION_COMP3_CONFIG          133
#define BATCH_CALIBRATION_COMP4_CONFIG          134
#define BATCH_CALIBRATION_COMP5_CONFIG          135
#define BATCH_CALIBRATION_COMP6_CONFIG          136
#define BATCH_CALIBRATION_COMP7_CONFIG          137
#define BATCH_CALIBRATION_COMP8_CONFIG          138
#define BATCH_CALIBRATION_COMP9_CONFIG          139
#define BATCH_CALIBRATION_COMP10_CONFIG         140
#define BATCH_CALIBRATION_COMP11_CONFIG         141
#define BATCH_CALIBRATION_COMP12_CONFIG         142

//       WEIGHT CALIBRATION DATA.
#define BATCH_CALIBRATION_CALIB_WEIGHT          145     // Kg with 3 dp's CAILBRATED WEIGHT 65.535 KGS          145
#define BATCH_CALIBRATION_UNUSED6                   146     // reserve for future -
#define BATCH_CALIBRATION_BATCH_SIZE                147     // Kg with 1 dp
#define BATCH_CALIBRATION_MAX_THROUGHPUT            148     // MAX TPT FOR #1 (INTEGER Kg)           148

#define BATCH_CALIBRATION_CODE                      149     //CALIB CODE (B0=HOP Wt/B1=tare)        149
// masks for the calibration code
#define BATCH_CALIBRATE_HOPPER          0x0001
#define BATCH_TARE_HOPPER                   0x0002
// bits 3 to 15 are unused

#define BATCH_CALIBRATION_TARE_COUNTS               150     // Long TARE COUNTS FOR BLEND 1 (LONG INT.)   150/1    ; NOTE READ ONLY.
#define BATCH_CALIBRATION_WEIGHT_CONSTANT       152     // Long


#define BATCH_CALIBRATION_PRINT_OPTIONS         154     // PRINTING OPTIONS
// Printing Option Masks
#define BATCH_PRINT_DIAGNOSTICS_BIT       0x0001 //DIAGNOSTICS PRNTINGBIT
#define BATCH_PRINT_MONTHLY_BIT             0x0002 //MONTHLY PRINTING   BIT
#define BATCH_PRINT_HOURLY_BIT              0x0004 //HOURLY PRINTING    BIT
#define BATCH_PRINT_SHIFT_BIT             0x0008 //SHIFT PRINTING     BIT
#define BATCH_PRINT_ALARM_BIT               0x0010 //ALARM PRINTING     BIT
#define BATCH_PRINT_ORDER_BIT             0x0020 //ORDER PRINTING     BIT
#define BATCH_PRINT_ROLL_BIT            0x0040 //ROLL PRINTING      BIT
#define BATCH_PRINT_SER_PAR_BIT           0x0080 //SERIAL / PARALLEL  BIT
#define BATCH_PRINT_SCREEN_BIT              0x0100 //SCREEN PRINT       BIT

/*
DIAGPENABBPOS           EQU     %0000000000000001       ;       DIAGNOSTICS PRNTING
MTHPENABBITPOS          EQU     %0000000000000010       ;       MONTHLY PRINTING
HRPENABBITPOS           EQU     %0000000000000100       ;       HOURLY PRINTING
SHFPENABBITPOS          EQU     %0000000000001000       ;       SHIFT PRINTING
ALPENABBITPOS           EQU     %0000000000010000       ;       ALARM PRINTING
ORPENABBITPOS           EQU     %0000000000100000       ;       ORDER PRINTING
RRPENABBITPOS           EQU     %0000000001000000       ;       ROLL PRINTING
SERPTROPBITPOS          EQU     %0000000010000000       ;       SERIAL / PARALLEL
SCPENABBITPOS           EQU     %0000000100000000       ;       SCREEN PRINT

*/

#define BATCH_CALIBRATION_CONFIG_OPTIONS            155     // CONFIGURATION OPTIONS.
// Congig Option Masks
#define BATCH_CONFIG_HILO_REG_BIT                       0x0001 //HI-LO REGRIND BIT
#define BATCH_CONFIG_MAX_COMP_PER_EN_BIT                0x0002 //Max Component % enableD BIT
#define BATCH_CONFIG_FIRST_COMP_COMPENSATION_BIT    0x0004 //1st component compensation BIT
#define BATCH_CONFIG_LAYERING_BIT                       0x0008 //Layering BIT
#define BATCH_CONFIG_MIXER_DETECT_BIT                   0x0010 //MIXER DETECT BIT
#define BATCH_CONFIG_DOUBLE_DUMP_BIT                    0x0020 //DOUBLE_DUMP BIT
#define BATCH_CONFIG_REGRIND_RETRY_BIT                  0x0040 //REGRIND RETRY BIT
#define BATCH_CONFIG_LEV_SEN_MON_BIT                    0x0080 //Level sensor monitoring BIT

#define BATCH_CONFIG_BYPASS_EN_BIT                      0x0100 //bypass enable BIT
#define BATCH_CONFIG_FILL_MAN_SORT_BIT                  0x0200 //Fill manual sort BIT
#define BATCH_CONFIG_TOPUP_EN_BIT                       0x0400 //Top Up BIT
#define BATCH_CONFIG_BLOW_EVERY_CYCLE_BIT               0x0800 //Blow every cycle in clean BIT
#define BATCH_CONFIG_CLEAN_EN_BIT                       0x1000 //Clean enable BIT
#define BATCH_CONFIG_SINGLE_WORD_WRITE_BIT          0x2000 //Single word write BIT SINGLEWORDWRITEBITPOS   EQU     %0010000000000000       ;
#define BATCH_CONFIG_FAST_SLOW_CYCLE_BIT            0x4000 //FASTSLOWBITPOS          EQU     %0100000000000000       ;       FAST / SLOW BIT POS
#define BATCH_CONFIG_PURGE_BIT                      0x8000 //PURGEBITPOS             EQU     %1000000000000000       ;       PURGE BIT POS




#define BATCH_CALIBRATION_PURGE_COMPONENT       156     // PURGE COMPONENT NO.
#define     BATCH_CALIBRATION_CONTROL_TYPE          157     // Control type
#define BATCH_CALIBRATION_CONTROL_LAG               158     // //MBCONTROLLAG   CONTROL LAG FOR INC/DEC               158
#define BATCH_CALIBRATION_INC_RATE                  159     // MBINCREASERATE  INCREASE RATE. FOR INC/DEC
#define BATCH_CALIBRATION_DEC_RATE                  160     // MBDECREASERATE  RATE. FOR INC/DEC
#define BATCH_CALIBRATION_DEADBAND                  161     // MBDEADBAND      DS      2       ; DEADBAND.     161
#define BATCH_CALIBRATION_FINE_INC_DEC          162     // MBFINEINCDEC    DS      2       ; FINE INCREASE / DECREASE. 162

#define BATCH_CALIBRATION_MAX_PERCENT_1         163     // Maximum comp % for comp #1

#define BATCH_CALIBRATION_STAGE_FILL_EN         175 //MBSTAGEFILLEN   DS      2       ; STAGE FILL ENABLE    175
#define BATCH_CALIBRATION_MOULDING                  176 //MOULDING OPTION                     176
#define BATCH_CALIBRATION_ENTRY_TYPE            177 //MBENTRYTYPE     DS      2       ; DEFINE PERCENTAGE ENTRY TYPE          177

#define BATCH_CALIBRATION_OPTIONS_2                 178 //MBOPTIONS2      DS      2       ; MODBUS OPTIONS #2                     178

#define BATCH_CALIBRATION_USE_REGRIND_AS_REF_BITPOS 0x01// USEREGRINDASREFBITPOS   EQU     %0000000000000001 ; USE REGIND AS REFERENCE
#define BATCH_CALIBRATION_SEND_EMAIL_ON_ORDER_CHANGE_BITPOS 0x02//


#define BATCH_CALIBRATION_PHASE_1_ON_TIME           179 //CLEAN PHASE 1 ON TIME
#define BATCH_CALIBRATION_PHASE_1_OFF_TIME          180 //CLEAN PHASE 1 OFF TIME
#define BATCH_CALIBRATION_PHASE_2_ON_TIME           181 //CLEAN PHASE 2 ON TIME
#define BATCH_CALIBRATION_PHASE_2_OFF_TIME          182 //CLEAN PHASE 2 OFF TIME

#define BATCH_CALIBRATION_CLEAN_REPS                183 //BCLEANREPETITIONS DS    ; NO OF REPETITIONS IN BLENDER CL   183
#define BATCH_CALIBRATION_MIX_CLEAN_REPS            184 //MBMCLEANREPETITIONS DS      2       ; NO OF REPETITIONS IN MIXER CLEAN  184


#define  BATCH_CALIBRATION_BYPASS_TIME_LIMIT        185   // MBBYPASSTIMELIMIT   DS      2       ; BY PASS TIME LIMIT                185
#define  BATCH_CALIBRATION_TOPUP_PERCENT            186     // MBPERTOPUP          DS      2       ; % TOP UP.                         186
#define  BATCH_CALIBRATION_MAX_BATCH_SIZE_KGS       187 //MBMAXGRAMSPERPER    DS      2       ;                                   187
#define  BATCH_CALIBRATION_BYP_TIME_LIMIT_CLN       188 // MBBYPASSDCLNTIMELIMIT DS    2       ; BYPASS TIME LIMIT DURING CLEANING 188
#define  BATCH_CALIBRATION_TOPUP_COMP_REF           189 //  MBTOPUPCOMPNOREF DS      2       ; TOP UP COMPONENT NO REF              189
#define  BATCH_CALIBRATION_INT_MIX_TIME             190 //  MBINTMIXTIME    DS      2       ; ADDED INTERMITTENT MIXING TIME        190
#define  BATCH_CALIBRATION_PASSWORD_SUPERVISOR      191    //  MBPASWRD        DS      2       ; MANAGER PASSWORD                      191
#define  BATCH_CALIBRATION_PASSWORD_OPERATOR        192    //  MBPASWRD2       DS      2       ; OPERATOR PASSWORD                     192
#define  BATCH_CALIBRATION_MAXIMUM_THROUGHPUT       193    //  MBMAXTHRUPUT        DS      2       ; MAXIMUM THROUGHPUT                    193
#define  BATCH_CALIBRATION_OFFLINE_MIXING_TIME      194    //  MBOFFLINEMTIME      DS      2       ; OFFLINE MIXING TIME.                  194
#define  BATCH_CALIBRATION_MIXING_SHUTOFF_TIME      195    //  MBMIXINGSHUTOFF     DS      2       ; MIXING MOTOR SHUT OFF (MINS)          195
#define  BATCH_CALIBRATION_MATERIAL_TRANSPORT_DELAY 196    //  MBVACUUMDELAY       DS      2       ; VACUUM DELAY.                         196
#define  BATCH_CALIBRATION_POST_VACUUM_DELAY        197    // MBVACUUMPOSTDELAY   DS      2       ; VACUUM POST DELAY                     197
#define  BATCH_DEVIATION_ALARM_ENABLE               198     // MBCOMPONENTPERENABLE DS     2       ;  COMPONENT PERCENTAGE ALARM ENABLE    198
#define  BATCH_CALIBRATION_CLEANING                 199    // MBCLEANCONFIG
#define  BATCH_COMMUNICATIONS_NO_PARITY             200


#define  BATCH_CALIBRATION_LATENCY                  201    // not defined before hand.

#define  BATCH_OPTIMISATION_UNUSED                  202     // OPTIMISATION OPTION

#define  BATCH_OPTIMISATION_VOLUME_1                203    // volume for bin litres
#define  BATCH_OPTIMISATION_VOLUME_2                204    //
#define  BATCH_OPTIMISATION_VOLUME_3                205    //
#define  BATCH_OPTIMISATION_VOLUME_4                206    //
#define  BATCH_OPTIMISATION_VOLUME_5                207    //
#define  BATCH_OPTIMISATION_VOLUME_6                208    //
#define  BATCH_OPTIMISATION_VOLUME_7                209    //
#define  BATCH_OPTIMISATION_VOLUME_8                210    //
#define  BATCH_OPTIMISATION_VOLUME_9                211    //
#define  BATCH_OPTIMISATION_VOLUME_10               212    //
#define  BATCH_OPTIMISATION_VOLUME_11               213    //
#define  BATCH_OPTIMISATION_VOLUME_12               214    //
#define  BATCH_CALIBRATION_PURGE                    215    // PURGE OPTION
#define  BATCH_OPTIMISATION                         216     // OPTIMISATION OPTION



#define  BATCH_CALIBRATION_MAX_PERCENTAGE_DEVIATION_1 217
#define  BATCH_CALIBRATION_MAX_PERCENTAGE_DEVIATION_2 218
#define  BATCH_CALIBRATION_MAX_PERCENTAGE_DEVIATION_3 219
#define  BATCH_CALIBRATION_MAX_PERCENTAGE_DEVIATION_4 220
#define  BATCH_CALIBRATION_MAX_PERCENTAGE_DEVIATION_5 221
#define  BATCH_CALIBRATION_MAX_PERCENTAGE_DEVIATION_6 222
#define  BATCH_CALIBRATION_MAX_PERCENTAGE_DEVIATION_7 223
#define  BATCH_CALIBRATION_MAX_PERCENTAGE_DEVIATION_8 224
#define  BATCH_CALIBRATION_MAX_PERCENTAGE_DEVIATION_9 225
#define  BATCH_CALIBRATION_MAX_PERCENTAGE_DEVIATION_10 226
#define  BATCH_CALIBRATION_MAX_PERCENTAGE_DEVIATION_11 227
#define  BATCH_CALIBRATION_MAX_PERCENTAGE_DEVIATION_12 228

#define  BATCH_CALIBRATION_MAX_PAUSE_ON_ERROR_NO       229
#define  BATCH_CALIBRATION_COMPONENT_NAMING_ALPHANUMERIC 230
#define  BATCH_CALIBRATION_LINE_SPEED_AVERAGE       231    // Line speed average
#define  BATCH_PAUSE_BLENDER_ON_POWER_UP            232    // pause blender on power up
#define  BATCH_LOG_TO_SD_CARD                       233    // log to sd card
#define  BATCH_LOG_FILE_TYPE_CSV                    234    // log file type csv
#define  BATCH_CALIBRATION_ALLOW_CONTINUOUS_DOWNLOAD 235
#define  BATCH_CALIBRATION_TELNET                   236
#define  BATCH_CHECK_FOR_VALID_COMPONENT_PER_TARGET 237

#define  BATCH_FILLING_METHOD_COMPONENT_1           238
#define  BATCH_FILLING_METHOD_COMPONENT_2           239
#define  BATCH_FILLING_METHOD_COMPONENT_3           240
#define  BATCH_FILLING_METHOD_COMPONENT_4           241
#define  BATCH_FILLING_METHOD_COMPONENT_5           242
#define  BATCH_FILLING_METHOD_COMPONENT_6           243
#define  BATCH_FILLING_METHOD_COMPONENT_7           244
#define  BATCH_FILLING_METHOD_COMPONENT_8           245
#define  BATCH_FILLING_METHOD_COMPONENT_9           246
#define  BATCH_FILLING_METHOD_COMPONENT_10          247
#define  BATCH_FILLING_METHOD_COMPONENT_11          248
#define  BATCH_FILLING_METHOD_COMPONENT_12          249

#define  BATCH_CALIBRATION_PASSWORD_TSM             250
#define  BATCH_MIN_OPENING_TIME_CHECK               251
#define  BATCH_FLOW_RATE_CHECK                      252
#define  BATCH_VAC8_EXPANSION                       253

#define  BATCH_END_OF_CONFIG                        254    //



#define BATCH_START_SYSTEM_CONFIG2              BATCH_CALIBRATION_PRINT_OPTIONS
#define  BATCH_END_SYSTEM_CONFIG2                   BATCH_END_OF_CONFIG    // end of system config                     197

#define  BATCH_END_SYSTEM_CONFIG                    BATCH_CALIBRATION_LINE_SPEED_SIGNAL_TYPE    // end of system config                     197

// Liquid Calibration.
#define  BATCH_CALIBRATION_LA_CONSTANT              255     // FLOW RATE CONST FOR Liquid Additive. (PIB)        229
#define  BATCH_CALIBRATION_LA_CALIBRATION_KGHR      256     // Calibration for liquid additive
#define  BATCH_CALIBRATION_LA_LAG                   257     // FLOW RATE control lag FOR L.A. (PIB)        231
#define  BATCH_CALIBRATION_LA_GAIN                  258     // FLOW RATE CONTROL GAIN FOR L.A. (PIB)        232
#define  BATCH_CALIBRATION_LA_SIGNAL                259     // SIGNAL TYPE FOR LIQ. ADDIT. (PIB)     233
//                                                                          // (0/1/2/3 = NONE/PULSES/ANALOG/COMMS)
#define  BATCH_CALIBRATION_LA_MODE                  260     // MODE (0/1 = MEAS/MEAS & CTRL)         234
#define BATCH_CALIBRATION_LA_SCALING_FACTOR         261     // 0 - 1.000
#define  BATCH_CALIBRATION_LA_TARE                  263     // LONG (263/4) FLOW RATE tare FOR L.A. (PIB)               237/8

#define BATCH_CALIBRATION_LA_UNUSED3                265     // (265-268) reserve for future
// Define start and end of calibration data block in MB.
#define BATCH_CALIBRATION_START             BATCH_CALIBRATION_CONTROL_AVERAGE
//#define   BATCH_CALIBRATION_END               BATCH_CALIBRATION_UNUSED7

// Define start and end of System Config (options) data block in MB.
#define BATCH_OPTIONS_START             BATCH_CALIBRATION_COMPONENTS
//#define   BATCH_OPTIONS_END                   BATCH_CALIBRATION_COMP_CONFIG

/*****************************************************************
*                TOGGLE STATUS COMMANDS                          *
*****************************************************************/
#define MODBUS_FORCE_RESET_COMMAND					 267	// force a CPU reset when value = FORCE_RESET_VALUE
#define FORCE_RESET_VALUE							(11011)	//
#define FORCE_FAST_INFLATE							(11012) // for testing Fast Inflate o/p.

#define FORCE_USMUX_A0_LOW				(11014)
#define FORCE_USMUX_A0_HIGH				(11015)
#define FORCE_USMUX_CLK_LOW				(11016)
#define FORCE_USMUX_CLK_HIGH			(11017)
#define CLR_USMUX_TESTS					(11013)
// CONTROL
#define BATCH_DTOA_INCREASE_COMMAND                  268   //MBINCREASE          DS      2       ;INCREASE                    268
#define BATCH_DTOA_DECREASE_COMMAND                  269   // MBDECREASE          DS      2       ;DECREASE                    269

#define BATCH_CONTROL_OPEN_COMMAND                  270
#define BATCH_CONTROL_INC_DEC                       271             // INCR/DECREASE    [INC : DEC]          271
// Flags
#define BATCH_CONTROL_INCREASE      0x0f00
#define BATCH_CONTROL_DECREASE      0x000f

#define BATCH_CONTROL_AUTO_MANUAL                   272             // AUTO/MANUAL, TOGGLE                   272
// Masks
#define BATCH_TOGGLE_AM_MASK        0x0001 //TOGGLE A/M BIT
#define BATCH_OPEN_OFFLINE_MASK 0x0002 // OPEN OFFLINE BIT
#define BATCH_FRC_BIT_MASK          0x0004 // FRONT ROLL CHANGE BIT
#define BATCH_BRC_BIT_MASK          0x0008 // BACK ROLL CHANGE  BIT
#define BATCH_CLOSE_OFFLINE_MASK    0x0010 // CLOSE OFFLINE BIT
#define BATCH_OPEN_DUMP_MASK        0x0020 // Open Dump bit
#define BATCH_CLOSE_DUMP_MASK       0x0040 // Close Dump bit

#define BATCH_TOGGLE_PAUSE_MASK 0x0100 // Toggle Pause.
#define BATCH_PAUSE_OFF_MASK        0x0200 // PAUSE OFF
#define BATCH_AM_ON_MASK            0x0400 // A/M ON
#define BATCH_AM_OFF_MASK           0x0800 // AM OFF

#define BATCH_EOC_PAUSE_MASK        0x1000 // END OF CYCLE PAUSE ON
#define BATCH_IMM_PAUSE_MASK        0x2000 // IMMEDIATE PAUSE ON

#define BATCH_CONTROL_CLEAR_COMMAND             273
// masks for the clear flag
#define  BATCH_CLEAR_ALARMS               0x0001   // RESET ALARMS
#define  BATCH_CLEAR_KEY_PRESSED          0x0002   // CLEAR ENTER KEY
#define  BATCH_CLEAR_FRONT_FLAG           0x0004   // CLEAR FRONT RC BIT.
#define  BATCH_CLEAR_BACK_FLAG            0x0008   // CLEAR BACK RC BIT.
#define BATCH_CLEAR_ORDER_FLAG              0x0010  // CLEAR ORDER CHANGE
#define BATCH_CLEAR_RESET_FLAG              0x0020
#define  BATCH_CLEAR_SHIFT_TOTALS         0x0040
#define BATCH_CLEAR_COMMS_TOTALS            0x0080  // RESET COMMS COUNTERS.

#define BATCH_CLEAR_CLEAN_BITS              0x0100  // Reset the clean cycle bits.
#define BATCH_CALIBRATION_ZERO_LONG_TERM_WEIGHT_ACCUMULATOR 0x0200//
// bits 9 to 15 are unused

#define BATCH_CONTROL_CLOSE_COMMAND             274 //

#define BATCH_CONTROL_SCREW_CALIBRATION         275 // SCREW CALIB COMMAND      ***NOT USED IN BATCH***             275
// masks for the screw calibration flags
// bits 0 to 3 specify the component number
#define BATCH_SCREW_CALIBRATE_START         0x0010
#define BATCH_SCREW_CALIBRATE_SAVE              0x0020
#define BATCH_SCREW_CALIBRATE_ABORT         0x0040      // bits 6 to 15 are unused

#define BATCH_CONTROL_WRITE_DAC                     276         // MBSETD_A            DS     2       ; D/A WRITE VALUE FOR EXTR.            276

// the first 15 bits are the DAC setpoint
// the msb indicates that this is a new WRITE value

#define BATCH_NEW_DAC_VALUE_MASK        0x8000

#define BATCH_CONTROL_SETPOINT_FLAGS                277
// masks for the setpoint flags
#define BATCH_NEW_PERCENTAGES                   0x0001
#define BATCH_NEW_THROUGHPUTS                   0x0002
#define BATCH_NEW_DESCRIPTIONS              0x0004
#define BATCH_NEW_K_FACTORS                 0x0008
// Bits 4 to 15 are unused

#define BATCH_CONTROL_SCREW_SPEED                   278  //MBSSWRIT            DS     2       ; SCREW SPEED WRITE(DOWN LOAD ON COMMs) 278
#define BATCH_CONTROL_LINE_SPEED                    279  //MBLSWRIT            DS     2       ; LINE SPEED WRITE(DOWN LOAD ON COMMs)  279

/* define codes to activate various outputs */

#define TOGGLE_STATUS_COMMAND_OUTPUT_DUMP               5
#define TOGGLE_STATUS_COMMAND_OUTPUT_OFFLINE            6
#define TOGGLE_STATUS_COMMAND_OUTPUT_BYPASS             7
#define TOGGLE_STATUS_COMMAND_OUTPUT_TOP_CLEAN          8
#define TOGGLE_STATUS_COMMAND_OUTPUT_LEFT_CORNER        9
#define TOGGLE_STATUS_COMMAND_OUTPUT_RIGHT_CORNER_CLEAN 10
#define TOGGLE_STATUS_COMMAND_OUTPUT_MIXER_CLEAN        11


#define BATCH_CONTROL_CLEAN                         280  //MBCLEAN             DS     2       ; CLEAN COMMAND                         280
#define BATCH_ENCRYPTION_WRITE_CODE                 281 // MBENCRYPTIONCODE    DS     4       ; MODBUS ENCRYPTION CODE.               281 / 282
#define BATCH_OUTPUT_DIAGNOSTICS                    283 // MBPRDIAG    DS     2       ; PRODUCTION DIAGNOSTIC OPEN CLOSE               283
#define BATCH_GENERAL_TOGGLE_STATUS_COMMANDS        284 //



#define BATCH_GENERAL_TOGGLE_STATUS_COMMANDS_RESET_TOTALS_BIT   0x01
#define BATCH_GENERAL_TOGGLE_STATUS_COMMANDS_SEND_ORDER_REPORT_EMAIL_BIT  0x02
#define BATCH_GENERAL_TOGGLE_STATUS_COMMANDS_SEND_SYSTEM_CONFIG_EMAIL_BIT 0x04

#define BATCH_START_TOGGLE_STATUS_COMMANDS          BATCH_DTOA_INCREASE_COMMAND
#define BATCH_END_TOGGLE_STATUS_COMMANDS            BATCH_GENERAL_TOGGLE_STATUS_COMMANDS // define end of toggle status commands.


#define BATCH_DTOA_PERCENTAGE                           300 // MBDTOAPER       DS      2      ; D TO A PERCENTAGE                  ;300
#define BATCH_RESET_FLOW_RATES                          301 // MBRESETFLOWRATE DS      2      ; RESET FLOW RATES                   ; 301
#define BATCH_TOGGLE_STATUS_TARE_SCREW_SPEED            302 // MBTARESCREWSPEED DS     2      ; TARE SCREW SPEED                   ; 302
#define BATCH_TOGGLE_STATUS_CALIBRATE_SCREW_SPEED       303 // MBCALSCEWSPEED  DS      2      ; CALIBRATE SCREW SPEED.             ; 303

#define BATCH_SOFTWARE_UPDATE_FROM_SD_CARD              304 //
#define BATCH_SOFTWARE_COPY_CONFIG_TO_SD_CARD           305 //
#define BATCH_SOFTWARE_COPY_CONFIG_TO_BACKUP            306 //
#define BATCH_SOFTWARE_LOAD_DEFAULT_CONFIG_FROM_SD_CARD 307 //
#define BATCH_SOFTWARE_LOAD_BACKUP_CONFIG_FROM_SD_CARD  308 //

#define TOGGLE_STATUS_COMMAND2_WRITE_RECIPE_NO         310
#define TOGGLE_STATUS_COMMAND2_WRITE_EXTRUDER_DATA     311
#define TOGGLE_STATUS_COMMAND2_CLEAR_DOWNLOAD_HISTORY  312
#define TOGGLE_STATUS_COMMAND2_PAUSE_MULTIBLEND        313
#define TOGGLE_STATUS_COMMAND2_RESTART_MULTIBLEND      314
#define TOGGLE_STATUS_COMMAND2_SET_NBB_NETWORK_SETTINGS 315
#define TOGGLE_STATUS_COMMAND2_VIBRATE_CATCH_BOX        316
#define TOGGLE_STATUS_COMMAND2_OPERATOR_PANEL_PAGE_ID   317
#define TOGGLE_STATUS_COMMAND2_SAFELY_REMOVE_SD_CARD    318
#define TOGGLE_STATUS_COMMAND2_PURGE                    320     // purge blender
#define TOGGLE_STATUS_COMMAND2_LENGTH                   321    // double length in metres


#define BATCH_SOFTWARE_UPDATE_FROM_USB                  323 //
#define BATCH_SOFTWARE_COPY_CONFIG_TO_USB               324 //
#define BATCH_SOFTWARE_COPY_CONFIG_TO_BACKUP_USB        325 //
#define BATCH_SOFTWARE_LOAD_DEFAULT_CONFIG_FROM_USB     326 //
#define BATCH_SOFTWARE_COPY_LOGS_TO_USB                 327 //

#define BATCH_TOGGLE_STATUS_RESET_LONG_TERM_COUNTERS    329 //
#define BATCH_TOGGLE_STATUS_RESET_SAMPLE_COUNTER        330 //

#define TOGGLE_STATUS_COMMAND2_LIQUID_ADDITIVE_COMMAND_INITIATE     331
#define TOGGLE_STATUS_COMMAND2_LIQUID_ADDITIVE_COMMAND_SET_DTOA     332
#define TOGGLE_STATUS_COMMAND2_LIQUID_ADDITIVE_COMMAND_SET_DTOA_PERCENTAGE  333
#define TOGGLE_STATUS_TEST_FUNCTIONALITY             334
#define TOGGLE_STATUS_COMMAND_INITIATION             335

#define BATCH_START_TOGGLE_STATUS_COMMANDS2          BATCH_DTOA_PERCENTAGE
#define BATCH_END_TOGGLE_STATUS_COMMANDS2            340                    // define end of toggle status commands.



// masks for the clean flag
#define  MB_START_NORMAL_CLEAN_BIT               0x0001   // Start normal clean

/*****************************************************************
*                       REAL TIME                                *
*****************************************************************/
#define BATCH_TIME_MIN_SEC                      285 // TIME [MIN:SEC]                        285
#define BATCH_TIME_DAY_HOUR                     286 // TIME [DAY:HOUR]                       286
#define BATCH_TIME_MONTH_DATE                   287 // TIME [MON:DATE]                       287
#define BATCH_TIME_YEAR                         288 // TIME [YEAR - 4 DIGITS, 2004]          288
#define BATCH_TIME_UNUSED1                      289 // UNUSED                               289
#define BATCH_TIME_START                BATCH_TIME_MIN_SEC
#define BATCH_TIME_END                  BATCH_TIME_YEAR

#define BATCH_TEMPERATURE                       290  // degrees

// system alarms split into one alarm per word

#define BATCH_SUMMARY_PROCESS_ANY_ALARM_OCCURRED              367
#define BATCH_SUMMARY_SYSTEM_LICENSE_ALARM                    368
#define BATCH_SUMMARY_SYSTEM_SOFTWARE_LICENSE_ALARM           369
#define BATCH_SUMMARY_SYSTEM_HARDWARE_LICENSE_ALARM           370
#define BATCH_SUMMARY_SYSTEM_UNUSED1                          371
#define BATCH_SUMMARY_SYSTEM_UNUSED2                          372
#define BATCH_SUMMARY_SYSTEM_UNUSED3                          373
#define BATCH_SUMMARY_SYSTEM_UNUSED4                          374
#define BATCH_SUMMARY_SYSTEM_UNUSED5                          375
#define BATCH_SUMMARY_SYSTEM_UNUSED6                          376
#define BATCH_SUMMARY_SYSTEM_UNUSED7                          377
#define BATCH_SUMMARY_SYSTEM_UNUSED8                          378
#define BATCH_SUMMARY_SYSTEM_UNUSED9                          379
#define BATCH_SUMMARY_SYSTEM_UNUSED10                         380
#define BATCH_SUMMARY_SYSTEM_UNUSED11                         381
#define BATCH_SUMMARY_SYSTEM_UNUSED12                         382
#define BATCH_SUMMARY_SYSTEM_UNUSED13                         383

#define BATCH_SUMMARY_SYSTEM_INDIVIDUAL_WORDS_START           BATCH_SUMMARY_SYSTEM_LICENSE_ALARM

// process alarms split into one alarm per word

#define BATCH_SUMMARY_PROCESS_ALARM_MIXING_MOTOR_STOPPED      384
#define BATCH_SUMMARY_PROCESS_ALARM_ZERO_LINESPEED            385
#define BATCH_SUMMARY_PROCESS_ALARM_BLENDER_SHUTDOWN          386
#define BATCH_SUMMARY_PROCESS_ALARM_FILL_ALARM                387
#define BATCH_SUMMARY_PROCESS_BIN_LOW_LEVEL_ALARM             388
#define BATCH_SUMMARY_PROCESS_SEI_COMMS_ALARM                 389
#define BATCH_SUMMARY_PROCESS_UNUSED1                         390
#define BATCH_SUMMARY_PROCESS_UNUSED2                         391
#define BATCH_SUMMARY_PROCESS_EEPROM_WRITE_FAILURE            392
#define BATCH_SUMMARY_PROCESS_CALIBRATION_INVALID_ALARM       393
#define BATCH_SUMMARY_PROCESS_PERCENTAGES_NOT100_ALARM        394
#define BATCH_SUMMARY_PROCESS_HOPPER_LEAK_ALARM               395
#define BATCH_SUMMARY_PROCESS_EXTRUDER_STALL_ALARM            396
#define BATCH_SUMMARY_PROCESS_HOPPER_TARE_ALARM               397
#define BATCH_SUMMARY_PROCESS_STARVATION_ALARM                398
#define BATCH_SUMMARY_PROCESS_EXTRUSION_CONTROL_ALARM         399

#define BATCH_SUMMARY_PROCESS_INDIVIDUAL_WORDS_START          BATCH_SUMMARY_PROCESS_ALARM_MIXING_MOTOR_STOPPED


/*
MBSYSALARM1     DS  2        ; SYSTEM ALARMS   16                 368-383
MBSYSALARM2     DS  2
MBSYSALARM3     DS  2
MBSYSALARM4     DS  2
MBSYSALARM5     DS  2
MBSYSALARM6     DS  2
MBSYSALARM7     DS  2
MBSYSALARM8     DS  2
MBSYSALARM9     DS  2
MBSYSALARM10    DS  2
MBSYSALARM11    DS  2
MBSYSALARM12    DS  2
MBSYSALARM13    DS  2
MBSYSALARM14    DS  2
MBSYSALARM15    DS  2
MBSYSALARM16    DS  2

                             ; PROCESS ALARMS                     384-399
MBPROALARM1     DS  2
MBPROALARM2     DS  2
MBPROALARM3     DS  2
MBPROALARM4     DS  2
MBPROALARM5     DS  2
MBPROALARM6     DS  2
MBPROALARM7     DS  2
MBPROALARM8     DS  2
MBPROALARM9     DS  2
MBPROALARM10    DS  2
MBPROALARM11    DS  2
MBPROALARM12    DS  2
MBPROALARM13    DS  2
MBPROALARM14    DS  2
MBPROALARM15    DS  2
MBPROALARM16    DS  2

*/

/*****************************************************************
*                    PRODUCTION SUMMARY                          *
*****************************************************************/

// SUMMARY
#define  BATCH_SUMMARY_TOTAL_THROUGHPUT         400      // TOTAL Kg/H (2DP)(>655.35Kg/H)         400/1
#define  BATCH_SUMMARY_TOTAL_WEIGHT_LENGTH      402      // ACTUAL g/m (2DP)                      402/3
#define  BATCH_SUMMARY_TOTAL_WEIGHT_AREA        404      // ACTUAL g/m2 (2DP)                     404
#define  BATCH_SUMMARY_FRONT_ROLL_LENGTH        405      // FRONT ROLL LENGTH (m 65535m)          405
#define  BATCH_SUMMARY_BACK_ROLL_LENGTH         406      // BACK ROLL LENGTH (m)                  406
#define  BATCH_SUMMARY_ORDER_LENGTH             407      // ORDER LENGTH (m)                      407/8
#define  BATCH_SUMMARY_FRONT_ROLL_WEIGHT        409      // FRONT ROLL WEIGHT (Kg 65535)          409
#define  BATCH_SUMMARY_BACK_ROLL_WEIGHT         410      // BACK ROLL WT. (Kg)                    410
#define  BATCH_SUMMARY_FRONT_SLIT_WEIGHT        411      // FRONT SLIT WT. (KG)                   411
#define  BATCH_SUMMARY_BACK_SLIT_WEIGHT         412      // BACK SLIT WT. (Kg)                    412
#define  BATCH_SUMMARY_ORDER_WEIGHT             413      // ORDER WT. (Kg) (NO DP)                413/4
#define  BATCH_SUMMARY_FRONT_ROLL_COUNT         415      // FRONT ROLL COUNTER (INTEGER)          415
#define  BATCH_SUMMARY_BACK_ROLL_COUNT          416      // BACK ROLL COUNTER (INTEGER)           416
#define  BATCH_SUMMARY_LINE_SPEED               417      // LINE SPEED (m/min, 2DP 655.35)        417
#define  BATCH_SUMMARY_EXTRUDER_SPEED           418      // EXTRUDER RPM 2DP                      418
#define  BATCH_SUMMARY_EXTRUDER_AD              419      // EXTRUDER A/D                                419
#define  BATCH_SUMMARY_OPERATIONAL_STATUS       420      //                                       420

// Operat. status mask
#define BATCH_STATUS_HIGH_REGRIND_LEVEL_SENSOR  0x0001      // IN CALIBRATION MODE.
#define BATCH_STATUS_BLENDER_STOPPED            0x0002      //  BLENDER IS STOPPED
#define BATCH_STATUS_MIXER_ON                   0x0004      //  MIXER IS ON.
#define BATCH_STATUS_DUMP_FLAP_OPEN         0x0008      //  DUMP FLAP IS OPEN.
#define BATCH_STATUS_STARTUP_MODE               0x0010      //  START-UP MODE(extr. auto control) BIT
#define BATCH_STATUS_RESET_OCCURRED_MASK        0x0020      //  RESET BIT
#define BATCH_STATUS_FLOW_CONTROL_VALVE     0x0040      //B6 FIBRE - NEW  (OFFLINE VALVE)

#define BATCH_STATUS_AUTOMANUAL_MASK            0x0080      //  AUTO MANUAL  ON
#define BATCH_STATUS_PAUSE                      0x0100      //   PAUSE STATUS ON
#define BATCH_STATUS_LOW_LEVEL_SENSOR           0x0200      //  LOW LEVEL SENSOR. OFF
#define  BATCH_STATUS_HIGH_LEVEL_SENSOR      0x0400     //  HIGH LEVEL SENSOR OFF
#define  BATCH_STATUS_MAIN_LEVEL_SENSOR      0x0800         //  DUMP LEVEL SENSOR
#define  BATCH_STATUS_IN_CLEAN_MODE          0x1000      // CLEAN STARTED               12
#define  BATCH_STATUS_FINISHED_CLEAN_MODE    0x2000      // CLEAN ENDED                 13
#define  BATCH_STATUS_BYPASS_VALVE           0x4000      // BYPASS O/P STATUS           14
#define BATCH_STATUS_IN_CLEAN_2NDSTAGE       0x8000      //MBCSTATCLEANST2POS      EQU     %1000000000000000       ; CLEAN STATUS STAGE 1

// End of masks
/*
; BIT DEFINITIONS FOR MBOPSTS

MBSTATINCALBPOS         EQU     %0000000000000001       ; IN CALIBRATION MODE.
MBSTATSTOPPEDBPOS       EQU     %0000000000000010       ; BLENDER IS STOPPED
MBSTATMIXONBPOS         EQU     %0000000000000100       ; MIXER IS ON.
MBSTATDUMPSTATUSBPOS    EQU     %0000000000001000       ; DUMP FLAP IS OPEN.
MBBLENDERSTARTUPMODE    EQU     %0000000000010000       ; BLENDER START UP MODE.
MBSTATRESETBPOS         EQU     %0000000000100000       ; RESET BIT
MBCSTATOFFLINESTPOS     EQU     %0000000001000000       ; OFFLINE O/P STATUS
MBSTATAUTOBPOS          EQU     %0000000010000000       ; AUTO MANUAL  ON
MBSTATPAUSEBPOS         EQU     %0000000100000000       ; PAUSE STATUS ON
MBSTATLOWLSPOS          EQU     %0000001000000000       ; LOW LEVEL SENSOR. OFF
MBSTATHIGHLSPOS         EQU     %0000010000000000       ; HIGH LEVEL SENSOR OFF
MBSTATDUMPLSSTATUSBPOS  EQU     %0000100000000000       ; DUMP LEVEL SENSOR
MBSTATCLEANSTARTEDBPOS  EQU     %0001000000000000       ; CLEAN STARTED
MBSTATCLEANENDEDBPOS    EQU     %0010000000000000       ; CLEAN ENDED
MBCSTATBYPASSSTPOS      EQU     %0100000000000000       ; BYPASS O/P STATUS
MBCSTATCLEANST2POS      EQU     %1000000000000000       ; CLEAN STATUS STAGE 1


*/

#define BATCH_SUMMARY_NEW_SETPOINTS             421
// New setpoints masks
#define BATCH_SETPOINTS_CHANGED_MASK            0x0001
#define BATCH_CALIBRATION_CHANGED_MASK      0x0002
#define BATCH_K_VALUES_CHANGED_MASK         0x0004
#define BATCH_FRONT_ROLL_MASK                   0x0100
#define BATCH_BACK_ROLL_MASK                        0x0200
#define BATCH_ORDER_MASK                            0x0400
//Clean status bits.
#define BATCH_CLEAN_TOP_MASK                        0x1000
#define BATCH_CLEAN_BOTTOM_MASK                 0x2000
#define BATCH_CLEAN_LEFT_CORNER_MASK            0x4000
#define BATCH_CLEAN_RIGHT_CORNER_MASK           0x8000
// End of masks

#define BATCH_SUMMARY_WIDTH                         422         // WIDTH (mm)                            422
#define BATCH_SUMMARY_TRIMMED_WIDTH             423         // TRIMMED WIDTH (mm)                    423
#define BATCH_SUMMARY_SYSTEM_ALARMS             424         // SYSTEM ALARMS FLAG                    424
// System alarms mask
    // Currently unused
// End of masks

#define BATCH_SUMMARY_PROCESS_ALARMS                425         // PROCESS ALARM FLAG                    425
// Process alarms masks
#define BATCH_MIXER_MOTOR_MASK                  0x0001
#define BATCH_ZERO_LINESPEED_MASK               0x0002
#define BATCH_BLENDER_SHUTDOWN_MASK         0x0004
#define BATCH_FILL_MASK                         0x0008
#define BATCH_RESERVOIR_LOW_LEVEL_MASK      0x0010
#define BATCH_SEI_COMMS_MASK                        0x0020
#define BATCH_REACHED_TARGET_WEIGHT         0x0040
#define BATCH_BLENDING_PERCENTAGE_ERROR     0x0080

//#define   BATCH_EEPROM_WRITE_MASK                 0x0100
#define BATCH_LS_MONITOR_MASK                   0x0100
#define BATCH_INVALID_CALIBRATION_MASK      0x0200
#define BATCH_INVALID_SETPOINTS_MASK            0x0400
#define BATCH_WEIGHT_CHANGE_MASK                0x0800
#define BATCH_EXTRUDER_STALLED_MASK         0x1000  // SEI Reset  ?? SAME AS SHUTDOWN??
#define BATCH_HOPPER_NOT_EMPTY_MASK         0x2000
#define BATCH_SENSOR_BEFORE_FILL_MASK           0x4000
#define BATCH_EXTRUDER_CONTROL_MASK         0x8000
// End of masks

#define BATCH_SUMMARY_SHIFT_WEIGHT                  426     // TOTAL SHIFT WT.  no d.p.'s   kg
#define BATCH_SUMMARY_SHIFT_LENGTH                  428     // TOTAL SHIFT LT.      no d.p.'s   m

#define BATCH_SUMMARY_HOPPER_WEIGHT             430     // HOPPER WEIGHT 4 d.p.'s       kg

#define BATCH_SUMMARY_WEIGHT_ACCUMULATOR            432     // MBTOTWACCBAT    DS      2 ; TOTAL ACCUMULATED BATCH WEIGHT FOR  432
#define BATCH_SUMMARY_SCREW_SPEED_ATD               433     // MBSCREWSPEEDATD DS      2       ; READ INPUTS STATE             433


#define BATCH_SUMMARY_CYCLE_COUNTER             434     // CYCLE OR BATCH COUNTER
#define BATCH_SUMMARY_OPERATIONAL_STATUS_2      435     // MBOPSTS2

#define BATCH_STATUS_2_TARE_MASK                0x0001  // MBSTATTAREBPOS           EQU     %0000000000000001       ;
#define BATCH_STATUS_2_CLEAN_LAST_BATCH_MASK    0x0002  // MBSTATCLEANLASTBATCHBPOS EQU     %0000000000000010       ;  LAST BATCH
#define BATCH_STATUS_2_BLENDER_CLEAN_MASK       0x0004  // MBSTATBLENDERCLEANBPOS   EQU     %0000000000000010       ;  BLENDER CLEAN
#define BATCH_STATUS_2_MIXER_CLEAN_MASK         0x0008  // MBSTATMIXERCLEANBPOS     EQU     %0000000000000100       ;  MIXER CLEAN
#define BATCH_STATUS_2_RIGHT_CORNER_CLEAN_MASK  0x0010  // MBSTATRCCBPOS            EQU     %0000000000001000       ;  RIGHT CORNER CLEAN
#define BATCH_STATUS_2_LEFT_CORNER_CLEAN_MASK   0x0020  // MBSTATLCCBPOS            EQU     %0000000000010000       ;  LEFT CORNER CLEAN
#define BATCH_STATUS_2_CYCLE_INDICATE_MASK      0x0040  // MBSTATCYCLEINDICATE      EQU     %0000000001000000       ;  CYCLE STATUS IN PROGRESS
#define  BATCH_STATUS_2_CLEAN_PHASE_1_MASK      0x0080   // MBBLENDERCLEANBITPOS     EQU     %0000000010000000       ;  BLENDER CLEAN IN PROGRESS
#define  BATCH_STATUS_2_CLEAN_PHASE_2_MASK      0x0100   // MBMIXERCLEANBITPOS       EQU     %0000000100000000       ;  MIXER CLEAN in progress
#define  BATCH_STATUS_2_RECIPE_WAITING          0x0200   // MBRECIPEWAITINGBITPOS    EQU     %0000001000000000       ;  BLENDER WAITING FOR RECIPE
#define  BATCH_STATUS_2_BLENDER_IDLE            0x0400   // MBINHIBITBITPOS          EQU     %0000010000000000       ;  BLENDER IS IDLE


/*
MBSTATTAREBPOS           EQU     %0000000000000001       ;
MBSTATCLEANLASTBATCHBPOS EQU     %0000000000000010       ;  LAST BATCH
MBSTATBLENDERCLEANBPOS   EQU     %0000000000000100       ;  BLENDER CLEAN
MBSTATMIXERCLEANBPOS     EQU     %0000000000001000       ;  MIXER CLEAN
MBSTATRCCBPOS            EQU     %0000000000010000       ;  RIGHT CORNER CLEAN
MBSTATLCCBPOS            EQU     %0000000000100000       ;  LEFT CORNER CLEAN
MBSTATCYCLEINDICATE      EQU     %0000000001000000       ;  CYCLE STATUS IN PROGRESS
MBBLENDERCLEANBITPOS     EQU     %0000000010000000       ;  BLENDER CLEAN IN PROGRESS
MBMIXERCLEANBITPOS       EQU     %0000000100000000       ;  MIXER CLEAN

MCE12-15
MBRECIPEWAITINGBITPOS    EQU     %0000001000000000       ;  BLENDER WAITING FOR RECIPE
MBINHIBITBITPOS          EQU     %0000010000000000       ;  BLENDER IS IDLE


*/
/*  MODBUS PRODUCTION SUMMARY (COMPONENT DATA) */
// offsets into Production Summary component block of data.
#define PSUM_TARGET_WT_OFFSET       0
#define PSUM_PERCENTAGE_OFFSET      2
#define PSUM_BATCH_WT_OFFSET            3
#define PSUM_FROLL_WT_OFFSET            5
#define PSUM_BROLL_WT_OFFSET            7
#define PSUM_ORDER_WT_OFFSET            9
#define PSUM_DAC_OFFSET                 11
#define PSUM_WT_ADC_OFFSET              12
#define PSUM_SLIDE_OFFSET               14
#define PSUM_SCREW_RPM_OFFSET           15
#define PSUM_STATUS_OFFSET              16
#define PSUM_SET_KGH_OFFSET             17
#define PSUM_SHIFT_WT_OFFSET            19
#define PSUM_LOW_LEV_SENSOR_OFFSET      21

#define BATCH_SUMMARY_TARGET_WEIGHT_1               436     // COMPONENT SET WEIGHT. (KG) 436/437
#define BATCH_SUMMARY_PERCENTAGE_1                  438     // ACTUAL % FOR BLEND (1DP)            438 2 d.p.'s     %
#define BATCH_SUMMARY_BATCH_WEIGHT_1                439     // BATCH Wt. 4 d.p.'s       kg
#define BATCH_SUMMARY_FRONT_ROLL_WEIGHT_1       441     // 2 d.p.'s     kg
#define BATCH_SUMMARY_BACK_ROLL_WEIGHT_1            443     // 2 d.p.'s     kg
#define BATCH_SUMMARY_ORDER_WEIGHT_1                445     // 2 d.p.'s     kg
#define BATCH_SUMMARY_SCREW_DAC_1                   447
#define BATCH_SUMMARY_WEIGHT_ADC_1                  448
#define BATCH_SUMMARY_BLEND_SLIDE_1             450
#define BATCH_SUMMARY_BLEND_RPM_1                   451
#define BATCH_SUMMARY_BLEND_STATUS_1                452     // BLEND STATUS (INCLUDE ALARM BITS)        452
// Blend status masks
#define MBFILLBYWEIGHT_MASK                     0x0001      // Fill by wt. bit.
#define BATCH_HOPPER_FILL_MASK                  0x0008
#define BATCH_COMPONENT_LOW_LEVEL_MASK      0x0010
// end of masks
#define BATCH_SUMMARY_BLEND_SET_KGH_1               453     // BLEND SETPT (Kg/H to 2DP)                    453/4
#define BATCH_SUMMARY_SHIFT_WEIGHT_1                455     // SHIFT WT. FOR COMPONENT 2 d.p.'s     kg
#define BATCH_SUMMARY_LOW_LEVEL_SENSOR_1            457     // LOW LEVEL SENSOR STATUS.

#define  PSUM_BLOCK_1       BATCH_SUMMARY_TARGET_WEIGHT_1       // Prod. Summary block for component #1
#define  PSUM_BLOCK_SIZE   (BATCH_SUMMARY_LOW_LEVEL_SENSOR_1 + 1 - PSUM_BLOCK_1)

#define  PSUM_BLOCK_2           (PSUM_BLOCK_1 + (PSUM_BLOCK_SIZE * 1))  // Prod. Summary block for component #2
#define  PSUM_BLOCK_3           (PSUM_BLOCK_1 + (PSUM_BLOCK_SIZE * 2))  // Prod. Summary block for component #3
#define  PSUM_BLOCK_4           (PSUM_BLOCK_1 + (PSUM_BLOCK_SIZE * 3))  // Prod. Summary block for component #4
#define  PSUM_BLOCK_5           (PSUM_BLOCK_1 + (PSUM_BLOCK_SIZE * 4))  // Prod. Summary block for component #5
#define  PSUM_BLOCK_6           (PSUM_BLOCK_1 + (PSUM_BLOCK_SIZE * 5))  // Prod. Summary block for component #6
#define  PSUM_BLOCK_7           (PSUM_BLOCK_1 + (PSUM_BLOCK_SIZE * 6))  // Prod. Summary block for component #7
#define  PSUM_BLOCK_8           (PSUM_BLOCK_1 + (PSUM_BLOCK_SIZE * 7))  // Prod. Summary block for component #8

#define  PSUM_BLOCK_9           (PSUM_BLOCK_1 + (PSUM_BLOCK_SIZE * 8))  // Prod. Summary block for component #9
#define  PSUM_BLOCK_10          (PSUM_BLOCK_1 + (PSUM_BLOCK_SIZE * 9))  // Prod. Summary block for component #10

#define  PSUM_BLENDER_TYPE      656    //BLENDER TYPE FOR panel purposes
#define  BATCH             (0)
#define  LIW               (1)
#define  BATCH_PLUS_LIW    (2)

#define BATCH_SUMMARY_WEIGHT_ACCUMULATOR_DOUBLE      657         // SPARE for expansion only


#define BATCH_SUMMARY_START         BATCH_SUMMARY_TOTAL_THROUGHPUT
#define BATCH_SUMMARY_END               BATCH_SUMMARY_UNUSED_2

/*****************************************************************
*                    FRONT ROLL REPORT                           *
*****************************************************************/

// FRONT ROLL
#define FRREPT_UNUSED1                          660                     // Note, there are spares, unused at start of roll report.
#define FRREPT_COMPONENTS                   675                     //NO. OF BLENDS/COMPONENTS             675
#define FRREPT_WEIGHT_LENGTH_SET            676                     // WT/M SETPOINT (2DP)
#define FRREPT_WEIGHT_AREA_SET              677                     // WT/M2(AREA) SPT (g/m2 2DP)
#define  FRREPT_THROUGHPUT_SET              678                  // KG/H SETPOINT (1DP)
#define FRREPT_LINE_SPEED_SET               679                     //  LINE SPEED SETPOINT (1DP
#define  FRREPT_WIDTH_SET                   680                  // WIDTH SETPOINT (mm)
#define FRREPT_TRIMMED_WIDTH_SET            681                     //  TRIMMED WIDTH (mm)

#define  FRREPT_ROLL_WEIGHT                 682                     // ROLL WT. VARIABLE (Kg 2DP)           682/83
#define FRREPT_SLIT_WEIGHT                  684                     // SLIT ROLL WT. VAR. (Kg 2DP)
#define FRREPT_ORDER_WEIGHT                 686                     // ORDER WT. VAR. (Kg 2DP)
#define FRREPT_ROLL_LENGTH                  688                     // ROLL LENGTH VARIABLE (m )
#define FRREPT_ORDER_LENGTH                 689                     // ORDER LENGTH VAR. (m)
#define FRREPT_CUSTOMER_CODE                    691                     // CUSTOMER CODE
#define FRREPT_MATERIAL_CODE                    699                     // MATERIAL CODE
#define FRREPT_ORDER_NUMBER                 707                     // ORDER NO.
#define FRREPT_FRONT_ROLL_NO                    715                     // CURRENT FRONT ROLL NO.
#define FRREPT_BACK_ROLL_NO                 716                     // CURRENT BACK ROLL NO.
#define FRREPT_SHEET_TUBE                       717                     // SHEET(1)/TUBE(0) FLAG
#define FRREPT_SLIT_REELS                       718                     // NO. OF SLIT REELS [BACK:FRONT}
#define FRREPT_DRY_ADDITIVE                 719                     // % DRY ADDITIVE
#define FRREPT_TRIM_FLAG                        720                     // TRIM BEING FED BACK(1=YES)
#define FRREPT_START_TIME                       721
#define FRREPT_START_DATE                       722
#define FRREPT_END_TIME                     723
#define FRREPT_END_DATE                     724
#define FRREPT_LA_ENABLED                       725
#define FRREPT_UNUSED_2                     726
#define FRREPT_UNUSED_3                     727

#define FRREPT_PERCENTAGE_OFFSET    0
#define FRREPT_WEIGHT_OFFSET            1


#define FRREPT_PERCENTAGE_1                 728
#define FRREPT_WEIGHT_1                     729
#define FRREPT_BLOCK_1_UNUSED1              731
#define FRREPT_BLOCK_1_UNUSED2              732
#define FRREPT_BLOCK_1_UNUSED3              733
#define FRREPT_BLOCK_1_UNUSED4              734
#define FRREPT_BLOCK_1_UNUSED5              735

#define FRREPT_BLOCK_SIZE       (FRREPT_BLOCK_1_UNUSED5 - FRREPT_BLOCK_1 + 1)           // I.E. 8

#define FRREPT_BLOCK_1                  FRREPT_PERCENTAGE_1
#define FRREPT_BLOCK_2                  (FRREPT_BLOCK_1 + (FRREPT_BLOCK_SIZE * 1))      // ROLL REPORT BLOCK FOR COMP. #2
#define FRREPT_BLOCK_3                  (FRREPT_BLOCK_1 + (FRREPT_BLOCK_SIZE * 2))      // ROLL REPORT BLOCK FOR COMP. #3
#define FRREPT_BLOCK_4                  (FRREPT_BLOCK_1 + (FRREPT_BLOCK_SIZE * 3))      // ROLL REPORT BLOCK FOR COMP. #4
#define FRREPT_BLOCK_5                  (FRREPT_BLOCK_1 + (FRREPT_BLOCK_SIZE * 4))      // ROLL REPORT BLOCK FOR COMP. #5
#define FRREPT_BLOCK_6                  (FRREPT_BLOCK_1 + (FRREPT_BLOCK_SIZE * 5))      // ROLL REPORT BLOCK FOR COMP. #6
#define FRREPT_BLOCK_7                  (FRREPT_BLOCK_1 + (FRREPT_BLOCK_SIZE * 6))      // ROLL REPORT BLOCK FOR COMP. #7
#define FRREPT_BLOCK_8                  (FRREPT_BLOCK_1 + (FRREPT_BLOCK_SIZE * 7))      // ROLL REPORT BLOCK FOR COMP. #8
#define FRREPT_BLOCK_9                  (FRREPT_BLOCK_1 + (FRREPT_BLOCK_SIZE * 8))      // ROLL REPORT BLOCK FOR COMP. #9
#define FRREPT_BLOCK_10             (FRREPT_BLOCK_1 + (FRREPT_BLOCK_SIZE * 9))      // ROLL REPORT BLOCK FOR COMP. #10

#define FRREPT_UNUSED_8                     808     // UNUSED AT END.
#define FRREPT_UNUSED_9                     809     // UNUSED AT END.

#define FRREPT_START            FRREPT_BLEND_COUNT
#define FRREPT_END              FRREPT_UNUSED_8
#define FRREPT_SIZE             (FRREPT_END - FRREPT_START)

/*****************************************************************
*                      BACK ROLL REPORT                          *                  // 810
*****************************************************************/

// BACK ROLL
#define BRREPT_UNUSED1                          810                     // Note, there are spares, unused at start of roll report.
#define BRREPT_COMPONENTS                   825                     //NO. OF BLENDS/COMPONENTS             825
#define BRREPT_WEIGHT_LENGTH_SET            826                     // WT/M SETPOINT (2DP)
#define BRREPT_WEIGHT_AREA_SET              827                     // WT/M2(AREA) SPT (g/m2 2DP)
#define  BRREPT_THROUGHPUT_SET              828                  // KG/H SETPOINT (1DP)
#define BRREPT_LINE_SPEED_SET               829                     //  LINE SPEED SETPOINT (1DP
#define  BRREPT_WIDTH_SET                   830                  // WIDTH SETPOINT (mm)
#define BRREPT_TRIMMED_WIDTH_SET            831                     //  TRIMMED WIDTH (mm)

#define  BRREPT_ROLL_WEIGHT                 832                     // ROLL WT. VARIABLE (Kg 2DP)           832/33
#define BRREPT_SLIT_WEIGHT                  834                     // SLIT ROLL WT. VAR. (Kg 2DP)
#define BRREPT_ORDER_WEIGHT                 836                     // ORDER WT. VAR. (Kg 2DP)
#define BRREPT_ROLL_LENGTH                  838                     // ROLL LENGTH VARIABLE (m )
#define BRREPT_ORDER_LENGTH                 839                     // ORDER LENGTH VAR. (m)
#define BRREPT_CUSTOMER_CODE                    841                     // CUSTOMER CODE
#define BRREPT_MATERIAL_CODE                    849                     // MATERIAL CODE
#define BRREPT_ORDER_NUMBER                 857                     // ORDER NO.
#define BRREPT_FRONT_ROLL_NO                    865                     // CURRENT FRONT ROLL NO.
#define BRREPT_BACK_ROLL_NO                 866                     // CURRENT BACK ROLL NO.
#define BRREPT_SHEET_TUBE                       867                     // SHEET(1)/TUBE(0) FLAG
#define BRREPT_SLIT_REELS                       868                     // NO. OF SLIT REELS [BACK:FRONT}
#define BRREPT_DRY_ADDITIVE                 869                     // % DRY ADDITIVE
#define BRREPT_TRIM_FLAG                        870                     // TRIM BEING FED BACK(1=YES)
#define BRREPT_START_TIME                       871
#define BRREPT_START_DATE                       872
#define BRREPT_END_TIME                     873
#define BRREPT_END_DATE                     874
#define BRREPT_LA_ENABLED                       875
#define BRREPT_UNUSED_2                     876
#define BRREPT_UNUSED_3                     877

#define BRREPT_PERCENTAGE_OFFSET    0                   // % OFFSET IN COMPONENT BLOCK
#define BRREPT_WEIGHT_OFFSET            1                   // WEIGHT OFFSET IN COMPONENT BLOCK.


#define BRREPT_PERCENTAGE_1                 878
#define BRREPT_WEIGHT_1                     879
#define BRREPT_BLOCK_1_UNUSED1              881
#define BRREPT_BLOCK_1_UNUSED2              882
#define BRREPT_BLOCK_1_UNUSED3              883
#define BRREPT_BLOCK_1_UNUSED4              884
#define BRREPT_BLOCK_1_UNUSED5              885

#define BRREPT_BLOCK_SIZE       (BRREPT_BLOCK_1_UNUSED5 - BRREPT_BLOCK_1 + 1)           // I.E. 8

#define BRREPT_BLOCK_1                  BRREPT_PERCENTAGE_1
#define BRREPT_BLOCK_2                  (BRREPT_BLOCK_1 + (BRREPT_BLOCK_SIZE * 1))      // ROLL REPORT BLOCK FOR COMP. #2
#define BRREPT_BLOCK_3                  (BRREPT_BLOCK_1 + (BRREPT_BLOCK_SIZE * 2))      // ROLL REPORT BLOCK FOR COMP. #3
#define BRREPT_BLOCK_4                  (BRREPT_BLOCK_1 + (BRREPT_BLOCK_SIZE * 3))      // ROLL REPORT BLOCK FOR COMP. #4
#define BRREPT_BLOCK_5                  (BRREPT_BLOCK_1 + (BRREPT_BLOCK_SIZE * 4))      // ROLL REPORT BLOCK FOR COMP. #5
#define BRREPT_BLOCK_6                  (BRREPT_BLOCK_1 + (BRREPT_BLOCK_SIZE * 5))      // ROLL REPORT BLOCK FOR COMP. #6
#define BRREPT_BLOCK_7                  (BRREPT_BLOCK_1 + (BRREPT_BLOCK_SIZE * 6))      // ROLL REPORT BLOCK FOR COMP. #7
#define BRREPT_BLOCK_8                  (BRREPT_BLOCK_1 + (BRREPT_BLOCK_SIZE * 7))      // ROLL REPORT BLOCK FOR COMP. #8
#define BRREPT_BLOCK_9                  (BRREPT_BLOCK_1 + (BRREPT_BLOCK_SIZE * 8))      // ROLL REPORT BLOCK FOR COMP. #9
#define BRREPT_BLOCK_10             (BRREPT_BLOCK_1 + (BRREPT_BLOCK_SIZE * 9))      // ROLL REPORT BLOCK FOR COMP. #10

#define BRREPT_UNUSED_8                     958     // UNUSED AT END.
#define BRREPT_UNUSED_9                     959     // UNUSED AT END.

#define BRREPT_START            BRREPT_BLEND_COUNT
#define BRREPT_END              BRREPT_UNUSED_8
#define BRREPT_SIZE             (BRREPT_END - BRREPT_START)

/*****************************************************************
*                      ORDER REPORT                               *                 // 810
*****************************************************************/

// ORDER REPORT
// ORDER
#define ORDREPT_UNUSED1                         960                     // Note, there are spares, unused at start of roll report.
#define ORDREPT_COMPONENTS                  975                     //NO. OF BLENDS/COMPONENTS             975
#define ORDREPT_WEIGHT_LENGTH_SET               976                     // WT/M SETPOINT (2DP)
#define ORDREPT_WEIGHT_AREA_SET                 977                     // WT/M2(AREA) SPT (g/m2 2DP)
#define  ORDREPT_THROUGHPUT_SET                 978                  // KG/H SETPOINT (1DP)
#define ORDREPT_LINE_SPEED_SET                  979                     //  LINE SPEED SETPOINT (1DP
#define  ORDREPT_WIDTH_SET                      980                  // WIDTH SETPOINT (mm)
#define ORDREPT_TRIMMED_WIDTH_SET               981                     //  TRIMMED WIDTH (mm)

#define  ORDREPT_ROLL_WEIGHT                    982                     // ROLL WT. VARIABLE (Kg 2DP)           832/33
#define ORDREPT_SLIT_WEIGHT                     984                     // SLIT ROLL WT. VAR. (Kg 2DP)
#define ORDREPT_ORDER_WEIGHT                        986                     // ORDER WT. VAR. (Kg 2DP)
#define ORDREPT_ROLL_LENGTH                     988                     // ROLL LENGTH VARIABLE (m )
#define ORDREPT_ORDER_LENGTH                        989                     // ORDER LENGTH VAR. (m)
#define ORDREPT_CUSTOMER_CODE                   991                     // CUSTOMER CODE
#define ORDREPT_MATERIAL_CODE                   999                     // MATERIAL CODE
#define ORDREPT_ORDER_NUMBER                        1007                        // ORDER NO.
#define ORDREPT_FRONT_ROLL_NO                   1015                        // CURRENT FRONT ROLL NO.
#define ORDREPT_BACK_ROLL_NO                        1016                        // CURRENT BACK ROLL NO.
#define ORDREPT_SHEET_TUBE                      1017                        // SHEET(1)/TUBE(0) FLAG
#define ORDREPT_SLIT_REELS                      1018                        // NO. OF SLIT REELS [BACK:FRONT}
#define ORDREPT_DRY_ADDITIVE                        1019                        // % DRY ADDITIVE
#define ORDREPT_TRIM_FLAG                           1020                        // TRIM BEING FED BACK(1=YES)
#define ORDREPT_START_TIME                      1021
#define ORDREPT_START_DATE                      1022
#define ORDREPT_END_TIME                            1023
#define ORDREPT_END_DATE                            1024
#define ORDREPT_LA_ENABLED                      1025
#define ORDREPT_UNUSED_2                            1026
#define ORDREPT_UNUSED_3                            1027

#define ORDREPT_PERCENTAGE_OFFSET   0                   // % OFFSET IN COMPONENT BLOCK
#define ORDREPT_WEIGHT_OFFSET       1                   // WEIGHT OFFSET IN COMPONENT BLOCK.


#define ORDREPT_PERCENTAGE_1                        1028
#define ORDREPT_WEIGHT_1                            1029
#define ORDREPT_BLOCK_1_UNUSED1                 1031
#define ORDREPT_BLOCK_1_UNUSED2                 1032
#define ORDREPT_BLOCK_1_UNUSED3                 1033
#define ORDREPT_BLOCK_1_UNUSED4                 1034
#define ORDREPT_BLOCK_1_UNUSED5                 1035

#define ORDREPT_BLOCK_SIZE      (ORDREPT_BLOCK_1_UNUSED5 - ORDREPT_BLOCK_1 + 1)         // I.E. 8

#define ORDREPT_BLOCK_1                 ORDREPT_PERCENTAGE_1
#define ORDREPT_BLOCK_2                 (ORDREPT_BLOCK_1 + (ORDREPT_BLOCK_SIZE * 1))        // ROLL REPORT BLOCK FOR COMP. #2
#define ORDREPT_BLOCK_3                 (ORDREPT_BLOCK_1 + (ORDREPT_BLOCK_SIZE * 2))        // ROLL REPORT BLOCK FOR COMP. #3
#define ORDREPT_BLOCK_4                 (ORDREPT_BLOCK_1 + (ORDREPT_BLOCK_SIZE * 3))        // ROLL REPORT BLOCK FOR COMP. #4
#define ORDREPT_BLOCK_5                 (ORDREPT_BLOCK_1 + (ORDREPT_BLOCK_SIZE * 4))        // ROLL REPORT BLOCK FOR COMP. #5
#define ORDREPT_BLOCK_6                 (ORDREPT_BLOCK_1 + (ORDREPT_BLOCK_SIZE * 5))        // ROLL REPORT BLOCK FOR COMP. #6
#define ORDREPT_BLOCK_7                 (ORDREPT_BLOCK_1 + (ORDREPT_BLOCK_SIZE * 6))        // ROLL REPORT BLOCK FOR COMP. #7
#define ORDREPT_BLOCK_8                 (ORDREPT_BLOCK_1 + (ORDREPT_BLOCK_SIZE * 7))        // ROLL REPORT BLOCK FOR COMP. #8
#define ORDREPT_BLOCK_9                 (ORDREPT_BLOCK_1 + (ORDREPT_BLOCK_SIZE * 8))        // ROLL REPORT BLOCK FOR COMP. #9
#define ORDREPT_BLOCK_10                    (ORDREPT_BLOCK_1 + (ORDREPT_BLOCK_SIZE * 9))        // ROLL REPORT BLOCK FOR COMP. #10
// Nb: Could add in blocks for components 11 & 12 because end of table.
// #define  ORDREPT_BLOCK_11                    (ORDREPT_BLOCK_1 + (ORDREPT_BLOCK_SIZE * 10))       // ROLL REPORT BLOCK FOR COMP. #11
// #define  ORDREPT_BLOCK_12                    (ORDREPT_BLOCK_1 + (ORDREPT_BLOCK_SIZE * 11))       // ROLL REPORT BLOCK FOR COMP. #12

#define ORDREPT_UNUSED_8                            1108        // UNUSED AT END.
#define ORDREPT_UNUSED_9                            1109        // UNUSED AT END.

#define ORDREPT_START           ORDREPT_BLEND_COUNT
#define ORDREPT_END             ORDREPT_UNUSED_8
#define ORDREPT_SIZE            (ORDREPT_END - ORDREPT_START)


// NEWLY ADDED - Oct 2004.
// MODBUS DIAGNOSTIC DATA.
#define BATCH_POWER_UP_TIME                     1200        // MBPOWERUPTIME
#define BATCH_POWER_UP_DATE                     1201        // MBPOWERUPTIME

#define  BATCH_VERSION_NO                       1202        // SOFTWARE VERSION ETC. (MBVERNUM)
#define  BATCH_CAL_MODULE_NO                    1207        // MBCALMODSERNO   CALIBRATION MODULE NO.
#define  BATCH_SBB_MODULE_NO                    1208        // MBSBBSERNO      DS      4       ; SBB SERIAL NO.

#define BATCH_TX_CTR_1                          1210    //MBGOODTXCTR1    DS      4       ; GOOD TX COUNTER MODULE 1      1210
#define BATCH_TX_CTR_2                          1212    //MBGOODTXCTR2    DS      4       ; GOOD TX COUNTER MODULE 2      1212
#define BATCH_TX_CTR_3                          1214    //MBGOODTXCTR3    DS      4       ; GOOD TX COUNTER MODULE 3      1214
#define BATCH_TX_CTR_4                          1216    //MBGOODTXCTR4    DS      4       ; GOOD TX COUNTER MODULE 3      1216

#define BATCH_RX_CTR_1                          1218    // MBGOODRXCTR1    DS      4       ; GOOD RX COUNTER MODULE 1      1218
#define BATCH_RX_CTR_2                          1220    // MBGOODRXCTR2    DS      4       ; GOOD RX COUNTER MODULE 2      1220
#define BATCH_RX_CTR_3                          1222    // MBGOODRXCTR3    DS      4       ; GOOD RX COUNTER MODULE 3      1222
#define BATCH_RX_CTR_4                          1224    // MBGOODRXCTR4    DS      4       ; GOOD RX COUNTER MODULE 4      1224

#define BATCH_CSUM_ERR_CTR_1                    1226    // MBCHECKSUMERR1  DS      4       ; CHECKSUM ERROR COUNTER #1     1226
#define BATCH_CSUM_ERR_CTR_2                    1228    //MBCHECKSUMERR2  DS      4       ; CHECKSUM ERROR COUNTER #2     1228
#define BATCH_CSUM_ERR_CTR_3                    1230    //MBCHECKSUMERR3  DS      4       ; CHECKSUM ERROR COUNTER #3     1230
#define BATCH_CSUM_ERR_CTR_4                    1232 // MBCHECKSUMERR4  DS      4       ; CHECKSUM ERROR COUNTER #4     1232

#define BATCH_TOUT_CTR_1                        1234    // MBTIMEOUTCTR1   DS      4       ; SERIAL TIMEOUT COUNTER #1     1234
#define BATCH_TOUT_CTR_2                        1236    // MBTIMEOUTCTR2   DS      4       ; SERIAL TIMEOUT COUNTER.#2     1236
#define BATCH_TOUT_CTR_3                        1238    // MBTIMEOUTCTR3   DS      4       ; SERIAL TIMEOUT COUNTER.#3     1238
#define BATCH_TOUT_CTR_4                        1240    // MBTIMEOUTCTR4   DS      4       ; SERIAL TIMEOUT COUNTER.#4     1240

#define BATCH_SBB_ADDRESS                       1242    // MBADDRESS       DS      2       ; MODBUS ADDRESS                1242
#define BATCH_DAC_KGH                           1243    // MBDACPKGHR      DS      2       ; D/A PER KG.                   1243
#define BATCH_DAC_MPM                           1244    // MBDACPMPM       DS      2       ; D/A PER MPM                   1244
#define BATCH_DIAG_OPTIONS                      1245    // MBDIAGOPTIONS   DS      2       ; LINK AND OTHER OPTIONS        1245
// link l1
#define BATCH_DIAG_LINK_L1                      1249
// link l2
#define BATCH_DIAG_LINK_L2                      1250
// link l3
#define BATCH_DIAG_LINK_L3                      1251




#define BATCH_DIAG_LINKL2BITPOS          0X01    // MBLINKL2BPOS    EQU     %0000000000000001       ;
#define BATCH_DIAG_LINKL3BITPOS          0X02    // MBLINKL3BPOS    EQU     %0000000000000010       ;
#define BATCH_DIAG_LINKL4BITPOS          0X02    // MBLINKL4BPOS    EQU     %0000000000000100       ;
#define BATCH_DIAG_LINKL5BITPOS          0X02    // MBLINKL5BPOS    EQU     %0000000000001000       ;

// MODBUS ENCRYPTION DATA.

#define BATCH_SEI_INPUTS                        1246    // MBSEIINPUTSTATE DS      2       ; SEI INPUTS                    1246
#define BATCH_SEI_ALARMS                        1247    // MBSEIALARMSTATE DS      2       ; SEI ALARM STATE               1247


#define BATCH_ENCRYPTION_OPTIONS                    1248 // MBOPTIONCODE        DS      2               ; OPTION CODE       1248
#define BATCH_ENCRYPTION_OPTIONS_2                  1249 // MBOPTIONCODE        DS      2               ; OPTION CODE       1248

// -review-- if these hour counters are necessary

//#define BATCH_ENCRYPTION_KGHR_COUNTER               1249 // unused
//#define BATCH_ENCRYPTION_GPM_COUNTER                1250 // unused
//#define BATCH_ENCRYPTION_MODBUS_HOUR_COUNTER        1251 // unused
#define BATCH_ENCRYPTION_BLENDERRUN_HOUR_COUNTER    1252 // MBBLENDERRUNHRCTR   DS      2       ;                           1252
#define BATCH_ENCRYPTION_REPORT_HOUR_COUNTER        1253 // unused
#define BATCH_ENCRYPTION_LLS_HOUR_COUNTER           1254 // unused
#define BATCH_ENCRYPTION_SOFTWAREID                 1255 // MBSOFTWAREID        DS      2       ; SOFTWARE ID               1255
#define BATCH_DIAG_LLS_INPUT_STATUS                 1256 // MBLLSINPUTSTATE     DS      2       ; LOW LEVEL SENSOR CARD     1256
#define BATCH_SINGLE_RECIPE_VACUUM_STATUS           1257 // MBVACUUMSTATUS      DS      2       ; VACUUMING STATUS          1257
#define BATCH_SINGLE_RECIPE_CURRENT_EXTRUDER        1258 // MBCURRENTEXTRUDERNO DS      2       ; CURRENT EXTRUDER NO.      1258
#define BATCH_SINGLE_RECIPE_VACUUM_INFO             1259 // MBVACUUMINFO        DS      2       ; VACUUM INFORMATION.       1259
#define BATCH_SINGLE_RECIPE_VACUUM_SEQUENCE         1260 // MBVACUUMSEQUENCE    DS      2       ;                           1260
#define BATCH_SINGLE_RECIPE_RECIPE_PENDING          1261 // MBPENDINGEXTRUDERNO DS      2       ; VACUUM INFORMATION.       1261
#define BATCH_EXTRUDER_SET_PERCENTAGE               1262 // MBEXTRUDERSETPERCENTAGE DS      2   ;                             1262

#define BATCH_POWER_UP_TIME_HOUR_SW                 1263 // RESET TIME
#define BATCH_POWER_UP_TIME_MIN_SW                  1264 //
#define BATCH_POWER_UP_TIME_DATE_SW                 1265 //
#define BATCH_POWER_UP_TIME_MONTH_SW                1266 //
#define BATCH_POWER_UP_RESET_COUNTER                1267 //
#define BATCH_VACUUM_ON                             1268 //

#define NETBURNER_CONFIG_IPADDRESS                  1270
#define NETBURNER_CONFIG_IPMASK                     1274
#define NETBURNER_CONFIG_IPGATEWAY                  1278
#define NETBURNER_CONFIG_IPDNS                      1282
#define NETBURNER_CONFIG_MACADDRESS                 1286
#define NETBURNER_DHCP_STATUS                       1292

#define BATCH_NETWORK_TX_CTR                        1293
#define BATCH_NETWORK_RX_CTR                        1295
#define BATCH_NETWORK_CSUM_ERR_CTR                  1297

#define BATCH_SINGLE_RECIPE_VACUUM_ON_BIT           0x01 // MBVACUUMISONBITPOS  EQU     %0000000000000001       ; VACUUM ON BIT POS




#define  BATCH_DIAG_L2_MASK                         0x0001   //MBLINKL2BPOS    EQU     %0000000000000001       ;
#define  BATCH_DIAG_L3_MASK                         0x0002   //MBLINKL3BPOS    EQU     %0000000000000010       ;
#define  BATCH_DIAG_L4_MASK                         0x0004   //MBLINKL4BPOS    EQU     %0000000000000100       ;
#define  BATCH_DIAG_L5_MASK                         0x0008   //MBLINKL5BPOS    EQU     %0000000000001000       ;



// Diagnostics data (for batch).
#define BATCH_CYCLE_TARGET_WEIGHT_1             1300            // MBPCMP1TARWGT Component #1 set weight. for Batch (g to 2 dp)
#define BATCH_CYCLE_ACTUAL_WEIGHT_1             1324            // MBPCMP1ACTWGT Component #1 act weight for Batch (g to 2 dp)
#define BATCH_CYCLE_TARGET_COUNTS_1             1348            // MBPCMP1CNTG Component #1 target counts for Batch
#define BATCH_CYCLE_ACTUAL_COUNTS_1             1372            // MBPCMP1CNAC Component #1 act counts for Batch
#define BATCH_CYCLE_ON_TIME_PITS_1              1396            // MBPCMP1TIMS Component #1 open(on) time, in PIT's
#define BATCH_CPI_1                             1420            // MBPCMP1CPI Component #1 Counts per interrupt.
#define BATCH_CPI_STAGE2_1                      1444            // MBPCMP1CPIST2 Component #1 Counts per interrupt, stage 2.
#define BATCH_CPI_STAGE3_1                      1468            // MBPCMP1CPIST3 Component #1 Counts per interrupt, stage 3.
#define BATCH_CYCLE_ON_TIME_SECS_1              1492            // MBPCMP1OPENSECONDS Component #1 open(on) time, in seconds to 4dp.
#define BATCH_CYCLE_LONG_TERM_RESETTABLE_WEIGHT 1516            //
#define BATCH_CYCLE_LONG_TERM_WEIGHT            1518            //
#define BATCH_CYCLE_LONG_TERM_RESETTABLE_WEIGHT_1 1520          //
#define BATCH_CYCLE_LONG_TERM_RESETTABLE_WEIGHT_2 1522          //
#define BATCH_CYCLE_LONG_TERM_RESETTABLE_WEIGHT_3 1524          //
#define BATCH_CYCLE_LONG_TERM_RESETTABLE_WEIGHT_4 1526          //
#define BATCH_CYCLE_LONG_TERM_RESETTABLE_WEIGHT_5 1528          //
#define BATCH_CYCLE_LONG_TERM_RESETTABLE_WEIGHT_6 1530          //
#define BATCH_CYCLE_LONG_TERM_RESETTABLE_WEIGHT_7 1532          //
#define BATCH_CYCLE_LONG_TERM_RESETTABLE_WEIGHT_8 1534          //
#define BATCH_CYCLE_LONG_TERM_RESETTABLE_WEIGHT_9 1536          //
#define BATCH_CYCLE_LONG_TERM_RESETTABLE_WEIGHT_10 1538          //
#define BATCH_CYCLE_LONG_TERM_RESETTABLE_WEIGHT_11 1540          //
#define BATCH_CYCLE_LONG_TERM_RESETTABLE_WEIGHT_12 1542          //


#define BATCH_CYCLE_LONG_TERM_WEIGHT_1          1544            // double
#define BATCH_CYCLE_LONG_TERM_WEIGHT_2          1546            //
#define BATCH_CYCLE_LONG_TERM_WEIGHT_3          1548            //
#define BATCH_CYCLE_LONG_TERM_WEIGHT_4          1550            //
#define BATCH_CYCLE_LONG_TERM_WEIGHT_5          1552            //
#define BATCH_CYCLE_LONG_TERM_WEIGHT_6          1554            //
#define BATCH_CYCLE_LONG_TERM_WEIGHT_7          1556            //
#define BATCH_CYCLE_LONG_TERM_WEIGHT_8          1558            //
#define BATCH_CYCLE_LONG_TERM_WEIGHT_9          1560            //
#define BATCH_CYCLE_LONG_TERM_WEIGHT_10         1562            //
#define BATCH_CYCLE_LONG_TERM_WEIGHT_11         1564            //
#define BATCH_CYCLE_LONG_TERM_WEIGHT_12         1566            //





#define BATCH_FILL_BY_WEIGHT_1                  1568            //
#define BATCH_FILL_BY_WEIGHT_2                  1569            //
#define BATCH_FILL_BY_WEIGHT_3                  1570            //
#define BATCH_FILL_BY_WEIGHT_4                  1571            //
#define BATCH_FILL_BY_WEIGHT_5                  1572            //
#define BATCH_FILL_BY_WEIGHT_6                  1573            //
#define BATCH_FILL_BY_WEIGHT_7                  1574            //
#define BATCH_FILL_BY_WEIGHT_8                  1575            //
#define BATCH_FILL_BY_WEIGHT_9                  1576            //
#define BATCH_FILL_BY_WEIGHT_10                 1577            //
#define BATCH_FILL_BY_WEIGHT_11                 1578            //
#define BATCH_FILL_BY_WEIGHT_12                 1579            //
#define BATCH_IS_DIAGNOSTICS_PASSWORD           1580            //
#define BATCH_HARDWARE_ID                       1581            // 8 words

#define BATCH_COMMS_STATUS_SEI                  1589
#define BATCH_COMMS_STATUS_LLS                  1590
#define BATCH_COMMS_STATUS_OPTIMISATION         1591
#define BATCH_COMMS_STATUS_NETWORK              1592
#define BATCH_COMMS_STATUS_TCPIP                1593
#define BATCH_COMMS_STATUS_PANEL                1594
#define BATCH_COMMS_STATUS_VAC8S                1595

#define BATCH_CYCLE_LONG_TERM_LENGTH_RESETTABLE 1596            //
#define BATCH_CYCLE_LONG_TERM_LENGTH            1598            //


#define BATCH_COMMS_PROTOCOL_UNUSED             0
#define BATCH_COMMS_PROTOCOL_COMMS_ERROR        1
#define BATCH_COMMS_PROTOCOL_COMMS_OKAY         2

// 0 unused
// 1 used and error
// 2 used and okay.


// next word available  1589


// Production summary blocks for components 11 and 12. **NB**
#define  PSUM_BLOCK_11          1600                                // Prod. Summary block for component #11
#define  PSUM_BLOCK_12          (PSUM_BLOCK_11 + PSUM_BLOCK_SIZE)   // Prod. Summary block for component #12

// Alarm blocks
/*
Block structure:
    Alarm type:Comp No. (m.s. bit of comp. no (b7 of word) used to indicate set/cleared - this will allow max 256 alarm types.
    Alarm time, Hour:Min
    Alarm Date, Month:Day of month
    Front Roll no.
    Front Roll length (length into roll)
    Back Roll no.
    Back Roll length (length into roll)
    Order No. (code describing order).
    Operator No. (code for operator on duty).
    Spare - for future use.
*/
// offsets into alarm block of data.
#define MB_ALARM_BLOCK_TYPE                 0   // alarm type:component.
#define MB_ALARM_BLOCK_SET_CLR_MASK     0x0080  // alarm state (set/cleared) use bit b7 of word.(1=set)
#define MB_ALARM_BLOCK_HOURS                1   // alarm Hours
#define MB_ALARM_BLOCK_MINUTES              2   // alarm Mins
#define MB_ALARM_BLOCK_MONTH                3   // Month:
#define MB_ALARM_BLOCK_DATE                 4   // Date:
#define MB_ALARM_BLOCK_OCCURRED             5   // occurred:
#define MB_ALARM_BLOCK_COMPONENT_NO         6   // Component No:
#define MB_ALARM_BLOCK_ORDER_NO             7   // order no.
#define MB_ALARM_BLOCK_OPERATOR_NO          8   // Operator no.
#define MB_ALARM_BLOCK_RESERVED             9   // spare for future use.

#define ALARM_BLOCK_SIZE        10                  // alarm block size in words.

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
// settle for 12 alarm blocks. (12 * 10 = 120 words, readable in one MB read message)

#define MB_ALARM_BLOCKS     12                  // No of alarm blocks

//#define  ALARM_BLOCK_13           (ALARM_BLOCK_1  + (ALARM_BLOCK_SIZE * 12 ))     // alarm block #13
//#define  ALARM_BLOCK_14           (ALARM_BLOCK_1  + (ALARM_BLOCK_SIZE * 13 ))     // alarm block #14


//#define  ALARM_BLOCK_15           (ALARM_BLOCK_1  + (ALARM_BLOCK_SIZE * 14 ))     // alarm block #15
//#define  ALARM_BLOCK_16           (ALARM_BLOCK_1  + (ALARM_BLOCK_SIZE * 15 ))     // alarm block #16





#define BATCH_END_OF_READ_ONLY_SECTION           1999           //


#define EMAIL_MAIL_SERVER                        2000            // mail server
#define EMAIL_USER_NAME                          2050            // mail user name
#define EMAIL_PASSWORD                           2100            // mail password
#define EMAIL_SUPERVISOR_EMAIL_ADDRESS           2150            // supervisor email address
#define EMAIL_SUPPORT_EMAIL_ADDRESS              2200            // operator email address
#define EMAIL_FROM_EMAIL_ADDRESS                 2250            // blender email address
#define EMAIL_BLENDERNAME                        2300            // blender email address

#define EMAIL_END                                2350            // END

#define NETBURNER_CONFIG_SET_IPADDRESS           2351
#define NETBURNER_CONFIG_SET_IPMASK              2355
#define NETBURNER_CONFIG_SET_IPGATEWAY           2359
#define NETBURNER_CONFIG_SET_IPDNS               2363

// 0 - off
// 1 - on
// blast air into bins
#define BATCH_AIR_BLAST_BIN_1                    2370
#define BATCH_AIR_BLAST_BIN_2                    2371
#define BATCH_AIR_BLAST_BIN_3                    2372
#define BATCH_AIR_BLAST_BIN_4                    2373
#define BATCH_AIR_BLAST_BIN_5                    2374
#define BATCH_AIR_BLAST_BIN_6                    2375
#define BATCH_AIR_BLAST_BIN_7                    2376
#define BATCH_AIR_BLAST_BIN_8                    2377
#define BATCH_AIR_BLAST_BIN_9                    2378
#define BATCH_AIR_BLAST_BIN_10                   2379
#define BATCH_AIR_BLAST_BIN_11                   2380
#define BATCH_AIR_BLAST_BIN_12                   2381

// 0 - off
// 1 - on
// drain bin
#define BATCH_DRAIN_BIN_1                        2382
#define BATCH_DRAIN_BIN_2                        2383
#define BATCH_DRAIN_BIN_3                        2384
#define BATCH_DRAIN_BIN_4                        2385
#define BATCH_DRAIN_BIN_5                        2386
#define BATCH_DRAIN_BIN_6                        2387
#define BATCH_DRAIN_BIN_7                        2388
#define BATCH_DRAIN_BIN_8                        2389
#define BATCH_DRAIN_BIN_9                        2390
#define BATCH_DRAIN_BIN_10                       2391
#define BATCH_DRAIN_BIN_11                       2392
#define BATCH_DRAIN_BIN_12                       2393



#define EMAIL_CONFIG_START                      EMAIL_MAIL_SERVER  // email start
#define EMAIL_CONFIG_END                         EMAIL_END

#define MULTIBLEND_RECIPEDOWNLOAD_BLOCK1                    2500            // recipe no
#define MULTIBLEND_BLOCK_SIZE                               50              // block size
#define  MULTIBLEND_RECIPEDOWNLOAD_BLOCK2       (MULTIBLEND_RECIPEDOWNLOAD_BLOCK1 + (MULTIBLEND_BLOCK_SIZE * 1))
#define  MULTIBLEND_RECIPEDOWNLOAD_BLOCK3       (MULTIBLEND_RECIPEDOWNLOAD_BLOCK1 + (MULTIBLEND_BLOCK_SIZE * 2))
#define  MULTIBLEND_RECIPEDOWNLOAD_BLOCK4       (MULTIBLEND_RECIPEDOWNLOAD_BLOCK1 + (MULTIBLEND_BLOCK_SIZE * 3))
#define  MULTIBLEND_RECIPEDOWNLOAD_BLOCK5       (MULTIBLEND_RECIPEDOWNLOAD_BLOCK1 + (MULTIBLEND_BLOCK_SIZE * 4))
#define  MULTIBLEND_RECIPEDOWNLOAD_BLOCK6       (MULTIBLEND_RECIPEDOWNLOAD_BLOCK1 + (MULTIBLEND_BLOCK_SIZE * 5))
#define  MULTIBLEND_RECIPEDOWNLOAD_BLOCK7       (MULTIBLEND_RECIPEDOWNLOAD_BLOCK1 + (MULTIBLEND_BLOCK_SIZE * 6))
#define  MULTIBLEND_RECIPEDOWNLOAD_BLOCK8       (MULTIBLEND_RECIPEDOWNLOAD_BLOCK1 + (MULTIBLEND_BLOCK_SIZE * 7))
#define  MULTIBLEND_RECIPEDOWNLOAD_BLOCK9       (MULTIBLEND_RECIPEDOWNLOAD_BLOCK1 + (MULTIBLEND_BLOCK_SIZE * 8))
#define  MULTIBLEND_RECIPEDOWNLOAD_BLOCK10      (MULTIBLEND_RECIPEDOWNLOAD_BLOCK1 + (MULTIBLEND_BLOCK_SIZE * 9))
#define  MULTIBLEND_RECIPEDOWNLOAD_BLOCK11      (MULTIBLEND_RECIPEDOWNLOAD_BLOCK1 + (MULTIBLEND_BLOCK_SIZE * 10))
#define  MULTIBLEND_RECIPEDOWNLOAD_BLOCK12      (MULTIBLEND_RECIPEDOWNLOAD_BLOCK1 + (MULTIBLEND_BLOCK_SIZE * 11))
#define  MULTIBLEND_RECIPEDOWNLOAD_BLOCK13      (MULTIBLEND_RECIPEDOWNLOAD_BLOCK1 + (MULTIBLEND_BLOCK_SIZE * 12))
#define  MULTIBLEND_RECIPEDOWNLOAD_BLOCK14      (MULTIBLEND_RECIPEDOWNLOAD_BLOCK1 + (MULTIBLEND_BLOCK_SIZE * 13))
#define  MULTIBLEND_RECIPEDOWNLOAD_BLOCK15      (MULTIBLEND_RECIPEDOWNLOAD_BLOCK1 + (MULTIBLEND_BLOCK_SIZE * 14))
#define  MULTIBLEND_RECIPEDOWNLOAD_BLOCK16      (MULTIBLEND_RECIPEDOWNLOAD_BLOCK1 + (MULTIBLEND_BLOCK_SIZE * 15))



#define RECIPEDOWNLOAD_BATCH_SETPOINT_PERCENT_1_OFFSET             0            // comp 1 %
#define RECIPEDOWNLOAD_BATCH_SETPOINT_PERCENT_2_OFFSET             1            //
#define RECIPEDOWNLOAD_BATCH_SETPOINT_PERCENT_3_OFFSET             2            //
#define RECIPEDOWNLOAD_BATCH_SETPOINT_PERCENT_4_OFFSET             3            //
#define RECIPEDOWNLOAD_BATCH_SETPOINT_PERCENT_5_OFFSET             4            //
#define RECIPEDOWNLOAD_BATCH_SETPOINT_PERCENT_6_OFFSET             5            //
#define RECIPEDOWNLOAD_BATCH_SETPOINT_PERCENT_7_OFFSET             6            //
#define RECIPEDOWNLOAD_BATCH_SETPOINT_PERCENT_8_OFFSET             7            //
#define RECIPEDOWNLOAD_BATCH_SETPOINT_PERCENT_9_OFFSET             8            //
#define RECIPEDOWNLOAD_BATCH_SETPOINT_PERCENT_10_OFFSET            9            //
#define RECIPEDOWNLOAD_BATCH_SETPOINT_PERCENT_11_OFFSET            10            //
#define RECIPEDOWNLOAD_BATCH_SETPOINT_PERCENT_12_OFFSET            11            //
#define RECIPEDOWNLOAD_BATCH_SETPOINT_REGRIND_COMPONENT_OFFSET     12
#define RECIPEDOWNLOAD_RECIPE_NO_OFFSET                            13           // recipe no
#define RECIPEDOWNLOAD_RECIPE_NAME_OFFSET                          14           // recipe name
#define RECIPEDOWNLOAD_ORDER_NAME_OFFSET                           24            // orderrecipe name



#define MULTIBLEND_EXTRUDERSETUP_EXTRUDER_NAME_1            3400
#define MULTIBLEND_EXTRUDERSETUP_EXTRUDER_NAME_2            3410
#define MULTIBLEND_EXTRUDERSETUP_EXTRUDER_NAME_3            3420
#define MULTIBLEND_EXTRUDERSETUP_EXTRUDER_NAME_4            3430
#define MULTIBLEND_EXTRUDERSETUP_EXTRUDER_NAME_5            3440
#define MULTIBLEND_EXTRUDERSETUP_EXTRUDER_NAME_6            3450
#define MULTIBLEND_EXTRUDERSETUP_EXTRUDER_NAME_7            3460
#define MULTIBLEND_EXTRUDERSETUP_EXTRUDER_NAME_8            3470
#define MULTIBLEND_EXTRUDERSETUP_EXTRUDER_NAME_9            3480
#define MULTIBLEND_EXTRUDERSETUP_EXTRUDER_NAME_10           3490
#define MULTIBLEND_EXTRUDERSETUP_EXTRUDER_NAME_11           3500
#define MULTIBLEND_EXTRUDERSETUP_EXTRUDER_NAME_12           3510
#define MULTIBLEND_EXTRUDERSETUP_EXTRUDER_NAME_13           3520
#define MULTIBLEND_EXTRUDERSETUP_EXTRUDER_NAME_14           3530
#define MULTIBLEND_EXTRUDERSETUP_EXTRUDER_NAME_15           3540
#define MULTIBLEND_EXTRUDERSETUP_EXTRUDER_NAME_16           3550

//#define MULTIBLEND_EXTRUDERSETUP_RECIPENO_EX1               3560
//#define MULTIBLEND_EXTRUDERSETUP_RECIPENO_EX2               3561
//#define MULTIBLEND_EXTRUDERSETUP_RECIPENO_EX3               3562
//#define MULTIBLEND_EXTRUDERSETUP_RECIPENO_EX4               3563
//#define MULTIBLEND_EXTRUDERSETUP_RECIPENO_EX5               3564
//#define MULTIBLEND_EXTRUDERSETUP_RECIPENO_EX6               3565
//#define MULTIBLEND_EXTRUDERSETUP_RECIPENO_EX7               3566
//#define MULTIBLEND_EXTRUDERSETUP_RECIPENO_EX8               3567
//#define MULTIBLEND_EXTRUDERSETUP_RECIPENO_EX9               3568
//#define MULTIBLEND_EXTRUDERSETUP_RECIPENO_EX10              3569
//#define MULTIBLEND_EXTRUDERSETUP_RECIPENO_EX11              3570
//#define MULTIBLEND_EXTRUDERSETUP_RECIPENO_EX12              3571
//#define MULTIBLEND_EXTRUDERSETUP_RECIPENO_EX13              3572
//#define MULTIBLEND_EXTRUDERSETUP_RECIPENO_EX14              3573
//#define MULTIBLEND_EXTRUDERSETUP_RECIPENO_EX15              3574
//#define MULTIBLEND_EXTRUDERSETUP_RECIPENO_EX16              3575

#define MULTIBLEND_EXTRUDERSETUP_ENABLE_EX1                 3560
#define MULTIBLEND_EXTRUDERSETUP_ENABLE_EX2                 3561
#define MULTIBLEND_EXTRUDERSETUP_ENABLE_EX3                 3562
#define MULTIBLEND_EXTRUDERSETUP_ENABLE_EX4                 3563
#define MULTIBLEND_EXTRUDERSETUP_ENABLE_EX5                 3564
#define MULTIBLEND_EXTRUDERSETUP_ENABLE_EX6                 3565
#define MULTIBLEND_EXTRUDERSETUP_ENABLE_EX7                 3566
#define MULTIBLEND_EXTRUDERSETUP_ENABLE_EX8                 3567
#define MULTIBLEND_EXTRUDERSETUP_ENABLE_EX9                 3568
#define MULTIBLEND_EXTRUDERSETUP_ENABLE_EX10                3569
#define MULTIBLEND_EXTRUDERSETUP_ENABLE_EX11                3570
#define MULTIBLEND_EXTRUDERSETUP_ENABLE_EX12                3571
#define MULTIBLEND_EXTRUDERSETUP_ENABLE_EX13                3572
#define MULTIBLEND_EXTRUDERSETUP_ENABLE_EX14                3573
#define MULTIBLEND_EXTRUDERSETUP_ENABLE_EX15                3574
#define MULTIBLEND_EXTRUDERSETUP_ENABLE_EX16                3575


#define MULTIBLEND_SUMMARY_QUEUE_POS_1                      3800
#define MULTIBLEND_SUMMARY_QUEUE_POS_2                      3801
#define MULTIBLEND_SUMMARY_QUEUE_POS_3                      3802
#define MULTIBLEND_SUMMARY_QUEUE_POS_4                      3803
#define MULTIBLEND_SUMMARY_QUEUE_POS_5                      3804
#define MULTIBLEND_SUMMARY_QUEUE_POS_6                      3805
#define MULTIBLEND_SUMMARY_QUEUE_POS_7                      3806
#define MULTIBLEND_SUMMARY_QUEUE_POS_8                      3807
#define MULTIBLEND_SUMMARY_QUEUE_POS_9                      3808
#define MULTIBLEND_SUMMARY_QUEUE_POS_10                     3809
#define MULTIBLEND_SUMMARY_QUEUE_POS_11                     3810
#define MULTIBLEND_SUMMARY_QUEUE_POS_12                     3811
#define MULTIBLEND_SUMMARY_QUEUE_POS_13                     3812
#define MULTIBLEND_SUMMARY_QUEUE_POS_14                     3813
#define MULTIBLEND_SUMMARY_QUEUE_POS_15                     3814
#define MULTIBLEND_SUMMARY_QUEUE_POS_16                     3815
#define MULTIBLEND_SUMMARY_QUEUE_LENGTH                     3816


#define MULTIBLEND_SUMMARY_FLAP_STATUS_EX1                  3817
#define MULTIBLEND_SUMMARY_FLAP_STATUS_EX2                  3818
#define MULTIBLEND_SUMMARY_FLAP_STATUS_EX3                  3819
#define MULTIBLEND_SUMMARY_FLAP_STATUS_EX4                  3820
#define MULTIBLEND_SUMMARY_FLAP_STATUS_EX5                  3821
#define MULTIBLEND_SUMMARY_FLAP_STATUS_EX6                  3822
#define MULTIBLEND_SUMMARY_FLAP_STATUS_EX7                  3823
#define MULTIBLEND_SUMMARY_FLAP_STATUS_EX8                  3824
#define MULTIBLEND_SUMMARY_FLAP_STATUS_EX9                  3825
#define MULTIBLEND_SUMMARY_FLAP_STATUS_EX10                 3826
#define MULTIBLEND_SUMMARY_FLAP_STATUS_EX11                 3827
#define MULTIBLEND_SUMMARY_FLAP_STATUS_EX12                 3828
#define MULTIBLEND_SUMMARY_FLAP_STATUS_EX13                 3829
#define MULTIBLEND_SUMMARY_FLAP_STATUS_EX14                 3830
#define MULTIBLEND_SUMMARY_FLAP_STATUS_EX15                 3831
#define MULTIBLEND_SUMMARY_FLAP_STATUS_EX16                 3832

#define MULTIBLEND_SUMMARY_IN_PROGREES_STATUS_EX1           3833
#define MULTIBLEND_SUMMARY_IN_PROGREES_STATUS_EX2           3834
#define MULTIBLEND_SUMMARY_IN_PROGREES_STATUS_EX3           3835
#define MULTIBLEND_SUMMARY_IN_PROGREES_STATUS_EX4           3836
#define MULTIBLEND_SUMMARY_IN_PROGREES_STATUS_EX5           3837
#define MULTIBLEND_SUMMARY_IN_PROGREES_STATUS_EX6           3838
#define MULTIBLEND_SUMMARY_IN_PROGREES_STATUS_EX7           3839
#define MULTIBLEND_SUMMARY_IN_PROGREES_STATUS_EX8           3840
#define MULTIBLEND_SUMMARY_IN_PROGREES_STATUS_EX9           3841
#define MULTIBLEND_SUMMARY_IN_PROGREES_STATUS_EX10          3842
#define MULTIBLEND_SUMMARY_IN_PROGREES_STATUS_EX11          3843
#define MULTIBLEND_SUMMARY_IN_PROGREES_STATUS_EX12          3844
#define MULTIBLEND_SUMMARY_IN_PROGREES_STATUS_EX13          3845
#define MULTIBLEND_SUMMARY_IN_PROGREES_STATUS_EX14          3846
#define MULTIBLEND_SUMMARY_IN_PROGREES_STATUS_EX15          3847
#define MULTIBLEND_SUMMARY_IN_PROGREES_STATUS_EX16          3848

#define MULTIBLEND_SUMMARY_VACUUM_ON_OFF_STATUS             3849
#define MULTIBLEND_SUMMARY_MULTIBLEND_STAGE                 3850
#define MULTIBLEND_SUMMARY_MULTIBLEND_PAUSED_STATUS         3851


#define MULTIBLEND_SUMMARY_DOWNLOAD_HISTORY_1               3852
#define MULTIBLEND_SUMMARY_DOWNLOAD_HISTORY_2               3853
#define MULTIBLEND_SUMMARY_DOWNLOAD_HISTORY_3               3854
#define MULTIBLEND_SUMMARY_DOWNLOAD_HISTORY_4               3855
#define MULTIBLEND_SUMMARY_DOWNLOAD_HISTORY_5               3856
#define MULTIBLEND_SUMMARY_DOWNLOAD_HISTORY_6               3857
#define MULTIBLEND_SUMMARY_DOWNLOAD_HISTORY_7               3858
#define MULTIBLEND_SUMMARY_DOWNLOAD_HISTORY_8               3859
#define MULTIBLEND_SUMMARY_DOWNLOAD_HISTORY_9               3860
#define MULTIBLEND_SUMMARY_DOWNLOAD_HISTORY_10              3861
#define MULTIBLEND_SUMMARY_DOWNLOAD_HISTORY_11              3862
#define MULTIBLEND_SUMMARY_DOWNLOAD_HISTORY_12              3863
#define MULTIBLEND_SUMMARY_DOWNLOAD_HISTORY_13              3864
#define MULTIBLEND_SUMMARY_DOWNLOAD_HISTORY_14              3865
#define MULTIBLEND_SUMMARY_DOWNLOAD_HISTORY_15              3866
#define MULTIBLEND_SUMMARY_DOWNLOAD_HISTORY_16              3867



#define LAST_MULTIBLEND_POSITION                            3999

#define BATCH_ICS_RECIPE_WORK_ORDER_REQUEST_NUMBER          3600
#define BATCH_ICS_RECIPE_WORK_ORDER_DOWNLOAD_STATUS         3601
#define BATCH_ICS_RECIPE_WORK_ORDER_ACTIVATE_ORDER_DOWNLOAD 3619
#define BATCH_ICS_RECIPE_ORDER_CODE                         3620
#define BATCH_ICS_RECIPE_CUSTOMER_CODE                      3625
#define BATCH_ICS_RECIPE_CODE                               3630
#define BATCH_ICS_RECIPE_MATERIAL_CODE_1                    3635
#define BATCH_ICS_RECIPE_MATERIAL_CODE_2                    3640
#define BATCH_ICS_RECIPE_MATERIAL_CODE_3                    3645
#define BATCH_ICS_RECIPE_MATERIAL_CODE_4                    3650
#define BATCH_ICS_RECIPE_MATERIAL_CODE_5                    3655
#define BATCH_ICS_RECIPE_MATERIAL_CODE_6                    3660
#define BATCH_ICS_RECIPE_MATERIAL_CODE_7                    3665
#define BATCH_ICS_RECIPE_MATERIAL_CODE_8                    3670
#define BATCH_ICS_RECIPE_MATERIAL_CODE_9                    3675
#define BATCH_ICS_RECIPE_MATERIAL_CODE_10                   3680
#define BATCH_ICS_RECIPE_MATERIAL_CODE_11                   3685
#define BATCH_ICS_RECIPE_MATERIAL_CODE_12                   3690


#define BATCH_ICS_RECIPE_START                              BATCH_ICS_RECIPE_WORK_ORDER_REQUEST_NUMBER
#define BATCH_ICS_RECIPE_END                                BATCH_ICS_RECIPE_MATERIAL_CODE_12+5




#define OPTIMISATION_SUMMARY_FLAP_STATUS_1                  4500
#define OPTIMISATION_SUMMARY_FLAP_STATUS_2                  4501
#define OPTIMISATION_SUMMARY_FLAP_STATUS_3                  4502
#define OPTIMISATION_SUMMARY_FLAP_STATUS_4                  4503
#define OPTIMISATION_SUMMARY_FLAP_STATUS_5                  4504
#define OPTIMISATION_SUMMARY_FLAP_STATUS_6                  4505
#define OPTIMISATION_SUMMARY_FLAP_STATUS_7                  4506
#define OPTIMISATION_SUMMARY_FLAP_STATUS_8                  4507
#define OPTIMISATION_SUMMARY_FLAP_STATUS_9                  4508
#define OPTIMISATION_SUMMARY_FLAP_STATUS_10                 4509
#define OPTIMISATION_SUMMARY_FLAP_STATUS_11                 4510
#define OPTIMISATION_SUMMARY_FLAP_STATUS_12                 4511
#define OPTIMISATION_SUMMARY_FLAP_STATUS_13                 4512
#define OPTIMISATION_SUMMARY_FLAP_STATUS_14                 4513
#define OPTIMISATION_SUMMARY_FLAP_STATUS_15                 4514
#define OPTIMISATION_SUMMARY_FLAP_STATUS_16                 4515


#define OPTIMISATION_SUMMARY_ENABLE_STATUS_1                4517
#define OPTIMISATION_SUMMARY_ENABLE_STATUS_2                4518
#define OPTIMISATION_SUMMARY_ENABLE_STATUS_3                4519
#define OPTIMISATION_SUMMARY_ENABLE_STATUS_4                4520
#define OPTIMISATION_SUMMARY_ENABLE_STATUS_5                4521
#define OPTIMISATION_SUMMARY_ENABLE_STATUS_6                4522
#define OPTIMISATION_SUMMARY_ENABLE_STATUS_7                4523
#define OPTIMISATION_SUMMARY_ENABLE_STATUS_8                4524
#define OPTIMISATION_SUMMARY_ENABLE_STATUS_9                4525
#define OPTIMISATION_SUMMARY_ENABLE_STATUS_10               4526
#define OPTIMISATION_SUMMARY_ENABLE_STATUS_11               4527
#define OPTIMISATION_SUMMARY_ENABLE_STATUS_12               4528
#define OPTIMISATION_SUMMARY_ENABLE_STATUS_13               4529
#define OPTIMISATION_SUMMARY_ENABLE_STATUS_14               4530
#define OPTIMISATION_SUMMARY_ENABLE_STATUS_15               4531
#define OPTIMISATION_SUMMARY_ENABLE_STATUS_16               4532

#define OPTIMISATION_SUMMARY_FILL_STATUS_1                  4533
#define OPTIMISATION_SUMMARY_FILL_STATUS_2                  4534
#define OPTIMISATION_SUMMARY_FILL_STATUS_3                  4535
#define OPTIMISATION_SUMMARY_FILL_STATUS_4                  4536
#define OPTIMISATION_SUMMARY_FILL_STATUS_5                  4537
#define OPTIMISATION_SUMMARY_FILL_STATUS_6                  4538
#define OPTIMISATION_SUMMARY_FILL_STATUS_7                  4539
#define OPTIMISATION_SUMMARY_FILL_STATUS_8                  4540
#define OPTIMISATION_SUMMARY_FILL_STATUS_9                  4541
#define OPTIMISATION_SUMMARY_FILL_STATUS_10                 4542
#define OPTIMISATION_SUMMARY_FILL_STATUS_11                 4543
#define OPTIMISATION_SUMMARY_FILL_STATUS_12                 4544
#define OPTIMISATION_SUMMARY_FILL_STATUS_13                 4545
#define OPTIMISATION_SUMMARY_FILL_STATUS_14                 4546
#define OPTIMISATION_SUMMARY_FILL_STATUS_15                 4547
#define OPTIMISATION_SUMMARY_FILL_STATUS_16                 4548


#define OPTIMISATION_BIN_AND_LOADER_WEIGHT_1                4549            // xxx.yy kgs
#define OPTIMISATION_BIN_AND_LOADER_WEIGHT_2                4550            //
#define OPTIMISATION_BIN_AND_LOADER_WEIGHT_3                4551            //
#define OPTIMISATION_BIN_AND_LOADER_WEIGHT_4                4552            //
#define OPTIMISATION_BIN_AND_LOADER_WEIGHT_5                4553            //
#define OPTIMISATION_BIN_AND_LOADER_WEIGHT_6                4554            //
#define OPTIMISATION_BIN_AND_LOADER_WEIGHT_7                4555            //
#define OPTIMISATION_BIN_AND_LOADER_WEIGHT_8                4556            //
#define OPTIMISATION_BIN_AND_LOADER_WEIGHT_9                4557            //
#define OPTIMISATION_BIN_AND_LOADER_WEIGHT_10               4558            //
#define OPTIMISATION_BIN_AND_LOADER_WEIGHT_11               4559            //
#define OPTIMISATION_BIN_AND_LOADER_WEIGHT_12               4560            //


#define OPTIMISATION_BIN_WEIGHT_1                           4561            // xxx.yy kgs
#define OPTIMISATION_BIN_WEIGHT_2                           4562            //
#define OPTIMISATION_BIN_WEIGHT_3                           4563            //
#define OPTIMISATION_BIN_WEIGHT_4                           4564            //
#define OPTIMISATION_BIN_WEIGHT_5                           4565            //
#define OPTIMISATION_BIN_WEIGHT_6                           4566            //
#define OPTIMISATION_BIN_WEIGHT_7                           4567            //
#define OPTIMISATION_BIN_WEIGHT_8                           4568            //
#define OPTIMISATION_BIN_WEIGHT_9                           4569            //
#define OPTIMISATION_BIN_WEIGHT_10                          4570            //
#define OPTIMISATION_BIN_WEIGHT_11                          4571            //
#define OPTIMISATION_BIN_WEIGHT_12                          4572            //

#define OPTIMISATION_LOADER_FULL_WEIGHT_1                   4573            // xxx.yy kgs
#define OPTIMISATION_LOADER_FULL_WEIGHT_2                   4574            //
#define OPTIMISATION_LOADER_FULL_WEIGHT_3                   4575            //
#define OPTIMISATION_LOADER_FULL_WEIGHT_4                   4576            //
#define OPTIMISATION_LOADER_FULL_WEIGHT_5                   4577            //
#define OPTIMISATION_LOADER_FULL_WEIGHT_6                   4578            //
#define OPTIMISATION_LOADER_FULL_WEIGHT_7                   4579            //
#define OPTIMISATION_LOADER_FULL_WEIGHT_8                   4580            //
#define OPTIMISATION_LOADER_FULL_WEIGHT_9                   4581            //
#define OPTIMISATION_LOADER_FULL_WEIGHT_10                  4582            //
#define OPTIMISATION_LOADER_FULL_WEIGHT_11                  4583            //
#define OPTIMISATION_LOADER_FULL_WEIGHT_12                  4584            //

#define OPTIMISATION_LOADER_EMPTYING_RATE_WEIGHT_1          4585            // x.yyy kgs / second
#define OPTIMISATION_LOADER_EMPTYING_RATE_WEIGHT_2          4586            //
#define OPTIMISATION_LOADER_EMPTYING_RATE_WEIGHT_3          4587            //
#define OPTIMISATION_LOADER_EMPTYING_RATE_WEIGHT_4          4588            //
#define OPTIMISATION_LOADER_EMPTYING_RATE_WEIGHT_5          4589            //
#define OPTIMISATION_LOADER_EMPTYING_RATE_WEIGHT_6          4590            //
#define OPTIMISATION_LOADER_EMPTYING_RATE_WEIGHT_7          4591            //
#define OPTIMISATION_LOADER_EMPTYING_RATE_WEIGHT_8          4592            //
#define OPTIMISATION_LOADER_EMPTYING_RATE_WEIGHT_9          4593            //
#define OPTIMISATION_LOADER_EMPTYING_RATE_WEIGHT_10         4594            //
#define OPTIMISATION_LOADER_EMPTYING_RATE_WEIGHT_11         4595            //
#define OPTIMISATION_LOADER_EMPTYING_RATE_WEIGHT_12         4596            //


#define OPTIMISATION_LOADER_WEIGHT_1                        4597            // xxx.yy kgs
#define OPTIMISATION_LOADER_WEIGHT_2                        4598            //
#define OPTIMISATION_LOADER_WEIGHT_3                        4599            //
#define OPTIMISATION_LOADER_WEIGHT_4                        4600            //
#define OPTIMISATION_LOADER_WEIGHT_5                        4601            //
#define OPTIMISATION_LOADER_WEIGHT_6                        4602            //
#define OPTIMISATION_LOADER_WEIGHT_7                        4603            //
#define OPTIMISATION_LOADER_WEIGHT_8                        4604            //
#define OPTIMISATION_LOADER_WEIGHT_9                        4605            //
#define OPTIMISATION_LOADER_WEIGHT_10                       4606            //
#define OPTIMISATION_LOADER_WEIGHT_11                       4607            //
#define OPTIMISATION_LOADER_WEIGHT_12                       4608            //

#define OPTIMISATION_REMAINING_ORDER_WEIGHT                 4609            //

#define OPTIMISATION_REMAINING_ORDER_WEIGHT_1               4611            //
#define OPTIMISATION_REMAINING_ORDER_WEIGHT_2               4613            //
#define OPTIMISATION_REMAINING_ORDER_WEIGHT_3               4615            //
#define OPTIMISATION_REMAINING_ORDER_WEIGHT_4               4617            //
#define OPTIMISATION_REMAINING_ORDER_WEIGHT_5               4619            //
#define OPTIMISATION_REMAINING_ORDER_WEIGHT_6               4621            //
#define OPTIMISATION_REMAINING_ORDER_WEIGHT_7               4623            //
#define OPTIMISATION_REMAINING_ORDER_WEIGHT_8               4625            //
#define OPTIMISATION_REMAINING_ORDER_WEIGHT_9               4627            //
#define OPTIMISATION_REMAINING_ORDER_WEIGHT_10              4629            //
#define OPTIMISATION_REMAINING_ORDER_WEIGHT_11              4631            //
#define OPTIMISATION_REMAINING_ORDER_WEIGHT_12              4633            //


#define OPTIMISATION_LOADER_EMPTYING_COUNTER_1              4635            //
#define OPTIMISATION_LOADER_EMPTYING_COUNTER_2              4636            //
#define OPTIMISATION_LOADER_EMPTYING_COUNTER_3              4637            //
#define OPTIMISATION_LOADER_EMPTYING_COUNTER_4              4638            //
#define OPTIMISATION_LOADER_EMPTYING_COUNTER_5              4639            //
#define OPTIMISATION_LOADER_EMPTYING_COUNTER_6              4640            //
#define OPTIMISATION_LOADER_EMPTYING_COUNTER_7              4641            //
#define OPTIMISATION_LOADER_EMPTYING_COUNTER_8              4642            //
#define OPTIMISATION_LOADER_EMPTYING_COUNTER_9              4643            //
#define OPTIMISATION_LOADER_EMPTYING_COUNTER_10             4644            //
#define OPTIMISATION_LOADER_EMPTYING_COUNTER_11             4645            //
#define OPTIMISATION_LOADER_EMPTYING_COUNTER_12             4646            //



#define OPTIMISATION_REMAINING_ORDER_LENGTH                 4647           //
#define OPTIMISATION__ORDER_LENGTH                          4649           //


#define MULTIBLEND_END  LAST_MULTIBLEND_POSITION


#define MULTIBLEND_RECIPEDOWNLOAD_SUMMARY_BLOCK1            4800           //
#define  MULTIBLEND_RECIPEDOWNLOAD_SUMMARY_BLOCK2       (MULTIBLEND_RECIPEDOWNLOAD_SUMMARY_BLOCK1 + (MULTIBLEND_BLOCK_SIZE * 1))
#define  MULTIBLEND_RECIPEDOWNLOAD_SUMMARY_BLOCK3       (MULTIBLEND_RECIPEDOWNLOAD_SUMMARY_BLOCK1 + (MULTIBLEND_BLOCK_SIZE * 2))
#define  MULTIBLEND_RECIPEDOWNLOAD_SUMMARY_BLOCK4       (MULTIBLEND_RECIPEDOWNLOAD_SUMMARY_BLOCK1 + (MULTIBLEND_BLOCK_SIZE * 3))
#define  MULTIBLEND_RECIPEDOWNLOAD_SUMMARY_BLOCK5       (MULTIBLEND_RECIPEDOWNLOAD_SUMMARY_BLOCK1 + (MULTIBLEND_BLOCK_SIZE * 4))
#define  MULTIBLEND_RECIPEDOWNLOAD_SUMMARY_BLOCK6       (MULTIBLEND_RECIPEDOWNLOAD_SUMMARY_BLOCK1 + (MULTIBLEND_BLOCK_SIZE * 5))
#define  MULTIBLEND_RECIPEDOWNLOAD_SUMMARY_BLOCK7       (MULTIBLEND_RECIPEDOWNLOAD_SUMMARY_BLOCK1 + (MULTIBLEND_BLOCK_SIZE * 6))
#define  MULTIBLEND_RECIPEDOWNLOAD_SUMMARY_BLOCK8       (MULTIBLEND_RECIPEDOWNLOAD_SUMMARY_BLOCK1 + (MULTIBLEND_BLOCK_SIZE * 7))
#define  MULTIBLEND_RECIPEDOWNLOAD_SUMMARY_BLOCK9       (MULTIBLEND_RECIPEDOWNLOAD_SUMMARY_BLOCK1 + (MULTIBLEND_BLOCK_SIZE * 8))
#define  MULTIBLEND_RECIPEDOWNLOAD_SUMMARY_BLOCK10      (MULTIBLEND_RECIPEDOWNLOAD_SUMMARY_BLOCK1 + (MULTIBLEND_BLOCK_SIZE * 9))
#define  MULTIBLEND_RECIPEDOWNLOAD_SUMMARY_BLOCK11      (MULTIBLEND_RECIPEDOWNLOAD_SUMMARY_BLOCK1 + (MULTIBLEND_BLOCK_SIZE * 10))
#define  MULTIBLEND_RECIPEDOWNLOAD_SUMMARY_BLOCK12      (MULTIBLEND_RECIPEDOWNLOAD_SUMMARY_BLOCK1 + (MULTIBLEND_BLOCK_SIZE * 11))
#define  MULTIBLEND_RECIPEDOWNLOAD_SUMMARY_BLOCK13      (MULTIBLEND_RECIPEDOWNLOAD_SUMMARY_BLOCK1 + (MULTIBLEND_BLOCK_SIZE * 12))
#define  MULTIBLEND_RECIPEDOWNLOAD_SUMMARY_BLOCK14      (MULTIBLEND_RECIPEDOWNLOAD_SUMMARY_BLOCK1 + (MULTIBLEND_BLOCK_SIZE * 13))
#define  MULTIBLEND_RECIPEDOWNLOAD_SUMMARY_BLOCK15      (MULTIBLEND_RECIPEDOWNLOAD_SUMMARY_BLOCK1 + (MULTIBLEND_BLOCK_SIZE * 14))
#define  MULTIBLEND_RECIPEDOWNLOAD_SUMMARY_BLOCK16      (MULTIBLEND_RECIPEDOWNLOAD_SUMMARY_BLOCK1 + (MULTIBLEND_BLOCK_SIZE * 15))


#define BATCH_CONFIG_PRINTER_USB_ENABLE                 5600
#define BATCH_CONFIG_ALARMS_DUMP_ENABLE                 5601
#define BATCH_CONFIG_ORDER_DUMP_ENABLE                  5602
#define BATCH_CONFIG_ROLL_DUMP_ENABLE                   5603
#define BATCH_CONFIG_HOURLY_DUMP_ENABLE                 5604
#define BATCH_CONFIG_BATCH_LOG_DUMP_ENABLE              5605
#define BATCH_CONFIG_EVENT_LOG_DUMP_ENABLE              5606
#define BATCH_CONFIG_HISTORY_LOG_DUMP_ENABLE            5607
#define BATCH_CONFIG_SHIFT_DUMP_ENABLE                  5608

#define BATCH_START_SYSTEM_CONFIG3                      BATCH_CONFIG_PRINTER_USB_ENABLE           //


#define BATCH_END_SYSTEM_CONFIG3                        5999            //



#define BATCH_STATICSTICS_SAMPLE_COUNTER_1               6000            //
#define BATCH_STATICSTICS_MAXIMUM_1                      6012            //
#define BATCH_STATICSTICS_MINIMUM_1                      6036            //
#define BATCH_STATICSTICS_MEAN_1                         6060            //
#define BATCH_STATICSTICS_STDEV_1                        6084            //

// next free is 6108


#define USB_INSERTED                                     6109
//#define USB_COPY_MOST_RECENT_LOG_FILE_TO_USB             6109            //
#define USB_COPY_MOST_RECENT_TWO_LOG_FILES_TO_USB        6110            //

#define USB_COPY_CONFIG_FILE_FROM_USB_TO_SD              6111            //
#define USB_COPY_CONFIG_FILE                             6112            //
#define USB_COPY_ALL_LOG_FILES_TO_USB                    6113            //

#define USB_COPY_FILE_TO_SD_CARD                         6114            //
#define USB_READ_SD_CARD_DIRECTORY                       6115            //

#define USB_COPY_IN_PROGRESS                             6116            //
#define USB_FILE_DATA_COPIED                             6117            // double nbb--todo-- is now single


#define SD_CARD_STATUS                                   6118            //
#define SD_CARD_NOT_INSERTED                             0
#define SD_CARD_INSERTED_AND_WRITABLE                    1
#define SD_CARD_INSERTED_AND_LOCKED                      2


#define SD_CARD_FILE_NO_COUNTER                          6119            //

#define SD_CARD_FILE_NAME_1                              6120            //
#define SD_CARD_FILE_NAME_2                              6130            //
#define SD_CARD_FILE_NAME_3                              6140            //
#define SD_CARD_FILE_NAME_4                              6150            //
#define SD_CARD_FILE_NAME_5                              6160            //
#define SD_CARD_FILE_NAME_6                              6170            //
#define SD_CARD_FILE_NAME_7                              6180            //
#define SD_CARD_FILE_NAME_8                              6190            //
#define SD_CARD_FILE_NAME_9                              6200            //
#define SD_CARD_FILE_NAME_10                             6210            //
#define SD_CARD_FILE_NAME_11                             6220            //
#define SD_CARD_FILE_NAME_12                             6230            //
#define SD_CARD_FILE_NAME_13                             6240            //
#define SD_CARD_FILE_NAME_14                             6250            //
#define SD_CARD_FILE_NAME_15                             6260            //
#define SD_CARD_FILE_NAME_16                             6270            //
#define SD_CARD_FILE_NAME_17                             6280            //
#define SD_CARD_FILE_NAME_18                             6290            //
#define SD_CARD_FILE_NAME_19                             6300            //
#define SD_CARD_FILE_NAME_20                             6310            //
#define SD_CARD_FILE_NAME_21                             6320            //
#define SD_CARD_FILE_NAME_22                             6330            //
#define SD_CARD_FILE_NAME_23                             6340            //
#define SD_CARD_FILE_NAME_24                             6350            //
#define SD_CARD_FILE_NAME_25                             6360            //
#define SD_CARD_FILE_NAME_26                             6370            //
#define SD_CARD_FILE_NAME_27                             6380            //
#define SD_CARD_FILE_NAME_28                             6390            //
#define SD_CARD_FILE_NAME_29                             6400            //
#define SD_CARD_FILE_NAME_30                             6410            //

#define SD_CARD_SOFTWARE_UPDATE_COMPLETE                 6420
#define SD_CARD_LOAD_CONFIG_COMPLETE                     6421
#define SD_CARD_LOAD_BACKUP_CONFIG_COMPLETE              6422
#define SD_CARD_SAVE_CONFIG_COMPLETE                     6423
#define SD_CARD_COPY_CONFIG_TO_BACKUP_COMPLETE           6424

#define BATCH_SEI_SOFTWARE_VERSION_NO                    6425  // 5 words
#define BATCH_SEI_HARDWARE_ID                            6430  // 5 words

#define BATCH_SD_CARD_DISABLE_ACCESS_CTR                 6435  //

#define BATCH_NBB_SERIAL_NUMBER                          6436   // 8 words
#define BATCH_PANEL_VERSION_NUMBER                       6444   // 4 words


#define BATCH_FUNCTION_RUNNING                           6448  //
#define BATCH_FUNCTION_SUBSECTION_RUNNING                6449  //
#define BATCH_BATSEQ                                     6450  //

#define BATCH_USB_READ_TIMEOUT_CTR                       6451  //
#define BATCH_USB_COMMAND_SEND_TIMEOUT_CTR               6452  //
#define BATCH_USB_FLUSH_TIMEOUT_CTR                      6453  //
#define BATCH_USB_WRITE_TIMEOUT_CTR                      6454  //
#define BATCH_USB_ECHOED_TIMEOUT_CTR                     6455  //
#define BATCH_USB_SEND_FILE_TIMEOUT_CTR                  6456  //
#define BATCH_USB_PRINTER_STATUS_TIMEOUT_CTR             6457  //

// Hardware Self test
#define BATCH_BLENDER_SELF_TEST_PASSED                   6458  //
#define BATCH_BLENDER_SELF_TEST_EEPROM                   6459  //
#define BATCH_BLENDER_SELF_TEST_SD_CARD                  6460  //
#define BATCH_BLENDER_SELF_TEST_A_TO_D                   6461  //
// Weight calibration
#define BATCH_BLENDER_SELF_TEST_CALIBRATION_CONSTANT_DEV 6462  //
#define BATCH_BLENDER_SELF_TEST_TARE_VALUE_DEV           6463  //
#define BATCH_BLENDER_SELF_TEST_COMPONENT_RETRY          6464  //
#define BATCH_BLENDER_SELF_TEST_HOPPER_TARE_DEV          6465  //
#define BATCH_BLENDER_SELF_TEST_COMP_TARE                6466  //
// events
#define BATCH_BLENDER_SELF_TEST_EVENTS                   6467  //
#define BATCH_BLENDER_SELF_TEST_TARE_ALARM               6468  //
#define BATCH_BLENDER_SELF_TEST_LEAK_ALARM               6469  //
#define BATCH_BLENDER_SELF_TEST_DUMP_FLAP                6470  //
// communications
#define BATCH_BLENDER_SELF_TEST_COMMS_PANEL              6471  //
#define BATCH_BLENDER_SELF_TEST_COMMS_SEI                6472  //
#define BATCH_BLENDER_SELF_TEST_COMMS_LLS                6473  //
#define BATCH_BLENDER_SELF_TEST_COMMS_OPT                6474  //
#define BATCH_BLENDER_SELF_TEST_COMMS_NETWORK            6475  //
#define BATCH_BLENDER_SELF_TEST_COMMS_TCPIP              6476  //

#define BATCH_BLENDER_SELF_TEST_COMP_STDEV               6477  //
#define BATCH_BLENDER_SELF_TEST_LOAD_CELL_SWITCH         6478  //
#define BATCH_BLENDER_SELF_TEST_IN_PROGRESS              6479  //

#define BATCH_LOCK_CONFIG_STATUS                         6481
#define BATCH_FOREGROUND_CTR                             6482
#define BATCH_FOREGROUND_ENTRY_EXIT                      6483
#define BATCH_MAX_KGPH_POSSIBLE                          6484

#define BATCH_BLENDER_SELF_TEST_REF_STABILITY            6485  //
#define BATCH_BLENDER_SELF_TEST_LOAD_CELL_STABILITY      6486  //


#define BATCH_SUMMARY_LIQUID_ADDITIVE_OUTPUT_PERCENTAGE  6500
#define BATCH_SUMMARY_LIQUID_ADDITIVE_OUTPUT_DTOA        6501
#define BATCH_SUMMARY_LIQUID_ADDITIVE_SET_KGHR           6502
#define BATCH_SUMMARY_LIQUID_ADDITIVE_ON_TIMER           6503

#define BATCH_BLENDER_MODBUS_DIAGNOSTIC_ADDRESS          6504  //
#define BATCH_BLENDER_MODBUS_DIAGNOSTIC_DATA             6505  // 20 words 6525 next free
#define BATCH_NBB_PASS_FAIL_STATUS                       6525  //
#define NBBTEST_UNRECOGNISED_STATUS                     0
#define NBBTEST_PASS_STATUS                             1
#define NBBTEST_FAIL_STATUS                             2

#define BATCH_SUMMARY_SD_CARD_ERROR_COUNTER              6526
#define BATCH_SUMMARY_SD_CARD_MOUNT_FAT_RETURN_ON_ERROR  6527
#define BATCH_SUMMARY_SD_CARD_CHDRIVE_RETURN_ON_ERROR    6528
#define BATCH_SUMMARY_SD_CARD_FP_RETURN_ON_ERROR         6529
#define BATCH_SUMMARY_SD_CARD_FILE_NAME_ON_ERROR         6530   //6 WORDS

#define BATCH_SUMMARY_WATCH_DOG_TIMER_STATUS             6536   //
#define BATCH_SUMMARY_EIP_ENABLED                        6537   //
#define BATCH_SUMMARY_EIP_CTR                            6538   //
#define BATCH_SUMMARY_EIP_ASSEMBLY_OBJECT_CTR            6539   //

#define BATCH_SUMMARY_SHA_SECRET_FAIL_CTR                6540   //
#define BATCH_SUMMARY_HASH_OF_ID_COMPARE_FAIL            6541   //
#define BATCH_SUMMARY_HASH_OF_TIME_FAIL                  6542   //

#define BATCH_SUMMARY_HASH_OF_SHA_IN_FLASH               6543
#define BATCH_SUMMARY_HASH_OF_SHA_CALCUALTED             6544
#define BATCH_SUMMARY_HASH_COMPARISON   		         6545 // 1 =same 0=different

#define BATCH_SUMMARY_NETBURNER_MODULE_TYPE		         6546
#define BATCH_SUMMARY_UNLIMITED_LICENSE_ACCESS           6547


// vacuum loader stuff
//


#define VACUUM_LOADER_NUMBER_OF_LOADERS_RW              6600    //no of loaders
#define VACUUM_LOADER_PUMP_ON_OFF_RO                    6601    //on off
#define VACUUM_LOADER_PUMP_IDLE_TIME_SECONDS_RW         6602    //
#define VACUUM_LOADER_PUMP_START_TIME_SECONDS_RW        6603    //
#define VACUUM_LOADER_VAC8IO_FOR_PUMP                   6604    // 1-3  (VAC8IO card used for pump, AV & Filter)
#define VACUUM_LOADER_ATMOSPHERE_VALVE_STATUS_RO        6605    //
#define VACUUM_LOADER_ATMOSPHERE_VALVE_OVERLAPTIME_RW   6606    //
#define VACUUM_LOADER_DUST_FILTER_STATUS_RO             6607    //
#define VACUUM_LOADER_DUST_FILTER_REPETITIONS_RW        6608    //
#define VACUUM_LOADER_DUST_FILTER_TYPE_RW               6609    //
#define VACUUM_LOADER_DUMP_DELAY_SECONDS_RW             6610    //


// 6620 - 6630
#define  VACUUM_LOADER_ON_OFF_OFFSET_RW                 (0)    //
#define  VACUUM_LOADER_SUCTION_TIME_SECONDS_OFFSET_RW   (1)    //
#define  VACUUM_LOADER_POST_FILL_OFFSET_RW              (2)    //
#define  VACUUM_LOADER_PRIORITY_OFFSET_RW               (3)    // priority 1-7
#define  VACUUM_LOADER_MANUAL_COMMAND_OFFSET            (4)    //
#define  VACUUM_LOADER_STATUS_OFFSET_RO                 (5)    //
// 0 normal, 1 alarm 2 override
#define  VACUUM_LOADER_FILL_TIMER_OFFSET_RO             (6)    //
#define  VACUUM_LOADER_FULL_STATUS_OFFSET_RO            (7)    // 0-empty, 1 full

#define VAC8_1_GOOD_TX_CTR                              6611
#define VAC8_1_GOOD_RX_CTR                              6613
#define VAC8_1_TIMEOUT_CTR                              6615
#define VAC8_1_CHECKSUM_ERROR_CTR                       6617

#define VAC8_2_GOOD_TX_CTR                              6619
#define VAC8_2_GOOD_RX_CTR                              6621
#define VAC8_2_TIMEOUT_CTR                              6623
#define VAC8_2_CHECKSUM_ERROR_CTR                       6625

#define VAC8_3_GOOD_TX_CTR                              6627
#define VAC8_3_GOOD_RX_CTR                              6629
#define VAC8_3_TIMEOUT_CTR                              6631
#define VAC8_3_CHECKSUM_ERROR_CTR                       6633

#define VACUUM_LOADER_MODE_MANUAL                       6635    // 0=DISABLED, 1= ENABLED
#define VACUUM_LOADER_PUMP_COMMAND                       6636    // 0=OFF, 1= ON
#define VACUUM_LOADER_ATMOSPHERE_VALVE_COMMAND           6637    // 0=OFF, 1= ON
#define VACUUM_LOADER_DUST_FILTER_COMMAND                6638    // 0=OFF, 1= ON
#define VACUUM_LOADER_CLEAR_ALARMS_COMMAND               6639    // 1= CLEAR ALARMS
#define VACUUM_LOADER_ALARMS_TOTAL                       6640    // no. of loader ALARMS


#define VACUUM_LOADER_BLOCK_SIZE                        10       //
#define VACUUM_LOADER_BLOCK1                            6650
#define VACUUM_LOADER_BLOCK2       (VACUUM_LOADER_BLOCK1 + (VACUUM_LOADER_BLOCK_SIZE * 1))
#define VACUUM_LOADER_BLOCK3       (VACUUM_LOADER_BLOCK1 + (VACUUM_LOADER_BLOCK_SIZE * 2))
#define VACUUM_LOADER_BLOCK4       (VACUUM_LOADER_BLOCK1 + (VACUUM_LOADER_BLOCK_SIZE * 3))
#define VACUUM_LOADER_BLOCK5       (VACUUM_LOADER_BLOCK1 + (VACUUM_LOADER_BLOCK_SIZE * 4))
#define VACUUM_LOADER_BLOCK6       (VACUUM_LOADER_BLOCK1 + (VACUUM_LOADER_BLOCK_SIZE * 5))
#define VACUUM_LOADER_BLOCK7       (VACUUM_LOADER_BLOCK1 + (VACUUM_LOADER_BLOCK_SIZE * 6))
#define VACUUM_LOADER_BLOCK8       (VACUUM_LOADER_BLOCK1 + (VACUUM_LOADER_BLOCK_SIZE * 7))
#define VACUUM_LOADER_BLOCK9       (VACUUM_LOADER_BLOCK1 + (VACUUM_LOADER_BLOCK_SIZE * 8))
#define VACUUM_LOADER_BLOCK10      (VACUUM_LOADER_BLOCK1 + (VACUUM_LOADER_BLOCK_SIZE * 9))
#define VACUUM_LOADER_BLOCK11      (VACUUM_LOADER_BLOCK1 + (VACUUM_LOADER_BLOCK_SIZE * 10))
#define VACUUM_LOADER_BLOCK12      (VACUUM_LOADER_BLOCK1 + (VACUUM_LOADER_BLOCK_SIZE * 11))
#define VACUUM_LOADER_BLOCK13      (VACUUM_LOADER_BLOCK1 + (VACUUM_LOADER_BLOCK_SIZE * 12))
#define VACUUM_LOADER_BLOCK14      (VACUUM_LOADER_BLOCK1 + (VACUUM_LOADER_BLOCK_SIZE * 13))
#define VACUUM_LOADER_BLOCK15      (VACUUM_LOADER_BLOCK1 + (VACUUM_LOADER_BLOCK_SIZE * 14))
#define VACUUM_LOADER_BLOCK16      (VACUUM_LOADER_BLOCK1 + (VACUUM_LOADER_BLOCK_SIZE * 15))

#define VACUUM_LOADER_BLOCK17      (VACUUM_LOADER_BLOCK1 + (VACUUM_LOADER_BLOCK_SIZE * 16))
#define VACUUM_LOADER_BLOCK18      (VACUUM_LOADER_BLOCK1 + (VACUUM_LOADER_BLOCK_SIZE * 17))
#define VACUUM_LOADER_BLOCK19      (VACUUM_LOADER_BLOCK1 + (VACUUM_LOADER_BLOCK_SIZE * 18))
#define VACUUM_LOADER_BLOCK20      (VACUUM_LOADER_BLOCK1 + (VACUUM_LOADER_BLOCK_SIZE * 19))
#define VACUUM_LOADER_BLOCK21      (VACUUM_LOADER_BLOCK1 + (VACUUM_LOADER_BLOCK_SIZE * 20))
#define VACUUM_LOADER_BLOCK22      (VACUUM_LOADER_BLOCK1 + (VACUUM_LOADER_BLOCK_SIZE * 21))
#define VACUUM_LOADER_BLOCK23      (VACUUM_LOADER_BLOCK1 + (VACUUM_LOADER_BLOCK_SIZE * 22))
#define VACUUM_LOADER_BLOCK24      (VACUUM_LOADER_BLOCK1 + (VACUUM_LOADER_BLOCK_SIZE * 23))

#define VACUUM_LOADER_START_CALIBRATION_BLOCK             VACUUM_LOADER_NUMBER_OF_LOADERS_RW
#define VACUUM_LOADER_END_CALIBRATION_BLOCK             VACUUM_LOADER_BLOCK24+ VACUUM_LOADER_BLOCK_SIZE
// next free 6890

#define  BATCH_SUMMARY_BLENDER_RUN_TIME                 7000  // double - blender power up time
#define  BATCH_SUMMARY_VALVE_OPERATIONS_1               7002  // double - valve operations
#define  BATCH_SUMMARY_VALVE_OPERATIONS_2               7004  // double - valve operations
#define  BATCH_SUMMARY_VALVE_OPERATIONS_3               7006  // double - valve operations
#define  BATCH_SUMMARY_VALVE_OPERATIONS_4               7008  // double - valve operations
#define  BATCH_SUMMARY_VALVE_OPERATIONS_5               7010  // double - valve operations
#define  BATCH_SUMMARY_VALVE_OPERATIONS_6               7012  // double - valve operations
#define  BATCH_SUMMARY_VALVE_OPERATIONS_7               7014  // double - valve operations
#define  BATCH_SUMMARY_VALVE_OPERATIONS_8               7016  // double - valve operations
#define  BATCH_SUMMARY_VALVE_OPERATIONS_9               7018  // double - valve operations
#define  BATCH_SUMMARY_VALVE_OPERATIONS_10              7020  // double - valve operations
#define  BATCH_SUMMARY_VALVE_OPERATIONS_11              7022  // double - valve operations
#define  BATCH_SUMMARY_VALVE_OPERATIONS_12              7024  // double - valve operations

#define  BATCH_HISTORIC_LOG_RETRY_C1_IN_HOUR           7026  //
#define  BATCH_HISTORIC_LOG_RETRY_C2_IN_HOUR           7027  //
#define  BATCH_HISTORIC_LOG_RETRY_C3_IN_HOUR           7028  //
#define  BATCH_HISTORIC_LOG_RETRY_C4_IN_HOUR           7029  //
#define  BATCH_HISTORIC_LOG_RETRY_C5_IN_HOUR           7030  //
#define  BATCH_HISTORIC_LOG_RETRY_C6_IN_HOUR           7031  //
#define  BATCH_HISTORIC_LOG_RETRY_C7_IN_HOUR           7032  //
#define  BATCH_HISTORIC_LOG_RETRY_C8_IN_HOUR           7033  //
#define  BATCH_HISTORIC_LOG_RETRY_C9_IN_HOUR           7034  //
#define  BATCH_HISTORIC_LOG_RETRY_C10_IN_HOUR          7035  //
#define  BATCH_HISTORIC_LOG_RETRY_C11_IN_HOUR          7036  //
#define  BATCH_HISTORIC_LOG_RETRY_C12_IN_HOUR          7037  //

#define  BATCH_HISTORIC_LOG_MOTOR_TRIPS_IN_HOUR 		7038  //
#define  BATCH_HISTORIC_LOG_DOOR_OPENINGS_IN_HOUR 		7039  //
#define  BATCH_HISTORIC_LEVEL_SENSOR_ACTIVATIONS_IN_HOUR 7040  //
#define  BATCH_HISTORIC_LOG_TARE_ALARMS_IN_HOUR 		7041  //
#define  BATCH_HISTORIC_LEAK_ALARMS_IN_HOUR 		    7042  //
#define  BATCH_HISTORIC_DUMP_FLAP_ALARMS_IN_HOUR 		7043  //
#define  BATCH_HISTORIC_LOG_MAX_TARE_VALUE_IN_HOUR 		7044  //
#define  BATCH_HISTORIC_LOG_MIN_TARE_VALUE_IN_HOUR 		7045  //
#define  BATCH_HISTORIC_LOG_LINE_SPEED_CHANGE_DETECTED	7046  //
#define  BATCH_HISTORIC_LOG_CONTROL_ALARMS_IN_HOUR  	7047  //
#define  BATCH_HISTORIC_LOG_MINUTES_IN_AUTO         	7048  //

#define BATCH_SET_LICENSE_OPTIONS_1                     8000  //
#define BATCH_SET_LICENSE_OPTIONS_2                     8001  //
#define BATCH_SET_LICENSE_OPTIONS_3                     8002  //
#define BATCH_SET_LICENSE_OPTIONS_4                     8003  //



#define MB_SD_CARD_FILE_NAME_SIZE                        (SD_CARD_FILE_NAME_2 - SD_CARD_FILE_NAME_1)

// next free 7000

#define LAST_MODBUS_DATA                                 (9000)
#define END_OF_MODBUS                                     LAST_MODBUS_DATA



#define MB_TABLE_END            END_OF_MODBUS

#define MB_TABLE_START          BATCH_SETPOINT_PERCENT_1
#define MB_TABLE_SIZE           (MB_TABLE_END - MB_TABLE_START)             // SIZE OF MODBUS TABLE IN REGISTERS (COMPLETE TABLE)

#define MB_TABLE_WRITE_START            BATCH_SETPOINT_PERCENT_1
//#define MB_TABLE_WRITE_END          BATCH_TIME_UNUSED1
//#define MB_TABLE_WRITE_END          EMAIL_END
#define MB_TABLE_WRITE_SIZE        (MB_TABLE_END - MB_TABLE_WRITE_START)      // WRITE SIZE (REGISTERS) OF MB TABLE.


#define MB_TABLE_RO_START       BATCH_SUMMARY_START
#define MB_TABLE_RO_END     MB_TABLE_END

// Read only area is area of table which we want to prevent people (external) writing into.
#define MB_TABLE_READ_ONLY_START       (BATCH_SUMMARY_START)
#define MB_TABLE_READ_ONLY_END     		(BATCH_END_OF_READ_ONLY_SECTION)
#define MB_TABLE_READ_ONLY_SIZE    		(MB_TABLE_READ_ONLY_START - MB_TABLE_READ_ONLY_END)


#define MB_TABLE_RO_SIZE        (MB_TABLE_END - MB_TABLE_RO_START)      // Read Only SIZE (REGISTERS) OF MB TABLE.
#define  BATCH_LAST_WRITE_REGISTER  MB_TABLE_END               //


#endif  // __BATCHMBINDICES_H__



