#include "asf.h"
#include "cdc.h"

int main (void)
{
	cpu_irq_enable();

	irq_initialize_vectors();

	sysclk_init();

	cdc_start();

	uint8_t buf[50];
	while(udi_cdc_getc() == 0);
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

		cdc_read_string(buf, 50);

		delay_s(5);

		cdc_write_string(buf);

		//while (1) {
		//	udi_cdc_getc();
		//}
	}

}
