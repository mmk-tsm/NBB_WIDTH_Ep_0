//////////////////////////////////////////////////////
// P.Smith                      25/11/08
// added TelnetClearComms,ShowTestResults;
//
//
// P.Smith                      8/12/08
// added void ShowOngoingCommsHistory,ShowHourlyCommsHistory,ShowOngoingEventsHistory
// ShowHourlyEventsHistory,ShowOngoingComponentEventsHistory,ShowHourlyComponentEventsHistory( void );
// added continuous update defn
//
// P.Smith                      9/12/08
// added ShowOngoingWeighingevents,ShowHourlyWeighingevents
//
// P.Smith                      11/12/08
// remove inStr
//
// P.Smith                      18/5/09
// added ShowPrompt
//
// P.Smith                      26/6/09
// added LoaderDebugOn, LoaderDebugOff
//
// P.Smith                      20/1/10
// added CheckForTelnetCommands
//
// P.Smith                      7/4/10
// added ShowSecurityResults
//////////////////////////////////////////////////////

#ifndef __TELNETCOMMANDS_H__
#define __TELNETCOMMANDS_H__

#define   TELNET_USERNAME_ENTRY   1
#define   TELNET_PASSWORD_ENTRY   2



void ProcessTelnetCommand( char *cCommand,int n );

void ShowFile( char *cFilename );
void ReadSDCardDirectoryForTelnet( void );
void Parse_Commands( char *cCommand );
void ShowHelpMenu( void );
void ShowUserNamePrompt( void );
void ShowCurrentLog( void );
void Showinfo( void );
void ShowAlarms( void );
void ShowBlenderInfoPage( void );
void NetworkWritesOn( void );
void NetworkWritesOff( void );
void ShowHopperCalibrationPage( void );
void CheckForTelNetTimeout( void );
void TerminateTelnetSession( void );
void ContinuousTelnetUpdate( void );
void UpdateHopperCalibration( void );
void TerminateContinuousUpdate( void );
void ShowFileInSegments( char *cFilename );
void ShowFileSegment( char *cFilename, BOOL bShowAll );
void ShowCommunications( void );
void TelnetClearComms( void );
void ShowTestResults( void );
void ShowOngoingCommsHistory( void );
void ShowHourlyCommsHistory( void );
void ShowOngoingEventsHistory( void );
void ShowHourlyEventsHistory( void );
void ShowOngoingComponentEventsHistory( void );
void ShowHourlyComponentEventsHistory( void );
void ShowOngoingWeighingevents( void );
void ShowHourlyWeighingevents( void );
void ShowPrompt( void );
void LoaderDebugOn( void );
void LoaderDebugOff( void );
void CheckForTelnetCommands( void );
void ShowSecurityResults( void );








#define   HOPPERCALIBRATIONPAGE             1
#define   COMMUNICATIONSCOUNTERS            2
#define   BLENDERINFORMATIONPAGE            3
#define   ONGOINGCOMMUNICATIONSHISTORY      4
#define   HOURLYCOMMUNICATIONSHISTORY       5
#define   ONGOINGEVENTSHISTORY              6
#define   ONGOINGHOURLYEVENTSHISTORY        7
#define   ONGOINGCOMPONENTEVENTSHISTORY     8
#define   HOURLYCOMPONENTEVENTSHISTORY      9
#define   ONGOINGWEIGHINGEVENTSHISTORY      10
#define   HOURLYWEIGHINGEVENTSHISTORY       11














#endif  // __TELNETCOMMANDS_H__


