#include <string.h>
#include <stdint.h>

#include <util/atomic.h>

#include "cdc.h"

#include "host_rx.h"

static hostMsg_fifo_t rx_fifo;

hostMsg_t msg;

volatile bool msg_recvd = false;

bool gotToLine30 = false;

bool update_message(hostMsg_t * msg);
bool update_message(hostMsg_t * msg) {
	iram_size_t nb = udi_cdc_get_nb_received_data();
    //cdc_log_int("nb: ", nb);
    if (nb > 0) {
    	if ((msg->len + nb - 1) < HOST_MSG_BUF_SIZE) {
    		udi_cdc_read_buf((void *)(msg->data + msg->len), nb); // read into buffer
    		msg->len += nb; // Update counter
    		if (msg->len >= HOST_MSG_BUF_SIZE-1) {
    				msg->data[HOST_MSG_BUF_SIZE - 1] = '\0';
    				msg->len = HOST_MSG_BUF_SIZE - 1;
    				msg_recvd = true;
    				return false;
    		} else if (msg->data[msg->len-1] == '\r' || msg->data[msg->len-1] == '\n') {
    			msg->data[msg->len-1] = '\0';
    			msg_recvd = true;
    			return false;
    		}
    	} else {
    		iram_size_t num_to_read = HOST_MSG_BUF_SIZE - msg->len - 1; // Number of bytes we can read
            num_to_read = nb < num_to_read ? nb : num_to_read;
            gotToLine30 = true;
            //cdc_log_int("num_to_read: ", num_to_read);
            //cdc_log_int("nb: ", nb);
    		msg->data[HOST_MSG_BUF_SIZE - 1] = '\0';
    		msg->len = HOST_MSG_BUF_SIZE - 1;
    		msg_recvd = true;
    		if (num_to_read <= 0) {
    			return false;
    		}
    		udi_cdc_read_buf((void *)(msg->data + msg->len), num_to_read);
    		return true;
    	}
    }
    return false;
}

void reset_message(hostMsg_t * msg);
void reset_message(hostMsg_t * msg) {
	msg->len = 0;
    msg_recvd = false;
	//memset((void *)msg->data, 0, HOST_MSG_BUF_SIZE);
}

void rx_callback(void);
void rx_callback(void) {
    bool call_again = update_message(&msg);

    if (msg_recvd) {
        if(!host_rx_isFull())
            hostMsg_fifo_write(&rx_fifo, &msg);
        reset_message(&msg);
    }

    uint8_t call_again_count = 0;

    while (call_again //&& call_again_count < 5
    ) {
        ++call_again_count;
    	call_again = update_message(&msg);

        if (msg_recvd) {
            if(!host_rx_isFull())
                hostMsg_fifo_write(&rx_fifo, &msg);
            reset_message(&msg);
        }

    }
}

void host_rx_init() {
    reset_message(&msg);
    hostMsg_fifo_init(&rx_fifo);
    cdc_set_rx_callback(rx_callback);
}

hostMsg_t * host_rx_peek() {
    return hostMsg_fifo_peek(&rx_fifo);
}

void host_rx_skip() {
    hostMsg_fifo_skip(&rx_fifo);
}

bool host_rx_isEmpty() {
    if (gotToLine30) {
        gotToLine30 = false;
        //cdc_write_line("gotToLine30");
    }
    return hostMsg_fifo_isEmpty(&rx_fifo);
}

bool host_rx_isFull() {
    return hostMsg_fifo_isFull(&rx_fifo);
}
