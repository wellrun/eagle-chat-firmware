#include <string.h>
#include <stdint.h>

#include <stdlib.h>

#include "host_msg_fifo.h"

void hostMsg_fifo_init(hostMsg_fifo_t *f) {
    f->h = 0;
	f->t = 0;
	f->stored = 0;
}

bool hostMsg_fifo_isEmpty(hostMsg_fifo_t *f) {
    return (f->stored == 0);
}

bool hostMsg_fifo_isFull(hostMsg_fifo_t *f) {
    return (f->stored > 0 && f->t == f->h);
}


void hostMsg_fifo_write(hostMsg_fifo_t *f, hostMsg_t * msg) {
    f->data[f->h] = msg->len;

    memcpy(&f->data[f->h + 1], msg->data, msg->len);
	f->h += HOST_MSG_FIFO_UNIT_LEN;
	if (f->h == HOST_MSG_FIFO_SIZE) f->h = 0;
	f->stored++;
}

hostMsg_t * hostMsg_fifo_read(hostMsg_fifo_t *f) {
    hostMsg_t *ret = hostMsg_fifo_peek(f);
    hostMsg_fifo_skip(f);
	return ret;
}

hostMsg_t * hostMsg_fifo_peek(hostMsg_fifo_t *f) {
    return (hostMsg_t *)&f->data[f->t];
}

void hostMsg_fifo_skip(hostMsg_fifo_t *f) {
    f->t += HOST_MSG_FIFO_UNIT_LEN;
	if (f->t == HOST_MSG_FIFO_SIZE) f->t = 0;
	f->stored--;
}

void hostMsg_addString(hostMsg_t *msg, uint8_t *buf) {
	while (msg->len < HOST_MSG_BUF_SIZE - 1 && *buf != 0) {
		hostMsg_addByte(msg, *buf);
		++buf;
	}
}

void hostMsg_addByte(hostMsg_t *msg, uint8_t byte) {
	msg->data[msg->len] = byte;
	++(msg->len);
}

void hostMsg_addBuffer(hostMsg_t *msg, uint8_t *buf, uint8_t len) {
	for (uint8_t i = 0; i < len; ++i) {
		hostMsg_addByte(msg, buf[i]);
	}
}

void hostMsg_addInt(hostMsg_t *msg, uint32_t num, uint8_t base) {
	uint8_t temp[33];
	ultoa(num, temp, base);
	hostMsg_addString(msg, temp);
}

void hostMsg_addHex(hostMsg_t *msg, uint8_t byte) {
	const uint8_t table[] = "0123456789ABCDEF";
	hostMsg_addByte(msg, table[(byte>>4) & 0x0F]);
	hostMsg_addByte(msg, table[byte & 0x0F]);
}

void hostMsg_addHexBuffer(hostMsg_t *msg, uint8_t *buf, uint8_t len) {
	for (uint8_t i = 0; i < len; ++i) {
		hostMsg_addHex(msg, buf[i]);
	}
}

uint8_t hostMsg_hexToByte(uint8_t high, uint8_t low) {

	high = scaleByteFromHex(high);
	low = scaleByteFromHex(low);

	return (high << 4 | low);
}

void hostMsg_hexBufferToBytes(uint8_t *hexbuf, uint8_t len, uint8_t *bytesbuf) {
	
	if (len % 2) // make sure len is a multiple of two
		return;

	for (uint8_t i = 0; i < len/2; ++i) {
		bytesbuf[i] = hostMsg_hexToByte(hexbuf[i*2], hexbuf[i*2 +1]);
	}
}

uint8_t scaleByteFromHex(uint8_t byte) {
	
	if (byte >= '0' && byte <= '9')
		return byte - '0';
	else if (byte >= 'A' && byte <= 'F')
		return byte - 'A' + 0xA;
	else 
		return 0;
}

void hostMsg_terminate(hostMsg_t *msg) {
	hostMsg_addByte(msg, '\n');
}