/* ********************************************************************
Module:  mbtcp_userobj.h
Author:  Jamin D. Wendorf     12/2003
         (c)2003 REAL TIME AUTOMATION

This file contains user object model definitions

Michael McKiernan			25.9.2007
			Defined NUMBER_OF_INREGS & NUMBER_OF_OUTREGS for TSM MB table. 
*********************************************************************** */



#ifndef __MBTCP_USEROBJ_H__
#define __MBTCP_USEROBJ_H__


#include "BatchMBIndices.h" 

/* uncomment the following definitions for each function supported */

// Class 0 Function Codes
#define FC03_RDHOLDREG    1
#define FC16_WRMULTREGS   1

// Class 1 Function Codes
// #define FC01_RDCOILSTAT   1
// #define FC02_RDINPUTSTAT  1
#define FC04_RDINPUTREG   1
// #define FC05_WRSINGLECOIL 1
#define FC06_WRSINGLEREG  1
// #define FC07_RDEXCEPTSTAT 1

/* 
   There are 4 data tables available in Modbus
     Input Discretes  - bits/coils - read only
     Output Discretes - bits/coils - read/write
     Input Registers  - 16-bit values - read only
     Output Registers - 16-bit values - read/write 

   Input/Output Discretes often overlay each other.
   Input/Output Registers often overlay each other.
   Discretes may also overlay Registers.

   This is decided in "mbtcp_userobj.c" where the data 
   tables are defined.

   The total number of discretes/registers + the starting
   location must not go above 65535,
 
   Starting address start at 1, not 0.
*/

// Define the input coil data range
#define STARTING_INCOIL_ADDR  1
#define NUMBER_OF_INCOILS     0  //1000

// Define the output coil data range
#define STARTING_OUTCOIL_ADDR 1
#define NUMBER_OF_OUTCOILS    0  //1000

// Define the input register data range
#define STARTING_INREG_ADDR   1
#define NUMBER_OF_INREGS    	(MB_TABLE_SIZE)	//  1000

// Define the output register data range
#define STARTING_OUTREG_ADDR  1
#define NUMBER_OF_OUTREGS     (MB_TABLE_SIZE)	//  1000

#endif /* __MBTCP_USEROBJ_H__ */
