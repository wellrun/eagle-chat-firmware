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

extern "C" {
	#include "asf.h"
	#include <avr/io.h>
	#include "cdc.h"
}

#include "RFM69.h"

RFM69 module;

int main (void)
{
	// Insert system clock initialization code here (sysclk_init()).

	cpu_irq_enable();

	irq_initialize_vectors();

	sysclk_init();

	rtc_init();

	cdc_start();

	//module = RFM69();

    while (1) {
    	while (udi_cdc_getc() != 'a');
    	cdc_write_string("Logging time: \n");
		cdc_log_int("rtc_get_time(): ", rtc_get_time());
		cdc_write_string("Logged time. \n");
	}

}
