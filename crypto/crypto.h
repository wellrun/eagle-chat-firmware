#ifndef CRYPTO_H_INCLUDED
#define CRYPTO_H_INCLUDED

#include "asf.h"

// The length required to hold the encrypted message corresponding to
// a plaintext string with length x
#define ENCRYPTED_LENGTH(x) 	(x + crypto_box_ZEROBYTES - crypto_box_BOXZEROBYTES)	// 	x + 16

// The length required to hold the plaintext string corresponding to
// an encrypted message with length x
#define DECRYPTED_LENGTH(x) 	(x - crypto_box_ZEROBYTES)

#define CRYPTO_OVERHEAD_TOTAL	((crypto_box_ZEROBYTES - crypto_box_BOXZEROBYTES) + crypto_box_NONCEBYTES) // (16) + 24 = 40

//! Creates a public and private key, given suitably random bits
/*!
  \param pk		The destination of the public key with length crypto_box_PUBLICKEYBYTES
  \param sk 	The destination of the private key with length crypto_box_SECRETKEYBYTES
  \param random	A suitably random string of length crypto_box_SECRETKEYBYTES
*/
void cr_generate_keypair(uint8_t *public, uint8_t *private, uint8_t *random);

//! Computes a shared secret key that allows two partners to maintain an open line of communication.
//! Before encryption, the sender should call this function with its own private key and the intended destination's public key
//! 	and if desired, store it and associate it with this particular destination.
//! Before decryption, the receiver should call this function with its own private key and the supposed sender's public key
//! 	and if desired, store it and associate it with this particular sender
//! These same two partners may use this pre-computed SSK for any subsequent communication between them.
/*!
  \param ssk 		Destination of the shared secret key associated with communication between this pair of length crypto_box_BEFORENMBYTES
  \param private	Client's private key of length crypto_box_SECRETKEYBYTES
  \param public		The public key of the other partner in this communication session of length crypto_box_PUBLICKEYBYTES
*/
int cr_get_session_ssk(uint8_t *ssk, uint8_t *private, uint8_t *public);

//! Encrypts a message
/*!
  \param encrypted 		Destination of the encrypted message with length ENCRYPTED_LENGTH(mlen)
  \param message		Plaintext string containing the message to be encrypted
  \param mlen			The length of 'message'
  \param session_ssk 	The shared secret key corresponding to the communication session between a particular sender and receiver of length crypto_box_BEFORENMBYTES
  \param nonce			A suitably random string of length crypto_box_NONCEBYTES
*/
int cr_encrypt(uint8_t *encrypted, uint8_t *message, uint32_t mlen, uint8_t *session_ssk, uint8_t *nonce);

//! Decrypts a message
/*!
  \param message 		Destination of the decrypted message with length DECRYPTED_LENGTH(elen)
  \param encrypted		Buffer containing the encrypted data to be decrypted
  \param elen			The length of 'encrypted'
  \param session_ssk 	The shared secret key corresponding to the communication session between a particular sender and receiver of length crypto_box_BEFORENMBYTES
  \param nonce			The same nonce that was used to encrypt the message of length crypto_box_NONCEBYTES
*/
int cr_decrypt(uint8_t *message, uint8_t *encrypted, uint32_t elen, uint8_t *session_ssk, uint8_t *nonce);

#endif /* CRYPTO_H_INCLUDED */