/* ********************************************************************
Module:  mbtcp_proto.h
Author:  Jamin D. Wendorf     12/2003
         (c)2003 REAL TIME AUTOMATION

This file contains Modbus TCP Server funcion prototypes.
*********************************************************************** */
#ifndef __MBTCP_PROTO_H__
#define __MBTCP_PROTO_H__

/* ********************************** */
/* Global Functions in mbtcp_rtasys.c */
/* ********************************** */
void mbtcp_rtasys_init        (void);
void mbtcp_rtasys_onTCPAccept (MBTCP_USERSYS_SOCKTYPE socket_id);
void mbtcp_rtasys_onTCPData   (MBTCP_USERSYS_SOCKTYPE socket_id, uint8 * socket_data, uint16 socket_data_size);
void mbtcp_rtasys_onTCPSent   (MBTCP_USERSYS_SOCKTYPE socket_id);
void mbtcp_rtasys_onTCPClose  (MBTCP_USERSYS_SOCKTYPE socket_id);

/* ********************************** */
/* Global Functions in mbtcp_server.c */
/* ********************************** */
void mbtcp_server_init (void);
void mbtcp_server_process (MBTCP_SOCKET_STRUCT *sock, uint8 *msg, uint16 msg_siz);

/* *********************************** */
/* Global Functions in mbtcp_userobj.c */
/* *********************************** */
void   mbtcp_userobj_init (void);
uint8  *mbtcp_userobj_getInputCoilPtr   (uint16 size_in_bits);
uint8  *mbtcp_userobj_getOutputCoilPtr  (uint16 size_in_bits);
uint16 *mbtcp_userobj_getInputRegPtr    (uint16 size_in_words);
uint16 *mbtcp_userobj_getOutputRegPtr   (uint16 size_in_words);
void   mbtcp_userobj_coilsWritten       (uint16 start_addr, uint16 length_in_bits);
void   mbtcp_userobj_registersWritten   (uint16 start_addr, uint16 length_in_words);
uint8  mbtcp_userobj_getExceptionStatus (void);

/* ************************************ */
/* Global Functions in mbtcp_usersock.c */
/* ************************************ */
void   mbtcp_usersock_init     (void);
void   mbtcp_usersock_process  (void);
void   mbtcp_usersock_sendData (MBTCP_USERSYS_SOCKTYPE socket_id, uint8 * socket_data, uint16 socket_data_size);
void   mbtcp_usersock_tcpClose (MBTCP_USERSYS_SOCKTYPE socket_id);

/* *********************************** */
/* Global Functions in mbtcp_usersys.c */
/* *********************************** */
void   mbtcp_usersys_init       (void);
void   mbtcp_usersys_fatalError (char *function_name, int16 error_num);

#endif /* __MBTCP_PROTO_H__ */
