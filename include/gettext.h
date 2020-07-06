//
// P.Smith                      1/7/08
// event log header file
//
// P.Smith                      1/10/08
// make ucAlarmCode an unsigned int.
//
// P.Smith                      24/4/09
// added GetSensitivityText, GetPurgeText, GetDiagnosticsText
// GetParityText,GetUSBDeviceTypeText
//
// P.Smith                      30/6/09
// added GetVacuumLoaderFilterTypeText
//
// P.Smith                      18/1/10
// added GetSpeedText
//
// P.Smith                      4/5/10
// added GetBaudRateText
//////////////////////////////////////////////////////

#ifndef __GETTEXT_H__
#define __GETTEXT_H__

#include <basictypes.h>


char* GetBlenderTypeText(unsigned int nMode);
char* GetBlenderModeText(unsigned int nMode);
char* GetMultiStageFillText(unsigned int nMode);
char* GetUnitsText(unsigned int nMode);
char* GetCleaningText(unsigned int nMode);
char* GetRecipeEntryModeText(unsigned int nMode);
char* GetLineSpeedSignalText(unsigned int nMode);
char* GetScrewSpeedSignalText(unsigned int nMode);
char* GetControlMethodText(unsigned int nMode);
char* GetRemoteModulesText(unsigned int nMode);
char* GetOfflineTypeText(unsigned int nMode);
char* GetControlTypeText(unsigned int nMode);
char* GetDeviationAlarmEnableText(unsigned int nMode);
char* GetSensitivityText(unsigned int nMode);
char* GetPurgeText(unsigned int nMode);
char* GetDiagnosticsText(unsigned int nMode);
char* GetParityText(unsigned int nMode);
char* GetUSBDeviceTypeText(unsigned int nMode);
char* GetVacuumLoaderFilterTypeText(unsigned int nMode);
char* GetSpeedText(BOOL bMode);
char* GetBaudRateText(unsigned int nMode);


#endif   // __GETTEXT_H__

