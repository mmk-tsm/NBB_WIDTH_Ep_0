///////////////////////////////////////////////////////////
// RecipeFormat.h
//
// Declaration of the format of the recipe file data
// structure
//
//
// M.Parks								28-03-2000
// First pass.
// M.McKiernan								17-12-2003
// Added Repro component, and Regrind component to layer data structure, used up old fSpare, so recipe length should stay same.
// M.McKiernan								16-02-2004
// Added extra parameters to recipe data structure, will be incompatible with previous recipes. m_nTrimFeed; m_fTrimmedWidth; m_fFilmThickness
//////////////////////////////////////////////////////////


//*****************************************************************************
// MACROS
//*****************************************************************************
#ifndef __RECIPEFORMAT_H__
#define __RECIPEFORMAT_H__

//*****************************************************************************
// INCLUDES
//*****************************************************************************
#include "General.h"
#include "ModbusIndices.h"
#include "Editor.h"



#define	EDIT_COMPONENT_FIELDS	2	// Make this 1 less than the actual count because we don't display the description
// The plus one is for the space before the component data
#define	EDIT_LAYER_FIELDS		(4 + 1)	// Make this 1 less than the actual count because we don't display the weight/area
#define	EDIT_COMMON_FIELDS		5

// The +1 at th end is to allow for the space btween the component data
// and the layer data
#define	EDIT_LAYER_MAX_FIELDS		(EDIT_LAYER_FIELDS + (MAX_COMPONENTS * EDIT_COMPONENT_FIELDS))

#define	EDIT_TOTAL_MAX_FIELDS		(EDIT_COMMON_FIELDS + (MAX_LAYERS * (EDIT_LAYER_FIELDS + (MAX_COMPONENTS * EDIT_COMPONENT_FIELDS))))


// Blender Component data
typedef struct {
	float	m_fSetpoint;
	float	m_fDensity;
	char	m_cDescription[MAX_ENTRY_LENGTH + 1];
	Bool	m_bRegrind;
	float	m_fSpare;

} structComponentData;


// Layer data
typedef struct {
	float	m_fLayerThroughput;
	float	m_fLayerWeightLength;
	float	m_fLayerWeightArea;
	float	m_fLayerRatio;
	float	m_fLayerDensity;
	int		m_nLayerReproComponent;					// used in fibre process!!
	int		m_nLayerRegrindComponent;
	float	m_fCompensatedLayerWtLen;
	float	m_fCompensatedLayerRatio;
//	float	m_fSpare;
	
	structComponentData	m_ComponentData[MAX_COMPONENTS];

} structLayerData;

// Recipe data
typedef	struct {
	float	m_fTotalThroughput;
	float	m_fTotalWeightLength;
	float	m_fTotalWeightArea;
	float	m_fLineSpeed;
	float	m_fDesiredWidth;

	Bool		m_bSheetTube;
	char		m_arrOrderNumber[10];
	char		m_arrCustomerNumber[10];
	char		m_arrMaterialCode[10];
// new 16.02.2004
	int		m_nTrimFeed;
	float	m_fTrimmedWidth;
	float	m_fFilmThickness;
// endnew
	float	m_fSpare1;				// leave some spares for expansion .
	float	m_fSpare2;
	float	m_fSpare3;
	
	structLayerData	m_LayerData[MAX_LAYERS];

} structRecipeData;


#endif	// __RECIPEFORMAT_H__