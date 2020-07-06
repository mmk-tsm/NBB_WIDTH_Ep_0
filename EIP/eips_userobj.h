/*
 *            Copyright (c) 2002-2009 by Real Time Automation, Inc.
 *
 *  This software is copyrighted by and is the sole property of
 *  Real Time Automation, Inc. (RTA).  All rights, title, ownership,
 *  or other interests in the software remain the property of RTA.
 *  This software may only be used in accordance with the corresponding
 *  license agreement.  Any unauthorised use, duplication, transmission,
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
// M. McKiernan				23.12.2009
// Defined EIPS_VS_RECIPE_DATA_STRUCT - object for Recipe data....at testing stage only.
// VS_OBJ_DOWNLOAD_RECIPE_DATA  0x64
// #define EIPS_USER_IDOBJ_NAME "TSM Batch Blender"    ....for ID object.
// define EIPS_USER_IDOBJ_NAME_SIZE  17	// this must be lenght of ID string below.

// M. McKiernan				18.01.2010
// Defined EIPRecipeStruct -
// EIPS_VS_RECIPE_DATA_STRUCT - object for Recipe data..this is used in Download and current recipe obj structures.
// NB - Defined sheet/tube as UINT16 - uint16      m_nSheetTube;
// Defined EIPS_VS_PRODUCTION_DATA_STRUCT; and EIPS_VS_CONTROL_DATA_STRUCT;
// Defined following objects:
//		define VS_OBJ_DOWNLOAD_RECIPE_DATA  	0x64 structure =
//		define VS_OBJ_CURRENT_RECIPE_DATA  	0x65
//		define VS_OBJ_PRODUCTION_DATA  		0x66
//		define VS_OBJ_CONTROL_DATA  			0x67

// M. McKiernan				29.01.2010
// This version has been included in Batch software - Previous version in standalone application (NBB_EIP)
// Main changes relate to Assembly object. See - //Emily27012010
// Added function prototypes void TestAsmData( void); void local_update_produce_asm (void);
// #define EIPS_USEROBJ_ASM_ELEMENT_SIZE 1 - size was 2.
// New Defines:
// 		#define EIPS_USEROBJ_ASM_INST_O2T1	    0x70
//		#define EIPS_USEROBJ_ASM_SIZE_O2T1	    4   	// size is in words (formated for PCCC)
//		#define EIPS_USEROBJ_ASM_INST_T2O1	    0x64
//		//#define EIPS_USEROBJ_ASM_SIZE_T2O1	    8   	// size is in words (formated for PCCC)
//		mmk28012010. changed to;
//		#define EIPS_USEROBJ_ASM_SIZE_T2O1	    128   	// size is in words (formated for PCCC)
//
// Added 3 spare attributes in  EIPRecipeStruct;    m_nRecipeSpareAttribute5,6,7; //ATTR 28-30


// M. McKiernan				3.02.2010
// Edited EIPS_VS_CONTROL_DATA_STRUCT structure
// Added m_nSetIncrease and m_nSetDecrease & 3 spare commands.
// Defined Bits to indicate that control command have been written from Ethernet/IP
// from EIP_INCREASE_WROTE			(0x0001) to  EIP_START_CLEAN_WROTE		(0x0040)


// M. McKiernan				15.02.2010
// Added extra parameter  in EIPS_VS_PRODUCTION_DATA_STRUCT. uint16	m_nSpareParameter1;		//Attr47

void TestAsmData( void);
void local_update_produce_asm (void);


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
                            /*12345678901234567890*/
#define EIPS_USER_IDOBJ_NAME "TSM Batch Blender"

/* *************************** */
/* Vendor Specific Definitions */
/* *************************** */
/* Assembly data can be in bytes (8-bit), words (16-bit) or longs (32-bit). */
/* ALL ASSEMBLIES MUST USE THE SAME TYPE!!!! */

//Emily27012010
#define EIPS_USEROBJ_ASM_ELEMENT_SIZE 1 //	/* bytes */	-
//#define EIPS_USEROBJ_ASM_ELEMENT_SIZE 2			/* words */
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
//Emily27012010
#define EIPS_USEROBJ_ASM_INST_O2T1	    0x70
#define EIPS_USEROBJ_ASM_SIZE_O2T1	    4   	// size is in words (formated for PCCC)


/* T2O Assemblies transfer data from us to the network client.  */
#define EIPS_USEROBJ_ASM_MAXNUM_T2OINST     1  /* must be at least 1 (data size of 0 allowed) */
//Emily27012010
#define EIPS_USEROBJ_ASM_INST_T2O1	    0x64
//#define EIPS_USEROBJ_ASM_SIZE_T2O1	    8   	// size is in words (formated for PCCC)
//mmk28012010.
#define EIPS_USEROBJ_ASM_SIZE_T2O1	    128   	// size is in words (formated for PCCC)


/* ******************************************************** */
/* END ASSEMBLY DEFINES */
/* ******************************************************** */

/* uncomment the below lines to support the DIP (0x08) and/or DOP (0x09) objects */
#define EIPS_USEROBJ_DIP_SUPPORTED  1
#define EIPS_USEROBJ_DOP_SUPPORTED  1

// Bits to indicate that control command have been written from Ethernet/IP
#define EIP_INCREASE_WROTE			(0x0001)
#define EIP_DECREASE_WROTE			(0x0002)
#define EIP_AUTO_MANUAL_WROTE		(0x0004)
#define EIP_EXTRUDER_DAC_WROTE		(0x0008)
#define EIP_SCREW_SPEED_WROTE		(0x0010)
#define EIP_LINE_SPEED_WROTE		(0x0020)
#define EIP_START_CLEAN_WROTE		(0x0040)


/* EMILY MODIFY START */
/* I'll do 4 attributes with as an example with 5 different data types */
/* ====================================================================
0x64 RECIPE DATA OBJECT ATTRIBUTES (VENDOR SPECIFIC)
======================================================================= */
typedef struct {
        float   m_fPercentage1;				//ATTR01
        float   m_fPercentage2;				//ATTR02
        float   m_fPercentage3;				//ATTR03
        float   m_fPercentage4;				//ATTR04
        float   m_fPercentage5;				//ATTR05
        float   m_fPercentage6;				//ATTR06
        float   m_fPercentage7;				//ATTR07
        float   m_fPercentage8;				//ATTR08
        float   m_fPercentage9;				//ATTR09
        float   m_fPercentage10;				//ATTR10
        float   m_fPercentage11;				//ATTR11
        float   m_fPercentage12;				//ATTR12

        uint16  m_nRegrindComponent;		//ATTR13  // component no. used for regrind.

        float       m_fTotalThroughput;		//ATTR14
        float       m_fTotalWeightLength;	//ATTR15
        float       m_fTotalWeightArea;		//ATTR16

        float       m_fLineSpeed;			//ATTR17
        float       m_fDesiredWidth;		//ATTR18
        uint16      m_nSheetTube;			//ATTR19
    	uint16      m_nFileNumber;			//ATTR 20

        uint16		m_nResetTotals;			//ATTR21	(71 in Modbus)
        uint16		m_nLoadRecipeCommand;	//ATTR22	(74 in Modbus)

       	uint16     m_nRecipeSpareAttribute1; //ATTR 23
       	uint16     m_nRecipeSpareAttribute2; //ATTR 24

        float       m_fRecipeSpareAttribute3; //ATTR25
        float       m_fRecipeSpareAttribute4; //ATTR26

        char		m_arrDescription[21];	  //ATTR27

       	uint16     m_nRecipeSpareAttribute5; //ATTR 28
       	uint16     m_nRecipeSpareAttribute6; //ATTR 29
       	uint16     m_nRecipeSpareAttribute7; //ATTR 30

}EIPRecipeStruct;

#define VS_OBJ_DOWNLOAD_RECIPE_DATA  	0x64
#define VS_OBJ_CURRENT_RECIPE_DATA  	0x65
#define VS_OBJ_PRODUCTION_DATA  		0x66
#define VS_OBJ_CONTROL_DATA  			0x67

typedef struct
{
    struct
    {
/* ATTR 01 */ uint16 ClassRev;	    /* Class Revision */
    }Class;

    EIPRecipeStruct	Inst;			//Note defined externally because use in 2nd object.

}EIPS_VS_RECIPE_DATA_STRUCT;
/* EMILY MODIFY END */


//  Production Summary data object

typedef struct
{
    struct
    {
    	uint16 ClassRev;	    // Class Revision  ATTR 01
    }Class;

    struct
    {
        float   m_fActualThroughput;				//ATTR01
        float   m_fActualWPL;						//ATTR02 wt/length
        float   m_fActualWPA;						//ATTR03 wt/area.
        float   m_fActualOrderWeight;				//ATTR04
        float   m_fActualLineSpeed;					//ATTR05
        uint16  m_nOperationalStatus;				//ATTR06
        uint16  m_nMotorDA;							//ATTR07
        uint16  m_nOperationalStatus2;				//ATTR08
        uint16	m_nBatchCounter;					//Attr09
        uint16	m_nProcessAlarms;					//Attr10

        float   m_fActualPercentage1;				//ATTR11
        float   m_fActualPercentage2;				//ATTR12
        float   m_fActualPercentage3;				//ATTR13
        float   m_fActualPercentage4;				//ATTR14
        float   m_fActualPercentage5;				//ATTR15
        float   m_fActualPercentage6;				//ATTR16
        float   m_fActualPercentage7;				//ATTR17
        float   m_fActualPercentage8;				//ATTR18
        float   m_fActualPercentage9;				//ATTR19
        float   m_fActualPercentage10;				//ATTR20
        float   m_fActualPercentage11;				//ATTR21
        float   m_fActualPercentage12;				//ATTR22

        float   m_fActualWeight1;					//ATTR23
        float   m_fActualWeight2;					//ATTR24
        float   m_fActualWeight3;					//ATTR25
        float   m_fActualWeight4;					//ATTR26
        float   m_fActualWeight5;					//ATTR27
        float   m_fActualWeight6;					//ATTR28
        float   m_fActualWeight7;					//ATTR29
        float   m_fActualWeight8;					//ATTR30
        float   m_fActualWeight9;					//ATTR31
        float   m_fActualWeight10;					//ATTR32
        float   m_fActualWeight11;					//ATTR33
        float   m_fActualWeight12;					//ATTR34

        float   m_fActualOrderWeight1;				//ATTR35
        float   m_fActualOrderWeight2;				//ATTR36
        float   m_fActualOrderWeight3;				//ATTR37
        float   m_fActualOrderWeight4;				//ATTR38
        float   m_fActualOrderWeight5;				//ATTR39
        float   m_fActualOrderWeight6;				//ATTR40
        float   m_fActualOrderWeight7;				//ATTR41
        float   m_fActualOrderWeight8;				//ATTR42
        float   m_fActualOrderWeight9;				//ATTR43
        float   m_fActualOrderWeight10;				//ATTR44
        float   m_fActualOrderWeight11;				//ATTR45
        float   m_fActualOrderWeight12;				//ATTR46

        uint16	m_nSpareParameter1;					//Attr47


    }Inst;
}EIPS_VS_PRODUCTION_DATA_STRUCT;


//  CONTROL data object

typedef struct
{
    struct
    {
    	uint16 ClassRev;	    // Class Revision  ATTR 01
    }Class;

    struct
    {
    	//uint16   m_nSetIncDec;				//ATTR01
    	uint16   m_nSetIncrease;			//ATTR01
    	uint16   m_nSetDecrease;			//ATTR02
    	uint16   m_nSetAutoManual;			//ATTR03
    	uint16   m_nSetExtruderDAC;			//ATTR04
    	float    m_fSetCommsScrewSpeed;		//ATTR05
    	float    m_fSetCommsLineSpeed;		//ATTR06
    	uint16   m_nStartClean;				//ATTR07
    	uint16   m_nSpareCommand1;			//ATTR08
       	uint16   m_nSpareCommand2;			//ATTR09
       	uint16   m_nSpareCommand3;			//ATTR010

    }Inst;
}EIPS_VS_CONTROL_DATA_STRUCT;



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




