
//////////////////////////////////////////////////////
// P.Smith                      25/3/10
// added ReadSHAHashes,ReadAndStoreSHAInFlash,CopySecurityDataToModbus
// CalculateHashOfTime,CheckTimeHash,Crash2
//
// P.Smith                      6/4/10
// added SaveDataInRAM,ReadDataFromRAM,SaveFuntionNosInRAM,ReadnDisplayDataFromRAM
//
// P.Smith                      15/4/10
// added check for g_wPlatFormInUse to determine what address should be used to
// save the function number.
//////////////////////////////////////////////////////


#include <stdio.h>

#include "SerialStuff.h"
#include "BatchMBIndices.h"
#include "BatchCalibrationFunctions.h"
#include "General.h"
#include "ConversionFactors.h"

#include "16R40C.h"
#include "ConfigurationFunctions.h"
#include "MBMHand.h"
#include "MBSHand.h"

#include "MBProgs.h"
#include "SetpointFormat.h"
#include "TimeDate.h"
#include "BatVars.h"
#include "BBCalc.h"
#include "MBTogSta.h"
#include "Alarms.h"
#include "KghCalc.h"
#include "BatVars2.h"
#include "Monbat.h"
#include "Blrep.h"
#include "MBProsum.h"
#include "Pause.h"
#include "CycleMonitor.h"
#include "TSMPeripheralsMBIndices.h"

#include <basictypes.h>
#include <string.h>
#include "NBBgpio.h"
#include "ConversionFactors.h"
#include "MultiblendMB.h"
#include "Mtttymux.h"
#include "Secret.h"
#include "security.h"
#include <system.h>
#include "Hash.h"


extern CalDataStruct    g_CalibrationData;
extern  BYTE DS2432Buffer[];
extern const char *PlatformName;
extern WORD g_wPlatFormInUse;

NV_SettingsStruct NV_Settings;

WORD g_nHashOfSHAID = 0;
WORD g_nHashOfSHAIDInFlash = 0;
WORD g_nSHASecretFailCtr = 0;
WORD g_nSHAHashOfIdFail = 0;
WORD g_nTimeHashFail = 0;
WORD g_nHashOfTime;

WORD SRAMData[2] __attribute__( ( aligned( 16 ) ) );
WORD *SRAMDataP = SRAMData;


//////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////


/*-------------------------------------------------------------------
 Reads SHA ID and stores in module flash.
 ------------------------------------------------------------------*/
void ReadSHAHashes( void )
{
	NV_SettingsStruct TSMSettings;
   NV_SettingsStruct *pData = ( NV_SettingsStruct * ) GetUserParameters();
   TSMSettings = *pData;

   g_nHashOfSHAIDInFlash = TSMSettings.m_nHashOfSHAID;
   BYTE nSHAIDBuf[10];
   BYTE *pSHAIDBuf = nSHAIDBuf;

   //First read board ID:
	 ReadDS2432ID( pSHAIDBuf );       //read SHA code from DS2432
	 g_nHashOfSHAID = TSMHashS( (char*)nSHAIDBuf, 7);	// nb hash is on 1st 7 bytes, not including crc.
}



/*-------------------------------------------------------------------
 Reads SHA ID and stores in module flash.
 ------------------------------------------------------------------*/
void CompareSHAHashes( void )
{
	g_nHashOfTime = CalculateHashOfTime(0 );        // call like this


	if(g_nHashOfSHAIDInFlash == g_nHashOfSHAID )
	{
		g_nSHAHashOfIdFail = 0;
	}
	else
	{
		g_nSHAHashOfIdFail++;
	}
}


//*-------------------------------------------------------------------
// Reads SHA ID and stores in module flash.
// ------------------------------------------------------------------*/
void ReadAndStoreSHAInFlash( void )
{
	int i;
	NV_SettingsStruct TSMSettings;
   NV_SettingsStruct *pData = ( NV_SettingsStruct * ) GetUserParameters();
   TSMSettings = *pData;

   BYTE nSHAIDBuf[10];
   BYTE *pSHAIDBuf = nSHAIDBuf;
   WORD nHashOfSHAID;

   //First read board ID:
	 ReadDS2432ID( pSHAIDBuf );       //read SHA code from DS2432

	 nHashOfSHAID = TSMHashS( (char*)nSHAIDBuf, 7);	// nb hash is on 1st 7 bytes, not including crc.
   if ( 1 )
   {

//ID from sHA chip (DS2432).
      for(i=0; i<8; i++)
      {
    	  TSMSettings.m_nSHAID[i] = nSHAIDBuf[i];
      }

	  TSMSettings.m_nHashOfSHAID = nHashOfSHAID;
      SaveUserParameters( &TSMSettings, sizeof( TSMSettings ) );
   }

}



//*-------------------------------------------------------------------
//
// ------------------------------------------------------------------*/
void CopySecurityDataToModbus( void )
{
    g_arrnMBTable[BATCH_SUMMARY_SHA_SECRET_FAIL_CTR] = g_nSHASecretFailCtr;
    g_arrnMBTable[BATCH_SUMMARY_HASH_OF_ID_COMPARE_FAIL] = g_nSHAHashOfIdFail;
    g_arrnMBTable[BATCH_SUMMARY_HASH_OF_TIME_FAIL] = g_nTimeHashFail;
    g_arrnMBTable[BATCH_SUMMARY_HASH_OF_SHA_IN_FLASH] = g_nHashOfSHAIDInFlash;
    g_arrnMBTable[BATCH_SUMMARY_HASH_OF_SHA_CALCUALTED] = g_nHashOfSHAID;
    if(g_nHashOfSHAIDInFlash == g_nHashOfSHAID)
    {
    	g_arrnMBTable[BATCH_SUMMARY_HASH_COMPARISON] = 0x01;
    }
    else
    {
    	g_arrnMBTable[BATCH_SUMMARY_HASH_COMPARISON] = 0;
    }
}


WORD CalculateHashOfTime( int nPrev )
{
	DWORD nSecondNo;

	BYTE nTimeBuf[3];

	WORD nHashOfTime;

/*
	g_CurrentTime[TIME_HOUR] = 6;
	g_CurrentTime[TIME_MINUTE] = 59;
	g_CurrentTime[TIME_SECOND] = 2;
*/

	nSecondNo = g_CurrentTime[TIME_HOUR]*3600 + g_CurrentTime[TIME_MINUTE]*60 + g_CurrentTime[TIME_SECOND];
	if(nPrev != 0)
	{
		if(nSecondNo >= nPrev)
				nSecondNo -= nPrev;
		else
			nSecondNo = nSecondNo+86400-nPrev;	// day roll-over.
	}


    nTimeBuf[2] = (BYTE)( nSecondNo>>16 & 0x000000FF);
    nTimeBuf[1] = (BYTE)( nSecondNo>>8 & 0x000000FF);
    nTimeBuf[0] =  (BYTE)(nSecondNo & 0x000000FF);

    nHashOfTime = TSMHashS((char*)nTimeBuf, 3);

    //testonly
//    if(fdTelnet>0)
//    {
//		iprintf("\n H:M:S= %02d:%02d:%02d",g_CurrentTime[TIME_HOUR],g_CurrentTime[TIME_MINUTE], g_CurrentTime[TIME_SECOND]);
//   		iprintf(" nSecondNo= %08x  Buf2= %02x Buf1= %02x Buf0= %02x nHashOfTime= %04x", nSecondNo, nTimeBuf[2], nTimeBuf[1], nTimeBuf[0], nHashOfTime );
//    }
	return(nHashOfTime);
}

void CheckTimeHash( void )
{
int i=0;
bool bOk = FALSE;
WORD nHash;

	for(i=0; i<3&&!bOk; i++)
	{
		nHash = CalculateHashOfTime( i );
		if(nHash == g_nHashOfTime )
				bOk = TRUE;
	}

	if(!bOk)
		g_nTimeHashFail++;

//    if(fdTelnet>0)
//		iprintf("\n H:M:S= %02d:%02d:%02d",g_CurrentTime[TIME_HOUR], g_CurrentTime[TIME_MINUTE], g_CurrentTime[TIME_SECOND]);
//    		iprintf("   g_nTimeHashFail= %d", g_nTimeHashFail );

}

/*
 Crash2()  From Netburner ticket - 22.3.2010
This crash will give you a nice trap report with interesting stuff in
the trap dump, both the basic monitor trap dump and the smart trap dump.
If you think the DEADBEEF in the smart trap dump is too in your face
change the constant 21524110 If some one searches the disassembled
source code for DEADBEEF the instructions or the ram address they won't
find it.
*/

void Crash2( void )
{
asm(" move.w #0x2700,%sr "); //Turn off interrupts.
asm(" move.l #60,%d1");
asm(" move.l #1,%d2");
//asm(" move.l #0x21524110,%d0");//11011110101011011011111010101111  (DEADBEEF when inverted)
asm(" move.l #0x23524017,%d0");//11011110101011011011111010101111
asm(" not.l %d0");
asm(" move.l %a6,%d6");
asm(" move.l %a6,%d5");
asm("overfl1:");
asm(" move.l %d5,(%a6)+");
asm(" move.l %d0,(%a6)+");
asm(" addq.l #4,%d0");		//
asm(" addq.l #4,%d5 ");
asm(" sub.l %d2,%d1");
asm(" bne overfl1");
asm(" move.l %d6,%a6");
asm(" move.l #0xDFFF0BE7,%d0"); // 2000F418 spot in SRAM
asm(" not.l %d0");
asm(" move.l %d0,%a0");
asm(" move.l #0xDF7FB18E,%d0");
asm(" not.l %d0");
asm(" move.l %d0,(%a0)");
asm(" clr.l %d0");
asm(" move.l %d0,%d1");
asm(" move.l %d0,%d2");
asm(" move.l %d0,%d3");
asm(" move.l %d0,%d4");
asm(" move.l %d0,%d5");
asm(" move.l %d0,%d6");
asm(" move.l %d0,%d7");
asm(" move.l %d0,%A1");
asm(" move.l %d0,%A2");
asm(" move.l %d0,%A3");
asm(" move.l %d0,%A4");
asm(" move.l %d0,%A5");
asm(" jmp (%a0)");
}

// Saves 4 words of data - SRAMData[4] in RAM at 0x021FFFFE0 (top of DRAM)
void SaveDataInRAM( void)
{
//   __asm__   (" .extern SRAMData ");
	   asm(".extern SRAMDataP");
	   asm(" move.l   (SRAMDataP),%a0 ");
	   if(g_wPlatFormInUse == PLATFORM_5270B)
	   {
		   __asm__  ("  move.l #0x027FFFF8,%a1 ");	// try DRAM.
	   }
	   else
	   {
		   __asm__  ("  move.l #0x021FFFF8,%a1 ");	// try DRAM.

	   }
   __asm__  ("  move.w (%a0)+ , (%a1)+ /*00*/ ");
   __asm__  ("  move.w (%a0)+ , (%a1)+ /*00*/ ");
   __asm__  ("  move.w (%a0)+ , (%a1)+ /*00*/ ");
   __asm__  ("  move.w (%a0)+ , (%a1)+ /*00*/ ");

}

// REads 4 words of data into SRAMData[4] from RAM at 0x021FFFFE0 (top of DRAM)
void ReadDataFromRAM( void)
{
   __asm__   (" .extern SRAMData ");


   __asm__  ("  move.l #SRAMData,%a0 ");	 	//trap_registers
   if(g_wPlatFormInUse == PLATFORM_5270B)
   {
	   __asm__  ("  move.l #0x027FFFF8,%a1 ");
   }
   else
   {
	   __asm__  ("  move.l #0x021FFFF8,%a1 ");
   }

   __asm__  ("  move.w (%a1)+ , (%a0)+ /*00*/ ");
   __asm__  ("  move.w (%a1)+ , (%a0)+ /*00*/ ");
   __asm__  ("  move.w (%a1)+ , (%a0)+ /*00*/ ");
   __asm__  ("  move.w (%a1)+ , (%a0)+ /*00*/ ");

}

//unsigned int g_nFunctionRunning;
//unsigned int g_nFunctionSubSectionRunning;

void SaveFuntionNosInRAM( void )
{

	SRAMData[0] = (WORD)g_nFunctionRunning;
	SRAMData[1] = (WORD)g_nFunctionSubSectionRunning;
	SaveDataInRAM();
}
void ReadnDisplayDataFromRAM(void)
{
	ReadDataFromRAM();
	SetupMttty();
	iprintf("\nfunction running is %d  sub fn %d %d %d \n", SRAMData[0], SRAMData[1], SRAMData[2], SRAMData[3]);
}
