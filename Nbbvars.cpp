/////////////////////////////////////////////////
//
// P,Smith                          5/1/06
// create variables file for Netburner
//
/////////////////////////////////////////////////

//*****************************************************************************
// MACROS
//*****************************************************************************

#include "General.h"
#include <basictypes.h>

int         g_nPitDivider;                                  // divides pit to the required frequency for blender operation.
long        g_lRawADCountsLTC2415 = 0;  //global
unsigned int g_nLTC2415Err;  
long         lTemp; 
WORD         arrwRxBuffer[2];
BYTE         PCA9555InputReg[2];
BOOL           g_bL1Fitted;                     // link L1 fitted

     
