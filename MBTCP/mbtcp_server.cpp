/*********************************************************************
Module:  Modbus TCP Server Processing (mbtcp_server.c)
Author:  Jamin D. Wendorf     12/2003
         (c)2003 Real Time Automation

This file contains the definitions needed for parsing and processing
Modbus TCP Server Messages.  The buffer passed with the request can not
be used for the response since an additional request may be in the 
passed buffer.

M.McKiernan          26.09.2007
        - Prevent writes into the read only (production summary) section of the MB table. 
        in function local_FC16PresetMultRegs().
        Also in function local_FC6PresetSingleReg(). 
P.Smith             23/6/08        
        increment g_nTCPIPReadHoldRegsCtr counter to show tcpip is operating
        
P.Smith             13/11/08        
        added general comms counter increment g_lTCPIPCommsCounter
P.Smith             8/12/08        
 g_sOngoingHistoryTCPIPComms.m_lGoodRx++
*********************************************************************** */

/* ---------------------------- */
/* INCLUDE FILES                */
/* ---------------------------- */
#include "rta_system.h"
#include "mbtcp_system.h"

#include "BatchMBIndices.h" 
#include "Batvars2.h" 
#include "HistoryLog.h"


/* ---------------------------- */
/* LOCAL FUNCTION PROTOTYPES    */
/* ---------------------------- */

#ifdef FC01_RDCOILSTAT /* only include supported functions */
  void local_FC1ReadCoilStatus (MBTCP_SOCKET_STRUCT *sock, uint8 *msg, uint16 msg_siz);
#endif

#ifdef FC02_RDINPUTSTAT /* only include supported functions */
  void local_FC2InputCoils (MBTCP_SOCKET_STRUCT *sock, uint8 *msg, uint16 msg_siz);
#endif

#ifdef FC03_RDHOLDREG /* only include supported functions */
  void local_FC3ReadHoldRegs (MBTCP_SOCKET_STRUCT *sock, uint8 *msg, uint16 msg_siz);
#endif

#ifdef FC04_RDINPUTREG /* only include supported functions */
   void local_FC4ReadInputRegs (MBTCP_SOCKET_STRUCT *sock, uint8 *msg, uint16 msg_siz);
#endif

#ifdef FC05_WRSINGLECOIL /* only include supported functions */
   void local_FC5ForceSingleCoil (MBTCP_SOCKET_STRUCT *sock, uint8 *msg, uint16 msg_siz);
#endif

#ifdef FC06_WRSINGLEREG /* only include supported functions */
   void local_FC6PresetSingleReg (MBTCP_SOCKET_STRUCT *sock, uint8 *msg, uint16 msg_siz);
#endif

#ifdef FC07_RDEXCEPTSTAT /* only include supported functions */
   void local_FC7ReadExceptionStatus (MBTCP_SOCKET_STRUCT *sock, uint8 *msg, uint16 msg_siz);
#endif

#ifdef FC16_WRMULTREGS /* only include supported functions */
   void local_FC16PresetMultRegs (MBTCP_SOCKET_STRUCT *sock, uint8 *msg, uint16 msg_siz);
#endif

// alway able to send an error message
void local_sendErrorMsg (MBTCP_SOCKET_STRUCT *sock, uint8 *msg, uint8 error_code);

/* ---------------------------- */
/* STATIC VARIABLES             */
/* ---------------------------- */
static uint8  *MBInputCoilPtr;
static uint8  *MBOutputCoilPtr;
static uint16 *MBInputRegPtr;
static uint16 *MBOutputRegPtr;

/* ---------------------------- */
/* MISCELLANEOUS                */
/* ---------------------------- */
MBTCP_CODESPACE uint8 MBCOIL_SHIFT_MASK_RIGHT[8]  = {0xFF,0xFE,0xFC,0xF8,0xF0,0xE0,0xC0,0x80};
MBTCP_CODESPACE uint8 MBCOIL_SHIFT_MASK_LEFT[8]   = {0x00,0x01,0x03,0x07,0x0F,0x1F,0x3F,0x7F};
MBTCP_CODESPACE uint8 MBCOIL_SHIFT_MASK_UNUSED[8] = {0xFF,0x01,0x03,0x07,0x0F,0x1F,0x3F,0x7F};

/**/
/* ******************************************************************** */
/*                      GLOBAL FUNCTIONS                                */
/* ******************************************************************** */
/* ====================================================================
Function:    mbtcp_server_init
Parameters:  init type
Returns:     N/A

This function initializes all static variables used in this file.
======================================================================= */
void mbtcp_server_init (void)
{
   // default all pointers to NULL
   MBInputCoilPtr  = NULL;
   MBOutputCoilPtr = NULL;
   MBInputRegPtr   = NULL;
   MBOutputRegPtr  = NULL;

   /* get data pointers (as needed) */

   // Discrete Output Coils
   #if defined (FC01_RDCOILSTAT) || defined (FC05_WRSINGLECOIL)
      // try to get a data pointer
      MBOutputCoilPtr = mbtcp_userobj_getOutputCoilPtr(NUMBER_OF_OUTCOILS);  

      // we could't get the pointer
      if(MBOutputCoilPtr == NULL)
         mbtcp_usersys_fatalError("mbtcp_server_init",1);
   #endif

   // Discrete Input Coils
   #if defined (FC02_RDINPUTSTAT)
      // try to get a data pointer
      MBInputCoilPtr = mbtcp_userobj_getInputCoilPtr(NUMBER_OF_INCOILS);  

      // we could't get the pointer
      if(MBInputCoilPtr == NULL)
         mbtcp_usersys_fatalError("mbtcp_server_init",2);
   #endif

   // Output Register
   #if defined (FC03_RDHOLDREG) || defined (FC06_WRSINGLEREG) || defined (FC16_WRMULTREGS)
      // try to get a data pointer
      MBOutputRegPtr = mbtcp_userobj_getOutputRegPtr(NUMBER_OF_OUTREGS);  

      // we could't get the pointer
      if(MBOutputRegPtr == NULL)
         mbtcp_usersys_fatalError("mbtcp_server_init",3);
   #endif

   // Input Registers
   #if defined (FC04_RDINPUTREG)
      // try to get a data pointer
      MBInputRegPtr = mbtcp_userobj_getInputRegPtr(NUMBER_OF_INREGS);  

      // we could't get the pointer
      if(MBInputRegPtr == NULL)
         mbtcp_usersys_fatalError("mbtcp_server_init",4);
   #endif
}

/* ====================================================================
Function:     mbtcp_server_process
Parameters:   socket structure pointer
              message pointer
              message size
Returns:      N/A

This function processes the Modbus TCP message. We don't care what the 
Unit Id (slave address) is.
======================================================================= */
void mbtcp_server_process (MBTCP_SOCKET_STRUCT *sock, uint8 *msg, uint16 msg_siz)
{
   // we need at least 2 bytes of data
   if(rta_GetBigEndian16(msg+MBTCP_OFFSET_LENGTH)< 2)
      return;   

   // make sure this isn't a response message
   if((msg[MBTCP_OFFSET_FUNCCODE] & MBTCP_FCERRBIT) == MBTCP_FCERRBIT)
      return;

   // if the Unit Identifier matters, verify it
   switch(msg[MBTCP_OFFSET_FUNCCODE])
   {
      #ifdef FC01_RDCOILSTAT /* only include supported functions */
      case 1:
         local_FC1ReadCoilStatus (sock, msg, msg_siz);
         break;
      #endif

      #ifdef FC02_RDINPUTSTAT /* only include supported functions */
      case 2:
         local_FC2InputCoils (sock, msg, msg_siz);
         break;
      #endif

      #ifdef FC03_RDHOLDREG /* only include supported functions */
      case 3:
         local_FC3ReadHoldRegs (sock, msg, msg_siz);
         break;
      #endif

      #ifdef FC04_RDINPUTREG /* only include supported functions */     
      case 4:
         local_FC4ReadInputRegs (sock, msg, msg_siz);
         break;
      #endif

      #ifdef FC05_WRSINGLECOIL /* only include supported functions */
      case 5:
         local_FC5ForceSingleCoil (sock, msg, msg_siz);
         break;
      #endif

      #ifdef FC06_WRSINGLEREG /* only include supported functions */
      case 6:
         local_FC6PresetSingleReg (sock, msg, msg_siz);
         break;
      #endif

      #ifdef FC07_RDEXCEPTSTAT /* only include supported functions */
      case 7:
         local_FC7ReadExceptionStatus (sock, msg, msg_siz);
         break;
      #endif

      #ifdef FC16_WRMULTREGS /* only include supported functions */
      case 16:
         local_FC16PresetMultRegs (sock, msg, msg_siz);
         break;
      #endif

      // return "Invalid Function Code"
      default:
         local_sendErrorMsg(sock, msg, MBTCP_ERRCODE_FUNCTION);
         break;
   }
} /* END-> "EIPProcess()" */

/**/
/* ******************************************************************* */
/*                      LOCAL FUNCTIONS                                */
/* ******************************************************************* */
/* ====================================================================
Function:     local_sendErrorMsg
Parameters:   socket structure pointer
              message pointer
              error code  
Returns:      N/A

This function builds the error message and sends it.
======================================================================= */
void local_sendErrorMsg (MBTCP_SOCKET_STRUCT *sock, uint8 *msg, uint8 error_code)
{
   uint8 error_rsp[9], i;
   uint16 error_rsp_size;

   // return if the Unit ID isn't 0
   if(msg[MBTCP_OFFSET_UNITID] == 0)
      return;

   // echo the first 7 bytes
   error_rsp_size = 0;
   for(i=0; i<7; i++)
      error_rsp[error_rsp_size++] = msg[i];  

   // turn bit 7 ON for the function code
   error_rsp[error_rsp_size++] = (msg[MBTCP_OFFSET_FUNCCODE] | MBTCP_FCERRBIT);  
 
   // store the error code as the first data byte
   error_rsp[error_rsp_size++] = error_code;

   // store the length
   rta_PutBigEndian16(((uint16)(error_rsp_size-6)),(error_rsp+MBTCP_OFFSET_LENGTH));

   // send the response (fixed at 9 bytes)
   mbtcp_usersock_sendData(sock->sock_id, error_rsp,error_rsp_size);
}

/* ====================================================================
Function:     local_FC1ReadCoilStatus
Parameters:   socket structure pointer
              message pointer
              message size 
Returns:      N/A

This function processes the Read Coil Status function code.
======================================================================= */
#ifdef FC01_RDCOILSTAT /* only include supported functions */
void local_FC1ReadCoilStatus (MBTCP_SOCKET_STRUCT *sock, uint8 *msg, uint16 msg_siz)
{
   uint8  rsp_buf[300]; // max size set by Modbus TCP spec
   uint16 rsp_buf_siz;
   uint16 temp16, ix, i;
   uint16 rel_coil_addr;
   uint8  shift_byte_right, shift_byte_left;
   uint8  byte_cnt, temp8;
   uint16 start_addr, num_pts;

   /* this is a broadcast address, return */
   if(msg[MBTCP_OFFSET_UNITID] == 0)
      return;

   /* make sure we have exactly 6 bytes of data */
   if(rta_GetBigEndian16(msg+MBTCP_OFFSET_LENGTH) != 6)
   {
      local_sendErrorMsg(sock, msg, MBTCP_ERRCODE_FUNCTION);
      return;
   }

   /* calculate the starting address and number of points */
   start_addr = rta_GetBigEndian16(msg+(MBTCP_OFFSET_DATA));
   num_pts = rta_GetBigEndian16(msg+(MBTCP_OFFSET_DATA+2));

   /* they are asking for too much data */
   if(num_pts > 2000) // max size set by Modbus TCP spec
   {
      local_sendErrorMsg(sock, msg, MBTCP_ERRCODE_ADDRESS);
      return;
   }

   /* the starting address is invalid */
   if((start_addr+1) < STARTING_OUTCOIL_ADDR)
   {
      local_sendErrorMsg(sock, msg, MBTCP_ERRCODE_ADDRESS);
      return;
   }

   /* the combination of starting address and length is invalid */
   if( ((start_addr+1) + num_pts - 1) > (STARTING_OUTCOIL_ADDR+NUMBER_OF_OUTCOILS-1))
   {
      local_sendErrorMsg(sock, msg, MBTCP_ERRCODE_ADDRESS);
      return;
   }

   /* calculate how many bytes are to be sent */
   temp16 = ((num_pts + 7)/8);
   byte_cnt = (uint8)temp16;

   /* calculate the relative starting bit position */
   rel_coil_addr = (start_addr-(STARTING_OUTCOIL_ADDR-1));

   /* calculate the first byte (integer divide) */
   ix = (rel_coil_addr/8);

   /* based on the bit position, we may need perform a binary shift to
      the right */
   shift_byte_right = MBTCP_MODX(rel_coil_addr,8);
   shift_byte_left  = 8 - shift_byte_right;

   /* ************************** */
   /* build the response message */
   /* ************************** */
   rsp_buf_siz = 0;   

   // echo the first 8 bytes
   for(i=0; i<8; i++)
      rsp_buf[rsp_buf_siz++] = msg[i];  
 
   // store the byte count
   rsp_buf[rsp_buf_siz++] = byte_cnt;

   /* add data to transmit queue */
   for(i=0; i<byte_cnt; i++, ix++)
   {
      /* store the data from the first index */
      temp8 = (MBOutputCoilPtr[ix]&MBCOIL_SHIFT_MASK_RIGHT[shift_byte_right]) >> shift_byte_right;

      /* if the data isn't byte aligned, add the bits from the next byte */
      if (shift_byte_right)
      {
         temp8 |= (MBOutputCoilPtr[(ix+1)]&MBCOIL_SHIFT_MASK_LEFT[shift_byte_right]) << shift_byte_left;
      }

      /* if this is the last byte, mask off unused bits */
      if((i+1) == byte_cnt)
      {
         temp8 &= MBCOIL_SHIFT_MASK_UNUSED[(MBTCP_MODX(num_pts,8))];
      }

      // store data byte
      rsp_buf[rsp_buf_siz++] = temp8;
   }

   // store the length
   rta_PutBigEndian16(((uint16)(rsp_buf_siz-6)),rsp_buf+MBTCP_OFFSET_LENGTH);

   // send the response
   mbtcp_usersock_sendData(sock->sock_id, rsp_buf, rsp_buf_siz);
}
#endif

/* ====================================================================
Function:     local_FC2InputCoils
Parameters:   socket structure pointer
              message pointer
              message size 
Returns:      N/A

This function processes the Read Input Coils function code.
======================================================================= */
#ifdef FC02_RDINPUTSTAT /* only include supported functions */
void local_FC2InputCoils (MBTCP_SOCKET_STRUCT *sock, uint8 *msg, uint16 msg_siz)
{
   uint8  rsp_buf[300]; // max size set by Modbus TCP spec
   uint16 rsp_buf_siz;
   uint16 temp16, ix, i;
   uint16 rel_coil_addr;
   uint8  shift_byte_right, shift_byte_left;
   uint8  byte_cnt, temp8;
   uint16 start_addr, num_pts;

   /* this is a broadcast address, return */
   if(msg[MBTCP_OFFSET_UNITID] == 0)
      return;

   /* make sure we have exactly 6 bytes of data */
   if(rta_GetBigEndian16(msg+MBTCP_OFFSET_LENGTH) != 6)
   {
      local_sendErrorMsg(sock, msg, MBTCP_ERRCODE_FUNCTION);
      return;
   }

   /* calculate the starting address and number of points */
   start_addr = rta_GetBigEndian16(msg+(MBTCP_OFFSET_DATA));
   num_pts = rta_GetBigEndian16(msg+(MBTCP_OFFSET_DATA+2));

   /* they are asking for too much data */
   if(num_pts > 2000) // max size set by Modbus TCP spec
   {
      local_sendErrorMsg(sock, msg, MBTCP_ERRCODE_ADDRESS);
      return;
   }

   /* the starting address is invalid */
   if((start_addr+1) < STARTING_INCOIL_ADDR)
   {
      local_sendErrorMsg(sock, msg, MBTCP_ERRCODE_ADDRESS);
      return;
   }

   /* the combination of starting address and length is invalid */
   if( ((start_addr+1) + num_pts - 1) > (STARTING_INCOIL_ADDR+NUMBER_OF_INCOILS-1))
   {
      local_sendErrorMsg(sock, msg, MBTCP_ERRCODE_ADDRESS);
      return;
   }

   /* calculate how many bytes are to be sent */
   temp16 = ((num_pts + 7)/8);
   byte_cnt = (uint8)temp16;

   /* calculate the relative starting bit position */
   rel_coil_addr = (start_addr-(STARTING_INCOIL_ADDR-1));

   /* calculate the first byte (integer divide) */
   ix = (rel_coil_addr/8);

   /* based on the bit position, we may need perform a binary shift to
      the right */
   shift_byte_right = MBTCP_MODX(rel_coil_addr,8);
   shift_byte_left  = 8 - shift_byte_right;

   /* ************************** */
   /* build the response message */
   /* ************************** */
   rsp_buf_siz = 0;   

   // echo the first 8 bytes
   for(i=0; i<8; i++)
      rsp_buf[rsp_buf_siz++] = msg[i];  
 
   // store the byte count
   rsp_buf[rsp_buf_siz++] = byte_cnt;

   /* add data to transmit queue */
   for(i=0; i<byte_cnt; i++, ix++)
   {
      /* store the data from the first index */
      temp8 = (MBInputCoilPtr[ix]&MBCOIL_SHIFT_MASK_RIGHT[shift_byte_right]) >> shift_byte_right;

      /* if the data isn't byte aligned, add the bits from the next byte */
      if (shift_byte_right)
      {
         temp8 |= (MBInputCoilPtr[(ix+1)]&MBCOIL_SHIFT_MASK_LEFT[shift_byte_right]) << shift_byte_left;
      }

      /* if this is the last byte, mask off unused bits */
      if((i+1) == byte_cnt)
      {
         temp8 &= MBCOIL_SHIFT_MASK_UNUSED[(MBTCP_MODX(num_pts,8))];
      }

      // store data byte
      rsp_buf[rsp_buf_siz++] = temp8;
   }

   // store the length
   rta_PutBigEndian16(((uint16)(rsp_buf_siz-6)),rsp_buf+MBTCP_OFFSET_LENGTH);

   // send the response
   mbtcp_usersock_sendData(sock->sock_id, rsp_buf, rsp_buf_siz);
}
#endif

/* ====================================================================
Function:     local_FC3ReadHoldRegs
Parameters:   socket structure pointer
              message pointer
              message size 
Returns:      N/A

This function processes the Read Holding Register function code.
======================================================================= */
#ifdef FC03_RDHOLDREG /* only include supported functions */
void local_FC3ReadHoldRegs (MBTCP_SOCKET_STRUCT *sock, uint8 *msg, uint16 msg_siz)
{
   uint8  rsp_buf[300]; // max size set by Modbus TCP spec
   uint16 rsp_buf_siz;
   uint16 i, ix, start_addr, num_pts;

   /* this is a broadcast address, return */
   if(msg[MBTCP_OFFSET_UNITID] == 0)
      return;

   /* make sure we have exactly 6 bytes of data */
   if(rta_GetBigEndian16(msg+MBTCP_OFFSET_LENGTH) != 6)
   {
      local_sendErrorMsg(sock, msg, MBTCP_ERRCODE_FUNCTION);
      return;
   }

   /* calculate the starting address and number of points */
   start_addr = rta_GetBigEndian16(msg+(MBTCP_OFFSET_DATA));
   num_pts = rta_GetBigEndian16(msg+(MBTCP_OFFSET_DATA+2));

   /* they are asking for too much data */
   if(num_pts > 125) // max size set by Modbus TCP spec
   {
      local_sendErrorMsg(sock, msg, MBTCP_ERRCODE_ADDRESS);
      return;
   }

   /* the starting address is invalid */
   if((start_addr+1) < STARTING_OUTREG_ADDR)
   {
      local_sendErrorMsg(sock, msg, MBTCP_ERRCODE_ADDRESS);
      return;
   }

   /* the combination of starting address and length is invalid */
   if( ((start_addr+1) + num_pts - 1) > (STARTING_OUTREG_ADDR+NUMBER_OF_OUTREGS-1))
   {
      local_sendErrorMsg(sock, msg, MBTCP_ERRCODE_ADDRESS);
      return;
   }

   /* ************************** */
   /* build the response message */
   /* ************************** */
   rsp_buf_siz = 0;   

   // echo the first 8 bytes
   for(i=0; i<8; i++)
      rsp_buf[rsp_buf_siz++] = msg[i];  
 
   // store the byte count
   rsp_buf[rsp_buf_siz++] = (num_pts * 2);

   // calculate the offset into the data buffer
   ix = (start_addr-(STARTING_OUTREG_ADDR-1));

   // store the data
   for(i=0; i<num_pts; i++)
   {
      rsp_buf[rsp_buf_siz++] = MBTCP_GETHI(*(MBOutputRegPtr+(ix+i)));
      rsp_buf[rsp_buf_siz++] = MBTCP_GETLO(*(MBOutputRegPtr+(ix+i)));
   }

   // store the length
   rta_PutBigEndian16(((uint16)(rsp_buf_siz-6)),rsp_buf+MBTCP_OFFSET_LENGTH);

   // send the response
   mbtcp_usersock_sendData(sock->sock_id, rsp_buf, rsp_buf_siz);
   g_nTCPIPReadHoldRegsCtr++;
   g_lTCPIPCommsCounter++;
   g_sOngoingHistoryTCPIPComms.m_lGoodRx++;
   
   
}
#endif

/* ====================================================================
Function:     local_FC4ReadInputRegs
Parameters:   socket structure pointer
              message pointer
              message size 
Returns:      N/A

This function processes the Read Input Register function code.
======================================================================= */
#ifdef FC04_RDINPUTREG /* only include supported functions */
void local_FC4ReadInputRegs (MBTCP_SOCKET_STRUCT *sock, uint8 *msg, uint16 msg_siz)
{
   uint8  rsp_buf[300]; // max size set by Modbus TCP spec
   uint16 rsp_buf_siz;
   uint16 i, ix, start_addr, num_pts;

   /* this is a broadcast address, return */
   if(msg[MBTCP_OFFSET_UNITID] == 0)
      return;

   /* make sure we have exactly 6 bytes of data */
   if(rta_GetBigEndian16(msg+MBTCP_OFFSET_LENGTH) != 6)
   {
      local_sendErrorMsg(sock, msg, MBTCP_ERRCODE_FUNCTION);
      return;
   }

   /* calculate the starting address and number of points */
   start_addr = rta_GetBigEndian16(msg+(MBTCP_OFFSET_DATA));
   num_pts = rta_GetBigEndian16(msg+(MBTCP_OFFSET_DATA+2));

   /* they are asking for too much data */
   if(num_pts > 125) // max size set by Modbus TCP spec
   {
      local_sendErrorMsg(sock, msg, MBTCP_ERRCODE_ADDRESS);
      return;
   }

   /* the starting address is invalid */
   if((start_addr+1) < STARTING_INREG_ADDR)
   {
      local_sendErrorMsg(sock, msg, MBTCP_ERRCODE_ADDRESS);
      return;
   }

   /* the combination of starting address and length is invalid */
   if( ((start_addr+1) + num_pts - 1) > (STARTING_INREG_ADDR+NUMBER_OF_INREGS-1))
   {
      local_sendErrorMsg(sock, msg, MBTCP_ERRCODE_ADDRESS);
      return;
   }

   /* ************************** */
   /* build the response message */
   /* ************************** */
   rsp_buf_siz = 0;   

   // echo the first 8 bytes
   for(i=0; i<8; i++)
      rsp_buf[rsp_buf_siz++] = msg[i];  
 
   // store the byte count
   rsp_buf[rsp_buf_siz++] = (num_pts * 2);

   // calculate the offset into the data buffer
   ix = (start_addr-(STARTING_INREG_ADDR-1));

   // store the data
   for(i=0; i<num_pts; i++)
   {
      rsp_buf[rsp_buf_siz++] = MBTCP_GETHI(*(MBInputRegPtr+(ix+i)));
      rsp_buf[rsp_buf_siz++] = MBTCP_GETLO(*(MBInputRegPtr+(ix+i)));
   }

   // store the length
   rta_PutBigEndian16(((uint16)(rsp_buf_siz-6)),rsp_buf+MBTCP_OFFSET_LENGTH);

   // send the response
   mbtcp_usersock_sendData(sock->sock_id, rsp_buf, rsp_buf_siz);
}
#endif

/* ====================================================================
Function:     local_FC5ForceSingleCoil
Parameters:   socket structure pointer
              message pointer
              message size 
Returns:      N/A

This function processes the Write Single Coil function code.
======================================================================= */
#ifdef FC05_WRSINGLECOIL /* only include supported functions */
void local_FC5ForceSingleCoil (MBTCP_SOCKET_STRUCT *sock, uint8 *msg, uint16 msg_siz)
{
   uint8  rsp_buf[12]; // response is fixed at 12 bytes
   uint16 rsp_buf_siz;
   uint16 rel_coil_addr;
   uint16 i, ix, start_addr, num_pts;
   uint8 bit_pos;

   /* first make sure we have only 6 bytes of data */
   if(rta_GetBigEndian16(msg+MBTCP_OFFSET_LENGTH) != 6)
   {
      local_sendErrorMsg(sock, msg, MBTCP_ERRCODE_FUNCTION);
      return;
   }

   /* calculate the starting address and number of points */
   start_addr = rta_GetBigEndian16(msg+(MBTCP_OFFSET_DATA));
   num_pts = 1; // this is fixed, since this is a single write

   /* the starting address is invalid */
   if((start_addr+1) < STARTING_OUTCOIL_ADDR)
   {
      local_sendErrorMsg(sock, msg, MBTCP_ERRCODE_ADDRESS);
      return;
   }

   /* the combination of starting address and length is invalid */
   if( ((start_addr+1) + num_pts - 1) > (STARTING_OUTCOIL_ADDR+NUMBER_OF_OUTCOILS-1))
   {
      local_sendErrorMsg(sock, msg, MBTCP_ERRCODE_ADDRESS);
      return;
   }

   /* calculate the relative starting bit position */
   rel_coil_addr = (start_addr-(STARTING_OUTCOIL_ADDR-1));

   /* calculate the byte index (integer divide) */
   ix = (rel_coil_addr/8);

   /* find the bit position to write */
   for(i=0,bit_pos = 0x01; i<MBTCP_MODX(rel_coil_addr,8); i++)
   {
      bit_pos = bit_pos << 1;
   }

   /* turn bit on */
   if(rta_GetBigEndian16(msg+(MBTCP_OFFSET_DATA+2)) == 0xFF00)
   {
      MBOutputCoilPtr[ix] |= bit_pos;
   }
   /* turn bit off */
   else if(rta_GetBigEndian16(msg+(MBTCP_OFFSET_DATA+2)) == 0x0000)
   {
      MBOutputCoilPtr[ix] &= ~bit_pos;
   }
   /* if the data isn't 0xFF00 or 0x0000, don't modify */

   // inform the user data was modified
   mbtcp_userobj_coilsWritten((uint16)(start_addr+1),num_pts);

   /* this is a broadcast address, return */
   if(msg[MBTCP_OFFSET_UNITID] == 0)
      return;

   /* ************************** */
   /* build the response message */
   /* ************************** */
   rsp_buf_siz = 0;   

   // echo the first 12 bytes
   for(i=0; i<12; i++)
      rsp_buf[rsp_buf_siz++] = msg[i];  
 
   // store the length
   rta_PutBigEndian16(((uint16)(rsp_buf_siz-6)),rsp_buf+MBTCP_OFFSET_LENGTH);

   // send the response
   mbtcp_usersock_sendData(sock->sock_id, rsp_buf, rsp_buf_siz);
}
#endif

/* ====================================================================
Function:     local_FC6PresetSingleReg
Parameters:   socket structure pointer
              message pointer
              message size 
Returns:      N/A

This function processes the Preset Single Register function code.
======================================================================= */
#ifdef FC06_WRSINGLEREG /* only include supported functions */
void local_FC6PresetSingleReg (MBTCP_SOCKET_STRUCT *sock, uint8 *msg, uint16 msg_siz)
{
   uint8  rsp_buf[12]; // response is fixed at 12 bytes
   uint16 rsp_buf_siz;
   uint16 i, ix, start_addr, num_pts;

   /* first make sure we have only 6 bytes of data */
   if(rta_GetBigEndian16(msg+MBTCP_OFFSET_LENGTH) != 6)
   {
      local_sendErrorMsg(sock, msg, MBTCP_ERRCODE_FUNCTION);
      return;
   }

   /* calculate the starting address and number of points */
   start_addr = rta_GetBigEndian16(msg+(MBTCP_OFFSET_DATA));
   num_pts = 1; // this is fixed, since this is a single write

   /* the starting address is invalid */
   if((start_addr+1) < STARTING_OUTREG_ADDR)
   {
      local_sendErrorMsg(sock, msg, MBTCP_ERRCODE_ADDRESS);
      return;
   }
//   MMK - Prevent writes into the read only (production summary) section of the MB table.
   if( (start_addr+1) > (MB_TABLE_READ_ONLY_START - num_pts) && (start_addr+1) < MB_TABLE_READ_ONLY_END )
   {
      local_sendErrorMsg(sock, msg, MBTCP_ERRCODE_ADDRESS);
      return;
   }

   /* the combination of starting address and length is invalid */
   if( ((start_addr+1) + num_pts - 1) > (STARTING_OUTREG_ADDR+NUMBER_OF_OUTREGS-1))
   {
      local_sendErrorMsg(sock, msg, MBTCP_ERRCODE_ADDRESS);
      return;
   }

   /* ************** */
   /* store the data */
   /* ************** */
   ix = (start_addr-(STARTING_OUTREG_ADDR-1));
   MBOutputRegPtr[ix] = rta_GetBigEndian16(msg+(MBTCP_OFFSET_DATA+2));

   // inform the user data was modified
   mbtcp_userobj_registersWritten((uint16)(start_addr+1),num_pts);

   /* ************************** */
   /* build the response message */
   /* ************************** */

   /* this is a broadcast address, return */
   if(msg[MBTCP_OFFSET_UNITID] == 0)
      return;
 
   rsp_buf_siz = 0;   

   // echo the first 12 bytes
   for(i=0; i<12; i++)
      rsp_buf[rsp_buf_siz++] = msg[i];  
 
   // store the length
   rta_PutBigEndian16(((uint16)(rsp_buf_siz-6)),rsp_buf+MBTCP_OFFSET_LENGTH);

   // send the response
   mbtcp_usersock_sendData(sock->sock_id, rsp_buf, rsp_buf_siz);
}
#endif

/* ====================================================================
Function:     local_FC7ReadExceptionStatus
Parameters:   socket structure pointer
              message pointer
              message size 
Returns:      N/A

This function processes the Read Exception Status function code.
======================================================================= */
#ifdef FC07_RDEXCEPTSTAT /* only include supported functions */
void local_FC7ReadExceptionStatus (MBTCP_SOCKET_STRUCT *sock, uint8 *msg, uint16 msg_siz)
{
   uint8  rsp_buf[9]; // response is fixed at 12 bytes
   uint16 rsp_buf_siz;
   uint16 i;

   /* make sure we have exactly 2 bytes of data */
   if(rta_GetBigEndian16(msg+MBTCP_OFFSET_LENGTH) != 2)
   {
      local_sendErrorMsg(sock, msg, MBTCP_ERRCODE_FUNCTION);
      return;
   }

   /* ************************** */
   /* build the response message */
   /* ************************** */

   /* this is a broadcast address, return */
   if(msg[MBTCP_OFFSET_UNITID] == 0)
      return;
 
   rsp_buf_siz = 0;   

   // echo the first 8 bytes
   for(i=0; i<8; i++)
      rsp_buf[rsp_buf_siz++] = msg[i];  
 
   // store the exception status
   rsp_buf[rsp_buf_siz++] = mbtcp_userobj_getExceptionStatus();

   // store the length
   rta_PutBigEndian16(((uint16)(rsp_buf_siz-6)),rsp_buf+MBTCP_OFFSET_LENGTH);

   // send the response
   mbtcp_usersock_sendData(sock->sock_id, rsp_buf, rsp_buf_siz);
 
}
#endif

/* ====================================================================
Function:     local_FC16PresetMultRegs
Parameters:   socket structure pointer
              message pointer
              message size 
Returns:      N/A

This function processes the Preset Multiple Registers function code.
======================================================================= */
#ifdef FC16_WRMULTREGS /* only include supported functions */
void local_FC16PresetMultRegs (MBTCP_SOCKET_STRUCT *sock, uint8 *msg, uint16 msg_siz)
{
   uint8  rsp_buf[12]; // response is fixed at 12 bytes
   uint16 rsp_buf_siz;
   uint16 i, ix, start_addr, num_pts;
   uint8  byte_cnt;

   /* first make sure we have at least 7 bytes of data */
   if(rta_GetBigEndian16(msg+MBTCP_OFFSET_LENGTH) < 7)
   {
      local_sendErrorMsg(sock, msg, MBTCP_ERRCODE_FUNCTION);
      return;
   }

   /* calculate the starting address and number of points */
   start_addr = rta_GetBigEndian16(msg+(MBTCP_OFFSET_DATA));
   num_pts = rta_GetBigEndian16(msg+(MBTCP_OFFSET_DATA+2));

   /* make sure we have the whole message */
   if(rta_GetBigEndian16(msg+MBTCP_OFFSET_LENGTH) != ((num_pts*2)+7))
   {
      local_sendErrorMsg(sock, msg, MBTCP_ERRCODE_FUNCTION);
      return;
   }

   /* they are trying to write too much data */
   if(num_pts > 100) // max size set by Modbus TCP spec
   {
      local_sendErrorMsg(sock, msg, MBTCP_ERRCODE_ADDRESS);
      return;
   }

   /* make sure the byte count equals (num_pts * 2) */
   byte_cnt = msg[(MBTCP_OFFSET_DATA+4)];
   if(byte_cnt != (num_pts * 2))
   {
      local_sendErrorMsg(sock, msg, MBTCP_ERRCODE_FUNCTION);
      return;
   }

   /* the starting address is invalid */
//   MMK - Prevent writes into the read only (production summary) section of the MB table.
   if( (start_addr+1) > (MB_TABLE_READ_ONLY_START - num_pts) && (start_addr+1) < MB_TABLE_READ_ONLY_END )
   {
      local_sendErrorMsg(sock, msg, MBTCP_ERRCODE_ADDRESS);
      return;
   }
   
    /* the starting address is invalid */  
   if((start_addr+1) < STARTING_OUTREG_ADDR)
   {
      local_sendErrorMsg(sock, msg, MBTCP_ERRCODE_ADDRESS);
      return;
   }

   /* the combination of starting address and length is invalid */
   if( ((start_addr+1) + num_pts - 1) > (STARTING_OUTREG_ADDR+NUMBER_OF_OUTREGS-1))
   {
      local_sendErrorMsg(sock, msg, MBTCP_ERRCODE_ADDRESS);
      return;
   }

   /* ************** */
   /* store the data */
   /* ************** */
   ix = (start_addr-(STARTING_OUTREG_ADDR-1));
   for(i=0; i<byte_cnt; i+=2)
      MBOutputRegPtr[ix++] = rta_GetBigEndian16(msg+(MBTCP_OFFSET_DATA+5+i));

   // inform the user data was modified
   mbtcp_userobj_registersWritten((uint16)(start_addr+1),num_pts);

   /* ************************** */
   /* build the response message */
   /* ************************** */

   /* this is a broadcast address, return */
   if(msg[MBTCP_OFFSET_UNITID] == 0)
      return;

   rsp_buf_siz = 0;   

   // echo the first 12 bytes
   for(i=0; i<12; i++)
      rsp_buf[rsp_buf_siz++] = msg[i];  
 
   // store the length
   rta_PutBigEndian16(((uint16)(rsp_buf_siz-6)),rsp_buf+MBTCP_OFFSET_LENGTH);

   // send the response
   mbtcp_usersock_sendData(sock->sock_id, rsp_buf, rsp_buf_siz);
}
#endif

/* *********** */
/* END OF FILE */
/* *********** */
