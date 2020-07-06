
// P.Smith                     7/5/10
// generate file
//
// P.Smith                     7/7/10
// added SetStartUpMode,CryovacControlAlgorithm,SetRapidIntegration
// added CheckStandardAlarmBands,CheckCryovacAlarmBands
//
// M.McKiernan					6/5/2020
// //wfh - MMK 6.5.2020
// Added void WidthManualFastIncrease( void );
//////////////////////////////////////////////////////
#ifndef __WIDTHCONTROLALGORITHM_H__
#define __WIDTHCONTROLALGOTITHM_H__

#include <basictypes.h>

void WidthControlAlgorithm( void );
void WidthAutoManualToggle( void );
void WidthToManual( void );
void WidthToAuto( void );
void WidthIncreaseOn( void );
void WidthIncreaseOff( void );
void WidthDecreaseOn( void );
void WidthDecreaseOff( void );
void WidthFastCorrectionIncreaseOn( void );
void WidthFastCorrectionIncreaseOff( void );
void SwitchOffAllControlOutputs( void );
void ResetWidthIntegration( void );
void WidthManualIncrease( void );
void WidthManualDecrease( void );
void StandardControlAlgorithm( void );
void WidthFastCorrectionIncreaseOn( void );
void WidthFastCorrectionIncreaseOff( void );
void SwitchWidthAlarmOnOrOff( void );
void WidthAlarmOn( void );
void WidthAlarmOff( void );
void SetStartUpMode( void );
void CryovacControlAlgorithm( void );
void SetRapidIntegration( void );
void CheckStandardAlarmBands( WORD wPositiveError);
void CheckCryovacAlarmBands( WORD wPositiveError);


//wfh - MMK 6.5.2020
void WidthManualFastIncrease( void );



#endif	//__WIDTHCONTROLALGOTITHM_H__
