#include <string.h>

#include "asf.h"
#include "cdc.h"
#include "routing.h"
#include "radio.h"

void stuff_fifo(uint8_t my_address);
void stuff_fifo(uint8_t my_address)
{
	PacketHeader h;

	h.source = my_address;
	h.dest = my_address == 1 ? 2 : 1;
	h.type = 0;
	uint8_t message[] = "'Sup guys?";

	if (queuePacket(h, message, sizeof(message))) {
		cdc_write_line("Put packet in fifo.");
	}
}

int main(void)
{
	cpu_irq_enable();

	irq_initialize_vectors();

	sysclk_init();

	cdc_start();

	while (!cdc_opened());

	cdc_write_line("Routing demo.");

	rtc_init();

	uint8_t my_address = 1;
	uint8_t bullshitbuf[4];

	while (1) {
		cdc_write_string("My address (1-3): ");
		cdc_read_string(bullshitbuf, 3);
		my_address = (uint8_t)atoi((const char *)bullshitbuf);
		cdc_newline();

		if (my_address >= 1 && my_address <= 3) {
			cdc_log_int("You said: ", my_address);
			break;
		}

		cdc_write_line("Invalid address.");
	}

	setupRouting(my_address); // configure router to be node one

	uint32_t then = rtc_get_time();

	while (1) {
		handleReceived();

		processSendQueue();

		if (packetsToRead()) {
			PacketHeader header;
			uint8_t *payload;
			uint8_t length = packetReceivedPeek(&header, &payload);
			cdc_log_int("Packet type: ", header.type);
			cdc_log_string("Received: ", payload);
			packetReceivedSkip();
			cdc_newline();
		}

		if (my_address == 1) {

			if (rtc_get_time() - then > 1000) {
				stuff_fifo(1);
				then = rtc_get_time();
			}
		}
	}
}
