////////////////////////////////////////////////////////////////
// ModbusMasterHandler.h
//
//
// 
//
// M.Parks					24-01-2000
// 
// P.Smith                      24/1/06
// first pass at netburner hardware conversion.
// Bool -> BOOL

////////////////////////////////////////////////////////////////
#ifndef __MBMHAND_H__
#define __MBMHAND_H__

#include "SerialStuff.h"


void	InitialiseModbusMaster( void );
void	TXHandler( void );
void	RXHandler( void );
void	GenerateReadRequest( void );
int		DecodeReadResponse( void );
void	GenerateWriteRequest( void );
void	GenerateSingleWriteRequest( int nIndex );
int		DecodeWriteResponse( void );
void	GenerateStatusReadRequest( void );
int		DecodeStatusReadResponse( void );

void	TxOn( void );
void	RxOn( void );
void	TxRxOff( void );

BOOL	AddToQueue( int nMessage, int nValue, structControllerData *pController );
BOOL	AddToQueueWithPriority( int nMessage, int nValue, structControllerData *pController );
int		GetTopOfQueue( structControllerData *pController );
void	RemoveFromQueue( structControllerData *pController );
void	EmptyQueue( structControllerData *pController );


#endif	// __MBHAND_H_
