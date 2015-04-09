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

#include <util/atomic.h>


volatile bool quit = false;
uint8_t my_address = 0;


void do_routing(void);
static volatile bool busy = false;
void do_routing(void) {

	if (busy) {
		return;
	}
	busy = true;
	handleReceived();
	processSendQueue();
	busy = false;
}

void processSendMessage(uint8_t *data);

void processSendMessage(uint8_t *data) {
	// expects :(address):(message string)

	// try to grab the address portion
	uint8_t addr = 0;
	char * token;
	token = strtok(data, ":");
	if (token != NULL)
		addr = (uint8_t) atoi(token);
	if (addr == 0) {
		cdc_write_line("INVALID: NO ADDRESS");
		return; // invalid host message, invalid address
	}

	// grab the message portion
	token = strtok(NULL, ":");
	if (token == NULL) {
		cdc_write_line("INVALID: NO CONTENT");
		return; // invalid host message, no message content
	}

	//cdc_log_int("sending to: ", addr);
	//cdc_log_string("content: ", token);

	// build a packet and queue it up
	PacketHeader h;

	h.source = my_address;
	h.dest = addr;
	h.type = PACKET_TYPE_CONTENT;

	bool success;
	//ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
		queuePacket(h, token, strlen(token));
	//}
}


void processIncomingProtocol(void);
void processIncomingProtocol() {
	static uint16_t msg_count = 0;

	// This is where we would parse host messages and take actions accordingly
	// Right now we just echo the messages
	if (!host_rx_isEmpty()) {

		msg_count++;

		hostMsg_t * msg = host_rx_peek();

		if ((*msg).len > 0) { // check for a valid length message
			uint8_t type = (*msg).data[0];
			switch(type) {
				case 's':
					processSendMessage((*msg).data + 1); // strip the first char
					break;
			}
		}

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

		host_rx_skip();
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
	pmic_init();

	pmic_set_scheduling(PMIC_SCH_ROUND_ROBIN);

	cpu_irq_enable();

	irq_initialize_vectors();

	sysclk_init();

	rtc_init();

	cdc_start();

	while (!cdc_opened());
	rtc_set_time(0);
	cdc_log_int("Starting app ", rtc_get_time());

	init();

	host_rx_init(); // Setup host rx module
	host_tx_init();

	setupRouting(my_address);

	system_timer_init();
	system_timer_set_callback(do_routing);
	system_timer_start();

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
		//cdc_write_line("processQueue");
		host_tx_processQueue();
		//cdc_write_line("processIncomingProtocol");
		processIncomingProtocol();

		if (packetsToRead()) {
			PacketHeader header;
			uint8_t *payload;
			uint8_t length = packetReceivedPeek(&header, &payload);
			cdc_log_int("From: ", header.source);
			cdc_log_string("Content: ", payload);
			packetReceivedSkip();
			cdc_newline();
		}
	}

	while (1);
}
