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
#include "avrnacl.h"
#include <avr/io.h>

int main (void)
{
	// Insert system clock initialization code here (sysclk_init()).

	sysclk_init();

	unsigned char buf1[crypto_secretbox_KEYBYTES];
	unsigned char buf2[crypto_secretbox_KEYBYTES];
	unsigned char buf3[crypto_secretbox_KEYBYTES];
	unsigned char buf4[crypto_secretbox_KEYBYTES];

	crypto_secretbox(buf1, buf2, crypto_secretbox_KEYBYTES, buf3, buf4);

	// Insert application code here, after the board has been initialized.
}
