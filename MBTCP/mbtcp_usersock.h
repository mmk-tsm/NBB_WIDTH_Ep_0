/* ********************************************************************
Module:  mbtcp_usersock.h
Author:  Jamin D. Wendorf     12/2003
	     (c)2003 REAL TIME AUTOMATION

This file contains user definitions specific to the sockets.
*********************************************************************** */
#ifndef __MBTCP_USERSOCK_H__
#define __MBTCP_USERSOCK_H__

/* define what the socket type is (usually int) */
#define MBTCP_USERSYS_SOCKTYPE      int
#define MBTCP_USER_MAX_NUM_SOCKETS  10
#define MBTCP_USER_MAX_NUM_SOCK_ID  FD_SETSIZE
#define MBTCP_USER_SOCKET_UNUSED    -1

#endif /* __MBTCP_USERSOCK_H__ */
