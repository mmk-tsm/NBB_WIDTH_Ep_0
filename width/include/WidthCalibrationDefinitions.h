//////////////////////////////////////////////////////
//
// P.Smith                            27/4/10
// defined ultrasonic variables required.
// mm / inches conversion factor
//
// P.Smith                            7/7/10
// added m_wUltrasonicAveraging, overrun, overrun deadband, bubble break
// auto ref calculation,calibration us from layflat,
//
// P.Smith                            8/7/10
// added m_bStandardiseOnAuto,m_bStandardiseOnTheHour,m_bAlarmInManual;
//
// P.Smith                            10/8/10
// added m_bFastCorrectionEnabled
// M.McKiernan						  20/04/2020
//mmk-wfh
//       float		m_fLength_AB;				// distance between sensors A(1) and B(2)
//       float		m_fLength_BC;				// distance between sensors B(2) and C(3)
//       float		m_fLength_AC;				// distance between sensors A(1) and C(3)
//       float		m_fLength_Ref;				// Reference Length
//        float		m_fSensorAngleA;			// angle of sesor A wrt horizontal.
//       float		m_fSensorAngleB;			// angle of sesor B wrt horizontal.
//       float		m_fSensorAngleC;			// angle of sesor C wrt horizontal.
//       BYTE 		m_nUSMethod;				// Ultrasonic measurement method.
//       float		m_fM2WidthOffset;			// Width offset for M2 measurement
//       BYTE			m_nM2WidthOffsetSign;		// sign of of offset
//////////////////////////////////////////////////////

#ifndef __WIDTHCALIBRATIONDEFINITIONS_H__
#define __WIDTHCALIBRATIONDEFINITIONS_H__

#include "General.h"
#include <basictypes.h>
#include "WidthDef.h"


typedef struct  {

       int          m_nSpare1;
       int          m_nSpare2;
       int          m_nSpare3;
       int          m_nSpare4;
       int          m_nSpare5;
       int          m_nSpare6;
       int          m_nSpare7;
       int          m_nSpare8;
       int          m_nSpare9;
       int          m_nSpare10;
       int          m_nSpare11;

       int          m_nSpare12;
       int          m_nSpare13;
       int          m_nSpare14;

       WORD			m_wWidthOffset;			// width offset in mm
       WORD			m_wWidthMode;				// layflat,ultrasonic, both
       WORD			m_wWidthType;				// standard,small line
       WORD			m_wLineType;		    // blown film / ibc
       WORD			m_wAlarmLimit;		    // alarm limit
       WORD			m_wProcessDelayInSeconds; // alarm limit
       WORD			m_wWidthIntegrationTimeinSeconds; //
       WORD			m_wWidthDeadband; //
       float		m_fNormalIncreaseControlGainPercentage; //
       float		m_fNormalDecreaseControlGainPercentage; //
       float		m_fFastCorrectedControlGainPercentage; //
       float		m_fWidthOffset;			// width offset in mm
       WORD			m_wMaximumScannerWidth; //
       WORD			m_wSlopFactorInmm; //
       WORD			m_wWidthAveragingInSeconds; //
       WORD			m_wunusedAlarmPercentageLimit;	// unused
       WORD         m_wUltrasonicAveraging;
       WORD         m_wWidthOverrun;
       WORD         m_wWidthOverrunDeadband;
       BOOL			m_bunusedspare1;
       BOOL         m_bStandardiseOnAuto;
       BOOL         m_bStandardiseOnTheHour;
       BOOL         m_bAlarmInManual;
       BOOL         m_bFastCorrectionEnabled;

       int          m_nuSpare5;
       int          m_nuSpare6;
       int          m_nuSpare7;

// ultrasonic stuff definitions
       float		m_fUltrasonicMMConversionFactor;			// counts to mm
       float		m_fDistanceToCentre[NUMBER_OF_ULTRASONICS_SENSORS];	// distance to centre
       WORD			m_wUltrasonicMMCalibratedAt;
       BOOL			b_USReferenceAutoCalibration;
       BOOL			b_USCalibrate_layflat_from_US;
       WORD			w_USCalibrate_Layflat_From_US_Interval;
       WORD			w_USCalibrateLayflatFromUSSampleNo;

//mmk-wfh
       float		m_fLength_Ref;				// Reference Length
       float		m_fLength_AB;				// distance between sensors A(1) and B(2)     // distance between sensors A(1) and B(2)
       float		m_fLength_BC;				// distance between sensors B(2) and C(3)     // distance between sensors B(2) and C(3)
       float		m_fLength_AC;				// distance between sensors A(1) and C(3)

       float		m_fSensorAngleA;			// angle of sesor A wrt horizontal.
       float		m_fSensorAngleB;			// angle of sesor B wrt horizontal.
       float		m_fSensorAngleC;			// angle of sesor C wrt horizontal.
       BYTE 		m_nUSMethod;				// Ultrasonic measurement method.
       float		m_fM2WidthOffset;			// Width offset for M2 measurement
       BYTE			m_nM2WidthOffsetSign;		// sign of of offset


       unsigned int  m_nChecksum;

    } WidthCalDataStruct;
/*
 * MODE            DS      1       ; LAYFLAY,ULTRASONIC,BOTH
LINETYPE        DS      1       ; LINE TYPE
SYSTEMTYPE      DS      1       ; SYSTEM TYPE


WIDOFF          DS     3        ;~FIXED WIDTH OFFSET
ALRMLT          DS     1        ;~ALARM LIMIT
PRCDLY          DS     1        ;~PROCESS DELAY
DISSEL          DS     1        ;~DISPLAY SELECT BYTE
WIDINT          DS     1        ;~WIDTH INTEGRATION TIME : WIDTH DEADBAND
LINNUM          DS     1        ;~LINE NUMBER
STPSPD          DS     1        ;~STEPPER SPEED CONTROL : UNUSED
CONGN1          DS     1        ;~CONTROL GAIN 1
CONGN2          DS     1        ;~CONTROL GAIN 2
SCRLMT          DS     2        ;~MAXIMUM SCANNER WIDTH
OPTFLG          DS     1        ;~OPTION FLAGS
SPARE6          DS     1        ;~UNUSED
SLPFAC          DS     1        ;~SLOP FACTOR
CORALM          DS     1        ;~NUMBER OF CORRECTIONS WHILST IN ALARM

WIDTHAVGENTRY   DS     1        ;~UNUSED
DEFCONTROLGAIN  DS     1        ;~UNUSED
;
CKSUM2          DS     2        ;~EEPROM DATA CHECKSUM

ALARMLIMIT1     DS      1       ;~ALARM LIMIT
ALARMLIMIT2     DS      1       ;~ALARM LIMIT #2
 *
 */

#endif //   __WIDTHCALIBRATIONDEFINITIONS_H__



