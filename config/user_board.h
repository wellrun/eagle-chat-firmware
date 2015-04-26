/**
 * \file
 *
 * \brief EagleChat board configuration. References v0.7 of the PCB design.
 *
 */

#ifndef USER_BOARD_H
#define USER_BOARD_H

// Pin and port definitions for the hardware on the board

/************
* SRAM chip
*************/

// CS = PC4
// SCK = PC7
// MOSI = PC5
// MISO = PC6
// HOLD = PD0

#define SRAM_SPI 		SPIC
#define SRAM_CS_pin 	IOPORT_CREATE_PIN(PORTC, 4)
#define SRAM_MOSI_pin	IOPORT_CREATE_PIN(PORTC, 5)
#define SRAM_MISO_pin	IOPORT_CREATE_PIN(PORTC, 6)
#define SRAM_SCK_pin	IOPORT_CREATE_PIN(PORTC, 7)
#define SRAM_HOLD_pin   IOPORT_CREATE_PIN(PORTD, 0)

/************************
* RFM69HW Radio Module
*************************/

// SS = PE4
// SCK = PE7
// MOSI = PE5
// MISO = PE6
// DIO0 = PF0
// DIO1 = PF1
// DIO2 = PF2
// DIO3 = PF3
// DIO4 = PF4
// DIO5 = PF5
// RST = PF7

#define RF_SPI 		SPIE
#define RF_DIO_PORT PORTF
#define RF_SS_pin	IOPORT_CREATE_PIN(PORTE, 4) //Unmodified board's correct SS output pin
#define RF_SCK_pin	IOPORT_CREATE_PIN(PORTE, 7)
#define RF_MOSI_pin	IOPORT_CREATE_PIN(PORTE, 5)
#define RF_MISO_pin	IOPORT_CREATE_PIN(PORTE, 6)
#define RF_DIO0_pin	IOPORT_CREATE_PIN(PORTF, 0)
#define RF_DIO1_pin	IOPORT_CREATE_PIN(PORTF, 1)
#define RF_DIO2_pin	IOPORT_CREATE_PIN(PORTF, 2)
#define RF_DIO3_pin	IOPORT_CREATE_PIN(PORTF, 3)
#define RF_DIO4_pin	IOPORT_CREATE_PIN(PORTF, 4)
#define RF_DIO5_pin	IOPORT_CREATE_PIN(PORTF, 5) //ALSO D-!!!!
#define RF_RST_pin	IOPORT_CREATE_PIN(PORTF, 7) //ALSO D+!!!!


/**
* ATSHA crypto module
*/

// SDA = PC0
// SCK = PC1

#define TWI_ATSHA	TWIC
#define ATSHA_SDA	IOPORT_CREATE_PIN(PORTC, 0)
#define ATSHA_SCK	IOPORT_CREATE_PIN(PORTC, 1)

/**
* Break out pins
*/

// OC0A = PD0
// OC0B XCK0 = PD1
// OC0C RXD0 = PD2
// OC0D TXD0 = PD3
// OC1A SS = PD4
// OC1B XCK1 MOSI = PD5
// D- RXD1 MISO = PD6
// D+ TXD1 SCK clkPER EVOUT = PD7

#define UART_DBG	USARTD0
#define SPI_DBG		SPID
#define DBG_LED		IOPORT_CREATE_PIN(PORTD, 0)
#define DBG_XCK		IOPORT_CREATE_PIN(PORTD, 1)
#define DBG_RXD		IOPORT_CREATE_PIN(PORTD, 2)
#define DBG_TXD		IOPORT_CREATE_PIN(PORTD, 3)
#define DBG_SS		IOPORT_CREATE_PIN(PORTD, 4)
#define DBG_MOSI	IOPORT_CREATE_PIN(PORTD, 5)
#define DBG_MISO	IOPORT_CREATE_PIN(PORTD, 6)
#define DBG_SCK		IOPORT_CREATE_PIN(PORTD, 7)

/**
* VBUS sense
*/

// pin PE3

#define VBUS_SENSE_pin	IOPORT_CREATE_PIN(PORTE, 3)



#define DEBUG_LED_pin	IOPORT_CREATE_PIN(PORTD, 4)


#endif // USER_BOARD_H
