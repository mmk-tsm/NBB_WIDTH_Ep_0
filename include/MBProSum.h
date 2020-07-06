///////////////////////////////////////////////////////////
// MBProSum.h
//
// ModBus production summary routines.
//
// M.McKiernan                          22-11-2004
// First Pass  
//  
//
// P.Smith                              23-08-2005
// remove void CopyBBAuxToMB( void );
//
// 
// P.Smith                              14/09/05
// Name change for void CopyProdSummDataToMB( void )
// 
// P.Smith                      15/2/06
// first pass at netburner hardware conversion.
//
// P.Smith                      19/3/09
// added CopyModbusDiagnosticData, define NUMBER_OF_MODBUS_DIAGNOSTIC_CELLS
//
// P.Smith                      20/4/09
// add CopyEncryptionDataToMB
//
// P.Smith                   16/9/09
// Added CopyVAC8IODiagnosticsToMB()

///////////////////////////////////////////////////////////

//*****************************************************************************
// MACROS
//*****************************************************************************
#ifndef __MBPROSUM_H__
#define __MBPROSUM_H__

//*****************************************************************************
// INCLUDES
//*****************************************************************************


void CopyProdSummDataToMB( void );
void CopyADCountsToMB( void );
void CheckForCompStatusOff( void );
void CopyFBWtsToMB( void );
void CopyFrontComponentWeightsToMB( void );
void CopyBackComponentWeightsToMB( void );
void CopyOrderComponentWeightsToMB( void );
void CopyShiftComponentWeightsToMB( void );
void CopyShiftLengthToMB( void );
void CopyModbusDiagnosticData( void );
void CopyEncryptionDataToMB( void );
void CopyVAC8IODiagnosticsToMB(void);


#define NUMBER_OF_MODBUS_DIAGNOSTIC_CELLS      (20)

#endif  // __MBPROSUM_H__
