// 
// P.Smith                      24/4/07
// 
//////////////////////////////////////////////////////

#ifndef __MULTIBLENDMB_H__
#define __MULTIBLENDMB_H__

#include <basictypes.h>
void SaveMultiblendExtruderSetUp( void );
void CopyMultiblendSummaryDataToMB( void );
BOOL StartRecipe( int nRecipe );
void CopyMultiblendRecipesToMB( void );
void CopyMultiblendSetupToMB( void );
void CopyCurrentRecipeToMBlend( void );



#endif   // __MULTIBLENDMB_H__

