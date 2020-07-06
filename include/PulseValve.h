//
// P.Smith                      8/9/09
// added  AddAToDInternalAToDTestPassedEventToLog, AddAToDInternalAToDTestFailedEventToLog(void);
//
// P.Smith                      15/9/09
// name change CheckIfPulsingCalibrateIsRequired
//////////////////////////////////////////////////////

#ifndef __PULSEVALVE_H__
#define __PULSEVALE_H__

#include <basictypes.h>

void CheckIfPulsingCalibrateIsRequired( void );
BOOL PulsingTargetReached(unsigned int CompNo );
void RecalibratePulsingForComponents( void );



#endif   // __EVENTLOG_H__
