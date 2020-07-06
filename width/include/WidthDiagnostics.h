
// P.Smith                     2/7/10
// generate file
// M.McKiernan						22/5/2020.
// Added void WriteWidthMeasurementDiagnosticDataToSDCard( void ).
// void GenerateWidthMeasurementDiagnostics( void )
//////////////////////////////////////////////////////
#ifndef __WIDTHDIAGNOSTICS_H__
#define __WIDTHDIAGNOSTICS_H__

#include <basictypes.h>

#define		WIDTH_PRINT_BUFFER_SIZE			(5000)

void GenerateControlDiagnostics( void );
void WriteWidthDiagnosticDataToSDCard( void );
//wfh
void WriteWidthMeasurementDiagnosticDataToSDCard( void );
void GenerateWidthMeasurementDiagnostics( void );
#endif	//__WIDTHDIAGNOSTICS_H__
