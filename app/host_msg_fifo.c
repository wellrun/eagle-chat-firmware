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
		msg->data[msg->len] = *buf;
		++buf;
		++(msg->len);
	}
}

void hostMsg_addInt(hostMsg_t *msg, uint32_t num, uint8_t base) {
	uint8_t temp[33];
	ultoa(num, temp, base);
	hostMsg_addString(msg, temp);
}
