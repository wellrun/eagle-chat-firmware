#include "asf.h"
#include "cdc.h"
#include <avr/io.h>

#include "randombytes.h"
#include "avrnacl.h"
#include "crypto.h"


#define MAX_MESSAGE_LENGTH 50

uint8_t randomSeed[crypto_box_SECRETKEYBYTES];

uint8_t aPublic[crypto_box_PUBLICKEYBYTES];
uint8_t aPrivate[crypto_box_SECRETKEYBYTES];

uint8_t bPublic[crypto_box_PUBLICKEYBYTES];
uint8_t bPrivate[crypto_box_SECRETKEYBYTES];

uint8_t message[MAX_MESSAGE_LENGTH];
uint8_t encryptedMessage[ENCRYPTED_LENGTH(MAX_MESSAGE_LENGTH)];

uint8_t ssk[crypto_box_BEFORENMBYTES];

uint8_t nonce[crypto_box_NONCEBYTES];

void encrypt() {

	cdc_write_string("First, generate a public and private key for us\n");

	randombytes(randomSeed, crypto_box_SECRETKEYBYTES);
	//getRandom32(randomSeed);
	cr_generate_keypair(aPublic, aPrivate, randomSeed);
	randombytes(randomSeed, crypto_box_SECRETKEYBYTES);
	//getRandom32(randomSeed);
	cr_generate_keypair(bPublic, bPrivate, randomSeed);

	cdc_log_hex_string("A Private: ", aPrivate, crypto_box_SECRETKEYBYTES);
	cdc_log_hex_string("A Public : ", aPublic, crypto_box_PUBLICKEYBYTES);
	cdc_log_hex_string("B Private: ", bPrivate, crypto_box_SECRETKEYBYTES);
	cdc_log_hex_string("B Public : ", bPublic, crypto_box_PUBLICKEYBYTES);


	randombytes(nonce, crypto_box_NONCEBYTES);

	cdc_write_string("Enter a message: ");
	uint32_t actualLength = cdc_read_string(message, MAX_MESSAGE_LENGTH);
	uint32_t encryptedLength = ENCRYPTED_LENGTH(actualLength);
	uint32_t decryptedLength = DECRYPTED_LENGTH(encryptedLength);
	uint8_t encrypted[encryptedLength];
	uint8_t decrypted[decryptedLength];

	cdc_log_string("Message: ", message);
	cdc_log_hex_string("Nonce  : ", nonce, crypto_box_NONCEBYTES);

	cr_get_session_ssk(ssk, aPrivate, bPublic);

	cr_encrypt(encrypted, message, actualLength, ssk, nonce);

	cdc_log_hex_string("Encrypted: ", encrypted, ENCRYPTED_LENGTH(actualLength));

	// int s = cr_decrypt(decrypted, encrypted, encryptedLength, aPublic, bPrivate, nonce);

	// cdc_log_int("Decrypt returned: ", s);
	// cdc_log_string("Decrypted: ", decrypted);

}

void decrypt() {

	cdc_read_string(aPublic, crypto_box_PUBLICKEYBYTES);
	cdc_read_string(bPrivate, crypto_box_SECRETKEYBYTES);

	uint8_t actualLength = cdc_read_string(encryptedMessage, ENCRYPTED_LENGTH(MAX_MESSAGE_LENGTH));
	cdc_read_string(nonce, crypto_box_NONCEBYTES);

	uint8_t decrypted[DECRYPTED_LENGTH(actualLength)];

	cr_get_session_ssk(ssk, bPrivate, aPublic);

	cr_decrypt(decrypted, encryptedMessage, actualLength, ssk, nonce);

	cdc_log_string("Decrypted: ", decrypted);

}

int main (void)
{
	cpu_irq_enable();

	irq_initialize_vectors();

	sysclk_init();

	cdc_start();

	while (1) {
		
		uint8_t ret = udi_cdc_getc();

		if (ret == 'e') {
			encrypt();
		} else if (ret == 'd') {
			decrypt();
		}

	}

}
