//////////////////////////////////////////////////////
// FiftyHz.h
//
// Allows the user to set the current time and date.
//
// M.McKiernan                          25-06-2004
// First pass
// M.McKiernan                          25-06-2004
// Added FiftyHzHandler, CheckPauseOnInput().
// P.Smith                              10-02-2005
// Added PollIncreaseDecrease,IncreaseKeyOn, DecreaseKeyOn
//
//
// P.Smith                              29-03-2005
// added void ReadMixerScrewRotationDetector ( void )
//////////////////////////////////////////////////////

#ifndef __FIFTYHZ_H__
#define __FIFTYHZ_H__

void ReadProcessMainLevelSensor( void );
void CheckIfPremLS( void);
void ReadHighLowLevelSensors( void);
void FiftyHzHandler( void );
void CheckPauseOnInput( void );
void PollIncreaseDecrease( void );
void IncreaseOn( void );
void DecreaseOn( void );
void ReadMixerScrewRotationDetector ( void );



#endif  // __FIFTYHZ_H__


