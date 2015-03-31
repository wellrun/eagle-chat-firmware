#define F_CPU	32000000L

#include "asf.h"
#include <avr/io.h>
#include "cdc.h"
#include <util/delay.h>

#include <string.h>

#include <radio.h>
#include <fifo.h>


uint32_t TRANSMITPERIOD = 150; //transmit a packet to gateway so often (in ms)
uint8_t payload[250];
uint8_t buff[20];
uint8_t sendSize=0;
uint8_t requestACK = true;

/* Dummy routing tables for this demo */

/*

	{1} <--> {2} <--> {3}

*/

uint8_t route_table_node_1[3] = {
	0,
	2,
	2,
};

uint8_t route_table_node_2[3] = {
	1,
	0,
	3,
};

uint8_t route_table_node_3[3] = {
	2,
	2,
	0,
};

uint8_t senderId;
uint8_t length;
uint8_t rbuf[256];
bool 	needsAck;

uint8_t dest;
bool    acked;

uint32_t start_time;
uint32_t send_time;

uint16_t failures = 0;



// Tries to send a packet using the routing table
void send_mode() {
	dest = 2;

	uint8_t first_hop = route_table_node_1[dest-1];

	while (1) {
		payload[0] = dest;
		payload[1] = 1; // my address

		memset(payload + 2, 'A', sizeof(payload) - 3);
		payload[sizeof(payload)-1] = 0;

		start_time = rtc_get_time();

		sendPacket(first_hop, payload, sizeof(payload));
		send_time = rtc_get_time();
		acked = false;
		while (rtc_get_time() - send_time < 50) {
			if ((acked = ackReceived(dest)))
				break;
		}
		if (!acked) {
			++failures;
			//cdc_write_line("Got no ack, bro.");
			cdc_write_line("Failed");
		} else {
			cdc_log_int("Received ack: ", rtc_get_time() - send_time);
			cdc_write_line("Waiting for reply.");

			send_time = rtc_get_time();
			while (rtc_get_time() - send_time < 2000) {
				if (packetsToRead()) {
					getNextPacket(&senderId, &length, rbuf, &needsAck);
					cdc_log_string("Got: ", rbuf);
					if (needsAck) {
						sendAck(senderId);
					}
					if (length >= 2 && rbuf[0] == 1 && rbuf[1] == dest) { // This may be our packet
						cdc_log_string("Got: ", rbuf);
						cdc_log_int("Waited for: ", rtc_get_time() - send_time);
						break;
					}
				}
			}
		}
		cdc_log_int("Round trip: ", rtc_get_time() - start_time);
		cdc_log_int("Failures: ", failures);
		_delay_ms(500);
	}
}



void router_mode_2() {
	while (1) {
		if (packetsToRead()) {
			getNextPacket(&senderId, &length, rbuf, &needsAck);
			if (needsAck) {
				sendAck(senderId);
			}
			if (length >= 2) { // Try to route packet
				dest = rbuf[0];
				if (dest >= 1 && dest <= 3) {
					cdc_log_string("Got: ", rbuf);
					if (dest == 2) {
						rbuf[0] = 1;
						rbuf[1] = 2;
						dest = 1;
					}
					uint8_t next_hop = route_table_node_2[dest-1];

					sendPacket(next_hop, rbuf, length);
					send_time = rtc_get_time();
					acked = false;
					while (rtc_get_time() - send_time < 50) {
						if ((acked = ackReceived(dest)))
							break;
					}
					if (!acked) {
						++failures;
						//cdc_write_line("Got no ack, bro.");
						cdc_write_line("Failed to ack. Lost packet?");
					} else {
						cdc_write_line("Sent packet on.");
					}
				}
			}
		}
	}
}



void router_mode_3() {
	while (1) {
		if (packetsToRead()) {
			getNextPacket(&senderId, &length, rbuf, &needsAck);
			if (needsAck) {
				sendAck(senderId);
			}
			if (length >= 2) { // Try to route packet
				dest = rbuf[0];

				if (dest >= 1 && dest <= 3) {
					cdc_log_string("Got: ", rbuf);
					if (dest == 3) {
						rbuf[0] = 1;
						rbuf[1] = 3;
						dest = 1;
					}

					uint8_t next_hop = route_table_node_3[dest - 1];
					cdc_log_int("Next hop: ", next_hop);

					sendPacket(next_hop, rbuf, length);
					send_time = rtc_get_time();
					acked = false;
					while (rtc_get_time() - send_time < 50) {
						if ((acked = ackReceived(dest)))
							break;
					}
					if (!acked) {
						++failures;
						//cdc_write_line("Got no ack, bro.");
						cdc_write_line("Failed to ack. Lost packet?");
					} else {
						cdc_write_line("Sent packet on.");
					}
				}
			}
		}
	}
}

int main(void);
int main ()
{
	cpu_irq_enable();

	irq_initialize_vectors();

	sysclk_init();

	ioport_init();

	rtc_init();

	cdc_start();

	while (!cdc_opened());
	rtc_set_time(0);
	cdc_log_int("About to instantiate module ", rtc_get_time());

	setupRadio();

	uint8_t address = 0;

	while(1) {
		cdc_write_string("My address (1-3): ");
		cdc_read_string(payload, 3);
		address = (uint8_t) atoi((const char *)payload);
		cdc_newline();

		if (address >= 1 && address <=3) {
			cdc_log_int("You said: ", address);
			break;
		}

		cdc_write_line("Invalid address.");
	}

	setAddress(address);

	cdc_write_line("Press any key to continue.");
	while (udi_cdc_getc() == 0);

	if (address == 1) {
		send_mode();
	} else if (address == 2) {
		router_mode_2();
	} else if (address == 3) {
		router_mode_3();
	}

	while (1);
}
