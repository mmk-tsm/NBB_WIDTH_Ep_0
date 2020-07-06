#include "BatVars.h"
#include "BatVars2.h"
#include "BatchCalibrationFunctions.h"
#include "General.h"
#include "BatchMBIndices.h"
#include "Init5270.h"
#include <NBBGpio.h>
#include "I2CFuncs.h"
#include "Initcomms.h"
#include "MBMHand.h"
#include <cfinter.h>
#include "Nbbvars.h"
#include "InitNBBcomms.h"
#include <stdio.h>


void PITMeasureHandler( void )
{   
   unsigned int   g_cSBBIOInputs;   
   float  fSeconds,fTime;  
   g_cSBBIOInputs = *(NBB_IPBUFFER);
   if(g_cSBBIOInputs & HW_LSENSORBIT)
   {
     if(g_bPulsed)
      {
      fTime = (float) g_nPulseOnTime;
      fSeconds = (fTime / 3000.0f);      
      printf("\nmeasurement time is %3.4f",fSeconds);
       g_bPulsed = FALSE;
       g_nPulseOnTime = 0;
      }
   }
   else
   {
      g_nPulseOnTime++;
      g_bPulsed = TRUE;
   }
}

