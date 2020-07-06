///////////////////////////////////////////////////////////
// MBProgs.h
//
// A library of functions for accessing the ModBus table
//
//
//  M.McKiernan                         11-06-2004
//  
//      M.McKiernan                    04-10-2004
//      Added CopySystemConfigurationToMB().
//      Added CopyLiquidCalibrationToMB().
//  Added CopySpeedParametersToMB()
//      Added LoadSystemConfigurationFromMB( ), CopyVersionDataToMB()
//      Added CopyCalibrationDataToMB(), CopyComponentConfigurationToMB()
//      Added   CopyMeasurementAndControlParametersToMB( ), CopyHopperConstantsToMB(), CopyWeightConstantsToMB()
//      Added LoadComponentConfigFromMB( ), LoadWeightCalibrationFromMB, LoadHopperCalibrationFromMB()
//      Added LoadSpeedParametersFromMB(),  LoadMeasParametersFromMB(), LoadLiquidCalibrationFromMB()
//     Added LoadCalibrationWeightFromMB()
//    Added  CopyADCountsToMB( void );
//    Added  CopyProdSummDataToMB1Hz( void );
//    Added  CopyActualPercentagesToMB( void );
//      M.McKiernan                    22-11-2004
//      Removed CopyADCountsToMB( void ), CopyProdSummDataToMB1Hz
//
//  P.Smith                             10-3-2005             
//  Added void CopyWeightsToMB( void )
//
//  P.Smith                             23-8-2005             
// Added void SetInProgressStatus( void ) ClearInProgressStatus( void )
//
// P.Smith                          23-08-2005
// remove void CopyBBAuxToMB( void );
//
// P.Smith                          18/11/05
// change LAG_TIME_FOR_LOADING_MB_SETPOINTS to 30 seconds
//
// P.Smith                          31/5/06
// added void SetUpConversionFactors( void );
//
// P.Smith                          15/6/06
// added void LoadEmailConfigFromMB( void );
// added void CopyEmailConfigDataToMB( void );
//
// P.Smith                          28/3/08
// added CopySdCardDetailsToMB,void PollUSBMB( void );
//
// P.Smith                          7/5/08
// added void StoreMBSetpointHistoryArray( void );
// added CompareMBSetpointHistoryArray( void );
//
// P.Smith                          12/6/08
// added CopyAlarmsToIndividualMBWords
//
// P.Smith                          30/6/08
// add InitialiseCommsRecipe
//
// P.Smith                          21/7/08
// remove PollUSBMB
//
// P.Smith                          11/12/08
// added void LoadSystemConfiguration3FromMB, CopySystemConfiguration3ToMB
//
// P.Smith                          21/1/09
// added CopyFunctionRunningToMB
//
// P.Smith                          1/4/09
// added LoadPanelVersionNumber
//
// P.Smith                          1/7/09
// added CopyConfigDataToMB
//
// P.Smith                          22/7/09
// added LoadICSRecipeMBData,CopyICSRecipeMBData( void );

//////////////////////////////////////////////////////////


//*****************************************************************************
// INCLUDES
//*****************************************************************************

#ifndef __MBPROGS_H__
#define __MBPROGS_H__

// #define  EEPROM_WRITE_IN_PROGRESS    0x01    // WRITE IN PROGRESS.

#define LAG_TIME_FOR_LOADING_MB_SETPOINTS       300         // NB: lag period in 10ths of second. (setpoints discarded if time expires)

// Function declarations
void    InitialiseMBTable( void );
void    InitialiseMBTableFlags( void );
char CheckMBValidData(void);                                
void DecrementMBLags( void );
void CopyTargetPercentagesToMB( void );
void LoadTargetPercentagesFromMB( void );
void CopyTargetThruputsToMB( void );
void CopyOrderInfoToMB( void );
void CopySetpointsToMB( void );
void PostMBWrite( void );
void LoadTimeFromMB( void );
void CopyTimeToMB( void );
void CopyComponentWeightsToMB( void );

void CopySystemConfigurationToMB( void );
void CopyLiquidCalibrationToMB(void);
void CopySpeedParametersToMB(void);
void LoadSystemConfigurationFromMB( void );
void CopyVersionDataToMB( void );
void CopyCalibrationDataToMB( void );
void CopyComponentConfigurationToMB( void );

void CopyMeasurementAndControlParametersToMB( void );
void CopyHopperConstantsToMB( void );
void CopyWeightConstantsToMB( void );
void LoadComponentConfigFromMB( void );
void LoadWeightCalibrationFromMB( void );
void LoadHopperCalibrationFromMB( void );
void LoadSpeedParametersFromMB( void );
void LoadMeasParametersFromMB( void );
void LoadLiquidCalibrationFromMB( void );
void LoadCalibrationWeightFromMB( void );
// void CopyADCountsToMB( void );
// void CopyProdSummDataToMB1Hz( void );
void CopyActualPercentagesToMB( void );

void LoadOrderInfoFromMB( void );
void LoadTargetThruputsFromMB( void );
void CopyWeightsToMB( void );
void CopyDACountsToMB( void );
void CopyHopperSetWeightsToMB( void );
void CopyHopperWeightToMB( void );
void CopyDiagnosticsToMB( void );
void SetInProgressStatus( void );
void ClearInProgressStatus( void );
void CheckForPendingRecipe( void );
void CopyBBAuxToMB( void );
void LoadPercentageTargetsFromComms( void );
void InitiateRecipeTransferToCurrentOrder( void );
void SetUpConversionFactors( void );
void LoadEmailConfigFromMB( void );
void CopyEmailConfigDataToMB( void );
void CheckForRecipeDownLoad( void );
void CopySdCardDetailsToMB( void );
//void PollUSBMB( void );
void StoreMBSetpointHistoryArray( void );
BOOL CompareMBSetpointHistoryArray( void );
void CopyAlarmsToIndividualMBWords( void );
void InitialiseCommsRecipe( void );
void LoadSystemConfiguration3FromMB( void );
void CopySystemConfiguration3ToMB( void );
void CopyFunctionRunningToMB( void );
void LoadPanelVersionNumber( void );
void CopyConfigDataToMB( void );
void LoadICSRecipeMBData( void );
void CopyICSRecipeMBData( void );






#endif	// __MBPROGS_H_

