
// P.Smith                     7/5/10
// generate file
//
// P.Smith                     30/6/10
// added WIDTH_CONFIG_PERCENTAGE_ALARM_BAND at 8517
//
// P.Smith                     7/7/10
// added auto ref calibrate,calibrate us from lf,us averaging,
// overrun, overrundb,calibrate ref,calibrate diameter,
// summary data for ultrasonic added at 8650
//
// P.Smith                     8/7/10
// added WIDTH_CONFIG_OPTION_STD_ON_AUTO,WIDTH_CONFIG_OPTION_HOUTLY_STD
// WIDTH_CONFIG_OPTION_ALARM_IN_MANUAL
//
// P.Smith                     9/7/10
// added decimal places multiplication factor and rounding to 1,2 decimal places.
//
// P.Smith                     19/7/10
// added M_DECIMAL_PLACES_MULTIPLICATION & I_DECIMAL_PLACES_MULTIPLICATION
// I_ROUND_DECIMAL_PLACES,M_ROUND_DECIMAL_PLACES
// added width offset and sign
//
// P.Smith                     16/9/10
// added fast correction enabled/disabled
//
// P.Smith                     20/9/10
// changed I_DECIMAL_PLACES_MULTIPLICATION to 1 decimal place.
// also I_ROUND_DECIMAL_PLACES to round to 1 decimal place
// WIDTH_SUMMARY_WIDTH define at 8640
//
// P.Smith                     23/9/10
// define PSUM_M version of rounding and multipliers
//
// P.Smith                     20/3/11
// add WIDTH_SUMMARY_AVERAGE_WIDTH at 8641,
// define PSUM_I_ONE_DECIMAL_PLACES_MULTIPLICATION
//
// M.McKiernan					20/4/2020
//mmk-wfh
//mmk-wfh
//#define  WIDTH_CONFIG_US_METHOD                 	8524   //
//#define	 US_METHOD_1				(0)						// Averaging 3 sensor lengths
//#define	 US_METHOD_2				(1)						// 3 points on circle method.

//#define  WIDTH_CONFIG_LENGTH_AB			            8525   //Length between sensors A & B
//#define  WIDTH_CONFIG_LENGTH_BC			            8526   //Length between sensors B & C
//#define  WIDTH_CONFIG_LENGTH_AC			            8527   //Length between sensors A & C
//#define  WIDTH_CONFIG_SENSOR_A_ANGLE	            8528   // sensor A angle. - degrees.
//#define  WIDTH_CONFIG_SENSOR_B_ANGLE	            8529   // sensor B angle.
//#define  WIDTH_CONFIG_SENSOR_C_ANGLE	            8530   // sensor C angle.
//wfh  27.04.2020
//#define  WIDTH_SUMMARY_D_METHOD2               		8642
//#define  WIDTH_SUMMARY_LF_METHOD2           			8643
//#define  WIDTH_SUMMARY_SPARE_8683					 8683	//
// wfh   Ultrasonic sensor status.
//#define WIDTH_SUMMARY_US_SENSOR_OK_1				8684	// OK => 1.
//#define WIDTH_SUMMARY_US_SENSOR_OK_2				8685	// OK => 1.
//#define WIDTH_SUMMARY_US_SENSOR_OK_3				8686	// OK => 1.
//#define WIDTH_SUMMARY_US_SENSOR_OK_4				8687	// OK => 1.

//#define WIDTH_SUMMARY_US_SENSOR4_TRIGS				8688		//debuggin triggers
//#define WIDTH_SUMMARY_US_SENSOR4_ECHOS				8689		//debuggin triggers
// 24.6.2020
// #define  WIDTH_SUMMARY_CALC_ANGLE_A           		8644		// calculated angle A,   degerees to 1 dp.
// #define  WIDTH_SUMMARY_CALC_ANGLE_B           		8645		// calculated angle B
// #define  WIDTH_SUMMARY_CALC_ANGLE_C           		8646		// calculated angle C.


//////////////////////////////////////////////////////
#ifndef __WIDTHMB_H__
#define __WIDTHMB_H__

#include <basictypes.h>

void CopyWidthSummmaryDataFromSSIF( void );
void CheckForStandardise( void );
void CopyWidthSummaryDataTOMB( void );
void CopyWidthConfigFromMB( void );
void CheckForWidthMBWrite( void );
void CopyWidthConfigToMB( void );
void CopyUltrasonicSummaryDataTOMB( void );

#define	M_DECIMAL_PLACES_MULTIPLICATION				(10.0f)
#define	PSUM_M_DECIMAL_PLACES_MULTIPLICATION		(1.0f)


#define	I_ONE_DECIMAL_PLACES						(10.0f)
#define	I_DECIMAL_PLACES_MULTIPLICATION				(10.0f)
#define	PSUM_I_DECIMAL_PLACES_MULTIPLICATION        (100.0f)

#define	PSUM_I_ONE_DECIMAL_PLACES_MULTIPLICATION     (10.0f)
#define	I_ROUND_TO_ONE_DECIMAL_PLACE				 (0.05f)

#define	I_ROUND_DECIMAL_PLACES				        (0.05f)
#define	PSUM_I_ROUND_DECIMAL_PLACES			        (0.005f)




#define	M_ROUND_DECIMAL_PLACES				        (0.05f)  //1dp
#define	PSUM_M_ROUND_DECIMAL_PLACES			        (0.5f)  //1dp


// width read definitions

#define  WIDTH_CONFIG_CALIBRATE_WIDTH               8500

#define  WIDTH_CONFIG_MAXIMUM_WIDTH                 8501
#define  WIDTH_CONFIG_MODE		                    8502   // layflat,ultrasonic,both
#define  WIDTH_CONFIG_TYPE		                    8503   // standard, small line
#define  WIDTH_CONFIG_LINE_TYPE		                8504   // blown film, ibc
#define  WIDTH_CONFIG_ALARM_BAND	                8505   // mm
#define  WIDTH_CONFIG_PROCESS_DELAY	                8506   //
#define  WIDTH_CONFIG_WIDTH_DEADBAND                8507   //
#define  WIDTH_CONFIG_NORMAL_INCREASE_CONTROL_GAIN  8508   //
#define  WIDTH_CONFIG_NORMAL_DECREASE_CONTROL_GAIN  8509   //

#define  WIDTH_CONFIG_FAST_CORRECTION_CONTROL_GAIN  8510   //
#define  WIDTH_CONFIG_WIDTH_AVERAGE_IN_SECONDS      8511   //
#define  WIDTH_CONFIG_WIDTH_CORRECTION_INTERVAL     8512   //
#define  WIDTH_CONFIG_US_AUTO_CALIBRATE_REFERENCE   8513   //
#define  WIDTH_CONFIG_US_AUTO_CAL_ULTRASONIC_FROM_LF 8514   //
#define  WIDTH_CONFIG_US_AUTO_CAL_US_INTERVAL       8515   //
#define  WIDTH_CONFIG_US_AUTO_CAL_US_SAMPLE_NO      8516   //
#define  WIDTH_CONFIG_US_AVERAGING      	        8517   //

#define  WIDTH_CONFIG_OVERRUN	         	        8518   //
#define  WIDTH_CONFIG_OVERRUN_DEADBAND     	        8519   //
#define  WIDTH_CONFIG_OPTION_STD_ON_AUTO   	        8520   //
#define  WIDTH_CONFIG_OPTION_HOUTLY_STD   	        8521   //
#define  WIDTH_CONFIG_OPTION_ALARM_IN_MANUAL        8522   //
#define  WIDTH_CONFIG_FAST_CORRECTION               8523   //
#define  WIDTH_CONFIG_US_METHOD                 	8524   //
#define	 US_METHOD_1				(0)						// Averaging 3 sensor lengths
#define	 US_METHOD_2				(1)						// 3 points on circle method.


//mmk-wfh
#define  WIDTH_CONFIG_LENGTH_AB			            8525   //Length between sensors A & B
#define  WIDTH_CONFIG_LENGTH_BC			            8526   //Length between sensors B & C
#define  WIDTH_CONFIG_LENGTH_AC			            8527   //Length between sensors A & C
#define  WIDTH_CONFIG_SENSOR_A_ANGLE	            8528   // sensor A angle. - degrees.
#define  WIDTH_CONFIG_SENSOR_B_ANGLE	            8529   // sensor B angle.
#define  WIDTH_CONFIG_SENSOR_C_ANGLE	            8530   // sensor C angle.

//#define  WIDTH_CONFIG_LENGTH_REF		            8528   //Reference Length for calibrating US sensors.
//#define  WIDTH_CONFIG_LAYFLAT_WIDTH		            8529   //Layflat width for calibration.
//#define  WIDTH_CONFIG_TARGET_DIAMETER	            8530   //Target Diameter for calibration.

//end mmk-wfh

#define  WIDTH_COMMAND_WIDTH_SETPOINT               8550

#define  WIDTH_COMMAND_AUTO_TOGGLE_COMMAND          8551
#define  WIDTH_COMMAND_INCREASE                     8552
#define  WIDTH_COMMAND_FAST_CORRECTION_INCREASE     8553
#define  WIDTH_COMMAND_DECREASE                     8554

// ultrasonic commands
#define  WIDTH_COMMAND_CALIBRATE_ULTRASONIC_REF     8555
#define  WIDTH_COMMAND_CALIBRATE_DIAMETER           8556

#define  WIDTH_COMMAND_US_LAYFLAT_OFFSET_SIGN       8557
#define  WIDTH_COMMAND_US_LAYFLAT_OFFSET            8558
#define  WIDTH_COMMAND_US_DIAMETER_FROM_LAYFLAT_ENTRY 8559


#define  MB_WIDTH_CONFIG_START                 		WIDTH_CONFIG_MAXIMUM_WIDTH
#define  MB_WIDTH_CONFIG_END                 		8549



// width control summary data
#define  WIDTH_SUMMARY_WIDTH_OFFSET            		8600
#define  WIDTH_SUMMARY_WIDTH_SCANNER           		8601
#define  WIDTH_SUMMARY_LEFT_WIDTH              		8602
#define  WIDTH_SUMMARY_RIGHT_WIDTH              	8603
#define  WIDTH_SUMMARY_LEFT_PULSES             		8604
#define  WIDTH_SUMMARY_RIGHT_PULSES             	8605
#define  WIDTH_SUMMARY_LEFT_LIMIT                	8606 //1- in limit
#define  WIDTH_SUMMARY_RIGHT_LIMIT                	8607 //1- in limit
#define  WIDTH_SUMMARY_LEFT_SENSOR_STATUS          	8608 //
#define  WIDTH_SUMMARY_RIGHT_SENSOR_STATUS          8609
#define  WIDTH_SUMMARY_LEFT_SENSOR_ATD          	8610
#define  WIDTH_SUMMARY_RIGHT_SENSOR_ATD          	8611
#define  WIDTH_SUMMARY_SSIF_TX_MESSAGES          	8612
#define  WIDTH_SUMMARY_SSIF_RX_MESSAGES          	8614
#define  WIDTH_SUMMARY_SSIF_RX_CHECKSUM_ERRORS      8616
#define  WIDTH_SUMMARY_SSIF_RX_TIMEOUTS          	8618

#define  WIDTH_SUMMARY_STANDARDISE_STATUS        	8620 // 1- in standardise
#define  WIDTH_SUMMARY_AUTO_STATUS               	8621 //0 - manual/1- auto
#define  WIDTH_SUMMARY_INCREASE_STATUS              8622 // 0- off/1-on
#define  WIDTH_SUMMARY_DECREASE_STATUS              8623 // 0- off/1-on
#define  WIDTH_SUMMARY_FAST_CORRECTION_STATUS       8624 // 0- off/1-on

// control info diagnostics

#define  WIDTH_SUMMARY_WIDTH_ERROR              	 8625
#define  WIDTH_SUMMARY_WIDTH_INTEGRATION           	 8626
#define  WIDTH_SUMMARY_CONTROL_AVERAGE_WIDTH       	 8627
#define  WIDTH_SUMMARY_CONTROL_AVERAGE_ERROR       	 8628
#define  WIDTH_SUMMARY_CONTROL_INCREASE_CORRECTION 	 8629
#define  WIDTH_SUMMARY_CONTROL_FCINCREASE_CORRECTION 8630
#define  WIDTH_SUMMARY_CONTROL_DECREASE_CORRECTION 	 8631
#define  WIDTH_SUMMARY_CONTROL_INCREASE_COUNTER 	 8632
#define  WIDTH_SUMMARY_CONTROL_FCINCREASE_COUNTER 	 8633
#define  WIDTH_SUMMARY_CONTROL_DECREASE_COUNTER 	 8634
#define  WIDTH_SUMMARY_CONTROL_PROCESS_DELAY_VALUE 	 8635
#define  WIDTH_SUMMARY_CONTROL_PROCESS_DELAY_CTR 	 8636
#define  WIDTH_SUMMARY_START_UP 	 				 8637
#define  WIDTH_SUMMARY_ALARM_STATUS	 				 8638
#define  WIDTH_SUMMARY_BUBBLE_BREAK_STATUS	 		 8639

#define  WIDTH_SUMMARY_WIDTH                 		8640
#define  WIDTH_SUMMARY_AVERAGE_WIDTH           		8641

//wfh
#define  WIDTH_SUMMARY_D_METHOD2               		8642
#define  WIDTH_SUMMARY_LF_METHOD2           		8643

// ultrasonic stuff
#define  WIDTH_SUMMARY_CALC_ANGLE_A           		8644		// calculated angle A,   degerees to 1 dp.
#define  WIDTH_SUMMARY_CALC_ANGLE_B           		8645		// calculated angle B
#define  WIDTH_SUMMARY_CALC_ANGLE_C           		8646		// calculated angle C.

#define  WIDTH_SUMMARY_US_ECHO_TIME_1 				 8650
#define  WIDTH_SUMMARY_US_ECHO_TIME_2 				 8652
#define  WIDTH_SUMMARY_US_ECHO_TIME_3 				 8654
#define  WIDTH_SUMMARY_US_ECHO_TIME_4 				 8656

#define  WIDTH_SUMMARY_US_AVERAGE_ECHO_TIME_1 	     8658
#define  WIDTH_SUMMARY_US_AVERAGE_ECHO_TIME_2 	     8660
#define  WIDTH_SUMMARY_US_AVERAGE_ECHO_TIME_3 	     8662
#define  WIDTH_SUMMARY_US_AVERAGE_ECHO_TIME_4 	     8664


#define  WIDTH_SUMMARY_US_COUNTS_PER_MM	             8666

#define  WIDTH_SUMMARY_US_DISTANCE_1 	     		 8667  //mm 1dp
#define  WIDTH_SUMMARY_US_DISTANCE_2 	     		 8668
#define  WIDTH_SUMMARY_US_DISTANCE_3 	     		 8669
#define  WIDTH_SUMMARY_US_DISTANCE_4 	     		 8670

#define  WIDTH_SUMMARY_US_DISTANCE_TO_CENTRE_1		 8671  //mm 1dp
#define  WIDTH_SUMMARY_US_DISTANCE_TO_CENTRE_2		 8672
#define  WIDTH_SUMMARY_US_DISTANCE_TO_CENTRE_3		 8673
#define  WIDTH_SUMMARY_US_DISTANCE_TO_CENTRE_4		 8674


#define  WIDTH_SUMMARY_US_RADIUS_1 	     		 	 8675  //mm 1dp
#define  WIDTH_SUMMARY_US_RADIUS_2 	     		 	 8676
#define  WIDTH_SUMMARY_US_RADIUS_3 	     		 	 8677

#define  WIDTH_SUMMARY_US_AVERAGE_RADIUS 		 	 8678 //mm 1dp
#define  WIDTH_SUMMARY_US_DIAMETER               	 8679 //mm 1dp

#define  WIDTH_SUMMARY_US_DIAMETER_WITH_OFFSET     	 8680 //mm 1dp
#define  WIDTH_SUMMARY_US_LAYFLAT_WIDTH_		  	 8681 //mm 1dp
#define  WIDTH_SUMMARY_US_LAYFLAT_WIDTH_WITH_OFFSET	 8682 //mm 1dp
#define  WIDTH_SUMMARY_SPARE_8683					 8683	//
// wfh   Ultrasonic sensor status.
#define WIDTH_SUMMARY_US_SENSOR_OK_1				8684	// OK => 1.
#define WIDTH_SUMMARY_US_SENSOR_OK_2				8685	// OK => 1.
#define WIDTH_SUMMARY_US_SENSOR_OK_3				8686	// OK => 1.
#define WIDTH_SUMMARY_US_SENSOR_OK_4				8687	// OK => 1.

#define WIDTH_SUMMARY_US_SENSOR4_TRIGS				8688		//debuggin triggers
#define WIDTH_SUMMARY_US_SENSOR4_ECHOS				8689		//debuggin triggers


#endif	//__WIDTHMB_H__
