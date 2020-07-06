/* ********************************************************************
Module:  mbtcp_system.h
Author:  Jamin D. Wendorf	   12/2003
         (c)2003 REAL TIME AUTOMATION

This file contains Modbus TCP Server specific definitions.
*********************************************************************** */
#ifndef __MBTCP_SYSTEM_H__
#define __MBTCP_SYSTEM_H__

#define MBTCP_PORT 502
#define SOCKTYPE_TCP 0

// parsing offsets
#define MBTCP_OFFSET_TRANSID  0
#define MBTCP_OFFSET_PROTOID  2
#define MBTCP_OFFSET_LENGTH   4
#define MBTCP_OFFSET_UNITID   6
#define MBTCP_OFFSET_FUNCCODE 7
#define MBTCP_OFFSET_DATA     8

// error codes
#define MBTCP_ERRCODE_FUNCTION 0x01
#define MBTCP_ERRCODE_ADDRESS  0x02
#define MBTCP_FCERRBIT         0x80

/* --------------------------------------------------------------- */
/*	MBTCP STANDARD INCLUDE FILES				   */
/* --------------------------------------------------------------- */
#include "mbtcp_usersys.h"
#include "mbtcp_userobj.h"
#include "mbtcp_usersock.h"
#include "mbtcp_struct.h"
#include "mbtcp_proto.h"
#include "mbtcp_revision.txt"

#endif /* __MBTCP_SYSTEM_H__ */
