#include "crypto.h"

#include "avrnacl.h"
#include <string.h>

void cr_generate_keypair(uint8_t *pk, uint8_t *sk, uint8_t *random) {
	memcpy(sk, random, crypto_box_SECRETKEYBYTES);
	crypto_scalarmult_base(pk, sk);
}

int cr_encrypt(uint8_t *encrypted, uint8_t *message, uint32_t mlen, uint8_t *sPrivate, uint8_t *rPublic, uint8_t *nonce) {

	uint8_t padded[crypto_box_ZEROBYTES + mlen];
	memset(padded, 0, crypto_box_ZEROBYTES);
	memcpy(&padded[crypto_box_ZEROBYTES], message, mlen);

	uint8_t holder[crypto_box_ZEROBYTES + mlen];

	uint8_t ret = crypto_box(holder, padded, crypto_box_ZEROBYTES + mlen, nonce, rPublic, sPrivate);

	memcpy(encrypted, &holder[crypto_box_BOXZEROBYTES], crypto_box_ZEROBYTES + mlen - crypto_box_BOXZEROBYTES);

	return ret;
}

int cr_decrypt(uint8_t *message, uint8_t *encrypted, uint32_t elen, uint8_t *sPublic, uint8_t *rPrivate, uint8_t *nonce) {

	uint8_t holder[elen + crypto_box_BOXZEROBYTES];

	uint8_t padded[elen + crypto_box_BOXZEROBYTES];
	memset(padded, 0, crypto_box_BOXZEROBYTES);
	memcpy(&padded[crypto_box_BOXZEROBYTES], encrypted, elen);

	uint8_t r = crypto_box_open(holder, padded, elen + crypto_box_BOXZEROBYTES, nonce, sPublic, rPrivate);

	memcpy(message, &holder[crypto_box_ZEROBYTES], elen + crypto_box_BOXZEROBYTES - crypto_box_ZEROBYTES);
	message[elen + crypto_box_BOXZEROBYTES - crypto_box_ZEROBYTES] = 0;

	return r;

}