#ifndef HOST_RX_H__
#define HOST_RX_H__

#include <stdbool.h>

#include "host_msg_fifo.h"

void host_rx_init(void);

hostMsg_t * host_rx_peek(void);
void host_rx_skip(void);
bool host_rx_isEmpty(void);
bool host_rx_isFull(void);

#endif // HOST_RX_H__
