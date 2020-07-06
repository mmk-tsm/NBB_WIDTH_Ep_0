
//////////////////////////////////////////////////////
// P.Smith                      15/1/08
// hash function added.
// P.Smith                      23/7/08
// removed g_CurrentTime extern
//
// P.Smith                      22/1/09
// set the software id hash after the software id has been changed
// set a full license if the hash code has failed.
// this allows the blender come up with a full license the first time that it
// has run the license hash check.
//
// P.Smith                      1/4/09
// add GenerateHashForOrderWeight
//
// P.Smith                      25/6/09
// added GenerateHashForLoader & CheckHashForLoader
// M.McKiernan                          26-06-2008
// added TSMHashOfFloat()
// M.McKiernan                          24-06-2009
// added TSMHashOfWord( WORD nValue ); TSMHashOfByte( BYTE nValue );
// CheckHashForLoader( void );  GenerateHashForLoader( void );
//
// P.Smith                      22/7/09
// added GenerateHashForPulsing & CheckHashForPulsing
//
// P.Smith                      22/7/09
// in CheckForValidSoftwareIDHash, set m_cPermanentOptions to all options except
// vacuum loading, leave the vacuum loading off
// in CheckHashForLoader reset vacuum loading license and set flag to
// allowing saving to eeprom.
//
// P.Smith                      15/1/10
// added TSMHashOfInt
// CheckHashForSDCardErrorCtr, checks hash for sd error ctr, if not okay,
// then reset counter to 0 and set flag to save data to eeprom.
//
// P.Smith                      16/2/10
// added CheckHashForEthernetIP, if the hash does not compare, the eip
// functionality is disabled.
// dummy word is set to AA, license bit is reset to 0
//
// P.Smith                      5/3/10
// set options to FFFF if the software hash does not agree
// remove LICENSE_CLEANOPTIONBIT from original license.
//
// P.Smith                      8/3/10
// in CheckForValidSoftwareIDHash, all options are set on
// particularly the ethernet ip needs to be enabled.
// this was not being done when the software id has for the licensing.
// failed.
//
// P.Smith                      25/3/10
// added TSMHashS in the line  for (hash=0, i=0 ; i<len ; i++),
// the hash=0 was hash=1
//
// P.Smith                      27/4/10
// added TSMHashOfLong and CheckHashHistoryLog.
// set baud rate to 9600 in hash check. This ensures that the first time
// that the software is run, the blender will set the baud rate to 9600
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
#include "OneWire.h"
#include "Hash.h"
#include <Random.h>
#include "VacVars.h"
#include "Vacldee.h"
#include "License.h"
#include "InitNBBComms.h"



extern CalDataStruct    g_CalibrationData;
extern  OnBoardCalDataStruct   g_OnBoardCalData;
extern   structVacCalib   g_VacCalibrationData;


/*-------------------------------------------------------------------------*/
/**
  @brief Compute the hash key for a buffer - Adapted from dictionary_hash
  @param key  Character string to use for key.
  @return 1 unsigned int

  This hash function has been taken from an Article in Dr Dobbs Journal.
  This is normally a collision-free function, distributing keys evenly.
 */
/*--------------------------------------------------------------------------*/

WORD TSMHash(unsigned char * key, int len )
{
// int   len ;
 WORD hash ;
 int   i ;

// len = strlen(key);
 for (hash=1, i=0 ; i<len ; i++) {
  hash += (unsigned)key[i] ;
  hash += (hash<<10);
  hash ^= (hash>>6) ;
 }
 hash += (hash <<3);
 hash ^= (hash >>11);
 hash += (hash <<15);
 return hash ;
}

WORD TSMHashS(char * key, int len )
{
// int   len ;
 WORD hash ;
 int   i ;

// len = strlen(key);
 for (hash=0, i=0 ; i<len ; i++) {
  hash += (unsigned)key[i] ;
  hash += (hash<<10);
  hash ^= (hash>>6) ;
 }
 hash += (hash <<3);
 hash ^= (hash >>11);
 hash += (hash <<15);
 return hash ;
}

WORD TSMHashOfFloat( float fValue)
{
 float fTemp = fValue;
 WORD nCSum;
 BYTE cBuf[sizeof(float)];

      memcpy(&cBuf, &fTemp, sizeof(float) );    // copy structure to array.
      nCSum = TSMHash(cBuf, sizeof(float) );          // calc. c'sum on data
  return(nCSum);
}

//////////////////////////////////////////////////////
// GenerateHashOfByte
//
//
//////////////////////////////////////////////////////
WORD TSMHashOfByte( BYTE nValue )
{
 BYTE nTemp = nValue;
 WORD nCSum;
 BYTE cBuf[sizeof(BYTE)];

   memcpy(&cBuf, &nTemp, sizeof(BYTE) );     // copy structure to array.
   nCSum = TSMHash(cBuf, sizeof(BYTE) );          // calc. c'sum on data
  return(nCSum);
}
//////////////////////////////////////////////////////
// GenerateHashOfByte
//
//
//////////////////////////////////////////////////////
WORD TSMHashOfWord( WORD nValue )
{
 WORD nTemp = nValue;
 WORD nCSum;
 BYTE cBuf[sizeof(WORD)];

   memcpy(&cBuf, &nTemp, sizeof(WORD) );     // copy structure to array.
   nCSum = TSMHash(cBuf, sizeof(WORD) );          // calc. c'sum on data
  return(nCSum);
}


//////////////////////////////////////////////////////
// TSMHashOfInt
//
//
//////////////////////////////////////////////////////
unsigned int TSMHashOfInt( unsigned int nValue )
{
 unsigned int nTemp = nValue;
 unsigned int nCSum;
 BYTE cBuf[sizeof(unsigned int)];

   memcpy(&cBuf, &nTemp, sizeof(unsigned int) );     // copy structure to array.
   nCSum = TSMHash(cBuf, sizeof(unsigned int) );          // calc. c'sum on data
  return(nCSum);
}

//////////////////////////////////////////////////////
// TSMHashOfLong
//
//
//////////////////////////////////////////////////////
WORD TSMHashOfLong( long nValue )
{
 long lTemp = nValue;
 WORD wCSum;
 BYTE cBuf[sizeof(long)];

   memcpy(&cBuf, &lTemp, sizeof(long) );     // copy structure to array.
   wCSum = TSMHash(cBuf, sizeof(long) );          // calc. c'sum on data
  return(wCSum);
}





//////////////////////////////////////////////////////
// GenerateHashForSoftwareID( void )
//
//
//////////////////////////////////////////////////////

void GenerateHashForSoftwareID( void )
{
    unsigned char cBuffer[4];
    cBuffer[0] = (g_OnBoardCalData.m_lSoftwareID & 0xf000) >>12;
    cBuffer[1]= (g_OnBoardCalData.m_lSoftwareID & 0x0f00) >>8;
    cBuffer[2]= (g_OnBoardCalData.m_lSoftwareID & 0x00f0) >>4;
    cBuffer[3]= (g_OnBoardCalData.m_lSoftwareID & 0x000f);

    g_OnBoardCalData.m_nSoftwareIDHash = TSMHash(cBuffer,4);
}

//////////////////////////////////////////////////////
// GenerateHashForSoftwareID( void )
//
//
//////////////////////////////////////////////////////

void CheckForValidSoftwareIDHash( void )
{
    unsigned char cBuffer[4],cTemp;
    long lTemp;
    cBuffer[0] = (g_OnBoardCalData.m_lSoftwareID & 0xf000) >>12;
    cBuffer[1]= (g_OnBoardCalData.m_lSoftwareID & 0x0f00) >>8;
    cBuffer[2]= (g_OnBoardCalData.m_lSoftwareID & 0x00f0) >>4;
    cBuffer[3]= (g_OnBoardCalData.m_lSoftwareID & 0x000f);
    lTemp = TSMHash(cBuffer,4);

    if(lTemp != g_OnBoardCalData.m_nSoftwareIDHash)
    {
        GetTime();
        g_OnBoardCalData.m_lSoftwareID = ((rand() *  g_CurrentTime[TIME_MINUTE]^ g_CurrentTime[TIME_SECOND]) & 0x1fff);   // use random number generator.

        g_bSaveAllCalibrationToOnBoardEEprom = TRUE;        // save calibration data
        cTemp = LICENSE_BLENDERRUNOPTIONBIT | LICENSE_KGHROPTIONBIT | LICENSE_GPMOPTIONBIT | LICENSE_LLSOPTIONBIT;
        g_OnBoardCalData.m_cPermanentOptions = 0xff;
        g_OnBoardCalData.m_wBlenderOptions = 0xffff;
        g_OnBoardCalData.m_cTemporaryOptions = 0;           // reset temporary options
        g_OnBoardCalData.m_nBlenderRunHourCounter = 0;      // blender run counter reset. //--review-- other counters are not necessary

        // generate hash for the new software id
        cBuffer[0] = (g_OnBoardCalData.m_lSoftwareID & 0xf000) >>12;
        cBuffer[1]= (g_OnBoardCalData.m_lSoftwareID & 0x0f00) >>8;
        cBuffer[2]= (g_OnBoardCalData.m_lSoftwareID & 0x00f0) >>4;
        cBuffer[3]= (g_OnBoardCalData.m_lSoftwareID & 0x000f);
        lTemp = TSMHash(cBuffer,4);
        g_OnBoardCalData.m_nSoftwareIDHash = lTemp;
    }
}


//////////////////////////////////////////////////////
// GenerateHashForOrderWeight( void )
// generate has for order weight
//
//////////////////////////////////////////////////////

void GenerateHashForOrderWeight( void )
{
    int nSize = sizeof(g_ProcessData.m_fComponentOrderWeightAccumulator);
    unsigned char cBuffer[nSize];
    memcpy(&cBuffer,&g_ProcessData.m_fComponentOrderWeightAccumulator,nSize);    // copy structure to array.
    g_ProcessData.m_nOrderWeightHash = TSMHash(cBuffer,nSize);
}


//////////////////////////////////////////////////////
// GenerateHashForLoader
//
//
//////////////////////////////////////////////////////

void GenerateHashForLoader( void )
{
   g_VacCalibrationData.m_nLoaderHash = TSMHashOfByte( g_VacCalibrationData.m_nLoaders );
}



//////////////////////////////////////////////////////
// CheckHashForLoader
//
//
//////////////////////////////////////////////////////

void CheckHashForLoader( void )
{
   unsigned char cData;
   if( g_VacCalibrationData.m_nLoaderHash != TSMHashOfByte( g_VacCalibrationData.m_nLoaders ) )
   {
       g_VacCalibrationData.m_nLoaders = 0;
       g_VacCalibrationData.m_nLoaderHash = TSMHashOfByte( g_VacCalibrationData.m_nLoaders );
       cData = LICENSE_VACUUMLOADING ^ 0xFF;
       g_OnBoardCalData.m_cPermanentOptions = g_OnBoardCalData.m_cPermanentOptions & cData;
       g_bSaveLoaderCalibrationData = TRUE;
       g_bSaveAllCalibrationToOnBoardEEprom = TRUE;        // save calibration data
   }
}

//////////////////////////////////////////////////////
// GenerateHashForLoader
//
//
//////////////////////////////////////////////////////

void GenerateHashForPulsing( void )
{
   g_CalibrationData.m_wFillingMethodHash  = TSMHashOfWord( g_CalibrationData.m_wFillingMethod[0]);
}

//////////////////////////////////////////////////////
// CheckHashForLoader
//
//
//////////////////////////////////////////////////////

void CheckHashForPulsing( void )
{
   unsigned int i;
   if( g_CalibrationData.m_wFillingMethodHash != TSMHashOfWord( g_CalibrationData.m_wFillingMethod[0]) )
   {
       for(i= 0; i < MAX_COMPONENTS; i++)
       {
           g_CalibrationData.m_wFillingMethod[i] = 0;
       }

       g_CalibrationData.m_wFillingMethodHash = TSMHashOfWord( g_CalibrationData.m_wFillingMethod[0] );
       g_bSaveAllCalibrationToEEprom = TRUE;

   }
   else
   {
   //    SetupMttty();
   //    iprintf("\n hash okay");
   }

}

//////////////////////////////////////////////////////
// CheckHashForSDCardErrorCtr( void )
//
//
//////////////////////////////////////////////////////

void CheckHashForSDCardErrorCtr( void )
{
   if( g_ProcessData.m_nSDErrorHash != TSMHashOfInt( g_ProcessData.m_nSdCardErrorCtr ) )
    {
    //   SetupMttty();
    //   iprintf("\n hash error %d %d",g_ProcessData.m_nSDErrorHash,h);

    g_ProcessData.m_nSdCardErrorCtr = 0;
    g_ProcessData.m_nSDErrorHash = TSMHashOfInt( g_ProcessData.m_nSdCardErrorCtr );
    g_bSaveAllProcessDataToEEprom = TRUE;

   }
}

//////////////////////////////////////////////////////
// GenerateHashForSDCardErrorCtr( void )
//
//
//////////////////////////////////////////////////////

void GenerateHashForSDCardErrorCtr( void )
{
    g_ProcessData.m_nSDErrorHash = TSMHashOfInt( g_ProcessData.m_nSdCardErrorCtr );
//    SetupMttty();
//    iprintf("\n hash is %d ctr %d",g_ProcessData.m_nSDErrorHash,g_ProcessData.m_nSdCardErrorCtr);

}


//////////////////////////////////////////////////////
// CheckHashForEthernetIP( void )
//
//
//////////////////////////////////////////////////////


void CheckHashForEthernetIP( void )
{
	unsigned char cData;
//	WORD h;
	if( g_ProcessData.m_wEtherNetIPHash != TSMHashOfWord( g_ProcessData.m_wEtherNetIPDummyWord ) )
    {
//	    h = TSMHashOfWord( g_ProcessData.m_wEtherNetIPDummyWord );

	    g_ProcessData.m_wEtherNetIPDummyWord = 0xAA;

        g_ProcessData.m_wEtherNetIPHash = TSMHashOfWord( g_ProcessData.m_wEtherNetIPDummyWord );
	    cData = LICENSE_ETHERNETIP ^ 0xFF;

	    g_OnBoardCalData.m_cPermanentOptions = g_OnBoardCalData.m_cPermanentOptions & cData;
//        SetupMttty();
//        iprintf("\n hash stored  %d %d %x",g_ProcessData.m_wEtherNetIPHash,h,g_OnBoardCalData.m_cPermanentOptions);

	    g_bSaveAllCalibrationToOnBoardEEprom = TRUE;        // save calibration data
	    g_bSaveAllProcessDataToEEprom = TRUE;
   }
}

//////////////////////////////////////////////////////
// CheckHashHistoryLog( void )
//
//
//////////////////////////////////////////////////////

void CheckHashHistoryLog( void )
{
	unsigned int i;
	WORD h,h1;
	if( g_ProcessData.m_nHistoryLogHash != TSMHashOfLong( g_ProcessData.m_lHoursBlenderIsRunning ) )
    {
		h = TSMHashOfLong( g_ProcessData.m_lHoursBlenderIsRunning);
	    g_ProcessData.m_lHoursBlenderIsRunning = 0;

	   g_ProcessData.m_lHoursBlenderIsRunning = 0;
	   g_ProcessData.m_n24HourOngoingResetCounter = 0;
	   g_ProcessData.m_n24HourOngoingMotorTripCounter = 0;
	   g_CalibrationData.m_nBaudRate = BAUD_RATE_9600_ID;
       g_ProcessData.m_nHistoryLogHash = TSMHashOfLong( g_ProcessData.m_lHoursBlenderIsRunning );
		h1 = TSMHashOfLong( g_ProcessData.m_lHoursBlenderIsRunning);


    	for(i = 0; i < MAX_COMPONENTS; i++ )
        {
        	g_ProcessData.m_lComponentActivations[i] = 0;
        }

//        SetupMttty();
//        iprintf("\n hash stored  %d calc %d new h1 %d ",g_ProcessData.m_nHistoryLogHash,h,h1);
	    g_bSaveAllProcessDataToEEprom = TRUE;
   }
}


//////////////////////////////////////////////////////
// GenerateHashForSDCardErrorCtr( void )
//
//
//////////////////////////////////////////////////////

void GenerateHashForHistoryLog( void )
{
    g_ProcessData.m_nHistoryLogHash = TSMHashOfLong( g_ProcessData.m_lHoursBlenderIsRunning );
}



