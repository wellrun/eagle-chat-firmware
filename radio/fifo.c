#include "fifo.h"
#include <string.h>

void fifo_init(fifo_t *f) {
	f->h = 0;
	f->t = 0;
}

uint8_t fifo_isEmpty(fifo_t *f) {
	return (f->h == f->t);
}

void fifo_write(fifo_t *f, uint8_t *buf, uint8_t len) {
	memcpy(&f->data[f->h], buf, len);
	f->h += FIFO_UNIT_LEN;
	if (f->h == FIFO_SIZE) f->h = 0;
}

void fifo_read(fifo_t *f, uint8_t *buf) {
	memcpy(buf, &f->data[f->t], FIFO_UNIT_LEN);
	f->t += FIFO_UNIT_LEN;
	if (f->t == FIFO_SIZE) f->t = 0;
}