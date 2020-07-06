//////////////////////////////////////////////////////
// Pause.h
//
// Pause functions
//
// M.McKiernan							17-09-2004
// First pass
//
// P.Smith							    29/5/06
// added SwitchPauseOffAndMakeCycle( void );
//
///////////////////////////////////////////////////////

#ifndef __PAUSE_H__
#define __PAUSE_H__

void PauseToggle( void );
void PauseOn( void );
void PauseOff( void );
void ImmediatePauseOn( void );
void OffLinePauseOn( void );
void VacInhibitOn( void );
void VacInhibitOff( void );
void EndOfCyclePauseOn( void );
void CheckForEndOfCyclePause( void );
void SwitchPauseOffAndMakeCycle( void );


#endif	// __PAUSE_H__


