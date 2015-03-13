#include "asf.h"
#include "cdc.h"
//#include "eeprom.h"
#include "packetbuf.h"
#include <string.h>

int main (void)
{
	cpu_irq_enable();
	irq_initialize_vectors();
	sysclk_init();
	ioport_init();
	sram_init();
	packetbuf_init();
	cdc_start();

	while (!cdc_opened());

	cdc_log_int("Setting sequential mode: ", sram_set_mode(SRAM_MODE_SEQUENTIAL));

	cdc_write_line("Beginning demo.");
	uint8_t buf[128];
	/*
	for (uint8_t i = 0; i < 2; ++i) {
		cdc_write_string("Enter message: ");
		uint8_t len = cdc_read_string(buf, 128);
		cdc_log_int("chars: ", len);
		cdc_log_int("Storing: ", packetbuf_store_packet(buf, len+1));
	}

	cdc_newline();

	while(packetbuf_has_next()) {
		memset(buf, 0, 128);
		cdc_log_int("Has next message: ", packetbuf_has_next());
		cdc_log_int("Reading message: ", packetbuf_read_packet(buf));
		cdc_write_line(buf);
	}
	cdc_newline();
	*/

	cdc_write_line("Stress test.");
	uint8_t len = 128;
	memset(buf, 0, len);
	for (uint8_t i = 0; i < 65; ++i) {
		memset(buf, i + 48, 10);
		udi_cdc_putc(i+48);
		cdc_log_int("Writing: ", packetbuf_store_packet(buf, len));
	}
	cdc_write_line("Tried to write 65 packets.");

	while(packetbuf_has_next()) {
		memset(buf, 0, 128);
		//cdc_log_int("Has next message: ", packetbuf_has_next());
		packetbuf_read_packet(buf);
		cdc_write_line(buf);
	}

	while(1);
}
