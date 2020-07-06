/*-------------------------------------------------------------------
 File: NBBGpio.h
 Description: GPIO for NBB pcb
 MMk -
 12.12.2006 Edited to work for NBB Rev B1.
 P.Smith                                    16/01/07
 Merge in latest hardware set up from mmk

 P.Smith                                    5/2/07
 Set_EEPROM_Write_Protect & Remove_EEPROM_Write_Protect

 P.Smith                                    20/2/07
 SPI_Select_USB no program assigned.

 P.Smith                                    3/4/07
 InitialiseNBTimerPins added to reinitialse pins used by sd card software.

 P.Smith                                   2/5/07
 InitialiseSPIChipSelects, SRCOnWithMapping, SRCOffWithMapping added
 externs for chip selects added
 definitions for NBB_REV_B1, NBB_REV_B2 added

 P.Smith                                   9/5/07
 InitialisePinsUsedBySDCard added to set spi pins to i/o

 P.Smith                                   23/5/07
 added SPI_RS422_ENABLE_HIGH, SPI_RS422_ENABLE_LOW

 P.Smith                                  25/6/07
 todo in SPI_Select_USB removed.

 P.Smith                                  11/1/08
 added U0_MODE_MTTTY, U0_MODE_USB U0_MODE_PANEL ,U0_MODE_CFG
 name change to U0_Select_USB add vinc functions, USB_5V_ENABLE etc
 definition of GetVNC1LModem, SetVNC1LDataMode, SetVNC1LCommandMode

 P.Smith                                  22/1/08
 added SPI_Select_USB code to switch the correct outputs to select the usb

 M.McKiernan                              07/4/2008
 Added sim.gpio.par_qspi = 0xFF; to SPI_Select_EEPROM (void)

   P.Smith                      21/7/08
   added SDCARD_INSERTED_BITPOS & SDCARD_LOCK_BITPOS definitions

   P.Smith                     26/11/08
   added ANALOG_SWITCH_ON /OFF for switching in test voltage on bridge

   P.Smith                     30/11/08
   added ReInitialiseGPIOPinsUsedBySDCard this only reinitialises the timer pins

	 M.McKiernan                              01/12/2009
   In SetU1CTS_Lo( void ) cast operand to byte e.g. (BYTE)(~0x80)

	P.Smith 						7/7/10
	added SetPD0_Lo & SetPD0_Hi
	added  US_TRIGGER_ON,US_TRIGGER_OFF

 -------------------------------------------------------------------*/

#ifndef __NBBGPIO_H__
#define __NBBGPIO_H__

#include <C:\Nburn\MOD5270\system\sim5270.h>
#include <ctype.h>
#include <basictypes.h>
#include "gpio5270.h"

extern unsigned char 	g_cBlenderNetworkAddress;

// Chip Select Define Address
#define NBB_CS3_BASE (0xB0000000)
#define NBB_CS2_BASE (0xB2000000)
#define NBB_CS1_BASE (0xB4000000)
//TESTONLY
#define LCOVERED TRUE
#define LUNCOVERED FALSE

#define FITTED (TRUE)
#define NOT_FITTED (FALSE)

#define FAULTY (FALSE)
#define OK (TRUE)

// KEYPAD ENCODER KEY CODES
#define	KEY_PLUS			0x02		// Plus
#define	KEY_MINUS			0x04		// Minus
#define	KEY_DOWN_ARROW		0x06		// Down arrow
#define	KEY_DOWN_TRIANGLE	0x08		// Down triangle
#define	KEY_UP_ARROW		0x0a		// Up arrow
#define	KEY_DIGIT_3			0x10		// Digit 3
#define	KEY_DIGIT_6			0x12		// Digit 6
#define	KEY_DIGIT_9			0x14		// Digit 9
#define	KEY_LETTER_E		0x16		// E (Enter)
#define	KEY_UP_TRIANGLE		0x18		// Up triangle
#define	KEY_DIGIT_2			0x20		// Digit 2
#define	KEY_DIGIT_5			0x22		// Digit 5
#define	KEY_DIGIT_8			0x24		// Digit 8
#define	KEY_DIGIT_0			0x26		// Digit 0
//#define	KEY_LETTER_F		0x28		// F (Fill)
#define	KEY_DIGIT_1			0x30		// Digit 1
#define	KEY_DIGIT_4			0x32		// Digit 4
#define	KEY_DIGIT_7			0x34		// Digit 7
#define	KEY_DECIMAL_POINT	0x36		// Decimal point
//#define	KEY_AUTO_MANUAL		0x38		// Auto/manual symbol

// On the CP2000/Vac250 we reverse the F and Auto keys and
// use the F as an Esc key.
#define	KEY_AUTO_MANUAL		0x28		// Auto/manual symbol
#define	KEY_ESC				0x38

#define	KEY_MENU_1			0x2a
#define	KEY_MENU_2			0x2c
#define	KEY_MENU_3			0x2e
#define	KEY_MENU_4			0x3a
#define	KEY_MENU_5			0x3c

#define	KEY_INVALID_CODE	-1

// If this code is detected the UFP has been reset
// and we would need to re-initialise the LCD.
#define	KEY_UFP_RESET		0xa5


// Offsets from the basic port address


#define	Q0		0		// FIRST OUTPUT
#define	Q1		1		//
#define	Q2		2		//
#define	Q3		3		//
#define	Q4		4		//
#define	Q5		5		//
#define	Q6		6		//
#define	Q7		7		//

#define PIT_FREQUENCY (3000)      // todo (check)
// Flash rate counters for LED.
#define	FLASH_RATE_1HZ		(PIT_FREQUENCY / 1)
#define	FLASH_RATE_2HZ		(PIT_FREQUENCY / 2)
#define	FLASH_RATE_5HZ		(PIT_FREQUENCY / 5)
#define	FLASH_RATE_10HZ	(PIT_FREQUENCY / 10)
#define	FLASH_RATE_100HZ	(PIT_FREQUENCY / 100)

#define U0_MODE_MTTTY (3)
#define U0_MODE_USB  (2)
#define U0_MODE_PANEL (0)
#define U0_MODE_CFG (1)


// ON-OFF STATES FOR OUTPUTS
#define	NBB_OUTPUT_ON		1		//
#define	NBB_OUTPUT_OFF		0		//
//RevB1  #define	MAX_SOURCE_OUTPUTS (40)     // maximum no. of sourcing outputs (2987) on NBB.
#define	MAX_SOURCE_OUTPUTS (16)     // maximum no. of sourcing outputs (2987) on NBB. Rev B1.

#define NBB_REV_B1  (0x21)       // B=0x20, 1=0x01
#define NBB_REV_B2  (0x22)       // B=0x20, 2=0x02

#define SDCARD_INSERTED_BITPOS   0x80
#define SDCARD_LOCK_BITPOS   0x40




extern    PVWORD	NBB_IPBUFFER;

extern    PVBYTE	NBB_OPLATCH_1;
extern    PVBYTE	NBB_OPLATCH_2;

extern    PVBYTE	NBB_IPBUFFER_1;
extern    PVBYTE	NBB_IPBUFFER_2;

extern    WORD  g_nEEPROMSPIChipSelect; // onboard EEPROM.
extern    WORD  g_nADSPIChipSelect;
extern    WORD  g_nFlashSPIChipSelect;
extern    WORD  g_nCANSPIChipSelect;
extern    WORD  g_nEX1SPIChipSelect;
extern    WORD  g_nVNC1LSPIChipSelect;
extern    WORD  g_nAD2SPIChipSelect;              //2nd A/D.


/******************************************************************
 * This function will read the onboard hex switch (address)
 ******************************************************************/
BYTE ReadHexSwitch( void );

BYTE ReadSDInputs( void );

/******************************************************************
 * This function will initialise the NBB gpio
 ******************************************************************/
void InitialiseNBBGpio( void );
void InitialiseNBBGpioTest( void );

void InitialiseNBTimerPins(void);

/******************************************************************
 * This function will initialise the NBB CS2
 ******************************************************************/
//void Init_ChipSelect3(void);

//Initialise chip selects for NBB Rev B1 pcb.
void Init_RevB1ChipSelects(void);
void Init_RevB1ChipSelect1( void );
void Init_RevB1ChipSelect2( void );
void Init_RevB1ChipSelect3( void );
void U0_Select_Mtty (void);
// This function initialises the edge port (IRQ pins).
void Init_EdgePort(void);

void SRCOff (int nSrcNo);
void SRCOn (int nSrcNo);

void ReadMachineInputs( void );
void InitialiseSPIChipSelects( void );
void InitialiseNBBUartPins(void);
void U0_Select_Operator_Panel(void);

/******************************************************************
 *  Set gpio signal PD0 from module low
 ******************************************************************/
inline void SetPD0_Lo( void )
{
           sim.gpio.pclrr_datal = ~0x01;        // - set PD0 low.
};
/******************************************************************
 *  Set gpio signal PD0 from module high
 ******************************************************************/
inline void SetPD0_Hi( void )
{
           sim.gpio.ppdsdr_datal = 0x01;        // - set PD0 high.
};
#define US_TRIGGER_ON      		( SetPD0_Hi )
#define US_TRIGGER_OFF     		( SetPD0_Lo )


//  Following is a set of functions for driving PD4, PD5, PD6 and PD8-PD15 gpio outputs.
//  NB: PD0-PD3 and PD7 are inputs on the NBB.

/******************************************************************
 *  Set gpio signal PD15 from module high
 ******************************************************************/
inline void SetPD15_Hi( void )
{
           sim.gpio.ppdsdr_datah = 0x80;        // - set PD15 high.
};
/******************************************************************
 *  Set gpio signal PD15 from module low
 ******************************************************************/
inline void SetPD15_Lo( void )
{
//           sim.gpio.pclrr_datah = ~0x80;        // - set PD15 low.
           sim.gpio.pclrr_datah = 0x7f;        // - set PD15 low.
};

/******************************************************************
 *  Set gpio signal PD14 from module low
 ******************************************************************/
inline void SetPD14_Lo( void )
{
           sim.gpio.pclrr_datah = ~0x40;        // - set PD14 low.
};
/******************************************************************
 *  Set gpio signal PD14 from module high
 ******************************************************************/
inline void SetPD14_Hi( void )
{
           sim.gpio.ppdsdr_datah = 0x40;        // - set PD14 high.
};

/******************************************************************
 *  Set gpio signal PD13 from module low
 ******************************************************************/
inline void SetPD13_Lo( void )
{
           sim.gpio.pclrr_datah = ~0x20;        // - set PD13 low.
};

/******************************************************************
 *  Set gpio signal PD13 from module high
 ******************************************************************/
inline void SetPD13_Hi( void )
{
           sim.gpio.ppdsdr_datah = 0x20;        // - set PD13 high.
};

/******************************************************************
 *  Set gpio signal PD12 from module low
 ******************************************************************/
inline void SetPD12_Lo( void )
{
           sim.gpio.pclrr_datah = ~0x10;        // - set PD12 low.
};
/******************************************************************
 *  Set gpio signal PD12 from module high
 ******************************************************************/
inline void SetPD12_Hi( void )
{
           sim.gpio.ppdsdr_datah = 0x10;        // - set PD12 high.
};

/******************************************************************
 *  Set gpio signal PD11 from module low
 ******************************************************************/
inline void SetPD11_Lo( void )
{
           sim.gpio.pclrr_datah = ~0x08;        // - set PD11 low.
};
/******************************************************************
 *  Set gpio signal PD11 from module high
 ******************************************************************/
inline void SetPD11_Hi( void )
{
           sim.gpio.ppdsdr_datah = 0x08;        // - set PD11 high.
};

/******************************************************************
 *  Set gpio signal PD10 from module low
 ******************************************************************/
inline void SetPD10_Lo( void )
{
           sim.gpio.pclrr_datah = ~0x04;        // - set PD10 low.
};
/******************************************************************
 *  Set gpio signal PD10 from module high
 ******************************************************************/
inline void SetPD10_Hi( void )
{
           sim.gpio.ppdsdr_datah = 0x04;        // - set PD10 high.
};

/******************************************************************
 *  Set gpio signal PD9 from module low
 ******************************************************************/
inline void SetPD9_Lo( void )
{
           sim.gpio.pclrr_datah = ~0x02;        // - set PD9 low.
};
/******************************************************************
 *  Set gpio signal PD9 from module high
 ******************************************************************/
inline void SetPD9_Hi( void )
{
           sim.gpio.ppdsdr_datah = 0x02;        // - set PD9 high.
};

/******************************************************************
 *  Set gpio signal PD8 from module low
 ******************************************************************/
inline void SetPD8_Lo( void )
{
           sim.gpio.pclrr_datah = ~0x01;        // - set PD8 low.
};

/******************************************************************
 *  Set gpio signal PD8 from module high
 ******************************************************************/
inline void SetPD8_Hi( void )
{
           sim.gpio.ppdsdr_datah = 0x01;        // - set PD8 high.
};


/******************************************************************
 *  Set gpio signal PD6 from module low
 ******************************************************************/
inline void SetPD6_Lo( void )
{
           sim.gpio.pclrr_datal = ~0x40;        // - set PD6 low.
};

/******************************************************************
 *  Set gpio signal PD6 from module high
 ******************************************************************/
inline void SetPD6_Hi( void )
{
           sim.gpio.ppdsdr_datal = 0x40;        // - set PD6 high.
};

/******************************************************************
 *  Set gpio signal PD5 from module low
 ******************************************************************/
inline void SetPD5_Lo( void )
{
           sim.gpio.pclrr_datal = ~0x20;        // - set PD5 low.
};
/******************************************************************
 *  Set gpio signal PD5 from module high
 ******************************************************************/
inline void SetPD5_Hi( void )
{
           sim.gpio.ppdsdr_datal = 0x20;        // - set PD5 high.
};


/******************************************************************
 *  Set gpio signal PD4 from module low
 ******************************************************************/
inline void SetPD4_Lo( void )
{
           sim.gpio.pclrr_datal = ~0x10;        // - set PD4 low.
};
/******************************************************************
 *  Set gpio signal PD4 from module high
 ******************************************************************/
inline void SetPD4_Hi( void )
{
           sim.gpio.ppdsdr_datal = 0x10;        // - set PD4 high.
};

inline void MakePD4AnInput( void )
{
    sim.gpio.pddr_datal &= ~PDDR_PODR_DATAL4;   //31012007 - Make PD4 an input - for VNC1L chip. spi mode
};







/******************************************************************
 *  Set gpio signal PD3 from module low
 ******************************************************************/
inline void SetPD3_Lo( void )
{
           sim.gpio.pclrr_datal = ~0x08;        // - set PD3 low.
};
/******************************************************************
 *  Set gpio signal PD3 from module high
 ******************************************************************/
inline void SetPD3_Hi( void )
{
           sim.gpio.ppdsdr_datal = 0x08;        // - set PD3 high.
};


/******************************************************************
 *  Set gpio signal PD2 from module low
 ******************************************************************/
inline void SetPD2_Lo( void )
{
           sim.gpio.pclrr_datal = ~0x04;        // - set PD2 low.
};
/******************************************************************
 *  Set gpio signal PD2 from module high
 ******************************************************************/
inline void SetPD2_Hi( void )
{
           sim.gpio.ppdsdr_datal = 0x04;        // - set PD2 high.
};



////////// UART gpio functions./////////////////////////////////////

/******************************************************************
 *  Set gpio signal U1RTS from module high
 ******************************************************************/
inline void SetU1RTS_Hi( void )
{
           sim.gpio.ppdsdr_uartl = 0x40;        // - set U1RTS high.
};

/******************************************************************
 *  Set gpio signal U1RTS from module high
 ******************************************************************/
inline void SetU1RTS_Lo( void )
{
           sim.gpio.pclrr_uartl = ~0x40;        // - set U1RTS low
};


/******************************************************************
 *  Set gpio signal U0CTS from module high
 ******************************************************************/
inline void SetU0CTS_Hi( void )
{
           sim.gpio.ppdsdr_uartl = 0x08;        // - set U0CTS high.
};
/******************************************************************
 *  Set gpio signal U0RTS from module high
 ******************************************************************/
inline void SetU0RTS_Hi( void )
{
           sim.gpio.ppdsdr_uartl = 0x04;        // - set U0RTS high.
};

/******************************************************************
 *  Set gpio signal U0CTS from module high
 ******************************************************************/
inline void SetU0CTS_Lo( void )
{
           sim.gpio.pclrr_uartl = ~0x08;        // - set U0CTS low
};


/******************************************************************
 *  Set gpio signal U0RTS from module low
 ******************************************************************/
inline void SetU0RTS_Lo( void )
{
           sim.gpio.pclrr_uartl = ~0x04;        // - set U0RTS low.
};

// New to Rev B1.
/******************************************************************
 *  Set gpio signal U1CTS from module high
 ******************************************************************/
inline void SetU1CTS_Hi( void )
{
           sim.gpio.ppdsdr_uartl = 0x80;        // - set U1CTS high.
};
// New to Rev B1.
/******************************************************************
 *  Set gpio signal U1CTS from module low
 ******************************************************************/
inline void SetU1CTS_Lo( void )
{
           sim.gpio.pclrr_uartl = (BYTE)(~0x80);        // - set U1CTS low.
};

/* Not present on Rev B1 - U1cts is an output.
inline BYTE ReadINT9555( void )
{
//	BYTE cValue = sim.gpio.ppdsdr_uartl;  	//
	return (sim.gpio.ppdsdr_uartl & 0x80);	   				// return b7 or UARTL reg ( U1CTS).
};
*/
inline BYTE ReadPD7( void )
{
//	BYTE cValue = sim.gpio.ppdsdr_uartl;  	//
	return (sim.gpio.ppdsdr_datal & 0x80);	   				// return b7 of dataL reg ( PD7).
};

inline BYTE ReadPD3( void )
{
//	BYTE cValue = sim.gpio.ppdsdr_uartl;  	//
	return (sim.gpio.ppdsdr_datal & 0x08);	   				// return b3 of dataL reg ( PD7).
};

//Timer signals.
inline BYTE ReadDT2IN( void )
{
	return (sim.gpio.ppdsdr_timer & 0x20);	   				// return b5 of timer reg ( DT2IN).
};

inline BYTE ReadDT1IN( void )
{
	return (sim.gpio.ppdsdr_timer & 0x08);	   				// return b3 of timer reg ( DT1IN).
};

inline BYTE ReadDT0IN( void )
{
	return (sim.gpio.ppdsdr_timer & 0x02);	   				// return b1 of timer reg ( DT0IN).
};

inline BYTE ReadTimerPort( void )
{
	return ( sim.gpio.ppdsdr_timer & 0xff );	   				// return all bits of timer reg
};

/******************************************************************
 *  Set gpio signal DT3OUT from module high
 ******************************************************************/
inline void SetDT3OUT_Hi( void )
{
//           sim.gpio.podr_timer |= 0x40;        // - set DT3OUT high.
           sim.gpio.ppdsdr_timer = 0x40;        // - set DT3OUT high. set b6 in port pin set data reg
};

/******************************************************************
 *  Set gpio signal DT3OUT from module low
 ******************************************************************/
inline void SetDT3OUT_Lo( void )
{
//           sim.gpio.podr_timer &= ~0x40;        // - set DT3OUT low.
            sim.gpio.pclrr_timer = ~0x40;        // - set DT3OUT low (clear bit b6)

};

/******************************************************************
 *  Set gpio signal DT1OUT from module high
 ******************************************************************/
inline void SetDT1OUT_Hi( void )
{
//           sim.gpio.podr_timer |= 0x04;        // - set DT1OUT high.
           sim.gpio.ppdsdr_timer = 0x04;        // - set DT1OUT high.
};

/******************************************************************
 *  Set gpio signal DT1OUT from module low
 ******************************************************************/
inline void SetDT1OUT_Lo( void )
{
//           sim.gpio.podr_timer &= ~0x04;        // - set DT1OUT
           sim.gpio.pclrr_timer = ~0x04;        // - set DT1OUT low
};

/******************************************************************
 *  Set gpio signal DT0OUT from module high
 ******************************************************************/
inline void SetDT0OUT_Hi( void )
{
//           sim.gpio.podr_timer |= 0x01;        // - set DT0OUT high.
           sim.gpio.ppdsdr_timer = 0x01;        // - set DT0OUT high.
};

/******************************************************************
 *  Set gpio signal DT0OUT from module low
 ******************************************************************/
inline void SetDT0OUT_Lo( void )
{
//           sim.gpio.podr_timer &= ~0x01;        // - set DT0OUT
           sim.gpio.pclrr_timer = ~0x01;        // - set DT0OUT low
};

#define SetSPIEX1_Lo ( SetU0RTS_Lo )
#define SetSPIEX1_Hi ( SetU0RTS_Hi )
#define SetSPIEX2_Lo ( SetU0CTS_Lo )
#define SetSPIEX2_Hi ( SetU0CTS_Hi )

#define SPIEX1_Hi ( SetU0RTS_Hi )
#define SPIEX1_Lo ( SetU0RTS_Lo )
#define SPIEX2_Hi ( SetU0CTS_Hi )
#define SPIEX2_Lo ( SetU0CTS_Lo )

// Due to error in PCB layout, MuxSA0/1 are reversed vs. the TSM schematic.
/*
#define MUXSA1_Hi ( SetPD13_Hi )
#define MUXSA1_Lo ( SetPD13_Lo )

#define MUXSA0_Hi ( SetPD12_Hi )
#define MUXSA0_Lo ( SetPD12_Lo )
*/
#define MUXSA1_Hi ( SetPD12_Hi )
#define MUXSA1_Lo ( SetPD12_Lo )

#define MUXSA0_Hi ( SetPD13_Hi )
#define MUXSA0_Lo ( SetPD13_Lo )

#define U1TXEN_Hi ( SetU1RTS_Hi )
#define U1TXEN_Lo ( SetU1RTS_Lo )

#define U2TXEN_Hi ( SetU1CTS_Hi )
#define U2TXEN_Lo ( SetU1CTS_Lo )


// Functions for turning 24V Sourcing outputs (UDN2987) OFF.
// src 1-8
inline void SRC1Off (void)
{
	*(NBB_OPLATCH_1 + Q0) = NBB_OUTPUT_OFF;
};
inline void SRC2Off (void)
{
	*(NBB_OPLATCH_1 + Q1) = NBB_OUTPUT_OFF;
};
inline void SRC3Off (void)
{
	*(NBB_OPLATCH_1 + Q2) = NBB_OUTPUT_OFF;
};
inline void SRC4Off (void)
{
	*(NBB_OPLATCH_1 + Q3) = NBB_OUTPUT_OFF;
};

inline void SRC5Off (void)
{
	*(NBB_OPLATCH_1 + Q4) = NBB_OUTPUT_OFF;
};
inline void SRC6Off (void)
{
	*(NBB_OPLATCH_1 + Q5) = NBB_OUTPUT_OFF;
};
inline void SRC7Off (void)
{
	*(NBB_OPLATCH_1 + Q6) = NBB_OUTPUT_OFF;
};
inline void SRC8Off (void)
{
	*(NBB_OPLATCH_1 + Q7) = NBB_OUTPUT_OFF;
};
// src 9-16
inline void SRC9Off (void)
{
	*(NBB_OPLATCH_2 + Q0) = NBB_OUTPUT_OFF;
};
inline void SRC10Off (void)
{
	*(NBB_OPLATCH_2 + Q1) = NBB_OUTPUT_OFF;
};
inline void SRC11Off (void)
{
	*(NBB_OPLATCH_2 + Q2) = NBB_OUTPUT_OFF;
};
inline void SRC12Off (void)
{
	*(NBB_OPLATCH_2 + Q3) = NBB_OUTPUT_OFF;
};

inline void SRC13Off (void)
{
	*(NBB_OPLATCH_2 + Q4) = NBB_OUTPUT_OFF;
};
inline void SRC14Off (void)
{
	*(NBB_OPLATCH_2 + Q5) = NBB_OUTPUT_OFF;
};
inline void SRC15Off (void)
{
	*(NBB_OPLATCH_2 + Q6) = NBB_OUTPUT_OFF;
};
inline void SRC16Off (void)
{
	*(NBB_OPLATCH_2 + Q7) = NBB_OUTPUT_OFF;
};
//Rev B1 - only 16 src o/p's.

// Functions for turning 24V Sourcing outputs (UDN2987) ON.
// src 1-8
inline void SRC1On (void)
{
	*(NBB_OPLATCH_1 + Q0) = NBB_OUTPUT_ON;
};
inline void SRC2On (void)
{
	*(NBB_OPLATCH_1 + Q1) = NBB_OUTPUT_ON;
};
inline void SRC3On (void)
{
	*(NBB_OPLATCH_1 + Q2) = NBB_OUTPUT_ON;
};
inline void SRC4On (void)
{
	*(NBB_OPLATCH_1 + Q3) = NBB_OUTPUT_ON;
};

inline void SRC5On (void)
{
	*(NBB_OPLATCH_1 + Q4) = NBB_OUTPUT_ON;
};
inline void SRC6On (void)
{
	*(NBB_OPLATCH_1 + Q5) = NBB_OUTPUT_ON;
};
inline void SRC7On (void)
{
	*(NBB_OPLATCH_1 + Q6) = NBB_OUTPUT_ON;
};
inline void SRC8On (void)
{
	*(NBB_OPLATCH_1 + Q7) = NBB_OUTPUT_ON;
};
// src 9-16
inline void SRC9On (void)
{
	*(NBB_OPLATCH_2 + Q0) = NBB_OUTPUT_ON;
};
inline void SRC10On (void)
{
	*(NBB_OPLATCH_2 + Q1) = NBB_OUTPUT_ON;
};
inline void SRC11On (void)
{
	*(NBB_OPLATCH_2 + Q2) = NBB_OUTPUT_ON;
};
inline void SRC12On (void)
{
	*(NBB_OPLATCH_2 + Q3) = NBB_OUTPUT_ON;
};

inline void SRC13On (void)
{
	*(NBB_OPLATCH_2 + Q4) = NBB_OUTPUT_ON;
};
inline void SRC14On (void)
{
	*(NBB_OPLATCH_2 + Q5) = NBB_OUTPUT_ON;
};
inline void SRC15On (void)
{
	*(NBB_OPLATCH_2 + Q6) = NBB_OUTPUT_ON;
};
inline void SRC16On (void)
{
	*(NBB_OPLATCH_2 + Q7) = NBB_OUTPUT_ON;
};

// SPI selects - using '139
//Rev B1.
inline void SPI_Select_EEPROM (void)
{
// SPICS0 LOW.
	SPIEX2_Lo();
	SPIEX1_Hi();
   sim.gpio.par_qspi = 0xFF;  //set SPI pins
};

inline void Set_EEPROM_Write_Protect (void)
{
	SetPD11_Lo();
};

inline void Remove_EEPROM_Write_Protect (void)
{
	SetPD11_Hi();
};


//Rev B1.
inline void SPI_Select_SD (void)
{
// SPICS0 LOW.
	SPIEX2_Hi();
	SPIEX1_Hi();
};
//Rev B1.
inline void SPI_Select_Flash (void)
{
// SPICS0 LOW.
	SPIEX2_Lo();
	SPIEX1_Lo();
};
//Rev B1.
inline void SPI_Select_AD (void)
{
// SPICS0 LOW.
	SPIEX2_Hi();
	SPIEX1_Lo();
};

// 2nd half OF '139
//Rev B1.
inline void SPI_Select_CAN (void)   // y2
{
// SPICS1 LOW.
	SPIEX2_Lo();
	SPIEX1_Hi();
};

//Rev B1.
inline void SPI_Select_USB (void)   // y3
{
	SPIEX2_Hi();
	SPIEX1_Hi();
};

//Rev B1.
inline void SPI_Select_EXPANSION1 (void) // y0
{
// SPICS1 LOW.
	SPIEX2_Lo();
	SPIEX1_Lo();
};

//Rev B1.
inline void SPI_Select_AD2 (void)   // y1
{
// SPICS1 LOW.
	SPIEX2_Hi();
	SPIEX1_Lo();
};

//Rev B1.
inline void U0_Select_Panel (void)   // Select port U0 (RS232) to go to Panel/display
{
// DT0OUT & DT3OUT both low.
	SetDT0OUT_Lo();
	SetDT3OUT_Lo();
};

//Rev B1.
inline void U0_Select_Config (void)   // Select port U0 (RS232) to go to Panel configuration.
{
// DT0OUT = 0, DT3OUT = 1
	SetDT0OUT_Lo();
	SetDT3OUT_Hi();
};


//Rev B1.
inline void U0_Select_USB (void)   // Select port U0 (RS232) to go to USB chip for programming.
{
// DT0OUT = 1, DT3OUT = 1
	SetDT0OUT_Hi();
	SetDT3OUT_Hi();
};

//Rev B1.
inline void AnalogMux_Select_TSTA1 (void)   // Select TSTA1 for analog multiplexer
{
// MUXSA1 = 0, MUXSA0 = 0
	MUXSA1_Lo();
	MUXSA0_Lo();
};

//Rev B1.
inline void AnalogMux_Select_REF (void)   // Select REF for analog multiplexer
{
// MUXSA1 = 0, MUXSA0 = 1
	MUXSA1_Lo();
	MUXSA0_Hi();
};

//Rev B1.
inline void AnalogMux_Select_DAV (void)   // Select D/A for analog multiplexer
{
// MUXSA1 = 1, MUXSA0 = 0
	MUXSA1_Hi();
	MUXSA0_Lo();
};

//Rev B1.
inline void AnalogMux_Select_FANI (void)   // Select FANI (filtered analog input) for analog multiplexer
{
// MUXSA1 = 1, MUXSA0 = 1
	MUXSA1_Hi();
	MUXSA0_Hi();
};

//Rev B1.
inline void VNC1L_PROG_LOW (void)   // Drive prog pin on VNC1L low.
{

	SetPD5_Lo();
};
inline void VNC1L_PROG_HIGH (void)   // Drive prog pin on VNC1L high.
{

	SetPD5_Hi();
};
inline void VNC1L_RESET_LOW (void)   // Drive Reset pin on VNC1L low.
{

	SetPD9_Lo();



};
inline void VNC1L_RESET_HIGH (void)   // Drive Reset pin on VNC1L high.
{

	SetPD9_Hi();
};// Following from general.h  todo perhaps re-instate general.h


// reinitialise pins affected by sd card

inline void ReInitialisePinsUsedBySDCard (void)
{
     sim.gpio.par_qspi = 0xFF;    // set SPI pins for SPI function (MMK).
     sim.gpio.pddr_timer |= 0x45;    //b6 = 1 (DT3out), b2=1 (DT1OUT), b0=1 (DT0OUT) outputs
};

inline void ReInitialiseGPIOPinsUsedBySDCard (void)
{
      sim.gpio.pddr_timer |= 0x45;    //b6 = 1 (DT3out), b2=1 (DT1OUT), b0=1 (DT0OUT) outputs
};


inline void InitialisePinsUsedBySDCard (void)
{
     sim.gpio.par_qspi = 0x1F;    // set SPI pins for i/o
};


inline void SPI_RS422_ENABLE_HIGH (void)   // Drive SPI_RS422_ENABLE high.
{
 SetPD10_Hi();
};//
inline void SPI_RS422_ENABLE_LOW (void)   // Drive SPI_RS422_ENABLE LOW.
{
 SetPD10_Lo();
};// F

inline void USB_5V_ENABLE(void)   // Drive USB 5V ENABLE high.
{
	SetPD2_Hi();
};//
inline void USB_5V_DISABLE(void)   // Drive USB 5V _ENABLE LOW.
{
	SetPD2_Lo();
};// F
/*
inline void VNC1L_COMMAND_MODE(void)   // Set VNC1L to Command mode
{
	SetPD3_Hi();
};//
inline void VNC1L_DATA_MODE(void)   // Set VNC1L to data mode
{
	SetPD3_Lo();
};// F
*/
//Rev B3 NBB pcb DATAREQ# uses PD8
inline void VNC1L_COMMAND_MODE(void)   // Set VNC1L to Command mode
{
	SetPD8_Hi();
};//
inline void VNC1L_DATA_MODE(void)   // Set VNC1L to data mode
{
	SetPD8_Lo();
};// F

inline void SetVNC1L_CTS_Lo(void)   // Set VNC1L's CTS Low
{
	SetPD4_Lo();
};
inline void SetVNC1L_CTS_Hi(void)   // Set VNC1L's CTS Low
{
	SetPD4_Hi();
};
inline BYTE ReadVNC1L_RTS( void )
{
//	BYTE cValue = sim.gpio.ppdsdr_uartl;  	//
	return (sim.gpio.ppdsdr_datal & 0x08);	   				// return b3 of dataL reg ( PD3).
};

/******************************************************************
 *  Set gpio signal PD1 from module low
 ******************************************************************/
inline void SetPD1_Lo( void )
{
           sim.gpio.pclrr_datal = ~0x02;        // - set PD1 low.
};
/******************************************************************
 *  Set gpio signal PD1 from module high
 ******************************************************************/
inline void SetPD1_Hi( void )
{
           sim.gpio.ppdsdr_datal = 0x02;        // - set PD1 high.
};

#define ANALOG_SWITCH_ON      ( SetPD1_Hi )
#define ANALOG_SWITCH_OFF     ( SetPD1_Lo )





// Following from general.h  todo perhaps re-instate general.h

// Convenient method of extracting the high and low
// bytes from an int or converting 2 bytes to an int

union CharsAndWord
{
	WORD		nValue;
	unsigned char	cValue[2];
};


union WordAndDWord
{
	DWORD	lValue;
	WORD	nValue[2];
};

union CharsAndFloat
{
	float	fValue;
	unsigned char	cValue[4];
};

union CharsAndLong
{
	DWORD	lValue;
	unsigned char	cValue[4];
};

void SRCOnWithMapping (int nSrcNo);
void SRCOffWithMapping (int nSrcNo);


#define FillValve1Open ( SRC1On )
#define FillValve2Open ( SRC2On )
#define FillValve3Open ( SRC3On )
#define FillValve4Open ( SRC4On )
#define FillValve5Open ( SRC5On )


#define FillValve1Close ( SRC1Off )
#define FillValve2Close ( SRC2Off )
#define FillValve3Close ( SRC3Off )
#define FillValve4Close ( SRC4Off )
#define FillValve5Close ( SRC5Off )

#define DumpValveOpen ( SRC15On )
#define DumpValveClose ( SRC15Off )

#define MixerMotorOn ( SRC16On )
#define MixerMotorOff ( SRC16Off )
//RevB1
#define BeaconOn( SRC13On )
#define BeaconOff( SRC13Off )

#define GetVNC1LMode ( ReadPD3 )

//For Rev B3 NBB
#define SetVNC1LDataMode ( SetPD8_Lo )
#define SetVNC1LCommandMode ( SetPD8_Hi )

#endif   // __NBBGPIO_H__

