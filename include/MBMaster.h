//////////////////////////////////////////////////////
// MBMaster.h
//
// TSM Modbus master functions.
//
// M.McKiernan							20-01-2005
// First pass
//
// 
// P.Smith                      23/1/06
// first pass at netburner hardware conversion.
// BOOL -> BOOL
// removed interrupt from  void SerialHandler( void );
// 
// P.Smith                      24/1/06
// added #include "SerialStuff.h"
// 
// P.Smith                      2/2/07
// Removed unused functions
//
// M.McKiernan							30-04-2008
// Added CheckForValidIncDecTachoSetpoint( void );
//
// M.McKiernan							5-05-2009
// Added void DecideWhichLiWPollToUse( void )

//////////////////////////////////////////////////////

#ifndef __MBMASTER_H__
#define __MBMASTER_H__
#include "SerialStuff.h"

void ConfigureModbusMaster( void );
void TxRxOff( void );
void GenerateReadRequest( void );
int DecodeReadResponse( void );
void GenerateStatusReadRequest( void );
int DecodeStatusReadResponse( void );
void GenerateWriteRequest( void );
void GenerateSingleWriteRequest( int nIndex );
int DecodeWriteResponse( void );
BOOL AddToQueue( int nMessage, int nValue, structControllerData *pController );
int GetTopOfQueue( structControllerData *pController );
void RemoveFromQueue( structControllerData *pController );
void EmptyQueue( structControllerData *pController );
void PITMBMHandler( void );
void GetNextDisplayWriteIndices( void );
void GetNextAttributeIndices( void );

void DecideWhichLiWPollToUse( void );

void ModbusMasterTest( void );

BOOL CheckForValidIncDecTachoSetpoint( void );
#endif	// __MBMASTER_H__

