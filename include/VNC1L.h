/*-------------------------------------------------------------------
 File: VNC1L.h
 Description: VNC1L (USB) function definitions and descriptions.
 
 P.Smith         16/10/08
 make m_nBytesToWrite unsigned int
 
 change buffer size to 10100 (VNC1L_BUFFER_SIZE)
 -------------------------------------------------------------------*/

#ifndef __VNC1L_H__
#define __VNC1L_H__

#define VNC1L_DATA_REGISTER   (0)
#define VNC1L_STATUS_REGISTER   (1)
#define VNC1L_BUFFER_SIZE     (10100)      //size of both tx and rx buffers.
#define VNC1L_PIT_FREQUENCY   (3000)      // Frequency of PIT than handles VNC1L transactions.

// Messages or commands for Command Q for VNC1L.   
#define VNC1L_PTP     (1)      //poll till prompt.
#define VNC1L_SCS     (5)      // Short Command Set.
#define VNC1L_OPW     (10)      // open file for write.
#define VNC1L_WRF     (15)      // write data to file
#define VNC1L_WKD     (16)      // WAKE UP DISK
#define VNC1L_CLF     (20)      // close file
#define VNC1L_DIR     (30)      // DIR
#define VNC1L_DIRF    (31)      // DIR . file
#define VNC1L_DIR_REPLY (33)   // Response to read directory.

#define VNC1L_OPR     (40)      // Open file for read
#define VNC1L_RDF     (45)      // read file
#define VNC1L_RDF_REPLY (47)    // read file response....i.e. data.

#define VNC1L_DLF     (50)      // delete file

#define VNC1L_IPA      (101)     // Input mode Ascii
#define VNC1L_SC0      (102)     // set current device to 0.
#define VNC1L_DSD      (105)     // device send data command.


   
#define VNC1L_TERMINATOR (0xAA)      // finished.
#define VNC1L_TIMEOUT_ERROR (1)    // timeout occurred during transaction.

// Data structure for VNC1L data transmission
typedef struct {

	// Flag to show if VNC1L is occupied.
   bool  m_bBusy;
   bool  m_bReading;    // set if reading, clear if writing.
   
	int	m_nTimeoutCounter;

    unsigned int   m_nBytesToWrite;  // this is the number of data bytes to write to a file.
	BYTE	m_cTxBuffer[VNC1L_BUFFER_SIZE];
	int		m_nTxIndex;
	int		m_nTxBytes;
	bool	m_bTXEnable;

	BYTE	m_cRxBuffer[VNC1L_BUFFER_SIZE];
	int		m_nRxIndex;
	bool	m_bRxEnable;
	int		m_nBytesToRead;			// no. of bytes to read from file.
	int      m_nBytesRead;           // no. of bytes actually read.
	int		m_nRxBytes;

   char  m_cFileName[12];        // currently limited to 8.3 size.
      
	// Flag to stop/Start 
	bool	m_bStart;
	
	bool m_bPageFeed;


	// Pointer to the data to be written
	BYTE	*m_pWriteData;
	// Pointer to buffer for the data read
	BYTE	*m_pReadData;
	int		m_nTimeoutPreset;		// starting value for the timeout

	bool	m_bActive;
	int		m_nRetries;
	int		m_nPollTimer;

	bool	m_bRunStartDelay;
	
	// error flag
	int     m_nErrorCode;
	
	bool    m_bTransactionFinished;
	bool    m_bTransactionInProgress;

   bool    m_bReadInProgress;
   bool    m_bWriteInProgress;
   
   bool    m_bCommandInProgress;
   bool    m_bPromptReceived;
      
   // Q of messages in transaction
   BYTE    m_cCommandQ[20];
   int     m_nCommandQIndex;

} structVNC1LData;



/******************************************************************
 * This function will Write data...MAX 16 BYTES.
 
 ******************************************************************/
void VNC1L_WRITE16(WORD chipSelectUsed, WORD TxLen, WORD * ptrTxBuffer);

/******************************************************************
 * This function will Read data...MAX 16 BYTES.
 
 ******************************************************************/
void VNC1L_READ(WORD chipSelectUsed, BYTE TxLen);

/******************************************************************
 * This function will Read 1 byte
 
 ******************************************************************/
WORD VNC1LRead1Byte(WORD chipSelectUsed, BYTE RegisterType);


/******************************************************************
 * This function will Write a single byte 
 
 ******************************************************************/
WORD VNC1LWrite1Byte(WORD chipSelectUsed,  BYTE TxData, BYTE RegisterType );

/******************************************************************
 * This function will Write a single byte - repeated tries until returned status OK.
 
 ******************************************************************/
void VNC1LWriteByte( BYTE DataByte ); 

/******************************************************************
 * This function reads the VNC1L to see if there is any data available.
 
 ******************************************************************/
void CheckForVNC1LData( void );

void SetVNC1LMode_SCS( void );
void VNC1L_OpenFileForWrite( char * Filename );
void VNC1L_CloseFile( char * Filename );
void VNC1L_WriteDataToFile( BYTE * pData,  int nLen );
void TransmitVNC1L(int fd0, BYTE* pVNC1LWriteData, int nWriteLen);
//void TransmitVNC1L(BYTE * pVNC1LWriteData, int nWriteLen);
int ReadDataFromVNC1L(  int nSecs );
void WriteToUSBFlashDisk( void );
void VNC1L_ReadDataFromFile( int nLen );

void RestoreU0MttySettings( void );
void RestoreU0VNC1LSettings( void );
void VPrintf( BYTE * pData, int nBytes );
void VNC1LReadDir( void );
// Open file for read
void VNC1L_OpenFileForRead( char * Filename );
int ReadDataFromVNC1LSPIMode( int nSecs );

void WriteToUSBFlashDiskSPIMode( void );
void SetVNC1LMode_SCS_SPI( void );
void VNC1LReadDir_SPI( void );
void VNC1L_OpenFileForWrite_SPI( char * Filename );
void VNC1L_WriteDataToFile_SPI( BYTE * pData,  int nLen );
void VNC1L_CloseFile_SPI( char * Filename );
void VNC1L_OpenFileForRead_SPI( char * Filename );
void VNC1L_ReadDataFromFile_SPI( int nLen );
void TransmitVNC1LSPIMode(BYTE* pVNC1LWriteData, int nWriteLen);
int PollVNC1LUntilPrompt_SPI( int nSecs );
void VNC1LISRHandler( void );

void VNC1LPITISRHandler( void );
void AssembleVNC1LCommand(BYTE nCommand );
void StartVNC1LFileWrite( int nWriteLen, BYTE * pDataToWrite, char * FileName);
void InitialiseVNC1lDataStructure( void );
void PrintVNC1LDiagnostics( void );
void StartVNC1LDirRead(  BYTE * pDirData );

void PrintDataUsingVNC1L( int nWriteLen, BYTE * pDataToWrite, bool bPageFeed );
void ResetVNC1LAndWaitForPrompt( void );
#endif // __VNC1L_H__


