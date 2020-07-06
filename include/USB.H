//////////////////////////////////////////////////////
// Vaccyc.h
//
// P.Smith                        9-9-2005
// First pass
//
// P.Smith                        22/6/07
// added CheckForCatchBoxVibration
//
// P.Smith                        22/1/08
// added USBDataLogging, PollForUSBWrite,USBDataLogging1
//
// P.Smith                        20/3/08
// added ReadFilesFromSDCardAndSaveToUSB
// added ReadSDCardDirectory,CopyFileFromSdToUSB, ReadSDFileAndStartUSBWrite
//

//////////////////////////////////////////////////////

#ifndef __USB_H__
#define __USB_H__

void InitialiseUSB( void );
void USBDataLogging( void );
void PollForUSBCopy( void );
void ReadFilesFromSDCardAndSaveToUSB( void );
void ReadSDCardDirectory( void );
void CopyFileFromSdToUSB( int nFileNo );
void ReadSDFileAndStartUSBWrite( int nFileNo );



#endif   // __USB_H__


