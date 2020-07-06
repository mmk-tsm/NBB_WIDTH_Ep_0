    extern  int g_ntestonly;

//////////////////////////////////////////////////////
// Init5270
//
// Initialisation of various hardware functions.
// P.Smith                              9/02/06
// read ltc2415 a/d implemented.
// #include "NBBVars.h"
//
// P.Smith                              14/02/06
// implement IRQ3 interrupt in SetNBBInterrupts
// put function into this module.
// 
//  P.Smith								29/6/06
// added SetupTimer2( void );
//
// P.Smith                          16/1/07
// Call SPI_Select_AD to select a/d
// LTC2415_SPI_CHIP_SELECT_REVB1 used
// call AnalogMux_Select_FANI on a/d read.
//
// P.Smith                          5/2/07
// Remove set of irq interrupts no longer used on rev b1
// remove unused iprintf
//
// P.Smith                          14/2/07
// added #include "IOMeasure.h"
// added my_pitr_measure to measure the on time repeatability of the plc blender
// added SetUpPITRT to set up the interrupt for the pulse measurement.
//
// P.Smith                      3/5/07
// allow for reading chip select from ram, this is to cater for rev b1,b2 boards
//
// P.Smith                      23/7/08
// remove set of g_lRawADCountsLTC2415 on eoc bit still set
// name change ReadLTC2415 to ReadAToD_1, change function to pass back
// a status word for the a/d. This will indicate if the a/d read has
// any errors
//
// P.Smith                      19/11/09
// if g_bRefAToDStabilityTestInProgress set, do not switch in load cell to mux
// as the ref a/d test is running.
//////////////////////////////////////////////////////

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
#include "Batvars2.h"


extern  volatile WORD chipSelectUsed; 
extern   WORD * ptrRxBuffer; 
extern long g_lRawADCountsLTC2415;  
extern unsigned int g_nLTC2415Err;  
extern WORD RxBuffer[];
extern long lTemp; 
 
 

extern "C" { 
/* This function sets up  the 5282 interup controller */
void SetIntc(long func, int vector, int level, int prio );  //from yahoo
}



/*-------------------------------------------------------------------
PIT Interrupt Service Routine
-------------------------------------------------------------------*/

INTERRUPT(my_pitr_func,0x2600)
{
 
   //static WORD led_count;
   WORD tmp = sim.pit[2].pcsr; // use PIT 2 
   // Clear PIT 2
   tmp &= 0xFF0F;
   tmp |= 0x0F;
   sim.pit[2].pcsr = tmp;
   PITHandler();
}

INTERRUPT(my_pitr_measure,0x2600)
{
 
   //static WORD led_count;
   WORD tmp = sim.pit[2].pcsr; // use PIT 2 
   // Clear PIT 2
   tmp &= 0xFF0F;
   tmp |= 0x0F;
   sim.pit[2].pcsr = tmp;
   PITMeasureHandler();
}



/*-------------------------------------------------------------------
PIT Setup function. See chapter 21 of the 5270 users manual
for details  
-------------------------------------------------------------------*/
void SetUpPITR(int pitr_ch, WORD clock_interval, BYTE pcsr_pre /* See table
13-13 in the users manual */)
{
WORD tmp;
if ((pitr_ch<1) || (pitr_ch > 3)) return;
/* populate the interrupt vector in the interrupt controller */
SetIntc((long)&my_pitr_func, 36+pitr_ch, 2/* IRQ 2 */ ,3);
sim.pit[pitr_ch].pmr=clock_interval;
tmp = pcsr_pre;
tmp = (tmp << 8) | 0x0F;
sim.pit[pitr_ch].pcsr = tmp;
}


void SetUpPITRT(int pitr_ch, WORD clock_interval, BYTE pcsr_pre /* See table
13-13 in the users manual */)
{
WORD tmp;
if ((pitr_ch<1) || (pitr_ch > 3)) return;
/* populate the interrupt vector in the interrupt controller */
SetIntc((long)&my_pitr_measure, 36+pitr_ch, 2/* IRQ 2 */ ,3);
sim.pit[pitr_ch].pmr=clock_interval;
tmp = pcsr_pre;
tmp = (tmp << 8) | 0x0F;
sim.pit[pitr_ch].pcsr = tmp;
}





/*-------------------------------------------------------------------
 IRQ3 interrupt service routine - 
 Declare our interrupt procedure.... 
name: our_irq1_pin_isr
masking level (The value of the ColdFire SR during the interrupt:

use 0x2700 to mask all interrupts.
	0x2500 to mask levels 1-5 etc...
	0x2100 to mask level 1 

 -------------------------------------------------------------------*/
INTERRUPT(IRQ3_pin_isr, 0x2300 )
{
  /* WARNING WARNING WARNING 
  Only a very limited set of RTOS functions can be called from 
  within an interrupt service routine.

  Basically, only OS POST functions and LED functions should be used
  No I/O (read, write or printf may be called), since they can block. */
	
  sim.eport.epfr=0x08; /* Clear the interrupt edge 0 0 0 0 1 0 0 0 */
}


INTERRUPT(IRQ5_pin_isr, 0x2500 )
{
  /* WARNING WARNING WARNING 
  Only a very limited set of RTOS functions can be called from 
  within an interrupt service routine.

  Basically, only OS POST functions and LED functions should be used
  No I/O (read, write or printf may be called), since they can block. */
                     
  sim.eport.epfr=0x20; /* Clear the interrupt edge 0 0 1 0 0 0 0 0 */
//  g_lRTCCount++;
//  OSSemPost(&IrqPostSem);
}

WORD ReadAToD_1 (void)
{ 
    WORD  nATDLTC2415Status;
    WORD wChipSelectUsed;  
    WORD   * ptrRxBuffer = arrwRxBuffer; 
    if(!g_bRefAToDStabilityTestInProgress)  // do not switch in load cell
    {
        AnalogMux_Select_FANI();  // select a/d through multiplexer.
    }
    wChipSelectUsed = g_nADSPIChipSelect; // for NBB  Rev B1.              
    SPI_Select_AD(); 
    ReadLTC2415(wChipSelectUsed, ptrRxBuffer);
    
    g_nLTC2415Err = 0;           // clear errors initially.
    
    nATDLTC2415Status = arrwRxBuffer[0];
    if( arrwRxBuffer[0] & LTC2415_EOC_BIT )          // EOC is high during conversion.
    {
        g_nLTC2415Err |= 0x0004;			   // Set b2 for EOC not set
//        g_lRawADCountsLTC2415 = 0;
//        iprintf("\n eoc error");   
    }

    else if( !(arrwRxBuffer[0] & LTC2415_SIG_BIT) )   // Sign is 1 if result positive.
    {
        g_nLTC2415Err |= 0x0002;			   // Set b1 for negative polarity.
//        g_lRawADCountsLTC2415 = 0;
    }
    else    // result is positive and conversion complete.
    {
        lTemp = arrwRxBuffer[1] | (arrwRxBuffer[0] << 16);          //amalgamate words
        lTemp >>= 10;                     // shift 10 bits right, i.e. keep 19.
        g_lRawADCountsLTC2415 = lTemp & 0x0007FFFF;             // discard top 3 and lowest 9 bits.                            
        if( g_lRawADCountsLTC2415 > LTC2415_MAX_VALUE )
        {
            g_lRawADCountsLTC2415 = LTC2415_MAX_VALUE;
            g_nLTC2415Err |= 0x0001;			   // Set b0 for over range. 
        }
        else
        g_nLTC2415Err = 0;                      // good data, clear error flag. 

    }
     
     return(nATDLTC2415Status);  // return a/d status
}


//////////////////////////////////////////////////////
// SetupTimer2()               
//
// sets dma timer 2 in free running mode.
// prescaler = 256
// clock divided by 16
// 
//
// P.Smith                              14-2-2005
//////////////////////////////////////////////////////


       
void SetupTimer2( void )

{

   BYTE prescale = 0xFF;      // gives maximum prescale possible.

  // Reset timer2.  All bits set to reset values.

  sim.timer[2].tmr |= 0x0001;   // enable timer
  sim.timer[2].tmr = 0;           // reset timer

//Setup Timer

  sim.timer[2].tmr |= 0x0004;   // Divide system clock by 16
  sim.timer[2].tmr &= ~0x0018;  // Set Ref Int disable and FreeRun = 0.
  sim.timer[2].tmr |= (prescale << 8); // Set prescale
  sim.timer[2].trr = 0x0000FFFF;  //ref_count;  // Set ref count...note Dword.
  sim.timer[2].tmr |= 0x0001;           // Start timer
}
           
               

              






