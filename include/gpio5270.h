/*******************************************************************************
*                                                                              *
*   NetBurner MOD5270 General Purpose Input/Output Header File                 *
*   gpio5270.h                                                                 *
*                                                                              *
*******************************************************************************/


#ifndef __GPIP5270_H__
#define __GPIP5270_H__


////////////////////////////////////////////////////////////////////////////////
// GPIO Pin Assignment Register Definitions. All signals are referenced to their
// primary functions. Writing the complement of each of the following defined
// values associated with a signal in the register will configure the
// corresponding pin for GPIO.
//

//
// For use with data port pin assignment register: sim.gpio.par_ad.
//
#define GPIO_PAR_DATA       (   0x01 )   // Bit       0 - P Data High/Low [15:0]

//
// For use with DMA timer port pin assignment register: sim.gpio.par_timer.
//
#define GPIO_PAR_DTOUT0     ( 0x0003 )   // Bit  1 -  0 - DMA Timer Output 0
#define GPIO_PAR_DTOUT1     ( 0x000C )   // Bit  3 -  2 - DMA Timer Output 1
#define GPIO_PAR_DTOUT3     ( 0x00C0 )   // Bit  7 -  6 - DMA Timer Output 3
#define GPIO_PAR_TIN0       ( 0x0300 )   // Bit  9 -  8 - DMA Timer Input 0
#define GPIO_PAR_TIN1       ( 0x0C00 )   // Bit 11 - 10 - DMA Timer Input 1
#define GPIO_PAR_TIN2       ( 0x3000 )   // Bit 13 - 12 - DMA Timer Input 2

//
// For use with I2C port pin assignment register: sim.gpio.par_feci2c.
//
#define GPIO_PAR_SDA        (   0x03 )   // Bit  1 -  0 - I2C Serial Data
#define GPIO_PAR_SCL        (   0xC0 )   // Bit  3 -  2 - I2C Serial Clock

//
// For use with QSPI port pin asssignment register: sim.gpio.par_qspi.
//
#define GPIO_PAR_SPICLK     (   0x03 )   // Bit  1 -  0 - SPI Clock
#define GPIO_PAR_SPIDOUT    (   0x04 )   // Bit       2 - SPI Data Out
#define GPIO_PAR_SPIDIN     (   0x18 )   // Bit  4 -  3 - SPI Data In
#define GPIO_PAR_SPICS0     (   0x20 )   // Bit       5 - SPI Chip Select 0
#define GPIO_PAR_SPICS1     (   0xC0 )   // Bit  7 -  6 - SPI Chip Select 1

//
// For use with UART port pin assignment register: sim.gpio.par_uart.
//
#define GPIO_PAR_URT0RTS    ( 0x0001 )   // Bit       0 - UART 0 - Ready to Send
#define GPIO_PAR_URT0CTS    ( 0x0002 )   // Bit       1 - UART 0 - Clear to Send
#define GPIO_PAR_UTXD0      ( 0x0004 )   // Bit       2 - UART 0 - Transmit
#define GPIO_PAR_URXD0      ( 0x0008 )   // Bit       3 - UART 0 - Receive
#define GPIO_PAR_URT1RTS    ( 0x0030 )   // Bit  5 -  4 - UART 1 - Ready to Send
#define GPIO_PAR_URT1CTS    ( 0x00C0 )   // Bit  7 -  6 - UART 1 - Clear to Send
#define GPIO_PAR_UTXD1      ( 0x0300 )   // Bit  9 -  8 - UART 1 - Transmit
#define GPIO_PAR_URXD1      ( 0x0C00 )   // Bit 11 - 10 - UART 1 - Receive
#define GPIO_PAR_UART2TX    ( 0x1000 )   // Bit      12 - UART 2 - Transmit
#define GPIO_PAR_UART2RX    ( 0x2000 )   // Bit      13 - UART 2 - Receive


////////////////////////////////////////////////////////////////////////////////
// GPIO Port Data Direction Register and Port Output Data Register Definitions.
// The following defined values are only for use on corresponding pins that are
// configured for GPIO.
//
// Port Data Direction Register
// Write: 0 = Input, 1 = Output
//
// Port Output Data Register
// Write: 0 = Low, 1 = High
// Read: The current values in the register (not the port pin values)
//

//
// For use with data port data direction register and data port output data
// register: sim.gpio.pddr_datal, sim.gpio.podr_datal.
//
#define PDDR_PODR_DATAL0    (   0x01 )   // Bit 0 - P Data Low   0
#define PDDR_PODR_DATAL1    (   0x02 )   // Bit 1 - P Data Low   1
#define PDDR_PODR_DATAL2    (   0x04 )   // Bit 2 - P Data Low   2
#define PDDR_PODR_DATAL3    (   0x08 )   // Bit 3 - P Data Low   3
#define PDDR_PODR_DATAL4    (   0x10 )   // Bit 4 - P Data Low   4
#define PDDR_PODR_DATAL5    (   0x20 )   // Bit 5 - P Data Low   5
#define PDDR_PODR_DATAL6    (   0x40 )   // Bit 6 - P Data Low   6
#define PDDR_PODR_DATAL7    (   0x80 )   // Bit 7 - P Data Low   7

//
// For use with data port data direction register and data port output data
// register: sim.gpio.pddr_datah, sim.gpio.podr_datah.
//
#define PDDR_PODR_DATAH8    (   0x01 )   // Bit 0 - P Data High  8
#define PDDR_PODR_DATAH9    (   0x02 )   // Bit 1 - P Data High  9
#define PDDR_PODR_DATAH10   (   0x04 )   // Bit 2 - P Data High 10
#define PDDR_PODR_DATAH11   (   0x08 )   // Bit 3 - P Data High 11
#define PDDR_PODR_DATAH12   (   0x10 )   // Bit 4 - P Data High 12
#define PDDR_PODR_DATAH13   (   0x20 )   // Bit 5 - P Data High 13
#define PDDR_PODR_DATAH14   (   0x40 )   // Bit 6 - P Data High 14
#define PDDR_PODR_DATAH15   (   0x80 )   // Bit 7 - P Data High 15

//
// For use with DMA timer port data direction register and DMA timer port output
// data register: sim.gpio.pddr_timer, sim.gpio.podr_timer.
//
#define PDDR_PODR_DTOUT0    (   0x01 )   // Bit 0 - DMA Timer Output 0
#define PDDR_PODR_DTOUT1    (   0x04 )   // Bit 2 - DMA Timer Output 1
#define PDDR_PODR_DTOUT3    (   0x40 )   // Bit 6 - DMA Timer Output 3
#define PDDR_PODR_TIN0      (   0x02 )   // Bit 1 - DMA Timer Input  0
#define PDDR_PODR_TIN1      (   0x08 )   // Bit 3 - DMA Timer Input  1
#define PDDR_PODR_TIN2      (   0x20 )   // Bit 5 - DMA Timer Input  2

//
// For use with I2C port data direction register and I2C port output data
// register: sim.gpio.pddr_feci2c, sim.gpio.podr_feci2c.
//
#define PDDR_PODR_SDA       (   0x01 )   // Bit 0 - I2C Serial Data
#define PDDR_PODR_SCL       (   0x02 )   // Bit 1 - I2C Serial Clock

//
// For use with QSPI port data direction register and QSPI port output data
// register: sim.gpio.pddr_qspi, sim.gpio.podr_qspi.
//
#define PDDR_PODR_SPICLK    (   0x04 )   // Bit 2 - SPI Clock
#define PDDR_PODR_SPIDOUT   (   0x01 )   // Bit 0 - SPI Data Out
#define PDDR_PODR_SPIDIN    (   0x02 )   // Bit 1 - SPI Data In
#define PDDR_PODR_SPICS0    (   0x08 )   // Bit 3 - SPI Chip Select 0
#define PDDR_PODR_SPICS1    (   0x10 )   // Bit 4 - SPI Chip Select 1

//
// For use with UART port data direction register and UART port output data
// register: sim.gpio.pddr_uartl, sim.gpio.podr_uartl.
//
#define PDDR_PODR_URT0RTS   (   0x04 )   // Bit 2 - UART 0 - Ready to Send
#define PDDR_PODR_URT0CTS   (   0x08 )   // Bit 3 - UART 0 - Clear to Send
#define PDDR_PODR_UTXD0     (   0x02 )   // Bit 1 - UART 0 - Transmit
#define PDDR_PODR_URXD0     (   0x01 )   // Bit 0 - UART 0 - Receive
#define PDDR_PODR_URT1RTS   (   0x40 )   // Bit 6 - UART 1 - Ready to Send
#define PDDR_PODR_URT1CTS   (   0x80 )   // Bit 7 - UART 1 - Clear to Send
#define PDDR_PODR_UTXD1     (   0x20 )   // Bit 5 - UART 1 - Transmit
#define PDDR_PODR_URXD1     (   0x10 )   // Bit 4 - UART 1 - Receive

//
// For use with UART port data direction register and UART port output data
// register: sim.gpio.pddr_uarth, sim.gpio.podr_uarth.
//
#define PDDR_PODR_UART2TX   (   0x02 )   // Bit 1 - UART 2 - Transmit
#define PDDR_PODR_UART2RX   (   0x01 )   // Bit 0 - UART 2 - Receive


#endif // __GPIP5270_H__
