//////////////////////////////////////////////////////
// Cleanio.h
//
// Activation of cleaning i/o
//
// P.Smith                          1/10/04
// 1st pass
//
// P.Smith                          7/11/05
// Add void CheckForValveDiagnostics ( int nCode )
//
// P.Smith                          20/11/07
// added SwitchAllCleansOff
//////////////////////////////////////////////////////

#ifndef __CLEANIO_H__
#define __CLEANIO_H__

void BlenderTopCleanOn  ( void );
void BlenderTopCleanOff ( void );
void BlenderMixerCleanOn ( void );
void BlenderMixerCleanOff ( void );
void LeftCornerCleanOn ( void );
void RightCornerCleanOn ( void );
void LeftCornerCleanOff ( void );
void RightCornerCleanOff ( void );
void CheckForCleanJetOnOff ( void );
void ToggleBlenderTopClean ( void );
void ToggleBlenderMixerClean ( void );
void ToggleRightCornerClean ( void );
void ToggleLeftCornerClean ( void );
void ToggleDump ( void );
void ToggleBypass ( void );
void ToggleOffline ( void );
void CheckForCleanTest ( void );
void DecimalPointKeyDiagnostics ( void );
void StartMouldClean( void );
void CheckForValveDiagnostics ( int nCode );
void SwitchAllCleansOff ( void );














#endif  // __CLEANIO_H__



