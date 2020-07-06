// Sdcard.h
//////////////////////////////////////////////////////
//
// P.Smith                      17/4/07
// added InitMMC,DisplayEffsErrorCode & MAX_EFFS_ERRORCODE
//
// P.Smith                      10/6/08
// added void LoadConfigFromSDCard, LoadBackupConfigFromSDCard,
// CopyConfigFileToBackup
//
// P.Smith                      17/7/08
// added WriteToSdCardFile, SDCardWritable, SDCardPresent
//
// P.Smith                      21/7/08
// added CopySDCardStatusToMB
//
// P.Smith                      25/11/08
// added WriteToSdCardFileNoAppend
//
// P.Smith                      25/6/09
// added WriteDiagnosticCSVDataToSDCard

// Eclipse  - functions now in FileSystemUtils.h
// M.McKiernan                      2/12/09
// Removed  DisplayEffsSpaceStats()
//
// P.Smith                      15/12/09
// removed  MAX_EFFS_ERRORCODE double definition
//
// P.Smith                      15/1/10
// WriteToSdCardFile modified to return a BOOL
// added StoreSDCardErrorDetailsForTextLog
// added MAX_SDCARD_WRITE_ATTEMPTS
////////////////////////////////////////////////////////
#ifndef _SDCARD_H__
#define _SDCARD_H__

#include "General.h"


void InitialiseSDCard( void );
void WriteDiagnosticDataToSDCard( void );
void InitMMC();
//void DisplayEffsErrorCode( int code );
int get_cd1( void );
void WriteMultiBlendDiagnosticsToSDCard( void );
void WriteControlDiagnosticDataToSDCard( void );
void LoadConfigFromSDCard( void );
void LoadBackupConfigFromSDCard( void );
void CopyConfigFileToBackup( void );
BOOL WriteToSdCardFile( char *cFileName, char *cString );
BOOL SDCardWritable( void );
BOOL SDCardPresent( void );
void CopySDCardStatusToMB( void );
void WriteToSdCardFileNoAppend( char *cFileName, char *cString );
void WriteDiagnosticCSVDataToSDCard( void );
void StoreSDCardErrorDetailsForTextLog( char *cFileName );


#define MAX_SDCARD_WRITE_ATTEMPTS   ( 10 )




#endif   /* _SDCARD_H */
