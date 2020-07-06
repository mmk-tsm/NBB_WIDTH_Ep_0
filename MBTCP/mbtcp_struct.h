/* ********************************************************************
Module:  mbtcp_struct.h
Author:  Jamin D. Wendorf       10/02
         (c)2002 REAL TIME AUTOMATION

This file contains Modbus TCP Server structure definitions.
*********************************************************************** */
#ifndef __MBTCP_STRUCT_H__
#define __MBTCP_STRUCT_H__

/* --------------------------------------------------------------- */
/*      STRUCTURE DEFINITIONS                                      */
/* --------------------------------------------------------------- */
/* ====================================================================
SOCKET STRUCTURES

Socket structures are used to simplify TCP/IP processing.
======================================================================= */
typedef struct
{
   uint8 sock_type;
   MBTCP_USERSYS_SOCKTYPE sock_id;
}MBTCP_SOCKET_STRUCT;

#endif /* __MBTCP_STRUCT_H__ */
