//////////////////////////////////////////////////////
// Filename = Email.cpp
//
// Functions for sending email.
// 
// P.Smith                              20/04/07
// added mfprintf,miprintf,mtprintf

#include "predef.h"
#include <stdio.h>
#include <ctype.h>
#include <startnet.h>
#include <autoupdate.h>
#include <mailto.h>
#include <taskmon.h>
#include "string.h"
#include "BatchCalibrationFunctions.h"
#include "Batvars.h"
#include "Batvars2.h"
#include "PrntDiag.h"
#include "Mtttymux.h"



void CheckForPrintedReport( void )
{
    if(g_bPrintNow)
    {
        g_bPrintNow = FALSE;
        PrintCycleDiagnostics();
    }
}



void mfprintf (char * array, float fTemp)
{
    if(!g_bRedirectStdioToPort2)                            
    {
        SetupMttty();
    }
    printf(array,fTemp);
}


void miprintf (char * array, int nTemp)
{
    if(!g_bRedirectStdioToPort2)                            
    {
        SetupMttty();
    }
    iprintf(array,nTemp);
}
void mtprintf (char * array)
{
    if(!g_bRedirectStdioToPort2)                            
    {
        SetupMttty();
    }
    iprintf(array);
}


