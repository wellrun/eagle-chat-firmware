#include "packet_fifo.h"
#include <string.h>

void packet_fifo_init(packet_fifo_t *f) {
	f->h = 0;
	f->t = 0;
	f->stored = 0;
}

uint8_t packet_fifo_isEmpty(packet_fifo_t *f) {
	return (f->stored == 0);
}

uint8_t packet_fifo_isFull(packet_fifo_t *f) {
	return (f->stored > 0 && f->t == f->h);
}

void packet_fifo_write(packet_fifo_t *f, PacketHeader h, uint8_t *payload, uint8_t payloadLen) {

    f->data[f->h] = payloadLen; // Store payloadLen as first byte in head

    // Store PacketHeader next
	memcpy(&f->data[f->h + 1], &h, sizeof(PacketHeader));

    // Store payload in remaining space
    memcpy(&f->data[f->h + 1 + sizeof(PacketHeader)], payload, payloadLen);

	f->h += PACKET_FIFO_UNIT_LEN;
	if (f->h == PACKET_FIFO_SIZE) f->h = 0;
	f->stored++;
}

uint8_t packet_fifo_read(packet_fifo_t *f, PacketHeader *h, uint8_t **payload) {

    uint8_t payloadLen = f->data[f->t];

    *h = *((PacketHeader *)&f->data[f->t + 1]); // Unpack PacketHeader

    *payload = &f->data[f->t + 1 + sizeof(PacketHeader)];

	packet_fifo_skip(f);

	return payloadLen;
}

PacketHeader packet_fifo_peekHeader(packet_fifo_t *f) {
    return *((PacketHeader *)&f->data[f->t + 1]);
}

/*
uint8_t packet_fifo_peek(packet_fifo_t *f, uint16_t offset) {
	return f->data[(f->t)*PACKET_FIFO_UNIT_LEN + offset];
}
*/

void packet_fifo_skip(packet_fifo_t *f) {
	f->t += PACKET_FIFO_UNIT_LEN;
	if (f->t == PACKET_FIFO_SIZE) f->t = 0;
	f->stored--;
}
