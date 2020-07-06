
/*-------------------------------------------------------------------------*/
/**
   @file    iniexample.h
   @author  M. McKiernan
   @date    Nov 2006
   @version $Revision: 1.11 $
   @brief   fn prototypes

   P.Smith                                      10/5/07
   remove parse_Batch_ini_file

   P.Smith                                      16/10/08
   remove PUpdate_Batch_ini_file

 */
/*--------------------------------------------------------------------------*/


#ifndef _INIEXAMPLE_H_
#define _INIEXAMPLE_H_

/*---------------------------------------------------------------------------
   								Includes
 ---------------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//TSM addition.

void create_example_ini_file(void);
int  parse_ini_file(char * ini_name);
void create_Batch_ini_file(void);
int parse_Test_ini_file(char * ini_name);
char* Testf_gets(char * ini_name);
void ShowDataAndAddr( PBYTE fromptr, unsigned int len );

long iniparser_getlong(dictionary * d, const char * key, int notfound);
#endif
