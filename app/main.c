#define F_CPU   32000000L

#include <string.h>

#include <avr/io.h>

#include "asf.h"
#include "cdc.h"
#include "system_timer.h"
#include "host_rx.h"
#include "host_tx.h"
#include "stack/stack.h"


volatile bool quit = false;


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

void processIncomingProtocol(void);
void processIncomingProtocol() {
	// This is where we would parse host messages and take actions accordingly
	// Right now we just echo the messages
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

	system_timer_init();
	system_timer_set_callback(app_step);
	system_timer_start();

	host_rx_init(); // Setup host rx module
	host_tx_init();

	uint16_t msg_count = 0;

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

	while (1);
}
