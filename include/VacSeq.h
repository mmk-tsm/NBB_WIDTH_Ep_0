//////////////////////////////////////////////////////
// VacSeq.h
//

// M.McKiernan                          09-06-2009
// First pass
//
// M.McKiernan                          07-09-2009
// Added ClearAllLoaderAlarms( void ); see new 7.Sept.2009
// New functions - VacLoaderSystemEnterManualMode(  );  VacLoaderSystemExitManualMode( );
// Added function - CheckForLoaderOutputOn();
//////////////////////////////////////////////////////

#ifndef __VACSEQ_H__
#define __VACSEQ_H__

void InitialiseVacSystem( void );
void AddToFillTable( BYTE nLoaderNo );
unsigned char PopFromToFillTable(void );
void VACLoaderLogic(void);
void PumpControl(void);
void SetUpVAC8OutputWords( bool bDoLoaders );
void VacTenHzPrograms( void );
void VAC8OneHzPrograms( void );
void UpdateLoaderDataFromVAC8IO( void );
void CheckToFillTable( void );
void HandleFilterSequence( void );
void AddToFillTableWithPriority( BYTE nLoaderNo );
void VacSimulatorOneHz( void );

// new 7.Sept.2009
void ClearAllLoaderAlarms( void );
void VacLoaderSystemEnterManualMode( void );
void VacLoaderSystemExitManualMode( void );
bool CheckForLoaderOutputOn( void );

#endif  // __VACSEQ_H__


