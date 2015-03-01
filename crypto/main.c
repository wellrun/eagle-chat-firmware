#include "asf.h"
#include "cdc.h"
#include <avr/io.h>

#include "randombytes.h"
#include "avrnacl.h"
#include "crypto.h"

void cdc_write_hex_string(char *string, uint16_t length) {
	for (uint8_t i = 0; i < length; ++i) {
		cdc_write_hex(string[i]);
	}
	cdc_write_string("\n");
}

int main (void)
{
	cpu_irq_enable();

	irq_initialize_vectors();

	sysclk_init();

	cdc_start();

	uint8_t randomSeed[crypto_box_SECRETKEYBYTES];

	uint8_t aPublic[crypto_box_PUBLICKEYBYTES];
	uint8_t aPrivate[crypto_box_SECRETKEYBYTES];

	uint8_t bPublic[crypto_box_PUBLICKEYBYTES];
	uint8_t bPrivate[crypto_box_SECRETKEYBYTES];

	uint8_t *testMessage = "test";
	uint32_t mlen = 4;

	uint32_t encryptedLength = ENCRYPTED_LENGTH(mlen);
	uint32_t decryptedLength = DECRYPTED_LENGTH(encryptedLength);

	uint8_t nonce[crypto_box_NONCEBYTES];

	uint8_t encrypted[encryptedLength];
	uint8_t decrypted[decryptedLength];

	while (1) {
		
		cdc_write_string("Press a to run: \n");
		while (udi_cdc_getc() != 'a');

		cdc_write_string("First, generate a public and private key for us\n");

		randombytes(randomSeed, crypto_box_SECRETKEYBYTES);
		cr_generate_keypair(aPublic, aPrivate, randomSeed);
		randombytes(randomSeed, crypto_box_SECRETKEYBYTES);
		cr_generate_keypair(bPublic, bPrivate, randomSeed);

		cdc_write_string("A private: ");
		cdc_write_hex_string(aPrivate, crypto_box_SECRETKEYBYTES); cdc_newline();
		cdc_write_string("A public : ");
		cdc_write_hex_string(aPublic, crypto_box_PUBLICKEYBYTES); cdc_newline();
		cdc_write_string("B private: ");
		cdc_write_hex_string(bPrivate, crypto_box_SECRETKEYBYTES); cdc_newline();
		cdc_write_string("B public : ");
		cdc_write_hex_string(bPublic, crypto_box_PUBLICKEYBYTES); cdc_newline();
		cdc_write_string("\n");


		randombytes(nonce, crypto_box_NONCEBYTES);

		cdc_write_string("Message: ");
		cdc_write_string(testMessage); cdc_newline();
		cdc_write_string("Nonce  : ");
		cdc_write_hex_string(nonce, crypto_box_NONCEBYTES); cdc_newline();

		cr_encrypt(encrypted, testMessage, mlen, aPrivate, bPublic, nonce);

		cdc_write_string("Encrypted: ");
		cdc_write_hex_string(encrypted, encryptedLength); cdc_newline();

		int s = cr_decrypt(decrypted, encrypted, encryptedLength, aPublic, bPrivate, nonce);

		cdc_log_int("Decrypt returned: ", s);
		cdc_write_string("Decrypted: ");
		cdc_write_string(decrypted); cdc_newline();


	}

}
