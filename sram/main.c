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
#include <avr/io.h>
#include "user_board.h"


/*pin 6 sck ->scl
pin 5 si ->mosi
pin 1 cs -> ss
in 2 so -> miso*/
int main (void)
{
	// Insert system clock initialization code here (sysclk_init()).

	board_init();

	sram_init();

	cpu_irq_enable();

	irq_initialize_vectors();

	sysclk_init();

	cdc_start();
       
	//spi_init_pins();
	
        spi_init_module();
	
	while(!cdc_opened());
	while (1) {
	//	write_mode();
		write_data();
         	read_data();
	//      display_read_results();
	//	uint8_t read = udi_cdc_getc();
		//if (read != 0)
			//cdc_write_string("Hello. This is quite a long string.\n");
	//	cdc_write_hex(read);
	//	cdc_write_string(" ");
	}


	// Insert application code here, after the board has been initialized.
}
