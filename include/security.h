//////////////////////////////////////////////////////
//
// P.Smith                              25/3/10
// added all security based function and definitions
// added fail times for time fail,hash of id fail,sha secret fail
//
// P.Smith                              6/4/10
// added SaveDataInRAM,ReadDataFromRAM,SaveFuntionNosInRAM,ReadnDisplayDataFromRAM

//////////////////////////////////////////////////////



#ifndef __SECURITY_H__
#define __SECURITY_H__

#include <basictypes.h>
#include "General.h"
#include "OneWire.h"

void ReandnDisplayTSMSettingsInFlash( void );
void ReadSHAHashes( void );
void CompareSHAHashes( void );
void CopySecurityDataToModbus( void );
WORD CalculateHashOfTime( int nPrev );
void CheckTimeHash( void );
void Crash2( void );
void SaveDataInRAM( void);
void ReadDataFromRAM( void);
void SaveFuntionNosInRAM( void );
void ReadnDisplayDataFromRAM(void);





void ReadAndStoreSHAInFlash( void );

#define     MAX_TIME_FAIL_IN_SECONDS            (600)//security--tsm--
#define     MAX_HASH_OF_ID_FAIL_IN_SECONDS      (300)//security--tsm--
#define     MAX_SHA_SECRET_FAIL_IN_SECONDS      (900)//security--tsm--

#define		SECURITY_PASS_CODE	                 (0xA1)
#define		TIME_HASH_FAIL_ERROR_CODE			 (0xE1)
#define		ID_HASH_FAIL_ERROR_CODE			     (0xE2)
#define		SHA_SECRET_FAIL_ERROR_CODE	         (0xE3)


struct NV_SettingsStruct
{
/*(
      DWORD DataBaudRate;
      DWORD connect_idle_timeout;
      DWORD listen_idle_timeout;
      DWORD new_connection_timeout;
      DWORD connection_retry_timeout;
      char ConnectName[80];
      char DeviceName[40];
      IPADDR ConnectAddress;
      WORD ListenPort;
      WORD ConnectPort;
      BYTE Output_Bits;
      BYTE Output_Stop;
      BYTE Output_Parity;
      BYTE SerialMode;
      BYTE FlowMode;
      BYTE ConnectMode;
      BYTE IP_Addr_mode;
      BYTE fill;
      char ConnectMessage[80];
      char ConnectLossMessage[80];
      SysInfo SysInfoData;
      WORD BreakInterval;
      BYTE BreakOnConnect;
      BYTE BreakKeyFlag;
      BYTE BreakKeyValue;
	  char UserName[40];
	  char Password[40];
*/
      DWORD m_nVerifyKey;
      char BoardRevision[40];
      BYTE BoardRevNo;

      BYTE m_nSHAID[8];
      WORD m_nWatchDogEnableCode;
      char m_cNBBSerialNo[20];
      WORD m_nHashOfSHAID;			// purposely seperate from SHA
//spare parameters.
      WORD m_nParameter1;
      WORD m_nParameter2;
      WORD m_nParameter3;
      WORD m_nParameter4;
      WORD m_nParameter5;
      WORD m_nParameter6;
      BYTE m_nSpareNVByte[20];
      char m_cSpareNVChars[20];

};


inline void AsmGetLostFunction3( void )
{
// Netburner correspondance.
//	hat is accessing memory at an illegal address via a pointer.
// You might try...

	asm(" move.w #0x2700,%sr "); //Turn off interrupts.
	asm(" move.l #0x4e75,%d0"); //Load the code for RTS instruction
	asm(" movea  0x20008a00,%a0"); //Pick random spot in SRAM
	asm(" move.w %d0,(%a0)"); //Store the RTS.
	asm(" move.l 0x0012340,%d0");//Pick a non mapped location
	asm(" move.l %d0,%a6");//Mess up frame pointer
	asm(" move.l %d0,%a7");//Mess up stack pointer
	asm(" clr.l %d0"); //Remove evidence of our tampering.
	asm(" nop"); //Make sure nothing is cached
	asm(" jmp (%a0)"); //Jump to RTS that will pop bogus stack and crash

	//That should crash in a way that will be hard to figure out.
}

inline void AsmGetLostFunction2( void )
{
	iprintf("\nAsmGetLostFunction Running");

	__asm__ (" move.l #7890,%a6");

}



 #endif   // __SECURITY_H__



