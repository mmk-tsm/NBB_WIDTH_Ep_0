/*-------------------------------------------------------------------
 File: NBBGpio.cpp
 GPIO functions for NBB pcb.
 
 MMk - 
 12.12.2006 Edited to work for NBB Rev B1.
 P.Smith                                        19/01/07
 Split uart pin initialisation as InitialiseNBBUartPins so that it can
 be called seperately, the open serial function sets up the uart pins again
 Therefore anywhere the open serial is called, the InitialiseNBBUartPins must 
 also be called. 
 
 P.Smith                                       22/01/07
 Added U0_Select_Operator_Panel

 
 P.Smith                                       2/5/07
 Added InitialiseSPIChipSelects & SRCOnWithMapping & SRCOffWithMapping
 
 P.Smith                                       3/6/09
 removed line sim.gpio.podr_uartl = 0x00; in InitialiseNBBUartPins
 this was causing a problem when the panel was timing out and calling this function.
 by setting the outputs to 0, this is affecting the network comms, probably the 
 u1 tx enable

 -------------------------------------------------------------------*/
#include <startnet.h>

#include <basictypes.h>
#include <stdio.h> 

#include "gpio5270.h"
#include "NBBGpio.h"
#include <C:\Nburn\MOD5270\system\sim5270.h>
#include "Batvars2.h"
#include "QSPIconstants.h"


// *** GLOBAL VARIABLES ***
//PVBYTE  		CS3;
//Rev B1  - No Duart on Rev B1
/*
    PVBYTE  SERIAL_PORT_A;  
    PVBYTE  SERIAL_PORT_B;  
    PVBYTE  PARALLEL_PORT;  
*/  
//Rev B1  - No LCD i/f on Rev B1  
/*
    PVBYTE	LCD_DATA;
    PVBYTE	LCD_STATUS;
    PVBYTE	LCD_COMMAND;
    
    PVBYTE	UFPKeyData;
    
*/    
    
    
    
    PVBYTE	NBB_OPLATCH_1;
    PVBYTE	NBB_OPLATCH_2;
//Rev B1  - 2 latches on Rev B1      
/*    
    PVBYTE	NBB_OPLATCH_3;
    PVBYTE	NBB_OPLATCH_4;
    PVBYTE	NBB_OPLATCH_5;
*/  
//Rev B1  - Following buffers dont really exist on Rev B1  
/*
    PVBYTE	NBB_IPBUFFER_1;
    PVBYTE	NBB_IPBUFFER_2;
    
    PVBYTE	CF_BASE_ADDRESS;
    PVBYTE	KEY_REGISTER;
    
    PVBYTE  CS1REGISTER;
*/
    PVWORD  NBB_IPBUFFER;    // 3 stands for cs3\.
    
   static bool NB_TIMER_PINS_INIT;

unsigned char 	g_cBlenderNetworkAddress;
   BYTE  g_nNBBBoardRevision = NBB_REV_B2;      // default to Rev B2.
   WORD  g_nEEPROMSPIChipSelect; // onboard EEPROM.
   WORD  g_nADSPIChipSelect;
   WORD  g_nFlashSPIChipSelect;
   WORD  g_nCANSPIChipSelect;
   WORD  g_nEX1SPIChipSelect;
   WORD  g_nVNC1LSPIChipSelect;
   WORD  g_nAD2SPIChipSelect;              //2nd A/D.    

   BYTE PIN40SelectSDState;   

  
void InitialiseNBBGpio( void ) 
{
    // Code examples (Note: sim5270.h and gpio5270.h header files must be included for
    // the following examples to work. See “Pin Assignment and GPIO Control Registers”
    // above for more information):
    
    // Data Pin Assignment Register. Set all sixteen pins (D15-D0) as GPIO.
    sim.gpio.par_ad &= ~GPIO_PAR_DATA;      // "~" is inverse
    
    // set all output pins low initially.	(wont affect the inputs)
    sim.gpio.podr_datal = 0x00;
    sim.gpio.podr_datah = 0x00;
    
    // Data Port Data Direction Register. Set signals PDATAL4-7 and PDATAH8-15
    // as GPIO outputs.
    // sim.gpio.pddr_datal |= PDDR_PODR_DATAL7;  PD7 is an input. 	
//Rev B1.    
    sim.gpio.pddr_datal &= ~PDDR_PODR_DATAL7;    // PD7 = 1-wire i/p
    sim.gpio.pddr_datal |= PDDR_PODR_DATAL6;    //nb a "1" sets bit as OUTPUT.
    sim.gpio.pddr_datal |= PDDR_PODR_DATAL5;
    
//Rev B1 - PD4-PD0 unused - set up as o/p's
    sim.gpio.pddr_datal |= PDDR_PODR_DATAL4;
    sim.gpio.pddr_datal |= PDDR_PODR_DATAL3;
    sim.gpio.pddr_datal |= PDDR_PODR_DATAL2;
    sim.gpio.pddr_datal |= PDDR_PODR_DATAL1;
    sim.gpio.pddr_datal |= PDDR_PODR_DATAL0;
    
//Rev A3  also on Rev B1.
    sim.gpio.pddr_datah &= ~PDDR_PODR_DATAH15;		// On Rev A3 PD15 & PD14 are inputs. (0).
    sim.gpio.pddr_datah &= ~PDDR_PODR_DATAH14;
    
    sim.gpio.pddr_datah |= PDDR_PODR_DATAH13;
    sim.gpio.pddr_datah |= PDDR_PODR_DATAH12;
    sim.gpio.pddr_datah |= PDDR_PODR_DATAH11;
    sim.gpio.pddr_datah |= PDDR_PODR_DATAH10;
    sim.gpio.pddr_datah |= PDDR_PODR_DATAH9;
//Rev B1.    
    sim.gpio.pddr_datah &= ~PDDR_PODR_DATAH8;    // PD8 = FAULT1 i/p
 
    
//Rev B1    // Set PDATAH3-0  and PDATAL7 as inputs  
//Rev B1    sim.gpio.pddr_datah &= 0x70;  // not strictly needed,  cleared on reset.

//Rev B1
    // set output pins initially.	(wont affect the inputs)
    sim.gpio.podr_datal = 0x20;  // PD4-0 = L, PD5=H, PD6=L.
    sim.gpio.podr_datah = 0x36;  //Pd9 = H, PD10 = H, PD11 = L, PD13-12 = H.(FANI selected).

    InitialiseNBBUartPins();

//Timer pins 
   InitialiseNBTimerPins();
        
} // close function


void InitialiseNBBUartPins(void)
{
//Uart pins 

    // uART Pin Assignment Register. Set all sixteen pins (D15-D0) as GPIO.
    sim.gpio.par_uart &= 0x3F0C; // b15 = 0 => irq2 function
                                 // b14 = 0
                                // b7-4 = 0 => U1CTS and U1RTS are gpio.
                                // b1 = 0 => U0CTS is gpio
                                // b0 = 0 => U0RTS is gpio.
    sim.gpio.par_uart |= 0x3F0C; // b13 = 1 => U2 RXd function
                                // b12 = 1 => U2 TXd function
                                // b11-8 = 0 => U2 RXd function & U2 TXd function
                                // b3 = 1 => U0 RXd function
                                // b2 = 1 => U0 TXd function
                                
        // set all uart gpio output pins low initially.	(wont affect the inputs)
                                                             
    // Uart Port Data Direction Register. Set signals U0RTS, U1RTS, U0cTS as o/ps
    // nb a "1" sets bit as OUTPUT.
//Rev B1. 
/*   
     sim.gpio.pddr_uartl |= 0x6E;    //b6-U1RTS, b5-U1TX, b3-U0CTS, b2-U0RTS, b1-U0Tx all o/p's
     sim.gpio.pddr_uartl &= 0x6E;    //b7-U1CTS, b4-U1Rx, b0-U0Rx all 1/p's
 */
 //Rev B1 as A3 except that U1CTs is also an o/p.
     sim.gpio.pddr_uartl |= 0xEE;    //b7-U1CTS, b6-U1RTS, b5-U1TX, b3-U0CTS, b2-U0RTS, b1-U0Tx all o/p's
     sim.gpio.pddr_uartl &= 0xEE;    //b4-U1Rx, b0-U0Rx all 1/p's
     
     sim.gpio.pddr_uarth |= 0x02;    //b1-U2Tx = o/p
     sim.gpio.pddr_uarth &= 0xFE;    //b0-U2Rx = 1/p

}




/*-------------------------------------------------------------------
 Configure Timer pins to be  GPIO
 -------------------------------------------------------------------*/
void InitialiseNBTimerPins(void)
{

 // NBB Rev A3 case
//   J2[30].function( PIN_GPIO );     //cs\ = SPICS0
//   J2[8].function( PIN_GPIO );     // cd = pd15
//   J2[6].function( PIN_GPIO );     // wp = pd14
//Timer pins 
// Rev B1 - no changes made, same as A3.
    // Timer Pin Assignment Register. Set DT3,DT1,DT0IN/OUT as GPIO. (DT2 not available on MOD5270)
    sim.gpio.par_timer &= 0x3030; // b15/b14 = 00 => DT3IN is gpio
                                // b11-b8 = 00 00 => DT1IN,DT0IN is gpio
                                // b7/b6 = 00 => DT3OUT is gpio
                                // b3-b0 = 00 00 => DT1OUT,DT0OUT is gpio
                                
        // set all uart gpio output pins low initially.	(wont affect the inputs)
    sim.gpio.podr_timer = 0x00;
    
    // Timer Port Data Direction Register. Set signals DT3OUT, DT1OUT, DT0OUT, as o/ps
    // 
    // nb a "1" sets bit as OUTPUT. 
    // Set pins DT3OUT, DT1OuT, DT0OUT, as o/ps
     sim.gpio.pddr_timer |= 0x45;    //b6 = 1 (DT3out), b2=1 (DT1OUT), b0=1 (DT0OUT) outputs
    // Set pins DT3IN, DT1IN, DT0IN, as i/ps
     sim.gpio.pddr_timer &= ~0x2A;    //(0010 1010) b5 = 0 (DT2IN), b3=0 (DT1IN), b1=0 (DT0IN) inputs

   
   NB_TIMER_PINS_INIT = TRUE;
}
//Rev B1.
void U0_Select_Mtty (void)   // Select port U0 (RS232) to go to Mtty (printf etc.)
{
   if( !NB_TIMER_PINS_INIT )
      InitialiseNBTimerPins();      // Timer pins must have been initialise.
// DT0OUT = 1, DT3OUT = 1
	SetDT0OUT_Hi();
	SetDT3OUT_Hi();	
}
void U0_Select_Operator_Panel (void)   // 
{
   if( !NB_TIMER_PINS_INIT )
      InitialiseNBTimerPins();      // Timer pins must have been initialise.
// DT0OUT = 1, DT3OUT = 1
	SetDT0OUT_Lo();
	SetDT3OUT_Lo();	
}

/*
BYTE ReadHexSwitch( void ) 
{
	BYTE cValue = sim.gpio.ppdsdr_datal;  	// d7-d0
	
	cValue = ~cValue;                      // invert. 
	return (cValue & 0x0F);	   				// return 4 l.s. bits.
}
*/
// ReadHexSwitch - Rev B1 pcb.
BYTE ReadHexSwitch( void ) 
{
   BYTE cTemp = 0x00;
	WORD nValue = *( NBB_IPBUFFER );  	   // 16 bit value. 	
	nValue = ~nValue;                      // invert.
   nValue >>= 8;                          // (Sw value in bits 11-8).
	cTemp = (BYTE)(nValue & 0x000F);	   	// return 4 l.s. bits.
	return( cTemp );
}

//Rev B1 - no changes from Rev A3.
BYTE ReadSDInputs( void ) 
{
	BYTE cValue = sim.gpio.ppdsdr_datah;  	// d15-d8
	
//	cValue = ~cValue;                      // invert.
	return (cValue & 0xC0);	   				// return 2 m.s. bits.
}

/******************************************************************
 * FUNCTION: void Init_EdgePort() 
 * DESCRIPTION: 
 * This function initialises the edge port pins (IRQ7\, IRQ5\, IRQ3\ IRQ1\)
 *
 ******************************************************************/
void Init_EdgePort(void)
{

//  Setup External IRQ7 - IRQ1
    sim.eport.eppar = 0x8008; // 10 00 00 00 00 00 00 00 see table 15-3 in UM 
//todo      sim.eport.eppar |= 0x8000;
//todo      sim.eport.eppar &= 0xB330;  // use or'ing and and'ing to not interfere with Irq6, 4, 2.
                                 //  IRQ7 is falling edge (10), rest are level sensitive.
      sim.eport.epddr=0x00; /* All edge port pins as inputs */
      
      sim.eport.epier = 0x0082; // Enable IRQ7& IRQ1 only 1 0 0 0 0 0 1 0   (TO DO, TESTONLY - rtc)
      
//      sim.eport.epfr=0xAA; /* Clear the interrupt edge 1 0 1 0 1 0 1 0  FOR PINS AVAILABLE*/

}
//Initialise chip selects for NBB Rev B1 pcb.
void Init_RevB1ChipSelects(void)
{
   sim.gpio.par_busctl |= 0x1000;		// Make sure that TA\ is configured for TA\ (not gpio).
   
   Init_RevB1ChipSelect1();   // initialise chip select #1
   Init_RevB1ChipSelect2();   // initialise chip select #2
   Init_RevB1ChipSelect3();   // initialise chip select #3
       
   NBB_OPLATCH_1 = ( PVBYTE ) NBB_CS1_BASE + 0x0000;     // at CS1 base address
   NBB_OPLATCH_2 = ( PVBYTE ) NBB_CS2_BASE + 0x0000;     // at CS2 base address
   
   NBB_IPBUFFER =  ( PVWORD ) NBB_CS3_BASE + 0x0000;    // IPBUFFER at CS3 base address  
}

//Initialise chip select #1 for NBB Rev B1 pcb.
void Init_RevB1ChipSelect1( void )
{
   sim.cs[1].csar = ( NBB_CS1_BASE >> 16 );
// enable AA.
// For MOD5270 1ws = 1/fsys/2, 13.6nS approx at 147MHz.  Secondary write wait states dont seem to work.
  sim.cs[1].cscr = 0x3d40;        // IWS int. wait states = F; AA auto ack = 1, disabled; PS = 8bits.
 // sim.cs[1].cscr = 0x3d47;        // IWS int. wait states = F; AA auto ack = 1, disabled; PS = 8bits, SWWS = 6.
  sim.cs[1].csmr = 0x00010001;    //BAM = 0001 (cs block =128Kbytes); WP=0(R/W)  V - valid bit set.
}

//Initialise chip select #2 for NBB Rev B1 pcb.
void Init_RevB1ChipSelect2( void )
{
   sim.cs[2].csar = ( NBB_CS2_BASE >> 16 );
// enable AA.
// For MOD5270 1ws = 1/fsys/2, 13.6nS approx at 147MHz.  Secondary write wait states dont seem to work.
  sim.cs[2].cscr = 0x3d40;        // IWS int. wait states = F; AA auto ack = 1, disabled; PS = 8bits.
  
  sim.cs[2].csmr = 0x00010001;    //BAM = 0001 (cs block =128Kbytes); WP=0(R/W) ,V - valid bit set.
}

//Initialise chip select #3 for NBB Rev B1 pcb. - NB - CS3 is for read only.
void Init_RevB1ChipSelect3( void )
{
   sim.cs[3].csar = ( NBB_CS3_BASE >> 16 );
// enable AA.
// For MOD5270 1ws = 1/fsys/2, 13.6nS approx at 147MHz.  Secondary write wait states dont seem to work.
  sim.cs[3].cscr = 0x3d80;        // IWS int. wait states = F; AA auto ack = 1, disabled; PS = 16bits.
  
  sim.cs[3].csmr = 0x00010101;    //BAM = 0001, (cs block =128Kbytes), WP=1(READ ONLY),  V - valid bit set.
}


void Init_ChipSelect3(void)
{
   sim.gpio.par_busctl |= 0x1000;		// Make sure that TA\ is configured for TA\ (not gpio).

   sim.cs[3].csar = ( NBB_CS3_BASE >> 16 );
   sim.cs[3].cscr = 0x3c40;        // IWS int. wait states = F; AA auto ack = 0, disabled; PS = 8.
//todo - test purposes, enable AA.
//   sim.cs[2].cscr = 0x3140;        // IWS int. wait states = F; AA auto ack = 1, disabled; PS = 8.
   sim.cs[3].csmr = 0x00010001;    //BAM = 0001 (cs block =128Kbytes); V - valid bit set.
//   CS2 = ( PVBYTE ) NBB_CS2_BASE;

   sim.cs[2].csar = ( NBB_CS2_BASE >> 16 );
 //  sim.cs[2].cscr = 0x3c40;        // IWS int. wait states = F; AA auto ack = 0, disabled; PS = 8.
//todo - test purposes, enable AA.
  sim.cs[2].cscr = 0x3d40;        // IWS int. wait states = F; AA auto ack = 1, disabled; PS = 8bits.
  sim.cs[2].csmr = 0x00010001;    //BAM = 0001 (cs block =128Kbytes); V - valid bit set.

   sim.cs[1].csar = ( NBB_CS1_BASE >> 16 );
 //  sim.cs[2].cscr = 0x3c40;        // IWS int. wait states = F; AA auto ack = 0, disabled; PS = 8.
//todo - test purposes, enable AA.
// For MOD5270 1ws = 1/fsys/2, 13.6nS approx at 147MHz.  Secondary write wait states dont seem to work.
 // sim.cs[1].cscr = 0x0d40;        // IWS int. wait states = 3; AA auto ack = 1, disabled; PS = 8bits.
  sim.cs[1].cscr = 0x3d40;        // IWS int. wait states = F; AA auto ack = 1, disabled; PS = 8bits.
 // sim.cs[1].cscr = 0x3d47;        // IWS int. wait states = F; AA auto ack = 1, disabled; PS = 8bits, SWWS = 6.
  sim.cs[1].csmr = 0x00010001;    //BAM = 0001 (cs block =128Kbytes); V - valid bit set.

//Rev B1        
   NBB_OPLATCH_1 = ( PVBYTE ) NBB_CS1_BASE + 0x0000;     // at CS1 base address
   NBB_OPLATCH_2 = ( PVBYTE ) NBB_CS3_BASE + 0xF000;     // at CS2 base address
   
//Rev B1   NBB_OPLATCH_3 = ( PVBYTE ) NBB_CS3_BASE + 0xE800;
//Rev B1   NBB_OPLATCH_4 = ( PVBYTE ) NBB_CS3_BASE + 0xE000;
//Rev B1   NBB_OPLATCH_5 = ( PVBYTE ) NBB_CS3_BASE + 0xD800;
    
//Rev B1   NBB_IPBUFFER_1 = ( PVBYTE ) NBB_CS3_BASE + 0xF800;    // READ ONLY 
//Rev B1   NBB_IPBUFFER_2 = ( PVBYTE ) NBB_CS3_BASE + 0xF000;

//Rev B1
   NBB_IPBUFFER = ( PVWORD ) NBB_CS3_BASE + 0x0000;    // IPBUFFER_3 at CS3 base address

//Rev B1
/*
   LCD_DATA = ( PVBYTE ) NBB_CS3_BASE + 0xD000;
   LCD_STATUS = ( PVBYTE ) NBB_CS3_BASE + 0xD001;
   LCD_COMMAND = ( PVBYTE ) NBB_CS3_BASE + 0xD001;   
   
   SERIAL_PORT_B = ( PVBYTE ) NBB_CS3_BASE + 0xC800;
   SERIAL_PORT_A = ( PVBYTE ) NBB_CS3_BASE + 0xC000; //
   PARALLEL_PORT = ( PVBYTE ) NBB_CS3_BASE + 0xB800;
   
   CF_BASE_ADDRESS = ( PVBYTE ) NBB_CS3_BASE + 0xB000;

   KEY_REGISTER = ( PVBYTE ) NBB_CS3_BASE + 0xA000;      // Key reg is in 2nd PAL.

   CS1REGISTER = ( PVBYTE ) NBB_CS1_BASE + 0x0000;      // CS1 TEST reg 
*/
   
}

//////////////////////////////////////////////////////
// SRCOff(i)
//
// Turns off the Source output for the specified component   - Component no is 0-MAX_SOURCE_OUTPUTS.
// Note, the component no. passed to the routine is 1 based, first component = 1.
//
// M.McKiernan							30-11-2005
// First pass.
//////////////////////////////////////////////////////
void SRCOff (int nSrcNo)
{	
	if(nSrcNo <= MAX_SOURCE_OUTPUTS)				// Dont do anything if not a valid component.
	{
		switch( nSrcNo )
		{
		case 1:
			SRC1Off();			// 
			break;
		case 2:
			SRC2Off();			// 
			break;
		case 3:
			SRC3Off();			// 
			break;
		case 4:
			SRC4Off();			// 
			break;

		case 5:
			SRC5Off();			// 
			break;
		case 6:
			SRC6Off();			// 
			break;
		case 7:
			SRC7Off();			// 
			break;
		case 8:
			SRC8Off();			// 
			break;

		case 9:
			SRC9Off();			// 
			break;
		case 10:
			SRC10Off();			// 
			break;
		case 11:
			SRC11Off();			// 
			break;
		case 12:
			SRC12Off();			// 
			break;
			
		case 13:
			SRC13Off();			// 
			break;
		case 14:
			SRC14Off();			// 
			break;
		case 15:
			SRC15Off();			// 
			break;
		case 16:
			SRC16Off();			// 
			break;
			
// Rev B1 only 16 source outputs			
/*   			
		case 17:
			SRC17Off();			// 
			break;
		case 18:
			SRC18Off();			// 
			break;
		case 19:
			SRC19Off();			// 
			break;
		case 20:
			SRC20Off();			// 
			break;

		case 21:
			SRC21Off();			// 
			break;
		case 22:
			SRC22Off();			// 
			break;
		case 23:
			SRC23Off();			// 
			break;
		case 24:
			SRC24Off();			// 
			break;

		case 25:
			SRC25Off();			// 
			break;
		case 26:
			SRC26Off();			// 
			break;
		case 27:
			SRC27Off();			// 
			break;
		case 28:
			SRC28Off();			// 
			break;
			
		case 29:
			SRC29Off();			// 
			break;
		case 30:
			SRC30Off();			// 
			break;
		case 31:
			SRC31Off();			// 
			break;
		case 32:
			SRC32Off();			// 
			break;
			
		case 33:
			SRC33Off();			// 
			break;
		case 34:
			SRC34Off();			// 
			break;
		case 35:
			SRC35Off();			// 
			break;
		case 36:
			SRC36Off();			// 
			break;

		case 37:
			SRC37Off();			// 
			break;
		case 38:
			SRC38Off();			// 
			break;
		case 39:
			SRC39Off();			// 
			break;
		case 40:
			SRC40Off();			// 
			break;
*/			
			// todo
            // add remaining source outputs here.			
		default:
			break;

		}  // end of switch statement.

	}
}

//////////////////////////////////////////////////////
// SRCOn(i)
//
// Turns on the Source output for the specified component   - Component no is 0-MAX_SOURCE_OUTPUTS.
// Note, the component no. passed to the routine is 1 based, first component = 1.
//
// M.McKiernan							30-11-2005
// First pass.
//////////////////////////////////////////////////////
void SRCOn (int nSrcNo)
{	
	if(nSrcNo <= MAX_SOURCE_OUTPUTS)				// Dont do anything if not a valid component.
	{
		switch( nSrcNo )
		{
		case 1:
			SRC1On();			// 
			break;
		case 2:
			SRC2On();			// 
			break;
		case 3:
			SRC3On();			// 
			break;
		case 4:
			SRC4On();			// 
			break;

		case 5:
			SRC5On();			// 
			break;
		case 6:
			SRC6On();			// 
			break;
		case 7:
			SRC7On();			// 
			break;
		case 8:
			SRC8On();			// 
			break;

		case 9:
			SRC9On();			// 
			break;
		case 10:
			SRC10On();			// 
			break;
		case 11:
			SRC11On();			// 
			break;
		case 12:
			SRC12On();			// 
			break;
			
		case 13:
			SRC13On();			// 
			break;
		case 14:
			SRC14On();			// 
			break;
		case 15:
			SRC15On();			// 
			break;
		case 16:
			SRC16On();			// 
			break;
// Rev B1 only 16 source outputs			
/*			
		case 17:
			SRC17On();			// 
			break;
		case 18:
			SRC18On();			// 
			break;
		case 19:
			SRC19On();			// 
			break;
		case 20:
			SRC20On();			// 
			break;

		case 21:
			SRC21On();			// 
			break;
		case 22:
			SRC22On();			// 
			break;
		case 23:
			SRC23On();			// 
			break;
		case 24:
			SRC24On();			// 
			break;

		case 25:
			SRC25On();			// 
			break;
		case 26:
			SRC26On();			// 
			break;
		case 27:
			SRC27On();			// 
			break;
		case 28:
			SRC28On();			// 
			break;
			
		case 29:
			SRC29On();			// 
			break;
		case 30:
			SRC30On();			// 
			break;
		case 31:
			SRC31On();			// 
			break;
		case 32:
			SRC32On();			// 
			break;
			
		case 33:
			SRC33On();			// 
			break;
		case 34:
			SRC34On();			// 
			break;
		case 35:
			SRC35On();			// 
			break;
		case 36:
			SRC36On();			// 
			break;

		case 37:
			SRC37On();			// 
			break;
		case 38:
			SRC38On();			// 
			break;
		case 39:
			SRC39On();			// 
			break;
		case 40:
			SRC40On();			// 
			break;
*/			
			// todo
            // add remaining source outputs here.			
		default:
			break;

		}  // end of switch statement.

	}
}


void SRCOnWithMapping (int nSrcNo)
{
    int nTemp,j;
    for(j=0; j<DEFINEDOUTPUTNO; j++)
    {
        nTemp = g_BlenderMapping[nSrcNo-1][j];
        if(nTemp != 0)
        {
//                miprintf("\n nsrc is %d",nTemp);
            SRCOn(nTemp);	
//            SRCOn(nSrcNo);	
        }
    }
}


void SRCOffWithMapping (int nSrcNo)
{
    int nTemp,j;
    for(j=0; j<DEFINEDOUTPUTNO; j++)
    {
        nTemp = g_BlenderMapping[nSrcNo-1][j];
        if(nTemp != 0)
        {
            SRCOff(nTemp);	
//            SRCOff(nSrcNo);	
        }
    }
}



// TESTONLY
BOOL g_bLevelSensorState;
BOOL g_bNBB_L1;
BOOL g_bNBB_L2;
BOOL g_bNBB_L3;
BOOL g_bFAULT2;
//Rev B1
void ReadMachineInputs( void )
{
WORD nTemp;
                  nTemp = 	*(NBB_IPBUFFER);
                  
                  iprintf("\n IPBUFFER = %04x", nTemp );
                  
                  if( nTemp & 0x0001)           // read by D16
                     iprintf( " STOPPED " );  
                  else
                     iprintf(" RUN ");
                     
                  if( nTemp & 0x0002)
                     iprintf( " MOTOR TRIPPED " );  
                  else
                     iprintf(" CONTACTOR ON ");
                     
                  if( nTemp & 0x0008)
                  {
                     iprintf( " LEVEL SENSOR UnCOVERED " ); 
                     g_bLevelSensorState = LUNCOVERED; 
                  }
                  else
                  {
                     iprintf(" LEVEL SENSOR COVERED ");
                      g_bLevelSensorState = LCOVERED; 
                 }
//link L1 - D28.                 
                 if( nTemp & 0x1000 )
                 {
                     g_bNBB_L1 = NOT_FITTED;
                     iprintf("\n L1 not fitted" );
                 }
                 else
                 {
                     g_bNBB_L1 = FITTED;
                     iprintf("\n L1 fitted" );
                 }
//link L2 - D29.                 
                 if( nTemp & 0x2000 )
                 {
                     g_bNBB_L2 = NOT_FITTED;
                     iprintf(" L2 not fitted" );
                 }
                 else
                 {
                     g_bNBB_L2 = FITTED;
                     iprintf(" L2 fitted" );
                 }
//link L3 - D30.                 
                 if( nTemp & 0x4000 )
                 {
                     g_bNBB_L3 = NOT_FITTED;
                     iprintf(" L3 not fitted" );
                 }
                 else
                 {
                     g_bNBB_L3 = FITTED;
                     iprintf(" L3 fitted" );
                 }
//Fault2 - D31.                 
                 if( nTemp & 0x8000 )
                 {
                     g_bFAULT2 = FAULTY;           // HIGH MEANS FAULT.
                     iprintf(" FAULT2 = FAULTY" );
                 }
                 else
                 {
                     g_bFAULT2 = OK;
                     iprintf(" FAULT2 = OK" );
                 }

}                                                    
void InitialiseSPIChipSelects( void )
{
   if(g_nNBBBoardRevision == NBB_REV_B1)
   {
      g_nEEPROMSPIChipSelect = EEPROM_SPI_CHIP_SELECT_REVB1; // onboard EEPROM.
      g_nADSPIChipSelect = LTC2415_SPI_CHIP_SELECT_REVB1;      //onboard A/D.
      g_nFlashSPIChipSelect = FLASH_SPI_CHIP_SELECT;           // SPI flash
      g_nCANSPIChipSelect = CAN_SPI_CHIP_SELECT;
      g_nEX1SPIChipSelect = SPI_EX1_CHIP_SELECT;
      g_nVNC1LSPIChipSelect = VNC1L_SPI_CHIP_SELECT;
      g_nAD2SPIChipSelect = SPIEXCS2_CHIP_SELECT;              //2nd A/D. 
      
      PIN40SelectSDState = 1;    //pin 40 = SPICS1, must be high for sD on Rev B1.   
   }
   else     // assume that it is NBB Rev B2
   {
      g_nEEPROMSPIChipSelect = EEPROM_SPI_CHIP_SELECT_REVB2; // onboard EEPROM.
      g_nADSPIChipSelect = LTC2415_SPI_CHIP_SELECT_REVB2;      //onboard A/D.
      g_nFlashSPIChipSelect = FLASH_SPI_CHIP_SELECT;           // SPI flash
      g_nCANSPIChipSelect = CAN_SPI_CHIP_SELECT_REVB2;
      g_nEX1SPIChipSelect = SPI_EX1_CHIP_SELECT_REVB2;
      g_nVNC1LSPIChipSelect = VNC1L_SPI_CHIP_SELECT_REVB2;
      g_nAD2SPIChipSelect = SPI_AD2_CHIP_SELECT_REVB2;              //2nd A/D.
      
      PIN40SelectSDState = 0;  //pin 40 = SPICS1, must be low for sD on Rev B2.   
   }


}


/*
void ReadMachineInputs( void )
{
BYTE cTemp;
                  cTemp = 	*(NBB_IPBUFFER_1);
                  if( cTemp & 0x01)
                     iprintf( " STOPPED " );  
                  else
                     iprintf(" RUN ");
                     
                  if( cTemp & 0x02)
                     iprintf( " MOTOR TRIPPED " );  
                  else
                     iprintf(" CONTACTOR ON ");
                     
                  if( cTemp & 0x08)
                  {
                     iprintf( " LEVEL SENSOR UnCOVERED " ); 
                     g_bLevelSensorState = LUNCOVERED; 
                  }
                  else
                  {
                     iprintf(" LEVEL SENSOR COVERED ");
                      g_bLevelSensorState = LCOVERED; 
                 }

}                                                    
*/

/*  FROM YAHOO USER GROUP

#define BASEADDRESS (0xA0000000) // taken from oryginal Netburner
// carrier board
static PBYTE pU2; // read inputs from U2 transceiver
static PBYTE pU3; // read inputs from U3 transceiver

void InitChipBoardSelect() // initialize Bus and PC port
{
sim.cs[1].csor = 0xFFFFFE40;
sim.cs[1].csbr = BASEADDRESS | 0x201; //0010 0000 0001 
pU2 = ( PWORD ) BASEADDRESS; // points to U2
pU3 = ( PWORD ) BASEADDRESS + 2; // points to U3 - is this
OK ?
}
// later in program I perform below instructions to access U2 & U3
//...
unsigned char pinsU2;
unsigned char pinsU3;

pinsU2 = *pU2; // this works !
pinsU3 = *pU3; // this DO NOT WORK !

*/


