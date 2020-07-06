//////////////////////////////////////////////////////
// M. McKiernan
// USBDisk.h
//////////////////////////////////////////////////////

#ifndef __USB_DISK_H__
#define __USB_DISK_H__

void TransferFileSegment( char *cFileName, int nSegmentNo );
void HandleFileTransferFromSD( void );
bool StartFileTransferFromSD( char * cFileName);


#endif  // __USB_DISK_H__


