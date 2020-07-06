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
 *     Module Name: eips_userdf1.c
 *          Author: Jamin D. Wendorf (jwendorf@rtaautomation.com)
 *        Language: Ansi C
 * Compile Options: N/A
 * Compile defines: N/A
 *       Libraries: N/A
 *    Link Options: N/A
 *
 * Description.
 * =======================================================================
 * This file contains the user functions needed for EtherNet/IP Server
 * DF1 processing.
 *
 */

/* ---------------------------- */
/* INCLUDE FILES                */
/* ---------------------------- */
#include "eips_system.h"

/* see if this module is used (this line must be after the includes) */
#ifdef EIPS_PCCC_USED

/* ---------------------------- */
/* LOCAL FUNCTION PROTOTYPES	*/
/* ---------------------------- */
static uint16 local_userdf1_typed_read (uint8 * req_msg, uint16 req_msg_size, uint8 * rsp_msg, uint16 max_rsp_msg_size);
static uint16 local_userdf1_typed_write (uint8 * req_msg, uint16 req_msg_size, uint8 * rsp_msg, uint16 max_rsp_msg_size);
static uint16 local_userdf1_block_read (uint8 * req_msg, uint16 req_msg_size, uint8 * rsp_msg, uint16 max_rsp_msg_size);
static uint16 local_userdf1_block_write (uint8 * req_msg, uint16 req_msg_size, uint8 * rsp_msg, uint16 max_rsp_msg_size);
static uint16 local_userdf1_rd_mod_wr_bit (uint8 * req_msg, uint16 req_msg_size, uint8 * rsp_msg, uint16 max_rsp_msg_size);
static uint16 local_userdf1_build_rspmsg (uint8 * rsp_msg, uint8 STS, uint8 EXT_STS);

/* ---------------------------- */
/* LOCAL VARIABLES              */
/* ---------------------------- */
/* used to parse the request message */
#define ACIP_SC_BUF_SIZ 10
struct
{
    uint8  command;             /* command, must be 0x0F */
    uint8  STS;                 /* status */
    uint16 TNS;                 /* transaction number */
    uint8  FNC;                 /* function code  ("Typed Read" - 0x68 /  "Typed Write" - 0x67") */
    uint16 pack_offset;         /* packet offset, must be 0 */
    uint16 total_trans;         /* total number of encoded transactions, must be 1 */
    uint16 num_regs;            /* number of regs to read/write */
    uint8  ascii_file_type[ACIP_SC_BUF_SIZ];  /* used on ASCII addresses */
    uint16 file_num;            /* forwarded to user (i.e. N7:3 would be 7) */
    uint16 element_num;         /* element number (i.e.  N7:3 would be 3) */
}DF1_REQ;

#define DF1_MAX_RDMOD_WR_REQ 30 /* (243 bytes / 8) = 30 */
struct
{
    uint8  ascii_file_type[ACIP_SC_BUF_SIZ];  /* used on ASCII addresses */
    uint16 file_num;            /* forwarded to user (i.e. N7:3 would be 7) */
    uint16 element_num;         /* element number (i.e.  N7:3 would be 3) */
    uint16 AND_mask;            /* AND Mask */
    uint16 OR_mask;             /* OR Mask */
    /* New Reg Value = ((Old Reg Value & AND_MASK) | OF_Mask); */
}DF1_RDMODWR_REQ[DF1_MAX_RDMOD_WR_REQ];

/* ---------------------------- */
/* MISCELLANEOUS                */
/* ---------------------------- */
#define DF1_CMD             0x0F
#define DF1_FNC_READ        0x68
#define DF1_FNC_WRITE       0x67
#define DF1_FNC_BLOCK_READ  0x01
#define DF1_FNC_BLOCK_WRITE 0x00
#define DF1_FNC_RDMODWR_BIT 0x26

/* ---------------------------- */
/* GLOBAL VARIABLES	            */
/* ---------------------------- */
/* temp data unions */
#define MAX_NUM_BYTES   252 /* 252 bytes MAX */

#define MAX_NUM_BITS    126 /* 126*2 = 252 bytes      */
#define MAX_NUM_INTS    126 /* 126*2 = 252 bytes      */
#define MAX_NUM_FLOATS  63  /* 63*4  = 252 bytes  */
#define MAX_NUM_STRINGS 3   /* 84*3  = 252 bytes  */

union
{
    uint16                  bits[MAX_NUM_BITS];
    uint16                  ints[MAX_NUM_INTS];
    float                   floats[MAX_NUM_FLOATS];
    EIPS_PCCC_STRING_TYPE   strings[MAX_NUM_STRINGS];
}pccc_temp_data;

/**/
/* ******************************************************************** */
/*                      GLOBAL FUNCTIONS                                */
/* ******************************************************************** */
/* ====================================================================
Function:   eips_userdf1_init
Parameters: init type
Returns:    N/A

This function initialize all user PCCC variables.
======================================================================= */
void eips_userdf1_init (uint8 init_type)
{
    if(init_type){} /* keep the compiler happy */
}

/* ====================================================================
Function:   eips_userdf1_proc_msg
Parameters: pointer to request message
            request message size
            pointer to response message
            max response message length
Returns:    actual response message length

This function processes the DF1 Command.
======================================================================= */
uint16 eips_userdf1_proc_msg (uint8 * req_msg, uint16 req_msg_size, uint8 * rsp_msg, uint16 max_rsp_msg_size)
{
    uint16  msg_ix, i, j, asc_ix;
    uint16  rsp_size;
 
    /* used to parse ASCII Address */
    char ascii_str[20];
    uint16 ascii_str_len;

    /* used to parse Binary Address */
    uint8 mask;

    /* initialize pameters */
    memset(&DF1_REQ, 0, sizeof(DF1_REQ));
    memset(&DF1_RDMODWR_REQ, 0, sizeof(DF1_RDMODWR_REQ));
    memset(&pccc_temp_data, 0, sizeof(pccc_temp_data));
    msg_ix = 0;   
    rsp_size = 0;

    /* store the command byte (should be 0x0f) */
    DF1_REQ.command = req_msg[msg_ix++];

    /* store the STS (should be 0) */
    DF1_REQ.STS = req_msg[msg_ix++];

    /* store the TNS */
    DF1_REQ.TNS = rta_GetLitEndian16(req_msg + msg_ix);
    msg_ix += 2;

    /* *************************** */
    /* validate the message so far */
    /* *************************** */

    /* validate the command is correct */
    if(DF1_REQ.command != PCCC_CMD)
    {
        eips_user_dbprint1("PCCC-> CMD Error (0x%02x)\r\n", DF1_REQ.command);
        return(local_userdf1_build_rspmsg (rsp_msg, STSERR_ILLCMD, EXTSTS_NONE));
    }

    /* store the FNC */
    DF1_REQ.FNC = req_msg[msg_ix++];

    /* ********************************************* */
    /*          Read Modify Write */
    /* ********************************************* */
    if(DF1_REQ.FNC == DF1_FNC_RDMODWR_BIT)
    {
        j = 0;

        while(msg_ix < req_msg_size)
        {
            /* *********************** */
            /* parse the ASCII address */
            /* *********************** */

            /* first byte should be NULL */
            if(req_msg[msg_ix] == 0)
            {
                /* increment the message index */
                msg_ix++;

                for(i=0, ascii_str_len=0; i<19 && req_msg[msg_ix] != 0; i++)
	                ascii_str[ascii_str_len++] = req_msg[msg_ix++];

                /* NULL terminate the string */
                ascii_str[ascii_str_len] = 0;
                msg_ix++; /* advace the pointer to accept the last NULL char */

                /* the first byte should be '$' */
                i=0;
                if(ascii_str[i++] != '$')
                {
	                /* bad symbol format */
	                return(local_userdf1_build_rspmsg (rsp_msg, STSERR_EXTSTS, EXTSTS_SYMBOLBADFORMAT));
                }

                /* store characters until we a number */
                for(asc_ix=0; (asc_ix<ACIP_SC_BUF_SIZ) && !((ascii_str[i] > '0') && (ascii_str[i] < '9')); asc_ix++)
                {      
                    /* the second byte is the file type */
                    DF1_RDMODWR_REQ[j].ascii_file_type[asc_ix] = ascii_str[i++];
                }

                /* the first ascii number is the file number */
                sscanf(&ascii_str[i], "%hd", &DF1_RDMODWR_REQ[j].file_num);

                /* find the colon */
                for(; i<ascii_str_len; i++)
                {
	                /* the second ascii number is the element number */
	                if(ascii_str[i] == ':')
	                {
	                    i++;
	                    sscanf(&ascii_str[i], "%hd", &DF1_RDMODWR_REQ[j].element_num);
	                    i = ascii_str_len;
	                }
                }
            }

            /* ************************ */
            /* parse the binary address */
            /* ************************ */
            else
            {
                /* not used in this mode */
                DF1_RDMODWR_REQ[j].ascii_file_type[0] = 0;

                /* store the mask and advance the index */
                mask = req_msg[msg_ix++];

                /* see if we have too many levels */
                if (mask > 0x07)
                {
	                /* too many levels */
	                return(local_userdf1_build_rspmsg (rsp_msg, STSERR_EXTSTS, EXTSTS_TOOMANYLEVELS));
                }

                /* we need upto 3 levels of addressing */
                if(mask & 0x07)
                {
	                /* Level 1 (Data Table if 0) */
	                if(mask & 0x01)
	                {  
	                    /* should be 0 (data table) */
	                    if(req_msg[msg_ix++] != 0)
	                    {
	                        /* bad format */
	                        return(local_userdf1_build_rspmsg (rsp_msg, STSERR_EXTSTS, EXTSTS_SYMBOLBADFORMAT));
	                    }
	                }

	                /* Level 2 (File Number) */
	                if(mask & 0x02)
	                {
	                    DF1_RDMODWR_REQ[j].file_num = req_msg[msg_ix++];

	                    /* get a 16-bit number if needed */
	                    if(DF1_RDMODWR_REQ[j].file_num == 255)
	                    {
	                        DF1_RDMODWR_REQ[j].file_num = rta_GetLitEndian16(req_msg + msg_ix);
	                        msg_ix += 2;
	                    }
	                }

	                /* we need Level 2 */
	                else
	                {
	                    /* too few levels */
	                    return(local_userdf1_build_rspmsg (rsp_msg, STSERR_EXTSTS, EXTSTS_TOOFEWLEVELS));
	                }

	                /* Level 3 (Element Number) */
	                if(mask & 0x04)
	                {
	                    DF1_RDMODWR_REQ[j].element_num = req_msg[msg_ix++];

	                    /* get a 16-bit number if needed */
	                    if(DF1_RDMODWR_REQ[j].element_num == 255)
	                    {
	                        DF1_RDMODWR_REQ[j].element_num = rta_GetLitEndian16(req_msg + msg_ix);
	                        msg_ix += 2;
	                    }
	                }

	                /* we need Level 3 */
	                else
	                {
                        /* too few levels */
	                    return(local_userdf1_build_rspmsg (rsp_msg, STSERR_EXTSTS, EXTSTS_TOOFEWLEVELS));
	                }
                }

                /* see if we have too few levels */
                else
                {
	                /* too few levels */
	                return(local_userdf1_build_rspmsg (rsp_msg, STSERR_EXTSTS, EXTSTS_TOOFEWLEVELS));
                }
            }

            /* Store the AND Mask */
            DF1_RDMODWR_REQ[j].AND_mask = rta_GetLitEndian16(req_msg + msg_ix);
            msg_ix += 2;

            /* Store the OR Mask */
            DF1_RDMODWR_REQ[j].OR_mask = rta_GetLitEndian16(req_msg + msg_ix);
            msg_ix += 2;

            /* Advance the index */
            j++;

            /* store the total transaction */
            DF1_REQ.total_trans = j;
        }        
    }

    /* ********************************************* */
    /*          All Other Commands  */
    /* ********************************************* */
    else
    {
        /* store the packet offset (should be 0) */
        DF1_REQ.pack_offset = rta_GetLitEndian16(req_msg + msg_ix);
        msg_ix += 2;

        /* validate the packet offset is 0 */
        if(DF1_REQ.pack_offset != 0)
        {
            /* error with field */
            eips_user_dbprint1("PCCC-> Packet Offset Error (%d)\r\n", DF1_REQ.pack_offset);
            return(local_userdf1_build_rspmsg (rsp_msg, STSERR_EXTSTS, EXTSTS_FIELDILLVAL));
        }

        /* store the total transaction (should be 1) */
        DF1_REQ.total_trans = rta_GetLitEndian16(req_msg + msg_ix);
        msg_ix += 2;

        /* *********************** */
        /* parse the ASCII address */
        /* *********************** */

        /* first byte should be NULL */
        if(req_msg[msg_ix] == 0)
        {
            /* increment the message index */
            msg_ix++;

            for(i=0, ascii_str_len=0; i<19 && req_msg[msg_ix] != 0; i++)
	            ascii_str[ascii_str_len++] = req_msg[msg_ix++];

            /* NULL terminate the string */
            ascii_str[ascii_str_len] = 0;
            msg_ix++; /* advace the pointer to accept the last NULL char */

            /* the first byte should be '$' */
            i=0;
            if(ascii_str[i++] != '$')
            {
	            /* bad symbol format */
	            return(local_userdf1_build_rspmsg (rsp_msg, STSERR_EXTSTS, EXTSTS_SYMBOLBADFORMAT));
            }

            /* store characters until we a number */
            for(asc_ix=0; (asc_ix<ACIP_SC_BUF_SIZ) && !((ascii_str[i] > '0') && (ascii_str[i] < '9')); asc_ix++)
            {      
                /* the second byte is the file type */
                DF1_REQ.ascii_file_type[asc_ix] = ascii_str[i++];
            }

            /* the first ascii number is the file number */
            sscanf(&ascii_str[i], "%hd", &DF1_REQ.file_num);

            /* find the colon */
            for(; i<ascii_str_len; i++)
            {
	            /* the second ascii number is the element number */
	            if(ascii_str[i] == ':')
	            {
	                i++;
	                sscanf(&ascii_str[i], "%hd", &DF1_REQ.element_num);
	                i = ascii_str_len;
	            }
            }
        }

        /* ************************ */
        /* parse the binary address */
        /* ************************ */
        else
        {
            /* not used in this mode */
            DF1_REQ.ascii_file_type[0] = 0;

            /* store the mask and advance the index */
            mask = req_msg[msg_ix++];

            /* see if we have too many levels */
            if (mask > 0x07)
            {
	            /* too many levels */
	            return(local_userdf1_build_rspmsg (rsp_msg, STSERR_EXTSTS, EXTSTS_TOOMANYLEVELS));
            }

            /* we need upto 3 levels of addressing */
            if(mask & 0x07)
            {
	            /* Level 1 (Data Table if 0) */
	            if(mask & 0x01)
	            {  
	                /* should be 0 (data table) */
	                if(req_msg[msg_ix++] != 0)
	                {
	                    /* bad format */
	                    return(local_userdf1_build_rspmsg (rsp_msg, STSERR_EXTSTS, EXTSTS_SYMBOLBADFORMAT));
	                }
	            }

	            /* Level 2 (File Number) */
	            if(mask & 0x02)
	            {
	                DF1_REQ.file_num = req_msg[msg_ix++];

	                /* get a 16-bit number if needed */
	                if(DF1_REQ.file_num == 255)
	                {
	                    DF1_REQ.file_num = rta_GetLitEndian16(req_msg + msg_ix);
	                    msg_ix += 2;
	                }
	            }

	            /* we need Level 2 */
	            else
	            {
	                /* too few levels */
	                return(local_userdf1_build_rspmsg (rsp_msg, STSERR_EXTSTS, EXTSTS_TOOFEWLEVELS));
	            }

	            /* Level 3 (Element Number) */
	            if(mask & 0x04)
	            {
	                DF1_REQ.element_num = req_msg[msg_ix++];

	                /* get a 16-bit number if needed */
	                if(DF1_REQ.element_num == 255)
	                {
	                    DF1_REQ.element_num = rta_GetLitEndian16(req_msg + msg_ix);
	                    msg_ix += 2;
	                }
	            }

	            /* we need Level 3 */
	            else
	            {
                    /* too few levels */
	                return(local_userdf1_build_rspmsg (rsp_msg, STSERR_EXTSTS, EXTSTS_TOOFEWLEVELS));
	            }
            }

            /* see if we have too few levels */
            else
            {
	            /* too few levels */
	            return(local_userdf1_build_rspmsg (rsp_msg, STSERR_EXTSTS, EXTSTS_TOOFEWLEVELS));
            }
        }
    }

    /* switch on the function code */
    switch(DF1_REQ.FNC)
    {
        /* PLC5 Typed Read */
        case DF1_FNC_READ:
            return(local_userdf1_typed_read(req_msg+msg_ix, (uint16)(req_msg_size-msg_ix), rsp_msg, max_rsp_msg_size));
            
        /* PLC5 Typed Write */
        case DF1_FNC_WRITE:
            return(local_userdf1_typed_write(req_msg+msg_ix, (uint16)(req_msg_size-msg_ix), rsp_msg, max_rsp_msg_size));

        /* PLC5 Word Range Read */
        case DF1_FNC_BLOCK_READ:
            return(local_userdf1_block_read(req_msg+msg_ix, (uint16)(req_msg_size-msg_ix), rsp_msg, max_rsp_msg_size));

        /* PLC5 Word Range Write */
        case DF1_FNC_BLOCK_WRITE:
            return(local_userdf1_block_write(req_msg+msg_ix, (uint16)(req_msg_size-msg_ix), rsp_msg, max_rsp_msg_size));

        /* PLC5 Read Modify-Write Bit */
        case DF1_FNC_RDMODWR_BIT:
            return(local_userdf1_rd_mod_wr_bit(req_msg+msg_ix, (uint16)(req_msg_size-msg_ix), rsp_msg, max_rsp_msg_size));
    };
    
    /* error with field */
    eips_user_dbprint1("PCCC-> FNC Error (0x%02x)\r\n", DF1_REQ.FNC);
    return(local_userdf1_build_rspmsg (rsp_msg, STSERR_EXTSTS, EXTSTS_FIELDILLVAL));
}

/**/
/* ******************************************************************** */
/*                      LOCAL FUNCTIONS                                 */
/* ******************************************************************** */
/* ====================================================================
Function:   local_userdf1_typed_read
Parameters: pointer to request message
            request message size
            pointer to response message
            max response message length
Returns:    actual response message length

This function processes the PLC5 "Typed Read" Command.
======================================================================= */
static uint16 local_userdf1_typed_read (uint8 * req_msg, uint16 req_msg_size, uint8 * rsp_msg, uint16 max_rsp_msg_size)
{
    uint16  msg_ix, i, j;
    uint8   ext_sts_err_b, ext_sts_err_n, ext_sts_err_f, ext_sts_err_st;
    uint8   msg_found;
    uint16  rsp_size;

    RTA_UNUSED_PARAM(max_rsp_msg_size);

/*    eips_user_dbprint3("Typed Read %s%d:%d\r\n",DF1_REQ.ascii_file_type, DF1_REQ.file_num, DF1_REQ.element_num); */

    /* set the default values */
    msg_ix         = 0;
    msg_found      = FALSE;
    ext_sts_err_n  = EXTSTS_BADADDRESS;
    ext_sts_err_f  = EXTSTS_BADADDRESS;
    ext_sts_err_b  = EXTSTS_BADADDRESS;
    ext_sts_err_st = EXTSTS_BADADDRESS;

    /* store the Number of Registers */
    DF1_REQ.num_regs = rta_GetLitEndian16(req_msg + msg_ix);
    msg_ix += 2;

    /* validate the Total Transaction is the same as the number of regs */
    if(DF1_REQ.total_trans != DF1_REQ.num_regs)
    {
	    /* error with field */
        eips_user_dbprint2("PCCC-> TotalTrans/NumReg Mismatch Offset Error (%d, %d)\r\n", DF1_REQ.total_trans, DF1_REQ.num_regs);
	    return(local_userdf1_build_rspmsg (rsp_msg, STSERR_EXTSTS, EXTSTS_FIELDILLVAL));
    }

    /* validate the request length is correct */
    if(msg_ix < req_msg_size)
    {
	    /* Error: Too Much Data */
        eips_pccc_build_error(ERR_TOOMUCH_DATA,0,NULL);
	    return(0);
    }
    if(msg_ix > req_msg_size)
    {
	    /* Error: Not Enough Data */
        eips_pccc_build_error(ERR_INSUFF_DATA,0,NULL);
	    return(0);
    }

    /* see if this is a bit request */
    if(msg_found == FALSE)
    {
        ext_sts_err_b = eips_userobj_read_B_reg_pccc (DF1_REQ.file_num, DF1_REQ.element_num, DF1_REQ.num_regs, pccc_temp_data.bits);

        /* we found the register */
        if(ext_sts_err_b != EXTSTS_BADADDRESS)
            msg_found =  TRUE;
    }

    /* see if this is an integer request */
    if(msg_found == FALSE)
    {
        ext_sts_err_n = eips_userobj_read_N_reg_pccc (DF1_REQ.file_num, DF1_REQ.element_num, DF1_REQ.num_regs, pccc_temp_data.ints);

        /* we found the register */
        if(ext_sts_err_n != EXTSTS_BADADDRESS)
            msg_found =  TRUE;
    }

    /* see if this is a float request */
    if(msg_found == FALSE)
    {
        ext_sts_err_f = eips_userobj_read_F_reg_pccc (DF1_REQ.file_num, DF1_REQ.element_num, DF1_REQ.num_regs, pccc_temp_data.floats);

        /* we found the register */
        if(ext_sts_err_f != EXTSTS_BADADDRESS)
            msg_found =  TRUE;
    }

    /* see if this is a string request */
    if(msg_found == FALSE)
    {
        ext_sts_err_st = eips_userobj_read_ST_reg_pccc (DF1_REQ.file_num, DF1_REQ.element_num, DF1_REQ.num_regs, pccc_temp_data.strings);

        /* we found the register */
        if(ext_sts_err_st != EXTSTS_BADADDRESS)
            msg_found =  TRUE;
    }

    /* build the error message (if needed) */
    if(msg_found == FALSE)
    {
        /* register type isn't found */
        return(local_userdf1_build_rspmsg (rsp_msg, STSERR_EXTSTS, EXTSTS_BADADDRESS));
    }

    /* ****************** */
    /* Build the Response */
    /* ****************** */
    i=0;

    /* return the command byte | 0x40 */
    rsp_msg[i++] = (uint8)(DF1_REQ.command | 0x40);

    /* return the status (0 on success) */
    rsp_msg[i++] = STSERR_SUCCESS;

    /* echo the TNS */
    rta_PutLitEndian16(DF1_REQ.TNS, (rsp_msg+i));
    i+=2;

    /* Bit Data */
    if(ext_sts_err_b == EXTSTS_NONE)
    {
        /* store the type of data */
        rsp_msg[i++] = 0x99; /* 1001 1001 (1 byte to follow for type; 1 byte to follow fo len) */
        rsp_msg[i++] = 0x09; /* type = 9 --> array */
        rsp_msg[i++] = (uint8)((DF1_REQ.num_regs*2)+1); /* length is data length + 1 (for element type) */
        rsp_msg[i++] = 0x42; /* element type is 0x42 (Bit (Int), 2 bytes per element) */

        /* store the data */
        for(j=0; j<DF1_REQ.num_regs; j++)
        {
            rta_PutLitEndian16(pccc_temp_data.bits[j], (rsp_msg+i));
            i+=2;
        }

        /* store the data size and return */
        rsp_size = i;
        return(rsp_size);
    }

    /* Integer Data */
    if(ext_sts_err_n == EXTSTS_NONE)
    {
        /* store the type of data */
        rsp_msg[i++] = 0x99; /* 1001 1001 (1 byte to follow for type; 1 byte to follow fo len) */
        rsp_msg[i++] = 0x09; /* type = 9 --> array */
        rsp_msg[i++] = (uint8)((DF1_REQ.num_regs*2)+1); /* length is data length + 1 (for element type) */
        rsp_msg[i++] = 0x42; /* element type is 0x42 (Integer, 2 bytes per element) */

        /* store the data */
        for(j=0; j<DF1_REQ.num_regs; j++)
        {
            rta_PutLitEndian16(pccc_temp_data.ints[j], (rsp_msg+i));
            i+=2;
        }

        /* store the data size and return */
        rsp_size = i;
        return(rsp_size);
    }

    /* Floating Point Data */
    if(ext_sts_err_f == EXTSTS_NONE)
    {
        /* store the type of data */
        rsp_msg[i++] = 0x99; /* 1001 1001 (1 byte to follow for type; 1 byte to follow fo len) */
        rsp_msg[i++] = 0x09; /* type = 9 --> array */
        rsp_msg[i++] = (uint8)((DF1_REQ.num_regs*4)+2); /* length is data length + 1 (for element type) */
        rsp_msg[i++] = 0x94; /* element type is 0x94, 0x08 (Float, 4 bytes per element) */
        rsp_msg[i++] = 0x08; 

        /* store the data */
        for(j=0; j<DF1_REQ.num_regs; j++)
        {
            rta_PutLitEndianFloat(pccc_temp_data.floats[j], (rsp_msg+i));
            i+=4;
        }

        /* store the data size and return */
        rsp_size = i;
        return(rsp_size);
    }

    /* String Data */
    if(ext_sts_err_st == EXTSTS_NONE)
    {   
        /* store the data */
        for(j=0; j<DF1_REQ.num_regs; j++)
        {
            rsp_msg[i++] = 0x39; /* 0011 1001 (byte (or char) string; 1 byte to follow fo len) */
            rsp_msg[i++] = (uint8)pccc_temp_data.strings[j].string_size; /* length of array */

            /* store the data  */
            memcpy(&rsp_msg[i], pccc_temp_data.strings[j].string_data, pccc_temp_data.strings[j].string_size);
            i = (uint16)(i + pccc_temp_data.strings[j].string_size);
        }        

        /* store the data size and return */
        rsp_size = i;
        return(rsp_size);
    }

    /* send a default error (if needed) */
    return(local_userdf1_build_rspmsg (rsp_msg, STSERR_EXTSTS, EXTSTS_BADADDRESS));
}

/* ====================================================================
Function:   local_userdf1_typed_write
Parameters: pointer to request message
            request message size
            pointer to response message
            max response message length
Returns:    actual response message length

This function processes the PLC5 "Typed Write" Command. Only one string
can be written at a time, but all other types support multiple writes.
======================================================================= */
static uint16 local_userdf1_typed_write (uint8 * req_msg, uint16 req_msg_size, uint8 * rsp_msg, uint16 max_rsp_msg_size)
{
    uint8   ext_sts_err_b, ext_sts_err_n, ext_sts_err_f, ext_sts_err_st;
    uint8   msg_found;
    uint16  msg_ix, temp_size, write_type, i;

    RTA_UNUSED_PARAM(max_rsp_msg_size);
    
/*    eips_user_dbprint3("Typed Write %s%d:%d\r\n",DF1_REQ.ascii_file_type, DF1_REQ.file_num, DF1_REQ.element_num); */

    /* set the default values */
    msg_ix         = 0;
    msg_found      = FALSE;
    ext_sts_err_n  = EXTSTS_BADADDRESS;
    ext_sts_err_f  = EXTSTS_BADADDRESS;
    ext_sts_err_b  = EXTSTS_BADADDRESS;
    ext_sts_err_st = EXTSTS_BADADDRESS;

    /* we need at least 2 bytes */
    if(req_msg_size < 2)
    {
	    /* Error: Not Enough Data */
        eips_pccc_build_error(ERR_INSUFF_DATA,0,NULL);
	    return(0);
    }

    /*  ------------------- */
    /*  STRING data format */
    /*  ------------------- */
    /*  0x39    data type     */
    /*  0xnn    number of bytes */
    if(req_msg[msg_ix] == 0x39)
    {
        /* advance the index */
        msg_ix++;

        /* the second byte is the length */
        pccc_temp_data.strings[0].string_size = req_msg[msg_ix++];

        /* validate we only have 1 request (and enough data) */
        if((pccc_temp_data.strings[0].string_size + 2) < req_msg_size)
        {
	        /* Error: Not Enough Data */
            eips_pccc_build_error(ERR_INSUFF_DATA,0,NULL);
	        return(0);
        }
        else if((pccc_temp_data.strings[0].string_size + 2) > req_msg_size)
        {
	        /* Error: Too Much Data */
            eips_pccc_build_error(ERR_TOOMUCH_DATA,0,NULL);
	        return(0);
        }

        /* store the data */
        memcpy(pccc_temp_data.strings[0].string_data, &req_msg[msg_ix], pccc_temp_data.strings[0].string_size);

        /* pass the request to the user  */
        ext_sts_err_st = eips_userobj_write_ST_reg_pccc (DF1_REQ.file_num, DF1_REQ.element_num, 1, pccc_temp_data.strings);

        /* error received */
        if(ext_sts_err_st != EXTSTS_NONE)
        {
            return(local_userdf1_build_rspmsg (rsp_msg, STSERR_EXTSTS, ext_sts_err_n));
        }

        /* build the success response */
        else
        {
            return(local_userdf1_build_rspmsg (rsp_msg, STSERR_SUCCESS, EXTSTS_NONE));
        }
    }

    /*  ------------------- */
    /*  BIT data format */
    /*  ------------------- */
    /*  0x99    data type     */
    /*  0x09    array */
    /*  0xnn    number of bytes */
    /*  0x42    integer data type */

    /*  ------------------- */
    /*  INTEGER data format */
    /*  ------------------- */
    /*  0x99    data type     */
    /*  0x09    array */
    /*  0xnn    number of bytes */
    /*  0x42    integer data type */

    /*  ------------------- */
    /*  FLOAT data format */
    /*  ------------------- */
    /*  0x99    data type     */
    /*  0x09    array */
    /*  0xnn    number of bytes */
    /*  0x94    float data type */
    /*  0x08 */
    else if(req_msg[msg_ix] == 0x99)
    {
        /* advance the index */
        msg_ix++;

        /* type should be array */
        if(req_msg[msg_ix++] != 0x09) /* 1 byte of each to follow */
        {
            /* bad symbol format */
            return(local_userdf1_build_rspmsg (rsp_msg, STSERR_EXTSTS, EXTSTS_SYMBOLBADFORMAT));
        }

        /* get the length in bytes */
        temp_size = req_msg[msg_ix++];
        write_type = req_msg[msg_ix++];

        /* switch on the data type */
        switch(write_type)
        {
            /* Integer or Bit */
            case 0x42:

                /* store the number of registers */
                DF1_REQ.num_regs = (uint16)((temp_size - 1)/2);
                if(DF1_REQ.num_regs > MAX_NUM_INTS)
                {
                    /* bad symbol format */
                    return(local_userdf1_build_rspmsg (rsp_msg, STSERR_EXTSTS, EXTSTS_SYMBOLBADFORMAT));
                }

                /* store the data to a temp buffer */
                for(i=0; i<DF1_REQ.num_regs; i++)
                {
                    pccc_temp_data.ints[i] = rta_GetLitEndian16(req_msg + msg_ix);
                    msg_ix += 2;
                }
                break;

            /* Float  */
            case 0x94: 
                /* next byte should be 0x08 */
                if(req_msg[msg_ix++] != 8)
                {
                    /* bad symbol format */
                    return(local_userdf1_build_rspmsg (rsp_msg, STSERR_EXTSTS, EXTSTS_SYMBOLBADFORMAT));
                }

                /* store the number of registers */
                DF1_REQ.num_regs = (uint16)((temp_size - 2)/4);
                if(DF1_REQ.num_regs > MAX_NUM_FLOATS)
                {
                    /* bad symbol format */
                    return(local_userdf1_build_rspmsg (rsp_msg, STSERR_EXTSTS, EXTSTS_SYMBOLBADFORMAT));
                }

                /* store the data to a temp buffer */
                for(i=0; i<DF1_REQ.num_regs; i++)
                {
                    pccc_temp_data.floats[i] = rta_GetLitEndianFloat(req_msg + msg_ix);
                    msg_ix += 4;
                }
                break;

            default:
                /* bad symbol format */
                return(local_userdf1_build_rspmsg (rsp_msg, STSERR_EXTSTS, EXTSTS_SYMBOLBADFORMAT));
        }

        /* validate the request length is correct */
        if(msg_ix < req_msg_size)
        {
            /* Error: Too Much Data */
            eips_pccc_build_error(ERR_TOOMUCH_DATA,0,NULL);
            return(0);
        }
        if(msg_ix > req_msg_size)
        {
            /* Error: Not Enough Data */
            eips_pccc_build_error(ERR_INSUFF_DATA,0,NULL);
            return(0);
        }

        /* switch on the data type */
        switch(write_type)
        {
            /* Integer or Bit */
            case 0x42:
                ext_sts_err_n = eips_userobj_write_N_reg_pccc (DF1_REQ.file_num, DF1_REQ.element_num, DF1_REQ.num_regs, pccc_temp_data.ints);

                /* if the address isn't found, try bits (still use int temp data) */
                if(ext_sts_err_n == EXTSTS_BADADDRESS)
                {
                    ext_sts_err_b = eips_userobj_write_B_reg_pccc (DF1_REQ.file_num, DF1_REQ.element_num, DF1_REQ.num_regs, pccc_temp_data.ints);
 
                    if(ext_sts_err_b != EXTSTS_NONE)
                    {
                        return(local_userdf1_build_rspmsg (rsp_msg, STSERR_EXTSTS, ext_sts_err_b));
                    } 
               }

                /* error received */
                else if(ext_sts_err_n != EXTSTS_NONE)
                {
                    return(local_userdf1_build_rspmsg (rsp_msg, STSERR_EXTSTS, ext_sts_err_n));
                }
                break;

            /* Float */
            case 0x94: 
                ext_sts_err_f = eips_userobj_write_F_reg_pccc (DF1_REQ.file_num, DF1_REQ.element_num, DF1_REQ.num_regs, pccc_temp_data.floats);

                /* error received */
                if(ext_sts_err_f != EXTSTS_NONE)
                {
                    return(local_userdf1_build_rspmsg (rsp_msg, STSERR_EXTSTS, ext_sts_err_f));
                }
                break;

            default:
                /* bad symbol format */
                return(local_userdf1_build_rspmsg (rsp_msg, STSERR_EXTSTS, EXTSTS_SYMBOLBADFORMAT));
        };

        /* build the response */
        return(local_userdf1_build_rspmsg (rsp_msg, STSERR_SUCCESS, EXTSTS_NONE));
    }

    /* error on data type */
    else
    {
        /* bad symbol format */
        return(local_userdf1_build_rspmsg (rsp_msg, STSERR_EXTSTS, EXTSTS_SYMBOLBADFORMAT));
    }        
}

/* ====================================================================
Function:   local_userdf1_block_read
Parameters: pointer to request message
            request message size
            pointer to response message
            max response message length
Returns:    actual response message length

This function processes the PLC5 "Word Range Read" Command.

This command returns raw word memory. All necessary steps are attempted
to make sure the data is returned in the correct order.
======================================================================= */
static uint16 local_userdf1_block_read (uint8 * req_msg, uint16 req_msg_size, uint8 * rsp_msg, uint16 max_rsp_msg_size)
{
    uint16  i, j, k;
    uint8   ext_sts_err_n, ext_sts_err_f, ext_sts_err_b, ext_sts_err_st;
    uint8   msg_found, num_bytes_req;
    uint16  rsp_size, number_of_units;
    uint8   temp8_buf4[4];

    RTA_UNUSED_PARAM(max_rsp_msg_size);

    /* request size MUST be 1 */
    if(req_msg_size < 1)
    {
	    /* Error: Not Enough Data */
        eips_pccc_build_error(ERR_INSUFF_DATA,0,NULL);
	    return(0);
    }
    else if(req_msg_size > 1)
    {
	    /* Error: Too Much Data */
        eips_pccc_build_error(ERR_TOOMUCH_DATA,0,NULL);
	    return(0);
    }
    
/*    eips_user_dbprint4("Block Read %s%d:%d (%d bytes)\r\n",DF1_REQ.ascii_file_type, DF1_REQ.file_num, DF1_REQ.element_num, req_msg[0]); */

    /* default values */
    msg_found      = FALSE;
    ext_sts_err_n  = EXTSTS_BADADDRESS;
    ext_sts_err_f  = EXTSTS_BADADDRESS;
    ext_sts_err_b  = EXTSTS_BADADDRESS;
    ext_sts_err_st = EXTSTS_BADADDRESS;
    number_of_units = 0;

    /* store the Number of Bytes */
    num_bytes_req = req_msg[0];

    /* calculate the Register Size (based on Total Trans) */
    DF1_REQ.num_regs = DF1_REQ.total_trans;

    /* see if this is a bit request */
    if(msg_found == FALSE)
    {
        /* try to read the Bit Registers */
        ext_sts_err_b = eips_userobj_read_B_reg_pccc (DF1_REQ.file_num, DF1_REQ.element_num, DF1_REQ.num_regs, pccc_temp_data.bits);

        /* we found the register */
        if(ext_sts_err_b != EXTSTS_BADADDRESS)
            msg_found =  TRUE;
    }

    /* see if this is an integer request */
    if(msg_found == FALSE)
    {
        /* try to read the Integer Registers */
        ext_sts_err_n = eips_userobj_read_N_reg_pccc (DF1_REQ.file_num, DF1_REQ.element_num, DF1_REQ.num_regs, pccc_temp_data.ints);

        /* we found the register */
        if(ext_sts_err_n != EXTSTS_BADADDRESS)
            msg_found =  TRUE;
    }

    /* see if this is a float request */
    if(msg_found == FALSE)
    {
        /* try to read the Float Registers */
        if(((DF1_REQ.num_regs*2) % sizeof(float)) == 0)
        {
            /* we need to convert to the actual number of floating point registers requested */
            number_of_units = (uint16)((DF1_REQ.num_regs*2) / sizeof(float));
            ext_sts_err_f = eips_userobj_read_F_reg_pccc (DF1_REQ.file_num, DF1_REQ.element_num, number_of_units, pccc_temp_data.floats);
        }

        /* we found the register */
        if(ext_sts_err_f != EXTSTS_BADADDRESS)
            msg_found =  TRUE;
    }
    
    /* see if this is a string request */
    if(msg_found == FALSE)
    {
        /* try to read the Float Registers */
        if(((DF1_REQ.num_regs*2) % sizeof(EIPS_PCCC_STRING_TYPE)) == 0)
        {
            /* we need to convert to the actual number of string registers requested */
            number_of_units = (uint16)((DF1_REQ.num_regs*2) / sizeof(EIPS_PCCC_STRING_TYPE));
            ext_sts_err_st = eips_userobj_read_ST_reg_pccc (DF1_REQ.file_num, DF1_REQ.element_num, number_of_units, pccc_temp_data.strings);
        }
            
        /* we found the register */
        if(ext_sts_err_st != EXTSTS_BADADDRESS)
            msg_found =  TRUE;
    }

    /* build the error message (if needed) */
    if(msg_found == FALSE)
    {
        /* register type isn't found */
        return(local_userdf1_build_rspmsg (rsp_msg, STSERR_EXTSTS, EXTSTS_BADADDRESS));
    }

    /* ****************** */
    /* Build the Response */
    /* ****************** */
    i=0;

    /* return the command byte | 0x40 */
    rsp_msg[i++] = (uint8)(DF1_REQ.command | 0x40);

    /* return the status (0 on success) */
    rsp_msg[i++] = STSERR_SUCCESS;

    /* echo the TNS */
    rta_PutLitEndian16(DF1_REQ.TNS, (rsp_msg+i));
    i+=2;

    /* ******************************************* */
    /*          Bit Data */
    /* ******************************************* */
    if(ext_sts_err_b == EXTSTS_NONE)
    {
        /* store the data */
        for(j=0; j<DF1_REQ.num_regs; j++)
        {
            rta_PutLitEndian16(pccc_temp_data.bits[j], (rsp_msg+i));
            i+=2;
        }

        /* store the data size and return */
        rsp_size = i;
        return(rsp_size);
    }

    /* ******************************************* */
    /*          Integer Data */
    /* ******************************************* */
    else if(ext_sts_err_n == EXTSTS_NONE)
    {
        /* store the data */
        for(j=0; j<DF1_REQ.num_regs; j++)
        {
            rta_PutLitEndian16(pccc_temp_data.ints[j], (rsp_msg+i));
            i+=2;
        }

        /* store the data size and return */
        rsp_size = i;
        return(rsp_size);
    }

    /* ******************************************* */
    /*          Floating Point Data */
    /* ******************************************* */
    else if(ext_sts_err_f == EXTSTS_NONE)
    {
        /* store the data */
        for(j=0; j<number_of_units; j++)
        {
            /* swap the words */
            rta_PutLitEndianFloat(pccc_temp_data.floats[j], temp8_buf4);
            rsp_msg[i]   = temp8_buf4[2];
            rsp_msg[i+1] = temp8_buf4[3];
            rsp_msg[i+2] = temp8_buf4[0];
            rsp_msg[i+3] = temp8_buf4[1];
            i+=4;
        }

        /* store the data size and return */
        rsp_size = i;
        return(rsp_size);
    }

    /* ******************************************* */
    /*          String Data */
    /* ******************************************* */
    else if(ext_sts_err_st == EXTSTS_NONE)
    {
        /* store the data */
        for(j=0; j<number_of_units; j++)
        {
            /* store the size */
            rta_PutLitEndian16(pccc_temp_data.strings[j].string_size, &rsp_msg[i]);
            i+=2;

            /* store the data  */
            for(k=0; k<sizeof(pccc_temp_data.strings[j].string_data); k+=2)
            {
                /* swap the bytes */
                rsp_msg[i]   = pccc_temp_data.strings[j].string_data[k+1];
                rsp_msg[i+1] = pccc_temp_data.strings[j].string_data[k];
                i+=2;
            }
        }

        /* store the data size and return */
        rsp_size = i;
        return(rsp_size);
    }

    /* send a default error (if needed) */
    return(local_userdf1_build_rspmsg (rsp_msg, STSERR_EXTSTS, EXTSTS_BADADDRESS));
}

/* ====================================================================
Function:   local_userdf1_block_write
Parameters: pointer to request message
            request message size
            pointer to response message
            max response message length
Returns:    actual response message length

This function processes the PLC5 "Word Range Write" Command.
======================================================================= */
static uint16 local_userdf1_block_write (uint8 * req_msg, uint16 req_msg_size, uint8 * rsp_msg, uint16 max_rsp_msg_size)
{
    uint16  i, j;
    uint8   ext_sts_err_n, ext_sts_err_f, ext_sts_err_b, ext_sts_err_st;
    uint8   msg_found;
    uint16  number_of_units;
    uint8   temp8_buf4[4];

    RTA_UNUSED_PARAM(max_rsp_msg_size);

    /* request size MUST be 1 */
    if(req_msg_size < 1)
    {
	    /* Error: Not Enough Data */
        eips_pccc_build_error(ERR_INSUFF_DATA,0,NULL);
	    return(0);
    }
    else if(req_msg_size > MAX_NUM_BYTES)
    {
	    /* Error: Too Much Data */
        eips_pccc_build_error(ERR_TOOMUCH_DATA,0,NULL);
	    return(0);
    } 

/*    eips_user_dbprint4("Block Write %s%d:%d (%d bytes)\r\n",DF1_REQ.ascii_file_type, DF1_REQ.file_num, DF1_REQ.element_num, req_msg_size); */

    /* default values */
    msg_found      = FALSE;
    ext_sts_err_n  = EXTSTS_BADADDRESS;
    ext_sts_err_f  = EXTSTS_BADADDRESS;
    ext_sts_err_b  = EXTSTS_BADADDRESS;
    ext_sts_err_st = EXTSTS_BADADDRESS;
    number_of_units = 0;

    /* ******************************************* */
    /*          Bit Data */
    /* ******************************************* */
    if(msg_found == FALSE)
    {        
        /* number of bytes must be even */
        if((req_msg_size % 2) == 0)
        {
            /* how many registers are being written? */
            number_of_units = (uint16)(req_msg_size/2);

            /* convert data to bit registers */
            for(j=0, i=0; j<number_of_units; j++)
            {
                pccc_temp_data.bits[j] = rta_GetLitEndian16(req_msg+i);
                i+=2;
            }

            /* try the write Bit Registers */
            ext_sts_err_b = eips_userobj_write_B_reg_pccc (DF1_REQ.file_num, DF1_REQ.element_num, number_of_units, pccc_temp_data.bits);

            /* we found the register */
            if(ext_sts_err_b != EXTSTS_BADADDRESS)
                msg_found =  TRUE;
        }
    }

    /* ******************************************* */
    /*          Integer Data */
    /* ******************************************* */
    if(msg_found == FALSE)
    {        
        /* number of bytes must be even */
        if((req_msg_size % 2) == 0)
        {
            /* how many registers are being written? */
            number_of_units = (uint16)(req_msg_size/2);

            /* convert data to bit registers */
            for(j=0, i=0; j<number_of_units; j++)
            {
                pccc_temp_data.ints[j] = rta_GetLitEndian16(req_msg+i);
                i+=2;
            }

            /* try the write Integer Registers */
            ext_sts_err_n = eips_userobj_write_N_reg_pccc (DF1_REQ.file_num, DF1_REQ.element_num, number_of_units, pccc_temp_data.ints);

            /* we found the register */
            if(ext_sts_err_n != EXTSTS_BADADDRESS)
                msg_found =  TRUE;
        }
    }

    /* ******************************************* */
    /*          Float Data */
    /* ******************************************* */
    if(msg_found == FALSE)
    {        
        /* number of bytes must be even */
        if((req_msg_size % (sizeof(float))) == 0)
        {
            /* how many registers are being written? */
            number_of_units = (uint16)(req_msg_size/(sizeof(float)));

            /* convert data to bit registers */
            for(j=0, i=0; j<number_of_units; j++)
            {
                temp8_buf4[2] = req_msg[i];
                temp8_buf4[3] = req_msg[i+1];
                temp8_buf4[0] = req_msg[i+2];
                temp8_buf4[1] = req_msg[i+3];

                pccc_temp_data.floats[j] = rta_GetLitEndianFloat(temp8_buf4);
                i+=4;
            }

            /* try the write Integer Registers */
            ext_sts_err_f = eips_userobj_write_F_reg_pccc (DF1_REQ.file_num, DF1_REQ.element_num, number_of_units, pccc_temp_data.floats);

            /* we found the register */
            if(ext_sts_err_f != EXTSTS_BADADDRESS)
                msg_found =  TRUE;
        }
    }

    /* ******************************************* */
    /*          String Data */
    /* ******************************************* */
    if(msg_found == FALSE)
    {        
        /* make sure we have enough data */
        if(req_msg_size >= 2)
        {
            /* get the length of the string (we can only write a single string)    */
            number_of_units = rta_GetLitEndian16(&req_msg[0]);
 
            /* make sure we have enough data */
            if((number_of_units + 2) <= req_msg_size)
            {
                /* store the string */
                pccc_temp_data.strings[0].string_size = number_of_units; 

                /* swap all bytes of data */
                memset(pccc_temp_data.strings[0].string_data, 0, sizeof(pccc_temp_data.strings[0].string_data));
                for(i=0; i<number_of_units; i+=2)
                {
                    pccc_temp_data.strings[0].string_data[i] = req_msg[3+i];
                    pccc_temp_data.strings[0].string_data[i+1] = req_msg[2+i];
                }

                /* try to write the string */
                ext_sts_err_st = eips_userobj_write_ST_reg_pccc (DF1_REQ.file_num, DF1_REQ.element_num, 1, pccc_temp_data.strings);

                /* we found the register */
                if(ext_sts_err_st != EXTSTS_BADADDRESS)
                    msg_found =  TRUE;
            }
        }    
    }

    /* build the error message (if needed) */
    if(msg_found == FALSE)
    {
        /* register type isn't found */
        return(local_userdf1_build_rspmsg (rsp_msg, STSERR_EXTSTS, EXTSTS_BADADDRESS));
    }

    /* build the success message */
    else
    {
        /* look for the first error that doesn't equal EXTSTS_BADADDRESS */
        if((ext_sts_err_b != EXTSTS_BADADDRESS) && (ext_sts_err_b != STSERR_SUCCESS))
            return(local_userdf1_build_rspmsg (rsp_msg, STSERR_EXTSTS, ext_sts_err_b));            
        else if((ext_sts_err_n != EXTSTS_BADADDRESS) && (ext_sts_err_n != STSERR_SUCCESS))
            return(local_userdf1_build_rspmsg (rsp_msg, STSERR_EXTSTS, ext_sts_err_n));            
        else if((ext_sts_err_f != EXTSTS_BADADDRESS) && (ext_sts_err_f != STSERR_SUCCESS))
            return(local_userdf1_build_rspmsg (rsp_msg, STSERR_EXTSTS, ext_sts_err_f));            
        else if((ext_sts_err_st != EXTSTS_BADADDRESS) && (ext_sts_err_st != STSERR_SUCCESS))
            return(local_userdf1_build_rspmsg (rsp_msg, STSERR_EXTSTS, ext_sts_err_st));            
        else
            return(local_userdf1_build_rspmsg (rsp_msg, STSERR_SUCCESS, 0));
    }
}

/* ====================================================================
Function:   local_userdf1_rd_mod_wr_bit
Parameters: pointer to request message
            request message size
            pointer to response message
            max response message length
Returns:    actual response message length

This function processes the PLC5 "Read Modify-Write Bit" Command.
======================================================================= */
static uint16 local_userdf1_rd_mod_wr_bit (uint8 * req_msg, uint16 req_msg_size, uint8 * rsp_msg, uint16 max_rsp_msg_size)
{
    uint16  i;
    uint16  old_val, new_val;
    uint8   ext_sts_err_n, ext_sts_err_b;

    RTA_UNUSED_PARAM(max_rsp_msg_size);
    RTA_UNUSED_PARAM(req_msg_size);
    RTA_UNUSED_PARAM(req_msg);

/*    eips_user_dbprint4("RdWrMod %s%d:%d (%d bytes)\r\n",DF1_REQ.ascii_file_type, DF1_REQ.file_num, DF1_REQ.element_num, req_msg_size); */

    /* default values */
    ext_sts_err_n  = EXTSTS_BADADDRESS;
    ext_sts_err_b  = EXTSTS_BADADDRESS;

    /* multiple commands can be entered */
    for(i=0; i<DF1_REQ.total_trans; i++)
    {
        /* try to read the bit */
        ext_sts_err_b = eips_userobj_read_B_reg_pccc (DF1_RDMODWR_REQ[i].file_num, DF1_RDMODWR_REQ[i].element_num, 1, &old_val);

        /* we found the register */
        if(ext_sts_err_b == EXTSTS_NONE)
        {
            new_val = (uint16)((old_val & DF1_RDMODWR_REQ[i].AND_mask) | DF1_RDMODWR_REQ[i].OR_mask);

            /* write the new value */
            ext_sts_err_b = eips_userobj_write_B_reg_pccc (DF1_RDMODWR_REQ[i].file_num, DF1_RDMODWR_REQ[i].element_num, 1, &new_val);

            /* if the error exist, send it */
            if(ext_sts_err_b != EXTSTS_NONE)
            {
                return(local_userdf1_build_rspmsg (rsp_msg, STSERR_EXTSTS, ext_sts_err_b));
            }
        }

        /* we didn't find the register, but we received a valid error (send the error response) */
        else if(ext_sts_err_b != EXTSTS_BADADDRESS)
        {
            return(local_userdf1_build_rspmsg (rsp_msg, STSERR_EXTSTS, ext_sts_err_b));
        }

        /* try to read the int */
        else
        {
            /* try to read the bit */
            ext_sts_err_n = eips_userobj_read_N_reg_pccc (DF1_RDMODWR_REQ[i].file_num, DF1_RDMODWR_REQ[i].element_num, 1, &old_val);

            /* we found the register */
            if(ext_sts_err_n == EXTSTS_NONE)
            {
                new_val = (uint16)((old_val & DF1_RDMODWR_REQ[i].AND_mask) | DF1_RDMODWR_REQ[i].OR_mask);

                /* write the new value */
                ext_sts_err_n = eips_userobj_write_N_reg_pccc (DF1_RDMODWR_REQ[i].file_num, DF1_RDMODWR_REQ[i].element_num, 1, &new_val);

                /* if the error exist, send it */
                if(ext_sts_err_n != EXTSTS_NONE)
                {
                    return(local_userdf1_build_rspmsg (rsp_msg, STSERR_EXTSTS, ext_sts_err_n));
                }
            }

            /* we didn't find the register, but we received a valid error (send the error response) */
            else 
            {
                return(local_userdf1_build_rspmsg (rsp_msg, STSERR_EXTSTS, ext_sts_err_n));
            }
        }
    }

    return(local_userdf1_build_rspmsg (rsp_msg, STSERR_SUCCESS, 0));
}

/* ====================================================================
Function:   local_userdf1_build_rspmsg
Parameters: pointer to response message
            STS and EXT_STS
Returns:    actual response message length

This function builds the response message.
======================================================================= */
static uint16 local_userdf1_build_rspmsg (uint8 * rsp_msg, uint8 STS, uint8 EXT_STS)
{
    uint16 i;

    i = 0;

    /* return the command byte | 0x40 */
    rsp_msg[i++] = (uint8)(DF1_REQ.command | 0x40);

    /* return the status (0 on success) */
    rsp_msg[i++] = STS;

    /* echo the TNS */
    rta_PutLitEndian16(DF1_REQ.TNS, (rsp_msg+i));
    i+=2;

    /* return the extended status (0 on success) */
    if(STS == STSERR_EXTSTS)
        rsp_msg[i++] = EXT_STS;

    return(i);
}

#endif
