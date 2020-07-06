//////////////////////////////////////////////////////
// PITHandler.h
//
// Functions related to  PIT.
//
// M.McKiernan							20-07-2004
// First Pass
//										
//	P.Smith								4/10/04			
//
// M.McKiernan							24-11-2004
// Removed FiftyHzHandler - now in FiftyHz.c
// Removed UpdateAutoCycleSymbol( void );void RunStop( void );
// Removed TenHzHandler( void ); - now in TenHz.c
//
//	P.Smith								8/9/09			
// added CheckForAToDError
//////////////////////////////////////////////////////
#ifndef __PITHANDLER_H__
#define __PITHANDLER_H__

void PITHandler( void );
void BlenderPIT( void );
void CheckForAToDError( WORD wStatus );



#endif	// __PITHANDLER_H__
