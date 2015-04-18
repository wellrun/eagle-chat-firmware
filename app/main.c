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
#include "ccpwrite.h"

#include <util/atomic.h>

#include "crypto/crypto.h"
#include "keys/keys.h"
#include "sha204/sha204.h"

#include "protocol.h"


#define AUTH			(authenticated)
#define CONFIG			(device_configured())
#define FULLREADY		(AUTH && CONFIG)
#define BREAK_IF_NOAUTH		if (!AUTH) { protocolReplyFail("Not authenticated"); break; }
#define BREAK_IF_NOCONFIG	if (!CONFIG) { protocolReplyFail("Not configured"); break; }
#define BREAK_IF_NOTREADY	if (!FULLREADY) { protocolReplyFail("Not authenticated or not configured"); break; }

volatile bool quit = false;
uint8_t my_address = 0;


bool authenticated;

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


#define SUCCESS					0
#define FAILURE_NOKEY			1
#define FAILURE_CANT_DECRYPT	2

uint8_t decryptMessageFrom(uint8_t *decrypted, uint8_t *decryptedLength, uint8_t addr, uint8_t *payload, uint8_t len);
uint8_t decryptMessageFrom(uint8_t *decrypted, uint8_t *decryptedLength, uint8_t addr, uint8_t *payload, uint8_t len) {

	uint8_t *nonce = &payload[0];
	uint8_t *emessage = &payload[crypto_box_NONCEBYTES];

	uint8_t elen = len - crypto_box_NONCEBYTES;

	uint8_t slot;
	uint8_t ssk[CRYPTO_KEY_SIZE];
	if (ssk_has_key(addr, &slot)) {

		ssk_read_key(slot, ssk);

		if (cr_decrypt(decrypted, emessage, elen, ssk, nonce) != 0) {
			return FAILURE_CANT_DECRYPT;
		}

	} else {

		return FAILURE_NOKEY;

	}

	*decryptedLength = DECRYPTED_LENGTH(elen);

	return SUCCESS;

}

uint8_t sendPublicKeyUpdate(uint8_t addr);
uint8_t sendPublicKeyUpdate(uint8_t addr) {

	PacketHeader h;

	h.source = my_address;
	h.dest = addr;
	h.type = PACKET_TYPE_PUBKEY;

	return queuePacket(h, get_public_key(), CRYPTO_KEY_SIZE);

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

	uint8_t slot;
	uint8_t ssk[CRYPTO_KEY_SIZE];
	if (ssk_has_key(addr, &slot)) {

		ssk_read_key(slot, ssk);

		cr_encrypt(encrypted, message, len, ssk, nonce);

	} else {

		return FAILURE_NOKEY;

	}

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

	set_public_key(public);
	set_private_key(private);

	protocolReplyOk();

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
		return; // invalid host message, invalid address
	}

	sendPublicKeyUpdate(addr);

	protocolReplyOk();

}


void returnStatus();
void returnStatus() {

	const setup_status_t *status = get_setup_status();

	hostMsg_t out;
	out.len = 0;

	uint8_t response[] = {PROTOCOL_RESPONSE_PREFIX, PROTOCOL_DELIM_CHAR};

	hostMsg_addBuffer(&out, response, sizeof(response));
	hostMsg_addHex(&out, ~(status->flags));
	hostMsg_terminate(&out);

	host_tx_queueMessage(&out);
}

void returnNodeId();
void returnNodeId() {
	const setup_status_t *status = get_setup_status();

	hostMsg_t out;
	out.len = 0;

	protocolAddReplyPrefix(&out);

	hostMsg_addHex(&out, status->node_id);

	hostMsg_terminate(&out);

	host_tx_queueMessage(&out);
}

void returnPublicKey();
void returnPublicKey() {

	hostMsg_t out;
	out.len = 0;

	protocolAddReplyPrefix(&out);
	hostMsg_addHexBuffer(&out, get_public_key(), CRYPTO_KEY_SIZE);
	hostMsg_terminate(&out);

	host_tx_queueMessage(&out);
}

void returnReceivedMessage(uint8_t source, uint8_t *data, uint8_t len) {
	hostMsg_t out;
	out.len = 0;

	protocolAddReceivePrefix(source, &out);
	hostMsg_addBuffer(&out, data, len);
	hostMsg_terminate(&out);

	host_tx_queueMessage(&out);
}


void processGet(uint8_t *data);
void processGet(uint8_t *data) {
	// expects (data)

	char * token;
	token = strtok(data, PROTOCOL_DELIM);

	uint8_t attr = 0;	
	attr = token[0];

	switch (attr) {
		case PROTOCOL_TOKEN_GET_KEY: // send host our public key
			BREAK_IF_NOTREADY;
			returnPublicKey();
			break;
		case PROTOCOL_TOKEN_GET_ID: // send host our id
			BREAK_IF_NOTREADY;
			returnNodeId();
			break;
		case PROTOCOL_TOKEN_GET_STATUS: // send host our status
			returnStatus();
			break;
	}

}

void processSetID(uint8_t *data);
void processSetID(uint8_t *data) {
	// expects (node_id)

	// try to grab the address portion
	uint8_t node_id = 0;
	char * token;

	token = strtok(data, PROTOCOL_DELIM);

	if (token != NULL) {

		node_id = hostMsg_hexToByte(token[0], token[1]); // decode our address

		if (node_id != 0xFF) {
		
			set_node_id(node_id); // store new id
			setRoutingId(node_id); // start routing as new id
			my_address = node_id;
			protocolReplyOk(); // notify success

		} else {

			protocolReplyFail("Node Id must be < 255"); // notify failure

		}
	}

}

void processSendMessage(uint8_t *data);
void processSendMessage(uint8_t *data) {
	// expects (address):(message string)

	// can't send if our address is 0
	if (my_address == 0) {
		protocolReplyFail("ID NOT CONFIGURED");
		return;
	}

	// try to grab the address portion
	uint8_t addr = 0;
	char * token;
	token = strtok(data, PROTOCOL_DELIM);
	if (token != NULL)
		addr = (uint8_t) atoi(token);
	if (addr == 0) {
		protocolReplyFail("INVALID: NO ADDRESS");
		return; // invalid host message, invalid address
	}

	// grab the message portion
	token = strtok(NULL, PROTOCOL_DELIM);
	if (token == NULL) {
		protocolReplyFail("INVALID: NO CONTENT");
		return; // invalid host message, no message content
	}

	uint8_t result = sendEncryptedTo(addr, token, strlen(token));
	if (result == SUCCESS) {
		protocolReplyOk();
	} else if (result == FAILURE_NOKEY) {
		protocolReplyFail("No public key entry for that node");
	} else {
		protocolReplyFail("Could not send message");
	}

}


void saveSSKFor(uint8_t addr, uint8_t *public);
void saveSSKFor(uint8_t addr, uint8_t *public) {

	uint8_t ssk[CRYPTO_KEY_SIZE];

	cr_get_session_ssk(ssk, get_private_key(), public);

	ssk_set_key(addr, ssk);

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
		return; // invalid host message, invalid address
	}

	// grab the message portion
	token = strtok(NULL, PROTOCOL_DELIM);
	if (token == NULL) {
		return; // invalid host message, no message content
	}

	uint8_t decoded_key[CRYPTO_KEY_SIZE];

	hostMsg_hexBufferToBytes(token, CRYPTO_KEY_SIZE*2, decoded_key); // Should have the decoded key now

	// token now holds the public key

	// We now store the appropriate shared secret to communicate with this partner.

	saveSSKFor(addr, decoded_key);

	protocolReplyOk();

}

void processSetPassword(uint8_t *pdata);
void processSetPassword(uint8_t *pdata) {

	uint8_t hash[30];

	hostMsg_hexBufferToBytes(pdata, 60, hash);

	set_password(hash);
	protocolReplyOk();
}

void processAuth(uint8_t *pdata);
void processAuth(uint8_t *pdata) {
	setup_status_t *s = get_setup_status();

	uint8_t hash[30];

	hostMsg_hexBufferToBytes(pdata, 60, hash);

	authenticated = (memcmp(hash, s->password, 30) == 0);
	if (authenticated)
		protocolReplyOk();
	else
		protocolReplyFail("PASSWORD INCORRECT");
}

void processCommitConfig() {
	if (all_components_configured()) {
		set_configured();
		protocolReplyOk();
	} else {
		protocolReplyFail("All components not configured");
	}
} 

void processDebug(uint8_t *data);
void processDebug(uint8_t *data) {
	// expects (debugcommand)

	char * token;
	token = strtok(data, PROTOCOL_DELIM);

	uint8_t attr = 0;	
	attr = token[0];

	switch (attr) {
		case PROTOCOL_TOKEN_DEBUG_RTABLE:
			debugPrintRoutingTable();
			break;
	}
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

			// Check for burn
			if (strcmp(msg->data, PROTOCOL_STRING_BURN) == 0) {
				cpu_irq_disable();
				burn_memory();
				CCPWrite( &RST.CTRL, RST_SWRST_bm );
				return;
			}

			uint8_t type = msg->data[0];



			switch(type) {
				case PROTOCOL_TOKEN_SEND:
					BREAK_IF_NOTREADY;
					processSendMessage(msg->data + 2); // strip 'x:'
					break;
				case PROTOCOL_TOKEN_SET_KEY: // Host wants to send us a node's dest_pubKey
					BREAK_IF_NOTREADY;
					processPublicKey(msg->data + 2);
					break;
				case PROTOCOL_TOKEN_PUBKEY_UPDATE:
					BREAK_IF_NOTREADY;
					processPublicKeyUpdate(msg->data + 2);
					break;
				case PROTOCOL_TOKEN_GENKEYS:
					processGenKeys();
					break;
				case PROTOCOL_TOKEN_SET_ID:
					processSetID(msg->data + 2);
					break;
				case PROTOCOL_TOKEN_SET_PASSWORD:
					processSetPassword(msg->data + 2);
					break;
				case PROTOCOL_TOKEN_GET:
					processGet(msg->data+2);
					break;
				case PROTOCOL_TOKEN_AUTH:
					BREAK_IF_NOCONFIG;
					processAuth(msg->data + 2);
					break;
				case PROTOCOL_TOKEN_COMMIT_CONFIG:
					processCommitConfig();
					break;
				case PROTOCOL_TOKEN_DEBUG:
					processDebug(msg->data + 2);
					break;

			}
		}

		host_rx_skip();
		
	}
}

// Returns the configured node id
uint8_t checkSetup();
uint8_t checkSetup() {
	// Read EEPROM
	load_private_key();
	load_public_key();
	load_setup_status();


	if (!device_configured())
		ssk_reset_table();
	ssk_load_table();

	

	const setup_status_t *status = get_setup_status();
	if (status->node_id == FLAGS_UNSET) {
		set_node_id(0);
		return 0;
	} else {
		return status->node_id;
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

	host_rx_init(); // Setup host rx module
	host_tx_init();

	my_address = checkSetup();

	setupRouting(my_address);

	system_timer_init();
	system_timer_set_callback(do_routing);
	system_timer_start();

	while (!cdc_opened());
	rtc_set_time(0);
	cdc_log_int("Starting app ", rtc_get_time());

	authenticated = false;

	while(1) {
		host_tx_processQueue();
		processIncomingProtocol();

		uint8_t decrypted[MAX_PAYLOAD_SIZE];
		uint8_t decryptedLength;

		if (packetsToRead()) {
			PacketHeader header;
			uint8_t *payload;
			uint8_t length = packetReceivedPeek(&header, &payload);

			if (header.type == PACKET_TYPE_CONTENT) {

				uint8_t result = decryptMessageFrom(decrypted, &decryptedLength, header.source, payload, length);

				if (result == SUCCESS) {
					returnReceivedMessage(header.source, decrypted, decryptedLength);
				} else if (result == FAILURE_NOKEY) {
					protocolReplyFail("Could not decrypt received message. No key.");
				} else {
					protocolReplyFail("Decryption failed.");
				}

			} else if (header.type == PACKET_TYPE_PUBKEY) {

				saveSSKFor(header.source, payload);

			}

			packetReceivedSkip();
		}
	}

	while (1);
}
