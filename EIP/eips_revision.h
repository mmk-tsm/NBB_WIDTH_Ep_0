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
 *     Module Name: eips_revision.h
 *          Author: Jamin D. Wendorf (jwendorf@rtaautomation.com)
 *        Language: Ansi C
 * Compile Options: N/A
 * Compile defines: N/A
 *       Libraries: N/A
 *    Link Options: N/A
 *
 * Description.
 * =======================================================================
 * This file contains the stack revision history.
 *
 */
// M.McKiernan 		23.12.2009
// No real changes made, see
// TSM just testing. - Following works, i.e can enter revision. Leave as was, 2.14 is EIP software rev.
#ifndef __EIPS_REVISION_H__
#define __EIPS_REVISION_H__

#define EIPS_VERSION_STRING "2.14"
#define EIPS_MAJOR_REV       0x02
#define EIPS_MINOR_REV       0x0e
#define EIPS_REVISION        0x0e02 /* minor/major */

// TSM just testing. - Following works, i.e can enter revision. Leave as was, 2.14 is EIP software rev.
/*
#define EIPS_VERSION_STRING "3.07"
#define EIPS_MAJOR_REV       0x03
#define EIPS_MINOR_REV       0x07
#define EIPS_REVISION        0x0703 */  /* minor/major */

/*
===========================================================================
12/16/2009  REVISION 02.14 (Author: Jamin D. Wendorf)
===========================================================================
1. Re-certified for ODVA Conformance A-7 (server side only).
2. Removed unsupported additional error codes
3. Modified Multicast Algorith to match specification
4. Close TCP connection if Class 3 Explicit connection times out
5. Changed RTA contact information in copyright header

===========================================================================
05/04/2009  REVISION 02.13 (Author: Emily Brockman)
===========================================================================
1. Added basic support for the CIP Connection Configuration Object (0xF3)
2. #define will allow CCO Object to be included or excluded easily from build

===========================================================================
02/03/2009  REVISION 02.12 (Author: Jamin D. Wendorf) Re-certified with EDITT
01/22/2009  REVISION 02.12 (Author: Jamin D. Wendorf)
===========================================================================
1. Added support for multiple copies on Windows.
2. Removed C++ comments (//) replaced with C comments
3. Conformed to A-6, Sept 17, 2008

===========================================================================
07/16/2008  REVISION 02.11 (Author: Jamin D. Wendorf)  ** Conform to A-6 **
                                                    ** Jun 24 2008 Build **
===========================================================================
1. Added #define "EIPS_USER_MIN_TICK_RES_IN_USEC" to allow user to specify
   the minimum RPI, independent of the tick resolution supported.
2. Never allow 0 for the RPI.
3. Properly free connection resources on error.

===========================================================================
01/15/2008  REVISION 02.10 (Author: Jamin D. Wendorf)  ** Conform to A-5 **
===========================================================================
1.  Fixed explicit connection timeouts on Duplicate ForwardOpen and RPI=0
2.  Changed timer code to reserve timer 0
3.  Added better error handling for ForwardOpen and ForwardClose for
    objects that don't support then.
4.  Changed all copyright strings to include inc. after RTA
5.  Expanded PCCC to support strings and bits
6.  Added code to emulate 1761-NET-ENI (Rockwell Automation) to allow OCX
    devices to access our device via PCCC.
7.  Increase revision to 2.10 to allow for various beta code shipped and
    avoid any confusion.

===========================================================================
04/02/2007  REVISION 02.05 (Author: Jamin D. Wendorf)  ** Conform to A-4 **
===========================================================================
1.  Encap (Reg Session) shouldn't send response on (opt != 0)
2.  Changed CPF to handle all service code errors
3.  Support 16-bit Class and Instance ID's
4.  Changed "data" keyword to "data_ptr"

===========================================================================
06/06/2006  REVISION 02.04 (Author: Jamin D. Wendorf)
===========================================================================
1.  Fixed close error in client code (eipc_client.c).

===========================================================================
05/24/2006  REVISION 02.03 (Author: Jamin D. Wendorf)
===========================================================================
1.  Passed the EIP Workshop Recommendations at Plugfest 5.

===========================================================================
04/14/2006  REVISION 02.02 (Author: Jamin D. Wendorf)
===========================================================================
1.  Add PCCC OnDemand.
2.  Improve Client Example Code to separate all test cases.

===========================================================================
02/07/2006  REVISION 02.01 (Author: Jamin D. Wendorf)
===========================================================================
1.  Change all file headers to have a more complete copyright notice.
2.  Removed "EIPS_IO_SUPPORTED" since it is now a standard feature.
3.  Added code for EIP Workshop Recommendations.
4.  Added support for Client (#define) to simplify upgrades.
5.  Changed PCCC to pass all read and write requests to the user.
6.  Added Float support for PCCC.
7.  Added support for 8-bit, 16-bit and 32-bit Assemblies (#define)
8.  Added pass through support for all TCP and Ethernet Link Object
    attributes. Also added Set capability where supported by ODVA.
9.  Added support for Input Only and Listen Only I/O connections.
10. Added support for multiple Configuration Assemblies.

===========================================================================
03/29/2004  REVISION 01.20 (Author: Jamin D. Wendorf)
===========================================================================
1.  Fixed minor conformance issues (A3.6).
2.  Fixed UDP interface to work with UDP implementations that don't use
    a socket id, just an IP Address and Port.

===========================================================================
03/26/2003  REVISION 01.01 (Author: Jamin D. Wendorf)
===========================================================================
1.  Sample Version - Cleaned up test code

*/

#endif /* __EIPS_REVISION_H__ */

