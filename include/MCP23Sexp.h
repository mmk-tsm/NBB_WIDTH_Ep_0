/*-------------------------------------------------------------------
 File: MCP23Sexp.h
 Description: McP23S08/17 SPI expander function definitions and descriptions.
 
 P.Smith                          12/1/09
 added MCP23S17_AT_ADDRESS_4
 use addressing information when accessing MCP23S17
 -------------------------------------------------------------------*/

#ifndef __MCP23SEXP_H__
#define __MCP23SEXP_H__


#define MCP23S08_NBB_ADDRESS (0x00)
#define MCP23S08_EXP_ADDRESS_3 (0x03)     // SO8 address bits = 11, i.e 0x03.
#define MCP23S17_AT_ADDRESS_3 (0x03)      //MCP23s17 address bits = 011.
#define MCP23S17_AT_ADDRESS_4 (0x04)      //MCP23s17 address bits = 100.  link out = 1


#define MCP23S08_GPIO (0x09)
#define MCP23S08_IODIR (0x00)
#define MCP23S08_IOCON (0x05)

#define MCP23S17_GPIOA (0x09)
#define MCP23S17_IODIRA (0x00)
#define MCP23S17_IOCON (0x0A)
#define MCP23S17_IOCONA (0x04)   // this assumes that bank bit = 1.

/******************************************************************
 * This function will 
 
 ******************************************************************/
BYTE ReadMCP23S08GPIO( void );

BYTE ReadExpansionMCP23S08GPIO( void );

BYTE MCP23S08Read1Byte(WORD chipSelectUsed, BYTE S08Address, BYTE RegisterNo);

void MCP23S08Write1Byte(WORD chipSelectUsed, BYTE S08Address, BYTE RegisterNo, BYTE WriteData );

void WriteOnNBBMCP23S08( BYTE RegisterNo, BYTE WriteData );

void InitialiseOnNBBMCP23S08( void );

void SetOnNBBMCP23S08_GP7( void );

void ResetOnNBBMCP23S08_GP7( void );

void InitialiseOffNBBMCP23S08( void );

void WriteOffNBBMCP23S08( BYTE RegisterNo, BYTE WriteData );

void SetOffNBBMCP23S08_GP7( void );

void ResetOffNBBMCP23S08_GP7( void );


void WriteNBBEXMCP23S17( BYTE RegisterNo,BYTE S08Address,BYTE WriteData );

void InitialiseNBBEXMCP23S17( BYTE S08Address );

void NBBEXMCP23S17AllOutputsOn( BYTE S08Address );

void NBBEXMCP23S17AllOutputsOff(BYTE S08Address );

void NBBEXMCP23S17ScrollOutputs( BYTE S08Address );
#endif // __MCP23Sexp_H__


