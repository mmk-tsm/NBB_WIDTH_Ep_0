//////////////////////////////////////////////////////
// ProgLoop.h
//
// Allows the user to set the current time and date.
//
// M.McKiernan                          23-07-2004
// First pass

// P.Smith                              31-03-2005
// ConvertBatchSetpointstoImperial added
//

// P.Smith                              16-08-2005
// added void CheckForBinPresence ( void );       

// P.Smith                              19-09-2005
// added void TransferSetPercentagesFromComms( void )


// P.Smith                              4/10/05
// add void ResetAllFillAlarms ( void )       
// added void CheckChecksum( void); 
//
// P.Smith                              12/2/08
// added CheckCalibrationDataChecksum
//////////////////////////////////////////////////////

#ifndef __PROGLOOP_H__
#define __PROGLOOP_H__

void ProgramLoop( void );
void CopySeqTableToPrevious( void );
void CheckTableForNewComponent( void );
void CalculateDToAPercentage( void );
void ConvertBatchSetpointstoImperial( void);
void CheckForBinPresence ( void );       
void TransferSetPercentagesFromComms( void );
void ResetAllFillAlarms ( void );       
void CheckChecksum( void); 
void CheckCalibrationDataChecksum( void );



#endif  // __PROGLOOP_H__


