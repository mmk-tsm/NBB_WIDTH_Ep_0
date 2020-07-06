
//////////////////////////////////////////////////////
// P.Smith                      2/5/07
// added LoadOutputMapping
//
// P.Smith                      24/5/07
// added mapping check for 650 machine.
//
// P.Smith                      6/6/07
// remove printf, this was causing the if statement not to function properly
// 
// P.Smith                      22/6/07
// added mapping for TSM1000
// added default mapping if system mapping not allocated.
// proper names for blender types added
// M.McKiernan                   6/9/07
//          For a TSM3000 with <= 8 components use TSM3000_8C_3CleansMapping
// 
// P.Smith                      3/10/07
// allow for kwh mapping, for now check for multi blend mode to allow their mapping 
// to function, when Multiblend is set up, the older mapping is used.                     
// 
// P.Smith                      5/11/07
// added TSM350 mapping
//
// P.Smith                          30/1/08
// correct compiler warnings
//

//////////////////////////////////////////////////////


#include <stdio.h>

#include "SerialStuff.h"
#include "BatchMBIndices.h"
#include "BatchCalibrationFunctions.h"
#include "General.h"
#include "ConversionFactors.h"

#include "16R40C.h"
#include "ConfigurationFunctions.h"
#include "MBMHand.h"
#include "MBSHand.h"

#include "MBProgs.h"
#include "SetpointFormat.h"
#include "TimeDate.h"
#include "BatVars.h"
#include "BBCalc.h"
#include "MBTogSta.h"
#include "Alarms.h"
#include "KghCalc.h"
#include "BatVars2.h"
#include "Monbat.h"
#include "Blrep.h"
#include "MBProsum.h"
#include "Pause.h"
#include "CycleMonitor.h"
#include "TSMPeripheralsMBIndices.h"

#include <basictypes.h>
#include <string.h>
#include "NBBgpio.h"
#include "ConversionFactors.h"
#include "MultiblendMB.h"
#include "Mtttymux.h"
#include "Mapping.h"

extern CalDataStruct    g_CalibrationData;

//////////////////////////////////////////////////////
// LoadOutputMapping( void )           
// 
//
//////////////////////////////////////////////////////
void LoadOutputMapping( void )
{
    unsigned int i,j;
    if(g_CalibrationData.m_nBlenderType == TSM3000)
    {
      if(IsKWHBlender())      //kwh blender
      {
        for(i=0; i<SOURCEOUTPUTNO; i++)
        {    
            for(j=0; j<DEFINEDOUTPUTNO; j++)
            {
                g_BlenderMapping[i][j] = TSM3000KWHMapping[i][j];
            }
        }
      }
      else     // normal 3000 machine
      {
         for(i=0; i<SOURCEOUTPUTNO; i++)
        {    
            for(j=0; j<DEFINEDOUTPUTNO; j++)
            {
                g_BlenderMapping[i][j] = TSM3000_8C_3CleansMapping[i][j];
            }
        }
     
      }
    }
    else
    if((g_CalibrationData.m_nBlenderType == TSM650) && (g_CalibrationData.m_nComponents >4))
    {
       for(i=0; i<SOURCEOUTPUTNO; i++)
        {    
            for(j=0; j<DEFINEDOUTPUTNO; j++)
            {
                g_BlenderMapping[i][j] = TSM650Mapping6ComponentsWithCleaning[i][j];
            }
        }
    }
    else
    if((g_CalibrationData.m_nBlenderType == TSM650) && (g_CalibrationData.m_nComponents <=4))
    {
       for(i=0; i<SOURCEOUTPUTNO; i++)
        {    
            for(j=0; j<DEFINEDOUTPUTNO; j++)
            {
                g_BlenderMapping[i][j] = TSM650Mapping4ComponentsWithCleaning[i][j];
            }
        }
    }
    else
    if(g_CalibrationData.m_nBlenderType == TSM350) 
    {
       for(i=0; i<SOURCEOUTPUTNO; i++)
        {    
            for(j=0; j<DEFINEDOUTPUTNO; j++)
            {
                g_BlenderMapping[i][j] = TSM350Mapping[i][j];
            }
        }
    }
    
    else
// nbb--todo-- add easydrain    if(g_CalibrationData.m_nBlenderType == EASYDRAIN)
    if(g_CalibrationData.m_nBlenderType == TSM1000)
    {
       for(i=0; i<SOURCEOUTPUTNO; i++)
        {    
            for(j=0; j<DEFINEDOUTPUTNO; j++)
            {
                g_BlenderMapping[i][j] = TSM1000Mapping[i][j];
            }
        }
    }
    else
    {
       for(i=0; i<SOURCEOUTPUTNO; i++)
        {    
            for(j=0; j<DEFINEDOUTPUTNO; j++)
            {
                g_BlenderMapping[i][j] = TSMDefaultMapping[i][j];
            }
        }
    }
    
}



//////////////////////////////////////////////////////
// IsKWHBlender( void )           
// 
// checks for name being kwh, if it is, then return true
// if not return false
//////////////////////////////////////////////////////
BOOL IsKWHBlender( void )
{
    BOOL  IsKWH = TRUE;
    char	cBuffer[20];
    int    i;
    sprintf(cBuffer,"       KWH");
    for(i = 0; i<10; i++)
    {
        if(g_CalibrationData.m_cEmailBlenderName[i] != cBuffer[i])
        {
            IsKWH = FALSE;
        }    
    }
    return(IsKWH);
}
    
    
 




