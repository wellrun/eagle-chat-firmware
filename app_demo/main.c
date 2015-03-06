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
	bool msg_recvd;
	uint8_t ctr; // the number of non-null bytes in this message
} cdc_message_t;

volatile cdc_message_t *msg;

void update_message(volatile cdc_message_t * msg);
void update_message(volatile cdc_message_t * msg) {
	iram_size_t nb = udi_cdc_get_nb_received_data();
	if ((msg->ctr + nb - 1) < BUF_SIZE) {
		udi_cdc_read_buf((void *)(msg->buf + msg->ctr), nb); // read into buffer
		msg->ctr += nb; // Update counter
		if (msg->ctr == BUF_SIZE) {
				msg->buf[BUF_SIZE - 1] = '\0';
				msg->ctr = BUF_SIZE - 1;
				msg->msg_recvd = true;
				return;
		} else if (msg->buf[msg->ctr-1] == '\r' || msg->buf[msg->ctr-1] == '\n') {
			msg->buf[msg->ctr-1] = '\0';
			msg->msg_recvd = true;
			return;
		}
	} else {
		iram_size_t num_to_read = BUF_SIZE - msg->ctr - 1; // Number of bytes we can read
		msg->buf[BUF_SIZE - 1] = '\0';
		msg->ctr = BUF_SIZE - 1;
		msg->msg_recvd = true;
		if (num_to_read <= 0) {
			return;
		}
		udi_cdc_read_buf((void *)(msg->buf + msg->ctr), num_to_read);
		return;
	} 
}

void reset_message(volatile cdc_message_t * msg);
void reset_message(volatile cdc_message_t * msg) {
	msg->ctr = 0;
	msg->msg_recvd = false;
	memset((void *)msg->buf, 0, BUF_SIZE);
}

void rx_callback(void);
void rx_callback(void) {
	update_message(msg);
}

void do_read(void);
void do_read() {
	uint8_t read_buf[32];

	nvm_read(INT_EEPROM, (uint32_t) EXAMPLE_ADDR, (void *)read_buf, 32);

	//cdc_log_string("Read back: ", read_buf);
	cdc_log_string("return:success:", read_buf);
}

uint8_t do_write(uint8_t * buf, uint8_t size);
uint8_t do_write(uint8_t * buf, uint8_t size) {
	cdc_log_string("Writing to eeprom: ", buf);

	/* Initialize the non volatile memory */
	if (nvm_init(INT_EEPROM) != STATUS_OK) {
		return ERR_INVALID_ARG;
	}

	/* Write test pattern to the specified address */
	uint8_t status = nvm_write(INT_EEPROM, (uint32_t) EXAMPLE_ADDR, buf, size <= 32 ? size : 32);

	return status;
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
	while (!cdc_opened());
	cdc_set_rx_callback(&rx_callback);
	
	cdc_write_line("Beginning demo.");
	while (1) {
		
		if (local_msg.msg_recvd) {
			uint8_t msg_copy[BUF_SIZE];
			uint8_t msg_count = local_msg.ctr;
			memcpy((void *)msg_copy, (void *)local_msg.buf, local_msg.ctr+1); // make a copy of the msg contents, including null terminator
			reset_message(&local_msg);
			if (msg_copy[0] == 'R') {
				do_read();
			} else if (msg_copy[0] == 'W') {			
				uint8_t status = STATUS_OK;
				status = do_write((uint8_t *)msg_copy+1, msg_count);
				if (status == ERR_INVALID_ARG) {
					cdc_write_line("return:error");
				}
				else if (status == STATUS_OK) {
					cdc_write_line("return:success");
				}
			}
		}
	}

}
