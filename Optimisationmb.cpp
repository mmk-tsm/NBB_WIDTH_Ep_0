////////////////////////////////////////////////////////
// P.Smith                      13/6/07
// 1st pass copy of optimisation data to modbus
// correct copy flap status, the data should be shifted 8 times not 4 to
//
// P.Smith                      18/6/07
// copy in the second word of data
// copy flap status, enable, and fill to modbus table
// SendOuputsToOptimisationCard implemented
//
// P.Smith                      26/6/07
// It has been decided to use the inputs in sequence, input 1 is on conn2, input 2 is on conn3
// 1st output on conn2 pin 6, second output on conn2 pint 8. etc.
// this makes reading the flap status easier.
// ConvertOutputToLogicalFormat added to convert output format for lls card.
//
// P.Smith                      5/7/07
// Copy weight and length info for optimisation to modbus
// Copy all component weight variables to modbus table for display on operator panel.
// use more meaningful terms like FULL and EMPTY
//
// P.Smith                      17/7/07
// Transfer bin weight to modbus table
//
// P.Smith                      27/7/07
// added LoadSetPointDataFromModbusTable to copy setpoints continuously to the current order.
// added copy of g_fOptimisationOrderLength to modbus table, this is the value that 
// is written down from the modbus table continuously. This will be displayed on the 
// optimisation status page.
//
//
// M.McKiernan                      17/9/07 
// Removed g_arrnMBTable[] & g_arrnMBTableFlags[] - now in BatVars2
//
// P.Smith                          23/7/08
// remove g_arrnMBTable and array comms extern 
//
// P.Smith                          7/9/08
// a problem has been seen where memory (dhcp bool) is getting ovewritten
// this is due to data being copied into the end of an array g_bOptimisationFlapStatus
// this is corrected by only copying up to MAX_COMPONENTS instead of 16
////////////////////////////////////////////////////////

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
#include "Optimisation.h"
#include "OptimisationMB.h"


extern  CalDataStruct   g_CalibrationData;
extern  structSetpointData  g_CurrentRecipe;


//////////////////////////////////////////////////////
// CopyOptimisationDataToMB( void )           
// 
//
//////////////////////////////////////////////////////

void CopyOptimisationDataToMB( void )
{
    int i,nFlapStatus;
    union WordAndDWord uValue;

    g_nuOptimisationSensorInputState = g_arrnReadOptimisationMBTable[MB_LLS_INPUTS];
    // now split out the input state so that it can be read easily 
    
     nFlapStatus = g_nuOptimisationSensorInputState;
     for(i=0; i<8; i++)
    {
    if (nFlapStatus & 0x0001)
			g_bOptimisationFlapStatus[i] = EMPTY;
		else
			g_bOptimisationFlapStatus[i] = FULL;
    nFlapStatus >>= 1;
	}
    nFlapStatus = g_nuOptimisationSensorInputState >> 8;
    
    for(i=8; i<MAX_COMPONENTS; i++)
    {
    if (nFlapStatus & 0x0001)
			g_bOptimisationFlapStatus[i] = EMPTY;
		else
			g_bOptimisationFlapStatus[i] = FULL;
    nFlapStatus >>= 1;
	}
// now copy in flap status.
//
    for(i=0; i<MAX_COMPONENTS; i++)
    {
        g_arrnMBTable[OPTIMISATION_SUMMARY_FLAP_STATUS_1+i] = g_bOptimisationFlapStatus[i]; 
        g_arrnMBTable[OPTIMISATION_SUMMARY_ENABLE_STATUS_1+i] = g_bOptimisationLoaderEnable[i]; 
        g_arrnMBTable[OPTIMISATION_SUMMARY_FILL_STATUS_1+i] = g_bOptimisationLoaderFill[i]; 
    }
    // 
    for(i=0; i < MAX_COMPONENTS; i++)
    {
        g_arrnMBTable[OPTIMISATION_BIN_AND_LOADER_WEIGHT_1+i] = (int) ((g_fComponentBinAndLoaderWeight[i] + 0.005) * 100.0);
        g_arrnMBTable[OPTIMISATION_LOADER_FULL_WEIGHT_1+i] = (int) ((g_CalibrationData.m_fComponentLoaderFullWeight[i] + 0.005) * 100.0);
        g_arrnMBTable[OPTIMISATION_BIN_WEIGHT_1+i] = (int) ((g_fComponentBinWeight[i] + 0.005) * 100.0);

        g_arrnMBTable[OPTIMISATION_LOADER_WEIGHT_1+i] = (int) ((g_fComponentLoaderWeight[i] + 0.005) * 100.0);
        g_arrnMBTable[OPTIMISATION_LOADER_EMPTYING_RATE_WEIGHT_1+i] = (int) ((g_CalibrationData.m_fComponentLoaderEmptyingRate[i] + 0.0005) * 1000.0);
        
        uValue.lValue = (long)((g_fComponentRemainingOrderWeight[i] * 100) + 0.005);                                                       // Length is in m.
        g_arrnMBTable[OPTIMISATION_REMAINING_ORDER_WEIGHT_1+(i*2)] = uValue.nValue[0];          // M.s.Byte. - 
        g_arrnMBTable[OPTIMISATION_REMAINING_ORDER_WEIGHT_1+(i*2)+1] = uValue.nValue[1];  // L.s.Byte.
        g_arrnMBTable[OPTIMISATION_LOADER_EMPTYING_COUNTER_1+i] = g_nLoaderEmptyingCounter[i];

    }

    uValue.lValue = (long)((g_fRemainingOrderWeight * 100) + 0.005);                                                       // Length is in m.
    g_arrnMBTable[OPTIMISATION_REMAINING_ORDER_WEIGHT] = uValue.nValue[0];          // M.s.Byte. - 
    g_arrnMBTable[OPTIMISATION_REMAINING_ORDER_WEIGHT+1] = uValue.nValue[1];  // L.s.Byte.

    uValue.lValue = (long)(g_fRemainingOrderLength + 0.5);                                                       // Length is in m.
    g_arrnMBTable[OPTIMISATION_REMAINING_ORDER_LENGTH] = uValue.nValue[0];          // M.s.Byte. - 
    g_arrnMBTable[OPTIMISATION_REMAINING_ORDER_LENGTH + 1] = uValue.nValue[1];  // L.s.Byte.

    uValue.lValue = (long)(g_fOptimisationOrderLength + 0.5);                                                       // Length is in m.
    g_arrnMBTable[OPTIMISATION__ORDER_LENGTH] = uValue.nValue[0];          // M.s.Byte. - 
    g_arrnMBTable[OPTIMISATION__ORDER_LENGTH + 1] = uValue.nValue[1];  // L.s.Byte.

}



//////////////////////////////////////////////////////
// LoadSetPointDataFromModbusTable( void )           
// 
//
//////////////////////////////////////////////////////

void LoadSetPointDataFromModbusTable( void )
{
    union WordAndDWord uValue;

// Order Length (long)
        uValue.nValue[0] = g_arrnMBTable[BATCH_SETPOINT_ORDER_LENGTH];          // M.s.WORD. - 
        uValue.nValue[1] = g_arrnMBTable[BATCH_SETPOINT_ORDER_LENGTH + 1];  // L.s.word.
        g_CurrentRecipe.m_fOrderLength = (float)uValue.lValue;
}


//////////////////////////////////////////////////////
// SendOuputsToOptimisationCard( void )           
// 
//
//////////////////////////////////////////////////////
void SendOuputsToOptimisationCard( void )
{
    unsigned int unTemp;
    unTemp = ConvertOutputToLogicalFormat(g_unOptimisationLoaderOuputs);
    g_arrnWriteOptimisationMBTable[MB_LLS_COMMAND] = unTemp;     // 
}


/////////////////////////////////////////////////////
// ConvertOutputToLogicalFormat()       
//
// Converts output format to allow the bit positions to be logical
// from a wiring point of view on the VAC8.
// bit pattern    xyxyxyxyxyxyxyxy converted to xxxxxxxxyyyyyyyy
// this ensures that the outputs from the vac 8 are out a1,a2,a4....
// and out b1,b2,b3...... for bit patterns 01,02,04 etc......
// this ensures that the wiring pattern for the vac8 will make logical sense.
// input and output are on the same connector.
//
//////////////////////////////////////////////////////


unsigned int ConvertOutputToLogicalFormat ( unsigned int unTemp )       
{
    unsigned int    x,y,i,nOutput;
    x = 1;  // bit to be checked
    y = 1;  // oring data
    nOutput = 0;
    for(i= 0; i < 8; i++)
    {
        if ((unTemp & x) != 0)
        {
            nOutput |= y;
        }
        else
        {
            nOutput &= ~y;
        }

    x <<= 1;
    y <<= 2;
    }            
   
   // now do the other part of the word
   
    x = 0x100;
    y = 2;
    for(i= 0; i < 8; i++)
    {
        if ((unTemp & x) != 0)
        {
            nOutput |= y;
        }
        else
        {
            nOutput &= ~y;
        }
    x <<= 1;
    y <<= 2;
 
     }            
    return(nOutput);
}










