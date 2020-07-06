//////////////////////////////////////////////////////
// InitialiseHardware.h
//
// Function initialises the RAM memory variables that
// hold the pointers to the hardware i/o addresses.
//
// M.Parks                              24-03-2000
// M.McKiernan                              11-06-2003
//          ReadCP2kAddress added.
// M.McKiernan                              28-11-2003
//          Latch1/2_Output_On/Off, ReadSBBIO_Input added.
// M.McKiernan                              17-06-2004
//          CmpOn(i), CmpOff(i) added. - mimicks CMP1ON - CMP12ON in batch ASM's.
// M.McKiernan                              04-11-2004
//      Added ReadSBBLinkOptions(void)
// M.McKiernan                              08-11-2004
//      Added Paraic's stuff.
// M.McKiernan                              18-11-2004
// OpenFeedIfDiagnosticMode renamed to OpenFeed1
// CloseFeedIfDiagnosticMode renamed to CloseFeed1
// Added OpenBypass1( void ) void CloseBypass1( void ), ReadContactorInput(), Bool ReadPauseInput( void )
//
// M.McKiernan                              28-11-2004
// Added SBBSSROutputOn( void ), SBBSSROutputOff( void )
//
// M.McKiernan                              13-12-2004
//  Added AlarmOutputsOn() AlarmOutputsOff( ) AlarmBeaconOn( ) 
// Added AlarmBeaconOff( ), AlarmRelayOn( void ) AlarmRelayOff()

//
// P.Smith                                  29-03-2005
// Added Bool ReadScrewDetection ( void )
//
// P.Smith                          19/1/06
// first pass at converting the software over to NBB hardware.
// 
// P.Smith                      9/2/06
// included  <basictypes.h>
//
// P.Smith                      15/1/09
// added InitialiseExpansion23S17
//////////////////////////////////////////////////////
#ifndef __INITIALISEHARDWARE_H__
#define __INITIALISEHARDWARE_H__

#include <basictypes.h>

void    InitialiseHardware( void );
char ReadCP2kAddress(void);
void SBBIO_Latch1_Output_On (int nOutput);
void SBBIO_Latch1_Output_Off (int nOutput);
void SBBIO_Latch2_Output_On (int nOutput);
void SBBIO_Latch2_Output_Off (int nOutput);

int ReadSBBIO_Input (unsigned char cBitMask);
void ToggleHBLED( void );
void CmpOn (int nComponent);
void CmpOff (int nComponent);
void OpenDump( void );
void OpenDumpValve( void );
void OpenDump1( void );
void CloseDump( void );
void CloseDumpValve( void );
void CloseDump1( void );

void MixerOn( void);
void MixerOff( void);

BOOL ReadLSensor( void );
BOOL ReadStopRun( void );
void ReadAllSBBIO_Inputs(void);

BOOL ReadHighLevelSensor( void );
BOOL ReadLowLevelSensor( void );
void ReadSBBLinkOptions(void);
//Paraic
void OpenFeed( void );
void CloseFeed( void );
void OpenFeed1( void );
void CloseFeed1( void );
void OpenBypass( void );
void OpenBypass1( void );
void CloseBypass( void );
void CloseBypass1( void );

BOOL ReadContactorInput( void );
BOOL ReadPauseInput( void );

void    SBBSSROutputOn( void );
void    SBBSSROutputOff( void );

void AlarmOutputsOn( void );
void AlarmOutputsOff( void );
void AlarmBeaconOn( void );
void AlarmBeaconOff( void );
void AlarmRelayOn( void );
void AlarmRelayOff( void );
void    StartUpOn( void );
void    StartUpOff( void );
BOOL ReadScrewDetection ( void );
BOOL ReadPauseOffInput( void );
void SwitchComponentsOnOrOff( void );
void SetUpDiagnostics( void );
void PollForOuputDiagnostics( void );
void AllOff( void );
void AllOn( void );
void InitialiseExpansion23S17(void);





typedef struct {
    
    int     m_nAction;
    int     m_nOutput;

} structDaignosticsData;





#endif  // __INITIALISEHARDWARE_H__




