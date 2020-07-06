///////////////////////////////////////////////////////////
// PrinterFunctions.h
//
// Functions for initialising and controlling the parallel
// printer port.
//
//
// M.Parks									12-05-2000
// First pass
//
//	M.McKiernan								15/12/2004
// Defined PRINT_BUFFER_SIZE  & MAX_PRINT_WIDTH
// EnableSerialPrint( void ) SerialPrintHandler(), CopyFloatDataJustified(), ClearLine()

// P.Smith                      26/1/06
// first pass at netburner hardware conversion.
// BooL -> BOOL
// included  <basictypes.h>
// added #ifndef __PRINTFUNCTIONS_H__
//       #define __PRINTFUNCTIONS_H__
//
// P.Smith                      28/1/06
// increased print buffer size to 5000
//
// P.Smith                      25/2/08
// increase MAX_PRINT_WIDTH to 200 , was 80
//
// P.Smith                      2/5/08
// increase PRINT_BUFFER_SIZE to 10000
///////////////////////////////////////////////////////////
#ifndef __PRINTFUNCTIONS_H__
#define __PRINTFUNCTIONS_H__

#include <basictypes.h>



// Functions available to the programmer
void	InitialiseParallelPrinter( void );
void	EnableParallelPrint( void );
BOOL	CanWePrint( void );

int CopyFloatDataJustified( unsigned char *Buf, double fData, int nDigits, int nDecimals, int nColumn, int nJustification, int nUnits );
void ClearLine( int nLength );

// These functions should not be called directly
void	ParallelPrintHandler( void );
void	PrintSingleCharacter( char cValue );
BOOL	IsPrinterBusy( void );
BOOL	PollPrinterAck( void );
void	StrobeData( void );
void EnableSerialPrint( void );
void SerialPrintHandler( void );

	
#define	MAX_PRINT_WIDTH		200		//i.e. was 80 columns.
// Parallel printer interface command bits
#define	PARALLEL_COMMAND_STROBE_LOW		0x01
#define	PARALLEL_COMMAND_STROBE_HIGH	0xfe
 
#define	PARALLEL_COMMAND_AUTOFDXT		0x02
#define	PARALLEL_COMMAND_INIT			0x04
#define	PARALLEL_COMMAND_SLCTIN			0x08
#define	PARALLEL_COMMAND_INTP			0x10

// Parallel printer interface control bits
#define	PARALLEL_CONTROL_STROBE			0x01
#define	PARALLEL_CONTROL_AUTOFDXT		0x02
#define	PARALLEL_CONTROL_INIT			0x04
#define	PARALLEL_CONTROL_SLCTIN			0x08

#define	PARALLEL_CONTROL_INTP_ENABLE	0x10
#define	PARALLEL_CONTROL_INTP_DISABLE	0xef

#define	PARALLEL_CONTROL_BIDEN			0x20

// Parallel printer interface status bits
#define	PARALLEL_STATUS_INTERRUPT		0x04
#define	PARALLEL_STATUS_ERROR			0x08
#define	PARALLEL_STATUS_SELECT			0x10
#define	PARALLEL_STATUS_PE				0x20
#define	PARALLEL_STATUS_ACK				0x40
#define	PARALLEL_STATUS_BUSY			0x80

#define	PRINT_BUFFER_SIZE		10000

#endif	//__PRINTFUNCTIONS_H__
