/**
 * \file
 *
 * \brief Empty user application template
 *
 */

/**
 * \mainpage User Application template doxygen documentation
 *
 * \par Empty user application template
 *
 * Bare minimum empty user application template
 *
 * \par Content
 *
 * -# Include the ASF header files (through asf.h)
 * -# "Insert system clock initialization code here" comment
 * -# Minimal main function that starts with a call to board_init()
 * -# "Insert application code here" comment
 *
 */

/*
 * Include header files for all drivers that have been imported from
 * Atmel Software Framework (ASF).
 */
 /**
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */
#include "asf.h"
#include "board.h"
#include "sram.h"
#include <string.h>
#include <avr/io.h>

int main (void)
{
	// Insert system clock initialization code here (sysclk_init()).

	board_init();

	sram_init();

	cpu_irq_enable();

	irq_initialize_vectors();

	sysclk_init();

	sram_init();

	cdc_start();

	while(!cdc_opened());

	cdc_write_line("Starting SRAM demo.");

	cdc_write_line("Setting byte mode.");

	bool success = sram_set_mode(SRAM_MODE_BYTE);

	if (success) {
		cdc_write_line("Byte mode set.");
	} else {
		cdc_write_line("Failure: byte mode not set.");
	}

	uint8_t test_value = 0xAA;
	cdc_log_hex("Writing to SRAM: ", test_value);
	sram_write_byte(0, test_value);
	test_value = 0;
	test_value = sram_read_byte(0);
	cdc_log_hex("Read back: ", test_value);

	uint8_t data[] = "This is a long string of data. Que?";
	size_t len = sizeof(data);
	uint8_t read_data[len];

	memset(read_data, 0, len);
	cdc_newline();
	cdc_write_line("Testing sequential writes and reads.");
	cdc_log_string("Wrote: ", data);
	sram_set_mode(SRAM_MODE_SEQUENTIAL);
	sram_write_packet(1000, data, len);
	sram_read_packet(1000, read_data, len);
	cdc_log_string("Read: ", read_data);

	memset(read_data, 0, len);
	cdc_newline();
	cdc_write_line("Testing page writes and reads.");
	cdc_log_string("Wrote: ", data);
	sram_set_mode(SRAM_MODE_PAGE);
	sram_write_packet(100, data, len);
	sram_read_packet(100, read_data, len);
	cdc_write_line("Read:");
	for (size_t i = 0; i < len; ++i) {
		udi_cdc_putc(read_data[i]);
	}
	cdc_newline();

	memset(read_data, 0, len);
	cdc_newline();
	cdc_write_line("Testing byte writes and reads.");
	cdc_log_string("Wrote: ", data);
	sram_set_mode(SRAM_MODE_BYTE);
	sram_write_packet(200, data, len-1);
	sram_read_packet(200, read_data, len-1);
	cdc_write_line("Read:");
	for (size_t i = 0; i < len-1; ++i) {
		udi_cdc_putc(read_data[i]);
	}
	cdc_newline();


	while (1) {

	}
}
