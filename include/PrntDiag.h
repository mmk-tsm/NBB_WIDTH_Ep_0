//////////////////////////////////////////////////////
// PrntDiag.h
//
// Prints the cycle diagnostics
//
// Entry:   Void.
//
// Exit:    Void
//
// M.McKiernan                          15-12-2000
// First Pass. - edited from PrintConfiguration.h of the CP2000
//
// P.Smith                          9-11-2005
// added void PrintResetSource( void )
//
//
// P.Smith                      1/6/06
// first pass at netburner hardware conversion.
// BooL -> BOOL
//
// P.Smith                      20/3/08
// added PrintControlDiagnostics
//
// P.Smith                      14/11/08
// add pointer to PrintModbusMessage added PrintModbusTCPMessage
//
// P.Smith                      15/1/09
// added WriteLogDataToUSB
//
// P.Smith                      1/9/09
// added RecordChangedModbusData
//////////////////////////////////////////////////////

//*****************************************************************************
// MACROS
//*****************************************************************************
#ifndef __PRNTDIAG_H__
#define __PRNTDIAG_H__


//#define   CONFIGURATION_REPORT_PRINT_BUFFER_SIZE      1700

BOOL    PrintCycleDiagnostics( void );
BOOL PrintSystemConfiguration( void );
void CSVVersion( void );
void PrintResetSource( void );
void PrintModbusMessage( unsigned char *Rxbuff );
void PrintControlDiagnostics( void );
void PrintModbusTCPMessage( unsigned int start_addr, unsigned int length_in_bits );
void WriteLogDataToUSB( void );
void RecordChangedModbusData(WORD wModbusAddress,WORD wOld,WORD wNew,BOOL bIsPanel);



#endif  //__PRNTDIAG_H__
