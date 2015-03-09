#include "sram.h"
#include <user_board.h>
#include "asf.h"

#define READ 	0b011
#define WRITE 	0b010
#define RDSR	0b101
#define WRSR	0b001

struct spi_device spi_device_conf = {
	.id = SRAM_CS_pin
};

void sram_init(void) {
	ioport_set_pin_dir(SRAM_CS_pin, IOPORT_DIR_OUTPUT);
	ioport_set_pin_dir(SRAM_MOSI_pin, IOPORT_DIR_OUTPUT);
	ioport_set_pin_dir(SRAM_MISO_pin, IOPORT_DIR_INPUT);
	ioport_set_pin_dir(SRAM_SCK_pin, IOPORT_DIR_OUTPUT);
	ioport_set_pin_dir(SRAM_HOLD_pin, IOPORT_DIR_OUTPUT);

	ioport_set_pin_high(SRAM_HOLD_pin); // Make sure our own CS pin doesn't trigger the SPI module to go into slave mode
	ioport_set_pin_high(SRAM_CS_pin); // Make sure our own CS pin doesn't trigger the SPI module to go into slave mode
	
	spi_master_init(&SRAM_SPI);
    spi_master_setup_device(&SRAM_SPI, &spi_device_conf, SPI_MODE_0, 31000000, 0);
    spi_enable(&SRAM_SPI);
}

void sram_write_byte(uint16_t address, uint8_t byte) {
	sram_start_t start = {WRITE, address};
	spi_select_device(&SRAM_SPI, &spi_device_conf);
	spi_write_packet(&SRAM_SPI, (uint8_t *) &start, sizeof(sram_start_t));
	spi_write_packet(&SRAM_SPI, &byte, 1);
	spi_deselect_device(&SRAM_SPI, &spi_device_conf);
}

void sram_write_packet(uint16_t address, const void * source, uint16_t len) {
	sram_start_t start = {WRITE, address};
	spi_select_device(&SRAM_SPI, &spi_device_conf);
	spi_write_packet(&SRAM_SPI, (uint8_t *) &start, sizeof(sram_start_t));
	spi_write_packet(&SRAM_SPI, (uint8_t *) source, len);
	spi_deselect_device(&SRAM_SPI, &spi_device_conf);
}

uint8_t sram_read_byte(uint16_t address) {
	uint8_t byte;
	sram_start_t start = {READ, address};
	spi_select_device(&SRAM_SPI, &spi_device_conf);
	spi_write_packet(&SRAM_SPI, (uint8_t *) &start, sizeof(sram_start_t));
	spi_read_packet(&SRAM_SPI, &byte, 1);
	spi_deselect_device(&SRAM_SPI, &spi_device_conf);
	return byte;
}

uint8_t * sram_read_packet(uint16_t address, uint8_t * dest, uint16_t len) {
	sram_start_t start = {READ, address};
	spi_select_device(&SRAM_SPI, &spi_device_conf);
	spi_write_packet(&SRAM_SPI, (uint8_t *) &start, sizeof(sram_start_t));
	spi_read_packet(&SRAM_SPI, dest, len);
	spi_deselect_device(&SRAM_SPI, &spi_device_conf);
	return dest;
}

bool sram_set_mode(sram_mode_t mode) {
	uint8_t packet[2] = {WRSR, (uint8_t) mode};
	spi_select_device(&SRAM_SPI, &spi_device_conf);
	spi_write_packet(&SRAM_SPI, packet, 2);
	spi_deselect_device(&SRAM_SPI, &spi_device_conf);
	uint8_t status = sram_read_status();
	return ((status & 0b11000000) == mode) && (status & 0b10); // check that read status is correct
}

uint8_t sram_read_status() {
	uint8_t packet[2] = {RDSR, 0};
	spi_select_device(&SRAM_SPI, &spi_device_conf);
	spi_write_packet(&SRAM_SPI, &packet[0], 1);
	spi_read_packet(&SRAM_SPI, &packet[1], 1);
	spi_deselect_device(&SRAM_SPI, &spi_device_conf);
	return packet[1];
}



