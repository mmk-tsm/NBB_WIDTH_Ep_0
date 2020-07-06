///////////////////////////////////////////////////////////
// SetpointFormat.h
//
// Declaration of the format of the Setpoint data
// structure
//
//
// First pass.
// M.McKiernan                              03-06-2004
//
// M.McKiernan                              14-09-2004
// Removed the regrind % and added a regrind component to structure.
//
// M.McKiernan                              26-11-2004
// added an original regrind % to structure.
//
// P.Smith                                  29-03-2005
// added m_fDeadbandForTargetWeight
//
// P.Smith                                  23-08-2005
// added m_nOperator_No,m_nOrder_No,m_nRecipe_No,m_nExtruder_No
//
//
// P.Smith                      2/2/06
// first pass at netburner hardware conversion.
// BooL -> BOOL
//#include <basictypes.h>
//
// P.Smith                      19/6/06
/// make all ints unsigned.
//
// P.Smith                      22/7/09
// added RECIPE_DESCRIPTION_STRING_LENGTH
//
// M.McKiernan                              5/2/2010
// In structSetpointData, replaced;  - see New 5.Feb.2010
//			float       m_fSpare2;   by
//    WORD		m_nResetTotalsFlag;
//    WORD		m_nSpareWord1;				//NB size should stay same.
//
// P.Smith                      7/7/10
// added m_fDesiredWidthOffset
//
// P.Smith                      19/7/10
// added m_wUSLayflatOffsetSign
//////////////////////////////////////////////////////////


//*****************************************************************************
// MACROS
//*****************************************************************************
#ifndef __SETPOINTFORMAT_H__
#define __SETPOINTFORMAT_H__

// *****************************************************************************
// INCLUDES
// *****************************************************************************
// #include "General.h"
// #include "ModbusIndices.h"
// #include "Editor.h"
#include <basictypes.h>
#define  RECIPE_DESCRIPTION_STRING_LENGTH      (10)
/// Recipe data
typedef struct {
    unsigned int     m_nFileNumber;
    char        m_arrDescription[20];                   // setpoint file name or description.
    float   m_fPercentage[MAX_COMPONENTS];
    float   m_fPercentageLiquid;
    float   m_fPercentageRegrindOriginal;           // used to store regrind % when high regrind % is used.
    float   m_fPercentageHighRegrind;
    float   m_fPercentageAdditive;
    unsigned int     m_nRegrindComponent;                        // component no. used for regrind.
    float   m_fDensity[MAX_COMPONENTS];
    float       m_fTotalThroughput;
    float       m_fTotalWeightLength;
    float       m_fTotalWeightArea;
    float       m_fLineSpeed;
    float       m_fDesiredWidth;
    float       m_fTrimmedWidth;

    float       m_fFrontRollLength;
    float       m_fBackRollLength;
    float       m_fOrderLength;
    float       m_fOrderWeight;
    unsigned int     m_nFrontSlitReels;
    unsigned int     m_nBackSlitReels;

    BOOL        m_bSheetTube;
    BOOL        m_bTrimFeed;

    char        m_arrOrderNumber[10];
    char        m_arrCustomerCode[10];
    char        m_arrMaterialCode[10];
    float       m_fFilmThickness;

    float       m_fDeadbandForTargetWeight;              // DBXXKGS DS    2        ; SET POINT DATA.
//New 5.Feb.2010
    //float       m_fSpare2;
    WORD		m_nResetTotalsFlag;
    WORD		m_nSpareWord1;
    unsigned int         m_nOperator_No;                         // OPERATORNO              DS      2       ; OPERATOR NO
    unsigned int         m_nOrder_No;                            // ORDERNO DS    3        ;~ORDER NO
    unsigned int         m_nRecipe_No;                           // RECIPENO    DS      2       ; RECIPE NO
    unsigned int         m_nExtruder_No;                         // EXTRUDERNO  DS      2       ; RECIPE NO --review-- should not be cleared on reset
    WORD		m_wUSLayflatOffsetSign;
    float       m_fDesiredWidthOffset;


} structSetpointData;


typedef struct {
    char                 m_arrMaterialDescription[MAX_COMPONENTS][RECIPE_DESCRIPTION_STRING_LENGTH];
} structSetpointDataAdditionalData;


#endif  // __SETPOINTFORMAT_H__
