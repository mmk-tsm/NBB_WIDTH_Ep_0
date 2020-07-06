/*
 *            Copyright (c) 2002-2007 by Real Time Automation, Inc.
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
 *  2825 N. Mayfair Road               USA 414.453.5100
 *  Suite 11                           http://www.rtaautomation.com
 *  Wauwatosa, WI 53222                software@rtaautomation.com
 *
 *************************************************************************
 *
 *    Version Date: 01/22/2009
 *         Version: 2.12
 *     Module Name: eips_usersock.h
 *          Author: Jamin D. Wendorf (jwendorf@rtaautomation.com)
 *        Language: Ansi C
 * Compile Options: N/A
 * Compile defines: N/A
 *       Libraries: N/A
 *    Link Options: N/A
 *
 * Description.
 * =======================================================================
 * This file contains user definitions specific to the sockets.
 *
 */

#ifndef __EIPS_USERSOCK_H__
#define __EIPS_USERSOCK_H__

/* define what the socket type is (usually int) */
#define EIPS_USERSYS_SOCKTYPE       int
#define EIPS_USER_MAX_NUM_SOCKETS   10
#define EIPS_USER_MAX_NUM_SOCK_ID   FD_SETSIZE
#define EIPS_USER_SOCKET_UNUSED     ((EIPS_USERSYS_SOCKTYPE) -1)

#endif /* __EIPS_USERSOCK_H__ */
