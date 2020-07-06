//	
// P.Smith                      16/2/06
// first pass at netburner hardware conversion.
// Bool -> BOOL
// #include <basictypes.h>
///////////////////////////////////////////////////////////

//*****************************************************************************
// MACROS
//*****************************************************************************
#ifndef __ASSREPORTS_H__
#define __ASSREPORTS_H__

//*****************************************************************************
// INCLUDES
//*****************************************************************************
#include <basictypes.h>

void CopyOrderReportToBuffer( void );   
void CopySystemConfigToBuffer( void );   
void StoreEnabledorDisabled( BOOL bEnabled );


#define     DATA_START_POSITION           25



#endif	// __ASSREPORTS_H__
