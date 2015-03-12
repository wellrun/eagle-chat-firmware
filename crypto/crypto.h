#ifndef CRYPTO_H_INCLUDED
#define CRYPTO_H_INCLUDED

#include "asf.h"

// The length required to hold the encrypted message corresponding to
// a plaintext string with length x
#define ENCRYPTED_LENGTH(x) 	(x + crypto_box_ZEROBYTES - crypto_box_BOXZEROBYTES)

// The length required to hold the plaintext string corresponding to
// an encrypted message with length x
#define DECRYPTED_LENGTH(x) 	(x - crypto_box_ZEROBYTES + 1)

//! Creates a public and private key, given suitably random bits
/*!
  \param pk		The destination of the public key with length crypto_box_PUBLICKEYBYTES
  \param sk 	The destination of the private key with length crypto_box_SECRETKEYBYTES
  \param random	A suitably random string of length crypto_box_SECRETKEYBYTES
*/
void cr_generate_keypair(uint8_t *pk, uint8_t *sk, uint8_t *random);

//! Encrypts a message
/*!
  \param encrypted 	Destination of the encrypted message with length ENCRYPTED_LENGTH(mlen)
  \param message	Plaintext string containing the message to be encrypted
  \param mlen		The length of 'message'
  \param sPrivate	The sender's private key of length crypto_box_SECRETKEYBYTES
  \param rPublic	The receiver's public key of length crypto_box_PUBLICKEYBYTES
  \param nonce		A suitably random string of length crypto_box_NONCEBYTES
*/
int cr_encrypt(uint8_t *encrypted, uint8_t *message, uint32_t mlen, uint8_t *sPrivate, uint8_t *rPublic, uint8_t *nonce);

//! Decrypts a message
/*!
  \param message 	Destination of the decrypted message with length DECRYPTED_LENGTH(elen)
  \param encrypted	Buffer containing the encrypted data to be decrypted
  \param elen		The length of 'encrypted'
  \param sPublic	The sender's public key of length crypto_box_PUBLICKEYBYTES
  \param rPrivate	The receiver's private key of length crypto_box_SECRETKEYBYTES
  \param nonce		The same nonce that was used to encrypt the message of length crypto_box_NONCEBYTES
*/
int cr_decrypt(uint8_t *message, uint8_t *encrypted, uint32_t elen, uint8_t *sPublic, uint8_t *rPrivate, uint8_t *nonce);

#endif /* CRYPTO_H_INCLUDED */