#include "asf.h"
#include "cdc.h"
#include <string.h>

#define BUF_SIZE (1 << 6)

typedef struct {
	uint8_t buf[BUF_SIZE];
	volatile bool msg_recvd;
	uint8_t ctr;
} cdc_message_t;

volatile cdc_message_t *msg;

void update_message(volatile cdc_message_t * msg);
void update_message(volatile cdc_message_t * msg) {
	uint8_t c = udi_cdc_getc();
	if (msg->ctr < BUF_SIZE) {
		if (c == '\r' || c == '\n' ) {
			msg->buf[msg->ctr] = '\0';
			msg->msg_recvd = true;
		} else if (!msg->msg_recvd) {
			msg->buf[msg->ctr] = c;
			++(msg->ctr);
		}
	}
}

void reset_message(volatile cdc_message_t * msg);
void reset_message(volatile cdc_message_t * msg) {
	msg->ctr = 0;
	msg->msg_recvd = false;
}

void rx_callback(void);
void rx_callback(void) {
	update_message(msg);
}

void do_read(void);
void do_read() {
	cdc_write_line("Sentinel");
}

void do_write(void);
void do_write(uint8_t * buf) {
	cdc_write_line("Sentinel");
}

int main (void)
{
	cpu_irq_enable();

	irq_initialize_vectors();

	sysclk_init();

	cdc_start();
	
	volatile cdc_message_t local_msg;
	reset_message(&local_msg);
	msg = &local_msg;
	
	
	while (udi_cdc_getc() == 0);
	cdc_set_rx_callback(&rx_callback);
	
	cdc_write_string("Beginning demo.");
	uint8_t ctr = 0;
	while (1) {
		
		if (local_msg.msg_recvd == 1) {
			cdc_write_string("Message received!");
			uint8_t msg_copy[BUF_SIZE];
			memcpy(msg_copy, local_msg.buf, local_msg.ctr+1); // make a copy of the msg contents, including null terminator
			reset_message(&local_msg);
			ctr=0;
			if (msg_copy[0] == 'R') {
				do_read();
			} else if msg_copy[0] == 'W' {
				do_write(local_msg.buf+1);
			}
		}
		/*
		cdc_write_string("Enter message: ");
		uint32_t len = cdc_read_string(buf, 50);
		cdc_write_string("Message was: ");
		cdc_write_string(buf);
		cdc_newline();
		cdc_log_int("Bytes received: ", len);
		cdc_log_int("Max length of buffer is: ", 50);
		*/
		//cdc_write_hex(0xAA);

		//cdc_read_string(buf, 50);

		//delay_s(5);

		//cdc_write_string(buf);

		//while (1) {
		//	udi_cdc_getc();
		//}
	}

}
