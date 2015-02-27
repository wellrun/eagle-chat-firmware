#include "asf.h"
#include "avrnacl.h"
#include "cdc.h"
#include "randombytes.h"
#include <avr/io.h>

int main (void)
{
	cpu_irq_enable();

	irq_initialize_vectors();

	sysclk_init();

	cdc_start();

	unsigned char k[crypto_box_BEFORENMBYTES]; // Buffer to hold the shared secret
	unsigned char public_key[crypto_box_PUBLICKEYBYTES]; // Public key of the other guy
	unsigned char secret_key[crypto_box_SECRETKEYBYTES]; // My private or secret key

	while (1) {
		while(udi_cdc_getc() != 'a');
		cdc_write_string("Secret key:\n");

		randombytes(secret_key, crypto_box_BEFORENMBYTES);

		for (uint8_t i = 0; i < crypto_box_BEFORENMBYTES; ++i) {
			cdc_write_hex(secret_key[i]);
		}
		cdc_write_string("\n");

		crypto_box_keypair(public_key, secret_key);

		cdc_write_string("Public key:\n");

		for (uint8_t i = 0; i < crypto_box_PUBLICKEYBYTES; ++i) {
			cdc_write_hex(public_key[i]);
		}
		cdc_write_string("\n");

		crypto_box_beforenm(k, public_key, secret_key); // Stores shared secret in k

		cdc_write_string("Shared secret:\n");

		for (uint8_t i = 0; i < crypto_box_BEFORENMBYTES; ++i) {
			cdc_write_hex(k[i]);
		}
		cdc_write_string("\n");
	}

}
