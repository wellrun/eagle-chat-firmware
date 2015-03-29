#define F_CPU	32000000L

#include "asf.h"
#include <avr/io.h>
#include "cdc.h"
#include <util/delay.h>

#include <string.h>

#include "radio.h"
#include "fifo.h"


uint32_t TRANSMITPERIOD = 150; //transmit a packet to gateway so often (in ms)
uint8_t payload[250];
uint8_t buff[20];
uint8_t sendSize=0;
uint8_t requestACK = true;

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

	uint8_t mode = 0;
	while (mode != 'S' && mode != 'R') {
		cdc_write_string("Enter S for send or R for receive, or F for fifo test: ");
		mode = udi_cdc_getc();
		//mode = 'R';
	}

	if (mode == 'S') {
		uint32_t start_time;
		uint8_t count = 0;
		while (1) {

			start_time = rtc_get_time();
			memset(payload, 'A', sizeof(payload));
			payload[sizeof(payload)-1] = 0;
			for (count = 0; count < 0xFF; ++count) {

				while (udi_cdc_getc() == 0);

				cdc_log_int("Send attempt: ", count);
				//memset(payload, 0, 5);
				//itoa(count, (char *)payload, 10);

				broadcastPacket(payload, sizeof(payload));
				_delay_ms(50);
			}

			cdc_log_int("Time to send 255 packets: ", rtc_get_time() - start_time);
			_delay_ms(3000);
		}

	} else if (mode == 'R') {
		//cdc_write_string("\nRead mode.");
		uint8_t senderId;
		uint8_t length;
		uint8_t rbuf[256];

		uint32_t p = 0;
		uint16_t now = rtc_get_time();
		while(1) {
			if (rtc_get_time() - now > 1000) {
				now = rtc_get_time();
				//cdc_write_line("MAIN LOOP");
			}
			while (packetsToRead()) {
				cdc_log_int("Packets: ", ++p);
				getNextPacket(&senderId, &length, rbuf);
			}

		}

    } else if (mode == 'F') {

    	// Play around with FIFO

    	fifo_t F;

    	fifo_init(&F);

    	fifo_write(&F, payload, 30);
    	fifo_write(&F, payload, 30);

    	uint8_t buf[FIFO_UNIT_LEN];

    	fifo_read(&F);

    }

	while (1);
}
