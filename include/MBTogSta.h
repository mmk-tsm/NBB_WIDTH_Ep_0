///////////////////////////////////////////////////////////
// MBTogSta.h
//
// ModBus toggle status routines.
//
// M.McKiernan                          15-11-2004
// First Pass
//
//
// P.Smith                              8-04-2005
// added void ClearShiftAccumulators ( void )
//
// P.Smith                              16/10/08
// added ZeroCountersCommand
//
// P.Smith                              1/9/09
// added InitiateCommand
//
// P.Smith                              10/9/09
// added ClearDiagnosticData
//
// P.Smith                              6/4/10
// added ENABLE_WATCH_DOG_AND_REBOOT,DISABLE_WATCH_DOG_AND_REBOOT
// TIME_TO_WATCH_DOG_REBOOT

///////////////////////////////////////////////////////////

//*****************************************************************************
// MACROS
//*****************************************************************************
#ifndef __MBTOGSTA_H__
#define __MBTOGSTA_H__

//*****************************************************************************
// INCLUDES
//*****************************************************************************


void MBToggleStatusCommands( void );
void MBCheckIncreaseDecrease( void );
void LoadExtruderRPMFromMB( void );
void PauseToggle( void );
void ClearCommsCounters( void );
void ClearShiftAccumulators ( void );
void ZeroCountersCommand ( int nCommand );
void InitiateCommand ( int nCommand );
void ClearDiagnosticData( void );

#define ENABLE_WATCH_DOG_AND_REBOOT	 (0xA5)
#define DISABLE_WATCH_DOG_AND_REBOOT (0xB5)
#define TIME_TO_WATCH_DOG_REBOOT     (0x03)  // seconds


#endif  // __MBTOGSTA_H__
