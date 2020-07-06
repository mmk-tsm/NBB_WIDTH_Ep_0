// 
// P.Smith                      20/6/07
//
// P.Smith                      28/6/07
// added ConvertOutputToLogicalFormat 
//
// P.Smith                      19/7/07
// LoadSetPointDataFromModbusTable added
//////////////////////////////////////////////////////

#ifndef __OPTIMISATIONMB_H__
#define __OPTIMISATIONMB_H__

#include <basictypes.h>

void CopyOptimisationDataToMB( void );
void SendOuputsToOptimisationCard( void );
unsigned int ConvertOutputToLogicalFormat ( unsigned int unTemp );       
void LoadSetPointDataFromModbusTable( void );


#endif   // __OPTIMISATIONMB_H__

