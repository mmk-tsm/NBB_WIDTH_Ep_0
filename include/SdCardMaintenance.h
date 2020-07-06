// 
// P.Smith                      11/7/08
// sd card maintenance header file
//////////////////////////////////////////////////////

#ifndef __SDCARDMAINTENANCE_H__
#define __SDCARDMAINTENANCE_H__

#include <basictypes.h>
void MaintainSDCard( int nCurrentMonth );
void MaintainEventLog( int nStartMonth,int nEndMonth );
void GetDirectoryandDeleteFiles( char *cName );
void MaintainBatchLog( int nStartMonth,int nEndMonth );
void CheckForSDCardMaintainenance( void );

#define     NOOFFILESTOKEEP         (3)



#endif   // __SDCARDMAINTENANCE_H__

