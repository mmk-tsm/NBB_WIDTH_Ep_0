
/*-------------------------------------------------------------------------*/
/**
  @file		strlib.c
  @author	N. Devillard
  @date		Jan 2001
  @version	$Revision: 1.9 $
  @brief	Various string handling routines to complement the C lib.

  This modules adds a few complementary string routines usually missing
  in the standard C library.
*/
/*--------------------------------------------------------------------------*/

/*
	$Id: strlib.c,v 1.9 2006-09-27 11:04:11 ndevilla Exp $
	$Author: ndevilla $
	$Date: 2006-09-27 11:04:11 $
	$Revision: 1.9 $
*/

/*---------------------------------------------------------------------------
   								Includes
 ---------------------------------------------------------------------------*/

#include <string.h>
#include <ctype.h>

#include "strlib.h"

/*---------------------------------------------------------------------------
   							    Defines	
 ---------------------------------------------------------------------------*/
#define ASCIILINESZ	200

/*---------------------------------------------------------------------------
  							Function codes
 ---------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------
  							Globals
 ---------------------------------------------------------------------------*/
 char g_cLwc[ASCIILINESZ+1];
 char g_cUpc[ASCIILINESZ+1];
 char g_cCrp[ASCIILINESZ+1];
 char g_cStrp[ASCIILINESZ+1];






/*-------------------------------------------------------------------------*/
/**
  @brief	Convert a string to lowercase.
  @param	s	String to convert.
  @return	ptr to statically allocated string.

  This function returns a pointer to a statically allocated string
  containing a lowercased version of the input string. Do not free
  or modify the returned string! Since the returned string is statically
  allocated, it will be modified at each function call (not re-entrant).
 */
/*--------------------------------------------------------------------------*/

char * strlwc(const char * s)
{
//    static char l[ASCIILINESZ+1];
    int i ;

    if (s==NULL) return NULL ;
    memset(g_cLwc, 0, ASCIILINESZ+1);
    i=0 ;
    while (s[i] && i<ASCIILINESZ) {
        g_cLwc[i] = (char)tolower((int)s[i]);
        i++ ;
    }
    g_cLwc[ASCIILINESZ]=(char)0;
    return (char*)g_cLwc ;
}



/*-------------------------------------------------------------------------*/
/**
  @brief	Convert a string to uppercase.
  @param	s	String to convert.
  @return	ptr to statically allocated string.

  This function returns a pointer to a statically allocated string
  containing an uppercased version of the input string. Do not free
  or modify the returned string! Since the returned string is statically
  allocated, it will be modified at each function call (not re-entrant).
 */
/*--------------------------------------------------------------------------*/

char * strupc(char * s)
{
//    static char l[ASCIILINESZ+1];
    int i ;

    if (s==NULL) return NULL ;
    memset(g_cUpc, 0, ASCIILINESZ+1);
    i=0 ;
    while (s[i] && i<ASCIILINESZ) {
        g_cUpc[i] = (char)toupper((int)s[i]);
        i++ ;
    }
    g_cUpc[ASCIILINESZ]=(char)0;
    return (char*)g_cUpc ;
}



/*-------------------------------------------------------------------------*/
/**
  @brief	Skip blanks until the first non-blank character.
  @param	s	String to parse.
  @return	Pointer to char inside given string.

  This function returns a  pointer to the first non-blank character in the
  given string.
 */
/*--------------------------------------------------------------------------*/

char * strskp(char * s)
{
    char * skip = s;
	if (s==NULL) return NULL ;
    while (isspace((int)*skip) && *skip) skip++;
    return skip ;
} 



/*-------------------------------------------------------------------------*/
/**
  @brief	Remove blanks at the end of a string.
  @param	s	String to parse.
  @return	ptr to statically allocated string.

  This function returns a pointer to a statically allocated string,
  which is identical to the input string, except that all blank
  characters at the end of the string have been removed.
  Do not free or modify the returned string! Since the returned string
  is statically allocated, it will be modified at each function call
  (not re-entrant).
 */
/*--------------------------------------------------------------------------*/

char * strcrop(char * s)
{
//    static char l[ASCIILINESZ+1];
	char * last ;

    if (s==NULL) return NULL ;
    memset(g_cCrp, 0, ASCIILINESZ+1);
	strcpy(g_cCrp, s);
	last = g_cCrp + strlen(g_cCrp);
	while (last > g_cCrp) {
		if (!isspace((int)*(last-1)))
			break ;
		last -- ;
	}
	*last = (char)0;
    return (char*)g_cCrp ;
}



/*-------------------------------------------------------------------------*/
/**
  @brief	Remove blanks at the beginning and the end of a string.
  @param	s	String to parse.
  @return	ptr to statically allocated string.

  This function returns a pointer to a statically allocated string,
  which is identical to the input string, except that all blank
  characters at the end and the beg. of the string have been removed.
  Do not free or modify the returned string! Since the returned string
  is statically allocated, it will be modified at each function call
  (not re-entrant).
 */
/*--------------------------------------------------------------------------*/
char * strstrip(char * s)
{
//    static char l[ASCIILINESZ+1];
	char * last ;
	
    if (s==NULL) return NULL ;
    
	while (isspace((int)*s) && *s) s++;
	
	memset(g_cStrp, 0, ASCIILINESZ+1);
	strcpy(g_cStrp, s);
	last = g_cStrp + strlen(g_cStrp);
	while (last > g_cStrp) {
		if (!isspace((int)*(last-1)))
			break ;
		last -- ;
	}
	*last = (char)0;

	return (char*)g_cStrp ;
}

/* Test code */
#ifdef TEST
int main(int argc, char * argv[])
{
	char * str ;

	str = "\t\tI'm a lumberkack and I'm OK      " ;
	printf("lowercase: [%s]\n", strlwc(str));
	printf("uppercase: [%s]\n", strupc(str));
	printf("skipped  : [%s]\n", strskp(str));
	printf("cropped  : [%s]\n", strcrop(str));
	printf("stripped : [%s]\n", strstrip(str));

	return 0 ;
}
#endif
/* vim: set ts=4 et sw=4 tw=75 */
