
// P.Smith                      4/10/07
// CreateDefaultIniFile used instead of UpDateIniFileWithCalibrationData
//

// P.Smith                      10/1/08
// added WriteCalibrationDataToSDCard
//
// P.Smith                      29/9/08
// added DeleteDefaultFile
//////////////////////////////////////////////////////
#ifndef __COPYCALIBRATIONTOINIFILE_H__
#define __COPYCALIBRATIONTOINIFILE_H__

#include <basictypes.h>

void CreateDefaultIniFile( void );
int WriteBatchIniFile(char * ini_name);
void WriteCalibrationDataToSDCard (void );
void DeleteDefaultFile( void ); 



#endif	//__COPYCALIBRATIONTOINIFILE_H__
