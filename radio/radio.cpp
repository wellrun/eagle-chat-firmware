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
	radio.promiscuous(false);
}

void setAddress(uint8_t address) {
	radio.setAddress(address);
}

void broadcastFrame(uint8_t *packet, uint8_t size) {

	radio.send(RF69_BROADCAST_ADDR, (uint8_t *)packet, (uint8_t)size, 0);

}

void sendFrame(uint8_t address, uint8_t *packet, uint8_t size) {

	radio.send(address, (uint8_t *)packet, (uint8_t)size, true);

}

void sendAck(uint8_t address) {
	radio.simpleSendACK(address);
}

bool ackReceived(uint8_t address) {
	return radio.ACKReceived(address);
}

bool framesToRead() {
	radio.receiveDone();
	return !fifo_isEmpty(&radio.RXFIFO);
}

void getNextFrame(uint8_t *senderId, uint8_t *destId, uint8_t *length, uint8_t *buf, bool *needsAck) {

	uint8_t *fbuf = fifo_read(&radio.RXFIFO);
	*senderId = fbuf[OFFSET_SENDER_ADDRESS];
	*destId = fbuf[OFFSET_TARGET_ADDRESS];
	*length = fbuf[OFFSET_LENGTH];
	*needsAck = fbuf[OFFSET_CTL] & MASK_ACK_REQUESTED;
	memcpy(buf, &fbuf[OFFSET_DATA], *length);

}
