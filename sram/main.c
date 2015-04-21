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
	uint8_t arry1[15] = "obrigadotudo";//12
	uint8_t arry2[15] = "redsor";//6 //345 chars in next array
	uint8_t arry3[350] = "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAB";
	uint8_t arry4[15] = "scooby";
	uint8_t fillthis[400];
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
		
		write_data(arry4);//array of data to send		
		write_data(arry2);
		read_data(400,fillthis);		
		cdc_write_string(fillthis);
		//cdc_newline();

		write_data(arry1);//array of data to send
		read_data(90,fillthis);
		cdc_write_string(fillthis);
		
		write_data(arry2);
		write_data(arry3);
		write_data(arry1);
		read_data(600,fillthis);
		cdc_write_string(fillthis);
		//cdc_newline();

	}


	// Insert application code here, after the board has been initialized.
}

