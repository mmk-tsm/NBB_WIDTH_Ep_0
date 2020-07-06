/* Notes for Translator */

/* Finnish has been allocated to the 6th text string in  each group of strings.
    Please translate only text within double quotation marks    "   ",
    required text is highlighted in yellow.  Overwrite existing 'yellow' text with Finnish text.
    Please do not delete or overwrite any formatting characters, such as \n (end of line) or %s or white spaces.
    Please try to keep translated text to similar lenghts as original text.  This is especially important for short or one word strings, or abbreviated text.
    An attempt has been made to show what all abbreviations mean.
    Please save the file as TEXT ONLY.  The editor used by TSM to incorporate the file into the working
    software cannot handle Word formatted files or documents.
    If you have any questions please phone Michael McKiernan at TSM Control Systems (+353 42 9335560) or
    Send query by email to mmk@tsm-controls.com.
*/



/////////////////////////////////////////////////
// TextStrings.h     (cp1v25a textstrings.doc)
//
//Language Allocations: 1st = English 2nd = French, 3rd = Portuguese, 4th = Spanish. 5th = Italian 6th = Finnish.(***Finnish not done*****)

// Text strings used in the coordinating panel.
// 13-05-2004            1.27e   Version -> 1.27e --   Any blender in alarm - Drive fibre outputs (provided not in clean cycle)
//
// version babl h update            5/9/05
//
// P.Smith                          4/10/05
// BABL.k
//
// P.Smith                          4/10/05
// BABL.l
//
// P.Smith                          27/10/05
// BABL.m
//
// P.Smith                          9/11/05
// BABL.n
//
// P.Smith                          14/11/05
// BABL.o
//
// P.Smith                          23/11/05
// BABL.p
// added text strings for retry component, and other test strings
//
// P.Smith                          15/12/05
// BABL.q
//
// P.Smith                          5/1/06
// BABL.r
//
// P.Smith                          9/1/06
// BABL.s
//
// P.Smith                          21/4/06
// BABL.t
//
// P.Smith                          3/4/06
// BABL.u
// added const   char   far   *strGramsPerMeterPrint[LANGUAGES_MAX] = {
//
// P.Smith                          5/5/06
// BABL.v
//
// P.Smith                          11/5/06
// BABL.w
//
// P.Smith                          15/5/06
// BABL.x
//
// P.Smith                      1/6/06
//#include <basictypes.h>

// P.Smith                      9/6/06
// revision no set to Beta1.4
//
// P.Smith                      16/6/06
// added text strings for email
//
// P.Smith                      19/7/06
// string corrections for diagnostics printing.
//
// P.Smith                      5/2/07
// version NBB_Beta 2.01
//
// P.Smith                      6/2/07
// version NBB_Beta 2.02
// ********************************Batch Blender***************************************************************** //

// 21-05-2004            1.01a   Version -> Version BaBl1.01a --   First batch blender attempt
// 24-05-2004                                           Batch blender texts: see **BB**  strRecipeMenuText
// 02-07-2004                                           strRecipeNo, strSelectRecipe, strRecipe.
// 20-07-2004                                           strPromptForAutoCycleMode,strCPI
// 13-08-2004               Version -> Version BaBl1.01a9 - just for ID purposes.
// 02-09-2004               Version -> Version BaBl1.10a
// 03-09-2004               Version -> Version BaBl1.10b
// 06-09-2004               Version -> Version BaBl1.10c
// 07-09-2004           Version -> Version BaBl1.10d
// 09-09-2004           Version -> Version BaBl1.10e
//
// 14-09-2004           Version -> Version BaBl1.10f
// 22-09-2004           Version -> Version BaBl1.10g
//          Added enable/disable layering strings.
// 28-09-2004           Version -> Version BaBl1.10h
// 28-09-2004           Version -> BaBl1.10i
// 01-11-2004           Version -> BaBl1.10j
// 01-11-2004           Version -> BaBl1.10k
// 08-11-2004           Version -> BaBl1.10l
// 08-11-2004           Version -> BaBl1.10m
// 15-11-2004           Version -> BaBl1.10n
// 22-11-2004           Version -> BaBl1.10o
// 07-12-2004           Version -> BaBl1.10p
// 15-12-2004           Version -> BaBl1.10q
// 15-12-2004           Version -> BABL1.11a
// 05-01-2005           Version -> BABL1.11b
// 18-01-2005           Version -> BABL1.12a
// P.Smith                              14-02-2005
// Added start up message
//
// P.Smith                              10/10/05
// Added strSoftwareID
//
// P.Smith                              2/11/06
// Beta 1.46
//
// P.Smith                              5/12/06
// Beta 1.47
//
// P.Smith                              11/1/07
// Beta 1.48
//
// P.Smith                              23/1/07
// Beta 2.0
//
// P.Smith                              6/2/07
// Beta 2.02
//
// P.Smith                              28/2/07
// Beta 2.03
//
// P.Smith                              8/3/07
// Beta 2.04 ultrasonic transducer read of sensor.
//
// P.Smith                              13/3/07
// Beta 2.05 ultrasonic transducer read of sensor.
//
// P.Smith                              23/4/07
// Beta 2.06 ultrasonic transducer read of sensor.
//
// P.Smith                              23/4/07
// Beta 2.07 Multiblend algorithm transferred to the blender
//
// P.Smith                              30/4/07
// Beta 2.08 Multiblend algorithm transferred to the blender
//
// P.Smith                              2/5/07
// Beta 2.09
//
// P.Smith                              15/5/07
// Beta 2.10
//
// P.Smith                              22/5/07
// Beta 2.11
//
// P.Smith                              28/5/07
// Beta 2.12
// added $1,$2 for printing.
//
// P.Smith                              30/5/07
// Beta 2.13
//
// P.Smith                              11/6/07
// Beta 2.14
//
// P.Smith                              12/6/07
// Beta 2.15
//
// P.Smith                              20/6/07
// Beta 2.16
//
// P.Smith                              25/6/07
// Beta 2.17
//
// P.Smith                              2/7/07
// Beta 2.18

//
// P.Smith                              17/7/07
// Beta 2.19
//
// P.Smith                              30/7/07
// Beta 2.20
//
// P.Smith                              31/7/07
// Beta 2.21
//
// P.Smith                              14/8/07
// Beta 2.22
// M.McKiernan                          06/09/2007
//  RevisionNumber -> 2.23
// M.McKiernan                          18/09/2007
//  RevisionNumber -> 2.24
// M.McKiernan                          26/09/2007
//  RevisionNumber -> 2.25
//
// P.Smith                              27/09/2007
//  RevisionNumber -> 2.26
//
// P.Smith                              3/10/2007
// added strBatchLength
//
// P.Smith                              24/10/2007
// RevisionNumber -> 2.27
//
// P.Smith                              2/11/2007
// RevisionNumber -> 2.28
//
//
// P.Smith                              12/11/2007
// RevisionNumber -> 2.29
//
// P.Smith                              15/11/2007
// RevisionNumber -> 2.30
//
// P.Smith                              20/11/2007
// RevisionNumber -> 2.31
//
// P.Smith                              21/11/2007
// RevisionNumber -> 2.32
//
//
// P.Smith                              21/11/2007
// RevisionNumber -> 2.33
//
//
// P.Smith                              22/11/2007
// RevisionNumber -> 2.34
//
// P.Smith                              28/11/2007
// RevisionNumber -> 2.35
//
// P.Smith                              29/11/2007
// RevisionNumber -> 2.36
//
// P.Smith                              21/12/2007
// RevisionNumber -> 2.37
//
// P.Smith                              10/1/2008
// RevisionNumber -> 2.38
//
// P.Smith                              11/1/2008
// RevisionNumber -> 2.39
//
// P.Smith                              11/1/2008
// RevisionNumber -> 2.40
//
// P.Smith                              16/1/2008
// RevisionNumber -> 2.41
//
// P.Smith                              22/1/2008
// RevisionNumber -> 2.42
// added const   strFillByWeightHeading,strFillByWeightMessage,strNoFillByWeightMessage
//
// P.Smith                              23/1/2008
// RevisionNumber -> 2.43
//
// P.Smith                              25/1/2008
// added text strings for staticstical data
//
// P.Smith                              29/1/2008
// RevisionNumber -> 2.44
//
// P.Smith                              31/1/2008
// RevisionNumber -> 2.45
//
// P.Smith                              4/2/2008
// RevisionNumber -> 2.46
//
// P.Smith                              10/2/2008
// RevisionNumber -> 2.47
//
// P.Smith                              12/2/2008
// RevisionNumber -> 2.48
//
// P.Smith                              19/2/2008
// added strLogBatch, strLogControl, strLogStatistics
//
// P.Smith                              20/3/2008
// added strActualLoadCellRawCounts,strActualLoadCellTareCounts etc for additional
// data logging
//
// P.Smith                              20/3/2008
// RevisionNumber -> 2.49
//
// P.Smith                              26/3/2008
// RevisionNumber -> 2.50
// M.McKiernan                          04/4/2008
// RevisionNumber -> 2.51
// M.McKiernan                          14/4/2008
// RevisionNumber -> 2.52
// P.Smith                              23/4/2008
// RevisionNumber -> 2.53
// P.Smith                              9/5/2008
// RevisionNumber -> 2.55   kwh changes and others
// P.Smith                              9/5/2008
// RevisionNumber -> 2.56
// P.Smith                              12/06/08
// RevisionNumber -> 2.57
// P.Smith                              12/06/08
// RevisionNumber -> 2.58
// P.Smith                              24/06/08
// RevisionNumber -> 2.59
// P.Smith                              27/06/08
// RevisionNumber -> 2.60
// add alarm text strings
// P.Smith                              18/07/08
// RevisionNumber -> 2.61
// P.Smith                              21/07/08
// RevisionNumber -> 2.62
// P.Smith                              23/07/08
// RevisionNumber -> 2.63
// P.Smith                              24/07/08
// RevisionNumber -> 2.64
// P.Smith                              24/07/08
// RevisionNumber -> 2.65
// P.Smith                              25/07/08
// RevisionNumber -> 2.66
// P.Smith                              25/07/08
// RevisionNumber -> 2.67
//
// P.Smith                              12/08/08
// RevisionNumber -> 2.68
//
// P.Smith                              26/08/08
// RevisionNumber -> 2.69
//
// P.Smith                              11/9/08
// RevisionNumber -> 2.70
// add retries text string strRetriesCounter
//
// P.Smith                              11/9/08
// RevisionNumber -> 2.71
//
// P.Smith                              26/9/08
// RevisionNumber -> 2.72
// add temperature string.
//
// P.Smith                              29/9/08
// RevisionNumber -> 2.73
// added strDumpFlapHasNotOpenedAlarm
//
// P.Smith                              1/10/08
// RevisionNumber -> 2.74
//
// P.Smith                              2/10/08
// RevisionNumber -> 2.75
//
// P.Smith                              8/10/08
// RevisionNumber -> 2.76
//
// P.Smith                              8/10/08
// RevisionNumber -> 2.77
//
// P.Smith                              9/10/08
// RevisionNumber -> 2.80
//
// P.Smith                              16/10/08
// RevisionNumber -> 2.81
// added strLiquidAdditiveSetKgHr,*strLiquidAdditiveKgHrCalibration
// *strLiquidAdditiveDToA
//
// P.Smith                              17/10/08
// RevisionNumber -> 2.82
//
// P.Smith                              22/10/08
// RevisionNumber -> 2.83
//
// P.Smith                              28/10/08
// RevisionNumber -> 2.84
//
// P.Smith                              29/10/08
// added strSDCardAlarm
//
// P.Smith                              14/11/08
// RevisionNumber -> 2.85
// added strCR
//
// P.Smith                              17/11/08
// RevisionNumber -> 2.86
//
// P.Smith                              19/11/08
// RevisionNumber -> 2.87
// added strings for order report, remove other strings for different languages.
// add reset sources text
//
// P.Smith                              26/11/08
// RevisionNumber -> 2.88
//
// P.Smith                              26/11/08
// RevisionNumber -> 2.89
//
// P.Smith                              9/12/08
// RevisionNumber -> 2.90
//
// P.Smith                              11/12/08
// RevisionNumber -> 2.91
//
// P.Smith                              16/12/08
// RevisionNumber -> 2.92
//
// P.Smith                              21/1/09
// RevisionNumber -> 2.93
//
// P.Smith                              22/1/09
// RevisionNumber -> 2.94
//
// P.Smith                              29/1/09
// RevisionNumber -> 2.95
//
// P.Smith                              29/1/09
// RevisionNumber -> 2.96
//
// P.Smith                              26/2/09
// RevisionNumber -> 2.97
// removed unused text strings
//
// P.Smith                              1/4/09
// RevisionNumber -> 2.98
// removed unused text strings
//
// P.Smith                              3/4/09
// RevisionNumber -> 2.99
//
// P.Smith                              3/4/09
// RevisionNumber -> 3.00
//
// P.Smith                              3/4/09
// RevisionNumber -> 3.01
//
// P.Smith                              25/5/09
// RevisionNumber -> 3.02
// added strI2CStopped
//
// P.Smith                              26/5/09
// RevisionNumber -> 3.03
//
// P.Smith                              8/6/09
// RevisionNumber -> 3.04
//
// P.Smith                              10/6/09
// RevisionNumber -> 3.05
//
// P.Smith                              10/6/09
// RevisionNumber -> 3.10
//
// P.Smith                              26/6/09
// RevisionNumber -> 3.11
//
// P.Smith                              1/7/09
// RevisionNumber -> 3.12
//
// P.Smith                              22/7/09
// RevisionNumber -> 3.13
//
// P.Smith                              25/8/09
// RevisionNumber -> 3.14
// added strDeviationAlarm & strInvalidConfigData
//
// P.Smith                              26/8/09
// RevisionNumber -> 3.15
//
// P.Smith                              26/8/09
// RevisionNumber -> 3.16
//
// P.Smith                              1/9/09
// RevisionNumber -> 3.17
// added strAToDEOCNotCompleteAlarm & strAToDNegativePolarityAlarm
//
// P.Smith                              8/9/09
// RevisionNumber -> 3.18
//
// P.Smith                              10/9/09
// RevisionNumber -> 3.19
//
// P.Smith                              15/9/09
// RevisionNumber -> 3.20
//
// P.Smith                              17/9/09
// RevisionNumber -> 3.22
//
// P.Smith                              28/9/09
// RevisionNumber -> 3.23
//
// P.Smith                              29/9/09
// RevisionNumber -> 3.24
//
// P.Smith                              30/9/09
// RevisionNumber -> 3.25
//
// P.Smith                              1/10/09
// RevisionNumber -> 3.26
//
// P.Smith                              5/10/09
// RevisionNumber -> 3.27
//
// P.Smith                              12/10/09
// RevisionNumber -> 3.28
//
// P.Smith                              13/10/09
// RevisionNumber -> 3.29
//
// P.Smith                              16/11/09
// RevisionNumber -> 3.30
// added load cell stability strings
//
// P.Smith                              16/11/09
// RevisionNumber -> 3.31
//
// P.Smith                              16/11/09
// RevisionNumber -> 3.32
//
// P.Smith                              24/11/09
// RevisionNumber -> 3.33
//
// P.Smith                              30/11/09
// RevisionNumber -> 3.34
//
// P.Smith                              2/12/09
// RevisionNumber -> 3.35
//
// P.Smith                              7/12/09
// RevisionNumber -> 3.36
//
// P.Smith                              8/12/09
// RevisionNumber -> 3.37
//
// P.Smith                              9/12/09
// RevisionNumber -> 3.38
//
// P.Smith                              9/12/09
// RevisionNumber -> 3.38
//
// P.Smith                              15/12/09
// RevisionNumber -> 3.40E
//
// P.Smith                              15/12/09
// RevisionNumber -> 3.41E
//
// P.Smith                              18/1/10
// RevisionNumber -> 4.42E
//
// P.Smith                              20/1/10
// RevisionNumber -> 4.43E
//
// P.Smith                              21/1/10
// RevisionNumber -> 4.44E
//
// P.Smith                              21/1/10
// RevisionNumber -> 4.45E
//
// P.Smith                              3/2/10
// RevisionNumber -> 4.46E
//
// P.Smith                              4/2/10
// Ethernet IP -> 4.50E
//
// P.Smith                              8/2/10
// Ethernet IP -> 4.51E  4.06 mmk version
//
// P.Smith                              15/2/10
// Ethernet IP -> 4.52E  4.07 mmk version
//
// P.Smith                              15/2/10
// Ethernet IP -> 4.53E
//
// P.Smith                              19/2/10
// Ethernet IP -> 4.54E
//
// P.Smith                              19/2/10
// Ethernet IP -> 4.55E
//
// P.Smith                              5/3/10
// Ethernet IP -> 4.56E
//
// P.Smith                              8/3/10
// Ethernet IP -> 4.57E
//
// P.Smith                              8/3/10
// Ethernet IP -> 4.58E
//
// P.Smith                              9/3/10
// Ethernet IP -> 4.59E
//
// P.Smith                              10/3/10
// Ethernet IP -> 4.60E
//
// P.Smith                              10/3/10
// Ethernet IP -> 4.61E
//
// P.Smith                              11/3/10
// Ethernet IP -> 4.62E
//
// P.Smith                              11/3/10
// Ethernet IP -> 4.63E
//
// P.Smith                              16/3/10
// Ethernet IP -> 4.64E
//
// P.Smith                              16/3/10
// Ethernet IP -> 4.65E
//
// P.Smith                              16/3/10
// Ethernet IP -> 4.66E
//
// P.Smith                              25/3/10
// Ethernet IP -> 4.67E
//
// P.Smith                              25/3/10
// Ethernet IP -> 4.68E
//
// P.Smith                              25/3/10
// Ethernet IP -> 4.69E
//
// P.Smith                              26/3/10
// Ethernet IP -> 4.70E
//
// P.Smith                              6/4/10
// Ethernet IP -> 4.71E
//
// P.Smith                              8/4/10
// Ethernet IP -> 4.72E
//
// P.Smith                              13/4/10
// Ethernet IP -> 4.73E
//
// P.Smith                              13/4/10
// Ethernet IP -> 4.74E
//
// P.Smith                              21/4/10
// Ethernet IP -> 4.75E
//
// P.Smith                              23/4/10
// Ethernet IP -> 4.76E
//
// P.Smith                              27/4/10
// Ethernet IP -> 4.77E
//
// P.Smith                              27/4/10
// Ethernet IP -> 4.78E
//
// P.Smith                              30/4/10
// Ethernet IP -> 4.79E
//
// P.Smith                              8/6/10
// width control   1.00
//
// P.Smith                              7/7/10
// width control   WIDV1.01E
//
// P.Smith                              7/7/10
// width control   WIDV1.02E
//
// P.Smith                              19/7/10
// width control   WIDV1.03E
//
// P.Smith                              16/9/10
// width control   WIDV1.04E
//
// P.Smith                              21/9/10
// width control   WIDV1.05E
//
// P.Smith                              21/9/10
// width control   WIDV1.06E
//
// P.Smith                              22/3/2011
// width control   WIDV1.08E

// P.Smith                              28/3/2011
// width control   WIDV1.09E

// P.Smith                              10/6/2011
// width control   WIDV1.10E

// P.Smith                              27/10/2011
// added new RTC   WIDV1.11E

// M.McKiernan		NBBWIDE_112			24/01/2020			// Got to compile again - no changes yet.
// M.McKiernan		NBBWIa_113			16/03/2020			// starting work for "corrected Math".
// M.McKiernan		NBBWIDa_114			24/04/2020			// wfh,  getting ready to test.
// M.McKiernan		NBBWIDi_114			25/04/2020			// wfh,  moved AB etc withing calib. struct.
// M.McKiernan		NBBWiDi_114			27/04/2020			// wfh,  restarting
// M.McKiernan		NBBWIDj_114			27/04/2020			// wfh,
// M.McKiernan		NBBWIDf_115			27/04/2020			// wfh, tidy up.
// M.McKiernan		NBBWfD_116			01/05/2020			// wfh, layflat offset imperial
// M.McKiernan		NBBWgE_116			02/05/2020			// wfh, DisplayLEDStatus()
// M.McKiernan		NBBWhE_116			05/05/2020			// wfh DisplayLEDStatus() nSensor = g_cBlenderNetworkAddress;	// set it equal to the hex switch
// M.McKiernan		NBBWiE_116			05/05/2020			// wfh DisplayLEDStatus()  testing....
// M.McKiernan		NBBWJE_116			06/05/2020			// wfh 11012 to test Fast Increase o/p ..[MODBUS_FORCE_RESET_COMMAND] == FORCE_FAST_INFLATE )WidthManualFastIncrease();
// M.McKiernan		NBBWKE_116			07/05/2020			// wfh 11013 ETC to test usmUX A0 & CLOCK);
// M.McKiernan		NBBWLE_116			07/05/2020			// wfh removed a SRC9OFF. in INTERRUPT(Timer0_ISR, 0x2500 )
// M.McKiernan		NBBWmE_116			07/05/2020			// wfh reversed state of A0 for LEDs ..... test only.
// M.McKiernan		NBBWnE_116			08/05/2020			// wfh US_METHOD_1 US_METHOD_2
// M.McKiernan		NBBWoE_116			08/05/2020			// wfh method in MB at 8524.
// M.McKiernan		NBBWpE_116			11/05/2020			// wfh checks on sensors. in WidthFiftyHertzHandler( void )
// M.McKiernan		NBBWqE_116			12/05/2020			// wfh
// M.McKiernan		NBBWrE_116			13/05/2020			// wfh set flag to update width config file. - g_bCopyWidthConfigFileToSDCard = TRUE;
// M.McKiernan		NBBWsE_116			13/05/2020			// wfh added new width config stuff to file for SD card.
// M.McKiernan		NBBWtE_116			13/05/2020			// wfh added missing width config stuff to file for SD card.
// M.McKiernan		NBBWuE_116			14/05/2020			// wfh added missing width config stuff to file for SD card.
// M.McKiernan		NBBWvE_116			14/05/2020			// wfh removed DisplayLEDStatus()
// M.McKiernan		NBBWwE_116			18/05/2020			// wfh added CalculateM2Offset().
// M.McKiernan		NBBWxE_116			19/05/2020			// wfh load config from SD card - complete.

// M.McKiernan		NBBWaE_117			20/05/2020			// wfh AdvanceToNextChannel now called in PITHandler.cpp.
// M.McKiernan		NBBWbE_117			21/05/2020			// wfh g_bUSSensorInAlarm[]
// M.McKiernan		NBBWdE_117			22/05/2020			// wfh g_bWriteWidthMeasurementDiagnosticsToSd
// M.McKiernan		NBBWeE_117			22/05/2020			// wfh g_bUploadedRefLength
// M.McKiernan		NBBWfE_117			22/05/2020			// wfh Added diameter, layflat, offset,  Method 1 & 2 to Width diagnostics.

// M.McKiernan		NBBWaE_118			25/05/2020			// wfh ......getting ready for release version.
// M.McKiernan		NBBWbE_118			25/05/2020			// wfh ......sensor statuses.
// M.McKiernan		NBBWdE_118			25/05/2020			// wfh ......Revision.
// M.McKiernan		NBBWeE_118			25/05/2020			// wfh ......status
// M.McKiernan		NBBWIE_118			26/05/2020			// wfh ......clean up

// M.McKiernan		NBBWIE_119			27/05/2020			// wfh ......dhcp
// M.McKiernan		NBBWIE_120			29/05/2020			// readying for SS.

// M.McKiernan		NBBWIx_120			4/06/2020			// Error analysis.  Sensors on 600mm radius circle, bubbles of 330 & 250mm
// M.McKiernan		NBBWIz_120			8/06/2020			// TestDrivingLEDs().
// M.McKiernan		NBBWIZ_120			9/06/2020			// PITTestForUSMuxLEDs().
// M.McKiernan		NBBWI2_120			16/06/2020			// .....error analysis.
// M.McKiernan		NBBWi3_120			18/06/2020			// .....angle calculations.
// M.McKiernan		NBBWi4_120			19/06/2020			// void CalculateSensorAngles( float fRadius )   still not caled.
// M.McKiernan		NBBWi5_120			22/06/2020			// void CalculateSensorAngles( float fRadius )   called at calibration from Layflat.
// M.McKiernan		NBBWi6_120			22/06/2020			// removed diagnostic.
// M.McKiernan		NBBWi7_120			23/06/2020			// removed PITTestForUSMuxLEDs(). - PITHandler.cpp
// M.McKiernan		NBBWi8_120			23/06/2020			// g_fSensorAAngle....
// M.McKiernan		NBBWi9_120			24/06/2020			// g_fSensorAAngle.... copied to MB.  Initialise to 30, 150, 170
// M.McKiernan		NBBWIE_121			24/06/2020			// Version for Minigrip.
// M.McKiernan		NBBWIa_121			26/06/2020			// //TODO debugging 26.6.2020 WriteWidthMeasurementDiagnosticDataToSDCard();
// M.McKiernan		NBBWIb_121			26/06/2020			////wfh 26.6.2020 debugging GenerateWidthMeasurementDiagnostics( );
// M.McKiernan		NBBWIc_121			26/06/2020			////back in GenerateWidthMeasurementDiagnostics( );
// M.McKiernan		NBBWId_121			26/06/2020			////removed cbuf1 offset form diagnostics.
// M.McKiernan		NBBWIe_121			26/06/2020			////write to SD restored.
// M.McKiernan		NBBWIg_121			26/06/2020			////restored cBuf1 in diagnostic
// M.McKiernan		NBBWIh_121			26/06/2020			////print out offset & sign of offset.
// M.McKiernan		NBBWIj_121			26/06/2020			// g_fMyWOffset
// M.McKiernan		NBBWIk_121			26/06/2020			// redo check in Initblnd.
// M.McKiernan		NBBWIm_121			26/06/2020			// redo checks in Initblnd. - sign,.
// M.McKiernan		NBBWIn_121			26/06/2020			// Added checks or new width calibration parameters - CheckForValidWidthConfigData(
// M.McKiernan		NBBWIo_121			26/06/2020			//  EnableUSMux1(); - call in PITHandler() Force the EN for the USMux #1.
// M.McKiernan		NBBWIp_121			29/06/2020			//  removed EnableUSMux1 from PIT.  Corrected void SetUltrasonicMux( void ).
// M.McKiernan		NBBWIQ_121			29/06/2020			//   In void CheckForValidHardware( void ) force it to skip the SHA test.
// M.McKiernan		NBBWIr_121			29/06/2020			//   diagnostic to see if check running. g_nSHATestMMK
// M.McKiernan		NBBWIs_121			29/06/2020			//   snprintf test.
// M.McKiernan		NBBWIE_122			29/06/2020			//   = -> == in PutAlarmTable.

// M.McKiernan		NBBWIE_123			30/06/2020			//   width alarms -> 340's.
// M.McKiernan		NBBWIf_123			30/06/2020			//   void SwitchWidthAlarmOnOrOff( void )    timer > 10 mins even if not out of startup.

// M.McKiernan		NBBWIE_124			02/07/2020			//   Version for SS

/////////////////////////////////////////////////

#ifndef __TEXTSTRINGS_H__
#define __TEXTSTRINGS_H__

#include "General.h"
#include "AlarmFunctions.h"

// These will not be translated and so are not set up
// as part of the language arrays.

const   char      *TSMName = "TSM Control Systems Ltd";
const   char      *Copyright = "CopyRight(c) 2020 - TSM";
const   char      *Initialise = "Initialising....";


//const int arrnBatchBlenderModbusTable[BATCH_READ_TABLE_SIZE][3] = {
//        { 0, 0, TABLE_REGISTER_SIZE_INT },              // Start of Common data, not actually modbus data



const char  *RevisionNumber[LANGUAGES_MAX] = {
        "NBBWIE_124     ",
        "NBBWIE_124     ",
        "NBBWIE_124     ",
        "NBBWIE_124     ",
        "NBBWIE_124     ",
        "NBBWIE_124     ",
               };

const   char  *WatchDogTimerReset = "** Watchdog Timer Reset Occurred **";

const   char   *PowerUpText = "Last Power-Up %02d:%02d %02d/%02d/%04d";
const   char   *strTSM = "TSM";
const   char   *strPC = "P.C.";
//const   char   *strNone = "None";

const char  *strHardwareSerialNumber = "Hardware Identification ";

const   char  *strPromptDisableLayering = "Disable Layering?";
const   char  *strPromptEnableLayering = "Enable Layering?";

const   char  *strCycleDiagCompHeader1 = "Comp   Set(kg)   Act(kg)   Time(PITs)  Time(s) Set(%) Act(%)";
const   char  *strCycleDiagCompHeader2 = "Comp   Targ Cts Act Cts  Raw Cts  H.Tare  Comp Tare  CPI ";

// //bb ***BB*** End of strings specifically used in Batch blender
const   char   *strWeightDisplayUnits[2] = {
   "kg",      // Metric
   "lb"   };   // Imperial



const   char   *strLineSpeed = "Line Speed(%s)";

const   char   *strWidth = "Width(%s)";

const   char   *strAutomatic[LANGUAGES_MAX] = {
      " Auto ",
      " Auto ",
      " Auto ",
      " Auto ",
      " Auto ",
      " Auto "
         };

const   char   *strManual[LANGUAGES_MAX] = {
      "Manual",
      "Manuel",
      "Manual",
      "Manual",
      "Manuale",
      "Manual"
         };

const   char   *strOpen[LANGUAGES_MAX] = {
      "Open",
      "Ouvert",
      "Aberto",
      "Abierto",
      "Aperto",
      "Auki"
         };





const   char   *strWeightConstant = "Weight Constant:";

const   char   *strHopperWeight = "Hopper Weight (%s)";

const   char   *strCalibrationWeight = "Calibration Weight (%s)";



const   char   *strNewLine = "\x0d\x0a";   // carriage return, line feed
const   char   *strCR = "\x0d";   // carriage return, line feed

// Configuration printed report
const   char   *strPrintConfigurationTitle[LANGUAGES_MAX] = {
      "SYSTEM CONFIGURATION",
      "CONFIGURATION SYSTEME ",
      "CONFIGURACAO SISTEMA",
      "CONFIGURACION SISTEMA",
      "CONFIGURAZIONE DI SISTEMA",
      "J\x8ERJESTELM\x8E KONFIGURAATIO"
         };

const   char   *strStartUp[LANGUAGES_MAX] = {
      "Start up",
      "Start up",
      "Start up",
      "Start up",
      "Start up",
      "Start up",
         };
const   char   *strRetry[LANGUAGES_MAX] = {
      " Blender has retried on component No ",
      " Blender has retried on component No ",
      " Blender has retried on component No ",
      " Blender has retried on component No ",
      " Blender has retried on component No ",
      " Blender has retried on component No ",
         };



const   char   *strPowerOnReset = "Power On Reset  ";


const   char   *strTargetWeightDiagnosticPrint[LANGUAGES_MAX] = {
      "Target weight $1",
      "Target weight $1",
      "Target weight $1",
      "Target weight $1",
      "Target weight $1",
      "Target weight $1",
         };

const   char   *strActualWeightDiagnosticPrint[LANGUAGES_MAX] = {
      "Actual Weight $2",
      "Actual Weight $2",
      "Actual Weight $2",
      "Actual Weight $2",
      "Actual Weight $2",
      "Actual Weight $2",
         };

const   char   *strTargetPercentageDiagnosticPrint[LANGUAGES_MAX] = {
      "Target %        ",
      "Target %        ",
      "Target %        ",
      "Target %        ",
      "Target %        ",
      "Target %        ",
         };

const   char   *strActualPercentageDiagnosticPrint[LANGUAGES_MAX] = {
      "Actual %        ",
      "Actual %        ",
      "Actual %        ",
      "Actual %        ",
      "Actual %        ",
      "Actual %        ",
         };

const   char   *strTargetWeightRetryDiagnosticPrint[LANGUAGES_MAX] = {
      "Target weight $r",
      "Target weight $r",
      "Target weight $r",
      "Target weight $r",
      "Target weight $r",
      "Target weight $r",
         };


const   char   *strActualWeightRetryDiagnosticPrint[LANGUAGES_MAX] = {
      "Actual Weight $r",
      "Actual Weight $r",
      "Actual Weight $r",
      "Actual Weight $r",
      "Actual Weight $r",
      "Actual Weight $r",
         };

const   char   *strTargetLoadCellCountsDiagnosticPrint[LANGUAGES_MAX] = {
      "Target load cell",
      "Target load cell",
      "Target load cell",
      "Target load cell",
      "Target load cell",
      "Target load cell",
         };
const   char   *strActualLoadCellCountsDiagnosticPrint[LANGUAGES_MAX] = {
      "Actual load cell",
      "Actual load cell",
      "Actual load cell",
      "Actual load cell",
      "Actual load cell",
      "Actual load cell",
         };


const   char   *strActualLoadCellRawCounts[LANGUAGES_MAX] = {
      "load cell counts",
      "load cell counts",
      "load cell counts",
      "load cell counts",
      "load cell counts",
      "load cell counts",
         };

const   char   *strSettledLoadCellCounts[LANGUAGES_MAX] = {
      "Av load cell cts ",
      "Av load cell cts ",
      "Av load cell cts ",
      "Av load cell cts ",
      "Av load cell cts ",
      "Av load cell cts ",
         };


const   char   *strActualLoadCellTareCounts[LANGUAGES_MAX] = {
      "Comp Tare counts",
      "Comp Tare counts",
      "Comp Tare counts",
      "Comp Tare counts",
      "Comp Tare counts",
      "Comp Tare counts",
         };

const   char   *strAToDStability[LANGUAGES_MAX] = {
      "ATD ",
      "ATD ",
      "ATD ",
      "ATD ",
      "ATD ",
      "ATD ",
         };


const   char   *strOpenTimeInPitsDiagnosticPrint[LANGUAGES_MAX] = {
      "Time(PITs)      ",
      "Time(PITs)      ",
      "Time(PITs)      ",
      "Time(PITs)      ",
      "Time(PITs)      ",
      "Time(PITs)      ",
         };

const   char   *strOpenTimeInSecondsDiagnosticPrint[LANGUAGES_MAX] = {
      "Time(Seconds)   ",
      "Time(Seconds)   ",
      "Time(Seconds)   ",
      "Time(Seconds)   ",
      "Time(Seconds)   ",
      "Time(Seconds)   ",
         };

const   char   *strFlowRateinGramsPerSecondDiagnosticPrint[LANGUAGES_MAX] = {
      "Flow (grams/sec)",
      "Flow (grams/sec)",
      "Flow (grams/sec)",
      "Flow (grams/sec)",
      "Flow (grams/sec)",
      "Flow (grams/sec)",
      };

const   char   *strFlowRateinCountPerSecondDiagnosticPrint[LANGUAGES_MAX] = {
      "Flow (counts/pit)",
      "Flow (counts/pit)",
      "Flow (counts/pit)",
      "Flow (counts/pit)",
      "Flow (counts/pit)",
      "Flow (counts/pit)",
      };


const   char   *strCycleCounter[LANGUAGES_MAX] = {
      "Cycle Counter   ",
      "Cycle Counter   ",
      "Cycle Counter   ",
      "Cycle Counter   ",
      "Cycle Counter   ",
      "Cycle Counter   ",
         };



const   char   *strBatchLength[LANGUAGES_MAX] = {
      "Batch Length  $7 ",
      "Batch Length  $7 ",
      "Batch Length  $7 ",
      "Batch Length  $7 ",
      "Batch Length  $7 ",
      "Batch Length  $7 ",
       };



const   char   *strTest[LANGUAGES_MAX] = {
      "1234567890123456789012345678901234567890",
      "1234567890123456789012345678901234567890",
      "1234567890123456789012345678901234567890",
      "1234567890123456789012345678901234567890",
      "1234567890123456789012345678901234567890",
         };


const   char   *strAToD[LANGUAGES_MAX] = {
      "a/d",
      "a/d",
      "a/d",
      "a/d",
      "a/d",
      "a/d",
         };


const   char   *strKilosperhourPrint[LANGUAGES_MAX] = {
      "kg/h Set:Instantaneous:Average $3   ",
      "kg/h Set:Instantaneous:Average $3   ",
      "kg/h Set:Instantaneous:Average $3   ",
      "kg/h Set:Instantaneous:Average $3   ",
      "kg/h Set:Instantaneous:Average $3   ",
      "kg/h Set:Instantaneous:Average $3   ",
         };


const   char   *strGramsPerMeterPrint[LANGUAGES_MAX] = {
      "g/m  Set:Instantaneous:Average $3 ",
      "g/m  Set:Instantaneous:Average $3 ",
      "g/m  Set:Instantaneous:Average $3 ",
      "g/m  Set:Instantaneous:Average $3 ",
      "g/m  Set:Instantaneous:Average $3 ",
      "g/m  Set:Instantaneous:Average $3 ",
         };



const   char   *strCycleTime[LANGUAGES_MAX] = {
      "Cycle Time",
      "Cycle Time",
      "Cycle Time",
      "Cycle Time",
      "Cycle Time",
      "Cycle Time",

         };



const   char   *strLoadTime[LANGUAGES_MAX] = {
      "Load Time",
      "Load Time",
      "Load Time",
      "Load Time",
      "Load Time",
      "Load Time",

         };


const   char   *strExtruderSpeed[LANGUAGES_MAX] = {
      "Extruder speed %",
      "Extruder speed %",
      "Extruder speed %",
      "Extruder speed %",
      "Extruder speed %",
      "Extruder speed %",

         };



const   char   *strKilosperhour[LANGUAGES_MAX] = {
      "kg/h  ",
      "kg/h  ",
      "kg/h  ",
      "kg/h  ",
      "kg/h  ",
      "kg/h  ",

         };

const   char   *strOrderReport[LANGUAGES_MAX] = {
      "        TSM ORDER REPORT",
      "        TSM ORDER REPORT",
      "        TSM ORDER REPORT",
      "        TSM ORDER REPORT",
      "        TSM ORDER REPORT",
      "        TSM ORDER REPORT",
       };

const   char   *strOrderStartedAt[LANGUAGES_MAX] = {
      "Order Started At :",
      "Order Started At :",
      "Order Started At :",
      "Order Started At :",
      "Order Started At :",
      "Order Started At :",
         };


const   char   *strOrderEndedAt[LANGUAGES_MAX] = {
      "Order Ended At   :",
      "Order Ended At   :",
      "Order Ended At   :",
      "Order Ended At   :",
      "Order Ended At   :",
      "Order Ended At   :",
         };

const   char   *strOrderName =   "Order Name       :";

const   char   *strOrderLength = "Order Length (m) :";

const   char   *strWeightTarget = "Component      Weight      Target % ";

const   char   *strWeightTotal[LANGUAGES_MAX] = {
      "Total",
      "Total",
      "Total",
      "Total",
      "Total",
      "Total",
         };

const   char   *strSystemConfig[LANGUAGES_MAX] = {
      "TSM SYSTEM CONFIGURATION",
      "System Config",
      "System Config",
      "System Config",
      "System Config",
      "System Config",
         };


const   char   *strMicraBatch = "MicraBatch";


const   char   *strOfflineHighLowMode[LANGUAGES_MAX] = {
      "High/Low Level Sensors        ",
      "High/Low Level Sensors",
      "High/Low Level Sensors",
      "High/Low Level Sensors",
      "High/Low Level Sensors",
      "High/Low Level Sensors",
         };

const   char   *strOfflineXXkgsMode[LANGUAGES_MAX] = {
      "XX Kgs",
      "XX Kgs",
      "XX Kgs",
      "XX Kgs",
      "XX Kgs",
      "XX Kgs",
         };

const   char   *strFillRetry[LANGUAGES_MAX] = {
      "Fill Retry (00 = Always)      ",
      "Fill Retry (00 = Always)",
      "Fill Retry (00 = Always)",
      "Fill Retry (00 = Always)",
      "Fill Retry (00 = Always)",
      "Fill Retry (00 = Always)",
         };

const   char   *strFirstComponentCompensation[LANGUAGES_MAX] = {
      "First Component Compensation  ",
      "First Component Compensation",
      "First Component Compensation",
      "First Component Compensation",
      "First Component Compensation",
      "First Component Compensation",
         };

const   char   *strHighLowRegrind[LANGUAGES_MAX] = {
      "High Low Regrind              ",
      "High Low Regrind",
      "High Low Regrind",
      "High Low Regrind",
      "High Low Regrind",
      "High Low Regrind",
         };

const   char   *strLayering[LANGUAGES_MAX] = {
     "Layering                      ",
     "Layering",
     "Layering",
     "Layering",
     "Layering",
     "Layering",
         };

const   char   *strMultiStageFilling[LANGUAGES_MAX] = {
      "Multi Stage Filling           ",
      "Multi Stage Filling",
      "Multi Stage Filling",
      "Multi Stage Filling",
      "Multi Stage Filling",
      "Multi Stage Filling",
         };

const   char   *str2stageFilling[LANGUAGES_MAX] = {
      "2 stage Filling",
      "2 stage Filling",
      "2 stage Filling",
      "2 stage Filling",
      "2 stage Filling",
      "2 stage Filling",
         };

const   char   *str3stageFilling[LANGUAGES_MAX] = {
      "3 stage Filling",
      "3 stage Filling",
      "3 stage Filling",
      "3 stage Filling",
      "3 stage Filling",
      "3 stage Filling",
         };


const   char   *strBatchesinKGHRAverage[LANGUAGES_MAX] = {
      "Batches in kg/h average       ",
      "Batches in kg/h average",
      "Batches in kg/h average",
      "Batches in kg/h average",
      "Batches in kg/h average",
      "Batches in kg/h average",
         };


const   char   *strMachineName[LANGUAGES_MAX] = {
      "Machine Name",
      "Machine Name",
      "Machine Name",
      "Machine Name",
      "Machine Name",
      "Machine Name",
         };


const   char   *strEnabled[LANGUAGES_MAX] = {
      "Enabled",
      "Enabled",
      "Enabled",
      "Enabled",
      "Enabled",
      "Enabled",
         };


const   char   *strDisabled[LANGUAGES_MAX] = {
      "Disabled",
      "Disabled",
      "Disabled",
      "Disabled",
      "Disabled",
      "Disabled",
         };


const   char   *strFillByWeightHeading[LANGUAGES_MAX] = {
      "Filling         ",
      "Filling         ",
      "Filling         ",
      "Filling         ",
      "Filling         ",
      "Filling         ",
         };


const   char   *strRetriesCounter[LANGUAGES_MAX] = {
      "Retries         ",
      "Retries         ",
      "Retries         ",
      "Retries         ",
      "Retries         ",
      "Retries         ",
         };


const   char   *strFillByWeightMessage[LANGUAGES_MAX] = {
      "FBW  ",
      "FBW  ",
      "FBW  ",
      "FBW  ",
      "FBW  ",
      "FBW  ",
         };

const   char   *strNoFillByWeightMessage[LANGUAGES_MAX] = {
      "Normal",
      "Normal",
      "Normal",
      "Normal",
      "Normal",
      "Normal",
         };

const   char   *strSamples[LANGUAGES_MAX] = {
      "Samples         ",
      "Samples         ",
      "Samples         ",
      "Samples         ",
      "Samples         ",
      "Samples         ",
          };

const   char   *strSampleCounter[LANGUAGES_MAX] = {
      "Sample  No.     ",
      "Sample  No.     ",
      "Sample  No.     ",
      "Sample  No.     ",
      "Sample  No.     ",
      "Sample  No.     ",
          };

const   char   *strMaximumWeight[LANGUAGES_MAX] = {
      "Maximum         ",
      "Maximum         ",
      "Maximum         ",
      "Maximum         ",
      "Maximum         ",
      "Maximum         ",
          };

const   char   *strMinimumWeight[LANGUAGES_MAX] = {
      "Minimum         ",
      "Minimum         ",
      "Minimum         ",
      "Minimum         ",
      "Minimum         ",
      "Minimum         ",
          };


const   char   *strMeanWeight[LANGUAGES_MAX] = {
      "Mean            ",
      "Mean            ",
      "Mean            ",
      "Mean            ",
      "Mean            ",
      "Mean            ",
          };

const   char   *strStandardDeviation[LANGUAGES_MAX] = {
      "Standard Dev    ",
      "Standard Dev    ",
      "Standard Dev    ",
      "Standard Dev    ",
      "Standard Dev    ",
      "Standard Dev    ",
          };

const   char   *strStandardDeviationPercentage[LANGUAGES_MAX] = {
      "Standard Dev %  ",
      "Standard Dev %  ",
      "Standard Dev %  ",
      "Standard Dev %  ",
      "Standard Dev %  ",
      "Standard Dev %  ",
          };


const   char   *strLogBatch[LANGUAGES_MAX] = {
      "Batch ",
      "Batch ",
      "Batch ",
      "Batch ",
      "Batch ",
      "Batch ",
          };

const   char   *strLogBlender[LANGUAGES_MAX] = {
      "Blender ",
      "Blender ",
      "Blender ",
      "Blender ",
      "Blender ",
      "Blender ",
          };

const   char   *strLogGeneral[LANGUAGES_MAX] = {
      "General ",
      "General ",
      "General ",
      "General ",
      "General ",
      "General ",
          };

const   char   *strLogControl[LANGUAGES_MAX] = {
      "Control ",
      "Control ",
      "Control ",
      "Control ",
      "Control ",
      "Control ",
          };

const   char   *strLogStatistics[LANGUAGES_MAX] = {
      "Statistics ",
      "Statistics ",
      "Statistics ",
      "Statistics ",
      "Statistics ",
      "Statistics ",
          };

const   char   *strSoftware[LANGUAGES_MAX] = {
      "Software",
      "Software",
      "Software",
      "Software",
      "Software",
      "Software",
          };

const   char   *strTimeOfBatch[LANGUAGES_MAX] = {
      "Date / Time",
      "Date / Time",
      "Date / Time",
      "Date / Time",
      "Date / Time",
      "Date / Time",
          };

const   char   *strDToAPerKg[LANGUAGES_MAX] = {
      "D/A per kg",
      "D/A per kg",
      "D/A per kg",
      "D/A per kg",
      "D/A per kg",
      "D/A per kg",
          };

const   char   *strRPMPerKg[LANGUAGES_MAX] = {
      "RPM per kg",
      "RPM per kg",
      "RPM per kg",
      "RPM per kg",
      "RPM per kg",
      "RPM per kg",
          };

const   char   *strControlType[LANGUAGES_MAX] = {
      "Control Type",
      "Control Type",
      "Control Type",
      "Control Type",
      "Control Type",
      "Control Type",
          };
const   char   *strControlModeDiag[LANGUAGES_MAX] = {
      "Control Mode",
      "Control Mode",
      "Control Mode",
      "Control Mode",
      "Control Mode",
      "Control Mode",
          };

const   char   *strControlSetpoint[LANGUAGES_MAX] = {
     "Control setpoint",
     "Control setpoint",
     "Control setpoint",
     "Control setpoint",
     "Control setpoint",
     "Control setpoint",
          };


const   char   *strTacho[LANGUAGES_MAX] = {
     "Tacho",
     "Tacho",
     "Tacho",
     "Tacho",
     "Tacho",
     "Tacho",
          };


const   char   *strExtruderRPM[LANGUAGES_MAX] = {
     "Extruder RPM ",
     "Extruder RPM ",
     "Extruder RPM ",
     "Extruder RPM ",
     "Extruder RPM ",
     "Extruder RPM ",
          };


const   char   *strFillAlarm               =  "Fill                      ";
const   char   *strZeroLinespeedAlarm      =  "Zero Line speed           ";
const   char   *strMixingMotorStoppedAlarm =  "Mixing Motor Stopped      ";
const   char   *strControlAlarm            =  "Control                   ";
const   char   *strInvalidCalibrationAlarm =  "Invalid Calibration       ";
const   char   *strLevelSensorMonitorAlarm =  "Level Sensor Monitor Alarm";
const   char   *strAtTargetWeightAlarm     =  "Reached Target Weight     ";
const   char   *strSEICommunicationsAlarm  =  "SEI Communications        ";
const   char   *strSEIResetAlarm           =  "SEI Reset                 ";
const   char   *strComponentNot100PercentAlarm =  "Components do not add to 100%";
const   char   *strStarvationAlarm          =  "Starvation               ";
const   char   *strLeakAlarm                =  "Material Leak from weigh hopper";
const   char   *strBinLowLevelAlarm         =  "Low Level Alarm on Bin   ";
const   char   *strTareAlarm                =  "Weigh Hopper did not empty";
const   char   *strShutDownAlarm            =  "Shut down                 ";
const   char   *strLicenseAlarm             =  "Software License          ";
const   char   *strHardwareLicenseAlarm     =  "Hardware License          ";
const   char   *strPercentageDeviationAlarm =  "Component Percentage Deviation";
const   char   *strAlarmNotRecognised       =  "Alarm Not Recognised      ";
const   char   *strDumpFlapHasNotOpenedAlarm =  "Dump Flap Not Opened      ";
const   char   *strSDCardAlarm              =  "SD Card Alarm             ";
const   char   *strAToDMaxExceededAlarm     =  "A To D Max Exceeded       ";
const   char   *strI2CStopped               =  "I2C Error                 ";
const   char   *strDeviationAlarm           =  "Deviation Alarm           ";

const   char   *strComponentOpenTimeTooShort =  "Open Time Too Short      ";
const   char   *strComponentOpenTimeTooLong  =  "Open Time Too Long       ";
const   char   *strInvalidConfigData         =  "Invalid Config Data      ";
const   char   *strAToDEOCNotCompleteAlarm   =  "AToD End Of Conversion   ";
const   char   *strAToDNegativePolarityAlarm =  "AToD Negative Polarity   ";


const   char   *strLiquidAdditiveSetKgHr    =  "Liquid Additive Set Kg/hr ";
const   char   *strLiquidAdditiveKgHrCalibration =  "Liquid Additive Kg/hr calibration ";
const   char   *strLiquidAdditiveDToA =  "Liquid Additive D/A ";


const   char   *strLossOfLockReset =  "Loss of PLL Lock Reset  ";
const   char   *strExternalDeviceReset =  "External Device Reset  ";
const   char   *strWatchdogReset =  "Watchdog Reset  ";
const   char   *strSoftwareReset =  "Software Reset  ";
const   char   *strLossOfClockReset =  "Loss of Clock Reset  ";


const   char   *strTemperature              =  "Temperature (C) ";

const   char   *strSkip = "\xa0 x 6";



#endif   // __TEXTSTRINGS_H__


