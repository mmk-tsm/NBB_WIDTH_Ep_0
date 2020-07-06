//////////////////////////////////////////////////////
// BatchCalibrationFunctions.h
//
// Group of functions that allows the user to calibrate
// a batch blender system.
//
// M.Parks                              30-03-2000
// First Pass

//bb
//  M.McKiernan                         26-05-2004
//  Removed the nLayer which used to be in the CP2k - batch blender has no layer selection.
//  M.McKiernan                         29-09-2004
// m_fMaxGramsPerPercent renamed to m_fMaxBatchSizeInKg;     asm = MAXGRAMSPERPER
//  M.McKiernan                         30-09-2004
// Adding the assembler names for backward reference purposes.
// Renamed m_bEICardPresent to m_nPeripheralCardsPresent.
// Added m_nShutDownTime
// m_NMaxPercentage[] redefined as floats m_fMaxPercentage[]
// m_bFillOrderAutoSelect renamed to m_bFillOrderManualSelect
// m_bScreenPrintFlag added.
// m_nScrewSpeedMax renamed to m_fMaxScrewSpeedPercentage - later removed, same as Max ext. spt.
// m_fMaxExtruderSetpoint renamed to  m_fMaxExtruderSpeedPercentage
// Added Liquid calibration data for completeness.
//
//  M.McKiernan                         26-11-2004
// Removed m_bBypassModeFlag, had been already defined by m_bBypassMode.
// m_bBypassModeFlag now used for m_bPurgeEnable.
//
//
// P.Smith                                  4-04-2005
// added m_nCalibrationModuleSerialNo and spares at the beginning.
//
// P.Smith                                  6-04-2005
// added OnBoardCalDataStruct
//
// P.Smith                                  10-08-2005
// added m_nIntermittentMixTime
// Added onboard calibration used.
//
// P.Smith                                  18-08-2005
// added m_nMaterialTransportDelay,m_nPostMaterialTransportDelay
//
//
// P.Smith                                  23-08-2005
// added m_nUseRegrindAsReference
//
// P.Smith                                  23-09-2005
// make m_cPermanentOptions & m_cTemporaryOptions unsigned characters
//
// P.Smith                                  6/10/05
// make m_nChecksum unsigned int
//
//
// P.Smith                                  17/10/05
// change m_nCorrectEveryNCycles to BOOL.m_bCorrectEveryNCycles
//
// P.Smith                                  19/12/05
// modify structure for onboard calibration data so that it lines
// up with the original assembler version.
//
// P.Smith                      16/1/06
// first pass at netburner hardware conversion.
// Bool -> BOOL
//
// P.Smith                      2/2/06
// #include <basictypes.h>
//
// P.Smith                      2/2/06
// changed  to unsigned int  m_nComponents;                     // NOBLNDS - No. of components
///unsigned int  m_nDiagnosticsMode;                // DIAGMODE
// unsigned int  m_nLanguage;                       // language
// unsigned int  m_nBlenderMode;                    // BLENDERMODE
// unsigned int  m_nDeltaTime;                      //
// unsigned int  m_nWeightAvgFactor;                // WGTAVGFACTOR weight  averaging factor(time in seconds worth of data averaged)
// unsigned int  m_nScrewSpeedMin;                  //
// unsigned int  m_nScrewSpeedTareCounts;           // Tare counts, for zeroing rpm
// unsigned int  m_nMaxThroughput;                   // MXTPT1 - max kg/h throughput.
// unsigned int  m_nBatchesInKghAvg;                // BATINKGHAVG
// unsigned int  m_nMixingTime;                     // MIXTIM DS     1        ; MIXING TIME.
// unsigned int  m_nRegrindComponentNumber;      // REGCOMP - Regrind component no.
// unsigned int  m_nControlMode;                    // CRLMODE - Haul-off/Extruder/blending only.
// unsigned int  m_nFillRetries;                    // FILLRETRY - 0 = disable, else no. of retries.
// unsigned int  m_bVacLoading;                         // VACLOAD - enabled/disabled
// unsigned int  m_nPrintHours;                     // hours after print.
// unsigned int  m_nScewSpeedSignalType;            // SSSTYPE - type of signal for screw speed
//
//unsigned int           m_nPhase1OnTime;                   // PH1ONTIME related to cleaning.
// unsigned int           m_nPhase1OffTime;                  // PH1OFFTIME related to cleaning.
// unsigned int           m_nPhase2OnTime;                   // PH2ONTIME related to cleaning.
// unsigned int           m_nPhase2OffTime;                  // PH2OFFTIME related to cleaning.
// unsigned int           m_nCleanRepetitions;               // BCLEANREPETITIONS no of repetitions in blender clean cycLE.
// unsigned int           m_nMixerCleanRepetitions;          // MCLEANREPETITIONS repetitions in mixer clean cycle.
// unsigned int           m_nBypassTimeLimit;                // BYPASSTIMELIMIT bypass time limit.
// unsigned int           m_nPercentageTopUp;                // PERTOPUP % top up.   // PERTOPUP % TOP UP.
// unsigned int           m_nBypassTimeLimitClean;           // BYPASSDCLNTIMELIMIT bypass time limit during cleaning.
// unsigned int           m_nTopUpCompNoRef;                 // TOPUPCOMPNOREF Top up component # reference.
// unsigned int m_nCalibrationModuleSerialNo;   // CALMODSERNO
//
//
// P.Smith                      15/6/06
// added char    m_cEmailxxxx strings
// put unsigned int  m_nChecksum;  at end of calibration block
//
// P.Smith                      26/6/06
// make m_nPeripheralCardsPresent unsigned int
//
// P.Smith                      14/11/06
// added m_bLineSpeedSignalType
// added m_fDtoAPerKilo,  m_fDtoAPerMeterPerMinute,m_fDtoAPerKiloCMR,m_bLineSpeedSignalType

// P.Smith                      16/11/06
// remove any reference to monitoring factor.
// m_nRecipeEntryType added

// P.Smith                      4/1/07
// change m_bLineSpeedSignalType to a BOOL
//
// P.Smith                            11/1/07
// name change g_CalibrationData.m_nLineSpeedSignalType
//
// P.Smith                            1/3/07
// added m_ffComponentCPI,m_ffComponentCPIStage2,m_ffComponentCPIStage3,m_nFlowRateChecksum
//
// P.Smith                            25/4/07
// add Multiblend structure to calibration data
//
// P.Smith                            24/5/07
// added m_nResetCounter
//
// P.Smith                            21/6/07
// add m_nCycleCounter to calibration data
//
// P.Smith                            26/6/07
// added m_bOptimisation to allow optimisation to tbe switched on /off
//
// P.Smith                            4/7/07
// added m_fComponentLoaderEmptyingRate,m_nComponentBinVolume[MAX_COMPONENTS]
// added m_fComponentLoaderEmptyingRate, m_fComponentLoaderFullWeight
//
//
// P.Smith                            20/7/07
// changed m_nPurgeEnable to int
//
// P.Smith                            30/7/07
// put m_nPurgeEnable at the bottom of the calibration data so that the
// older revisions of software will have the calibration data in the same
// place
//
// P.Smith                            1/8/07
// added m_nCurrentExtruderNo to calibration data, this needs to be retained when
// the blender starts up to allow the catch box to be emptied to the last extruder
// that vacuuming material.
//
// P.Smith                            18/10/07
// added m_nCleaning, leave old bool to stop calibration table from moving.
// added m_bComponentNamingAlphanumeric
//
// P.Smith                            12/11/07
// line speed average added m_nLineSpeedAverage
//
//
// P.Smith                            19/12/07
// added  m_fRPMPerKilo;
//          m_fMaxPercentageDeviation[MAX_COMPONENTS];
//          m_nPauseOnErrorNo; //
//          m_nDeviationAlarmEnable;
//
// P.Smith                            10/1/08
// added m_bPauseBlenderOnPowerup
//
// P.Smith                            29/1/08
// added software id hash m_nSoftwareIDHash
//
// P.Smith                            12/2/08
// m_nChecksum is put at the end of the calibration menu.
//
// P.Smith                            19/2/08
// add m_nLogToSDCard, m_nLogFormatIsCSV;
//
// P.Smith                            27/3/08
// new structure for flow rates and process data formed RepeatedDataStruct
//
// P.Smith                            23/4/08
// m_nCycleCounter removed from cal data
// added m_bAllowForContinuousDownload
// remove m_nCurrentExtruderNo, name change m_nLogToSDCard to m_bLogToSDCard
//
// P.Smith                            23/6/08
// added checksum for process data
// added m_bTelnet
//
// P.Smith                            18/9/08
// add m_fLiquidAdditiveKgPerHour - this is the throughput that the pib pump gives at
// 0.25 of d/a output.
//
// P.Smith                            16/10/08
// added m_fLiquidAdditiveScalingFactor
//
// P.Smith                            11/12/08
// add usb printer config eeprom.
//
// P.Smith                            29/1/09
// added m_bNoParity
//
// P.Smith                            27/2/09
// added m_fFactoryWeightConstant,m_lFactoryTareCounts, hash
//
// P.Smith                            2/3/09
// added m_n24HourOngoingResetCounter & m_n24HourOngoingMotorTripCounter to repeated
// data structure
//
// P.Smith                            6/3/09
// add DescriptionStruct for serial nos etc
//
// P.Smith                            24/4/09
// added m_cNBBTestPassFailStatus to DescriptionStruct
//
// P.Smith                            29/5/09
// added m_bCheckForValidComponentTargetPercentage
//
// P.Smith                            17/6/09
// added m_m_nBlenderTypewFillingMethod to indicate filling method
// 0 will be normal, then 1 pulsing etc.
//
// P.Smith                            8/7/09
// added m_wFillingMethod and m_wFillingMethodHash
//
// P.Smith                            25/8/2009
// Added m_lTSMPassWord;  used m_lCalibSpare2, so data should not move.
//
// P.Smith                            17/9/09
// name change m_nLogToSDCard
//
// P.Smith                            2/10/09
// added m_bMinimumOpeningTimeCheck
//
// P.Smith                            12/10/09
// added m_bFlowRateCheck
//
// P.Smith                            15/10/09
// added m_bVac8Expansion
//
// P.Smith                            15/1/09
// allow sd card error counters and storage.
//
// P.Smith                            17/2/09
// added m_wEtherNetIPDummyWord  &  m_wEtherNetIPHash;
//
// P.Smith                            5/3/10
// added m_wBlenderOptions to OnBoardCalDataStruct this is used as
// the second set of options for the blender licensing.
//
// P.Smith                            23/3/10
// define m_wSecurityFailErrorCode
//
// P.Smith                            27/4/10
// added  m_lComponentActivations,m_lHoursBlenderIsRunning,m_nHistoryLogHash;
//
// P.Smith                            27/10/11
// added m_bPCF8563RTCFitted
// M.McKiernan							23/4/2020
//....wfh MMK.. - 23.4.2020.
// Added       WORD			m_nMachineType;

//////////////////////////////////////////////////////

#ifndef __BATCHCALIBRATIONFUNCTIONS_H__
#define __BATCHCALIBRATIONFUNCTIONS_H__

#include "General.h"
#include <basictypes.h>

// Calibration data structure
//

typedef struct
{
	unsigned int			m_unRecipeNo;
	BOOL        			m_bEnabled;
	char                    m_cExtruderName[20];

} MultiblendExtruderStruct;

typedef struct
{
    char         m_cNBBSerialNo[NBB_SERIAL_NUMBER_SIZE];
    char         m_cUnused[3];
    char         m_cNBBTestPassFailStatus;
} DescriptionStruct;


typedef struct  {

       unsigned int m_nCalibrationModuleSerialNo;   // CALMODSERNO
       int          m_nSpare1;
       int          m_nSpare2;
       int          m_nSpare3;
       int          m_nSpare4;
       int          m_nSpare5;
       int          m_nSpare6;
       int          m_nSpare7;
       int          m_nSpare8;
       int          m_nSpare9;
       int          m_nSpare10;
       int          m_nSpare11;
       int          m_nSpare12;
       int          m_nSpare13;
       int          m_nSpare14;


       float         m_fBatchSize;                      // DFTW1 Batch size
       float         m_fWeightConstant;                 // Caib. weight constant.
       long          m_lTareCounts;                     // Hopper calibration Tare counts.
       unsigned int  m_nDiagnosticsMode;                // DIAGMODE
       unsigned int  m_nLanguage;                       // language
       unsigned int  m_nControlAvgFactor;               // CONTAVGFACTOR control averaging factor
       unsigned int  m_nDeltaTime;                      //
       unsigned int  m_nWeightAvgFactor;                // WGTAVGFACTOR weight  averaging factor(time in seconds worth of data averaged)
       float         m_fLineSpeedConstant;
       unsigned int  m_nBlenderMode;                    // BLENDERMODE
       float         m_fScrewSpeedConstant;
//       float         m_fMaxScrewSpeedPercentage;      // MAXEXSP - duplicated below.
       unsigned int  m_nScrewSpeedMin;                  //
       unsigned int  m_nScrewSpeedTareCounts;           // Tare counts, for zeroing rpm
       unsigned int  m_nMaxThroughput;                   // MXTPT1 - max kg/h throughput.
       float         m_fControlGain;                    // CLGAIN
       unsigned int  m_nBatchesInKghAvg;                // BATINKGHAVG
       unsigned int  m_nMixingTime;                     // MIXTIM DS     1        ; MIXING TIME.
       unsigned int  m_nRegrindComponentNumber;      // REGCOMP - Regrind component no.
       long          m_lManagerPassWord;
       long          m_lOperatorPassWord;
       unsigned int  m_nComponents;                     // NOBLNDS - No. of components
       float         m_fDACountsPerRPM;                 // D/A counts per RPM
       BOOL          m_bExtruderStallCheck;             // EXSENAB -
       long          m_lTSMPassWord;                    // Reserve 8 bytes here in case data needs to be stuffed in
       long          m_lCalibSpare2;
       unsigned int  m_nControlMode;                    // CRLMODE - Haul-off/Extruder/blending only.
       BOOL               m_bFastControlResponse;                 // FSTCTRL  0 = OFF data, 1 = ON ??
       unsigned int  m_nFillRetries;                    // FILLRETRY - 0 = disable, else no. of retries.
       unsigned int  m_bVacLoading;                         // VACLOAD - enabled/disabled
       BOOL          m_bCycleFastFlag;                  // batch cycling fast or slow - settling time
       unsigned int  m_nPrintHours;                     // hours after print.
       unsigned int  m_nScewSpeedSignalType;            // SSSTYPE - type of signal for screw speed

       BOOL          m_bNoParity;

       BOOL          m_bVolumetricModeFlag;             // ISVOLMODE - enabled/disabled.  //ISVOLMODE
       BOOL          m_bShutDownFlag;                   // SHUTDOWNEN - enabled/disabled
       unsigned int  m_nShutDownTime;                   // SHUTDOWNTM - time in minutes.
       unsigned int  m_nLevelSensorDelay;               // LSDELAY - Level sensor delay
       unsigned int  m_nPeripheralCardsPresent;         // EIOMITTED - YES/NO (b0 = SEI, b1 = LLS,...)
       float         m_fMaxExtruderSpeedPercentage;     // MAXEXSP          // D/A % (0 - 999)
       float         m_fLiquidKValue;                    // flow calibration, kg/h at a given speed.
       BOOL               m_bLiquidEnabled;                           // AVECPIB - facility for liquid enabled (TRUE/FALSE)
       unsigned int  m_nUnitType;                       // UNITTYPE - type of units (0 = metric, 1 = Imperial)

       unsigned int  m_nLineNumber;                     // LINENO - Line number
//       int           m_nScrewFitted[MAX_COMPONENTS];    // screw no. fitted per component.


       unsigned int  m_nPrinterType;                    // printer type.
       unsigned int  m_nBaudRate;                       // serial baud rate
       unsigned int  m_nDataFormat;                     // data bits, etc.
       BOOL          m_bRollReportPrintFlag;            // Enabled/disabled.
       BOOL          m_bOrderReportPrintFlag;           // Enabled/disabled.
       BOOL          m_bShiftReportPrintFlag;           // Enabled/disabled.
       BOOL          m_bHourlyPrintFlag;                // hourly printout Enabled/disabled.
       BOOL          m_bMonthlyPrintFlag;               // monthly printout Enabled/disabled.
       BOOL          m_bDiagPrintFlag;                  // printing of diagnostics Enabled/disabled.
       BOOL          m_bAlarmPrintFlag;                 // printing of alarms Enabled/disabled.
       BOOL          m_bScreenPrintFlag;                 // printing of screen Enabled/disabled.

       unsigned int  m_nShift1Time;                     // shift 1 time.
       unsigned int  m_nShift2Time;                     // shift 2 time.
       unsigned int  m_nShift3Time;                     // shift 3 time.
       unsigned int  m_nShift4Time;                     // shift 4 time.

       BOOL          m_bStandardCompConfig;             // STDCCFG  Standard component config (YES/NO)STDCCFG
//       int           m_nCompConfig[MAX_COMPONENTS];     // COMP1CFG Component config word for each comCOMP1CFGponent.
       unsigned int  m_nBlenderType;                    // BLENDERTYPE - Micra etc.
       unsigned int  m_nOfflineType;                    // OFFLINETYPE  - OFFLINE TYPE BLEND USING LEVEL SENSORS. hi/lo, blend to xx kgs, etc
       unsigned int  m_nPurgeComponentNumber;           // PURGECOMPNO
       BOOL          m_bHighLoRegrindOption;            // HILOREGENABLED enabled/disabled
       unsigned int  m_nControlType;                    // CONTROLTYPE control type, 0-10V, inc/dec, etc.
       unsigned int  m_nControlLag;                     // CONTROLLAG - seconds?
       unsigned int  m_nIncreaseRate;                   // INCREASERATE - Increase rate - d/a counts per second
       unsigned int  m_nDecreaseRate;                   // DECREASERATE - Decrease rate - d/a counts per second
       unsigned int  m_nControlDeadband;                // DEADBAND - deadband for inc/dec control.
       unsigned int  m_nFineIncDec;                     // FINEINCDEC - Fine increase/decrease
       unsigned int  m_nControlPercentageLimit;         // Control % limit
//       float         m_fMaxPercentage[MAX_COMPONENTS];  // MAXPCNT1 - 12. - Max comp % that can be entered for each component.
       BOOL          m_bMaxComponentPercentageCheck;    // enabled/disabled
       unsigned int  m_nLatency;                        // latency time for valve.
       unsigned int  m_nMixShutoffTime;                 // Mixer shut off time in minutes  MIXINGSHUTOFF   DS      1       ; MIXING SHUT OFF IN MINUTES
       unsigned int  m_nIntermittentMixTime;            // INTMIXTIME        DS      1       ; ADDED INTERMITTENT MIXING TIME

       BOOL          m_bCheckForValidComponentTargetPercentage;
       BOOL          m_bFirstComponentCompensation;     // 1st comp compensation Enabled/disabled
       unsigned int  m_bCorrectEveryNCycles;            // CORRECTEVERYNCYCLES  DS   1       ; CORRECT EVERY CYCLE.
       unsigned int  m_nStageFillEn;                    // STAGEFILLEN - Multistage filling (0 = no, 1 = 2 phase, 2 = 3 phase..) ASM = STAGEFILLEN ( can be single/two/three)
       BOOL          m_bLayering;                       // Enabled/disabled.
       BOOL          m_bMixerScrewDetect;               // enable/disable
       BOOL          m_bDoubleDump;                     // enable/disable
       BOOL          m_bMouldingOption;                 // MOULDING enable/disable.
       BOOL          m_bRegrindRetry;                   // Yes/NO
       int           m_nRecipeEntryType;                // ENTRYTYPE
       BOOL          m_bBypassMode;                     // enable/disable.
       BOOL          m_bFillOrderManualSelect;          // MANUALFILLSORT enable/disable.
       BOOL          m_bTopUpMode;                      // TOPUP    enable/disable.

       BOOL          m_bMinimumOpeningTimeCheck;
       unsigned int  m_nPhase1OnTime;                   // PH1ONTIME related to cleaning.
       unsigned int  m_nPhase1OffTime;                  // PH1OFFTIME related to cleaning.
       unsigned int  m_nPhase2OnTime;                   // PH2ONTIME related to cleaning.
       unsigned int  m_nPhase2OffTime;                  // PH2OFFTIME related to cleaning.
       unsigned int  m_nCleanRepetitions;               // BCLEANREPETITIONS no of repetitions in blender clean cycLE.
       unsigned int  m_nMixerCleanRepetitions;          // MCLEANREPETITIONS repetitions in mixer clean cycle.
       unsigned int  m_nBypassTimeLimit;                // BYPASSTIMELIMIT bypass time limit.
       unsigned int  m_nPercentageTopUp;                // PERTOPUP % top up.   // PERTOPUP % TOP UP.
       BOOL          m_bBlowEveryCycle;                 // BLOWEVERYCYCLE YES/NO.
       float         m_fMaxBatchSizeInKg;               // MAXGRAMSPERPER                         (MAXGRAMSPERPER)
       unsigned int  m_nBypassTimeLimitClean;           // BYPASSDCLNTIMELIMIT bypass time limit during cleaning.
       unsigned int  m_nTopUpCompNoRef;                 // TOPUPCOMPNOREF Top up component # reference.

       BOOL          m_bSingleWordWrite;                // SINGLEWORDWRITE  facility to write %'s one word at a time.
//LIQUID
       float         m_fLiquidCalibrationConstant;          //LAPCON - LOW RATE CONST FOR Liquid Additive
         long               m_lLiquidTareValue;                     //LAATARE - FLOW RATE tare FOR L.A. (PIB)
       float         m_fLiquidControlGain;                  //LACG - CONTROL GAIN FOR L.A.
       unsigned int  m_nLiquidControlLag;                   //LACLAG - control lag FOR L.A.
       unsigned int  m_nLiquidSignalType;                   //LASTYPE - SIGNAL TYPE FOR LIQ. ADDIT.
       unsigned int  m_nLiquidMode;                         //LAMODE - MODE (0/1 = MEAS/MEAS & CTRL)
       unsigned int  m_nMaterialTransportDelay;             //VACUUMDELAY       DS      1       ; VACUUM DELAY.
       unsigned int  m_nPostMaterialTransportDelay;         //VACUUMPOSTDELAY   DS      1       ; VACUUM POST DELAY.
       unsigned int  m_nUseRegrindAsReference;              // USEREGRINDASREF   DS      1       ; USE REGRIND AS A REGERENCE FOR THE OTHER COMPONENTS 0=DISABLED
       unsigned int  m_nScrewFitted[MAX_COMPONENTS];    // screw no. fitted per component.
       unsigned int  m_nCompConfig[MAX_COMPONENTS];     // COMP1CFG Component config word for each comCOMP1CFGponent.
       float         m_fMaxPercentage[MAX_COMPONENTS];  // MAXPCNT1 - 12. - Max comp % that can be entered for each component.
       float         m_fLatency;                        // not defined before hand.

       char    m_cEmailMailServer[EMAIL_STRING_SIZE];
       char    m_cEmailUserName[EMAIL_STRING_SIZE];
       char    m_cEmailPassword[EMAIL_STRING_SIZE];
       char    m_cEmailSupervisorEmailAddress[EMAIL_STRING_SIZE];
       char    m_cEmailSupportEmailAddress[EMAIL_STRING_SIZE];
       char    m_cEmailFromEmailAddress[EMAIL_STRING_SIZE];
       char    m_cEmailBlenderName[EMAIL_STRING_SIZE];

       BOOL    m_bSendEmailOnOrderChange;                //
       float       m_fLiquidAdditiveScalingFactor;                                                     // DACPKGHR        DS      5       ; D/A PER KG/HR VALUE
       float       old_m_fDtoAPerMeterPerMinute;                      // DACPMPM  D/A PER M/min
       float       old_m_fDtoAPerKiloCMR;                                                  // DACPKGCMR       DS      3       ; D/A PER KG FOR CMR SYSTEM
       unsigned int         m_nLineSpeedSignalType;                     //LSSTYPE  DS     1       ; Line SPEED SIGNAL TYPE (0=pulses,1=COMMs,others=NA)
       double      old_m_ffComponentCPI[MAX_COMPONENTS];
       double      old_m_ffComponentCPIStage2[MAX_COMPONENTS];
       double      old_m_ffComponentCPIStage3[MAX_COMPONENTS];
       unsigned int         old_m_nFlowRateChecksum;
       MultiblendExtruderStruct m_stMultiblendExtruders[MULTIBLEND_EXTRUDER_NO];
       unsigned int old_m_nResetCounter;
       unsigned int old_m_nCycleCounter;
       BOOL       m_bOptimisation;
       unsigned int m_nComponentBinVolume[MAX_COMPONENTS];
       float      m_fComponentLoaderEmptyingRate[MAX_COMPONENTS];
       float      m_fComponentLoaderFullWeight[MAX_COMPONENTS];
       unsigned int  m_nPurgeEnable;                  // PURGEOPTION enable/disable. -
       BOOL       m_bFlowRateCheck;
       BOOL       m_bVac8Expansion;
       BOOL       m_bPCF8563RTCFitted;    // 0 - x1226 rtc fitted 1- PCF8653 rtc fitted
       BOOL       m_bunusedbool3;
       unsigned int  m_nUnused;                        // CALDSM DS     2                 ;~CHECKSUM FOR CAL. DATA
       unsigned int m_nCleaning;
       BOOL       m_bComponentNamingAlphanumeric;
       unsigned int m_nLineSpeedAverage; // LINESPEEDAVG    DS      1       ; LINE SPEED AVERAGE
       float        m_fLiquidAdditiveKgPerHour;
       float       m_fMaxPercentageDeviation[MAX_COMPONENTS];  //MAXPCNT1DEV     DS      2       ; MAX COMP %
       unsigned int m_nPauseOnErrorNo; // PAUSEONERRNO    DS      1
       unsigned int m_nDeviationAlarmEnable;                // COMPONENTPERENABLE DS   1       ; COMPONENT % ENABLE.
       BOOL       m_bPauseBlenderOnPowerup;
       BYTE       m_nLogToSDCard;
       BOOL       m_bLogFormatIsCSV;
       BOOL        m_bAllowForContinuousDownload;
       BOOL        m_bTelnet;    // indicates telnet is enabled.
// printer formatiing

       unsigned int  m_nDumpDeviceType;                 // printer type.
       BOOL          m_bAlarmDumpFlag;                 // printing of alarms Enabled/disabled.
       BOOL          m_bOrderReportDumpFlag;           // Enabled/disabled.
       BOOL          m_bRollReportDumpFlag;            // Enabled/disabled.
       BOOL          m_bHourlyDumpFlag;                // hourly printout Enabled/disabled.
       BOOL          m_bBatchLogDumpFlag;              //  batch log
       BOOL          m_bEventLogDumpFlag;              //  batch log
       BOOL          m_bHistoryLogDumpFlag;              //  batch log
       BOOL          m_bMonthlyDumpFlag;               // monthly printout Enabled/disabled.
       BOOL          m_bShiftReportDumpFlag;           // Enabled/disabled.
       BOOL          m_bScreenDumpFlag;                 // printing of screen Enabled/disabled.
       float         m_fFactoryWeightConstant;       // factory set weight constant
       long          m_lFactoryTareCounts;                     // Hopper calibration Tare counts.
       WORD          m_nFactoryWeightConstantHash;          //

       WORD          m_wFillingMethod[MAX_COMPONENTS];     //
       WORD          m_wFillingMethodHash;     //

       // liquid additive calibration parameters
       //....wfh MMK.. - 23.4.2020.
       WORD			m_nMachineType;



       unsigned int  m_nChecksum;                        // CALDSM DS     2                 ;~CHECKSUM FOR CAL. DATA


    } CalDataStruct;


typedef struct
{
       double      m_ffComponentCPI[MAX_COMPONENTS];
       double      m_ffComponentCPIStage2[MAX_COMPONENTS];
       double      m_ffComponentCPIStage3[MAX_COMPONENTS];
       unsigned int m_nFlowRateChecksum;
       unsigned int m_nResetCounter;
       float       m_fRPMPerKilo;                                                  // DACPKGCMR       DS      3       ; D/A PER KG FOR CMR SYSTEM
       float       m_fDtoAPerKilo;                                                     // DACPKGHR        DS      5       ; D/A PER KG/HR VALUE
       float       m_fDtoAPerMeterPerMinute;                      // DACPMPM  D/A PER M/min
       float       m_fDtoAPerKiloCMR;                                                  // DACPKGCMR       DS      3       ; D/A PER KG FOR CMR SYSTEM
       unsigned int m_n24HourOngoingResetCounter;
       unsigned int m_n24HourOngoingMotorTripCounter;
       float       m_fComponentOrderWeightAccumulator[MAX_COMPONENTS];                                                  // DACPKGCMR       DS      3       ; D/A PER KG FOR CMR SYSTEM
       WORD       m_nOrderWeightHash;
       float      m_fOrderLength;                                                  // DACPKGCMR       DS      3       ; D/A PER KG FOR CMR SYSTEM
       unsigned int    m_nMountFatReturnedValueOnSdError;
       unsigned int    m_nChangeDriveReturnedValueOnSdError;
       unsigned int    m_nFilePointerOnSdError;
       char            m_cFileNameOnSdCardError[SD_CARD_NAME_SIZE];
       unsigned int    m_nSdCardErrorCtr;
       unsigned int    m_nSDErrorHash;
       WORD 		   m_wEtherNetIPDummyWord;
       WORD 		   m_wEtherNetIPHash;
       BYTE 		   m_bySecurityFailErrorCode;
       long			   m_lComponentActivations[MAX_COMPONENTS];
       long			   m_lHoursBlenderIsRunning;
       WORD			   m_nHistoryLogHash;

       unsigned int  m_nChecksum;                        // CALDSM DS     2                 ;~CHECKSUM FOR CAL. DATA
} RepeatedDataStruct;



typedef struct  {

       unsigned int          m_nSBBModuleSerialNo;       //SBBSERNO
       unsigned char         m_cDummy;                   // DUMMY UNUSED
       unsigned int          m_cTestCode;                //TESTCODE            DS      2       ; TEST CODE
       unsigned int          m_cPowerUpStatus;           //POWERUPSTATUS       DS      2       ; POWER UP STATUS.
       unsigned char         m_cPermanentOptions;        //OPTIONCODE          DS      2       ; OPTION CODE
       unsigned char         m_cTemporaryOptions;        //OPTIONCODE          DS      2       ; OPTION CODE
       unsigned int          m_nBlenderRunHourCounter;   //BLENDERRUNHRCTR     DS      2       ;
       unsigned int          m_lSoftwareID;              //SOFTWAREID          DS      3       ; SOFTWAREID.
       WORD					 m_wBlenderOptions;			 // blender options
       WORD                  m_nSoftwareIDHash;          //


    } OnBoardCalDataStruct;



typedef struct  {

       unsigned long         m_lMasterLicenseCode;       //MASTERMODULE    DS      4       ; ADDRESS 1F80 MASTER MODULE CODE IS HELD HERE.


    } OnBoardEndCalDataStruct;







void    CalibrateHopper( void  );
void    CalibrateSpeed( void );

#endif //   __BATCHCALIBRATIONFUNCTIONS_H__



