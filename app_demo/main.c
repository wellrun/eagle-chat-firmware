#include "asf.h"
#include "cdc.h"

#define BUF_SIZE (1 << 6)

uint8_t buf[BUF_SIZE];
bool msg_recvd = false;
uint8_t ctr = 0;

void rx_callback(void) {
	uint8_t c = udi_cdc_getc();
	if (ctr < BUF_SIZE) {
		if (c == '\r' || c == '\n' ) {
			buf[ctr] = '\0';
			msg_recvd = true;
		} else {
			buf[ctr] = c;
			++ctr;
		}
	}
}

int main (void)
{
	cpu_irq_enable();

	irq_initialize_vectors();

	sysclk_init();

	cdc_start();

	//while(udi_cdc_getc() == 0);

	cdc_set_rx_callback(&rx_callback);

	while (1) {
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
