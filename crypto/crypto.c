#include "crypto.h"

#include "avrnacl.h"
#include <string.h>

void cr_generate_keypair(uint8_t *pk, uint8_t *sk, uint8_t *random) {
	memcpy(sk, random, crypto_box_SECRETKEYBYTES);
	crypto_scalarmult_base(pk, sk);
}

int cr_encrypt(uint8_t *encrypted, uint8_t *message, uint32_t mlen, uint8_t *sPrivate, uint8_t *rPublic, uint8_t *nonce) {

	uint8_t M = mlen;							// Original message length
	uint8_t P = crypto_box_ZEROBYTES;			// Padding length
	uint8_t T = crypto_box_ZEROBYTES + mlen;	// Total length = M + P

	uint8_t padded[T];
	memset(padded, 0, P);
	memcpy(&padded[P], message, M);

	// padded = [ padding | message ]

	uint8_t holder[T];

	// Perform the encryption
	uint8_t ret = crypto_box(holder, padded, T, nonce, rPublic, sPrivate);

	// holder = [ boxpadding | encrypted message ]

	P = crypto_box_BOXZEROBYTES;
	M = T - crypto_box_BOXZEROBYTES;

	// Extract the encrypted message part from holder
	memcpy(encrypted, &holder[P], M);

	return ret;
}

int cr_decrypt(uint8_t *message, uint8_t *encrypted, uint32_t elen, uint8_t *sPublic, uint8_t *rPrivate, uint8_t *nonce) {

	uint8_t P = crypto_box_BOXZEROBYTES;			// Padding length
	uint8_t M = elen;								// Encrypted message length
	uint8_t T = elen + crypto_box_BOXZEROBYTES;		// Total length = M + P;

	uint8_t padded[T];
	memset(padded, 0, P);
	memcpy(&padded[P], encrypted, M);

	// padded = [boxpadding | encrypted message ]

	uint8_t holder[T];

	// Perform the decryption
	uint8_t r = crypto_box_open(holder, padded, T, nonce, sPublic, rPrivate);


	// holder = [padding | original message]

	P = crypto_box_ZEROBYTES;
	M = T - crypto_box_ZEROBYTES;

	// Extract the original message part from holder
	memcpy(message, &holder[P], M);
	message[M] = 0;

	return r;

}