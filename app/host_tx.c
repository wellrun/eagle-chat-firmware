#include <string.h>
#include <stdint.h>

#include "cdc.h"

#include "host_tx.h"

static hostMsg_fifo_t tx_fifo;

hostMsg_t * host_tx_peek(void);
void host_tx_skip(void);

void host_tx_init() {
    hostMsg_fifo_init(&tx_fifo);
}

hostMsg_t * host_tx_peek() {
    return hostMsg_fifo_peek(&tx_fifo);
}

void host_tx_skip() {
    hostMsg_fifo_skip(&tx_fifo);
}

bool host_tx_isEmpty() {
    return hostMsg_fifo_isEmpty(&tx_fifo);
}

bool host_tx_isFull() {
    return hostMsg_fifo_isFull(&tx_fifo);
}

void host_tx_queueMessage(hostMsg_t *msg) {
	hostMsg_fifo_write(&tx_fifo, msg);
}


void host_tx_processQueue() {
	while(!host_tx_isEmpty()) {
		hostMsg_t *msg = host_tx_peek();
		cdc_write_buffer(msg->data, msg->len);
		host_tx_skip();
	}
}
