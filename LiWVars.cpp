/////////////////////////////////////////////////
//
// M. McKiernan                      8/1/08
// create variables file for LiW 
//
// M. McKiernan                      18/2/08
// Added variables for accumulators and control related.
// Initialisation of component structure variables.
// M.McKiernan                         28/3/2008
// Added fdlisten/fdnet for telnet purposes.
// g_sLiWComponent[i].m_fEstimatedKgH = 0.0f;
// Added g_fCurrentTargetKgH, g_bEnterAutoFlag;
// Added g_fLineSpeedUsedForSetpointCalc
// added g_bSaveAllProcessDataToEEprom
// Added RepeatedDataStruct   g_ProcessData;
// Added  variables for simulated batch;   g_nLiWBatchTimer g_nLiWSimulatedBatchCounter 
// Changed m_bFillAlarm to m_nFillAlarm

// M. McKiernan                      2/5/08
// Added .m_fActualPercentage
//                                  28/5/2008
// Added g_bForeground50HzFlag, g_nForeground50HzCounter
// M. McKiernan                      5/6/08
// Added initialisation for variables for Screw Calibration in component structure. & g_bScrewCalibrationFlag
// also m_fNewK1 & g_nMBScrewCalComponent
// Added   .m_fKgHSetpointInManual = 0.0f;
//
// M. McKiernan                      21/7/08
// Added g_nSDCardDelayTimer & .m_bFillValveState = OFF;
//
// M. McKiernan                      22/7/08
// Added initialisation of "fast" throughput variables - m_fFastKgH,  m_bFastKgHValid, 
//     m_fMinExpectedKgH, m_nLessThanMinKgHCounter 
//
// M. McKiernan                      19/2/09
// Added pile of variables relating to Blending only mode. Also temperature.
//
// M. McKiernan                      23/2/09
// Added alarm stuff
// g_bAlarmOccurredHistory[MAXIMUM_ALARMS][MAX_COMPONENTS];                       
// g_bAddAlarmOccurredToEventLog;  g_bAddAlarmClearedToEventLog;
// g_ucAlarmCode  g_nAlarmComponent;
//
// M. McKiernan                      23/2/09
// Offline related variables added.
//
// M. McKiernan                      1/5/09
// added  g_cLiWPollTime  g_nLiWPollDecisionCtr; g_bNetCommsParityChange;
// Added structLiWCardData g_sLiWCard[MAX_LIW_CARDS];
// 
//
// M. McKiernan                      2/6/09
// Added   m_bUseLiWPollA, m_nPollDataUsed to structLiWComponentData
// Added followning to structLiWCardData
// long  m_lADCValue_Ap; m_lADCValue_Bp;  m_nADCValueMSW_Ap; m_nADCValueMSW_Bp;
// m_lADCValue_App;  m_lADCValue_Bpp;    m_nADCValueMSW_App; m_nADCValueMSW_Bpp;
//
// P.Smith                          25/6/09
// removed duplicate definitions
/////////////////////////////////////////////////

//*****************************************************************************
// MACROS
//*****************************************************************************

#include "General.h"
#include <basictypes.h>
#include "LiWVars.h"
#include "BatchCalibrationFunctions.h"
#include "BatVars.h"

structLiWComponentData g_sLiWComponent[MAX_COMPONENTS];
structLiWLSpeed      g_sLiWLineSpeed;
structLiWCardData g_sLiWCard[MAX_LIW_CARDS];


extern CalDataStruct    g_CalibrationData;

// for telnet
int      fdlisten;
int      fdnet;
char     cTelWrBuffer[200];


int         g_nADBufferIndex;    
int         g_nADEntriesSinceFill;
int         g_nLiWCycleSecondNo;
int         g_nCHF10HzCounter;

BOOL        g_bForeground50HzFlag;
DWORD       g_nForeground50HzCounter;

float       g_fADBuffer[ADMAXENTRIES];
int         g_nTimeInLiWCycle[ADMAXENTRIES];  
    
float       g_fSlope1;

float       g_fHopperWeight[MAX_COMPONENTS];

float       g_fMinHopperWeight[MAX_COMPONENTS];
float       g_fMaxHopperWeight[MAX_COMPONENTS];

BOOL        g_bFillOnFlag[MAX_COMPONENTS];
int         g_nFillTimeRemaining[MAX_COMPONENTS];     // tenths of second.
int         g_nAfterFillLagRemaining[MAX_COMPONENTS];     // tenths of second.
int         g_nFillTime10Hz[MAX_COMPONENTS];
int         g_nHopperEmptyCounter[MAX_COMPONENTS];
int         g_nLiW1SWtAvailCounter[MAX_COMPONENTS];

BOOL        g_bLiW1SWeightAverageAvailable;
BOOL        g_bMaxHopperWeightReached[MAX_COMPONENTS];

BOOL        g_bVolumetricChangeFlag;
BOOL        g_bScrewCalibrationFlag;

BOOL        g_bLiWDebug;
BOOL        g_bLiWSimulatorRunning;
long        g_lOneSecondSimulatedADC;

int         g_nAF1;           // todo - AF1 will be a calibration parameter.
int         g_nAF2;           //todo
int         g_nDeltaT;


float    g_fTotalFrontRollWeight;
float    g_fTotalBackRollWeight;
float    g_fTotalOrderWeight;
float    g_fTotalShiftWeight;

int      g_nControlLag;
BOOL     g_bLiWBlenderPaused;
BOOL     g_bCalibrationMode;
int      g_nTimeSinceManualChange;     //MCHGTIMER
int      g_nManualChangeLag;           //MCHGTIM lag, decremented to zero after a manual change.
BOOL     g_bManualChangeOccurred;      // MCHGFLG
BOOL     g_bEnterAutoFlag;             // EATOFG
// used for weight calibration/taring - used to store component no. for which
// new calibrationn data has been written.
int   g_nMBWeightCalibrationComponent;
int   g_nMBTareCommandComponent;
int   g_nMBScrewCalComponent;

float    g_fActualGrammesPer12Ft2;
float    g_fCurrentTargetKgH;
float    g_fLineSpeedUsedForSetpointCalc;    //value of line speed used in calculating kg/h spts
BOOL     g_bNormalManualEnable;
BOOL     g_bHMinFlag;      //FLAG TO IND. MINUTE FOR CALCULATING THRUPUT IN H. O. MODE - HMINFLG

WORD     g_nLiWBatchTimer;
WORD     g_nLiWSimulatedBatchCounter;
WORD     g_nStartUpTimer;
WORD     g_nSDCardDelayTimer;

// relating to Blending Only.
int      g_nHOSeconds;                             //HOFSECS
int      g_nBOManualLag;                           //HMANLAG 
int      g_nNormalManualLag;                       //NMANLAG 

int      g_nHOControlLag;                          // HC_LAG.

float    g_fBOExtruderRPM;                         //HSSRPM
float    g_fFastKgHTotal;
float    g_fBOTotalKgH;                            //HATTKGH
float    g_fHOBOKgH;                               //HPKGH
float    g_fHOBOKgHUsed;                           //HPKGHU

bool     g_bBOPercentageChangeFlag;
bool     g_bFastKgHValid;                          //FTVALID
bool     g_fFastKgHTotalValidity;                  //FTTVLDTY

float    g_fTotalKgHSetpoint;    //KGPHSPT
bool     g_bShortDurationSpeedFluctuationMode;     // SDSFMODE
bool     g_bShortDurationSpeedFluctuationFlag;     // SDSFFLAG
float    g_fSDSF_KgH;                              // SDSFKGH 
float    g_fSDSF_RPM;                              // SDSFRPM
float    g_fBOExtruderRPMOld;                      // SSRPMO
int      g_nHODataSaveLag;                         // HADSLAG 
bool     g_bEnterBlendingOnlyAutoFlag;             // EHATOFG
bool     g_bLineStartFlag;                         // LINSTRTFLG
float    g_fPercentScrewChange;                    // not in ASM version.
bool     g_bScrewSpeedChangeFlag;                  // SSCHFLG
bool     g_bHaulOffTimeFlag;                       // HMINFLG
bool     g_bHOThroughputValid;                     // HATVALD
float    g_fScrewCorrectionFactor;                 // SSCFCTR 
float    g_fHaulOffRPM;                            // HPRPM 
float    g_fHaulOffScrewRatio;                     // HPSRATIO
float    g_fBOTotalKgHOld;                         // HATTOLD
bool     g_bPrevBOExtruderRPMWasZero;              // not in ASM version.
bool     g_bHOFastSaveRate;                        //HSVRATE
float    g_fHOMPerMinSetpoint;                     // HMMINSP 
bool     g_bKgHPerRevIsValid;

float    g_fLiquidKgH;                             // LAKGH

int      g_nExtruderRPMGEMinCounter;
int      g_nExtruderRPMLTMinCounter;
bool     g_bExtruderStopped;
float    g_fBOComponent1TargetPercentage;
float    g_fExtruderRPMRatio;
float    g_fExtruderRPMDiff;
int      g_nTimeSinceExtruderRPMChange;
float    g_fPrevExtruderRPM;
float    g_fBOTotalKgH_KPR;         // blending only total kgh based on kgh/rev.
int      g_nExtruderRPMSumCtr;
float    g_fExtruderRPMAvg3;
float    g_fExtruderRPMSum;


// Offline related.
BOOL     g_bOffLinePaused;               //PAUSFLG   ~AT POWER-UP, NOT IN PAUSED MODE
BOOL     g_bOffLineManualAutoFlag;       // PMANATO  ~AT POWER-UP, SAVED STATUS SET TO MANUAL
BOOL     g_bOffLineSetpointChangedFlag; // OFLSCFG ~OFF-LINE SETPT CHANGE FLAG
BOOL     g_bCommsRestartFlag;
BOOL     g_bCommsPauseFlag;
int      g_nLineSpeedMinorChangeCounter;
int      g_nLiWAtQCtr;
int      g_nUpdLiWCtr;
int      g_nDPTAtResponse;
int      g_nDPTAtResponseInUADC;
BOOL     g_bSecondRolloverForPolling;
char     g_cLiWPollTime;
int      g_nLiWPollDecisionCtr;

BOOL     g_bNetCommsParityChange;

void InitialiseLiWVars( void ) 
{
int i;
   fdlisten = 0;
   fdnet = -1;
   g_nSDCardDelayTimer = 0;
   
   g_bLiWDebug = FALSE;           //todo - Debug on for testing.
//   g_bLiWDebug = FALSE;           //todo - REMOVE DEBUG PRINTING - Release version
   //testonly
   g_bLiWSimulatorRunning = TRUE;
   g_bLiWSimulatorRunning = FALSE;     // simulator disabled.
   g_lOneSecondSimulatedADC = 100000;     // start it off at mid-range (100,000)
   
   g_nCHF10HzCounter = 0;
   
   g_bForeground50HzFlag  = FALSE;
   g_nForeground50HzCounter = 0;
       
   g_nADBufferIndex = 0;    
   g_nADEntriesSinceFill = 0; 
   g_nLiWCycleSecondNo = 0; 

   for(i=0; i<ADMAXENTRIES; i++)
      g_fADBuffer[i] = 0.0f;

   for(i=0; i<ADMAXENTRIES; i++)
      g_nTimeInLiWCycle[i] = 0;  
    
   g_fSlope1 = 1.0f;

   for(i=0; i<MAX_COMPONENTS; i++)
   {
      g_bFillOnFlag[i] = FALSE;
      
      g_nFillTimeRemaining[i] = 0;     // tenths of second.
      g_nAfterFillLagRemaining[i]=0;     // tenths of second.
      
      g_bMaxHopperWeightReached[i] = FALSE;
      
      g_fMinHopperWeight[i] = 0.5f;     //  todo....this will be a calibration parameter.
      
      g_fHopperWeight[i] = 2.2f;     //  todo.....
      
      g_fMaxHopperWeight[i] = 3.0f;     //  todo....this will be a calibration parameter.
      
      g_nFillTime10Hz[i] = 40+(10*i);   //  todo....this will be a calibration parameter.
      
      g_nHopperEmptyCounter[i] = 0;
      g_nLiW1SWtAvailCounter[i] = 0;
      
   }   
   g_bLiW1SWeightAverageAvailable = FALSE;
   g_bVolumetricChangeFlag = FALSE;
     
   g_nAF1 = 6;           // todo - AF1 will be a calibration parameter.
   g_nAF2 = 50;          //todo - "     "      "
   g_nDeltaT = 28;       // todo - DT will be a calibration parameter.

   g_fTotalFrontRollWeight = 0.0f;
   g_fTotalBackRollWeight = 0.0f;
   g_fTotalOrderWeight = 0.0f;
   g_fTotalShiftWeight = 0.0f;
 
   //control related.
   g_nControlLag = 60;        // initialise control lag to 60 seconds initially.
   g_bLiWBlenderPaused = FALSE;
   g_bCalibrationMode = FALSE;
   g_nTimeSinceManualChange = 0;    //ToDo....perhaps this should be larger.
   g_bManualChangeOccurred = FALSE; //ToDo....need to review.
   g_nManualChangeLag = 0;          //ToDo....need to review.
   g_bEnterAutoFlag = FALSE;
   g_bScrewCalibrationFlag = FALSE;
      
   g_fActualGrammesPer12Ft2 = 0.0f;
   
   g_fCurrentTargetKgH = 0.0f;
   g_fLineSpeedUsedForSetpointCalc = 0.0f;
   
   g_bNormalManualEnable = FALSE;
   g_bHMinFlag = FALSE;
   
   g_nLiWBatchTimer = 0;         //timer for simulated batch.
   g_nLiWSimulatedBatchCounter = 0; // simulated batch counter.
     
   g_nLiWPollDecisionCtr = 0;
 
   for(i=0; i<MAX_COMPONENTS; i++)  //ToDo - note hard coding calibration data for test purposes
   { 
/*         
      g_CalibrationData.m_fMaxHopperWeight[i] = 3.5f;  //
      g_CalibrationData.m_fMinHopperWeight[i] = 1.0f;  //
      g_CalibrationData.m_fFillTime[i] = 2.2f;        //
      
      g_CalibrationData.m_fCompMaxThroughput[i] = 555.0f;  //
      g_CalibrationData.m_bFillEnable[i] = TRUE;       // 
      
      g_CalibrationData.m_fMotorSetpointPercentage[i] = 0.00f;
      
      
      g_CalibrationData.m_nAF1[i] = 6;             // Averaging Factor #1 (avg for wt)
      g_CalibrationData.m_nAF2[i] = 50;             // Averaging Factor #2 (avg for wt loss)
      g_CalibrationData.m_nDT[i] = 28;              // Delta Time
      g_CalibrationData.m_fLiWControlGain[i] = 0.78f;  //control gain
*/      
   }
   
   g_nHOSeconds = 0;
   g_nBOManualLag = 0;
   g_nNormalManualLag = 0;
   g_nHOControlLag = 0;
   g_fBOExtruderRPM = 0.0;
   g_fFastKgHTotal = 0.0;
   g_fBOTotalKgH = 0.0;
   g_fHOBOKgHUsed = 0.0;
   g_fHOBOKgH = 0.0;
   g_bBOPercentageChangeFlag = FALSE;
   g_bFastKgHValid = FALSE;
   g_fFastKgHTotalValidity = FALSE;
   
   g_fTotalKgHSetpoint = 0.0;
   g_bShortDurationSpeedFluctuationMode = FALSE;
   g_bShortDurationSpeedFluctuationFlag = FALSE;   
   g_fSDSF_KgH = 0.0;                              
   g_fSDSF_RPM = 0.0;  
   g_fBOExtruderRPMOld = 0.0; 
   g_nHODataSaveLag = 0;  
   g_bEnterBlendingOnlyAutoFlag = FALSE;
   g_bLineStartFlag = FALSE;
   g_fPercentScrewChange = 0.0;   
   g_bScrewSpeedChangeFlag = FALSE;
   g_bHaulOffTimeFlag = FALSE;   
   g_bHOThroughputValid = FALSE; 
   g_fScrewCorrectionFactor = 1.0f; 
   g_fHaulOffRPM = 0.0;
   g_fHaulOffScrewRatio = 1.0f;
   g_fBOTotalKgHOld = 0.0f;
   g_bPrevBOExtruderRPMWasZero = FALSE;
   g_bHOFastSaveRate = FALSE;
   g_fHOMPerMinSetpoint = 0.0f;
   g_bKgHPerRevIsValid = FALSE;
   g_fLiquidKgH = 0.0f;   
    
   g_nExtruderRPMGEMinCounter = 0;
   g_nExtruderRPMLTMinCounter = 0;
   g_bExtruderStopped = TRUE;
   g_fBOComponent1TargetPercentage = 100.0f;
   g_fExtruderRPMRatio = 1.0f;         
   g_fExtruderRPMDiff = 0;
   g_nTimeSinceExtruderRPMChange = 0;
   g_fPrevExtruderRPM = 0.0f;
   g_nExtruderRPMSumCtr = 0;
   g_fExtruderRPMAvg3 = 0.0f;
   g_fExtruderRPMSum = 0.0f;

   
   g_fBOTotalKgH_KPR = 0.0f; 

   g_bOffLinePaused = FALSE;             //PAUSFLG   ~AT POWER-UP, NOT IN PAUSED MODE
   g_bOffLineManualAutoFlag = MANUAL;    // PMANATO  ~AT POWER-UP, SAVED STATUS SET TO MANUAL
   g_bOffLineSetpointChangedFlag = FALSE; // OFLSCFG ~OFF-LINE SETPT CHANGE FLAG
   g_bCommsRestartFlag = FALSE;
   g_bCommsPauseFlag = FALSE;
   g_nLineSpeedMinorChangeCounter = 0;
   g_nLiWAtQCtr = 0;
   g_nUpdLiWCtr = 0;
   g_bSecondRolloverForPolling = FALSE;
   g_cLiWPollTime = 'A';
   
   g_bNetCommsParityChange = FALSE;
   
}
   
void InitialiseLiWLineSpeedStructure( void )
{
int i;
   for(i=0; i<MAX_ENTRIES_LSPD_BUF; i++)
	  g_sLiWLineSpeed.m_nPulsesPerSecond[i] = 0;
	  
	g_sLiWLineSpeed.m_nNoOfEntriesLSPDBuf = 0;
	g_sLiWLineSpeed.m_nIndexLSPDBuf = 0;
	g_sLiWLineSpeed.m_nAverageLSPDBuf = 0;
	g_sLiWLineSpeed.m_fLineSpeedAvg = 0.0f;
	g_sLiWLineSpeed.m_bLineSpeedValid = FALSE;
	
} 
// initialise comoponent data structures.
void InitialiseLiWComponentStructures( void )         
{
   int i,j;
   for(i=0; i<MAX_COMPONENTS; i++)
   {
	// general	   
      g_sLiWComponent[i].m_bTareFlag = FALSE;     // taring flag, i.e. set while taring.
      g_sLiWComponent[i].m_bHopperEmptyFlag = FALSE;
      g_sLiWComponent[i].m_nHopperEmptyCounter = 0;
      g_sLiWComponent[i].m_fWeightInHopper = 0.0f;
      g_sLiWComponent[i].m_bMaxHopperWeightReached = FALSE;
      
      g_sLiWComponent[i].m_bFillOnFlag = FALSE;
      g_sLiWComponent[i].m_nFillTimeRemaining = 0;     //  
      g_sLiWComponent[i].m_nAfterFillLagRemaining = 0;     //  
      g_sLiWComponent[i].m_bFillCompleteFlag = FALSE;
      g_sLiWComponent[i].m_nFillAlarm = 0;
      g_sLiWComponent[i].m_bFillValveState = OFF;

      g_sLiWComponent[i].m_bLiW1SWeightAverageAvailable = FALSE;
      
      g_sLiWComponent[i].m_lOneSecondSimulatedADC = 0;      // for simulator.
      g_sLiWComponent[i].m_lOneSecondMeasuredADC = 0;
      g_sLiWComponent[i].m_bUseLiWPollA = TRUE;
      g_sLiWComponent[i].m_nLiWADCError = 0;
      
      g_sLiWComponent[i].m_nADEntriesSinceFill = 0;
      g_sLiWComponent[i].m_lLiWCycleSecondNo = 0;
      
   
	// Raw Data.
      g_sLiWComponent[i].m_nIndexRBuf = 0;
	   for(j=0; j<MAX_ENTRIES_RBUF; j++)
	   {
	     g_sLiWComponent[i].m_lOneSecondADValue[j] = 0;
	   }
	   g_sLiWComponent[i].m_nNoOfEntriesRBuf = 0;
	   g_sLiWComponent[i].m_nIndexRBuf = 0;
	   g_sLiWComponent[i].m_lAverageRBuf = 0;
      g_sLiWComponent[i].m_nLiW1SWtAvailCounter = 0;     // one second wt avail counter.
	   
	// Time Buffer Data
	   for(j=0; j<MAX_ENTRIES_TBUF; j++)
	   {
	     g_sLiWComponent[i].m_lTimeBufData[j] = 0;
	   }
	   g_sLiWComponent[i].m_nNoOfEntriesTBuf = 0;
	   g_sLiWComponent[i].m_nIndexTBuf = 0;
	   g_sLiWComponent[i].m_fCountsPerSecond = 0.0f;
	   g_sLiWComponent[i].m_nCurrentDT = 0;  // Delta Time in use.
      g_sLiWComponent[i].m_bDT5 = FALSE;
      g_sLiWComponent[i].m_fCountsPerSecond_5 = 0.0f;
      	
	// Weight loss Buffer Data
	   for(j=0; j<MAX_ENTRIES_WBUF; j++)
	   {
	     g_sLiWComponent[i].m_fWLBufData[j] = 0.0f;   //data in W.L. buf is counts/sec.
	   }
	   
	   g_sLiWComponent[i].m_nIndexWBuf = 0;
	   g_sLiWComponent[i].m_nNoOfEntriesWBuf = 0;
	   g_sLiWComponent[i].m_nIndexCurrentWBuf = 0;
	   g_sLiWComponent[i].m_fAverageWBuf = 0.0f;
	   g_sLiWComponent[i].m_fSumWBuf = 0.0f;
	   g_sLiWComponent[i].m_nEntriesInSumWBuf = 0;
	   g_sLiWComponent[i].m_bControlFlag = FALSE;
	   g_sLiWComponent[i].m_bNoNewDataFlag = TRUE;    // no new data
   
       //  control related
       g_sLiWComponent[i].m_nDACValue = 0;      // D/A value.     //ToDo  may not want to zero.	   
       g_sLiWComponent[i].m_nDACSetpoint = 0;
       g_sLiWComponent[i].m_bPotentiometerControl = FALSE;
 
	     g_sLiWComponent[i].m_fControlValue = 0.0f;
	     g_sLiWComponent[i].m_fControlSum = 0.0f;
	     g_sLiWComponent[i].m_nControlCount = 0;           // WCCTOF in ASM
	     g_sLiWComponent[i].m_nControlUpdateLag = 0;
	     g_sLiWComponent[i].m_nControlActions = 0;
	     g_sLiWComponent[i].m_bRapidControlResponse = FALSE;

	     g_sLiWComponent[i].m_fControlValue2 = 0.0f;
	     g_sLiWComponent[i].m_bControlValue2Available = FALSE;
	     
       // screw calibration related
       g_sLiWComponent[i].m_bScrewCalibrationInProgress = FALSE;
       g_sLiWComponent[i].m_nScrewCalibrationInProgressTime = 0;
       g_sLiWComponent[i].m_nScrewCalibrationStabilityCounter = 0;
       g_sLiWComponent[i].m_fCurrentKgH = 0.0f;
       g_sLiWComponent[i].m_fPreviousKgH = 0.0f;
             
       // kg/h
       g_sLiWComponent[i].m_fActualKgH = 0.0f;
       g_sLiWComponent[i].m_fTargetKgH = 0.0f;
       g_sLiWComponent[i].m_fEstimatedKgH = 0.0f;
       g_sLiWComponent[i].m_fKgHSetpointInManual = 0.0f;
       g_sLiWComponent[i].m_fActualPercentage = 0.0f;    // component actual %.
       g_sLiWComponent[i].m_fFastKgH = 0.0f;
       g_sLiWComponent[i].m_fFastKgH20 = 0.0f;
       g_sLiWComponent[i].m_fMinExpectedKgH = 0.0f;
       g_sLiWComponent[i].m_nLessThanMinKgHCounter = 0;
       g_sLiWComponent[i].m_bFastKgHValid = FALSE;
       g_sLiWComponent[i].m_bFastKgH20Valid = FALSE;
       g_sLiWComponent[i].m_fFastCountsPer10S = 0.0f;

//K1 related.
//       g_sLiWComponent[i].m_fUsedK1 = g_CalibrationData.m_fK1[i];  //assumes calibration data has been loaded.
//       g_sLiWComponent[i].m_fRevisedK1 = g_sLiWComponent[i].m_fUsedK1;       // set revised = calibration K1.
	     g_sLiWComponent[i].m_nRevisedK1Validity = 0; //
	     g_sLiWComponent[i].m_fNewK1 = 0.0f; 
	     
       g_sLiWComponent[i].m_fMotorSetPercentage = 0.0f;
       
       
       
       // Wt. Accumulators
      g_sLiWComponent[i].m_fFrontRollWeightAccumulator= 0.0f;
      g_sLiWComponent[i].m_fBackRollWeightAccumulator= 0.0f;
      g_sLiWComponent[i].m_fOrderWeightAccumulator= 0.0f;
      g_sLiWComponent[i].m_fShiftWeightAccumulator = 0.0f;
      // simulated batch.
      g_sLiWComponent[i].m_fLiWBatchWeightAccumulator = 0.0f;	 
      g_sLiWComponent[i].m_fLiWBatchWeight = 0.0f;  
      
      g_sLiWComponent[i].m_fBOKgH = 0.0f;
      g_sLiWComponent[i].m_fBOControlValue = 0.0f;

      
   }
   
   
   g_sLiWComponent[0].m_nDACValue = g_nExtruderDAValue;     //g_nExtruderDAValue is value used for C 1.   
   for(i=0; i<MAX_LIW_CARDS; i++)
   {
      g_sLiWCard[i].m_nADCValueMSW_A = 0;    //msb for poll A
      g_sLiWCard[i].m_nADCValueLSW_A = 0;    //lsb for poll A
      g_sLiWCard[i].m_lADCValue_A = 0;       // amalgamated result for poll A.
      g_sLiWCard[i].m_nADCValueMSW_B = 0;    //msb for poll B
      g_sLiWCard[i].m_nADCValueLSW_B = 0;    //lsb for poll B
      g_sLiWCard[i].m_lADCValue_B = 0;       // amalgamated result for poll B.
      g_sLiWCard[i].m_nBEqualsCurrentACtr = 0;  //counter for data from polls A and B being equal 
      g_sLiWCard[i].m_nAEqualsPreviousBCtr = 0; // counter for data from poll A equalling previous B poll.
      g_sLiWCard[i].m_nBEqualsPreviousBCtr = 0; //
      g_sLiWCard[i].m_nAEqualsPreviousACtr = 0; //
      g_sLiWCard[i].m_nTickA = 0;  //
      g_sLiWCard[i].m_nTickB = 0;  //
      g_sLiWCard[i].m_nADIndexA = 0; //
      g_sLiWCard[i].m_nADIndexB = 0; //
      g_sLiWCard[i].m_nADReg = 0; //
           
      
      g_sLiWCard[i].m_bUsePollB = TRUE;             // bool to show which poll data to use(set use B, else use A).

      g_sLiWCard[i].m_lADCValue_Ap = 0;       // A previous
      g_sLiWCard[i].m_lADCValue_Bp = 0;       // B previous.
      g_sLiWCard[i].m_nADCValueMSW_Ap = 0;
      g_sLiWCard[i].m_nADCValueMSW_Bp = 0;
         
      
      g_sLiWCard[i].m_lADCValue_App = 0;       // A prior to previous
      g_sLiWCard[i].m_lADCValue_Bpp = 0;       // B prior to previous.
      g_sLiWCard[i].m_nADCValueMSW_App = 0;
      g_sLiWCard[i].m_nADCValueMSW_Bpp = 0;
      
   
   }


}

     
