#include <stdlib.h>
#include <stdio.h>
#include <basictypes.h>
#include "Batvars2.h"
#include "Batvars.h"
#include "General.h"
#include "init5270.h"
#include <pins.h>
#include "NBBGpio.h"


#include <basictypes.h>


#include "predef.h" 
#include <stdio.h>
#include <ctype.h>
#include <startnet.h>
#include <autoupdate.h>
#include <dhcpclient.h>
#include <http.h> 
#include <htmlfiles.h>
#include <C:\Nburn\MOD5270\system\sim5270.h>
#include <cfinter.h>
#include "init5270.h"
#include "PITHandler.h"
#include "I2CFuncs.h"
#include "NBBGpio.h"
#include "QSPIconstants.h"
#include "LTC2415.h"
#include "NBBVars.h"
#include "MBSHand.h"
#include "IOMeasure.h"
#include "UltrasonicRead.h"
#include "Batvars2.h"

#define US_DISREGARD_SIZE (5)

void ProcessUSData ( void )
{
int i;
int nLSqSize1 = 5;       // least squares size (no. of points for line)
int nLSqSize2 = 15;      // least squares size (no. of points for line)
long nSUMx,nSUMxx;
float fSUMy,fSUMxy,fukgh1,fukgh2,fukgh3;
int nLSqSize3 = 20;      // least squares size (no. of points for line)

   if(g_bUSDataAvailable && g_bUltrasonicLIWInstalled)
   {
     g_bUSDataAvailable = FALSE;    // clear flag to prevent same data used again.
         
      if( g_nUSBufferIndex > (USMAXENTRIES-1) )   // is buffer full?
      {
         for(i=0; i<(USMAXENTRIES-2); i++)
         {
            g_fCycleUSDistance[i] = g_fCycleUSDistance[i+1];   // slide entries back.
            g_nTimeInCycle[i] = g_nTimeInCycle[i+1];           // slide entries back.
         
         }
         g_nUSBufferIndex = (USMAXENTRIES-1);
      }


      
// put current data in buffer(s) at current index(g_nUSBufferIndex.      
      g_fCycleUSDistance[g_nUSBufferIndex] = g_fUSDistance;     // data coming in once per second.
      g_nTimeInCycle[g_nUSBufferIndex] = g_nCycleSecondNo;     // second no. corresponding to data.

     if( g_nUSBufferIndex >= (nLSqSize1) )   // Enough entries to do 1st (smallest) least squares fit??
      {
         nSUMx = 0; fSUMy = 0; fSUMxy = 0; nSUMxx = 0;
         for (i=0; i<nLSqSize1; i++) 
         {
            nSUMx = nSUMx + g_nTimeInCycle[g_nUSBufferIndex-i];
            fSUMy = fSUMy + g_fCycleUSDistance[g_nUSBufferIndex-i];
            fSUMxy = fSUMxy + g_nTimeInCycle[g_nUSBufferIndex-i]*g_fCycleUSDistance[g_nUSBufferIndex-i];
            nSUMxx = nSUMxx + g_nTimeInCycle[g_nUSBufferIndex-i]*g_nTimeInCycle[g_nUSBufferIndex-i];
         }
 // slope = ( SUMx*SUMy - n*SUMxy ) / ( SUMx*SUMx - n*SUMxx );
   if( g_nUSBufferIndex >= (nLSqSize1 + US_DISREGARD_SIZE) )   // Enough entries to do 2ND ( least squares fit??
        {
            g_fslope1 = ( nSUMx*fSUMy - nLSqSize1*fSUMxy ) / ( nSUMx*nSUMx - nLSqSize1*nSUMxx  );
        }         
      }
// 2nd Least Squares line calculation     
    if( g_nUSBufferIndex >= (nLSqSize2 + US_DISREGARD_SIZE) )   // Enough entries to do 2ND ( least squares fit??
      {
         nSUMx = 0; fSUMy = 0; fSUMxy = 0; nSUMxx = 0;
         for (i=0; i<nLSqSize2; i++) 
         {
            nSUMx = nSUMx + g_nTimeInCycle[g_nUSBufferIndex-i];
            fSUMy = fSUMy + g_fCycleUSDistance[g_nUSBufferIndex-i];
            fSUMxy = fSUMxy + g_nTimeInCycle[g_nUSBufferIndex-i]*g_fCycleUSDistance[g_nUSBufferIndex-i];
            nSUMxx = nSUMxx + g_nTimeInCycle[g_nUSBufferIndex-i]*g_nTimeInCycle[g_nUSBufferIndex-i];
         }
 // slope = ( SUMx*SUMy - n*SUMxy ) / ( SUMx*SUMx - n*SUMxx );
         if((nLSqSize2*nSUMxx - nSUMx*nSUMx) > 0)
         {
             g_fslope2 = ( nSUMx*fSUMy - nLSqSize2*fSUMxy ) / ( nSUMx*nSUMx - nLSqSize2*nSUMxx);
         }
      }
 

 
// 3Rd Least Squares line calculation     
     if( g_nUSBufferIndex >= (nLSqSize3 + US_DISREGARD_SIZE) )   // Enough entries to do 2ND ( least squares fit??
      {
         nSUMx = 0; fSUMy = 0; fSUMxy = 0; nSUMxx = 0;
         for (i=0; i<nLSqSize3; i++) 
         {
            nSUMx = nSUMx + g_nTimeInCycle[g_nUSBufferIndex-i];
            fSUMy = fSUMy + g_fCycleUSDistance[g_nUSBufferIndex-i];
            fSUMxy = fSUMxy + g_nTimeInCycle[g_nUSBufferIndex-i]*g_fCycleUSDistance[g_nUSBufferIndex-i];
            nSUMxx = nSUMxx + g_nTimeInCycle[g_nUSBufferIndex-i]*g_nTimeInCycle[g_nUSBufferIndex-i];
         }
 // slope = ( SUMx*SUMy - n*SUMxy ) / ( SUMx*SUMx - n*SUMxx );
         g_fslope3 = ( nSUMx*fSUMy - nLSqSize3*fSUMxy ) / ( nSUMx*nSUMx - nLSqSize3*nSUMxx );
     
      }


      fukgh1 = (g_fslope1 * (120.0f / 0.75f));
      fukgh2 = (g_fslope2 * (120.0f / 0.75f));
      fukgh3 = (g_fslope3 * (120.0f / 0.75f));
      printf("\n %d  %6.2f mm  S1 %6.2f  S2 %6.2f  S3 %6.2f kg/h %6.2f  kg/h 1 %6.2f kg/h 2 %6.2f kg/h 3 %6.2f ",g_nCycleSecondNo,g_fUSDistance,g_fslope1,g_fslope2,g_fslope3,g_fBatchKgPerHour,fukgh1,fukgh2,fukgh3);
        
// adjust indices for next pass.
         g_nCycleSecondNo++; 
         g_nUSBufferIndex++;
     
          
   }

}


/*  To Do
1. set flag g_bUSDataAvailable when 50Hz average (1sec) of U.S. data available.
2. Call ProcessUSData in foreground (when g_bUSDataAvailable found set).
3. Clear following variables on batch dump & startup:   g_nCycleSecondNo    g_nUSBufferIndex
4. Add this file ProcessUSData.cpp to project.
*/


/* FILE: least-squares.c
 * This program computes a linear model for a set of given data.
 *
 * PROBLEM DESCRIPTION:
 *  The method of least squares is a standard technique used to find
 *  the equation of a straight line from a set of data. Equation for a
 *  straight line is given by 
 *	 y = mx + b
 *  where m is the slope of the line and b is the y-intercept.
 *
 *  Given a set of n points {(x1,y1), x2,y2),...,xn,yn)}, let
 *      SUMx = x1 + x2 + ... + xn
 *      SUMy = y1 + y2 + ... + yn
 *      SUMxy = x1*y1 + x2*y2 + ... + xn*yn
 *      SUMxx = x1*x1 + x2*x2 + ... + xn*xn
 *
 *  The slope and y-intercept for the least-squares line can be 
 *  calculated using the following equations:
 *        slope (m) = ( SUMx*SUMy - n*SUMxy ) / ( SUMx*SUMx - n*SUMxx ) 
 *  y-intercept (b) = ( SUMy - slope*SUMx ) / n
 *
 * AUTHOR: Dora Abdullah (Fortran version, 11/96)
 * REVISED: RYL (converted to C, 12/11/96)
 * ---------------------------------------------------------------------- */
#include <stdlib.h>
#include <math.h>
#include <stdio.h>
/*
int main(int argc, char **argv) {

  double *x, *y;
  double SUMx, SUMy, SUMxy, SUMxx, SUMres, res, slope,
         y_intercept, y_estimate ;
  int i,n;
  FILE *infile;

  infile = fopen("xydata", "r");
  if (infile == NULL) printf("error opening file\n");    
  fscanf (infile, "%d", &n);
  x = (double *) malloc (n*sizeof(double));
  y = (double *) malloc (n*sizeof(double));
  
  SUMx = 0; SUMy = 0; SUMxy = 0; SUMxx = 0;
  for (i=0; i<n; i++) {
    fscanf (infile, "%lf %lf", &x[i], &y[i]);
    SUMx = SUMx + x[i];
    SUMy = SUMy + y[i];
    SUMxy = SUMxy + x[i]*y[i];
    SUMxx = SUMxx + x[i]*x[i];
  }
  slope = ( SUMx*SUMy - n*SUMxy ) / ( SUMx*SUMx - n*SUMxx );
  y_intercept = ( SUMy - slope*SUMx ) / n;
  
  printf ("\n");
  printf ("The linear equation that best fits the given data:\n");
  printf ("       y = %6.2lfx + %6.2lf\n", slope, y_intercept);
  printf ("--------------------------------------------------\n");
  printf ("   Original (x,y)     Estimated y     Residual\n");
  printf ("--------------------------------------------------\n");
      
  SUMres = 0;
  for (i=0; i<n; i++) {
    y_estimate = slope*x[i] + y_intercept;
    res = y[i] - y_estimate;
    SUMres = SUMres + res*res;
    printf ("   (%6.2lf %6.2lf)      %6.2lf       %6.2lf\n", 
	    x[i], y[i], y_estimate, res);
  }
  printf("--------------------------------------------------\n");
  printf("Residual sum = %6.2lf\n", SUMres);
}
*/



