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
 *     Module Name: eips_usersock.c
 *          Author: Jamin D. Wendorf (jwendorf@rtaautomation.com)
 *        Language: Ansi C
 * Compile Options: N/A
 * Compile defines: N/A
 *       Libraries: N/A
 *    Link Options: N/A
 *
 * Description.
 * =======================================================================
 * This file contains the definitions needed to handle the TCP processing.
 *
 */
// M.McKiernan			19.01.2010
// In eips_usersock_process (void), Replace  select() by ZeroWaitSelect() - dont wait around..
// See TSM - dont hang here

// M.McKiernan			12.02.2010
// Close the the UDP Tasks ( UdpReaderMain() and UdpIOReaderMain() ) if a fatal error in Ethernet/IP software
// Uses an OS_FLAGS variable gEIPTaskErrorFlag for this purpose.  If non-zero (set by fatalError), then delete task.
//

/* ---------------------------- */
/* INCLUDE FILES                */
/* ---------------------------- */
#include "eips_system.h"
#include <ethernet.h>

/* ---------------------------- */
/* LOCAL STRUCTURE DEFINITIONS  */
/* ---------------------------- */
#define USERSOCK_MAX_BUF_SIZ 550
#define USERSOCK_LISTEN_IX   0
#define USERSOCK_UDP_IX      1
#define USERSOCK_IO_IX       2
#define USERSOCK_TCPSTART_IX 3
#define USERSOCK_SOCKID_UNUSED -1

typedef struct
{
   IPADDR sockaddr; // socket IP Address
   WORD sockport; // socket Port
   EIPS_USERSYS_SOCKTYPE sockid; // socket IP
   uint8 tx_in_prog_flag; // transmit in progress flag
}USERSOCK_SOCKSTRUCT;

/* ---------------------------- */
/* STATIC VARIABLES             */
/* ---------------------------- */
static USERSOCK_SOCKSTRUCT usersock_sockets [EIPS_USER_MAX_NUM_SOCKETS];

/* ---------------------------- */
/* LOCAL FUNCTIONS              */
/* ---------------------------- */
static void   local_startTcpListen     (USERSOCK_SOCKSTRUCT *user_sock);
static int16  local_findSocketIx       (EIPS_USERSYS_SOCKTYPE socket_id);
static void   local_freeSocketIx       (EIPS_USERSYS_SOCKTYPE socket_id);
static int16  local_findUnusedSocketIx (void);

// flag used to stop the UDP tasks.
extern OS_FLAGS gEIPTaskErrorFlag;
/* ---------------------------- */
/* MISCELLANEOUS                */
/* ---------------------------- */

/**/
/* ******************************************************************** */
/*			GLOBAL FUNCTIONS				*/
/* ******************************************************************** */
/* ====================================================================
Function:   eips_usersock_init
Parameters: init type
Returns:    N/A

This function initialize all user socket variables.

======================================================================= */
void eips_usersock_init (uint8 init_type)
{
    int16 i;

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

    /* initialize all static variables */
    for(i=0; i<EIPS_USER_MAX_NUM_SOCKETS; i++)
    {
        usersock_sockets[i].sockid = USERSOCK_SOCKID_UNUSED;
        usersock_sockets[i].sockaddr = 0;
        usersock_sockets[i].sockport = 0;
        usersock_sockets[i].tx_in_prog_flag = FALSE;
    }

    /* start a listening socket */
    local_startTcpListen (&usersock_sockets[USERSOCK_LISTEN_IX]);

    /* start a listening socket (port = EIP_CIP_PORT) */
    local_startTcpListen (&usersock_sockets[USERSOCK_LISTEN_IX]);
}

/* ====================================================================
Function:   eips_usersock_process
Parameters: N/A
Returns:    N/A

This function checks for socket activity.  This includes new connect
requests, close requests and data received.
======================================================================= */
void eips_usersock_process (void)
{
    int return_code;
    int16 i, buffer_size;
    char buffer[USERSOCK_MAX_BUF_SIZ];
    fd_set readfds, writefds;
    IPADDR close_sockaddr; // close - socket IP Address
    WORD close_sockport; // close - socket Port

    /* we need to zero the socket set every time */
    FD_ZERO(&readfds);
    FD_ZERO(&writefds);

    /* add the TCP listening socket to the read set */
    FD_SET(usersock_sockets[USERSOCK_LISTEN_IX].sockid, &readfds);

    /* add any other sockets that are being used */
    for(i=USERSOCK_TCPSTART_IX; i<EIPS_USER_MAX_NUM_SOCKETS; i++)
    {
        if(usersock_sockets[i].sockid != USERSOCK_SOCKID_UNUSED)
        {
            // check for error on socket and close if found
            if(getsocketerror(usersock_sockets[i].sockid))
            {
               // Close socket
               eips_usersock_tcpClose(usersock_sockets[i].sockid);
            }

            // add to the file set
            else
            {
                FD_SET(usersock_sockets[i].sockid, &readfds);
                FD_SET(usersock_sockets[i].sockid, &writefds);
            }
        }
    }

    /* check for activity */
//    return_code = select(EIPS_USER_MAX_NUM_SOCK_ID, &readfds, &writefds, NULL, 0);
    //TSM - dont hang here
    return_code = ZeroWaitSelect(EIPS_USER_MAX_NUM_SOCK_ID, &readfds, &writefds, NULL);

    /* error returned */
    if (return_code < 0)
    {
        eips_usersys_fatalError("usersock_process", 0);
    }

    /* We have an event */
    else if (return_code > 0)
    {
        /* New TCP Connection Received */
        if (FD_ISSET(usersock_sockets[USERSOCK_LISTEN_IX].sockid, &readfds))
        {
            /* look for unused socket */
            i = local_findUnusedSocketIx();

            /* out of sockets */
            if(i == -1)
            {
                eips_user_dbprint0("Out of sockets!\r\n");

                // close the socket, we don't have enough resources
                close(accept(usersock_sockets[USERSOCK_LISTEN_IX].sockid, &close_sockaddr, &close_sockport, 1));
            }

            /* try to accept the socket */
            else
            {
                usersock_sockets[i].sockid = accept(usersock_sockets[USERSOCK_LISTEN_IX].sockid,
                                                    &usersock_sockets[i].sockaddr,
                                                    &usersock_sockets[i].sockport, 1);

                /* accept failed */
                if(usersock_sockets[i].sockid <= 0)
                    eips_usersys_fatalError("usersock_process", 1);

                /* inform the RTA source of the new connection */
                else
                    eips_rtasys_onTCPAccept(usersock_sockets[i].sockid);
            }
        } // END-> "New TCP Connection Received"

        /* TCP Data Received or Send Complete */
        for(i=USERSOCK_TCPSTART_IX; i<EIPS_USER_MAX_NUM_SOCKETS; i++)
        {
            /* only check used sockets */
            if (usersock_sockets[i].sockid != USERSOCK_SOCKID_UNUSED)
            {
                /* we received data */
                if (FD_ISSET(usersock_sockets[i].sockid, &readfds))
                {
                    buffer_size = read(usersock_sockets[i].sockid, buffer, USERSOCK_MAX_BUF_SIZ);

                    /* error returned, close socket */
                    if (buffer_size <= 0 )
                        eips_usersock_tcpClose(usersock_sockets[i].sockid);

                    /* get successful */
                    else
                        eips_rtasys_onTCPData (usersock_sockets[i].sockid, (uint8 *)buffer, (uint16) buffer_size);
                }

                /* send completed */
                if (FD_ISSET(usersock_sockets[i].sockid, &writefds))
                {
                    if(usersock_sockets[i].tx_in_prog_flag == TRUE)
                    {
                        /* if a tx was in progress, inform the user when done */
                        eips_rtasys_onTCPSent(usersock_sockets[i].sockid);
                        usersock_sockets[i].tx_in_prog_flag = FALSE;
                    }
                }
            } // END-> "only check used sockets */
        } // END-> "TCP or I/O Data Received or Send Complete"
    } // END-> "We have an event"
} // END-> function

/* ====================================================================
Function:   eips_usersock_sendTCPData
Parameters: socket id
            pointer to socket data
            socket data size
Returns:    N/A

This function sends the passed TCP data to the network client.
======================================================================= */
void eips_usersock_sendTCPData (EIPS_USERSYS_SOCKTYPE socket_id, uint8 * socket_data, uint16 socket_data_size)
{
    int16 ix;

    /* successful send (keep track to inform the user on success */
    ix = local_findSocketIx(socket_id);

    // use write for TCP
    if(ix >= USERSOCK_TCPSTART_IX)
    {
        /* there is an error with the send, close the socket */
        if(write (socket_id, (char *) socket_data, socket_data_size) < 0)
        {
            eips_usersock_tcpClose(socket_id);
	        return;
        }

        usersock_sockets[ix].tx_in_prog_flag = TRUE;
        #if EIPS_PRINTALL > 1
            eips_user_dbprint2("TCP Sent %d bytes [Socket %d]\r\n", socket_data_size, socket_id);
        #endif
    }
}

/* ====================================================================
Function:   eips_usersock_sendUDPData
Parameters: ip address to send the data to
            port to send the data to
            pointer to socket data
            socket data size
Returns:    N/A

This function sends the passed data to the network client.
======================================================================= */
void eips_usersock_sendUDPData (uint32 ipaddr, uint16 port, uint8 * socket_data, uint16 socket_data_size)
{
    UDPPacket pkt;

    // build and send the UDP message
    pkt.SetSourcePort(EIP_CIP_PORT);
    pkt.SetDestinationPort(port);
    pkt.ResetData();
    pkt.AddData(socket_data,socket_data_size);
    pkt.Send(ipaddr);

    #if EIPS_PRINTALL > 1
        eips_user_dbprint2("UDP Sent %d bytes [Socket %d]\n", socket_data_size, usersock_sockets[USERSOCK_UDP_IX].sockid);
    #endif
}

/* ====================================================================
Function:   eips_usersock_sendIOMessage
Parameters: IP Address
            Socket ID
            pointer to message to send
            message size
Returns:    N/A

This function tries to send the message over the passed socket.  Since
this is a udp socket, send failures are ignored.
======================================================================= */
void eips_usersock_sendIOMessage (uint32 ipaddr, uint8 *resp_msg, int16 resp_msg_siz)
{
    UDPPacket pkt;

    // build and send the UDP message
    pkt.SetSourcePort(EIPS_UDPIO_PORT);
    pkt.SetDestinationPort(EIPS_UDPIO_PORT);
    pkt.ResetData();
    pkt.AddData(resp_msg,resp_msg_siz);
    pkt.Send(ipaddr);

    #if EIPS_PRINTALL > 1
        eips_user_dbprint2("I/O Sent %d bytes [Socket %d]\n", resp_msg_siz, usersock_sockets[USERSOCK_IO_IX].sockid);
    #endif
}

/* ====================================================================
Function:   eips_usersock_tcpClose
Parameters: socket id
Returns:    N/A

This function closes the passed socket.
======================================================================= */
void eips_usersock_tcpClose (EIPS_USERSYS_SOCKTYPE socket_id)
{
   int16 return_code;

   /* try to close the socket */
   return_code = close (socket_id);

   /* close was successful, inform RTA source */
   eips_rtasys_onTCPClose(socket_id);

   /* Free the socket to be used */
   local_freeSocketIx(socket_id);
}

/* ====================================================================
Function:   eips_usersock_getSockInfo_sin_addr
Parameters: socket id
Returns:    sin_addr for the passed socket.

This function returns the sin_addr for the passed socket (0 on error).
======================================================================= */
uint32 eips_usersock_getSockInfo_sin_addr (EIPS_USERSYS_SOCKTYPE socket_id)
{
   uint16 ix;

   /* find which socket instance they are interested in */
   ix = local_findSocketIx (socket_id);

   /* the socket does not exist */
   if(ix == EIPS_USER_MAX_NUM_SOCKETS)
      return(0);

   return(usersock_sockets[ix].sockaddr);
}

/* ====================================================================
Function:   eips_usersock_getOurIPAddr
Parameters: N/A
Returns:    32-bit IP Address

This function returns the current IP address of the device.
======================================================================= */
uint32 eips_usersock_getOurIPAddr (void)
{
   	return(gConfigRec.ip_Addr);
}

// -------------------------------------------------------------
//
//                      TCP/IP Object
//
// -------------------------------------------------------------
/* ====================================================================
Function:   eips_usersock_getTCPObj_Attr01_Status
Parameters: pointer to store data
Returns:    General Status - "ERR_SUCCESS" on Success;
                             Error code from eips_cpf.h on Error
C/I/A       0xF5,0x01,0x01

See the TCP/IP Interface Object (0xF5) for information on this attribute.
======================================================================= */
uint8 eips_usersock_getTCPObj_Attr01_Status (uint32 *data_ptr)
{
    // validate the passed data pointer
    if(data_ptr == NULL)
        return(ERR_NO_ATTR_DATA);

    // store the data
    *data_ptr = 1; // valid configuration

    // return Success
    return(ERR_SUCCESS);
}

/* ====================================================================
Function:   eips_usersock_getTCPObj_Attr02_CfgCap
Parameters: pointer to store data
Returns:    General Status - "ERR_SUCCESS" on Success;
                             Error code from eips_cpf.h on Error
C/I/A       0xF5,0x01,0x02

See the TCP/IP Interface Object (0xF5) for information on this attribute.
======================================================================= */
uint8 eips_usersock_getTCPObj_Attr02_CfgCap (uint32 *data_ptr)
{
    // validate the passed data pointer
    if(data_ptr == NULL)
        return(ERR_NO_ATTR_DATA);

    // store the data
    *data_ptr = 0x00;

    // return Success
    return(ERR_SUCCESS);
}

/* ====================================================================
Function:   eips_usersock_getTCPObj_Attr03_CfgCtrl
Parameters: pointer to store data
Returns:    General Status - "ERR_SUCCESS" on Success;
                             Error code from eips_cpf.h on Error
C/I/A       0xF5,0x01,0x03

See the TCP/IP Interface Object (0xF5) for information on this attribute.
======================================================================= */
uint8 eips_usersock_getTCPObj_Attr03_CfgCtrl (uint32 *data_ptr)
{
    // validate the passed data pointer
    if(data_ptr == NULL)
        return(ERR_NO_ATTR_DATA);

    // store the data
    *data_ptr = 0;

    // return Success
    return(ERR_SUCCESS);
}

/* ====================================================================
Function:   eips_usersock_getTCPObj_Attr04_PhyLinkObj
Parameters: pointer to store data
Returns:    General Status - "ERR_SUCCESS" on Success;
                             Error code from eips_cpf.h on Error
C/I/A       0xF5,0x01,0x04

See the TCP/IP Interface Object (0xF5) for information on this attribute.

typedef struct
{
  uint16  PathSiz;
  uint8   Path[12];
}EIPS_PHY_LINK_STRUCT;
======================================================================= */
uint8 eips_usersock_getTCPObj_Attr04_PhyLinkObj (EIPS_PHY_LINK_STRUCT *data_ptr)
{
    // validate the passed data pointer
    if(data_ptr == NULL)
        return(ERR_NO_ATTR_DATA);

    // store the data
    data_ptr->PathSiz = 2; // in words
    data_ptr->Path[0] = 0x20;   // 8-bit Class
    data_ptr->Path[1] = 0xF6;   // Ethernet Link Class
    data_ptr->Path[2] = 0x24;   // 8-bit Instance
    data_ptr->Path[3] = 0x01;   // Instance 1

    // return Success
    return(ERR_SUCCESS);
}

/* ====================================================================
Function:   eips_usersock_getTCPObj_Attr05_IntfCfg
Parameters: pointer to store data
Returns:    General Status - "ERR_SUCCESS" on Success;
                             Error code from eips_cpf.h on Error
C/I/A       0xF5,0x01,0x05

See the TCP/IP Interface Object (0xF5) for information on this attribute.

typedef struct
{
  uint32 IPAddress;
  uint32 NtwkMask;
  uint32 GtwyIPAddress;
  uint32 NameServer;
  uint32 NameServer2;
  EIPS_TCPOBJ_BUF_STRUCT DomainName;
}EIPS_IPADDR_STRUCT;
======================================================================= */
uint8 eips_usersock_getTCPObj_Attr05_IntfCfg (EIPS_IPADDR_STRUCT *data_ptr)
{
    // validate the passed data pointer
    if(data_ptr == NULL)
        return(ERR_NO_ATTR_DATA);

    // store the data
    data_ptr->IPAddress         = gConfigRec.ip_Addr;    // IP Address
    data_ptr->NtwkMask          = gConfigRec.ip_Mask;    // Network Mask
    data_ptr->GtwyIPAddress     = gConfigRec.ip_GateWay; // Gateway IP Address
    data_ptr->NameServer        = 0;    // Name Server IP Address
    data_ptr->NameServer2       = 0;    // Name Server 2 IP Address
    data_ptr->DomainName.Bufsiz = 0;    // Domain Name

    // return Success
    return(ERR_SUCCESS);
}

/* ====================================================================
Function:   eips_usersock_getTCPObj_Attr06_HostName
Parameters: pointer to store data
Returns:    General Status - "ERR_SUCCESS" on Success;
                             Error code from eips_cpf.h on Error
C/I/A       0xF5,0x01,0x06

See the TCP/IP Interface Object (0xF5) for information on this attribute.

typedef struct
{
  uint16  Bufsiz;
  uint8   Buf[64];
}EIPS_TCPOBJ_BUF_STRUCT;
======================================================================= */
uint8 eips_usersock_getTCPObj_Attr06_HostName (EIPS_TCPOBJ_BUF_STRUCT *data_ptr)
{
    // validate the passed data pointer
    if(data_ptr == NULL)
        return(ERR_NO_ATTR_DATA);

    // store the data
    data_ptr->Bufsiz = 0;   // not used
//    data_ptr->Buf[0] = 'A';

    // return Success
    return(ERR_SUCCESS);
}

/* ====================================================================
Function:   eips_usersock_setTCPObj_Attr03_CfgCtrl
Parameters: pointer with new data
Returns:    General Status - "ERR_SUCCESS" on Success;
                             Error code from eips_cpf.h on Error
C/I/A       0xF5,0x01,0x03

See the TCP/IP Interface Object (0xF5) for information on this attribute.
======================================================================= */
uint8 eips_usersock_setTCPObj_Attr03_CfgCtrl (uint32 *data_ptr)
{
    // validate the passed data pointer
    if(data_ptr == NULL)
        return(ERR_NO_ATTR_DATA);

    // data is read only
    return(ERR_ATTR_READONLY);

    // return Success
//    return(ERR_SUCCESS);
}

/* ====================================================================
Function:   eips_usersock_setTCPObj_Attr05_IntfCfg
Parameters: pointer with new data
Returns:    General Status - "ERR_SUCCESS" on Success;
                             Error code from eips_cpf.h on Error
C/I/A       0xF5,0x01,0x05

See the TCP/IP Interface Object (0xF5) for information on this attribute.

typedef struct
{
  uint32 IPAddress;
  uint32 NtwkMask;
  uint32 GtwyIPAddress;
  uint32 NameServer;
  uint32 NameServer2;
  EIPS_TCPOBJ_BUF_STRUCT DomainName;
}EIPS_IPADDR_STRUCT;
======================================================================= */
uint8 eips_usersock_setTCPObj_Attr05_IntfCfg (EIPS_IPADDR_STRUCT *data_ptr)
{
    // validate the passed data pointer
    if(data_ptr == NULL)
        return(ERR_NO_ATTR_DATA);

    // data is read only
    return(ERR_ATTR_READONLY);

    // return Success
//    return(ERR_SUCCESS);
}

/* ====================================================================
Function:   eips_usersock_setTCPObj_Attr06_HostName
Parameters: pointer with new data
Returns:    General Status - "ERR_SUCCESS" on Success;
                             Error code from eips_cpf.h on Error
C/I/A       0xF5,0x01,0x06

See the TCP/IP Interface Object (0xF5) for information on this attribute.

typedef struct
{
  uint16  Bufsiz;
  uint8   Buf[64];
}EIPS_TCPOBJ_BUF_STRUCT;
======================================================================= */
uint8 eips_usersock_setTCPObj_Attr06_HostName (EIPS_TCPOBJ_BUF_STRUCT *data_ptr)
{
    // validate the passed data pointer
    if(data_ptr == NULL)
        return(ERR_NO_ATTR_DATA);

    // data is read only
    return(ERR_ATTR_READONLY);

    // return Success
//    return(ERR_SUCCESS);
}

// -------------------------------------------------------------
//
//                  Ethernet Link Object
//
// -------------------------------------------------------------
/* ====================================================================
Function:   eips_usersock_getEthLinkObj_Attr01_IntfSpeed
Parameters: pointer to store data
Returns:    General Status - "ERR_SUCCESS" on Success;
                             Error code from eips_cpf.h on Error
C/I/A       0xF6,0x01,0x01

See the Ethernet Link Object (0xF6) for information on this attribute.
======================================================================= */
uint8 eips_usersock_getEthLinkObj_Attr01_IntfSpeed (uint32 *data_ptr)
{
    // validate the passed data pointer
    if(data_ptr == NULL)
        return(ERR_NO_ATTR_DATA);

    if (EtherSpeed100())
        *data_ptr = (100); /* 100Mbps */
    else
        *data_ptr = (10); /* 10Mbps */

    // return Success
    return(ERR_SUCCESS);
}

/* ====================================================================
Function:   eips_usersock_getEthLinkObj_Attr02_IntfFlags
Parameters: pointer to store data
Returns:    General Status - "ERR_SUCCESS" on Success;
                             Error code from eips_cpf.h on Error
C/I/A       0xF6,0x01,0x02

See the Ethernet Link Object (0xF6) for information on this attribute.
======================================================================= */
uint8 eips_usersock_getEthLinkObj_Attr02_IntfFlags (uint32 *data_ptr)
{
    // validate the passed data pointer
    if(data_ptr == NULL)
        return(ERR_NO_ATTR_DATA);

	// store the default
	*data_ptr = 0x0C;

    // store the link status
	if(EtherLink())
    	*data_ptr |= 1;

	// store the link duplex
	if(EtherDuplex())
    	*data_ptr |= 2;

    // return Success
    return(ERR_SUCCESS);
}

/* ====================================================================
Function:   eips_usersock_getEthLinkObj_Attr03_MacAddr
Parameters: pointer to store data
Returns:    General Status - "ERR_SUCCESS" on Success;
                             Error code from eips_cpf.h on Error
C/I/A       0xF6,0x01,0x03

See the Ethernet Link Object (0xF6) for information on this attribute.

uint8 madid_6byte_string[6];
======================================================================= */
uint8 eips_usersock_getEthLinkObj_Attr03_MacAddr (uint8 *macid_6byte_string)
{
    // validate the passed data pointer
    if(macid_6byte_string == NULL)
        return(ERR_NO_ATTR_DATA);

    // store the data
	memcpy(macid_6byte_string, gConfigRec.mac_address, 6);

    // return Success
    return(ERR_SUCCESS);
}

/* ====================================================================
Function:   eips_usersock_getEthLinkObj_Attr04_IntfCnts
Parameters: pointer to store data
Returns:    General Status - "ERR_SUCCESS" on Success;
                             Error code from eips_cpf.h on Error
C/I/A       0xF6,0x01,0x04 (Optional)

See the Ethernet Link Object (0xF6) for information on this attribute.

typedef struct
{
    uint32 InOctets;
    uint32 InUcastPackets;
    uint32 InNUcastPackets;
    uint32 InDiscards;
    uint32 InErrors;
    uint32 InUnknownProtos;
    uint32 OutOctets;
    uint32 OutUcastPackets;
    uint32 OutNUcastPackets;
    uint32 OutDiscards;
    uint32 OutErrors;
}EIPS_ELINKOBJ_INTFCNTS_STRUCT;
======================================================================= */
uint8 eips_usersock_getEthLinkObj_Attr04_IntfCnts (EIPS_ELINKOBJ_INTFCNTS_STRUCT *data_ptr)
{
    RTA_UNUSED_PARAM(data_ptr);
/*
    // validate the passed data pointer
    if(data == NULL)
        return(ERR_NO_ATTR_DATA);

    // store the data (set to 0 if unsupported)
    memset(data,0,sizeof(EIPS_ELINKOBJ_INTFCNTS_STRUCT));

    // return Success
    return(ERR_SUCCESS);
*/
    // return "attribute unsupported"
    return(ERR_ATTR_UNSUPP);
}

/* ====================================================================
Function:   eips_usersock_getEthLinkObj_Attr05_MediaCnts
Parameters: pointer to store data
Returns:    General Status - "ERR_SUCCESS" on Success;
                             Error code from eips_cpf.h on Error
C/I/A       0xF6,0x01,0x05 (Optional)

See the Ethernet Link Object (0xF6) for information on this attribute.

typedef struct
{
    uint32 AlignmentErrors;
    uint32 FCSErrors;
    uint32 SingleCollisions;
    uint32 MultipleCollisions;
    uint32 SQETestErrors;
    uint32 DeferredTransmissions;
    uint32 LateCollisions;
    uint32 ExcessiveCollisions;
    uint32 MACTransmitErrors;
    uint32 CarrierSenseErrors;
    uint32 FrameTooLarge;
    uint32 MACReceiveErrors;
}EIPS_ELINKOBJ_MEDIACNTS_STRUCT;
======================================================================= */
uint8 eips_usersock_getEthLinkObj_Attr05_MediaCnts (EIPS_ELINKOBJ_MEDIACNTS_STRUCT *data_ptr)
{
    RTA_UNUSED_PARAM(data_ptr);
/*
    // validate the passed data pointer
    if(data == NULL)
        return(ERR_NO_ATTR_DATA);

    // store the data (set to 0 if unsupported)
    memset(data,0,sizeof(EIPS_ELINKOBJ_MEDIACNTS_STRUCT));

    // return Success
    return(ERR_SUCCESS);
*/
    // return "attribute unsupported"
    return(ERR_ATTR_UNSUPP);
}

/* ====================================================================
Function:   eips_usersock_getEthLinkObj_Attr06_IntfCtrl
Parameters: pointer to store data
Returns:    General Status - "ERR_SUCCESS" on Success;
                             Error code from eips_cpf.h on Error
C/I/A       0xF6,0x01,0x06 (Optional)

See the Ethernet Link Object (0xF6) for information on this attribute.

typedef struct
{
    uint16 ControlBits;
    uint16 ForcedInterfaceSpeed;
}EIPS_ELINKOBJ_INTFCTRL_STRUCT;
======================================================================= */
uint8 eips_usersock_getEthLinkObj_Attr06_IntfCtrl (EIPS_ELINKOBJ_INTFCTRL_STRUCT *data_ptr)
{
    RTA_UNUSED_PARAM(data_ptr);
/*
    // validate the passed data pointer
    if(data == NULL)
        return(ERR_NO_ATTR_DATA);

    // store the data
    data->ControlBits = 0;
    data->ForcedInterfaceSpeed = 0;

    // return Success
    return(ERR_SUCCESS);
*/
    // return "attribute unsupported"
    return(ERR_ATTR_UNSUPP);
}

/* ====================================================================
Function:   eips_usersock_setEthLinkObj_Attr06_IntfCtrl
Parameters: pointer to store data
Returns:    General Status - "ERR_SUCCESS" on Success;
                             Error code from eips_cpf.h on Error
C/I/A       0xF6,0x01,0x06 (Optional)

See the Ethernet Link Object (0xF6) for information on this attribute.

typedef struct
{
    uint16 ControlBits;
    uint16 ForcedInterfaceSpeed;
}EIPS_ELINKOBJ_INTFCTRL_STRUCT;
======================================================================= */
uint8 eips_usersock_setEthLinkObj_Attr06_IntfCtrl (EIPS_ELINKOBJ_INTFCTRL_STRUCT *data_ptr)
{
    RTA_UNUSED_PARAM(data_ptr);
/*
    // validate the passed data pointer
    if(data == NULL)
        return(ERR_NO_ATTR_DATA);

    // data is read only
    return(ERR_ATTR_READONLY);

    // return Success
//    return(ERR_SUCCESS);
*/
    // return "attribute unsupported" if some Ethernet Link attributes are settable
    return(ERR_ATTR_UNSUPP);
}

/**/
/* ******************************************************************** */
/*                      LOCAL FUNCTIONS                                 */
/* ******************************************************************** */
/* ====================================================================
Function:     UdpReaderMain
Parameters:   port
Returns:      N/A

The UCOS task fucntion that just sits and waits for UDP packets...
======================================================================= */
void UdpReaderMain(void * pd)
{
    WORD len;
    int port=(int )pd;
    eips_user_dbprint1("UDP Listening on port %d\r\n",port);

    OS_FIFO fifo;
    OSFifoInit(&fifo);

    //Register to listen for UDP packets on port number 'port'
    RegisterUDPFifo(port, &fifo);

    while (1)
    {
        //We construct a UDP packet object using the FIFO....
        //This constructor will only return when we have received a packet
        UDPPacket upkt(&fifo,0);

        //Did we get a valid packet? or just time out?
        if (upkt.Validate())
        {
            len = upkt.GetDataSize();
            eips_rtasys_onUDPData (upkt.GetSourceAddress(), upkt.GetSourcePort(), (uint8 *)upkt.GetDataBuffer(), (uint16) len);

            //Close the Task if a fatal error in Ethernet/IP software
            DWORD nState = OSFlagState( &gEIPTaskErrorFlag);	//set b0
            if(nState !=0 )
            	OSTaskDelete();

        }
    }
}

/* ====================================================================
Function:     UdpIOReaderMain
Parameters:   port
Returns:      N/A

The UCOS task fucntion that just sits and waits for UDP I/O packets...
======================================================================= */
void UdpIOReaderMain(void * pd)
{
    WORD len;
    int port=(int )pd;
    eips_user_dbprint1("UDP I/O Listening on port %d\r\n",port);

    OS_FIFO fifo;
    OSFifoInit(&fifo);

    //Register to listen for UDP packets on port number 'port'
    RegisterUDPFifo(port, &fifo);

    while (1)
    {
        //We construct a UDP packet object using the FIFO....
        //This constructor will only return when we have received a packet
        UDPPacket upkt(&fifo,0);

        //Did we get a valid packet? or just time out?
        if (upkt.Validate())
        {
            len = upkt.GetDataSize();
            eips_rtasys_onIOData (upkt.GetSourceAddress(), (uint8 *)upkt.GetDataBuffer(), (uint16) len);
        }
        //Close the Task if a fatal error in Ethernet/IP software
        DWORD nState = OSFlagState( &gEIPTaskErrorFlag);	//
        if(nState !=0 )
        	OSTaskDelete();

    }
}

/* ====================================================================
Function:   local_startTcpListen
Parameters: listen socket structure pointer
Returns:    N/A

This function attemps to open and bind a TCP socket based on the
parameters stored in the socket structure.  This socket is used to
listen for other socket connect requests.
======================================================================= */
static void local_startTcpListen (USERSOCK_SOCKSTRUCT *user_sock)
{
   /* get socket */
   user_sock->sockid = listen(INADDR_ANY, EIP_CIP_PORT, 1);
}

/* ====================================================================
Function:   local_findSocketIx
Parameters: socket id
Returns:    ix (-1 on error)

This function searches through the array of socket ids and returns the
index for the matching element.
======================================================================= */
static int16 local_findSocketIx (EIPS_USERSYS_SOCKTYPE socket_id)
{
   int16 i;

   /* search through the list of sockets for the passed socket id */
   for(i=0; i<EIPS_USER_MAX_NUM_SOCKETS; i++)
      if (usersock_sockets[i].sockid == socket_id)
	 return(i);
   /* no match was found */
   return(-1);
}

/* ====================================================================
Function:   local_freeSocketIx
Parameters: socket id
Returns:    N/A

This function searches through the array of socket ids for the passed
socket id and frees the socket ix.
======================================================================= */
static void local_freeSocketIx (EIPS_USERSYS_SOCKTYPE socket_id)
{
    int16 i;

    /* search through the list of sockets for the passed socket id */
    for(i=0; i<EIPS_USER_MAX_NUM_SOCKETS; i++)
    {
        if (usersock_sockets[i].sockid == socket_id)
        {
            usersock_sockets[i].sockid = USERSOCK_SOCKID_UNUSED;
            usersock_sockets[i].sockaddr = 0;
            usersock_sockets[i].sockport = 0;
            usersock_sockets[i].tx_in_prog_flag = FALSE;
            return;
        }
    }
}

/* ====================================================================
Function:   local_findUnusedSocketIx
Parameters: N/A
Returns:    ix (-1 on error)

This function returns the first free index or -1 if all sockets are
used.
======================================================================= */
static int16 local_findUnusedSocketIx (void)
{
    int16 i;

    /* search through the list of sockets for a free socket */
    for(i=USERSOCK_TCPSTART_IX; i<EIPS_USER_MAX_NUM_SOCKETS; i++)
        if (usersock_sockets[i].sockid == USERSOCK_SOCKID_UNUSED)
            return(i);
    /* no free sockets */
    return(-1);
}

/* *********** */
/* END OF FILE */
/* *********** */

