/*********************************************************************
Module:  mbtcp_usersock.c
Author:  Jamin D. Wendorf     02/2003
         (c)2003 Real Time Automation

This file contains socket definitions.  When the init routine is
called, the user must open a listening socket to the port 502.  When
data is received, it is passed on to "mbtcp_rtasys_onTCPData" for
processing.

 M.McKiernan				30.4.2010
 Added function - void mbtcp_usersock_closeall (void)

*********************************************************************** */

/* ---------------------------- */
/* INCLUDE FILES                */
/* ---------------------------- */
#include "rta_system.h"
#include "mbtcp_system.h"
#include "ethernet.h"

/* ---------------------------- */
/* LOCAL STRUCTURE DEFINITIONS  */
/* ---------------------------- */
#define USERSOCK_MAX_BUF_SIZ 300
#define USERSOCK_LISTEN_IX   0
#define USERSOCK_TCPSTART_IX 1
#define USERSOCK_SOCKID_UNUSED -1

typedef struct
{
    IPADDR sockaddr;                // socket IP Address
    WORD sockport;                  // socket Port
    MBTCP_USERSYS_SOCKTYPE sockid;   // socket IP
    uint8 tx_in_prog_flag;          // transmit in progress flag
}USERSOCK_SOCKSTRUCT;

/* ---------------------------- */
/* STATIC VARIABLES             */
/* ---------------------------- */
static USERSOCK_SOCKSTRUCT usersock_sockets [MBTCP_USER_MAX_NUM_SOCKETS];

/* ---------------------------- */
/* LOCAL FUNCTIONS              */
/* ---------------------------- */
static void   local_startTcpListen     (USERSOCK_SOCKSTRUCT *user_sock);
static int16  local_findSocketIx       (MBTCP_USERSYS_SOCKTYPE socket_id);
static void   local_freeSocketIx       (MBTCP_USERSYS_SOCKTYPE socket_id);
static int16  local_findUnusedSocketIx (void);

/* ---------------------------- */
/* MISCELLANEOUS                */
/* ---------------------------- */

/**/
/* ******************************************************************** */
/*                EXTERN FUNCTIONS (not in mbtcp_proto.h)                */
/* ******************************************************************** */
/* ====================================================================
Function:     mbtcp_usersock_process
Parameters:   N/A
Returns:      N/A

The sockets process code is the task or function call which processes
Modbus TCP/IP Server events. These events include TCP data received,
connection open events, close events, data packets received and send
complete events.
======================================================================= */
void mbtcp_usersock_process (void)
{
    int return_code;
    int16 i;
    int16 buffer_size;
    char buffer[USERSOCK_MAX_BUF_SIZ];
    fd_set readfds;
    fd_set writefds;
    fd_set exceptfds;
    unsigned long timeout;

    /* use the select() call to check multiple sockets with one call */
    timeout = 0; /* don't wait (if we do need to wait, min is 10ms) */

    /* we need to zero the socket set every time */
    FD_ZERO(&readfds);
    FD_ZERO(&writefds);
    FD_ZERO(&exceptfds);

    /* add the TCP listening socket to the read set */
    FD_SET(usersock_sockets[USERSOCK_LISTEN_IX].sockid, &readfds);
    FD_SET(usersock_sockets[USERSOCK_LISTEN_IX].sockid, &exceptfds);

    /* add any other sockets that are being used */
    for(i=USERSOCK_TCPSTART_IX; i<MBTCP_USER_MAX_NUM_SOCKETS; i++)
    {
        if(usersock_sockets[i].sockid != USERSOCK_SOCKID_UNUSED)
        {
            FD_SET(usersock_sockets[i].sockid, &readfds);
	        FD_SET(usersock_sockets[i].sockid, &writefds);
	        FD_SET(usersock_sockets[i].sockid, &exceptfds);
        }
    }

    /* check for activity */
    return_code = ZeroWaitSelect(MBTCP_USER_MAX_NUM_SOCK_ID, &readfds, &writefds, &exceptfds);

    /* error returned */
    if (return_code < 0)
    {
        return; // stop processing
    }

    /* We have an event */
    else if (return_code > 0)
    {
        for(i=0; i<MBTCP_USER_MAX_NUM_SOCKETS; i++)
        {
            // socket closed
            if (FD_ISSET(usersock_sockets[i].sockid, &exceptfds))
            {
                mbtcp_user_dbprint1("mbtcp - select error data event on %d\r\n",usersock_sockets[i].sockid);
                mbtcp_usersock_tcpClose(usersock_sockets[i].sockid);
            }
        }

        /* New TCP Connection Received */
        if (FD_ISSET(usersock_sockets[USERSOCK_LISTEN_IX].sockid, &readfds))
        {
            /* look for unused socket */
            i = local_findUnusedSocketIx();

            /* out of sockets */
            if(i == -1)
            {
                mbtcp_user_dbprint0("mbtcp out of sockets!\n");
            }

            /* try to accept the socket */
            else
            {
                usersock_sockets[i].sockid = accept(usersock_sockets[USERSOCK_LISTEN_IX].sockid,
                                                    &usersock_sockets[i].sockaddr,
                                                    &usersock_sockets[i].sockport, 1);

                /* accept failed */
                if(usersock_sockets[i].sockid < 0)
                    mbtcp_usersys_fatalError ("usersock_process", 2);

                /* inform the RTA source of the new connection */
                else
                    mbtcp_rtasys_onTCPAccept(usersock_sockets[i].sockid);
            }
        } // END-> "New TCP Connection Received"

        /* TCP Data Received or Send Complete */
        for(i=USERSOCK_TCPSTART_IX; i<MBTCP_USER_MAX_NUM_SOCKETS; i++)
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
                        mbtcp_usersock_tcpClose(usersock_sockets[i].sockid);

                    /* get successful */
                    else
                        mbtcp_rtasys_onTCPData (usersock_sockets[i].sockid, (uint8 *)buffer, (uint16) buffer_size);
                }

                /* send completed */
                if (FD_ISSET(usersock_sockets[i].sockid, &writefds))
                {
                    if(usersock_sockets[i].tx_in_prog_flag == TRUE)
                    {
                        /* if a tx was in progress, inform the user when done */
                        mbtcp_rtasys_onTCPSent(usersock_sockets[i].sockid);
                        usersock_sockets[i].tx_in_prog_flag = FALSE;
                    }
                }
            } // END-> "only check used sockets */
        } // END-> "TCP or I/O Data Received or Send Complete"
    } // END-> "We have an event"
} // END-> function

/**/
/* ******************************************************************** */
/*                      GLOBAL FUNCTIONS                                */
/* ******************************************************************** */
/* ====================================================================
Function:     mbtcp_usersock_init
Parameters:   N/A
Returns:      N/A

This function initialize all user socket variables.
======================================================================= */
void mbtcp_usersock_init (void)
{
    int16 i;

    /* initialize all static variables */
    for(i=0; i<MBTCP_USER_MAX_NUM_SOCKETS; i++)
    {
        usersock_sockets[i].sockid = USERSOCK_SOCKID_UNUSED;
        usersock_sockets[i].sockaddr = 0;
        usersock_sockets[i].sockport = 0;
        usersock_sockets[i].tx_in_prog_flag = FALSE;
    }

    /* start a listening socket */
    local_startTcpListen (&usersock_sockets[USERSOCK_LISTEN_IX]);
}

/* ====================================================================
Function:     mbtcp_usersock_sendData
Parameters:   socket id
              pointer to socket data
              socket data size
Returns:      N/A

This function sends the passed data to the network client.
======================================================================= */
void mbtcp_usersock_sendData (MBTCP_USERSYS_SOCKTYPE socket_id, uint8 * socket_data, uint16 socket_data_size)
{
    int16 return_code, ix;

    /* successful send (keep track to inform the user on success */
    ix = local_findSocketIx(socket_id);
    if(ix)
    {
        // use send for TCP, sendto for UDP
        if(ix >= USERSOCK_TCPSTART_IX)
	       return_code = write (socket_id, (char *) socket_data, socket_data_size);

        else
            return_code = write (socket_id, (char *) socket_data, socket_data_size);

        /* there is an error with the send, close the socket */
        if(return_code < 0)
        {
	       mbtcp_usersock_tcpClose(socket_id);
	       return;
        }

        usersock_sockets[ix].tx_in_prog_flag = TRUE;
#ifdef MBTCP_PRINTALL
        mbtcp_user_dbprint2("Sent %d bytes [Socket %d]\n", socket_data_size, socket_id);
#endif
    }
}

/* ====================================================================
Function:     mbtcp_usersock_tcpClose
Parameters:   socket id
Returns:      N/A

This function closes the passed socket.
======================================================================= */
void mbtcp_usersock_tcpClose (MBTCP_USERSYS_SOCKTYPE socket_id)
{
    int16 return_code;

    mbtcp_user_dbprint1("mbtcp_usersock_tcpClose %d\r\n", socket_id);

    /* try to close the socket */
    return_code = close (socket_id);

    /* close was successful, inform RTA source */
    if(return_code == 0)
    {
        mbtcp_rtasys_onTCPClose(socket_id);
    }

    /* Free the socket to be used */
    local_freeSocketIx(socket_id);
}

/**/
/* ******************************************************************** */
/*                      LOCAL FUNCTIONS                                 */
/* ******************************************************************** */
/* ====================================================================
Function:     local_startTcpListen
Parameters:   listen socket structure pointer
Returns:      N/A

This function attemps to open and bind a TCP socket based on the
parameters stored in the socket structure.  This socket is used to
listen for other socket connect requests.
======================================================================= */
static void local_startTcpListen (USERSOCK_SOCKSTRUCT *user_sock)
{
    /* initialize listen socket */
    user_sock->sockaddr = 0;
    user_sock->sockport = 0;

    /* get socket */
    user_sock->sockid = listen(INADDR_ANY, MBTCP_PORT, 1);

    /* listen failed */
    if(user_sock->sockid  < 0)
    {
        mbtcp_usersys_fatalError ("local_startTcpListen", 2);
        return;
    }
}

/* ====================================================================
Function:     local_findSocketIx
Parameters:   socket id
Returns:      ix (-1 on error)

This function searches through the array of socket ids and returns the
index for the matching element.
======================================================================= */
static int16 local_findSocketIx (MBTCP_USERSYS_SOCKTYPE socket_id)
{
   int16 i;

   /* search through the list of sockets for the passed socket id */
   for(i=0; i<MBTCP_USER_MAX_NUM_SOCKETS; i++)
      if (usersock_sockets[i].sockid == socket_id)
         return(i);
   /* no match was found */
   return(-1);
}

/* ====================================================================
Function:     local_freeSocketIx
Parameters:   socket id
Returns:      N/A

This function searches through the array of socket ids for the passed
socket id and frees the socket ix.
======================================================================= */
static void local_freeSocketIx (MBTCP_USERSYS_SOCKTYPE socket_id)
{
    int16 i;

    /* search through the list of sockets for the passed socket id */
    for(i=0; i<MBTCP_USER_MAX_NUM_SOCKETS; i++)
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
Function:     local_findUnusedSocketIx
Parameters:   N/A
Returns:      ix (-1 on error)

This function returns the first free index or -1 if all sockets are
used.
======================================================================= */
static int16 local_findUnusedSocketIx (void)
{
    int16 i;

    /* search through the list of sockets for a free socket */
    for(i=USERSOCK_TCPSTART_IX; i<MBTCP_USER_MAX_NUM_SOCKETS; i++)
        if (usersock_sockets[i].sockid == USERSOCK_SOCKID_UNUSED)
            return(i);

    /* no free sockets */
    return(-1);
}

// TSM functiion
// Correspondance from Jamin of RTA
// You should be able to write a function in mbtcp_usersock.cpp to close all sockets.
void mbtcp_usersock_closeall (void)
{
	int i;
/* initialize all static variables */

	for(i=0; i<MBTCP_USER_MAX_NUM_SOCKETS; i++)
    {
           if(usersock_sockets[i].sockid != USERSOCK_SOCKID_UNUSED)
        	   close(usersock_sockets[i].sockid);

     }

}
/* *********** */
/* END OF FILE */
/* *********** */
