#include <string.h>
#include <stdint.h>

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
