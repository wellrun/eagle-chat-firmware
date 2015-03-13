#include "asf.h"
#include "cdc.h"
//#include "eeprom.h"
#include "msgbuf.h"
#include <string.h>

int main (void)
{
	cpu_irq_enable();
	irq_initialize_vectors();
	sysclk_init();
	ioport_init();
	sram_init();
	msgbuf_init();
	cdc_start();

	while (!cdc_opened());

	cdc_log_int("Setting sequential mode: ", sram_set_mode(SRAM_MODE_SEQUENTIAL));

	cdc_write_line("Beginning demo.");
	uint8_t buf[128];
	while (1) {
		for (uint8_t i = 0; i < 2; ++i) {
			cdc_write_string("Enter message: ");
			uint8_t len = cdc_read_string(buf, 128);
			cdc_log_int("chars: ", len);
			cdc_log_int("Storing: ", msgbuf_store_message(buf, len+1));
		}

		cdc_newline();

		while(msgbuf_next_length()) {
			memset(buf, 0, 128);
			cdc_log_int("Length of next message: ", msgbuf_next_length());
			cdc_log_int("Reading message: ", msgbuf_read_message(buf));
			cdc_write_line(buf);
		}
		cdc_newline();
	}

}
