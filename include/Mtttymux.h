//////////////////////////////////////////////////////
// GenerateRollReport.h
//
// Include file for the roll report generator.
//
//
// M.Parks                              11-05-2000
// First Pass.
// M.McKiernan                              27-02-2004
//      Added widths & sheet tube to roll report structure. Added trim fed back to layer struct.
//
// P,Smith                                  1-07-2005
// Generate roll report template
//
// P.Smith                                  13/07/05
// Define order report structure.
//
// P.Smith                                  7/10/05
// added structBatchReport & structBatchComponentReport
//
// P.Smith                      2/2/06
// first pass at netburner hardware conversion.
// Bool-> BOOL
//
// P.Smith                      15/5/07
// added RestartPanel1
//////////////////////////////////////////////////////

//*****************************************************************************
// MACROS
//*****************************************************************************
#ifndef __MTTTYMUX_H__
#define __MTTTYMUX_H__

//*****************************************************************************
// INCLUDES
//*****************************************************************************

#include "General.h"
#include <basictypes.h>


void SetupMttty( void );
void RestartPanel( void );
void RestartPanel1( void );




   
  
#endif      //__MTTTYMUX_H__
