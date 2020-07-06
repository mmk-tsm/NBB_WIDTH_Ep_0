/*-------------------------------------------------------------------
 File: VNC1L_G.h
 Description: VNC1L (USB) function definitions and descriptions.
 
 P.Smith                                   11/12/08
 added    m_bStartOrderReportLogWrite &  char  m_cOrderReportFileName[12];       
 
 P.Smith                                   15/1/09
 use USBFILENAMESTRINGLENGTH to define the name length.     
 
 -------------------------------------------------------------------*/

#ifndef __VNC1L_GH__
#define __VNC1L_GH__

   
#define USB_READ_BUF_SIZE  (100)
#define PRINT_BUF_SIZE     (130)
#define USB_TX_BUF_SIZE    (10000)

//#define MAX_LINE_SIZE      (70)
#define MAX_USB_BLOCK_SIZE (30)
// 25.11.2008 - changed to make compatible with what was working in NBB printer version.
#define MAX_LINE_SIZE      (80)
//#define MAX_USB_BLOCK_SIZE (50)  // 25.11.08 - didnt work with 50!!!!!!!!


//VNC1L commands - short command mode - 2 byte commands, the 0D gets added in function.
#define CMD_SCS 0x10
#define CMD_IPH 0x91

#define CMD_VST 0x20
#define CMD_V3A 0x21
#define CMD_VRR 0x8F
#define CMD_VRA 0x8A
#define CMD_VSF 0x25
#define CMD_VSB 0x26

#define CMD_PGS 0x81    // printer get status.
#define CMD_VP 0x8B
#define CMD_VF 0x8C
#define CMD_VB 0x8D
#define CMD_VSV 0x88

#define CMD_SCD1 0xE1      // set current devide to 1
#define CMD_SCD2 0xE2      // set current devide to 2

#define CMD_CR 0x0d

#define CMD_WKD 0x16    // wake up disk
#define CMD_OPW 0x09    // open file fro write
#define CMD_OPR 0x0E    // open file for read
#define CMD_WRF 0x08    // write data to file
#define CMD_SEK 0x28    // seek to offset in file

#define CMD_CLF 0x0A    // close
#define CMD_RDF 0x0B    // read data from open file
#define CMD_DLF 0x07    // delete file
#define CMD_DIRF 0x01    // dir file

#define CMD_E  0x45     // E or echo, used for synchronisation.

#define USB_MODE_PRINTER      (FALSE)
#define USB_MODE_FLASH_DISK   (TRUE)

#define USB_FLASH_BLOCK_SIZE  (256)
#define USB_READ_BLOCK_SIZE       (256)
#include "General.h"



/*
char   *strShortCmdSet = "SCS\x0D";
char   *strQueryPort1 = "\x2B\x0D";  	             //query port 1
char   *strQueryPort2 = "\x2C\x0D";	                //query port 2
char   *strDeviceSet1 = "\x86,\x20,\x00,\x0D";	   // SET CURRENT PORT1
char   *strDeviceSet2 = "\x86,\x20,\x01,\x0D";	   // SET CURRENT PORT2
char   *strDeviceSendData = "\x83,\x20\x04\x0D";		// dsd  string	   // 83 20 byte 0D data
//                        Note 3rd char is no. of data bytes - initialise to 4 - change based on data bytes.
*/

// Data structure for USB (VNC1L) data transmission
typedef struct {

	// Flag to show if VNC1L has been detected online
	bool  m_bSCSMode;
	bool  m_bDiskWokeUp;
   bool  m_bOnlineDetected;
   bool  m_bDevicePresentP1;    // set if device detected on port P1 (cleared by device removed P1)
   bool  m_bDevicePresentP1Transition; // set when device first detected.
   
   bool  m_bDevicePresentP2;    // set if device detected on port P2 (cleared by device removed P2)
   bool  m_bDevicePresentP2Transition; // set when device first detected.
   bool  m_bUSBDriveReady;
   
   bool  m_bUSBInitialised;
   bool  m_bPrinterInitialised;
   bool  m_bPromptReceived;
   
   bool  m_bPrinterFound;
   bool  m_bPrinterReady;
   BYTE  m_nPrinterStatus;
   bool  m_bPaperEmpty;
   
   char * m_pDataToPrint;     //pointer to data which should be printed.
   bool  m_bPrintRequest;
   
   
   int   m_nCharsRead;
   int   m_nBytesReceived;
      
	char	m_cReadBuf[USB_READ_BUF_SIZE];
	BYTE	m_ucReadBuf[USB_READ_BUF_SIZE];     //byte version.
	
   char	m_cTxBuffer[USB_TX_BUF_SIZE];
   
   int   m_nTxIndex;
   bool  m_bPrintFinished;
   bool  m_bPrintInProgress;
   int   m_nCharsInLine;
   
	int	m_nTimeoutCounter;

   int   m_nBytesToWrite;  // this is the number of data bytes to write to a file.
//	BYTE	m_cTxBuffer[USB_BUFFER_SIZE];
	int	m_nTxBytes;
   bool  m_bFlashWriteFinished;
   bool  m_bFlashWriteInProgress;
   int   m_nTransmittedBytes;
   char * m_pWrData;
   char * m_pRdData;
   char  m_cFileName[12];        // currently limited to 8.3 size.
	
	
	bool	m_bTXEnable;

//	BYTE	m_cRxBuffer[USB_BUFFER_SIZE];
	int		m_nRxIndex;
	bool	   m_bRxEnable;
	int		m_nBytesToRead;			// no. of bytes to read from file.
	int      m_nBytesRead;           // no. of bytes actually read.
	int		m_nRxBytes;

      
	// Flag to stop/Start 
	bool	m_bStart;
	
	bool m_bPageFeed;
	
   int   m_nReadIndex;
   bool  m_bFlashReadFinished;
   bool  m_bFlashReadInProgress;
   bool  m_bEchoReceived;
   bool  m_bGTCharReceived;
   
// for diagnostics   
   bool  m_bStartDiagWrite;         // Start Diagnostics write (i.e when set Write to USB needs to be started)
   char  m_cDiagFileName[USBFILENAMESTRINGLENGTH];        // Diagnostics file name currently limited to 8.3 size.
// for Event Log   
   bool  m_bStartEvLogWrite;         // Start Event log write (i.e when set Write to USB needs to be started)
   char  m_cEvLogFileName[USBFILENAMESTRINGLENGTH];        // Event log file name currently limited to 8.3 size.
// for order report   
   bool  m_bStartOrderReportLogWrite;       // Start Order Report log write (i.e when set Write to USB needs to be started)
   char  m_cOrderReportFileName[USBFILENAMESTRINGLENGTH];        // Event log file name currently limited to 8.3 size.

// for Transferring from sD card   
   bool  m_bStartXfrWrite;         // Start Transfer write (i.e when set Write to USB needs to be started)
   char  m_cXfrFileName[USBFILENAMESTRINGLENGTH];        // Transfer file name currently limited to 8.3 size.
   int   m_nXfrBytes;               // no. of bytes to transfer.

   bool m_bUSBMode;               // Printer or Flash_Disk.
      
} structUSB;




void USB_Init( void );
bool ReadVNC1LStatus( void );
void USB_Query( void );
int USB_Read(void);
void USB_WriteCommand(  BYTE * pData );
void USB_Write(  char * pData, int nLen );
void USB_Print(  char * pData  );
void Init_Printer( void );
void USB_Send_Print(  char * pCommand, int nChars  );

//int Search_String(char cBuf, int nBufLen, char search_string);
int inStr(char *pattern, char *str) ;
void ReadAndParseUSBResponse( void );
void InitialiseUSBDataStructure( void );
void TestPrinting( void );
void HandleUSBPrintJob( void );
void StartUSBPrint( char * pData, bool AppendPageFeed );
void MonitorPrinterPort10Hz( void );
void ResetVNC1L( void );
void FlushVNC1LTransmitter( void );


void USBCmdSend(char Cmd);
void USBSendFileCommands(char Cmd, char * cFileName, char * pData, int nLen);

bool StartUSBFlashWrite( char * pData, char * cFileName, int nLen );
void HandleUSBFlashWriteJob( void );
int USB_ReadData(char * pData, int nLen );
void StartUSBFlashRead( char * pData, char * cFileName, int nLen, int nOffset );
void HandleUSBFlashReadJob( void );
void DeleteUSBFile( char * cFileName);
long GetUSBFileSize( char * cFileName );
void SynchroniseUSB(void);

void SetUSBMode( bool bMode );
void ScheduleUSBPrint( char * pData );
void GetPrinterStatus( void );

#endif // __VNC1L_GH__


