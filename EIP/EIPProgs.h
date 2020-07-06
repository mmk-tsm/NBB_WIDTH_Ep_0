//////////////////////////////////////////////////////
// EIPProgs.h
//
//
// M.McKiernan                          25-01-2010
// First pass
//
//
// M.McKiernan                          04-02-2010
// Added bool CheckEIPPercentagesTotal100(void)
// Added InitiateRecipeTransferFromEIP( void );
// Added HandleEIPControlCommands( void );
//////////////////////////////////////////////////////

#ifndef __EIPPROGS_H__
#define __EIPPROGS_H__

void EIPOneHzProgram(void);
void LoadEIPDownloadRecipe(void);
void CopyCurrentRecipeToEIPCurrentRecipeDataObj( void );
void CopyProductionDataToEIPProductionDataObj( void );
bool CheckEIPPercentagesTotal100(void);
void InitiateRecipeTransferFromEIP( void );
void HandleEIPControlCommands( void );

#endif  // __EIPPROGS_H__

