///////////////////////////////////////////////////////////
// WidthDiagnostics.cpp
// P.Smith							2/7/10

// M.McKiernan						22/5/2020.
// Added void WriteWidthMeasurementDiagnosticDataToSDCard( void ).
// void GenerateWidthMeasurementDiagnostics( void )
// 23.6.2020.	Added sensor angles to SD card diagnostic.   g_fSensorAAngle etc.
//////////////////////////////////////////////////////////

//*****************************************************************************
// INCLUDES
//*****************************************************************************
#include <stdio.h>
#include <Stdlib.h>
#include <String.h>
#include "WidthVars.h"
#include "WidthForeground.h"
#include "WidthDef.h"
#include "BatchCalibrationFunctions.h"
#include "Batvars.h"
#include "Batvars2.h"
#include "WidthRWToNonVolatileMemory.h"
#include "WidthMB.h"
#include "CopyWidthCalibrationDataToIniFile.h"
#include "LoadWidthCalDataFromSDCard.h"
#include "WidthUltrasonics.h"
#include "WidthDiagnostics.h"
#include "SetpointFormat.h"
#include "TimeDate.h"
#include "SdCard.h"

extern  char    *strNewLine;
extern  char    *strAutomatic[];
extern  char    *strManual[];
extern  char    *RevisionNumber;

extern structSetpointData   g_CurrentRecipe;
extern CalDataStruct	g_CalibrationData;
char    g_cWidthPrintBuffer[WIDTH_PRINT_BUFFER_SIZE];

char    g_cWidthMeasurementBuffer[WIDTH_PRINT_BUFFER_SIZE];
bool	g_bWriteWidthMeasurementDiagnosticsToSd = FALSE;
extern float 	g_fUSMethod2Layflat;
extern float	g_fUSMethod2Diameter;

extern float g_fSensorAAngle;	// Sensor A angle.
extern float g_fSensorBAngle;	// Sensor B angle.
extern float g_fSensorCAngle;	// Sensor C angle.
extern float g_fMyWOffset;
extern WORD  g_nMyWOffsetSign;
extern int g_nSHATestMMK;
// Locally declared global variables

void GenerateControlDiagnostics( void )
{
	  char cBuffer[200];  //MAX_PRINT_WIDTH
	  g_cWidthPrintBuffer[0] = '\0';
      sprintf( cBuffer, "%02d-%02d-%04d  %02d:%02d:%02d ", g_CurrentTime[TIME_DATE], g_CurrentTime[TIME_MONTH], g_CurrentTime[TIME_YEAR], g_CurrentTime[TIME_HOUR], g_CurrentTime[TIME_MINUTE], g_CurrentTime[TIME_SECOND] );
      strcat( g_cWidthPrintBuffer, cBuffer );

      // Auto/manual
      if(g_bWidthAuto)
      sprintf( cBuffer, strManual[0] );
      else
      sprintf( cBuffer, strAutomatic[0] );
      strcat( g_cWidthPrintBuffer, cBuffer);


      sprintf( cBuffer, " Set %4.1f Control Average %4.1f Error %4.1f",g_CurrentRecipe.m_fDesiredWidth,g_fWidthControlAverage,g_fWidthControlAverageError);   //
      strcat( g_cWidthPrintBuffer, cBuffer );
      strcat( g_cWidthPrintBuffer, strNewLine );
//      if(fdTelnet>0)
//      iprintf("\n control diagnostics");
//     iprintf("%s",g_cWidthPrintBuffer);
      g_bWriteWidthDiagnosticsToSd = TRUE;
}


void WriteWidthDiagnosticDataToSDCard( void )
{
    char    cName[20];
    BOOL    bOkay = FALSE;
    if((g_CalibrationData.m_nLogToSDCard & LOG_BATCH_LOG_TO_SD_CARD) != 0)
    {
        sprintf( cName, "wl%02d%02d%02d",g_CurrentTime[TIME_YEAR] - TIME_CENTURY,g_CurrentTime[TIME_MONTH],g_CurrentTime[TIME_DATE]);     // Counts per int. stage 1.
        strcat( cName, ".txt");
        bOkay = WriteToSdCardFile(cName,g_cWidthPrintBuffer);
     }
}
//wfh
void WriteWidthMeasurementDiagnosticDataToSDCard( void )
{
    char    cName[20];
    BOOL    bOkay = FALSE;
    if((g_CalibrationData.m_nLogToSDCard & LOG_BATCH_LOG_TO_SD_CARD) != 0)
    {
    	//wfh - using same file as the control diagnostics  (check later)
        sprintf( cName, "wl%02d%02d%02d",g_CurrentTime[TIME_YEAR] - TIME_CENTURY,g_CurrentTime[TIME_MONTH],g_CurrentTime[TIME_DATE]);     // Counts per int. stage 1.
        strcat( cName, ".txt");
        bOkay = WriteToSdCardFile(cName, g_cWidthMeasurementBuffer);
     }
}


void GenerateWidthMeasurementDiagnostics( void )
{
	  char cBuffer[1000];  //MAX_PRINT_WIDTH
	  char cBuf3[7];
	  int i;
//	  g_cWidthMeasurementBuffer[0] = '\0';
	  sprintf( g_cWidthMeasurementBuffer, "\n\n" );
//	  strcat( g_cWidthMeasurementBuffer, strNewLine );		// 2 new lines.

      sprintf( cBuffer, "\n::: %02d/%02d/%04d %02d:%02d:%02d - %s",
    		  g_CurrentTime[TIME_DATE], g_CurrentTime[TIME_MONTH], g_CurrentTime[TIME_YEAR],
    		  g_CurrentTime[TIME_HOUR], g_CurrentTime[TIME_MINUTE], g_CurrentTime[TIME_SECOND], RevisionNumber );
      strcat( g_cWidthMeasurementBuffer, cBuffer );
//      sprintf(cBuffer, " %s", RevisionNumber);
//      strcat( g_cWidthMeasurementBuffer, cBuffer );
//      sprintf(cTempBuffer,"\nBlender Software Revision : %s",RevisionNumber);
      // Auto/manual
      if( g_bWidthAuto == FALSE )
    	  sprintf( cBuffer, strManual[0] );
      else
    	  sprintf( cBuffer, strAutomatic[0] );
      strcat( g_cWidthMeasurementBuffer, cBuffer);
      char cBufStatus[4][10];
//      char cBufStatus2[10];
//      char cBufStatus3[10];
//      char cBufStatus4[10];
//      memset(dest, '\0', sizeof(dest));
//         strcpy(src, "This is tutorialspoint.com");
//         strcpy(dest, src);

      for(i=0; i<4; i++)
     {
		  if(g_bUSSensorOK[i])
			  sprintf(cBufStatus[i], "OK");
		  else
			  sprintf(cBufStatus[i], "xx");
     }
      	// this didnt work.



  	if( (fdTelnet>0) && g_nTSMDebug.m_bUltrasonicsDebug )
  		iprintf("\n.... ....%s - %s - %s - %s", cBufStatus[0],cBufStatus[1],cBufStatus[2],cBufStatus[3]);

      sprintf( cBuffer, "\n Sensor Status: 1=%s 2=%s 3=%s 4=%s ", cBufStatus[0], cBufStatus[1], cBufStatus[2] ,cBufStatus[3]);
      strcat( g_cWidthMeasurementBuffer, cBuffer );

      sprintf( cBuffer, "\n Sensor Distances: d1= %4.1f d2= %4.1f d3= %4.1f d4= %4.1f Cts\\mm= %5.1f ",
    		  g_fUSSensorDistance[0], g_fUSSensorDistance[1], g_fUSSensorDistance[2], g_fUSSensorDistance[3], g_stWidthCalibration.m_fUltrasonicMMConversionFactor );   //
      strcat( g_cWidthMeasurementBuffer, cBuffer );
      sprintf( cBuffer, "\n Radii r1= %4.1f r2= %4.1f r3= %4.1f Avg r= %4.1f ",  g_fUSSensorRadius[0],  g_fUSSensorRadius[1],  g_fUSSensorRadius[2], g_fUSSensorAverageRadius );
      strcat( g_cWidthMeasurementBuffer, cBuffer );
   char cBuf1[10];
   char cBuf2[10];
//   cBuf2[0] = ASCII_NULL;
   /*
      if(g_CurrentRecipe.m_wUSLayflatOffsetSign == NEGATIVE_SIGN)
    	  sprintf( cBuf1, "-%4.1f", g_CurrentRecipe.m_fDesiredWidthOffset );
      else
    	  sprintf( cBuf1, "+%4.1f", g_CurrentRecipe.m_fDesiredWidthOffset );
      */
	   sprintf(cBuf1, " 1. ");		//debugging.
      sprintf(cBuf2, " 2. ");	//debugging.
      //snprintf ( buffer, 100, "The half of %d is %d", 60, 60/2 );
//testonly
      int fVal = 773456;
      snprintf(cBuf3, 6, "v= %d", fVal);
      if( (fdTelnet>0) && g_nTSMDebug.m_bUltrasonicsDebug )
    	  printf("\n..7.DWOffs= %4.1f sign= %d %x MyOoffs= %4.1f MyOfSign= %d",
    			  g_CurrentRecipe.m_fDesiredWidthOffset, g_CurrentRecipe.m_wUSLayflatOffsetSign, g_CurrentRecipe.m_wUSLayflatOffsetSign, g_fMyWOffset, g_nMyWOffsetSign );
/*
 *
      if(g_stWidthCalibration.m_nM2WidthOffsetSign == NEGATIVE_SIGN)
     	  sprintf( cBuf2, "-%4.1f", g_stWidthCalibration.m_fM2WidthOffset );
       else
     	  sprintf( cBuf2, "+%4.1f", g_stWidthCalibration.m_fM2WidthOffset );
*/
      sprintf( cBuffer, "\n D= %4.1f LF= %4.1f Ofs1= %s  D2= %4.1f LF2= %4.1f Ofs2= %s",
    		  g_fUSSensorAverageDiameterWithOffset, g_fUSSensorAverageLayFlatWidth, cBuf1, g_fUSMethod2Diameter, g_fUSMethod2Layflat, cBuf2 );
      strcat( g_cWidthMeasurementBuffer, cBuffer );

// g_stWidthCalibration.m_fM2WidthOffset, g_stWidthCalibration.m_nM2WidthOffsetSign
      //g_stWidthCalibration.m_fDistanceToCentre[i] - g_fUSSensorDistance[i];
      sprintf( cBuffer, "\n .....Calculated Sensor Angles: A= %4.1f B= %4.1f C= %4.1f",
    		  g_fSensorAAngle, g_fSensorBAngle, g_fSensorCAngle );
      strcat( g_cWidthMeasurementBuffer, cBuffer );

      strcat( g_cWidthMeasurementBuffer, strNewLine );
//      if(fdTelnet>0)
//      iprintf("\n control diagnostics");
//     iprintf("%s",g_cWidthMeasurementBuffer);
      g_bWriteWidthMeasurementDiagnosticsToSd = TRUE;
      //test purposes.
  	if( (fdTelnet>0) && g_nTSMDebug.m_bUltrasonicsDebug )
  			iprintf("\n%s SHATestMMK= %d cBuf3= %s", g_cWidthMeasurementBuffer, g_nSHATestMMK, cBuf3);

}




