#ifndef HOST_TX_H__
#define HOST_TX_H__

#include <stdbool.h>

#include "host_msg_fifo.h"

void host_tx_init(void);
bool host_tx_isEmpty(void);
bool host_tx_isFull(void);
void host_tx_queueMessage(hostMsg_t *msg);
void host_tx_processQueue(void);

#endif // HOST_TX_H__
