//////////////////////////////////////////////////////
// ControlAlgorithm.h
//
//
// P.Smith                        1-02-2005
// First pass
//
//
// P.Smith                        6-07-2005
// Length accumulation.
//
// P.Smith                      21/2/06
// first pass at netburner hardware conversion.
//
// P.Smith                      14/11/06
// added COMMUNICATION_LINE_SPEED & PULSES_LINE_SPEED 
// added IntegrateCommsLineSpeed
//
// P.Smith                      21/2/07
// Correct PULSES_LINE_SPEED & COMMUNICATION_LINE_SPEED  definition
//
// P.Smith                      1/3/07
// LASTLINESPEEDSIGNALTYPE defined
///////////////////////////////////////////////////////

#ifndef __LSPEED_H__
#define __LSPEED_H__

void AccumulateSEIPulses( void );
void LineSpeedCalculation( void );
void AddPulsesToLineSpeedCounters( int nPulses );
void CheckForZeroLineSpeed( void );
void CheckForLineSpeedChange( void );
void BatchLengthCalculation( void );
void CalculateAdditionalSEIPulses( void );
void ReactToChangeInLineSpeed( void );
void SetSpeedForSetPointChange( void );
void AccumulateFrontLength( void );
void AccumulateBackLength( void );
void IntegrateCommsLineSpeed( void );


#define     PULSES_LINE_SPEED               0
#define     COMMUNICATION_LINE_SPEED        1
#define     LASTLINESPEEDSIGNALTYPE         COMMUNICATION_LINE_SPEED      

#endif   // __LSPEED_H__


