//////////////////////////////////////////////////////
// FGOneHz.h
//
//
// M.McKiernan                          26-11-2004
// First pass
//
// M.McKiernan                          15-08-2005
// Added CheckLicenseForFlashing 
//
// P.Smith                          20/3/08
// added CheckForAChangeInSEICalibrationData
//
// P.Smith                          23/6/08
// added AnalyseCommunications
//
// P.Smith                          2/3/09
// added CheckFor24HourRollOver
//
// P.Smith                          25/6/09
// call LoaderDiagnostics
//////////////////////////////////////////////////////

#ifndef __FGONEHZ_H__
#define __FGONEHZ_H__

void ForegroundOneHzProgram( void );
void ForegroundOneHzProgram2( void );
void ForegroundOneMinuteProgram( void );

void CheckForBypassActivation( void );
void CheckHighLowSensors( void );
void UpdateRegrindHiLoPercentage( void );   

void DecrementRollChangeLags( void );
void AnalyseRotationDetector( void );
void CheckMixerContactor( void );
void CheckIfTripped( void );
void CheckHighLowRegrindSensors( void );
void UpdateRegrindHighLow( void );
void CheckForSEITimeOuts ( void );
void CheckForMixerOff ( void );
void CheckLicenseForFlashing ( void );
void CheckForAChangeInSEICalibrationData ( void );  
void AnalyseCommunications ( void );  
void CheckFor24HourRollOver( void ); 
void LoaderDiagnostics( void ); 



#endif  // __FGONEHZ_H__


