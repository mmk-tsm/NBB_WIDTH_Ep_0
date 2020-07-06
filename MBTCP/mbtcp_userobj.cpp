/*********************************************************************
Module:  mbtcp_userobj.c
Author:  Jamin D. Wendorf     12/2003
         (c)2003 Real Time Automation

This file contains register and coil data functions.
*********************************************************************** */
// M.McKiernan                      17/9/07
// Removed g_arrnMBTable[] & g_arrnMBTableFlags[], now in BatVars2
// g_nMBWriteOrFlag, g_nMBWriteOrErrorFlag, g_nMBWriteOrFlagTemp

// M.McKiernan                      26/9/07
// Redefined IN_REGISTER_SIZE_IN_WORDS (MB_TABLE_SIZE)
// Removed all the printf's   see //mmk
//
// P.Smith                              26/11/07
// g_bModbusLengthWritten set if length is written to modbus table
// added BATCH_CONTROL_SCREW_SPEED check in writenwrd & write1wrd, g_bModbusScrewSpeedWritten flag
// is set if this word is written.
//
// P.Smith                          24/6/08
// remove g_arrnMBTable
//
// P.Smith                          29/9/08
// added liquid additive commands for d/a and d/a % write.
//
// P.Smith                          16/10/08
// correct compiler warning
//
// P.Smith                          13/11/08
// added PrintModbusTCPMessage to print modbus message written to telnet.
//
// P.Smith                          17/6/09
// allow for download of data for vacuum loading set point data
// check for vacuum loader block and set g_bVacuumLoaderCalibrationWritten
// this will then initiate the download of the data to eeprom.
//
// P.Smith                          22/7/09
// set g_bICSRecipeDataWritten if ics data written
//
// P.Smith                          15/4/10
// added check for BATCH_SET_LICENSE_OPTIONS_1-4 written and set flag indicating write
//
// P.Smith                          13/5/10
// added check for Width Config data written and set flags to indicate that the write has
// happened
//
// P.Smith                          7/7/10
// added ultrasonic ref and diameter commands
//
// P.Smith                          19/7/10
// added offset sign,offset,diameter from lay flat entry
//
// P.Smith                          16/9/10
// added width set point command
//
/* INCLUDE FILES                */
/* ---------------------------- */
#include "rta_system.h"
#include "mbtcp_system.h"
#include "BatchMBIndices.h"
#include "MBProgs.h"
#include "MBSHand.h"
#include "Batvars2.h"
#include "PrntDiag.h"
#include "Vacvars.h"
#include "WidthMB.h"
#include "WidthVars.h"

extern BOOL	g_bLicenseOptions1Written;
extern BOOL	g_bLicenseOptions2Written;
extern BOOL	g_bLicenseOptions3Written;
extern BOOL	g_bLicenseOptions4Written;


BYTE MBTCPWriteNWords(WORD nStartAddress, WORD nNumwords );


/* ---------------------------- */
/* LOCAL STRUCTURE DEFINITIONS  */
/* ---------------------------- */

/* ---------------------------- */
/* STATIC VARIABLES             */
/* ---------------------------- */
#define IN_COIL_SIZE_IN_BYTES 20    //125
static uint8 MBTCP_InCoils[IN_COIL_SIZE_IN_BYTES];

#define OUT_COIL_SIZE_IN_BYTES 20   //125
static uint8 MBTCP_OutCoils[OUT_COIL_SIZE_IN_BYTES];

#define IN_REGISTER_SIZE_IN_WORDS (MB_TABLE_SIZE)    //(MB_TABLE_RO_SIZE)
static uint16 MBTCP_InRegisters[IN_REGISTER_SIZE_IN_WORDS];

#define OUT_REGISTER_SIZE_IN_WORDS (MB_TABLE_WRITE_SIZE)
static uint16 MBTCP_OutRegisters[OUT_REGISTER_SIZE_IN_WORDS];


//MMK - 11.July.2007....testonly
//#define MB_TABLE_SIZE 1000
//uint16 g_arrnMBTable2[MB_TABLE_SIZE];
/* ---------------------------- */
/* LOCAL FUNCTIONS              */
/* ---------------------------- */

/* ---------------------------- */
/* MISCELLANEOUS                */
/* ---------------------------- */

/**/
/* ******************************************************************** */
/*                      GLOBAL FUNCTIONS                                */
/* ******************************************************************** */
/* ====================================================================
Function:     mbtcp_userobj_init
Parameters:   N/A
Returns:      N/A

This function initialize all user object model variables.
======================================================================= */
void mbtcp_userobj_init (void)
{
   uint16 i;

   // default the input reg values to be the address
   for(i=0; i<IN_REGISTER_SIZE_IN_WORDS; i++)
      MBTCP_InRegisters[i] = i;

   // default the input coil values to be the address
   for(i=0; i<IN_COIL_SIZE_IN_BYTES; i++)
      MBTCP_InCoils[i] = (uint8)i;

   // default the output reg value to 0
   for(i=0; i<OUT_REGISTER_SIZE_IN_WORDS; i++)
      MBTCP_OutRegisters[i] = 0;

   // default the output coil values to 0
   for(i=0; i<OUT_COIL_SIZE_IN_BYTES; i++)
      MBTCP_OutCoils[i] = (uint8)0;

//MMK - 11.July.2007....testonly
   // default the MB table values to 1000 + 3 times reg no.
//   for(i=0; i<MB_TABLE_SIZE; i++)
//      g_arrnMBTable2[i] = 1000 + 3*i;

   // print the current configuration
   if(NUMBER_OF_INCOILS > 0)
		;
//mmk      printf("%5d Input Coils       -> 0X Reference (%d-%d)\n",NUMBER_OF_INCOILS, STARTING_INCOIL_ADDR,(STARTING_INCOIL_ADDR + NUMBER_OF_INCOILS - 1));
   if(NUMBER_OF_OUTCOILS > 0)
		;
//mmk      printf("%5d Output Coils      -> 1X Reference (%d-%d)\n",NUMBER_OF_OUTCOILS, STARTING_OUTCOIL_ADDR,(STARTING_OUTCOIL_ADDR + NUMBER_OF_OUTCOILS - 1));
   if(NUMBER_OF_OUTREGS > 0)
		;
//mmk      printf("%5d Holding Registers -> 4X Reference (%d-%d)\n",NUMBER_OF_OUTREGS, STARTING_OUTREG_ADDR,(STARTING_OUTREG_ADDR + NUMBER_OF_OUTREGS - 1));
   if(NUMBER_OF_INREGS > 0)
		;
//mmk      printf("%5d Input Registers   -> 3X Reference (%d-%d)\n",NUMBER_OF_INREGS, STARTING_INREG_ADDR,(STARTING_INREG_ADDR + NUMBER_OF_INREGS - 1));

   // print the function codes that are supported
//mmk   printf("\nFunction Codes Supported:\n");

   #if defined(FC01_RDCOILSTAT)
		;
//mmk   printf("  01: Read Coil Status\n");
   #endif

   #if defined(FC02_RDINPUTSTAT)
		;
//mmk   printf("  02: Read Input Status\n");
   #endif

   #if defined(FC03_RDHOLDREG)
		;
//mmk   printf("  03: Read Holding Registers\n");
   #endif

   #if defined(FC04_RDINPUTREG)
		;
//mmk   printf("  04: Read Input Registers\n");
   #endif

   #if defined(FC05_WRSINGLECOIL)
		;
//mmk   printf("  05: Force Single Coil\n");
   #endif

   #if defined(FC06_WRSINGLEREG)
		;
//mmk   printf("  06: Preset Single Register\n");
   #endif

   #if defined(FC07_RDEXCEPTSTAT)
		;
//mmk   printf("  07: Read Exception Status\n");
   #endif

   #if defined(FC16_WRMULTREGS)
		;
//mmk   printf("  16: Preset Multiple Registers\n");
   #endif
}

/* ====================================================================
Function:     mbtcp_userobj_getInputCoilPtr
Parameters:   requested size (in bits)
Returns:      pointer to Input Coil Data Table
              (NULL on error)

This function verifies enough data is allocated, then returns the
address of the Input Coil data table.
======================================================================= */
uint8 *mbtcp_userobj_getInputCoilPtr (uint16 size_in_bits)
{
   uint16 size_in_bytes;

   // max size
   if(size_in_bits > 65528)
      size_in_bytes = 8192;

   // round up on the number of bytes
   size_in_bytes = (size_in_bits+7) / 8;

   // too much data requested, or 0 data requested, return an error
   if((size_in_bytes > IN_COIL_SIZE_IN_BYTES) || (size_in_bytes == 0))
      return(NULL);

   // success, return the pointer to the input coils
   return(((uint8 *)&MBTCP_InCoils[0]));
}

/* ====================================================================
Function:     mbtcp_userobj_getOutputCoilPtr
Parameters:   requested size (in bits)
Returns:      pointer to Output Coil Data Table
              (NULL on error)

This function verifies enough data is allocated, then returns the
address of the Output Coil data table.
======================================================================= */
uint8 *mbtcp_userobj_getOutputCoilPtr (uint16 size_in_bits)
{
   uint16 size_in_bytes;

   // max size
   if(size_in_bits > 65528)
      size_in_bytes = 8192;

   // round up on the number of bytes
   size_in_bytes = (size_in_bits+7) / 8;

   // too much data requested, or 0 data requested, return an error
   if((size_in_bytes > OUT_COIL_SIZE_IN_BYTES) || (size_in_bytes == 0))
      return(NULL);

   // success, return the pointer to the input coils
   return(((uint8 *)&MBTCP_OutCoils[0]));
}

/* ====================================================================
Function:     mbtcp_userobj_getInputRegPtr
Parameters:   requested size (in words)
Returns:      pointer to Input Register Data Table
              (NULL on error)

This function verifies enough data is allocated, then returns the
address of the Input Register data table.
======================================================================= */
uint16 *mbtcp_userobj_getInputRegPtr (uint16 size_in_words)
{
   // too much data requested, or 0 data requested, return an error
   if((size_in_words > IN_REGISTER_SIZE_IN_WORDS) || (size_in_words == 0))
      return(NULL);

   // success, return the pointer to the input coils
// MMK - 11.July.2007.... testonly
//   return(((uint16 *)&MBTCP_InRegisters[0]));
   return(((uint16 *)&g_arrnMBTable[0]));    //BATCH_SUMMARY_START

}

/* ====================================================================
Function:     mbtcp_userobj_getOutputRegPtr
Parameters:   requested size (in words)
Returns:      pointer to Output Register Data Table
              (NULL on error)

This function verifies enough data is allocated, then returns the
address of the Output Register data table.
======================================================================= */
uint16 *mbtcp_userobj_getOutputRegPtr (uint16 size_in_words)
{
   // too much data requested, or 0 data requested, return an error
   if((size_in_words > OUT_REGISTER_SIZE_IN_WORDS) || (size_in_words == 0))
      return(NULL);

   // success, return the pointer to the input coils
//   return(((uint16 *)&MBTCP_OutRegisters[0]));
// MMK - 11.July.2007.... testonly
   return(((uint16 *)&g_arrnMBTable[0]));

}

/* ====================================================================
Function:     mbtcp_userobj_coilsWritten
Parameters:   starting address (1-based)
              length in bits
Returns:      N/A

This function is called when coil data is written to the output coil
table.  The address and length of the coil data that was written is
passed.
======================================================================= */
void mbtcp_userobj_coilsWritten (uint16 start_addr, uint16 length_in_bits)
{
	;
//mmk   printf("%d coils written starting at %d.\n",length_in_bits, start_addr);
}

/* ====================================================================
Function:     mbtcp_userobj_registersWritten
Parameters:   starting address (1-based)
              length in bits
Returns:      N/A

This function is called when register data is written to the output
register table.  The address and length of the register data that was
written is passed.
======================================================================= */
void mbtcp_userobj_registersWritten (uint16 start_addr, uint16 length_in_bits)
{

   BYTE cErrorCode;
   cErrorCode = MBTCPWriteNWords(start_addr, length_in_bits );
   PrintModbusTCPMessage(start_addr, length_in_bits);
//      SetupMttty();
//      printf("\n Statr Addr = %d, Len = %d", start_addr, length_in_bits );
//   unsigned int i;
//   for(i=0; i<length_in_bits; i++)
//   {
		//mmk
//     printf("\n MB Addr: %d  Value = %ld", start_addr+i, g_arrnMBTable[i+start_addr-1]);
//   }
}

/* ====================================================================
Function:     mbtcp_userobj_getExceptionStatus
Parameters:   N/A
Returns:      8-bit exception status

This function returns the exception status for the device.
======================================================================= */
uint8 mbtcp_userobj_getExceptionStatus (void)
{
   static uint8 exc_stat = 1;
   exc_stat++;
   return(exc_stat);
}

/**/
/* ******************************************************************** */
/*                      LOCAL FUNCTIONS                                 */
/* ******************************************************************** */

/* *********** */
/* END OF FILE */
/* *********** */
#define SPTS_INHIBIT_DOWNLOAD (1)
#define SPTS_TRIGGER_DOWNLOAD (2)

BYTE MBTCPWriteNWords(WORD nStartAddress, WORD nNumwords )
{

//    unsigned char nNumbytes,nNumwords;                   //,BlockStartAddr, MaxWriteAddr
	unsigned char j;     // kb,x,
	WORD	k;
//	WORD i=7;                       // point to 7th byte  in Rx1buff
	WORD MBByteIndex;               // byte index in TSM MB table, 0 based.
//	union   CharsAndWord     MBRegisterAddress;
//	union   CharsAndWord     uData;

   BYTE cErrorCode = 0;
/*
	Tx1Buff[0] = g_cChannelAddr;
	Tx1Buff[1] = g_cMBRx1Func;
	Tx1Buff[2] = Rx1Buff[2];
	Tx1Buff[3] = Rx1Buff[3];               // address of 1st word
	Tx1Buff[4] = Rx1Buff[4];
	Tx1Buff[5] = Rx1Buff[5];               // no. of words to  write

	nNumbytes = Rx1Buff[6];    // bytes to write
	nNumwords = Rx1Buff[5];    // words to write

	MBRegisterAddress.cValue[0] = Rx1Buff[2];
	MBRegisterAddress.cValue[1] = Rx1Buff[3];
*/

	MBByteIndex = nStartAddress-1;    // word address in MB Table (NB: RTA stuff is 1 based)
//	MaxWriteAddr = 0 + MBBLOCKWSIZ*2;
//      SetupMttty();
//      printf("\n MBByteIndex = %d, BATCH_LAST_WRITE_REGISTER = %d", MBByteIndex, BATCH_LAST_WRITE_REGISTER );

    if((MBByteIndex + nNumwords) > BATCH_LAST_WRITE_REGISTER)		// last writeable addres.
	{
		return(ILLEGAL_MB_DATA_ACCESS);    // error, illegal address
	}
	else
	{
      //	Distribute the required data
   if( g_arrnMBTable[BATCH_SETPOINT_DOWNLOAD_MANAGER] == 0 )  // i.e. dont do if trigger download set.
   {
        g_nMBWriteOrFlagTemp = 0;

   }
	for(j = 0; j < nNumwords; j++)
		{
         k = MBByteIndex + j;

//		  	uData.cValue[0]= Rx1Buff[i];          				// read msb
//        checkcheck  i++;
//		  	uData.cValue[1] = Rx1Buff[i+1];         			// read lsb
//		  	g_arrnMBTable[k] = uData.nValue; 						// move data to modbus table
		  	g_nMBWriteOrFlagTemp |= g_arrnMBTableFlags[k];	// set the approp. bit by oring with the corresponding MB flag.
//      printf("\n  = %x, = %x", g_nMBWriteOrFlagTemp, g_arrnMBTableFlags[k] );

//        	i += 2;
            if(k == BATCH_CONTROL_LINE_SPEED)
            {
                g_bModbusLineSpeedWritten = TRUE;
            }
            if(k == BATCH_CONTROL_WRITE_DAC)
            {
                g_bModbusDToAWritten = TRUE;
            }
             if(k == BATCH_DTOA_PERCENTAGE)
            {
                g_bModbusDToAPercentageWritten = TRUE;
            }
            if(k == TOGGLE_STATUS_COMMAND2_LIQUID_ADDITIVE_COMMAND_SET_DTOA)
            {
                g_bModbusLiquidDToAWritten = TRUE;
            }
            if(k == TOGGLE_STATUS_COMMAND2_LIQUID_ADDITIVE_COMMAND_SET_DTOA_PERCENTAGE)
            {
                g_bModbusLiquidDToAPercentageWritten = TRUE;
            }
            if(k == TOGGLE_STATUS_COMMAND2_LENGTH)
            {
                g_bModbusLengthWritten = TRUE;
            }

            if(k == BATCH_CONTROL_SCREW_SPEED)
            {
                g_bModbusScrewSpeedWritten = TRUE;
            }
            if((k >= VACUUM_LOADER_START_CALIBRATION_BLOCK) && (k <= VACUUM_LOADER_END_CALIBRATION_BLOCK))
            {
                g_bVacuumLoaderCalibrationWritten = TRUE;
            }
            if((k >= BATCH_ICS_RECIPE_START) && (k <= BATCH_ICS_RECIPE_END))
            {
                g_bICSRecipeDataWritten = TRUE;
            }
            if(k == BATCH_SET_LICENSE_OPTIONS_1)
             {
                 g_bLicenseOptions1Written = TRUE;
             }
             if(k == BATCH_SET_LICENSE_OPTIONS_2)
             {
                 g_bLicenseOptions2Written = TRUE;
             }
             if(k == BATCH_SET_LICENSE_OPTIONS_3)
             {
            	 g_bLicenseOptions3Written = TRUE;
             }
             if(k == BATCH_SET_LICENSE_OPTIONS_4)
             {
            	 g_bLicenseOptions4Written = TRUE;
             }

             if((k >= MB_WIDTH_CONFIG_START) && (k <= MB_WIDTH_CONFIG_END))
             {
                 g_bWidthConfigWritten = TRUE;
             }
             if(k == WIDTH_CONFIG_CALIBRATE_WIDTH)
             {
             	g_bWidthCalibrateInConfigWritten = TRUE;
             }
             if(k == WIDTH_CONFIG_MAXIMUM_WIDTH)
             {
             	g_bMaxWidthCalibrateInConfigWritten = TRUE;
             }
             if(k == WIDTH_COMMAND_AUTO_TOGGLE_COMMAND)
             {
             	g_bWidthAutoManualToggleCommandWritten = TRUE;
             }
             if(k == WIDTH_COMMAND_INCREASE)
             {
             	g_bWidthIncreaseCommandWritten = TRUE;
             }
             if(k == WIDTH_COMMAND_FAST_CORRECTION_INCREASE)
             {
             	g_bWidthIncreaseFastCorrectionCommandWritten = TRUE;
             }
             if(k == WIDTH_COMMAND_DECREASE)
             {
             	g_bWidthDecreaseCommandWritten = TRUE;
             }

             if(k == WIDTH_COMMAND_CALIBRATE_ULTRASONIC_REF)
             {
             	g_bWidthUltrasonicRefCommandWritten = TRUE;
             }

             if(k == WIDTH_COMMAND_CALIBRATE_DIAMETER)
             {
            	 g_bWidthUltrasonicDiameterCommandWritten = TRUE;
             }
             if(k == WIDTH_COMMAND_US_LAYFLAT_OFFSET_SIGN)
             {
            	 g_bWidthUltrasonicLayFlatOffsetSignWritten = TRUE;
             }
             if(k == WIDTH_COMMAND_US_LAYFLAT_OFFSET)
             {
            	 g_bWidthUltrasonicLayFlatOffsetWritten = TRUE;
             }
             if(k == WIDTH_COMMAND_US_DIAMETER_FROM_LAYFLAT_ENTRY)
             {
             	g_bWidthUltrasonicDiameterFromLayFlatEntry = TRUE;
             }
             if(k == WIDTH_COMMAND_WIDTH_SETPOINT)
             {
             	g_bWidthSetPointWritten = TRUE;
             }
		}
		g_nMBWriteOrFlagTemp &= 0x7FFF;						// clear the m.s. bit.

//      printf("\n OrFlagTemp %x", g_nMBWriteOrFlagTemp );

       if(g_arrnMBTable[BATCH_SETPOINT_DOWNLOAD_MANAGER] == 0 || g_arrnMBTable[BATCH_SETPOINT_DOWNLOAD_MANAGER] == SPTS_TRIGGER_DOWNLOAD)
        {
		    cErrorCode = CheckMBValidData();						// some checks to see if data written is valid. (asm = CHMBVDATA)
        }
        else
        {
            cErrorCode = 0;
        }
//      printf("\n OrFlag %x", g_nMBWriteOrFlag );

//      printf("\n cErrorCode %x", cErrorCode );

//		g_nTx1Ptr = 6;                              // tx1ptr point to next vacant byte
//		g_cVAC350MBUpdateFlag = 1;		// indicate the write modbus command is initiated
		return(cErrorCode);
	}

}



