//////////////////////////////////////////////////////
// PrintConfiguration.h
//
// Prints the system configuration.
//
// Entry:	Void.
//
// Exit:	Void
//
// M.Parks								04-08-2000
// First Pass.
// M.McKiernan	26-06-02 	Reduced config_report buffer size to 1400.

// P.Smith                      26/1/06
// first pass at netburner hardware conversion.
// Bool -> BOOL
// included  <basictypes.h>


//////////////////////////////////////////////////////

//*****************************************************************************
// MACROS
//*****************************************************************************
#ifndef __PRINTCONFIGURATION_H__
#define __PRINTCONFIGURATION_H__
#include <basictypes.h>


// Empirically measured as 1528 bytes in English with
// 32 units on 08-11-2000
//#define	CONFIGURATION_REPORT_PRINT_BUFFER_SIZE		1700
#define	CONFIGURATION_REPORT_PRINT_BUFFER_SIZE		1400

BOOL	PrintConfiguration( void );


#endif	//__PRINTCONFIGURATION_H__
