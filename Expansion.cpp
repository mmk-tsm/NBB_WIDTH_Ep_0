//
//   P.Smith                                               22/6/07
//   add g_bSPIBusBusy, proper blender names inserted
//
//   P.Smith                                               22/6/07
//   added write to expansion card No 2.
//   a flag is set g_bWriteToExpansion2 in one hz, when the spi is free, the
//   write to the expansion card is initiated.
//   CopyExpansion2MBData translates "air blast" and "bin drain" into outputs.
//   if "air blast" is active set output and reset after the blast time.
//
//   P.Smith                                               15/10/09
//   name change nBlastOutput & nDrainOutput
//   copy in outputs for vac 8 expansion g_nVAC8ExpansionOutput
//   copy expansion data to expansion card is enabled.
//
//   P.Smith                                               25/2/10
//   check for license before running expansion stuff for bin clean drain
//   function

#include <startnet.h>
#include <basictypes.h>
#include <stdio.h>
#include "MCP23Sexp.h"
#include "Batvars.h"
#include "Batvars2.h"
#include "BatchCalibrationFunctions.h"
#include "Expansion.h"
#include "TSMPeripheralsMBIndices.h"
#include "License.h"

extern CalDataStruct    g_CalibrationData;
extern   int g_arrnWriteVAC8IOMBTable[MAX_VAC8IO_CARDS][VAC8IO_TABLE_WRITE_SIZE];
extern  OnBoardCalDataStruct   g_OnBoardCalData;

//////////////////////////////////////////////////////
// UpdateExpanIO  asm = UPDATEEXPANIO
//
//
// P.Smith                          29/5/07
// expansion update called on blender pit at frequency of 1500hz.
//
// M. McKiernan                      20/6/07
//  see ....//mmk - 7/6/2007 - added SPIBusBusy flag...cannot run if SPI is busy.
//

//////////////////////////////////////////////////////


void UpdateExpanIO( void )
{
  //mmk - 7/6/2007 - added SPIBusBusy flag...cannot run if SPI is busy.
    if(g_bExpanioFitted && !g_bSPIEEActive  && !g_bSPIBusBusy )
    {
        SPI_Select_EXPANSION1();
        SPI_RS422_ENABLE_LOW();
        WriteNBBEXMCP23S17( MCP23S17_GPIOA,MCP23S17_AT_ADDRESS_3, g_nExpansionOutput );     // gpa0-gpa7 set TO 1's.
        SPI_RS422_ENABLE_HIGH();

    }
    if(!g_bSPIBusBusy && !g_bSPIEEActive && g_bWriteToExpansion2)
    {
        g_bWriteToExpansion2 = FALSE;
        CopyExpansion2MBData();
        WriteToExpansionCard2();
    }
}

void DecideIfExpansionisRequired( void )
{
    if((g_CalibrationData.m_nBlenderType == TSM3000) && (g_CalibrationData.m_nComponents) > 8 )
    {
        g_bExpanioFitted = TRUE;
    }
    else
    {
        g_bExpanioFitted = FALSE;
    }
}

//////////////////////////////////////////////////////
// CopyExpansion2MBData
// Copy expansion commands from mb to output format.
//////////////////////////////////////////////////////

void CopyExpansion2MBData( void )
{
    unsigned int    x,y,i,nBlastOutput,nDrainOutput;
    x = 1;
    y = 1;
    if((g_OnBoardCalData.m_wBlenderOptions & LICENSE_BINCLEANOPTIONBIT) != 0)
    {
    	nBlastOutput = 0;
    	nDrainOutput = 0;

    	for(i=0; i < MAX_COMPONENTS; i++)    //--review-- modbus expan;sion
    	{
    		if (g_arrnMBTable[BATCH_AIR_BLAST_BIN_1+i] != 0)
    		{
    			if(g_nBlastMomentaryOnCtr[i] == 0)
    			{
    				g_nBlastMomentaryOnCtr[i] = BLAST_ON_TIME_HZ;
    				nBlastOutput |= x;
    			}
    			else
    			{
    				g_nBlastMomentaryOnCtr[i]--;
    				if(g_nBlastMomentaryOnCtr[i] == 0)
    				{
    					g_arrnMBTable[BATCH_AIR_BLAST_BIN_1+i] = 0;
    					nBlastOutput &= ~x;
    				}
    			}
    		}
    		else
    		{
    			nBlastOutput &= ~x;
    		}
    		x <<= 1;
    	}
    	y = 1;
    	for(i=0; i < MAX_COMPONENTS; i++)    //--review-- modbus expan;sion
    	{
    		if (g_arrnMBTable[BATCH_DRAIN_BIN_1+i] != 0)
    		{
    			nDrainOutput |= y;
    		}
    		else
    		{
    			nDrainOutput &= ~y;
    		}
    		y <<= 1;
    	}
    	g_nExpansionOutput2 = (nBlastOutput & 0x0f)|(nDrainOutput << 4);

    	g_nVAC8ExpansionOutput = (nBlastOutput & 0xff)|(nDrainOutput << 8);
    	if(g_CalibrationData.m_bVac8Expansion)
    	{
    		g_arrnWriteVAC8IOMBTable[g_nVAC8ExpansionIndex][MB_VAC8IO_COMMAND1] = g_nVAC8ExpansionOutput;   //1st card
    	}
//    if(fdTelnet >0)
//    iprintf("\n writing %x to index %d",g_nVAC8ExpansionOutput,g_nVAC8ExpansionIndex);
    }
}



//////////////////////////////////////////////////////
// WriteToExpansionCard2
//
//////////////////////////////////////////////////////

void WriteToExpansionCard2( void )
{
    SPI_Select_EXPANSION1();
    SPI_RS422_ENABLE_LOW();
    WriteNBBEXMCP23S17( MCP23S17_GPIOA,MCP23S17_AT_ADDRESS_4, g_nExpansionOutput2 );     // gpa0-gpa7 set TO 1's.
    SPI_RS422_ENABLE_HIGH();
}

