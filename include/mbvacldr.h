/////////////////////////////////////////////////////
// MBVacLdr.h
// initial definition of mb table for vacuum loading.

//////////////////////////////////////////////////////

#ifndef __MBVACLDR_H__
#define __MBVACLDR_H__

void LoadVacuumLoaderCalibrationParametersFromMB( void ); 
void CopyVacuumLoaderCalibrationParametersToMB(void);
void CopyVacuumLoaderSummaryDataToMB(void);
void VacuumLoaderOneHertz(void);



#endif  // __MBVACLDR_H__
