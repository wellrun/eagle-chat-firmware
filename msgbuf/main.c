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
	uint16_t addr = 0;
	while (addr < 64) {
		sram_write_byte(addr, (uint8_t)addr & 0xFF);
		++addr;
	}

	for (uint16_t i = 0; i < 32; ++i) {
		uint8_t byte = sram_read_byte(i);
		//if (byte > 32) {
			udi_cdc_putc(byte);
		//}
	}
	/*
	cdc_newline();
	for (uint16_t i = 0; i < MSGBUF_PAGE_SIZE * 3; ++i) {
		uint8_t byte = sram_read_byte(i);
		//if (byte > 32) {
			udi_cdc_putc(byte);
		//}
	}
	cdc_newline();
	for (uint16_t i = 0; i < MSGBUF_PAGE_SIZE * 3; ++i) {
		uint8_t byte = sram_read_byte(i);
		//if (byte > 32) {
			udi_cdc_putc(byte);
		//}
	}
	*/
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

		memset(buf, 0, 128);
		cdc_write_line("Emptied buffer:");
		for (uint16_t j = 0; j < 128; ++j) {
			uint8_t byte = buf[j];
			udi_cdc_putc(byte);
		}
		sram_read_packet(0, buf, 128);
		//cdc_write_line(buf);
		cdc_newline();
		cdc_write_line("Memory contents: ");
		for (uint16_t k = 0; k < 128; ++k) {
			uint8_t byte = buf[k];
			udi_cdc_putc(byte);
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
