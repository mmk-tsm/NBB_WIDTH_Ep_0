/////////////////////////////////////////////////
// General.h
//
// Include file containing general declarations
// used in more than one class or function.
//
// M.Parks                          26-07-1999
// M.McKiernan                          27-03-2001
// defined filter types.
// M.McKiernan                          03-08-2001
// LANGUAGES_MAX increased to 6, defined Italian and Finnish.
//  P.Smith                             03-05-03
//  Added MAINPITDIVISION defined as  2                     //
// M.McKiernan                          19-11-2003
// MAX_LAYERS increased to 6, previously 5.
// M.McKiernan                          03-09-2004
// Added def's for PERCENTAGEFILL and PERCENTAGEFILLBW
// M.McKiernan                          07-09-2004
// Corrected MINWEIGHT & added MINWEIGHTMEGA.  100g and 500g respectively.
// Added definitions for DDCLOSETIME & DDOPENTIME
// Added HILOFILTER50HZ
// Definitions for Process Alarms changed, bits defined on a 16 bit basis,
// because they are part of MB Process Alarm Word
//
// M.McKiernan                          16-09-2004
// Added more stage fill definitions:
//      MINTARGETWEIGHT, TWOSTAGEFILLINGID, THREESTAGEFILLINGID, MIN3RDFILLTARGET
//      Changed definition for STAGE_1 from 0 to 1.
//
// M.McKiernan                          07-10-2004
// Added AUTO/MANUAL definitions:
//  P.Smith                             09-11-2004
//  Added cleaning definitions and diagnostics password.
// M.McKiernan                          15-11-2004
// Added INC_DEC_MULTIPLIER SCEW_SPEED_SIGNAL_COMMS, SCEW_SPEED_SIGNAL_TACHO
// PAUSE_INPUT_TRIGGER, HW_PAUSEBIT
// M.McKiernan                          25-11-2004
// LSMONITORALARM     0x60      uses code previously used by EEPROMALARM  --REVIEW--
// Added ControlType definitions: CONTROL_TYPE_0_10 CONTROL_TYPE_INC_DEC
// Added DO_DIAGNOSTICS_PERIOD
// UPDATE_TIME_MINS  MIXER_SHUTOFF_MINS  MIXER_OFF_TIME,  MIN_ROTATION_PULSES_IN_10S
//
// M.McKiernan                          05-01-2005
// Added MAX_KGH_RR_SAMPLES
//
// P.Smith                              2-02-2005
// Added control definitions
//
// P.Smith                              1-03-2005
// Added Minimum line speed
//
//
// P.Smith                              16-03-2005
// SECONDSTOACCEPTSTALL,MINDAFOREXTRUDERSTALL defined
//
// P.Smith                              29-03-2005
// added HW_SCREWDETECTIONBIT

// P.Smith                              20-03-2005
// changed MIXSTOPBIT to MIXSTOPALARMBIT
//
// P.Smith                              6-04-2005
// Added SEITIMEOUTMAX
// Added HW_PAUSEOFFBIT
//
//
// P.Smith                              7-04-2005
// added MIXERTRANSITIONDELAY
// added ROLLCHANGEBOUNCETIMEINSECONDS
// added INCDEC_COUNTSPERSECOND_CONVERSION
//
//
// P.Smith                              7-04-2005
// added CUSTOMERCODECHARNO, MATERIALCODECHARNO
//
//
// P.Smith                              10-08-2005
// Added Single Recipe mode
// Added encryption option bits.
//
//
// P.Smith                              15-08-2005
// Added LICENSEALARMBITPOS,ALARMFLASHRATE
//
// P.Smith                              17-08-2005
// added IGNORELSALARMNO
//
// P.Smith                              30/08/2005
// changed MAX_COMPONENTS to 12
//
// P.Smith                              12/09/2005
// added LLSENBITPOS
//
// added batch sequence definitions for single recipe mode
//
// P.Smith                              23/9/05
// added LICENSEALARM
//
// P.Smith                              30/9/05
// added MULTI_STAGE_FILL_MINWEIGHT
//
// P.Smith                              11/10/05
// added license option bits
// remove original
// CONTROLDEADBANDPERCENTAGE & STARTUPCONTROLBANDPERCENTAGE defined as % eg 10.0f for 10 %
//
// P.Smith                              20/10/05
// added ALLOWLARGEBANDINCYCLES
//
// P.Smith                              15/12/05
// LICENSE_ONE_WEEK set to 168
//
// P.Smith                              21/12/05
// SEITIMEOUTMAX changed to 20 seconds
//
// P.Smith                              23/1/06
// initial pass at netburner development.
// removed #define MAX132_MAXIMUM_VALUE from this file
//
// P.Smith                              26/1/06
// defined HW_HIGHLSENSORBIT as 0x40 for nbb hardware
// defined HW_LOWLSENSORBIT as 0x80 for nbb hardware
// defined HW_SCREWDETECTIONBIT as 0x20
// defined HW_PAUSEBIT as 0x10
//
// P.Smith                              7/2/06
// define BLENDER_PIT_FREQUENCY as PIT_FREQUENCY / 2.5
// #define PIT_FIFTY_HZ_PERIOD     BLENDER_PIT_FREQUENCY/50
// #define PIT_TEN_HZ_PERIOD           BLENDER_PIT_FREQUENCY/10
// #define PIT_AD_POLL_PERIOD      BLENDER_PIT_FREQUENCY/15
//
// P.Smith                              7/2/06
// #define AD_C ONVERSION_RATE 			12
// This will allow the required averaging to be calculated from this definition
// #define	ATD_SECOND_AVERAGE				AD_CONVERSION_RATE;		//nbb
// #define	ATD_HALF_SECOND_AVERAGE			AD_CONVERSION_RATE/2;	//nbb
// #define	ATD_EIGHT_SECOND_AVERAGE		2;								//nbb leave this at 2
//
// P.Smith                              15/2/06
// put back in union CharsAndInt & union IntsAndLong to allow compilation.
// put back in IntsAndLong
//
// P.Smith                              21/2/06
//#include "NBBGpio.h"
//
// P.Smith                              23/2/06
// HW_LSENSORBIT define as 0x08 was defined incorrectly at 0x04
//
// P.Smith                              19/5/06
// added definition of MODBUSINCREASEDECREASEDTOAINCREMENT to 60        // MBDTOAINCREMENT EQU     60
// added EMAIL_STRING_SIZE definition
//
// P.Smith                              20/6/06
// define EMAIL_BUFFER_SIZE
// set NOMINAL_LATENCY to 0.082f
//
// P.Smith                              21/6/06
// add limits for batch size.
//
// P.Smith                              28/6/06
// added MINIMUM_DTOA_PER_KIL0
// added  MINIMUM_LENGTH
//
// P.Smith                              3/7/06
// MAX_LIMIT_CONTROL_AVERAGE_FACTOR defined
//
// P.Smith                              3/7/06
// added lastblendertype,laststagefill,lastcontroltype,lastdiagnosticsmode
// more limits for calibration data added.
//
// P.Smith                              14/7/06
// added MAXIMUM_CONTROL_LEVEL_SENSOR_ALARMS
//
// P.Smith                              19/7/06
// added LCOVERED TRUE, LUNCOVERED FALSE
//
// P.Smith                              5/12/06
// added EEPROMCOMMANDTIMEOUT
//
// P.Smith                              5/2/07
// added SPIEEACTIVETIMEOUT
//
// P.Smith                              5/2/07
// added MITTYTPRINTPERIOD, defined in milliseconds
// define link settings for LINKS_L1, LINKS_L2, LINKS_L3
// MAX_LIMIT_WEIGHT_AVERAGE_FACTOR added
// removed unused unions
//
// P.Smith                              7/3/07
// Added RECIPECHANGEDBIT
// Define FLOWRATERESETID
// Define zero line speed MINIMUMLINESPEED as 0.05 m/min was 0.25m/min, this was not low
// enough for some applications.
// define OPERATORPANELADDRESS at 1
//
// P.Smith                              8/3/07
// ultrasonic definitions added
//
// P.Smith                              13/3/07
// added USMAXENTRIES
//
// P.Smith                              19/4/07
// added mfprintf,miprintf,mtprintf
//
// P.Smith                              19/4/07
// added definitions for Multiblend.
// downloadhistory changed to 16 was 5
//
// P.Smith                              30/4/07
// define RUNNINGRECIPEFILENO as no file no 20 for now
// defined QUEUEINGINTERVAL
//
// P.Smith                              2/5/07
// RUNNINGRECIPEFILENO defined as 17, SOURCEOUTPUTNO & DEFINEDOUTPUTNO defined
//
// P.Smith                              9/5/07
// RESETRRNO set to 3, COMPONENT_1_FLOWRATE_REDUCED set to 20,RUNNINGRECIPEFILENO set to 1 for now
//
// P.Smith                              24/5/07
// change expansion definitions so that they run sequentially.
// change SOURCEOUTPUTNO to 20 to allow for cleans
//
// P.Smith                              30/5/07
// move mode MODE_SINGLERECIPEMODE to 11, and add grams per 12sq feet mode.
// correct read of links on NBB
// set RUNNINGRECIPEFILENO to 17
// set RECIPE_NAME_STRING_SIZE to 20
//
// P.Smith                              12/6/07
// added OPTIMISATIONENABLEBITPOS
//
// P.Smith                              22/6/07
// blender type naming corrected.
// CATCHBOXVIBRATIONDELAY set to 50 - 5 seconds.
//
// P.Smith                              4/7/07
// define full and empty
// change MITTYTPRINTPERIOD back to 90ms
//
// P.Smith                              20/7/07
// added purge definitions
//
// P.Smith                              2/8/07
// added MINIMUMLATENCY & NOMINAL_LATENCY
//
// P.Smith                              27/8/07
// Remove double definition of nominal latency.
//
// P.Smith                              15/10/07
// added DEFAULT_LOAD_ADDRESS, OVERRIDETIMEOUT
//
// P.Smith                              24/10/07
// define READINHIBITFORCONTROLINPUTS as 3
// this inhibits the read of sei inputs auto, frc,brc if they have become active
//
// P.Smith                              9/11/07
// make LASTCONTROLMODE 3, now individual bits are used.
//
// P.Smith                              13/11/07
// added GP12SQFTTOGPM2CONVERSIONFACTOR set to  1.1148360
// allow for new bit set up for control  mode.
// extrusion b0, haul off b1, voltage follower b2                  ;
// set lastcontrolmode to 6
// added linespeedavgdefault,linespeedavgmax,volumetricno  //nbb--todo check volumetricno                                                     ;
// define MAX_LINE_SPEED_AVERAGE & DEFAULT_LINE_SPEED_AVERAGE defined
//
// P.Smith                              19/11/07
// set SCREWSPEEDDEVIATIONLIMIT to 3%
// set VOLUMETRICESTIMATETIMEINSECONDS to 20
//
// P.Smith                              20/11/07
// LAST_SCREW_SPEED_SIGNAL_TYPE
// added MAX_PERIPHERAL_CARD_SETTING
// MAX_BATCHES_IN_KGHR_AVERAGE   10 ,DEFAULT_BATCHES_IN_KGHR_AVERAGE   3
// DEFAULT_BATCHES_IN_KGHR_AVERAGE   3
// LAST_OFFLINE_TYPE,MAX_PERIPHERAL_CARD_SETTING
// define MAXIMUMLATENCY,DEFAULT_MATERIAL_TRANSPORT_DELAY,DEFAULT_POST_MATERIAL_TRANSPORT_DELAY
//
// P.Smith                              29/11/07
// change INCDEC_COUNTSPERSECOND_CONVERSION to 10
//
// P.Smith                              5/12/07
// SUSPENDCONTROLNO added                                          ;
// Set HOLDOFFLSENSORALINSECONDS to 120
// added component percentage deviation alarm PERCENTAGEDEVIATIONALARM
// added SEIWRITEHOLDOFFTIMEINSECONDS, FULLCLEAN, MIXERCLEAN,LASTCLEAN
// COMPPERENABLEDWITHNOPAUSE,COMPPERENABLEDWITHPAUSE,LASTCOMPONENTPERRNABLE
// MAX_PAUSE_ON_ERROR_NO,DEFAULT_PAUSE_ON_ERROR_NO,INCREASEOUTPUTONTIME10HZ,DECREASEOUTPUTONTIME10HZ
//
// P.Smith                              9/1/08
// changed FLAPDEBOUNCETIME to 60 seconds 600
//
// P.Smith                              10/1/08
// AUTOCYCLELEVELSENSOROFFTIME10HZ defined as 20
//
// P.Smith                              11/1/08
// define SUPERFASTSETTLINGTIME as 5
// set SUPER_FAST_MATERIAL_FALL_TIME to 5 and SUPER_FAST_AFTER_TARE_TIME to 5
// set SUPER_FAST_AFTER_TARE_TIME to 5
//
// P.Smith                              15/1/08
// added HARDWARELICENSEALARM and HARDWARELICENSEALARMBITPOS
//
// P.Smith                              28/1/08
// correct definition of VENTURI_BAND_LIMIT, should be in kgs, not grams
// LICENSEALARM set back to 01
// PERCENTAGEDEVIATIONALARM set to AA
// LOWER_COMPONENT_PERCENTAGE_LIMIT changed to 10% (was 20)
// HIGHER_COMPONENT_PERCENTAGE_LIMIT changed to 5% (was 10)
// venturi band VENTURI_BAND_LIMIT changed to proper kg units
// MAX_CYCLE_DIAGNOSTICS_SAMPLES defined as 5000
//
//
// P.Smith                              28/1/08
// correct compiler warnings
// remove on/off/yes/no definition from this file, this is a duplication.
//
// P.Smith                              31/1/08
// added MB_TABLE_SETPOINT_SIZE
//
// P.Smith                              11/2/08
// added CHECKDOWNLOADOFDACNO
//
// P.Smith                              18/2/08
// defined COPY_DATA_FROM_SD_CARD, WRITE_DATA_TO_USB, WRITE_FINAL_BLOCK_TO_USB                 3
//
// P.Smith                              7/3/08
// define SENDCALDATATOSEINO as 3
//
// P.Smith                              14/3/08
// set PERCENTAGECHANGELIMITFORINCDEC to 15%  for inc dec limit
// MAXEXTRUDERTACHO set to 32768
// DEFAULT_BATCH_SIZE set to 2.9kgs
//
// P.Smith                              26/3/08
// added SAVETOEEPROMCYCLENO to indicate when the save to eeprom should take place.
// added NOOFWRITESTOEEPROM to determine no of writes on reset and after order change
//
// P.Smith                              7/5/08
// added BATCH_CALIBRATION_ENTRY_TYPE definitions
//
// P.Smith                              6/6/08
// define SD_CARD_OPERATION_SHOWN_TIME_SECONDS as 5
//
// P.Smith                              11/6/08
// added DOWNLOADMANAGERTIMEOUT50HZ
//
// P.Smith                              12/6/08
// define NOOFCYCLES_TODISABLEDAPERKG_DURINGCLEAN as 3
//
// P.Smith                              23/6/08
// VACDELAY set to 18 seconds, IGNORELSALARMNO set to 3, was 5
// set MAX_SD_CARD_FILES to 29 this was causing a problem in the copy
// of the modbus data by overwriting the modbus table sei version no.
//
// P.Smith                              27/6/08
// MAX_LIMIT_CONTROL_AVERAGE_FACTOR set to 8, add DEFAULT_LIMIT_CONTROL_AVERAGE_FACTOR
//
// P.Smith                              17/6/08
// remove MICRABATCH
// define NOOFSECONDSTOWRITETOSDCARD, and pages for panel
// SDCARDCOMMANDDELAYTIME set to half a second.
//
// P.Smith                              23/7/08
// move TIME_ARRAY_SIZE def to here
//
// P.Smith                              24/7/08
// set SDCARDACCESSDISABLEINSECS defined as 30 seconds
//
//
// P.Smith                              23/9/08
// TIMETOWEIGHHOPPEROPENCHECK defined as 5
// this is the time after which the amount of material dump from the weigh hopper has reduced by.
// added STARTLIQUIDCALIBRATIONPERCENTAGE defined as 0.25%
// added DUMPFLAPHASNOTOPENED defined as 300,FIRSTOFNEWALARMS,EMPTYBATCHPERCENTAGE
// STARTLIQUIDCALIBRATION,ENDLIQUIDCALIBRATION,RUNLIQUIDCALIBRATIONFORONEMINUTE
// TIMETOWEIGHHOPPEROPENCHECK,TIMETOWEIGHHOPPEROPENCHECK
// LIQUIDADDITIVEONTIME - time that liquid additive cal runs for.
// MAXIMUM_ALARMS set to 1000
//
// P.Smith                              3/10/08
// change MINIMUM_WEIGHT_CHANGE to 0.100
// define MAX_OFFLINE_MIXING_TIME,DEFAULT_OFFLINE_MIXING_TIME,MAX_MAX_THROUGHPUT
// DEFAULT_MAX_THROUGHPUT,MAX_FILL_RETRIES,DEFAULT_FILL_RETRIES,MAX_BYPASS_TIME_LIMIT
// DEFAULT_BYPASS_TIME_LIMIT,MAX_CLEAN_BYPASS_TIME_LIMIT,DEFAULT_CLEAN_BYPASS_TIME_LIMIT
//
// P.Smith                              16/10/08
// define ZEROLONGTERMRESETTABLECOUNTERSCOMMAND,ZEROLONGTERMCOUNTERSCOMMAND
//
// P.Smith                              24/10/08
// add SDCARDFAULTALARM & SDCARDFILENOTOPENED
//
// P.Smith                              29/10/08
// added first blender type
//
// P.Smith                              17/11/08
// TELNETTIMEOUTINMINS & TELNETWINDOWINPITS
//
// P.Smith                              18/11/08
// EVENTLOGSTRINGLENGTH set at 2000
//
// P.Smith                              25/11/08
// added ATODMAXEXCEEDEDALARM alarm
// added MAXLTC2415ATODVALUE
// add reset source definitions
// define ORDER_REPORT_BUFFER_SIZE as 2000
//
// P.Smith                              23/12/08
// defined USBTIMEOUT as 20 milliseconds
// defined DUMP_DEVICE_DISABLED,DUMP_DEVICE_USB,DUMP_DEVICE_PRINTER                       2
// LAST_DUMP_DEVICE_TYPE defined as DUMP_DEVICE_PRINTER
// USBFILENAMESTRINGLENGTH defined as 13
//
// P.Smith                              12/1/09
// added BLAST_ON_TIME_HZ
//
// P.Smith                              21/1/09
// added function codes from FUNCTION_1 to FUNCTION_55
// and FUNCTION_SUB_SECTION_1 to FUNCTION_SUB_SECTION_5
//
// P.Smith                              13/2/09
// changed  USBTIMEOUT to 50 * MILLISECONDS
// added USBLONGTIMEOUT 200 * MILLISECONDS
//
// P.Smith                              31/3/09
// increase max latency to 0.5
// PANEL_REVISION_STRING_SIZE, NBB_SERIAL_NUMBER_SIZE, NBB_SERIAL_NUMBER_SIZE
//
// P.Smith                              24/4/09
// added SEI_SOFTWARE_REVISION_STRING_SIZE
//
// P.Smith                              18/5/09
// change NBB_SERIAL_NUMBER_SIZE to 17
// add MAXIMUM_LINESPEED_CONSTANT,MINIMUM_LINESPEED_CONSTANT,NOMINAL_LINESPEED_CONSTANT
//
// P.Smith                              25/5/09
// defined I2CSTOPPEDALARM
//
// P.Smith                              8/6/09
// added COMPOPENTIMETOOSHORTALARM & COMPOPENTIMETOOLONGALARM
//
// P.Smith                              17/6/09
// put back in on / off definition for now
//
// P.Smith                              26/6/09
// add in empty / full status, used in vacuuming stuff.
// change CYCLECLEANBLOWINGTIME to 5 seconds
// add CYCLECLEANBLOWINGSHORTTIME time at 0.3 seconds.
//
// P.Smith                              30/6/09
// added ifndef for on / off status
//
// P.Smith                              22/7/09
// added pulsing definitions
//
// P.Smith                              25/8/09
// added OPTIMIX150, added INVALIDCONFIGDATA alarm
// added defaults,max, min batch sizes
// added MAXIMUM_COMPONENT_PULSES and PULSE_INCREMENT_TIME
//
// P.Smith                              1/9/09
// added BLENDER_LOCK_CODE,BLENDER_UNLOCK_CODE
// added MODBUS_EVENT_LOG_BUFFER_LENGTH as 10000
//
// P.Smith                              8/9/09
// added alarms ATODENDOFCONVNOTCOMPLETE,ATODNEGATIVEPOLARITYALARM,CALIBRATIONWRITEALARM
// PROCESSDATAWRITEALARM,CONFIGCHECKSUMALARM
// added MINWEIGHTTSM3000,MINIMUM_WEIGHT_TO_ALLOW_HOPPER_REFILL& TIME_FOR_ATOD_TO_SETTLE_AFTER_SDWRITE
//
// P.Smith                              10/9/09
// rename FASTSETTLINGTIME to NORMALSETTLINGTIME
//
// P.Smith                              15/9/09
// added INITIAL_PULSE_TIME_FOR_AUTOCYCLE,FILLING_METHOD_PULSING_MODE
//
// P.Smith                              17/9/09
// added VACUUMLOADERALARM, LOG_EVENTS_TO_SD_CARD,LOG_BATCH_LOG_TO_SD_CARD
// name change LICENSE_VACUUMLOADING
//
// P.Smith                              29/9/09
// MIN_LIMIT_SHUTOFFTIME change to 1 MINIMUM_PIT_OPEN_TIME defined as 5
//
// P.Smith                              17/10/09
// added ENDOFCYCLETIMEINSECONDS
//
// P.Smith                              16/11/09
// change SUPERFASTSETTLINGTIME to 8, this was causing a problem with the
// settling of the weight reading.
// change download manager to 40 seconds.
// define #define     MAX_STABILITY_READINGS & DELAY_BEFORE_STABILITY_DATA_READ
//
// P.Smith                              19/11/09
// added ATOD_REFERENCE_TIMEOUT_SECONDS
//
// P.Smith                              27/11/09
// TOPUPCOMPONENT defined as eight
//
// P.Smith                              7/12/09
// reduce RUN_STOP_FILTER to 1 second, this allows the stop / run to be recognised more quickly.
//
// P.Smith                              15/1/10
// added SD_CARD_NAME_SIZE

// M.McKiernan							8/2/2010
// #define TRANSFERSPFROM_EIP   	0xA7  //INDICATE TRANSFER SET POINTS FROM Ethernet/IP
//
// M.McKiernan							11/2/2010
// Added ETHERNETIP_ALARM			(314)
//
// P.Smith							17/2/2010
// set LICENSE_KGHROPTIONBIT to 0 for now, is currently unused.
// define LICENSE_ETHERNETIP as 04
//
// P.Smith							5/3/10
// added OPTIMIX50 and OPTIMIX250
// remove licensing stuff from here.
// change batch defaults to reflect the max batch sizes.
//
// P.Smith							9/3/10
// added BLENDER_WATCH_DOG_TIMER_ON_CODE & BLENDER_WATCH_DOG_TIMER_OFF_CODE
//
// P.Smith							11/3/10
// a problem has been seen where the max batch size check is failing due to the fact that a float is
// being used.
// the solution is to put an f after the definition of the max and increase the max
// greater than the nominal.
//
// P.Smith							15/3/10
// set SOFTWARE_WATCHTIME_SECONDS_TIMEOUT to 20 seconds
//
// P.Smith							6/4/10
// added FUNCTION_56 - 61
//
// P.Smith							15/4/10
// define PLATFORM_5270B
//
// P.Smith							4/5/10
// set LASTBLENDERTYPE to 250
//
//
// P.Smith							7/7/10
// define WMS_TRIGGER_TIME & WMS_OSCILLATION_TIME
// a problem was seen where a different sensor was used and because
// of the timing the times had to be extended.
//
// P.Smith							12/8/10
// added width control alarm.
// removed ultrasonic stuff from here, this has been moved to ultrasonicread.h
//
// P.Smith							20/9/10
// added WIDTH_SSIF_COMMS_ALARM

// M.McKiernan
// 30.6.2020
// Moved width alarms to 340's
//#define		US_SENSOR_1_ALARM				(341)	// Ultrasonic sensor #1 alarm.
//#define		US_SENSOR_2_ALARM				(342)	// Ultrasonic sensor #2 alarm.
//#define		US_SENSOR_3_ALARM				(343)	// Ultrasonic sensor #3 alarm.
//#define		US_SENSOR_4_ALARM				(344)	// Ultrasonic sensor #4 alarm.
//#define		US_SENSOR_5_ALARM				(345)	// Ultrasonic sensor #5 alarm.		// for sensor 5 if needed.

//#define		WIDTH_CONTROL_ALARM		  		(346)		// Width Control Alarm  	(was at 315)
//#define		WIDTH_SSIF_COMMS_ALARM    		(347)		// Width ssif comms alarm	(was at 316)


////////////////////////////////////////////////

//*****************************************************************************
// MACROS
//*****************************************************************************
#ifndef __GENERAL_H__
#define __GENERAL_H__


//#include "Hardware.h"
#include "NBBGpio.h"
#include    "Mtttymux.h"


#define MAX_LAYERS          6

#define MAX_COMPONENTS      12   // max dry components, add 1 to include PIB

// Supported languages
#define LANGUAGE_ENGLISH        0
#define LANGUAGE_FRENCH     1
#define LANGUAGE_SPANISH        2
#define LANGUAGE_PORTUGESE      3

#define LANGUAGE_ITALIAN        4
#define LANGUAGE_FINNISH        5

#define LANGUAGES_MAX           6

#define PROTOCOL_TSM            0
#define PROTOCOL_MODBUS     1

// Supported Filters
#define FILTER_TYPE_NONE        0
#define FILTER_TYPE_TSM     1
#define FILTER_TYPE_PC          2

#define FILTER_TYPES_MAX            3

#define ASCII_NULL              0x00

#define ASCII_LF                0x0A        // Line feed
#define ASCII_FF                0x0C        // Form feed
#define ASCII_CR                0x0D        // Carraige return

#define ASCII_BLANK             0x20        // ASCII space
#define ASCII_DECIMAL_POINT     0x2e
#define ASCII_DIGIT_ZERO        0x30


// General 1 second timer counts in PIT ticks
#define BLENDER_PIT_FREQUENCY   PIT_FREQUENCY/2  //PIT now at 3khz

#define GENERAL_TIMER_SECOND    BLENDER_PIT_FREQUENCY

#define MAINPITDIVISION         2

// following periods are from the PIT, assumed to be running at 1170Hz.

//nbb #define PIT_FIFTY_HZ_PERIOD     24
#define PIT_FIFTY_HZ_PERIOD     BLENDER_PIT_FREQUENCY/50

#define PIT_TEN_HZ_PERIOD           BLENDER_PIT_FREQUENCY/10
//nbb #define PIT_TEN_HZ_PERIOD           117

//nbb #define PIT_AD_POLL_PERIOD      80
#define AD_CONVERSION_RATE					12
#define PIT_AD_POLL_PERIOD      BLENDER_PIT_FREQUENCY/AD_CONVERSION_RATE

#define	ATD_ONE_SECOND_AVERAGE			AD_CONVERSION_RATE		//nbb
#define	ATD_HALF_SECOND_AVERAGE			(AD_CONVERSION_RATE/2)	//nbb
#define	ATD_TWO_SECOND_AVERAGE			(AD_CONVERSION_RATE*2)	//nbb
#define	ATD_EIGHT_SECOND_AVERAGE		2								//nbb leave this at 2



#define GPT_FREQUENCY               18000           // MAIN CLOCK DIV BY 256 PRESCALER /16. (i.e. (147mhz/2)/(256 *16)15990784 / 256 = 62464)

//bb
//nb #define MAX132_MAXIMUM_VALUE        279040      // maximum value for MAX132 a/d converter.
#define  AD_SAMPLES_PER_SECOND  16              // no.  of A/D samples per sec (used in the averaging process)

//bb - Display Page numbers for batch blender.
#define PRODUCTION_SUMMARY  1
#define CURRENT_RECIPE          10
#define CHANGE_TIME             20
#define INFORMATION_PAGE        30

#define  CALIBRATE_HOPPER       100         // use 100 - 199 for calibration/configuration programs
#define REVISION_DETAILS        200

#ifndef ON
#define  ON          TRUE
#endif

#ifndef OFF
#define  OFF        FALSE
#endif

//#define     YES     TRUE
//#define  NO         FALSE

#define     OPEN            TRUE
#define  CLOSED     FALSE

#define     ENABLED     TRUE
#define  DISABLED       FALSE

#define     ENABLE      TRUE
#define  DISABLE        FALSE

// for level sensors.
#define     COVERED         TRUE            // covered. - level sensor is active - input is 0
#define  UNCOVERED      FALSE

// For run stop switch.
#define     RUN     TRUE
#define  STOP       FALSE
// Automatic.
#define  AUTO       TRUE        //1
#define     MANUAL  FALSE       //0

#define     EMPTY           FALSE
#define     FULL            TRUE

// Diagnostic modes.

#define NO_DIAGNOSTICS_PROGRAM  0       //no diagnostics program.
#define AUTOCYCLE_PROGRAM       1   // ACPROG          EQU     1       ; AUTO CYCLE PROG
#define TEST_PROGRAM_1          2   // TESTPROG1       EQU     2 - output diagnostics.
#define TEST_PROGRAM_2          3   // TESTPROG2       EQU     3 -
#define TEST_PROGRAM_3          4   // TESTPROG3       EQU     4 -

#define  TSM1000         0       // STANDARD
#define  TSM1500         1       // EXTENDED
#define  TSM650          2       // MINI.
#define  TSM3000         3       // MEGA.
#define  OPTIMIX150      4       //
#define  TSM150          5       // MIDI
#define  TSM350          6       // COMPACT TYPE.
#define  TSM50           7       // BABY TYPE.
// new to be added
#define  OPTIMIX50       8       // OPTIMIX 50
#define  OPTIMIX250      9       // OPTIMIX 250


#define  FIRSTBLENDERTYPE TSM1000
#define  LASTBLENDERTYPE OPTIMIX250

#define MAX_BLENDER_TYPES   (BABYTYPE + 1)      // note start at 0.

#define REGRIND_BIT                     0x80
#define FILL_BY_WEIGHT_BIT          0x40
#define LEAVE_FILL_BY_WEIGHT_BIT    0x20
#define SEQDELIMITER                    0xAA            // Sequence table delimiter.
#define RESETRRNO                       3              //RESETRRNO               EQU     3      ; RESET ROUND ROBIN FOR CPI CALCULATION.

#define     CALC1STCMPTAR   1                               // CALC1STCMPTAR   EQU     1
#define     CALC            2                               // CALC            EQU     2
#define     NORMCALC        3                               // NORMCALC        EQU     3
#define     CALCMPNTAR      4                               // CALCMPNTAR      EQU     4

#define LFILLONTIME10HZ 30                          // LFILLONTIME10HZ         EQU     30      ; MODBUS TABLE ON TIME.

//Batch Sequence equates
#define  BATSEQ_AUTOCYCLETARE       1       // AUTOCYCLETARE   EQU     1
//#define  BATSEQ_CALCNTPINT          2       // CALCNTPINT      EQU     2
#define  BATSEQ_CALC                2       // CALC             EQU     2
#define  BATSEQ_LEVPOLL             3       // LEVPOLL         EQU     3       ; LEVEL SENSOR POLL MODE
#define  BATSEQ_DUMP                4       // DUMP            EQU     4       ; DUMP MODE OF OPERATION
#define  BATSEQ_TAREID              5       // TAREID          EQU     5
#define  BATSEQ_MATERIALFALL        6       // MATERIALFALL    EQU     6
#define  BATSEQ_AFTERTAREID         7       // AFTERTAREID     EQU     7       ; TIME BETWEEN TARING AND FILLING.
#define  BATSEQ_ENDCYCLEID          8       // ENDCYCLEID      EQU     8       ; END CYCLE NOW ;
#define  BATSEQ_FILL                9       // FILL            EQU     9
#define  BATSEQ_OFFDELBDMPID        10      // OFFDELBDMPID    EQU     10
#define  BATSEQ_OFFDELADMPID        11      // OFFDELADMPID    EQU     11
#define  BATSEQ_VACLOADING          12      // VACLOADING      EQU     12      ; VAC LOADING
#define  BATSEQ_NOFILL              13      // NOFILL          EQU     13
#define  BATSEQ_WAITFORTRAN         14      // WAITFORTRAN     EQU     14      ; WAIT FOR LS TRANSITION.
#define  BATSEQ_DELAYBFILL          15      // DELAYBFILL      EQU     15      ; DELAY BEFORE FILL.
#define  BATSEQ_PAUSEBFILL          16      // PAUSEBFILL      EQU     16      ; PAUSE MODE.
#define  BATSEQ_BYPASSID            17      // BYPASSID        EQU     17      ; BY PASS MODE
#define  BATSEQ_CONFIRMPAUSEID      18      // CONFIRMPAUSEID  EQU     18      ; CONFIRM
#define  BATSEQ_CONFIRMBYPASSID     19      // CONFIRMBYPASSID EQU     19      ; CONFIRM BYPASS.
#define  BATSEQ_VOLMODE             20      // VOLMODE         EQU     20      ; VOLUMETRIC MODE.
#define  BATSEQ_SETTLING            21      // SETTLING        EQU     21      ; HOPPER WT SETTLING.
#define  BATSEQ_MIXINGID            22      // MIXINGID        EQU     22      ; MIXING ID.
#define  BATSEQ_DOUBLEDUMPCLOSEID   23      // DOUBLEDUMPCLOSEID EQU   23
#define  BATSEQ_DOUBLEDUMPOPENID    24      // DOUBLEDUMPOPENID  EQU   24
#define  BATSEQ_DOUBLEDUMPFREQID    25      // DOUBLEDUMPFREQID  EQU   25

#define  BATSEQ_ACTIVATINGJET1ID      26    //   ACTIVATINGJET1ID        EQU     26      ; ACTIVATING JET 1
#define  BATSEQ_ACTIVATINGJET1PH2ID   27    //   ACTIVATINGJET1PH2ID     EQU     27
#define  BATSEQ_AIRJET1OFF1ID         28    //   AIRJET1OFF1ID           EQU     28
#define  BATSEQ_ACTIVATINGJET1PH3ID   29    //   ACTIVATINGJET1PH3ID     EQU     29
#define  BATSEQ_AIRJET1OFF2ID         30    //   AIRJET1OFF2ID           EQU     30
#define  BATSEQ_ACTIVATINGJET2ID      31    //   ACTIVATINGJET2ID        EQU     31      ; ACTIVATING JET 2
#define  BATSEQ_CLOSEDUMP1ID          32    //   CLOSEDUMP1ID            EQU     32      ; CLOSE DUMP NTH TIME
#define  BATSEQ_OPENDUMP1ID           33    //   OPENDUMP1ID             EQU     33      ; OPEN DUMP NTH TIME
#define  BATSEQ_CLOSEDUMP2ID          34    //   CLOSEDUMP2ID            EQU     34
#define  BATSEQ_OPENDUMP2ID           35    //   OPENDUMP2ID             EQU     35      ;
#define  BATSEQ_SETTLEAFTERBANGID     36    //   SETTLEAFTERBANGID       EQU     36      ; SETTLING AFTER BANG
#define  BATSEQ_DELAYAFTERCLEANID     37    //   DELAYAFTERCLEANID       EQU     37
#define  BATSEQ_AIRJET2OFFID          38    //   AIRJET2OFFID            EQU     38
#define  BATSEQ_CCLACTIVEID           39    //   CCLACTIVEID             EQU     39      ; LEFT CC
#define  BATSEQ_CCLOFFID              40    //   CCLOFFID                EQU     40      ; LEFT CC OFF
#define  BATSEQ_CCRACTIVEID           41    //   CCRACTIVEID             EQU     41      ; RIGHT CC
#define  BATSEQ_CCROFFID              42    //   CCROFFID                EQU     42      ; RIGHT CC OFF


// settling times                                     //
#define NORMALSETTLINGTIME              8           //FASTSETTLINGTIME EQU     8   ; SETTLING TIME FOR FILLING.
#define SUPERFASTSETTLINGTIME           8           //
#define AUTOCYCLEDELAY                  40          // AUTOCYCLEDELAY  EQU     40       ; AUTOCYCLER TARE DELAY

// Hardware SBBIO input bit definitions
#define  HW_BUBBLEBREAKBIT  0x02                    //BUBBLEBREAKBIT  EQU     %00000010       ; BUBBLE BREAK BIT


#define  HW_RUNSTOPBIT       0x01                   //nbb IP1 RUNSTOPBIT      EQU     %00000001
#define  HW_CINBIT           0x02                   //nbb IP2 CINBIT          EQU     %00000010
#define  HW_PAUSEOFFBIT      0x04                   //nbb IP3 --review-- this output IO2CONN60BIT    EQU     %10000000       ; CONN60 IO2
#define  HW_LSENSORBIT       0x08                   //nbb IP4 LSENSORBIT      EQU     %00000100       ; LEVEL SENSOR INPUT.
#define  HW_PAUSEBIT         0x10                   //nbb  IP5 modified for nbb hardware IO3CONN60BIT    EQU     %00100000         ; pAUSE INPUT.
#define  HW_SCREWDETECTIONBIT   0x20                //nbb IP6 modified for nbbb hardware IO1CONN60BIT    EQU     %01000000       ; CONN60 IO1
#define  HW_HIGHLSENSORBIT  0x40                    //nbb IP7 modified for nbb hardware HIGHLSENSORBIT  EQU     %00001000        ; HIGH LEVEL SENSOR BIT.
#define  HW_LOWLSENSORBIT   0x80                    //nbb  IP8 modified for nbb hardware LOWLSENSORBIT   EQU     %00010000        ; HIGH LEVEL SENSOR BIT.

#define LEVSEN1LAG               40                 //LEVSEN1LAG      EQU     40      ; LEVEL SENSOR #1 LAG
#define LEVEL_SENSOR_OFF_TIME 50                    // LEVELSENSOROFFTIME      EQU     50              ; ONE SECOND
#define NO_OF_ESTIMATED_CYCLES   3                  //NOESTIMATEDCYCLES       EQU  3       ; NO OF ESTIMATED CYCLES SET

// Blender modes.
#define MODE_BLENDINGONLY         0             //BLENDINGONLY    EQU     0       ; BLENDING ONLY.
#define MODE_OFFLINEMODE          1             //OFFLINEMODE     EQU     1       ; OFFLINE MODE.
#define MODE_THROUGHPUTMON        2             //THROUGHPUTMON   EQU     2       ; THROUGHPUT MONITOR
#define MODE_MAXTHROUGHPUTZT      3             //MAXTHROUGHPUTZT EQU     3       ; 0 - 10 VOLTS FOR MAX THROUGHPUT.
#define MODE_KGHCONTROLMODE       4             //KGHCONTROLMODE  EQU     4       ; KG/H CONTROL.
#define MODE_GPMGPMENTRY          5             //GPMGPMENTRY     EQU     5       ; G/M CONTROL WITH G/M ENTRY
#define MODE_GPMGPMENXXKGS        6             //GPMGPMENXXKGS   EQU     6       ; G/M CONTROL WITH G/M ENTRY
#define MODE_GPMWTPLENTRY         7             //GPMWTPLENTRY    EQU     7       ; G/A CONTROL WITH WT/ LENGTH ENTRY.
#define MODE_GPMMICRONENTRY       8             //GPMMICRONENTRY  EQU     8       ; G/M CONTROL WITH MICRON ENTRY.
#define MODE_GPSQMENTRY           9             //GPSQMENTRY      EQU     9       ; G/MM CONTROL WITH SP ENTRY.
#define MODE_GP12SQFEET           10            //GPSQMENTRYIMP   EQU     10      ; G/MM ENTRY IN GRAMS / 12SQ FEET
#define MODE_SINGLERECIPEMODE     11            //SINGLERECIPEMODE EQU    10      ; SINGLE RECIPE MODE

#define LASTBLENDERMODE      MODE_SINGLERECIPEMODE      //LASTBLENDERMODE EQU     SINGLERECIPEMODE ; LAST ENTRY

// Autocycler
#define AUTOCYCLE_INITIAL_TARE_VALUE    20187       // tare counts for autocycler.

#define TWO                         2
#define THREE                           3
#define FOUR                            4
#define EIGHT                           8

#define FILLFIRST                 1         //FILLFIRST               EQU     1   ; FILL COLOUR
#define FILLCOLOUR                2         //FILLCOLOUR              EQU     2   ; FILL COLOUR
#define TOPUPCOLOUR               3         //TOPUPCOLOUR             EQU     3   ; TOP UP COLOUR.
#define FIRSTCOMPFILL             4         //FIRSTCOMPFILL           EQU     4   ; FIRST COMP FILL
#define NORMALOPERATION           5         //NORMALOPERATION         EQU     5   ; BACK TO NORMAL CYCLE

// Pauses defined
//#define PAUSEATENDOFCYCLE            1        //PAUSEATENDOFCYCLE        EQU    1               ; INDICATE PAUSE AT END OF CYCLE.
//#define IMMEDIATEPAUSE               2        //IMMEDIATEPAUSE           EQU    2               ; IMMEDIATE PAUSE
//#define OFFLINEPAUSEATENDOFCYCLE     3        //OFFLINEPAUSEATENDOFCYCLE EQU    3               ; PAUSE AT END OF CYCLE.
// Pause flag bit definitions
#define NORMAL_PAUSE_BIT                        0x0001  // NORMALPAUSE  Normal Pause mode
#define END_OF_CYCLE_PAUSE_BIT              0x0001  // End of cycle pause mode


#define OFFLINE_END_OF_CYCLE_PAUSE_BIT  0x0002  // Offline End of cycle pause mode  OFFLINEPAUSEATENDOFCYCLEBIT EQU %00000010       ; OFFLINE PAUSE BIT
#define ANY_PAUSE_BITS                          0x0003  // PAUSEBITS any pause mode
#define VAC_INHIBIT_PAUSE_BIT               0x0004  // VACINHIBIT - VAC inhibits blender.

// CYCLEPAUSETYPE definitions
#define NORMAL_PAUSE_TYPE           1           // NORMALPAUSE
#define END_OF_CYCLE_PAUSE_TYPE 1           // PAUSEATENDOFCYCLE        EQU    1     ; INDICATE PAUSE AT END OF CYCLE.
#define IMMEDIATE_PAUSE_TYPE        2           // IMMEDIATEPAUSE
#define OFFLINE_END_OF_CYCLE_PAUSE_TYPE 3   // OFFLINEPAUSEATENDOFCYCLE EQU    3   ; PAUSE AT END OF CYCLE.


#define TRANSFERSPID            0xAB            //TRANSFERSPID            EQU     $AB; CODE TO INDICATE THAT SET CURRENT ORDER SETPOINTS SHOULD BE TRANSFERRED.
#define TRANSFERSPFROMCOMMSID   0xA5            //TRANSFERSPFROMCOMMSID   EQU     $A5; INDICATE TRANSFER SET POINTS FROM COMMS.
#define TRANSFERSPFROM_EIP   	0xA7            //INDICATE TRANSFER SET POINTS FROM Ethernet/IP


#define     STAGE_1_MFILLRATE       0x06
#define     STAGE_2_MFILLRATE       0x05
#define     STAGE_3_MFILLRATE       0x05

#define     COMPONENT_1_FLOWRATE        28
#define     COMPONENT_1_FLOWRATE_REDUCED 20
#define     COMPONENT_2_FLOWRATE        7
#define     COMPONENT_3_FLOWRATE        7
#define     COMPONENT_4_FLOWRATE        2
#define     COMPONENT_5_FLOWRATE    2
#define     COMPONENT_5_FLOWRATE_REDUCED    1


#define     STAGE_3                     3
#define     STAGE_2                     2
#define     STAGE_1                     1
#define     STAGE_0                     0               // 0 => no multi-stage filling.
#define     SINGLE_STAGE_FILLING        0               // 0 => no multi-stage filling.

#define     TOPUP_COMPONENT         8

#define     FILLBYWEIGHT            0x40        //FILLBYWEIGHT    EQU     %01000000
#define     LEAVEFILLBYWEIGHT    0x20   //LEAVEFILLBYWEIGHT EQU   %00100000 ; LEAVE FILL BY WEIGHT.
#define     RECIPECHANGEDBIT     0x10   //RECIPECHANGEDBIT    EQU     %00010000 ; INDICATE THAT RECIPE HAS CHANGED.


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
#define     LICENSEALARM        0x01      //.
#define     PERCENTAGEDEVIATIONALARM  0xAA      //.

#define     DUMPFLAPHASNOTOPENED      (300)
#define     SDCARDFAULTALARM          (301)
#define     ATODMAXEXCEEDEDALARM      (302)
#define     I2CSTOPPEDALARM           (303)
#define     COMPOPENTIMETOOSHORTALARM (304)
#define     COMPOPENTIMETOOLONGALARM  (305)
#define     INVALIDCONFIGDATA         (306)
#define     ATODENDOFCONVNOTCOMPLETE  (307)
#define     ATODNEGATIVEPOLARITYALARM (308)
#define     HOPPERCALIBRATEDATZEROALARM (309)


#define     CALIBRATIONWRITEALARM     (310)
#define     PROCESSDATAWRITEALARM     (311)
#define     CONFIGCHECKSUMALARM       (312)
#define     VACUUMLOADERALARM         (313)
#define		ETHERNETIP_ALARM		  (314)		// ALARM FOR Ethernet/IP
//#define		WIDTH_CONTROL_ALARM		  (315)		// Width Control Alarm
//#define		WIDTH_SSIF_COMMS_ALARM    (316)		// Width ssif comms alarm


//wfh   Important that sensor alarms are consecutive.
#define		US_SENSOR_1_ALARM				(341)	// Ultrasonic sensor #1 alarm.
#define		US_SENSOR_2_ALARM				(342)	// Ultrasonic sensor #2 alarm.
#define		US_SENSOR_3_ALARM				(343)	// Ultrasonic sensor #3 alarm.
#define		US_SENSOR_4_ALARM				(344)	// Ultrasonic sensor #4 alarm.
#define		US_SENSOR_5_ALARM				(345)	// Ultrasonic sensor #5 alarm.		// for sensor 5 if needed.

#define		WIDTH_CONTROL_ALARM		  		(346)		// Width Control Alarm
#define		WIDTH_SSIF_COMMS_ALARM    		(347)		// Width ssif comms alarm




#define     MAXLTC2415ATODVALUE       262000

#define     SDCARDFILENOTOPENED       (1)



#define     HARDWARELICENSEALARM      0xF3      //.

#define     FIRSTOFNEWALARMS          DUMPFLAPHASNOTOPENED
#define     EMPTYBATCHPERCENTAGE      (10)    //





//nbb bits defined on a 16 bit basis, because they are part of MB Process Alarm Word.
#define     CONTROLALARMBIT     0x8000        //CONTROLALARMBIT EQU     %10000000    EXTRUDER CONTROL
#define     LEVELSENALBIT       0x4000        //LEVELSENALBIT   EQU     %01000000    LEVEL SENSOR ALARM BIT.
#define     TAREALBIT           0x2000        //TAREALBIT       EQU     %00100000    TARE ALARM BIT.
#define     STALLALARMBIT       0x1000        //STALLALARMBIT   EQU     %00010000    STALLED ALARM BIT.
#define     WTCHANGEALBIT       0x0800        //WTCHANGEALBIT   EQU     %00001000    WT CHANGE ALARM BIT
#define     PERALBIT            0x0400        //PERALBIT        EQU     %00000100    % = 100 %.
#define     CALINVALARMBIT      0x0200        //CALINVALARMBIT  EQU     %00000010    CALIBRATION DATA INVALID
#define     LSMONITORALARMBIT   0x0100        //LSMONITORALARMBIT EQU     %00000001  ; LEVEL SENSOR MONITOR ALARM..

#define     MIXSTOPALARMBIT      0x0001        //MIXSTOPBIT      EQU     %00000001       ;
#define     ZEROLSBIT            0x0002        //ZEROLSBIT       EQU     %00000010       ; ZEROLS ALARM.
#define     SHUTDOWNALARMBIT     0x0004        //SHUTDOWNALARMBIT EQU    %00000100       ; SHUTDOWN ALARM BIT
#define     FILLALARMBIT         0x0008        //FILLALARMBIT    EQU     %00001000       ; FILL BIT
#define     LOWLEVALBIT          0x0010        //LOWLEVALBIT     EQU     %00010000       ; LOW LEVEL ALARM BIT.
#define     SEICOMMSALARMBIT     0x0020        //SEICOMMSALARMBIT EQU    %00100000       ; SEI COMMS ALARM BIT
#define     ATTARWTALARMBIT      0x0040        //ATTARWTALARMBIT EQU     %01000000       ; AT TARGET WEIGHT

// b0 - Mixer motor stopped, b1 - Zero linespeed, b2 - Blender Shutdown , b3 - Fill alarm,
// b4 - Low level in reservoir, b5 - SEI communications,
//b8  - LEVEL SENSOR MONITOR ALARM
// b9  - Calibration data invalid, b10 - Invalid setpoint data, setpoint %'s not = 100%, b11 - Weight change in hopper weight,
// b12 - extruder stall, b13 - Weigh hopper not empty,
//b14 - Level sensor uncovered before filliing completed, b15 - Extrusion control control alarm.

#define     TIME_BEFORE_TARE            10          // TIMEBTARE       EQU     10       ; TIME (10THS SECONDS) BEFORE TARE AFTER CLOSING DUMP
#define     SUPER_FAST_TIME_BEFORE_TARE  5          // TIMEBTARE       EQU     10       ; TIME (10THS SECONDS) BEFORE TARE AFTER CLOSING DUMP

#define     MATERIAL_FALL_TIME          10          // MATFALLTIME     EQU     10     ; MATERIAL FALL TIME
#define     SUPER_FAST_MATERIAL_FALL_TIME 5      //

#define     AFTER_TARE_TIME         10      // AFTERTARETIME   EQU     10
#define     SUPER_FAST_AFTER_TARE_TIME  5      //
#define     MICRA_AFTER_TARE_TIME   20          // MICRAAFTERTARETIME EQU  20

#define     MAX_TARE_TIME            200        //MXTARTIM        EQU     200 ; MAX TARE TIME.
#define     MAX_TARE_TIME_MEGA      400     //MXTARTIMMEGA    EQU     400 ; MAX TARE TIME SET TO 40 SECONDS

#define     FILLTIMEOUT     30       //FILLTIMEOUT     EQU     20  FILL TIMEOUT (SECONDS).
#define     FILLTIMEOUTG4   50      // FILLTIMEOUTG4   EQU     50
#define     FILLTIMEOUTG5P  100     // FILLTIMEOUTG5P  EQU     100
#define     VACDELAY        180     // VACDELAY        EQU     250

#define     THREEINCHLATENCY    50   //THREEINCHLATENCY        EQU     0050
#define     TWOINCHLATENCY      40   //TWOINCHLATENCY          EQU     0040
#define     VENTURILATENCY      1   //VENTURILATENCY          EQU     0001

#define     OFFLINETYPE_HILO      0       //OFFLINETYPEHILO EQU     0       ; OFFLINE MODE HIGH AND LOW LEVEL SENSORS.
#define     OFFLINETYPE_XXKG      1       //OFFLINETYPEXXKG EQU     1       ; OFFLINE MODE BLEND UNTIL XX KGS.
#define     LAST_OFFLINE_TYPE     OFFLINETYPE_XXKG

// CLEANING PHASE DEFINITIONS
#define     CLEANPHASE1ID               1       //CLEANPHASE1ID           EQU     1       ; CLEANING CYCLE PHASE 1
#define     CLEANPHASE2ID               2       //CLEANPHASE2ID           EQU     2       ; CLEANING CYCLE PHASE 2

#define     BATSEQ_ACTIVATINGJET1ID            26      //ACTIVATINGJET1ID        EQU     26      ; ACTIVATING JET 1

#define     BATSEQ_ACTIVATINGJET1PH2ID         27      //ACTIVATINGJET1PH2ID     EQU     27
#define     BATSEQ_AIRJET1OFF1ID               28      //AIRJET1OFF1ID           EQU     28

#define     BATSEQ_ACTIVATINGJET1PH3ID         29      //ACTIVATINGJET1PH3ID     EQU     29
#define     BATSEQ_AIRJET1OFF2ID               30      //AIRJET1OFF2ID           EQU     30

#define     BATSEQ_ACTIVATINGJET2ID            31      //ACTIVATINGJET2ID        EQU     31      ; ACTIVATING JET 2

#define     CLOSEDUMP1ID                32      //CLOSEDUMP1ID            EQU     32      ; CLOSE DUMP NTH TIME
#define     OPENDUMP1ID                 33      //OPENDUMP1ID             EQU     33      ; OPEN DUMP NTH TIME

#define     CLOSEDUMP2ID                34      //CLOSEDUMP2ID            EQU     34
#define     OPENDUMP2ID                 35      //OPENDUMP2ID             EQU     35      ;
#define     SETTLEAFTERBANGID           36      //SETTLEAFTERBANGID       EQU     36      ; SETTLING AFTER BANG
#define     DELAYAFTERCLEANID           37      //DELAYAFTERCLEANID       EQU     37
#define     AIRJET2OFFID                38      //AIRJET2OFFID            EQU     38
#define     CCLACTIVEID                 39      //CCLACTIVEID             EQU     39      ; LEFT CC
#define     CCLOFFID                    40      //CCLOFFID                EQU     40      ; LEFT CC OFF
#define     CCRACTIVEID                 41      //CCRACTIVEID             EQU     41      ; RIGHT CC
#define     CCROFFID                    42      //CCROFFID                EQU     42      ; RIGHT CC OFF

#define     OFFDELAY1      20     //OFFDELAY1       EQU     20       ; 2 SECONDS
#define     OFFDELAY2      100    //OFFDELAY2       EQU     100      ; DELAY BEFORE MATERIAL RELEASE.

#define     MINWEIGHT      100      //MINWEIGHT       EQU     $0010   ; MINIMUM WEIGHT ; MINIMUM WEIGHT   (assume 100g)
#define     MINWEIGHTTSM3000  500      //MINWEIGHTMEGA   EQU     $0050   ; MINIMUM WEIGHT

#define         RUN_STOP_FILTER 10    //RSTOPFILTER     EQU     20 RUN STOP FILTER (for stop/run switch debounce)

#define     MIN_VALVE_OPEN_TIME            150      //MINVALVEOPENTIME        EQU     150           ; MINIMUM OPENTIME FOR SLIDE VALVE
#define     MIN_VENTURI_OPEN_TIME          30       //MINVENTURIOPENTIME      EQU     30
#define     MAX_MIN_VALVE_OPEN_TIME         150         //MAXMINVALVEOPENTIME     EQU     150           ; MINIMUM OPENTIME FOR SLIDE VALVE

#define     MAX_RETRY_NO                2       // MAXRETRYNO              EQU     2        ; NO OF RETRIES BEFORE INTRODUCING MINIMUM FILL
#define     MAX_RETRY_CYCLES            5           // MAXRETRYCYCLES          EQU     5

#define     LOWER_TARGET_LIMIT                    40 //LOWTARGETLIMIT          EQU     $40
#define     LOWER_COMPONENT_PERCENTAGE_LIMIT      10 //LOWERCOMPPERLIMIT       EQU     $20      ; LOWER COMPONENT % LIMIT.
#define     HIGHER_COMPONENT_PERCENTAGE_LIMIT     05 //HIGHERCOMPPERLIMIT      EQU     $10      ; HIGHER COMPONENT % LIMIT.
#define     VENTURI_BAND_LIMIT           0.005          //VENTURIBANDLIMIT        EQU     $0050    ; ALLOW HIGHER BAND BELOW XX GRAMS

#define     MIN_CPI_VALUE       0.01        // New C definition (min allowable CPI value --review-- (as necessary).
#define     CPI_THRESHOLD     3                    // CPITHRESHOLD    EQU     3   ; CPI THRESHOLD

// Stage Filling
#define     PERCENTAGEFILL      70          //PERCENTAGEFILL  EQU     $70              ; % FILL
#define     PERCENTAGEFILLBW    50          //PERCENTAGEFILLBW EQU    $50             // for fill by weight.

#define     MINTARGETWEIGHT      150       // MINTARGETWEIGHT EQU     $0150
#define     TWOSTAGEFILLINGID    1         // TWOSTAGEFILLINGID EQU   1               ; TWO STAGE FILLING ID
#define     THREESTAGEFILLINGID  2         // THREESTAGEFILLINGID EQU 2               ; THREE STAGE FILL ID
#define     LASTSTAGEFILL        THREESTAGEFILLINGID
#define     MIN3RDFILLTARGET     70        // MIN3RDFILLTARGET  EQU   $0070           ; MINIMUM FILL FOR STAGE 3 TARGET.



#define     DDCLOSETIME         10          //DDCLOSETIME     EQU     10      ; CLOSE TIME
#define     DDOPENTIME          10          //DDOPENTIME      EQU     10      ; OPEN TIME
// clean time definitions (in tenths of second)
#define     CLOSEDUMP1TIMETENTHS            10   //CLOSEDUMP1TIME10S       EQU     0010
#define     AIRJET1ONTIMETENTHS             30   //AIRJET1ONTIME10S        EQU     0030
#define     AIRJET1OFF1TIMETENTHS           10   //AIRJET1OFF1TIME10S      EQU     0010
#define     AIRJET1OFF2TIMETENTHS           10   //AIRJET1OFF2TIME10S      EQU     0010
#define     AIRJET1ONTIMEPH2TENTHS          30   //AIRJET1ONTIMEPH210S     EQU     0030
#define     AIRJET1ONTIMEPH3TENTHS          30   //AIRJET1ONTIMEPH310S     EQU     0030
#define     AIRJET2ONTIMETENTHS             50   //AIRJET2ONTIME10S        EQU     0050
#define     OPENDUMP1TIMETENTHS             10   //OPENDUMP1TIME10S        EQU     0010
#define     SETTLEAFTERBANGTIMETENTHS       20   //SETTLEAFTERBANGTIME10S  EQU     0020


#define     HILOFILTER50HZ       20                 //HILOFILTER50HZ          EQU     20     ; FILTER FOR SENSORS. 20 * 20mS = 400mS

#define         MINIMUM_WEIGHT_CHANGE   0.100           //MINWTCHG      EQU     $3000   ; 300 GRAMS CHANGE. (In C, wt change is in kg)


// Cleaning definitions
//

#define     EXPANSIONOUTPUT8BITPOS      0x0080  //BIT8    EQU     %00000001
#define     EXPANSIONOUTPUT7BITPOS      0x0040  //BIT7    EQU     %00000010
#define     EXPANSIONOUTPUT6BITPOS      0x0020  //BIT6    EQU     %00000100
#define     EXPANSIONOUTPUT5BITPOS      0x0010  //BIT5    EQU     %00001000
#define     EXPANSIONOUTPUT4BITPOS      0x0008  //BIT4    EQU     %00010000
#define     EXPANSIONOUTPUT3BITPOS      0x0004  //BIT3    EQU     %00100000
#define     EXPANSIONOUTPUT2BITPOS      0x0002  //BIT2    EQU     %01000000
#define     EXPANSIONOUTPUT1BITPOS      0x0001  //BIT1    EQU     %10000000

#define         DIAGNOSTICS_PASSWORD    6553            // Diagnostics password

#define         INC_DEC_MULTIPLIER      1           // used for inc/dec via modbus.
#define         SCEW_SPEED_SIGNAL_COMMS 1           // source/type of screw speed signal.
#define         SCEW_SPEED_SIGNAL_TACHO 0

#define         LAST_SCREW_SPEED_SIGNAL_TYPE    SCEW_SPEED_SIGNAL_COMMS           // last screw speed signal type.


#define     PAUSE_INPUT_TRIGGER     50          //PONACTIVENO     EQU     50       ; NO OF 50Hz AFTER WHICH THE INPUT IS ACTIVE

#define         CONTROL_TYPE_0_10           0           // 0-10v
#define         CONTROL_TYPE_INC_DEC        1           // INCREASEDECREASE.
#define         LASTCONTROLTYPE             CONTROL_TYPE_INC_DEC


#define         CONTROL_MODE_EXTRUSION      1           // EXTRUSION       EQU     1       ; EXTRUSION CONTROL.
#define         CONTROL_MODE_HAULOFF            2       // HAULOFF         EQU     2       ; CRLMODE HAUL OFF CONTROL
#define         CONTROL_MODE_VOLTAGE_FOLLOWER   4       // VOLTAGEFOLLOWER EQU     4       ; VOLTAGE FOLLOWER MODE.
#define         LASTCONTROLMODE                 6


#define         SET_TO_CYCLE                5           // SECTOCYCLE              EQU     5
#define         ZERO_WEIGHT                 0.030       // ZEROWEIGHT      EQU     $0300        30g. ( in C wt is in kg)
#define         DO_DIAGNOSTICS_PERIOD       6           // 6 tenths of second, asm equivalent was DIAG4SEC

#define     UPDATE_TIME_MINS                5           // UPDATETIMEMINS EQU      5        ; UPDATE TIME IN MINUTES.
#define     MIXER_SHUTOFF_MINS      30      // LSUMIXS EQU     30               ; MIXER WILL STOP AFTER
#define     MIXER_OFF_TIME                  10          // MIXOFFTIME              EQU     10
#define     MIN_ROTATION_PULSES_IN_10S      5       // MINROTATIONCOUNT

// 2005
#define     MAX_KGH_RR_SAMPLES              10      // --review--
#define     MAX_GPM_RR_SAMPLES              10      // --review--



// Control definitions
#define     NOOFCORRECTIONSAFTERSTARTUP     5      // NOOFCORRAFTERSTARTUP EQU        5
#define     MAXCONTROLERRORPASSES           2       // NOT DEFINED AS A LABEL IN ASM
#define     CONTROLDEADBANDPERCENTAGE       0.3f    // NOT DEFINED AS A LABEL IN ASM
#define     STARTUPCONTROLBANDPERCENTAGE    10.0f   // BAND INSIDE WHICH START UP IS TURNED OFF
#define     MAXEXTRUDERDTOA                 4095    // MAX D/A SET
#define     MAXEXTRUDERTACHO                32768  // MAX TACHO READING USED IN INCREASE / DECREASE CONTROL.
#define     MINIMUMDAVALUEFORDA             0.05* MAXEXTRUDERDTOA    //MINDAPKGCALC            EQU     205     ; 5% OF MAX NO D/A PER KG CALCULATION BELOW THIS.
#define     NUMBEROFESTIMATIONS             2       //NOOFESTIMATIONS          EQU     2       ; NO OF ESTIMATIONS TO BE RUN.
#define     MAXEXTRUDERTACHOPER             0.10f   // PERLIMIDC               EQU     9830
#define     PERCENTAGECHANGELIMITFORINCDEC  0.15f   //

#define     MAXEXTRUDERDTOAPER              0.20f   //PERLIMNC                 EQU     614
#define     DTOADECREMENTVALUE              16      //DDECRM EQU    16                ;~DAC DECREMENT VALUE (12 BIT DAC)
#define     DTOAINCREMENTVALUE              16      // DINCRM EQU    16       ;~DAC INCREMENT VALUE (FOR INC/DEC KEYS)
#define     EXTRUDERSTALLINHIBITTIME        15      // time before checking extruder stall again.
#define     INCREASEDECREASERATE            2       // INCREASEDECREASERATE    EQU     2  rate for increase / decrease key.
#define     SEIENBITPOS                     0x0001  // SEIENBITPOS     EQU     %00000001      ; SEI BIT POSITION.
#define     LLSENBITPOS                     0x0002  // LLSENBITPOS     EQU     %00000010      ; LLS BIT POSITION.
#define     OPTIMISATIONENABLEBITPOS        0x0004  // optimsiation
#define     MAX_PERIPHERAL_CARD_SETTING     7


#define     MINIMUMLINESPEED                0.05    // min line speed in meter / minute - below this is zero line speed

#define     MINIMUMLINESPEEDCHANGE          1.0f     //MINLSPDCHANGE   EQU     1       ; MIN LINE SPEED CHANGE.
#define     MINIMUMLINESPEEDPERCHANGE       3.0     //MINLSPDPERCHG   EQU     3       ; MIN LSPEED CHANGE
#define     LINESPEEDACTIONNO               2      //LSPEEDACTNO     EQU     3       ; NO OF READINGS AFTER WHICH REACTION TO LINE SPEED.
#define     MINLINESPEEDFORREACTION         2.0    //ZEROLINESPEEDLIMIT      EQU  2    ; LINE SPEED LIMIT
#define     INCREASEDECREASELAMPSTATUSBITPOS      0x0003 // 1st two bits for increase/ decrease lamp status not previously defined
#define     REMOTEINCREASEDECREASEBITPOS    0x0001       // indicates remote inc/dec activation
#define     LOCALINCREASEDECREASEBITPOS     0x0002       // indicates local inc/dec activation
#define     TIMEFORCMRVALID                 10          //  on time to for cmr valid output
#define     SECONDSTOACCEPTSTALL            5           // seconds after which stall is accepted
#define     MINDAFOREXTRUDERSTALL           200         // min d/a for extruder stall.
#define     HILOTRANSITIONON_TO_OFF         1                    //1
#define     HILOTRANSITIONOFF_TO_ON         2                    //2
#define     SEITIMEOUTMAX                   20         //SEITIMEOUTMAX   EQU     50      ; SEI TIMEOUT LIMIT.

#define     PAUSEOFFACTIVENO                10          //POACTIVENO      EQU     10              ; NO OF INCREMENTS AFTER WHICH PAUSE OFF IS ACTIVE
#define     MIXERTRANSITIONDELAY            100         //MIXERTRANSITIONDELAY EQU   100      ;
#define     ROLLCHANGEBOUNCETIMEINSECONDS   15          //ROLLCHANGEBOUNCE EQU    15              ; DEBOUNCE TIME.
#define     INCDEC_COUNTSPERSECOND_CONVERSION 10        // Increase /Decrease conversion factor for counts per second for SEI inc/dec mode

#define     CUSTOMERCODECHARACTERNO         10        // Customer code character no.
#define     MATERIALCODECHARACTERNO         10        // Material code character no.
#define     ORDERNOCHARACTERNO              10        // Material code character no.

#define     CYCLECLEANBLOWINGTIME            50      // time in tenths
#define     CYCLECLEANBLOWINGSHORTTIME       03      // time in tenths
#define     WARMUPLONGTIME                  120     // warmup time in seconds
#define     WARMUPSHORTTIME                 30      // warmup time in seconds
#define     BLENDERREADYTIME                3      // BLENDERREADYTIME    EQU     3           ; DO NOT READ SENSOR FOR N SECONDS.

// system alarm bits
#define     LICENSEALARMBITPOS              0x01   // LICENSEALARMBITPOS  EQU     %0000000000000001 ;
#define     PERALARMBITPOS                  0x02   // PERALARMBIT         EQU     %0000000000000010
#define     HARDWARELICENSEALARMBITPOS      0x04   //

#define     ALARMFLASHRATE                  3      // ALARMFLASHRATE          EQU     3
#define     IGNORELSALARMNO                 3      // IGNORELSALARMNO EQU     5               ; IGNORE LS ALARM FOR N BATCHES



#define     VACSEQ_WAITFOREMPTYBINPRIORTOVACUUMID    1      // WAITFORPRECBEMPTYID     EQU     1
#define     VACSEQ_DELAYBEFOREOFFLINEVALVEOPENEDID   2      // PREVACDELAYID           EQU     2
#define     VACSEQ_MATERIALDROPDELAYDELAYID          3      // DROPDELAYID             EQU     3
#define     VACSEQ_WAITFOREMPTYBINPRIORAFTERVACUUMID 4      // WAITFORPOSTCBEMPTYID    EQU     4
#define     VACSEQ_MATERIALTRANSPORTDELAYID          5      // VACTRANSPORTDELAYID     EQU     5
#define     VACSEQ_POSTMATERIALTRANSPORTDELAYID      6      // POSTVACUUMDELAYID       EQU     6
#define     VACSEQ_NOSEQUENCEID                      0      // DISABLEVACSEQID         EQU     0

#define     VACSEQ_OFFLINEVALVEOPENDELAY            50 // PREVACDELAY             EQU     0050                ; DELAY BEFORE STARTING THE VACUUM
#define     VACSEQ_MATERIALDROPDELAY                50 // DROPDELAY               EQU     0050                ; DELAY BEFORE CHECKING CATCH BOX SENSOR
#define     VACSEQ_MATERIALTRANSPORTDELAY           50 // VACUUMTRANSPORTDELAY    EQU     0050                ; VACUUM TRANSPORT




#define     LICENSE_POWERUPSTATUSID                 0xAA55  // POWERUPSTATUSID     EQU     $AA55       ;INDICATES THAT BLENDER HAS RUN BEFORE.
#define     LICENSE_TESTCODEID                      0x9876  // TESTCODEID          EQU     $9876       ; INDICATES BLENDER IN TEST MODE
#define     LICENSE_MASTERMODULECODE1               0x1234  // MASTERMODULECODE1   EQU     $1234
#define     LICENSE_MASTERMODULECODE2               0x5678  // MASTERMODULECODE2   EQU     $5678
#define     LICENSE_WARNING_HOURS                   0x504   // WARNINGHRS          EQU     504                ;


#define     LICENSE_MASTERCODE                      0x12345678  // MASTERMODULECODE1   EQU     $1234
                                                                // MASTERMODULECODE2   EQU     $5678
#define     LICENSE_TESTCODE                        0x9876      // TESTCODEID          EQU     $9876       ; INDICATES BLENDER IN TEST MODE
#define     LICENSE_POWERUPSTATUSID                 0xAA55      // POWERUPSTATUSID     EQU     $AA55       ;INDICATES THAT BLENDER HAS RUN BEFORE.
#define     LICENSE_WARNINGHOURS                    504         // WARNINGHRS          EQU     504                ;


#define     MULTI_STAGE_FILL_MINWEIGHT              0.010f      // MINMULTISTAGEWEIGHT EQU   $0100           ; MIN MULTIPLE STAGE FILL WEIGHT.
#define     ALLOWLARGEBANDINCYCLES                  3           // ALLOWLARGEBANDINCYCLES  EQU     3


#define     DIAGNOSTICS_ALL_ON                      0xAA        //
#define     DIAGNOSTICS_ALL_OFF                     0xAB        //
#define     DIAGNOSTICS_FINISH_ACTION               0xAC        //
#define     DIAGNOSTICS_SINGLE_OUTPUT_ON            0xAD        //
#define     DIAGNOSTICS_SINGLE_OUTPUT_OFF           0xAE        //

#define     DIAGNOSTICS_SINGLE_OUTPUT2_ON           0xAF        //
#define     DIAGNOSTICS_SINGLE_OUTPUT2_OFF          0xB0        //
#define     DIAGNOSTICS_DUAL_FIRST_TWO_OUTPUTS_OFF  0xB1        //
#define     DIAGNOSTICS_DUAL_FIRST_TWO_OUTPUTS_ON   0xB2        //
#define     DIAGNOSTICS_DUAL_SECOND_TWO_OUTPUTS_ON  0xB3        //
#define     DIAGNOSTICS_DUAL_SECOND_TWO_OUTPUTS_OFF 0xB4        //
#define     DIAGNOSTICS_SINGLE_VALVE_CYCLE_ON       0xB5        //
#define     DIAGNOSTICS_SINGLE_VALVE_CYCLE_OFF      0xB6        //


#define     DIAGNOSTICS_FOUR_COMPONENT_CYCLE        0x02        // TESTPROG1       EQU     2
#define     DIAGNOSTICS_FULL_OUTPUT_CYCLE           0x03        // TESTPROG2       EQU     3
#define     DIAGNOSTICS_DUAL_COMPONENT_CYCLE        0x04        // TESTPROG3       EQU     4
#define     DIAGNOSTICS_DUAL_SLOW_COMPONENT_CYCLE   0x05        // TESTPROG4       EQU     5
#define     DIAGNOSTICS_SINGLE_VALVE_CYCLE          0x06        // TESTPROG5       EQU     6
#define		LAST_DIAGNOSTICS_MODE						 DIAGNOSTICS_SINGLE_VALVE_CYCLE

#define     MODBUSINCREASEDECREASEDTOAINCREMENT     60        // MBDTOAINCREMENT EQU     60
#define     NOMINAL_LATENCY                         0.070f      // NORMALATENCY            EQU     $0820           ;


#define     LINKS_L1                                0x1000        // link l1 position 1 off, 0 on.
#define     LINKS_L2                                0X2000        // link l2 position 1 off, 0 on
#define     LINKS_L3                                0X4000        // link l3 position 1 off, 0 on

#define     EMAIL_STRING_SIZE                       40
#define     EMAIL_BUFFER_SIZE                       2500


// limits for calibration data.
//
#define     LIMIT_BATCH_SIZE_MAX              50.0f
#define DEFAULT_BATCH_SIZE_UNKNOWN            2.9
#define DEFAULT_BATCH_SIZE_TSM350             2.0
#define DEFAULT_BATCH_SIZE_TSM650             4.0
#define DEFAULT_BATCH_SIZE_TSM1000            7.5
#define DEFAULT_BATCH_SIZE_TSM1500            12.0
#define DEFAULT_BATCH_SIZE_TSM3000            25.0
#define DEFAULT_BATCH_SIZE_OPTIMIX50          0.6
#define DEFAULT_BATCH_SIZE_OPTIMIX150         0.8
#define DEFAULT_BATCH_SIZE_OPTIMIX250         1.2

#define DEFAULT_BATCH_SIZE_TSM150             1.1
#define DEFAULT_BATCH_SIZE_TSM50              0.6


#define MAX_BATCH_SIZE_UNKNOWN            50.0f
#define MAX_BATCH_SIZE_OPTIMIX50          0.61f
#define MAX_BATCH_SIZE_OPTIMIX150         0.81f
#define MAX_BATCH_SIZE_OPTIMIX250         1.21f
#define MAX_BATCH_SIZE_TSM350             2.01f
#define MAX_BATCH_SIZE_TSM650             4.01f
#define MAX_BATCH_SIZE_TSM1000            7.51f
#define MAX_BATCH_SIZE_TSM1500            12.01f
#define MAX_BATCH_SIZE_TSM3000            25.01f
#define MAX_BATCH_SIZE_TSM150             1.51f
#define MAX_BATCH_SIZE_TSM50              1.51f

#define MIN_BATCH_SIZE_UNKNOWN            0.5f
#define MIN_BATCH_SIZE_OPTIMIX50          0.5f
#define MIN_BATCH_SIZE_OPTIMIX150         0.5f
#define MIN_BATCH_SIZE_OPTIMIX250         0.5f
#define MIN_BATCH_SIZE_TSM350             0.5f
#define MIN_BATCH_SIZE_TSM650             0.5f
#define MIN_BATCH_SIZE_TSM1000            0.5f
#define MIN_BATCH_SIZE_TSM1500            0.5f
#define MIN_BATCH_SIZE_TSM3000            0.5f
#define MIN_BATCH_SIZE_TSM150             0.5f
#define MIN_BATCH_SIZE_TSM50              0.5f





#define     LIMIT_BATCH_SIZE_MIN                     0.1f
#define     MINIMUM_DTOA_PER_KIL0                    1.0f
#define     MINIMUM_LENGTH                           0.001f
#define     MAX_LIMIT_CONTROL_AVERAGE_FACTOR         8
#define     DEFAULT_LIMIT_CONTROL_AVERAGE_FACTOR     3
#define     MAX_LIMIT_WEIGHT_AVERAGE_FACTOR          8
#define		MIN_LIMIT_CONTROL_GAIN						  0.1f
#define		MAX_LIMIT_CONTROL_GAIN						  1.0f

#define     MIN_LIMIT_PHASE1_CLEAN_ON_TIME           5              // in tenths of seconds
#define     MAX_LIMIT_PHASE1_CLEAN_ON_TIME           500
#define     NOMINAL_PHASE1_CLEAN_ON_TIME             10

#define     MIN_LIMIT_PHASE1_CLEAN_OFF_TIME          5              // in tenths of seconds
#define     MAX_LIMIT_PHASE1_CLEAN_OFF_TIME          500
#define     NOMINAL_PHASE1_CLEAN_OFF_TIME            10

#define     MIN_LIMIT_PHASE2_CLEAN_ON_TIME           5              // in tenths of seconds
#define     MAX_LIMIT_PHASE2_CLEAN_ON_TIME           500
#define     NOMINAL_PHASE2_CLEAN_ON_TIME             10

#define     MIN_LIMIT_PHASE2_CLEAN_OFF_TIME          5              // in tenths of seconds
#define     MAX_LIMIT_PHASE2_CLEAN_OFF_TIME          500
#define     NOMINAL_PHASE2_CLEAN_OFF_TIME            10

#define     MIN_LIMIT_SHUTOFFTIME                    1              // in minutes
#define     MAX_LIMIT_SHUTOFFTIME                    120
#define     NOMINAL_SHUTOFFTIME                      30

#define     MIN_LIMIT_INTERMITTENT_MIX_TIME          5              // in seconds
#define     MAX_LIMIT_INTERMITTENT_MIX_TIME          500
#define     INTERMITTENT_MIX_TIME_MIX_ALWAYS         99

#define     MIN_LIMIT_CLEAN_REPETITIONS              1
#define     MAX_LIMIT_CLEAN_REPETITIONS              20
#define     NOMINAL_CLEAN_REPETITIONS                1

#define     MIN_LIMIT_MIXER_CLEAN_REPETITIONS      1
#define     MAX_LIMIT_MIXER_CLEAN_REPETITIONS      20
#define     NOMINAL_MIXER_CLEAN_REPETITIONS        1


#define     MAXIMUM_CONTROL_LEVEL_SENSOR_ALARMS     5  //LEVELSENSORALARMSACCEPTED   EQU     5       ; NO OF LEVEL SENSOR ALARMS ACCEPTED

#define LCOVERED TRUE
#define LUNCOVERED FALSE
#define     EEPROMCOMMANDTIMEOUT        PIT_FREQUENCY/50
#define     SPIEEACTIVETIMEOUT          PIT_FREQUENCY/50
#define     MILLISECONDS                PIT_FREQUENCY /1000
#define     MITTYTPRINTPERIOD           90 * MILLISECONDS
#define     HALFSECONDINPITS            500 * MILLISECONDS
#define     ONESECONDINPITS             1000 * MILLISECONDS

#define     USBTIMEOUT                  50 * MILLISECONDS
#define     USBLONGTIMEOUT              200 * MILLISECONDS



#define     FLOWRATERESETID             0xAA
#define     OPERATORPANELADDRESS        01

#define USMAXENTRIES (100)

#define MULTIBLEND_EXTRUDER_NAME_SIZE (10)
#define MULTIBLEND_EXTRUDER_NO        (16)
#define DOWNLOADHISTORYNO             (16)

#define RUNNINGRECIPEFILENO           (17)
#define QUEUEINGINTERVAL              (7)

#define SOURCEOUTPUTNO                (20)
#define DEFINEDOUTPUTNO               (16)

#define RECIPE_NAME_STRING_SIZE        (20)
#define CATCHBOXVIBRATIONDELAY         (50)  // tenths of seconds

//#define FLAPDEBOUNCETIME                (150) // tenths of seconds
#define FLAPDEBOUNCETIME                (600) // tenths of seconds

#define NO_PURGE                        (0)
#define MANUAL_PURGE                    (1)
#define AUTO_PURGE                      (2)

#define LASTPURGEMODE                   AUTO_PURGE
#define MINIMUMLATENCY                  (0.02)
#define MAXIMUMLATENCY                  (0.5)

#define DEFAULT_LOAD_ADDRESS              (0)
#define OVERRIDETIMEOUT                  (300)      // OVERRIDE TIMEOUT
#define READINHIBITFORCONTROLINPUTS      (3)      // time to inhibit read of sei control inputs

#define MAX_LINE_SPEED_AVERAGE          (5)           // max line speed average
#define DEFAULT_LINE_SPEED_AVERAGE      (5)           // default line speed average

#define GP12SQFTTOGPM2CONVERSIONFACTOR   1.1148360f   // gpm12sq feet to gpm conversion factor

#define SCREWSPEEDDEVIATIONLIMIT   3.0f   // screw speed deviation limit
#define VOLUMETRICESTIMATETIMEINSECONDS   20   // volumetric esimtate time

#define MAX_BATCHES_IN_KGHR_AVERAGE   10   //
#define DEFAULT_BATCHES_IN_KGHR_AVERAGE   3   //
#define DEFAULT_BATCHES_IN_KGHR_AVERAGE   3   //

#define DEFAULT_MATERIAL_TRANSPORT_DELAY        25
#define DEFAULT_POST_MATERIAL_TRANSPORT_DELAY   10
#define SEIWRITEHOLDOFFTIMEINSECONDS            10
#define SUSPENDCONTROLNO                        3    // SUSPENDCONTROLNO            EQU    3
#define HOLDOFFLSENSORALINSECONDS               120  //HOLDOFFLSENSORALINSECONDS   EQU    120


#define FULLCLEAN                               1  //FULLCLEAN               EQU     01
#define MIXERCLEAN                              2  //MIXERCLEAN              EQU     02
#define LASTCLEAN                           MIXERCLEAN  //LASTCLEAN               EQU     MIXERCLEAN

#define COMPPERENABLEDWITHNOPAUSE              1  //
#define COMPPERENABLEDWITHPAUSE                2  //COMPPERENANDPAUSE           EQU     2

#define LASTCOMPONENTPERRNABLE                 COMPPERENABLEDWITHPAUSE  //LASTCLEAN               EQU     MIXERCLEAN
#define MAX_PAUSE_ON_ERROR_NO                 10
#define DEFAULT_PAUSE_ON_ERROR_NO              1

#define INCREASEOUTPUTONTIME10HZ              10
#define DECREASEOUTPUTONTIME10HZ              10
#define AUTOCYCLELEVELSENSOROFFTIME10HZ       20
#define MAX_CYCLE_DIAGNOSTICS_SAMPLES         5000

#define MB_TABLE_SETPOINT_SIZE                75    // first 75 words
#define CHECKDOWNLOADOFDACNO                  10    //

//#define USB_PRINT_BUFFER_SIZE                40000    //

#define MAX_SD_CARD_FILES                    29    //
#define MAX_SD_CARD_FILE_NAME_SIZE           20    //


#define COPY_DATA_FROM_SD_CARD                1
#define WRITE_DATA_TO_USB                     2
#define WRITE_FINAL_BLOCK_TO_USB              3
#define USB_WRITE_COMPLETED                   4

#define SENDCALDATATOSEINO                    3
#define SAVETOEEPROMCYCLENO                   20
#define NOOFWRITESTOEEPROM                    3

//   BATCH_CALIBRATION_ENTRY_TYPE modes

#define AUTOREGRINDPLUSGREATERTHAN100          0  //nbb--todo-- check for this regrind equal to 100%
#define MANUALREGRINDPLUSGREATERTHAN100        1
#define AUTOREGRINDPLUSEQUALTO100              2
#define MANUALREGRINDPLUSEQUALTO100            3

#define SD_CARD_OPERATION_SHOWN_TIME_SECONDS   5

#define DOWNLOADMANAGERTIMEOUT50HZ             (40 * 50)     // 50 seconds at 50hz
#define NOOFCYCLES_TODISABLEDAPERKG_DURINGCLEAN 3

#define NOOFSECONDSTOWRITETOSDCARD              5

#define OPPANEL_PROD_SUMMARY_PAGE1_ID           (1)
#define OPPANEL_RECIPE_PAGE_ID                  (2)
#define OPPANEL_ALARM_PAGE_ID                   (3)
#define OPPANEL_CONFIG_PAGE1_ID                 (15)
#define OPPANEL_CONFIG_PAGE2_ID                 (16)
#define OPPANEL_CONFIG_PAGE3_ID                 (17)
#define OPPANEL_UPDATE_PAGE_ID                  (18)
#define OPPANEL_NETWORK_IP_CONFIG_PAGE_ID       (19)
#define OPPANEL_CLEANING_CONFIG_PAGE_ID         (20)
#define OPPANEL_PASSWORD_CONFIG_PAGE_ID         (21)
#define OPPANEL_LICENSE_PAGE_ID                 (22)
#define OPPANEL_OPTIMISATION_CONFIG_PAGE_ID     (23)


#define OPPANEL_LINE_SPEED_CALIBRATION_PAGE_ID  (26)
#define OPPANEL_MEASUREMENT_CONTROL_PAGE_ID     (27)
#define OPPANEL_DEVIATION_PAGE_ID               (28)
#define OPPANEL_BIN_CCONFIG_PAGE_ID             (29)
#define OPPANEL_MODE_PAGE_ID                    (30)

#define SDCARDCOMMANDDELAYTIME                   HALFSECONDINPITS
#define TIME_ARRAY_SIZE                          (7)
#define SDCARDACCESSDISABLEINSECS                (30)  // stop write to sd card for this time

#define STARTLIQUIDCALIBRATIONPERCENTAGE         (0.25f)
#define STARTLIQUIDCALIBRATION                   (1)
#define ENDLIQUIDCALIBRATION                     (2)
#define RUNLIQUIDCALIBRATIONFORONEMINUTE         (3)


#define TIMETOWEIGHHOPPEROPENCHECK               (5)
#define LIQUIDADDITIVEONTIME                     (60 * ONESECONDINPITS)

#define MAXIMUM_ALARMS                           (1000)

#define MAX_OFFLINE_MIXING_TIME                  (120)
#define DEFAULT_OFFLINE_MIXING_TIME              (10)
#define MAX_MAX_THROUGHPUT                       (5000)
#define DEFAULT_MAX_THROUGHPUT                   (1000)
#define MAX_FILL_RETRIES                         (5)
#define DEFAULT_FILL_RETRIES                     (0)

#define MAX_BYPASS_TIME_LIMIT                     (500)
#define DEFAULT_BYPASS_TIME_LIMIT                 (10)

#define MAX_CLEAN_BYPASS_TIME_LIMIT               (500)
#define DEFAULT_CLEAN_BYPASS_TIME_LIMIT           (10)

#define ZEROLONGTERMRESETTABLECOUNTERSCOMMAND     (1)
#define ZEROLONGTERMCOUNTERSCOMMAND               (2)

#define TELNETTIMEOUTINMINS                       (30)

#define TELNETWINDOWINPITS                        (50)
#define EVENTLOGSTRINGLENGTH                      (10000)

#define ORDER_REPORT_BUFFER_SIZE                  (2000)

#define LOSS_OF_LOCK_RESET_STATUS_BIT              0X01
#define LOSS_OF_CLOCK_RESET_STATUS_BIT             0X02
#define EXTERNAL_DEVICE_RESET_STATUS_BIT           0X04
#define POWER_ON_RESET_STATUS_BIT                  0X08
#define WATCHDOG_RESET_STATUS_BIT                  0X10
#define SOFTWARE_RESET_STATUS_BIT                  0X20



#define     DUMP_DEVICE_DISABLED                      0
#define     DUMP_DEVICE_USB                           1
#define     DUMP_DEVICE_PRINTER                       2

#define     LAST_DUMP_DEVICE_TYPE     DUMP_DEVICE_PRINTER

#define USBFILENAMESTRINGLENGTH       (13)
#define BLAST_ON_TIME_HZ                           (02)


#define     FUNCTION_1                                (1)
#define     FUNCTION_2                                (2)
#define     FUNCTION_3                                (3)
#define     FUNCTION_4                                (4)
#define     FUNCTION_5                                (5)
#define     FUNCTION_6                                (6)
#define     FUNCTION_7                                (7)
#define     FUNCTION_8                                (8)
#define     FUNCTION_9                                (9)
#define     FUNCTION_10                               (10)
#define     FUNCTION_11                               (11)
#define     FUNCTION_12                               (12)
#define     FUNCTION_13                               (13)
#define     FUNCTION_14                               (14)
#define     FUNCTION_15                               (15)
#define     FUNCTION_16                               (16)
#define     FUNCTION_17                               (17)
#define     FUNCTION_18                               (18)
#define     FUNCTION_19                               (19)
#define     FUNCTION_20                               (20)
#define     FUNCTION_21                               (21)
#define     FUNCTION_22                               (22)
#define     FUNCTION_23                               (23)
#define     FUNCTION_24                               (24)
#define     FUNCTION_25                               (25)
#define     FUNCTION_26                               (26)
#define     FUNCTION_27                               (27)
#define     FUNCTION_28                               (28)
#define     FUNCTION_29                               (29)
#define     FUNCTION_30                               (30)
#define     FUNCTION_31                               (31)
#define     FUNCTION_32                               (32)
#define     FUNCTION_33                               (33)
#define     FUNCTION_34                               (34)
#define     FUNCTION_35                               (35)
#define     FUNCTION_36                               (36)
#define     FUNCTION_37                               (37)
#define     FUNCTION_38                               (38)
#define     FUNCTION_39                               (39)
#define     FUNCTION_40                               (40)
#define     FUNCTION_41                               (41)
#define     FUNCTION_42                               (42)
#define     FUNCTION_43                               (43)
#define     FUNCTION_44                               (44)
#define     FUNCTION_45                               (45)
#define     FUNCTION_46                               (46)
#define     FUNCTION_47                               (47)
#define     FUNCTION_48                               (48)
#define     FUNCTION_49                               (49)
#define     FUNCTION_50                               (50)
#define     FUNCTION_51                               (51)
#define     FUNCTION_52                               (52)
#define     FUNCTION_53                               (53)
#define     FUNCTION_54                               (54)
#define     FUNCTION_55                               (55)
#define     FUNCTION_56                               (56)
#define     FUNCTION_57                               (57)
#define     FUNCTION_58                               (58)
#define     FUNCTION_59                               (59)
#define     FUNCTION_60                               (60)
#define     FUNCTION_61                               (61)

#define     FUNCTION_SUB_SECTION_1                    (1)
#define     FUNCTION_SUB_SECTION_2                    (2)
#define     FUNCTION_SUB_SECTION_3                    (3)
#define     FUNCTION_SUB_SECTION_4                    (4)
#define     FUNCTION_SUB_SECTION_5                    (5)

#define PANEL_REVISION_STRING_SIZE  9
#define NBB_SERIAL_NUMBER_SIZE  17
#define SEI_SOFTWARE_REVISION_STRING_SIZE  9

#define     MAXIMUM_LINESPEED_CONSTANT                10000.0f
#define     MINIMUM_LINESPEED_CONSTANT                100.0f

#define     NOMINAL_LINESPEED_CONSTANT                2000.0f

#define     PRE_PULSING_STAGE                           (1)
#define     VALVE_PULSING_STAGE                         (2)

#define     INITIAL_PULSE_TIME_FOR_AUTOCYCLE            (1)
#define     INITIAL_PULSE_TIME                         (50)
#define     MAXIMUM_COMPONENT_PULSES                   (30)
#define     PULSE_INCREMENT_TIME                       (5)

#define     FILLING_METHOD_PULSING_MODE                (1)

#define     BLENDER_LOCK_CODE                          (9990)
#define     BLENDER_UNLOCK_CODE                        (9991)
#define     BLENDER_WATCH_DOG_TIMER_ON_CODE            (9992)
#define     BLENDER_WATCH_DOG_TIMER_OFF_CODE           (9993)


#define     MODBUS_EVENT_LOG_BUFFER_LENGTH             (10000)
#define     MINIMUM_WEIGHT_TO_ALLOW_HOPPER_REFILL      (0.050)
#define     TIME_FOR_ATOD_TO_SETTLE_AFTER_SDWRITE      (500)  // pits at 3000/second

#define     LOG_EVENTS_TO_SD_CARD                      (1)
#define     LOG_BATCH_LOG_TO_SD_CARD                   (2)

#define     MINIMUM_PIT_OPEN_TIME                      (5)
#define     ENDOFCYCLETIMEINSECONDS                    (5)

#define     MAX_STABILITY_READINGS                    (10)
#define     DELAY_BEFORE_STABILITY_DATA_READ          (5)
#define     ATOD_REFERENCE_TIMEOUT_SECONDS            (60)

#define     TOPUPCOMPONENT                            (8)
#define     SD_CARD_NAME_SIZE                         (12)
#define     SOFTWARE_WATCHTIME_SECONDS_TIMEOUT        (20)
#define     PLATFORM_5270B        					 (0xA5)


// usb copy definitions



/*
; SEI LAMP DEFINITIONS.
DECLAMPBITPOS           EQU     %00000010       ; DECREASE LAMP.
INCLAMPBITPOS           EQU     %00000100       ; INCREASE LAMP POSITION.
ALARMBITPOS             EQU     %00001000       ; ALARM BIT POSITION.
GOTOVFMODEBITPOS        EQU     %00100000       ; VOLTAGE FOLLOWER BIT POSITION.

*/

// Convenient method of extracting the high and low
// bytes from an int or converting 2 bytes to an int

//nbb--testonly--
union CharsAndInt
{
    unsigned int        nValue;
   char    cValue[2];
};

//
union IntsAndLong
{
    unsigned long   lValue;
    int nValue[2];
};

void mfprintf (char * array, float fTemp);
void miprintf (char * array, int nTemp);
void mtprintf (char * array);



#endif  // __GENERAL_H__
