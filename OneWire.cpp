/*-------------------------------------------------------------------
 File: OneWire.cpp
 Description: One wire functions.

 M. McKiernan		25-11-2005

   P.Smith                          30/1/08
   correct compiler warnings


 M. McKiernan		3-12-2009
 Changed the loop count from 17 to 38 to get 1uS period in Delay_1uS

 P.Smith		26/3/10
 Removed read of id and assembly of secret from here
 -------------------------------------------------------------------*/
#include <startnet.h>

#include <basictypes.h>
#include <stdio.h>
#include <string.h>

#include "gpio5270.h"
#include "NBBGpio.h"
#include <C:\Nburn\MOD5270\system\sim5270.h>
#include "SHA1.h"

#include "OneWire.h"
#include <stdlib.h>

structDS2432Data DS2432Data;

 BYTE g_cDS2432Data[20];   // testpurposes
 WORD g_nCalculatedCRC16;
const BYTE CRCtbl8[256] =
{
0, 94, 188, 226, 97, 63, 221, 131, 194, 156,
126, 32, 163, 253, 31, 65, 157, 195, 33, 127,
252, 162, 64, 30, 95, 1, 227, 189, 62, 96,
130, 220, 35, 125, 159, 193, 66, 28, 254, 160,
225, 191, 93, 3, 128, 222, 60, 98, 190, 224,
2, 92, 223, 129, 99, 61, 124, 34, 192, 158,
29, 67, 161, 255, 70, 24, 250, 164, 39, 121,
155, 197, 132, 218, 56, 102, 229, 187, 89, 7,
219, 133, 103, 57, 186, 228, 6, 88, 25, 71,
165, 251, 120, 38, 196, 154, 101, 59, 217, 135,
4, 90, 184, 230, 167, 249, 27, 69, 198, 152,
122, 36, 248, 166, 68, 26, 153, 199, 37, 123,
58, 100, 134, 216, 91, 5, 231, 185, 140, 210,
48, 110, 237, 179, 81, 15, 78, 16, 242, 172,
47, 113, 147, 205, 17, 79, 173, 243, 112, 46,
204, 146, 211, 141, 111, 49, 178, 236, 14, 80,
175, 241, 19, 77, 206, 144, 114, 44, 109, 51,
209, 143, 12, 82, 176, 238, 50, 108, 142, 208,
83, 13, 239, 177, 240, 174, 76, 18, 145, 207,
45, 115, 202, 148, 118, 40, 171, 245, 23, 73,
8, 86, 180, 234, 105, 55, 213, 139, 87, 9,
235, 181, 54, 104, 138, 212, 149, 203, 41, 119,
244, 170, 72, 22, 233, 183, 85, 11, 136, 214,
52, 106, 43, 117, 151, 201, 74, 20, 246, 168,
116, 42, 200, 150, 21, 75, 169, 247, 182, 232,
10, 84, 215, 137, 107, 53
};



// ******************************************************
// ********* Delay MS Routine *************************
// ******************************************************
void delay_ms(WORD time)
{
WORD x;

sim.pit[1].pmr = 0x4800; // reload values 0x4800 for 1ms counter
sim.pit[1].pcsr = 0x207; // divide ratio and options

for(x=0;x<time;x++)
{
while(!(sim.pit[1].pcsr & 0x0004)); // while bit is clear
sim.pit[1].pcsr |= 0x0004; // clear Bit by
}
}


// ******************************************************
// ********* Delay microsecond Routine *****************
// *****************************************************
/*
void Delay_uS(WORD nMicroSeconds)
{
	if(nMicroSeconds > 1000)
		nMicroSeconds = 1000;		// limit to 1mS, i.e. 1000 _US.

sim.pit[3].pcsr = 0x0117; // divide ratio and options		 PRE = 1, divider = 2.  OVW => Writing PMRn immediately replaces value in PIT counter.
//								 // pIF cleared, RLD set => Counter reloaded from PMRn on count of 0x0000, EN enabled.
sim.pit[3].pmr = (nMicroSeconds* 37); //reload value 37 PIT's per _US	  NB: should cause counter to be loaded also (OVW bit set)

while(!(sim.pit[3].pcsr & 0x0004))
	; // wait while PIF bit is clear
// sim.pit[3].pcsr |= 0x0005; // clear PIF) Bit by writing to it.
sim.pit[3].pcsr &= ~0x0001; // clear EN Bit, PIT disabled.

}

*/

    WORD	A_US;
    WORD	B_US;
    WORD	C_US;
    WORD	D_US;
    WORD	E_US;
    WORD	F_US;
    WORD	G_US;
    WORD	H_US;
    WORD	I_US;
    WORD	J_US;

// ********************************* 1 Wire Stuff *********************************
void SetSpeed_1Wire(int standard)
{
// Adjust tick values depending on speed
	if (standard)
	{
	// Standard Speed
	A_US = 6;
	B_US = 64;
	C_US = 60;
	D_US = 10;
	E_US = 9;
	F_US = 55;
	G_US = 10;
	H_US = 480;
	I_US = 70;
	J_US = 410;
	}
}



//-----------------------------------------------------------------------------
// Generate a 1-Wire reset, return 1 if no presence detect was found,
// return 0 otherwise.
// (NOTE: Does not handle alarm presence from DS2404/DS1994)
//
int OWTouchReset(void)
{
	BYTE result;
	Release1WireBus();
	Delay_uS(G_US);
//		outp(PORTADDRESS,0x00); // Drives DQ low
	Drive1WireBusLow();
	Delay_uS(H_US);
//		outp(PORTADDRESS,0x01); // Releases the bus
	Release1WireBus();
	Delay_uS(I_US);
//		result = inp(PORTADDRESS) & 0x01; // Sample for presence pulse from slave
	result = (sim.gpio.ppdsdr_datal & 0x80);  	//
	Delay_uS(J_US); // Complete the reset sequence recovery
	return result; // Return sample presence pulse result
}
//-----------------------------------------------------------------------------
// Send a 1-Wire write bit. Provide 10us recovery time.
//
void OWWriteBit(int bit)
{
	if (bit)
	{
	// Write '1' bit

//		outp(PORTADDRESS,0x00); // Drives DQ low
		Drive1WireBusLow();
		Delay_uS(A_US);
//		outp(PORTADDRESS,0x01); // Releases the bus
		Release1WireBus();

		Delay_uS(B_US); // Complete the time slot and 10us recovery
	}
	else
	{
	// Write '0' bit

//		outp(PORTADDRESS,0x00); // Drives DQ low
		Drive1WireBusLow();
		Delay_uS(C_US);
//		outp(PORTADDRESS,0x01); // Releases the bus
		Release1WireBus();

		Delay_uS(D_US);
	}
}

//-----------------------------------------------------------------------------
// Read a bit from the 1-Wire bus and return it. Provide 10us recovery time.//

int OWReadBit(void)
{
	BYTE result;
//		outp(PORTADDRESS,0x00); // Drives DQ low
		Drive1WireBusLow();
	Delay_uS(A_US);
//		outp(PORTADDRESS,0x01); // Releases the bus
	Release1WireBus();
	Delay_uS(E_US);
//		result = inp(PORTADDRESS) & 0x01; // Sample the bit value from the slave
	result = (sim.gpio.ppdsdr_datal & 0x80);  	// Need to check bit used for reading 1Wire.
	Delay_uS(F_US); 	// Complete the time slot and 10us recovery

	return result;
}

//-----------------------------------------------------------------------------
// Write 1-Wire data byte
//
void OWWriteByte(BYTE data)
{
	BYTE loop;
	// Loop to write each bit in the byte, LS-bit first
	for (loop = 0; loop < 8; loop++)
	{
		OWWriteBit(data & 0x01);
		// shift the data byte for the next bit
		data >>= 1;
	}
}


//-----------------------------------------------------------------------------
// Read 1-Wire data byte and return it
//
BYTE OWReadByte(void)
{
	BYTE loop, result=0;
	for (loop = 0; loop < 8; loop++)
	{
		// shift the result to get it ready for the next bit
		result >>= 1;
		// if result is one, then set MS bit
		if (OWReadBit())
		result |= 0x80;
	}
	return result;
}

//-----------------------------------------------------------------------------
// Write a 1-Wire data byte and return the sampled result.
//
BYTE OWTouchByte(BYTE data)
{
	BYTE loop, result=0;

	for (loop = 0; loop < 8; loop++)
	{
		// shift the result to get it ready for the next bit
		result >>= 1;
		// If sending a '1' then read a bit else write a '0'
		if (data & 0x01)
		{
			if (OWReadBit())
				result |= 0x80;
		}
		else
			OWWriteBit(0);
		// shift the data byte for the next bit
		data >>= 1;
	}
	return result;
}

//-----------------------------------------------------------------------------
// Write a block 1-Wire data bytes and return the sampled result in the same
// buffer.
//
void OWBlock(unsigned char *data, int data_len)
{
	int loop;
	for (loop = 0; loop < data_len; loop++)
	{
		data[loop] = OWTouchByte(data[loop]);
	}
}

/*
The owTouchByte operation is a simultaneous write and read from the 1-Wire bus. This function was derived so
that a block of both writes and reads could be constructed. This is more efficient on some platforms and is
commonly used in API's provided by Dallas Semiconductor. The OWBlock function simply sends and receives a
block of data to the 1-Wire using the OWTouchByte function. Note that OWTouchByte(0xFF) is equivalent to
OWReadByte() and OWTouchByte(data) is equivalent to OWWriteByte(data).
These functions plus tickDelay are all that are required for basic control of the 1-Wire bus at the bit, byte, and block
level. The following example in Example 4 shows how these functions can be used together to read a SHA-1
authenticated page of the DS2432.
*/

//-----------------------------------------------------------------------------
// Read and return the page data and SHA-1 message authentication code (MAC)
// from a DS2432.
//
int ReadPageMAC(int page, unsigned char *page_data, unsigned char *mac)
{
	int i;
	unsigned short data_crc16, mac_crc16;
	// set the speed to 'standard'
	SetSpeed_1Wire(1);
	// select the device
	if (OWTouchReset()) // Reset the 1-Wire bus
		return 0; // Return if no devices found

	OWWriteByte(0xCC); // Send Skip ROM command to select single device
	// read the page
	OWWriteByte(0xA5); // Read Authentication command
	OWWriteByte((page << 5) & 0xFF); // TA1
	OWWriteByte(0); // TA2 (always zero for DS2432)


	// read the page data
	for (i = 0; i < 32; i++)
	page_data[i] = OWReadByte();

	OWWriteByte(0xFF);
	// read the CRC16 of command, address, and data
	data_crc16 = OWReadByte();
	data_crc16 |= (OWReadByte() << 8);
	// delay 2ms for the device MAC computation
		Delay_uS(1000); // 1mS
		Delay_uS(1000); // 1mS

	// read the MAC
	for (i = 0; i < 20; i++)
	mac[i] = OWReadByte();

	// read CRC16 of the MAC
	mac_crc16 = OWReadByte();
	mac_crc16 |= (OWReadByte() << 8);
	// check CRC16...

	return 1;
}


// *****************************************************
// ********* Function to read a DS2401 1-wire serial number.
// * Data returned in array DS2401Buffer[10], DS2401FamilyCode, S.No. and DS2401CRC.
//
// MSB                                                      LSB
// ------------------------------------------------------------
// | 8 bit CRC |     48 bit Serial No.      | Family code = 01 |
// ------------------------------------------------------------
// Note data comes LSB first.

// NB: This function uses USER_ENTER_CRITICAL(), an OS call which stops all interrupts
//     and task switching.  It should therefore only be called on start-up or when nothing
//     time critical is happening.
// *****************************************************
BYTE DS2401Buffer[10];

void ReadDS2401(void)
{
	int i;
//	BYTE CRC = 0;

//	DS2401CRC = 0;
    SetSpeed_1Wire( 1 );        // set 1-wire for standard speed.

    USER_ENTER_CRITICAL();    // OS call - will hold off interrupts.

	if (!( OWTouchReset() ) ) // Reset the 1-Wire bus (Presence pulse => 0)
	{

//		return 0; // Return if no devices found

	OWWriteByte(0x0F);		//Read Rom command

//		DS2401FamilyCode = ReadByte_1W();  //now read all 8 bytes of data starting with family code

		for(i=0; i < 8; i++)
		{
			DS2401Buffer[i] = OWReadByte();  // read all 8 data bytes, including crc (8bits)
		}

//		DS2401CRC = ReadByte_1W();



    }

    USER_EXIT_CRITICAL();    // OS call - will allow interrupts.

// m    for(i=0; i<7; i++)
//    CRC = CRCtbl8[(CRC ^ DS2401Buffer[i]) & 0xFF];
//    DS2401Buffer[9] = CRC;
// Calculate the CRC on the 1st 7 bytes of S.No.

    DS2401Buffer[8] = ComputeCRC8( DS2401Buffer, 7);   //calculated cRC
    //NB, the crc from the device will be in DS2401Buffer[7], i.e. last byte read.

}

// *****************************************************
// ********* Function to read a DS2432 1-wire serial number.
// * Data returned in array DS2432Buffer[10], DS2432FamilyCode, S.No. and DS2432CRC.
//
// MSB                                                      LSB
// ------------------------------------------------------------
// | 8 bit CRC |     48 bit Serial No.      | Family code = 01 |
// ------------------------------------------------------------
// Note data comes LSB first.

// NB: This function uses USER_ENTER_CRITICAL(), an OS call which stops all interrupts
//     and task switching.  It should therefore only be called on start-up or when nothing
//     time critical is happening.
// *****************************************************
BYTE DS2432Buffer[10];

void ReadDS2432ID(BYTE *pBuf)
{
	int i;
//	BYTE CRC = 0;

//	DS2401CRC = 0;
    SetSpeed_1Wire( 1 );        // set 1-wire for standard speed.

    USER_ENTER_CRITICAL();    // OS call - will hold off interrupts.

	if (!( OWTouchReset() ) ) // Reset the 1-Wire bus (Presence pulse => 0)
	{

//		return 0; // Return if no devices found

	OWWriteByte(READ_ROM);		//Read Rom command

//		DS2401FamilyCode = ReadByte_1W();  //now read all 8 bytes of data starting with family code

		for(i=0; i < 8; i++)
		{
			*(pBuf+i) = OWReadByte();  // read all 8 data bytes, including crc (8bits)
		}

//		DS2401CRC = ReadByte_1W();



    }

    USER_EXIT_CRITICAL();    // OS call - will allow interrupts.

////??     for(i=0; i<7; i++)
//    CRC = CRCtbl8[(CRC ^ DS2401Buffer[i]) & 0xFF];
//    DS2401Buffer[9] = CRC;
// Calculate the CRC on the 1st 7 bytes of S.No.

    *(pBuf+8) = ComputeCRC8( pBuf, 7);   //calculated cRC
    //NB, the crc from the device will be in DS2432Buffer[7], i.e. last byte read.

}



// ******************************************************
// ********* Delay microsecond Routine *****************
// *****************************************************
/*
void Delay_uS(WORD nMicroSeconds)
{
//	if(nMicroSeconds > 1000)
//		nMicroSeconds = 1000;		// limit to 1mS, i.e. 1000 _US.

sim.pit[3].pcsr = 0x0117; // divide ratio and options		 PRE = 1, divider = 2.  OVW => Writing PMRn immediately replaces value in PIT counter.
//								 // pIF cleared, RLD set => Counter reloaded from PMRn on count of 0x0000, EN enabled.
sim.pit[3].pmr = (nMicroSeconds* 37); //reload value 37 PIT's per _US	  NB: should cause counter to be loaded also (OVW bit set)

while(!(sim.pit[3].pcsr & 0x0004))
	; // wait while PIF bit is clear
// sim.pit[3].pcsr |= 0x0005; // clear PIF) Bit by writing to it.
sim.pit[3].pcsr &= ~0x0001; // clear EN Bit, PIT disabled.

}
*/
void Delay_1uS( void )
{
//asm("   move.l #17,%d0");
asm("   move.l #38,%d0");		// Eclipse
asm("DECLOOP:");
asm(" 	subq.l #1,%d0");
asm(" 	bne DECLOOP");
}

void Delay_uS( WORD nMicroSeconds )
{
 volatile WORD i;
    for( i= 0; i < nMicroSeconds; i++ )
    {
        Delay_1uS();
    }
}

// ******************************************************
// ********* 1-Wire CRC (8bit) Routine *****************
// *****************************************************
BYTE ComputeCRC8(BYTE *msg, WORD msgLen)
{
BYTE CRC = 0x00;
WORD i;

for(i=0; i<msgLen; i++)
CRC = CRCtbl8[(CRC ^ msg[i]) & 0xFF];

return CRC;
}




//******** DS2432....MEMORY FUNCTIONS.....**********//
void WriteDS2432Scratchpad(BYTE *pWriteData, WORD nAddress, int nLen, WORD *pCRC16)
{
	int i;
	BYTE nTemp;
	BYTE cWriteBuffer[12];
	WORD nWordTemp;

    SetSpeed_1Wire( 1 );        // set 1-wire for standard speed.

    USER_ENTER_CRITICAL();    // OS call - will hold off interrupts.

	if (!( OWTouchReset() ) ) // Reset the 1-Wire bus (Presence pulse => 0)
	{

	OWWriteByte(SKIP_ROM);		//Write SKIP ROM command

	OWWriteByte(WRITE_SCRATCHPAD);		//Write Scratchpad command
	cWriteBuffer[0] = WRITE_SCRATCHPAD;

// write addresss (TA1 & TA2)
   nTemp = nAddress & 0x00F8;   // L.S. byte of addres, 3 l.s. bits should be 0.
   cWriteBuffer[1] = nTemp;

 	OWWriteByte(nTemp);		//Write L.S. byte of address
      g_cDS2432Data[0] = nTemp;

   nTemp = nAddress >> 8;   //M.S. byte of address
   cWriteBuffer[2] = nTemp;
 	OWWriteByte(nTemp);		//Write m.S. byte of address
      g_cDS2432Data[1] = nTemp;

		for(i=0; i < nLen; i++)
		{
         cWriteBuffer[i+3] = *pWriteData;
			OWWriteByte(*pWriteData);  // write out each byte of data.
         g_cDS2432Data[i+2] = *pWriteData;
			pWriteData++;
		}


		g_nCalculatedCRC16 = Calculate1WCRC16(cWriteBuffer, 11);
// not sure about this...reading the CRC16 from DS2432, CRC computed on CMD, ADDR. & DataBytes.(pg. 8 of datasheet)
      nTemp = OWReadByte();   // read CRC lsb
      g_cDS2432Data[11] = nTemp;
      g_cDS2432Data[12] = OWReadByte();  // read msbyte
      nWordTemp = g_cDS2432Data[12];  //  msbyte
      nWordTemp = (nWordTemp << 8) + nTemp;
      *pCRC16 = nWordTemp;
      OWTouchReset();

    }

    USER_EXIT_CRITICAL();    // OS call - will allow interrupts.
/*
    iprintf("\nIn WriteScratchPad function: ");
 		for(i=0; i<12; i++)
		 iprintf(" %2x, %d", cWriteBuffer[i],i);
    iprintf("\nLeaving WriteScratchPad function:\n ");
*/

}
//
// Assembe secret. Result to pBuf, ID must be in IDBuf.
void AssembleSecretData( BYTE *pBuf, BYTE *pIDBuf )
{
      *(pBuf+0) = '1';
      *(pBuf+1) = 'G';
      *(pBuf+2) = *(pIDBuf+1) + 21;   // use 4 bytes from ID.
      *(pBuf+3) = *(pIDBuf+2) + 17;
      *(pBuf+4) = *(pIDBuf+3) + 12;
      *(pBuf+5) = *(pIDBuf+4) + 33;
      *(pBuf+6) = 'W';
      *(pBuf+7) = '8';
}


//******** DS2432....MEMORY FUNCTIONS.....**********//
void ReadDS2432Scratchpad(BYTE *pReadData, int nBytes)
{
	int i;
	BYTE cWriteBuffer[16];
	BYTE cTemp;
	WORD nWordTemp,nCalculatedCRC16;
	bool bResult = FALSE;        // success or failure for read command.

    SetSpeed_1Wire( 1 );        // set 1-wire for standard speed.

    USER_ENTER_CRITICAL();    // OS call - will hold off interrupts.

	if (!( OWTouchReset() ) ) // Reset the 1-Wire bus (Presence pulse => 0)
	{

	OWWriteByte(SKIP_ROM);		//Write SKIP ROM command

	OWWriteByte(READ_SCRATCHPAD);		//Read Scratchpad command
	cWriteBuffer[0] = READ_SCRATCHPAD;

// begin reading addresss (TA1 & TA2)
//			g_cDS2432Data[0] = OWReadByte( );  // read address byte 1
        *pReadData =OWReadByte( );  // read address byte 1
        cWriteBuffer[1] = *pReadData;
			pReadData++;
//			g_cDS2432Data[1] = OWReadByte( );  // read address byte 2
        *pReadData =OWReadByte( );  // read address byte 2
        cWriteBuffer[2] = *pReadData;
			pReadData++;
//			g_cDS2432Data[2] = OWReadByte( );  // read E/S byte
        *pReadData =OWReadByte( );  // read E/S byte
        cWriteBuffer[3] = *pReadData;
			pReadData++;


		for(i=0; i < nBytes; i++)
		{
//			g_cDS2432Data[i+3] = OWReadByte( );  // read n bytes of data.
			*pReadData = OWReadByte( );  // read n bytes of data.
         cWriteBuffer[i+4] = *pReadData;
			pReadData++;
		}

// not sure about this...reading the CRC16 from DS2432, CRC computed on CMD, ADDR. & DataBytes.(pg. 8 of datasheet)
			*pReadData = OWReadByte( );  // read CRC byte 1
			cWriteBuffer[12] = *pReadData;
			pReadData++;
			*pReadData = OWReadByte( );  // read CRC byte 2
			cWriteBuffer[13] = *pReadData;
			pReadData++;

			*pReadData = OWReadByte( );  // read past end....should get all 1's
			cWriteBuffer[14] = *pReadData;
			pReadData++;

//      g_cDS2432Data[13] = 0xea;      // test purposes only.

      OWTouchReset();

    }

    USER_EXIT_CRITICAL();    // OS call - will allow interrupts.

      cTemp = cWriteBuffer[12];   // read CRC lsb
      nWordTemp = cWriteBuffer[13];   // //  msbyte
      nWordTemp = (nWordTemp << 8) + cTemp;

   nCalculatedCRC16 = Calculate1WCRC16(cWriteBuffer, 12);   //include command byte, 3 addr, 8 data
   if(nWordTemp == (~nCalculatedCRC16 & 0xFFFF) )
      bResult = TRUE;


}

//



//******** DS2432....MEMORY FUNCTIONS.....**********//
bool NewReadDS2432Scratchpad(BYTE *pReadData, int nBytes)
{
	int i;
	BYTE cWriteBuffer[16];
	BYTE cTemp;
	WORD nWordTemp,nCalculatedCRC16;
	bool bResult = FALSE;        // success or failure for read command.

    SetSpeed_1Wire( 1 );        // set 1-wire for standard speed.

    USER_ENTER_CRITICAL();    // OS call - will hold off interrupts.

	if (!( OWTouchReset() ) ) // Reset the 1-Wire bus (Presence pulse => 0)
	{

	OWWriteByte(SKIP_ROM);		//Write SKIP ROM command

	OWWriteByte(READ_SCRATCHPAD);		//Read Scratchpad command
	cWriteBuffer[0] = READ_SCRATCHPAD;

// begin reading addresss (TA1 & TA2)
//			g_cDS2432Data[0] = OWReadByte( );  // read address byte 1
        *pReadData =OWReadByte( );  // read address byte 1
        cWriteBuffer[1] = *pReadData;
			pReadData++;
//			g_cDS2432Data[1] = OWReadByte( );  // read address byte 2
        *pReadData =OWReadByte( );  // read address byte 2
        cWriteBuffer[2] = *pReadData;
			pReadData++;
//			g_cDS2432Data[2] = OWReadByte( );  // read E/S byte
        *pReadData =OWReadByte( );  // read E/S byte
        cWriteBuffer[3] = *pReadData;
			pReadData++;


		for(i=0; i < nBytes; i++)
		{
//			g_cDS2432Data[i+3] = OWReadByte( );  // read n bytes of data.
			*pReadData = OWReadByte( );  // read n bytes of data.
         cWriteBuffer[i+4] = *pReadData;
			pReadData++;
		}

// not sure about this...reading the CRC16 from DS2432, CRC computed on CMD, ADDR. & DataBytes.(pg. 8 of datasheet)
			*pReadData = OWReadByte( );  // read CRC byte 1
			cWriteBuffer[12] = *pReadData;
			pReadData++;
			*pReadData = OWReadByte( );  // read CRC byte 2
			cWriteBuffer[13] = *pReadData;
			pReadData++;

			*pReadData = OWReadByte( );  // read past end....should get all 1's
			cWriteBuffer[14] = *pReadData;
			pReadData++;

//      g_cDS2432Data[13] = 0xea;      // test purposes only.

      OWTouchReset();

    }

    USER_EXIT_CRITICAL();    // OS call - will allow interrupts.

      cTemp = cWriteBuffer[12];   // read CRC lsb
      nWordTemp = cWriteBuffer[13];   // //  msbyte
      nWordTemp = (nWordTemp << 8) + cTemp;

   nCalculatedCRC16 = Calculate1WCRC16(cWriteBuffer, 12);   //include command byte, 3 addr, 8 data
   if(nWordTemp == (~nCalculatedCRC16 & 0xFFFF) )
      bResult = TRUE;

   return( bResult );

}

//******** DS2432....MEMORY FUNCTIONS.....* *********//
void ReadDS2432Memory(BYTE *pReadData, WORD nAddr, int nBytes)
{
	int i;
	BYTE nTemp;
	BYTE cTempBuffer[164];

    SetSpeed_1Wire( 1 );        // set 1-wire for standard speed.

    USER_ENTER_CRITICAL();    // OS call - will hold off interrupts.



	if (!( OWTouchReset() ) ) // Reset the 1-Wire bus (Presence pulse => 0)
	{
/*
  // Read Memory command
   send_block[send_cnt++] = CMD_READ_MEMORY;
   // TA1
   send_block[send_cnt++] = (uchar)(address & 0xFF);
   // TA2
   send_block[send_cnt++] = (uchar)((address >> 8) & 0xFF);
    SetSpeed_1Wire( 1 );        // set 1-wire for standard speed.

    USER_ENTER_CRITICAL();    // OS call - will hold off interrupts.

	if (!( OWTouchReset() ) ) // Reset the 1-Wire bus (Presence pulse => 0)
	{

	OWWriteByte(SKIP_ROM);		//Write SKIP ROM command

	OWWriteByte(READ_SCRATCHPAD);		//Read Scratchpad command


*/


	OWWriteByte(SKIP_ROM);		     //Write SKIP ROM command

	OWWriteByte(READ_MEMORY);		//Read Memory command
   cTempBuffer[0] = READ_MEMORY;


// write addresss (TA1 & TA2)

   nTemp = (BYTE)(nAddr & 0xFF); // & 0x00F8;   // L.S. byte of addres, 3 l.s. bits should be 0.
 	OWWriteByte(nTemp);		//Write L.S. byte of address
   cTempBuffer[1] = nTemp;

   nTemp = (BYTE)( (nAddr >> 8) & 0xFF);   //M.S. byte of address
 	OWWriteByte(nTemp);		//Write m.S. byte of address
   cTempBuffer[2] = nTemp;


		for(i=0; i < nBytes; i++)
		{
			*pReadData = OWReadByte( );  // read n bytes of data.
         cTempBuffer[i+3] = *pReadData;

			pReadData++;
		}


      OWTouchReset();

    }

    USER_EXIT_CRITICAL();    // OS call - will allow interrupts.

///     iprintf("\nIon ReadDS2432Memory function: \n");
// 		for(i=0; i<nBytes+3; i++)
/// 		for(i=0; i<11; i++)
///		 iprintf(" %2x", cTempBuffer[i] );


}

//******** DS2432....MEMORY FUNCTIONS.....**********//
BYTE LoadFirstSecretDS2432( BYTE *pSecretData )
{
   WORD nCRC16[1];
   WORD *ptrCRC16 = nCRC16;
   BYTE  Buffer[16];
   BYTE  *pBuffer = Buffer;
   BYTE  cTA1Byte,cTA2Byte,cESByte;
   BYTE  cTemp = 0;
   bool  bScratchReadResult = FALSE;
   int i;
   int nError = 0;

// write the secret to the scratchpad.
   WriteDS2432Scratchpad(pSecretData, 0x80, 8, ptrCRC16);
/*
   if((~g_nCalculatedCRC16 & 0xffff) == *ptrCRC16 )
      iprintf("\nWrite to scraaatch succeeded: \n");
   else
      iprintf("\nWrite to scraaatch failed: \n");
*/
   bScratchReadResult = NewReadDS2432Scratchpad(pBuffer, 8 );      // 8 is no. of data bytes (there will be more, e.g. TA..)
   for(i=0; i<8; i++)
   {
      if(Buffer[i+3] != *(pSecretData+i) )
      {
         nError++;
//         iprintf("\n %x %x ", Buffer[i+3], *pSecretData+i );
      }

   }
/*
   if(nError == 0)
      iprintf("\nWrite to scratch succeeeded: \n");
   else
      iprintf("\nWrite to scratch falled: \n");
*/

// need these bytes for Load First Secret command.
   cTA1Byte = Buffer[0];
   cTA2Byte = Buffer[1];
   cESByte =  Buffer[2];
//    iprintf("\nRead Spad ", cESByte);
//      for( i=0; i<11; i++ )
//         iprintf("%2x ", Buffer[i]);

//    iprintf("\nSecret, Wr Spad, AA result: %2x\n", cESByte);

    USER_ENTER_CRITICAL();    // OS call - will hold off interrupts.

	if (!( OWTouchReset() ) ) // Reset the 1-Wire bus (Presence pulse => 0)
	{
/*  - Double checking that data still on scratchpad after reset.
      ReadDS2432Scratchpad(pBuffer, 8 );      // 8 is no. of data bytes (there will be more, e.g. TA..)
      iprintf("\nRead Spad - 2: "  );
      for( i=0; i<11; i++ )
         iprintf("%2x ", Buffer[i]);
*/

	     OWWriteByte(SKIP_ROM);		     //Write SKIP ROM command

	     OWWriteByte(LOAD_FIRST_SECRET);		//lOAD 1ST sECRET command

// write addresss (TA1 & TA2)
 	    OWWriteByte(cTA1Byte);		//Write bytes as received from prev. read scratchpad cmd
 	    OWWriteByte(cTA2Byte);
 	    OWWriteByte(cESByte);

      Delay_uS( 10000 );   // 10mS - allow DS2432 to write to eeprom.
      Delay_uS( 1000 );   // 1mS - give an extra mS to be sure.

      cTemp = OWReadByte();   // should be 1's and 0's (AA or 55).


      ReadDS2432Scratchpad(pBuffer, 8 );      // 8 data bytes, reading for  TA..)

 // Resulting bytes for Load First Secret command.
//       iprintf("\nRead Scratchpad - After 11mS: "  );
      cESByte =  Buffer[2];
//      for( i=0; i<11; i++ )
//         iprintf("%2x ",Buffer[i]);

      OWTouchReset();


    }

    USER_EXIT_CRITICAL();    // OS call - will allow interrupts.

   return( cTemp );     // return the E/S byte.

}
// Compare 8 bytes - returns 1 if any different, 0 if all same.
BYTE Compare8Bytes(BYTE *pBuf1, BYTE *pBuf2)
{
int i;
   for(i=0; i<8; i++)
   {
      if( *(pBuf1+i) != *(pBuf2+i) )
         return(1);
   }
   return(0);
}

#define TEST5   "Michael"
char *testarray3 = TEST5;

//******** DS2432....CopyScratchpadDS2432.....**********//
void CopyScratchpadDS2432( BYTE *pBuf, WORD nWriteAddress )
{
   WORD nCRC16[1];
   WORD *ptrCRC16 = nCRC16;
   BYTE *pReadData;
   BYTE  Buffer[20];
   BYTE  *pBuffer = Buffer;
   BYTE  InputBuffer[70];
   BYTE  *pInputBuffer = InputBuffer;
   BYTE Message_MAC[20];
   SHA1Context ShaStruct;

   int   i;
   int   nError = 0;
   int   nShaErr = 0;
   DWORD A,B,C,D,E,Temp;

 // read ID into structure.
   //ReadDS2432ID( DS2432Data.m_cIDBuf );

/*
   iprintf("\n Secret in structure:");
   for(i=0; i<8; i++)
      iprintf(" %x", DS2432Data.m_cSecretBuf[i] );
*/

 //   iprintf("\n ID in structure:");
 //   for(i=0; i<8; i++)
 //     iprintf(" %x", DS2432Data.m_cIDBuf[i] );
//testonly - outgoing data.
//    iprintf("\n Data for Scratchpad:");
//    for(i=0; i<8; i++)
//      iprintf(" %x", *(pBuf+i) );
   pReadData = DS2432Data.m_cPage0Buf;
   ReadDS2432Memory(pReadData, PAGE_0_ADDR, DS2432_PAGE_SIZE);     // read 1st page of memory from DS2432.

   pReadData = DS2432Data.m_cRegisterPage;
   ReadDS2432Memory(pReadData, REG_PAGE_ADDR, DS2432_REG_PAGE_SIZE);     // read register page into struct.
   /*
   iprintf("\n Reg. Page :");
   for(i=0; i<8; i++)
      iprintf("%2x ", DS2432Data.m_cRegisterPage[i]);
*/

// write the data to the scratchpad.
   WriteDS2432Scratchpad(pBuf, nWriteAddress, 8, ptrCRC16);

   ReadDS2432Scratchpad(pBuffer, 8 );      // 8 is no. of data bytes (there will be more, e.g. TA..)

   nError = Compare8Bytes(pBuf, pBuffer+3);
/*
   if( nError )
   {
      iprintf("\n Error Writing Scratchpad");
      for(i=0; i<8; i++)
      {
         iprintf("\n %x, %x", *(pBuf+i), *(pBuffer+i+3) );
      }
   }
   else
      iprintf("\n Write to Scratchpad OK");
*/

// need these bytes for ....
   DS2432Data.m_cTA1Byte = Buffer[0];
   DS2432Data.m_cTA2Byte = Buffer[1];
   DS2432Data.m_cESByte =  Buffer[2];
//   iprintf("\n Data on Scratchpad:");
   for(i=3; i<11; i++)
   {
     DS2432Data.m_cScratchpadBuf[i-3] = Buffer[i];  // copy scratchpad data to structure.
//testonly - readback data.
//     iprintf(" %x", DS2432Data.m_cScratchpadBuf[i-3]);
   }
//      iprintf("\n");
   DS2432Data.m_nCRC16 = ( Buffer[11]<<8 ) | Buffer[12];  // the CRC16

// verify the data is correct. ( i.e. written correctly to scratchpad)
/*
   for(i=0; i<8; i++)
   {
     if( *(pBuf+i) != Buffer[i+3])
     {
       nError++;  // copy scratchpad data to structure.
       iprintf("\nBad data: %d, %x, %x", i, *(pBuf+i), Buffer[i+3]);
      }
   }
*/

// now ready to Copy Scratchpad.
   if(!nError)
   {
      USER_ENTER_CRITICAL();    // OS call - will allow interrupts.

      OWTouchReset();

	     OWWriteByte(SKIP_ROM);		     //Write SKIP ROM command
 	     OWWriteByte(COPY_SCRATCHPAD);		//Copy Scratchpad command

   // write addresss (TA1 & TA2)
 	    OWWriteByte(DS2432Data.m_cTA1Byte);		//Write bytes as received from prev. read scratchpad cmd
 	    OWWriteByte(DS2432Data.m_cTA2Byte);
 	    OWWriteByte(DS2432Data.m_cESByte);

 	    // wait 3 mS
      Delay_uS( 3000 );   // 3mS - allow DS2432 to compute MAC (I assume).

//more to do here.
//COMPUTE mac.
 //  for(j=0; j<8; j++)
//   {
      // assumes secret is in the scructure.
//      DS2432Data.m_cSecretBuf[j] = 'A' + j;     // load up secret  // todo, //
//   }

//compute SHA-1 MAC
      if(nWriteAddress == REG_PAGE_ADDR)
         AssembleInputDataCopyScratchpadRegPage( pInputBuffer );   //NB input data different for reg page.
      else
         AssembleInputDataCopyScratchpad( pInputBuffer );         //NB: this only works for page 0.
/*
    iprintf("\nInputDataCopyScratchpad: \n");
     for( i=0; i<64; i++)
     {
          if( ((i) % 8) == 0  )
            iprintf("\n %d ", i);
        iprintf("%x ", InputBuffer[i] );     //
     }
*/
      ComputeSHAEE(InputBuffer, &A, &B,  &C,  &D, &E);
/*
      iprintf ("A: %x\n", A);
      iprintf ("B: %x\n", B);
      iprintf ("C: %x\n", C);
      iprintf ("D: %x\n", D);
      iprintf ("E: %x\n", E);
*/

 //     AssembleInputDataCopyScratchpad( pInputBuffer );
/*
    iprintf("\nInputDataCopyScratchpad: \n");
     for( i=0; i<64; i++)
     {
         if( ((i) % 8) == 0 )
            iprintf("\n %d ", i);
         iprintf("%x ", InputBuffer[i] );     //
     }
*/
      nShaErr = SHA1Reset(&ShaStruct);

      nShaErr = SHA1Input(&ShaStruct, InputBuffer, 64);

/*
      iprintf("\nAfter Sha1Input:");
     iprintf("\n ShA-1 result H0-H4:\n");
      iprintf ("H0: %x ", ShaStruct.Intermediate_Hash[0]);
      iprintf ("H1: %x ", ShaStruct.Intermediate_Hash[1]);
      iprintf ("H2: %x ", ShaStruct.Intermediate_Hash[2]);
      iprintf ("H3: %x ", ShaStruct.Intermediate_Hash[3]);
      iprintf ("H4: %x ", ShaStruct.Intermediate_Hash[4]);

    iprintf("SHA Context Input Buffer: \n");
     for( i=0; i<64; i++)
     {
         iprintf("  %x", ShaStruct.Message_Block[i] );     //
         if( ((i+1) % 4) == 0)
            iprintf("\n %d ", i+1);
     }
*/
 //    nShaErr = SHA1Result(&ShaStruct, Message_MAC);
    for(i = 0; i < SHA1HashSize; ++i)
    {
        Message_MAC[i] = ShaStruct.Intermediate_Hash[i>>2] >> 8 * ( 3 - ( i & 0x03 ) );
    }

//testonly
/*
    iprintf("\n  SHA Input Buffer: \n");
     for( i=0; i<64; i++)
     {
         iprintf(" %x", InputBuffer[i] );     //
         if( ((i+1) % 8) == 0)
            iprintf("\n");
     }
      iprintf("\nSHA1-MAC: ");
      for(i = 0; i < 20 ; i++)
      {
          iprintf("%02X ", Message_MAC[i]);
      }
      iprintf("\n");
     iprintf("\n ShA-1 result H0-H4:\n");
      iprintf ("H0: %x ", ShaStruct.Intermediate_Hash[0]);
      iprintf ("H1: %x ", ShaStruct.Intermediate_Hash[1]);
      iprintf ("H2: %x ", ShaStruct.Intermediate_Hash[2]);
      iprintf ("H3: %x ", ShaStruct.Intermediate_Hash[3]);
      iprintf ("H4: %x ", ShaStruct.Intermediate_Hash[4]);
*/
 //endtestonly.


//	Send MAC to DS2432....l.s byte first.
/*
      for(i = 0; i < 20 ; i++)
      {
          OWWriteByte( Message_MAC[19-i] );
          iprintf( "%2x ",Message_MAC[19-i] );
      }
*/

         int send_cnt = 0;

         // sending MAC
         Temp = E;
         for(i=0;i<4;i++)
         {
            Buffer[send_cnt++] = (BYTE) (Temp & 0x000000FF);
//            printf("%02X",Buffer[send_cnt-1]);
            Temp >>= 8;
         }
//         printf("\n");

         Temp = D;
         for(i=0;i<4;i++)
         {
            Buffer[send_cnt++] = (BYTE) (Temp & 0x000000FF);
//            printf("%02X",Buffer[send_cnt-1]);
            Temp >>= 8;
         }
//         printf("\n");

         Temp = C;
         for(i=0;i<4;i++)
         {
            Buffer[send_cnt++] = (BYTE) (Temp & 0x000000FF);
//            printf("%02X",Buffer[send_cnt-1]);
            Temp >>= 8;
         }
//         printf("\n");

         Temp = B;
         for(i=0;i<4;i++)
         {
            Buffer[send_cnt++] = (BYTE) (Temp & 0x000000FF);
//            printf("%02X",Buffer[send_cnt-1]);
            Temp >>= 8;
         }
//         printf("\n");

         Temp = A;
         for(i=0;i<4;i++)
         {
            Buffer[send_cnt++] = (BYTE) (Temp & 0x000000FF);
//            printf("%02X",Buffer[send_cnt-1]);
            Temp >>= 8;
         }
//         iprintf("\n");
      for(i = 0; i < 20 ; i++)
      {
          OWWriteByte( Buffer[i] );
//          iprintf( "%2x ",Buffer[i] );
      }


// wait 10 mS
      Delay_uS( 10000 );   // 10mS - allow DS2432 to write to eeprom.
      Delay_uS( 10000 );   // 1mS - give an extra mS to be sure.
// success should be getting 0 and 1's's, failure => 1'S
/*
      iprintf("\nResult: ");
      for(i = 0; i < 10 ; i++)
      {
          iprintf("%2x ", OWReadByte( ));
      }
*/
      OWTouchReset();         //NB   only for testing, see more to do

      USER_EXIT_CRITICAL();    // OS call - will allow interrupts.
    }
//check if data written..
/*
   ReadDS2432Memory(pBuffer, nWriteAddress, 8);
     iprintf("\nRead Memory Result: ");
     for(i = 0; i < 8 ; ++i)
      {
          iprintf("%2x ", Buffer[i] );
      }
*/
}
// Write Protect the Secret in the DS2432 - write AA to Address 0x88.
void WriteProtectSecret( void )
{
int i;
   BYTE  Buffer[8];
   BYTE  *pBuffer = Buffer;
   for(i=0; i<8; i++)
   {
      Buffer[i] = 0x00;
   }
   Buffer[0] = 0xAA;          //data = AA & 7 00's.

   CopyScratchpadDS2432( pBuffer, REG_PAGE_ADDR );     //Note: must write 8 bytes at a time.
}
// Function to test SHA. Reads data from addr. 0 in DS2432, changes it, writes back to
// DS2432 and verifies if written correctly.
bool TestSHAFunction( void )
{
BYTE ReadBuf[10];
BYTE WriteBuf[10];
int i;
bool bResult = FALSE;
int nRandomInt;
   ReadDS2432Memory(ReadBuf, 0x0000, 0x08);    // read curent data in DS2432
   for(i=0; i<8; i++)
   {
      nRandomInt = rand();
      WriteBuf[i] = ReadBuf[i] + (BYTE)nRandomInt;            // generate new data.
   }
   CopyScratchpadDS2432( WriteBuf, 0x0000 );    // Attempt to write data to DS2432 at addr 0.
   ReadDS2432Memory(ReadBuf, 0x0000, 0x08);    // read back data from DS2432

   if(Compare8Bytes(WriteBuf, ReadBuf) == 0) // 0 => data same, write worked.
      bResult = TRUE;
//testonly.
//   for(i=0; i<8; i++)
//      iprintf("\n %2x %2x", WriteBuf[i], ReadBuf[i]);
   return(bResult);
}

// Configured at the moment only for page 0.
void AssembleInputDataCopyScratchpad( BYTE *pBuf )
{
   int i,j;
   for(i=0,j=0; i<4; i++,j++)
   {
      // assumes secret is in the scructure.
      *(pBuf + i) = DS2432Data.m_cSecretBuf[j];
   }
   for(i=48,j=4; i<52; i++,j++)
   {
      // assumes secret is in the scructure.
      *(pBuf + i) = DS2432Data.m_cSecretBuf[j];
   }

   for(i=4,j=0; i<32; i++,j++)
   {
      // assumes Page 0 has been read into the scructure.
      *(pBuf + i) = DS2432Data.m_cPage0Buf[j];
   }

   for(i=32,j=0; i<40; i++,j++)
   {
      // assumes scratchpad is in the scructure.
      *(pBuf + i) = DS2432Data.m_cScratchpadBuf[j];
   }
//MP
   *(pBuf + 40) = 0x0000 + PAGE_0;             // we are writing into page 0.
   *(pBuf + 41) = DS2432Data.m_cIDBuf[0];

   for(i=1; i<7; i++)
   {
      // assumes ID is in the scructure.
      *(pBuf + 41 + i) = DS2432Data.m_cIDBuf[i];
   }

   for(i=52; i<55; i++)
   {
      // 3 FF's
      *(pBuf + i) = 0xFF;
   }

   *(pBuf + 55) = 0x80;             //

   for(i=56; i<62; i++)
   {
      // 6 00's
      *(pBuf + i) = 0x00;
   }
   *(pBuf + 62) = 0x01;             //
   *(pBuf + 63) = 0xB8;             //

}

// Configured  for register page
void AssembleInputDataCopyScratchpadRegPage( BYTE *pBuf )
{
int i,j;

   AssembleInputDataCopyScratchpad( pBuf );  // call the normal assemble input - will do 50%.
//M1 & M2
   for(i=0,j=4; i<8; i++,j++)
   {
      // assumes secret is in the scructure.
      *(pBuf + j) = DS2432Data.m_cSecretBuf[i];    // copy 8 bytes of secret into M1 & M2.
   }

//M3 & M4  - current contents of register page.
   for(i=0,j=12; i<8; i++,j++)
   {
      // assumes register page is in the scructure.
      *(pBuf + j) = DS2432Data.m_cRegisterPage[i];    // copy 8 bytes of reg. page into M3 & M4.
   }

//M5 & M6  - current contents of register No.
   for(i=0,j=20; i<8; i++,j++)
   {
      // assumes ID is in the scructure.
      *(pBuf + j) = DS2432Data.m_cIDBuf[i];    // copy 8 bytes of ID
   }

//M7  - 4 FF's
   for(i=0,j=28; i<4; i++,j++)
   {
      // assumes register page is in the scructure.
      *(pBuf + j) = 0xFF;    // copy 8 bytes of reg. page into M3 & M4.
   }
//M10
   *(pBuf+40) = 4;      // page no. for register page.


}








/* This wasn't working
void Delay_OneuS( void )
{
asm("   move.l #180000, %a0");
asm("PIFLOOP:");
asm("   move.w (a0), %d0");
asm(" 	btst #2, %d0");   //bit b2 = PIF.
asm(" 	beq PIFLOOP");
}
*/

 static short oddparity[16] = { 0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0 };

//--------------------------------------------------------------------------
// Calculate a new 1-wire CRC16 from the input data short.
//
// 'data'     - data to perform a CRC16 on
//
// Returns: the current CRC16
//
WORD Calculate1WCRC16(BYTE *pData, int nCount)
{
int i;
WORD n1WCRC16 = 0x0000;
WORD nData;
	for( i = 0; i < nCount; i++ )
	{
 		nData = (WORD)*(pData + i);			// get data char, cast to word (16bits)

      nData = (nData ^ (n1WCRC16 & 0xff)) & 0xff;
      n1WCRC16 >>= 8;

      if (oddparity[nData & 0xf] ^ oddparity[nData >> 4])
        n1WCRC16 ^= 0xc001;

      nData <<= 6;
      n1WCRC16   ^= nData;
      nData <<= 1;
      n1WCRC16   ^= nData;
   }
   return n1WCRC16;
}

//----------------------------------------------------------------------
// Compute the 160-bit MAC
//
// 'MT'  - input data
// 'A'   - part of the 160 bits
// 'B'   - part of the 160 bits
// 'C'   - part of the 160 bits
// 'D'   - part of the 160 bits
// 'E'   - part of the 160 bits
//
// Note: This algorithm is the SHA-1 algorithm as specified in the
// datasheet for the DS1961S, where the last step of the official
// FIPS-180 SHA routine is omitted (which only involves the addition of
// constant values).
//
//
void ComputeSHAEE(BYTE *MT, DWORD *A, DWORD *B,  DWORD *C,  DWORD *D, DWORD *E)
{
   DWORD MTword[80];
   int i;
   DWORD ShftTmp;
   DWORD Temp;

   for(i=0;i<16;i++)
      MTword[i] = (MT[i*4] << 24) | (MT[i*4+1] << 16) |
                  (MT[i*4+2] << 8) | MT[i*4+3];

   for(i=16;i<80;i++)
   {
      ShftTmp = MTword[i-3] ^ MTword[i-8] ^ MTword[i-14] ^ MTword[i-16];
      MTword[i] = ((ShftTmp << 1) & 0xfffffffe) |
                  ((ShftTmp >> 31) & 0x00000001);
   }

   *A=0x67452301;
   *B=0xefcdab89;
   *C=0x98badcfe;
   *D=0x10325476;
   *E=0xc3d2e1f0;

   for(i=0;i<80;i++)
   {
      ShftTmp = ((*A << 5) & 0xffffffe0) | ((*A >> 27) & 0x0000001f);
      Temp = NLF(*B,*C,*D,i) + *E + KTN(i) + MTword[i] + ShftTmp;
      *E = *D;
      *D = *C;
      *C = ((*B << 30) & 0xc0000000) | ((*B >> 2) & 0x3fffffff);
      *B = *A;
      *A = Temp;
   }
}

// calculation used for the MAC
DWORD KTN (int n)
{
   if(n<20)
      return 0x5a827999;
   else if (n<40)
      return 0x6ed9eba1;
   else if (n<60)
      return 0x8f1bbcdc;
   else
      return 0xca62c1d6;
}

// calculation used for the MAC
DWORD NLF ( DWORD B,  DWORD C,  DWORD D, int n)
{
   if(n<20)
      return ((B&C)|((~B)&D));
   else if(n<40)
      return (B^C^D);
   else if(n<60)
      return ((B&C)|(B&D)|(C&D));
   else
      return (B^C^D);
}




//----------------------------------------------------------------------
// Read Authenticated Page.
//
// 'portnum'     - number 0 to MAX_PORTNUM-1.  This number is provided to
//                 indicate the symbolic port number.
// 'address'     - address of page to do a read authenticate
// 'secret'      - current secret
// 'SerNum'      - serial number for the part
// 'data'        - buffer to how data read from page
// 'challenge'   - the challenge that is to be written to the scratchpad
//
// Return: 1 - MAC's agree
//         0 - MAC's dont agree.
//
BYTE ReadAuthPageDS2432(WORD nAddress, BYTE *secret, BYTE *SerNum,
                    BYTE *data, BYTE *challenge)
{
//   short send_cnt=0;
//   BYTE send_block[80];
   int i;
   DWORD A,B,C,D,E;
   DWORD Temp;
   BYTE MT[64],CalculatedMACBuffer[20];
   BYTE memory[32],scratch[8];      //,es
   WORD tmpadd;
//   WORD lastcrc16;
   WORD nCRC16[1];
   WORD *ptrCRC16 = nCRC16;
   BYTE  Buffer[40];
   BYTE  *pBuffer = Buffer;
	WORD	nReceivedCRC16, nRxMACCRC16, nCalMACCRC16, nCalDataCRC16;
	WORD  nWordTemp;
	BYTE  nTemp;
   BYTE  ucResult = 0;

   iprintf("\nS ec ret: ");
   for(i=0;i<8;i++)
      iprintf("%x ",secret[i]);

	WriteDS2432Scratchpad(challenge, nAddress, 8, ptrCRC16);

//   if(!WriteScratchSHAEE(portnum,address,challenge,8))
//      return FALSE;

   for(i=0;i<4;i++)
      MT[i] = secret[i];

   ReadDS2432Memory(memory, (nAddress & 0x00E0), 32);		// read full page containing nAddress. MMK.

//   if(ReadMem(portnum,(WORD)((address/((WORD)32))*32),memory))
      for(i=4;i<36;i++)
         MT[i] = memory[i-4];
//   else
//   {
//      iprintf("ERROR DUE TO READING MEMORY PAGE DATA\n");
//      return FALSE;
//   }

   for(i=36;i<40;i++)
      MT[i] = 0xFF;
//m
   ReadDS2432Scratchpad(pBuffer, 8 );      // 8 is no. of data bytes (there will be more, e.g. TA..)
// need these bytes for ....
   DS2432Data.m_cTA1Byte = Buffer[0];
   DS2432Data.m_cTA2Byte = Buffer[1];
   DS2432Data.m_cESByte =  Buffer[2];
	for(i=0; i<8; i++)
	{
		scratch[i] = Buffer[i+3];
	}

	tmpadd =  (Buffer[1] << 8) | Buffer[0]; 	// regenerating the address.

//   iprintf("\n Data on Scratchpad:");
//   for(i=3; i<11; i++)
//   {
//     DS2432Data.m_cScratchpadBuf[i-3] = Buffer[i];  // copy scratchpad data to structure.

//m
//   if(ReadScratchSHAEE(portnum, &tmpadd, &es, scratch))
//   {

      MT[40] = (0x40 | (BYTE)(((tmpadd << 3) & 0x08) |
                       (BYTE)((tmpadd >> 5) & 0x07)));

      for(i=41;i<48;i++)
         MT[i] = SerNum[i-41];
      for(i=48;i<52;i++)
         MT[i] = secret[i-44];
      for(i=52;i<55;i++)
         MT[i] = scratch[i-48];
//   }
//   else
//   {
//      iprintf("ERROR DUT TO READING SCRATCH PAD DATA\n");
//      return FALSE;
//   }

   MT[55] = 0x80;

   for(i=56;i<62;i++)
      MT[i] = 0x00;

   MT[62] = 0x01;
   MT[63] = 0xB8;
//testing
    iprintf("\n  Read Auth. Page - SHA Input Buffer: \n");
     for( i=0; i<64; i++)
     {
         iprintf(" %x", MT[i] );     //
         if( ((i+1) % 8) == 0)
            iprintf("\n");
     }

//testing end
   ComputeSHAEE(MT,&A,&B,&C,&D,&E);
     iprintf("\n Read Auth. Page - SHA1-MAC: \n");
         iprintf("%X %X %X %X %X ", A,B,C,D,E);
      iprintf("\n");

        int j = 0;

         // Disassemble MAC into buffer
         Temp = E;
         for(i=0;i<4;i++)
         {
            CalculatedMACBuffer[j++] = (BYTE) (Temp & 0x000000FF);
            Temp >>= 8;
         }

         Temp = D;
         for(i=0;i<4;i++)
         {
            CalculatedMACBuffer[j++] = (BYTE) (Temp & 0x000000FF);
            Temp >>= 8;
         }

         Temp = C;
         for(i=0;i<4;i++)
         {
            CalculatedMACBuffer[j++] = (BYTE) (Temp & 0x000000FF);
            Temp >>= 8;
         }

         Temp = B;
         for(i=0;i<4;i++)
         {
            CalculatedMACBuffer[j++] = (BYTE) (Temp & 0x000000FF);
            Temp >>= 8;
         }

         Temp = A;
         for(i=0;i<4;i++)
         {
            CalculatedMACBuffer[j++] = (BYTE) (Temp & 0x000000FF);
            Temp >>= 8;
         }
      iprintf("\n Calculated - SHA1-MAC: \n");
        for( i=0; i<20; i++)
        {
            iprintf(" %x", CalculatedMACBuffer[i] );     //
        }

// MMK from here.
    USER_ENTER_CRITICAL();    // OS call - will hold off interrupts.

	if (!( OWTouchReset() ) ) // Reset the 1-Wire bus (Presence pulse => 0)
	{

	OWWriteByte(SKIP_ROM);		     //Write SKIP ROM command

	OWWriteByte(READ_AUTH_PAGE);		//Read auth. page command.

		Buffer[0] =  READ_AUTH_PAGE;

// write addresss (TA1 & TA2)
   nTemp =tmpadd & 0x00F8;   // L.S. byte of addres, 3 l.s. bits should be 0.
 	OWWriteByte(nTemp);		//Write L.S. byte of address

		Buffer[1] =  nTemp;

   nTemp = tmpadd >> 8;   //M.S. byte of address
 	OWWriteByte(nTemp);		//Write m.S. byte of address

		Buffer[2] =  nTemp;

		Buffer[0] =  READ_AUTH_PAGE;
		for(i=0; i < 35; i++)			// Read 32 bytes page data + FF + inverted CRC.
		{
			Buffer[i+3] = OWReadByte( );  // read n bytes of data.
		}

      nReceivedCRC16 = (Buffer[37] << 8) | Buffer[36];
//data crc16 stuff
      nWordTemp = Calculate1WCRC16( Buffer, 36 );  // CRC on CMD, Address(2bytes), data(32bytes) and FF byte. (36total)
      nCalDataCRC16 = ~nWordTemp;                // note the inversion. (Dallas use the inverted cRC16)

      iprintf("\nRx CRC = %x Cal CRC = %04x", nReceivedCRC16, nCalDataCRC16);

      Delay_uS( 3000 );   // 3mS - give DS2432 time to compute MAC.

 // Now read MaC from dS2432.

		for(i=0; i < 22; i++)			// Read 22 bytes 20 bytes MAC + inverted CRC.
		{
			Buffer[i] = OWReadByte( );  // read n bytes of data.
		}
     iprintf("\n Read Auth. Page - Returned MAC: & CRC \n");
     for (i=0; i < 22; i++)
         iprintf("%X ", Buffer[i]);
      iprintf("\n");
//MAC crc16 stuff
      nRxMACCRC16 = (Buffer[21] << 8) | Buffer[20];
      nWordTemp = Calculate1WCRC16( Buffer, 20 );
      nCalMACCRC16 = ~nWordTemp;                // note the inversion. (Dallas use the inverted cRC16)

      iprintf("\nRx CRC = %x Cal CRC = %04x", nRxMACCRC16, nCalMACCRC16);

		nTemp = OWReadByte( );		// should now receive 1's and 0's
       iprintf("\n %2x\n", nTemp);

     OWTouchReset();

//compare MACs
     ucResult = 1;
     for (i=0; i < 20; i++)
     {
      if(Buffer[i] != CalculatedMACBuffer[i] )
         ucResult = 0;     // set to zero if any byte doesnt match.
     }
     if(!ucResult)
        iprintf("\n Fail - MAC's dont Match\n");
     else
        iprintf("\n OK - MAC's Match\n");

    }

    USER_EXIT_CRITICAL();    // OS call - will allow interrupts.

// MMK to here.


	 /*
   // access the device
   if (SelectSHAEE(portnum) == 1)
   {
      setcrc16(portnum,0);

      // Read Authenticated Command
      send_block[send_cnt] = 0xA5;
      lastcrc16 = docrc16(portnum,send_block[send_cnt++]);

      // address 1
      send_block[send_cnt] = (BYTE)(tmpadd & 0xFF);
      lastcrc16 = docrc16(portnum,send_block[send_cnt++]);
      // address 2
      send_block[send_cnt] = (BYTE)((tmpadd >> 8) & 0xFF);
      lastcrc16 = docrc16(portnum,send_block[send_cnt++]);

      // data + FF byte
      for (i = 0; i < 35; i++)
         send_block[send_cnt++] = 0xFF;

      // now send the block
      if (owBlock(portnum,FALSE,send_block,send_cnt))
      {
         // calculate CRC16 of result
         for (i = 3; i < send_cnt; i++)
            lastcrc16 = docrc16(portnum,send_block[i]);

         // verify CRC16 is correct
         if (lastcrc16 != 0xB001)
         {
            iprintf("FIRST CRC TEST FAILED %04X\n",lastcrc16);
            return FALSE;
         }

         for(i=3;i<35;i++)
            data[i-3] = send_block[i];

         send_cnt = 0;
         for(i=0;i<22;i++)
            send_block[send_cnt++] = 0xFF;

         if(owBlock(portnum,FALSE,send_block,send_cnt))
         {
            // calculate CRC16 of result
            setcrc16(portnum,0);
            for (i = 0; i < send_cnt ; i++)
               lastcrc16 = docrc16(portnum,send_block[i]);

            // verify CRC16 is correct
            if (lastcrc16 != 0xB001)
            {
               iprintf("SECOND CRC TEST FAILED %04X\n",lastcrc16);
               return FALSE;
            }

            send_cnt = 0;
            Temp = E;
            for(i=0;i<4;i++)
            {
               if(send_block[send_cnt++] != (BYTE) (Temp & 0x000000FF))
               {
                  iprintf("COMPARING MAC FAILED\n");
                  return FALSE;
               }
               Temp >>= 8;
            }

            Temp = D;
            for(i=0;i<4;i++)
            {
               if(send_block[send_cnt++] != (BYTE) (Temp & 0x000000FF))
               {
                  iprintf("COMPARING MAC FAILED\n");
                  return FALSE;
               }
               Temp >>= 8;
            }

            Temp = C;
            for(i=0;i<4;i++)
            {
               if(send_block[send_cnt++] != (BYTE) (Temp & 0x000000FF))
               {
                  iprintf("COMPARING MAC FAILED\n");
                  return FALSE;
               }
               Temp >>= 8;
            }

            Temp = B;
            for(i=0;i<4;i++)
            {
               if(send_block[send_cnt++] != (BYTE) (Temp & 0x000000FF))
               {
                  iprintf("COMPARING MAC FAILED\n");
                  return FALSE;
               }
               Temp >>= 8;
            }

            Temp = A;
            for(i=0;i<4;i++)
            {
               if(send_block[send_cnt++] != (BYTE) (Temp & 0x000000FF))
               {
                  iprintf("COMPARING MAC FAILED\n");
                  return FALSE;
               }
               Temp >>= 8;
            }

            return TRUE;
         }

      }
   }
	*/

   return ucResult;
}

// GetNBBBoardRevision returns board rev no. either  NBB_REV_B1 or  NBB_REV_B2
// NB: Function uses USER_ENTER_CRITICAL()  OS call - will hold off interrupts, only use at startup.
BYTE GetNBBBoardRevision( void )
{
   BYTE IDBuffer[10];
   BYTE *pIDBuffer = IDBuffer;
   bool bRevB1BoardFound = FALSE;
   int nMatchingIDDBytes = 0;
//3 Rev B1 board ID's.
   BYTE  RevB1ID_1[7] = {0x33, 0x3F, 0xFE, 0xe4, 0x00, 0x00, 0x00};
   BYTE  RevB1ID_2[7] = {0x33, 0x50, 0x0f, 0xe5, 0x00, 0x00, 0x00};
   BYTE  RevB1ID_3[7] = {0x33, 0xe6, 0x0e, 0xe5, 0x00, 0x00, 0x00};

   int i;

           ReadDS2432ID( pIDBuffer );       // read board ID.
           nMatchingIDDBytes = 0;
           for( i= 0; i < 7; i++ )        // Check for 1st Rev B1 board.
           {
               if(IDBuffer[i] == RevB1ID_1[i] )
                  nMatchingIDDBytes++;
//                  iprintf(" %x", IDBuffer[i] );       // print the 8 bytes returned               if(IDBuffer[i] == RevB1ID_1[i] )
           }
           if( nMatchingIDDBytes == 7)
               bRevB1BoardFound = TRUE;

         nMatchingIDDBytes = 0;
           for( i= 0; i < 7; i++ )                 // Check for 2d Rev B1 board.
           {
               if(IDBuffer[i] == RevB1ID_2[i] )
                  nMatchingIDDBytes++;
           }
           if( nMatchingIDDBytes == 7)
               bRevB1BoardFound = TRUE;

         nMatchingIDDBytes = 0;
           for( i= 0; i < 7; i++ )
           {
               if(IDBuffer[i] == RevB1ID_3[i] )      // Check for 3rd Rev B1 board.
                  nMatchingIDDBytes++;
           }
           if( nMatchingIDDBytes == 7)
               bRevB1BoardFound = TRUE;

           if( bRevB1BoardFound )
           {
//               iprintf("\n Rev B1 found");
               return( NBB_REV_B1 );
            }
            else
               return( NBB_REV_B2 );

}

/*
Mods to SBB to fit DS2432
1. Remove U12 (under EPROM)
2. Remove Q1
3. Remove R9   (May need to remove R11 - DON'T for now)
4. Fit 2N7000 FET in Q1 position wrong way around. - (compared to silk screen).
5. Fit DS2432 in U12 position:
   DS2432 mounted on perspex.
   DS2432 pin 1 connects to U12 pin 4.
   DS2432 pin 2 connects to U12 pin 3.
   Connect U12 pin 3 to U12 pin 1.


*/


