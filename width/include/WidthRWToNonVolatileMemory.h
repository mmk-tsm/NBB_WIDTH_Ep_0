
//
// P.Smith                     7/7/10
// added CheckForValidWidthConfigData
//////////////////////////////////////////////////////
#ifndef __WIDTHRWTONONVOLATILEMEMORY_H__
#define __WIDTHRWTONONVOLATILEMEMORY_H__

#include <basictypes.h>

int LoadWidthCalibrationFromEE( void );
void SaveWidthControlCalibrationToEE( void );
void CheckForValidWidthConfigData( void );


#define  WIDTH_CONTROL_CALIBRATION_DATA_START  7000     //


#endif	//__WIDTHRWTONONVOLATILEMEMORY_H__
