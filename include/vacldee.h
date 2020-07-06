/////////////////////////////////////////////////
// Vacldee.h
//
//
// P.Smith                      26/6/09
// added CheckForValidLoaderConfigData
//
// P.Smith                      1/7/09
// removed SetLoaderDefaults

/////////////////////////////////////////////////

//*****************************************************************************
// MACROS
//*****************************************************************************
#ifndef __VACLDEE_H__
#define __VACLDEE_H__

#define VACUUM_LOADER_CALIBRATION_DATA_START           5000    

int LoadLoaderCalibrationDataFromEEprom( void );
void SaveLoaderDataToEEprom( void );
BOOL CheckForValidLoaderConfigData( void );



#endif  // __VACLDEE_H__
