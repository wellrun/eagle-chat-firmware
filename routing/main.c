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
	uint8_t bullshitbuf[4];

	while(1) {
		cdc_write_string("My address (1-3): ");
		cdc_read_string(bullshitbuf, 3);
		my_address = (uint8_t) atoi((const char *)bullshitbuf);
		cdc_newline();

		if (my_address >= 1 && my_address <=3) {
			cdc_log_int("You said: ", my_address);
			break;
		}

		cdc_write_line("Invalid address.");
	}

	setupRouting(my_address); // configure router to be node one

	uint32_t then = rtc_get_time();

	bool previouslyFailed = false;
	bool routeRequestResolved = false;


	while (1) {
		handleReceived();

		if (my_address == 1) {

			if (rtc_get_time() - then > 1000) {

				PacketHeader h;
				h.source = my_address;
				h.dest = my_address == 1 ? 2 : 1;
				h.flags = 0;
				h.type = 0;
				uint8_t message[] = "'Sup guys?";

				if (!routeRequestInProgress()) {
					if (routeExistsTo(h.dest)) {
						sendPacket(&h, message, sizeof(message));
					} else {
						initiateRouteRequest(h.dest);
					}
				}
				
				then = rtc_get_time();
			}
		}
	}

}
