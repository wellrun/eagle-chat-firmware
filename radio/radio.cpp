#include "radio.h"

extern "C" {
	#include "asf.h"
	#include <avr/io.h>
	#include "cdc.h"
	#include "fifo.h"
}
#include <string.h>

#include "RFM69.h"


RFM69 radio;

void setupRadio() {
	
	radio = RFM69();

	cdc_log_int("About to intialize module ", rtc_get_time());
	radio.initialize(FREQUENCY,NODEID,NETWORKID);

	cdc_log_int("Initialized: ", rtc_get_time());

	radio.setHighPower();
	radio.setPowerLevel(31);
	radio.promiscuous(true);
}

void broadcastPacket(uint8_t *packet, uint8_t size) {

	radio.send(RF69_BROADCAST_ADDR, (uint8_t *)packet, (uint8_t)size, 0);

}

bool packetsToRead() {
	radio.receiveDone();
	return !fifo_isEmpty(&radio.RXFIFO);
}

void getNextPacket(uint8_t *senderId, uint8_t *length, uint8_t *buf) {

	uint8_t *fbuf = fifo_read(&radio.RXFIFO);
	*senderId = fbuf[0];
	*length = fbuf[1];
	memcpy(buf, &fbuf[2], *length);

}