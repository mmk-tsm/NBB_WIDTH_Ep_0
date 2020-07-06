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
 *     Module Name: eips_userobj.c
 *          Author: Jamin D. Wendorf (jwendorf@rtaautomation.com)
 *        Language: Ansi C
 * Compile Options: N/A
 * Compile defines: N/A
 *       Libraries: N/A
 *    Link Options: N/A
 *
 * Description.
 * =======================================================================
 * This file contains vendor specific object model definitions.
 *
 */
// M. McKiernan				23.12.2009
// File modified initially by Emily Brockman of RTA - See references to Emily
// Function added - InitEIPSDownloadRecipeDataObj() Hard codes recipe data for testing.
// Major change in local_get_attr() function to get or read data from recipe data object.
// Emily modified the local_set_attr() function - I have not looked at this yet.


// M. McKiernan				18.01.2010
// Declared following objects/structures;
//EIPS_VS_RECIPE_DATA_STRUCT 		eips_vs_DownloadRecipeDataObj;
//EIPS_VS_RECIPE_DATA_STRUCT 		eips_vs_CurrentRecipeDataObj;
//EIPS_VS_PRODUCTION_DATA_STRUCT	eips_vs_ProductionDataObj;
//EIPS_VS_CONTROL_DATA_STRUCT		eips_vs_ControlDataObj;
// Added following funcitons;
// void InitEIPSDownloadRecipeDataObj(void );
// void InitEIPSCurrentRecipeDataObj (void );
// void InitEIPSProductionDataObj(void );
// void InitEIPSControlDataObj(void );
// Called the oject init's in eips_userobj_init().
// Edited function local_get_attr ( ...) to cater for TSM objects, i.e.
//  case VS_OBJ_DOWNLOAD_RECIPE_DATA:, case VS_OBJ_CURRENT_RECIPE_DATA:, case VS_OBJ_PRODUCTION_DATA:, case VS_OBJ_CONTROL_DATA
// Edited function local_set_attr (....) to cater for TSM write (download) objects, i.e.
//  case VS_OBJ_DOWNLOAD_RECIPE_DATA:, case VS_OBJ_CONTROL_DATA:


// M. McKiernan				29.01.2010
// This version has been included in Batch software - Previous version in standalone application (NBB_EIP)
// Main changes relate to Assembly object.
// See - //Emily27012010
// 		local_t2o_asm_struct[0].size = EIPS_USEROBJ_ASM_SIZE_T2O1; // old value 1;
//    	local_o2t_asm_struct[0].size = EIPS_USEROBJ_ASM_SIZE_O2T1; // old value 1;
// Change to function  eips_userobj_asmDataWrote ().
// Added 3 extra (spare)attributes (Attr28,29,30) to recipe objects -  m_nRecipeSpareAttribute5, 6, 7.
// Added function local_update_produce_asm (void) -This function is when the produce assembly needs to be updated
//

// M. McKiernan				03.02.2010
// Edited reading and writing of Control data object to cater for structure changes
// 					m_nSetIncrease;			//ATTR01
// 					m_nSetDecrease;			//ATTR02
//    		uint16   m_nSpareCommand1;			//ATTR08
//       	uint16   m_nSpareCommand2;			//ATTR09
//       	uint16   m_nSpareCommand3;			//ATTR010
// Added movement to Modbus table for conrol object data
// g_nEIPToggleStatusCommandWritten flag set when each piece of data written into Control object.
//

// M. McKiernan				11.02.2010
// In eips_userobj_asmDataWrote () function, dont store downloaded data, i.e. dont provide
// Originator to Target (OtoT) assembly object.
// All printf's qualified by: if( fdTelnet>0 && g_nTSMDebug.m_bEIPDebug )


// M. McKiernan				15.02.2010
// Added case 47: //Spare Parameter rta_PutLitEndian16(eips_vs_ProductionDataObj.Inst.m_nSpareParameter1 in production data object
// And is Assembly object:
// 	    	 	rta_PutLitEndian16(eips_vs_ProductionDataObj.Inst.m_nSpareParameter1, &local_t2o_asm_struct[0].data_ptr[126]);

// P.Smith				10/3/10
// increment g_nEIPCtr
/* ---------------------------- */
/*      INCLUDE FILES           */
/* ---------------------------- */
#include "eips_system.h"
#include "General.h"
#include "SetpointFormat.h"
#include "eips_userobj.h"

#include "BatVars2.h"
#include "BatVars.h"
#include "BatchMBIndices.h"
#include "TimeDate.h"
extern unsigned int	g_nEIPCtr;
extern unsigned int	g_nEIPAssemblyObjectCtr;

/* ---------------------------- */
/* LOCAL STRUCTURE DEFINITIONS	*/
/* ---------------------------- */

/* ---------------------------- */
/* STATIC VARIABLES             */
/* ---------------------------- */
#if EIPS_USEROBJ_ASM_MAXNUM_O2TINST > 0
    static struct
    {
        uint16 instID;
        uint16 size;    /* in data_ptr units */

    #if EIPS_USEROBJ_ASM_ELEMENT_SIZE == 4      /* DWORDS */
        uint32 data_ptr[125];
    #elif EIPS_USEROBJ_ASM_ELEMENT_SIZE == 2    /* WORDS */
        uint16 data_ptr[250];
    #else                                       /* BYTES */
        uint8  data_ptr[500];
    #endif
    }local_o2t_asm_struct[EIPS_USEROBJ_ASM_MAXNUM_O2TINST];
#endif

static struct
{
    uint16 instID;
    uint16 size;    /* in data_ptr units */

#if EIPS_USEROBJ_ASM_ELEMENT_SIZE == 4      /* DWORDS */
    uint32 data_ptr[125];
#elif EIPS_USEROBJ_ASM_ELEMENT_SIZE == 2    /* WORDS */
    uint16 data_ptr[250];
#else                                       /* BYTES */
    uint8  data_ptr[500];
#endif
}local_t2o_asm_struct[EIPS_USEROBJ_ASM_MAXNUM_T2OINST];

EIPS_VS_DISCRETE_INPUT_DATA_STRUCT  eips_vs_discreteInDataObj;
EIPS_VS_DISCRETE_OUTPUT_DATA_STRUCT eips_vs_discreteOutDataObj;

/* EMILY MODIFY START */
EIPS_VS_RECIPE_DATA_STRUCT eips_vs_DownloadRecipeDataObj;
/* EMILY MODIFY END */
// Current or running recipe on blender.
EIPS_VS_RECIPE_DATA_STRUCT eips_vs_CurrentRecipeDataObj;


EIPS_VS_PRODUCTION_DATA_STRUCT	eips_vs_ProductionDataObj;

EIPS_VS_CONTROL_DATA_STRUCT		eips_vs_ControlDataObj;

//Batch Related
extern structSetpointData   g_CurrentRecipe;
extern uint16 EIPOneHzProgramCounter;

#ifdef EIPS_PCCC_USED /* OPTIONAL */
    #define PCCC_BIT_SIZE       120 /* 16-bits per */
    #define PCCC_INT_SIZE       120
    #define PCCC_FLOAT_SIZE     60
    #define PCCC_STRING_SIZE    10

    uint16 pccc_bits[PCCC_BIT_SIZE];
    uint16 pccc_integers[PCCC_INT_SIZE];
    float  pccc_floats[PCCC_FLOAT_SIZE];
    EIPS_PCCC_STRING_TYPE pccc_strings[PCCC_STRING_SIZE];
#endif

//

WORD g_nEIPToggleStatusCommandWritten = 0x0000;
/* ---------------------------- */
/* LOCAL FUNCTIONS		*/
/* ---------------------------- */
void  local_get_attr (uint16 classid, uint16 instid, uint8 attrid, EIPS_USER_MSGRTR_RSP_FMT *response);
void  local_set_attr (uint16 classid, uint16 instid, uint8 attrid, uint16 data_siz, uint8 *data_ptr, EIPS_USER_MSGRTR_RSP_FMT *response);
uint8 local_dlc_valid (uint16 needed_siz, uint16 actual_siz, EIPS_USER_MSGRTR_RSP_FMT *response);
void InitEIPSDownloadRecipeDataObj(void );
void InitEIPSCurrentRecipeDataObj (void );
void InitEIPSProductionDataObj(void );
void InitEIPSControlDataObj(void );
// Batch.


/* ---------------------------- */
/*      MISCELLANEOUS           */
/* ---------------------------- */

/**/
/* ******************************************************************** */
/*                      GLOBAL FUNCTIONS                                */
/* ******************************************************************** */
/* ====================================================================
Function:   eips_userobj_init
Parameters: init type
Returns:    N/A

This function initialize all user object model variables.
======================================================================= */
void eips_userobj_init (uint8 init_type)
{
    uint16 i;

    /* ************************************************* */
    /*		 static variables */
    /* ************************************************* */

		/* EMILY MODIFY START */
		/* This is where you initialize default values (if any) */
		eips_vs_DownloadRecipeDataObj.Class.ClassRev = 1;  // Vendor specific revisions will always begin with 1
		/* EMILY MODIFY END */
		InitEIPSDownloadRecipeDataObj ();		//TestOnly - tsm

		eips_vs_CurrentRecipeDataObj.Class.ClassRev = 1;  // Vendor specific revisions will always begin with 1
		InitEIPSCurrentRecipeDataObj();

		eips_vs_ProductionDataObj.Class.ClassRev = 1;  // Vendor specific revisions will always begin with 1
		InitEIPSProductionDataObj();

		eips_vs_ControlDataObj.Class.ClassRev = 1;  // Vendor specific revisions will always begin with 1
		InitEIPSControlDataObj();

    /* discrete input object */
    eips_vs_discreteInDataObj.Class.ClassRev = 1;
    eips_vs_discreteInDataObj.Inst.DataSize = VS_MAX_DISCRETE_INPUT_UNITS;
    memset((uint8 *)eips_vs_discreteInDataObj.Inst.DataPtr, 0, sizeof(eips_vs_discreteInDataObj.Inst.DataPtr));

    /* discrete output object */
    eips_vs_discreteOutDataObj.Class.ClassRev = 1;
    eips_vs_discreteOutDataObj.Inst.DataSize = VS_MAX_DISCRETE_INPUT_UNITS;
    memset((uint8 *)eips_vs_discreteOutDataObj.Inst.DataPtr, 0, sizeof(eips_vs_discreteOutDataObj.Inst.DataPtr));

#ifdef EIPS_PCCC_USED /* OPTIONAL */
    memset(pccc_bits,       0, sizeof(pccc_bits));
    memset(pccc_integers,   0, sizeof(pccc_integers));
    memset(pccc_floats,     0, sizeof(pccc_floats));
    memset(pccc_strings,    0, sizeof(pccc_strings));
#endif

    /* ************************************************* */
    /*		 non-volatile variables */
    /* ************************************************* */

    /* different initialization based on based parameters */
    switch (init_type)
    {
        /* Out of Box Initialization */
        case EIPSINIT_OUTOFBOX:
	        break;

        /* Normal Initialization */
        case EIPSINIT_NORMAL:
        default:
            break;
    };

    /* initialized user I/O data structures */

    /* t2o (produce) */
    for(i=0; i<EIPS_USEROBJ_ASM_MAXNUM_T2OINST; i++)
    {
        local_t2o_asm_struct[i].instID = (uint16)(0x64 + i);
        local_t2o_asm_struct[i].size = (uint16)(i+1);
        memset(local_t2o_asm_struct[i].data_ptr, 0, sizeof(local_t2o_asm_struct[i].data_ptr));
    }
//local_t2o_asm_struct[0].size = 1;
//Emily27012010
local_t2o_asm_struct[0].size = EIPS_USEROBJ_ASM_SIZE_T2O1; // old value 1;

#if EIPS_USEROBJ_ASM_MAXNUM_O2TINST > 0
    /* o2t (consume) */
    for(i=0; i<EIPS_USEROBJ_ASM_MAXNUM_O2TINST; i++)
    {
        local_o2t_asm_struct[i].instID = (uint16)(0x70 + i);
        local_o2t_asm_struct[i].size = (uint16)(i+1);
        memset(local_o2t_asm_struct[i].data_ptr, 0, sizeof(local_o2t_asm_struct[i].data_ptr));
    }
//local_o2t_asm_struct[0].size = 1;
//Emily27012010
    local_o2t_asm_struct[0].size = EIPS_USEROBJ_ASM_SIZE_O2T1; // old value 1;

#endif
}

void TestAsmData( void)
{
	//testonly
	local_t2o_asm_struct[0].data_ptr[0] = EIPOneHzProgramCounter;
}

/* ====================================================================
Function:   eips_userobj_process
Parameters: N/A
Returns:    N/A

This function is called every loop to handle user defined processing.
======================================================================= */
void eips_userobj_process (void)
{
    /* check if the outputs are valid (based on a I/O connection in RUN mode) */
    if(eips_iomsg_outputs_valid(local_o2t_asm_struct[0].instID) == FALSE)
    {
        /* set the outputs to the safe state */
    }
}

/* ====================================================================
Function:   eips_userobj_read_B_reg_pccc
Parameters: File Number
            File Offset
            Length (in words)
            Data Pointer (store the register data here)
Returns:    (see below)

EXTSTS_NONE	           - No additional error
EXTSTS_FIELDILLVAL     - A field has an illegal value
EXTSTS_SYMBOLNOTFOUND  - Symbol not found
EXTSTS_SYMBOLBADFORMAT - Symbol is of improper format
EXTSTS_BADADDRESS      - Address doesn't point to something useful
EXTSTS_DATAFILETOOLRG  - Data or file is too large
EXTSTS_TRANSSIZETOOLRG - Transaction size plus word address is too large
EXTSTS_REGISRO	       - Access denied, improper privilege (WR to RO Reg)

This function is called when a PCCC read of B (Bit) Registers
is received. Bits are accessed in 16-bit blocks.

We don't always know what the data type is. If the file number is
invalid, return EXTSTS_BADADDRESS. You may see a call to all reads
for the same address unless EXTSTS_NONE or an error code other than
EXTSTS_BADADDRESS is returned for a previous read.

SAMPLE CODE: B3:0 - B3:(PCCC_BIT_SIZE-1)
======================================================================= */
#ifdef EIPS_PCCC_USED /* OPTIONAL */
int8 eips_userobj_read_B_reg_pccc (uint16 file_num, uint16 file_offset, uint16 length, uint16 *reg_data)
{
    /* validate the file number */
    if(file_num != 3)
        return(EXTSTS_BADADDRESS);

    /* validate the request is in range */
    if((file_offset + length) > PCCC_BIT_SIZE)
        return(EXTSTS_DATAFILETOOLRG);

    /* successful */
    memcpy((uint8 *)reg_data, (uint8 *)&pccc_bits[file_offset], (length*(sizeof(uint16))));

    eips_user_dbprint3("eips_userobj_read_B_reg_pccc B%d:%d, Len %d\r\n", file_num, file_offset, length);
    return(EXTSTS_NONE);
}
#endif

/* ====================================================================
Function:   eips_userobj_read_N_reg_pccc
Parameters: File Number
            File Offset
            Length (in words)
            Data Pointer (store the register data here)
Returns:    (see below)

EXTSTS_NONE	           - No additional error
EXTSTS_FIELDILLVAL     - A field has an illegal value
EXTSTS_SYMBOLNOTFOUND  - Symbol not found
EXTSTS_SYMBOLBADFORMAT - Symbol is of improper format
EXTSTS_BADADDRESS      - Address doesn't point to something useful
EXTSTS_DATAFILETOOLRG  - Data or file is too large
EXTSTS_TRANSSIZETOOLRG - Transaction size plus word address is too large
EXTSTS_REGISRO	       - Access denied, improper privilege (WR to RO Reg)

This function is called when a PCCC read of N (Integer) Registers
is received.

We don't always know what the data type is. If the file number is
invalid, return EXTSTS_BADADDRESS. You may see a call to all reads
for the same address unless EXTSTS_NONE or an error code other than
EXTSTS_BADADDRESS is returned for a previous read.

SAMPLE CODE: N7:0 - N7:(PCCC_INT_SIZE-1)
======================================================================= */
#ifdef EIPS_PCCC_USED /* OPTIONAL */
int8 eips_userobj_read_N_reg_pccc (uint16 file_num, uint16 file_offset, uint16 length, uint16 *reg_data)
{
    /* validate the file number */
    if(file_num != 7)
        return(EXTSTS_BADADDRESS);

    /* validate the request is in range */
    if((file_offset + length) > PCCC_INT_SIZE)
        return(EXTSTS_DATAFILETOOLRG);

    /* successful */
    memcpy((uint8 *)reg_data, (uint8 *)&pccc_integers[file_offset], (length*(sizeof(uint16))));

    eips_user_dbprint3("eips_userobj_read_N_reg_pccc N%d:%d, Len %d\r\n", file_num, file_offset, length);
    return(EXTSTS_NONE);
}
#endif

/* ====================================================================
Function:   eips_userobj_read_F_reg_pccc
Parameters: File Number
            File Offset
            Length (in words)
            Data Pointer (store the register data here)
Returns:    (see below)

EXTSTS_NONE	           - No additional error
EXTSTS_FIELDILLVAL     - A field has an illegal value
EXTSTS_SYMBOLNOTFOUND  - Symbol not found
EXTSTS_SYMBOLBADFORMAT - Symbol is of improper format
EXTSTS_BADADDRESS      - Address doesn't point to something useful
EXTSTS_DATAFILETOOLRG  - Data or file is too large
EXTSTS_TRANSSIZETOOLRG - Transaction size plus word address is too large
EXTSTS_REGISRO	       - Access denied, improper privilege (WR to RO Reg)

This function is called when a PCCC read of F (Float) Registers
is received.

We don't always know what the data type is. If the file number is
invalid, return EXTSTS_BADADDRESS. You may see a call to all reads
for the same address unless EXTSTS_NONE or an error code other than
EXTSTS_BADADDRESS is returned for a previous read.

SAMPLE CODE: F8:0 - F8:(PCCC_FLOAT_SIZE-1)
======================================================================= */
#ifdef EIPS_PCCC_USED /* OPTIONAL */
int8 eips_userobj_read_F_reg_pccc (uint16 file_num, uint16 file_offset, uint16 length, float *reg_data)
{
    /* validate the file number */
    if(file_num != 8)
        return(EXTSTS_BADADDRESS);

    /* validate the request is in range */
    if((file_offset + length) > PCCC_FLOAT_SIZE)
        return(EXTSTS_DATAFILETOOLRG);

    /* successful */
    memcpy((uint8 *)reg_data, (uint8 *)&pccc_floats[file_offset], (length*(sizeof(float))));

    eips_user_dbprint3("eips_userobj_read_F_reg_pccc F%d:%d, Len %d\r\n", file_num, file_offset, length);
    return(EXTSTS_NONE);
}
#endif

/* ====================================================================
Function:   eips_userobj_read_ST_reg_pccc
Parameters: File Number
            File Offset
            Length (in words)
            Data Pointer (store the register data here)
Returns:    (see below)

EXTSTS_NONE	           - No additional error
EXTSTS_FIELDILLVAL     - A field has an illegal value
EXTSTS_SYMBOLNOTFOUND  - Symbol not found
EXTSTS_SYMBOLBADFORMAT - Symbol is of improper format
EXTSTS_BADADDRESS      - Address doesn't point to something useful
EXTSTS_DATAFILETOOLRG  - Data or file is too large
EXTSTS_TRANSSIZETOOLRG - Transaction size plus word address is too large
EXTSTS_REGISRO	       - Access denied, improper privilege (WR to RO Reg)

This function is called when a PCCC read of ST (String) Registers
is received.

Stings have a unique structure:

#define EIPS_PCCC_MAX_STRING_LEN    82
typedef struct
{
    uint32  string_size;
    uint8   string_data[EIPS_PCCC_MAX_STRING_LEN];
}EIPS_PCCC_STRING_TYPE;

We don't always know what the data type is. If the file number is
invalid, return EXTSTS_BADADDRESS. You may see a call to all reads
for the same address unless EXTSTS_NONE or an error code other than
EXTSTS_BADADDRESS is returned for a previous read.

SAMPLE CODE: ST10:0 - ST10:(PCCC_STRING_SIZE-1)
======================================================================= */
#ifdef EIPS_PCCC_USED /* OPTIONAL */
int8 eips_userobj_read_ST_reg_pccc (uint16 file_num, uint16 file_offset, uint16 length, EIPS_PCCC_STRING_TYPE *reg_data)
{
    /* validate the file number */
    if(file_num != 10)
        return(EXTSTS_BADADDRESS);

    /* validate the request is in range */
    if((file_offset + length) > PCCC_STRING_SIZE)
        return(EXTSTS_DATAFILETOOLRG);

    /* successful */
    memcpy((uint8 *)reg_data, (uint8 *)&pccc_strings[file_offset], (length*(sizeof(EIPS_PCCC_STRING_TYPE))));

    eips_user_dbprint3("eips_userobj_read_ST_reg_pccc ST%d:%d, Len %d\r\n", file_num, file_offset, length);
    return(EXTSTS_NONE);
}
#endif

/* ====================================================================
Function:   eips_userobj_write_B_reg_pccc
Parameters: File Number
            File Offset
            Length (in words)
            Data Pointer (new register data is stored here)
Returns:    (see below)

EXTSTS_NONE	           - No additional error
EXTSTS_FIELDILLVAL     - A field has an illegal value
EXTSTS_SYMBOLNOTFOUND  - Symbol not found
EXTSTS_SYMBOLBADFORMAT - Symbol is of improper format
EXTSTS_BADADDRESS      - Address doesn't point to something useful
EXTSTS_DATAFILETOOLRG  - Data or file is too large
EXTSTS_TRANSSIZETOOLRG - Transaction size plus word address is too large
EXTSTS_REGISRO	       - Access denied, improper privilege (WR to RO Reg)

This function is called when a PCCC write of B (Bit) Registers
is received.

We don't always know what the data type is. If the file number is
invalid, return EXTSTS_BADADDRESS. You may see a call to all writes
for the same address unless EXTSTS_NONE or an error code other than
EXTSTS_BADADDRESS is returned for a previous write.

SAMPLE CODE: B3:0 - B3:(PCCC_BIT_SIZE-1)
======================================================================= */
#ifdef EIPS_PCCC_USED /* OPTIONAL */
int8 eips_userobj_write_B_reg_pccc (uint16 file_num, uint16 file_offset, uint16 length, uint16 *reg_data)
{
    /* validate the file number */
    if(file_num != 3)
        return(EXTSTS_BADADDRESS);

    /* validate the request is in range */
    if((file_offset + length) > PCCC_BIT_SIZE)
        return(EXTSTS_DATAFILETOOLRG);

    /* successful */
    memcpy((uint8 *)&pccc_bits[file_offset], (uint8 *)reg_data, (length*(sizeof(uint16))));

/* dummy increment to see data change on the loop back!! */
/*pccc_bits[file_offset]++; */

    eips_user_dbprint3("eips_userobj_write_B_reg_pccc B%d:%d, Len %d\r\n", file_num, file_offset, length);
    return(EXTSTS_NONE);
}
#endif

/* ====================================================================
Function:   eips_userobj_write_N_reg_pccc
Parameters: File Number
            File Offset
            Length (in words)
            Data Pointer (new register data is stored here)
Returns:    (see below)

EXTSTS_NONE	           - No additional error
EXTSTS_FIELDILLVAL     - A field has an illegal value
EXTSTS_SYMBOLNOTFOUND  - Symbol not found
EXTSTS_SYMBOLBADFORMAT - Symbol is of improper format
EXTSTS_BADADDRESS      - Address doesn't point to something useful
EXTSTS_DATAFILETOOLRG  - Data or file is too large
EXTSTS_TRANSSIZETOOLRG - Transaction size plus word address is too large
EXTSTS_REGISRO	       - Access denied, improper privilege (WR to RO Reg)

This function is called when a PCCC write of N (Integer) Registers
is received.

We don't always know what the data type is. If the file number is
invalid, return EXTSTS_BADADDRESS. You may see a call to all writes
for the same address unless EXTSTS_NONE or an error code other than
EXTSTS_BADADDRESS is returned for a previous write.

SAMPLE CODE: N7:0 - N7:(PCCC_INT_SIZE-1)
======================================================================= */
#ifdef EIPS_PCCC_USED /* OPTIONAL */
int8 eips_userobj_write_N_reg_pccc (uint16 file_num, uint16 file_offset, uint16 length, uint16 *reg_data)
{
    /* validate the file number */
    if(file_num != 7)
        return(EXTSTS_BADADDRESS);

    /* validate the request is in range */
    if((file_offset + length) > PCCC_INT_SIZE)
        return(EXTSTS_DATAFILETOOLRG);

    /* successful */
    memcpy((uint8 *)&pccc_integers[file_offset], (uint8 *)reg_data, (length*(sizeof(uint16))));

/* dummy increment to see data change on the loop back!! */
/*pccc_integers[file_offset]++; */

    eips_user_dbprint3("eips_userobj_write_N_reg_pccc N%d:%d, Len %d\r\n", file_num, file_offset, length);
    return(EXTSTS_NONE);
}
#endif

/* ====================================================================
Function:   eips_userobj_write_F_reg_pccc
Parameters: File Number
            File Offset
            Length (in words)
            Data Pointer (new register data is stored here)
Returns:    (see below)

EXTSTS_NONE	           - No additional error
EXTSTS_FIELDILLVAL     - A field has an illegal value
EXTSTS_SYMBOLNOTFOUND  - Symbol not found
EXTSTS_SYMBOLBADFORMAT - Symbol is of improper format
EXTSTS_BADADDRESS      - Address doesn't point to something useful
EXTSTS_DATAFILETOOLRG  - Data or file is too large
EXTSTS_TRANSSIZETOOLRG - Transaction size plus word address is too large
EXTSTS_REGISRO	       - Access denied, improper privilege (WR to RO Reg)

This function is called when a PCCC write of F (Float) Registers
is received.

We don't always know what the data type is. If the file number is
invalid, return EXTSTS_BADADDRESS. You may see a call to all writes
for the same address unless EXTSTS_NONE or an error code other than
EXTSTS_BADADDRESS is returned for a previous write.

SAMPLE CODE: F8:0 - F8:(PCCC_FLOAT_SIZE-1)
======================================================================= */
#ifdef EIPS_PCCC_USED /* OPTIONAL */
int8 eips_userobj_write_F_reg_pccc (uint16 file_num, uint16 file_offset, uint16 length, float *reg_data)
{
    /* validate the file number */
    if(file_num != 8)
        return(EXTSTS_BADADDRESS);

    /* validate the request is in range */
    if((file_offset + length) > PCCC_FLOAT_SIZE)
        return(EXTSTS_DATAFILETOOLRG);

    /* successful */
    memcpy((uint8 *)&pccc_floats[file_offset], (uint8 *)reg_data, (length*(sizeof(float))));

/* dummy increment to see data change on the loop back!! */
/*pccc_floats[file_offset] = (float)(pccc_floats[file_offset] + 1.23); */

    eips_user_dbprint3("eips_userobj_write_F_reg_pccc F%d:%d, Len %d\r\n", file_num, file_offset, length);
    return(EXTSTS_NONE);
}
#endif

/* ====================================================================
Function:   eips_userobj_write_ST_reg_pccc
Parameters: File Number
            File Offset
            Length (in words)
            Data Pointer (new register data is stored here)
Returns:    (see below)

EXTSTS_NONE	           - No additional error
EXTSTS_FIELDILLVAL     - A field has an illegal value
EXTSTS_SYMBOLNOTFOUND  - Symbol not found
EXTSTS_SYMBOLBADFORMAT - Symbol is of improper format
EXTSTS_BADADDRESS      - Address doesn't point to something useful
EXTSTS_DATAFILETOOLRG  - Data or file is too large
EXTSTS_TRANSSIZETOOLRG - Transaction size plus word address is too large
EXTSTS_REGISRO	       - Access denied, improper privilege (WR to RO Reg)

This function is called when a PCCC write of F (Float) Registers
is received.

We don't always know what the data type is. If the file number is
invalid, return EXTSTS_BADADDRESS. You may see a call to all writes
for the same address unless EXTSTS_NONE or an error code other than
EXTSTS_BADADDRESS is returned for a previous write.

SAMPLE CODE: ST10:0 - ST10:(PCCC_STRING_SIZE-1)
======================================================================= */
#ifdef EIPS_PCCC_USED /* OPTIONAL */
int8 eips_userobj_write_ST_reg_pccc (uint16 file_num, uint16 file_offset, uint16 length, EIPS_PCCC_STRING_TYPE *reg_data)
{
    /* validate the file number */
    if(file_num != 10)
        return(EXTSTS_BADADDRESS);

    /* validate the request is in range */
    if((file_offset + length) > PCCC_FLOAT_SIZE)
        return(EXTSTS_DATAFILETOOLRG);

    /* successful */
    memcpy((uint8 *)&pccc_strings[file_offset], (uint8 *)reg_data, (length*(sizeof(EIPS_PCCC_STRING_TYPE))));

    eips_user_dbprint3("eips_userobj_write_ST_reg_pccc ST%d:%d, Len %d\r\n", file_num, file_offset, length);
    return(EXTSTS_NONE);
}
#endif

/* ====================================================================
Function:   eips_userobj_cfg_asm_process
Parameters: configuration assembly instance
            pointer to configuration data
            configuration data size (in bytes)
            pointer to the general error code (0 on success (default))
            pointer to the extended error code (0 on success (default))
Returns:    SUCCESS / FAILURE

This function validates the configuration instance id and processes the
configuration data.

If the instance id or size is incorrect
    *err_rsp_code = ERR_CNXN_FAILURE;
    *add_err_rsp_code = CMERR_BAD_SEGMENT;
======================================================================= */
uint8 eips_userobj_cfg_asm_process(uint16 cfg_inst_id, uint8 * config_data, uint16 config_data_size_in_bytes, uint8 *err_rsp_code, uint16 *add_err_rsp_code)
{
    RTA_UNUSED_PARAM(cfg_inst_id);
    RTA_UNUSED_PARAM(config_data);
    RTA_UNUSED_PARAM(config_data_size_in_bytes);

/*    eips_user_dbprint2("eips_userobj_cfg_asm_process - inst %d, %d bytes\r\n",cfg_inst_id, config_data_size_in_bytes); */

    /* make sure all the passed pointers are valid */
    if( (err_rsp_code == NULL) ||
        (add_err_rsp_code == NULL) ||
        ((config_data == NULL) && (config_data_size_in_bytes != 0)))
    {
        /* invalid pointers  */
        return(FAILURE);
    }

    /* match the id value (if used, else don't care); allow a size of n (valid) or 0 (NULL) */
/*    if(cfg_inst_id == 128 && (config_data_size_in_bytes == 10 || config_data_size_in_bytes == 0)) */

    /* we don't care about the id, just that the size is 0 */
    if(config_data_size_in_bytes == 0)
        return(SUCCESS);

    /* error with size or instance ID */
    *err_rsp_code = ERR_CNXN_FAILURE;
    *add_err_rsp_code = CMERR_BAD_SEGMENT;
    return(FAILURE);
}

/* Identity Object */
/* ====================================================================
Function:   eips_userobj_getSerialNumber
Parameters: N/A
Returns:    32-bit unique serial number

This function returns a unique 32-bit serial number.  The least
significant 4 bytes of the MAC ID can be used since they must be
unique. Every device from a vendor must have a unique serial number.
======================================================================= */
uint32 eips_userobj_getSerialNumber (void)
{
    uint8 temp_buf[6];

    /* get the MAC ID from the user */
    eips_usersock_getEthLinkObj_Attr03_MacAddr(temp_buf);

    /* return the last 4 bytes as a uint32 */
    return(rta_GetBigEndian32(&temp_buf[2]));
}

/* Assembly Object */
/* ====================================================================
Function:   eips_userobj_asmGetO2TInstList
Parameters: pointer to store the array of supported instance
	        max number of instances allowed (EIPS_USEROBJ_ASM_MAXNUM_O2TINST)
Returns:    actual number of instances used

This function returns the list of O2T (Consume) Assembly Instances.
Each instance number must be unique (between all the O2T instances,
all the T2O instances, EIPS_USEROBJ_ASM_INPUTONLY_HB_INST and
EIPS_USEROBJ_ASM_LISTERONLY_HB_INST) and must be in the range of 100 -
199 (0x64 - 0xc7). This function is only called once at start up so
make sure you reset the code if changes are made to the assembly list.
======================================================================= */
uint16 eips_userobj_asmGetO2TInstList (uint16 * list_ptr, uint16 max_num_inst)
{
#if EIPS_USEROBJ_ASM_MAXNUM_O2TINST > 0
    uint16 i;
#endif

    /* validate the list pointer */
    if(list_ptr == NULL)
        return(0);

#if EIPS_USEROBJ_ASM_MAXNUM_O2TINST > 0
    /* store the instances */
    for(i=0; i<EIPS_USEROBJ_ASM_MAXNUM_O2TINST && i<max_num_inst; i++)
    {
        list_ptr[i] = local_o2t_asm_struct[i].instID;
    }
#endif

    return(EIPS_USEROBJ_ASM_MAXNUM_O2TINST);
}

/* ====================================================================
Function:   eips_userobj_asmGetT2OInstList
Parameters: pointer to store the array of supported instance
	        max number of instances allowed (EIPS_USEROBJ_ASM_MAXNUM_T2OINST)
Returns:    actual number of instances used

This function returns the list of T2O (Produce) Assembly Instances.
Each instance number must be unique (between all the O2T instances,
all the T2O instances, EIPS_USEROBJ_ASM_INPUTONLY_HB_INST and
EIPS_USEROBJ_ASM_LISTERONLY_HB_INST) and must be in the range of 100 -
199 (0x64 - 0xc7). This function is only called once at start up so
make sure you reset the code if changes are made to the assembly list.
======================================================================= */
uint16  eips_userobj_asmGetT2OInstList (uint16 * list_ptr, uint16 max_num_inst)
{
    uint16 i;

    /* validate the list pointer */
    if(list_ptr == NULL)
        return(0);

    /* store the instances */
    for(i=0; i<EIPS_USEROBJ_ASM_MAXNUM_T2OINST && i<max_num_inst; i++)
    {
        list_ptr[i] = local_t2o_asm_struct[i].instID;
    }

    return(EIPS_USEROBJ_ASM_MAXNUM_T2OINST);
}

/* ====================================================================
Function:   eips_userobj_getAsmPtr
Parameters: assembly instance id
	        pointer to expected size in words
Returns:    pointer (NULL on error)

This function validate the Assembly instance and size.	If they both
match an expected Assembly instance, return the pointer to the
Assembly data.  If the size is incorrect, modify the size data pointer.
If the instance doesn't match, return NULL.
======================================================================= */
#if EIPS_USEROBJ_ASM_ELEMENT_SIZE == 4      /* DWORDS */
    uint32 * eips_userobj_getAsmPtr (uint16 inst_id, uint16 *size)
#elif EIPS_USEROBJ_ASM_ELEMENT_SIZE == 2    /* WORDS */
    uint16 * eips_userobj_getAsmPtr (uint16 inst_id, uint16 *size)
#else                                       /* BYTES */
    uint8 * eips_userobj_getAsmPtr (uint16 inst_id, uint16 *size)
#endif
{
    uint16 i;

    /* search through t2o instances */
    for(i=0; i<EIPS_USEROBJ_ASM_MAXNUM_T2OINST; i++)
    {
        if(local_t2o_asm_struct[i].instID == inst_id)
        {
            *size = local_t2o_asm_struct[i].size;
            return(local_t2o_asm_struct[i].data_ptr);
        }
    }

#if EIPS_USEROBJ_ASM_MAXNUM_O2TINST > 0
    /* search through o2t instances */
    for(i=0; i<EIPS_USEROBJ_ASM_MAXNUM_O2TINST; i++)
    {
        if(local_o2t_asm_struct[i].instID == inst_id)
        {
            *size = local_o2t_asm_struct[i].size;
            return(local_o2t_asm_struct[i].data_ptr);
        }
    }
#endif

    /* There is an error, set the passed pointer to NULL */
    *size = 0;
    return(NULL);
}

/* ====================================================================
Function:   eips_userobj_asmDataWrote
Parameters: O_to_T assembly instance
	        Number of bytes written
Returns:    N/A

This function is called when an Assembly is written.
======================================================================= */
void eips_userobj_asmDataWrote (uint16 inst_id, uint16 bytes_written)
{
    RTA_UNUSED_PARAM(inst_id);
    RTA_UNUSED_PARAM(bytes_written);
//Emily27012010
    /* EMILY MODIFY START */
    #if 0
        /* simple echo test */
        local_t2o_asm_struct[0].data_ptr[0] = local_o2t_asm_struct[0].data_ptr[0];
    	eips_iomsg_t2o_cos(local_t2o_asm_struct[0].instID);
    #endif

    	//if instance is an output assembly update
    	if(inst_id == local_o2t_asm_struct[0].instID)
    	{
    		// copy bytes 0 & 1 to this attribute
//todo    		eips_vs_CurrentRecipeDataObj.Inst.m_nRegrindComponent = rta_GetLitEndian16(&local_o2t_asm_struct[0].data_ptr[0]);
    		// copy bytes 2 & 3 to this attribute
//todo    		eips_vs_ControlDataObj.Inst.m_nSetIncrease = rta_GetLitEndian16(&local_o2t_asm_struct[0].data_ptr[2]);
    	}

    	// see if we have a new message to send
//mmk        local_update_produce_asm();
    /* EMILY MODIFY END */

#ifdef EIPS_PCCC_USED
   /* copy the output data to the PCCC registers */
#endif
}

/* ====================================================================
Function:   eips_userobj_procVendSpecObject
Parameters: Pointer to Message Router Request structure
            Pointer to Message Router Response structure
Returns:    SUCCESS - Use response in "response" structure
            FAILURE - Use default error "Object does not exist"

This function passes the Message Router Request and Response structures.
If the user supports any vendor specific objects, parse the message,
validate the passed request and build a response message in the passed
response strucute.  If the passed object isn't supported, return
FAILURE and don't fill in the passed response structure.
======================================================================= */
uint8 eips_userobj_procVendSpecObject (EIPS_USER_MSGRTR_REQ_FMT *request, EIPS_USER_MSGRTR_RSP_FMT *response)
{
    uint16  class_id, inst_id;
    uint8   attr_id;
    uint8   class_found, inst_found, attr_found;
    uint8   path_seg, path_val;
    int16   i;

    /* set the response to success */
    response->gen_stat = ERR_SUCCESS;
    response->ext_stat_size = 0;

    /* initialize variables */
    class_id = 0;
    inst_id = 0;
    attr_id = 0;
    class_found = FALSE;
    inst_found  = FALSE;
    attr_found  = FALSE;

    /* parse the path */
    for(i=0; i<request->path_size; i++)
    {
        path_seg = EIPS_LO(request->path[i]);
        path_val = EIPS_HI(request->path[i]);

        /* The only valid order is Class, Inst, Attr */
        switch (path_seg)
        {
            /* 8 Bit Class */
            case LOGSEG_8BITCLASS:
                /* This must be the first path */
	            if( (class_found == FALSE) &&
		            ( inst_found == FALSE) &&
		            ( attr_found == FALSE) )
	            {
	                class_found = TRUE;
	                class_id = path_val;
	            }

	            /* Bad Path order */
	            else
	            {
                    return(FAILURE); /* let the RTA app set the error */
	            }
	            break;

            /* 16 Bit Class */
            case LOGSEG_16BITCLASS:
                /* This must be the first path */
	            if( (class_found == FALSE) &&
		            ( inst_found == FALSE) &&
		            ( attr_found == FALSE) )
	            {
	                class_found = TRUE;
                    i++; /* the class id is stored in the next word) */
	                class_id = (uint16)(request->path[i]);
	            }

	            /* Bad Path order */
	            else
	            {
                    return(FAILURE); /* let the RTA app set the error */
	            }
	            break;

	        /* 8 Bit Instance */
	        case LOGSEG_8BITINST:
	            /* This must be the second path */
	            if( (class_found == TRUE)  &&
		            ( inst_found == FALSE) &&
		            ( attr_found == FALSE) )
	            {
                    inst_found = TRUE;
                    inst_id = path_val;
	            }

	            /* Bad Path order */
	            else
	            {
                    return(FAILURE); /* let the RTA app set the error */
	            }
                break;

	        /* 16 Bit Instance */
	        case LOGSEG_16BITINST:
	            /* This must be the second path */
	            if( (class_found == TRUE)  &&
		            ( inst_found == FALSE) &&
		            ( attr_found == FALSE) )
	            {
                    inst_found = TRUE;
                    i++; /* the class id is stored in the next word) */
	                inst_id = (uint16)(request->path[i]);
	            }

	            /* Bad Path order */
	            else
	            {
                    return(FAILURE); /* let the RTA app set the error */
	            }
                break;

	        /* 8 Bit Attribute */
	        case LOGSEG_8BITATTR:
	            /* This must be the third path */
	            if( (class_found == TRUE) &&
		            ( inst_found == TRUE) &&
		            ( attr_found == FALSE) )
	            {
	                attr_found = TRUE;
	                attr_id = path_val;
	            }

	            /* Bad Path order */
	            else
	            {
                    return(FAILURE); /* let the RTA app set the error */
	            }
	            break;

	        /* ERROR with path */
	        default:
                    return(FAILURE); /* let the RTA app set the error */
        }; /* END-> "switch (path_seg)" */
    }

    /* This function code requires a valid class, instance, attribute */
    if((class_found == FALSE) || (inst_found == FALSE))
    {
        return(FAILURE); // let the RTA app set the error
    }

		/* EMILY MODIFY START */
    /* validate the object exists */
    if( (class_id != VS_OBJ_DISCRETE_INPUT_DATA) &&
        (class_id != VS_OBJ_DISCRETE_OUTPUT_DATA) &&
        (class_id != VS_OBJ_DOWNLOAD_RECIPE_DATA)  &&
        (class_id != VS_OBJ_CURRENT_RECIPE_DATA)  &&
        (class_id != VS_OBJ_PRODUCTION_DATA) &&
        (class_id != VS_OBJ_CONTROL_DATA)  )
    {
        return(FAILURE); /* let the RTA app set the error */
    }
		/* EMILY MODIFY END */

    /* at this point we can take over the error code handling.... */

    /* switch on the service code */
    switch(request->service)
    {
        /* Get_Attribute_Single Service Code */
        case CIP_SC_GET_ATTR_SINGLE:
            /* make sure we have the attribute */
            if(attr_found == TRUE)
                local_get_attr (class_id, inst_id, attr_id, response);
            else
                return(FAILURE); /* let the RTA app set the error */
            break;

        /* Set_Attribute_Single Service Code */
        case CIP_SC_SET_ATTR_SINGLE:
            /* make sure we have the attribute */
            if(attr_found == TRUE)
                local_set_attr (class_id, inst_id, attr_id, request->req_data_size, request->req_data, response);
            else
                return(FAILURE); /* let the RTA app set the error */
            break;

        default:
            /* Error: Path Destination Unknown */
            response->gen_stat = ERR_SERV_UNSUPP;
	        return(SUCCESS); /* exit on an error */
    };

    return(SUCCESS);
}

/**/
/* ******************************************************************** */
/*			LOCAL FUNCTIONS 				*/
/* ******************************************************************** */
/* ====================================================================
Function:   local_get_attr
Parameters: 16-bit class id
            16-bit instance id
            8-bit attribute id
            Pointer to Message Router Response structure
Returns:    N/A

This function is called to process the Get Attribute Single request.
======================================================================= */
void local_get_attr (uint16 class_id, uint16 inst_id, uint8 attr_id, EIPS_USER_MSGRTR_RSP_FMT *response)
{
    uint16 i;
    g_nEIPCtr++;
//    BYTE nTemp;
/*    eips_user_dbprint3("VS_GetSingle Class 0x%02x Inst 0x%02x, Attr 0x%02x\r\n", class_id, inst_id, attr_id); */

    /* switch on the class id */
    switch(class_id)
    {

		/* EMILY MODIFY START */
// Download recipe.
        case VS_OBJ_DOWNLOAD_RECIPE_DATA:
            // Validate the Instance ID //
            if(inst_id > 1) // class and 1 instance //
            {
                /* Error: Object Does Not Exist */
                response->gen_stat = ERR_UNEXISTANT_OBJ;
                return;
            }

            /* Class Attributes */
            if(inst_id == 0)
            {
                /* switch on the Attribute ID */
                switch(attr_id)
                {
                    case 1: // Revision (uint16)
                        rta_PutLitEndian16(eips_vs_DownloadRecipeDataObj.Class.ClassRev, response->rsp_data);
                        response->rsp_data_size = 2;
                        break;

                    /* Attribute Unsupported */
                    default:
                        /* Error: Attribute Not Supported */
                        response->gen_stat = ERR_ATTR_UNSUPP;
                        break;
                }
            }

            /* Instance Attributes */
            else
            {
                /* switch on the Attribute ID */
                switch(attr_id)
                {
					case 1: /* m_fPercentage1 */
            		 	rta_PutLitEndianFloat(eips_vs_DownloadRecipeDataObj.Inst.m_fPercentage1, response->rsp_data);
            		 	response->rsp_data_size = 4;
            		 	break;
					case 2: /* m_fPercentage2 */
            		 	rta_PutLitEndianFloat(eips_vs_DownloadRecipeDataObj.Inst.m_fPercentage2, response->rsp_data);
            		 	response->rsp_data_size = 4;
            		 	break;


	                	case 3: /* m_fPercentage3 */
	                		 	rta_PutLitEndianFloat(eips_vs_DownloadRecipeDataObj.Inst.m_fPercentage3, response->rsp_data);
	                		 	response->rsp_data_size = 4;
	                    	break;
	                	case 4: /* m_fPercentage4 */
	                		 	rta_PutLitEndianFloat(eips_vs_DownloadRecipeDataObj.Inst.m_fPercentage4, response->rsp_data);
	                		 	response->rsp_data_size = 4;
	                    	break;

	                	case 5: /* m_fPercentage5 */
	                		 	rta_PutLitEndianFloat(eips_vs_DownloadRecipeDataObj.Inst.m_fPercentage5, response->rsp_data);
	                		 	response->rsp_data_size = 4;
	                    	break;

	                	case 6: /* m_fPercentage6 */
	                		 	rta_PutLitEndianFloat(eips_vs_DownloadRecipeDataObj.Inst.m_fPercentage6, response->rsp_data);
	                		 	response->rsp_data_size = 4;
	                    	break;

	                	case 7: /* m_fPercentage7 */
	                		 	rta_PutLitEndianFloat(eips_vs_DownloadRecipeDataObj.Inst.m_fPercentage7, response->rsp_data);
	                		 	response->rsp_data_size = 4;
	                    	break;

	                	case 8: /* m_fPercentage8 */
	                		 	rta_PutLitEndianFloat(eips_vs_DownloadRecipeDataObj.Inst.m_fPercentage8, response->rsp_data);
	                		 	response->rsp_data_size = 4;
	                    	break;
	                	case 9: /* m_fPercentage9 */
	                		 	rta_PutLitEndianFloat(eips_vs_DownloadRecipeDataObj.Inst.m_fPercentage9, response->rsp_data);
	                		 	response->rsp_data_size = 4;
	                    	break;
	                	case 10: /* m_fPercentage10 */
	                		 	rta_PutLitEndianFloat(eips_vs_DownloadRecipeDataObj.Inst.m_fPercentage10, response->rsp_data);
	                		 	response->rsp_data_size = 4;
	                    	break;
	                	case 11: /* m_fPercentage11 */
	                		 	rta_PutLitEndianFloat(eips_vs_DownloadRecipeDataObj.Inst.m_fPercentage11, response->rsp_data);
	                		 	response->rsp_data_size = 4;
	                    	break;
	                	case 12: /* m_fPercentage12 */
	                		 	rta_PutLitEndianFloat(eips_vs_DownloadRecipeDataObj.Inst.m_fPercentage12, response->rsp_data);
	                		 	response->rsp_data_size = 4;
	                    	break;
	                	case 13:		//RegrindComponent;		//ATTR13
	                        rta_PutLitEndian16(eips_vs_DownloadRecipeDataObj.Inst.m_nRegrindComponent, response->rsp_data);
	                        response->rsp_data_size = 2;
	                        break;
	                	case 14:	//TotalThroughput;		//ATTR14
                		 	rta_PutLitEndianFloat(eips_vs_DownloadRecipeDataObj.Inst.m_fTotalThroughput, response->rsp_data);
                		 	response->rsp_data_size = 4;
	                    	break;
	                	case 15:	//TotalWeightLength;	//ATTR15
                		 	rta_PutLitEndianFloat(eips_vs_DownloadRecipeDataObj.Inst.m_fTotalWeightLength, response->rsp_data);
                		 	response->rsp_data_size = 4;
	                    	break;
	                	case 16:	//TotalWeightArea;		//ATTR16
                		 	rta_PutLitEndianFloat(eips_vs_DownloadRecipeDataObj.Inst.m_fTotalWeightArea, response->rsp_data);
                		 	response->rsp_data_size = 4;
	                    	break;
	                	case 17:	//LineSpeed;			//ATTR17
                		 	rta_PutLitEndianFloat(eips_vs_DownloadRecipeDataObj.Inst.m_fLineSpeed, response->rsp_data);
                		 	response->rsp_data_size = 4;
	                    	break;
	                	case 18:	// DesiredWidth;		//ATTR18
                		 	rta_PutLitEndianFloat(eips_vs_DownloadRecipeDataObj.Inst.m_fDesiredWidth, response->rsp_data);
                		 	response->rsp_data_size = 4;
	                    	break;
	                	case 19:	// SheetTube;	(uint16)		//ATTR19
	                        rta_PutLitEndian16(eips_vs_DownloadRecipeDataObj.Inst.m_nSheetTube, response->rsp_data);
	                        response->rsp_data_size = 2;
	                        break;
/*
					        // copy 1 char of data
	                		nTemp = 0;
	                		if(eips_vs_DownloadRecipeDataObj.Inst.m_nSheetTube)
	                			nTemp = 1;
					        rta_ByteMove(response->rsp_data, &nTemp, 1);
					        // the total length is the 1chars of data ??
                		 	response->rsp_data_size = 1;
	                		 break;
*/

	                    case 20: /* m_nFileNumber (uint16) */
	                        rta_PutLitEndian16(eips_vs_DownloadRecipeDataObj.Inst.m_nFileNumber, response->rsp_data);
	                        response->rsp_data_size = 2;
	                        break;

	                	case 21:	//m_nResetTotals;			//ATTR21
	                        rta_PutLitEndian16(eips_vs_DownloadRecipeDataObj.Inst.m_nResetTotals, response->rsp_data);
	                        response->rsp_data_size = 2;

	                		 break;
	                	case 22:	//m_nLoadRecipeCommand;	//ATTR22
	                        rta_PutLitEndian16(eips_vs_DownloadRecipeDataObj.Inst.m_nLoadRecipeCommand, response->rsp_data);
	                        response->rsp_data_size = 2;
	                		 break;

	                	case 23:	// m_nRecipeSpareAttribute1; //ATTR 23
	                        rta_PutLitEndian16(eips_vs_DownloadRecipeDataObj.Inst.m_nRecipeSpareAttribute1, response->rsp_data);
	                        response->rsp_data_size = 2;
	                		 break;
	                	case 24:	// m_nRecipeSpareAttribute2; //ATTR 24
	                        rta_PutLitEndian16(eips_vs_DownloadRecipeDataObj.Inst.m_nRecipeSpareAttribute2, response->rsp_data);
	                        response->rsp_data_size = 2;
	                		 break;

	                	case 25:	// m_fRecipeSpareAttribute3; //ATTR25
                		 	rta_PutLitEndianFloat(eips_vs_DownloadRecipeDataObj.Inst.m_fRecipeSpareAttribute3, response->rsp_data);
                		 	response->rsp_data_size = 4;
	                		 break;
	                	case 26:	// m_fRecipeSpareAttribute4; //ATTR26
                		 	rta_PutLitEndianFloat(eips_vs_DownloadRecipeDataObj.Inst.m_fRecipeSpareAttribute4, response->rsp_data);
                		 	response->rsp_data_size = 4;
	                		 break;

	                	case 27: /* m_arrDescription */
	                		  //1st byte is equal to the number of characters in the description
								        response->rsp_data[0] = 20;

								        // copy 20 chars of data
								        rta_ByteMove(response->rsp_data+1, (uint8*)eips_vs_DownloadRecipeDataObj.Inst.m_arrDescription, 20);

								        /* the total length is the 20 chars of data + 1 */
								        response->rsp_data_size = 20+1;
	                    	break;
	                	case 28:	// m_nRecipeSpareAttribute5; //ATTR 28
	                        rta_PutLitEndian16(eips_vs_DownloadRecipeDataObj.Inst.m_nRecipeSpareAttribute5, response->rsp_data);
	                        response->rsp_data_size = 2;
	                		 break;
	                	case 29:	// m_nRecipeSpareAttribute6; //ATTR 29
	                        rta_PutLitEndian16(eips_vs_DownloadRecipeDataObj.Inst.m_nRecipeSpareAttribute6, response->rsp_data);
	                        response->rsp_data_size = 2;
	                		 break;
	                	case 30:	// m_nRecipeSpareAttribute7; //ATTR 30
	                        rta_PutLitEndian16(eips_vs_DownloadRecipeDataObj.Inst.m_nRecipeSpareAttribute7, response->rsp_data);
	                        response->rsp_data_size = 2;
	                		 break;

                    /* Attribute Unsupported */
                    default:
                        /* Error: Attribute Not Supported */
                        response->gen_stat = ERR_ATTR_UNSUPP;
                        break;
                }
            }
            break;
//download recipe data object end.

//Current Recipe
//Current recipe.
		case VS_OBJ_CURRENT_RECIPE_DATA:
			/* Validate the Instance ID */
			if(inst_id > 1) /* class and 1 instance */
			{
				/* Error: Object Does Not Exist */
				response->gen_stat = ERR_UNEXISTANT_OBJ;
				return;
			}

			/* Class Attributes */
			if(inst_id == 0)
			{
				/* switch on the Attribute ID */
				switch(attr_id)
				{
					case 1: /* Revision (uint16) */
						rta_PutLitEndian16(eips_vs_CurrentRecipeDataObj.Class.ClassRev, response->rsp_data);
						response->rsp_data_size = 2;
						break;

					/* Attribute Unsupported */
					default:
						/* Error: Attribute Not Supported */
						response->gen_stat = ERR_ATTR_UNSUPP;
						break;
				}
			}

			/* Instance Attributes */
			else
			{
				/* switch on the Attribute ID */
				switch(attr_id)
				{
					case 1: /* m_fPercentage1 */
						rta_PutLitEndianFloat(eips_vs_CurrentRecipeDataObj.Inst.m_fPercentage1, response->rsp_data);
						response->rsp_data_size = 4;
						break;
					case 2: /* m_fPercentage2 */
						rta_PutLitEndianFloat(eips_vs_CurrentRecipeDataObj.Inst.m_fPercentage2, response->rsp_data);
						response->rsp_data_size = 4;
						break;

						case 3: /* m_fPercentage3 */
								rta_PutLitEndianFloat(eips_vs_CurrentRecipeDataObj.Inst.m_fPercentage3, response->rsp_data);
								response->rsp_data_size = 4;
							break;
						case 4: /* m_fPercentage4 */
								rta_PutLitEndianFloat(eips_vs_CurrentRecipeDataObj.Inst.m_fPercentage4, response->rsp_data);
								response->rsp_data_size = 4;
							break;

						case 5: /* m_fPercentage5 */
								rta_PutLitEndianFloat(eips_vs_CurrentRecipeDataObj.Inst.m_fPercentage5, response->rsp_data);
								response->rsp_data_size = 4;
							break;

						case 6: /* m_fPercentage6 */
								rta_PutLitEndianFloat(eips_vs_CurrentRecipeDataObj.Inst.m_fPercentage6, response->rsp_data);
								response->rsp_data_size = 4;
							break;

						case 7: /* m_fPercentage7 */
								rta_PutLitEndianFloat(eips_vs_CurrentRecipeDataObj.Inst.m_fPercentage7, response->rsp_data);
								response->rsp_data_size = 4;
							break;

						case 8: /* m_fPercentage8 */
								rta_PutLitEndianFloat(eips_vs_CurrentRecipeDataObj.Inst.m_fPercentage8, response->rsp_data);
								response->rsp_data_size = 4;
							break;
						case 9: /* m_fPercentage9 */
								rta_PutLitEndianFloat(eips_vs_CurrentRecipeDataObj.Inst.m_fPercentage9, response->rsp_data);
								response->rsp_data_size = 4;
							break;
						case 10: /* m_fPercentage10 */
								rta_PutLitEndianFloat(eips_vs_CurrentRecipeDataObj.Inst.m_fPercentage10, response->rsp_data);
								response->rsp_data_size = 4;
							break;
						case 11: /* m_fPercentage11 */
								rta_PutLitEndianFloat(eips_vs_CurrentRecipeDataObj.Inst.m_fPercentage11, response->rsp_data);
								response->rsp_data_size = 4;
							break;
						case 12: /* m_fPercentage12 */
								rta_PutLitEndianFloat(eips_vs_CurrentRecipeDataObj.Inst.m_fPercentage12, response->rsp_data);
								response->rsp_data_size = 4;
							break;
						case 13:		//RegrindComponent;		//ATTR13
							rta_PutLitEndian16(eips_vs_CurrentRecipeDataObj.Inst.m_nRegrindComponent, response->rsp_data);
							response->rsp_data_size = 2;
							break;
						case 14:	//TotalThroughput;		//ATTR14
							rta_PutLitEndianFloat(eips_vs_CurrentRecipeDataObj.Inst.m_fTotalThroughput, response->rsp_data);
							response->rsp_data_size = 4;
							break;
						case 15:	//TotalWeightLength;	//ATTR15
							rta_PutLitEndianFloat(eips_vs_CurrentRecipeDataObj.Inst.m_fTotalWeightLength, response->rsp_data);
							response->rsp_data_size = 4;
							break;
						case 16:	//TotalWeightArea;		//ATTR16
							rta_PutLitEndianFloat(eips_vs_CurrentRecipeDataObj.Inst.m_fTotalWeightArea, response->rsp_data);
							response->rsp_data_size = 4;
							break;
						case 17:	//LineSpeed;			//ATTR17
							rta_PutLitEndianFloat(eips_vs_CurrentRecipeDataObj.Inst.m_fLineSpeed, response->rsp_data);
							response->rsp_data_size = 4;
							break;
						case 18:	// DesiredWidth;		//ATTR18
							rta_PutLitEndianFloat(eips_vs_CurrentRecipeDataObj.Inst.m_fDesiredWidth, response->rsp_data);
							response->rsp_data_size = 4;
							break;
						case 19:	// SheetTube;	(uint16)		//ATTR19
							rta_PutLitEndian16(eips_vs_CurrentRecipeDataObj.Inst.m_nSheetTube, response->rsp_data);
							response->rsp_data_size = 2;
							break;
/*
							// copy 1 char of data
							nTemp = 0;
							if(eips_vs_CurrentRecipeDataObj.Inst.m_nSheetTube)
								nTemp = 1;
							rta_ByteMove(response->rsp_data, &nTemp, 1);
							// the total length is the 1chars of data ?? //
							response->rsp_data_size = 1;
							 break;
*/

						case 20: /* m_nFileNumber (uint16) */
							rta_PutLitEndian16(eips_vs_CurrentRecipeDataObj.Inst.m_nFileNumber, response->rsp_data);
							response->rsp_data_size = 2;
							break;

						case 21:	//m_nResetTotals;			//ATTR21
							rta_PutLitEndian16(eips_vs_CurrentRecipeDataObj.Inst.m_nResetTotals, response->rsp_data);
							response->rsp_data_size = 2;

							 break;
						case 22:	//m_nLoadRecipeCommand;	//ATTR22
							rta_PutLitEndian16(eips_vs_CurrentRecipeDataObj.Inst.m_nLoadRecipeCommand, response->rsp_data);
							response->rsp_data_size = 2;
							 break;

						case 23:	// m_nRecipeSpareAttribute1; //ATTR 23
							rta_PutLitEndian16(eips_vs_CurrentRecipeDataObj.Inst.m_nRecipeSpareAttribute1, response->rsp_data);
							response->rsp_data_size = 2;
							 break;
						case 24:	// m_nRecipeSpareAttribute2; //ATTR 24
							rta_PutLitEndian16(eips_vs_CurrentRecipeDataObj.Inst.m_nRecipeSpareAttribute2, response->rsp_data);
							response->rsp_data_size = 2;
							 break;

						case 25:	// m_fRecipeSpareAttribute3; //ATTR25
							rta_PutLitEndianFloat(eips_vs_CurrentRecipeDataObj.Inst.m_fRecipeSpareAttribute3, response->rsp_data);
							response->rsp_data_size = 4;
							 break;
						case 26:	// m_fRecipeSpareAttribute4; //ATTR26
							rta_PutLitEndianFloat(eips_vs_CurrentRecipeDataObj.Inst.m_fRecipeSpareAttribute4, response->rsp_data);
							response->rsp_data_size = 4;
							 break;

						case 27: /* m_arrDescription */
							  //1st byte is equal to the number of characters in the description
										response->rsp_data[0] = 20;

										// copy 20 chars of data
										rta_ByteMove(response->rsp_data+1, (uint8*)eips_vs_CurrentRecipeDataObj.Inst.m_arrDescription, 20);

										/* the total length is the 20 chars of data + 1 */
										response->rsp_data_size = 20+1;
							break;
						case 28:	// m_nRecipeSpareAttribute5; //ATTR 28
							rta_PutLitEndian16(eips_vs_CurrentRecipeDataObj.Inst.m_nRecipeSpareAttribute5, response->rsp_data);
							response->rsp_data_size = 2;
							 break;
						case 29:	// m_nRecipeSpareAttribute6; //ATTR 29
							rta_PutLitEndian16(eips_vs_CurrentRecipeDataObj.Inst.m_nRecipeSpareAttribute6, response->rsp_data);
							response->rsp_data_size = 2;
							 break;
						case 30:	// m_nRecipeSpareAttribute7; //ATTR 30
							rta_PutLitEndian16(eips_vs_CurrentRecipeDataObj.Inst.m_nRecipeSpareAttribute7, response->rsp_data);
							response->rsp_data_size = 2;
							 break;

					/* Attribute Unsupported */
					default:
						/* Error: Attribute Not Supported */
						response->gen_stat = ERR_ATTR_UNSUPP;
						break;
				}
			}
			break;
//current recipe data object end.
//Current recipe data object end.


		/* EMILY MODIFY END */
// Production Data.
			case VS_OBJ_PRODUCTION_DATA:
				/* Validate the Instance ID */
				if(inst_id > 1) /* class and 1 instance */
				{
					/* Error: Object Does Not Exist */
					response->gen_stat = ERR_UNEXISTANT_OBJ;
					return;
				}

				/* Class Attributes */
				if(inst_id == 0)
				{
					/* switch on the Attribute ID */
					switch(attr_id)
					{
						case 1: /* Revision (uint16) */
							rta_PutLitEndian16(eips_vs_ProductionDataObj.Class.ClassRev, response->rsp_data);
							response->rsp_data_size = 2;
							break;

						/* Attribute Unsupported */
						default:
							/* Error: Attribute Not Supported */
							response->gen_stat = ERR_ATTR_UNSUPP;
							break;
					}
				}

				/* Instance Attributes */
				else
				{
					/* switch on the Attribute ID */
					switch(attr_id)
					{
						case 1: 	// Actual Throughput
							rta_PutLitEndianFloat(eips_vs_ProductionDataObj.Inst.m_fActualThroughput, response->rsp_data);
							response->rsp_data_size = 4;
							if( fdTelnet>0 && g_nTSMDebug.m_bEIPDebug) printf( "\nReading Prod Summ. Attr. #1");
							break;
						case 2: // Actual Wt/len
							rta_PutLitEndianFloat(eips_vs_ProductionDataObj.Inst.m_fActualWPL, response->rsp_data);
							response->rsp_data_size = 4;
							break;


							case 3: // Actual Wt/Area.
									rta_PutLitEndianFloat(eips_vs_ProductionDataObj.Inst.m_fActualWPA, response->rsp_data);
									response->rsp_data_size = 4;
								break;
							case 4: // Actual Order Wt.
									rta_PutLitEndianFloat(eips_vs_ProductionDataObj.Inst.m_fActualOrderWeight, response->rsp_data);
									response->rsp_data_size = 4;
								break;

							case 5: // Actual Line Speed
									rta_PutLitEndianFloat(eips_vs_ProductionDataObj.Inst.m_fActualLineSpeed, response->rsp_data);
									response->rsp_data_size = 4;
								break;

							case 6: // Operational Status
								rta_PutLitEndian16(eips_vs_ProductionDataObj.Inst.m_nOperationalStatus, response->rsp_data);
									response->rsp_data_size = 2;
								break;

							case 7: // Motor DA
								rta_PutLitEndian16(eips_vs_ProductionDataObj.Inst.m_nMotorDA, response->rsp_data);
									response->rsp_data_size = 2;
								break;

							case 8: //Operational Status 2
								rta_PutLitEndian16(eips_vs_ProductionDataObj.Inst.m_nOperationalStatus2, response->rsp_data);
									response->rsp_data_size = 2;
								break;
							case 9: // Batch Counter.
								rta_PutLitEndian16(eips_vs_ProductionDataObj.Inst.m_nBatchCounter, response->rsp_data);
									response->rsp_data_size = 2;
								break;
							case 10: // Process Alarm Word
								rta_PutLitEndian16(eips_vs_ProductionDataObj.Inst.m_nProcessAlarms, response->rsp_data);
									response->rsp_data_size = 2;
								break;
// Component actual %'s
							case 11: //Actual Percentage 1
									rta_PutLitEndianFloat(eips_vs_ProductionDataObj.Inst.m_fActualPercentage1, response->rsp_data);
									response->rsp_data_size = 4;
								break;
							case 12: //Actual Percentage 2
									rta_PutLitEndianFloat(eips_vs_ProductionDataObj.Inst.m_fActualPercentage2, response->rsp_data);
									response->rsp_data_size = 4;
								break;
							case 13: //Actual Percentage 3
									rta_PutLitEndianFloat(eips_vs_ProductionDataObj.Inst.m_fActualPercentage3, response->rsp_data);
									response->rsp_data_size = 4;
								break;
							case 14: //Actual Percentage 4
									rta_PutLitEndianFloat(eips_vs_ProductionDataObj.Inst.m_fActualPercentage4, response->rsp_data);
									response->rsp_data_size = 4;
								break;
							case 15: //Actual Percentage 5
									rta_PutLitEndianFloat(eips_vs_ProductionDataObj.Inst.m_fActualPercentage5, response->rsp_data);
									response->rsp_data_size = 4;
								break;
							case 16: //Actual Percentage 6
									rta_PutLitEndianFloat(eips_vs_ProductionDataObj.Inst.m_fActualPercentage6, response->rsp_data);
									response->rsp_data_size = 4;
								break;
							case 17: //Actual Percentage 7
									rta_PutLitEndianFloat(eips_vs_ProductionDataObj.Inst.m_fActualPercentage7, response->rsp_data);
									response->rsp_data_size = 4;
								break;
							case 18: //Actual Percentage 8
									rta_PutLitEndianFloat(eips_vs_ProductionDataObj.Inst.m_fActualPercentage8, response->rsp_data);
									response->rsp_data_size = 4;
								break;
							case 19: //Actual Percentage 9
									rta_PutLitEndianFloat(eips_vs_ProductionDataObj.Inst.m_fActualPercentage9, response->rsp_data);
									response->rsp_data_size = 4;
								break;
							case 20: //Actual Percentage 10
									rta_PutLitEndianFloat(eips_vs_ProductionDataObj.Inst.m_fActualPercentage10, response->rsp_data);
									response->rsp_data_size = 4;
								break;
							case 21: //Actual Percentage 11
									rta_PutLitEndianFloat(eips_vs_ProductionDataObj.Inst.m_fActualPercentage11, response->rsp_data);
									response->rsp_data_size = 4;
								break;
							case 22: //Actual Percentage 12
									rta_PutLitEndianFloat(eips_vs_ProductionDataObj.Inst.m_fActualPercentage12, response->rsp_data);
									response->rsp_data_size = 4;
								break;

// Actual wts in batch.
							case 23: //Actual component wt 1
									rta_PutLitEndianFloat(eips_vs_ProductionDataObj.Inst.m_fActualWeight1, response->rsp_data);
									response->rsp_data_size = 4;
								break;
							case 24: //Actual component wt 2
									rta_PutLitEndianFloat(eips_vs_ProductionDataObj.Inst.m_fActualWeight2, response->rsp_data);
									response->rsp_data_size = 4;
								break;
							case 25: //Actual component wt 3
									rta_PutLitEndianFloat(eips_vs_ProductionDataObj.Inst.m_fActualWeight3, response->rsp_data);
									response->rsp_data_size = 4;
								break;
							case 26: //Actual component wt 4
									rta_PutLitEndianFloat(eips_vs_ProductionDataObj.Inst.m_fActualWeight4, response->rsp_data);
									response->rsp_data_size = 4;
								break;
							case 27: //Actual component wt 5
									rta_PutLitEndianFloat(eips_vs_ProductionDataObj.Inst.m_fActualWeight5, response->rsp_data);
									response->rsp_data_size = 4;
								break;
							case 28: //Actual component wt 6
									rta_PutLitEndianFloat(eips_vs_ProductionDataObj.Inst.m_fActualWeight6, response->rsp_data);
									response->rsp_data_size = 4;
								break;
							case 29: //Actual component wt 7
									rta_PutLitEndianFloat(eips_vs_ProductionDataObj.Inst.m_fActualWeight7, response->rsp_data);
									response->rsp_data_size = 4;
								break;
							case 30: //Actual component wt 8
									rta_PutLitEndianFloat(eips_vs_ProductionDataObj.Inst.m_fActualWeight8, response->rsp_data);
									response->rsp_data_size = 4;
								break;
							case 31: //Actual component wt 9
									rta_PutLitEndianFloat(eips_vs_ProductionDataObj.Inst.m_fActualWeight9, response->rsp_data);
									response->rsp_data_size = 4;
								break;
							case 32: //Actual component wt 10
									rta_PutLitEndianFloat(eips_vs_ProductionDataObj.Inst.m_fActualWeight10, response->rsp_data);
									response->rsp_data_size = 4;
								break;
							case 33: //Actual component wt 11
									rta_PutLitEndianFloat(eips_vs_ProductionDataObj.Inst.m_fActualWeight11, response->rsp_data);
									response->rsp_data_size = 4;
								break;
							case 34: //Actual component wt 12
									rta_PutLitEndianFloat(eips_vs_ProductionDataObj.Inst.m_fActualWeight12, response->rsp_data);
									response->rsp_data_size = 4;
								break;

// Actual wts in batch.
							case 35: //Actual Order wt 1
									rta_PutLitEndianFloat(eips_vs_ProductionDataObj.Inst.m_fActualOrderWeight1, response->rsp_data);
									response->rsp_data_size = 4;
								break;
							case 36: //Actual Order wt 2
									rta_PutLitEndianFloat(eips_vs_ProductionDataObj.Inst.m_fActualOrderWeight2, response->rsp_data);
									response->rsp_data_size = 4;
								break;
							case 37: //Actual Order wt 3
									rta_PutLitEndianFloat(eips_vs_ProductionDataObj.Inst.m_fActualOrderWeight3, response->rsp_data);
									response->rsp_data_size = 4;
								break;
							case 38: //Actual Order wt 4
									rta_PutLitEndianFloat(eips_vs_ProductionDataObj.Inst.m_fActualOrderWeight4, response->rsp_data);
									response->rsp_data_size = 4;
								break;
							case 39: //Actual Order wt 5
									rta_PutLitEndianFloat(eips_vs_ProductionDataObj.Inst.m_fActualOrderWeight5, response->rsp_data);
									response->rsp_data_size = 4;
								break;
							case 40: //Actual Order wt 6
									rta_PutLitEndianFloat(eips_vs_ProductionDataObj.Inst.m_fActualOrderWeight6, response->rsp_data);
									response->rsp_data_size = 4;
								break;
							case 41: //Actual Order wt 7
									rta_PutLitEndianFloat(eips_vs_ProductionDataObj.Inst.m_fActualOrderWeight7, response->rsp_data);
									response->rsp_data_size = 4;
								break;
							case 42: //Actual Order wt 8
									rta_PutLitEndianFloat(eips_vs_ProductionDataObj.Inst.m_fActualOrderWeight8, response->rsp_data);
									response->rsp_data_size = 4;
								break;
							case 43: //Actual Order wt 9
									rta_PutLitEndianFloat(eips_vs_ProductionDataObj.Inst.m_fActualOrderWeight9, response->rsp_data);
									response->rsp_data_size = 4;
								break;
							case 44: //Actual Order wt 10
									rta_PutLitEndianFloat(eips_vs_ProductionDataObj.Inst.m_fActualOrderWeight10, response->rsp_data);
									response->rsp_data_size = 4;
								break;
							case 45: //Actual Order wt 11
									rta_PutLitEndianFloat(eips_vs_ProductionDataObj.Inst.m_fActualOrderWeight11, response->rsp_data);
									response->rsp_data_size = 4;
								break;
							case 46: //Actual Order wt 12
									rta_PutLitEndianFloat(eips_vs_ProductionDataObj.Inst.m_fActualOrderWeight12, response->rsp_data);
									response->rsp_data_size = 4;
								break;
							case 47: //Spare Parameter
								rta_PutLitEndian16(eips_vs_ProductionDataObj.Inst.m_nSpareParameter1, response->rsp_data);
									response->rsp_data_size = 2;
								break;



						/* Attribute Unsupported */
						default:
							/* Error: Attribute Not Supported */
							response->gen_stat = ERR_ATTR_UNSUPP;
							break;
					}
				}
				break;
//Production data object end.
// Control Data
			case VS_OBJ_CONTROL_DATA:
				/* Validate the Instance ID */
				if(inst_id > 1) /* class and 1 instance */
				{
					/* Error: Object Does Not Exist */
					response->gen_stat = ERR_UNEXISTANT_OBJ;
					return;
				}

				/* Class Attributes */
				if(inst_id == 0)
				{
					/* switch on the Attribute ID */
					switch(attr_id)
					{
						case 1: /* Revision (uint16) */
							rta_PutLitEndian16(eips_vs_ControlDataObj.Class.ClassRev, response->rsp_data);
							response->rsp_data_size = 2;
							break;

						/* Attribute Unsupported */
						default:
							/* Error: Attribute Not Supported */
							response->gen_stat = ERR_ATTR_UNSUPP;
							break;
					}
				}

				/* Instance Attributes */
				else
				{
					/* switch on the Attribute ID */
					switch(attr_id)
					{
/*
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
 */
						case 1: 	// SetSetIncrease
							rta_PutLitEndian16(eips_vs_ControlDataObj.Inst.m_nSetIncrease, response->rsp_data);
							response->rsp_data_size = 2;
							//printf("\nReading control. attr. #1");
							break;

						case 2: 	// Set Decrease
							rta_PutLitEndian16(eips_vs_ControlDataObj.Inst.m_nSetDecrease, response->rsp_data);
							response->rsp_data_size = 2;
							//printf("\nReading control. attr. #2");
							break;

						case 3: 	// AutoManual.
							rta_PutLitEndian16(eips_vs_ControlDataObj.Inst.m_nSetAutoManual, response->rsp_data);
							response->rsp_data_size = 2;
							//printf("\nReading control. attr. #3");
							break;
						case 4: 	// Set Extruder DAC.
							rta_PutLitEndian16(eips_vs_ControlDataObj.Inst.m_nSetExtruderDAC, response->rsp_data);
							response->rsp_data_size = 2;
							//printf("\nReading control. attr. #4");
							break;
						case 5: 	// Set screw speed (rpm).
							rta_PutLitEndianFloat(eips_vs_ControlDataObj.Inst.m_fSetCommsScrewSpeed, response->rsp_data);
							response->rsp_data_size = 4;
							//printf("\nReading control. attr. #5");
							break;
						case 6: 	// Set line speed (m/min)
							rta_PutLitEndianFloat(eips_vs_ControlDataObj.Inst.m_fSetCommsLineSpeed, response->rsp_data);
							response->rsp_data_size = 4;
							//printf("\nReading control. attr. #6");
							break;

						case 7: //StartClean command
							rta_PutLitEndian16(eips_vs_ControlDataObj.Inst.m_nStartClean, response->rsp_data);
							response->rsp_data_size = 2;
							//printf("\nReading control. attr. #7");
							break;
// following provided for expansion.
						case 8: //Spare command 1
							rta_PutLitEndian16(eips_vs_ControlDataObj.Inst.m_nSpareCommand1, response->rsp_data);
							response->rsp_data_size = 2;
							//printf("\nReading control. attr. #8");
							break;

						case 9: //Spare command 2
							rta_PutLitEndian16(eips_vs_ControlDataObj.Inst.m_nSpareCommand2, response->rsp_data);
							response->rsp_data_size = 2;
							//printf("\nReading control. attr. #9");
							break;

						case 10: //Spare command 3
							rta_PutLitEndian16(eips_vs_ControlDataObj.Inst.m_nSpareCommand3, response->rsp_data);
							response->rsp_data_size = 2;
							//printf("\nReading control. attr. #10");
							break;

					/* Attribute Unsupported */
					default:
						/* Error: Attribute Not Supported */
						response->gen_stat = ERR_ATTR_UNSUPP;
						break;
				}
			}
			break;
			// end of control data object.

        /* **************************************************************************** */
        /*			     Discrete Input Data Object */
        /* **************************************************************************** */
        case VS_OBJ_DISCRETE_INPUT_DATA:
            /* Validate the Instance ID */
            if(inst_id > 1) /* class and 1 instance */
            {
                /* Error: Object Does Not Exist */
                response->gen_stat = ERR_UNEXISTANT_OBJ;
                return;
            }

            /* Class Attributes */
            if(inst_id == 0)
            {
                /* switch on the Attribute ID */
                switch(attr_id)
                {
                    case 1: /* Revision (uint16) */
                        rta_PutLitEndian16(eips_vs_discreteInDataObj.Class.ClassRev, response->rsp_data);
                        response->rsp_data_size = 2;
                        break;

                    /* Attribute Unsupported */
                    default:
                        /* Error: Attribute Not Supported */
                        response->gen_stat = ERR_ATTR_UNSUPP;
                        break;
                }
            }

            /* Instance Attributes */
            else
            {
                /* switch on the Attribute ID */
                switch(attr_id)
                {
                    case 1: /* Number of Input Units (uint16) */
                        rta_PutLitEndian16(eips_vs_discreteInDataObj.Inst.DataSize, response->rsp_data);
                        response->rsp_data_size = 2;
                        break;

	                case 3: /* Input Data (uint8[], uint16[] or uint32[]) */
                        response->rsp_data_size = 0;

                        for(i=0; i<eips_vs_discreteInDataObj.Inst.DataSize; i++)
                        {
#if EIPS_USEROBJ_ASM_ELEMENT_SIZE == 4      /* DWORDS */
                            rta_PutLitEndian32(eips_vs_discreteInDataObj.Inst.DataPtr[i], (response->rsp_data+response->rsp_data_size));
                            response->rsp_data_size += 4;
#elif EIPS_USEROBJ_ASM_ELEMENT_SIZE == 2    /* WORDS */
                            rta_PutLitEndian16(eips_vs_discreteInDataObj.Inst.DataPtr[i], (response->rsp_data+response->rsp_data_size));
                            response->rsp_data_size += 2;
#else                                       /* BYTES */
                            response->rsp_data[response->rsp_data_size] = eips_vs_discreteInDataObj.Inst.DataPtr[i];
                            response->rsp_data_size += 1;
#endif
                        }
	                    break;

                    /* Attribute Unsupported */
                    default:
                        /* Error: Attribute Not Supported */
                        response->gen_stat = ERR_ATTR_UNSUPP;
                        break;
                }
            }
            break;

        /* ******************************************************************** */
        /*			     Discrete Output Data Object */
        /* ******************************************************************** */
        case VS_OBJ_DISCRETE_OUTPUT_DATA:
            /* Validate the Instance ID */
            if(inst_id > 1) /* class and 1 instance */
            {
                /* Error: Object Does Not Exist */
                response->gen_stat = ERR_UNEXISTANT_OBJ;
                return;
            }

            /* Class Attributes */
            if(inst_id == 0)
            {
                /* switch on the Attribute ID */
                switch(attr_id)
                {
                    case 1: /* Revision (uint16) */
                        rta_PutLitEndian16(eips_vs_discreteOutDataObj.Class.ClassRev, response->rsp_data);
                        response->rsp_data_size = 2;
                        break;

                    /* Attribute Unsupported */
                    default:
                        /* Error: Attribute Not Supported */
                        response->gen_stat = ERR_ATTR_UNSUPP;
                        break;
                }
            }

            /* Instance Attributes */
            else
            {
                /* switch on the Attribute ID */
                switch(attr_id)
                {
                    case 1: /* Number of Output Units (uint16) */
                        rta_PutLitEndian16(eips_vs_discreteOutDataObj.Inst.DataSize, response->rsp_data);
                        response->rsp_data_size = 2;
                        break;

	                case 3: /* Output Data (uint8[], uint16[] or uint32[]) */
                        response->rsp_data_size = 0;
                        for(i=0; i<eips_vs_discreteOutDataObj.Inst.DataSize; i++)
                        {
#if EIPS_USEROBJ_ASM_ELEMENT_SIZE == 4      /* DWORDS */
                            rta_PutLitEndian32(eips_vs_discreteOutDataObj.Inst.DataPtr[i], (response->rsp_data+response->rsp_data_size));
                            response->rsp_data_size += 4;
#elif EIPS_USEROBJ_ASM_ELEMENT_SIZE == 2    /* WORDS */
                            rta_PutLitEndian16(eips_vs_discreteOutDataObj.Inst.DataPtr[i], (response->rsp_data+response->rsp_data_size));
                            response->rsp_data_size += 2;
#else                                       /* BYTES */
                            response->rsp_data[response->rsp_data_size] = eips_vs_discreteOutDataObj.Inst.DataPtr[i];
                            response->rsp_data_size += 1;
#endif
                        }
	                    break;

                    /* Attribute Unsupported */
                    default:
                        /* Error: Attribute Not Supported */
                        response->gen_stat = ERR_ATTR_UNSUPP;
                        break;
                }
            }
            break;

        /* ******************************************************************** */
        /*			     Unknown Object */
        /* ******************************************************************** */
        default:
	        /* Error: Object Does Not Exist */
	        response->gen_stat = ERR_UNEXISTANT_OBJ;
	        break;
    };
}

/* ====================================================================
Function:   local_set_attr
Parameters: 16-bit class id
            16-bit instance id
	        8-bit attribute id
	        data buffer size
	        data buffer
	        Pointer to Message Router Response structure
Returns:    N/A

This function is called to process the Get Attribute Single request.
======================================================================= */
void local_set_attr (uint16 class_id, uint16 inst_id, uint8 attr_id, uint16 data_siz, uint8 *data_ptr, EIPS_USER_MSGRTR_RSP_FMT *response)
{
    uint16 i;
/*    eips_user_dbprint3("VS_SetSingle Class 0x%02x Inst 0x%02x, Attr 0x%02x\r\n", class_id, inst_id, attr_id); */

    /* switch on the class id */
    switch(class_id)
    {

		/* EMILY MODIFY START */
		/* if any of the attributes here can be written to, then do the following.  If some are writeable and some aren't, that's okay too.
			As an example, let's say Attribute 1 is writeable, but Attributes 2,3,and 4 are not */
        case VS_OBJ_DOWNLOAD_RECIPE_DATA:
            /* Validate the Instance ID */
            if(inst_id > 1) /* class and 1 instance */
            {
                /* Error: Object Does Not Exist */
                response->gen_stat = ERR_UNEXISTANT_OBJ;
                return;
            }

            /* Class Attributes */
            if(inst_id == 0)
            {
                // Service isn't supported for the class instance
   	            response->gen_stat = ERR_SERV_UNSUPP;
   	            return;
            }

            /* Instance Attributes */
            else
            {
                // switch on the Attribute ID
                switch(attr_id)
                {
					case 1: //m_fPercentage 1 (FLOAT)
						// make sure only 4 bytes are passed
	                    if(local_dlc_valid (data_siz, 4, response) == FALSE)
	                        return; //  return on error
                    	 eips_vs_DownloadRecipeDataObj.Inst.m_fPercentage1 = rta_GetLitEndianFloat(data_ptr);
                    	 if( fdTelnet>0 && g_nTSMDebug.m_bEIPDebug) printf( "\n Percentage 1 Written - %6.2f", eips_vs_DownloadRecipeDataObj.Inst.m_fPercentage1 );
                        break;
					case 2: 	//m_fPercentage 2 (FLOAT)
						// make sure only 4 bytes are passed
	                    if(local_dlc_valid (data_siz, 4, response) == FALSE)
	                        return; //  return on error
                    	 eips_vs_DownloadRecipeDataObj.Inst.m_fPercentage2 = rta_GetLitEndianFloat(data_ptr);
                    	 if( fdTelnet>0 && g_nTSMDebug.m_bEIPDebug) printf( "\n Percentage 2 Written - %6.2f", eips_vs_DownloadRecipeDataObj.Inst.m_fPercentage2 );
                        break;
					case 3: 	//m_fPercentage 3 (FLOAT)
						// make sure only 4 bytes are passed
	                    if(local_dlc_valid (data_siz, 4, response) == FALSE)
	                        return; //  return on error
                    	 eips_vs_DownloadRecipeDataObj.Inst.m_fPercentage3 = rta_GetLitEndianFloat(data_ptr);
                    	 if( fdTelnet>0 && g_nTSMDebug.m_bEIPDebug) printf( "\n Percentage 3 Written - %6.2f", eips_vs_DownloadRecipeDataObj.Inst.m_fPercentage3 );
                        break;
					case 4: 	//m_fPercentage 4 (FLOAT)
						// make sure only 4 bytes are passed
	                    if(local_dlc_valid (data_siz, 4, response) == FALSE)
	                        return; //  return on error
                    	 eips_vs_DownloadRecipeDataObj.Inst.m_fPercentage4 = rta_GetLitEndianFloat(data_ptr);
                    	 if( fdTelnet>0 && g_nTSMDebug.m_bEIPDebug) printf( "\n Percentage 4 Written - %6.2f", eips_vs_DownloadRecipeDataObj.Inst.m_fPercentage4 );
                        break;
					case 5: 	//m_fPercentage 5 (FLOAT)
						// make sure only 4 bytes are passed
	                    if(local_dlc_valid (data_siz, 4, response) == FALSE)
	                        return; //  return on error
                    	 eips_vs_DownloadRecipeDataObj.Inst.m_fPercentage5 = rta_GetLitEndianFloat(data_ptr);
                    	 if( fdTelnet>0 && g_nTSMDebug.m_bEIPDebug) printf( "\n Percentage 5 Written - %6.2f", eips_vs_DownloadRecipeDataObj.Inst.m_fPercentage5 );
                        break;
					case 6: 	//m_fPercentage 6 (FLOAT)
						// make sure only 4 bytes are passed
	                    if(local_dlc_valid (data_siz, 4, response) == FALSE)
	                        return; //  return on error
                    	 eips_vs_DownloadRecipeDataObj.Inst.m_fPercentage6 = rta_GetLitEndianFloat(data_ptr);
                    	 if( fdTelnet>0 && g_nTSMDebug.m_bEIPDebug) printf( "\n Percentage 6 Written - %6.2f", eips_vs_DownloadRecipeDataObj.Inst.m_fPercentage6 );
                        break;
					case 7: 	//m_fPercentage 7 (FLOAT)
						// make sure only 4 bytes are passed
	                    if(local_dlc_valid (data_siz, 4, response) == FALSE)
	                        return; //  return on error
                    	 eips_vs_DownloadRecipeDataObj.Inst.m_fPercentage7 = rta_GetLitEndianFloat(data_ptr);
                    	 if( fdTelnet>0 && g_nTSMDebug.m_bEIPDebug) printf( "\n Percentage 7 Written - %6.2f", eips_vs_DownloadRecipeDataObj.Inst.m_fPercentage7 );
                        break;
					case 8: 	//m_fPercentage 8 (FLOAT)
						// make sure only 4 bytes are passed
	                    if(local_dlc_valid (data_siz, 4, response) == FALSE)
	                        return; //  return on error
                    	 eips_vs_DownloadRecipeDataObj.Inst.m_fPercentage8 = rta_GetLitEndianFloat(data_ptr);
                    	 if( fdTelnet>0 && g_nTSMDebug.m_bEIPDebug) printf( "\n Percentage 8 Written - %6.2f", eips_vs_DownloadRecipeDataObj.Inst.m_fPercentage8 );
                        break;
					case 9: 	//m_fPercentage 9 (FLOAT)
						// make sure only 4 bytes are passed
	                    if(local_dlc_valid (data_siz, 4, response) == FALSE)
	                        return; //  return on error
                    	 eips_vs_DownloadRecipeDataObj.Inst.m_fPercentage9 = rta_GetLitEndianFloat(data_ptr);
                    	 if( fdTelnet>0 && g_nTSMDebug.m_bEIPDebug) printf( "\n Percentage 9 Written - %6.2f", eips_vs_DownloadRecipeDataObj.Inst.m_fPercentage9 );
                        break;
					case 10: 	//m_fPercentage 10 (FLOAT)
						// make sure only 4 bytes are passed
	                    if(local_dlc_valid (data_siz, 4, response) == FALSE)
	                        return; //  return on error
                    	 eips_vs_DownloadRecipeDataObj.Inst.m_fPercentage10 = rta_GetLitEndianFloat(data_ptr);
                    	 if( fdTelnet>0 && g_nTSMDebug.m_bEIPDebug) printf( "\n Percentage 10 Written - %6.2f", eips_vs_DownloadRecipeDataObj.Inst.m_fPercentage10 );
                        break;
					case 11: 	//m_fPercentage 11 (FLOAT)
						// make sure only 4 bytes are passed
	                    if(local_dlc_valid (data_siz, 4, response) == FALSE)
	                        return; //  return on error
                    	 eips_vs_DownloadRecipeDataObj.Inst.m_fPercentage11 = rta_GetLitEndianFloat(data_ptr);
                    	 if( fdTelnet>0 && g_nTSMDebug.m_bEIPDebug) printf( "\n Percentage 11 Written - %6.2f", eips_vs_DownloadRecipeDataObj.Inst.m_fPercentage11 );
                        break;
					case 12: 	//m_fPercentage 12 (FLOAT)
						// make sure only 4 bytes are passed
	                    if(local_dlc_valid (data_siz, 4, response) == FALSE)
	                        return; //  return on error
                    	 eips_vs_DownloadRecipeDataObj.Inst.m_fPercentage12 = rta_GetLitEndianFloat(data_ptr);
                    	 if( fdTelnet>0 && g_nTSMDebug.m_bEIPDebug) printf( "\n Percentage 12 Written - %6.2f", eips_vs_DownloadRecipeDataObj.Inst.m_fPercentage12 );
                        break;

//
                	case 13:		//RegrindComponent;		//ATTR13
						// make sure only 2 bytes are passed
	                    if(local_dlc_valid (data_siz, 2, response) == FALSE)
	                        return; //  return on error
                    	 eips_vs_DownloadRecipeDataObj.Inst.m_nRegrindComponent = rta_GetLitEndian16(data_ptr);
                    	 if( fdTelnet>0 && g_nTSMDebug.m_bEIPDebug) printf( "\n m_nRegrindComponent Written - %d", eips_vs_DownloadRecipeDataObj.Inst.m_nRegrindComponent );
                        break;


                	case 14:	//TotalThroughput;		//ATTR14
						// make sure only 4 bytes are passed
	                    if(local_dlc_valid (data_siz, 4, response) == FALSE)
	                        return; //  return on error
                    	 eips_vs_DownloadRecipeDataObj.Inst.m_fTotalThroughput = rta_GetLitEndianFloat(data_ptr);
                    	 if( fdTelnet>0 && g_nTSMDebug.m_bEIPDebug) printf( "\n TotalThroughput Written - %6.2f", eips_vs_DownloadRecipeDataObj.Inst.m_fTotalThroughput );
                        break;

                	case 15:	//TotalWeightLength;	//ATTR15
						// make sure only 4 bytes are passed
	                    if(local_dlc_valid (data_siz, 4, response) == FALSE)
	                        return; //  return on error
                    	 eips_vs_DownloadRecipeDataObj.Inst.m_fTotalWeightLength = rta_GetLitEndianFloat(data_ptr);
                    	 if( fdTelnet>0 && g_nTSMDebug.m_bEIPDebug) printf( "\nTotalWeightLength Written - %6.2f", eips_vs_DownloadRecipeDataObj.Inst.m_fTotalWeightLength );
                        break;

                	case 16:	//TotalWeightArea;		//ATTR16
						// make sure only 4 bytes are passed
	                    if(local_dlc_valid (data_siz, 4, response) == FALSE)
	                        return; //  return on error
                    	 eips_vs_DownloadRecipeDataObj.Inst.m_fTotalWeightArea = rta_GetLitEndianFloat(data_ptr);
                    	 if( fdTelnet>0 && g_nTSMDebug.m_bEIPDebug) printf( "\n TotalWeightArea Written - %6.2f", eips_vs_DownloadRecipeDataObj.Inst.m_fTotalWeightArea );
                        break;


                	case 17:	//LineSpeed;			//ATTR17
						// make sure only 4 bytes are passed
	                    if(local_dlc_valid (data_siz, 4, response) == FALSE)
	                        return; //  return on error
                    	 eips_vs_DownloadRecipeDataObj.Inst.m_fLineSpeed = rta_GetLitEndianFloat(data_ptr);
                    	 if( fdTelnet>0 && g_nTSMDebug.m_bEIPDebug) printf( "\n LineSpeed Written - %6.2f", eips_vs_DownloadRecipeDataObj.Inst.m_fLineSpeed );
                        break;

                	case 18:	// DesiredWidth;		//ATTR18
						// make sure only 4 bytes are passed
	                    if(local_dlc_valid (data_siz, 4, response) == FALSE)
	                        return; //  return on error
                    	 eips_vs_DownloadRecipeDataObj.Inst.m_fDesiredWidth = rta_GetLitEndianFloat(data_ptr);
                    	 if( fdTelnet>0 && g_nTSMDebug.m_bEIPDebug) printf( "\n Desired Width written - %6.2f", eips_vs_DownloadRecipeDataObj.Inst.m_fDesiredWidth );
                        break;

                	case 19:	// SheetTube;	((uint16))		//ATTR19
						// make sure only 2 bytes are passed
	                    if(local_dlc_valid (data_siz, 2, response) == FALSE)
	                        return; //  return on error
                    	 eips_vs_DownloadRecipeDataObj.Inst.m_nSheetTube = rta_GetLitEndian16(data_ptr);
                    	 if( fdTelnet>0 && g_nTSMDebug.m_bEIPDebug) printf( "\n SheetTube Written - %d", eips_vs_DownloadRecipeDataObj.Inst.m_nSheetTube );
                        break;

/*
                		nTemp = 0;
                		if(eips_vs_DownloadRecipeDataObj.Inst.m_nSheetTube)
                			nTemp = 1;
				        rta_ByteMove(response->rsp_data, &nTemp, 1);
				        // the total length is the 1chars of data ??
            		 	response->rsp_data_size = 1;
                		 break;
*/

                    case 20: // m_nFileNumber (uint16)
						// make sure only 2 bytes are passed
	                    if(local_dlc_valid (data_siz, 2, response) == FALSE)
	                        return; //  return on error
                    	 eips_vs_DownloadRecipeDataObj.Inst.m_nFileNumber = rta_GetLitEndian16(data_ptr);
                    	 if( fdTelnet>0 && g_nTSMDebug.m_bEIPDebug) printf( "\n File Number Written - %d", eips_vs_DownloadRecipeDataObj.Inst.m_nFileNumber );
                        break;


                	case 21:	//m_nResetTotals;			//ATTR21
						// make sure only 2 bytes are passed
	                    if(local_dlc_valid (data_siz, 2, response) == FALSE)
	                        return; //  return on error
                    	 eips_vs_DownloadRecipeDataObj.Inst.m_nResetTotals = rta_GetLitEndian16(data_ptr);
                    	 if( fdTelnet>0 && g_nTSMDebug.m_bEIPDebug) printf( "\n ResetTotals Written - %d", eips_vs_DownloadRecipeDataObj.Inst.m_nResetTotals );
                        break;

/*
                        rta_PutLitEndian16(eips_vs_DownloadRecipeDataObj.Inst.m_nResetTotals, response->rsp_data);
                        response->rsp_data_size = 2;
*/

                		 break;
                	case 22:	//m_nLoadRecipeCommand;	//ATTR22
						// make sure only 2 bytes are passed
	                    if(local_dlc_valid (data_siz, 2, response) == FALSE)
	                        return; //  return on error
                    	 eips_vs_DownloadRecipeDataObj.Inst.m_nLoadRecipeCommand = rta_GetLitEndian16(data_ptr);
                    	 if( fdTelnet>0 && g_nTSMDebug.m_bEIPDebug) printf( "\n Load Recipe Command Written - %d", eips_vs_DownloadRecipeDataObj.Inst.m_nLoadRecipeCommand );
                        break;
/*
                        rta_PutLitEndian16(eips_vs_DownloadRecipeDataObj.Inst.m_nLoadRecipeCommand, response->rsp_data);
                        response->rsp_data_size = 2;
                		 break;
*/

                	case 23:	// m_nRecipeSpareAttribute1; //ATTR 23
						// make sure only 2 bytes are passed
	                    if(local_dlc_valid (data_siz, 2, response) == FALSE)
	                        return; //  return on error
                    	 eips_vs_DownloadRecipeDataObj.Inst.m_nRecipeSpareAttribute1 = rta_GetLitEndian16(data_ptr);
                    	 if( fdTelnet>0 && g_nTSMDebug.m_bEIPDebug) printf( "\n Recipe Spare Attribute1 Written - %d", eips_vs_DownloadRecipeDataObj.Inst.m_nRecipeSpareAttribute1 );
                        break;

//                        rta_PutLitEndian16(eips_vs_DownloadRecipeDataObj.Inst.m_nRecipeSpareAttribute1, response->rsp_data);
//                        response->rsp_data_size = 2;
//                		 break;
                	case 24:	// m_nRecipeSpareAttribute2; //ATTR 24
						// make sure only 2 bytes are passed
	                    if(local_dlc_valid (data_siz, 2, response) == FALSE)
	                        return; //  return on error
                    	 eips_vs_DownloadRecipeDataObj.Inst.m_nRecipeSpareAttribute2 = rta_GetLitEndian16(data_ptr);
                    	 if( fdTelnet>0 && g_nTSMDebug.m_bEIPDebug) printf( "\n Recipe Spare Attribute2 Written - %d", eips_vs_DownloadRecipeDataObj.Inst.m_nRecipeSpareAttribute2 );
                        break;

                	case 25:	// m_fRecipeSpareAttribute3; //ATTR25
						// make sure only 4 bytes are passed
	                    if(local_dlc_valid (data_siz, 4, response) == FALSE)
	                        return; //  return on error
                    	 eips_vs_DownloadRecipeDataObj.Inst.m_fRecipeSpareAttribute3 = rta_GetLitEndianFloat(data_ptr);
                    	 if( fdTelnet>0 && g_nTSMDebug.m_bEIPDebug) printf( "\n RecipeSpareAttribute3 written - %6.2f", eips_vs_DownloadRecipeDataObj.Inst.m_fRecipeSpareAttribute3 );
                        break;

//            		 	rta_PutLitEndianFloat(eips_vs_DownloadRecipeDataObj.Inst.m_fRecipeSpareAttribute3, response->rsp_data);
//            		 	response->rsp_data_size = 4;
//                		 break;
                	case 26:	// m_fRecipeSpareAttribute4; //ATTR26
						// make sure only 4 bytes are passed
	                    if(local_dlc_valid (data_siz, 4, response) == FALSE)
	                        return; //  return on error
                    	 eips_vs_DownloadRecipeDataObj.Inst.m_fRecipeSpareAttribute4 = rta_GetLitEndianFloat(data_ptr);
                    	 if( fdTelnet>0 && g_nTSMDebug.m_bEIPDebug) printf( "\n RecipeSpareAttribute4 written - %6.2f", eips_vs_DownloadRecipeDataObj.Inst.m_fRecipeSpareAttribute4 );
                        break;

//            		 	rta_PutLitEndianFloat(eips_vs_DownloadRecipeDataObj.Inst.m_fRecipeSpareAttribute4, response->rsp_data);
//            		 	response->rsp_data_size = 4;
//                		 break;

                	case 27: // m_arrDescription
// - Assuming use of character string called SHORT_STRING that stores the length as the first byte
//   and then stores the data (no NULL termination). SHORT_STRING is typically 32 characters or less and can vary in length.
                        // we must have at least one byte
                        if(data_siz < 1)		//data_siz is no. of chars in msg, including 1st, i.e. string length.
                        {						//data_siz-1 = chars in string.
                            // Error: "Not Enough Data"
                            response->gen_stat = ERR_INSUFF_DATA;
                            return; // return on error
                        }
                        // validate data size
                        if(local_dlc_valid (data_siz, (data_ptr[0]+1), response) == FALSE)
                        	return;

                        if(data_siz-1 > 19)	//.m_arrDescription  max size = 20 bytes, & reserve 1 byte for termination.
                        {
                            // Error: "Too Much Data"
                            response->gen_stat = ERR_TOOMUCH_DATA;
                            return; // return on error
                        }

						rta_ByteMove( (unsigned char*)eips_vs_DownloadRecipeDataObj.Inst.m_arrDescription, data_ptr+1, data_siz-1);
						eips_vs_DownloadRecipeDataObj.Inst.m_arrDescription[data_siz-1] = '\0';

						// make sure only 20 bytes are passed
//	                    if(local_dlc_valid (data_siz, 20, response) == FALSE)
//	                        return; //  return on error
//						rta_ByteMove( (unsigned char*)eips_vs_DownloadRecipeDataObj.Inst.m_arrDescription, data_ptr, 20);

                    	 //eips_vs_DownloadRecipeDataObj.Inst.m_arrDescription = rta_GetLitEndianFloat(data_ptr);
                    	 if( fdTelnet>0 && g_nTSMDebug.m_bEIPDebug) printf( "\n Recipe Description written - %s", eips_vs_DownloadRecipeDataObj.Inst.m_arrDescription );
                        break;

                		  //1st byte is equal to the number of characters in the description
							        //response->rsp_data[0] = 20;

							        // copy 20 chars of data
							        //rta_ByteMove(response->rsp_data+1, (uint8*)eips_vs_DownloadRecipeDataObj.Inst.m_arrDescription, 20);

							        // the total length is the 20 chars of data + 1 //
							        //response->rsp_data_size = 20+1;
								//break;

//
                	case 28:	// m_nRecipeSpareAttribute5; //ATTR 28
						// make sure only 2 bytes are passed
	                    if(local_dlc_valid (data_siz, 2, response) == FALSE)
	                        return; //  return on error
                    	 eips_vs_DownloadRecipeDataObj.Inst.m_nRecipeSpareAttribute5 = rta_GetLitEndian16(data_ptr);
                    	 if( fdTelnet>0 && g_nTSMDebug.m_bEIPDebug) printf( "\n Recipe Spare Attribute5 Written - %d", eips_vs_DownloadRecipeDataObj.Inst.m_nRecipeSpareAttribute5 );
                        break;
                	case 29:	// m_nRecipeSpareAttribute6; //ATTR 29
						// make sure only 2 bytes are passed
	                    if(local_dlc_valid (data_siz, 2, response) == FALSE)
	                        return; //  return on error
                    	 eips_vs_DownloadRecipeDataObj.Inst.m_nRecipeSpareAttribute6 = rta_GetLitEndian16(data_ptr);
                    	 if( fdTelnet>0 && g_nTSMDebug.m_bEIPDebug) printf( "\n Recipe Spare Attribute6 Written - %d", eips_vs_DownloadRecipeDataObj.Inst.m_nRecipeSpareAttribute6 );
                        break;
                	case 30:	// m_nRecipeSpareAttribute7; //ATTR 30
						// make sure only 2 bytes are passed
	                    if(local_dlc_valid (data_siz, 2, response) == FALSE)
	                        return; //  return on error
                    	 eips_vs_DownloadRecipeDataObj.Inst.m_nRecipeSpareAttribute7 = rta_GetLitEndian16(data_ptr);
                    	 if( fdTelnet>0 && g_nTSMDebug.m_bEIPDebug) printf( "\n Recipe Spare Attribute7 Written - %d", eips_vs_DownloadRecipeDataObj.Inst.m_nRecipeSpareAttribute7 );
                        break;

	                	case 31: //
                        response->gen_stat = ERR_ATTR_READONLY;
                        return;

                    /* Attribute Unsupported */
                    default:
                        /* Error: Attribute Not Supported */
                        response->gen_stat = ERR_ATTR_UNSUPP;
                        break;
                }
            }
            break;
		/* EMILY MODIFY END */
//control object start
        case VS_OBJ_CONTROL_DATA:
            /* Validate the Instance ID */
            if(inst_id > 1) /* class and 1 instance */
            {
                /* Error: Object Does Not Exist */
                response->gen_stat = ERR_UNEXISTANT_OBJ;
                return;
            }

            /* Class Attributes */
            if(inst_id == 0)
            {
                /* Service isn't supported for the class instance */
   	            response->gen_stat = ERR_SERV_UNSUPP;
   	            return;
            }

            /* Instance Attributes */
            else
            {
                /* switch on the Attribute ID */
                switch(attr_id)
                {
/*
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

 */

                    case 1: //SetIncrease(uint16)
                    	/* make sure only 2 bytes are passed */
	                    /* validate the data length (function builds the err msg if needed) */
	                    if(local_dlc_valid (data_siz, 2, response) == FALSE)
	                        return; /* return on error */
                    	 eips_vs_ControlDataObj.Inst.m_nSetIncrease = rta_GetLitEndian16(data_ptr);
                    	 g_nEIPToggleStatusCommandWritten |= EIP_INCREASE_WROTE;
                        break;
                    case 2: //SetDecrease(uint16)
                     	/* make sure only 2 bytes are passed */
 	                    /* validate the data length (function builds the err msg if needed) */
 	                    if(local_dlc_valid (data_siz, 2, response) == FALSE)
 	                        return; /* return on error */
                     	 eips_vs_ControlDataObj.Inst.m_nSetDecrease = rta_GetLitEndian16(data_ptr);
						 g_nEIPToggleStatusCommandWritten |= EIP_DECREASE_WROTE;

                         break;

	                	case 3: //SetAutoManual
		                    if(local_dlc_valid (data_siz, 2, response) == FALSE)
		                        return; /* return on error */
	                    	 eips_vs_ControlDataObj.Inst.m_nSetAutoManual = rta_GetLitEndian16(data_ptr);
							 g_nEIPToggleStatusCommandWritten |= EIP_AUTO_MANUAL_WROTE;
	                        break;

	                	case 4: //SetExtruderDAC
		                    if(local_dlc_valid (data_siz, 2, response) == FALSE)
		                        return; /* return on error */
	                    	 eips_vs_ControlDataObj.Inst.m_nSetExtruderDAC = rta_GetLitEndian16(data_ptr);
							 g_nEIPToggleStatusCommandWritten |= EIP_EXTRUDER_DAC_WROTE;
	                        break;

	                	case 5: //SetCommsScrewSpeed
		                    if(local_dlc_valid (data_siz, 4, response) == FALSE)
		                        return; /* return on error */
	                    	 eips_vs_ControlDataObj.Inst.m_fSetCommsScrewSpeed = rta_GetLitEndianFloat(data_ptr);
							 g_nEIPToggleStatusCommandWritten |= EIP_SCREW_SPEED_WROTE;
	                        break;

	                	case 6: //SetCommsLineSpeed
		                    if(local_dlc_valid (data_siz, 4, response) == FALSE)
		                        return; /* return on error */
	                    	 eips_vs_ControlDataObj.Inst.m_fSetCommsLineSpeed = rta_GetLitEndianFloat(data_ptr);
							 g_nEIPToggleStatusCommandWritten |= EIP_LINE_SPEED_WROTE;

	                        break;

	                	case 7: //StartClean
	                        if(local_dlc_valid (data_siz, 2, response) == FALSE)
	      		                        return; /* return on error */
	      	                    	 eips_vs_ControlDataObj.Inst.m_nStartClean = rta_GetLitEndian16(data_ptr);
	    							 g_nEIPToggleStatusCommandWritten |= EIP_START_CLEAN_WROTE;
							 break;
// following provided for expansion.
	                    case 8: //Spare Command1(uint16)
	                     	/* make sure only 2 bytes are passed */
	 	                    /* validate the data length (function builds the err msg if needed) */
	 	                    if(local_dlc_valid (data_siz, 2, response) == FALSE)
	 	                        return; /* return on error */
	                     	 eips_vs_ControlDataObj.Inst.m_nSpareCommand1 = rta_GetLitEndian16(data_ptr);
							 // COPY TO MODBUS TABLE
//	                     	 if( fdTelnet>0 && g_nTSMDebug.m_bEIPDebug) printf( "\n SpareCommand1 Written - %04x", eips_vs_ControlDataObj.Inst.m_nSpareCommand1 );
	                         break;

	                    case 9: //Spare Command2(uint16)
	                     	/* make sure only 2 bytes are passed */
	 	                    /* validate the data length (function builds the err msg if needed) */
	 	                    if(local_dlc_valid (data_siz, 2, response) == FALSE)
	 	                        return; /* return on error */
	                     	 eips_vs_ControlDataObj.Inst.m_nSpareCommand2 = rta_GetLitEndian16(data_ptr);
							 // COPY TO MODBUS TABLE
	                     	 //if( fdTelnet>0 && g_nTSMDebug.m_bEIPDebug) printf( "\n SpareCommand2 Written - %04x", eips_vs_ControlDataObj.Inst.m_nSpareCommand2 );
	                         break;

	                    case 10: //Spare Command3(uint16)
	                     	/* make sure only 2 bytes are passed */
	 	                    /* validate the data length (function builds the err msg if needed) */
	 	                    if(local_dlc_valid (data_siz, 2, response) == FALSE)
	 	                        return; /* return on error */
	                     	 eips_vs_ControlDataObj.Inst.m_nSpareCommand3 = rta_GetLitEndian16(data_ptr);
							 // COPY TO MODBUS TABLE
	                     	 //if( fdTelnet>0 && g_nTSMDebug.m_bEIPDebug) printf( "\n SpareCommand3 Written - %04x", eips_vs_ControlDataObj.Inst.m_nSpareCommand3 );
	                         break;


                    /* Attribute Unsupported */
                    default:
                        /* Error: Attribute Not Supported */
                        response->gen_stat = ERR_ATTR_UNSUPP;
                        break;
                }
            }
            break;

//control object end

        /* **************************************************************************** */
        /*			     Discrete Input Data Object */
        /*			     Analog Input Data Object */
        /* **************************************************************************** */
        case VS_OBJ_DISCRETE_INPUT_DATA:
            /* Validate the Instance ID */
            if(inst_id > 1) /* class and 1 instance */
            {
                /* Error: Object Does Not Exist */
                response->gen_stat = ERR_UNEXISTANT_OBJ;
                return;
	        }

            // Service isn't supported for this object //
   	        response->gen_stat = ERR_SERV_UNSUPP;
	        return;

	        // break; // this isn't needed since we return //

        /* **************************************************************************** */
        /*			     Discrete Output Data Object */
        /* **************************************************************************** */
        case VS_OBJ_DISCRETE_OUTPUT_DATA:
            // Validate the Instance ID //
            if(inst_id > 1) // class and 1 instance //
            {
    	        /* Error: Object Does Not Exist */
	            response->gen_stat = ERR_UNEXISTANT_OBJ;
  	            return;
	        }

	        /* Class Attributes */
	        if(inst_id == 0)
	        {
                // Service isn't supported for the class instance
   	            response->gen_stat = ERR_SERV_UNSUPP;
   	            return;
   	        }

   	        // Instance Attributes
   	        else
   	        {
   	            /* data byte 0 is the Attribute ID */
   	            switch(attr_id)
   	            {
   	                /* ***************************** */
   	                /* Error: Attribute(s) Read Only */
   	                /* ***************************** */
                    case 1: /* Number of Output Words (uint16) */
                        response->gen_stat = ERR_ATTR_READONLY;
                        return;

	                case 3: /* Output Data (uint16[]) */
                        /* validate we don't have an I/O connection (else "Device State Conflict") */
                        if(eips_iomsg_allocated(local_o2t_asm_struct[0].instID) == TRUE)
                        {
                            /* "Device State Conflict" */
                            response->gen_stat = ERR_DEV_STATE_CONFLICT;
                            return; /* return on error */
                        }

#if EIPS_USEROBJ_ASM_ELEMENT_SIZE == 4      /* DWORDS */
	                    /* validate the data length (function builds the err msg if needed) */
	                    if(local_dlc_valid (data_siz, (uint16)(eips_vs_discreteOutDataObj.Inst.DataSize*4), response) == FALSE)
	                        return; /* return on error */

	                    /* we don't validate the data, just store it */
                        for(i=0; i<eips_vs_discreteOutDataObj.Inst.DataSize; i++)
                        {
                            eips_vs_discreteOutDataObj.Inst.DataPtr[i] = rta_GetLitEndian32((data_ptr+(i*4)));

                            /* update the assembly with the output data */
                            local_o2t_asm_struct[0].data_ptr[i] = eips_vs_discreteOutDataObj.Inst.DataPtr[i];
                        }

#elif EIPS_USEROBJ_ASM_ELEMENT_SIZE == 2    /* WORDS */
	                    /* validate the data length (function builds the err msg if needed) */
	                    if(local_dlc_valid (data_siz, (uint16)(eips_vs_discreteOutDataObj.Inst.DataSize*2), response) == FALSE)
	                        return; /* return on error */

	                    /* we don't validate the data, just store it */
                        for(i=0; i<eips_vs_discreteOutDataObj.Inst.DataSize; i++)
                        {
                            eips_vs_discreteOutDataObj.Inst.DataPtr[i] = rta_GetLitEndian16((data_ptr+(i*2)));

#if EIPS_USEROBJ_ASM_MAXNUM_O2TINST > 0
                            /* update the assembly with the output data */
                            local_o2t_asm_struct[0].data_ptr[i] = eips_vs_discreteOutDataObj.Inst.DataPtr[i];
#endif
                        }

#else                                       /* BYTES */
	                    /* validate the data length (function builds the err msg if needed) */
	                    if(local_dlc_valid (data_siz, (uint16)(eips_vs_discreteOutDataObj.Inst.DataSize*2), response) == FALSE)
	                        return; /* return on error */

	                    /* we don't validate the data, just store it */
                        for(i=0; i<eips_vs_discreteOutDataObj.Inst.DataSize; i++)
                        {
                            eips_vs_discreteOutDataObj.Inst.DataPtr[i] = data_ptr[i];

                            /* update the assembly with the output data */
                            local_o2t_asm_struct[0].data_ptr[i] = eips_vs_discreteOutDataObj.Inst.DataPtr[i];
                        }
#endif
                        break;

                    /* Attribute Unsupported */
                    default:
                        /* Error: Attribute Not Supported */
                        response->gen_stat = ERR_ATTR_UNSUPP;
                        break;
                }; /* END-> "switch(attr_id)"       */
            } /* END-> else "Instance Attributes"  */
            break;

        default:
            /* Error: Object Does Not Exist */
            response->gen_stat = ERR_UNEXISTANT_OBJ;
	        break;
    }; /* END-> "switch(class_id)" */
}

/* ====================================================================
Function:   local_dlc_valid
Parameters: actual size
            needed size
            pointer for the error message (if needed)
Returns:    TRUE/FALSE

This function is called to start the SPI task.
======================================================================= */
uint8 local_dlc_valid (uint16 actual_siz, uint16 needed_siz, EIPS_USER_MSGRTR_RSP_FMT *response)
{
    /* Error: Not Enough Data */
    if(actual_siz < needed_siz)
    {
        response->gen_stat = ERR_INSUFF_DATA;
        return(FALSE);
    }

    /* Error: Too Much Data */
    if(actual_siz > needed_siz)
    {
        response->gen_stat = ERR_TOOMUCH_DATA;
        return(FALSE);
    }

    /* size is correct */
    return(TRUE);
}
//Emily27012010
/* ====================================================================
Function:     local_update_produce_asm
Parameters:   N/A
Returns:      N/A

This function is when the produce assembly needs to be updated
======================================================================= */
void local_update_produce_asm (void)
{
		// copy bytes 0 - 3 to this attribute
//		rta_PutLitEndianFloat(eips_vs_CurrentRecipeDataObj.Inst.m_fPercentage1, &local_t2o_asm_struct[0].data_ptr[0]);
		// copy bytes 4 - 7 to this attribute
//		rta_PutLitEndianFloat(eips_vs_ProductionDataObj.Inst.m_fActualThroughput, &local_t2o_asm_struct[0].data_ptr[4]);

// copy bytes 0 - 3 to this attribute
	    rta_PutLitEndianFloat(eips_vs_ProductionDataObj.Inst.m_fActualThroughput, &local_t2o_asm_struct[0].data_ptr[0]);				//ATTR01
// copy bytes 4 - 7 to this attribute
	    rta_PutLitEndianFloat(eips_vs_ProductionDataObj.Inst.m_fActualWPL, &local_t2o_asm_struct[0].data_ptr[4]);						//ATTR02 wt/length
	    rta_PutLitEndianFloat(eips_vs_ProductionDataObj.Inst.m_fActualWPA, &local_t2o_asm_struct[0].data_ptr[8]);						//ATTR03 wt/area.
	    rta_PutLitEndianFloat(eips_vs_ProductionDataObj.Inst.m_fActualOrderWeight, &local_t2o_asm_struct[0].data_ptr[12]);				//ATTR04
	    rta_PutLitEndianFloat(eips_vs_ProductionDataObj.Inst.m_fActualLineSpeed, &local_t2o_asm_struct[0].data_ptr[16]);					//ATTR05
	    rta_PutLitEndian16(eips_vs_ProductionDataObj.Inst.m_nOperationalStatus, &local_t2o_asm_struct[0].data_ptr[20]);				//ATTR06
	    rta_PutLitEndian16(eips_vs_ProductionDataObj.Inst.m_nMotorDA, &local_t2o_asm_struct[0].data_ptr[22]);							//ATTR07
	    rta_PutLitEndian16(eips_vs_ProductionDataObj.Inst.m_nOperationalStatus2, &local_t2o_asm_struct[0].data_ptr[24]);				//ATTR08
	 	rta_PutLitEndian16(eips_vs_ProductionDataObj.Inst.m_nBatchCounter, &local_t2o_asm_struct[0].data_ptr[26]);					//Attr09
	 	rta_PutLitEndian16(eips_vs_ProductionDataObj.Inst.m_nProcessAlarms, &local_t2o_asm_struct[0].data_ptr[28]);					//Attr10
// Actual %'s
	    rta_PutLitEndianFloat(eips_vs_ProductionDataObj.Inst.m_fActualPercentage1, &local_t2o_asm_struct[0].data_ptr[30]);				//ATTR11
	    rta_PutLitEndianFloat(eips_vs_ProductionDataObj.Inst.m_fActualPercentage2, &local_t2o_asm_struct[0].data_ptr[34]);				//ATTR12
	    rta_PutLitEndianFloat(eips_vs_ProductionDataObj.Inst.m_fActualPercentage3, &local_t2o_asm_struct[0].data_ptr[38]);				//ATTR13
	    rta_PutLitEndianFloat(eips_vs_ProductionDataObj.Inst.m_fActualPercentage4, &local_t2o_asm_struct[0].data_ptr[42]);				//ATTR14

	    rta_PutLitEndianFloat(eips_vs_ProductionDataObj.Inst.m_fActualPercentage5, &local_t2o_asm_struct[0].data_ptr[46]);				//ATTR15
	    rta_PutLitEndianFloat(eips_vs_ProductionDataObj.Inst.m_fActualPercentage6, &local_t2o_asm_struct[0].data_ptr[50]);				//ATTR16
	    rta_PutLitEndianFloat(eips_vs_ProductionDataObj.Inst.m_fActualPercentage7, &local_t2o_asm_struct[0].data_ptr[54]);  			//ATTR17
	    rta_PutLitEndianFloat(eips_vs_ProductionDataObj.Inst.m_fActualPercentage8, &local_t2o_asm_struct[0].data_ptr[58]);				//ATTR18



	    rta_PutLitEndianFloat(eips_vs_ProductionDataObj.Inst.m_fActualPercentage9, &local_t2o_asm_struct[0].data_ptr[62]);				//ATTR15
	    rta_PutLitEndianFloat(eips_vs_ProductionDataObj.Inst.m_fActualPercentage10, &local_t2o_asm_struct[0].data_ptr[66]);				//ATTR16
	    rta_PutLitEndianFloat(eips_vs_ProductionDataObj.Inst.m_fActualPercentage11, &local_t2o_asm_struct[0].data_ptr[70]);  			//ATTR17
	    rta_PutLitEndianFloat(eips_vs_ProductionDataObj.Inst.m_fActualPercentage12, &local_t2o_asm_struct[0].data_ptr[74]);				//ATTR18

//Target %'s
		rta_PutLitEndianFloat(eips_vs_CurrentRecipeDataObj.Inst.m_fPercentage1, &local_t2o_asm_struct[0].data_ptr[78]);
		rta_PutLitEndianFloat(eips_vs_CurrentRecipeDataObj.Inst.m_fPercentage2, &local_t2o_asm_struct[0].data_ptr[82]);
		rta_PutLitEndianFloat(eips_vs_CurrentRecipeDataObj.Inst.m_fPercentage3, &local_t2o_asm_struct[0].data_ptr[86]);
		rta_PutLitEndianFloat(eips_vs_CurrentRecipeDataObj.Inst.m_fPercentage4, &local_t2o_asm_struct[0].data_ptr[90]);

		rta_PutLitEndianFloat(eips_vs_CurrentRecipeDataObj.Inst.m_fPercentage5, &local_t2o_asm_struct[0].data_ptr[94]);
		rta_PutLitEndianFloat(eips_vs_CurrentRecipeDataObj.Inst.m_fPercentage6, &local_t2o_asm_struct[0].data_ptr[98]);
		rta_PutLitEndianFloat(eips_vs_CurrentRecipeDataObj.Inst.m_fPercentage7, &local_t2o_asm_struct[0].data_ptr[102]);
		rta_PutLitEndianFloat(eips_vs_CurrentRecipeDataObj.Inst.m_fPercentage8, &local_t2o_asm_struct[0].data_ptr[106]);

		rta_PutLitEndianFloat(eips_vs_CurrentRecipeDataObj.Inst.m_fPercentage9, &local_t2o_asm_struct[0].data_ptr[110]);
		rta_PutLitEndianFloat(eips_vs_CurrentRecipeDataObj.Inst.m_fPercentage10, &local_t2o_asm_struct[0].data_ptr[114]);
		rta_PutLitEndianFloat(eips_vs_CurrentRecipeDataObj.Inst.m_fPercentage11, &local_t2o_asm_struct[0].data_ptr[118]);
		rta_PutLitEndianFloat(eips_vs_CurrentRecipeDataObj.Inst.m_fPercentage12, &local_t2o_asm_struct[0].data_ptr[122]);
// test only put test value at end.
	    // test only put test value at end.
	    	 	rta_PutLitEndian16(eips_vs_ProductionDataObj.Inst.m_nSpareParameter1, &local_t2o_asm_struct[0].data_ptr[126]);
//	 	rta_PutLitEndian16(22, &local_t2o_asm_struct[0].data_ptr[126]);

#if 0
#endif
}

/* *********** */
/* END OF FILE */
/* *********** */
// Added by TSM  Test only.
void InitEIPSDownloadRecipeDataObj (void )
{
	eips_vs_DownloadRecipeDataObj.Class.ClassRev = 1;

	sprintf( eips_vs_DownloadRecipeDataObj.Inst.m_arrDescription, "Recipe 01");

	eips_vs_DownloadRecipeDataObj.Inst.m_fPercentage1 = 53.0f;
	eips_vs_DownloadRecipeDataObj.Inst.m_fPercentage2 = 27.0f;
	eips_vs_DownloadRecipeDataObj.Inst.m_fPercentage3 = 14.82f;
	eips_vs_DownloadRecipeDataObj.Inst.m_fPercentage4 = 5.18f;

	eips_vs_DownloadRecipeDataObj.Inst.m_fPercentage5 = 0.0f;
	eips_vs_DownloadRecipeDataObj.Inst.m_fPercentage6 = 0.0f;
	eips_vs_DownloadRecipeDataObj.Inst.m_fPercentage7 = 0.0f;
	eips_vs_DownloadRecipeDataObj.Inst.m_fPercentage8 = 0.0f;
	eips_vs_DownloadRecipeDataObj.Inst.m_fPercentage9 = 0.0f;
	eips_vs_DownloadRecipeDataObj.Inst.m_fPercentage10 = 0.0f;
	eips_vs_DownloadRecipeDataObj.Inst.m_fPercentage11 = 0.0f;
	eips_vs_DownloadRecipeDataObj.Inst.m_fPercentage12 = 0.0f;

	eips_vs_DownloadRecipeDataObj.Inst.m_nRegrindComponent = 0;

	eips_vs_DownloadRecipeDataObj.Inst.m_fTotalThroughput = 99.9f;
	eips_vs_DownloadRecipeDataObj.Inst.m_fTotalWeightLength = 25.5f;
	eips_vs_DownloadRecipeDataObj.Inst.m_fTotalWeightArea = 24.2f;

	eips_vs_DownloadRecipeDataObj.Inst.m_fLineSpeed = 64.2f;
	eips_vs_DownloadRecipeDataObj.Inst.m_fDesiredWidth = 1001.0f;
	eips_vs_DownloadRecipeDataObj.Inst.m_nSheetTube = 0;

	eips_vs_DownloadRecipeDataObj.Inst.m_nFileNumber = 1;

	eips_vs_DownloadRecipeDataObj.Inst.m_nResetTotals = 0;
	eips_vs_DownloadRecipeDataObj.Inst.m_nLoadRecipeCommand = 0;

	eips_vs_DownloadRecipeDataObj.Inst.m_nRecipeSpareAttribute1 = 0;
	eips_vs_DownloadRecipeDataObj.Inst.m_nRecipeSpareAttribute2 = 0;

	eips_vs_DownloadRecipeDataObj.Inst.m_fRecipeSpareAttribute3 = 0.0f;
	eips_vs_DownloadRecipeDataObj.Inst.m_fRecipeSpareAttribute4 = 0.0f;

	eips_vs_DownloadRecipeDataObj.Inst.m_nRecipeSpareAttribute5 = 1;
	eips_vs_DownloadRecipeDataObj.Inst.m_nRecipeSpareAttribute6 = 2;
	eips_vs_DownloadRecipeDataObj.Inst.m_nRecipeSpareAttribute7 = 3;

}

void InitEIPSCurrentRecipeDataObj (void )
{
	eips_vs_CurrentRecipeDataObj.Class.ClassRev = 1;

	sprintf( eips_vs_CurrentRecipeDataObj.Inst.m_arrDescription, "Recipe 11");

	eips_vs_CurrentRecipeDataObj.Inst.m_fPercentage1 = 49.90f;
	eips_vs_CurrentRecipeDataObj.Inst.m_fPercentage2 = 10.1f;
	eips_vs_CurrentRecipeDataObj.Inst.m_fPercentage3 = 6.0f;
	eips_vs_CurrentRecipeDataObj.Inst.m_fPercentage4 = 14.0f;

	eips_vs_CurrentRecipeDataObj.Inst.m_fPercentage5 = 1.0f;
	eips_vs_CurrentRecipeDataObj.Inst.m_fPercentage6 = 2.0f;
	eips_vs_CurrentRecipeDataObj.Inst.m_fPercentage7 = 3.0f;
	eips_vs_CurrentRecipeDataObj.Inst.m_fPercentage8 = 4.0f;
	eips_vs_CurrentRecipeDataObj.Inst.m_fPercentage9 = 1.0f;
	eips_vs_CurrentRecipeDataObj.Inst.m_fPercentage10 = 2.0f;
	eips_vs_CurrentRecipeDataObj.Inst.m_fPercentage11 = 3.0f;
	eips_vs_CurrentRecipeDataObj.Inst.m_fPercentage12 = 4.0f;

	eips_vs_CurrentRecipeDataObj.Inst.m_nRegrindComponent = 0;

	eips_vs_CurrentRecipeDataObj.Inst.m_fTotalThroughput = 99.9f;
	eips_vs_CurrentRecipeDataObj.Inst.m_fTotalWeightLength = 25.3f;
	eips_vs_CurrentRecipeDataObj.Inst.m_fTotalWeightArea = 24.2f;

	eips_vs_CurrentRecipeDataObj.Inst.m_fLineSpeed = 64.2f;
	eips_vs_CurrentRecipeDataObj.Inst.m_fDesiredWidth = 1100.0f;
	eips_vs_CurrentRecipeDataObj.Inst.m_nSheetTube = 0;

	eips_vs_CurrentRecipeDataObj.Inst.m_nFileNumber = 1;

	eips_vs_CurrentRecipeDataObj.Inst.m_nResetTotals = 0;
	eips_vs_CurrentRecipeDataObj.Inst.m_nLoadRecipeCommand = 0;

	eips_vs_CurrentRecipeDataObj.Inst.m_nRecipeSpareAttribute1 = 0;
	eips_vs_CurrentRecipeDataObj.Inst.m_nRecipeSpareAttribute2 = 0;

	eips_vs_CurrentRecipeDataObj.Inst.m_fRecipeSpareAttribute3 = 0.0f;
	eips_vs_CurrentRecipeDataObj.Inst.m_fRecipeSpareAttribute4 = 0.0f;

	eips_vs_CurrentRecipeDataObj.Inst.m_nRecipeSpareAttribute5 = 4;
	eips_vs_CurrentRecipeDataObj.Inst.m_nRecipeSpareAttribute6 = 5;
	eips_vs_CurrentRecipeDataObj.Inst.m_nRecipeSpareAttribute7 = 6;

}


void InitEIPSProductionDataObj (void )
{
	eips_vs_ProductionDataObj.Class.ClassRev = 1;

//Common Production Summary data

	eips_vs_ProductionDataObj.Inst.m_fActualThroughput = 33.45f;
	eips_vs_ProductionDataObj.Inst.m_fActualWPL = 44.45f;
	eips_vs_ProductionDataObj.Inst.m_fActualWPA = 55.45f;

	eips_vs_ProductionDataObj.Inst.m_fActualOrderWeight = 556.78f;
	eips_vs_ProductionDataObj.Inst.m_fActualLineSpeed = 29.18f;

	eips_vs_ProductionDataObj.Inst.m_nOperationalStatus = 0x2570;
	eips_vs_ProductionDataObj.Inst.m_nOperationalStatus2 = 0x1872;
	eips_vs_ProductionDataObj.Inst.m_nProcessAlarms = 0x0319;

	eips_vs_ProductionDataObj.Inst.m_nMotorDA = 3976;

	eips_vs_ProductionDataObj.Inst.m_nBatchCounter = 987;

//Component Data.

	eips_vs_ProductionDataObj.Inst.m_fActualPercentage1 = 50.0f;
	eips_vs_ProductionDataObj.Inst.m_fActualPercentage2 = 20.0f;
	eips_vs_ProductionDataObj.Inst.m_fActualPercentage3 = 15.0f;
	eips_vs_ProductionDataObj.Inst.m_fActualPercentage4 = 5.0f;

	eips_vs_ProductionDataObj.Inst.m_fActualPercentage5 = 1.0f;
	eips_vs_ProductionDataObj.Inst.m_fActualPercentage6 = 1.0f;
	eips_vs_ProductionDataObj.Inst.m_fActualPercentage7 = 1.0f;
	eips_vs_ProductionDataObj.Inst.m_fActualPercentage8 = 1.0f;
	eips_vs_ProductionDataObj.Inst.m_fActualPercentage9 = 1.0f;
	eips_vs_ProductionDataObj.Inst.m_fActualPercentage10 = 1.0f;
	eips_vs_ProductionDataObj.Inst.m_fActualPercentage11 = 1.4f;
	eips_vs_ProductionDataObj.Inst.m_fActualPercentage12 = 1.6f;

// actual weights in batch.
	eips_vs_ProductionDataObj.Inst.m_fActualWeight1 = 1.01f;
	eips_vs_ProductionDataObj.Inst.m_fActualWeight2 = 1.02f;
	eips_vs_ProductionDataObj.Inst.m_fActualWeight3 = 1.03f;
	eips_vs_ProductionDataObj.Inst.m_fActualWeight4 = 1.04f;

	eips_vs_ProductionDataObj.Inst.m_fActualWeight5 = 1.05f;
	eips_vs_ProductionDataObj.Inst.m_fActualWeight6 = 1.06f;
	eips_vs_ProductionDataObj.Inst.m_fActualWeight7 = 1.07f;
	eips_vs_ProductionDataObj.Inst.m_fActualWeight8 = 1.08f;
	eips_vs_ProductionDataObj.Inst.m_fActualWeight9 = 1.09f;
	eips_vs_ProductionDataObj.Inst.m_fActualWeight10 = 0.10f;
	eips_vs_ProductionDataObj.Inst.m_fActualWeight11 = 0.11f;
	eips_vs_ProductionDataObj.Inst.m_fActualWeight12 = 0.12f;

// actual order weights.
	eips_vs_ProductionDataObj.Inst.m_fActualOrderWeight1 = 257.01f;
	eips_vs_ProductionDataObj.Inst.m_fActualOrderWeight2 = 257.02f;
	eips_vs_ProductionDataObj.Inst.m_fActualOrderWeight3 = 257.03f;
	eips_vs_ProductionDataObj.Inst.m_fActualOrderWeight4 = 257.04f;

	eips_vs_ProductionDataObj.Inst.m_fActualOrderWeight5 = 257.05f;
	eips_vs_ProductionDataObj.Inst.m_fActualOrderWeight6 = 257.06f;
	eips_vs_ProductionDataObj.Inst.m_fActualOrderWeight7 = 257.07f;
	eips_vs_ProductionDataObj.Inst.m_fActualOrderWeight8 = 257.08f;
	eips_vs_ProductionDataObj.Inst.m_fActualOrderWeight9 = 257.09f;
	eips_vs_ProductionDataObj.Inst.m_fActualOrderWeight10 = 257.10f;
	eips_vs_ProductionDataObj.Inst.m_fActualOrderWeight11 = 257.11f;
	eips_vs_ProductionDataObj.Inst.m_fActualOrderWeight12 = 257.12f;


}


void InitEIPSControlDataObj(void )
{
	eips_vs_ControlDataObj.Class.ClassRev = 1;



	eips_vs_ControlDataObj.Inst.m_nSetIncrease = 0x0000;
	eips_vs_ControlDataObj.Inst.m_nSetDecrease = 0x0000;
	eips_vs_ControlDataObj.Inst.m_nSetAutoManual = 0x0000;
	eips_vs_ControlDataObj.Inst.m_nSetExtruderDAC = 0;
	eips_vs_ControlDataObj.Inst.m_fSetCommsScrewSpeed = 0.0f;
	eips_vs_ControlDataObj.Inst.m_fSetCommsLineSpeed = 0.0f;
	eips_vs_ControlDataObj.Inst.m_nStartClean = 0x0000;
	eips_vs_ControlDataObj.Inst.m_nSpareCommand1 = 0x0000;
	eips_vs_ControlDataObj.Inst.m_nSpareCommand2 = 0x0000;
	eips_vs_ControlDataObj.Inst.m_nSpareCommand3 = 0x0000;

}
