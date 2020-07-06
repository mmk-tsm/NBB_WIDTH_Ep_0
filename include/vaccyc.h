//////////////////////////////////////////////////////
// Vaccyc.h
//
// P.Smith                        9-9-2005
// First pass
//
// P.Smith                        22/6/07
// added CheckForCatchBoxVibration
//////////////////////////////////////////////////////

#ifndef __VACCYC_H__
#define __VACCYC_H__

void MonitorVacuumSequence( void );
void CheckForVacuumSequence( void );
void CheckForVacuumOn( void );
void CheckIfVacuumRequired( void );
void WaitForMaterialToBeVacuumed( void );
void VacuumOn( void );
void CheckForCatchBoxVibration( void );   



#endif   // __VACCYC_H__


