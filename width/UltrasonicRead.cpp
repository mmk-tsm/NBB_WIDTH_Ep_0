///////////////////////////////////////////////////////////
//
// P.Smith                       13/3/07
// g_bUSDataAvailable set to show data available

// M.McKiernan					8.3.2010
// Replaced: SRC9On(); by US_TRIGGER_ON();
// Replaced: SRC9Off(); by US_TRIGGER_OFF();
//
// M.McKiernan wfh				11.5.2020
// in Timer0 interrupt
//			g_nUSSensorEchosReceived[g_wUltrasonicSensorNo]++;
// //wfh removed AdvanceToNextChannel();.  now advance on PIT(L. 643 PITHandler.cpp)      AdvanceToNextChannel();    // advance to next channel
// In void TriggerUltrasonic( void )
//new     added   ie generate a pulse only.
//    Delay_uS(45);
//    SRC5Off();			//Trigger off.
///////////////////////////////////////////////////////////


#include <stdlib.h>
#include <stdio.h>
#include <basictypes.h>
#include "Batvars2.h"
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
#include "BatVars.h"
#include "WidthVars.h"
#include "WidthMB.h"
#include "WidthUltrasonics.h"
#include "OneWire.h"

USDataStruct g_sUSData;

extern "C" {
/* This function sets up  the 5282 interup controller */
void SetIntc(long func, int vector, int level, int prio );  //from yahoo
}

INTERRUPT(Timer0_ISR, 0x2500 )
{
  /* WARNING WARNING WARNING
  Only a very limited set of RTOS functions can be called from
  within an interrupt service routine.
-
  Basically, only OS POST functions and LED functions should be used
  No I/O (read, write or printf may be called), since they can block. */

  sim.timer[ULTRASONIC_TIMER].ter =0x03; // Writing bit 0,1  of TER clears the both Ref & capture interrupts.
  g_nTimer0Count++;        // count the no. of interrupts.

   if(g_nT0InterruptMode == T0_TRIGGER)
   {
      if(sim.timer[ULTRASONIC_TIMER].tcn > WMS_TRIGGER_TIME)
      {
         SRC5Off();   //nbb--width--testonly--
    //	  US_TRIGGER_OFF();
//     sim.timer[0].trr = 82500;    	// Ref register. 1100uS worth of time. (13.3nS period)

         g_nT0InterruptMode = T0_INPUT_CAPTURE;
         sim.timer[ULTRASONIC_TIMER].tmr &= ~TMR_ORRI_BIT;  // Disable the ref compare.
         g_lTriggerTime = sim.timer[ULTRASONIC_TIMER].tcn; // read the counter.

      }
   }
   else if(g_nT0InterruptMode == T0_INPUT_CAPTURE )
   {
     if( sim.timer[ULTRASONIC_TIMER].tcn >  WMS_OSCILLATION_TIME)
     {
      g_lTimer0CaptureRegister = sim.timer[ULTRASONIC_TIMER].tcr; // read the capture register.
      g_lUltrasonicEchoTime[g_wUltrasonicSensorNo] = g_lTimer0CaptureRegister;

      g_nUSSensorEchosReceived[g_wUltrasonicSensorNo]++;

      g_nT0InterruptMode = T0_IDLE;
      AverageUltrasonicReading();
//wfh removed.  now advance on PIT(L. 643 PITHandler.cpp)      AdvanceToNextChannel();    // advance to next channel
     }
   }
   else
      g_lTimer0CounterRegister = sim.timer[ULTRASONIC_TIMER].tcn; // just read the counter.
 }




///////////////////////////////////////////////////////////////////////////////
void SetupTimer0( void )
{
   SetIntc( (long) &Timer0_ISR, 19, 5, 3 );
  J2[31].function( PINJ2_31_TIN0 ); //setup pin 31 for DTIN0 input
//  J2[35].function( PINJ2_35_TIN2 ); //setup pin 35 for DTIN2 input

   BYTE prescale = 0x00;      // gives  prescale  of 1.
  // Reset timer2.  All bits set to reset values.
  sim.timer[ULTRASONIC_TIMER].tmr |= 0x0001; 	// enable timer
  sim.timer[ULTRASONIC_TIMER].tmr = 0;           // reset timer

//Setup Timer
  sim.timer[ULTRASONIC_TIMER].tmr |= 0x0002;    // divide sys clk by 1      4;	// Divide system clock by 16
  sim.timer[ULTRASONIC_TIMER].tmr |= 0x0080;    // b7/b6 = 10, => falling edge trigger.(note inversion by H11L1).
  sim.timer[ULTRASONIC_TIMER].tmr &= ~0x0018;	// Set Ref Int disable and FreeRun = 0, i.e free running.
  sim.timer[ULTRASONIC_TIMER].tmr |= (prescale << 8); // Set prescale

  sim.timer[ULTRASONIC_TIMER].txmr |= 0x00;    	// b7=0, DMA disable, b0=0 incr by 1.
  sim.timer[ULTRASONIC_TIMER].ter |= 0x01;    	// event register, capture on rising edge.
  sim.timer[ULTRASONIC_TIMER].tcn |= 0x00000000;    	// writing to counter clears it always.


  sim.timer[ULTRASONIC_TIMER].trr = 0x0000FFFF;  //ref_count;	// Set ref count...note Dword.


  sim.timer[ULTRASONIC_TIMER].tmr |= 0x0001;    	// Start timer
}

void TriggerUltrasonic( void )
{
    SRC5On();   //nbb--width--testonly--
//	US_TRIGGER_ON();

    sim.timer[ULTRASONIC_TIMER].trr = WMS_TRIGGER_TIME;    	// Ref register. 150uS worth of time. (13.3nS period)
    g_nT0InterruptMode = T0_TRIGGER;
    sim.timer[ULTRASONIC_TIMER].tmr |= TMR_ORRI_BIT;   //0x0010;    // ORRI = 1, => reference request.
    sim.timer[ULTRASONIC_TIMER].tcn |= 0x00000000;    	// writing to counter clears it
//new
    Delay_uS(45);
    SRC5Off();			//Trigger off.
}

int nUSIndex = 0;
float g_fUSDist[5];
int g_nUSCycleCounter=0;
float g_fUSDistancePrev = 0.0f;
float g_fUSDistanceNow = 0.0f;
float g_fUSDistanceIncrement = 0.0f;
int	g_nIncrementCounter = 0;
float g_fIncrementSum = 0.0f;
float g_fIncrementAvg = 0.0f;
float g_fBatchIncrementAvg = 0.0f;
float g_fRelevantBatchWt = 0.0f;

#define USRR_MAX_ENTRIES (5)
float g_fUSRRDistBuf[USRR_MAX_ENTRIES];
int g_nUSRRDistBufIndex = 0;
int g_nUSRRDistBufEntries = 0;		// no. of entries.

float g_fUSRRAvgDistance = 0.0f;

float CalculateUSRRAvgUSDistance( void )
{
int i;
float fSum, fAvg;
	if(g_nUSRRDistBufEntries < USRR_MAX_ENTRIES )
	{
		g_fUSRRDistBuf[g_nUSRRDistBufIndex] = g_fUSDistance;	// put data in buf at index.
		g_nUSRRDistBufEntries++;
		g_nUSRRDistBufIndex++;
		if(g_nUSRRDistBufIndex >=USRR_MAX_ENTRIES )
			g_nUSRRDistBufIndex = USRR_MAX_ENTRIES - 1;		// leave pointing to last entry position.
	}
	else
	{
		for(i=0; i<(USRR_MAX_ENTRIES-1); i++)
		{
			g_fUSRRDistBuf[i] = g_fUSRRDistBuf[i+1];	// slide entries back 1.
		}
		g_fUSRRDistBuf[USRR_MAX_ENTRIES-1] = g_fUSDistance;	// put data in buf at last position
		g_nUSRRDistBufEntries = USRR_MAX_ENTRIES;

	}

	fSum = 0.0f;
	for(i=0; i<g_nUSRRDistBufEntries; i++)
		fSum += g_fUSRRDistBuf[i];

	fAvg = fSum / g_nUSRRDistBufEntries;

	return(fAvg);

}

float g_fSt = 0.0f;					// exponentially smoothed value.
float g_fStPrev = 0.0f;
//#define F_ALPHA (0.33f)				// alpha value used in EMA calculation.
//#define F_ALPHA (0.16f)				// alpha value used in EMA calculation. (@= 2/(N+1) ) N would be secs in this case.
#define F_ALPHA (0.25f)				// alpha value used in EMA calculation. (@= 2/(N+1) ) N would be 7 secs in this case. (2/(7+1) = .25)

void ReadUltrasonicSensorTime ( void )
{
	int j;
	DWORD nMax,nMin;
	DWORD nMax2,nMin2;

	float fAvgDistance = 0.0f;
	float fAvgDistance5S = 0.0f;
	float fNewAvg,fNewDistance;
	bool b5STime = FALSE;

	g_lUSTimeSum += g_lTimer0CaptureRegister;
	g_nUSTimeSumEntries++;
    if( g_nUSTimeSumEntries == 50)
    {
        g_bUSDataAvailable = TRUE;
        g_lUSTimeAvg = g_lUSTimeSum/50;
//        g_fUSDistance = g_lUSTimeAvg / 440.5;    //approx only.
        g_fUSDistance = g_lUSTimeAvg / g_fUltrasonicDivisor;    //


        g_fUSRRAvgDistance = CalculateUSRRAvgUSDistance();

        g_nUSCycleCounter++;
        //put data into cyclic buffer.
        if(g_nUSCycleCounter == 1)
        {
        	nUSIndex = 0;
        	g_fRelevantBatchWt = g_fOneSecondHopperWeightAtEndOfCycle;
        }

        // Calculate EMA (exponential moving avg.)
        if(g_nUSCycleCounter < 8)
        {
        	g_fStPrev = g_fSt;
        	g_fSt = g_fUSDistance;
        }
        else if(g_nUSCycleCounter == 8)
        {
        	g_fStPrev = g_fSt;
        	g_fSt = (g_fUSDistance + g_fSt)/2;	// i.e. mean of this reading & previous.
        }
        else
        {
				//St = @*Xt + (1-@)St-1.
        	g_fStPrev = g_fSt;				// i..e the prvious St value, or from last pass.
        	g_fSt = F_ALPHA*g_fUSDistance + (1-F_ALPHA)*g_fStPrev;
        }

        g_fUSDist[nUSIndex] = g_fUSDistance;
        nUSIndex++;
        if(nUSIndex>=5)
        {
        	b5STime = TRUE;
        	nUSIndex=0;
        	fAvgDistance5S = ( g_fUSDist[0]+g_fUSDist[1]+g_fUSDist[2]+g_fUSDist[3]+g_fUSDist[4] )/5;
        	if(g_nUSCycleCounter >= 15)
        	{
				g_fUSDistancePrev = g_fUSDistanceNow;
				g_fUSDistanceNow = fAvgDistance5S;
				g_fUSDistanceIncrement = g_fUSDistanceNow - g_fUSDistancePrev;
				g_nIncrementCounter++;
				g_fIncrementSum += g_fUSDistanceIncrement;
				g_fIncrementAvg = g_fIncrementSum/g_nIncrementCounter;
        	}
        	else
        	{
				g_fUSDistancePrev = g_fUSDistanceNow;
				g_fUSDistanceNow = fAvgDistance5S;

				g_fBatchIncrementAvg = g_fIncrementAvg;
				g_fIncrementSum = 0.0f;
				g_nIncrementCounter = 0;

				//g_fUSDistanceIncrement = 0;
				//g_nIncrementCounter = 0;
				//g_fIncrementSum = 0.0f;
        	}
        }
        if(g_nUSCycleCounter == 15)
        {
        	fAvgDistance = fAvgDistance5S;
        }
//testonly
//        if(fdTelnet>0)
        /* was already commented out by if(0)
            if(0)  //nbb--width--test only--
        {
        	nMax = 0;
        	nMax2 = 0;
        	nMin = 0xFFFFFFFF;
        	nMin2 = 0xFFFFFFFF;
//pick out min & max values.
        	for(j=0; j<50; j++)
        	{
        		if(g_sUSData.m_lUSBuf[j] > nMax)
        		{
//        			nMax2 = nMax;	// let nMax2 = previous largest.
					nMax = g_sUSData.m_lUSBuf[j];
        		}
        		else if( g_sUSData.m_lUSBuf[j] > nMax2 )
        		{
					nMax2 = g_sUSData.m_lUSBuf[j];
        		}

	        	if(g_sUSData.m_lUSBuf[j] < nMin)
	        	{
//						nMin2 = nMin;	// nMin2 = prev smallest..
						nMin = g_sUSData.m_lUSBuf[j];
	        	}
	        	else if(g_sUSData.m_lUSBuf[j] < nMin2 )
	        	{
						nMin2 = g_sUSData.m_lUSBuf[j];
	        	}

	        	fNewAvg = (g_sUSData.m_lSumUSBuf - nMax - nMax2 - nMin - nMin2)/46.0;
	            fNewDistance = fNewAvg / 440.5;    //approx only.


        	}

        	printf("\r\nUSDistance= %6.2f",  g_fUSDistance );
        	iprintf(" US Values=");
        	//for(j=0; j<50; j++)
        		//iprintf(" %ld", g_sUSData.m_lUSBuf[j]);
           	iprintf(" Max= %ld Max2= %ld Min= %ld Min2= %ld", nMax, nMax2,  nMin, nMin2);
           	//iprintf(" Max2= %ld Min2= %ld", nMax2, nMin2);

           	printf(" NewAvg= %7.2f, fNewDistance= %6.2f", fNewAvg, fNewDistance);
           	printf(" MOT_Set= %d  %6.2f ",g_nExtruderDAValue, (float)(g_nExtruderDAValue/14.0) );
           	printf(" St= %6.2f", g_fSt);
           	printf(" RRAvg= %6.2f", g_fUSRRAvgDistance);

        }  */
 //       if(fdTelnet>0)
        /* was already commented out by if(0)
        if(0)
        {
        	printf("\nCycleCtr= %d BatWt= %6.2f USTimeAvg= %ld  USDistance= %6.2f", g_nUSCycleCounter, g_fRelevantBatchWt, g_lUSTimeAvg, g_fUSDistance );
        	printf(" RRAvg= %ld  RRDistance = %6.2f", g_sUSData.m_lAverageUSBuf, (float)g_sUSData.m_lAverageUSBuf/440.5f );
        	printf(" IntCount= %d T0CR= %ld",g_nTimer0Count, g_lTimer0CaptureRegister);
			if( g_bLevSen1Stat== COVERED)
				printf(" COVERED");
			else
				printf(" UNCOVERED");

			if(fAvgDistance > 1.0f)
				printf(" &Height=");			//row checker for excel.
			else
				printf(" Height= ");
			printf(" %6.2f",fAvgDistance );

			printf(" BatchIncrementAvg= %6.2f", g_fBatchIncrementAvg);

			if(b5STime)
			{
				printf(" DistanceChange= %6.2f Ctr= %d",g_fUSDistanceIncrement, g_nIncrementCounter );
				//print batch avg at start of batch.
				if(g_nUSCycleCounter < 10)
					printf(" BatchMean= %6.2f kgh= %6.2f", g_fBatchIncrementAvg, (g_fBatchIncrementAvg*40) );
			}
        }  */
//      g_fWeightInChamber = (g_fUSDistance - g_fUltrasonicTareDistance) * g_fUltrasonicmmpkg ;
        g_nUSTimeSumEntries = 0;
        g_lUSTimeSum = 0;
    }

    PutDataInUSBuffer( g_lTimer0CaptureRegister );
}



// Put data into UltraSonic Buffer at the buffer index position (g_sUSData.m_nIndexUSBuf)
void PutDataInUSBuffer(  DWORD lNewValue )
{
int nIndex, nMinus50Index;
bool bValidNewUSData = TRUE;
DWORD lMinus50Value;
//DWORD lNewValue = g_lTimer0CaptureRegister;		//i.e. new data value

   // Valid data add into US. buffer -
   if( bValidNewUSData )
   {
      //g_sUSData.m_bNoNewDataFlag = FALSE;  // flag to show new data.

      g_sUSData.m_nNoOfEntriesUSBuf++;     //

      // add to current sum.
      g_sUSData.m_lSumUSBuf += lNewValue;


      if( g_sUSData.m_nNoOfEntriesUSBuf > 50 )  // to do, calibration parameter.
      {
         g_sUSData.m_nNoOfEntriesUSBuf = 50;    // set no. of entries = Avg. Factor

         if( g_sUSData.m_nIndexUSBuf >= 50 )
            nMinus50Index = g_sUSData.m_nIndexUSBuf - 50;
         else
            nMinus50Index = MAX_ENTRIES_USBUF + g_sUSData.m_nIndexUSBuf - 50;

//         g_nMAF2Index = nMinusAF2Index;   //testonly

         //now subtract value from AF2 previously.
         lMinus50Value = g_sUSData.m_lUSBuf[nMinus50Index];

         if( g_sUSData.m_lSumUSBuf > lMinus50Value )
            g_sUSData.m_lSumUSBuf -= lMinus50Value;
         else
            g_sUSData.m_lSumUSBuf = 0;
      }

      g_sUSData.m_nEntriesInSumUSBuf = g_sUSData.m_nNoOfEntriesUSBuf;   //no. of entries in current sum

      //CALWMN - calculate the mean of the buffer.
      if( g_sUSData.m_nNoOfEntriesUSBuf > 0 )
         g_sUSData.m_lAverageUSBuf = g_sUSData.m_lSumUSBuf / g_sUSData.m_nNoOfEntriesUSBuf;

     // MVWLBF - store current  value in buffer - at current index.
      nIndex = g_sUSData.m_nIndexUSBuf;
      g_sUSData.m_lUSBuf[nIndex] = lNewValue;

      // INCWPR - increment the index or data pointer.
      g_sUSData.m_nIndexUSBuf++;
      if(g_sUSData.m_nIndexUSBuf >= MAX_ENTRIES_USBUF)
         g_sUSData.m_nIndexUSBuf = 0;                  //wrap around to start.

		g_sUSData.m_bNewDataAvailable = TRUE;

	}

}



void InitialiseUSBuffer(  void )
{
int i;

	for(i=0; i<MAX_ENTRIES_USBUF; i++)
		g_sUSData.m_lUSBuf[i] = 0;

	g_sUSData.m_lAverageUSBuf = 0;
	g_sUSData.m_lSumUSBuf = 0;
	g_sUSData.m_nIndexUSBuf = 0;
	g_sUSData.m_nNoOfEntriesUSBuf = 0;
	g_sUSData.m_nEntriesInSumUSBuf = 0;
	g_sUSData.m_bNewDataAvailable = FALSE;

}
// was called UltrasonicCalculation() and in file UltrasonicCalc.cpp.
void UltrasonicCycleReset( void )
{
    //printf("\n batch cycle time is %f",g_fBatchTimeInSeconds);
//    printf("\n level sensor tripped at %f mm",g_fUSDistance);
    g_nCycleSecondNo = 0;
    g_nUSBufferIndex = 0;
}



/// This was the test software.
//  Causes the US sensor to be read 20 times a second.  So could call on 20Hz PIt.
//  cts/mm CONSTANT IS ONLY APPROXIMATION.

/*
              iprintf( "Ultrasonic Sensor Test:\n" );
               SetupTimer0();    // initialise timer 0.


                     g_nUSTimeSumEntries = 0;
                     g_lUSTimeSum = 0;
                     g_lUSTimeAvg = 0;

               while( 1 )        //infinite loop.
               {


                  OSTimeDly(TICKS_PER_SECOND/20);
                  g_lUSTimeSum += g_lTimer0CaptureRegister;
                  g_nUSTimeSumEntries++;
                  if( g_nUSTimeSumEntries == 20)
                  {
                     g_lUSTimeAvg = g_lUSTimeSum/20;
                     g_fUSDistance = g_lUSTimeAvg / 440.5;    //approx only.
                     printf( "\nAvg Time = %ld Distance = %f mm", g_lUSTimeAvg, g_fUSDistance);
                     g_nUSTimeSumEntries = 0;
                     g_lUSTimeSum = 0;
                  }

*/

void CalculateCountsPermm(  void )
{
	g_fUltrasonicDivisor = float(g_lUltrasonicEchoTime[3]) / (float)g_arrnMBTable[WIDTH_COMMAND_CALIBRATE_ULTRASONIC_REF];
	if(fdTelnet > 0)
		printf("\n divisor is %f",g_fUltrasonicDivisor);
}


