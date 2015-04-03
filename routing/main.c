#include <string.h>

#include "asf.h"
#include "cdc.h"
#include "routing.h"
#include "radio.h"


int main (void)
{
	cpu_irq_enable();

	irq_initialize_vectors();

	sysclk_init();

	cdc_start();

	while (!cdc_opened());

	cdc_write_line("Routing demo.");

	rtc_init();

	uint8_t my_address = 1;

	setupRouting(my_address); // configure router to be node one

	uint32_t then = rtc_get_time();

	while (1) {
		handleReceived();

		if (rtc_get_time() - then > 1000) {
			PacketHeader h;
			h.source = my_address;
			h.dest = my_address == 1 ? 2 : 1;
			h.flags = 0;
			uint8_t message[] = "'Sup guys?";
			sendPacket(&h, message, sizeof message);
			then = rtc_get_time();
		}
	}

}
