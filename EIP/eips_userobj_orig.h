/*
 *            Copyright (c) 2002-2009 by Real Time Automation, Inc.
 *
 *  This software is copyrighted by and is the sole property of
 *  Real Time Automation, Inc. (RTA).  All rights, title, ownership,
 *  or other interests in the software remain the property of RTA.
 *  This software may only be used in accordance with the corresponding
 *  license agreement.  Any unauthorized use, duplication, transmission,
 *  distribution, or disclosure of this software is expressly forbidden.
 *
 *  This Copyright notice MAY NOT be removed or modified without prior
 *  written consent of RTA.
 *
 *  RTA reserves the right to modify this software without notice.
 *
 *  Real Time Automation
 *  150 S. Sunny Slope Road            USA 262.439.4999
 *  Suite 130                          http://www.rtaautomation.com
 *  Brookfield, WI 53005               software@rtaautomation.com
 *
 *************************************************************************
 *
 *    Version Date: 12/16/2009
 *         Version: 2.14
 *    Conformed To: EtherNet/IP Protocol Conformance Test A-7 (17-AUG-2009)
 *     Module Name: eips_userobj.h
 *          Author: Jamin D. Wendorf (jwendorf@rtaautomation.com)
 *        Language: Ansi C
 * Compile Options: N/A
 * Compile defines: N/A
 *       Libraries: N/A
 *    Link Options: N/A
 *
 * Description.
 * =======================================================================
 * This file contains user object model definitions.
 *
 */

#ifndef __EIPS_USEROBJ_H__
#define __EIPS_USEROBJ_H__

/* *************************** */
/* Identity Object Definitions */
/* *************************** */

/* Vendor ID is "Real Time Automation" 0x0032 */
#define EIPS_USER_IDOBJ_VENDORID   0x0032
/* Device Types is "Generic Device" 0x00 */
#define EIPS_USER_IDOBJ_DEVTYPE    0x0000
/* Product Code is unique for each product */
#define EIPS_USER_IDOBJ_PRODCODE   3
/* Product Name is 1-byte length followed by the name string */
#define EIPS_USER_IDOBJ_NAME_SIZE  17			// this must be lenght of ID string below.
                            /*0        1	 2	   3 */
                            /*12345678901234567890123456789012 */
//#define EIPS_USER_IDOBJ_NAME "RTA Sample Code"
// TSM
#define EIPS_USER_IDOBJ_NAME "TSM Batch Blender"

/* *************************** */
/* Vendor Specific Definitions */
/* *************************** */
/* Assembly data can be in bytes (8-bit), words (16-bit) or longs (32-bit). */
/* ALL ASSEMBLIES MUST USE THE SAME TYPE!!!! */
/*#define EIPS_USEROBJ_ASM_ELEMENT_SIZE 1 */	/* bytes */
#define EIPS_USEROBJ_ASM_ELEMENT_SIZE 2			/* words */
/*#define EIPS_USEROBJ_ASM_ELEMENT_SIZE 4 */	/* double words */

#if EIPS_USEROBJ_ASM_ELEMENT_SIZE == 4      /* DWORDS */
    #define VS_MAX_DISCRETE_INPUT_UNITS  1
    #define VS_MAX_DISCRETE_OUTPUT_UNITS 1
#elif EIPS_USEROBJ_ASM_ELEMENT_SIZE == 2    /* WORDS */
    #define VS_MAX_DISCRETE_INPUT_UNITS  1
    #define VS_MAX_DISCRETE_OUTPUT_UNITS 1
#else                                       /* BYTES */
    #define VS_MAX_DISCRETE_INPUT_UNITS  1
    #define VS_MAX_DISCRETE_OUTPUT_UNITS 1
#endif

/* ******************************************************** */
/* START ASSEMBLY DEFINES */
/* ******************************************************** */

/* define to include Listen Only and Input Only I/O Connections */
#define EIPS_USEROBJ_ASM_INPUTONLY_HB_O2TINST  0x80 /* (128 dec) define to include Input Only I/O Connections */
#define EIPS_USEROBJ_ASM_LISTENONLY_HB_O2TINST 0x81 /* (129 dec) define to include Listen Only I/O Connections */

/* define the max assembly instance used */
#define EIPS_USEROBJ_ASM_MAXINST            0x81    /* (129 dec) */

/* O2T Assemblies transfer data from the network client to us  */
#define EIPS_USEROBJ_ASM_MAXNUM_O2TINST     1  /* can be set to 0 */

/* T2O Assemblies transfer data from us to the network client.  */
#define EIPS_USEROBJ_ASM_MAXNUM_T2OINST     1  /* must be at least 1 (data size of 0 allowed) */

/* ******************************************************** */
/* END ASSEMBLY DEFINES */
/* ******************************************************** */

/* uncomment the below lines to support the DIP (0x08) and/or DOP (0x09) objects */
#define EIPS_USEROBJ_DIP_SUPPORTED  1
#define EIPS_USEROBJ_DOP_SUPPORTED  1

/* ====================================================================
0x70 DICRETE INPUT DATA OBJECT ATTRIBUTES (VENDOR SPECIFIC)
======================================================================= */
#define VS_OBJ_DISCRETE_INPUT_DATA  0x70
typedef struct
{
    struct
    {
/* ATTR 01 */ uint16 ClassRev;	    /* Class Revision */
    }Class;

    struct
    {
/* ATTR 01 */ uint16 DataSize; /* Number of Input Data Units (0-n) */
/* ATTR 02 */ /* unused */

#if EIPS_USEROBJ_ASM_ELEMENT_SIZE == 4      /* DWORDS */
    /* ATTR 03 */ uint32 DataPtr[VS_MAX_DISCRETE_INPUT_UNITS]; /* Input Data (uint32) */
#elif EIPS_USEROBJ_ASM_ELEMENT_SIZE == 2    /* WORDS */
    /* ATTR 03 */ uint16 DataPtr[VS_MAX_DISCRETE_INPUT_UNITS]; /* Input Data (uint16) */
#else                                       /* BYTES */
    /* ATTR 03 */ uint8  DataPtr[VS_MAX_DISCRETE_INPUT_UNITS]; /* Input Data (uint8) */
#endif
    }Inst;
}EIPS_VS_DISCRETE_INPUT_DATA_STRUCT;

/* = ===================================================================
0x71 DISCRETE OUTPUT DATA ATTRIBUTES (VENDOR SPECIFIC)
======================================================================= */
#define VS_OBJ_DISCRETE_OUTPUT_DATA 0x71
typedef struct
{
  struct
  {
/* ATTR 01 */ uint16 ClassRev;	     /* Class Revision */
  }Class;

  struct
  {
/* ATTR 01 */ uint16 DataSize; /* Number of Output Data Units (0-n) */
/* ATTR 02 */ /* unused */

#if EIPS_USEROBJ_ASM_ELEMENT_SIZE == 4      /* DWORDS */
    /* ATTR 03 */ uint32 DataPtr[VS_MAX_DISCRETE_OUTPUT_UNITS]; /* Output Data (uint32) */
#elif EIPS_USEROBJ_ASM_ELEMENT_SIZE == 2    /* WORDS */
    /* ATTR 03 */ uint16 DataPtr[VS_MAX_DISCRETE_OUTPUT_UNITS]; /* Output Data (uint16) */
#else                                       /* BYTES */
    /* ATTR 03 */ uint8  DataPtr[VS_MAX_DISCRETE_OUTPUT_UNITS]; /* Output Data (uint8) */
#endif
  }Inst;
}EIPS_VS_DISCRETE_OUTPUT_DATA_STRUCT;

#endif /* __EIPS_USEROBJ_H__ */
