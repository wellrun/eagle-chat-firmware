#define F_CPU   32000000L

#include <string.h>

#include <avr/io.h>

#include "asf.h"
#include "cdc.h"
#include "system_timer.h"
#include "host_rx.h"
#include "host_tx.h"
#include "routing/routing.h"
#include "stack/stack.h"

#include <util/atomic.h>

#include "crypto/crypto.h"
#include "keys/keys.h"
#include "sha204/sha204.h"

#include "protocol.h"

volatile bool quit = false;
uint8_t my_address = 0;

// The destination's public key. JUST FOR THIS DEMO
uint8_t dest_pubKey[32];

uint8_t shared_secret[32]; // also 32 bytes?

void do_routing(void);
static volatile bool busy = false;
void do_routing(void) {

	if (busy) {
		return;
	}
	busy = true;
	handleReceived();
	processSendQueue();
	busy = false;
}


#define SUCCESS			0
#define FAILURE_NOKEY	1

uint8_t decryptMessageFrom(uint8_t *decrypted, uint8_t *decryptedLength, uint8_t addr, uint8_t *payload, uint8_t len);
uint8_t decryptMessageFrom(uint8_t *decrypted, uint8_t *decryptedLength, uint8_t addr, uint8_t *payload, uint8_t len) {

	uint8_t *nonce = &payload[0];
	uint8_t *emessage = &payload[crypto_box_NONCEBYTES];

	uint8_t elen = len - crypto_box_NONCEBYTES;

	ssk_load_table();

	uint8_t slot;
	uint8_t ssk[CRYPTO_KEY_SIZE];
	if (ssk_has_key(addr, &slot)) {

		ssk_read_key(slot, ssk);

		cdc_log_hex_string("Decrypt Nonce: ", nonce, crypto_box_NONCEBYTES);
		cdc_log_hex_string("Decrypt SSK: ", ssk, CRYPTO_KEY_SIZE);

		cr_decrypt(decrypted, emessage, elen, ssk, nonce);

	} else {

		return FAILURE_NOKEY;

	}

	*decryptedLength = DECRYPTED_LENGTH(elen);
	cdc_log_int("Encrypted length: ", elen);
	cdc_log_int("ZEROBYTES: ", crypto_box_ZEROBYTES);
	cdc_log_int("Decrypted length: ", DECRYPTED_LENGTH(elen));

	return SUCCESS;

}

uint8_t sendPublicKeyUpdate(uint8_t addr);
uint8_t sendPublicKeyUpdate(uint8_t addr) {

	PacketHeader h;

	h.source = my_address;
	h.dest = addr;
	h.type = PACKET_TYPE_PUBKEY;

	load_public_key();

	queuePacket(h, get_public_key(), CRYPTO_KEY_SIZE);

}

uint8_t sendEncryptedTo(uint8_t addr, uint8_t *message, uint8_t len);
uint8_t sendEncryptedTo(uint8_t addr, uint8_t *message, uint8_t len) {

	// build a packet and queue it up
	PacketHeader h;

	h.source = my_address;
	h.dest = addr;
	h.type = PACKET_TYPE_CONTENT;

	uint8_t encrypted[ENCRYPTED_LENGTH(len)];

	// Make a larger payload to hold nonce and encrypted message
	uint8_t payload[MAX_PAYLOAD_SIZE];

	// Get a nonce from the SHA chip
	uint8_t nonce[32];
	sha204_getRandom32(nonce);

	ssk_load_table();

	uint8_t slot;
	uint8_t ssk[CRYPTO_KEY_SIZE];
	if (ssk_has_key(addr, &slot)) {

		ssk_read_key(slot, ssk);

		cr_encrypt(encrypted, message, len, ssk, nonce);

	} else {

		return FAILURE_NOKEY;

	}

	cdc_log_hex_string("Encrypt Nonce: ", nonce, crypto_box_NONCEBYTES);
	cdc_log_hex_string("Encrypt SSK: ", ssk, CRYPTO_KEY_SIZE);

	memcpy(payload, nonce, crypto_box_NONCEBYTES);
	memcpy(&payload[crypto_box_NONCEBYTES], encrypted, ENCRYPTED_LENGTH(len));

	queuePacket(h, payload, len + CRYPTO_OVERHEAD_TOTAL);

	return SUCCESS;

}

void processGenKeys(void);
void processGenKeys() {
	// expects nothing

	uint8_t random[32];
	sha204_getRandom32(random);

	uint8_t public[CRYPTO_KEY_SIZE];
	uint8_t private[CRYPTO_KEY_SIZE];

	cr_generate_keypair(public, private, random);

	store_public_key(public);
	store_private_key(private);

	cdc_write_line("Successfully generated keys");

}

void processPublicKeyUpdate(uint8_t *data);
void processPublicKeyUpdate(uint8_t *data) {
	// expects (address):(message string)

	// try to grab the address portion
	uint8_t addr = 0;
	char * token;
	token = strtok(data, PROTOCOL_DELIM);
	if (token != NULL)
		addr = (uint8_t) atoi(token);
	if (addr == 0) {
		cdc_write_line("INVALID: NO ADDRESS");
		return; // invalid host message, invalid address
	}

	sendPublicKeyUpdate(addr);

	cdc_log_int("Sent public key update to: ", addr);

}

void processSendMessage(uint8_t *data);
void processSendMessage(uint8_t *data) {
	// expects (address):(message string)

	// try to grab the address portion
	uint8_t addr = 0;
	char * token;
	token = strtok(data, PROTOCOL_DELIM);
	if (token != NULL)
		addr = (uint8_t) atoi(token);
	if (addr == 0) {
		cdc_write_line("INVALID: NO ADDRESS");
		return; // invalid host message, invalid address
	}

	// grab the message portion
	token = strtok(NULL, PROTOCOL_DELIM);
	if (token == NULL) {
		cdc_write_line("INVALID: NO CONTENT");
		return; // invalid host message, no message content
	}

	uint8_t result = sendEncryptedTo(addr, token, strlen(token));
	if (result == SUCCESS) {
		cdc_write_line("Sent message successfully");
	} else if (result == FAILURE_NOKEY) {
		cdc_write_line("No public key entry for that node");
	}

}


void saveSSKFor(uint8_t addr, uint8_t *public);
void saveSSKFor(uint8_t addr, uint8_t *public) {

	ssk_load_table();
	load_private_key();
	load_public_key();

	uint8_t ssk[CRYPTO_KEY_SIZE];

	cdc_log_hex_string("My private: ", get_private_key(), CRYPTO_KEY_SIZE);
	cdc_log_hex_string("My public: ", get_public_key(), CRYPTO_KEY_SIZE);
	cdc_log_hex_string("His public: ", public, CRYPTO_KEY_SIZE);

	cr_get_session_ssk(ssk, get_private_key(), public);
	
	cdc_log_hex_string("SSK: ", ssk, CRYPTO_KEY_SIZE);

	ssk_store_key(addr, ssk);

	ssk_store_table();

	cdc_log_int("Calculated and stored ssk for: ", addr);

}

// Given a node's public key,
void processPublicKey(uint8_t *data);
void processPublicKey(uint8_t *data) {
	// expects (address):(public key)

	uint8_t addr = 0;
	char * token;

	token = strtok(data, PROTOCOL_DELIM);

	if (token != NULL)
		addr = (uint8_t) atoi(token);
	if (addr == 0) {
		cdc_write_line("INVALID: NO ADDRESS");
		return; // invalid host message, invalid address
	}

	// grab the message portion
	token = strtok(NULL, PROTOCOL_DELIM);
	if (token == NULL) {
		cdc_write_line("INVALID: NO CONTENT");
		return; // invalid host message, no message content
	}

	// token now holds the public key

	// We now store the appropriate shared secret to communicate with this partner.

	saveSSKFor(addr, token);

}

void processIncomingProtocol(void);
void processIncomingProtocol() {
	static uint16_t msg_count = 0;

	// This is where we would parse host messages and take actions accordingly
	// Right now we just echo the messages
	if (!host_rx_isEmpty()) {

		msg_count++;

		hostMsg_t * msg = host_rx_peek();

		if ((*msg).len > 0) { // check for a valid length message
			uint8_t type = (*msg).data[0];
			switch(type) {
				case PROTOCOL_TOKEN_SEND:
					processSendMessage((*msg).data + 1); // strip the first char
					break;
				case PROTOCOL_TOKEN_KEY: // Host wants to send us a node's dest_pubKey
					processPublicKey((*msg).data + 1); // strip the first char
					break;
				case PROTOCOL_TOKEN_PUBKEY_UPDATE:
					processPublicKeyUpdate((*msg).data + 1);
					break;
				case PROTOCOL_TOKEN_GENKEYS:
					processGenKeys();
					break;
			}
		}

		/*
		hostMsg_t out;
		out.len = 0;

		hostMsg_addString(&out, "msg count: ");
		hostMsg_addInt(&out, msg_count, 10);
		hostMsg_addString(&out, "\ndata ===\n");
		hostMsg_addString(&out, msg->data);
		hostMsg_addString(&out, "\n\n");

		host_rx_skip();

		host_tx_queueMessage(&out);
		*/

		///*

		host_rx_skip();
		//*/
	}
}

void init(void) {
	uint8_t inputbuf[4];
	while (1) {
		cdc_write_string("My address (1-3): ");
		cdc_read_string(inputbuf, 3);
		my_address = (uint8_t)atoi((const char *)inputbuf);
		cdc_newline();

		if (my_address >= 1 && my_address <= 3) {
			cdc_log_int("You said: ", my_address);
			break;
		}

		cdc_write_line("Invalid address.");
	}
}


int main(void);
int main()
{

	// System initialization

	pmic_init();
	pmic_set_scheduling(PMIC_SCH_ROUND_ROBIN);

	cpu_irq_enable();
	irq_initialize_vectors();

	sysclk_init();

	rtc_init();

	cdc_start();

	sha204_init();

	while (!cdc_opened());
	rtc_set_time(0);
	cdc_log_int("Starting app ", rtc_get_time());

	init();

	host_rx_init(); // Setup host rx module
	host_tx_init();

	setupRouting(my_address);

	system_timer_init();
	system_timer_set_callback(do_routing);
	system_timer_start();

	// This is part of the initialization process, but we fake it here for now.
	ssk_reset_table();


	while(1) {
		host_tx_processQueue();
		processIncomingProtocol();

		uint8_t decrypted[MAX_PAYLOAD_SIZE];
		uint8_t decryptedLength;

		if (packetsToRead()) {
			PacketHeader header;
			uint8_t *payload;
			uint8_t length = packetReceivedPeek(&header, &payload);
			cdc_log_int("From: ", header.source);

			if (header.type == PACKET_TYPE_CONTENT) {

				uint8_t result = decryptMessageFrom(decrypted, &decryptedLength, header.source, payload, length);
				cdc_log_int("Decrypted length: ", decryptedLength);

				if (result == SUCCESS) {
					decrypted[decryptedLength] = 0;
					cdc_log_string("Message: ", decrypted);
				} else if (result == FAILURE_NOKEY) {
					cdc_write_line("Couldn't decrypt because no public key for sender");
				}

			} else if (header.type == PACKET_TYPE_PUBKEY) {

				ssk_load_table();

				saveSSKFor(header.source, payload);

				cdc_log_int("successfully stored public key for: ", header.source);

			}

			packetReceivedSkip();
			cdc_newline();
		}
	}

	while (1);
}
