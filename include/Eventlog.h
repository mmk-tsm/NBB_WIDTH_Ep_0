//
// P.Smith                      1/7/08
// event log header file
//
// P.Smith                      1/10/08
// make ucAlarmCode an unsigned int.
//
// P.Smith                      26/11/08
// added cText to AddPowerOnResetToEventLog
//
// P.Smith                      15/1/09
// added void DumpEvent(void);
//
// P.Smith                      25/5/09
// added AddIC2EventToLog
//
// P.Smith                      1/9/09
// added AddLockToEventLog, AddUnLockToEventLog( void )
// AddModbusWriteEventToLog,WriteEventToSDCard(char *cBuffer);
//
// P.Smith                      8/9/09
// added  AddAToDInternalAToDTestPassedEventToLog, AddAToDInternalAToDTestFailedEventToLog(void);
//
// P.Smith                      17/12/09
// added ALARMCODETEXTSIZE definition
//
// P.Smith                      18/1/10
// added AddFileCouldNotBeOpenedEventToLog
//
// P.Smith                      15/3/10
// added AddSoftwareResetPendingEventToLog
//
// P.Smith                      25/3/10
// added AddEventToLogPowerUp
//
// P.Smith                      4/5/10
// added AddModbusTCPFatalErrorToLog
//////////////////////////////////////////////////////

#ifndef __EVENTLOG_H__
#define __EVENTLOG_H__

#include <basictypes.h>

void AddEventToLog(  char *cEventType,char *cDescriptor1,char *cDescriptor2,char *cDescriptor3,char *cDescriptor4 );
void AddAlarmOccurredToEventLog(unsigned int ucAlarmCode,  int nComponent );
void AddAlarmClearedToEventLog(unsigned int ucAlarmCode,  int nComponent );
char* GetAlarmText(unsigned int ucAlarmCode);
void AddBlenderRunToStopEventToLog( void );
void AddBlenderStopToRunEventToLog( void );
void AddBlenderToAutoEventToLog( void );
void AddBlenderToManualEventToLog( void);
void AddBlenderToManualControlAlarmEventToLog( void );
void AddBlenderToManualPotControlEventToLog( void );
void AddBlenderToManualSEIForcedExitEventToLog( void );
void AddBlenderToManualZeroLinespeedEventToLog( void );
void AddBlenderToManualLeakAlarmEventToLog( void );
void AddBlenderToManualExtruderstalledEventToLog( void );
void AddBlenderToManualBlenderPausedEventToLog( void );
void AddBlenderToManualIncDecSetpointErrorEventToLog( void );
void AddBlenderToManualStarvationEventToLog( void );
void AddBlenderInstantVolCorrDueToLspeedChangeEventToLog( void );
void AddPowerOnResetToEventLog( char *cText );
void AddRecipeChangeToEventLog( void );
void AddWeightHopperTareToEventLog( void );
void AddWeightHopperCalibratedToEventLog( void );
void AddSDCardLoadConfigCompleteToEventLog( void );
void AddSDCardLoadBackupConfigCompleteToEventLog( void );
void AddSDCardSaveConfigCompleteToEventLog( void );
void AddSDCardCopyConfigToBackupCompleteToEventLog( void );
void AddSDCardSaveConfigCompleteToEventLog( void );
void AddSDCardCopyConfigToBackupCompleteToEventLog( void );
void AddSDCardCleainingCycleInitiatedToEventLog( void );
void AddSDCardBypassOpenedToEventLog( void );
void AddSDCardBypassClosedToEventLog( void );
void AddOperatorPanelPageToEventLog( unsigned int nPageId );
void DumpEvent(void);
void AddIC2EventToLog( void );
void AddLockToEventLog( void );
void AddUnLockToEventLog( void );
void AddModbusWriteEventToLog(char *cText);
void WriteEventToSDCard(char *cBuffer);
void AddAToDInternalAToDTestPassedEventToLog(void);
void AddAToDInternalAToDTestFailedEventToLog(void);
void AddFileCouldNotBeOpenedEventToLog(void);
void AddSoftwareResetPendingEventToLog(void);
void AddEventToLogPowerUp(  char *cEventType,char *cDescriptor1,char *cDescriptor2,char *cDescriptor3,char *cDescriptor4 );
void AddModbusTCPFatalErrorToLog(void);


#define	ALARMCODETEXTSIZE				(20)






#endif   // __EVENTLOG_H__

