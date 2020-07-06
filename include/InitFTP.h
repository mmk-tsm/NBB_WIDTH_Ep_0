// InitFTP.h
//////////////////////////////////////////////////////
//
// M.McKiernan                      07/6/07
// Initialise FTP
////////////////////////////////////////////////////////

// Eclipse  - functions now in FileSystemUtils.h
// M.McKiernan                      2/12/09
// Removed DumpDir() and  DisplayEffsSpaceStats()
// Removed void ReadWriteTest()

#ifndef _INITFTP_H__
#define _INITFTP_H__

//#include "General.h"


void ShowFileContents( int fdr );
void InitialiseFTP( void );
//void ReadWriteTest();
//void DisplayEffsSpaceStats();
//void DumpDir();
//void DisplayEffsErrorCode( int code );

#endif   /* _INITFTP_H */
