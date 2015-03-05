#include "asf.h"
#include "cdc.h"
//#include "eeprom.h"
#include <common_nvm.h>
#include <string.h>

#define BUF_SIZE (1 << 6)

#define EXAMPLE_PAGE 2
#define EXAMPLE_ADDR EXAMPLE_PAGE * EEPROM_PAGE_SIZE

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
	uint8_t read_buf[32];

	status_code_t status = nvm_read(INT_EEPROM, (uint32_t) EXAMPLE_ADDR, (void *)read_buf, 32);

	cdc_log_string("Read back: ", read_buf);
	cdc_log_string("return:success:", read_buf);
}

uint8_t do_write(uint8_t * buf, uint8_t size);
uint8_t do_write(uint8_t * buf, uint8_t size) {
	cdc_log_string("Writing to eeprom: ", (const char *) buf);

	/* Initialize the non volatile memory */
	if (nvm_init(INT_EEPROM) != STATUS_OK) {
		return ERR_INVALID_ARG;
	}

	/* Write test pattern to the specified address */
	uint8_t status = nvm_write(INT_EEPROM, (uint32_t) EXAMPLE_ADDR, buf, size);
	if (status == ERR_INVALID_ARG) {
		cdc_write_line("nvm_write failed");
	}
	return STATUS_OK;
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
	
	cdc_write_line("Beginning demo.");
	while (1) {
		
		if (local_msg.msg_recvd) {
			//cdc_write_line("Message received!");
			uint8_t msg_copy[BUF_SIZE];
			uint8_t msg_count = local_msg.ctr;
			memcpy(msg_copy, local_msg.buf, local_msg.ctr+1); // make a copy of the msg contents, including null terminator
			reset_message(&local_msg);
			if (msg_copy[0] == 'R') {
				cdc_write_line("Doing read");
				do_read();
			} else if (msg_copy[0] == 'W') {
				cdc_write_line("Doing write");
				
				uint8_t status = STATUS_OK;
				status = do_write(local_msg.buf+1, msg_count);
				
				if (status == ERR_INVALID_ARG) {
					cdc_write_line("nvm_init failed");
					cdc_write_line("return:error");
				}
				else if (status == STATUS_OK) {
					cdc_write_line("do_write succeeded!");
					cdc_write_line("return:success");
				}
			}
		}
		/*
		cdc_write_line("Enter message: ");
		uint32_t len = cdc_read_line(buf, 50);
		cdc_write_line("Message was: ");
		cdc_write_line(buf);
		cdc_newline();
		cdc_log_int("Bytes received: ", len);
		cdc_log_int("Max length of buffer is: ", 50);
		*/
		//cdc_write_hex(0xAA);

		//cdc_read_line(buf, 50);

		//delay_s(5);

		//cdc_write_line(buf);

		//while (1) {
		//	udi_cdc_getc();
		//}
	}

}
