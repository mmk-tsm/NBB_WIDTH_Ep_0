///////////////////////////////////////////////////////////
// Foreground.h
//
// Executes all of the foreground routines that are not
// time critical such as the display update.
//
// 
// P.Smith                      16/2/06
// first pass at netburner hardware conversion.
// Bool -> BOOL
// #include <basictypes.h>
////////////////////////////////////////////////////////////

//*****************************************************************************
// MACROS
//*****************************************************************************
#ifndef __FOREGROUND_H__
#define __FOREGROUND_H__

//*****************************************************************************
// INCLUDES
//*****************************************************************************
//nbb #include <stdtypes.h>
#include <basictypes.h>

void Foreground( BOOL bShowTime );


#endif	// __FOREGROUND_H__
