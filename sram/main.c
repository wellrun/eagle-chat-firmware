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


int main (void)
{
	// Insert system clock initialization code here (sysclk_init()).
	uint8_t arrayt[15] = "obrigadotudo";
	uint8_t arry2[15] = "redsor";
	uint8_t fillthis[40];
	long int i,j;
	board_init();

	sram_init();

	cpu_irq_enable();

	irq_initialize_vectors();

	sysclk_init();

	cdc_start();
	
        spi_init_module();
	
	while(!cdc_opened());
	while (1) {
		for (i=1 ; i<15000000 ; i++) ;
		{
			j=i;
		}
		
		write_mode(1);// 0 byte, 1 sequential, 2 page
		write_data(arrayt);//array of data to send		
		write_data(arry2);//array of data to send
		
		read_data(9,fillthis);
		cdc_write_string(fillthis);
		cdc_newline();

	}


	// Insert application code here, after the board has been initialized.
}
