//////////////////////////////////////////////////////
// Blrep.c
//
// blender reports

// P.Smith                              1-7-2005
// First pass
//
// P.Smith                              12/7/2005
// front roll change is verified as operational.
//
// P.Smith                              13/07/2005
// correct slit weight calculation for front/ back reports.
// Order report first pass.

//
// P.Smith                              9/08/2005
// Copy of hourly report added
//
// P.Smith                              29/08/2005
// Added batch report
//
// P.Smith                              28/09/2005
// enable report printing
//
// P.Smith                              4/10/05
// Rename low priority todos
//
// P.Smith                              7/10/05
// implement CopyBatchData( void )
//
// P.Smith      = --> ==   if(g_CurrentRecipe.m_nRegrindComponent == i)
//
// P.Smith                              24/10/05
// Removed "testonly" code
//
// P.Smith                              22/11/05
// correct copy of m_fPercentageSetpoint to report area for order & roll.
// should be 2 decimal places instead of 1.
//
// P.Smith                              10/1/06
// correct wanrings.
//
// correct copy of g_Hour.m_cStartTime / m_cEndTime[i] to g_HourlyReport.m_cStartTime / m_cEndTime[i]
//
// P.Smith                      3/3/06
// first pass at netburner hardware conversion.
// included  <basictypes.h>
//   g_BackRollReport.m_bLiquidEnabled = g_CurrentRecipe.m_fPercentageLiquid; // liquid additive % to roll report
// TO g_BackRollReport.m_fPercentageAdditive= g_CurrentRecipe.m_fPercentageLiquid; // liquid additive % to roll report
//
// P.Smith                            25/5/06
// multiply modbus data by conversion factor.
//
// P.Smith                      31/5/06
// changed unions IntsAndLong to WordAndDWord
// correct copy of component order weight g_fComponentOrderWeightAccumulator to order
// report area
//  Correct copy of order component weights to order report fTemp = g_fComponentOrderWeightAccumulator[i];
// was originally g_fComponentFrontWeightAccumulator[i]
//
// P.Smith                      20/6/06
// if g_CalibrationData.m_bSendEmailOnOrderChange is set copy order report to email
// buffer CopyOrderReportToBuffer()
//
// P.Smith                      9/8/06
// added  addition of ongoing weight counters to GenerateTotalWeights
//
// P.Smith                            11/1/07
// Reset the back roll report when front roll change is activated and blender is in tube mode
//
// M.McKiernan                      17/9/07
// Removed g_arrnMBTable[] & g_arrnMBTableFlags[], now in BatVars2
//
// P.Smith                      9/1/08
// modify CopyOrderReportData not to try and compensate for the weight that is used in the batch.
// check for g_fComponentOrderWeightAccumulator [n] being negative, if it is then set it to 0
//
// P.Smith                      28/1/08
// put back in PreCompensateOrderWeight and PostCompensateOrderWeight for order report
//
// P.Smith                          30/1/08
// correct compiler warnings
//
// P.Smith                          25/4/08
// remove double conversion of order report data, the data is out by a factor of 2.
// the imperial conversion was taking place in the copy order report and also
// in the copy to modbus CopyOrderReportData
// add check for non zero to avoid divide by zero,g_fTotalFrontWeightAccumulator
// g_fTotalBackWeightAccumulator,g_CurrentRecipe.m_nBackSlitReels
//
// P.Smith                          24/6/08
// remove g_arrnMBTable ,g_CurrentTime
//
// P.Smith                          24/6/08
// copy order report to buffer CopyOrderReportToBuffer put back in
// to allow the order report to be generated.
//
// P.Smith                          11/12/08
// removed m_bRollReportPrintFlag, m_bOrderReportPrintFlag, m_bHourlyPrintFlag
//
// P.Smith                          2/4/09
// swap order number characters copied for the panel nbb--todo-- change panel for this
//
// P.Smith                          2/7/09
// correct double roll change when in tube mode
//
// P.Smith                          21/7/09
// use g_fComponentHourlyWeightAccumulator when generating hourly data.
// reset totals when the accumulated hourly data has been generated.
//
// P.Smith                          25/8/09
// reset offline batch weight accumulator when resetting totals.
// reset g_fTotalBatchWeightAccumulator and g_fTotalHistoryBatchWeightAccumulator
//////////////////////////////////////////////////////

//*****************************************************************************
// INCLUDES
//*****************************************************************************

#include <stdio.h>
#include "General.h"
#include "BatVars.h"
#include "GPMCalc.h"
#include "ConversionFactors.h"
#include "BatchCalibrationFunctions.h"
#include "SetpointFormat.h"
#include "BatchMBIndices.h"
#include "BatVars2.h"
#include "Acumwt.h"
#include "Mbprogs.h"
#include "Bbcalc.h"
#include "BatVars2.h"
#include "Alarms.h"
#include "BlRep.h"
#include "Lspeed.h"
#include <basictypes.h>
#include "AssReports.h"
#include "VNC1L_g.h"


structRollReport    g_FrontRollReport;
structRollReport    g_BackRollReport;
structRollData      g_FrontRoll;
structRollData      g_BackRoll;
structRollData      g_Order;
structHourData      g_Hour;
structBatchData     g_Batch;

structRollData      g_CurrentShiftFront;
structRollData      g_CurrentShiftBack;
structRollData      g_OngoingShift;
structOrderReport   g_OrderReport;
structHourlyReport  g_HourlyReport;
structBatchReport   g_BatchReport;



extern structRollComponentData m_ComponentData[MAX_COMPONENTS];

extern CalDataStruct    g_CalibrationData;
extern structSetpointData   g_CurrentRecipe;
extern  char        g_cPrinterBuffer[];
extern structUSB   g_sUSB;



//////////////////////////////////////////////////////
// CopyFrontRollChangeData()               ASM = FRELCHG
// Copy front roll data to roll report structure
//

// P.Smith                              9-3-2005
//////////////////////////////////////////////////////

void CopyFrontRollChangeData( void )
{

    int i;
    PreCompensateFrontRollWeight(); //    ASM = PRECOMPFROLL - subtract off unused batch weight from front roll weight

//  merge this inline -ASM = FRPER

    for(i = 0; i < MAX_COMPONENTS; i++)  // calculate component %.
    {
        if(g_fTotalFrontWeightAccumulator > 0.0001)
        {
            g_fComponentFrontRollPercentage[i] = (100 * g_fComponentFrontWeightAccumulator[i]) / g_fTotalFrontWeightAccumulator;
        }
    }

    g_FrontRollReport.m_nLayerCount = g_CalibrationData.m_nComponents; // no of components
    g_FrontRollReport.m_fWeightLengthSetpoint = g_CurrentRecipe.m_fTotalWeightLength; // weight / length set point

    g_FrontRollReport.m_fTotalWeightArea = g_CurrentRecipe.m_fTotalWeightArea; // grams per square meter

    g_FrontRollReport.m_fThroughputSetpoint = g_CurrentRecipe.m_fTotalThroughput;     // throughput set point

    g_FrontRollReport.m_fLineSpeedSetpoint = g_CurrentRecipe.m_fLineSpeed;     // line speed

    if(g_CurrentRecipe.m_bSheetTube) // sheet ? store slit weight.
    {

        g_FrontRollReport.m_fRollWeight = g_fTotalFrontWeightAccumulator / 2;  // if sheet divide by 2
    }
    else
    {
        g_FrontRollReport.m_fRollWeight = g_fTotalFrontWeightAccumulator;
    }

    g_FrontRollReport.m_fSlitRollWeight = g_FrontRollReport.m_fRollWeight / g_CurrentRecipe.m_nFrontSlitReels;  // front roll weight accumulator
    g_FrontRollReport.m_fOrderWeight = g_fTotalOrderWeightAccumulator;      // Order weight

    AccumulateFrontLength(); //   ASM = ACCLT_F

    g_FrontRollReport.m_fRollLength = g_FrontRoll.m_fLength;           // roll length
    g_FrontRoll.m_fLength = 0.0f;                                 // reset counter
    g_arrnMBTable[BATCH_SUMMARY_FRONT_ROLL_LENGTH] = 0;             // reset modbus roll length counter

    g_FrontRollReport.m_fOrderLength = g_Order.m_fLength;      // order length



    for(i = 0; i < CUSTOMERCODECHARACTERNO; i++)        // customer & material codes copied.
    {
        g_FrontRollReport.m_arrCustomerCode[i] = g_CurrentRecipe.m_arrCustomerCode[i];
    }
    for(i = 0; i < MATERIALCODECHARACTERNO; i++)                  // customer & material codes copied.
    {
        g_FrontRollReport.m_arrMaterialCode[i] = g_CurrentRecipe.m_arrMaterialCode[i];
    }


    for(i = 0; i < ORDERNOCHARACTERNO; i++)             // order no.
    {
        g_FrontRollReport.m_arrOrderNo[i] = g_CurrentRecipe.m_arrOrderNumber[i];
    }



    g_FrontRollReport.m_nFrontRollNo =  g_FrontRoll.m_nRollNo;
    g_FrontRollReport.m_nBackRollNo  =  g_BackRoll.m_nRollNo;

    g_FrontRollReport.m_bSheetTube = g_CurrentRecipe.m_bSheetTube;              // sheet / tube
    g_FrontRollReport.m_nFrontSlitReels = g_CurrentRecipe.m_nFrontSlitReels;     // front slits
    g_FrontRollReport.m_nBackSlitReels = g_CurrentRecipe.m_nBackSlitReels;      // back slits
    g_FrontRollReport.m_fWidthSetpoint = g_CurrentRecipe.m_fDesiredWidth;       // width sp
    g_FrontRollReport.m_fTrimmedWidth = g_CurrentRecipe.m_fTrimmedWidth;
    g_FrontRollReport.m_bTrimBeingFedBack = g_CurrentRecipe.m_bTrimFeed;        // trim fed back



    for(i = 0; i < TIME_ARRAY_SIZE; i++) // copy in start time                  // .
    {
        g_FrontRoll.m_cStartTime[i] = g_FrontRoll.m_cEndTime[i];       //

    }
    for(i = 0; i < TIME_ARRAY_SIZE; i++)                  // .
    {
        g_FrontRoll.m_cEndTime[i] = g_CurrentTime[i];
    }


    for(i = 0; i < TIME_ARRAY_SIZE; i++) // copy in start time              // .
    {
        g_FrontRollReport.m_cStartTime[i] = g_FrontRoll.m_cStartTime[i];
    }

    for(i = 0; i < TIME_ARRAY_SIZE; i++) // copy in end time              // .
    {
        g_FrontRollReport.m_cEndTime[i] = g_FrontRoll.m_cEndTime[i];
    }


    g_FrontRollReport.m_bLiquidEnabled = g_CalibrationData.m_bLiquidEnabled;      // back slits


    for(i = 0; i < MAX_COMPONENTS; i++)  //
    {
        g_FrontRollReport.m_ComponentData[i].m_fPercentageSetpoint = g_CurrentRecipe.m_fPercentage[i];
        if(g_CurrentRecipe.m_bSheetTube) // sheet ? / divide component weight by 2.
        {
            g_FrontRollReport.m_ComponentData[i].m_fRollWeight = (g_fComponentFrontWeightAccumulator[i] / 2);
        }
        else
        {
            g_FrontRollReport.m_ComponentData[i].m_fRollWeight = g_fComponentFrontWeightAccumulator[i];
        }
    }

    g_FrontRollReport.m_fPercentageAdditive = g_CurrentRecipe.m_fPercentageLiquid; // liquid additive % to roll report nbb--sbbswchange--

    for(i = 0; i < MAX_COMPONENTS; i++)  //
    {
        g_fComponentFrontWeightAccumulator[i] = 0.0f; // set front roll component weights to zero.
    }

// --todolp-- copy in liquid additive data to modbus --review-- if this this necessary.

    g_FrontRoll.m_nRollNo ++;       // increment roll no.
    g_CurrentShiftFront.m_nRollNo++;     // increment shift no



    g_fTotalFrontWeightAccumulator = 0.0f;

// merge STARTFRONTROLLREPORTPRINT in line
//    if(g_CalibrationData.m_bRollReportPrintFlag)
//    {
//        g_bPrintFrontRollReport = TRUE;     // enable report printing
//    }


    if(g_bResetOrderTotalsAtNextRollChange)
    {
        ResetOrderTotals(); // asm - ORDINIT
    }


//    if(!g_CurrentRecipe.m_bSheetTube)
//    {
//        g_fTotalBackWeightAccumulator = 0.0f;   // asm TOTWACCB
//
//        for(i = 0; i < MAX_COMPONENTS; i++)  // calculate component %.
//        {
//            g_fComponentBackWeightAccumulator[i] = 0.0f;   // asm CH1WACCB
//        }
//        g_BackRoll.m_fLength = 0.0f;
//        CopyBackRollReportToMB();     // asm  MBBRL this will copy the data to be modbus table
//    }

    CopyFrontRollReportToMB();     // ASM = CPYFRRMB  copy front roll data to modbus table.
    PostCompensateFrontRollWeight();                   // add on unused weights to component accumulators
    CopyWeightsToMB();      // ASM = CPYTWTMB copy weights to modbus

}




//////////////////////////////////////////////////////
// CopyBackRollChangeData()               ASM = BRELCHG
// Copy back roll data to roll report structure
//

// P.Smith                              9-3-2005
//////////////////////////////////////////////////////

void CopyBackRollChangeData( void )
{

    int i;
    PreCompensateBackRollWeight(); //    ASM = PRECOMPFROLL - subtract off unused batch weight from front roll weight

//  merge this inline -ASM = BRPER

    for(i = 0; i < MAX_COMPONENTS; i++)  // calculate component %.
    {
        if(g_fTotalBackWeightAccumulator > 0.0001)
        {
            g_fComponentBackRollPercentage[i] = (100 * g_fComponentBackWeightAccumulator[i]) / g_fTotalBackWeightAccumulator;
        }
    }

    g_BackRollReport.m_nLayerCount = g_CalibrationData.m_nComponents; // no of components
    g_BackRollReport.m_fWeightLengthSetpoint = g_CurrentRecipe.m_fTotalWeightLength; // weight / length set point

    g_BackRollReport.m_fTotalWeightArea = g_CurrentRecipe.m_fTotalWeightArea; // grams per square meter

    g_BackRollReport.m_fThroughputSetpoint = g_CurrentRecipe.m_fTotalThroughput;     // throughput set point

    g_BackRollReport.m_fLineSpeedSetpoint = g_CurrentRecipe.m_fLineSpeed;     // line speed

    if(g_CurrentRecipe.m_bSheetTube) // sheet ? store slit weight.
    {

        g_BackRollReport.m_fRollWeight = g_fTotalBackWeightAccumulator / 2;  // if sheet divide by 2
    }
    else
    {
        g_BackRollReport.m_fRollWeight = g_fTotalBackWeightAccumulator;
    }

    if(g_CurrentRecipe.m_nBackSlitReels > 0)
    {
        g_BackRollReport.m_fSlitRollWeight = g_BackRollReport.m_fRollWeight / g_CurrentRecipe.m_nBackSlitReels;  // front roll weight accumulator
    }
    g_BackRollReport.m_fOrderWeight = g_fTotalOrderWeightAccumulator;      // Order weight

    AccumulateFrontLength(); //   ASM = ACCLT_F
    AccumulateBackLength(); //   ASM = ACCLT_B

    g_BackRollReport.m_fRollLength = g_BackRoll.m_fLength;           // roll length
    g_BackRoll.m_fLength = 0.0f;                                 // reset counter
    g_arrnMBTable[BATCH_SUMMARY_BACK_ROLL_LENGTH] = 0;             // reset modbus roll length counter

    g_BackRollReport.m_fOrderLength = g_Order.m_fLength;      // order length



    for(i = 0; i < CUSTOMERCODECHARACTERNO; i++)        // customer & material codes copied.
    {
        g_BackRollReport.m_arrCustomerCode[i] = g_CurrentRecipe.m_arrCustomerCode[i];
    }
    for(i = 0; i < MATERIALCODECHARACTERNO; i++)                  // customer & material codes copied.
    {
        g_BackRollReport.m_arrMaterialCode[i] = g_CurrentRecipe.m_arrMaterialCode[i];
    }


    for(i = 0; i < ORDERNOCHARACTERNO; i++)             // order no.
    {
        g_BackRollReport.m_arrOrderNo[i] = g_CurrentRecipe.m_arrOrderNumber[i];
    }



    g_BackRollReport.m_nFrontRollNo =  g_FrontRoll.m_nRollNo;
    g_BackRollReport.m_nBackRollNo  =  g_BackRoll.m_nRollNo;

    g_BackRollReport.m_bSheetTube = g_CurrentRecipe.m_bSheetTube;              // sheet / tube
    g_BackRollReport.m_nBackSlitReels = g_CurrentRecipe.m_nFrontSlitReels;     // front slits
    g_BackRollReport.m_nBackSlitReels = g_CurrentRecipe.m_nBackSlitReels;      // back slits
    g_BackRollReport.m_fWidthSetpoint = g_CurrentRecipe.m_fDesiredWidth;       // width sp
    g_BackRollReport.m_fTrimmedWidth = g_CurrentRecipe.m_fTrimmedWidth;
    g_BackRollReport.m_bTrimBeingFedBack = g_CurrentRecipe.m_bTrimFeed;        // trim fed back



    for(i = 0; i < TIME_ARRAY_SIZE; i++) // copy in start time                  // .
    {
        g_BackRoll.m_cStartTime[i] = g_BackRoll.m_cEndTime[i];       //

    }
    for(i = 0; i < TIME_ARRAY_SIZE; i++)                  // .
    {
        g_BackRoll.m_cEndTime[i] = g_CurrentTime[i];
    }



    for(i = 0; i < TIME_ARRAY_SIZE; i++) // copy in start time              // .
    {
        g_BackRollReport.m_cStartTime[i] = g_BackRoll.m_cStartTime[i];
    }

    for(i = 0; i < TIME_ARRAY_SIZE; i++) // copy in end time              // .
    {
        g_BackRollReport.m_cEndTime[i] = g_BackRoll.m_cEndTime[i];
    }




    g_BackRollReport.m_bLiquidEnabled = g_CalibrationData.m_bLiquidEnabled;      // back slits


    for(i = 0; i < MAX_COMPONENTS; i++)  //
    {
        g_BackRollReport.m_ComponentData[i].m_fPercentageSetpoint = g_CurrentRecipe.m_fPercentage[i];
        if(g_CurrentRecipe.m_bSheetTube) // sheet ? / divide component weight by 2.
        {
            g_BackRollReport.m_ComponentData[i].m_fRollWeight = (g_fComponentBackWeightAccumulator[i] / 2);
        }
        else
        {
            g_BackRollReport.m_ComponentData[i].m_fRollWeight = g_fComponentBackWeightAccumulator[i];
        }
    }

    g_BackRollReport.m_fPercentageAdditive= g_CurrentRecipe.m_fPercentageLiquid; // liquid additive % to roll report nbb--sbbswchange--


    for(i = 0; i < MAX_COMPONENTS; i++)  //
    {
        g_fComponentBackWeightAccumulator[i] = 0.0f; // set front roll component weights to zero.
    }

// --todolp-- copy in liquid additive data to modbus --review-- if this this necessary.

    g_BackRoll.m_nRollNo ++;       // increment roll no.
    g_CurrentShiftBack.m_nRollNo++;     // increment shift no


    g_fTotalBackWeightAccumulator = 0.0f;

// merge STARTBACKROLLREPORTPRINT in line
//    if(g_CalibrationData.m_bRollReportPrintFlag)
//    {
//        g_bPrintBackRollReport = TRUE;
//    }



    if(g_bResetOrderTotalsAtNextRollChange)
    {
        ResetOrderTotals(); // asm - ORDINIT
    }


    CopyBackRollReportToMB();     // ASM = CPYBRRMB  copy back roll data to modbus table.
    PostCompensateBackRollWeight();                   // add on unused weights to component accumulators
    CopyWeightsToMB();      // ASM = CPYTWTMB copy weights to modbus

}






//////////////////////////////////////////////////////
//
// BatchWeightCompensation()            ASM = BATCHWTCOMP
// Calculated the unused batch weight per component
// This weight will have to be subtrated from the roll
// weight.
//
// P.Smith                              4-7-2005
//////////////////////////////////////////////////////

void BatchWeightCompensation( void )
{
    int i;

    g_fUsedBatchWeight = (g_nKgHrSecondsCounter * g_fThroughputKgPerHour) / 3600;  //  (grams in 1 hour / 3600 seconds) * Seconds so far in this batch
    g_fUnUsedBatchWeight = g_fPreviousBatchWeight - g_fUsedBatchWeight;

// merge inline calculate unused batch weight per component ASM = CALBTPER

    for(i = 0; i < MAX_COMPONENTS; i++)  //  .
    {
        g_fComponentActualUnusedWeight[i] = (g_CurrentRecipe.m_fPercentage[i]/100) * g_fUnUsedBatchWeight;
    }
}


//////////////////////////////////////////////////////
//
// PreCompensateFrontRollWeight()            ASM = PRECOMPFROLL
// subtracts compensated or unused weight from the front roll
// weight counter for each component. The component weights are
// later added up..
//
// P.Smith                              5-7-2005
//////////////////////////////////////////////////////

void PreCompensateFrontRollWeight( void )
{
    int i;
    BatchWeightCompensation();
    for(i = 0; i < MAX_COMPONENTS; i++)  // .
    {
        g_fComponentFrontWeightAccumulator[i] = g_fComponentFrontWeightAccumulator[i] - g_fComponentActualUnusedWeight[i];

    }
    GenerateTotalWeights();  //ASM = GENTOTALS
    GenerateOrderComponentPercentages();   //ASM = ORDERPER  --review-- does this need to be called here ?
}


//////////////////////////////////////////////////////
//
// PostCompensateFrontRollWeight()            ASM = POSTCOMPFROLL
// Adds compensated or unused weight to the front roll
// weight counter for each component.
//
// P.Smith                              5-7-2005
//////////////////////////////////////////////////////

void PostCompensateFrontRollWeight( void )
{
    int i;
    BatchWeightCompensation();
    for(i = 0; i < MAX_COMPONENTS; i++)  // .
    {
        g_fComponentFrontWeightAccumulator[i] = g_fComponentFrontWeightAccumulator[i] + g_fComponentActualUnusedWeight[i];
    }
    GenerateTotalWeights();  //ASM = GENTOTALS
    GenerateOrderComponentPercentages();   //ASM = ORDERPER  --review-- does this need to be called here ?

}


//////////////////////////////////////////////////////
//
// PreCompensateBackRollWeight()            ASM = PRECOMPBROLL
// subtracts compensated or unused weight from the back roll
// weight counter for each component. The component weights are
// later added up..
//
// P.Smith                              5-7-2005
//////////////////////////////////////////////////////

void PreCompensateBackRollWeight( void )
{
    int i;
    BatchWeightCompensation();
    for(i = 0; i < MAX_COMPONENTS; i++)  // .
    {
        g_fComponentBackWeightAccumulator[i] = g_fComponentBackWeightAccumulator[i] - g_fComponentActualUnusedWeight[i];
    }
    GenerateTotalWeights();  //ASM = GENTOTALS
    GenerateOrderComponentPercentages();   //ASM = ORDERPER  --review-- does this need to be called here ?
}

//////////////////////////////////////////////////////
//
// PostCompensateBackRollWeight()            ASM = POSTCOMPBROLL
// Adds compensated or unused weight to the back roll
// weight counter for each component.
//
// P.Smith                              5-7-2005
//////////////////////////////////////////////////////

void PostCompensateBackRollWeight( void )
{
    int i;
    BatchWeightCompensation();
    for(i = 0; i < MAX_COMPONENTS; i++)  //
    {
        g_fComponentBackWeightAccumulator[i] = g_fComponentBackWeightAccumulator[i] + g_fComponentActualUnusedWeight[i];
    }
    GenerateTotalWeights();  //ASM = GENTOTALS
    GenerateOrderComponentPercentages();   //ASM = ORDERPER  --review-- does this need to be called here ?

}

//////////////////////////////////////////////////////
//
// PreCompensateHourlyWeight()            ASM = PRECOMPHOUR
// subtracts compensated or unused weight from the hourly
// weight counter for each component. The component weights are
// later added up..
//
// P.Smith                              5-7-2005
//////////////////////////////////////////////////////

void PreCompensateHourlyWeight( void )
{
    int i;
    BatchWeightCompensation();
    for(i = 0; i < MAX_COMPONENTS; i++)  //
    {
        g_fComponentHourlyWeightAccumulator[i] = g_fComponentHourlyWeightAccumulator[i] - g_fComponentActualUnusedWeight[i];
    }
    GenerateTotalWeights();  //ASM = GENTOTALS
    GenerateOrderComponentPercentages();   //ASM = ORDERPER  --review-- does this need to be called here ?
}

//////////////////////////////////////////////////////
//
// PostCompensateHourlyWeight()            ASM = POSTCOMPHOUR
// Adds compensated or unused weight to the hourly
// weight counter for each component.
//
// P.Smith                              5-7-2005
//////////////////////////////////////////////////////

void PostCompensateHourlyWeight( void )
{
    int i;
    BatchWeightCompensation();
    for(i = 0; i < MAX_COMPONENTS; i++)  // .
    {
        g_fComponentHourlyWeightAccumulator[i] = g_fComponentHourlyWeightAccumulator[i] + g_fComponentActualUnusedWeight[i];
    }
    GenerateTotalWeights();  //ASM = GENTOTALS
    GenerateOrderComponentPercentages();   //ASM = ORDERPER  --review-- does this need to be called here ?

}


//////////////////////////////////////////////////////
//
// PreCompensateOrderWeight()            ASM = PRECOMPORDER
// subtracts compensated or unused weight from the order
// weight counter for each component. The component weights are
// later added up..
//
// P.Smith                              5-7-2005
//////////////////////////////////////////////////////

void PreCompensateOrderWeight( void )
{
    int i;
    BatchWeightCompensation();
    for(i = 0; i < MAX_COMPONENTS; i++)  //
    {
        g_fComponentOrderWeightAccumulator[i] = g_fComponentOrderWeightAccumulator[i] - g_fComponentActualUnusedWeight[i];
    }
    GenerateTotalWeights();  // ASM = GENTOTALS
    GenerateOrderComponentPercentages();   //ASM = ORDERPER  --review-- does this need to be called here ?
}

//////////////////////////////////////////////////////
//
// PostCompensateOrderWeight()            ASM = POSTCOMPORDER
// Adds compensated or unused weight to the hourly
// weight counter for each component.
//
// P.Smith                              5-7-2005
//////////////////////////////////////////////////////

void PostCompensateOrderWeight( void )
{
    int i;
    BatchWeightCompensation();
    for(i = 0; i < MAX_COMPONENTS; i++)  // copy component set %.
    {
        g_fComponentOrderWeightAccumulator[i] = g_fComponentOrderWeightAccumulator[i] + g_fComponentActualUnusedWeight[i];
        if( g_fComponentOrderWeightAccumulator[i] < 0.00)
        {
            g_fComponentOrderWeightAccumulator[i] = 0.00;
        }
    }
    GenerateTotalWeights();  // ASM = GENTOTALS
    GenerateOrderComponentPercentages();   //ASM = ORDERPER  --review-- does this need to be called here ?

}




//////////////////////////////////////////////////////
//
// PreCompensateShiftWeight()            ASM = PRECOMPSHIFT
// subtracts compensated or unused weight from the shift
// weight counter for each component. The component weights are
// later added up..
//
// P.Smith                              5-7-2005
//////////////////////////////////////////////////////

void PreCompensateShiftWeight( void )
{
    int i;
    BatchWeightCompensation();
    for(i = 0; i < MAX_COMPONENTS; i++)  //
    {
        g_fComponentShiftWeightAccumulator[i] = g_fComponentShiftWeightAccumulator[i] - g_fComponentActualUnusedWeight[i];
    }
    GenerateTotalWeights();  // ASM = GENTOTALS
    GenerateOrderComponentPercentages();   //ASM = ORDERPER  --review-- does this need to be called here ?
}

//////////////////////////////////////////////////////
//
// PostCompensateOrderWeight()            ASM = POSTCOMPSHIFT
// Adds compensated or unused weight to the shift
// weight counter for each component.
//
// P.Smith                              5-7-2005
//////////////////////////////////////////////////////

void PostCompensateShiftWeight( void )
{
    int i;
    BatchWeightCompensation();
    for(i = 0; i < MAX_COMPONENTS; i++)  // .
    {
        g_fComponentShiftWeightAccumulator[i] = g_fComponentShiftWeightAccumulator[i] + g_fComponentActualUnusedWeight[i];
    }
    GenerateTotalWeights(); // ASM = GENTOTALS
    GenerateOrderComponentPercentages();   //ASM = ORDERPER  --review-- does this need to be called here ?
}


//////////////////////////////////////////////////////
//
// GenerateTotalWeights()            ASM = GENTOTALS
// Add component weights to generate totals
//
// P.Smith                              5-7-2005
//////////////////////////////////////////////////////

void GenerateTotalWeights( void )
{
    float   fAddition;
    int i;

    fAddition = 0.0f;        // set counter to 0
    for(i = 0; i < MAX_COMPONENTS; i++)  // .
    {
        fAddition += g_fComponentOrderWeightAccumulator[i];
    }
    g_fTotalOrderWeightAccumulator = fAddition;   // TOTWACCO in ASM

// front roll weight

    fAddition = 0.0f;        // set counter to 0
    for(i = 0; i < MAX_COMPONENTS; i++)  // .
    {
        fAddition += g_fComponentFrontWeightAccumulator[i];
    }
    g_fTotalFrontWeightAccumulator = fAddition;    //TOTWACCF in ASM

// back roll weight

    fAddition = 0.0f;        // set counter to 0
    for(i = 0; i < MAX_COMPONENTS; i++)  // .
    {
        fAddition += g_fComponentBackWeightAccumulator[i];
    }
    g_fTotalBackWeightAccumulator = fAddition;    //TOTWACCB in ASM


// shift weight

    fAddition = 0.0f;        // set counter to 0
    for(i = 0; i < MAX_COMPONENTS; i++)  // .
    {
        fAddition += g_fComponentShiftWeightAccumulator[i];
    }
    g_fTotalShiftWeightAccumulator = fAddition;    // CURSHIFTWEIGHT in ASM

// shift weight

    fAddition = 0.0f;        // set counter to 0
    for(i = 0; i < MAX_COMPONENTS; i++)  // .
    {
        fAddition += g_fComponentShiftWeightOngoingAccumulator[i];
    }
    g_fTotalShiftWeightOngoingAccumulator = fAddition;    // TOTWACCS9 in ASM


// long term weight

    fAddition = 0.0f;        // set counter to 0
    for(i = 0; i < MAX_COMPONENTS; i++)  // .
    {
        fAddition += g_fComponentLongTermWeightAccumulator[i];
    }
    g_fTotalLongTermWeightAccumulator = fAddition;    // TOTWACCM in ASM


// Long Term Weight Ongoing (resettable)
    i = 0;
    fAddition = 0.0;
    for(i=0; i < MAX_COMPONENTS; i++)
    {
        fAddition += g_fComponentLongTermWeightResettableAccumulator[i];
    }
    g_fTotalLongTermWeightResettableAccumulator = fAddition;      //

// hourly weight

    fAddition = 0.0f;        // set counter to 0
    for(i = 0; i < MAX_COMPONENTS; i++)  // .
    {
        fAddition += g_fComponentHourlyWeightAccumulator[i];
    }
    g_fTotalHourlyWeightAccumulator = fAddition;    // TOTWACCH in ASM


// Batching weight for offline mode

    fAddition = 0.0f;        // set counter to 0
    for(i = 0; i < MAX_COMPONENTS; i++)  // .
    {
        fAddition += g_fComponentBatchWeightAccumulator[i];
    }
    g_fTotalBatchWeightAccumulator = fAddition;    // TOTWACCBAT in ASM

    g_fTotalHistoryBatchWeightAccumulator = g_fTotalBatchWeightAccumulator;  // ASM = TOTWACCBATHI
    g_fTotalBatchWeightAccumulatorImp = g_fTotalBatchWeightAccumulator * WEIGHT_CONVERSION_FACTOR; // ASM = CONVKGRHI
    g_fTotalHistoryBatchWeightAccumulatorImp = g_fTotalHistoryBatchWeightAccumulator;

}

//////////////////////////////////////////////////////
//
// GenerateOrderComponentPercentages()      ASM = ORDERPER
// Calculate order % from total weight and component weights
// weight counter for each component.
// if regrind is being used, subtract off the regrind weight if not
// the regrind component.
//
// P.Smith                              5-7-2005
//////////////////////////////////////////////////////

void GenerateOrderComponentPercentages( void )
{
    unsigned int i;
    if( g_bRegrindPresent )
    {
        for(i = 0; i < MAX_COMPONENTS; i++)  // .
        {
            if(g_CurrentRecipe.m_nRegrindComponent == i)
            {
                g_fComponentOrderPercentage[i] = (100 * g_fComponentOrderWeightAccumulator[i]) / g_fTotalOrderWeightAccumulator;
            }
            else  // not the regrind component - subtract off the regrind component.
            {
                 g_fComponentOrderPercentage[i] = (100 * g_fComponentOrderWeightAccumulator[i]) / (g_fTotalOrderWeightAccumulator - g_fComponentOrderWeightAccumulator[i]);
            }

        }

    }
    else // no regrind.
    {
        for(i = 0; i < MAX_COMPONENTS; i++)  // .
        {
            g_fComponentOrderPercentage[i] = (100 * g_fComponentOrderWeightAccumulator[i]) / g_fTotalOrderWeightAccumulator;

        }
    }

}


//////////////////////////////////////////////////////
//
// CopyFrontRollReportToMB()      ASM = CPYFRRMB
// Copy front roll report data to modbus table.
//
//
// P.Smith                              5-7-2005
//////////////////////////////////////////////////////

void CopyFrontRollReportToMB( void )
{
    float   fValue;
    int i;
    union WordAndDWord uValue,uTemp;
    union   CharsAndWord uValue1,uValue2;

// no of components

    g_arrnMBTable[FRREPT_COMPONENTS] = g_FrontRollReport.m_nLayerCount; // no of components

// weight/length
        fValue = g_FrontRollReport.m_fWeightLengthSetpoint;
        fValue *=    g_fWeightPerLengthConversionFactor;

        fValue += 0.005;                                                            // for rounding to nearest 2nd decimal digit.
        g_arrnMBTable[FRREPT_WEIGHT_LENGTH_SET] = (int)(fValue * 100);              // wt/m is to 2dp
//--review-- should check for 65535 be put in here as in ASM

//  copy in wt/m2 to modbus front roll report

        fValue = g_FrontRollReport.m_fTotalWeightArea;
        fValue *=    g_fWeightPerAreaConversionFactor;
        fValue += 0.005;                                                            // for rounding to nearest 2nd decimal digit.
        g_arrnMBTable[FRREPT_WEIGHT_AREA_SET] = (int)(fValue * 100);              // wt/m is to 2dp


// kg/h set point

        fValue = g_FrontRollReport.m_fThroughputSetpoint;
        fValue *=    g_fThroughputConversionFactor;
        fValue += 0.05;                                                             // for rounding to nearest  decimal digit.
        g_arrnMBTable[FRREPT_THROUGHPUT_SET] = (int)(fValue * 10);                  // Throughput is to 1dp

// Linespeed
        fValue =    g_FrontRollReport.m_fLineSpeedSetpoint;
        fValue *=    g_fLineSpeedConversionFactor;
        fValue += 0.05;                                                             // for rounding to nearest  decimal digit.
        g_arrnMBTable[FRREPT_LINE_SPEED_SET] = (int)(fValue * 10);                  // Linespeed is to 1dp



// Width
        fValue =    g_FrontRollReport.m_fWidthSetpoint;
        fValue *=    g_fWidthConversionFactor;
        fValue += 0.5;                                                              // for rounding to nearest  decimal digit.
        g_arrnMBTable[FRREPT_WIDTH_SET] = (int)(fValue);                            // Width is in mm.

// Trimmed Width
        fValue =    g_FrontRollReport.m_fTrimmedWidth;
        fValue *=    g_fWidthConversionFactor;
        fValue += 0.5;                                                              // for rounding to nearest  decimal digit.
        g_arrnMBTable[FRREPT_TRIMMED_WIDTH_SET] = (int)(fValue);                    // Width is in mm.


// Roll Weight (long) - 2dp

        fValue = g_FrontRollReport.m_fRollWeight;
        fValue *=    g_fWeightConversionFactor;
        fValue += 0.005;                                                            // for rounding to nearest 2 decimal places
        uValue.lValue = (long)(fValue * 100.0f);   // 2dp
        g_arrnMBTable[FRREPT_ROLL_WEIGHT] = uValue.nValue[0];                       // M.s.Byte. -
        g_arrnMBTable[FRREPT_ROLL_WEIGHT + 1] = uValue.nValue[1];                   // L.s.Byte.

// Slit Roll Weight (long) - 2p

        fValue = g_FrontRollReport.m_fSlitRollWeight;
        fValue *=    g_fWeightConversionFactor;
        fValue += 0.005;                                                            // for rounding to nearest 2 decimal places
        uValue.lValue = (long)(fValue * 100.0f);   // 2dp
        g_arrnMBTable[FRREPT_SLIT_WEIGHT] = uValue.nValue[0];                       // M.s.Byte. -
        g_arrnMBTable[FRREPT_SLIT_WEIGHT + 1] = uValue.nValue[1];                   // L.s.Byte.


// Order Weight (long) - 2dp

        fValue = g_FrontRollReport.m_fOrderWeight;
        fValue *=    g_fWeightConversionFactor;
        fValue += 0.005;                                                            // for rounding to nearest 2 decimal places
        uValue.lValue = (long)(fValue * 100.0f);   // 2dp
        g_arrnMBTable[FRREPT_ORDER_WEIGHT] = uValue.nValue[0];                       // M.s.Byte. -
        g_arrnMBTable[FRREPT_ORDER_WEIGHT + 1] = uValue.nValue[1];                   // L.s.Byte.


// Roll Length

        fValue  = g_FrontRollReport.m_fRollLength;
        fValue *=    g_fLengthConversionFactor;
        fValue += 0.5;                                                                  // for rounding to nearest meter
        g_arrnMBTable[FRREPT_ROLL_LENGTH] = (int)(fValue);                              // length (m)


// Order Length

        fValue  = g_FrontRollReport.m_fOrderLength;
        fValue *=    g_fLengthConversionFactor;
        fValue += 0.5;                                                                  // round to nearest meter.
        uValue.lValue = (long)(fValue);
        g_arrnMBTable[FRREPT_ORDER_LENGTH] = uValue.nValue[0];                          // M.s.Byte. -
        g_arrnMBTable[FRREPT_ORDER_LENGTH + 1] = uValue.nValue[1];                      // L.s.Byte.


// Customer code

    for(i = 0; i < (CUSTOMERCODECHARACTERNO/2); i++)                  // Customer Code
    {
        uValue1.cValue[0] = g_FrontRollReport.m_arrCustomerCode[(i*2)];                          // Modbus has two ascii chars in each register.
        uValue1.cValue[1] = g_FrontRollReport.m_arrCustomerCode[(i*2)+1];                       // Modbus has two ascii chars in each register.
        g_arrnMBTable[FRREPT_CUSTOMER_CODE + i] = uValue1.nValue;
    }




// Material code

     for(i = 0; i < (MATERIALCODECHARACTERNO/2); i++)                  // Customer Code
    {
        uValue1.cValue[0] = g_FrontRollReport.m_arrMaterialCode[(i*2)];                          // Modbus has two ascii chars in each register.
        uValue1.cValue[1] = g_FrontRollReport.m_arrMaterialCode[(i*2)+1];                       // Modbus has two ascii chars in each register.
        g_arrnMBTable[FRREPT_MATERIAL_CODE + i] = uValue1.nValue;
    }

// Order No.

     for(i = 0; i < (ORDERNOCHARACTERNO/2); i++)                  // Order No
    {
        uValue1.cValue[0] = g_FrontRollReport.m_arrOrderNo[(i*2)];
        uValue1.cValue[1] = g_FrontRollReport.m_arrOrderNo[(i*2)+1];
        g_arrnMBTable[FRREPT_ORDER_NUMBER + i] = uValue1.nValue;
    }


// Front Roll No.

        g_arrnMBTable[FRREPT_FRONT_ROLL_NO] = g_FrontRollReport.m_nFrontRollNo;

// Back Roll No.
        g_arrnMBTable[FRREPT_BACK_ROLL_NO] = g_FrontRollReport.m_nBackRollNo;

// Sheet / Tube
        g_arrnMBTable[FRREPT_SHEET_TUBE] = g_FrontRollReport.m_bSheetTube;

// No of slits (Back : Front)

        uValue1.nValue = g_CurrentRecipe.m_nBackSlitReels;
        uValue2.cValue[1] = uValue1.cValue[1];
        uValue1.nValue = g_CurrentRecipe.m_nFrontSlitReels;
        uValue2.cValue[0] = uValue1.cValue[1];
        g_arrnMBTable[FRREPT_SLIT_REELS] = uValue2.nValue;

// dry addititve unused


// Trim feed
         g_arrnMBTable[FRREPT_TRIM_FLAG] = g_FrontRollReport.m_bTrimBeingFedBack;

// roll start time  - hour /min

        uValue1.cValue[0] =  g_FrontRollReport.m_cStartTime[TIME_HOUR];
        uValue1.cValue[1] =  g_FrontRollReport.m_cStartTime[TIME_MINUTE];
        g_arrnMBTable[FRREPT_START_TIME] = uValue1.nValue;

// roll start date - month / date

        uValue1.cValue[0] =  g_FrontRollReport.m_cStartTime[TIME_MONTH];
        uValue1.cValue[1] =  g_FrontRollReport.m_cStartTime[TIME_DATE];
        g_arrnMBTable[FRREPT_START_DATE] = uValue1.nValue;


// roll end time  - hour /min

        uValue1.cValue[0] =  g_FrontRollReport.m_cEndTime[TIME_HOUR];
        uValue1.cValue[1] =  g_FrontRollReport.m_cEndTime[TIME_MINUTE];
        g_arrnMBTable[FRREPT_END_TIME] = uValue1.nValue;

// roll end date - month / date

        uValue1.cValue[0] =  g_FrontRollReport.m_cEndTime[TIME_MONTH];
        uValue1.cValue[1] =  g_FrontRollReport.m_cEndTime[TIME_DATE];
        g_arrnMBTable[FRREPT_END_DATE] = uValue1.nValue;


// liquid additive
        g_arrnMBTable[FRREPT_LA_ENABLED] = g_FrontRollReport.m_bLiquidEnabled;



// Target percentage  - 2dp
// Component weight   - 2dp

        for(i = 0; i < 10; i++)  // --review-- only 10 component used at the moment. .
        {
            fValue = g_FrontRollReport.m_ComponentData[i].m_fPercentageSetpoint;
            g_arrnMBTable[FRREPT_BLOCK_1 + (i*FRREPT_BLOCK_SIZE) + FRREPT_PERCENTAGE_OFFSET] = (int)(100 * fValue); // % to 1 dp


            fValue = g_FrontRollReport.m_ComponentData[i].m_fRollWeight;
            fValue *=    g_fWeightConversionFactor;
            uTemp.lValue = (long)(fValue * 100.0f);  // 2 dp

            // M.s.Byte.
            g_arrnMBTable[FRREPT_BLOCK_1 + (i*FRREPT_BLOCK_SIZE) + FRREPT_WEIGHT_OFFSET] = uTemp.nValue[0];
           // l.s.Byte.
            g_arrnMBTable[FRREPT_BLOCK_1 + (i*FRREPT_BLOCK_SIZE) + FRREPT_WEIGHT_OFFSET + 1] = uTemp.nValue[1];
        }


            g_arrnMBTable[BATCH_SUMMARY_NEW_SETPOINTS] = g_arrnMBTable[BATCH_SUMMARY_NEW_SETPOINTS] | BATCH_FRONT_ROLL_MASK; // indicate roll report available

// --todolp-- copy of pib component data to modbus --review-- is this actually required?
// merge STARTFRONTROLLREPORTPRINT in line
//    if(g_CalibrationData.m_bRollReportPrintFlag)
//    {
//        g_bPrintFrontRollReport = TRUE;
//    }


}



//////////////////////////////////////////////////////
//
// CopyBackRollReportToMB()      ASM = CPYBRRMB
// Copy back roll report data to modbus table.
//
//
// P.Smith                              12-7-2005
//////////////////////////////////////////////////////

void CopyBackRollReportToMB( void )
{
    float   fValue;
    int i;
    union WordAndDWord uValue,uTemp;
    union   CharsAndWord uValue1,uValue2;

// no of components

    g_arrnMBTable[BRREPT_COMPONENTS] = g_BackRollReport.m_nLayerCount; // no of components

// weight/length
        fValue = g_BackRollReport.m_fWeightLengthSetpoint;
        fValue *=    g_fWeightPerLengthConversionFactor;
        fValue += 0.005;                                                            // for rounding to nearest 2nd decimal digit.
        g_arrnMBTable[BRREPT_WEIGHT_LENGTH_SET] = (int)(fValue * 100);              // wt/m is to 2dp
//--review-- should check for 65535 be put in here as in ASM

//  copy in wt/m2 to modbus Back roll report

        fValue = g_BackRollReport.m_fTotalWeightArea;
        fValue *=    g_fWeightPerAreaConversionFactor;
        fValue += 0.005;                                                            // for rounding to nearest 2nd decimal digit.
        g_arrnMBTable[BRREPT_WEIGHT_AREA_SET] = (int)(fValue * 100);              // wt/m is to 2dp


// kg/h set point

        fValue = g_BackRollReport.m_fThroughputSetpoint;
        fValue *=    g_fWeightConversionFactor;
        fValue += 0.05;                                                             // for rounding to nearest  decimal digit.
        g_arrnMBTable[BRREPT_THROUGHPUT_SET] = (int)(fValue * 10);                  // Throughput is to 1dp

// Linespeed
        fValue =    g_BackRollReport.m_fLineSpeedSetpoint;
        fValue *=    g_fLineSpeedConversionFactor;
        fValue += 0.05;                                                             // for rounding to nearest  decimal digit.
        g_arrnMBTable[BRREPT_LINE_SPEED_SET] = (int)(fValue * 10);                  // Linespeed is to 1dp



// Width
        fValue =    g_BackRollReport.m_fWidthSetpoint;
        fValue *=    g_fWidthConversionFactor;
        fValue += 0.5;                                                              // for rounding to nearest  decimal digit.
        g_arrnMBTable[BRREPT_WIDTH_SET] = (int)(fValue);                            // Width is in mm.

// Trimmed Width
        fValue =    g_BackRollReport.m_fTrimmedWidth;
        fValue *=    g_fWidthConversionFactor;
        fValue += 0.5;                                                              // for rounding to nearest  decimal digit.
        g_arrnMBTable[BRREPT_TRIMMED_WIDTH_SET] = (int)(fValue);                    // Width is in mm.


// Roll Weight (long) - 2dp

        fValue = g_BackRollReport.m_fRollWeight;
        fValue *=    g_fWeightConversionFactor;
        fValue += 0.005;                                                            // for rounding to nearest 2 decimal places
        uValue.lValue = (long)(fValue * 100.0f);   // 2dp
        g_arrnMBTable[BRREPT_ROLL_WEIGHT] = uValue.nValue[0];                       // M.s.Byte. -
        g_arrnMBTable[BRREPT_ROLL_WEIGHT + 1] = uValue.nValue[1];                   // L.s.Byte.

// Slit Roll Weight (long) - 2p

        fValue = g_BackRollReport.m_fSlitRollWeight;
        fValue *=    g_fWeightConversionFactor;
        fValue += 0.005;                                                            // for rounding to nearest 2 decimal places
        uValue.lValue = (long)(fValue * 100.0f);   // 2dp
        g_arrnMBTable[BRREPT_SLIT_WEIGHT] = uValue.nValue[0];                       // M.s.Byte. -
        g_arrnMBTable[BRREPT_SLIT_WEIGHT + 1] = uValue.nValue[1];                   // L.s.Byte.


// Order Weight (long) - 2dp

        fValue = g_BackRollReport.m_fOrderWeight;
        fValue *=    g_fWeightConversionFactor;
        fValue += 0.005;                                                            // for rounding to nearest 2 decimal places
        uValue.lValue = (long)(fValue * 100.0f);   // 2dp
        g_arrnMBTable[BRREPT_ORDER_WEIGHT] = uValue.nValue[0];                       // M.s.Byte. -
        g_arrnMBTable[BRREPT_ORDER_WEIGHT + 1] = uValue.nValue[1];                   // L.s.Byte.


// Roll Length

        fValue  = g_BackRollReport.m_fRollLength;
        fValue *=    g_fLengthConversionFactor;
        fValue += 0.5;                                                                  // for rounding to nearest meter
        g_arrnMBTable[BRREPT_ROLL_LENGTH] = (int)(fValue);                              // length (m)


// Order Length

        fValue  = g_BackRollReport.m_fOrderLength;
        fValue *=    g_fLengthConversionFactor;
        fValue += 0.5;                                                                  // round to nearest meter.
        uValue.lValue = (long)(fValue);
        g_arrnMBTable[BRREPT_ORDER_LENGTH] = uValue.nValue[0];                          // M.s.Byte. -
        g_arrnMBTable[BRREPT_ORDER_LENGTH + 1] = uValue.nValue[1];                      // L.s.Byte.


// Customer code

    for(i = 0; i < (CUSTOMERCODECHARACTERNO/2); i++)                  // Customer Code
    {
        uValue1.cValue[0] = g_BackRollReport.m_arrCustomerCode[(i*2)];                          // Modbus has two ascii chars in each register.
        uValue1.cValue[1] = g_BackRollReport.m_arrCustomerCode[(i*2)+1];                       // Modbus has two ascii chars in each register.
        g_arrnMBTable[BRREPT_CUSTOMER_CODE + i] = uValue1.nValue;
    }




// Material code

     for(i = 0; i < (MATERIALCODECHARACTERNO/2); i++)                  // Customer Code
    {
        uValue1.cValue[0] = g_BackRollReport.m_arrMaterialCode[(i*2)];                          // Modbus has two ascii chars in each register.
        uValue1.cValue[1] = g_BackRollReport.m_arrMaterialCode[(i*2)+1];                       // Modbus has two ascii chars in each register.
        g_arrnMBTable[BRREPT_MATERIAL_CODE + i] = uValue1.nValue;
    }

// Order No.

     for(i = 0; i < (ORDERNOCHARACTERNO/2); i++)                  // Order No
    {
        uValue1.cValue[0] = g_BackRollReport.m_arrOrderNo[(i*2)];
        uValue1.cValue[1] = g_BackRollReport.m_arrOrderNo[(i*2)+1];
        g_arrnMBTable[BRREPT_ORDER_NUMBER + i] = uValue1.nValue;
    }


// Front Roll No.

        g_arrnMBTable[BRREPT_FRONT_ROLL_NO] = g_BackRollReport.m_nFrontRollNo;

// Back Roll No.
        g_arrnMBTable[BRREPT_BACK_ROLL_NO] = g_BackRollReport.m_nBackRollNo;

// Sheet / Tube
        g_arrnMBTable[BRREPT_SHEET_TUBE] = g_BackRollReport.m_bSheetTube;

// No of slits (Back : Front)

        uValue1.nValue = g_CurrentRecipe.m_nBackSlitReels;
        uValue2.cValue[1] = uValue1.cValue[1];
        uValue1.nValue = g_CurrentRecipe.m_nFrontSlitReels;
        uValue2.cValue[0] = uValue1.cValue[1];
        g_arrnMBTable[BRREPT_SLIT_REELS] = uValue2.nValue;

// dry addititve unused


// Trim feed
         g_arrnMBTable[BRREPT_TRIM_FLAG] = g_BackRollReport.m_bTrimBeingFedBack;

// roll start time  - hour /min

        uValue1.cValue[0] =  g_BackRollReport.m_cStartTime[TIME_HOUR];
        uValue1.cValue[1] =  g_BackRollReport.m_cStartTime[TIME_MINUTE];
        g_arrnMBTable[BRREPT_START_TIME] = uValue1.nValue;

// roll start date - month / date

        uValue1.cValue[0] =  g_BackRollReport.m_cStartTime[TIME_MONTH];
        uValue1.cValue[1] =  g_BackRollReport.m_cStartTime[TIME_DATE];
        g_arrnMBTable[BRREPT_START_DATE] = uValue1.nValue;


// roll end time  - hour /min

        uValue1.cValue[0] =  g_BackRollReport.m_cEndTime[TIME_HOUR];
        uValue1.cValue[1] =  g_BackRollReport.m_cEndTime[TIME_MINUTE];
        g_arrnMBTable[BRREPT_END_TIME] = uValue1.nValue;

// roll end date - month / date

        uValue1.cValue[0] =  g_BackRollReport.m_cEndTime[TIME_MONTH];
        uValue1.cValue[1] =  g_BackRollReport.m_cEndTime[TIME_DATE];
        g_arrnMBTable[BRREPT_END_DATE] = uValue1.nValue;


// liquid additive
        g_arrnMBTable[BRREPT_LA_ENABLED] = g_BackRollReport.m_bLiquidEnabled;



// Target percentage  - 2dp
// Component weight   - 2dp

        for(i = 0; i < 10; i++)  // --review-- only 10 component used at the moment. .
        {
            fValue = g_BackRollReport.m_ComponentData[i].m_fPercentageSetpoint;
            g_arrnMBTable[BRREPT_BLOCK_1 + (i*BRREPT_BLOCK_SIZE) + BRREPT_PERCENTAGE_OFFSET] = (int)(100 * fValue); // % to 1 dp


            fValue = g_BackRollReport.m_ComponentData[i].m_fRollWeight;
            fValue *=    g_fWeightConversionFactor;
            uTemp.lValue = (long)(fValue * 100.0f);  // 2 dp

            // M.s.Byte.
            g_arrnMBTable[BRREPT_BLOCK_1 + (i*BRREPT_BLOCK_SIZE) + BRREPT_WEIGHT_OFFSET] = uTemp.nValue[0];
           // l.s.Byte.
            g_arrnMBTable[BRREPT_BLOCK_1 + (i*BRREPT_BLOCK_SIZE) + BRREPT_WEIGHT_OFFSET + 1] = uTemp.nValue[1];
        }


            g_arrnMBTable[BATCH_SUMMARY_NEW_SETPOINTS] = g_arrnMBTable[BATCH_SUMMARY_NEW_SETPOINTS] | BATCH_BACK_ROLL_MASK; // indicate roll report available

// --todolp-- copy of pib component data to modbus --review-- is this actually required?

}





//////////////////////////////////////////////////////
// CopyOrderReportData()               ASM = ASSORPT  &  ASSEMBLEORDERDATA combined
// Copy front roll data to roll report structure
//

// P.Smith                              13-7-2005
//////////////////////////////////////////////////////

void CopyOrderReportData( void )
{
    int i;
    float fTemp;
    PreCompensateOrderWeight(); // pre compensate order weight.

    //  merge this inline -ASM = ORPER

    for(i = 0; i < MAX_COMPONENTS; i++)  // calculate component %.
    {
        g_fComponentOrderPercentage[i] = (100 * g_fComponentOrderWeightAccumulator[i]) / g_fTotalOrderWeightAccumulator;
    }

    g_OrderReport.m_nLayerCount = g_CalibrationData.m_nComponents; // no of components


    g_OrderReport.m_fWeightLengthSetpoint = g_CurrentRecipe.m_fTotalWeightLength;                                // weight / length set point

    g_OrderReport.m_fTotalWeightArea = g_CurrentRecipe.m_fTotalWeightArea;                                    // grams per square meter

    g_OrderReport.m_fThroughputSetpoint = g_CurrentRecipe.m_fTotalThroughput;     // throughput set point

    g_OrderReport.m_fLineSpeedSetpoint = g_CurrentRecipe.m_fLineSpeed;      // line speed

    g_OrderReport.m_fOrderWeight = g_fTotalOrderWeightAccumulator;           // Order weight ASM TOTWACCO

    g_OrderReport.m_fOrderLength = g_Order.m_fLength;


    for(i = 0; i < CUSTOMERCODECHARACTERNO; i++)        // customer & material codes copied.
    {
        g_OrderReport.m_arrCustomerCode[i] = g_CurrentRecipe.m_arrCustomerCode[i];
    }

    for(i = 0; i < MATERIALCODECHARACTERNO; i++)                  // customer & material codes copied.
    {
        g_OrderReport.m_arrMaterialCode[i] = g_CurrentRecipe.m_arrMaterialCode[i];
    }


    for(i = 0; i < ORDERNOCHARACTERNO; i++)             // order no.
    {
        g_OrderReport.m_arrOrderNo[i] = g_CurrentRecipe.m_arrOrderNumber[i];
    }



    g_OrderReport.m_nFrontRollNo =  g_FrontRoll.m_nRollNo;
    g_OrderReport.m_nBackRollNo  =  g_BackRoll.m_nRollNo;
    g_OrderReport.m_bSheetTube = g_CurrentRecipe.m_bSheetTube;              // sheet / tube
    g_OrderReport.m_nFrontSlitReels = g_CurrentRecipe.m_nFrontSlitReels;     // front slits
    g_OrderReport.m_nBackSlitReels = g_CurrentRecipe.m_nBackSlitReels;      // back slits

    g_OrderReport.m_fWidthSetpoint = g_CurrentRecipe.m_fDesiredWidth ;       // width sp

    g_OrderReport.m_fTrimmedWidth = g_CurrentRecipe.m_fTrimmedWidth;

    g_OrderReport.m_bTrimBeingFedBack = g_CurrentRecipe.m_bTrimFeed;        // trim fed back



    for(i = 0; i < TIME_ARRAY_SIZE; i++) // copy in start time              // .
    {
        g_Order.m_cStartTime[i] = g_Order.m_cEndTime[i];

    }
    for(i = 0; i < TIME_ARRAY_SIZE; i++)
    {
        g_Order.m_cEndTime[i] = g_CurrentTime[i];
    }

    for(i = 0; i < TIME_ARRAY_SIZE; i++) // copy in start time              // .
    {
        g_OrderReport.m_cStartTime[i] = g_Order.m_cStartTime[i];
    }


    for(i = 0; i < TIME_ARRAY_SIZE; i++) // copy in end time              // .
    {
        g_OrderReport.m_cEndTime[i] = g_Order.m_cEndTime[i];
    }

    g_OrderReport.m_bLiquidEnabled = g_CalibrationData.m_bLiquidEnabled;

    for(i = 0; i < MAX_COMPONENTS; i++)  //
    {
        g_OrderReport.m_ComponentData[i].m_fPercentageSetpoint = g_CurrentRecipe.m_fPercentage[i];

        fTemp = g_fComponentOrderWeightAccumulator[i];
        g_OrderReport.m_ComponentData[i].m_fWeight = fTemp;
    }
    g_OrderReport.m_bRegrindPresent = g_bRegrindPresent;


// merge STARTORDERREPORTPRINT in line
//    if(g_CalibrationData.m_bOrderReportPrintFlag)
//    {
//        g_bPrintOrderReport = TRUE;
 //   }

    CopyOrderReportToMB();  // ASM = CPYORRMB
    CopyOrderReportToBuffer();
    ResetOrderTotals();  // reset order totals
    PostCompensateOrderWeight(); // post compensate order weight.
    DumpOrderReportToUSB();
 }

//////////////////////////////////////////////////////
// DumpOrderReportToUSB()
// Dumps order report to USB
//////////////////////////////////////////////////////

void DumpOrderReportToUSB(void)
{
    if((g_CalibrationData.m_nDumpDeviceType == DUMP_DEVICE_USB) && g_CalibrationData.m_bOrderReportDumpFlag )  // use 1 for USB memory stick.      if( 1 )     // tthis works.
    {
//        if( fdTelnet > 0)
//        {
//        iprintf("\n order  report %s",g_OrderReportName);
//        }

        strncpy( g_sUSB.m_cOrderReportFileName, g_OrderReportName,USBFILENAMESTRINGLENGTH);     // save the filename for diagnostics
        g_sUSB.m_bStartOrderReportLogWrite = TRUE;
    }
    else if( (g_CalibrationData.m_nDumpDeviceType == DUMP_DEVICE_PRINTER) && g_CalibrationData.m_bOrderReportDumpFlag )  // use 0 for UsB printer.
    {
        StartUSBPrint( g_cOrderReportStorageBuffer, FALSE );
    }
}


/*


;  ASSORPT  ASSEMBLES THE ORDER REPORT DATA

ASSORPT:
        JSR     ASSEMBLEANDPRINTORDERDATA
        JSR     ORDINIT                 ; INITIALISE ORDER DATA
        JSR     POSTCOMPORDER
        LDAA    #1
        STAA    OCOFLG
        RTS

; ASSEMBLE AND PRINT ORDER REPORT

ASSEMBLEANDPRINTORDERDATA:
        JSR     ASSEMBLEORDERDATA
        JSR     STARTORDERREPORTPRINT
        JSR     CPYORRMB                ; COPY ORDER REPORT -> MB TABLE
        RTS



*/

//////////////////////////////////////////////////////
//
// CopyOrderReportToMB()      ASM = CPYORRMB
// Copy order report data to modbus table.
//
//
// P.Smith                              13-7-2005
//////////////////////////////////////////////////////

void CopyOrderReportToMB( void )
{
    float   fValue;
    int i;
    union WordAndDWord uValue,uTemp;
    union   CharsAndWord uValue1,uValue2;



    g_arrnMBTable[ORDREPT_COMPONENTS] = g_BackRollReport.m_nLayerCount; // no of components

// weight/length
        fValue = g_OrderReport.m_fWeightLengthSetpoint;
        fValue *=    g_fWeightPerLengthConversionFactor;
        fValue += 0.005;                                                            // for rounding to nearest 2nd decimal digit.
        g_arrnMBTable[ORDREPT_WEIGHT_LENGTH_SET] = (int)(fValue * 100);              // wt/m is to 2dp
//--review-- should check for 65535 be put in here as in ASM

//  copy in wt/m2 to modbus report

        fValue = g_OrderReport.m_fTotalWeightArea;
        fValue *=    g_fWeightPerAreaConversionFactor;
        fValue += 0.005;                                                            // for rounding to nearest 2nd decimal digit.
        g_arrnMBTable[ORDREPT_WEIGHT_AREA_SET] = (int)(fValue * 100);              // wt/m is to 2dp


// kg/h set point

        fValue = g_OrderReport.m_fThroughputSetpoint;
        fValue *=    g_fWeightConversionFactor;
        fValue += 0.05;                                                             // for rounding to nearest  decimal digit.
        g_arrnMBTable[ORDREPT_THROUGHPUT_SET] = (int)(fValue * 10);                  // Throughput is to 1dp

// Linespeed
        fValue =    g_OrderReport.m_fLineSpeedSetpoint;
        fValue *=    g_fLineSpeedConversionFactor;
        fValue += 0.05;                                                             // for rounding to nearest  decimal digit.
        g_arrnMBTable[ORDREPT_LINE_SPEED_SET] = (int)(fValue * 10);                  // Linespeed is to 1dp



// Width
        fValue =    g_OrderReport.m_fWidthSetpoint;
        fValue *=    g_fWidthConversionFactor;
        fValue += 0.5;                                                              // for rounding to nearest  decimal digit.
        g_arrnMBTable[ORDREPT_WIDTH_SET] = (int)(fValue);                            // Width is in mm.

// Trimmed Width
        fValue =    g_OrderReport.m_fTrimmedWidth;
        fValue *=    g_fWidthConversionFactor;
        fValue += 0.5;                                                              // for rounding to nearest  decimal digit.
        g_arrnMBTable[ORDREPT_TRIMMED_WIDTH_SET] = (int)(fValue);                    // Width is in mm.


// Order Weight (long) - 2dp

        fValue = g_OrderReport.m_fOrderWeight;
        fValue *=    g_fWeightConversionFactor;
        fValue += 0.005;                                                            // for rounding to nearest 2 decimal places
        uValue.lValue = (long)(fValue * 100.0f);   // 2dp
        g_arrnMBTable[ORDREPT_ORDER_WEIGHT] = uValue.nValue[0];                       // M.s.Byte. -
        g_arrnMBTable[ORDREPT_ORDER_WEIGHT + 1] = uValue.nValue[1];                   // L.s.Byte.




// Order Length

        fValue  = g_OrderReport.m_fOrderLength;
        fValue *=    g_fLengthConversionFactor;
        fValue += 0.5;                                                                  // round to nearest meter.
        uValue.lValue = (long)(fValue);
        g_arrnMBTable[ORDREPT_ORDER_LENGTH] = uValue.nValue[0];                          // M.s.Byte. -
        g_arrnMBTable[ORDREPT_ORDER_LENGTH + 1] = uValue.nValue[1];                      // L.s.Byte.



// Customer code

    for(i = 0; i < (CUSTOMERCODECHARACTERNO/2); i++)                  // Customer Code
    {
        uValue1.cValue[0] = g_OrderReport.m_arrCustomerCode[(i*2)];                          // Modbus has two ascii chars in each register.
        uValue1.cValue[1] = g_OrderReport.m_arrCustomerCode[(i*2)+1];                       // Modbus has two ascii chars in each register.
        g_arrnMBTable[ORDREPT_CUSTOMER_CODE + i] = uValue1.nValue;
    }




// Material code

     for(i = 0; i < (MATERIALCODECHARACTERNO/2); i++)                  // Customer Code
    {
        uValue1.cValue[0] = g_OrderReport.m_arrMaterialCode[(i*2)];                          // Modbus has two ascii chars in each register.
        uValue1.cValue[1] = g_OrderReport.m_arrMaterialCode[(i*2)+1];                       // Modbus has two ascii chars in each register.
        g_arrnMBTable[ORDREPT_MATERIAL_CODE + i] = uValue1.nValue;
    }

// Order No.

     for(i = 0; i < (ORDERNOCHARACTERNO/2); i++)                  // Order No
    {
        uValue1.cValue[1] = g_OrderReport.m_arrOrderNo[(i*2)];
        uValue1.cValue[0] = g_OrderReport.m_arrOrderNo[(i*2)+1];
        g_arrnMBTable[ORDREPT_ORDER_NUMBER + i] = uValue1.nValue;
    }



// Front Roll No.

        g_arrnMBTable[ORDREPT_FRONT_ROLL_NO] = g_OrderReport.m_nFrontRollNo;

// Back Roll No.
        g_arrnMBTable[ORDREPT_BACK_ROLL_NO] = g_OrderReport.m_nBackRollNo;

// Sheet / Tube
        g_arrnMBTable[ORDREPT_SHEET_TUBE] = g_OrderReport.m_bSheetTube;

// No of slits (Back : Front)

        uValue1.nValue = g_CurrentRecipe.m_nBackSlitReels;
        uValue2.cValue[1] = uValue1.cValue[1];
        uValue1.nValue = g_CurrentRecipe.m_nFrontSlitReels;
        uValue2.cValue[0] = uValue1.cValue[1];
        g_arrnMBTable[ORDREPT_SLIT_REELS] = uValue2.nValue;

// dry addititve unused


// Trim feed
         g_arrnMBTable[ORDREPT_TRIM_FLAG] = g_OrderReport.m_bTrimBeingFedBack;

// roll start time  - hour /min

        uValue1.cValue[0] =  g_OrderReport.m_cStartTime[TIME_HOUR];
        uValue1.cValue[1] =  g_OrderReport.m_cStartTime[TIME_MINUTE];
        g_arrnMBTable[ORDREPT_START_TIME] = uValue1.nValue;

// roll start date - month / date

        uValue1.cValue[0] =  g_OrderReport.m_cStartTime[TIME_MONTH];
        uValue1.cValue[1] =  g_OrderReport.m_cStartTime[TIME_DATE];
        g_arrnMBTable[ORDREPT_START_DATE] = uValue1.nValue;


// roll end time  - hour /min

        uValue1.cValue[0] =  g_OrderReport.m_cEndTime[TIME_HOUR];
        uValue1.cValue[1] =  g_OrderReport.m_cEndTime[TIME_MINUTE];
        g_arrnMBTable[ORDREPT_END_TIME] = uValue1.nValue;

// roll end date - month / date

        uValue1.cValue[0] =  g_OrderReport.m_cEndTime[TIME_MONTH];
        uValue1.cValue[1] =  g_OrderReport.m_cEndTime[TIME_DATE];
        g_arrnMBTable[ORDREPT_END_DATE] = uValue1.nValue;


// liquid additive
        g_arrnMBTable[ORDREPT_LA_ENABLED] = g_OrderReport.m_bLiquidEnabled;


// Target percentage  - 1dp
// Component weight   - 2dp

        for(i = 0; i < 10; i++)  // --review-- only 10 component used at the moment. .
        {
            fValue = g_OrderReport.m_ComponentData[i].m_fPercentageSetpoint;

            g_arrnMBTable[ORDREPT_BLOCK_1 + (i*ORDREPT_BLOCK_SIZE) + ORDREPT_PERCENTAGE_OFFSET] = (int)(100 * fValue); // % to 1 dp


            fValue = g_OrderReport.m_ComponentData[i].m_fWeight;
            fValue *=    g_fWeightConversionFactor;
            uTemp.lValue = (long)(fValue * 100.0f);  // 2 dp

            // M.s.Byte.
            g_arrnMBTable[ORDREPT_BLOCK_1 + (i*ORDREPT_BLOCK_SIZE) + ORDREPT_WEIGHT_OFFSET] = uTemp.nValue[0];

            // l.s.Byte.
            g_arrnMBTable[ORDREPT_BLOCK_1 + (i*ORDREPT_BLOCK_SIZE) + ORDREPT_WEIGHT_OFFSET + 1] = uTemp.nValue[1];
        }
        g_arrnMBTable[BATCH_SUMMARY_NEW_SETPOINTS] = g_arrnMBTable[BATCH_SUMMARY_NEW_SETPOINTS] | BATCH_ORDER_MASK; // indicate roll report available

}



//////////////////////////////////////////////////////
//
// ResetOrderTotals()      ASM = ORDINIT
// Reset order totals.
//
//
// P.Smith                              13-7-2005
//////////////////////////////////////////////////////

void ResetOrderTotals( void )
{
    int i;
    g_FrontRoll.m_nRollNo = 1;      // set roll no to `1
    g_BackRoll.m_nRollNo = 1;       // set roll no to `1

// reset front weight counters

    g_fTotalFrontWeightAccumulator = 0.0f;  // total  ASM TOTWACCF

    for(i = 0; i < MAX_COMPONENTS; i++)  //
    {
        g_fComponentFrontWeightAccumulator[i] = 0.0f; // set component weights to zero. ASM CH1WACCF
    }

// reset back weight counters

    g_fTotalBackWeightAccumulator = 0.0f;  // total

    for(i = 0; i < MAX_COMPONENTS; i++)  //
    {
        g_fComponentBackWeightAccumulator[i] = 0.0f; // set weights to zero.
    }

// reset front roll length counter

    g_FrontRoll.m_fLength = 0.0f;                            // reset counter
    g_arrnMBTable[BATCH_SUMMARY_FRONT_ROLL_LENGTH] = 0;     // reset modbus

// reset back roll length counter

    g_BackRoll.m_fLength = 0.0f;                            // reset counter
    g_arrnMBTable[BATCH_SUMMARY_BACK_ROLL_LENGTH] = 0;     // reset modbus


    for(i = 0; i < TIME_ARRAY_SIZE; i++) // copy in order to front/ back roll start times            // .
    {
        g_FrontRoll.m_cStartTime[i] =  g_OrderReport.m_cStartTime[i];
        g_BackRoll.m_cStartTime[i] =  g_OrderReport.m_cStartTime[i];
    }

// reset order totals.
    g_fTotalOrderWeightAccumulator = 0.0f;

    for(i = 0; i < MAX_COMPONENTS; i++)  //
    {
        g_fComponentOrderWeightAccumulator[i] = 0.0f; // set weights to zero.
    }
// --review-- need to clear liquid additive here.

// reset order lengths
    g_Order.m_fLength = 0.0f;


    g_arrnMBTable[BATCH_SUMMARY_ORDER_LENGTH] = 0;  // reset order length in modbus table --review-- if necessary
    g_arrnMBTable[BATCH_SUMMARY_ORDER_LENGTH+1] = 0;
    g_bResetOrderTotalsAtNextRollChange = FALSE;
    for(i=0; i < MAX_COMPONENTS; i++)
    {
        g_fComponentBatchWeightAccumulator[i] = 0.0f;
    }
    g_fTotalBatchWeightAccumulator = 0.0f;
    g_fTotalHistoryBatchWeightAccumulator = 0.0f;

}




//////////////////////////////////////////////////////
// CopyHourlyData()               ASM = GENHRREP
// Copy hourly data to hourly report structure.
//
//
// P.Smith                              4-8-2005
//////////////////////////////////////////////////////

void CopyHourlyData( void )
{
    int i;
    float fTemp;

    // merge calculation of hourly length in line // ASM = CALCHRLENGTH

    g_fTotalHourlyLengthAccumulator = g_lHourlyPulsesAccumulator / LINE_SPEED_CONVERSION_FACTOR; //
    g_lHourlyPulsesAccumulator = 0;

    PreCompensateHourlyWeight();    // Compensate weight

    //  merge hourly % calculation inline -ASM = HRPER

    for(i = 0; i < MAX_COMPONENTS; i++)  // calculate component %.
    {
        g_fComponentHourlyPercentage[i] = (100 * g_fComponentHourlyWeightAccumulator[i]) / g_fTotalHourlyWeightAccumulator;
    }

    // copy in start / end time.

    for(i = 0; i < TIME_ARRAY_SIZE; i++) // copy in start time              // .
    {
        g_Hour.m_cStartTime[i] = g_Hour.m_cEndTime[i];

    }
    for(i = 0; i < TIME_ARRAY_SIZE; i++)
    {
        g_Hour.m_cEndTime[i] = g_CurrentTime[i];
    }


    for(i = 0; i <         TIME_ARRAY_SIZE; i++) // copy in start time              // .
    {
        g_HourlyReport.m_cStartTime[i] = g_Hour.m_cStartTime[i];
    }

    for(i = 0; i < TIME_ARRAY_SIZE; i++) // copy in end time              // .
    {
        g_HourlyReport.m_cEndTime[i] = g_Hour.m_cEndTime[i];
    }

    // copy component data to hourly report


    for(i = 0; i < MAX_COMPONENTS; i++)  //
    {
         fTemp = g_fComponentHourlyWeightAccumulator[i];
         fTemp *=    g_fWeightConversionFactor;
         g_HourlyReport.m_ComponentData[i].m_fRollWeight = fTemp;
    }

    fTemp = g_fTotalHourlyWeightAccumulator;
    fTemp *=    g_fWeightConversionFactor;
    g_HourlyReport.m_fWeight = fTemp;

    fTemp = g_fTotalHourlyLengthAccumulator;
    fTemp *=    g_fLengthConversionFactor;
    g_HourlyReport.m_fLength = fTemp;


    for(i = 0; i < MAX_COMPONENTS; i++)  //
    {
        g_fComponentHourlyWeightAccumulator[i] = 0.0f; // hourly weight set to 0
    }

    g_fTotalHourlyWeightAccumulator = 0.0f;
    PostCompensateHourlyWeight();  // Compensate weight, calculate what is left for the next hour.

//  enable printing of hourly report.
//    if(g_CalibrationData.m_bHourlyPrintFlag)
//    {
//        g_bPrintHourlyReport = TRUE;
//    }
}


//////////////////////////////////////////////////////
// CopyBatchData()               ASM = GENBATREP
// Copy batch data to batch report structure.
//
//
// P.Smith                              7/10/05
//////////////////////////////////////////////////////

void CopyBatchData( void )
{

    int i;
    float fTemp;
    for(i = 0; i < MAX_COMPONENTS; i++)  // calculate component %.
    {
        g_fComponentBatchWeightAccumulatorPercentage[i] = (100 * g_fComponentBatchWeightAccumulator[i]) / g_fTotalBatchWeightAccumulator;
    }


    for(i = 0; i < TIME_ARRAY_SIZE; i++) /* copy in start time                  */
    {
        g_Batch.m_cStartTime[i] = g_Batch.m_cEndTime[i];

    }
    for(i = 0; i < TIME_ARRAY_SIZE; i++)
    {
        g_Batch.m_cEndTime[i] = g_CurrentTime[i];
    }

    for(i = 0; i < TIME_ARRAY_SIZE; i++) // copy in start time               .
    {
        g_BatchReport.m_cStartTime[i] = g_Batch.m_cStartTime[i];
    }

    for(i = 0; i < TIME_ARRAY_SIZE; i++) // copy in end time               .
    {
        g_BatchReport.m_cEndTime[i] = g_Batch.m_cEndTime[i];
    }


    fTemp = g_fTotalBatchWeightAccumulator;
    fTemp *=    g_fWeightConversionFactor;
    g_BatchReport.m_fWeight = fTemp;

    for(i = 0; i < MAX_COMPONENTS; i++)
    {
        g_BatchReport.m_ComponentData[i].m_fPercentage = g_fComponentBatchWeightAccumulatorPercentage[i];

        fTemp = g_fComponentBatchWeightAccumulator[i];
        fTemp *=    g_fWeightConversionFactor;
        g_BatchReport.m_ComponentData[i].m_fWeight = fTemp;
    }
}



/*

;  FRELCHG IS THE PROGRAM THAT HANDLES A FRONT REEL CHANGE
FRELCHG:
        JSR     PRECOMPFROLL
        JSR     FRPER              ; CALC PER.
        LDX      #FRRNBDS          ;~ROLL REPORT , NO. OF BLENDS AREA
        STX      MVMPTR
        LDAA     NOBLNDS           ; NO. OF BLENDS
        JSR      MOVAMRY
;MOVE WT/M or g/m2 SETPOINT TO  ROLL REPORT
        LDX      #WTPMSP           ;~WT/M SETPOINT
;        LDAA     SPTMODE
;        CMPA     #3             ; g/m2 MODE??
;        BNE      TPTSPT2
;        LDX      #GPM2SPT
TPTSPT2 LDAB     #3
        JSR      MOVMRY            ;~TRANSFER SETPOINT
;MOVE TOTAL KG/H SETPOINT TO REPORT
        LDX      #TKGHSP           ;~TOTAL KG/H SP
        LDAB     #2
        JSR      MOVMRY
;TRANSFER LINE SPEED SETPT
        LDX      #LSPDSP
        LDAB     #2
        JSR      MOVMRY
;MOVE ROLL WEIGHT TO ROLL REPORT
        LDX      #TOTWACCF       ;~FRONT ROLL WT ACCUMULATOR
        JSR      DIVWTBY2          ; WEIGHT / 2
        LDX      #EREG+1
        LDAB     #3
        JSR      MOVMRY            ;~MOVE WT  TO REPORT
        LDX      #FRRRWTVST       ; STORAGE AREA.
        JSR      IFEREG           ;

;MOVE SLIT ROLL WT TO REPORT
;        LDX      #FSWTACC+1        ;~FRONT SLIT WT ACCUMULATOR
        JSR     CALCFSLIT            ; CALCULATE SLIT WEIGHT
        LDAB     #3
        JSR      MOVMRY            ;~MOVE SLIT WT TO REPORT

        LDX      #FRWTACC
        JSR      CLRNM1            ;~CLR ROLL WT ACCUMULATOR

        CLR      ACWTCNT           ;~COUNTER

;MOVE ORDER WT TO REPORT  (ABCDEF Kg)
;        LDX      #OWTACC           ;~ORDER WT ACCUMULATOR
;        LDX      #TOTWACCO
        LDX      #TOTWACCO+1
        LDAB     #3
        JSR      MOVMRY            ;~MOVE ORDER WT TO REPORT
;MOVE LENGTH TO ROLL REPORT
        JSR      ACCLT_F           ;~ACCUMULATE LENGTH (FRONT)

        LDX      #FRRRLTV          ;~ROLL LENGTH VARIABLE IN ROLL RPT
        STX      MVMPTR

        LDX      #FRLTACC+1
        LDAB     #3
        JSR      MOVMRY            ;~NOW ORDER LENGTH IN ROLL RPT

        LDX      #FRLTACC
        JSR      CLRNM1            ;~CLR ROLL LENGTH ACCUMULATOR
        CLRW     MBFRL             ; CLEAR LENGTH IN MODBUS TABLE

;MOVE ORDER LENGTH TO ROLL REPORT
        JSR      MOV0MRY           ;~1ST BYTE = 00
        LDX      #OLTACC
        LDAB     #4
        JSR      MOVMRY            ;~NOW ORDER LENGTH IN ROLL RPT
;MOVE CUSTOMER CODE, MATERIAL CODE & ORDER NO. TO REPORT
        LDX     #CUSCODE           ;~CUSTOMER CODE
        LDAB    #9                 ;~3 BYTES EACH
        JSR     MOVMRY
;MOVE FRONT & BACK ROLL NO.S INTO REPORT
        LDX     #FROLLNO
        LDAB    #2
        JSR     MOVMRY
        LDX     #BROLLNO
        LDAB    #2
        JSR     MOVMRY

        LDAA    SHTTUBE            ;~SHEET/TUBE FLAG
        JSR     MOVAMRY
        LDAA    FSLITRS            ;~NO. OF FRONT SLIT REELS
        JSR     MOVAMRY
        LDAA    BSLITRS            ;~NO. OF BACK SLIT REELS
        JSR     MOVAMRY
        LDX     #PCT_ADD           ;~PERCENT ADDITIVE
        LDAB    #2
        JSR     MOVMRY             ;~2 BYTES, % ADDIT.
        LDX     #WIDTHSP            ;~WIDTH SETPOINT
        LDAB    #2
        JSR     MOVMRY

        LDX     #TRIMWID           ;~TRIMMED WIDTH
        LDAB    #2
        JSR     MOVMRY
        LDAA    TRIMFED            ;~TRIM FEEDBACK
        JSR     MOVAMRY

        LDX     #FROLSTM           ;~ROLL START TIME HRS/MINS/DATE/MON
        LDAB    #4
        JSR     MOVMRY

        LDX     #TIMEBUF
        LDAA    HOUR,X
        STAA    FROLSTM
        LDAA    MIN,X
        STAA    FROLSTM+1
        LDAA    DATE,X
        STAA    FROLSTM+2          ; STORE CURRENT TIME, HRS/MIN/DATE/MONTH
        LDAA    MON,X
        STAA    FROLSTM+3


        LDX     #FROLSTM           ;~FINISHED ROLL END TIME
        LDAB    #4
        JSR     MOVMRY

        LDAA    AVECPIB            ; PIB FLAG
        JSR     MOVAMRY
        JSR     MOV0MRY
        JSR     MOV0MRY             ; 2 SPARE BYTES

        LDAB    #1
FRELCH_B STAB  MVCHNO          ; COMP #1
        LDX     #PCNT1          ; COMPONENT 1 SETPT % (ABC.D%)
        LDAA    #2
        JSR     MVCHOFST        ; ADD IN REQUIRED OFFSET
        JSR     MOV2MRY         ; 2 BYTES

        LDX     #CH1WACCF        ; CHANNEL 1 FRONT WT. ACCUM. (ABCD.EFGHIJ Kg)
        LDAA    #5
        JSR     MVCHOFST        ; ADD IN REQUIRED OFFSET
        PSHM    X
;        TST      SHTTUBE
;        BEQ      ISTUBFCWT         ; TUBE
        JSR      DIVWTBY2          ; WEIGHT / 2
ISTUBFCWT:
        JSR     MOV5MRY
        PULM    X
        CLRW    0,X
        CLRW     2,X             ; CLEAR ACCUMULATORS
        CLR      4,X
;        LDX     #CH1STYP        ; SCREW TYP
;        LDAB    MVCHNO
;        DECB
;        ABX
;        LDAA    0,X             ; PICK UP FILL STATUS
;        JSR     MOVAMRY

;        JSR     MOV0MRY        ; MOVE 2 BLANKs TO BUFFER (SPARE)
        JSR     MOV0MRY

        LDAB    MVCHNO
        INCB                    ; REPEAT FOR ALL BLENDS
        CMPB    NOBLNDS
        LBLS    FRELCH_B       ;

; Liquid Additive
FRELCH_L LDAB   AVECPIB
        BEQ     FRELCH_X       ; DONT SEND LIQ ADD. BLOCK IF AVECPIB=0

        LDX     #PCNTLIQ        ; LIQ. ADD. % SETPT (ABC.D%)
        JSR     MOV2MRY         ; 2 BYTES

        LDX     #LIQWACCF        ; LIQ. WT ACCUM. (ABCD.EF Kg)
        JSR     MOV3MRY          ;
        CLRW    LIQWACCF
        CLRW    LIQWACCF+2       ; CLEAR ACCUMULATOR
        CLR     LIQWACCF+4

        JSR     MOV0MRY         ; MOVE 3 0'S TO FINISH BLOCK
        JSR     MOV0MRY         ;
        JSR     MOV0MRY         ;
FRELCH_X:


; INCREMENT THE ROLL NO. (NOTE ROLL NO. IN BCD)
        LDAA    FROLLNO+1  ;~INCREMENT THE ROLL NO
        ADDA    #1
        DAA
        STAA    FROLLNO+1
        LDAA    FROLLNO
        ADCA    #0
        DAA
        STAA     FROLLNO

        LDAA    CURSHIFTFROLLS+1  ;~INCREMENT THE ROLL NO
        ADDA    #1
        DAA
        STAA    CURSHIFTFROLLS+1
        LDAA    CURSHIFTFROLLS
        ADCA    #0
        DAA
        STAA     CURSHIFTFROLLS


        LDX     #TOTWACCF               ; TOTAL ROLL WT. ACC
;        LDAB    #5*10                   ; 8 BLENDS + PIB + TOTAL
        LDAA    #FIVE
        LDAB    #MAXCOMPONENTNO+2
        MUL
        TBA
        JSR     CLRBBS                  ; CLEAR

        LDAA    #$0F
        STAA    FRCOFLG                 ;~SET REEL CHANGE OCCURRED FLAG
        JSR     STARTFRONTROLLREPORTPRINT    ; START ROLL REPORT.

        TST     RSTOFLAG
        BEQ     FRELCH_Z
        JSR     ORDINIT
FRELCH_Z:
        JSR     CPYFRRMB                ; COPY FRONT ROLL REPORT -> MODBUS
        JSR     POSTCOMPFROLL
        JSR     CPYTWTMB                ; UPDATE PRODUCTION SUMMARY DATA
        RTS

STARTFRONTROLLREPORTPRINT:
        TST     RRPENAB                 ; ROLL REPORT PRINTING ENABLED?
        BEQ     FRELCH_Y
        LDAB    TOPRNFLG
        ORAB    #$02                    ; SET BIT #1 FOR F.R. REPORT PRINT
        STAB    TOPRNFLG
FRELCH_Y:
        RTS


;  BRELCHG IS THE PROGRAM THAT HANDLES A BACK REEL CHANGE
;MOVE WT/M SETPOINT TO  ROLL REPORT
BRELCHG:
        JSR      PRECOMPBROLL
        JSR      BRPER             ; % CALC FOR BACK ROLL.
        LDX      #BRRNBDS          ;~ROLL REPORT , NO. OF BLENDS AREA
        STX      MVMPTR
        LDAA     NOBLNDS           ; NO. OF BLENDS
        JSR      MOVAMRY
;MOVE WT/M SETPOINT TO  ROLL REPORT
        LDX      #WTPMSP           ;~WT/M SETPOINT
;        LDAA     SPTMODE
;        CMPA     #3             ; g/m2 MODE??
;        BNE      TPTSPT3
;        LDX      #GPM2SPT
TPTSPT3 LDAB     #3
        JSR      MOVMRY            ;~TRANSFER SETPOINT
;MOVE TOTAL KG/H SETPOINT TO REPORT
        LDX      #TKGHSP           ;~TOTAL KG/H SP
        LDAB     #2
        JSR      MOVMRY
;TRANSFER LINE SPEED SETPT
        LDX      #LSPDSP
        LDAB     #2
        JSR      MOVMRY
;MOVE ROLL WEIGHT TO ROLL REPORT
;        LDX      #BRWTACC+1        ;~BACK ROLL WT ACCUMULATOR
;        LDAB     #3
;        JSR      MOVMRY            ;~MOVE WT (2D.P.) TO REPORT
;MOVE ROLL WEIGHT TO ROLL REPORT
        LDX      #TOTWACCB         ;~BACK ROLL WT ACCUMULATOR
        JSR      DIVWTBY2          ; WEIGHT / 2
        LDX      #EREG+1
        LDAB     #3
        JSR      MOVMRY            ;~MOVE WT  TO REPORT
        LDX      #BRRRWTVST       ; STORAGE AREA.
        JSR      IFEREG           ;


;MOVE SLIT ROLL WT TO REPORT
        JSR      CALCBSLIT          ; BACK SLIT CALC
        LDAB     #3
        JSR      MOVMRY            ;~MOVE WT (2D.P.) TO REPORT

        LDX      #BRWTACC
        JSR      CLRNM1            ;~CLR ROLL WT ACCUMULATOR

;MOVE ORDER WT TO REPORT
;        LDX      #OWTACC           ;~ORDER WT ACCUMULATOR
        LDX      #TOTWACCO+1
;;P        LDX      #TOTWACCO
        LDAB     #3
        JSR      MOVMRY            ;~MOVE ORDER WT TO REPORT
;MOVE LENGTH TO ROLL REPORT
;        JSR      ACCLT_F           ;~UPDATE FRONT ROLL & ORDER LENGTHS

        JSR      ACCLT_B           ;~ACCUMULATE LENGTH (BACK)
        JSR      ACCLT_F           ;~ACCUMULATE FRONT LENGTH ALSO FOR ORD. LEN.
        LDX      #BRRRLTV          ;~ROLL LENGTH VARIABLE IN ROLL RPT
        STX      MVMPTR
        LDX      #BRLTACC+1
        LDAB     #3
        JSR      MOVMRY            ;~NOW ORDER LENGTH IN ROLL RPT

        LDX      #BRLTACC
        JSR      CLRNM1            ;~CLR ROLL LENGTH ACCUMULATOR
        CLRW     MBBRL             ; CLEAR LENGTH IN MODBUS TABLE

;MOVE ORDER LENGTH TO ROLL REPORT
        JSR      MOV0MRY           ;~1ST BYTE = 00
        LDX      #OLTACC
        LDAB     #4
        JSR      MOVMRY            ;~NOW ORDER LENGTH IN ROLL RPT
;MOVE CUSTOMER CODE, ORDER CODE & ORDER NO. TO REPORT
        LDX     #CUSCODE           ;~CUSTOMER CODE
        LDAB    #9                 ;~3 BYTES EACH
        JSR     MOVMRY
;MOVE FRONT & BACK ROLL NO.S INTO REPORT
        LDX     #FROLLNO
        LDAB    #2
        JSR     MOVMRY
        LDX     #BROLLNO
        LDAB    #2
        JSR     MOVMRY

        LDAA    SHTTUBE            ;~SHEET/TUBE FLAG
        JSR     MOVAMRY
        LDAA    FSLITRS            ;~NO. OF FRONT SLIT REELS
        JSR     MOVAMRY
        LDAA    BSLITRS            ;~NO. OF BACK SLIT REELS
        JSR     MOVAMRY
        LDX     #PCT_ADD           ;~PERCENT ADDITIVE
        LDAB    #2
        JSR     MOVMRY             ;~2 BYTES, % ADDIT.
        LDX     #WIDTHSP            ;~WIDTH SETPOINT
        LDAB    #2
        JSR     MOVMRY

        LDX     #TRIMWID           ;~TRIMMED WIDTH
        LDAB    #2
        JSR     MOVMRY
        LDAA    TRIMFED            ;~TRIM FEEDBACK
        JSR     MOVAMRY

        LDX     #BROLSTM           ;~ROLL START TIME HRS/MINS/DATE/MON
        LDAB    #4
        JSR     MOVMRY

        LDX     #TIMEBUF
        LDAA    HOUR,X
        STAA    BROLSTM
        LDAA    MIN,X
        STAA    BROLSTM+1
        LDAA    DATE,X
        STAA    BROLSTM+2          ; STORE CURRENT TIME, HRS/MIN/DATE/MONTH
        LDAA    MON,X
        STAA    BROLSTM+3



        LDX     #BROLSTM           ;~FINISHED ROLL END TIME
        LDAB    #4
        JSR     MOVMRY

        LDAA    AVECPIB            ; PIB FLAG
        JSR     MOVAMRY
        JSR     MOV0MRY
        JSR     MOV0MRY             ; 2 SPARE BYTES

        LDAB    #1
BRELCH_B STAB  MVCHNO          ; COMP #1
        LDX     #PCNT1          ; COMPONENT 1 SETPT % (ABC.D%)
        LDAA    #2
        JSR     MVCHOFST        ; ADD IN REQUIRED OFFSET
        JSR     MOV2MRY         ; 2 BYTES

        LDX     #CH1WACCB        ; CHANNEL 1 BACK WT. ACCUM. (ABCD.EFGHIJ Kg)
        LDAA    #5
        JSR     MVCHOFST        ; ADD IN REQUIRED OFFSET
        PSHM    X

;        TST      SHTTUBE
;        BEQ      ISTUBBCWT         ; TUBE
        JSR      DIVWTBY2          ; WEIGHT / 2
ISTUBBCWT:
        JSR     MOV5MRY

        PULM    X
        CLRW    0,X
        CLRW    2,X             ; CLEAR ACCUMULATORS
        CLR     4,X


;        LDX     #CH1STYP        ; SCREW TYP
;        LDAB    MVCHNO
;        DECB
;        ABX
;        LDAA    0,X             ; PICK UP FILL STATUS
;        JSR     MOVAMRY

;        JSR     MOV0MRY        ; MOVE 2 BLANKs TO BUFFER (SPARE)
        JSR     MOV0MRY

        LDAB    MVCHNO
        INCB                    ; REPEAT FOR ALL BLENDS
        CMPB    NOBLNDS
        LBLS    BRELCH_B       ;

; Liquid Additive
BRELCH_L LDAB   AVECPIB
        BEQ     BRELCH_X       ; DONT SEND LIQ ADD. BLOCK IF AVECPIB=0

        LDX     #PCNTLIQ        ; LIQ. ADD. % SETPT (ABC.D%)
        JSR     MOV2MRY         ; 2 BYTES

        LDX     #LIQWACCB         ; LIQ. WT ACCUM. (ABCD.EF Kg)
        JSR     MOV3MRY          ;
        CLRW    LIQWACCB
        CLRW    LIQWACCB+2       ; CLEAR WT. ACCUMULATOR
        CLR     LIQWACCB+4

        JSR     MOV0MRY         ; MOVE 3 0'S TO FINISH BLOCK
        JSR     MOV0MRY         ;
        JSR     MOV0MRY         ;
BRELCH_X:


; INCREMENT THE ROLL NO. (NOTE ROLL NO. IN BCD)
        LDAA    BROLLNO+1  ;~INCREMENT THE ROLL NO
        ADDA    #1
        DAA
        STAA    BROLLNO+1
        LDAA    BROLLNO
        ADCA    #0
        DAA
        STAA     BROLLNO

        LDAA    CURSHIFTBROLLS+1  ;~INCREMENT THE ROLL NO
        ADDA    #1
        DAA
        STAA    CURSHIFTBROLLS+1
        LDAA    CURSHIFTBROLLS
        ADCA    #0
        DAA
        STAA    CURSHIFTBROLLS




        LDX     #TOTWACCB
;        LDAB    #5*10                   ; 8 BLENDS + PIB + TOTAL

        LDAA    #FIVE
        LDAB    #MAXCOMPONENTNO+2
        MUL
        TBA
        JSR     CLRBBS                  ; CLEAR

        LDAA    #$0F
        STAA    BRCOFLG                 ;~SET REEL CHANGE OCCURRED FLAG

;        LDD     #$0001
;        STD     MBBRCOFLG               ; SET FLAG IN MODBUS TABLE ALSO
        JSR     STARTBACKROLLREPORTPRINT        ; PRINTING STARTED.
        TST     RSTOFLAG
        BEQ     BRELCH_Z
        JSR     ORDINIT
BRELCH_Z:
        JSR     CPYBRRMB                ; COPY BACK ROLL REPORT -> MODBUS
        JSR     POSTCOMPBROLL
        JSR     CPYTWTMB                ; UPDATE PRODUCTION SUMMARY DATA

;        LDD     MBNEWDATA
;        ORD     #$0200          ; SET B9 OF MBNEWDATA TO SHOW BACK ROLL CHANGE
;        STD     MBNEWDATA
        RTS


; INITIATE BACK ROLL REPORT PRINTING.

STARTBACKROLLREPORTPRINT:

        TST     RRPENAB                 ; ROLL REPORT PRINTING ENABLED?
        BEQ     BRELCH_Y
        LDAB    TOPRNFLG
        ORAB    #$04                    ; SET BIT #2 FOR B.R. REPORT PRINT
        STAB    TOPRNFLG
BRELCH_Y:
        RTS


;  ASSORPT  ASSEMBLES THE ORDER REPORT DATA

ASSORPT:
        JSR     ASSEMBLEANDPRINTORDERDATA
        JSR     ORDINIT                 ; INITIALISE ORDER DATA
        JSR     POSTCOMPORDER
        LDAA    #1
        STAA    OCOFLG
        RTS


; ASSEMBLE AND PRINT ORDER REPORT

ASSEMBLEANDPRINTORDERDATA:
        JSR     ASSEMBLEORDERDATA
        JSR     STARTORDERREPORTPRINT
        JSR     CPYORRMB                ; COPY ORDER REPORT -> MB TABLE
        RTS




ASSEMBLEORDERDATA:
        JSR     PRECOMPORDER
        JSR     ORPER              ; ORDER % CALCULATION.
        LDX      #ORRNBDS          ;~ORDER REPORT , WT/M SETPT AREA
        STX      MVBPTR
;MOVE WT/M SETPOINT TO  ORDER REPORT
        LDAA     NOBLNDS
        JSR      MOVABUF           ; NO OF BLENDS

        LDX      #WTPMSP+1         ;~WT/M SETPOINT
        LDAB     #3
        JSR      MOVBUF            ;~TRANSFER SETPOINT
;MOVE TOTAL KG/H SETPOINT TO REPORT
        LDX      #TKGHSP           ;~TOTAL KG/H SP
        LDAB     #2
        JSR      MOVBUF
;TRANSFER LINE SPEED SETPT
        LDX      #LSPDSP
        LDAB     #2
        JSR      MOVBUF

;MOVE ORDER WT TO REPORT
;        LDAB    #1
;        JSR     MOV0BUF        ; ;~1ST BYTE = 00
        LDX      #TOTWACCO         ;~ORDER WT ACCUMULATOR
        LDAB     #3
        JSR      MOVBUF            ;~MOVE ORDER WT TO REPORT
;;P
        LDX     TOTWACCO
        STX     ORRTOTW
        LDD     TOTWACCO+2
        STD     ORRTOTW+2
        LDAA    TOTWACCO+4
        STAA    ORRTOTW+4



;MOVE ORDER LENGTH TO ORDER REPORT
        LDX      #OLTACC
        LDAB     #3
        JSR      MOVBUF            ;~NOW ORDER LENGTH IN ORDER RPT
;MOVE CUSTOMER CODE, ORDER CODE & ORDER NO. TO REPORT
        LDX     #CUSCODE           ;~CUSTOMER CODE
        LDAB    #9                 ;~3 BYTES EACH
        JSR     MOVBUF
;MOVE FRONT & BACK ROLL NO.S INTO REPORT
        LDX     #FROLLNO
        LDAB    #2
        JSR     MOVBUF
        LDX     #BROLLNO
        LDAB    #2
        JSR     MOVBUF

        LDAA    SHTTUBE            ;~SHEET/TUBE FLAG
        JSR     MOVABUF
        LDAA    FSLITRS            ;~NO. OF FRONT SLIT REELS
        JSR     MOVABUF
        LDAA    BSLITRS            ;~NO. OF BACK SLIT REELS
        JSR     MOVABUF
        LDX     #PCT_ADD           ;~PERCENT ADDITIVE
        LDAB    #2
        JSR     MOVBUF             ;~2 BYTES, % ADDIT.
        LDX     #WIDTHSP            ;~WIDTH SETPOINT
        LDAB    #2
        JSR     MOVBUF

        LDX     #TRIMWID           ;~TRIMMED WIDTH
        LDAB    #2
        JSR     MOVBUF
        LDAA    TRIMFED            ;~TRIM FEEDBACK
        JSR     MOVABUF

        LDX     #ORDRSTM           ;~ORDER START TIME HRS/MINS/DATE/MON
        LDAB    #4
        JSR     MOVBUF

        LDX     #TIMEBUF
        LDAA    HOUR,X
        STAA    ORDRSTM
        LDAA    MIN,X
        STAA    ORDRSTM+1
        LDAA    DATE,X
        STAA    ORDRSTM+2          ; STORE CURRENT TIME, HRS/MIN/DATE/MONTH
        LDAA    MON,X
        STAA    ORDRSTM+3


        LDX     #ORDRSTM           ;~FINISHED ORDER END TIME
        LDAB    #4
        JSR     MOVBUF

        LDAA    AVECPIB             ; PIB FLAG
        JSR     MOVABUF
        LDAB    #2
        JSR     MOV0BUF        ; MOVE 2 BLANKs TO BUFFER (SPARE)

        LDAB    #1
ASORCH_B STAB  MVCHNO          ; COMP #1
        LDX     #PCNT1          ; COMPONENT 1 SETPT % (ABC.D%)
        LDAA    #2
        JSR     MVCHOFST        ; ADD IN REQUIRED OFFSET
        JSR     MOV2BF         ; 2 BYTES

        LDX     #CH1WACCO        ; CHANNEL 1 WT. ACCUM. (ABCD.EF Kg)
        LDAA    #5
        JSR     MVCHOFST        ; ADD IN REQUIRED OFFSET
        PSHM    X
        JSR     MOV5BF
        PULM    X
;        CLRW    0,X
;        CLRW    2,X             ; CLEAR ACCUMULATORS
;        CLR     4,X
        LDAB    #1
        JSR     MOV0BUF        ; MOVE 2 BLANKs TO BUFFER (SPARE)
        LDAB    #1
        JSR     MOV0BUF        ; MOVE 2 BLANKs TO BUFFER (SPARE)


        JSR     MOV0MRY



        LDAB    #2
        JSR     MOV0BUF        ; MOVE 2 BLANKs TO BUFFER (SPARE)

        LDAB    MVCHNO
        INCB                    ; REPEAT FOR ALL BLENDS
        CMPB    NOBLNDS
        LBLS    ASORCH_B       ;

        LDAA    REG_PRESENT     ; CURRENT REGRIND SET UP
        STAA    ORRREG          ; STORE

; Liquid Additive
ASORCH_L LDAB   AVECPIB
        BEQ     ASORCH_X       ; DONT SEND LIQ ADD. BLOCK IF AVECPIB=0

        LDX     #PCNTLIQ        ; LIQ. ADD. % SETPT (ABC.D%)
        JSR     MOV2BF         ; 2 BYTES

        LDX     #LIQWACC         ; LIQ. WT ACCUM. (ABCD.EF Kg)
        JSR     MOV3BF          ;
;        CLRW    LIQWACC
;        CLRW    LIQWACC+2       ; CLEAR WT. ACCUMULATOR
;        CLR     LIQWACC+4

        LDX     #LA_K
        JSR     MOV3BF          ; K VALUE (KG/H) FOR LIQ. ADD.

        LDAB    #3
        JSR     MOV0BUF         ; MOVE 3 0'S TO FINISH BLOCK

ASORCH_X:

        RTS


STARTORDERREPORTPRINT:
        TST     ORPENAB                 ; ORDER REPORT PRINTING ENABLED?
        BEQ     ASORCH_Y
        LDAB    TOPRNFLG
        ORAB    #$01                    ; SET BIT #0 FOR ORD REPORT PRINT
        STAB    TOPRNFLG
ASORCH_Y:
        RTS


;NOW INITIALISE FOR NEXT ORDER
ORDINIT LDD     #1
        STD     FROLLNO
        STD     BROLLNO                 ;~SET FRONT & BACK ROLL NO.S = 1
        LDX     #TOTWACCF               ; TOTAL ROLL WT. ACC
        LDAA    #FIVE
        LDAB    #MAXCOMPONENTNO+2
        MUL
        TBA
        JSR     CLRBBS                  ; CLEAR
        LDX     #TOTWACCB
        LDAA    #FIVE
        LDAB    #MAXCOMPONENTNO+2
        MUL
        TBA
        JSR     CLRBBS                  ; CLEAR
        LDX     #FRLTACC           ;~FRONT ROLL LENGTH
        JSR      CL5BYTS
        CLRW     MBFRL             ; CLEAR LENGTH IN MODBUS TABLE

        LDX     #BRLTACC           ;~BACK ROLL LENGTH
        JSR      CL5BYTS
        CLRW     MBBRL             ; CLEAR LENGTH IN MODBUS TABLE

        LDX     ORDRSTM            ;~STORE FRONT & BACK ROLL START TIMES
        STX     FROLSTM
        STX     BROLSTM
        LDX     ORDRSTM+2
        STX     FROLSTM+2
        STX     BROLSTM+2


        LDX      #TOTWACCO         ;~ORDER WT
        LDAA    #FIVE
        LDAB    #MAXCOMPONENTNO+2
        MUL
        TBA
        JSR      CLRBBS

        LDX      #OLTACC           ;~ORDER LENGTH
        JSR      CL5BYTS
        CLRW     MBORDL            ; CLEAR LENGTH IN MODBUS TABLE
        CLRW     MBORDL+2
        CLR      RSTOFLAG          ; RESET ORDER (AT NEXT RC) FLAG
        RTS


ASSSHIFT:
        JSR     SHPER           ; CALCULATE SHIFT %
        LDX      #SHNUM            ;~ORDER REPORT , WT/M SETPT AREA
        STX      MVBPTR
;MOVE WT/M SETPOINT TO  ORDER REPORT
        LDAA     SHIFTNO
        JSR      MOVABUF           ; SHIFT NO

        LDAB    SHIFTNO
        LDX     #SHIFT1STTIME       ; START TIME DATE.
        DECB
        ASLB
        ASLB
        ABX
        LDAB     #4
        JSR      MOVBUF            ;

        LDAB    SHIFTNO
        LDX     #SHIFT1ENDTIME      ; END TIME DATE.
        DECB
        ASLB
        ASLB
        ABX
        LDAB     #4
        JSR      MOVBUF            ;

        LDAB    SHIFTNO
        LDX     #SHIFT1LENGTH       ; SHIFT LENGTH.
        DECB
        ABX
        ABX
        ABX
        ABX
        ABX
        LDAB     #5
        JSR      MOVBUF

        LDAB    SHIFTNO
        LDX     #SHIFT1FROLLS      ; FRONT ROLLS.
        DECB
        ASLB
        ABX
        LDAB    #2
        JSR     MOVBUF

        LDAB    SHIFTNO
        LDX     #SHIFT1BROLLS      ; BACK ROLLS.
        DECB
        ASLB
        ABX
        LDAB    #2
        JSR     MOVBUF


        LDX     #CURSHIFTWEIGHT
        LDAB    #5
        JSR     MOVBUF

;       STORE COMPONENT DATA.
        LDAB    #1
NXTSCMP PSHB
        LDX     #PCNT1
        DECB
        ASLB
        ABX
        LDAB    #2
        JSR     MOVBUF
        PULB
        PSHB
        DECB
        LDAA    #5
        MUL
        LDX     #CH1WACCS
        ABX
        LDAB    #5
        JSR     MOVBUF
        PULB
        INCB
        CMPB    NOBLNDS
        BLS     NXTSCMP         ; NEXT SHIFT COMP
        LDX     #CH1WACCS
;        LDAB    #5           ;SHIFT WEIGHT ACCUMULATION
        LDAB    #MAXCOMPONENTNO
        LDAA    #SHBLKSIZ
        MUL
        JSR     CLRBBS                  ; CLEAR
        RTS


                   ;



; CL5BYTS  IS A SUBR TO CLEAR 5 BYTES FROM X
CL5BYTS CLRW   0,X
;        CLR   1,X
        CLRW   2,X
;        CLR   3,X
        CLR   4,X
        RTS

DIVWTBY2:
        PSHM    D
        JSR     ITCREG             ; ROLL WEIGHT
        JSR     CLAREG
        TST     SHTTUBE
        BEQ     ISTUB
        LDAA    #2
        BRA     STA
ISTUB   LDAA    #1
STA     STAA    AREG+4         ;
        JSR     DIV                ; WEIGHT / 2
        LDX     #EREG              ; POSITION OF RESULT.
        PULM    D
        RTS

; CPYFRRMB - ROUTINE TO COPY FRONT ROLL REPORT INTO MODBUS TABLE
CPYFRRMB:
        TST      PROTOCOL
        LBPL     CPYFRR_X            ; ONLY COPY IF IN MODBUS PROTOCOL

        LDAB     FRRNBDS           ; NO. OF BLENDS
        CLRA
        STD      MBFRRNBDS

;        LDX      #FRRWPMS           ;~WT/M SETPOINT
        LDX      #WTPMSP+1             ; LOAD THE G/M SP
        JSR      BCDHEX3X           ; CONVERT 3 BYTES AT X TO HEX
        TSTB                        ; CHECK FOR > 65535
        BEQ     NOOVF               ; OVER FLOW
        LDE     #MAXMODBUS
NOOVF   STE      MBFRRWPMS

        LDX      #FRRGPAS           ;~WT/M2 SETPOINT (g/AREA)
        JSR      BCDHEX3X           ; CONVERT 3 BYTES AT X TO HEX
        STE      MBFRRGPAS

;MOVE TOTAL KG/H SETPOINT TO REPORT
        LDX      #FRRKGHS           ;~TOTAL KG/H SP TO 2 DP
        LDAB     0,X
        LDE      1,X
        LSRB
        RORE
        LSRB
        RORE
        LSRB                        ; SHIFT LOT 4 BITS RIGHT (DIV BY 10)
        RORE                        ; AB CDEF -> 0A BCDE
        LSRB
        RORE

        JSR      BCDHEX3            ; CONVERT 3 BYTES  TO HEX
        STE      MBFRRKGHS          ; MAX EXPECTED = 6553.5 (I.E. 1 DP)
;TRANSFER LINE SPEED SETPT
        LDD      FRRLSPS
        ASLD
        ASLD                         ; ASSUMING MAX VALUE OF 999.
        ASLD
        ASLD                         ; NOW HAVE 10 TIMES
        JSR      BCDHEX2
        STD      MBFRRLSPS           ; VALUE IN MB IS TO 1 DP

        LDD     FRRWIDS            ;~WIDTH SETPOINT
        JSR     BCDHEX2
        STD     MBFRRWIDS

        LDD     FRRTWID            ;~TRIMMED WIDTH
        JSR     BCDHEX2
        STD     MBFRRTWID


;MOVE ROLL WEIGHT TO ROLL REPORT
        LDX      #FRRRWTVST+1      ;~FRONT ROLL WT
        JSR      BCDHEX3X          ; CONVERT TO HEX
        CLRA
        STD      MBFRRWTV
        STE      MBFRRWTV+2
;MOVE SLIT ROLL WT TO REPORT
        LDX      #FRRSRWV          ;~FRONT SLIT ROLL WT
        JSR      BCDHEX3X          ; CONVERT TO HEX
        CLRA
        STD      MBFRRSWV
        STE      MBFRRSWV+2

;MOVE ORDER WT TO REPORT  (ABCDEF Kg)
        LDX      #FRROWTV          ;~ORDER WT
        JSR      BCDHEX3X          ; CONVERT TO HEX
        CLRA
        STD      MBFRROWV
        STE      MBFRROWV+2
;MOVE LENGTH TO ROLL REPORT
        LDD      FRRRLTV           ;~FRONT ROLL LENGTH (m 2DP)
        JSR      BCDHEX2           ; CONVERT (m) TO HEX
        STD      MBFRRLTV          ; EXPECTING MAX LENGTH OF 65,535m ON ROLL

;MOVE ORDER LENGTH TO ROLL REPORT
        LDX      #FRROLTV          ;~ORDER LENGTH
        LDAB     1,X
        LDE      2,X
        JSR      BCDHEX3          ; CONVERT TO HEX
        CLRA
        STD      MBFRROLV
        STE      MBFRROLV+2
;MOVE CUSTOMER CODE, MATERIAL CODE & ORDER NO. TO REPORT
        LDX     #FRRCCOD
        LDAA    0,X
        JSR     C2ASCII         ; CONVERT 2 DIGITS TO 2 ASCII CHAR.S
        STD     MBFRRCCOD
        LDAA    1,X
        JSR     C2ASCII         ; CONVERT 2 DIGITS TO 2 ASCII CHAR.S
        STD     MBFRRCCOD+2
        LDAA    2,X
        JSR     C2ASCII         ; CONVERT 2 DIGITS TO 2 ASCII CHAR.S
        STD     MBFRRCCOD+4

        LDX     #FRRMCOD        ; MATERIAL CODE
        LDAA    0,X
        JSR     C2ASCII         ; CONVERT 2 DIGITS TO 2 ASCII CHAR.S
        STD     MBFRRMCOD
        LDAA    1,X
        JSR     C2ASCII         ; CONVERT 2 DIGITS TO 2 ASCII CHAR.S
        STD     MBFRRMCOD+2
        LDAA    2,X
        JSR     C2ASCII         ; CONVERT 2 DIGITS TO 2 ASCII CHAR.S
        STD     MBFRRMCOD+4

        LDX     #FRRONUM        ; MATERIAL CODE
        LDAA    0,X
        JSR     C2ASCII         ; CONVERT 2 DIGITS TO 2 ASCII CHAR.S
        STD     MBFRRONUM
        LDAA    1,X
        JSR     C2ASCII         ; CONVERT 2 DIGITS TO 2 ASCII CHAR.S
        STD     MBFRRONUM+2
        LDAA    2,X
        JSR     C2ASCII         ; CONVERT 2 DIGITS TO 2 ASCII CHAR.S
        STD     MBFRRONUM+4

;MOVE FRONT & BACK  ROLL NO.S INTO REPORT
        LDD     FRRFRNO         ; FRONT ROLL NO
        JSR     BCDHEX2
        STD     MBFRRFRNO

        LDD     FRRBRNO         ; BACK ROLL NO
        JSR     BCDHEX2
        STD     MBFRRBRNO

        LDAB    FRRSTUB            ;~SHEET/TUBE FLAG
        CLRA
        STD     MBFRRSTUB

        LDAB    FRRNBSR         ; NO OF BACK SLIT REELS
        JSR     BCDHEX1
        TBA
        LDAB    FRRNFSR            ;~NO. OF FRONT SLIT REELS
        JSR     BCDHEX1

        STD     MBFRRNSLR       ; [BACK SLITS:FRONT SLITS]

        CLRA
        LDAB    FRRTFED         ; TRIM FED BACK FLAG
        STD     MBFRRTFED
        LDAB    FRRAPIB
        STD     MBFRRAPIB       ; PIB ASSOCIATED FLAG

        LDD     FRRPADD
        JSR     BCDHEX2
        STD     MBFRRPADD

;roll start time
        LDAB    FRRSTIM         ;~ROLL START TIME HRS/MINS/DATE/MON
        JSR     BCDHEX1         ; CONVERT HOURS TO HEX
        TBA
        LDAB    FRRSTIM+1
        JSR     BCDHEX1
        STD     MBFRRSTIM       ; MODBUS FRR START [HOURS:MINS]

        LDAB    FRRSDAT+1       ;~ROLL START TIME DATE/MON (LOAD MONTH)
        JSR     BCDHEX1         ; CONVERT MONTH TO HEX
        TBA
        LDAB    FRRSDAT+0       ; LOAD DATE (DAY OF MONTH)
        JSR     BCDHEX1
        STD     MBFRRSDAT       ; MODBUS FRR START [MONTH:DATE]

;roll end time
        LDAB    FRRETIM         ;~ROLL END TIME HRS/MINS/DATE/MON
        JSR     BCDHEX1         ; CONVERT HOURS TO HEX
        TBA
        LDAB    FRRETIM+1
        JSR     BCDHEX1
        STD     MBFRRETIM       ; MODBUS FRR END [HOURS:MINS]

        LDAB    FRREDAT+1       ;~ROLL END TIME DATE/MON (LOAD MONTH)
        JSR     BCDHEX1         ; CONVERT MONTH TO HEX
        TBA
        LDAB    FRREDAT+0       ; LOAD DATE (DAY OF MONTH)
        JSR     BCDHEX1
        STD     MBFRREDAT       ; MODBUS FRR END [MONTH:DATE]

; COMPONENT DATA
        LDAB    #RAMBANK
        TBZK
        LDZ     #MBFRRBLK1      ; MODBUS TABLE, CH #1 CAL BLOCK
        LDAB    #1              ; BLEND #, START AT 1

CPYFRR10 PSHB

        LDX     #FRRBLK1
        DECB
        LDAA    #FRRBSIZ      ;
        MUL
        ABX                     ; X NOW POINTS TO ROLL REPORT BLOCK FOR CH.
        LDD     0,X             ; PICK UP TARGET %
        JSR     BCDHEX2
        STD     0,Z

        AIX     #2              ; X NOW POINTS TO Kg FOR CH.
        LDD     0,X
        LDE     2,X
        JSR     BCDHEX4                 ; CONVERT 4 BYTES TO HEX
;        JSR     BCDHEX3X        ; CONVERT TO HEX (3BYTES)
;        CLRA
        STD     2,Z             ; STORE IN MODBUS TABLE
        STE     4,Z

        AIX     #3              ; X NOW POINTS TO SCREW FOR CH.
        LDAB    0,X
        CLRA
        STD     6,Z             ; MODBUS SCREW TYPE

        PULB
        AIZ     #MBFRRBSIZ      ; ADVANCE DESTINATION PTR TO NEXT COMP BLOCK
        INCB                    ; REPEAT FOR ALL BLENDS
        CMPB    NOBLNDS
        LBLS    CPYFRR10        ;

; Liquid Additive (NOTE USES NEXT BLOCK AFTER LAST BLEND COMPONENT)
        LDAB   FRRAPIB
        BEQ    CPYFRR_X       ; DONT SEND LIQ ADD. BLOCK IF AVECPIB=0

        LDAB    NOBLNDS
        LDX     #FRRBLK1
        LDAA    #FRRBSIZ      ;
        MUL
        ABX                     ; X NOW POINTS TO FRONT ROLL REPORT BLOCK FOR LIQ.

        LDD     0,X             ; LIQ. ADD. % SETPT (ABC.D%)
        JSR     BCDHEX2
        STD     MBLFRRPCT       ;

        AIX     #2              ; LIQ. WT ACCUM. (ABCD.EF Kg)
        JSR     BCDHEX3X        ; CONVERT TO HEX
        CLRA
        STD     MBLFRRKG
        STE     MBLFRRKG+2

CPYFRR_X:
        LDD     MBNEWDATA
        ORD     #$0100          ; SET B8 OF MBNEWDATA TO SHOW FRONT ROLL CHANGE
        STD     MBNEWDATA

        RTS



; CPYBRRMB - ROUTINE TO COPY BACK ROLL REPORT INTO MODBUS TABLE
CPYBRRMB:
        TST      PROTOCOL
        LBPL     CPYBRR_X            ; ONLY COPY IF IN MODBUS PROTOCOL

        LDAB     BRRNBDS           ; NO. OF BLENDS
        CLRA
        STD      MBBRRNBDS

        LDX      #WTPMSP+1             ; LOAD THE G/M SP
        JSR      BCDHEX3X           ; CONVERT 3 BYTES AT X TO HEX
        TSTB                        ; CHECK FOR > 65535
        BEQ      NOOVFB               ; OVER FLOW
        LDE      #MAXMODBUS
NOOVFB  STE      MBBRRWPMS


;       LDX      #BRRWPMS           ;~WT/M SETPOINT
;       JSR      BCDHEX3X           ; CONVERT 3 BYTES AT X TO HEX
;       STE      MBBRRWPMS

        LDX      #BRRGPAS           ;~WT/M2 SETPOINT (g/AREA)
        JSR      BCDHEX3X           ; CONVERT 3 BYTES AT X TO HEX
        STE      MBBRRGPAS

;MOVE TOTAL KG/H SETPOINT TO REPORT
        LDX      #BRRKGHS           ;~TOTAL KG/H SP
        LDAB     0,X
        LDE      1,X
        LSRB
        RORE
        LSRB
        RORE
        LSRB                        ; SHIFT LOT 4 BITS RIGHT (DIV BY 10)
        RORE                        ; AB CDEF -> 0A BCDE
        LSRB
        RORE

        JSR      BCDHEX3            ; CONVERT 3 BYTES  TO HEX
        STE      MBBRRKGHS          ; MAX EXPECTED = 6553.5 KG/H
;TRANSFER LINE SPEED SETPT
        LDD      BRRLSPS
        ASLD
        ASLD                         ; ASSUMING MAX VALUE OF 999.
        ASLD
        ASLD                         ; NOW HAVE 10 TIMES
        JSR      BCDHEX2
        STD      MBBRRLSPS

        LDD     BRRWIDS            ;~WIDTH SETPOINT
        JSR     BCDHEX2
        STD     MBBRRWIDS

        LDD     BRRTWID            ;~TRIMMED WIDTH
        JSR     BCDHEX2
        STD     MBBRRTWID


;MOVE ROLL WEIGHT TO ROLL REPORT
        LDX      #BRRRWTVST+1     ;~FRONT ROLL WT
        JSR     BCDHEX3X          ; CONVERT TO HEX
        CLRA
        STD      MBBRRWTV
        STE      MBBRRWTV+2
;MOVE SLIT ROLL WT TO REPORT
        LDX      #BRRSRWV          ;~FRONT SLIT ROLL WT
        JSR      BCDHEX3X          ; CONVERT TO HEX
        CLRA
        STD      MBBRRSWV
        STE      MBBRRSWV+2

;MOVE ORDER WT TO REPORT  (ABCDEF Kg)
        LDX      #BRROWTV          ;~ORDER WT
        JSR      BCDHEX3X          ; CONVERT TO HEX
        CLRA
        STD      MBBRROWV
        STE      MBBRROWV+2
;MOVE LENGTH TO ROLL REPORT
        LDD      BRRRLTV           ;~FRONT ROLL LENGTH (m 2DP)
        JSR      BCDHEX2           ; CONVERT (m) TO HEX
        STD      MBBRRLTV          ; EXPECTING MAX LENGTH OF 65,535m ON ROLL

;MOVE ORDER LENGTH TO ROLL REPORT
        LDX      #BRROLTV          ;~ORDER LENGTH
        LDAB     1,X
        LDE      2,X
        JSR      BCDHEX3          ; CONVERT TO HEX
        CLRA
        STD      MBBRROLV
        STE      MBBRROLV+2
;MOVE CUSTOMER CODE, MATERIAL CODE & ORDER NO. TO REPORT
        LDX     #BRRCCOD
        LDAA    0,X
        JSR     C2ASCII         ; CONVERT 2 DIGITS TO 2 ASCII CHAR.S
        STD     MBBRRCCOD
        LDAA    1,X
        JSR     C2ASCII         ; CONVERT 2 DIGITS TO 2 ASCII CHAR.S
        STD     MBBRRCCOD+2
        LDAA    2,X
        JSR     C2ASCII         ; CONVERT 2 DIGITS TO 2 ASCII CHAR.S
        STD     MBBRRCCOD+4

        LDX     #BRRMCOD        ; MATERIAL CODE
        LDAA    0,X
        JSR     C2ASCII         ; CONVERT 2 DIGITS TO 2 ASCII CHAR.S
        STD     MBBRRMCOD
        LDAA    1,X
        JSR     C2ASCII         ; CONVERT 2 DIGITS TO 2 ASCII CHAR.S
        STD     MBBRRMCOD+2
        LDAA    2,X
        JSR     C2ASCII         ; CONVERT 2 DIGITS TO 2 ASCII CHAR.S
        STD     MBBRRMCOD+4

        LDX     #BRRONUM        ; MATERIAL CODE
        LDAA    0,X
        JSR     C2ASCII         ; CONVERT 2 DIGITS TO 2 ASCII CHAR.S
        STD     MBBRRONUM
        LDAA    1,X
        JSR     C2ASCII         ; CONVERT 2 DIGITS TO 2 ASCII CHAR.S
        STD     MBBRRONUM+2
        LDAA    2,X
        JSR     C2ASCII         ; CONVERT 2 DIGITS TO 2 ASCII CHAR.S
        STD     MBBRRONUM+4

;MOVE FRONT & BACK  ROLL NO.S INTO REPORT
        LDD     BRRFRNO         ; FRONT ROLL NO
        JSR     BCDHEX2
        STD     MBBRRFRNO

        LDD     BRRBRNO         ; BACK ROLL NO
        JSR     BCDHEX2
        STD     MBBRRBRNO

        LDAB    BRRSTUB            ;~SHEET/TUBE FLAG
        CLRA
        STD     MBBRRSTUB

        LDAB    BRRNBSR         ; NO OF BACK SLIT REELS
        JSR     BCDHEX1
        TBA
        LDAB    BRRNFSR            ;~NO. OF FRONT SLIT REELS
        JSR     BCDHEX1

        STD     MBBRRNSLR       ; [BACK SLITS:FRONT SLITS]

        CLRA
        LDAB    BRRTFED         ; TRIM FED BACK FLAG
        STD     MBBRRTFED
        LDAB    BRRAPIB
        STD     MBBRRAPIB       ; PIB ASSOCIATED FLAG

        LDD     BRRPADD
        JSR     BCDHEX2
        STD     MBBRRPADD

;roll start time
        LDAB    BRRSTIM         ;~ROLL START TIME HRS/MINS/DATE/MON
        JSR     BCDHEX1         ; CONVERT HOURS TO HEX
        TBA
        LDAB    BRRSTIM+1
        JSR     BCDHEX1
        STD     MBBRRSTIM       ; MODBUS BRR START [HOURS:MINS]

        LDAB    BRRSDAT+1       ;~ROLL START TIME DATE/MON (LOAD MONTH)
        JSR     BCDHEX1         ; CONVERT MONTH TO HEX
        TBA
        LDAB    BRRSDAT+0       ; LOAD DATE (DAY OF MONTH)
        JSR     BCDHEX1
        STD     MBBRRSDAT       ; MODBUS BRR START [MONTH:DATE]

;roll end time
        LDAB    BRRETIM         ;~ROLL END TIME HRS/MINS/DATE/MON
        JSR     BCDHEX1         ; CONVERT HOURS TO HEX
        TBA
        LDAB    BRRETIM+1
        JSR     BCDHEX1
        STD     MBBRRETIM       ; MODBUS BRR END [HOURS:MINS]

        LDAB    BRREDAT+1       ;~ROLL END TIME DATE/MON (LOAD MONTH)
        JSR     BCDHEX1         ; CONVERT MONTH TO HEX
        TBA
        LDAB    BRREDAT+0       ; LOAD DATE (DAY OF MONTH)
        JSR     BCDHEX1
        STD     MBBRREDAT       ; MODBUS BRR END [MONTH:DATE]

; COMPONENT DATA
        LDAB    #RAMBANK
        TBZK
        LDZ     #MBBRRBLK1      ; MODBUS TABLE, CH #1 CAL BLOCK
        LDAB    #1              ; BLEND #, START AT 1

CPYBRR10 PSHB

        LDX     #BRRBLK1
        DECB
        LDAA    #BRRBSIZ      ;
        MUL
        ABX                     ; X NOW POINTS TO ROLL REPORT BLOCK FOR CH.
        LDD     0,X             ; PICK UP TARGET %
        JSR     BCDHEX2
        STD     0,Z

        AIX     #2              ; X NOW POINTS TO Kg FOR CH.
        LDD     0,X
        LDE     2,X
        JSR     BCDHEX4                 ; CONVERT 4 BYTES TO HEX
        STD     2,Z             ; STORE IN MODBUS TABLE
        STE     4,Z

        AIX     #3              ; X NOW POINTS TO SCREW FOR CH.
        LDAB    0,X
        CLRA
        STD     6,Z             ; MODBUS SCREW TYPE

        PULB
        AIZ     #MBBRRBSIZ      ; ADVANCE DESTINATION PTR TO NEXT COMP BLOCK
        INCB                    ; REPEAT FOR ALL BLENDS
        CMPB    NOBLNDS
        LBLS    CPYBRR10        ;

; Liquid Additive (NOTE USES NEXT BLOCK AFTER LAST BLEND COMPONENT)
        LDAB   BRRAPIB
        BEQ    CPYBRR_X       ; DONT SEND LIQ ADD. BLOCK IF AVECPIB=0

        LDAB    NOBLNDS
        LDX     #BRRBLK1
        LDAA    #BRRBSIZ      ;
        MUL
        ABX                     ; X NOW POINTS TO BACK ROLL REPORT BLOCK FOR LIQ.

        LDD     0,X             ; LIQ. ADD. % SETPT (ABC.D%)
        JSR     BCDHEX2
        STD     MBLBRRPCT       ;


        AIX     #2              ; LIQ. WT ACCUM. (ABCD.EF Kg)
        JSR     BCDHEX3X        ; CONVERT TO HEX
        CLRA
        STD     MBLBRRKG
        STE     MBLBRRKG+2

CPYBRR_X:
        LDD     MBNEWDATA
        ORD     #$0200          ; SET B9 OF MBNEWDATA TO SHOW BACK ROLL CHANGE
        STD     MBNEWDATA


        RTS

; CPYORRMB - ROUTINE TO COPY ORDER REPORT INTO MODBUS TABLE
CPYORRMB:
        TST      PROTOCOL
        LBPL     CPYORR_X            ; ONLY COPY IF IN MODBUS PROTOCOL

        LDAB     ORRNBDS           ; NO. OF BLENDS
        CLRA
        STD      MBORRNBDS

        LDX      #ORRWPMS           ;~WT/M SETPOINT
        JSR      BCDHEX3X           ; CONVERT 3 BYTES AT X TO HEX
        STE      MBORRWPMS

        LDX      #ORRGPAS           ;~WT/M2 SETPOINT (g/AREA)
        JSR      BCDHEX3X           ; CONVERT 3 BYTES AT X TO HEX
        STE      MBORRGPAS

;MOVE TOTAL KG/H SETPOINT TO REPORT
        LDX      #ORRKGHS           ;~TOTAL KG/H SP
        LDAB     0,X
        LDE      1,X
        LSRB
        RORE
        LSRB
        RORE
        LSRB                        ; SHIFT 3 BYTES 4 BITS RIGHT (DIV BY 10)
        RORE                        ; AB CDEF -> 0A BCDE
        LSRB
        RORE

        JSR      BCDHEX3            ; CONVERT 3 BYTES  TO HEX
        STE      MBORRKGHS          ; MAX EXPECTED = 6553.5
;TRANSFER LINE SPEED SETPT
        LDD      ORRLSPS
        ASLD
        ASLD                         ; ASSUMING MAX VALUE OF 999.
        ASLD
        ASLD                         ; NOW HAVE 10 TIMES
        JSR      BCDHEX2
        STD      MBORRLSPS           ; LINE SPEED SETPT IN MB TO 1 DP

        LDD     ORRWIDS            ;~WIDTH SETPOINT
        JSR     BCDHEX2
        STD     MBORRWIDS

        LDD     ORRTWID            ;~TRIMMED WIDTH
        JSR     BCDHEX2
        STD     MBORRTWID

;MOVE ORDER WT TO REPORT  (ABCDEF Kg)

        LDX      #ORRTOTW+1              ;~ORDER WT
        JSR      BCDHEX3X                ; CONVERT 4 BYTES TO HEX
        CLRA
        STD      MBORROWV
        STE      MBORROWV+2


;MOVE ORDER LENGTH TO  REPORT
        LDX      #ORROLTV          ;~ORDER LENGTH
;        LDAB     1,X
;        LDE      2,X
        JSR      BCDHEX3X         ; CONVERT TO HEX
        CLRA
        STD      MBORROLV
        STE      MBORROLV+2
;MOVE CUSTOMER CODE, MATERIAL CODE & ORDER NO. TO REPORT
        LDX     #ORRCCOD
        LDAA    0,X
        JSR     C2ASCII         ; CONVERT 2 DIGITS TO 2 ASCII CHAR.S
        STD     MBORRCCOD
        LDAA    1,X
        JSR     C2ASCII         ; CONVERT 2 DIGITS TO 2 ASCII CHAR.S
        STD     MBORRCCOD+2
        LDAA    2,X
        JSR     C2ASCII         ; CONVERT 2 DIGITS TO 2 ASCII CHAR.S
        STD     MBORRCCOD+4

        LDX     #ORRMCOD        ; MATERIAL CODE
        LDAA    0,X
        JSR     C2ASCII         ; CONVERT 2 DIGITS TO 2 ASCII CHAR.S
        STD     MBORRMCOD
        LDAA    1,X
        JSR     C2ASCII         ; CONVERT 2 DIGITS TO 2 ASCII CHAR.S
        STD     MBORRMCOD+2
        LDAA    2,X
        JSR     C2ASCII         ; CONVERT 2 DIGITS TO 2 ASCII CHAR.S
        STD     MBORRMCOD+4

        LDX     #ORRONUM        ; MATERIAL CODE
        LDAA    0,X
        JSR     C2ASCII         ; CONVERT 2 DIGITS TO 2 ASCII CHAR.S
        STD     MBORRONUM
        LDAA    1,X
        JSR     C2ASCII         ; CONVERT 2 DIGITS TO 2 ASCII CHAR.S
        STD     MBORRONUM+2
        LDAA    2,X
        JSR     C2ASCII         ; CONVERT 2 DIGITS TO 2 ASCII CHAR.S
        STD     MBORRONUM+4

;MOVE FRONT & BACK  ROLL NO.S INTO REPORT
        LDD     ORRFRNO         ; FRONT ROLL NO
        JSR     BCDHEX2
        STD     MBORRFRNO

        LDD     ORRBRNO         ; BACK ROLL NO
        JSR     BCDHEX2
        STD     MBORRBRNO

        LDAB    ORRSTUB            ;~SHEET/TUBE FLAG
        CLRA
        STD     MBORRSTUB

        LDAB    ORRNBSR         ; NO OF BACK SLIT REELS
        JSR     BCDHEX1
        TBA
        LDAB    ORRNFSR            ;~NO. OF FRONT SLIT REELS
        JSR     BCDHEX1

        STD     MBORRNSLR       ; # OF SLIT RELLS[BACK:FRONT]

        CLRA
        LDAB    ORRTFED         ; TRIM FED BACK FLAG
        STD     MBORRTFED
        LDAB    ORRAPIB
        STD     MBORRAPIB       ; PIB ASSOCIATED FLAG

        LDD     ORRPADD
        JSR     BCDHEX2
        STD     MBORRPADD

;order start time
        LDAB    ORRSTIM         ;~ROLL START TIME HRS/MINS/DATE/MON
        JSR     BCDHEX1         ; CONVERT HOURS TO HEX
        TBA
        LDAB    ORRSTIM+1
        JSR     BCDHEX1
        STD     MBORRSTIM       ; MODBUS ORR START [HOURS:MINS]

        LDAB    ORRSDAT+1       ;~ROLL START TIME DATE/MON (LOAD MONTH)
        JSR     BCDHEX1         ; CONVERT MONTH TO HEX
        TBA
        LDAB    ORRSDAT+0       ; LOAD DATE (DAY OF MONTH)
        JSR     BCDHEX1
        STD     MBORRSDAT       ; MODBUS ORR START [MONTH:DATE]

;order end time
        LDAB    ORRETIM         ;~ROLL END TIME HRS/MINS/DATE/MON
        JSR     BCDHEX1         ; CONVERT HOURS TO HEX
        TBA
        LDAB    ORRETIM+1
        JSR     BCDHEX1
        STD     MBORRETIM       ; MODBUS ORR END [HOURS:MINS]

        LDAB    ORREDAT+1       ;~ROLL END TIME DATE/MON (LOAD MONTH)
        JSR     BCDHEX1         ; CONVERT MONTH TO HEX
        TBA
        LDAB    ORREDAT+0       ; LOAD DATE (DAY OF MONTH)
        JSR     BCDHEX1
        STD     MBORREDAT       ; MODBUS ORR END [MONTH:DATE]

; COMPONENT DATA
        LDAB    #RAMBANK
        TBZK
        LDZ     #MBORRBLK1      ; MODBUS TABLE, CH #1 CAL BLOCK
        LDAB    #1              ; BLEND #, START AT 1

CPYORR10 PSHB
        LDX     #ORRBLK1
        DECB
        LDAA    #ORRBSIZ      ;
        MUL
        ABX                     ; X NOW POINTS TO ROLL REPORT BLOCK FOR CH.
        LDD     0,X             ; PICK UP TARGET %
        JSR     BCDHEX2
        STD     0,Z

        AIX     #2              ; X NOW POINTS TO Kg FOR CH.

        LDD     0,X
        LDE     2,X
        JSR     BCDHEX4                 ; CONVERT 4 BYTES TO HEX

;        JSR     BCDHEX3X        ; CONVERT TO HEX (3BYTES)
;        CLRA
        STD     2,Z             ; STORE IN MODBUS TABLE
        STE     4,Z

        AIX     #3              ; X NOW POINTS TO K VALUE FOR CH.
        JSR     BCDHEX3X        ; CONVERT TO HEX (3BYTES)
        CLRE
        STE     8,Z             ; EXPECTING MAX 655.35

        AIX     #3              ; X NOW POINTS TO SCREW FOR CH.
        LDAB    0,X
        CLRD
        STD     6,Z             ; MODBUS SCREW TYPE


        PULB
        AIZ     #MBORRBSIZ      ; ADVANCE DESTINATION PTR TO NEXT COMP BLOCK
        INCB                    ; REPEAT FOR ALL BLENDS
        CMPB    NOBLNDS
        LBLS    CPYORR10        ;

; Liquid Additive  (NOTE USES NEXT BLOCK AFTER LAST BLEND COMPONENT)
        LDAB   ORRAPIB
        BEQ    CPYORR_X       ; DONT SEND LIQ ADD. BLOCK IF AVECPIB=0

        LDAB    NOBLNDS
        LDX     #ORRBLK1
        LDAA    #ORRBSIZ      ;
        MUL
        ABX                     ; X NOW POINTS TO ORDER REPORT BLOCK FOR LIQ.

        LDD     0,X             ; LIQ. ADD. % SETPT (ABC.D%)
        JSR     BCDHEX2
        STD     MBLORRPCT       ;


        AIX     #2              ; LIQ. WT ACCUM. (ABCD.EF Kg)
        JSR     BCDHEX3X        ; CONVERT TO HEX
        CLRA
        STD     MBLORRKG
        STE     MBLORRKG+2

        AIX     #3              ; X NOW POINTS TO K VALUE FOR LIQ.
        JSR     BCDHEX3X        ; CONVERT TO HEX (3BYTES)
        STE     MBLORR_K        ; EXPECTING MAX 655.35

CPYORR_X:
        LDD     #1234
        STD     MBORREND        ; JUST TO CHECK ADDR.

        LDD     MBNEWDATA
        ORD     #$0400          ; SET B10 OF MBNEWDATA TO SHOW ORDER CHANGE
        STD     MBNEWDATA
        RTS



;
;       GENERATE LONG TERM MONTHLY REPORT.
;

GENLTREP:
        JSR     LTPER           ; LONG TERM %
        LDD     STTIMEM         ; READ START TIME.
        STD     LTSTRP          ; TO
        LDD     STTIMEM+2
        STD     LTSTRP+2
        LDX     #TIMEBUF
        LDAA    HOUR,X
        STAA    ENDTIMM
        LDAA    MIN,X
        STAA    ENDTIMM+1
        LDAA    DATE,X
        STAA    ENDTIMM+2
        LDAA    MON,X
        STAA    ENDTIMM+3
        LDX      #LTSTRP
        STX      MVMPTR
        LDX      #STTIMEM
        LDAB     #LTBLSIZ
        JSR      MOVMRY            ;~TRANSFER
        LDAA    ENDTIMM
        STAA    STTIMEM
        LDAA    ENDTIMM+1
        STAA    STTIMEM+1
        LDAA    ENDTIMM+2
        STAA    STTIMEM+2
        LDAA    ENDTIMM+3
        STAA    STTIMEM+3

        LDX     #TOTWACCM
;        LDAB    #9*5               ; RESET WEIGHT ACCUMULATORS.
        LDAA    #FIVE
        LDAB    #MAXCOMPONENTNO+1
        MUL
        TBA
        JSR     CLRBBS
        LDAA    #$80
        ORAA    TOPRNFLG           ; ENABLE
        STAA    TOPRNFLG           ;

        RTS

;       HOURLY REPORT GENERATION.

GENHRREP:
        JSR     CALCHRLENGTH    ; CALCULATE HOURLY LENGTH.
        JSR     PRECOMPHOUR
        JSR     HRPER           ; CALL % CALC
        LDD     STTIMEH         ; READ START TIME.
        STD     HRSTRP          ; TO
        LDD     STTIMEH+2
        STD     HRSTRP+2
        LDX     #TIMEBUF
        LDAA    HOUR,X
        STAA    ENDTIMH
        LDAA    MIN,X
        STAA    ENDTIMH+1
        LDAA    DATE,X
        STAA    ENDTIMH+2
        LDAA    MON,X
        STAA    ENDTIMH+3
        LDX      #HRSTRP
        STX      MVMPTR
        LDX      #STTIMEH
        LDAB     #HRBLSIZ
        JSR      MOVMRY            ;~TRANSFER
        LDAA    ENDTIMH
        STAA    STTIMEH
        LDAA    ENDTIMH+1
        STAA    STTIMEH+1
        LDAA    ENDTIMH+2
        STAA    STTIMEH+2
        LDAA    ENDTIMH+3
        STAA    STTIMEH+3

        LDX     #TOTWACCH
;        LDAB    #9*5               ; RESET WEIGHT ACCUMULATORS.
        LDAA    #FIVE
        LDAB    #MAXCOMPONENTNO+1
        MUL
        TBA
        JSR     CLRBBS
        LDAA    #$40
        ORAA    TOPRNFLG           ; ENABLE
        STAA    TOPRNFLG           ;
        JSR     POSTCOMPHOUR
        RTS

CALCFSLIT:
       LDX    #FRRRWTVST
CALCFSLITX:
       JSR    ITCREG            ;~ACCUMULATED WT -> CREG
       JSR    CLAREG
       LDAA   FSLITRS           ;~NO. OF FRONT SLIT ROLLS
       STAA   AREG+4
       JSR    DIV                       ;~NOW WE HAVE WT. OF SLIT ROLL
       LDX    #FSWTACC                  ;~FRONT SLIT WT ACC
       JSR    IFEREG
       LDX      #EREG+1
       RTS

CALCBSLIT:
       LDX    #BRRRWTVST        ;~BACK ROLL WEIGHT ACCUMULATED VALUE
CALCBSLITX:
       JSR    ITCREG            ;~ACCUMULATED WT -> CREG
       JSR    CLAREG
       LDAA   BSLITRS           ;~NO. OF BACK  SLIT ROLLS
       STAA   AREG+4
       JSR    DIV                       ;~NOW WE HAVE WT. OF SLIT ROLL
       LDX    #BSWTACC                  ;~BACK SLIT WT ACC
       JSR    IFEREG
       LDX    #EREG+1
       RTS


; BATCH REPORT GENERATION


GENBATREPORT:
        LDX      #BATSTRP
        STX      MVMPTR
        LDX      #STTIMEH
        LDAB     #BATBLSIZ
        JSR      MOVMRY            ;~TRANSFER
        RTS







;       BATCH WEIGHT COMPENSATION.
;
;       XX XX YY YY GRAMS / 3600 SECONDS  * ( XX XX .YY SECONDS SINCE LAST DUMP )
;       ( GRAMS / HOUR FOR LAST BATCH) / 3600 SECONDS ( BATCH SECONDS SO FAR)


BATCHWTCOMP:
        JSR     CLCREG7         ; RESET REG
        JSR     CLAREG7          ; C=0
        LDX     #0008           ;
        STX     MATHCREG        ;
        LDX     TPTKGH          ; A = TPTKGH
        STX     MATHCREG+2
        LDAA    TPTKGH+2
        STAA    MATHCREG+4
        LDX     #0004
        STX     AREG            ;
        LDX     #$3600          ; 1 HOUR OF SECONDS.
        STX     AREG+2
        JSR     FDIV            ;
        LDX     #MATHDREG
        JSR     FTAREG         ; RESULT TO AREG
        JSR     CLCREG7         ;  C = 0

        LDD     KGHRCOUNT       ; READ SECONDS
        JSR     HEXBCD2
        LDX     #0004
        STX     MATHCREG        ; C = KGHRCOUNT (BCD)
        STE     MATHCREG+2      ; SECONDS TRANSFERRED FOR ADDITION.
        JSR     FMUL            ; MULTIPLICATION.
        JSR     FPTINC          ; INTEGER FORMAT.
        LDD     MATHDREG+2      ; EXTRA ON LAST ROLL
        STD     BATCHUSEDWT  ;
        LDAA    MATHDREG+4      ;
        STAA    BATCHUSEDWT+2  ;

;       CALCULATE UNUSED BATCH WEIGHT
;

        JSR     CLRAC
        LDX     #PREVBATCHWEIGHT
        JSR     AMOVE           ; BATCH WEIGHT
        LDX     #BATCHUSEDWT
        JSR     CMOVE
        JSR     SUB
        LDX     #BATCHUNUSEDWT
        JSR     EMOVE
        JSR      CALBTPER         ; CALCULATE COMP COMPSENSATED WEIGHT
        RTS



; CALCULATION OF COMPONENT WEIGHTS FOR SYSTEM

CALBTPER:
        JSR     CLAREG          ; RESET A AND C REGISTERS.
        LDAB   #RAMBANK
        TBZK                    ;~;SET EK FOR BANK 1, I.E. RAM
        LDZ     #SEQTABLE       ;
REPCLW  LDX     #BATCHUNUSEDWT
        JSR     AMOVE           ; TO A REG
        JSR     CLCREG
        LDAB    0,Z             ; READ COMPONENT NO.
        ANDB    #$0F
        PSHB
        LDX     #PCNT1
        DECB
        ASLB
        ABX
        LDD     0,X
        STD     MATHCREG+3
        JSR     MUL             ; XXX.Y% X XXXXX.Y GRAMS
        LDX     #CMP1ACTWCM
        PULB
        DECB
        ABX
        ABX
        ABX
        LDD     EREG
        STD     0,X             ; STORE ACTUAL WEIGHT
        LDAA    EREG+2          ;
        STAA    2,X             ;
        AIZ     #1
        LDAA    0,Z
        CMPA    #SEQDELIMITER
        BNE     REPCLW
        RTS




 from bbcalc74.asm



; THIS PROGRAM SUBTRACTS THE COMPENSATED WEIGHT FROM THE TOTAL ROLL
; COMPONENT WEIGHTS WEIGHTS.

SUBCOMPWT:
        LDAB    #1
REPADDIT:
        PSHM    X               ; SAVE ADDRESS STORAGE
        PSHB                    ;       SAVE COMPONENT NO
        DECB
        ABX
        ABX
        ABX
        ABX
        ABX                     ; ADDRESS FOR DATA STORAGE.
        JSR     ITAREG          ;
        LDX     #CMP1ACTWCM     ; COMP 1 ACTUAL WEIGHT
        PULB
        PSHB
        DECB                    ;
        ABX
        ABX
        ABX
        JSR     CMOVE           ; COMP WEIGHT TO A REG 00 00 XX XX X.Y GRAMS
        JSR     SUB
        PULB
        PULM    X               ; COMP 1 ACTUAL WEIGHT
        PSHM    X
        PSHB
        DECB                    ;
        ABX
        ABX
        ABX
        ABX
        ABX
        JSR     IFEREG          ; STORE ACCUMULATED DATA.
        PULB
        PULM    X
        CMPB    NOBLNDS         ; AT LAST ONE
        BEQ     YESEXIT         ;
        INCB                    ; TO NEXT BLEND.
        BRA     REPADDIT

YESEXIT:
        JSR     GENTOTALS       ; GENERATE TOTALS
        JSR     ORDERPER        ; CALCULATE ACTUAL %
        RTS



;       ADDS COMPONENT COMPENSATED WEIGHT TO THE INDIVIDUAL COMPONENT DATA.
;       X REG POINTS TO THE COMPONENT WEIGHTS


ADDCOMPWT:
        LDAB    #1
REPADD:
        PSHM    X               ; SAVE ADDRESS STORAGE
        PSHB                    ;       SAVE COMPONENT NO
        DECB
        ABX
        ABX
        ABX
        ABX
        ABX                     ; ADDRESS FOR DATA STORAGE.
        JSR     ITAREG          ;
        LDX     #CMP1ACTWCM     ; COMP 1 ACTUAL WEIGHT
        PULB
        PSHB
        DECB                    ;
        ABX
        ABX
        ABX
        JSR     CMOVE           ; COMP WEIGHT TO A REG 00 00 XX XX X.Y GRAMS
        JSR     ADD
        PULB
        PULM    X               ; COMP 1 ACTUAL WEIGHT
        PSHM    X
        PSHB
        DECB                    ;
        ABX
        ABX
        ABX
        ABX
        ABX
        JSR     IFEREG          ; STORE ACCUMULATED DATA.
        PULB
        PULM    X
        CMPB    NOBLNDS         ; AT LAST ONE
        BEQ     YESEXAD         ;
        INCB                    ; TO NEXT BLEND.
        BRA     REPADD

YESEXAD:
        JSR     GENTOTALS       ; GENERATE TOTALS
        JSR     ORDERPER        ; CALCULATE ACTUAL %
        RTS

;       FRONT ROLL COMPENSATION.
;

PRECOMPFROLL:
        JSR     BATCHWTCOMP     ; CALCULATE UNUSED WEIGHT
        LDX     #CH1WACCF
        JSR     SUBCOMPWT
        RTS

POSTCOMPFROLL:
        LDX     #CH1WACCF
        JSR     ADDCOMPWT
        RTS

;       BACK ROLL COMPENSATION.
;

PRECOMPBROLL:
        JSR     BATCHWTCOMP     ; CALCULATE UNUSED WEIGHT
        LDX     #CH1WACCB
        JSR     SUBCOMPWT
        RTS

POSTCOMPBROLL:
        LDX     #CH1WACCB
        JSR     ADDCOMPWT
        RTS


;       HOURLY COMPENSATION.
;
PRECOMPHOUR:
        JSR     BATCHWTCOMP     ; CALCULATE UNUSED WEIGHT
        LDX     #CH1WACCH
        JSR     SUBCOMPWT
        RTS

POSTCOMPHOUR:
        LDX     #CH1WACCH
        JSR     ADDCOMPWT
        RTS


;       SHIFT COMPENSATION.
;
PRECOMPSHIFT:
        JSR     BATCHWTCOMP     ; CALCULATE UNUSED WEIGHT
        LDX     #CH1WACCS
        JSR     SUBCOMPWT
        RTS

POSTCOMPSHIFT:
        LDX     #CH1WACCS
        JSR     ADDCOMPWT
        RTS

;       ORDER COMPENSATION
;
PRECOMPORDER:
        JSR     BATCHWTCOMP     ; CALCULATE UNUSED WEIGHT
        LDX     #CH1WACCO
        JSR     SUBCOMPWT
        RTS

POSTCOMPORDER:
        LDX     #CH1WACCO
        JSR     ADDCOMPWT
        RTS


ORDERPER:
        LDX     #ORBPCNT1
        LDAA    #24
        JSR    CLRMEM   ;~CLEAR MEMEORY

        LDX     #SEQTABLE       ; COMPONENT START ADDRESS
NEXTCOMPPER:
        LDAB    0,X             ; READ COMP NO
        ANDB    #$0F
        PSHM    X               ; SAVE INFO AS REQUIRED.
        PSHB
        LDX     #$0006          ;EXPONENT FOR DATA
        STX     AREG
        STX     MATHCREG        ;
        LDX     #TOTWACCO       ; ORDER WEIGHT
        LDD     0,X
        STD     AREG+2
        LDD     2,X
        STD     AREG+4
        LDAA    4,X
        STAA    AREG+6          ; DENOMINATOR.
        TST     REG_PRESENT     ; ANY REGRIND.
        BEQ     NORPCL          ; DO NORMAL % CALCULATION.
        PULB                    ; READ COMPONENT NO
        PSHB
        CMPB    REGCOMP         ; IS THIS THE REGRIND COMPONENT ?
        BEQ     NORPCL          ; NORMAL CALCULATION
        LDX     #CH1WACCO       ; SUBTRACT OFF REGRIND COMPONENT
        LDAB    REGCOMP
        DECB
        ABX
        ABX
        ABX
        ABX
        ABX
        LDD     0,X
        STD     MATHCREG+2
        LDD     2,X
        STD     MATHCREG+4
        LDAA    4,X
        STAA    MATHCREG+6      ; COMPONENT WEIGHT
        JSR     FSUB            ; SUBTRACT OFF REGRIND WEIGHT FROM TOTAL
        LDX     #MATHDREG
        JSR     FTAREG          ; RESULT BACK TO A  (TOTAL WEIGHT - REGRIND COMPONENT)


NORPCL  LDX     #$0006          ;EXPONENT FOR DATA
        STX     MATHCREG        ;
        LDX     #CH1WACCO       ; ORDER WEIGHT FOR CHANNEL #1
        PULB
        PSHB                    ; COMP NO
        DECB
        ABX
        ABX
        ABX
        ABX
        ABX
        LDD     0,X
        STD     MATHCREG+2
        LDD     2,X
        STD     MATHCREG+4
        LDAA    4,X
        STAA    MATHCREG+6      ; COMPONENT WEIGHT
        JSR     FDIV
        JSR     FPTINC          ; INTEGER FORMAT
        PULB                    ; COMP NO
        LDX     #ORBPCNT1       ;
        DECB
        ABX
        ABX
        ABX
        LDAA    MATHDREG+4
        STAA    0,X             ; COMP % STORED.
        LDD     EREG
        STD     1,X             ; XXXX.YY % STORED.
        PULM    X
        AIX     #1
        LDAA    0,X
        CMPA    #SEQDELIMITER   ; AT END OF TABLE ?
        LBNE     NEXTCOMPPER
        RTS


 from sblspd38.asm

 GENTOTALS:
        LDX     #TOTWACCO       ;       TOTAL WEIGHT
        STX     TOTSTORAGE      ;       BUFFER
        LDX     #CH1WACCO       ;       COMPONENT TOTAL
;        JSR     ADDCMPTOTALS    ;
        JSR     ADDCMPTOTALSFR
        LDX     #TOTWACCF       ;       TOTAL WEIGHT
        STX     TOTSTORAGE      ;       BUFFER
        LDX     #CH1WACCF       ;       COMPONENT TOTAL
        JSR     ADDCMPTOTALS    ;
        LDX     #TOTWACCB       ;       TOTAL WEIGHT
        STX     TOTSTORAGE      ;       BUFFER
        LDX     #CH1WACCB       ;       COMPONENT TOTAL
        JSR     ADDCMPTOTALS    ;

        LDX     #CURSHIFTWEIGHT ;
        STX     TOTSTORAGE      ;
        LDX     #CH1WACCS       ;
        JSR     ADDCMPTOTALS  ;

        LDX     #TOTWACCS9      ;
        STX     TOTSTORAGE      ;
        LDX     #CH1WACCS9      ;
        JSR     ADDCMPTOTALS  ;

        LDX     #TOTWACCM       ;
        STX     TOTSTORAGE      ;
        LDX     #CH1WACCM       ;
        JSR     ADDCMPTOTALS  ;

        LDX     #TOTWACCH       ;
        STX     TOTSTORAGE      ;
        LDX     #CH1WACCH       ;
        JSR     ADDCMPTOTALS  ;

        LDX     #TOTWACCBAT   ;
        STX     TOTSTORAGE    ;
        LDX     #CH1WACCBAT   ;
        JSR     ADDCMPTOTALS  ;
        LDX     TOTWACCBAT    ; TRANSFER TO HISTORY
        STX     TOTWACCBATH   ;
        LDX     TOTWACCBAT+2  ;
        STX     TOTWACCBATH+2 ;
        LDAA    TOTWACCBAT+4  ;
        STAA    TOTWACCBATH+4 ;
        JSR     CONVKGHRI     ; CONVERT TOT IMPERIAL MODE.
        LDX     TOTWACCBATI    ; TRANSFER TO HISTORY
        STX     TOTWACCBATHI   ;
        LDX     TOTWACCBATI+2    ; TRANSFER TO HISTORY
        STX     TOTWACCBATHI+2   ;
        LDAA    TOTWACCBATI+4  ;
        STAA    TOTWACCBATHI+4 ;
        RTS




ADDCMPTOTALSFR:
        LDAB    #1
        STAB    FULLRES
        BRA     CONAD

ADDCMPTOTALS:
        CLR     FULLRES
CONAD   LDAB    #RAMBANK
        TBZK
        LDZ     #SEQTABLE       ; SEQUENCE TABLE SET UP
        JSR     CLRAC
;        LDX     #CH1WACCO        ; CHANNEL WEIGHT ACCUMULATOR
NEXTADD:
        PSHM    X
        LDAB    0,Z
        ANDB    #$0F
        DECB
        ABX
        ABX
        ABX
        ABX
        ABX
        JSR     ITAREG
        TST     FULLRES
        BNE     FULRES
        CLR     AREG+4          ; DISPLAYED ADDITION ADDS UP
FULRES  JSR     ADD             ; DO ADDITION
        LDX     #MATHCREG
        JSR     IFEREG          ; TRANSFER RESULT.
        PULM    X               ; RECOVER X REG
        AIZ     #1              ; TABLE POINTER
        LDAA    0,Z             ; NEXT COMP ?
        CMPA    #SEQDELIMITER   ; DELIMITER
        BNE     NEXTADD         ; NEXT COMP
        LDX     TOTSTORAGE      ; TOTAL STORAGE POINTER
        JSR     IFEREG
        RTS

*/
