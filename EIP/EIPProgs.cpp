//////////////////////////////////////////////////////
// EIPProgs.cpp
//
// Group of functions that move blender data to and from the Ethernet/IP structures.
//
// First Pass
// M.McKiernan                          25.01.2010
// Available functions:
// 		void EIPOneHzProgram(void);
//		void LoadEIPDownloadRecipe(void);
//		void CopyCurrentRecipeToEIPCurrentRecipeDataObj( void );
//		void CopyProductionDataToEIPProductionDataObj( void );
//
// M.McKiernan                          04-02-2010

// Edited EIPOneHzProgram(void) to Check for a recipe download from the ethernet/ip
// see 	if( eips_vs_DownloadRecipeDataObj.Inst.m_nLoadRecipeCommand == 1)
// Edited LoadEIPDownloadRecipe();	- loads into g_EIPRecipe
// Added function CheckEIPPercentagesTotal100(void)
// Added functions InitiateRecipeTransferFromEIP( void ) and HandleEIPControlCommands( void )
//
//
// M.McKiernan                          11-02-2010
// In CopyCurrentRecipeToEIPCurrentRecipeDataObj( void ) - zero unused component %'s.
// All printf's qualified by if( fdTelnet>0 && g_nTSMDebug.m_bEIPDebug )

//
// M. McKiernan				15.02.2010
// In CopyProductionDataToEIPProductionDataObj( void )
//    eips_vs_ProductionDataObj.Inst.m_nSpareParameter1 = (WORD)g_CurrentTime[TIME_SECOND];
//
// P.Smith				23/2/10
// a problem has been seen where component 4 percentage was getting cleared.
// this was due to the wrong index being used in clearing of unused components
//
// P.Smith				10/3/10
// added g_nEIPCtr & g_nEIPAssemblyObjectCtr;

///////////////////////////////////////////////////////

//*****************************************************************************
// INCLUDES
//*****************************************************************************

#include "EIPProgs.h"
#include <Stdio.h>
#include <StdLib.h>

#include "InitialiseHardware.h"
#include "SerialStuff.h"
#include "General.h"
#include "ConversionFactors.h"

#include "BatchCalibrationFunctions.h"
#include "BatchMBIndices.h"
#include "ConfigurationFunctions.h"
#include "16R40C.h"
#include    "FiftyHz.h"
#include    "BatVars.h"
#include "CycleMonitor.h"
#include    "Pause.h"
#include "Alarms.h"
#include "Batvars2.h"
#include "Conalg.h"
#include "TSMPeripheralsMBIndices.h"
#include "FgOnehz.h"
#include "TSMPeripheralsMBIndices.h"
#include "Eventlog.h"

#include <basictypes.h>

#include "eips_system.h"
#include "SetpointFormat.h"
#include "BatVars.h"
#include "eips_userobj.h"

#include "MBTogSta.h"
#include "TimeDate.h"

// testonly
//#define EIPPrintfEnabled (FALSE)		//define true for printing

extern EIPS_VS_RECIPE_DATA_STRUCT eips_vs_DownloadRecipeDataObj;
/* EMILY MODIFY END */
// Current or running recipe on blender.
extern EIPS_VS_RECIPE_DATA_STRUCT eips_vs_CurrentRecipeDataObj;


extern EIPS_VS_PRODUCTION_DATA_STRUCT	eips_vs_ProductionDataObj;

extern EIPS_VS_CONTROL_DATA_STRUCT		eips_vs_ControlDataObj;


//extern local_t2o_asm_struct[];

//Batch Related
extern structSetpointData   g_CurrentRecipe,g_CommsRecipe;
extern CalDataStruct    g_CalibrationData;
extern  structSetpointData  g_TempRecipe;
extern  WORD g_nEIPToggleStatusCommandWritten;
structSetpointData  g_EIPRecipe;
unsigned int	g_nEIPCtr;
unsigned int	g_nEIPAssemblyObjectCtr;


//
// Function EIPOneHzProgram() - called in foreground once a second.
uint16 EIPOneHzProgramCounter = 0;
void EIPOneHzProgram(void)
{

	// Check for a recipe download from the ethernet/ip
	if( eips_vs_DownloadRecipeDataObj.Inst.m_nLoadRecipeCommand == 1)
	{
		if( CheckEIPPercentagesTotal100() )
		{
			LoadEIPDownloadRecipe();	// loads into g_EIPRecipe.
			InitiateRecipeTransferFromEIP();
		}

		//CLEAR THE LOAD COMMAND.
		eips_vs_DownloadRecipeDataObj.Inst.m_nLoadRecipeCommand = 0;
	}

	// copy the current recipe to the EIP struct for same.
	CopyCurrentRecipeToEIPCurrentRecipeDataObj();
	// copy production data.
	CopyProductionDataToEIPProductionDataObj();

//testonly
	//TestAsmData();
//	local_t2o_asm_struct[0].data_ptr[0] = 0x08;
	EIPOneHzProgramCounter++;

	//todo - this function will copy data into assembly object.
	local_update_produce_asm();
}

// load Ethernet/IP Recipe
void LoadEIPDownloadRecipe(void)
{
	 int i;

//		eips_vs_DownloadRecipeDataObj.Class.ClassRev = 1;
	 // first fill the EIP recipe with the current recipe data.
		 memcpy(&g_EIPRecipe, &g_CurrentRecipe, sizeof( g_CurrentRecipe ) );

		g_EIPRecipe.m_nFileNumber = eips_vs_DownloadRecipeDataObj.Inst.m_nFileNumber;

		 if(eips_vs_DownloadRecipeDataObj.Inst.m_arrDescription[0]>0 && eips_vs_DownloadRecipeDataObj.Inst.m_arrDescription[0]<20)
		for(i=0; i<20; i++)
		{
			g_EIPRecipe.m_arrDescription[i] = eips_vs_DownloadRecipeDataObj.Inst.m_arrDescription[i+1];

		}
		 g_EIPRecipe.m_arrDescription[19] = '\0';	//ensure description is terminated.

	// component target %'s.
		 g_EIPRecipe.m_fPercentage[0] = eips_vs_DownloadRecipeDataObj.Inst.m_fPercentage1;
		 g_EIPRecipe.m_fPercentage[1] = eips_vs_DownloadRecipeDataObj.Inst.m_fPercentage2;
		 g_EIPRecipe.m_fPercentage[2] = eips_vs_DownloadRecipeDataObj.Inst.m_fPercentage3;
		 g_EIPRecipe.m_fPercentage[3] = eips_vs_DownloadRecipeDataObj.Inst.m_fPercentage4;

		 g_EIPRecipe.m_fPercentage[4] = eips_vs_DownloadRecipeDataObj.Inst.m_fPercentage5;
		 g_EIPRecipe.m_fPercentage[5] = eips_vs_DownloadRecipeDataObj.Inst.m_fPercentage6;
		 g_EIPRecipe.m_fPercentage[6] = eips_vs_DownloadRecipeDataObj.Inst.m_fPercentage7;
		 g_EIPRecipe.m_fPercentage[7] = eips_vs_DownloadRecipeDataObj.Inst.m_fPercentage8;

		 g_EIPRecipe.m_fPercentage[8] = eips_vs_DownloadRecipeDataObj.Inst.m_fPercentage9;
		 g_EIPRecipe.m_fPercentage[9] = eips_vs_DownloadRecipeDataObj.Inst.m_fPercentage10;
		 g_EIPRecipe.m_fPercentage[10] = eips_vs_DownloadRecipeDataObj.Inst.m_fPercentage11;
		 g_EIPRecipe.m_fPercentage[11] = eips_vs_DownloadRecipeDataObj.Inst.m_fPercentage12;

		 g_EIPRecipe.m_nRegrindComponent = eips_vs_DownloadRecipeDataObj.Inst.m_nRegrindComponent;
		 if( g_EIPRecipe.m_nRegrindComponent > g_CalibrationData.m_nComponents )
			 g_EIPRecipe.m_nRegrindComponent = 0;

		 // Cater for units.
		 g_EIPRecipe.m_fTotalThroughput = eips_vs_DownloadRecipeDataObj.Inst.m_fTotalThroughput / g_fWeightConversionFactor;;
		 g_EIPRecipe.m_fTotalWeightLength = eips_vs_DownloadRecipeDataObj.Inst.m_fTotalWeightLength / g_fWeightPerLengthConversionFactor;
		 g_EIPRecipe.m_fTotalWeightArea = eips_vs_DownloadRecipeDataObj.Inst.m_fTotalWeightArea / g_fWeightPerAreaConversionFactor;

		 g_EIPRecipe.m_fLineSpeed = eips_vs_DownloadRecipeDataObj.Inst.m_fLineSpeed / g_fLineSpeedConversionFactor;
		 g_EIPRecipe.m_fDesiredWidth = eips_vs_DownloadRecipeDataObj.Inst.m_fDesiredWidth / g_fWidthConversionFactor;

	//sheet tube flag.
		if(eips_vs_DownloadRecipeDataObj.Inst.m_nSheetTube > 0)
			g_EIPRecipe.m_bSheetTube = TRUE;
		else
			g_EIPRecipe.m_bSheetTube = FALSE;



	// set spare values to 0.
		eips_vs_DownloadRecipeDataObj.Inst.m_nRecipeSpareAttribute1 = 0;
		eips_vs_DownloadRecipeDataObj.Inst.m_nRecipeSpareAttribute2 = 0;

		eips_vs_DownloadRecipeDataObj.Inst.m_fRecipeSpareAttribute3 = 0.0f;
		eips_vs_DownloadRecipeDataObj.Inst.m_fRecipeSpareAttribute4 = 0.0f;

		eips_vs_DownloadRecipeDataObj.Inst.m_nRecipeSpareAttribute5 = 0;
		eips_vs_DownloadRecipeDataObj.Inst.m_nRecipeSpareAttribute6 = 0;
		eips_vs_DownloadRecipeDataObj.Inst.m_nRecipeSpareAttribute7 = 0;


		 g_EIPRecipe.m_nResetTotalsFlag = eips_vs_DownloadRecipeDataObj.Inst.m_nResetTotals;


}
//end of load EIP recipe

void CopyCurrentRecipeToEIPCurrentRecipeDataObj( void )
{
 int i;

	eips_vs_CurrentRecipeDataObj.Class.ClassRev = 1;

	for(i=0; i<20; i++)
	{
		eips_vs_CurrentRecipeDataObj.Inst.m_arrDescription[i]=0;
	}
	if(strlen(g_CurrentRecipe.m_arrDescription) < 20 )
	{
		strcpy( eips_vs_CurrentRecipeDataObj.Inst.m_arrDescription+1, g_CurrentRecipe.m_arrDescription );
		eips_vs_CurrentRecipeDataObj.Inst.m_arrDescription[0]=strlen(g_CurrentRecipe.m_arrDescription);
	}

// component target %'s.
	eips_vs_CurrentRecipeDataObj.Inst.m_fPercentage1 = g_CurrentRecipe.m_fPercentage[0];
//testonly
	//if(fdTelnet>0)
//		printf("\n CRDO 1%% = %4.2f CR 1%% = %4.2f", eips_vs_CurrentRecipeDataObj.Inst.m_fPercentage1, g_CurrentRecipe.m_fPercentage[0] );

	// zero unused component %'s.
	if( g_CalibrationData.m_nComponents < MAX_COMPONENTS )
	{
		for(i=g_CalibrationData.m_nComponents; i<MAX_COMPONENTS; i++)
			g_CurrentRecipe.m_fPercentage[i] = 0.0f;
	}
	eips_vs_CurrentRecipeDataObj.Inst.m_fPercentage2 = g_CurrentRecipe.m_fPercentage[1];
	eips_vs_CurrentRecipeDataObj.Inst.m_fPercentage3 = g_CurrentRecipe.m_fPercentage[2];
	eips_vs_CurrentRecipeDataObj.Inst.m_fPercentage4 = g_CurrentRecipe.m_fPercentage[3];

	eips_vs_CurrentRecipeDataObj.Inst.m_fPercentage5 = g_CurrentRecipe.m_fPercentage[4];
	eips_vs_CurrentRecipeDataObj.Inst.m_fPercentage6 = g_CurrentRecipe.m_fPercentage[5];
	eips_vs_CurrentRecipeDataObj.Inst.m_fPercentage7 = g_CurrentRecipe.m_fPercentage[6];
	eips_vs_CurrentRecipeDataObj.Inst.m_fPercentage8 = g_CurrentRecipe.m_fPercentage[7];

	eips_vs_CurrentRecipeDataObj.Inst.m_fPercentage9 = g_CurrentRecipe.m_fPercentage[8];
	eips_vs_CurrentRecipeDataObj.Inst.m_fPercentage10 = g_CurrentRecipe.m_fPercentage[9];
	eips_vs_CurrentRecipeDataObj.Inst.m_fPercentage11 = g_CurrentRecipe.m_fPercentage[10];
	eips_vs_CurrentRecipeDataObj.Inst.m_fPercentage12 = g_CurrentRecipe.m_fPercentage[11];

	eips_vs_CurrentRecipeDataObj.Inst.m_nRegrindComponent = g_CurrentRecipe.m_nRegrindComponent;

// Cater for units.

	eips_vs_CurrentRecipeDataObj.Inst.m_fTotalThroughput = g_CurrentRecipe.m_fTotalThroughput * g_fWeightConversionFactor;
	eips_vs_CurrentRecipeDataObj.Inst.m_fTotalWeightLength = g_CurrentRecipe.m_fTotalWeightLength * g_fWeightPerLengthConversionFactor;
	eips_vs_CurrentRecipeDataObj.Inst.m_fTotalWeightArea = g_CurrentRecipe.m_fTotalWeightArea * g_fWeightPerAreaConversionFactor;

	eips_vs_CurrentRecipeDataObj.Inst.m_fLineSpeed = g_CurrentRecipe.m_fLineSpeed * g_fLineSpeedConversionFactor;
	eips_vs_CurrentRecipeDataObj.Inst.m_fDesiredWidth = g_CurrentRecipe.m_fDesiredWidth * g_fWidthConversionFactor;

//sheet tube flag.
	if(g_CurrentRecipe.m_bSheetTube)
		eips_vs_CurrentRecipeDataObj.Inst.m_nSheetTube = 1;
	else
		eips_vs_CurrentRecipeDataObj.Inst.m_nSheetTube = 0;

	eips_vs_CurrentRecipeDataObj.Inst.m_nFileNumber = g_CurrentRecipe.m_nFileNumber;

	eips_vs_CurrentRecipeDataObj.Inst.m_nResetTotals = 0;				// default - dont clear
	eips_vs_CurrentRecipeDataObj.Inst.m_nLoadRecipeCommand = 0;

// set spare values to 0.
	eips_vs_CurrentRecipeDataObj.Inst.m_nRecipeSpareAttribute1 = 0;
	eips_vs_CurrentRecipeDataObj.Inst.m_nRecipeSpareAttribute2 = 0;

	eips_vs_CurrentRecipeDataObj.Inst.m_fRecipeSpareAttribute3 = 0.0f;
	eips_vs_CurrentRecipeDataObj.Inst.m_fRecipeSpareAttribute4 = 0.0f;

}

// Copies the current recipe into the Ethernet/IP download recipe object.
// Could call at start up for example.
void CopyCurrentRecipeToEIPDownloadRecipeDataObj( void )
{
 int i;
// float fMult;

	eips_vs_DownloadRecipeDataObj.Class.ClassRev = 1;

	for(i=0; i<20; i++)
	{
		eips_vs_DownloadRecipeDataObj.Inst.m_arrDescription[i]=0;
	}
	if(strlen(g_CurrentRecipe.m_arrDescription) < 20 )	// allow 19 max.
	{
		strcpy( eips_vs_DownloadRecipeDataObj.Inst.m_arrDescription+1, g_CurrentRecipe.m_arrDescription );
		eips_vs_DownloadRecipeDataObj.Inst.m_arrDescription[0] = strlen(g_CurrentRecipe.m_arrDescription);
	}

// component target %'s.
	eips_vs_DownloadRecipeDataObj.Inst.m_fPercentage1 = g_CurrentRecipe.m_fPercentage[0];
//testonly
	//if(fdTelnet>0)
//		printf("\n CRDO 1%% = %4.2f CR 1%% = %4.2f", eips_vs_CurrentRecipeDataObj.Inst.m_fPercentage1, g_CurrentRecipe.m_fPercentage[0] );
	eips_vs_DownloadRecipeDataObj.Inst.m_fPercentage2 = g_CurrentRecipe.m_fPercentage[1];
	eips_vs_DownloadRecipeDataObj.Inst.m_fPercentage3 = g_CurrentRecipe.m_fPercentage[2];
	eips_vs_CurrentRecipeDataObj.Inst.m_fPercentage4 = g_CurrentRecipe.m_fPercentage[3];

	eips_vs_DownloadRecipeDataObj.Inst.m_fPercentage5 = g_CurrentRecipe.m_fPercentage[4];
	eips_vs_DownloadRecipeDataObj.Inst.m_fPercentage6 = g_CurrentRecipe.m_fPercentage[5];
	eips_vs_DownloadRecipeDataObj.Inst.m_fPercentage7 = g_CurrentRecipe.m_fPercentage[6];
	eips_vs_DownloadRecipeDataObj.Inst.m_fPercentage8 = g_CurrentRecipe.m_fPercentage[7];

	eips_vs_DownloadRecipeDataObj.Inst.m_fPercentage9 = g_CurrentRecipe.m_fPercentage[8];
	eips_vs_DownloadRecipeDataObj.Inst.m_fPercentage10 = g_CurrentRecipe.m_fPercentage[9];
	eips_vs_DownloadRecipeDataObj.Inst.m_fPercentage11 = g_CurrentRecipe.m_fPercentage[10];
	eips_vs_DownloadRecipeDataObj.Inst.m_fPercentage12 = g_CurrentRecipe.m_fPercentage[11];

	eips_vs_DownloadRecipeDataObj.Inst.m_nRegrindComponent = g_CurrentRecipe.m_nRegrindComponent;

// Remember to cater for units.
	eips_vs_DownloadRecipeDataObj.Inst.m_fTotalThroughput = g_CurrentRecipe.m_fTotalThroughput * g_fWeightConversionFactor;
	eips_vs_DownloadRecipeDataObj.Inst.m_fTotalWeightLength = g_CurrentRecipe.m_fTotalWeightLength * g_fWeightPerLengthConversionFactor;
	eips_vs_DownloadRecipeDataObj.Inst.m_fTotalWeightArea = g_CurrentRecipe.m_fTotalWeightArea * g_fWeightPerAreaConversionFactor;

	eips_vs_DownloadRecipeDataObj.Inst.m_fLineSpeed = g_CurrentRecipe.m_fLineSpeed * g_fLineSpeedConversionFactor;
	eips_vs_DownloadRecipeDataObj.Inst.m_fDesiredWidth = g_CurrentRecipe.m_fDesiredWidth * g_fWidthConversionFactor;
//sheet tube flag.
	if(g_CurrentRecipe.m_bSheetTube)
		eips_vs_DownloadRecipeDataObj.Inst.m_nSheetTube = 1;
	else
		eips_vs_DownloadRecipeDataObj.Inst.m_nSheetTube = 0;

	eips_vs_DownloadRecipeDataObj.Inst.m_nFileNumber = g_CurrentRecipe.m_nFileNumber;

	eips_vs_DownloadRecipeDataObj.Inst.m_nResetTotals = 0;				// default - dont clear
	eips_vs_DownloadRecipeDataObj.Inst.m_nLoadRecipeCommand = 0;

// set spare values to 0.
	eips_vs_DownloadRecipeDataObj.Inst.m_nRecipeSpareAttribute1 = 0;
	eips_vs_DownloadRecipeDataObj.Inst.m_nRecipeSpareAttribute2 = 0;

	eips_vs_DownloadRecipeDataObj.Inst.m_fRecipeSpareAttribute3 = 0.0f;
	eips_vs_DownloadRecipeDataObj.Inst.m_fRecipeSpareAttribute4 = 0.0f;

}
/*
// Conversion factors for each of the dimensions.
 * // Different measurement bases supported
#define	UNIT_METRIC				0
#define	UNIT_IMPERIAL			1
#define	UNIT_IMPERIAL_MODBUS    2
 *
// These are applied to the metric value as a multiplier.
#define	WIDTH_CONVERSION_FACTOR				0.03937f
#define	GAUGE_CONVERSION_FACTOR				0.03937f
#define	LENGTH_CONVERSION_FACTOR			3.2808f
#define	WEIGHT_CONVERSION_FACTOR			2.2046f
#define	THROUGHPUT_CONVERSION_FACTOR		2.2046f
#define	WEIGHT_PER_LENGTH_CONVERSION_FACTOR	0.6719f
#define	WEIGHT_PER_AREA_CONVERSION_FACTOR	1.0f		// No idea what this should be
#define	LINE_SPEED_CONVERSION_FACTOR		3.2808f
#define	DENSITY_CONVERSION_FACTOR			62.43f
*/

void CopyProductionDataToEIPProductionDataObj( void )
{
	//
    eips_vs_ProductionDataObj.Inst.m_fActualThroughput =  g_fRRThroughputKgPerHour * g_fWeightConversionFactor;				//ATTR01

    eips_vs_ProductionDataObj.Inst.m_fActualWPL =  g_fWeightPerMeterRR * g_fWeightPerLengthConversionFactor;						//ATTR02 wt/length
    eips_vs_ProductionDataObj.Inst.m_fActualWPA =  g_fActualGrammesPerM2 * WEIGHT_PER_AREA_CONVERSION_FACTOR;						//ATTR03 wt/area.
    eips_vs_ProductionDataObj.Inst.m_fActualOrderWeight =  g_fTotalOrderWeightAccumulator * g_fWeightConversionFactor;				//ATTR04
    eips_vs_ProductionDataObj.Inst.m_fActualLineSpeed =  g_fLineSpeed5sec * g_fLengthConversionFactor;					//ATTR05
    eips_vs_ProductionDataObj.Inst.m_nOperationalStatus =  g_arrnMBTable[BATCH_SUMMARY_OPERATIONAL_STATUS];				//ATTR06
    eips_vs_ProductionDataObj.Inst.m_nMotorDA =  g_nExtruderDAValue ;							//ATTR07
    eips_vs_ProductionDataObj.Inst.m_nOperationalStatus2 =  g_arrnMBTable[BATCH_SUMMARY_OPERATIONAL_STATUS_2];				//ATTR08
 	eips_vs_ProductionDataObj.Inst.m_nBatchCounter =  g_nCycleCounter;					//Attr09
 	eips_vs_ProductionDataObj.Inst.m_nProcessAlarms =  g_arrnMBTable[BATCH_SUMMARY_PROCESS_ALARMS];					//Attr10

    eips_vs_ProductionDataObj.Inst.m_fActualPercentage1 =  g_fComponentActualPercentage[0];				//ATTR11
    eips_vs_ProductionDataObj.Inst.m_fActualPercentage2 =  g_fComponentActualPercentage[1];				//ATTR12
    eips_vs_ProductionDataObj.Inst.m_fActualPercentage3 =  g_fComponentActualPercentage[2];				//ATTR13
    eips_vs_ProductionDataObj.Inst.m_fActualPercentage4 =  g_fComponentActualPercentage[3];				//ATTR14
    eips_vs_ProductionDataObj.Inst.m_fActualPercentage5 =  g_fComponentActualPercentage[4];				//ATTR15
    eips_vs_ProductionDataObj.Inst.m_fActualPercentage6 =  g_fComponentActualPercentage[5];				//ATTR16
    eips_vs_ProductionDataObj.Inst.m_fActualPercentage7 =  g_fComponentActualPercentage[6];  			//ATTR17
    eips_vs_ProductionDataObj.Inst.m_fActualPercentage8 =  g_fComponentActualPercentage[7];				//ATTR18
    eips_vs_ProductionDataObj.Inst.m_fActualPercentage9 =  g_fComponentActualPercentage[8];				//ATTR19
    eips_vs_ProductionDataObj.Inst.m_fActualPercentage10 =  g_fComponentActualPercentage[9];				//ATTR20
    eips_vs_ProductionDataObj.Inst.m_fActualPercentage11 =  g_fComponentActualPercentage[10];				//ATTR21
    eips_vs_ProductionDataObj.Inst.m_fActualPercentage12 =  g_fComponentActualPercentage[11];				//ATTR22

    eips_vs_ProductionDataObj.Inst.m_fActualWeight1 =  g_fComponentActualWeight[0] * g_fWeightConversionFactor;					//ATTR23
    eips_vs_ProductionDataObj.Inst.m_fActualWeight2 =  g_fComponentActualWeight[1] * g_fWeightConversionFactor;					//ATTR24
    eips_vs_ProductionDataObj.Inst.m_fActualWeight3 =  g_fComponentActualWeight[2] * g_fWeightConversionFactor;					//ATTR25
    eips_vs_ProductionDataObj.Inst.m_fActualWeight4 =  g_fComponentActualWeight[3] * g_fWeightConversionFactor;					//ATTR26
    eips_vs_ProductionDataObj.Inst.m_fActualWeight5 =  g_fComponentActualWeight[4] * g_fWeightConversionFactor;					//ATTR27
    eips_vs_ProductionDataObj.Inst.m_fActualWeight6 =  g_fComponentActualWeight[5] * g_fWeightConversionFactor;					//ATTR28
    eips_vs_ProductionDataObj.Inst.m_fActualWeight7 =  g_fComponentActualWeight[6] * g_fWeightConversionFactor;					//ATTR29
    eips_vs_ProductionDataObj.Inst.m_fActualWeight8 =  g_fComponentActualWeight[7] * g_fWeightConversionFactor;					//ATTR30
    eips_vs_ProductionDataObj.Inst.m_fActualWeight9 =  g_fComponentActualWeight[8] * g_fWeightConversionFactor;					//ATTR31
    eips_vs_ProductionDataObj.Inst.m_fActualWeight10 =  g_fComponentActualWeight[9] * g_fWeightConversionFactor;					//ATTR32
    eips_vs_ProductionDataObj.Inst.m_fActualWeight11 =  g_fComponentActualWeight[10] * g_fWeightConversionFactor;					//ATTR33
    eips_vs_ProductionDataObj.Inst.m_fActualWeight12 =  g_fComponentActualWeight[11] * g_fWeightConversionFactor;					//ATTR34

    eips_vs_ProductionDataObj.Inst.m_fActualOrderWeight1 =  g_fComponentOrderWeightAccumulator[0] * g_fWeightConversionFactor;				//ATTR35
    eips_vs_ProductionDataObj.Inst.m_fActualOrderWeight2 =  g_fComponentOrderWeightAccumulator[1] * g_fWeightConversionFactor;				//ATTR36
    eips_vs_ProductionDataObj.Inst.m_fActualOrderWeight3 =  g_fComponentOrderWeightAccumulator[2] * g_fWeightConversionFactor;				//ATTR37
    eips_vs_ProductionDataObj.Inst.m_fActualOrderWeight4 =  g_fComponentOrderWeightAccumulator[3] * g_fWeightConversionFactor;				//ATTR38
    eips_vs_ProductionDataObj.Inst.m_fActualOrderWeight5 =  g_fComponentOrderWeightAccumulator[4] * g_fWeightConversionFactor;				//ATTR39
    eips_vs_ProductionDataObj.Inst.m_fActualOrderWeight6 =  g_fComponentOrderWeightAccumulator[5] * g_fWeightConversionFactor;				//ATTR40
    eips_vs_ProductionDataObj.Inst.m_fActualOrderWeight7 =  g_fComponentOrderWeightAccumulator[6] * g_fWeightConversionFactor;				//ATTR41
    eips_vs_ProductionDataObj.Inst.m_fActualOrderWeight8 =  g_fComponentOrderWeightAccumulator[7] * g_fWeightConversionFactor;				//ATTR42
    eips_vs_ProductionDataObj.Inst.m_fActualOrderWeight9 =  g_fComponentOrderWeightAccumulator[8] * g_fWeightConversionFactor;				//ATTR43
    eips_vs_ProductionDataObj.Inst.m_fActualOrderWeight10 =  g_fComponentOrderWeightAccumulator[9] * g_fWeightConversionFactor;				//ATTR44
    eips_vs_ProductionDataObj.Inst.m_fActualOrderWeight11 =  g_fComponentOrderWeightAccumulator[10] * g_fWeightConversionFactor;				//ATTR45
    eips_vs_ProductionDataObj.Inst.m_fActualOrderWeight12 =  g_fComponentOrderWeightAccumulator[11] * g_fWeightConversionFactor;				//ATTR46

    eips_vs_ProductionDataObj.Inst.m_nSpareParameter1 = (WORD)g_CurrentTime[TIME_SECOND];

}
//
// Function checks that the recipe %'s downloaded from Ethernet/IP total 100%.
// Returns TRUE if ok, i.e. 100%
bool CheckEIPPercentagesTotal100(void)
{
	float fTotal = 0.0f;
	float fPct[MAX_COMPONENTS];
	WORD nRegrindComp;
	int i;
	bool bGood = TRUE;

	 fPct[0] = eips_vs_DownloadRecipeDataObj.Inst.m_fPercentage1;
     fPct[1] = eips_vs_DownloadRecipeDataObj.Inst.m_fPercentage2;
	 fPct[2] = eips_vs_DownloadRecipeDataObj.Inst.m_fPercentage3;
	 fPct[3] = eips_vs_DownloadRecipeDataObj.Inst.m_fPercentage4;

	 fPct[4] = eips_vs_DownloadRecipeDataObj.Inst.m_fPercentage5;
	 fPct[5] = eips_vs_DownloadRecipeDataObj.Inst.m_fPercentage6;
	 fPct[6] = eips_vs_DownloadRecipeDataObj.Inst.m_fPercentage7;
	 fPct[7] = eips_vs_DownloadRecipeDataObj.Inst.m_fPercentage8;

	 fPct[8] = eips_vs_DownloadRecipeDataObj.Inst.m_fPercentage9;
	 fPct[9] = eips_vs_DownloadRecipeDataObj.Inst.m_fPercentage10;
	 fPct[10] = eips_vs_DownloadRecipeDataObj.Inst.m_fPercentage11;
	 fPct[11] = eips_vs_DownloadRecipeDataObj.Inst.m_fPercentage12;


	nRegrindComp = eips_vs_DownloadRecipeDataObj.Inst.m_nRegrindComponent;      // the regrind component.

    if(nRegrindComp >  g_CalibrationData.m_nComponents)
    {
          nRegrindComp = 0;
    }

		//totalise if not regrind component & valid component.
    for(i=0; i<MAX_COMPONENTS && i<g_CalibrationData.m_nComponents; i++)
    {
		if( i+1 != nRegrindComp )		// nRegrindComp is 1 based (i.e. 1 if component #1)
		{
			fTotal += fPct[i];
		}
    }

    if(fTotal > 99.95 && fTotal < 100.05) // %'s to 2 decimal places, therefore 100% = 10,000. accept .01 error.
    {           //  i.e. total = 100%
        // Remove % alarm from alarm table (if already there)
        if( g_arrnMBTable[BATCH_SUMMARY_PROCESS_ALARMS] & PERALBIT ) // percentage alarm bit set???
        {  //
            RemoveAlarmTable( PERALARM,  0 );       // indicate alarm cleared, Component no = 0 i.e. not component alarm.
        }
    }
    else
    {
        bGood = FALSE;
        // Take action??
        // Put % alarm in alarm table (if not already there)
        if( (g_arrnMBTable[BATCH_SUMMARY_PROCESS_ALARMS] & PERALBIT) == 0 ) // percentage alarm bit not set???
        {  //
            PutAlarmTable( PERALARM,  0 );      // indicate alarm, Component no = 0 i.e. not component alarm.
        }

    }  // end of 100% check found in error.


    return(bGood);

}

//
//todo - put this stuff in proper places
bool g_bNewEIPRecipeFlag = FALSE;		//batvars??

void InitiateRecipeTransferFromEIP( void )
{

// recipe no. set to 1.

//                   SetupMttty();
//                   iprintf("\n transfer to current order");
                   g_EIPRecipe.m_nFileNumber = RUNNINGRECIPEFILENO;        // Set the downloaded file number = 1 (can be changed by operator and saved to any file)
                   g_bNewEIPRecipeFlag = TRUE;                   // set flag to indicate new setpoint data from Comms.

                    g_nTransferSetpointsFlag = TRANSFERSPFROM_EIP;   // set flag to cause setpoints to get loaded at end of cycle.

                    memcpy(&g_TempRecipe, &g_EIPRecipe, sizeof( g_EIPRecipe ) );
//nbb--todo--put back      memcpy(&g_TempRecipeAdditionalData, &g_CommsRecipeAdditionalData, sizeof( g_CommsRecipeAdditionalData ) );

                    g_nSaveRecipeSequence = SAVE_RECIPE_START;
                    ForegroundSaveRecipe( );    // save recipe while running in foreground (1 byte per pass).

}

// Function to deal with Control (object 0x67) commands written down on Ethernet/IP
// This program should called in the foreground(ProgLoop) to check for commands written.
// How it works:
// When data is written to Object 0x67, the appropriate bits are set in g_nEIPToggleStatusCommandWritten
// HandleEIPControlCommands() is a function called repeatedly in Foreground, it checks to see
// if g_nEIPToggleStatusCommandWritten has been written.  If so, it moves the data from Ethernet/IP object 0x67
// into the Modbus table, sets appropriate flags & calls MBToggleStatusCommands() the standard modbus function
// for handling toggle status commands written to Modbus.
void HandleEIPControlCommands( void )
{
	if(g_nEIPToggleStatusCommandWritten )
	{
		if( g_nEIPToggleStatusCommandWritten & EIP_INCREASE_WROTE )
		{
			 // COPY TO MODBUS TABLE
			g_arrnMBTable[BATCH_DTOA_INCREASE_COMMAND] = eips_vs_ControlDataObj.Inst.m_nSetIncrease;
			if( fdTelnet>0 && g_nTSMDebug.m_bEIPDebug )
				printf("\n SetIncrease Written - %04x", eips_vs_ControlDataObj.Inst.m_nSetIncrease );

		}
		if( g_nEIPToggleStatusCommandWritten & EIP_DECREASE_WROTE )
		{
        	 // COPY TO MODBUS TABLE
			 g_arrnMBTable[BATCH_DTOA_DECREASE_COMMAND] = eips_vs_ControlDataObj.Inst.m_nSetDecrease;
				if( fdTelnet>0 && g_nTSMDebug.m_bEIPDebug )
					printf("\n SetDecrease Written - %04x", eips_vs_ControlDataObj.Inst.m_nSetDecrease );
		}
		if( g_nEIPToggleStatusCommandWritten & EIP_AUTO_MANUAL_WROTE )
		{
        	 // COPY TO MODBUS TABLE
			 g_arrnMBTable[BATCH_CONTROL_AUTO_MANUAL] = eips_vs_ControlDataObj.Inst.m_nSetAutoManual;

			if( fdTelnet>0 && g_nTSMDebug.m_bEIPDebug )
				printf("\n SetAutoManual Written - %04x", eips_vs_ControlDataObj.Inst.m_nSetAutoManual );

		}
		if( g_nEIPToggleStatusCommandWritten & EIP_EXTRUDER_DAC_WROTE )
		{
			// COPY TO MODBUS TABLE
			 g_arrnMBTable[BATCH_CONTROL_WRITE_DAC] = eips_vs_ControlDataObj.Inst.m_nSetExtruderDAC;
			 g_bModbusDToAWritten = TRUE;
			if( fdTelnet>0 && g_nTSMDebug.m_bEIPDebug )
					printf("\n SetExtruderDAC Written - %04x", eips_vs_ControlDataObj.Inst.m_nSetExtruderDAC );

		}
		if( g_nEIPToggleStatusCommandWritten & EIP_SCREW_SPEED_WROTE )
		{
       	 // COPY TO MODBUS TABLE
			 g_arrnMBTable[BATCH_CONTROL_SCREW_SPEED] = (WORD)(eips_vs_ControlDataObj.Inst.m_fSetCommsScrewSpeed * 100);	//screw rpm is to 2dp in MB
			 g_bModbusScrewSpeedWritten = TRUE;
			if( fdTelnet>0 && g_nTSMDebug.m_bEIPDebug )
					printf("\n SetCommsScrewSpeed Written - %6.2f .. g_arrnMBTable[BATCH_CONTROL_SCREW_SPEED]= %d", eips_vs_ControlDataObj.Inst.m_fSetCommsScrewSpeed, g_arrnMBTable[BATCH_CONTROL_SCREW_SPEED] );

		}
		if( g_nEIPToggleStatusCommandWritten & EIP_LINE_SPEED_WROTE )
		{
       	 // COPY TO MODBUS TABLE
			 g_arrnMBTable[BATCH_CONTROL_LINE_SPEED] = (WORD)(eips_vs_ControlDataObj.Inst.m_fSetCommsLineSpeed * 100);	// line speed is to 2dp in MB.
			 g_bModbusLineSpeedWritten = TRUE;
			 if( fdTelnet>0 && g_nTSMDebug.m_bEIPDebug )
				 printf("\n SetCommsLineSpeed Written - %6.2f g_arrnMBTable[BATCH_CONTROL_LINE_SPEED]= %d", eips_vs_ControlDataObj.Inst.m_fSetCommsLineSpeed, g_arrnMBTable[BATCH_CONTROL_LINE_SPEED] );
		}
		if( g_nEIPToggleStatusCommandWritten & EIP_START_CLEAN_WROTE )
		{
			// COPY TO MODBUS TABLE
			 g_arrnMBTable[BATCH_CONTROL_CLEAN] = eips_vs_ControlDataObj.Inst.m_nStartClean;
			 if( fdTelnet>0 && g_nTSMDebug.m_bEIPDebug )
				 printf("\n StartClean Written - %04x", eips_vs_ControlDataObj.Inst.m_nStartClean );

		}

		g_nEIPToggleStatusCommandWritten = 0x0000;		// clear flag for next pass.
		MBToggleStatusCommands();						// Call standard modbus function to deal with data in modbus table.
	}
}

#if 0
#define EIP_INCREASE_WROTE			(0x0001)
#define EIP_DECREASE_WROTE			(0x0002)
#define EIP_AUTO_MANUAL_WROTE		(0x0004)
#define EIP_EXTRUDER_DAC_WROTE		(0x0008)
#define EIP_SCREW_SPEED_WROTE		(0x0010)
#define EIP_LINE_SPEED_WROTE		(0x0020)
#define EIP_START_CLEAN_WROTE		(0x0040)

// Function copies the production data into the Ethernet/IP structure.
void CopyProductionDataToEIPProductionDataObj( void )
{
	//TODO
    eips_vs_ProductionDataObj.Inst.m_fActualThroughput =  x;				//ATTR01
    eips_vs_ProductionDataObj.Inst.m_fActualWPL =  x;						//ATTR02 wt/length
    eips_vs_ProductionDataObj.Inst.m_fActualWPA =  x;						//ATTR03 wt/area.
    eips_vs_ProductionDataObj.Inst.m_fActualOrderWeight =  x;				//ATTR04
    eips_vs_ProductionDataObj.Inst.m_fActualLineSpeed =  x;					//ATTR05
    eips_vs_ProductionDataObj.Inst.m_nOperationalStatus =  x;				//ATTR06
    eips_vs_ProductionDataObj.Inst.m_nMotorDA =  x;							//ATTR07
    eips_vs_ProductionDataObj.Inst.m_nOperationalStatus2 =  x;				//ATTR08
 	eips_vs_ProductionDataObj.Inst.m_nBatchCounter =  x;					//Attr09
 	eips_vs_ProductionDataObj.Inst.m_nProcessAlarms =  x;					//Attr10

    eips_vs_ProductionDataObj.Inst.m_fActualPercentage1 =  x;				//ATTR11
    eips_vs_ProductionDataObj.Inst.m_fActualPercentage2 =  x;				//ATTR12
    eips_vs_ProductionDataObj.Inst.m_fActualPercentage3 =  x;				//ATTR13
    eips_vs_ProductionDataObj.Inst.m_fActualPercentage4 =  x;				//ATTR14
    eips_vs_ProductionDataObj.Inst.m_fActualPercentage5 =  x;				//ATTR15
    eips_vs_ProductionDataObj.Inst.m_fActualPercentage6 =  x;				//ATTR16
    eips_vs_ProductionDataObj.Inst.m_fActualPercentage7 =  x;				//ATTR17
    eips_vs_ProductionDataObj.Inst.m_fActualPercentage8 =  x;				//ATTR18
    eips_vs_ProductionDataObj.Inst.m_fActualPercentage9 =  x;				//ATTR19
    eips_vs_ProductionDataObj.Inst.m_fActualPercentage10 =  x;				//ATTR20
    eips_vs_ProductionDataObj.Inst.m_fActualPercentage11 =  x;				//ATTR21
    eips_vs_ProductionDataObj.Inst.m_fActualPercentage12 =  x;				//ATTR22

    eips_vs_ProductionDataObj.Inst.m_fActualWeight1 =  x;					//ATTR23
    eips_vs_ProductionDataObj.Inst.m_fActualWeight2 =  x;					//ATTR24
    eips_vs_ProductionDataObj.Inst.m_fActualWeight3 =  x;					//ATTR25
    eips_vs_ProductionDataObj.Inst.m_fActualWeight4 =  x;					//ATTR26
    eips_vs_ProductionDataObj.Inst.m_fActualWeight5 =  x;					//ATTR27
    eips_vs_ProductionDataObj.Inst.m_fActualWeight6 =  x;					//ATTR28
    eips_vs_ProductionDataObj.Inst.m_fActualWeight7 =  x;					//ATTR29
    eips_vs_ProductionDataObj.Inst.m_fActualWeight8 =  x;					//ATTR30
    eips_vs_ProductionDataObj.Inst.m_fActualWeight9 =  x;					//ATTR31
    eips_vs_ProductionDataObj.Inst.m_fActualWeight10 =  x;					//ATTR32
    eips_vs_ProductionDataObj.Inst.m_fActualWeight11 =  x;					//ATTR33
    eips_vs_ProductionDataObj.Inst.m_fActualWeight12 =  x;					//ATTR34

    eips_vs_ProductionDataObj.Inst.m_fActualOrderWeight1 =  x;				//ATTR35
    eips_vs_ProductionDataObj.Inst.m_fActualOrderWeight2 =  x;				//ATTR36
    eips_vs_ProductionDataObj.Inst.m_fActualOrderWeight3 =  x;				//ATTR37
    eips_vs_ProductionDataObj.Inst.m_fActualOrderWeight4 =  x;				//ATTR38
    eips_vs_ProductionDataObj.Inst.m_fActualOrderWeight5 =  x;				//ATTR39
    eips_vs_ProductionDataObj.Inst.m_fActualOrderWeight6 =  x;				//ATTR40
    eips_vs_ProductionDataObj.Inst.m_fActualOrderWeight7 =  x;				//ATTR41
    eips_vs_ProductionDataObj.Inst.m_fActualOrderWeight8 =  x;				//ATTR42
    eips_vs_ProductionDataObj.Inst.m_fActualOrderWeight9 =  x;				//ATTR43
    eips_vs_ProductionDataObj.Inst.m_fActualOrderWeight10 =  x;				//ATTR44
    eips_vs_ProductionDataObj.Inst.m_fActualOrderWeight11 =  x;				//ATTR45
    eips_vs_ProductionDataObj.Inst.m_fActualOrderWeight12 =  x;				//ATTR46

}
#endif

#if 0
typedef struct
{
    struct
    {
    	uint16 ClassRev;	    // Class Revision  ATTR 01
    }Class;

    struct
    {
        float   m_fActualThroughput;				//ATTR01
        float   m_fActualWPL;						//ATTR02 wt/length
        float   m_fActualWPA;						//ATTR03 wt/area.
        float   m_fActualOrderWeight;				//ATTR04
        float   m_fActualLineSpeed;					//ATTR05

        uint16  m_nOperationalStatus;				//ATTR06
        uint16  m_nMotorDA;							//ATTR07
        uint16  m_nOperationalStatus2;				//ATTR08
        uint16	m_nBatchCounter;					//Attr09
        uint16	m_nProcessAlarms;					//Attr10

        float   m_fActualPercentage1;				//ATTR11
        float   m_fActualPercentage2;				//ATTR12
        float   m_fActualPercentage3;				//ATTR13
        float   m_fActualPercentage4;				//ATTR14
        float   m_fActualPercentage5;				//ATTR15
        float   m_fActualPercentage6;				//ATTR16
        float   m_fActualPercentage7;				//ATTR17
        float   m_fActualPercentage8;				//ATTR18
        float   m_fActualPercentage9;				//ATTR19
        float   m_fActualPercentage10;				//ATTR20
        float   m_fActualPercentage11;				//ATTR21
        float   m_fActualPercentage12;				//ATTR22

        float   m_fActualWeight1;					//ATTR23
        float   m_fActualWeight2;					//ATTR24
        float   m_fActualWeight3;					//ATTR25
        float   m_fActualWeight4;					//ATTR26
        float   m_fActualWeight5;					//ATTR27
        float   m_fActualWeight6;					//ATTR28
        float   m_fActualWeight7;					//ATTR29
        float   m_fActualWeight8;					//ATTR30
        float   m_fActualWeight9;					//ATTR31
        float   m_fActualWeight10;					//ATTR32
        float   m_fActualWeight11;					//ATTR33
        float   m_fActualWeight12;					//ATTR34

        float   m_fActualOrderWeight1;				//ATTR35
        float   m_fActualOrderWeight2;				//ATTR36
        float   m_fActualOrderWeight3;				//ATTR37
        float   m_fActualOrderWeight4;				//ATTR38
        float   m_fActualOrderWeight5;				//ATTR39
        float   m_fActualOrderWeight6;				//ATTR40
        float   m_fActualOrderWeight7;				//ATTR41
        float   m_fActualOrderWeight8;				//ATTR42
        float   m_fActualOrderWeight9;				//ATTR43
        float   m_fActualOrderWeight10;				//ATTR44
        float   m_fActualOrderWeight11;				//ATTR45
        float   m_fActualOrderWeight12;				//ATTR46



            eips_vs_ProductionDataObj.Inst.m_fActualThroughput =  x;				//ATTR01
            eips_vs_ProductionDataObj.Inst.m_fActualWPL =  x;						//ATTR02 wt/length
            eips_vs_ProductionDataObj.Inst.m_fActualWPA =  x;						//ATTR03 wt/area.
            eips_vs_ProductionDataObj.Inst.m_fActualOrderWeight =  x;				//ATTR04
            eips_vs_ProductionDataObj.Inst.m_fActualLineSpeed =  x;					//ATTR05
            eips_vs_ProductionDataObj.Inst.m_nOperationalStatus =  x;				//ATTR06
            eips_vs_ProductionDataObj.Inst.m_nMotorDA =  x;							//ATTR07
            eips_vs_ProductionDataObj.Inst.m_nOperationalStatus2 =  x;				//ATTR08
         	eips_vs_ProductionDataObj.Inst.m_nBatchCounter =  x;					//Attr09
         	eips_vs_ProductionDataObj.Inst.m_nProcessAlarms =  x;					//Attr10

            eips_vs_ProductionDataObj.Inst.m_fActualPercentage1 =  x;				//ATTR11
            eips_vs_ProductionDataObj.Inst.m_fActualPercentage2 =  x;				//ATTR12
            eips_vs_ProductionDataObj.Inst.m_fActualPercentage3 =  x;				//ATTR13
            eips_vs_ProductionDataObj.Inst.m_fActualPercentage4 =  x;				//ATTR14
            eips_vs_ProductionDataObj.Inst.m_fActualPercentage5 =  x;				//ATTR15
            eips_vs_ProductionDataObj.Inst.m_fActualPercentage6 =  x;				//ATTR16
            eips_vs_ProductionDataObj.Inst.m_fActualPercentage7 =  x;				//ATTR17
            eips_vs_ProductionDataObj.Inst.m_fActualPercentage8 =  x;				//ATTR18
            eips_vs_ProductionDataObj.Inst.m_fActualPercentage9 =  x;				//ATTR19
            eips_vs_ProductionDataObj.Inst.m_fActualPercentage10 =  x;				//ATTR20
            eips_vs_ProductionDataObj.Inst.m_fActualPercentage11 =  x;				//ATTR21
            eips_vs_ProductionDataObj.Inst.m_fActualPercentage12 =  x;				//ATTR22

            eips_vs_ProductionDataObj.Inst.m_fActualWeight1 =  x;					//ATTR23
            eips_vs_ProductionDataObj.Inst.m_fActualWeight2 =  x;					//ATTR24
            eips_vs_ProductionDataObj.Inst.m_fActualWeight3 =  x;					//ATTR25
            eips_vs_ProductionDataObj.Inst.m_fActualWeight4 =  x;					//ATTR26
            eips_vs_ProductionDataObj.Inst.m_fActualWeight5 =  x;					//ATTR27
            eips_vs_ProductionDataObj.Inst.m_fActualWeight6 =  x;					//ATTR28
            eips_vs_ProductionDataObj.Inst.m_fActualWeight7 =  x;					//ATTR29
            eips_vs_ProductionDataObj.Inst.m_fActualWeight8 =  x;					//ATTR30
            eips_vs_ProductionDataObj.Inst.m_fActualWeight9 =  x;					//ATTR31
            eips_vs_ProductionDataObj.Inst.m_fActualWeight10 =  x;					//ATTR32
            eips_vs_ProductionDataObj.Inst.m_fActualWeight11 =  x;					//ATTR33
            eips_vs_ProductionDataObj.Inst.m_fActualWeight12 =  x;					//ATTR34

            eips_vs_ProductionDataObj.Inst.m_fActualOrderWeight1 =  x;				//ATTR35
            eips_vs_ProductionDataObj.Inst.m_fActualOrderWeight2 =  x;				//ATTR36
            eips_vs_ProductionDataObj.Inst.m_fActualOrderWeight3 =  x;				//ATTR37
            eips_vs_ProductionDataObj.Inst.m_fActualOrderWeight4 =  x;				//ATTR38
            eips_vs_ProductionDataObj.Inst.m_fActualOrderWeight5 =  x;				//ATTR39
            eips_vs_ProductionDataObj.Inst.m_fActualOrderWeight6 =  x;				//ATTR40
            eips_vs_ProductionDataObj.Inst.m_fActualOrderWeight7 =  x;				//ATTR41
            eips_vs_ProductionDataObj.Inst.m_fActualOrderWeight8 =  x;				//ATTR42
            eips_vs_ProductionDataObj.Inst.m_fActualOrderWeight9 =  x;				//ATTR43
            eips_vs_ProductionDataObj.Inst.m_fActualOrderWeight10 =  x;				//ATTR44
            eips_vs_ProductionDataObj.Inst.m_fActualOrderWeight11 =  x;				//ATTR45
            eips_vs_ProductionDataObj.Inst.m_fActualOrderWeight12 =  x;				//ATTR46

    }Inst;
}EIPS_VS_PRODUCTION_DATA_STRUCT;

#endif
