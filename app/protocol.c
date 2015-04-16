#include "protocol.h"
#include "host_rx.h"
#include "host_tx.h"

const uint8_t REPLY_PREFIX[] = {PROTOCOL_RESPONSE_PREFIX, PROTOCOL_DELIM_CHAR};
const uint8_t RECEIVED_PREFIX[] = {PROTOCOL_RECEIVE_PREFIX, PROTOCOL_DELIM_CHAR};

void protocolReplyOk() {
	hostMsg_t out;
	out.len = 0;

	hostMsg_addBuffer(&out, REPLY_PREFIX, sizeof(REPLY_PREFIX));
	hostMsg_addString(&out, PROTOCOL_RESPONSE_OK);
	hostMsg_terminate(&out);

	host_tx_queueMessage(&out);
}


void protocolReplyFail(uint8_t *reason) {
	hostMsg_t out;
	out.len = 0;

	hostMsg_addBuffer(&out, REPLY_PREFIX, sizeof(REPLY_PREFIX));
	hostMsg_addString(&out, PROTOCOL_RESPONSE_FAIL);

	if (reason[0] != 0) { // if there is a reason to append
		hostMsg_addByte(&out, PROTOCOL_DELIM_CHAR);
		hostMsg_addString(&out, reason);
	}

	hostMsg_terminate(&out);

	host_tx_queueMessage(&out);
}


void protocolAddReplyPrefix(hostMsg_t *msg) {

	hostMsg_addBuffer(msg, REPLY_PREFIX, sizeof(REPLY_PREFIX));

}

void protocolAddReceivePrefix(uint8_t source, hostMsg_t *msg) {

	hostMsg_addBuffer(msg, RECEIVED_PREFIX, sizeof(RECEIVED_PREFIX));
	hostMsg_addHex(msg, source);
	hostMsg_addByte(msg, PROTOCOL_DELIM_CHAR);

}