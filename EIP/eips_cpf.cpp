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
 *     Module Name: eips_cpf.c
 *          Author: Jamin D. Wendorf (jwendorf@rtaautomation.com)
 *        Language: Ansi C
 * Compile Options: N/A
 * Compile defines: N/A
 *       Libraries: N/A
 *    Link Options: N/A
 *
 * Description.
 * =======================================================================
 * This file contains the definitions needed for parsing EIP Server
 * Common Packet Format messages. This also handles the CIP processing.
 *
 */

/* ---------------------------- */
/* INCLUDE FILES                */
/* ---------------------------- */
#include "eips_system.h"

/* ---------------------------- */
/* FUNCTION PROTOTYPES          */
/* ---------------------------- */
static void eips_local_objModProc (uint8 msg_type);

/* ********************************************** */
/* Local Function Processing (Service Code Level) */
/* ********************************************** */
static void eips_local_getSingle (CPF_MESSAGE_STRUCT *);
static void eips_local_setSingle (CPF_MESSAGE_STRUCT *);

/* Identity Object Functions */
static void eips_local_IDObjInstGetAll (CPF_MESSAGE_STRUCT *);
static void eips_local_IDObjReset      (CPF_MESSAGE_STRUCT *);

/* TCP Object Functions */
static void eips_local_TcpObjInstGetAll (CPF_MESSAGE_STRUCT *);

/* Ethernet Link Object Functions */
static void eips_local_EnetObjInstGetAll(CPF_MESSAGE_STRUCT *);

/* ********************************************** */
/* Local Function Processing (Attribute Level)    */
/* ********************************************** */
/* Identity Object Functions */
static void eips_local_IDObjProdName   (void);

/* Assembly Object Functions */
static void eips_local_GetSingleAsm (uint16 instance, uint8 attribute);
static void eips_local_SetSingleAsm (uint16 instance, uint8 attribute);

/* TCP Object Functions */
static void eips_local_TcpObjStatus     (void);   /* Attr 1 */
static void eips_local_TcpObjCfgCap     (void);   /* Attr 2 */
static void eips_local_TcpObjCfgCtrl    (void);   /* Attr 3 */
static void eips_local_TcpObjPhyLink    (void);   /* Attr 4 */
static void eips_local_TcpObjIntfCfg    (void);   /* Attr 5 */
static void eips_local_TcpObjHostName   (void);   /* Attr 6 */

/* Ethernet Link Object Functions */
static void eips_local_EnetObjIntfSpeed (void);   /* Attr 1 */
static void eips_local_EnetObjIntfFlags (void);   /* Attr 2 */
static void eips_local_EnetObjPhyAddr   (void);   /* Attr 3 */
static void eips_local_EnetObjIntfCnts  (void);   /* Attr 4 */
static void eips_local_EnetObjMediaCnts (void);   /* Attr 5 */
static void eips_local_EnetObjIntfCtrl  (void);   /* Attr 6 */

/* build error message */
static void eips_local_cpf_buildError (CPF_MESSAGE_STRUCT *cpf, uint8 gen_stat, uint8 ext_size, uint16 ext_err);

/* ---------------------------- */
/* STATIC VARIABLES		*/
/* ---------------------------- */
/*static*/ CPF_MESSAGE_STRUCT           eips_stc_cpf;
/*static*/ EXPLICIT_REQUEST_STRUCT      eips_stc_emreq;
/*static*/ EXPLICIT_RESPONSE_STRUCT     eips_stc_emrsp;

/* ---------------------------- */
/*      GLOBAL VARIABLES        */
/* ---------------------------- */
EIPS_IDENTITY_OBJ_STRUCT    eips_IDObj;
EIPS_TCP_OBJ_STRUCT         eips_TcpObj;
EIPS_ELINK_OBJ_STRUCT       eips_EnetObj;

#if EIPS_USEROBJ_ASM_MAXNUM_O2TINST > 0
    EIPS_ASSEMBLY_INST_STRUCT   eips_AsmO2TObj[EIPS_USEROBJ_ASM_MAXNUM_O2TINST];
#endif

EIPS_ASSEMBLY_INST_STRUCT   eips_AsmT2OObj[EIPS_USEROBJ_ASM_MAXNUM_T2OINST];

/* ---------------------------- */
/* MISCELLANEOUS		*/
/* ---------------------------- */
#define EIPS_MSGTYP_UCMM 0
#define EIPS_MSGTYP_EM	 1

#define SERVCODEIX_GETATTRALL   0
#define SERVCODEIX_RESET        1
#define SERVCODEIX_GETSINGLE    2
#define SERVCODEIX_SETSINGLE    3
#define SERVCODEIX_EXECPCCC     4     
#define SERVCODEIX_FWDOPEN      5
#define SERVCODEIX_FWDCLOSE     6
#define SERVCODEIX_MAX          7

/* Only parse the message once */
static struct
{
    uint8   ServCode;
    uint16  ServCodeIx;
    uint16  ClassID;
    uint8   ClassID_Found;
    uint16  InstID;
    uint8   InstID_Found;
    uint8   AttrID;
    uint8   AttrID_Found;    
}CIA_Struct;

/*      
    All objects (except the assembly) must have entries in this table for every instance.  
    If the service code isn't supported, enter a NULL pointer in the function pointer field. 
*/

typedef struct
{
    uint8 ClassID;
    uint8 InstID;
    void (* FnPtr[SERVCODEIX_MAX]) (CPF_MESSAGE_STRUCT *);
}EIPS_CPF_SERVCODE_LOOKUP_STRUCT;

#define NULL_CPF_FUNCPTR    NULL

EIPS_CODESPACE EIPS_CPF_SERVCODE_LOOKUP_STRUCT EIPS_ServCode_Lookup[] =
{
/* ************************************************************************************************************************************************************************************ */
/*                      GetAttributeAll                 Reset                   GetAttributeSingle      SetAttributeSingle      ExecutePCCC         ForwardOpen         ForwardClose    */
/* Class         Inst   0x01                            0x05                    0x0e                    0x10                    0x4B                0x54                0x4e            */
/* ************************************************************************************************************************************************************************************ */
{CLASS_IDENTITY,    0,  {NULL_CPF_FUNCPTR,              NULL_CPF_FUNCPTR,       eips_local_getSingle,   NULL_CPF_FUNCPTR,       NULL_CPF_FUNCPTR,   NULL_CPF_FUNCPTR,   NULL_CPF_FUNCPTR }},
{CLASS_IDENTITY,    1,  {eips_local_IDObjInstGetAll,    eips_local_IDObjReset,  eips_local_getSingle,   NULL_CPF_FUNCPTR,       NULL_CPF_FUNCPTR,   NULL_CPF_FUNCPTR,   NULL_CPF_FUNCPTR }},
{CLASS_MSGROUTER,   0,  {NULL_CPF_FUNCPTR,              NULL_CPF_FUNCPTR,       NULL_CPF_FUNCPTR,       NULL_CPF_FUNCPTR,       NULL_CPF_FUNCPTR,   NULL_CPF_FUNCPTR,   NULL_CPF_FUNCPTR }},
{CLASS_MSGROUTER,   1,  {NULL_CPF_FUNCPTR,              NULL_CPF_FUNCPTR,       NULL_CPF_FUNCPTR,       NULL_CPF_FUNCPTR,       NULL_CPF_FUNCPTR,   NULL_CPF_FUNCPTR,   NULL_CPF_FUNCPTR }},
{CLASS_CONNECTMGR,  0,  {NULL_CPF_FUNCPTR,              NULL_CPF_FUNCPTR,       NULL_CPF_FUNCPTR,       NULL_CPF_FUNCPTR,       NULL_CPF_FUNCPTR,   NULL_CPF_FUNCPTR,   NULL_CPF_FUNCPTR }},
{CLASS_CONNECTMGR,  1,  {NULL_CPF_FUNCPTR,              NULL_CPF_FUNCPTR,       NULL_CPF_FUNCPTR,       NULL_CPF_FUNCPTR,       NULL_CPF_FUNCPTR,   eips_cnxn_fwdOpen,  eips_cnxn_fwdClose }},
{CLASS_TCPIP,       0,  {NULL_CPF_FUNCPTR,              NULL_CPF_FUNCPTR,       eips_local_getSingle,   NULL_CPF_FUNCPTR,       NULL_CPF_FUNCPTR,   NULL_CPF_FUNCPTR,   NULL_CPF_FUNCPTR }},
{CLASS_TCPIP,       1,  {eips_local_TcpObjInstGetAll,   NULL_CPF_FUNCPTR,       eips_local_getSingle,   eips_local_setSingle,   NULL_CPF_FUNCPTR,   NULL_CPF_FUNCPTR,   NULL_CPF_FUNCPTR }},
{CLASS_ENETLINK,    0,  {NULL_CPF_FUNCPTR,              NULL_CPF_FUNCPTR,       eips_local_getSingle,   NULL_CPF_FUNCPTR,       NULL_CPF_FUNCPTR,   NULL_CPF_FUNCPTR,   NULL_CPF_FUNCPTR }},
{CLASS_ENETLINK,    1,  {eips_local_EnetObjInstGetAll,  NULL_CPF_FUNCPTR,       eips_local_getSingle,   eips_local_setSingle,   NULL_CPF_FUNCPTR,   NULL_CPF_FUNCPTR,   NULL_CPF_FUNCPTR }},
#ifdef EIPS_PCCC_USED
{CLASS_PCCC,        0,  {NULL_CPF_FUNCPTR,              NULL_CPF_FUNCPTR,       NULL_CPF_FUNCPTR,       NULL_CPF_FUNCPTR,       NULL_CPF_FUNCPTR,   NULL_CPF_FUNCPTR,   NULL_CPF_FUNCPTR }},
{CLASS_PCCC,        1,  {NULL_CPF_FUNCPTR,              NULL_CPF_FUNCPTR,       NULL_CPF_FUNCPTR,       NULL_CPF_FUNCPTR,       eips_pccc_execute,  NULL_CPF_FUNCPTR,   NULL_CPF_FUNCPTR }},
#endif
};
#define EIS_SERVCODE_LOOKUP_SIZE    (sizeof(EIPS_ServCode_Lookup)/sizeof(EIPS_CPF_SERVCODE_LOOKUP_STRUCT))

/* ----------------------------------------------------------------------------
  *********** VERY IMPORTANT NOTE ***********

  The classes MUST be in numerical order, the instances within the class MUST
  be in numerical order and the attribute numbers within the class MUST be in
  numerical order. A data type of "EIPS_TYPE_UNUSED" tells the parser to go
  right to the function pointer.
  -----------------------------------------------------------------------------
*/
EIPS_CODESPACE EIPS_GETSETATTR_TABLE_STRUCT EIPSGetSetAttrTable[] =
{
/* 0 - 8 "IDENTITY CLASS" */
/* ********************************************************************************************************** */
/*    SrvCdMsk		 Class	   Inst Attr	 Type	       Data Ptr 			   FnPtr      */
/* ********************************************************************************************************** */
{EIPS_SCMASK_GETSNG, CLASS_IDENTITY, 0, 1, EIPS_TYPE_UINT16, (void*)&eips_IDObj.Class.ClassRev,   NULLPTR},
{EIPS_SCMASK_GETSNG, CLASS_IDENTITY, 0, 2, EIPS_TYPE_UINT16, (void*)&eips_IDObj.Class.MaxInst,    NULLPTR},
{EIPS_SCMASK_GETSNG, CLASS_IDENTITY, 1, 1, EIPS_TYPE_UINT16, (void*)&eips_IDObj.Inst.Vendor,      NULLPTR},
{EIPS_SCMASK_GETSNG, CLASS_IDENTITY, 1, 2, EIPS_TYPE_UINT16, (void*)&eips_IDObj.Inst.DeviceType,  NULLPTR},
{EIPS_SCMASK_GETSNG, CLASS_IDENTITY, 1, 3, EIPS_TYPE_UINT16, (void*)&eips_IDObj.Inst.ProductCode, NULLPTR},
{EIPS_SCMASK_GETSNG, CLASS_IDENTITY, 1, 4, EIPS_TYPE_UINT16, (void*)&eips_IDObj.Inst.Revision,    NULLPTR},
{EIPS_SCMASK_GETSNG, CLASS_IDENTITY, 1, 5, EIPS_TYPE_UINT16, (void*)&eips_IDObj.Inst.Status,      NULLPTR},
{EIPS_SCMASK_GETSNG, CLASS_IDENTITY, 1, 6, EIPS_TYPE_UINT32, (void*)&eips_IDObj.Inst.SerialNum,   NULLPTR},
{EIPS_SCMASK_GETSNG, CLASS_IDENTITY, 1, 7, EIPS_TYPE_UNUSED, (void*)NULLPTR,     eips_local_IDObjProdName},

/* ********************************************* */
/* NO ATTRIBUTES FOR: "MESSAGE ROUTER CLASS"     */
/* "ASSEMBLY CLASS" IS HANDLED UNIQUELY          */
/* NO ATTRIBUTES FOR: "CONNECTION MANAGER CLASS" */
/* ********************************************* */

/* 9 - 15 "TCP/IP CLASS" */
/* ************************************************************************************************ */
/*    SrvCdMsk	       Class    Inst Attr    Type           Data Ptr                        FnPtr   */
/* ************************************************************************************************ */
{EIPS_SCMASK_GETSNG, CLASS_TCPIP, 0, 1, EIPS_TYPE_UINT16, (void*)&eips_TcpObj.Class.ClassRev, NULLPTR},
{EIPS_SCMASK_SETSNG, CLASS_TCPIP, 1, 1, EIPS_TYPE_UNUSED, (void*)NULLPTR,     eips_local_TcpObjStatus}, /* FnPtr Determines Get/Set */
{EIPS_SCMASK_SETSNG, CLASS_TCPIP, 1, 2, EIPS_TYPE_UNUSED, (void*)NULLPTR,     eips_local_TcpObjCfgCap}, /* FnPtr Determines Get/Set */
{EIPS_SCMASK_SETSNG, CLASS_TCPIP, 1, 3, EIPS_TYPE_UNUSED, (void*)NULLPTR,    eips_local_TcpObjCfgCtrl}, /* FnPtr Determines Get/Set */
{EIPS_SCMASK_SETSNG, CLASS_TCPIP, 1, 4, EIPS_TYPE_UNUSED, (void*)NULLPTR,	 eips_local_TcpObjPhyLink}, /* FnPtr Determines Get/Set */
{EIPS_SCMASK_SETSNG, CLASS_TCPIP, 1, 5, EIPS_TYPE_UNUSED, (void*)NULLPTR,	 eips_local_TcpObjIntfCfg}, /* FnPtr Determines Get/Set */
{EIPS_SCMASK_SETSNG, CLASS_TCPIP, 1, 6, EIPS_TYPE_UNUSED, (void*)NULLPTR,   eips_local_TcpObjHostName}, /* FnPtr Determines Get/Set */

/* 16 - 22 "ETHERNET LINK CLASS" */
/* **************************************************************************************************** */
/*    SrvCdMsk	       Class	   Inst Attr    Type            Data Ptr                        FnPtr   */
/* **************************************************************************************************** */
{EIPS_SCMASK_GETSNG, CLASS_ENETLINK, 0, 1, EIPS_TYPE_UINT16, (void*)&eips_EnetObj.Class.ClassRev, NULLPTR},
{EIPS_SCMASK_SETSNG, CLASS_ENETLINK, 1, 1, EIPS_TYPE_UNUSED, (void*)NULLPTR,  eips_local_EnetObjIntfSpeed}, /* FnPtr Determines Get/Set */
{EIPS_SCMASK_SETSNG, CLASS_ENETLINK, 1, 2, EIPS_TYPE_UNUSED, (void*)NULLPTR,  eips_local_EnetObjIntfFlags}, /* FnPtr Determines Get/Set */
{EIPS_SCMASK_SETSNG, CLASS_ENETLINK, 1, 3, EIPS_TYPE_UNUSED, (void*)NULLPTR,    eips_local_EnetObjPhyAddr}, /* FnPtr Determines Get/Set */
{EIPS_SCMASK_SETSNG, CLASS_ENETLINK, 1, 4, EIPS_TYPE_UNUSED, (void*)NULLPTR,   eips_local_EnetObjIntfCnts}, /* FnPtr Determines Get/Set */
{EIPS_SCMASK_SETSNG, CLASS_ENETLINK, 1, 5, EIPS_TYPE_UNUSED, (void*)NULLPTR,  eips_local_EnetObjMediaCnts}, /* FnPtr Determines Get/Set */
{EIPS_SCMASK_SETSNG, CLASS_ENETLINK, 1, 6, EIPS_TYPE_UNUSED, (void*)NULLPTR,   eips_local_EnetObjIntfCtrl}  /* FnPtr Determines Get/Set */
};
#define EIPS_GETSETATTRTAB_SIZE (sizeof(EIPSGetSetAttrTable)/sizeof(EIPS_GETSETATTR_TABLE_STRUCT))

/**/
/* ******************************************************************** */
/*			GLOBAL FUNCTIONS				*/
/* ******************************************************************** */
/* ====================================================================
Function:	eips_cpf_init
Parameters:	init type
Returns:	N/A

This function initializes static variables used by this file.
======================================================================= */
void eips_cpf_init (uint8 init_type)
{
    int16 i, temp_size;

#if EIPS_USEROBJ_ASM_MAXNUM_O2TINST > 0
    uint16 o2t_list[EIPS_USEROBJ_ASM_MAXNUM_O2TINST];
#endif
    uint16 t2o_list[EIPS_USEROBJ_ASM_MAXNUM_T2OINST];

    /* make sure 1 assembly type is selected */
    #if EIPS_USEROBJ_ASM_ELEMENT_SIZE != 1 && EIPS_USEROBJ_ASM_ELEMENT_SIZE != 2 && EIPS_USEROBJ_ASM_ELEMENT_SIZE !=4
        #error "EIPS_USEROBJ_ASM_ELEMENT_SIZE defined incorrectly in eips_userobj.h"
    #endif

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

    /* Initialize Identity Object Attributes */
    eips_IDObj.Class.ClassRev = EIPS_IDENTITY_CLASS_REV;
    eips_IDObj.Class.MaxInst = 1;
    eips_IDObj.Inst.Vendor = EIPS_USER_IDOBJ_VENDORID;
    eips_IDObj.Inst.DeviceType = EIPS_USER_IDOBJ_DEVTYPE;
    eips_IDObj.Inst.ProductCode = EIPS_USER_IDOBJ_PRODCODE;
    eips_IDObj.Inst.Revision = EIPS_REVISION;
    eips_IDObj.Inst.Status = 0;
    eips_IDObj.Inst.SerialNum = eips_userobj_getSerialNumber();
    eips_IDObj.Inst.State = 0xFF; /* default defined by CIP */

/*  DYNAMIC - SET BY USER!!!! */
/*  rta_ByteMove((uint8*)eips_IDObj.Inst.ProductName, (uint8*)EIPS_USER_IDOBJ_NAME, EIPS_USER_IDOBJ_NAME_SIZE);  */

#if EIPS_USEROBJ_ASM_MAXNUM_O2TINST > 0
    /* Initialize O_to_T Assembly Info */
    memset(eips_AsmO2TObj, 0, sizeof(eips_AsmO2TObj));
    memset(o2t_list, 0, sizeof(o2t_list));
    temp_size = eips_userobj_asmGetO2TInstList(o2t_list, EIPS_USEROBJ_ASM_MAXNUM_O2TINST);
    for(i=0; i<temp_size && i<EIPS_USEROBJ_ASM_MAXNUM_O2TINST; i++)
    {
        /* store the instance */
        eips_AsmO2TObj[i].Inst = o2t_list[i];

        /* validate the instance is non-zero */
        if(eips_AsmO2TObj[i].Inst != 0)
        {
            /* get the data pointer */
            eips_AsmO2TObj[i].DataPtr = eips_userobj_getAsmPtr ((uint16) eips_AsmO2TObj[i].Inst, (uint16 *)&eips_AsmO2TObj[i].DataSize);

            /* verify the size is non-zero */
            if((eips_AsmO2TObj[i].DataSize) && (eips_AsmO2TObj[i].DataPtr == NULL))
            {
                /* if we expect a pointer and receive NULL we have a fatal error */
                eips_usersys_fatalError ("eips_cpf_init (o2t)", eips_AsmO2TObj[i].Inst);
            }
        }
    }
#endif

    /* Initialize T_to_O Assembly Info */
    memset(eips_AsmT2OObj, 0, sizeof(eips_AsmT2OObj));
    memset(t2o_list, 0, sizeof(t2o_list));
    temp_size = eips_userobj_asmGetT2OInstList(t2o_list, EIPS_USEROBJ_ASM_MAXNUM_T2OINST);
    for(i=0; i<EIPS_USEROBJ_ASM_MAXNUM_T2OINST; i++)
    {
        /* store the instance */
        eips_AsmT2OObj[i].Inst = t2o_list[i];

        /* validate the instance is non-zero */
        if(eips_AsmT2OObj[i].Inst != 0)
        {
            /* get the data pointer */
            eips_AsmT2OObj[i].DataPtr = eips_userobj_getAsmPtr ((uint16) eips_AsmT2OObj[i].Inst, (uint16 *)&eips_AsmT2OObj[i].DataSize);

            /* verify the size is non-zero */
            if((eips_AsmT2OObj[i].DataSize) && (eips_AsmT2OObj[i].DataPtr == NULL))
            {
                /* if we expect a pointer and receive NULL we have a fatal error */
                eips_usersys_fatalError ("eips_cpf_init (t2o)", eips_AsmT2OObj[i].Inst);
            }
        }
    }

#ifdef EIPS_PRINTALL

    #ifdef EIPS_USEROBJ_ASM_LISTENONLY_HB_O2TINST
            eips_user_dbprint2("(eips) Listen Only Heartbeat Inst 0x%04x (%d)\r\n", EIPS_USEROBJ_ASM_LISTENONLY_HB_O2TINST, EIPS_USEROBJ_ASM_LISTENONLY_HB_O2TINST);
    #else
            eips_user_dbprint0("(eips) Listen Only Heartbeat Inst Not Supported\r\n");
    #endif

    #ifdef EIPS_USEROBJ_ASM_INPUTONLY_HB_O2TINST
            eips_user_dbprint2("(eips) Input Only Heartbeat Inst 0x%04x (%d)\r\n", EIPS_USEROBJ_ASM_INPUTONLY_HB_O2TINST, EIPS_USEROBJ_ASM_INPUTONLY_HB_O2TINST);
    #else
            eips_user_dbprint0("(eips) Input Only Heartbeat Inst Not Supported\r\n");
    #endif

    #if EIPS_USEROBJ_ASM_MAXNUM_O2TINST > 0  
        #if EIPS_USEROBJ_ASM_ELEMENT_SIZE == 4      /* DWORDS */
            /* Print the Asm Info */
            for(i=0; i<EIPS_USEROBJ_ASM_MAXNUM_O2TINST; i++)
            {
                eips_user_dbprint5("(eips) O_to_T[%d] Inst 0x%04x (%d) Size %d dwords (%d bytes)\r\n",i, eips_AsmO2TObj[i].Inst, eips_AsmO2TObj[i].Inst, eips_AsmO2TObj[i].DataSize, (eips_AsmO2TObj[i].DataSize*4));
            }
        #elif EIPS_USEROBJ_ASM_ELEMENT_SIZE == 2    /* WORDS */
            /* Print the Asm Info */
            for(i=0; i<EIPS_USEROBJ_ASM_MAXNUM_O2TINST; i++)
            {
                eips_user_dbprint5("(eips) O_to_T[%d] Inst 0x%04x (%d) Size %d words (%d bytes)\r\n",i, eips_AsmO2TObj[i].Inst, eips_AsmO2TObj[i].Inst, eips_AsmO2TObj[i].DataSize, (eips_AsmO2TObj[i].DataSize*2));
            }
        #else
            /* Print the Asm Info */
            for(i=0; i<EIPS_USEROBJ_ASM_MAXNUM_O2TINST; i++)
            {
                eips_user_dbprint4("(eips) O_to_T[%d] Inst 0x%04x (%d) Size %d bytes\r\n",i, eips_AsmO2TObj[i].Inst, eips_AsmO2TObj[i].Inst, eips_AsmO2TObj[i].DataSize);
            }
        #endif
    #endif

    #if EIPS_USEROBJ_ASM_ELEMENT_SIZE == 4      /* DWORDS */
        /* Print the Asm Info */
        for(i=0; i<EIPS_USEROBJ_ASM_MAXNUM_T2OINST; i++)
        {
            eips_user_dbprint5("(eips) T_to_O[%d] Inst 0x%04x (%d) Size %d dwords (%d bytes)\r\n",i, eips_AsmT2OObj[i].Inst, eips_AsmT2OObj[i].Inst, eips_AsmT2OObj[i].DataSize, (eips_AsmT2OObj[i].DataSize*4));
        }
    #elif EIPS_USEROBJ_ASM_ELEMENT_SIZE == 2    /* WORDS */
        /* Print the Asm Info */
        for(i=0; i<EIPS_USEROBJ_ASM_MAXNUM_T2OINST; i++)
        {
            eips_user_dbprint5("(eips) T_to_O[%d] Inst 0x%04x (%d) Size %d words (%d bytes)\r\n",i, eips_AsmT2OObj[i].Inst, eips_AsmT2OObj[i].Inst, eips_AsmT2OObj[i].DataSize, (eips_AsmT2OObj[i].DataSize*2));
        }
    #else
        /* Print the Asm Info */
        for(i=0; i<EIPS_USEROBJ_ASM_MAXNUM_T2OINST; i++)
        {
            eips_user_dbprint4("(eips) T_to_O[%d] Inst 0x%04x (%d) Size %d bytes\r\n",i, eips_AsmT2OObj[i].Inst, eips_AsmT2OObj[i].Inst, eips_AsmT2OObj[i].DataSize);
        }                                       /* BYTES */
    #endif

#endif /* #ifdef EIPS_PRINTALL */

    /* Initialize TCP Object Attributes */
    eips_TcpObj.Class.ClassRev = EIPS_TCP_CLASS_REV;
    /* all instance level attributes are handled by the user */

    /* Initialize Ethernet Link Object Attributes */
    eips_EnetObj.Class.ClassRev = EIPS_ENETLINK_CLASS_REV;
    /* all instance level attributes are handled by the user */

    /* set up the cpf structure */
    eips_stc_cpf.emreq = &eips_stc_emreq;
    eips_stc_cpf.emrsp = &eips_stc_emrsp;
}

/* ====================================================================
Function:	eips_cpf_ProcessCPF
Parameters: Encapsulated message structure
            socket structure pointer
Returns:	N/A

This function parses the CIP message, processes the req_data and builds
a rsp_data if needed.

The message that is passed to this function has the following format:
   Interface Handle (4 Bytes)
   Timeout (2 Bytes)
   CPF (Remainder of the bytes)
======================================================================= */
void eips_cpf_ProcessCPF(EIPS_ENCAP_MSG_STRUCT *EncapReq, EIPS_SOCKET_STRUCT *sock)
{
    int16 i, j, data_ix, cnxn_ix;
    uint8 *data_ptr;
    uint8 resp_msg[EIPS_CPF_MAX_RSP_SIZ];
    int16 resp_msg_siz;
    EIPS_SESSION_STRUCT *session;

#ifdef EIPS_PCCC_USED
    uint16 df1_resp_size;
#endif

    /* store the socket id */
    eips_stc_cpf.sock_id = sock->sock_id;

    /* set the cpf encap pointer to EncapReq */
    eips_stc_cpf.encap = EncapReq;

    /* initialize the rsp_data message structure on every call to this function */
    eips_stc_cpf.emrsp->reserved = 0;
    eips_stc_cpf.emrsp->user_struct.gen_stat = 0;
    eips_stc_cpf.emrsp->user_struct.ext_stat_size = 0;
    eips_stc_cpf.emrsp->user_struct.rsp_data_size = 0;

    /* store the Encapsulated data length and data pointer */
    eips_stc_cpf.bytes_used = 0;
    eips_stc_cpf.encap_data = EncapReq->EncapsulatedData;

    /* Get Interface Handle */
    eips_stc_cpf.InterfaceHandle = rta_GetLitEndian32(eips_stc_cpf.encap_data);
    eips_stc_cpf.encap_data+=4;	 /* move data pointer by data copied length */
    eips_stc_cpf.bytes_used += 4; /* incr the count (validate data prior to use) */

    /* Get Timeout */
    eips_stc_cpf.Timeout = rta_GetLitEndian16(eips_stc_cpf.encap_data);
    eips_stc_cpf.encap_data+=2;	 /* move data pointer by data copied length */
    eips_stc_cpf.bytes_used += 2; /* incr the count (validate data prior to use) */

    /* Validate the number of items */
    eips_stc_cpf.ItemCount = rta_GetLitEndian16(eips_stc_cpf.encap_data);
    eips_stc_cpf.encap_data+=2;	 /* move data pointer by data copied length */
    eips_stc_cpf.bytes_used += 2; /* incr the count (validate data prior to use) */

    /* Error: Too Many Items */
    if(eips_stc_cpf.ItemCount > EIPS_CPF_MAX_ITEM_CNT)
    {
        EncapReq->Status = EIP_ENCAPSTATUSERR_FORMAT;
        eips_encap_unSupEncapMsg(EncapReq, sock);
        return; /* Stop this function on an error */
    }

    /* Parse the CPF message into items */
    for(i=0; i<eips_stc_cpf.ItemCount; i++)
    {
        /* store the type */
        eips_stc_cpf.Items[i].TypeId = rta_GetLitEndian16(eips_stc_cpf.encap_data);
        eips_stc_cpf.encap_data+=2;   /* move data pointer by data copied length */
        eips_stc_cpf.bytes_used += 2; /* incr the count (validate data prior to use) */

        /* store the length */
        eips_stc_cpf.Items[i].Length = rta_GetLitEndian16(eips_stc_cpf.encap_data);
        eips_stc_cpf.encap_data+=2;   /* move data pointer by data copied length */
        eips_stc_cpf.bytes_used += 2; /* incr the count (validate data prior to use) */

        /* validate there is enough room for data */
        if(eips_stc_cpf.Items[i].Length > EIPS_CPF_MAX_ITEM_DATA_LEN)
        {
	        EncapReq->Status = EIP_ENCAPSTATUSERR_FORMAT;
	        eips_encap_unSupEncapMsg(EncapReq, sock);
	        return; /* Stop this function on an error */
        }

        /* store the data */
        rta_ByteMove(eips_stc_cpf.Items[i].Data, eips_stc_cpf.encap_data, eips_stc_cpf.Items[i].Length);
        eips_stc_cpf.encap_data += eips_stc_cpf.Items[i].Length; /* move data pointer by data copied length */
        eips_stc_cpf.bytes_used = (uint16)(eips_stc_cpf.bytes_used + eips_stc_cpf.Items[i].Length); /* incr the count (validate data prior to use) */
    }

    /* validate the buffer had the exact message prior to use */
    if(eips_stc_cpf.bytes_used != EncapReq->Length)
    {
        EncapReq->Status = EIP_ENCAPSTATUSERR_FORMAT;
        eips_encap_unSupEncapMsg(EncapReq, sock);
        return; /* Stop this function on an error */
    }

#ifdef EIPS_PCCC_USED
    /* ***************************************** */
    /*    Encap Command: EIP_ENCAPCMD_SENDRRDATA */
    /*   Addr Item Type: ADDR_TYP_ID_NULL */
    /* Addr Item Length: Varies (>0) */
    /*   Data Item Type: DATA_TYP_ID_UCMM */
    /* Data Item Length: Varies (>0) */
    /*      Description: UCMM Unconnected */
    /* ***************************************** */
    if( (EncapReq->Command == EIP_ENCAPCMD_SENDRRDATA) &&
        (eips_stc_cpf.Items[EIPS_CPF_ADDR_ITEM].TypeId == ADDR_TYP_ID_ENBT) &&
        (eips_stc_cpf.Items[EIPS_CPF_ADDR_ITEM].Length > 0) &&
        (eips_stc_cpf.Items[EIPS_CPF_DATA_ITEM].TypeId == DATA_TYP_ID_ENBT) &&
        (eips_stc_cpf.Items[EIPS_CPF_DATA_ITEM].Length > 0) )
    {
        /* ************************** */
        /* Build the Response Message */
        /* ************************** */

        /* store the Encapsulated header */
        rta_PutLitEndian16(EncapReq->Command, resp_msg+0);
        /* "EncapReq->Length" is stored later */
        rta_PutLitEndian32(EncapReq->SessionHandle, resp_msg+4);
        rta_PutLitEndian32(EncapReq->Status, resp_msg+8);
        for(i=0; i<8; i++)
            resp_msg[12+i] = EncapReq->SenderContext[i];
        rta_PutLitEndian32(EncapReq->Options, resp_msg+20);

        /* store the CPF portion of the message */
        data_ptr = resp_msg + EIP_ENCAP_HEADER_SIZ;
        resp_msg_siz = 0;

        /* store the Interface Handle (always 0) */
        rta_PutLitEndian32(0, data_ptr+resp_msg_siz);
        resp_msg_siz += 4;

        /* store the Timeout (always 0) */
        rta_PutLitEndian16(0, data_ptr+resp_msg_siz);
        resp_msg_siz += 2;

        /* store the Item Count (always 2) */
        rta_PutLitEndian16(2, data_ptr+resp_msg_siz);
        resp_msg_siz += 2;

        /* ****************************************** */
        /* store the Address Item (ECHO FROM REQUEST) */
        /* ****************************************** */
        /* store the Item Code */
        rta_PutLitEndian16(eips_stc_cpf.Items[EIPS_CPF_ADDR_ITEM].TypeId, data_ptr+resp_msg_siz);
        resp_msg_siz += 2;

        /* store the Item Length */
        rta_PutLitEndian16(eips_stc_cpf.Items[EIPS_CPF_ADDR_ITEM].Length, data_ptr+resp_msg_siz);
        resp_msg_siz += 2;

        /* store the Item Data */
        for(j=0; j<eips_stc_cpf.Items[EIPS_CPF_ADDR_ITEM].Length; j++)
        {
            data_ptr[resp_msg_siz] = eips_stc_cpf.Items[EIPS_CPF_ADDR_ITEM].Data[j];
            resp_msg_siz++;
        }

        /* PASS THE REQUEST TO THE USER */

        /* ******************************* */
        /* store the Data Item (from User) */
        /* ******************************* */

        /* store the Item Code */
        rta_PutLitEndian16(eips_stc_cpf.Items[EIPS_CPF_DATA_ITEM].TypeId, data_ptr+resp_msg_siz);
        resp_msg_siz += 2;

        /* pass to user (this function stores the data) */
        df1_resp_size = eips_userdf1_proc_msg (eips_stc_cpf.Items[EIPS_CPF_DATA_ITEM].Data, (uint16)(eips_stc_cpf.Items[EIPS_CPF_DATA_ITEM].Length), data_ptr+resp_msg_siz+2, (uint16)(sizeof(resp_msg)-(resp_msg_siz+2)));

        /* if the returned length is 0, send the encap error length message */
        if(df1_resp_size == 0)
        {
            EncapReq->Status = EIP_ENCAPSTATUSERR_LENGTH;
            eips_encap_unSupEncapMsg(EncapReq, sock);
            return; /* Stop this function on an error */
        }

        /* send the valid response */
        else
        {
            /* store the Data Length (func rsp code) and data (in function) */
            rta_PutLitEndian16(df1_resp_size, data_ptr+resp_msg_siz);
            resp_msg_siz = (uint16)(resp_msg_siz  + (2+df1_resp_size));

            /* store the length */
            EncapReq->Length = resp_msg_siz;
            rta_PutLitEndian16(EncapReq->Length, resp_msg+2);
            resp_msg_siz = (uint16)(EncapReq->Length+EIP_ENCAP_HEADER_SIZ);

            /* transmit the message */
            eips_usersock_sendTCPData (sock->sock_id, resp_msg, resp_msg_siz);
            return; /* Stop this function on a successful response */
        }
    }
#endif

    /* ***************************************** */
    /*    Encap Command: EIP_ENCAPCMD_SENDRRDATA */
    /*   Addr Item Type: ADDR_TYP_ID_NULL */
    /* Addr Item Length: Always 0 */
    /*   Data Item Type: DATA_TYP_ID_UCMM */
    /* Data Item Length: Varies (>0) */
    /*      Description: UCMM Unconnected */
    /* ***************************************** */
    if( (EncapReq->Command == EIP_ENCAPCMD_SENDRRDATA) &&
        (eips_stc_cpf.Items[EIPS_CPF_ADDR_ITEM].TypeId == ADDR_TYP_ID_NULL) &&
        (eips_stc_cpf.Items[EIPS_CPF_ADDR_ITEM].Length == 0x0000) &&
        (eips_stc_cpf.Items[EIPS_CPF_DATA_ITEM].TypeId == DATA_TYP_ID_UCMM) &&
        (eips_stc_cpf.Items[EIPS_CPF_DATA_ITEM].Length > 0) )
    {
        /* Process UCMM Encap Message */
        data_ix=0; /* keep track of where we are in the data field */

        /* Store the Service Code */
        eips_stc_cpf.emreq->user_struct.service = *(eips_stc_cpf.Items[EIPS_CPF_DATA_ITEM].Data+data_ix);
        eips_stc_cpf.emrsp->service  = eips_stc_cpf.emreq->user_struct.service; /* needed for rsp_data */
        data_ix++; /* advance the data pointer */

        /* Store the Path Size */
        eips_stc_cpf.emreq->user_struct.path_size = *(eips_stc_cpf.Items[EIPS_CPF_DATA_ITEM].Data+data_ix);
        data_ix++; /* advance the data pointer */

        /* Validate the Path Size */
        if(eips_stc_cpf.emreq->user_struct.path_size > EIPS_USER_MSGRTR_PATH_WORDS)
        {
            /* build the error message */
            EncapReq->Status = EIP_ENCAPSTATUSERR_FORMAT;
            eips_encap_unSupEncapMsg(EncapReq, sock);
            return;
        }

        /* Store the Path */
        for(i=0; i<eips_stc_cpf.emreq->user_struct.path_size; i++)
        {
            eips_stc_cpf.emreq->user_struct.path[i] = rta_GetLitEndian16(eips_stc_cpf.Items[EIPS_CPF_DATA_ITEM].Data+data_ix);
            data_ix+=2; /* advance the data pointer */
        }

        /* Store the Data */
        eips_stc_cpf.emreq->user_struct.req_data_size = (uint16)(eips_stc_cpf.Items[EIPS_CPF_DATA_ITEM].Length - data_ix);
        if(eips_stc_cpf.emreq->user_struct.req_data_size <= EIPS_USER_MSGRTR_DATA_SIZE)
        {
            rta_ByteMove( eips_stc_cpf.emreq->user_struct.req_data,
                        ( eips_stc_cpf.Items[EIPS_CPF_DATA_ITEM].Data+data_ix),
		                  eips_stc_cpf.emreq->user_struct.req_data_size);

	        /* process the object model message */
	        eips_local_objModProc(EIPS_MSGTYP_UCMM);

	        /* ************************** */
	        /* Build the Response Message */
	        /* ************************** */

            /* store the Encapsulated header */
            rta_PutLitEndian16(EncapReq->Command, resp_msg+0);
	        /* "EncapReq->Length" is stored later */
	        rta_PutLitEndian32(EncapReq->SessionHandle, resp_msg+4);
	        rta_PutLitEndian32(EncapReq->Status, resp_msg+8);
	        for(i=0; i<8; i++)
                resp_msg[12+i] = EncapReq->SenderContext[i];
	        rta_PutLitEndian32(EncapReq->Options,	     resp_msg+20);

	        /* store the CPF portion of the message */
	        data_ptr = resp_msg + EIP_ENCAP_HEADER_SIZ;
	        resp_msg_siz = 0;

	        /* store the Interface Handle (always 0) */
	        rta_PutLitEndian32(0, data_ptr+resp_msg_siz);
	        resp_msg_siz += 4;

	        /* store the Timeout (always 0) */
	        rta_PutLitEndian16(0, data_ptr+resp_msg_siz);
	        resp_msg_siz += 2;

	        /* store the Item Count */
	        rta_PutLitEndian16(eips_stc_cpf.ItemCount, data_ptr+resp_msg_siz);
	        resp_msg_siz += 2;

	        /* store the Items */
	        for(i=0; i<eips_stc_cpf.ItemCount; i++)
            {
	            /* store the Item Code */
	            rta_PutLitEndian16(eips_stc_cpf.Items[i].TypeId, data_ptr+resp_msg_siz);
	            resp_msg_siz += 2;

	            /* store the Item Length */
	            rta_PutLitEndian16(eips_stc_cpf.Items[i].Length, data_ptr+resp_msg_siz);
	            resp_msg_siz += 2;

	            /* store the Item Data */
	            for(j=0; j<eips_stc_cpf.Items[i].Length; j++)
	            {
	                data_ptr[resp_msg_siz] = eips_stc_cpf.Items[i].Data[j];
	                resp_msg_siz++;
	            }
	        }

	        /* store the length */
	        EncapReq->Length = resp_msg_siz;
	        rta_PutLitEndian16(EncapReq->Length, resp_msg+2);
	        resp_msg_siz = (uint16)(EncapReq->Length+EIP_ENCAP_HEADER_SIZ);

	        /* transmit the message */
	        eips_usersock_sendTCPData (sock->sock_id, resp_msg, resp_msg_siz);
        }

        /* invalid data length */
        else
        {
	        /* build the error message */
	        EncapReq->Status = EIP_ENCAPSTATUSERR_FORMAT;
	        eips_encap_unSupEncapMsg(EncapReq, sock);
	        return;
        }
    }

    /* ******************************************* */
    /*    Encap Command: EIP_ENCAPCMD_SENDUNITDATA */
    /*	 Addr Item Type: ADDR_TYP_ID_CNXNBASED */
    /* Addr Item Length: Always 4 */
    /*   Data Item Type: DATA_TYP_ID_PDU */
    /* Data Item Length: Varies (>0) */
    /*      Description: EM Connected */
    /* ******************************************* */
    else if((EncapReq->Command == EIP_ENCAPCMD_SENDUNITDATA) &&
            (eips_stc_cpf.Items[EIPS_CPF_ADDR_ITEM].TypeId == ADDR_TYP_ID_CNXNBASED) &&
            (eips_stc_cpf.Items[EIPS_CPF_ADDR_ITEM].Length == 0x0004) &&
	        (eips_stc_cpf.Items[EIPS_CPF_DATA_ITEM].TypeId == DATA_TYP_ID_PDU) &&
	        (eips_stc_cpf.Items[EIPS_CPF_DATA_ITEM].Length > 0) )
    {
        /* Store the Connection ID */
        eips_stc_cpf.ConnectionID = rta_GetLitEndian32(eips_stc_cpf.Items[EIPS_CPF_ADDR_ITEM].Data);

        /* Process UCMM Encap Message */
        data_ix=0; /* keep track of where we are in the data field */

        /* Store the Sequence Number */
        eips_stc_cpf.SequenceNumber = rta_GetLitEndian16(eips_stc_cpf.Items[EIPS_CPF_DATA_ITEM].Data+data_ix);
        data_ix+=2; /* advance the data pointer */

        /* Store the Service Code */
        eips_stc_cpf.emreq->user_struct.service = *(eips_stc_cpf.Items[EIPS_CPF_DATA_ITEM].Data+data_ix);
        eips_stc_cpf.emrsp->service  = eips_stc_cpf.emreq->user_struct.service; /* needed for rsp_data */
        data_ix++; /* advance the data pointer */

        /* Store the Path Size */
        eips_stc_cpf.emreq->user_struct.path_size = *(eips_stc_cpf.Items[EIPS_CPF_DATA_ITEM].Data+data_ix);
        data_ix++; /* advance the data pointer */

        /* Validate the Path Size */
        if(eips_stc_cpf.emreq->user_struct.path_size > EIPS_USER_MSGRTR_PATH_WORDS)
        {
            /* build the error message */
            EncapReq->Status = EIP_ENCAPSTATUSERR_FORMAT;
            eips_encap_unSupEncapMsg(EncapReq, sock);
            return;
        }

        /* Store the Path */
        for(i=0; i<eips_stc_cpf.emreq->user_struct.path_size; i++)
        {
            eips_stc_cpf.emreq->user_struct.path[i] = rta_GetLitEndian16(eips_stc_cpf.Items[EIPS_CPF_DATA_ITEM].Data+data_ix);
            data_ix+=2; /* advance the data pointer */
        }

        /* Store the Data */
        eips_stc_cpf.emreq->user_struct.req_data_size = (uint16)(eips_stc_cpf.Items[EIPS_CPF_DATA_ITEM].Length - data_ix);
        if(eips_stc_cpf.emreq->user_struct.req_data_size <= EIPS_USER_MSGRTR_DATA_SIZE)
        {
            /* store the data */
            rta_ByteMove(eips_stc_cpf.emreq->user_struct.req_data,
                        (eips_stc_cpf.Items[EIPS_CPF_DATA_ITEM].Data+data_ix),
                        eips_stc_cpf.emreq->user_struct.req_data_size);

            /* make sure the session exists */
            session = eips_encap_getSessionStruct (EncapReq->SessionHandle);
            if(session == NULL)
            {
                /* we should never get here... this is a fatal error */
/*                eips_usersys_fatalError ("eips_cpf_ProcessCPF", 1); */
                return;
            }

            /* Check if we have a connection that matches the cnxn ID and seq # */
            for(i=0, cnxn_ix = -1; ((i<EIPS_USER_MAX_NUM_EM_CNXNS_PER_SESSION) && (cnxn_ix == -1)); i++)
            {
                /* we have a valid connection */
                if(session->CnxnPtrs[i] != NULLPTR)
                {
                    /* The sequence number should never roll over */
                    if(session->CnxnPtrs[i]->O2T_CID == eips_stc_cpf.ConnectionID)
                    {
                        cnxn_ix = i;
                    }
                }
            }

            /* do nothing if we can't find the connection */
            if(cnxn_ix == -1)
            {
                return;
            }

            /* **************************** */
            /* We found a valid CPF message */
            /* **************************** */
#if 0
            /*  If the sequence count is the same as the previous message, notify the user
                of the duplicate message and return */
            if(eips_stc_cpf.SequenceNumber == session->CnxnPtrs[cnxn_ix]->O2T_SeqNum)
            {
                return;
            }

            /* reset the timer on new messages */
            else
            {
                eips_timer_reset(session->CnxnPtrs[cnxn_ix]->O2T_TimerNum);
            }
#else
            /* reset the timer on new messages */
            eips_timer_reset(session->CnxnPtrs[cnxn_ix]->O2T_TimerNum);
#endif

            /* update the sequence numbers */
            session->CnxnPtrs[cnxn_ix]->O2T_SeqNum = eips_stc_cpf.SequenceNumber;
            session->CnxnPtrs[cnxn_ix]->T2O_SeqNum = eips_stc_cpf.SequenceNumber;

            /* update the Connection ID to the T2O Connection ID */
            rta_PutLitEndian32(session->CnxnPtrs[cnxn_ix]->T2O_CID, eips_stc_cpf.Items[EIPS_CPF_ADDR_ITEM].Data);

            /* process the object model message */
            eips_local_objModProc(EIPS_MSGTYP_EM);

            /* ************************** */
            /* Build the Response Message */
            /* ************************** */

            /* store the Encapsulated header */
            rta_PutLitEndian16(EncapReq->Command, resp_msg+0);
            /* "EncapReq->Length" is stored later */
            rta_PutLitEndian32(EncapReq->SessionHandle, resp_msg+4);
            rta_PutLitEndian32(EncapReq->Status, resp_msg+8);
            for(i=0; i<8; i++)
                resp_msg[12+i] = EncapReq->SenderContext[i];
            rta_PutLitEndian32(EncapReq->Options, resp_msg+20);

            /* store the CPF portion of the message */
            data_ptr = resp_msg + EIP_ENCAP_HEADER_SIZ;
            resp_msg_siz = 0;

            /* store the Interface Handle (always 0) */
            rta_PutLitEndian32(0, data_ptr+resp_msg_siz);
            resp_msg_siz += 4;

            /* store the Timeout (always 0) */
            rta_PutLitEndian16(0, data_ptr+resp_msg_siz);
            resp_msg_siz += 2;

            /* store the Item Count */
            rta_PutLitEndian16(eips_stc_cpf.ItemCount, data_ptr+resp_msg_siz);
            resp_msg_siz += 2;

            /* store the Items */
            for(i=0; i<eips_stc_cpf.ItemCount; i++)
            {
                /* store the Item Code */
                rta_PutLitEndian16(eips_stc_cpf.Items[i].TypeId, data_ptr+resp_msg_siz);
                resp_msg_siz += 2;

                /* store the Item Length */
                rta_PutLitEndian16(eips_stc_cpf.Items[i].Length, data_ptr+resp_msg_siz);
                resp_msg_siz += 2;

                /* store the Item Data */
                for(j=0; j<eips_stc_cpf.Items[i].Length; j++)
                {
                    data_ptr[resp_msg_siz] = eips_stc_cpf.Items[i].Data[j];
                    resp_msg_siz++;
                }
            }

            /* store the length */
            EncapReq->Length = resp_msg_siz;
            rta_PutLitEndian16(EncapReq->Length, resp_msg+2);
            resp_msg_siz = (uint16)(EncapReq->Length+EIP_ENCAP_HEADER_SIZ);

            /* transmit the message */
            eips_usersock_sendTCPData (sock->sock_id, resp_msg, resp_msg_siz);
        }

        /* invalid data length */
        else
        {
            /* build the error message */
            EncapReq->Status = EIP_ENCAPSTATUSERR_FORMAT;
            eips_encap_unSupEncapMsg(EncapReq, sock);
            return;
        }
    }

    /* ************************** */
    /* Common Packat Format Error */
    /* ************************** */
    else
    {
        /* build the error message */
        EncapReq->Status = EIP_ENCAPSTATUSERR_FORMAT;
        eips_encap_unSupEncapMsg(EncapReq, sock);
        return;
    }
}

/* ====================================================================
Function:   eips_local_objModProc
Parameters: Encapsulated message structure
            socket structure pointer
Returns:    SUCCESS/FAILURE

This function processes the explicit message and returns the rsp_data.
There is special processing for Vendor Specific Services.
======================================================================= */
static void eips_local_objModProc (uint8 msg_type)
{
    uint8   path_seg, path_val;
    int16   i, j, table_ix;
    uint8   stop_processing;
    uint8   sc_not_UCMM;

    /* initialize variables */
    CIA_Struct.ServCode = eips_stc_cpf.emreq->user_struct.service;
    CIA_Struct.ServCodeIx = SERVCODEIX_MAX;
    CIA_Struct.ClassID = 0;
    CIA_Struct.ClassID_Found = FALSE;
    CIA_Struct.InstID = 0;
    CIA_Struct.InstID_Found = FALSE;
    CIA_Struct.AttrID = 0;
    CIA_Struct.AttrID_Found = FALSE;    
    stop_processing =  0;
    sc_not_UCMM = FALSE;

    /* parse the path */
    for(i=0; i<eips_stc_cpf.emreq->user_struct.path_size && stop_processing == FALSE; i++)
    {
        path_seg = EIPS_LO(eips_stc_cpf.emreq->user_struct.path[i]);
        path_val = EIPS_HI(eips_stc_cpf.emreq->user_struct.path[i]);

        /* The only valid order is Class, Inst, Attr */
        switch (path_seg)
        {
            /* 8 Bit Class */
            case LOGSEG_8BITCLASS:
                /* This must be the first path */
                if( (CIA_Struct.ClassID_Found == FALSE) &&
                    ( CIA_Struct.InstID_Found == FALSE) &&
                    ( CIA_Struct.AttrID_Found == FALSE) )
                {
                    CIA_Struct.ClassID_Found = TRUE;
                    CIA_Struct.ClassID = path_val;
	            }

	            /* Bad Path order */
	            else
	            {
	                /* Error: Path Segment Error */
                    eips_local_cpf_buildError(&eips_stc_cpf, ERR_PATHSEGMENT, 0, 0);
	                stop_processing = 1;
                }   
	            break;

            /* 16 Bit Class */
            case LOGSEG_16BITCLASS:
                /* This must be the first path */
                if( (CIA_Struct.ClassID_Found == FALSE) &&
                    ( CIA_Struct.InstID_Found == FALSE) &&
                    ( CIA_Struct.AttrID_Found == FALSE) )
                {
                    CIA_Struct.ClassID_Found = TRUE;
                    i++; /* the class id is stored in the next word) */
	                CIA_Struct.ClassID = (uint16)(eips_stc_cpf.emreq->user_struct.path[i]);
	            }

	            /* Bad Path order */
	            else
	            {
	                /* Error: Path Segment Error */
                    eips_local_cpf_buildError(&eips_stc_cpf, ERR_PATHSEGMENT, 0, 0);
	                stop_processing = 1;
                }   
	            break;

            /* 8 Bit Instance */
            case LOGSEG_8BITINST:
                /* This must be the second path */
                if( (CIA_Struct.ClassID_Found == TRUE)  &&
                    ( CIA_Struct.InstID_Found == FALSE) &&
		            ( CIA_Struct.AttrID_Found == FALSE) )
	            {
	                CIA_Struct.InstID_Found = TRUE;
	                CIA_Struct.InstID = path_val;
                }

	            /* Bad Path order */
	            else
	            {
	                /* Error: Path Segment Error */
                    eips_local_cpf_buildError(&eips_stc_cpf, ERR_PATHSEGMENT, 0, 0);
	                stop_processing = 1;
	            }
	            break;

            /* 16 Bit Instance */
            case LOGSEG_16BITINST:
                /* This must be the second path */
                if( (CIA_Struct.ClassID_Found == TRUE)  &&
                    ( CIA_Struct.InstID_Found == FALSE) &&
		            ( CIA_Struct.AttrID_Found == FALSE) )
	            {
	                CIA_Struct.InstID_Found = TRUE;
                    i++; /* the instance id is stored in the next word) */
	                CIA_Struct.InstID = (uint16)(eips_stc_cpf.emreq->user_struct.path[i]);
                }

	            /* Bad Path order */
	            else
	            {
	                /* Error: Path Segment Error */
                    eips_local_cpf_buildError(&eips_stc_cpf, ERR_PATHSEGMENT, 0, 0);
	                stop_processing = 1;
	            }
	            break;

	            /* 8 Bit Attribute */
	            case LOGSEG_8BITATTR:
                    /* This must be the third path */
	                if( (CIA_Struct.ClassID_Found == TRUE) &&
		                ( CIA_Struct.InstID_Found == TRUE) &&
		                ( CIA_Struct.AttrID_Found == FALSE) )
	                {
                        CIA_Struct.AttrID_Found = TRUE;
	                    CIA_Struct.AttrID = path_val;
	                }

	                /* Bad Path order */
	                else
	                {
	                    /* Error: Path Segment Error */
                        eips_local_cpf_buildError(&eips_stc_cpf, ERR_PATHSEGMENT, 0, 0);
	                    stop_processing = 1;
	                }
	                break;

                /* ERROR with path */
                default:
                    /* Error: Path Segment Error */
                    eips_local_cpf_buildError(&eips_stc_cpf, ERR_PATHSEGMENT, 0, 0);
	                stop_processing = 1;
                    break;
        }; /* END-> "switch (path_seg)" */
    }

    /* keep processing if we don't have an error */
    if(!stop_processing)
    {
        /* This function code requires a valid class, instance, attribute */
        if((CIA_Struct.ClassID_Found == FALSE) || (CIA_Struct.InstID_Found == FALSE))
        {
            /* Error: Path Destination Unknown */
            eips_local_cpf_buildError(&eips_stc_cpf, ERR_PATHDESTUNKNOWN, 0, 0);
            stop_processing = 1;
        }
    }

#ifdef EIPC_CCO_USED
    /* check if we need to keep processing */
    if(!stop_processing)
    {
		if (CIA_Struct.ClassID == CLASS_CCO)
		{
            if(eipc_cco_procObject (&eips_stc_cpf.emreq->user_struct, &eips_stc_cpf.emrsp->user_struct) == FAILURE)
	        {
                /* Error: Path Destination Unknown */
                eips_local_cpf_buildError(&eips_stc_cpf, ERR_PATHDESTUNKNOWN, 0, 0);
            }
            stop_processing = 1;
		}
    }
#endif

    /* check if we need to keep processing */
    if(!stop_processing)
    {
        /* switch on the service code */
        switch(CIA_Struct.ServCode)
        {
#ifdef EIPS_PCCC_USED
            /* PCCC Execute */
            case CIP_SC_EXECUTE_PCCC:
                /* make sure we don't have an attribute */
                if(CIA_Struct.AttrID_Found == FALSE)
                {
                    CIA_Struct.ServCodeIx = SERVCODEIX_EXECPCCC;    
                }

                else
                {
                    /* Error: Path Destination Unknown */
                    eips_local_cpf_buildError(&eips_stc_cpf, ERR_PATHDESTUNKNOWN, 0, 0);
                    stop_processing = 1;
                }
                break;
#endif

            /* Get Attribute All */
            case CIP_SC_GET_ATTR_ALL: 
                /* make sure we don't have an attribute */
                if(CIA_Struct.AttrID_Found == FALSE)
                {
                    CIA_Struct.ServCodeIx = SERVCODEIX_GETATTRALL;
                }

                else
                {
                    /* Error: Path Destination Unknown */
                    eips_local_cpf_buildError(&eips_stc_cpf, ERR_PATHDESTUNKNOWN, 0, 0);
                    stop_processing = 1;
                }
                break;

            /* Reset */
            case CIP_SC_RESET:
                /* make sure we don't have an attribute */
                if(CIA_Struct.AttrID_Found == FALSE)
                {
                    CIA_Struct.ServCodeIx = SERVCODEIX_RESET;
                }

                else
                {
                    /* Error: Path Destination Unknown */
                    eips_local_cpf_buildError(&eips_stc_cpf, ERR_PATHDESTUNKNOWN, 0, 0);
                    stop_processing = 1;
                }
                break;

            /* Get Attribute Single */
            case CIP_SC_GET_ATTR_SINGLE: 
                /* make sure we have an attribute */
                if(CIA_Struct.AttrID_Found == TRUE)
                {
                    CIA_Struct.ServCodeIx = SERVCODEIX_GETSINGLE;       
                }

                else
                {
                    /* Error: Path Destination Unknown */
                    eips_local_cpf_buildError(&eips_stc_cpf, ERR_PATHDESTUNKNOWN, 0, 0);
                    stop_processing = 1;
                }
                break;

            /* Set Attribute Single */
            case CIP_SC_SET_ATTR_SINGLE:
                /* make sure we have an attribute */
                if(CIA_Struct.AttrID_Found == TRUE)
                {
                    CIA_Struct.ServCodeIx = SERVCODEIX_SETSINGLE;       
                }

                else
                {
                    /* Error: Path Destination Unknown */
                    eips_local_cpf_buildError(&eips_stc_cpf, ERR_PATHDESTUNKNOWN, 0, 0);
                    stop_processing = 1;
                }
                break;

            /* Forward Open Request */
            case CIP_SC_FWD_OPEN_REQ:
                /* make sure we don't have an attribute */
                if(CIA_Struct.AttrID_Found == FALSE)
                {
                    CIA_Struct.ServCodeIx = SERVCODEIX_FWDOPEN;       

                    /* Only UCMM messages can open connections */
                    if(msg_type != EIPS_MSGTYP_UCMM)
                       sc_not_UCMM = TRUE;                       
                }

                else
                {
                    /* Error: Path Destination Unknown */
                    eips_local_cpf_buildError(&eips_stc_cpf, ERR_PATHDESTUNKNOWN, 0, 0);
                    stop_processing = 1;
                }   				
	            break;

            /* Forward Close Request */
            case CIP_SC_FWD_CLOSE_REQ:
                /* make sure we don't have an attribute */
                if(CIA_Struct.AttrID_Found == FALSE)
                {
                    CIA_Struct.ServCodeIx = SERVCODEIX_FWDCLOSE;       

                    /* Only UCMM messages can close connections */
                    if(msg_type != EIPS_MSGTYP_UCMM)
                       sc_not_UCMM = TRUE;
                }

                else
                {
                    /* Error: Path Destination Unknown */
                    eips_local_cpf_buildError(&eips_stc_cpf, ERR_PATHDESTUNKNOWN, 0, 0);
                    stop_processing = 1;
                }                
                break;

            /* Service Not Supported */
            default:
                /* see if this is a vendor specific access */
		        if(eips_userobj_procVendSpecObject (&eips_stc_cpf.emreq->user_struct, &eips_stc_cpf.emrsp->user_struct) == FAILURE)
		        {
                    /* Error: Service Not Supported */
                    eips_local_cpf_buildError(&eips_stc_cpf, ERR_SERV_UNSUPP, 0, 0);
                }
                stop_processing = 1;
                break;
        } /* END-> "switch on the service code" */
    }

    /* keep processing if we don't have an error */
    if(!stop_processing)
    {
        /* process the request */
        if(CIA_Struct.ServCodeIx < SERVCODEIX_MAX)
        {
            /* find the table index */
            for(i=0, table_ix=-1; i<EIS_SERVCODE_LOOKUP_SIZE && table_ix<0; i++)
            {
                /* we found the class and instance */
                if( (EIPS_ServCode_Lookup[i].ClassID == CIA_Struct.ClassID) &&
                    (EIPS_ServCode_Lookup[i].InstID == CIA_Struct.InstID) )
                {
                    table_ix = i;
                }
            }

            /* we found the table index (i.e. the Class Instance and Service Code are supported */
            if(table_ix >= 0)
            {
                /* if the function pointer is valid, call it */
                if((sc_not_UCMM == FALSE) && (EIPS_ServCode_Lookup[table_ix].FnPtr[CIA_Struct.ServCodeIx] != NULL_CPF_FUNCPTR))
                {
                    EIPS_ServCode_Lookup[table_ix].FnPtr[CIA_Struct.ServCodeIx](&eips_stc_cpf);
                }

                /* return an error */
                else
                {
                    /* Error: Service Not Supported */
                    eips_local_cpf_buildError(&eips_stc_cpf, ERR_SERV_UNSUPP, 0, 0);
                    stop_processing = 1;
                }
            }

            /* pass to user code */
            else
            {
                /* if we didn't find the table index, check for Assembly Processing */
                if((CIA_Struct.ClassID == CLASS_ASSEMBLY) && (eips_cpf_asmInstExists(CIA_Struct.InstID)))
                {
                    /* switch on the service code */
                    switch(CIA_Struct.ServCode)
                    {
                        /* Get Attribute Single */
                        case CIP_SC_GET_ATTR_SINGLE: 
                            /* make sure we have an attribute */
                            if(CIA_Struct.AttrID_Found == TRUE)
                            {
                                eips_local_GetSingleAsm(CIA_Struct.InstID,CIA_Struct.AttrID);
                            }

                            else
                            {
                                /* Error: Path Destination Unknown */
                                eips_local_cpf_buildError(&eips_stc_cpf, ERR_PATHDESTUNKNOWN, 0, 0);
                                stop_processing = 1;
                            }
                            break;

                        /* Set Attribute Single */
                        case CIP_SC_SET_ATTR_SINGLE: 
                            /* make sure we have an attribute */
                            if(CIA_Struct.AttrID_Found == TRUE)
                            {
                                eips_local_SetSingleAsm(CIA_Struct.InstID,CIA_Struct.AttrID);
                            }

                            else
                            {
                                /* Error: Path Destination Unknown */
                                eips_local_cpf_buildError(&eips_stc_cpf, ERR_PATHDESTUNKNOWN, 0, 0);
                                stop_processing = 1;
                            }
                            break;

                        /* Service Not Supported */
                        default:
	                        /* Error: Service Not Supported */
                            eips_local_cpf_buildError(&eips_stc_cpf, ERR_SERV_UNSUPP, 0, 0);
                            stop_processing = 1;
                            break;
                    };
                }

                /* see if this is a vendor specific access */
		        else if(eips_userobj_procVendSpecObject (&eips_stc_cpf.emreq->user_struct, &eips_stc_cpf.emrsp->user_struct) == FAILURE)
		        {
	                /* Error: Path Destination Unknown */
                    eips_local_cpf_buildError(&eips_stc_cpf, ERR_PATHDESTUNKNOWN, 0, 0);
                }
                stop_processing = 1;
            }
        }

        /* pass to user code */
        else
        {
            /* see if this is a vendor specific access */
		    if (eips_userobj_procVendSpecObject (&eips_stc_cpf.emreq->user_struct, &eips_stc_cpf.emrsp->user_struct) == FAILURE)
		    {
	            /* Error: Path Destination Unknown */
                eips_local_cpf_buildError(&eips_stc_cpf, ERR_PATHDESTUNKNOWN, 0, 0);
            }
            stop_processing = 1;
        }        
    }

    /* *************************** */
    /* Build the Explicit Response */
    /* *************************** */

    /* if we receive a UCMM message, start at 0 */
    i = 0;

    /* if we receive a connected message, store the sequence number */
    if(msg_type == EIPS_MSGTYP_EM)
    {
        rta_PutLitEndian16(eips_stc_cpf.SequenceNumber, eips_stc_cpf.Items[EIPS_CPF_DATA_ITEM].Data+i);
        i += 2;
    }

    /* store the service code (7th bit shows rsp_data) */
    eips_stc_cpf.Items[EIPS_CPF_DATA_ITEM].Data[i++] = (uint8)(eips_stc_cpf.emrsp->service | 0x80);

    /* store the reserved byte (always 0) */
    eips_stc_cpf.Items[EIPS_CPF_DATA_ITEM].Data[i++] = 0;

    /* store the general status */
    eips_stc_cpf.Items[EIPS_CPF_DATA_ITEM].Data[i++] = eips_stc_cpf.emrsp->user_struct.gen_stat;

    /* store the extended status size */
    eips_stc_cpf.Items[EIPS_CPF_DATA_ITEM].Data[i++] = eips_stc_cpf.emrsp->user_struct.ext_stat_size;

    /* store the extended status */
    for(j=0; j<eips_stc_cpf.emrsp->user_struct.ext_stat_size; j++)
    {
        rta_PutLitEndian16(eips_stc_cpf.emrsp->user_struct.ext_stat[j], eips_stc_cpf.Items[EIPS_CPF_DATA_ITEM].Data+i);
        i += 2;
    }

    /* store the rsp_data data */
    for(j=0; j<eips_stc_cpf.emrsp->user_struct.rsp_data_size; j++)
    {
        eips_stc_cpf.Items[EIPS_CPF_DATA_ITEM].Data[i++] = eips_stc_cpf.emrsp->user_struct.rsp_data[j];
    }

    /* store the item length */
    eips_stc_cpf.Items[EIPS_CPF_DATA_ITEM].Length = i;
} /* END-> eips_local_objModProc() */

/* ====================================================================
Function:   eips_local_getSingle
Parameters: N/A
Returns:    N/A

This function processes the Get_Attribute_Single service code.
======================================================================= */
static void eips_local_getSingle (CPF_MESSAGE_STRUCT *cpf)
{
    int16  i, table_ix;

    RTA_UNUSED_PARAM(cpf);

    /* This function code is invalid if any data is passed */
    if(eips_stc_cpf.emreq->user_struct.req_data_size != 0)
    {
        /* Error: Too Much Data */
        eips_local_cpf_buildError(&eips_stc_cpf, ERR_TOOMUCH_DATA, 0, 0);
        return; /* exit on an error */
    }

    /* Vendor Specific Class (pass to user) */
    if( (CIA_Struct.ClassID >= CLASSRANGE8_VENDSPEC_MIN) && 
        (CIA_Struct.ClassID <= CLASSRANGE8_VENDSPEC_MAX))
    {
        /* see if this is a vendor specific access */
		if(eips_userobj_procVendSpecObject (&eips_stc_cpf.emreq->user_struct, &eips_stc_cpf.emrsp->user_struct) == FAILURE)
		{
	        /* Error: Path Destination Unknown */
            eips_local_cpf_buildError(&eips_stc_cpf, ERR_PATHDESTUNKNOWN, 0, 0);
        }
        return;
    }

    /* ----------------------------------------------------------------------------- */
    /* we previously matched that the class, instance and service code are supported */
    /* ----------------------------------------------------------------------------- */

    /* Find where to start looking for the attribute */
    table_ix = -1;

    /* Validate the Attribute ID */
    for(i=0; ((i<EIPS_GETSETATTRTAB_SIZE) && (table_ix < 0)); i++)
    {
        /* instance is found */
        if( (EIPSGetSetAttrTable[i].ClassID == CIA_Struct.ClassID) && (EIPSGetSetAttrTable[i].InstID == CIA_Struct.InstID) &&
            (EIPSGetSetAttrTable[i].AttrID == CIA_Struct.AttrID))
        {
            /* store the table index */
            table_ix = i;
        }
    }

    /* Attribute isn't found */
    if(table_ix < 0)
    {
        /* Vendor Specific Attribute (if class/instance is supported, pass to user) */
        if((CIA_Struct.AttrID >= ATTRRANGE8_VENDSPEC_MIN) && (CIA_Struct.AttrID <= ATTRRANGE8_VENDSPEC_MAX))
        {
            /* see if this is a vendor specific access */
            if(eips_userobj_procVendSpecObject (&eips_stc_cpf.emreq->user_struct, &eips_stc_cpf.emrsp->user_struct) == FAILURE)
            {
                /* Error: Attribute not supported */
                eips_local_cpf_buildError(&eips_stc_cpf, ERR_ATTR_UNSUPP, 0, 0);
            }
            return;
        }
 
        /* Error: Attribute not supported */
        eips_local_cpf_buildError(&eips_stc_cpf, ERR_ATTR_UNSUPP, 0, 0);
        return; /* exit on an error */
    }

    /* Valid Service Code */
    if( (EIPSGetSetAttrTable[table_ix].SrvCdMsk == EIPS_SCMASK_GETSNG) ||
        (EIPSGetSetAttrTable[table_ix].SrvCdMsk == EIPS_SCMASK_SETSNG))
    {
        /* Check if we need to go right to the function pointer */
        if(EIPSGetSetAttrTable[table_ix].Type == EIPS_TYPE_UNUSED)
        {
            if(EIPSGetSetAttrTable[table_ix].FnPtr != NULLPTR)
            {
                EIPSGetSetAttrTable[table_ix].FnPtr();
                return;
            }

	        /* we should never get here */
	        else
            {
                return;
            }
        }

        /* verify the data pointer isn't NULL */
        if(EIPSGetSetAttrTable[table_ix].DataPtr == NULLPTR)
        {
            return;
        }

        /* based on the data type, build the rsp_data message */
        switch(EIPSGetSetAttrTable[table_ix].Type)
        {
            case EIPS_TYPE_UINT8:
            case EIPS_TYPE_INT8:
                eips_stc_cpf.emrsp->user_struct.rsp_data_size = 1;
                eips_stc_cpf.emrsp->user_struct.rsp_data[0] = *((uint8 *)EIPSGetSetAttrTable[table_ix].DataPtr);
                break;

            case EIPS_TYPE_UINT16:
            case EIPS_TYPE_INT16:
                eips_stc_cpf.emrsp->user_struct.rsp_data_size = 2;
                rta_PutLitEndian16(*((uint16 *)EIPSGetSetAttrTable[table_ix].DataPtr), eips_stc_cpf.emrsp->user_struct.rsp_data);
                break;

            case EIPS_TYPE_UINT32:
            case EIPS_TYPE_INT32:
            case EIPS_TYPE_FLOAT:
                eips_stc_cpf.emrsp->user_struct.rsp_data_size = 4;
                rta_PutLitEndian32(*((uint32 *)EIPSGetSetAttrTable[table_ix].DataPtr), eips_stc_cpf.emrsp->user_struct.rsp_data);
                break;

            /* we should never get here */
            default:
                return;
        }
    }

    /* Invalid Service Code */
    else
    {
        /* Error: Service not supported */
        eips_local_cpf_buildError(&eips_stc_cpf, ERR_SERV_UNSUPP, 0, 0);
        return; /* exit on an error */
    }
}

/* ====================================================================
Function:   eips_local_setSingle
Parameters: N/A
Returns:    N/A

This function processes the Set_Attribute_Single service code.
======================================================================= */
static void eips_local_setSingle (CPF_MESSAGE_STRUCT *cpf)
{
    int16  i, table_ix;

    RTA_UNUSED_PARAM(cpf);

    /* Vendor Specific Class (pass to user) */
    if( (CIA_Struct.ClassID >= CLASSRANGE8_VENDSPEC_MIN) && 
        (CIA_Struct.ClassID <= CLASSRANGE8_VENDSPEC_MAX))
    {
        /* see if this is a vendor specific access */
		if(eips_userobj_procVendSpecObject (&eips_stc_cpf.emreq->user_struct, &eips_stc_cpf.emrsp->user_struct) == FAILURE)
		{
	        /* Error: Path Destination Unknown */
            eips_local_cpf_buildError(&eips_stc_cpf, ERR_PATHDESTUNKNOWN, 0, 0);
        }
        return;
    }

    /* ----------------------------------------------------------------------------- */
    /* we previously matched that the class, instance and service code are supported */
    /* ----------------------------------------------------------------------------- */

    /* Validate the Attribute ID */
    for(i=0, table_ix = -1; ((i<EIPS_GETSETATTRTAB_SIZE) && (table_ix < 0)); i++)
    {
        /* instance is found */
        if( (EIPSGetSetAttrTable[i].ClassID == CIA_Struct.ClassID) && (EIPSGetSetAttrTable[i].InstID == CIA_Struct.InstID) &&
            (EIPSGetSetAttrTable[i].AttrID == CIA_Struct.AttrID))
        {
            /* store the table index */
            table_ix = i;
        }
    }

    /* Attribute isn't found */
    if(table_ix < 0)
    {
        /* Vendor Specific Attribute (if class/instance is supported, pass to user) */
        if((CIA_Struct.AttrID >= ATTRRANGE8_VENDSPEC_MIN) && (CIA_Struct.AttrID <= ATTRRANGE8_VENDSPEC_MAX))
        {
            /* see if this is a vendor specific access */
            if(eips_userobj_procVendSpecObject (&eips_stc_cpf.emreq->user_struct, &eips_stc_cpf.emrsp->user_struct) == FAILURE)
            {
                /* Error: Attribute not supported */
                eips_local_cpf_buildError(&eips_stc_cpf, ERR_ATTR_UNSUPP, 0, 0);
            }
            return;
        }

        /* Error: Attribute not supported */
        eips_local_cpf_buildError(&eips_stc_cpf, ERR_ATTR_UNSUPP, 0, 0);
        return; /* exit on an error */
    }

    /* Valid Service Code */
    if(EIPSGetSetAttrTable[table_ix].SrvCdMsk == EIPS_SCMASK_SETSNG)
    {
        /* Check if we need to go right to the function pointer */
        if(EIPSGetSetAttrTable[table_ix].Type == EIPS_TYPE_UNUSED)
        {
            if(EIPSGetSetAttrTable[table_ix].FnPtr != NULLPTR)
            {
                EIPSGetSetAttrTable[table_ix].FnPtr();
                return;
            }

            /* we should never get here */
            else
            {
                return;
            }
        }

        else
        {
            return;
        }
    }

    /* Invalid Service Code */
    else
    {
        /* Error: Service not supported */
        eips_local_cpf_buildError(&eips_stc_cpf, ERR_SERV_UNSUPP, 0, 0);
        return; /* exit on an error */
    }
}

/**/
/* ******************************************************************** */
/*                      LOCAL FUNCTIONS                                 */
/* ******************************************************************** */

/* ************************* */
/* IDENTITY OBJECT FUNCTIONS */
/* ************************* */
/* ====================================================================
Function:   eips_local_IDObjInstGetAll
Parameters: N/A
Returns:    N/A
C/I/A:      Identity (0x01, 0x01, N/A)

Before this function is called, the service code, class and instance
is validated. This function only needs to build the rsp_data message.
======================================================================= */
static void eips_local_IDObjInstGetAll (CPF_MESSAGE_STRUCT *cpf)
{
    RTA_UNUSED_PARAM(cpf);

    /* initialize the data size to 0 */
    eips_stc_cpf.emrsp->user_struct.rsp_data_size = 0;

    /* Store the Vendor ID */
    rta_PutLitEndian16(eips_IDObj.Inst.Vendor, (eips_stc_cpf.emrsp->user_struct.rsp_data+eips_stc_cpf.emrsp->user_struct.rsp_data_size));
    eips_stc_cpf.emrsp->user_struct.rsp_data_size += 2;

    /* Store the Device Type */
    rta_PutLitEndian16(eips_IDObj.Inst.DeviceType, (eips_stc_cpf.emrsp->user_struct.rsp_data+eips_stc_cpf.emrsp->user_struct.rsp_data_size));
    eips_stc_cpf.emrsp->user_struct.rsp_data_size += 2;

    /* Store the Product Code */
    rta_PutLitEndian16(eips_IDObj.Inst.ProductCode, (eips_stc_cpf.emrsp->user_struct.rsp_data+eips_stc_cpf.emrsp->user_struct.rsp_data_size));
    eips_stc_cpf.emrsp->user_struct.rsp_data_size += 2;

    /* Store the Revision */
    rta_PutLitEndian16(eips_IDObj.Inst.Revision, (eips_stc_cpf.emrsp->user_struct.rsp_data+eips_stc_cpf.emrsp->user_struct.rsp_data_size));
    eips_stc_cpf.emrsp->user_struct.rsp_data_size += 2;

    /* Store the Status */
    rta_PutLitEndian16(eips_IDObj.Inst.Status, (eips_stc_cpf.emrsp->user_struct.rsp_data+eips_stc_cpf.emrsp->user_struct.rsp_data_size));
    eips_stc_cpf.emrsp->user_struct.rsp_data_size += 2;

    /* Store the Serial Number */
    rta_PutLitEndian32(eips_IDObj.Inst.SerialNum, (eips_stc_cpf.emrsp->user_struct.rsp_data+eips_stc_cpf.emrsp->user_struct.rsp_data_size));
    eips_stc_cpf.emrsp->user_struct.rsp_data_size += 4;

    /* Store the Product Name */
    eips_stc_cpf.emrsp->user_struct.rsp_data[eips_stc_cpf.emrsp->user_struct.rsp_data_size++] = RTA_MIN(EIPS_USER_IDOBJ_NAME_SIZE, EIPS_STRUCT_MAX_PROD_NAME_SIZE);
    rta_ByteMove((eips_stc_cpf.emrsp->user_struct.rsp_data+eips_stc_cpf.emrsp->user_struct.rsp_data_size), (uint8*)EIPS_USER_IDOBJ_NAME, RTA_MIN(EIPS_USER_IDOBJ_NAME_SIZE, EIPS_STRUCT_MAX_PROD_NAME_SIZE));
    eips_stc_cpf.emrsp->user_struct.rsp_data_size += RTA_MIN(EIPS_USER_IDOBJ_NAME_SIZE, EIPS_STRUCT_MAX_PROD_NAME_SIZE);
}

/* ====================================================================
Function:   eips_local_IDObjReset
Parameters: N/A
Returns:    N/A
C/I/A:      Identity (0x01, 0x01, N/A)

Before this function is called, the service code, class and instance
is validated. This function only needs to validate the data and
perform the reset service.
======================================================================= */
static void eips_local_IDObjReset (CPF_MESSAGE_STRUCT *cpf)
{

    RTA_UNUSED_PARAM(cpf);

    /* Normal Reset */
    if ((eips_stc_cpf.emreq->user_struct.req_data_size == 0) ||
        ((eips_stc_cpf.emreq->user_struct.req_data_size == 1) &&
        (eips_stc_cpf.emreq->user_struct.req_data[0] == 0)))
    {
        /* change the state */
        eips_rtasys_setState(0,EIPS_STATE_RESETNORMAL); /*jdw fix me */
    }

    /* Out of Box Reset */
    else if((eips_stc_cpf.emreq->user_struct.req_data_size == 1) &&
            (eips_stc_cpf.emreq->user_struct.req_data[0] == 1))
    {
        /* change the state */
        eips_rtasys_setState(0,EIPS_STATE_RESETOUTOFBOX); /*jdw fix me */
    }

    /* Error */
    else
    {
        /* Error: Invalid Parameter */
        eips_local_cpf_buildError(&eips_stc_cpf, ERR_INV_SERVICE_PARM, 0, 0);
    }
}

/* ====================================================================
Function:   eips_local_IDObjProdName
Parameters: N/A
Returns:    N/A
C/I/A:      Identity (0x01, 0x01, 0x07)
Settable?:  NO

This function returns the ID Object's Product Name.
======================================================================= */
static void eips_local_IDObjProdName (void)
{
    /* We should only be called by Get_Attribute_Single */
    if(eips_stc_cpf.emreq->user_struct.service == CIP_SC_GET_ATTR_SINGLE)
    {
        /* store the name length first */
        eips_stc_cpf.emrsp->user_struct.rsp_data[0] = RTA_MIN(EIPS_USER_IDOBJ_NAME_SIZE, EIPS_STRUCT_MAX_PROD_NAME_SIZE);

        /* store the name next */        
        rta_ByteMove(eips_stc_cpf.emrsp->user_struct.rsp_data+1, (uint8*)EIPS_USER_IDOBJ_NAME, RTA_MIN(EIPS_USER_IDOBJ_NAME_SIZE, EIPS_STRUCT_MAX_PROD_NAME_SIZE));

        /* the total length is the name length + 1 */
        eips_stc_cpf.emrsp->user_struct.rsp_data_size = (RTA_MIN(EIPS_USER_IDOBJ_NAME_SIZE, EIPS_STRUCT_MAX_PROD_NAME_SIZE)+1);
    }
}

/* ************************* */
/* ASSEMBLY OBJECT FUNCTIONS */
/* ************************* */
/* ====================================================================
Function:   eips_local_GetSingleAsm
Parameters: instance
	        attribute
Returns:    N/A
C/I/A:      Assembly (0x04, 0x??, 0x03)
Settable?:  NO

This function validate the passed class and instance.  If both are
supported, a successful response is sent.  The only valid attributes
are instance 0 attribute 1 (Class Revision) and valid instances with
attribute 3 (data).
======================================================================= */
static void eips_local_GetSingleAsm (uint16 instance, uint8 attribute)
{
    uint16 i, j;
    int16 size;
   
    /* set the return size to 0 */
    eips_stc_cpf.emrsp->user_struct.rsp_data_size = 0;

    /* ************************************************ */
    /*              Class Level Attributes              */
    /* ************************************************ */
    if(instance == 0)
    {
        /* validate the attribute id is "Revision" */
        if(attribute == 1)
        {
            rta_PutLitEndian16(EIPS_ASSEMBLY_CLASS_REV, eips_stc_cpf.emrsp->user_struct.rsp_data);
            eips_stc_cpf.emrsp->user_struct.rsp_data_size = 2;
            return; /* exit on success */
        }

        /* validate the attribute id is "Max Instance" */
        else if(attribute == 2)
        {
            rta_PutLitEndian16(EIPS_USEROBJ_ASM_MAXINST, eips_stc_cpf.emrsp->user_struct.rsp_data);
            eips_stc_cpf.emrsp->user_struct.rsp_data_size = 2;
            return; /* exit on success */
        }

        /* unsupported attribute id */
        else
        {
            /* Error: Attribute not supported */
            eips_local_cpf_buildError(&eips_stc_cpf, ERR_ATTR_UNSUPP, 0, 0);
            return; /* exit on an error */
        }
    }

    /* ************************************************ */
    /*            Instance Level Attributes             */
    /* ************************************************ */

    /* get the up to date size, error if not available */
    size = eips_cpf_getAsmSize (instance);
    if(size == -1)
    {
        /* Error: Object Does Not Exist */
        eips_local_cpf_buildError(&eips_stc_cpf, ERR_UNEXISTANT_OBJ, 0, 0);
        return; /* exit on an error */
    }

#if 0 /* don't include heartbeats in attribute access */
    /* Input Only     */
    #ifdef EIPS_USEROBJ_ASM_INPUTONLY_HB_O2TINST
        if(instance == EIPS_USEROBJ_ASM_INPUTONLY_HB_O2TINST)
        {
            /* validate the attribute id is "Data" */
            if(attribute == 3)
            {
                eips_stc_cpf.emrsp->user_struct.rsp_data_size = 0;
                return; /* exit on success */
            }

            /* unsupported attribute id */
            else
            {
                /* Error: Attribute not supported */
                eips_local_cpf_buildError(&eips_stc_cpf, ERR_ATTR_UNSUPP, 0, 0);
                return; /* exit on an error */
            }
        }
    #endif
#endif

#if 0 /* don't include heartbeats in attribute access */
    /* Listen Only */
    #ifdef EIPS_USEROBJ_ASM_LISTENONLY_HB_O2TINST
        if(instance == EIPS_USEROBJ_ASM_LISTENONLY_HB_O2TINST)
        {
            /* validate the attribute id is "Data" */
            if(attribute == 3)
            {
                eips_stc_cpf.emrsp->user_struct.rsp_data_size = 0;
                return; /* exit on success */
            }

            /* unsupported attribute id */
            else
            {
                /* Error: Attribute not supported */
                eips_local_cpf_buildError(&eips_stc_cpf, ERR_ATTR_UNSUPP, 0, 0);
                return; /* exit on an error */
            }
        }
    #endif
#endif

#if EIPS_USEROBJ_ASM_MAXNUM_O2TINST > 0   
    /* O_to_T Assembly Instance(s) */
    for(j=0; j<EIPS_USEROBJ_ASM_MAXNUM_O2TINST; j++)
    {
        /* validate we support the instance */
        if(eips_AsmO2TObj[j].Inst == instance)
        {
            /* validate the attribute id is "Data" */
            if(attribute == 3)
            {
                /* store the data */	     
                EIPS_GET_MUTEX; /* get the MUTEX on the shared data */
                for(i=0; i<eips_AsmO2TObj[j].DataSize; i++)
                {
#if EIPS_USEROBJ_ASM_ELEMENT_SIZE == 4      /* DWORDS */
                    rta_PutLitEndian32(eips_AsmO2TObj[j].DataPtr[i], eips_stc_cpf.emrsp->user_struct.rsp_data + eips_stc_cpf.emrsp->user_struct.rsp_data_size);
                    eips_stc_cpf.emrsp->user_struct.rsp_data_size += 4;
#elif EIPS_USEROBJ_ASM_ELEMENT_SIZE == 2    /* WORDS */
                    rta_PutLitEndian16(eips_AsmO2TObj[j].DataPtr[i], eips_stc_cpf.emrsp->user_struct.rsp_data + eips_stc_cpf.emrsp->user_struct.rsp_data_size);
                    eips_stc_cpf.emrsp->user_struct.rsp_data_size += 2;
#else                                       /* BYTES */
                    eips_stc_cpf.emrsp->user_struct.rsp_data[eips_stc_cpf.emrsp->user_struct.rsp_data_size] = eips_AsmO2TObj[j].DataPtr[i];
                    eips_stc_cpf.emrsp->user_struct.rsp_data_size += 1;
#endif
                }
                EIPS_PUT_MUTEX; /* return the MUTEX on the shared data */
                return; /* exit on success */
            }

            /* unsupported attribute id */
            else
            {
                /* Error: Attribute not supported */
                eips_local_cpf_buildError(&eips_stc_cpf, ERR_ATTR_UNSUPP, 0, 0);
                return; /* exit on an error */
            }
        }
    }
#endif

    /* T_to_O Assembly Instance(s) */
    for(j=0; j<EIPS_USEROBJ_ASM_MAXNUM_T2OINST; j++)
    {
        /* validate we support the instance */
        if(eips_AsmT2OObj[j].Inst == instance)
        {
            /* validate the attribute id is "Data" */
            if(attribute == 3)
            {
                /* store the data */
                EIPS_GET_MUTEX; /* get the MUTEX on the shared data */
                for(i=0; i<eips_AsmT2OObj[j].DataSize; i++)
                {
#if EIPS_USEROBJ_ASM_ELEMENT_SIZE == 4      /* DWORDS */
                    rta_PutLitEndian32(eips_AsmT2OObj[j].DataPtr[i], eips_stc_cpf.emrsp->user_struct.rsp_data + eips_stc_cpf.emrsp->user_struct.rsp_data_size);
                    eips_stc_cpf.emrsp->user_struct.rsp_data_size += 4;
#elif EIPS_USEROBJ_ASM_ELEMENT_SIZE == 2    /* WORDS */
                    rta_PutLitEndian16(eips_AsmT2OObj[j].DataPtr[i], eips_stc_cpf.emrsp->user_struct.rsp_data + eips_stc_cpf.emrsp->user_struct.rsp_data_size);
                    eips_stc_cpf.emrsp->user_struct.rsp_data_size += 2;
#else                                       /* BYTES */
                    eips_stc_cpf.emrsp->user_struct.rsp_data[eips_stc_cpf.emrsp->user_struct.rsp_data_size] = eips_AsmT2OObj[j].DataPtr[i];
                    eips_stc_cpf.emrsp->user_struct.rsp_data_size += 1;
#endif
                }
	            EIPS_PUT_MUTEX; /* return the MUTEX on the shared data */
                return; /* exit on success */
            }

            /* unsupported attribute id */
            else
            {
                /* Error: Attribute not supported */
                eips_local_cpf_buildError(&eips_stc_cpf, ERR_ATTR_UNSUPP, 0, 0);
                return; /* exit on an error */
            }
        }
    }

    /* Error: Object Does Not Exist */
    eips_local_cpf_buildError(&eips_stc_cpf, ERR_UNEXISTANT_OBJ, 0, 0);
    return; /* exit on an error */
}

/* ====================================================================
Function:   eips_local_SetSingleAsm
Parameters: instance
	        attribute
Returns:    N/A
C/I/A:      Assemb;y (0x04, 0x??, 0x03)
Settable?:  YES

This function validate the passed class and instance.  If both are
supported, a successful response is sent.  The only valid attributes
are valid instances with attribute 3 (data).
======================================================================= */
static void eips_local_SetSingleAsm (uint16 instance, uint8 attribute)
{
#if EIPS_USEROBJ_ASM_MAXNUM_O2TINST > 0
    uint16 j, k;
#endif
    uint16 i;
    int16 size;
   
    /* Class Revision */
    if(instance == 0)
    {
        /* validate the attribute id is "Revision" */
        if(attribute == 1)
        {
            /* Error: Attribute Read Only */
            eips_local_cpf_buildError(&eips_stc_cpf, ERR_ATTR_READONLY, 0, 0);
            return; /* exit on an error */
        }

        /* unsupported attribute id */
        else
        {
            /* Error: Attribute not supported */
            eips_local_cpf_buildError(&eips_stc_cpf, ERR_ATTR_UNSUPP, 0, 0);
            return; /* exit on an error */
        }
    }

    /* get the up to date size, error if not available */
    size = eips_cpf_getAsmSize (instance);
    if(size == -1)
    {
        /* Error: Object Does Not Exist */
        eips_local_cpf_buildError(&eips_stc_cpf, ERR_UNEXISTANT_OBJ, 0, 0);
        return; /* exit on an error */
    }

   /* adjust the size to byte size if needed */
#if EIPS_USEROBJ_ASM_ELEMENT_SIZE == 4      /* DWORDS */
    size = (int16) (size * 4);
#elif EIPS_USEROBJ_ASM_ELEMENT_SIZE == 2    /* WORDS */
    size = (int16) (size * 2);
#else                                       /* BYTES */
/*    size = size * 1; */
#endif

#if 0 /* don't include heartbeats in attribute access */
    /* Input Only     */
    #ifdef EIPS_USEROBJ_ASM_INPUTONLY_HB_O2TINST
        if(instance == EIPS_USEROBJ_ASM_INPUTONLY_HB_O2TINST)
        {
            /* validate the attribute id is "Data" */
            if(attribute == 3)
            {
                /* size must be 0 */
                if(eips_stc_cpf.emreq->user_struct.req_data_size > 0)
                {
                    /* Error: Too Much Data */
                    eips_local_cpf_buildError(&eips_stc_cpf, ERR_TOOMUCH_DATA, 0, 0);
                    return; /* exit on an error */
                }

                return; /* exit on success */
            }

            /* unsupported attribute id */
            else
            {
                /* Error: Attribute not supported */
                eips_local_cpf_buildError(&eips_stc_cpf, ERR_ATTR_UNSUPP, 0, 0);
                return; /* exit on an error */
            }
        }
    #endif
#endif

#if 0 /* don't include heartbeats in attribute access */
    /* Listen Only */
    #ifdef EIPS_USEROBJ_ASM_LISTENONLY_HB_O2TINST
        if(instance == EIPS_USEROBJ_ASM_LISTENONLY_HB_O2TINST)
        {
            /* validate the attribute id is "Data" */
            if(attribute == 3)
            {
                /* size must be 0 */
                if(eips_stc_cpf.emreq->user_struct.req_data_size > 0)
                {
                    /* Error: Too Much Data */
                    eips_local_cpf_buildError(&eips_stc_cpf, ERR_TOOMUCH_DATA, 0, 0);
                    return; /* exit on an error */
                }

                return; /* exit on success */
            }

            /* unsupported attribute id */
            else
            {
                /* Error: Attribute not supported */
                eips_local_cpf_buildError(&eips_stc_cpf, ERR_ATTR_UNSUPP, 0, 0);
                return; /* exit on an error */
            }
        }
    #endif
#endif

    /* T_to_O Assembly Instance(s) */
    for(i=0; i<EIPS_USEROBJ_ASM_MAXNUM_T2OINST; i++)
    {
        /* validate we support the instance */
        if(eips_AsmT2OObj[i].Inst == instance)
        {
            /* Error: Service not supported */
            eips_local_cpf_buildError(&eips_stc_cpf, ERR_SERV_UNSUPP, 0, 0);
            return; /* exit on an error */
        }
    }

#if EIPS_USEROBJ_ASM_MAXNUM_O2TINST > 0
    /* O_to_T Assembly Instance(s) */
    for(j=0; j<EIPS_USEROBJ_ASM_MAXNUM_O2TINST; j++)
    {
        /* validate we support the instance */
        if(eips_AsmO2TObj[j].Inst == instance)
        {
            /* validate the attribute id is "Data" */
            if(attribute == 3)
            {
                /* validate we don't have an I/O connection (else "Device State Conflict") */
                if(eips_iomsg_allocated(eips_AsmO2TObj[j].Inst) == TRUE)
                {
                    /* "Device State Conflict" */
                    eips_local_cpf_buildError(&eips_stc_cpf, ERR_DEV_STATE_CONFLICT, 0, 0);
                    return; /* return on error */
                }

                /* validate the size matches */
                if(eips_stc_cpf.emreq->user_struct.req_data_size < size)
                {
                    /* Error: Not Enough Data */
                    eips_local_cpf_buildError(&eips_stc_cpf, ERR_INSUFF_DATA, 0, 0);
                    return; /* exit on an error */
                }

                else if(eips_stc_cpf.emreq->user_struct.req_data_size > size)
                {
                    /* Error: Too Much Data */
                    eips_local_cpf_buildError(&eips_stc_cpf, ERR_TOOMUCH_DATA, 0, 0);
                    return; /* exit on an error */
                }

                /* store the data */
                EIPS_GET_MUTEX; /* get the MUTEX on the shared data */

#if EIPS_USEROBJ_ASM_ELEMENT_SIZE == 4      /* DWORDS */
                for(i=0, k=0; i<eips_stc_cpf.emreq->user_struct.req_data_size; i+=4)
                    eips_AsmO2TObj[j].DataPtr[k++] = rta_GetLitEndian32(eips_stc_cpf.emreq->user_struct.req_data + i);
#elif EIPS_USEROBJ_ASM_ELEMENT_SIZE == 2    /* WORDS */
                for(i=0, k=0; i<eips_stc_cpf.emreq->user_struct.req_data_size; i+=2)
                    eips_AsmO2TObj[j].DataPtr[k++] = rta_GetLitEndian16(eips_stc_cpf.emreq->user_struct.req_data + i);
#else                                       /* BYTES */
                for(i=0, k=0; i<eips_stc_cpf.emreq->user_struct.req_data_size; i++)
                    eips_AsmO2TObj[j].DataPtr[k++] = eips_stc_cpf.emreq->user_struct.req_data[i];
#endif
                EIPS_PUT_MUTEX; /* return the MUTEX on the shared data */

#if 0 /* don't include heartbeats in attribute access */
    /* Input Only     */
    #ifdef EIPS_USEROBJ_ASM_INPUTONLY_HB_O2TINST
                    /* don't tell the user about heartbeat messages */
                    if(instance == EIPS_USEROBJ_ASM_INPUTONLY_HB_O2TINST)
                        return;
    #endif

    /* Listen Only */
    #ifdef EIPS_USEROBJ_ASM_LISTENONLY_HB_O2TINST
                    /* don't tell the user about heartbeat messages     */
                    if(instance == EIPS_USEROBJ_ASM_LISTENONLY_HB_O2TINST)
                        return;
    #endif
#endif

                /* infom the user app the data was updated */
                eips_userobj_asmDataWrote (eips_AsmO2TObj[j].Inst, eips_stc_cpf.emreq->user_struct.req_data_size);
                return; /* exit on success */
            }

            /* unsupported attribute id */
            else
            {
                /* Error: Attribute not supported */
                eips_local_cpf_buildError(&eips_stc_cpf, ERR_ATTR_UNSUPP, 0, 0);
                return; /* exit on an error */
            }
        }
    }
#endif

    /* Error: Object Does Not Exist */
    eips_local_cpf_buildError(&eips_stc_cpf, ERR_UNEXISTANT_OBJ, 0, 0);
    return; /* exit on an error */
}

/* ====================================================================
Function:   eips_cpf_o2tCnxnPtValid
Parameters: 8-bit connection point id
Returns:    TRUE/FALSE

This function validate the connection point (O_to_T) is valid.
======================================================================= */
uint8 eips_cpf_o2tCnxnPtValid (uint8 cnxnpt)
{
#if EIPS_USEROBJ_ASM_MAXNUM_O2TINST > 0    
    uint16 i;
#endif

    /* Input Only     */
#ifdef EIPS_USEROBJ_ASM_INPUTONLY_HB_O2TINST
    if(cnxnpt == EIPS_USEROBJ_ASM_INPUTONLY_HB_O2TINST)
        return(TRUE);
#endif

    /* Listen Only */
#ifdef EIPS_USEROBJ_ASM_LISTENONLY_HB_O2TINST
    if(cnxnpt == EIPS_USEROBJ_ASM_LISTENONLY_HB_O2TINST)
        return(TRUE);
#endif

#if EIPS_USEROBJ_ASM_MAXNUM_O2TINST > 0    
    /* validate we support the O_to_T Instance */
    for(i=0; i<EIPS_USEROBJ_ASM_MAXNUM_O2TINST; i++)
        if(eips_AsmO2TObj[i].Inst == ((uint16)cnxnpt))
            return(TRUE);
#endif

    /* the connection point is invalid */
    return(FALSE);
}

/* ====================================================================
Function:   eips_cpf_t2oCnxnPtValid
Parameters: 8-bit connection point id
Returns:    TRUE/FALSE

This function validate the connection point (T_to_O) is valid.
======================================================================= */
uint8 eips_cpf_t2oCnxnPtValid (uint8 cnxnpt)
{
    uint16 i;

    /* validate we support the T_to_O Instance */
    for(i=0; i<EIPS_USEROBJ_ASM_MAXNUM_T2OINST; i++)
        if(eips_AsmT2OObj[i].Inst == ((uint16)cnxnpt))
	        return(TRUE);

    /* the connection point is invalid */
    return(FALSE);
}

/* ====================================================================
Function:   eips_cpf_getAsmSize
Parameters: assembly instance ID
Returns:    Size of assembly instance (-1 on error)

This function checks to see if the assembly instance is supported and
returns the number of bytes/words stored in the assembly or -1 on error.
======================================================================= */
int16 eips_cpf_getAsmSize (uint16 instance)
{
    uint16 i;

    /* Input Only     */
#ifdef EIPS_USEROBJ_ASM_INPUTONLY_HB_O2TINST
    if(instance == EIPS_USEROBJ_ASM_INPUTONLY_HB_O2TINST)
        return(0);
#endif

    /* Listen Only */
#ifdef EIPS_USEROBJ_ASM_LISTENONLY_HB_O2TINST
    if(instance == EIPS_USEROBJ_ASM_LISTENONLY_HB_O2TINST)
        return(0);
#endif
       
    /* T_to_O Assembly */
    for(i=0; i<EIPS_USEROBJ_ASM_MAXNUM_T2OINST; i++)
    {
        if(eips_AsmT2OObj[i].Inst == instance)
        {
            /* update the size */
            eips_userobj_getAsmPtr ((uint16) eips_AsmT2OObj[i].Inst, (uint16 *)&eips_AsmT2OObj[i].DataSize);	  
            return(eips_AsmT2OObj[i].DataSize);
        }
    }

#if EIPS_USEROBJ_ASM_MAXNUM_O2TINST > 0        
    /* O_to_T Assembly */
    for(i=0; i<EIPS_USEROBJ_ASM_MAXNUM_O2TINST; i++)
    {
        if(eips_AsmO2TObj[i].Inst == instance)
        {
            /* update the size */
            eips_userobj_getAsmPtr ((uint16) eips_AsmO2TObj[i].Inst, (uint16 *)&eips_AsmO2TObj[i].DataSize);	  
            return(eips_AsmO2TObj[i].DataSize);
        }
    }
#endif

    /* instance not found */
    return(-1);
}

/* ====================================================================
Function:     eips_cpf_asmInstExists
Parameters:   16-bit instance ID
Returns:      TRUE/FALSE

This function returns TRUE if the passed instance exists in the 
Assembly Object. This is needed for sending correct error codes on
service codes that aren't supported.
======================================================================= */
uint8 eips_cpf_asmInstExists (uint16 instance)
{
    uint16 j;

    /* Class Revision */
    if(instance == 0)
    {
        return(TRUE);
    }

#if 0 /* don't include heartbeats in attribute access */   
    /* Heartbeat Instances */
    if( (EIPS_USEROBJ_ASM_INPUTONLY_HB_O2TINST == instance) ||
        (EIPS_USEROBJ_ASM_LISTENONLY_HB_O2TINST == instance))
    {
        return(TRUE);
    }
#endif

    /* O_to_T Assembly Instance(s) */
    for(j=0; j<EIPS_USEROBJ_ASM_MAXNUM_O2TINST; j++)
    {
        /* validate we support the instance */
        if(eips_AsmO2TObj[j].Inst == instance)
        {
            return(TRUE);
        }
    }

    /* T_to_O Assembly Instance(s) */
    for(j=0; j<EIPS_USEROBJ_ASM_MAXNUM_T2OINST; j++)
    {    
        /* validate we support the instance */
        if(eips_AsmT2OObj[j].Inst == instance)
        {
            return(TRUE);
        }
    }

    /* instance isn't supported */
    return(FALSE);
}

/* ******************** */
/* TCP OBJECT FUNCTIONS */
/* ******************** */
/* ====================================================================
Function:   eips_local_TcpObjInstGetAll
Parameters: N/A
Returns:    N/A
C/I/A:      TCP (0xF5, 0x01, N/A)

Before this function is called, the service code, class and instance
is validated. This function only needs to build the rsp_data message.
======================================================================= */
static void eips_local_TcpObjInstGetAll (CPF_MESSAGE_STRUCT *cpf)
{
/* ATTR 01 */ uint32  InterfaceStatus;
/* ATTR 02 */ uint32  InterfaceCapFlags;
/* ATTR 03 */ uint32  InterfaceCtrlFlags;
/* ATTR 04 */ EIPS_PHY_LINK_STRUCT PhysicalLinkObj;
/* ATTR 05 */ EIPS_IPADDR_STRUCT   InterfaceCfg;
/* ATTR 06 */ EIPS_TCPOBJ_BUF_STRUCT  HostName;

    int16 i;

    RTA_UNUSED_PARAM(cpf);

    /* get the up to date values from the user (set to 0 on error) */
    if(eips_usersock_getTCPObj_Attr01_Status(&InterfaceStatus) != ERR_SUCCESS)
        memset(&InterfaceStatus, 0, sizeof(InterfaceStatus));

    if(eips_usersock_getTCPObj_Attr02_CfgCap(&InterfaceCapFlags) != ERR_SUCCESS)
        memset(&InterfaceCapFlags, 0, sizeof(InterfaceCapFlags));

    if(eips_usersock_getTCPObj_Attr03_CfgCtrl(&InterfaceCtrlFlags) != ERR_SUCCESS)
        memset(&InterfaceCtrlFlags, 0, sizeof(InterfaceCtrlFlags));

    if(eips_usersock_getTCPObj_Attr04_PhyLinkObj(&PhysicalLinkObj) != ERR_SUCCESS)
        memset(&PhysicalLinkObj, 0, sizeof(PhysicalLinkObj));

    if(eips_usersock_getTCPObj_Attr05_IntfCfg(&InterfaceCfg) != ERR_SUCCESS)
        memset(&InterfaceCfg, 0, sizeof(InterfaceCfg));

    if(eips_usersock_getTCPObj_Attr06_HostName(&HostName) != ERR_SUCCESS)
        memset(&HostName, 0, sizeof(HostName));

    /* initialize the data size to 0 */
    eips_stc_cpf.emrsp->user_struct.rsp_data_size = 0;

    /* ******* */
    /* ATTR 01 */
    /* ******* */

    /* Store the Status */
    rta_PutLitEndian32(InterfaceStatus, (eips_stc_cpf.emrsp->user_struct.rsp_data+eips_stc_cpf.emrsp->user_struct.rsp_data_size));
    eips_stc_cpf.emrsp->user_struct.rsp_data_size += 4;

    /* ******* */
    /* ATTR 02 */
    /* ******* */

    /* Store Configuration Capability */
    rta_PutLitEndian32(InterfaceCapFlags, (eips_stc_cpf.emrsp->user_struct.rsp_data+eips_stc_cpf.emrsp->user_struct.rsp_data_size));
    eips_stc_cpf.emrsp->user_struct.rsp_data_size += 4;

    /* ******* */
    /* ATTR 03 */
    /* ******* */

    /* Store Configuration Control */
    rta_PutLitEndian32(InterfaceCtrlFlags, (eips_stc_cpf.emrsp->user_struct.rsp_data+eips_stc_cpf.emrsp->user_struct.rsp_data_size));
    eips_stc_cpf.emrsp->user_struct.rsp_data_size += 4;

    /* ******* */
    /* ATTR 04 */
    /* ******* */

    /* Store the Path Size (in words) */
    rta_PutLitEndian16(PhysicalLinkObj.PathSiz, (eips_stc_cpf.emrsp->user_struct.rsp_data+eips_stc_cpf.emrsp->user_struct.rsp_data_size));
    eips_stc_cpf.emrsp->user_struct.rsp_data_size += 2;

    /* Store the Path (in bytes - truncate if needed) */
    if(PhysicalLinkObj.PathSiz > sizeof(PhysicalLinkObj.Path))
        PhysicalLinkObj.PathSiz = sizeof(PhysicalLinkObj.Path);
    for(i=0; i<(PhysicalLinkObj.PathSiz*2); i++)
    {
        eips_stc_cpf.emrsp->user_struct.rsp_data[eips_stc_cpf.emrsp->user_struct.rsp_data_size++] = PhysicalLinkObj.Path[i];
    }

    /* ******* */
    /* ATTR 05 */
    /* ******* */

    /* Store the IP Address */
    rta_PutLitEndian32(InterfaceCfg.IPAddress, (eips_stc_cpf.emrsp->user_struct.rsp_data+eips_stc_cpf.emrsp->user_struct.rsp_data_size));
    eips_stc_cpf.emrsp->user_struct.rsp_data_size += 4;

    /* Store the Network Mask */
    rta_PutLitEndian32(InterfaceCfg.NtwkMask, (eips_stc_cpf.emrsp->user_struct.rsp_data+eips_stc_cpf.emrsp->user_struct.rsp_data_size));
    eips_stc_cpf.emrsp->user_struct.rsp_data_size += 4;

    /* Store the Gateway Address */
    rta_PutLitEndian32(InterfaceCfg.GtwyIPAddress, (eips_stc_cpf.emrsp->user_struct.rsp_data+eips_stc_cpf.emrsp->user_struct.rsp_data_size));
    eips_stc_cpf.emrsp->user_struct.rsp_data_size += 4;

    /* Store the Name Server */
    rta_PutLitEndian32(InterfaceCfg.NameServer, (eips_stc_cpf.emrsp->user_struct.rsp_data+eips_stc_cpf.emrsp->user_struct.rsp_data_size));
    eips_stc_cpf.emrsp->user_struct.rsp_data_size += 4;

    /* Store the Name Server 2 */
    rta_PutLitEndian32(InterfaceCfg.NameServer2, (eips_stc_cpf.emrsp->user_struct.rsp_data+eips_stc_cpf.emrsp->user_struct.rsp_data_size));
    eips_stc_cpf.emrsp->user_struct.rsp_data_size += 4;

    /* Store the Domain Name Size*/
    rta_PutLitEndian16(InterfaceCfg.DomainName.Bufsiz, (eips_stc_cpf.emrsp->user_struct.rsp_data+eips_stc_cpf.emrsp->user_struct.rsp_data_size));
    eips_stc_cpf.emrsp->user_struct.rsp_data_size += 2;

    /* Store the Domain Name - truncate if needed */
    if(InterfaceCfg.DomainName.Bufsiz > sizeof(InterfaceCfg.DomainName.Buf))
        InterfaceCfg.DomainName.Bufsiz = sizeof(InterfaceCfg.DomainName.Buf);
    for(i=0; i<InterfaceCfg.DomainName.Bufsiz; i++)
    {
        eips_stc_cpf.emrsp->user_struct.rsp_data[eips_stc_cpf.emrsp->user_struct.rsp_data_size++] = InterfaceCfg.DomainName.Buf[i];
    }
    if(i%2 != 0) /* pad Domain Name to even number of bytes */
        eips_stc_cpf.emrsp->user_struct.rsp_data[eips_stc_cpf.emrsp->user_struct.rsp_data_size++] = 0;

    /* ******* */
    /* ATTR 06 */
    /* ******* */

    /* Store the Host Name Size */
    rta_PutLitEndian16(HostName.Bufsiz, (eips_stc_cpf.emrsp->user_struct.rsp_data+eips_stc_cpf.emrsp->user_struct.rsp_data_size));
    eips_stc_cpf.emrsp->user_struct.rsp_data_size += 2;

    /* Store the Host Name - truncate if needed */
    if(HostName.Bufsiz > sizeof(HostName.Buf))
        HostName.Bufsiz = sizeof(HostName.Buf);
    for(i=0; i<HostName.Bufsiz; i++)
    {
        eips_stc_cpf.emrsp->user_struct.rsp_data[eips_stc_cpf.emrsp->user_struct.rsp_data_size++] = HostName.Buf[i];
    }

    if(i%2 != 0) /* pad Host Name to even number of bytes */
        eips_stc_cpf.emrsp->user_struct.rsp_data[eips_stc_cpf.emrsp->user_struct.rsp_data_size++] = 0;
}

/* ====================================================================
Function:   eips_local_TcpObjStatus
C/I/A:      TCP (0xF5, 0x01, 0x01)
Settable?:  NO
======================================================================= */
static void eips_local_TcpObjStatus (void)
{
    uint32  Status;
    uint8   return_code;

    /* Get Attribute Single */
    if(eips_stc_cpf.emreq->user_struct.service == CIP_SC_GET_ATTR_SINGLE)
    {
        /* get the parameter from the user */
        return_code = eips_usersock_getTCPObj_Attr01_Status(&Status);

        /* return_code is invalid, return it */
        if(return_code != ERR_SUCCESS)
        {
            eips_local_cpf_buildError(&eips_stc_cpf, return_code, 0, 0);
            return; /* return on error */
        }

        /* return_code successful, send the response */
        rta_PutLitEndian32(Status, eips_stc_cpf.emrsp->user_struct.rsp_data);
        eips_stc_cpf.emrsp->user_struct.rsp_data_size = 4;
    }

    /* Set Attribute Single */
    else if(eips_stc_cpf.emreq->user_struct.service == CIP_SC_SET_ATTR_SINGLE)
    {
        /* "Attribute Read Only" */
        eips_local_cpf_buildError(&eips_stc_cpf, ERR_ATTR_READONLY, 0, 0);
        return; /* return on error */
    }

    /* Unsupported Service Code */
    else
    {
        /* "Service Not Supported" */
        eips_local_cpf_buildError(&eips_stc_cpf, ERR_SERV_UNSUPP, 0, 0);
        return; /* return on error */
    }
}

/* ====================================================================
Function:   eips_local_TcpObjCfgCap
C/I/A:      TCP (0xF5, 0x01, 0x02)
Settable?:  NO
======================================================================= */
static void eips_local_TcpObjCfgCap (void)
{
    uint32  CfgCapability;
    uint8   return_code;

    /* Get Attribute Single */
    if(eips_stc_cpf.emreq->user_struct.service == CIP_SC_GET_ATTR_SINGLE)
    {
        /* get the parameter from the user */
        return_code = eips_usersock_getTCPObj_Attr02_CfgCap(&CfgCapability);

        /* return_code is invalid, return it */
        if(return_code != ERR_SUCCESS)
        {
            eips_local_cpf_buildError(&eips_stc_cpf, return_code, 0, 0);
            return; /* return on error */
        }

        /* return_code successful, send the response */
        rta_PutLitEndian32(CfgCapability, eips_stc_cpf.emrsp->user_struct.rsp_data);
        eips_stc_cpf.emrsp->user_struct.rsp_data_size = 4;
    }

    /* Set Attribute Single */
    else if(eips_stc_cpf.emreq->user_struct.service == CIP_SC_SET_ATTR_SINGLE)
    {
        /* "Attribute Read Only" */
        eips_local_cpf_buildError(&eips_stc_cpf, ERR_ATTR_READONLY, 0, 0);
        return; /* return on error */
    }

    /* Unsupported Service Code */
    else
    {
        /* "Service Not Supported" */
        eips_local_cpf_buildError(&eips_stc_cpf, ERR_SERV_UNSUPP, 0, 0);
        return; /* return on error */
    }
}

/* ====================================================================
Function:   eips_local_TcpObjCfgCtrl
C/I/A:      TCP (0xF5, 0x01, 0x03)
Settable?:  YES
======================================================================= */
static void eips_local_TcpObjCfgCtrl (void)
{
    uint32  CfgControl;
    uint8   return_code;

    /* Get Attribute Single */
    if(eips_stc_cpf.emreq->user_struct.service == CIP_SC_GET_ATTR_SINGLE)
    {
        /* get the parameter from the user */
        return_code = eips_usersock_getTCPObj_Attr03_CfgCtrl(&CfgControl);

        /* return_code is invalid, return it */
        if(return_code != ERR_SUCCESS)
        {
            eips_local_cpf_buildError(&eips_stc_cpf, return_code, 0, 0);
            return; /* return on error */
        }

        /* return_code successful, send the response */
        rta_PutLitEndian32(CfgControl, eips_stc_cpf.emrsp->user_struct.rsp_data);
        eips_stc_cpf.emrsp->user_struct.rsp_data_size = 4;
    }

    /* Set Attribute Single */
    else if(eips_stc_cpf.emreq->user_struct.service == CIP_SC_SET_ATTR_SINGLE)
    {
        /* size must be 4 */
        if(eips_stc_cpf.emreq->user_struct.req_data_size < 4)
        {
            /* Error: Too Much Data */
            eips_local_cpf_buildError(&eips_stc_cpf, ERR_INSUFF_DATA, 0, 0);
            return; /* exit on an error */
        }
        if(eips_stc_cpf.emreq->user_struct.req_data_size > 4)
        {
            /* Error: Too Much Data */
            eips_local_cpf_buildError(&eips_stc_cpf, ERR_TOOMUCH_DATA, 0, 0);
            return; /* exit on an error */
        }

        /* store the data from the client */
        CfgControl = rta_GetLitEndian32(eips_stc_cpf.emreq->user_struct.req_data);

        /* pass the request to the user */
        return_code = eips_usersock_setTCPObj_Attr03_CfgCtrl(&CfgControl);

        /* return the error (if needed) */
        if(return_code != ERR_SUCCESS)
        {
            eips_local_cpf_buildError(&eips_stc_cpf, return_code, 0, 0);
            return; /* return on error */
        }
        return; /* return - Successful */
    }

    /* Unsupported Service Code */
    else
    {
        /* "Service Not Supported" */
        eips_local_cpf_buildError(&eips_stc_cpf, ERR_SERV_UNSUPP, 0, 0);
        return; /* return on error */
    }
}

/* ====================================================================
Function:   eips_local_TcpObjPhyLink
C/I/A:      TCP (0xF5, 0x01, 0x04)
Settable?:  NO
======================================================================= */
static void eips_local_TcpObjPhyLink (void)
{
    EIPS_PHY_LINK_STRUCT PhysicalLinkObj;
    uint8   return_code;
    uint16  i;

    /* Get Attribute Single */
    if(eips_stc_cpf.emreq->user_struct.service == CIP_SC_GET_ATTR_SINGLE)
    {
        /* get the parameter from the user */
        return_code = eips_usersock_getTCPObj_Attr04_PhyLinkObj(&PhysicalLinkObj);

        /* return_code is invalid, return it */
        if(return_code != ERR_SUCCESS)
        {
            eips_local_cpf_buildError(&eips_stc_cpf, return_code, 0, 0);
            return; /* return on error */
        }

        /* truncate the response if too big */
        if(PhysicalLinkObj.PathSiz > sizeof(PhysicalLinkObj.Path))
            PhysicalLinkObj.PathSiz = sizeof(PhysicalLinkObj.Path);

        /* return_code successful, send the response */
        rta_PutLitEndian16(PhysicalLinkObj.PathSiz, eips_stc_cpf.emrsp->user_struct.rsp_data);
        eips_stc_cpf.emrsp->user_struct.rsp_data_size = 2;
        for(i=0; i<(PhysicalLinkObj.PathSiz*2); i++)
            eips_stc_cpf.emrsp->user_struct.rsp_data[eips_stc_cpf.emrsp->user_struct.rsp_data_size++] = PhysicalLinkObj.Path[i];
    }

    /* Set Attribute Single */
    else if(eips_stc_cpf.emreq->user_struct.service == CIP_SC_SET_ATTR_SINGLE)
    {
        /* "Attribute Read Only" */
        eips_local_cpf_buildError(&eips_stc_cpf, ERR_ATTR_READONLY, 0, 0);
        return; /* return on error */
    }

    /* Unsupported Service Code */
    else
    {
        /* "Service Not Supported" */
        eips_local_cpf_buildError(&eips_stc_cpf, ERR_SERV_UNSUPP, 0, 0);
        return; /* return on error */
    }
}

/* ====================================================================
Function:   eips_local_TcpObjIntfCfg
C/I/A:      TCP (0xF5, 0x01, 0x05)
Settable?:  YES
======================================================================= */
static void eips_local_TcpObjIntfCfg (void)
{
    EIPS_IPADDR_STRUCT  InterfaceConfig;
    uint8   return_code;
    uint16  i;

    /* Get Attribute Single */
    if(eips_stc_cpf.emreq->user_struct.service == CIP_SC_GET_ATTR_SINGLE)
    {
        /* get the parameter from the user */
        return_code = eips_usersock_getTCPObj_Attr05_IntfCfg(&InterfaceConfig);

        /* return_code is invalid, return it */
        if(return_code != ERR_SUCCESS)
        {
            eips_local_cpf_buildError(&eips_stc_cpf, return_code, 0, 0);
            return; /* return on error */
        }

        /* return_code successful, send the response */
        eips_stc_cpf.emrsp->user_struct.rsp_data_size = 0;

        /* Store the IP Address */
        rta_PutLitEndian32(InterfaceConfig.IPAddress, (eips_stc_cpf.emrsp->user_struct.rsp_data+eips_stc_cpf.emrsp->user_struct.rsp_data_size));
        eips_stc_cpf.emrsp->user_struct.rsp_data_size += 4;

        /* Store the Network Mask */
        rta_PutLitEndian32(InterfaceConfig.NtwkMask, (eips_stc_cpf.emrsp->user_struct.rsp_data+eips_stc_cpf.emrsp->user_struct.rsp_data_size));
        eips_stc_cpf.emrsp->user_struct.rsp_data_size += 4;

        /* Store the Gateway Address */
        rta_PutLitEndian32(InterfaceConfig.GtwyIPAddress, (eips_stc_cpf.emrsp->user_struct.rsp_data+eips_stc_cpf.emrsp->user_struct.rsp_data_size));
        eips_stc_cpf.emrsp->user_struct.rsp_data_size += 4;

        /* Store the Name Server */
        rta_PutLitEndian32(InterfaceConfig.NameServer, (eips_stc_cpf.emrsp->user_struct.rsp_data+eips_stc_cpf.emrsp->user_struct.rsp_data_size));
        eips_stc_cpf.emrsp->user_struct.rsp_data_size += 4;

        /* Store the Name Server 2 */
        rta_PutLitEndian32(InterfaceConfig.NameServer2, (eips_stc_cpf.emrsp->user_struct.rsp_data+eips_stc_cpf.emrsp->user_struct.rsp_data_size));
        eips_stc_cpf.emrsp->user_struct.rsp_data_size += 4;

        /* truncate the response if too big */
        if(InterfaceConfig.DomainName.Bufsiz > sizeof(InterfaceConfig.DomainName.Buf))
            InterfaceConfig.DomainName.Bufsiz = sizeof(InterfaceConfig.DomainName.Buf);

        /* Store the Domain Name Size*/
        rta_PutLitEndian16(InterfaceConfig.DomainName.Bufsiz, (eips_stc_cpf.emrsp->user_struct.rsp_data+eips_stc_cpf.emrsp->user_struct.rsp_data_size));
        eips_stc_cpf.emrsp->user_struct.rsp_data_size += 2;

        /* Store the Domain Name */
        for(i=0; i<InterfaceConfig.DomainName.Bufsiz; i++)
        {
	        eips_stc_cpf.emrsp->user_struct.rsp_data[eips_stc_cpf.emrsp->user_struct.rsp_data_size++] = InterfaceConfig.DomainName.Buf[i];
        }

        /* pad Domain Name to even number of bytes */
        if(i%2 != 0) 
	        eips_stc_cpf.emrsp->user_struct.rsp_data[eips_stc_cpf.emrsp->user_struct.rsp_data_size++] = 0;
    }

    /* Set Attribute Single */
    else if(eips_stc_cpf.emreq->user_struct.service == CIP_SC_SET_ATTR_SINGLE)
    {
        /* size must be between 22 and 70 */
        if(eips_stc_cpf.emreq->user_struct.req_data_size < 22)
        {
            /* Error: Too Much Data */
            eips_local_cpf_buildError(&eips_stc_cpf, ERR_INSUFF_DATA, 0, 0);
            return; /* exit on an error */
        }
        if(eips_stc_cpf.emreq->user_struct.req_data_size > 70)
        {
            /* Error: Too Much Data */
            eips_local_cpf_buildError(&eips_stc_cpf, ERR_TOOMUCH_DATA, 0, 0);
            return; /* exit on an error */
        }

        /* store the data from the client */
        InterfaceConfig.IPAddress           = rta_GetLitEndian32(eips_stc_cpf.emreq->user_struct.req_data);
        InterfaceConfig.NtwkMask            = rta_GetLitEndian32(eips_stc_cpf.emreq->user_struct.req_data+4);
        InterfaceConfig.GtwyIPAddress       = rta_GetLitEndian32(eips_stc_cpf.emreq->user_struct.req_data+8);
        InterfaceConfig.NameServer          = rta_GetLitEndian32(eips_stc_cpf.emreq->user_struct.req_data+12);
        InterfaceConfig.NameServer2         = rta_GetLitEndian32(eips_stc_cpf.emreq->user_struct.req_data+16);
        InterfaceConfig.DomainName.Bufsiz   = rta_GetLitEndian16(eips_stc_cpf.emreq->user_struct.req_data+20);

        /* make sure we have enough bytes for the domain name!!!!! */

        /* size must be between 22 and 70 */
        if(eips_stc_cpf.emreq->user_struct.req_data_size < (InterfaceConfig.DomainName.Bufsiz + 22))
        {
            /* Error: Too Much Data */
            eips_local_cpf_buildError(&eips_stc_cpf, ERR_INSUFF_DATA, 0, 0);
            return; /* exit on an error */
        }
        if(eips_stc_cpf.emreq->user_struct.req_data_size > (InterfaceConfig.DomainName.Bufsiz + 22))
        {
            /* Error: Too Much Data */
            eips_local_cpf_buildError(&eips_stc_cpf, ERR_TOOMUCH_DATA, 0, 0);
            return; /* exit on an error */
        }

        /* store the rest of the domain name */
        for(i=0; i<InterfaceConfig.DomainName.Bufsiz; i++)
            InterfaceConfig.DomainName.Buf[i] = eips_stc_cpf.emreq->user_struct.req_data[22+i];

        /* pass the request to the user */
        return_code = eips_usersock_setTCPObj_Attr05_IntfCfg(&InterfaceConfig);

        /* return the error (if needed) */
        if(return_code != ERR_SUCCESS)
        {
            eips_local_cpf_buildError(&eips_stc_cpf, return_code, 0, 0);
            return; /* return on error */
        }
        return; /* return - Successful */
    }

    /* Unsupported Service Code */
    else
    {
        /* "Service Not Supported" */
        eips_local_cpf_buildError(&eips_stc_cpf, ERR_SERV_UNSUPP, 0, 0);
        return; /* return on error */
    }
}

/* ====================================================================
Function:   eips_local_TcpObjHostName
C/I/A:      TCP (0xF5, 0x01, 0x06)
Settable?:  YES
======================================================================= */
static void eips_local_TcpObjHostName (void)
{
    EIPS_TCPOBJ_BUF_STRUCT HostName;
    uint8   return_code;
    uint16  i;

    /* Get Attribute Single */
    if(eips_stc_cpf.emreq->user_struct.service == CIP_SC_GET_ATTR_SINGLE)
    {
        /* get the parameter from the user */
        return_code = eips_usersock_getTCPObj_Attr06_HostName(&HostName);

        /* return_code is invalid, return it */
        if(return_code != ERR_SUCCESS)
        {
            eips_local_cpf_buildError(&eips_stc_cpf, return_code, 0, 0);
            return; /* return on error */
        }

        /* truncate the response if too big */
        if(HostName.Bufsiz > sizeof(HostName.Buf))
            HostName.Bufsiz = sizeof(HostName.Buf);

        /* return_code successful, send the response */
        rta_PutLitEndian16(HostName.Bufsiz, eips_stc_cpf.emrsp->user_struct.rsp_data);
        eips_stc_cpf.emrsp->user_struct.rsp_data_size = 2;
        for(i=0; i<HostName.Bufsiz; i++)
            eips_stc_cpf.emrsp->user_struct.rsp_data[eips_stc_cpf.emrsp->user_struct.rsp_data_size++] = HostName.Buf[i];
    }

    /* Set Attribute Single */
    else if(eips_stc_cpf.emreq->user_struct.service == CIP_SC_SET_ATTR_SINGLE)
    {
        /* size must be at least 2 */
        if(eips_stc_cpf.emreq->user_struct.req_data_size < 2)
        {
            /* Error: Too Much Data */
            eips_local_cpf_buildError(&eips_stc_cpf, ERR_INSUFF_DATA, 0, 0);
            return; /* exit on an error */
        }
        /* size cannot exceed 66 */
        if(eips_stc_cpf.emreq->user_struct.req_data_size > 66)
        {
            /* Error: Too Much Data */
            eips_local_cpf_buildError(&eips_stc_cpf, ERR_TOOMUCH_DATA, 0, 0);
            return; /* exit on an error */
        }

        HostName.Bufsiz = rta_GetLitEndian16(eips_stc_cpf.emreq->user_struct.req_data);

        /* make sure we have enough bytes for the domain name!!!!! */

        /* size must be between 2 and 66 */
        if(eips_stc_cpf.emreq->user_struct.req_data_size < (HostName.Bufsiz + 2))
        {
            /* Error: Too Much Data */
            eips_local_cpf_buildError(&eips_stc_cpf, ERR_INSUFF_DATA, 0, 0);
            return; /* exit on an error */
        }
        if(eips_stc_cpf.emreq->user_struct.req_data_size > (HostName.Bufsiz + 2))
        {
            /* Error: Too Much Data */
            eips_local_cpf_buildError(&eips_stc_cpf, ERR_TOOMUCH_DATA, 0, 0);
            return; /* exit on an error */
        }

        /* store the rest of the domain name */
        for(i=0; i<HostName.Bufsiz; i++)
            HostName.Buf[i] = eips_stc_cpf.emreq->user_struct.req_data[2+i];

        /* pass the request to the user */
        return_code = eips_usersock_setTCPObj_Attr06_HostName(&HostName);

        /* return the error (if needed) */
        if(return_code != ERR_SUCCESS)
        {
            eips_local_cpf_buildError(&eips_stc_cpf, return_code, 0, 0);
            return; /* return on error */
        }
        return; /* return - Successful */
    }

    /* Unsupported Service Code */
    else
    {
        /* "Service Not Supported" */
        eips_local_cpf_buildError(&eips_stc_cpf, ERR_SERV_UNSUPP, 0, 0);
        return; /* return on error */
    }
}

/* ****************************** */
/* Ethernet Link Object Functions */
/* ****************************** */
/* ====================================================================
Function:   eips_local_EnetObjInstGetAll
Parameters: N/A
Returns:    N/A
C/I/A:      Ethernet Link (0xF6, 0x01, N/A)

Before this function is called, the service code, class and instance
is validated. This function only needs to build the rsp_data message.
======================================================================= */
static void eips_local_EnetObjInstGetAll(CPF_MESSAGE_STRUCT *cpf)
{
/* ATTR 01 */ uint32    InterfaceSpeed;
/* ATTR 02 */ uint32    InterfaceFlags;
/* ATTR 03 */ uint8     PhysicalAddress[6];
/* ATTR 04 */ EIPS_ELINKOBJ_INTFCNTS_STRUCT     InterfaceCnts;
/* ATTR 05 */ EIPS_ELINKOBJ_MEDIACNTS_STRUCT    MediaCnts;
/* ATTR 06 */ EIPS_ELINKOBJ_INTFCTRL_STRUCT     InterfaceControl;

    uint16  i;

    RTA_UNUSED_PARAM(cpf);

    /* get the up to date values from the user (set to 0 on error) */
    if(eips_usersock_getEthLinkObj_Attr01_IntfSpeed(&InterfaceSpeed) != ERR_SUCCESS)
        memset(&InterfaceSpeed, 0, sizeof(InterfaceSpeed));

    if(eips_usersock_getEthLinkObj_Attr02_IntfFlags(&InterfaceFlags) != ERR_SUCCESS)
        memset(&InterfaceFlags, 0, sizeof(InterfaceFlags));

    if(eips_usersock_getEthLinkObj_Attr03_MacAddr(PhysicalAddress) != ERR_SUCCESS)
        memset(&PhysicalAddress, 0, sizeof(PhysicalAddress));

    if(eips_usersock_getEthLinkObj_Attr04_IntfCnts(&InterfaceCnts) != ERR_SUCCESS)
        memset(&InterfaceCnts, 0, sizeof(InterfaceCnts));

    if(eips_usersock_getEthLinkObj_Attr05_MediaCnts(&MediaCnts) != ERR_SUCCESS)
        memset(&MediaCnts, 0, sizeof(MediaCnts));

    if(eips_usersock_getEthLinkObj_Attr06_IntfCtrl(&InterfaceControl) != ERR_SUCCESS)
        memset(&InterfaceControl, 0, sizeof(InterfaceControl));

    /* initialize the data size to 0 */
    eips_stc_cpf.emrsp->user_struct.rsp_data_size = 0;

    /* ******* */
    /* ATTR 01 */
    /* ******* */

    /* Store the Interface Speed */
    rta_PutLitEndian32(InterfaceSpeed, (eips_stc_cpf.emrsp->user_struct.rsp_data+eips_stc_cpf.emrsp->user_struct.rsp_data_size));
    eips_stc_cpf.emrsp->user_struct.rsp_data_size += 4;

    /* ******* */
    /* ATTR 02 */
    /* ******* */

    /* Store the Interface Flags */
    rta_PutLitEndian32(InterfaceFlags, (eips_stc_cpf.emrsp->user_struct.rsp_data+eips_stc_cpf.emrsp->user_struct.rsp_data_size));
    eips_stc_cpf.emrsp->user_struct.rsp_data_size += 4;

    /* ******* */
    /* ATTR 03 */
    /* ******* */

    /* Store the Physical Address */
    for(i=0; i<6; i++)
        eips_stc_cpf.emrsp->user_struct.rsp_data[eips_stc_cpf.emrsp->user_struct.rsp_data_size++] = PhysicalAddress[i];

    /* ******* */
    /* ATTR 04 */
    /* ******* */

    /* Store the Interface Counters */
    rta_PutLitEndian32(InterfaceCnts.InOctets, (eips_stc_cpf.emrsp->user_struct.rsp_data+eips_stc_cpf.emrsp->user_struct.rsp_data_size));
    eips_stc_cpf.emrsp->user_struct.rsp_data_size += 4;

    rta_PutLitEndian32(InterfaceCnts.InUcastPackets, (eips_stc_cpf.emrsp->user_struct.rsp_data+eips_stc_cpf.emrsp->user_struct.rsp_data_size));
    eips_stc_cpf.emrsp->user_struct.rsp_data_size += 4;

    rta_PutLitEndian32(InterfaceCnts.InNUcastPackets, (eips_stc_cpf.emrsp->user_struct.rsp_data+eips_stc_cpf.emrsp->user_struct.rsp_data_size));
    eips_stc_cpf.emrsp->user_struct.rsp_data_size += 4;

    rta_PutLitEndian32(InterfaceCnts.InDiscards, (eips_stc_cpf.emrsp->user_struct.rsp_data+eips_stc_cpf.emrsp->user_struct.rsp_data_size));
    eips_stc_cpf.emrsp->user_struct.rsp_data_size += 4;

    rta_PutLitEndian32(InterfaceCnts.InErrors, (eips_stc_cpf.emrsp->user_struct.rsp_data+eips_stc_cpf.emrsp->user_struct.rsp_data_size));
    eips_stc_cpf.emrsp->user_struct.rsp_data_size += 4;

    rta_PutLitEndian32(InterfaceCnts.InUnknownProtos, (eips_stc_cpf.emrsp->user_struct.rsp_data+eips_stc_cpf.emrsp->user_struct.rsp_data_size));
    eips_stc_cpf.emrsp->user_struct.rsp_data_size += 4;

    rta_PutLitEndian32(InterfaceCnts.OutOctets, (eips_stc_cpf.emrsp->user_struct.rsp_data+eips_stc_cpf.emrsp->user_struct.rsp_data_size));
    eips_stc_cpf.emrsp->user_struct.rsp_data_size += 4;

    rta_PutLitEndian32(InterfaceCnts.OutUcastPackets, (eips_stc_cpf.emrsp->user_struct.rsp_data+eips_stc_cpf.emrsp->user_struct.rsp_data_size));
    eips_stc_cpf.emrsp->user_struct.rsp_data_size += 4;

    rta_PutLitEndian32(InterfaceCnts.OutNUcastPackets, (eips_stc_cpf.emrsp->user_struct.rsp_data+eips_stc_cpf.emrsp->user_struct.rsp_data_size));
    eips_stc_cpf.emrsp->user_struct.rsp_data_size += 4;

    rta_PutLitEndian32(InterfaceCnts.OutDiscards, (eips_stc_cpf.emrsp->user_struct.rsp_data+eips_stc_cpf.emrsp->user_struct.rsp_data_size));
    eips_stc_cpf.emrsp->user_struct.rsp_data_size += 4;

    rta_PutLitEndian32(InterfaceCnts.OutErrors, (eips_stc_cpf.emrsp->user_struct.rsp_data+eips_stc_cpf.emrsp->user_struct.rsp_data_size));
    eips_stc_cpf.emrsp->user_struct.rsp_data_size += 4;

    /* ******* */
    /* ATTR 05 */
    /* ******* */

    /* Store the Media Counters */
    rta_PutLitEndian32(MediaCnts.AlignmentErrors, (eips_stc_cpf.emrsp->user_struct.rsp_data+eips_stc_cpf.emrsp->user_struct.rsp_data_size));
    eips_stc_cpf.emrsp->user_struct.rsp_data_size += 4;

    rta_PutLitEndian32(MediaCnts.FCSErrors, (eips_stc_cpf.emrsp->user_struct.rsp_data+eips_stc_cpf.emrsp->user_struct.rsp_data_size));
    eips_stc_cpf.emrsp->user_struct.rsp_data_size += 4;

    rta_PutLitEndian32(MediaCnts.SingleCollisions, (eips_stc_cpf.emrsp->user_struct.rsp_data+eips_stc_cpf.emrsp->user_struct.rsp_data_size));
    eips_stc_cpf.emrsp->user_struct.rsp_data_size += 4;

    rta_PutLitEndian32(MediaCnts.MultipleCollisions, (eips_stc_cpf.emrsp->user_struct.rsp_data+eips_stc_cpf.emrsp->user_struct.rsp_data_size));
    eips_stc_cpf.emrsp->user_struct.rsp_data_size += 4;

    rta_PutLitEndian32(MediaCnts.SQETestErrors, (eips_stc_cpf.emrsp->user_struct.rsp_data+eips_stc_cpf.emrsp->user_struct.rsp_data_size));
    eips_stc_cpf.emrsp->user_struct.rsp_data_size += 4;

    rta_PutLitEndian32(MediaCnts.DeferredTransmissions, (eips_stc_cpf.emrsp->user_struct.rsp_data+eips_stc_cpf.emrsp->user_struct.rsp_data_size));
    eips_stc_cpf.emrsp->user_struct.rsp_data_size += 4;

    rta_PutLitEndian32(MediaCnts.LateCollisions, (eips_stc_cpf.emrsp->user_struct.rsp_data+eips_stc_cpf.emrsp->user_struct.rsp_data_size));
    eips_stc_cpf.emrsp->user_struct.rsp_data_size += 4;

    rta_PutLitEndian32(MediaCnts.ExcessiveCollisions, (eips_stc_cpf.emrsp->user_struct.rsp_data+eips_stc_cpf.emrsp->user_struct.rsp_data_size));
    eips_stc_cpf.emrsp->user_struct.rsp_data_size += 4;

    rta_PutLitEndian32(MediaCnts.MACTransmitErrors, (eips_stc_cpf.emrsp->user_struct.rsp_data+eips_stc_cpf.emrsp->user_struct.rsp_data_size));
    eips_stc_cpf.emrsp->user_struct.rsp_data_size += 4;

    rta_PutLitEndian32(MediaCnts.CarrierSenseErrors, (eips_stc_cpf.emrsp->user_struct.rsp_data+eips_stc_cpf.emrsp->user_struct.rsp_data_size));
    eips_stc_cpf.emrsp->user_struct.rsp_data_size += 4;

    rta_PutLitEndian32(MediaCnts.FrameTooLarge, (eips_stc_cpf.emrsp->user_struct.rsp_data+eips_stc_cpf.emrsp->user_struct.rsp_data_size));
    eips_stc_cpf.emrsp->user_struct.rsp_data_size += 4;

    rta_PutLitEndian32(MediaCnts.MACReceiveErrors, (eips_stc_cpf.emrsp->user_struct.rsp_data+eips_stc_cpf.emrsp->user_struct.rsp_data_size));
    eips_stc_cpf.emrsp->user_struct.rsp_data_size += 4;

    /* ******* */
    /* ATTR 06 */
    /* ******* */

    /* Store the Interface Control */
    rta_PutLitEndian16(InterfaceControl.ControlBits, (eips_stc_cpf.emrsp->user_struct.rsp_data+eips_stc_cpf.emrsp->user_struct.rsp_data_size));
    eips_stc_cpf.emrsp->user_struct.rsp_data_size += 2;

    rta_PutLitEndian16(InterfaceControl.ForcedInterfaceSpeed, (eips_stc_cpf.emrsp->user_struct.rsp_data+eips_stc_cpf.emrsp->user_struct.rsp_data_size));
    eips_stc_cpf.emrsp->user_struct.rsp_data_size += 2;
}

/* ====================================================================
Function:   eips_local_EnetObjIntfSpeed
C/I/A:      Ethernet Link (0xF6, 0x01, 0x01)
Settable?:  NO
======================================================================= */
static void eips_local_EnetObjIntfSpeed (void)
{
    uint32  InterfaceSpeed;
    uint8   return_code;

    /* Get Attribute Single */
    if(eips_stc_cpf.emreq->user_struct.service == CIP_SC_GET_ATTR_SINGLE)
    {
        /* get the parameter from the user */
        return_code = eips_usersock_getEthLinkObj_Attr01_IntfSpeed(&InterfaceSpeed);

        /* return_code is invalid, return it */
        if(return_code != ERR_SUCCESS)
        {
            eips_local_cpf_buildError(&eips_stc_cpf, return_code, 0, 0);
            return; /* return on error */
        }

        /* return_code successful, send the response */
        rta_PutLitEndian32(InterfaceSpeed, eips_stc_cpf.emrsp->user_struct.rsp_data);
        eips_stc_cpf.emrsp->user_struct.rsp_data_size = 4;
    }

    /* Set Attribute Single */
    else if(eips_stc_cpf.emreq->user_struct.service == CIP_SC_SET_ATTR_SINGLE)
    {
        /* "Attribute Read Only" */
        eips_local_cpf_buildError(&eips_stc_cpf, ERR_ATTR_READONLY, 0, 0);
        return; /* return on error */
    }

    /* Unsupported Service Code */
    else
    {
        /* "Service Not Supported" */
        eips_local_cpf_buildError(&eips_stc_cpf, ERR_SERV_UNSUPP, 0, 0);
        return; /* return on error */
    }
}

/* ====================================================================
Function:   eips_local_EnetObjIntfFlags
C/I/A:      Ethernet Link (0xF6, 0x01, 0x02)
Settable?:  NO
======================================================================= */
static void eips_local_EnetObjIntfFlags (void)
{
    uint32  InterfaceFlags;
    uint8   return_code;

    /* Get Attribute Single */
    if(eips_stc_cpf.emreq->user_struct.service == CIP_SC_GET_ATTR_SINGLE)
    {
        /* get the parameter from the user */
        return_code = eips_usersock_getEthLinkObj_Attr02_IntfFlags(&InterfaceFlags);

        /* return_code is invalid, return it */
        if(return_code != ERR_SUCCESS)
        {
            eips_local_cpf_buildError(&eips_stc_cpf, return_code, 0, 0);
            return; /* return on error */
        }

        /* return_code successful, send the response */
        rta_PutLitEndian32(InterfaceFlags, eips_stc_cpf.emrsp->user_struct.rsp_data);
        eips_stc_cpf.emrsp->user_struct.rsp_data_size = 4;
    }

    /* Set Attribute Single */
    else if(eips_stc_cpf.emreq->user_struct.service == CIP_SC_SET_ATTR_SINGLE)
    {
        /* "Attribute Read Only" */
        eips_local_cpf_buildError(&eips_stc_cpf, ERR_ATTR_READONLY, 0, 0);
        return; /* return on error */
    }

    /* Unsupported Service Code */
    else
    {
        /* "Service Not Supported" */
        eips_local_cpf_buildError(&eips_stc_cpf, ERR_SERV_UNSUPP, 0, 0);
        return; /* return on error */
    }
}

/* ====================================================================
Function:   eips_local_EnetObjPhyAddr
C/I/A:      Ethernet Link (0xF6, 0x01, 0x03)
Settable?:  NO
======================================================================= */
static void eips_local_EnetObjPhyAddr (void)
{
    uint8   PhysicalAddress[6];
    uint8   return_code;
    uint16  i;

    /* Get Attribute Single */
    if(eips_stc_cpf.emreq->user_struct.service == CIP_SC_GET_ATTR_SINGLE)
    {
        /* get the parameter from the user */
        return_code = eips_usersock_getEthLinkObj_Attr03_MacAddr(PhysicalAddress);

        /* return_code is invalid, return it */
        if(return_code != ERR_SUCCESS)
        {
            eips_local_cpf_buildError(&eips_stc_cpf, return_code, 0, 0);
            return; /* return on error */
        }

        /* return_code successful, send the response */
        for(i=0; i<6; i++)
            eips_stc_cpf.emrsp->user_struct.rsp_data[i] = PhysicalAddress[i];
        eips_stc_cpf.emrsp->user_struct.rsp_data_size = 6;
    }

    /* Set Attribute Single */
    else if(eips_stc_cpf.emreq->user_struct.service == CIP_SC_SET_ATTR_SINGLE)
    {
        /* "Attribute Read Only" */
        eips_local_cpf_buildError(&eips_stc_cpf, ERR_ATTR_READONLY, 0, 0);
        return; /* return on error */
    }

    /* Unsupported Service Code */
    else
    {
        /* "Service Not Supported" */
        eips_local_cpf_buildError(&eips_stc_cpf, ERR_SERV_UNSUPP, 0, 0);
        return; /* return on error */
    }
}

/* ====================================================================
Function:   eips_local_EnetObjIntfCnts
C/I/A:      Ethernet Link (0xF6, 0x01, 0x04)
Settable?:  NO
======================================================================= */
static void eips_local_EnetObjIntfCnts (void)
{
    EIPS_ELINKOBJ_INTFCNTS_STRUCT InterfaceCnts;
    uint8   return_code;

    /* Get Attribute Single */
    if(eips_stc_cpf.emreq->user_struct.service == CIP_SC_GET_ATTR_SINGLE)
    {
        /* get the parameter from the user */
        return_code = eips_usersock_getEthLinkObj_Attr04_IntfCnts(&InterfaceCnts);

        /* return_code is invalid, return it */
        if(return_code != ERR_SUCCESS)
        {
            eips_local_cpf_buildError(&eips_stc_cpf, return_code, 0, 0);
            return; /* return on error */
        }

        /* set the size to 0 */
        eips_stc_cpf.emrsp->user_struct.rsp_data_size = 0;

        /* Store the Interface Counters */
        rta_PutLitEndian32(InterfaceCnts.InOctets, (eips_stc_cpf.emrsp->user_struct.rsp_data+eips_stc_cpf.emrsp->user_struct.rsp_data_size));
        eips_stc_cpf.emrsp->user_struct.rsp_data_size += 4;

        rta_PutLitEndian32(InterfaceCnts.InUcastPackets, (eips_stc_cpf.emrsp->user_struct.rsp_data+eips_stc_cpf.emrsp->user_struct.rsp_data_size));
        eips_stc_cpf.emrsp->user_struct.rsp_data_size += 4;

        rta_PutLitEndian32(InterfaceCnts.InNUcastPackets, (eips_stc_cpf.emrsp->user_struct.rsp_data+eips_stc_cpf.emrsp->user_struct.rsp_data_size));
        eips_stc_cpf.emrsp->user_struct.rsp_data_size += 4;

        rta_PutLitEndian32(InterfaceCnts.InDiscards, (eips_stc_cpf.emrsp->user_struct.rsp_data+eips_stc_cpf.emrsp->user_struct.rsp_data_size));
        eips_stc_cpf.emrsp->user_struct.rsp_data_size += 4;

        rta_PutLitEndian32(InterfaceCnts.InErrors, (eips_stc_cpf.emrsp->user_struct.rsp_data+eips_stc_cpf.emrsp->user_struct.rsp_data_size));
        eips_stc_cpf.emrsp->user_struct.rsp_data_size += 4;

        rta_PutLitEndian32(InterfaceCnts.InUnknownProtos, (eips_stc_cpf.emrsp->user_struct.rsp_data+eips_stc_cpf.emrsp->user_struct.rsp_data_size));
        eips_stc_cpf.emrsp->user_struct.rsp_data_size += 4;

        rta_PutLitEndian32(InterfaceCnts.OutOctets, (eips_stc_cpf.emrsp->user_struct.rsp_data+eips_stc_cpf.emrsp->user_struct.rsp_data_size));
        eips_stc_cpf.emrsp->user_struct.rsp_data_size += 4;

        rta_PutLitEndian32(InterfaceCnts.OutUcastPackets, (eips_stc_cpf.emrsp->user_struct.rsp_data+eips_stc_cpf.emrsp->user_struct.rsp_data_size));
        eips_stc_cpf.emrsp->user_struct.rsp_data_size += 4;

        rta_PutLitEndian32(InterfaceCnts.OutNUcastPackets, (eips_stc_cpf.emrsp->user_struct.rsp_data+eips_stc_cpf.emrsp->user_struct.rsp_data_size));
        eips_stc_cpf.emrsp->user_struct.rsp_data_size += 4;

        rta_PutLitEndian32(InterfaceCnts.OutDiscards, (eips_stc_cpf.emrsp->user_struct.rsp_data+eips_stc_cpf.emrsp->user_struct.rsp_data_size));
        eips_stc_cpf.emrsp->user_struct.rsp_data_size += 4;

        rta_PutLitEndian32(InterfaceCnts.OutErrors, (eips_stc_cpf.emrsp->user_struct.rsp_data+eips_stc_cpf.emrsp->user_struct.rsp_data_size));
        eips_stc_cpf.emrsp->user_struct.rsp_data_size += 4;
    }

    /* Set Attribute Single */
    else if(eips_stc_cpf.emreq->user_struct.service == CIP_SC_SET_ATTR_SINGLE)
    {
        /* get the parameter from the user (to make sure the parameter exists) */
        return_code = eips_usersock_getEthLinkObj_Attr04_IntfCnts(&InterfaceCnts);

        /* return_code is invalid, return it */
        if(return_code != ERR_SUCCESS)
        {
            /* "Attribute Doesn't Exist" */
            eips_local_cpf_buildError(&eips_stc_cpf, ERR_ATTR_UNSUPP, 0, 0);
            return; /* return on error */
        }

        /* attribute exists, but is "read only" */
        else
        {
            /* "Attribute Read Only" */
            eips_local_cpf_buildError(&eips_stc_cpf, ERR_ATTR_READONLY, 0, 0);
            return; /* return on error */
        }
    }

    /* Unsupported Service Code */
    else
    {
        /* "Service Not Supported" */
        eips_local_cpf_buildError(&eips_stc_cpf, ERR_SERV_UNSUPP, 0, 0);
        return; /* return on error */
    }
}

/* ====================================================================
Function:   eips_local_EnetObjMediaCnts
C/I/A:      Ethernet Link (0xF6, 0x01, 0x05)
Settable?:  NO
======================================================================= */
static void eips_local_EnetObjMediaCnts (void)
{
    EIPS_ELINKOBJ_MEDIACNTS_STRUCT MediaCnts;
    uint8   return_code;

    /* Get Attribute Single */
    if(eips_stc_cpf.emreq->user_struct.service == CIP_SC_GET_ATTR_SINGLE)
    {
        /* get the parameter from the user */
        return_code = eips_usersock_getEthLinkObj_Attr05_MediaCnts(&MediaCnts);

        /* return_code is invalid, return it */
        if(return_code != ERR_SUCCESS)
        {
            eips_local_cpf_buildError(&eips_stc_cpf, return_code, 0, 0);
            return; /* return on error */
        }

        /* set the size to 0 */
        eips_stc_cpf.emrsp->user_struct.rsp_data_size = 0;

        /* Store the Media Counters */
        rta_PutLitEndian32(MediaCnts.AlignmentErrors, (eips_stc_cpf.emrsp->user_struct.rsp_data+eips_stc_cpf.emrsp->user_struct.rsp_data_size));
        eips_stc_cpf.emrsp->user_struct.rsp_data_size += 4;

        rta_PutLitEndian32(MediaCnts.FCSErrors, (eips_stc_cpf.emrsp->user_struct.rsp_data+eips_stc_cpf.emrsp->user_struct.rsp_data_size));
        eips_stc_cpf.emrsp->user_struct.rsp_data_size += 4;

        rta_PutLitEndian32(MediaCnts.SingleCollisions, (eips_stc_cpf.emrsp->user_struct.rsp_data+eips_stc_cpf.emrsp->user_struct.rsp_data_size));
        eips_stc_cpf.emrsp->user_struct.rsp_data_size += 4;

        rta_PutLitEndian32(MediaCnts.MultipleCollisions, (eips_stc_cpf.emrsp->user_struct.rsp_data+eips_stc_cpf.emrsp->user_struct.rsp_data_size));
        eips_stc_cpf.emrsp->user_struct.rsp_data_size += 4;

        rta_PutLitEndian32(MediaCnts.SQETestErrors, (eips_stc_cpf.emrsp->user_struct.rsp_data+eips_stc_cpf.emrsp->user_struct.rsp_data_size));
        eips_stc_cpf.emrsp->user_struct.rsp_data_size += 4;

        rta_PutLitEndian32(MediaCnts.DeferredTransmissions, (eips_stc_cpf.emrsp->user_struct.rsp_data+eips_stc_cpf.emrsp->user_struct.rsp_data_size));
        eips_stc_cpf.emrsp->user_struct.rsp_data_size += 4;

        rta_PutLitEndian32(MediaCnts.LateCollisions, (eips_stc_cpf.emrsp->user_struct.rsp_data+eips_stc_cpf.emrsp->user_struct.rsp_data_size));
        eips_stc_cpf.emrsp->user_struct.rsp_data_size += 4;

        rta_PutLitEndian32(MediaCnts.ExcessiveCollisions, (eips_stc_cpf.emrsp->user_struct.rsp_data+eips_stc_cpf.emrsp->user_struct.rsp_data_size));
        eips_stc_cpf.emrsp->user_struct.rsp_data_size += 4;

        rta_PutLitEndian32(MediaCnts.MACTransmitErrors, (eips_stc_cpf.emrsp->user_struct.rsp_data+eips_stc_cpf.emrsp->user_struct.rsp_data_size));
        eips_stc_cpf.emrsp->user_struct.rsp_data_size += 4;

        rta_PutLitEndian32(MediaCnts.CarrierSenseErrors, (eips_stc_cpf.emrsp->user_struct.rsp_data+eips_stc_cpf.emrsp->user_struct.rsp_data_size));
        eips_stc_cpf.emrsp->user_struct.rsp_data_size += 4;

        rta_PutLitEndian32(MediaCnts.FrameTooLarge, (eips_stc_cpf.emrsp->user_struct.rsp_data+eips_stc_cpf.emrsp->user_struct.rsp_data_size));
        eips_stc_cpf.emrsp->user_struct.rsp_data_size += 4;

        rta_PutLitEndian32(MediaCnts.MACReceiveErrors, (eips_stc_cpf.emrsp->user_struct.rsp_data+eips_stc_cpf.emrsp->user_struct.rsp_data_size));
        eips_stc_cpf.emrsp->user_struct.rsp_data_size += 4;
    }

    /* Set Attribute Single */
    else if(eips_stc_cpf.emreq->user_struct.service == CIP_SC_SET_ATTR_SINGLE)
    {
        /* get the parameter from the user (to make sure the parameter exists) */
        return_code = eips_usersock_getEthLinkObj_Attr05_MediaCnts(&MediaCnts);

        /* return_code is invalid, return it */
        if(return_code != ERR_SUCCESS)
        {
            /* "Attribute Doesn't Exist" */
            eips_local_cpf_buildError(&eips_stc_cpf, ERR_ATTR_UNSUPP, 0, 0);
            return; /* return on error */
        }

        /* attribute exists, but is "read only" */
        else
        {
            /* "Attribute Read Only" */
            eips_local_cpf_buildError(&eips_stc_cpf, ERR_ATTR_READONLY, 0, 0);
            return; /* return on error */
        }
    }

    /* Unsupported Service Code */
    else
    {
        /* "Service Not Supported" */
        eips_local_cpf_buildError(&eips_stc_cpf, ERR_SERV_UNSUPP, 0, 0);
        return; /* return on error */
    }
}

/* ====================================================================
Function:   eips_local_EnetObjIntfCtrl
C/I/A:      Ethernet Link (0xF6, 0x01, 0x06)
Settable?:  YES
======================================================================= */
static void eips_local_EnetObjIntfCtrl (void)
{
    EIPS_ELINKOBJ_INTFCTRL_STRUCT InterfaceControl;
    uint8   return_code;

    /* Get Attribute Single */
    if(eips_stc_cpf.emreq->user_struct.service == CIP_SC_GET_ATTR_SINGLE)
    {
        /* get the parameter from the user */
        return_code = eips_usersock_getEthLinkObj_Attr06_IntfCtrl(&InterfaceControl);

        /* return_code is invalid, return it */
        if(return_code != ERR_SUCCESS)
        {
            eips_local_cpf_buildError(&eips_stc_cpf, return_code, 0, 0);
            return; /* return on error */
        }

        /* return_code successful, send the response */
        rta_PutLitEndian16(InterfaceControl.ControlBits, eips_stc_cpf.emrsp->user_struct.rsp_data);
        rta_PutLitEndian16(InterfaceControl.ForcedInterfaceSpeed, eips_stc_cpf.emrsp->user_struct.rsp_data+2);
        eips_stc_cpf.emrsp->user_struct.rsp_data_size = 4;
    }

    /* Set Attribute Single */
    else if(eips_stc_cpf.emreq->user_struct.service == CIP_SC_SET_ATTR_SINGLE)
    {
        /* get the parameter from the user (to make sure the parameter exists) */
        return_code = eips_usersock_getEthLinkObj_Attr06_IntfCtrl(&InterfaceControl);

        /* return_code is invalid, return it */
        if(return_code != ERR_SUCCESS)
        {
            /* "Attribute Doesn't Exist" */
            eips_local_cpf_buildError(&eips_stc_cpf, ERR_ATTR_UNSUPP, 0, 0);
            return; /* return on error */
        }

        /* size must be 4 */
        if(eips_stc_cpf.emreq->user_struct.req_data_size < 4)
        {
            /* Error: Too Much Data */
            eips_local_cpf_buildError(&eips_stc_cpf, ERR_INSUFF_DATA, 0, 0);
            return; /* exit on an error */
        }
        if(eips_stc_cpf.emreq->user_struct.req_data_size > 4)
        {
            /* Error: Too Much Data */
            eips_local_cpf_buildError(&eips_stc_cpf, ERR_TOOMUCH_DATA, 0, 0);
            return; /* exit on an error */
        }

        /* store the data from the client */
        InterfaceControl.ControlBits = rta_GetLitEndian16(eips_stc_cpf.emreq->user_struct.req_data);
        InterfaceControl.ForcedInterfaceSpeed = rta_GetLitEndian16(eips_stc_cpf.emreq->user_struct.req_data+2);

        /* pass the request to the user */
        return_code = eips_usersock_setEthLinkObj_Attr06_IntfCtrl(&InterfaceControl);

        /* return the error (if needed) */
        if(return_code != ERR_SUCCESS)
        {
            eips_local_cpf_buildError(&eips_stc_cpf, return_code, 0, 0);
            return; /* return on error */
        }
        return; /* return - Successful */
    }

    /* Unsupported Service Code */
    else
    {
        /* "Service Not Supported" */
        eips_local_cpf_buildError(&eips_stc_cpf, ERR_SERV_UNSUPP, 0, 0);
        return; /* return on error */
    }
}

/* ====================================================================
Function:   eips_local_cpf_buildError
Parameters: pointer to CPF message structure
            general status
            extended status size (0 or 1)
            extended status code
Returns:    N/A

This function builds the error message for all CPF message.
======================================================================= */
static void eips_local_cpf_buildError (CPF_MESSAGE_STRUCT *cpf, uint8 gen_stat, uint8 ext_size, uint16 ext_err)
{
    /* validate we have a cpf pointer */
    if(cpf == NULL)
        return;

    /* store the general response message */
    cpf->emrsp->user_struct.gen_stat = gen_stat;
    
    /* make sure the extended status size is 0 or 1 */
    if (ext_size > 1)
        ext_size = 1;

    /* store the extended status */
    cpf->emrsp->user_struct.ext_stat_size = ext_size;
    cpf->emrsp->user_struct.ext_stat[0] = ext_err;
}

/* *********** */
/* END OF FILE */
/* *********** */
