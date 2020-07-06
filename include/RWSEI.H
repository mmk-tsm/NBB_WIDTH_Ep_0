//////////////////////////////////////////////////////
// ControlAlgorithm.h
//
//
// P.Smith                        1-02-2005
// First pass
//
// P.Smith                      4/10/05
// added void CalculateScrewSpeed( void )
// 
// P.Smith                      21/2/06
// first pass at netburner hardware conversion.
// removed unused prototypes.
// P.Smith                      21/2/06
// first pass at netburner hardware conversion.
//
// P.Smith                      7/12/07
// added SEIDecreaseLampOnFlag
//////////////////////////////////////////////////////

#ifndef __RWSEI_H__
#define __RWSEI_H__

void ReadSEIData( void );
void SEIAutoLampOn( void );
void SEIAutoLampOff( void );
void SEIIncreaseDecreaseLampsOnOff( void );
void SEIIncreaseLampOn( void );
void SEIIncreaseLampOff( void );
void SEIDecreaseLampOn( void );
void SEIDecreaseLampOff( void );
void SEIIncreaseLampOnFlag( void );
void SEIIncreaseLampOffFlag( void );
void SEIIncreaseLampOnFlag( void );
void SEIDecreaseLampOffFlag( void );
void SEILocalIncreaseLampOnFlag( void );
void SEILocalIncreaseLampOffFlag( void );
void SEIRemoteIncreaseLampOnFlag( void );
void SEIRemoteIncreaseLampOffFlag( void );
void SEIVoltageFollowerOn( void );
void SEIVoltageFollowerOff( void );
void ProcessSEIInputData( void );
void ProcessIncreaseDecrease( void );
void AverageAToDReading( void );
void StoreDToAReading ( void );
void FrontRollChange( void );
void BackRollChange( void );
void CalculateScrewSpeed( void );
void SEIDecreaseLampOnFlag( void );




#endif   // __RWSEI__


