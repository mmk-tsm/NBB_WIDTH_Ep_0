//////////////////////////////////////////////////////
//  P.Smith                                 2/3/5
// if start up reset kg/h round robin.
//
//  P.Smith                              14/09/05
//  Name change for void CopyProdSummDataToMB( void )
//
// P.Smith                      22/2/06
// first pass at netburner hardware conversion.
// included  <basictypes.h>
//
// P.Smith                      28/2/06
// CopyKghToMB();                  // copy kg/hr                       // asm= CPYKGHMB
// CopyGPMToModbus();              // copy grams per meter to modbus   //  ASM = CPYGPMMB 
// #include "KghCalc.h"
// #include "Gpmcalc.h"
//
// M.McKiernan                      17/9/07 
// Removed g_arrnMBTable[] & g_arrnMBTableFlags[], now in BatVars2
//
// P.Smith                          24/6/08
// remove g_arrnMBTable
//////////////////////////////////////////////////////

//*****************************************************************************
// INCLUDES
//*****************************************************************************
#include "General.h"
#include "BatVars.h"
#include "KghCalc.h"
#include "ConversionFactors.h"
#include "BatchCalibrationFunctions.h"
#include "SetpointFormat.h"
#include "BatchMBIndices.h"
#include "ExStall.h"
#include "Mbprogs.h"
#include "Gpmcalc.h"
#include "MBProsum.h"
#include "BatVars2.h"
#include <basictypes.h>




extern CalDataStruct    g_CalibrationData;
extern structSetpointData   g_CurrentRecipe;


//////////////////////////////////////////////////////
// ClearThroughputTotals( void )           from ASM = CLEARTHROUGHPUTTOTALS
//
// Resets all throughput totals
//
// P.Smith                          15-3-2005
// Reset of throughput totals
//
//////////////////////////////////////////////////////
void ClearThroughputTotals( void )
{
    g_fWeightPerMeter = 0.0;        // grams per meter                                                                     
    g_fWeightPerMeterRR = 0.0;       // round robin value
    g_fLbsPer1000Feet = 0.0;        // reset lbs/1000 feet              //ACTLBKF 

    g_fThroughputKgPerHour = 0.0;   // kg/hr reset to 0    
    g_fRRThroughputKgPerHour = 0.0;   // kg/hr reset to 0
    g_fThroughputLbPerHour = 0.0;   //TPTLBH
    g_fRRThroughputLbPerHour = 0.0; //reset imperial throughput         //RRTPTLBH

    CopyProdSummDataToMB();         // copy prod summ data to modbus    //asm = COPYMBPSUM
    CopyKghToMB();                  // copy kg/hr                       // asm= CPYKGHMB
    CopyGPMToModbus();              // copy grams per meter to modbus   //  ASM = CPYGPMMB 
}


