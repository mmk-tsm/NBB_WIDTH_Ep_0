//////////////////////////////////////////////////////
// ConfigurationFunctions.h
//
// Loads the configuration from EEPROM and initialises
// the necessary data structures
//
// M.Parks                              24-03-2000
// Moved from main to seperate function.
// M.McKiernan                              20-11-2003
//  Added prototype for SaveProcessData.
//  Added prototype FindNthLayerInGroupY
//   Added prototype CheckSystemConfiguration
// M.McKiernan                              23-01-2004
//  Added prototype CheckLoaderConfiguration
// M.McKiernan                              01-11-2004
//  Added prototypes ForegroundRecipeSave(),SaveAllCalibrationDataToEEprom(), LoadAllCalibrationDataFromEEprom
// Added definitions: #define   SAVE_RECIPE_START       1   SAVE_RECIPE_ONGOING 2
//
// P.Smith                                  6/9/05
// added void    SaveAllCalibrationDataToOnBoardEEprom( void );
// int     LoadAllCalibrationDataFromOnBoardEEprom( void );
//
// P.Smith                      23/1/06
// first pass at netburner hardware conversion.
// Bool -> BOOL
//
// P.Smith                      2/2/06
// #include <basictypes.h>
//
// P.Smith                      21/6/06
// removed LoadConfiguration
// removed SaveCommsConfiguration( void );
// removed void    SaveOptions( void );
//
// P.Smith                      30/6/06
// added CheckForValidCalibrationData( void );
//
// P.Smith                      27/3/08
// added SaveAllProcessDataToEEprom & LoadAllProcessDataFromEEprom
// 
// P.Smith                      1/4/09
// added LoadAllDescriptionDataFromEEprom
//
// P.Smith                      22/7/09
// void SaveAdditionalDataToEEPROM,LoadAdditionalDataFromEEPROM( void );
//
// P.Smith                      25/8/09
// added CheckForValidBatchSize
//
// P.Smith                      8/9/09
// added LoadAllCalibrationDataFromEEprom
//
// P.Smith                      10/9/09
// added bools to CheckForValidCalibrationData
//////////////////////////////////////////////////////
#ifndef __CONFIGURATIONFUNCTIONS_H__
#define __CONFIGURATIONFUNCTIONS_H__

#include <basictypes.h>

BOOL LoadRecipe( int nRecipeNo );

void    SaveRecipe( void );
void    ForegroundSaveRecipe( void );
void    SaveAllCalibrationDataToEEprom( void );
BOOL    LoadAllCalibrationDataFromEEprom( void );
void    SaveAllCalibrationDataToOnBoardEEprom( void );
int     LoadAllCalibrationDataFromOnBoardEEprom( void );
int    CheckForValidCalibrationData(BOOL bCheckBatchSize);
void SaveAllProcessDataToEEprom( void );
int LoadAllProcessDataFromEEprom( void );
int LoadAllDescriptionDataFromEEprom( void );
void SaveAdditionalDataToEEPROM( void );
int LoadAdditionalDataFromEEPROM( void );
BOOL CheckForValidBatchSize( void );




// Masks used forthe error status returned by the load configuration function
#define BAD_EEPROM_DATA_MASK    1
#define MEMORY_ALLOCATION_MASK  2

#define SAVE_RECIPE_START       1
#define SAVE_RECIPE_ONGOING 2

#endif	//__CONFIGURATIONFUNCTIONS_H__
