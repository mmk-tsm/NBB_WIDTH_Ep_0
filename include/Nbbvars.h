/////////////////////////////////////////////////
//
// P,Smith                          5/1/06
// create variables file for Netburner
//
/////////////////////////////////////////////////

//*****************************************************************************
// MACROS
//*****************************************************************************
#ifndef __NBBVARS_H__
#define __NBBVARS_H__


#include "General.h"
#include <basictypes.h>
#include "NBBVars.h"

extern  int         g_nPitDivider;                                  // divides pit to the required frequency for blender operation.
extern  long        g_lRawADCountsLTC2415;  //global
extern  unsigned int g_nLTC2415Err;  //
extern  long        lTemp; 
extern  WORD         arrwRxBuffer[];
extern  BYTE         PCA9555InputReg[2];
extern  BOOL           g_bL1Fitted;                     // link L1 fitted
 


#endif //__NBBVARS_H_
