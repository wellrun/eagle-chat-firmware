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
#include "cdc.h"
#include <string.h>
#include <avr/io.h>
#include "conf_atsha204.h"
#include "sha204_timer.h"
#include "sha204_command_marshaling.h"
#include "sha204_lib_return_codes.h"

uint8_t sha204_i2c_address(uint8_t index);

uint8_t sha204_i2c_address(uint8_t index)
{
	return SHA204_I2C_DEFAULT_ADDRESS;
}


int main (void)
{
	// Insert system clock initialization code here (sysclk_init()).

	cpu_irq_enable();

	irq_initialize_vectors();

	sysclk_init();

	// Initialize the board.
	board_init();

	cdc_start();

	uint8_t tx_buffer_command[SHA204_CMD_SIZE_MIN];
	uint8_t rx_buffer[DEVREV_RSP_SIZE];
	uint8_t sha204_lib_return;
	uint8_t i;
	uint8_t device_present_mask;
	uint8_t sha204_revision;

	while (udi_cdc_getc() != 'g');

	// Indicate entering main loop.
	cdc_write_hex(0xFF);
	sha204h_delay_ms(1000);
	cdc_write_hex(0x00);

	// The main loop wakes up a device, retrieves its revision, and puts it
	// back to sleep. It does this for all the SHA204 devices on the
	// Security Xplained extension board except the one that is used for identifying
	// an extension board.
	while (true) {
		device_present_mask = sha204_revision = 0;
		// Generate Wakeup pulse. All SHA204 devices that share SDA will wake up.
	    cdc_write_string("\nLine 84\n");
		sha204_lib_return = sha204p_wakeup();
		cdc_write_string("return code=");
		cdc_write_hex(sha204_lib_return);
		cdc_write_string("\n");
	    if (sha204_lib_return != SHA204_SUCCESS) {
			// Indicate Wakeup failure.
			for (i = 0; i < 8; i++) {
				cdc_write_hex(0xFF);
				sha204h_delay_ms(50);
				cdc_write_hex(0x00);
				sha204h_delay_ms(50);
			}
			continue;		 
	    }

	    cdc_write_string("\nLine 96\n");

        // Read the revision from all devices and put them to sleep.
		struct sha204_dev_rev_parameters dev_rev = {
				.rx_buffer = rx_buffer, .tx_buffer = tx_buffer_command};
		for (i = 0; i < 1; i++) {
			sha204p_set_device_id(sha204_i2c_address(i));
			memset(rx_buffer, 0, sizeof(rx_buffer));
	   
			// Send DevRev command and receive its response.
			sha204_lib_return = sha204m_dev_rev(&dev_rev);
			if (sha204_lib_return != SHA204_SUCCESS)
				continue;
			
			// Store result.
			device_present_mask |= (1 << i);
			sha204_revision = rx_buffer[SHA204_BUFFER_POS_DATA + 3];

			// Send Sleep command.
			sha204p_sleep();
		}
		cdc_write_hex(device_present_mask);
		cdc_write_hex(sha204_revision);
		//display_status(device_present_mask, sha204_revision);
	}	

	return sha204_lib_return;


	// Insert application code here, after the board has been initialized.
}
