
// P.Smith                     7/5/10
// generate file
// P.Smith                     7/7/10
// added WidthFGOneMinute
//
// P.Smith                     10/6/11
// added CalculateWidthOffsetMinWidthInSteps
//
//////////////////////////////////////////////////////
#ifndef __WIDTHFOREGROUND_H__
#define __WIDTHFOREGROUND_H__

#include <basictypes.h>

void WidthForeground( void );
void CalculateLayFlatWidth( void );
void InitiateStandardise( void );
void SetMaxMotorPulses( unsigned int unMaxLeftPulses,unsigned int unMaxRightPulses);
void CalculateWidthAverage( void);
void ResetStandardiseCommand( void );
void WidthHourFG( void );
float CalculateMinimumWidth( unsigned int nWidth );
void SetSSIFWidthControlType( void);
void WidthFGOneMinute( void);
void CalculateWidthOffsetMinWidthInSteps( void);






#endif	//__WIDTHFOREGROUND_H__
