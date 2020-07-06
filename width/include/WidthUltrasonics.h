
// P.Smith                     7/7/10
// define ULTRASONIC_AVERAGING_FACTOR, add AutoCalibrateReferenceSensor

// P.Smith                     19/7/10
// change CalculateDistanceToCentre to pass float

// M.McKiernan		wfh			7/5/2020.
//void SetA0Lo( void );
//void SetA0Hi( void );
//void USMuxClockLo( void );
//void USMuxClockHi( void );
//void DisableUSMux1(void);
//void EnableUSMux1(void);
// wfh 18.5.2020
// added void CalculateM2Offset( float fLayflatmm);
// 8.6.2020
// void TestDrivingLEDs(void).
//////////////////////////////////////////////////////
#ifndef __WIDTHULTRASONICS_H__
#define __WIDTHULTRASONICS_H__

#include <basictypes.h>

#define		ULTRASONIC_SAMPLE_AVERAGE		(64)
#define		ULTRASONIC_AVERAGING_FACTOR		(10)


void SetUltrasonicMux( void );
void DisplayLedStatus( void );
void AdvanceToNextChannel( void );
void AverageUltrasonicReading( void );
void CalibrateUltrasonicReferenceSensor(WORD wWidthMMCalibrateAt);
void CalculateUltrasonicDistances(void);
void CalculateDistanceToCentre(float fDiamter);
void AutoCalibrateReferenceSensor(void);

void SetA0Lo( void );
void SetA0Hi( void );
void USMuxClockLo( void );
void USMuxClockHi( void );
void DisableUSMux1(void);
void EnableUSMux1(void);

//wfh
void CalculateM2Offset( float fLayflatmm);
void TestDrivingLEDs(void);
void PITTestForUSMuxLEDs(void);

#endif	//__WIDTHULTRASONICS_H__
