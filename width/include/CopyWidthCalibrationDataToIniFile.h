
// P.Smith                     26/5/10

//////////////////////////////////////////////////////
#ifndef __COPYWIDTHCALIBATIONDATATOINIFILE_H__
#define __COPYWIDTHCALIBATIONDATATOINIFILE_H__

#include <basictypes.h>
void CreateWidthDefaultIniFile (void );
int WriteWidthIniFile(char * ini_name);
void WriteWidthCalibrationDataToSDCard (void );
void DeleteWidthDefaultFile( void );
char* GetWidthModeText(unsigned int nMode);
char* GetWidthTypeText(unsigned int nMode);
char* GetWidthLineText(unsigned int nMode);



#endif	//__COPYWIDTHCALIBATIONDATATOINIFILE_H__
