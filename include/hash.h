//////////////////////////////////////////////////////
//
// P.Smith                              29/1/08
// hash.h pass 1
//
// P.Smith                              1/4/09
// added GenerateHashForOrderWeight
//
// P.Smith                              26/6/09
// added GenerateHashForLoader,CheckHashForLoader
//
// P.Smith                              22/7/09
// added CheckHashForPulsing,GenerateHashForPulsing( void );
//
// P.Smith                              17/9/09
// add TSMHashOfByte
//
// P.Smith                              15/1/10
// added TSMHashOfInt, CheckHashForSDCardErrorCtr,GenerateHashForSDCardErrorCtr
//
// P.Smith                              17/2/10
// added CheckHashForEthernetIP
//
// P.Smith                              25/3/10
// added TSMHashS,TSMHashOfWord
//
// P.Smith                              27/4/10
// added WORD TSMHashOfLong,CheckHashHistoryLog,GenerateHashForHistoryLog

//////////////////////////////////////////////////////


#ifndef __HASH_H__
#define __HASH_H__

#include <basictypes.h>
#include "General.h"

WORD TSMHash(unsigned char * key, int len );
void GenerateHashForSoftwareID( void );
void CheckForValidSoftwareIDHash( void );
void GenerateHashForOrderWeight( void );
void GenerateHashForLoader( void );
void CheckHashForLoader( void );
void CheckHashForPulsing( void );
void GenerateHashForPulsing( void );
WORD TSMHashOfByte( BYTE nValue );
unsigned int TSMHashOfInt( unsigned int nValue );
void CheckHashForSDCardErrorCtr( void );
void GenerateHashForSDCardErrorCtr( void );
void CheckHashForEthernetIP( void );
WORD TSMHashS(char * key, int len );
WORD TSMHashOfWord( WORD nValue );
WORD TSMHashOfLong( long nValue );
void CheckHashHistoryLog( void );
void GenerateHashForHistoryLog( void );






 #endif   // __HASH_H__



