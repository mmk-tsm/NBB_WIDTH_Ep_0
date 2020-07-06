//////////////////////////////////////////////////////
// P.Smith                      20/2/09
// blender self test
//
//
// P.Smith                      19/3/09
// added EventsCheck
// increased COMPONENT_TARE_COUNTS_DIFFERENCE_ALLOWED  to 70 to stop nusance alarms
// also increase  BLENDER_SELF_TEST_MAX_TARE_DIFFERENCE_ALLOWED to 200
// added limits for switched load cell voltage test.
// max retries allowed in self test.
// added items to structSelfTest for remaining tests
//
// P.Smith                      19/11/09
// added   m_nRefAToDStabilityTest, m_nLoadCellAToDStabilityTest
// define ATOD_REF_MAXIMUM_DEVIATION_ALLOWED   
// added SWITCH_IN_06_REFERENCE & SWITCH_IN_FANI_INPUT to allow different voltages
// to be switched to the a/d
//
// P.Smith                      23/11/09
// increased ATOD_REF_MAXIMUM_DEVIATION_ALLOWED to 10 and 
// ATOD_LOAD_CELL_MAXIMUM_DEVIATION_ALLOWED to 30 to ensure that the test will be passed
//
// P.Smith                      15/1/10
// change the self test parameters to allow it to work properly with the rev B6 board.
//
//////////////////////////////////////////////////////

#ifndef __BLENDERSELFTEST_H__
#define __BLENDERSELFTEST_H__

#define  PASSED     0x01;
#define  FAILED     0x02;

#define   FACTORY_TARE_PERCENTAGE_DIFFERENCE_ALLOWED                 10.0f
#define   FACTORY_WEIGHT_CONSTANT_PERCENTAGE_DIFFERENCE_ALLOWED      10.0f
#define   COMPONENT_TARE_COUNTS_DIFFERENCE_ALLOWED                   70
#define   BLENDER_SELF_TEST_MAX_TARE_DIFFERENCE_ALLOWED              200

#define   MAX_RETRIES_ALLOWED_IN_SELF_TEST                           10


// blender tests
//

#define   BLENDER_SELF_TEST                                           0x01
#define   SWITCH_IN_06_REFERENCE                                      0xA5
#define   SWITCH_IN_FANI_INPUT                                        0xA6


#define   LOW_LIMIT_COUNTS_FOR_LOAD_CELL_SWITCHED_VOLTAGE             (40000)
#define   HIGH_LIMIT_COUNTS_FOR_LOAD_CELL_SWITCHED_VOLTAGE            (60000)

#define   TSM3000_LOW_LIMIT_COUNTS_FOR_LOAD_CELL_SWITCHED_VOLTAGE     (15000)
#define   TSM3000_HIGH_LIMIT_COUNTS_FOR_LOAD_CELL_SWITCHED_VOLTAGE    (35000)
#define   ATOD_REF_MAXIMUM_DEVIATION_ALLOWED        (10)
#define   ATOD_LOAD_CELL_MAXIMUM_DEVIATION_ALLOWED  (30)




void BlenderSelfTestDiagnostic(  void );
BOOL EEPROMSelfTest( void );
BOOL SdCardTest( void );
BOOL AtoDInternalTest( void );
BOOL LoadCellCalibrationConstantCheck( void );
BOOL LoadCellCalibrationTareCheck( void );
BOOL ComponentTareDeviationCheck( void );
BOOL EventsCheck( void );
void TestFunction( int nTest );
void CopySelfResultsToMB( void );
BOOL CommunicationsTest( void );
void CheckCalibrationHash( void );
BOOL ComponentHasRetried( void );
BOOL HopperTareDeviationExceeded( void );
long GenerateFactoryCalibrationHash( void );
void UpDateFactorySettings( void );
BOOL ComponentStandardDeviationOk( void );
BOOL SwitchedLoadCellVoltageCheckOk( void );
void DecideOnRefAToDStabilityTest( void );
void RefAToDStabilityCheck( void );
void LoadCellAToDStabilityCheck( void );
void DecideOnLoadCellAToDStabilityTest( void );




//  Historic data
typedef struct {
   WORD   m_nSelfTestResult;    
   WORD   m_nEEPROMTest;    
   WORD   m_nSDCardTest; 
   WORD   m_nAtoDTest;
   WORD   m_nCalibrationConstantTest;
   WORD   m_nTareValueTest;
   WORD   m_nComponentTareTest;
   WORD   m_nEventsTest;
   WORD   m_nTareAlarms;
   WORD   m_nLeakAlarms;
   WORD   m_nDumpFlapAlarms;
   WORD   m_nCommunicationsTest;
   WORD   m_nPanelComms;
   WORD   m_nOptimisationComms;
   WORD   m_nSEIComms;
   WORD   m_nLLSComms;
   WORD   m_nNetworkComms;
   WORD   m_nTCPIPComms;
   WORD   m_nMotorTrip;
   WORD   m_nComponentRetryTest;
   WORD   m_nWeighHopperTareDeviationTest;
   WORD   m_nComponentStandardDeviationTest;
   WORD   m_nSwitchedLoadCellVoltageTest;
   WORD   m_nRefAToDStabilityTest;
   WORD   m_nLoadCellAToDStabilityTest;
   
   
   
   
        
} structSelfTest;


#endif  // __BLENDERSELFTEST_H__

