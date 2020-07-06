///////////////////////////////////////////////////////
// TSMPeripheralsMBIndices.h
//
//
// M.McKiernan                                  11-01-2005
// First pass.  (Adapted from BatchMBIndices.h)
//
// Proposing to treat the remote operator panel as two devices - UFP1 and UFP2.
// The modbus data for the panel consists of 320 words(640 bytes of display data) plus some
// command data, attribute write data, and key read data.
// The maximum write message size to the UFP is limited to 80 bytes, i.e. 2 display rows (PIC limitation).
// The key data needs to be read at fixed intervals - ideally up to 5 times/sec.  The display data can be
// written when time available.  To facillitate sequencing then, split the panel in two.
// Then the UFP2 (key data) can be polled more frequently than data written to display and with higher priority.
//
//
// P.Smith                                      7/3/5
// Added SEI_CMD_GO_TO_VFOLLOWER_BIT definition
//
// P.Smith                                      12/7/07
// added OPTIMISATION_SUMMARY_START /END
/*
Message polling for each peripheral.
1. SEI -    Once per second:                        COMMS_READ_SUMMARY
                                                            COMMS_WRITE_COMMANDS        (perhaps have an enable)
                On Power up(reset):                 COMMS_READ_CALIBRATION
                                                            COMMS_WRITE_CALIBRATION
                                                            COMMS_READ_ID
                When on an SEI calibration screen or entry to Calibration menu:
                                                            COMMS_READ_CALIBRATION      (poll calibration flag set)
                When required, after calibration:
                                                            COMMS_WRITE_CALIBRATION

2. LLS -    Once per second:                        COMMS_READ_SUMMARY
                                                            COMMS_WRITE_COMMANDS        (perhaps have an enable)

3. UFP1 -   Continuous(when data changed):  COMMS_WRITE_DISPLAY

4. UFP2 -   2-5 timse per second:               COMMS_READ_UFP2_SUMMARY
                Continuous(only when required): COMMS_WRITE_COMMANDS        (perhaps have an enable)

// P.Smith                                      11-02-2005
// MB_SEI_INC_DEC_REG defined as 19, correct MB_SEI_INC_RATE_MEASURED set to 20
// and MB_SEI_DEC_RATE_MEASURED set to 21
//

// P.Smith                                      28/11/05
#define SEI_COMMANDS_END            MB_SEI_UNUSED_WRITE_2

// P.Smith                                      12/6/07
// added OPTIMISATION_WRITE_START / END, OPTIMISATION_COMMANDS_START / END
//
// P.Smith                                      12/6/07
// increase sei summary read from sei to start at MB_SEI_DA instead of MB_SEI_STATUS
//
// P.Smith                                      10/3/08
// defined MB_SEI_INCREASE_DECREASE_TARGET as word 8
// added SEI_SHORTENED_COMMANDS_END
// defined MB_TACHO_SETPOINT as MB_SEI_INCREASE_DECREASE_TARGET for now
//
// P.Smith                                      12/3/08
// added SEI_STATUS_EXITING_AUTO_INC_DEC_MODE, #define SEI_CMD_CLR_INC_DEC_AUTO_ALARM
// added SEI_CMD_CLR_INC_DEC_AUTO_ALARM, SEI_STATUS_FORCED_EXIT_FROM_AUTO
// SEI_STATUS_AUTO_MANUAL_STATUS
//
// M.McKiernan                      30/4/08
// Copy taken from batch blender v2.52.  Original MBMastr.cpp had not changed from time copy
// of blender software taken for LiW, i.e. 2.36.
// Changed definitions, SEI_INCREASE_DECREASE_COMMANDS_END & SEI_WRITE_END
//
// M.McKiernan                      6/5/08
// Added defines for LiW cards.
*/
//
// M.McKiernan                      5/5/09
// Changed max from 4 to 6 define MAX_LIW_CARDS                        (6)   // This will cater for a 7 component
//
// M.McKiernan                      9/6/09
// Added defines for VAC8IO cards.
// Note, currently MAX_VAC8IO_CARDS defined as 2.
//
// P.Smith                      25/8/09
// A problem has been seen where the tacho setpoing being written down
// to the sei was getting corrupted.
// this was due to insufficient memory being allocated for the sei write
// array.
// this is corrected by setting SEI_CALIBRATION_END to MB_SEI_UNUSED_WRITE_2
//
// P.Smith                      30/9/09
// set up MAX_VAC8IO_CARDS as 3 to allow for 3 vac 8s
//
// P.Smith                      15/10/09
// change MAX_VAC8IO_CARDS  to 4
//
// P.Smith                      8/6/10
// include  "SSIFPeripheralMBIndices.h"

///////////////////////////////////////////////////////
//*****************************************************************************
// MACROS
// *****************************************************************************
#ifndef __TSMPERIPHERALSMBINDICES_H__
#define __TSMPERIPHERALSMBINDICES_H__

#include "General.h"
/*  Has been moved to Serialstuff.h
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

#define COMMS_INVALID_MESSAGE       -1

*/
#include  "SSIFPeripheralMBIndices.h"


/*
// modbus tables for TSM peripherals.

int g_arrnWriteSEIMBTable[SEI_TABLE_READ_SIZE];
int g_arrnReadSEIMBTable[SEI_TABLE_WRITE_SIZE];

int g_arrnWriteLLSMBTable[LLS_TABLE_READ_SIZE];
int g_arrnReadLLSMBTable[LLS_TABLE_WRITE_SIZE];
//
// The data for UFP1 is simply the VRAM in blender (640 bytes - 320 words)
// Also it is write only - not read back.
// #define g_arrnWriteUFP1MBTable[]     g_arrTextBuffer[]
//
int g_arrnWriteUFP2MBTable[UFP2_TABLE_WRITE_SIZE];
int g_arrnReadUFP2MBTable[UFP2_TABLE_READ_SIZE];

*/

// ******************************************
// SEI - MODBUS INDICES
// ******************************************

// Writable parameters
#define  MB_SEI_DA                              0   // D/A value
#define  MB_SEI_COMMAND                         1   // various command bits - definitions above.

// Defs for write commands message
#define SEI_COMMANDS_START      MB_SEI_DA
#define SEI_COMMANDS_END            MB_SEI_UNUSED_WRITE_2

// masks for the SEI write command
// once only operation - clears automatically.
#define SEI_CMD_CLR_RESET_BIT                   0x0001  // ASM = CLEARRESETBITPOS        EQU     %00000001
#define SEI_CMD_GO_TO_REMOTE_BIT                0x0002  // ASM = GOTOREMOTEBITPOS        EQU     %00000010
#define SEI_CMD_CLR_PULSE_ACCUM_BIT             0x0004
#define SEI_CMD_CLR_INC_DEC_AUTO_ALARM          0x0008  //
#define SEI_CMD_CLR_AUTO_MAN_BIT                0x0010  // ASM = CLEARAMBITPOS           EQU     %00010000       ; A/M RESET.
#define SEI_CMD_GO_TO_VFOLLOWER_BIT             0x2000  // ASM = GOTOVFMODEBITPOS        EQU     %00100000       ; VOLTAGE FOLLOWER BIT POSITION.
#define SEI_CMD_CLR_BRC_BIT                     0x0040  // ASM = CLEARBRCBITPOS          EQU     %01000000       ; BACK RC RESET.
#define SEI_CMD_CLR_FRC_BIT                     0x0080  // ASM = CLEARFRCBITPOS          EQU     %10000000       ; FRONT RC RESET.



// continuous operation - performs function according to bit state.
#define SEI_CMD_AM_LAMP_BIT                     0x0100
#define SEI_CMD_DEC_OP_BIT                      0x0200
#define SEI_CMD_INC_OP_BIT                      0x0400
#define SEI_CMD_ALARM_RLY_BIT                   0x0800          //
#define SEI_CMD_LSPD_TEST_RELAY_BIT             0x1000          //
#define SEI_CMD_EXTR_CTRL_MODE_BIT0             0x2000          // 00 = , 01 = , 10 = .
#define SEI_CMD_EXTR_CTRL_MODE_BIT1             0x4000          //
#define SEI_CMD_LSPD_TEST_MODE                  0X8000          //



#define  MB_SEI_STARTUP_RELAY_DA                    2   // D/A value to activate the start-up relay.
#define  MB_SEI_CONTROL_LAG                         3   // Lag in 10ths of second
#define  MB_SEI_INC_RATE                            4   // A/D counts change per 0.1 sec.
#define  MB_SEI_DEC_RATE                            5   // A/D counts change per 0.1 sec.
#define  MB_SEI_DEADBAND                            6   // No correction when within deadband (a/d counts)
#define  MB_SEI_FINE_BAND                           7   // Continuous increase/decrease when outside Fineband.
#define  MB_SEI_INCREASE_DECREASE_TARGET            8
#define  MB_SEI_UNUSED_WRITE_2                      9

// Defs for write calibration message
#define SEI_CALIBRATION_START       MB_SEI_STARTUP_RELAY_DA
#define SEI_CALIBRATION_END         MB_SEI_UNUSED_WRITE_2

#define SEI_SHORTENED_COMMANDS_END         MB_SEI_COMMAND

#define SEI_INCREASE_DECREASE_COMMANDS_START         MB_SEI_INCREASE_DECREASE_TARGET
//#define SEI_INCREASE_DECREASE_COMMANDS_END           MB_SEI_UNUSED_WRITE_2
// MMK - set inc-dec commands end to one word only.
#define SEI_INCREASE_DECREASE_COMMANDS_END           (MB_SEI_INCREASE_DECREASE_TARGET)

#define MB_TACHO_SETPOINT           MB_SEI_INCREASE_DECREASE_TARGET



// Defs for write AREA - will be checked when writing to SEI.
#define SEI_WRITE_START         MB_SEI_DA
//#define SEI_WRITE_END           MB_SEI_FINE_BAND
// mmk
#define SEI_WRITE_END           (MB_SEI_UNUSED_WRITE_2)



// Read only parameters - PROD. SUMMARY.
#define  MB_SEI_STATUS                              10  // Alarm(msbyte) and Status byte.




// masks for the SEI status word.
#define SEI_STATUS_RESET_BIT                0x0001 //  ASM = RESETBITPOS     EQU     %0000000000000001
#define SEI_STATUS_LOCALMODE_BIT            0x0002 //  ASM = LOCALBITPOS     EQU     %0000000000000010
#define SEI_STATUS_VOLTAGE_FOLLOWER_BIT     0x0004  // ASM = VOLFOLBITPOS    EQU     %0000000000000100
#define SEI_STATUS_INCDEC_BIT               0x0008  // ASM = INCDECBITPOS    EQU     %0000000000001000
#define SEI_STATUS_AUTO_MAN_BIT             0x0010  // ASM = AMFBITPOS       EQU     %0000000000010000       ; A / M BIT POSITION.
#define SEI_STATUS_POT_CTRL_BIT             0x0020
#define SEI_STATUS_BRC_BIT                  0x0040  // ASM = BRCBITPOS       EQU     %0000000001000000       ; BACK ROLL CHANGE.
#define SEI_STATUS_FRC_BIT                  0x0080  // ASM = FRCBITPOS       EQU     %0000000010000000
#define SEI_STATUS_FORCED_EXIT_FROM_AUTO    0x0100 // asm not used.
#define SEI_STATUS_AUTO_MANUAL_STATUS       0x0200  // Auto status auto = 1




#define  MB_SEI_INPUTS                              11  // SEI inputs

#define SEI_INPUT_AUTO_BIT                  0x0002      // Auto             AMINPUTBITPOS           EQU     %0000000000000010       ; AUTO
#define SEI_INPUT_INCREASE_BIT              0x0008      // Auto             INCINPUTBITPOS          EQU     %0000000000001000       ; INC
#define SEI_INPUT_DECREASE_BIT              0x0004      // Decrease         DECINPUTBITPOS          EQU     %0000000000000100       ; DEC
#define SEI_INPUT_FRONT_ROLL_CHANGE_BIT     0x0080      // Decrease         FRCINPUTBITPOS          EQU     %0000000010000000       ; FRC
#define SEI_INPUT_BACK_ROLL_CHANGE_BIT      0x0040      // Decrease         BRCINPUTBITPOS          EQU     %0000000001000000       ; BRC
#define SEI_INPUT_SPARE1_BIT                0x0001      // Decrease         SP1INPUTBITPOS          EQU     %0000000000000001       ; SPARE 1
#define SEI_INPUT_EXTRUDER_STALL_BIT        0x0010      // Extruder stall   SP2INPUTBITPOS          EQU     %0000000000010000       ; SPARE 2
#define SEI_INPUT_POT_CONTROL_BIT           0x0020      // Extruder stall   PCRINPUTBITPOS          EQU     %0000000000100000       ; POT CONTROL.






#define  MB_SEI_CURRENT_DA                          12  // D/A value being output by SEI
#define  MB_SEI_LSPD_ACCUM_MSW                      13  // Line speed pulses accumulator m.s.word
#define  MB_SEI_LSPD_ACCUM_LSW                      14  // Line speed pulses accumulator l.s.word
#define  MB_SEI_LSPD_PPS                            15  // Line speed pulses/SEC
#define  MB_SEI_LSPD_PPS_2                          16  // Line speed pulses/SEC (T = 2sec)

#define  MB_SEI_AD                                  17  // A/D value


#define  MB_SEI_INC_DEC_REG                         19  // Increase /decrease reg

#define  MB_SEI_INC_RATE_MEASURED                   20  // measure inc. rate in a/d cts per 0.1s
#define  MB_SEI_DEC_RATE_MEASURED                   21  // measure dec. rate in a/d cts per  0.1s

// Defs for read summary message
#define SEI_SUMMARY_START       MB_SEI_DA
#define SEI_SUMMARY_END         MB_SEI_DEC_RATE_MEASURED

// Defs for read all message
#define SEI_ALL_START           MB_SEI_DA
#define SEI_ALL_END             MB_SEI_DEC_RATE_MEASURED

// Read only parameters - ID.
#define  MB_SEI_SOFTWARE_REVISION               1000    // Software rev. - 5 words, 10 ASCII
#define  MB_SEI_BOARD_SERIAL_NO                 1005    // board serial no. - 5 words, 10 ASCII

// Defs for read ID message
#define SEI_ID_START            MB_SEI_SOFTWARE_REVISION
#define SEI_ID_END              (MB_SEI_BOARD_SERIAL_NO + 4)        // NB - last read register addr.
#define SEI_ID_OFFSET           (SEI_ID_START - (SEI_SUMMARY_END +1) )      // Table in blender contiguous,
                                                                                        // need to remove offset between summary and ID.

#define SEI_TABLE_READ_SIZE         ( SEI_ID_END - SEI_ID_OFFSET + 1 )   // Complete table size in blender.
#define SEI_TABLE_WRITE_SIZE            ( SEI_CALIBRATION_END + 1 )  // Complete table write size in blender.



// ******************************************
// LLS - MODBUS INDICES (Low level sensor card, using VAC8)
// ******************************************

// Writeable parameters
#define  MB_LLS_COMMAND                         0   // command bits - outputs.

// Defs for write commands message
#define LLS_COMMANDS_START      MB_LLS_COMMAND
#define LLS_COMMANDS_END            MB_LLS_COMMAND      // Currently only 1 word.

// Defs for write AREA - will be checked when writing to LLS.
#define LLS_WRITE_START     MB_LLS_COMMAND
#define LLS_WRITE_END           MB_LLS_COMMAND


// Read only parameters - PROD. SUMMARY.

#define  MB_LLS_INPUTS                              8   // INPUTS ON LLS CARD.

// Defs for read summary message
#define LLS_SUMMARY_START           MB_LLS_INPUTS
#define LLS_SUMMARY_END         MB_LLS_INPUTS       // Currently only 1 word.

#define LLS_TABLE_READ_SIZE     (LLS_SUMMARY_END + 1)  // Complete table read size in blender.
#define LLS_TABLE_WRITE_SIZE        (MB_LLS_COMMAND + 1)   // Complete table write size in blender,  only one word used so far.


#define OPTIMISATION_SUMMARY_START           MB_LLS_INPUTS
#define OPTIMISATION_SUMMARY_END             MB_LLS_INPUTS       // Currently only 1 word.

#define OPTIMISATION_WRITE_START         MB_LLS_COMMAND
#define OPTIMISATION_WRITE_END           MB_LLS_COMMAND

#define OPTIMISATION_COMMANDS_START      MB_LLS_COMMAND
#define OPTIMISATION_COMMANDS_END            MB_LLS_COMMAND      // Currently only 1 word.



// ******************************************
//  UFP1 - MODBUS INDICES (Remote Front Panel - display data only)
// ******************************************

// Writeable parameters
#define  MB_UFP1_VRAM                           0   // command bits - outputs.

// Defs for write Display message
#define UFP1_START              MB_UFP1_VRAM
#define UFP1_END                    ( MB_UFP1_VRAM + 320 - 1 )      // Size of VRAM (16*40 / 2 words)

#define UFP1_TABLE_WRITE_SIZE       (UFP1_END + 1)  // Complete table Write size

// Defs for write AREA - will be checked when writing to UFP1
#define UFP1_WRITE_START        UFP1_START
#define UFP1_WRITE_END          UFP1_END


// ******************************************
//  UFP2 - MODBUS INDICES (Remote Front Panel - Commands, cursor, attributes, key data)
// ******************************************

// Writeable parameters
#define  MB_UFP2_CURSOR                             320     // Cursor address
#define  MB_UFP2_COMMAND1                       321     // Command register 1

// masks for the UFP write command #1
// once only operation - clears automatically.
#define UFP2_CMD_CLR_VRAM_BIT                   0x0001
#define UFP2_CMD_ATTS_NORMAL_BIT                0x0002
#define UFP2_CMD_BUZZER_ON_BIT                  0x0004
#define UFP2_CMD_CLR_KEYS_BIT                   0x0008  //
#define UFP2_CMD_CLR_RESET_BIT                  0x0010
#define UFP2_CMD_CLR_CRC_ERRS_BIT               0x0020

// continuous operation - performs function according to bit state.
#define UFP2_CMD_CURSOR_ON_BIT                      0x0100
#define UFP2_CMD_CURSOR_OFF_BIT                     0xFEFF      //
#define UFP2_CMD_NO_REFRESH_BIT                     0x0200


#define  MB_UFP2_COMMAND2                       322     // Command register 2
// 1 bit per line (e.g. b0 clears L. 1)

#define  MB_UFP2_ATTRIBUTES_1                   323     // Attributes register 1
#define  MB_UFP2_ATTRIBUTES_2                   325     // Attributes register 2
#define  MB_UFP2_ATTRIBUTES_3                   327     // Attributes register 3
#define  MB_UFP2_ATTRIBUTES_4                   329     // Attributes register 4

#define  MB_UFP2_ATTRIBUTES_5                   331     // Attributes register 5
#define  MB_UFP2_ATTRIBUTES_6                   333     // Attributes register 6
#define  MB_UFP2_ATTRIBUTES_7                   335     // Attributes register 7
#define  MB_UFP2_ATTRIBUTES_8                   337     // Attributes register 8

#define  MB_UFP2_UNUSED_1                       339     // RESERVED WRITE PARAMETER.


// Defs for write commands message
#define UFP2_COMMANDS_START         MB_UFP2_CURSOR
#define UFP2_COMMANDS_END               (MB_UFP2_ATTRIBUTES_8 + 1)      // last writable register.

// Defs for write AREA - will be checked when writing to UFP2
#define UFP2_WRITE_START        MB_UFP2_CURSOR
#define UFP2_WRITE_END          (MB_UFP2_ATTRIBUTES_8 + 1)


#define  MB_UFP2_KEY_REG                    340     // Key data register
#define  MB_UFP2_INC_DEC_KEY_REG            341     // Increase/Decrease Key data register. (DEC in l.s. byte)
#define  MB_UFP2_CRC_ERRS                   342     // CRC error counter.

// Defs for read summary message
#define UFP2_SUMMARY_START          MB_UFP2_KEY_REG
#define UFP2_SUMMARY_END                MB_UFP2_CRC_ERRS


// Sizes for MB table for UFP2.
#define UFP2_TABLE_WRITE_SIZE       (UFP2_COMMANDS_END - UFP2_COMMANDS_START + 1)  // table Write size
#define UFP2_TABLE_READ_SIZE            (UFP2_SUMMARY_END - UFP2_COMMANDS_START + 1)  // table Write size
#define UFP2_OFFSET                     MB_UFP2_CURSOR

typedef struct {
    unsigned char cType;
    unsigned char cSize;
    int nAddress;
} structAttributeData;

#define MAX_ATT_BLOCKS          8           //
#define MAX_ATTRIB_BLOCK_SIZE   250     // absolute max is 255, because is a byte only.
//  structAttributeData g_CommsAttributeArray[MAX_ATTS];




// ******************************************
// LIW - MODBUS INDICES
// ******************************************
#define MAX_LIW_CARDS                        (6)   // This will cater for a 7 component blender. (main comp + 4 LiW cards)
// Writable parameters
#define  MB_LIW_DA                              0   // D/A value
#define  MB_LIW_COMMAND                         1   // various command bits - definitions above.
// masks for the LIW write command
//ToDo - copied from SEI.
// once only operation - clears automatically.
#define LIW_CMD_CLR_RESET_BIT                   0x0001  // ASM = CLEARRESETBITPOS        EQU     %00000001
#define LIW_CMD_GO_TO_REMOTE_BIT                0x0002  // ASM = GOTOREMOTEBITPOS        EQU     %00000010
#define LIW_CMD_CLR_PULSE_ACCUM_BIT             0x0004
#define LIW_CMD_CLR_INC_DEC_AUTO_ALARM          0x0008  //
#define LIW_CMD_CLR_AUTO_MAN_BIT                0x0010  // ASM = CLEARAMBITPOS           EQU     %00010000       ; A/M RESET.
#define LIW_CMD_GO_TO_VFOLLOWER_BIT             0x2000  // ASM = GOTOVFMODEBITPOS        EQU     %00100000       ; VOLTAGE FOLLOWER BIT POSITION.
#define LIW_CMD_CLR_BRC_BIT                     0x0040  // ASM = CLEARBRCBITPOS          EQU     %01000000       ; BACK RC RESET.
#define LIW_CMD_CLR_FRC_BIT                     0x0080  // ASM = CLEARFRCBITPOS          EQU     %10000000       ; FRONT RC RESET.

#define  MB_LIW_STARTUP_RELAY_VALUE             (2)
#define  MB_LIW_PARAM_3                         (3)
#define  MB_LIW_PARAM_4                         (4)
#define  MB_LIW_PARAM_5                         (5)
#define  MB_LIW_PARAM_6                         (6)
#define  MB_LIW_PARAM_7                         (7)
#define  MB_LIW_PARAM_8                         (8)
#define  MB_LIW_PARAM_9                         (9)

// Read only parameters - PROD. SUMMARY.
#define  MB_LIW_STATUS                          (10)  // Alarm(msbyte) and Status byte.
//ToDo....copied from SEI
// masks for the LIW status word.
#define LIW_STATUS_RESET_BIT                0x0001 //  ASM = RESETBITPOS     EQU     %0000000000000001
#define LIW_STATUS_LOCALMODE_BIT            0x0002 //  ASM = LOCALBITPOS     EQU     %0000000000000010
// not used. #define LIW_STATUS_VOLTAGE_FOLLOWER_BIT     0x0004  // ASM = VOLFOLBITPOS    EQU     %0000000000000100
#define LIW_STATUS_INCDEC_BIT               0x0008  // ASM = INCDECBITPOS    EQU     %0000000000001000
#define LIW_STATUS_AUTO_MAN_BIT             0x0010  // ASM = AMFBITPOS       EQU     %0000000000010000       ; A / M BIT POSITION.
#define LIW_STATUS_POT_CTRL_BIT             0x0020
#define LIW_STATUS_BRC_BIT                  0x0040  // ASM = BRCBITPOS       EQU     %0000000001000000       ; BACK ROLL CHANGE.
#define LIW_STATUS_FRC_BIT                  0x0080  // ASM = FRCBITPOS       EQU     %0000000010000000
// #define LIW_STATUS_FORCED_EXIT_FROM_AUTO    0x0100 // asm not used.
// #define LIW_STATUS_AUTO_MANUAL_STATUS       0x0200  // Auto status auto = 1
#define LIW_STATUS_EEPROM_ERROR             0x0400  // ASM =
#define LIW_STATUS_PFO_ERROR                0x8000  // ASM =


#define  MB_LIW_INPUTS                       (11)  // LIW inputs

//ToDo....copied from SEI
#define LIW_INPUT_AUTO_BIT                  0x0002      // Auto             AMINPUTBITPOS           EQU     %0000000000000010       ; AUTO
#define LIW_INPUT_INCREASE_BIT              0x0008      // Auto             INCINPUTBITPOS          EQU     %0000000000001000       ; INC
#define LIW_INPUT_DECREASE_BIT              0x0004      // Decrease         DECINPUTBITPOS          EQU     %0000000000000100       ; DEC
#define LIW_INPUT_FRONT_ROLL_CHANGE_BIT     0x0080      // Decrease         FRCINPUTBITPOS          EQU     %0000000010000000       ; FRC
#define LIW_INPUT_BACK_ROLL_CHANGE_BIT      0x0040      // Decrease         BRCINPUTBITPOS          EQU     %0000000001000000       ; BRC
#define LIW_INPUT_SPARE1_BIT                0x0001      // Decrease         SP1INPUTBITPOS          EQU     %0000000000000001       ; SPARE 1
#define LIW_INPUT_EXTRUDER_STALL_BIT        0x0010      // Extruder stall   SP2INPUTBITPOS          EQU     %0000000000010000       ; SPARE 2
#define LIW_INPUT_POT_CONTROL_BIT           0x0020      // Extruder stall   PCRINPUTBITPOS          EQU     %0000000000100000       ; POT CONTROL.

#define  MB_LIW_CURRENT_DA                  (12)  // D/A value being output by LIW
#define  MB_LIW_LSPD_ACCUM_MSW               (13)  // Line speed pulses accumulator m.s.word
#define  MB_LIW_LSPD_ACCUM_LSW                (14)  // Line speed pulses accumulator l.s.word
#define  MB_LIW_LSPD_PPS                      (15)  // Line speed pulses/SEC
#define  MB_LIW_LSPD_PPS_2                    (16)  // Line speed pulses/SEC (T = 2sec)

#define  MB_LIW_AD                            (17)  // A/D value


#define  MB_LIW_INC_DEC_REG                   (19)  // Increase /decrease reg

#define  MB_LIW_REVISION_DETAILS              (20)  // board revision, software etc.
#define  MB_LIW_REVISION_DETAILS_END          (23)  //
#define  MB_LIW_LAST_REGISTER                 (25)  // board revision, software etc.

// Defs for read summary message
#define LIW_SUMMARY_START       MB_LIW_STATUS
#define LIW_SUMMARY_END         MB_LIW_INC_DEC_REG

// Defs for write commands message
#define LIW_COMMANDS_START          (MB_LIW_DA)
#define LIW_COMMANDS_END            (MB_LIW_COMMAND)

// Defs for write calibration message
#define LIW_CALIBRATION_START          ( MB_LIW_STARTUP_RELAY_VALUE )
#define LIW_CALIBRATION_END            ( MB_LIW_STARTUP_RELAY_VALUE )

// Defs for read id message
#define LIW_ID_START       MB_LIW_REVISION_DETAILS
#define LIW_ID_END         MB_LIW_REVISION_DETAILS_END

#define LIW_TABLE_READ_SIZE            ( MB_LIW_LAST_REGISTER + 1 )   // Complete table size in blender.
#define LIW_TABLE_WRITE_SIZE           ( MB_LIW_PARAM_9 + 1 )  // Complete table write size in WORDS

// VAC8IO cards

// ******************************************
// VAC8IO - MODBUS INDICES
// ******************************************
#define MAX_VAC8IO_CARDS                        (4)   // This will cater for 24 loaders
// Writable parameters
#define  MB_VAC8IO_COMMAND1                      (0)  // O/P's command 1
#define  MB_VAC8IO_COMMAND2                      (1)  // O/P's command 2
// masks for the VAC8IO write command
//ToDo - c
// once only operation - clears automatically.
#define VAC8IO_CMD2_PUMP_BIT                    0x0001  // A
#define VAC8IO_CMD2_AV_BIT                      0x0002  // A
#define VAC8IO_CMD2_FILTER_BIT                  0x0008  // A
#define VAC8IO_CMD2_CLR_RESET_BIT               0x0010  // A

#define  MB_VAC8IO_PARAM_2                         (2)
#define  MB_VAC8IO_PARAM_3                         (3)
#define  MB_VAC8IO_PARAM_4                         (4)
// Read only parameters - PROD. SUMMARY.
#define  MB_VAC8IO_PARAM_5                            (5)
#define  MB_VAC8IO_OPS_STATUS                         (6)
#define  MB_VAC8IO_OPS2_STATUS                        (7)
#define  MB_VAC8IO_IPS_STATUS                         (8)
#define  MB_VAC8IO_PARAM_9                            (9)


// masks for the VAC8IO OPS2 status word.
#define VAC8IO_OPS2_STATUS_PUMP_BIT                   0x0001 //
#define VAC8IO_OPS2_STATUS_AV_BIT                     0x0002 //
#define VAC8IO_OPS2_STATUS_FILTER_BIT                 0x0008 //
#define VAC8IO_OPS2_STATUS_RESET_BIT                  0x0010 //



// ToDo
#define  MB_VAC8IO_REVISION_DETAILS              (20)  // board revision, software etc.
#define  MB_VAC8IO_REVISION_DETAILS_END          (23)  //
#define  MB_VAC8IO_LAST_REGISTER                 (25)  // board revision, software etc.

// Defs for read summary message
#define VAC8IO_SUMMARY_START       (MB_VAC8IO_OPS_STATUS)
#define VAC8IO_SUMMARY_END         (MB_VAC8IO_IPS_STATUS)

// Defs for write commands message
#define VAC8IO_COMMANDS_START          (MB_VAC8IO_COMMAND1)
#define VAC8IO_COMMANDS_END            (MB_VAC8IO_COMMAND2)

// Defs for write calibration message  - not used on VAC8IO...as of now.
//#define VAC8IO_CALIBRATION_START          ( )
//#define VAC8IO_CALIBRATION_END            (  )

// Defs for read id message  - not used on VAC8IO...as of now.
#define VAC8IO_ID_START       MB_VAC8IO_REVISION_DETAILS
#define VAC8IO_ID_END         MB_VAC8IO_REVISION_DETAILS_END

#define VAC8IO_TABLE_READ_SIZE            ( MB_VAC8IO_PARAM_9 + 1 )   // Complete table size in blender.
#define VAC8IO_TABLE_WRITE_SIZE           ( MB_VAC8IO_PARAM_2 + 1 )  // Complete table write size in WORDS


#endif  // TSMPERIPHERALSMBINDICES_H__

// LiW card bytes in ModBus table (DEFINE AS APPROPRIATE)
/*
#define MBDA_MSB        0           // D/A VALUE
#define MBDA_LSB        1           // D/A VALUE LSB

#define CMD_MSB         2           // COMMAND most sign. byte MBPARAM0_MSB
#define CMD_LSB         3           // COMMAND least sign. byte MBPARAM0_LSB

#define DASURLY_MSB    4           //
#define DASURLY_LSB    5           // D/A VALUE AT WHICH START-UP RELAY ACTIVATES

#define MBPARAM3_MSB    6           //
#define MBPARAM3_LSB    7           //

#define MBPARAM4_MSB    8           //
#define MBPARAM4_LSB    9           //

#define MBPARAM5_MSB    10          //
#define MBPARAM5_LSB    11          //

#define MBPARAM6_MSB      12        //
#define MBPARAM6_LSB      13        //

#define MBPARAM7_MSB      14        //
#define MBPARAM7_LSB      15        //

#define MBPARAM8_MSB       16       //
#define MBPARAM8_LSB       17       //

#define MBPARAM9_MSB      18        //
#define MBPARAM9_LSB      19        //

// Read only

#define ALARM           20          // alarm byte
#define STATUS          21          // Status byte

#define MBIPS_MSB       22           //
#define MBIPS_LSB       23           //

#define MBACTDA_MSB      24           //  CURRENT OR ACTIVE D/A VALUE, I.E.
#define MBACTDA_LSB      25           //  VALUE BEING OUTPUT FROM SEI.

//#define INCKEYREG       26           // INCREASE KEY - VALUE IN 10mS PERIODS
//#define DECKEYREG       27           // INCREASE KEY - VALUE IN 10mS PERIODS


#define PULSESMSW_MSB    26          //
#define PULSESMSW_LSB    27          //

#define PULSESLSW_MSB    28          //
#define PULSESLSW_LSB    29          //

#define LSPD1SEC_MSB    30           //
#define LSPD1SEC_LSB    31           //

#define LSPD2SEC_MSB    32          //
#define LSPD2SEC_LSB    33          //

//TESTONLY
#define MBADMSW_MSB      34           //  CURRENT A/D
#define MBADMSW_LSB      35           //  VALUE

//#define MBPARAM17_MSB    34          //
//#define MBPARAM17_LSB    35          //
#define MBADLSW_MSB      36           //  CURRENT A/D
#define MBADLSW_LSB      37           //  VALUE

//#define MBPARAM18_MSB    36          //
//#define MBPARAM18_LSB    37          //

#define INCKEYREG       38           // INCREASE KEY - VALUE IN 10mS PERIODS
#define DECKEYREG       39           // INCREASE KEY - VALUE IN 10mS PERIODS
//#define MBPARAM19_MSB    38          //
//#define MBPARAM19_LSB    39          //
#define MBTABLEREVISION	40		// 40-47    Place for software revision in MB table (8 bytes)

*/
