/* ********************************************************************
Module:  mbtcp_usersys.h
Author:  Jamin D. Wendorf     12/2003
         (c)2003 REAL TIME AUTOMATION

This file contains user definitions specific to Modbus TCP General
behavior.
M.McKiernan          26.09.2007
               define MBTCP_DEBUG 0 // disable debug printing
               Added prototype - void FGOneHzMBTCPErrorChecking(void ); - call in FGOneHz.cpp
*********************************************************************** */
#ifndef __MBTCP_USERSYS_H__
#define __MBTCP_USERSYS_H__

/* Include files we need */
#include <taskmon.h>
#include <smarttrap.h>
#include "predef.h"
#include <stdio.h>
#include <ctype.h>
#include <startnet.h>
#include <serial.h>
#include <iosys.h>
#include <utils.h>
#include <ip.h>
#include <tcp.h>
#include <ucos.h>
#include <udp.h>
#include <autoupdate.h>
#include <bsp.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>


void FGOneHzMBTCPErrorChecking(void );

/* we put lookup tables in code space to save RAM */
#define MBTCP_CODESPACE const

/* MB_GETHI returns the high byte of w */
#define MBTCP_GETHI(w)   ((uint8)(((uint16)w >> 8) & 0x00FF))

/* MB_GETLO returns the low byte of w */
#define MBTCP_GETLO(w)   ((uint8)((uint16)w & 0x00FF))

/* MB_SETHI stores b to the high byte of w */
#define MBTCP_SETHI(w,b) (w = (uint16)((uint16)w&0x00FF)|(uint16)(((uint16)b<<8)&0xFF00))

/* MB_SETLO stores b to the low byte of w */
#define MBTCP_SETLO(w,b) (w = (uint16)((uint16)w&0xFF00)|(uint16)((uint16)b&0x00FF))

/* MB_MODX is the macro for v%b */
#define MBTCP_MODX(v,b)  (v-((v/b)*b))

#define MBTCP_DEBUG 1 /* define this to 1 to enable debug printing */
//#define MBTCP_DEBUG 0 /* disable debug printing */
#if MBTCP_DEBUG > 0
  #define mbtcp_user_dbprint0(f) printf(f)
  #define mbtcp_user_dbprint1(f,a1) printf(f,a1)
  #define mbtcp_user_dbprint2(f,a1,a2) printf(f,a1,a2)
  #define mbtcp_user_dbprint3(f,a1,a2,a3) printf(f,a1,a2,a3)
  #define mbtcp_user_dbprint4(f,a1,a2,a3,a4) printf(f,a1,a2,a3,a4)
  #define mbtcp_user_dbprint5(f,a1,a2,a3,a4,a5) printf(f,a1,a2,a3,a4,a5)
  #define mbtcp_user_dbprint6(f,a1,a2,a3,a4,a5,a6) printf(f,a1,a2,a3,a4,a5,a6)
#else
  #define mbtcp_user_dbprint0(f)
  #define mbtcp_user_dbprint1(f,a1)
  #define mbtcp_user_dbprint2(f,a1,a2)
  #define mbtcp_user_dbprint3(f,a1,a2,a3)
  #define mbtcp_user_dbprint4(f,a1,a2,a3,a4)
  #define mbtcp_user_dbprint5(f,a1,a2,a3,a4,a5)
  #define mbtcp_user_dbprint6(f,a1,a2,a3,a4,a5,a6)
#endif

#endif /* __MBTCP_USERSYS_H__ */
