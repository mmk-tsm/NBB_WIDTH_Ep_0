
// P.Smith                    18/5/10
// generate file
//
// P.Smith                    7/7/10
// added CheckForNewWidthSetPoint & MonitorBubbleBreak
//
// P.Smith                    9/7/10
// added CheckForCryovacAlarmPeriod

// M.McKiernan					19/2/2020
// Added void WidthCalculation3Points( void )
// 16/3/2020
// Added float CalculateRadiusFrom3Points( float Xa, float Ya, float Xb, float Yb, float Xc, float Yc )
//
//wfh
// void CalculateBubbleCoordinates( void );
// void CalculateSensorCoordinates( void );
// float  CalculateTriangleAngleA( float fAdj1, float fAdj2, float fOpp);
// void CalculateSensorAngles( float fRadius );
//////////////////////////////////////////////////////
#ifndef __WIDTH_H__
#define __WIDTH_H__

#include <basictypes.h>

void WidthIntegration( void );
void InitialisationWhileStandardising( void );
void CheckForChangeInErrorDirection( void );
void CheckForNewWidthSetPoint( void );
void MonitorBubbleBreak( void );
void CheckForCryovacAlarmPeriod( void );
void WidthCalculation3Points( void );
float CalculateRadiusFrom3Points( float Xa, float Ya, float Xb, float Yb, float Xc, float Yc );
//wfh
void CalculateBubbleCoordinates( void );
void CalculateSensorCoordinates( void );
float  CalculateTriangleAngleA( float fAdj1, float fAdj2, float fOpp);
void CalculateSensorAngles( float fRadius );
void CopyCalculatedSensorAnglesToMB( void );

#endif	//__WIDTH_H__
