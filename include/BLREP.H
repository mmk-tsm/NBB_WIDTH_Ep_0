//////////////////////////////////////////////////////
// GenerateRollReport.h
//
// Include file for the roll report generator.
//
//
// M.Parks                              11-05-2000
// First Pass.
// M.McKiernan                              27-02-2004
//      Added widths & sheet tube to roll report structure. Added trim fed back to layer struct.
//
// P,Smith                                  1-07-2005
// Generate roll report template
//
// P.Smith                                  13/07/05
// Define order report structure.
//
// P.Smith                                  7/10/05
// added structBatchReport & structBatchComponentReport
//
// P.Smith                      2/2/06
// first pass at netburner hardware conversion.
// Bool-> BOOL
//
// P.Smith                      26/11/08
// added CopyOrderReportToBuffer
//
// P.Smith                      15/1/09
// added DumpOrderReportToUSB
//////////////////////////////////////////////////////

//*****************************************************************************
// MACROS
//*****************************************************************************
#ifndef __BLREP_H__
#define __BLREP_H__

//*****************************************************************************
// INCLUDES
//*****************************************************************************

#include "General.h"
#include "TimeDate.h"
#include "SerialStuff.h"


void    GenerateRollReport( BOOL bSide, BOOL *bStatus );
void    PrintRollReport( BOOL bSide );
void CopyFrontRollChangeData( void );   
void BatchWeightCompensation( void );   
void PreCompensateFrontRollWeight( void );   
void PostCompensateFrontRollWeight( void );   
void PreCompensateBackRollWeight( void );
void PostCompensateBackRollWeight( void );   
void PreCompensateHourlyWeight( void );
void PostCompensateHourlyWeight( void );   
void PreCompensateOrderWeight( void );   
void PostCompensateOrderWeight( void );   
void PreCompensateShiftWeight( void );
void PostCompensateShiftWeight( void );   
void GenerateTotalWeights( void );   
void GenerateOrderComponentPercentages( void );   
void CopyFrontRollReportToMB( void );   
void CopyBackRollReportToMB( void );   
void CopyBackRollChangeData( void );   
void CopyBackRollReportToMB( void );   
void CopyOrderReportData( void );   
void CopyOrderReportToMB( void );   
void ResetOrderTotals( void );   
void CopyHourlyData( void );   
void CopyBatchData( void );
void CopyOrderReportToBuffer( void );
void DumpOrderReportToUSB(void);


   
   


// Empirically measured as 1000 bytes in English with
// 3 layers and 4 components each on 16-05-2000
#define ROLL_REPORT_PRINT_BUFFER_SIZE   1500


typedef struct {
    
    float   m_fPercentageSetpoint;

    float   m_fRollWeight;

} structRollComponentData;



typedef struct {
    
    float   m_fPercentageSetpoint;

    float   m_fWeight;

} structOrderComponentData;


typedef struct {
    BOOL    m_bStatus;      // TRUE = valid data, FALSE = timeout
    int     m_nComponentCount;
    BOOL        m_bTrimBeingFedBack;
    char    m_arrcDescriptor[MAX_DESCRIPTION_LENGTH];

    float   m_fThroughputSetpoint;
    float   m_fWeightLengthSetpoint;
    float   m_fRollWeight;
    float   m_fOrderWeight;

    structRollComponentData m_ComponentData[MAX_COMPONENTS];



} structRollLayerData;


typedef struct {
    BOOL    m_bSide;            // FALSE = Front, TRUE = Back
    int m_nLayerCount;          // No of components
    BOOL    m_bPrinted;         // FALSE = not yet printed, TRUE = print done
    float   m_fWeightLengthSetpoint;
    float   m_fTotalWeightArea;
    float   m_fThroughputSetpoint;
    float   m_fLineSpeedSetpoint;
    float   m_fRollWeight;
    float   m_fSlitRollWeight;
    float   m_fOrderWeight;
    float   m_fRollLength;
    float   m_fOrderLength;
    char    m_arrCustomerCode[CUSTOMERCODECHARACTERNO];
    char    m_arrMaterialCode[MATERIALCODECHARACTERNO];
    int     m_arrOrderNo[ORDERNOCHARACTERNO];
    int     m_nFrontRollNo;             // front roll no.
    int     m_nBackRollNo;             // back roll no.
    BOOL    m_bSheetTube;
    int     m_nFrontSlitReels;
    int     m_nBackSlitReels;
    float   m_fPercentageAdditive;
    float   m_fWidthSetpoint;
    float   m_fTrimmedWidth;
    BOOL    m_bTrimBeingFedBack;
    char    m_cStartTime[TIME_ARRAY_SIZE];  // Start Time of the event
    char    m_cEndTime[TIME_ARRAY_SIZE];    // EndTime of the event
    BOOL    m_bLiquidEnabled;               //  facility for liquid enabled (TRUE/FALSE)

    structRollComponentData m_ComponentData[MAX_COMPONENTS];

    } structRollReport;


typedef struct {
    int     m_nRollNo;
    float   m_fLength;
    char    m_cStartTime[TIME_ARRAY_SIZE];  // Start Time of the event
    char    m_cEndTime[TIME_ARRAY_SIZE];    // EndTime of the event


} structRollData;



typedef struct {
    int     m_nRollNo;
    float   m_fLength;
    char    m_cStartTime[TIME_ARRAY_SIZE];  // Start Time of the event
    char    m_cEndTime[TIME_ARRAY_SIZE];    // EndTime of the event


} structHourData;



typedef struct {
    int m_nLayerCount;          // No of components
    BOOL    m_bPrinted;         // FALSE = not yet printed, TRUE = print done
    float   m_fWeightLengthSetpoint;
    float   m_fTotalWeightArea;
    float   m_fThroughputSetpoint;
    float   m_fLineSpeedSetpoint;
    float   m_fWeight;
    float   m_fSlitRollWeight;
    float   m_fOrderWeight;
    float   m_fOrderLength;
    char    m_arrCustomerCode[CUSTOMERCODECHARACTERNO];
    char    m_arrMaterialCode[MATERIALCODECHARACTERNO];
    int     m_arrOrderNo[ORDERNOCHARACTERNO];
    int     m_nFrontRollNo;             // front roll no.
    int     m_nBackRollNo;             // back roll no.
    BOOL    m_bSheetTube;
    int     m_nFrontSlitReels;
    int     m_nBackSlitReels;
    float   m_fPercentageAdditive;
    float   m_fWidthSetpoint;
    float   m_fTrimmedWidth;
    BOOL    m_bTrimBeingFedBack;
    char    m_cStartTime[TIME_ARRAY_SIZE];  // Start Time of the event
    char    m_cEndTime[TIME_ARRAY_SIZE];    // EndTime of the event
    BOOL    m_bLiquidEnabled;               //  facility for liquid enabled (TRUE/FALSE)
    BOOL    m_bRegrindPresent;

    structOrderComponentData m_ComponentData[MAX_COMPONENTS];

}structOrderReport;



typedef struct {
    
    float   m_fRollWeight;

} structHourlyComponentData;


typedef struct {

    char    m_cStartTime[TIME_ARRAY_SIZE];  // Start Time of the event
    char    m_cEndTime[TIME_ARRAY_SIZE];    // EndTime of the event
    float   m_fWeight;
    float   m_fLength;

    structHourlyComponentData m_ComponentData[MAX_COMPONENTS];

}structHourlyReport;



typedef struct {
    
    float   m_fWeight;
    float   m_fPercentage;

} structBatchComponentData;


typedef struct {

    char    m_cStartTime[TIME_ARRAY_SIZE];  // Start Time of the event
    char    m_cEndTime[TIME_ARRAY_SIZE];    // EndTime of the event
    float   m_fWeight;

    structBatchComponentData m_ComponentData[MAX_COMPONENTS];

}structBatchReport;



typedef struct {
    float   m_fWeight;
    char    m_cStartTime[TIME_ARRAY_SIZE];  // Start Time of the event
    char    m_cEndTime[TIME_ARRAY_SIZE];    // EndTime of the event


} structBatchData;





#endif      //__BLREP_H__
