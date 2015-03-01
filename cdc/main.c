#include "asf.h"
#include "cdc.h"

int main (void)
{
	cpu_irq_enable();

	irq_initialize_vectors();

	sysclk_init();

	cdc_start();

	uint8_t buf[50];

	while (1) {

		cdc_write_string("Enter message: ");
		cdc_read_string(buf, 50);
		cdc_write_string("Message was: ");
		cdc_write_string(buf);
		cdc_newline();
		cdc_log_int("Max length of buffer is: ", 50);

	}

}
