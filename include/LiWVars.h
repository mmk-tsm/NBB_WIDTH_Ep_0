/////////////////////////////////////////////////
//
// M. McKiernan                        5/1/08
// create variables file for LiW - loss in weight.
//
// M.McKiernan                         25/2/2008
// Added control, thruput and accumulator variables to  structLiWComponentData struct.
// and also variables below Version 2.
// M.McKiernan                         28/3/2008
// Added fdlisten/fdnet for telnet purposes.
// Added m_fEstimatedKgH, .m_nDACSetpoint
// added g_fCurrentTargetKgH,   g_bEnterAutoFlag;
// added struct for Lines speed - structLiWLSpeed
// added g_fLineSpeedUsedForSetpointCalc
// added g_bSaveAllProcessDataToEEprom
// simulated batch.   m_fLiWBatchWeightAccumulator; m_fLiWBatchWeight;
//
// M. McKiernan                      2/5/08
// Added .m_fActualPercentage
// M. McKiernan                      28/5/08
// Added g_bForeground50HzFlag  g_nForeground50HzCounter;
//
// M. McKiernan                      5/6/08
// Added variables for Screw Calibration in component structure. & g_bScrewCalibrationFlag
// Added define - DAC_PERCENT_25.
// Added m_fNewK1 & g_nMBScrewCalComponent, m_fKgHSetpointInManual
// Added m_bFillValveState;
// Added g_nSDCardDelayTimer
// Added #define ATTACHED_TO_9000 (TRUE)        
//      To do, this will need to be configurable.(defines what gets copied to MB table)
//
// M. McKiernan                      22/7/08
// Added "fast" throughput variables - m_fFastKgH,  m_bFastKgHValid, m_fMinExpectedKgH, m_nLessThanMinKgHCounter
//
// M. McKiernan                      21/8/08
// Added  DAC_PERCENT_5 (205)
//
// M. McKiernan                      19/2/2009
// Added pile of variables for blending only mode.
//
// M. McKiernan                      25/2/2009
// Added definitions for #define DAC_PERCENT_4 DAC_PERCENT_20 
//
// M. McKiernan                      30/4/2009
// Added variables for  Offline related.
//
// M. McKiernan                      1/5/09
// added  g_cLiWPollTime  g_nLiWPollDecisionCtr; g_bNetCommsParityChange;
// Added structure structLiWCardData 
//
// M. McKiernan                      26/5/09
// Changed AFTER_FILL_SETTLING_TIME  from 3 to to (6 * 10)    // 6 seconds 
// Added  m_bUseLiWPollA, m_nPollDataUsed to structLiWComponentData
// Added followning to structLiWCardData
// long  m_lADCValue_Ap; m_lADCValue_Bp;  m_nADCValueMSW_Ap; m_nADCValueMSW_Bp;
// m_lADCValue_App;  m_lADCValue_Bpp;    m_nADCValueMSW_App; m_nADCValueMSW_Bpp;
//   
// P.Smith                      26/6/09
// remove double definitions
/////////////////////////////////////////////////

//*****************************************************************************
// MACROS
//*****************************************************************************
#ifndef __LIWVARS_H__
#define __LIWVARS_H__


#include "General.h"
#include <basictypes.h>
#include "Alarms.h"

#define     ADMAXENTRIES   (100)
//#define     AFTER_FILL_SETTLING_TIME   (3 * 10)    // 3 seconds  - 10Hz.
#define     AFTER_FILL_SETTLING_TIME   (6 * 10)    // 6 seconds  - 10Hz.
#define     LIW_WT_AVAIL_CTR_MAX (99)              //stop at 99, used to determine valid data available
#define     SIMULATOR_INCREMENT_FILL   (2000)
#define     SIMULATOR_DECREMENT        (100)

#define DAC_PERCENT_1 (40)    // 1% of 4096
#define DAC_PERCENT_4 (164)    // 4% of 4096
#define DAC_PERCENT_5 (205)    // 5% of 4096
#define DAC_PERCENT_10 (409)    // 10% of 4096
#define DAC_PERCENT_20 (818)    // 20% of 4096
#define DAC_PERCENT_25 (1024)   //25% OF 4096
#define DAC_PERCENT_90 (3686)   //90% OF 4096
#define DAC_PERCENT_100 (4095)   //100%


#define     MAX_ENTRIES_RBUF  (10)
#define     MAX_ENTRIES_TBUF  (41)
#define     MAX_ENTRIES_WBUF  (121)

#define     MAX_ENTRIES_LSPD_BUF  (20)

#define     LIW_BATCH_TIME    (30)     // generate a simulated batch every 30 second.

#define ATTACHED_TO_9000 (TRUE)        // To do, this will need to be configurable.(defines what gets copied to MB table)

void InitialiseLiWVars( void );
void InitialiseLiWComponentStructures( void );
void InitialiseLiWLineSpeedStructure( void );

// Structure holding the buffer data for component.
typedef struct {

	// general
	BOOL     m_bTareFlag;     // taring flag, i.e. set while taring.
	BOOL    m_bHopperEmptyFlag;
	int     m_nHopperEmptyCounter;
	
	float		m_fWeightInHopper;
	BOOL     m_bMaxHopperWeightReached;
   BOOL     m_bFillOnFlag;
   int      m_nFillTimeRemaining;     // tenths of second.
   int      m_nAfterFillLagRemaining;     // tenths of second.
	BOOL    m_bFillCompleteFlag;
	WORD    m_nFillAlarm;
	BOOL    m_bFillValveState;

   BOOL     m_bLiW1SWeightAverageAvailable;

   long		m_lOneSecondSimulatedADC;
   long     m_lOneSecondMeasuredADC; 
   BOOL     m_bUseLiWPollA;            // bool to show which poll data to use(set use A ).
   BYTE     m_nPollDataUsed;
   
   int      m_nADEntriesSinceFill;
   long     m_lLiWCycleSecondNo;
   BYTE     m_nLiWADCError;
   
  
	// Raw Data.
	long		m_lOneSecondADValue[MAX_ENTRIES_RBUF];
	int		m_nNoOfEntriesRBuf;
	int      m_nIndexRBuf;
	long     m_lAverageRBuf;
	
	int		m_nLiW1SWtAvailCounter;     // one second wt avail counter.
	
	// Time Buffer Data
	long		m_lTimeBufData[MAX_ENTRIES_TBUF];
	int		m_nNoOfEntriesTBuf;
	int      m_nIndexTBuf;
	float    m_fCountsPerSecond;
	int      m_nCurrentDT;           // Delta Time in use.
	BOOL     m_bDT5;
	float    m_fCountsPerSecond_5;

	// Weight loss Buffer Data
	float		m_fWLBufData[MAX_ENTRIES_WBUF];   //data in W.L. buf is counts/sec.
	int		m_nNoOfEntriesWBuf;
	int      m_nIndexWBuf;
	int      m_nIndexCurrentWBuf;
	float    m_fAverageWBuf;
	float    m_fSumWBuf;
	int      m_nEntriesInSumWBuf;
	int      m_bNoNewDataFlag;
	
	// Extruder & screw control related. 
	WORD    m_nDACValue;
	WORD    m_nDACSetpoint;
	BOOL    m_bPotentiometerControl;
	// screw calibration
	BOOL    m_bScrewCalibrationInProgress;
	WORD    m_nScrewCalibrationInProgressTime;
	WORD    m_nScrewCalibrationStabilityCounter;
	float   m_fCurrentKgH;
	float   m_fPreviousKgH;
	
	
	float   m_fControlValue;
	float   m_fControlSum;
	int     m_nControlCount;           // WCCTOF in ASM
	int     m_nControlUpdateLag;
	int     m_nControlActions;
   BOOL     m_bControlFlag;         // set when controlling
   BOOL     m_bRapidControlResponse;
	
	float   m_fControlValue2;
	BOOL    m_bControlValue2Available;
	
	float   m_fMotorSetPercentage;
	
	// kg/h
	float   m_fActualKgH;
   float   m_fTargetKgH;
   float   m_fEstimatedKgH;
   float   m_fKgHSetpointInManual;
   float   m_fFastKgH;
   BOOL    m_bFastKgHValid;
   float   m_fFastKgH20;
   BOOL    m_bFastKgH20Valid;
   float   m_fFastCountsPer10S;
   
   float    m_fMinExpectedKgH;
   int      m_nLessThanMinKgHCounter;

	float   m_fActualPercentage;    // component actual %.
   
	float   m_fUsedK1;          // kg/h at 25% being used by blender
	float   m_fRevisedK1;       // revised or updated K1.
	float   m_fNewK1;            // new K1 during screw calibration.
	int     m_nRevisedK1Validity; //   higher value indicates better data.

   // Weight Accumulators
   float    m_fFrontRollWeightAccumulator;
   float    m_fBackRollWeightAccumulator;
   float    m_fOrderWeightAccumulator;
   float    m_fShiftWeightAccumulator;
//simulated batch.   
   float    m_fLiWBatchWeightAccumulator;
   float    m_fLiWBatchWeight;
   
   //blending only
   float    m_fBOKgH;
   float    m_fBOControlValue;

   // Offline operation.
   WORD     m_nOffLineSavedDACValue;
   int      m_nOffLineSavedControlActions;
   float    m_fOffLineSavedAverageWBuf;
      
  
} structLiWComponentData;

// Structure holding the buffer data for component.
typedef struct {
	// 
	int		m_nPulsesPerSecond[MAX_ENTRIES_LSPD_BUF];
	int		m_nNoOfEntriesLSPDBuf;
	int      m_nIndexLSPDBuf;
	int      m_nAverageLSPDBuf;
	float    m_fLineSpeedAvg;
	BOOL     m_bLineSpeedValid;

} structLiWLSpeed;

// structure for data coming directly from LiW card(s)
typedef struct {
   WORD  m_nADCValueMSW_A;    //msb for poll A
   WORD  m_nADCValueLSW_A;    //lsb for poll A
   long  m_lADCValue_A;       // amalgamated result for poll A.
   WORD  m_nADCValueMSW_B;    //msb for poll B
   WORD  m_nADCValueLSW_B;    //lsb for poll B
   long  m_lADCValue_B;       // amalgamated result for poll B.
   
   long  m_lADCValue_Ap;       // A previous
   long  m_lADCValue_Bp;       // B previous.
   WORD  m_nADCValueMSW_Ap;
   WORD  m_nADCValueMSW_Bp;
   

   long  m_lADCValue_App;       // A prior to previous
   long  m_lADCValue_Bpp;       // B prior to previous.
   WORD  m_nADCValueMSW_App;
   WORD  m_nADCValueMSW_Bpp;
   
   
   WORD  m_nBEqualsCurrentACtr;  //counter for data from polls A and B being equal 
   WORD  m_nAEqualsPreviousBCtr; // counter for data from poll A equalling previous B poll.
   WORD  m_nBEqualsPreviousBCtr;
   WORD  m_nAEqualsPreviousACtr;
   BYTE  m_nTickA;
   BYTE  m_nTickB;
   BYTE  m_nADIndexA;
   BYTE  m_nADIndexB;
   WORD  m_nADReg;
   
   BOOL  m_bUsePollB;            // bool to show which poll data to use(set use B, else use A).
   
} structLiWCardData;

// for telnet connection.
extern  int      fdlisten;
extern  int      fdnet;
extern  char     cTelWrBuffer[];

extern  BOOL     g_bSaveAllProcessDataToEEprom;

extern  BOOL        g_bLiWDebug;
extern  BOOL        g_bLiWSimulatorRunning;
extern  long        g_lOneSecondSimulatedADC;

extern  int          g_nCHF10HzCounter;
extern  BOOL        g_bForeground50HzFlag;
extern  DWORD       g_nForeground50HzCounter;

extern  int         g_nADBufferIndex;    
extern  int         g_nADEntriesSinceFill;
extern  int         g_nLiWCycleSecondNo;

extern  float       g_fADBuffer[ADMAXENTRIES];
extern  int         g_nTimeInLiWCycle[ADMAXENTRIES];   
extern  float       g_fMinHopperWeight[MAX_COMPONENTS];
extern  float       g_fMaxHopperWeight[MAX_COMPONENTS];
extern  float       g_fHopperWeight[MAX_COMPONENTS];    
extern  int         g_nFillTime10Hz[MAX_COMPONENTS];
extern  int         g_nHopperEmptyCounter[MAX_COMPONENTS];
extern  int         g_nLiW1SWtAvailCounter[MAX_COMPONENTS];

extern  float       g_fSlope1;

extern  BOOL        g_bFillOnFlag[MAX_COMPONENTS];
extern  BOOL        g_bMaxHopperWeightReached[MAX_COMPONENTS];

extern  int         g_nFillTimeRemaining[MAX_COMPONENTS];     // tenths of second.
extern  int         g_nAfterFillLagRemaining[MAX_COMPONENTS];     // tenths of second.

extern  BOOL        g_bLiW1SWeightAverageAvailable;

extern   BOOL        g_bVolumetricChangeFlag;
extern   BOOL        g_bScrewCalibrationFlag; 

extern  int         g_nAF1;  //todo
extern  int         g_nAF2;  //todo
extern   int        g_nDeltaT; //todo

// Version 2.
extern  float    g_fTotalFrontRollWeight;
extern  float    g_fTotalBackRollWeight;
extern  float    g_fTotalOrderWeight;
extern  float    g_fTotalShiftWeight;

extern  int   g_nMBWeightCalibrationComponent;
extern  int   g_nMBTareCommandComponent;
extern  int   g_nMBScrewCalComponent;

extern  int   g_nControlLag;
extern  BOOL  g_bLiWBlenderPaused;
extern  BOOL  g_bCalibrationMode;
extern  int   g_nTimeSinceManualChange;
extern  BOOL  g_bManualChangeOccurred;
extern   int  g_nManualChangeLag;
extern  float g_fActualGrammesPer12Ft2;
extern  BOOL  g_bNormalManualEnable;
extern  BOOL  g_bHMinFlag;
extern  BOOL  g_bEnterAutoFlag;
extern  float g_fCurrentTargetKgH;
extern  float g_fLineSpeedUsedForSetpointCalc; 

extern  WORD     g_nLiWBatchTimer;
extern  WORD     g_nLiWSimulatedBatchCounter;
extern  WORD     g_nStartUpTimer;
extern  WORD     g_nSDCardDelayTimer;

// relating to Blending Only.
extern  int      g_nHOSeconds;
extern  int      g_nBOManualLag;
extern  int      g_nNormalManualLag;
extern  int      g_nHOControlLag;

extern  float    g_fBOExtruderRPM;
extern  float    g_fFastKgHTotal;
extern  float    g_fBOTotalKgH;
extern  float    g_fHOBOKgH;                               //HPKGH
extern  float    g_fHOBOKgHUsed;                           //HPKGHU

extern  bool     g_bBOPercentageChangeFlag;
extern  bool     g_bFastKgHValid;
extern  bool     g_fFastKgHTotalValidity;

extern  float    g_fTotalKgHSetpoint;
extern  bool     g_bShortDurationSpeedFluctuationMode;
extern  bool     g_bShortDurationSpeedFluctuationFlag;
extern  float    g_fSDSF_KgH;                              
extern  float    g_fSDSF_RPM;    
extern  float    g_fBOExtruderRPMOld;                           
extern  int      g_nHODataSaveLag;
extern  bool     g_bEnterBlendingOnlyAutoFlag;
extern  bool      g_bLineStartFlag;
extern  float    g_fPercentScrewChange;
extern  bool     g_bScrewSpeedChangeFlag;
extern  bool     g_bHaulOffTimeFlag;
extern  bool     g_bHOThroughputValid;
extern  float    g_fScrewCorrectionFactor;
extern  float    g_fHaulOffRPM;
extern  float    g_fHaulOffScrewRatio;
extern  float    g_fBOTotalKgHOld;
extern  bool     g_bPrevBOExtruderRPMWasZero;
extern  bool     g_bHOFastSaveRate;
extern  bool     g_bKgHPerRevIsValid;

extern  float    g_fHOMPerMinSetpoint;   

extern  float    g_fLiquidKgH;

extern  int      g_nExtruderRPMGEMinCounter;
extern  int      g_nExtruderRPMLTMinCounter;
extern  bool     g_bExtruderStopped;
extern  float    g_fBOComponent1TargetPercentage;
extern  float    g_fExtruderRPMRatio;
extern  float    g_fExtruderRPMDiff;
extern  int      g_nTimeSinceExtruderRPMChange;
extern  float    g_fPrevExtruderRPM;
extern  float    g_fBOTotalKgH_KPR; 

extern  int      g_nExtruderRPMSumCtr;
extern  float    g_fExtruderRPMAvg3;
extern  float    g_fExtruderRPMSum;
extern  BOOL     g_bAlarmOccurredHistory[MAXIMUM_ALARMS][MAX_COMPONENTS];                       

// Offline related.
extern  BOOL     g_bOffLinePaused;               
extern  BOOL     g_bOffLineManualAutoFlag;       
extern  BOOL     g_bOffLineSetpointChangedFlag; 
extern  BOOL     g_bCommsRestartFlag;
extern  BOOL     g_bCommsPauseFlag;
extern  int      g_nLineSpeedMinorChangeCounter;
extern  int      g_nLiWAtQCtr;
extern  int      g_nUpdLiWCtr;
extern  int      g_nDPTAtResponse;
extern  int      g_nDPTAtResponseInUADC;
extern  BOOL     g_bSecondRolloverForPolling;
extern  char     g_cLiWPollTime;
extern  int      g_nLiWPollDecisionCtr;

extern  BOOL     g_bNetCommsParityChange;

#endif //__LIWVARS_H_
