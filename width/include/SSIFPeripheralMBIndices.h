///////////////////////////////////////////////////////
// SSIFperipheralMBIndices.h
//
// P.Smith                                  6/5/10
// SSIF MODBUS SPEC (MMK)
//
// P.Smith                                  20/9/10
// change SSIF_ADDRESS to 1, this will allow other devices like the sei to
// be on the same bus
///////////////////////////////////////////////////////


#ifndef __SSIFPERIPHERALMBINDICES_H__
#define __SSIFPERIPHERALMBINDICES_H__

// ******************************************
// SSIF - MODBUS INDICES
// ******************************************
#define	 SSIF_ADDRESS							(1)
//#define MAX_SSIF_CARDS                        (1)   // This will cater for a 1 scanner
// Writable parameters
#define  MB_SSIF_COMMAND                       (0)   // various command bits - definitions above.
// masks for the SSIF write command
//ToDo - copied from liw.
// once only operation - clears automatically.
#define SSIF_CMD_CLR_RESET_BIT                   0x0001  // ASM = CLEARRESETBITPOS        EQU     %00000001
#define SSIF_CMD_GO_TO_REMOTE_BIT                0x0002  // ASM = GOTOREMOTEBITPOS        EQU     %00000010
#define SSIF_CMD_STANDARDISE_BIT                 0x0004	 // STandardise both motors
#define SSIF_CMD_CLR_ALARM_BIT          		 0x0008  // CLEAR alarm byte.



//Most sign. Byte – continuous operation(1=on, 0=off):
#define SSIF_CMD_ALARM_RELAY _BIT                0x0100  // Alarm Relay       ;
#define SSIF_CMD_SCANNER_TYPE_BIT                0x0200  // 0=STANDARD, 1 = SMALL LINE
#define SSIF_CMD_MOTOR1_PWR_BIT             	 0x0400  //  Motor 1 power (1 = reduced)
#define SSIF_CMD_MOTOR2_PWR_BIT             	 0x0800  //  Motor 2 power (1 = reduced)

#define SSIF_CMD_HCOP1_BIT                       0x1000  //  HCOP1 (1=on, 0=off)
#define SSIF_CMD_HCOP2_BIT                       0x2000  //  HCOP1 (1=on, 0=off)
#define SSIF_CMD_HCOP3_BIT                       0x4000  //  HCOP1 (1=on, 0=off)
#define SSIF_CMD_HCOP4_BIT                       0x8000  //  HCOP1 (1=on, 0=off)

#define  MB_SSIF_MAX_TRAVEL_MOTOR1_LEFT_STEPS    (1)
#define  MB_SSIF_MAX_TRAVEL_MOTOR2_RIGHT_STEPS   (2)
#define  MB_SSIF_WIDTH_OFFSET_MM                 (3)
#define  MB_SSIF_WIDTH_OFFSET_STEPS              (4)
#define  MB_SSIF_MAX_WIDTH_MM                 	 (5)

#define  MB_SSIF_PARAM_6                         (6)	//UNUSED.

#define  MB_SSIF_MIN_WIDTH_STEPS               	 (7)
#define  MB_SSIF_WINDOW_LOW_ADC                  (8)	//Window comparator low value in a/d cts.
#define  MB_SSIF_WINDOW_HIGH_ADC                 (9)	//Window comparator high value in a/d cts.

#define  MB_SSIF_PARAM_10                        (10)	//UNUSED.
//Following 4 cmds Work only in Remote mode
#define  MB_SSIF_STEP_MOTOR1_N_CW                (11)	//Step Motor 1 N steps clockwise
#define  MB_SSIF_STEP_MOTOR1_N_CCW                (12)	//Step Motor 1 N steps anti-clockwise
#define  MB_SSIF_STEP_MOTOR2_N_CW                (13)	//Step Motor 2 N steps clockwise
#define  MB_SSIF_STEP_MOTOR2_N_CCW                (14)	//Step Motor 2 N steps c-clockwise

#define  MB_SSIF_PARAM_15                        (15)	//UNUSED.
#define  MB_SSIF_PARAM_16                        (16)	//UNUSED.
#define  MB_SSIF_PARAM_17                        (17)	//UNUSED.
#define  MB_SSIF_PARAM_18                        (18)	//UNUSED.
#define  MB_SSIF_PARAM_19                        (19)	//UNUSED.

// Read only parameters - PROD. SUMMARY.
#define  MB_SSIF_WIDTH_MM                        (20)	//NOT AVAILABLE - YET (COMPILER)
#define  MB_SSIF_STATUS                          (21)  // Alarm(msbyte) and Status byte.
//ToDo...
// masks for the SSIF status word.
//LS byte
#define SSIF_STATUS_RESET_BIT                0x0001 //
#define SSIF_STATUS_LOCALMODE_BIT            0x0002 //

#define SSIF_STATUS_MOTOR1_TRACKING_BIT      0x0004  // ASM =
#define SSIF_STATUS_MOTOR1_ON_EDGE_BIT       0x0008  // A
#define SSIF_STATUS_MOTOR1_STANDARDISING_BIT 0x0010  // A

#define SSIF_STATUS_MOTOR2_TRACKING_BIT      0x0020  // ASM =
#define SSIF_STATUS_MOTOR2_ON_EDGE_BIT       0x0040  // A
#define SSIF_STATUS_MOTOR2_STANDARDISING_BIT 0x0080  // A

//MS BYTE.
#define SSIF_STATUS_LEFT_MAX_TRAVEL			 0x0100 // Left Maximum Travel exceeded
#define SSIF_STATUS_RIGHT_MAX_TRAVEL		 0x0200 // RIGHT Maximum Travel exceeded
#define SSIF_STATUS_EEPROM_ERROR             0x0400  // ASM =

#define SSIF_STATUS_IN_TEST_MODE                0x8000  // b7 = Switch = 0, test mode.


#define  MB_SSIF_INPUTS                       (22)  // SSIF inputs
//LS byte
#define SSIF_INPUT_CONN1_1_BIT               0x0001      //
#define SSIF_INPUT_CONN1_2_BIT               0x0002      //

#define SSIF_INPUT_LIM1_BIT               	 0x0004      //
#define SSIF_INPUT_LIM2_BIT               	 0x0008      //

#define SSIF_INPUT_INA8_BIT               	 0x0010      //
#define SSIF_INPUT_INB8_BIT               	 0x0020      //

// MS byte
//       B1,b0  00 – sensor 1 uncovered
//                    01 – sensor on edge
//                    11 – sensor covered by film
#define SSIF_INPUT_EDGE1_0_BIT               0x0100     //
#define SSIF_INPUT_EDGE1_1_BIT               0x0200      //
#define SSIF_INPUT_MOT1_IN_STD_BIT           0x0400      // Limit (in standardise position) #1
#define SSIF_INPUT_MOT1_STD_BIT           	 0x0800      // Standardise flag  #1
//       B5,b4  00 – sensor2 uncovered
//                      01 – sensor on edge
//                      11 – sensor covered by film
#define SSIF_INPUT_EDGE2_0_BIT               0x1000     //
#define SSIF_INPUT_EDGE2_1_BIT               0x2000      //
#define SSIF_INPUT_MOT2_IN_STD_BIT           0x4000      // Limit (in standardise position) #2
#define SSIF_INPUT_MOT2_STD_BIT           	 0x8000      // Standardise flag  #2

#define  MB_SSIF_EDGE1_STEPS                  (23)  // Edge 1 (left) steps value
#define  MB_SSIF_EDGE2_STEPS                  (24)  // Edge 2 (RIGHT) steps value
#define  MB_SSIF_TOTAL_STEPS                  (25)  // TOTAL (23+24) steps value

#define  MB_SSIF_SENSOR1_ADC                  (26)  // SENSOR 1 A/D COUNTS value - 12 bits
#define  MB_SSIF_SENSOR2_ADC                  (27)  // SENSOR 2 A/D COUNTS value - 12 bits

#define  MB_SSIF_EDGE1_INST_STEPS             (28)  // Edge 1 (left) steps instantaneous value
#define  MB_SSIF_EDGE2_INST_STEPS             (29)  // Edge 2 (RIGHT) steps instantaneous value

#define  MB_SSIF_LTC2424_CH0              	  (30)  // A/D value for ch0 – LTC2424 - 16 bits
#define  MB_SSIF_LTC2424_CH1              	  (31)  // A/D value for ch1 – LTC2424 - 16 bits
#define  MB_SSIF_LTC2424_CH2              	  (32)  // A/D value for ch2 – LTC2424 - 16 bits
#define  MB_SSIF_LTC2424_CH3              	  (33)  // A/D value for ch3 – LTC2424 - 16 bits

#define  MB_SSIF_PARAM_34                     (34)	//UNUSED.
#define  MB_SSIF_PARAM_35                     (35)	//UNUSED.
#define  MB_SSIF_PARAM_36                     (36)	//UNUSED.
#define  MB_SSIF_PARAM_37                     (37)	//UNUSED.
#define  MB_SSIF_PARAM_38                     (38)	//UNUSED.
#define  MB_SSIF_PARAM_39                     (39)	//UNUSED.

// Read only parameters - ID.
#define  MB_SSIF_SOFTWARE_REVISION             (1000)   // Software rev. - 5 words, 10 ASCII
#define  MB_SSIF_BOARD_SERIAL_NO               (1005)   // board serial no. - 3 words, 48 bit silicon serial no.
#define  MB_SSIF_PARAM_1008                    (1008)	//UNUSED.
#define  MB_SSIF_PARAM_1009                    (1009)	//UNUSED.

#define  MB_SSIF_REVISION_DETAILS              (1000)  // board revision, software etc.
#define  MB_SSIF_REVISION_DETAILS_END          (1009)  //
#define  MB_SSIF_LAST_REGISTER                 (1009)  // board revision, software etc.

// Defs for read summary message
#define SSIF_SUMMARY_START       0
//#define SSIF_SUMMARY_START       MB_SSIF_WIDTH_MM	//MB_SSIF_STATUS
#define SSIF_SUMMARY_END         MB_SSIF_LTC2424_CH3

// Defs for write commands message
#define SSIF_COMMANDS_START          (MB_SSIF_COMMAND)
#define SSIF_COMMANDS_END            (MB_SSIF_COMMAND)

// Defs for write calibration message
#define SSIF_CALIBRATION_START          (MB_SSIF_MAX_TRAVEL_MOTOR1_LEFT_STEPS)
#define SSIF_CALIBRATION_END            (MB_SSIF_MIN_WIDTH_STEPS)

// Defs for read id message
#define SSIF_ID_START       	MB_SSIF_REVISION_DETAILS
#define SSIF_ID_END         	MB_SSIF_REVISION_DETAILS_END

#define SSIF_ID_OFFSET      	(SSIF_ID_START - (SSIF_SUMMARY_END +1) )      // Table in blender contiguous,
                                                                              // need to remove offset between summary and ID.

#define SSIF_TABLE_READ_SIZE  	( SSIF_ID_END - SSIF_ID_OFFSET + 1 )    // Complete table size in blender.
#define SSIF_TABLE_WRITE_SIZE  	( SSIF_CALIBRATION_END + 1 )  			// Complete table write size in blender.


#endif  // __SSIFPERIPHERALMBINDICES_H__
