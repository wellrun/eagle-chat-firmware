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
#include "sha204_comm.h"
#include "sha204_timer.h"
#include "sha204_command_marshaling.h"
#include "sha204_lib_return_codes.h"

uint8_t sha204_i2c_address(uint8_t index);

uint8_t sha204_i2c_address(uint8_t index)
{
	return SHA204_I2C_DEFAULT_ADDRESS;
}


void getDeviceRevision() {

	cdc_write_line("Getting device revision");

	uint8_t tx_buffer_command[DEVREV_COUNT];
	uint8_t rx_buffer[DEVREV_RSP_SIZE];
	uint8_t libreturn;

	struct sha204_dev_rev_parameters dev_rev;
	dev_rev.rx_buffer = rx_buffer;
	dev_rev.tx_buffer = tx_buffer_command;

	sha204p_set_device_id(200);
	memset(tx_buffer_command, 0, sizeof(tx_buffer_command));
	memset(rx_buffer, 0, sizeof(rx_buffer));

	libreturn = sha204m_dev_rev(&dev_rev);
	if (libreturn == SHA204_SUCCESS) {
		uint8_t sha204_revision = rx_buffer[SHA204_BUFFER_POS_DATA + 3];
		cdc_log_hex("Revision: ", sha204_revision);
	}
}

void lock(uint8_t zone) {

	uint8_t tx_buffer_command[LOCK_COUNT];
	uint8_t rx_buffer[LOCK_RSP_SIZE];
	uint8_t libreturn;

	struct sha204_lock_parameters lock;
	lock.zone = zone;
	lock.tx_buffer = tx_buffer_command;
	lock.rx_buffer = rx_buffer;
	lock.summary = 0x0000;

	sha204p_set_device_id(200);
	memset(tx_buffer_command, 0, sizeof(tx_buffer_command));
	memset(rx_buffer, 0, sizeof(rx_buffer));

	libreturn = sha204m_lock(&lock);

	cdc_log_int("Lock success: ", libreturn);

}

void getRandom() {

	uint8_t tx_buffer_command[RANDOM_COUNT];
	uint8_t rx_buffer[RANDOM_RSP_SIZE];
	uint8_t libreturn;

	struct sha204_random_parameters random;
	random.mode = 0;
	random.tx_buffer = tx_buffer_command;
	random.rx_buffer = rx_buffer;

	sha204p_set_device_id(200);
	memset(tx_buffer_command, 0, sizeof(tx_buffer_command));
	memset(rx_buffer, 0, sizeof(rx_buffer));

	cdc_write_line("About to call random");
	libreturn = sha204m_random(&random);

	cdc_log_int("Success: ", libreturn);
	if (libreturn == SHA204_SUCCESS) {

		cdc_log_hex_string("Random buffer (hopefully): ", &random.rx_buffer[SHA204_BUFFER_POS_DATA], 32);

	}
}

int demo() {

	uint8_t wr[4];

	while (true) {

		while(udi_cdc_getc() != 'a');

		cdc_log_int("Wakeup: ", sha204p_wakeup());

		getDeviceRevision();
		getRandom();

		sha204p_sleep();

	}
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

	cdc_write_line("Waiting 1s");
	sha204h_delay_ms(1000);

	uint8_t result;
	while(udi_cdc_getc() != 'a');
	cdc_write_line("Press a or b to perform lock 1 or lock 2 respectivley,  or c to run the demo");
	result = udi_cdc_getc();


	if (result == 'a') {

		cdc_log_int("Wakeup: ", sha204p_wakeup());

		cdc_write_line("Locking configuration zone first.");
		lock(0x80);

		sha204p_sleep();
		sha204h_delay_ms(5000);
		sha204p_wakeup();

		cdc_write_line("Locking Data/OTP zone second");
		lock(0x81);

		sha204p_sleep();

		cdc_write_line("Done. Super done.");
		for (int i = 0; i < 50; i ++) {
			cdc_write_string("This is printed to flush the buffer or some shit that makes the rest of the shit actually print... ");
		}

		while (udi_cdc_getc() != 'a');
		cdc_write_line("Done!");
		// for (int i = 0; i < 50; i ++) {
		// 	cdc_write_string("Done! ");
		// }

	} else if (result == 'b') {

		// cdc_log_int("Wakeup: ", sha204p_wakeup());

		// cdc_write_line("Locking Data/OTP zone second");
		// lock(0x81);

		// sha204p_sleep();

		// cdc_write_line("Done.");

	} else if (result == 'c') {

		cdc_write_line("Running demo");
		demo();
	
	}


}
