/*-------------------------------------------------------------------
 File: OneWire.h
 Description: 1-wire functions.
 
 -------------------------------------------------------------------*/


#ifndef __ONE_WIRE_H__
#define __ONE_WIRE_H__

extern BYTE g_cDS2432Data[20];   // testpurposes

//DS2432 COMMANDS
#define  READ_ROM          (0x33)
#define  SKIP_ROM          (0xCC)
#define  WRITE_SCRATCHPAD  (0x0F)
#define  READ_SCRATCHPAD   (0xAA)
#define  READ_MEMORY       (0xF0)
#define  LOAD_FIRST_SECRET (0x5A)
#define  COPY_SCRATCHPAD   (0x55)
#define  READ_AUTH_PAGE    (0xA5)

#define  DS2432_PAGE_SIZE  (32)
#define  DS2432_REG_PAGE_SIZE  (8)
#define  DS2432_SCRATCHPAD_SIZE  (8)
#define  DS2432_SECRET_SIZE  (8)
#define  DS2432_ID_SIZE  (10)          //allow for 2 extra bytes, one the calculated CRC.

#define  PAGE_0  (0)
#define  PAGE_1  (1)
#define  PAGE_2  (2)
#define  PAGE_3  (3)

#define  PAGE_0_ADDR  (0x00)
#define  PAGE_1_ADDR  (0x20)
#define  PAGE_2_ADDR  (0x40)
#define  PAGE_3_ADDR  (0x60)
#define  REG_PAGE_ADDR (0x88)

// Data structure for DS2432
typedef struct {
	// Flag to indicate working DS2432
	BOOL	   m_bDS2432Responding;
	// Buffers for each page of memory
	BYTE		m_cPage0Buf[DS2432_PAGE_SIZE];
	BYTE		m_cPage1Buf[DS2432_PAGE_SIZE];
	BYTE		m_cPage2Buf[DS2432_PAGE_SIZE];
	BYTE		m_cPage3Buf[DS2432_PAGE_SIZE];
	// Buffer for scratchpad
	BYTE		m_cScratchpadBuf[DS2432_SCRATCHPAD_SIZE];
	BYTE     m_cTA1Byte;
	BYTE     m_cTA2Byte;
	BYTE     m_cESByte; 
	WORD     m_nCRC16;
	
	// Buffer for secret
	BYTE		m_cSecretBuf[DS2432_SECRET_SIZE];
	BYTE     m_cRegisterPage[DS2432_REG_PAGE_SIZE];
	BYTE		m_cIDBuf[DS2432_ID_SIZE];  // buffer for data stream read from chip, includes Family code & crc.
	BYTE		m_cFamilyCode;
	BYTE		m_cID_CRC;
	
} structDS2432Data;


extern structDS2432Data DS2432Data;

/******************************************************************
 * This function initialises times for standard speed 1-wire (Stantard should be non-zero when called)
 ******************************************************************/
void SetSpeed_1Wire(int standard);

// ******************************************************
// ********* Delay MS Routine *************************
// ******************************************************
void delay_ms(WORD time);

// ********* Delay 1 microsecond  *************************
void Delay_1uS( void );

// ********* Delay n microseconds  *************************
void Delay_uS( WORD nMicroSeconds );

/******************************************************************
 *  Drive 1-wire bus low. 
 *  Drive o/p high to activate FET.
 ******************************************************************/
inline void Drive1WireBusLow( void )   
{
        sim.gpio.podr_datal |= 0x40;         // - set PD6 high.  // need to check bit used on NBB.
}

/******************************************************************
 *  Release 1-wire bus low, allow to be pulled high.
 *  Drive o/p low to turn off FET.
 ******************************************************************/
inline void Release1WireBus( void )   
{
        sim.gpio.podr_datal &= ~0x40;         // - set PD6 low.  // need to check bit used on NBB.
}

//-----------------------------------------------------------------------------
// Generate a 1-Wire reset, return 1 if no presence detect was found,
// return 0 otherwise.
// (NOTE: Does not handle alarm presence from DS2404/DS1994)
int OWTouchReset(void);

//-----------------------------------------------------------------------------
// Send a 1-Wire write bit. Provide 10us recovery time.
//
void OWWriteBit(int bit);

// Read a bit from the 1-Wire bus and return it. Provide 10us recovery time.//
int OWReadBit(void);

// Write 1-Wire data byte
void OWWriteByte(BYTE data);

// Read 1-Wire data byte and return it
BYTE OWReadByte(void);

// Write a 1-Wire data byte and return the sampled result.
BYTE OWTouchByte(BYTE data);

// Write a block 1-Wire data bytes and return the sampled result in the same
// buffer.
void OWBlock(unsigned char *data, int data_len);

// Read and return the page data and SHA-1 message authentication code (MAC)
// from a DS2432.
int ReadPageMAC(int page, unsigned char *page_data, unsigned char *mac);

// *****************************************************
// ********* Function to read a DS2401 *****************
// *****************************************************
void ReadDS2401(void);
//void ReadDS2432ID(void);
void ReadDS2432ID(BYTE *pBuf);
// *****************************************************
// ********* 1-Wire CRC (8bit) Routine *****************
// *****************************************************
BYTE ComputeCRC8(BYTE *msg, WORD msgLen);

// *****************************************************
// ********* DS2432 Routines *****************
// *****************************************************
void WriteDS2432Scratchpad(BYTE *pWriteData, WORD nAddress, int nLen, WORD *pCRC16);
void ReadDS2432Scratchpad(BYTE *pReadData, int nBytes);
bool NewReadDS2432Scratchpad(BYTE *pReadData, int nBytes);
void ReadDS2432Memory(BYTE *pReadData, WORD nAddress, int nBytes);
BYTE LoadFirstSecretDS2432( BYTE *pSecretData );
WORD Calculate1WCRC16(BYTE *pData, int nCount);
void CopyScratchpadDS2432( BYTE *pBuf, WORD nWriteAddress );
void AssembleInputDataCopyScratchpad( BYTE *pBuf );
BYTE ReadAuthPageDS2432(WORD nAddress, BYTE *secret, BYTE *SerNum,
                    BYTE *data, BYTE *challenge);
BYTE GetNBBBoardRevision( void );

// calculation used for the MAC
DWORD KTN (int n);
// calculation used for the MAC
DWORD NLF ( DWORD B,  DWORD C,  DWORD D, int n);
void ComputeSHAEE(BYTE *MT, DWORD *A, DWORD *B,  DWORD *C,  DWORD *D, DWORD *E);

void WriteProtectSecret( void );
BYTE Compare8Bytes(BYTE *pBuf1, BYTE *pBuf2);
void AssembleInputDataCopyScratchpadRegPage( BYTE *pBuf );
void AssembleSecretData( BYTE *pBuf, BYTE *pIDBuf );
bool TestSHAFunction( void );
#endif //__ONE_WIRE_H__
