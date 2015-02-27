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

#define SRAM_SPI 		SPIC
#define SRAM_CS_pin 	IOPORT_CREATE_PIN(PORTC, 4)
#define SRAM_MOSI_pin	IOPORT_CREATE_PIN(PORTC, 5)
#define SRAM_MISO_pin	IOPORT_CREATE_PIN(PORTC, 6)
#define SRAM_SCK_pin	IOPORT_CREATE_PIN(PORTC, 7)

/************************
* RFM69HW Radio Module
*************************/

/**
* ATSHA crypto module
*/

/**
* Break out pins
*/

/**
* VBUS sense
*/

// pin PE3


#endif // USER_BOARD_H
