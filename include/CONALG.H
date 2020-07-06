//////////////////////////////////////////////////////
// ControlAlgorithm.h
//
//
// P.Smith                        1-02-2005
// First pass
//
// P.Smith                      16/2/06
// first pass at netburner hardware conversion.
//
// P.Smith                      16/2/06
// added check d/a limit CheckDToALimit1 - CheckDToALimit6
//
// P.Smith                      20/3/08
// added SetVolumetricExtruderSpeedForIncreaseDecrease 
// SetVolumetricExtruderSpeedZeroToTen( void );  
// LimitCorrectionForIncDec( unsigned int nNewTarget, unsigned int nOldTarget );  
// LimitCorrectionForZeroToTen( unsigned int nNewTarget, unsigned int nOldTarget );  
// LimitCorrection remove name change LimitCorrection1
//
// P.Smith                      8/6/09
// remove CheckDToALimit4
//////////////////////////////////////////////////////

#ifndef __CONALG_H__
#define __CONALG_H__

void ExtruderControlAlgotithm( void );
void InitiateCorrection( void );
void AutoManualToggle( void );
void ToAuto( void );
void ToManual( void );
void SetExtruderControlStartup( void );                    
void CalculateDAPerKg( void ); 
void ExtruderControlAlgotithm( void );
void CalculateDAPerKgCMR(void);
void EstimateKgPerHour(void);  
void SetVolumetricExtruderSpeed( void );  
void LimitCorrection1( void );
void SetVolumetricLineSpeed( void );  
void CheckForRoundRobinReset(void);  
void CheckDToALimit1( void );
void CheckDToALimit2( void );
void CheckDToALimit3( void );
void CheckDToALimit6( void );

void CheckForMaxExtruderPercentage( void );
void CheckAlarmLimit( void );   // Check for round robin reset.
void GoToAuto( void );
void CalculateCMRVoltage( void );                        
void ExtruderStallCheck( void );                        
void SetVolumetricExtruderSpeedForIncreaseDecrease( void );  
void SetVolumetricExtruderSpeedZeroToTen( void );  
unsigned int LimitCorrectionForIncDec( unsigned int nNewTarget, unsigned int nOldTarget );  
unsigned int LimitCorrectionForZeroToTen( unsigned int nNewTarget, unsigned int nOldTarget );  


#endif   // __CONALG_H__


