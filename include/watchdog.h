//////////////////////////////////////////////////////
// P.Smith                      29/10/08
// watchdog timer header file
//
// M.McKiernan					3.3.2010
// Added functions EnableWatchdogNoReboot, EnableWatchdogAndReboot, DisableWatchdogNoReboot, DisableWatchdogAndReboot
// GetWatchdogStatus, InitialiseWatchdog
//////////////////////////////////////////////////////

#ifndef __WATCHDOG_H__
#define __WATCHDOG_H__
void Enable_watchdog( void );
void Disable_watchdog( void );
void Service_Watchdog( void );

void EnableWatchdogNoReboot( void );
void EnableWatchdogAndReboot( void );
void DisableWatchdogNoReboot( void );
void DisableWatchdogAndReboot( void );
bool GetWatchdogStatus( void );
void InitialiseWatchdog( void );
#endif  // __WATCHDOG_H__


