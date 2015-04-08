#define F_CPU   32000000L

#include <string.h>

#include <avr/io.h>

#include "asf.h"
#include "cdc.h"
#include "system_timer.h"
#include "host_rx.h"
#include "host_tx.h"
#include "routing/routing.h"
#include "stack/stack.h"


volatile bool quit = false;
uint8_t my_address = 0;

/*
void app_step(void);

void app_step(void)
{
	static uint32_t start = 0;
	static uint16_t count = 0;

	if (start == 0)
		start = rtc_get_time();
	uint32_t now = rtc_get_time();
	//cdc_log_int("Overflow. Time diff: ", now - start);

	if (count % 20 == 0) {
		if (!host_tx_isFull()) {
			hostMsg_t msg;
			msg.len = 0;
			uint8_t log_string[] = "Overflow. Time diff: ";

			hostMsg_addString(&msg, log_string);
			ltoa(now - start, log_string, 10); // reuse buffer
			hostMsg_addString(&msg, log_string);
			hostMsg_addString(&msg, ". Hope you could read that. Uninitialized memory sucks.\n");
			hostMsg_addString(&msg, "Free stack bytes: ");
			ltoa(stack_count(), log_string, 10);
			hostMsg_addString(&msg, log_string);
			hostMsg_addString(&msg, "\n\n");
			host_tx_queueMessage(&msg);
		}
	}

	start = now;

	count++;
}
*/

void processIncomingProtocol(void);
void processIncomingProtocol() {
	static uint16_t msg_count = 0;
	// This is where we would parse host messages and take actions accordingly
	// Right now we just echo the messages
	if (!host_rx_isEmpty()) {

		msg_count++;

		hostMsg_t * msg = host_rx_peek();

		/*
		hostMsg_t out;
		out.len = 0;

		hostMsg_addString(&out, "msg count: ");
		hostMsg_addInt(&out, msg_count, 10);
		hostMsg_addString(&out, "\ndata ===\n");
		hostMsg_addString(&out, msg->data);
		hostMsg_addString(&out, "\n\n");

		host_rx_skip();

		host_tx_queueMessage(&out);
		*/

		///*
		PacketHeader h;

		h.source = my_address;
		h.dest = my_address == 1 ? 2 : 1;
		h.type = 0;
		//uint8_t message[] = "'Sup guys?";

		cdc_write_line("Got host message.");

		if (queuePacket(h, msg->data, msg->len)) {
			cdc_write_line("Put packet in fifo.");
			host_rx_skip();
		}
		//*/
	}
}

void init(void) {
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
}


int main(void);
int main()
{
	cpu_irq_enable();

	irq_initialize_vectors();

	sysclk_init();

	rtc_init();

	cdc_start();

	while (!cdc_opened());
	rtc_set_time(0);
	cdc_log_int("Starting app ", rtc_get_time());

	/*
	system_timer_init();
	system_timer_set_callback(app_step);
	system_timer_start();
	*/

	init();

	host_rx_init(); // Setup host rx module
	host_tx_init();

	setupRouting(my_address);

	/*
	while(1) {
		if (!host_rx_isEmpty()) {

			msg_count++;

			hostMsg_t * msg = host_rx_peek();

			hostMsg_t out;
			out.len = 0;

			hostMsg_addString(&out, "msg count: ");
			hostMsg_addInt(&out, msg_count, 10);
			hostMsg_addString(&out, "\ndata ===\n");
			hostMsg_addString(&out, msg->data);
			hostMsg_addString(&out, "\n\n");

			host_rx_skip();

			host_tx_queueMessage(&out);
		}
		host_tx_processQueue();
	}
	*/
	while(1) {

		handleReceived();

		processSendQueue();

		host_tx_processQueue();
		processIncomingProtocol();

		if (packetsToRead()) {
			PacketHeader header;
			uint8_t *payload;
			uint8_t length = packetReceivedPeek(&header, &payload);
			cdc_log_int("Packet type: ", header.type);
			cdc_log_string("Received: ", payload);
			packetReceivedSkip();
			cdc_newline();
		}
	}

	while (1);
}
